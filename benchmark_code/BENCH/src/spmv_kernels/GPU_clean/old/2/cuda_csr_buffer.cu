#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

#include <cuda.h>

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
#ifdef __cplusplus
}
#endif


INT_T * thread_block_i_s = NULL;
INT_T * thread_block_i_e = NULL;

INT_T * thread_block_i_s_d = NULL;
INT_T * thread_block_i_e_d = NULL;

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

#ifndef TIME_IT
#define TIME_IT 0
#endif

struct CSRArrays : Matrix_Format
{
	INT_T * ia;      // the usual rowptr (of size m+1)
	INT_T * ja;      // the colidx of each NNZ (of size nnz)
	ValueType * a;   // the values (of size NNZ)
	unsigned char * rel_row_idx;

	INT_T * ia_d;
	INT_T * ja_d;
	ValueType * a_d;
	unsigned char * rel_row_idx_d;

	INT_T * ia_h;
	INT_T * ja_h;
	ValueType * a_h;
	unsigned char * rel_row_idx_h;
	INT_T * thread_block_i_s_h;
	INT_T * thread_block_i_e_h;

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
	cudaEvent_t startEvent_memcpy_thread_block_i_s;
	cudaEvent_t endEvent_memcpy_thread_block_i_s;
	cudaEvent_t startEvent_memcpy_thread_block_i_e;
	cudaEvent_t endEvent_memcpy_thread_block_i_e;
	cudaEvent_t startEvent_memcpy_rel_row_idx;
	cudaEvent_t endEvent_memcpy_rel_row_idx;

	cudaEvent_t startEvent_memcpy_x;
	cudaEvent_t endEvent_memcpy_x;
	cudaEvent_t startEvent_memcpy_y;
	cudaEvent_t endEvent_memcpy_y;

	int max_smem_per_block, multiproc_count, max_threads_per_block, warp_size, max_threads_per_multiproc, max_persistent_l2_cache, max_block_dim_x;
	int num_threads;
	int block_size;
	int num_blocks;

	int row_cluster_size;

	CSRArrays(INT_T * ia, INT_T * ja, ValueType * a, long m, long n, long nnz) : Matrix_Format(m, n, nnz), ia(ia), ja(ja), a(a)
	{
		double time_balance;
		long i;

		gpuCudaErrorCheck(cudaDeviceGetAttribute(&max_smem_per_block, cudaDevAttrMaxSharedMemoryPerBlock, 0));
		gpuCudaErrorCheck(cudaDeviceGetAttribute(&multiproc_count, cudaDevAttrMultiProcessorCount, 0));
		gpuCudaErrorCheck(cudaDeviceGetAttribute(&max_threads_per_block, cudaDevAttrMaxThreadsPerBlock , 0));
		gpuCudaErrorCheck(cudaDeviceGetAttribute(&warp_size, cudaDevAttrWarpSize , 0));
		gpuCudaErrorCheck(cudaDeviceGetAttribute(&max_threads_per_multiproc, cudaDevAttrMaxThreadsPerMultiProcessor, 0));
		gpuCudaErrorCheck(cudaDeviceGetAttribute(&max_block_dim_x, cudaDevAttrMaxBlockDimX, 0));
		gpuCudaErrorCheck(cudaDeviceGetAttribute(&max_persistent_l2_cache, cudaDevAttrMaxPersistingL2CacheSize, 0));
		printf("max_smem_per_block(bytes)=%d\n", max_smem_per_block);
		printf("multiproc_count=%d\n", multiproc_count);
		printf("max_threads_per_block=%d\n", max_threads_per_block);
		printf("warp_size=%d\n", warp_size);
		printf("max_threads_per_multiproc=%d\n", max_threads_per_multiproc);
		printf("max_block_dim_x=%d\n", max_block_dim_x);
		printf("max_persistent_l2_cache=%d\n", max_persistent_l2_cache);

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

		thread_block_i_s = (INT_T *) malloc(num_blocks * sizeof(*thread_block_i_s));
		thread_block_i_e = (INT_T *) malloc(num_blocks * sizeof(*thread_block_i_e));
		time_balance = time_it(1,
			for (i=0; i<num_blocks; i++)
			{
				// loop_partitioner_balance_iterations(num_blocks, i, 0, m, &thread_block_i_s[i], &thread_block_i_e[i]);
				loop_partitioner_balance_prefix_sums(num_blocks, i, ia, m, nnz, &thread_block_i_s[i], &thread_block_i_e[i]);
			}
		);
		printf("balance time = %g\n", time_balance);

		rel_row_idx = (typeof(rel_row_idx)) malloc(nnz * sizeof(*rel_row_idx));
		#pragma omp parallel
		{
			long i, i_s, i_e, i_rel, j, k;
			#pragma omp for
			for (k=0;k<num_blocks;k++)
			{
				i_s = thread_block_i_s[k];
				i_e = thread_block_i_e[k];
				for (i=i_s;i<i_e;i++)
				{
					i_rel = (i - i_s) % row_cluster_size;
					for (j=ia[i];j<ia[i+1];j++)
					{
						rel_row_idx[j] = i_rel;
					}
				}
			}
		}

		gpuCudaErrorCheck(cudaMalloc(&ia_d, (m+1) * sizeof(*ia_d)));
		gpuCudaErrorCheck(cudaMalloc(&ja_d, nnz * sizeof(*ja_d)));
		gpuCudaErrorCheck(cudaMalloc(&a_d, nnz * sizeof(*a_d)));
		gpuCudaErrorCheck(cudaMalloc(&rel_row_idx_d, nnz * sizeof(*rel_row_idx_d)));
		gpuCudaErrorCheck(cudaMalloc(&x_d, n * sizeof(*x_d)));
		gpuCudaErrorCheck(cudaMalloc(&y_d, m * sizeof(*y_d)));
		gpuCudaErrorCheck(cudaMalloc(&thread_block_i_s_d, num_blocks * sizeof(*thread_block_i_s_d)));
		gpuCudaErrorCheck(cudaMalloc(&thread_block_i_e_d, num_blocks * sizeof(*thread_block_i_e_d)));

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
			gpuCudaErrorCheck(cudaEventCreate(&startEvent_memcpy_rel_row_idx));
			gpuCudaErrorCheck(cudaEventCreate(&endEvent_memcpy_rel_row_idx));
			gpuCudaErrorCheck(cudaEventCreate(&startEvent_memcpy_thread_block_i_s));
			gpuCudaErrorCheck(cudaEventCreate(&endEvent_memcpy_thread_block_i_s));
			gpuCudaErrorCheck(cudaEventCreate(&startEvent_memcpy_thread_block_i_e));
			gpuCudaErrorCheck(cudaEventCreate(&endEvent_memcpy_thread_block_i_e));

			gpuCudaErrorCheck(cudaEventCreate(&startEvent_memcpy_x));
			gpuCudaErrorCheck(cudaEventCreate(&endEvent_memcpy_x));
			gpuCudaErrorCheck(cudaEventCreate(&startEvent_memcpy_y));
			gpuCudaErrorCheck(cudaEventCreate(&endEvent_memcpy_y));
		}

		gpuCudaErrorCheck(cudaMallocHost(&ia_h, (m+1) * sizeof(*ia_h)));
		gpuCudaErrorCheck(cudaMallocHost(&ja_h, nnz * sizeof(*ja_h)));
		gpuCudaErrorCheck(cudaMallocHost(&a_h, nnz * sizeof(*a_h)));
		gpuCudaErrorCheck(cudaMallocHost(&rel_row_idx_h, nnz * sizeof(*rel_row_idx_h)));
		gpuCudaErrorCheck(cudaMallocHost(&thread_block_i_s_h, num_blocks * sizeof(*thread_block_i_s_h)));
		gpuCudaErrorCheck(cudaMallocHost(&thread_block_i_e_h, num_blocks * sizeof(*thread_block_i_e_h)));
		gpuCudaErrorCheck(cudaMallocHost(&x_h, n * sizeof(*x_h)));
		gpuCudaErrorCheck(cudaMallocHost(&y_h, m * sizeof(*y_h)));

		memcpy(ia_h, ia, (m+1) * sizeof(*ia_h));
		memcpy(ja_h, ja, nnz * sizeof(*ja_h));
		memcpy(a_h, a, nnz * sizeof(*a_h));
		memcpy(rel_row_idx_h, rel_row_idx, nnz * sizeof(*rel_row_idx_h));
		memcpy(thread_block_i_s_h, thread_block_i_s, num_blocks * sizeof(*thread_block_i_s_h));
		memcpy(thread_block_i_e_h, thread_block_i_e, num_blocks * sizeof(*thread_block_i_e_h));

		if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(startEvent_memcpy_ia, stream));
		gpuCudaErrorCheck(cudaMemcpyAsync(ia_d, ia_h, (m+1) * sizeof(*ia_d), cudaMemcpyHostToDevice, stream));
		if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(endEvent_memcpy_ia, stream));

		if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(startEvent_memcpy_ja, stream));
		gpuCudaErrorCheck(cudaMemcpyAsync(ja_d, ja_h, nnz * sizeof(*ja_d), cudaMemcpyHostToDevice, stream));
		if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(endEvent_memcpy_ja, stream));

		if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(startEvent_memcpy_a, stream));
		gpuCudaErrorCheck(cudaMemcpyAsync(a_d, a_h, nnz * sizeof(*a_d), cudaMemcpyHostToDevice, stream));
		if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(endEvent_memcpy_a, stream));

		if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(startEvent_memcpy_rel_row_idx, stream));
		gpuCudaErrorCheck(cudaMemcpyAsync(rel_row_idx_d, rel_row_idx_h, nnz * sizeof(*rel_row_idx_d), cudaMemcpyHostToDevice, stream));
		if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(endEvent_memcpy_rel_row_idx, stream));

		if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(startEvent_memcpy_thread_block_i_s, stream));
		gpuCudaErrorCheck(cudaMemcpyAsync(thread_block_i_s_d, thread_block_i_s_h, num_blocks * sizeof(*thread_block_i_s_d), cudaMemcpyHostToDevice, stream));
		if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(endEvent_memcpy_thread_block_i_s, stream));

		if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(startEvent_memcpy_thread_block_i_e, stream));
		gpuCudaErrorCheck(cudaMemcpyAsync(thread_block_i_e_d, thread_block_i_e_h, num_blocks * sizeof(*thread_block_i_e_d), cudaMemcpyHostToDevice, stream));
		if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(endEvent_memcpy_thread_block_i_e, stream));

		if(TIME_IT){
			gpuCudaErrorCheck(cudaEventSynchronize(endEvent_memcpy_ia));
			gpuCudaErrorCheck(cudaEventSynchronize(endEvent_memcpy_ja));
			gpuCudaErrorCheck(cudaEventSynchronize(endEvent_memcpy_a));
			gpuCudaErrorCheck(cudaEventSynchronize(endEvent_memcpy_rel_row_idx));
			gpuCudaErrorCheck(cudaEventSynchronize(endEvent_memcpy_thread_block_i_s));
			gpuCudaErrorCheck(cudaEventSynchronize(endEvent_memcpy_thread_block_i_e));

			float memcpyTime_cuda_ia, memcpyTime_cuda_ja, memcpyTime_cuda_a, memcpyTime_cuda_rel_row_idx, memcpyTime_cuda_thread_block_i_s, memcpyTime_cuda_thread_block_i_e;
			gpuCudaErrorCheck(cudaEventElapsedTime(&memcpyTime_cuda_ia, startEvent_memcpy_ia, endEvent_memcpy_ia));
			gpuCudaErrorCheck(cudaEventElapsedTime(&memcpyTime_cuda_ja, startEvent_memcpy_ja, endEvent_memcpy_ja));
			gpuCudaErrorCheck(cudaEventElapsedTime(&memcpyTime_cuda_a, startEvent_memcpy_a, endEvent_memcpy_a));
			gpuCudaErrorCheck(cudaEventElapsedTime(&memcpyTime_cuda_rel_row_idx, startEvent_memcpy_rel_row_idx, endEvent_memcpy_rel_row_idx));
			gpuCudaErrorCheck(cudaEventElapsedTime(&memcpyTime_cuda_thread_block_i_s, startEvent_memcpy_thread_block_i_s, endEvent_memcpy_thread_block_i_s));
			gpuCudaErrorCheck(cudaEventElapsedTime(&memcpyTime_cuda_thread_block_i_e, startEvent_memcpy_thread_block_i_e, endEvent_memcpy_thread_block_i_e));
			printf("(CUDA) Memcpy ia time = %.4lf ms, ja time = %.4lf ms, a time = %.4lf ms, rel_row_idx time = %.4lf ms, thread_block_s = %.4lf ms, thread_block_e = %.4lf ms\n", memcpyTime_cuda_ia, memcpyTime_cuda_ja, memcpyTime_cuda_a, memcpyTime_cuda_rel_row_idx, memcpyTime_cuda_thread_block_i_s, memcpyTime_cuda_thread_block_i_e);
		}
	}

	~CSRArrays()
	{
		free(a);
		free(ia);
		free(ja);
		free(rel_row_idx);
		free(thread_block_i_s);
		free(thread_block_i_e);

		gpuCudaErrorCheck(cudaFree(ia_d));
		gpuCudaErrorCheck(cudaFree(ja_d));
		gpuCudaErrorCheck(cudaFree(a_d));
		gpuCudaErrorCheck(cudaFree(rel_row_idx_d));
		gpuCudaErrorCheck(cudaFree(thread_block_i_s_d));
		gpuCudaErrorCheck(cudaFree(thread_block_i_e_d));
		gpuCudaErrorCheck(cudaFree(x_d));
		gpuCudaErrorCheck(cudaFree(y_d));

		gpuCudaErrorCheck(cudaFreeHost(ia_h));
		gpuCudaErrorCheck(cudaFreeHost(ja_h));
		gpuCudaErrorCheck(cudaFreeHost(a_h));
		gpuCudaErrorCheck(cudaFreeHost(rel_row_idx_h));
		gpuCudaErrorCheck(cudaFreeHost(thread_block_i_s_h));
		gpuCudaErrorCheck(cudaFreeHost(thread_block_i_e_h));
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
			gpuCudaErrorCheck(cudaEventDestroy(startEvent_memcpy_rel_row_idx));
			gpuCudaErrorCheck(cudaEventDestroy(endEvent_memcpy_rel_row_idx));
			gpuCudaErrorCheck(cudaEventDestroy(startEvent_memcpy_thread_block_i_s));
			gpuCudaErrorCheck(cudaEventDestroy(endEvent_memcpy_thread_block_i_s));
			gpuCudaErrorCheck(cudaEventDestroy(startEvent_memcpy_thread_block_i_e));
			gpuCudaErrorCheck(cudaEventDestroy(endEvent_memcpy_thread_block_i_e));

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
csr_to_format(INT_T * row_ptr, INT_T * col_ind, ValueType * values, long m, long n, long nnz, int symmetric)
{
	if (symmetric)
		error("symmetric matrices not supported by this format, expand symmetry");
	struct CSRArrays * csr = new CSRArrays(row_ptr, col_ind, values, m, n, nnz);
	csr->mem_footprint = nnz * (sizeof(ValueType) + sizeof(INT_T)) + (m+1) * sizeof(INT_T);
	char *format_name;
	format_name = (char *)malloc(100*sizeof(char));
	snprintf(format_name, 100, "Custom_CSR_CUDA_BUFFER_t%d_rc_%d", csr->num_threads, csr->row_cluster_size);
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

	if (csr->x == NULL)
	{
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

	gpu_kernel_csr_basic<<<grid_dims, block_dims, (csr->row_cluster_size*block_size*sizeof(ValueType)), csr->stream>>>(thread_block_i_s_d, thread_block_i_e_d, csr->ia_d, csr->ja_d, csr->a_d, csr->x_d, csr->y_d, csr->rel_row_idx_d, csr->row_cluster_size);
	// gpu_kernel_csr_flat<<<grid_dims, block_dims>>>(thread_block_i_s_d, thread_block_i_e_d, csr->ia_d, csr->ja_d, csr->a_d, csr->x_d, csr->y_d);
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

