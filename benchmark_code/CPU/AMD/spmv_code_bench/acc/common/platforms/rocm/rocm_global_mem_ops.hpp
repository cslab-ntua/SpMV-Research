//
// Created by genshen on 2021/5/7.
//

#ifndef SPMV_ACC_ROCM_GLOBAL_MEM_OPS_HPP
#define SPMV_ACC_ROCM_GLOBAL_MEM_OPS_HPP

typedef struct {
  double a;
  double b;
} dbl_x2;

typedef struct {
  int a;
  int b;
} int_x2;

/**
 * load data from memory with the address specified by @param ptr to variable @param val
 * @tparam offset
 * @param ptr target address
 * @param val the receive variable
 * @return
 */
__device__ __forceinline__ void global_load(const void *ptr, dbl_x2 &val) {
  asm volatile("global_load_dwordx4 %0, %1, off " : "=v"(val) : "v"(ptr));
}

__device__ __forceinline__ void global_load_int(const void *ptr, int_x2 &val) {
  asm volatile("global_load_dwordx2 %0, %1, off \n s_waitcnt vmcnt(0) " : "=v"(val) : "v"(ptr));
}

__device__ __forceinline__ void global_load_dbl_async(const void *ptr, double &val) {
  asm volatile("global_load_dwordx2 %0, %1, off" : "=v"(val) : "v"(ptr));
}

__device__ __forceinline__ void global_load_int_async(const void *ptr, int &val) {
  asm volatile("global_load_dword %0, %1, off" : "=v"(val) : "v"(ptr));
}

__device__ __forceinline__ void global_load_intx2_async(const void *ptr, int_x2 &val) {
  asm volatile("global_load_dwordx2 %0, %1, off" : "=v"(val) : "v"(ptr));
}

__device__ __forceinline__ void s_waitcnt() { asm volatile("s_waitcnt vmcnt(0)"); }

#endif // SPMV_ACC_ROCM_GLOBAL_MEM_OPS_HPP
