#ifndef VECTORIZATION_X86_AVX256_MASK32_H
#define VECTORIZATION_X86_AVX256_MASK32_H

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <x86intrin.h>

#include "macros/cpp_defines.h"
#include "macros/macrolib.h"
#include "bit_ops.h"

#include "vectorization/vectorization_util.h"


typedef union __attribute__((packed, aligned(4))) { int32_t v;    float  vf;    }  vec_mask_m32_1_t;
typedef union __attribute__((packed, aligned(4))) { __m128i v;    __m128 vf;    }  vec_mask_m32_4_t;
typedef union __attribute__((packed, aligned(4))) { __m256i v;    __m256 vf;    }  vec_mask_m32_8_t;
typedef union __attribute__((packed, aligned(4))) { __m256i v[2]; __m256 vf[2]; }  vec_mask_m32_16_t;
typedef union __attribute__((packed, aligned(4))) { __m256i v[4]; __m256 vf[4]; }  vec_mask_m32_32_t;

typedef uint32_t vec_mask_packed_m32_1_t;
typedef uint32_t vec_mask_packed_m32_4_t;
typedef uint32_t vec_mask_packed_m32_8_t;
typedef uint32_t vec_mask_packed_m32_16_t;
typedef uint32_t vec_mask_packed_m32_32_t;


#define vec_mask_m32_8(val)                                ( (vec_mask_m32_8_t)  { .v = (val) } )
#define vec_mask_m32_4(val)                                ( (vec_mask_m32_4_t)  { .v = (val) } )
#define vec_mask_m32_1(val)                                ( (vec_mask_m32_1_t)  { .v = (val) } )


//------------------------------------------------------------------------------------------------------------------------------------------
//- Set - Load - Store
//------------------------------------------------------------------------------------------------------------------------------------------

#define vec_mask_pack_m32_32(a)                            vec_loop_expr_init(uint32_t, 4, _tmp, 0, _i, _tmp |= (uint32_t) _mm256_movemask_ps((a).vf[_i]) << (_i * 8);)
#define vec_mask_pack_m32_16(a)                            vec_loop_expr_init(uint32_t, 2, _tmp, 0, _i, _tmp |= (uint32_t) _mm256_movemask_ps((a).vf[_i]) << (_i * 8);)
#define vec_mask_pack_m32_8(a)                             ((uint32_t) _mm256_movemask_ps((a).vf))
#define vec_mask_pack_m32_4(a)                             ((uint32_t) _mm_movemask_ps((a).vf))
#define vec_mask_pack_m32_1(a)                             ((uint32_t) (a).v)

#define vec_mask_packed_get_bit_m32_32(a, pos)             bits_u32_extract(a, pos, 1)
#define vec_mask_packed_get_bit_m32_16(a, pos)             bits_u32_extract(a, pos, 1)
#define vec_mask_packed_get_bit_m32_8(a, pos)              bits_u32_extract(a, pos, 1)
#define vec_mask_packed_get_bit_m32_4(a, pos)              bits_u32_extract(a, pos, 1)
#define vec_mask_packed_get_bit_m32_1(a, pos)              bits_u32_extract(a, pos, 1)

#define vec_mask_loadu_packed_m32_32(ptr)                  vec_loop_expr(vec_mask_m32_32_t, 4, _tmp, _i, (_tmp).v[_i] =  _mm256_set_epi32(vec_iter_expr_8(_j, 0, ((uint32_t *) ptr)[0] & (1 << (8*_i+_j)) ? -1 : 0));)
#define vec_mask_loadu_packed_m32_16(ptr)                  vec_loop_expr(vec_mask_m32_16_t, 2, _tmp, _i, (_tmp).v[_i] =  _mm256_set_epi32(vec_iter_expr_8(_j, 0, ((uint32_t *) ptr)[0] & (1 << (8*_i+_j)) ? -1 : 0));)
#define vec_mask_loadu_packed_m32_8(ptr)                   vec_mask_m32_8( _mm256_set_epi32(vec_iter_expr_8(_j, 0, ((uint32_t *) ptr)[0] & (1 << _j) ? -1 : 0)) )
#define vec_mask_loadu_packed_m32_4(ptr)                   vec_mask_m32_4( _mm_set_epi32(vec_iter_expr_4(_j, 0, ((uint32_t *) ptr)[0] & (1 << _j) ? -1 : 0)) )
#define vec_mask_loadu_packed_m32_1(ptr)                   vec_mask_m32_1( vec_iter_expr_1(_j, 0, (((uint32_t *) ptr)[0] & (1 << _j)) ? -1 : 0) )

#define vec_mask_whilelt_m32_32(i, N)                      vec_loop_expr(vec_mask_m32_32_t, 4, _tmp, _i, (_tmp).v[_i] = _mm256_cmpgt_epi32( _mm256_set1_epi32(N-i), _mm256_set_epi32(vec_iter_expr_8(_j, 4*_i, _j)) );)
#define vec_mask_whilelt_m32_16(i, N)                      vec_loop_expr(vec_mask_m32_16_t, 2, _tmp, _i, (_tmp).v[_i] = _mm256_cmpgt_epi32( _mm256_set1_epi32(N-i), _mm256_set_epi32(vec_iter_expr_8(_j, 4*_i, _j)) );)
#define vec_mask_whilelt_m32_8(i, N)                       vec_mask_m32_8( _mm256_cmpgt_epi32( _mm256_set1_epi32(N-i), _mm256_set_epi32(vec_iter_expr_8(_j, 0, _j)) ) )
#define vec_mask_whilelt_m32_4(i, N)                       vec_mask_m32_4( _mm_cmpgt_epi32( _mm_set1_epi32(N-i), _mm_set_epi32(vec_iter_expr_4(_j, 0, _j)) ) )
#define vec_mask_whilelt_m32_1(i, N)                       vec_mask_m32_1( (i < N) ? -1 : 0 )

#define vec_mask_firstN_m32_32(N)                          vec_loop_expr(vec_mask_m32_32_t, 4, _tmp, _i, (_tmp).v[_i] = _mm256_cmpgt_epi32( _mm256_set1_epi32(N), _mm256_set_epi32(vec_iter_expr_8(_j, 4*_i, _j)) );)
#define vec_mask_firstN_m32_16(N)                          vec_loop_expr(vec_mask_m32_16_t, 2, _tmp, _i, (_tmp).v[_i] = _mm256_cmpgt_epi32( _mm256_set1_epi32(N), _mm256_set_epi32(vec_iter_expr_8(_j, 4*_i, _j)) );)
#define vec_mask_firstN_m32_8(N)                           vec_mask_m32_8( _mm256_cmpgt_epi32( _mm256_set1_epi32(N), _mm256_set_epi32(vec_iter_expr_8(_j, 0, _j)) ) )
#define vec_mask_firstN_m32_4(N)                           vec_mask_m32_4( _mm_cmpgt_epi32( _mm_set1_epi32(N), _mm_set_epi32(vec_iter_expr_4(_j, 0, _j)) ) )
#define vec_mask_firstN_m32_1(N)                           vec_mask_m32_1( (N > 0) ? -1 : 0 )


//------------------------------------------------------------------------------------------------------------------------------------------
//- Operations
//------------------------------------------------------------------------------------------------------------------------------------------

#define vec_and_m32_32(a, b)                               vec_loop_expr(vec_mask_m32_32_t, 4, _tmp, _i, (_tmp).v[_i] = _mm256_and_si256((a).v[_i], (b).v[_i]);)
#define vec_and_m32_16(a, b)                               vec_loop_expr(vec_mask_m32_16_t, 2, _tmp, _i, (_tmp).v[_i] = _mm256_and_si256((a).v[_i], (b).v[_i]);)
#define vec_and_m32_8(a, b)                                vec_mask_m32_8( _mm256_and_si256((a).v, (b).v) )
#define vec_and_m32_4(a, b)                                vec_mask_m32_4( _mm_and_si128((a).v, (b).v) )
#define vec_and_m32_1(a, b)                                vec_mask_m32_1( ((a).v & (b).v) )

#define vec_or_m32_32(a, b)                                vec_loop_expr(vec_mask_m32_32_t, 4, _tmp, _i, (_tmp).v[_i] = _mm256_or_si256((a).v[_i], (b).v[_i]);)
#define vec_or_m32_16(a, b)                                vec_loop_expr(vec_mask_m32_16_t, 2, _tmp, _i, (_tmp).v[_i] = _mm256_or_si256((a).v[_i], (b).v[_i]);)
#define vec_or_m32_8(a, b)                                 vec_mask_m32_8( _mm256_or_si256((a).v, (b).v) )
#define vec_or_m32_4(a, b)                                 vec_mask_m32_4( _mm_or_si128((a).v, (b).v) )
#define vec_or_m32_1(a, b)                                 vec_mask_m32_1( ((a).v | (b).v) )

#define vec_not_m32_32(a)                                  vec_loop_expr(vec_mask_m32_32_t, 4, _tmp, _i, (_tmp).v[_i] = _mm256_xor_si256((a).v[_i], _mm256_set1_epi32(-1));)
#define vec_not_m32_16(a)                                  vec_loop_expr(vec_mask_m32_16_t, 2, _tmp, _i, (_tmp).v[_i] = _mm256_xor_si256((a).v[_i], _mm256_set1_epi32(-1));)
#define vec_not_m32_8(a)                                   vec_mask_m32_8( _mm256_xor_si256((a).v, _mm256_set1_epi32(-1)) )
#define vec_not_m32_4(a)                                   vec_mask_m32_4( _mm_xor_si128((a).v, _mm_set1_epi32(-1)) )
#define vec_not_m32_1(a)                                   vec_mask_m32_1( ((a).v ^ 1) )

#define vec_xor_m32_32(a, b)                               vec_loop_expr(vec_mask_m32_32_t, 4, _tmp, _i, (_tmp).v[_i] = _mm256_xor_si256((a).v[_i], (b).v[_i]);)
#define vec_xor_m32_16(a, b)                               vec_loop_expr(vec_mask_m32_16_t, 2, _tmp, _i, (_tmp).v[_i] = _mm256_xor_si256((a).v[_i], (b).v[_i]);)
#define vec_xor_m32_8(a, b)                                vec_mask_m32_8( _mm256_xor_si256((a).v, (b).v) )
#define vec_xor_m32_4(a, b)                                vec_mask_m32_4( _mm_xor_si128((a).v, (b).v) )
#define vec_xor_m32_1(a, b)                                vec_mask_m32_1( ((a).v ^ (b).v) )


#endif /* VECTORIZATION_X86_AVX256_MASK32_H */

