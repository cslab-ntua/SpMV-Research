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


INT_T * thread_i_s = NULL;
INT_T * thread_i_e = NULL;

INT_T * thread_i_s_dev = NULL;
INT_T * thread_i_e_dev = NULL;


extern int prefetch_distance;

double * thread_time_compute, * thread_time_barrier;


struct CSRArrays : Matrix_Format
{
	INT_T * ia;      // the usual rowptr (of size m+1)
	INT_T * ja;      // the colidx of each NNZ (of size nnz)
	ValueType * a;   // the values (of size NNZ)

	INT_T * ia_dev;
	INT_T * ja_dev;
	ValueType * a_dev;

	ValueType * x = NULL;
	ValueType * y = NULL;
	ValueType * x_dev = NULL;
	ValueType * y_dev = NULL;

	int max_smem_per_block, multiproc_count, max_threads_per_block, warp_size, max_threads_per_multiproc;
	int num_threads;

	CSRArrays(INT_T * ia, INT_T * ja, ValueType * a, long m, long n, long nnz) : Matrix_Format(m, n, nnz), ia(ia), ja(ja), a(a)
	{
		double time_balance;
		long i;

		cudaDeviceGetAttribute(&max_smem_per_block, cudaDevAttrMaxSharedMemoryPerBlock, 0);
		cudaDeviceGetAttribute(&multiproc_count, cudaDevAttrMultiProcessorCount, 0);
		cudaDeviceGetAttribute(&max_threads_per_block, cudaDevAttrMaxThreadsPerBlock , 0);
		cudaDeviceGetAttribute(&warp_size, cudaDevAttrWarpSize , 0);
		cudaDeviceGetAttribute(&max_threads_per_multiproc, cudaDevAttrMaxThreadsPerMultiProcessor, 0);
		printf("max_smem_per_block=%d\n", max_smem_per_block);
		printf("multiproc_count=%d\n", multiproc_count);
		printf("max_threads_per_block=%d\n", max_threads_per_block);
		printf("warp_size=%d\n", warp_size);
		printf("max_threads_per_multiproc=%d\n", max_threads_per_multiproc);

		// num_threads = 1024;
		// num_threads = 3584;
		// num_threads = 4096;
		num_threads = 8192;
		// num_threads = 14336;
		// num_threads = 16384;

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

		cudaMalloc(&ia_dev, (m+1) * sizeof(*ia_dev));
		cudaMalloc(&ja_dev, nnz * sizeof(*ja_dev));
		cudaMalloc(&a_dev, nnz * sizeof(*a_dev));
		cudaMalloc(&x_dev, n * sizeof(*x_dev));
		cudaMalloc(&y_dev, m * sizeof(*y_dev));
		cudaMalloc(&thread_i_s_dev, num_threads * sizeof(*thread_i_s_dev));
		cudaMalloc(&thread_i_e_dev, num_threads * sizeof(*thread_i_e_dev));

		cudaMemcpy(ia_dev, ia, (m+1) * sizeof(*ia_dev), cudaMemcpyHostToDevice);
		cudaMemcpy(ja_dev, ja, nnz * sizeof(*ja_dev), cudaMemcpyHostToDevice);
		cudaMemcpy(a_dev, a, nnz * sizeof(*a_dev), cudaMemcpyHostToDevice);
		cudaMemcpy(thread_i_s_dev, thread_i_s, num_threads * sizeof(*thread_i_s_dev), cudaMemcpyHostToDevice);
		cudaMemcpy(thread_i_e_dev, thread_i_e, num_threads * sizeof(*thread_i_e_dev), cudaMemcpyHostToDevice);

	}

	~CSRArrays()
	{
		free(a);
		free(ia);
		free(ja);
		free(thread_i_s);
		free(thread_i_e);
		cudaFree(ia_dev);
		cudaFree(ja_dev);
		cudaFree(a_dev);


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
void compute_csr_kahan(CSRArrays * restrict csr, ValueType * restrict x, ValueType * restrict y);
void compute_csr_prefetch(CSRArrays * restrict csr, ValueType * restrict x , ValueType * restrict y);
void compute_csr_omp_simd(CSRArrays * restrict csr, ValueType * restrict x , ValueType * restrict y);
void compute_csr_vector(CSRArrays * restrict csr, ValueType * restrict x , ValueType * restrict y);
void compute_csr_vector_perfect_nnz_balance(CSRArrays * restrict csr, ValueType * restrict x , ValueType * restrict y);


void
CSRArrays::spmv(ValueType * x, ValueType * y)
{
	compute_csr(this, x, y);
}


struct Matrix_Format *
csr_to_format(INT_T * row_ptr, INT_T * col_ind, ValueType * values, long m, long n, long nnz)
{
	struct CSRArrays * csr = new CSRArrays(row_ptr, col_ind, values, m, n, nnz);
	// for (long i=0;i<10;i++)
		// printf("%d\n", row_ptr[i]);
	csr->mem_footprint = nnz * (sizeof(ValueType) + sizeof(INT_T)) + (m+1) * sizeof(INT_T);
	csr->format_name = (char *) "Custom_CSR_CUDA";
	return csr;
}


//==========================================================================================================================================
//= CSR Custom
//==========================================================================================================================================


// __device__ int add(int a, int b)
// {
	// return a + b;
// }


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
	// long shared_mem_size = block_size * sizeof(*C_dev);

	if (csr->x == NULL)
	{
		csr->x = x;
		cudaMemcpy(csr->x_dev, csr->x, csr->n * sizeof(*csr->x), cudaMemcpyHostToDevice);
	}

	gpu_kernel_csr_basic<<<grid_dims, block_dims>>>(thread_i_s_dev, thread_i_e_dev, csr->ia_dev, csr->ja_dev, csr->a_dev, csr->x_dev, csr->y_dev);
	// gpu_kernel_csr_flat<<<grid_dims, block_dims>>>(thread_i_s_dev, thread_i_e_dev, csr->ia_dev, csr->ja_dev, csr->a_dev, csr->x_dev, csr->y_dev);

	cudaError_t err;
	err = cudaDeviceSynchronize();
	if (err != cudaSuccess)
		error("gpu kernel error: %s\n", cudaGetErrorString(err));

	if (csr->y == NULL)
	{
		csr->y = y;
		cudaMemcpy(csr->y, csr->y_dev, csr->m * sizeof(*csr->y), cudaMemcpyDeviceToHost);
	}

	// exit(0);
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

