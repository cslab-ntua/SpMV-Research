//
// Created by genshen on 2021/06/30.
//

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <iostream>

#include <hip/hip_runtime.h>
#include <hip/hip_runtime_api.h>

#include "../common/utils.h"
#include "building_config.h"

/**
 * We solve SpMV with line method, which is called CSR-Stream method in https://doi.org/10.1109/SC.2014.68.
 *
 * @tparam ROW_SIZE the max nnz in one row.
 * @tparam WF_SIZE threads in one wavefront
 * @tparam BLOCKS total blocks on one GPU (blocks in one grid).
 * @tparam I type of data in matrix index
 * @tparam T type of data in matrix A, vector x, vector y and alpha, beta.
 * @param m rows in matrix A
 * @param alpha alpha value
 * @param beta beta value
 * @param row_offset row offset array of csr matrix A
 * @param csr_col_ind col index of csr matrix A
 * @param csr_val matrix A in csr format
 * @param x vector x
 * @param y vector y
 * @return
 */
template <int ROW_SIZE, int WF_SIZE, int BLOCKS, typename I, typename T>
__global__ void spmv_line_kernel(int m, const T alpha, const T beta, const I *row_offset, const I *csr_col_ind,
                                 const T *csr_val, const T *x, T *y) {
  const int global_thread_id = threadIdx.x + blockDim.x * blockIdx.x;

  const int block_id = blockIdx.x;                         // block id
  const int threads_in_block = blockDim.x;                 // threads in one block
  const int tid_in_block = threadIdx.x % threads_in_block; // thread id in one block

  constexpr unsigned int shared_len = 64 * 1024 / (BLOCKS / AVAILABLE_CU) / sizeof(T); // max nnz per block
  __shared__ T shared_val[shared_len];
  constexpr int rows_per_block = shared_len / ROW_SIZE; // rows processed in each loop
  constexpr int nnz_per_block = (shared_len / ROW_SIZE) * ROW_SIZE;
  const I last_element_index = row_offset[m] - 1;

  for (int k = block_id * rows_per_block; k < m; k += BLOCKS * rows_per_block) {
    const I block_start_row_id = k;
    const I base_row_id = row_offset[block_start_row_id];
    for (int i = tid_in_block; i < nnz_per_block; i += threads_in_block) {
      const I index = min(i + base_row_id, last_element_index);
      shared_val[i] = csr_val[index] * x[csr_col_ind[index]];
    }
    __syncthreads();

    // reduce via LDS.
    const I block_end_row_id = min(block_start_row_id + rows_per_block, m);
    // In fact, the threads number in one block can be less than the rows processed
    // in iteration loop (`rows_per_block`).
    // e.g. `threads_in_block` is 256, but `rows_per_block` can be 512.
    // Thus, a for loop is needed for reducing all rows.
    I reduce_row_id = block_start_row_id + tid_in_block;
    for (; reduce_row_id < block_end_row_id; reduce_row_id += threads_in_block) {
      T sum = static_cast<T>(0);
      const I reduce_start_inx = row_offset[reduce_row_id] - base_row_id;
      const I reduce_end_inx = row_offset[reduce_row_id + 1] - base_row_id;
      for (int i = reduce_start_inx; i < reduce_end_inx; i++) {
        sum += shared_val[i];
      }
      y[reduce_row_id] = device_fma(beta, y[reduce_row_id], alpha * sum);
    }
    __syncthreads();
  }
}

#define LINE_KERNEL_WRAPPER(N)                                                                                         \
  (spmv_line_kernel<N, __WF_SIZE__, 512, int, double>)<<<512, 256>>>(m, alpha, beta, rowptr, colindex, value, x, y)
