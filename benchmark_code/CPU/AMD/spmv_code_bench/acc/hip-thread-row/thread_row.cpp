//
// Created by genshen on 2021/7/15.
//

#include "thread_row.h"
#include "building_config.h"
#include "thread_row_config.h"

void thread_row_sparse_spmv(int trans, const int alpha, const int beta, const csr_desc<int, double> d_csr_desc,
                            const double *d_x, double *d_y) {
  const int m = d_csr_desc.rows;
  const int n = d_csr_desc.cols;
  const int *d_row_ptr = d_csr_desc.row_ptr;
  const int *d_csr_col_index = d_csr_desc.col_index;
  const double *d_csr_value = d_csr_desc.values;

  const int avg_nnz_per_row = d_csr_desc.nnz / m;
  if (avg_nnz_per_row <= 4) {
    constexpr int MAX_ROW_NNZ = 5; // 5 is up bound.

    if (THREAD_ROW_OPT_LEVEL == THREAD_ROW_OPT_LEVEL_BLOCK) { // thread-row block level
      constexpr int BLOCKS = 7010;                            // 112 * AVAILABLE_CU;
      (kernel_thread_row_block_level<1, MAX_ROW_NNZ, __WF_SIZE__, 512, int, double>)<<<BLOCKS, 512>>>(
          alpha, beta, m, d_row_ptr, d_csr_col_index, d_csr_value, d_x, d_y);
    } else if (THREAD_ROW_OPT_LEVEL == THREAD_ROW_OPT_LEVEL_BLOCK_VEC_X) { // thread-row block level with x remapping
      constexpr int BLOCKS = 7010;                                         // 112 * AVAILABLE_CU;
      (kernel_thread_row_block_v2<1, MAX_ROW_NNZ, __WF_SIZE__, 512, int, double>)<<<BLOCKS, 512>>>(
          alpha, beta, m, d_row_ptr, d_csr_col_index, d_csr_value, d_x, d_y);
    } else if (THREAD_ROW_OPT_LEVEL == THREAD_ROW_OPT_LEVEL_BLOCK_VEC_X_SINGLE) {
      // thread-row block level with x remapping, and single row per thread.
      constexpr int THREADS_PER_BLOCK = 512;
      const int BLOCKS = m / THREADS_PER_BLOCK + (m % THREADS_PER_BLOCK == 0 ? 0 : 1);
      (kernel_thread_row_block_v3<1, MAX_ROW_NNZ, __WF_SIZE__, THREADS_PER_BLOCK, int,
                                  double>)<<<BLOCKS, THREADS_PER_BLOCK>>>(alpha, beta, m, d_row_ptr, d_csr_col_index,
                                                                          d_csr_value, d_x, d_y);
    } else if (THREAD_ROW_OPT_LEVEL == THREAD_ROW_OPT_LEVEL_REMAP_VEC_X) {
      // thread-row wavefront level with x remapping
      constexpr int BLOCKS = 112 * AVAILABLE_CU;
      (kernel_thread_row_v2<1, MAX_ROW_NNZ, __WF_SIZE__, 256, int, double>)<<<BLOCKS, 256>>>(
          alpha, beta, m, d_row_ptr, d_csr_col_index, d_csr_value, d_x, d_y);
    } else { // thread-row wavefront level
      // fallback to normal one.
      constexpr int BLOCKS = 53 * AVAILABLE_CU;
      (kernel_thread_row<1, MAX_ROW_NNZ, __WF_SIZE__, 256, int, double>)<<<BLOCKS, 256>>>(
          alpha, beta, m, d_row_ptr, d_csr_col_index, d_csr_value, d_x, d_y);
    }
  } else {
    native_thread_row<<<128, 1024>>>(trans, alpha, beta, m, n, d_row_ptr, d_csr_col_index, d_csr_value, d_x, d_y);
  }
  return;
}
