#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

#include <cuda.h>
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
	#include "aux/csr_util.h"
	#include "aux/csr_converter.h"
	#include "aux/csc_util.h"
	#include "aux/csc_converter.h"
#ifdef __cplusplus
}
#endif


extern int prefetch_distance;

double * thread_time_compute, * thread_time_barrier;

#ifndef NUM_THREADS
#define NUM_THREADS 1024
#endif

// int row_cluster_size = 256;
// int row_cluster_size = 192;
// int row_cluster_size = 128;
// int row_cluster_size = 64;
// int row_cluster_size = 32;
// int row_cluster_size = 16;
// int row_cluster_size = 8;
#ifndef ROW_CLUSTER_SIZE
#define ROW_CLUSTER_SIZE 4
#endif

#ifndef NUM_STREAMS
#define NUM_STREAMS 1
#endif

#ifndef TIME_IT
#define TIME_IT 0
#endif

INT_T * thread_block_i_s[NUM_STREAMS];
INT_T * thread_block_i_e[NUM_STREAMS];
unsigned char * rel_row_idx[NUM_STREAMS];

struct CSRArrays : Matrix_Format
{
	INT_T * ia;      // the usual rowptr (of size m+1)
	INT_T * ja;      // the colidx of each NNZ (of size nnz)
	ValueType * a;   // the values (of size NNZ)
	INT_T * ia_h[NUM_STREAMS];
	INT_T * ja_h[NUM_STREAMS];
	ValueType * a_h[NUM_STREAMS];
	INT_T * thread_block_i_s_h[NUM_STREAMS];
	INT_T * thread_block_i_e_h[NUM_STREAMS];
	unsigned char * rel_row_idx_h[NUM_STREAMS];

	INT_T * ia_d[NUM_STREAMS];
	INT_T * ja_d[NUM_STREAMS];
	ValueType * a_d[NUM_STREAMS];
	INT_T * thread_block_i_s_d[NUM_STREAMS];
	INT_T * thread_block_i_e_d[NUM_STREAMS];
	unsigned char * rel_row_idx_d[NUM_STREAMS];

	ValueType * x = NULL;
	ValueType * y = NULL;
	ValueType * x_h[NUM_STREAMS];
	ValueType * y_h[NUM_STREAMS];
	ValueType * x_d[NUM_STREAMS];
	// ValueType * y_d[NUM_STREAMS];
	ValueType * y_d2;
	ValueType * y_d_reduction;

	cudaStream_t stream[NUM_STREAMS];
	INT_T n_stream[NUM_STREAMS];
	INT_T nnz_stream[NUM_STREAMS];

	// cudaEvent_t is useful for timing, but for performance use " cudaEventCreateWithFlags ( &event, cudaEventDisableTiming) "
	cudaEvent_t startEvent_execution[NUM_STREAMS];
	cudaEvent_t endEvent_execution[NUM_STREAMS];
	
	cudaEvent_t startEvent_memcpy_ia[NUM_STREAMS];
	cudaEvent_t endEvent_memcpy_ia[NUM_STREAMS];
	cudaEvent_t startEvent_memcpy_ja[NUM_STREAMS];
	cudaEvent_t endEvent_memcpy_ja[NUM_STREAMS];
	cudaEvent_t startEvent_memcpy_a[NUM_STREAMS];
	cudaEvent_t endEvent_memcpy_a[NUM_STREAMS];
	cudaEvent_t startEvent_memcpy_thread_block_i_s[NUM_STREAMS];
	cudaEvent_t endEvent_memcpy_thread_block_i_s[NUM_STREAMS];
	cudaEvent_t startEvent_memcpy_thread_block_i_e[NUM_STREAMS];
	cudaEvent_t endEvent_memcpy_thread_block_i_e[NUM_STREAMS];
	cudaEvent_t startEvent_memcpy_rel_row_idx[NUM_STREAMS];
	cudaEvent_t endEvent_memcpy_rel_row_idx[NUM_STREAMS];

	cudaEvent_t startEvent_memcpy_x[NUM_STREAMS];
	cudaEvent_t endEvent_memcpy_x[NUM_STREAMS];
	cudaEvent_t startEvent_memcpy_y;
	cudaEvent_t endEvent_memcpy_y;

	cublasHandle_t handle;

	int max_smem_per_block, multiproc_count, max_threads_per_block, warp_size, max_threads_per_multiproc;
	int num_threads;
	int block_size;
	int num_blocks;

	int row_cluster_size;

	int num_streams;

	CSRArrays(INT_T * ia, INT_T * ja, ValueType * a, long m, long n, long nnz) : Matrix_Format(m, n, nnz), ia(ia), ja(ja), a(a)
	{
		double time_balance;

		gpuCudaErrorCheck(cudaDeviceGetAttribute(&max_smem_per_block, cudaDevAttrMaxSharedMemoryPerBlock, 0));
		gpuCudaErrorCheck(cudaDeviceGetAttribute(&multiproc_count, cudaDevAttrMultiProcessorCount, 0));
		gpuCudaErrorCheck(cudaDeviceGetAttribute(&max_threads_per_block, cudaDevAttrMaxThreadsPerBlock , 0));
		gpuCudaErrorCheck(cudaDeviceGetAttribute(&warp_size, cudaDevAttrWarpSize , 0));
		gpuCudaErrorCheck(cudaDeviceGetAttribute(&max_threads_per_multiproc, cudaDevAttrMaxThreadsPerMultiProcessor, 0));
		printf("max_smem_per_block=%d\n", max_smem_per_block);
		printf("multiproc_count=%d\n", multiproc_count);
		printf("max_threads_per_block=%d\n", max_threads_per_block);
		printf("warp_size=%d\n", warp_size);
		printf("max_threads_per_multiproc=%d\n", max_threads_per_multiproc);

		// block_size = warp_size / 2;
		block_size = warp_size;
		// block_size = warp_size * 2;
		// block_size = warp_size * 4;
		row_cluster_size = ROW_CLUSTER_SIZE;

		// num_threads = 128;
		// num_threads = 1ULL << 10;
		// num_threads = 3584;
		// num_threads = 1ULL << 12;
		// num_threads = 1ULL << 13;
		// num_threads = 1ULL << 14;
		// num_threads = 1ULL << 15;
		// num_threads = 1ULL << 16;
		// num_threads = 1ULL << 17;
		// num_threads = 1ULL << 21;
		// num_threads = 1ULL << 22;
		num_threads = NUM_THREADS;

		num_threads = ((num_threads + block_size - 1) / block_size) * block_size;

		num_blocks = num_threads / block_size;

		printf("num_threads=%d, block_size=%d, num_blocks=%d\n", num_threads, block_size, num_blocks);

		num_streams = NUM_STREAMS;

		/********************************************************************************************************/
		printf("/********************************************************************************************************/\n");
		// Convert CSR representation ton CSC
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
		time_balance = time_it(1,
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
			// REMOVE THIS
			/*
			if(0){
				char * replace_str;
				replace_str = (char *)malloc(100*sizeof(char));
				sprintf(replace_str, "stream_%d", i);
				char * file_fig = fig_name_gen("matrix.mtx", replace_str);
				printf("file_fig = %s\n", file_fig);

				long num_pixels = 4096;
				long num_pixels_x = (n_stream[i] < num_pixels) ? n_stream[i] : num_pixels;
				long num_pixels_y = (m < num_pixels) ? m : num_pixels;
				if(m!=n_stream[i]) {
					double ratio = n_stream[i]*1.0 / m;
					// if((ratio>16.0) || (ratio<(1/16.0)))
					if(ratio>16.0)
						ratio=16.0;
					if(ratio < (1/16.0))
						ratio=1/16.0;
					// in order to keep both below 1024
					if(ratio>1) // n > m
						num_pixels_y = (1/ratio) * num_pixels_x;
					else // m > n
						num_pixels_x = ratio * num_pixels_y;
				}
				csr_plot(file_fig, row_ptr_stream[i], col_idx_stream[i], val_stream[i], m, n_stream[i], nnz_stream[i], 0, num_pixels_x, num_pixels_y);
			}
			*/

			free(col_indices);
		}

		for(int i=0; i<num_streams; i++){
			free(row_idx_stream[i]);
			free(col_ptr_stream[i]);
			free(val_c_stream[i]);
		}
		free(row_idx);
		free(col_ptr);
		free(val_c);

		printf("/********************************************************************************************************/\n");
		/********************************************************************************************************/

		for(int i=0; i<num_streams; i++){
			thread_block_i_s[i] = (INT_T *) malloc(num_blocks * sizeof(INT_T));
			thread_block_i_e[i] = (INT_T *) malloc(num_blocks * sizeof(INT_T));
			for (int j=0; j<num_blocks; j++)
			{
				// loop_partitioner_balance_iterations(num_blocks, i, 0, m, &thread_block_i_s[i], &thread_block_i_e[i]);
				loop_partitioner_balance_prefix_sums(num_blocks, j, row_ptr_stream[i], m, nnz_stream[i], &(thread_block_i_s[i][j]), &(thread_block_i_e[i][j]));
			}

			rel_row_idx[i] = (unsigned char *) malloc(nnz_stream[i] * sizeof(unsigned char));
			#pragma omp parallel
			{
				long ii, i_s, i_e, i_rel, j, k;
				#pragma omp for
				for (k=0;k<num_blocks;k++)
				{
					i_s = thread_block_i_s[i][k];
					i_e = thread_block_i_e[i][k];
					for (ii=i_s;ii<i_e;ii++)
					{
						i_rel = (ii - i_s) % row_cluster_size;
						for (j=row_ptr_stream[i][ii];j<row_ptr_stream[i][ii+1];j++)
						{
							rel_row_idx[i][j] = i_rel;
						}
					}
				}
			}
		}

		for(int i=0; i<num_streams; i++){
			gpuCudaErrorCheck(cudaMalloc(&ia_d[i], (m+1) * sizeof(INT_T)));
			gpuCudaErrorCheck(cudaMalloc(&ja_d[i], nnz_stream[i] * sizeof(INT_T)));
			gpuCudaErrorCheck(cudaMalloc(&a_d[i], nnz_stream[i] * sizeof(ValueType)));
			gpuCudaErrorCheck(cudaMalloc(&x_d[i], n_stream[i] * sizeof(ValueType)));
			// gpuCudaErrorCheck(cudaMalloc(&y_d[i], m * sizeof(ValueType)));
			gpuCudaErrorCheck(cudaMalloc(&thread_block_i_s_d[i], num_blocks * sizeof(INT_T)));
			gpuCudaErrorCheck(cudaMalloc(&thread_block_i_e_d[i], num_blocks * sizeof(INT_T)));
			gpuCudaErrorCheck(cudaMalloc(&rel_row_idx_d[i], nnz_stream[i] * sizeof(unsigned char)));
		}
		gpuCudaErrorCheck(cudaMalloc(&y_d2, m * num_streams * sizeof(ValueType)));
		gpuCudaErrorCheck(cudaMalloc(&y_d_reduction, m * sizeof(ValueType)));
		gpuCublasErrorCheck(cublasCreate(&handle));

		for(int i=0; i<num_streams; i++){
			gpuCudaErrorCheck(cudaMallocHost(&ia_h[i], (m+1) * sizeof(INT_T)));
			gpuCudaErrorCheck(cudaMallocHost(&ja_h[i], nnz_stream[i] * sizeof(INT_T)));
			gpuCudaErrorCheck(cudaMallocHost(&a_h[i], nnz_stream[i] * sizeof(ValueType)));
			gpuCudaErrorCheck(cudaMallocHost(&x_h[i], n_stream[i] * sizeof(ValueType)));
			gpuCudaErrorCheck(cudaMallocHost(&y_h[i], m * sizeof(ValueType)));
			gpuCudaErrorCheck(cudaMallocHost(&thread_block_i_s_h[i], num_blocks * sizeof(INT_T)));
			gpuCudaErrorCheck(cudaMallocHost(&thread_block_i_e_h[i], num_blocks * sizeof(INT_T)));
			gpuCudaErrorCheck(cudaMallocHost(&rel_row_idx_h[i], nnz_stream[i] * sizeof(unsigned char)));
		}

		double time_memcpy = time_it(1,
		for(int i=0; i<num_streams; i++){
			memcpy(ia_h[i], row_ptr_stream[i], (m + 1) * sizeof(INT_T));
			memcpy(ja_h[i], col_idx_stream[i], nnz_stream[i] * sizeof(INT_T));
			memcpy(a_h[i], val_stream[i], nnz_stream[i] * sizeof(ValueType));
			memcpy(thread_block_i_s_h[i], thread_block_i_s[i], num_blocks * sizeof(INT_T));
			memcpy(thread_block_i_e_h[i], thread_block_i_e[i], num_blocks * sizeof(INT_T));
			memcpy(rel_row_idx_h[i], rel_row_idx[i], nnz_stream[i] * sizeof(unsigned char));
		}
		);
		printf("time_memcpy (ia_h, ja_h, a_h, thr_i_s, thr_i_e) = %lf ms\n", time_memcpy*1e3);

		// cuda events for timing measurements
		for(int i=0; i<num_streams; i++){
			gpuCudaErrorCheck(cudaStreamCreate(&stream[i]));

			gpuCudaErrorCheck(cudaEventCreate(&startEvent_execution[i]));
			gpuCudaErrorCheck(cudaEventCreate(&endEvent_execution[i]));
		}
		gpuCublasErrorCheck(cublasSetStream(handle, stream[0]));

		if(TIME_IT){
			for(int i=0; i<num_streams; i++){
				gpuCudaErrorCheck(cudaEventCreate(&startEvent_memcpy_ia[i]));
				gpuCudaErrorCheck(cudaEventCreate(&endEvent_memcpy_ia[i]));
				gpuCudaErrorCheck(cudaEventCreate(&startEvent_memcpy_ja[i]));
				gpuCudaErrorCheck(cudaEventCreate(&endEvent_memcpy_ja[i]));
				gpuCudaErrorCheck(cudaEventCreate(&startEvent_memcpy_a[i]));
				gpuCudaErrorCheck(cudaEventCreate(&endEvent_memcpy_a[i]));
				gpuCudaErrorCheck(cudaEventCreate(&startEvent_memcpy_rel_row_idx[i]));
				gpuCudaErrorCheck(cudaEventCreate(&endEvent_memcpy_rel_row_idx[i]));
				gpuCudaErrorCheck(cudaEventCreate(&startEvent_memcpy_thread_block_i_s[i]));
				gpuCudaErrorCheck(cudaEventCreate(&endEvent_memcpy_thread_block_i_s[i]));
				gpuCudaErrorCheck(cudaEventCreate(&startEvent_memcpy_thread_block_i_e[i]));
				gpuCudaErrorCheck(cudaEventCreate(&endEvent_memcpy_thread_block_i_e[i]));

				gpuCudaErrorCheck(cudaEventCreate(&startEvent_memcpy_x[i]));
				gpuCudaErrorCheck(cudaEventCreate(&endEvent_memcpy_x[i]));
			}
			gpuCudaErrorCheck(cudaEventCreate(&startEvent_memcpy_y));
			gpuCudaErrorCheck(cudaEventCreate(&endEvent_memcpy_y));
		}

		for(int i=0; i<num_streams; i++){
			if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(startEvent_memcpy_ia[i], stream[i]));
			gpuCudaErrorCheck(cudaMemcpyAsync(ia_d[i], ia_h[i], (m+1) * sizeof(INT_T), cudaMemcpyHostToDevice, stream[i]));
			if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(endEvent_memcpy_ia[i], stream[i]));
			
			if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(startEvent_memcpy_ja[i], stream[i]));
			gpuCudaErrorCheck(cudaMemcpyAsync(ja_d[i], ja_h[i], nnz_stream[i] * sizeof(INT_T), cudaMemcpyHostToDevice, stream[i]));
			if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(endEvent_memcpy_ja[i], stream[i]));
			
			if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(startEvent_memcpy_a[i], stream[i]));
			gpuCudaErrorCheck(cudaMemcpyAsync(a_d[i], a_h[i], nnz_stream[i] * sizeof(ValueType), cudaMemcpyHostToDevice, stream[i]));
			if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(endEvent_memcpy_a[i], stream[i]));
			
			if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(startEvent_memcpy_thread_block_i_s[i], stream[i]));
			gpuCudaErrorCheck(cudaMemcpyAsync(thread_block_i_s_d[i], thread_block_i_s_h[i], num_blocks * sizeof(INT_T), cudaMemcpyHostToDevice, stream[i]));
			if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(endEvent_memcpy_thread_block_i_s[i], stream[i]));
			
			if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(startEvent_memcpy_thread_block_i_e[i], stream[i]));
			gpuCudaErrorCheck(cudaMemcpyAsync(thread_block_i_e_d[i], thread_block_i_e_h[i], num_blocks * sizeof(INT_T), cudaMemcpyHostToDevice, stream[i]));
			if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(endEvent_memcpy_thread_block_i_e[i], stream[i]));

			if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(startEvent_memcpy_rel_row_idx[i], stream[i]));
			gpuCudaErrorCheck(cudaMemcpyAsync(rel_row_idx_d[i], rel_row_idx_h[i], nnz_stream[i] * sizeof(unsigned char), cudaMemcpyHostToDevice, stream[i]));
			if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(endEvent_memcpy_rel_row_idx[i], stream[i]));
		}

		if(TIME_IT){
			for(int i=0; i<num_streams; i++){
				gpuCudaErrorCheck(cudaStreamSynchronize(stream[i]));
				float memcpyTime_cuda_ia, memcpyTime_cuda_ja, memcpyTime_cuda_a, memcpyTime_cuda_rel_row_idx, memcpyTime_cuda_thread_block_i_s, memcpyTime_cuda_thread_block_i_e;
				gpuCudaErrorCheck(cudaEventElapsedTime(&memcpyTime_cuda_ia, startEvent_memcpy_ia[i], endEvent_memcpy_ia[i]));
				gpuCudaErrorCheck(cudaEventElapsedTime(&memcpyTime_cuda_ja, startEvent_memcpy_ja[i], endEvent_memcpy_ja[i]));
				gpuCudaErrorCheck(cudaEventElapsedTime(&memcpyTime_cuda_a, startEvent_memcpy_a[i], endEvent_memcpy_a[i]));
				gpuCudaErrorCheck(cudaEventElapsedTime(&memcpyTime_cuda_rel_row_idx, startEvent_memcpy_rel_row_idx[i], endEvent_memcpy_rel_row_idx[i]));
				gpuCudaErrorCheck(cudaEventElapsedTime(&memcpyTime_cuda_thread_block_i_s, startEvent_memcpy_thread_block_i_s[i], endEvent_memcpy_thread_block_i_s[i]));
				gpuCudaErrorCheck(cudaEventElapsedTime(&memcpyTime_cuda_thread_block_i_e, startEvent_memcpy_thread_block_i_e[i], endEvent_memcpy_thread_block_i_e[i]));
				printf("(CUDA) (stream %d) Memcpy ia time = %.4lf ms, ja time = %.4lf ms, a time = %.4lf ms, rel_row_idx time = %.4lf ms, thread_block_s = %.4lf ms, thread_block_e = %.4lf ms\n", i, memcpyTime_cuda_ia, memcpyTime_cuda_ja, memcpyTime_cuda_a, memcpyTime_cuda_rel_row_idx, memcpyTime_cuda_thread_block_i_s, memcpyTime_cuda_thread_block_i_e);
			}
		}
	}

	~CSRArrays()
	{
		free(a);
		free(ia);
		free(ja);
		for(int i=0; i<num_streams; i++){
			free(thread_block_i_s[i]);
			free(thread_block_i_e[i]);
			free(rel_row_idx[i]);
		}

		for(int i=0; i<num_streams; i++){
			gpuCudaErrorCheck(cudaFree(ia_d[i]));
			gpuCudaErrorCheck(cudaFree(ja_d[i]));
			gpuCudaErrorCheck(cudaFree(a_d[i]));
			gpuCudaErrorCheck(cudaFree(x_d[i]));
			// gpuCudaErrorCheck(cudaFree(y_d[i]));
			gpuCudaErrorCheck(cudaFree(thread_block_i_s_d[i]));
			gpuCudaErrorCheck(cudaFree(thread_block_i_e_d[i]));
			gpuCudaErrorCheck(cudaFree(rel_row_idx_d[i]));

			gpuCudaErrorCheck(cudaFreeHost(ia_h[i]));
			gpuCudaErrorCheck(cudaFreeHost(ja_h[i]));
			gpuCudaErrorCheck(cudaFreeHost(a_h[i]));
			gpuCudaErrorCheck(cudaFreeHost(x_h[i]));
			gpuCudaErrorCheck(cudaFreeHost(y_h[i]));
			gpuCudaErrorCheck(cudaFreeHost(thread_block_i_s_h[i]));
			gpuCudaErrorCheck(cudaFreeHost(thread_block_i_e_h[i]));
			gpuCudaErrorCheck(cudaFreeHost(rel_row_idx_h[i]));

			gpuCudaErrorCheck(cudaStreamDestroy(stream[i]));

			gpuCudaErrorCheck(cudaEventDestroy(startEvent_execution[i]));
			gpuCudaErrorCheck(cudaEventDestroy(endEvent_execution[i]));
		}
		gpuCudaErrorCheck(cudaFree(y_d2));
		gpuCudaErrorCheck(cudaFree(y_d_reduction));
		gpuCublasErrorCheck(cublasDestroy(handle));

		if(TIME_IT){
			for(int i=0; i<num_streams; i++){
				gpuCudaErrorCheck(cudaEventDestroy(startEvent_memcpy_x[i]));
				gpuCudaErrorCheck(cudaEventDestroy(endEvent_memcpy_x[i]));
				gpuCudaErrorCheck(cudaEventDestroy(startEvent_memcpy_thread_block_i_s[i]));
				gpuCudaErrorCheck(cudaEventDestroy(endEvent_memcpy_thread_block_i_s[i]));
				gpuCudaErrorCheck(cudaEventDestroy(startEvent_memcpy_thread_block_i_e[i]));
				gpuCudaErrorCheck(cudaEventDestroy(endEvent_memcpy_thread_block_i_e[i]));
				gpuCudaErrorCheck(cudaEventDestroy(startEvent_memcpy_rel_row_idx[i]));
				gpuCudaErrorCheck(cudaEventDestroy(endEvent_memcpy_rel_row_idx[i]));

				gpuCudaErrorCheck(cudaEventDestroy(startEvent_memcpy_ia[i]));
				gpuCudaErrorCheck(cudaEventDestroy(endEvent_memcpy_ia[i]));
				gpuCudaErrorCheck(cudaEventDestroy(startEvent_memcpy_ja[i]));
				gpuCudaErrorCheck(cudaEventDestroy(endEvent_memcpy_ja[i]));
				gpuCudaErrorCheck(cudaEventDestroy(startEvent_memcpy_a[i]));
				gpuCudaErrorCheck(cudaEventDestroy(endEvent_memcpy_a[i]));
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
	char *format_name;
	format_name = (char *)malloc(100*sizeof(char));
	snprintf(format_name, 100, "Custom_CSR_CUDA_BUFFER_s%d_t%d_rc_%d", csr->num_streams, csr->num_threads, csr->row_cluster_size);
	csr->format_name = format_name;
	return csr;
}


//==========================================================================================================================================
//= CSR Custom
//==========================================================================================================================================


__global__ void gpu_kernel_csr_basic(INT_T * thread_block_i_s, INT_T * thread_block_i_e, INT_T * ia, INT_T * ja, ValueType * a, ValueType * restrict x, ValueType * restrict y, unsigned char * rel_row_idx, int row_cluster_size)
{
	extern __shared__ ValueType sdata[];
	int tidg = cuda_get_thread_num();
	int tidb = threadIdx.x;
	int block_id = blockIdx.x;
	int block_size = blockDim.x;
	long i, i_s, i_e, i_rel, i_rel_e, j, j_s, j_e, k;
	ValueType sum;
	for (i=0;i<row_cluster_size;i++)
		sdata[i*block_size + tidb] = 0;
	__syncthreads();
	i_s = thread_block_i_s[block_id];
	i_e = thread_block_i_e[block_id];
	// printf("%d,%d: bs=%d , bid=%d , %ld %ld\n", tidg, tidb, block_size, block_id, i_s, i_e);
	for (k=i_s;k<i_e;k+=row_cluster_size)
	{
		i_rel_e = k + row_cluster_size > i_e ? i_e - k : row_cluster_size;
		j_s = ia[k];
		j_e = ia[k+i_rel_e];
		for (j=j_s+tidb;j<j_e;j+=block_size)
		{
			i_rel = rel_row_idx[j];
			sdata[i_rel*block_size + tidb] += a[j] * x[ja[j]];
		}

		__syncthreads();

		for (i_rel=tidb;i_rel<i_rel_e;i_rel+=block_size)
		{
			sum = 0;
			for (j=0;j<block_size;j++)
			{
				sum += sdata[i_rel*block_size + j];
				sdata[i_rel*block_size + j] = 0;
			}
			y[k + i_rel] = sum;
		}

		// for (i_rel=0;i_rel<i_rel_e;i_rel++)
		// {
			// sum = 0;
			// for (j=1;j<block_size;j*=2)
			// {
				// if (tidb % (2*j) == 0)
				// {
					// sdata[i_rel*block_size + tidb] += sdata[i_rel*block_size + tidb + j];
					// sdata[i_rel*block_size + tidb + j] = 0;
				// }
				// __syncthreads();
			// }
			// if (tidb == 0)
			// {
				// y[k + i_rel] = sdata[i_rel*block_size];
				// sdata[i_rel*block_size] = 0;
			// }
		// }

		__syncthreads();
	}
}


void
compute_csr(CSRArrays * restrict csr, ValueType * restrict x, ValueType * restrict y)
{
	int block_size = csr->block_size;
	int num_blocks = csr->num_blocks;
	dim3 block_dims(block_size);
	dim3 grid_dims(num_blocks);
	// printf("Grid : {%d, %d, %d} blocks. Blocks : {%d, %d, %d} threads.\n", grid_dims.x, grid_dims.y, grid_dims.z, block_dims.x, block_dims.y, block_dims.z);
	// long shared_mem_size = block_size * sizeof(*C_d);

	if (csr->x == NULL)
	{
		csr->x = x;
		int offset = 0;
		for(int i=0; i<csr->num_streams; i++){
			memcpy(csr->x_h[i], x + offset, csr->n_stream[i] * sizeof(ValueType));
			offset += csr->n_stream[i];
		}

		for(int i=0; i<csr->num_streams; i++){
			if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(csr->startEvent_memcpy_x[i], csr->stream[i]));
			gpuCudaErrorCheck(cudaMemcpyAsync(csr->x_d[i], csr->x_h[i], csr->n_stream[i] * sizeof(ValueType), cudaMemcpyHostToDevice, csr->stream[i]));
			if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(csr->endEvent_memcpy_x[i], csr->stream[i]));
		}

		for(int i=0; i<csr->num_streams; i++)
			gpuCudaErrorCheck(cudaStreamSynchronize(csr->stream[i]));

		if(TIME_IT){
			for(int i=0; i<csr->num_streams; i++){
				float memcpyTime_cuda;
				gpuCudaErrorCheck(cudaEventElapsedTime(&memcpyTime_cuda, csr->startEvent_memcpy_x[i], csr->endEvent_memcpy_x[i]));
				printf("(CUDA) (stream %d) Memcpy x time = %.4lf ms\n", i, memcpyTime_cuda);
			}
		}
	}

	for(int i=0; i<csr->num_streams; i++){
		gpu_kernel_csr_basic<<<grid_dims, block_dims, (csr->row_cluster_size*block_size*sizeof(ValueType)), csr->stream[i]>>>(csr->thread_block_i_s_d[i], csr->thread_block_i_e_d[i], csr->ia_d[i], csr->ja_d[i], csr->a_d[i], csr->x_d[i], csr->y_d2 + i*csr->m, csr->rel_row_idx_d[i], csr->row_cluster_size);
	}

	gpuCudaErrorCheck(cudaPeekAtLastError());
	for(int i=0; i<csr->num_streams; i++)
		gpuCudaErrorCheck(cudaStreamSynchronize(csr->stream[i]));

	if (csr->y == NULL)
	{
		csr->y = y;

		if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(csr->startEvent_memcpy_y, csr->stream[0]));

		ValueType *ones_host, *ones_device;
		gpuCudaErrorCheck(cudaMallocHost(&ones_host, csr->num_streams * sizeof(ValueType)));
		for (int i=0; i<csr->num_streams; i++) ones_host[i] = 1.0;
		gpuCudaErrorCheck(cudaMalloc(&ones_device, csr->num_streams * sizeof(ValueType)));	
		gpuCudaErrorCheck(cudaMemcpyAsync(ones_device, ones_host, csr->num_streams * sizeof(ValueType), cudaMemcpyHostToDevice, csr->stream[0]));
	
		ValueType  alpha = 1.0, beta = 0.0;
		gpuCublasErrorCheck(cublasDgemv(csr->handle, CUBLAS_OP_N, csr->m, csr->num_streams, &alpha, csr->y_d2, csr->m, ones_device, 1, &beta, csr->y_d_reduction, 1));

		gpuCudaErrorCheck(cudaPeekAtLastError());
		gpuCudaErrorCheck(cudaMemcpyAsync(csr->y, csr->y_d_reduction, csr->m * sizeof(csr->y), cudaMemcpyDeviceToHost, csr->stream[0]));

		gpuCudaErrorCheck(cudaFreeHost(ones_host));
		gpuCudaErrorCheck(cudaFree(ones_device));

		if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(csr->endEvent_memcpy_y, csr->stream[0]));
		gpuCudaErrorCheck(cudaStreamSynchronize(csr->stream[0]));
		if(TIME_IT){
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

