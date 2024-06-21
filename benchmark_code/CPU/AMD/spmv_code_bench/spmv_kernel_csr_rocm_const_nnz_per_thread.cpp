#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

#include "rocm/example_utils.hpp"

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
#ifdef __cplusplus
}
#endif

#ifndef NNZ_PER_THREAD
#define NNZ_PER_THREAD  4
#endif

#ifndef BLOCK_SIZE
#define BLOCK_SIZE  512
#endif

#ifndef TIME_IT
#define TIME_IT 0
#endif

extern int prefetch_distance;

double * thread_time_compute, * thread_time_barrier;

// void
// cuda_push_duplicate_base(void ** dst_ptr, void * src, long bytes)
// {
// 	cudaMalloc(dst_ptr, bytes);
// 	cudaMemcpy(*((char **) dst_ptr), src, bytes, cudaMemcpyHostToDevice);
// }
// #define cuda_push_duplicate(dst_ptr, src, bytes) cuda_push_duplicate_base((void **) dst_ptr, src, bytes)


struct CSRArrays : Matrix_Format
{
	INT_T * row_ptr;
	INT_T * ia;
	INT_T * ja;
	ValueType * a;
	INT_T * thread_block_i_s = NULL;
	INT_T * thread_block_i_e = NULL;
	INT_T * thread_block_j_s = NULL;
	INT_T * thread_block_j_e = NULL;

	INT_T * row_ptr_d;
	INT_T * ia_d;
	INT_T * ja_d;
	ValueType * a_d;
	INT_T * thread_block_i_s_d = NULL;
	INT_T * thread_block_i_e_d = NULL;
	INT_T * thread_block_j_s_d = NULL;
	INT_T * thread_block_j_e_d = NULL;

	INT_T * row_ptr_h;
	INT_T * ia_h;
	INT_T * ja_h;
	ValueType * a_h;
	INT_T * thread_block_i_s_h = NULL;
	INT_T * thread_block_i_e_h = NULL;
	INT_T * thread_block_j_s_h = NULL;
	INT_T * thread_block_j_e_h = NULL;

	// ValueType * multres_d;

	ValueType * x = NULL;
	ValueType * y = NULL;
	ValueType * x_d = NULL;
	ValueType * y_d = NULL;
	ValueType * x_h = NULL;
	ValueType * y_h = NULL;

	hipStream_t stream;
	// hipEvent_t is useful for timing, but for performance use " hipEventCreateWithFlags ( &event, cudaEventDisableTiming) "
	hipEvent_t startEvent_execution;
	hipEvent_t endEvent_execution;

	hipEvent_t startEvent_memcpy_row_ptr;
	hipEvent_t endEvent_memcpy_row_ptr;
	hipEvent_t startEvent_memcpy_ia;
	hipEvent_t endEvent_memcpy_ia;
	hipEvent_t startEvent_memcpy_ja;
	hipEvent_t endEvent_memcpy_ja;
	hipEvent_t startEvent_memcpy_a;
	hipEvent_t endEvent_memcpy_a;
	hipEvent_t startEvent_memcpy_thread_block_i_s;
	hipEvent_t endEvent_memcpy_thread_block_i_s;
	hipEvent_t startEvent_memcpy_thread_block_i_e;
	hipEvent_t endEvent_memcpy_thread_block_i_e;
	hipEvent_t startEvent_memcpy_thread_block_j_s;
	hipEvent_t endEvent_memcpy_thread_block_j_s;
	hipEvent_t startEvent_memcpy_thread_block_j_e;
	hipEvent_t endEvent_memcpy_thread_block_j_e;

	hipEvent_t startEvent_memcpy_x;
	hipEvent_t endEvent_memcpy_x;
	hipEvent_t startEvent_memcpy_y;
	hipEvent_t endEvent_memcpy_y;

	int max_smem_per_block, multiproc_count, max_threads_per_block, warp_size, max_threads_per_multiproc, max_block_dim_x, max_persistent_l2_cache, max_num_threads;
	int num_threads;
	int block_size;
	int num_blocks;

	CSRArrays(INT_T * row_ptr, INT_T * ja, ValueType * a, long m, long n, long nnz) : Matrix_Format(m, n, nnz), row_ptr(row_ptr), ja(ja), a(a)
	{
		double time_balance;
		long i;

		HIP_CHECK(hipDeviceGetAttribute(&max_smem_per_block, hipDeviceAttributeMaxSharedMemoryPerBlock, 0));
		HIP_CHECK(hipDeviceGetAttribute(&multiproc_count, hipDeviceAttributeMultiprocessorCount, 0));
		HIP_CHECK(hipDeviceGetAttribute(&max_threads_per_block, hipDeviceAttributeMaxThreadsPerBlock , 0));
		HIP_CHECK(hipDeviceGetAttribute(&warp_size, hipDeviceAttributeWarpSize , 0));
		HIP_CHECK(hipDeviceGetAttribute(&max_threads_per_multiproc, hipDeviceAttributeMaxThreadsPerMultiProcessor, 0));
		HIP_CHECK(hipDeviceGetAttribute(&max_block_dim_x, hipDeviceAttributeMaxBlockDimX, 0));
		// HIP_CHECK(hipDeviceGetAttribute(&max_persistent_l2_cache, cudaDevAttrMaxPersistingL2CacheSize, 0));
		max_num_threads = max_threads_per_multiproc * multiproc_count;
		printf("max_smem_per_block(bytes)=%d\n", max_smem_per_block);
		printf("multiproc_count=%d\n", multiproc_count);
		printf("max_threads_per_block=%d\n", max_threads_per_block);
		printf("warp_size=%d\n", warp_size);
		printf("max_threads_per_multiproc=%d\n", max_threads_per_multiproc);
		printf("max_block_dim_x=%d\n", max_block_dim_x);
		// printf("max_persistent_l2_cache=%d\n", max_persistent_l2_cache);
		printf("max_num_threads=%d\n", max_num_threads);

		block_size = BLOCK_SIZE;

		num_threads = (nnz + NNZ_PER_THREAD - 1) / NNZ_PER_THREAD;

		num_threads = ((num_threads + BLOCK_SIZE - 1) / BLOCK_SIZE) * BLOCK_SIZE;

		num_blocks = num_threads / BLOCK_SIZE;

		printf("num_threads=%d, block_size=%d, num_blocks=%d\n", num_threads, BLOCK_SIZE, num_blocks);

		thread_block_i_s = (INT_T *) malloc(num_blocks * sizeof(*thread_block_i_s));
		thread_block_i_e = (INT_T *) malloc(num_blocks * sizeof(*thread_block_i_e));
		thread_block_j_s = (INT_T *) malloc(num_blocks * sizeof(*thread_block_j_s));
		thread_block_j_e = (INT_T *) malloc(num_blocks * sizeof(*thread_block_j_e));
		time_balance = time_it(1,
			long lower_boundary;
			// for (i=0;i<num_blocks;i++)
			// {
				// loop_partitioner_balance_iterations(num_blocks, i, 0, nnz, &thread_block_j_s[i], &thread_block_j_e[i]);
				// macros_binary_search(row_ptr, 0, m, thread_block_j_s[i], &lower_boundary, NULL);           // Index boundaries are inclusive.
				// thread_block_i_s[i] = lower_boundary;
			// }
			long nnz_per_block = BLOCK_SIZE * NNZ_PER_THREAD;
			for (i=0;i<num_blocks;i++)
			{
				thread_block_j_s[i] = nnz_per_block * i;
				thread_block_j_e[i] = nnz_per_block * (i+ 1);
				if (thread_block_j_s[i] > nnz)
					thread_block_j_s[i] = nnz;
				if (thread_block_j_e[i] > nnz)
					thread_block_j_e[i] = nnz;
				macros_binary_search(row_ptr, 0, m, thread_block_j_s[i], &lower_boundary, NULL);           // Index boundaries are inclusive.
				thread_block_i_s[i] = lower_boundary;
			}
			for (i=0;i<num_blocks;i++)
			{
				if (i == num_blocks - 1)   // If we calculate each thread's boundaries individually some empty rows might be unassigned.
					thread_block_i_e[i] = m;
				else
					thread_block_i_e[i] = thread_block_i_s[i+1] + 1;
				if ((thread_block_j_s[i] >= row_ptr[thread_block_i_e[i]]) || (thread_block_j_s[i] < row_ptr[thread_block_i_s[i]]))
					error("bad binary search of row start: i=%d j:[%d, %d] j=%d", thread_block_i_s[i], row_ptr[thread_block_i_s[i]], row_ptr[thread_block_i_e[i]], thread_block_j_s[i]);
			}
		);
		printf("balance time = %g\n", time_balance);

		ia = (typeof(ia)) malloc(nnz * sizeof(*ia));
		_Pragma("omp parallel")
		{
			long i, j;
			_Pragma("omp for")
			for (i=0;i<m;i++)
			{
				for (j=row_ptr[i];j<row_ptr[i+1];j++)
				{
					ia[j] = i;
				}
			}
		}

		_Pragma("omp parallel")
		{
			long i, j;
			_Pragma("omp for")
			for (j=0;j<nnz;j+=32*NNZ_PER_THREAD)
			{
				long j_e = j + 32*NNZ_PER_THREAD;
				if (j_e > nnz)
					j_e = nnz;
				if (ia[j] == ia[j_e-1])
				{
					for (i=j;i<j_e;i++)
					{
						ja[i] = ja[i] | 0x80000000;
					}
				}
			}
		}

		// cuda_push_duplicate(&row_ptr_d, row_ptr, (m+1) * sizeof(*row_ptr_d));
		// cuda_push_duplicate(&ia_d, ia, nnz * sizeof(*ia_d));
		// cuda_push_duplicate(&ja_d, ja, nnz * sizeof(*ja_d));
		// cuda_push_duplicate(&a_d, a, nnz * sizeof(*a_d));
		// cudaMalloc(&multres_d, nnz * sizeof(*y_d));

		// cuda_push_duplicate(&thread_block_i_s_d, thread_block_i_s, num_blocks * sizeof(*thread_block_i_s_d));
		// cuda_push_duplicate(&thread_block_i_e_d, thread_block_i_e, num_blocks * sizeof(*thread_block_i_e_d));
		// cuda_push_duplicate(&thread_block_j_s_d, thread_block_j_s, num_blocks * sizeof(*thread_block_j_s_d));
		// cuda_push_duplicate(&thread_block_j_e_d, thread_block_j_e, num_blocks * sizeof(*thread_block_j_e_d));

		HIP_CHECK(hipMalloc((void**)&row_ptr_d, (m+1) * sizeof(*row_ptr_d)));
		HIP_CHECK(hipMalloc((void**)&ia_d, nnz * sizeof(*ia_d)));
		HIP_CHECK(hipMalloc((void**)&ja_d, nnz * sizeof(*ja_d)));
		HIP_CHECK(hipMalloc((void**)&a_d, nnz * sizeof(*a_d)));
		HIP_CHECK(hipMalloc((void**)&thread_block_i_s_d, num_blocks * sizeof(*thread_block_i_s_d)));
		HIP_CHECK(hipMalloc((void**)&thread_block_i_e_d, num_blocks * sizeof(*thread_block_i_e_d)));
		HIP_CHECK(hipMalloc((void**)&thread_block_j_s_d, num_blocks * sizeof(*thread_block_j_s_d)));
		HIP_CHECK(hipMalloc((void**)&thread_block_j_e_d, num_blocks * sizeof(*thread_block_j_e_d)));
		HIP_CHECK(hipMalloc((void**)&x_d, n * sizeof(*x_d)));
		HIP_CHECK(hipMalloc((void**)&y_d, m * sizeof(*y_d)));

		HIP_CHECK(hipStreamCreate(&stream));

		// rocm events for timing measurements
		HIP_CHECK(hipEventCreate(&startEvent_execution));
		HIP_CHECK(hipEventCreate(&endEvent_execution));

		if(TIME_IT){
			HIP_CHECK(hipEventCreate(&startEvent_memcpy_row_ptr));
			HIP_CHECK(hipEventCreate(&endEvent_memcpy_row_ptr));
			HIP_CHECK(hipEventCreate(&startEvent_memcpy_ia));
			HIP_CHECK(hipEventCreate(&endEvent_memcpy_ia));
			HIP_CHECK(hipEventCreate(&startEvent_memcpy_ja));
			HIP_CHECK(hipEventCreate(&endEvent_memcpy_ja));
			HIP_CHECK(hipEventCreate(&startEvent_memcpy_a));
			HIP_CHECK(hipEventCreate(&endEvent_memcpy_a));
			HIP_CHECK(hipEventCreate(&startEvent_memcpy_thread_block_i_s));
			HIP_CHECK(hipEventCreate(&endEvent_memcpy_thread_block_i_s));
			HIP_CHECK(hipEventCreate(&startEvent_memcpy_thread_block_i_e));
			HIP_CHECK(hipEventCreate(&endEvent_memcpy_thread_block_i_e));
			HIP_CHECK(hipEventCreate(&startEvent_memcpy_thread_block_j_s));
			HIP_CHECK(hipEventCreate(&endEvent_memcpy_thread_block_j_s));
			HIP_CHECK(hipEventCreate(&startEvent_memcpy_thread_block_j_e));
			HIP_CHECK(hipEventCreate(&endEvent_memcpy_thread_block_j_e));

			HIP_CHECK(hipEventCreate(&startEvent_memcpy_x));
			HIP_CHECK(hipEventCreate(&endEvent_memcpy_x));
			HIP_CHECK(hipEventCreate(&startEvent_memcpy_y));
			HIP_CHECK(hipEventCreate(&endEvent_memcpy_y));
		}

		HIP_CHECK(hipHostMalloc((void**)&row_ptr_h, (m+1) * sizeof(*row_ptr_h)));
		HIP_CHECK(hipHostMalloc((void**)&ia_h, nnz * sizeof(*ia_h)));
		HIP_CHECK(hipHostMalloc((void**)&ja_h, nnz * sizeof(*ja_h)));
		HIP_CHECK(hipHostMalloc((void**)&a_h, nnz * sizeof(*a_h)));
		HIP_CHECK(hipHostMalloc((void**)&thread_block_i_s_h, num_blocks * sizeof(*thread_block_i_s_h)));
		HIP_CHECK(hipHostMalloc((void**)&thread_block_i_e_h, num_blocks * sizeof(*thread_block_i_e_h)));
		HIP_CHECK(hipHostMalloc((void**)&thread_block_j_s_h, num_blocks * sizeof(*thread_block_j_s_h)));
		HIP_CHECK(hipHostMalloc((void**)&thread_block_j_e_h, num_blocks * sizeof(*thread_block_j_e_h)));
		HIP_CHECK(hipHostMalloc((void**)&x_h, n * sizeof(*x_h)));
		HIP_CHECK(hipHostMalloc((void**)&y_h, m * sizeof(*y_h)));

		memcpy(row_ptr_h, row_ptr, (m + 1) * sizeof(*row_ptr_h));
		memcpy(ia_h, ia, nnz * sizeof(*ia_h));
		memcpy(ja_h, ja, nnz * sizeof(*ja_h));
		memcpy(a_h, a, nnz * sizeof(*a_h));
		memcpy(thread_block_i_s_h, thread_block_i_s, num_blocks * sizeof(*thread_block_i_s_h));
		memcpy(thread_block_i_e_h, thread_block_i_e, num_blocks * sizeof(*thread_block_i_e_h));
		memcpy(thread_block_j_s_h, thread_block_j_s, num_blocks * sizeof(*thread_block_j_s_h));
		memcpy(thread_block_j_e_h, thread_block_j_e, num_blocks * sizeof(*thread_block_j_e_h));

		if(TIME_IT) HIP_CHECK(hipEventRecord(startEvent_memcpy_row_ptr, stream));
		HIP_CHECK(hipMemcpyAsync(row_ptr_d, row_ptr_h, (m+1) * sizeof(*row_ptr_d), hipMemcpyHostToDevice, stream));
		if(TIME_IT) HIP_CHECK(hipEventRecord(endEvent_memcpy_row_ptr, stream));

		if(TIME_IT) HIP_CHECK(hipEventRecord(startEvent_memcpy_ia, stream));
		HIP_CHECK(hipMemcpyAsync(ia_d, ia_h, nnz * sizeof(*ia_d), hipMemcpyHostToDevice, stream));
		if(TIME_IT) HIP_CHECK(hipEventRecord(endEvent_memcpy_ia, stream));

		if(TIME_IT) HIP_CHECK(hipEventRecord(startEvent_memcpy_ja, stream));
		HIP_CHECK(hipMemcpyAsync(ja_d, ja_h, nnz * sizeof(*ja_d), hipMemcpyHostToDevice, stream));
		if(TIME_IT) HIP_CHECK(hipEventRecord(endEvent_memcpy_ja, stream));

		if(TIME_IT) HIP_CHECK(hipEventRecord(startEvent_memcpy_a, stream));
		HIP_CHECK(hipMemcpyAsync(a_d, a_h, nnz * sizeof(*a_d), hipMemcpyHostToDevice, stream));
		if(TIME_IT) HIP_CHECK(hipEventRecord(endEvent_memcpy_a, stream));

		if(TIME_IT) HIP_CHECK(hipEventRecord(startEvent_memcpy_thread_block_i_s, stream));
		HIP_CHECK(hipMemcpyAsync(thread_block_i_s_d, thread_block_i_s_h, num_blocks * sizeof(*thread_block_i_s_d), hipMemcpyHostToDevice, stream));
		if(TIME_IT) HIP_CHECK(hipEventRecord(endEvent_memcpy_thread_block_i_s, stream));

		if(TIME_IT) HIP_CHECK(hipEventRecord(startEvent_memcpy_thread_block_i_e, stream));
		HIP_CHECK(hipMemcpyAsync(thread_block_i_e_d, thread_block_i_e_h, num_blocks * sizeof(*thread_block_i_e_d), hipMemcpyHostToDevice, stream));
		if(TIME_IT) HIP_CHECK(hipEventRecord(endEvent_memcpy_thread_block_i_e, stream));

		if(TIME_IT) HIP_CHECK(hipEventRecord(startEvent_memcpy_thread_block_j_s, stream));
		HIP_CHECK(hipMemcpyAsync(thread_block_j_s_d, thread_block_j_s_h, num_blocks * sizeof(*thread_block_j_s_d), hipMemcpyHostToDevice, stream));
		if(TIME_IT) HIP_CHECK(hipEventRecord(endEvent_memcpy_thread_block_j_s, stream));

		if(TIME_IT) HIP_CHECK(hipEventRecord(startEvent_memcpy_thread_block_j_e, stream));
		HIP_CHECK(hipMemcpyAsync(thread_block_j_e_d, thread_block_j_e_h, num_blocks * sizeof(*thread_block_j_e_d), hipMemcpyHostToDevice, stream));
		if(TIME_IT) HIP_CHECK(hipEventRecord(endEvent_memcpy_thread_block_j_e, stream));

		if(TIME_IT){
			HIP_CHECK(hipEventSynchronize(endEvent_memcpy_ia));
			HIP_CHECK(hipEventSynchronize(endEvent_memcpy_row_ptr));
			HIP_CHECK(hipEventSynchronize(endEvent_memcpy_ja));
			HIP_CHECK(hipEventSynchronize(endEvent_memcpy_a));
			HIP_CHECK(hipEventSynchronize(endEvent_memcpy_thread_block_i_s));
			HIP_CHECK(hipEventSynchronize(endEvent_memcpy_thread_block_i_e));
			HIP_CHECK(hipEventSynchronize(endEvent_memcpy_thread_block_j_s));
			HIP_CHECK(hipEventSynchronize(endEvent_memcpy_thread_block_j_e));

			float memcpyTime_rocm_row_ptr, memcpyTime_rocm_ia, memcpyTime_rocm_ja, memcpyTime_rocm_a, memcpyTime_rocm_thread_block_i_s, memcpyTime_rocm_thread_block_i_e, memcpyTime_rocm_thread_block_j_s, memcpyTime_rocm_thread_block_j_e;
			HIP_CHECK(hipEventElapsedTime(&memcpyTime_rocm_row_ptr, startEvent_memcpy_row_ptr, endEvent_memcpy_row_ptr));
			HIP_CHECK(hipEventElapsedTime(&memcpyTime_rocm_ia, startEvent_memcpy_ia, endEvent_memcpy_ia));
			HIP_CHECK(hipEventElapsedTime(&memcpyTime_rocm_ja, startEvent_memcpy_ja, endEvent_memcpy_ja));
			HIP_CHECK(hipEventElapsedTime(&memcpyTime_rocm_a, startEvent_memcpy_a, endEvent_memcpy_a));
			HIP_CHECK(hipEventElapsedTime(&memcpyTime_rocm_thread_block_i_s, startEvent_memcpy_thread_block_i_s, endEvent_memcpy_thread_block_i_s));
			HIP_CHECK(hipEventElapsedTime(&memcpyTime_rocm_thread_block_i_e, startEvent_memcpy_thread_block_i_e, endEvent_memcpy_thread_block_i_e));
			HIP_CHECK(hipEventElapsedTime(&memcpyTime_rocm_thread_block_j_s, startEvent_memcpy_thread_block_j_s, endEvent_memcpy_thread_block_j_s));
			HIP_CHECK(hipEventElapsedTime(&memcpyTime_rocm_thread_block_j_e, startEvent_memcpy_thread_block_j_e, endEvent_memcpy_thread_block_j_e));
			printf("(ROCM) Memcpy row_ptr time = %.4lf ms, ia time = %.4lf ms, ja time = %.4lf ms, a time = %.4lf ms, thread_block_i_s time = %.4lf, thread_block_i_e time = %.4lf, thread_block_j_s time = %.4lf, thread_block_j_e time = %.4lf\n", memcpyTime_rocm_row_ptr, memcpyTime_rocm_ia, memcpyTime_rocm_ja, memcpyTime_rocm_a, memcpyTime_rocm_thread_block_i_s, memcpyTime_rocm_thread_block_i_e, memcpyTime_rocm_thread_block_j_s, memcpyTime_rocm_thread_block_j_e);
		}
		HIP_CHECK(hipStreamSynchronize(stream));

		_Pragma("omp parallel")
		{
			long j;
			_Pragma("omp for")
			for (j=0;j<nnz;j++)
			{
				ja[j] = ja[j] & 0x7FFFFFFF;
			}
		}
	}

	~CSRArrays()
	{
		free(a);
		free(row_ptr);
		free(ia);
		free(ja);
		free(thread_block_i_s);
		free(thread_block_i_e);
		free(thread_block_j_s);
		free(thread_block_j_e);

		HIP_CHECK(hipFree(row_ptr_d));
		HIP_CHECK(hipFree(ia_d));
		HIP_CHECK(hipFree(ja_d));
		HIP_CHECK(hipFree(a_d));
		// HIP_CHECK(hipFree(multres_d));
		HIP_CHECK(hipFree(thread_block_i_s_d));
		HIP_CHECK(hipFree(thread_block_i_e_d));
		HIP_CHECK(hipFree(thread_block_j_s_d));
		HIP_CHECK(hipFree(thread_block_j_e_d));
		HIP_CHECK(hipFree(x_d));
		HIP_CHECK(hipFree(y_d));

		HIP_CHECK(hipHostFree(row_ptr_h));
		HIP_CHECK(hipHostFree(ia_h));
		HIP_CHECK(hipHostFree(ja_h));
		HIP_CHECK(hipHostFree(a_h));
		// HIP_CHECK(hipHostFree(multres_h));
		HIP_CHECK(hipHostFree(thread_block_i_s_h));
		HIP_CHECK(hipHostFree(thread_block_i_e_h));
		HIP_CHECK(hipHostFree(thread_block_j_s_h));
		HIP_CHECK(hipHostFree(thread_block_j_e_h));
		HIP_CHECK(hipHostFree(x_h));
		HIP_CHECK(hipHostFree(y_h));

		HIP_CHECK(hipStreamDestroy(stream));

		HIP_CHECK(hipEventDestroy(startEvent_execution));
		HIP_CHECK(hipEventDestroy(endEvent_execution));

		if(TIME_IT){
			HIP_CHECK(hipEventDestroy(startEvent_memcpy_x));
			HIP_CHECK(hipEventDestroy(endEvent_memcpy_x));
			HIP_CHECK(hipEventDestroy(startEvent_memcpy_y));
			HIP_CHECK(hipEventDestroy(endEvent_memcpy_y));

			HIP_CHECK(hipEventDestroy(startEvent_memcpy_row_ptr));
			HIP_CHECK(hipEventDestroy(endEvent_memcpy_row_ptr));
			HIP_CHECK(hipEventDestroy(startEvent_memcpy_ia));
			HIP_CHECK(hipEventDestroy(endEvent_memcpy_ia));
			HIP_CHECK(hipEventDestroy(startEvent_memcpy_ja));
			HIP_CHECK(hipEventDestroy(endEvent_memcpy_ja));
			HIP_CHECK(hipEventDestroy(startEvent_memcpy_a));
			HIP_CHECK(hipEventDestroy(endEvent_memcpy_a));
			HIP_CHECK(hipEventDestroy(startEvent_memcpy_thread_block_i_s));
			HIP_CHECK(hipEventDestroy(endEvent_memcpy_thread_block_i_s));
			HIP_CHECK(hipEventDestroy(startEvent_memcpy_thread_block_i_e));
			HIP_CHECK(hipEventDestroy(endEvent_memcpy_thread_block_i_e));
			HIP_CHECK(hipEventDestroy(startEvent_memcpy_thread_block_j_s));
			HIP_CHECK(hipEventDestroy(endEvent_memcpy_thread_block_j_s));
			HIP_CHECK(hipEventDestroy(startEvent_memcpy_thread_block_j_e));
			HIP_CHECK(hipEventDestroy(endEvent_memcpy_thread_block_j_e));
		}
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
	snprintf(format_name, 100, "Custom_CSR_ROCM_constant_nnz_per_thread_b%d_nnz%d", BLOCK_SIZE, NNZ_PER_THREAD);
	csr->format_name = format_name;
	return csr;
}


//==========================================================================================================================================
//= CSR Custom
//==========================================================================================================================================
// This has to be written elsewhere. It is compiled with hipcc (clang compiler) and then linked together with this!
extern "C" void launch_kernel_wrapper(dim3 grid_dims, dim3 block_dims, long shared_mem_size, hipStream_t stream, INT_T * thread_block_i_s_d, INT_T * thread_block_i_e_d, INT_T * thread_block_j_s_d, INT_T * thread_block_j_e_d, INT_T * row_ptr_d, INT_T * ia_d, INT_T * ja_d, ValueType * a_d, INT_T m, INT_T n, INT_T nnz, ValueType * x_d, ValueType * y_d);

void
compute_csr(CSRArrays * restrict csr, ValueType * restrict x, ValueType * restrict y)
{
	dim3 block_dims(BLOCK_SIZE);
	dim3 grid_dims(csr->num_blocks);
	// long shared_mem_size = BLOCK_SIZE * (sizeof(ValueType));
	// long shared_mem_size = BLOCK_SIZE * (sizeof(ValueType) + sizeof(INT_T));
	long shared_mem_size = 0;

	if (csr->x == NULL)
	{
		printf("Grid : {%d, %d, %d} blocks. Blocks : {%d, %d, %d} threads.\n", grid_dims.x, grid_dims.y, grid_dims.z, block_dims.x, block_dims.y, block_dims.z);
		csr->x = x;
		if(TIME_IT) HIP_CHECK(hipEventRecord(csr->startEvent_memcpy_x, csr->stream));
		memcpy(csr->x_h, x, csr->n * sizeof(ValueType));
		HIP_CHECK(hipMemcpyAsync(csr->x_d, csr->x_h, csr->n * sizeof(*csr->x_d), hipMemcpyHostToDevice, csr->stream));
		if(TIME_IT) HIP_CHECK(hipEventRecord(csr->endEvent_memcpy_x, csr->stream));
		if(TIME_IT){
			HIP_CHECK(hipEventSynchronize(csr->endEvent_memcpy_x));
			float memcpyTime_rocm;
			HIP_CHECK(hipEventElapsedTime(&memcpyTime_rocm, csr->startEvent_memcpy_x, csr->endEvent_memcpy_x));
			printf("(ROCM) Memcpy x time = %.4lf ms\n", memcpyTime_rocm);
		}
	}

	HIP_CHECK(hipMemsetAsync(csr->y_d, 0, csr->m * sizeof(csr->y_d), csr->stream));

	launch_kernel_wrapper(grid_dims, block_dims, shared_mem_size, csr->stream, csr->thread_block_i_s_d, csr->thread_block_i_e_d, csr->thread_block_j_s_d, csr->thread_block_j_e_d, csr->row_ptr_d, csr->ia_d, csr->ja_d, csr->a_d, csr->m, csr->n, csr->nnz, csr->x_d, csr->y_d);
	HIP_CHECK(hipDeviceSynchronize());

	if (csr->y == NULL)
	{
		csr->y = y;
		if(TIME_IT) HIP_CHECK(hipEventRecord(csr->startEvent_memcpy_y, csr->stream));
		HIP_CHECK(hipMemcpyAsync(csr->y_h, csr->y_d, csr->m * sizeof(*csr->y_d), hipMemcpyDeviceToHost, csr->stream));
		HIP_CHECK(hipStreamSynchronize(csr->stream));
		memcpy(y, csr->y_h, csr->m * sizeof(ValueType));
		if(TIME_IT) HIP_CHECK(hipEventRecord(csr->endEvent_memcpy_y, csr->stream));

		if(TIME_IT){
			HIP_CHECK(hipEventSynchronize(csr->endEvent_memcpy_y));
			float memcpyTime_rocm;
			HIP_CHECK(hipEventElapsedTime(&memcpyTime_rocm, csr->startEvent_memcpy_y, csr->endEvent_memcpy_y));
			printf("(ROCM) Memcpy y time = %.4lf ms\n", memcpyTime_rocm);
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

