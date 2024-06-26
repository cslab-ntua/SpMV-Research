//
// Created by dly on 2021/4/26.
//

#ifndef SPMV_ACC_SPMV_HIP_ACC_IMP_BLOCK_H
#define SPMV_ACC_SPMV_HIP_ACC_IMP_BLOCK_H

#include <hip/hip_runtime.h>
#include <hip/hip_runtime_api.h>

__global__ void block_row_device_sparse_spmv_acc(int trans, const int alpha, const int beta, int m, int n,
                                                 const int *rowptr, const int *colindex, const double *value,
                                                 const double *x, double *y);

void block_row_sparse_spmv(int trans, const int alpha, const int beta, const csr_desc<int, double> d_csr_desc,
                           const double *d_x, double *d_y);

#endif // SPMV_ACC_SPMV_HIP_ACC_IMP_BLOCK_H
