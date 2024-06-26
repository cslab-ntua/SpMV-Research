//
// Created by dly on 2021/4/19.
//
// spmv_csr_scalar_kernel version: one thread process one row of matrix A.

#include <iostream>
#include <stdio.h>  // printf
#include <stdlib.h> // EXIT_FAILURE

#include <hip/hip_runtime.h>
#include <hip/hip_runtime_api.h> // hipMalloc, hipMemcpy, etc.

#include "../common/global_mem_ops.h"
#include "../common/utils.h"
#include "thread_row_config.h"

template <int N, int MAX_ROW_NNZ, int WF_SIZE, int THREADS, typename I, typename T>
__global__ void kernel_thread_row(const T alpha, const T beta, const I m, const I *__restrict__ row_ptr,
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
  T *_wf_shared_val = shared_data + wf_shared_start_inx;          // LDS memory for current wavefront.

  // In each loop, each thread process N rows.
  const I wf_rounds = m / WF_SIZE + (m % (WF_SIZE) == 0 ? 0 : 1);

  constexpr int N_UNROLLING_SHIFT = 1;
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

#ifndef THREAD_ROW_GLOBAL_LOAD_X2
    for (I j = wf_start_index + tid_in_wf; j < wf_end_index; j += WF_SIZE) {
      const T local_val = csr_val[j] * x[csr_col_inx[j]];
      // sum += local_val;
      _wf_shared_val[j - wf_start_index] = local_val;
    }
#endif
#ifdef THREAD_ROW_GLOBAL_LOAD_X2
    const int n_lds_load = wf_end_index - wf_start_index;
    const int unrolling_loop_end = wf_start_index + ((n_lds_load >> N_UNROLLING_SHIFT) << N_UNROLLING_SHIFT);
    for (I j = wf_start_index + 2 * tid_in_wf; j < unrolling_loop_end; j += 2 * WF_SIZE) {
      dbl_x2 dbl_v_x2;
      int_x2 int_v_x2;
      global_load(static_cast<const void *>(csr_val + j), dbl_v_x2);
      global_load_int(static_cast<const void *>(csr_col_inx + j), int_v_x2);
      _wf_shared_val[j - wf_start_index] = dbl_v_x2.a * x[int_v_x2.a];
      _wf_shared_val[j - wf_start_index + 1] = dbl_v_x2.b * x[int_v_x2.b];
    }
    for (int j = unrolling_loop_end + tid_in_wf; j < wf_end_index; j += WF_SIZE) {
      const T local_val = csr_val[j] * x[csr_col_inx[j]];
      _wf_shared_val[j - wf_start_index] = local_val;
    }
#endif

    const T y_local = __builtin_nontemporal_load(y + reduce_row_id);
    // reduction
    // todo: multiples rows per thread support in reduction

    // The last row may be reduced and stored more than once by threads in the last wavefront,
    // but it does not matter.
    const I reduce_start_index = thread_row_start - wf_start_index;
    const I reduce_end_index = thread_row_end - wf_start_index;
    T sum = static_cast<T>(0);
    for (I k = reduce_start_index; k < reduce_end_index; k++) {
      sum += _wf_shared_val[k];
    }

    const T y_result = alpha * sum + beta * y_local;
    __builtin_nontemporal_store(y_result, y + reduce_row_id);
  }
}
