//
// Created by chaohu on 2021/05/25.
//

#ifndef SPMV_ACC_VECTOR_ROW_H
#define SPMV_ACC_VECTOR_ROW_H

#include <hip/hip_runtime.h>
#include <hip/hip_runtime_api.h>

#include "api/types.h"

template <int VECTOR_SIZE, int WF_SIZE, typename T>
__global__ void native_vector_row_kernel(int m, const T alpha, const T beta, const int *row_offset,
                                         const int *csr_col_ind, const T *csr_val, const T *x, T *y);

template <int VECTOR_SIZE, int WF_VECTORS, int WF_SIZE, int BLOCKS, typename T>
__global__ void spmv_vector_row_kernel(int m, const T alpha, const T beta, const int *row_offset,
                                       const int *csr_col_ind, const T *csr_val, const T *x, T *y);

template <int VECTOR_SIZE, int WF_VECTORS, int WF_SIZE, int BLOCKS, typename I, typename T>
__global__ void vector_row_kernel_pipeline(int m, const T alpha, const T beta, const I *row_offset,
                                           const I *csr_col_ind, const T *csr_val, const T *x, T *y);

template <int VECTOR_SIZE, int WF_VECTORS, int WF_SIZE, int BLOCKS, typename I, typename T>
__global__ void spmv_vector_row_kernel_double_buffer(int m, const T alpha, const T beta, const I *row_offset,
                                                     const I *csr_col_ind, const T *csr_val, const T *x, T *y);

template <int VECTOR_SIZE, int WF_VECTORS, int WF_SIZE, int BLOCKS, typename I, typename T>
__global__ void spmv_vector_row_kernel_double_buffer_legacy(int m, const T alpha, const T beta, const I *row_offset,
                                                            const I *csr_col_ind, const T *csr_val, const T *x, T *y);

template <int DATA_BLOCKS, int WF_SIZE, typename T>
__global__ void adaptive_vector_row_kernel(int m, const T alpha, const T beta, const int *row_offset,
                                           const int *csr_col_ind, const T *csr_val, const T *x, T *y,
                                           const int block_bp);

void vec_row_sparse_spmv(int trans, const int alpha, const int beta, const csr_desc<int, double> d_csr_desc,
                         const double *x, double *y);

void adaptive_vec_row_sparse_spmv(const int nnz_block_0, const int nnz_block_1, int trans, const int alpha,
                                  const int beta, const csr_desc<int, double> d_csr_desc, const double *x, double *y);

#include "opt_double_buffer.hpp"
#include "vector_row.inl"
#include "vector_row_adaptive.hpp"
#include "vector_row_native.hpp"

#endif // SPMV_ACC_VECTOR_ROW_H
