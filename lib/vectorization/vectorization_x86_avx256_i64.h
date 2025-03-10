#ifndef VECTORIZATION_X86_AVX256_I64_H
#define VECTORIZATION_X86_AVX256_I64_H

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <x86intrin.h>

#include "macros/cpp_defines.h"
#include "macros/macrolib.h"

#include "vectorization/vectorization_util.h"


typedef struct [[gnu::packed]] { __m256i v[4]; }  __m256i_i64_4;
#define vec_i64_16_t  __m256i_i64_4
typedef struct [[gnu::packed]] { __m256i v[2]; }  __m256i_i64_2;
#define vec_i64_8_t   __m256i_i64_2
#define vec_i64_4_t   __m256i
#define vec_i64_2_t   __m128i
#define vec_i64_1_t   int64_t

#define vec_len_default_i64  4

typedef uint64_t vec_i64_t [[gnu::may_alias]];


#define vec_array_i64_16(val)                    ({vec_i64_16_t * _tmp = &val; (vec_i64_t *) _tmp;})
#define vec_array_i64_8(val)                     ({vec_i64_8_t  * _tmp = &val; (vec_i64_t *) _tmp;})
#define vec_array_i64_4(val)                     ({vec_i64_4_t  * _tmp = &val; (vec_i64_t *) _tmp;})
#define vec_array_i64_2(val)                     ({vec_i64_2_t  * _tmp = &val; (vec_i64_t *) _tmp;})
#define vec_array_i64_1(val)                     ({vec_i64_1_t  * _tmp = &val; (vec_i64_t *) _tmp;})

#define vec_set1_i64_16(val)                     vec_loop_expr(__m256i_i64_4, 4, _tmp, _i, _tmp.v[_i] = vec_set1_i64_4(val);)
#define vec_set1_i64_8(val)                      vec_loop_expr(__m256i_i64_2, 2, _tmp, _i, _tmp.v[_i] = vec_set1_i64_4(val);)
#define vec_set1_i64_4(val)                      _mm256_set1_epi64x(val)
#define vec_set1_i64_2(val)                      _mm_set1_epi64x(val)
#define vec_set1_i64_1(val)                      val

#define vec_set_iter_i64_16(iter, expr)          vec_loop_expr(__m256i_i64_4, 4, _tmp, _i, _tmp.v[_i] = _mm256_set_epi64x(vec_iter_expr_4(iter, 4*_i, expr));)
#define vec_set_iter_i64_8(iter, expr)           vec_loop_expr(__m256i_i64_2, 2, _tmp, _i, _tmp.v[_i] = _mm256_set_epi64x(vec_iter_expr_4(iter, 4*_i, expr));)
#define vec_set_iter_i64_4(iter, expr)           _mm256_set_epi64x(vec_iter_expr_4(iter, 0, expr))
#define vec_set_iter_i64_2(iter, expr)           _mm_set_epi64x(vec_iter_expr_2(iter, 0, expr))
#define vec_set_iter_i64_1(iter, expr)           vec_iter_expr_1(iter, 0, expr)


#define vec_loadu_i64_16(ptr)                    vec_loop_expr(__m256i_i64_4, 4, _tmp, _i, _tmp.v[_i] = vec_loadu_i64_4(((vec_i64_4_t*)(ptr)) + _i);)
#define vec_loadu_i64_8(ptr)                     vec_loop_expr(__m256i_i64_2, 2, _tmp, _i, _tmp.v[_i] = vec_loadu_i64_4(((vec_i64_4_t*)(ptr)) + _i);)
#define vec_loadu_i64_4(ptr)                     _mm256_loadu_si256((vec_i64_4_t*)(ptr))
#define vec_loadu_i64_2(ptr)                     _mm_loadu_si128((vec_i64_2_t*)(ptr))
#define vec_loadu_i64_1(ptr)                     (*((vec_i64_1_t*)(ptr)))

#define vec_storeu_i64_16(ptr, vec)              vec_loop_stmt(4, _i, vec_storeu_i64_4(((vec_i64_4_t*)(ptr)) + _i, (vec).v[_i]);)
#define vec_storeu_i64_8(ptr, vec)               vec_loop_stmt(2, _i, vec_storeu_i64_4(((vec_i64_4_t*)(ptr)) + _i, (vec).v[_i]);)
#define vec_storeu_i64_4(ptr, vec)               _mm256_storeu_si256((vec_i64_4_t*)(ptr), vec)
#define vec_storeu_i64_2(ptr, vec)               _mm_storeu_si128((vec_i64_2_t*)(ptr), vec)
#define vec_storeu_i64_1(ptr, vec)               do { (*((vec_i64_1_t*)(ptr))) = (vec); } while (0)


#define vec_add_i64_16(a, b)                     vec_loop_expr(__m256i_i64_4, 4, _tmp, _i, _tmp.v[_i] = vec_add_i64_4(a.v[_i], b.v[_i]);)
#define vec_add_i64_8(a, b)                      vec_loop_expr(__m256i_i64_2, 2, _tmp, _i, _tmp.v[_i] = vec_add_i64_4(a.v[_i], b.v[_i]);)
#define vec_add_i64_4(a, b)                      _mm256_add_epi64(a, b)
#define vec_add_i64_2(a, b)                      _mm_add_epi64(a, b)
#define vec_add_i64_1(a, b)                      (a + b)

#define vec_sub_i64_16(a, b)                     vec_loop_expr(__m256i_i64_4, 4, _tmp, _i, _tmp.v[_i] = vec_sub_i64_4(a.v[_i], b.v[_i]);)
#define vec_sub_i64_8(a, b)                      vec_loop_expr(__m256i_i64_2, 2, _tmp, _i, _tmp.v[_i] = vec_sub_i64_4(a.v[_i], b.v[_i]);)
#define vec_sub_i64_4(a, b)                      _mm256_sub_epi64(a, b)
#define vec_sub_i64_2(a, b)                      _mm_sub_epi64(a, b)
#define vec_sub_i64_1(a, b)                      (a - b)


#define vec_mul_i64_16(a, b)                     vec_loop_expr(__m256i_i64_4, 4, _tmp, _i, _tmp.v[_i] = vec_mul_i64_4(a.v[_i], b.v[_i]);)
#define vec_mul_i64_8(a, b)                      vec_loop_expr(__m256i_i64_2, 2, _tmp, _i, _tmp.v[_i] = vec_mul_i64_4(a.v[_i], b.v[_i]);)
#define vec_mul_i64_4(a, b)                      vec_set_iter_i64_4(_j, a[_j]*b[_j])
#define vec_mul_i64_2(a, b)                      vec_set_iter_i64_2(_j, a[_j]*b[_j])
#define vec_mul_i64_1(a, b)                      (a * b)

#define vec_div_i64_16(a, b)                     vec_loop_expr(__m256i_i64_4, 4, _tmp, _i, _tmp.v[_i] = vec_div_i64_4(a.v[_i], b.v[_i]);)
#define vec_div_i64_8(a, b)                      vec_loop_expr(__m256i_i64_2, 2, _tmp, _i, _tmp.v[_i] = vec_div_i64_4(a.v[_i], b.v[_i]);)
#define vec_div_i64_4(a, b)                      vec_set_iter_i64_4(_j, a[_j]/b[_j])
#define vec_div_i64_2(a, b)                      vec_set_iter_i64_2(_j, a[_j]/b[_j])
#define vec_div_i64_1(a, b)                      (a / b)


#define vec_and_i64_16(a, b)                     vec_loop_expr(__m256i_i64_4, 4, _tmp, _i, _tmp.v[_i] = vec_and_i64_4(a.v[_i], b.v[_i]);)
#define vec_and_i64_8(a, b)                      vec_loop_expr(__m256i_i64_2, 2, _tmp, _i, _tmp.v[_i] = vec_and_i64_4(a.v[_i], b.v[_i]);)
#define vec_and_i64_4(a, b)                      _mm256_and_si256(a, b)
#define vec_and_i64_2(a, b)                      _mm_and_si128(a, b)
#define vec_and_i64_1(a, b)                      (a & b)

#define vec_or_i64_16(a, b)                      vec_loop_expr(__m256i_i64_4, 4, _tmp, _i, _tmp.v[_i] = vec_or_i64_4(a.v[_i], b.v[_i]);)
#define vec_or_i64_8(a, b)                       vec_loop_expr(__m256i_i64_2, 2, _tmp, _i, _tmp.v[_i] = vec_or_i64_4(a.v[_i], b.v[_i]);)
#define vec_or_i64_4(a, b)                       _mm256_or_si256(a, b)
#define vec_or_i64_2(a, b)                       _mm_or_si128(a, b)
#define vec_or_i64_1(a, b)                       (a | b)


#define vec_slli_i64_16(a, imm8)                 vec_loop_expr(__m256i_i64_4, 4, _tmp, _i, _tmp.v[_i] = vec_slli_i64_4(a.v[_i], imm8);)
#define vec_slli_i64_8(a, imm8)                  vec_loop_expr(__m256i_i64_2, 2, _tmp, _i, _tmp.v[_i] = vec_slli_i64_4(a.v[_i], imm8);)
#define vec_slli_i64_4(a, imm8)                  _mm256_slli_epi64(a, imm8)
#define vec_slli_i64_2(a, imm8)                  _mm_slli_epi64(a, imm8)
#define vec_slli_i64_1(a, imm8)                  ((imm8 < 64) ? ((uint64_t) a)<<imm8 : 0)

#define vec_srli_i64_16(a, imm8)                 vec_loop_expr(__m256i_i64_4, 4, _tmp, _i, _tmp.v[_i] = vec_srli_i64_4(a.v[_i], imm8);)
#define vec_srli_i64_8(a, imm8)                  vec_loop_expr(__m256i_i64_2, 2, _tmp, _i, _tmp.v[_i] = vec_srli_i64_4(a.v[_i], imm8);)
#define vec_srli_i64_4(a, imm8)                  _mm256_srli_epi64(a, imm8)
#define vec_srli_i64_2(a, imm8)                  _mm_srli_epi64(a, imm8)
#define vec_srli_i64_1(a, imm8)                  ((imm8 < 64) ? ((uint64_t) a)>>imm8 : 0)


#define vec_srai_i64_16(a, imm8)                 vec_loop_expr(__m256i_i64_4, 4, _tmp, _i, _tmp.v[_i] = vec_srai_i64_4(a.v[_i], imm8);)
#define vec_srai_i64_8(a, imm8)                  vec_loop_expr(__m256i_i64_2, 2, _tmp, _i, _tmp.v[_i] = vec_srai_i64_4(a.v[_i], imm8);)
#define vec_srai_i64_4(a, imm8)                  ({union {vec_i64_4_t v; uint64_t s[4];} buf_a={.v=a}; vec_set_iter_i64_4(_j, vec_srai_i64_1(buf_a.s[_j], imm8));})
#define vec_srai_i64_2(a, imm8)                  ({union {vec_i64_2_t v; uint64_t s[2];} buf_a={.v=a}; vec_set_iter_i64_2(_j, vec_srai_i64_1(buf_a.s[_j], imm8));})
#define vec_srai_i64_1(a, imm8)                  ((imm8 < 64) ? ((int64_t) a)>>imm8 : (((int64_t) a) < 0) ? -1 : 0)


#define vec_sllv_i64_16(a, count)                vec_loop_expr(__m256i_i64_4, 4, _tmp, _i, _tmp.v[_i] = vec_sllv_i64_4(a.v[_i], count.v[_i]);)
#define vec_sllv_i64_8(a, count)                 vec_loop_expr(__m256i_i64_2, 2, _tmp, _i, _tmp.v[_i] = vec_sllv_i64_4(a.v[_i], count.v[_i]);)
#define vec_sllv_i64_4(a, count)                 _mm256_sllv_epi64(a, count)
#define vec_sllv_i64_2(a, count)                 _mm_sllv_epi64(a, count)
#define vec_sllv_i64_1(a, count)                 ((count < 64) ? ((uint64_t) a)<<count : 0)


#define vec_srlv_i64_16(a, count)                vec_loop_expr(__m256i_i64_4, 4, _tmp, _i, _tmp.v[_i] = vec_srlv_i64_4(a.v[_i], count.v[_i]);)
#define vec_srlv_i64_8(a, count)                 vec_loop_expr(__m256i_i64_2, 2, _tmp, _i, _tmp.v[_i] = vec_srlv_i64_4(a.v[_i], count.v[_i]);)
#define vec_srlv_i64_4(a, count)                 _mm256_srlv_epi64(a, count)
#define vec_srlv_i64_2(a, count)                 _mm_srlv_epi64(a, count)
#define vec_srlv_i64_1(a, count)                 ((count < 64) ? ((uint64_t) a)>>count : 0)


#define vec_srav_i64_16(a, count)                vec_loop_expr(__m256i_i64_4, 4, _tmp, _i, _tmp.v[_i] = vec_srav_i64_4(a.v[_i], count.v[_i]);)
#define vec_srav_i64_8(a, count)                 vec_loop_expr(__m256i_i64_2, 2, _tmp, _i, _tmp.v[_i] = vec_srav_i64_4(a.v[_i], count.v[_i]);)
#define vec_srav_i64_4(a, count)                 ({union {vec_i64_4_t v; uint64_t s[4];} buf_a={.v=a}, buf_count={.v=count}; vec_set_iter_i64_4(_j, vec_srav_i64_1(buf_a.s[_j], buf_count.s[_j]));})
#define vec_srav_i64_2(a, count)                 ({union {vec_i64_2_t v; uint64_t s[2];} buf_a={.v=a}, buf_count={.v=count}; vec_set_iter_i64_2(_j, vec_srav_i64_1(buf_a.s[_j], buf_count.s[_j]));})
#define vec_srav_i64_1(a, count)                 ((count < 64) ? ((int64_t) a)>>count : (((int64_t) a) < 0) ? -1 : 0)


#endif /* VECTORIZATION_X86_AVX256_I64_H */

