#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

#include <cuda.h>
#include <cusparse.h>

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
	#include "cuda/cusparse_util.h"
#ifdef __cplusplus
}
#endif


#if DOUBLE == 0
	#define ValueTypeCuda  CUDA_R_32F
#elif DOUBLE == 1
	#define ValueTypeCuda  CUDA_R_64F
#endif

double * thread_time_compute, * thread_time_barrier;

#ifndef TIME_IT
#define TIME_IT 1
#endif

#ifndef VERIFIED
#define VERIFIED 1
#endif

struct CSRArrays : Matrix_Format
{
	INT_T * ia;      // the usual rowptr (of size m+1)
	INT_T * ja;      // the colidx of each NNZ (of size nnz)
	ValueType * a;   // the values (of size NNZ)

	INT_T * ia_d;
	INT_T * ja_d;
	ValueType * a_d;

	cusparseHandle_t     handle = NULL;
	cusparseSpMatDescr_t matA;
	void*                dBuffer    = NULL;
	size_t               bufferSize = 0;

	ValueType * x = NULL;
	ValueType * y = NULL;
	ValueType * x_d = NULL;
	ValueType * y_d = NULL;
	cusparseDnVecDescr_t vecX;
	cusparseDnVecDescr_t vecY;

	// cudaEvent_t is useful for timing, but for performance use " cudaEventCreateWithFlags ( &event, cudaEventDisableTiming) "
	cudaEvent_t startEvent_execution;
	cudaEvent_t endEvent_execution;

	cudaEvent_t startEvent_memcpy_x;
	cudaEvent_t endEvent_memcpy_x;
	cudaEvent_t startEvent_memcpy_y;
	cudaEvent_t endEvent_memcpy_y;

	cudaEvent_t startEvent_memcpy_ia;
	cudaEvent_t endEvent_memcpy_ia;
	cudaEvent_t startEvent_memcpy_ja;
	cudaEvent_t endEvent_memcpy_ja;
	cudaEvent_t startEvent_memcpy_a;
	cudaEvent_t endEvent_memcpy_a;

	cudaEvent_t startEvent_create_matA;
	cudaEvent_t endEvent_create_matA;
	cudaEvent_t startEvent_spmv_buffersize;
	cudaEvent_t endEvent_spmv_buffersize;

	cudaEvent_t startEvent_create_vecX;
	cudaEvent_t endEvent_create_vecX;
	cudaEvent_t startEvent_create_vecY;
	cudaEvent_t endEvent_create_vecY;


	CSRArrays(INT_T * ia, INT_T * ja, ValueType * a, long m, long n, long nnz) : Matrix_Format(m, n, nnz), ia(ia), ja(ja), a(a)
	{
		int max_smem_per_block, multiproc_count, max_threads_per_block, warp_size, max_threads_per_multiproc;
		gpuCudaErrorCheck(cudaDeviceGetAttribute(&max_smem_per_block, cudaDevAttrMaxSharedMemoryPerBlock, 0));
		gpuCudaErrorCheck(cudaDeviceGetAttribute(&multiproc_count, cudaDevAttrMultiProcessorCount, 0));
		gpuCudaErrorCheck(cudaDeviceGetAttribute(&max_threads_per_block, cudaDevAttrMaxThreadsPerBlock , 0));
		gpuCudaErrorCheck(cudaDeviceGetAttribute(&warp_size, cudaDevAttrWarpSize , 0));
		gpuCudaErrorCheck(cudaDeviceGetAttribute(&max_threads_per_multiproc, cudaDevAttrMaxThreadsPerMultiProcessor, 0));
		// printf("max_smem_per_block=%d\n", max_smem_per_block);
		// printf("multiproc_count=%d\n", multiproc_count);
		// printf("max_threads_per_block=%d\n", max_threads_per_block);
		// printf("warp_size=%d\n", warp_size);
		// printf("max_threads_per_multiproc=%d\n", max_threads_per_multiproc);

		gpuCudaErrorCheck(cudaMalloc(&ia_d, (m+1) * sizeof(*ia_d)));
		gpuCudaErrorCheck(cudaMalloc(&ja_d, nnz * sizeof(*ja_d)));
		gpuCudaErrorCheck(cudaMalloc(&a_d, nnz * sizeof(*a_d)));
		gpuCudaErrorCheck(cudaMalloc(&x_d, n * sizeof(*x_d)));
		gpuCudaErrorCheck(cudaMalloc(&y_d, m * sizeof(*y_d)));

		gpuCusparseErrorCheck(cusparseCreate(&handle));

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
			gpuCudaErrorCheck(cudaEventCreate(&startEvent_create_matA));
			gpuCudaErrorCheck(cudaEventCreate(&endEvent_create_matA));
			gpuCudaErrorCheck(cudaEventCreate(&startEvent_spmv_buffersize));
			gpuCudaErrorCheck(cudaEventCreate(&endEvent_spmv_buffersize));

			gpuCudaErrorCheck(cudaEventCreate(&startEvent_create_vecX));
			gpuCudaErrorCheck(cudaEventCreate(&endEvent_create_vecX));
			gpuCudaErrorCheck(cudaEventCreate(&startEvent_create_vecY));
			gpuCudaErrorCheck(cudaEventCreate(&endEvent_create_vecY));
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

		// Create sparse matrix A in CSR format
		if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(startEvent_create_matA));
		gpuCusparseErrorCheck(cusparseCreateCsr(&matA, m, n, nnz, ia_d, ja_d, a_d, CUSPARSE_INDEX_32I, CUSPARSE_INDEX_32I, CUSPARSE_INDEX_BASE_ZERO, ValueTypeCuda));
		if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(endEvent_create_matA));

		if(TIME_IT){
			gpuCudaErrorCheck(cudaEventSynchronize(endEvent_memcpy_ia));
			gpuCudaErrorCheck(cudaEventSynchronize(endEvent_memcpy_ja));
			gpuCudaErrorCheck(cudaEventSynchronize(endEvent_memcpy_a));
			gpuCudaErrorCheck(cudaEventSynchronize(endEvent_create_matA));

			float memcpyTime_cuda_ia, memcpyTime_cuda_ja, memcpyTime_cuda_a, create_matA_Time;//memcpyTime_cuda_thread_i_e;
			gpuCudaErrorCheck(cudaEventElapsedTime(&memcpyTime_cuda_ia, startEvent_memcpy_ia, endEvent_memcpy_ia));
			gpuCudaErrorCheck(cudaEventElapsedTime(&memcpyTime_cuda_ja, startEvent_memcpy_ja, endEvent_memcpy_ja));
			gpuCudaErrorCheck(cudaEventElapsedTime(&memcpyTime_cuda_a, startEvent_memcpy_a, endEvent_memcpy_a));
			gpuCudaErrorCheck(cudaEventElapsedTime(&create_matA_Time, startEvent_create_matA, endEvent_create_matA));

			printf("(CUDA) Memcpy ia time = %.4lf ms, ja time = %.4lf ms, a time = %.4lf ms, matA time = %.4lf ms\n", memcpyTime_cuda_ia, memcpyTime_cuda_ja, memcpyTime_cuda_a, create_matA_Time);
		}
	}

	~CSRArrays()
	{
		free(a);
		free(ia);
		free(ja);

		// destroy matrix/vector descriptors
		gpuCusparseErrorCheck(cusparseDestroySpMat(matA));
		gpuCusparseErrorCheck(cusparseDestroyDnVec(vecX));
		gpuCusparseErrorCheck(cusparseDestroyDnVec(vecY));
		gpuCusparseErrorCheck(cusparseDestroy(handle));

		gpuCudaErrorCheck(cudaFree(ia_d));
		gpuCudaErrorCheck(cudaFree(ja_d));
		gpuCudaErrorCheck(cudaFree(a_d));

		gpuCudaErrorCheck(cudaFree(x_d));
		gpuCudaErrorCheck(cudaFree(y_d));
		gpuCudaErrorCheck(cudaFree(dBuffer));

		gpuCudaErrorCheck(cudaEventDestroy(startEvent_execution));
		gpuCudaErrorCheck(cudaEventDestroy(endEvent_execution));

		if(TIME_IT){
			gpuCudaErrorCheck(cudaEventDestroy(startEvent_memcpy_ia));
			gpuCudaErrorCheck(cudaEventDestroy(endEvent_memcpy_ia));
			gpuCudaErrorCheck(cudaEventDestroy(startEvent_memcpy_ja));
			gpuCudaErrorCheck(cudaEventDestroy(endEvent_memcpy_ja));
			gpuCudaErrorCheck(cudaEventDestroy(startEvent_memcpy_a));
			gpuCudaErrorCheck(cudaEventDestroy(endEvent_memcpy_a));

			gpuCudaErrorCheck(cudaEventDestroy(startEvent_create_matA));
			gpuCudaErrorCheck(cudaEventDestroy(endEvent_create_matA));
			gpuCudaErrorCheck(cudaEventDestroy(startEvent_spmv_buffersize));
			gpuCudaErrorCheck(cudaEventDestroy(endEvent_spmv_buffersize));

			gpuCudaErrorCheck(cudaEventDestroy(startEvent_create_vecX));
			gpuCudaErrorCheck(cudaEventDestroy(endEvent_create_vecX));
			gpuCudaErrorCheck(cudaEventDestroy(startEvent_create_vecY));
			gpuCudaErrorCheck(cudaEventDestroy(endEvent_create_vecY));
			gpuCudaErrorCheck(cudaEventDestroy(startEvent_memcpy_x));
			gpuCudaErrorCheck(cudaEventDestroy(endEvent_memcpy_x));
			gpuCudaErrorCheck(cudaEventDestroy(startEvent_memcpy_y));
			gpuCudaErrorCheck(cudaEventDestroy(endEvent_memcpy_y));
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
	csr->format_name = (char *) "CUSPARSE_CSR";
	return csr;
}


//==========================================================================================================================================
//= CSR Custom
//==========================================================================================================================================


void
compute_csr(CSRArrays * restrict csr, ValueType * restrict x, ValueType * restrict y)
{
	const double alpha = 1.0;
	const double beta = 0.0;
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

		// Create dense vector X
		if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(csr->startEvent_create_vecX));
		gpuCusparseErrorCheck(cusparseCreateDnVec(&csr->vecX, csr->n, csr->x_d, ValueTypeCuda));
		if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(csr->endEvent_create_vecX));

		// Create dense vector y
		if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(csr->startEvent_create_vecY));
		gpuCusparseErrorCheck(cusparseCreateDnVec(&csr->vecY, csr->m, csr->y_d, ValueTypeCuda));
		if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(csr->endEvent_create_vecY));

		// Allocate an external buffer if needed
		if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(csr->startEvent_spmv_buffersize));
		gpuCusparseErrorCheck(cusparseSpMV_bufferSize(csr->handle, CUSPARSE_OPERATION_NON_TRANSPOSE, &alpha, csr->matA, csr->vecX, &beta, csr->vecY, ValueTypeCuda, CUSPARSE_SPMV_ALG_DEFAULT, &csr->bufferSize));
		gpuCudaErrorCheck(cudaMalloc(&csr->dBuffer, csr->bufferSize));
		if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(csr->endEvent_spmv_buffersize));
		// printf("SpMV_bufferSize = %zu bytes\n", csr->bufferSize, csr->bufferSize); // size of the workspace that is needed by cusparseSpMV()

		if(TIME_IT){
			gpuCudaErrorCheck(cudaEventSynchronize(csr->endEvent_create_vecX));
			gpuCudaErrorCheck(cudaEventSynchronize(csr->endEvent_create_vecY));
			gpuCudaErrorCheck(cudaEventSynchronize(csr->endEvent_spmv_buffersize));
			float create_vecX_time, create_vecY_time, spmv_buffersize_time;
			gpuCudaErrorCheck(cudaEventElapsedTime(&create_vecX_time, csr->startEvent_create_vecX, csr->endEvent_create_vecX));
			gpuCudaErrorCheck(cudaEventElapsedTime(&create_vecY_time, csr->startEvent_create_vecY, csr->endEvent_create_vecY));
			gpuCudaErrorCheck(cudaEventElapsedTime(&spmv_buffersize_time, csr->startEvent_spmv_buffersize, csr->endEvent_spmv_buffersize));
			printf("(CUDA) Create vecX time = %.4lf ms, vecY time = %.4lf ms, spmv_buffersize time = %.4lf (SpMV_bufferSize = %zu)\n", create_vecX_time, create_vecY_time, spmv_buffersize_time, csr->bufferSize);
		}
	}

	if(VERIFIED){
		int num_loops = 1000;
		for(int k=0;k<num_loops;k++)
			gpuCusparseErrorCheck(cusparseSpMV(csr->handle, CUSPARSE_OPERATION_NON_TRANSPOSE, &alpha, csr->matA, csr->vecX, &beta, csr->vecY, ValueTypeCuda, CUSPARSE_SPMV_ALG_DEFAULT, csr->dBuffer));
		gpuCudaErrorCheck(cudaPeekAtLastError());
		gpuCudaErrorCheck(cudaDeviceSynchronize());
	}

	// Execute SpMV
	gpuCudaErrorCheck(cudaEventRecord(csr->startEvent_execution));

	int num_loops = 128;
	double time_execution = time_it(1,
		for(int k=0;k<num_loops;k++){
			gpuCusparseErrorCheck(cusparseSpMV(csr->handle, CUSPARSE_OPERATION_NON_TRANSPOSE, &alpha, csr->matA, csr->vecX, &beta, csr->vecY, ValueTypeCuda, CUSPARSE_SPMV_ALG_DEFAULT, csr->dBuffer));
			gpuCudaErrorCheck(cudaPeekAtLastError());
			gpuCudaErrorCheck(cudaDeviceSynchronize());
		}
	);

	double gflops = csr->nnz / time_execution * num_loops * 2 * 1e-9;
	printf("(DGAL timing) Execution time = %.4lf ms (%.4lf GFLOPS for %.2lf MB workload)\n", time_execution*1e3, gflops, csr->mem_footprint/(1024*1024.0));

	gpuCudaErrorCheck(cudaEventRecord(csr->endEvent_execution));
	float executionTime_cuda;
	gpuCudaErrorCheck(cudaEventSynchronize(csr->endEvent_execution));
	gpuCudaErrorCheck(cudaEventElapsedTime(&executionTime_cuda, csr->startEvent_execution, csr->endEvent_execution));

	double gflops_cuda = csr->nnz / executionTime_cuda * num_loops * 2 * 1e-6;
	printf("(CUDA) Execution time = %.4lf ms (%.4lf GFLOPS for %.2lf MB workload)\n", executionTime_cuda, gflops_cuda, csr->mem_footprint/(1024*1024.0));

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

