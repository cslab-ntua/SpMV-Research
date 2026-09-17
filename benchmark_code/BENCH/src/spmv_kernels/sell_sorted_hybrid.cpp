#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

#include "macros/cpp_defines.h"

#include "spmv_kernel.h"

#ifdef __cplusplus
extern "C"{
#endif
	#include "macros/macrolib.h"
	#include "time_it.h"
	#include "parallel_util.h"
	#include "array_metrics.h"

	// #define VEC_FORCE

	// #define VEC_X86_512
	// #define VEC_X86_256
	// #define VEC_X86_128
	// #define VEC_ARM_SVE

	#if DOUBLE == 0
		#define VTI   i32
		#define VTF   f32
		// #define VEC_LEN  1
		#define VEC_LEN  vec_len_default_f32
	#elif DOUBLE == 1
		#define VTI   i64
		#define VTF   f64
		// #define VEC_LEN  1
		#define VEC_LEN  vec_len_default_f64
	#endif

	// #include "vectorization.h"
	#include "vectorization/vectorization_gen.h"

	long
	reduce_add_long(long a, long b)
	{
		return a + b;
	}

	#include "functools/functools_gen_undef.h"
	#define FUNCTOOLS_GEN_TYPE_1  INT_T
	#define FUNCTOOLS_GEN_TYPE_2  INT_T
	#define FUNCTOOLS_GEN_SUFFIX  _i_i
	#include "functools/functools_gen.c"
	static inline
	INT_T
	functools_map_fun(INT_T * A, long i)
	{
		return A[i];
	}
	static inline
	INT_T
	functools_reduce_fun(INT_T a, INT_T b)
	{
		return a + b;
	}

	#include "sort/bucketsort/bucketsort_gen_undef.h"
	#define BUCKETSORT_GEN_TYPE_1  INT_T
	#define BUCKETSORT_GEN_TYPE_2  INT_T
	#define BUCKETSORT_GEN_TYPE_3  INT_T
	#define BUCKETSORT_GEN_TYPE_4  INT_T
	#define BUCKETSORT_GEN_SUFFIX  _degree
	#include "sort/bucketsort/bucketsort_gen.c"
	static inline
	int
	bucketsort_find_bucket(INT_T * A, long i, __attribute__((unused)) INT_T * degree_max_ptr)
	{
		return A[i+1] - A[i];   // Ascending order.
		// return *degree_max_ptr - (A[i+1] - A[i]);   // Descending order.
	}

	#include "sort/quicksort/quicksort_gen_undef.h"
	#define QUICKSORT_GEN_TYPE_1  INT_T
	#define QUICKSORT_GEN_TYPE_2  INT_T
	#define QUICKSORT_GEN_TYPE_3  void
	#define QUICKSORT_GEN_FUNCTION_ATTRIBUTES
	#define QUICKSORT_GEN_SUFFIX  _index
	#include "sort/quicksort/quicksort_gen.c"
	static inline
	int
	quicksort_cmp(INT_T a, INT_T b, __attribute__((unused)) void * unused)
	{
		return (a > b) ? 1 : (a < b) ? -1 : 0;
	}

#ifdef __cplusplus
}
#endif


struct thread_data {
	long crossover_row;
	long i_s;
	long i_e;

	long crossover_row_cluster;
	long num_row_clusters_private;
	long ii_s;
	long ii_e;
};

static struct thread_data ** tds;


template<typename T>
void
transpose(T * A, INT_T m, INT_T n)
{
	T * buf = (typeof(buf)) aligned_alloc(64, m*n * sizeof(*buf));
	long i, j;
	for (j=0;j<n;j++)
	{
		for (i=0;i<m;i++)
			buf[j*m + i] = A[i*n + j];
	}
	for (i=0;i<m*n;i++)
		A[i] = buf[i];
	free(buf);
}


inline
int
row_is_above_crossover(INT_T degree)
{
	INT_T crossover_degree = 50;
	int ret = 0;
	if (degree >= crossover_degree)
		ret = 1;
	return ret;
}


struct SELLCSRArray : Matrix_Format
{
	ValueType * a;
	long num_row_clusters;
	INT_T * row_cluster_ptr;   // Contains both row clusters for SELL (VEC_LEN rows) and single rows for CSR.
	INT_T * ja;

	INT_T * permutation;
	INT_T * rev_permutation;

	long nnz_ext;

	SELLCSRArray(INT_T * row_ptr, INT_T * col_ind, ValueTypeReference * values, long m, long n, long nnz) : Matrix_Format(m, n, nnz)
	{
		long num_threads = omp_get_max_threads();

		tds = (typeof(tds)) aligned_alloc(64, num_threads * sizeof(*tds));

		printf("VEC_LEN = %d\n", VEC_LEN);

		num_row_clusters = 0;

		permutation = (typeof(permutation)) aligned_alloc(64, m * sizeof(*permutation));
		rev_permutation = (typeof(rev_permutation)) aligned_alloc(64, m * sizeof(*rev_permutation));

		INT_T * row_ptr_reordered = (typeof(row_ptr_reordered)) aligned_alloc(64, (m+1) * sizeof(*row_ptr_reordered));
		INT_T * col_ind_reordered = (typeof(col_ind_reordered)) aligned_alloc(64, nnz * sizeof(*col_ind_reordered));
		ValueType * values_reordered = (typeof(values_reordered)) aligned_alloc(64, nnz * sizeof(*values_reordered));

		#pragma omp parallel
		{
			long tnum = omp_get_thread_num();
			struct thread_data * td;
			long i, ii_s, ii_e, i_s, i_e, j, k;
			long crossover_row;
			long num_row_clusters_private;

			td = (typeof(td)) aligned_alloc(64, sizeof(*td));
			tds[tnum] = td;

			loop_partitioner_balance_prefix_sums(num_threads, tnum, row_ptr, m, nnz, &i_s, &i_e);
			if (tnum == num_threads - 1)
				i_e = m;
			td->i_s = i_s;
			td->i_e = i_e;

			long num_rows_below = 0;
			INT_T degree, degree_max = 0;
			for (i=i_s;i<i_e;i++)
			{
				degree = row_ptr[i+1] - row_ptr[i];
				if (degree > degree_max)
					degree_max = degree;
				if (!row_is_above_crossover(degree))
					num_rows_below++;
			}
			num_rows_below = num_rows_below - num_rows_below % VEC_LEN;
			crossover_row = i_s + num_rows_below;
			td->crossover_row = crossover_row;

			long num_row_clusters_sell = (crossover_row - i_s) / VEC_LEN;
			long num_rows_csr = i_e - crossover_row;
			num_row_clusters_private = num_row_clusters_sell + num_rows_csr;
			td->num_row_clusters_private = num_row_clusters_private;
			omp_thread_reduce_global(reduce_add_long, num_row_clusters_private, 0, 1, 0, &ii_s, &num_row_clusters); // omp_thread_reduce_global(_reduce_fun, _partial, _zero, exclusive, _backwards, _local_result_ptr_ret, _total_result_ptr_ret);
			ii_e = ii_s + num_row_clusters_private;
			td->ii_s = ii_s;
			td->ii_e = ii_e;
			td->crossover_row_cluster = ii_s + num_row_clusters_sell;

			#pragma omp barrier

			#pragma omp single
			{
				row_cluster_ptr = (typeof(row_cluster_ptr)) aligned_alloc(64, (num_row_clusters+1) * sizeof(*row_cluster_ptr));
			}

			/* Sort by row size. */
			// void bucketsort_stable_recalculate_bucket_serial(_TYPE_V * restrict A, long N, _TYPE_BUCKET_I num_buckets, _TYPE_AD * restrict aux_data, _TYPE_I * restrict permutation_out, _TYPE_I * restrict offsets_out);
			bucketsort_stable_recalculate_bucket_serial(&row_ptr[i_s], i_e-i_s, degree_max+1, &degree_max, &permutation[i_s], NULL);

			for (i=i_s;i<i_e;i++)
			{
				permutation[i] += i_s;
				rev_permutation[permutation[i]] = i;
			}

			/* Restore order in CSR part, sort by row index. */
			quicksort(&rev_permutation[crossover_row], i_e - crossover_row, NULL, NULL);

			for (i=i_s;i<i_e;i++)
			{
				permutation[rev_permutation[i]] = i;
			}


			for (i=i_s;i<i_e;i++)
			{
				row_ptr_reordered[permutation[i]] = row_ptr[i+1] - row_ptr[i];
			}
			#pragma omp single
			{
				row_ptr_reordered[m] = 0;
			}
			scan_reduce_concurrent(row_ptr_reordered, row_ptr_reordered, m+1, 0, 1, 0);

			#pragma omp barrier

			k = row_ptr[i_s];
			for (i=i_s;i<i_e;i++)
			{
				for (j=row_ptr[rev_permutation[i]];j<row_ptr[rev_permutation[i]+1];j++,k++)
				{
					col_ind_reordered[k] = col_ind[j];
					values_reordered[k] = values[j];
				}
			}
		}

		/* Extend SELL row clusters to local max row. 
		 * Transpose row clusters.
		 * Extend CSR rows to multiples of 'VEC_LEN'.
		 */
		#pragma omp parallel
		{
			long tnum = omp_get_thread_num();
			struct thread_data * td = tds[tnum];
			long i, ii, j, jj, k;
			long ii_s, ii_e;
			long i_s, i_e;
			long crossover_row, crossover_row_cluster;
			long degree;
			long col = 0;
			long width;

			ii_s = td->ii_s;
			ii_e = td->ii_e;
			i_s = td->i_s;
			i_e = td->i_e;
			crossover_row = td->crossover_row;
			crossover_row_cluster = td->crossover_row_cluster;

			for (i=i_s,ii=ii_s;i<crossover_row;i+=VEC_LEN,ii++)
			{
				width = 0;
				for (k=i;k<i+VEC_LEN;k++)
				{
					degree = row_ptr_reordered[k+1] - row_ptr_reordered[k];
					if (degree > width)
						width = degree;
				}
				row_cluster_ptr[ii] = VEC_LEN * width;
			}
			for (i=crossover_row,ii=crossover_row_cluster;i<i_e;i++,ii++)
			{
				degree = row_ptr_reordered[i+1] - row_ptr_reordered[i];
				degree = ((degree + VEC_LEN - 1) / VEC_LEN) * VEC_LEN;
				row_cluster_ptr[ii] = degree;
			}
			#pragma omp single
			{
				row_cluster_ptr[num_row_clusters] = 0;
			}
			scan_reduce_concurrent(row_cluster_ptr, row_cluster_ptr, num_row_clusters+1, 0, 1, 0);   // scan_reduce_concurrent(_TYPE_IN * A, _TYPE_OUT * P, long N, _TYPE_OUT zero, const int exclusive, const int backwards);

			#pragma omp barrier

			#pragma omp single
			{
				nnz_ext = row_cluster_ptr[num_row_clusters];
				a = (typeof(a)) aligned_alloc(64, nnz_ext * sizeof(*a));
				ja = (typeof(ja)) aligned_alloc(64, nnz_ext * sizeof(*ja));
			}

			col = 0;
			for (i=i_s,ii=ii_s;i<crossover_row;i+=VEC_LEN,ii++)
			{
				width = (row_cluster_ptr[ii+1] - row_cluster_ptr[ii]) / VEC_LEN;
				jj = row_cluster_ptr[ii];
				for (k=i;k<i+VEC_LEN;k++)
				{
					for (j=row_ptr_reordered[k];j<row_ptr_reordered[k+1];j++,jj++)
					{
						a[jj] = values_reordered[j];
						col = col_ind_reordered[j];
						ja[jj] = col;
					}
					for (;j<row_ptr_reordered[k]+width;j++,jj++)   // Padding of smaller rows.
					{
						a[jj] = 0;
						ja[jj] = col;
					}
				}
				transpose(&a[row_cluster_ptr[ii]], VEC_LEN, width);
				transpose(&ja[row_cluster_ptr[ii]], VEC_LEN, width);
			}
			for (i=crossover_row,ii=crossover_row_cluster;i<i_e;i++,ii++)
			{
				jj = row_cluster_ptr[ii];
				for (j=row_ptr_reordered[i];j<row_ptr_reordered[i+1];j++,jj++)
				{
					a[jj] = values_reordered[j];
					col = col_ind_reordered[j];
					ja[jj] = col;
				}
				for (;jj<row_cluster_ptr[ii+1];jj++)   // Padding of smaller rows.
				{
					a[jj] = 0;
					ja[jj] = col;
				}
			}

			printf("%2ld: i_s=%10ld, crossover_row=%10ld, i_e=%10ld, nnz=%10d, rows=%10ld, sell_rows=%10ld, csr_rows=%10ld\n", tnum, i_s, crossover_row, i_e, row_cluster_ptr[ii_e]-row_cluster_ptr[ii_s], i_e-i_s, crossover_row-i_s, i_e-crossover_row);
		}

		mem_footprint = (num_row_clusters+1) * sizeof(INT_T) + nnz_ext * (sizeof(ValueType) + sizeof(INT_T));
	}

	~SELLCSRArray()
	{
		free(a);
		free(ja);
	}

	void spmv(ValueType * x, ValueType * y);
	void statistics_start();
	int statistics_print_data(__attribute__((unused)) char * buf, __attribute__((unused)) long buf_n);
};


void compute_sell_csr_hybrid(SELLCSRArray * sell, ValueType * x , ValueType * y);


void
SELLCSRArray::spmv(ValueType * x, ValueType * y)
{
	compute_sell_csr_hybrid(this, x, y);
}


struct Matrix_Format *
csr_to_format(INT_T * row_ptr, INT_T * col_ind, ValueTypeReference * values, long m, long n, long nnz, long symmetric, long symmetry_expanded)
{
	if (symmetric && !symmetry_expanded)
		error("symmetric matrices have to be expanded to be supported by this format");
	struct SELLCSRArray * sell = new SELLCSRArray(row_ptr, col_ind, values, m, n, nnz);
	sell->format_name = (char *) "SELL_SORTED";
	return sell;
}


//==========================================================================================================================================
//= SELLPACK
//==========================================================================================================================================


void
compute_sell_csr_hybrid(SELLCSRArray * sell, ValueType * x , ValueType * y)
{
	#pragma omp parallel
	{
		long tnum = omp_get_thread_num();
		struct thread_data * td = tds[tnum];
		vec_t(VTF, VEC_LEN) zero = vec_set1(VTF, VEC_LEN, 0);
		__attribute__((unused)) vec_t(VTF, VEC_LEN) v_a = zero, v_x = zero, v_sum = zero;
		__attribute__((unused)) vec_t(i32, VEC_LEN) v_col;
		ValueType sum;
		long ii, ii_s, ii_e, jj, jj_s, jj_e;
		long i, k;
		long i_s, i_e;
		long crossover_row, crossover_row_cluster;
		i_s = td->i_s;
		i_e = td->i_e;
		ii_s = td->ii_s;
		ii_e = td->ii_e;
		crossover_row = td->crossover_row;
		crossover_row_cluster = td->crossover_row_cluster;

		long ii_e_last = ii_e;
		if (ii_e * VEC_LEN > i_e)
			ii_e_last -= VEC_LEN;

		for (i=i_s,ii=ii_s;i<crossover_row;i+=VEC_LEN,ii++)
		{
			v_sum = vec_set1(VTF, VEC_LEN, 0);
			jj_s = sell->row_cluster_ptr[ii];
			jj_e = sell->row_cluster_ptr[ii+1];
			for (jj=jj_s;jj<jj_e;jj+=VEC_LEN)
			{
				v_a = vec_loadu(VTF, VEC_LEN, &sell->a[jj]);
				// v_x = vec_set_iter(VTF, VEC_LEN, iter, x[sell->ja[jj+iter]]);
				v_col = vec_loadu(i32, VEC_LEN, &sell->ja[jj]);
				v_x = vec_gather(VTF, i32, VEC_LEN, x, v_col);
				v_sum = vec_fmadd(VTF, VEC_LEN, v_a, v_x, v_sum);
			}
			// for (k=0;k<VEC_LEN;k++)
				// y[sell->rev_permutation[i+k]] = vec_array(VTF, VEC_LEN, v_sum)[k];
			vec_storeu(VTF, VEC_LEN, &y[i], v_sum);

		}

		for (i=crossover_row,ii=crossover_row_cluster;i<i_e;i++,ii++)
		{
			v_sum = vec_set1(VTF, VEC_LEN, 0);
			jj_s = sell->row_cluster_ptr[ii];
			jj_e = sell->row_cluster_ptr[ii+1];
			for (jj=jj_s;jj<jj_e;jj+=VEC_LEN)
			{
				v_a = vec_loadu(VTF, VEC_LEN, &sell->a[jj]);
				// v_x = vec_set_iter(VTF, VEC_LEN, iter, x[sell->ja[jj+iter]]);
				v_col = vec_loadu(i32, VEC_LEN, &sell->ja[jj]);
				v_x = vec_gather(VTF, i32, VEC_LEN, x, v_col);
				v_sum = vec_fmadd(VTF, VEC_LEN, v_a, v_x, v_sum);
			}
			sum = vec_reduce_add(VTF, VEC_LEN, v_sum);
			// y[sell->rev_permutation[i]] = sum;
			y[i] = sum;
		}

	}
}


//==========================================================================================================================================
//= Print Statistics
//==========================================================================================================================================


void
SELLCSRArray::statistics_start()
{
}


int
statistics_print_labels(__attribute__((unused)) char * buf, __attribute__((unused)) long buf_n)
{
	return 0;
}


int
SELLCSRArray::statistics_print_data(__attribute__((unused)) char * buf, __attribute__((unused)) long buf_n)
{
	return 0;
}

