//
// Created by reget on 2021/09/29.
//

#ifndef LINE_IMP_ONE_PASS_INL
#define LINE_IMP_ONE_PASS_INL

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <iostream>

#include <hip/hip_runtime.h>
#include <hip/hip_runtime_api.h>

#include "../common/utils.h"
#include "building_config.h"
#include "line_config.h"

template <typename I, typename T>
__device__ __forceinline__ void
line_one_pass_kernel(const I block_thread_num, const I block_thread_id, const I block_row_begin, const I block_row_end,
                     const I block_row_idx_end, const I block_row_idx_begin, T *shared_val, const I m, const T alpha,
                     const T beta, const I *row_offset, const I *csr_col_ind, const T *csr_val, const T *x, T *y);

template <int ROW_SIZE, int BLOCK_LDS_SIZE, typename I, typename T>
__global__ void spmv_line_one_pass_kernel(I m, const T alpha, const T beta, const I *row_offset, const I *csr_col_ind,
                                          const T *csr_val, const T *x, T *y) {
  const int global_thread_id = threadIdx.x + blockDim.x * blockIdx.x;
  const int block_id = blockIdx.x;                                 // global block id
  const int block_thread_num = blockDim.x;                         // threads num in a block
  const int block_thread_id = global_thread_id % block_thread_num; // local thread id in current block

  __shared__ T shared_val[BLOCK_LDS_SIZE];
  const I block_row_begin = block_id * ROW_SIZE;
  const I block_row_end = min(block_row_begin + ROW_SIZE, m);
  // load val to lds parallel
  const I block_row_idx_begin = row_offset[block_row_begin];
  const I block_row_idx_end = row_offset[block_row_end];
  const I n_values_load = block_row_idx_end - block_row_idx_begin;

  // In this case, user can make sure the values of this Block would not exceed the LDS array.
  line_one_pass_kernel<I, T>(block_thread_num, block_thread_id, block_row_begin, block_row_end, block_row_idx_end,
                             block_row_idx_begin, shared_val, m, alpha, beta, row_offset, csr_col_ind, csr_val, x, y);
}

template <typename I, typename T>
__device__ __forceinline__ void
line_one_pass_kernel(const I block_thread_num, const I block_thread_id, const I block_row_begin, const I block_row_end,
                     const I block_row_idx_end, const I block_row_idx_begin, T *shared_val, const I m, const T alpha,
                     const T beta, const I *row_offset, const I *csr_col_ind, const T *csr_val, const T *x, T *y) {
#ifdef LINE_GLOBAL_LOAD_X2
  constexpr int N_UNROLLING_SHIFT = 1;
#endif

#ifndef LINE_GLOBAL_LOAD_X2
  for (I i = block_row_idx_begin + block_thread_id; i < block_row_idx_end; i += block_thread_num) {
    shared_val[i - block_row_idx_begin] = csr_val[i] * x[csr_col_ind[i]];
  }
#endif
#ifdef LINE_GLOBAL_LOAD_X2
  {
    const int n_lds_load = block_row_idx_end - block_row_idx_begin;
    const int unrolling_loop_end = block_row_idx_begin + ((n_lds_load >> N_UNROLLING_SHIFT) << N_UNROLLING_SHIFT);
    for (I i = block_row_idx_begin + 2 * block_thread_id; i < unrolling_loop_end; i += 2 * block_thread_num) {
      shared_val[i - block_row_idx_begin] = csr_val[i] * x[csr_col_ind[i]];
      shared_val[i - block_row_idx_begin + 1] = csr_val[i + 1] * x[csr_col_ind[i + 1]];
    }
    for (I i = unrolling_loop_end + block_thread_id; i < block_row_idx_end; i += block_thread_num) {
      shared_val[i - block_row_idx_begin] = csr_val[i] * x[csr_col_ind[i]];
    }
  }
#endif
  __syncthreads();
  // `ROW_SIZE` must smaller than `block_thread_num`
  const I reduce_row_id = block_row_begin + block_thread_id;
  if (reduce_row_id >= block_row_end) {
    return;
  }
  const I reduce_row_idx_begin = row_offset[reduce_row_id];
  const I reduce_row_idx_end = row_offset[reduce_row_id + 1];
  T sum = static_cast<T>(0);
  for (I i = reduce_row_idx_begin; i < reduce_row_idx_end; i++) {
    sum += shared_val[i - block_row_idx_begin];
  }
  y[reduce_row_id] = alpha * sum + y[reduce_row_id];
}

#define LINE_ONE_PASS_KERNEL_WRAPPER(N, BLOCK_LDS_SIZE, BLOCKS, THREADS)                                               \
  (spmv_line_one_pass_kernel<N, BLOCK_LDS_SIZE, int, double>)<<<BLOCKS, THREADS>>>(m, alpha, beta, rowptr, colindex,   \
                                                                                   value, x, y)

#endif // LINE_IMP_ONE_PASS_INL
