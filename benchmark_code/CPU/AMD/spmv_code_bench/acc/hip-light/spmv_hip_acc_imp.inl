//
// Created by chaohu on 2021/05/25.
//

#include <cstdio>
#include <cstdlib>
#include <iostream>

#include <hip/hip_runtime.h>
#include <hip/hip_runtime_api.h>

#include "../common/utils.h"

/**
 * Implementation of SpMV with LightSpMV (see: https://doi.org/10.1007/s11265-016-1216-4).
 * In this method, wavefront can be divided into several vectors (wavefront must be divided with no remainder).
 * (e.g. vector size can only be 1, 2,4,8,16,32,64 if \tparam WF_SIZE is 64).
 * Then, each vector can process one row of matrix A,
 * which also means one wavefront with multiple vectors can compute multiple rows.
 *
 * @tparam THREADS_PER_VECTOR threads in on vector
 * @tparam WF_SIZE threads in one wavefront
 * @tparam T type of data in matrix A, vector x, vector y and alpha, beta.
 * @param m rows in matrix A
 * @param alpha alpha value
 * @param beta beta value
 * @param hip_row_counter shared GRM data
 * @param row_offset row offset array of csr matrix A
 * @param csr_col_ind col index of csr matrix A
 * @param csr_val matrix A in csr format
 * @param x vector x
 * @param y vector y
 * @return
 */
template <int THREADS_PER_VECTOR, int WF_SIZE, typename T>
__global__ void spmv_light_kernel(const int m, const T alpha, const T beta, int *hip_row_counter, const int *row_offset,
                                  const int *csr_col_ind, const T *csr_val, const T *x, T *y) {
  const int land_id = threadIdx.x % THREADS_PER_VECTOR;  // land_id in current vector
  const int wf_land_id = threadIdx.x & (WF_SIZE - 1);    // land_id in current wavefront
  const int wf_vec_id = wf_land_id / THREADS_PER_VECTOR; // vector id in current wavefront

  int i;
  int row;
  // Each time, in each wavefront, it will consume {vectors in one wavefront} rows.
  if (wf_land_id == 0) {
    // atomicAdd is only for the first thread in wavefront
    row = atomicAdd(hip_row_counter, WF_SIZE / THREADS_PER_VECTOR);
  }
  row = __shfl(row, 0, WF_SIZE) + wf_vec_id;
  __syncthreads();

  while (row < m) {
    const int row_start = row_offset[row];
    const int row_end = row_offset[row + 1];
    T sum = 0;

    for (i = row_start + land_id; i < row_end; i += THREADS_PER_VECTOR) {
      sum += csr_val[i] * x[csr_col_ind[i]];
    }

    // reduction
    for (i = THREADS_PER_VECTOR >> 1; i > 0; i >>= 1) {
      sum += __shfl_down(sum, i, THREADS_PER_VECTOR);
      __syncthreads();
    }

    if (land_id == 0) {
      y[row] = device_fma(beta, y[row], alpha * sum);
    }

    if (wf_land_id == 0) {
      row = atomicAdd(hip_row_counter, WF_SIZE / THREADS_PER_VECTOR);
    }
    row = __shfl(row, 0, WF_SIZE) + wf_vec_id;
    __syncthreads();
  }
}
