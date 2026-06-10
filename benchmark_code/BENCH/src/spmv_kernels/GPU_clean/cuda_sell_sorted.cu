#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

#include <cuda.h>
#include <cooperative_groups.h>
#include <cuda_pipeline_primitives.h>

#include "macros/cpp_defines.h"

#include "../spmv_kernel.h"


#define ValueType  ValueType
// #define ValueType  float


#ifndef BLOCK_SIZE
	// #define BLOCK_SIZE  32
	// #define BLOCK_SIZE  64
	#define BLOCK_SIZE  128
	// #define BLOCK_SIZE  256
	// #define BLOCK_SIZE  512
	// #define BLOCK_SIZE  1024
#endif


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
	#define FUNCTOOLS_GEN_SUFFIX  _CUDA_CSR_TRANSPOSE_EXPAND_ROWS_CU
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
	#define BUCKETSORT_GEN_SUFFIX  _CUDA_CSR_TRANSPOSE_EXPAND_ROWS_CU
	#include "sort/bucketsort/bucketsort_gen.c"
	static inline
	INT_T
	bucketsort_find_bucket(INT_T * A, long i, __attribute__((unused)) INT_T * degree_max_ptr)
	{
		return A[i+1] - A[i];   // Ascending order.
		// return *degree_max_ptr - (A[i+1] - A[i]);   // Descending order.
	}

	struct samplesort_csr_s {
		INT_T * row_ptr;
		INT_T * ja;
		// ValueType * a;
	};
	#include "sort/samplesort/samplesort_gen_undef.h"
	#define SAMPLESORT_GEN_TYPE_1  INT_T
	#define SAMPLESORT_GEN_TYPE_2  INT_T
	#define SAMPLESORT_GEN_TYPE_3  int
	#define SAMPLESORT_GEN_TYPE_4  struct samplesort_csr_s
	#define SAMPLESORT_GEN_FUNCTION_ATTRIBUTES
	#define SAMPLESORT_GEN_SUFFIX  _CUDA_CSR_TRANSPOSE_EXPAND_ROWS_CU
	#include "sort/samplesort/samplesort_gen.c"
	static inline
	int
	samplesort_cmp(INT_T a, INT_T b, struct samplesort_csr_s * csr)
	{
		__attribute__((unused)) INT_T j_s_a=csr->row_ptr[a], j_e_a=csr->row_ptr[a+1];
		__attribute__((unused)) INT_T j_s_b=csr->row_ptr[b], j_e_b=csr->row_ptr[b+1];
		__attribute__((unused)) INT_T row_cluster_a = a / BLOCK_SIZE, row_cluster_b = b / BLOCK_SIZE;
		__attribute__((unused)) INT_T col_s_a = csr->ja[j_s_a], col_e_a = csr->ja[j_e_a];
		__attribute__((unused)) INT_T col_s_b = csr->ja[j_s_b], col_e_b = csr->ja[j_e_b];
		__attribute__((unused)) INT_T col_center_a = (col_s_a + col_e_a) / 2;
		__attribute__((unused)) INT_T col_center_b = (col_s_b + col_e_b) / 2;
		__attribute__((unused)) INT_T degree_a = j_e_a - j_s_a;
		__attribute__((unused)) INT_T degree_b = j_e_b - j_s_b;
		__attribute__((unused)) double col_median_a = 0, col_median_b = 0;
		__attribute__((unused)) double col_mean_a = 0, col_mean_b = 0;
		__attribute__((unused)) INT_T col_closer_to_diag_a = 0, col_closer_to_diag_b = 0;
		INT_T dist_min, dist;
		long j;
		if (degree_a != 0)
		{
			array_seg_quantile_serial(csr->ja, j_s_a, j_e_a, 0.5, NULL, &col_median_a, idx_to_double);
			array_seg_mean_serial(csr->ja, j_s_a, j_e_a, &col_mean_a, idx_to_double);
		}
		if (degree_b != 0)
		{
			array_seg_quantile_serial(csr->ja, j_s_b, j_e_b, 0.5, NULL, &col_median_b, idx_to_double);
			array_seg_mean_serial(csr->ja, j_s_b, j_e_b, &col_mean_b, idx_to_double);
		}
		if (degree_a != 0)
		{
			col_closer_to_diag_a = col_s_a;
			dist_min = abs(col_s_a - a);
			for (j=j_s_a;j<j_e_a;j++)
			{
				dist = abs(csr->ja[j] - a);
				if (dist < dist_min)
				{
					dist_min = dist;
					col_closer_to_diag_a = csr->ja[j];
				}
			}
		}
		if (degree_b != 0)
		{
			col_closer_to_diag_b = col_s_b;
			dist_min = abs(col_s_b - b);
			for (j=j_s_b;j<j_e_b;j++)
			{
				dist = abs(csr->ja[j] - b);
				if (dist < dist_min)
				{
					dist_min = dist;
					col_closer_to_diag_b = csr->ja[j];
				}
			}
		}
		int ret = 0;
		// if (ret == 0) ret = (row_cluster_a > row_cluster_b) ? 1 : (row_cluster_a < row_cluster_b) ? -1 : 0;
		if (ret == 0) ret = (degree_a > degree_b) ? 1 : (degree_a < degree_b) ? -1 : 0;
		// if (ret == 0) ret = (col_e_a > col_e_b) ? 1 : (col_e_a < col_e_b) ? -1 : 0;
		if (ret == 0) ret = (col_s_a > col_s_b) ? 1 : (col_s_a < col_s_b) ? -1 : 0;
		// if (ret == 0) ret = (col_closer_to_diag_a > col_closer_to_diag_b) ? 1 : (col_closer_to_diag_a < col_closer_to_diag_b) ? -1 : 0;
		// if (ret == 0) ret = (col_center_a > col_center_b) ? 1 : (col_center_a < col_center_b) ? -1 : 0;
		// if (ret == 0) ret = (col_median_a > col_median_b) ? 1 : (col_median_a < col_median_b) ? -1 : 0;
		// if (ret == 0) ret = (col_mean_a > col_mean_b) ? 1 : (col_mean_a < col_mean_b) ? -1 : 0;
		if (ret == 0) ret = a > b ? 1 : a < b ? -1 : 0;
		return ret;
		// return -ret;  // Descending order.
	}

#ifdef __cplusplus
}
#endif


namespace cg = cooperative_groups;

using namespace cooperative_groups;


void
cuda_push_duplicate_base(void ** dst_ptr, void * src, long bytes)
{
	cudaMalloc(dst_ptr, bytes);
	cudaMemcpy(*((char **) dst_ptr), src, bytes, cudaMemcpyHostToDevice);
}
#define cuda_push_duplicate(dst_ptr, src, bytes) cuda_push_duplicate_base((void **) dst_ptr, src, bytes)


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


struct SELLArrays : Matrix_Format
{
	INT_T crossover_row;

	long m_extended;
	long num_row_clusters;
	long nnz_extended;

	ValueType * a;

	INT_T * row_ptr_h;
	INT_T * row_cluster_ptr_h;
	INT_T * ja_h;
	ValueType * a_h;

	INT_T * row_ptr_d;
	INT_T * row_cluster_ptr_d;
	INT_T * ja_d;
	ValueType * a_d;

	ValueType * x = NULL;
	ValueType * y = NULL;
	ValueType * x_h = NULL;
	ValueType * y_h = NULL;
	ValueType * x_d = NULL;
	ValueType * y_d = NULL;

	int num_threads;
	int thread_block_size;
	int num_thread_blocks;
	int num_thread_warps;

	INT_T * row_permutation = NULL;

	SELLArrays(INT_T * row_ptr, INT_T * ja, ValueTypeReference * a_ref, long m, long n, long nnz) : Matrix_Format(m, n, nnz)
	{
		__attribute__((unused)) long enable_legend = 1;
		__attribute__((unused)) long num_pixels_x = 1080;
		__attribute__((unused)) long num_pixels_y = 1080;
		long i;

		cuda_device_print_attributes();

		// Convert values from ValueTypeReference (double) to ValueType (e.g., float).
		a = (typeof(a)) malloc(nnz * sizeof(*a));
		#pragma omp parallel for
		for (long i = 0; i < nnz; i++)
			a[i] = (ValueType) a_ref[i];

		thread_block_size = BLOCK_SIZE;
		if (thread_block_size % 32)
			error("select a thread block size that is a multiple of 32");

		num_thread_blocks = (m + BLOCK_SIZE - 1) / BLOCK_SIZE;
		num_threads = num_thread_blocks * BLOCK_SIZE;
		num_thread_warps = num_threads / 32;
		printf("num_threads=%d, thread_block_size=%d, num_thread_blocks=%d\n", num_threads, BLOCK_SIZE, num_thread_blocks);

		m_extended = num_threads;
		printf("m=%ld , m_extended=%ld\n", m, m_extended);
		num_row_clusters = num_thread_warps;
		row_ptr_h = (typeof(row_ptr_h)) malloc((m_extended+1) * sizeof(*row_ptr_h));
		row_cluster_ptr_h = (typeof(row_cluster_ptr_h)) malloc((num_row_clusters+1) * sizeof(*row_cluster_ptr_h));
		_Pragma("omp parallel")
		{
			long i;
			_Pragma("omp for")
			for (i=0;i<m;i++)
			{
				row_ptr_h[i] = row_ptr[i];
			}
		}
		for (i=m;i<=m_extended;i++)
			row_ptr_h[i] = row_ptr[m];

		/* Sort rows. */
		row_permutation = (typeof(row_permutation)) malloc(m_extended * sizeof(*row_permutation));
		_Pragma("omp parallel")
		{
			long i;
			_Pragma("omp for")
			for (i=0;i<m_extended;i++)
				row_permutation[i] = i;
		}
		double time_sort_rows = time_it(1,
			// csr_plot("matrix", row_ptr_h, ja, a, m_extended, n, nnz, enable_legend, num_pixels_x, num_pixels_y);

			double max;
			array_max(row_ptr_h, m_extended, &max, NULL, row_ptr_to_degree_double);
			INT_T degree_max = max;
			printf("degree_max=%d\n", degree_max);
			// bucketsort_stable_recalculate_bucket_serial(row_ptr_h, m_extended, degree_max+1, &degree_max, row_permutation, NULL);
			INT_T * rev_row_permutation = (typeof(rev_row_permutation)) malloc(m_extended * sizeof(*rev_row_permutation));
			_Pragma("omp parallel")
			{
				long i;
				_Pragma("omp for")
				for (i=0;i<m_extended;i++)
					rev_row_permutation[i] = i;
			}
			struct samplesort_csr_s tmp_csr = { .row_ptr=row_ptr_h, .ja=ja };
			samplesort(rev_row_permutation, m_extended, &tmp_csr);
			for (i=0;i<m_extended;i++)
				row_permutation[rev_row_permutation[i]] = i;
			free(rev_row_permutation);

			INT_T * reordered_row_ptr = (typeof(reordered_row_ptr)) malloc((m_extended+1) * sizeof(*reordered_row_ptr));
			INT_T * reordered_col_idx = (typeof(reordered_col_idx)) malloc(nnz * sizeof(*reordered_col_idx));
			ValueType * reordered_values = (typeof(reordered_values)) malloc(nnz * sizeof(*reordered_values));
			csr_reorder_rows(row_permutation, row_ptr_h, ja, a, m_extended, n, nnz, reordered_row_ptr, reordered_col_idx, reordered_values);
			free(row_ptr_h);
			row_ptr_h = reordered_row_ptr;
			ja_h = reordered_col_idx;
			a_h = reordered_values;

			// csr_plot("matrix_reordered", row_ptr_h, ja_h, a_h, m_extended, n, nnz, enable_legend, num_pixels_x, num_pixels_y);

		);
		printf("time sort rows = %g\n", time_sort_rows);


		INT_T degree_avg = nnz / m;
		INT_T degree_max = row_ptr_h[m_extended] - row_ptr_h[m_extended-1];
		INT_T crossover_ratio = 5;
		crossover_row = m_extended;
		if (degree_max / degree_avg > crossover_ratio)
		{
			for (i=0;i<m_extended;i++)
			{
				long degree = row_ptr_h[i+1] - row_ptr_h[i];
				if (degree / degree_avg > crossover_ratio)
					break;
			}
			crossover_row = i;
		}

		/* Extend row clusters to local max row. 
		 * Transpose row clusters.
		 */
		INT_T * row_ptr_h_new = (typeof(row_ptr_h_new)) malloc((m_extended+1) * sizeof(*row_ptr_h_new));
		INT_T * ja_h_new;
		ValueType * a_h_new;
		_Pragma("omp parallel")
		{
			long i, j1, j2, k;
			long degree;
			long degree_cluster_max;
			_Pragma("omp for")
			for (i=0;i<m_extended;i+=32)
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
			_Pragma("omp single")
			{
				row_ptr_h_new[m_extended] = 0;
			}
			scan_reduce_concurrent(row_ptr_h_new, row_ptr_h_new, m_extended+1, 0, 1, 0);
			_Pragma("omp single")
			{
				nnz_extended = row_ptr_h_new[m_extended];
				ja_h_new = (typeof(ja_h_new)) malloc(nnz_extended * sizeof(*ja_h_new));
				a_h_new = (typeof(a_h_new)) malloc(nnz_extended * sizeof(*a_h_new));
			}
			_Pragma("omp for")
			for (i=0;i<m_extended;i+=32)
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
				transpose(&ja_h_new[row_ptr_h_new[i]], 32, degree);
				transpose(&a_h_new[row_ptr_h_new[i]], 32, degree);
				row_cluster_ptr_h[i/32] = row_ptr_h_new[i];
			}
		}
		free(row_ptr_h);
		row_ptr_h = row_ptr_h_new;
		row_cluster_ptr_h[num_row_clusters] = row_ptr_h_new[m_extended];
		free(ja_h);
		free(a_h);
		ja_h = ja_h_new;
		a_h = a_h_new;
		printf("nnz=%ld, nnz_extended=%ld\n", nnz, nnz_extended);

		cuda_assert(cudaMalloc(&row_ptr_d, (m_extended+1) * sizeof(*row_ptr_d)));
		cuda_assert(cudaMalloc(&row_cluster_ptr_d, (num_row_clusters+1) * sizeof(*row_cluster_ptr_d)));
		cuda_assert(cudaMalloc(&ja_d, nnz_extended * sizeof(*ja_d)));
		cuda_assert(cudaMalloc(&a_d, nnz_extended * sizeof(*a_d)));
		cuda_assert(cudaMalloc(&x_d, n * sizeof(*x_d)));
		cuda_assert(cudaMalloc(&y_d, m_extended * sizeof(*y_d)));

		x_h = (typeof(x_h)) malloc(n * sizeof(*x_h));
		y_h = (typeof(y_h)) malloc(m_extended * sizeof(*y_h));

		cuda_assert(cudaMemcpy(row_ptr_d, row_ptr_h, (m_extended+1) * sizeof(*row_ptr_d), cudaMemcpyHostToDevice));
		cuda_assert(cudaMemcpy(row_cluster_ptr_d, row_cluster_ptr_h, (num_row_clusters+1) * sizeof(*row_cluster_ptr_d), cudaMemcpyHostToDevice));
		cuda_assert(cudaMemcpy(ja_d, ja_h, nnz_extended * sizeof(*ja_d), cudaMemcpyHostToDevice));
		cuda_assert(cudaMemcpy(a_d, a_h, nnz_extended * sizeof(*a_d), cudaMemcpyHostToDevice));
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
	csr->mem_footprint = csr->nnz_extended * (sizeof(ValueType) + sizeof(INT_T)) + (csr->m_extended+1) * sizeof(INT_T);
	char *format_name;
	format_name = (char *)malloc(100*sizeof(char));
	snprintf(format_name, 100, "Custom_CSR_CUDA_sell_sorted_b%d", BLOCK_SIZE);
	csr->format_name = format_name;
	return csr;
}


//==========================================================================================================================================
//= CSR Custom
//==========================================================================================================================================


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
spmv_const_nnz_per_thread(INT_T i, INT_T * row_ptr, INT_T * ja, ValueType * a, long m, long n, long nnz, ValueType * restrict x, ValueType * restrict y)
{
	extern __shared__ char sm[];
	thread_block_tile<32> g = tiled_partition<32>(this_thread_block());
	const int tid = cuda_get_thread_num_bc();
	const int tidw = g.thread_rank();
	const int wid = tid / 32;
	double sum;
	__attribute__((unused)) int j, jj, j_s, j_e, j_w_s, k;
	j_w_s = row_ptr[i] + wid * 32 * nnz_per_thread;
	sum = 0;
	for (k=0,jj=j_w_s+tidw;k<nnz_per_thread;k++,jj+=g.size())
	{
		sum = __fma_rn((ValueType) a[jj], x[ja[jj]], sum);
	}
	sum = reduce_warp_single_row(g, sum);
	if (tidw == 0)
		atomicAdd(&y[i], sum);
}


__device__
void
spmv_full_block_hybrid(INT_T * row_cluster_ptr, INT_T * ja, ValueType * a, long m, long n, long nnz, ValueType * restrict x, ValueType * restrict y)
{
	extern __shared__ char sm[];
	thread_block_tile<32> g = tiled_partition<32>(this_thread_block());
	const int tid = cuda_get_thread_num_bc();
	const int tidw = g.thread_rank();
	const int wid = tid / g.size();
	double sum;
	__attribute__((unused)) int i, j, j_s, j_e, c;
	i = tid;
	j_s = row_cluster_ptr[wid] + tidw;
	j_e = row_cluster_ptr[wid+1];
	sum = 0;
	y[i] = 0;
	for (j=j_s;j<j_e;j+=g.size())
	{
		sum = __fma_rn((ValueType) a[j], x[ja[j]], sum);
	}
	y[i] = sum;
}


__device__
void
spmv_full_block(INT_T * row_cluster_ptr, INT_T * ja, ValueType * a, long m, long n, long nnz, ValueType * restrict x, ValueType * restrict y)
{
	extern __shared__ char sm[];
	thread_block_tile<32> g = tiled_partition<32>(this_thread_block());
	const int tid = cuda_get_thread_num_bc();
	const int tidw = g.thread_rank();
	const int wid = tid / g.size();
	double sum;
	__attribute__((unused)) int i, j, j_s, j_e, c;
	i = tid;
	j_s = row_cluster_ptr[wid] + tidw;
	j_e = row_cluster_ptr[wid+1];
	sum = 0;
	for (j=j_s;j<j_e;j+=g.size())
	{
		// sum = __fma_rn((ValueType) a[j], x[ja[j]], sum);
		sum += a[j] * x[ja[j]];
	}
	y[i] = sum;
}


__global__
void
gpu_kernel_sell_sorted(INT_T * row_cluster_ptr, INT_T * ja, ValueType * a, long m, long n, long nnz, ValueType * restrict x, ValueType * restrict y)
{
	spmv_full_block(row_cluster_ptr, ja, a, m, n, nnz, x, y);
	// spmv_full_block_hybrid(row_cluster_ptr, ja, a, m, n, nnz, x, y);
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

	// cudaMemset(csr->y_d, 0, csr->m_extended * sizeof(csr->y_d));

	// cudaFuncCachePreferNone:   no preference for shared memory or L1 (default);
	// cudaFuncCachePreferShared: prefer larger shared memory and smaller L1 cache;
	// cudaFuncCachePreferL1:     prefer larger L1 cache and smaller shared memory;
	// cuda_assert(cudaFuncSetCacheConfig(gpu_kernel_sell_sorted, cudaFuncCachePreferL1));
	// cuda_assert(cudaFuncSetCacheConfig(gpu_kernel_sell_sorted, cudaFuncCachePreferShared));
	gpu_kernel_sell_sorted<<<grid_dims, block_dims, shared_mem_size>>>(csr->row_cluster_ptr_d, csr->ja_d, csr->a_d, csr->m_extended, csr->n, csr->nnz_extended, csr->x_d, csr->y_d);
	cuda_assert(cudaPeekAtLastError());
	cuda_assert(cudaDeviceSynchronize());

	// exit(0);

	if (csr->y == NULL)
	{
		csr->y = y;

		cuda_assert(cudaMemcpy(csr->y_h, csr->y_d, csr->m_extended * sizeof(*csr->y_d), cudaMemcpyDeviceToHost));

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

