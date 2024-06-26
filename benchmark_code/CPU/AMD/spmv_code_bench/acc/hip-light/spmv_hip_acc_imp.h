//
// Created by chaohu on 2021/05/25.
//

#ifndef SPMV_ACC_SPMV_HIP_ACC_IMP_LIGHT_H
#define SPMV_ACC_SPMV_HIP_ACC_IMP_LIGHT_H

#include <hip/hip_runtime.h>
#include <hip/hip_runtime_api.h>

#include "api/types.h"

template <int THREADS_PER_VECTOR, int WF_SIZE, typename T>
__global__ void spmv_light_kernel(const int m, const T alpha, const T beta, int *hip_row_counter, const int *row_offset,
                                  const int *csr_col_ind, const T *csr_val, const T *x, T *y);

void light_sparse_spmv(int trans, const int alpha, const int beta, const csr_desc<int, double> d_csr_desc,
                       const double *x, double *y);

#include "spmv_hip_acc_imp.inl"

#endif // SPMV_ACC_SPMV_HIP_ACC_IMP_LIGHT_H
