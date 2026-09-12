#ifndef VECTORIZATION_RISCV_SVV_F32_H
#define VECTORIZATION_RISCV_SVV_F32_H

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <riscv_vector.h>

#include "macros/cpp_defines.h"
#include "macros/macrolib.h"

#include "vectorization/vectorization_util.h"
#include "vectorization_riscv_svv_m32.h"


typedef float        __attribute__((may_alias))  vec_alias_float32_t;

typedef float        __attribute__((may_alias))  vec_f32_1_t;
typedef vfloat32m1_t __attribute__((may_alias))  vec_f32_2_t;
typedef vfloat32m1_t __attribute__((may_alias))  vec_f32_4_t;
typedef vfloat32m1_t __attribute__((may_alias))  vec_f32_8_t;
typedef vfloat32m1_t __attribute__((may_alias))  vec_f32_16_t;
typedef vfloat32m1_t __attribute__((may_alias))  vec_f32_32_t;
typedef vfloat32m1_t __attribute__((may_alias))  vec_f32_256_t;
typedef vfloat32m1_t __attribute__((may_alias))  vec_f32_512_t;

#define vec_len_default_f32     8
#define vec_len_default_f32_f2  4


//------------------------------------------------------------------------------------------------------------------------------------------
//- Cast
//------------------------------------------------------------------------------------------------------------------------------------------

#define vec_cast_to_i32_f32_32(val)                        vec_loop_expr(vec_i32_32_t, 2, _tmp, _i, (_tmp).v[_i] = (val).vi[_i];)
#define vec_cast_to_i32_f32_16(val)                        ( (vec_i32_16_t) { .v = (val).v } )
#define vec_cast_to_i32_f32_8(val)                         ( (vec_i32_8_t)  { .v = (val).v } )
#define vec_cast_to_i32_f32_4(val)                         ( (vec_i32_4_t)  { .v = (val).v } )
#define vec_cast_to_i32_f32_1(val)                         ( (vec_i32_1_t)  { .v = (val).v } )

#define vec_cast_to_i64_f32_32(val)                        vec_loop_expr(vec_i64_16_t, 2, _tmp, _i, (_tmp).v[_i] = (val).vi[_i];)
#define vec_cast_to_i64_f32_16(val)                        ( (vec_i64_8_t)  { .v = (val).vi } )
#define vec_cast_to_i64_f32_8(val)                         ( (vec_i64_4_t)  { .v = (val).vi } )
#define vec_cast_to_i64_f32_4(val)                         ( (vec_i64_2_t)  { .v = (val).vi } )
#define vec_cast_to_i64_f32_1(val)                         ( (vec_i64_1_t)  { .v = (val).vi } )

#define vec_cast_to_f32_f32_32(val)                        (val)
#define vec_cast_to_f32_f32_16(val)                        (val)
#define vec_cast_to_f32_f32_8(val)                         (val)
#define vec_cast_to_f32_f32_4(val)                         (val)
#define vec_cast_to_f32_f32_1(val)                         (val)

#define vec_cast_to_f64_f32_32(val)                        vec_loop_expr(vec_f64_16_t, 2, _tmp, _i, (_tmp).v[_i] = (val).vf64[_i];)
#define vec_cast_to_f64_f32_16(val)                        ( (vec_f64_8_t)  { .v = (val).vf64 } )
#define vec_cast_to_f64_f32_8(val)                         ( (vec_f64_4_t)  { .v = (val).vf64 } )
#define vec_cast_to_f64_f32_4(val)                         ( (vec_f64_2_t)  { .v = (val).vf64 } )
#define vec_cast_to_f64_f32_1(val)                         ( (vec_f64_1_t)  { .v = (val).vf64 } )


//------------------------------------------------------------------------------------------------------------------------------------------
//- Set - Load - Store
//------------------------------------------------------------------------------------------------------------------------------------------

#define vec_elem_get_f32_512(vec, index)                   ( ((vec_alias_float32_t *) &vec)[index] )
#define vec_elem_get_f32_256(vec, index)                   ( ((vec_alias_float32_t *) &vec)[index] )
#define vec_elem_get_f32_32(vec, index)                    ( ((vec_alias_float32_t *) &vec)[index] )
#define vec_elem_get_f32_16(vec, index)                    ( ((vec_alias_float32_t *) &vec)[index] )
#define vec_elem_get_f32_8(vec, index)                     ( ((vec_alias_float32_t *) &vec)[index] )
#define vec_elem_get_f32_4(vec, index)                     ( ((vec_alias_float32_t *) &vec)[index] )
#define vec_elem_get_f32_1(vec, index)                     vec

#define vec_elem_set_f32_512(vec, index, expr)             do { ((vec_alias_float32_t *) &vec)[index] = (expr); } while (0)
#define vec_elem_set_f32_256(vec, index, expr)             do { ((vec_alias_float32_t *) &vec)[index] = (expr); } while (0)
#define vec_elem_set_f32_32(vec, index, expr)              do { ((vec_alias_float32_t *) &vec)[index] = (expr); } while (0)
#define vec_elem_set_f32_16(vec, index, expr)              do { ((vec_alias_float32_t *) &vec)[index] = (expr); } while (0)
#define vec_elem_set_f32_8(vec, index, expr)               do { ((vec_alias_float32_t *) &vec)[index] = (expr); } while (0)
#define vec_elem_set_f32_4(vec, index, expr)               do { ((vec_alias_float32_t *) &vec)[index] = (expr); } while (0)
#define vec_elem_set_f32_1(vec, index, expr)               do { vec = (expr); } while (0)

#define vec_array_f32_512(vec)                             ( (vec_alias_float32_t *) &vec )
#define vec_array_f32_256(vec)                             ( (vec_alias_float32_t *) &vec )
#define vec_array_f32_32(vec)                              ( (vec_alias_float32_t *) &vec )
#define vec_array_f32_16(vec)                              ( (vec_alias_float32_t *) &vec )
#define vec_array_f32_8(vec)                               ( (vec_alias_float32_t *) &vec )
#define vec_array_f32_4(vec)                               ( (vec_alias_float32_t *) &vec )
#define vec_array_f32_1(vec)                               ( (vec_alias_float32_t *) &vec )

#define vec_set1_f32_512(val)                              __riscv_vfmv_v_f_f32m1((val), 512)
#define vec_set1_f32_256(val)                              __riscv_vfmv_v_f_f32m1((val), 256)
#define vec_set1_f32_32(val)                               __riscv_vfmv_v_f_f32m1((val),  32)
#define vec_set1_f32_16(val)                               __riscv_vfmv_v_f_f32m1((val),  16)
#define vec_set1_f32_8(val)                                __riscv_vfmv_v_f_f32m1((val),   8)
#define vec_set1_f32_4(val)                                __riscv_vfmv_v_f_f32m1((val),   4)
#define vec_set1_f32_1(val)                                ( (vec_alias_float32_t) (val) )

#define vec_set_iter_f32_512(iter, expr)                   ({ vec_f32_512_t _buf; vec_loop_stmt( 512, iter, ((vec_alias_float32_t *) &_buf)[iter] = (expr););  _buf; })
#define vec_set_iter_f32_256(iter, expr)                   ({ vec_f32_256_t _buf; vec_loop_stmt( 256, iter, ((vec_alias_float32_t *) &_buf)[iter] = (expr););  _buf; })
#define vec_set_iter_f32_32(iter, expr)                    ({ vec_f32_32_t  _buf; vec_loop_stmt(  32, iter, ((vec_alias_float32_t *) &_buf)[iter] = (expr););  _buf; })
#define vec_set_iter_f32_16(iter, expr)                    ({ vec_f32_16_t  _buf; vec_loop_stmt(  16, iter, ((vec_alias_float32_t *) &_buf)[iter] = (expr););  _buf; })
#define vec_set_iter_f32_8(iter, expr)                     ({ vec_f32_8_t   _buf; vec_loop_stmt(   8, iter, ((vec_alias_float32_t *) &_buf)[iter] = (expr););  _buf; })
#define vec_set_iter_f32_4(iter, expr)                     ({ vec_f32_4_t   _buf; vec_loop_stmt(   4, iter, ((vec_alias_float32_t *) &_buf)[iter] = (expr););  _buf; })
#define vec_set_iter_f32_1(iter, expr)                     ( (vec_alias_float32_t) vec_iter_expr_1(iter, 0, (expr)) )

#define vec_loadu_f32_512(ptr)                             __riscv_vle32_v_f32m1((vec_alias_float32_t *) (ptr), 512)
#define vec_loadu_f32_256(ptr)                             __riscv_vle32_v_f32m1((vec_alias_float32_t *) (ptr), 256)
#define vec_loadu_f32_32(ptr)                              __riscv_vle32_v_f32m1((vec_alias_float32_t *) (ptr),  32)
#define vec_loadu_f32_16(ptr)                              __riscv_vle32_v_f32m1((vec_alias_float32_t *) (ptr),  16)
#define vec_loadu_f32_8(ptr)                               __riscv_vle32_v_f32m1((vec_alias_float32_t *) (ptr),   8)
#define vec_loadu_f32_4(ptr)                               __riscv_vle32_v_f32m1((vec_alias_float32_t *) (ptr),   4)
#define vec_loadu_f32_1(ptr)                               ( (*((vec_alias_float32_t *) (ptr))) )

#define vec_loadu_maskedz_f32_512(ptr, mask)               ( __riscv_vmerge_vvm_f32m1(__riscv_vfmv_v_f_f32m1(0, 512), __riscv_vle32_v_f32m1_m(mask, (vec_alias_float32_t *) (ptr), 512), mask, 512) )
#define vec_loadu_maskedz_f32_256(ptr, mask)               ( __riscv_vmerge_vvm_f32m1(__riscv_vfmv_v_f_f32m1(0, 256), __riscv_vle32_v_f32m1_m(mask, (vec_alias_float32_t *) (ptr), 256), mask, 256) )
#define vec_loadu_maskedz_f32_32(ptr, mask)                ( __riscv_vmerge_vvm_f32m1(__riscv_vfmv_v_f_f32m1(0,  32), __riscv_vle32_v_f32m1_m(mask, (vec_alias_float32_t *) (ptr),  32), mask,  32) )
#define vec_loadu_maskedz_f32_16(ptr, mask)                ( __riscv_vmerge_vvm_f32m1(__riscv_vfmv_v_f_f32m1(0,  16), __riscv_vle32_v_f32m1_m(mask, (vec_alias_float32_t *) (ptr),  16), mask,  16) )
#define vec_loadu_maskedz_f32_8(ptr, mask)                 ( __riscv_vmerge_vvm_f32m1(__riscv_vfmv_v_f_f32m1(0,   8), __riscv_vle32_v_f32m1_m(mask, (vec_alias_float32_t *) (ptr),   8), mask,   8) )
#define vec_loadu_maskedz_f32_4(ptr, mask)                 ( __riscv_vmerge_vvm_f32m1(__riscv_vfmv_v_f_f32m1(0,   4), __riscv_vle32_v_f32m1_m(mask, (vec_alias_float32_t *) (ptr),   4), mask,   4) )
#define vec_loadu_maskedz_f32_1(ptr, mask)                 ( (mask) ? (*((vec_alias_float32_t *) (ptr))) : 0 )

#define vec_storeu_f32_512(ptr, vec)                       __riscv_vse32_v_f32m1((vec_alias_float32_t *) (ptr), vec, 512)
#define vec_storeu_f32_256(ptr, vec)                       __riscv_vse32_v_f32m1((vec_alias_float32_t *) (ptr), vec, 256)
#define vec_storeu_f32_32(ptr, vec)                        __riscv_vse32_v_f32m1((vec_alias_float32_t *) (ptr), vec,  32)
#define vec_storeu_f32_16(ptr, vec)                        __riscv_vse32_v_f32m1((vec_alias_float32_t *) (ptr), vec,  16)
#define vec_storeu_f32_8(ptr, vec)                         __riscv_vse32_v_f32m1((vec_alias_float32_t *) (ptr), vec,   8)
#define vec_storeu_f32_4(ptr, vec)                         __riscv_vse32_v_f32m1((vec_alias_float32_t *) (ptr), vec,   4)
#define vec_storeu_f32_1(ptr, vec)                         do { (*((vec_alias_float32_t *) (ptr))) = (vec); } while (0)

#define vec_storeu_masked_f32_512(ptr, vec, mask)          __riscv_vse32_v_f32m1_m(mask, (vec_alias_float32_t *) (ptr), vec, 512)
#define vec_storeu_masked_f32_256(ptr, vec, mask)          __riscv_vse32_v_f32m1_m(mask, (vec_alias_float32_t *) (ptr), vec, 256)
#define vec_storeu_masked_f32_32(ptr, vec, mask)           __riscv_vse32_v_f32m1_m(mask, (vec_alias_float32_t *) (ptr), vec,  32)
#define vec_storeu_masked_f32_16(ptr, vec, mask)           __riscv_vse32_v_f32m1_m(mask, (vec_alias_float32_t *) (ptr), vec,  16)
#define vec_storeu_masked_f32_8(ptr, vec, mask)            __riscv_vse32_v_f32m1_m(mask, (vec_alias_float32_t *) (ptr), vec,   8)
#define vec_storeu_masked_f32_4(ptr, vec, mask)            __riscv_vse32_v_f32m1_m(mask, (vec_alias_float32_t *) (ptr), vec,   4)
#define vec_storeu_masked_f32_1(ptr, vec, mask)            do { if (mask) (*((vec_alias_float32_t *) (ptr))) = (vec); } while (0)

#define vec_gather_f32_i32_512(ptr, idx)                   __riscv_vluxei32_v_f32m1((vec_alias_float32_t *) (ptr), __riscv_vreinterpret_v_i32m1_u32m1(riscv_vsll_vx_i32m1(idx, 2, 512)), 512)
#define vec_gather_f32_i32_256(ptr, idx)                   __riscv_vluxei32_v_f32m1((vec_alias_float32_t *) (ptr), __riscv_vreinterpret_v_i32m1_u32m1(riscv_vsll_vx_i32m1(idx, 2, 256)), 256)
#define vec_gather_f32_i32_32(ptr, idx)                    __riscv_vluxei32_v_f32m1((vec_alias_float32_t *) (ptr), __riscv_vreinterpret_v_i32m1_u32m1(riscv_vsll_vx_i32m1(idx, 2,  32)),  32)
#define vec_gather_f32_i32_16(ptr, idx)                    __riscv_vluxei32_v_f32m1((vec_alias_float32_t *) (ptr), __riscv_vreinterpret_v_i32m1_u32m1(riscv_vsll_vx_i32m1(idx, 2,  16)),  16)
#define vec_gather_f32_i32_8(ptr, idx)                     __riscv_vluxei32_v_f32m1((vec_alias_float32_t *) (ptr), __riscv_vreinterpret_v_i32m1_u32m1(riscv_vsll_vx_i32m1(idx, 2,   8)),   8)
#define vec_gather_f32_i32_4(ptr, idx)                     __riscv_vluxei32_v_f32m1((vec_alias_float32_t *) (ptr), __riscv_vreinterpret_v_i32m1_u32m1(riscv_vsll_vx_i32m1(idx, 2,   4)),   4)
#define vec_gather_f32_i32_1(ptr, idx)                     ( ((vec_alias_float32_t *) (ptr))[idx] )

#define vec_gather_f32_i64_512(ptr, idx)                   __riscv_vluxei64_v_f32m1((vec_alias_float32_t *) (ptr), __riscv_vreinterpret_v_i64m1_u64m1(riscv_vsll_vx_i64m1(idx, 2, 512)), 512)
#define vec_gather_f32_i64_256(ptr, idx)                   __riscv_vluxei64_v_f32m1((vec_alias_float32_t *) (ptr), __riscv_vreinterpret_v_i64m1_u64m1(riscv_vsll_vx_i64m1(idx, 2, 256)), 256)
#define vec_gather_f32_i64_32(ptr, idx)                    __riscv_vluxei64_v_f32m1((vec_alias_float32_t *) (ptr), __riscv_vreinterpret_v_i64m1_u64m1(riscv_vsll_vx_i64m1(idx, 2,  32)),  32)
#define vec_gather_f32_i64_16(ptr, idx)                    __riscv_vluxei64_v_f32m1((vec_alias_float32_t *) (ptr), __riscv_vreinterpret_v_i64m1_u64m1(riscv_vsll_vx_i64m1(idx, 2,  16)),  16)
#define vec_gather_f32_i64_8(ptr, idx)                     __riscv_vluxei64_v_f32m1((vec_alias_float32_t *) (ptr), __riscv_vreinterpret_v_i64m1_u64m1(riscv_vsll_vx_i64m1(idx, 2,   8)),   8)
#define vec_gather_f32_i64_4(ptr, idx)                     __riscv_vluxei64_v_f32m1((vec_alias_float32_t *) (ptr), __riscv_vreinterpret_v_i64m1_u64m1(riscv_vsll_vx_i64m1(idx, 2,   4)),   4)
#define vec_gather_f32_i64_1(ptr, idx)                     ( ((vec_alias_float32_t *) (ptr))[idx] )


//------------------------------------------------------------------------------------------------------------------------------------------
//- Operations
//------------------------------------------------------------------------------------------------------------------------------------------

#define vec_add_f32_512(a, b)                              __riscv_vfadd_vv_f32m1(a, b, 512)
#define vec_add_f32_256(a, b)                              __riscv_vfadd_vv_f32m1(a, b, 256)
#define vec_add_f32_32(a, b)                               __riscv_vfadd_vv_f32m1(a, b,  32)
#define vec_add_f32_16(a, b)                               __riscv_vfadd_vv_f32m1(a, b,  16)
#define vec_add_f32_8(a, b)                                __riscv_vfadd_vv_f32m1(a, b,   8)
#define vec_add_f32_4(a, b)                                __riscv_vfadd_vv_f32m1(a, b,   4)
#define vec_add_f32_1(a, b)                                ( (a + b) )

#define vec_sub_f32_512(a, b)                              __riscv_vfsub_vv_f32m1(a, b, 512)
#define vec_sub_f32_256(a, b)                              __riscv_vfsub_vv_f32m1(a, b, 256)
#define vec_sub_f32_32(a, b)                               __riscv_vfsub_vv_f32m1(a, b,  32)
#define vec_sub_f32_16(a, b)                               __riscv_vfsub_vv_f32m1(a, b,  16)
#define vec_sub_f32_8(a, b)                                __riscv_vfsub_vv_f32m1(a, b,   8)
#define vec_sub_f32_4(a, b)                                __riscv_vfsub_vv_f32m1(a, b,   4)
#define vec_sub_f32_1(a, b)                                ( (a - b) )

#define vec_mul_f32_512(a, b)                              __riscv_vfmul_vv_f32m1(a, b, 512)
#define vec_mul_f32_256(a, b)                              __riscv_vfmul_vv_f32m1(a, b, 256)
#define vec_mul_f32_32(a, b)                               __riscv_vfmul_vv_f32m1(a, b,  32)
#define vec_mul_f32_16(a, b)                               __riscv_vfmul_vv_f32m1(a, b,  16)
#define vec_mul_f32_8(a, b)                                __riscv_vfmul_vv_f32m1(a, b,   8)
#define vec_mul_f32_4(a, b)                                __riscv_vfmul_vv_f32m1(a, b,   4)
#define vec_mul_f32_1(a, b)                                ( (a * b) )

#define vec_div_f32_512(a, b)                              __riscv_vfdiv_vv_f32m1(a, b, 512)
#define vec_div_f32_256(a, b)                              __riscv_vfdiv_vv_f32m1(a, b, 256)
#define vec_div_f32_32(a, b)                               __riscv_vfdiv_vv_f32m1(a, b,  32)
#define vec_div_f32_16(a, b)                               __riscv_vfdiv_vv_f32m1(a, b,  16)
#define vec_div_f32_8(a, b)                                __riscv_vfdiv_vv_f32m1(a, b,   8)
#define vec_div_f32_4(a, b)                                __riscv_vfdiv_vv_f32m1(a, b,   4)
#define vec_div_f32_1(a, b)                                ( (a / b) )

#define vec_fmadd_f32_512(a, b, c)                         __riscv_vfmadd_vv_f32m1(a, b, c, 512)
#define vec_fmadd_f32_256(a, b, c)                         __riscv_vfmadd_vv_f32m1(a, b, c, 256)
#define vec_fmadd_f32_32(a, b, c)                          __riscv_vfmadd_vv_f32m1(a, b, c,  32)
#define vec_fmadd_f32_16(a, b, c)                          __riscv_vfmadd_vv_f32m1(a, b, c,  16)
#define vec_fmadd_f32_8(a, b, c)                           __riscv_vfmadd_vv_f32m1(a, b, c,   8)
#define vec_fmadd_f32_4(a, b, c)                           __riscv_vfmadd_vv_f32m1(a, b, c,   4)
#define vec_fmadd_f32_1(a, b, c)                           ( ((a * b + c)) )


#define vec_reduce_add_f32_512(a)                          ( __riscv_vfmv_f_s_f32m1_f32(__riscv_vfredusum_vs_f32m1_f32m1(a, __riscv_vfmv_v_f_f32m1(0, 512), 512)) )
#define vec_reduce_add_f32_256(a)                          ( __riscv_vfmv_f_s_f32m1_f32(__riscv_vfredusum_vs_f32m1_f32m1(a, __riscv_vfmv_v_f_f32m1(0, 256), 256)) )
#define vec_reduce_add_f32_32(a)                           ( __riscv_vfmv_f_s_f32m1_f32(__riscv_vfredusum_vs_f32m1_f32m1(a, __riscv_vfmv_v_f_f32m1(0,  32),  32)) )
#define vec_reduce_add_f32_16(a)                           ( __riscv_vfmv_f_s_f32m1_f32(__riscv_vfredusum_vs_f32m1_f32m1(a, __riscv_vfmv_v_f_f32m1(0,  16),  16)) )
#define vec_reduce_add_f32_8(a)                            ( __riscv_vfmv_f_s_f32m1_f32(__riscv_vfredusum_vs_f32m1_f32m1(a, __riscv_vfmv_v_f_f32m1(0,   8),   8)) )
#define vec_reduce_add_f32_4(a)                            ( __riscv_vfmv_f_s_f32m1_f32(__riscv_vfredusum_vs_f32m1_f32m1(a, __riscv_vfmv_v_f_f32m1(0,   4),   4)) )
#define vec_reduce_add_f32_1(a)                            (a)


//------------------------------------------------------------------------------------------------------------------------------------------
//- Compare
//------------------------------------------------------------------------------------------------------------------------------------------

#define vec_cmpeq_f32_512(a, b)                            __riscv_vmfeq_vv_f32m1_b32(a, b, 512)
#define vec_cmpeq_f32_256(a, b)                            __riscv_vmfeq_vv_f32m1_b32(a, b, 256)
#define vec_cmpeq_f32_32(a, b)                             __riscv_vmfeq_vv_f32m1_b32(a, b,  32)
#define vec_cmpeq_f32_16(a, b)                             __riscv_vmfeq_vv_f32m1_b32(a, b,  16)
#define vec_cmpeq_f32_8(a, b)                              __riscv_vmfeq_vv_f32m1_b32(a, b,   8)
#define vec_cmpeq_f32_4(a, b)                              __riscv_vmfeq_vv_f32m1_b32(a, b,   4)
#define vec_cmpeq_f32_1(a, b)                              (a == b)

#define vec_cmpgt_f32_512(a, b)                            __riscv_vmfgt_vv_f32m1_b32(a, b, 512)
#define vec_cmpgt_f32_256(a, b)                            __riscv_vmfgt_vv_f32m1_b32(a, b, 256)
#define vec_cmpgt_f32_32(a, b)                             __riscv_vmfgt_vv_f32m1_b32(a, b,  32)
#define vec_cmpgt_f32_16(a, b)                             __riscv_vmfgt_vv_f32m1_b32(a, b,  16)
#define vec_cmpgt_f32_8(a, b)                              __riscv_vmfgt_vv_f32m1_b32(a, b,   8)
#define vec_cmpgt_f32_4(a, b)                              __riscv_vmfgt_vv_f32m1_b32(a, b,   4)
#define vec_cmpgt_f32_1(a, b)                              (a > b)


#endif /* VECTORIZATION_RISCV_SVV_F32_H */


