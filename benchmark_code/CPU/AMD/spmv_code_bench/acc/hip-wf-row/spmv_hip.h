//
// Created by genshen on 2021/5/4.
//

#ifndef SPMV_ACC_SPMV_HIP_WF_ROW_H
#define SPMV_ACC_SPMV_HIP_WF_ROW_H

#include <hip/hip_runtime.h>
#include <hip/hip_runtime_api.h>

#include "api/types.h"

template <unsigned int BLOCK_SIZE, unsigned int WF_SIZE, typename I, typename J, typename T>
__global__ void device_spmv_wf_row_default(J m, T alpha, T beta, const I *row_offset, const J *csr_col_ind,
                                           const T *csr_val, const T *x, T *y);

template <unsigned int BLOCK_SIZE, unsigned int WF_SIZE>
__global__ void device_spmv_wf_row_lds(int trans, const double alpha, const double beta, int m, int n,
                                       const int *rowptr, const int *colindex, const double *value, const double *x,
                                       double *y);

template <unsigned int BLOCK_SIZE, unsigned int WF_SIZE>
__global__ void device_spmv_wf_row_reg(int trans, const double alpha, const double beta, int m, int n,
                                       const int *rowptr, const int *colindex, const double *value, const double *x,
                                       double *y);

void wf_row_sparse_spmv(int htrans, const int halpha, const int hbeta, const csr_desc<int, double> d_csr_desc,
                        const double *hx, double *hy);

#include "wavefront_row_default.hpp"
#include "wavefront_row_lds.hpp"
#include "wavefront_row_reg.hpp"

#endif // SPMV_ACC_SPMV_HIP_WF_ROW_H
