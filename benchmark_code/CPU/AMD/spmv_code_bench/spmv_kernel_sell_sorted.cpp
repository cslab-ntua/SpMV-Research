#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

#include "macros/cpp_defines.h"

#include "spmv_bench_common.h"
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
		// #define VEC_LEN  vec_len_default_f64
		// #define VEC_LEN  4
		// #define VEC_LEN  8
		// #define VEC_LEN  16
		// #define VEC_LEN  32
	#elif DOUBLE == 1
		#define VTI   i64
		#define VTF   f64
		#define VEC_LEN  vec_len_default_f64
		// #define VEC_LEN  1
	#endif

	// #include "vectorization.h"
	#include "vectorization/vectorization_gen.h"

	#include "functools/functools_gen_undef.h"
	#define FUNCTOOLS_GEN_TYPE_1  int
	#define FUNCTOOLS_GEN_TYPE_2  int
	#define FUNCTOOLS_GEN_SUFFIX  _i_i
	#include "functools/functools_gen.c"
	static inline
	int
	functools_map_fun(int * A, long i)
	{
		return A[i];
	}
	static inline
	int
	functools_reduce_fun(int a, int b)
	{
		return a + b;
	}

	#include "sort/bucketsort/bucketsort_gen_undef.h"
	#define BUCKETSORT_GEN_TYPE_1  int
	#define BUCKETSORT_GEN_TYPE_2  int
	#define BUCKETSORT_GEN_TYPE_3  int
	#define BUCKETSORT_GEN_TYPE_4  void
	#define BUCKETSORT_GEN_SUFFIX  _i_i_i_v
	#include "sort/bucketsort/bucketsort_gen.c"
	static inline
	int
	bucketsort_find_bucket(int * A, long i, [[gnu::unused]] void * unused)
	{
		return A[i+1] - A[i];
	}

#ifdef __cplusplus
}
#endif


struct thread_data {
	long ii_s;
	long ii_e;

	long i_s;
	long i_e;
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


struct SELLArray : Matrix_Format
{
	ValueType * a;
	long num_row_clusters;
	INT_T * row_cluster_ptr;
	INT_T * ja;

	int * permutation;
	int * rev_permutation;
	ValueType * y_buf;

	long m_ext;
	long nnz_ext;

	SELLArray(INT_T * row_ptr, INT_T * col_ind, ValueTypeReference * values, long m, long n, long nnz) : Matrix_Format(m, n, nnz)
	{
		long num_threads = omp_get_max_threads();

		tds = (typeof(tds)) aligned_alloc(64, num_threads * sizeof(*tds));

		num_row_clusters = (m + VEC_LEN - 1) / VEC_LEN;
		m_ext = num_row_clusters * VEC_LEN;

		row_cluster_ptr = (typeof(row_cluster_ptr)) aligned_alloc(64, (num_row_clusters+1) * sizeof(*row_cluster_ptr));

		permutation = (typeof(permutation)) aligned_alloc(64, m * sizeof(*permutation));
		rev_permutation = (typeof(rev_permutation)) aligned_alloc(64, m * sizeof(*rev_permutation));
		y_buf = (typeof(y_buf)) aligned_alloc(64, m_ext * sizeof(*y_buf));

		INT_T * row_ptr_reordered = (typeof(row_ptr_reordered)) aligned_alloc(64, (m+1) * sizeof(*row_ptr_reordered));
		INT_T * col_ind_reordered = (typeof(col_ind_reordered)) aligned_alloc(64, nnz * sizeof(*col_ind_reordered));
		ValueType * values_reordered = (typeof(values_reordered)) aligned_alloc(64, nnz * sizeof(*values_reordered));

		#pragma omp parallel
		{
			long tnum = omp_get_thread_num();
			struct thread_data * td;
			long i, ii_s, ii_e, i_s, i_e, j, k;

			td = (typeof(td)) aligned_alloc(64, sizeof(*td));
			tds[tnum] = td;

			#pragma omp for
			for (i=0;i<num_row_clusters;i++)
			{
				row_cluster_ptr[i] = row_ptr[i * VEC_LEN];
			}
			#pragma omp single
			{
				row_cluster_ptr[num_row_clusters] = row_ptr[m];
			}


			loop_partitioner_balance_prefix_sums(num_threads, tnum, row_cluster_ptr, num_row_clusters, nnz, &ii_s, &ii_e);
			i_s = ii_s * VEC_LEN;
			i_e = ii_e * VEC_LEN;
			if (tnum == num_threads - 1)
				i_e = m;
			td->ii_s = ii_s;
			td->ii_e = ii_e;
			td->i_s = i_s;
			td->i_e = i_e;
			printf("%2ld: %ld %ld (%d), m=%ld\n", tnum, i_s, i_e, row_ptr[i_e] - row_ptr[i_s], m);

			bucketsort_stable_recalculate_bucket_serial(&row_ptr[i_s], i_e-i_s, m, NULL, &permutation[i_s], NULL);
			for (i=i_s;i<i_e;i++)
			{
				permutation[i] += i_s;
				rev_permutation[permutation[i]] = i;
			}

			for (i=i_s;i<i_e;i++)
			{
				row_ptr_reordered[permutation[i]] = row_ptr[i+1] - row_ptr[i];
				// if (tnum == 0)
					// printf("%d\n", row_ptr[i+1] - row_ptr[i]);
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
				// if (tnum == 0)
					// printf("%d\n", row_ptr[rev_permutation[i]+1] - row_ptr[rev_permutation[i]]);
				for (j=row_ptr[rev_permutation[i]];j<row_ptr[rev_permutation[i]+1];j++,k++)
				// for (j=row_ptr[i];j<row_ptr[i+1];j++,k++)
				{
					col_ind_reordered[k] = col_ind[j];
					values_reordered[k] = values[j];
				}
			}
		}

		#pragma omp parallel
		{
			long tnum = omp_get_thread_num();
			struct thread_data * td = tds[tnum];
			long i, ii, j, jj, k, k_s, k_e;
			long ii_s, ii_e;
			long i_s, i_e;
			long degree;
			long col = 0;
			long width;

			ii_s = td->ii_s;
			ii_e = td->ii_e;
			i_s = td->i_s;
			i_e = td->i_e;

			for (i=i_s;i<i_e;i+=VEC_LEN)
			{
				width = 0;
				k_s = i;
				k_e = i + VEC_LEN;
				if (k_e > m)
					k_e = m;
				for (k=k_s;k<k_e;k++)
				{
					degree = row_ptr_reordered[k+1] - row_ptr_reordered[k];
					if (degree > width)
						width = degree;
				}
				row_cluster_ptr[i/VEC_LEN] = VEC_LEN * width;
			}
			#pragma omp single
			{
				row_cluster_ptr[num_row_clusters] = 0;
			}
			scan_reduce_concurrent(row_cluster_ptr, row_cluster_ptr, num_row_clusters+1, 0, 1, 0);
			// scan_reduce_concurrent(_TYPE_IN * A, _TYPE_OUT * P, long N, _TYPE_OUT zero, const int exclusive, const int backwards);

			#pragma omp barrier

			#pragma omp single
			{
				nnz_ext = row_cluster_ptr[num_row_clusters];
				a = (typeof(a)) aligned_alloc(64, nnz_ext * sizeof(*a));
				ja = (typeof(ja)) aligned_alloc(64, nnz_ext * sizeof(*ja));
			}

			col = 0;
			for (ii=ii_s;ii<ii_e;ii++)
			{
				width = (row_cluster_ptr[ii+1] - row_cluster_ptr[ii]) / VEC_LEN;
				long i_c_s = VEC_LEN * ii;
				long i_c_e = i_c_s + VEC_LEN;
				if (i_c_e > m)
					i_c_e = m;
				jj = row_cluster_ptr[ii];
				for (i=i_c_s;i<i_c_e;i++)
				{
					for (j=row_ptr_reordered[i];j<row_ptr_reordered[i+1];j++,jj++)
					{
						a[jj] = values_reordered[j];
						col = col_ind_reordered[j];
						ja[jj] = col;
					}
					for (;j<row_ptr_reordered[i]+width;j++,jj++)   // Padding of smaller rows.
					{
						a[jj] = 0;
						ja[jj] = col;
					}
				}
				for (;jj<row_cluster_ptr[ii+1];jj++)   // Padding of missing rows for last row cluster.
				{
					a[jj] = 0;
					ja[jj] = col;
				}
				transpose(&a[row_cluster_ptr[ii]], VEC_LEN, width);
				transpose(&ja[row_cluster_ptr[ii]], VEC_LEN, width);
			}
		}

		mem_footprint = (num_row_clusters+1) * sizeof(INT_T) + nnz_ext * (sizeof(ValueType) + sizeof(INT_T));
	}

	~SELLArray()
	{
		free(a);
		free(ja);
	}

	void spmv(ValueType * x, ValueType * y);
	void statistics_start();
	int statistics_print_data(__attribute__((unused)) char * buf, __attribute__((unused)) long buf_n);
};


void compute_sell(SELLArray * sell, ValueType * x , ValueType * y);


void
SELLArray::spmv(ValueType * x, ValueType * y)
{
	compute_sell(this, x, y);
}


struct Matrix_Format *
csr_to_format(INT_T * row_ptr, INT_T * col_ind, ValueTypeReference * values, long m, long n, long nnz, long symmetric, long symmetry_expanded)
{
	if (symmetric && !symmetry_expanded)
		error("symmetric matrices have to be expanded to be supported by this format");
	struct SELLArray * sell = new SELLArray(row_ptr, col_ind, values, m, n, nnz);
	sell->format_name = (char *) "SELL";
	return sell;
}


//==========================================================================================================================================
//= SELLPACK
//==========================================================================================================================================


void
compute_sell(SELLArray * sell, ValueType * x , ValueType * y)
{
	#pragma omp parallel
	{
		long tnum = omp_get_thread_num();
		struct thread_data * td = tds[tnum];
		vec_t(VTF, VEC_LEN) zero = vec_set1(VTF, VEC_LEN, 0);
		__attribute__((unused)) vec_t(VTF, VEC_LEN) val = zero, mul = zero, x_buf = zero, sum = zero;
		long ii, ii_s, ii_e, jj, jj_s, jj_e;
		long i, i_s, i_e;
		__attribute__((unused)) long k;
		ii_s = td->ii_s;
		ii_e = td->ii_e;
		i_s = td->i_s;
		i_e = td->i_e;

		// #pragma GCC unroll 2
		for (ii=ii_s;ii<ii_e;ii++)
		{
			// width = (sell->row_cluster_ptr[ii+1] - sell->row_cluster_ptr[ii]) / VEC_LEN;
			sum = vec_set1(VTF, VEC_LEN, 0);
			jj_s = sell->row_cluster_ptr[ii];
			jj_e = sell->row_cluster_ptr[ii+1];
			for (jj=jj_s;jj<jj_e;jj+=VEC_LEN)
			{

				// for (k=0;k<VEC_LEN;k++)
				// {
					// vec_array(VTF, VEC_LEN, mul)[k] = sell->a[jj+k] * x[sell->ja[jj+k]];
				// }
				// sum = vec_add(VTF, VEC_LEN, sum, mul);

				val = vec_loadu(VTF, VEC_LEN, &sell->a[jj]);
				x_buf = vec_set_iter(VTF, VEC_LEN, iter, x[sell->ja[jj+iter]]);
				sum = vec_fmadd(VTF, VEC_LEN, val, x_buf, sum);

			}
			i = VEC_LEN * ii;
			// vec_storeu(VTF, VEC_LEN, &sell->y_buf[i], sum);
			for (k=0;k<VEC_LEN;k++)
			{
				y[sell->rev_permutation[VEC_LEN * ii + k]] = vec_array(VTF, VEC_LEN, sum)[k];
			}

		}

		// for (i=i_s;i<i_e;i++)
		// {
			// y[sell->rev_permutation[i]] = sell->y_buf[i];
		// }

	}
}


//==========================================================================================================================================
//= Print Statistics
//==========================================================================================================================================


void
SELLArray::statistics_start()
{
}


int
statistics_print_labels(__attribute__((unused)) char * buf, __attribute__((unused)) long buf_n)
{
	return 0;
}


int
SELLArray::statistics_print_data(__attribute__((unused)) char * buf, __attribute__((unused)) long buf_n)
{
	return 0;
}

