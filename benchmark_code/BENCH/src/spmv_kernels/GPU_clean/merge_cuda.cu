#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

#include <map>
#include <vector>
#include <algorithm>
#include <cstdio>
#include <fstream>

#include <cuda.h>

#include "macros/cpp_defines.h"

#include "../spmv_kernel.h"

#ifdef __cplusplus
extern "C"{
#endif
	#include "macros/macrolib.h"
	#include "time_it.h"
	#include "parallel_util.h"
	#include "array_metrics.h"

	#include "cuda/cuda_util.h"
#ifdef __cplusplus
}
#endif

#include "spmv_kernels/merge/cub/util_allocator.cuh"
#include "spmv_kernels/merge/cub/device/device_spmv.cuh"
#include "spmv_kernels/merge/cub/iterator/tex_obj_input_iterator.cuh"

#include "spmv_kernels/merge/sparse_matrix.h"
// #include "spmv_kernels/merge/utils.h"

double * thread_time_compute, * thread_time_barrier;

#ifndef TIME_IT
#define TIME_IT 1
#endif

// struct CachingDeviceAllocatorWrapper {
// 	cub::CachingDeviceAllocator  g_allocator;
// 	CachingDeviceAllocatorWrapper() : g_allocator(true) {
// 	// Other constructor code if needed
// 	}
// };

struct MERGEArrays : Matrix_Format
{
	INT_T * ia;      // the usual rowptr (of size m+1)
	INT_T * ja;      // the colidx of each NNZ (of size nnz)
	ValueType * a;   // the values (of size NNZ)

	INT_T * ia_d;
	INT_T * ja_d;
	ValueType * a_d;

	ValueType * x = NULL;
	ValueType * y = NULL;
	ValueType * x_d = NULL;
	ValueType * y_d = NULL;

	// CachingDeviceAllocatorWrapper alloc_wrapper;
	size_t temp_storage_bytes = 0;
	void *d_temp_storage = NULL;

	// cudaEvent_t is useful for timing, but for performance use " cudaEventCreateWithFlags ( &event, cudaEventDisableTiming) "
	cudaEvent_t startEvent_execution;
	cudaEvent_t endEvent_execution;
	
	cudaEvent_t startEvent_memcpy_ia;
	cudaEvent_t endEvent_memcpy_ia;
	cudaEvent_t startEvent_memcpy_ja;
	cudaEvent_t endEvent_memcpy_ja;
	cudaEvent_t startEvent_memcpy_a;
	cudaEvent_t endEvent_memcpy_a;

	cudaEvent_t startEvent_memcpy_x;
	cudaEvent_t endEvent_memcpy_x;
	cudaEvent_t startEvent_memcpy_y;
	cudaEvent_t endEvent_memcpy_y;

	MERGEArrays(INT_T * ia, INT_T * ja, ValueTypeReference * a, long m, long n, long nnz) : Matrix_Format(m, n, nnz), ia(ia), ja(ja), a(a)
	{
		
		// cuda_assert(alloc_wrapper.g_allocator.DeviceAllocate((void**)&ia_d, (m+1) * sizeof(*ia_d)));
		// cuda_assert(alloc_wrapper.g_allocator.DeviceAllocate((void**)&ja_d, nnz * sizeof(*ja_d)));
		// cuda_assert(alloc_wrapper.g_allocator.DeviceAllocate((void**)&a_d, nnz * sizeof(*a_d)));
		// cuda_assert(alloc_wrapper.g_allocator.DeviceAllocate((void**)&x_d, n * sizeof(*x_d)));
		// cuda_assert(alloc_wrapper.g_allocator.DeviceAllocate((void**)&y_d, m * sizeof(*y_d)));
		cuda_assert(cudaMalloc(&ia_d, (m+1) * sizeof(*ia_d)));
		cuda_assert(cudaMalloc(&ja_d, nnz * sizeof(*ja_d)));
		cuda_assert(cudaMalloc(&a_d, nnz * sizeof(*a_d)));
		cuda_assert(cudaMalloc(&x_d, n * sizeof(*x_d)));
		cuda_assert(cudaMalloc(&y_d, m * sizeof(*y_d)));

		// cuda events for timing measurements
		cuda_assert(cudaEventCreate(&startEvent_execution));
		cuda_assert(cudaEventCreate(&endEvent_execution));

		if(TIME_IT){
			cuda_assert(cudaEventCreate(&startEvent_memcpy_ia));
			cuda_assert(cudaEventCreate(&endEvent_memcpy_ia));
			cuda_assert(cudaEventCreate(&startEvent_memcpy_ja));
			cuda_assert(cudaEventCreate(&endEvent_memcpy_ja));
			cuda_assert(cudaEventCreate(&startEvent_memcpy_a));
			cuda_assert(cudaEventCreate(&endEvent_memcpy_a));

			cuda_assert(cudaEventCreate(&startEvent_memcpy_x));
			cuda_assert(cudaEventCreate(&endEvent_memcpy_x));
			cuda_assert(cudaEventCreate(&startEvent_memcpy_y));
			cuda_assert(cudaEventCreate(&endEvent_memcpy_y));
		}

		if(TIME_IT) cuda_assert(cudaEventRecord(startEvent_memcpy_ia));
		cuda_assert(cudaMemcpy(ia_d, ia, (m+1) * sizeof(*ia_d), cudaMemcpyHostToDevice));
		if(TIME_IT) cuda_assert(cudaEventRecord(endEvent_memcpy_ia));

		if(TIME_IT) cuda_assert(cudaEventRecord(startEvent_memcpy_ja));
		cuda_assert(cudaMemcpy(ja_d, ja, nnz * sizeof(*ja_d), cudaMemcpyHostToDevice));
		if(TIME_IT) cuda_assert(cudaEventRecord(endEvent_memcpy_ja));

		if(TIME_IT) cuda_assert(cudaEventRecord(startEvent_memcpy_a));
		cuda_assert(cudaMemcpy(a_d, a, nnz * sizeof(*a_d), cudaMemcpyHostToDevice));
		if(TIME_IT) cuda_assert(cudaEventRecord(endEvent_memcpy_a));

		if(TIME_IT){
			cuda_assert(cudaEventSynchronize(endEvent_memcpy_ia));
			cuda_assert(cudaEventSynchronize(endEvent_memcpy_ja));
			cuda_assert(cudaEventSynchronize(endEvent_memcpy_a));

			float memcpyTime_cuda_ia, memcpyTime_cuda_ja, memcpyTime_cuda_a;
			cuda_assert(cudaEventElapsedTime(&memcpyTime_cuda_ia, startEvent_memcpy_ia, endEvent_memcpy_ia));
			cuda_assert(cudaEventElapsedTime(&memcpyTime_cuda_ja, startEvent_memcpy_ja, endEvent_memcpy_ja));
			cuda_assert(cudaEventElapsedTime(&memcpyTime_cuda_a, startEvent_memcpy_a, endEvent_memcpy_a));

			printf("(CUDA) Memcpy ia time = %.4lf ms, ja time = %.4lf ms, a time = %.4lf ms\n", memcpyTime_cuda_ia, memcpyTime_cuda_ja, memcpyTime_cuda_a);
		}
	}

	~MERGEArrays()
	{
		free(a);
		free(ia);
		free(ja);

		// cuda_assert(alloc_wrapper.g_allocator.DeviceFree(ia_d));
		// cuda_assert(alloc_wrapper.g_allocator.DeviceFree(ja_d));
		// cuda_assert(alloc_wrapper.g_allocator.DeviceFree(a_d));
		// cuda_assert(alloc_wrapper.g_allocator.DeviceFree(x_d));
		// cuda_assert(alloc_wrapper.g_allocator.DeviceFree(y_d));
		// cuda_assert(alloc_wrapper.g_allocator.DeviceFree(d_temp_storage));
		cuda_assert(cudaFree(ia_d));
		cuda_assert(cudaFree(ja_d));
		cuda_assert(cudaFree(a_d));
		cuda_assert(cudaFree(x_d));
		cuda_assert(cudaFree(y_d));
		cuda_assert(cudaFree(d_temp_storage));

		cuda_assert(cudaEventDestroy(startEvent_execution));
		cuda_assert(cudaEventDestroy(endEvent_execution));

		if(TIME_IT){
			cuda_assert(cudaEventDestroy(startEvent_memcpy_x));
			cuda_assert(cudaEventDestroy(endEvent_memcpy_x));
			cuda_assert(cudaEventDestroy(startEvent_memcpy_y));
			cuda_assert(cudaEventDestroy(endEvent_memcpy_y));

			cuda_assert(cudaEventDestroy(startEvent_memcpy_ia));
			cuda_assert(cudaEventDestroy(endEvent_memcpy_ia));
			cuda_assert(cudaEventDestroy(startEvent_memcpy_ja));
			cuda_assert(cudaEventDestroy(endEvent_memcpy_ja));
			cuda_assert(cudaEventDestroy(startEvent_memcpy_a));
			cuda_assert(cudaEventDestroy(endEvent_memcpy_a));
		}

		#ifdef PRINT_STATISTICS
			free(thread_time_barrier);
			free(thread_time_compute);
		#endif
	}

	void spmv(ValueType * x, ValueType * y);
	void statistics_start();
	int statistics_print_data(__attribute__((unused)) char * buf, __attribute__((unused)) long buf_n);
};


void compute_csr(MERGEArrays * restrict csr, ValueType * restrict x , ValueType * restrict y);

void
MERGEArrays::spmv(ValueType * x, ValueType * y)
{
	compute_csr(this, x, y);
}


struct Matrix_Format *
csr_to_format(INT_T * row_ptr, INT_T * col_ind, ValueTypeReference * values, long m, long n, long nnz, long symmetric, long symmetry_expanded)
{
	struct MERGEArrays * csr = new MERGEArrays(row_ptr, col_ind, values, m, n, nnz);
	csr->mem_footprint = nnz * (sizeof(ValueType) + sizeof(INT_T)) + (m+1) * sizeof(INT_T);
	char *format_name;
	format_name = (char *)malloc(100*sizeof(char));
	snprintf(format_name, 100, "MERGE_CUDA");
	csr->format_name = format_name;
	return csr;
}


//==========================================================================================================================================
//= MERGE CUDA
//==========================================================================================================================================

void
compute_csr(MERGEArrays * restrict csr, ValueType * restrict x, ValueType * restrict y)
{

	if (csr->x == NULL)
	{
		csr->x = x;

		if(TIME_IT) cuda_assert(cudaEventRecord(csr->startEvent_memcpy_x));
		cuda_assert(cudaMemcpy(csr->x_d, csr->x, csr->n * sizeof(*csr->x), cudaMemcpyHostToDevice));

		if(TIME_IT) cuda_assert(cudaEventRecord(csr->endEvent_memcpy_x));

		if(TIME_IT){
			cuda_assert(cudaEventSynchronize(csr->endEvent_memcpy_x));
			float memcpyTime_cuda;
			cuda_assert(cudaEventElapsedTime(&memcpyTime_cuda, csr->startEvent_memcpy_x, csr->endEvent_memcpy_x));
			printf("(CUDA) Memcpy x time = %.4lf ms\n", memcpyTime_cuda);
		}

		// Get amount of temporary storage needed
		cub::DeviceSpmv::CsrMV(csr->d_temp_storage, csr->temp_storage_bytes, 
			csr->a_d, csr->ia_d, csr->ja_d, csr->x_d, csr->y_d, csr->m, csr->n, csr->nnz,
			(cudaStream_t) 0, 0);
		cuda_assert(cudaDeviceSynchronize());
		printf("temp_storage_bytes = %ld\n", csr->temp_storage_bytes);
		// cuda_assert(csr->alloc_wrapper.g_allocator.DeviceAllocate(&csr->d_temp_storage, csr->temp_storage_bytes));
		cuda_assert(cudaMalloc(&csr->d_temp_storage, csr->temp_storage_bytes));
		cuda_assert(cudaMemset(csr->y_d, 0, csr->m * sizeof(csr->y_d)));
	}

	cub::DeviceSpmv::CsrMV(csr->d_temp_storage, csr->temp_storage_bytes, 
		csr->a_d, csr->ia_d, csr->ja_d, csr->x_d, csr->y_d, csr->m, csr->n, csr->nnz,
		(cudaStream_t) 0, 0);
	cuda_assert(cudaDeviceSynchronize());
	
	if (csr->y == NULL)
	{
		csr->y = y;

		if(TIME_IT) cuda_assert(cudaEventRecord(csr->startEvent_memcpy_y));
		cuda_assert(cudaMemcpy(csr->y, csr->y_d, csr->m * sizeof(*csr->y), cudaMemcpyDeviceToHost));
		if(TIME_IT) cuda_assert(cudaEventRecord(csr->endEvent_memcpy_y));

		if(TIME_IT){
			cuda_assert(cudaEventSynchronize(csr->endEvent_memcpy_y));
			float memcpyTime_cuda;
			cuda_assert(cudaEventElapsedTime(&memcpyTime_cuda, csr->startEvent_memcpy_y, csr->endEvent_memcpy_y));
			printf("(CUDA) Memcpy y time = %.4lf ms\n", memcpyTime_cuda);
		}
	}
}


//==========================================================================================================================================
//= Print Statistics
//==========================================================================================================================================


void
MERGEArrays::statistics_start()
{
}


int
statistics_print_labels(__attribute__((unused)) char * buf, __attribute__((unused)) long buf_n)
{
	return 0;
}


int
MERGEArrays::statistics_print_data(__attribute__((unused)) char * buf, __attribute__((unused)) long buf_n)
{
	return 0;
}

