#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

#include <cuda.h>
#include <cooperative_groups.h>
#include <cuda_pipeline.h>

#include "macros/cpp_defines.h"

#include "spmv_kernel.h"

#ifdef __cplusplus
extern "C"{
#endif
	#include "macros/macrolib.h"
	#include "time_it.h"
	#include "parallel_util.h"
	#include "array_metrics.h"

	#include "cuda/cuda_util.h"
	#include "aux/csr_util.h"
	#include "aux/csr_converter.h"
	#include "aux/csc_util.h"
	#include "aux/csc_converter.h"
#ifdef __cplusplus
}
#endif

using namespace cooperative_groups;

double * thread_time_compute, * thread_time_barrier;

#ifndef BLOCK_SIZE
#define BLOCK_SIZE 1024
#endif

#ifndef MULTIBLOCK_SIZE
#define MULTIBLOCK_SIZE 4
#endif

#ifndef TIME_IT
#define TIME_IT 0
#endif

INT_T spmv_csr_adaptive_rowblocks(INT_T *row_ptr, INT_T m, INT_T *row_blocks){
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

	INT_T * row_blocks;
	INT_T row_blocks_cnt;

	INT_T * ia_d;
	INT_T * row_blocks_d;
	INT_T * ja_d;
	ValueType * a_d;

	INT_T * ia_h;
	INT_T * row_blocks_h;
	INT_T * ja_h;
	ValueType * a_h;

	ValueType * x = NULL;
	ValueType * y = NULL;
	ValueType * x_d = NULL;
	ValueType * y_d = NULL;
	ValueType * x_h = NULL;
	ValueType * y_h = NULL;

	cudaStream_t stream;
	// cudaEvent_t is useful for timing, but for performance use " cudaEventCreateWithFlags ( &event, cudaEventDisableTiming) "
	cudaEvent_t startEvent_execution;
	cudaEvent_t endEvent_execution;
	
	cudaEvent_t startEvent_memcpy_ia;
	cudaEvent_t endEvent_memcpy_ia;
	cudaEvent_t startEvent_memcpy_row_blocks;
	cudaEvent_t endEvent_memcpy_row_blocks;
	cudaEvent_t startEvent_memcpy_ja;
	cudaEvent_t endEvent_memcpy_ja;
	cudaEvent_t startEvent_memcpy_a;
	cudaEvent_t endEvent_memcpy_a;

	cudaEvent_t startEvent_memcpy_x;
	cudaEvent_t endEvent_memcpy_x;
	cudaEvent_t startEvent_memcpy_y;
	cudaEvent_t endEvent_memcpy_y;

	int max_smem_per_block, multiproc_count, max_threads_per_block, warp_size, block_size, block_size2, max_threads_per_multiproc, max_persistent_l2_cache;

	CSRArrays(INT_T * ia, INT_T * ja, ValueType * a, long m, long n, long nnz) : Matrix_Format(m, n, nnz), ia(ia), ja(ja), a(a)
	{
		gpuCudaErrorCheck(cudaDeviceGetAttribute(&max_smem_per_block, cudaDevAttrMaxSharedMemoryPerBlock, 0));
		gpuCudaErrorCheck(cudaDeviceGetAttribute(&multiproc_count, cudaDevAttrMultiProcessorCount, 0));
		gpuCudaErrorCheck(cudaDeviceGetAttribute(&max_threads_per_block, cudaDevAttrMaxThreadsPerBlock , 0));
		gpuCudaErrorCheck(cudaDeviceGetAttribute(&warp_size, cudaDevAttrWarpSize , 0));
		gpuCudaErrorCheck(cudaDeviceGetAttribute(&max_threads_per_multiproc, cudaDevAttrMaxThreadsPerMultiProcessor, 0));
		gpuCudaErrorCheck(cudaDeviceGetAttribute(&max_persistent_l2_cache, cudaDevAttrMaxPersistingL2CacheSize, 0));
		printf("max_smem_per_block=%d\n", max_smem_per_block);
		printf("multiproc_count=%d\n", multiproc_count);
		printf("max_threads_per_block=%d\n", max_threads_per_block);
		printf("warp_size=%d\n", warp_size);
		printf("max_threads_per_multiproc=%d\n", max_threads_per_multiproc);
		printf("max_persistent_l2_cache=%d\n", max_persistent_l2_cache);

		block_size = BLOCK_SIZE;
		block_size2 = MULTIBLOCK_SIZE;

		row_blocks = (typeof(row_blocks)) malloc(m * sizeof(*row_blocks));
		row_blocks_cnt = spmv_csr_adaptive_rowblocks(ia, m, row_blocks);
		printf("%ld nnz, %d row_blocks ( %lf row blocks per thread block, %.0lf nnz/row_block )\n", nnz, row_blocks_cnt, row_blocks_cnt*1.0/MULTIBLOCK_SIZE, nnz*1.0/row_blocks_cnt);
		
		gpuCudaErrorCheck(cudaMalloc(&ia_d, (m+1) * sizeof(*ia_d)));
		gpuCudaErrorCheck(cudaMalloc(&row_blocks_d, row_blocks_cnt * sizeof(*row_blocks_d)));
		gpuCudaErrorCheck(cudaMalloc(&ja_d, nnz * sizeof(*ja_d)));
		gpuCudaErrorCheck(cudaMalloc(&a_d, nnz * sizeof(*a_d)));
		gpuCudaErrorCheck(cudaMalloc(&x_d, n * sizeof(*x_d)));
		gpuCudaErrorCheck(cudaMalloc(&y_d, m * sizeof(*y_d)));

		gpuCudaErrorCheck(cudaStreamCreate(&stream));

		// cuda events for timing measurements
		gpuCudaErrorCheck(cudaEventCreate(&startEvent_execution));
		gpuCudaErrorCheck(cudaEventCreate(&endEvent_execution));

		if(TIME_IT){
			gpuCudaErrorCheck(cudaEventCreate(&startEvent_memcpy_ia));
			gpuCudaErrorCheck(cudaEventCreate(&endEvent_memcpy_ia));
			gpuCudaErrorCheck(cudaEventCreate(&startEvent_memcpy_row_blocks));
			gpuCudaErrorCheck(cudaEventCreate(&endEvent_memcpy_row_blocks));
			gpuCudaErrorCheck(cudaEventCreate(&startEvent_memcpy_ja));
			gpuCudaErrorCheck(cudaEventCreate(&endEvent_memcpy_ja));
			gpuCudaErrorCheck(cudaEventCreate(&startEvent_memcpy_a));
			gpuCudaErrorCheck(cudaEventCreate(&endEvent_memcpy_a));

			gpuCudaErrorCheck(cudaEventCreate(&startEvent_memcpy_x));
			gpuCudaErrorCheck(cudaEventCreate(&endEvent_memcpy_x));
			gpuCudaErrorCheck(cudaEventCreate(&startEvent_memcpy_y));
			gpuCudaErrorCheck(cudaEventCreate(&endEvent_memcpy_y));
		}

		gpuCudaErrorCheck(cudaMallocHost(&ia_h, (m+1) * sizeof(*ia_h)));
		gpuCudaErrorCheck(cudaMallocHost(&row_blocks_h, row_blocks_cnt * sizeof(*row_blocks_h)));
		gpuCudaErrorCheck(cudaMallocHost(&ja_h, nnz * sizeof(*ja_h)));
		gpuCudaErrorCheck(cudaMallocHost(&a_h, nnz * sizeof(*a_h)));
		gpuCudaErrorCheck(cudaMallocHost(&x_h, n * sizeof(*x_h)));
		gpuCudaErrorCheck(cudaMallocHost(&y_h, m * sizeof(*y_h)));

		memcpy(ia_h, ia, (m+1) * sizeof(*ia_h));
		memcpy(row_blocks_h, row_blocks, row_blocks_cnt * sizeof(*row_blocks_h));
		memcpy(ja_h, ja, nnz * sizeof(*ja_h));
		memcpy(a_h, a, nnz * sizeof(*a_h));

		if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(startEvent_memcpy_ia));
		gpuCudaErrorCheck(cudaMemcpyAsync(ia_d, ia_h, (m+1) * sizeof(*ia_d), cudaMemcpyHostToDevice, stream));
		if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(endEvent_memcpy_ia));

		if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(startEvent_memcpy_row_blocks));
		gpuCudaErrorCheck(cudaMemcpyAsync(row_blocks_d, row_blocks_h, row_blocks_cnt * sizeof(*row_blocks_d), cudaMemcpyHostToDevice, stream));
		if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(endEvent_memcpy_row_blocks));

		if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(startEvent_memcpy_ja));
		gpuCudaErrorCheck(cudaMemcpyAsync(ja_d, ja_h, nnz * sizeof(*ja_d), cudaMemcpyHostToDevice, stream));
		if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(endEvent_memcpy_ja));

		if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(startEvent_memcpy_a));
		gpuCudaErrorCheck(cudaMemcpyAsync(a_d, a_h, nnz * sizeof(*a_d), cudaMemcpyHostToDevice, stream));
		if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(endEvent_memcpy_a));

		if(TIME_IT){
			gpuCudaErrorCheck(cudaEventSynchronize(endEvent_memcpy_ia));
			gpuCudaErrorCheck(cudaEventSynchronize(endEvent_memcpy_row_blocks));
			gpuCudaErrorCheck(cudaEventSynchronize(endEvent_memcpy_ja));
			gpuCudaErrorCheck(cudaEventSynchronize(endEvent_memcpy_a));

			float memcpyTime_cuda_ia, memcpyTime_cuda_row_blocks, memcpyTime_cuda_ja, memcpyTime_cuda_a;
			gpuCudaErrorCheck(cudaEventElapsedTime(&memcpyTime_cuda_ia, startEvent_memcpy_ia, endEvent_memcpy_ia));
			gpuCudaErrorCheck(cudaEventElapsedTime(&memcpyTime_cuda_row_blocks, startEvent_memcpy_row_blocks, endEvent_memcpy_row_blocks));
			gpuCudaErrorCheck(cudaEventElapsedTime(&memcpyTime_cuda_ja, startEvent_memcpy_ja, endEvent_memcpy_ja));
			gpuCudaErrorCheck(cudaEventElapsedTime(&memcpyTime_cuda_a, startEvent_memcpy_a, endEvent_memcpy_a));
			printf("(CUDA) Memcpy ia time = %.4lf ms, row_blocks time = %.4lf ms, ja time = %.4lf ms, a time = %.4lf ms\n", memcpyTime_cuda_ia, memcpyTime_cuda_row_blocks, memcpyTime_cuda_ja, memcpyTime_cuda_a);
		}
	}

	~CSRArrays()
	{
		free(a);
		free(ia);
		free(row_blocks);
		free(ja);

		gpuCudaErrorCheck(cudaFree(ia_d));
		gpuCudaErrorCheck(cudaFree(row_blocks_d));
		gpuCudaErrorCheck(cudaFree(ja_d));
		gpuCudaErrorCheck(cudaFree(a_d));
		gpuCudaErrorCheck(cudaFree(x_d));
		gpuCudaErrorCheck(cudaFree(y_d));

		gpuCudaErrorCheck(cudaFreeHost(ia_h));
		gpuCudaErrorCheck(cudaFreeHost(row_blocks_h));
		gpuCudaErrorCheck(cudaFreeHost(ja_h));
		gpuCudaErrorCheck(cudaFreeHost(a_h));
		gpuCudaErrorCheck(cudaFreeHost(x_h));
		gpuCudaErrorCheck(cudaFreeHost(y_h));

		gpuCudaErrorCheck(cudaStreamDestroy(stream));

		gpuCudaErrorCheck(cudaEventDestroy(startEvent_execution));
		gpuCudaErrorCheck(cudaEventDestroy(endEvent_execution));

		if(TIME_IT){
			gpuCudaErrorCheck(cudaEventDestroy(startEvent_memcpy_x));
			gpuCudaErrorCheck(cudaEventDestroy(endEvent_memcpy_x));
			gpuCudaErrorCheck(cudaEventDestroy(startEvent_memcpy_y));
			gpuCudaErrorCheck(cudaEventDestroy(endEvent_memcpy_y));

			gpuCudaErrorCheck(cudaEventDestroy(startEvent_memcpy_ia));
			gpuCudaErrorCheck(cudaEventDestroy(endEvent_memcpy_ia));
			gpuCudaErrorCheck(cudaEventDestroy(startEvent_memcpy_ja));
			gpuCudaErrorCheck(cudaEventDestroy(endEvent_memcpy_ja));
			gpuCudaErrorCheck(cudaEventDestroy(startEvent_memcpy_a));
			gpuCudaErrorCheck(cudaEventDestroy(endEvent_memcpy_a));

			gpuCudaErrorCheck(cudaEventDestroy(startEvent_memcpy_row_blocks));
			gpuCudaErrorCheck(cudaEventDestroy(endEvent_memcpy_row_blocks));
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
	snprintf(format_name, 100, "Custom_CSR_CUDA_ADAPTIVE_b%d_%d", csr->block_size, csr->block_size2);
	csr->format_name = format_name;
	return csr;
}


//==========================================================================================================================================
//= CSR Custom
//==========================================================================================================================================
/*
__global__ void gpu_kernel_csr_adaptive(INT_T * ia, INT_T * ja, ValueType * a, INT_T * row_blocks, ValueType * restrict x, ValueType * restrict y)
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
}
*/
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


void
compute_csr(CSRArrays * restrict csr, ValueType * restrict x, ValueType * restrict y)
{
	dim3 block_dims(csr->block_size);
	// dim3 grid_dims(csr->row_blocks_cnt-1);
	dim3 grid_dims(ceil((csr->row_blocks_cnt-1)/MULTIBLOCK_SIZE));

	if (csr->x == NULL)
	{
		printf("Grid : {%d, %d, %d} blocks. Blocks : {%d, %d, %d} threads.\n", grid_dims.x, grid_dims.y, grid_dims.z, block_dims.x, block_dims.y, block_dims.z);
		csr->x = x;
		if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(csr->startEvent_memcpy_x, csr->stream));
		memcpy(csr->x_h, x, csr->n * sizeof(ValueType));
		gpuCudaErrorCheck(cudaMemcpyAsync(csr->x_d, csr->x_h, csr->n * sizeof(*csr->x_d), cudaMemcpyHostToDevice, csr->stream));
		if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(csr->endEvent_memcpy_x, csr->stream));
		if(TIME_IT){
			gpuCudaErrorCheck(cudaEventSynchronize(csr->endEvent_memcpy_x));
			float memcpyTime_cuda;
			gpuCudaErrorCheck(cudaEventElapsedTime(&memcpyTime_cuda, csr->startEvent_memcpy_x, csr->endEvent_memcpy_x));
			printf("(CUDA) Memcpy x time = %.4lf ms\n", memcpyTime_cuda);
		}

		#ifdef PERSISTENT_L2_PREFETCH
			int x_d_size = csr->n * sizeof(*csr->x);
			gpuCudaErrorCheck(cudaCtxResetPersistingL2Cache()); // This needs to happen every time before running kernel for 1st time for a matrix...
			if(x_d_size < csr->max_persistent_l2_cache){
				cudaStreamAttrValue attribute;
				auto &window = attribute.accessPolicyWindow;
				window.base_ptr = csr->x_d;
				window.num_bytes = x_d_size;
				window.hitRatio = 1.0;
				window.hitProp = cudaAccessPropertyPersisting;
				window.missProp = cudaAccessPropertyStreaming;
				gpuCudaErrorCheck(cudaStreamSetAttribute(csr->stream, cudaStreamAttributeAccessPolicyWindow, &attribute));
			}
		#endif
	}

	gpu_kernel_csr_adaptive<<<grid_dims, block_dims, 0, csr->stream>>>(csr->ia_d, csr->ja_d, csr->a_d, csr->row_blocks_d, csr->x_d, csr->y_d);
	gpuCudaErrorCheck(cudaPeekAtLastError());
	gpuCudaErrorCheck(cudaDeviceSynchronize());

	if (csr->y == NULL)
	{
		csr->y = y;
		if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(csr->startEvent_memcpy_y, csr->stream));
		gpuCudaErrorCheck(cudaMemcpyAsync(csr->y_h, csr->y_d, csr->m * sizeof(*csr->y_d), cudaMemcpyDeviceToHost, csr->stream));
		gpuCudaErrorCheck(cudaStreamSynchronize(csr->stream));
		memcpy(y, csr->y_h, csr->m * sizeof(ValueType));
		if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(csr->endEvent_memcpy_y, csr->stream));

		if(TIME_IT){
			gpuCudaErrorCheck(cudaEventSynchronize(csr->endEvent_memcpy_y));
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

