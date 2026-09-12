#ifndef VECTORIZATION_X86_AVX256_MASK64_H
#define VECTORIZATION_X86_AVX256_MASK64_H

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <x86intrin.h>

#include "macros/cpp_defines.h"
#include "macros/macrolib.h"
#include "bit_ops.h"

#include "vectorization/vectorization_util.h"


typedef union __attribute__((packed, aligned(8))) { int64_t v;    double  vf;    }  vec_mask_m64_1_t;
typedef union __attribute__((packed, aligned(8))) { __m128i v;    __m128d vf;    }  vec_mask_m64_2_t;
typedef union __attribute__((packed, aligned(8))) { __m256i v;    __m256d vf;    }  vec_mask_m64_4_t;
typedef union __attribute__((packed, aligned(8))) { __m256i v[2]; __m256d vf[2]; }  vec_mask_m64_8_t;
typedef union __attribute__((packed, aligned(8))) { __m256i v[4]; __m256d vf[4]; }  vec_mask_m64_16_t;

typedef uint32_t vec_mask_packed_m64_1_t;
typedef uint32_t vec_mask_packed_m64_2_t;
typedef uint32_t vec_mask_packed_m64_4_t;
typedef uint32_t vec_mask_packed_m64_8_t;
typedef uint32_t vec_mask_packed_m64_16_t;


#define vec_mask_m64_4(val)                                ( (vec_mask_m64_4_t)  { .v = (val) } )
#define vec_mask_m64_2(val)                                ( (vec_mask_m64_2_t)  { .v = (val) } )
#define vec_mask_m64_1(val)                                ( (vec_mask_m64_1_t)  { .v = (val) } )


//------------------------------------------------------------------------------------------------------------------------------------------
//- Set - Load - Store
//------------------------------------------------------------------------------------------------------------------------------------------

#define vec_mask_pack_m64_16(a)                            vec_loop_expr_init(uint32_t, 4, _tmp, 0, _i, _tmp |= (uint32_t) _mm256_movemask_pd((a).vf[_i]) << (_i * 4);)
#define vec_mask_pack_m64_8(a)                             vec_loop_expr_init(uint32_t, 2, _tmp, 0, _i, _tmp |= (uint32_t) _mm256_movemask_pd((a).vf[_i]) << (_i * 4);)
#define vec_mask_pack_m64_4(a)                             ((uint32_t) _mm256_movemask_pd((a).vf))
#define vec_mask_pack_m64_2(a)                             ((uint32_t) _mm_movemask_pd((a).vf))
#define vec_mask_pack_m64_1(a)                             ((uint32_t) (a).v)

#define vec_mask_packed_get_bit_m64_16(a, pos)             bits_u64_extract(a, pos, 1)
#define vec_mask_packed_get_bit_m64_8(a, pos)              bits_u64_extract(a, pos, 1)
#define vec_mask_packed_get_bit_m64_4(a, pos)              bits_u64_extract(a, pos, 1)
#define vec_mask_packed_get_bit_m64_2(a, pos)              bits_u64_extract(a, pos, 1)
#define vec_mask_packed_get_bit_m64_1(a, pos)              bits_u64_extract(a, pos, 1)

#define vec_mask_loadu_packed_m64_16(ptr)                  vec_loop_expr(vec_mask_m64_16_t, 4, _tmp, _i, (_tmp).v[_i] = _mm256_set_epi64x(vec_iter_expr_4(_j, 0, ((uint32_t *) ptr)[0] & (1 << (4*_i+_j)) ? -1 : 0));)
#define vec_mask_loadu_packed_m64_8(ptr)                   vec_loop_expr(vec_mask_m64_8_t,  2, _tmp, _i, (_tmp).v[_i] = _mm256_set_epi64x(vec_iter_expr_4(_j, 0, ((uint32_t *) ptr)[0] & (1 << (4*_i+_j)) ? -1 : 0));)
#define vec_mask_loadu_packed_m64_4(ptr)                   vec_mask_m64_4( _mm256_set_epi64x(vec_iter_expr_4(_j, 0, ((uint32_t *) ptr)[0] & (1 << _j) ? -1 : 0)) )
#define vec_mask_loadu_packed_m64_2(ptr)                   vec_mask_m64_2( _mm_set_epi64x(vec_iter_expr_2(_j, 0, ((uint32_t *) ptr)[0] & (1 << _j) ? -1 : 0)) )
#define vec_mask_loadu_packed_m64_1(ptr)                   vec_mask_m64_1( vec_iter_expr_1(_j, 0, (((uint32_t *) ptr)[0] & (1 << _j)) ? -1 : 0) )

#define vec_mask_whilelt_m64_16(i, N)                      vec_loop_expr(vec_mask_m64_16_t, 4, _tmp, _i, (_tmp).v[_i] = _mm256_cmpgt_epi64( _mm256_set1_epi64x(N-i), _mm256_set_epi64x(vec_iter_expr_4(_j, 4*_i, _j)) );)
#define vec_mask_whilelt_m64_8(i, N)                       vec_loop_expr(vec_mask_m64_8_t,  2, _tmp, _i, (_tmp).v[_i] = _mm256_cmpgt_epi64( _mm256_set1_epi64x(N-i), _mm256_set_epi64x(vec_iter_expr_4(_j, 4*_i, _j)) );)
#define vec_mask_whilelt_m64_4(i, N)                       vec_mask_m64_4( _mm256_cmpgt_epi64( _mm256_set1_epi64x(N-i), _mm256_set_epi64x(vec_iter_expr_4(_j, 0, _j)) ) )
#define vec_mask_whilelt_m64_2(i, N)                       vec_mask_m64_2( _mm_cmpgt_epi64( _mm_set1_epi64x(N-i), _mm_set_epi64x(vec_iter_expr_2(_j, 0, _j)) ) )
#define vec_mask_whilelt_m64_1(i, N)                       vec_mask_m64_1( (i < N) ? -1 : 0 )

#define vec_mask_firstN_m64_16(N)                          vec_loop_expr(vec_mask_m64_16_t, 4, _tmp, _i, (_tmp).v[_i] = _mm256_cmpgt_epi64( _mm256_set1_epi64x(N), _mm256_set_epi64x(vec_iter_expr_4(_j, 4*_i, _j)) );)
#define vec_mask_firstN_m64_8(N)                           vec_loop_expr(vec_mask_m64_8_t,  2, _tmp, _i, (_tmp).v[_i] = _mm256_cmpgt_epi64( _mm256_set1_epi64x(N), _mm256_set_epi64x(vec_iter_expr_4(_j, 4*_i, _j)) );)
#define vec_mask_firstN_m64_4(N)                           vec_mask_m64_4( _mm256_cmpgt_epi64( _mm256_set1_epi64x(N), _mm256_set_epi64x(vec_iter_expr_4(_j, 0, _j)) ) )
#define vec_mask_firstN_m64_2(N)                           vec_mask_m64_2( _mm_cmpgt_epi64( _mm_set1_epi64x(N), _mm_set_epi64x(vec_iter_expr_2(_j, 0, _j)) ) )
#define vec_mask_firstN_m64_1(N)                           vec_mask_m64_1( (N > 0) ? -1 : 0 )


//------------------------------------------------------------------------------------------------------------------------------------------
//- Operations
//------------------------------------------------------------------------------------------------------------------------------------------

#define vec_and_m64_16(a, b)                               vec_loop_expr(vec_mask_m64_16_t, 4, _tmp, _i, (_tmp).v[_i] = _mm256_and_si256((a).v[_i], (b).v[_i]);)
#define vec_and_m64_8(a, b)                                vec_loop_expr(vec_mask_m64_8_t,  2, _tmp, _i, (_tmp).v[_i] = _mm256_and_si256((a).v[_i], (b).v[_i]);)
#define vec_and_m64_4(a, b)                                vec_mask_m64_4( _mm256_and_si256((a).v, (b).v) )
#define vec_and_m64_2(a, b)                                vec_mask_m64_2( _mm_and_si128((a).v, (b).v) )
#define vec_and_m64_1(a, b)                                vec_mask_m64_1( ((a).v & (b).v) )

#define vec_or_m64_16(a, b)                                vec_loop_expr(vec_mask_m64_16_t, 4, _tmp, _i, (_tmp).v[_i] = _mm256_or_si256((a).v[_i], (b).v[_i]);)
#define vec_or_m64_8(a, b)                                 vec_loop_expr(vec_mask_m64_8_t,  2, _tmp, _i, (_tmp).v[_i] = _mm256_or_si256((a).v[_i], (b).v[_i]);)
#define vec_or_m64_4(a, b)                                 vec_mask_m64_4( _mm256_or_si256((a).v, (b).v) )
#define vec_or_m64_2(a, b)                                 vec_mask_m64_2( _mm_or_si128((a).v, (b).v) )
#define vec_or_m64_1(a, b)                                 vec_mask_m64_1( ((a).v | (b).v) )

#define vec_not_m64_16(a)                                  vec_loop_expr(vec_mask_m64_16_t, 4, _tmp, _i, (_tmp).v[_i] = _mm256_xor_si256((a).v[_i], _mm256_set1_epi64x(-1LL));)
#define vec_not_m64_8(a)                                   vec_loop_expr(vec_mask_m64_8_t,  2, _tmp, _i, (_tmp).v[_i] = _mm256_xor_si256((a).v[_i], _mm256_set1_epi64x(-1LL));)
#define vec_not_m64_4(a)                                   vec_mask_m64_4( _mm256_xor_si256((a).v, _mm256_set1_epi64x(-1LL)) )
#define vec_not_m64_2(a)                                   vec_mask_m64_2( _mm_xor_si128((a).v, _mm_set1_epi64x(-1LL)) )
#define vec_not_m64_1(a)                                   vec_mask_m64_1( ((a).v ^ 1) )

#define vec_xor_m64_16(a, b)                               vec_loop_expr(vec_mask_m64_16_t, 4, _tmp, _i, (_tmp).v[_i] = _mm256_xor_si256((a).v[_i], (b).v[_i]);)
#define vec_xor_m64_8(a, b)                                vec_loop_expr(vec_mask_m64_8_t,  2, _tmp, _i, (_tmp).v[_i] = _mm256_xor_si256((a).v[_i], (b).v[_i]);)
#define vec_xor_m64_4(a, b)                                vec_mask_m64_4( _mm256_xor_si256((a).v, (b).v) )
#define vec_xor_m64_2(a, b)                                vec_mask_m64_2( _mm_xor_si128((a).v, (b).v) )
#define vec_xor_m64_1(a, b)                                vec_mask_m64_1( ((a).v ^ (b).v) )


#endif /* VECTORIZATION_X86_AVX256_MASK64_H */

