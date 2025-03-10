#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

#include "macros/cpp_defines.h"

#include "spmv_bench_common.h"
#include "spmv_kernel.h"

#include <immintrin.h>

#ifdef __cplusplus
extern "C"{
#endif
	#include "macros/macrolib.h"
	#include "time_it.h"
	#include "time_it_tsc.h"
	#include "parallel_util.h"
	#include "x86_util.h"
	#include "spmv_subkernel_csr_x86_d.hpp"

	#include "aux/hashtable.h"

#ifdef __cplusplus
}
#endif


INT_T * thread_i_s;
INT_T * thread_i_e;

extern int prefetch_distance;


struct CSRArrays : Matrix_Format
{
	INT_T * row_ptr;      // the usual rowptr (of size m+1)
	INT_T * ja;      // the colidx of each NNZ (of size nnz)
	long num_unique;
	ValueType * lut;
	long a_idx_len;
	unsigned char * a_idx;

	CSRArrays(INT_T * row_ptr_in, INT_T * col_ind, ValueTypeReference * values, long m, long n, long nnz) : Matrix_Format(m, n, nnz)
	{
		int num_threads = omp_get_max_threads();
		double time_balance;
		struct hashtable * ht;
		long i, j;

		row_ptr = (typeof(row_ptr)) aligned_alloc(64, (m+1) * sizeof(*row_ptr));
		ja = (typeof(ja)) aligned_alloc(64, nnz * sizeof(*ja));
		#pragma omp parallel for
		for (long i=0;i<m+1;i++)
			row_ptr[i] = row_ptr_in[i];
		#pragma omp parallel for
		for(long i=0;i<nnz;i++)
		{
			ja[i]=col_ind[i];
		}

		ht = hashtable_new(nnz);
		thread_i_s = (INT_T *) malloc(num_threads * sizeof(*thread_i_s));
		thread_i_e = (INT_T *) malloc(num_threads * sizeof(*thread_i_e));
		time_balance = time_it(1,
			_Pragma("omp parallel")
			{
				int tnum = omp_get_thread_num();
				int use_processes = atoi(getenv("USE_PROCESSES"));
				if (use_processes)
				{
					loop_partitioner_balance_iterations(num_threads, tnum, 0, m, &thread_i_s[tnum], &thread_i_e[tnum]);
				}
				else
				{
					loop_partitioner_balance_prefix_sums(num_threads, tnum, row_ptr, m, nnz, &thread_i_s[tnum], &thread_i_e[tnum]);
				}
			}
		);
		printf("balance time = %g\n", time_balance);
		_Pragma("omp parallel")
		{
			long j;
			_Pragma("omp for")
			for (j=0;j<nnz;j++)
			{
				hashtable_insert_concurrent(ht, values[j], -1, 0);
			}

			_Pragma("omp barrier")

			hashtable_entries_concurrent(ht, &lut, NULL, &num_unique);
		}

		j = 0;
		for (i=0;i<num_unique;i++)
		{
			// printf("%g\n", lut[i]);
			hashtable_insert_serial(ht, lut[i], j, 1);
			j++;
		}

		a_idx_len = (num_unique < 1LL<< 8) ? 1
			: (num_unique < 1LL<<16) ? 2
			: (num_unique < 1LL<<24) ? 3
			: 4;
		a_idx = (typeof(a_idx)) malloc(a_idx_len * nnz * sizeof(*a_idx));
		_Pragma("omp parallel")
		{
			long j, k;
			union {
				INT_T i;
				char c[sizeof(INT_T)];
			} idx;
			idx.i = 0;
			_Pragma("omp for")
			for (j=0;j<nnz;j++)
			{
				hashtable_contains(ht, values[j], &idx.i);
				for (k=0;k<a_idx_len;k++)
				{
					a_idx[a_idx_len * j + k] = idx.c[k];
				}
			}
		}

		hashtable_destroy(&ht);
	}

	~CSRArrays()
	{
		free(row_ptr);
		free(ja);
		free(lut);
		free(a_idx);
		free(thread_i_s);
		free(thread_i_e);
	}

	void spmv(ValueType * x, ValueType * y);
	void statistics_start();
	int statistics_print_data(char * buf, long buf_n);
};


void compute_csr_vector_x86(CSRArrays * restrict csr, ValueType * restrict x , ValueType * restrict y);
void compute_csr_vector_x86_queues(CSRArrays * restrict csr, ValueType * restrict x , ValueType * restrict y);
void compute_csr_vector_x86_perfect_nnz_balance(CSRArrays * restrict csr, ValueType * restrict x , ValueType * restrict y);


void
CSRArrays::spmv(ValueType * x, ValueType * y)
{
	compute_csr_vector_x86(this, x, y);
}


struct Matrix_Format *
csr_to_format(INT_T * row_ptr, INT_T * col_ind, ValueTypeReference * values, long m, long n, long nnz, long symmetric, long symmetry_expanded)
{
	if (symmetric && !symmetry_expanded)
		error("symmetric matrices have to be expanded to be supported by this format");
	struct CSRArrays * csr = new CSRArrays(row_ptr, col_ind, values, m, n, nnz);
	// for (long i=0;i<10;i++)
		// printf("%d\n", row_ptr[i]);
	// csr->mem_footprint = nnz * (sizeof(ValueType) + sizeof(INT_T)) + (m+1) * sizeof(INT_T);
	csr->mem_footprint = csr->num_unique * (sizeof(ValueType) + sizeof(csr->a_idx_len)) + nnz * sizeof(INT_T) + (m+1) * sizeof(INT_T);
	csr->format_name = (char *) "Custom_CSR_BV_LUT_x86";
	return csr;
}


//==========================================================================================================================================
//= CSR x86
//==========================================================================================================================================


__attribute__((hot,pure))
static __attribute__((always_inline)) inline
uint64_t
subkernel_csr_lut_extract_idx(long a_idx_len, unsigned char * a_idx, long j)
{
	const uint64_t idx_mask = (1ULL << (a_idx_len*8)) - 1;
	uint64_t idx = *((uint64_t *)&a_idx[j*a_idx_len]);
	idx &= idx_mask;
	return idx;
}
__attribute__((hot,pure))
static __attribute__((always_inline)) inline
uint64_t
subkernel_csr_lut_extract_idx_len1(__attribute__((unused)) long a_idx_len, unsigned char * a_idx, long j)
{
	uint64_t idx = a_idx[j];
	return idx;
}
__attribute__((hot,pure))
static __attribute__((always_inline)) inline
uint64_t
subkernel_csr_lut_extract_idx_len2(__attribute__((unused)) long a_idx_len, unsigned char * a_idx, long j)
{
	uint64_t idx = *((uint16_t *)&a_idx[j*a_idx_len]);
	return idx;
}
__attribute__((hot,pure))
static __attribute__((always_inline)) inline
uint64_t
subkernel_csr_lut_extract_idx_len4(__attribute__((unused)) long a_idx_len, unsigned char * a_idx, long j)
{
	uint64_t idx = *((uint32_t *)&a_idx[j*a_idx_len]);
	return idx;
}


__attribute__((hot,pure))
static __attribute__((always_inline)) inline
double
subkernel_csr_lut(INT_T * restrict ja, ValueType * restrict lut, long a_idx_len, unsigned char * a_idx, ValueType * restrict x, long j_s, long j_e,
		uint64_t (* extract_idx)(long a_idx_len, unsigned char * a_idx, long j))
{
	long j;
	ValueType sum = 0;
	sum = 0;
	for (j=j_s;j<j_e;j++)
	{
		uint64_t idx = extract_idx(a_idx_len, a_idx, j);
		sum += lut[idx] * x[ja[j]];
	}
	return sum;
}

__attribute__((hot,pure))
static __attribute__((always_inline)) inline
double
subkernel_csr_lut_x86_256d(INT_T * restrict ja, ValueType * restrict lut, long a_idx_len, unsigned char * a_idx, ValueType * restrict x, long j_s, long j_e,
		uint64_t (* extract_idx)(long a_idx_len, unsigned char * a_idx, long j))
{
	long j, j_e_vector;
	const uint64_t mask = ~(8ULL - 1); // Minimum number of elements for the vectorized code (power of 2).
	// const uint64_t idx_mask = (1ULL << (a_idx_len*8)) - 1;
	// const __m256i v_idx_mask = _mm256_set1_epi64x(idx_mask);
	__m256d v_a, v_x, v_sum;
	// __m256i v_idx;
	ValueType sum = 0, sum_v = 0;
	v_sum = _mm256_setzero_pd();
	sum = 0;
	sum_v = 0;
	j_e_vector = j_s + ((j_e - j_s) & mask);
	if (j_s != j_e_vector)
	{
		for (j=j_s;j<j_e_vector;j+=4)
		{
			// v_idx = _mm256_set_epi64x(*((uint64_t *)&a_idx[(j+3)*a_idx_len]), *((uint64_t *)&a_idx[(j+2)*a_idx_len]), *((uint64_t *)&a_idx[(j+1)*a_idx_len]), *((uint64_t *)&a_idx[(j+0)*a_idx_len]));
			// v_idx &= v_idx_mask;
			// v_a = _mm256_set_pd(lut[v_idx[3]], lut[v_idx[2]], lut[v_idx[1]], lut[v_idx[0]]);

			v_a = _mm256_set_pd(lut[extract_idx(a_idx_len, a_idx, j+3)], lut[extract_idx(a_idx_len, a_idx, j+2)], lut[extract_idx(a_idx_len, a_idx, j+1)], lut[extract_idx(a_idx_len, a_idx, j+0)]);
			v_x = _mm256_set_pd(x[ja[j+3]], x[ja[j+2]], x[ja[j+1]], x[ja[j+0]]);
			v_sum = _mm256_fmadd_pd(v_a, v_x, v_sum);

			// __m256d v_tmp, v_compensation = _mm256_setzero_pd();
			// v_x = _mm256_set_pd(x[ja[j+3]], x[ja[j+2]], x[ja[j+1]], x[ja[j+0]]);
			// v_a = _mm256_set_pd(lut[extract_idx(a_idx_len, a_idx, j+3)], lut[extract_idx(a_idx_len, a_idx, j+2)], lut[extract_idx(a_idx_len, a_idx, j+1)], lut[extract_idx(a_idx_len, a_idx, j+0)]);
			// v_a = _mm256_fmsub_pd(v_a, v_x, v_compensation);
			// v_tmp = v_sum + v_a;
			// v_compensation = (v_tmp - v_sum) - v_a;
			// v_sum = v_tmp;
		}
		sum_v = hsum256_pd(v_sum);
		// sum_v = v_sum[3] + v_sum[2] + v_sum[1] + v_sum[0];
	}
	for (j=j_e_vector;j<j_e;j++)
	{
		uint64_t idx = extract_idx(a_idx_len, a_idx, j);
		sum += lut[idx] * x[ja[j]];
	}
	return sum + sum_v;
}


void
compute_csr_vector_x86(CSRArrays * restrict csr, ValueType * restrict x, ValueType * restrict y)
{
	#pragma omp parallel
	{
		int tnum = omp_get_thread_num();
		long i, i_s, i_e, j_s, j_e;
		const long a_idx_len = csr->a_idx_len;
		i_s = thread_i_s[tnum];
		i_e = thread_i_e[tnum];
		j_e = csr->row_ptr[i_s];
		for (i=i_s;i<i_e;i++)
		{
			y[i] = 0;
			j_s = j_e;
			j_e = csr->row_ptr[i+1];
			if (j_s == j_e)
				continue;

			// switch (a_idx_len) {
				// case 1: y[i] = subkernel_csr_lut(csr->ja, csr->lut, a_idx_len, csr->a_idx, x, j_s, j_e, subkernel_csr_lut_extract_idx_len1); continue;
				// case 2: y[i] = subkernel_csr_lut(csr->ja, csr->lut, a_idx_len, csr->a_idx, x, j_s, j_e, subkernel_csr_lut_extract_idx_len2); continue;
				// case 4: y[i] = subkernel_csr_lut(csr->ja, csr->lut, a_idx_len, csr->a_idx, x, j_s, j_e, subkernel_csr_lut_extract_idx_len4); continue;
				// default: y[i] = subkernel_csr_lut(csr->ja, csr->lut, a_idx_len, csr->a_idx, x, j_s, j_e, subkernel_csr_lut_extract_idx);
			// }

			switch (a_idx_len) {
				case 1: y[i] = subkernel_csr_lut_x86_256d(csr->ja, csr->lut, a_idx_len, csr->a_idx, x, j_s, j_e, subkernel_csr_lut_extract_idx_len1); continue;
				case 2: y[i] = subkernel_csr_lut_x86_256d(csr->ja, csr->lut, a_idx_len, csr->a_idx, x, j_s, j_e, subkernel_csr_lut_extract_idx_len2); continue;
				case 4: y[i] = subkernel_csr_lut_x86_256d(csr->ja, csr->lut, a_idx_len, csr->a_idx, x, j_s, j_e, subkernel_csr_lut_extract_idx_len4); continue;
				default: y[i] = subkernel_csr_lut_x86_256d(csr->ja, csr->lut, a_idx_len, csr->a_idx, x, j_s, j_e, subkernel_csr_lut_extract_idx);
			}

			// y[i] = subkernel_csr_lut_len1(csr->ja, csr->lut, a_idx_len, csr->a_idx, x, j_s, j_e);
			// y[i] = subkernel_csr_lut(csr->ja, csr->lut, a_idx_len, csr->a_idx, x, j_s, j_e);
			// y[i] = subkernel_csr_lut_x86_256d(csr->ja, csr->lut, a_idx_len, csr->a_idx, x, j_s, j_e);
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
	// int num_threads = omp_get_max_threads();
	// long i, i_s, i_e;
	// for (i=0;i<num_threads;i++)
	// {
		// i_s = thread_i_s[i];
		// i_e = thread_i_e[i];
		// printf("%3ld: i=[%8ld, %8ld] (%8ld) , nnz=%8d\n", i, i_s, i_e, i_e - i_s, row_ptr[i_e] - row_ptr[i_s]);
	// }
	printf("num_unique=%ld, a_idx_len=%ld\n", num_unique, a_idx_len);
	return 0;
}

