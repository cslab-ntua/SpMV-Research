#include "cuda_reduction_utils.h"

__global__ void vector_add_kernel(const ValueType* y_cpu_buf, ValueType* y, int m) {
    int i = blockDim.x * blockIdx.x + threadIdx.x;
    if (i < m) {
        y[i] += y_cpu_buf[i];
    }
}

__global__ void vector_add_kernel2(const ValueType* y_cpu_buf, const ValueType* y_gpu_buf, ValueType* y, int m) {
    int i = blockDim.x * blockIdx.x + threadIdx.x;
    if (i < m) {
        y[i] = y_cpu_buf[i] + y_gpu_buf[i];
    }
}

extern "C" void launch_gpu_vector_add(const ValueType* y_cpu_buf, ValueType* y, int m, cudaStream_t stream) {
    int threads = 256;
    int blocks = (m + threads - 1) / threads;
    vector_add_kernel<<<blocks, threads, 0, stream>>>(y_cpu_buf, y, m);
}

extern "C" void launch_gpu_vector_add2(const ValueType* y_cpu_buf, const ValueType* y_gpu_buf, ValueType* y, int m, cudaStream_t stream) {
    int threads = 256;
    int blocks = (m + threads - 1) / threads;
    vector_add_kernel2<<<blocks, threads, 0, stream>>>(y_cpu_buf, y_gpu_buf, y, m);
}
