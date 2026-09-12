#ifndef VECTORIZATION_SCALAR_M32_H
#define VECTORIZATION_SCALAR_M32_H

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "macros/cpp_defines.h"
#include "macros/macrolib.h"
#include "bit_ops.h"

#include "vectorization/vectorization_util.h"


typedef union __attribute__((packed)) { uint8_t  v; }  vec_mask_m32_1_t;
typedef union __attribute__((packed)) { uint8_t  v; }  vec_mask_m32_4_t;
typedef union __attribute__((packed)) { uint8_t  v; }  vec_mask_m32_8_t;
typedef union __attribute__((packed)) { uint16_t v; }  vec_mask_m32_16_t;
typedef union __attribute__((packed)) { uint32_t v; }  vec_mask_m32_32_t;

typedef uint8_t  vec_mask_packed_m32_1_t;
typedef uint8_t  vec_mask_packed_m32_4_t;
typedef uint8_t  vec_mask_packed_m32_8_t;
typedef uint16_t vec_mask_packed_m32_16_t;
typedef uint32_t vec_mask_packed_m32_32_t;


#define vec_mask_m32_32(val)                               ( (vec_mask_m32_32_t) { .v = (val) } )
#define vec_mask_m32_16(val)                               ( (vec_mask_m32_16_t) { .v = (val) } )
#define vec_mask_m32_8(val)                                ( (vec_mask_m32_8_t)  { .v = (val) } )
#define vec_mask_m32_4(val)                                ( (vec_mask_m32_4_t)  { .v = (val) } )
#define vec_mask_m32_1(val)                                ( (vec_mask_m32_1_t)  { .v = (val) } )


//------------------------------------------------------------------------------------------------------------------------------------------
//- Set - Load - Store
//------------------------------------------------------------------------------------------------------------------------------------------

#define vec_mask_pack_m32_32(a)                            ((a).v)
#define vec_mask_pack_m32_16(a)                            ((a).v)
#define vec_mask_pack_m32_8(a)                             ((a).v)
#define vec_mask_pack_m32_4(a)                             ((a).v)
#define vec_mask_pack_m32_1(a)                             ((a).v)

#define vec_mask_packed_get_bit_m32_32(a, pos)             bits_u32_extract(a, pos, 1)
#define vec_mask_packed_get_bit_m32_16(a, pos)             bits_u32_extract(a, pos, 1)
#define vec_mask_packed_get_bit_m32_8(a, pos)              bits_u32_extract(a, pos, 1)
#define vec_mask_packed_get_bit_m32_4(a, pos)              bits_u32_extract(a, pos, 1)
#define vec_mask_packed_get_bit_m32_1(a, pos)              bits_u32_extract(a, pos, 1)

#define vec_mask_loadu_packed_m32_32(ptr)                  vec_loop_expr_init(vec_mask_m32_32_t, 32, _tmp, vec_mask_m32_32(0), _i, (_tmp).v |= (ptr) & (1 << _i);)
#define vec_mask_loadu_packed_m32_16(ptr)                  vec_loop_expr_init(vec_mask_m32_16_t, 16, _tmp, vec_mask_m32_16(0), _i, (_tmp).v |= (ptr) & (1 << _i);)
#define vec_mask_loadu_packed_m32_8(ptr)                   vec_loop_expr_init(vec_mask_m32_8_t,   8, _tmp, vec_mask_m32_8(0),  _i, (_tmp).v |= (ptr) & (1 << _i);)
#define vec_mask_loadu_packed_m32_4(ptr)                   vec_loop_expr_init(vec_mask_m32_4_t,   4, _tmp, vec_mask_m32_4(0),  _i, (_tmp).v |= (ptr) & (1 << _i);)
#define vec_mask_loadu_packed_m32_1(ptr)                   vec_loop_expr_init(vec_mask_m32_1_t,   1, _tmp, vec_mask_m32_1(0),  _i, (_tmp).v |= (ptr) & (1 << _i);)


//------------------------------------------------------------------------------------------------------------------------------------------
//- Operations
//------------------------------------------------------------------------------------------------------------------------------------------

// Bitwise operations return at least 'int' type, so it gives warning of 'narrowing conversion'.
#define vec_and_m32_32(a, b)                               vec_mask_m32_32( (typeof((a).v)) ((a).v & (b).v) )
#define vec_and_m32_16(a, b)                               vec_mask_m32_16( (typeof((a).v)) ((a).v & (b).v) )
#define vec_and_m32_8(a, b)                                vec_mask_m32_8(  (typeof((a).v)) ((a).v & (b).v) )
#define vec_and_m32_4(a, b)                                vec_mask_m32_4(  (typeof((a).v)) ((a).v & (b).v) )
#define vec_and_m32_1(a, b)                                vec_mask_m32_1(  (typeof((a).v)) ((a).v & (b).v) )

#define vec_or_m32_32(a, b)                                vec_mask_m32_32( (typeof((a).v)) ((a).v | (b).v) )
#define vec_or_m32_16(a, b)                                vec_mask_m32_16( (typeof((a).v)) ((a).v | (b).v) )
#define vec_or_m32_8(a, b)                                 vec_mask_m32_8(  (typeof((a).v)) ((a).v | (b).v) )
#define vec_or_m32_4(a, b)                                 vec_mask_m32_4(  (typeof((a).v)) ((a).v | (b).v) )
#define vec_or_m32_1(a, b)                                 vec_mask_m32_1(  (typeof((a).v)) ((a).v | (b).v) )

#define vec_not_m32_32(a)                                  vec_mask_m32_32( (typeof((a).v)) (~(a).v) )
#define vec_not_m32_16(a)                                  vec_mask_m32_16( (typeof((a).v)) (~(a).v) )
#define vec_not_m32_8(a)                                   vec_mask_m32_8(  (typeof((a).v)) (~(a).v) )
#define vec_not_m32_4(a)                                   vec_mask_m32_4(  (typeof((a).v)) (~(a).v) )
#define vec_not_m32_1(a)                                   vec_mask_m32_1(  (typeof((a).v)) (~(a).v) )

#define vec_xor_m32_32(a, b)                               vec_mask_m32_32( (typeof((a).v)) ((a).v ^ (b).v) )
#define vec_xor_m32_16(a, b)                               vec_mask_m32_16( (typeof((a).v)) ((a).v ^ (b).v) )
#define vec_xor_m32_8(a, b)                                vec_mask_m32_8(  (typeof((a).v)) ((a).v ^ (b).v) )
#define vec_xor_m32_4(a, b)                                vec_mask_m32_4(  (typeof((a).v)) ((a).v ^ (b).v) )
#define vec_xor_m32_1(a, b)                                vec_mask_m32_1(  (typeof((a).v)) ((a).v ^ (b).v) )


#endif /* VECTORIZATION_SCALAR_M32_H */

