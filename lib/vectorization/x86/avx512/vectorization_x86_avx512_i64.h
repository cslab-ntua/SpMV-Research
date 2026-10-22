#ifndef VECTORIZATION_X86_AVX512_I64_H
#define VECTORIZATION_X86_AVX512_I64_H

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <x86intrin.h>

#include "macros/cpp_defines.h"
#include "macros/macrolib.h"

#include "vectorization/vectorization_util.h"
#include "vectorization_x86_avx512_m64.h"


typedef union __attribute__((packed, aligned(8))) { int64_t v;    double  vf64;     float  vf32;    int64_t s[ 1]; int64_t si[ 1]; uint64_t su[ 1]; double sf[ 1]; }  vec_i64_1_t;
typedef union __attribute__((packed, aligned(8))) { __m128i v;    __m128d vf64;     __m128 vf32;    int64_t s[ 2]; int64_t si[ 2]; uint64_t su[ 2]; double sf[ 2]; }  vec_i64_2_t;
typedef union __attribute__((packed, aligned(8))) { __m256i v;    __m256d vf64;     __m256 vf32;    int64_t s[ 4]; int64_t si[ 4]; uint64_t su[ 4]; double sf[ 4]; }  vec_i64_4_t;
typedef union __attribute__((packed, aligned(8))) { __m512i v;    __m512d vf64;     __m512 vf32;    int64_t s[ 8]; int64_t si[ 8]; uint64_t su[ 8]; double sf[ 8]; }  vec_i64_8_t;
typedef union __attribute__((packed, aligned(8))) { __m512i v[2]; __m512d vf64[2];  __m512 vf32[2]; int64_t s[16]; int64_t si[16]; uint64_t su[16]; double sf[16]; }  vec_i64_16_t;

typedef vec_i64_1_t   vec_perm_p64_1_t;
typedef vec_i64_2_t   vec_perm_p64_2_t;
typedef vec_i64_4_t   vec_perm_p64_4_t;
typedef vec_i64_8_t   vec_perm_p64_8_t;
typedef vec_i64_16_t  vec_perm_p64_16_t;

typedef int64_t  vec_perm_elem_p64_1_t;
typedef int64_t  vec_perm_elem_p64_2_t;
typedef int64_t  vec_perm_elem_p64_4_t;
typedef int64_t  vec_perm_elem_p64_8_t;
typedef int64_t  vec_perm_elem_p64_16_t;

#define vec_len_default_i64     8
#define vec_len_default_i64_f2  4


#define vec_i64_8(val)                                     ( (vec_i64_8_t)  { .v = (val) } )
#define vec_i64_4(val)                                     ( (vec_i64_4_t)  { .v = (val) } )
#define vec_i64_2(val)                                     ( (vec_i64_2_t)  { .v = (val) } )
#define vec_i64_1(val)                                     ( (vec_i64_1_t)  { .v = (val) } )

#define vec_perm_p64_8(val)                                ( (vec_perm_p64_8_t)  { .v = (val) } )
#define vec_perm_p64_4(val)                                ( (vec_perm_p64_4_t)  { .v = (val) } )
#define vec_perm_p64_2(val)                                ( (vec_perm_p64_2_t)  { .v = (val) } )
#define vec_perm_p64_1(val)                                ( (vec_perm_p64_1_t)  { .v = (val) } )


//------------------------------------------------------------------------------------------------------------------------------------------
//- Cast
//------------------------------------------------------------------------------------------------------------------------------------------

#define vec_cast_to_i32_i64_16(val)                        vec_loop_expr(vec_i32_32_t, 2, _tmp, _i, (_tmp).v[_i] = (val).v[_i];)
#define vec_cast_to_i32_i64_8(val)                         ( (vec_i32_16_t) { .v = (val).v } )
#define vec_cast_to_i32_i64_4(val)                         ( (vec_i32_8_t)  { .v = (val).v } )
#define vec_cast_to_i32_i64_2(val)                         ( (vec_i32_4_t)  { .v = (val).v } )
#define vec_cast_to_i32_i64_1(val)                         ( (vec_i32_2_t)  { .v = (val).v } )

#define vec_cast_to_i64_i64_16(val)                        (val)
#define vec_cast_to_i64_i64_8(val)                         (val)
#define vec_cast_to_i64_i64_4(val)                         (val)
#define vec_cast_to_i64_i64_2(val)                         (val)
#define vec_cast_to_i64_i64_1(val)                         (val)

#define vec_cast_to_f32_i64_16(val)                        vec_loop_expr(vec_f32_32_t, 2, _tmp, _i, (_tmp).v[_i] = (val).vf32[_i];)
#define vec_cast_to_f32_i64_8(val)                         ( (vec_f32_16_t) { .v = (val).vf32 } )
#define vec_cast_to_f32_i64_4(val)                         ( (vec_f32_8_t)  { .v = (val).vf32 } )
#define vec_cast_to_f32_i64_2(val)                         ( (vec_f32_4_t)  { .v = (val).vf32 } )
#define vec_cast_to_f32_i64_1(val)                         ( (vec_f32_2_t)  { .v = (val).vf32 } )

#define vec_cast_to_f64_i64_16(val)                        vec_loop_expr(vec_f64_16_t, 2, _tmp, _i, (_tmp).v[_i] = (val).vf64[_i];)
#define vec_cast_to_f64_i64_8(val)                         ( (vec_f64_8_t)  { .v = (val).vf64 } )
#define vec_cast_to_f64_i64_4(val)                         ( (vec_f64_4_t)  { .v = (val).vf64 } )
#define vec_cast_to_f64_i64_2(val)                         ( (vec_f64_2_t)  { .v = (val).vf64 } )
#define vec_cast_to_f64_i64_1(val)                         ( (vec_f64_1_t)  { .v = (val).vf64 } )


//------------------------------------------------------------------------------------------------------------------------------------------
//- Cast
//------------------------------------------------------------------------------------------------------------------------------------------

#define vec_cvt_to_i32_i64_16(val)                        vec_loop_expr(vec_i32_32_t, 2, _tmp, _i, (_tmp).v[_i] = _mm512_cvtepi64_epi32((val).v[_i]);)
#define vec_cvt_to_i32_i64_8(val)                         vec_i32_8( _mm512_cvtepi64_epi32((a).v) )
#define vec_cvt_to_i32_i64_4(val)                         vec_i32_4( _mm256_cvtepi64_epi32((a).v) )
#define vec_cvt_to_i32_i64_2(val)                         vec_i32_2( _mm_cvtepi64_epi32((a).v) )
#define vec_cvt_to_i32_i64_1(val)                         vec_i32_1( (int32_t) ((a).v) )

#define vec_cvt_to_i64_i64_16(val)                        (val)
#define vec_cvt_to_i64_i64_8(val)                         (val)
#define vec_cvt_to_i64_i64_4(val)                         (val)
#define vec_cvt_to_i64_i64_2(val)                         (val)
#define vec_cvt_to_i64_i64_1(val)                         (val)

#define vec_cvt_to_f32_i64_16(val)                        vec_loop_expr(vec_f32_32_t, 2, _tmp, _i, (_tmp).v[_i] = (val).vf32[_i];)
#define vec_cvt_to_f32_i64_8(val)                         ( (vec_f32_16_t) { .v = (val).vf32 } )
#define vec_cvt_to_f32_i64_4(val)                         ( (vec_f32_8_t)  { .v = (val).vf32 } )
#define vec_cvt_to_f32_i64_2(val)                         ( (vec_f32_4_t)  { .v = (val).vf32 } )
#define vec_cvt_to_f32_i64_1(val)                         ( (vec_f32_2_t)  { .v = (val).vf32 } )

#define vec_cvt_to_f64_i64_16(val)                        vec_loop_expr(vec_f64_16_t, 2, _tmp, _i, (_tmp).v[_i] = (val).vf64[_i];)
#define vec_cvt_to_f64_i64_8(val)                         ( (vec_f64_8_t)  { .v = (val).vf64 } )
#define vec_cvt_to_f64_i64_4(val)                         ( (vec_f64_4_t)  { .v = (val).vf64 } )
#define vec_cvt_to_f64_i64_2(val)                         ( (vec_f64_2_t)  { .v = (val).vf64 } )
#define vec_cvt_to_f64_i64_1(val)                         ( (vec_f64_1_t)  { .v = (val).vf64 } )


//------------------------------------------------------------------------------------------------------------------------------------------
//- Set - Load - Store
//------------------------------------------------------------------------------------------------------------------------------------------

#define vec_elem_get_i64_16(vec, index)                    ((vec).s[index])
#define vec_elem_get_i64_8(vec, index)                     ((vec).s[index])
#define vec_elem_get_i64_4(vec, index)                     ((vec).s[index])
#define vec_elem_get_i64_2(vec, index)                     ((vec).s[index])
#define vec_elem_get_i64_1(vec, index)                     ((vec).s[index])

#define vec_elem_get_p64_16(vec, index)                    ((vec).s[index])
#define vec_elem_get_p64_8(vec, index)                     ((vec).s[index])
#define vec_elem_get_p64_4(vec, index)                     ((vec).s[index])
#define vec_elem_get_p64_2(vec, index)                     ((vec).s[index])
#define vec_elem_get_p64_1(vec, index)                     ((vec).s[index])

#define vec_elem_set_i64_16(vec, index, expr)              { (vec).s[index] = (expr); } while (0)
#define vec_elem_set_i64_8(vec, index, expr)               { (vec).s[index] = (expr); } while (0)
#define vec_elem_set_i64_4(vec, index, expr)               { (vec).s[index] = (expr); } while (0)
#define vec_elem_set_i64_2(vec, index, expr)               { (vec).s[index] = (expr); } while (0)
#define vec_elem_set_i64_1(vec, index, expr)               { (vec).s[index] = (expr); } while (0)

#define vec_elem_set_p64_16(vec, index, expr)              { (vec).s[index] = (expr); } while (0)
#define vec_elem_set_p64_8(vec, index, expr)               { (vec).s[index] = (expr); } while (0)
#define vec_elem_set_p64_4(vec, index, expr)               { (vec).s[index] = (expr); } while (0)
#define vec_elem_set_p64_2(vec, index, expr)               { (vec).s[index] = (expr); } while (0)
#define vec_elem_set_p64_1(vec, index, expr)               { (vec).s[index] = (expr); } while (0)

#define vec_array_i64_16(vec)                              (vec).s
#define vec_array_i64_8(vec)                               (vec).s
#define vec_array_i64_4(vec)                               (vec).s
#define vec_array_i64_2(vec)                               (vec).s
#define vec_array_i64_1(vec)                               (vec).s

#define vec_set1_i64_16(val)                               vec_loop_expr(vec_i64_16_t, 2, _tmp, _i, (_tmp).v[_i] = _mm512_set1_epi64(val);)
#define vec_set1_i64_8(val)                                vec_i64_8( _mm512_set1_epi64(val) )
#define vec_set1_i64_4(val)                                vec_i64_4( _mm256_set1_epi64x(val) )
#define vec_set1_i64_2(val)                                vec_i64_2( _mm_set1_epi64x(val) )
#define vec_set1_i64_1(val)                                vec_i64_1( (int64_t) (val) )

#define vec_set_iter_i64_16(iter, expr)                    vec_loop_expr(vec_i64_16_t, 2, _tmp, _i, (_tmp).v[_i] = _mm512_set_epi64(vec_iter_expr_8(iter, 8*_i, (expr)));)
#define vec_set_iter_i64_8(iter, expr)                     vec_i64_8( _mm512_set_epi64(vec_iter_expr_8(iter, 0, (expr))) )
#define vec_set_iter_i64_4(iter, expr)                     vec_i64_4( _mm256_set_epi64x(vec_iter_expr_4(iter, 0, (expr))) )
#define vec_set_iter_i64_2(iter, expr)                     vec_i64_2( _mm_set_epi64x(vec_iter_expr_2(iter, 0, (expr))) )
#define vec_set_iter_i64_1(iter, expr)                     vec_i64_1( (int64_t) vec_iter_expr_1(iter, 0, (expr)) )

#define vec_set_iter_p64_16(iter, expr)                    vec_loop_expr(vec_perm_p64_16_t, 2, _tmp, _i, (_tmp).v[_i] = _mm512_set_epi64(vec_iter_expr_8(iter, 8*_i, (expr)));)
#define vec_set_iter_p64_8(iter, expr)                     vec_perm_p64_8( _mm512_set_epi64(vec_iter_expr_8(iter, 0, (expr))) )
#define vec_set_iter_p64_4(iter, expr)                     vec_perm_p64_4( _mm256_set_epi64x(vec_iter_expr_4(iter, 0, (expr))) )
#define vec_set_iter_p64_2(iter, expr)                     vec_loop_expr(vec_perm_p64_2_t,   2, _tmp, iter, _tmp.s[iter] = ((expr) << 1);)    /* _mm_permutevar_pd: How paranoid must one be to use the second bit for selection??? FML. */
#define vec_set_iter_p64_1(iter, expr)                     vec_perm_p64_1( (int64_t) vec_iter_expr_1(iter, 0, (expr)) ) 

#define vec_loadu_i64_16(ptr)                              vec_loop_expr(vec_i64_16_t, 2, _tmp, _i, (_tmp).v[_i] = _mm512_loadu_si512(((__m512i *) (ptr)) + _i);)
#define vec_loadu_i64_8(ptr)                               vec_i64_8( _mm512_loadu_si512((__m512i *) (ptr)) )
#define vec_loadu_i64_4(ptr)                               vec_i64_4( _mm256_loadu_si256((__m256i *) (ptr)) )
#define vec_loadu_i64_2(ptr)                               vec_i64_2( _mm_loadu_si128((__m128i *) (ptr)) )
#define vec_loadu_i64_1(ptr)                               vec_i64_1( (*((int64_t *) (ptr))) )

#define vec_loadu_p64_16(ptr)                              vec_loop_expr(vec_i64_16_t, 2, _tmp, _i, (_tmp).v[_i] = _mm512_loadu_si512(((__m512i *) (ptr)) + _i);)
#define vec_loadu_p64_8(ptr)                               vec_i64_8( _mm512_loadu_si512((__m512i *) (ptr)) )
#define vec_loadu_p64_4(ptr)                               vec_i64_4( _mm256_loadu_si256((__m256i *) (ptr)) )
#define vec_loadu_p64_2(ptr)                               vec_i64_2( _mm_loadu_si128((__m128i *) (ptr)) )
#define vec_loadu_p64_1(ptr)                               vec_i64_1( (*((int64_t *) (ptr))) )

#define vec_loadu_maskedz_i64_16(ptr, mask)                vec_loop_expr(vec_i64_16_t, 2, _tmp, _i, (_tmp).v[_i] = _mm512_maskz_loadu_epi64((mask).v[_i], ((__m512i *) (ptr)) + _i);)
#define vec_loadu_maskedz_i64_8(ptr, mask)                 vec_i64_8( _mm512_maskz_loadu_epi64((mask).v, (__m512i *) (ptr)) )
#define vec_loadu_maskedz_i64_4(ptr, mask)                 vec_i64_4( _mm256_maskz_loadu_epi64((mask).v, (__m256i *) (ptr)) )
#define vec_loadu_maskedz_i64_2(ptr, mask)                 vec_i64_2( _mm_maskz_loadu_epi64((mask).v, (__m128i *) (ptr)) )
#define vec_loadu_maskedz_i64_1(ptr, mask)                 vec_i64_1( ((mask).v) ? (*((int64_t *) (ptr))) : 0 )

#define vec_storeu_i64_16(ptr, vec)                        vec_loop_stmt(2, _i, _mm512_storeu_si512(((__m512i *) (ptr)) + _i, (vec).v[_i]);)
#define vec_storeu_i64_8(ptr, vec)                         _mm512_storeu_si512((__m512i *) (ptr), (vec).v)
#define vec_storeu_i64_4(ptr, vec)                         _mm256_storeu_si256((__m256i *) (ptr), (vec).v)
#define vec_storeu_i64_2(ptr, vec)                         _mm_storeu_si128((__m128i *) (ptr), (vec).v)
#define vec_storeu_i64_1(ptr, vec)                         do { (*((int64_t *) (ptr))) = ((vec).v); } while (0)

#define vec_storeu_p64_16(ptr, vec)                        vec_loop_stmt(2, _i, _mm512_storeu_si512(((__m512i *) (ptr)) + _i, (vec).v[_i]);)
#define vec_storeu_p64_8(ptr, vec)                         _mm512_storeu_si512((__m512i *) (ptr), (vec).v)
#define vec_storeu_p64_4(ptr, vec)                         _mm256_storeu_si256((__m256i *) (ptr), (vec).v)
#define vec_storeu_p64_2(ptr, vec)                         _mm_storeu_si128((__m128i *) (ptr), (vec).v)
#define vec_storeu_p64_1(ptr, vec)                         do { (*((int64_t *) (ptr))) = ((vec).v); } while (0)

#define vec_storeu_masked_i64_16(ptr, vec, mask)           vec_loop_stmt(2, _i, _mm512_mask_storeu_epi64(((__m512i *) (ptr)) + _i, (mask).v[_i], (vec).v[_i]);)
#define vec_storeu_masked_i64_8(ptr, vec, mask)            _mm512_mask_storeu_epi64((__m512i *) (ptr), (mask).v, (vec).v)
#define vec_storeu_masked_i64_4(ptr, vec, mask)            _mm256_mask_storeu_epi64((__m256i *) (ptr), (mask).v, (vec).v)
#define vec_storeu_masked_i64_2(ptr, vec, mask)            _mm_mask_storeu_epi64((__m128i *) (ptr), (mask).v, (vec).v)
#define vec_storeu_masked_i64_1(ptr, vec, mask)            do { if ((mask).v) (*((int64_t *) (ptr))) = ((vec).v); } while (0)

#define vec_gather_i64_i32_16(ptr, idx)                    vec_loop_expr(vec_i64_16_t, 4, _tmp, _i, (_tmp).v[_i] = _mm512_loadu_si512(((__m512i *) (ptr)) + _i);)
#define vec_gather_i64_i32_8(ptr, idx)                     vec_i64_8( _mm512_i32gather_epi64((idx).v, (long long const *) (ptr), 8) )
#define vec_gather_i64_i32_4(ptr, idx)                     vec_i64_4( _mm256_i32gather_epi64((long long const *) (ptr), (idx).v, 8) )
#define vec_gather_i64_i32_2(ptr, idx)                     vec_i64_2( _mm_i32gather_epi64((long long const *) (ptr), (idx).v, 8) )
#define vec_gather_i64_i32_1(ptr, idx)                     vec_i64_1( ((int64_t *) (ptr))[(idx).v] )

#define vec_gather_i64_i64_16(ptr, idx)                    vec_loop_expr(vec_i64_16_t, 4, _tmp, _i, (_tmp).v[_i] = _mm512_loadu_si512(((__m512i *) (ptr)) + _i);)
#define vec_gather_i64_i64_8(ptr, idx)                     vec_i64_8( _mm512_i64gather_epi64((idx).v, (long long const *) (ptr), 8) )  /* Arguments in different order for avx512! Why? */
#define vec_gather_i64_i64_4(ptr, idx)                     vec_i64_4( _mm256_i64gather_epi64((long long const *) (ptr), (idx).v, 8) )
#define vec_gather_i64_i64_2(ptr, idx)                     vec_i64_2( _mm_i64gather_epi64((long long const *) (ptr), (idx).v, 8) )
#define vec_gather_i64_i64_1(ptr, idx)                     vec_i64_1( ((int64_t *) (ptr))[((idx)).v] )


//------------------------------------------------------------------------------------------------------------------------------------------
//- Operations
//------------------------------------------------------------------------------------------------------------------------------------------

#define vec_add_i64_16(a, b)                               vec_loop_expr(vec_i64_16_t, 2, _tmp, _i, (_tmp).v[_i] = _mm512_add_epi64((a).v[_i], (b).v[_i]);)
#define vec_add_i64_8(a, b)                                vec_i64_8( _mm512_add_epi64((a).v, (b).v) )
#define vec_add_i64_4(a, b)                                vec_i64_4( _mm256_add_epi64((a).v, (b).v) )
#define vec_add_i64_2(a, b)                                vec_i64_2( _mm_add_epi64((a).v, (b).v) )
#define vec_add_i64_1(a, b)                                vec_i64_1( ((a).v + (b).v) )

#define vec_sub_i64_16(a, b)                               vec_loop_expr(vec_i64_16_t, 2, _tmp, _i, (_tmp).v[_i] = _mm512_sub_epi64((a).v[_i], (b).v[_i]);)
#define vec_sub_i64_8(a, b)                                vec_i64_8( _mm512_sub_epi64((a).v, (b).v) )
#define vec_sub_i64_4(a, b)                                vec_i64_4( _mm256_sub_epi64((a).v, (b).v) )
#define vec_sub_i64_2(a, b)                                vec_i64_2( _mm_sub_epi64((a).v, (b).v) )
#define vec_sub_i64_1(a, b)                                vec_i64_1( ((a).v - (b).v) )

#define vec_mul_i64_16(a, b)                               vec_loop_expr(vec_i64_16_t, 2, _tmp, _i, (_tmp).v[_i] = _mm512_mullo_epi64((a).v[_i], (b).v[_i]);)
#define vec_mul_i64_8(a, b)                                vec_i64_8( _mm512_mullo_epi64((a).v, (b).v) )
#define vec_mul_i64_4(a, b)                                vec_i64_4( _mm256_mullo_epi64((a).v, (b).v) )
#define vec_mul_i64_2(a, b)                                vec_i64_2( _mm_mullo_epi64((a).v, (b).v) )
#define vec_mul_i64_1(a, b)                                vec_i64_1( ((a).v * (b).v) )

#define vec_div_i64_16(a, b)                               vec_set_iter_i64_16(_j, a.si[_j]/b.si[_j])
#define vec_div_i64_8(a, b)                                vec_set_iter_i64_8(_j, a.si[_j]/b.si[_j])
#define vec_div_i64_4(a, b)                                vec_set_iter_i64_4(_j, a.si[_j]/b.si[_j])
#define vec_div_i64_2(a, b)                                vec_set_iter_i64_2(_j, a.si[_j]/b.si[_j])
#define vec_div_i64_1(a, b)                                vec_i64_1( ((a).v / (b).v) )

#define vec_and_i64_16(a, b)                               vec_loop_expr(vec_i64_16_t, 2, _tmp, _i, (_tmp).v[_i] = _mm512_and_si512((a).v[_i], (b).v[_i]);)
#define vec_and_i64_8(a, b)                                vec_i64_8( _mm512_and_si512((a).v, (b).v) )
#define vec_and_i64_4(a, b)                                vec_i64_4( _mm256_and_si256((a).v, (b).v) )
#define vec_and_i64_2(a, b)                                vec_i64_2( _mm_and_si128((a).v, (b).v) )
#define vec_and_i64_1(a, b)                                vec_i64_1( ((a).v & (b).v) )

#define vec_or_i64_16(a, b)                                vec_loop_expr(vec_i64_16_t, 2, _tmp, _i, (_tmp).v[_i] = _mm512_or_si512((a).v[_i], (b).v[_i]);)
#define vec_or_i64_8(a, b)                                 vec_i64_8( _mm512_or_si512((a).v, (b).v) )
#define vec_or_i64_4(a, b)                                 vec_i64_4( _mm256_or_si256((a).v, (b).v) )
#define vec_or_i64_2(a, b)                                 vec_i64_2( _mm_or_si128((a).v, (b).v) )
#define vec_or_i64_1(a, b)                                 vec_i64_1( ((a).v | (b).v) )

#define vec_not_i64_16(a)                                  vec_loop_expr(vec_i64_16_t, 2, _tmp, _i, (_tmp).v[_i] = _mm512_xor_si512((a).v[_i], _mm512_set1_epi64(-1LL));)
#define vec_not_i64_8(a)                                   vec_i64_8( _mm512_xor_si512((a).v, _mm512_set1_epi64(-1LL)) )
#define vec_not_i64_4(a)                                   vec_i64_4( _mm256_xor_si256((a).v, _mm256_set1_epi64x(-1LL)) )
#define vec_not_i64_2(a)                                   vec_i64_2( _mm_xor_si128((a).v, _mm_set1_epi64x(-1LL)) )
#define vec_not_i64_1(a)                                   vec_i64_1( ((a).v ^ -1) )

#define vec_xor_i64_16(a, b)                               vec_loop_expr(vec_i64_16_t, 2, _tmp, _i, (_tmp).v[_i] = _mm512_xor_si512((a).v[_i], (b).v[_i]);)
#define vec_xor_i64_8(a, b)                                vec_i64_8( _mm512_xor_si512((a).v, (b).v) )
#define vec_xor_i64_4(a, b)                                vec_i64_4( _mm256_xor_si256((a).v, (b).v) )
#define vec_xor_i64_2(a, b)                                vec_i64_2( _mm_xor_si128((a).v, (b).v) )
#define vec_xor_i64_1(a, b)                                vec_i64_1( ((a).v ^ (b).v) )


//------------------------------------------------------------------------------------------------------------------------------------------
//- Shift
//------------------------------------------------------------------------------------------------------------------------------------------

#define vec_slli_i64_16(a, imm8)                           vec_loop_expr(vec_i64_16_t, 2, _tmp, _i, (_tmp).v[_i] = _mm512_slli_epi64((a).v[_i], imm8);)
#define vec_slli_i64_8(a, imm8)                            vec_i64_8( _mm512_slli_epi64((a).v, imm8) )
#define vec_slli_i64_4(a, imm8)                            vec_i64_4( _mm256_slli_epi64((a).v, imm8) )
#define vec_slli_i64_2(a, imm8)                            vec_i64_2( _mm_slli_epi64((a).v, imm8) )
#define vec_slli_i64_1(a, imm8)                            vec_i64_1( (int64_t) ((imm8 < 64) ? ((uint64_t) (a).v)<<imm8 : 0) )

#define vec_srli_i64_16(a, imm8)                           vec_loop_expr(vec_i64_16_t, 2, _tmp, _i, (_tmp).v[_i] = _mm512_srli_epi64((a).v[_i], imm8);)
#define vec_srli_i64_8(a, imm8)                            vec_i64_8( _mm512_srli_epi64((a).v, imm8) )
#define vec_srli_i64_4(a, imm8)                            vec_i64_4( _mm256_srli_epi64((a).v, imm8) )
#define vec_srli_i64_2(a, imm8)                            vec_i64_2( _mm_srli_epi64((a).v, imm8) )
#define vec_srli_i64_1(a, imm8)                            vec_i64_1( (int64_t) ((imm8 < 64) ? ((uint64_t) (a).v)>>imm8 : 0) )

#define vec_srai_i64_16(a, imm8)                           vec_loop_expr(vec_i64_16_t, 2, _tmp, _i, (_tmp).v[_i] = _mm512_srai_epi64((a).v[_i], imm8);)
#define vec_srai_i64_8(a, imm8)                            vec_i64_8( _mm512_srai_epi64((a).v, imm8) )
#define vec_srai_i64_4(a, imm8)                            vec_i64_4( _mm256_srai_epi64((a).v, imm8) )
#define vec_srai_i64_2(a, imm8)                            vec_i64_2( _mm_srai_epi64((a).v, imm8) )
#define vec_srai_i64_1(a, imm8)                            vec_set_iter_i64_1(_j, vec_srai_i64_s(a.si[_j], imm8))
#define vec_srai_i64_s(a, imm8)                            ((imm8 < 64) ? ((int64_t) a)>>imm8 : (((int64_t) a) < 0) ? -1 : 0)

#define vec_sllv_i64_16(a, count)                          vec_loop_expr(vec_i64_16_t, 2, _tmp, _i, (_tmp).v[_i] = _mm512_sllv_epi64((a).v[_i], (count).v[_i]);)
#define vec_sllv_i64_8(a, count)                           vec_i64_8( _mm512_sllv_epi64((a).v, (count).v) )
#define vec_sllv_i64_4(a, count)                           vec_i64_4( _mm256_sllv_epi64((a).v, (count).v) )
#define vec_sllv_i64_2(a, count)                           vec_i64_2( _mm_sllv_epi64((a).v, (count).v) )
#define vec_sllv_i64_1(a, count)                           vec_i64_1( (int64_t) (((count).v < 64) ? ((uint64_t) (a).v)<<(count).v : 0) )

#define vec_srlv_i64_16(a, count)                          vec_loop_expr(vec_i64_16_t, 2, _tmp, _i, (_tmp).v[_i] = _mm512_srlv_epi64((a).v[_i], (count).v[_i]);)
#define vec_srlv_i64_8(a, count)                           vec_i64_8( _mm512_srlv_epi64((a).v, (count).v) )
#define vec_srlv_i64_4(a, count)                           vec_i64_4( _mm256_srlv_epi64((a).v, (count).v) )
#define vec_srlv_i64_2(a, count)                           vec_i64_2( _mm_srlv_epi64((a).v, (count).v) )
#define vec_srlv_i64_1(a, count)                           vec_i64_1( (int64_t) (((count).v < 64) ? ((uint64_t) (a).v)>>(count).v : 0) )

#define vec_srav_i64_16(a, count)                          vec_loop_expr(vec_i64_16_t, 2, _tmp, _i, (_tmp).v[_i] = _mm512_srav_epi64((a).v[_i], (count).v[_i]);)
#define vec_srav_i64_8(a, count)                           vec_i64_8( _mm512_srav_epi64((a).v, (count).v) )
#define vec_srav_i64_4(a, count)                           vec_i64_4( _mm256_srav_epi64((a).v, (count).v) )
#define vec_srav_i64_2(a, count)                           vec_i64_2( _mm_srav_epi64((a).v, (count).v) )
#define vec_srav_i64_1(a, count)                           vec_set_iter_i64_1(_j, vec_srav_i64_s(a.si[_j], count.si[_j]))
#define vec_srav_i64_s(a, count)                           ((count < 64) ? ((int64_t) a)>>count : (((int64_t) a) < 0) ? -1 : 0)


//------------------------------------------------------------------------------------------------------------------------------------------
//- Compare
//------------------------------------------------------------------------------------------------------------------------------------------

#define vec_cmpeq_i64_16(a, b)                             vec_loop_expr(vec_mask_m64_16_t, 2, _tmp, _i, (_tmp).v[_i] = _mm512_cmpeq_epi64_mask((a).v[_i], (b).v[_i]);)
#define vec_cmpeq_i64_8(a, b)                              vec_mask_m64_8( _mm512_cmpeq_epi64_mask((a).v, (b).v) )
#define vec_cmpeq_i64_4(a, b)                              vec_mask_m64_4( _mm256_cmpeq_epi64_mask((a).v, (b).v) )
#define vec_cmpeq_i64_2(a, b)                              vec_mask_m64_2( _mm_cmpeq_epi64_mask((a).v, (b).v) )
#define vec_cmpeq_i64_1(a, b)                              vec_mask_m64_1( (__mmask8) ((a).v == (b).v) )

#define vec_cmpgt_i64_16(a, b)                             vec_loop_expr(vec_mask_m64_16_t, 2, _tmp, _i, (_tmp).v[_i] = _mm512_cmpgt_epi64_mask((a).v[_i], (b).v[_i]);)
#define vec_cmpgt_i64_8(a, b)                              vec_mask_m64_8( _mm512_cmpgt_epi64_mask((a).v, (b).v) )
#define vec_cmpgt_i64_4(a, b)                              vec_mask_m64_4( _mm256_cmpgt_epi64_mask((a).v, (b).v) )
#define vec_cmpgt_i64_2(a, b)                              vec_mask_m64_2( _mm_cmpgt_epi64_mask((a).v, (b).v) )
#define vec_cmpgt_i64_1(a, b)                              vec_mask_m64_1( (__mmask8) ((a).v > (b).v) )


//------------------------------------------------------------------------------------------------------------------------------------------
//- Shuffle - Permute
//------------------------------------------------------------------------------------------------------------------------------------------

#define vec_permute_i64_16(a, idx)                         vec_loop_expr(vec_i64_16_t, 16, _tmp, _i, _tmp.s[_i] = a.s[idx.s[_i]];)
#define vec_permute_i64_8(a, idx)                          vec_i64_8( _mm512_permutexvar_epi64((idx).v, (a).v) )
#define vec_permute_i64_4(a, idx)                          vec_i64_4( _mm256_permutexvar_epi64((idx).v, (a).v) )
#define vec_permute_i64_2(a, idx)                          vec_i64_2( _mm_castpd_si128(_mm_permutevar_pd(_mm_castsi128_pd((a).v), (idx).v)) )
#define vec_permute_i64_1(a, idx)                          a


#endif /* VECTORIZATION_X86_AVX512_I64_H */

