#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

#include <cuda.h>
#include <cusparse.h>

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
	#include "cuda/cusparse_util.h"
#ifdef __cplusplus
}
#endif

#if DOUBLE == 0
	#define ValueTypeCuda  CUDA_R_32F
#elif DOUBLE == 1
	#define ValueTypeCuda  CUDA_R_64F
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
	cusparseDnVecDescr_t vecX;
	cusparseDnVecDescr_t vecY;

	CSCArrays(INT_T * row_ptr, INT_T * col_ind, ValueTypeReference * a_ref, long m, long n, long nnz) : Matrix_Format(m, n, nnz)
	{
		// Allocate and convert values from ValueTypeReference (e.g. double) to ValueType (e.g. float).
		a = (ValueType *) aligned_alloc(64, nnz * sizeof(ValueType));
		#pragma omp parallel for
		for (long i = 0; i < nnz; i++)
			a[i] = (ValueType) a_ref[i];

		int max_smem_per_block, multiproc_count, max_threads_per_block, warp_size, max_threads_per_multiproc, max_persistent_l2_cache;
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

		gpuCudaErrorCheck(cudaMalloc(&ia_d, nnz * sizeof(*ia_d)));
		gpuCudaErrorCheck(cudaMalloc(&ja_d, (n+1) * sizeof(*ja_d)));
		gpuCudaErrorCheck(cudaMalloc(&a_d, nnz * sizeof(*a_d)));
		gpuCudaErrorCheck(cudaMalloc(&x_d, n * sizeof(*x_d)));
		gpuCudaErrorCheck(cudaMalloc(&y_d, m * sizeof(*y_d)));

		gpuCusparseErrorCheck(cusparseCreate(&handle));

		size_t csc_bufferSize = 0;
		void* csc_dBuffer    = NULL;
		gpuCusparseErrorCheck(cusparseCsr2cscEx2_bufferSize(handle, m, n, nnz, a, row_ptr, col_ind, a, ja, ia, ValueTypeCuda, CUSPARSE_ACTION_NUMERIC, CUSPARSE_INDEX_BASE_ZERO, CUSPARSE_CSR2CSC_ALG_DEFAULT, &csc_bufferSize));
		gpuCudaErrorCheck(cudaMalloc(&csc_dBuffer, csc_bufferSize));
		gpuCusparseErrorCheck(cusparseCsr2cscEx2(handle, m, n, nnz, a, row_ptr, col_ind, a, ja, ia, ValueTypeCuda, CUSPARSE_ACTION_NUMERIC, CUSPARSE_INDEX_BASE_ZERO, CUSPARSE_CSR2CSC_ALG_DEFAULT, csc_dBuffer));
		gpuCudaErrorCheck(cudaFree(csc_dBuffer));

		gpuCudaErrorCheck(cudaMallocHost(&ia_h, nnz * sizeof(*ia_h)));
		gpuCudaErrorCheck(cudaMallocHost(&ja_h, (n+1) * sizeof(*ja_h)));
		gpuCudaErrorCheck(cudaMallocHost(&a_h, nnz * sizeof(*a_h)));

		memcpy(ia_h, ia, nnz * sizeof(*ia_h));
		memcpy(ja_h, ja, (n+1) * sizeof(*ja_h));
		memcpy(a_h, a, nnz * sizeof(*a_h));

		gpuCudaErrorCheck(cudaMemcpy(ia_d, ia_h, nnz * sizeof(*ia_d), cudaMemcpyHostToDevice));

		gpuCudaErrorCheck(cudaMemcpy(ja_d, ja_h, (n+1) * sizeof(*ja_d), cudaMemcpyHostToDevice));

		gpuCudaErrorCheck(cudaMemcpy(a_d, a_h, nnz * sizeof(*a_d), cudaMemcpyHostToDevice));

		// Create sparse matrix A in CSC format
		gpuCusparseErrorCheck(cusparseCreateCsc(&matA, m, n, nnz, ja_d, ia_d, a_d, CUSPARSE_INDEX_32I, CUSPARSE_INDEX_32I, CUSPARSE_INDEX_BASE_ZERO, ValueTypeCuda));
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

		gpuCudaErrorCheck(cudaFree(ia_d));
		gpuCudaErrorCheck(cudaFree(ja_d));
		gpuCudaErrorCheck(cudaFree(a_d));
		gpuCudaErrorCheck(cudaFree(x_d));
		gpuCudaErrorCheck(cudaFree(y_d));
		gpuCudaErrorCheck(cudaFree(dBuffer));

		gpuCudaErrorCheck(cudaFreeHost(ia_h));
		gpuCudaErrorCheck(cudaFreeHost(ja_h));
		gpuCudaErrorCheck(cudaFreeHost(a_h));
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
csr_to_format(INT_T * row_ptr, INT_T * col_ind, ValueTypeReference * values, long m, long n, long nnz, long symmetric, long symmetry_expanded)
{
	if (symmetric && !symmetry_expanded)
		error("symmetric matrices not supported by this format, expand symmetry");
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
	const ValueType alpha = 1.0;
	const ValueType beta = 0.0;
	if (csc->x == NULL)
	{
		csc->x = x;
		gpuCudaErrorCheck(cudaMemcpy(csc->x_d, x, csc->n * sizeof(*csc->x_d), cudaMemcpyHostToDevice));

		// Create dense vector X
		gpuCusparseErrorCheck(cusparseCreateDnVec(&csc->vecX, csc->n, csc->x_d, ValueTypeCuda));

		// Create dense vector y
		gpuCusparseErrorCheck(cusparseCreateDnVec(&csc->vecY, csc->m, csc->y_d, ValueTypeCuda));

		// Allocate an external buffer if needed
		gpuCusparseErrorCheck(cusparseSpMV_bufferSize(csc->handle, CUSPARSE_OPERATION_NON_TRANSPOSE, &alpha, csc->matA, csc->vecX, &beta, csc->vecY, ValueTypeCuda, CUSPARSE_SPMV_ALG_DEFAULT, &csc->bufferSize));
		gpuCudaErrorCheck(cudaMalloc(&csc->dBuffer, csc->bufferSize));
		printf("SpMV_bufferSize = %zu bytes\n", csc->bufferSize); // size of the workspace that is needed by cusparseSpMV()

		gpuCusparseErrorCheck(cusparseSpMV_preprocess(csc->handle, CUSPARSE_OPERATION_NON_TRANSPOSE, &alpha, csc->matA, csc->vecX, &beta, csc->vecY, ValueTypeCuda, CUSPARSE_SPMV_ALG_DEFAULT, csc->dBuffer));
	}

	gpuCusparseErrorCheck(cusparseSpMV(csc->handle, CUSPARSE_OPERATION_NON_TRANSPOSE, &alpha, csc->matA, csc->vecX, &beta, csc->vecY, ValueTypeCuda, CUSPARSE_SPMV_ALG_DEFAULT, csc->dBuffer));
	gpuCudaErrorCheck(cudaDeviceSynchronize());

	if (csc->y == NULL)
	{
		csc->y = y;
		gpuCudaErrorCheck(cudaMemcpy(y, csc->y_d, csc->m * sizeof(*csc->y_d), cudaMemcpyDeviceToHost));
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

