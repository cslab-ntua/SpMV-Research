#ifndef VECTORIZATION_X86_AVX512_F64_H
#define VECTORIZATION_X86_AVX512_F64_H

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <x86intrin.h>

#include "macros/cpp_defines.h"
#include "macros/macrolib.h"

#include "vectorization/vectorization_util.h"


typedef struct [[gnu::packed]] { __m512d v[2]; }  __m512d_f64_2;
#define vec_f64_16_t  __m512d_f64_2
#define vec_f64_8_t   __m512d
#define vec_f64_4_t   __m256d
#define vec_f64_2_t   __m128d
#define vec_f64_1_t  double

#define vec_len_default_f64  8

typedef double  vec_f64_t [[gnu::may_alias]];


#define vec_array_f64_16(val)                    ({vec_f64_16_t * _tmp = &val; (vec_f64_t *) _tmp;})
#define vec_array_f64_8(val)                     ({vec_f64_8_t  * _tmp = &val; (vec_f64_t *) _tmp;})
#define vec_array_f64_4(val)                     ({vec_f64_4_t  * _tmp = &val; (vec_f64_t *) _tmp;})
#define vec_array_f64_2(val)                     ({vec_f64_2_t  * _tmp = &val; (vec_f64_t *) _tmp;})
#define vec_array_f64_1(val)                     ({vec_f64_1_t  * _tmp = &val; (vec_f64_t *) _tmp;})

#define vec_set1_f64_16(val)                     vec_loop_expr(__m512d_f64_2, 2, _tmp, _i, _tmp.v[_i] = vec_set1_f64_8(val);)
#define vec_set1_f64_8(val)                      _mm512_set1_pd(val)
#define vec_set1_f64_4(val)                      _mm256_set1_pd(val)
#define vec_set1_f64_2(val)                      _mm_set1_pd(val)
#define vec_set1_f64_1(val)                      val

#define vec_set_iter_f64_16(iter, expr)          vec_loop_expr(__m512d_f64_2, 2, _tmp, _i, _tmp.v[_i] = _mm512_set_pd(vec_iter_expr_8(iter, 8*_i, expr));)
#define vec_set_iter_f64_8(iter, expr)           _mm512_set_pd(vec_iter_expr_8(iter, 0, expr))
#define vec_set_iter_f64_4(iter, expr)           _mm256_set_pd(vec_iter_expr_4(iter, 0, expr))
#define vec_set_iter_f64_2(iter, expr)           _mm_set_pd(vec_iter_expr_2(iter, 0, expr))
#define vec_set_iter_f64_1(iter, expr)           vec_iter_expr_1(iter, 0, expr)


#define vec_loadu_f64_16(ptr)                    vec_loop_expr(__m512d_f64_2, 2, _tmp, _i, _tmp.v[_i] = vec_loadu_f64_8(((double*)(ptr)) + _i*8);)
#define vec_loadu_f64_8(ptr)                     _mm512_loadu_pd((double*)(ptr))
#define vec_loadu_f64_4(ptr)                     _mm256_loadu_pd((double*)(ptr))
#define vec_loadu_f64_2(ptr)                     _mm_loadu_pd((double*)(ptr))
#define vec_loadu_f64_1(ptr)                     (*((vec_f64_1_t*)(ptr)))

#define vec_storeu_f64_16(ptr, vec)              vec_loop_stmt(2, _i, vec_storeu_f64_8(((double*)(ptr)) + _i*8, (vec).v[_i]);)
#define vec_storeu_f64_8(ptr, vec)               _mm512_storeu_pd((double*)(ptr), vec)
#define vec_storeu_f64_4(ptr, vec)               _mm256_storeu_pd((double*)(ptr), vec)
#define vec_storeu_f64_2(ptr, vec)               _mm_storeu_pd((double*)(ptr), vec)
#define vec_storeu_f64_1(ptr, vec)               do { (*((vec_f64_1_t*)(ptr))) = (vec); } while (0)


#define vec_add_f64_16(a, b)                     vec_loop_expr(__m512d_f64_2, 2, _tmp, _i, _tmp.v[_i] = vec_add_f64_8(a.v[_i], b.v[_i]);)
#define vec_add_f64_8(a, b)                      _mm512_add_pd(a, b)
#define vec_add_f64_4(a, b)                      _mm256_add_pd(a, b)
#define vec_add_f64_2(a, b)                      _mm_add_pd(a, b)
#define vec_add_f64_1(a, b)                      (a + b)

#define vec_sub_f64_16(a, b)                     vec_loop_expr(__m512d_f64_2, 2, _tmp, _i, _tmp.v[_i] = vec_sub_f64_8(a.v[_i], b.v[_i]);)
#define vec_sub_f64_8(a, b)                      _mm512_sub_pd(a, b)
#define vec_sub_f64_4(a, b)                      _mm256_sub_pd(a, b)
#define vec_sub_f64_2(a, b)                      _mm_sub_pd(a, b)
#define vec_sub_f64_1(a, b)                      (a - b)

#define vec_mul_f64_16(a, b)                     vec_loop_expr(__m512d_f64_2, 2, _tmp, _i, _tmp.v[_i] = vec_mul_f64_8(a.v[_i], b.v[_i]);)
#define vec_mul_f64_8(a, b)                      _mm512_mul_pd(a, b)
#define vec_mul_f64_4(a, b)                      _mm256_mul_pd(a, b)
#define vec_mul_f64_2(a, b)                      _mm_mul_pd(a, b)
#define vec_mul_f64_1(a, b)                      (a * b)

#define vec_div_f64_16(a, b)                     vec_loop_expr(__m512d_f64_2, 2, _tmp, _i, _tmp.v[_i] = vec_div_f64_8(a.v[_i], b.v[_i]);)
#define vec_div_f64_8(a, b)                      _mm512_div_pd(a, b)
#define vec_div_f64_4(a, b)                      _mm256_div_pd(a, b)
#define vec_div_f64_2(a, b)                      _mm_div_pd(a, b)
#define vec_div_f64_1(a, b)                      (a / b)

#define vec_fmadd_f64_16(a, b, c)                vec_loop_expr(__m512d_f64_2, 2, _tmp, _i, _tmp.v[_i] = vec_fmadd_f64_8(a.v[_i], b.v[_i], c.v[_i]);)
#define vec_fmadd_f64_8(a, b, c)                 _mm512_fmadd_pd(a, b, c)
#define vec_fmadd_f64_4(a, b, c)                 _mm256_fmadd_pd(a, b, c)
#define vec_fmadd_f64_2(a, b, c)                 _mm_fmadd_pd(a, b, c)
#define vec_fmadd_f64_1(a, b, c)                 (a * b + c)


#define vec_reduce_add_f64_16(a)                 vec_reduce_expr(double, 2, 0, _tmp, _i, _tmp += vec_reduce_add_f64_8(a.v[_i]);)

#define vec_reduce_add_f64_8(a)                  _mm512_reduce_add_pd(a);

#define vec_reduce_add_f64_4(a)                                                                                                                                                                                                     \
({                                                                                                                                                                                                                                  \
	__m128d low_128d  = _mm256_castpd256_pd128(a);        /* Cast vector of type __m256d to type __m128d. This intrinsic is only used for compilation and does not generate any instructions, thus it has zero latency. */      \
	__m128d high_128d = _mm256_extractf128_pd(a, 1);      /* High 128: Extract 128 bits (composed of 2 packed double-precision (64-bit) floating-point elements) from a, selected with imm8, and store the result in dst. */    \
	low_128d  = _mm_add_pd(low_128d, high_128d);          /* Add low 128 and high 128. */                                                                                                                                       \
	__m128d high64 = _mm_unpackhi_pd(low_128d, low_128d); /* High 64: Unpack and interleave double-precision (64-bit) floating-point elements from the high half of a and b, and store the results in dst. */                   \
	_mm_cvtsd_f64(_mm_add_sd(low_128d, high64));          /* Reduce to scalar. */                                                                                                                                               \
})

#define vec_reduce_add_f64_2(a)                    \
({                                                 \
	__m128d high64 = _mm_unpackhi_pd(a, a);    \
	_mm_cvtsd_f64(_mm_add_sd(a, high64));      \
})

#define vec_reduce_add_f64_1(a)                  (a)


#endif /* VECTORIZATION_X86_AVX512_F64_H */

