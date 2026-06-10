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
	#define FUNCTOOLS_GEN_SUFFIX  _CUDA_CSR_TRANSPOSE_ONE_ROW_PER_THREAD_CU
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

// #ifndef NNZ_PER_THREAD
	// #define NNZ_PER_THREAD  36
// #endif
long NNZ_PER_THREAD = 6;

#ifndef BLOCK_SIZE
	// #define BLOCK_SIZE  32
	// #define BLOCK_SIZE  64
	#define BLOCK_SIZE  128
	// #define BLOCK_SIZE  256
	// #define BLOCK_SIZE  512
	// #define BLOCK_SIZE  1024
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


template<typename T>
void
transpose_split_rows(T * A, INT_T m, INT_T n, INT_T num_row_segments)
{
	T * buf = (typeof(buf)) aligned_alloc(64, m*n * sizeof(*buf));
	INT_T m_new = m * num_row_segments;
	INT_T n_div = n / num_row_segments;  // Normal segment length.
	INT_T num_row_segments_extended = n % num_row_segments;  // The initial segments that expand to n_div+1 due to n%num_row_segments != 0;
	INT_T i, j, i_new, j_new, ii;
	for (i=0;i<m;i++)
	{
		j = 0;
		for (ii=0;ii<num_row_segments;ii++)
		{
			i_new = i * num_row_segments + ii;
			INT_T n_actual = ii < num_row_segments_extended ? n_div + 1 : n_div;
			INT_T i_r = ii < num_row_segments_extended ? ii : num_row_segments_extended;
			for (j_new=0;j_new<n_actual;j_new++)
			{
				INT_T j_r = j_new < n_div ? i_new : i * num_row_segments_extended + ii;
				buf[j_new*m_new + j_r] = A[i*n + ii*n_div + i_r + j_new];
				j++;
			}
		}
	}
	for (i=0;i<m*n;i++)
		A[i] = buf[i];
}


struct CSRArrays : Matrix_Format
{
	long nnz_extended;

	INT_T * row_ptr_h;
	INT_T * ja_h;
	ValueType * a_h;

	INT_T * row_ptr_d;
	INT_T * ja_d;
	ValueType * a_d;

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
	int num_active_threads;
	int num_active_thread_warps;

	int nnz_per_block;
	int nnz_per_warp;

	CSRArrays(INT_T * row_ptr, INT_T * ja, ValueTypeReference * a, long m, long n, long nnz) : Matrix_Format(m, n, nnz)
	{
		long nnz_per_row = nnz / m;
		if (nnz_per_row < 1)
			nnz_per_row = 1;
		NNZ_PER_THREAD = nnz_per_row;

		nnz_per_block = BLOCK_SIZE * NNZ_PER_THREAD;
		nnz_per_warp = 32 * NNZ_PER_THREAD;
		// long i, j;

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
				// if (degree != NNZ_PER_THREAD)
					// error("degree == %ld", degree);
				// degree = NNZ_PER_THREAD * ((degree + NNZ_PER_THREAD - 1) / NNZ_PER_THREAD);
				row_ptr_h[i] = degree;
			}
		}
		printf("NNZ_PER_THREAD = %ld\n", NNZ_PER_THREAD);
		row_ptr_h[m] = 0;
		scan_reduce(row_ptr_h, row_ptr_h, m+1, 0, 1, 0);
		nnz_extended = row_ptr_h[m];
		printf("nnz_extended=%ld\n", nnz_extended);
		ja_h = (typeof(ja_h)) malloc(nnz_extended * sizeof(*ja_h));
		a_h = (typeof(a_h)) malloc(nnz_extended * sizeof(*a_h));
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


		// num_threads = (nnz_extended + NNZ_PER_THREAD - 1) / NNZ_PER_THREAD;
		num_threads = m;
		num_active_threads = m;
		num_active_thread_warps = (num_active_threads + 32 - 1) / 32;
		num_thread_blocks = (num_threads + BLOCK_SIZE - 1) / BLOCK_SIZE;
		num_threads = num_thread_blocks * BLOCK_SIZE;
		num_thread_warps = num_threads / 32;
		printf("num_threads=%d, thread_block_size=%d, num_thread_blocks=%d, num_thread_warps=%d, num_active_threads=%d, num_active_thread_warps=%d\n",
				num_threads, BLOCK_SIZE, num_thread_blocks, num_thread_warps, num_active_threads, num_active_thread_warps);

		cuda_assert(cudaMalloc(&row_ptr_d, (m+1) * sizeof(*row_ptr_d)));
		cuda_assert(cudaMalloc(&ja_d, nnz_extended * sizeof(*ja_d)));
		cuda_assert(cudaMalloc(&a_d, nnz_extended * sizeof(*a_d)));
		cuda_assert(cudaMalloc(&x_d, n * sizeof(*x_d)));
		cuda_assert(cudaMalloc(&y_d, m * sizeof(*y_d)));

		x_h = (typeof(x_h)) malloc(n * sizeof(*x_h));
		y_h = (typeof(y_h)) malloc(m * sizeof(*y_h));

		_Pragma("omp parallel")
		{
			long j;
			_Pragma("omp for")
			// for (j=0;j<(num_thread_blocks-1)*nnz_per_block;j+=32*NNZ_PER_THREAD)
			// for (j=0;j<(num_thread_warps-1)*nnz_per_warp;j+=32*NNZ_PER_THREAD)
			for (j=0;j<(num_active_thread_warps-1)*nnz_per_warp;j+=32*NNZ_PER_THREAD)
			{
				long j_e = j + 32*NNZ_PER_THREAD;
				if (j_e < nnz_extended)
				{
					transpose(&a_h[j], 32, NNZ_PER_THREAD);
					transpose(&ja_h[j], 32, NNZ_PER_THREAD);
					// transpose_split_rows(&a_h[j], 32, NNZ_PER_THREAD, 2);
					// transpose_split_rows(&ja_h[j], 32, NNZ_PER_THREAD, 2);
				}
			}
		}
		// int last_warp_num_active_threads = num_active_threads % 32;
		// if (last_warp_num_active_threads)
		// {
			// long j = (num_active_thread_warps-1)*nnz_per_warp;
			// transpose(&a_h[j], last_warp_num_active_threads, NNZ_PER_THREAD);
			// transpose(&ja_h[j], last_warp_num_active_threads, NNZ_PER_THREAD);
		// }

		/* _Pragma("omp parallel")
		{
			long j;
			_Pragma("omp for")
			for (j=0;j<(num_thread_blocks-1)*nnz_per_block;j+=BLOCK_SIZE*NNZ_PER_THREAD)
			{
				long j_e = j + BLOCK_SIZE*NNZ_PER_THREAD;
				if (j_e < nnz_extended)
				{
					transpose(&a_h[j], BLOCK_SIZE, NNZ_PER_THREAD);
					transpose(&ja_h[j], BLOCK_SIZE, NNZ_PER_THREAD);
				}
			}
		}
		_Pragma("omp parallel")
		{
			long j;
			_Pragma("omp for")
			for (j=(num_thread_blocks-1)*nnz_per_block;j<num_active_thread_warps*nnz_per_warp;j+=32*NNZ_PER_THREAD)
			{
				long j_e = j + 32*NNZ_PER_THREAD;
				if (j_e < nnz_extended)
				{
					transpose(&a_h[j], 32, NNZ_PER_THREAD);
					transpose(&ja_h[j], 32, NNZ_PER_THREAD);
				}
			}
		}
		// int last_warp_num_active_threads = num_active_threads % 32;
		// if (last_warp_num_active_threads)
		// {
			// long j = (num_active_thread_warps-1)*nnz_per_warp;
			// transpose(&a_h[j], last_warp_num_active_threads, NNZ_PER_THREAD);
			// transpose(&ja_h[j], last_warp_num_active_threads, NNZ_PER_THREAD);
		// } */


		cuda_assert(cudaMemcpy(row_ptr_d, row_ptr_h, (m+1) * sizeof(*row_ptr_d), cudaMemcpyHostToDevice));
		cuda_assert(cudaMemcpy(ja_d, ja_h, nnz_extended * sizeof(*ja_d), cudaMemcpyHostToDevice));
		cuda_assert(cudaMemcpy(a_d, a_h, nnz_extended * sizeof(*a_d), cudaMemcpyHostToDevice));
	}

	~CSRArrays()
	{
		cuda_assert(cudaFree(row_ptr_d));
		cuda_assert(cudaFree(ja_d));
		cuda_assert(cudaFree(a_d));
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
	csr->mem_footprint = csr->nnz_extended * (sizeof(ValueType) + sizeof(INT_T)) + (m+1) * sizeof(INT_T);
	char *format_name;
	format_name = (char *)malloc(100*sizeof(char));
	snprintf(format_name, 100, "Custom_CSR_CUDA_one_row_per_thread_b%d_nnz%ld", BLOCK_SIZE, NNZ_PER_THREAD);
	csr->format_name = format_name;
	return csr;
}


//==========================================================================================================================================
//= CSR Custom
//==========================================================================================================================================


template<typename T>
inline
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
spmv_last_block(group_t g, INT_T * row_ptr, INT_T * ja, ValueType * a, int m, int n, int nnz, ValueType * restrict x, ValueType * restrict y, int num_active_threads)
{
	// extern __shared__ char sm[];
	const int tid = blockIdx.x * BLOCK_SIZE + threadIdx.x;
	const int tidw = g.thread_rank();   // Group lane.
	const int wid = tid / g.size();
	__attribute__((unused)) int i, j, jj, j_s, j_e, j_w_s, j_w_e, k;
	double sum = 0;
	int num_active_thread_warps = (num_active_threads + 32 - 1) / 32;
	i = tid;
	if (wid < num_active_thread_warps - 1)
	{
		j_w_s = row_ptr[wid * g.size()];
		j_w_e = row_ptr[(wid+1) * g.size()];
		// j_s = row_ptr[i];
		// j_e = row_ptr[i+1];
		// for (j=row_ptr[i],jj=j;j<row_ptr[i+1];j++,jj++)
		// for (j=j_s,jj=j_w_s+tidw;j<j_e;j++,jj+=g.size())
		for (jj=j_w_s+tidw;jj<j_w_e;jj+=g.size())
		// for (jj=j_w_s+2*tidw;jj<j_w_e;jj+=2*g.size())
		{
			sum = __fma_rn(a[jj], x[ja[jj]], sum);
			// sum = __fma_rn(a[jj], x[ja[jj]], sum);
			// sum = __fma_rn(a[jj+1], x[ja[jj+1]], sum);
		}
		y[i] = sum;
	}
	else
	{
		if (i < m)
		// if (tidw < last_warp_num_active_threads)
		{
			for (j=row_ptr[i],jj=j;j<row_ptr[i+1];j++,jj++)
			// for (j=row_ptr[i],jj=j_w_s+tidw;j<row_ptr[i+1];j++,jj+=last_warp_num_active_threads)
			// for (j=row_ptr[i],jj=j_b_s+threadIdx.x;j<row_ptr[i+1];j++,jj+=BLOCK_SIZE)
			{
				sum = __fma_rn(a[jj], x[ja[jj]], sum);
			}
			y[i] = sum;
		}
	}
}


template <typename group_t>
inline
__device__
void
spmv_full_block(group_t g, INT_T * row_ptr, INT_T * ja, ValueType * a, int m, int n, int nnz, ValueType * restrict x, ValueType * restrict y)
{
	extern __shared__ char sm[];
	const int tid = blockIdx.x * BLOCK_SIZE + threadIdx.x;
	const int tidw = g.thread_rank();   // Group lane.
	const int wid = tid / g.size();
	__attribute__((unused)) int i, j, jj, j_s, j_e, j_w_s, j_w_e, k;
	double sum = 0;
	j_w_s = row_ptr[wid * g.size()];
	j_w_e = row_ptr[(wid+1) * g.size()];
	// j_w_s = wid * g.size() * 8;
	// j_w_e = j_w_s + 32 * 8;
	// if (threadIdx.x % 2)
		// *(reinterpret_cast<double2*>(&sm[threadIdx.x])) = *(reinterpret_cast<double2*>(&a[j_w_s+threadIdx.x]));
	// __syncthreads();
	i = tid;
	// j_s = row_ptr[i];
	// j_e = row_ptr[i+1];
	// for (jj=row_ptr[i];jj<row_ptr[i+1];j++)
	// for (j=j_s,jj=j_w_s+tidw;j<j_e;j++,jj+=g.size())
	for (jj=j_w_s+tidw;jj<j_w_e;jj+=g.size())
	// for (jj=j_w_s+2*tidw;jj<j_w_e;jj+=2*g.size())
	{
		// double2 av = *(reinterpret_cast<double2*>(&a[jj]));
		// int2 jav = *(reinterpret_cast<int2*>(&ja[jj]));
		// sum = __fma_rn(av.x, x[jav.x], sum);
		// sum = __fma_rn(av.y, x[jav.y], sum);
		sum = __fma_rn(a[jj], x[ja[jj]], sum);
		// sum = __fma_rn(a[jj+1], x[ja[jj+1]], sum);
		// if (wid == 0)
			// printf("tidw=%d, i=%d, jj=%d, a[jj]=%g, ja[jj]=%d\n", tidw, i, jj, a[jj], ja[jj]);
	}
	y[i] = sum;
}


__global__
void
gpu_kernel_spmv_row_indices_continuous(INT_T * row_ptr, INT_T * ja, ValueType * a, int m, int n, int nnz, ValueType * restrict x, ValueType * restrict y, int num_active_threads)
{
	thread_block_tile<32> tile32 = tiled_partition<32>(this_thread_block());
	const int grid_size = gridDim.x;
	const int block_id = blockIdx.x;
	if (block_id != grid_size - 1)
		spmv_full_block(tile32, row_ptr, ja, a, m, n, nnz, x, y);
	else
		spmv_last_block(tile32, row_ptr, ja, a, m, n, nnz, x, y, num_active_threads);
}


void
compute_csr(CSRArrays * restrict csr, ValueType * restrict x, ValueType * restrict y)
{
	dim3 block_dims(BLOCK_SIZE);
	dim3 grid_dims(csr->num_thread_blocks);
	long shared_mem_size = 0;
	// shared_mem_size = BLOCK_SIZE * (sizeof(ValueType));
	// shared_mem_size = BLOCK_SIZE * (sizeof(ValueType) + sizeof(INT_T));

	if (csr->x == NULL)
	{
		printf("Grid : {%d, %d, %d} blocks. Blocks : {%d, %d, %d} threads.\n", grid_dims.x, grid_dims.y, grid_dims.z, block_dims.x, block_dims.y, block_dims.z);
		csr->x = x;
		memcpy(csr->x_h, x, csr->n * sizeof(ValueType));
		cuda_assert(cudaMemcpy(csr->x_d, csr->x_h, csr->n * sizeof(*csr->x_d), cudaMemcpyHostToDevice));
	}

	// cudaMemset(csr->y_d, 0, csr->m * sizeof(csr->y_d));

	// cudaFuncCachePreferNone:   no preference for shared memory or L1 (default);
	// cudaFuncCachePreferShared: prefer larger shared memory and smaller L1 cache;
	// cudaFuncCachePreferL1:     prefer larger L1 cache and smaller shared memory;
	// cuda_assert(cudaFuncSetCacheConfig(gpu_kernel_spmv_row_indices_continuous, cudaFuncCachePreferL1));
	// cuda_assert(cudaFuncSetCacheConfig(gpu_kernel_spmv_row_indices_continuous, cudaFuncCachePreferShared));
	gpu_kernel_spmv_row_indices_continuous<<<grid_dims, block_dims, shared_mem_size>>>(csr->row_ptr_d, csr->ja_d, csr->a_d, csr->m, csr->n, csr->nnz_extended, csr->x_d, csr->y_d, csr->num_active_threads);
	cuda_assert(cudaPeekAtLastError());
	cuda_assert(cudaDeviceSynchronize());

	// exit(0);

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

