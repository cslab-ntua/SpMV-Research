//
// Created by dly on 2021/4/26.
//

#include <hip/hip_runtime.h>
#include <hip/hip_runtime_api.h> // hipMalloc, hipMemcpy, etc.
#include <iostream>
#include <stdio.h>  // printf
#include <stdlib.h> // EXIT_FAILURE

#include "../common/macros.h"

const unsigned blocks = 64;
const unsigned threadPerBlock = 256;

__global__ void block_row_device_sparse_spmv_acc(int trans, const int alpha, const int beta, int m, int n,
                                                 const int *rowptr, const int *colindex, const double *value,
                                                 const double *x, double *y) {

  __shared__ double sum_row[threadPerBlock];
  double sum_thread = 0.0;
  for (int i = hipBlockIdx_x; i < m; i += blocks) {
    int startRow = rowptr[i];
    int endRow = rowptr[i + 1];
    sum_thread = 0.0;
    for (int j = startRow + hipThreadIdx_x; j < endRow; j += threadPerBlock) {
      sum_thread += value[j] * x[colindex[j]];
    }
    sum_row[hipThreadIdx_x] = sum_thread;
    __syncthreads();
    if (hipThreadIdx_x < 128) {
      sum_row[hipThreadIdx_x] += sum_row[hipThreadIdx_x + 128];
    }
    //	__syncthreads();
    if (hipThreadIdx_x < 64) {
      sum_row[hipThreadIdx_x] += sum_row[hipThreadIdx_x + 64];
    }
    //	__syncthreads();
    if (hipThreadIdx_x < 32) {
      sum_row[hipThreadIdx_x] += sum_row[hipThreadIdx_x + 32];
    }
    //	__syncthreads();
    if (hipThreadIdx_x < 16) {
      sum_row[hipThreadIdx_x] += sum_row[hipThreadIdx_x + 16];
    }
    //	__syncthreads();
    if (hipThreadIdx_x < 8) {
      sum_row[hipThreadIdx_x] += sum_row[hipThreadIdx_x + 8];
    }
    //	__syncthreads();
    if (hipThreadIdx_x < 4) {
      sum_row[hipThreadIdx_x] += sum_row[hipThreadIdx_x + 4];
    }
    //	__syncthreads();
    if (hipThreadIdx_x < 2) {
      sum_row[hipThreadIdx_x] += sum_row[hipThreadIdx_x + 2];
    }
    //	__syncthreads();
    if (hipThreadIdx_x < 1) {
      sum_row[hipThreadIdx_x] += sum_row[hipThreadIdx_x + 1];
    }
    __syncthreads();
    if (hipThreadIdx_x == 0) {
      y[i] = alpha * sum_row[0] + beta * y[i];
    }
  }
}

void block_row_sparse_spmv(int trans, const int alpha, const int beta, const csr_desc<int, double> d_csr_desc,
                           const double *x, double *y) {
  VAR_FROM_CSR_DESC(d_csr_desc);
  const int n = d_csr_desc.cols;

  block_row_device_sparse_spmv_acc<<<64, 256>>>(trans, alpha, beta, m, n, rowptr, colindex, value, x, y);
}
