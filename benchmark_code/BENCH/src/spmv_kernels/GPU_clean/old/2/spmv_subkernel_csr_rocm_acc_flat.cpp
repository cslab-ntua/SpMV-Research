#include "hip/hip_runtime.h"

#include "macros/cpp_defines.h"

#include "acc/hip-flat/flat_imp.inl"
#include "acc/hip-flat/flat_imp_one_pass.hpp"

extern "C" void launch_warmup_kernel_wrapper(dim3 grid_dims, dim3 block_dims, long shared_mem_size, hipStream_t stream, INT_T * ia_d, INT_T m, INT_T * break_points_d, INT_T break_points_len)
{
	constexpr int R = 2;
	hipLaunchKernelGGL(HIP_KERNEL_NAME(pre_calc_break_point<R * BLOCK_SIZE, 0, INT_T>), grid_dims, block_dims, shared_mem_size, stream, ia_d, m, break_points_d, break_points_len);
}

extern "C" void launch_kernel_wrapper_adaptive(dim3 grid_dims, dim3 block_dims, long shared_mem_size, hipStream_t stream, INT_T * ia_d, INT_T * ja_d, ValueType * a_d, INT_T m, INT_T nnz, INT_T * break_points_d, double avg_block_nnz_max, ValueType * x_d, ValueType * y_d)
{
	constexpr int WF = 64; // warp size of AMD GPUs
	const ValueType alpha = 1.0;
	const ValueType beta = 0.0;

	constexpr int R = 2;
	if(avg_block_nnz_max <= 32) {
		constexpr int REDUCE_OPTION = FLAT_REDUCE_OPTION_DIRECT;
		constexpr int REDUCE_VEC_SIZE = 1;
		hipLaunchKernelGGL(HIP_KERNEL_NAME(spmv_flat_one_pass_kernel<WF, R, REDUCE_OPTION, REDUCE_VEC_SIZE, BLOCK_SIZE, INT_T, ValueType>), grid_dims, block_dims, shared_mem_size, stream, m, alpha, beta, ia_d, break_points_d, ja_d, a_d, x_d, y_d);
	}
	else if(avg_block_nnz_max <= 64) {
		constexpr int REDUCE_OPTION = FLAT_REDUCE_OPTION_VEC; // FLAT_REDUCE_OPTION_VEC_MEM_COALESCING;
		constexpr int REDUCE_VEC_SIZE = 4;
		hipLaunchKernelGGL(HIP_KERNEL_NAME(spmv_flat_one_pass_kernel<WF, R, REDUCE_OPTION, REDUCE_VEC_SIZE, BLOCK_SIZE, INT_T, ValueType>), grid_dims, block_dims, shared_mem_size, stream, m, alpha, beta, ia_d, break_points_d, ja_d, a_d, x_d, y_d);
	}
	else{
		constexpr int REDUCE_OPTION = FLAT_REDUCE_OPTION_VEC; // FLAT_REDUCE_OPTION_VEC_MEM_COALESCING;
		constexpr int REDUCE_VEC_SIZE = 16;
		hipLaunchKernelGGL(HIP_KERNEL_NAME(spmv_flat_one_pass_kernel<WF, R, REDUCE_OPTION, REDUCE_VEC_SIZE, BLOCK_SIZE, INT_T, ValueType>), grid_dims, block_dims, shared_mem_size, stream, m, alpha, beta, ia_d, break_points_d, ja_d, a_d, x_d, y_d);
	}
}

extern "C" void launch_kernel_wrapper(dim3 grid_dims, dim3 block_dims, long shared_mem_size, hipStream_t stream, INT_T * ia_d, INT_T * ja_d, ValueType * a_d, INT_T m, INT_T nnz, INT_T * break_points_d, double avg_block_nnz_max, ValueType * x_d, ValueType * y_d)
{
	constexpr int WF = 64; // warp size of AMD GPUs
	const ValueType alpha = 1.0;
	const ValueType beta = 0.0;

	constexpr int R = 2;
	constexpr int REDUCE_OPTION = FLAT_REDUCE_OPTION_VEC;
	constexpr int REDUCE_VEC_SIZE = 4;
	hipLaunchKernelGGL(HIP_KERNEL_NAME(spmv_flat_one_pass_kernel<WF, R, REDUCE_OPTION, REDUCE_VEC_SIZE, BLOCK_SIZE, INT_T, ValueType>), grid_dims, block_dims, shared_mem_size, stream, m, alpha, beta, ia_d, break_points_d, ja_d, a_d, x_d, y_d);
}
