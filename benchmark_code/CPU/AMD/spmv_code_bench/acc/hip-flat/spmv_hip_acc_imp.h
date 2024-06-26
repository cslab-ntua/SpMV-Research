//
// Created by genshen on 2021/07/06.
//

#ifndef SPMV_ACC_SPMV_HIP_ACC_IMP_FLAT_H
#define SPMV_ACC_SPMV_HIP_ACC_IMP_FLAT_H

#include <hip/hip_runtime.h>
#include <hip/hip_runtime_api.h>

#include "flat_imp_one_pass.hpp"
#include "api/types.h"

template <int WF_SIZE, int R, int BLOCKS, int THREADS, typename I, typename T>
__global__ void spmv_flat_kernel(int m, const T alpha, const T beta, const I *__restrict__ row_offset,
                                 const I *__restrict__ break_points, const I *__restrict__ csr_col_ind,
                                 const T *__restrict__ csr_val, const T *__restrict__ x, T *__restrict__ y);

template <int BREAK_STRIDE, int BLOCKS, typename I>
__global__ void pre_calc_break_point(const I *__restrict__ row_ptr, const I m, I *__restrict__ break_points,
                                     const I bp_len);

void flat_sparse_spmv(int trans, const int alpha, const int beta, const csr_desc<int, double> h_csr_desc,
                      const csr_desc<int, double> d_csr_desc, const double *x, double *y);

void adaptive_flat_sparse_spmv(const int nnz_block_0, const int nnz_block_1, int trans, const int alpha, const int beta,
                               const csr_desc<int, double> d_csr_desc, const double *x, double *y);

#include "flat_imp.inl"

#endif // SPMV_ACC_SPMV_HIP_ACC_IMP_FLAT_H
