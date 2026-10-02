#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

#include "macros/cpp_defines.h"

#include "spmv_kernel.h"

#ifdef __cplusplus
extern "C"{
#endif
	#include "macros/macrolib.h"
	#include "macros/permutation.h"
	#include "time_it.h"
	#include "parallel_util.h"
	#include "array_metrics.h"

	#include "aux/csr_util.h"

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
	long i_huge_s;
	long i_huge_e;

	long crossover_row_cluster;
	long ii_s;
	long ii_e;
	long ii_huge_s;
	long ii_huge_e;
	long jj_huge_s;
	long jj_huge_e;
	int huge_row_empty_s;
	int huge_row_empty_e;
	int huge_row_set_s;
	int huge_row_set_e;
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
row_is_above_crossover_sell(INT_T degree)
{
	INT_T crossover_degree = 50;
	int ret = 0;
	if (degree >= crossover_degree)
		ret = 1;
	return ret;
}


inline
int
row_is_above_crossover_huge_rows(INT_T degree, INT_T m, INT_T nnz, INT_T num_threads)
{
	// INT_T max_num_rows = num_threads*num_threads;
	// INT_T max_num_rows = num_threads;
	// if (max_num_rows > m)
		// max_num_rows = m;
	// INT_T crossover_degree = (nnz + max_num_rows - 1) / max_num_rows;   // e.g.: rows with 10% of nnz can't be more than 10.
	INT_T crossover_degree = nnz / num_threads;
	// INT_T crossover_degree = 10000;
	int ret = 0;
	if (degree >= crossover_degree)
		ret = 1;
	return ret;
}


#define test(num)                            \
do {                                         \
	_Pragma("omp barrier")               \
	_Pragma("omp single")                \
	{                                    \
		printf("test %d\n", num);    \
	}                                    \
	_Pragma("omp barrier")               \
} while (0);


struct SELLCSRArray : Matrix_Format
{
	ValueType * a;
	long num_row_clusters;
	long num_row_clusters_normal;
	INT_T * row_cluster_ptr;   // Contains both row clusters for SELL (VEC_LEN rows) and single rows for CSR.
	INT_T * ja;

	long m_normal;
	long nnz_normal;

	INT_T * permutation_total;
	INT_T * rev_permutation_total;

	long nnz_normal_ext;
	long nnz_ext;

	SELLCSRArray(INT_T * row_ptr, INT_T * col_ind, ValueTypeReference * values, long m, long n, long nnz) : Matrix_Format(m, n, nnz)
	{
		long num_threads = omp_get_max_threads();
		INT_T * permutation;
		INT_T * rev_permutation;

		tds = (typeof(tds)) aligned_alloc(64, num_threads * sizeof(*tds));

		printf("VEC_LEN = %d\n", VEC_LEN);

		permutation_total = (typeof(permutation_total)) aligned_alloc(64, m * sizeof(*permutation_total));
		rev_permutation_total = (typeof(rev_permutation_total)) aligned_alloc(64, m * sizeof(*rev_permutation_total));

		permutation = (typeof(permutation)) aligned_alloc(64, m * sizeof(*permutation));
		rev_permutation = (typeof(rev_permutation)) aligned_alloc(64, m * sizeof(*rev_permutation));

		INT_T * row_ptr_buf = (typeof(row_ptr_buf)) aligned_alloc(64, (m+1) * sizeof(*row_ptr_buf));
		INT_T * col_ind_buf = (typeof(col_ind_buf)) aligned_alloc(64, nnz * sizeof(*col_ind_buf));
		ValueType * values_buf = (typeof(values_buf)) aligned_alloc(64, nnz * sizeof(*values_buf));

		_Pragma("omp parallel")
		{
			long tnum = omp_get_thread_num();
			long num_rows_normal_t = 0;
			long num_rows_huge_t = 0;
			long nnz_normal_t = 0;
			long offset_normal, offset_huge;
			long degree;
			long i, i_s, i_e, j, k;
			_Pragma("omp for")
			for (i=0;i<m;i++)
				rev_permutation_total[i] = i;
			loop_partitioner_balance_iterations(num_threads, tnum, 0, m, &i_s, &i_e);
			for (i=i_s;i<i_e;i++)
			{
				degree = row_ptr[i+1] - row_ptr[i];
				if (row_is_above_crossover_huge_rows(degree, m, nnz, num_threads))
					num_rows_huge_t++;
				else
					nnz_normal_t += degree;
			}
			num_rows_normal_t = i_e - i_s - num_rows_huge_t;
			_Pragma("omp barrier")
			omp_thread_reduce_global(reduce_add_long, num_rows_normal_t, 0, 1, 0, &offset_normal, &m_normal); // omp_thread_reduce_global(_reduce_fun, _partial, _zero, exclusive, _backwards, _local_result_ptr_ret, _total_result_ptr_ret);
			omp_thread_reduce_global(reduce_add_long, num_rows_huge_t, 0, 1, 0, &offset_huge, /*unused*/);
			omp_thread_reduce_global(reduce_add_long, nnz_normal_t, 0, 1, 0, /*unused*/, &nnz_normal);
			j = offset_normal;
			k = m_normal + offset_huge;
			for (i=i_s;i<i_e;i++)
			{
				degree = row_ptr[i+1] - row_ptr[i];
				if (row_is_above_crossover_huge_rows(degree, m, nnz, num_threads))
				{
					permutation[i] = k;
					k++;
				}
				else
				{
					permutation[i] = j;
					j++;
				}
			}

			_Pragma("omp barrier")

			for (i=i_s;i<i_e;i++)
			{
				rev_permutation[permutation[i]] = i;
			}

		}

		csr_reorder_rows(permutation, row_ptr, col_ind, values, m, n, nnz, row_ptr_buf, col_ind_buf, values_buf);
		permutation_apply_replace_parallel(&rev_permutation_total, permutation, m, 0);

		INT_T * row_ptr_reordered = (typeof(row_ptr_reordered)) aligned_alloc(64, (m+1) * sizeof(*row_ptr_reordered));
		INT_T * col_ind_reordered = (typeof(col_ind_reordered)) aligned_alloc(64, nnz * sizeof(*col_ind_reordered));
		ValueType * values_reordered = (typeof(values_reordered)) aligned_alloc(64, nnz * sizeof(*values_reordered));

		_Pragma("omp parallel")
		{
			long tnum = omp_get_thread_num();
			struct thread_data * td;
			long i, i_s, i_e, i_huge_s, i_huge_e, ii_s, ii_e, j, k;
			long crossover_row;

			td = (typeof(td)) aligned_alloc(64, sizeof(*td));
			tds[tnum] = td;

			loop_partitioner_balance_prefix_sums(num_threads, tnum, row_ptr_buf, m_normal, nnz_normal, &i_s, &i_e);
			td->i_s = i_s;
			td->i_e = i_e;

			/* Find thread partitions for the huge rows. */
			loop_partitioner_balance_prefix_sums(num_threads, tnum, &(row_ptr_buf[m_normal]), m-m_normal, nnz-nnz_normal, &i_huge_s, &i_huge_e);   // Temporary partitioning for preprocessing.
			i_huge_s += m_normal;
			i_huge_e += m_normal;
			td->i_huge_s = i_huge_s;
			td->i_huge_e = i_huge_e;
			// printf("%2ld: i[%10ld,%10ld), i_huge=[%10ld,%10ld]\n", tnum, i_s, i_e, i_huge_s, i_huge_e);
			_Pragma("omp barrier")
			// long lower_boundary;
			// loop_partitioner_balance_iterations(num_threads, tnum, nnz_normal, nnz, &jj_huge_s, &jj_huge_e);   // loop_partitioner_balance_iterations(_num_workers, _worker_pos, _start, _end, _local_start_ptr, _local_end_ptr)
			// td->jj_huge_s = jj_huge_s;
			// td->jj_huge_e = jj_huge_e;
			// macros_binary_search(row_ptr_buf, m_normal, m, jj_huge_s, &lower_boundary, NULL);           // Index boundaries are inclusive.
			// i_huge_s = lower_boundary;
			// td->i_huge_s = i_huge_s;
			// _Pragma("omp barrier")
			// if (tnum == num_threads - 1)
				// i_huge_e = m;
			// else
				// i_huge_e = tds[tnum+1]->i_huge_s;
			// td->i_huge_e = i_huge_e;

			/* Find SELL-CSR crossover row. */
			long num_rows_below = 0;
			INT_T degree, degree_max = 0;
			for (i=i_s;i<i_e;i++)
			{
				degree = row_ptr_buf[i+1] - row_ptr_buf[i];
				if (degree > degree_max)
					degree_max = degree;
				if (!row_is_above_crossover_sell(degree))
					num_rows_below++;
			}
			num_rows_below = num_rows_below - num_rows_below % VEC_LEN;
			crossover_row = i_s + num_rows_below;
			td->crossover_row = crossover_row;

			/* Find number of row clusters for normal and total rows.
			 * Transform thread row boundaries to row cluster boundaries.
			 */
			long num_row_clusters_sell = (crossover_row - i_s) / VEC_LEN;
			long num_rows_csr = i_e - crossover_row;
			long num_row_clusters_normal_private = num_row_clusters_sell + num_rows_csr;
			omp_thread_reduce_global(reduce_add_long, num_row_clusters_normal_private, 0, 1, 0, &ii_s, &num_row_clusters_normal); // omp_thread_reduce_global(_reduce_fun, _partial, _zero, exclusive, _backwards, _local_result_ptr_ret, _total_result_ptr_ret);
			ii_e = ii_s + num_row_clusters_normal_private;
			td->ii_s = ii_s;
			td->ii_e = ii_e;
			td->crossover_row_cluster = ii_s + num_row_clusters_sell;
			td->ii_huge_s = i_huge_s - m_normal + num_row_clusters_normal;
			td->ii_huge_e = i_huge_e - m_normal + num_row_clusters_normal;
			_Pragma("omp single")
			{
				num_row_clusters = num_row_clusters_normal + m - m_normal;
				printf("m=%ld, m_normal=%ld, num_row_clusters=%ld, num_row_clusters_normal=%ld\n", m, m_normal, num_row_clusters, num_row_clusters_normal);
				row_cluster_ptr = (typeof(row_cluster_ptr)) aligned_alloc(64, (num_row_clusters+1) * sizeof(*row_cluster_ptr));
			}

			/* Sort thread normal row partition by row size. */
			// void bucketsort_stable_recalculate_bucket_serial(_TYPE_V * restrict A, long N, _TYPE_BUCKET_I num_buckets, _TYPE_AD * restrict aux_data, _TYPE_I * restrict permutation_out, _TYPE_I * restrict offsets_out);
			bucketsort_stable_recalculate_bucket_serial(&row_ptr_buf[i_s], i_e-i_s, degree_max+1, &degree_max, &permutation[i_s], NULL);

			for (i=i_s;i<i_e;i++)
				permutation[i] += i_s;
			for (i=i_huge_s;i<i_huge_e;i++)
				permutation[i] = i;

			_Pragma("omp barrier")
			_Pragma("omp for")
			for (i=0;i<m;i++)
				rev_permutation[permutation[i]] = i;

			/* Restore order in CSR part, sort by row index. */
			quicksort(&rev_permutation[crossover_row], i_e - crossover_row, NULL, NULL);

			_Pragma("omp barrier")
			_Pragma("omp for")
			for (i=0;i<m;i++)
				permutation[rev_permutation[i]] = i;

			_Pragma("omp barrier")
			_Pragma("omp for")
			for (i=0;i<m;i++)
				row_ptr_reordered[permutation[i]] = row_ptr_buf[i+1] - row_ptr_buf[i];
			permutation_apply_replace_concurrent(&rev_permutation_total, permutation, m, 0);

			_Pragma("omp single")
			{
				row_ptr_reordered[m] = 0;
			}
			scan_reduce_concurrent(row_ptr_reordered, row_ptr_reordered, m+1, 0, 1, 0);

			_Pragma("omp barrier")

			k = row_ptr_buf[i_s];
			for (i=i_s;i<i_e;i++)
			{
				for (j=row_ptr_buf[rev_permutation[i]];j<row_ptr_buf[rev_permutation[i]+1];j++,k++)
				{
					col_ind_reordered[k] = col_ind_buf[j];
					values_reordered[k] = values_buf[j];
				}
			}
			k = row_ptr_buf[i_huge_s];
			for (i=i_huge_s;i<i_huge_e;i++)
			{
				for (j=row_ptr_buf[rev_permutation[i]];j<row_ptr_buf[rev_permutation[i]+1];j++,k++)
				{
					col_ind_reordered[k] = col_ind_buf[j];
					values_reordered[k] = values_buf[j];
				}
			}
		}

		free(row_ptr_buf);
		free(col_ind_buf);
		free(values_buf);

		/* Extend SELL row clusters to local max row. 
		 * Transpose row clusters.
		 * Extend CSR rows to multiples of 'VEC_LEN'.
		 */
		_Pragma("omp parallel")
		{
			long tnum = omp_get_thread_num();
			struct thread_data * td = tds[tnum];
			long i, i_huge_s, i_huge_e, ii, ii_huge_s, ii_huge_e, j, jj, jj_huge_s, jj_huge_e, k;
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
			i_huge_s = td->i_huge_s;
			i_huge_e = td->i_huge_e;
			ii_huge_s = td->ii_huge_s;
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
				row_cluster_ptr[ii] = macros_next_multiple(row_ptr_reordered[i+1] - row_ptr_reordered[i], VEC_LEN);
			for (i=i_huge_s,ii=ii_huge_s;i<i_huge_e;i++,ii++)
				row_cluster_ptr[ii] = macros_next_multiple(row_ptr_reordered[i+1] - row_ptr_reordered[i], VEC_LEN);
			_Pragma("omp single")
			{
				row_cluster_ptr[num_row_clusters] = 0;
			}
			scan_reduce_concurrent(row_cluster_ptr, row_cluster_ptr, num_row_clusters+1, 0, 1, 0);   // scan_reduce_concurrent(_TYPE_IN * A, _TYPE_OUT * P, long N, _TYPE_OUT zero, const int exclusive, const int backwards);

			_Pragma("omp barrier")
			// _Pragma("omp for")
			// for (i=num_row_clusters_normal;i<num_row_clusters;i++)
			// {
				// degree = row_cluster_ptr[i+1] - row_cluster_ptr[i];
				// if (degree % VEC_LEN)
					// error("test");
			// }

			_Pragma("omp single")
			{
				nnz_normal_ext = row_cluster_ptr[num_row_clusters_normal];
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
			for (i=i_huge_s,ii=ii_huge_s;i<i_huge_e;i++,ii++)
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

			/* After expanding huge rows we need to rebalance them, taking into account that the rows are now a multiple of 'VEC_LEN'. */
			_Pragma("omp barrier")
			loop_partitioner_balance_iterations(num_threads, tnum, nnz_normal_ext, nnz_ext, &jj_huge_s, &jj_huge_e);   // loop_partitioner_balance_iterations(_num_workers, _worker_pos, _start, _end, _local_start_ptr, _local_end_ptr)
			jj_huge_s -= nnz_normal_ext;
			jj_huge_e -= nnz_normal_ext;
			jj_huge_s = macros_next_multiple(jj_huge_s, VEC_LEN);
			jj_huge_e = macros_next_multiple(jj_huge_e, VEC_LEN);
			jj_huge_s += nnz_normal_ext;
			jj_huge_e += nnz_normal_ext;
			if (jj_huge_e > nnz_ext)
				error("jj_huge_e > nnz_ext");
			td->jj_huge_s = jj_huge_s;
			td->jj_huge_e = jj_huge_e;

			long lower_boundary;
			macros_binary_search(row_cluster_ptr, num_row_clusters_normal, num_row_clusters, jj_huge_s, &lower_boundary, NULL);           // Index boundaries are inclusive.
			ii_huge_s = lower_boundary;
			td->ii_huge_s = ii_huge_s;
			_Pragma("omp barrier")
			if (tnum == num_threads - 1)
				ii_huge_e = num_row_clusters;
			else
				ii_huge_e = tds[tnum+1]->ii_huge_s;
			td->ii_huge_e = ii_huge_e;

			td->i_huge_s = ii_huge_s - num_row_clusters_normal + m_normal;
			td->i_huge_e = ii_huge_e - num_row_clusters_normal + m_normal;

			td->huge_row_empty_s = (jj_huge_s >= jj_huge_e) ? 1 : 0;
			td->huge_row_set_s = (jj_huge_s == row_cluster_ptr[ii_huge_s]) ? 1 : 0;

			if (ii_huge_s == ii_huge_e)   // Only one row.
			{
				td->huge_row_empty_e = 1;
				td->huge_row_set_e = 0;
			}
			else   // More than one rows.
			{
				td->huge_row_empty_e = (jj_huge_e == row_cluster_ptr[ii_huge_e]) ? 1 : 0;
				td->huge_row_set_e = (jj_huge_e > row_cluster_ptr[ii_huge_e]) ? 1 : 0;
			}

			// printf("%2ld: i_s=%10ld, crossover_row=%10ld, i_e=%10ld, i_huge_s=%10ld, i_huge_e=%10ld, jj_huge_s=%10ld, jj_huge_e=%10ld, nnz=%10d, rows=%10ld, sell_rows=%10ld, csr_rows=%10ld, huge_rows=%10ld\n",
					// tnum, i_s, crossover_row, i_e, i_huge_s, i_huge_e, td->jj_huge_s, td->jj_huge_e, row_cluster_ptr[ii_e]-row_cluster_ptr[ii_s], i_e-i_s, crossover_row-i_s, i_e-crossover_row, i_huge_e - i_huge_s);
			printf("%2ld: i[%10ld,%10ld:%5ld], nnz_normal=%10d, crossover_row=%10ld, i_huge=[%10ld,%10ld:%5ld], nnz_huge=%10d, jj_huge=[%10ld,%10ld:%5ld], ii=[%10ld,%10ld:%5ld], ii_huge=[%10ld,%10ld:%5ld], empty=[%d,%d], set=[%d,%d]\n",
					tnum, i_s, i_e, i_e-i_s, row_cluster_ptr[ii_e]-row_cluster_ptr[ii_s], crossover_row, i_huge_s, i_huge_e, i_huge_e-i_huge_s, row_cluster_ptr[ii_huge_e]-row_cluster_ptr[ii_huge_s], td->jj_huge_s, td->jj_huge_e, td->jj_huge_e-td->jj_huge_s, ii_s, ii_e, ii_e-ii_s, td->ii_huge_s, td->ii_huge_e, td->ii_huge_e-td->ii_huge_s, td->huge_row_empty_s, td->huge_row_empty_e, td->huge_row_set_s, td->huge_row_set_e);
		}

		free(permutation);
		free(rev_permutation);
		free(row_ptr_reordered);
		free(col_ind_reordered);
		free(values_reordered);

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


void compute_sell_csr(SELLCSRArray * sell, ValueType * x , ValueType * y);


void
SELLCSRArray::spmv(ValueType * x, ValueType * y)
{
	compute_sell_csr(this, x, y);
}


struct Matrix_Format *
csr_to_format(INT_T * row_ptr, INT_T * col_ind, ValueTypeReference * values, long m, long n, long nnz, long symmetric, long symmetry_expanded)
{
	if (symmetric && !symmetry_expanded)
		error("symmetric matrices have to be expanded to be supported by this format");
	struct SELLCSRArray * sell = new SELLCSRArray(row_ptr, col_ind, values, m, n, nnz);
	sell->format_name = (char *) "SELL_SORTED_CSR";
	return sell;
}


//==========================================================================================================================================
//= SELLPACK
//==========================================================================================================================================


#define RESTORE_Y_ORDER  0
// #define RESTORE_Y_ORDER  1


void
compute_sell_csr(SELLCSRArray * sell, ValueType * x , ValueType * y)
{
	_Pragma("omp parallel")
	{
		long tnum = omp_get_thread_num();
		struct thread_data * td = tds[tnum];
		vec_t(VTF, VEC_LEN) zero = vec_set1(VTF, VEC_LEN, 0);
		__attribute__((unused)) vec_t(VTF, VEC_LEN) v_a = zero, v_x = zero, v_sum = zero;
		__attribute__((unused)) vec_t(i32, VEC_LEN) v_col;
		ValueType sum;
		long ii, ii_s, ii_e, jj, jj_s, jj_e;
		__attribute__((unused)) long i, k;
		long i_s, i_e;
		long i_huge_s, i_huge_e;
		long ii_huge_s, ii_huge_e;
		long jj_huge_s, jj_huge_e;
		long crossover_row, crossover_row_cluster;
		i_s = td->i_s;
		i_e = td->i_e;
		ii_s = td->ii_s;
		ii_e = td->ii_e;
		i_huge_s = td->i_huge_s;
		i_huge_e = td->i_huge_e;
		ii_huge_s = td->ii_huge_s;
		ii_huge_e = td->ii_huge_e;
		jj_huge_s = td->jj_huge_s;
		jj_huge_e = td->jj_huge_e;
		crossover_row = td->crossover_row;
		crossover_row_cluster = td->crossover_row_cluster;

		long ii_e_last = ii_e;
		if (ii_e * VEC_LEN > i_e)
			ii_e_last -= VEC_LEN;

		/* SELL */
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
			#if RESTORE_Y_ORDER
				for (k=0;k<VEC_LEN;k++)
					y[sell->rev_permutation_total[i+k]] = vec_array(VTF, VEC_LEN, v_sum)[k];
			#else
				vec_storeu(VTF, VEC_LEN, &y[i], v_sum);
			#endif

		}

		/* CSR */
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
			#if RESTORE_Y_ORDER
				y[sell->rev_permutation_total[i]] = sum;
			#else
				y[i] = sum;
			#endif
		}

		/* CSR huge rows */
		ValueType val_s = 0, val_e = 0;
		if (!td->huge_row_empty_s)
		{
			i=i_huge_s;
			ii=ii_huge_s;
			jj_s = jj_huge_s;
			jj_e = sell->row_cluster_ptr[ii+1];
			if (jj_e > jj_huge_e)
				jj_e = jj_huge_e;
			{
				v_sum = vec_set1(VTF, VEC_LEN, 0);
				for (jj=jj_s;jj<jj_e;jj+=VEC_LEN)
				{
					v_a = vec_loadu(VTF, VEC_LEN, &sell->a[jj]);
					v_col = vec_loadu(i32, VEC_LEN, &sell->ja[jj]);
					v_x = vec_gather(VTF, i32, VEC_LEN, x, v_col);
					v_sum = vec_fmadd(VTF, VEC_LEN, v_a, v_x, v_sum);
				}
				sum = vec_reduce_add(VTF, VEC_LEN, v_sum);
				val_s = sum;
			}
			for (i=i_huge_s+1,ii=ii_huge_s+1;i<i_huge_e;i++,ii++)
			{
				v_sum = vec_set1(VTF, VEC_LEN, 0);
				jj_s = sell->row_cluster_ptr[ii];
				jj_e = sell->row_cluster_ptr[ii+1];
				for (jj=jj_s;jj<jj_e;jj+=VEC_LEN)
				{
					v_a = vec_loadu(VTF, VEC_LEN, &sell->a[jj]);
					v_col = vec_loadu(i32, VEC_LEN, &sell->ja[jj]);
					v_x = vec_gather(VTF, i32, VEC_LEN, x, v_col);
					v_sum = vec_fmadd(VTF, VEC_LEN, v_a, v_x, v_sum);
				}
				sum = vec_reduce_add(VTF, VEC_LEN, v_sum);
				#if RESTORE_Y_ORDER
					y[sell->rev_permutation_total[i]] = sum;
				#else
					y[i] = sum;
				#endif
			}
			if (!td->huge_row_empty_e)
			{
				i=i_huge_e;
				ii=ii_huge_e;
				jj_s = sell->row_cluster_ptr[ii];
				if (jj_s < jj_huge_s)
					jj_s = jj_huge_s;
				jj_e = jj_huge_e;
				{
					v_sum = vec_set1(VTF, VEC_LEN, 0);
					for (jj=jj_s;jj<jj_e;jj+=VEC_LEN)
					{
						v_a = vec_loadu(VTF, VEC_LEN, &sell->a[jj]);
						v_col = vec_loadu(i32, VEC_LEN, &sell->ja[jj]);
						v_x = vec_gather(VTF, i32, VEC_LEN, x, v_col);
						v_sum = vec_fmadd(VTF, VEC_LEN, v_a, v_x, v_sum);
					}
					sum = vec_reduce_add(VTF, VEC_LEN, v_sum);
					val_e = sum;
				}
			}
			if (td->huge_row_set_s)
				#if RESTORE_Y_ORDER
					y[sell->rev_permutation_total[i_huge_s]] = val_s;
				#else
					y[i_huge_s] = val_s;
				#endif
			if ((!td->huge_row_empty_e) && td->huge_row_set_e)
				#if RESTORE_Y_ORDER
					y[sell->rev_permutation_total[i_huge_e]] = val_e;
				#else
					y[i_huge_e] = val_e;
				#endif
			_Pragma("omp barrier")
			if (!td->huge_row_set_s)
			{
				#pragma omp atomic
				#if RESTORE_Y_ORDER
					y[sell->rev_permutation_total[i_huge_s]] += val_s;
				#else
					y[i_huge_s] += val_s;
				#endif
			}
			if ((!td->huge_row_empty_e) && (!td->huge_row_set_e))
			{
				#pragma omp atomic
				#if RESTORE_Y_ORDER
					y[sell->rev_permutation_total[i_huge_e]] += val_e;
				#else
					y[i_huge_e] += val_e;
				#endif
			}
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

