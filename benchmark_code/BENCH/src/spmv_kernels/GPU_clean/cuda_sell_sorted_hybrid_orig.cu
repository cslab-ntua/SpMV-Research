#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

#include <cuda.h>
#include <cooperative_groups.h>
#include <cuda_pipeline_primitives.h>

#include "macros/cpp_defines.h"

#include "../spmv_kernel.h"


#define ValueTypeStored  ValueType
// #define ValueTypeStored  float


#ifndef BLOCK_SIZE
	// #define BLOCK_SIZE  32
	// #define BLOCK_SIZE  64
	#define BLOCK_SIZE  128
	// #define BLOCK_SIZE  256
	// #define BLOCK_SIZE  512
	// #define BLOCK_SIZE  1024
#endif

#define NNZ_PER_THREAD  5


#ifdef __cplusplus
extern "C"{
#endif
	#include "macros/macrolib.h"
	#include "time_it.h"
	#include "parallel_util.h"
	#include "array_metrics.h"

	#include "aux/csr_util.h"

	#include "cuda/cuda_util.h"

	static inline
	double
	idx_to_double(void * A, long i)
	{
		return (double) ((INT_T *) A)[i];
	}

	static inline
	double
	row_ptr_to_degree_double(void * A, long i)
	{
		return (double) (((INT_T *) A)[i+1] - ((INT_T *) A)[i]);
	}

	#include "functools/functools_gen_push.h"
	#define FUNCTOOLS_GEN_TYPE_1  int
	#define FUNCTOOLS_GEN_TYPE_2  int
	#define FUNCTOOLS_GEN_SUFFIX  _CUDA_SELL_SORTED_HYBRID
	#include "functools/functools_gen.c"
	__attribute__((pure))
	static inline
	int
	functools_map_fun(int * A, long i)
	{
		return A[i];
	}
	__attribute__((pure))
	static inline
	int
	functools_reduce_fun(int a, int b)
	{
		return a + b;
	}

	#include "sort/bucketsort/bucketsort_gen_undef.h"
	#define BUCKETSORT_GEN_TYPE_1  INT_T
	#define BUCKETSORT_GEN_TYPE_2  INT_T
	#define BUCKETSORT_GEN_TYPE_3  int
	#define BUCKETSORT_GEN_TYPE_4  int
	#define BUCKETSORT_GEN_SUFFIX  _CUDA_SELL_SORTED_HYBRID
	#include "sort/bucketsort/bucketsort_gen.c"
	static inline
	INT_T
	bucketsort_find_bucket(INT_T * A, long i, __attribute__((unused)) INT_T * degree_max_ptr)
	{
		return A[i+1] - A[i];   // Ascending order.
		// return *degree_max_ptr - (A[i+1] - A[i]);   // Descending order.
	}

	struct samplesort_data_s {
		INT_T * row_ptr;
		INT_T * ja;
		// ValueType * a;
		INT_T * row_membership;
	};
	#include "sort/samplesort/samplesort_gen_undef.h"
	#define SAMPLESORT_GEN_TYPE_1  INT_T
	#define SAMPLESORT_GEN_TYPE_2  INT_T
	#define SAMPLESORT_GEN_TYPE_3  int
	#define SAMPLESORT_GEN_TYPE_4  struct samplesort_data_s
	#define SAMPLESORT_GEN_FUNCTION_ATTRIBUTES
	#define SAMPLESORT_GEN_SUFFIX  _CUDA_SELL_SORTED_HYBRID
	#include "sort/samplesort/samplesort_gen.c"
	static inline
	int
	samplesort_cmp(INT_T a, INT_T b, struct samplesort_data_s * data)
	{
		int ret = 0;
		if (data != NULL)
		{
			// INT_T ac = data->row_membership[a];
			// INT_T bc = data->row_membership[b];
			INT_T ac = a;
			INT_T bc = b;
			__attribute__((unused)) INT_T j_s_a=data->row_ptr[ac], j_e_a=data->row_ptr[ac+1];
			__attribute__((unused)) INT_T j_s_b=data->row_ptr[bc], j_e_b=data->row_ptr[bc+1];
			__attribute__((unused)) INT_T col_s_a = data->ja[j_s_a], col_e_a = j_e_a > 0 ? data->ja[j_e_a - 1] : 0;
			__attribute__((unused)) INT_T col_s_b = data->ja[j_s_b], col_e_b = j_e_b > 0 ? data->ja[j_e_b - 1] : 0;
			__attribute__((unused)) INT_T col_center_a = (col_s_a + col_e_a) / 2;
			__attribute__((unused)) INT_T col_center_b = (col_s_b + col_e_b) / 2;
			__attribute__((unused)) INT_T degree_a = j_e_a - j_s_a;
			__attribute__((unused)) INT_T degree_b = j_e_b - j_s_b;

			double ae = fabs(degree_a - degree_b);
			double degree_min = (degree_a < degree_b) ? degree_a : degree_b;
			double relative_dist = ae / degree_min;
			if ( ! ((degree_min >= 4) && (relative_dist < 1.0/4)) )
			{
				if (ret == 0) ret = (degree_a > degree_b) ? 1 : (degree_a < degree_b) ? -1 : 0;
			}
			// if (ret == 0) ret = (degree_a > degree_b) ? 1 : (degree_a < degree_b) ? -1 : 0;
			if (ret == 0) ret = (col_e_a > col_e_b) ? 1 : (col_e_a < col_e_b) ? -1 : 0;
			// if (ret == 0) ret = (col_s_a > col_s_b) ? 1 : (col_s_a < col_s_b) ? -1 : 0;
			// if (ret == 0) ret = (col_center_a > col_center_b) ? 1 : (col_center_a < col_center_b) ? -1 : 0;

		}
		if (ret == 0) ret = a > b ? 1 : a < b ? -1 : 0;
		return ret;
		// return -ret;  // Descending order.
	}


	#include "sort/quicksort/quicksort_gen_undef.h"
	#define QUICKSORT_GEN_TYPE_1  INT_T
	#define QUICKSORT_GEN_TYPE_2  INT_T
	#define QUICKSORT_GEN_TYPE_3  INT_T
	#define QUICKSORT_GEN_SUFFIX  _CUDA_SELL_SORTED_HYBRID
	#include "sort/quicksort/quicksort_gen.c"
	static inline
	int
	quicksort_cmp(INT_T a, INT_T b, INT_T * data)
	{
		int ret = 0;
		INT_T va=data[a], vb=data[b];
		if (ret == 0) ret = va > vb ? 1 : va < vb ? -1 : 0;
		if (ret == 0) ret = a > b ? 1 : a < b ? -1 : 0;
		return ret;
	}


#ifdef __cplusplus
}
#endif


namespace cg = cooperative_groups;

using namespace cooperative_groups;


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
row_is_above_crossover(INT_T degree, long m, long nnz, int device_max_num_threads)
{
	INT_T crossover_ratio = 5;
	INT_T crossover_degree = 50;
	int ret = 0;
	INT_T degree_avg = nnz / m;
	if (m < 2 * device_max_num_threads)
	{
		if (degree >= crossover_degree)
			ret = 1;
	}
	else
	{
		if (degree / degree_avg >= crossover_ratio)
			ret = 1;
		if (degree >= crossover_degree)
			ret = 1;
	}
	return ret;
}


double
cross_row_similarity(long i1, long i2, INT_T * row_ptr, INT_T * ja)
{
	long cache_line_size = 128;
	long cache_line_elems = cache_line_size / sizeof(ValueType);
	long j1, j2;
	long degree1, degree2;
	long col, col1, col2;
	long num_same_cls = 0;
	degree1 = row_ptr[i1+1] - row_ptr[i1];
	degree2 = row_ptr[i2+1] - row_ptr[i2];
	j1 = row_ptr[i1];
	j2 = row_ptr[i2];
	while ((j1 < row_ptr[i1+1]) && (j2 < row_ptr[i2+1]))
	{
		col1 = ja[j1] - ja[j1] % cache_line_elems;
		col2 = ja[j2] - ja[j2] % cache_line_elems;
		if (col1 < col2)
			j1++;
		else if (col1 > col2)
			j2++;
		else
		{
			col = col1;
			while (col == col1)
			{
				num_same_cls++;
				j1++;
				if (j1 >= row_ptr[i1+1])
					break;
				col1 = ja[j1] - ja[j1] % cache_line_elems;
			}
			while (col == col2)
			{
				num_same_cls++;
				j2++;
				if (j2 >= row_ptr[i2+1])
					break;
				col2 = ja[j2] - ja[j2] % cache_line_elems;
			}
		}
	}
	return ((double) num_same_cls) / (degree1 + degree2);
}


/* We don't change the cluster center as we compare rows, because it can cause drifting to very different
 * row structures if we e.g., compare to the previous row.
 * We want to be able to confine the bandwidth of the cluster somehow.
 */
void
cluster_similar_rows(INT_T * row_membership, INT_T * row_ptr, INT_T * ja, long m)
{
	long num_threads_cpu = omp_get_max_threads();
	_Pragma("omp parallel")
	{
		long tnum = omp_get_thread_num();
		long i, i_s, i_e, k, cluster_center;
		long degree, cluster_degree_center;
		long cluster_num_rows;
		long cluster_degree_min, cluster_degree_max;
		double crs;
		double crs_min = 0.4;
		long threshold_degree = 4;
		double threshold_degree_rel_diff = 1.0 / threshold_degree;
		double ae;
		double rel_diff;
		loop_partitioner_balance_iterations(num_threads_cpu, tnum, 0, m, &i_s, &i_e);
		_Pragma("omp for")
		for (i=0;i<m;i++)
		{
			row_membership[i] = i;
		}
		i = i_s;
		while (i < i_e - 1)
		{
			cluster_center = i;
			cluster_degree_center = row_ptr[i+1] - row_ptr[i];
			if (cluster_degree_center < threshold_degree)
			{
				i++;
				continue;
			}

			cluster_num_rows = 1;
			cluster_degree_min = cluster_degree_center;
			cluster_degree_max = cluster_degree_center;
			while (1)
			{
				k = i + cluster_num_rows;
				degree = row_ptr[k+1] - row_ptr[k];
				if (degree < threshold_degree)
					break;
				if (degree < cluster_degree_min)
				{
					ae = fabs(degree - cluster_degree_max);
					rel_diff = ae / degree;
					if (rel_diff > threshold_degree_rel_diff)
						break;
					cluster_degree_min = degree;
				}
				else if (degree > cluster_degree_max)
				{
					ae = fabs(degree - cluster_degree_min);
					rel_diff = ae / cluster_degree_min;
					if (rel_diff > threshold_degree_rel_diff)
						break;
					cluster_degree_max = degree;
				}
				crs = cross_row_similarity(cluster_center, k, row_ptr, ja);
				if (crs < crs_min)
					break;
				cluster_num_rows++;
			}

			for (k=i;k<i+cluster_num_rows;k++)
			{
				row_membership[k] = cluster_center;
			}

			i += cluster_num_rows;
		}
	}
}


void
sort_sell_warp_columns(INT_T i, INT_T * row_ptr, INT_T * ja, ValueTypeStored * a, long m)
{
	long cache_line_size = 128;
	long cache_line_elems = cache_line_size / sizeof(ValueType);
	long nnz_warp = row_ptr[i+32] - row_ptr[i];
	INT_T * ja_warp = &ja[row_ptr[i]];
	ValueType * a_warp = &a[row_ptr[i]];
	INT_T * idx = (typeof(idx)) malloc(nnz_warp * sizeof(*idx));
	INT_T * rfs = (typeof(rfs)) malloc(nnz_warp * sizeof(*rfs));
	INT_T * ja_buf = (typeof(ja_buf)) malloc(nnz_warp * sizeof(*ja_buf));
	ValueType * a_buf = (typeof(a_buf)) malloc(nnz_warp * sizeof(*a_buf));
	INT_T * ja_row;
	ValueType * a_row;
	INT_T * rfs_row;
	INT_T degree, rf;
	long j, k, l;
	for (j=0;j<nnz_warp;j++)
		idx[j] = j;
	quicksort(idx, nnz_warp, ja_warp, NULL);   // Sort all warp nnz by column index.
	k = 0;
	j = 0;
	while (j<nnz_warp)   // Find replication factors of column indices.
	{
		rf = 1;
		j++;
		for (;j<nnz_warp;j++)
		{
			if (ja_warp[idx[j]] / cache_line_elems != ja_warp[idx[j-1]] / cache_line_elems)
				break;
			rf++;
		}
		for (l=k;l<j;l++)
		{
			rfs[idx[l]] = -rf;   // Negative to sort descending after.
		}
		k = j;
	}
	ja_row = ja_warp;
	a_row = a_warp;
	rfs_row = rfs;
	for (k=i;k<i+32;k++)   // Sort each row by replication factors, descending.
	{
		degree = row_ptr[k+1] - row_ptr[k];
		for (j=0;j<degree;j++)
			idx[j] = j;
		quicksort(idx, degree, rfs_row, NULL);
		for (j=0;j<degree;j++)
		{
			ja_buf[j] = ja_row[idx[j]];
			a_buf[j] = a_row[idx[j]];
		}
		for (j=0;j<degree;j++)
		{
			ja_row[j] = ja_buf[j];
			a_row[j] = a_buf[j];
		}
		ja_row += degree;
		a_row += degree;
		rfs_row += degree;
	}
	free(idx);
	free(rfs);
	free(ja_buf);
	free(a_buf);
}


struct SELLArrays : Matrix_Format
{
	long crossover_row;   // A row index in the SORTED matrix, where we change from SELL to CSR. Multiple of BLOCK_SIZE.
	long nnz_per_thread;
	long nnz_per_block;
	long nnz_per_warp;

	long num_row_clusters;
	long nnz_extended;
	long nnz_sell;
	long nnz_csr;

	INT_T * row_ptr_h;
	INT_T * row_cluster_ptr_h;
	INT_T * ja_h;
	ValueTypeStored * a_h;
	INT_T * thread_warp_i_s = NULL;
	INT_T * thread_warp_i_e = NULL;

	INT_T * row_ptr_d;
	INT_T * row_cluster_ptr_d;
	INT_T * ja_d;
	ValueTypeStored * a_d;
	INT_T * thread_warp_i_s_d = NULL;
	INT_T * thread_warp_i_e_d = NULL;

	ValueType * x = NULL;
	ValueType * y = NULL;
	ValueType * x_h = NULL;
	ValueType * y_h = NULL;
	ValueType * x_d = NULL;
	ValueType * y_d = NULL;

	int thread_block_size;

	int num_threads;
	int num_threads_sell;
	int num_threads_csr;
	int num_thread_warps_sell;
	int num_thread_warps_csr;
	int num_thread_blocks;
	int num_thread_blocks_sell;
	int num_thread_blocks_csr;

	INT_T * row_permutation = NULL;


	SELLArrays(INT_T * row_ptr, INT_T * ja, ValueTypeReference * a, long m, long n, long nnz) : Matrix_Format(m, n, nnz)
	{
		long num_threads_cpu = omp_get_max_threads();
		__attribute__((unused)) long enable_legend = 1;
		__attribute__((unused)) long num_pixels_x = 1080;
		__attribute__((unused)) long num_pixels_y = 1080;
		double time;
		long i;

		cuda_device_print_attributes();

		int device_multiproc_count, device_max_threads_per_multiproc, device_max_num_threads;
		cuda_assert(cudaDeviceGetAttribute(&device_multiproc_count, cudaDevAttrMultiProcessorCount, 0));
		cuda_assert(cudaDeviceGetAttribute(&device_max_threads_per_multiproc, cudaDevAttrMaxThreadsPerMultiProcessor, 0));
		device_max_num_threads = device_max_threads_per_multiproc * device_multiproc_count;

		nnz_per_thread = NNZ_PER_THREAD;
		nnz_per_block = nnz_per_thread * BLOCK_SIZE;
		nnz_per_warp = nnz_per_thread * 32;

		thread_block_size = BLOCK_SIZE;
		if (thread_block_size % 32)
			error("select a thread block size that is a multiple of 32");

		/* Find crossover row.
		 * It should be a multiple of BLOCK_SIZE to simplify the SELL code. */
		crossover_row = 0;
		_Pragma("omp parallel")
		{
			INT_T degree;
			long num_rows_below = 0;
			long i;
			_Pragma("omp for")
			for (i=0;i<m;i++)
			{
				degree = row_ptr[i+1] - row_ptr[i];
				if (!row_is_above_crossover(degree, m, nnz, device_max_num_threads))
					num_rows_below++;
			}
			__atomic_fetch_add(&crossover_row, num_rows_below, __ATOMIC_RELAXED);
		}
		crossover_row = crossover_row - crossover_row % BLOCK_SIZE;
		printf("m=%ld, crossover_row=%ld\n", m, crossover_row);

		/* Sort rows. */
		row_permutation = (typeof(row_permutation)) malloc(m * sizeof(*row_permutation));
		_Pragma("omp parallel")
		{
			long i;
			_Pragma("omp for")
			for (i=0;i<m;i++)
				row_permutation[i] = i;
		}
		INT_T * row_membership = (typeof(row_membership)) malloc(m * sizeof(*row_membership));
		time = time_it(1,
			cluster_similar_rows(row_membership, row_ptr, ja, m);
		);
		printf("time cluster_similar_rows = %g\n", time);
		double time_sort_rows = time_it(1,
			// csr_plot("matrix", row_ptr, ja, a, m, n, nnz, enable_legend, num_pixels_x, num_pixels_y);

			// double max;
			// array_max(row_ptr, m, &max, NULL, row_ptr_to_degree_double);
			// INT_T degree_max = max;
			// printf("degree_max=%d\n", degree_max);
			// bucketsort_stable_recalculate_bucket_serial(row_ptr, m, degree_max+1, &degree_max, row_permutation, NULL);
			INT_T * rev_row_permutation = (typeof(rev_row_permutation)) malloc(m * sizeof(*rev_row_permutation));
			_Pragma("omp parallel")
			{
				long i;
				_Pragma("omp for")
				for (i=0;i<m;i++)
					rev_row_permutation[i] = i;
			}

			// struct samplesort_data_s tmp_csr = { .row_ptr=row_ptr, .ja=ja };
			struct samplesort_data_s tmp_csr = { .row_ptr=row_ptr, .ja=ja, .row_membership=row_membership };
			/* Sort by row size. */
			samplesort(rev_row_permutation, m, &tmp_csr);

			/* Restore order in CSR part, sort by row index. */
			samplesort(&rev_row_permutation[crossover_row], m-crossover_row, NULL);

			for (i=0;i<m;i++)
				row_permutation[rev_row_permutation[i]] = i;
			free(rev_row_permutation);

			row_ptr_h = (typeof(row_ptr_h)) malloc((m+1) * sizeof(*row_ptr_h));
			ja_h = (typeof(ja_h)) malloc(nnz * sizeof(*ja_h));
			a_h = (typeof(a_h)) malloc(nnz * sizeof(*a_h));
			csr_reorder_rows(row_permutation, row_ptr, ja, a, m, n, nnz, row_ptr_h, ja_h, a_h);

			// csr_plot("matrix_reordered", row_ptr_h, ja_h, a_h, m, n, nnz, enable_legend, num_pixels_x, num_pixels_y);

			free(row_membership);
		);
		printf("time sort rows = %g\n", time_sort_rows);


		/* Extend SELL row clusters to local max row. 
		 * Transpose row clusters.
		 * Extend CSR rows to multiples of 'nnz_per_thread'.
		 * Extend last row so that CSR has multiple of BLOCK_SIZE number of threads,
		 * i.e., CSR has multiple of 'nnz_per_thread * BLOCK_SIZE' nonzeros.
		 */
		INT_T * row_ptr_h_new = (typeof(row_ptr_h_new)) malloc((m+1) * sizeof(*row_ptr_h_new));
		INT_T * ja_h_new;
		ValueTypeStored * a_h_new;
		_Pragma("omp parallel")
		{
			long i, j, j1, j2, j_s, j_e, k;
			long degree;
			long degree_cluster_max;
			_Pragma("omp for")
			for (i=0;i<crossover_row;i+=32)
			{
				degree_cluster_max = 0;
				for (k=i;k<i+32;k++)
				{
					degree = row_ptr_h[k+1] - row_ptr_h[k];
					if (degree > degree_cluster_max)
						degree_cluster_max = degree;
				}
				for (k=i;k<i+32;k++)
					row_ptr_h_new[k] = degree_cluster_max;
			}
			_Pragma("omp for")
			for (i=crossover_row;i<m;i++)
			{
				degree = row_ptr_h[i+1] - row_ptr_h[i];
				degree = nnz_per_thread * ((degree + nnz_per_thread - 1) / nnz_per_thread);
				row_ptr_h_new[i] = degree;
			}
			_Pragma("omp single")
			{
				row_ptr_h_new[m] = 0;
			}
			scan_reduce_concurrent(row_ptr_h_new, row_ptr_h_new, m+1, 0, 1, 0);
			_Pragma("omp single")
			{
				nnz_extended = row_ptr_h_new[m];
				nnz_sell = row_ptr_h_new[crossover_row];
				nnz_csr = nnz_extended - nnz_sell;
				nnz_csr = nnz_per_block * ((nnz_csr + nnz_per_block - 1) / nnz_per_block);
				nnz_extended = nnz_sell + nnz_csr;
				row_ptr_h_new[m] = nnz_extended;
				ja_h_new = (typeof(ja_h_new)) malloc(nnz_extended * sizeof(*ja_h_new));
				a_h_new = (typeof(a_h_new)) malloc(nnz_extended * sizeof(*a_h_new));
			}
			_Pragma("omp for")
			for (i=0;i<crossover_row;i+=32)
			{
				for (k=i;k<i+32;k++)
				{
					for (j1=row_ptr_h[k],j2=row_ptr_h_new[k];j1<row_ptr_h[k+1];j1++,j2++)
					{
						ja_h_new[j2] = ja_h[j1];
						a_h_new[j2] = a_h[j1];
					}
					for (;j2<row_ptr_h_new[k+1];j2++)
					{
						ja_h_new[j2] = ja_h[row_ptr_h[k+1] - 1];
						a_h_new[j2] = 0;
					}
				}
				degree = row_ptr_h_new[i+1] - row_ptr_h_new[i];
				/* if (i == (1500000ULL & (~31ULL)))
				{
					for (k=i;k<i+32;k++)
					{
						for (j=row_ptr_h_new[k];j<row_ptr_h_new[k+1];j++)
						{
							// printf("%8d ", ja_h_new[j] / 128);
							printf("%8d ", ja_h_new[j]);
						}
						printf("\n");
					}
					printf("\n");
				} */

				// sort_sell_warp_columns(i, row_ptr_h_new, ja_h_new, a_h_new, m);

				transpose(&ja_h_new[row_ptr_h_new[i]], 32, degree);
				transpose(&a_h_new[row_ptr_h_new[i]], 32, degree);
			}
			_Pragma("omp for")
			for (i=crossover_row;i<m;i++)
			{
				for (j1=row_ptr_h[i],j2=row_ptr_h_new[i];j1<row_ptr_h[i+1];j1++,j2++)
				{
					ja_h_new[j2] = ja_h[j1];
					a_h_new[j2] = a_h[j1];
				}
				for (;j2<row_ptr_h_new[i+1];j2++)
				{
					ja_h_new[j2] = ja_h[row_ptr_h[i+1] - 1];
					a_h_new[j2] = 0;
				}
				ja_h_new[row_ptr_h_new[i]] |= 0x80000000;

				j_s = row_ptr_h_new[i];
				while (j_s < row_ptr_h_new[i+1])   // Interleave thread nnz of same row (round-robin).
				{
					j_e = j_s + nnz_per_warp;
					j_e = j_e - ((j_e - nnz_sell) % nnz_per_warp);
					if (j_e > row_ptr_h_new[i+1])
						j_e = row_ptr_h_new[i+1];
					long local_num_threads = (j_e - j_s) / nnz_per_thread;
					transpose(&a_h_new[j_s], nnz_per_thread, local_num_threads);
					transpose(&ja_h_new[j_s], nnz_per_thread, local_num_threads);
					j_s = j_e;
				}
			}
			_Pragma("omp for")
			for (j=row_ptr_h_new[crossover_row];j<nnz_extended;j+=32*nnz_per_thread)
			{
				transpose(&a_h_new[j], 32, nnz_per_thread);
				transpose(&ja_h_new[j], 32, nnz_per_thread);
			}
		}
		free(row_ptr_h);
		free(ja_h);
		free(a_h);
		row_ptr_h = row_ptr_h_new;
		ja_h = ja_h_new;
		a_h = a_h_new;
		printf("nnz=%ld, nnz_extended=%ld, nnz_sell=%ld, nnz_csr=%ld\n", nnz, nnz_extended, nnz_sell, nnz_csr);

		/* Find number of threads for each format. */
		num_threads = crossover_row + nnz_csr / nnz_per_thread;
		num_threads_sell = crossover_row;
		num_threads_csr = num_threads - num_threads_sell;
		num_thread_blocks = num_threads / BLOCK_SIZE;
		num_thread_blocks_sell = num_threads_sell / BLOCK_SIZE;
		num_thread_blocks_csr = num_threads_csr / BLOCK_SIZE;
		num_thread_warps_sell = num_threads_sell / 32;
		num_thread_warps_csr = num_threads_csr / 32;
		printf("num_threads=%d, thread_block_size=%d, num_thread_blocks=%d\n", num_threads, BLOCK_SIZE, num_thread_blocks);

		/* Find SELL row clusters offsets. */
		num_row_clusters = crossover_row / 32;
		row_cluster_ptr_h = (typeof(row_cluster_ptr_h)) malloc((num_row_clusters+1) * sizeof(*row_cluster_ptr_h));
		for (i=0;i<crossover_row;i+=32)
			row_cluster_ptr_h[i/32] = row_ptr_h[i];
		row_cluster_ptr_h[num_row_clusters] = row_ptr_h[crossover_row];

		/* Find CSR warps row boundaries. */
		thread_warp_i_s = (INT_T *) malloc(num_thread_warps_csr * sizeof(*thread_warp_i_s));
		thread_warp_i_e = (INT_T *) malloc(num_thread_warps_csr * sizeof(*thread_warp_i_e));
		double time_balance = time_it(1,
			_Pragma("omp parallel")
			{
				INT_T thread_warp_j_s, thread_warp_j_e;
				long lower_boundary, higher_boundary;
				_Pragma("omp for")
				for (i=0;i<num_thread_warps_csr;i++)
				{
					thread_warp_j_s = nnz_sell + nnz_per_warp * i;

					if (thread_warp_j_s > nnz_extended)
						thread_warp_j_s = nnz_extended;
					macros_binary_search(row_ptr_h, 0, m, thread_warp_j_s, &lower_boundary, NULL);           // Index boundaries are inclusive.
					while (row_ptr_h[lower_boundary] == row_ptr_h[lower_boundary+1])
						lower_boundary++;
					thread_warp_i_s[i] = lower_boundary;
					thread_warp_j_e = thread_warp_j_s + nnz_per_warp;
					if (thread_warp_j_e > nnz_extended)
						thread_warp_j_e = nnz_extended;
					macros_binary_search(row_ptr_h, 0, m, thread_warp_j_e, NULL, &higher_boundary);           // Index boundaries are inclusive.
					while (row_ptr_h[higher_boundary] == row_ptr_h[higher_boundary+1])
						higher_boundary--;
					thread_warp_i_e[i] = higher_boundary;
				}
			}
		);
		printf("time find warp boundaries = %g\n", time_balance);

		cuda_assert(cudaMalloc(&row_ptr_d, (m+1) * sizeof(*row_ptr_d)));
		cuda_assert(cudaMalloc(&row_cluster_ptr_d, (num_row_clusters+1) * sizeof(*row_cluster_ptr_d)));
		cuda_assert(cudaMalloc(&ja_d, nnz_extended * sizeof(*ja_d)));
		cuda_assert(cudaMalloc(&a_d, nnz_extended * sizeof(*a_d)));
		cuda_assert(cudaMalloc(&x_d, n * sizeof(*x_d)));
		cuda_assert(cudaMalloc(&y_d, m * sizeof(*y_d)));
		cuda_assert(cudaMalloc(&thread_warp_i_s_d, num_thread_warps_csr * sizeof(*thread_warp_i_s_d)));
		cuda_assert(cudaMalloc(&thread_warp_i_e_d, num_thread_warps_csr * sizeof(*thread_warp_i_e_d)));

		x_h = (typeof(x_h)) malloc(n * sizeof(*x_h));
		y_h = (typeof(y_h)) malloc(m * sizeof(*y_h));

		cuda_assert(cudaMemcpy(row_ptr_d, row_ptr_h, (m+1) * sizeof(*row_ptr_d), cudaMemcpyHostToDevice));
		cuda_assert(cudaMemcpy(row_cluster_ptr_d, row_cluster_ptr_h, (num_row_clusters+1) * sizeof(*row_cluster_ptr_d), cudaMemcpyHostToDevice));
		cuda_assert(cudaMemcpy(ja_d, ja_h, nnz_extended * sizeof(*ja_d), cudaMemcpyHostToDevice));
		cuda_assert(cudaMemcpy(a_d, a_h, nnz_extended * sizeof(*a_d), cudaMemcpyHostToDevice));
		cuda_assert(cudaMemcpy(thread_warp_i_s_d, thread_warp_i_s, num_thread_warps_csr * sizeof(*thread_warp_i_s_d), cudaMemcpyHostToDevice));
		cuda_assert(cudaMemcpy(thread_warp_i_e_d, thread_warp_i_e, num_thread_warps_csr * sizeof(*thread_warp_i_e_d), cudaMemcpyHostToDevice));
	}

	~SELLArrays()
	{
		cuda_assert(cudaFree(row_ptr_d));
		cuda_assert(cudaFree(row_cluster_ptr_d));
		cuda_assert(cudaFree(ja_d));
		cuda_assert(cudaFree(a_d));
		cuda_assert(cudaFree(x_d));
		cuda_assert(cudaFree(y_d));

		cuda_assert(cudaFreeHost(row_ptr_h));
		cuda_assert(cudaFree(row_cluster_ptr_h));
		cuda_assert(cudaFreeHost(ja_h));
		cuda_assert(cudaFreeHost(a_h));
		cuda_assert(cudaFreeHost(x_h));
		cuda_assert(cudaFreeHost(y_h));
	}

	void spmv(ValueType * x, ValueType * y);
	void statistics_start();
	int statistics_print_data(__attribute__((unused)) char * buf, __attribute__((unused)) long buf_n);
};


void compute_sell_sorted(SELLArrays * restrict csr, ValueType * restrict x , ValueType * restrict y);

void
SELLArrays::spmv(ValueType * x, ValueType * y)
{
	compute_sell_sorted(this, x, y);
}


struct Matrix_Format *
csr_to_format(INT_T * row_ptr, INT_T * col_ind, ValueTypeReference * values, long m, long n, long nnz, long symmetric, long symmetry_expanded)
{
	if (symmetric && !symmetry_expanded)
		error("symmetric matrices not supported by this format, extend symmetry");
	struct SELLArrays * csr = new SELLArrays(row_ptr, col_ind, values, m, n, nnz);
	// for (long i=0;i<10;i++)
		// printf("%d\n", row_ptr[i]);
	csr->mem_footprint = csr->nnz_extended * (sizeof(ValueTypeStored) + sizeof(INT_T)) + (csr->m+1) * sizeof(INT_T);
	char *format_name;
	format_name = (char *)malloc(100*sizeof(char));
	snprintf(format_name, 100, "Custom_CSR_CUDA_sell_sorted_hybrid_b%d", BLOCK_SIZE);
	csr->format_name = format_name;
	return csr;
}


//==========================================================================================================================================
//= SELL Custom
//==========================================================================================================================================


template<typename T, typename group_t>
static __attribute__((always_inline)) inline
__device__
T
warp_scan_reduce_inclusive(group_t g, T value)
{
	int lane = g.thread_rank();
	T prev;
	int i;
	#pragma unroll
	for (i=1;i<32;i*=2)
	{
		prev = g.shfl_up(value, i);
		if (lane >= i)
			value += prev;
	}
	return value;
}


template<typename T, typename group_t>
static __attribute__((always_inline)) inline
__device__
T
warp_scan_reduce_exclusive(group_t g, T value)
{
	return warp_scan_reduce_inclusive(g, value) - value;
}


template <typename T, typename group_t>
inline
__device__
void
reduce_warp(group_t g, INT_T row, T val, T * restrict y)
{
	const int tidw = g.thread_rank();   // Group lane.
	int mask_same_row = g.match_any(row);
	int k;
	#pragma unroll
	for (k=g.size()/2; k>=1; k/=2)
	{
		int tidl_next = tidw + k;
		T val_next = g.shfl(val, tidl_next);
		if ((tidl_next < g.size()) && (mask_same_row & (1 << tidl_next)))
		{
			val += val_next;
		}
	}
	if (tidw == __ffs(mask_same_row) - 1)  // __ffs enumeration is 1-based.
		atomicAdd(&y[row], val);
}


template <typename T, typename group_t>
inline
__device__
T
reduce_warp_single_row(group_t g, T val)
{
	// Use XOR mode to perform butterfly reduction
	for (int i=g.size()/2; i>=1; i/=2)
	{
		val += g.shfl_xor(val, i); // __shfl_xor_sync(0xFFFFFFFF, val, i, g.size());   // Total sum is same on all threads.
		// val += __shfl_down_sync(0xFFFFFFFF, val, i, g.size());   // Only thread 0 has the total sum.
	}
	return val;
}


template <const int nnz_per_thread>
__device__
void
spmv_csr(const int tid, INT_T crossover_row, INT_T crossover_offset, INT_T * thread_warp_i_s, INT_T * ja, ValueTypeStored * a, INT_T m, INT_T n, INT_T nnz, ValueType * restrict x, ValueType * restrict y)
{
	const int tid_csr = tid - crossover_row;
	extern __shared__ char sm[];
	thread_block_tile<32> g = tiled_partition<32>(this_thread_block());
	const int tidw = g.thread_rank();
	const int wid_csr = tid_csr / g.size();
	double sum;
	INT_T new_row;
	int single_row;
	__attribute__((unused)) INT_T i, i_w_s, i_e, j, jj, jj_s, j_w_s, k, col;
	j_w_s = crossover_offset + wid_csr * g.size() * nnz_per_thread;
	jj_s = j_w_s + tidw;

	i_w_s = thread_warp_i_s[wid_csr];

	/* The CSR segment has non-empty rows due to sorting!
	 * Only the first element can be on a row start.
	 * Ignore new row for first thread in warp to have a correct scan reduce.
	 */
	col = ja[jj_s];
	new_row = ((tidw > 0) && (col & 0x80000000)) ? 1 : 0;
	i = warp_scan_reduce_inclusive(g, new_row) + i_w_s;
	// if (wid_csr == 0)
		// printf("%2d: i=%d\n", tid_csr, i);

	sum = ((ValueType) a[jj_s]) * x[col & 0x7FFFFFFF];
	for (k=1,jj=jj_s+g.size();k<nnz_per_thread;k++,jj+=g.size())
	{
		col = ja[jj];
		sum = __fma_rn((ValueType) a[jj], x[col], sum);
	}


	g.match_all(i, single_row);   // 'single_row' is passed as reference!!! Passing as pointer gives compilation error.
	if (single_row)
	{
		sum = reduce_warp_single_row(g, sum);
		if (tidw == 0)
			atomicAdd(&y[i], sum);
		// invoke_one(g, [y, i, sum]() {
			// atomicAdd(&y[i], sum);
		// });
	}
	else
	{
		reduce_warp(g, i, sum, y);
	}
}


__device__
void
spmv_sell(const int tid, INT_T * row_cluster_ptr, INT_T * ja, ValueTypeStored * a, INT_T m, INT_T n, INT_T nnz, ValueType * restrict x, ValueType * restrict y)
{
	extern __shared__ char sm[];
	thread_block_tile<32> g = tiled_partition<32>(this_thread_block());
	const int tidw = g.thread_rank();
	const int wid = tid / g.size();
	double sum;
	__attribute__((unused)) INT_T i, j, j_s, j_e, c;
	i = tid;
	j_s = row_cluster_ptr[wid] + tidw;
	j_e = row_cluster_ptr[wid+1];
	sum = 0;
	for (j=j_s;j<j_e;j+=g.size())
	{
		sum = __fma_rn((ValueType) a[j], x[ja[j]], sum);
	}
	y[i] = sum;
}


__global__
void
gpu_kernel_sell_sorted(INT_T crossover_row, INT_T crossover_offset,
		INT_T * thread_warp_i_s,
		INT_T * row_cluster_ptr, INT_T * ja, ValueTypeStored * a, INT_T m, INT_T n, INT_T nnz, ValueType * restrict x, ValueType * restrict y)
{
	const int tid = cuda_get_thread_num_bc();
	// const int tid = blockIdx.x * BLOCK_SIZE + threadIdx.x;
	if (tid < crossover_row)
		spmv_sell(tid, row_cluster_ptr, ja, a, m, n, nnz, x, y);
	else
	{
		spmv_csr<NNZ_PER_THREAD>(tid, crossover_row, crossover_offset, thread_warp_i_s, ja, a, m, n, nnz, x, y);
	}
}


void
compute_sell_sorted(SELLArrays * restrict csr, ValueType * restrict x, ValueType * restrict y)
{
	dim3 block_dims(BLOCK_SIZE);
	dim3 grid_dims(csr->num_thread_blocks);
	long shared_mem_size = 0;
	// shared_mem_size = BLOCK_SIZE * (sizeof(ValueType));
	// shared_mem_size = BLOCK_SIZE * (sizeof(ValueType) + sizeof(INT_T));
	// shared_mem_size = BLOCK_SIZE * NNZ_PER_THREAD * sizeof(INT_T);
	// shared_mem_size = BLOCK_SIZE * NNZ_PER_THREAD * (sizeof(ValueType) + sizeof(INT_T));

	if (csr->x == NULL)
	{
		printf("Grid : {%d, %d, %d} blocks. Blocks : {%d, %d, %d} threads. Shared memory : %ld.\n", grid_dims.x, grid_dims.y, grid_dims.z, block_dims.x, block_dims.y, block_dims.z, shared_mem_size);
		csr->x = x;
		memcpy(csr->x_h, x, csr->n * sizeof(ValueType));
		cuda_assert(cudaMemcpy(csr->x_d, csr->x_h, csr->n * sizeof(*csr->x_d), cudaMemcpyHostToDevice));
	}

	cudaMemset(&csr->y_d[csr->crossover_row], 0, (csr->m - csr->crossover_row) * sizeof(csr->y_d));

	// cudaFuncCachePreferNone:   no preference for shared memory or L1 (default);
	// cudaFuncCachePreferShared: prefer larger shared memory and smaller L1 cache;
	// cudaFuncCachePreferL1:     prefer larger L1 cache and smaller shared memory;
	// cuda_assert(cudaFuncSetCacheConfig(gpu_kernel_sell_sorted, cudaFuncCachePreferL1));
	// cuda_assert(cudaFuncSetCacheConfig(gpu_kernel_sell_sorted, cudaFuncCachePreferShared));
	gpu_kernel_sell_sorted<<<grid_dims, block_dims, shared_mem_size>>>(
			csr->crossover_row, csr->row_ptr_h[csr->crossover_row],
			csr->thread_warp_i_s_d,
			csr->row_cluster_ptr_d, csr->ja_d, csr->a_d, csr->m, csr->n, csr->nnz_extended, csr->x_d, csr->y_d);
	cuda_assert(cudaPeekAtLastError());
	cuda_assert(cudaDeviceSynchronize());

	// exit(0);

	if (csr->y == NULL)
	{
		csr->y = y;

		cuda_assert(cudaMemcpy(csr->y_h, csr->y_d, csr->m * sizeof(*csr->y_d), cudaMemcpyDeviceToHost));

		for (long i=0;i<csr->m;i++)
		{
			y[i] = csr->y_h[csr->row_permutation[i]];
		}
		// memcpy(y, csr->y_h, csr->m * sizeof(ValueType));

	}
}


//==========================================================================================================================================
//= Print Statistics
//==========================================================================================================================================


void
SELLArrays::statistics_start()
{
}


int
statistics_print_labels(__attribute__((unused)) char * buf, __attribute__((unused)) long buf_n)
{
	return 0;
}


int
SELLArrays::statistics_print_data(__attribute__((unused)) char * buf, __attribute__((unused)) long buf_n)
{
	return 0;
}

