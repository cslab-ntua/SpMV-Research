//
// Created by chu genshen on 2021/9/8.
//

#ifndef SPMV_ACC_FLAT_REDUCE_HPP
#define SPMV_ACC_FLAT_REDUCE_HPP

#include "../common/utils.h"

template <typename I, typename T, int NNZ_PER_BLOCK, int THREADS, int VECTOR_SIZE>
__device__ __forceinline__ void
flat_reduce_in_vector(const int n_reduce_rows_num, const int tid_in_block, const int bp_index,
                      const I reduce_start_row_id, const I reduce_end_row_id, const T alpha,
                      const I *__restrict__ row_offset, const T *__restrict__ _lds_shared_data, T *__restrict__ y) {
  // use `vec_num` vectors, each vector can process reduction of one row by involving `vec_size` threads.
  const I vec_size = VECTOR_SIZE;
  const I vec_num = THREADS / vec_size;
  const I vec_id = tid_in_block / vec_size;
  const I tid_in_vec = tid_in_block % vec_size;

  I reduce_row_id = reduce_start_row_id + vec_id;
  if (reduce_row_id < reduce_end_row_id) {
    const I reduce_start_inx = max(0, row_offset[reduce_row_id] - bp_index * NNZ_PER_BLOCK);
    const I reduce_end_inx = min(NNZ_PER_BLOCK, row_offset[reduce_row_id + 1] - bp_index * NNZ_PER_BLOCK);
    // reduce LDS via vectors.
    T sum = static_cast<T>(0);
    for (int i = reduce_start_inx + tid_in_vec; i < reduce_end_inx; i += vec_size) {
      sum += _lds_shared_data[i];
    }
    // reduce in vector
    for (int i = vec_size >> 1; i > 0; i >>= 1) {
      sum += __shfl_down(sum, i, vec_size);
    }
    // store value
    if (tid_in_vec == 0) {
      atomicAdd(y + reduce_row_id, alpha * sum);
    }
  }

  reduce_row_id += vec_num;
  for (; reduce_row_id < reduce_end_row_id; reduce_row_id += vec_num) {
    const I reduce_start_inx = max(0, row_offset[reduce_row_id] - bp_index * NNZ_PER_BLOCK);
    const I reduce_end_inx = min(NNZ_PER_BLOCK, row_offset[reduce_row_id + 1] - bp_index * NNZ_PER_BLOCK);
    // reduce LDS via vectors.
    T sum = static_cast<T>(0);
    for (int i = reduce_start_inx + tid_in_vec; i < reduce_end_inx; i += vec_size) {
      sum += _lds_shared_data[i];
    }
    // reduce in vector
    for (int i = vec_size >> 1; i > 0; i >>= 1) {
      sum += __shfl_down(sum, i, vec_size);
    }
    // store value
    if (tid_in_vec == 0) {
      atomicAdd(y + reduce_row_id, alpha * sum);
    }
  }
}

template <typename I, typename T, int NNZ_PER_BLOCK, int THREADS>
__device__ __forceinline__ void flat_reduce_direct(const int tid_in_block, const int bp_index,
                                                   const I reduce_start_row_id, const I reduce_end_row_id,
                                                   const T alpha, const I *__restrict__ row_offset,
                                                   const T *__restrict__ _lds_shared_data, T *__restrict__ y) {
  I reduce_row_id = reduce_start_row_id + tid_in_block;
  if (reduce_row_id < reduce_end_row_id) {
    T sum = static_cast<T>(0);
    // what if it has a very long row? which means `reduce_start_row_id == reduce_end_row_id`.
    const I reduce_start_inx = max(0, row_offset[reduce_row_id] - bp_index * NNZ_PER_BLOCK);
    const I reduce_end_inx = min(NNZ_PER_BLOCK, row_offset[reduce_row_id + 1] - bp_index * NNZ_PER_BLOCK);
    for (int i = reduce_start_inx; i < reduce_end_inx; i++) {
      sum += _lds_shared_data[i];
    }
    atomicAdd(y + reduce_row_id, alpha * sum);
  }
  reduce_row_id += THREADS;
  for (; reduce_row_id < reduce_end_row_id; reduce_row_id += THREADS) {
    T sum = static_cast<T>(0);
    const I reduce_start_inx = max(0, row_offset[reduce_row_id] - bp_index * NNZ_PER_BLOCK);
    const I reduce_end_inx = min(NNZ_PER_BLOCK, row_offset[reduce_row_id + 1] - bp_index * NNZ_PER_BLOCK);
    for (int i = reduce_start_inx; i < reduce_end_inx; i++) {
      sum += _lds_shared_data[i];
    }
    atomicAdd(y + reduce_row_id, alpha * sum);
  }
}

// reduce results of a row in a vector with memory coalescing.
template <typename I, typename T, int NNZ_PER_BLOCK, int THREADS, int VECTOR_SIZE>
__device__ __forceinline__ void
flat_reduce_in_vector_with_mem_coalescing(const int n_reduce_rows_num, const int tid_in_block, const int bp_index,
                                          const I reduce_start_row_id, const I reduce_end_row_id, const T alpha,
                                          const I *__restrict__ row_offset, const T *__restrict__ _lds_shared_data,
                                          T *__restrict__ y) {
  // use `vec_num` vectors, each vector can process reduction of one row by involving `vec_size` threads.
  constexpr I vec_size = VECTOR_SIZE;
  constexpr I vec_num = THREADS / vec_size;
  const I vec_id = tid_in_block / vec_size;
  const I tid_in_vec = tid_in_block % vec_size;
  __shared__ T lds_y[THREADS / VECTOR_SIZE];

  I reduce_row_id = reduce_start_row_id + vec_id;
  I thread_reduce_row_id = reduce_start_row_id + tid_in_block;
  if (reduce_row_id < reduce_end_row_id) {
    const I reduce_start_inx = max(0, row_offset[reduce_row_id] - bp_index * NNZ_PER_BLOCK);
    const I reduce_end_inx = min(NNZ_PER_BLOCK, row_offset[reduce_row_id + 1] - bp_index * NNZ_PER_BLOCK);
    // reduce LDS via vectors.
    T sum = static_cast<T>(0);
    for (int i = reduce_start_inx + tid_in_vec; i < reduce_end_inx; i += vec_size) {
      sum += _lds_shared_data[i];
    }
    // reduce in vector
    for (int i = vec_size >> 1; i > 0; i >>= 1) {
      sum += __shfl_down(sum, i, vec_size);
    }
    // store sum value to y with memory coalescing
    if (tid_in_vec == 0) {
      lds_y[vec_id] = sum;
    }
  }
  __syncthreads();

  if (reduce_row_id < reduce_end_row_id) {
    // store sum value to y with memory coalescing
    if (tid_in_block < vec_num && thread_reduce_row_id < reduce_end_row_id) {
      const T local_sum = lds_y[tid_in_block];
      atomicAdd(y + thread_reduce_row_id, alpha * local_sum);
    }
  }

  reduce_row_id += vec_num;
  thread_reduce_row_id += vec_num;
  const int n_loop = (n_reduce_rows_num / vec_num) + (n_reduce_rows_num % vec_num == 0 ? 0 : 1) - 1;
  for (int k = 0; k < n_loop; k++) {
    if (reduce_row_id < reduce_end_row_id) {
      const I reduce_start_inx = max(0, row_offset[reduce_row_id] - bp_index * NNZ_PER_BLOCK);
      const I reduce_end_inx = min(NNZ_PER_BLOCK, row_offset[reduce_row_id + 1] - bp_index * NNZ_PER_BLOCK);
      // reduce LDS via vectors.
      T sum = static_cast<T>(0);
      for (int i = reduce_start_inx + tid_in_vec; i < reduce_end_inx; i += vec_size) {
        sum += _lds_shared_data[i];
      }
      // reduce in vector
      for (int i = vec_size >> 1; i > 0; i >>= 1) {
        sum += __shfl_down(sum, i, vec_size);
      }

      if (tid_in_vec == 0) {
        lds_y[vec_id] = sum;
      }
    }

    __syncthreads();

    if (reduce_row_id < reduce_end_row_id) {
      // store sum value to y with memory coalescing
      if (thread_reduce_row_id < reduce_end_row_id && tid_in_block < vec_num) {
        const T local_sum = lds_y[tid_in_block];
        atomicAdd(y + thread_reduce_row_id, alpha * local_sum);
      }
    }
    reduce_row_id += vec_num;
    thread_reduce_row_id += vec_num;
  }
}

#endif // SPMV_ACC_FLAT_REDUCE_HPP
