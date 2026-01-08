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
	#define NNZ_PER_THREAD  6
#endif

#ifndef BLOCK_SIZE
	// #define BLOCK_SIZE  32
	// #define BLOCK_SIZE  64
	// #define BLOCK_SIZE  128
	// #define BLOCK_SIZE  256
	// #define BLOCK_SIZE  512
	#define BLOCK_SIZE  1024
#endif

#ifndef TIME_IT
	#define TIME_IT 0
#endif


double * thread_time_compute, * thread_time_barrier;

void
cuda_push_duplicate_base(void ** dst_ptr, void * src, long bytes)
{
	cudaMalloc(dst_ptr, bytes);
	cudaMemcpy(*((char **) dst_ptr), src, bytes, cudaMemcpyHostToDevice);
}
#define cuda_push_duplicate(dst_ptr, src, bytes) cuda_push_duplicate_base((void **) dst_ptr, src, bytes)


struct CSRArrays : Matrix_Format
{
	INT_T * row_ptr;
	INT_T * ia;
	INT_T * ja;
	ValueType * a;
	INT_T * thread_block_i_s = NULL;
	INT_T * thread_block_i_e = NULL;

	INT_T * row_ptr_d;
	INT_T * ia_d;
	INT_T * ja_d;
	ValueType * a_d;
	INT_T * thread_block_i_s_d = NULL;
	INT_T * thread_block_i_e_d = NULL;

	INT_T * row_ptr_h;
	INT_T * ia_h;
	INT_T * ja_h;
	ValueType * a_h;
	INT_T * thread_block_i_s_h = NULL;
	INT_T * thread_block_i_e_h = NULL;
	INT_T * thread_block_j_s_h = NULL;
	INT_T * thread_block_j_e_h = NULL;

	// ValueType * multres_d;

	ValueType * x = NULL;
	ValueType * y = NULL;
	ValueType * x_h = NULL;
	ValueType * y_h = NULL;
	ValueType * x_d = NULL;
	ValueType * y_d = NULL;

	int max_smem_per_block, multiproc_count, max_threads_per_block, warp_size, max_threads_per_multiproc, max_block_dim_x, max_persistent_l2_cache, max_num_threads;
	int num_threads;
	int thread_block_size;
	int num_thread_blocks;

	CSRArrays(INT_T * row_ptr, INT_T * ja, ValueType * a, long m, long n, long nnz) : Matrix_Format(m, n, nnz), row_ptr(row_ptr), ja(ja), a(a)
	{
		const long nnz_per_block = BLOCK_SIZE * NNZ_PER_THREAD;
		double time_balance;
		long i;

		gpuCudaErrorCheck(cudaDeviceGetAttribute(&max_smem_per_block, cudaDevAttrMaxSharedMemoryPerBlock, 0));
		gpuCudaErrorCheck(cudaDeviceGetAttribute(&multiproc_count, cudaDevAttrMultiProcessorCount, 0));
		gpuCudaErrorCheck(cudaDeviceGetAttribute(&max_threads_per_block, cudaDevAttrMaxThreadsPerBlock , 0));
		gpuCudaErrorCheck(cudaDeviceGetAttribute(&warp_size, cudaDevAttrWarpSize , 0));
		gpuCudaErrorCheck(cudaDeviceGetAttribute(&max_threads_per_multiproc, cudaDevAttrMaxThreadsPerMultiProcessor, 0));
		gpuCudaErrorCheck(cudaDeviceGetAttribute(&max_block_dim_x, cudaDevAttrMaxBlockDimX, 0));
		gpuCudaErrorCheck(cudaDeviceGetAttribute(&max_persistent_l2_cache, cudaDevAttrMaxPersistingL2CacheSize, 0));
		max_num_threads = max_threads_per_multiproc * multiproc_count;
		printf("max_smem_per_block(bytes)=%d\n", max_smem_per_block);
		printf("multiproc_count=%d\n", multiproc_count);
		printf("max_threads_per_block=%d\n", max_threads_per_block);
		printf("warp_size=%d\n", warp_size);
		printf("max_threads_per_multiproc=%d\n", max_threads_per_multiproc);
		printf("max_block_dim_x=%d\n", max_block_dim_x);
		printf("max_persistent_l2_cache=%d\n", max_persistent_l2_cache);
		printf("max_num_threads=%d\n", max_num_threads);

		thread_block_size = BLOCK_SIZE;

		num_threads = (nnz + NNZ_PER_THREAD - 1) / NNZ_PER_THREAD;

		num_threads = ((num_threads + BLOCK_SIZE - 1) / BLOCK_SIZE) * BLOCK_SIZE;

		num_thread_blocks = num_threads / BLOCK_SIZE;

		printf("num_threads=%d, thread_block_size=%d, num_thread_blocks=%d\n", num_threads, BLOCK_SIZE, num_thread_blocks);

		thread_block_i_s = (INT_T *) malloc(num_thread_blocks * sizeof(*thread_block_i_s));
		thread_block_i_e = (INT_T *) malloc(num_thread_blocks * sizeof(*thread_block_i_e));
		time_balance = time_it(1,
			_Pragma("omp parallel")
			{
				INT_T thread_block_j_s;
				long lower_boundary;
				_Pragma("omp for")
				for (i=0;i<num_thread_blocks;i++)
				{
					thread_block_j_s = nnz_per_block * i;
					if (thread_block_j_s > nnz)
						thread_block_j_s = nnz;
					macros_binary_search(row_ptr, 0, m, thread_block_j_s, &lower_boundary, NULL);           // Index boundaries are inclusive.
					thread_block_i_s[i] = lower_boundary;
				}
				_Pragma("omp for")
				for (i=0;i<num_thread_blocks;i++)
				{
					if (i == num_thread_blocks - 1)   // If we calculate each thread's boundaries individually some empty rows might be unassigned.
						thread_block_i_e[i] = m;
					else
						thread_block_i_e[i] = thread_block_i_s[i+1] + 1;
				}
			}
		);
		printf("balance time blocks = %g\n", time_balance);

		ia = (typeof(ia)) malloc(nnz * sizeof(*ia));
		_Pragma("omp parallel")
		{
			long i, j;
			_Pragma("omp for")
			for (i=0;i<m;i++)
			{
				for (j=row_ptr[i];j<row_ptr[i+1];j++)
				{
					ia[j] = i;
				}
			}
		}

		_Pragma("omp parallel")
		{
			long i, j;
			_Pragma("omp for")
			for (j=0;j<nnz;j+=32*NNZ_PER_THREAD)
			{
				long j_e = j + 32*NNZ_PER_THREAD;
				if (j_e > nnz)
					j_e = nnz;
				if (ia[j] == ia[j_e-1])
				{
					for (i=j;i<j_e;i++)
					{
						ja[i] = ja[i] | 0x80000000;
					}
				}
			}
		}

		// cuda_push_duplicate(&row_ptr_d, row_ptr, (m+1) * sizeof(*row_ptr_d));
		// cuda_push_duplicate(&ia_d, ia, nnz * sizeof(*ia_d));
		// cuda_push_duplicate(&ja_d, ja, nnz * sizeof(*ja_d));
		// cuda_push_duplicate(&a_d, a, nnz * sizeof(*a_d));
		// cudaMalloc(&multres_d, nnz * sizeof(*y_d));

		// cuda_push_duplicate(&thread_block_i_s_d, thread_block_i_s, num_thread_blocks * sizeof(*thread_block_i_s_d));
		// cuda_push_duplicate(&thread_block_i_e_d, thread_block_i_e, num_thread_blocks * sizeof(*thread_block_i_e_d));

		gpuCudaErrorCheck(cudaMalloc(&row_ptr_d, (m+1) * sizeof(*row_ptr_d)));
		gpuCudaErrorCheck(cudaMalloc(&ia_d, nnz * sizeof(*ia_d)));
		gpuCudaErrorCheck(cudaMalloc(&ja_d, nnz * sizeof(*ja_d)));
		gpuCudaErrorCheck(cudaMalloc(&a_d, nnz * sizeof(*a_d)));
		gpuCudaErrorCheck(cudaMalloc(&thread_block_i_s_d, num_thread_blocks * sizeof(*thread_block_i_s_d)));
		gpuCudaErrorCheck(cudaMalloc(&thread_block_i_e_d, num_thread_blocks * sizeof(*thread_block_i_e_d)));
		gpuCudaErrorCheck(cudaMalloc(&x_d, n * sizeof(*x_d)));
		gpuCudaErrorCheck(cudaMalloc(&y_d, m * sizeof(*y_d)));


		gpuCudaErrorCheck(cudaMallocHost(&row_ptr_h, (m+1) * sizeof(INT_T)));
		gpuCudaErrorCheck(cudaMallocHost(&ia_h, nnz * sizeof(INT_T)));
		gpuCudaErrorCheck(cudaMallocHost(&ja_h, nnz * sizeof(INT_T)));
		gpuCudaErrorCheck(cudaMallocHost(&a_h, nnz * sizeof(ValueType)));
		gpuCudaErrorCheck(cudaMallocHost(&thread_block_i_s_h, num_thread_blocks * sizeof(INT_T)));
		gpuCudaErrorCheck(cudaMallocHost(&thread_block_i_e_h, num_thread_blocks * sizeof(INT_T)));
		gpuCudaErrorCheck(cudaMallocHost(&thread_block_j_s_h, num_thread_blocks * sizeof(INT_T)));
		gpuCudaErrorCheck(cudaMallocHost(&thread_block_j_e_h, num_thread_blocks * sizeof(INT_T)));
		gpuCudaErrorCheck(cudaMallocHost(&x_h, n * sizeof(ValueType)));
		gpuCudaErrorCheck(cudaMallocHost(&y_h, m * sizeof(ValueType)));

		memcpy(row_ptr_h, row_ptr, (m + 1) * sizeof(INT_T));
		memcpy(ia_h, ia, nnz * sizeof(INT_T));
		memcpy(ja_h, ja, nnz * sizeof(INT_T));
		memcpy(a_h, a, nnz * sizeof(ValueType));
		memcpy(thread_block_i_s_h, thread_block_i_s, num_thread_blocks * sizeof(INT_T));
		memcpy(thread_block_i_e_h, thread_block_i_e, num_thread_blocks * sizeof(INT_T));

		gpuCudaErrorCheck(cudaMemcpy(row_ptr_d, row_ptr_h, (m+1) * sizeof(*row_ptr_d), cudaMemcpyHostToDevice));

		gpuCudaErrorCheck(cudaMemcpy(ia_d, ia_h, nnz * sizeof(*ia_d), cudaMemcpyHostToDevice));

		gpuCudaErrorCheck(cudaMemcpy(ja_d, ja_h, nnz * sizeof(*ja_d), cudaMemcpyHostToDevice));

		gpuCudaErrorCheck(cudaMemcpy(a_d, a_h, nnz * sizeof(*a_d), cudaMemcpyHostToDevice));

		gpuCudaErrorCheck(cudaMemcpy(thread_block_i_s_d, thread_block_i_s_h, num_thread_blocks * sizeof(*thread_block_i_s_d), cudaMemcpyHostToDevice));

		gpuCudaErrorCheck(cudaMemcpy(thread_block_i_e_d, thread_block_i_e_h, num_thread_blocks * sizeof(*thread_block_i_e_d), cudaMemcpyHostToDevice));


		_Pragma("omp parallel")
		{
			long j;
			_Pragma("omp for")
			for (j=0;j<nnz;j++)
			{
				ja[j] = ja[j] & 0x7FFFFFFF;
			}
		}
	}

	~CSRArrays()
	{
		free(a);
		free(row_ptr);
		free(ia);
		free(ja);
		free(thread_block_i_s);
		free(thread_block_i_e);

		gpuCudaErrorCheck(cudaFree(row_ptr_d));
		gpuCudaErrorCheck(cudaFree(ia_d));
		gpuCudaErrorCheck(cudaFree(ja_d));
		gpuCudaErrorCheck(cudaFree(a_d));
		// gpuCudaErrorCheck(cudaFree(multres_d));
		gpuCudaErrorCheck(cudaFree(thread_block_i_s_d));
		gpuCudaErrorCheck(cudaFree(thread_block_i_e_d));
		gpuCudaErrorCheck(cudaFree(x_d));
		gpuCudaErrorCheck(cudaFree(y_d));

		gpuCudaErrorCheck(cudaFreeHost(row_ptr_h));
		gpuCudaErrorCheck(cudaFreeHost(ia_h));
		gpuCudaErrorCheck(cudaFreeHost(ja_h));
		gpuCudaErrorCheck(cudaFreeHost(a_h));
		// gpuCudaErrorCheck(cudaFreeHost(multres_h));
		gpuCudaErrorCheck(cudaFreeHost(thread_block_i_s_h));
		gpuCudaErrorCheck(cudaFreeHost(thread_block_i_e_h));
		gpuCudaErrorCheck(cudaFreeHost(thread_block_j_s_h));
		gpuCudaErrorCheck(cudaFreeHost(thread_block_j_e_h));
		gpuCudaErrorCheck(cudaFreeHost(x_h));
		gpuCudaErrorCheck(cudaFreeHost(y_h));
	}

	void spmv(ValueType * x, ValueType * y);
	void statistics_start();
	int statistics_print_data(__attribute__((unused)) char * buf, __attribute__((unused)) long buf_n);
};


void compute_csr(CSRArrays * restrict csr, ValueType * restrict x , ValueType * restrict y);

void
CSRArrays::spmv(ValueType * x, ValueType * y)
{
	compute_csr(this, x, y);
}


struct Matrix_Format *
csr_to_format(INT_T * row_ptr, INT_T * col_ind, ValueTypeReference * values, long m, long n, long nnz, long symmetric, long symmetry_expanded)
{
	if (symmetric && !symmetry_expanded)
		error("symmetric matrices not supported by this format, expand symmetry");
	struct CSRArrays * csr = new CSRArrays(row_ptr, col_ind, values, m, n, nnz);
	// for (long i=0;i<10;i++)
		// printf("%d\n", row_ptr[i]);
	csr->mem_footprint = nnz * (sizeof(ValueType) + sizeof(INT_T)) + (m+1) * sizeof(INT_T);
	char *format_name;
	format_name = (char *)malloc(100*sizeof(char));
	snprintf(format_name, 100, "Custom_CSR_CUDA_constant_nnz_per_thread_b%d_nnz%d", BLOCK_SIZE, NNZ_PER_THREAD);
	csr->format_name = format_name;
	return csr;
}


//==========================================================================================================================================
//= CSR Custom
//==========================================================================================================================================


template<typename T>
__device__ T binary_search_gpu(T * A, long s, long e, T target)
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


/* inline
__device__ void reduce_block(INT_T * ia_buf, ValueType * val_buf, ValueType * restrict y)
{
	const int tidb = threadIdx.x;
	int row = ia_buf[tidb];
	int k;
	for (k=1;k<BLOCK_SIZE;k*=2)
	{
		if ((tidb & (2*k-1)) == k-1)
		{
			ValueType val = val_buf[tidb];
			if (row == ia_buf[tidb+k])
			{
				val_buf[tidb+k] += val;
				// val_buf[tidb] = 0;
			}
			else
			{
				atomicAdd(&y[row], val);
				// y[row] += val;
			}
		}
		__syncthreads();
	}
	if (tidb == 0)
		atomicAdd(&y[ia_buf[BLOCK_SIZE-1]], val_buf[BLOCK_SIZE-1]);
} */


/* inline
__device__ void reduce_block(INT_T * ia_buf, ValueType * val_buf, ValueType * restrict y)
{
	const int tidb = threadIdx.x;
	int k;
	INT_T row = ia_buf[tidb];
	for (k=1;k<BLOCK_SIZE;k*=2)
	{
		if ((tidb & (2*k-1)) == 0)
		{
			INT_T row_next = ia_buf[tidb+k];
			ValueType val_next = val_buf[tidb+k];
			if (row == row_next)
			{
				val_buf[tidb] += val_next;
			}
			else
			{
				atomicAdd(&y[row], val_buf[tidb]);
				val_buf[tidb] = val_next;
				ia_buf[tidb] = row_next;
			}
		}
		__syncthreads();
	}
	if (tidb == 0)
		atomicAdd(&y[ia_buf[0]], val_buf[0]);
} */


/* template <typename group_t>
__device__ void reduce_warp(group_t g, INT_T * ia_buf, ValueType * val_buf, ValueType * restrict y)
{
	const int tidl = g.thread_rank();   // Group lane.
	int row = ia_buf[tidl];
	ValueType val;
	int k;
	#pragma unroll
	for (k=1;k<g.size();k*=2)
	{
		// val = val_buf[tidl];
		// if ((tidl & (2*k-1)) == k-1)
		// {
			// if (tidl >= k && row == ia_buf[tidl-k])
			// {
				// val_buf[tidl-k] += val;
				// val = 0;
			// }
		// }
		// g.sync();
		// if ((tidl & (2*k-1)) == k-1 && val != 0)
		// {
			// if (row == ia_buf[tidl+k])
			// {
				// val_buf[tidl+k] += val;
			// }
			// else
			// {
				// atomicAdd(&y[row], val);
			// }
		// }
		// g.sync();
		val = val_buf[tidl];
		if ((tidl & (2*k-1)) == k-1)
		{
			if (row == ia_buf[tidl+k])
			{
				val_buf[tidl+k] += val;
			}
			else
			{
				atomicAdd(&y[row], val);
			}
		}
		g.sync();
	}
}
inline
__device__ void reduce_block(INT_T * ia_buf, ValueType * val_buf, ValueType * restrict y)
{
	const int tidb = threadIdx.x;
	const int tidb_div = tidb / 32;
	const int tidb_mod = tidb % 32;
	thread_block_tile<32> tile32 = tiled_partition<32>(this_thread_block());
	reduce_warp(tile32, &ia_buf[tidb_div*32], &val_buf[tidb_div*32], y);
	// __syncthreads();
	// if (tidb_mod == 31)
	// {
		// ia_buf[tidb_mod] = ia_buf[tidb];
		// val_buf[tidb_mod] = val_buf[tidb];
	// }
	// __syncthreads();
	// if (tidb_div == 0)
		// reduce_warp(tile32, ia_buf, val_buf, y);
	// if (tidb == 0)
		// atomicAdd(&y[ia_buf[31]], val_buf[31]);
	if (tidb_mod == 31)
		atomicAdd(&y[ia_buf[tidb]], val_buf[tidb]);
} */


/* template <typename group_t>
__device__ void reduce_warp(group_t g, INT_T * row_ptr, ValueType * val_ptr, ValueType * restrict y)
{
	const int tidl = g.thread_rank();   // Group lane.
	INT_T row = *row_ptr;
	ValueType val = *val_ptr;
	int k;
	g.sync();
	#pragma unroll
	for (k=1;k<g.size();k*=2)
	{
		INT_T row_next;
		ValueType val_next;
		row_next = __shfl_sync(0xffffffff, row, tidl+k);
		val_next = __shfl_sync(0xffffffff, val, tidl+k);
		if ((tidl & (2*k-1)) == 0)
		{
			if (row == row_next)
			{
				val += val_next;
			}
			else
			{
				atomicAdd(&y[row], val);
				val = val_next;
				row = row_next;
			}
		}
		g.sync();
	}
	*row_ptr = row;
	*val_ptr = val;
	// if (tidl == 0)
		// atomicAdd(&y[row], val);
}
inline
__device__ void reduce_block(INT_T row, ValueType val, ValueType * restrict y)
{
	const int tidb = threadIdx.x;
	const int tidb_div = tidb / 32;
	const int tidb_mod = tidb % 32;
	thread_block_tile<32> tile32 = tiled_partition<32>(this_thread_block());
	reduce_warp(tile32, &row, &val, y);
	if (tidb_mod == 0)
		atomicAdd(&y[row], val);
	// extern __shared__ char sm[];
	// ValueType * val_buf = (typeof(val_buf)) sm;
	// INT_T * ia_buf = (typeof(ia_buf)) &sm[32 * sizeof(ValueType)];
	// if (tidb_mod == 0)
	// {
		// ia_buf[tidb_div] = row;
		// val_buf[tidb_div] = val;
	// }
	// __syncthreads();
	// if (tidb_div == 0)
	// {
		// row = ia_buf[tidb];
		// val = val_buf[tidb];
		// reduce_warp(tile32, &row, &val, y);
	// }
	// if (tidb == 0)
		// atomicAdd(&y[row], val);
} */


/* template <typename group_t>
__device__ void reduce_warp(group_t g, INT_T row, ValueType val, ValueType * restrict y)
{
	const int tidl = g.thread_rank();   // Group lane.
	int k;
	g.sync();
	#pragma unroll
	for (k=1;k<g.size();k*=2)
	{
		INT_T row_prev;
		ValueType val_prev;
		row_prev = __shfl_sync(0xffffffff, row, tidl-k);  // FULL_MASK
		val_prev = __shfl_sync(0xffffffff, val, tidl-k);
		if ((tidl & (2*k-1)) == 2*k-1)
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
	}
	if (tidl == 31)
		atomicAdd(&y[row], val);
}
inline
__device__ void reduce_block(INT_T row, ValueType val, ValueType * restrict y)
{
	thread_block_tile<32> tile32 = tiled_partition<32>(this_thread_block());
	reduce_warp(tile32, row, val, y);
} */


// Threads may only read data from another thread which is actively participating in the __shfl_sync() command.
// If the target thread is inactive, the retrieved value is undefined.
template <typename group_t>
__device__ void reduce_warp(group_t g, INT_T row, ValueType val, ValueType * restrict y)
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
}
inline
__device__ void reduce_block(INT_T row, ValueType val, ValueType * restrict y)
{
	thread_block_tile<32> tile32 = tiled_partition<32>(this_thread_block());
	reduce_warp(tile32, row, val, y);
}


__device__ void spmv_last_block(INT_T * thread_block_i_s, INT_T * thread_block_i_e, INT_T * row_ptr, INT_T * ia, INT_T * ja, ValueType * a, long m, long n, long nnz, ValueType * restrict x, ValueType * restrict y)
{
	// extern __shared__ char sm[];
	const int tidb = threadIdx.x;
	const int block_id = blockIdx.x;
	const int nnz_per_block = BLOCK_SIZE * NNZ_PER_THREAD;
	// ValueType * val_buf = (typeof(val_buf)) sm;
	// INT_T * ia_buf = (typeof(ia_buf)) &sm[BLOCK_SIZE * sizeof(ValueType)];
	__attribute__((unused)) int i, i_s, i_e, j, j_s, j_e, k, l, p;
	i_s = thread_block_i_s[block_id];
	i_e = thread_block_i_e[block_id];
	j_s = block_id * nnz_per_block + tidb * NNZ_PER_THREAD;
	j_e = j_s + NNZ_PER_THREAD;
	if (j_e > nnz)
		j_e = nnz;
	i = binary_search_gpu(row_ptr, i_s, i_e, j_s);
	double sum = 0;
	for (j=j_s;j<j_e;j++)
	{
		if (j >= row_ptr[i+1])
		{
			atomicAdd(&y[i], sum);
			sum = 0;
			while (j >= row_ptr[i+1])
				i++;
		}
		// sum += a[j] * x[ja[j] & 0x7FFFFFFF];
		sum = __fma_rn(a[j], x[ja[j] & 0x7FFFFFFF], sum);
	}
	reduce_block(i, sum, y);
}


template <typename group_t>
__device__ ValueType reduce_warp_single_row(group_t g, ValueType val)
{
	// Use XOR mode to perform butterfly reduction
	for (int i=g.size()/2; i>=1; i/=2)
	{
		val += g.shfl_xor(val, i); // __shfl_xor_sync(0xffffffff, val, i, g.size());   // 'sum' is same on all threads
		// val += __shfl_down_sync(0xffffffff, val, i, g.size());   // Only thread 0 has the total sum.
	}
	return val;
}


template <typename group_t>
__device__ void spmv_warp_single_row(group_t g, int i, int j_s, int j_e, INT_T * ja, ValueType * a, ValueType * restrict x, ValueType * restrict y)
{
	const int tidl = g.thread_rank();   // Group lane.
	int j;
	double sum = 0;
	for (j=j_s;j<j_e;j++)
	{
		sum = __fma_rn(a[j], x[ja[j] & 0x7FFFFFFF], sum);
	}
	sum = reduce_warp_single_row(g, sum);
	if (tidl == 0)
		atomicAdd(&y[i], sum);
}


template <typename group_t>
__device__ void spmv_full_warp(group_t g, int single_row, int i_s, int j_s, int j_e, INT_T * row_ptr, INT_T * ja, ValueType * a, ValueType * restrict x, ValueType * restrict y)
{
	int i, j;
	int ptr_next;
	double sum = 0;
	// int i_w_s, i_w_e;
	// i_w_s = __shfl_sync(0xffffffff, i_s, 0);
	// i_w_e = __shfl_sync(0xffffffff, i, 31);
	// if (i_w_e != i_w_s)
	if (single_row)
	{
		spmv_warp_single_row(g, i_s, j_s, j_e, ja, a, x, y);
	}
	else
	{
		i = i_s;
		ptr_next = row_ptr[i+1];
		for (j=j_s;j<j_e;j++)
		{
			if (j >= ptr_next)
			{
				atomicAdd(&y[i], sum);
				sum = 0;
				while (j >= ptr_next)
				{
					i++;
					ptr_next = row_ptr[i+1];
				}
			}
			// sum += a[j] * x[ja[j] & 0x7FFFFFFF];
			sum = __fma_rn(a[j], x[ja[j] & 0x7FFFFFFF], sum);
		}
		reduce_warp(g, i, sum, y);
	}
}


__device__ void spmv_full_block(INT_T * thread_block_i_s, INT_T * thread_block_i_e, INT_T * row_ptr, INT_T * ia, INT_T * ja, ValueType * a, long m, long n, long nnz, ValueType * restrict x, ValueType * restrict y)
{
	// extern __shared__ char sm[];
	// const int tidb = threadIdx.x;
	const int tidw = threadIdx.x % 32;
	const int warp_id = threadIdx.x / 32;
	const int block_id = blockIdx.x;
	const int nnz_per_block = BLOCK_SIZE * NNZ_PER_THREAD;
	// ValueType * val_buf = (typeof(val_buf)) sm;
	// INT_T * ia_buf = (typeof(ia_buf)) &sm[BLOCK_SIZE * sizeof(ValueType)];
	__attribute__((unused)) int i_s, i_e, j, j_s, j_e, j_w_s, k, l, p;
	j_w_s = block_id * nnz_per_block + warp_id * NNZ_PER_THREAD * 32;
	j_s = j_w_s + tidw * NNZ_PER_THREAD;
	j_e = j_s + NNZ_PER_THREAD;

	// i_s = 0;
	// i_e = m;
	i_s = thread_block_i_s[block_id];
	i_e = thread_block_i_e[block_id];
	i_s = binary_search_gpu(row_ptr, i_s, i_e, j_s);

	int single_row = (ja[j_s] & 0x80000000) ? 1 : 0;
	// int single_row = 0;
	thread_block_tile<32> tile32 = tiled_partition<32>(this_thread_block());
	spmv_full_warp(tile32, single_row, i_s, j_s, j_e, row_ptr, ja, a, x, y);
}


__global__ void gpu_kernel_spmv_row_indices_continuous(INT_T * thread_block_i_s, INT_T * thread_block_i_e, INT_T * row_ptr, INT_T * ia, INT_T * ja, ValueType * a, long m, long n, long nnz, ValueType * restrict x, ValueType * restrict y)
{
	int grid_size = gridDim.x;
	int block_id = blockIdx.x;
	if (block_id == grid_size - 1)
		spmv_last_block(thread_block_i_s, thread_block_i_e, row_ptr, ia, ja, a, m, n, nnz, x, y);
	else
		spmv_full_block(thread_block_i_s, thread_block_i_e, row_ptr, ia, ja, a, m, n, nnz, x, y);
}


void
compute_csr(CSRArrays * restrict csr, ValueType * restrict x, ValueType * restrict y)
{
	dim3 block_dims(BLOCK_SIZE);
	dim3 grid_dims(csr->num_thread_blocks);
	// long shared_mem_size = BLOCK_SIZE * (sizeof(ValueType));
	// long shared_mem_size = BLOCK_SIZE * (sizeof(ValueType) + sizeof(INT_T));
	long shared_mem_size = 0;

	if (csr->x == NULL)
	{
		printf("Grid : {%d, %d, %d} blocks. Blocks : {%d, %d, %d} threads.\n", grid_dims.x, grid_dims.y, grid_dims.z, block_dims.x, block_dims.y, block_dims.z);
		csr->x = x;
		memcpy(csr->x_h, x, csr->n * sizeof(ValueType));
		gpuCudaErrorCheck(cudaMemcpy(csr->x_d, csr->x_h, csr->n * sizeof(*csr->x_d), cudaMemcpyHostToDevice));
	}

	cudaMemset(csr->y_d, 0, csr->m * sizeof(csr->y_d));

	// cudaFuncCachePreferNone:   no preference for shared memory or L1 (default);
	// cudaFuncCachePreferShared: prefer larger shared memory and smaller L1 cache;
	// cudaFuncCachePreferL1:     prefer larger L1 cache and smaller shared memory;
	gpuCudaErrorCheck(cudaFuncSetCacheConfig(gpu_kernel_spmv_row_indices_continuous, cudaFuncCachePreferL1));
	gpu_kernel_spmv_row_indices_continuous<<<grid_dims, block_dims, shared_mem_size>>>(csr->thread_block_i_s_d, csr->thread_block_i_e_d, csr->row_ptr_d, csr->ia_d, csr->ja_d, csr->a_d, csr->m, csr->n, csr->nnz, csr->x_d, csr->y_d);
	gpuCudaErrorCheck(cudaPeekAtLastError());
	gpuCudaErrorCheck(cudaDeviceSynchronize());

	if (csr->y == NULL)
	{
		csr->y = y;

		gpuCudaErrorCheck(cudaMemcpy(csr->y_h, csr->y_d, csr->m * sizeof(*csr->y_d), cudaMemcpyDeviceToHost));
		memcpy(y, csr->y_h, csr->m * sizeof(ValueType));
	}
}


//==========================================================================================================================================
//= Print Statistics
//==========================================================================================================================================


void
CSRArrays::statistics_start()
{
}


int
statistics_print_labels(__attribute__((unused)) char * buf, __attribute__((unused)) long buf_n)
{
	return 0;
}


int
CSRArrays::statistics_print_data(__attribute__((unused)) char * buf, __attribute__((unused)) long buf_n)
{
	return 0;
}

