//
// Created by chu genshen on 2021/10/2.
//

#include <hip/hip_runtime.h>
#include <hip/hip_runtime_api.h>

#include "building_config.h"
#include "line_enhance_reduce.hpp"

template <int REDUCE_OPTION, int WF_SIZE, int VEC_SIZE, int ROWS_PER_BLOCK, int R, int THREADS, typename I, typename T>
__global__ void line_enhance_kernel(int m, const T alpha, const T beta, const I *__restrict__ row_offset,
                                    const I *__restrict__ csr_col_ind, const T *__restrict__ csr_val,
                                    const T *__restrict__ x, T *__restrict__ y) {
  static_assert(THREADS / VEC_SIZE >= ROWS_PER_BLOCK,
                "vector number in block must larger or equal then the rows processed per block");

  const int g_tid = threadIdx.x + blockDim.x * blockIdx.x; // global thread id
  const int g_bid = blockIdx.x;                            // global block id
  const int tid_in_block = g_tid % THREADS;                // local thread id in current block

  constexpr int shared_len = THREADS * R;
  __shared__ T shared_val[shared_len];

  const I block_row_begin = g_bid * ROWS_PER_BLOCK;
  const I block_row_end = min(block_row_begin + ROWS_PER_BLOCK, m);
  const I block_row_idx_start = row_offset[block_row_begin];
  const I block_row_idx_end = row_offset[block_row_end];

  // vector reduce, if VEC_SIZE is set to 1, it will be direct reduction.
  const I vec_id_in_block = g_tid / VEC_SIZE % (THREADS / VEC_SIZE);
  const I tid_in_vec = g_tid % VEC_SIZE;
  // load reduce row bound
  const I reduce_row_id = block_row_begin + vec_id_in_block;
  I reduce_row_idx_begin = 0;
  I reduce_row_idx_end = 0;
  if (reduce_row_id < block_row_end) {
    reduce_row_idx_begin = row_offset[reduce_row_id];
    reduce_row_idx_end = row_offset[reduce_row_id + 1];
  }

  T sum = static_cast<T>(0);
  const int rounds = (block_row_idx_end - block_row_idx_start) / (R * THREADS) +
                     ((block_row_idx_end - block_row_idx_start) % (R * THREADS) == 0 ? 0 : 1);
  for (int r = 0; r < rounds; r++) {
    // start and end data index in each round
    const I block_round_inx_start = block_row_idx_start + r * R * THREADS;
    const I block_round_inx_end = min(block_round_inx_start + R * THREADS, block_row_idx_end);
    I i = block_round_inx_start + tid_in_block;

    __syncthreads();
// in each inner loop, it processes R*THREADS element at max
#pragma unroll
    for (int k = 0; k < R; k++) {
      if (i < block_row_idx_end) {
        const T tmp = csr_val[i] * x[csr_col_ind[i]];
        shared_val[i - block_round_inx_start] = tmp;
      }
      i += THREADS;
    }

    __syncthreads();
    // reduce
    if (REDUCE_OPTION == LE_REDUCE_OPTION_DIRECT) {
      line_enhance_direct_reduce<I, T>(reduce_row_id, block_row_end, reduce_row_idx_begin, reduce_row_idx_end,
                                       block_round_inx_start, block_round_inx_end, shared_val, sum);
    }
    if (REDUCE_OPTION == LE_REDUCE_OPTION_VEC || REDUCE_OPTION == LE_REDUCE_OPTION_VEC_MEM_COALESCING) {
      sum += line_enhance_vec_reduce<I, T, VEC_SIZE>(reduce_row_id, block_row_end, reduce_row_idx_begin,
                                                     reduce_row_idx_end, block_round_inx_start, block_round_inx_end,
                                                     shared_val, tid_in_vec);
    }
  }
  // store result
  if (REDUCE_OPTION == LE_REDUCE_OPTION_DIRECT) {
    if (reduce_row_id < block_row_end) {
      y[reduce_row_id] = alpha * sum + y[reduce_row_id];
    }
  }
  if (REDUCE_OPTION == LE_REDUCE_OPTION_VEC) {
    line_enhance_vec_local_shift<I, T, VEC_SIZE>(sum);
    if (reduce_row_id < block_row_end && tid_in_vec == 0) {
      y[reduce_row_id] = alpha * sum + y[reduce_row_id];
    }
  }
  if (REDUCE_OPTION == LE_REDUCE_OPTION_VEC_MEM_COALESCING) {
    const I thread_reduce_row_id = block_row_begin + tid_in_block;
    line_enhance_vec_local_shift<I, T, VEC_SIZE>(sum);
    sum = line_enhance_vec_global_shift<I, T, THREADS / VEC_SIZE>(tid_in_block, vec_id_in_block, tid_in_vec,
                                                                   shared_val, sum);
    if (thread_reduce_row_id < block_row_end) {
      y[thread_reduce_row_id] = alpha * sum + y[thread_reduce_row_id];
    }
  }
}

#define LINE_ENHANCE_KERNEL_WRAPPER(REDUCE, ROWS_PER_BLOCK, VEC_SIZE, R, BLOCKS, THREADS)                              \
  (line_enhance_kernel<REDUCE, __WF_SIZE__, VEC_SIZE, ROWS_PER_BLOCK, R, THREADS, int, double>)<<<BLOCKS, THREADS>>>(  \
      m, alpha, beta, rowptr, colindex, value, x, y)
