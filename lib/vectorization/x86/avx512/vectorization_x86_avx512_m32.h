#ifndef VECTORIZATION_X86_AVX512_M32_H
#define VECTORIZATION_X86_AVX512_M32_H

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <x86intrin.h>

#include "macros/cpp_defines.h"
#include "macros/macrolib.h"
#include "bit_ops.h"

#include "vectorization/vectorization_util.h"


typedef union __attribute__((packed)) { __mmask8  v;    }  vec_mask_m32_1_t;
typedef union __attribute__((packed)) { __mmask8  v;    }  vec_mask_m32_4_t;
typedef union __attribute__((packed)) { __mmask8  v;    }  vec_mask_m32_8_t;
typedef union __attribute__((packed)) { __mmask16 v;    }  vec_mask_m32_16_t;
typedef union __attribute__((packed)) { __mmask16 v[2]; }  vec_mask_m32_32_t;

typedef uint8_t  vec_mask_packed_m32_1_t;
typedef uint8_t  vec_mask_packed_m32_4_t;
typedef uint8_t  vec_mask_packed_m32_8_t;
typedef uint16_t vec_mask_packed_m32_16_t;
typedef uint32_t vec_mask_packed_m32_32_t;


#define vec_mask_m32_16(val)                               ( (vec_mask_m32_16_t) { .v = (val) } )
#define vec_mask_m32_8(val)                                ( (vec_mask_m32_8_t)  { .v = (val) } )
#define vec_mask_m32_4(val)                                ( (vec_mask_m32_4_t)  { .v = (val) } )
#define vec_mask_m32_1(val)                                ( (vec_mask_m32_1_t)  { .v = (val) } )


//------------------------------------------------------------------------------------------------------------------------------------------
//- Set - Load - Store
//------------------------------------------------------------------------------------------------------------------------------------------

#define vec_mask_pack_m32_32(a)                            vec_loop_expr_init(vec_mask_packed_m32_32_t, 2, _tmp, 0, _i, _tmp |= ((vec_mask_packed_m32_32_t) (a).v[_i]) << (_i * 16);)
#define vec_mask_pack_m32_16(a)                            ((a).v)
#define vec_mask_pack_m32_8(a)                             ((a).v)
#define vec_mask_pack_m32_4(a)                             ((a).v)
#define vec_mask_pack_m32_1(a)                             ((a).v)

#define vec_mask_packed_get_bit_m32_32(a, pos)             bits_u32_extract(a, pos, 1)
#define vec_mask_packed_get_bit_m32_16(a, pos)             bits_u32_extract(a, pos, 1)
#define vec_mask_packed_get_bit_m32_8(a, pos)              bits_u32_extract(a, pos, 1)
#define vec_mask_packed_get_bit_m32_4(a, pos)              bits_u32_extract(a, pos, 1)
#define vec_mask_packed_get_bit_m32_1(a, pos)              bits_u32_extract(a, pos, 1)

#define vec_mask_loadu_packed_m32_32(ptr)                  vec_loop_expr(vec_mask_m32_32_t, 2, _tmp, _i, (_tmp).v[_i] = _cvtu32_mask16(((uint32_t *) ptr)[0]);)
#define vec_mask_loadu_packed_m32_16(ptr)                  vec_mask_m32_16( _cvtu32_mask16(((uint32_t *) ptr)[0]) )
#define vec_mask_loadu_packed_m32_8(ptr)                   vec_mask_m32_8( _cvtu32_mask8(((uint32_t *) ptr)[0]) )
#define vec_mask_loadu_packed_m32_4(ptr)                   vec_mask_m32_4( _cvtu32_mask8(((uint32_t *) ptr)[0]) )
#define vec_mask_loadu_packed_m32_1(ptr)                   vec_mask_m32_1( (uint8_t) vec_iter_expr_1(_j, 0, ((((uint32_t *) ptr)[0]) & (1 << _j)) ? 1 : 0) )

#define vec_mask_whilelt_m32_32(i, N)                       vec_loop_expr(vec_mask_m32_32_t, 2, _tmp, _i, (_tmp).v[_i] = _mm512_cmpgt_epi32_mask( _mm512_set1_epi32(N-i), _mm512_set_epi32(vec_iter_expr_16(_j, 16*_i, _j)) );)
#define vec_mask_whilelt_m32_16(i, N)                      vec_mask_m32_16( _mm512_cmpgt_epi32_mask( _mm512_set1_epi32(N-i), _mm512_set_epi32(vec_iter_expr_16(_j, 0, _j)) ) )
#define vec_mask_whilelt_m32_8(i, N)                       vec_mask_m32_8( _mm256_cmpgt_epi32_mask( _mm256_set1_epi32(N-i), _mm256_set_epi32(vec_iter_expr_8(_j, 0, _j)) ) )
#define vec_mask_whilelt_m32_4(i, N)                       vec_mask_m32_4( _mm_cmpgt_epi32_mask( _mm_set1_epi32(N-i), _mm_set_epi32(vec_iter_expr_4(_j, 0, _j)) ) )
#define vec_mask_whilelt_m32_1(i, N)                       vec_mask_m32_1( (uint8_t) ((i < N) ? -1 : 0) )

#define vec_mask_firstN_m32_32(N)                          vec_loop_expr(vec_mask_m32_32_t, 2, _tmp, _i, (_tmp).v[_i] = (uint16_t) (((1ULL << (N)) - 1) >> 16*_i);)
#define vec_mask_firstN_m32_16(N)                          vec_mask_m32_16( (uint16_t) ((1ULL << (N)) - 1) )
#define vec_mask_firstN_m32_8(N)                           vec_mask_m32_8( (uint8_t) ((1ULL << (N)) - 1) )
#define vec_mask_firstN_m32_4(N)                           vec_mask_m32_4( (uint8_t) ((1ULL << (N)) - 1) )
#define vec_mask_firstN_m32_1(N)                           vec_mask_m32_1( (uint8_t) ((1ULL << (N)) - 1) )


//------------------------------------------------------------------------------------------------------------------------------------------
//- Operations
//------------------------------------------------------------------------------------------------------------------------------------------

#define vec_and_m32_32(a, b)                               vec_loop_expr(vec_mask_m32_32_t, 2, _tmp, _i, (_tmp).v[_i] = _kand_mask16((a).v[_i], (b).v[_i]);)
#define vec_and_m32_16(a, b)                               vec_mask_m32_16( _kand_mask16((a).v, (b).v) )
#define vec_and_m32_8(a, b)                                vec_mask_m32_8( _kand_mask8((a).v, (b).v) )
#define vec_and_m32_4(a, b)                                vec_mask_m32_4( _kand_mask8((a).v, (b).v) )
#define vec_and_m32_1(a, b)                                vec_mask_m32_1( (uint8_t) ((a).v & (b).v) )

#define vec_or_m32_32(a, b)                                vec_loop_expr(vec_mask_m32_32_t, 2, _tmp, _i, (_tmp).v[_i] = _kor_mask16((a).v[_i], (b).v[_i]);)
#define vec_or_m32_16(a, b)                                vec_mask_m32_16( _kor_mask16((a).v, (b).v) )
#define vec_or_m32_8(a, b)                                 vec_mask_m32_8( _kor_mask8((a).v, (b).v) )
#define vec_or_m32_4(a, b)                                 vec_mask_m32_4( _kor_mask8((a).v, (b).v) )
#define vec_or_m32_1(a, b)                                 vec_mask_m32_1( (uint8_t) ((a).v | (b).v) )

#define vec_not_m32_32(a)                                  vec_loop_expr(vec_mask_m32_32_t, 2, _tmp, _i, (_tmp).v[_i] = _knot_mask16((a).v[_i]);)
#define vec_not_m32_16(a)                                  vec_mask_m32_16( _knot_mask16((a).v) )
#define vec_not_m32_8(a)                                   vec_mask_m32_8( _knot_mask8((a).v) )
#define vec_not_m32_4(a)                                   vec_mask_m32_4( _knot_mask8((a).v) )
#define vec_not_m32_1(a)                                   vec_mask_m32_1( (uint8_t) ((a).v ^ 1) )

#define vec_xor_m32_32(a, b)                               vec_loop_expr(vec_mask_m32_32_t, 2, _tmp, _i, (_tmp).v[_i] = _kxor_mask16((a).v[_i], (b).v[_i]);)
#define vec_xor_m32_16(a, b)                               vec_mask_m32_16( _kxor_mask16((a).v, (b).v) )
#define vec_xor_m32_8(a, b)                                vec_mask_m32_8( _kxor_mask8((a).v, (b).v) )
#define vec_xor_m32_4(a, b)                                vec_mask_m32_4( _kxor_mask8((a).v, (b).v) )
#define vec_xor_m32_1(a, b)                                vec_mask_m32_1( (uint8_t) ((a).v ^ (b).v) )


#endif /* VECTORIZATION_X86_AVX512_M32_H */

