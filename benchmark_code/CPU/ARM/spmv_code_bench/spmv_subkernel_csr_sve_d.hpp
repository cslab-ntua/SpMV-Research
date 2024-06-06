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
	#include "time_it_tsc.h"
	#include "parallel_util.h"
#ifdef __cplusplus
}
#endif


//==========================================================================================================================================
//= Subkernels Single Row CSR SVE
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
__subkernel_row_csr_vector_SVE(INT_T * restrict ja, ValueType * restrict a, ValueType * restrict x, long j_s, long j_e)
{
	long j, j_e_vector;
	// const long mask = ~(((long) 2) - 1); // Minimum number of elements for the vectorized code (power of 2).
	const long vector_length = svcntd();  // Get the number of double elements in an SVE vector
	svfloat64_t v_a, v_sum, v_x; // __m128d v_a, v_x, v_sum;
	ValueType sum = 0, sum_v = 0;
    v_sum = svdup_f64(0.0); // _mm_setzero_pd();
	// j_e_vector = j_s + ((j_e - j_s) & mask);
	j_e_vector = j_s + ((j_e - j_s) & ~(vector_length - 1));  // Align to vector length
	if (j_s != j_e_vector)
	{
		for (j=j_s;j<j_e_vector;j+=vector_length)
		{
			svbool_t pg = svwhilelt_b64_s32(j, j_e_vector); // Active predicate for SVE
			v_a = svld1_f64(pg, &a[j]); // _mm_loadu_pd(&a[j]);
			svint64_t indices = svld1(pg, &ja[j]);
			v_x = svld1_gather_index(pg, x, indices); // _mm_set_pd(x[ja[j]], x[ja[j+1]]);
			v_sum = svmla_f64_m(pg, v_sum, v_a, v_x); // _mm_fmadd_pd(v_a, v_x, v_sum);
		}
		sum_v = svaddv_f64(svptrue_b64(), v_sum);  // hsum128_pd(v_sum);
	}
	for (j=j_e_vector;j<j_e;j++)
		sum += a[j] * x[ja[j]];
	return sum + sum_v;
}



__attribute__((hot,pure))
static inline
double
subkernel_row_csr_vector_SVE(INT_T * restrict ja, ValueType * restrict a, ValueType * restrict x, long j_s, long j_e)
{
	// For the Grace-Hopper CPU, only 128-bit vector operations are possible. 
	// It is controlled through the svcntd() function which returns the number of double elements in an SVE vector.
	// In the future, I have to create custom for 128, 256, 512 bits operations.
	#if defined(__ARM_FEATURE_SVE)
		return __subkernel_row_csr_vector_SVE(ja, a, x, j_s, j_e);
	#else
		return subkernel_row_csr_scalar(ja, a, x, j_s, j_e);
	#endif
}



//==========================================================================================================================================
//= Subkernels CSR SVE
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
subkernel_csr_vector_SVE(INT_T * restrict row_ptr, INT_T * restrict ja, ValueType * restrict a, ValueType * restrict x, ValueType * restrict y, long i_s, long i_e)
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
		y[i] = subkernel_row_csr_vector_SVE(ja, a, x, j_s, j_e);
	}
}

//==========================================================================================================================================
//= Subkernel CSR SVE Density
//==========================================================================================================================================


void
subkernel_csr_SVE_density(INT_T * restrict row_ptr, INT_T * restrict ja, ValueType * restrict a, ValueType * restrict x, ValueType * restrict y, long i_s, long i_e)
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
	else
	{
		subkernel_csr_vector_SVE(row_ptr, ja, a, x, y, i_s, i_e);
	}
}

