#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

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


INT_T * thread_i_s = NULL;
INT_T * thread_i_e = NULL;

INT_T * thread_j_s = NULL;
INT_T * thread_j_e = NULL;

extern int prefetch_distance;

struct COOArrays : Matrix_Format
{
	INT_T * ia;      // the rowidx of each NNZ (of size NNZ)
	INT_T * ja;      // the colidx of each NNZ (of size NNZ)
	ValueType * a;   // the values (of size NNZ)
	char * row_change; // will be 1 when the last element of the row is reached
	ValueType * tmp_res;

	long num_loops;

	COOArrays(INT_T * row_ptr, INT_T * ja, ValueType * a, long m, long n, long nnz) : Matrix_Format(m, n, nnz), ja(ja), a(a)
	{
		int num_threads = omp_get_max_threads();
		

		ia = (typeof(ia)) malloc(nnz * sizeof(*ia));
		tmp_res = (typeof(tmp_res)) malloc(nnz * sizeof(*tmp_res));
		row_change = (typeof(row_change)) calloc(nnz, sizeof(*row_change));

		#pragma omp parallel
		{
			long i, j, j_s, j_e;
			#pragma omp for
			for (i=0;i<m;i++)
			{
				j_s = row_ptr[i];
				j_e = row_ptr[i+1];
				for (j=j_s;j<j_e;j++){
					ia[j] = i;
				}
				row_change[j_e-1] = 1;
			}
		}

		double time_balance;
		thread_i_s = (INT_T *) malloc(num_threads * sizeof(*thread_i_s));
		thread_i_e = (INT_T *) malloc(num_threads * sizeof(*thread_i_e));
		thread_j_s = (INT_T *) malloc(num_threads * sizeof(*thread_j_s));
		thread_j_e = (INT_T *) malloc(num_threads * sizeof(*thread_j_e));
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
						loop_partitioner_balance_prefix_sums(num_threads, tnum, row_ptr, m, nnz, &thread_i_s[tnum], &thread_i_e[tnum]);
					}
				#endif
				thread_j_s[tnum] = row_ptr[thread_i_s[tnum]];
				thread_j_e[tnum] = row_ptr[thread_i_e[tnum]];
			}
		);
		printf("balance time = %g\n", time_balance);

		#ifdef PRINT_STATISTICS
			long i;
			num_loops = 0;
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
				printf("%10ld: rows=[%10d %10d) = %d rows, nnz = %d (thread_j = (%d - %d)\n", i, i_s, i_e,  rows, nnz, j_s, j_e);
			}
		#endif
	}

	~COOArrays()
	{
		free(a);
		free(row_change);
		free(ia);
		free(ja);
		free(thread_i_s);
		free(thread_i_e);
		free(thread_j_s);
		free(thread_j_e);
	}

	void spmv(ValueType * x, ValueType * y);
	void statistics_start();
	int statistics_print_data(__attribute__((unused)) char * buf, __attribute__((unused)) long buf_n);
};


void compute_coo(COOArrays * restrict coo, ValueType * restrict x , ValueType * restrict y);
void compute_coo_kahan(COOArrays * restrict coo, ValueType * restrict x, ValueType * restrict y);
void compute_coo_prefetch(COOArrays * restrict coo, ValueType * restrict x , ValueType * restrict y);
void compute_coo_omp_simd(COOArrays * restrict coo, ValueType * restrict x , ValueType * restrict y);
void compute_coo_vector_perfect_nnz_balance(COOArrays * restrict coo, ValueType * restrict x , ValueType * restrict y);


void
COOArrays::spmv(ValueType * x, ValueType * y)
{
	num_loops++;
	#if defined(CUSTOM_PREFETCH)
		compute_coo_prefetch(this, x, y); // TODO
	#elif defined(CUSTOM_SIMD)
		compute_coo_omp_simd(this, x, y); // TODO
	#elif defined(CUSTOM_VECTOR)
		compute_coo_vector(this, x, y); // TODO
	#elif defined(CUSTOM_VECTOR_PERFECT_NNZ_BALANCE)
		compute_coo_vector_perfect_nnz_balance(this, x, y); // TODO
	#elif defined(CUSTOM_KAHAN)
		compute_coo_kahan(this, x, y);
	#else
		compute_coo(this, x, y);
	#endif
}


struct Matrix_Format *
csr_to_format(INT_T * row_ptr, INT_T * col_ind, ValueType * values, long m, long n, long nnz)
{
	struct COOArrays * coo = new COOArrays(row_ptr, col_ind, values, m, n, nnz);
	// for (long i=0;i<10;i++)
		// printf("%d\n", row_ptr[i]);
	coo->mem_footprint = nnz * (sizeof(ValueType) + sizeof(INT_T) + sizeof(INT_T));
	#if defined(NAIVE)
		coo->format_name = (char *) "Naive_COO_CPU";
	#elif defined(CUSTOM_VECTOR_PERFECT_NNZ_BALANCE)
		coo->format_name = (char *) "Custom_COO_PBV";
	#elif defined(CUSTOM_VECTOR)
		coo->format_name = (char *) "Custom_COO_BV";
	#else
		coo->format_name = (char *) "Custom_COO_B";
	#endif

	return coo;
}


//==========================================================================================================================================
//= Subkernels COO
//==========================================================================================================================================


void
subkernel_coo_scalar_slow(COOArrays * restrict coo, ValueType * restrict x, ValueType * restrict y, long j_s, long j_e)
{
	long j;
	for(j = j_s; j < j_e; j++)
	{
		y[coo->ia[j]] += coo->a[j] * x[coo->ja[j]];
	}
}

void
subkernel_coo_scalar(COOArrays * restrict coo, ValueType * restrict x, ValueType * restrict y, long j_s, long j_e)
{
	ValueType sum = 0;
	long j;

	for(j = j_s; j < j_e; j++)
	{
		sum += coo->a[j] * x[coo->ja[j]];
		if(coo->row_change[j])
		{
			long i = coo->ia[j];
			y[i] = sum;
			sum = 0;
		}
	}
}

void
subkernel_coo_scalar_kahan(COOArrays * restrict coo, ValueType * restrict x, ValueType * restrict y, long j_s, long j_e)
{
	ValueType sum = 0, val, tmp, compensation = 0;
	long j;

	for(j = j_s; j < j_e; j++)
	{
		val = coo->a[j] * x[coo->ja[j]] - compensation;
		tmp = sum + val;
		compensation = (tmp - sum) - val;
		sum = tmp;

		if(coo->row_change[j])
		{
			long i = coo->ia[j];
			y[i] = sum;
			sum = 0;
			compensation = 0;
		}
	}
}


//==========================================================================================================================================
//= COO Custom
//==========================================================================================================================================


void
compute_coo(COOArrays * restrict coo, ValueType * restrict x, ValueType * restrict y)
{
	#pragma omp parallel
	{
		int tnum = omp_get_thread_num();
		long j, i_e, j_s, j_e;
		j_s = thread_j_s[tnum];
		j_e = thread_j_e[tnum];
		// subkernel_coo_scalar_slow(coo, x, y, j_s, j_e);
		subkernel_coo_scalar(coo, x, y, j_s, j_e);
	}
}

//==========================================================================================================================================
//= COO Kahan
//==========================================================================================================================================


void
compute_coo_kahan(COOArrays * restrict coo, ValueType * restrict x, ValueType * restrict y)
{
	#pragma omp parallel
	{
		int tnum = omp_get_thread_num();
		long j, i_e, j_s, j_e;
		j_s = thread_j_s[tnum];
		j_e = thread_j_e[tnum];
		subkernel_coo_scalar_kahan(coo, x, y, j_s, j_e);
	}
}


//==========================================================================================================================================
//= COO Custom Vector Omp Prefetch
//==========================================================================================================================================


// prefetch distance for wikipedia-20051105.mtx on ryzen 3700x is optimized at 64 (!) with locality=3, for about +14% gflops.

void
compute_coo_prefetch(COOArrays * restrict coo, ValueType * restrict x, ValueType * restrict y)
{

}


//==========================================================================================================================================
//= COO Custom Vector Omp Simd
//==========================================================================================================================================


void
compute_coo_omp_simd(COOArrays * restrict coo, ValueType * restrict x, ValueType * restrict y)
{
	
}


//==========================================================================================================================================
//= COO Custom Perfect NNZ Balance
//==========================================================================================================================================


void
compute_coo_vector_perfect_nnz_balance(COOArrays * restrict coo, ValueType * restrict x, ValueType * restrict y)
{

}

//==========================================================================================================================================
//= Print Statistics
//==========================================================================================================================================


void
COOArrays::statistics_start()
{

}


int
statistics_print_labels(__attribute__((unused)) char * buf, __attribute__((unused)) long buf_n)
{
	return 0;
}


int
COOArrays::statistics_print_data(__attribute__((unused)) char * buf, __attribute__((unused)) long buf_n)
{
	return 0;
}

