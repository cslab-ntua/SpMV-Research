#ifndef X86_UTIL_H
#define X86_UTIL_H

#include <x86intrin.h>

#include "macros/cpp_defines.h"


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//------------------------------------------------------------------------------------------------------------------------------------------
//-                                                            Vectorization                                                               -
//------------------------------------------------------------------------------------------------------------------------------------------
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


//==========================================================================================================================================
//= Horizontal Sums
//==========================================================================================================================================


// Reduce add 2 double-precision numbers.
__attribute__((const))
static inline
double
hsum128_pd(__m128d v_128d)
{
	__m128d high64 = _mm_unpackhi_pd(v_128d, v_128d);
	return  _mm_cvtsd_f64(_mm_add_sd(v_128d, high64));
}


// Reduce add 4 double-precision numbers.
__attribute__((const))
static inline
double
hsum256_pd(__m256d v_256d)
{
	// double sum;
	// __m256d hsum = _mm256_add_pd(v_256d, _mm256_permute2f128_pd(v_256d, v_256d, 0x1));
	// _mm_store_sd(&sum, _mm_hadd_pd( _mm256_castpd256_pd128(hsum), _mm256_castpd256_pd128(hsum) ) );
	// return sum;

	// __m256d temp = _mm256_hadd_pd(v_256d, v_256d);
	// return ((double*)&temp)[0] + ((double*)&temp)[2];

	// __m256d temp = _mm256_hadd_pd(v_256d, v_256d);
	// __m128d sum_high = _mm256_extractf128_pd(temp, 1);
	// __m128d result = _mm_add_pd(sum_high, _mm256_castpd256_pd128(temp));
	// return ((double*)&result)[0];

	__m128d low_128d  = _mm256_castpd256_pd128(v_256d);   // Cast vector of type __m256d to type __m128d. This intrinsic is only used for compilation and does not generate any instructions, thus it has zero latency.
	__m128d high_128d = _mm256_extractf128_pd(v_256d, 1); // High 128: Extract 128 bits (composed of 2 packed double-precision (64-bit) floating-point elements) from a, selected with imm8, and store the result in dst.
	low_128d  = _mm_add_pd(low_128d, high_128d);          // Add low 128 and high 128.
	__m128d high64 = _mm_unpackhi_pd(low_128d, low_128d); // High 64: Unpack and interleave double-precision (64-bit) floating-point elements from the high half of a and b, and store the results in dst.
	return  _mm_cvtsd_f64(_mm_add_sd(low_128d, high64));  // Reduce to scalar.
}


// Reduce add 8 double-precision numbers.
__attribute__((const))
static inline
double
hsum512_pd(__m512d v_512d)
{
	// __m256d low  = _mm512_castpd512_pd256(v_512d);
	// __m256d high = _mm512_extractf64x4_pd(v_512d, 1);
	// low  = _mm256_add_pd(low, high);                  // Add low 256 and high 256.
	// return hsum256_pd(low);
	return _mm512_reduce_add_pd(v_512d);
}


/*
	__m256i start256i, stop256i, mask256i;
	__m128i v_colind;
	v_colind = _mm_loadu_si128((__m128i const*)&csr->ja[j]);
	v_x = _mm256_set_pd(x[_mm_extract_epi32(v_colind,0)], x[_mm_extract_epi32(v_colind,1)], x[_mm_extract_epi32(v_colind,2)], x[_mm_extract_epi32(v_colind,3)]);
						
	v_sum2 = _mm256_setzero_pd();
	for (j=j_e_vector,k=0;j<j_e;j++,k++)
		v_sum_2[k] = csr->a[j] * x[csr->ja[j]];
	__m256d temp = _mm256_hadd_pd(v_sum, v_sum_2);
	__m128d sum_high = _mm256_extractf128_pd(temp, 1);
	__m128d result = _mm_add_pd(sum_high, _mm256_castpd256_pd128(temp));
	y[i] = hsum256_pd(v_sum);

	x256d[0] = x[csr->ja[j_e_vector]];
	x256d[1] = x[csr->ja[j_e_vector+1]];
	x256d[2] = x[csr->ja[j_e_vector+2]];
	x256d[3] = 0;
	v_x = _mm256_load_pd(x256d);
	start256i = _mm256_set1_epi64x(j_e_vector);
	stop256i = _mm256_set1_epi64x(j_e);
	start256i = _mm256_add_epi64(start256i, _mm256_set_epi64x(0, 1, 2, 3));
	mask256i = _mm256_cmpgt_epi64(stop256i, start256i);
	v_a = _mm256_maskload_pd(&csr->a[j], mask256i);
	v_sum = _mm256_fmadd_pd(v_a, v_x, v_sum);
	y[i] = hsum256_pd(v_sum);
*/


#endif /* X86_UTIL_H */

