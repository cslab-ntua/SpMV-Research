#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

#include <cuda.h>
#include <cusparse.h>
#include <cublas_v2.h>
#include <cuda_runtime.h>

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
	#include "cuda/cublas_util.h"
	#include "cuda/cusparse_util.h"
	#include "aux/csr_util.h"
	#include "aux/csr_converter.h"
	#include "aux/csc_util.h"
	#include "aux/csc_converter.h"
#ifdef __cplusplus
}
#endif


#if DOUBLE == 0
	#define ValueTypeCuda  CUDA_R_32F
#elif DOUBLE == 1
	#define ValueTypeCuda  CUDA_R_64F
#endif

double * thread_time_compute, * thread_time_barrier;

#ifndef NUM_STREAMS
#define NUM_STREAMS 128
#endif

#ifndef TIME_IT
#define TIME_IT 1
#endif

#ifndef VERIFIED
#define VERIFIED 1
#endif

struct COOArrays : Matrix_Format
{
	INT_T * ia;      // the usual rowptr (of size m+1)
	INT_T * ja;      // the colidx of each NNZ (of size nnz)
	ValueType * a;   // the values (of size NNZ)
	INT_T * ia_h[NUM_STREAMS];
	INT_T * ja_h[NUM_STREAMS];
	ValueType * a_h[NUM_STREAMS];

	INT_T * ia_d[NUM_STREAMS];
	INT_T * ja_d[NUM_STREAMS];
	ValueType * a_d[NUM_STREAMS];

	cudaStream_t stream[NUM_STREAMS];
	INT_T n_stream[NUM_STREAMS];
	INT_T nnz_stream[NUM_STREAMS];

	cusparseHandle_t     handle[NUM_STREAMS];
	cusparseSpMatDescr_t matA[NUM_STREAMS];
	void*                dBuffer[NUM_STREAMS];
	size_t               bufferSize[NUM_STREAMS];

	ValueType * x = NULL;
	ValueType * y = NULL;
	ValueType * x_h[NUM_STREAMS];
	ValueType * y_h[NUM_STREAMS];
	ValueType * x_d[NUM_STREAMS];
	// ValueType * y_d[NUM_STREAMS];
	ValueType * y_d2;
	ValueType * y_d_reduction;

	cusparseDnVecDescr_t vecX[NUM_STREAMS];
	cusparseDnVecDescr_t vecY[NUM_STREAMS];

	// cudaEvent_t is useful for timing, but for performance use " cudaEventCreateWithFlags ( &event, cudaEventDisableTiming) "
	cudaEvent_t startEvent_execution[NUM_STREAMS];
	cudaEvent_t endEvent_execution[NUM_STREAMS];

	cudaEvent_t startEvent_memcpy_x[NUM_STREAMS];
	cudaEvent_t endEvent_memcpy_x[NUM_STREAMS];
	cudaEvent_t startEvent_memcpy_y;
	cudaEvent_t endEvent_memcpy_y;

	cudaEvent_t startEvent_memcpy_ia[NUM_STREAMS];
	cudaEvent_t endEvent_memcpy_ia[NUM_STREAMS];
	cudaEvent_t startEvent_memcpy_ja[NUM_STREAMS];
	cudaEvent_t endEvent_memcpy_ja[NUM_STREAMS];
	cudaEvent_t startEvent_memcpy_a[NUM_STREAMS];
	cudaEvent_t endEvent_memcpy_a[NUM_STREAMS];

	cudaEvent_t startEvent_create_matA[NUM_STREAMS];
	cudaEvent_t endEvent_create_matA[NUM_STREAMS];
	cudaEvent_t startEvent_spmv_buffersize[NUM_STREAMS];
	cudaEvent_t endEvent_spmv_buffersize[NUM_STREAMS];

	cudaEvent_t startEvent_create_vecX[NUM_STREAMS];
	cudaEvent_t endEvent_create_vecX[NUM_STREAMS];
	cudaEvent_t startEvent_create_vecY[NUM_STREAMS];
	cudaEvent_t endEvent_create_vecY[NUM_STREAMS];

	cublasHandle_t handle_blas;

	int num_streams;

	COOArrays(INT_T * ia, INT_T * ja, ValueType * a, long m, long n, long nnz) : Matrix_Format(m, n, nnz), ia(ia), ja(ja), a(a)
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

		num_streams = NUM_STREAMS;

		/********************************************************************************************************/
		printf("/********************************************************************************************************/\n");
		// Convert COO representation ton CSC
		INT_T * row_indices; //for CSC format
		INT_T * row_idx;
		INT_T * col_ptr;
		ValueType * val_c;

		row_indices = (typeof(row_indices)) malloc(nnz * sizeof(*row_indices));
		row_idx = (typeof(row_idx)) malloc(nnz * sizeof(*row_idx));
		col_ptr = (typeof(col_ptr)) malloc((n+1) * sizeof(*col_ptr));
		val_c = (typeof(val_c)) malloc(nnz * sizeof(*val_c));

		double time = time_it(1,
			csr_row_indices(ia, ja, m, n, nnz, &row_indices);
			coo_to_csc(row_indices, ja, a, m, n, nnz, row_idx, col_ptr, val_c, 1);
			free(row_indices);
		);
		printf("time coo_to_csc = %g ms\n", time*1e3);

		INT_T *local_stream_j_s = (INT_T *) malloc(num_streams * sizeof(*local_stream_j_s));
		INT_T *local_stream_j_e = (INT_T *) malloc(num_streams * sizeof(*local_stream_j_e));
		double time_balance = time_it(1,
			for (int i=0;i<num_streams;i++)
				loop_partitioner_balance_prefix_sums(num_streams, i, col_ptr, n, nnz, &local_stream_j_s[i], &local_stream_j_e[i]);
		);

		int cnt=0, cnt2=0;
		for(int i=0; i<num_streams; i++){
			nnz_stream[i] = col_ptr[local_stream_j_e[i]] - col_ptr[local_stream_j_s[i]];
			n_stream[i] = local_stream_j_e[i] - local_stream_j_s[i];
			// printf("local_stream[%d] = %d - %d (%d cols) (%d nnz)\n", i, local_stream_j_s[i], local_stream_j_e[i], n_stream[i], nnz_stream[i]);

			cnt  += nnz_stream[i];
			cnt2 += n_stream[i];
		}
		printf("balance time (col) = %g ms\n", time_balance*1e3);

		INT_T * row_idx_stream[num_streams];
		INT_T * col_ptr_stream[num_streams];
		ValueType * val_c_stream[num_streams];
		
		double time_memcpy_stream_locals = time_it(1,
		for(int i=0; i<num_streams; i++){
			col_ptr_stream[i] = (INT_T *) malloc((n_stream[i]+1) * sizeof(INT_T));
			row_idx_stream[i] = (INT_T *) malloc(nnz_stream[i] * sizeof(INT_T));
			val_c_stream[i] = (ValueType *) malloc(nnz_stream[i] * sizeof(ValueType));

			memcpy(col_ptr_stream[i], col_ptr + local_stream_j_s[i], (n_stream[i] + 1) * sizeof(INT_T));
			// col_ptr needs to be fixed, so that it will start from 0 again...
			for(int j=0; j<n_stream[i]+1; j++)
				col_ptr_stream[i][j] -= col_ptr[local_stream_j_s[i]];
			memcpy(row_idx_stream[i], row_idx + col_ptr[local_stream_j_s[i]], nnz_stream[i] * sizeof(INT_T));
			memcpy(val_c_stream[i], val_c + col_ptr[local_stream_j_s[i]], nnz_stream[i] * sizeof(ValueType));
		}
		);
		printf("time_memcpy_stream_locals = %lf ms\n", time_memcpy_stream_locals*1e3);
		free(local_stream_j_s);
		free(local_stream_j_e);

		INT_T * row_ptr_stream[num_streams];
		INT_T * col_idx_stream[num_streams];
		ValueType * val_stream[num_streams];

		for(int i=0; i<num_streams; i++){
			INT_T * col_indices;
			csc_col_indices(row_idx_stream[i], col_ptr_stream[i], m, n_stream[i], nnz_stream[i], &col_indices);

			row_ptr_stream[i] = (INT_T *) malloc((m+1) * sizeof(INT_T));
			col_idx_stream[i] = (INT_T *) malloc(nnz_stream[i] * sizeof(INT_T));
			val_stream[i] = (ValueType *) malloc(nnz_stream[i] * sizeof(ValueType));

			coo_to_csr(row_idx_stream[i], col_indices, val_c_stream[i], m, n_stream[i], nnz_stream[i], row_ptr_stream[i], col_idx_stream[i], val_stream[i], 1, 0);

			free(col_indices);
		}

		for(int i=0; i<num_streams; i++){
			// free(row_idx_stream[i]);
			free(col_ptr_stream[i]);
			free(val_c_stream[i]);
		}
		free(row_idx);
		free(col_ptr);
		free(val_c);

		printf("/********************************************************************************************************/\n");
		/********************************************************************************************************/

		for(int i=0; i<num_streams; i++){
			gpuCudaErrorCheck(cudaMalloc(&ia_d[i], nnz_stream[i] * sizeof(INT_T)));
			gpuCudaErrorCheck(cudaMalloc(&ja_d[i], nnz_stream[i] * sizeof(INT_T)));
			gpuCudaErrorCheck(cudaMalloc(&a_d[i], nnz_stream[i] * sizeof(ValueType)));
			gpuCudaErrorCheck(cudaMalloc(&x_d[i], n_stream[i] * sizeof(ValueType)));
			// gpuCudaErrorCheck(cudaMalloc(&y_d[i], m * sizeof(ValueType)));
		}
		gpuCudaErrorCheck(cudaMalloc(&y_d2, m * num_streams * sizeof(ValueType)));
		gpuCudaErrorCheck(cudaMalloc(&y_d_reduction, m * sizeof(ValueType)));
		gpuCublasErrorCheck(cublasCreate(&handle_blas));

		for(int i=0; i<num_streams; i++){
			gpuCudaErrorCheck(cudaMallocHost(&ia_h[i], nnz_stream[i] * sizeof(INT_T)));
			gpuCudaErrorCheck(cudaMallocHost(&ja_h[i], nnz_stream[i] * sizeof(INT_T)));
			gpuCudaErrorCheck(cudaMallocHost(&a_h[i], nnz_stream[i] * sizeof(ValueType)));
			gpuCudaErrorCheck(cudaMallocHost(&x_h[i], n_stream[i] * sizeof(ValueType)));
			gpuCudaErrorCheck(cudaMallocHost(&y_h[i], m * sizeof(ValueType)));
		}

		double time_memcpy = time_it(1,
		for(int i=0; i<num_streams; i++){
			memcpy(ia_h[i], row_idx_stream[i], nnz_stream[i] * sizeof(INT_T));
			memcpy(ja_h[i], col_idx_stream[i], nnz_stream[i] * sizeof(INT_T));
			memcpy(a_h[i], val_stream[i], nnz_stream[i] * sizeof(ValueType));
		}
		for(int i=0; i<num_streams; i++)
			free(row_idx_stream[i]);
		);
		printf("time_memcpy (ia_h, ja_h, a_h) = %lf ms\n", time_memcpy*1e3);

		for(int i=0; i<num_streams; i++){
			gpuCudaErrorCheck(cudaStreamCreate(&stream[i]));
			gpuCusparseErrorCheck(cusparseCreate(&handle[i]));
			gpuCusparseErrorCheck(cusparseSetStream(handle[i], stream[i]));

			// cuda events for timing measurements
			gpuCudaErrorCheck(cudaEventCreate(&startEvent_execution[i]));
			gpuCudaErrorCheck(cudaEventCreate(&endEvent_execution[i]));
		}
		gpuCublasErrorCheck(cublasSetStream(handle_blas, stream[0]));

		if(TIME_IT){
			for(int i=0; i<num_streams; i++){
				gpuCudaErrorCheck(cudaEventCreate(&startEvent_memcpy_ia[i]));
				gpuCudaErrorCheck(cudaEventCreate(&endEvent_memcpy_ia[i]));
				gpuCudaErrorCheck(cudaEventCreate(&startEvent_memcpy_ja[i]));
				gpuCudaErrorCheck(cudaEventCreate(&endEvent_memcpy_ja[i]));
				gpuCudaErrorCheck(cudaEventCreate(&startEvent_memcpy_a[i]));
				gpuCudaErrorCheck(cudaEventCreate(&endEvent_memcpy_a[i]));
				gpuCudaErrorCheck(cudaEventCreate(&startEvent_create_matA[i]));
				gpuCudaErrorCheck(cudaEventCreate(&endEvent_create_matA[i]));
				gpuCudaErrorCheck(cudaEventCreate(&startEvent_spmv_buffersize[i]));
				gpuCudaErrorCheck(cudaEventCreate(&endEvent_spmv_buffersize[i]));

				gpuCudaErrorCheck(cudaEventCreate(&startEvent_create_vecX[i]));
				gpuCudaErrorCheck(cudaEventCreate(&endEvent_create_vecX[i]));
				gpuCudaErrorCheck(cudaEventCreate(&startEvent_create_vecY[i]));
				gpuCudaErrorCheck(cudaEventCreate(&endEvent_create_vecY[i]));
				gpuCudaErrorCheck(cudaEventCreate(&startEvent_memcpy_x[i]));
				gpuCudaErrorCheck(cudaEventCreate(&endEvent_memcpy_x[i]));
			}
			gpuCudaErrorCheck(cudaEventCreate(&startEvent_memcpy_y));
			gpuCudaErrorCheck(cudaEventCreate(&endEvent_memcpy_y));
		}

		for(int i=0; i<num_streams; i++){
			if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(startEvent_memcpy_ia[i], stream[i]));
			gpuCudaErrorCheck(cudaMemcpyAsync(ia_d[i], ia_h[i], nnz_stream[i] * sizeof(INT_T), cudaMemcpyHostToDevice, stream[i]));
			if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(endEvent_memcpy_ia[i], stream[i]));

			if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(startEvent_memcpy_ja[i], stream[i]));
			gpuCudaErrorCheck(cudaMemcpyAsync(ja_d[i], ja_h[i], nnz_stream[i] * sizeof(INT_T), cudaMemcpyHostToDevice, stream[i]));
			if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(endEvent_memcpy_ja[i], stream[i]));

			if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(startEvent_memcpy_a[i], stream[i]));
			gpuCudaErrorCheck(cudaMemcpyAsync(a_d[i], a_h[i], nnz_stream[i] * sizeof(ValueType), cudaMemcpyHostToDevice, stream[i]));
			if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(endEvent_memcpy_a[i], stream[i]));

			// Create sparse matrix A in COO format
			if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(startEvent_create_matA[i], stream[i]));
			gpuCusparseErrorCheck(cusparseCreateCoo(&matA[i], m, n_stream[i], nnz_stream[i], ia_d[i], ja_d[i], a_d[i], CUSPARSE_INDEX_32I, CUSPARSE_INDEX_BASE_ZERO, ValueTypeCuda));
			if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(endEvent_create_matA[i], stream[i]));
		}

		if(TIME_IT){
			for(int i=0; i<num_streams; i++){
				gpuCudaErrorCheck(cudaStreamSynchronize(stream[i]));
				float memcpyTime_cuda_ia, memcpyTime_cuda_ja, memcpyTime_cuda_a, create_matA_Time;//memcpyTime_cuda_thread_i_e;
				gpuCudaErrorCheck(cudaEventElapsedTime(&memcpyTime_cuda_ia, startEvent_memcpy_ia[i], endEvent_memcpy_ia[i]));
				gpuCudaErrorCheck(cudaEventElapsedTime(&memcpyTime_cuda_ja, startEvent_memcpy_ja[i], endEvent_memcpy_ja[i]));
				gpuCudaErrorCheck(cudaEventElapsedTime(&memcpyTime_cuda_a, startEvent_memcpy_a[i], endEvent_memcpy_a[i]));
				gpuCudaErrorCheck(cudaEventElapsedTime(&create_matA_Time, startEvent_create_matA[i], endEvent_create_matA[i]));
				printf("(CUDA) (stream %d): Memcpy ia time = %.4lf ms, ja time = %.4lf ms, a time = %.4lf ms, matA time = %.4lf ms\n", i, memcpyTime_cuda_ia, memcpyTime_cuda_ja, memcpyTime_cuda_a, create_matA_Time);
			}			
		}
	}

	~COOArrays()
	{
		free(a);
		free(ia);
		free(ja);

		// destroy matrix/vector descriptors
		for(int i=0; i<num_streams; i++){
			gpuCudaErrorCheck(cudaFree(ia_d[i]));
			gpuCudaErrorCheck(cudaFree(ja_d[i]));
			gpuCudaErrorCheck(cudaFree(a_d[i]));
			gpuCudaErrorCheck(cudaFree(x_d[i]));
			// gpuCudaErrorCheck(cudaFree(y_d[i]));

			gpuCudaErrorCheck(cudaFreeHost(ia_h[i]));
			gpuCudaErrorCheck(cudaFreeHost(ja_h[i]));
			gpuCudaErrorCheck(cudaFreeHost(a_h[i]));
			gpuCudaErrorCheck(cudaFreeHost(x_h[i]));
			gpuCudaErrorCheck(cudaFreeHost(y_h[i]));

			gpuCudaErrorCheck(cudaStreamDestroy(stream[i]));

			gpuCudaErrorCheck(cudaEventDestroy(startEvent_execution[i]));
			gpuCudaErrorCheck(cudaEventDestroy(endEvent_execution[i]));

			gpuCusparseErrorCheck(cusparseDestroy(handle[i]));
			gpuCusparseErrorCheck(cusparseDestroySpMat(matA[i]));
			gpuCusparseErrorCheck(cusparseDestroyDnVec(vecX[i]));
			gpuCusparseErrorCheck(cusparseDestroyDnVec(vecY[i]));
			
			gpuCudaErrorCheck(cudaFree(dBuffer[i]));
		}
		gpuCudaErrorCheck(cudaFree(y_d2));
		gpuCudaErrorCheck(cudaFree(y_d_reduction));
		gpuCublasErrorCheck(cublasDestroy(handle_blas));

		if(TIME_IT){
			for(int i=0; i<num_streams; i++){
				gpuCudaErrorCheck(cudaEventDestroy(startEvent_memcpy_ia[i]));
				gpuCudaErrorCheck(cudaEventDestroy(endEvent_memcpy_ia[i]));
				gpuCudaErrorCheck(cudaEventDestroy(startEvent_memcpy_ja[i]));
				gpuCudaErrorCheck(cudaEventDestroy(endEvent_memcpy_ja[i]));
				gpuCudaErrorCheck(cudaEventDestroy(startEvent_memcpy_a[i]));
				gpuCudaErrorCheck(cudaEventDestroy(endEvent_memcpy_a[i]));

				gpuCudaErrorCheck(cudaEventDestroy(startEvent_create_matA[i]));
				gpuCudaErrorCheck(cudaEventDestroy(endEvent_create_matA[i]));
				gpuCudaErrorCheck(cudaEventDestroy(startEvent_spmv_buffersize[i]));
				gpuCudaErrorCheck(cudaEventDestroy(endEvent_spmv_buffersize[i]));

				gpuCudaErrorCheck(cudaEventDestroy(startEvent_create_vecX[i]));
				gpuCudaErrorCheck(cudaEventDestroy(endEvent_create_vecX[i]));
				gpuCudaErrorCheck(cudaEventDestroy(startEvent_create_vecY[i]));
				gpuCudaErrorCheck(cudaEventDestroy(endEvent_create_vecY[i]));
				gpuCudaErrorCheck(cudaEventDestroy(startEvent_memcpy_x[i]));
				gpuCudaErrorCheck(cudaEventDestroy(endEvent_memcpy_x[i]));
			}
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
	char *format_name;
	format_name = (char *)malloc(100*sizeof(char));
	snprintf(format_name, 100, "CUSPARSE_COO_STREAM_%d", coo->num_streams);
	coo->format_name = format_name;
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
		int offset = 0;
		for(int i=0; i<coo->num_streams; i++){
			memcpy(coo->x_h[i], x + offset, coo->n_stream[i] * sizeof(ValueType));
			offset += coo->n_stream[i];
		}

		for(int i=0; i<coo->num_streams; i++){
			if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(coo->startEvent_memcpy_x[i], coo->stream[i]));
			gpuCudaErrorCheck(cudaMemcpyAsync(coo->x_d[i], coo->x_h[i], coo->n_stream[i] * sizeof(*coo->x), cudaMemcpyHostToDevice, coo->stream[i]));
			if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(coo->endEvent_memcpy_x[i], coo->stream[i]));
		}

		for(int i=0; i<coo->num_streams; i++)
			gpuCudaErrorCheck(cudaStreamSynchronize(coo->stream[i]));

		if(TIME_IT){
			for(int i=0; i<coo->num_streams; i++){
				float memcpyTime_cuda;
				gpuCudaErrorCheck(cudaEventElapsedTime(&memcpyTime_cuda, coo->startEvent_memcpy_x[i], coo->endEvent_memcpy_x[i]));
				printf("(CUDA) (stream %d) Memcpy x time = %.4lf ms\n", i, memcpyTime_cuda);
			}
		}

		for(int i=0; i<coo->num_streams; i++){
			// Create dense vector X
			if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(coo->startEvent_create_vecX[i], coo->stream[i]));
			gpuCusparseErrorCheck(cusparseCreateDnVec(&coo->vecX[i], coo->n_stream[i], coo->x_d[i], ValueTypeCuda));
			if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(coo->endEvent_create_vecX[i], coo->stream[i]));

			// Create dense vector y
			if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(coo->startEvent_create_vecY[i], coo->stream[i]));
			// gpuCusparseErrorCheck(cusparseCreateDnVec(&coo->vecY[i], coo->m, coo->y_d[i], ValueTypeCuda));
			gpuCusparseErrorCheck(cusparseCreateDnVec(&coo->vecY[i], coo->m, coo->y_d2 + i*coo->m, ValueTypeCuda));
			if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(coo->endEvent_create_vecY[i], coo->stream[i]));

			// Allocate an external buffer if needed
			if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(coo->startEvent_spmv_buffersize[i], coo->stream[i]));
			gpuCusparseErrorCheck(cusparseSpMV_bufferSize(coo->handle[i], CUSPARSE_OPERATION_NON_TRANSPOSE, &alpha, coo->matA[i], coo->vecX[i], &beta, coo->vecY[i], ValueTypeCuda, CUSPARSE_SPMV_ALG_DEFAULT, &coo->bufferSize[i]));
			gpuCudaErrorCheck(cudaMalloc(&coo->dBuffer[i], coo->bufferSize[i]));
			if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(coo->endEvent_spmv_buffersize[i], coo->stream[i]));
			// printf("(stream %d) SpMV_bufferSize = %ld\n", i, coo->bufferSize[i]); // size of the workspace that is needed by cusparseSpMV()
		}

		if(TIME_IT){
			for(int i=0; i<coo->num_streams; i++){
				gpuCudaErrorCheck(cudaStreamSynchronize(coo->stream[i]));
				float create_vecX_time, create_vecY_time, spmv_buffersize_time;
				gpuCudaErrorCheck(cudaEventElapsedTime(&create_vecX_time, coo->startEvent_create_vecX[i], coo->endEvent_create_vecX[i]));
				gpuCudaErrorCheck(cudaEventElapsedTime(&create_vecY_time, coo->startEvent_create_vecY[i], coo->endEvent_create_vecY[i]));
				gpuCudaErrorCheck(cudaEventElapsedTime(&spmv_buffersize_time, coo->startEvent_spmv_buffersize[i], coo->endEvent_spmv_buffersize[i]));
				printf("(CUDA) (stream %d) Create vecX time = %.4lf ms, vecY time = %.4lf ms, spmv_buffersize time = %.4lf (SpMV_bufferSize = %zu)\n", i, create_vecX_time, create_vecY_time, spmv_buffersize_time, coo->bufferSize[i]);
			}
		}
	}

	for(int i=0; i<coo->num_streams; i++)
		gpuCudaErrorCheck(cudaEventRecord(coo->startEvent_execution[i], coo->stream[i]));

	int num_loops = 128;
	double time_execution = time_it(1,
		for(int k=0;k<num_loops;k++){
			for(int i=0; i<coo->num_streams; i++){
				gpuCusparseErrorCheck(cusparseSpMV(coo->handle[i], CUSPARSE_OPERATION_NON_TRANSPOSE, &alpha, coo->matA[i], coo->vecX[i], &beta, coo->vecY[i], ValueTypeCuda, CUSPARSE_SPMV_ALG_DEFAULT, coo->dBuffer[i]));
			}
			// gpuCudaErrorCheck(cudaPeekAtLastError());
			gpuCudaErrorCheck(cudaDeviceSynchronize());
		}
	);

	double gflops = coo->nnz / time_execution * num_loops * 2 * 1e-9;
	printf("(DGAL timing) Execution time = %.4lf ms (%.4lf GFLOPS %d streams for %.2lf MB workload)\n", time_execution*1e3, gflops, coo->num_streams, coo->mem_footprint/(1024*1024.0));

	for(int i=0; i<coo->num_streams; i++)
		gpuCudaErrorCheck(cudaEventRecord(coo->endEvent_execution[i], coo->stream[i]));

	for(int i=0; i<coo->num_streams; i++)
		gpuCudaErrorCheck(cudaStreamSynchronize(coo->stream[i]));

	if(TIME_IT){
		for(int i=0; i<coo->num_streams; i++){
			float executionTime_cuda;
			gpuCudaErrorCheck(cudaEventElapsedTime(&executionTime_cuda, coo->startEvent_execution[i], coo->endEvent_execution[i]));

			double gflops_cuda = coo->nnz_stream[i] / executionTime_cuda * num_loops * 2 * 1e-6;
			double mem_footprint = (coo->nnz_stream[i] * (sizeof(ValueType) + sizeof(INT_T)) + (coo->m+1) * sizeof(INT_T))/(1024*1024.0);
			printf("(CUDA) (stream %d) Execution time = %.4lf ms (%.4lf GFLOPS for %.2lf MB workload)\n", i, executionTime_cuda, gflops_cuda, mem_footprint);
		}
	}

	if (coo->y == NULL)
	{
		coo->y = y;

		// for(int i=0; i<coo->num_streams; i++){
		// 	if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(coo->startEvent_memcpy_y[i], coo->stream[i]));
		// 	gpuCudaErrorCheck(cudaMemcpyAsync(coo->y_h[i], coo->y_d[i], coo->m * sizeof(ValueType), cudaMemcpyDeviceToHost, coo->stream[i]));
		// 	if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(coo->endEvent_memcpy_y[i], coo->stream[i]));
		// }

		// if(TIME_IT){
		// 	for(int i=0; i<coo->num_streams; i++){
		// 		gpuCudaErrorCheck(cudaEventSynchronize(coo->endEvent_memcpy_y[i]));
		// 		float memcpyTime_cuda;
		// 		gpuCudaErrorCheck(cudaEventElapsedTime(&memcpyTime_cuda, coo->startEvent_memcpy_y[i], coo->endEvent_memcpy_y[i]));
		// 		printf("(CUDA) (stream %d) Memcpy y time = %.4lf ms\n", i, memcpyTime_cuda);
		// 	}
		// }

		// // for(int i=0; i<coo->num_streams; i++)
		// // 	memcpy(y, coo->y_h[i], coo->m * sizeof(ValueType));
		// double time_y_reduction = time_it(1,
		// 	for(int i=0; i<coo->m; i++){
		// 		coo->y_h_final[i] = 0;
		// 		for(int k=0; k<coo->num_streams; k++)
		// 			coo->y_h_final[i] += coo->y_h[k][i];
		// 	}
		// 	memcpy(y, coo->y_h_final, coo->m * sizeof(ValueType));
		// );
		// printf("time_y_reduction = %.4lf ms\n", time_y_reduction);

		if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(coo->startEvent_memcpy_y, coo->stream[0]));

		ValueType *ones_host, *ones_device;

		gpuCudaErrorCheck(cudaMallocHost(&ones_host, coo->num_streams * sizeof(ValueType)));
		for (int i=0; i<coo->num_streams; i++) ones_host[i] = 1.0;
		gpuCudaErrorCheck(cudaMalloc(&ones_device, coo->num_streams * sizeof(ValueType)));	
		gpuCudaErrorCheck(cudaMemcpyAsync(ones_device, ones_host, coo->num_streams * sizeof(ValueType), cudaMemcpyHostToDevice, coo->stream[0]));
	
		ValueType  alpha = 1.0, beta = 0.0;
		gpuCublasErrorCheck(cublasDgemv(coo->handle_blas, CUBLAS_OP_N, coo->m, coo->num_streams, &alpha, coo->y_d2, coo->m, ones_device, 1, &beta, coo->y_d_reduction, 1));

		gpuCudaErrorCheck(cudaPeekAtLastError());
		gpuCudaErrorCheck(cudaMemcpyAsync(coo->y, coo->y_d_reduction, coo->m * sizeof(coo->y), cudaMemcpyDeviceToHost, coo->stream[0]));

		gpuCudaErrorCheck(cudaFreeHost(ones_host));
		gpuCudaErrorCheck(cudaFree(ones_device));

		if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(coo->endEvent_memcpy_y, coo->stream[0]));
		gpuCudaErrorCheck(cudaStreamSynchronize(coo->stream[0]));
		if(TIME_IT){
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

