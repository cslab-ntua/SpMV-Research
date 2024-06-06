#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

#include "macros/cpp_defines.h"

#include "spmv_bench_common.h"
#include "spmv_kernel.h"

#include <arm_sve.h>

#ifdef __cplusplus
extern "C"{
#endif
	#include "macros/macrolib.h"
	#include "time_it.h"
	#include "parallel_util.h"
	#include "array_metrics.h"
	#include "spmv_subkernel_csr_sve_d.hpp"
#ifdef __cplusplus
}
#endif


INT_T * thread_i_s = NULL;
INT_T * thread_i_e = NULL;

INT_T * thread_j_s = NULL;
INT_T * thread_j_e = NULL;

// ValueType * thread_v_s = NULL;
ValueType * thread_v_e = NULL;

extern int prefetch_distance;

double * thread_time_compute, * thread_time_barrier;


struct CSRArrays : Matrix_Format
{
	INT_T * row_ptr;      // the usual rowptr (of size m+1)
	INT_T * ja;      // the colidx of each NNZ (of size nnz)
	ValueType * a;   // the values (of size NNZ)

	long num_loops;

	CSRArrays(INT_T * row_ptr, INT_T * ja, ValueType * a, long m, long n, long nnz) : Matrix_Format(m, n, nnz), row_ptr(row_ptr), ja(ja), a(a)
	{
		int num_threads = omp_get_max_threads();
		double time_balance;
		thread_i_s = (INT_T *) malloc(num_threads * sizeof(*thread_i_s));
		thread_i_e = (INT_T *) malloc(num_threads * sizeof(*thread_i_e));
		thread_j_s = (INT_T *) malloc(num_threads * sizeof(*thread_j_s));
		thread_j_e = (INT_T *) malloc(num_threads * sizeof(*thread_j_e));
		// thread_v_s = (ValueType *) malloc(num_threads * sizeof(*thread_v_s));
		thread_v_e = (ValueType *) malloc(num_threads * sizeof(*thread_v_e));
		time_balance = time_it(1,
			_Pragma("omp parallel")
			{
				int tnum = omp_get_thread_num();
				#if defined(NAIVE)
					loop_partitioner_balance_iterations(num_threads, tnum, 0, m, &thread_i_s[tnum], &thread_i_e[tnum]);
				#else
					int use_processes = atoi(getenv("USE_PROCESSES"));
					if (use_processes)
					{
						loop_partitioner_balance_iterations(num_threads, tnum, 0, m, &thread_i_s[tnum], &thread_i_e[tnum]);
					}
					else
					{
						#ifdef CUSTOM_SVE_VECTOR_PERFECT_NNZ_BALANCE
							long lower_boundary;
							// long higher_boundary;
							loop_partitioner_balance_iterations(num_threads, tnum, 0, nnz, &thread_j_s[tnum], &thread_j_e[tnum]);
							macros_binary_search(row_ptr, 0, m, thread_j_s[tnum], &lower_boundary, NULL);           // Index boundaries are inclusive.
							thread_i_s[tnum] = lower_boundary;
							// macros_binary_search(row_ptr, 0, m, thread_j_e[tnum] - 1, NULL, &higher_boundary);     // Index boundaries are inclusive.
							// thread_i_e[tnum] = higher_boundary;
							_Pragma("omp barrier")
							if (tnum == num_threads - 1)   // If we calculate each thread's boundaries individually some empty rows might be unassigned.
								thread_i_e[tnum] = m;
							else
								thread_i_e[tnum] = thread_i_s[tnum+1] + 1;
							// _Pragma("omp single")
							// {
								// this->row_ptr = (INT_T *) aligned_alloc(64, (m+1 + VECTOR_ELEM_NUM) * sizeof(INT_T));
							// }
							// _Pragma("omp barrier")
							// for (long i=thread_i_s[tnum];i<thread_i_e[tnum];i++)
								// this->row_ptr[i] = row_ptr[i];
							// if (tnum == num_threads - 1)
								// this->row_ptr[m] = row_ptr[m];
							#if 0
								_Pragma("omp barrier")
								_Pragma("omp single")
								{
									int i_s, i_e, j_s, j_e, t;
									for (t=0;t<num_threads;t++)
									{
										i_s = thread_i_s[t];
										i_e = thread_i_e[t];
										j_s = thread_j_s[t];
										j_e = thread_j_e[t];
										printf("%3d:  i=[%7d,%7d]  |  j=[%7d,%7d] (%7d)  ,  row_ptr[i]=[%7d,%7d] (%7d)  ,  row_ptr[i+1]=[%7d,%7d]\n",
											t,
											i_s, i_e,
											j_s, j_e, (j_e - j_s),
											row_ptr[i_s], row_ptr[i_e], (row_ptr[i_e] - row_ptr[i_s]),
											row_ptr[i_s+1], row_ptr[i_e+1]
										);
									}
								}
							#endif
						#else
							loop_partitioner_balance_prefix_sums(num_threads, tnum, row_ptr, m, nnz, &thread_i_s[tnum], &thread_i_e[tnum]);
							// loop_partitioner_balance(num_threads, tnum, 2, row_ptr, m, nnz, &thread_i_s[tnum], &thread_i_e[tnum]);
						#endif
					}
				#endif
			}
		);
		printf("balance time = %g\n", time_balance);

		#ifdef PRINT_STATISTICS
			long i;
			num_loops = 0;
			thread_time_barrier = (double *) malloc(num_threads * sizeof(*thread_time_barrier));
			thread_time_compute = (double *) malloc(num_threads * sizeof(*thread_time_compute));
			for (i=0;i<num_threads;i++)
			{
				long rows, nnz;
				INT_T i_s, i_e, j_s, j_e;
				i_s = thread_i_s[i];
				i_e = thread_i_e[i];
				j_s = thread_j_s[i];
				j_e = thread_j_e[i];
				rows = i_e - i_s;
				nnz = row_ptr[i_e] - row_ptr[i_s];
				printf("%10ld: rows=[%10d(%10d), %10d(%10d)] : %10ld(%10ld)   ,   nnz=[%10d, %10d]:%10d\n", i, i_s, row_ptr[i_s], i_e, row_ptr[i_e], rows, nnz, j_s, j_e, j_e-j_s);
			}
		#endif
	}

	~CSRArrays()
	{
		free(a);
		free(row_ptr);
		free(ja);
		free(thread_i_s);
		free(thread_i_e);
		free(thread_j_s);
		free(thread_j_e);
		// free(thread_v_s);
		free(thread_v_e);

		#ifdef PRINT_STATISTICS
			free(thread_time_barrier);
			free(thread_time_compute);
		#endif
	}

	void spmv(ValueType * x, ValueType * y);
	void statistics_start();
	int statistics_print_data(__attribute__((unused)) char * buf, __attribute__((unused)) long buf_n);
};


void compute_csr_vector_sve(CSRArrays * restrict csr, ValueType * restrict x , ValueType * restrict y);
void compute_csr_vector_sve_perfect_nnz_balance(CSRArrays * restrict csr, ValueType * restrict x , ValueType * restrict y);


void
CSRArrays::spmv(ValueType * x, ValueType * y)
{
	num_loops++;
	#if defined(CUSTOM_SVE_VECTOR)
		compute_csr_vector_sve(this, x, y);
	#elif defined(CUSTOM_SVE_VECTOR_PERFECT_NNZ_BALANCE)
		compute_csr_vector_sve_perfect_nnz_balance(this, x, y);
	#endif
}


struct Matrix_Format *
csr_to_format(INT_T * row_ptr, INT_T * col_ind, ValueType * values, long m, long n, long nnz)
{
	struct CSRArrays * csr = new CSRArrays(row_ptr, col_ind, values, m, n, nnz);
	// for (long i=0;i<10;i++)
		// printf("%d\n", row_ptr[i]);
	csr->mem_footprint = nnz * (sizeof(ValueType) + sizeof(INT_T)) + (m+1) * sizeof(INT_T);
	#if defined(CUSTOM_SVE_VECTOR_PERFECT_NNZ_BALANCE)
		csr->format_name = (char *) "Custom_CSR_PBV_SVE";
	#elif defined(CUSTOM_SVE_VECTOR)
		csr->format_name = (char *) "Custom_CSR_BV_SVE";
	#endif
	return csr;
}


//==========================================================================================================================================
//= Subkernels Single Row CSR
//==========================================================================================================================================


__attribute__((hot,pure))
static inline
double
subkernel_row_csr_scalar(CSRArrays * restrict csr, ValueType * restrict x, long j_s, long j_e)
{
	ValueType sum;
	long j;
	sum = 0;
	for (j=j_s;j<j_e;j++)
	{
		sum += csr->a[j] * x[csr->ja[j]];
	}
	return sum;
}


__attribute__((hot,pure))
static inline
double
subkernel_row_csr_vector(CSRArrays * restrict csr, ValueType * restrict x, INT_T j_s, INT_T j_e)
{
	long j, k, j_rem, rows;
	Vector_Value_t zero = {0};
	__attribute__((unused)) Vector_Value_t v_a, v_x, v_mul, v_sum;
	ValueType sum = 0;

	rows = j_e - j_s;
	if (rows <= 0)
		return 0;
	sum = 0;
	j_rem = j_s + rows % VECTOR_ELEM_NUM;
	for (j=j_s;j<j_rem;j++)
		sum += csr->a[j] * x[csr->ja[j]];
	if (rows >= VECTOR_ELEM_NUM)
	{
		v_sum = zero;
		v_mul = zero;
		for (j=j_rem;j<j_e;j+=VECTOR_ELEM_NUM)
		{
			v_a = *(Vector_Value_t *) &csr->a[j];
			PRAGMA(GCC unroll VECTOR_ELEM_NUM)
			PRAGMA(GCC ivdep)
			for (k=0;k<VECTOR_ELEM_NUM;k++)
			{
				v_mul[k] = v_a[k] * x[csr->ja[j+k]];
			}
			v_sum += v_mul;
		}
		PRAGMA(GCC unroll VECTOR_ELEM_NUM)
		for (j=0;j<VECTOR_ELEM_NUM;j++)
			sum += v_sum[j];
	}
	return sum;
}




//==========================================================================================================================================
//= CSR Custom Vector GCC
//==========================================================================================================================================

void
compute_csr_vector_sve(CSRArrays * restrict csr, ValueType * restrict x, ValueType * restrict y)
{
	#pragma omp parallel
	{
		int tnum = omp_get_thread_num();
		long i_s, i_e;
		i_s = thread_i_s[tnum];
		i_e = thread_i_e[tnum];
		subkernel_csr_SVE_density(csr->row_ptr, csr->ja, csr->a, x, y, i_s, i_e);
	}
}


//==========================================================================================================================================
//= CSR Custom Perfect NNZ Balance
//==========================================================================================================================================

void
compute_csr_vector_sve_perfect_nnz_balance(CSRArrays * restrict csr, ValueType * restrict x, ValueType * restrict y)
{
	int num_threads = omp_get_max_threads();
	long t;
	#pragma omp parallel
	{
		int tnum = omp_get_thread_num();
		long i, i_s, i_e, j, j_s, j_e;

		i_s = thread_i_s[tnum];
		i_e = thread_i_e[tnum];

		if (i_e - 1 >= 0)
			y[i_e - 1] = 0;

		#pragma omp barrier

		ValueType sum;
		j_s = thread_j_s[tnum];

		j = j_s;
		for (i=i_s;i<i_e-1;i++)
		{
			j_e = csr->row_ptr[i+1];
			// sum = 0;
			// for (;j<j_e;j++)
			// {
				// sum += csr->a[j] * x[csr->ja[j]];
			// }
			// y[i] = sum;

			// y[i] = subkernel_row_csr_scalar(csr, x, j, j_e);
			// y[i] = subkernel_row_csr_vector_SVE_512d(csr->ja, csr->a, x, j, j_e);

			long degree = j_e - j;
			if (degree < 4)
			{
				y[i] = subkernel_row_csr_scalar(csr->ja, csr->a, x, j, j_e);
			}
			else
			{
				y[i] = subkernel_row_csr_vector_SVE(csr->ja, csr->a, x, j, j_e);
			}

			j = j_e;
		}

		i = i_e - 1;
		j =  csr->row_ptr[i];
		if (j_s > j)
			j = j_s;
		j_e = thread_j_e[tnum];
		sum = 0;
		for (;j<j_e;j++)
		{
			sum += csr->a[j] * x[csr->ja[j]];
		}
		thread_v_e[tnum] = sum;
	}
	for (t=0;t<num_threads;t++)
	{
		if (thread_i_e[t] - 1 < csr->m)
			y[thread_i_e[t] - 1] += thread_v_e[t];
	}
}


//==========================================================================================================================================
//= Print Statistics
//==========================================================================================================================================


void
CSRArrays::statistics_start()
{
	int num_threads = omp_get_max_threads();
	long i;
	num_loops = 0;
	for (i=0;i<num_threads;i++)
	{
		thread_time_compute[i] = 0;
		thread_time_barrier[i] = 0;
	}
}


int
statistics_print_labels(__attribute__((unused)) char * buf, __attribute__((unused)) long buf_n)
{
	return 0;
}


int
CSRArrays::statistics_print_data(__attribute__((unused)) char * buf, __attribute__((unused)) long buf_n)
{
	/* int num_threads = omp_get_max_threads();
	double iters_per_t[num_threads];
	double nnz_per_t[num_threads];
	__attribute__((unused)) double gflops_per_t[num_threads];
	double iters_per_t_min, iters_per_t_max, iters_per_t_avg, iters_per_t_std, iters_per_t_balance;
	double nnz_per_t_min, nnz_per_t_max, nnz_per_t_avg, nnz_per_t_std, nnz_per_t_balance;
	__attribute__((unused)) double time_per_t_min, time_per_t_max, time_per_t_avg, time_per_t_std, time_per_t_balance;
	__attribute__((unused)) double gflops_per_t_min, gflops_per_t_max, gflops_per_t_avg, gflops_per_t_std, gflops_per_t_balance;
	long i, i_s, i_e;

	for (i=0;i<num_threads;i++)
	{
		i_s = thread_i_s[i];
		i_e = thread_i_e[i];
		iters_per_t[i] = i_e - i_s;
		// nnz_per_t[i] = &(a[row_ptr[i_e]]) - &(a[row_ptr[i_s]]);
		nnz_per_t[i] = row_ptr[i_e] - row_ptr[i_s];
		gflops_per_t[i] = nnz_per_t[i] / thread_time_compute[i] * num_loops * 2 * 1e-9;   // Calculate before making nnz_per_t a ratio.
		iters_per_t[i] /= m;    // As a fraction of m.
		nnz_per_t[i] /= nnz;    // As a fraction of nnz.
	}

	array_min_max(iters_per_t, num_threads, &iters_per_t_min, NULL, &iters_per_t_max, NULL, val_to_double);
	array_mean(iters_per_t, num_threads, &iters_per_t_avg, val_to_double);
	array_std(iters_per_t, num_threads, &iters_per_t_std, val_to_double);
	iters_per_t_balance = iters_per_t_avg / iters_per_t_max;

	array_min_max(nnz_per_t, num_threads, &nnz_per_t_min, NULL, &nnz_per_t_max, NULL, val_to_double);
	array_mean(nnz_per_t, num_threads, &nnz_per_t_avg, val_to_double);
	array_std(nnz_per_t, num_threads, &nnz_per_t_std, val_to_double);
	nnz_per_t_balance = nnz_per_t_avg / nnz_per_t_max;

	array_min_max(thread_time_compute, num_threads, &time_per_t_min, NULL, &time_per_t_max, NULL, val_to_double);
	array_mean(thread_time_compute, num_threads, &time_per_t_avg, val_to_double);
	array_std(thread_time_compute, num_threads, &time_per_t_std, val_to_double);
	time_per_t_balance = time_per_t_avg / time_per_t_max;

	array_min_max(gflops_per_t, num_threads, &gflops_per_t_min, NULL, &gflops_per_t_max, NULL, val_to_double);
	array_mean(gflops_per_t, num_threads, &gflops_per_t_avg, val_to_double);
	array_std(gflops_per_t, num_threads, &gflops_per_t_std, val_to_double);
	gflops_per_t_balance = gflops_per_t_avg / gflops_per_t_max;

	printf("i:%g,%g,%g,%g,%g\n", iters_per_t_min, iters_per_t_max, iters_per_t_avg, iters_per_t_std, iters_per_t_balance);
	printf("nnz:%g,%g,%g,%g,%g\n", nnz_per_t_min, nnz_per_t_max, nnz_per_t_avg, nnz_per_t_std, nnz_per_t_balance);
	printf("time:%g,%g,%g,%g,%g\n", time_per_t_min, time_per_t_max, time_per_t_avg, time_per_t_std, time_per_t_balance);
	printf("gflops:%g,%g,%g,%g,%g\n", gflops_per_t_min, gflops_per_t_max, gflops_per_t_avg, gflops_per_t_std, gflops_per_t_balance);
	printf("tnum i_s i_e num_rows_frac nnz_frac\n");
	for (i=0;i<num_threads;i++)
	{
		i_s = thread_i_s[i];
		i_e = thread_i_e[i];
		printf("%ld %ld %ld %g %g\n", i, i_s, i_e, iters_per_t[i], nnz_per_t[i]);
	}
	printf("tnum gflops compute barrier total barrier/compute%%\n");
	for (i=0;i<num_threads;i++)
	{
		double time_compute, time_barrier, time_total, percent;
		time_compute = thread_time_compute[i];
		time_barrier = thread_time_barrier[i];
		time_total = time_compute + time_barrier;
		percent = time_barrier / time_compute * 100;
		printf("%ld %g %g %g %g %g\n", i, gflops_per_t[i], time_compute, time_barrier, time_total, percent);
	} */

	// i += snprintf(buf + i, buf_n - i, ",%lf", iters_per_t_avg);
	// i += snprintf(buf + i, buf_n - i, ",%lf", iters_per_t_std);
	// i += snprintf(buf + i, buf_n - i, ",%lf", iters_per_t_balance);
	// i += snprintf(buf + i, buf_n - i, ",%lf", nnz_per_t_avg);
	// i += snprintf(buf + i, buf_n - i, ",%lf", nnz_per_t_std);
	// i += snprintf(buf + i, buf_n - i, ",%lf", nnz_per_t_balance);
	// i += snprintf(buf + i, buf_n - i, ",%lf", time_per_t_avg);
	// i += snprintf(buf + i, buf_n - i, ",%lf", time_per_t_std);
	// i += snprintf(buf + i, buf_n - i, ",%lf", time_per_t_balance);
	// i += snprintf(buf + i, buf_n - i, ",%lf", gflops_per_t_avg);
	// i += snprintf(buf + i, buf_n - i, ",%lf", gflops_per_t_std);
	// i += snprintf(buf + i, buf_n - i, ",%lf", gflops_per_t_balance);
	return 0;
}

