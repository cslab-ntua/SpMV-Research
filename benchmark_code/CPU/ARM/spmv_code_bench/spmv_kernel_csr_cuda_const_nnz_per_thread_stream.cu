#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

#include <cuda.h>
#include <cooperative_groups.h>
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

using namespace cooperative_groups;

#ifndef NNZ_PER_THREAD
#define NNZ_PER_THREAD  6
#endif

#ifndef BLOCK_SIZE
// #define BLOCK_SIZE  32
// #define BLOCK_SIZE  64
// #define BLOCK_SIZE  128
// #define BLOCK_SIZE  256
// #define BLOCK_SIZE  512
#define BLOCK_SIZE  1024
#endif

#ifndef NUM_STREAMS
#define NUM_STREAMS 1
#endif

#ifndef TIME_IT
#define TIME_IT 0
#endif

#ifndef TIME_IT2
#define TIME_IT2 0
#endif


struct CSRArrays : Matrix_Format
{
	INT_T * row_ptr;      // the usual rowptr (of size m+1)
	INT_T * ia[NUM_STREAMS];      // the usual rowptr (of size m+1)
	INT_T * ja;      // the colidx of each NNZ (of size nnz)
	ValueType * a;   // the values (of size NNZ)

	INT_T * thread_block_i_s[NUM_STREAMS];
	INT_T * thread_block_i_e[NUM_STREAMS];
	INT_T * thread_block_j_s[NUM_STREAMS];
	INT_T * thread_block_j_e[NUM_STREAMS];
	INT_T * row_ptr_stream[NUM_STREAMS];

	INT_T * row_ptr_h[NUM_STREAMS];
	INT_T * ia_h[NUM_STREAMS];
	INT_T * ja_h[NUM_STREAMS];
	ValueType * a_h[NUM_STREAMS];
	INT_T * thread_block_i_s_h[NUM_STREAMS];
	INT_T * thread_block_i_e_h[NUM_STREAMS];
	INT_T * thread_block_j_s_h[NUM_STREAMS];
	INT_T * thread_block_j_e_h[NUM_STREAMS];

	INT_T * row_ptr_d[NUM_STREAMS];
	INT_T * ia_d[NUM_STREAMS];
	INT_T * ja_d[NUM_STREAMS];
	ValueType * a_d[NUM_STREAMS];
	INT_T * thread_block_i_s_d[NUM_STREAMS];
	INT_T * thread_block_i_e_d[NUM_STREAMS];
	INT_T * thread_block_j_s_d[NUM_STREAMS];
	INT_T * thread_block_j_e_d[NUM_STREAMS];

	ValueType * x = NULL;
	ValueType * y = NULL;
	ValueType * x_h[NUM_STREAMS];
	ValueType * y_h[NUM_STREAMS];
	// ValueType * y_h2;
	// ValueType * y_h_final;
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
	float execution_time[NUM_STREAMS];
	int iterations;
	
	cudaEvent_t startEvent_memcpy_row_ptr[NUM_STREAMS];
	cudaEvent_t endEvent_memcpy_row_ptr[NUM_STREAMS];
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
	cudaEvent_t startEvent_memcpy_thread_block_j_s[NUM_STREAMS];
	cudaEvent_t endEvent_memcpy_thread_block_j_s[NUM_STREAMS];
	cudaEvent_t startEvent_memcpy_thread_block_j_e[NUM_STREAMS];
	cudaEvent_t endEvent_memcpy_thread_block_j_e[NUM_STREAMS];
	cudaEvent_t startEvent_memcpy_x[NUM_STREAMS];
	cudaEvent_t endEvent_memcpy_x[NUM_STREAMS];
	cudaEvent_t startEvent_memcpy_y;
	cudaEvent_t endEvent_memcpy_y;

	cublasHandle_t handle;

	int max_smem_per_block, multiproc_count, max_threads_per_block, warp_size, max_threads_per_multiproc, max_block_dim_x, max_num_threads;
	int nnz_per_thread;
	int num_threads[NUM_STREAMS];
	int block_size;
	int num_blocks[NUM_STREAMS];
	int num_streams;

	CSRArrays(INT_T * row_ptr, INT_T * ja, ValueType * a, long m, long n, long nnz) : Matrix_Format(m, n, nnz), row_ptr(row_ptr), ja(ja), a(a)
	{
		gpuCudaErrorCheck(cudaDeviceGetAttribute(&max_smem_per_block, cudaDevAttrMaxSharedMemoryPerBlock, 0));
		gpuCudaErrorCheck(cudaDeviceGetAttribute(&multiproc_count, cudaDevAttrMultiProcessorCount, 0));
		gpuCudaErrorCheck(cudaDeviceGetAttribute(&max_threads_per_block, cudaDevAttrMaxThreadsPerBlock , 0));
		gpuCudaErrorCheck(cudaDeviceGetAttribute(&warp_size, cudaDevAttrWarpSize , 0));
		gpuCudaErrorCheck(cudaDeviceGetAttribute(&max_threads_per_multiproc, cudaDevAttrMaxThreadsPerMultiProcessor, 0));
		gpuCudaErrorCheck(cudaDeviceGetAttribute(&max_block_dim_x, cudaDevAttrMaxBlockDimX, 0));
		max_num_threads = max_threads_per_multiproc * multiproc_count;
		printf("max_smem_per_block(bytes)=%d\n", max_smem_per_block);
		printf("multiproc_count=%d\n", multiproc_count);
		printf("max_threads_per_block=%d\n", max_threads_per_block);
		printf("warp_size=%d\n", warp_size);
		printf("max_threads_per_multiproc=%d\n", max_threads_per_multiproc);
		printf("max_block_dim_x=%d\n", max_block_dim_x);
		printf("max_num_threads=%d\n", max_num_threads);

		block_size = BLOCK_SIZE;
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
			csr_row_indices(row_ptr, ja, m, n, nnz, &row_indices);
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
			printf("local_stream[%d] = %d - %d (%d cols) (%d nnz)\n", i, local_stream_j_s[i], local_stream_j_e[i], n_stream[i], nnz_stream[i]);

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
			free(row_idx_stream[i]);
			free(col_ptr_stream[i]);
			free(val_c_stream[i]);
		}
		free(row_idx);
		free(col_ptr);
		free(val_c);

		// for(int i=0; i<num_streams; i++)
		// 	printf("Stream %d: %d columns, %d nnz\n", i, n_stream[i], nnz_stream[i]);

		printf("/********************************************************************************************************/\n");
		/********************************************************************************************************/


		for(int i=0; i<num_streams; i++){
			num_threads[i] = (nnz_stream[i] + NNZ_PER_THREAD - 1) / NNZ_PER_THREAD;

			num_threads[i] = ((num_threads[i] + BLOCK_SIZE - 1) / BLOCK_SIZE) * BLOCK_SIZE;

			num_blocks[i] = num_threads[i] / BLOCK_SIZE;

			printf("Stream %d: %d columns, %d nnz\tnum_threads=%d, block_size=%d, num_blocks=%d\n", i, n_stream[i], nnz_stream[i], num_threads[i], BLOCK_SIZE, num_blocks[i]);
			thread_block_i_s[i] = (INT_T *) malloc(num_blocks[i] * sizeof(INT_T));
			thread_block_i_e[i] = (INT_T *) malloc(num_blocks[i] * sizeof(INT_T));
			thread_block_j_s[i] = (INT_T *) malloc(num_blocks[i] * sizeof(INT_T));
			thread_block_j_e[i] = (INT_T *) malloc(num_blocks[i] * sizeof(INT_T));
			// double time_balance = time_it(1,
			long lower_boundary;
			// for (i=0;i<num_blocks;i++)
			// {
				// loop_partitioner_balance_iterations(num_blocks, i, 0, nnz, &thread_block_j_s[i], &thread_block_j_e[i]);
				// macros_binary_search(row_ptr, 0, m, thread_block_j_s[i], &lower_boundary, NULL);           // Index boundaries are inclusive.
				// thread_block_i_s[i] = lower_boundary;
			// }
			long nnz_per_block = block_size * NNZ_PER_THREAD;
			for (int k=0;k<num_blocks[i];k++)
			{
				thread_block_j_s[i][k] = nnz_per_block * k;
				thread_block_j_e[i][k] = nnz_per_block * (k+ 1);
				if (thread_block_j_s[i][k] > nnz_stream[i])
					thread_block_j_s[i][k] = nnz_stream[i];
				if (thread_block_j_e[i][k] > nnz_stream[i])
					thread_block_j_e[i][k] = nnz_stream[i];
				macros_binary_search(row_ptr_stream[i], 0, m, thread_block_j_s[i][k], &lower_boundary, NULL);           // Index boundaries are inclusive.
				thread_block_i_s[i][k] = lower_boundary;
			}
			for (int k=0;k<num_blocks[i];k++)
			{
				if (k == num_blocks[i] - 1)   // If we calculate each thread's boundaries individually some empty rows might be unassigned.
					thread_block_i_e[i][k] = m;
				else
					thread_block_i_e[i][k] = thread_block_i_s[i][k+1] + 1;
				if ((thread_block_j_s[i][k] >= row_ptr_stream[i][thread_block_i_e[i][k]]) || (thread_block_j_s[i][k] < row_ptr_stream[i][thread_block_i_s[i][k]]))
					error("bad binary search of row start: i=%d j:[%d, %d] j=%d", thread_block_i_s[i][k], row_ptr_stream[i][thread_block_i_s[i][k]], row_ptr_stream[i][thread_block_i_e[i][k]], thread_block_j_s[i][k]);
			}
			// );
			// printf("Stream %d: balance time = %g\n", i, time_balance);

			ia[i] = (INT_T*) malloc(nnz_stream[i] * sizeof(INT_T));
			_Pragma("omp parallel")
			{
				long k, j;
				_Pragma("omp for")
				for (k=0;k<m;k++)
				{
					for (j=row_ptr_stream[i][k];j<row_ptr_stream[i][k+1];j++)
					{
						ia[i][j] = k;
					}
				}
			}

			_Pragma("omp parallel")
			{
				long k, j;
				_Pragma("omp for")
				for (j=0;j<nnz_stream[i];j+=32*NNZ_PER_THREAD)
				{
					long j_e = j + 32*NNZ_PER_THREAD;
					if (j_e > nnz_stream[i])
						j_e = nnz_stream[i];
					if (ia[i][j] == ia[i][j_e-1])
					{
						for (k=j;k<j_e;k++)
						{
							col_idx_stream[i][k] = col_idx_stream[i][k] | 0x80000000;
						}
					}
				}
			}
		}

		for(int i=0; i<num_streams; i++){
			gpuCudaErrorCheck(cudaMalloc(&row_ptr_d[i], (m+1) * sizeof(INT_T)));
			gpuCudaErrorCheck(cudaMalloc(&ia_d[i], nnz_stream[i] * sizeof(INT_T)));
			gpuCudaErrorCheck(cudaMalloc(&ja_d[i], nnz_stream[i] * sizeof(INT_T)));
			gpuCudaErrorCheck(cudaMalloc(&a_d[i], nnz_stream[i] * sizeof(ValueType)));
			gpuCudaErrorCheck(cudaMalloc(&thread_block_i_s_d[i], num_blocks[i] * sizeof(INT_T)));
			gpuCudaErrorCheck(cudaMalloc(&thread_block_i_e_d[i], num_blocks[i] * sizeof(INT_T)));
			gpuCudaErrorCheck(cudaMalloc(&thread_block_j_s_d[i], num_blocks[i] * sizeof(INT_T)));
			gpuCudaErrorCheck(cudaMalloc(&thread_block_j_e_d[i], num_blocks[i] * sizeof(INT_T)));
			gpuCudaErrorCheck(cudaMalloc(&x_d[i], n_stream[i] * sizeof(ValueType)));
			// gpuCudaErrorCheck(cudaMalloc(&y_d[i], m * sizeof(ValueType)));
		}
		gpuCudaErrorCheck(cudaMalloc(&y_d2, m * num_streams * sizeof(ValueType)));
		gpuCudaErrorCheck(cudaMalloc(&y_d_reduction, m * sizeof(ValueType)));
		gpuCublasErrorCheck(cublasCreate(&handle));

		for(int i=0; i<num_streams; i++){
			gpuCudaErrorCheck(cudaMallocHost(&row_ptr_h[i], (m+1) * sizeof(INT_T)));
			gpuCudaErrorCheck(cudaMallocHost(&ia_h[i], nnz_stream[i] * sizeof(INT_T)));
			gpuCudaErrorCheck(cudaMallocHost(&ja_h[i], nnz_stream[i] * sizeof(INT_T)));
			gpuCudaErrorCheck(cudaMallocHost(&a_h[i], nnz_stream[i] * sizeof(ValueType)));
			gpuCudaErrorCheck(cudaMallocHost(&thread_block_i_s_h[i], num_blocks[i] * sizeof(INT_T)));
			gpuCudaErrorCheck(cudaMallocHost(&thread_block_i_e_h[i], num_blocks[i] * sizeof(INT_T)));
			gpuCudaErrorCheck(cudaMallocHost(&thread_block_j_s_h[i], num_blocks[i] * sizeof(INT_T)));
			gpuCudaErrorCheck(cudaMallocHost(&thread_block_j_e_h[i], num_blocks[i] * sizeof(INT_T)));
			gpuCudaErrorCheck(cudaMallocHost(&x_h[i], n_stream[i] * sizeof(ValueType)));
			gpuCudaErrorCheck(cudaMallocHost(&y_h[i], m * sizeof(ValueType)));
		}
		// gpuCudaErrorCheck(cudaMallocHost(&y_h2, m * num_streams * sizeof(ValueType)));
		// gpuCudaErrorCheck(cudaMallocHost(&y_h_final, m * sizeof(ValueType)));

		double time_memcpy = time_it(1,
		for(int i=0; i<num_streams; i++){
			memcpy(row_ptr_h[i], row_ptr_stream[i], (m + 1) * sizeof(INT_T));
			memcpy(ia_h[i], ia[i], nnz_stream[i] * sizeof(INT_T));
			memcpy(ja_h[i], col_idx_stream[i], nnz_stream[i] * sizeof(INT_T));
			memcpy(a_h[i], val_stream[i], nnz_stream[i] * sizeof(ValueType));
			memcpy(thread_block_i_s_h[i], thread_block_i_s[i], num_blocks[i] * sizeof(INT_T));
			memcpy(thread_block_i_e_h[i], thread_block_i_e[i], num_blocks[i] * sizeof(INT_T));
			memcpy(thread_block_j_s_h[i], thread_block_j_s[i], num_blocks[i] * sizeof(INT_T));
			memcpy(thread_block_j_e_h[i], thread_block_j_e[i], num_blocks[i] * sizeof(INT_T));
		}
		);
		printf("time_memcpy (row_ptr_h, ia_h, ja_h, a_h) = %lf ms\n", time_memcpy*1e3);

		// cuda events for timing measurements
		for(int i=0; i<num_streams; i++){
			gpuCudaErrorCheck(cudaStreamCreate(&stream[i]));

			gpuCudaErrorCheck(cudaEventCreate(&startEvent_execution[i]));
			gpuCudaErrorCheck(cudaEventCreate(&endEvent_execution[i]));
		}
		iterations=0;
		gpuCublasErrorCheck(cublasSetStream(handle, stream[0]));

		if(TIME_IT){
			for(int i=0; i<num_streams; i++){
				gpuCudaErrorCheck(cudaEventCreate(&startEvent_memcpy_row_ptr[i]));
				gpuCudaErrorCheck(cudaEventCreate(&endEvent_memcpy_row_ptr[i]));
				gpuCudaErrorCheck(cudaEventCreate(&startEvent_memcpy_ia[i]));
				gpuCudaErrorCheck(cudaEventCreate(&endEvent_memcpy_ia[i]));
				gpuCudaErrorCheck(cudaEventCreate(&startEvent_memcpy_ja[i]));
				gpuCudaErrorCheck(cudaEventCreate(&endEvent_memcpy_ja[i]));
				gpuCudaErrorCheck(cudaEventCreate(&startEvent_memcpy_a[i]));
				gpuCudaErrorCheck(cudaEventCreate(&endEvent_memcpy_a[i]));
				gpuCudaErrorCheck(cudaEventCreate(&startEvent_memcpy_thread_block_i_s[i]));
				gpuCudaErrorCheck(cudaEventCreate(&endEvent_memcpy_thread_block_i_s[i]));
				gpuCudaErrorCheck(cudaEventCreate(&startEvent_memcpy_thread_block_i_e[i]));
				gpuCudaErrorCheck(cudaEventCreate(&endEvent_memcpy_thread_block_i_e[i]));
				gpuCudaErrorCheck(cudaEventCreate(&startEvent_memcpy_thread_block_j_s[i]));
				gpuCudaErrorCheck(cudaEventCreate(&endEvent_memcpy_thread_block_j_s[i]));
				gpuCudaErrorCheck(cudaEventCreate(&startEvent_memcpy_thread_block_j_e[i]));
				gpuCudaErrorCheck(cudaEventCreate(&endEvent_memcpy_thread_block_j_e[i]));

				gpuCudaErrorCheck(cudaEventCreate(&startEvent_memcpy_x[i]));
				gpuCudaErrorCheck(cudaEventCreate(&endEvent_memcpy_x[i]));
			}
			gpuCudaErrorCheck(cudaEventCreate(&startEvent_memcpy_y));
			gpuCudaErrorCheck(cudaEventCreate(&endEvent_memcpy_y));
		}

		for(int i=0; i<num_streams; i++){
			if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(startEvent_memcpy_row_ptr[i], stream[i]));
			gpuCudaErrorCheck(cudaMemcpyAsync(row_ptr_d[i], row_ptr_h[i], (m+1) * sizeof(INT_T), cudaMemcpyHostToDevice, stream[i]));
			if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(endEvent_memcpy_row_ptr[i], stream[i]));

			if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(startEvent_memcpy_ia[i]));
			gpuCudaErrorCheck(cudaMemcpyAsync(ia_d[i], ia_h[i], nnz_stream[i] * sizeof(INT_T), cudaMemcpyHostToDevice, stream[i]));
			if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(endEvent_memcpy_ia[i], stream[i]));

			if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(startEvent_memcpy_ja[i]));
			gpuCudaErrorCheck(cudaMemcpyAsync(ja_d[i], ja_h[i], nnz_stream[i] * sizeof(INT_T), cudaMemcpyHostToDevice, stream[i]));
			if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(endEvent_memcpy_ja[i], stream[i]));

			if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(startEvent_memcpy_a[i]));
			gpuCudaErrorCheck(cudaMemcpyAsync(a_d[i], a_h[i], nnz_stream[i] * sizeof(ValueType), cudaMemcpyHostToDevice, stream[i]));
			if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(endEvent_memcpy_a[i], stream[i]));

			if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(startEvent_memcpy_thread_block_i_s[i]));
			gpuCudaErrorCheck(cudaMemcpyAsync(thread_block_i_s_d[i], thread_block_i_s_h[i], num_blocks[i] * sizeof(INT_T), cudaMemcpyHostToDevice, stream[i]));
			if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(endEvent_memcpy_thread_block_i_s[i], stream[i]));

			if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(startEvent_memcpy_thread_block_i_e[i]));
			gpuCudaErrorCheck(cudaMemcpyAsync(thread_block_i_e_d[i], thread_block_i_e_h[i], num_blocks[i] * sizeof(INT_T), cudaMemcpyHostToDevice, stream[i]));
			if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(endEvent_memcpy_thread_block_i_e[i], stream[i]));

			if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(startEvent_memcpy_thread_block_j_s[i]));
			gpuCudaErrorCheck(cudaMemcpyAsync(thread_block_j_s_d[i], thread_block_j_s_h[i], num_blocks[i] * sizeof(INT_T), cudaMemcpyHostToDevice, stream[i]));
			if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(endEvent_memcpy_thread_block_j_s[i], stream[i]));

			if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(startEvent_memcpy_thread_block_j_e[i]));
			gpuCudaErrorCheck(cudaMemcpyAsync(thread_block_j_e_d[i], thread_block_j_e_h[i], num_blocks[i] * sizeof(INT_T), cudaMemcpyHostToDevice, stream[i]));
			if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(endEvent_memcpy_thread_block_j_e[i], stream[i]));
		}

		if(TIME_IT){
			for(int i=0; i<num_streams; i++){
				gpuCudaErrorCheck(cudaStreamSynchronize(stream[i]));
				float memcpyTime_cuda_row_ptr, memcpyTime_cuda_ia, memcpyTime_cuda_ja, memcpyTime_cuda_a, memcpyTime_cuda_thread_block_i_s, memcpyTime_cuda_thread_block_i_e, memcpyTime_cuda_thread_block_j_s, memcpyTime_cuda_thread_block_j_e;
				gpuCudaErrorCheck(cudaEventElapsedTime(&memcpyTime_cuda_row_ptr, startEvent_memcpy_row_ptr[i], endEvent_memcpy_row_ptr[i]));
				gpuCudaErrorCheck(cudaEventElapsedTime(&memcpyTime_cuda_ia, startEvent_memcpy_ia[i], endEvent_memcpy_ia[i]));
				gpuCudaErrorCheck(cudaEventElapsedTime(&memcpyTime_cuda_ja, startEvent_memcpy_ja[i], endEvent_memcpy_ja[i]));
				gpuCudaErrorCheck(cudaEventElapsedTime(&memcpyTime_cuda_a, startEvent_memcpy_a[i], endEvent_memcpy_a[i]));
				gpuCudaErrorCheck(cudaEventElapsedTime(&memcpyTime_cuda_thread_block_i_s, startEvent_memcpy_thread_block_i_s[i], endEvent_memcpy_thread_block_i_s[i]));
				gpuCudaErrorCheck(cudaEventElapsedTime(&memcpyTime_cuda_thread_block_i_e, startEvent_memcpy_thread_block_i_e[i], endEvent_memcpy_thread_block_i_e[i]));
				gpuCudaErrorCheck(cudaEventElapsedTime(&memcpyTime_cuda_thread_block_j_s, startEvent_memcpy_thread_block_j_s[i], endEvent_memcpy_thread_block_j_s[i]));
				gpuCudaErrorCheck(cudaEventElapsedTime(&memcpyTime_cuda_thread_block_j_e, startEvent_memcpy_thread_block_j_e[i], endEvent_memcpy_thread_block_j_e[i]));
				printf("(CUDA) (stream %d) Memcpy row_ptr time = %.4lf ms, ia time = %.4lf ms, ja time = %.4lf ms, a time = %.4lf ms, thread_block_i_s time = %.4lf, thread_block_i_e time = %.4lf, thread_block_j_s time = %.4lf, thread_block_j_e time = %.4lf\n", i, memcpyTime_cuda_row_ptr, memcpyTime_cuda_ia, memcpyTime_cuda_ja, memcpyTime_cuda_a, memcpyTime_cuda_thread_block_i_s, memcpyTime_cuda_thread_block_i_e, memcpyTime_cuda_thread_block_j_s, memcpyTime_cuda_thread_block_j_e);
			}
		}
		for(int i=0; i<num_streams; i++){
			_Pragma("omp parallel")
			{
				long j;
				_Pragma("omp for")
				for (j=0;j<nnz_stream[i];j++)
				{
					ja_h[i][j] = ja_h[i][j] & 0x7FFFFFFF;
				}
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
			free(thread_block_j_s[i]);
			free(thread_block_j_e[i]);

			gpuCudaErrorCheck(cudaFree(ia_d[i]));
			gpuCudaErrorCheck(cudaFree(row_ptr_d[i]));
			gpuCudaErrorCheck(cudaFree(ja_d[i]));
			gpuCudaErrorCheck(cudaFree(a_d[i]));
			gpuCudaErrorCheck(cudaFree(x_d[i]));
			// gpuCudaErrorCheck(cudaFree(y_d[i]));
			gpuCudaErrorCheck(cudaFree(thread_block_i_s_d[i]));
			gpuCudaErrorCheck(cudaFree(thread_block_i_e_d[i]));
			gpuCudaErrorCheck(cudaFree(thread_block_j_s_d[i]));
			gpuCudaErrorCheck(cudaFree(thread_block_j_e_d[i]));

			gpuCudaErrorCheck(cudaFreeHost(ia_h[i]));
			gpuCudaErrorCheck(cudaFreeHost(row_ptr_h[i]));
			gpuCudaErrorCheck(cudaFreeHost(ja_h[i]));
			gpuCudaErrorCheck(cudaFreeHost(a_h[i]));
			gpuCudaErrorCheck(cudaFreeHost(x_h[i]));
			gpuCudaErrorCheck(cudaFreeHost(y_h[i]));
			gpuCudaErrorCheck(cudaFreeHost(thread_block_i_s_h[i]));
			gpuCudaErrorCheck(cudaFreeHost(thread_block_i_e_h[i]));
			gpuCudaErrorCheck(cudaFreeHost(thread_block_j_s_h[i]));
			gpuCudaErrorCheck(cudaFreeHost(thread_block_j_e_h[i]));

			gpuCudaErrorCheck(cudaStreamDestroy(stream[i]));

			gpuCudaErrorCheck(cudaEventDestroy(startEvent_execution[i]));
			gpuCudaErrorCheck(cudaEventDestroy(endEvent_execution[i]));
		}
		gpuCudaErrorCheck(cudaFree(y_d2));
		gpuCudaErrorCheck(cudaFree(y_d_reduction));
		gpuCublasErrorCheck(cublasDestroy(handle));
		// gpuCudaErrorCheck(cudaFreeHost(y_h2));
		// gpuCudaErrorCheck(cudaFreeHost(y_h_final));

		if(TIME_IT){
			for(int i=0; i<num_streams; i++){
				gpuCudaErrorCheck(cudaEventDestroy(startEvent_memcpy_x[i]));
				gpuCudaErrorCheck(cudaEventDestroy(endEvent_memcpy_x[i]));

				gpuCudaErrorCheck(cudaEventDestroy(startEvent_memcpy_row_ptr[i]));
				gpuCudaErrorCheck(cudaEventDestroy(endEvent_memcpy_row_ptr[i]));
				gpuCudaErrorCheck(cudaEventDestroy(startEvent_memcpy_ia[i]));
				gpuCudaErrorCheck(cudaEventDestroy(endEvent_memcpy_ia[i]));
				gpuCudaErrorCheck(cudaEventDestroy(startEvent_memcpy_ja[i]));
				gpuCudaErrorCheck(cudaEventDestroy(endEvent_memcpy_ja[i]));
				gpuCudaErrorCheck(cudaEventDestroy(startEvent_memcpy_a[i]));
				gpuCudaErrorCheck(cudaEventDestroy(endEvent_memcpy_a[i]));
				gpuCudaErrorCheck(cudaEventDestroy(startEvent_memcpy_thread_block_i_s[i]));
				gpuCudaErrorCheck(cudaEventDestroy(endEvent_memcpy_thread_block_i_s[i]));
				gpuCudaErrorCheck(cudaEventDestroy(startEvent_memcpy_thread_block_i_e[i]));
				gpuCudaErrorCheck(cudaEventDestroy(endEvent_memcpy_thread_block_i_e[i]));
				gpuCudaErrorCheck(cudaEventDestroy(startEvent_memcpy_thread_block_j_s[i]));
				gpuCudaErrorCheck(cudaEventDestroy(endEvent_memcpy_thread_block_j_s[i]));
				gpuCudaErrorCheck(cudaEventDestroy(startEvent_memcpy_thread_block_j_e[i]));
				gpuCudaErrorCheck(cudaEventDestroy(endEvent_memcpy_thread_block_j_e[i]));
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
	snprintf(format_name, 100, "Custom_CSR_CUDA_constant_nnz_per_thread_b%d_nnz%d_s%d", BLOCK_SIZE, NNZ_PER_THREAD, csr->num_streams);
	csr->format_name = format_name;
	/*if(0){
		for(int i=0; i<csr->num_streams; i++){
			char matrix_part[100];
			sprintf(matrix_part, "Stream%d", i);
			csr_matrix_features_validation(matrix_part, csr->row_ptr_h[i], csr->ja_h[i], csr->m, csr->n_stream[i], csr->nnz_stream[i]);
			char file_fig[100];
			sprintf(file_fig, "figures/Stream%d", i);
			long num_pixels = 4096;
			long num_pixels_x = (csr->n_stream[i] < num_pixels) ? csr->n_stream[i] : num_pixels;
			long num_pixels_y = (csr->m < num_pixels) ? csr->m : num_pixels;
			if(csr->m!=csr->n_stream[i]) {
				double ratio = csr->n_stream[i]*1.0 / csr->m;
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
				printf("Stream %d: ratio %lf\n", i, ratio);
			}

			csr_plot(file_fig, csr->row_ptr_h[i], csr->ja_h[i], csr->a_h[i], csr->m, csr->n_stream[i], csr->nnz_stream[i], 0, num_pixels_x, num_pixels_y);
		}
	}*/
	return csr;
}


//==========================================================================================================================================
//= CSR Custom
//==========================================================================================================================================


/* inline
__device__ void reduce_block(INT_T * ia_buf, ValueType * val_buf, ValueType * restrict y)
{
	const int tidb = threadIdx.x;
	int row = ia_buf[tidb];
	int k;
	for (k=1;k<BLOCK_SIZE;k*=2)
	{
		if ((tidb & (2*k-1)) == k-1)
		{
			ValueType val = val_buf[tidb];
			if (row == ia_buf[tidb+k])
			{
				val_buf[tidb+k] += val;
				// val_buf[tidb] = 0;
			}
			else
			{
				atomicAdd(&y[row], val);
				// y[row] += val;
			}
		}
		__syncthreads();
	}
	if (tidb == 0)
		atomicAdd(&y[ia_buf[BLOCK_SIZE-1]], val_buf[BLOCK_SIZE-1]);
} */


/* inline
__device__ void reduce_block(INT_T * ia_buf, ValueType * val_buf, ValueType * restrict y)
{
	const int tidb = threadIdx.x;
	int k;
	INT_T row = ia_buf[tidb];
	for (k=1;k<BLOCK_SIZE;k*=2)
	{
		if ((tidb & (2*k-1)) == 0)
		{
			INT_T row_next = ia_buf[tidb+k];
			ValueType val_next = val_buf[tidb+k];
			if (row == row_next)
			{
				val_buf[tidb] += val_next;
			}
			else
			{
				atomicAdd(&y[row], val_buf[tidb]);
				val_buf[tidb] = val_next;
				ia_buf[tidb] = row_next;
			}
		}
		__syncthreads();
	}
	if (tidb == 0)
		atomicAdd(&y[ia_buf[0]], val_buf[0]);
} */


/* template <typename group_t>
__device__ void reduce_warp(group_t g, INT_T * ia_buf, ValueType * val_buf, ValueType * restrict y)
{
	const int tidl = g.thread_rank();   // Group lane.
	int row = ia_buf[tidl];
	ValueType val;
	int k;
	#pragma unroll
	for (k=1;k<g.size();k*=2)
	{
		// val = val_buf[tidl];
		// if ((tidl & (2*k-1)) == k-1)
		// {
			// if (tidl >= k && row == ia_buf[tidl-k])
			// {
				// val_buf[tidl-k] += val;
				// val = 0;
			// }
		// }
		// g.sync();
		// if ((tidl & (2*k-1)) == k-1 && val != 0)
		// {
			// if (row == ia_buf[tidl+k])
			// {
				// val_buf[tidl+k] += val;
			// }
			// else
			// {
				// atomicAdd(&y[row], val);
			// }
		// }
		// g.sync();
		val = val_buf[tidl];
		if ((tidl & (2*k-1)) == k-1)
		{
			if (row == ia_buf[tidl+k])
			{
				val_buf[tidl+k] += val;
			}
			else
			{
				atomicAdd(&y[row], val);
			}
		}
		g.sync();
	}
}
inline
__device__ void reduce_block(INT_T * ia_buf, ValueType * val_buf, ValueType * restrict y)
{
	const int tidb = threadIdx.x;
	const int tidb_div = tidb / 32;
	const int tidb_mod = tidb % 32;
	thread_block_tile<32> tile32 = tiled_partition<32>(this_thread_block());
	reduce_warp(tile32, &ia_buf[tidb_div*32], &val_buf[tidb_div*32], y);
	// __syncthreads();
	// if (tidb_mod == 31)
	// {
		// ia_buf[tidb_mod] = ia_buf[tidb];
		// val_buf[tidb_mod] = val_buf[tidb];
	// }
	// __syncthreads();
	// if (tidb_div == 0)
		// reduce_warp(tile32, ia_buf, val_buf, y);
	// if (tidb == 0)
		// atomicAdd(&y[ia_buf[31]], val_buf[31]);
	if (tidb_mod == 31)
		atomicAdd(&y[ia_buf[tidb]], val_buf[tidb]);
} */


/* template <typename group_t>
__device__ void reduce_warp(group_t g, INT_T * row_ptr, ValueType * val_ptr, ValueType * restrict y)
{
	const int tidl = g.thread_rank();   // Group lane.
	INT_T row = *row_ptr;
	ValueType val = *val_ptr;
	int k;
	g.sync();
	#pragma unroll
	for (k=1;k<g.size();k*=2)
	{
		INT_T row_next;
		ValueType val_next;
		row_next = __shfl_sync(0xffffffff, row, tidl+k);
		val_next = __shfl_sync(0xffffffff, val, tidl+k);
		if ((tidl & (2*k-1)) == 0)
		{
			if (row == row_next)
			{
				val += val_next;
			}
			else
			{
				atomicAdd(&y[row], val);
				val = val_next;
				row = row_next;
			}
		}
		g.sync();
	}
	*row_ptr = row;
	*val_ptr = val;
	// if (tidl == 0)
		// atomicAdd(&y[row], val);
}
inline
__device__ void reduce_block(INT_T row, ValueType val, ValueType * restrict y)
{
	const int tidb = threadIdx.x;
	const int tidb_div = tidb / 32;
	const int tidb_mod = tidb % 32;
	thread_block_tile<32> tile32 = tiled_partition<32>(this_thread_block());
	reduce_warp(tile32, &row, &val, y);
	if (tidb_mod == 0)
		atomicAdd(&y[row], val);
	// extern __shared__ char sm[];
	// ValueType * val_buf = (typeof(val_buf)) sm;
	// INT_T * ia_buf = (typeof(ia_buf)) &sm[32 * sizeof(ValueType)];
	// if (tidb_mod == 0)
	// {
		// ia_buf[tidb_div] = row;
		// val_buf[tidb_div] = val;
	// }
	// __syncthreads();
	// if (tidb_div == 0)
	// {
		// row = ia_buf[tidb];
		// val = val_buf[tidb];
		// reduce_warp(tile32, &row, &val, y);
	// }
	// if (tidb == 0)
		// atomicAdd(&y[row], val);
} */


/* template <typename group_t>
__device__ void reduce_warp(group_t g, INT_T row, ValueType val, ValueType * restrict y)
{
	const int tidl = g.thread_rank();   // Group lane.
	int k;
	g.sync();
	#pragma unroll
	for (k=1;k<g.size();k*=2)
	{
		INT_T row_prev;
		ValueType val_prev;
		row_prev = __shfl_sync(0xffffffff, row, tidl-k);
		val_prev = __shfl_sync(0xffffffff, val, tidl-k);
		if ((tidl & (2*k-1)) == 2*k-1)
		{
			if (row == row_prev)
			{
				val += val_prev;
			}
			else
			{
				atomicAdd(&y[row_prev], val_prev);
			}
		}
		g.sync();
	}
	if (tidl == 31)
		atomicAdd(&y[row], val);
}
inline
__device__ void reduce_block(INT_T row, ValueType val, ValueType * restrict y)
{
	thread_block_tile<32> tile32 = tiled_partition<32>(this_thread_block());
	reduce_warp(tile32, row, val, y);
} */


// Threads may only read data from another thread which is actively participating in the __shfl_sync() command.
// If the target thread is inactive, the retrieved value is undefined.
template <typename group_t>
__device__ void reduce_warp(group_t g, INT_T row, ValueType val, ValueType * restrict y)
{
	const int tidl = g.thread_rank();   // Group lane.
	const int tidl_one_hot = 1 << tidl;
	int flag;
	INT_T row_prev;
	ValueType val_prev;
	flag = 0xaaaaaaaa; // 10101010101010101010101010101010
	row_prev = g.shfl_up(row, 1); // __shfl_sync(flag, row, tidl-1);
	val_prev = g.shfl_up(val, 1); // __shfl_sync(flag, val, tidl-1);
	if (tidl_one_hot & flag)
	{
		if (row == row_prev)
		{
			val += val_prev;
		}
		else
		{
			atomicAdd(&y[row_prev], val_prev);
		}
	}
	flag = 0x88888888; // 10001000100010001000100010001000
	row_prev = g.shfl_up(row, 2); // __shfl_sync(flag, row, tidl-2);
	val_prev = g.shfl_up(val, 2); // __shfl_sync(flag, val, tidl-2);
	if (tidl_one_hot & flag)
	{
		if (row == row_prev)
		{
			val += val_prev;
		}
		else
		{
			atomicAdd(&y[row_prev], val_prev);
		}
	}
	flag = 0x80808080; // 10000000100000001000000010000000
	row_prev = g.shfl_up(row, 4); // __shfl_sync(flag, row, tidl-4);
	val_prev = g.shfl_up(val, 4); // __shfl_sync(flag, val, tidl-4);
	if (tidl_one_hot & flag)
	{
		if (row == row_prev)
		{
			val += val_prev;
		}
		else
		{
			atomicAdd(&y[row_prev], val_prev);
		}
	}
	flag = 0x80008000; // 10000000000000001000000000000000
	row_prev = g.shfl_up(row, 8); // __shfl_sync(flag, row, tidl-8);
	val_prev = g.shfl_up(val, 8); // __shfl_sync(flag, val, tidl-8);
	if (tidl_one_hot & flag)
	{
		if (row == row_prev)
		{
			val += val_prev;
		}
		else
		{
			atomicAdd(&y[row_prev], val_prev);
		}
	}
	flag = 0x80000000; // 10000000000000000000000000000000
	row_prev = g.shfl_up(row, 16); // __shfl_sync(flag, row, tidl-16);
	val_prev = g.shfl_up(val, 16); // __shfl_sync(flag, val, tidl-16);
	if (tidl_one_hot & flag)
	{
		if (row == row_prev)
		{
			val += val_prev;
		}
		else
		{
			atomicAdd(&y[row_prev], val_prev);
		}
	}
	g.sync();
	if (tidl == 31)
		atomicAdd(&y[row], val);
}
inline
__device__ void reduce_block(INT_T row, ValueType val, ValueType * restrict y)
{
	thread_block_tile<32> tile32 = tiled_partition<32>(this_thread_block());
	reduce_warp(tile32, row, val, y);
}


__device__ void spmv_last_block(INT_T * thread_block_i_s, INT_T * thread_block_i_e, INT_T * thread_block_j_s, INT_T * thread_block_j_e, INT_T * row_ptr, INT_T * ia, INT_T * ja, ValueType * a, long m, long n, long nnz, ValueType * restrict x, ValueType * restrict y)
{
	// extern __shared__ char sm[];
	const int tidb = threadIdx.x;
	const int block_id = blockIdx.x;
	const int nnz_per_block = BLOCK_SIZE * NNZ_PER_THREAD;
	// ValueType * val_buf = (typeof(val_buf)) sm;
	// INT_T * ia_buf = (typeof(ia_buf)) &sm[BLOCK_SIZE * sizeof(ValueType)];
	[[gnu::unused]] int i, i_s, i_e, j, j_s, j_e, k, l, p;
	i_s = thread_block_i_s[block_id];
	i_e = thread_block_i_e[block_id];
	j_s = block_id * nnz_per_block + tidb * NNZ_PER_THREAD;
	j_e = j_s + NNZ_PER_THREAD;
	if (j_e > nnz)
		j_e = nnz;
	k = (i_e + i_s) / 2;
	while (i_s < i_e)
	{
		if (j_s >= row_ptr[k])
		{
			i_s = k + 1;
		}
		else
		{
			i_e = k;
		}
		k = (i_e + i_s) / 2;
	}
	i = i_s - 1;
	double sum = 0;
	int ptr_next = row_ptr[i+1];
	for (j=j_s;j<j_e;j++)
	{
		if (j >= ptr_next)
		{
			atomicAdd(&y[i], sum);
			sum = 0;
			while (j >= ptr_next)
			{
				i++;
				ptr_next = row_ptr[i+1];
			}
		}
		// sum += a[j] * x[ja[j] & 0x7FFFFFFF];
		sum = __fma_rn(a[j], x[ja[j] & 0x7FFFFFFF], sum);
	}
	reduce_block(i, sum, y);
}


template <typename group_t>
__device__ ValueType reduce_warp_single_line(group_t g, ValueType val, ValueType * restrict y)
{
	// Use XOR mode to perform butterfly reduction
	for (int i=g.size()/2; i>=1; i/=2)
	{
		val += g.shfl_xor(val, i); // __shfl_xor_sync(0xffffffff, val, i, g.size());   // 'sum' is same on all threads
		// val += __shfl_down_sync(0xffffffff, val, i, g.size());   // Only thread 0 has the total sum.
	}
	return val;
}


template <typename group_t>
__device__ void spmv_warp_single_row(group_t g, int i, int j_s, int j_e, INT_T * ja, ValueType * a, ValueType * restrict x, ValueType * restrict y)
{
	const int tidl = g.thread_rank();   // Group lane.
	int j;
	double sum = 0;
	for (j=j_s;j<j_e;j++)
	{
		sum = __fma_rn(a[j], x[ja[j] & 0x7FFFFFFF], sum);
	}
	sum = reduce_warp_single_line(g, sum, y);
	if (tidl == 0)
		atomicAdd(&y[i], sum);
}


template <typename group_t>
__device__ void spmv_full_warp(group_t g, int one_line, int i_s, int j_s, int j_e, INT_T * row_ptr, INT_T * ja, ValueType * a, ValueType * restrict x, ValueType * restrict y)
{
	[[gnu::unused]] int i, j, k, l, p;
	int ptr_next;
	i = i_s;
	ptr_next = row_ptr[i_s+1];
	for (j=j_s;j<j_e;j++)   // Find the row of the last nnz.
	{
		if (j >= ptr_next)
		{
			i++;
			break;
		}
	}
	double sum = 0;
	// int i_w_s, i_w_e;
	// i_w_s = __shfl_sync(0xffffffff, i_s, 0);
	// i_w_e = __shfl_sync(0xffffffff, i, 31);
	i = i_s;
	// if (i_w_e != i_w_s)
	if (one_line)
	{
		spmv_warp_single_row(g, i_s, j_s, j_e, ja, a, x, y);
	}
	else
	{
		ptr_next = row_ptr[i+1];
		k = 0;
		for (j=j_s;j<j_e;j++)
		{
			if (j >= ptr_next)
			{
				atomicAdd(&y[i], sum);
				sum = 0;
				while (j >= ptr_next)
				{
					i++;
					ptr_next = row_ptr[i+1];
				}
				k++;
			}
			// sum += a[j] * x[ja[j] & 0x7FFFFFFF];
			sum = __fma_rn(a[j], x[ja[j] & 0x7FFFFFFF], sum);
		}
		reduce_warp(g, i, sum, y);
	}
}


__device__ void spmv_full_block(INT_T * thread_block_i_s, INT_T * thread_block_i_e, INT_T * row_ptr, INT_T * ia, INT_T * ja, ValueType * a, long m, long n, long nnz, ValueType * restrict x, ValueType * restrict y)
{
	// extern __shared__ char sm[];
	// const int tidb = threadIdx.x;
	const int tidw = threadIdx.x % 32;
	const int warp_id = threadIdx.x / 32;
	const int block_id = blockIdx.x;
	const int nnz_per_block = BLOCK_SIZE * NNZ_PER_THREAD;
	// ValueType * val_buf = (typeof(val_buf)) sm;
	// INT_T * ia_buf = (typeof(ia_buf)) &sm[BLOCK_SIZE * sizeof(ValueType)];
	[[gnu::unused]] int i_s, i_e, j, j_s, j_e, j_w_s, k, l, p;
	i_s = thread_block_i_s[block_id];
	i_e = thread_block_i_e[block_id];
	// i_s = 0;
	// i_e = m;
	j_w_s = block_id * nnz_per_block + warp_id * NNZ_PER_THREAD * 32;
	j_s = j_w_s + tidw * NNZ_PER_THREAD;
	j_e = j_s + NNZ_PER_THREAD;
	k = (i_e + i_s) / 2;
	while (i_s < i_e)
	{
		if (j_s >= row_ptr[k])
		{
			i_s = k + 1;
		}
		else
		{
			i_e = k;
		}
		k = (i_e + i_s) / 2;
	}
	i_s--;
	int one_line = (ja[j_s] & 0x80000000) ? 1 : 0;
	// int one_line = 0;
	thread_block_tile<32> tile32 = tiled_partition<32>(this_thread_block());
	spmv_full_warp(tile32, one_line, i_s, j_s, j_e, row_ptr, ja, a, x, y);
}


__global__ void gpu_kernel_spmv_row_indices_continuous(INT_T * thread_block_i_s, INT_T * thread_block_i_e, INT_T * thread_block_j_s, INT_T * thread_block_j_e, INT_T * row_ptr, INT_T * ia, INT_T * ja, ValueType * a, long m, long n, long nnz, ValueType * restrict x, ValueType * restrict y)
{
	int grid_size = gridDim.x;
	int block_id = blockIdx.x;
	if (block_id == grid_size - 1)
		spmv_last_block(thread_block_i_s, thread_block_i_e, thread_block_j_s, thread_block_j_e, row_ptr, ia, ja, a, m, n, nnz, x, y);
	else
		spmv_full_block(thread_block_i_s, thread_block_i_e, row_ptr, ia, ja, a, m, n, nnz, x, y);
}


void
compute_csr(CSRArrays * restrict csr, ValueType * restrict x, ValueType * restrict y)
{
	dim3 block_dims(BLOCK_SIZE);
	dim3 grid_dims[csr->num_streams];
	for(int i=0; i<csr->num_streams; i++)
		grid_dims[i] = dim3(csr->num_blocks[i]);
	// long shared_mem_size = BLOCK_SIZE * (sizeof(ValueType));
	// long shared_mem_size = BLOCK_SIZE * (sizeof(ValueType) + sizeof(INT_T));
	long shared_mem_size = 0;

	if (csr->x == NULL)
	{
		for(int i=0; i<csr->num_streams; i++)
			printf("Grid : {%d, %d, %d} blocks. Blocks : {%d, %d, %d} threads.\n", grid_dims[i].x, grid_dims[i].y, grid_dims[i].z, block_dims.x, block_dims.y, block_dims.z);
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

	if(TIME_IT2){
		for(int i=0; i<csr->num_streams; i++)
			gpuCudaErrorCheck(cudaEventRecord(csr->startEvent_execution[i], csr->stream[i]));
	}

	cudaMemset(csr->y_d2, 0, csr->m * csr->num_streams * sizeof(csr->y_d2));
	// cudaFuncCachePreferNone:   no preference for shared memory or L1 (default);
	// cudaFuncCachePreferShared: prefer larger shared memory and smaller L1 cache;
	// cudaFuncCachePreferL1:     prefer larger L1 cache and smaller shared memory;
	gpuCudaErrorCheck(cudaFuncSetCacheConfig(gpu_kernel_spmv_row_indices_continuous, cudaFuncCachePreferL1));
	for(int i=0; i<csr->num_streams; i++){
		// if(TIME_IT2){
		// 	gpuCudaErrorCheck(cudaEventRecord(csr->startEvent_execution[i], csr->stream[i]));
		// }
		gpu_kernel_spmv_row_indices_continuous<<<grid_dims[i], block_dims, shared_mem_size, csr->stream[i]>>>(csr->thread_block_i_s_d[i], csr->thread_block_i_e_d[i], csr->thread_block_j_s_d[i], csr->thread_block_j_e_d[i], csr->row_ptr_d[i], csr->ia_d[i], csr->ja_d[i], csr->a_d[i], csr->m, csr->n_stream[i], csr->nnz_stream[i], csr->x_d[i], csr->y_d2 + i*csr->m);
		// if(TIME_IT2){
		// 	gpuCudaErrorCheck(cudaEventRecord(csr->endEvent_execution[i], csr->stream[i]));
		// 	gpuCudaErrorCheck(cudaEventSynchronize(csr->endEvent_execution[i]));
		// 	float curr_execution_time;
		// 	gpuCudaErrorCheck(cudaEventElapsedTime(&curr_execution_time, csr->startEvent_execution[i], csr->endEvent_execution[i]));
		// 	csr->execution_time[i] += curr_execution_time;
		// }
		// printf("arxi %d\n", i);
		// gpuCudaErrorCheck(cudaStreamSynchronize(csr->stream[i]));
		// printf("telos %d\n", i);
	}

	gpuCudaErrorCheck(cudaPeekAtLastError());
	// for(int i=0; i<csr->num_streams; i++)
	// 	gpuCudaErrorCheck(cudaStreamSynchronize(csr->stream[i]));
	gpuCudaErrorCheck(cudaDeviceSynchronize());

	if(TIME_IT2){
		for(int i=0; i<csr->num_streams; i++){
			gpuCudaErrorCheck(cudaEventRecord(csr->endEvent_execution[i], csr->stream[i]));
			gpuCudaErrorCheck(cudaEventSynchronize(csr->endEvent_execution[i]));
			float curr_execution_time;
			gpuCudaErrorCheck(cudaEventElapsedTime(&curr_execution_time, csr->startEvent_execution[i], csr->endEvent_execution[i]));
			csr->execution_time[i] += curr_execution_time;	
		}
	}
	csr->iterations++;

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
	#ifdef PRINT_STATISTICS
	if(TIME_IT2){
		iterations = 0;
		for(int i=0; i<num_streams; i++)
			execution_time[i]=0.0;
	}
	#endif
}


int
statistics_print_labels(__attribute__((unused)) char * buf, __attribute__((unused)) long buf_n)
{
	return 0;
}


int
CSRArrays::statistics_print_data(__attribute__((unused)) char * buf, __attribute__((unused)) long buf_n)
{
	#ifdef PRINT_STATISTICS
	if(TIME_IT2){
		printf("--------\n");
		for(int i=0; i<num_streams; i++){
			double gflops = 2.0 * nnz_stream[i] / execution_time[i] / 1e6 * iterations;
			printf("Stream %d: %lf ms (GFLOPs = %.4lf)\n", i, execution_time[i], gflops);
		}
		printf("--------\n");
	}
	#endif
	return 0;
}

