#include <stdlib.h>
#include <stdio.h>
#include <omp.h>
#include <stdint.h>

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
#ifdef __cplusplus
}
#endif


#if DOUBLE == 0
	#define ValueType_cast_int  uint32_t
#elif DOUBLE == 1
	#define ValueType_cast_int  uint64_t
#endif


INT_T * thread_i_s = NULL;
INT_T * thread_i_e = NULL;

extern int prefetch_distance;


struct CSRArrays : Matrix_Format
{
	INT_T * row_ptr;      // the usual rowptr (of size m+1)
	INT_T * ja;      // the colidx of each NNZ (of size nnz)
	ValueType * a;   // the values (of size NNZ)

	ValueType * y_upper;

	long num_loops;

	CSRArrays(INT_T * row_ptr_in, INT_T * col_ind, ValueTypeReference * values, long m, long n, long nnz) : Matrix_Format(m, n, nnz)
	{
		int num_threads = omp_get_max_threads();
		double time_balance;

		row_ptr = (typeof(row_ptr)) aligned_alloc(64, (m+1) * sizeof(*row_ptr));
		ja = (typeof(ja)) aligned_alloc(64, nnz * sizeof(*ja));
		a = (typeof(a)) aligned_alloc(64, nnz * sizeof(*a));
		#pragma omp parallel for
		for (long i=0;i<m+1;i++)
			row_ptr[i] = row_ptr_in[i];
		#pragma omp parallel for
		for(long i=0;i<nnz;i++)
		{
			a[i]=values[i];
			ja[i]=col_ind[i];
		}

		thread_i_s = (INT_T *) malloc(num_threads * sizeof(*thread_i_s));
		thread_i_e = (INT_T *) malloc(num_threads * sizeof(*thread_i_e));
		time_balance = time_it(1,
			_Pragma("omp parallel")
			{
				int tnum = omp_get_thread_num();
				loop_partitioner_balance_prefix_sums(num_threads, tnum, row_ptr, m, nnz, &thread_i_s[tnum], &thread_i_e[tnum]);
			}
		);
		printf("balance time = %g\n", time_balance);
		y_upper = (typeof(y_upper)) malloc(m * sizeof(*y_upper));
	}

	~CSRArrays()
	{
		free(a);
		free(row_ptr);
		free(ja);
		free(thread_i_s);
		free(thread_i_e);
		free(y_upper);
	}

	void spmv(ValueType * x, ValueType * y);
	void statistics_start();
	int statistics_print_data(__attribute__((unused)) char * buf, __attribute__((unused)) long buf_n);
};


void compute_csr(CSRArrays * restrict csr, ValueType * restrict x , ValueType * restrict y);


void
CSRArrays::spmv(ValueType * x, ValueType * y)
{
	num_loops++;
	compute_csr(this, x, y);
}


struct Matrix_Format *
csr_to_format(INT_T * row_ptr, INT_T * col_ind, ValueTypeReference * values, long m, long n, long nnz, long symmetric, long symmetry_expanded)
{
	if (!symmetric || symmetry_expanded)
		error("only symmetric un-expanded matrices are supported by this format");
	struct CSRArrays * csr = new CSRArrays(row_ptr, col_ind, values, m, n, nnz);
	printf("n=%ld m=%ld nnz=%ld\n", n, m, nnz);
	csr->mem_footprint = nnz * (sizeof(ValueType) + sizeof(INT_T)) + (m+1) * sizeof(INT_T);
	csr->format_name = (char *) "CSR_SYM_CPU";
	return csr;
}


//==========================================================================================================================================
//= Subkernels CSR
//==========================================================================================================================================


void
subkernel_csr_sym(CSRArrays * restrict csr, ValueType * restrict x, ValueType * restrict y, long i_s, long i_e)
{
	union {
		ValueType f;
		#if DOUBLE == 0
			uint32_t u;
		#elif DOUBLE == 1
			uint64_t u;
		#endif
	} y_buf, sum;
	ValueType prod;
	long i, j, j_e, col;
	j = csr->row_ptr[i_s];
	for (i=i_s;i<i_e;i++)
	{
		j_e = csr->row_ptr[i+1];
		sum.f = 0;
		for (;j<j_e;j++)
		{
			col = csr->ja[j];
			// y[i] += csr->a[j] * x[col];
			// if (i != col)
				// y[col] += csr->a[j] * x[i];
			#pragma omp atomic
			y[i] += csr->a[j] * x[col];
			if (i != col)
			{
				#pragma omp atomic
				y[col] += csr->a[j] * x[i];
			}
			// prod = csr->a[j] * x[col];
			// while (1)
			// {
				// y_buf.u = __atomic_load_n((ValueType_cast_int *) &y[i], __ATOMIC_RELAXED);
				// sum.f = y_buf.f + prod;
				// if (__atomic_compare_exchange_n((ValueType_cast_int *) &y[i], &y_buf.u, sum.u, 0, __ATOMIC_RELAXED, __ATOMIC_RELAXED))
					// break;
			// }
			// if (i != col)
			// {
				// prod = csr->a[j] * x[i];
				// while (1)
				// {
					// y_buf.u = __atomic_load_n((ValueType_cast_int *) &y[col], __ATOMIC_RELAXED);
					// sum.f = y_buf.f + prod;
					// if (__atomic_compare_exchange_n((ValueType_cast_int *) &y[col], &y_buf.u, sum.u, 0, __ATOMIC_RELAXED, __ATOMIC_RELAXED))
						// break;
				// }
			// }
		}
	}
}


void
subkernel_csr_sym_split(CSRArrays * restrict csr, ValueType * restrict x, ValueType * restrict y, ValueType * restrict y_upper, long i_s, long i_e)
{
	union {
		ValueType f;
		#if DOUBLE == 0
			uint32_t u;
		#elif DOUBLE == 1
			uint64_t u;
		#endif
	} y_buf, sum;
	ValueType prod, sum_upper;
	long i, j, j_e, col;
	j = csr->row_ptr[i_s];
	for (i=i_s;i<i_e;i++)
	{
		j_e = csr->row_ptr[i+1];
		sum_upper = 0;
		for (;j<j_e;j++)
		{
			col = csr->ja[j];
			sum_upper += csr->a[j] * x[col];
			if (i != col)
			{
				prod = csr->a[j] * x[i];
				if (col >= i_s && col < i_e)
					y_upper[col] += prod;
				else
				{
					while (1)
					{
						y_buf.u = __atomic_load_n((ValueType_cast_int *) &y[col], __ATOMIC_RELAXED);
						sum.f = y_buf.f + prod;
						if (__atomic_compare_exchange_n((ValueType_cast_int *) &y[col], &y_buf.u, sum.u, 0, __ATOMIC_RELAXED, __ATOMIC_RELAXED))
							break;
					}
					// #pragma omp atomic
					// y[col] += prod;
				}
			}
		}
		y_upper[i] = sum_upper;
	}
}


//==========================================================================================================================================
//= CSR Custom
//==========================================================================================================================================


void
compute_csr(CSRArrays * restrict csr, ValueType * restrict x, ValueType * restrict y)
{
	#pragma omp parallel
	{
		int tnum = omp_get_thread_num();
		long i, i_s, i_e;
		#pragma omp for
		for (i=0;i<csr->m;i++)
		{
			y[i] = 0;
			csr->y_upper[i] = 0;
		}
		i_s = thread_i_s[tnum];
		i_e = thread_i_e[tnum];
		// subkernel_csr_sym(csr, x, y, i_s, i_e);
		subkernel_csr_sym_split(csr, x, y, csr->y_upper, i_s, i_e);
		#pragma omp barrier
		#pragma omp for
		for (i=0;i<csr->m;i++)
			y[i] += csr->y_upper[i];
	}
}


//==========================================================================================================================================
//= Print Statistics
//==========================================================================================================================================


void
CSRArrays::statistics_start()
{
	// int num_threads = omp_get_max_threads();
	// long i;
	// num_loops = 0;
	// for (i=0;i<num_threads;i++)
	// {
	// }
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

