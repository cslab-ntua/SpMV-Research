#include "hip/hip_runtime.h"

#include "macros/cpp_defines.h"

/*
__global__ void gpu_kernel_csr_adaptive(INT_T * ia, INT_T * ja, ValueType * a, INT_T * row_blocks, ValueType * restrict x, ValueType * restrict y)
{
	INT_T startRow = row_blocks[blockIdx.x];
	INT_T nextStartRow = row_blocks[blockIdx.x + 1];
	INT_T num_rows = nextStartRow - startRow;
	INT_T i = threadIdx.x;
	__shared__ volatile ValueType LDS[BLOCK_SIZE];
	
	// If the block consists of more than one row then run CSR Stream
	if (num_rows > 1) {
		// how many nonzeros does this rowblock hold?
		// they will be less than the BLOCK_SIZE (the size of LDS)
		int nnz = ia[nextStartRow] - ia[startRow];
		int col_offset = ia[startRow];

		// Each thread writes to shared memory the result of multiplication for one nonzero
		if (i < nnz)
			LDS[i] = a[col_offset + i] * x[ja[col_offset + i]];
 		// After all positions of LDS have been filled, proceed. 
		__syncthreads();
		
		// Threads that fall within a range sum up the partial results
		// Thread0 of the block will be assigned with the first row of the thread block (startRow+0) and then the next row will be (startRow+BLOCK_SIZE) etc...
		// How many rows per thread depends on how few nonzeros this specific block can hold...
		for (int k = startRow + i; k < nextStartRow; k += BLOCK_SIZE){
			ValueType temp = 0;
			// Sum partial results that this row (k) has results in LDS
			for (INT_T j = (ia[k] - col_offset); j < (ia[k + 1] - col_offset); j++)
				temp = temp + LDS[j];
			// And finally store result in the output y vector.
			y[k] = temp;
		}
	}
	// If the block consists of only one row then run CSR Vector
	else {
		// Thread ID in warp
		INT_T ia_Start = ia[startRow];
		INT_T ia_End   = ia[nextStartRow];
		ValueType sum  = 0;

		// Use all threads in a warp to accumulate multiplied elements
		// Due to the fact that each for loop starts from "ia_Start" + some i (the index inside the thread block) 
		// LDS will be filled with all partial results from this specific row
		// It may be underutilized, considering the fact that this row will consist of less than BLOCK_SIZE elements
		for (INT_T j = ia_Start + i; j < ia_End; j += BLOCK_SIZE){
			INT_T col = ja[j];
			sum += a[j] * x[col];
		}
		// write partial sum at position i (index in thread block) in the LDS array
		LDS[i] = sum;
		__syncthreads();

		// Reduce partial sums
		// reduce results as in 
		// (BS/2 sums)  LDS[i] = LDS[i] + LDS[i + BS/2];, LDS[i+1] = LDS[i+1] + LDS[i+1 + BS/2];
		// (BS/4 sums)  LDS[i] = LDS[i] + LDS[i + BS/4]
		// ...
		// (1 sum)      LDS[i] = LDS[i] + LDS[i+1]; and then finish
		for (int stride = BLOCK_SIZE >> 1; stride > 0; stride >>= 1) {
			__syncthreads();
			if (i < stride)
				LDS[i] += LDS[i + stride]; 
		}
		// Write result
		if (i == 0){
			y[startRow] = LDS[i];
		}
	}
}
*/
__global__ void gpu_kernel_csr_adaptive(INT_T * ia, INT_T * ja, ValueType * a, INT_T * row_blocks, ValueType * restrict x, ValueType * restrict y)
{
	__shared__ volatile ValueType LDS[MULTIBLOCK_SIZE][BLOCK_SIZE];
	INT_T i = threadIdx.x;

	INT_T startRow[MULTIBLOCK_SIZE];
	INT_T nextStartRow[MULTIBLOCK_SIZE];
	INT_T num_rows[MULTIBLOCK_SIZE];

	for(int kk = 0; kk < MULTIBLOCK_SIZE; kk++){
		startRow[kk]     = row_blocks[blockIdx.x*MULTIBLOCK_SIZE + kk];
		nextStartRow[kk] = row_blocks[blockIdx.x*MULTIBLOCK_SIZE + kk + 1];
		num_rows[kk]     = nextStartRow[kk] - startRow[kk];
	}

	for(int kk = 0; kk < MULTIBLOCK_SIZE; kk++){
		// If the block consists of more than one row then run CSR Stream
		if (num_rows[kk] > 1) {
			// how many nonzeros does this rowblock hold?
			// they will be less than the BLOCK_SIZE (the size of LDS)
			int nnz = ia[nextStartRow[kk]] - ia[startRow[kk]];
			int col_offset = ia[startRow[kk]];

			// Each thread writes to shared memory the result of multiplication for one nonzero
			if (i < nnz)
				LDS[kk][i] = a[col_offset + i] * x[ja[col_offset + i]];
	 		// After all positions of LDS have been filled, proceed. 
			__syncthreads();
			
			// Threads that fall within a range sum up the partial results
			// Thread0 of the block will be assigned with the first row of the thread block (startRow+0) and then the next row will be (startRow+BLOCK_SIZE) etc...
			// How many rows per thread depends on how few nonzeros this specific block can hold...
			for (int k = startRow[kk] + i; k < nextStartRow[kk]; k += BLOCK_SIZE){
				ValueType temp = 0;
				// Sum partial results that this row (k) has results in LDS
				for (INT_T j = (ia[k] - col_offset); j < (ia[k + 1] - col_offset); j++)
					temp = temp + LDS[kk][j];
				// And finally store result in the output y vector.
				y[k] = temp;
			}
		}
		// If the block consists of only one row then run CSR Vector
		else if(num_rows[kk] == 1) {
			// Thread ID in warp
			INT_T ia_Start = ia[startRow[kk]];
			INT_T ia_End   = ia[nextStartRow[kk]];
			ValueType sum  = 0;

			// Use all threads in a warp to accumulate multiplied elements
			// Due to the fact that each for loop starts from "ia_Start" + some i (the index inside the thread block) 
			// LDS will be filled with all partial results from this specific row
			// It may be underutilized, considering the fact that this row will consist of less than BLOCK_SIZE elements
			for (INT_T j = ia_Start + i; j < ia_End; j += BLOCK_SIZE){
				INT_T col = ja[j];
				sum = __fma_rn(a[j], x[col], sum); // sum += a[j] * x[col];
			}
			// write partial sum at position i (index in thread block) in the LDS array
			LDS[kk][i] = sum;
			__syncthreads();

			// Reduce partial sums
			// reduce results as in 
			// (BS/2 sums)  LDS[i] = LDS[i] + LDS[i + BS/2];, LDS[i+1] = LDS[i+1] + LDS[i+1 + BS/2];
			// (BS/4 sums)  LDS[i] = LDS[i] + LDS[i + BS/4]
			// ...
			// (1 sum)      LDS[i] = LDS[i] + LDS[i+1]; and then finish
			for (int stride = BLOCK_SIZE >> 1; stride > 0; stride >>= 1) {
				__syncthreads();
				if (i < stride)
					LDS[kk][i] += LDS[kk][i + stride]; 
			}
			// Write result
			if (i == 0){
				y[startRow[kk]] = LDS[kk][i];
			}
		}
	}
}

extern "C" void launch_kernel_wrapper(dim3 grid_dims, dim3 block_dims, long shared_mem_size, hipStream_t stream, INT_T * ia_d, INT_T * ja_d, ValueType * a_d, INT_T * row_blocks_d, ValueType * restrict x_d, ValueType * restrict y_d)
{
	hipLaunchKernelGGL(gpu_kernel_csr_adaptive, grid_dims, block_dims, shared_mem_size, stream, ia_d, ja_d, a_d, row_blocks_d, x_d, y_d);
}