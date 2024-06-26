//
// Created by genshen on 2021/4/15.
//

#ifndef SPMV_ACC_SPMV_HIP_ACC_IMP_DEFAULT_H
#define SPMV_ACC_SPMV_HIP_ACC_IMP_DEFAULT_H

#include <hip/hip_runtime.h>
#include <hip/hip_runtime_api.h>

#include "../common/macros.h"

__global__ void default_device_sparse_spmv_acc(int trans, const int alpha, const int beta, int m, int n,
                                               const int *rowptr, const int *colindex, const double *value,
                                               const double *x, double *y);

void default_sparse_spmv(int trans, const int alpha, const int beta, const csr_desc<int, double> d_csr_desc,
                         const double *x, double *y);

#endif // SPMV_ACC_SPMV_HIP_ACC_IMP_DEFAULT_H
