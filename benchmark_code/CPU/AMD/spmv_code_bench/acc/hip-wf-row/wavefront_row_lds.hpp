//
// Created by reget on 2021/4/23.
//

#ifndef SPMV_ACC_WAVEFRONT_ROW_LDS_HPP
#define SPMV_ACC_WAVEFRONT_ROW_LDS_HPP

#include <iostream>
#include <stdio.h>  // printf
#include <stdlib.h> // EXIT_FAILURE

#include <hip/hip_runtime.h>
#include <hip/hip_runtime_api.h>

#include "../common/utils.h"

template <unsigned int BLOCK_SIZE, unsigned int WF_SIZE>
__global__ void device_spmv_wf_row_lds(int trans, const double alpha, const double beta, int m, int n, const int *rowptr,
                                       const int *colindex, const double *value, const double *x, double *y) {
  // thread id in block
  int block_thread_id = threadIdx.x;
  // thread id in wavefront
  int wf_thread_id = threadIdx.x & (WF_SIZE - 1);
  // thread id in global
  int global_thread_id = blockDim.x * blockIdx.x + threadIdx.x;
  // wavefront id in global
  int global_wf_id = global_thread_id / WF_SIZE;
  // number of wavefront
  int num_wf = gridDim.x * BLOCK_SIZE / WF_SIZE;
  // share memory for store thread result
  __shared__ volatile double share_mem[BLOCK_SIZE];
  for (int row = global_wf_id; row < m; row += num_wf) {
    int row_begin = rowptr[row];
    int row_end = rowptr[row + 1];
    double sum = 0;
    // calculate sum for all element in thread.
    for (int j = row_begin + wf_thread_id; j < row_end; j += WF_SIZE) {
      sum += value[j] * __ldg(x + colindex[j]);
    }
    share_mem[block_thread_id] = alpha * sum;
    __syncthreads();
    // reduce thread sum to row sum
    if (wf_thread_id < 32)
      share_mem[block_thread_id] += share_mem[block_thread_id + 32];
    if (wf_thread_id < 16)
      share_mem[block_thread_id] += share_mem[block_thread_id + 16];
    if (wf_thread_id < 8)
      share_mem[block_thread_id] += share_mem[block_thread_id + 8];
    if (wf_thread_id < 4)
      share_mem[block_thread_id] += share_mem[block_thread_id + 4];
    if (wf_thread_id < 2)
      share_mem[block_thread_id] += share_mem[block_thread_id + 2];
    if (wf_thread_id < 1)
      share_mem[block_thread_id] += share_mem[block_thread_id + 1];
    __syncthreads();
    // first thread in wavefront write result into global memory.
    if (wf_thread_id == 0) {
      y[row] = share_mem[block_thread_id] + beta * y[row];
    }
  }
}

#endif // SPMV_ACC_WAVEFRONT_ROW_LDS_HPP
