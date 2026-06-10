#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

#include <cuda.h>
#include <cooperative_groups.h>
#include <cuda_pipeline_primitives.h>

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

	#include "functools/functools_gen_push.h"
	#define FUNCTOOLS_GEN_TYPE_1  int
	#define FUNCTOOLS_GEN_TYPE_2  int
	#define FUNCTOOLS_GEN_SUFFIX  _CUDA_CSR_TRANSPOSE_EXPAND_ROWS_CU
	#include "functools/functools_gen.c"
	__attribute__((pure))
	static inline
	int
	functools_map_fun(int * A, long i)
	{
		return A[i];
	}
	__attribute__((pure))
	static inline
	int
	functools_reduce_fun(int a, int b)
	{
		return a + b;
	}

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



struct timers_s {
	unsigned long long time_kernel;
	unsigned long long time_warp;
	unsigned long long time_load_warp_i_bounds;
	unsigned long long time_bin_search;
	unsigned long long time_reduce_gen;
	unsigned long long time_reduce_single_row;
};


struct CSRArrays : Matrix_Format
{
	long nnz_expanded;

	INT_T * row_ptr_h;
	INT_T * ja_h;
	ValueType * a_h;
	INT_T * thread_warp_i_s = NULL;
	INT_T * thread_warp_i_e = NULL;
	INT_T * thread_i_s = NULL;

	INT_T * row_ptr_d;
	INT_T * ja_d;
	ValueType * a_d;
	INT_T * thread_warp_i_s_d = NULL;
	INT_T * thread_warp_i_e_d = NULL;
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
	int num_thread_warps;

	struct timers_s * timers;
	struct timers_s * timers_d;
	unsigned long long * timer_block;
	unsigned long long * timer_block_d;

	CSRArrays(INT_T * row_ptr, INT_T * ja, ValueTypeReference * a, long m, long n, long nnz) : Matrix_Format(m, n, nnz)
	{
		const long nnz_per_block = BLOCK_SIZE * NNZ_PER_THREAD;
		const long nnz_per_warp = 32 * NNZ_PER_THREAD;
		double time_balance;
		long i;

		cuda_device_print_attributes();

		thread_block_size = BLOCK_SIZE;

		row_ptr_h = (typeof(row_ptr_h)) malloc((m+1) * sizeof(*row_ptr_h));
		_Pragma("omp parallel")
		{
			long i;
			long degree;
			_Pragma("omp for")
			for (i=0;i<m;i++)
			{
				degree = row_ptr[i+1] - row_ptr[i];
				degree = NNZ_PER_THREAD * ((degree + NNZ_PER_THREAD - 1) / NNZ_PER_THREAD);
				row_ptr_h[i] = degree;
			}
		}
		row_ptr_h[m] = 0;
		scan_reduce(row_ptr_h, row_ptr_h, m+1, 0, 1, 0);
		nnz_expanded = row_ptr_h[m];
		printf("nnz_expanded=%ld\n", nnz_expanded);
		ja_h = (typeof(ja_h)) malloc(nnz_expanded * sizeof(*ja_h));
		a_h = (typeof(a_h)) malloc(nnz_expanded * sizeof(*a_h));
		_Pragma("omp parallel")
		{
			long i, j1, j2;
			_Pragma("omp barrier")
			_Pragma("omp for")
			for (i=0;i<m;i++)
			{
				for (j1=row_ptr[i],j2=row_ptr_h[i];j1<row_ptr[i+1];j1++,j2++)
				{
					ja_h[j2] = ja[j1];
					a_h[j2] = a[j1];
				}
				for (;j2<row_ptr_h[i+1];j2++)
				{
					ja_h[j2] = ja[row_ptr[i+1] - 1];
					a_h[j2] = 0;
				}
			}
		}


		num_threads = (nnz_expanded + NNZ_PER_THREAD - 1) / NNZ_PER_THREAD;
		num_thread_blocks = (num_threads + BLOCK_SIZE - 1) / BLOCK_SIZE;
		num_threads = num_thread_blocks * BLOCK_SIZE;
		num_thread_warps = (num_threads + 32 - 1) / 32;
		printf("num_threads=%d, thread_block_size=%d, num_thread_blocks=%d\n", num_threads, BLOCK_SIZE, num_thread_blocks);

		thread_warp_i_s = (INT_T *) malloc(num_thread_warps * sizeof(*thread_warp_i_s));
		thread_warp_i_e = (INT_T *) malloc(num_thread_warps * sizeof(*thread_warp_i_e));

		thread_i_s = (INT_T *) malloc(num_threads * sizeof(*thread_i_s));

		time_balance = time_it(1,
			_Pragma("omp parallel")
			{
				INT_T thread_warp_j_s, thread_warp_j_e;
				long lower_boundary, higher_boundary;
				_Pragma("omp for")
				for (i=0;i<num_thread_warps;i++)
				{
					thread_warp_j_s = nnz_per_warp * i;
					if (thread_warp_j_s > nnz_expanded)
						thread_warp_j_s = nnz_expanded;
					macros_binary_search(row_ptr_h, 0, m, thread_warp_j_s, &lower_boundary, NULL);           // Index boundaries are inclusive.
					thread_warp_i_s[i] = lower_boundary;
					thread_warp_j_e = thread_warp_j_s + nnz_per_warp;
					if (thread_warp_j_e > nnz_expanded)
						thread_warp_j_e = nnz_expanded;
					macros_binary_search(row_ptr_h, 0, m, thread_warp_j_e, NULL, &higher_boundary);           // Index boundaries are inclusive.
					thread_warp_i_e[i] = higher_boundary;
				}
			}
		);
		printf("balance time warps = %g\n", time_balance);

		time_balance = time_it(1,
			_Pragma("omp parallel")
			{
				INT_T thread_j_s;
				long lower_boundary;
				long i;
				_Pragma("omp for")
				for (i=0;i<num_threads;i++)
				{
					thread_j_s = NNZ_PER_THREAD * i;
					if (thread_j_s > nnz_expanded)
						thread_j_s = nnz_expanded;
					macros_binary_search(row_ptr_h, 0, m, thread_j_s, &lower_boundary, NULL);           // Index boundaries are inclusive.
					thread_i_s[i] = lower_boundary;
				}
			}
		);
		printf("balance time threads = %g\n", time_balance);

		cuda_assert(cudaMalloc(&row_ptr_d, (m+1) * sizeof(*row_ptr_d)));
		cuda_assert(cudaMalloc(&ja_d, nnz_expanded * sizeof(*ja_d)));
		cuda_assert(cudaMalloc(&a_d, nnz_expanded * sizeof(*a_d)));
		cuda_assert(cudaMalloc(&thread_warp_i_s_d, num_thread_warps * sizeof(*thread_warp_i_s_d)));
		cuda_assert(cudaMalloc(&thread_warp_i_e_d, num_thread_warps * sizeof(*thread_warp_i_e_d)));
		cuda_assert(cudaMalloc(&thread_i_s_d, num_threads * sizeof(*thread_i_s_d)));
		cuda_assert(cudaMalloc(&x_d, n * sizeof(*x_d)));
		cuda_assert(cudaMalloc(&y_d, m * sizeof(*y_d)));

		x_h = (typeof(x_h)) malloc(n * sizeof(*x_h));
		y_h = (typeof(y_h)) malloc(m * sizeof(*y_h));

		_Pragma("omp parallel")
		{
			long i_s, i_e, j, jj;
			_Pragma("omp for")
			for (j=0;j<nnz_expanded;j+=32*NNZ_PER_THREAD)
			{
				long j_e = j + 32*NNZ_PER_THREAD;
				if (j_e > nnz_expanded)
					j_e = nnz_expanded;
				macros_binary_search(row_ptr_h, 0, m, j, &i_s, NULL);           // Index boundaries are inclusive.
				macros_binary_search(row_ptr_h, 0, m, j_e-1, &i_e, NULL);           // Index boundaries are inclusive.
				if (i_s == i_e)
				{
					// int tid_s = j / NNZ_PER_THREAD;
					// int tid_e = (j_e + NNZ_PER_THREAD - 1) / NNZ_PER_THREAD;
					// for (int t=tid_s;t<tid_e;t++)
						// thread_i_s[t] = thread_i_s[t] | 0x80000000;
					// int wid = j / (32*NNZ_PER_THREAD);
					// thread_warp_i_s[wid] = thread_warp_i_s[wid] | 0x80000000;
					// for (jj=j;jj<j_e;jj++)
						// ja_h[jj] = ja_h[jj] | 0x80000000;
				}
			}
		}

		_Pragma("omp parallel")
		{
			long j;
			_Pragma("omp for")
			for (j=0;j<(num_thread_blocks-1)*nnz_per_block;j+=32*NNZ_PER_THREAD)
			{
				long j_e = j + 32*NNZ_PER_THREAD;
				if (j_e > nnz_expanded)
					j_e = nnz_expanded;
				if (j_e < nnz_expanded)
				{
					transpose(&a_h[j], 32, NNZ_PER_THREAD);
					transpose(&ja_h[j], 32, NNZ_PER_THREAD);
				}
			}
		}

		// _Pragma("omp parallel")
		// {
			// long j;
			// _Pragma("omp for")
			// for (j=0;j<(num_thread_blocks-1)*nnz_per_block;j+=BLOCK_SIZE*NNZ_PER_THREAD)
			// {
				// long j_e = j + BLOCK_SIZE*NNZ_PER_THREAD;
				// if (j_e > nnz_expanded)
					// j_e = nnz_expanded;
				// if (j_e < nnz_expanded)
				// {
					// transpose(&a_h[j], BLOCK_SIZE, NNZ_PER_THREAD);
					// transpose(&ja_h[j], BLOCK_SIZE, NNZ_PER_THREAD);
				// }
			// }
		// }

		cuda_assert(cudaMemcpy(row_ptr_d, row_ptr_h, (m+1) * sizeof(*row_ptr_d), cudaMemcpyHostToDevice));
		cuda_assert(cudaMemcpy(ja_d, ja_h, nnz_expanded * sizeof(*ja_d), cudaMemcpyHostToDevice));
		cuda_assert(cudaMemcpy(a_d, a_h, nnz_expanded * sizeof(*a_d), cudaMemcpyHostToDevice));
		cuda_assert(cudaMemcpy(thread_warp_i_s_d, thread_warp_i_s, num_thread_warps * sizeof(*thread_warp_i_s_d), cudaMemcpyHostToDevice));
		cuda_assert(cudaMemcpy(thread_warp_i_e_d, thread_warp_i_e, num_thread_warps * sizeof(*thread_warp_i_e_d), cudaMemcpyHostToDevice));
		cuda_assert(cudaMemcpy(thread_i_s_d, thread_i_s, num_threads * sizeof(*thread_i_s_d), cudaMemcpyHostToDevice));

		timers = (typeof(timers)) malloc(sizeof(*timers));
		cuda_assert(cudaMalloc(&timers_d, sizeof(*timers_d)));

		timer_block = (typeof(timer_block)) malloc(num_thread_blocks * sizeof(*timer_block));
		cuda_assert(cudaMalloc(&timer_block_d, num_thread_blocks * sizeof(*timer_block_d)));

	}

	~CSRArrays()
	{
		free(thread_warp_i_s);
		free(thread_warp_i_e);
		free(thread_i_s);

		cuda_assert(cudaFree(row_ptr_d));
		cuda_assert(cudaFree(ja_d));
		cuda_assert(cudaFree(a_d));
		cuda_assert(cudaFree(thread_warp_i_s_d));
		cuda_assert(cudaFree(thread_warp_i_e_d));
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
	csr->mem_footprint = csr->nnz_expanded * (sizeof(ValueType) + sizeof(INT_T)) + (m+1) * sizeof(INT_T);
	char *format_name;
	format_name = (char *)malloc(100*sizeof(char));
	snprintf(format_name, 100, "Custom_CSR_CUDA_expanded_rows_b%d_nnz%d", BLOCK_SIZE, NNZ_PER_THREAD);
	csr->format_name = format_name;
	return csr;
}


//==========================================================================================================================================
//= CSR Custom
//==========================================================================================================================================


template<typename T>
__device__
T
binary_search_gpu(T * A, long s, long e, T target)
{
	long m;
	while (1)
	{
		m = (s + e) / 2;
		if (m == s || m == e)
			break;
		if (target > A[m])
			s = m;
		else
			e = m;
	}
	if (target == A[e])
		return e;
	return s;
}


template <typename group_t>
inline
__device__
void
reduce_warp(group_t g, INT_T row, ValueType val, ValueType * restrict y)
{
	const int tidw = g.thread_rank();   // Group lane.
	int mask_same_row = g.match_any(row);
	int k;
	#pragma unroll
	for (k=g.size()/2; k>=1; k/=2)
	{
		int tidl_next = tidw + k;
		ValueType val_next = g.shfl(val, tidl_next);
		if ((tidl_next < g.size()) && (mask_same_row & (1 << tidl_next)))
		{
			val += val_next;
		}
	}
	if (tidw == __ffs(mask_same_row) - 1)  // __ffs enumeration is 1-based.
		atomicAdd(&y[row], val);
}



inline
__device__
void
reduce_block(INT_T row, ValueType val, ValueType * restrict y)
{
	thread_block_tile<32> tile32 = tiled_partition<32>(this_thread_block());
	reduce_warp(tile32, row, val, y);
}


__device__
void
spmv_last_block(INT_T * thread_i_s, INT_T * thread_warp_i_s, INT_T * thread_warp_i_e, INT_T * row_ptr, INT_T * ja, ValueType * a, long m, long n, long nnz, ValueType * restrict x, ValueType * restrict y)
{
	// extern __shared__ char sm[];
	const int tid = cuda_get_thread_num_bc();
	const int tidb = threadIdx.x;
	const int wid = tid / 32;
	const int widb = tidb / 32;
	const int block_id = blockIdx.x;
	const int nnz_per_block = BLOCK_SIZE * NNZ_PER_THREAD;
	// ValueType * val_buf = (typeof(val_buf)) sm;
	__attribute__((unused)) int i, i_s, i_e, j, j_s, j_e, k;
	j_s = block_id * nnz_per_block + tidb * NNZ_PER_THREAD;
	j_e = j_s + NNZ_PER_THREAD;
	if (j_e > nnz)
		j_e = nnz;

	// i = thread_i_s[tid];
	i_s = thread_warp_i_s[wid];
	i_e = thread_warp_i_e[wid];
	i = binary_search_gpu(row_ptr, i_s, i_e, j_s);

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
		// sum += a[j] * x[ja[j]];
		sum = __fma_rn(a[j], x[ja[j] & 0x7FFFFFFF], sum);
		// sum = __fma_rn(a[j], x[ja[j]], sum);
	}
	reduce_block(i, sum, y);
}


template <typename group_t>
inline
__device__
ValueType
reduce_warp_single_row(group_t g, ValueType val)
{
	// Use XOR mode to perform butterfly reduction
	for (int i=g.size()/2; i>=1; i/=2)
	{
		val += g.shfl_xor(val, i); // __shfl_xor_sync(0xFFFFFFFF, val, i, g.size());   // Total sum is same on all threads.
		// val += __shfl_down_sync(0xFFFFFFFF, val, i, g.size());   // Only thread 0 has the total sum.
	}
	return val;
}


template <typename group_t>
__device__
void
spmv_full_warp(group_t g, struct timers_s * timers, int i_s, int j_s, int j_b_s, int j_w_s, INT_T * row_ptr, INT_T * ja, ValueType * a, ValueType * restrict x, ValueType * restrict y)
{
	unsigned long long ts_r=0, ts_re=0;
	unsigned long long time_reduce_gen=0, time_reduce_single_row=0;

	// extern __shared__ double x_smem[];
	const int tidw = g.thread_rank();   // Group lane.
	int i, j, jj;
	int ptr_next;
	double sum = 0;
	double x_buf;
	int j_e = j_s + NNZ_PER_THREAD;
	int single_row;
	// jj = j_s;
	// jj = j_w_s + tidw;
	// __pipeline_memcpy_async(&x_smem[threadIdx.x], &x[ja[jj]], 8);
	// __pipeline_commit();
	i = i_s;
	ptr_next = row_ptr[i+1];
	// PRAGMA(unroll NNZ_PER_THREAD)
	// for (j=j_s,jj=j_s;j<j_e;j++,jj++)
	// for (j=j_s,jj=j_b_s+threadIdx.x;j<j_e;j++,jj+=BLOCK_SIZE)
	for (j=j_s,jj=j_w_s+tidw;j<j_e;j++,jj+=g.size())
	{
		// __pipeline_wait_prior(0); //wait on needed prefetch value
		// x_buf = x_smem[threadIdx.x];
		// if (j + 1 < j_e)
		// {
			// __pipeline_memcpy_async(&x_smem[threadIdx.x], &x[ja[jj + g.size()]], 8);
			// __pipeline_commit();
		// }
		sum = __fma_rn(a[jj], x[ja[jj] & 0x7FFFFFFF], sum);
		// sum = __fma_rn(a[jj], x[ja[jj]], sum);
		// sum = __fma_rn(a[jj], x_buf, sum);
		// sum = __fma_rn(a[j], x[ja[j]], sum);
	}


	ts_r = clock64();
	g.match_all(i_s, single_row);   // 'single_row' is passed as reference!!!
	if (single_row)
	{
		sum = reduce_warp_single_row(g, sum);
		if (tidw == 0)
			atomicAdd(&y[i_s], sum);
		ts_re = clock64();
		time_reduce_single_row = ts_re - ts_r;
		__syncthreads();
		atomicAdd(&timers->time_reduce_single_row, ts_re - ts_r);
	}
	else
	{
		reduce_warp(g, i, sum, y);
		ts_re = clock64();
		time_reduce_gen = ts_re - ts_r;
		__syncthreads();
		atomicAdd(&timers->time_reduce_gen, ts_re - ts_r);
	}
}


__device__
void
spmv_full_block(struct timers_s * timers, INT_T * thread_i_s, INT_T * thread_warp_i_s, INT_T * thread_warp_i_e, INT_T * row_ptr, INT_T * ja, ValueType * a, long m, long n, long nnz, ValueType * restrict x, ValueType * restrict y)
{
	unsigned long long ts_s=0, ts_e=0, ts_lidx=0, ts_bs=0, ts_ex=0;
	unsigned long long time_kernel=0, time_load_warp_i_bounds=0, time_bin_search=0, time_warp=0;

	ts_s = clock64();

	// extern __shared__ char sm[];
	const int tid = cuda_get_thread_num_bc();
	// const int tid = blockIdx.x * BLOCK_SIZE + threadIdx.x;
	const int tidb = threadIdx.x;
	const int tidw = tidb % 32;
	const int wid = tid / 32;
	const int widb = tidb / 32;
	const int block_id = blockIdx.x;


	// ValueType * val_buf = (typeof(val_buf)) sm;
	// INT_T * ia_buf = (typeof(ia_buf)) &sm[BLOCK_SIZE * sizeof(ValueType)];
	// INT_T * ia_buf = (typeof(ia_buf)) sm;
	__attribute__((unused)) int i_s, i_e, j_s, j_b_s, j_w_s, k;
	j_b_s = block_id * BLOCK_SIZE * NNZ_PER_THREAD;
	// j_w_s = j_b_s + widb * 32 * NNZ_PER_THREAD;
	j_w_s = wid * 32 * NNZ_PER_THREAD;
	// j_s = j_w_s + tidw * NNZ_PER_THREAD;
	j_s = tid * NNZ_PER_THREAD;

	ts_lidx = clock64();

	// i_s = thread_i_s[tid];
	i_s = thread_warp_i_s[wid];
	i_e = thread_warp_i_e[wid];

	ts_bs = clock64();

	i_s = binary_search_gpu(row_ptr, i_s, i_e, j_s);

	ts_ex = clock64();

	thread_block_tile<32> tile32 = tiled_partition<32>(this_thread_block());
	spmv_full_warp(tile32, timers, i_s, j_s, j_b_s, j_w_s, row_ptr, ja, a, x, y);

	ts_e = clock64();

	__syncthreads();
	atomicAdd(&timers->time_kernel, ts_e - ts_s);
	atomicAdd(&timers->time_warp, ts_e - ts_ex);
	atomicAdd(&timers->time_load_warp_i_bounds, ts_bs - ts_lidx);
	atomicAdd(&timers->time_bin_search, ts_ex - ts_bs);
}


__global__
void
gpu_kernel_spmv_row_indices_continuous(struct timers_s * timers, unsigned long long * timer_block, INT_T * thread_i_s, INT_T * thread_warp_i_s, INT_T * thread_warp_i_e, INT_T * row_ptr, INT_T * ja, ValueType * a, long m, long n, long nnz, ValueType * restrict x, ValueType * restrict y)
{
	unsigned long long ts_s=0, ts_e=0;
	int grid_size = gridDim.x;
	int block_id = blockIdx.x;
	int thread_id = threadIdx.x;

	ts_s = clock64();
	if (block_id != grid_size - 1)
		spmv_full_block(timers, thread_i_s, thread_warp_i_s, thread_warp_i_e, row_ptr, ja, a, m, n, nnz, x, y);
	else
		spmv_last_block(thread_i_s, thread_warp_i_s, thread_warp_i_e, row_ptr, ja, a, m, n, nnz, x, y);
	ts_e = clock64();

	if (thread_id == 0)
		timer_block[block_id] = ts_e - ts_s;
}


void
compute_csr(CSRArrays * restrict csr, ValueType * restrict x, ValueType * restrict y)
{
	dim3 block_dims(BLOCK_SIZE);
	dim3 grid_dims(csr->num_thread_blocks);
	long shared_mem_size = 0;
	// shared_mem_size = BLOCK_SIZE * (sizeof(ValueType));
	// shared_mem_size = BLOCK_SIZE * (sizeof(ValueType) + sizeof(INT_T));
	// shared_mem_size = BLOCK_SIZE * NNZ_PER_THREAD * sizeof(INT_T);

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
	// cuda_assert(cudaFuncSetCacheConfig(gpu_kernel_spmv_row_indices_continuous, cudaFuncCachePreferL1));
	// cuda_assert(cudaFuncSetCacheConfig(gpu_kernel_spmv_row_indices_continuous, cudaFuncCachePreferShared));
	gpu_kernel_spmv_row_indices_continuous<<<grid_dims, block_dims, shared_mem_size>>>(csr->timers_d, csr->timer_block_d, csr->thread_i_s_d, csr->thread_warp_i_s_d, csr->thread_warp_i_e_d, csr->row_ptr_d, csr->ja_d, csr->a_d, csr->m, csr->n, csr->nnz_expanded, csr->x_d, csr->y_d);
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
	cuda_assert(cudaMemset(timers_d, 0, sizeof(*timers_d)));
}


int
statistics_print_labels(__attribute__((unused)) char * buf, __attribute__((unused)) long buf_n)
{
	return 0;
}


int
CSRArrays::statistics_print_data(__attribute__((unused)) char * buf, __attribute__((unused)) long buf_n)
{
	cuda_assert(cudaMemcpy(timers, timers_d, sizeof(*timers_d), cudaMemcpyDeviceToHost));
	printf("fraction_kernel=%g\n", (double) timers->time_kernel / timers->time_kernel);
	printf("fraction_warp=%g\n", (double) timers->time_warp / timers->time_kernel);
	printf("fraction_load_warp_i_bounds=%g\n", (double) timers->time_load_warp_i_bounds / timers->time_kernel);
	printf("fraction_bin_search=%g\n", (double) timers->time_bin_search / timers->time_kernel);
	printf("fraction_reduce_gen=%g\n", (double) timers->time_reduce_gen / timers->time_kernel);
	printf("fraction_reduce_single_row=%g\n", (double) timers->time_reduce_single_row / timers->time_kernel);
	
	cuda_assert(cudaMemcpy(timer_block, timer_block_d, num_thread_blocks * sizeof(*timer_block_d), cudaMemcpyDeviceToHost));
	// printf("timer_block\n----\n");
	// for(int i=0; i<num_thread_blocks; i++) printf("%d\t%lld\n", i, timer_block[i]);
	// printf("----\n");
	FILE *fp = fopen("temp_tb.txt", "w");
	if (fp == NULL) {
		perror("Error opening file");
		return 0; // or handle error appropriately
	}
	for(int i = 0; i < num_thread_blocks; i++) fprintf(fp, "%lld\n", timer_block[i]);
	fclose(fp);
	return 0;
}

