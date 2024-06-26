//
// Created by genshen on 2021/6/28.
//

#ifndef SPMV_ACC_VECTOR_ROW_OPT_DOUBLE_BUFFER_HPP
#define SPMV_ACC_VECTOR_ROW_OPT_DOUBLE_BUFFER_HPP

#include "../common/global_mem_ops.h"
#include "../common/utils.h"

#include "building_config.h"
#include "vector_config.h"

/**
 * one element data in csr matrix.
 * @tparam I type of column index type
 * @tparam T type of value in matrix
 */
template <typename I, typename T> struct _type_matrix_data {
  T value;   // value of matrix
  I col_ind; // column index of matrix
};

typedef int_x2 _type_row_offsets;

/**
 * load one row into local variable (e.g. register).
 * @tparam VECTOR_SIZE threads in one vector.
 * @tparam I type of index
 * @tparam T type of data in matrix
 * @param vector_thread_id thread id in current vector.
 * @param csr_val array of csr data
 * @param csr_col_ind array of csr column index
 * @param next_row_offsets the start offset and end offset for one row to be loaded.
 * @param next_ele1, next_ele2, next_ele3 local variable for storing data (matrix value and column index) in one row.
 */
template <int VECTOR_SIZE, typename I, typename T>
__device__ __forceinline__ void
load_row_into_reg(const int vector_thread_id, const T *csr_val, const I *csr_col_ind,
                  const _type_row_offsets row_offsets, _type_matrix_data<I, T> &next_ele1,
                  _type_matrix_data<I, T> &next_ele2, _type_matrix_data<I, T> &next_ele3) {
  const I load_count = row_offsets.b - row_offsets.a;
  I cursor = row_offsets.a + vector_thread_id;
  if (vector_thread_id < load_count) {
    // for vector size 2, each thread load 1 element.
    next_ele1.value = csr_val[cursor];
    next_ele1.col_ind = csr_col_ind[cursor];
  }
  cursor += VECTOR_SIZE;
  if (vector_thread_id + VECTOR_SIZE < load_count) {
    // for vector size 2, each thread load 2 elements.
    next_ele2.value = csr_val[cursor];
    next_ele2.col_ind = csr_col_ind[cursor];
  }
  cursor += VECTOR_SIZE;
  if (vector_thread_id + 2 * VECTOR_SIZE < load_count) {
    // for vector size 2, each thread load 3 elements.
    next_ele3.value = csr_val[cursor];
    next_ele3.col_ind = csr_col_ind[cursor];
  } else {
    // todo: if one row has more than 6 elements.
  }
}

template <int VECTOR_SIZE, typename I, typename T>
__device__ __forceinline__ void load_vec_x_into_reg(const int vector_thread_id, const I load_count, const T *x,
                                                    T &next_x1, T &next_x2, T &next_x3, const I col_ind1,
                                                    const I col_ind2, const I col_ind3) {
  if (vector_thread_id < load_count) {
    // for vector size 2, each thread load 1 element.
    next_x1 = device_ldg(x + col_ind1);
  }
  if (vector_thread_id + VECTOR_SIZE < load_count) {
    // for vector size 2, each thread load 2 elements.
    next_x2 = device_ldg(x + col_ind2);
  }
  if (vector_thread_id + 2 * VECTOR_SIZE < load_count) {
    // for vector size 2, each thread load 3 elements.
    next_x3 = device_ldg(x + col_ind3);
  } else {
    // todo: if one row has more than 6 elements.
  }
}

/**
 * pipeline support of vector strategy.
 * Load csr_value, csr_col_index and x vector asynchronously.
 * @tparam VECTOR_SIZE
 * @tparam WF_VECTORS
 * @tparam WF_SIZE
 * @tparam BLOCKS
 * @tparam I
 * @tparam T
 * @param m
 * @param alpha
 * @param beta
 * @param row_offset
 * @param csr_col_ind
 * @param csr_val
 * @param x
 * @param y
 * @return
 */
template <int VECTOR_SIZE, int WF_VECTORS, int WF_SIZE, int BLOCKS, typename I, typename T>
__global__ void vector_row_kernel_pipeline(int m, const T alpha, const T beta, const I *row_offset,
                                           const I *csr_col_ind, const T *csr_val, const T *x, T *y) {
  const int global_thread_id = threadIdx.x + blockDim.x * blockIdx.x;
  const int vector_thread_id = global_thread_id % VECTOR_SIZE; // local thread id in current vector
  const int vector_id = global_thread_id / VECTOR_SIZE;        // global vector id
  const int vector_num = gridDim.x * blockDim.x / VECTOR_SIZE; // total vectors on device

  // todo: assert(vector_id < m);
  _type_row_offsets next_row_offsets{0, 0};
  next_row_offsets.a = row_offset[vector_id];
  next_row_offsets.b = row_offset[vector_id + 1];

  // todo: assert(vector_id + vector_num < m);
  _type_row_offsets next_next_row_offsets{0, 0};
  next_next_row_offsets.a = row_offset[vector_id + vector_num];
  next_next_row_offsets.b = row_offset[vector_id + vector_num + 1];

  _type_matrix_data<I, T> next_ele1, next_ele2, next_ele3;
  _type_matrix_data<I, T> next_next_ele1, next_next_ele2, next_next_ele3;
  load_row_into_reg<VECTOR_SIZE, I, T>(vector_thread_id, csr_val, csr_col_ind, next_row_offsets, next_ele1, next_ele2,
                                       next_ele3);
  load_row_into_reg<VECTOR_SIZE, I, T>(vector_thread_id, csr_val, csr_col_ind, next_next_row_offsets, next_next_ele1,
                                       next_next_ele2, next_next_ele3);

  T next_x1 = 0.0, next_x2 = 0.0, next_x3 = 0.0;
  const I outer_load_count = next_row_offsets.b - next_row_offsets.a;
  load_vec_x_into_reg<VECTOR_SIZE, I, T>(vector_thread_id, outer_load_count, x, next_x1, next_x2, next_x3,
                                         next_ele1.col_ind, next_ele2.col_ind, next_ele3.col_ind);

  for (I row = vector_id; row < m; row += vector_num) {
    const I row_start = next_row_offsets.a;
    const I row_end = next_row_offsets.b;
    const I next_row_inx = row + vector_num;
    const I next_next_row_inx = next_row_inx + vector_num;

    if (next_next_row_inx < m) {
      next_row_offsets.a = next_next_row_offsets.a;
      next_row_offsets.b = next_next_row_offsets.b;
      next_next_row_offsets.a = row_offset[next_next_row_inx];
      next_next_row_offsets.b = row_offset[next_next_row_inx + 1];
    } else if (next_row_inx < m) {
      next_row_offsets.a = next_next_row_offsets.a;
      next_row_offsets.b = next_next_row_offsets.b;
    }

    T sum = static_cast<T>(0);

    const _type_matrix_data<I, T> cur_ele1 = next_ele1, cur_ele2 = next_ele2, cur_ele3 = next_ele3;
    next_ele1 = next_next_ele1, next_ele2 = next_next_ele2, next_ele3 = next_next_ele3;

    // In fact, it is not necessary to make this function call with `if (next_row_inx < m)` wrapped.
    // Because, if current row is the last row, value of `next_row_offsets` will not get changed.
    // Then, it will still load data of current row.
    load_row_into_reg<VECTOR_SIZE, I, T>(vector_thread_id, csr_val, csr_col_ind, next_next_row_offsets, next_next_ele1,
                                         next_next_ele2, next_next_ele3);

    // load next x.
    const T x1 = next_x1, x2 = next_x2, x3 = next_x3;
    const I load_count = next_row_offsets.b - next_row_offsets.a;
    load_vec_x_into_reg<VECTOR_SIZE, I, T>(vector_thread_id, load_count, x, next_x1, next_x2, next_x3,
                                           next_ele1.col_ind, next_ele2.col_ind, next_ele3.col_ind);

    // calculation
    const I cur_data_count = row_end - row_start;
    if (vector_thread_id < cur_data_count) { // cur_data_count <= 2 &&
      asm_v_fma_f64(cur_ele1.value, x1, sum);
    }
    if (vector_thread_id + VECTOR_SIZE < cur_data_count) {
      asm_v_fma_f64(cur_ele2.value, x2, sum);
    }
    if (vector_thread_id + 2 * VECTOR_SIZE < cur_data_count) {
      asm_v_fma_f64(cur_ele3.value, x3, sum);
    } else {
      // todo:
    }

    // reduce inside a vector
#pragma unroll
    for (int i = VECTOR_SIZE >> 1; i > 0; i >>= 1) {
      sum += __shfl_down(sum, i, VECTOR_SIZE);
    }

    if (vector_thread_id == 0) {
      y[row] = device_fma(beta, y[row], alpha * sum);
    }
  }
}

/**
 * double buffer support of vector strategy..
 * @tparam VECTOR_SIZE
 * @tparam WF_VECTORS
 * @tparam WF_SIZE
 * @tparam BLOCKS
 * @tparam I
 * @tparam T
 * @param m
 * @param alpha
 * @param beta
 * @param row_offset
 * @param csr_col_ind
 * @param csr_val
 * @param x
 * @param y
 * @return
 */
template <int VECTOR_SIZE, int WF_VECTORS, int WF_SIZE, int BLOCKS, typename I, typename T>
__global__ void spmv_vector_row_kernel_double_buffer(int m, const T alpha, const T beta, const I *row_offset,
                                                     const I *csr_col_ind, const T *csr_val, const T *x, T *y) {
  const int global_thread_id = threadIdx.x + blockDim.x * blockIdx.x;
  const int vector_thread_id = global_thread_id % VECTOR_SIZE; // local thread id in current vector
  const int vector_id = global_thread_id / VECTOR_SIZE;        // global vector id
  const int vector_num = gridDim.x * blockDim.x / VECTOR_SIZE; // total vectors on device

  // todo: assert(vector_id < m);
  _type_row_offsets next_row_offsets{0, 0};
  next_row_offsets.a = row_offset[vector_id];
  next_row_offsets.b = row_offset[vector_id + 1];

  _type_matrix_data<I, T> next_ele1, next_ele2, next_ele3;
  load_row_into_reg<VECTOR_SIZE, I, T>(vector_thread_id, csr_val, csr_col_ind, next_row_offsets, next_ele1, next_ele2,
                                       next_ele3);

  for (I row = vector_id; row < m; row += vector_num) {
    const I row_start = next_row_offsets.a;
    const I row_end = next_row_offsets.b;
    const I next_row_inx = row + vector_num;
    if (next_row_inx < m) {
      next_row_offsets.a = row_offset[next_row_inx];
      next_row_offsets.b = row_offset[next_row_inx + 1];
    }

    T sum = static_cast<T>(0);

    const _type_matrix_data<I, T> cur_ele1 = next_ele1, cur_ele2 = next_ele2, cur_ele3 = next_ele3;

    // In fact, it is not necessary to make this function call with `if (next_row_inx < m)` wrapped.
    // Because, if current row is the last row, value of `next_row_offsets` will not get changed.
    // Then, it will still load data of current row.
    load_row_into_reg<VECTOR_SIZE, I, T>(vector_thread_id, csr_val, csr_col_ind, next_row_offsets, next_ele1, next_ele2,
                                         next_ele3);

    // calculation
    const I cur_data_count = row_end - row_start;
    if (vector_thread_id < cur_data_count) { // cur_data_count <= 2 &&
      asm_v_fma_f64(cur_ele1.value, device_ldg(x + cur_ele1.col_ind), sum);
    }
    if (vector_thread_id + VECTOR_SIZE < cur_data_count) {
      asm_v_fma_f64(cur_ele2.value, device_ldg(x + cur_ele2.col_ind), sum);
    }
    if (vector_thread_id + 2 * VECTOR_SIZE < cur_data_count) {
      asm_v_fma_f64(cur_ele3.value, device_ldg(x + cur_ele3.col_ind), sum);
    } else {
      // todo:
    }

    // reduce inside a vector
#pragma unroll
    for (int i = VECTOR_SIZE >> 1; i > 0; i >>= 1) {
      sum += __shfl_down(sum, i, VECTOR_SIZE);
    }

    if (vector_thread_id == 0) {
      y[row] = device_fma(beta, y[row], alpha * sum);
    }
  }
}

/**
 * legacy double buffer support of vector strategy.
 * @tparam VECTOR_SIZE
 * @tparam WF_VECTORS
 * @tparam WF_SIZE
 * @tparam BLOCKS
 * @tparam I
 * @tparam T
 * @param m
 * @param alpha
 * @param beta
 * @param row_offset
 * @param csr_col_ind
 * @param csr_val
 * @param x
 * @param y
 * @return
 */
template <int VECTOR_SIZE, int WF_VECTORS, int WF_SIZE, int BLOCKS, typename I, typename T>
__global__ void spmv_vector_row_kernel_double_buffer_legacy(int m, const T alpha, const T beta, const I *row_offset,
                                                            const I *csr_col_ind, const T *csr_val, const T *x, T *y) {
  const int global_thread_id = threadIdx.x + blockDim.x * blockIdx.x;
  const int vector_thread_id = global_thread_id % VECTOR_SIZE; // local thread id in current vector
  const int vector_id = global_thread_id / VECTOR_SIZE;        // global vector id
  const int vector_num = gridDim.x * blockDim.x / VECTOR_SIZE; // total vectors on device

  // todo: assert(vector_id < m);
  _type_row_offsets next_row_offsets{0, 0};
  next_row_offsets.a = row_offset[vector_id];
  next_row_offsets.b = row_offset[vector_id + 1];
  _type_matrix_data<I, T> buffer1{
      csr_val[next_row_offsets.a + vector_thread_id],
      csr_col_ind[next_row_offsets.a + vector_thread_id],
  };

  for (I row = vector_id; row < m; row += vector_num) {
    const I row_start = next_row_offsets.a;
    const I row_end = next_row_offsets.b;
    const I next_row_inx = row + vector_num;
    if (next_row_inx < m) {
#ifdef ASYNC_LOAD
      global_load_intx2_async(static_cast<const void *>(row_offset + next_row_inx), next_row_offsets);
#endif
#ifndef ASYNC_LOAD
      next_row_offsets.a = row_offset[next_row_inx];
      next_row_offsets.b = row_offset[next_row_inx + 1];
#endif
    }

    T sum = static_cast<T>(0);

    for (I i = row_start + vector_thread_id; i < row_end; i += VECTOR_SIZE) {
#ifdef ASYNC_LOAD
      s_waitcnt(); // wait loading row offset and buffer1
#endif
      const T cur_csr_value = buffer1.value;
      const I cur_csr_col_inx = buffer1.col_ind;

      asm_v_fma_f64(cur_csr_value, device_ldg(x + cur_csr_col_inx), sum);
      const I next_col_inx = i + VECTOR_SIZE;
      // load next element
      if (next_col_inx < row_end) {
#ifdef ASYNC_LOAD
        global_load_dbl_async(static_cast<const void *>(csr_val + next_col_inx), buffer1.value);
        global_load_int_async(static_cast<const void *>(csr_col_ind + next_col_inx), buffer1.col_ind);
#endif
#ifndef ASYNC_LOAD
        buffer1.value = csr_val[next_col_inx];
        buffer1.col_ind = csr_col_ind[next_col_inx];
#endif
      }
    }

    // load first element of next row/line
    // note: what if the last row (or some row) have no element.
    const I next_ele_index = next_row_offsets.a + vector_thread_id;
    if (next_ele_index < next_row_offsets.b && next_row_inx < m) {
#ifdef ASYNC_LOAD
      global_load_dbl_async(static_cast<const void *>(csr_val + next_ele_index), buffer1.value);
      global_load_int_async(static_cast<const void *>(csr_col_ind + next_ele_index), buffer1.col_ind);
#endif
#ifndef ASYNC_LOAD
      buffer1.value = csr_val[next_ele_index];
      buffer1.col_ind = csr_col_ind[next_ele_index];
#endif
    }

    // reduce inside a vector
#pragma unroll
    for (int i = VECTOR_SIZE >> 1; i > 0; i >>= 1) {
      sum += __shfl_down(sum, i, VECTOR_SIZE);
    }

    if (vector_thread_id == 0) {
      y[row] = device_fma(beta, y[row], alpha * sum);
    }
  }
}

#define VECTOR_KERNEL_WRAPPER_PIPELINE(N)                                                                              \
  (vector_row_kernel_pipeline<N, (__WF_SIZE__ / N), __WF_SIZE__, 512, int, double>)<<<512, 256>>>(                     \
      m, alpha, beta, rowptr, colindex, value, x, y)

#define VECTOR_KERNEL_WRAPPER_DB_BUFFER(N)                                                                             \
  (spmv_vector_row_kernel_double_buffer<N, (__WF_SIZE__ / N), __WF_SIZE__, 512, int, double>)<<<512, 256>>>(           \
      m, alpha, beta, rowptr, colindex, value, x, y)

#define VECTOR_KERNEL_WRAPPER_DB_BUFFER_LEGACY(N)                                                                      \
  (spmv_vector_row_kernel_double_buffer_legacy<N, (__WF_SIZE__ / N), __WF_SIZE__, 512, int, double>)<<<512, 256>>>(    \
      m, alpha, beta, rowptr, colindex, value, x, y)

#endif // SPMV_ACC_VECTOR_ROW_OPT_DOUBLE_BUFFER_HPP
