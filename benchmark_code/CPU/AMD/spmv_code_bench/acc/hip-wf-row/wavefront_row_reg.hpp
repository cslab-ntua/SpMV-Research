//
// Created by reget on 2021/4/27.
//

#ifndef SPMV_ACC_WAVEFRONT_ROW_REG_HPP
#define SPMV_ACC_WAVEFRONT_ROW_REG_HPP

#include <hip/hip_runtime.h>
#include <hip/hip_runtime_api.h> // hipMalloc, hipMemcpy, etc.

#include "../common/utils.h"

template <unsigned int BLOCK_SIZE, unsigned int WF_SIZE>
__global__ void device_spmv_wf_row_reg(int trans, const double alpha, const double beta, int m, int n,
                                       const int *rowptr, const int *colindex, const double *value, const double *x,
                                       double *y) {
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
  for (int row = global_wf_id; row < m; row += num_wf) {
    int row_begin = rowptr[row];
    int row_end = rowptr[row + 1];
    double local_sum, total_sum;
    local_sum = 0;
    total_sum = 0;
    // calculate sum for all element in thread.
    for (int j = row_begin + wf_thread_id; j < row_end; j += WF_SIZE) {
      local_sum += value[j] * __ldg(x + colindex[j]);
    }
    // reduce thread sum to row sum
    SHFL_DOWN_WF_REDUCE(total_sum, local_sum)
    if (wf_thread_id == 0) {
      y[row] = alpha * total_sum + beta * y[row];
    }
  }
}

#endif // SPMV_ACC_WAVEFRONT_ROW_REG_HPP