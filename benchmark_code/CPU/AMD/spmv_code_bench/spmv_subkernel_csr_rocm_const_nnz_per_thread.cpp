#include "hip/hip_runtime.h"
#include "hip/hip_cooperative_groups.h"

#include "macros/cpp_defines.h"

using namespace cooperative_groups;

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


__device__ void spmv_last_block(INT_T * thread_block_i_s, INT_T * thread_block_i_e, INT_T * thread_block_j_s, INT_T * thread_block_j_e, INT_T * row_ptr, INT_T * ia, INT_T * ja, ValueType * a, long m, long n, long nnz, ValueType * restrict x, ValueType * restrict y)
{
	// extern __shared__ char sm[];
	const int tidb = threadIdx.x;
	const int block_id = blockIdx.x;
	const int nnz_per_block = BLOCK_SIZE * NNZ_PER_THREAD;
	// ValueType * val_buf = (typeof(val_buf)) sm;
	// INT_T * ia_buf = (typeof(ia_buf)) &sm[BLOCK_SIZE * sizeof(ValueType)];
	[[gnu::unused]] int i, i_s, i_e, j, j_s, j_e, k, l, p;
	i_s = thread_block_i_s[block_id];
	i_e = thread_block_i_e[block_id];
	j_s = block_id * nnz_per_block + tidb * NNZ_PER_THREAD;
	j_e = j_s + NNZ_PER_THREAD;
	if (j_e > nnz)
		j_e = nnz;
	k = (i_e + i_s) / 2;
	while (i_s < i_e)
	{
		if (j_s >= row_ptr[k])
		{
			i_s = k + 1;
		}
		else
		{
			i_e = k;
		}
		k = (i_e + i_s) / 2;
	}
	i = i_s - 1;
	double sum = 0;
	int ptr_next = row_ptr[i+1];
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
	reduce_block(i, sum, y);
}


template <typename group_t>
__device__ ValueType reduce_warp_single_line(group_t g, ValueType val, ValueType * restrict y)
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
	sum = reduce_warp_single_line(g, sum, y);
	if (tidl == 0)
		atomicAdd(&y[i], sum);
}


template <typename group_t>
__device__ void spmv_full_warp(group_t g, int one_line, int i_s, int j_s, int j_e, INT_T * row_ptr, INT_T * ja, ValueType * a, ValueType * restrict x, ValueType * restrict y)
{
	[[gnu::unused]] int i, j, k, l, p;
	int ptr_next;
	i = i_s;
	ptr_next = row_ptr[i_s+1];
	for (j=j_s;j<j_e;j++)   // Find the row of the last nnz.
	{
		if (j >= ptr_next)
		{
			i++;
			break;
		}
	}
	double sum = 0;
	// int i_w_s, i_w_e;
	// i_w_s = __shfl_sync(0xffffffff, i_s, 0);
	// i_w_e = __shfl_sync(0xffffffff, i, 31);
	i = i_s;
	// if (i_w_e != i_w_s)
	if (one_line)
	{
		spmv_warp_single_row(g, i_s, j_s, j_e, ja, a, x, y);
	}
	else
	{
		ptr_next = row_ptr[i+1];
		k = 0;
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
				k++;
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
	[[gnu::unused]] int i_s, i_e, j, j_s, j_e, j_w_s, k, l, p;
	i_s = thread_block_i_s[block_id];
	i_e = thread_block_i_e[block_id];
	// i_s = 0;
	// i_e = m;
	j_w_s = block_id * nnz_per_block + warp_id * NNZ_PER_THREAD * 32;
	j_s = j_w_s + tidw * NNZ_PER_THREAD;
	j_e = j_s + NNZ_PER_THREAD;
	k = (i_e + i_s) / 2;
	while (i_s < i_e)
	{
		if (j_s >= row_ptr[k])
		{
			i_s = k + 1;
		}
		else
		{
			i_e = k;
		}
		k = (i_e + i_s) / 2;
	}
	i_s--;
	int one_line = (ja[j_s] & 0x80000000) ? 1 : 0;
	// int one_line = 0;
	thread_block_tile<32> tile32 = tiled_partition<32>(this_thread_block());
	spmv_full_warp(tile32, one_line, i_s, j_s, j_e, row_ptr, ja, a, x, y);
}


__global__ void gpu_kernel_spmv_row_indices_continuous(INT_T * thread_block_i_s, INT_T * thread_block_i_e, INT_T * thread_block_j_s, INT_T * thread_block_j_e, INT_T * row_ptr, INT_T * ia, INT_T * ja, ValueType * a, long m, long n, long nnz, ValueType * restrict x, ValueType * restrict y)
{
	int grid_size = gridDim.x;
	int block_id = blockIdx.x;
	if (block_id == grid_size - 1)
		spmv_last_block(thread_block_i_s, thread_block_i_e, thread_block_j_s, thread_block_j_e, row_ptr, ia, ja, a, m, n, nnz, x, y);
	else
		spmv_full_block(thread_block_i_s, thread_block_i_e, row_ptr, ia, ja, a, m, n, nnz, x, y);
}

extern "C" void launch_kernel_wrapper(dim3 grid_dims, dim3 block_dims, long shared_mem_size, hipStream_t stream, INT_T * thread_block_i_s_d, INT_T * thread_block_i_e_d, INT_T * thread_block_j_s_d, INT_T * thread_block_j_e_d, INT_T * row_ptr_d, INT_T * ia_d, INT_T * ja_d, ValueType * a_d, INT_T m, INT_T n, INT_T nnz, ValueType * x_d, ValueType * y_d)
{
	hipLaunchKernelGGL(gpu_kernel_spmv_row_indices_continuous, grid_dims, block_dims, shared_mem_size, stream, thread_block_i_s_d, thread_block_i_e_d, thread_block_j_s_d, thread_block_j_e_d, row_ptr_d, ia_d, ja_d, a_d, m, n, nnz, x_d, y_d);
}