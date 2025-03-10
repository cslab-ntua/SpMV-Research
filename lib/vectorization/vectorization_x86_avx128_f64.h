#ifndef VECTORIZATION_X86_AVX128_F64_H
#define VECTORIZATION_X86_AVX128_F64_H

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <x86intrin.h>

#include "macros/cpp_defines.h"
#include "macros/macrolib.h"

#include "vectorization/vectorization_util.h"


typedef struct [[gnu::packed]] { __m128d v[4]; }  __m128d_f64_4;
#define vec_f64_8_t   __m128d_f64_4
typedef struct [[gnu::packed]] { __m128d v[2]; }  __m128d_f64_2;
#define vec_f64_4_t   __m128d_f64_2
#define vec_f64_2_t  __m128d
#define vec_f64_1_t  double

#define vec_len_default_f64  2

typedef double vec_f64_t [[gnu::may_alias]];


#define vec_array_f64_8(val)                     ({vec_f64_8_t  * _tmp = &val; (vec_f64_t *) _tmp;})
#define vec_array_f64_4(val)                     ({vec_f64_4_t  * _tmp = &val; (vec_f64_t *) _tmp;})
#define vec_array_f64_2(val)                     ({vec_f64_2_t  * _tmp = &val; (vec_f64_t *) _tmp;})
#define vec_array_f64_1(val)                     ({vec_f64_1_t  * _tmp = &val; (vec_f64_t *) _tmp;})

#define vec_set1_f64_8(val)                      vec_loop_expr(__m128d_f64_4, 4, _tmp, _i, _tmp.v[_i] = vec_set1_f64_2(val);)
#define vec_set1_f64_4(val)                      vec_loop_expr(__m128d_f64_2, 2, _tmp, _i, _tmp.v[_i] = vec_set1_f64_2(val);)
#define vec_set1_f64_2(val)                      _mm_set1_pd(val)
#define vec_set1_f64_1(val)                      val

#define vec_set_iter_f64_8(iter, expr)           vec_loop_expr(__m128d_f64_4, 4, _tmp, _i, _tmp.v[_i] = _mm_set_pd(vec_iter_expr_2(iter, 2*_i, expr));)
#define vec_set_iter_f64_4(iter, expr)           vec_loop_expr(__m128d_f64_2, 2, _tmp, _i, _tmp.v[_i] = _mm_set_pd(vec_iter_expr_2(iter, 2*_i, expr));)
#define vec_set_iter_f64_2(iter, expr)           _mm_set_pd(vec_iter_expr_2(iter, 0, expr))
#define vec_set_iter_f64_1(iter, expr)           vec_iter_expr_1(iter, 0, expr)


#define vec_loadu_f64_8(ptr)                     vec_loop_expr(__m128d_f64_4, 4, _tmp, _i, _tmp.v[_i] = vec_loadu_f64_2(((double*)(ptr)) + _i*2);)
#define vec_loadu_f64_4(ptr)                     vec_loop_expr(__m128d_f64_2, 2, _tmp, _i, _tmp.v[_i] = vec_loadu_f64_2(((double*)(ptr)) + _i*2);)
#define vec_loadu_f64_2(ptr)                     _mm_loadu_pd((double*)(ptr))
#define vec_loadu_f64_1(ptr)                     (*((vec_f64_1_t*)(ptr)))

#define vec_storeu_f64_8(ptr, vec)               vec_loop_stmt(4, _i, vec_storeu_f64_2(((double*)(ptr)) + _i*2, (vec).v[_i]);)
#define vec_storeu_f64_4(ptr, vec)               vec_loop_stmt(2, _i, vec_storeu_f64_2(((double*)(ptr)) + _i*2, (vec).v[_i]);)
#define vec_storeu_f64_2(ptr, vec)               _mm_storeu_pd((double*)(ptr), vec)
#define vec_storeu_f64_1(ptr, vec)               do { (*((vec_f64_1_t*)(ptr))) = (vec); } while (0)


#define vec_add_f64_8(a, b)                      vec_loop_expr(__m128d_f64_4, 4, _tmp, _i, _tmp.v[_i] = vec_add_f64_2(a.v[_i], b.v[_i]);)
#define vec_add_f64_4(a, b)                      vec_loop_expr(__m128d_f64_2, 2, _tmp, _i, _tmp.v[_i] = vec_add_f64_2(a.v[_i], b.v[_i]);)
#define vec_add_f64_2(a, b)                      _mm_add_pd(a, b)
#define vec_add_f64_1(a, b)                      (a + b)

#define vec_sub_f64_8(a, b)                      vec_loop_expr(__m128d_f64_4, 4, _tmp, _i, _tmp.v[_i] = vec_sub_f64_2(a.v[_i], b.v[_i]);)
#define vec_sub_f64_4(a, b)                      vec_loop_expr(__m128d_f64_2, 2, _tmp, _i, _tmp.v[_i] = vec_sub_f64_2(a.v[_i], b.v[_i]);)
#define vec_sub_f64_2(a, b)                      _mm_sub_pd(a, b)
#define vec_sub_f64_1(a, b)                      (a - b)

#define vec_mul_f64_8(a, b)                      vec_loop_expr(__m128d_f64_4, 4, _tmp, _i, _tmp.v[_i] = vec_mul_f64_2(a.v[_i], b.v[_i]);)
#define vec_mul_f64_4(a, b)                      vec_loop_expr(__m128d_f64_2, 2, _tmp, _i, _tmp.v[_i] = vec_mul_f64_2(a.v[_i], b.v[_i]);)
#define vec_mul_f64_2(a, b)                      _mm_mul_pd(a, b)
#define vec_mul_f64_1(a, b)                      (a * b)

#define vec_div_f64_8(a, b)                      vec_loop_expr(__m128d_f64_4, 4, _tmp, _i, _tmp.v[_i] = vec_div_f64_2(a.v[_i], b.v[_i]);)
#define vec_div_f64_4(a, b)                      vec_loop_expr(__m128d_f64_2, 2, _tmp, _i, _tmp.v[_i] = vec_div_f64_2(a.v[_i], b.v[_i]);)
#define vec_div_f64_2(a, b)                      _mm_div_pd(a, b)
#define vec_div_f64_1(a, b)                      (a / b)

#define vec_fmadd_f64_8(a, b, c)                 vec_loop_expr(__m128d_f64_4, 4, _tmp, _i, _tmp.v[_i] = vec_fmadd_f64_2(a.v[_i], b.v[_i], c.v[_i]);)
#define vec_fmadd_f64_4(a, b, c)                 vec_loop_expr(__m128d_f64_2, 2, _tmp, _i, _tmp.v[_i] = vec_fmadd_f64_2(a.v[_i], b.v[_i], c.v[_i]);)
#define vec_fmadd_f64_2(a, b, c)                 _mm_fmadd_pd(a, b, c)
#define vec_fmadd_f64_1(a, b, c)                 (a * b + c)


#define vec_reduce_add_f64_8(a)                  vec_reduce_expr(double, 4, 0, _tmp, _i, _tmp += vec_reduce_add_f64_2(a.v[_i]);)
#define vec_reduce_add_f64_4(a)                  vec_reduce_expr(double, 2, 0, _tmp, _i, _tmp += vec_reduce_add_f64_2(a.v[_i]);)

#define vec_reduce_add_f64_2(a)                                      \
({                                                                   \
	const __m128d low_64   = a;                                  \
	const __m128d high_64  = _mm_unpackhi_pd(low_64, low_64);    \
	const __m128d x64      = _mm_add_sd(low_64, high_64);        \
	_mm_cvtsd_f64(x64);                                          \
})

#define vec_reduce_add_f64_1(a)                  (a)


#endif /* VECTORIZATION_X86_AVX128_F64_H */

