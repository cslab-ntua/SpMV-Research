#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

#include <cuda.h>
#include <cooperative_groups.h>
#include <cuda_pipeline_primitives.h>

#include "macros/cpp_defines.h"

#include "../spmv_kernel.h"


#define ValueTypeStored  ValueType
// #define ValueTypeStored  float


#ifndef BLOCK_SIZE
	// #define BLOCK_SIZE  32
	// #define BLOCK_SIZE  64
	#define BLOCK_SIZE  128
	// #define BLOCK_SIZE  256
	// #define BLOCK_SIZE  512
	// #define BLOCK_SIZE  1024
#endif


#ifdef __cplusplus
extern "C"{
#endif
	#include "macros/macrolib.h"
	#include "time_it.h"
	#include "parallel_util.h"
	#include "array_metrics.h"

	#include "aux/csr_util.h"

	#include "cuda/cuda_util.h"

	static inline
	double
	idx_to_double(void * A, long i)
	{
		return (double) ((INT_T *) A)[i];
	}

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

	#include "sort/bucketsort/bucketsort_gen_undef.h"
	#define BUCKETSORT_GEN_TYPE_1  INT_T
	#define BUCKETSORT_GEN_TYPE_2  INT_T
	#define BUCKETSORT_GEN_TYPE_3  int
	#define BUCKETSORT_GEN_TYPE_4  int
	#define BUCKETSORT_GEN_SUFFIX  _CUDA_CSR_TRANSPOSE_EXPAND_ROWS_CU
	#include "sort/bucketsort/bucketsort_gen.c"
	static inline
	INT_T
	bucketsort_find_bucket(INT_T * A, long i, __attribute__((unused)) INT_T * degree_max_ptr)
	{
		return A[i+1] - A[i];   // Ascending order.
		// return *degree_max_ptr - (A[i+1] - A[i]);   // Descending order.
	}

	struct samplesort_csr_s {
		INT_T * row_ptr;
		INT_T * ja;
		// ValueType * a;
	};
	#include "sort/samplesort/samplesort_gen_undef.h"
	#define SAMPLESORT_GEN_TYPE_1  INT_T
	#define SAMPLESORT_GEN_TYPE_2  INT_T
	#define SAMPLESORT_GEN_TYPE_3  int
	#define SAMPLESORT_GEN_TYPE_4  struct samplesort_csr_s
	#define SAMPLESORT_GEN_FUNCTION_ATTRIBUTES
	#define SAMPLESORT_GEN_SUFFIX  _CUDA_CSR_TRANSPOSE_EXPAND_ROWS_CU
	#include "sort/samplesort/samplesort_gen.c"
	static inline
	int
	samplesort_cmp(INT_T a, INT_T b, struct samplesort_csr_s * csr)
	{
		__attribute__((unused)) INT_T j_s_a=csr->row_ptr[a], j_e_a=csr->row_ptr[a+1] - 1;
		__attribute__((unused)) INT_T j_s_b=csr->row_ptr[b], j_e_b=csr->row_ptr[b+1] - 1;
		__attribute__((unused)) INT_T col_s_a = csr->ja[j_s_a], col_e_a = csr->ja[j_e_a];
		__attribute__((unused)) INT_T col_s_b = csr->ja[j_s_b], col_e_b = csr->ja[j_e_b];
		__attribute__((unused)) INT_T col_center_a = (col_s_a + col_e_a) / 2;
		__attribute__((unused)) INT_T col_center_b = (col_s_b + col_e_b) / 2;
		__attribute__((unused)) INT_T degree_a = j_e_a - j_s_a;
		__attribute__((unused)) INT_T degree_b = j_e_b - j_s_b;
		__attribute__((unused)) double col_median_a = 0, col_median_b = 0;
		__attribute__((unused)) double col_mean_a = 0, col_mean_b = 0;
		if (degree_a != 0)
		{
			array_seg_quantile_serial(csr->ja, j_s_a, j_e_a, 0.5, NULL, &col_median_a, idx_to_double);
			array_seg_mean_serial(csr->ja, j_s_a, j_e_a, &col_mean_a, idx_to_double);
		}
		if (degree_b != 0)
		{
			array_seg_quantile_serial(csr->ja, j_s_b, j_e_b, 0.5, NULL, &col_median_b, idx_to_double);
			array_seg_mean_serial(csr->ja, j_s_b, j_e_b, &col_mean_b, idx_to_double);
		}
		int ret = 0;
		ret = (degree_a > degree_b) ? 1 : (degree_a < degree_b) ? -1 : 0;
		if (ret == 0)
		{
			// if (col_center_a == col_center_b)
				// return 0;
		}
		// if (ret == 0) ret = (col_e_a > col_e_b) ? 1 : (col_e_a < col_e_b) ? -1 : 0;
		// if (ret == 0) ret = (col_s_a > col_s_b) ? 1 : (col_s_a < col_s_b) ? -1 : 0;
		// if (ret == 0) ret = (col_center_a > col_center_b) ? 1 : (col_center_a < col_center_b) ? -1 : 0;
		// if (ret == 0) ret = (col_median_a > col_median_b) ? 1 : (col_median_a < col_median_b) ? -1 : 0;
		if (ret == 0) ret = (col_mean_a > col_mean_b) ? 1 : (col_mean_a < col_mean_b) ? -1 : 0;
		if (ret == 0) ret = a > b ? 1 : a < b ? -1 : 0;
		return ret;
	}

#ifdef __cplusplus
}
#endif


namespace cg = cooperative_groups;

using namespace cooperative_groups;


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
	long nnz_per_thread;
	long nnz_per_block;
	long nnz_per_warp = 32 * nnz_per_thread;

	long nnz_extended;

	ValueType * a;

	INT_T * row_ptr_h;
	INT_T * ja_h;
	ValueTypeStored * a_h;
	INT_T * thread_warp_i_s = NULL;
	INT_T * thread_warp_i_e = NULL;
	INT_T * thread_i_s = NULL;

	INT_T * row_ptr_d;
	INT_T * ja_d;
	ValueTypeStored * a_d;
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

	INT_T * row_permutation = NULL;

	CSRArrays(INT_T * row_ptr, INT_T * ja, ValueTypeReference * a_ref, long m, long n, long nnz) : Matrix_Format(m, n, nnz)
	{
		double nnz_per_row = ((double) nnz) / m;
		if (nnz_per_row < 1)
			nnz_per_thread = 1;
		else if (nnz_per_row < 3)
			nnz_per_thread = nnz_per_row + 0.5;
		else if (nnz_per_row < 5)
			nnz_per_thread = nnz_per_row + 0.9;
		else
			nnz_per_thread = 5;

		nnz_per_block = nnz_per_thread * BLOCK_SIZE;
		nnz_per_warp = nnz_per_thread * 32;

		double time_balance;
		INT_T degree_max;
		long i;

		cuda_device_print_attributes();

		thread_block_size = BLOCK_SIZE;

		// Convert values from ValueTypeReference (double) to ValueType (e.g., float).
		a = (typeof(a)) malloc(nnz * sizeof(*a));
		#pragma omp parallel for
		for (long i = 0; i < nnz; i++)
			a[i] = (ValueType) a_ref[i];

		// Extend rows to multiple of nnz_per_thread.
		row_ptr_h = (typeof(row_ptr_h)) malloc((m+1) * sizeof(*row_ptr_h));
		_Pragma("omp parallel")
		{
			long i;
			long degree;
			_Pragma("omp for")
			for (i=0;i<m;i++)
			{
				degree = row_ptr[i+1] - row_ptr[i];
				degree = nnz_per_thread * ((degree + nnz_per_thread - 1) / nnz_per_thread);
				row_ptr_h[i] = degree;
			}
		}
		double max;
		long max_id;
		array_max(row_ptr_h, m, &max, &max_id, idx_to_double);
		degree_max = max;
		printf("degree_max=%d\n", degree_max);
		row_ptr_h[m] = 0;
		scan_reduce(row_ptr_h, row_ptr_h, m+1, 0, 1, 0);
		nnz_extended = row_ptr_h[m];
		nnz_extended = nnz_per_block * ((nnz_extended + nnz_per_block - 1) / nnz_per_block);   // Extend nnz to multiple of BLOCK_SIZE threads, i.e., multiple of 'nnz_per_thread * BLOCK_SIZE' nonzeros.
		row_ptr_h[m] = nnz_extended;                                                           // Put the padding in the last row.
		printf("nnz_extended=%ld\n", nnz_extended);
		ja_h = (typeof(ja_h)) malloc(nnz_extended * sizeof(*ja_h));
		a_h = (typeof(a_h)) malloc(nnz_extended * sizeof(*a_h));
		_Pragma("omp parallel")
		{
			long i, j1, j2, j_s, j_e;
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
				j_s = row_ptr_h[i];
				while (j_s < row_ptr_h[i+1])
				{
					j_e = j_s + nnz_per_warp;
					j_e = j_e - (j_e % nnz_per_warp);
					if (j_e > row_ptr_h[i+1])
						j_e = row_ptr_h[i+1];
					long local_num_threads = (j_e - j_s) / nnz_per_thread;
					transpose(&a_h[j_s], nnz_per_thread, local_num_threads);
					transpose(&ja_h[j_s], nnz_per_thread, local_num_threads);
					j_s = j_e;
				}
				// j_s = row_ptr_h[i];
				// j_e = row_ptr_h[i+1];
				// long local_num_threads = (j_e - j_s) / nnz_per_thread;
				// transpose(&a_h[j_s], nnz_per_thread, local_num_threads);
				// transpose(&ja_h[j_s], nnz_per_thread, local_num_threads);
			}
		}


		row_permutation = (typeof(row_permutation)) malloc(m * sizeof(*row_permutation));
		_Pragma("omp parallel")
		{
			long i;
			_Pragma("omp for")
			for (i=0;i<m;i++)
				row_permutation[i] = i;
		}

		/* double time_sort_rows = time_it(1,
			__attribute__((unused)) long enable_legend = 1;
			__attribute__((unused)) long num_pixels_x = 1080;
			__attribute__((unused)) long num_pixels_y = 1080;
			// csr_plot("matrix", row_ptr_h, ja_h, a_h, m, n, nnz_extended, enable_legend, num_pixels_x, num_pixels_y);
			bucketsort_stable_recalculate_bucket_serial(row_ptr_h, m, degree_max+1, &degree_max, row_permutation, NULL);
			// INT_T * rev_row_permutation = (typeof(rev_row_permutation)) malloc(m * sizeof(*rev_row_permutation));
			// _Pragma("omp parallel")
			// {
				// long i;
				// _Pragma("omp for")
				// for (i=0;i<m;i++)
					// rev_row_permutation[i] = i;
			// }
			// struct samplesort_csr_s tmp_csr = { .row_ptr=row_ptr_h, .ja=ja_h };
			// samplesort(rev_row_permutation, m, &tmp_csr);
			// for (i=0;i<m;i++)
				// row_permutation[rev_row_permutation[i]] = i;
			// free(rev_row_permutation);
			INT_T * reordered_row_ptr = (typeof(reordered_row_ptr)) malloc((m+1) * sizeof(*reordered_row_ptr));
			INT_T * reordered_col_idx = (typeof(reordered_col_idx)) malloc(nnz_extended * sizeof(*reordered_col_idx));
			ValueType * reordered_values = (typeof(reordered_values)) malloc(nnz_extended * sizeof(*reordered_values));
			csr_reorder_rows(row_permutation, row_ptr_h, ja_h, a_h, m, n, nnz_extended, reordered_row_ptr, reordered_col_idx, reordered_values);
			free(row_ptr_h);
			free(ja_h);
			free(a_h);
			row_ptr_h = reordered_row_ptr;
			ja_h = reordered_col_idx;
			a_h = reordered_values;
			INT_T degree, degree_prev = 0;
			for (i=0;i<m;i++)
			{
				degree = row_ptr_h[i+1] - row_ptr_h[i];
				if (degree < degree_prev)
					error("");
				degree_prev = degree;
			}
			// csr_plot("matrix_reordered", row_ptr_h, ja_h, a_h, m, n, nnz_extended, enable_legend, num_pixels_x, num_pixels_y);
		);
		printf("time sort rows = %g\n", time_sort_rows); */


		num_thread_blocks = (nnz_extended + nnz_per_block - 1) / nnz_per_block;
		num_threads = num_thread_blocks * BLOCK_SIZE;
		num_thread_warps = num_threads / 32;
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
					if (thread_warp_j_s > nnz_extended)
						thread_warp_j_s = nnz_extended;
					macros_binary_search(row_ptr_h, 0, m, thread_warp_j_s, &lower_boundary, NULL);           // Index boundaries are inclusive.
					while (row_ptr_h[lower_boundary] == row_ptr_h[lower_boundary+1])
						lower_boundary++;
					thread_warp_i_s[i] = lower_boundary;
					thread_warp_j_e = thread_warp_j_s + nnz_per_warp;
					if (thread_warp_j_e > nnz_extended)
						thread_warp_j_e = nnz_extended;
					macros_binary_search(row_ptr_h, 0, m, thread_warp_j_e, NULL, &higher_boundary);           // Index boundaries are inclusive.
					while (row_ptr_h[higher_boundary] == row_ptr_h[higher_boundary+1])
						higher_boundary--;
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
					thread_j_s = nnz_per_thread * i;
					if (thread_j_s > nnz_extended)
						thread_j_s = nnz_extended;
					macros_binary_search(row_ptr_h, 0, m, thread_j_s, &lower_boundary, NULL);           // Index boundaries are inclusive.
					while (row_ptr_h[lower_boundary] == row_ptr_h[lower_boundary+1])
						lower_boundary++;
					// thread_i_s[i] = lower_boundary;
					// INT_T thread_j_e = nnz_per_thread * (i+1);
					// if (thread_j_s < row_ptr_h[i] || thread_j_e > row_ptr_h[i+1])
						// printf("error: [%d %d] , %ld : [%d %d]\n", thread_j_s, thread_j_e, lower_boundary, row_ptr_h[lower_boundary], row_ptr_h[lower_boundary+1]);
				}
			}
		);
		printf("balance time threads = %g\n", time_balance);

		cuda_assert(cudaMalloc(&row_ptr_d, (m+1) * sizeof(*row_ptr_d)));
		cuda_assert(cudaMalloc(&ja_d, nnz_extended * sizeof(*ja_d)));
		cuda_assert(cudaMalloc(&a_d, nnz_extended * sizeof(*a_d)));
		cuda_assert(cudaMalloc(&thread_warp_i_s_d, num_thread_warps * sizeof(*thread_warp_i_s_d)));
		cuda_assert(cudaMalloc(&thread_warp_i_e_d, num_thread_warps * sizeof(*thread_warp_i_e_d)));
		cuda_assert(cudaMalloc(&thread_i_s_d, num_threads * sizeof(*thread_i_s_d)));
		cuda_assert(cudaMalloc(&x_d, n * sizeof(*x_d)));
		cuda_assert(cudaMalloc(&y_d, m * sizeof(*y_d)));

		x_h = (typeof(x_h)) malloc(n * sizeof(*x_h));
		y_h = (typeof(y_h)) malloc(m * sizeof(*y_h));

		_Pragma("omp parallel")
		{
			long i_s, i_e, j;
			_Pragma("omp for")
			for (j=0;j<nnz_extended;j+=32*nnz_per_thread)
			{
				long j_e = j + 32*nnz_per_thread;
				if (j_e > nnz_extended)
					j_e = nnz_extended;
				macros_binary_search(row_ptr_h, 0, m, j, &i_s, NULL);           // Index boundaries are inclusive.
				macros_binary_search(row_ptr_h, 0, m, j_e-1, &i_e, NULL);           // Index boundaries are inclusive.
				if (i_s == i_e)
				{
					// int tid_s = j / nnz_per_thread;
					// int tid_e = (j_e + nnz_per_thread - 1) / nnz_per_thread;
					// for (int t=tid_s;t<tid_e;t++)
						// thread_i_s[t] = thread_i_s[t] | 0x80000000;
					// int wid = j / (32*nnz_per_thread);
					// thread_warp_i_s[wid] = thread_warp_i_s[wid] | 0x80000000;
					// for (long jj=j;jj<j_e;jj++)
						// ja_h[jj] = ja_h[jj] | 0x80000000;
				}
			}
		}

		_Pragma("omp parallel")
		{
			long i;
			_Pragma("omp for")
			for (i=0;i<m;i++)
			{
				// if (row_ptr_h[i] == row_ptr_h[i+1])
					// error("empty row");
				ja_h[row_ptr_h[i]] |= 0x80000000;
			}
		}

		_Pragma("omp parallel")
		{
			long j;
			_Pragma("omp for")
			for (j=0;j<nnz_extended;j+=32*nnz_per_thread)
			{
				transpose(&a_h[j], 32, nnz_per_thread);
				transpose(&ja_h[j], 32, nnz_per_thread);
			}
		}

		// _Pragma("omp parallel")
		// {
			// long j;
			// _Pragma("omp for")
			// for (j=0;j<nnz_extended;j+=BLOCK_SIZE*nnz_per_thread)
			// {
				// transpose(&a_h[j], BLOCK_SIZE, nnz_per_thread);
				// transpose(&ja_h[j], BLOCK_SIZE, nnz_per_thread);
			// }
		// }

		cuda_assert(cudaMemcpy(row_ptr_d, row_ptr_h, (m+1) * sizeof(*row_ptr_d), cudaMemcpyHostToDevice));
		cuda_assert(cudaMemcpy(ja_d, ja_h, nnz_extended * sizeof(*ja_d), cudaMemcpyHostToDevice));
		cuda_assert(cudaMemcpy(a_d, a_h, nnz_extended * sizeof(*a_d), cudaMemcpyHostToDevice));
		cuda_assert(cudaMemcpy(thread_warp_i_s_d, thread_warp_i_s, num_thread_warps * sizeof(*thread_warp_i_s_d), cudaMemcpyHostToDevice));
		cuda_assert(cudaMemcpy(thread_warp_i_e_d, thread_warp_i_e, num_thread_warps * sizeof(*thread_warp_i_e_d), cudaMemcpyHostToDevice));
		cuda_assert(cudaMemcpy(thread_i_s_d, thread_i_s, num_threads * sizeof(*thread_i_s_d), cudaMemcpyHostToDevice));
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
	csr->mem_footprint = csr->nnz_extended * (sizeof(ValueTypeStored) + sizeof(INT_T)) + (m+1) * sizeof(INT_T);
	char *format_name;
	format_name = (char *)malloc(100*sizeof(char));
	snprintf(format_name, 100, "Custom_CSR_CUDA_transpose_expand_rows_b%d_nnz%ld", BLOCK_SIZE, csr->nnz_per_thread);
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
spmv_full_block(INT_T * thread_i_s, INT_T * thread_warp_i_s, INT_T * thread_warp_i_e, INT_T * row_ptr, INT_T * ja, ValueTypeStored * a, long m, long n, long nnz, ValueType * restrict x, ValueType * restrict y)
{
	extern __shared__ char sm[];
	thread_block_tile<32> g = tiled_partition<32>(this_thread_block());
	const int tid = cuda_get_thread_num_bc();
	// const int tid = blockIdx.x * BLOCK_SIZE + threadIdx.x;
	const int tidw = g.thread_rank();
	const int wid = tid / 32;
	const int block_id = blockIdx.x;
	// const int widb = threadIdx.x / 32;
	double sum;
	INT_T row;
	int single_row;
	// ValueType * a_buf = &(((typeof(a_buf)) sm)[widb*32*nnz_per_thread]);
	// INT_T * ja_buf = &(((typeof(ja_buf)) &sm[BLOCK_SIZE * nnz_per_thread * sizeof(ValueType)])[widb*32*nnz_per_thread]);
	__attribute__((unused)) int i, i_s, i_e, j, jj, j_s, j_e, j_b_s, j_w_s, k;
	j_b_s = block_id * BLOCK_SIZE * nnz_per_thread;
	j_w_s = wid * 32 * nnz_per_thread;
	// j_s = j_w_s + tidw * nnz_per_thread;
	j_s = tid * nnz_per_thread;
	j_e = j_s + nnz_per_thread;

	// const int tidb = threadIdx.x;
	// const int widb = tidb / 32;
	// INT_T * sm_row_t = &(((typeof(sm_row_t)) sm)[widb * 32]);

	// i = thread_i_s[tid];
	i_s = thread_warp_i_s[wid];
	i_e = thread_warp_i_e[wid];
	i = binary_search_gpu(row_ptr, i_s, i_e, j_s+1);  // 'j_s + 1' so that we evade empty rows.

	// for (j=j_s,jj=tidw;j<j_e;j++,jj+=g.size())
	// {
		// a_buf[jj] = a[j_w_s+jj];
		// ja_buf[jj] = ja[j_w_s+jj];
	// }

	sum = 0;
	row = 0;
	for (j=j_s,jj=j_w_s+tidw;j<j_e;j++,jj+=g.size())
	// for (j=j_s,jj=tidw;j<j_e;j++,jj+=g.size())
	{
		sum = __fma_rn((ValueType) a[jj], x[ja[jj] & 0x7FFFFFFF], sum);
		row |= ja[jj];
		// sum = __fma_rn((ValueType) a_buf[jj], x[ja_buf[jj] & 0x7FFFFFFF], sum);
		// row |= ja_buf[jj];
	}

	/* row &= 0x80000000;
	if (row)
		row = 1;
	if (tidw == 0)
		row = 0;
	// sm_row_t[tidw] = row;
	row = warp_scan_reduce_inclusive(g, row) + i_s;
	// if (wid == 0)
		// printf("%2d: row=%d\n", tid, row);
	// if (i != row)
		// printf("%2d: row=%d i=%d i_s=%d\n", tidw, row, i, i_s);
	i = row; */

	g.match_all(i, single_row);   // 'single_row' is passed as reference!!! Passing as pointer gives compilation error.
	if (single_row)
	{
		sum = reduce_warp_single_row(g, sum);
		if (tidw == 0)
			atomicAdd(&y[i], sum);
		// invoke_one(g, [y, i, sum]() {
			// atomicAdd(&y[i], sum);
		// });
	}
	else
	{
		reduce_warp(g, i, sum, y);
	}
}


template <const int nnz_per_thread>
__global__
void
gpu_kernel_spmv_row_indices_continuous(INT_T * thread_i_s, INT_T * thread_warp_i_s, INT_T * thread_warp_i_e, INT_T * row_ptr, INT_T * ja, ValueTypeStored * a, long m, long n, long nnz, ValueType * restrict x, ValueType * restrict y)
{
	spmv_full_block<nnz_per_thread>(thread_i_s, thread_warp_i_s, thread_warp_i_e, row_ptr, ja, a, m, n, nnz, x, y);
}


void
compute_csr(CSRArrays * restrict csr, ValueType * restrict x, ValueType * restrict y)
{
	dim3 block_dims(BLOCK_SIZE);
	dim3 grid_dims(csr->num_thread_blocks);
	long shared_mem_size = 0;
	// shared_mem_size = BLOCK_SIZE * (sizeof(ValueType));
	// shared_mem_size = BLOCK_SIZE * (sizeof(ValueType) + sizeof(INT_T));
	// shared_mem_size = BLOCK_SIZE * nnz_per_thread * sizeof(INT_T);
	// shared_mem_size = BLOCK_SIZE * nnz_per_thread * (sizeof(ValueType) + sizeof(INT_T));

	if (csr->x == NULL)
	{
		printf("Grid : {%d, %d, %d} blocks. Blocks : {%d, %d, %d} threads. Shared memory : %ld.\n", grid_dims.x, grid_dims.y, grid_dims.z, block_dims.x, block_dims.y, block_dims.z, shared_mem_size);
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
	switch (csr->nnz_per_thread) {
		case 1:
			gpu_kernel_spmv_row_indices_continuous<1><<<grid_dims, block_dims, shared_mem_size>>>(csr->thread_i_s_d, csr->thread_warp_i_s_d, csr->thread_warp_i_e_d, csr->row_ptr_d, csr->ja_d, csr->a_d, csr->m, csr->n, csr->nnz_extended, csr->x_d, csr->y_d);
			break;
		case 2:
			gpu_kernel_spmv_row_indices_continuous<2><<<grid_dims, block_dims, shared_mem_size>>>(csr->thread_i_s_d, csr->thread_warp_i_s_d, csr->thread_warp_i_e_d, csr->row_ptr_d, csr->ja_d, csr->a_d, csr->m, csr->n, csr->nnz_extended, csr->x_d, csr->y_d);
			break;
		case 3:
			gpu_kernel_spmv_row_indices_continuous<3><<<grid_dims, block_dims, shared_mem_size>>>(csr->thread_i_s_d, csr->thread_warp_i_s_d, csr->thread_warp_i_e_d, csr->row_ptr_d, csr->ja_d, csr->a_d, csr->m, csr->n, csr->nnz_extended, csr->x_d, csr->y_d);
			break;
		case 4:
			gpu_kernel_spmv_row_indices_continuous<4><<<grid_dims, block_dims, shared_mem_size>>>(csr->thread_i_s_d, csr->thread_warp_i_s_d, csr->thread_warp_i_e_d, csr->row_ptr_d, csr->ja_d, csr->a_d, csr->m, csr->n, csr->nnz_extended, csr->x_d, csr->y_d);
			break;
		default:
			gpu_kernel_spmv_row_indices_continuous<5><<<grid_dims, block_dims, shared_mem_size>>>(csr->thread_i_s_d, csr->thread_warp_i_s_d, csr->thread_warp_i_e_d, csr->row_ptr_d, csr->ja_d, csr->a_d, csr->m, csr->n, csr->nnz_extended, csr->x_d, csr->y_d);
	}
	cuda_assert(cudaPeekAtLastError());
	cuda_assert(cudaDeviceSynchronize());

	// exit(0);

	if (csr->y == NULL)
	{
		csr->y = y;

		cuda_assert(cudaMemcpy(csr->y_h, csr->y_d, csr->m * sizeof(*csr->y_d), cudaMemcpyDeviceToHost));

		for (long i=0;i<csr->m;i++)
		{
			y[i] = csr->y_h[csr->row_permutation[i]];
		}
		// memcpy(y, csr->y_h, csr->m * sizeof(ValueType));

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

