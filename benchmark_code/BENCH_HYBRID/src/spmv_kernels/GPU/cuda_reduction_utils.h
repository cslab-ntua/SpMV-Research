#ifndef CUDA_REDUCTION_UTILS_H
#define CUDA_REDUCTION_UTILS_H

#include <cuda_runtime.h>
#include "../spmv_kernel.h"

#ifdef __cplusplus
extern "C" {
#endif

void launch_gpu_vector_add(const ValueType* y_cpu_buf, ValueType* y, int m, cudaStream_t stream);
void launch_gpu_vector_add2(const ValueType* y_cpu_buf, const ValueType* y_gpu_buf, ValueType* y, int m, cudaStream_t stream);

#ifdef __cplusplus
}
#endif

#endif // CUDA_REDUCTION_UTILS_H
