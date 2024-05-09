#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

#include "macros/cpp_defines.h"

#include "spmv_bench_common.h"
#include "spmv_kernel.h"

#include <x86intrin.h>
// #include <immintrin.h>

#ifdef __cplusplus
extern "C"{
#endif
	#include "macros/macrolib.h"
	#include "time_it.h"
	#include "time_it_tsc.h"
	#include "parallel_util.h"
	#include "x86_util.h"
#ifdef __cplusplus
}
#endif


//==========================================================================================================================================
//= Subkernels Single Row CSR x86
//==========================================================================================================================================


__attribute__((hot,pure))
static inline
double
subkernel_row_csr_scalar(INT_T * restrict ja, ValueType * restrict a, ValueType * restrict x, long j_s, long j_e)
{
	ValueType sum;
	long j;
	sum = 0;
	for (j=j_s;j<j_e;j++)
	{
		sum += a[j] * x[ja[j]];
	}
	return sum;
}


__attribute__((hot,pure))
static inline
ValueType
subkernel_row_csr_scalar_kahan(INT_T * restrict ja, ValueType * restrict a, ValueType * restrict x, INT_T j_s, INT_T j_e)
{
	ValueType sum, val, tmp, compensation = 0;
	long j;
	sum = 0;
	for (j=j_s;j<j_e;j++)
	{
		val = a[j] * x[ja[j]] - compensation;
		tmp = sum + val;
		compensation = (tmp - sum) - val;
		sum = tmp;
	}
	return sum;
}


__attribute__((hot,pure))
static inline
double
__subkernel_row_csr_vector_x86_128d(INT_T * restrict ja, ValueType * restrict a, ValueType * restrict x, long j_s, long j_e)
{
	long j, j_e_vector;
	const long mask = ~(((long) 2) - 1); // Minimum number of elements for the vectorized code (power of 2).
	__m128d v_a, v_x, v_sum;
	ValueType sum = 0, sum_v = 0;
	v_sum = _mm_setzero_pd();
	sum = 0;
	sum_v = 0;
	j_e_vector = j_s + ((j_e - j_s) & mask);
	if (j_s != j_e_vector)
	{
		for (j=j_s;j<j_e_vector;j+=2)
		{
			v_a = _mm_loadu_pd(&a[j]);
			v_x = _mm_set_pd(x[ja[j]], x[ja[j+1]]);
			v_sum = _mm_fmadd_pd(v_a, v_x, v_sum);
		}
		sum_v = hsum128_pd(v_sum);
	}
	for (j=j_e_vector;j<j_e;j++)
		sum += a[j] * x[ja[j]];
	return sum + sum_v;
}


__attribute__((hot,pure))
static inline
double
__subkernel_row_csr_vector_x86_256d(INT_T * restrict ja, ValueType * restrict a, ValueType * restrict x, long j_s, long j_e)
{
	long j, j_e_vector;
	const long mask = ~(((long) 4) - 1); // Minimum number of elements for the vectorized code (power of 2).
	__m256d v_a, v_x, v_sum;
	ValueType sum = 0, sum_v = 0;
	v_sum = _mm256_setzero_pd();
	sum = 0;
	sum_v = 0;
	j_e_vector = j_s + ((j_e - j_s) & mask);
	if (j_s != j_e_vector)
	{
		for (j=j_s;j<j_e_vector;j+=4)
		{
			v_a = _mm256_loadu_pd(&a[j]);
			v_x = _mm256_set_pd(x[ja[j]], x[ja[j+1]], x[ja[j+2]], x[ja[j+3]]);
			v_sum = _mm256_fmadd_pd(v_a, v_x, v_sum);
		}
		sum_v = hsum256_pd(v_sum);
	}
	for (j=j_e_vector;j<j_e;j++)
		sum += a[j] * x[ja[j]];
	return sum + sum_v;
}


__attribute__((hot,pure))
static inline
double
__subkernel_row_csr_vector_x86_512d(INT_T * restrict ja, ValueType * restrict a, ValueType * restrict x, long j_s, long j_e)
{
	long j, j_e_vector;
	const long mask = ~(((long) 8) - 1); // Minimum number of elements for the vectorized code (power of 2).
	__m512d v_a, v_x, v_sum;
	ValueType sum = 0, sum_v = 0;
	v_sum = _mm512_setzero_pd();
	sum = 0;
	sum_v = 0;
	j_e_vector = j_s + ((j_e - j_s) & mask);
	if (j_s != j_e_vector)
	{
		for (j=j_s;j<j_e_vector;j+=8)
		{
			v_a = _mm512_loadu_pd(&a[j]);
			v_x = _mm512_set_pd(x[ja[j]], x[ja[j+1]], x[ja[j+2]], x[ja[j+3]], x[ja[j+4]], x[ja[j+5]], x[ja[j+6]], x[ja[j+7]]);
			v_sum = _mm512_fmadd_pd(v_a, v_x, v_sum);
		}
		sum_v = hsum512_pd(v_sum);
	}
	for (j=j_e_vector;j<j_e;j++)
		sum += a[j] * x[ja[j]];
	return sum + sum_v;
}


__attribute__((hot,pure))
static inline
double
subkernel_row_csr_vector_x86_128d(INT_T * restrict ja, ValueType * restrict a, ValueType * restrict x, long j_s, long j_e)
{
	#if defined(__AVX__)
		return __subkernel_row_csr_vector_x86_128d(ja, a, x, j_s, j_e);
	#else
		return subkernel_row_csr_scalar(ja, a, x, j_s, j_e);
	#endif
}


__attribute__((hot,pure))
static inline
double
subkernel_row_csr_vector_x86_256d(INT_T * restrict ja, ValueType * restrict a, ValueType * restrict x, long j_s, long j_e)
{
	#if defined(__AVX2__)
		return __subkernel_row_csr_vector_x86_256d(ja, a, x, j_s, j_e);
	#else
		return subkernel_row_csr_vector_x86_128d(ja, a, x, j_s, j_e);
	#endif
}


__attribute__((hot,pure))
static inline
double
subkernel_row_csr_vector_x86_512d(INT_T * restrict ja, ValueType * restrict a, ValueType * restrict x, long j_s, long j_e)
{
	#if defined(__AVX512F__)
		return __subkernel_row_csr_vector_x86_512d(ja, a, x, j_s, j_e);
	#else
		return subkernel_row_csr_vector_x86_256d(ja, a, x, j_s, j_e);
	#endif
}


__attribute__((hot,pure))
static inline
double
subkernel_row_csr_vector_x86(INT_T * restrict ja, ValueType * restrict a, ValueType * restrict x, long j_s, long j_e)
{
	return subkernel_row_csr_vector_x86_512d(ja, a, x, j_s, j_e);
}


//==========================================================================================================================================
//= Subkernels CSR x86
//==========================================================================================================================================


void
subkernel_csr_scalar(INT_T * restrict row_ptr, INT_T * restrict ja, ValueType * restrict a, ValueType * restrict x, ValueType * restrict y, long i_s, long i_e)
{
	ValueType sum;
	long i, j, j_e;
	j = row_ptr[i_s];
	for (i=i_s;i<i_e;i++)
	{
		j_e = row_ptr[i+1];
		sum = 0;
		for (;j<j_e;j++)
			sum += a[j] * x[ja[j]];
		y[i] = sum;
	}
}


void
subkernel_csr_vector_x86_128d(INT_T * restrict row_ptr, INT_T * restrict ja, ValueType * restrict a, ValueType * restrict x, ValueType * restrict y, long i_s, long i_e)
{
	long i, j_s, j_e;
	j_e = row_ptr[i_s];
	for (i=i_s;i<i_e;i++)
	{
		y[i] = 0;
		j_s = j_e;
		j_e = row_ptr[i+1];
		if (j_s == j_e)
			continue;
		y[i] = subkernel_row_csr_vector_x86_128d(ja, a, x, j_s, j_e);
	}
}


void
subkernel_csr_vector_x86_256d(INT_T * restrict row_ptr, INT_T * restrict ja, ValueType * restrict a, ValueType * restrict x, ValueType * restrict y, long i_s, long i_e)
{
	long i, j_s, j_e;
	j_e = row_ptr[i_s];
	for (i=i_s;i<i_e;i++)
	{
		y[i] = 0;
		j_s = j_e;
		j_e = row_ptr[i+1];
		if (j_s == j_e)
			continue;
		y[i] = subkernel_row_csr_vector_x86_256d(ja, a, x, j_s, j_e);
	}
}


void
subkernel_csr_vector_x86_512d(INT_T * restrict row_ptr, INT_T * restrict ja, ValueType * restrict a, ValueType * restrict x, ValueType * restrict y, long i_s, long i_e)
{
	long i, j_s, j_e;
	j_e = row_ptr[i_s];
	for (i=i_s;i<i_e;i++)
	{
		y[i] = 0;
		j_s = j_e;
		j_e = row_ptr[i+1];
		if (j_s == j_e)
			continue;
		y[i] = subkernel_row_csr_vector_x86_512d(ja, a, x, j_s, j_e);
	}
}


//==========================================================================================================================================
//= Subkernel CSR x86 Density
//==========================================================================================================================================


void
subkernel_csr_x86_density(INT_T * restrict row_ptr, INT_T * restrict ja, ValueType * restrict a, ValueType * restrict x, ValueType * restrict y, long i_s, long i_e)
{
	double density;
	if (i_s >= i_e)
		return;
	density = ((double) row_ptr[i_e] - row_ptr[i_s]) / (i_e - i_s);
	if (density < 4)
	{
		// printf("%d: scalar %lf\n", tnum, density);
		subkernel_csr_scalar(row_ptr, ja, a, x, y, i_s, i_e);
	}
	else if (density < 8)
	{
		// printf("%d: 128 %lf\n", tnum, density);
		subkernel_csr_vector_x86_128d(row_ptr, ja, a, x, y, i_s, i_e);
	}
	else if (density < 16)
	{
		// printf("%d: 256 %lf\n", tnum, density);
		subkernel_csr_vector_x86_256d(row_ptr, ja, a, x, y, i_s, i_e);
	}
	else
	{
		// printf("%d: 512 %lf\n", tnum, density);
		subkernel_csr_vector_x86_512d(row_ptr, ja, a, x, y, i_s, i_e);
	}
}

