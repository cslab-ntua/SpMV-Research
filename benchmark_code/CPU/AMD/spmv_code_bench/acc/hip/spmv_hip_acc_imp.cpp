//
// Created by genshen on 2021/4/15.
//

#include <iostream>
#include <stdio.h>  // printf
#include <stdlib.h> // EXIT_FAILURE

#include <hip/hip_runtime.h>
#include <hip/hip_runtime_api.h> // hipMalloc, hipMemcpy, etc.

#include "api/spmv.h"
#include "spmv_hip_acc_imp.h"

__global__ void default_device_sparse_spmv_acc(int trans, const int alpha, const int beta, int m, int n,
                                               const int *rowptr, const int *colindex, const double *value,
                                               const double *x, double *y) {
  int thread_id = threadIdx.x + blockDim.x * blockIdx.x;
  if (thread_id == 0) {
    for (int i = 0; i < m; i++) {
      double y0 = 0;
      for (int j = rowptr[i]; j < rowptr[i + 1]; j++)
        y0 += value[j] * x[colindex[j]];
      y[i] = alpha * y0 + beta * y[i];
    }
  }
}

void default_sparse_spmv(int trans, const int alpha, const int beta, const csr_desc<int, double> d_csr_desc,
                         const double *x, double *y) {
  VAR_FROM_CSR_DESC(d_csr_desc);
  const int n = d_csr_desc.cols;

  default_device_sparse_spmv_acc<<<1, 256>>>(trans, alpha, beta, m, n, rowptr, colindex, value, x, y);
}
