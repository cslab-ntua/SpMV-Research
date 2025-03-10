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

#ifndef BLOCK_SIZE
#define BLOCK_SIZE 1024
#endif

#ifndef MULTIBLOCK_SIZE
#define MULTIBLOCK_SIZE 4
#endif

#ifndef NUM_STREAMS
#define NUM_STREAMS 1
#endif

#ifndef TIME_IT
#define TIME_IT 0
#endif

#ifndef TIME_IT2
#define TIME_IT2 1
#endif

INT_T spmv_csr_adaptive_rowblocks(INT_T *row_ptr, INT_T m, INT_T *row_blocks)
{
	row_blocks[0] = 0; 
	INT_T sum = 0; 
	INT_T last_i = 0; 
	INT_T cnt = 1;
	for (INT_T i = 1; i < m; i++) {
		// Count non-zeroes in this row 
		sum += row_ptr[i] - row_ptr[i-1];
		if (sum == BLOCK_SIZE){
			// This row fills up LOCAL_SIZE 
			last_i = i;
			row_blocks[cnt++] = i;
			sum = 0;
		}
		else if (sum > BLOCK_SIZE){
			if (i - last_i > 1) {
				// This extra row will not fit 
				row_blocks[cnt++] = i - 1;
				i--;
			}
			else if (i - last_i == 1){
				// This one row is too large
				row_blocks[cnt++] = i;
			}
			last_i = i;
			sum = 0;
		}
	}
	//  fill remaining positions of row_blocks until cnt % MULTIBLOCK_SIZE equals zero
	while (cnt % MULTIBLOCK_SIZE != 0)
		row_blocks[cnt++] = m;
	row_blocks[cnt++] = m;
	return cnt;
}

struct CSRArrays : Matrix_Format
{
	INT_T * ia;      // the usual rowptr (of size m+1)
	INT_T * ja;      // the colidx of each NNZ (of size nnz)
	ValueType * a;   // the values (of size NNZ)

	INT_T * row_blocks[NUM_STREAMS];
	INT_T row_blocks_cnt[NUM_STREAMS];

	INT_T * ia_h[NUM_STREAMS];
	INT_T * ja_h[NUM_STREAMS];
	ValueType * a_h[NUM_STREAMS];

	INT_T * row_blocks_h[NUM_STREAMS];

	INT_T * ia_d[NUM_STREAMS];
	INT_T * ja_d[NUM_STREAMS];
	ValueType * a_d[NUM_STREAMS];

	INT_T * row_blocks_d[NUM_STREAMS];

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
	
	cudaEvent_t startEvent_memcpy_ia[NUM_STREAMS];
	cudaEvent_t endEvent_memcpy_ia[NUM_STREAMS];
	cudaEvent_t startEvent_memcpy_row_blocks[NUM_STREAMS];
	cudaEvent_t endEvent_memcpy_row_blocks[NUM_STREAMS];
	cudaEvent_t startEvent_memcpy_ja[NUM_STREAMS];
	cudaEvent_t endEvent_memcpy_ja[NUM_STREAMS];
	cudaEvent_t startEvent_memcpy_a[NUM_STREAMS];
	cudaEvent_t endEvent_memcpy_a[NUM_STREAMS];

	cudaEvent_t startEvent_memcpy_x[NUM_STREAMS];
	cudaEvent_t endEvent_memcpy_x[NUM_STREAMS];
	cudaEvent_t startEvent_memcpy_y;
	cudaEvent_t endEvent_memcpy_y;

	cublasHandle_t handle;

	int max_smem_per_block, multiproc_count, max_threads_per_block, warp_size, block_size, block_size2, max_threads_per_multiproc;
	int num_streams;

	CSRArrays(INT_T * ia, INT_T * ja, ValueType * a, long m, long n, long nnz) : Matrix_Format(m, n, nnz), ia(ia), ja(ja), a(a)
	{
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

		block_size = BLOCK_SIZE;
		block_size2 = MULTIBLOCK_SIZE;
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
			row_blocks[i] = (INT_T *) malloc(m * sizeof(INT_T));
			row_blocks_cnt[i] = spmv_csr_adaptive_rowblocks(row_ptr_stream[i], m, row_blocks[i]);
			printf("Stream %d: %d columns, %d nnz, %d row_blocks ( %.0lf nnz/row_block )\n", i, n_stream[i], nnz_stream[i], row_blocks_cnt[i], nnz_stream[i]*1.0/row_blocks_cnt[i]);
		}

		for(int i=0; i<num_streams; i++){
			gpuCudaErrorCheck(cudaMalloc(&ia_d[i], (m+1) * sizeof(INT_T)));
			gpuCudaErrorCheck(cudaMalloc(&row_blocks_d[i], row_blocks_cnt[i] * sizeof(INT_T)));
			gpuCudaErrorCheck(cudaMalloc(&ja_d[i], nnz_stream[i] * sizeof(INT_T)));
			gpuCudaErrorCheck(cudaMalloc(&a_d[i], nnz_stream[i] * sizeof(ValueType)));
			gpuCudaErrorCheck(cudaMalloc(&x_d[i], n_stream[i] * sizeof(ValueType)));
			// gpuCudaErrorCheck(cudaMalloc(&y_d[i], m * sizeof(ValueType)));
		}
		gpuCudaErrorCheck(cudaMalloc(&y_d2, m * num_streams * sizeof(ValueType)));
		gpuCudaErrorCheck(cudaMalloc(&y_d_reduction, m * sizeof(ValueType)));
		gpuCublasErrorCheck(cublasCreate(&handle));

		for(int i=0; i<num_streams; i++){
			gpuCudaErrorCheck(cudaMallocHost(&ia_h[i], (m+1) * sizeof(INT_T)));
			gpuCudaErrorCheck(cudaMallocHost(&row_blocks_h[i], row_blocks_cnt[i] * sizeof(INT_T)));
			gpuCudaErrorCheck(cudaMallocHost(&ja_h[i], nnz_stream[i] * sizeof(INT_T)));
			gpuCudaErrorCheck(cudaMallocHost(&a_h[i], nnz_stream[i] * sizeof(ValueType)));
			gpuCudaErrorCheck(cudaMallocHost(&x_h[i], n_stream[i] * sizeof(ValueType)));
			gpuCudaErrorCheck(cudaMallocHost(&y_h[i], m * sizeof(ValueType)));
		}
		// gpuCudaErrorCheck(cudaMallocHost(&y_h2, m * num_streams * sizeof(ValueType)));
		// gpuCudaErrorCheck(cudaMallocHost(&y_h_final, m * sizeof(ValueType)));

		double time_memcpy = time_it(1,
		for(int i=0; i<num_streams; i++){
			memcpy(ia_h[i], row_ptr_stream[i], (m + 1) * sizeof(INT_T));
			memcpy(row_blocks_h[i], row_blocks[i], row_blocks_cnt[i] * sizeof(INT_T));
			memcpy(ja_h[i], col_idx_stream[i], nnz_stream[i] * sizeof(INT_T));
			memcpy(a_h[i], val_stream[i], nnz_stream[i] * sizeof(ValueType));
		}
		);
		printf("time_memcpy (ia_h, row_blocks_h, ja_h, a_h) = %lf ms\n", time_memcpy*1e3);

		// cuda events for timing measurements
		for(int i=0; i<num_streams; i++){
			gpuCudaErrorCheck(cudaStreamCreate(&stream[i]));

			gpuCudaErrorCheck(cudaEventCreate(&startEvent_execution[i]));
			gpuCudaErrorCheck(cudaEventCreate(&endEvent_execution[i]));
			execution_time[i] = 0.0;
		}
		iterations=0;
		gpuCublasErrorCheck(cublasSetStream(handle, stream[0]));

		if(TIME_IT){
			for(int i=0; i<num_streams; i++){
				gpuCudaErrorCheck(cudaEventCreate(&startEvent_memcpy_ia[i]));
				gpuCudaErrorCheck(cudaEventCreate(&endEvent_memcpy_ia[i]));
				gpuCudaErrorCheck(cudaEventCreate(&startEvent_memcpy_row_blocks[i]));
				gpuCudaErrorCheck(cudaEventCreate(&endEvent_memcpy_row_blocks[i]));
				gpuCudaErrorCheck(cudaEventCreate(&startEvent_memcpy_ja[i]));
				gpuCudaErrorCheck(cudaEventCreate(&endEvent_memcpy_ja[i]));
				gpuCudaErrorCheck(cudaEventCreate(&startEvent_memcpy_a[i]));
				gpuCudaErrorCheck(cudaEventCreate(&endEvent_memcpy_a[i]));

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
			
			if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(startEvent_memcpy_row_blocks[i], stream[i]));
			gpuCudaErrorCheck(cudaMemcpyAsync(row_blocks_d[i], row_blocks_h[i], row_blocks_cnt[i] * sizeof(INT_T), cudaMemcpyHostToDevice, stream[i]));
			if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(endEvent_memcpy_row_blocks[i], stream[i]));

			if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(startEvent_memcpy_ja[i], stream[i]));
			gpuCudaErrorCheck(cudaMemcpyAsync(ja_d[i], ja_h[i], nnz_stream[i] * sizeof(INT_T), cudaMemcpyHostToDevice, stream[i]));
			if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(endEvent_memcpy_ja[i], stream[i]));
			
			if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(startEvent_memcpy_a[i], stream[i]));
			gpuCudaErrorCheck(cudaMemcpyAsync(a_d[i], a_h[i], nnz_stream[i] * sizeof(ValueType), cudaMemcpyHostToDevice, stream[i]));
			if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(endEvent_memcpy_a[i], stream[i]));
		}

		if(TIME_IT){
			for(int i=0; i<num_streams; i++){
				gpuCudaErrorCheck(cudaStreamSynchronize(stream[i]));
				float memcpyTime_cuda_ia, memcpyTime_cuda_row_blocks, memcpyTime_cuda_ja, memcpyTime_cuda_a;
				gpuCudaErrorCheck(cudaEventElapsedTime(&memcpyTime_cuda_ia, startEvent_memcpy_ia[i], endEvent_memcpy_ia[i]));
				gpuCudaErrorCheck(cudaEventElapsedTime(&memcpyTime_cuda_row_blocks, startEvent_memcpy_row_blocks[i], endEvent_memcpy_row_blocks[i]));
				gpuCudaErrorCheck(cudaEventElapsedTime(&memcpyTime_cuda_ja, startEvent_memcpy_ja[i], endEvent_memcpy_ja[i]));
				gpuCudaErrorCheck(cudaEventElapsedTime(&memcpyTime_cuda_a, startEvent_memcpy_a[i], endEvent_memcpy_a[i]));
				printf("(CUDA) (stream %d) Memcpy ia time = %.4lf ms, row_blocks time = %.4lf ms, ja time = %.4lf ms, a time = %.4lf ms\n", i, memcpyTime_cuda_ia, memcpyTime_cuda_row_blocks, memcpyTime_cuda_ja, memcpyTime_cuda_a);
			}
		}
	}

	~CSRArrays()
	{
		free(a);
		free(ia);
		free(ja);
		for(int i=0; i<num_streams; i++){
			free(row_blocks[i]);

			gpuCudaErrorCheck(cudaFree(ia_d[i]));
			gpuCudaErrorCheck(cudaFree(row_blocks_d[i]));
			gpuCudaErrorCheck(cudaFree(ja_d[i]));
			gpuCudaErrorCheck(cudaFree(a_d[i]));
			gpuCudaErrorCheck(cudaFree(x_d[i]));
			// gpuCudaErrorCheck(cudaFree(y_d[i]));

			gpuCudaErrorCheck(cudaFreeHost(ia_h[i]));
			gpuCudaErrorCheck(cudaFreeHost(row_blocks_h[i]));
			gpuCudaErrorCheck(cudaFreeHost(ja_h[i]));
			gpuCudaErrorCheck(cudaFreeHost(a_h[i]));
			gpuCudaErrorCheck(cudaFreeHost(x_h[i]));
			gpuCudaErrorCheck(cudaFreeHost(y_h[i]));

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

				gpuCudaErrorCheck(cudaEventDestroy(startEvent_memcpy_ia[i]));
				gpuCudaErrorCheck(cudaEventDestroy(endEvent_memcpy_ia[i]));
				gpuCudaErrorCheck(cudaEventDestroy(startEvent_memcpy_ja[i]));
				gpuCudaErrorCheck(cudaEventDestroy(endEvent_memcpy_ja[i]));
				gpuCudaErrorCheck(cudaEventDestroy(startEvent_memcpy_a[i]));
				gpuCudaErrorCheck(cudaEventDestroy(endEvent_memcpy_a[i]));

				gpuCudaErrorCheck(cudaEventDestroy(startEvent_memcpy_row_blocks[i]));
				gpuCudaErrorCheck(cudaEventDestroy(endEvent_memcpy_row_blocks[i]));
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
csr_to_format(INT_T * row_ptr, INT_T * col_ind, ValueType * values, long m, long n, long nnz, int symmetric)
{
	if (symmetric)
		error("symmetric matrices not supported by this format, expand symmetry");
	struct CSRArrays * csr = new CSRArrays(row_ptr, col_ind, values, m, n, nnz);
	csr->mem_footprint = nnz * (sizeof(ValueType) + sizeof(INT_T)) + (m+1) * sizeof(INT_T);
	char *format_name;
	format_name = (char *)malloc(100*sizeof(char));
	snprintf(format_name, 100, "Custom_CSR_CUDA_ADAPTIVE_b%d_%d_s%d", csr->block_size, csr->block_size2, csr->num_streams);

	csr->format_name = format_name;
	return csr;
}


//==========================================================================================================================================
//= CSR Custom
//==========================================================================================================================================

/*__global__ void gpu_kernel_csr_adaptive(INT_T * ia, INT_T * ja, ValueType * a, INT_T * row_blocks, ValueType * restrict x, ValueType * restrict y)
{
	INT_T startRow = row_blocks[blockIdx.x];
	INT_T nextStartRow = row_blocks[blockIdx.x + 1];
	INT_T num_rows = nextStartRow - startRow;
	INT_T i = threadIdx.x;
	__shared__ volatile ValueType LDS[BLOCK_SIZE];
	
	// If the block consists of more than one row then run CSR Stream
	if (num_rows > 1) {
		// how many nonzeros does this rowblock hold?
		// they will be less than the BLOCK_SIZE (the size of LDS)
		int nnz = ia[nextStartRow] - ia[startRow];
		int col_offset = ia[startRow];

		// Each thread writes to shared memory the result of multiplication for one nonzero
		// However, if there are less nonzeros than the block size, some threads will not be utilized
		if (i < nnz)
			LDS[i] = a[col_offset + i] * x[ja[col_offset + i]];
 		// After all positions of LDS have been filled, proceed. 
		__syncthreads();
		
		// Threads that fall within a range sum up the partial results
		// Thread0 of the block will be assigned with the first row of the thread block (startRow+0) and then the next row will be (startRow+BLOCK_SIZE) etc...
		// How many rows per thread depends on how few nonzeros this specific block can hold...
		for (int k = startRow + i; k < nextStartRow; k += BLOCK_SIZE){
			ValueType temp = 0;
			// Sum partial results that this row (k) has results in LDS
			for (INT_T j = (ia[k] - col_offset); j < (ia[k + 1] - col_offset); j++)
				temp = temp + LDS[j];
			// And finally store result in the output y vector.
			y[k] = temp;
		}
	}
	// If the block consists of only one row then run CSR Vector
	else {
		// Thread ID in warp
		INT_T ia_Start = ia[startRow];
		INT_T ia_End   = ia[nextStartRow];
		ValueType sum  = 0;

		// Use all threads in a warp to accumulate multiplied elements
		// Due to the fact that each for loop starts from "ia_Start" + some i (the index inside the thread block) 
		// LDS will be filled with all partial results from this specific row
		// It may be underutilized, considering the fact that this row will consist of less than BLOCK_SIZE elements
		for (INT_T j = ia_Start + i; j < ia_End; j += BLOCK_SIZE){
			INT_T col = ja[j];
			sum += a[j] * x[col];
		}
		// write partial sum at position i (index in thread block) in the LDS array
		LDS[i] = sum;
		__syncthreads();

		// Reduce partial sums
		// reduce results as in 
		// (BS/2 sums)  LDS[i] = LDS[i] + LDS[i + BS/2];, LDS[i+1] = LDS[i+1] + LDS[i+1 + BS/2];
		// (BS/4 sums)  LDS[i] = LDS[i] + LDS[i + BS/4]
		// ...
		// (1 sum)      LDS[i] = LDS[i] + LDS[i+1]; and then finish
		for (int stride = BLOCK_SIZE >> 1; stride > 0; stride >>= 1) {
			__syncthreads();
			if (i < stride)
				LDS[i] += LDS[i + stride]; 
		}
		// Write result
		if (i == 0){
			y[startRow] = LDS[i];
		}
	}
}*/


__global__ void gpu_kernel_csr_adaptive(INT_T * ia, INT_T * ja, ValueType * a, INT_T * row_blocks, ValueType * restrict x, ValueType * restrict y)
{
	__shared__ volatile ValueType LDS[MULTIBLOCK_SIZE][BLOCK_SIZE];
	INT_T i = threadIdx.x;

	INT_T startRow[MULTIBLOCK_SIZE];
	INT_T nextStartRow[MULTIBLOCK_SIZE];
	INT_T num_rows[MULTIBLOCK_SIZE];

	for(int kk = 0; kk < MULTIBLOCK_SIZE; kk++){
		startRow[kk]     = row_blocks[blockIdx.x*MULTIBLOCK_SIZE + kk];
		nextStartRow[kk] = row_blocks[blockIdx.x*MULTIBLOCK_SIZE + kk + 1];
		num_rows[kk]     = nextStartRow[kk] - startRow[kk];
	}

	for(int kk = 0; kk < MULTIBLOCK_SIZE; kk++){
		// If the block consists of more than one row then run CSR Stream
		if (num_rows[kk] > 1) {
			// how many nonzeros does this rowblock hold?
			// they will be less than the BLOCK_SIZE (the size of LDS)
			int nnz = ia[nextStartRow[kk]] - ia[startRow[kk]];
			int col_offset = ia[startRow[kk]];

			// Each thread writes to shared memory the result of multiplication for one nonzero
			if (i < nnz)
				LDS[kk][i] = a[col_offset + i] * x[ja[col_offset + i]];
	 		// After all positions of LDS have been filled, proceed. 
			__syncthreads();
			
			// Threads that fall within a range sum up the partial results
			// Thread0 of the block will be assigned with the first row of the thread block (startRow+0) and then the next row will be (startRow+BLOCK_SIZE) etc...
			// How many rows per thread depends on how few nonzeros this specific block can hold...
			for (int k = startRow[kk] + i; k < nextStartRow[kk]; k += BLOCK_SIZE){
				ValueType temp = 0;
				// Sum partial results that this row (k) has results in LDS
				for (INT_T j = (ia[k] - col_offset); j < (ia[k + 1] - col_offset); j++)
					temp = temp + LDS[kk][j];
				// And finally store result in the output y vector.
				y[k] = temp;
			}
		}
		// If the block consists of only one row then run CSR Vector
		else if(num_rows[kk] == 1) {
			// Thread ID in warp
			INT_T ia_Start = ia[startRow[kk]];
			INT_T ia_End   = ia[nextStartRow[kk]];
			ValueType sum  = 0;

			// Use all threads in a warp to accumulate multiplied elements
			// Due to the fact that each for loop starts from "ia_Start" + some i (the index inside the thread block) 
			// LDS will be filled with all partial results from this specific row
			// It may be underutilized, considering the fact that this row will consist of less than BLOCK_SIZE elements
			for (INT_T j = ia_Start + i; j < ia_End; j += BLOCK_SIZE){
				INT_T col = ja[j];
				sum = __fma_rn(a[j], x[col], sum); // sum += a[j] * x[col];
			}
			// write partial sum at position i (index in thread block) in the LDS array
			LDS[kk][i] = sum;
			__syncthreads();

			// Reduce partial sums
			// reduce results as in 
			// (BS/2 sums)  LDS[i] = LDS[i] + LDS[i + BS/2];, LDS[i+1] = LDS[i+1] + LDS[i+1 + BS/2];
			// (BS/4 sums)  LDS[i] = LDS[i] + LDS[i + BS/4]
			// ...
			// (1 sum)      LDS[i] = LDS[i] + LDS[i+1]; and then finish
			for (int stride = BLOCK_SIZE >> 1; stride > 0; stride >>= 1) {
				__syncthreads();
				if (i < stride)
					LDS[kk][i] += LDS[kk][i + stride]; 
			}
			// Write result
			if (i == 0){
				y[startRow[kk]] = LDS[kk][i];
			}
		}
	}
}

__global__ void gpu_kernel_csr_adaptive_local2048(INT_T * ia, INT_T * ja, ValueType * a, INT_T * row_blocks, ValueType * restrict x, ValueType * restrict y)
{
	__shared__ volatile ValueType LDS[MULTIBLOCK_SIZE][BLOCK_SIZE];
	INT_T i = threadIdx.x;
	__shared__ volatile ValueType x_local[2048];
	// instruct each thread of block to fetch some values of x to x_local
	for(int j=i; j<2048; j+=BLOCK_SIZE)
		x_local[j] = x[j];

	INT_T startRow[MULTIBLOCK_SIZE];
	INT_T nextStartRow[MULTIBLOCK_SIZE];
	INT_T num_rows[MULTIBLOCK_SIZE];

	for(int kk = 0; kk < MULTIBLOCK_SIZE; kk++){
		startRow[kk]     = row_blocks[blockIdx.x*MULTIBLOCK_SIZE + kk];
		nextStartRow[kk] = row_blocks[blockIdx.x*MULTIBLOCK_SIZE + kk + 1];
		num_rows[kk]     = nextStartRow[kk] - startRow[kk];
	}

	for(int kk = 0; kk < MULTIBLOCK_SIZE; kk++){
		// If the block consists of more than one row then run CSR Stream
		if (num_rows[kk] > 1) {
			// how many nonzeros does this rowblock hold?
			// they will be less than the BLOCK_SIZE (the size of LDS)
			int nnz = ia[nextStartRow[kk]] - ia[startRow[kk]];
			int col_offset = ia[startRow[kk]];

			// Each thread writes to shared memory the result of multiplication for one nonzero
			if (i < nnz)
				LDS[kk][i] = a[col_offset + i] * x_local[ja[col_offset + i]];
	 		// After all positions of LDS have been filled, proceed. 
			__syncthreads();
			
			// Threads that fall within a range sum up the partial results
			// Thread0 of the block will be assigned with the first row of the thread block (startRow+0) and then the next row will be (startRow+BLOCK_SIZE) etc...
			// How many rows per thread depends on how few nonzeros this specific block can hold...
			for (int k = startRow[kk] + i; k < nextStartRow[kk]; k += BLOCK_SIZE){
				ValueType temp = 0;
				// Sum partial results that this row (k) has results in LDS
				for (INT_T j = (ia[k] - col_offset); j < (ia[k + 1] - col_offset); j++)
					temp = temp + LDS[kk][j];
				// And finally store result in the output y vector.
				y[k] = temp;
			}
		}
		// If the block consists of only one row then run CSR Vector
		else if(num_rows[kk] == 1) {
			// Thread ID in warp
			INT_T ia_Start = ia[startRow[kk]];
			INT_T ia_End   = ia[nextStartRow[kk]];
			ValueType sum  = 0;

			// Use all threads in a warp to accumulate multiplied elements
			// Due to the fact that each for loop starts from "ia_Start" + some i (the index inside the thread block) 
			// LDS will be filled with all partial results from this specific row
			// It may be underutilized, considering the fact that this row will consist of less than BLOCK_SIZE elements
			for (INT_T j = ia_Start + i; j < ia_End; j += BLOCK_SIZE){
				INT_T col = ja[j];
				sum += a[j] * x_local[col];
			}
			// write partial sum at position i (index in thread block) in the LDS array
			LDS[kk][i] = sum;
			__syncthreads();

			// Reduce partial sums
			// reduce results as in 
			// (BS/2 sums)  LDS[i] = LDS[i] + LDS[i + BS/2];, LDS[i+1] = LDS[i+1] + LDS[i+1 + BS/2];
			// (BS/4 sums)  LDS[i] = LDS[i] + LDS[i + BS/4]
			// ...
			// (1 sum)      LDS[i] = LDS[i] + LDS[i+1]; and then finish
			for (int stride = BLOCK_SIZE >> 1; stride > 0; stride >>= 1) {
				__syncthreads();
				if (i < stride)
					LDS[kk][i] += LDS[kk][i + stride]; 
			}
			// Write result
			if (i == 0){
				y[startRow[kk]] = LDS[kk][i];
			}
		}
	}
}


void
compute_csr(CSRArrays * restrict csr, ValueType * restrict x, ValueType * restrict y)
{
	dim3 block_dims(csr->block_size);
	dim3 grid_dims[csr->num_streams];
		// dim3 grid_dims(csr->row_blocks_cnt-1);
	// dim3 grid_dims(ceil((csr->row_blocks_cnt-1)/MULTIBLOCK_SIZE));

	for(int i=0; i<csr->num_streams; i++)
		grid_dims[i] = dim3(ceil((csr->row_blocks_cnt[i]-1)/MULTIBLOCK_SIZE));

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

	for(int i=0; i<csr->num_streams; i++){
		gpu_kernel_csr_adaptive<<<grid_dims[i], block_dims, 0, csr->stream[i]>>>(csr->ia_d[i], csr->ja_d[i], csr->a_d[i], csr->row_blocks_d[i], csr->x_d[i], csr->y_d2 + i*csr->m);
	}

	gpuCudaErrorCheck(cudaPeekAtLastError());
	for(int i=0; i<csr->num_streams; i++)
		gpuCudaErrorCheck(cudaStreamSynchronize(csr->stream[i]));

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

