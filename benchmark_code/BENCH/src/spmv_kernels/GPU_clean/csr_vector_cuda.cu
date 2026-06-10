#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

#include <cuda.h>

#include "macros/cpp_defines.h"

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

	INT_T * ia_h;
	INT_T * ja_h;
	ValueType * a_h;

	ValueType * x = NULL;
	ValueType * y = NULL;
	ValueType * x_d = NULL;
	ValueType * y_d = NULL;
	ValueType * x_h = NULL;
	ValueType * y_h = NULL;

	cudaStream_t stream;
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


	CSRArrays(INT_T * ia, INT_T * ja, ValueTypeReference * a, long m, long n, long nnz) : Matrix_Format(m, n, nnz), ia(ia), ja(ja), a(a)
	{

		cuda_device_print_attributes();

		block_size = BLOCK_SIZE;

		cuda_assert(cudaMalloc(&ia_d, (m+1) * sizeof(*ia_d)));
		cuda_assert(cudaMalloc(&ja_d, nnz * sizeof(*ja_d)));
		cuda_assert(cudaMalloc(&a_d, nnz * sizeof(*a_d)));
		cuda_assert(cudaMalloc(&x_d, n * sizeof(*x_d)));
		cuda_assert(cudaMalloc(&y_d, m * sizeof(*y_d)));

		cuda_assert(cudaStreamCreate(&stream));

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

		cuda_assert(cudaMallocHost(&ia_h, (m+1) * sizeof(*ia_h)));
		cuda_assert(cudaMallocHost(&ja_h, nnz * sizeof(*ja_h)));
		cuda_assert(cudaMallocHost(&a_h, nnz * sizeof(*a_h)));
		cuda_assert(cudaMallocHost(&x_h, n * sizeof(*x_h)));
		cuda_assert(cudaMallocHost(&y_h, m * sizeof(*y_h)));

		memcpy(ia_h, ia, (m+1) * sizeof(*ia_h));
		memcpy(ja_h, ja, nnz * sizeof(*ja_h));
		memcpy(a_h, a, nnz * sizeof(*a_h));

		if(TIME_IT) cuda_assert(cudaEventRecord(startEvent_memcpy_ia));
		cuda_assert(cudaMemcpyAsync(ia_d, ia_h, (m+1) * sizeof(*ia_d), cudaMemcpyHostToDevice, stream));
		if(TIME_IT) cuda_assert(cudaEventRecord(endEvent_memcpy_ia));

		if(TIME_IT) cuda_assert(cudaEventRecord(startEvent_memcpy_ja));
		cuda_assert(cudaMemcpyAsync(ja_d, ja_h, nnz * sizeof(*ja_d), cudaMemcpyHostToDevice, stream));
		if(TIME_IT) cuda_assert(cudaEventRecord(endEvent_memcpy_ja));

		if(TIME_IT) cuda_assert(cudaEventRecord(startEvent_memcpy_a));
		cuda_assert(cudaMemcpyAsync(a_d, a_h, nnz * sizeof(*a_d), cudaMemcpyHostToDevice, stream));
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

	~CSRArrays()
	{
		free(a);
		free(ia);
		free(ja);

		cuda_assert(cudaFree(ia_d));
		cuda_assert(cudaFree(ja_d));
		cuda_assert(cudaFree(a_d));
		cuda_assert(cudaFree(x_d));
		cuda_assert(cudaFree(y_d));

		cuda_assert(cudaFreeHost(ia_h));
		cuda_assert(cudaFreeHost(ja_h));
		cuda_assert(cudaFreeHost(a_h));
		cuda_assert(cudaFreeHost(x_h));
		cuda_assert(cudaFreeHost(y_h));

		cuda_assert(cudaStreamDestroy(stream));

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


void compute_csr(CSRArrays * restrict csr, ValueType * restrict x , ValueType * restrict y);

void
CSRArrays::spmv(ValueType * x, ValueType * y)
{
	compute_csr(this, x, y);
}


struct Matrix_Format *
csr_to_format(INT_T * row_ptr, INT_T * col_ind, ValueTypeReference * values, long m, long n, long nnz, int symmetric)
{
	if (symmetric)
		error("symmetric matrices not supported by this format, expand symmetry");
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
		if(TIME_IT) cuda_assert(cudaEventRecord(csr->startEvent_memcpy_x, csr->stream));
		memcpy(csr->x_h, x, csr->n * sizeof(ValueType));
		cuda_assert(cudaMemcpyAsync(csr->x_d, csr->x_h, csr->n * sizeof(*csr->x_d), cudaMemcpyHostToDevice, csr->stream));
		if(TIME_IT) cuda_assert(cudaEventRecord(csr->endEvent_memcpy_x, csr->stream));
		if(TIME_IT){
			cuda_assert(cudaEventSynchronize(csr->endEvent_memcpy_x));
			float memcpyTime_cuda;
			cuda_assert(cudaEventElapsedTime(&memcpyTime_cuda, csr->startEvent_memcpy_x, csr->endEvent_memcpy_x));
			printf("(CUDA) Memcpy x time = %.4lf ms\n", memcpyTime_cuda);
		}

		#ifdef PERSISTENT_L2_PREFETCH
			int x_d_size = csr->n * sizeof(*csr->x);
			cuda_assert(cudaCtxResetPersistingL2Cache()); // This needs to happen every time before running kernel for 1st time for a matrix...
			if(x_d_size < csr->max_persistent_l2_cache){
				cudaStreamAttrValue attribute;
				auto &window = attribute.accessPolicyWindow;
				window.base_ptr = csr->x_d;
				window.num_bytes = x_d_size;
				window.hitRatio = 1.0;
				window.hitProp = cudaAccessPropertyPersisting;
				window.missProp = cudaAccessPropertyStreaming;
				cuda_assert(cudaStreamSetAttribute(csr->stream, cudaStreamAttributeAccessPolicyWindow, &attribute));
			}
		#endif
	}

	gpu_kernel_csr_vector<<<grid_dims, block_dims, 0, csr->stream>>>(csr->ia_d, csr->ja_d, csr->a_d, csr->m, csr->block_size, csr->warp_size, csr->x_d, csr->y_d);
	cuda_assert(cudaPeekAtLastError());
	cuda_assert(cudaDeviceSynchronize());

	if (csr->y == NULL)
	{
		csr->y = y;
		if(TIME_IT) cuda_assert(cudaEventRecord(csr->startEvent_memcpy_y, csr->stream));
		cuda_assert(cudaMemcpyAsync(csr->y_h, csr->y_d, csr->m * sizeof(*csr->y_d), cudaMemcpyDeviceToHost, csr->stream));
		cuda_assert(cudaStreamSynchronize(csr->stream));
		memcpy(y, csr->y_h, csr->m * sizeof(ValueType));
		if(TIME_IT) cuda_assert(cudaEventRecord(csr->endEvent_memcpy_y, csr->stream));

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

