//
// Created by genshen on 2021/11/12.
//

#ifndef SPMV_ACC_GLOBAL_MEM_OPS_H
#define SPMV_ACC_GLOBAL_MEM_OPS_H

#ifdef __HIP_PLATFORM_HCC__
#include "platforms/rocm/rocm_global_mem_ops.hpp"
#endif // __HIP_PLATFORM_HCC__

#ifndef __HIP_PLATFORM_HCC__
// fallback to normal implementation.

typedef struct {
  double a;
  double b;
} dbl_x2;

typedef struct {
  int a;
  int b;
} int_x2;

__device__ __forceinline__ void global_load_dbl_async(const void *ptr, double &val) {
  double *dbl_ptr = (double *)(ptr);
  val = *dbl_ptr;
}

__device__ __forceinline__ void global_load_int_async(const void *ptr, int &val) {
  int *int_ptr = (int *)(ptr);
  val = *int_ptr;
}

__device__ __forceinline__ void global_load_intx2_async(const void *ptr, int_x2 &val) {
  int *int_ptr = (int *)(ptr);
  val.a = *int_ptr;
  val.b = *(int_ptr + 1);
}

// load 2 double in one instruction
__device__ __forceinline__ void global_load(const void *ptr, dbl_x2 &val) {
  double *dbl_ptr = (double *)(ptr);
  val.a = *dbl_ptr;
  val.b = *(dbl_ptr + 1);
}

// load 2 int in one instruction synchronously
__device__ __forceinline__ void global_load_int(const void *ptr, int_x2 &val) {
  global_load_intx2_async(ptr, val); // fixme: no s_waitcnt.
}

__device__ __forceinline__ void s_waitcnt() {}

#endif

__device__ __forceinline__ void global_load_intx2_and_wait(const void *ptr, int_x2 &val) {
  global_load_int(ptr, val);
  s_waitcnt();
}

__device__ __forceinline__ void global_load_dblx2_and_wait(const void *ptr, dbl_x2 &val) {
  global_load(ptr, val);
  s_waitcnt();
}

#endif // SPMV_ACC_GLOBAL_MEM_OPS_H
