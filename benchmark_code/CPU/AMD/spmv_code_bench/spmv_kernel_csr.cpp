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
#ifdef __cplusplus
}
#endif


extern INT_T * thread_i_s;
extern INT_T * thread_i_e;

extern INT_T * thread_j_s;
extern INT_T * thread_j_e;

extern ValueType * thread_v_s;
extern ValueType * thread_v_e;

extern int prefetch_distance;


struct CSRArrays : Matrix_Format
{
	INT_T * ia;      // the usual rowptr (of size m+1)
	INT_T * ja;      // the colidx of each NNZ (of size nnz)
	ValueType * a;   // the values (of size NNZ)

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
						#ifdef CUSTOM_VECTOR_PERFECT_NNZ_BALANCE
							long lower_boundary;
							// long higher_boundary;
							loop_partitioner_balance_iterations(num_threads, tnum, 0, nnz, &thread_j_s[tnum], &thread_j_e[tnum]);
							binary_search(ia, 0, m, thread_j_s[tnum], &lower_boundary, NULL);           // Index boundaries are inclusive.
							thread_i_s[tnum] = lower_boundary;
							// binary_search(ia, 0, m, thread_j_e[tnum] - 1, NULL, &higher_boundary);     // Index boundaries are inclusive.
							// thread_i_e[tnum] = higher_boundary;
							_Pragma("omp barrier")
							if (tnum == num_threads - 1)   // If we calculate each thread's boundaries individually some empty rows might be unassigned.
								thread_i_e[tnum] = m;
							else
								thread_i_e[tnum] = thread_i_s[tnum+1] + 1;
							// _Pragma("omp single")
							// {
								// this->ia = (INT_T *) aligned_alloc(64, (m+1 + VECTOR_ELEM_NUM) * sizeof(INT_T));
							// }
							// _Pragma("omp barrier")
							// for (long i=thread_i_s[tnum];i<thread_i_e[tnum];i++)
								// this->ia[i] = ia[i];
							// if (tnum == num_threads - 1)
								// this->ia[m] = ia[m];
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
										printf("%3d:  i=[%7d,%7d]  |  j=[%7d,%7d] (%7d)  ,  ia[i]=[%7d,%7d] (%7d)  ,  ia[i+1]=[%7d,%7d]\n",
											t,
											i_s, i_e,
											j_s, j_e, (j_e - j_s),
											ia[i_s], ia[i_e], (ia[i_e] - ia[i_s]),
											ia[i_s+1], ia[i_e+1]
										);
									}
								}
							#endif
						#else
							loop_partitioner_balance_partial_sums(num_threads, tnum, ia, m, nnz, &thread_i_s[tnum], &thread_i_e[tnum]);
							// loop_partitioner_balance(num_threads, tnum, 2, ia, m, nnz, &thread_i_s[tnum], &thread_i_e[tnum]);
						#endif
					}
				#endif
			}
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
};


void compute_csr(CSRArrays * restrict csr, ValueType * restrict x , ValueType * restrict y);
void compute_csr_kahan(CSRArrays * restrict csr, ValueType * restrict x, ValueType * restrict y);
void compute_csr_prefetch(CSRArrays * restrict csr, ValueType * restrict x , ValueType * restrict y);
void compute_csr_omp_simd(CSRArrays * restrict csr, ValueType * restrict x , ValueType * restrict y);
void compute_csr_vector(CSRArrays * restrict csr, ValueType * restrict x , ValueType * restrict y);
void compute_csr_vector_perfect_nnz_balance(CSRArrays * restrict csr, ValueType * restrict x , ValueType * restrict y);


void
CSRArrays::spmv(ValueType * x, ValueType * y)
{
	#if defined(CUSTOM_PREFETCH)
		compute_csr_prefetch(this, x, y);
	#elif defined(CUSTOM_SIMD)
		compute_csr_omp_simd(this, x, y);
	#elif defined(CUSTOM_VECTOR)
		compute_csr_vector(this, x, y);
	#elif defined(CUSTOM_VECTOR_PERFECT_NNZ_BALANCE)
		compute_csr_vector_perfect_nnz_balance(this, x, y);
	#elif defined(CUSTOM_KAHAN)
		compute_csr_kahan(this, x, y);
	#else
		compute_csr(this, x, y);
	#endif
}


struct Matrix_Format *
csr_to_format(INT_T * row_ptr, INT_T * col_ind, ValueType * values, long m, long n, long nnz)
{
	struct CSRArrays * csr = new CSRArrays(row_ptr, col_ind, values, m, n, nnz);
	// for (long i=0;i<10;i++)
		// printf("%d\n", row_ptr[i]);
	csr->mem_footprint = nnz * (sizeof(ValueType) + sizeof(INT_T)) + (m+1) * sizeof(INT_T);
	#if defined(NAIVE)
		csr->format_name = (char *) "Naive_CSR_CPU";
	#elif defined(CUSTOM_VECTOR_PERFECT_NNZ_BALANCE)
		csr->format_name = (char *) "Custom_CSR_PBV";
	#elif defined(CUSTOM_VECTOR)
		csr->format_name = (char *) "Custom_CSR_BV";
	#else
		csr->format_name = (char *) "Custom_CSR_B";
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


#ifndef __XLC__
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
#endif /* __XLC__ */


//==========================================================================================================================================
//= Subkernels CSR
//==========================================================================================================================================


// void
// subkernel_csr_scalar(CSRArrays * restrict csr, ValueType * restrict x, ValueType * restrict y, long i_s, long i_e)
// {
	// ValueType sum;
	// long i, j, j_s, j_e;
	// j_e = csr->ia[i_s];
	// for (i=i_s;i<i_e;i++)
	// {
		// y[i] = 0;
		// j_s = j_e;
		// j_e = csr->ia[i+1];
		// if (j_s == j_e)
			// continue;
		// sum = 0;
		// for (j=j_s;j<j_e;j++)
		// {
			// sum += csr->a[j] * x[csr->ja[j]];
		// }
		// y[i] = sum;
	// }
// }


void
subkernel_csr_scalar(CSRArrays * restrict csr, ValueType * restrict x, ValueType * restrict y, long i_s, long i_e)
{
	ValueType sum;
	long i, j, j_e;
	j = csr->ia[i_s];
	for (i=i_s;i<i_e;i++)
	{
		j_e = csr->ia[i+1];
		sum = 0;
		for (;j<j_e;j++)
		{
			sum += csr->a[j] * x[csr->ja[j]];
		}
		y[i] = sum;
	}
}


void
subkernel_csr_scalar_kahan(CSRArrays * restrict csr, ValueType * restrict x, ValueType * restrict y, long i_s, long i_e)
{
	ValueType sum, val, tmp, compensation = 0;
	long i, j, j_e;
	j = csr->ia[i_s];
	for (i=i_s;i<i_e;i++)
	{
		j_e = csr->ia[i+1];
		sum = 0;
		compensation = 0;
		for (;j<j_e;j++)
		{
			val = csr->a[j] * x[csr->ja[j]] - compensation;
			tmp = sum + val;
			compensation = (tmp - sum) - val;
			sum = tmp;
		}
		y[i] = sum;
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
		long i_s, i_e;
		i_s = thread_i_s[tnum];
		i_e = thread_i_e[tnum];
		#ifdef TIME_BARRIER
		double time;
		time = time_it(1,
		#endif
			subkernel_csr_scalar(csr, x, y, i_s, i_e);
		#ifdef TIME_BARRIER
		);
		thread_time_compute[tnum] += time;
		time = time_it(1,
			_Pragma("omp barrier")
		);
		thread_time_barrier[tnum] += time;
		#endif
	}
}


//==========================================================================================================================================
//= CSR Kahan
//==========================================================================================================================================


void
compute_csr_kahan(CSRArrays * restrict csr, ValueType * restrict x, ValueType * restrict y)
{
	#pragma omp parallel
	{
		int tnum = omp_get_thread_num();
		long i_s, i_e;
		i_s = thread_i_s[tnum];
		i_e = thread_i_e[tnum];
		subkernel_csr_scalar_kahan(csr, x, y, i_s, i_e);
	}
}


//==========================================================================================================================================
//= CSR Custom Vector Omp Prefetch
//==========================================================================================================================================


// prefetch distance for wikipedia-20051105.mtx on ryzen 3700x is optimized at 64 (!) with locality=3, for about +14% gflops.

void
compute_csr_prefetch(CSRArrays * restrict csr, ValueType * restrict x, ValueType * restrict y)
{
	#pragma omp parallel
	{
		int tnum = omp_get_thread_num();
		ValueType sum;
		long i, i_s, i_e, j, j_s, j_e;
		i_s = thread_i_s[tnum];
		i_e = thread_i_e[tnum];
		for (i=i_s;i<i_e;i++)
		{
			j_s = csr->ia[i];
			j_e = csr->ia[i+1];
			if (j_s == j_e)
				continue;
			sum = 0;
			for (j=j_s;j<j_e;j++)
			{
				__builtin_prefetch(&csr->ja[j + prefetch_distance], 0, 3);
				__builtin_prefetch(&x[csr->ja[j + 2*prefetch_distance]], 0, 3);
				sum += csr->a[j] * x[csr->ja[j]];
			}
			y[i] = sum;
		}
	}
}


//==========================================================================================================================================
//= CSR Custom Vector Omp Simd
//==========================================================================================================================================


void
compute_csr_omp_simd(CSRArrays * restrict csr, ValueType * restrict x, ValueType * restrict y)
{
	#pragma omp parallel
	{
		int tnum = omp_get_thread_num();
		ValueType sum;
		long i, i_s, i_e, j, j_s, j_e;
		i_s = thread_i_s[tnum];
		i_e = thread_i_e[tnum];
		for (i=i_s;i<i_e;i++)
		{
			j_s = csr->ia[i];
			j_e = csr->ia[i+1];
			if (j_s == j_e)
				continue;
			sum = 0;
			#pragma omp simd reduction(+:sum)
			for (j=j_s;j<j_e;j++)
				sum += csr->a[j] * x[csr->ja[j]];
			y[i] = sum;
		}
	}
}


#ifndef __XLC__

//==========================================================================================================================================
//= CSR Custom Vector GCC
//==========================================================================================================================================


/* void compute_csr_vector2(CSRArrays * csr, ValueType * x , ValueType * y)
{
	#pragma omp parallel
	{
		int tnum = omp_get_thread_num();
		long i, i_s, i_e, j, j_s, j_e, k, j_e_vector;
		const long mask = ~(((long) VECTOR_ELEM_NUM) - 1);      // VECTOR_ELEM_NUM is a power of 2.
		Vector_Value_t zero = {0};
		__attribute__((unused)) Vector_Value_t v_a, v_x = zero, v_mul = zero, v_sum = zero;
		__attribute__((unused)) ValueType sum = 0;
		i_s = thread_i_s[tnum];
		i_e = thread_i_e[tnum];
		for (i=i_s;i<i_e;i++)
		{
			v_sum = zero;
			y[i] = 0;
			j_s = csr->ia[i];
			j_e = csr->ia[i+1];
			if (j_s == j_e)
				continue;
			v_a = *(Vector_Value_t *) &csr->a[0];
			j = j_s;
			j_e_vector = j_s + ((j_e - j_s) & mask);
			for (j=j_s;j<j_e_vector;j+=VECTOR_ELEM_NUM)
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
			for (;j<j_e;j++)
				v_sum[0] += csr->a[j] * x[csr->ja[j]];
			PRAGMA(GCC unroll VECTOR_ELEM_NUM)
			for (j=1;j<VECTOR_ELEM_NUM;j++)
				v_sum[0] += v_sum[j];
			y[i] = v_sum[0];
		}
	}
} */


void
compute_csr_vector(CSRArrays * restrict csr, ValueType * restrict x, ValueType * restrict y)
{
	#pragma omp parallel
	{
		int tnum = omp_get_thread_num();
		long i, i_s, i_e, j, j_s, j_e, k, j_e_vector;
		const long mask = ~(((long) VECTOR_ELEM_NUM) - 1);      // VECTOR_ELEM_NUM is a power of 2.
		Vector_Value_t zero = {0};
		__attribute__((unused)) Vector_Value_t v_a, v_x = zero, v_mul = zero, v_sum = zero;
		ValueType sum = 0, sum_v = 0;
		i_s = thread_i_s[tnum];
		i_e = thread_i_e[tnum];
		for (i=i_s;i<i_e;i++)
		{
			y[i] = 0;
			j_s = csr->ia[i];
			j_e = csr->ia[i+1];
			if (j_s == j_e)
				continue;
			v_sum = zero;
			sum = 0;
			sum_v = 0;
			j_e_vector = j_s + ((j_e - j_s) & mask);
			if (j_s != j_e_vector)
			{
				for (j=j_s;j<j_e_vector;j+=VECTOR_ELEM_NUM)
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
				for (k=0;k<VECTOR_ELEM_NUM;k++)
					sum_v += v_sum[k];
			}
			for (j=j_e_vector;j<j_e;j++)
				sum += csr->a[j] * x[csr->ja[j]];
			y[i] = sum + sum_v;
		}
	}
}


#endif /* __XLC__ */


//==========================================================================================================================================
//= CSR Custom Perfect NNZ Balance
//==========================================================================================================================================


/* __attribute__((hot,pure))
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
		sum_v = subkernel_row_csr_vector(csr, x, j_s, j_e_vector);

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
	// return compute_csr_line_case(csr, x, j_s, j_e);
	return subkernel_row_csr_vector(csr, x, j_s, j_e);
} */


void
compute_csr_vector_perfect_nnz_balance(CSRArrays * restrict csr, ValueType * restrict x, ValueType * restrict y)
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
			thread_v_s[tnum] = subkernel_row_csr_scalar(csr, x, j_s, j_e);
		}

		subkernel_csr_scalar(csr, x, y, i_s+1, i_e-1);

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
				thread_v_e[tnum] = subkernel_row_csr_scalar(csr, x, j_s, j_e);
		}
	}
	for (t=0;t<num_threads;t++)
	{
		y[thread_i_s[t]] += thread_v_s[t];
		if (thread_i_e[t] - 1 > thread_i_s[t])
			y[thread_i_e[t] - 1] += thread_v_e[t];
	}
}

