#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

#include <cuda.h>
#include <cusparse.h>

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
	#include "cuda/cusparse_util.h"
#ifdef __cplusplus
}
#endif

extern int prefetch_distance;

#if DOUBLE == 0
	#define ValueTypeCuda  CUDA_R_32F
#elif DOUBLE == 1
	#define ValueTypeCuda  CUDA_R_64F
#endif

double * thread_time_compute, * thread_time_barrier;

#ifndef TIME_IT
#define TIME_IT 0
#endif

struct CSCArrays : Matrix_Format
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

	cusparseHandle_t     handle = NULL;
	cusparseSpMatDescr_t matA;
	void*                dBuffer    = NULL;
	size_t               bufferSize = 0;

	ValueType * x = NULL;
	ValueType * y = NULL;
	ValueType * x_d = NULL;
	ValueType * y_d = NULL;
	ValueType * x_h = NULL;
	ValueType * y_h = NULL;
	cusparseDnVecDescr_t vecX;
	cusparseDnVecDescr_t vecY;

	cudaStream_t stream;
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
	cudaEvent_t startEvent_spmv_preprocess;
	cudaEvent_t endEvent_spmv_preprocess;

	cudaEvent_t startEvent_create_vecX;
	cudaEvent_t endEvent_create_vecX;
	cudaEvent_t startEvent_create_vecY;
	cudaEvent_t endEvent_create_vecY;

	int max_persistent_l2_cache;

	CSCArrays(INT_T * row_ptr, INT_T * col_ind, ValueType * values, long m, long n, long nnz) : Matrix_Format(m, n, nnz)
	{
		int max_smem_per_block, multiproc_count, max_threads_per_block, warp_size, max_threads_per_multiproc;
		gpuCudaErrorCheck(cudaDeviceGetAttribute(&max_smem_per_block, cudaDevAttrMaxSharedMemoryPerBlock, 0));
		gpuCudaErrorCheck(cudaDeviceGetAttribute(&multiproc_count, cudaDevAttrMultiProcessorCount, 0));
		gpuCudaErrorCheck(cudaDeviceGetAttribute(&max_threads_per_block, cudaDevAttrMaxThreadsPerBlock , 0));
		gpuCudaErrorCheck(cudaDeviceGetAttribute(&warp_size, cudaDevAttrWarpSize , 0));
		gpuCudaErrorCheck(cudaDeviceGetAttribute(&max_threads_per_multiproc, cudaDevAttrMaxThreadsPerMultiProcessor, 0));
		gpuCudaErrorCheck(cudaDeviceGetAttribute(&max_persistent_l2_cache, cudaDevAttrMaxPersistingL2CacheSize, 0));
		// printf("max_smem_per_block=%d\n", max_smem_per_block);
		// printf("multiproc_count=%d\n", multiproc_count);
		// printf("max_threads_per_block=%d\n", max_threads_per_block);
		// printf("warp_size=%d\n", warp_size);
		// printf("max_threads_per_multiproc=%d\n", max_threads_per_multiproc);

		ia = (INT_T *) malloc(nnz * sizeof(INT_T));
		ja = (INT_T *) malloc((n+1) * sizeof(INT_T));
		a = (ValueType *) malloc(nnz * sizeof(ValueType));

		gpuCudaErrorCheck(cudaMalloc(&ia_d, nnz * sizeof(*ia_d)));
		gpuCudaErrorCheck(cudaMalloc(&ja_d, (n+1) * sizeof(*ja_d)));
		gpuCudaErrorCheck(cudaMalloc(&a_d, nnz * sizeof(*a_d)));
		gpuCudaErrorCheck(cudaMalloc(&x_d, n * sizeof(*x_d)));
		gpuCudaErrorCheck(cudaMalloc(&y_d, m * sizeof(*y_d)));

		gpuCudaErrorCheck(cudaStreamCreate(&stream));
		gpuCusparseErrorCheck(cusparseCreate(&handle));
		gpuCusparseErrorCheck(cusparseSetStream(handle, stream));

		size_t csc_bufferSize = 0;
		void* csc_dBuffer    = NULL;
		gpuCusparseErrorCheck(cusparseCsr2cscEx2_bufferSize(handle, m, n, nnz, values, row_ptr, col_ind, a, ja, ia, ValueTypeCuda, CUSPARSE_ACTION_NUMERIC, CUSPARSE_INDEX_BASE_ZERO, CUSPARSE_CSR2CSC_ALG_DEFAULT, &csc_bufferSize));
		gpuCudaErrorCheck(cudaMalloc(&csc_dBuffer, csc_bufferSize));
		gpuCusparseErrorCheck(cusparseCsr2cscEx2(handle, m, n, nnz, values, row_ptr, col_ind, a, ja, ia, ValueTypeCuda, CUSPARSE_ACTION_NUMERIC, CUSPARSE_INDEX_BASE_ZERO, CUSPARSE_CSR2CSC_ALG_DEFAULT, csc_dBuffer));
		gpuCudaErrorCheck(cudaFree(csc_dBuffer));

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
			gpuCudaErrorCheck(cudaEventCreate(&startEvent_spmv_preprocess));
			gpuCudaErrorCheck(cudaEventCreate(&endEvent_spmv_preprocess));

			gpuCudaErrorCheck(cudaEventCreate(&startEvent_create_vecX));
			gpuCudaErrorCheck(cudaEventCreate(&endEvent_create_vecX));
			gpuCudaErrorCheck(cudaEventCreate(&startEvent_create_vecY));
			gpuCudaErrorCheck(cudaEventCreate(&endEvent_create_vecY));
			gpuCudaErrorCheck(cudaEventCreate(&startEvent_memcpy_x));
			gpuCudaErrorCheck(cudaEventCreate(&endEvent_memcpy_x));
			gpuCudaErrorCheck(cudaEventCreate(&startEvent_memcpy_y));
			gpuCudaErrorCheck(cudaEventCreate(&endEvent_memcpy_y));
		}

		gpuCudaErrorCheck(cudaMallocHost(&ia_h, nnz * sizeof(*ia_h)));
		gpuCudaErrorCheck(cudaMallocHost(&ja_h, (n+1) * sizeof(*ja_h)));
		gpuCudaErrorCheck(cudaMallocHost(&a_h, nnz * sizeof(*a_h)));
		gpuCudaErrorCheck(cudaMallocHost(&x_h, n * sizeof(*x_h)));
		gpuCudaErrorCheck(cudaMallocHost(&y_h, m * sizeof(*y_h)));

		memcpy(ia_h, ia, nnz * sizeof(*ia_h));
		memcpy(ja_h, ja, (n+1) * sizeof(*ja_h));
		memcpy(a_h, a, nnz * sizeof(*a_h));

		if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(startEvent_memcpy_ia));
		gpuCudaErrorCheck(cudaMemcpyAsync(ia_d, ia_h, nnz * sizeof(*ia_d), cudaMemcpyHostToDevice, stream));
		if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(endEvent_memcpy_ia));

		if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(startEvent_memcpy_ja));
		gpuCudaErrorCheck(cudaMemcpyAsync(ja_d, ja_h, (n+1) * sizeof(*ja_d), cudaMemcpyHostToDevice, stream));
		if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(endEvent_memcpy_ja));

		if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(startEvent_memcpy_a));
		gpuCudaErrorCheck(cudaMemcpyAsync(a_d, a_h, nnz * sizeof(*a_d), cudaMemcpyHostToDevice, stream));
		if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(endEvent_memcpy_a));

		// Create sparse matrix A in CSC format
		if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(startEvent_create_matA));
		gpuCusparseErrorCheck(cusparseCreateCsc(&matA, m, n, nnz, ja_d, ia_d, a_d, CUSPARSE_INDEX_32I, CUSPARSE_INDEX_32I, CUSPARSE_INDEX_BASE_ZERO, ValueTypeCuda));
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

	~CSCArrays()
	{
		free(a);
		free(ia);
		free(ja);

		// destroy matrix/vector descriptors
		gpuCusparseErrorCheck(cusparseDestroySpMat(matA));
		gpuCusparseErrorCheck(cusparseDestroyDnVec(vecX));
		gpuCusparseErrorCheck(cusparseDestroyDnVec(vecY));
		gpuCusparseErrorCheck(cusparseDestroy(handle));
		gpuCudaErrorCheck(cudaStreamDestroy(stream));

		gpuCudaErrorCheck(cudaFree(ia_d));
		gpuCudaErrorCheck(cudaFree(ja_d));
		gpuCudaErrorCheck(cudaFree(a_d));
		gpuCudaErrorCheck(cudaFree(x_d));
		gpuCudaErrorCheck(cudaFree(y_d));
		gpuCudaErrorCheck(cudaFree(dBuffer));

		gpuCudaErrorCheck(cudaFreeHost(ia_h));
		gpuCudaErrorCheck(cudaFreeHost(ja_h));
		gpuCudaErrorCheck(cudaFreeHost(a_h));
		gpuCudaErrorCheck(cudaFreeHost(x_h));
		gpuCudaErrorCheck(cudaFreeHost(y_h));

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
			gpuCudaErrorCheck(cudaEventDestroy(startEvent_spmv_preprocess));
			gpuCudaErrorCheck(cudaEventDestroy(endEvent_spmv_preprocess));

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


void compute_csc(CSCArrays * restrict csc, ValueType * restrict x , ValueType * restrict y);


void
CSCArrays::spmv(ValueType * x, ValueType * y)
{
	compute_csc(this, x, y);
}


struct Matrix_Format *
csr_to_format(INT_T * row_ptr, INT_T * col_ind, ValueType * values, long m, long n, long nnz)
{
	struct CSCArrays * csc = new CSCArrays(row_ptr, col_ind, values, m, n, nnz);
	csc->mem_footprint = nnz * (sizeof(ValueType) + sizeof(INT_T)) + (n+1) * sizeof(INT_T);
	csc->format_name = (char *) "CUSPARSE_CSC";
	return csc;
}


//==========================================================================================================================================
//= CSC Custom
//==========================================================================================================================================


void
compute_csc(CSCArrays * restrict csc, ValueType * restrict x, ValueType * restrict y)
{
	const double alpha = 1.0;
	const double beta = 0.0;
	if (csc->x == NULL)
	{
		csc->x = x;
		if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(csc->startEvent_memcpy_x, csc->stream));
		memcpy(csc->x_h, x, csc->n * sizeof(ValueType));
		gpuCudaErrorCheck(cudaMemcpyAsync(csc->x_d, csc->x_h, csc->n * sizeof(*csc->x_d), cudaMemcpyHostToDevice, csc->stream));
		if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(csc->endEvent_memcpy_x, csc->stream));
		if(TIME_IT){
			gpuCudaErrorCheck(cudaEventSynchronize(csc->endEvent_memcpy_x));
			float memcpyTime_cuda;
			gpuCudaErrorCheck(cudaEventElapsedTime(&memcpyTime_cuda, csc->startEvent_memcpy_x, csc->endEvent_memcpy_x));
			printf("(CUDA) Memcpy x time = %.4lf ms\n", memcpyTime_cuda);
		}

		// Create dense vector X
		if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(csc->startEvent_create_vecX));
		gpuCusparseErrorCheck(cusparseCreateDnVec(&csc->vecX, csc->n, csc->x_d, ValueTypeCuda));
		if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(csc->endEvent_create_vecX));

		// Create dense vector y
		if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(csc->startEvent_create_vecY));
		gpuCusparseErrorCheck(cusparseCreateDnVec(&csc->vecY, csc->m, csc->y_d, ValueTypeCuda));
		if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(csc->endEvent_create_vecY));

		// Allocate an external buffer if needed
		if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(csc->startEvent_spmv_buffersize));
		gpuCusparseErrorCheck(cusparseSpMV_bufferSize(csc->handle, CUSPARSE_OPERATION_NON_TRANSPOSE, &alpha, csc->matA, csc->vecX, &beta, csc->vecY, ValueTypeCuda, CUSPARSE_SPMV_ALG_DEFAULT, &csc->bufferSize));
		gpuCudaErrorCheck(cudaMalloc(&csc->dBuffer, csc->bufferSize));
		if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(csc->endEvent_spmv_buffersize));
		// printf("SpMV_bufferSize = %zu bytes\n", csc->bufferSize, csc->bufferSize); // size of the workspace that is needed by cusparseSpMV()

		if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(csc->startEvent_spmv_preprocess));
		gpuCusparseErrorCheck(cusparseSpMV_preprocess(csc->handle, CUSPARSE_OPERATION_NON_TRANSPOSE, &alpha, csc->matA, csc->vecX, &beta, csc->vecY, ValueTypeCuda, CUSPARSE_SPMV_ALG_DEFAULT, csc->dBuffer));
		if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(csc->endEvent_spmv_preprocess));

		if(TIME_IT){
			gpuCudaErrorCheck(cudaEventSynchronize(csc->endEvent_create_vecX));
			gpuCudaErrorCheck(cudaEventSynchronize(csc->endEvent_create_vecY));
			gpuCudaErrorCheck(cudaEventSynchronize(csc->endEvent_spmv_buffersize));
			gpuCudaErrorCheck(cudaEventSynchronize(csc->endEvent_spmv_preprocess));
			float create_vecX_time, create_vecY_time, spmv_buffersize_time, spmv_preprocess_time;
			gpuCudaErrorCheck(cudaEventElapsedTime(&create_vecX_time, csc->startEvent_create_vecX, csc->endEvent_create_vecX));
			gpuCudaErrorCheck(cudaEventElapsedTime(&create_vecY_time, csc->startEvent_create_vecY, csc->endEvent_create_vecY));
			gpuCudaErrorCheck(cudaEventElapsedTime(&spmv_buffersize_time, csc->startEvent_spmv_buffersize, csc->endEvent_spmv_buffersize));
			gpuCudaErrorCheck(cudaEventElapsedTime(&spmv_preprocess_time, csc->startEvent_spmv_preprocess, csc->endEvent_spmv_preprocess));
			printf("(CUDA) Create vecX time = %.4lf ms, vecY time = %.4lf ms, spmv_buffersize time = %.4lf (SpMV_bufferSize = %zu), spmv_preprocess time = %.4lf\n", create_vecX_time, create_vecY_time, spmv_buffersize_time, csc->bufferSize, spmv_preprocess_time);
		}

		#ifdef PERSISTENT_L2_PREFETCH
			int x_d_size = csc->n * sizeof(*csc->x);
			gpuCudaErrorCheck(cudaCtxResetPersistingL2Cache()); // This needs to happen every time before running kernel for 1st time for a matrix...
			if(x_d_size < csc->max_persistent_l2_cache){
				cudaStreamAttrValue attribute;
				auto &window = attribute.accessPolicyWindow;
				window.base_ptr = csc->x_d;
				window.num_bytes = x_d_size;
				window.hitRatio = 1.0;
				window.hitProp = cudaAccessPropertyPersisting;
				window.missProp = cudaAccessPropertyStreaming;
				gpuCudaErrorCheck(cudaStreamSetAttribute(csc->stream, cudaStreamAttributeAccessPolicyWindow, &attribute));
			}
		#endif
	}

	gpuCusparseErrorCheck(cusparseSpMV(csc->handle, CUSPARSE_OPERATION_NON_TRANSPOSE, &alpha, csc->matA, csc->vecX, &beta, csc->vecY, ValueTypeCuda, CUSPARSE_SPMV_ALG_DEFAULT, csc->dBuffer));
	gpuCudaErrorCheck(cudaPeekAtLastError());
	gpuCudaErrorCheck(cudaDeviceSynchronize());

	if (csc->y == NULL)
	{
		csc->y = y;
		if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(csc->startEvent_memcpy_y, csc->stream));
		gpuCudaErrorCheck(cudaMemcpyAsync(csc->y_h, csc->y_d, csc->m * sizeof(*csc->y_d), cudaMemcpyDeviceToHost, csc->stream));
		gpuCudaErrorCheck(cudaStreamSynchronize(csc->stream));
		memcpy(y, csc->y_h, csc->m * sizeof(ValueType));
		if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(csc->endEvent_memcpy_y, csc->stream));

		if(TIME_IT){
			gpuCudaErrorCheck(cudaEventSynchronize(csc->endEvent_memcpy_y));
			float memcpyTime_cuda;
			gpuCudaErrorCheck(cudaEventElapsedTime(&memcpyTime_cuda, csc->startEvent_memcpy_y, csc->endEvent_memcpy_y));
			printf("(CUDA) Memcpy y time = %.4lf ms\n", memcpyTime_cuda);
		}
	}
}


//==========================================================================================================================================
//= Print Statistics
//==========================================================================================================================================


void
CSCArrays::statistics_start()
{
}


int
statistics_print_labels(__attribute__((unused)) char * buf, __attribute__((unused)) long buf_n)
{
	return 0;
}


int
CSCArrays::statistics_print_data(__attribute__((unused)) char * buf, __attribute__((unused)) long buf_n)
{
	return 0;
}

