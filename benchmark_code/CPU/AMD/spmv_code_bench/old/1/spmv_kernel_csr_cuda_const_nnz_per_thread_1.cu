#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

#include <cuda.h>
#include <cooperative_groups.h>

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

	#include "cuda/cuda_util.h"
#ifdef __cplusplus
}
#endif


using namespace cooperative_groups;


#define NNZ_PER_THREAD  4


INT_T * thread_block_i_s = NULL;
INT_T * thread_block_i_e = NULL;

INT_T * thread_block_j_s = NULL;
INT_T * thread_block_j_e = NULL;


INT_T * thread_block_i_s_dev = NULL;
INT_T * thread_block_i_e_dev = NULL;

INT_T * thread_block_j_s_dev = NULL;
INT_T * thread_block_j_e_dev = NULL;


extern int prefetch_distance;

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

	INT_T * row_ptr_dev;
	INT_T * ia_dev;
	INT_T * ja_dev;
	ValueType * a_dev;

	ValueType * multres_dev;

	ValueType * x = NULL;
	ValueType * y = NULL;
	ValueType * x_dev = NULL;
	ValueType * y_dev = NULL;

	int max_smem_per_block, multiproc_count, max_threads_per_block, warp_size, max_threads_per_multiproc, max_block_dim_x, max_num_threads;
	int num_threads;
	int block_size;
	int num_blocks;

	CSRArrays(INT_T * row_ptr, INT_T * ja, ValueType * a, long m, long n, long nnz) : Matrix_Format(m, n, nnz), row_ptr(row_ptr), ja(ja), a(a)
	{
		double time_balance;
		long i;

		cudaDeviceGetAttribute(&max_smem_per_block, cudaDevAttrMaxSharedMemoryPerBlock, 0);
		cudaDeviceGetAttribute(&multiproc_count, cudaDevAttrMultiProcessorCount, 0);
		cudaDeviceGetAttribute(&max_threads_per_block, cudaDevAttrMaxThreadsPerBlock , 0);
		cudaDeviceGetAttribute(&warp_size, cudaDevAttrWarpSize , 0);
		cudaDeviceGetAttribute(&max_threads_per_multiproc, cudaDevAttrMaxThreadsPerMultiProcessor, 0);
		cudaDeviceGetAttribute(&max_block_dim_x, cudaDevAttrMaxBlockDimX, 0);
		max_num_threads = max_threads_per_multiproc * multiproc_count;
		printf("max_smem_per_block(bytes)=%d\n", max_smem_per_block);
		printf("multiproc_count=%d\n", multiproc_count);
		printf("max_threads_per_block=%d\n", max_threads_per_block);
		printf("warp_size=%d\n", warp_size);
		printf("max_threads_per_multiproc=%d\n", max_threads_per_multiproc);
		printf("max_block_dim_x=%d\n", max_block_dim_x);
		printf("max_num_threads=%d\n", max_num_threads);

		// block_size = 32;
		// block_size = 64;
		// block_size = 128;
		// block_size = 256;
		// block_size = 512;
		block_size = 1024;

		num_threads = (nnz + NNZ_PER_THREAD - 1) / NNZ_PER_THREAD;

		num_threads = ((num_threads + block_size - 1) / block_size) * block_size;

		num_blocks = num_threads / block_size;

		printf("num_threads=%d, block_size=%d, num_blocks=%d\n", num_threads, block_size, num_blocks);

		thread_block_i_s = (INT_T *) malloc(num_blocks * sizeof(*thread_block_i_s));
		thread_block_i_e = (INT_T *) malloc(num_blocks * sizeof(*thread_block_i_e));
		thread_block_j_s = (INT_T *) malloc(num_blocks * sizeof(*thread_block_j_s));
		thread_block_j_e = (INT_T *) malloc(num_blocks * sizeof(*thread_block_j_e));
		time_balance = time_it(1,
			long lower_boundary;
			// for (i=0;i<num_blocks;i++)
			// {
				// loop_partitioner_balance_iterations(num_blocks, i, 0, nnz, &thread_block_j_s[i], &thread_block_j_e[i]);
				// macros_binary_search(row_ptr, 0, m, thread_block_j_s[i], &lower_boundary, NULL);           // Index boundaries are inclusive.
				// thread_block_i_s[i] = lower_boundary;
			// }
			long nnz_per_block = block_size * NNZ_PER_THREAD;
			for (i=0;i<num_blocks;i++)
			{
				thread_block_j_s[i] = nnz_per_block * i;
				thread_block_j_e[i] = nnz_per_block * (i+ 1);
				if (thread_block_j_s[i] > nnz)
					thread_block_j_s[i] = nnz;
				if (thread_block_j_e[i] > nnz)
					thread_block_j_e[i] = nnz;
				macros_binary_search(row_ptr, 0, m, thread_block_j_s[i], &lower_boundary, NULL);           // Index boundaries are inclusive.
				thread_block_i_s[i] = lower_boundary;
			}
			for (i=0;i<num_blocks;i++)
			{
				if (i == num_blocks - 1)   // If we calculate each thread's boundaries individually some empty rows might be unassigned.
					thread_block_i_e[i] = m;
				else
					thread_block_i_e[i] = thread_block_i_s[i+1] + 1;
				if ((thread_block_j_s[i] >= row_ptr[thread_block_i_e[i]]) || (thread_block_j_s[i] < row_ptr[thread_block_i_s[i]]))
					error("bad binary search of row start: i=%d j:[%d, %d] j=%d", thread_block_i_s[i], row_ptr[thread_block_i_s[i]], row_ptr[thread_block_i_e[i]], thread_block_j_s[i]);
			}
		);
		printf("balance time = %g\n", time_balance);

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

		cuda_push_duplicate(&row_ptr_dev, row_ptr, (m+1) * sizeof(*row_ptr_dev));
		cuda_push_duplicate(&ia_dev, ia, nnz * sizeof(*ia_dev));
		cuda_push_duplicate(&ja_dev, ja, nnz * sizeof(*ja_dev));
		cuda_push_duplicate(&a_dev, a, nnz * sizeof(*a_dev));
		cudaMalloc(&multres_dev, nnz * sizeof(*y_dev));

		cudaMalloc(&x_dev, n * sizeof(*x_dev));
		cudaMalloc(&y_dev, m * sizeof(*y_dev));

		cuda_push_duplicate(&thread_block_i_s_dev, thread_block_i_s, num_blocks * sizeof(*thread_block_i_s_dev));
		cuda_push_duplicate(&thread_block_i_e_dev, thread_block_i_e, num_blocks * sizeof(*thread_block_i_e_dev));
		cuda_push_duplicate(&thread_block_j_s_dev, thread_block_j_s, num_blocks * sizeof(*thread_block_j_s_dev));
		cuda_push_duplicate(&thread_block_j_e_dev, thread_block_j_e, num_blocks * sizeof(*thread_block_j_e_dev));

	}

	~CSRArrays()
	{
		free(a);
		free(row_ptr);
		free(ia);
		free(ja);
		free(thread_block_i_s);
		free(thread_block_i_e);

		cudaFree(row_ptr_dev);
		cudaFree(ia_dev);
		cudaFree(ja_dev);
		cudaFree(a_dev);
	}

	void spmv(ValueType * x, ValueType * y);
	void statistics_start();
	int statistics_print_data(__attribute__((unused)) char * buf, __attribute__((unused)) long buf_n);
};


void compute_csr(CSRArrays * restrict csr, ValueType * restrict x , ValueType * restrict y);
void compute_csr_kahan(CSRArrays * restrict csr, ValueType * restrict x, ValueType * restrict y);
void compute_csr_prefetch(CSRArrays * restrict csr, ValueType * restrict x , ValueType * restrict y);
void compute_csr_omp_simd(CSRArrays * restrict csr, ValueType * restrict x , ValueType * restrict y);
void compute_csr_vector(CSRArrays * restrict csr, ValueType * restrict x , ValueType * restrict y);
void compute_csr_vector_perfect_nnz_balance(CSRArrays * restrict csr, ValueType * restrict x , ValueType * restrict y);


void
CSRArrays::spmv(ValueType * x, ValueType * y)
{
	compute_csr(this, x, y);
}


struct Matrix_Format *
csr_to_format(INT_T * row_ptr, INT_T * col_ind, ValueType * values, long m, long n, long nnz)
{
	struct CSRArrays * csr = new CSRArrays(row_ptr, col_ind, values, m, n, nnz);
	// for (long i=0;i<10;i++)
		// printf("%d\n", row_ptr[i]);
	csr->mem_footprint = nnz * (sizeof(ValueType) + sizeof(INT_T)) + (m+1) * sizeof(INT_T);
	csr->format_name = (char *) "Custom_CSR_CUDA_reduce";
	return csr;
}


//==========================================================================================================================================
//= CSR Custom
//==========================================================================================================================================


/* inline
__device__ void reduce_block(INT_T * ia_buf, ValueType * val_buf, ValueType * restrict y)
{
	const int tidb = threadIdx.x;
	const int block_size = blockDim.x;
	int row = ia_buf[tidb];
	int k;
	for (k=1;k<block_size;k*=2)
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
		atomicAdd(&y[ia_buf[block_size-1]], val_buf[block_size-1]);
} */


/* inline
__device__ void reduce_block(INT_T * ia_buf, ValueType * val_buf, ValueType * restrict y)
{
	const int tidb = threadIdx.x;
	const int block_size = blockDim.x;
	int k;
	INT_T row = ia_buf[tidb];
	for (k=1;k<block_size;k*=2)
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
		row_prev = __shfl_sync(0xffffffff, row, tidl-k);
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
	flag = 0xaaaaaaaa;
	row_prev = __shfl_sync(flag, row, tidl-1);
	val_prev = __shfl_sync(flag, val, tidl-1);
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
	flag = 0x88888888;
	row_prev = __shfl_sync(flag, row, tidl-2);
	val_prev = __shfl_sync(flag, val, tidl-2);
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
	flag = 0x80808080;
	row_prev = __shfl_sync(flag, row, tidl-4);
	val_prev = __shfl_sync(flag, val, tidl-4);
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
	flag = 0x80008000;
	row_prev = __shfl_sync(flag, row, tidl-8);
	val_prev = __shfl_sync(flag, val, tidl-8);
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
	flag = 0x80000000;
	row_prev = __shfl_sync(flag, row, tidl-16);
	val_prev = __shfl_sync(flag, val, tidl-16);
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


__device__ void spmv_last_block(INT_T * thread_block_i_s, INT_T * thread_block_i_e, INT_T * thread_block_j_s, INT_T * thread_block_j_e, INT_T * row_ptr, INT_T * ia, INT_T * ja, ValueType * a, ValueType * restrict x, ValueType * restrict y)
{
	// extern __shared__ char sm[];
	const int tidb = threadIdx.x;
	const int block_id = blockIdx.x;
	const int block_size = blockDim.x;
	// ValueType * val_buf = (typeof(val_buf)) sm;
	// INT_T * ia_buf = (typeof(ia_buf)) &sm[block_size * sizeof(ValueType)];
	[[gnu::unused]] int i, i_s, i_e, j, j_s, j_e, k, l, p;
	i_s = thread_block_i_s[block_id];
	i_e = thread_block_i_e[block_id];
	j_s = thread_block_j_s[block_id];
	j_e = thread_block_j_e[block_id];
	const int total_j = j_e - j_s;
	const int mod = total_j % block_size;
	int j_l_s, j_l_e;
	j_l_s = j_s + tidb * (total_j / block_size);
	j_l_e = j_l_s + (total_j / block_size);
	if (tidb < mod)
	{
		j_l_s += tidb;
		j_l_e += tidb + 1;
	}
	else
	{
		j_l_s += mod;
		j_l_e += mod;
	}
	// int m = (i_e + i_s) / 2;
	// while (i_s < i_e)
	// {
		// if (j_l_s >= row_ptr[m])
		// {
			// i_s = m + 1;
		// }
		// else
		// {
			// i_e = m;
		// }
		// m = (i_e + i_s) / 2;
	// }
	// i = i_s - 1;
	i = ia[j_l_s];
	// if (tidb == block_size-1)
	// {
		// if (j_l_e != j_e)
		// {
			// printf("wrong");
		// }
	// }
	double sum = 0;
	int ptr_next = row_ptr[i+1];
	for (j=j_l_s;j<j_l_e;j++)
	{
		// if (ia[j] != i)
		// {
			// atomicAdd(&y[i], sum);
			// sum = 0;
			// i = ia[j];
		// }
		if (j >= ptr_next)
		{
			atomicAdd(&y[i], sum);
			// y[i] += sum;
			sum = 0;
			while (j >= ptr_next)
			{
				i++;
				ptr_next = row_ptr[i+1];
			}
			// i = ia[j];
		}
		// sum += a[j] * x[ja[j]];
		sum = __fma_rn(a[j], x[ja[j]], sum);
	}
	// if (j_l_s < j_l_e)
		// atomicAdd(&y[i], sum);
	// val_buf[tidb] = sum;
	// ia_buf[tidb] = i;
	// __syncthreads();
	// reduce_block(ia_buf, val_buf, y);
	reduce_block(i, sum, y);
}


__device__ void spmv_full_block(INT_T * thread_block_i_s, INT_T * thread_block_i_e, INT_T * row_ptr, INT_T * ia, INT_T * ja, ValueType * a, ValueType * restrict x, ValueType * restrict y)
{
	// extern __shared__ char sm[];
	const int tidb = threadIdx.x;
	const int block_id = blockIdx.x;
	const int block_size = blockDim.x;
	const int nnz_per_block = block_size * NNZ_PER_THREAD;
	// ValueType * val_buf = (typeof(val_buf)) sm;
	// INT_T * ia_buf = (typeof(ia_buf)) &sm[block_size * sizeof(ValueType)];
	[[gnu::unused]] int i, i_s, i_e, j, j_s, j_e, k, l, p;
	i_s = thread_block_i_s[block_id];
	i_e = thread_block_i_e[block_id];
	j_s = block_id * nnz_per_block;
	// j_e = (block_id + 1) * nnz_per_block;
	int j_l_s, j_l_e;
	j_l_s = j_s + tidb * NNZ_PER_THREAD;
	j_l_e = j_l_s + NNZ_PER_THREAD;
	// int m = (i_e + i_s) / 2;
	// while (i_s < i_e)
	// {
		// if (j_l_s >= row_ptr[m])
		// {
			// i_s = m + 1;
		// }
		// else
		// {
			// i_e = m;
		// }
		// m = (i_e + i_s) / 2;
	// }
	// i = i_s - 1;
	i = ia[j_l_s];
	// if (tidb == block_size-1)
	// {
		// if (j_l_e != j_e)
		// {
			// printf("wrong");
		// }
	// }
	double sum = 0;
	int ptr_next = row_ptr[i+1];
	for (j=j_l_s;j<j_l_e;j++)
	{
		// if (ia[j] != i)
		// {
			// atomicAdd(&y[i], sum);
			// sum = 0;
			// i = ia[j];
		// }
		if (j >= ptr_next)
		{
			atomicAdd(&y[i], sum);
			// y[i] += sum;
			sum = 0;
			while (j >= ptr_next)
			{
				i++;
				ptr_next = row_ptr[i+1];
			}
			// i = ia[j];
		}
		// sum += a[j] * x[ja[j]];
		sum = __fma_rn(a[j], x[ja[j]], sum);
	}
	// if (j_l_s < j_l_e)
		// atomicAdd(&y[i], sum);
	// val_buf[tidb] = sum;
	// ia_buf[tidb] = i;
	// __syncthreads();
	// reduce_block(ia_buf, val_buf, y);
	reduce_block(i, sum, y);
}


__global__ void gpu_kernel_spmv_row_indices_continuous(INT_T * thread_block_i_s, INT_T * thread_block_i_e, INT_T * thread_block_j_s, INT_T * thread_block_j_e, INT_T * row_ptr, INT_T * ia, INT_T * ja, ValueType * a, ValueType * restrict x, ValueType * restrict y)
{
	int grid_size = gridDim.x;
	int block_id = blockIdx.x;
	if (block_id == grid_size - 1)
		spmv_last_block(thread_block_i_s, thread_block_i_e, thread_block_j_s, thread_block_j_e, row_ptr, ia, ja, a, x, y);
	else
		spmv_full_block(thread_block_i_s, thread_block_i_e, row_ptr, ia, ja, a, x, y);
}


void
compute_csr(CSRArrays * restrict csr, ValueType * restrict x, ValueType * restrict y)
{
	// int num_threads = csr->num_threads;
	int block_size = csr->block_size;
	int num_blocks = csr->num_blocks;
	dim3 block_dims(block_size);
	dim3 grid_dims(num_blocks);
	// long shared_mem_size = block_size * (sizeof(ValueType));
	// long shared_mem_size = block_size * (sizeof(ValueType) + sizeof(INT_T));
	long shared_mem_size = 0;

	if (csr->x == NULL)
	{
		csr->x = x;
		cudaMemcpy(csr->x_dev, csr->x, csr->n * sizeof(*csr->x), cudaMemcpyHostToDevice);
	}

	cudaMemset(csr->y_dev, 0, csr->m * sizeof(csr->y_dev));

	gpu_kernel_spmv_row_indices_continuous<<<grid_dims, block_dims, shared_mem_size>>>(thread_block_i_s_dev, thread_block_i_e_dev, thread_block_j_s_dev, thread_block_j_e_dev, csr->row_ptr_dev, csr->ia_dev, csr->ja_dev, csr->a_dev, csr->x_dev, csr->y_dev);

	cudaError_t err;
	err = cudaDeviceSynchronize();
	if (err != cudaSuccess)
		error("cudaDeviceSynchronize: %s\n", cudaGetErrorString(err));
	err = cudaGetLastError();
	if (err != cudaSuccess)
		error("gpu kernel error: %s\n", cudaGetErrorString(err));

	if (csr->y == NULL)
	{
		csr->y = y;
		cudaMemcpy(csr->y, csr->y_dev, csr->m * sizeof(*csr->y), cudaMemcpyDeviceToHost);
	}

	// exit(0);
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

