#ifndef VECTORIZATION_RISCV_SVV_I32_H
#define VECTORIZATION_RISCV_SVV_I32_H

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <riscv_vector.h>

#include "macros/cpp_defines.h"
#include "macros/macrolib.h"

#include "vectorization/vectorization_util.h"
#include "vectorization_riscv_svv_m32.h"


typedef int32_t    __attribute__((may_alias))  vec_alias_int32_t;

typedef int32_t    __attribute__((may_alias))  vec_i32_1_t;
typedef vint32m1_t __attribute__((may_alias))  vec_i32_2_t;
typedef vint32m1_t __attribute__((may_alias))  vec_i32_4_t;
typedef vint32m1_t __attribute__((may_alias))  vec_i32_8_t;
typedef vint32m1_t __attribute__((may_alias))  vec_i32_16_t;
typedef vint32m1_t __attribute__((may_alias))  vec_i32_32_t;
typedef vint32m1_t __attribute__((may_alias))  vec_i32_256_t;
typedef vint32m1_t __attribute__((may_alias))  vec_i32_512_t;

typedef vec_i32_1_t   vec_perm_p32_1_t;
typedef vec_i32_2_t   vec_perm_p32_2_t;
typedef vec_i32_4_t   vec_perm_p32_4_t;
typedef vec_i32_8_t   vec_perm_p32_8_t;
typedef vec_i32_16_t  vec_perm_p32_16_t;
typedef vec_i32_32_t  vec_perm_p32_32_t;
typedef vec_i32_256_t  vec_perm_p32_256_t;
typedef vec_i32_512_t  vec_perm_p32_512_t;

typedef int32_t  vec_perm_elem_p32_1_t;
typedef int32_t  vec_perm_elem_p32_4_t;
typedef int32_t  vec_perm_elem_p32_8_t;
typedef int32_t  vec_perm_elem_p32_16_t;
typedef int32_t  vec_perm_elem_p32_32_t;
typedef int32_t  vec_perm_elem_p32_256_t;
typedef int32_t  vec_perm_elem_p32_512_t;

#define vec_len_default_i32     8
#define vec_len_default_i32_f2  4


//------------------------------------------------------------------------------------------------------------------------------------------
//- Cast
//------------------------------------------------------------------------------------------------------------------------------------------

#define vec_cast_to_i32_i32_32(val)                        (val)
#define vec_cast_to_i32_i32_16(val)                        (val)
#define vec_cast_to_i32_i32_8(val)                         (val)
#define vec_cast_to_i32_i32_4(val)                         (val)
#define vec_cast_to_i32_i32_1(val)                         (val)

#define vec_cast_to_i64_i32_32(val)                        vec_loop_expr(vec_i64_16_t, 2, _tmp, _i, (_tmp).v[_i] = (val).v[_i];)
#define vec_cast_to_i64_i32_16(val)                        ( (vec_i64_8_t)  { .v = (val).v } )
#define vec_cast_to_i64_i32_8(val)                         ( (vec_i64_4_t)  { .v = (val).v } )
#define vec_cast_to_i64_i32_4(val)                         ( (vec_i64_2_t)  { .v = (val).v } )
#define vec_cast_to_i64_i32_1(val)                         ( (vec_i64_1_t)  { .v = (val).v } )

#define vec_cast_to_f32_i32_32(val)                        vec_loop_expr(vec_f32_32_t, 2, _tmp, _i, (_tmp).v[_i] = (val).vf32[_i];)
#define vec_cast_to_f32_i32_16(val)                        ( (vec_f32_16_t) { .v = (val).v } )
#define vec_cast_to_f32_i32_8(val)                         ( (vec_f32_8_t)  { .v = (val).v } )
#define vec_cast_to_f32_i32_4(val)                         ( (vec_f32_4_t)  { .v = (val).v } )
#define vec_cast_to_f32_i32_1(val)                         ( (vec_f32_2_t)  { .v = (val).v } )

#define vec_cast_to_f64_i32_32(val)                        vec_loop_expr(vec_f64_16_t, 2, _tmp, _i, (_tmp).v[_i] = (val).vf64[_i];)
#define vec_cast_to_f64_i32_16(val)                        ( (vec_f64_8_t)  { .v = (val).v } )
#define vec_cast_to_f64_i32_8(val)                         ( (vec_f64_4_t)  { .v = (val).v } )
#define vec_cast_to_f64_i32_4(val)                         ( (vec_f64_2_t)  { .v = (val).v } )
#define vec_cast_to_f64_i32_1(val)                         ( (vec_f64_1_t)  { .v = (val).v } )


//------------------------------------------------------------------------------------------------------------------------------------------
//- Set - Load - Store
//------------------------------------------------------------------------------------------------------------------------------------------

#define vec_elem_get_i32_512(vec, index)                   ( ((vec_alias_int32_t *) &vec)[index] )
#define vec_elem_get_i32_256(vec, index)                   ( ((vec_alias_int32_t *) &vec)[index] )
#define vec_elem_get_i32_32(vec, index)                    ( ((vec_alias_int32_t *) &vec)[index] )
#define vec_elem_get_i32_16(vec, index)                    ( ((vec_alias_int32_t *) &vec)[index] )
#define vec_elem_get_i32_8(vec, index)                     ( ((vec_alias_int32_t *) &vec)[index] )
#define vec_elem_get_i32_4(vec, index)                     ( ((vec_alias_int32_t *) &vec)[index] )
#define vec_elem_get_i32_1(vec, index)                     vec

#define vec_elem_set_i32_512(vec, index, expr)             do { ((vec_alias_int32_t *) &vec)[index] = (expr); } while (0)
#define vec_elem_set_i32_256(vec, index, expr)             do { ((vec_alias_int32_t *) &vec)[index] = (expr); } while (0)
#define vec_elem_set_i32_32(vec, index, expr)              do { ((vec_alias_int32_t *) &vec)[index] = (expr); } while (0)
#define vec_elem_set_i32_16(vec, index, expr)              do { ((vec_alias_int32_t *) &vec)[index] = (expr); } while (0)
#define vec_elem_set_i32_8(vec, index, expr)               do { ((vec_alias_int32_t *) &vec)[index] = (expr); } while (0)
#define vec_elem_set_i32_4(vec, index, expr)               do { ((vec_alias_int32_t *) &vec)[index] = (expr); } while (0)
#define vec_elem_set_i32_1(vec, index, expr)               do { vec = (expr); } while (0)

#define vec_array_i32_512(vec)                             ((vec_alias_int32_t *) &vec)
#define vec_array_i32_256(vec)                             ((vec_alias_int32_t *) &vec)
#define vec_array_i32_32(vec)                              ((vec_alias_int32_t *) &vec)
#define vec_array_i32_16(vec)                              ((vec_alias_int32_t *) &vec)
#define vec_array_i32_8(vec)                               ((vec_alias_int32_t *) &vec)
#define vec_array_i32_4(vec)                               ((vec_alias_int32_t *) &vec)
#define vec_array_i32_1(vec)                               ((vec_alias_int32_t *) &vec)

#define vec_set1_i32_512(val)                              __riscv_vmv_v_x_i32m1((val), 512)
#define vec_set1_i32_256(val)                              __riscv_vmv_v_x_i32m1((val), 256)
#define vec_set1_i32_32(val)                               __riscv_vmv_v_x_i32m1((val),  32)
#define vec_set1_i32_16(val)                               __riscv_vmv_v_x_i32m1((val),  16)
#define vec_set1_i32_8(val)                                __riscv_vmv_v_x_i32m1((val),   8)
#define vec_set1_i32_4(val)                                __riscv_vmv_v_x_i32m1((val),   4)
#define vec_set1_i32_1(val)                                ( (int32_t) (val) )

#define vec_set_iter_i32_512(iter, expr)                   ({ vec_i32_512_t _buf; vec_loop_stmt(512, iter, ((vec_alias_int32_t *) &_buf)[iter] = (expr););  _buf; })
#define vec_set_iter_i32_256(iter, expr)                   ({ vec_i32_256_t _buf; vec_loop_stmt(256, iter, ((vec_alias_int32_t *) &_buf)[iter] = (expr););  _buf; })
#define vec_set_iter_i32_32(iter, expr)                    ({ vec_i32_32_t  _buf; vec_loop_stmt( 32, iter, ((vec_alias_int32_t *) &_buf)[iter] = (expr););  _buf; })
#define vec_set_iter_i32_16(iter, expr)                    ({ vec_i32_16_t  _buf; vec_loop_stmt( 16, iter, ((vec_alias_int32_t *) &_buf)[iter] = (expr););  _buf; })
#define vec_set_iter_i32_8(iter, expr)                     ({ vec_i32_8_t   _buf; vec_loop_stmt(  8, iter, ((vec_alias_int32_t *) &_buf)[iter] = (expr););  _buf; })
#define vec_set_iter_i32_4(iter, expr)                     ({ vec_i32_4_t   _buf; vec_loop_stmt(  4, iter, ((vec_alias_int32_t *) &_buf)[iter] = (expr););  _buf; })
#define vec_set_iter_i32_1(iter, expr)                     (  ( vec_alias_int32_t) vec_iter_expr_1(iter, 0, (expr)) )

#define vec_set_iter_p32_512(iter, expr)                   ({ vec_i32_512_t _buf; vec_loop_stmt(512, iter, ((vec_alias_int32_t *) &_buf)[iter] = (expr););  _buf; })
#define vec_set_iter_p32_256(iter, expr)                   ({ vec_i32_256_t _buf; vec_loop_stmt(256, iter, ((vec_alias_int32_t *) &_buf)[iter] = (expr););  _buf; })
#define vec_set_iter_p32_32(iter, expr)                    ({ vec_i32_32_t  _buf; vec_loop_stmt( 32, iter, ((vec_alias_int32_t *) &_buf)[iter] = (expr););  _buf; })
#define vec_set_iter_p32_16(iter, expr)                    ({ vec_i32_16_t  _buf; vec_loop_stmt( 16, iter, ((vec_alias_int32_t *) &_buf)[iter] = (expr););  _buf; })
#define vec_set_iter_p32_8(iter, expr)                     ({ vec_i32_8_t   _buf; vec_loop_stmt(  8, iter, ((vec_alias_int32_t *) &_buf)[iter] = (expr););  _buf; })
#define vec_set_iter_p32_4(iter, expr)                     ({ vec_i32_4_t   _buf; vec_loop_stmt(  4, iter, ((vec_alias_int32_t *) &_buf)[iter] = (expr););  _buf; })
#define vec_set_iter_p32_1(iter, expr)                     (  ( vec_alias_int32_t) vec_iter_expr_1(iter, 0, (expr)) )

#define vec_loadu_i32_512(ptr)                             __riscv_vle32_v_i32m1((vec_alias_int32_t *) (ptr), 512)
#define vec_loadu_i32_256(ptr)                             __riscv_vle32_v_i32m1((vec_alias_int32_t *) (ptr), 256)
#define vec_loadu_i32_32(ptr)                              __riscv_vle32_v_i32m1((vec_alias_int32_t *) (ptr),  32)
#define vec_loadu_i32_16(ptr)                              __riscv_vle32_v_i32m1((vec_alias_int32_t *) (ptr),  16)
#define vec_loadu_i32_8(ptr)                               __riscv_vle32_v_i32m1((vec_alias_int32_t *) (ptr),   8)
#define vec_loadu_i32_4(ptr)                               __riscv_vle32_v_i32m1((vec_alias_int32_t *) (ptr),   4)
#define vec_loadu_i32_1(ptr)                               ( (*((vec_alias_int32_t *) (ptr))) )

#define vec_loadu_p32_512(ptr)                             __riscv_vle32_v_i32m1((vec_alias_int32_t *) (ptr), 512)
#define vec_loadu_p32_256(ptr)                             __riscv_vle32_v_i32m1((vec_alias_int32_t *) (ptr), 256)
#define vec_loadu_p32_32(ptr)                              __riscv_vle32_v_i32m1((vec_alias_int32_t *) (ptr),  32)
#define vec_loadu_p32_16(ptr)                              __riscv_vle32_v_i32m1((vec_alias_int32_t *) (ptr),  16)
#define vec_loadu_p32_8(ptr)                               __riscv_vle32_v_i32m1((vec_alias_int32_t *) (ptr),   8)
#define vec_loadu_p32_4(ptr)                               __riscv_vle32_v_i32m1((vec_alias_int32_t *) (ptr),   4)
#define vec_loadu_p32_1(ptr)                               ( (*((vec_alias_int32_t *) (ptr))) )

#define vec_loadu_maskedz_i32_512(ptr, mask)               ( __riscv_vmerge_vvm_i32m1(__riscv_vmv_v_x_i32m1(0, 512), __riscv_vle32_v_i32m1_m(mask, (vec_alias_int32_t *) (ptr), 512), mask, 512) )
#define vec_loadu_maskedz_i32_256(ptr, mask)               ( __riscv_vmerge_vvm_i32m1(__riscv_vmv_v_x_i32m1(0, 256), __riscv_vle32_v_i32m1_m(mask, (vec_alias_int32_t *) (ptr), 256), mask, 256) )
#define vec_loadu_maskedz_i32_32(ptr, mask)                ( __riscv_vmerge_vvm_i32m1(__riscv_vmv_v_x_i32m1(0,  32), __riscv_vle32_v_i32m1_m(mask, (vec_alias_int32_t *) (ptr),  32), mask,  32) )
#define vec_loadu_maskedz_i32_16(ptr, mask)                ( __riscv_vmerge_vvm_i32m1(__riscv_vmv_v_x_i32m1(0,  16), __riscv_vle32_v_i32m1_m(mask, (vec_alias_int32_t *) (ptr),  16), mask,  16) )
#define vec_loadu_maskedz_i32_8(ptr, mask)                 ( __riscv_vmerge_vvm_i32m1(__riscv_vmv_v_x_i32m1(0,   8), __riscv_vle32_v_i32m1_m(mask, (vec_alias_int32_t *) (ptr),   8), mask,   8) )
#define vec_loadu_maskedz_i32_4(ptr, mask)                 ( __riscv_vmerge_vvm_i32m1(__riscv_vmv_v_x_i32m1(0,   4), __riscv_vle32_v_i32m1_m(mask, (vec_alias_int32_t *) (ptr),   4), mask,   4) )
#define vec_loadu_maskedz_i32_1(ptr, mask)                 ( (mask) ? (*((vec_alias_int32_t *) (ptr))) : 0 )

// #define vec_loadu_firstNz_i32_512(ptr)                             __riscv_vle32_v_i32m1((vec_alias_int32_t *) (ptr), 512)
// #define vec_loadu_firstNz_i32_256(ptr)                             __riscv_vle32_v_i32m1((vec_alias_int32_t *) (ptr), 256)
// #define vec_loadu_firstNz_i32_32(ptr)                              __riscv_vle32_v_i32m1((vec_alias_int32_t *) (ptr),  32)
// #define vec_loadu_firstNz_i32_16(ptr)                              __riscv_vle32_v_i32m1((vec_alias_int32_t *) (ptr),  16)
// #define vec_loadu_firstNz_i32_8(ptr)                               __riscv_vle32_v_i32m1((vec_alias_int32_t *) (ptr),   8)
// #define vec_loadu_firstNz_i32_4(ptr)                               __riscv_vle32_v_i32m1((vec_alias_int32_t *) (ptr),   4)
// #define vec_loadu_firstNz_i32_1(ptr)                               ( (*((vec_alias_int32_t *) (ptr))) )

#define vec_storeu_i32_512(ptr, vec)                       __riscv_vse32_v_i32m1((vec_alias_int32_t *) (ptr), vec, 512)
#define vec_storeu_i32_256(ptr, vec)                       __riscv_vse32_v_i32m1((vec_alias_int32_t *) (ptr), vec, 256)
#define vec_storeu_i32_32(ptr, vec)                        __riscv_vse32_v_i32m1((vec_alias_int32_t *) (ptr), vec,  32)
#define vec_storeu_i32_16(ptr, vec)                        __riscv_vse32_v_i32m1((vec_alias_int32_t *) (ptr), vec,  16)
#define vec_storeu_i32_8(ptr, vec)                         __riscv_vse32_v_i32m1((vec_alias_int32_t *) (ptr), vec,   8)
#define vec_storeu_i32_4(ptr, vec)                         __riscv_vse32_v_i32m1((vec_alias_int32_t *) (ptr), vec,   4)
#define vec_storeu_i32_1(ptr, vec)                         do { (*((vec_alias_int32_t *) (ptr))) = (vec); } while (0)

#define vec_storeu_p32_512(ptr, vec)                       __riscv_vse32_v_i32m1((vec_alias_int32_t *) (ptr), vec, 512)
#define vec_storeu_p32_256(ptr, vec)                       __riscv_vse32_v_i32m1((vec_alias_int32_t *) (ptr), vec, 256)
#define vec_storeu_p32_32(ptr, vec)                        __riscv_vse32_v_i32m1((vec_alias_int32_t *) (ptr), vec,  32)
#define vec_storeu_p32_16(ptr, vec)                        __riscv_vse32_v_i32m1((vec_alias_int32_t *) (ptr), vec,  16)
#define vec_storeu_p32_8(ptr, vec)                         __riscv_vse32_v_i32m1((vec_alias_int32_t *) (ptr), vec,   8)
#define vec_storeu_p32_4(ptr, vec)                         __riscv_vse32_v_i32m1((vec_alias_int32_t *) (ptr), vec,   4)
#define vec_storeu_p32_1(ptr, vec)                         do { (*((vec_alias_int32_t *) (ptr))) = (vec); } while (0)

#define vec_storeu_masked_i32_512(ptr, vec, mask)          __riscv_vse32_v_i32m1_m(mask, (vec_alias_int32_t *) (ptr), vec, 512)
#define vec_storeu_masked_i32_256(ptr, vec, mask)          __riscv_vse32_v_i32m1_m(mask, (vec_alias_int32_t *) (ptr), vec, 256)
#define vec_storeu_masked_i32_32(ptr, vec, mask)           __riscv_vse32_v_i32m1_m(mask, (vec_alias_int32_t *) (ptr), vec,  32)
#define vec_storeu_masked_i32_16(ptr, vec, mask)           __riscv_vse32_v_i32m1_m(mask, (vec_alias_int32_t *) (ptr), vec,  16)
#define vec_storeu_masked_i32_8(ptr, vec, mask)            __riscv_vse32_v_i32m1_m(mask, (vec_alias_int32_t *) (ptr), vec,   8)
#define vec_storeu_masked_i32_4(ptr, vec, mask)            __riscv_vse32_v_i32m1_m(mask, (vec_alias_int32_t *) (ptr), vec,   4)
#define vec_storeu_masked_i32_1(ptr, vec, mask)            do { if (mask) (*((vec_alias_int32_t *) (ptr))) = (vec); } while (0)

#define vec_gather_i32_i32_512(ptr, idx)                   __riscv_vluxei32_v_i32m1((vec_alias_int32_t *) (ptr), riscv_vsll_vx_i32m1(idx, 2, 512), 512)
#define vec_gather_i32_i32_256(ptr, idx)                   __riscv_vluxei32_v_i32m1((vec_alias_int32_t *) (ptr), riscv_vsll_vx_i32m1(idx, 2, 256), 256)
#define vec_gather_i32_i32_32(ptr, idx)                    __riscv_vluxei32_v_i32m1((vec_alias_int32_t *) (ptr), riscv_vsll_vx_i32m1(idx, 2,  32),  32)
#define vec_gather_i32_i32_16(ptr, idx)                    __riscv_vluxei32_v_i32m1((vec_alias_int32_t *) (ptr), riscv_vsll_vx_i32m1(idx, 2,  16),  16)
#define vec_gather_i32_i32_8(ptr, idx)                     __riscv_vluxei32_v_i32m1((vec_alias_int32_t *) (ptr), riscv_vsll_vx_i32m1(idx, 2,   8),   8)
#define vec_gather_i32_i32_4(ptr, idx)                     __riscv_vluxei32_v_i32m1((vec_alias_int32_t *) (ptr), riscv_vsll_vx_i32m1(idx, 2,   4),   4)
#define vec_gather_i32_i32_1(ptr, idx)                     ( ((vec_alias_int32_t *) (ptr))[idx] )

#define vec_gather_i32_i64_512(ptr, idx)                   __riscv_vluxei64_v_i32m1((vec_alias_int32_t *) (ptr), __riscv_vlmul_ext_v_i64m1_i64m2(riscv_vsll_vx_i64m1(idx, 2, 512)), 512)
#define vec_gather_i32_i64_256(ptr, idx)                   __riscv_vluxei64_v_i32m1((vec_alias_int32_t *) (ptr), __riscv_vlmul_ext_v_i64m1_i64m2(riscv_vsll_vx_i64m1(idx, 2, 256)), 256)
#define vec_gather_i32_i64_32(ptr, idx)                    __riscv_vluxei64_v_i32m1((vec_alias_int32_t *) (ptr), __riscv_vlmul_ext_v_i64m1_i64m2(riscv_vsll_vx_i64m1(idx, 2,  32)),  32)
#define vec_gather_i32_i64_16(ptr, idx)                    __riscv_vluxei64_v_i32m1((vec_alias_int32_t *) (ptr), __riscv_vlmul_ext_v_i64m1_i64m2(riscv_vsll_vx_i64m1(idx, 2,  16)),  16)
#define vec_gather_i32_i64_8(ptr, idx)                     __riscv_vluxei64_v_i32m1((vec_alias_int32_t *) (ptr), __riscv_vlmul_ext_v_i64m1_i64m2(riscv_vsll_vx_i64m1(idx, 2,   8)),   8)
#define vec_gather_i32_i64_4(ptr, idx)                     __riscv_vluxei64_v_i32m1((vec_alias_int32_t *) (ptr), __riscv_vlmul_ext_v_i64m1_i64m2(riscv_vsll_vx_i64m1(idx, 2,   4)),   4)
#define vec_gather_i32_i64_1(ptr, idx)                     ( ((vec_alias_int32_t *) (ptr))[idx] )


//------------------------------------------------------------------------------------------------------------------------------------------
//- Operations
//------------------------------------------------------------------------------------------------------------------------------------------

#define vec_add_i32_512(a, b)                              __riscv_vadd_vv_i32m1(a, b, 512)
#define vec_add_i32_256(a, b)                              __riscv_vadd_vv_i32m1(a, b, 256)
#define vec_add_i32_32(a, b)                               __riscv_vadd_vv_i32m1(a, b,  32)
#define vec_add_i32_16(a, b)                               __riscv_vadd_vv_i32m1(a, b,  16)
#define vec_add_i32_8(a, b)                                __riscv_vadd_vv_i32m1(a, b,   8)
#define vec_add_i32_4(a, b)                                __riscv_vadd_vv_i32m1(a, b,   4)
#define vec_add_i32_1(a, b)                                ( (a + b) )

#define vec_sub_i32_512(a, b)                              __riscv_vsub_vv_i32m1(a, b, 512)
#define vec_sub_i32_256(a, b)                              __riscv_vsub_vv_i32m1(a, b, 256)
#define vec_sub_i32_32(a, b)                               __riscv_vsub_vv_i32m1(a, b,  32)
#define vec_sub_i32_16(a, b)                               __riscv_vsub_vv_i32m1(a, b,  16)
#define vec_sub_i32_8(a, b)                                __riscv_vsub_vv_i32m1(a, b,   8)
#define vec_sub_i32_4(a, b)                                __riscv_vsub_vv_i32m1(a, b,   4)
#define vec_sub_i32_1(a, b)                                ( (a - b) )

#define vec_mul_i32_512(a, b)                              __riscv_vmul_vv_i32m1(a, b, 512)
#define vec_mul_i32_256(a, b)                              __riscv_vmul_vv_i32m1(a, b, 256)
#define vec_mul_i32_32(a, b)                               __riscv_vmul_vv_i32m1(a, b,  32)
#define vec_mul_i32_16(a, b)                               __riscv_vmul_vv_i32m1(a, b,  16)
#define vec_mul_i32_8(a, b)                                __riscv_vmul_vv_i32m1(a, b,   8)
#define vec_mul_i32_4(a, b)                                __riscv_vmul_vv_i32m1(a, b,   4)
#define vec_mul_i32_1(a, b)                                ( (a * b) )

#define vec_div_i32_512(a, b)                              __riscv_vdiv_vv_i32m1(a, b, 512)
#define vec_div_i32_256(a, b)                              __riscv_vdiv_vv_i32m1(a, b, 256)
#define vec_div_i32_32(a, b)                               __riscv_vdiv_vv_i32m1(a, b,  32)
#define vec_div_i32_16(a, b)                               __riscv_vdiv_vv_i32m1(a, b,  16)
#define vec_div_i32_8(a, b)                                __riscv_vdiv_vv_i32m1(a, b,   8)
#define vec_div_i32_4(a, b)                                __riscv_vdiv_vv_i32m1(a, b,   4)
#define vec_div_i32_1(a, b)                                ( (a / b) )

#define vec_and_i32_512(a, b)                              __riscv_vand_vv_i32m1(a, b, 512)
#define vec_and_i32_256(a, b)                              __riscv_vand_vv_i32m1(a, b, 256)
#define vec_and_i32_32(a, b)                               __riscv_vand_vv_i32m1(a, b,  32)
#define vec_and_i32_16(a, b)                               __riscv_vand_vv_i32m1(a, b,  16)
#define vec_and_i32_8(a, b)                                __riscv_vand_vv_i32m1(a, b,   8)
#define vec_and_i32_4(a, b)                                __riscv_vand_vv_i32m1(a, b,   4)
#define vec_and_i32_1(a, b)                                ( (a & b) )

#define vec_or_i32_512(a, b)                               __riscv_vor_vv_i32m1(a, b, 512)
#define vec_or_i32_256(a, b)                               __riscv_vor_vv_i32m1(a, b, 256)
#define vec_or_i32_32(a, b)                                __riscv_vor_vv_i32m1(a, b,  32)
#define vec_or_i32_16(a, b)                                __riscv_vor_vv_i32m1(a, b,  16)
#define vec_or_i32_8(a, b)                                 __riscv_vor_vv_i32m1(a, b,   8)
#define vec_or_i32_4(a, b)                                 __riscv_vor_vv_i32m1(a, b,   4)
#define vec_or_i32_1(a, b)                                 ( (a | b) )

#define vec_not_i32_512(a)                                 __riscv_vnot_v_i32m1(a, 512)
#define vec_not_i32_256(a)                                 __riscv_vnot_v_i32m1(a, 256)
#define vec_not_i32_32(a)                                  __riscv_vnot_v_i32m1(a,  32)
#define vec_not_i32_16(a)                                  __riscv_vnot_v_i32m1(a,  16)
#define vec_not_i32_8(a)                                   __riscv_vnot_v_i32m1(a,   8)
#define vec_not_i32_4(a)                                   __riscv_vnot_v_i32m1(a,   4)
#define vec_not_i32_1(a)                                   ( (a ^ -1) )

#define vec_xor_i32_512(a, b)                              __riscv_vxor_vv_i32m1(a, b, 512)
#define vec_xor_i32_256(a, b)                              __riscv_vxor_vv_i32m1(a, b, 256)
#define vec_xor_i32_32(a, b)                               __riscv_vxor_vv_i32m1(a, b,  32)
#define vec_xor_i32_16(a, b)                               __riscv_vxor_vv_i32m1(a, b,  16)
#define vec_xor_i32_8(a, b)                                __riscv_vxor_vv_i32m1(a, b,   8)
#define vec_xor_i32_4(a, b)                                __riscv_vxor_vv_i32m1(a, b,   4)
#define vec_xor_i32_1(a, b)                                ( (a ^ b) )


//------------------------------------------------------------------------------------------------------------------------------------------
//- Shift
//------------------------------------------------------------------------------------------------------------------------------------------

#define vec_slli_i32_512(a, imm8)                          riscv_vsll_vx_i32m1(a, imm8, 512)
#define vec_slli_i32_256(a, imm8)                          riscv_vsll_vx_i32m1(a, imm8, 256)
#define vec_slli_i32_32(a, imm8)                           riscv_vsll_vx_i32m1(a, imm8,  32)
#define vec_slli_i32_16(a, imm8)                           riscv_vsll_vx_i32m1(a, imm8,  16)
#define vec_slli_i32_8(a, imm8)                            riscv_vsll_vx_i32m1(a, imm8,   8)
#define vec_slli_i32_4(a, imm8)                            riscv_vsll_vx_i32m1(a, imm8,   4)
#define vec_slli_i32_1(a, imm8)                            ( (int32_t) ((imm8 < 32) ? ((uint32_t) a)<<imm8 : 0) )
#define riscv_vsll_vx_i32m1(a, imm8, vl)                   __riscv_vsll_vx_i32m1(a, (uint32_t) imm8, vl)

#define vec_srli_i32_512(a, imm8)                          riscv_vsrl_vx_u32m1(a, imm8, 512)
#define vec_srli_i32_256(a, imm8)                          riscv_vsrl_vx_u32m1(a, imm8, 256)
#define vec_srli_i32_32(a, imm8)                           riscv_vsrl_vx_u32m1(a, imm8,  32)
#define vec_srli_i32_16(a, imm8)                           riscv_vsrl_vx_u32m1(a, imm8,  16)
#define vec_srli_i32_8(a, imm8)                            riscv_vsrl_vx_u32m1(a, imm8,   8)
#define vec_srli_i32_4(a, imm8)                            riscv_vsrl_vx_u32m1(a, imm8,   4)
#define vec_srli_i32_1(a, imm8)                            ( (int32_t) ((imm8 < 32) ? ((uint32_t) a)>>imm8 : 0) )
#define riscv_vsrl_vx_u32m1(a, imm8, vl)                   __riscv_vreinterpret_v_u32m1_i32m1( __riscv_vsrl_vx_u32m1(__riscv_vreinterpret_v_i32m1_u32m1(a), (uint32_t) imm8, vl) )

#define vec_srai_i32_512(a, imm8)                          __riscv_vsra_vx_i32m1(a, imm8, 512)
#define vec_srai_i32_256(a, imm8)                          __riscv_vsra_vx_i32m1(a, imm8, 256)
#define vec_srai_i32_32(a, imm8)                           __riscv_vsra_vx_i32m1(a, imm8,  32)
#define vec_srai_i32_16(a, imm8)                           __riscv_vsra_vx_i32m1(a, imm8,  16)
#define vec_srai_i32_8(a, imm8)                            __riscv_vsra_vx_i32m1(a, imm8,   8)
#define vec_srai_i32_4(a, imm8)                            __riscv_vsra_vx_i32m1(a, imm8,   4)
#define vec_srai_i32_1(a, imm8)                            ((imm8 < 32) ? ((int32_t) a)>>imm8 : (((int32_t) a) < 0) ? -1 : 0)

#define vec_sllv_i32_512(a, count)                         ({ vec_mask_m32_512_t _mask = __riscv_vmsge_vx_i32m1_b32(count, 32, 512); vec_i32_512_t _shifted = riscv_vsll_vv_i32m1(a, count, 512); __riscv_vmerge_vxm_i32m1(_shifted, 0, _mask, 512); })
#define vec_sllv_i32_256(a, count)                         ({ vec_mask_m32_256_t _mask = __riscv_vmsge_vx_i32m1_b32(count, 32, 256); vec_i32_256_t _shifted = riscv_vsll_vv_i32m1(a, count, 256); __riscv_vmerge_vxm_i32m1(_shifted, 0, _mask, 256); })
#define vec_sllv_i32_32(a, count)                          ({ vec_mask_m32_32_t  _mask = __riscv_vmsge_vx_i32m1_b32(count, 32,  32); vec_i32_32_t _shifted =  riscv_vsll_vv_i32m1(a, count,  32); __riscv_vmerge_vxm_i32m1(_shifted, 0, _mask,  32); })
#define vec_sllv_i32_16(a, count)                          ({ vec_mask_m32_16_t  _mask = __riscv_vmsge_vx_i32m1_b32(count, 32,  16); vec_i32_16_t _shifted =  riscv_vsll_vv_i32m1(a, count,  16); __riscv_vmerge_vxm_i32m1(_shifted, 0, _mask,  16); })
#define vec_sllv_i32_8(a, count)                           ({ vec_mask_m32_8_t   _mask = __riscv_vmsge_vx_i32m1_b32(count, 32,   8); vec_i32_8_t  _shifted =  riscv_vsll_vv_i32m1(a, count,   8); __riscv_vmerge_vxm_i32m1(_shifted, 0, _mask,   8); })
#define vec_sllv_i32_4(a, count)                           ({ vec_mask_m32_4_t   _mask = __riscv_vmsge_vx_i32m1_b32(count, 32,   4); vec_i32_4_t  _shifted =  riscv_vsll_vv_i32m1(a, count,   4); __riscv_vmerge_vxm_i32m1(_shifted, 0, _mask,   4); })
#define vec_sllv_i32_1(a, count)                           ( (int32_t) ((count < 32) ? ((uint32_t) a)<<count : 0) )
#define riscv_vsll_vv_i32m1(a, count, vl)                  __riscv_vsll_vv_i32m1(a, __riscv_vreinterpret_v_i32m1_u32m1(count), vl)

#define vec_srlv_i32_512(a, count)                         ({ vec_mask_m32_512_t _mask = __riscv_vmsge_vx_i32m1_b32(count, 32, 512); vec_i32_512_t _shifted = riscv_vsrl_vv_u32m1(a, count, 512); __riscv_vmerge_vxm_i32m1(_shifted, 0, _mask, 512); })
#define vec_srlv_i32_256(a, count)                         ({ vec_mask_m32_256_t _mask = __riscv_vmsge_vx_i32m1_b32(count, 32, 256); vec_i32_256_t _shifted = riscv_vsrl_vv_u32m1(a, count, 256); __riscv_vmerge_vxm_i32m1(_shifted, 0, _mask, 256); })
#define vec_srlv_i32_32(a, count)                          ({ vec_mask_m32_32_t  _mask = __riscv_vmsge_vx_i32m1_b32(count, 32,  32); vec_i32_32_t _shifted =  riscv_vsrl_vv_u32m1(a, count,  32); __riscv_vmerge_vxm_i32m1(_shifted, 0, _mask,  32); })
#define vec_srlv_i32_16(a, count)                          ({ vec_mask_m32_16_t  _mask = __riscv_vmsge_vx_i32m1_b32(count, 32,  16); vec_i32_16_t _shifted =  riscv_vsrl_vv_u32m1(a, count,  16); __riscv_vmerge_vxm_i32m1(_shifted, 0, _mask,  16); })
#define vec_srlv_i32_8(a, count)                           ({ vec_mask_m32_8_t   _mask = __riscv_vmsge_vx_i32m1_b32(count, 32,   8); vec_i32_8_t  _shifted =  riscv_vsrl_vv_u32m1(a, count,   8); __riscv_vmerge_vxm_i32m1(_shifted, 0, _mask,   8); })
#define vec_srlv_i32_4(a, count)                           ({ vec_mask_m32_4_t   _mask = __riscv_vmsge_vx_i32m1_b32(count, 32,   4); vec_i32_4_t  _shifted =  riscv_vsrl_vv_u32m1(a, count,   4); __riscv_vmerge_vxm_i32m1(_shifted, 0, _mask,   4); })
#define vec_srlv_i32_1(a, count)                           ( (int32_t) ((count < 32) ? ((uint32_t) a)>>count : 0) )
#define riscv_vsrl_vv_u32m1(a, count, vl)                  __riscv_vreinterpret_v_u32m1_i32m1(__riscv_vsrl_vv_u32m1(__riscv_vreinterpret_v_i32m1_u32m1(a), __riscv_vreinterpret_v_i32m1_u32m1(count), vl) )

#define vec_srav_i32_512(a, count)                         ({ vec_mask_m32_512_t _mask = __riscv_vmsge_vx_i32m1_b32(count, 32, 512); vec_i32_512_t _count = __riscv_vmerge_vxm_i32m1(count, 31, _mask, 512); __riscv_vsra_vv_i32m1(a, _count, 512); })
#define vec_srav_i32_256(a, count)                         ({ vec_mask_m32_256_t _mask = __riscv_vmsge_vx_i32m1_b32(count, 32, 256); vec_i32_256_t _count = __riscv_vmerge_vxm_i32m1(count, 31, _mask, 256); __riscv_vsra_vv_i32m1(a, _count, 256); })
#define vec_srav_i32_32(a, count)                          ({ vec_mask_m32_32_t  _mask = __riscv_vmsge_vx_i32m1_b32(count, 32,  32); vec_i32_32_t _count =  __riscv_vmerge_vxm_i32m1(count, 31, _mask,  32); __riscv_vsra_vv_i32m1(a, _count,  32); })
#define vec_srav_i32_16(a, count)                          ({ vec_mask_m32_16_t  _mask = __riscv_vmsge_vx_i32m1_b32(count, 32,  16); vec_i32_16_t _count =  __riscv_vmerge_vxm_i32m1(count, 31, _mask,  16); __riscv_vsra_vv_i32m1(a, _count,  16); })
#define vec_srav_i32_8(a, count)                           ({ vec_mask_m32_8_t   _mask = __riscv_vmsge_vx_i32m1_b32(count, 32,   8); vec_i32_8_t  _count =  __riscv_vmerge_vxm_i32m1(count, 31, _mask,   8); __riscv_vsra_vv_i32m1(a, _count,   8); })
#define vec_srav_i32_4(a, count)                           ({ vec_mask_m32_4_t   _mask = __riscv_vmsge_vx_i32m1_b32(count, 32,   4); vec_i32_4_t  _count =  __riscv_vmerge_vxm_i32m1(count, 31, _mask,   4); __riscv_vsra_vv_i32m1(a, _count,   4); })
#define vec_srav_i32_1(a, count)                           ((count < 32) ? ((int32_t) a)>>count : (((int32_t) a) < 0) ? -1 : 0)


//------------------------------------------------------------------------------------------------------------------------------------------
//- Compare
//------------------------------------------------------------------------------------------------------------------------------------------

#define vec_cmpeq_i32_512(a, b)                            __riscv_vmseq_vv_i32m1_b32(a, b, 512)
#define vec_cmpeq_i32_256(a, b)                            __riscv_vmseq_vv_i32m1_b32(a, b, 256)
#define vec_cmpeq_i32_32(a, b)                             __riscv_vmseq_vv_i32m1_b32(a, b,  32)
#define vec_cmpeq_i32_16(a, b)                             __riscv_vmseq_vv_i32m1_b32(a, b,  16)
#define vec_cmpeq_i32_8(a, b)                              __riscv_vmseq_vv_i32m1_b32(a, b,   8)
#define vec_cmpeq_i32_4(a, b)                              __riscv_vmseq_vv_i32m1_b32(a, b,   4)
#define vec_cmpeq_i32_1(a, b)                              (a == b)

#define vec_cmpgt_i32_512(a, b)                            __riscv_vmsgt_vv_i32m1_b32(a, b, 512)
#define vec_cmpgt_i32_256(a, b)                            __riscv_vmsgt_vv_i32m1_b32(a, b, 256)
#define vec_cmpgt_i32_32(a, b)                             __riscv_vmsgt_vv_i32m1_b32(a, b,  32)
#define vec_cmpgt_i32_16(a, b)                             __riscv_vmsgt_vv_i32m1_b32(a, b,  16)
#define vec_cmpgt_i32_8(a, b)                              __riscv_vmsgt_vv_i32m1_b32(a, b,   8)
#define vec_cmpgt_i32_4(a, b)                              __riscv_vmsgt_vv_i32m1_b32(a, b,   4)
#define vec_cmpgt_i32_1(a, b)                              (a > b)


//------------------------------------------------------------------------------------------------------------------------------------------
//- Shuffle - Permute
//------------------------------------------------------------------------------------------------------------------------------------------

#define vec_permute_i32_512(a, idx)                        riscv_vrgather_vv_i32m1(a, idx, 512);
#define vec_permute_i32_256(a, idx)                        riscv_vrgather_vv_i32m1(a, idx, 256);
#define vec_permute_i32_32(a, idx)                         riscv_vrgather_vv_i32m1(a, idx,  32);
#define vec_permute_i32_16(a, idx)                         riscv_vrgather_vv_i32m1(a, idx,  16);
#define vec_permute_i32_8(a, idx)                          riscv_vrgather_vv_i32m1(a, idx,   8);
#define vec_permute_i32_4(a, idx)                          riscv_vrgather_vv_i32m1(a, idx,   4);
#define vec_permute_i32_1(a, idx)                          a
#define riscv_vrgather_vv_i32m1(a, idx, vl)                __riscv_vrgather_vv_i32m1(a, __riscv_vreinterpret_v_i32m1_u32m1(idx), vl);


#endif /* VECTORIZATION_RISCV_SVV_I32_H */

