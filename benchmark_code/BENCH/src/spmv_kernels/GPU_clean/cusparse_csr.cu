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

	CSRArrays(INT_T * ia, INT_T * ja, ValueTypeReference * a_ref, long m, long n, long nnz) : Matrix_Format(m, n, nnz), ia(ia), ja(ja)
	{
		cuda_device_print_attributes();

		cuda_assert(cudaMalloc(&ia_d, (m+1) * sizeof(*ia_d)));
		cuda_assert(cudaMalloc(&ja_d, nnz * sizeof(*ja_d)));
		cuda_assert(cudaMalloc(&a_d, nnz * sizeof(*a_d)));
		cuda_assert(cudaMalloc(&x_d, n * sizeof(*x_d)));
		cuda_assert(cudaMalloc(&y_d, m * sizeof(*y_d)));

		// Convert values from ValueTypeReference (double) to ValueType (e.g., float).
		a = (typeof(a)) malloc(nnz * sizeof(*a));
		#pragma omp parallel for
		for (long i = 0; i < nnz; i++)
			a[i] = (ValueType) a_ref[i];

		gpuCusparseErrorCheck(cusparseCreate(&handle));

		cuda_assert(cudaMallocHost(&ia_h, (m+1) * sizeof(*ia_h)));
		cuda_assert(cudaMallocHost(&ja_h, nnz * sizeof(*ja_h)));
		cuda_assert(cudaMallocHost(&a_h, nnz * sizeof(*a_h)));

		memcpy(ia_h, ia, (m+1) * sizeof(*ia_h));
		memcpy(ja_h, ja, nnz * sizeof(*ja_h));
		memcpy(a_h, a, nnz * sizeof(*a_h));

		cuda_assert(cudaMemcpy(ia_d, ia_h, (m+1) * sizeof(*ia_d), cudaMemcpyHostToDevice));

		cuda_assert(cudaMemcpy(ja_d, ja_h, nnz * sizeof(*ja_d), cudaMemcpyHostToDevice));

		cuda_assert(cudaMemcpy(a_d, a_h, nnz * sizeof(*a_d), cudaMemcpyHostToDevice));

		// Create sparse matrix A in CSR format
		gpuCusparseErrorCheck(cusparseCreateCsr(&matA, m, n, nnz, ia_d, ja_d, a_d, CUSPARSE_INDEX_32I, CUSPARSE_INDEX_32I, CUSPARSE_INDEX_BASE_ZERO, ValueTypeCuda));

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

		cuda_assert(cudaFree(ia_d));
		cuda_assert(cudaFree(ja_d));
		cuda_assert(cudaFree(a_d));

		cuda_assert(cudaFree(x_d));
		cuda_assert(cudaFree(y_d));
		cuda_assert(cudaFree(dBuffer));

		cuda_assert(cudaFreeHost(ia_h));
		cuda_assert(cudaFreeHost(ja_h));
		cuda_assert(cudaFreeHost(a_h));
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
csr_to_format(INT_T * row_ptr, INT_T * col_ind, ValueTypeReference * values, long m, long n, long nnz, long symmetric, long symmetry_expanded)
{
	if (symmetric && !symmetry_expanded)
		error("symmetric matrices not supported by this format, expand symmetry");
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
	const ValueType alpha = 1.0;
	const ValueType beta = 0.0;
	if (csr->x == NULL)
	{
		csr->x = x;
		cuda_assert(cudaMemcpy(csr->x_d, x, csr->n * sizeof(*csr->x_d), cudaMemcpyHostToDevice));

		// Create dense vector X
		gpuCusparseErrorCheck(cusparseCreateDnVec(&csr->vecX, csr->n, csr->x_d, ValueTypeCuda));

		// Create dense vector y
		gpuCusparseErrorCheck(cusparseCreateDnVec(&csr->vecY, csr->m, csr->y_d, ValueTypeCuda));

		// Allocate an external buffer if needed
		gpuCusparseErrorCheck(cusparseSpMV_bufferSize(csr->handle, CUSPARSE_OPERATION_NON_TRANSPOSE, &alpha, csr->matA, csr->vecX, &beta, csr->vecY, ValueTypeCuda, CUSPARSE_SPMV_ALG_DEFAULT, &csr->bufferSize));
		cuda_assert(cudaMalloc(&csr->dBuffer, csr->bufferSize));
		printf("SpMV_bufferSize = %lu bytes\n", csr->bufferSize); // size of the workspace that is needed by cusparseSpMV()

		gpuCusparseErrorCheck(cusparseSpMV_preprocess(csr->handle, CUSPARSE_OPERATION_NON_TRANSPOSE, &alpha, csr->matA, csr->vecX, &beta, csr->vecY, ValueTypeCuda, CUSPARSE_SPMV_ALG_DEFAULT, csr->dBuffer));
	}

	gpuCusparseErrorCheck(cusparseSpMV(csr->handle, CUSPARSE_OPERATION_NON_TRANSPOSE, &alpha, csr->matA, csr->vecX, &beta, csr->vecY, ValueTypeCuda, CUSPARSE_SPMV_ALG_DEFAULT, csr->dBuffer));
	cuda_assert(cudaDeviceSynchronize());

	if (csr->y == NULL)
	{
		csr->y = y;
		cuda_assert(cudaMemcpy(y, csr->y_d, csr->m * sizeof(*csr->y_d), cudaMemcpyDeviceToHost));
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

