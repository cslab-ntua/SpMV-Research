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
#ifdef __cplusplus
}
#endif


#if DOUBLE == 0
	#define ValueTypeCuda  CUDA_R_32F
#elif DOUBLE == 1
	#define ValueTypeCuda  CUDA_R_64F
#endif

double * thread_time_compute, * thread_time_barrier;


struct COOArrays : Matrix_Format
{
	INT_T * rowind;      // the usual rowptr (of size m+1)
	INT_T * colind;      // the colidx of each NNZ (of size nnz)
	ValueType * a;   // the values (of size NNZ)

	INT_T * rowind_dev;
	INT_T * colind_dev;
	ValueType * a_dev;

	cusparseHandle_t     handle = NULL;
	cusparseSpMatDescr_t matA;
	void*                dBuffer    = NULL;
	size_t               bufferSize = 0;

	ValueType * x = NULL;
	ValueType * y = NULL;
	ValueType * x_dev = NULL;
	ValueType * y_dev = NULL;
	cusparseDnVecDescr_t vecX;
	cusparseDnVecDescr_t vecY;

	int max_smem_per_block, multiproc_count, max_threads_per_block, warp_size, max_threads_per_multiproc;

	COOArrays(INT_T * ia, INT_T * ja, ValueType * a, long m, long n, long nnz) : Matrix_Format(m, n, nnz), colind(ja), a(a)
	{
		cudaDeviceGetAttribute(&max_smem_per_block, cudaDevAttrMaxSharedMemoryPerBlock, 0);
		cudaDeviceGetAttribute(&multiproc_count, cudaDevAttrMultiProcessorCount, 0);
		cudaDeviceGetAttribute(&max_threads_per_block, cudaDevAttrMaxThreadsPerBlock , 0);
		cudaDeviceGetAttribute(&warp_size, cudaDevAttrWarpSize , 0);
		cudaDeviceGetAttribute(&max_threads_per_multiproc, cudaDevAttrMaxThreadsPerMultiProcessor, 0);
		printf("max_smem_per_block=%d\n", max_smem_per_block);
		printf("multiproc_count=%d\n", multiproc_count);
		printf("max_threads_per_block=%d\n", max_threads_per_block);
		printf("warp_size=%d\n", warp_size);
		printf("max_threads_per_multiproc=%d\n", max_threads_per_multiproc);

		cudaMalloc(&rowind_dev, nnz * sizeof(*rowind_dev));
		cudaMalloc(&colind_dev, nnz * sizeof(*colind_dev));
		cudaMalloc(&a_dev, nnz * sizeof(*a_dev));
		cudaMalloc(&x_dev, n * sizeof(*x_dev));
		cudaMalloc(&y_dev, m * sizeof(*y_dev));

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

		cudaMemcpy(rowind_dev, rowind, nnz * sizeof(*rowind_dev), cudaMemcpyHostToDevice);
		cudaMemcpy(colind_dev, colind, nnz * sizeof(*colind_dev), cudaMemcpyHostToDevice);
		cudaMemcpy(a_dev, a, nnz * sizeof(*a_dev), cudaMemcpyHostToDevice);

		cusparseCreate(&handle);
		// Create sparse matrix A in CSR format
		cusparseCreateCoo(&matA, m, n, nnz, rowind_dev, colind_dev, a_dev, CUSPARSE_INDEX_32I, CUSPARSE_INDEX_BASE_ZERO, ValueTypeCuda);
	}

	~COOArrays()
	{
		free(a);
		free(rowind);
		free(colind);

		// destroy matrix/vector descriptors
		cusparseDestroySpMat(matA);
		cusparseDestroyDnVec(vecX);
		cusparseDestroyDnVec(vecY);
		cusparseDestroy(handle);

		cudaFree(rowind_dev);
		cudaFree(colind_dev);
		cudaFree(a_dev);

		#ifdef PRINT_STATISTICS
			free(thread_time_barrier);
			free(thread_time_compute);
		#endif
	}

	void spmv(ValueType * x, ValueType * y);
	void statistics_start();
	int statistics_print_data(__attribute__((unused)) char * buf, __attribute__((unused)) long buf_n);
};


void compute_csr(COOArrays * restrict coo, ValueType * restrict x , ValueType * restrict y);
void compute_csr_kahan(COOArrays * restrict coo, ValueType * restrict x, ValueType * restrict y);
void compute_csr_prefetch(COOArrays * restrict coo, ValueType * restrict x , ValueType * restrict y);
void compute_csr_omp_simd(COOArrays * restrict coo, ValueType * restrict x , ValueType * restrict y);
void compute_csr_vector(COOArrays * restrict coo, ValueType * restrict x , ValueType * restrict y);
void compute_csr_vector_perfect_nnz_balance(COOArrays * restrict coo, ValueType * restrict x , ValueType * restrict y);


void
COOArrays::spmv(ValueType * x, ValueType * y)
{
	compute_csr(this, x, y);
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
//= CSR Custom
//==========================================================================================================================================


void
compute_csr(COOArrays * restrict coo, ValueType * restrict x, ValueType * restrict y)
{
	const double alpha = 1.0;
	const double beta = 0.0;
	if (coo->x == NULL)
	{
		coo->x = x;
		cudaMemcpy(coo->x_dev, coo->x, coo->n * sizeof(*coo->x), cudaMemcpyHostToDevice);
		// Create dense vector X
		cusparseCreateDnVec(&coo->vecX, coo->n, coo->x_dev, ValueTypeCuda);
		// Create dense vector y
		cusparseCreateDnVec(&coo->vecY, coo->m, coo->y_dev, ValueTypeCuda);
		// Allocate an external buffer if needed
		cusparseSpMV_bufferSize(coo->handle, CUSPARSE_OPERATION_NON_TRANSPOSE, &alpha, coo->matA, coo->vecX, &beta, coo->vecY, ValueTypeCuda, CUSPARSE_SPMV_ALG_DEFAULT, &coo->bufferSize);
		cudaMalloc(&coo->dBuffer, coo->bufferSize);
	}
	// Execute SpMV
	cusparseSpMV(coo->handle, CUSPARSE_OPERATION_NON_TRANSPOSE, &alpha, coo->matA, coo->vecX, &beta, coo->vecY, ValueTypeCuda, CUSPARSE_SPMV_ALG_DEFAULT, coo->dBuffer);
	cudaError_t err;
	err = cudaDeviceSynchronize();
	if (err != cudaSuccess)
		error("gpu kernel error: %s\n", cudaGetErrorString(err));
	if (coo->y == NULL)
	{
		coo->y = y;
		cudaMemcpy(coo->y, coo->y_dev, coo->m * sizeof(*coo->y), cudaMemcpyDeviceToHost);
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

