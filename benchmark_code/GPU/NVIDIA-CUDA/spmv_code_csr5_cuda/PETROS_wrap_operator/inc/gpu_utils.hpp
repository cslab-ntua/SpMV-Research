///
/// \author Anastasiadis Petros (panastas@cslab.ece.ntua.gr)
///
/// \brief Some CUDA function calls with added error-checking
///
#ifndef GPU_UTILS_H
#define GPU_UTILS_H

#include <cuda.h>
#include <spmv_utils.hpp>

/// Print all available CUDA devices and their basic info
void print_devices();

/// Check if there are CUDA errors on the stack
void cudaCheckErrors();

/// Allocate 'count' bytes of CUDA device memory (+errorcheck)
void *gpu_alloc(size_t count);

/// Free the CUDA device  memory pointed by 'gpuptr' (+errorcheck)
void gpu_free(void *gpuptr);

/// Print Free/Total CUDA device memory along with 'message' (+errorcheck)
void gpu_showMem(char *message);

/// Copy 'count' bytes from host to CUDA device (+errorcheck)
void copy_to_gpu(const void *host, void *gpu, size_t count);

/// Copy 'count' bytes from CUDA device to host (+errorcheck)
void copy_from_gpu(void *host, const void *gpu, size_t count);

/// Free the dataptr vector from mem_alloc type memory
void vec_free(void *dataptr, int size, SpmvMemType mem_alloc);

// FIXME: generalize
///
SpmvOperator **split_nz(SpmvOperator *op, int div);

__global__ void gpu_memCopy(double *destination, double *source, int size);

#endif
