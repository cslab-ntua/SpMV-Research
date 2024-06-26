//
// Created by genshen on 2021/7/22.
//

#ifndef SPMV_ACC_VECTOR_ROW_NATIVE_HPP
#define SPMV_ACC_VECTOR_ROW_NATIVE_HPP

#include "../common/cross_lane_ops.h"
#include "building_config.h"
#include "vector_config.h"

// memory coalescing of loading and storing vector y at block level.
template <int THREADS, int VECTOR_SIZE, int WF_SIZE, typename T>
__device__ __forceinline__ void block_store_y_with_coalescing(const int tid_in_wf, const int gid, const int row,
                                                              const int m, const T alpha, const T beta, const T sum,
                                                              const T *__restrict__ y_in, T *__restrict__ y_out,
                                                              T *__restrict__ lds_y) {
  constexpr int WF_VECTORS = WF_SIZE / VECTOR_SIZE;
  constexpr int BLOCK_VECTORS = THREADS / VECTOR_SIZE;

  const int wf_id_in_block = (gid % THREADS) / WF_SIZE;      // wavefront id in current block
  const int vec_id_in_block = (gid % THREADS) / VECTOR_SIZE; // vector id in current block
  const int tid_in_block = (gid % THREADS);                  // thread id in current block

  const int vec_row = row - tid_in_block / VECTOR_SIZE + tid_in_block;

  T y_local;
  if (tid_in_block < BLOCK_VECTORS && vec_row < m) {
    y_local = y_in[vec_row];
  }

  if (tid_in_wf % VECTOR_SIZE == 0) {
    lds_y[vec_id_in_block] = sum;
  }

  __syncthreads();

  if (tid_in_block < BLOCK_VECTORS && vec_row < m) {
    const T local_sum = lds_y[tid_in_block];
    y_out[vec_row] = device_fma(beta, y_local, alpha * local_sum);
  }
}

// memory coalescing of loading and storing vector y at wavefront level.
template <int VECTOR_SIZE, int WF_SIZE, typename T>
__device__ __forceinline__ void
store_y_with_coalescing(const int gid, const int tid_in_wf, const int wf_id, const int row, const int m, const T alpha,
                        const T beta, const T sum, const T *__restrict__ y_in, T *__restrict__ y_out) {
  constexpr int WF_VECTORS = WF_SIZE / VECTOR_SIZE;

  dbl_b32_t recv_sum;
  recv_sum.val = mv_to_front_lanes_f64<VECTOR_SIZE, WF_VECTORS, WF_SIZE>(sum, gid, tid_in_wf, wf_id);

  int vec_row = row - tid_in_wf / VECTOR_SIZE + tid_in_wf;
  if (tid_in_wf < WF_VECTORS && vec_row < m) {
    y_out[vec_row] = device_fma(beta, y_in[vec_row], alpha * recv_sum.val);
  }
}

/**
 * We solve SpMV with vector method.
 * In this method, wavefront can be divided into several groups (wavefront must be divided with no remainder).
 * (e.g. groups size can only be 1, 2,4,8,16,32,64 if \tparam WF_SIZE is 64).
 * Here, one group of threads are called a "vector".
 * Then, each vector can process one row of matrix A,
 * which also means one wavefront with multiple vectors can compute multiple rows.
 *
 * @tparam VECTOR_SIZE threads in one vector
 * @tparam THREADS threads in one block
 * @tparam WF_SIZE threads in one wavefront
 * @tparam T type of data in matrix A, vector x, vector y and alpha, beta.
 * @param m rows in matrix A
 * @param alpha alpha value
 * @param beta beta value
 * @param row_offset row offset array of csr matrix A
 * @param csr_col_ind col index of csr matrix A
 * @param csr_val matrix A in csr format
 * @param x vector x
 * @param y vector y
 * @return
 */
template <int VECTOR_SIZE, int THREADS, int WF_SIZE, typename T>
__global__ void native_vector_row_kernel(int m, const T alpha, const T beta, const int *row_offset,
                                         const int *csr_col_ind, const T *csr_val, const T *x, T *y) {
  const int global_thread_id = threadIdx.x + blockDim.x * blockIdx.x;
  const int vector_thread_id = global_thread_id % VECTOR_SIZE; // local thread id in current vector
  const int vector_id = global_thread_id / VECTOR_SIZE;        // global vector id
  const int vector_num = gridDim.x * blockDim.x / VECTOR_SIZE; // total vectors on device

  const int tid_in_wf = global_thread_id % WF_SIZE;
  const int wf_id = global_thread_id / WF_SIZE;

  __shared__ T lds_y[THREADS / VECTOR_SIZE];

  for (int row = vector_id; row < m; row += vector_num) {
    const int row_start = row_offset[row];
    const int row_end = row_offset[row + 1];
    T sum = static_cast<T>(0);

    for (int i = row_start + vector_thread_id; i < row_end; i += VECTOR_SIZE) {
      asm_v_fma_f64(csr_val[i], device_ldg(x + csr_col_ind[i]), sum);
    }

    // reduce inside a vector
    for (int i = VECTOR_SIZE >> 1; i > 0; i >>= 1) {
      sum += __shfl_down(sum, i, VECTOR_SIZE);
    }

#ifdef MEMORY_ACCESS_Y_COALESCING
    block_store_y_with_coalescing<THREADS, VECTOR_SIZE, WF_SIZE, T>(tid_in_wf, global_thread_id, row, m, alpha, beta,
                                                                    sum, y, y, lds_y);
//    store_y_with_coalescing<VECTOR_SIZE, WF_SIZE, T>(global_thread_id, tid_in_wf, wf_id, row, m, alpha, beta, sum, y,
//                                                   y);
#endif
#ifndef MEMORY_ACCESS_Y_COALESCING
    if (vector_thread_id == 0) {
      y[row] = device_fma(beta, y[row], alpha * sum);
    }
#endif
  }
}

#define NATIVE_VECTOR_KERNEL_WRAPPER(N)                                                                                \
  (native_vector_row_kernel<N, 256, __WF_SIZE__, double>)<<<512, 256>>>(m, alpha, beta, rowptr, colindex, value, x, y);

#endif // SPMV_ACC_VECTOR_ROW_NATIVE_HPP
