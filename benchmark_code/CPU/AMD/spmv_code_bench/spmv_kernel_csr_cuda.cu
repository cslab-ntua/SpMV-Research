#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

#include <cuda.h>

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

extern int prefetch_distance;

double * thread_time_compute, * thread_time_barrier;

#ifndef NUM_THREADS
#define NUM_THREADS 1024
#endif

#ifndef TIME_IT
#define TIME_IT 0
#endif

// #ifndef VERIFIED
// #define VERIFIED 1
// #endif

struct CSRArrays : Matrix_Format
{
	INT_T * ia;      // the usual rowptr (of size m+1)
	INT_T * ja;      // the colidx of each NNZ (of size nnz)
	ValueType * a;   // the values (of size NNZ)
	INT_T * thread_i_s = NULL;
	INT_T * thread_i_e = NULL;

	INT_T * ia_d;
	INT_T * ja_d;
	ValueType * a_d;
	INT_T * thread_i_s_d;
	INT_T * thread_i_e_d;

	INT_T * ia_h;
	INT_T * ja_h;
	ValueType * a_h;
	INT_T * thread_i_s_h;
	INT_T * thread_i_e_h;

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
	cudaEvent_t startEvent_memcpy_ja;
	cudaEvent_t endEvent_memcpy_ja;
	cudaEvent_t startEvent_memcpy_a;
	cudaEvent_t endEvent_memcpy_a;
	cudaEvent_t startEvent_memcpy_thread_i_s;
	cudaEvent_t endEvent_memcpy_thread_i_s;
	cudaEvent_t startEvent_memcpy_thread_i_e;
	cudaEvent_t endEvent_memcpy_thread_i_e;

	cudaEvent_t startEvent_memcpy_x;
	cudaEvent_t endEvent_memcpy_x;
	cudaEvent_t startEvent_memcpy_y;
	cudaEvent_t endEvent_memcpy_y;

	int max_smem_per_block, multiproc_count, max_threads_per_block, warp_size, max_threads_per_multiproc, max_persistent_l2_cache;
	int num_threads;

	CSRArrays(INT_T * ia, INT_T * ja, ValueType * a, long m, long n, long nnz) : Matrix_Format(m, n, nnz), ia(ia), ja(ja), a(a)
	{
		double time_balance;
		long i;

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

		num_threads = NUM_THREADS;
		printf("NUM_THREADS=%d\n", num_threads);

		thread_i_s = (INT_T *) malloc(num_threads * sizeof(*thread_i_s));
		thread_i_e = (INT_T *) malloc(num_threads * sizeof(*thread_i_e));
		time_balance = time_it(1,
			for (i=0;i<num_threads;i++)
			{
				// loop_partitioner_balance_iterations(num_threads, i, 0, m, &thread_i_s[i], &thread_i_e[i]);
				loop_partitioner_balance_prefix_sums(num_threads, i, ia, m, nnz, &thread_i_s[i], &thread_i_e[i]);
			}
		);
		printf("balance time = %g\n", time_balance);

		gpuCudaErrorCheck(cudaMalloc(&ia_d, (m+1) * sizeof(*ia_d)));
		gpuCudaErrorCheck(cudaMalloc(&ja_d, nnz * sizeof(*ja_d)));
		gpuCudaErrorCheck(cudaMalloc(&a_d, nnz * sizeof(*a_d)));
		gpuCudaErrorCheck(cudaMalloc(&thread_i_s_d, num_threads * sizeof(*thread_i_s_d)));
		gpuCudaErrorCheck(cudaMalloc(&thread_i_e_d, num_threads * sizeof(*thread_i_e_d)));
		gpuCudaErrorCheck(cudaMalloc(&x_d, n * sizeof(*x_d)));
		gpuCudaErrorCheck(cudaMalloc(&y_d, m * sizeof(*y_d)));

		gpuCudaErrorCheck(cudaStreamCreate(&stream));

		// cuda events for timing measurements
		gpuCudaErrorCheck(cudaEventCreate(&startEvent_execution));
		gpuCudaErrorCheck(cudaEventCreate(&endEvent_execution));

		if(TIME_IT){
			gpuCudaErrorCheck(cudaEventCreate(&startEvent_memcpy_ia));
			gpuCudaErrorCheck(cudaEventCreate(&endEvent_memcpy_ia));
			gpuCudaErrorCheck(cudaEventCreate(&startEvent_memcpy_ja));
			gpuCudaErrorCheck(cudaEventCreate(&endEvent_memcpy_ja));
			gpuCudaErrorCheck(cudaEventCreate(&startEvent_memcpy_a));
			gpuCudaErrorCheck(cudaEventCreate(&endEvent_memcpy_a));
			gpuCudaErrorCheck(cudaEventCreate(&startEvent_memcpy_thread_i_s));
			gpuCudaErrorCheck(cudaEventCreate(&endEvent_memcpy_thread_i_s));
			gpuCudaErrorCheck(cudaEventCreate(&startEvent_memcpy_thread_i_e));
			gpuCudaErrorCheck(cudaEventCreate(&endEvent_memcpy_thread_i_e));

			gpuCudaErrorCheck(cudaEventCreate(&startEvent_memcpy_x));
			gpuCudaErrorCheck(cudaEventCreate(&endEvent_memcpy_x));
			gpuCudaErrorCheck(cudaEventCreate(&startEvent_memcpy_y));
			gpuCudaErrorCheck(cudaEventCreate(&endEvent_memcpy_y));
		}

		gpuCudaErrorCheck(cudaMallocHost(&ia_h, (m+1) * sizeof(*ia_h)));
		gpuCudaErrorCheck(cudaMallocHost(&ja_h, nnz * sizeof(*ja_h)));
		gpuCudaErrorCheck(cudaMallocHost(&a_h, nnz * sizeof(*a_h)));
		gpuCudaErrorCheck(cudaMallocHost(&thread_i_s_h, num_threads * sizeof(*thread_i_s_h)));
		gpuCudaErrorCheck(cudaMallocHost(&thread_i_e_h, num_threads * sizeof(*thread_i_e_h)));
		gpuCudaErrorCheck(cudaMallocHost(&x_h, n * sizeof(*x_h)));
		gpuCudaErrorCheck(cudaMallocHost(&y_h, m * sizeof(*y_h)));

		memcpy(ia_h, ia, (m+1) * sizeof(*ia_h));
		memcpy(ja_h, ja, nnz * sizeof(*ja_h));
		memcpy(a_h, a, nnz * sizeof(*a_h));
		memcpy(thread_i_s_h, thread_i_s, num_threads * sizeof(*thread_i_s_h));
		memcpy(thread_i_e_h, thread_i_e, num_threads * sizeof(*thread_i_e_h));

		if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(startEvent_memcpy_ia));
		gpuCudaErrorCheck(cudaMemcpyAsync(ia_d, ia_h, (m+1) * sizeof(*ia_d), cudaMemcpyHostToDevice, stream));
		if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(endEvent_memcpy_ia));

		if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(startEvent_memcpy_ja));
		gpuCudaErrorCheck(cudaMemcpyAsync(ja_d, ja_h, nnz * sizeof(*ja_d), cudaMemcpyHostToDevice, stream));
		if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(endEvent_memcpy_ja));

		if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(startEvent_memcpy_a));
		gpuCudaErrorCheck(cudaMemcpyAsync(a_d, a_h, nnz * sizeof(*a_d), cudaMemcpyHostToDevice, stream));
		if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(endEvent_memcpy_a));

		if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(startEvent_memcpy_thread_i_s));
		gpuCudaErrorCheck(cudaMemcpyAsync(thread_i_s_d, thread_i_s_h, num_threads * sizeof(*thread_i_s_d), cudaMemcpyHostToDevice, stream));
		if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(endEvent_memcpy_thread_i_s));

		if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(startEvent_memcpy_thread_i_e));
		gpuCudaErrorCheck(cudaMemcpyAsync(thread_i_e_d, thread_i_e_h, num_threads * sizeof(*thread_i_e_d), cudaMemcpyHostToDevice, stream));
		if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(endEvent_memcpy_thread_i_e));

		if(TIME_IT){
			gpuCudaErrorCheck(cudaEventSynchronize(endEvent_memcpy_ia));
			gpuCudaErrorCheck(cudaEventSynchronize(endEvent_memcpy_ja));
			gpuCudaErrorCheck(cudaEventSynchronize(endEvent_memcpy_a));
			gpuCudaErrorCheck(cudaEventSynchronize(endEvent_memcpy_thread_i_s));
			gpuCudaErrorCheck(cudaEventSynchronize(endEvent_memcpy_thread_i_e));

			float memcpyTime_cuda_ia, memcpyTime_cuda_ja, memcpyTime_cuda_a, memcpyTime_cuda_thread_i_s, memcpyTime_cuda_thread_i_e;
			gpuCudaErrorCheck(cudaEventElapsedTime(&memcpyTime_cuda_ia, startEvent_memcpy_ia, endEvent_memcpy_ia));
			gpuCudaErrorCheck(cudaEventElapsedTime(&memcpyTime_cuda_ja, startEvent_memcpy_ja, endEvent_memcpy_ja));
			gpuCudaErrorCheck(cudaEventElapsedTime(&memcpyTime_cuda_a, startEvent_memcpy_a, endEvent_memcpy_a));
			gpuCudaErrorCheck(cudaEventElapsedTime(&memcpyTime_cuda_thread_i_s, startEvent_memcpy_thread_i_s, endEvent_memcpy_thread_i_s));
			gpuCudaErrorCheck(cudaEventElapsedTime(&memcpyTime_cuda_thread_i_e, startEvent_memcpy_thread_i_e, endEvent_memcpy_thread_i_e));
			printf("(CUDA) Memcpy ia time = %.4lf ms, ja time = %.4lf ms, a time = %.4lf ms, thread_s = %.4lf ms, thread_e = %.4lf ms\n", memcpyTime_cuda_ia, memcpyTime_cuda_ja, memcpyTime_cuda_a, memcpyTime_cuda_thread_i_s, memcpyTime_cuda_thread_i_e);
		}
	}

	~CSRArrays()
	{
		free(a);
		free(ia);
		free(ja);
		free(thread_i_s);
		free(thread_i_e);

		gpuCudaErrorCheck(cudaFree(ia_d));
		gpuCudaErrorCheck(cudaFree(ja_d));
		gpuCudaErrorCheck(cudaFree(a_d));
		gpuCudaErrorCheck(cudaFree(x_d));
		gpuCudaErrorCheck(cudaFree(y_d));
		gpuCudaErrorCheck(cudaFree(thread_i_s_d));
		gpuCudaErrorCheck(cudaFree(thread_i_e_d));

		gpuCudaErrorCheck(cudaFreeHost(ia_h));
		gpuCudaErrorCheck(cudaFreeHost(ja_h));
		gpuCudaErrorCheck(cudaFreeHost(a_h));
		gpuCudaErrorCheck(cudaFreeHost(x_h));
		gpuCudaErrorCheck(cudaFreeHost(y_h));
		gpuCudaErrorCheck(cudaFreeHost(thread_i_s_h));
		gpuCudaErrorCheck(cudaFreeHost(thread_i_e_h));

		gpuCudaErrorCheck(cudaStreamDestroy(stream));

		gpuCudaErrorCheck(cudaEventDestroy(startEvent_execution));
		gpuCudaErrorCheck(cudaEventDestroy(endEvent_execution));

		if(TIME_IT){
			gpuCudaErrorCheck(cudaEventDestroy(startEvent_memcpy_x));
			gpuCudaErrorCheck(cudaEventDestroy(endEvent_memcpy_x));
			gpuCudaErrorCheck(cudaEventDestroy(startEvent_memcpy_y));
			gpuCudaErrorCheck(cudaEventDestroy(endEvent_memcpy_y));
			gpuCudaErrorCheck(cudaEventDestroy(startEvent_memcpy_thread_i_s));
			gpuCudaErrorCheck(cudaEventDestroy(endEvent_memcpy_thread_i_s));
			gpuCudaErrorCheck(cudaEventDestroy(startEvent_memcpy_thread_i_e));
			gpuCudaErrorCheck(cudaEventDestroy(endEvent_memcpy_thread_i_e));

			gpuCudaErrorCheck(cudaEventDestroy(startEvent_memcpy_ia));
			gpuCudaErrorCheck(cudaEventDestroy(endEvent_memcpy_ia));
			gpuCudaErrorCheck(cudaEventDestroy(startEvent_memcpy_ja));
			gpuCudaErrorCheck(cudaEventDestroy(endEvent_memcpy_ja));
			gpuCudaErrorCheck(cudaEventDestroy(startEvent_memcpy_a));
			gpuCudaErrorCheck(cudaEventDestroy(endEvent_memcpy_a));
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
	snprintf(format_name, 100, "Custom_CSR_CUDA_t%d", csr->num_threads);
	csr->format_name = format_name;
	return csr;
}


//==========================================================================================================================================
//= CSR Custom
//==========================================================================================================================================


__global__ void gpu_kernel_csr_basic(INT_T * thread_i_s, INT_T * thread_i_e, INT_T * ia, INT_T * ja, ValueType * a, ValueType * restrict x, ValueType * restrict y)
{
	int tgid = cuda_get_thread_num();
	long i, i_s, i_e, j, j_e;
	ValueType sum;
	i_s = thread_i_s[tgid];
	i_e = thread_i_e[tgid];
	j = ia[i_s];
	// printf("%d: %ld %ld\n", tgid, i_s, i_e);
	for (i=i_s;i<i_e;i++)
	{
		j_e = ia[i+1];
		sum = 0;
		for (;j<j_e;j++)
		{
			sum += a[j] * x[ja[j]];
		}
		y[i] = sum;
	}
}


__global__ void gpu_kernel_csr_flat(INT_T * thread_i_s, INT_T * thread_i_e, INT_T * ia, INT_T * ja, ValueType * a, ValueType * restrict x, ValueType * restrict y)
{
	int tgid = cuda_get_thread_num();
	long i, i_s, i_e, j, j_e;
	ValueType sum;
	i_s = thread_i_s[tgid];
	i_e = thread_i_e[tgid];
	i = i_s;
	j = ia[i_s];
	j_e = ia[i_s+1];
	sum = 0;
	for (j=ia[i_s];i<i_e;j++)
	{
		if (j == j_e)
		{
			y[i] = sum;
			sum = 0;
			i++;
			j_e = ia[i+1];
			// if (i == i_e)
				// break;
		}
		sum += a[j] * x[ja[j]];
	}
}


void
compute_csr(CSRArrays * restrict csr, ValueType * restrict x, ValueType * restrict y)
{
	int num_threads = csr->num_threads;
	int block_size = csr->warp_size;
	dim3 block_dims(block_size);
	dim3 grid_dims(num_threads / block_size);

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

	// if(VERIFIED){
	// 	int num_loops = 1000;
	// 	for(int k=0;k<num_loops;k++)
	// 		gpu_kernel_csr_basic<<<grid_dims, block_dims>>>(thread_i_s_d, thread_i_e_d, csr->ia_d, csr->ja_d, csr->a_d, csr->x_d, csr->y_d);
	// 	gpuCudaErrorCheck(cudaPeekAtLastError());
	// 	gpuCudaErrorCheck(cudaDeviceSynchronize());
	// }

	// gpuCudaErrorCheck(cudaEventRecord(csr->startEvent_execution));

	// int num_loops = 128;
	// double time_execution = time_it(1,
	// 	for(int k=0;k<num_loops;k++){
	gpu_kernel_csr_basic<<<grid_dims, block_dims, 0, csr->stream>>>(csr->thread_i_s_d, csr->thread_i_e_d, csr->ia_d, csr->ja_d, csr->a_d, csr->x_d, csr->y_d);
	gpuCudaErrorCheck(cudaPeekAtLastError());
	gpuCudaErrorCheck(cudaDeviceSynchronize());
	// 	}
	// );

	// double gflops = csr->nnz / time_execution * num_loops * 2 * 1e-9;
	// printf("(DGAL timing) Execution time = %.4lf ms (%.4lf GFLOPS scalar-%d)\n", time_execution*1e3, gflops, csr->num_threads);

	// gpuCudaErrorCheck(cudaEventRecord(csr->endEvent_execution));
	// float executionTime_cuda;
	// gpuCudaErrorCheck(cudaEventSynchronize(csr->endEvent_execution));
	// gpuCudaErrorCheck(cudaEventElapsedTime(&executionTime_cuda, csr->startEvent_execution, csr->endEvent_execution));

	// double gflops_cuda = csr->nnz / executionTime_cuda * num_loops * 2 * 1e-6;
	// printf("(CUDA) Execution time = %.4lf ms (%.4lf GFLOPS @ %d threads for %.2lf MB workload)\n", executionTime_cuda, gflops_cuda, csr->num_threads, csr->mem_footprint/(1024*1024.0));

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

