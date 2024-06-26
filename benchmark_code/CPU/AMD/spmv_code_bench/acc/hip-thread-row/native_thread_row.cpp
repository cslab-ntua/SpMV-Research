//
// Created by genshen on 2021/7/15.
//

#include <hip/hip_runtime.h>
#include <hip/hip_runtime_api.h>

#include "thread_row.h"

__global__ void native_thread_row(int trans, const double alpha, const double beta, int m, int n, const int *rowptr,
                                  const int *colindex, const double *value, const double *x, double *y) {
  int thread_id = threadIdx.x + blockDim.x * blockIdx.x;
  const int next_row_step = blockDim.x * gridDim.x;
  double y0 = 0.0;
  for (int i = thread_id; i < m; i += next_row_step) {
    y0 = 0.0;
    for (int j = rowptr[i]; j < rowptr[i + 1]; j++) {
      y0 += value[j] * x[colindex[j]];
    }
    y[i] = alpha * y0 + beta * y[i];
  }
}
