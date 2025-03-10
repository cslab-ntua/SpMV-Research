#ifndef VECTORIZATION_X86_AVX256_F32_H
#define VECTORIZATION_X86_AVX256_F32_H

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <x86intrin.h>

#include "macros/cpp_defines.h"
#include "macros/macrolib.h"

#include "vectorization/vectorization_util.h"


typedef struct [[gnu::packed]] { __m256 v[2]; }  __m256_f32_2;
#define vec_f32_16_t  __m256_f32_2
#define vec_f32_8_t   __m256
#define vec_f32_4_t   __m128
#define vec_f32_1_t   float

#define vec_len_default_f32  8

typedef float vec_f32_t [[gnu::may_alias]];


#define vec_array_f32_16(val)                    ({vec_f32_16_t * _tmp = &val; (vec_f32_t *) _tmp;})
#define vec_array_f32_8(val)                     ({vec_f32_8_t  * _tmp = &val; (vec_f32_t *) _tmp;})
#define vec_array_f32_4(val)                     ({vec_f32_4_t  * _tmp = &val; (vec_f32_t *) _tmp;})
#define vec_array_f32_1(val)                     ({vec_f32_1_t  * _tmp = &val; (vec_f32_t *) _tmp;})

#define vec_set1_f32_16(val)                     vec_loop_expr(__m256_f32_2, 2, _tmp, _i, _tmp.v[_i] = vec_set1_f32_8(val);)
#define vec_set1_f32_8(val)                      _mm256_set1_ps(val)
#define vec_set1_f32_4(val)                      _mm_set1_ps(val)
#define vec_set1_f32_1(val)                      val

#define vec_set_iter_f32_16(iter, expr)          vec_loop_expr(__m256_f32_2, 2, _tmp, _i, _tmp.v[_i] = _mm256_set_ps(vec_iter_expr_8(iter, 8*_i, expr));)
#define vec_set_iter_f32_8(iter, expr)           _mm256_set_ps(vec_iter_expr_8(iter, 0, expr))
#define vec_set_iter_f32_4(iter, expr)           _mm_set_ps(vec_iter_expr_4(iter, 0, expr))
#define vec_set_iter_f32_1(iter, expr)           vec_iter_expr_1(iter, 0, expr)


#define vec_loadu_f32_16(ptr)                    vec_loop_expr(__m256_f32_2, 2, _tmp, _i, _tmp.v[_i] = vec_loadu_f32_8(((float*)(ptr)) + _i*8);)
#define vec_loadu_f32_8(ptr)                     _mm256_loadu_ps((float*)ptr)
#define vec_loadu_f32_4(ptr)                     _mm_loadu_ps((float*)ptr)
#define vec_loadu_f32_1(ptr)                     (*((vec_f32_1_t*)(ptr)))

#define vec_storeu_f32_16(ptr, vec)              vec_loop_stmt(2, _i, vec_storeu_f32_8(((float*)(ptr)) + _i*8, (vec).v[_i]);)
#define vec_storeu_f32_8(ptr, vec)               _mm256_storeu_ps((float*)(ptr), vec)
#define vec_storeu_f32_4(ptr, vec)               _mm_storeu_ps((float*)(ptr), vec)
#define vec_storeu_f32_1(ptr, vec)               do { (*((vec_f32_1_t*)(ptr))) = (vec); } while (0)


#define vec_add_f32_16(a, b)                     vec_loop_expr(__m256_f32_2, 2, _tmp, _i, _tmp.v[_i] = vec_add_f32_8(a.v[_i], b.v[_i]);)
#define vec_add_f32_8(a, b)                      _mm256_add_ps(a, b)
#define vec_add_f32_4(a, b)                      _mm_add_ps(a, b)
#define vec_add_f32_1(a, b)                      (a + b)

#define vec_sub_f32_16(a, b)                     vec_loop_expr(__m256_f32_2, 2, _tmp, _i, _tmp.v[_i] = vec_sub_f32_8(a.v[_i], b.v[_i]);)
#define vec_sub_f32_8(a, b)                      _mm256_sub_ps(a, b)
#define vec_sub_f32_4(a, b)                      _mm_sub_ps(a, b)
#define vec_sub_f32_1(a, b)                      (a - b)

#define vec_mul_f32_16(a, b)                     vec_loop_expr(__m256_f32_2, 2, _tmp, _i, _tmp.v[_i] = vec_mul_f32_8(a.v[_i], b.v[_i]);)
#define vec_mul_f32_8(a, b)                      _mm256_mul_ps(a, b)
#define vec_mul_f32_4(a, b)                      _mm_mul_ps(a, b)
#define vec_mul_f32_1(a, b)                      (a * b)

#define vec_div_f32_16(a, b)                     vec_loop_expr(__m256_f32_2, 2, _tmp, _i, _tmp.v[_i] = vec_div_f32_8(a.v[_i], b.v[_i]);)
#define vec_div_f32_8(a, b)                       _mm256_div_ps(a, b)
#define vec_div_f32_4(a, b)                       _mm_div_ps(a, b)
#define vec_div_f32_1(a, b)                      (a / b)

#define vec_fmadd_f32_16(a, b, c)                vec_loop_expr(__m256_f32_2, 2, _tmp, _i, _tmp.v[_i] = vec_fmadd_f32_8(a.v[_i], b.v[_i], c.v[_i]);)
#define vec_fmadd_f32_8(a, b, c)                 _mm256_fmadd_ps(a, b, c)
#define vec_fmadd_f32_4(a, b, c)                 _mm_fmadd_ps(a, b, c)
#define vec_fmadd_f32_1(a, b, c)                 (a * b + c)


#define vec_reduce_add_f32_16(a)                 vec_reduce_expr(float, 2, 0, _tmp, _i, _tmp += vec_reduce_add_f32_8(a.v[_i]);)

#define vec_reduce_add_f32_8(a)                                          \
({                                                                       \
	const __m128 low_128  = _mm256_castps256_ps128(a);               \
	const __m128 high_128 = _mm256_extractf128_ps(a, 1);             \
	const __m128 low_64   = _mm_add_ps(low_128, high_128);           \
	const __m128 high_64  = _mm_movehl_ps(low_64, low_64);           \
	const __m128 low_32   = _mm_add_ps(low_64, high_64);             \
	const __m128 high_32  = _mm_shuffle_ps(low_32, low_32, 0x01);    \
	const __m128 x32      = _mm_add_ss(low_32, high_32);             \
	_mm_cvtss_f32(x32);                                              \
})

#define vec_reduce_add_f32_4(a)                                          \
({                                                                       \
	const __m128 low_64   = a;                                       \
	const __m128 high_64  = _mm_movehl_ps(low_64, low_64);           \
	const __m128 low_32   = _mm_add_ps(low_64, high_64);             \
	const __m128 high_32  = _mm_shuffle_ps(low_32, low_32, 0x01);    \
	const __m128 x32      = _mm_add_ss(low_32, high_32);             \
	_mm_cvtss_f32(x32);                                              \
})

#define vec_reduce_add_f32_1(a)                  (a)


#endif /* VECTORIZATION_X86_AVX256_F32_H */

