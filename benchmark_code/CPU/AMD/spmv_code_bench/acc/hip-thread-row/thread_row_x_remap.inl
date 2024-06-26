//
// Created by chu genshen on 2021/8/10.
//

#ifndef SPMV_ACC_THREAD_ROW_X_REMAP_INL
#define SPMV_ACC_THREAD_ROW_X_REMAP_INL

#include <hip/hip_runtime.h>
#include <hip/hip_runtime_api.h>

#include "../common/global_mem_ops.h"
#include "../common/utils.h"
#include "thread_row_config.h"

/**
 * another thread row strategy implementation with different data loading method.
 * In this method, memory access of vector x is remapped in column-first mode.
 *
 * @tparam N each wavefront may process __WF_SIZE__ * N rows. Currently, it must be 1.
 * @tparam MAX_ROW_NNZ max non-zeros per row in the algorithm.
 *   If some row have nnz larger than @tparam MAX_ROW_NNZ/ it can fallback to native thread row method.
 * @tparam WF_SIZE threads number in a wavefront
 * @tparam BLOCKS total blocks in system.
 * @tparam THREADS threads number in block.
 *
 * @tparam I index type
 * @tparam T data type. (e.g matrix value, vector x, y)
 *
 * @param alpha, beta: the alpha and beta value
 * @param m row number in matrix A.
 * @param row_ptr row offset pointer in CSR format.
 * @param csr_col_inx column index pointer in CSR format.
 * @param csr_val matrix value in CSR format.
 * @param x vector x for @param alpha * A*x
 * @param y result vector for y = alpha*A*x + beta*y.
 * @return
 */
template <int N, int MAX_ROW_NNZ, int WF_SIZE, int THREADS, typename I, typename T>
__global__ void kernel_thread_row_v2(const T alpha, const T beta, const I m, const I *__restrict__ row_ptr,
                                     const I *__restrict__ csr_col_inx, const T *__restrict__ csr_val,
                                     const T *__restrict__ x, T *__restrict__ y) {
  int t_id = threadIdx.x + blockDim.x * blockIdx.x;
  const int global_threads_num = blockDim.x * gridDim.x;

  const int g_wf_id = t_id / WF_SIZE;                     // global wavefront id
  const int tid_in_wf = t_id % WF_SIZE;                   // thread id in current wavefront
  const int global_wf_num = global_threads_num / WF_SIZE; // wavefront number in system
  const int wf_id_in_block = threadIdx.x / WF_SIZE;       // wavefront id in current block

  constexpr int shared_len = N * THREADS * MAX_ROW_NNZ;
  __shared__ T shared_data[shared_len];
  const int shared_len_wf = N * WF_SIZE * MAX_ROW_NNZ;            // data size in a wavefront.
  const int wf_shared_start_inx = wf_id_in_block * shared_len_wf; // start index of shared mem for current
  // LDS memory (for saving matrix value and x vector) for current wavefront.
  T *_wf_shared_val = shared_data + wf_shared_start_inx;
  // LDS memory (for saving column indexes) for current wavefront.
  I *_wf_shared_indexes = (I *)((void *)(_wf_shared_val));

  // In each loop, each thread process N rows.
  const I wf_rounds = m / WF_SIZE + (m % (WF_SIZE) == 0 ? 0 : 1);

  constexpr int N_UNROLLING_SHIFT = 1;
  T *last_y_addr = nullptr;
  T last_y_value = 0;
  for (I i = N * g_wf_id; i < wf_rounds; i += N * global_wf_num) {
    // each wavefront process `N * WF_SIZE` rows.
    // In a wavefront, read data from row g_wf_id to g_wf_id + N*WF_SIZE.
    if (i * WF_SIZE >= m) {
      return;
    }
    const I wf_row_start_id = i * WF_SIZE;
    const I wf_row_end_id = min((i + N) * WF_SIZE, m);

    // we have: wf_row_start_id < wf_row_end_id and wf_row_start_id < m.
    const I reduce_row_id = min(wf_row_start_id + tid_in_wf, m - 1);
    const I thread_row_start = row_ptr[reduce_row_id];
    const I thread_row_end = row_ptr[reduce_row_id + 1];

    // thread 0 broadcast to other threads.
    const I wf_start_index = __shfl(thread_row_start, 0);
    // thread 63 broadcast to other threads).
    const I wf_end_index = __shfl(thread_row_end, WF_SIZE - 1);

    // step1: load csr column index into LDS
#ifndef THREAD_ROW_GLOBAL_LOAD_X2
    for (I j = wf_start_index + tid_in_wf; j < wf_end_index; j += WF_SIZE) {
      _wf_shared_indexes[j - wf_start_index] = csr_col_inx[j];
    }
#endif
#ifdef THREAD_ROW_GLOBAL_LOAD_X2
    {
      const int n_lds_load = wf_end_index - wf_start_index;
      const int unrolling_loop_end = wf_start_index + ((n_lds_load >> N_UNROLLING_SHIFT) << N_UNROLLING_SHIFT);
      for (I j = wf_start_index + 2 * tid_in_wf; j < unrolling_loop_end; j += 2 * WF_SIZE) {
        int_x2 int_v_x2;
        global_load_int(static_cast<const void *>(csr_col_inx + j), int_v_x2);
        s_waitcnt();
        _wf_shared_indexes[j - wf_start_index] = int_v_x2.a;
        _wf_shared_indexes[j - wf_start_index + 1] = int_v_x2.b;
      }
      for (int j = unrolling_loop_end + tid_in_wf; j < wf_end_index; j += WF_SIZE) {
        _wf_shared_indexes[j - wf_start_index] = csr_col_inx[j];
      }
    }
#endif

    // step2: remapping LDS (column index) and read x vector via the remapped column index.
    // each thread read data belonging to its row.
    T _thread_local_x_vec[MAX_ROW_NNZ];
#pragma unroll
    for (I j = 0; j < MAX_ROW_NNZ; j++) {
      const I col_addr = thread_row_start + j;
      if (col_addr < thread_row_end) {
        const I thread_col_inx = _wf_shared_indexes[col_addr - wf_start_index];
        _thread_local_x_vec[j] = x[thread_col_inx];
      }
    }

// step3: load matrix value to LDS
#ifndef THREAD_ROW_GLOBAL_LOAD_X2
    for (I j = wf_start_index + tid_in_wf; j < wf_end_index; j += WF_SIZE) {
      _wf_shared_val[j - wf_start_index] = csr_val[j];
    }
#endif
#ifdef THREAD_ROW_GLOBAL_LOAD_X2
    {
      const int n_lds_load = wf_end_index - wf_start_index;
      const int unrolling_loop_end = wf_start_index + ((n_lds_load >> N_UNROLLING_SHIFT) << N_UNROLLING_SHIFT);
      for (I j = wf_start_index + 2 * tid_in_wf; j < unrolling_loop_end; j += 2 * WF_SIZE) {
        dbl_x2 dbl_v_x2;
        global_load_dblx2_and_wait(static_cast<const void *>(csr_val + j), dbl_v_x2);
        _wf_shared_val[j - wf_start_index] = dbl_v_x2.a;
        _wf_shared_val[j - wf_start_index + 1] = dbl_v_x2.b;
      }
      for (int j = unrolling_loop_end + tid_in_wf; j < wf_end_index; j += WF_SIZE) {
        _wf_shared_val[j - wf_start_index] = csr_val[j];
      }
    }
#endif
    // preload y value.
    const T y_local = __builtin_nontemporal_load(y + reduce_row_id);

    // step4: remapping matrix value and perform multiplication of Ax.
    T sum = static_cast<T>(0);
#pragma unroll
    for (I j = 0; j < MAX_ROW_NNZ; j++) {
      const I col_addr = thread_row_start + j;
      if (col_addr < thread_row_end) {
        const T thread_value = _wf_shared_val[col_addr - wf_start_index];
        sum += thread_value * _thread_local_x_vec[j];
      }
    }
    // step5: store y
    const T y_result = alpha * sum + beta * y_local;
    __builtin_nontemporal_store(y_result, y + reduce_row_id);
  }
}

#endif // SPMV_ACC_THREAD_ROW_X_REMAP_INL
