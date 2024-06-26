#include "hip/hip_runtime.h"

#include "macros/cpp_defines.h"

#include "acc/hip-line-enhance/line_enhance_spmv_imp.inl"

extern "C" void launch_kernel_wrapper_adaptive(dim3 grid_dims, dim3 block_dims, long shared_mem_size, hipStream_t stream, INT_T * ia_d, INT_T * ja_d, ValueType * a_d, INT_T m, INT_T nnz, double avg_nnz_per_row, ValueType * x_d, ValueType * y_d)
{
	constexpr int WF = 64;
	const ValueType alpha = 1.0;
	const ValueType beta = 0.0;

	if (nnz <= (1 << 24)) { // 2^24=16,777,216
		if (avg_nnz_per_row >= 32) {  // matrix has long rows
			constexpr int R = 4;
			constexpr int ROWS_PER_BLOCK = 64;
			constexpr int REDUCE_OPTION = LE_REDUCE_OPTION_VEC;
			constexpr int VEC_SIZE = 8;
			hipLaunchKernelGGL(HIP_KERNEL_NAME(line_enhance_kernel<REDUCE_OPTION, WF, VEC_SIZE, ROWS_PER_BLOCK, R, BLOCK_SIZE, INT_T, ValueType>), grid_dims, block_dims, shared_mem_size, stream, m, alpha, beta, ia_d, ja_d, a_d, x_d, y_d);
		}
		else { // matrix has short rows, then, use less thread(e.g. direct reduction) for reduction
			constexpr int R = 2;
			constexpr int ROWS_PER_BLOCK = 64;
			constexpr int REDUCE_OPTION = LE_REDUCE_OPTION_DIRECT;
			constexpr int VEC_SIZE = 1;
			hipLaunchKernelGGL(HIP_KERNEL_NAME(line_enhance_kernel<REDUCE_OPTION, WF, VEC_SIZE, ROWS_PER_BLOCK, R, BLOCK_SIZE, INT_T, ValueType>), grid_dims, block_dims, shared_mem_size, stream, m, alpha, beta, ia_d, ja_d, a_d, x_d, y_d);
		}
	}
	else {
		if (avg_nnz_per_row >= 24) { // long row matrix
			constexpr int R = 2;
			constexpr int ROWS_PER_BLOCK = 64;
			constexpr int REDUCE_OPTION = LE_REDUCE_OPTION_VEC;
			constexpr int VEC_SIZE = 4;
			hipLaunchKernelGGL(HIP_KERNEL_NAME(line_enhance_kernel<REDUCE_OPTION, WF, VEC_SIZE, ROWS_PER_BLOCK, R, BLOCK_SIZE, INT_T, ValueType>), grid_dims, block_dims, shared_mem_size, stream, m, alpha, beta, ia_d, ja_d, a_d, x_d, y_d);
		}
		else { // short row matrix, use direct reduce
			constexpr int R = 2;
			constexpr int ROWS_PER_BLOCK = 128;
			constexpr int REDUCE_OPTION = LE_REDUCE_OPTION_DIRECT;
			constexpr int VEC_SIZE = 1;
			hipLaunchKernelGGL(HIP_KERNEL_NAME(line_enhance_kernel<REDUCE_OPTION, WF, VEC_SIZE, ROWS_PER_BLOCK, R, BLOCK_SIZE, INT_T, ValueType>), grid_dims, block_dims, shared_mem_size, stream, m, alpha, beta, ia_d, ja_d, a_d, x_d, y_d);
		}
	}
}

extern "C" void launch_kernel_wrapper(dim3 grid_dims, dim3 block_dims, long shared_mem_size, hipStream_t stream, INT_T * ia_d, INT_T * ja_d, ValueType * a_d, INT_T m, INT_T nnz, double avg_nnz_per_row, ValueType * x_d, ValueType * y_d)
{
	constexpr int WF = 64;
	const ValueType alpha = 1.0;
	const ValueType beta = 0.0;

	constexpr int R = 2;
	constexpr int ROWS_PER_BLOCK = 32;
	constexpr int REDUCE_OPTION = LE_REDUCE_OPTION_VEC;
	constexpr int VEC_SIZE = 4;
	hipLaunchKernelGGL(HIP_KERNEL_NAME(line_enhance_kernel<REDUCE_OPTION, WF, VEC_SIZE, ROWS_PER_BLOCK, R, BLOCK_SIZE, INT_T, ValueType>), grid_dims, block_dims, shared_mem_size, stream, m, alpha, beta, ia_d, ja_d, a_d, x_d, y_d);
}
