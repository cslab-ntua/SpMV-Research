//
// Created by genshen on 2021/4/17.
//

#ifndef SPMV_ACC_WAVEFRONT_ROW_DEFAULT_HPP
#define SPMV_ACC_WAVEFRONT_ROW_DEFAULT_HPP

#include <iostream>
#include <stdio.h>  // printf
#include <stdlib.h> // EXIT_FAILURE

#include <hip/hip_runtime.h>
#include <hip/hip_runtime_api.h> // hipMalloc, hipMemcpy, etc.

#include "../common/global_mem_ops.h"
#include "../common/utils.h"

#define N_UNROLLING 2
#define N_UNROLLING_SHIFT 1

struct IntCom {
  std::int32_t i1; // occupies 4 bytes
  std::int32_t i2; // occupies 4 bytes
};

union Address {
  std::uint64_t L; // occupies 8 bytes
  IntCom I;        // occupies 8 bytes
};

/**
 * calculate: Y = alpha * A*X+beta * y
 * @tparam BLOCK_SIZE block size
 * @tparam WF_SIZE wavefront size
 * @tparam I type of indexing
 * @tparam J type of csr col indexing
 * @tparam T type of data in matrix
 * @param m rows of matrix A.
 * @param alpha alpha value
 * @param beta beta value
 * @param row_offset row offset array of csr matrix A
 * @param csr_col_ind col index of csr matrix A
 * @param csr_val matrix A in csr format
 * @param x  vector x
 * @param y vector y
 * @return
 */
template <unsigned int BLOCK_SIZE, unsigned int WF_SIZE, typename I, typename J, typename T>
__global__ void device_spmv_wf_row_default(J m, T alpha, T beta, const I *row_offset, const J *csr_col_ind,
                                           const T *csr_val, const T *x, T *y) {
  const int lid = hipThreadIdx_x & (WF_SIZE - 1); // local id in the wavefront

  const J gid = hipBlockIdx_x * BLOCK_SIZE + hipThreadIdx_x; // global thread id
  const J nwf = hipGridDim_x * BLOCK_SIZE / WF_SIZE;         // number of wavefront, or step length

  Address col_index_a = {0L};
  Address col_index_b = {0L};
  // Loop over rows
  for (J row = gid / WF_SIZE; row < m; row += nwf) {
    // Each wavefront processes one row
    const I row_start = row_offset[row];
    const I row_end = row_offset[row + 1];

    T sum = static_cast<T>(0);

    // Loop over non-zero elements
    const J unrolling_loop_end = row_start + (((row_end - row_start) >> N_UNROLLING_SHIFT) << N_UNROLLING_SHIFT);
    for (I _j = row_start + N_UNROLLING * lid; _j < unrolling_loop_end; _j += N_UNROLLING * WF_SIZE) {
      dbl_x2 val_x2;
      global_load(static_cast<const void *>(csr_val + _j), val_x2);
      col_index_a.I.i1 = csr_col_ind[_j];
      col_index_b.I.i1 = csr_col_ind[_j + 1];
      sum = device_fma(val_x2.a, device_ldg(x + col_index_a.L), sum);
      sum = device_fma(val_x2.b, device_ldg(x + col_index_b.L), sum);
    }
    for (I j = unrolling_loop_end + lid; j < row_end; j += WF_SIZE) {
      sum = device_fma(csr_val[j], device_ldg(x + csr_col_ind[j]), sum);
    }

    // Obtain row sum using parallel reduction
    sum = wfreduce_sum<WF_SIZE>(sum);

    // First thread of each wavefront writes result into global memory
    if (lid == WF_SIZE - 1) {
      if (beta == static_cast<T>(0)) {
        y[row] = sum;
      } else {
        y[row] = device_fma(beta, y[row], alpha * sum);
      }
    }
  }
}

#endif // SPMV_ACC_WAVEFRONT_ROW_DEFAULT_HPP
