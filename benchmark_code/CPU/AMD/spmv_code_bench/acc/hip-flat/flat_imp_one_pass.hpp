//
// Created by chu genshen on 2021/9/13.
//

#ifndef SPMV_ACC_FLAT_IMP_ONE_PASS_HPP
#define SPMV_ACC_FLAT_IMP_ONE_PASS_HPP

#include "flat_config.h"
#include "flat_reduce.hpp"
#include "building_config.h"

#include "../common/global_mem_ops.h"
#include "../common/utils.h"

template <int WF_SIZE, int R, int REDUCE_OPTION, int REDUCE_VEC_SIZE, int THREADS, typename I, typename T>
__global__ void spmv_flat_one_pass_kernel(int m, const T alpha, const T beta, const I *__restrict__ row_offset,
                                          const I *__restrict__ break_points, const I *__restrict__ csr_col_ind,
                                          const T *__restrict__ csr_val, const T *__restrict__ x, T *__restrict__ y) {
  const int global_thread_id = threadIdx.x + THREADS * blockIdx.x;
  const int global_threads_num = hipGridDim_x * THREADS;

  const int wf_id_in_block = blockIdx.x / WF_SIZE; // wavefront id in block
  const int block_id = blockIdx.x;                 // block id
  const int tid_in_block = threadIdx.x % THREADS;  // thread id in one block

  constexpr int nnz_per_block = THREADS * R;
  __shared__ T shared_val[nnz_per_block];
  const I last_element_index = row_offset[m];

  I bp_index = block_id;
  // each block process THREADS * R elements.
  int k = nnz_per_block * block_id;

#pragma unroll
  for (int i = 0; i < R; i++) {
    const I shared_inx = tid_in_block + i * THREADS;
    const I index = min(k + shared_inx, last_element_index - 1);
    shared_val[shared_inx] = csr_val[index] * x[csr_col_ind[index]];
  }
  __syncthreads();

  // reduce via LDS.
  const I reduce_start_row_id = min(break_points[bp_index], m);
  I reduce_end_row_id = min(break_points[bp_index + 1], m);
  // if it is the last block
  if (reduce_end_row_id == 0) {
    reduce_end_row_id = m;
  }
  // if start of the next block cuts some row.
  // or some row cross multiple blocks.
  // what if it has a very long row? which means `reduce_start_row_id == reduce_end_row_id`.
  if (row_offset[reduce_end_row_id] % nnz_per_block != 0 || reduce_end_row_id == reduce_start_row_id) {
    reduce_end_row_id = min(reduce_end_row_id + 1, m); // make sure `reduce_end_row_id` is not larger than m
  }

  // reduce and store result to memory
  if (REDUCE_OPTION == FLAT_REDUCE_OPTION_VEC) {
    const I n_reduce_rows_num = reduce_end_row_id - reduce_start_row_id;
    flat_reduce_in_vector<I, T, nnz_per_block, THREADS, REDUCE_VEC_SIZE>(n_reduce_rows_num, tid_in_block, bp_index,
                                                                         reduce_start_row_id, reduce_end_row_id, alpha,
                                                                         row_offset, shared_val, y);
  } else if (REDUCE_OPTION == FLAT_REDUCE_OPTION_VEC_MEM_COALESCING) {
    const I n_reduce_rows_num = reduce_end_row_id - reduce_start_row_id;
    flat_reduce_in_vector_with_mem_coalescing<I, T, nnz_per_block, THREADS, REDUCE_VEC_SIZE>(
        n_reduce_rows_num, tid_in_block, bp_index, reduce_start_row_id, reduce_end_row_id, alpha, row_offset,
        shared_val, y);
  } else {
    // direct reduction
    flat_reduce_direct<I, T, nnz_per_block, THREADS>(tid_in_block, bp_index, reduce_start_row_id, reduce_end_row_id,
                                                     alpha, row_offset, shared_val, y);
  }
}

#define FLAT_KERNEL_ONE_PASS_WRAPPER(R, REDUCE_OPTION, REDUCE_VEC_SIZE, BLOCKS, THREADS)                               \
  (spmv_flat_one_pass_kernel<__WF_SIZE__, R, REDUCE_OPTION, REDUCE_VEC_SIZE, THREADS, int,                             \
                             double>)<<<(BLOCKS), (THREADS)>>>(m, alpha, beta, rowptr, break_points, colindex, value,  \
                                                               x, y)

#endif // SPMV_ACC_FLAT_IMP_ONE_PASS_HPP
