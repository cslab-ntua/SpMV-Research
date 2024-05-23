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

struct COOArrays : Matrix_Format
{
	INT_T * rowind;      // the usual rowptr (of size m+1)
	INT_T * colind;      // the colidx of each NNZ (of size nnz)
	ValueType * a;   // the values (of size NNZ)

	INT_T * rowind_d;
	INT_T * colind_d;
	ValueType * a_d;

	INT_T * rowind_h;
	INT_T * colind_h;
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

	cudaEvent_t startEvent_memcpy_rowind;
	cudaEvent_t endEvent_memcpy_rowind;
	cudaEvent_t startEvent_memcpy_colind;
	cudaEvent_t endEvent_memcpy_colind;
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

	int max_persistent_l2_cache;

	COOArrays(INT_T * ia, INT_T * ja, ValueType * a, long m, long n, long nnz) : Matrix_Format(m, n, nnz), colind(ja), a(a)
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

		gpuCudaErrorCheck(cudaMalloc(&rowind_d, nnz * sizeof(*rowind_d)));
		gpuCudaErrorCheck(cudaMalloc(&colind_d, nnz * sizeof(*colind_d)));
		gpuCudaErrorCheck(cudaMalloc(&a_d, nnz * sizeof(*a_d)));
		gpuCudaErrorCheck(cudaMalloc(&x_d, n * sizeof(*x_d)));
		gpuCudaErrorCheck(cudaMalloc(&y_d, m * sizeof(*y_d)));

		gpuCudaErrorCheck(cudaStreamCreate(&stream));
		gpuCusparseErrorCheck(cusparseCreate(&handle));
		gpuCusparseErrorCheck(cusparseSetStream(handle, stream));

		// cuda events for timing measurements
		gpuCudaErrorCheck(cudaEventCreate(&startEvent_execution));
		gpuCudaErrorCheck(cudaEventCreate(&endEvent_execution));

		if(TIME_IT){
			gpuCudaErrorCheck(cudaEventCreate(&startEvent_memcpy_rowind));
			gpuCudaErrorCheck(cudaEventCreate(&endEvent_memcpy_rowind));
			gpuCudaErrorCheck(cudaEventCreate(&startEvent_memcpy_colind));
			gpuCudaErrorCheck(cudaEventCreate(&endEvent_memcpy_colind));
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

		rowind = (typeof(rowind)) malloc(nnz * sizeof(*rowind));
		#pragma omp parallel
		{
			long i, j, j_s, j_e;
			#pragma omp for
			for (i=0;i<nnz;i++)
			{
				rowind[i] = 0;
			}
			#pragma omp for
			for (i=0;i<m;i++)
			{
				j_s = ia[i];
				j_e = ia[i+1];
				for (j=j_s;j<j_e;j++)
					rowind[j] = i;
			}
		}

		gpuCudaErrorCheck(cudaMallocHost(&rowind_h, nnz * sizeof(*rowind_h)));
		gpuCudaErrorCheck(cudaMallocHost(&colind_h, nnz * sizeof(*colind_h)));
		gpuCudaErrorCheck(cudaMallocHost(&a_h, nnz * sizeof(*a_h)));
		gpuCudaErrorCheck(cudaMallocHost(&x_h, n * sizeof(*x_h)));
		gpuCudaErrorCheck(cudaMallocHost(&y_h, m * sizeof(*y_h)));

		memcpy(rowind_h, rowind, nnz * sizeof(*rowind_h));
		memcpy(colind_h, colind, nnz * sizeof(*colind_h));
		memcpy(a_h, a, nnz * sizeof(*a_h));

		if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(startEvent_memcpy_rowind));
		gpuCudaErrorCheck(cudaMemcpyAsync(rowind_d, rowind_h, nnz * sizeof(*rowind_d), cudaMemcpyHostToDevice, stream));
		if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(endEvent_memcpy_rowind));

		if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(startEvent_memcpy_colind));
		gpuCudaErrorCheck(cudaMemcpyAsync(colind_d, colind_h, nnz * sizeof(*colind_d), cudaMemcpyHostToDevice, stream));
		if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(endEvent_memcpy_colind));

		if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(startEvent_memcpy_a));
		gpuCudaErrorCheck(cudaMemcpyAsync(a_d, a_h, nnz * sizeof(*a_d), cudaMemcpyHostToDevice, stream));
		if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(endEvent_memcpy_a));

		// Create sparse matrix A in COO format
		if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(startEvent_create_matA));
		gpuCusparseErrorCheck(cusparseCreateCoo(&matA, m, n, nnz, rowind_d, colind_d, a_d, CUSPARSE_INDEX_32I, CUSPARSE_INDEX_BASE_ZERO, ValueTypeCuda));
		if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(endEvent_create_matA));

		if(TIME_IT){
			gpuCudaErrorCheck(cudaEventSynchronize(endEvent_memcpy_rowind));
			gpuCudaErrorCheck(cudaEventSynchronize(endEvent_memcpy_colind));
			gpuCudaErrorCheck(cudaEventSynchronize(endEvent_memcpy_a));
			gpuCudaErrorCheck(cudaEventSynchronize(endEvent_create_matA));

			float memcpyTime_cuda_rowind, memcpyTime_cuda_colind, memcpyTime_cuda_a, create_matA_Time;//memcpyTime_cuda_thread_i_e;
			gpuCudaErrorCheck(cudaEventElapsedTime(&memcpyTime_cuda_rowind, startEvent_memcpy_rowind, endEvent_memcpy_rowind));
			gpuCudaErrorCheck(cudaEventElapsedTime(&memcpyTime_cuda_colind, startEvent_memcpy_colind, endEvent_memcpy_colind));
			gpuCudaErrorCheck(cudaEventElapsedTime(&memcpyTime_cuda_a, startEvent_memcpy_a, endEvent_memcpy_a));
			gpuCudaErrorCheck(cudaEventElapsedTime(&create_matA_Time, startEvent_create_matA, endEvent_create_matA));
			printf("(CUDA) Memcpy rowind time = %.4lf ms, colind time = %.4lf ms, a time = %.4lf ms, matA time = %.4lf ms\n", memcpyTime_cuda_rowind, memcpyTime_cuda_colind, memcpyTime_cuda_a, create_matA_Time);
		}
	}

	~COOArrays()
	{
		free(a);
		free(rowind);
		free(colind);

		// destroy matrix/vector descriptors
		gpuCusparseErrorCheck(cusparseDestroySpMat(matA));
		gpuCusparseErrorCheck(cusparseDestroyDnVec(vecX));
		gpuCusparseErrorCheck(cusparseDestroyDnVec(vecY));
		gpuCusparseErrorCheck(cusparseDestroy(handle));
		gpuCudaErrorCheck(cudaStreamDestroy(stream));

		gpuCudaErrorCheck(cudaFree(rowind_d));
		gpuCudaErrorCheck(cudaFree(colind_d));
		gpuCudaErrorCheck(cudaFree(a_d));
		gpuCudaErrorCheck(cudaFree(x_d));
		gpuCudaErrorCheck(cudaFree(y_d));
		gpuCudaErrorCheck(cudaFree(dBuffer));

		gpuCudaErrorCheck(cudaFreeHost(rowind_h));
		gpuCudaErrorCheck(cudaFreeHost(colind_h));
		gpuCudaErrorCheck(cudaFreeHost(a_h));
		gpuCudaErrorCheck(cudaFreeHost(x_h));
		gpuCudaErrorCheck(cudaFreeHost(y_h));

		gpuCudaErrorCheck(cudaEventDestroy(startEvent_execution));
		gpuCudaErrorCheck(cudaEventDestroy(endEvent_execution));

		if(TIME_IT){
			gpuCudaErrorCheck(cudaEventDestroy(startEvent_memcpy_rowind));
			gpuCudaErrorCheck(cudaEventDestroy(endEvent_memcpy_rowind));
			gpuCudaErrorCheck(cudaEventDestroy(startEvent_memcpy_colind));
			gpuCudaErrorCheck(cudaEventDestroy(endEvent_memcpy_colind));
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


void compute_coo(COOArrays * restrict coo, ValueType * restrict x , ValueType * restrict y);


void
COOArrays::spmv(ValueType * x, ValueType * y)
{
	compute_coo(this, x, y);
}


struct Matrix_Format *
csr_to_format(INT_T * row_ptr, INT_T * col_ind, ValueType * values, long m, long n, long nnz)
{
	struct COOArrays * coo = new COOArrays(row_ptr, col_ind, values, m, n, nnz);
	coo->mem_footprint = nnz * (sizeof(ValueType) + 2 * sizeof(INT_T));
	coo->format_name = (char *) "CUSPARSE_COO";
	return coo;
}


//==========================================================================================================================================
//= COO Custom
//==========================================================================================================================================


void
compute_coo(COOArrays * restrict coo, ValueType * restrict x, ValueType * restrict y)
{
	const double alpha = 1.0;
	const double beta = 0.0;
	if (coo->x == NULL)
	{
		coo->x = x;
		if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(coo->startEvent_memcpy_x, coo->stream));
		memcpy(coo->x_h, x, coo->n * sizeof(ValueType));
		gpuCudaErrorCheck(cudaMemcpyAsync(coo->x_d, coo->x_h, coo->n * sizeof(*coo->x_d), cudaMemcpyHostToDevice, coo->stream));
		if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(coo->endEvent_memcpy_x, coo->stream));
		if(TIME_IT){
			gpuCudaErrorCheck(cudaEventSynchronize(coo->endEvent_memcpy_x));
			float memcpyTime_cuda;
			gpuCudaErrorCheck(cudaEventElapsedTime(&memcpyTime_cuda, coo->startEvent_memcpy_x, coo->endEvent_memcpy_x));
			printf("(CUDA) Memcpy x time = %.4lf ms\n", memcpyTime_cuda);
		}

		// Create dense vector X
		if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(coo->startEvent_create_vecX));
		gpuCusparseErrorCheck(cusparseCreateDnVec(&coo->vecX, coo->n, coo->x_d, ValueTypeCuda));
		if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(coo->endEvent_create_vecX));

		// Create dense vector y
		if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(coo->startEvent_create_vecY));
		gpuCusparseErrorCheck(cusparseCreateDnVec(&coo->vecY, coo->m, coo->y_d, ValueTypeCuda));
		if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(coo->endEvent_create_vecY));

		// Allocate an external buffer if needed
		if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(coo->startEvent_spmv_buffersize));
		gpuCusparseErrorCheck(cusparseSpMV_bufferSize(coo->handle, CUSPARSE_OPERATION_NON_TRANSPOSE, &alpha, coo->matA, coo->vecX, &beta, coo->vecY, ValueTypeCuda, CUSPARSE_SPMV_ALG_DEFAULT, &coo->bufferSize));
		gpuCudaErrorCheck(cudaMalloc(&coo->dBuffer, coo->bufferSize));
		if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(coo->endEvent_spmv_buffersize));
		// printf("SpMV_bufferSize = %zu bytes\n", coo->bufferSize, coo->bufferSize); // size of the workspace that is needed by cusparseSpMV()

		if(TIME_IT){
			gpuCudaErrorCheck(cudaEventSynchronize(coo->endEvent_create_vecX));
			gpuCudaErrorCheck(cudaEventSynchronize(coo->endEvent_create_vecY));
			gpuCudaErrorCheck(cudaEventSynchronize(coo->endEvent_spmv_buffersize));
			float create_vecX_time, create_vecY_time, spmv_buffersize_time;
			gpuCudaErrorCheck(cudaEventElapsedTime(&create_vecX_time, coo->startEvent_create_vecX, coo->endEvent_create_vecX));
			gpuCudaErrorCheck(cudaEventElapsedTime(&create_vecY_time, coo->startEvent_create_vecY, coo->endEvent_create_vecY));
			gpuCudaErrorCheck(cudaEventElapsedTime(&spmv_buffersize_time, coo->startEvent_spmv_buffersize, coo->endEvent_spmv_buffersize));
			printf("(CUDA) Create vecX time = %.4lf ms, vecY time = %.4lf ms, spmv_buffersize time = %.4lf (SpMV_bufferSize = %zu)\n", create_vecX_time, create_vecY_time, spmv_buffersize_time, coo->bufferSize);
		}

		#ifdef PERSISTENT_L2_PREFETCH
			int x_d_size = coo->n * sizeof(*coo->x);
			gpuCudaErrorCheck(cudaCtxResetPersistingL2Cache()); // This needs to happen every time before running kernel for 1st time for a matrix...
			if(x_d_size < coo->max_persistent_l2_cache){
				cudaStreamAttrValue attribute;
				auto &window = attribute.accessPolicyWindow;
				window.base_ptr = coo->x_d;
				window.num_bytes = x_d_size;
				window.hitRatio = 1.0;
				window.hitProp = cudaAccessPropertyPersisting;
				window.missProp = cudaAccessPropertyStreaming;
				gpuCudaErrorCheck(cudaStreamSetAttribute(coo->stream, cudaStreamAttributeAccessPolicyWindow, &attribute));
			}
		#endif
	}

	gpuCusparseErrorCheck(cusparseSpMV(coo->handle, CUSPARSE_OPERATION_NON_TRANSPOSE, &alpha, coo->matA, coo->vecX, &beta, coo->vecY, ValueTypeCuda, CUSPARSE_SPMV_ALG_DEFAULT, coo->dBuffer));
	gpuCudaErrorCheck(cudaPeekAtLastError());
	gpuCudaErrorCheck(cudaDeviceSynchronize());

	if (coo->y == NULL)
	{
		coo->y = y;
		if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(coo->startEvent_memcpy_y, coo->stream));
		gpuCudaErrorCheck(cudaMemcpyAsync(coo->y_h, coo->y_d, coo->m * sizeof(*coo->y_d), cudaMemcpyDeviceToHost, coo->stream));
		gpuCudaErrorCheck(cudaStreamSynchronize(coo->stream));
		memcpy(y, coo->y_h, coo->m * sizeof(ValueType));
		if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(coo->endEvent_memcpy_y, coo->stream));

		if(TIME_IT){
			gpuCudaErrorCheck(cudaEventSynchronize(coo->endEvent_memcpy_y));
			float memcpyTime_cuda;
			gpuCudaErrorCheck(cudaEventElapsedTime(&memcpyTime_cuda, coo->startEvent_memcpy_y, coo->endEvent_memcpy_y));
			printf("(CUDA) Memcpy y time = %.4lf ms\n", memcpyTime_cuda);
		}
	}
}


//==========================================================================================================================================
//= Print Statistics
//==========================================================================================================================================


void
COOArrays::statistics_start()
{
}


int
statistics_print_labels(__attribute__((unused)) char * buf, __attribute__((unused)) long buf_n)
{
	return 0;
}


int
COOArrays::statistics_print_data(__attribute__((unused)) char * buf, __attribute__((unused)) long buf_n)
{
	return 0;
}

