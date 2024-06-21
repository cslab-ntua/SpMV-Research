#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

#include "rocm/example_utils.hpp"
#include "rocm/rocsparse_utils.hpp"

#include "hip/hip_runtime.h"
#include "rocsparse/rocsparse.h"

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

extern int prefetch_distance;

#if DOUBLE == 0
	#define ValueTypeRocm  rocsparse_datatype_f32_r
#elif DOUBLE == 1
	#define ValueTypeRocm  rocsparse_datatype_f64_r
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

	INT_T * ia_d;
	INT_T * ja_d;
	ValueType * a_d;

	INT_T * ia_h;
	INT_T * ja_h;
	ValueType * a_h;

	rocsparse_handle handle;
	rocsparse_mat_descr matA;
	rocsparse_hyb_mat hyb_matA;

	ValueType * x = NULL;
	ValueType * y = NULL;
	ValueType * x_d = NULL;
	ValueType * y_d = NULL;
	ValueType * x_h = NULL;
	ValueType * y_h = NULL;

	hipStream_t stream;
	// hipEvent_t is useful for timing, but for performance use " hipEventCreateWithFlags ( &event, hipEventDisableTiming) "
	hipEvent_t startEvent_execution;
	hipEvent_t endEvent_execution;

	hipEvent_t startEvent_memcpy_x;
	hipEvent_t endEvent_memcpy_x;
	hipEvent_t startEvent_memcpy_y;
	hipEvent_t endEvent_memcpy_y;

	hipEvent_t startEvent_memcpy_ia;
	hipEvent_t endEvent_memcpy_ia;
	hipEvent_t startEvent_memcpy_ja;
	hipEvent_t endEvent_memcpy_ja;
	hipEvent_t startEvent_memcpy_a;
	hipEvent_t endEvent_memcpy_a;

	hipEvent_t startEvent_create_matA;
	hipEvent_t endEvent_create_matA;

	// int max_persistent_l2_cache;

	CSRArrays(INT_T * ia, INT_T * ja, ValueType * a, long m, long n, long nnz) : Matrix_Format(m, n, nnz), ia(ia), ja(ja), a(a)
	{
		// int max_smem_per_block, multiproc_count, max_threads_per_block, warp_size, max_threads_per_multiproc;
		// gpuCudaErrorCheck(cudaDeviceGetAttribute(&max_smem_per_block, cudaDevAttrMaxSharedMemoryPerBlock, 0));
		// gpuCudaErrorCheck(cudaDeviceGetAttribute(&multiproc_count, cudaDevAttrMultiProcessorCount, 0));
		// gpuCudaErrorCheck(cudaDeviceGetAttribute(&max_threads_per_block, cudaDevAttrMaxThreadsPerBlock , 0));
		// gpuCudaErrorCheck(cudaDeviceGetAttribute(&warp_size, cudaDevAttrWarpSize , 0));
		// gpuCudaErrorCheck(cudaDeviceGetAttribute(&max_threads_per_multiproc, cudaDevAttrMaxThreadsPerMultiProcessor, 0));
		// gpuCudaErrorCheck(cudaDeviceGetAttribute(&max_persistent_l2_cache, cudaDevAttrMaxPersistingL2CacheSize, 0));
		// printf("max_smem_per_block=%d\n", max_smem_per_block);
		// printf("multiproc_count=%d\n", multiproc_count);
		// printf("max_threads_per_block=%d\n", max_threads_per_block);
		// printf("warp_size=%d\n", warp_size);
		// printf("max_threads_per_multiproc=%d\n", max_threads_per_multiproc);
		
		HIP_CHECK(hipMalloc((void**)&ia_d, (m+1) * sizeof(*ia_d)));
		HIP_CHECK(hipMalloc((void**)&ja_d, nnz * sizeof(*ja_d)));
		HIP_CHECK(hipMalloc((void**)&a_d, nnz * sizeof(*a_d)));
		HIP_CHECK(hipMalloc((void**)&x_d, n * sizeof(*x_d)));
		HIP_CHECK(hipMalloc((void**)&y_d, m * sizeof(*y_d)));

		HIP_CHECK(hipStreamCreate(&stream));
		ROCSPARSE_CHECK(rocsparse_create_handle(&handle));
		ROCSPARSE_CHECK(rocsparse_set_stream(handle, stream));

		// rocm events for timing measurements
		HIP_CHECK(hipEventCreate(&startEvent_execution));
		HIP_CHECK(hipEventCreate(&endEvent_execution));

		if(TIME_IT){
			HIP_CHECK(hipEventCreate(&startEvent_memcpy_ia));
			HIP_CHECK(hipEventCreate(&endEvent_memcpy_ia));
			HIP_CHECK(hipEventCreate(&startEvent_memcpy_ja));
			HIP_CHECK(hipEventCreate(&endEvent_memcpy_ja));
			HIP_CHECK(hipEventCreate(&startEvent_memcpy_a));
			HIP_CHECK(hipEventCreate(&endEvent_memcpy_a));
			HIP_CHECK(hipEventCreate(&startEvent_create_matA));
			HIP_CHECK(hipEventCreate(&endEvent_create_matA));

			HIP_CHECK(hipEventCreate(&startEvent_memcpy_x));
			HIP_CHECK(hipEventCreate(&endEvent_memcpy_x));
			HIP_CHECK(hipEventCreate(&startEvent_memcpy_y));
			HIP_CHECK(hipEventCreate(&endEvent_memcpy_y));
		}

		HIP_CHECK(hipHostMalloc((void**)&ia_h, (m+1) * sizeof(*ia_h)));
		HIP_CHECK(hipHostMalloc((void**)&ja_h, nnz * sizeof(*ja_h)));
		HIP_CHECK(hipHostMalloc((void**)&a_h, nnz * sizeof(*a_h)));
		HIP_CHECK(hipHostMalloc((void**)&x_h, n * sizeof(*x_h)));
		HIP_CHECK(hipHostMalloc((void**)&y_h, m * sizeof(*y_h)));

		memcpy(ia_h, ia, (m+1) * sizeof(*ia_h));
		memcpy(ja_h, ja, nnz * sizeof(*ja_h));
		memcpy(a_h, a, nnz * sizeof(*a_h));

		if(TIME_IT) HIP_CHECK(hipEventRecord(startEvent_memcpy_ia));
		HIP_CHECK(hipMemcpyAsync(ia_d, ia_h, (m+1) * sizeof(*ia_d), hipMemcpyHostToDevice, stream));
		if(TIME_IT) HIP_CHECK(hipEventRecord(endEvent_memcpy_ia));

		if(TIME_IT) HIP_CHECK(hipEventRecord(startEvent_memcpy_ja));
		HIP_CHECK(hipMemcpyAsync(ja_d, ja_h, nnz * sizeof(*ja_d), hipMemcpyHostToDevice, stream));
		if(TIME_IT) HIP_CHECK(hipEventRecord(endEvent_memcpy_ja));

		if(TIME_IT) HIP_CHECK(hipEventRecord(startEvent_memcpy_a));
		HIP_CHECK(hipMemcpyAsync(a_d, a_h, nnz * sizeof(*a_d), hipMemcpyHostToDevice, stream));
		if(TIME_IT) HIP_CHECK(hipEventRecord(endEvent_memcpy_a));

		// Create sparse matrix A in HYB format
		if(TIME_IT) HIP_CHECK(hipEventRecord(startEvent_create_matA));
		ROCSPARSE_CHECK(rocsparse_create_mat_descr(&matA));
		ROCSPARSE_CHECK(rocsparse_create_hyb_mat(&hyb_matA));
		ROCSPARSE_CHECK(rocsparse_dcsr2hyb(handle, m, n, matA, a_d, ia_d, ja_d, hyb_matA, 0, rocsparse_hyb_partition_auto));
		if(TIME_IT) HIP_CHECK(hipEventRecord(endEvent_create_matA));

		if(TIME_IT){
			HIP_CHECK(hipEventSynchronize(endEvent_memcpy_ia));
			HIP_CHECK(hipEventSynchronize(endEvent_memcpy_ja));
			HIP_CHECK(hipEventSynchronize(endEvent_memcpy_a));
			HIP_CHECK(hipEventSynchronize(endEvent_create_matA));

			float memcpyTime_rocm_ia, memcpyTime_rocm_ja, memcpyTime_rocm_a, create_matA_Time;//memcpyTime_rocm_thread_i_e;
			HIP_CHECK(hipEventElapsedTime(&memcpyTime_rocm_ia, startEvent_memcpy_ia, endEvent_memcpy_ia));
			HIP_CHECK(hipEventElapsedTime(&memcpyTime_rocm_ja, startEvent_memcpy_ja, endEvent_memcpy_ja));
			HIP_CHECK(hipEventElapsedTime(&memcpyTime_rocm_a, startEvent_memcpy_a, endEvent_memcpy_a));
			HIP_CHECK(hipEventElapsedTime(&create_matA_Time, startEvent_create_matA, endEvent_create_matA));
			printf("(ROCM) Memcpy ia time = %.4lf ms, ja time = %.4lf ms, a time = %.4lf ms, matA time = %.4lf ms\n", memcpyTime_rocm_ia, memcpyTime_rocm_ja, memcpyTime_rocm_a, create_matA_Time);
		}
	}

	~CSRArrays()
	{
		free(a);
		free(ia);
		free(ja);

		// destroy matrix/vector descriptors
		ROCSPARSE_CHECK(rocsparse_destroy_mat_descr(matA));
		ROCSPARSE_CHECK(rocsparse_destroy_hyb_mat(hyb_matA));
		ROCSPARSE_CHECK(rocsparse_destroy_handle(handle));

		HIP_CHECK(hipFree(ia_d));
		HIP_CHECK(hipFree(ja_d));
		HIP_CHECK(hipFree(a_d));
		HIP_CHECK(hipFree(x_d));
		HIP_CHECK(hipFree(y_d));

		HIP_CHECK(hipHostFree(ia_h));
		HIP_CHECK(hipHostFree(ja_h));
		HIP_CHECK(hipHostFree(a_h));
		HIP_CHECK(hipHostFree(x_h));
		HIP_CHECK(hipHostFree(y_h));

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
	csr->format_name = (char *) "ROCSPARSE_HYB";
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

		// #ifdef PERSISTENT_L2_PREFETCH
		// 	int x_d_size = csr->n * sizeof(*csr->x);
		// 	gpuCudaErrorCheck(cudaCtxResetPersistingL2Cache()); // This needs to happen every time before running kernel for 1st time for a matrix...
		// 	if(x_d_size < csr->max_persistent_l2_cache){
		// 		cudaStreamAttrValue attribute;
		// 		auto &window = attribute.accessPolicyWindow;
		// 		window.base_ptr = csr->x_d;
		// 		window.num_bytes = x_d_size;
		// 		window.hitRatio = 1.0;
		// 		window.hitProp = cudaAccessPropertyPersisting;
		// 		window.missProp = cudaAccessPropertyStreaming;
		// 		gpuCudaErrorCheck(cudaStreamSetAttribute(csr->stream, cudaStreamAttributeAccessPolicyWindow, &attribute));
		// 	}
		// #endif
	}

	ROCSPARSE_CHECK(rocsparse_dhybmv(csr->handle, rocsparse_operation_none, &alpha, csr->matA, csr->hyb_matA, csr->x_d, &beta, csr->y_d));
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

