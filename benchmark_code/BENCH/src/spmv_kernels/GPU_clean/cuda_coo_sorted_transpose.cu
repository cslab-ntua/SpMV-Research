#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

#include <cuda.h>
#include <cooperative_groups.h>

#include "macros/cpp_defines.h"

#include "../spmv_kernel.h"

#ifdef __cplusplus
extern "C"{
#endif
	#include "macros/macrolib.h"
	#include "time_it.h"
	#include "parallel_util.h"
	#include "array_metrics.h"

	#include "cuda/cuda_util.h"
#ifdef __cplusplus
}
#endif


using namespace cooperative_groups;

#ifndef NNZ_PER_THREAD
	#define NNZ_PER_THREAD  5
#endif

#ifndef BLOCK_SIZE
	// #define BLOCK_SIZE  32
	// #define BLOCK_SIZE  64
	#define BLOCK_SIZE  128
	// #define BLOCK_SIZE  256
	// #define BLOCK_SIZE  512
	// #define BLOCK_SIZE  1024
#endif

#ifndef TIME_IT
	#define TIME_IT 0
#endif


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


struct COOArrays : Matrix_Format
{
	INT_T * row_ptr_h;
	INT_T * ia_h;
	INT_T * ja_h;
	ValueType * a_h;

	INT_T * row_ptr_d;
	INT_T * ia_d;
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

	COOArrays(INT_T * row_ptr, INT_T * ja, ValueTypeReference * a, long m, long n, long nnz) : Matrix_Format(m, n, nnz)
	{
		double time;
		const long nnz_per_block = BLOCK_SIZE * NNZ_PER_THREAD;

		cuda_device_print_attributes();

		thread_block_size = BLOCK_SIZE;

		num_threads = (nnz + NNZ_PER_THREAD - 1) / NNZ_PER_THREAD;
		num_thread_blocks = (num_threads + BLOCK_SIZE - 1) / BLOCK_SIZE;
		num_threads = num_thread_blocks * BLOCK_SIZE;
		printf("num_threads=%d, thread_block_size=%d, num_thread_blocks=%d\n", num_threads, BLOCK_SIZE, num_thread_blocks);

		cuda_assert(cudaMalloc(&row_ptr_d, (m+1) * sizeof(*row_ptr_d)));
		cuda_assert(cudaMalloc(&ia_d, nnz * sizeof(*ia_d)));
		cuda_assert(cudaMalloc(&ja_d, nnz * sizeof(*ja_d)));
		cuda_assert(cudaMalloc(&a_d, nnz * sizeof(*a_d)));
		cuda_assert(cudaMalloc(&x_d, n * sizeof(*x_d)));
		cuda_assert(cudaMalloc(&y_d, m * sizeof(*y_d)));

		cuda_assert(cudaMallocHost(&row_ptr_h, (m+1) * sizeof(INT_T)));
		cuda_assert(cudaMallocHost(&ia_h, nnz * sizeof(INT_T)));
		cuda_assert(cudaMallocHost(&ja_h, nnz * sizeof(INT_T)));
		cuda_assert(cudaMallocHost(&a_h, nnz * sizeof(ValueType)));
		cuda_assert(cudaMallocHost(&x_h, n * sizeof(ValueType)));
		cuda_assert(cudaMallocHost(&y_h, m * sizeof(ValueType)));

		memcpy(row_ptr_h, row_ptr, (m + 1) * sizeof(INT_T));
		memcpy(ja_h, ja, nnz * sizeof(INT_T));
		memcpy(a_h, a, nnz * sizeof(ValueType));

		time = time_it(1,
			_Pragma("omp parallel")
			{
				long i, j, j_s, j_e;
				_Pragma("omp for")
				for (i=0;i<m;i++)
				{
					j_s = row_ptr[i];
					j_e = row_ptr[i+1];
					for (j=j_s;j<j_e;j++)
					{
						ia_h[j] = i;
					}
				}
			}
		);
		printf("find row indices = %g\n", time);

		_Pragma("omp parallel")
		{
			long i_s, i_e, j, jj;
			_Pragma("omp for")
			for (j=0;j<nnz;j+=32*NNZ_PER_THREAD)
			{
				long j_e = j + 32*NNZ_PER_THREAD;
				if (j_e > nnz)
					j_e = nnz;
				macros_binary_search(row_ptr, 0, m, j, &i_s, NULL);           // Index boundaries are inclusive.
				macros_binary_search(row_ptr, 0, m, j_e-1, &i_e, NULL);           // Index boundaries are inclusive.
				if (i_s == i_e)
				{
					for (jj=j;jj<j_e;jj++)
					{
						ja_h[jj] = ja_h[jj] | 0x80000000;
					}
				}
			}
		}

		_Pragma("omp parallel")
		{
			long j;
			_Pragma("omp for")
			for (j=0;j<(num_thread_blocks-1)*nnz_per_block;j+=32*NNZ_PER_THREAD)
			{
				long j_e = j + 32*NNZ_PER_THREAD;
				if (j_e > nnz)
					j_e = nnz;
				if (j_e < nnz)
				{
					transpose(&a_h[j], 32, NNZ_PER_THREAD);
					transpose(&ia_h[j], 32, NNZ_PER_THREAD);
					transpose(&ja_h[j], 32, NNZ_PER_THREAD);
				}
			}
		}

		// _Pragma("omp parallel")
		// {
			// long j;
			// _Pragma("omp for")
			// for (j=0;j<(num_thread_blocks-1)*nnz_per_block;j+=BLOCK_SIZE*NNZ_PER_THREAD)
			// {
				// long j_e = j + BLOCK_SIZE*NNZ_PER_THREAD;
				// if (j_e > nnz)
					// j_e = nnz;
				// if (j_e < nnz)
				// {
					// transpose(&a_h[j], BLOCK_SIZE, NNZ_PER_THREAD);
					// transpose(&ia_h[j], BLOCK_SIZE, NNZ_PER_THREAD);
					// transpose(&ja_h[j], BLOCK_SIZE, NNZ_PER_THREAD);
				// }
			// }
		// }

		cuda_assert(cudaMemcpy(row_ptr_d, row_ptr_h, (m+1) * sizeof(*row_ptr_d), cudaMemcpyHostToDevice));
		cuda_assert(cudaMemcpy(ia_d, ia_h, nnz * sizeof(*ia_d), cudaMemcpyHostToDevice));
		cuda_assert(cudaMemcpy(ja_d, ja_h, nnz * sizeof(*ja_d), cudaMemcpyHostToDevice));
		cuda_assert(cudaMemcpy(a_d, a_h, nnz * sizeof(*a_d), cudaMemcpyHostToDevice));
	}

	~COOArrays()
	{
		cuda_assert(cudaFree(row_ptr_d));
		cuda_assert(cudaFree(ia_d));
		cuda_assert(cudaFree(ja_d));
		cuda_assert(cudaFree(a_d));
		cuda_assert(cudaFree(x_d));
		cuda_assert(cudaFree(y_d));

		cuda_assert(cudaFreeHost(row_ptr_h));
		cuda_assert(cudaFreeHost(ia_h));
		cuda_assert(cudaFreeHost(ja_h));
		cuda_assert(cudaFreeHost(a_h));
		cuda_assert(cudaFreeHost(x_h));
		cuda_assert(cudaFreeHost(y_h));
	}

	void spmv(ValueType * x, ValueType * y);
	void statistics_start();
	int statistics_print_data(__attribute__((unused)) char * buf, __attribute__((unused)) long buf_n);
};


void compute_coo(COOArrays * restrict coo, ValueType * restrict x , ValueType * restrict y);

void
COOArrays::spmv(ValueType * x, ValueType * y)
{
	compute_coo(this, x, y);
}


struct Matrix_Format *
csr_to_format(INT_T * row_ptr, INT_T * col_ind, ValueTypeReference * values, long m, long n, long nnz, long symmetric, long symmetry_expanded)
{
	if (symmetric && !symmetry_expanded)
		error("symmetric matrices not supported by this format, expand symmetry");
	struct COOArrays * coo = new COOArrays(row_ptr, col_ind, values, m, n, nnz);
	// for (long i=0;i<10;i++)
		// printf("%d\n", row_ptr[i]);
	coo->mem_footprint = nnz * (sizeof(ValueType) + sizeof(INT_T)) + (m+1) * sizeof(INT_T);
	char *format_name;
	format_name = (char *)malloc(100*sizeof(char));
	snprintf(format_name, 100, "Custom_COO_CUDA_constant_nnz_per_thread_b%d_nnz%d", BLOCK_SIZE, NNZ_PER_THREAD);
	coo->format_name = format_name;
	return coo;
}


//==========================================================================================================================================
//= COO Custom
//==========================================================================================================================================


template<typename T>
__device__
T
binary_search_gpu(T * A, long s, long e, T target)
{
	long m;
	while (1)
	{
		m = (s + e) / 2;
		if (m == s || m == e)
			break;
		if (target > A[m])
			s = m;
		else
			e = m;
	}
	if (target == A[e])
		return e;
	return s;
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


/* // Threads may only read data from another thread which is actively participating in the __shfl_sync() command.
// If the target thread is inactive, the retrieved value is undefined.
template <typename group_t>
__device__
void
reduce_warp(group_t g, INT_T row, ValueType val, ValueType * restrict y)
{
	const int tidl = g.thread_rank();   // Group lane.
	const int tidl_one_hot = 1 << tidl;
	int flag;
	INT_T row_prev;
	ValueType val_prev;
	flag = 0xaaaaaaaa; // 10101010101010101010101010101010
	row_prev = g.shfl_up(row, 1); // __shfl_sync(flag, row, tidl-1);
	val_prev = g.shfl_up(val, 1); // __shfl_sync(flag, val, tidl-1);
	if (tidl_one_hot & flag)
	{
		if (row == row_prev)
		{
			val += val_prev;
		}
		else
		{
			atomicAdd(&y[row_prev], val_prev);
		}
	}
	flag = 0x88888888; // 10001000100010001000100010001000
	row_prev = g.shfl_up(row, 2); // __shfl_sync(flag, row, tidl-2);
	val_prev = g.shfl_up(val, 2); // __shfl_sync(flag, val, tidl-2);
	if (tidl_one_hot & flag)
	{
		if (row == row_prev)
		{
			val += val_prev;
		}
		else
		{
			atomicAdd(&y[row_prev], val_prev);
		}
	}
	flag = 0x80808080; // 10000000100000001000000010000000
	row_prev = g.shfl_up(row, 4); // __shfl_sync(flag, row, tidl-4);
	val_prev = g.shfl_up(val, 4); // __shfl_sync(flag, val, tidl-4);
	if (tidl_one_hot & flag)
	{
		if (row == row_prev)
		{
			val += val_prev;
		}
		else
		{
			atomicAdd(&y[row_prev], val_prev);
		}
	}
	flag = 0x80008000; // 10000000000000001000000000000000
	row_prev = g.shfl_up(row, 8); // __shfl_sync(flag, row, tidl-8);
	val_prev = g.shfl_up(val, 8); // __shfl_sync(flag, val, tidl-8);
	if (tidl_one_hot & flag)
	{
		if (row == row_prev)
		{
			val += val_prev;
		}
		else
		{
			atomicAdd(&y[row_prev], val_prev);
		}
	}
	flag = 0x80000000; // 10000000000000000000000000000000
	row_prev = g.shfl_up(row, 16); // __shfl_sync(flag, row, tidl-16);
	val_prev = g.shfl_up(val, 16); // __shfl_sync(flag, val, tidl-16);
	if (tidl_one_hot & flag)
	{
		if (row == row_prev)
		{
			val += val_prev;
		}
		else
		{
			atomicAdd(&y[row_prev], val_prev);
		}
	}
	g.sync();
	if (tidl == 31)
		atomicAdd(&y[row], val);
} */


inline
__device__
void
reduce_block(INT_T row, ValueType val, ValueType * restrict y)
{
	thread_block_tile<32> tile32 = tiled_partition<32>(this_thread_block());
	reduce_warp(tile32, row, val, y);
}


__device__
void
spmv_last_block(INT_T * ia, INT_T * ja, ValueType * a, long m, long n, long nnz, ValueType * restrict x, ValueType * restrict y)
{
	// extern __shared__ char sm[];
	const int tid = cuda_get_thread_num_bc();
	const int tidb = threadIdx.x;
	const int block_id = blockIdx.x;
	const int nnz_per_block = BLOCK_SIZE * NNZ_PER_THREAD;
	// ValueType * val_buf = (typeof(val_buf)) sm;
	__attribute__((unused)) int i, i_e, j, j_s, j_e, k;
	j_s = block_id * nnz_per_block + tidb * NNZ_PER_THREAD;
	j_e = j_s + NNZ_PER_THREAD;
	if (j_e > nnz)
		j_e = nnz;

	double sum = 0;
	if (j_s >= nnz)
		i = ia[nnz-1];
	else
		i = ia[j_s];
	for (j=j_s;j<j_e;j++)
	{
		if (ia[j] != i)
		{
			atomicAdd(&y[i], sum);
			sum = 0;
			i = ia[j];
		}
		// sum += a[j] * x[ja[j] & 0x7FFFFFFF];
		sum = __fma_rn(a[j], x[ja[j] & 0x7FFFFFFF], sum);
	}
	reduce_block(i, sum, y);
}


template <typename group_t>
__device__
ValueType
reduce_warp_single_row(group_t g, ValueType val)
{
	// Use XOR mode to perform butterfly reduction
	for (int i=g.size()/2; i>=1; i/=2)
	{
		val += g.shfl_xor(val, i); // __shfl_xor_sync(0xffffffff, val, i, g.size());   // Total sum is same on all threads.
		// val += __shfl_down_sync(0xffffffff, val, i, g.size());   // Only thread 0 has the total sum.
	}
	return val;
}


template <typename group_t>
__device__
void
spmv_warp_single_row(group_t g, int i, int j_s, int j_b_s, int j_w_s, INT_T * ja, ValueType * a, ValueType * restrict x, ValueType * restrict y)
{
	const int tidl = g.thread_rank();   // Group lane.
	int j, jj;
	double sum = 0;
	// PRAGMA(unroll NNZ_PER_THREAD)
	// for (j=j_s,jj=j_s;j<j_s+NNZ_PER_THREAD;j++,jj++)
	// for (j=j_s,jj=j_b_s+threadIdx.x;j<j_s+NNZ_PER_THREAD;j++,jj+=BLOCK_SIZE)
	for (j=j_s,jj=j_w_s+tidl;j<j_s+NNZ_PER_THREAD;j++,jj+=g.size())
	{
		sum = __fma_rn(a[jj], x[ja[jj] & 0x7FFFFFFF], sum);
	}
	sum = reduce_warp_single_row(g, sum);
	if (tidl == 0)
		atomicAdd(&y[i], sum);
}


template <typename group_t>
__device__
void
spmv_full_warp(group_t g, int single_row, int j_s, int j_b_s, int j_w_s, INT_T * ia, INT_T * ja, ValueType * a, ValueType * restrict x, ValueType * restrict y)
{
	extern __shared__ char sm[];
	const int tidl = g.thread_rank();   // Group lane.
	int i, j, jj;
	double sum = 0;
	jj=j_w_s+tidl;
	i = ia[jj];
	if (single_row)
	{
		spmv_warp_single_row(g, i, j_s, j_b_s, j_w_s, ja, a, x, y);
	}
	else
	{
		// PRAGMA(unroll NNZ_PER_THREAD)
		// for (j=j_s,jj=j_s;j<j_s+NNZ_PER_THREAD;j++,jj++)
		// for (j=j_s,jj=j_b_s+threadIdx.x;j<j_s+NNZ_PER_THREAD;j++,jj+=BLOCK_SIZE)
		for (j=j_s,jj=j_w_s+tidl;j<j_s+NNZ_PER_THREAD;j++,jj+=g.size())
		{
			if (ia[jj] != i)
			{
				atomicAdd(&y[i], sum);
				sum = 0;
				i = ia[jj];
			}
			// sum += a[jj] * x[ja[jj] & 0x7FFFFFFF];
			sum = __fma_rn(a[jj], x[ja[jj] & 0x7FFFFFFF], sum);
		}
		reduce_warp(g, i, sum, y);
	}
}


__device__
void
spmv_full_block(INT_T * ia, INT_T * ja, ValueType * a, long m, long n, long nnz, ValueType * restrict x, ValueType * restrict y)
{
	// extern __shared__ char sm[];
	// const int tidb = threadIdx.x;
	const int tid = cuda_get_thread_num_bc();
	const int tidw = threadIdx.x % 32;
	const int warp_id = threadIdx.x / 32;
	const int block_id = blockIdx.x;
	// ValueType * val_buf = (typeof(val_buf)) sm;
	// INT_T * ia_buf = (typeof(ia_buf)) &sm[BLOCK_SIZE * sizeof(ValueType)];
	// INT_T * ia_buf = (typeof(ia_buf)) sm;
	__attribute__((unused)) int i_s, i_e, j_s, j_b_s, j_w_s, k;
	j_b_s = block_id * BLOCK_SIZE * NNZ_PER_THREAD;
	j_w_s = j_b_s + warp_id * 32 * NNZ_PER_THREAD;
	j_s = j_w_s + tidw * NNZ_PER_THREAD;

	// int jj = j_b_s + threadIdx.x;
	int jj = j_s;
	int single_row = (ja[jj] & 0x80000000) ? 1 : 0;
	// int single_row = 0;
	thread_block_tile<32> tile32 = tiled_partition<32>(this_thread_block());
	spmv_full_warp(tile32, single_row, j_s, j_b_s, j_w_s, ia, ja, a, x, y);
}


__global__
void
gpu_kernel_spmv_row_indices_continuous(INT_T * ia, INT_T * ja, ValueType * a, long m, long n, long nnz, ValueType * restrict x, ValueType * restrict y)
{
	int grid_size = gridDim.x;
	int block_id = blockIdx.x;
	if (block_id == grid_size - 1)
		spmv_last_block(ia, ja, a, m, n, nnz, x, y);
	else
		spmv_full_block(ia, ja, a, m, n, nnz, x, y);
}


void
compute_coo(COOArrays * restrict coo, ValueType * restrict x, ValueType * restrict y)
{
	dim3 block_dims(BLOCK_SIZE);
	dim3 grid_dims(coo->num_thread_blocks);
	// long shared_mem_size = BLOCK_SIZE * (sizeof(ValueType));
	// long shared_mem_size = BLOCK_SIZE * (sizeof(ValueType) + sizeof(INT_T));
	// long shared_mem_size = BLOCK_SIZE * NNZ_PER_THREAD * sizeof(INT_T);
	long shared_mem_size = 0;

	if (coo->x == NULL)
	{
		printf("Grid : {%d, %d, %d} blocks. Blocks : {%d, %d, %d} threads.\n", grid_dims.x, grid_dims.y, grid_dims.z, block_dims.x, block_dims.y, block_dims.z);
		coo->x = x;
		memcpy(coo->x_h, x, coo->n * sizeof(ValueType));
		cuda_assert(cudaMemcpy(coo->x_d, coo->x_h, coo->n * sizeof(*coo->x_d), cudaMemcpyHostToDevice));
	}

	cudaMemset(coo->y_d, 0, coo->m * sizeof(coo->y_d));

	// cudaFuncCachePreferNone:   no preference for shared memory or L1 (default);
	// cudaFuncCachePreferShared: prefer larger shared memory and smaller L1 cache;
	// cudaFuncCachePreferL1:     prefer larger L1 cache and smaller shared memory;
	cuda_assert(cudaFuncSetCacheConfig(gpu_kernel_spmv_row_indices_continuous, cudaFuncCachePreferL1));
	gpu_kernel_spmv_row_indices_continuous<<<grid_dims, block_dims, shared_mem_size>>>(coo->ia_d, coo->ja_d, coo->a_d, coo->m, coo->n, coo->nnz, coo->x_d, coo->y_d);
	cuda_assert(cudaPeekAtLastError());
	cuda_assert(cudaDeviceSynchronize());

	if (coo->y == NULL)
	{
		coo->y = y;

		cuda_assert(cudaMemcpy(coo->y_h, coo->y_d, coo->m * sizeof(*coo->y_d), cudaMemcpyDeviceToHost));
		memcpy(y, coo->y_h, coo->m * sizeof(ValueType));
	}
}


//==========================================================================================================================================
//= Print Statistics
//==========================================================================================================================================


void
COOArrays::statistics_start()
{
}


int
statistics_print_labels(__attribute__((unused)) char * buf, __attribute__((unused)) long buf_n)
{
	return 0;
}


int
COOArrays::statistics_print_data(__attribute__((unused)) char * buf, __attribute__((unused)) long buf_n)
{
	return 0;
}

