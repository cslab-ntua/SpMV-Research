//
// Created by chu genshen on 2021/10/2.
//

#ifndef SPMV_ACC_LINE_ENHANCE_SPMV_IMP_H
#define SPMV_ACC_LINE_ENHANCE_SPMV_IMP_H

#include <hip/hip_runtime.h>
#include <hip/hip_runtime_api.h>

/**
 * In this kernel strategy.
 * we combine line method (inspired by CSR-Stream) and flat strategy.
 * In this strategy, each block is assigned the same number of rows.
 * Multiplication is performed by threads in block and write the results into LDS.
 * In reduction step, each thread will reduce the results belonging to it from LDS in multiple rounds.
 *
 * @tparam WF_SIZE wavefront size, threads in a wavefront.
 * @tparam ROWS_PER_BLOCK rows assigned to a HIP block.
 * @tparam R set for the LDS size in HIP block.
 *     The LDS size of a HIP block will be set to @tparam R* @tparam THREADS
 * @tparam THREADS threads number in a HIP block.
 * @tparam I type of index
 * @tparam T type of float pointer number (e.g. float or double).
 * @param m rows of the matrix.
 * @param alpha, beta alpha and beta value in: y = alpha * A * x + beta * y
 * @param row_offset row offset in csr format.
 * @param csr_col_ind column index in csr format.
 * @param csr_val matrix values in csr format
 * @param x vector x in: y = alpha * A * x + beta * y
 * @param y vector y in: y = alpha * A * x + beta * y
 * @return
 */
template <int WF_SIZE, int ROWS_PER_BLOCK, int R, int THREADS, typename I, typename T>
__global__ void line_enhance_kernel(int m, const T alpha, const T beta, const I *__restrict__ row_offset,
                                 const I *__restrict__ csr_col_ind, const T *__restrict__ csr_val,
                                 const T *__restrict__ x, T *__restrict__ y);

#include "line_enhance_spmv_imp.inl"

#endif // SPMV_ACC_LINE_ENHANCE_SPMV_IMP_H
