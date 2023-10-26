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
#ifdef __cplusplus
}
#endif


INT_T * thread_i_s;
INT_T * thread_i_e;

INT_T * thread_j_s;
INT_T * thread_j_e;

ValueType * thread_v_s;
ValueType * thread_v_e;

extern int prefetch_distance;


struct CSRArrays : Matrix_Format
{
	INT_T * ia;      // the usual rowptr (of size m+1)
	INT_T * ja;      // the colidx of each NNZ (of size nnz)
	ValueType * a;   // the values (of size NNZ)

	void compute_csr_vector_x86_timings(ValueType * restrict x, ValueType * restrict y, long * timings);

	CSRArrays(INT_T * ia, INT_T * ja, ValueType * a, long m, long n, long nnz) : Matrix_Format(m, n, nnz), ia(ia), ja(ja), a(a)
	{
		int num_threads = omp_get_max_threads();
		double time_balance;
		thread_i_s = (INT_T *) malloc(num_threads * sizeof(*thread_i_s));
		thread_i_e = (INT_T *) malloc(num_threads * sizeof(*thread_i_e));
		thread_j_s = (INT_T *) malloc(num_threads * sizeof(*thread_j_s));
		thread_j_e = (INT_T *) malloc(num_threads * sizeof(*thread_j_e));
		thread_v_s = (ValueType *) malloc(num_threads * sizeof(*thread_v_s));
		thread_v_e = (ValueType *) malloc(num_threads * sizeof(*thread_v_e));
		time_balance = time_it(1,
			#ifdef CUSTOM_X86_VECTOR_ORACLE_BALANCE
				#ifndef BLOCK_SIZE
					#define BLOCK_SIZE  64
				#endif
				ValueType * x;
				ValueType * y;
				int dimMultipleBlock, dimMultipleBlock_y;
				dimMultipleBlock = ((m+BLOCK_SIZE-1)/BLOCK_SIZE)*BLOCK_SIZE;
				dimMultipleBlock_y = ((n+BLOCK_SIZE-1)/BLOCK_SIZE)*BLOCK_SIZE;
				x = (ValueType *) aligned_alloc(64, dimMultipleBlock_y * sizeof(ValueType));
				_Pragma("omp parallel for")
				for (int idx=0;idx<dimMultipleBlock_y;++idx)
					x[idx] = 1.0;
				y = (ValueType *) aligned_alloc(64, dimMultipleBlock * sizeof(ValueType));
				_Pragma("omp parallel for")
				for (long i=0;i<dimMultipleBlock;i++)
					y[i] = 0.0;
				// long timings_num;
				long * timings = (typeof(timings)) malloc((m + 1) * sizeof(*timings));
				long * prefix_sums = (typeof(prefix_sums)) malloc((m + 1) * sizeof(*prefix_sums));
				// timings_num = m;
				_Pragma("omp parallel")
				{
					int tnum = omp_get_thread_num();
					long i;
					loop_partitioner_balance_prefix_sums(num_threads, tnum, ia, m, nnz, &thread_i_s[tnum], &thread_i_e[tnum]);
					_Pragma("omp for")
					for (i=0;i<m;i++)
						timings[i] = 0;
				}
				// spmv(x, y);
				for (long rep=0;rep<5;rep++)
				{
					compute_csr_vector_x86_timings(x, y, timings);
					double sum = 0, tmp;
					for (int i=0;i<m+1;i++)
					{
						tmp = timings[i];
						// prefix_sums[i] = sum;
						prefix_sums[i] = sum / (rep+1);
						sum += tmp;
					}
					_Pragma("omp parallel")
					{
						int tnum = omp_get_thread_num();
						loop_partitioner_balance_prefix_sums(num_threads, tnum, prefix_sums, m, prefix_sums[m], &thread_i_s[tnum], &thread_i_e[tnum]);
						if (tnum == num_threads - 1)
							thread_i_e[tnum] = m;
					}
					for (long i=0;i<num_threads;i++)
					{
						long i_s = thread_i_s[i];
						long i_e = thread_i_e[i];
						printf("%3ld: i=[%8ld, %8ld] (%8ld) , nnz=%8d , timings=%ld\n", i, i_s, i_e, i_e - i_s, ia[i_e] - ia[i_s], (prefix_sums[i_e] - prefix_sums[i_s]));
					}
					printf("\n");
				}
				// printf("timings_num = %ld\n", timings_num);
				// for (int i=0;i<10;i++)
					// printf("timings[%d] = %ld\n", i, timings[i]);
				free(timings);
				free(x);
				free(y);
				// sleep(10);
			#else
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
						#ifdef CUSTOM_X86_VECTOR_PERFECT_NNZ_BALANCE
							long lower_boundary;
							loop_partitioner_balance_iterations(num_threads, tnum, 0, nnz, &thread_j_s[tnum], &thread_j_e[tnum]);
							binary_search(ia, 0, m, thread_j_s[tnum], &lower_boundary, NULL);           // Index boundaries are inclusive.
							thread_i_s[tnum] = lower_boundary;
							_Pragma("omp barrier")
							if (tnum == num_threads - 1)   // If we calculate each thread's boundaries individually some empty rows might be unassigned.
								thread_i_e[tnum] = m;
							else
								thread_i_e[tnum] = thread_i_s[tnum+1] + 1;
						#else
							loop_partitioner_balance_prefix_sums(num_threads, tnum, ia, m, nnz, &thread_i_s[tnum], &thread_i_e[tnum]);
							// loop_partitioner_balance(num_threads, tnum, 2, ia, m, nnz, &thread_i_s[tnum], &thread_i_e[tnum]);
						#endif
					}
				}
			#endif
		);
		printf("balance time = %g\n", time_balance);
	}

	~CSRArrays()
	{
		free(a);
		free(ia);
		free(ja);
		free(thread_i_s);
		free(thread_i_e);
		free(thread_j_s);
		free(thread_j_e);
		free(thread_v_s);
		free(thread_v_e);
	}

	void spmv(ValueType * x, ValueType * y);
	void statistics_start();
	int statistics_print_data(__attribute__((unused)) char * buf, __attribute__((unused)) long buf_n);
};


void compute_csr_vector_x86(CSRArrays * restrict csr, ValueType * restrict x , ValueType * restrict y);
void compute_csr_vector_x86_queues(CSRArrays * restrict csr, ValueType * restrict x , ValueType * restrict y);
void compute_csr_vector_x86_perfect_nnz_balance(CSRArrays * restrict csr, ValueType * restrict x , ValueType * restrict y);


void
CSRArrays::spmv(ValueType * x, ValueType * y)
{
	#if defined(CUSTOM_X86_VECTOR)
		compute_csr_vector_x86(this, x, y);
	#elif defined(CUSTOM_X86_VECTOR_QUEUES)
		compute_csr_vector_x86_queues(this, x, y);
	#elif defined(CUSTOM_X86_VECTOR_PERFECT_NNZ_BALANCE)
		compute_csr_vector_x86_perfect_nnz_balance(this, x, y);
	#endif
}


struct Matrix_Format *
csr_to_format(INT_T * row_ptr, INT_T * col_ind, ValueType * values, long m, long n, long nnz)
{
	struct CSRArrays * csr = new CSRArrays(row_ptr, col_ind, values, m, n, nnz);
	// for (long i=0;i<10;i++)
		// printf("%d\n", row_ptr[i]);
	csr->mem_footprint = nnz * (sizeof(ValueType) + sizeof(INT_T)) + (m+1) * sizeof(INT_T);
	#if defined(CUSTOM_X86_VECTOR)
		csr->format_name = (char *) "Custom_CSR_BV_x86";
	#elif defined(CUSTOM_X86_VECTOR_PERFECT_NNZ_BALANCE)
		csr->format_name = (char *) "Custom_CSR_PBV_x86";
	#elif defined(CUSTOM_X86_VECTOR_QUEUES)
		csr->format_name = (char *) "Custom_CSR_BV_x86_QUEUES";
	#endif
	return csr;
}


//==========================================================================================================================================
//= CSR x86
//==========================================================================================================================================


void
compute_csr_vector_x86(CSRArrays * restrict csr, ValueType * restrict x, ValueType * restrict y)
{
	#pragma omp parallel
	{
		int tnum = omp_get_thread_num();
		long i_s, i_e;
		i_s = thread_i_s[tnum];
		i_e = thread_i_e[tnum];
		// subkernel_csr_scalar(csr->ia, csr->ja, csr->a, x, y, i_s, i_e);
		subkernel_csr_x86_density(csr->ia, csr->ja, csr->a, x, y, i_s, i_e);
		// subkernel_csr_vector_x86_512d(csr->ia, csr->ja, csr->a, x, y, i_s, i_e);
	}
}


//==========================================================================================================================================
//= CSR x86 timings
//==========================================================================================================================================


void
CSRArrays::compute_csr_vector_x86_timings(ValueType * restrict x, ValueType * restrict y, long * timings)
{
	// int num_threads = omp_get_max_threads();
	#pragma omp parallel
	{
		int tnum = omp_get_thread_num();
		long i, i_s, i_e, j_s, j_e, k, k_e, num_rows;
		long cycles;
		i_s = thread_i_s[tnum];
		i_e = thread_i_e[tnum];
		j_e = ia[i_s];
		for (i=i_s;i<i_e;i+=num_rows)
		{
			// for (k_e=i+1;k_e<i_e;k_e++)
			// {
				// if (ia[k_e] - ia[k_e-1] > 1)
				// {
					// if (k_e > i+1) // If not alone then time it alone next time.
						// k_e--;
					// break;
				// }
				// if (ia[k_e] - ia[i] > 64)
					// break;
			// }
			// num_rows = k_e - i;
			cycles = time_it_tsc(1,
				for (k_e=i+1;k_e<i_e;k_e++)
				{
					if (ia[k_e] - ia[k_e-1] > 8)
					{
						if (k_e > i+1) // If not alone then time it alone next time.
							k_e--;
						break;
					}
					if (k_e - i > 32)
						break;
				}
				num_rows = k_e - i;
				for (k=i;k<k_e;k++)
				{
					j_s = j_e;
					j_e = ia[k+1];
					if (j_s == j_e)
						continue;
					// y[k] = subkernel_row_csr_scalar(ja, a, x, j_s, j_e);
					y[k] += subkernel_row_csr_vector_x86_512d(ja, a, x, j_s, j_e);
				}
			);
			for (k=i;k<k_e;k++)
			{
				// timings[i] = cycles / num_rows;
				timings[i] += cycles / num_rows;
			}
		}
	}
}


//==========================================================================================================================================
//= CSR x86 Queues
//==========================================================================================================================================


__attribute__((hot))
static inline
void
compute_csr_x86_queues_vector(CSRArrays * restrict csr, ValueType * restrict x, ValueType * restrict y, int * qv_i, int * qv_j_s, int * qv_degree, int n)
{
	long i, j, j_s, j_e, k;
	__m256d v_a, v_x, v_sum;
	__attribute__((aligned(32))) ValueType x256d[4] = {0};
	for (k=0;k<n;k++)
	{
		i = qv_i[k];
		j_s = qv_j_s[k];
		j_e = j_s + qv_degree[k];
		v_sum = _mm256_setzero_pd();
		for (j=j_s;j<j_e;j+=4)
		{
			v_a = _mm256_loadu_pd(&csr->a[j]);
			x256d[0] = x[csr->ja[j]];
			x256d[1] = x[csr->ja[j+1]];
			x256d[2] = x[csr->ja[j+2]];
			x256d[3] = x[csr->ja[j+3]];
			v_x = _mm256_load_pd(x256d);
			v_sum = _mm256_fmadd_pd(v_a, v_x, v_sum);
		}
		y[i] = hsum256_pd(v_sum);
	}
}


void
compute_csr_vector_x86_queues(CSRArrays * restrict csr, ValueType * restrict x, ValueType * restrict y)
{
	#pragma omp parallel
	{
		int tnum = omp_get_thread_num();
		long i, i_s, i_e, j, j_s, j_e, k;
		// const long mask = ~(((long) 4) - 1); // Minimum number of elements for the vectorized code (power of 2).
		const long qv_n = 16;
		__attribute__((aligned(32))) int qv_i[qv_n],  qv_j_s[qv_n], qv_degree[qv_n];
		long iter_v = 0;
		long degree, degree_v, degree_s;
		__m256d v_a, v_x, v_sum;
		__attribute__((aligned(32))) ValueType x256d[4] = {0};
		ValueType sum = 0;
		i_s = thread_i_s[tnum];
		i_e = thread_i_e[tnum];
		for (i=i_s;i<i_e;i++)
		{
			y[i] = 0;
			j_s = csr->ia[i];
			j_e = csr->ia[i+1];
			if (j_s == j_e)
				continue;
			degree = j_e - j_s;
			degree_v = degree & (~3);
			degree_s = degree & 3;
			if (degree_v != 0)
			{
				qv_i[iter_v] = i;
				qv_j_s[iter_v] = j_s;
				qv_degree[iter_v] = degree_v;
				iter_v++;
				if (iter_v == qv_n)
				{
					#pragma GCC unroll(qv_n)
					for (k=0;k<qv_n;k++)
					{
						i = qv_i[k];
						v_sum = _mm256_setzero_pd();
						for (j=qv_j_s[k];j<qv_j_s[k]+qv_degree[k];j+=4)
						{
							v_a = _mm256_loadu_pd(&csr->a[j]);
							x256d[0] = x[csr->ja[j]];
							x256d[1] = x[csr->ja[j+1]];
							x256d[2] = x[csr->ja[j+2]];
							x256d[3] = x[csr->ja[j+3]];
							v_x = _mm256_load_pd(x256d);
							v_sum = _mm256_fmadd_pd(v_a, v_x, v_sum);
						}
						y[i] += hsum256_pd(v_sum);
					}
					// compute_csr_x86_queues_vector(csr, x, y, qv_i, qv_j_s, qv_degree, qv_n);
					iter_v = 0;
				}
			}
			if (degree_s != 0)
			{
				sum = 0;
				for (j=j_s+degree_v;j<j_e;j++)
					sum += csr->a[j] * x[csr->ja[j]];
				y[i] += sum;
			}
		}
	}
}


//==========================================================================================================================================
//= CSR x86 Perfect NNZ Balance
//==========================================================================================================================================


#if 0
__attribute__((hot,pure))
static inline
double
compute_csr_line_case_default(CSRArrays * csr, ValueType * x, INT_T j_s, INT_T j_e)
{
	long j;
	__m256d v_a, v_x, v_sum;
	ValueType x256d0, x256d1, x256d2, x256d3;
	v_sum = _mm256_setzero_pd();
	for (j=j_s;j<j_e;j+=4)
	{
		v_a = _mm256_loadu_pd(&csr->a[j]);
		x256d0 = x[csr->ja[j]];
		x256d1 = x[csr->ja[j + 1]];
		x256d2 = x[csr->ja[j + 2]];
		x256d3 = x[csr->ja[j + 3]];
		v_x = _mm256_set_pd(x256d3, x256d2, x256d1, x256d0);
		v_sum = _mm256_fmadd_pd(v_a, v_x, v_sum);
	}
	return hsum256_pd(v_sum);
}


__attribute__((hot,pure))
static inline
double
compute_csr_line_case(CSRArrays * restrict csr, ValueType * restrict x, INT_T j_s, INT_T j_e)
{

	__label__ OUT_LABEL, CASE1_LABEL, CASE2_LABEL, CASE3_LABEL;
	static const void * jump_table[4] = {
		[0] = &&OUT_LABEL,
		[1] = &&CASE1_LABEL,
		[2] = &&CASE2_LABEL,
		[3] = &&CASE3_LABEL,
	};

	const long mask = ~(((long) 4) - 1); // Minimum number of elements for the vectorized code (power of 2).
	ValueType sum, sum_v;
	long j, j_e_vector, degree_rem;
	ValueType * a;
	INT_T * ja;

	sum = 0;
	sum_v = 0;

	j_e_vector = j_s + ((j_e - j_s) & mask);
	degree_rem = j_e - j_e_vector;

	if (j_s != j_e_vector)
		sum_v = compute_csr_line_case_default(csr, x, j_s, j_e_vector);

	for (j=j_e_vector;j<j_e;j++)
		sum += csr->a[j] * x[csr->ja[j]];

	a = &csr->a[j_e_vector];
	ja = &csr->ja[j_e_vector];

	goto *jump_table[degree_rem];
	CASE3_LABEL:
		sum += a[2]*x[ja[2]];
	CASE2_LABEL:
		sum += a[1]*x[ja[1]];
	CASE1_LABEL:
		sum += a[0]*x[ja[0]];
	OUT_LABEL:

	return sum + sum_v;
}


__attribute__((hot,pure))
static inline
double
compute_csr_line(CSRArrays * csr, ValueType * x, INT_T j_s, INT_T j_e)
{
	return compute_csr_line_case(csr, x, j_s, j_e);
}
#endif


void
compute_csr_vector_x86_perfect_nnz_balance(CSRArrays * restrict csr, ValueType * restrict x, ValueType * restrict y)
{
	int num_threads = omp_get_max_threads();
	long t;
	#pragma omp parallel
	{
		int tnum = omp_get_thread_num();
		long i, i_s, i_e, j_s, j_e;
		thread_v_s[tnum] = 0;
		thread_v_e[tnum] = 0;

		i_s = thread_i_s[tnum];
		i_e = thread_i_e[tnum];

		i = i_s;
		y[i] = 0;
		j_s = csr->ia[i];
		j_e = csr->ia[i+1];
		if (thread_j_s[tnum] > j_s)
			j_s = thread_j_s[tnum];
		if (thread_j_e[tnum] < j_e)
			j_e = thread_j_e[tnum];
		if (j_s < j_e)
		{
			thread_v_s[tnum] = subkernel_row_csr_vector_x86(csr->ja, csr->a, x, j_s, j_e);
		}

		subkernel_csr_x86_density(csr->ia, csr->ja, csr->a, x, y, i_s+1, i_e-1);

		i = i_e-1;
		if (i > i_s)
		{
			y[i] = 0;
			j_s = csr->ia[i];
			j_e = csr->ia[i+1];
			if (thread_j_s[tnum] > j_s)
				j_s = thread_j_s[tnum];
			if (thread_j_e[tnum] < j_e)
				j_e = thread_j_e[tnum];
			if (j_s < j_e)
				thread_v_e[tnum] = subkernel_row_csr_vector_x86(csr->ja, csr->a, x, j_s, j_e);
		}
	}
	for (t=0;t<num_threads;t++)
	{
		y[thread_i_s[t]] += thread_v_s[t];
		if (thread_i_e[t] - 1 > thread_i_s[t])
			y[thread_i_e[t] - 1] += thread_v_e[t];
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
		// printf("%3ld: i=[%8ld, %8ld] (%8ld) , nnz=%8d\n", i, i_s, i_e, i_e - i_s, ia[i_e] - ia[i_s]);
	// }
	return 0;
}

