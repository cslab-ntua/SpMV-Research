//
// Created by chaohu on 2021/05/25.
//

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <iostream>

#include <hip/hip_runtime.h>
#include <hip/hip_runtime_api.h>

#include "../common/global_mem_ops.h"
#include "../common/utils.h"
#include "building_config.h"

constexpr int N_UNROLLING = 2;
constexpr int N_UNROLLING_SHIFT = 1;

/**
 *
 * calculate a row using a vector in `vector-row` strategy.
 * And write the result (vector y) back to device memory.
 *
 * Note, the data of @param csr_col_ind and @param csr_val can come from LDS or device memory.
 * @tparam I index type
 * @tparam T type of data in matrix (e.g. double or float).
 * @tparam WITH_OFFSET specific index offset when indexing csr matrix data.
 * @tparam WF_SIZE wavefront size
 * @tparam VECTOR_SIZE vector size, number of threads in a vector.
 * @param vector_thread_id current thread id in current vector.
 * @param thread_index_offset offset index for current thread used in inner column iteration.
 * @param row current row id to be calculated.
 * @param m total number of rows in matrix
 * @param alpha alpha value
 * @param beta beta value
 * @param row_offset row offset array of csr matrix A.
 * @param csr_col_ind col index of csr matrix A. It may located in LDS or device memory.
 * @param csr_val matrix A in csr format. It may located in LDS or device memory.
 * @param x vector x
 * @param y vector y
 */
template <typename I, typename T, bool WITH_OFFSET, unsigned int WF_SIZE, unsigned int VECTOR_SIZE>
__device__ __forceinline__ void vector_calc_a_row(const int vector_thread_id, const I thread_index_offset, I row, I m,
                                                  const T alpha, const T beta, const I *row_offset,
                                                  const I *csr_col_ind, const T *csr_val, const T *x, T *y) {
  if (row < m) {
    const int row_start = row_offset[row];
    const int row_end = row_offset[row + 1];
    T sum = static_cast<T>(0);

    for (int i = row_start + vector_thread_id; i < row_end; i += VECTOR_SIZE) {
      if (WITH_OFFSET) {
        asm_v_fma_f64(csr_val[i - thread_index_offset], device_ldg(x + csr_col_ind[i - thread_index_offset]), sum);
      } else {
        asm_v_fma_f64(csr_val[i], device_ldg(x + csr_col_ind[i]), sum);
      }
    }

    // reduce inside a vector
    // #pragma unroll
    for (int i = VECTOR_SIZE >> 1; i > 0; i >>= 1) {
      sum += __shfl_down(sum, i, VECTOR_SIZE);
    }

    if (vector_thread_id == 0) {
      y[row] = device_fma(beta, y[row], alpha * sum);
    }
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
 * @tparam WF_SIZE threads in one wavefront
 * @tparam WF_VECTORS vectors number in one wavefront
 * @tparam BLOCKS total blocks on one GPU (blocks in one grid).
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
template <int VECTOR_SIZE, int WF_VECTORS, int WF_SIZE, int BLOCKS, typename T>
__global__ void spmv_vector_row_kernel(int m, const T alpha, const T beta, const int *row_offset,
                                       const int *csr_col_ind, const T *csr_val, const T *x, T *y) {
  const int global_thread_id = threadIdx.x + blockDim.x * blockIdx.x;
  const int vector_thread_id = global_thread_id % VECTOR_SIZE; // local thread id in current vector
  const int vector_id = global_thread_id / VECTOR_SIZE;        // global vector id
  const int vector_num = gridDim.x * blockDim.x / VECTOR_SIZE; // total vectors on device

  const int nwf_in_block = blockDim.x / WF_SIZE;          // wavefront number in a block
  const int global_wf_id = global_thread_id / WF_SIZE;    // global wavefront id
  const int thread_id_in_wf = global_thread_id % WF_SIZE; // thread id in current wavefront
  const int wf_id_in_block = threadIdx.x / WF_SIZE;       // wavefront id in current block

  constexpr unsigned int shared_len = 64 * 1024 / (BLOCKS / AVAILABLE_CU) / (sizeof(T) + sizeof(int));
  __shared__ T shared_csr[shared_len];
  __shared__ int shared_col_inx[shared_len];
  const int shared_len_wf = shared_len / nwf_in_block;            // data size in a wavefront.
  const int shared_wf_start_inx = wf_id_in_block * shared_len_wf; // start index of shared mem for current wavefront.
  T *_wf_shared_csr = shared_csr + shared_wf_start_inx;           // LDS memory for current wavefront.
  int *_wf_shared_col_inx = shared_col_inx + shared_wf_start_inx; // LDS memory for current wavefront.

  const int n_loops = m / vector_num + (m % vector_num == 0 ? 0 : 1);
  int row = vector_id;
  for (int k = 0; k < n_loops; k++) { // all threads in one wavefront will have the same `n_loops`.
    // load data into LDS.
    const int left_base_index = min((row / WF_VECTORS) * WF_VECTORS, m);
    const int right_base_index = min(left_base_index + WF_VECTORS, m);
    const int start_index = row_offset[left_base_index];
    const int end_index = row_offset[right_base_index];
    // todo: assert (end_index - start_index < shared_len/nwf_in_block)
    const int n_lds_load = end_index - start_index;
    if (n_lds_load > shared_len_wf) {
      vector_calc_a_row<int, T, false, WF_SIZE, VECTOR_SIZE>(vector_thread_id, 0, row, m, alpha, beta, row_offset,
                                                             csr_col_ind, csr_val, x, y);
    } else {
#ifdef GLOBAL_LOAD_X2
      if (n_lds_load <= WF_SIZE) { // load all data just in one round.
        if (thread_id_in_wf < n_lds_load) {
          _wf_shared_csr[thread_id_in_wf] = csr_val[start_index + thread_id_in_wf];
          _wf_shared_col_inx[thread_id_in_wf] = csr_col_ind[start_index + thread_id_in_wf];
        }
      } else {
        // unrolling
        const int unrolling_loop_end = start_index + ((n_lds_load >> N_UNROLLING_SHIFT) << N_UNROLLING_SHIFT);
        for (int j = start_index + N_UNROLLING * thread_id_in_wf; j < unrolling_loop_end; j += N_UNROLLING * WF_SIZE) {
          dbl_x2 dbl_v_x2;
          int_x2 int_v_x2;
          global_load(static_cast<const void *>(csr_val + j), dbl_v_x2);
          global_load_int(static_cast<const void *>(csr_col_ind + j), int_v_x2);
          _wf_shared_csr[j - start_index] = dbl_v_x2.a;
          _wf_shared_csr[j - start_index + 1] = dbl_v_x2.b;
          _wf_shared_col_inx[j - start_index] = int_v_x2.a;
          _wf_shared_col_inx[j - start_index + 1] = int_v_x2.b;
        }
        for (int j = unrolling_loop_end + thread_id_in_wf; j < end_index; j += WF_SIZE) {
          _wf_shared_csr[j - start_index] = csr_val[j];
          _wf_shared_col_inx[j - start_index] = csr_col_ind[j];
        }
      }
#endif
#ifndef GLOBAL_LOAD_X2
      for (int i = start_index + thread_id_in_wf; i < end_index; i += WF_SIZE) {
        _wf_shared_csr[i - start_index] = csr_val[i];
        _wf_shared_col_inx[i - start_index] = csr_col_ind[i];
      }
#endif

      // calculate
      vector_calc_a_row<int, T, true, WF_SIZE, VECTOR_SIZE>(vector_thread_id, start_index, row, m, alpha, beta,
                                                            row_offset, _wf_shared_col_inx, _wf_shared_csr, x, y);
    }
    row += vector_num;
  }
}

#define VECTOR_KERNEL_WRAPPER(N)                                                                                       \
  (spmv_vector_row_kernel<N, (__WF_SIZE__ / N), __WF_SIZE__, 512, double>)<<<512, 256>>>(m, alpha, beta, rowptr, colindex, value, x, y)
