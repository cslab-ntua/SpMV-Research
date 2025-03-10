#ifndef VECTORIZATION_X86_AVX128_I32_H
#define VECTORIZATION_X86_AVX128_I32_H

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <x86intrin.h>

#include "macros/cpp_defines.h"
#include "macros/macrolib.h"

#include "vectorization/vectorization_util.h"


typedef struct [[gnu::packed]] { __m128i v[4]; }  __m128i_i32_4;
#define vec_i32_16_t   __m128i_i32_4
typedef struct [[gnu::packed]] { __m128i v[2]; }  __m128i_i32_2;
#define vec_i32_8_t   __m128i_i32_2
#define vec_i32_4_t   __m128i
#define vec_i32_1_t   int32_t

#define vec_len_default_i32  4

typedef uint32_t vec_i32_t [[gnu::may_alias]];


#define vec_array_i32_16(val)                    ({vec_i32_16_t * _tmp = &val; (vec_i32_t *) _tmp;})
#define vec_array_i32_8(val)                     ({vec_i32_8_t  * _tmp = &val; (vec_i32_t *) _tmp;})
#define vec_array_i32_4(val)                     ({vec_i32_4_t  * _tmp = &val; (vec_i32_t *) _tmp;})
#define vec_array_i32_1(val)                     ({vec_i32_1_t  * _tmp = &val; (vec_i32_t *) _tmp;})

#define vec_set1_i32_16(val)                     vec_loop_expr(__m128i_i32_4, 4, _tmp, _i, _tmp.v[_i] = vec_set1_i32_4(val);)
#define vec_set1_i32_8(val)                      vec_loop_expr(__m128i_i32_2, 2, _tmp, _i, _tmp.v[_i] = vec_set1_i32_4(val);)
#define vec_set1_i32_4(val)                      _mm_set1_epi32(val)
#define vec_set1_i32_1(val)                      val

#define vec_set_iter_i32_16(iter, expr)          vec_loop_expr(__m128i_i32_4, 4, _tmp, _i, _tmp.v[_i] = _mm_set_epi32(vec_iter_expr_4(iter, 4*_i, expr));)
#define vec_set_iter_i32_8(iter, expr)           vec_loop_expr(__m128i_i32_2, 2, _tmp, _i, _tmp.v[_i] = _mm_set_epi32(vec_iter_expr_4(iter, 4*_i, expr));)
#define vec_set_iter_i32_4(iter, expr)           _mm_set_epi32(vec_iter_expr_4(iter, 0, expr))
#define vec_set_iter_i32_1(iter, expr)           vec_iter_expr_1(iter, 0, expr)


#define vec_loadu_i32_16(ptr)                    vec_loop_expr(__m128i_i32_4, 4, _tmp, _i, _tmp.v[_i] = vec_loadu_i32_4(((vec_i32_4_t*)(ptr)) + _i);)
#define vec_loadu_i32_8(ptr)                     vec_loop_expr(__m128i_i32_2, 2, _tmp, _i, _tmp.v[_i] = vec_loadu_i32_4(((vec_i32_4_t*)(ptr)) + _i);)
#define vec_loadu_i32_4(ptr)                     _mm_loadu_si128((ptr))
#define vec_loadu_i32_1(ptr)                     (*((vec_i32_1_t*)(ptr)))

#define vec_storeu_i32_16(ptr, vec)              vec_loop_stmt(4, _i, vec_storeu_i32_4(((vec_i32_4_t*)(ptr)) + _i, (vec).v[_i]);)
#define vec_storeu_i32_8(ptr, vec)               vec_loop_stmt(2, _i, vec_storeu_i32_4(((vec_i32_4_t*)(ptr)) + _i, (vec).v[_i]);)
#define vec_storeu_i32_4(ptr, vec)               _mm_storeu_si128((ptr), vec)
#define vec_storeu_i32_1(ptr, vec)               do { (*((vec_i32_1_t*)(ptr))) = (vec); } while (0)


#define vec_add_i32_16(a, b)                     vec_loop_expr(__m128i_i32_4, 4, _tmp, _i, _tmp.v[_i] = vec_add_i32_4(a.v[_i], b.v[_i]);)
#define vec_add_i32_8(a, b)                      vec_loop_expr(__m128i_i32_2, 2, _tmp, _i, _tmp.v[_i] = vec_add_i32_4(a.v[_i], b.v[_i]);)
#define vec_add_i32_4(a, b)                      _mm_add_epi32(a, b)
#define vec_add_i32_1(a, b)                      (a + b)

#define vec_sub_i32_16(a, b)                     vec_loop_expr(__m128i_i32_4, 4, _tmp, _i, _tmp.v[_i] = vec_sub_i32_4(a.v[_i], b.v[_i]);)
#define vec_sub_i32_8(a, b)                      vec_loop_expr(__m128i_i32_2, 2, _tmp, _i, _tmp.v[_i] = vec_sub_i32_4(a.v[_i], b.v[_i]);)
#define vec_sub_i32_4(a, b)                      _mm_sub_epi32(a, b)
#define vec_sub_i32_1(a, b)                      (a - b)


#define vec_mul_i32_16(a, b)                     vec_loop_expr(__m128i_i32_4, 4, _tmp, _i, _tmp.v[_i] = vec_mul_i32_4(a.v[_i], b.v[_i]);)
#define vec_mul_i32_8(a, b)                      vec_loop_expr(__m128i_i32_2, 2, _tmp, _i, _tmp.v[_i] = vec_mul_i32_4(a.v[_i], b.v[_i]);)
#define vec_mul_i32_4(a, b)                      _mm_mullo_epi32(a, b)
#define vec_mul_i32_1(a, b)                      (a * b)

#define vec_div_i32_16(a, b)                     vec_loop_expr(__m128i_i32_4, 4, _tmp, _i, _tmp.v[_i] = vec_div_i32_4(a.v[_i], b.v[_i]);)
#define vec_div_i32_8(a, b)                      vec_loop_expr(__m128i_i32_2, 2, _tmp, _i, _tmp.v[_i] = vec_div_i32_4(a.v[_i], b.v[_i]);)
#define vec_div_i32_4(a, b)                      ({union {vec_i32_4_t v; uint32_t s[4];} buf_a={.v=a}, buf_b={.v=b}; vec_set_iter_i32_4(_j, buf_a.s[_j] / buf_b.s[_j]);})
#define vec_div_i32_1(a, b)                      (a / b)


#define vec_and_i32_16(a, b)                     vec_loop_expr(__m128i_i32_4, 4, _tmp, _i, _tmp.v[_i] = vec_and_i32_4(a.v[_i], b.v[_i]);)
#define vec_and_i32_8(a, b)                      vec_loop_expr(__m128i_i32_2, 2, _tmp, _i, _tmp.v[_i] = vec_and_i32_4(a.v[_i], b.v[_i]);)
#define vec_and_i32_4(a, b)                      _mm_and_si128(a, b)
#define vec_and_i32_1(a, b)                      (a & b)

#define vec_or_i32_16(a, b)                      vec_loop_expr(__m128i_i32_4, 4, _tmp, _i, _tmp.v[_i] = vec_or_i32_4(a.v[_i], b.v[_i]);)
#define vec_or_i32_8(a, b)                       vec_loop_expr(__m128i_i32_2, 2, _tmp, _i, _tmp.v[_i] = vec_or_i32_4(a.v[_i], b.v[_i]);)
#define vec_or_i32_4(a, b)                       _mm_or_si128(a, b)
#define vec_or_i32_1(a, b)                       (a | b)


#define vec_slli_i32_16(a, imm8)                 vec_loop_expr(__m128i_i32_4, 4, _tmp, _i, _tmp.v[_i] = vec_slli_i32_4(a.v[_i], imm8);)
#define vec_slli_i32_8(a, imm8)                  vec_loop_expr(__m128i_i32_2, 2, _tmp, _i, _tmp.v[_i] = vec_slli_i32_4(a.v[_i], imm8);)
#define vec_slli_i32_4(a, imm8)                  _mm_slli_epi32(a, imm8)
#define vec_slli_i32_1(a, imm8)                  ((imm8 < 32) ? ((uint32_t) a)<<imm8 : 0)

#define vec_srli_i32_16(a, imm8)                 vec_loop_expr(__m128i_i32_4, 4, _tmp, _i, _tmp.v[_i] = vec_srli_i32_4(a.v[_i], imm8);)
#define vec_srli_i32_8(a, imm8)                  vec_loop_expr(__m128i_i32_2, 2, _tmp, _i, _tmp.v[_i] = vec_srli_i32_4(a.v[_i], imm8);)
#define vec_srli_i32_4(a, imm8)                  _mm_srli_epi32(a, imm8)
#define vec_srli_i32_1(a, imm8)                  ((imm8 < 32) ? ((uint32_t) a)>>imm8 : 0)


#define vec_srai_i32_16(a, imm8)                 vec_loop_expr(__m128i_i32_4, 4, _tmp, _i, _tmp.v[_i] = vec_srai_i32_4(a.v[_i], imm8);)
#define vec_srai_i32_8(a, imm8)                  vec_loop_expr(__m128i_i32_2, 2, _tmp, _i, _tmp.v[_i] = vec_srai_i32_4(a.v[_i], imm8);)
#define vec_srai_i32_4(a, imm8)                  _mm_srai_epi32(a, imm8)
#define vec_srai_i32_1(a, imm8)                  ((imm8 < 32) ? ((int32_t) a)>>imm8 : (((int32_t) a) < 0) ? -1 : 0)


#define vec_sllv_i32_16(a, count)                vec_loop_expr(__m128i_i32_4, 4, _tmp, _i, _tmp.v[_i] = vec_sllv_i32_4(a.v[_i], count.v[_i]);)
#define vec_sllv_i32_8(a, count)                 vec_loop_expr(__m128i_i32_2, 2, _tmp, _i, _tmp.v[_i] = vec_sllv_i32_4(a.v[_i], count.v[_i]);)
#define vec_sllv_i32_4(a, count)                 ({union {vec_i32_4_t v; uint32_t s[4];} buf_a={.v=a}, buf_count={.v=count}; vec_set_iter_i32_4(_j, vec_sllv_i32_1(buf_a.s[_j], buf_count.s[_j]));})
#define vec_sllv_i32_1(a, count)                 ((count < 32) ? ((uint32_t) a)<<count : 0)


#define vec_srlv_i32_16(a, count)                vec_loop_expr(__m128i_i32_4, 4, _tmp, _i, _tmp.v[_i] = vec_srlv_i32_4(a.v[_i], count.v[_i]);)
#define vec_srlv_i32_8(a, count)                 vec_loop_expr(__m128i_i32_2, 2, _tmp, _i, _tmp.v[_i] = vec_srlv_i32_4(a.v[_i], count.v[_i]);)
#define vec_srlv_i32_4(a, count)                 ({union {vec_i32_4_t v; uint32_t s[4];} buf_a={.v=a}, buf_count={.v=count}; vec_set_iter_i32_4(_j, vec_srlv_i32_1(buf_a.s[_j], buf_count.s[_j]));})
#define vec_srlv_i32_1(a, count)                 ((count < 32) ? ((uint32_t) a)>>count : 0)


#define vec_srav_i32_16(a, count)                vec_loop_expr(__m128i_i32_4, 4, _tmp, _i, _tmp.v[_i] = vec_srav_i32_4(a.v[_i], count.v[_i]);)
#define vec_srav_i32_8(a, count)                 vec_loop_expr(__m128i_i32_2, 2, _tmp, _i, _tmp.v[_i] = vec_srav_i32_4(a.v[_i], count.v[_i]);)
#define vec_srav_i32_4(a, count)                 ({union {vec_i32_4_t v; uint32_t s[4];} buf_a={.v=a}, buf_count={.v=count}; vec_set_iter_i32_4(_j, vec_srav_i32_1(buf_a.s[_j], buf_count.s[_j]));})
#define vec_srav_i32_1(a, count)                 ((count < 32) ? ((int32_t) a)>>count : (((int32_t) a) < 0) ? -1 : 0)


#endif /* VECTORIZATION_X86_AVX128_I32_H */

