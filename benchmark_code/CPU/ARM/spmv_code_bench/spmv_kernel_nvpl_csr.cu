#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

#include <cuda.h>
#include "nvpl_sparse.h"
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
	#include "cuda/nvpl_sparse_util.h"
#ifdef __cplusplus
}
#endif

extern int prefetch_distance;

#if DOUBLE == 0
	#define ValueTypeCuda  NVPL_SPARSE_R_32F
#elif DOUBLE == 1
	#define ValueTypeCuda  NVPL_SPARSE_R_64F
#endif

double * thread_time_compute, * thread_time_barrier;

#ifndef TIME_IT
#define TIME_IT 0
#endif

struct CSRArrays : Matrix_Format
{
	INT_T * ia;      // the usual rowptr (of size m+1)
	INT_T * ja;      // the colidx of each NNZ (of size nnz)
	ValueType * a;   // the values (of size NNZ)

	nvpl_sparse_handle_t     handle = NULL;
	nvpl_sparse_sp_mat_descr_t matA;
    nvpl_sparse_spmv_descr_t   mv_descr;
	size_t               bufferSize = 0;

	ValueType * x = NULL;
	ValueType * y = NULL;
	ValueType * z = NULL;
	nvpl_sparse_dn_vec_descr_t vecX;
	nvpl_sparse_dn_vec_descr_t vecY;
	nvpl_sparse_dn_vec_descr_t vecZ;

	int max_persistent_l2_cache;

	CSRArrays(INT_T * ia, INT_T * ja, ValueType * a, long m, long n, long nnz) : Matrix_Format(m, n, nnz), ia(ia), ja(ja), a(a)
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

		gpuNVPLSparseErrorCheck(nvpl_sparse_create(&handle));
	    gpuNVPLSparseErrorCheck(nvpl_sparse_spmv_create_descr(&mv_descr));

		// Create sparse matrix A in CSR format
		gpuNVPLSparseErrorCheck(nvpl_sparse_create_csr(&matA, m, n, nnz, ia, ja, a, NVPL_SPARSE_INDEX_32I, NVPL_SPARSE_INDEX_32I, NVPL_SPARSE_INDEX_BASE_ZERO, ValueTypeCuda));
	}

	~CSRArrays()
	{
		free(a);
		free(ia);
		free(ja);

		// destroy matrix/vector descriptors
		gpuNVPLSparseErrorCheck(nvpl_sparse_destroy_sp_mat(matA));
		gpuNVPLSparseErrorCheck(nvpl_sparse_destroy_dn_vec(vecX));
		gpuNVPLSparseErrorCheck(nvpl_sparse_destroy_dn_vec(vecY));
		gpuNVPLSparseErrorCheck(nvpl_sparse_destroy_dn_vec(vecZ));
		gpuNVPLSparseErrorCheck(nvpl_sparse_spmv_destroy_descr(mv_descr));
		gpuNVPLSparseErrorCheck(nvpl_sparse_destroy(handle));

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
	csr->format_name = (char *) "NVPL_SPARSE_CSR";
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
		// Create dense vector X
		gpuNVPLSparseErrorCheck(nvpl_sparse_create_dn_vec(&csr->vecX, csr->n, x, ValueTypeCuda));

		// Create dense vector y
		gpuNVPLSparseErrorCheck(nvpl_sparse_create_dn_vec(&csr->vecY, csr->m, y, ValueTypeCuda));

		// Create dense vector z
		gpuNVPLSparseErrorCheck(nvpl_sparse_create_dn_vec(&csr->vecZ, csr->m, y, ValueTypeCuda));

		// Allocate an external buffer if needed
		void* dBuffer = NULL;
		gpuNVPLSparseErrorCheck(nvpl_sparse_spmv_buffer_size(csr->handle, NVPL_SPARSE_OPERATION_NON_TRANSPOSE, &alpha, csr->matA, csr->vecX, &beta, csr->vecY, csr->vecZ, ValueTypeCuda, NVPL_SPARSE_SPMV_ALG_DEFAULT, csr->mv_descr, &csr->bufferSize));
		dBuffer = malloc(csr->bufferSize);
		gpuNVPLSparseErrorCheck(nvpl_sparse_spmv_analysis(csr->handle, NVPL_SPARSE_OPERATION_NON_TRANSPOSE, &alpha, csr->matA, csr->vecX, &beta, csr->vecY, csr->vecZ, ValueTypeCuda, NVPL_SPARSE_SPMV_ALG_DEFAULT, csr->mv_descr, dBuffer));
	}

	gpuNVPLSparseErrorCheck(nvpl_sparse_spmv(csr->handle, NVPL_SPARSE_OPERATION_NON_TRANSPOSE, &alpha, csr->matA, csr->vecX, &beta, csr->vecY, csr->vecZ, ValueTypeCuda, NVPL_SPARSE_SPMV_ALG_DEFAULT, csr->mv_descr));
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

