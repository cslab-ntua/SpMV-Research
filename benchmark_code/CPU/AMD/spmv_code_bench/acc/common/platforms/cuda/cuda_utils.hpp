//
// Created by genshen on 2021/11/12.
//

#ifndef SPMV_ACC_PLATFORMS_CUDA_UTILS_H
#define SPMV_ACC_PLATFORMS_CUDA_UTILS_H

// see also: https://stackoverflow.com/a/39287554
#if !defined(__CUDA_ARCH__) || __CUDA_ARCH__ >= 600
#else
static __inline__ __device__ double atomicAdd(double *address, double val) {
  unsigned long long int *address_as_ull = (unsigned long long int *)address;
  unsigned long long int old = *address_as_ull, assumed;
  if (val == 0.0)
    return __longlong_as_double(old);
  do {
    assumed = old;
    old = atomicCAS(address_as_ull, assumed, __double_as_longlong(val + __longlong_as_double(assumed)));
  } while (assumed != old);

  return __longlong_as_double(old);
}
#endif

#endif // SPMV_ACC_PLATFORMS_CUDA_UTILS_H
