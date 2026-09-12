#ifndef VECTORIZATION_X86_AVX512_M64_H
#define VECTORIZATION_X86_AVX512_M64_H

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <x86intrin.h>

#include "macros/cpp_defines.h"
#include "macros/macrolib.h"
#include "bit_ops.h"

#include "vectorization/vectorization_util.h"


typedef union __attribute__((packed)) { __mmask8 v;    }  vec_mask_m64_1_t;
typedef union __attribute__((packed)) { __mmask8 v;    }  vec_mask_m64_2_t;
typedef union __attribute__((packed)) { __mmask8 v;    }  vec_mask_m64_4_t;
typedef union __attribute__((packed)) { __mmask8 v;    }  vec_mask_m64_8_t;
typedef union __attribute__((packed)) { __mmask8 v[2]; }  vec_mask_m64_16_t;

typedef uint8_t  vec_mask_packed_m64_1_t;
typedef uint8_t  vec_mask_packed_m64_2_t;
typedef uint8_t  vec_mask_packed_m64_4_t;
typedef uint8_t  vec_mask_packed_m64_8_t;
typedef uint16_t vec_mask_packed_m64_16_t;


#define vec_mask_m64_8(val)                                ( (vec_mask_m64_8_t)  { .v = (val) } )
#define vec_mask_m64_4(val)                                ( (vec_mask_m64_4_t)  { .v = (val) } )
#define vec_mask_m64_2(val)                                ( (vec_mask_m64_2_t)  { .v = (val) } )
#define vec_mask_m64_1(val)                                ( (vec_mask_m64_1_t)  { .v = (val) } )


//------------------------------------------------------------------------------------------------------------------------------------------
//- Set - Load - Store
//------------------------------------------------------------------------------------------------------------------------------------------

#define vec_mask_pack_m64_16(a)                            vec_loop_expr_init(vec_mask_packed_m64_16_t, 2, _tmp, 0, _i, _tmp |= ((vec_mask_packed_m64_16_t) (a).v[_i]) << (_i * 8);)
#define vec_mask_pack_m64_8(a)                             ((a).v)
#define vec_mask_pack_m64_4(a)                             ((a).v)
#define vec_mask_pack_m64_2(a)                             ((a).v)
#define vec_mask_pack_m64_1(a)                             ((a).v)

#define vec_mask_packed_get_bit_m64_16(a, pos)             bits_u64_extract(a, pos, 1)
#define vec_mask_packed_get_bit_m64_8(a, pos)              bits_u64_extract(a, pos, 1)
#define vec_mask_packed_get_bit_m64_4(a, pos)              bits_u64_extract(a, pos, 1)
#define vec_mask_packed_get_bit_m64_2(a, pos)              bits_u64_extract(a, pos, 1)
#define vec_mask_packed_get_bit_m64_1(a, pos)              bits_u64_extract(a, pos, 1)

#define vec_mask_loadu_packed_m64_16(ptr)                  vec_loop_expr(vec_mask_m64_16_t, 2, _tmp, _i, (_tmp).v[_i] = _cvtu32_mask8(((uint32_t *) ptr)[0]);)
#define vec_mask_loadu_packed_m64_8(ptr)                   vec_mask_m64_8( _cvtu32_mask8(((uint32_t *) ptr)[0]) )
#define vec_mask_loadu_packed_m64_4(ptr)                   vec_mask_m64_4( _cvtu32_mask8(((uint32_t *) ptr)[0]) )
#define vec_mask_loadu_packed_m64_2(ptr)                   vec_mask_m64_2( _cvtu32_mask8(((uint32_t *) ptr)[0]) )
#define vec_mask_loadu_packed_m64_1(ptr)                   vec_mask_m64_1( (uint8_t) vec_iter_expr_1(_j, 0, ((((uint32_t *) ptr)[0]) & (1 << _j)) ? 1 : 0) )

#define vec_mask_whilelt_m64_16(i, N)                      vec_loop_expr(vec_mask_m64_16_t, 2, _tmp, _i, (_tmp).v[_i] = _mm512_cmpgt_epi64_mask( _mm512_set1_epi64(N-i), _mm512_set_epi64(vec_iter_expr_8(_j, 8*_i, _j)) );)
#define vec_mask_whilelt_m64_8(i, N)                       vec_mask_m64_8( _mm512_cmpgt_epi64_mask( _mm512_set1_epi64(N-i), _mm512_set_epi64(vec_iter_expr_8(_j, 0, _j)) ) )
#define vec_mask_whilelt_m64_4(i, N)                       vec_mask_m64_4( _mm256_cmpgt_epi64_mask( _mm256_set1_epi64x(N-i), _mm256_set_epi64x(vec_iter_expr_4(_j, 0, _j)) ) )
#define vec_mask_whilelt_m64_2(i, N)                       vec_mask_m64_2( _mm_cmpgt_epi64_mask( _mm_set1_epi64x(N-i), _mm_set_epi64x(vec_iter_expr_2(_j, 0, _j)) ) )
#define vec_mask_whilelt_m64_1(i, N)                       vec_mask_m64_1( (uint8_t) ((i < N) ? -1 : 0) )

#define vec_mask_firstN_m64_16(N)                          vec_loop_expr(vec_mask_m64_16_t, 2, _tmp, _i, (_tmp).v[_i] = (uint8_t) (((1ULL << (N)) - 1) >> 8*_i);)
#define vec_mask_firstN_m64_8(N)                           vec_mask_m64_8( (uint8_t) ((1ULL << (N)) - 1) )
#define vec_mask_firstN_m64_4(N)                           vec_mask_m64_4( (uint8_t) ((1ULL << (N)) - 1) )
#define vec_mask_firstN_m64_2(N)                           vec_mask_m64_2( (uint8_t) ((1ULL << (N)) - 1) )
#define vec_mask_firstN_m64_1(N)                           vec_mask_m64_1( (uint8_t) ((1ULL << (N)) - 1) )


//------------------------------------------------------------------------------------------------------------------------------------------
//- Operations
//------------------------------------------------------------------------------------------------------------------------------------------

#define vec_and_m64_16(a, b)                               vec_loop_expr(vec_mask_m64_16_t, 2, _tmp, _i, (_tmp).v[_i] = _kand_mask8((a).v[_i], (b).v[_i]);)
#define vec_and_m64_8(a, b)                                vec_mask_m64_8( _kand_mask8((a).v, (b).v) )
#define vec_and_m64_4(a, b)                                vec_mask_m64_4( _kand_mask8((a).v, (b).v) )
#define vec_and_m64_2(a, b)                                vec_mask_m64_2( _kand_mask8((a).v, (b).v) )
#define vec_and_m64_1(a, b)                                vec_mask_m64_1( (uint8_t) ((a).v & (b).v) )

#define vec_or_m64_16(a, b)                                vec_loop_expr(vec_mask_m64_16_t, 2, _tmp, _i, (_tmp).v[_i] = _kor_mask8((a).v[_i], (b).v[_i]);)
#define vec_or_m64_8(a, b)                                 vec_mask_m64_8( _kor_mask8((a).v, (b).v) )
#define vec_or_m64_4(a, b)                                 vec_mask_m64_4( _kor_mask8((a).v, (b).v) )
#define vec_or_m64_2(a, b)                                 vec_mask_m64_2( _kor_mask8((a).v, (b).v) )
#define vec_or_m64_1(a, b)                                 vec_mask_m64_1( (uint8_t) ((a).v | (b).v) )

#define vec_not_m64_16(a)                                  vec_loop_expr(vec_mask_m64_16_t, 2, _tmp, _i, (_tmp).v[_i] = _knot_mask8((a).v[_i]);)
#define vec_not_m64_8(a)                                   vec_mask_m64_8( _knot_mask8((a).v) )
#define vec_not_m64_4(a)                                   vec_mask_m64_4( _knot_mask8((a).v) )
#define vec_not_m64_2(a)                                   vec_mask_m64_2( _knot_mask8((a).v) )
#define vec_not_m64_1(a)                                   vec_mask_m64_1( (uint8_t) ((a).v ^ 1) )

#define vec_xor_m64_16(a, b)                               vec_loop_expr(vec_mask_m64_16_t, 2, _tmp, _i, (_tmp).v[_i] = _kxor_mask8((a).v[_i], (b).v[_i]);)
#define vec_xor_m64_8(a, b)                                vec_mask_m64_8( _kxor_mask8((a).v, (b).v) )
#define vec_xor_m64_4(a, b)                                vec_mask_m64_4( _kxor_mask8((a).v, (b).v) )
#define vec_xor_m64_2(a, b)                                vec_mask_m64_2( _kxor_mask8((a).v, (b).v) )
#define vec_xor_m64_1(a, b)                                vec_mask_m64_1( (uint8_t) ((a).v ^ (b).v) )


#endif /* VECTORIZATION_X86_AVX512_M64_H */

