//
// Created by genshen on 2021/06/30.
//

#ifndef SPMV_ACC_SPMV_HIP_ACC_IMP_LINE_H
#define SPMV_ACC_SPMV_HIP_ACC_IMP_LINE_H

#include <hip/hip_runtime.h>
#include <hip/hip_runtime_api.h>

#include "api/types.h"

template <int ROW_SIZE, int WF_SIZE, int BLOCKS, typename I, typename T>
__global__ void spmv_line_kernel(int m, const T alpha, const T beta, const I *row_offset, const I *csr_col_ind,
                                 const T *csr_val, const T *x, T *y);

template <int ROW_SIZE, int MAX_ROW_NNZ, typename I, typename T>
__global__ void spmv_line_one_pass_kernel(I m, const T alpha, const T beta, const I *row_offset, const I *csr_col_ind,
                                          const T *csr_val, const T *x, T *y);

template <int ROW_SIZE, int MAX_ROW_NNZ, int THREADS, int WF_SIZE, int VECTOR_SIZE, typename I, typename T,
          bool NO_LDS_EXCEED>
__global__ void spmv_adaptive_line_kernel(const I m, const T alpha, const T beta, const I *row_offset,
                                          const I *csr_col_ind, const T *csr_val, const T *x, T *y);

void adaptive_line_sparse_spmv(int trans, const double alpha, const double beta, const csr_desc<int, double> d_csr_desc,
                               const double *x, double *y);

// only for api compatibility
void adaptive_line_sparse_spmv(int trans, const int alpha, const int beta, const csr_desc<int, double> d_csr_desc,
                               const double *x, double *y);

void line_sparse_spmv(int trans, const int alpha, const int beta, const csr_desc<int, double> d_csr_desc,
                      const double *x, double *y);

#include "line_adaptive_one_pass.inl"
#include "line_imp_one_pass.inl"
#include "line_kernel_imp.inl"

#endif // SPMV_ACC_SPMV_HIP_ACC_IMP_LINE_H
