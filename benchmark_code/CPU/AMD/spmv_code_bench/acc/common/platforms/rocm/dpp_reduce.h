//
// Created by genshen on 2021/11/12.
//

#ifndef SPMV_ACC_PLATFORM_ROCM_DPP_REDUCE_H
#define SPMV_ACC_PLATFORM_ROCM_DPP_REDUCE_H

#include <hip/hip_runtime.h>

// __hip_move_dpp is already defined in hip_runtime.

// DPP-based double wavefront reduction
template <unsigned int WFSIZE> __device__ __forceinline__ double dpp_wf_reduce_sum(double sum) {
  typedef union dbl_b32 {
    double val;
    uint32_t b32[2];
  } dbl_b32_t;
  dbl_b32_t upper_sum;
  dbl_b32_t temp_sum;
  temp_sum.val = sum;

  if (WFSIZE > 1) {
    upper_sum.b32[0] = __hip_move_dpp(temp_sum.b32[0], 0x111, 0xf, 0xf, false);
    upper_sum.b32[1] = __hip_move_dpp(temp_sum.b32[1], 0x111, 0xf, 0xf, false);
    temp_sum.val += upper_sum.val;
  }
  if (WFSIZE > 2) {
    upper_sum.b32[0] = __hip_move_dpp(temp_sum.b32[0], 0x112, 0xf, 0xf, false);
    upper_sum.b32[1] = __hip_move_dpp(temp_sum.b32[1], 0x112, 0xf, 0xf, false);
    temp_sum.val += upper_sum.val;
  }
  if (WFSIZE > 4) {
    upper_sum.b32[0] = __hip_move_dpp(temp_sum.b32[0], 0x114, 0xf, 0xe, false);
    upper_sum.b32[1] = __hip_move_dpp(temp_sum.b32[1], 0x114, 0xf, 0xe, false);
    temp_sum.val += upper_sum.val;
  }
  if (WFSIZE > 8) {
    upper_sum.b32[0] = __hip_move_dpp(temp_sum.b32[0], 0x118, 0xf, 0xc, false);
    upper_sum.b32[1] = __hip_move_dpp(temp_sum.b32[1], 0x118, 0xf, 0xc, false);
    temp_sum.val += upper_sum.val;
  }
  if (WFSIZE > 16) {
    upper_sum.b32[0] = __hip_move_dpp(temp_sum.b32[0], 0x142, 0xa, 0xf, false);
    upper_sum.b32[1] = __hip_move_dpp(temp_sum.b32[1], 0x142, 0xa, 0xf, false);
    temp_sum.val += upper_sum.val;
  }
  if (WFSIZE > 32) {
    upper_sum.b32[0] = __hip_move_dpp(temp_sum.b32[0], 0x143, 0xc, 0xf, false);
    upper_sum.b32[1] = __hip_move_dpp(temp_sum.b32[1], 0x143, 0xc, 0xf, false);
    temp_sum.val += upper_sum.val;
  }
  sum = temp_sum.val;
  return sum;
}

#endif // SPMV_ACC_PLATFORM_ROCM_DPP_REDUCE_H
