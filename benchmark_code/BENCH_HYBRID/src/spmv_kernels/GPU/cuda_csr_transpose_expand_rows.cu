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
	#define FUNCTOOLS_GEN_SUFFIX  CONCAT(_CSR_GEN_add_i, CSR_GEN_SUFFIX)
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
	// #define NNZ_PER_THREAD  6
	#define NNZ_PER_THREAD  5
#endif

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

#ifndef DETAILED_TIMING
	// #define DETAILED_TIMING 1
	#define DETAILED_TIMING 0
#endif

// Set this to 1 to enable the standalone GPU optimization (D2D full transfer of y to x)
// Set this to 0 to use the standard method (full H2D transfer of x every iteration)
#define STANDALONE_ITERATIVE_OPTIMIZATION 1
// #define STANDALONE_ITERATIVE_OPTIMIZATION 0

// Set this to 1 to enable the iterative optimization (partial H2D + D2D transfer)
// Set this to 0 to use the standard method (full H2D transfer every iteration)
#define HYBRID_ITERATIVE_OPTIMIZATION 1
// #define HYBRID_ITERATIVE_OPTIMIZATION 0


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


struct Cuda_CSR_Transpose_Expand_Rows_Arrays : Matrix_Format
{
	long m_cpu = -1, max_mn = -1;
	// this offset is used for hybrid mode to indicate the start of the GPU part
	long offset;
	long nnz_expanded;
	bool is_first_iteration = true;
	bool is_last_iteration = false;

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
	#ifdef VECTOR_ALLOC_EXPLICIT
		ValueType * x_d = NULL;
		ValueType * y_d = NULL;
	#endif

	cudaStream_t stream;
	#ifdef VECTOR_ALLOC_EXPLICIT
		cudaStream_t memset_stream;
		cudaStream_t h2d_stream;
	#endif
	cudaEvent_t start_event, stop_event;
	cudaEvent_t memset_event;
	#ifdef VECTOR_ALLOC_EXPLICIT
		cudaEvent_t h2d_event, kernel_event, d2h_event, memset_done_event, h2d_done_event, pure_memset_start, pure_memset_stop;
	#endif
	float last_duration_ms = 0;

	double time_h2d_ms = 0;
	double time_memset_ms = 0;
	double time_kernel_ms = 0;
	double time_d2h_ms = 0;
	double time_pure_memset_ms = 0;
	long call_count = 0;

	int max_smem_per_block, multiproc_count, max_threads_per_block, warp_size, max_threads_per_multiproc, max_block_dim_x, max_persistent_l2_cache, max_num_threads;
	int num_threads;
	int thread_block_size;
	int num_thread_blocks;
	int num_thread_warps;

	Cuda_CSR_Transpose_Expand_Rows_Arrays(INT_T * row_ptr, INT_T * ja, ValueType * a, long m, long n, long nnz, long m_cpu = -1) : Matrix_Format(m, n, nnz), m_cpu(m_cpu)
	{
		const long nnz_per_block = BLOCK_SIZE * NNZ_PER_THREAD;
		const long nnz_per_warp = 32 * NNZ_PER_THREAD;
		double time_balance;
		long i;

		gpuCudaErrorCheck(cudaDeviceGetAttribute(&max_smem_per_block, cudaDevAttrMaxSharedMemoryPerBlock, 0));
		gpuCudaErrorCheck(cudaDeviceGetAttribute(&multiproc_count, cudaDevAttrMultiProcessorCount, 0));
		gpuCudaErrorCheck(cudaDeviceGetAttribute(&max_threads_per_block, cudaDevAttrMaxThreadsPerBlock , 0));
		gpuCudaErrorCheck(cudaDeviceGetAttribute(&warp_size, cudaDevAttrWarpSize , 0));
		gpuCudaErrorCheck(cudaDeviceGetAttribute(&max_threads_per_multiproc, cudaDevAttrMaxThreadsPerMultiProcessor, 0));
		gpuCudaErrorCheck(cudaDeviceGetAttribute(&max_block_dim_x, cudaDevAttrMaxBlockDimX, 0));
		gpuCudaErrorCheck(cudaDeviceGetAttribute(&max_persistent_l2_cache, cudaDevAttrMaxPersistingL2CacheSize, 0));
		max_num_threads = max_threads_per_multiproc * multiproc_count;
		printf("max_smem_per_block(bytes)=%d\n", max_smem_per_block);
		printf("multiproc_count=%d\n", multiproc_count);
		printf("max_threads_per_block=%d\n", max_threads_per_block);
		printf("warp_size=%d\n", warp_size);
		printf("max_threads_per_multiproc=%d\n", max_threads_per_multiproc);
		printf("max_block_dim_x=%d\n", max_block_dim_x);
		printf("max_persistent_l2_cache=%d\n", max_persistent_l2_cache);
		printf("max_num_threads=%d\n", max_num_threads);

		gpuCudaErrorCheck(cudaStreamCreate(&stream));
		#ifdef VECTOR_ALLOC_EXPLICIT
			gpuCudaErrorCheck(cudaStreamCreate(&memset_stream));
			gpuCudaErrorCheck(cudaStreamCreate(&h2d_stream));
		#endif
			gpuCudaErrorCheck(cudaEventCreate(&start_event));
			gpuCudaErrorCheck(cudaEventCreate(&stop_event));
			gpuCudaErrorCheck(cudaEventCreate(&memset_event));
		#ifdef VECTOR_ALLOC_EXPLICIT
			gpuCudaErrorCheck(cudaEventCreate(&h2d_event));
			gpuCudaErrorCheck(cudaEventCreate(&kernel_event));
			gpuCudaErrorCheck(cudaEventCreate(&d2h_event));
			gpuCudaErrorCheck(cudaEventCreate(&memset_done_event));
			gpuCudaErrorCheck(cudaEventCreate(&h2d_done_event));
			gpuCudaErrorCheck(cudaEventCreate(&pure_memset_start));
			gpuCudaErrorCheck(cudaEventCreate(&pure_memset_stop));
			gpuCudaErrorCheck(cudaEventRecord(memset_done_event, memset_stream));
			gpuCudaErrorCheck(cudaEventRecord(h2d_done_event, h2d_stream));
		#endif

		thread_block_size = BLOCK_SIZE;

		gpuCudaErrorCheck(cudaMallocHost(&row_ptr_h, (m+1) * sizeof(*row_ptr_h)));
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
		// printf("nnz_expanded=%ld\n", nnz_expanded);
		printf("nnz = %ld, nnz_expanded = %ld (padding %.2f),\tnum_threads=%d, thread_block_size=%d, num_thread_blocks=%d\n", nnz, nnz_expanded, (nnz_expanded - nnz)*100.0/nnz, num_threads, BLOCK_SIZE, num_thread_blocks);

		ja_h = NULL;
		a_h = NULL;
		gpuCudaErrorCheck(cudaMallocHost(&ja_h, nnz_expanded * sizeof(*ja_h)));
		gpuCudaErrorCheck(cudaMallocHost(&a_h, nnz_expanded * sizeof(*a_h)));
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

		gpuCudaErrorCheck(cudaMallocHost(&thread_warp_i_s, num_thread_warps * sizeof(*thread_warp_i_s)));
		gpuCudaErrorCheck(cudaMallocHost(&thread_warp_i_e, num_thread_warps * sizeof(*thread_warp_i_e)));

		gpuCudaErrorCheck(cudaMallocHost(&thread_i_s, num_threads * sizeof(*thread_i_s)));

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

		gpuCudaErrorCheck(cudaMalloc(&row_ptr_d, (m+1) * sizeof(*row_ptr_d)));
		gpuCudaErrorCheck(cudaMalloc(&ja_d, nnz_expanded * sizeof(*ja_d)));
		gpuCudaErrorCheck(cudaMalloc(&a_d, nnz_expanded * sizeof(*a_d)));
		gpuCudaErrorCheck(cudaMalloc(&thread_warp_i_s_d, num_thread_warps * sizeof(*thread_warp_i_s_d)));
		gpuCudaErrorCheck(cudaMalloc(&thread_warp_i_e_d, num_thread_warps * sizeof(*thread_warp_i_e_d)));
		gpuCudaErrorCheck(cudaMalloc(&thread_i_s_d, num_threads * sizeof(*thread_i_s_d)));
		
		// when in standalone mode, m is actual number of rows
		// when in hybrid mode, m is the number of the GPU part of y vector, therefore we need to account for m_cpu too.
		offset = (m_cpu != -1) ? m_cpu : 0;
		long total_m = (m_cpu != -1) ? (m + m_cpu) : m;
		max_mn = (total_m > n) ? total_m : n;
		#ifdef VECTOR_ALLOC_EXPLICIT
			gpuCudaErrorCheck(cudaMalloc(&x_d, max_mn * sizeof(*x_d)));
			gpuCudaErrorCheck(cudaMalloc(&y_d, max_mn * sizeof(*y_d)));
		#endif

		_Pragma("omp parallel")
		{
			long i_s, i_e, j; // jj;
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

		cudaEvent_t setup_h2d_start, setup_h2d_stop;
		gpuCudaErrorCheck(cudaEventCreate(&setup_h2d_start));
		gpuCudaErrorCheck(cudaEventCreate(&setup_h2d_stop));
		gpuCudaErrorCheck(cudaEventRecord(setup_h2d_start, stream));

		gpuCudaErrorCheck(cudaMemcpyAsync(row_ptr_d, row_ptr_h, (m+1) * sizeof(*row_ptr_d), cudaMemcpyHostToDevice, stream));
		gpuCudaErrorCheck(cudaMemcpyAsync(ja_d, ja_h, nnz_expanded * sizeof(*ja_d), cudaMemcpyHostToDevice, stream));
		gpuCudaErrorCheck(cudaMemcpyAsync(a_d, a_h, nnz_expanded * sizeof(*a_d), cudaMemcpyHostToDevice, stream));
		gpuCudaErrorCheck(cudaMemcpyAsync(thread_warp_i_s_d, thread_warp_i_s, num_thread_warps * sizeof(*thread_warp_i_s_d), cudaMemcpyHostToDevice, stream));
		gpuCudaErrorCheck(cudaMemcpyAsync(thread_warp_i_e_d, thread_warp_i_e, num_thread_warps * sizeof(*thread_warp_i_e_d), cudaMemcpyHostToDevice, stream));
		gpuCudaErrorCheck(cudaMemcpyAsync(thread_i_s_d, thread_i_s, num_threads * sizeof(*thread_i_s_d), cudaMemcpyHostToDevice, stream));

		gpuCudaErrorCheck(cudaEventRecord(setup_h2d_stop, stream));
		gpuCudaErrorCheck(cudaEventSynchronize(setup_h2d_stop));
		float setup_h2d_ms = 0;
		gpuCudaErrorCheck(cudaEventElapsedTime(&setup_h2d_ms, setup_h2d_start, setup_h2d_stop));
		
		#if DETAILED_TIMING
			double total_bytes = ((m+1) * sizeof(*row_ptr_d)) + (nnz_expanded * sizeof(*ja_d)) + (nnz_expanded * sizeof(*a_d)) + 
			                     (num_thread_warps * sizeof(*thread_warp_i_s_d)) + (num_thread_warps * sizeof(*thread_warp_i_e_d)) + 
			                     (num_threads * sizeof(*thread_i_s_d));
			double bw_GBs = (setup_h2d_ms > 0) ? (total_bytes / 1e6) / setup_h2d_ms : 0;
			printf("Sparse Matrix H2D Transfer: %.4lf ms (%.2lf MB - %.2lf GB/s)\n", setup_h2d_ms, total_bytes / 1e6, bw_GBs);
		#endif

		gpuCudaErrorCheck(cudaEventDestroy(setup_h2d_start));
		gpuCudaErrorCheck(cudaEventDestroy(setup_h2d_stop));
	}

	~Cuda_CSR_Transpose_Expand_Rows_Arrays()
	{
		gpuCudaErrorCheck(cudaFreeHost(thread_warp_i_s));
		gpuCudaErrorCheck(cudaFreeHost(thread_warp_i_e));
		gpuCudaErrorCheck(cudaFreeHost(thread_i_s));

		gpuCudaErrorCheck(cudaFree(row_ptr_d));
		gpuCudaErrorCheck(cudaFree(ja_d));
		gpuCudaErrorCheck(cudaFree(a_d));
		gpuCudaErrorCheck(cudaFree(thread_warp_i_s_d));
		gpuCudaErrorCheck(cudaFree(thread_warp_i_e_d));
		gpuCudaErrorCheck(cudaFree(thread_i_s_d));
		#ifdef VECTOR_ALLOC_EXPLICIT
			gpuCudaErrorCheck(cudaFree(x_d));
			gpuCudaErrorCheck(cudaFree(y_d));
		#endif

		gpuCudaErrorCheck(cudaFreeHost(row_ptr_h));
		gpuCudaErrorCheck(cudaFreeHost(ja_h));
		gpuCudaErrorCheck(cudaFreeHost(a_h));

		gpuCudaErrorCheck(cudaStreamDestroy(stream));
		#ifdef VECTOR_ALLOC_EXPLICIT
			gpuCudaErrorCheck(cudaStreamDestroy(memset_stream));
			gpuCudaErrorCheck(cudaStreamDestroy(h2d_stream));
		#endif
		gpuCudaErrorCheck(cudaEventDestroy(start_event));
		gpuCudaErrorCheck(cudaEventDestroy(stop_event));
		gpuCudaErrorCheck(cudaEventDestroy(memset_event));
		#ifdef VECTOR_ALLOC_EXPLICIT
			gpuCudaErrorCheck(cudaEventDestroy(h2d_event));
			gpuCudaErrorCheck(cudaEventDestroy(kernel_event));
			gpuCudaErrorCheck(cudaEventDestroy(d2h_event));
			gpuCudaErrorCheck(cudaEventDestroy(memset_done_event));
			gpuCudaErrorCheck(cudaEventDestroy(h2d_done_event));
			gpuCudaErrorCheck(cudaEventDestroy(pure_memset_start));
			gpuCudaErrorCheck(cudaEventDestroy(pure_memset_stop));
		#endif
	}

	void spmv(ValueType * x, ValueType * y) override;

	void synchronize() override { 
		gpuCudaErrorCheck(cudaStreamSynchronize(stream));
		#ifdef VECTOR_ALLOC_EXPLICIT
			gpuCudaErrorCheck(cudaStreamSynchronize(h2d_stream));
		#endif
		gpuCudaErrorCheck(cudaEventElapsedTime(&last_duration_ms, start_event, stop_event));
		
		#if DETAILED_TIMING
			#ifdef VECTOR_ALLOC_EXPLICIT
				gpuCudaErrorCheck(cudaStreamSynchronize(memset_stream));
				float h2d_ms = 0, memset_ms = 0, kernel_ms = 0, d2h_ms = 0, pure_memset = 0;
				if (last_duration_ms > 0) {
					gpuCudaErrorCheck(cudaEventElapsedTime(&h2d_ms, start_event, h2d_event));
					gpuCudaErrorCheck(cudaEventElapsedTime(&memset_ms, h2d_event, memset_event));
					gpuCudaErrorCheck(cudaEventElapsedTime(&kernel_ms, memset_event, kernel_event));
					gpuCudaErrorCheck(cudaEventElapsedTime(&d2h_ms, kernel_event, d2h_event));
					gpuCudaErrorCheck(cudaEventElapsedTime(&pure_memset, pure_memset_start, pure_memset_stop));
				}
				time_h2d_ms += h2d_ms;
				time_memset_ms += memset_ms;
				time_kernel_ms += kernel_ms;
				time_d2h_ms += d2h_ms;
				time_pure_memset_ms += pure_memset;
			#else
				float memset_ms = 0, kernel_ms = 0;
				if (last_duration_ms > 0) {
					gpuCudaErrorCheck(cudaEventElapsedTime(&memset_ms, start_event, memset_event));
					gpuCudaErrorCheck(cudaEventElapsedTime(&kernel_ms, memset_event, stop_event));
				}
				time_memset_ms += memset_ms;
				time_kernel_ms += kernel_ms;
			#endif
			call_count++;
		#endif
	}

	double get_last_duration() override { return (double)last_duration_ms; }
	void set_last_iteration(bool is_last) override { is_last_iteration = is_last;}
	void statistics_start() override;
	int statistics_print_data(__attribute__((unused)) char * buf, __attribute__((unused)) long buf_n) override;

	void issue_h2d_for_next_iteration(ValueType * y) override {
		#ifdef VECTOR_ALLOC_EXPLICIT
			if (m_cpu == -1) return;
			
			#if HYBRID_ITERATIVE_OPTIMIZATION
				long h2d_copy_elements = (m_cpu < n) ? m_cpu : n;
				if (h2d_copy_elements > 0) {
					// Precaution: guarantee we don't overwrite x_d before current iteration's kernel finishes evaluating it!
					gpuCudaErrorCheck(cudaStreamWaitEvent(h2d_stream, kernel_event, 0));
					
					// Pipeline the async Host array push mapping directly back to Device Vector x_d
					gpuCudaErrorCheck(cudaMemcpyAsync(x_d, y, h2d_copy_elements * sizeof(*x_d), cudaMemcpyHostToDevice, h2d_stream));
					
					// Record the proactive dispatch
					gpuCudaErrorCheck(cudaEventRecord(h2d_done_event, h2d_stream));
				}
			#endif
		#endif
	}
};


void compute_csr(Cuda_CSR_Transpose_Expand_Rows_Arrays * restrict csr, ValueType * restrict x , ValueType * restrict y);

void
Cuda_CSR_Transpose_Expand_Rows_Arrays::spmv(ValueType * x, ValueType * y)
{
	compute_csr(this, x, y);
}


struct Matrix_Format *
cuda_csr_transpose_expand_rows_to_format(INT_T * row_ptr, INT_T * col_ind, ValueTypeReference * values, long m, long n, long nnz, long symmetric, long symmetry_expanded, long m_cpu)
{
	if (symmetric && !symmetry_expanded)
		error("symmetric matrices not supported by this format, expand symmetry");
	struct Cuda_CSR_Transpose_Expand_Rows_Arrays * csr = new Cuda_CSR_Transpose_Expand_Rows_Arrays(row_ptr, col_ind, values, m, n, nnz, m_cpu);
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
	// const int widb = tidb / 32;
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
spmv_full_warp(group_t g, int i_s, int j_s, int j_b_s, int j_w_s, INT_T * row_ptr, INT_T * ja, ValueType * a, ValueType * restrict x, ValueType * restrict y)
{
	// extern __shared__ double x_smem[];
	const int tidw = g.thread_rank();   // Group lane.
	int i, j, jj;
	double sum = 0;
	// double x_buf;
	int j_e = j_s + NNZ_PER_THREAD;
	int single_row;
	// jj = j_s;
	// jj = j_w_s + tidw;
	// __pipeline_memcpy_async(&x_smem[threadIdx.x], &x[ja[jj]], 8);
	// __pipeline_commit();
	i = i_s;
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
		// __syncthreads();
	}
	g.match_all(i_s, single_row);   // 'single_row' is passed as reference!!! Passing as pointer gives compilation error.
	if (single_row)
	{
		sum = reduce_warp_single_row(g, sum);
		if (tidw == 0)
			atomicAdd(&y[i_s], sum);
	}
	else
	{
		reduce_warp(g, i, sum, y);
	}
}


__device__
void
spmv_full_block(INT_T * thread_i_s, INT_T * thread_warp_i_s, INT_T * thread_warp_i_e, INT_T * row_ptr, INT_T * ja, ValueType * a, long m, long n, long nnz, ValueType * restrict x, ValueType * restrict y)
{
	// extern __shared__ char sm[];
	const int tid = cuda_get_thread_num_bc();
	// const int tid = blockIdx.x * BLOCK_SIZE + threadIdx.x;
	// const int tidb = threadIdx.x;
	// const int tidw = tidb % 32;
	const int wid = tid / 32;
	// const int widb = tidb / 32;
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

	// i_s = thread_i_s[tid];
	i_s = thread_warp_i_s[wid];
	i_e = thread_warp_i_e[wid];
	i_s = binary_search_gpu(row_ptr, i_s, i_e, j_s);

	thread_block_tile<32> tile32 = tiled_partition<32>(this_thread_block());
	spmv_full_warp(tile32, i_s, j_s, j_b_s, j_w_s, row_ptr, ja, a, x, y);
}


__global__
void
gpu_kernel_spmv_row_indices_continuous(INT_T * thread_i_s, INT_T * thread_warp_i_s, INT_T * thread_warp_i_e, INT_T * row_ptr, INT_T * ja, ValueType * a, long m, long n, long nnz, ValueType * restrict x, ValueType * restrict y)
{
	int grid_size = gridDim.x;
	int block_id = blockIdx.x;
	if (block_id != grid_size - 1)
		spmv_full_block(thread_i_s, thread_warp_i_s, thread_warp_i_e, row_ptr, ja, a, m, n, nnz, x, y);
	else
		spmv_last_block(thread_i_s, thread_warp_i_s, thread_warp_i_e, row_ptr, ja, a, m, n, nnz, x, y);
}


void
compute_csr(Cuda_CSR_Transpose_Expand_Rows_Arrays * restrict csr, ValueType * restrict x, ValueType * restrict y)
{
	gpuCudaErrorCheck(cudaEventRecord(csr->start_event, csr->stream));

	dim3 block_dims(BLOCK_SIZE);
	dim3 grid_dims(csr->num_thread_blocks);
	long shared_mem_size = 0;
	// shared_mem_size = BLOCK_SIZE * (sizeof(ValueType));
	// shared_mem_size = BLOCK_SIZE * (sizeof(ValueType) + sizeof(INT_T));
	// shared_mem_size = BLOCK_SIZE * NNZ_PER_THREAD * sizeof(INT_T);

	#ifdef VECTOR_ALLOC_EXPLICIT
		// === EXPLICIT MODE: Full H2D/D2H pipeline with device buffers x_d, y_d ===

		if (csr->m_cpu == -1) // --- STANDALONE MODE ---
		{
			#if STANDALONE_ITERATIVE_OPTIMIZATION
				if (csr->is_first_iteration) {
					gpuCudaErrorCheck(cudaMemcpyAsync(csr->x_d, x, csr->n * sizeof(*csr->x_d), cudaMemcpyHostToDevice, csr->stream));
				}
				// else {
				// // Subsequent iterations: Do nothing for H2D. x_d is populated via D2D from the previous iteration.
				// }
			#else
				// Old method: Always transfer full x from Host
				gpuCudaErrorCheck(cudaMemcpyAsync(csr->x_d, x, csr->n * sizeof(*csr->x_d), cudaMemcpyHostToDevice, csr->stream));
			#endif
		}
		else // --- HYBRID MODE ---
		{
			#if HYBRID_ITERATIVE_OPTIMIZATION
				if (csr->is_first_iteration) {
					// First iteration must bootstrap logically
					gpuCudaErrorCheck(cudaMemcpyAsync(csr->x_d, x, csr->n * sizeof(*csr->x_d), cudaMemcpyHostToDevice, csr->stream));
				} else {
					// Successive iterations evaluate off proactive dispatch linked inside h2d_done_event 
					gpuCudaErrorCheck(cudaStreamWaitEvent(csr->stream, csr->h2d_done_event, 0));
				}
			#else
				// Standard method: transfer full x every time
				gpuCudaErrorCheck(cudaMemcpyAsync(csr->x_d, x, csr->n * sizeof(*csr->x_d), cudaMemcpyHostToDevice, csr->stream));
			#endif
		}

		#if DETAILED_TIMING
			gpuCudaErrorCheck(cudaEventRecord(csr->h2d_event, csr->stream));
		#endif

		if (csr->is_first_iteration)
		{
			printf("Grid : {%d, %d, %d} blocks. Blocks : {%d, %d, %d} threads.\n", grid_dims.x, grid_dims.y, grid_dims.z, block_dims.x, block_dims.y, block_dims.z);
			if (csr->m_cpu == -1)
				printf("Standalone configuration (Optimization: %s)\n", STANDALONE_ITERATIVE_OPTIMIZATION ? "ON" : "OFF");
			else
				printf("Hybrid configuration: m_cpu is %ld (Optimization: %s)\n", csr->m_cpu, HYBRID_ITERATIVE_OPTIMIZATION ? "ON" : "OFF");
			csr->is_first_iteration = false;
			csr->x = x;
		}
		
		// The memset operation differs between standalone and hybrid modes.
		// For standalone mode, this memset just needs to happen before computation starts, so we place it in the csr->stream
		// For hybrid mode, it runs independently from the main computation stream, on a separate stream, to overlap with the H2D transfer of the next iteration regarding the CPU side of y_i that will now become x_i+1.
		if (csr->m_cpu != -1) // HYBRID MODE
		{
			#if DETAILED_TIMING
				gpuCudaErrorCheck(cudaEventRecord(csr->pure_memset_start, csr->memset_stream));
			#endif

			gpuCudaErrorCheck(cudaMemsetAsync(csr->y_d + csr->offset, 0, csr->m * sizeof(*csr->y_d), csr->memset_stream));

			#if DETAILED_TIMING
				gpuCudaErrorCheck(cudaEventRecord(csr->pure_memset_stop, csr->memset_stream));
			#endif

			// Signal the main stream that the memset is done
			gpuCudaErrorCheck(cudaEventRecord(csr->memset_done_event, csr->memset_stream));
		}
		else // STANDALONE MODE
		{
			#if DETAILED_TIMING
				gpuCudaErrorCheck(cudaEventRecord(csr->pure_memset_start, csr->stream));
			#endif

			gpuCudaErrorCheck(cudaMemsetAsync(csr->y_d + csr->offset, 0, csr->m * sizeof(*csr->y_d), csr->stream));

			#if DETAILED_TIMING
				gpuCudaErrorCheck(cudaEventRecord(csr->pure_memset_stop, csr->stream));
			#endif
		}

		if (csr->m_cpu != -1) // HYBRID MODE
		{
			// Wait on the overlapping stream's background memset loop!
			gpuCudaErrorCheck(cudaStreamWaitEvent(csr->stream, csr->memset_done_event, 0));
		}

		// Pointer swapping eliminates the D2D background transfer completely!

		#if DETAILED_TIMING
			// For accurate pure-kernel profiling, we isolate "pre-kernel synchronization" (Memset Wait) here
			gpuCudaErrorCheck(cudaEventRecord(csr->memset_event, csr->stream));
		#endif

		gpu_kernel_spmv_row_indices_continuous<<<grid_dims, block_dims, shared_mem_size, csr->stream>>>(csr->thread_i_s_d, csr->thread_warp_i_s_d, csr->thread_warp_i_e_d, csr->row_ptr_d, csr->ja_d, csr->a_d, csr->m, csr->n, csr->nnz_expanded, csr->x_d, csr->y_d + csr->offset);
		
		// Record completion of Kernel
		gpuCudaErrorCheck(cudaEventRecord(csr->kernel_event, csr->stream));

		gpuCudaErrorCheck(cudaPeekAtLastError());
		// gpuCudaErrorCheck(cudaDeviceSynchronize()); // Removed for async overlap
		
		if (csr->y == NULL)
		{
			csr->y = y;
		}

		bool skip_d2h = false;
		if (csr->m_cpu == -1) {
			#if STANDALONE_ITERATIVE_OPTIMIZATION
				skip_d2h = !csr->is_last_iteration;
			#endif
		}

		if (!skip_d2h) {
			gpuCudaErrorCheck(cudaMemcpyAsync(y + csr->offset, csr->y_d + csr->offset, csr->m * sizeof(*csr->y_d), cudaMemcpyDeviceToHost, csr->stream));
		}
		
		#if DETAILED_TIMING
			gpuCudaErrorCheck(cudaEventRecord(csr->d2h_event, csr->stream));
		#endif

		if (!csr->is_last_iteration) {
			// Zero-cost Ping-Pong Pointer Swap handles DtD perfectly without executing a transfer!
			ValueType * tmp = csr->x_d;
			csr->x_d = csr->y_d;
			csr->y_d = tmp;
		}

	#else
		// === NON-EXPLICIT MODE (MALLOCHOST / MANAGED / MALLOC): Direct host pointer access ===

		if (csr->is_first_iteration)
		{
			printf("Grid : {%d, %d, %d} blocks. Blocks : {%d, %d, %d} threads.\n", grid_dims.x, grid_dims.y, grid_dims.z, block_dims.x, block_dims.y, block_dims.z);
			if (csr->m_cpu == -1)
				printf("Standalone configuration (non-explicit memory)\n");
			else
				printf("Hybrid configuration: m_cpu is %ld (%.2lf MB) (non-explicit memory)\n", csr->m_cpu, csr->m_cpu * 1.0 * sizeof(ValueType) / (1024*1024));
			csr->is_first_iteration = false;
			csr->x = x;
		}

		// Memset y directly (no device buffer)
		gpuCudaErrorCheck(cudaMemsetAsync(y + csr->offset, 0, csr->m * sizeof(*y), csr->stream));

		#if DETAILED_TIMING
			gpuCudaErrorCheck(cudaEventRecord(csr->memset_event, csr->stream));
		#endif

		// Launch kernel with host pointers directly (zero-copy / ATS / unified memory)
		gpu_kernel_spmv_row_indices_continuous<<<grid_dims, block_dims, shared_mem_size, csr->stream>>>(csr->thread_i_s_d, csr->thread_warp_i_s_d, csr->thread_warp_i_e_d, csr->row_ptr_d, csr->ja_d, csr->a_d, csr->m, csr->n, csr->nnz_expanded, x, y + csr->offset);
		
		gpuCudaErrorCheck(cudaPeekAtLastError());

		if (csr->y == NULL)
			csr->y = y;

	#endif

	gpuCudaErrorCheck(cudaEventRecord(csr->stop_event, csr->stream));
}


//==========================================================================================================================================
//= Print Statistics
//==========================================================================================================================================


void
Cuda_CSR_Transpose_Expand_Rows_Arrays::statistics_start()
{
	time_h2d_ms = 0;
	time_memset_ms = 0;
	time_kernel_ms = 0;
	time_d2h_ms = 0;
	time_pure_memset_ms = 0;
	call_count = 0;
	is_first_iteration = true; // need to consider whether we will do it or not
	is_last_iteration = false;
}


int
cuda_csr_transpose_expand_rows_statistics_print_labels(__attribute__((unused)) char * buf, __attribute__((unused)) long buf_n)
{
	// int len = 0;
	// len += snprintf(buf + len, buf_n - len, ",%s,%s,%s,%s", "time_h2d_ms_avg", "time_memset_ms_avg", "time_kernel_ms_avg", "time_d2h_ms_avg");
	// return len;
	return 0;
}


int
Cuda_CSR_Transpose_Expand_Rows_Arrays::statistics_print_data(__attribute__((unused)) char * buf, __attribute__((unused)) long buf_n)
{
	// int len = 0;
	// len += snprintf(buf + len, buf_n - len, ",%g,%g,%g,%g", avg_h2d, avg_memset, avg_kernel, avg_d2h);
	// return len;
	#if DETAILED_TIMING
		double total_h2d_mb = 0, total_d2h_mb = 0;
		long actual_h2d_calls = 0, actual_d2h_calls = 0;

		if (m_cpu == -1) // --- STANDALONE MODE ---
		{ 
			#if STANDALONE_ITERATIVE_OPTIMIZATION
				total_h2d_mb = (n * sizeof(ValueType) / 1e6);
				actual_h2d_calls = 1;
				total_d2h_mb = (m * sizeof(ValueType) / 1e6);
				actual_d2h_calls = 1;
			#else
				total_h2d_mb = call_count * (n * sizeof(ValueType) / 1e6);
				actual_h2d_calls = call_count;
				total_d2h_mb = call_count * (m * sizeof(ValueType) / 1e6);
				actual_d2h_calls = call_count;
			#endif
		} 
		else // --- HYBRID MODE ---
		{
			#if HYBRID_ITERATIVE_OPTIMIZATION
				total_h2d_mb = (n + (call_count > 1 ? (call_count - 1) : 0) * m_cpu) * sizeof(ValueType) / 1e6;
				actual_h2d_calls = call_count;
			#else
				total_h2d_mb = call_count * (n * sizeof(ValueType) / 1e6);
				actual_h2d_calls = call_count;
			#endif
			total_d2h_mb = call_count * (m * sizeof(ValueType) / 1e6);
			actual_d2h_calls = call_count;
		}

		double avg_h2d = (actual_h2d_calls > 0) ? (time_h2d_ms / actual_h2d_calls) : 0;
		double avg_d2h = (actual_d2h_calls > 0) ? (time_d2h_ms / actual_d2h_calls) : 0;

		double avg_memset = (call_count > 0) ? (time_memset_ms / call_count) : 0;
		double avg_pure_memset = (call_count > 0) ? (time_pure_memset_ms / call_count) : 0;
		double avg_kernel = (call_count > 0) ? (time_kernel_ms / call_count) : 0;

		double h2d_bw = (time_h2d_ms > 0) ? (total_h2d_mb / time_h2d_ms) : 0;
		double d2h_bw = (time_d2h_ms > 0) ? (total_d2h_mb / time_d2h_ms) : 0;

		double moved_h2d_mb_avg = (actual_h2d_calls > 0) ? (total_h2d_mb / actual_h2d_calls) : 0;
		double moved_d2h_mb_avg = (actual_d2h_calls > 0) ? (total_d2h_mb / actual_d2h_calls) : 0;
		
		printf("h2d: %.4lf ms (%.2lf MB - %.2lf GB/s), sync/memset: %.4lf ms (pure memset: %.4lf), kernel: %.4lf ms, d2h: %.4lf ms (%.2lf MB - %.2lf GB/s)\n", 
		       avg_h2d, moved_h2d_mb_avg, h2d_bw, avg_memset, avg_pure_memset, avg_kernel, avg_d2h, moved_d2h_mb_avg, d2h_bw);
		double total_time = time_h2d_ms + time_memset_ms + time_kernel_ms + time_d2h_ms;
		printf("Timing summary percentages: h2d: %.2lf%%, memset: %.2lf%%, kernel: %.2lf%%, d2h: %.2lf%%\n", 
		       (time_h2d_ms / total_time) * 100, (time_memset_ms / total_time) * 100, (time_kernel_ms / total_time) * 100, (time_d2h_ms / total_time) * 100);
	#endif
	return 0;
}

#ifndef HYBRID
struct Matrix_Format *
csr_to_format(INT_T * row_ptr, INT_T * col_ind, ValueTypeReference * values, long m, long n, long nnz, long symmetric, long symmetry_expanded)
{
	return cuda_csr_transpose_expand_rows_to_format(row_ptr, col_ind, values, m, n, nnz, symmetric, symmetry_expanded, -1);
}

int
statistics_print_labels(char * buf, long buf_n)
{
	// synchronize();
	return cuda_csr_transpose_expand_rows_statistics_print_labels(buf, buf_n);
}
#endif
