//
// Created by genshen on 2021/11/14.
//

#ifndef SPMV_ACC_CROSS_LANE_OPS_H
#define SPMV_ACC_CROSS_LANE_OPS_H

#include <hip/hip_runtime.h>

typedef union dbl_b32 {
  double val;
  uint32_t b32[2];
} dbl_b32_t;

// move data from vector to front lanes
template <int VECTOR_SIZE, int WF_VECTORS, int WF_SIZE>
__device__ __forceinline__ double mv_to_front_lanes_f64(const double sum, const int gid, const int tid_in_wf,
                                                        const int wf_id) {
  dbl_b32_t vec_sum;
  dbl_b32_t recv_sum;
  vec_sum.val = sum;

#ifdef __HIP_PLATFORM_HCC__
  int src_tid = tid_in_wf * VECTOR_SIZE + wf_id * WF_SIZE; // each vector's thread-0 in current wavefront
  // or: remove if and else-if, just keep inner sentences of if.
  if (tid_in_wf < WF_VECTORS) { // load each vector's sum to the first WF_VECTORS threads in a wavefront
    recv_sum.b32[0] = __hip_ds_bpermute(4 * src_tid, vec_sum.b32[0]);
    recv_sum.b32[1] = __hip_ds_bpermute(4 * src_tid, vec_sum.b32[1]);
  } else if (tid_in_wf % VECTOR_SIZE == 0) { // active thread 0 of each vector.
    recv_sum.b32[0] = __hip_ds_bpermute(4 * gid, vec_sum.b32[0]);
    recv_sum.b32[1] = __hip_ds_bpermute(4 * gid, vec_sum.b32[1]);
  }

  __syncthreads();
#endif

#ifndef __HIP_PLATFORM_HCC__                            /* fallback to normal implementation.*/
  int src_offset = tid_in_wf * VECTOR_SIZE - tid_in_wf; // each vector's thread-0 in current wavefront
  recv_sum.val = __shfl_down(sum, src_offset);
#endif

  return recv_sum.val;
}

#endif // SPMV_ACC_CROSS_LANE_OPS_H
