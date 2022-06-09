///
/// \author Anastasiadis Petros (panastas@cslab.ece.ntua.gr)
///
/// \brief Some CUDA function calls with added error-checking
///

#include <cassert>
#include <cstdio>
//#include "CSR5.hpp"
//#include "cuSPARSE.hpp"
//#include <numa.h>
#include <unistd.h>

#include "gpu_utils.hpp"

void print_devices() {
  ddebug(" -> print_devices()\n");
  cudaDeviceProp properties;
  int nDevices = 0;
  cudaGetDeviceCount(&nDevices);
  for (int i = 0; i < nDevices; i++) {
    cudaGetDeviceProperties(&properties, i);
    printf("Device Number: %d\n", i);
    printf("  Device name: %s\n", properties.name);
    printf("  Memory Clock Rate (MHz): %d\n",
           properties.memoryClockRate / 1024);
    printf("  Memory Bus Width (bits): %d\n", properties.memoryBusWidth);
    printf("  Peak Memory Bandwidth (GB/s): %f\n",
           2.0 * properties.memoryClockRate * (properties.memoryBusWidth / 8) /
               1.0e6);
    if (properties.major >= 3)
      printf("  Unified Memory support: YES\n\n");
    else
      printf("  Unified Memory support: NO\n\n");
  }
  ddebug(" <- print_devices()\n");
}

void cudaCheckErrors() {
  massert(cudaGetLastError() == cudaSuccess,
          cudaGetErrorString(cudaGetLastError()));
}

void *gpu_alloc(size_t count) {
  void *ret;
  assert(cudaMalloc(&ret, count) == cudaSuccess);
  return ret;
}

void gpu_free(void *gpuptr) { assert(cudaFree(gpuptr) == cudaSuccess); }

void copy_to_gpu(const void *host, void *gpu, size_t count) {
  assert(cudaMemcpy(gpu, host, count, cudaMemcpyHostToDevice) == cudaSuccess);
}

void copy_from_gpu(void *host, const void *gpu, size_t count) {
  assert(cudaMemcpy(host, gpu, count, cudaMemcpyDeviceToHost) == cudaSuccess);
}

void gpu_showMem(char *message) {
  size_t free, total;
  assert(cudaMemGetInfo(&free, &total) == cudaSuccess);
  printf("showMem(%s): %u free out of %u MB \n", message, free / (1024 * 1024),
         total / (1024 * 1024));
}

void vec_free(void *dataptr, int size, SpmvMemType mem_alloc) {
  ddebug(" -> vec_free(dataptr, mem_alloc)\n");
  switch (mem_alloc) {
    case (SPMV_MEMTYPE_NUMA): {
      massert(false, "vec_free -> mem_alloc default reached");
      //numa_free(dataptr, size);
    } break;
    case (SPMV_MEMTYPE_HOST): {
      // free(dataptr);
      cudaFreeHost(dataptr);
    } break;
    case (SPMV_MEMTYPE_DEVICE):
    case (SPMV_MEMTYPE_UNIFIED): {
      gpu_free(dataptr);
    } break;
    default:
      massert(false, "vec_free -> mem_alloc default reached");
      break;
  }
  ddebug(" <- vec_free(dataptr, mem_alloc)\n");
}

__global__ void gpu_memCopy(double *destination, double *source, int size) {
  // int *dest=(int *)destination;

  // int *src=(int *)source;

  for (int tid = threadIdx.x; tid < size; tid += blockDim.x)

    destination[tid] = source[tid];
}
