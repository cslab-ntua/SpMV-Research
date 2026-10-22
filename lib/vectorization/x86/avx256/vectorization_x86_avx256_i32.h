#ifndef VECTORIZATION_X86_AVX256_I32_H
#define VECTORIZATION_X86_AVX256_I32_H

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <x86intrin.h>

#include "macros/cpp_defines.h"
#include "macros/macrolib.h"

#include "vectorization/vectorization_util.h"
#include "vectorization_x86_avx256_m32.h"


typedef union __attribute__((packed, aligned(4))) { int32_t v;    double  vf64;    float  vf32;     int32_t s[ 1]; int32_t si[ 1]; uint32_t su[ 1]; float sf[ 1]; }  vec_i32_1_t;
typedef union __attribute__((packed, aligned(4))) { __m128i v;    __m128d vf64;    __m128 vf32;     int32_t s[ 4]; int32_t si[ 4]; uint32_t su[ 4]; float sf[ 4]; }  vec_i32_4_t;
typedef union __attribute__((packed, aligned(4))) { __m256i v;    __m256d vf64;    __m256 vf32;     int32_t s[ 8]; int32_t si[ 8]; uint32_t su[ 8]; float sf[ 8]; }  vec_i32_8_t;
typedef union __attribute__((packed, aligned(4))) { __m256i v[2]; __m256d vf64[2]; __m256 vf32[2];  int32_t s[16]; int32_t si[16]; uint32_t su[16]; float sf[16]; }  vec_i32_16_t;
typedef union __attribute__((packed, aligned(4))) { __m256i v[4]; __m256d vf64[4]; __m256 vf32[4];  int32_t s[32]; int32_t si[32]; uint32_t su[32]; float sf[32]; }  vec_i32_32_t;

typedef vec_i32_1_t   vec_perm_p32_1_t;
typedef vec_i32_4_t   vec_perm_p32_4_t;
typedef vec_i32_8_t   vec_perm_p32_8_t;
typedef vec_i32_16_t  vec_perm_p32_16_t;
typedef vec_i32_32_t  vec_perm_p32_32_t;

typedef int32_t  vec_perm_elem_p32_1_t;
typedef int32_t  vec_perm_elem_p32_4_t;
typedef int32_t  vec_perm_elem_p32_8_t;
typedef int32_t  vec_perm_elem_p32_16_t;
typedef int32_t  vec_perm_elem_p32_32_t;

#define vec_len_default_i32     8
#define vec_len_default_i32_f2  4


#define vec_i32_8(val)                                     ( (vec_i32_8_t)  { .v = (val) } )
#define vec_i32_4(val)                                     ( (vec_i32_4_t)  { .v = (val) } )
#define vec_i32_1(val)                                     ( (vec_i32_1_t)  { .v = (val) } )

#define vec_perm_p32_8(val)                                ( (vec_perm_p32_8_t)  { .v = (val) } )
#define vec_perm_p32_4(val)                                ( (vec_perm_p32_4_t)  { .v = (val) } )
#define vec_perm_p32_1(val)                                ( (vec_perm_p32_1_t)  { .v = (val) } )


//------------------------------------------------------------------------------------------------------------------------------------------
//- Cast
//------------------------------------------------------------------------------------------------------------------------------------------

#define vec_cast_to_i32_i32_32(val)                        (val)
#define vec_cast_to_i32_i32_16(val)                        (val)
#define vec_cast_to_i32_i32_8(val)                         (val)
#define vec_cast_to_i32_i32_4(val)                         (val)
#define vec_cast_to_i32_i32_1(val)                         (val)

#define vec_cast_to_i64_i32_32(val)                        vec_loop_expr(vec_i64_16_t, 4, _tmp, _i, (_tmp).v[_i] = (val).v[_i];)
#define vec_cast_to_i64_i32_16(val)                        vec_loop_expr(vec_i64_8_t,  2, _tmp, _i, (_tmp).v[_i] = (val).v[_i];)
#define vec_cast_to_i64_i32_8(val)                         ( (vec_i64_4_t)  { .v = (val).v } )
#define vec_cast_to_i64_i32_4(val)                         ( (vec_i64_2_t)  { .v = (val).v } )
#define vec_cast_to_i64_i32_1(val)                         ( (vec_i64_1_t)  { .v = (val).v } )

#define vec_cast_to_f32_i32_32(val)                        vec_loop_expr(vec_f32_32_t, 4, _tmp, _i, (_tmp).v[_i] = (val).vf32[_i];)
#define vec_cast_to_f32_i32_16(val)                        vec_loop_expr(vec_f32_16_t, 2, _tmp, _i, (_tmp).v[_i] = (val).vf32[_i];)
#define vec_cast_to_f32_i32_8(val)                         ( (vec_f32_8_t)  { .v = (val).v } )
#define vec_cast_to_f32_i32_4(val)                         ( (vec_f32_4_t)  { .v = (val).v } )
#define vec_cast_to_f32_i32_1(val)                         ( (vec_f32_2_t)  { .v = (val).v } )

#define vec_cast_to_f64_i32_32(val)                        vec_loop_expr(vec_f64_16_t, 4, _tmp, _i, (_tmp).v[_i] = (val).vf64[_i];)
#define vec_cast_to_f64_i32_16(val)                        vec_loop_expr(vec_f64_8_t,  2, _tmp, _i, (_tmp).v[_i] = (val).vf64[_i];)
#define vec_cast_to_f64_i32_8(val)                         ( (vec_f64_4_t)  { .v = (val).v } )
#define vec_cast_to_f64_i32_4(val)                         ( (vec_f64_2_t)  { .v = (val).v } )
#define vec_cast_to_f64_i32_1(val)                         ( (vec_f64_1_t)  { .v = (val).v } )


//------------------------------------------------------------------------------------------------------------------------------------------
//- Set - Load - Store
//------------------------------------------------------------------------------------------------------------------------------------------

#define vec_elem_get_i32_32(vec, index)                    ((vec).s[index])
#define vec_elem_get_i32_16(vec, index)                    ((vec).s[index])
#define vec_elem_get_i32_8(vec, index)                     ((vec).s[index])
#define vec_elem_get_i32_4(vec, index)                     ((vec).s[index])
#define vec_elem_get_i32_1(vec, index)                     ((vec).s[index])

#define vec_elem_get_p32_32(vec, index)                    ((vec).s[index])
#define vec_elem_get_p32_16(vec, index)                    ((vec).s[index])
#define vec_elem_get_p32_8(vec, index)                     ((vec).s[index])
#define vec_elem_get_p32_4(vec, index)                     ((vec).s[index])
#define vec_elem_get_p32_1(vec, index)                     ((vec).s[index])

#define vec_elem_set_i32_32(vec, index, expr)              { (vec).s[index] = (expr); } while (0)
#define vec_elem_set_i32_16(vec, index, expr)              { (vec).s[index] = (expr); } while (0)
#define vec_elem_set_i32_8(vec, index, expr)               { (vec).s[index] = (expr); } while (0)
#define vec_elem_set_i32_4(vec, index, expr)               { (vec).s[index] = (expr); } while (0)
#define vec_elem_set_i32_1(vec, index, expr)               { (vec).s[index] = (expr); } while (0)

#define vec_elem_set_p32_32(vec, index, expr)              { (vec).s[index] = (expr); } while (0)
#define vec_elem_set_p32_16(vec, index, expr)              { (vec).s[index] = (expr); } while (0)
#define vec_elem_set_p32_8(vec, index, expr)               { (vec).s[index] = (expr); } while (0)
#define vec_elem_set_p32_4(vec, index, expr)               { (vec).s[index] = (expr); } while (0)
#define vec_elem_set_p32_1(vec, index, expr)               { (vec).s[index] = (expr); } while (0)

#define vec_array_i32_32(vec)                              (vec).s
#define vec_array_i32_16(vec)                              (vec).s
#define vec_array_i32_8(vec)                               (vec).s
#define vec_array_i32_4(vec)                               (vec).s
#define vec_array_i32_1(vec)                               (vec).s

#define vec_set1_i32_32(val)                               vec_loop_expr(vec_i32_32_t, 4, _tmp, _i, (_tmp).v[_i] = _mm256_set1_epi32(val);)
#define vec_set1_i32_16(val)                               vec_loop_expr(vec_i32_16_t, 2, _tmp, _i, (_tmp).v[_i] = _mm256_set1_epi32(val);)
#define vec_set1_i32_8(val)                                vec_i32_8( _mm256_set1_epi32(val) )
#define vec_set1_i32_4(val)                                vec_i32_4( _mm_set1_epi32(val) )
#define vec_set1_i32_1(val)                                vec_i32_1( (int32_t) (val) )

#define vec_set_iter_i32_32(iter, expr)                    vec_loop_expr(vec_i32_32_t, 4, _tmp, _i, (_tmp).v[_i] = _mm256_set_epi32(vec_iter_expr_8(iter, 8*_i, (expr)));)
#define vec_set_iter_i32_16(iter, expr)                    vec_loop_expr(vec_i32_16_t, 2, _tmp, _i, (_tmp).v[_i] = _mm256_set_epi32(vec_iter_expr_8(iter, 8*_i, (expr)));)
#define vec_set_iter_i32_8(iter, expr)                     vec_i32_8( _mm256_set_epi32(vec_iter_expr_8(iter, 0, (expr))) )
#define vec_set_iter_i32_4(iter, expr)                     vec_i32_4( _mm_set_epi32(vec_iter_expr_4(iter, 0, (expr))) )
#define vec_set_iter_i32_1(iter, expr)                     vec_i32_1( (int32_t) vec_iter_expr_1(iter, 0, (expr)) )

#define vec_set_iter_p32_32(iter, expr)                    vec_loop_expr(vec_perm_p32_32_t, 4, _tmp, _i, (_tmp).v[_i] = _mm256_set_epi32(vec_iter_expr_8(iter, 8*_i, (expr)));)
#define vec_set_iter_p32_16(iter, expr)                    vec_loop_expr(vec_perm_p32_16_t, 2, _tmp, _i, (_tmp).v[_i] = _mm256_set_epi32(vec_iter_expr_8(iter, 8*_i, (expr)));)
#define vec_set_iter_p32_8(iter, expr)                     vec_perm_p32_8( _mm256_set_epi32(vec_iter_expr_8(iter, 0, (expr))) )
#define vec_set_iter_p32_4(iter, expr)                     vec_perm_p32_4( _mm_set_epi32(vec_iter_expr_4(iter, 0, (expr))) )
#define vec_set_iter_p32_1(iter, expr)                     vec_perm_p32_1( (int32_t) vec_iter_expr_1(iter, 0, (expr)) )

#define vec_loadu_i32_32(ptr)                              vec_loop_expr(vec_i32_32_t, 4, _tmp, _i, (_tmp).v[_i] = _mm256_loadu_si256(((__m256i *) (ptr)) + _i);)
#define vec_loadu_i32_16(ptr)                              vec_loop_expr(vec_i32_16_t, 2, _tmp, _i, (_tmp).v[_i] = _mm256_loadu_si256(((__m256i *) (ptr)) + _i);)
#define vec_loadu_i32_8(ptr)                               vec_i32_8( _mm256_loadu_si256((__m256i *) (ptr)) )
#define vec_loadu_i32_4(ptr)                               vec_i32_4( _mm_loadu_si128((__m128i *) (ptr)) )
#define vec_loadu_i32_1(ptr)                               vec_i32_1( (*((int32_t *) (ptr))) )

#define vec_loadu_p32_32(ptr)                              vec_loop_expr(vec_i32_32_t, 4, _tmp, _i, (_tmp).v[_i] = _mm256_loadu_si256(((__m256i *) (ptr)) + _i);)
#define vec_loadu_p32_16(ptr)                              vec_loop_expr(vec_i32_16_t, 2, _tmp, _i, (_tmp).v[_i] = _mm256_loadu_si256(((__m256i *) (ptr)) + _i);)
#define vec_loadu_p32_8(ptr)                               vec_i32_8( _mm256_loadu_si256((__m256i *) (ptr)) )
#define vec_loadu_p32_4(ptr)                               vec_i32_4( _mm_loadu_si128((__m128i *) (ptr)) )
#define vec_loadu_p32_1(ptr)                               vec_i32_1( (*((int32_t *) (ptr))) )

#define vec_loadu_maskedz_i32_32(ptr, mask)                vec_loop_expr(vec_i32_32_t, 4, _tmp, _i, (_tmp).v[_i] = _mm256_maskload_epi32(((int const *) (ptr)) + 8*_i, (mask).v[_i]);)
#define vec_loadu_maskedz_i32_16(ptr, mask)                vec_loop_expr(vec_i32_16_t, 2, _tmp, _i, (_tmp).v[_i] = _mm256_maskload_epi32(((int const *) (ptr)) + 8*_i, (mask).v[_i]);)
#define vec_loadu_maskedz_i32_8(ptr, mask)                 vec_i32_8( _mm256_maskload_epi32((int const *) (ptr), (mask).v) )
#define vec_loadu_maskedz_i32_4(ptr, mask)                 vec_i32_4( _mm_maskload_epi32((int const *) (ptr), (mask).v) )
#define vec_loadu_maskedz_i32_1(ptr, mask)                 vec_i32_1( ((mask).v) ? (*((int32_t *) (ptr))) : 0 )

#define vec_loadu_firstNz_i32_32(ptr, N)                   vec_loop_expr(vec_i32_32_t, 4, _tmp, _i, (_tmp).v[_i] = _mm256_maskload_epi32(((int const *) (ptr)) + 8*_i, (_mask).v[_i]);)
#define vec_loadu_firstNz_i32_16(ptr, N)                   vec_loop_expr(vec_i32_16_t, 2, _tmp, _i, (_tmp).v[_i] = _mm256_maskload_epi32(((int const *) (ptr)) + 8*_i, (_mask).v[_i]);)
#define vec_loadu_firstNz_i32_8(ptr, N)                    vec_i32_8( ({ vec_mask_m32_8_t _mask = vec_mask_firstN_m32_8(N); _mm256_maskload_epi32((int const *) (ptr), (_mask).v); }) )
#define vec_loadu_firstNz_i32_4(ptr, N)                    vec_i32_4( ({ vec_mask_m32_4_t _mask = vec_mask_firstN_m32_4(N); _mm_maskload_epi32((int const *) (ptr), (_mask).v); }) )
#define vec_loadu_firstNz_i32_1(ptr, N)                    vec_i32_1( (N > 0) ? (*((int32_t *) (ptr))) : 0 )

#define vec_storeu_i32_32(ptr, vec)                        vec_loop_stmt(4, _i, _mm256_storeu_si256(((__m256i *) (ptr)) + _i, (vec).v[_i]);)
#define vec_storeu_i32_16(ptr, vec)                        vec_loop_stmt(2, _i, _mm256_storeu_si256(((__m256i *) (ptr)) + _i, (vec).v[_i]);)
#define vec_storeu_i32_8(ptr, vec)                         _mm256_storeu_si256((__m256i *) (ptr), (vec).v)
#define vec_storeu_i32_4(ptr, vec)                         _mm_storeu_si128((__m128i *) (ptr), (vec).v)
#define vec_storeu_i32_1(ptr, vec)                         do { (*((int32_t *) (ptr))) = ((vec).v); } while (0)

#define vec_storeu_p32_32(ptr, vec)                        vec_loop_stmt(4, _i, _mm256_storeu_si256(((__m256i *) (ptr)) + _i, (vec).v[_i]);)
#define vec_storeu_p32_16(ptr, vec)                        vec_loop_stmt(2, _i, _mm256_storeu_si256(((__m256i *) (ptr)) + _i, (vec).v[_i]);)
#define vec_storeu_p32_8(ptr, vec)                         _mm256_storeu_si256((__m256i *) (ptr), (vec).v)
#define vec_storeu_p32_4(ptr, vec)                         _mm_storeu_si128((__m128i *) (ptr), (vec).v)
#define vec_storeu_p32_1(ptr, vec)                         do { (*((int32_t *) (ptr))) = ((vec).v); } while (0)

#define vec_storeu_masked_i32_32(ptr, vec, mask)           vec_loop_stmt(4, _i, _mm256_maskstore_epi32(((int *) (ptr)) + 8*_i, (mask).v[_i], (vec).v[_i]);)
#define vec_storeu_masked_i32_16(ptr, vec, mask)           vec_loop_stmt(2, _i, _mm256_maskstore_epi32(((int *) (ptr)) + 8*_i, (mask).v[_i], (vec).v[_i]);)
#define vec_storeu_masked_i32_8(ptr, vec, mask)            _mm256_maskstore_epi32((int *) (ptr), (mask).v, (vec).v)  /* Reversed order of vec and mask vs maskload, just to escape boredom I guess. */
#define vec_storeu_masked_i32_4(ptr, vec, mask)            _mm_maskstore_epi32((int *) (ptr), (mask).v, (vec).v)
#define vec_storeu_masked_i32_1(ptr, vec, mask)            do { if ((mask).v) (*((int32_t *) (ptr))) = ((vec).v); } while (0)

#define vec_gather_i32_i32_32(ptr, idx)                    vec_loop_expr(vec_i32_32_t, 4, _tmp, _i, (_tmp).v[_i] = _mm256_loadu_si256(((__m256i *) (ptr)) + _i);)
#define vec_gather_i32_i32_16(ptr, idx)                    vec_loop_expr(vec_i32_32_t, 2, _tmp, _i, (_tmp).v[_i] = _mm256_loadu_si256(((__m256i *) (ptr)) + _i);)
#define vec_gather_i32_i32_8(ptr, idx)                     vec_i32_8( _mm256_i32gather_epi32((int const *) (ptr), (idx).v, 4) )
#define vec_gather_i32_i32_4(ptr, idx)                     vec_i32_4( _mm_i32gather_epi32((int const *) (ptr), (idx).v, 4) )
#define vec_gather_i32_i32_1(ptr, idx)                     vec_i32_1( ((int32_t *) (ptr))[(idx).v] )

#define vec_gather_i32_i64_32(ptr, idx)                    vec_loop_expr(vec_i32_32_t, 4, _tmp, _i, (_tmp).v[_i] = _mm256_loadu_si256(((__m256i *) (ptr)) + _i);)
#define vec_gather_i32_i64_16(ptr, idx)                    vec_loop_expr(vec_i32_32_t, 2, _tmp, _i, (_tmp).v[_i] = _mm256_loadu_si256(((__m256i *) (ptr)) + _i);)
#define vec_gather_i32_i64_8(ptr, idx)                     vec_i32_8( _mm256_i64gather_epi32((int const *) (ptr), (idx).v, 4) )
#define vec_gather_i32_i64_4(ptr, idx)                     vec_i32_4( _mm_i64gather_epi32((int const *) (ptr), (idx).v, 4) )
#define vec_gather_i32_i64_1(ptr, idx)                     vec_i32_1( ((int32_t *) (ptr))[(idx).v] )


//------------------------------------------------------------------------------------------------------------------------------------------
//- Operations
//------------------------------------------------------------------------------------------------------------------------------------------

#define vec_add_i32_32(a, b)                               vec_loop_expr(vec_i32_32_t, 4, _tmp, _i, (_tmp).v[_i] = _mm256_add_epi32((a).v[_i], (b).v[_i]);)
#define vec_add_i32_16(a, b)                               vec_loop_expr(vec_i32_16_t, 2, _tmp, _i, (_tmp).v[_i] = _mm256_add_epi32((a).v[_i], (b).v[_i]);)
#define vec_add_i32_8(a, b)                                vec_i32_8( _mm256_add_epi32((a).v, (b).v) )
#define vec_add_i32_4(a, b)                                vec_i32_4( _mm_add_epi32((a).v, (b).v) )
#define vec_add_i32_1(a, b)                                vec_i32_1( ((a).v + (b).v) )

#define vec_sub_i32_32(a, b)                               vec_loop_expr(vec_i32_32_t, 4, _tmp, _i, (_tmp).v[_i] = _mm256_sub_epi32((a).v[_i], (b).v[_i]);)
#define vec_sub_i32_16(a, b)                               vec_loop_expr(vec_i32_16_t, 2, _tmp, _i, (_tmp).v[_i] = _mm256_sub_epi32((a).v[_i], (b).v[_i]);)
#define vec_sub_i32_8(a, b)                                vec_i32_8( _mm256_sub_epi32((a).v, (b).v) )
#define vec_sub_i32_4(a, b)                                vec_i32_4( _mm_sub_epi32((a).v, (b).v) )
#define vec_sub_i32_1(a, b)                                vec_i32_1( ((a).v - (b).v) )

#define vec_mul_i32_32(a, b)                               vec_loop_expr(vec_i32_32_t, 4, _tmp, _i, (_tmp).v[_i] = _mm256_mullo_epi32((a).v[_i], (b).v[_i]);)
#define vec_mul_i32_16(a, b)                               vec_loop_expr(vec_i32_16_t, 2, _tmp, _i, (_tmp).v[_i] = _mm256_mullo_epi32((a).v[_i], (b).v[_i]);)
#define vec_mul_i32_8(a, b)                                vec_i32_8( _mm256_mullo_epi32((a).v, (b).v) )
#define vec_mul_i32_4(a, b)                                vec_i32_4( _mm_mullo_epi32((a).v, (b).v) )
#define vec_mul_i32_1(a, b)                                vec_i32_1( ((a).v * (b).v) )

#define vec_div_i32_32(a, b)                               vec_set_iter_i32_32(_j, a.si[_j]/b.si[_j])
#define vec_div_i32_16(a, b)                               vec_set_iter_i32_16(_j, a.si[_j]/b.si[_j])
#define vec_div_i32_8(a, b)                                vec_set_iter_i32_8(_j, a.si[_j]/b.si[_j])
#define vec_div_i32_4(a, b)                                vec_set_iter_i32_4(_j, a.si[_j]/b.si[_j])
#define vec_div_i32_1(a, b)                                vec_i32_1( ((a).v / (b).v) )

#define vec_and_i32_32(a, b)                               vec_loop_expr(vec_i32_32_t, 4, _tmp, _i, (_tmp).v[_i] = _mm256_and_si256((a).v[_i], (b).v[_i]);)
#define vec_and_i32_16(a, b)                               vec_loop_expr(vec_i32_16_t, 2, _tmp, _i, (_tmp).v[_i] = _mm256_and_si256((a).v[_i], (b).v[_i]);)
#define vec_and_i32_8(a, b)                                vec_i32_8( _mm256_and_si256((a).v, (b).v) )
#define vec_and_i32_4(a, b)                                vec_i32_4( _mm_and_si128((a).v, (b).v) )
#define vec_and_i32_1(a, b)                                vec_i32_1( ((a).v & (b).v) )

#define vec_or_i32_32(a, b)                                vec_loop_expr(vec_i32_32_t, 4, _tmp, _i, (_tmp).v[_i] = _mm256_or_si256((a).v[_i], (b).v[_i]);)
#define vec_or_i32_16(a, b)                                vec_loop_expr(vec_i32_16_t, 2, _tmp, _i, (_tmp).v[_i] = _mm256_or_si256((a).v[_i], (b).v[_i]);)
#define vec_or_i32_8(a, b)                                 vec_i32_8( _mm256_or_si256((a).v, (b).v) )
#define vec_or_i32_4(a, b)                                 vec_i32_4( _mm_or_si128((a).v, (b).v) )
#define vec_or_i32_1(a, b)                                 vec_i32_1( ((a).v | (b).v) )

#define vec_not_i32_32(a)                                  vec_loop_expr(vec_i32_32_t, 4, _tmp, _i, (_tmp).v[_i] = _mm256_xor_si256((a).v[_i], _mm256_set1_epi32(-1));)
#define vec_not_i32_16(a)                                  vec_loop_expr(vec_i32_16_t, 2, _tmp, _i, (_tmp).v[_i] = _mm256_xor_si256((a).v[_i], _mm256_set1_epi32(-1));)
#define vec_not_i32_8(a)                                   vec_i32_8( _mm256_xor_si256((a).v, _mm256_set1_epi32(-1)) )
#define vec_not_i32_4(a)                                   vec_i32_4( _mm_xor_si128((a).v, _mm_set1_epi32(-1)) )
#define vec_not_i32_1(a)                                   vec_i32_1( ((a).v ^ -1) )

#define vec_xor_i32_32(a, b)                               vec_loop_expr(vec_i32_32_t, 4, _tmp, _i, (_tmp).v[_i] = _mm256_xor_si256((a).v[_i], (b).v[_i]);)
#define vec_xor_i32_16(a, b)                               vec_loop_expr(vec_i32_16_t, 2, _tmp, _i, (_tmp).v[_i] = _mm256_xor_si256((a).v[_i], (b).v[_i]);)
#define vec_xor_i32_8(a, b)                                vec_i32_8( _mm256_xor_si256((a).v, (b).v) )
#define vec_xor_i32_4(a, b)                                vec_i32_4( _mm_xor_si128((a).v, (b).v) )
#define vec_xor_i32_1(a, b)                                vec_i32_1( ((a).v ^ (b).v) )


//------------------------------------------------------------------------------------------------------------------------------------------
//- Shift
//------------------------------------------------------------------------------------------------------------------------------------------

#define vec_slli_i32_32(a, imm8)                           vec_loop_expr(vec_i32_32_t, 4, _tmp, _i, (_tmp).v[_i] = _mm256_slli_epi32((a).v[_i], imm8);)
#define vec_slli_i32_16(a, imm8)                           vec_loop_expr(vec_i32_16_t, 2, _tmp, _i, (_tmp).v[_i] = _mm256_slli_epi32((a).v[_i], imm8);)
#define vec_slli_i32_8(a, imm8)                            vec_i32_8( _mm256_slli_epi32((a).v, imm8) )
#define vec_slli_i32_4(a, imm8)                            vec_i32_4( _mm_slli_epi32((a).v, imm8) )
#define vec_slli_i32_1(a, imm8)                            vec_i32_1( (int32_t) ((imm8 < 32) ? ((uint32_t) (a).v)<<imm8 : 0) )

#define vec_srli_i32_32(a, imm8)                           vec_loop_expr(vec_i32_32_t, 4, _tmp, _i, (_tmp).v[_i] = _mm256_srli_epi32((a).v[_i], imm8);)
#define vec_srli_i32_16(a, imm8)                           vec_loop_expr(vec_i32_16_t, 2, _tmp, _i, (_tmp).v[_i] = _mm256_srli_epi32((a).v[_i], imm8);)
#define vec_srli_i32_8(a, imm8)                            vec_i32_8( _mm256_srli_epi32((a).v, imm8) )
#define vec_srli_i32_4(a, imm8)                            vec_i32_4( _mm_srli_epi32((a).v, imm8) )
#define vec_srli_i32_1(a, imm8)                            vec_i32_1( (int32_t) ((imm8 < 32) ? ((uint32_t) (a).v)>>imm8 : 0) )

#define vec_srai_i32_32(a, imm8)                           vec_loop_expr(vec_i32_32_t, 4, _tmp, _i, (_tmp).v[_i] = _mm256_srai_epi32((a).v[_i], imm8);)
#define vec_srai_i32_16(a, imm8)                           vec_loop_expr(vec_i32_16_t, 2, _tmp, _i, (_tmp).v[_i] = _mm256_srai_epi32((a).v[_i], imm8);)
#define vec_srai_i32_8(a, imm8)                            vec_i32_8( _mm256_srai_epi32((a).v, imm8) )
#define vec_srai_i32_4(a, imm8)                            vec_i32_4( _mm_srai_epi32((a).v, imm8) )
#define vec_srai_i32_1(a, imm8)                            vec_set_iter_i32_1(_j, vec_srai_i32_s(a.si[_j], imm8))
#define vec_srai_i32_s(a, imm8)                            ( (imm8 < 32) ? ((int32_t) a)>>imm8 : (((int32_t) a) < 0) ? -1 : 0 )

#define vec_sllv_i32_32(a, count)                          vec_loop_expr(vec_i32_32_t, 4, _tmp, _i, (_tmp).v[_i] = _mm256_sllv_epi32((a).v[_i], (count).v[_i]);)
#define vec_sllv_i32_16(a, count)                          vec_loop_expr(vec_i32_16_t, 2, _tmp, _i, (_tmp).v[_i] = _mm256_sllv_epi32((a).v[_i], (count).v[_i]);)
#define vec_sllv_i32_8(a, count)                           vec_i32_8( _mm256_sllv_epi32((a).v, (count).v) )
#define vec_sllv_i32_4(a, count)                           vec_i32_4( _mm_sllv_epi32((a).v, (count).v) )
#define vec_sllv_i32_1(a, count)                           vec_i32_1( (int32_t) (((count).v < 32) ? ((uint32_t) (a).v)<<(count).v : 0) )

#define vec_srlv_i32_32(a, count)                          vec_loop_expr(vec_i32_32_t, 4, _tmp, _i, (_tmp).v[_i] = _mm256_srlv_epi32((a).v[_i], (count).v[_i]);)
#define vec_srlv_i32_16(a, count)                          vec_loop_expr(vec_i32_16_t, 2, _tmp, _i, (_tmp).v[_i] = _mm256_srlv_epi32((a).v[_i], (count).v[_i]);)
#define vec_srlv_i32_8(a, count)                           vec_i32_8( _mm256_srlv_epi32((a).v, (count).v) )
#define vec_srlv_i32_4(a, count)                           vec_i32_4( _mm_srlv_epi32((a).v, (count).v) )
#define vec_srlv_i32_1(a, count)                           vec_i32_1( (int32_t) (((count).v < 32) ? ((uint32_t) (a).v)>>(count).v : 0) )

#define vec_srav_i32_32(a, count)                          vec_loop_expr(vec_i32_32_t, 4, _tmp, _i, (_tmp).v[_i] = _mm256_srav_epi32((a).v[_i], (count).v[_i]);)
#define vec_srav_i32_16(a, count)                          vec_loop_expr(vec_i32_16_t, 2, _tmp, _i, (_tmp).v[_i] = _mm256_srav_epi32((a).v[_i], (count).v[_i]);)
#define vec_srav_i32_8(a, count)                           vec_i32_8( _mm256_srav_epi32((a).v, (count).v) )
#define vec_srav_i32_4(a, count)                           vec_i32_4( _mm_srav_epi32((a).v, (count).v) )
#define vec_srav_i32_1(a, count)                           vec_set_iter_i32_1(_j, vec_srav_i32_s(a.si[_j], count.si[_j]))
#define vec_srav_i32_s(a, count)                           ( (count < 32) ? ((int32_t) a)>>count : (((int32_t) a) < 0) ? -1 : 0 )


//------------------------------------------------------------------------------------------------------------------------------------------
//- Compare
//------------------------------------------------------------------------------------------------------------------------------------------

#define vec_cmpeq_i32_32(a, b)                             vec_loop_expr(vec_mask_m32_32_t, 4, _tmp, _i, (_tmp).v[_i] = _mm256_cmpeq_epi32((a).v[_i], (b).v[_i]);)
#define vec_cmpeq_i32_16(a, b)                             vec_loop_expr(vec_mask_m32_16_t, 2, _tmp, _i, (_tmp).v[_i] = _mm256_cmpeq_epi32((a).v[_i], (b).v[_i]);)
#define vec_cmpeq_i32_8(a, b)                              vec_mask_m32_8( _mm256_cmpeq_epi32((a).v, (b).v) )
#define vec_cmpeq_i32_4(a, b)                              vec_mask_m32_4( _mm_cmpeq_epi32((a).v, (b).v) )
#define vec_cmpeq_i32_1(a, b)                              vec_mask_m32_1( (a).v == (b).v )

#define vec_cmpgt_i32_32(a, b)                             vec_loop_expr(vec_mask_m32_32_t, 4, _tmp, _i, (_tmp).v[_i] = _mm256_cmpgt_epi32((a).v[_i], (b).v[_i]);)
#define vec_cmpgt_i32_16(a, b)                             vec_loop_expr(vec_mask_m32_16_t, 2, _tmp, _i, (_tmp).v[_i] = _mm256_cmpgt_epi32((a).v[_i], (b).v[_i]);)
#define vec_cmpgt_i32_8(a, b)                              vec_mask_m32_8( _mm256_cmpgt_epi32((a).v, (b).v) )
#define vec_cmpgt_i32_4(a, b)                              vec_mask_m32_4( _mm_cmpgt_epi32((a).v, (b).v) )
#define vec_cmpgt_i32_1(a, b)                              vec_mask_m32_1( (a).v > (b).v )


//------------------------------------------------------------------------------------------------------------------------------------------
//- Shuffle - Permute
//------------------------------------------------------------------------------------------------------------------------------------------

#define vec_permute_i32_32(a, idx)                         vec_loop_expr(vec_i32_32_t, 32, _tmp, _i, _tmp.s[_i] = a.s[idx.s[_i]];)
#define vec_permute_i32_16(a, idx)                         vec_loop_expr(vec_i32_16_t, 16, _tmp, _i, _tmp.s[_i] = a.s[idx.s[_i]];)
#define vec_permute_i32_8(a, idx)                          vec_i32_8( _mm256_permutevar8x32_epi32((a).v, (idx).v) )
#define vec_permute_i32_4(a, idx)                          vec_i32_4( _mm_castps_si128(_mm_permutevar_ps((a).vf32, (idx).v)) )
#define vec_permute_i32_1(a, idx)                          a


#endif /* VECTORIZATION_X86_AVX256_I32_H */

