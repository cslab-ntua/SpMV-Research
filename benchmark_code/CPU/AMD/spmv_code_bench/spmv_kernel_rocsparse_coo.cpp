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

struct COOArrays : Matrix_Format
{
	INT_T * rowind;  // the rowidx of each NNZ (of size nnz)
	INT_T * colind;  // the colidx of each NNZ (of size nnz)
	ValueType * a;   // the values (of size NNZ)

	INT_T * rowind_d;
	INT_T * colind_d;
	ValueType * a_d;

	INT_T * rowind_h;
	INT_T * colind_h;
	ValueType * a_h;

	rocsparse_handle handle;
	rocsparse_mat_descr matA;

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

	hipEvent_t startEvent_memcpy_rowind;
	hipEvent_t endEvent_memcpy_rowind;
	hipEvent_t startEvent_memcpy_colind;
	hipEvent_t endEvent_memcpy_colind;
	hipEvent_t startEvent_memcpy_a;
	hipEvent_t endEvent_memcpy_a;

	hipEvent_t startEvent_create_matA;
	hipEvent_t endEvent_create_matA;

	// int max_persistent_l2_cache;

	COOArrays(INT_T * ia, INT_T * ja, ValueType * a, long m, long n, long nnz) : Matrix_Format(m, n, nnz), colind(ja), a(a)
	{
		rowind = (typeof(rowind)) malloc(nnz * sizeof(*rowind));
		#pragma omp parallel
		{
			long i, j, j_s, j_e;
			#pragma omp for
			for (i=0;i<nnz;i++)
			{
				rowind[i] = 0;
			}
			#pragma omp for
			for (i=0;i<m;i++)
			{
				j_s = ia[i];
				j_e = ia[i+1];
				for (j=j_s;j<j_e;j++)
					rowind[j] = i;
			}
		}

		HIP_CHECK(hipMalloc((void**)&rowind_d, nnz * sizeof(*rowind_d)));
		HIP_CHECK(hipMalloc((void**)&colind_d, nnz * sizeof(*colind_d)));
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
			HIP_CHECK(hipEventCreate(&startEvent_memcpy_rowind));
			HIP_CHECK(hipEventCreate(&endEvent_memcpy_rowind));
			HIP_CHECK(hipEventCreate(&startEvent_memcpy_colind));
			HIP_CHECK(hipEventCreate(&endEvent_memcpy_colind));
			HIP_CHECK(hipEventCreate(&startEvent_memcpy_a));
			HIP_CHECK(hipEventCreate(&endEvent_memcpy_a));
			HIP_CHECK(hipEventCreate(&startEvent_create_matA));
			HIP_CHECK(hipEventCreate(&endEvent_create_matA));

			HIP_CHECK(hipEventCreate(&startEvent_memcpy_x));
			HIP_CHECK(hipEventCreate(&endEvent_memcpy_x));
			HIP_CHECK(hipEventCreate(&startEvent_memcpy_y));
			HIP_CHECK(hipEventCreate(&endEvent_memcpy_y));
		}

		HIP_CHECK(hipHostMalloc((void**)&rowind_h, nnz * sizeof(*rowind_h)));
		HIP_CHECK(hipHostMalloc((void**)&colind_h, nnz * sizeof(*colind_h)));
		HIP_CHECK(hipHostMalloc((void**)&a_h, nnz * sizeof(*a_h)));
		HIP_CHECK(hipHostMalloc((void**)&x_h, n * sizeof(*x_h)));
		HIP_CHECK(hipHostMalloc((void**)&y_h, m * sizeof(*y_h)));

		memcpy(rowind_h, rowind, nnz * sizeof(*rowind_h));
		memcpy(colind_h, colind, nnz * sizeof(*colind_h));
		memcpy(a_h, a, nnz * sizeof(*a_h));

		if(TIME_IT) HIP_CHECK(hipEventRecord(startEvent_memcpy_rowind));
		HIP_CHECK(hipMemcpyAsync(rowind_d, rowind_h, (m+1) * sizeof(*rowind_d), hipMemcpyHostToDevice, stream));
		if(TIME_IT) HIP_CHECK(hipEventRecord(endEvent_memcpy_rowind));

		if(TIME_IT) HIP_CHECK(hipEventRecord(startEvent_memcpy_colind));
		HIP_CHECK(hipMemcpyAsync(colind_d, colind_h, nnz * sizeof(*colind_d), hipMemcpyHostToDevice, stream));
		if(TIME_IT) HIP_CHECK(hipEventRecord(endEvent_memcpy_colind));

		if(TIME_IT) HIP_CHECK(hipEventRecord(startEvent_memcpy_a));
		HIP_CHECK(hipMemcpyAsync(a_d, a_h, nnz * sizeof(*a_d), hipMemcpyHostToDevice, stream));
		if(TIME_IT) HIP_CHECK(hipEventRecord(endEvent_memcpy_a));

		// Create sparse matrix A in COO format
		if(TIME_IT) HIP_CHECK(hipEventRecord(startEvent_create_matA));
		ROCSPARSE_CHECK(rocsparse_create_mat_descr(&matA));
		if(TIME_IT) HIP_CHECK(hipEventRecord(endEvent_create_matA));

		if(TIME_IT){
			HIP_CHECK(hipEventSynchronize(endEvent_memcpy_rowind));
			HIP_CHECK(hipEventSynchronize(endEvent_memcpy_colind));
			HIP_CHECK(hipEventSynchronize(endEvent_memcpy_a));
			HIP_CHECK(hipEventSynchronize(endEvent_create_matA));

			float memcpyTime_rocm_rowind, memcpyTime_rocm_colind, memcpyTime_rocm_a, create_matA_Time;//memcpyTime_rocm_thread_i_e;
			HIP_CHECK(hipEventElapsedTime(&memcpyTime_rocm_rowind, startEvent_memcpy_rowind, endEvent_memcpy_rowind));
			HIP_CHECK(hipEventElapsedTime(&memcpyTime_rocm_colind, startEvent_memcpy_colind, endEvent_memcpy_colind));
			HIP_CHECK(hipEventElapsedTime(&memcpyTime_rocm_a, startEvent_memcpy_a, endEvent_memcpy_a));
			HIP_CHECK(hipEventElapsedTime(&create_matA_Time, startEvent_create_matA, endEvent_create_matA));
			printf("(ROCM) Memcpy ia time = %.4lf ms, ja time = %.4lf ms, a time = %.4lf ms, matA time = %.4lf ms\n", memcpyTime_rocm_rowind, memcpyTime_rocm_colind, memcpyTime_rocm_a, create_matA_Time);
		}

	}

	~COOArrays()
	{
		free(a);
		free(rowind);
		free(colind);

		// destroy matrix/vector descriptors
		ROCSPARSE_CHECK(rocsparse_destroy_mat_descr(matA));
		ROCSPARSE_CHECK(rocsparse_destroy_handle(handle));

		HIP_CHECK(hipFree(rowind_d));
		HIP_CHECK(hipFree(colind_d));
		HIP_CHECK(hipFree(a_d));
		HIP_CHECK(hipFree(x_d));
		HIP_CHECK(hipFree(y_d));

		HIP_CHECK(hipHostFree(rowind_h));
		HIP_CHECK(hipHostFree(colind_h));
		HIP_CHECK(hipHostFree(a_h));
		HIP_CHECK(hipHostFree(x_h));
		HIP_CHECK(hipHostFree(y_h));

		HIP_CHECK(hipEventDestroy(startEvent_execution));
		HIP_CHECK(hipEventDestroy(endEvent_execution));

		if(TIME_IT){
			HIP_CHECK(hipEventDestroy(startEvent_memcpy_rowind));
			HIP_CHECK(hipEventDestroy(endEvent_memcpy_rowind));
			HIP_CHECK(hipEventDestroy(startEvent_memcpy_colind));
			HIP_CHECK(hipEventDestroy(endEvent_memcpy_colind));
			HIP_CHECK(hipEventDestroy(startEvent_memcpy_a));
			HIP_CHECK(hipEventDestroy(endEvent_memcpy_a));

			HIP_CHECK(hipEventDestroy(startEvent_create_matA));
			HIP_CHECK(hipEventDestroy(endEvent_create_matA));

			HIP_CHECK(hipEventDestroy(startEvent_memcpy_x));
			HIP_CHECK(hipEventDestroy(endEvent_memcpy_x));
			HIP_CHECK(hipEventDestroy(startEvent_memcpy_y));
			HIP_CHECK(hipEventDestroy(endEvent_memcpy_y));
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
	coo->format_name = (char *) "ROCSPARSE_COO";
	return coo;
}


//==========================================================================================================================================
//= CSR Custom
//==========================================================================================================================================


void
compute_coo(COOArrays * restrict coo, ValueType * restrict x, ValueType * restrict y)
{
	const double alpha = 1.0;
	const double beta = 0.0;
	if (coo->x == NULL)
	{
		coo->x = x;
		if(TIME_IT) HIP_CHECK(hipEventRecord(coo->startEvent_memcpy_x, coo->stream));
		memcpy(coo->x_h, x, coo->n * sizeof(ValueType));
		HIP_CHECK(hipMemcpyAsync(coo->x_d, coo->x_h, coo->n * sizeof(*coo->x_d), hipMemcpyHostToDevice, coo->stream));
		if(TIME_IT) HIP_CHECK(hipEventRecord(coo->endEvent_memcpy_x, coo->stream));
		if(TIME_IT){
			HIP_CHECK(hipEventSynchronize(coo->endEvent_memcpy_x));
			float memcpyTime_rocm;
			HIP_CHECK(hipEventElapsedTime(&memcpyTime_rocm, coo->startEvent_memcpy_x, coo->endEvent_memcpy_x));
			printf("(ROCM) Memcpy x time = %.4lf ms\n", memcpyTime_rocm);
		}

		// #ifdef PERSISTENT_L2_PREFETCH
		// 	int x_d_size = coo->n * sizeof(*coo->x);
		// 	gpuCudaErrorCheck(cudaCtxResetPersistingL2Cache()); // This needs to happen every time before running kernel for 1st time for a matrix...
		// 	if(x_d_size < coo->max_persistent_l2_cache){
		// 		cudaStreamAttrValue attribute;
		// 		auto &window = attribute.accessPolicyWindow;
		// 		window.base_ptr = coo->x_d;
		// 		window.num_bytes = x_d_size;
		// 		window.hitRatio = 1.0;
		// 		window.hitProp = cudaAccessPropertyPersisting;
		// 		window.missProp = cudaAccessPropertyStreaming;
		// 		gpuCudaErrorCheck(cudaStreamSetAttribute(coo->stream, cudaStreamAttributeAccessPolicyWindow, &attribute));
		// 	}
		// #endif
	}

	ROCSPARSE_CHECK(rocsparse_dcoomv(coo->handle, rocsparse_operation_none, coo->m, coo->n, coo->nnz, &alpha, coo->matA, coo->a_d, coo->rowind_d, coo->colind_d, coo->x_d, &beta, coo->y_d));
	HIP_CHECK(hipDeviceSynchronize());

	if (coo->y == NULL)
	{
		coo->y = y;
		if(TIME_IT) HIP_CHECK(hipEventRecord(coo->startEvent_memcpy_y, coo->stream));
		HIP_CHECK(hipMemcpyAsync(coo->y_h, coo->y_d, coo->m * sizeof(*coo->y_d), hipMemcpyDeviceToHost, coo->stream));
		HIP_CHECK(hipStreamSynchronize(coo->stream));
		memcpy(y, coo->y_h, coo->m * sizeof(ValueType));
		if(TIME_IT) HIP_CHECK(hipEventRecord(coo->endEvent_memcpy_y, coo->stream));

		if(TIME_IT){
			HIP_CHECK(hipEventSynchronize(coo->endEvent_memcpy_y));
			float memcpyTime_rocm;
			HIP_CHECK(hipEventElapsedTime(&memcpyTime_rocm, coo->startEvent_memcpy_y, coo->endEvent_memcpy_y));
			printf("(ROCM) Memcpy y time = %.4lf ms\n", memcpyTime_rocm);
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

