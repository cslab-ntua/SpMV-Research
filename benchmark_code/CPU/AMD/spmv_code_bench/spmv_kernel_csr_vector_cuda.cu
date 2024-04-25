#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

#include <cuda.h>

#include "macros/cpp_defines.h"

#include "spmv_bench_common.h"
#include "spmv_kernel.h"

#ifdef __cplusplus
extern "C"{
#endif
	#include "macros/macrolib.h"
	#include "time_it.h"
	#include "parallel_util.h"
	#include "array_metrics.h"

	#include "cuda/cuda_util.h"
	#include "aux/csr_util.h"
	#include "aux/csr_converter.h"
	#include "aux/csc_util.h"
	#include "aux/csc_converter.h"
#ifdef __cplusplus
}
#endif


extern int prefetch_distance;

double * thread_time_compute, * thread_time_barrier;

#ifndef BLOCK_SIZE
#define BLOCK_SIZE 1024
#endif

#ifndef TIME_IT
#define TIME_IT 0
#endif

struct CSRArrays : Matrix_Format
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

	int max_smem_per_block, multiproc_count, max_threads_per_block, warp_size, block_size, max_threads_per_multiproc;
	// int num_streams;

	CSRArrays(INT_T * ia, INT_T * ja, ValueType * a, long m, long n, long nnz) : Matrix_Format(m, n, nnz), ia(ia), ja(ja), a(a)
	{
		gpuCudaErrorCheck(cudaDeviceGetAttribute(&max_smem_per_block, cudaDevAttrMaxSharedMemoryPerBlock, 0));
		gpuCudaErrorCheck(cudaDeviceGetAttribute(&multiproc_count, cudaDevAttrMultiProcessorCount, 0));
		gpuCudaErrorCheck(cudaDeviceGetAttribute(&max_threads_per_block, cudaDevAttrMaxThreadsPerBlock , 0));
		gpuCudaErrorCheck(cudaDeviceGetAttribute(&warp_size, cudaDevAttrWarpSize , 0));
		gpuCudaErrorCheck(cudaDeviceGetAttribute(&max_threads_per_multiproc, cudaDevAttrMaxThreadsPerMultiProcessor, 0));
		printf("max_smem_per_block=%d\n", max_smem_per_block);
		printf("multiproc_count=%d\n", multiproc_count);
		printf("max_threads_per_block=%d\n", max_threads_per_block);
		printf("warp_size=%d\n", warp_size);
		printf("max_threads_per_multiproc=%d\n", max_threads_per_multiproc);

		block_size = BLOCK_SIZE;

		gpuCudaErrorCheck(cudaMalloc(&ia_d, (m+1) * sizeof(*ia_d)));
		gpuCudaErrorCheck(cudaMalloc(&ja_d, nnz * sizeof(*ja_d)));
		gpuCudaErrorCheck(cudaMalloc(&a_d, nnz * sizeof(*a_d)));
		gpuCudaErrorCheck(cudaMalloc(&x_d, n * sizeof(*x_d)));
		gpuCudaErrorCheck(cudaMalloc(&y_d, m * sizeof(*y_d)));

		// cuda events for timing measurements
		gpuCudaErrorCheck(cudaEventCreate(&startEvent_execution));
		gpuCudaErrorCheck(cudaEventCreate(&endEvent_execution));

		if(TIME_IT){
			gpuCudaErrorCheck(cudaEventCreate(&startEvent_memcpy_ia));
			gpuCudaErrorCheck(cudaEventCreate(&endEvent_memcpy_ia));
			gpuCudaErrorCheck(cudaEventCreate(&startEvent_memcpy_ja));
			gpuCudaErrorCheck(cudaEventCreate(&endEvent_memcpy_ja));
			gpuCudaErrorCheck(cudaEventCreate(&startEvent_memcpy_a));
			gpuCudaErrorCheck(cudaEventCreate(&endEvent_memcpy_a));

			gpuCudaErrorCheck(cudaEventCreate(&startEvent_memcpy_x));
			gpuCudaErrorCheck(cudaEventCreate(&endEvent_memcpy_x));
			gpuCudaErrorCheck(cudaEventCreate(&startEvent_memcpy_y));
			gpuCudaErrorCheck(cudaEventCreate(&endEvent_memcpy_y));
		}

		if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(startEvent_memcpy_ia));
		gpuCudaErrorCheck(cudaMemcpy(ia_d, ia, (m+1) * sizeof(*ia_d), cudaMemcpyHostToDevice));
		if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(endEvent_memcpy_ia));

		if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(startEvent_memcpy_ja));
		gpuCudaErrorCheck(cudaMemcpy(ja_d, ja, nnz * sizeof(*ja_d), cudaMemcpyHostToDevice));
		if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(endEvent_memcpy_ja));

		if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(startEvent_memcpy_a));
		gpuCudaErrorCheck(cudaMemcpy(a_d, a, nnz * sizeof(*a_d), cudaMemcpyHostToDevice));
		if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(endEvent_memcpy_a));

		if(TIME_IT){
			gpuCudaErrorCheck(cudaEventSynchronize(endEvent_memcpy_ia));
			gpuCudaErrorCheck(cudaEventSynchronize(endEvent_memcpy_ja));
			gpuCudaErrorCheck(cudaEventSynchronize(endEvent_memcpy_a));

			float memcpyTime_cuda_ia, memcpyTime_cuda_ja, memcpyTime_cuda_a;
			gpuCudaErrorCheck(cudaEventElapsedTime(&memcpyTime_cuda_ia, startEvent_memcpy_ia, endEvent_memcpy_ia));
			gpuCudaErrorCheck(cudaEventElapsedTime(&memcpyTime_cuda_ja, startEvent_memcpy_ja, endEvent_memcpy_ja));
			gpuCudaErrorCheck(cudaEventElapsedTime(&memcpyTime_cuda_a, startEvent_memcpy_a, endEvent_memcpy_a));
			printf("(CUDA) Memcpy ia time = %.4lf ms, ja time = %.4lf ms, a time = %.4lf ms\n", memcpyTime_cuda_ia, memcpyTime_cuda_ja, memcpyTime_cuda_a);
		}
	}

	~CSRArrays()
	{
		free(a);
		free(ia);
		free(ja);

		gpuCudaErrorCheck(cudaFree(ia_d));
		gpuCudaErrorCheck(cudaFree(ja_d));
		gpuCudaErrorCheck(cudaFree(a_d));
		gpuCudaErrorCheck(cudaFree(x_d));
		gpuCudaErrorCheck(cudaFree(y_d));

		gpuCudaErrorCheck(cudaEventDestroy(startEvent_execution));
		gpuCudaErrorCheck(cudaEventDestroy(endEvent_execution));

		if(TIME_IT){
			gpuCudaErrorCheck(cudaEventDestroy(startEvent_memcpy_x));
			gpuCudaErrorCheck(cudaEventDestroy(endEvent_memcpy_x));
			gpuCudaErrorCheck(cudaEventDestroy(startEvent_memcpy_y));
			gpuCudaErrorCheck(cudaEventDestroy(endEvent_memcpy_y));

			gpuCudaErrorCheck(cudaEventDestroy(startEvent_memcpy_ia));
			gpuCudaErrorCheck(cudaEventDestroy(endEvent_memcpy_ia));
			gpuCudaErrorCheck(cudaEventDestroy(startEvent_memcpy_ja));
			gpuCudaErrorCheck(cudaEventDestroy(endEvent_memcpy_ja));
			gpuCudaErrorCheck(cudaEventDestroy(startEvent_memcpy_a));
			gpuCudaErrorCheck(cudaEventDestroy(endEvent_memcpy_a));
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


void compute_csr(CSRArrays * restrict csr, ValueType * restrict x , ValueType * restrict y);

void
CSRArrays::spmv(ValueType * x, ValueType * y)
{
	compute_csr(this, x, y);
}


struct Matrix_Format *
csr_to_format(INT_T * row_ptr, INT_T * col_ind, ValueType * values, long m, long n, long nnz)
{
	struct CSRArrays * csr = new CSRArrays(row_ptr, col_ind, values, m, n, nnz);
	csr->mem_footprint = nnz * (sizeof(ValueType) + sizeof(INT_T)) + (m+1) * sizeof(INT_T);
	char *format_name;
	format_name = (char *)malloc(100*sizeof(char));
	snprintf(format_name, 100, "Custom_CSR_CUDA_VECTOR_b%d", csr->block_size);
	csr->format_name = format_name;
	return csr;
}


//==========================================================================================================================================
//= CSR Custom
//==========================================================================================================================================

__global__ void gpu_kernel_csr_vector(INT_T * ia, INT_T * ja, ValueType * a, INT_T m, int block_size, int warp_size, ValueType * restrict x, ValueType * restrict y)
{
	// Thread ID in block
	INT_T t = threadIdx.x;

	// Thread ID in warp
	INT_T lane = t & (warp_size-1);

	// Number of warps per block
	INT_T warpsPerBlock = blockDim.x / warp_size;

	// One row per warp
	INT_T row = (blockIdx.x * warpsPerBlock) + (t / warp_size);

	__shared__ volatile ValueType LDS[BLOCK_SIZE];

	if (row < m){
		INT_T rowStart = ia[row];
		INT_T rowEnd = ia[row+1];
		ValueType sum = 0;

		// Use all threads in a warp accumulate multiplied elements
		for (INT_T j = rowStart + lane; j < rowEnd; j += warp_size){
			INT_T col = ja[j];
			sum += a[j] * x[col];
		}
		LDS[t] = sum;
		__syncthreads();
	
		// Reduce partial sums
		if (lane < 16) LDS[t] += LDS[t + 16];
		if (lane <  8) LDS[t] += LDS[t + 8];
		if (lane <  4) LDS[t] += LDS[t + 4];
		if (lane <  2) LDS[t] += LDS[t + 2];
		if (lane <  1) LDS[t] += LDS[t + 1];
		__syncthreads();
		
		// Write result
		if (lane == 0){
			y[row] = LDS[t];
		}
	}
}


void
compute_csr(CSRArrays * restrict csr, ValueType * restrict x, ValueType * restrict y)
{
	dim3 block_dims(csr->block_size);
	dim3 grid_dims(ceil(csr->m/((float)csr->block_size/csr->warp_size)));
	// printf("Grid : {%d, %d, %d} blocks. Blocks : {%d, %d, %d} threads.\n", grid_dims.x, grid_dims.y, grid_dims.z, block_dims.x, block_dims.y, block_dims.z);

	if (csr->x == NULL)
	{
		csr->x = x;

		if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(csr->startEvent_memcpy_x));
		gpuCudaErrorCheck(cudaMemcpy(csr->x_d, csr->x, csr->n * sizeof(*csr->x), cudaMemcpyHostToDevice));
		if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(csr->endEvent_memcpy_x));

		if(TIME_IT){
			gpuCudaErrorCheck(cudaEventSynchronize(csr->endEvent_memcpy_x));
			float memcpyTime_cuda;
			gpuCudaErrorCheck(cudaEventElapsedTime(&memcpyTime_cuda, csr->startEvent_memcpy_x, csr->endEvent_memcpy_x));
			printf("(CUDA) Memcpy x time = %.4lf ms\n", memcpyTime_cuda);
		}
	}

	gpu_kernel_csr_vector<<<grid_dims, block_dims>>>(csr->ia_d, csr->ja_d, csr->a_d, csr->m, csr->block_size, csr->warp_size, csr->x_d, csr->y_d);
	gpuCudaErrorCheck(cudaPeekAtLastError());
	gpuCudaErrorCheck(cudaDeviceSynchronize());

	if (csr->y == NULL)
	{
		csr->y = y;

		if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(csr->startEvent_memcpy_y));
		gpuCudaErrorCheck(cudaMemcpy(csr->y, csr->y_d, csr->m * sizeof(*csr->y), cudaMemcpyDeviceToHost));
		if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(csr->endEvent_memcpy_y));

		if(TIME_IT){
			gpuCudaErrorCheck(cudaEventSynchronize(csr->endEvent_memcpy_y));
			float memcpyTime_cuda;
			gpuCudaErrorCheck(cudaEventElapsedTime(&memcpyTime_cuda, csr->startEvent_memcpy_y, csr->endEvent_memcpy_y));
			printf("(CUDA) Memcpy y time = %.4lf ms\n", memcpyTime_cuda);
		}
	}
}


//==========================================================================================================================================
//= Print Statistics
//==========================================================================================================================================


void
CSRArrays::statistics_start()
{
}


int
statistics_print_labels(__attribute__((unused)) char * buf, __attribute__((unused)) long buf_n)
{
	return 0;
}


int
CSRArrays::statistics_print_data(__attribute__((unused)) char * buf, __attribute__((unused)) long buf_n)
{
	return 0;
}

