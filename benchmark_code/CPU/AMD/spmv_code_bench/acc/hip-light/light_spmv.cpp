//
// Created by genshen on 2021/7/15.
//

#include "spmv_hip_acc_imp.h"

#include <hip/hip_runtime.h>
#include <hip/hip_runtime_api.h>

#include "../common/macros.h"
#include "building_config.h"

#define LIGHT_KERNEL_CALLER(N)                                                                                         \
  ((spmv_light_kernel<N, __WF_SIZE__, double>) <<<256,256>>> (m, alpha, beta, hip_row_counter, rowptr, colindex, value, x, y))

void light_sparse_spmv(int trans, const int alpha, const int beta, const csr_desc<int, double> d_csr_desc,
                       const double *x, double *y) {
  VAR_FROM_CSR_DESC(d_csr_desc);

  int *hip_row_counter;
  hipMalloc((void **)&hip_row_counter, sizeof(int));
  const int avg_eles_per_row = d_csr_desc.nnz / m;
  hipMemset(hip_row_counter, 0, sizeof(int));

  if (avg_eles_per_row <= 2) {
    LIGHT_KERNEL_CALLER(1);
  } else if (avg_eles_per_row <= 4 || __WF_SIZE__ <= 2) {
    LIGHT_KERNEL_CALLER(2);
  } else if (avg_eles_per_row <= 8 || __WF_SIZE__ <= 4) {
    LIGHT_KERNEL_CALLER(4);
  } else if (avg_eles_per_row <= 16 || __WF_SIZE__ <= 8) {
    LIGHT_KERNEL_CALLER(8);
  } else if (avg_eles_per_row <= 32 || __WF_SIZE__ <= 16) {
    LIGHT_KERNEL_CALLER(16);
  } else if (avg_eles_per_row <= 64 || __WF_SIZE__ <= 32) {
    LIGHT_KERNEL_CALLER(32);
  } else {
    LIGHT_KERNEL_CALLER(64);
  }

  hipFree(hip_row_counter);
}
