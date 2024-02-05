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


struct CSRArrays : Matrix_Format
{
	INT_T * ia;      // the usual rowptr (of size m+1)
	INT_T * ja;      // the colidx of each NNZ (of size nnz)
	ValueType * a;   // the values (of size NNZ)

	INT_T * ia_dev;
	INT_T * ja_dev;
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

	CSRArrays(INT_T * ia, INT_T * ja, ValueType * a, long m, long n, long nnz) : Matrix_Format(m, n, nnz), ia(ia), ja(ja), a(a)
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


		cudaMalloc(&ia_dev, (m+1) * sizeof(*ia_dev));
		cudaMalloc(&ja_dev, nnz * sizeof(*ja_dev));
		cudaMalloc(&a_dev, nnz * sizeof(*a_dev));
		cudaMalloc(&x_dev, n * sizeof(*x_dev));
		cudaMalloc(&y_dev, m * sizeof(*y_dev));

		cudaMemcpy(ia_dev, ia, (m+1) * sizeof(*ia_dev), cudaMemcpyHostToDevice);
		cudaMemcpy(ja_dev, ja, nnz * sizeof(*ja_dev), cudaMemcpyHostToDevice);
		cudaMemcpy(a_dev, a, nnz * sizeof(*a_dev), cudaMemcpyHostToDevice);

		cusparseCreate(&handle);
		// Create sparse matrix A in CSR format
		cusparseCreateCsr(&matA, m, n, nnz, ia_dev, ja_dev, a_dev, CUSPARSE_INDEX_32I, CUSPARSE_INDEX_32I, CUSPARSE_INDEX_BASE_ZERO, ValueTypeCuda);
	}

	~CSRArrays()
	{
		free(a);
		free(ia);
		free(ja);

		// destroy matrix/vector descriptors
		cusparseDestroySpMat(matA);
		cusparseDestroyDnVec(vecX);
		cusparseDestroyDnVec(vecY);
		cusparseDestroy(handle);

		cudaFree(ia_dev);
		cudaFree(ja_dev);
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


void compute_csr(CSRArrays * restrict csr, ValueType * restrict x , ValueType * restrict y);
void compute_csr_kahan(CSRArrays * restrict csr, ValueType * restrict x, ValueType * restrict y);
void compute_csr_prefetch(CSRArrays * restrict csr, ValueType * restrict x , ValueType * restrict y);
void compute_csr_omp_simd(CSRArrays * restrict csr, ValueType * restrict x , ValueType * restrict y);
void compute_csr_vector(CSRArrays * restrict csr, ValueType * restrict x , ValueType * restrict y);
void compute_csr_vector_perfect_nnz_balance(CSRArrays * restrict csr, ValueType * restrict x , ValueType * restrict y);


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
		cudaMemcpy(csr->x_dev, csr->x, csr->n * sizeof(*csr->x), cudaMemcpyHostToDevice);
		// Create dense vector X
		cusparseCreateDnVec(&csr->vecX, csr->n, csr->x_dev, ValueTypeCuda);
		// Create dense vector y
		cusparseCreateDnVec(&csr->vecY, csr->m, csr->y_dev, ValueTypeCuda);
		// Allocate an external buffer if needed
		cusparseSpMV_bufferSize(csr->handle, CUSPARSE_OPERATION_NON_TRANSPOSE, &alpha, csr->matA, csr->vecX, &beta, csr->vecY, ValueTypeCuda, CUSPARSE_SPMV_ALG_DEFAULT, &csr->bufferSize);
		cudaMalloc(&csr->dBuffer, csr->bufferSize);
	}
	// Execute SpMV
	cusparseSpMV(csr->handle, CUSPARSE_OPERATION_NON_TRANSPOSE, &alpha, csr->matA, csr->vecX, &beta, csr->vecY, ValueTypeCuda, CUSPARSE_SPMV_ALG_DEFAULT, csr->dBuffer);
	cudaError_t err;
	err = cudaDeviceSynchronize();
	if (err != cudaSuccess)
		error("gpu kernel error: %s\n", cudaGetErrorString(err));
	if (csr->y == NULL)
	{
		csr->y = y;
		cudaMemcpy(csr->y, csr->y_dev, csr->m * sizeof(*csr->y), cudaMemcpyDeviceToHost);
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

