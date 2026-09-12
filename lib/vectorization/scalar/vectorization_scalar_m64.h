#ifndef VECTORIZATION_SCALAR_M64_H
#define VECTORIZATION_SCALAR_M64_H

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "macros/cpp_defines.h"
#include "macros/macrolib.h"
#include "bit_ops.h"

#include "vectorization/vectorization_util.h"


typedef union __attribute__((packed)) { uint8_t  v; }  vec_mask_m64_1_t;
typedef union __attribute__((packed)) { uint8_t  v; }  vec_mask_m64_2_t;
typedef union __attribute__((packed)) { uint8_t  v; }  vec_mask_m64_4_t;
typedef union __attribute__((packed)) { uint8_t  v; }  vec_mask_m64_8_t;
typedef union __attribute__((packed)) { uint16_t v; }  vec_mask_m64_16_t;

typedef uint8_t  vec_mask_packed_m64_1_t;
typedef uint8_t  vec_mask_packed_m64_2_t;
typedef uint8_t  vec_mask_packed_m64_4_t;
typedef uint8_t  vec_mask_packed_m64_8_t;
typedef uint16_t vec_mask_packed_m64_16_t;


#define vec_mask_m64_16(val)                               ( (vec_mask_m64_16_t) { .v = (val) } )
#define vec_mask_m64_8(val)                                ( (vec_mask_m64_8_t)  { .v = (val) } )
#define vec_mask_m64_4(val)                                ( (vec_mask_m64_4_t)  { .v = (val) } )
#define vec_mask_m64_2(val)                                ( (vec_mask_m64_2_t)  { .v = (val) } )
#define vec_mask_m64_1(val)                                ( (vec_mask_m64_1_t)  { .v = (val) } )


//------------------------------------------------------------------------------------------------------------------------------------------
//- Set - Load - Store
//------------------------------------------------------------------------------------------------------------------------------------------

#define vec_mask_pack_m64_16(a)                            ((a).v)
#define vec_mask_pack_m64_8(a)                             ((a).v)
#define vec_mask_pack_m64_4(a)                             ((a).v)
#define vec_mask_pack_m64_2(a)                             ((a).v)
#define vec_mask_pack_m64_1(a)                             ((a).v)

#define vec_mask_packed_get_bit_m64_16(a, pos)             bits_u64_extract(a, pos, 1)
#define vec_mask_packed_get_bit_m64_8(a, pos)              bits_u64_extract(a, pos, 1)
#define vec_mask_packed_get_bit_m64_4(a, pos)              bits_u64_extract(a, pos, 1)
#define vec_mask_packed_get_bit_m64_2(a, pos)              bits_u64_extract(a, pos, 1)
#define vec_mask_packed_get_bit_m64_1(a, pos)              bits_u64_extract(a, pos, 1)

#define vec_mask_loadu_packed_m64_16(ptr)                  vec_loop_expr_init(vec_mask_m64_16_t, 16, _tmp, vec_mask_m64_16(0), _i, (_tmp).v |= (ptr) & (1 << _i);)
#define vec_mask_loadu_packed_m64_8(ptr)                   vec_loop_expr_init(vec_mask_m64_8_t,   8, _tmp, vec_mask_m64_8(0),  _i, (_tmp).v |= (ptr) & (1 << _i);)
#define vec_mask_loadu_packed_m64_4(ptr)                   vec_loop_expr_init(vec_mask_m64_4_t,   4, _tmp, vec_mask_m64_4(0),  _i, (_tmp).v |= (ptr) & (1 << _i);)
#define vec_mask_loadu_packed_m64_2(ptr)                   vec_loop_expr_init(vec_mask_m64_2_t,   2, _tmp, vec_mask_m64_2(0),  _i, (_tmp).v |= (ptr) & (1 << _i);)
#define vec_mask_loadu_packed_m64_1(ptr)                   vec_loop_expr_init(vec_mask_m64_1_t,   1, _tmp, vec_mask_m64_1(0),  _i, (_tmp).v |= (ptr) & (1 << _i);)


//------------------------------------------------------------------------------------------------------------------------------------------
//- Operations
//------------------------------------------------------------------------------------------------------------------------------------------

// Bitwise operations return at least 'int' type, so it gives warning of 'narrowing conversion'.
#define vec_and_m64_16(a, b)                               vec_mask_m64_16( (typeof((a).v)) ((a).v & (b).v) )
#define vec_and_m64_8(a, b)                                vec_mask_m64_8(  (typeof((a).v)) ((a).v & (b).v) )
#define vec_and_m64_4(a, b)                                vec_mask_m64_4(  (typeof((a).v)) ((a).v & (b).v) )
#define vec_and_m64_2(a, b)                                vec_mask_m64_2(  (typeof((a).v)) ((a).v & (b).v) )
#define vec_and_m64_1(a, b)                                vec_mask_m64_1(  (typeof((a).v)) ((a).v & (b).v) )

#define vec_or_m64_16(a, b)                                vec_mask_m64_16( (typeof((a).v)) ((a).v | (b).v) )
#define vec_or_m64_8(a, b)                                 vec_mask_m64_8(  (typeof((a).v)) ((a).v | (b).v) )
#define vec_or_m64_4(a, b)                                 vec_mask_m64_4(  (typeof((a).v)) ((a).v | (b).v) )
#define vec_or_m64_2(a, b)                                 vec_mask_m64_2(  (typeof((a).v)) ((a).v | (b).v) )
#define vec_or_m64_1(a, b)                                 vec_mask_m64_1(  (typeof((a).v)) ((a).v | (b).v) )

#define vec_not_m64_16(a)                                  vec_mask_m64_16( (typeof((a).v)) (~(a).v) )
#define vec_not_m64_8(a)                                   vec_mask_m64_8(  (typeof((a).v)) (~(a).v) )
#define vec_not_m64_4(a)                                   vec_mask_m64_4(  (typeof((a).v)) (~(a).v) )
#define vec_not_m64_2(a)                                   vec_mask_m64_2(  (typeof((a).v)) (~(a).v) )
#define vec_not_m64_1(a)                                   vec_mask_m64_1(  (typeof((a).v)) (~(a).v) )

#define vec_xor_m64_16(a, b)                               vec_mask_m64_16( (typeof((a).v)) ((a).v ^ (b).v) )
#define vec_xor_m64_8(a, b)                                vec_mask_m64_8(  (typeof((a).v)) ((a).v ^ (b).v) )
#define vec_xor_m64_4(a, b)                                vec_mask_m64_4(  (typeof((a).v)) ((a).v ^ (b).v) )
#define vec_xor_m64_2(a, b)                                vec_mask_m64_2(  (typeof((a).v)) ((a).v ^ (b).v) )
#define vec_xor_m64_1(a, b)                                vec_mask_m64_1(  (typeof((a).v)) ((a).v ^ (b).v) )


#endif /* VECTORIZATION_SCALAR_M64_H */

