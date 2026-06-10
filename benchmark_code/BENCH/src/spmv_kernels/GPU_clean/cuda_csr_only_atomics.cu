#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

#include <cuda.h>
#include <cooperative_groups.h>

#include "macros/cpp_defines.h"

#include "../spmv_kernel.h"

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

#ifndef TIME_IT
	#define TIME_IT 0
#endif


double * thread_time_compute, * thread_time_barrier;

void
cuda_push_duplicate_base(void ** dst_ptr, void * src, long bytes)
{
	cudaMalloc(dst_ptr, bytes);
	cudaMemcpy(*((char **) dst_ptr), src, bytes, cudaMemcpyHostToDevice);
}
#define cuda_push_duplicate(dst_ptr, src, bytes) cuda_push_duplicate_base((void **) dst_ptr, src, bytes)


template<typename T>
void
transpose(T * A, INT_T m, INT_T n)
{
	T * buf = (typeof(buf)) aligned_alloc(64, m*n * sizeof(*buf));
	long i, j;
	for (j=0;j<n;j++)
	{
		for (i=0;i<m;i++)
			buf[j*m + i] = A[i*n + j];
	}
	for (i=0;i<m*n;i++)
		A[i] = buf[i];
	free(buf);
}


struct CSRArrays : Matrix_Format
{
	INT_T * row_ptr_h;
	INT_T * ja_h;
	ValueType * a_h;
	INT_T * thread_block_i_s = NULL;
	INT_T * thread_block_i_e = NULL;
	INT_T * thread_i_s = NULL;

	INT_T * row_ptr_d;
	INT_T * ja_d;
	ValueType * a_d;
	INT_T * thread_block_i_s_d = NULL;
	INT_T * thread_block_i_e_d = NULL;
	INT_T * thread_i_s_d = NULL;

	ValueType * x = NULL;
	ValueType * y = NULL;
	ValueType * x_h = NULL;
	ValueType * y_h = NULL;
	ValueType * x_d = NULL;
	ValueType * y_d = NULL;

	int num_threads;
	int thread_block_size;
	int num_thread_blocks;

	CSRArrays(INT_T * row_ptr, INT_T * ja, ValueTypeReference * a, long m, long n, long nnz) : Matrix_Format(m, n, nnz)
	{
		double time_balance;
		const long nnz_per_block = BLOCK_SIZE * NNZ_PER_THREAD;
		long i;

		cuda_device_print_attributes();

		thread_block_size = BLOCK_SIZE;

		num_threads = (nnz + NNZ_PER_THREAD - 1) / NNZ_PER_THREAD;

		num_threads = ((num_threads + BLOCK_SIZE - 1) / BLOCK_SIZE) * BLOCK_SIZE;

		num_thread_blocks = num_threads / BLOCK_SIZE;

		printf("num_threads=%d, thread_block_size=%d, num_thread_blocks=%d\n", num_threads, BLOCK_SIZE, num_thread_blocks);

		thread_block_i_s = (INT_T *) malloc(num_thread_blocks * sizeof(*thread_block_i_s));
		thread_block_i_e = (INT_T *) malloc(num_thread_blocks * sizeof(*thread_block_i_e));

		thread_i_s = (INT_T *) malloc(num_threads * sizeof(*thread_i_s));

		time_balance = time_it(1,
			_Pragma("omp parallel")
			{
				INT_T thread_block_j_s;
				long lower_boundary;
				_Pragma("omp for")
				for (i=0;i<num_thread_blocks;i++)
				{
					thread_block_j_s = nnz_per_block * i;
					if (thread_block_j_s > nnz)
						thread_block_j_s = nnz;
					macros_binary_search(row_ptr, 0, m, thread_block_j_s, &lower_boundary, NULL);           // Index boundaries are inclusive.
					thread_block_i_s[i] = lower_boundary;
				}
				_Pragma("omp for")
				for (i=0;i<num_thread_blocks;i++)
				{
					if (i == num_thread_blocks - 1)   // If we calculate each thread's boundaries individually some empty rows might be unassigned.
						thread_block_i_e[i] = m;
					else
						thread_block_i_e[i] = thread_block_i_s[i+1] + 1;
				}
			}
		);
		printf("balance time blocks = %g\n", time_balance);

		time_balance = time_it(1,
			_Pragma("omp parallel")
			{
				INT_T thread_j_s;
				long lower_boundary;
				_Pragma("omp for")
				for (i=0;i<num_threads;i++)
				{
					thread_j_s = NNZ_PER_THREAD * i;
					if (thread_j_s > nnz)
						thread_j_s = nnz;
					macros_binary_search(row_ptr, 0, m, thread_j_s, &lower_boundary, NULL);           // Index boundaries are inclusive.
					thread_i_s[i] = lower_boundary;
				}
			}
		);
		printf("balance time threads = %g\n", time_balance);

		// for (i=0;i<num_threads;i++)
		// {
			// printf("%3ld: %10d\n", i, thread_i_s[i]);
		// }

		cuda_assert(cudaMalloc(&row_ptr_d, (m+1) * sizeof(*row_ptr_d)));
		cuda_assert(cudaMalloc(&ja_d, nnz * sizeof(*ja_d)));
		cuda_assert(cudaMalloc(&a_d, nnz * sizeof(*a_d)));
		cuda_assert(cudaMalloc(&thread_block_i_s_d, num_thread_blocks * sizeof(*thread_block_i_s_d)));
		cuda_assert(cudaMalloc(&thread_block_i_e_d, num_thread_blocks * sizeof(*thread_block_i_e_d)));
		cuda_assert(cudaMalloc(&thread_i_s_d, num_threads * sizeof(*thread_i_s_d)));
		cuda_assert(cudaMalloc(&x_d, n * sizeof(*x_d)));
		cuda_assert(cudaMalloc(&y_d, m * sizeof(*y_d)));

		cuda_assert(cudaMallocHost(&row_ptr_h, (m+1) * sizeof(INT_T)));
		cuda_assert(cudaMallocHost(&ja_h, nnz * sizeof(INT_T)));
		cuda_assert(cudaMallocHost(&a_h, nnz * sizeof(ValueType)));
		cuda_assert(cudaMallocHost(&x_h, n * sizeof(ValueType)));
		cuda_assert(cudaMallocHost(&y_h, m * sizeof(ValueType)));

		memcpy(row_ptr_h, row_ptr, (m + 1) * sizeof(INT_T));
		memcpy(ja_h, ja, nnz * sizeof(INT_T));
		memcpy(a_h, a, nnz * sizeof(ValueType));

		_Pragma("omp parallel")
		{
			long i_s, i_e, j, jj;
			_Pragma("omp for")
			for (j=0;j<nnz;j+=32*NNZ_PER_THREAD)
			{
				long j_e = j + 32*NNZ_PER_THREAD;
				if (j_e > nnz)
					j_e = nnz;
				macros_binary_search(row_ptr, 0, m, j, &i_s, NULL);           // Index boundaries are inclusive.
				macros_binary_search(row_ptr, 0, m, j_e-1, &i_e, NULL);           // Index boundaries are inclusive.
				if (i_s == i_e)
				{
					for (jj=j;jj<j_e;jj++)
					{
						ja_h[jj] = ja_h[jj] | 0x80000000;
					}
				}
			}
		}

		cuda_assert(cudaMemcpy(row_ptr_d, row_ptr_h, (m+1) * sizeof(*row_ptr_d), cudaMemcpyHostToDevice));
		cuda_assert(cudaMemcpy(ja_d, ja_h, nnz * sizeof(*ja_d), cudaMemcpyHostToDevice));
		cuda_assert(cudaMemcpy(a_d, a_h, nnz * sizeof(*a_d), cudaMemcpyHostToDevice));
		cuda_assert(cudaMemcpy(thread_block_i_s_d, thread_block_i_s, num_thread_blocks * sizeof(*thread_block_i_s_d), cudaMemcpyHostToDevice));
		cuda_assert(cudaMemcpy(thread_block_i_e_d, thread_block_i_e, num_thread_blocks * sizeof(*thread_block_i_e_d), cudaMemcpyHostToDevice));
		cuda_assert(cudaMemcpy(thread_i_s_d, thread_i_s, num_threads * sizeof(*thread_i_s_d), cudaMemcpyHostToDevice));
	}

	~CSRArrays()
	{
		free(thread_block_i_s);
		free(thread_block_i_e);
		free(thread_i_s);

		cuda_assert(cudaFree(row_ptr_d));
		cuda_assert(cudaFree(ja_d));
		cuda_assert(cudaFree(a_d));
		cuda_assert(cudaFree(thread_block_i_s_d));
		cuda_assert(cudaFree(thread_block_i_e_d));
		cuda_assert(cudaFree(thread_i_s_d));
		cuda_assert(cudaFree(x_d));
		cuda_assert(cudaFree(y_d));

		cuda_assert(cudaFreeHost(row_ptr_h));
		cuda_assert(cudaFreeHost(ja_h));
		cuda_assert(cudaFreeHost(a_h));
		cuda_assert(cudaFreeHost(x_h));
		cuda_assert(cudaFreeHost(y_h));
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
csr_to_format(INT_T * row_ptr, INT_T * col_ind, ValueTypeReference * values, long m, long n, long nnz, long symmetric, long symmetry_expanded)
{
	if (symmetric && !symmetry_expanded)
		error("symmetric matrices not supported by this format, expand symmetry");
	struct CSRArrays * csr = new CSRArrays(row_ptr, col_ind, values, m, n, nnz);
	// for (long i=0;i<10;i++)
		// printf("%d\n", row_ptr[i]);
	csr->mem_footprint = nnz * (sizeof(ValueType) + sizeof(INT_T)) + (m+1) * sizeof(INT_T);
	char *format_name;
	format_name = (char *)malloc(100*sizeof(char));
	snprintf(format_name, 100, "Custom_CSR_CUDA_constant_nnz_per_thread_b%d_nnz%d", BLOCK_SIZE, NNZ_PER_THREAD);
	csr->format_name = format_name;
	return csr;
}


//==========================================================================================================================================
//= CSR Custom
//==========================================================================================================================================


__device__ void spmv_last_block(INT_T * thread_i_s, INT_T * thread_block_i_s, INT_T * thread_block_i_e, INT_T * row_ptr, INT_T * ja, ValueType * a, long m, long n, long nnz, ValueType * restrict x, ValueType * restrict y)
{
	// extern __shared__ char sm[];
	const int tid = cuda_get_thread_num_bc();
	const int tidb = threadIdx.x;
	const int block_id = blockIdx.x;
	const int nnz_per_block = BLOCK_SIZE * NNZ_PER_THREAD;
	// ValueType * val_buf = (typeof(val_buf)) sm;
	__attribute__((unused)) int i, i_s, i_e, j, j_s, j_e, k;
	j_s = block_id * nnz_per_block + tidb * NNZ_PER_THREAD;
	j_e = j_s + NNZ_PER_THREAD;
	if (j_e > nnz)
		j_e = nnz;

	i = thread_i_s[tid];
	// i_s = thread_block_i_s[block_id];
	// i_e = thread_block_i_e[block_id];
	// k = (i_e + i_s) / 2;
	// while (i_s < i_e)
	// {
		// if (j_s >= row_ptr[k])
		// {
			// i_s = k + 1;
		// }
		// else
		// {
			// i_e = k;
		// }
		// k = (i_e + i_s) / 2;
	// }
	// i = i_s - 1;

	if (i >= m)
		i = m-1;

	double sum = 0;
	for (j=j_s;j<j_e;j++)
	{
		if (j >= row_ptr[i+1])
		{
			atomicAdd(&y[i], sum);
			sum = 0;
			while (j >= row_ptr[i+1])
				i++;
		}
		// sum += a[j] * x[ja[j] & 0x7FFFFFFF];
		sum = __fma_rn(a[j], x[ja[j] & 0x7FFFFFFF], sum);
	}
	reduce_block(i, sum, y);
}


template <typename group_t>
__device__ void spmv_warp_single_row(group_t g, int i, int j_s, int j_b_s, int j_w_s, INT_T * ja, ValueType * a, ValueType * restrict x, ValueType * restrict y)
{
	const int tidl = g.thread_rank();   // Group lane.
	int j, jj;
	double sum = 0;
	// PRAGMA(unroll NNZ_PER_THREAD)
	for (j=j_s,jj=j_s;j<j_s+NNZ_PER_THREAD;j++,jj++)
	{
		sum = __fma_rn(a[jj], x[ja[jj] & 0x7FFFFFFF], sum);
	}
	atomicAdd(&y[i], sum);
}


template <typename group_t>
__device__ void spmv_full_warp(group_t g, int single_row, int i_s, int j_s, int j_b_s, int j_w_s, INT_T * row_ptr, INT_T * ja, ValueType * a, ValueType * restrict x, ValueType * restrict y)
{
	extern __shared__ char sm[];
	const int tidl = g.thread_rank();   // Group lane.
	int i, j, jj;
	double sum = 0;
	i = i_s;
	if (single_row)
	{
		spmv_warp_single_row(g, i_s, j_s, j_b_s, j_w_s, ja, a, x, y);
	}
	else
	{
		// PRAGMA(unroll NNZ_PER_THREAD)
		for (j=j_s,jj=j_s;j<j_s+NNZ_PER_THREAD;j++,jj++)
		{
			if (j >= row_ptr[i+1])
			{
				while (j >= row_ptr[i+1])
					i++;
			}
			sum = a[jj] * x[ja[jj] & 0x7FFFFFFF];
			atomicAdd(&y[i], sum);
		}
		reduce_warp(g, i, sum, y);
	}
}


__device__ void spmv_full_block(INT_T * thread_i_s, INT_T * thread_block_i_s, INT_T * thread_block_i_e, INT_T * row_ptr, INT_T * ja, ValueType * a, long m, long n, long nnz, ValueType * restrict x, ValueType * restrict y)
{
	// extern __shared__ char sm[];
	// const int tidb = threadIdx.x;
	const int tid = cuda_get_thread_num_bc();
	const int tidw = threadIdx.x % 32;
	const int warp_id = threadIdx.x / 32;
	const int block_id = blockIdx.x;
	// ValueType * val_buf = (typeof(val_buf)) sm;
	// INT_T * ia_buf = (typeof(ia_buf)) &sm[BLOCK_SIZE * sizeof(ValueType)];
	// INT_T * ia_buf = (typeof(ia_buf)) sm;
	__attribute__((unused)) int i_s, i_e, j_s, j_b_s, j_w_s, k;
	j_b_s = block_id * BLOCK_SIZE * NNZ_PER_THREAD;
	j_w_s = j_b_s + warp_id * 32 * NNZ_PER_THREAD;
	j_s = j_w_s + tidw * NNZ_PER_THREAD;

	i_s = thread_i_s[tid];
	// i_s = thread_block_i_s[block_id];
	// i_e = thread_block_i_e[block_id];
	// k = (i_e + i_s) / 2;
	// while (i_s < i_e)
	// {
		// if (j_s >= row_ptr[k])
		// {
			// i_s = k + 1;
		// }
		// else
		// {
			// i_e = k;
		// }
		// k = (i_e + i_s) / 2;
	// }
	// i_s--;

	int single_row = (ja[j_s] & 0x80000000) ? 1 : 0;
	// int single_row = 0;
	thread_block_tile<32> tile32 = tiled_partition<32>(this_thread_block());
	spmv_full_warp(tile32, single_row, i_s, j_s, j_b_s, j_w_s, row_ptr, ja, a, x, y);
}


__global__ void gpu_kernel_spmv_row_indices_continuous(INT_T * thread_i_s, INT_T * thread_block_i_s, INT_T * thread_block_i_e, INT_T * row_ptr, INT_T * ja, ValueType * a, long m, long n, long nnz, ValueType * restrict x, ValueType * restrict y)
{
	int grid_size = gridDim.x;
	int block_id = blockIdx.x;
	if (block_id == grid_size - 1)
		spmv_last_block(thread_i_s, thread_block_i_s, thread_block_i_e, row_ptr, ja, a, m, n, nnz, x, y);
	else
		spmv_full_block(thread_i_s, thread_block_i_s, thread_block_i_e, row_ptr, ja, a, m, n, nnz, x, y);
}


void
compute_csr(CSRArrays * restrict csr, ValueType * restrict x, ValueType * restrict y)
{
	dim3 block_dims(BLOCK_SIZE);
	dim3 grid_dims(csr->num_thread_blocks);
	// long shared_mem_size = BLOCK_SIZE * (sizeof(ValueType));
	// long shared_mem_size = BLOCK_SIZE * (sizeof(ValueType) + sizeof(INT_T));
	// long shared_mem_size = BLOCK_SIZE * NNZ_PER_THREAD * sizeof(INT_T);
	long shared_mem_size = 0;

	if (csr->x == NULL)
	{
		printf("Grid : {%d, %d, %d} blocks. Blocks : {%d, %d, %d} threads.\n", grid_dims.x, grid_dims.y, grid_dims.z, block_dims.x, block_dims.y, block_dims.z);
		csr->x = x;
		memcpy(csr->x_h, x, csr->n * sizeof(ValueType));
		cuda_assert(cudaMemcpy(csr->x_d, csr->x_h, csr->n * sizeof(*csr->x_d), cudaMemcpyHostToDevice));
	}

	cudaMemset(csr->y_d, 0, csr->m * sizeof(csr->y_d));

	// cudaFuncCachePreferNone:   no preference for shared memory or L1 (default);
	// cudaFuncCachePreferShared: prefer larger shared memory and smaller L1 cache;
	// cudaFuncCachePreferL1:     prefer larger L1 cache and smaller shared memory;
	cuda_assert(cudaFuncSetCacheConfig(gpu_kernel_spmv_row_indices_continuous, cudaFuncCachePreferL1));
	gpu_kernel_spmv_row_indices_continuous<<<grid_dims, block_dims, shared_mem_size>>>(csr->thread_i_s_d, csr->thread_block_i_s_d, csr->thread_block_i_e_d, csr->row_ptr_d, csr->ja_d, csr->a_d, csr->m, csr->n, csr->nnz, csr->x_d, csr->y_d);
	cuda_assert(cudaPeekAtLastError());
	cuda_assert(cudaDeviceSynchronize());

	if (csr->y == NULL)
	{
		csr->y = y;

		cuda_assert(cudaMemcpy(csr->y_h, csr->y_d, csr->m * sizeof(*csr->y_d), cudaMemcpyDeviceToHost));
		memcpy(y, csr->y_h, csr->m * sizeof(ValueType));
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

