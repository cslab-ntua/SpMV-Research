#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

#include <cuda.h>
#include <cooperative_groups.h>
#include <cuda_pipeline_primitives.h>

#include "macros/cpp_defines.h"

#include "../spmv_kernel.h"


#ifndef BLOCK_SIZE
	// #define BLOCK_SIZE  32
	// #define BLOCK_SIZE  64
	#define BLOCK_SIZE  128
	// #define BLOCK_SIZE  256
	// #define BLOCK_SIZE  512
	// #define BLOCK_SIZE  1024
#endif

#define NNZ_PER_THREAD  5


#define GPU_TIMERS  0
// #define GPU_TIMERS  1


#ifdef __cplusplus
extern "C"{
#endif
	#include "macros/macrolib.h"
	#include "time_it.h"
	#include "parallel_util.h"
	#include "array_metrics.h"
	#include "bit_ops.h"
	#include "bitstream.h"
	#include "hash/hash.h"
	#include "string_util.h"
	#include "plot/plot.h"

	#include "aux/csr_converter.h"
	#include "aux/csr_util.h"

	#include "cuda/cuda_util.h"

	// static inline
	// double
	// idx_to_double(void * A, long i)
	// {
	// 	return (double) ((INT_T *) A)[i];
	// }

	// static inline
	// double
	// row_ptr_to_degree_double(void * A, long i)
	// {
	// 	return (double) (((INT_T *) A)[i+1] - ((INT_T *) A)[i]);
	// }

	static inline
	double
	ull_to_double(void * A, long i)
	{
		return (double) ((unsigned long long *) A)[i];
	}

	static inline
	double
	int_to_double(void * A, long i)
	{
		return (double) ((int *) A)[i];
	}

	#include "functools/functools_gen_push.h"
	#define FUNCTOOLS_GEN_TYPE_1  int
	#define FUNCTOOLS_GEN_TYPE_2  int
	#define FUNCTOOLS_GEN_SUFFIX  _CUDA_SELL_SORTED_HYBRID
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

	#include "sort/bucketsort/bucketsort_gen_undef.h"
	#define BUCKETSORT_GEN_TYPE_1  INT_T
	#define BUCKETSORT_GEN_TYPE_2  INT_T
	#define BUCKETSORT_GEN_TYPE_3  int
	#define BUCKETSORT_GEN_TYPE_4  void
	#define BUCKETSORT_GEN_SUFFIX  _CUDA_SELL_SORTED_HYBRID
	#include "sort/bucketsort/bucketsort_gen.c"
	static inline
	INT_T
	bucketsort_find_bucket(INT_T * A, long i, __attribute__((unused)) void * unused)
	{
		return A[i];
	}


	struct samplesort_pass_2_data_s {
		double com;
		INT_T col_s;
		INT_T col_e;
	};
	#include "sort/samplesort/samplesort_gen_undef.h"
	#define SAMPLESORT_GEN_TYPE_1  INT_T
	#define SAMPLESORT_GEN_TYPE_2  INT_T
	#define SAMPLESORT_GEN_TYPE_3  int
	#define SAMPLESORT_GEN_TYPE_4  samplesort_pass_2_data_s
	#define SAMPLESORT_GEN_SUFFIX  _pass_2
	#include "sort/samplesort/samplesort_gen.c"
	static inline
	int
	samplesort_cmp(INT_T a, INT_T b, struct samplesort_pass_2_data_s * data)
	{
		int ret = 0;
		INT_T col_s_a = data[a].col_s, col_s_b = data[b].col_s;
		INT_T col_e_a = data[a].col_e, col_e_b = data[b].col_e;
		// double coma=data[a].com, comb=data[b].com;
		// ret = coma > comb ? 1 : coma < comb ? -1 : 0;
		ret = col_e_a > col_e_b ? 1 : col_e_a < col_e_b ? -1 : 0;
		if (ret == 0)
			ret = col_s_a > col_s_b ? 1 : col_s_a < col_s_b ? -1 : 0;
		if (ret == 0)
			ret = a > b ? 1 : a < b ? -1 : 0;
		return ret;
	}


	struct samplesort_data_s {
		INT_T degree;
		INT_T col_e;
	};
	#include "sort/samplesort/samplesort_gen_undef.h"
	#define SAMPLESORT_GEN_TYPE_1  INT_T
	#define SAMPLESORT_GEN_TYPE_2  INT_T
	#define SAMPLESORT_GEN_TYPE_3  int
	#define SAMPLESORT_GEN_TYPE_4  struct samplesort_data_s
	#define SAMPLESORT_GEN_SUFFIX  _degree_order
	#include "sort/samplesort/samplesort_gen.c"
	static inline
	int
	samplesort_cmp(INT_T a, INT_T b, struct samplesort_data_s * data)
	{
		int ret = 0;
		if (data == NULL)
		{
			return a > b ? 1 : a < b ? -1 : 0;
		}
		INT_T degree_a = data[a].degree;
		INT_T degree_b = data[b].degree;
		INT_T col_e_a = data[a].col_e;
		INT_T col_e_b = data[b].col_e;
		double ae = fabs(degree_a - degree_b);
		double degree_min = (degree_a < degree_b) ? degree_a : degree_b;
		double relative_dist = ae / degree_min;
		if ( ! ((degree_min >= 4) && (relative_dist < 1.0/4)) )
		{
			ret = (degree_a > degree_b) ? 1 : (degree_a < degree_b) ? -1 : 0;
		}
		if (ret == 0)
			ret = (col_e_a > col_e_b) ? 1 : (col_e_a < col_e_b) ? -1 : 0;
		if (ret == 0)
			ret = (a > b) ? 1 : (a < b) ? -1 : 0;
		return ret;
	}


	#include "sort/quicksort/quicksort_gen_undef.h"
	#define QUICKSORT_GEN_TYPE_1  INT_T
	#define QUICKSORT_GEN_TYPE_2  INT_T
	#define QUICKSORT_GEN_TYPE_3  INT_T
	#define QUICKSORT_GEN_SUFFIX  _CUDA_SELL_SORTED_HYBRID
	#include "sort/quicksort/quicksort_gen.c"
	static inline
	int
	quicksort_cmp(INT_T a, INT_T b, INT_T * data)
	{
		int ret = 0;
		INT_T va=data[a], vb=data[b];
		if (ret == 0) ret = va > vb ? 1 : va < vb ? -1 : 0;
		if (ret == 0) ret = a > b ? 1 : a < b ? -1 : 0;
		return ret;
	}


#ifdef __cplusplus
}
#endif


namespace cg = cooperative_groups;

using namespace cooperative_groups;

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


inline
int
row_is_above_crossover(INT_T degree, long m, __attribute__((unused)) long nnz, int device_max_num_threads)
{
	INT_T crossover_degree = 50;
	int ret = 0;
	if (m < 2 * device_max_num_threads)
		crossover_degree = 10;
	if (degree >= crossover_degree)
		ret = 1;
	return ret;
}


void
sort_sell_warp_columns(INT_T i, INT_T * row_ptr, INT_T * ja, ValueType * a, __attribute__((unused)) long m)
{
	long cache_line_size = 128;
	long cache_line_elements = cache_line_size / sizeof(ValueType);
	long nnz_warp = row_ptr[i+32] - row_ptr[i];
	INT_T * ja_warp = &ja[row_ptr[i]];
	ValueType * a_warp = &a[row_ptr[i]];
	INT_T * idx = (typeof(idx)) malloc(nnz_warp * sizeof(*idx));
	INT_T * rfs = (typeof(rfs)) malloc(nnz_warp * sizeof(*rfs));
	INT_T * ja_buf = (typeof(ja_buf)) malloc(nnz_warp * sizeof(*ja_buf));
	ValueType * a_buf = (typeof(a_buf)) malloc(nnz_warp * sizeof(*a_buf));
	INT_T * ja_row;
	ValueType * a_row;
	INT_T * rfs_row;
	INT_T degree, rf;
	long j, k, l;
	for (j=0;j<nnz_warp;j++)
		idx[j] = j;
	quicksort(idx, nnz_warp, ja_warp, NULL);   // Sort all warp nnz by column index.
	k = 0;
	j = 0;
	while (j<nnz_warp)   // Find replication factors of column indices.
	{
		rf = 1;
		j++;
		for (;j<nnz_warp;j++)
		{
			if (ja_warp[idx[j]] / cache_line_elements != ja_warp[idx[j-1]] / cache_line_elements)
				break;
			rf++;
		}
		for (l=k;l<j;l++)
		{
			rfs[idx[l]] = -rf;   // Negative to sort descending after.
		}
		k = j;
	}
	ja_row = ja_warp;
	a_row = a_warp;
	rfs_row = rfs;
	for (k=i;k<i+32;k++)   // Sort each row by replication factors, descending.
	{
		degree = row_ptr[k+1] - row_ptr[k];
		for (j=0;j<degree;j++)
			idx[j] = j;
		quicksort(idx, degree, rfs_row, NULL);
		for (j=0;j<degree;j++)
		{
			ja_buf[j] = ja_row[idx[j]];
			a_buf[j] = a_row[idx[j]];
		}
		for (j=0;j<degree;j++)
		{
			ja_row[j] = ja_buf[j];
			a_row[j] = a_buf[j];
		}
		ja_row += degree;
		a_row += degree;
		rfs_row += degree;
	}
	free(idx);
	free(rfs);
	free(ja_buf);
	free(a_buf);
}

struct warp_coords_t {
	INT_T coords[2];
};

struct Cuda_SELL_Sorted_Hybrid_Arrays : Matrix_Format
{
	// --- Hybrid metadata ---
	long m_cpu = -1, max_mn = -1; // number of CPU rows (-1 in standalone mode)
	long offset; // this offset is used for hybrid mode to indicate the start of the GPU part
	bool is_first_iteration = true;
	bool is_last_iteration = false;

	char * filename_base;

	long crossover_row;   // A row index in the SORTED matrix, where we change from SELL to CSR. Multiple of BLOCK_SIZE.
	long nnz_per_thread;
	long nnz_per_block;
	long nnz_per_warp;

	long num_row_clusters;
	long nnz_extended;
	long nnz_sell;
	long nnz_csr;

	ValueType * a;

	INT_T * row_ptr_h;
	INT_T * row_cluster_ptr_h;
	INT_T * ja_h;
	ValueType * a_h;
	INT_T * thread_warp_i_s = NULL;
	INT_T * thread_warp_i_e = NULL;
	INT_T * thread_warp_j_s = NULL;
	INT_T * thread_warp_j_e = NULL;
	warp_coords_t * thread_warp_coords = NULL;

	INT_T * row_ptr_d;
	INT_T * row_cluster_ptr_d;
	INT_T * ja_d;
	ValueType * a_d;
	INT_T * thread_warp_i_s_d = NULL;
	INT_T * thread_warp_i_e_d = NULL;
	warp_coords_t * thread_warp_coords_d = NULL;

	ValueType * x = NULL;
	ValueType * y = NULL;
	#ifdef VECTOR_ALLOC_EXPLICIT
		ValueType * x_d = NULL;
		ValueType * y_d = NULL;
	#endif

	int thread_block_size;

	int num_threads;
	int num_threads_sell;
	int num_threads_csr;
	int num_thread_warps;
	int num_thread_warps_sell;
	int num_thread_warps_csr;
	int num_thread_blocks;
	int num_thread_blocks_sell;
	int num_thread_blocks_csr;

	INT_T * row_permutation = NULL;

	unsigned long long * timers;
	unsigned long long * timers_d;

	// --- CUDA streams ---
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

	// --- Timing accumulators (used when DETAILED_TIMING=1) ---
	double time_h2d_ms = 0;
	double time_memset_ms = 0;
	double time_kernel_ms = 0;
	double time_d2h_ms = 0;
	double time_pure_memset_ms = 0;
	long call_count = 0;

	Cuda_SELL_Sorted_Hybrid_Arrays(INT_T * row_ptr, INT_T * ja, ValueTypeReference * a_ref, long m, long n, long nnz, long m_cpu = -1) : Matrix_Format(m, n, nnz), m_cpu(m_cpu)
	{
		long num_threads_cpu = omp_get_max_threads();
		__attribute__((unused)) long enable_legend = 1;
		__attribute__((unused)) long num_pixels_x = 1080;
		__attribute__((unused)) long num_pixels_y = 1080;
		double time;
		long i;

		// const long cache_line_size = 128;
		// const long cache_line_size = 64;

		long buf_n = 1000;
		char buf[buf_n];
		char * file_in = getenv("MATRIX_NAME");
		__attribute__((cleanup(cleanup_free))) char * path=NULL, * filename=NULL;
		str_path_split_path(file_in, strlen(file_in) + 1, buf, buf_n, &path, &filename);
		path = strdup(path);
		filename = strdup(filename);
		str_path_split_ext(filename, strlen(filename) + 1, buf, buf_n, &filename_base, NULL);
		filename_base = strdup(filename_base);

		time = time_it(1,
			cudaFree(0);
		);
		printf("time cuda context creation = %g\n", time);

		int device_multiproc_count, device_max_threads_per_multiproc, device_max_num_threads;
		time = time_it(1,
			cuda_device_print_attributes();

			cuda_assert(cudaDeviceGetAttribute(&device_multiproc_count, cudaDevAttrMultiProcessorCount, 0));
			cuda_assert(cudaDeviceGetAttribute(&device_max_threads_per_multiproc, cudaDevAttrMaxThreadsPerMultiProcessor, 0));
			device_max_num_threads = device_max_threads_per_multiproc * device_multiproc_count;
		);
		printf("time cudaDeviceGetAttribute = %g\n", time);

		// Convert values from ValueTypeReference (double) to ValueType (e.g., float).
		a = (typeof(a)) malloc(nnz * sizeof(*a));
		#pragma omp parallel for
		for (long i = 0; i < nnz; i++)
			a[i] = (ValueType) a_ref[i];

		// Hybrid mode: GPU handles rows [m_cpu, m_cpu+m); offset into the shared y vector.
		// Standalone mode: GPU handles all rows; offset = 0.
		offset = (m_cpu != -1) ? m_cpu : 0;
		long total_m = (m_cpu != -1) ? (m + m_cpu) : m;
		max_mn = (total_m > n) ? total_m : n;

		cuda_assert(cudaStreamCreate(&stream));
		#ifdef VECTOR_ALLOC_EXPLICIT
			cuda_assert(cudaStreamCreate(&memset_stream));
			cuda_assert(cudaStreamCreate(&h2d_stream));
		#endif
		cuda_assert(cudaEventCreate(&start_event));
		cuda_assert(cudaEventCreate(&stop_event));
		cuda_assert(cudaEventCreate(&memset_event));
		#ifdef VECTOR_ALLOC_EXPLICIT
			cuda_assert(cudaEventCreate(&h2d_event));
			cuda_assert(cudaEventCreate(&kernel_event));
			cuda_assert(cudaEventCreate(&d2h_event));
			cuda_assert(cudaEventCreate(&memset_done_event));
			cuda_assert(cudaEventCreate(&h2d_done_event));
			cuda_assert(cudaEventCreate(&pure_memset_start));
			cuda_assert(cudaEventCreate(&pure_memset_stop));
			cuda_assert(cudaEventRecord(memset_done_event, memset_stream));
			cuda_assert(cudaEventRecord(h2d_done_event, h2d_stream));
		#endif

		nnz_per_thread = NNZ_PER_THREAD;
		nnz_per_block = nnz_per_thread * BLOCK_SIZE;
		nnz_per_warp = nnz_per_thread * 32;

		thread_block_size = BLOCK_SIZE;
		if (thread_block_size % 32)
			error("select a thread block size that is a multiple of 32");

		/* Find crossover row.
		 * It should be a multiple of BLOCK_SIZE to simplify the SELL code. */
		crossover_row = 0;
		_Pragma("omp parallel")
		{
			INT_T degree;
			long num_rows_below = 0;
			long i;
			_Pragma("omp for")
			for (i=0;i<m;i++)
			{
				degree = row_ptr[i+1] - row_ptr[i];
				if (!row_is_above_crossover(degree, m, nnz, device_max_num_threads))
					num_rows_below++;
			}
			__atomic_fetch_add(&crossover_row, num_rows_below, __ATOMIC_RELAXED);
		}
		crossover_row = crossover_row - crossover_row % BLOCK_SIZE;
		printf("m=%ld, crossover_row=%ld, csr_rows=%ld\n", m, crossover_row, m - crossover_row);

		/* Sort rows. */
		time = time_it(1,
			long fig_name_base_n = 1000;
			char fig_name_base[fig_name_base_n];

			snprintf(fig_name_base, fig_name_base_n, "figures/%s", filename_base);
			// csr_plot(fig_name_base, row_ptr, ja, a, m, n, nnz, enable_legend, num_pixels_x, num_pixels_y);

			INT_T * reverse_row_permutation = (typeof(reverse_row_permutation)) malloc(m * sizeof(*reverse_row_permutation));
			row_permutation = (typeof(row_permutation)) malloc(m * sizeof(*row_permutation));

			/* Sort by row size. */
			struct samplesort_data_s * cmp_data = (typeof(cmp_data)) malloc(m * sizeof(*cmp_data));
			_Pragma("omp parallel")
			{
				long i;
				_Pragma("omp for")
				for (i=0;i<m;i++)
				{
					reverse_row_permutation[i] = i;
					row_permutation[i] = i;
					cmp_data[i].degree = row_ptr[i+1] - row_ptr[i];
					cmp_data[i].col_e = ja[row_ptr[i+1] - 1];
				}
			}
			samplesort_degree_order(reverse_row_permutation, m, cmp_data);
			free(cmp_data);

			/* Restore order in CSR part, sort by row index. */
			samplesort_degree_order(&reverse_row_permutation[crossover_row], m-crossover_row, NULL);
			// bucketsort_recalculate_bucket(&reverse_row_permutation[crossover_row], m-crossover_row, m, NULL, _TYPE_I * restrict permutation_out, NULL);

			_Pragma("omp parallel")
			{
				long i;
				_Pragma("omp for")
				for (i=0;i<m;i++)
					row_permutation[reverse_row_permutation[i]] = i;
			}

			row_ptr_h = (typeof(row_ptr_h)) malloc((m+1) * sizeof(*row_ptr_h));
			ja_h = (typeof(ja_h)) malloc(nnz * sizeof(*ja_h));
			a_h = (typeof(a_h)) malloc(nnz * sizeof(*a_h));
			csr_reorder_rows(row_permutation, row_ptr, ja, a, m, n, nnz, row_ptr_h, ja_h, a_h);

			/* Sort SELL warps by right-most column index. */
			struct samplesort_pass_2_data_s * cmp_pass_2_data;
			cmp_pass_2_data = (typeof(cmp_pass_2_data)) malloc(m * sizeof(*cmp_pass_2_data));
			INT_T * row_permutation_2 = (typeof(row_permutation_2)) malloc(m * sizeof(*row_permutation_2));
			INT_T * reverse_row_permutation_2 = (typeof(reverse_row_permutation_2)) malloc(m * sizeof(*reverse_row_permutation_2));
			_Pragma("omp parallel")
			{
				long i, j, k;
				INT_T col_s, col_e;
				_Pragma("omp for")
				for (i=0;i<m;i++)
					reverse_row_permutation_2[i] = i;
				_Pragma("omp for")
				for (i=0;i<crossover_row;i+=32)
				{
					col_s = n;
					col_e = 0;
					for (k=i;k<i+32;k++)
					{
						for (j=row_ptr_h[k];j<row_ptr_h[k+1];j++)
						{
							if (ja_h[j] > col_e)
								col_e = ja_h[j];
							if (ja_h[j] < col_s)
								col_s = ja_h[j];
						}
					}
					for (k=i;k<i+32;k++)
					{
						cmp_pass_2_data[k].col_s = col_s;
						cmp_pass_2_data[k].col_e = col_e;
					}
				}
				_Pragma("omp for")
				for (i=crossover_row;i<m;i++)
				{
					col_s = n;
					col_e = 0;
					for (j=row_ptr_h[i];j<row_ptr_h[i+1];j++)
					{
						if (ja_h[j] > col_e)
							col_e = ja_h[j];
						if (ja_h[j] < col_s)
							col_s = ja_h[j];
					}
					cmp_pass_2_data[i].col_s = n + 1 + col_s;
					cmp_pass_2_data[i].col_e = n + 1 + col_e;
				}
				// _Pragma("omp for")
				// for (i=crossover_row;i<m;i+=32)
				// {
					// col_s = n;
					// col_e = 0;
					// long k_s = i, k_e = i + 32;
					// if (k_e > m)
						// k_e = m;
					// for (k=k_s;k<k_e;k++)
					// {
						// for (j=row_ptr_h[k];j<row_ptr_h[k+1];j++)
						// {
							// if (ja_h[j] > col_e)
								// col_e = ja_h[j];
							// if (ja_h[j] < col_s)
								// col_s = ja_h[j];
						// }
					// }
					// for (k=k_s;k<k_e;k++)
					// {
						// cmp_pass_2_data[k].col_s = n + 1 + col_s;
						// cmp_pass_2_data[k].col_e = n + 1 + col_e;
					// }
				// }
			}
			// samplesort_pass_2(reverse_row_permutation_2, m, cmp_pass_2_data);
			samplesort_pass_2(reverse_row_permutation_2, crossover_row, cmp_pass_2_data);
			_Pragma("omp parallel")
			{
				long i;
				_Pragma("omp for")
				for (i=0;i<m;i++)
					row_permutation_2[reverse_row_permutation_2[i]] = i;
			}
			INT_T * row_ptr_buf = row_ptr_h;
			INT_T * ja_buf = ja_h;
			ValueType * a_buf = a_h;
			INT_T * reverse_row_permutation_buf = reverse_row_permutation;
			row_ptr_h = (typeof(row_ptr_h)) malloc((m+1) * sizeof(*row_ptr_h));
			ja_h = (typeof(ja_h)) malloc(nnz * sizeof(*ja_h));
			a_h = (typeof(a_h)) malloc(nnz * sizeof(*a_h));
			reverse_row_permutation = (typeof(reverse_row_permutation)) malloc(m * sizeof(*reverse_row_permutation));
			csr_reorder_rows(row_permutation_2, row_ptr_buf, ja_buf, a_buf, m, n, nnz, row_ptr_h, ja_h, a_h);
			_Pragma("omp parallel")
			{
				long i;
				_Pragma("omp for")
				for (i=0;i<m;i++)
					reverse_row_permutation[row_permutation_2[i]] = reverse_row_permutation_buf[i];
			}
			_Pragma("omp parallel")
			{
				long i;
				_Pragma("omp for")
				for (i=0;i<m;i++)
					row_permutation[reverse_row_permutation[i]] = i;
			}
			free(row_ptr_buf);
			free(ja_buf);
			free(a_buf);
			free(cmp_pass_2_data);
			free(row_permutation_2);
			free(reverse_row_permutation_2);
			free(reverse_row_permutation_buf);

			/* // Try column reordering.
			if (n == m)
			{
				_Pragma("omp parallel")
				{
					long j;
					_Pragma("omp for")
					for (j=0;j<nnz;j++)
					{
						ja_h[j] = row_permutation[ja_h[j]];
					}
				}
				csr_sort_columns(row_ptr_h, ja_h, a_h, m, n, nnz);
			} */

			free(reverse_row_permutation);

			snprintf(fig_name_base, fig_name_base_n, "figures/%s_reordered", filename_base);
			// csr_plot(fig_name_base, row_ptr_h, ja_h, a_h, m, n, nnz, enable_legend, num_pixels_x, num_pixels_y);
		);
		printf("time sort rows = %g\n", time);


		/* Extend SELL row clusters to local max row. 
		 * Transpose row clusters.
		 * Extend CSR rows to multiples of 'nnz_per_thread'.
		 * Extend last row so that CSR has multiple of BLOCK_SIZE number of threads,
		 * i.e., CSR has multiple of 'nnz_per_thread * BLOCK_SIZE' nonzeros.
		 */
		time = time_it(1,
			INT_T * row_ptr_h_new = (typeof(row_ptr_h_new)) malloc((m+1) * sizeof(*row_ptr_h_new));
			INT_T * ja_h_new;
			ValueType * a_h_new;
			_Pragma("omp parallel")
			{
				long i, j, j1, j2, j_s, j_e, k;
				long degree;
				long degree_cluster_max;
				_Pragma("omp for")
				for (i=0;i<crossover_row;i+=32)
				{
					degree_cluster_max = 0;
					for (k=i;k<i+32;k++)
					{
						degree = row_ptr_h[k+1] - row_ptr_h[k];
						if (degree > degree_cluster_max)
							degree_cluster_max = degree;
					}
					for (k=i;k<i+32;k++)
						row_ptr_h_new[k] = degree_cluster_max;
				}
				_Pragma("omp for")
				for (i=crossover_row;i<m;i++)
				{
					degree = row_ptr_h[i+1] - row_ptr_h[i];
					degree = nnz_per_thread * ((degree + nnz_per_thread - 1) / nnz_per_thread);
					row_ptr_h_new[i] = degree;
				}
				_Pragma("omp single")
				{
					row_ptr_h_new[m] = 0;
				}
				scan_reduce_concurrent(row_ptr_h_new, row_ptr_h_new, m+1, 0, 1, 0);
				_Pragma("omp single")
				{
					nnz_extended = row_ptr_h_new[m];
					nnz_sell = row_ptr_h_new[crossover_row];
					nnz_csr = nnz_extended - nnz_sell;
					nnz_csr = nnz_per_block * ((nnz_csr + nnz_per_block - 1) / nnz_per_block);
					nnz_extended = nnz_sell + nnz_csr;
					row_ptr_h_new[m] = nnz_extended;
					ja_h_new = (typeof(ja_h_new)) malloc(nnz_extended * sizeof(*ja_h_new));
					a_h_new = (typeof(a_h_new)) malloc(nnz_extended * sizeof(*a_h_new));
				}
				_Pragma("omp for")
				for (i=0;i<crossover_row;i+=32)
				{
					for (k=i;k<i+32;k++)
					{
						for (j1=row_ptr_h[k],j2=row_ptr_h_new[k];j1<row_ptr_h[k+1];j1++,j2++)
						{
							ja_h_new[j2] = ja_h[j1];
							a_h_new[j2] = a_h[j1];
						}
						for (;j2<row_ptr_h_new[k+1];j2++)
						{
							ja_h_new[j2] = ja_h[row_ptr_h[k+1] - 1];
							a_h_new[j2] = 0;
						}
					}
					degree = row_ptr_h_new[i+1] - row_ptr_h_new[i];
					/* if (i == (1500000ULL & (~31ULL)))
					{
						for (k=i;k<i+32;k++)
						{
							for (j=row_ptr_h_new[k];j<row_ptr_h_new[k+1];j++)
							{
								// printf("%8d ", ja_h_new[j] / 128);
								printf("%8d ", ja_h_new[j]);
							}
							printf("\n");
						}
						printf("\n");
					} */

					// sort_sell_warp_columns(i, row_ptr_h_new, ja_h_new, a_h_new, m);

					transpose(&ja_h_new[row_ptr_h_new[i]], 32, degree);
					transpose(&a_h_new[row_ptr_h_new[i]], 32, degree);
				}
				_Pragma("omp for")
				for (i=crossover_row;i<m;i++)
				{
					for (j1=row_ptr_h[i],j2=row_ptr_h_new[i];j1<row_ptr_h[i+1];j1++,j2++)
					{
						ja_h_new[j2] = ja_h[j1];
						a_h_new[j2] = a_h[j1];
					}
					for (;j2<row_ptr_h_new[i+1];j2++)
					{
						ja_h_new[j2] = ja_h[row_ptr_h[i+1] - 1];
						a_h_new[j2] = 0;
					}
					ja_h_new[row_ptr_h_new[i]] |= 0x80000000;

					j_s = row_ptr_h_new[i];
					while (j_s < row_ptr_h_new[i+1])   // Interleave thread nnz of same row (round-robin).
					{
						j_e = j_s + nnz_per_warp;
						j_e = j_e - ((j_e - nnz_sell) % nnz_per_warp);
						if (j_e > row_ptr_h_new[i+1])
							j_e = row_ptr_h_new[i+1];
						long local_num_threads = (j_e - j_s) / nnz_per_thread;
						transpose(&a_h_new[j_s], nnz_per_thread, local_num_threads);
						transpose(&ja_h_new[j_s], nnz_per_thread, local_num_threads);
						j_s = j_e;
					}
				}
				_Pragma("omp for")
				for (j=row_ptr_h_new[crossover_row];j<nnz_extended;j+=32*nnz_per_thread)
				{
					transpose(&a_h_new[j], 32, nnz_per_thread);
					transpose(&ja_h_new[j], 32, nnz_per_thread);
				}
			}
			free(row_ptr_h);
			free(ja_h);
			free(a_h);
			row_ptr_h = row_ptr_h_new;
			ja_h = ja_h_new;
			a_h = a_h_new;
		);
		printf("time extend and transpose = %g\n", time);
		// printf("nnz=%ld, nnz_extended=%ld, nnz_sell=%ld, nnz_csr=%ld\n", nnz, nnz_extended, nnz_sell, nnz_csr);
		printf("m=%ld, nnz=%ld | rows_sell=%ld (%.2f%%), rows_csr=%ld (%.2f%%) | nnz_extended=%ld (padding: %.2f%%), nnz_sell=%ld (%.2f%%), nnz_csr=%ld (%.2f%%)\n",
				  m, nnz, crossover_row, (double) crossover_row / m * 100, m - crossover_row, (double) (m - crossover_row) / m * 100, 
				  nnz_extended, (double)(nnz_extended - nnz) / nnz * 100, nnz_sell, (double) nnz_sell / nnz_extended * 100, nnz_csr, (double) nnz_csr / nnz_extended * 100);

		/* Find number of threads for each format. */
		num_threads = crossover_row + nnz_csr / nnz_per_thread;
		num_threads_sell = crossover_row;
		num_threads_csr = num_threads - num_threads_sell;
		num_thread_blocks = num_threads / BLOCK_SIZE;
		num_thread_blocks_sell = num_threads_sell / BLOCK_SIZE;
		num_thread_blocks_csr = num_threads_csr / BLOCK_SIZE;
		num_thread_warps = num_threads / 32;
		num_thread_warps_sell = num_threads_sell / 32;
		num_thread_warps_csr = num_threads_csr / 32;
		printf("num_threads=%d, thread_block_size=%d, num_thread_blocks=%d\n", num_threads, BLOCK_SIZE, num_thread_blocks);

		/* Find SELL row clusters offsets. */
		num_row_clusters = crossover_row / 32;
		row_cluster_ptr_h = (typeof(row_cluster_ptr_h)) malloc((num_row_clusters+1) * sizeof(*row_cluster_ptr_h));
		_Pragma("omp parallel")
		{
			_Pragma("omp for")
			for (i=0;i<crossover_row;i+=32)
				row_cluster_ptr_h[i/32] = row_ptr_h[i];
		}
		row_cluster_ptr_h[num_row_clusters] = row_ptr_h[crossover_row];

		/* Find CSR warps row boundaries. */
		thread_warp_i_s = (INT_T *) malloc(num_thread_warps_csr * sizeof(*thread_warp_i_s));
		thread_warp_i_e = (INT_T *) malloc(num_thread_warps_csr * sizeof(*thread_warp_i_e));
		thread_warp_j_s = (INT_T *) malloc(num_thread_warps_csr * sizeof(*thread_warp_j_s));
		thread_warp_j_e = (INT_T *) malloc(num_thread_warps_csr * sizeof(*thread_warp_j_e));
		thread_warp_coords = (warp_coords_t *) malloc((num_thread_warps_csr+1) * sizeof(*thread_warp_coords));
		time = time_it(1,
			_Pragma("omp parallel")
			{
				long lower_boundary, higher_boundary;
				_Pragma("omp for")
				for (i=0;i<num_thread_warps_csr;i++)
				{
					thread_warp_j_s[i] = nnz_sell + nnz_per_warp * i;

					if (thread_warp_j_s[i] > nnz_extended)
						thread_warp_j_s[i] = nnz_extended;
					macros_binary_search(row_ptr_h, 0, m, thread_warp_j_s[i], &lower_boundary, NULL);           // Index boundaries are inclusive.
					while (row_ptr_h[lower_boundary] == row_ptr_h[lower_boundary+1])
						lower_boundary++;
					thread_warp_i_s[i] = lower_boundary;
					thread_warp_j_e[i] = thread_warp_j_s[i] + nnz_per_warp;
					if (thread_warp_j_e[i] > nnz_extended)
						thread_warp_j_e[i] = nnz_extended;
					macros_binary_search(row_ptr_h, 0, m, thread_warp_j_e[i], NULL, &higher_boundary);           // Index boundaries are inclusive.
					while (row_ptr_h[higher_boundary] == row_ptr_h[higher_boundary+1])
						higher_boundary--;
					thread_warp_i_e[i] = higher_boundary;
					thread_warp_coords[i] = { thread_warp_i_s[i], thread_warp_j_s[i] };
				}
			}
			thread_warp_coords[num_thread_warps_csr] = { 0, (INT_T) nnz_extended };
		);
		printf("time find warp boundaries = %g\n", time);

		time = time_it(1,
			cuda_assert(cudaMalloc(&row_ptr_d, (m+1) * sizeof(*row_ptr_d)));
			cuda_assert(cudaMalloc(&row_cluster_ptr_d, (num_row_clusters+1) * sizeof(*row_cluster_ptr_d)));
			cuda_assert(cudaMalloc(&ja_d, nnz_extended * sizeof(*ja_d)));
			cuda_assert(cudaMalloc(&a_d, nnz_extended * sizeof(*a_d)));
			cuda_assert(cudaMalloc(&thread_warp_i_s_d, num_thread_warps_csr * sizeof(*thread_warp_i_s_d)));
			cuda_assert(cudaMalloc(&thread_warp_i_e_d, num_thread_warps_csr * sizeof(*thread_warp_i_e_d)));
			cuda_assert(cudaMalloc(&thread_warp_coords_d, (num_thread_warps_csr+1) * sizeof(*thread_warp_coords_d)));
			
			#ifdef VECTOR_ALLOC_EXPLICIT
				cuda_assert(cudaMalloc(&x_d, max_mn * sizeof(*x_d)));
				cuda_assert(cudaMalloc(&y_d, max_mn * sizeof(*y_d)));
			#endif

			cudaEvent_t setup_h2d_start, setup_h2d_stop;
			cuda_assert(cudaEventCreate(&setup_h2d_start));
			cuda_assert(cudaEventCreate(&setup_h2d_stop));
			cuda_assert(cudaEventRecord(setup_h2d_start, stream));

			cuda_assert(cudaMemcpyAsync(row_ptr_d, row_ptr_h, (m+1) * sizeof(*row_ptr_d), cudaMemcpyHostToDevice, stream));
			cuda_assert(cudaMemcpyAsync(row_cluster_ptr_d, row_cluster_ptr_h, (num_row_clusters+1) * sizeof(*row_cluster_ptr_d), cudaMemcpyHostToDevice, stream));
			cuda_assert(cudaMemcpyAsync(ja_d, ja_h, nnz_extended * sizeof(*ja_d), cudaMemcpyHostToDevice, stream));
			cuda_assert(cudaMemcpyAsync(a_d, a_h, nnz_extended * sizeof(*a_d), cudaMemcpyHostToDevice, stream));
			cuda_assert(cudaMemcpyAsync(thread_warp_i_s_d, thread_warp_i_s, num_thread_warps_csr * sizeof(*thread_warp_i_s_d), cudaMemcpyHostToDevice, stream));
			cuda_assert(cudaMemcpyAsync(thread_warp_i_e_d, thread_warp_i_e, num_thread_warps_csr * sizeof(*thread_warp_i_e_d), cudaMemcpyHostToDevice, stream));
			cuda_assert(cudaMemcpyAsync(thread_warp_coords_d, thread_warp_coords, (num_thread_warps_csr+1) * sizeof(*thread_warp_coords_d), cudaMemcpyHostToDevice, stream));
			
			cuda_assert(cudaEventRecord(setup_h2d_stop, stream));
			cuda_assert(cudaEventSynchronize(setup_h2d_stop));
			float setup_h2d_ms = 0;
			cuda_assert(cudaEventElapsedTime(&setup_h2d_ms, setup_h2d_start, setup_h2d_stop));

			#if DETAILED_TIMING
				double total_bytes = ((m+1) * sizeof(*row_ptr_d)) + ((num_row_clusters+1) * sizeof(*row_cluster_ptr_d)) + 
									(nnz_extended * sizeof(*ja_d)) + (nnz_extended * sizeof(*a_d)) + 
									(num_thread_warps_csr * sizeof(*thread_warp_i_s_d)) + (num_thread_warps_csr * sizeof(*thread_warp_i_e_d)) +
									((num_thread_warps_csr+1) * sizeof(*thread_warp_coords_d));
				double bw_GBs = (setup_h2d_ms > 0) ? (total_bytes / 1e6) / setup_h2d_ms : 0;
				printf("Sparse Matrix H2D Transfer: %.4lf ms (%.2lf MB - %.2lf GB/s)\n", setup_h2d_ms, total_bytes / 1e6, bw_GBs);
			#endif

			cuda_assert(cudaEventDestroy(setup_h2d_start));
			cuda_assert(cudaEventDestroy(setup_h2d_stop));

		);
		printf("time cudaMemcpy = %g\n", time);

		#if GPU_TIMERS
			timers = (typeof(timers)) malloc(num_thread_warps * sizeof(*timers));
			cuda_assert(cudaMalloc(&timers_d, num_thread_warps * sizeof(*timers_d)));
			cuda_assert(cudaMemset(timers_d, UINT_MAX, num_thread_warps * sizeof(*timers_d)));   // UINT_MAX because it takes an integer type for the BYTE values.
		#endif

	}

	~Cuda_SELL_Sorted_Hybrid_Arrays()
	{
		cuda_assert(cudaFree(row_ptr_d));
		cuda_assert(cudaFree(row_cluster_ptr_d));
		cuda_assert(cudaFree(ja_d));
		cuda_assert(cudaFree(a_d));
		cuda_assert(cudaFree(thread_warp_i_s_d));
		cuda_assert(cudaFree(thread_warp_i_e_d));
		cuda_assert(cudaFree(thread_warp_coords_d));
			
		#ifdef VECTOR_ALLOC_EXPLICIT
			cuda_assert(cudaFree(x_d));
			cuda_assert(cudaFree(y_d));
		#endif

		cuda_assert(cudaFreeHost(row_ptr_h));
		cuda_assert(cudaFree(row_cluster_ptr_h));
		cuda_assert(cudaFreeHost(ja_h));
		cuda_assert(cudaFreeHost(a_h));

		cuda_assert(cudaStreamDestroy(stream));
		#ifdef VECTOR_ALLOC_EXPLICIT
			cuda_assert(cudaStreamDestroy(memset_stream));
			cuda_assert(cudaStreamDestroy(h2d_stream));
		#endif
		cuda_assert(cudaEventDestroy(start_event));
		cuda_assert(cudaEventDestroy(stop_event));
		cuda_assert(cudaEventDestroy(memset_event));
		#ifdef VECTOR_ALLOC_EXPLICIT
			cuda_assert(cudaEventDestroy(h2d_event));
			cuda_assert(cudaEventDestroy(kernel_event));
			cuda_assert(cudaEventDestroy(d2h_event));
			cuda_assert(cudaEventDestroy(memset_done_event));
			cuda_assert(cudaEventDestroy(h2d_done_event));
			cuda_assert(cudaEventDestroy(pure_memset_start));
			cuda_assert(cudaEventDestroy(pure_memset_stop));
		#endif
	}
	void spmv(ValueType * x, ValueType * y) override;
	void statistics_start() override;
	int statistics_print_data(char * buf, long buf_n) override;

	void synchronize() override { 
		cuda_assert(cudaStreamSynchronize(stream));
		#ifdef VECTOR_ALLOC_EXPLICIT
			cuda_assert(cudaStreamSynchronize(h2d_stream));
		#endif
		cuda_assert(cudaEventElapsedTime(&last_duration_ms, start_event, stop_event));
		
		#if DETAILED_TIMING
			#ifdef VECTOR_ALLOC_EXPLICIT
				cuda_assert(cudaStreamSynchronize(memset_stream));
				float h2d_ms = 0, memset_ms = 0, kernel_ms = 0, d2h_ms = 0, pure_memset = 0;
				if (last_duration_ms > 0) {
					cuda_assert(cudaEventElapsedTime(&h2d_ms, start_event, h2d_event));
					cuda_assert(cudaEventElapsedTime(&memset_ms, h2d_event, memset_event));
					cuda_assert(cudaEventElapsedTime(&kernel_ms, memset_event, kernel_event));
					cuda_assert(cudaEventElapsedTime(&d2h_ms, kernel_event, d2h_event));
					cuda_assert(cudaEventElapsedTime(&pure_memset, pure_memset_start, pure_memset_stop));
				}
				time_h2d_ms += h2d_ms;
				time_memset_ms += memset_ms;
				time_kernel_ms += kernel_ms;
				time_d2h_ms += d2h_ms;
				time_pure_memset_ms += pure_memset;
			#else
				float memset_ms = 0, kernel_ms = 0;
				if (last_duration_ms > 0) {
					cuda_assert(cudaEventElapsedTime(&memset_ms, start_event, memset_event));
					cuda_assert(cudaEventElapsedTime(&kernel_ms, memset_event, stop_event));
				}
				time_memset_ms += memset_ms;
				time_kernel_ms += kernel_ms;
			#endif
			call_count++;
		#endif
	}

	double get_last_duration() override { return (double)last_duration_ms; }
	void set_last_iteration(bool is_last) override { is_last_iteration = is_last;}

	void issue_h2d_for_next_iteration(ValueType * y) override {
		#ifdef VECTOR_ALLOC_EXPLICIT
			if (m_cpu == -1) return;
			
			#if HYBRID_ITERATIVE_OPTIMIZATION
				long h2d_copy_elements = (m_cpu < n) ? m_cpu : n;
				if (h2d_copy_elements > 0) {
					// Precaution: guarantee we don't overwrite x_d before current iteration's kernel finishes evaluating it!
					cuda_assert(cudaStreamWaitEvent(h2d_stream, kernel_event, 0));
					
					// Pipeline the async Host array push mapping directly back to Device Vector x_d
					cuda_assert(cudaMemcpyAsync(x_d, y, h2d_copy_elements * sizeof(*x_d), cudaMemcpyHostToDevice, h2d_stream));
					
					// Record the proactive dispatch
					cuda_assert(cudaEventRecord(h2d_done_event, h2d_stream));
				}
			#endif
		#endif
	}
};


void compute_sell_sorted(Cuda_SELL_Sorted_Hybrid_Arrays * restrict csr, ValueType * restrict x , ValueType * restrict y);

void
Cuda_SELL_Sorted_Hybrid_Arrays::spmv(ValueType * x, ValueType * y)
{
	compute_sell_sorted(this, x, y);
}


struct Matrix_Format *
cuda_sell_sorted_hybrid_to_format(INT_T * row_ptr, INT_T * col_ind, ValueTypeReference * values, long m, long n, long nnz, long symmetric, long symmetry_expanded, long m_cpu)
{
	if (symmetric && !symmetry_expanded)
		error("symmetric matrices not supported by this format, extend symmetry");
	struct Cuda_SELL_Sorted_Hybrid_Arrays * csr = new Cuda_SELL_Sorted_Hybrid_Arrays(row_ptr, col_ind, values, m, n, nnz, m_cpu);
	// for (long i=0;i<10;i++)
		// printf("%d\n", row_ptr[i]);
	csr->mem_footprint = csr->nnz_extended * (sizeof(ValueType) + sizeof(INT_T)) + (csr->m+1) * sizeof(INT_T);
	char *format_name;
	format_name = (char *)malloc(100*sizeof(char));
	snprintf(format_name, 100, "Custom_CUDA_sell_sorted_hybrid_b%d", BLOCK_SIZE);
	csr->format_name = format_name;
	return csr;
}


//==========================================================================================================================================
//= SELL Custom
//==========================================================================================================================================


template<typename T, typename group_t>
static __attribute__((always_inline)) inline
__device__
T
warp_scan_reduce_inclusive(group_t g, T value)
{
	int lane = g.thread_rank();
	T prev;
	int i;
	#pragma unroll
	for (i=1;i<32;i*=2)
	{
		prev = g.shfl_up(value, i);
		if (lane >= i)
			value += prev;
	}
	return value;
}


template<typename T, typename group_t>
static __attribute__((always_inline)) inline
__device__
T
warp_scan_reduce_exclusive(group_t g, T value)
{
	return warp_scan_reduce_inclusive(g, value) - value;
}


template <typename T, typename group_t>
inline
__device__
void
reduce_warp(group_t g, INT_T row, T val, T * restrict y)
{
	const int tidw = g.thread_rank();   // Group lane.
	int mask_same_row = g.match_any(row);
	int k;
	#pragma unroll
	for (k=g.size()/2; k>=1; k/=2)
	{
		int tidl_next = tidw + k;
		T val_next = g.shfl(val, tidl_next);
		if ((tidl_next < g.size()) && (mask_same_row & (1 << tidl_next)))
		{
			val += val_next;
		}
	}
	if (tidw == __ffs(mask_same_row) - 1)  // __ffs enumeration is 1-based.
		atomicAdd(&y[row], val);
}


template <typename T, typename group_t>
inline
__device__
T
reduce_warp_single_row(group_t g, T val)
{
	// Use XOR mode to perform butterfly reduction
	for (int i=g.size()/2; i>=1; i/=2)
	{
		val += g.shfl_xor(val, i); // __shfl_xor_sync(0xFFFFFFFF, val, i, g.size());   // Total sum is same on all threads.
		// val += __shfl_down_sync(0xFFFFFFFF, val, i, g.size());   // Only thread 0 has the total sum.
	}
	return val;
}


template <const int nnz_per_thread>
__device__
void
spmv_csr(const int tid, INT_T crossover_row, INT_T crossover_offset, INT_T * thread_warp_i_s, warp_coords_t * thread_warp_coords, INT_T * ja, ValueType * a, INT_T m, INT_T n, INT_T nnz, ValueType * restrict x, ValueType * restrict y)
{
	const int tid_csr = tid - crossover_row;
	extern __shared__ char sm[];
	thread_block_tile<32> g = tiled_partition<32>(this_thread_block());
	const int tidw = g.thread_rank();
	const int wid_csr = tid_csr / g.size();
	double sum;
	INT_T new_row;
	// int single_row;
	__attribute__((unused)) INT_T i, i_w_s, i_e, j, jj, jj_s, j_w_s, k, col;
	warp_coords_t coords = thread_warp_coords[wid_csr];
	j_w_s = coords.coords[1];
	// j_w_s = crossover_offset + wid_csr * g.size() * nnz_per_thread;
	jj_s = j_w_s + tidw;

	i_w_s = coords.coords[0];
	// i_w_s = thread_warp_i_s[wid_csr];

	/* The CSR segment has non-empty rows due to sorting!
	 * Only the first element of each thread can be on a row start.
	 * Ignore new row for first thread in warp to have a correct scan reduce.
	 */
	col = ja[jj_s];
	new_row = ((tidw > 0) && (col & 0x80000000)) ? 1 : 0;
	i = warp_scan_reduce_inclusive(g, new_row) + i_w_s;
	// if (wid_csr == 0)
		// printf("%2d: i=%d\n", tid_csr, i);

	sum = ((ValueType) a[jj_s]) * x[col & 0x7FFFFFFF];
	for (k=1,jj=jj_s+g.size();k<nnz_per_thread;k++,jj+=g.size())
	{
		col = ja[jj];
		sum = __fma_rn((ValueType) a[jj], x[col], sum);
	}

	/* g.match_all(i, single_row);   // 'single_row' is passed as reference!!! Passing as pointer gives compilation error.
	if (single_row)
	{
		sum = reduce_warp_single_row(g, sum);
		if (tidw == 0)
			atomicAdd(&y[i], sum);
		// invoke_one(g, [y, i, sum]() {
			// atomicAdd(&y[i], sum);
		// });
	}
	else */
	{
		reduce_warp(g, i, sum, y);
	}
}


__device__
void
spmv_sell(const int tid, INT_T * row_cluster_ptr, INT_T * ja, ValueType * a, INT_T m, INT_T n, INT_T nnz, ValueType * restrict x, ValueType * restrict y)
{
	extern __shared__ char sm[];
	thread_block_tile<32> g = tiled_partition<32>(this_thread_block());
	const int tidw = g.thread_rank();
	const int wid = tid / g.size();
	double sum;
	__attribute__((unused)) INT_T i, j, jj, j_s, j_e, k;
	i = tid;
	j_s = row_cluster_ptr[wid] + tidw;
	j_e = row_cluster_ptr[wid+1];
	// const int bs = 8;
	// INT_T nnz_t = j_e - j_s;
	// INT_T j_mul = j_e - nnz_t % (g.size()*bs);
	sum = 0;
	// double sums[bs] = {0};
	// double vals[bs], x_buf[bs];
	// INT_T cols[bs];

	/* for (j=j_s;j<j_mul;j+=g.size()*bs)
	{
		#pragma unroll
		for (k=0;k<bs;k++)
		{
			jj = j + k * g.size();
			vals[k] = (ValueType) a[jj];
			cols[k] = ja[jj];
		}
		// #pragma unroll
		// for (k=0;k<bs;k++)
			// x_buf[k] = x[cols[k]];
		// #pragma unroll
		// for (k=0;k<bs;k++)
		// {
			// sums[k] += vals[k] * x[cols[k]];
			// sums[k] += vals[k] * x_buf[k];
			// sum = __fma_rn(vals[k], x_buf[k], sum);
		// }
		#pragma unroll
		for (k=0;k<bs;k++)
			sum = __fma_rn(vals[k], x[cols[k]], sum);
	}
	// for (k=0;k<bs;k++)
		// sum += sums[k];
	for (j=j_mul;j<j_e;j+=g.size())
		sum = __fma_rn((ValueType) a[j], x[ja[j]], sum); */

	for (j=j_s;j<j_e;j+=g.size())
	{
		sum = __fma_rn((ValueType) a[j], x[ja[j]], sum);
	}

	y[i] = sum;
}


__device__
__forceinline__
uint64_t
globaltimer()
{
	uint64_t t;
	asm volatile("mov.u64 %0, %%globaltimer;" : "=l"(t));
	return t;
}


__global__
void
gpu_kernel_sell_sorted(INT_T crossover_row, INT_T crossover_offset,
		INT_T * thread_warp_i_s,
		warp_coords_t * thread_warp_coords,
		INT_T * row_cluster_ptr, INT_T * ja, ValueType * a, INT_T m, INT_T n, INT_T nnz, ValueType * restrict x, ValueType * restrict y,
		unsigned long long * timers)
{
	const int tid = cuda_get_thread_num_bc();
	// const int tid = blockIdx.x * BLOCK_SIZE + threadIdx.x;

	#if GPU_TIMERS == 1
		thread_block_tile<32> g = tiled_partition<32>(this_thread_block());
		const int tidw = tid % 32;
		const int wid = tid / 32;
		unsigned long long ts, te, dt;
		g.sync();
		if (tidw == 0)
			ts = clock64();
	#endif

	if (tid < crossover_row)
		spmv_sell(tid, row_cluster_ptr, ja, a, m, n, nnz, x, y);
	else
	{
		spmv_csr<NNZ_PER_THREAD>(tid, crossover_row, crossover_offset, thread_warp_i_s, thread_warp_coords, ja, a, m, n, nnz, x, y);
	}

	#if GPU_TIMERS == 1
		g.sync();
		if (tidw == 0)
		{
			te = clock64();
			dt = te - ts;
			if (dt < timers[wid])
				timers[wid] = dt;
			// uint64_t c0 = clock64();
			// uint64_t t0 = globaltimer();
			// volatile int tmp = 0;
			// for (long i=0;i<100;i++)
				// tmp++;
			// uint64_t c1 = clock64();
			// uint64_t t1 = globaltimer();
			// double freq = (c1 - c0) / ((double) (t1 - t0));
			// if (wid % 1000 == 0)
				// printf("%d: freq=%g\n", wid, freq);
		}
	#endif
}


void
compute_sell_sorted(Cuda_SELL_Sorted_Hybrid_Arrays * restrict csr, ValueType * restrict x, ValueType * restrict y)
{
	cuda_assert(cudaEventRecord(csr->start_event, csr->stream));

	dim3 block_dims(BLOCK_SIZE);
	dim3 grid_dims(csr->num_thread_blocks);
	long shared_mem_size = 0;
	// shared_mem_size = BLOCK_SIZE * (sizeof(ValueType));
	// shared_mem_size = BLOCK_SIZE * (sizeof(ValueType) + sizeof(INT_T));
	// shared_mem_size = BLOCK_SIZE * NNZ_PER_THREAD * sizeof(INT_T);
	// shared_mem_size = BLOCK_SIZE * NNZ_PER_THREAD * (sizeof(ValueType) + sizeof(INT_T));

	#ifdef VECTOR_ALLOC_EXPLICIT
		// === EXPLICIT MODE: Full H2D/D2H pipeline with device buffers x_d, y_d ===

		if (csr->m_cpu == -1) // --- STANDALONE MODE ---
		{
			#if STANDALONE_ITERATIVE_OPTIMIZATION
				if (csr->is_first_iteration) {
					cuda_assert(cudaMemcpyAsync(csr->x_d, x, csr->n * sizeof(*csr->x_d), cudaMemcpyHostToDevice, csr->stream));
				}
				// else {
				// // Subsequent iterations: Do nothing for H2D. x_d is populated via D2D from the previous iteration.
				// }
			#else
				// Old method: Always transfer full x from Host
				cuda_assert(cudaMemcpyAsync(csr->x_d, x, csr->n * sizeof(*csr->x_d), cudaMemcpyHostToDevice, csr->stream));
			#endif
		}
		else // --- HYBRID MODE ---
		{
			#if HYBRID_ITERATIVE_OPTIMIZATION
				if (csr->is_first_iteration) {
					// First iteration must bootstrap logically
					cuda_assert(cudaMemcpyAsync(csr->x_d, x, csr->n * sizeof(*csr->x_d), cudaMemcpyHostToDevice, csr->stream));
				} else {
					// Successive iterations evaluate off proactive dispatch linked inside h2d_done_event 
					cuda_assert(cudaStreamWaitEvent(csr->stream, csr->h2d_done_event, 0));
				}
			#else
				// Standard method: transfer full x every time
				cuda_assert(cudaMemcpyAsync(csr->x_d, x, csr->n * sizeof(*csr->x_d), cudaMemcpyHostToDevice, csr->stream));
			#endif
		}

		#if DETAILED_TIMING
			cuda_assert(cudaEventRecord(csr->h2d_event, csr->stream));
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
				cuda_assert(cudaEventRecord(csr->pure_memset_start, csr->memset_stream));
			#endif

			if (csr->m - csr->crossover_row > 0)
				cuda_assert(cudaMemsetAsync(csr->y_d + csr->offset + csr->crossover_row, 0, (csr->m - csr->crossover_row) * sizeof(*csr->y_d), csr->memset_stream));

			#if DETAILED_TIMING
				cuda_assert(cudaEventRecord(csr->pure_memset_stop, csr->memset_stream));
			#endif

			// Signal the main stream that the memset is done
			cuda_assert(cudaEventRecord(csr->memset_done_event, csr->memset_stream));
		}
		else // STANDALONE MODE
		{
			#if DETAILED_TIMING
				cuda_assert(cudaEventRecord(csr->pure_memset_start, csr->stream));
			#endif

			if (csr->m - csr->crossover_row > 0)
				cuda_assert(cudaMemsetAsync(csr->y_d + csr->offset + csr->crossover_row, 0, (csr->m - csr->crossover_row) * sizeof(*csr->y_d), csr->stream));


			#if DETAILED_TIMING
				cuda_assert(cudaEventRecord(csr->pure_memset_stop, csr->stream));
			#endif
		}

		if (csr->m_cpu != -1) // HYBRID MODE
		{
			// Wait on the overlapping stream's background memset loop!
			cuda_assert(cudaStreamWaitEvent(csr->stream, csr->memset_done_event, 0));
		}

		// Pointer swapping eliminates the D2D background transfer completely!

		#if DETAILED_TIMING
			// For accurate pure-kernel profiling, we isolate "pre-kernel synchronization" (Memset Wait) here
			cuda_assert(cudaEventRecord(csr->memset_event, csr->stream));
		#endif

		gpu_kernel_sell_sorted<<<grid_dims, block_dims, shared_mem_size, csr->stream>>>(
				csr->crossover_row, csr->row_ptr_h[csr->crossover_row],
				csr->thread_warp_i_s_d,
				csr->thread_warp_coords_d,
				csr->row_cluster_ptr_d, csr->ja_d, csr->a_d, csr->m, csr->n, csr->nnz_extended, csr->x_d, csr->y_d + csr->offset, csr->timers_d);

		// Record completion of Kernel
		cuda_assert(cudaEventRecord(csr->kernel_event, csr->stream));

		cuda_assert(cudaPeekAtLastError());
		// cuda_assert(cudaDeviceSynchronize()); // Removed for async overlap

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
			/*
			cuda_assert(cudaMemcpy(csr->y_h, csr->y_d, csr->m * sizeof(*csr->y_d), cudaMemcpyDeviceToHost));

			for (long i=0;i<csr->m;i++)
			{
				y[i] = csr->y_h[csr->row_permutation[i]];
			}
			// memcpy(y, csr->y_h, csr->m * sizeof(ValueType));
			*/
			// This may need fix, because permutation is necessary!!!
			cuda_assert(cudaMemcpyAsync(y + csr->offset, csr->y_d + csr->offset, csr->m * sizeof(*csr->y_d), cudaMemcpyDeviceToHost, csr->stream));
		}
		
		#if DETAILED_TIMING
			cuda_assert(cudaEventRecord(csr->d2h_event, csr->stream));
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
		cuda_assert(cudaMemsetAsync(y + csr->offset, 0, csr->m * sizeof(*y), csr->stream));

		#if DETAILED_TIMING
			cuda_assert(cudaEventRecord(csr->memset_event, csr->stream));
		#endif

		// Launch kernel with host pointers directly (zero-copy / ATS / unified memory)
		gpu_kernel_sell_sorted<<<grid_dims, block_dims, shared_mem_size, csr->stream>>>(
				csr->crossover_row, csr->row_ptr_h[csr->crossover_row],
				csr->thread_warp_i_s_d,
				csr->thread_warp_coords_d,
				csr->row_cluster_ptr_d, csr->ja_d, csr->a_d, csr->m, csr->n, csr->nnz_extended, x, y + csr->offset, csr->timers_d);

		cuda_assert(cudaPeekAtLastError());

		if (csr->y == NULL)
			csr->y = y;

	#endif

	cuda_assert(cudaEventRecord(csr->stop_event, csr->stream));
}


//==========================================================================================================================================
//= Print Statistics
//==========================================================================================================================================


void
Cuda_SELL_Sorted_Hybrid_Arrays::statistics_start()
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
cuda_sell_sorted_hybrid_statistics_print_labels(char * buf, long buf_n)
{
	long i = 0;
	i += snprintf(buf+i, buf_n-i, ",nnz_sell");
	i += snprintf(buf+i, buf_n-i, ",nnz_csr");
	return i;
}


int
Cuda_SELL_Sorted_Hybrid_Arrays::statistics_print_data(char * buf, long buf_n)
{
	long i = 0;
	i += snprintf(buf+i, buf_n-i, ",%ld", nnz_sell);
	i += snprintf(buf+i, buf_n-i, ",%ld", nnz_csr);

	#if GPU_TIMERS
	{
		long fig_name_n = 1000;
		char fig_name[fig_name_n];
		long j;

		cuda_assert(cudaMemcpy(timers, timers_d, num_thread_warps * sizeof(*timers_d), cudaMemcpyDeviceToHost));

		snprintf(fig_name, fig_name_n, "figures/%s_warp_cycles.png", filename_base);

		long x_num_pixels = 1080, y_num_pixels = 1080;
		struct Figure_Series * s;
		__attribute__((cleanup(figure_destroy))) struct Figure * fig = (typeof(fig)) malloc(sizeof(*fig));
		figure_init(fig, x_num_pixels, y_num_pixels);
		{
			s = figure_add_series(fig, timers, NULL, NULL, num_thread_warps, 0, ull_to_double, NULL, NULL);
		}
		int hor_line[x_num_pixels];
		int cross_line_y = y_num_pixels * crossover_row / m;
		{
			for (j=0;j<x_num_pixels;j++)
				hor_line[j] = cross_line_y;
			s = figure_add_series(fig, NULL, hor_line, NULL, x_num_pixels, 0, NULL, int_to_double, NULL);
			figure_series_type_pixel_coords(s);
			figure_series_set_color(s, -1, 0, 0);
		}
		figure_set_bounds_x_min(fig, 0);
		figure_axes_flip_y(fig);
		figure_enable_legend(fig);
		figure_set_title(fig, "warp run times (cycles)");
		figure_plot(fig);
		figure_save(fig, fig_name);

		// printf("timer[0] = %llu\n", timers[0]);
	}
	#endif

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

	return i;
}

#ifndef HYBRID
struct Matrix_Format *
csr_to_format(INT_T * row_ptr, INT_T * col_ind, ValueTypeReference * values, long m, long n, long nnz, long symmetric, long symmetry_expanded)
{
	return cuda_sell_sorted_hybrid_to_format(row_ptr, col_ind, values, m, n, nnz, symmetric, symmetry_expanded, -1);
}

int
statistics_print_labels(char * buf, long buf_n)
{
	// synchronize();
	return cuda_sell_sorted_hybrid_statistics_print_labels(buf, buf_n);
}
#endif
