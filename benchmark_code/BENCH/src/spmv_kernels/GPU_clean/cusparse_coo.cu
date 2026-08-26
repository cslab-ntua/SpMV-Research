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


struct COOArrays : Matrix_Format
{
	INT_T * rowind;      // the usual rowptr (of size m+1)
	INT_T * colind;      // the colidx of each NNZ (of size nnz)
	ValueType * a;       // the values (of size NNZ)

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
	cusparseDnVecDescr_t vecX;
	cusparseDnVecDescr_t vecY;

	COOArrays(INT_T * ia, INT_T * ja, ValueTypeReference * a_ref, long m, long n, long nnz) : Matrix_Format(m, n, nnz), colind(ja)
	{

		cuda_device_print_attributes();

		cuda_assert(cudaMalloc(&rowind_d, nnz * sizeof(*rowind_d)));
		cuda_assert(cudaMalloc(&colind_d, nnz * sizeof(*colind_d)));
		cuda_assert(cudaMalloc(&a_d, nnz * sizeof(*a_d)));
		cuda_assert(cudaMalloc(&x_d, n * sizeof(*x_d)));
		cuda_assert(cudaMalloc(&y_d, m * sizeof(*y_d)));
		a = (typeof(a)) malloc(nnz * sizeof(*a));

		// Convert values from ValueTypeReference (double) to ValueType (e.g., float).
		#pragma omp parallel for
		for (long i = 0; i < nnz; i++)
			a[i] = (ValueType) a_ref[i];

		cusparse_assert(cusparseCreate(&handle));


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

		cuda_assert(cudaMallocHost(&rowind_h, nnz * sizeof(*rowind_h)));
		cuda_assert(cudaMallocHost(&colind_h, nnz * sizeof(*colind_h)));
		cuda_assert(cudaMallocHost(&a_h, nnz * sizeof(*a_h)));

		memcpy(rowind_h, rowind, nnz * sizeof(*rowind_h));
		memcpy(colind_h, colind, nnz * sizeof(*colind_h));
		memcpy(a_h, a, nnz * sizeof(*a_h));

		cuda_assert(cudaMemcpy(rowind_d, rowind_h, nnz * sizeof(*rowind_d), cudaMemcpyHostToDevice));

		cuda_assert(cudaMemcpy(colind_d, colind_h, nnz * sizeof(*colind_d), cudaMemcpyHostToDevice));

		cuda_assert(cudaMemcpy(a_d, a_h, nnz * sizeof(*a_d), cudaMemcpyHostToDevice));

		// Create sparse matrix A in COO format
		cusparse_assert(cusparseCreateCoo(&matA, m, n, nnz, rowind_d, colind_d, a_d, CUSPARSE_INDEX_32I, CUSPARSE_INDEX_BASE_ZERO, ValueTypeCuda));

	}

	~COOArrays()
	{
		free(a);
		free(rowind);
		free(colind);

		// destroy matrix/vector descriptors
		cusparse_assert(cusparseDestroySpMat(matA));
		cusparse_assert(cusparseDestroyDnVec(vecX));
		cusparse_assert(cusparseDestroyDnVec(vecY));
		cusparse_assert(cusparseDestroy(handle));

		cuda_assert(cudaFree(rowind_d));
		cuda_assert(cudaFree(colind_d));
		cuda_assert(cudaFree(a_d));

		cuda_assert(cudaFree(x_d));
		cuda_assert(cudaFree(y_d));
		cuda_assert(cudaFree(dBuffer));

		cuda_assert(cudaFreeHost(rowind_h));
		cuda_assert(cudaFreeHost(colind_h));
		cuda_assert(cudaFreeHost(a_h));
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
csr_to_format(INT_T * row_ptr, INT_T * col_ind, ValueTypeReference * values, long m, long n, long nnz, long symmetric, long symmetry_expanded)
{
	if (symmetric && !symmetry_expanded)
		error("symmetric matrices not supported by this format, expand symmetry");
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
	const ValueType alpha = 1.0;
	const ValueType beta = 0.0;
	if (coo->x == NULL)
	{
		coo->x = x;

		cuda_assert(cudaMemcpy(coo->x_d, x, coo->n * sizeof(*coo->x_d), cudaMemcpyHostToDevice));

		// Create dense vector X
		cusparse_assert(cusparseCreateDnVec(&coo->vecX, coo->n, coo->x_d, ValueTypeCuda));

		// Create dense vector y
		cusparse_assert(cusparseCreateDnVec(&coo->vecY, coo->m, coo->y_d, ValueTypeCuda));

		// Allocate an external buffer if needed
		cusparse_assert(cusparseSpMV_bufferSize(coo->handle, CUSPARSE_OPERATION_NON_TRANSPOSE, &alpha, coo->matA, coo->vecX, &beta, coo->vecY, ValueTypeCuda, CUSPARSE_SPMV_ALG_DEFAULT, &coo->bufferSize));
		cuda_assert(cudaMalloc(&coo->dBuffer, coo->bufferSize));
		printf("SpMV_bufferSize = %lu bytes\n", coo->bufferSize); // size of the workspace that is needed by cusparseSpMV()
	}

	cusparse_assert(cusparseSpMV(coo->handle, CUSPARSE_OPERATION_NON_TRANSPOSE, &alpha, coo->matA, coo->vecX, &beta, coo->vecY, ValueTypeCuda, CUSPARSE_SPMV_ALG_DEFAULT, coo->dBuffer));
	cuda_assert(cudaDeviceSynchronize());

	if (coo->y == NULL)
	{
		coo->y = y;
		cuda_assert(cudaMemcpy(y, coo->y_d, coo->m * sizeof(*coo->y_d), cudaMemcpyDeviceToHost));
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

