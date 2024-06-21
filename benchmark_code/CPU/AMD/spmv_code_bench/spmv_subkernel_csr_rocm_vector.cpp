#include "hip/hip_runtime.h"

#include "macros/cpp_defines.h"

__global__ void gpu_kernel_csr_vector(INT_T * ia, INT_T * ja, ValueType * a, INT_T m, int block_size, int warp_size, ValueType * restrict x, ValueType * restrict y)
{
	// Thread ID in block
	INT_T t = threadIdx.x;

	// Thread ID in warp
	INT_T lane = t & (warp_size-1);

	// Number of warps per block
	INT_T warpsPerBlock = blockDim.x / warp_size;

	// One row per warp
	INT_T row = (blockIdx.x * warpsPerBlock) + (t / warp_size);

	__shared__ volatile ValueType LDS[BLOCK_SIZE];

	if (row < m){
		INT_T rowStart = ia[row];
		INT_T rowEnd = ia[row+1];
		ValueType sum = 0;

		// Use all threads in a warp accumulate multiplied elements
		for (INT_T j = rowStart + lane; j < rowEnd; j += warp_size){
			INT_T col = ja[j];
			sum += a[j] * x[col];
		}
		LDS[t] = sum;
		__syncthreads();
	
		// Reduce partial sums
		// For AMD GPUs, the size of warp is 64, not 32. Therefore, one extra step for reduction
		if (lane < 32) LDS[t] += LDS[t + 32];
		if (lane < 16) LDS[t] += LDS[t + 16];
		if (lane <  8) LDS[t] += LDS[t + 8];
		if (lane <  4) LDS[t] += LDS[t + 4];
		if (lane <  2) LDS[t] += LDS[t + 2];
		if (lane <  1) LDS[t] += LDS[t + 1];
		__syncthreads();
		
		// Write result
		if (lane == 0){
			y[row] = LDS[t];
		}
	}
}

extern "C" void launch_kernel_wrapper(dim3 grid_dims, dim3 block_dims, long shared_mem_size, hipStream_t stream, INT_T * ia_d, INT_T * ja_d, ValueType * a_d, INT_T m, int block_size, int warp_size, ValueType * restrict x_d, ValueType * restrict y_d)
{
	hipLaunchKernelGGL(gpu_kernel_csr_vector, grid_dims, block_dims, shared_mem_size, stream, ia_d, ja_d, a_d, m, block_size, warp_size, x_d, y_d);
}