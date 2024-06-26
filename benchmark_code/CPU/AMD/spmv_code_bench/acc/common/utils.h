//
// Created by genshen on 2021/4/18.
//

#ifndef SPMV_ACC_COMMON_UTILS_H
#define SPMV_ACC_COMMON_UTILS_H

#include <hip/hip_runtime.h>

#include "building_config.h"
#ifdef __HIP_PLATFORM_NVCC__
#include "platforms/cuda/cuda_utils.hpp"
#endif

#ifdef __HIP_PLATFORM_HCC__
#include "../common/platforms/rocm/dpp_reduce.h"
#endif

#ifndef __HIP_PLATFORM_HCC__
template <typename T> __forceinline__ __device__ T __builtin_nontemporal_load(const T *addr) { return *addr; }

template <typename T> __forceinline__ __device__ void __builtin_nontemporal_store(const T &val, T *ptr) { *ptr = val; }
#endif

#define device_ldg(ptr) __ldg(ptr)

#define device_fma(p, q, r) fma(p, q, r)

#ifdef __HIP_PLATFORM_HCC__
#define asm_v_fma_f64(p, q, r) asm volatile("v_fma_f64 %0, %1, %2, %3" : "=v"(r) : "v"(p), "v"(q), "v"(r));
#endif // __HIP_PLATFORM_HCC__

#ifndef __HIP_PLATFORM_HCC__
#define asm_v_fma_f64(p, q, r) (r = device_fma(p, q, r));
#endif // __HIP_PLATFORM_HCC__

// register and __shfl_down based wavefront reduction
#define SHFL_DOWN_WF_REDUCE(total_sum, local_sum)                                                                      \
  {                                                                                                                    \
    total_sum += local_sum;                                                                                            \
    if (__WF_SIZE__ > 32) {                                                                                            \
      total_sum += __shfl_down(total_sum, 32, __WF_SIZE__);                                                            \
    }                                                                                                                  \
    if (__WF_SIZE__ > 16) {                                                                                            \
      total_sum += __shfl_down(total_sum, 16, __WF_SIZE__);                                                            \
    }                                                                                                                  \
    if (__WF_SIZE__ > 8) {                                                                                             \
      total_sum += __shfl_down(total_sum, 8, __WF_SIZE__);                                                             \
    }                                                                                                                  \
    if (__WF_SIZE__ > 4) {                                                                                             \
      total_sum += __shfl_down(total_sum, 4, __WF_SIZE__);                                                             \
    }                                                                                                                  \
    if (__WF_SIZE__ > 2) {                                                                                             \
      total_sum += __shfl_down(total_sum, 2, __WF_SIZE__);                                                             \
    }                                                                                                                  \
    if (__WF_SIZE__ > 1) {                                                                                             \
      total_sum += __shfl_down(total_sum, 1, __WF_SIZE__);                                                             \
    }                                                                                                                  \
  }

template <unsigned int WFSIZE> __device__ __forceinline__ double wfreduce_sum(double sum) {
#ifdef __HIP_PLATFORM_HCC__
  return dpp_wf_reduce_sum<WFSIZE>(sum);
#endif

#ifndef __HIP_PLATFORM_HCC__
  // fallback to shfl_down reduction.
  double total_sum = static_cast<double>(0);
  SHFL_DOWN_WF_REDUCE(total_sum, sum);
  return total_sum;
#endif
}

#endif // SPMV_ACC_COMMON_UTILS_H
