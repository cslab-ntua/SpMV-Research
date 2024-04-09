#ifndef VECTORIZATION_H
#define VECTORIZATION_H

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <x86intrin.h>

#include "macros/cpp_defines.h"
#include "macros/macrolib.h"


/* AVX512 is slow. */
#ifdef __AVX512F__
	// #define VEC_AVX512F
	#define VEC_AVX2
#endif

#ifdef __AVX2__
	#define VEC_AVX2
#endif

#ifdef __AVX__
	#define VEC_AVX
#endif

// #undef VEC_AVX512F
// #undef VEC_AVX2
// #undef VEC_AVX

// #define VEC_AVX512F
// #define VEC_AVX2
// #define VEC_AVX


#if defined(VEC_AVX512F)
	#define vec_d_t  __m512d
	#define vec_i_t  __m512i
	#define vec_len_pd     8
	#define vec_len_epi64  8
#elif defined(VEC_AVX2)
	#define vec_d_t  __m256d
	#define vec_i_t  __m256i
	#define vec_len_pd     4
	#define vec_len_epi64  4
#elif defined(VEC_AVX)
	#define vec_d_t  __m128d
	#define vec_i_t  __m128i
	#define vec_len_pd     2
	#define vec_len_epi64  2
#else
	// #define vec_d_t  __m128d
	// #define vec_i_t  __m128i
	// #define vec_len_pd     2
	// #define vec_len_epi64  2
	#define vec_d_t  __m256d
	#define vec_i_t  __m256i
	#define vec_len_pd     4
	#define vec_len_epi64  4
#endif


#if defined(VEC_AVX512F)
	#define vec_set1_pd(val)                 _mm512_set1_pd(val)
	#define vec_set_iter_pd(iter, expr)      _mm512_set_pd(({long iter=7; expr;}), ({long iter=6; expr;}), ({long iter=5; expr;}), ({long iter=4; expr;}), ({long iter=3; expr;}), ({long iter=2; expr;}), ({long iter=1; expr;}), ({long iter=0; expr;}))
	#define vec_set_epi64(...)               _mm512_set_epi64(__VA_ARGS__)
	#define vec_set1_epi64(val)              _mm512_set1_epi64(val)
	#define vec_set_iter_epi64(iter, expr)   _mm512_set_epi64(({long iter=7; expr;}), ({long iter=6; expr;}), ({long iter=5; expr;}), ({long iter=4; expr;}), ({long iter=3; expr;}), ({long iter=2; expr;}), ({long iter=1; expr;}), ({long iter=0; expr;}))
#elif defined(VEC_AVX2)
	#define vec_set1_pd(val)                 _mm256_set1_pd(val)
	#define vec_set_iter_pd(iter, expr)      _mm256_set_pd(({long iter=3; expr;}), ({long iter=2; expr;}), ({long iter=1; expr;}), ({long iter=0; expr;}))
	#define vec_set_epi64(...)               _mm256_set_epi64x(__VA_ARGS__)
	#define vec_set1_epi64(val)              _mm256_set1_epi64x(val)
	#define vec_set_iter_epi64(iter, expr)   _mm256_set_epi64x(({long iter=3; expr;}), ({long iter=2; expr;}), ({long iter=1; expr;}), ({long iter=0; expr;}))
#elif defined(VEC_AVX)
	#define vec_set1_pd(val)                 _mm_set1_pd(val)
	#define vec_set_iter_pd(iter, expr)      _mm_set_pd(({long iter=1; expr;}), ({long iter=0; expr;}))
	#define vec_set_epi64(...)               _mm_set_epi64x(__VA_ARGS__)
	#define vec_set1_epi64(val)              _mm_set1_epi64x(val)
	#define vec_set_iter_epi64(iter, expr)   _mm_set_epi64x(({long iter=1; expr;}), ({long iter=0; expr;}))
#else
	#define vec_set1_pd(val)                 ({vec_d_t ret; for (long __i=0;__i<vec_len_pd;__i++) ret[__i] = val; ret;})
	#define vec_set_iter_pd(iter, expr)      ({vec_d_t ret; for (long iter=0;iter<vec_len_pd;iter++) ret[iter] = expr; ret;})
	#define vec_set1_epi64(val)              ({vec_i_t ret; for (long __i=0;__i<vec_len_pd;__i++) ret[__i] = val; ret;})
	#define vec_set_iter_epi64(iter, expr)   ({vec_i_t ret; for (long iter=0;iter<vec_len_pd;iter++) ret[iter] = expr; ret;})
#endif


#if defined(VEC_AVX512F)
	#define vec_loadu_pd(ptr)        _mm512_loadu_pd(ptr)
	#define vec_storeu_pd(ptr, vec)  _mm512_storeu_pd(ptr, vec)
#elif defined(VEC_AVX2)
	#define vec_loadu_pd(ptr)        _mm256_loadu_pd(ptr)
	#define vec_storeu_pd(ptr, vec)  _mm256_storeu_pd(ptr, vec)
#elif defined(VEC_AVX)
	#define vec_loadu_pd(ptr)        _mm_loadu_pd(ptr)
	#define vec_storeu_pd(ptr, vec)  _mm_storeu_pd(ptr, vec)
#else
	#define vec_loadu_pd(ptr)        ({vec_d_t ret; for (long __i=0;__i<vec_len_pd;__i++) ret[__i] = (ptr)[__i]; ret;})
	#define vec_storeu_pd(ptr, vec)  do {for (long __i=0;__i<vec_len_pd;__i++) (ptr)[__i] = vec[__i];} while (0)
#endif


#if defined(VEC_AVX512F)
	#define vec_add_pd(a, b)       _mm512_add_pd(a, b)
	#define vec_sub_pd(a, b)       _mm512_sub_pd(a, b)
	#define vec_mul_pd(a, b)       _mm512_mul_pd(a, b)
	#define vec_fmadd_pd(a, b, c)  _mm512_fmadd_pd(a, b, c)
	#define vec_add_epi64(a, b)    _mm512_add_epi64(a, b)
	#define vec_sub_epi64(a, b)    _mm512_sub_epi64(a, b)
	#define vec_mullo_epi64(a, b)  _mm512_mullo_epi64(a, b)
#elif defined(VEC_AVX2)
	#define vec_add_pd(a, b)       _mm256_add_pd(a, b)
	#define vec_sub_pd(a, b)       _mm256_sub_pd(a, b)
	#define vec_mul_pd(a, b)       _mm256_mul_pd(a, b)
	#define vec_fmadd_pd(a, b, c)  _mm256_fmadd_pd(a, b, c)
	#define vec_add_epi64(a, b)    _mm256_add_epi64(a, b)
	#define vec_sub_epi64(a, b)    _mm256_sub_epi64(a, b)
#elif defined(VEC_AVX)
	#define vec_add_pd(a, b)       _mm_add_pd(a, b)
	#define vec_sub_pd(a, b)       _mm_sub_pd(a, b)
	#define vec_mul_pd(a, b)       _mm_mul_pd(a, b)
	#define vec_fmadd_pd(a, b, c)  _mm_fmadd_pd(a, b, c)
	#define vec_add_epi64(a, b)    _mm_add_epi64(a, b)
	#define vec_sub_epi64(a, b)    _mm_sub_epi64(a, b)
#else
	#define vec_add_pd(a, b)       ({vec_d_t ret; for (long __i=0;__i<vec_len_pd;__i++) ret[__i] = a[__i] + b[__i]; ret;})
	#define vec_sub_pd(a, b)       ({vec_d_t ret; for (long __i=0;__i<vec_len_pd;__i++) ret[__i] = a[__i] - b[__i]; ret;})
	#define vec_mul_pd(a, b)       ({vec_d_t ret; for (long __i=0;__i<vec_len_pd;__i++) ret[__i] = a[__i] * b[__i]; ret;})
	#define vec_fmadd_pd(a, b, c)  ({vec_d_t ret; for (long __i=0;__i<vec_len_pd;__i++) ret[__i] = a[__i] * b[__i] + c[__i]; ret;})
	#define vec_add_epi64(a, b)    ({vec_i_t ret; for (long __i=0;__i<vec_len_pd;__i++) ret[__i] = a[__i] + b[__i]; ret;})
	#define vec_sub_epi64(a, b)    ({vec_i_t ret; for (long __i=0;__i<vec_len_pd;__i++) ret[__i] = a[__i] - b[__i]; ret;})
#endif


#if defined(VEC_AVX512F)
	#define vec_and_si(a, b)          _mm512_and_si512(a, b)
	#define vec_slli_epi32(a, imm8)   _mm512_slli_epi32(a, imm8)
	#define vec_slli_epi64(a, imm8)   _mm512_slli_epi64(a, imm8)
	#define vec_srli_epi32(a, imm8)   _mm512_srli_epi32(a, imm8)
	#define vec_srli_epi64(a, imm8)   _mm512_srli_epi64(a, imm8)

	#define vec_sllv_epi32(a, count)  _mm512_sllv_epi32(a, count)
	#define vec_sllv_epi64(a, count)  _mm512_sllv_epi64(a, count)
	#define vec_srlv_epi32(a, count)  _mm512_srlv_epi32(a, count)
	#define vec_srlv_epi64(a, count)  _mm512_srlv_epi64(a, count)
#elif defined(VEC_AVX2)
	#define vec_and_si(a, b)          _mm256_and_si256(a, b)
	#define vec_slli_epi32(a, imm8)   _mm256_slli_epi32(a, imm8)
	#define vec_slli_epi64(a, imm8)   _mm256_slli_epi64(a, imm8)
	#define vec_srli_epi32(a, imm8)   _mm256_srli_epi32(a, imm8)
	#define vec_srli_epi64(a, imm8)   _mm256_srli_epi64(a, imm8)

	#define vec_sllv_epi32(a, count)  _mm256_sllv_epi32(a, count)
	#define vec_sllv_epi64(a, count)  _mm256_sllv_epi64(a, count)
	#define vec_srlv_epi32(a, count)  _mm256_srlv_epi32(a, count)
	#define vec_srlv_epi64(a, count)  _mm256_srlv_epi64(a, count)
#elif defined(VEC_AVX)
	#define vec_and_si(a, b)          _mm_and_si128(a, b)
	#define vec_slli_epi32(a, imm8)   _mm_slli_epi32(a, imm8)
	#define vec_slli_epi64(a, imm8)   _mm_slli_epi64(a, imm8)
	#define vec_srli_epi32(a, imm8)   _mm_srli_epi32(a, imm8)
	#define vec_srli_epi64(a, imm8)   _mm_srli_epi64(a, imm8)

	#define vec_sllv_epi32(a, count)  _mm_set_epi32x((count[1] < 32) ? ((uint32_t) a[1])<<count[1] : 0, (count[0] < 32) ? ((uint32_t) a[0])<<count[0] : 0)
	#define vec_sllv_epi64(a, count)  _mm_set_epi64x((count[1] < 64) ? ((uint64_t) a[1])<<count[1] : 0, (count[0] < 64) ? ((uint64_t) a[0])<<count[0] : 0)
	#define vec_srlv_epi32(a, count)  _mm_set_epi32x((count[1] < 32) ? ((uint32_t) a[1])>>count[1] : 0, (count[0] < 32) ? ((uint32_t) a[0])>>count[0] : 0)
	#define vec_srlv_epi64(a, count)  _mm_set_epi64x((count[1] < 64) ? ((uint64_t) a[1])>>count[1] : 0, (count[0] < 64) ? ((uint64_t) a[0])>>count[0] : 0)
#else
	#define vec_and_si(a, b)          ({vec_i_t ret; for (long __i=0;__i<vec_len_pd;__i++) ret[__i] = a[__i] & b[__i]; ret;})
	#define vec_slli_epi64(a, imm8)   ({vec_i_t ret; for (long __i=0;__i<vec_len_pd;__i++) ret[__i] = (imm8 < 64) ? a[__i] << imm8 : 0; ret;})
	#define vec_srli_epi64(a, imm8)   ({vec_i_t ret; for (long __i=0;__i<vec_len_pd;__i++) ret[__i] = (imm8 < 64) ? a[__i] >> imm8 : 0; ret;})

	#define vec_sllv_epi64(a, count)  ({vec_i_t ret; for (long __i=0;__i<vec_len_pd;__i++) ret[__i] = (count[__i] < 64) ? a[__i] << count[__i] : 0; ret;})
	#define vec_srlv_epi64(a, count)  ({vec_i_t ret; for (long __i=0;__i<vec_len_pd;__i++) ret[__i] = (count[__i] < 64) ? a[__i] >> count[__i] : 0; ret;})
#endif


#endif /* VECTORIZATION_H */

