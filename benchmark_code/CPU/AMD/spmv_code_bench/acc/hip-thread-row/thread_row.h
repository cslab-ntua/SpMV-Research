//
// Created by dly on 2021/4/19.
//

#ifndef SPMV_ACC_THREAD_ROW_H
#define SPMV_ACC_THREAD_ROW_H

#include <hip/hip_runtime.h>
#include <hip/hip_runtime_api.h>

#include "api/types.h"

__global__ void native_thread_row(int trans, const double alpha, const double beta, int m, int n, const int *rowptr,
                                  const int *colindex, const double *value, const double *x, double *y);

template <int N, int MAX_ROW_NNZ, int WF_SIZE, int THREADS, typename I, typename T>
__global__ void kernel_thread_row(const T alpha, const T beta, const I m, const I *__restrict__ row_ptr,
                                  const I *__restrict__ csr_col_inx, const T *__restrict__ csr_val,
                                  const T *__restrict__ x, T *__restrict__ y_in, T *__restrict__ y_out);

template <int N, int MAX_ROW_NNZ, int WF_SIZE, int THREADS, typename I, typename T>
__global__ void kernel_thread_row_block_level(const T alpha, const T beta, const I m, const I *__restrict__ row_ptr,
                                              const I *__restrict__ csr_col_inx, const T *__restrict__ csr_val,
                                              const T *__restrict__ x, T *__restrict__ y);

template <int N, int MAX_ROW_NNZ, int WF_SIZE, int THREADS, typename I, typename T>
__global__ void kernel_thread_row_v2(const T alpha, const T beta, const I m, const I *__restrict__ row_ptr,
                                     const I *__restrict__ csr_col_inx, const T *__restrict__ csr_val,
                                     const T *__restrict__ x, T *__restrict__ y);

template <int N, int MAX_ROW_NNZ, int WF_SIZE, int THREADS, typename I, typename T>
__global__ void kernel_thread_row_block_v2(const T alpha, const T beta, const I m, const I *__restrict__ row_ptr,
                                           const I *__restrict__ csr_col_inx, const T *__restrict__ csr_val,
                                           const T *__restrict__ x, T *__restrict__ y);

void thread_row_sparse_spmv(int trans, const int alpha, const int beta, const csr_desc<int, double> d_csr_desc,
                            const double *d_x, double *d_y);

#include "thread_row.inl"
#include "thread_row_block.hpp"
#include "thread_row_x_remap.inl"
#include "thread_row_block_x_remap.hpp"

#endif // SPMV_ACC_THREAD_ROW_H
