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

extern int prefetch_distance;

double * thread_time_compute, * thread_time_barrier;

#ifndef BLOCK_SIZE
#define BLOCK_SIZE 1024
#endif

#ifndef MULTIBLOCK_SIZE
#define MULTIBLOCK_SIZE 1
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

	hipStream_t stream;
	// hipEvent_t is useful for timing, but for performance use " cudaEventCreateWithFlags ( &event, cudaEventDisableTiming) "
	hipEvent_t startEvent_execution;
	hipEvent_t endEvent_execution;
	
	hipEvent_t startEvent_memcpy_ia;
	hipEvent_t endEvent_memcpy_ia;
	hipEvent_t startEvent_memcpy_row_blocks;
	hipEvent_t endEvent_memcpy_row_blocks;
	hipEvent_t startEvent_memcpy_ja;
	hipEvent_t endEvent_memcpy_ja;
	hipEvent_t startEvent_memcpy_a;
	hipEvent_t endEvent_memcpy_a;

	hipEvent_t startEvent_memcpy_x;
	hipEvent_t endEvent_memcpy_x;
	hipEvent_t startEvent_memcpy_y;
	hipEvent_t endEvent_memcpy_y;

	int block_size, block_size2;

	CSRArrays(INT_T * ia, INT_T * ja, ValueType * a, long m, long n, long nnz) : Matrix_Format(m, n, nnz), ia(ia), ja(ja), a(a)
	{
		block_size = BLOCK_SIZE;
		block_size2 = MULTIBLOCK_SIZE;

		row_blocks = (typeof(row_blocks)) malloc(m * sizeof(*row_blocks));
		row_blocks_cnt = spmv_csr_adaptive_rowblocks(ia, m, row_blocks);
		printf("%ld nnz, %d row_blocks ( %lf row blocks per thread block, %.0lf nnz/row_block )\n", nnz, row_blocks_cnt, row_blocks_cnt*1.0/MULTIBLOCK_SIZE, nnz*1.0/row_blocks_cnt);
		
		HIP_CHECK(hipMalloc((void**)&ia_d, (m+1) * sizeof(*ia_d)));
		HIP_CHECK(hipMalloc((void**)&row_blocks_d, row_blocks_cnt * sizeof(*row_blocks_d)));
		HIP_CHECK(hipMalloc((void**)&ja_d, nnz * sizeof(*ja_d)));
		HIP_CHECK(hipMalloc((void**)&a_d, nnz * sizeof(*a_d)));
		HIP_CHECK(hipMalloc((void**)&x_d, n * sizeof(*x_d)));
		HIP_CHECK(hipMalloc((void**)&y_d, m * sizeof(*y_d)));

		HIP_CHECK(hipStreamCreate(&stream));

		// rocm events for timing measurements
		HIP_CHECK(hipEventCreate(&startEvent_execution));
		HIP_CHECK(hipEventCreate(&endEvent_execution));

		if(TIME_IT){
			HIP_CHECK(hipEventCreate(&startEvent_memcpy_ia));
			HIP_CHECK(hipEventCreate(&endEvent_memcpy_ia));
			HIP_CHECK(hipEventCreate(&startEvent_memcpy_row_blocks));
			HIP_CHECK(hipEventCreate(&endEvent_memcpy_row_blocks));
			HIP_CHECK(hipEventCreate(&startEvent_memcpy_ja));
			HIP_CHECK(hipEventCreate(&endEvent_memcpy_ja));
			HIP_CHECK(hipEventCreate(&startEvent_memcpy_a));
			HIP_CHECK(hipEventCreate(&endEvent_memcpy_a));

			HIP_CHECK(hipEventCreate(&startEvent_memcpy_x));
			HIP_CHECK(hipEventCreate(&endEvent_memcpy_x));
			HIP_CHECK(hipEventCreate(&startEvent_memcpy_y));
			HIP_CHECK(hipEventCreate(&endEvent_memcpy_y));
		}

		HIP_CHECK(hipHostMalloc((void**)&ia_h, (m+1) * sizeof(*ia_h)));
		HIP_CHECK(hipHostMalloc((void**)&row_blocks_h, row_blocks_cnt * sizeof(*row_blocks_h)));
		HIP_CHECK(hipHostMalloc((void**)&ja_h, nnz * sizeof(*ja_h)));
		HIP_CHECK(hipHostMalloc((void**)&a_h, nnz * sizeof(*a_h)));
		HIP_CHECK(hipHostMalloc((void**)&x_h, n * sizeof(*x_h)));
		HIP_CHECK(hipHostMalloc((void**)&y_h, m * sizeof(*y_h)));

		memcpy(ia_h, ia, (m+1) * sizeof(*ia_h));
		memcpy(row_blocks_h, row_blocks, row_blocks_cnt * sizeof(*row_blocks_h));
		memcpy(ja_h, ja, nnz * sizeof(*ja_h));
		memcpy(a_h, a, nnz * sizeof(*a_h));

		if(TIME_IT) HIP_CHECK(hipEventRecord(startEvent_memcpy_ia));
		HIP_CHECK(hipMemcpyAsync(ia_d, ia_h, (m+1) * sizeof(*ia_d), hipMemcpyHostToDevice, stream));
		if(TIME_IT) HIP_CHECK(hipEventRecord(endEvent_memcpy_ia));

		if(TIME_IT) HIP_CHECK(hipEventRecord(startEvent_memcpy_row_blocks));
		HIP_CHECK(hipMemcpyAsync(row_blocks_d, row_blocks_h, row_blocks_cnt * sizeof(*row_blocks_d), hipMemcpyHostToDevice, stream));
		if(TIME_IT) HIP_CHECK(hipEventRecord(endEvent_memcpy_row_blocks));

		if(TIME_IT) HIP_CHECK(hipEventRecord(startEvent_memcpy_ja));
		HIP_CHECK(hipMemcpyAsync(ja_d, ja_h, nnz * sizeof(*ja_d), hipMemcpyHostToDevice, stream));
		if(TIME_IT) HIP_CHECK(hipEventRecord(endEvent_memcpy_ja));

		if(TIME_IT) HIP_CHECK(hipEventRecord(startEvent_memcpy_a));
		HIP_CHECK(hipMemcpyAsync(a_d, a_h, nnz * sizeof(*a_d), hipMemcpyHostToDevice, stream));
		if(TIME_IT) HIP_CHECK(hipEventRecord(endEvent_memcpy_a));

		if(TIME_IT){
			HIP_CHECK(hipEventSynchronize(endEvent_memcpy_ia));
			HIP_CHECK(hipEventSynchronize(endEvent_memcpy_row_blocks));
			HIP_CHECK(hipEventSynchronize(endEvent_memcpy_ja));
			HIP_CHECK(hipEventSynchronize(endEvent_memcpy_a));

			float memcpyTime_rocm_ia, memcpyTime_rocm_row_blocks, memcpyTime_rocm_ja, memcpyTime_rocm_a;
			HIP_CHECK(hipEventElapsedTime(&memcpyTime_rocm_ia, startEvent_memcpy_ia, endEvent_memcpy_ia));
			HIP_CHECK(hipEventElapsedTime(&memcpyTime_rocm_row_blocks, startEvent_memcpy_row_blocks, endEvent_memcpy_row_blocks));
			HIP_CHECK(hipEventElapsedTime(&memcpyTime_rocm_ja, startEvent_memcpy_ja, endEvent_memcpy_ja));
			HIP_CHECK(hipEventElapsedTime(&memcpyTime_rocm_a, startEvent_memcpy_a, endEvent_memcpy_a));
			printf("(ROCM) Memcpy ia time = %.4lf ms, row_blocks time = %.4lf ms, ja time = %.4lf ms, a time = %.4lf ms\n", memcpyTime_rocm_ia, memcpyTime_rocm_row_blocks, memcpyTime_rocm_ja, memcpyTime_rocm_a);
		}
	}

	~CSRArrays()
	{
		free(a);
		free(ia);
		free(row_blocks);
		free(ja);

		HIP_CHECK(hipFree(ia_d));
		HIP_CHECK(hipFree(row_blocks_d));
		HIP_CHECK(hipFree(ja_d));
		HIP_CHECK(hipFree(a_d));
		HIP_CHECK(hipFree(x_d));
		HIP_CHECK(hipFree(y_d));

		HIP_CHECK(hipHostFree(ia_h));
		HIP_CHECK(hipHostFree(row_blocks_h));
		HIP_CHECK(hipHostFree(ja_h));
		HIP_CHECK(hipHostFree(a_h));
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

			HIP_CHECK(hipEventDestroy(startEvent_memcpy_ia));
			HIP_CHECK(hipEventDestroy(endEvent_memcpy_ia));
			HIP_CHECK(hipEventDestroy(startEvent_memcpy_ja));
			HIP_CHECK(hipEventDestroy(endEvent_memcpy_ja));
			HIP_CHECK(hipEventDestroy(startEvent_memcpy_a));
			HIP_CHECK(hipEventDestroy(endEvent_memcpy_a));

			HIP_CHECK(hipEventDestroy(startEvent_memcpy_row_blocks));
			HIP_CHECK(hipEventDestroy(endEvent_memcpy_row_blocks));
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
	snprintf(format_name, 100, "Custom_CSR_ROCM_ADAPTIVE_b%d_%d", csr->block_size, csr->block_size2);
	csr->format_name = format_name;
	return csr;
}


//==========================================================================================================================================
//= CSR Custom
//==========================================================================================================================================
// This has to be written elsewhere. It is compiled with hipcc (clang compiler) and then linked together with this!
extern "C" void launch_kernel_wrapper(dim3 grid_dims, dim3 block_dims, long shared_mem_size, hipStream_t stream, INT_T * ia_d, INT_T * ja_d, ValueType * a_d, INT_T * row_blocks_d, ValueType * restrict x_d, ValueType * restrict y_d);

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

	launch_kernel_wrapper(grid_dims, block_dims, 0, csr->stream, csr->ia_d, csr->ja_d, csr->a_d, csr->row_blocks_d, csr->x_d, csr->y_d);
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

