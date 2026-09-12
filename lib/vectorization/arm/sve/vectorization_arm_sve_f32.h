#ifndef VECTORIZATION_ARM_SVE_F32_H
#define VECTORIZATION_ARM_SVE_F32_H

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <arm_sve.h>

#include "macros/cpp_defines.h"
#include "macros/macrolib.h"

#include "vectorization/vectorization_util.h"
#include "vectorization_arm_sve_m32.h"


typedef float        __attribute__((may_alias))  vec_alias_float32_t;

/* Without the arm_sve_vector_bits attribute, the sve types have many limitations:
 *     - no sizeof().
 *     - can't include in structs or unions.
 *     - can't use pointers to address them.
 * */
#if __ARM_FEATURE_SVE_BITS==512
	#define vec_len_default_f32  16
	#define vec_len_default_f32_f2  8
	#define vec_pred_true_f32  svptrue_pat_b32(SV_VL16)
	typedef float        __attribute__((may_alias))  vec_f32_1_t;
	typedef svfloat32_t  __attribute__((arm_sve_vector_bits(512)))  vec_f32_2_t;
	typedef svfloat32_t  __attribute__((arm_sve_vector_bits(512)))  vec_f32_4_t;
	typedef svfloat32_t  __attribute__((arm_sve_vector_bits(512)))  vec_f32_8_t;
	typedef svfloat32_t  __attribute__((arm_sve_vector_bits(512)))  vec_f32_16_t;
#elif __ARM_FEATURE_SVE_BITS==256
	#define vec_len_default_f32  8
	#define vec_len_default_f32_f2  4
	#define vec_pred_true_f32  svptrue_pat_b32(SV_VL8)
	typedef float        __attribute__((may_alias))  vec_f32_1_t;
	typedef svfloat32_t  __attribute__((arm_sve_vector_bits(256)))  vec_f32_2_t;
	typedef svfloat32_t  __attribute__((arm_sve_vector_bits(256)))  vec_f32_4_t;
	typedef svfloat32_t  __attribute__((arm_sve_vector_bits(256)))  vec_f32_8_t;
	typedef svfloat32_t  __attribute__((arm_sve_vector_bits(256)))  vec_f32_16_t;
#elif __ARM_FEATURE_SVE_BITS==128
	#define vec_len_default_f32  4
	#define vec_len_default_f32_f2  2
	#define vec_pred_true_f32  svptrue_pat_b32(SV_VL4)
	typedef float        __attribute__((may_alias))  vec_f32_1_t;
	typedef svfloat32_t  __attribute__((arm_sve_vector_bits(128)))  vec_f32_2_t;
	typedef svfloat32_t  __attribute__((arm_sve_vector_bits(128)))  vec_f32_4_t;
	typedef svfloat32_t  __attribute__((arm_sve_vector_bits(128)))  vec_f32_8_t;
	typedef svfloat32_t  __attribute__((arm_sve_vector_bits(128)))  vec_f32_16_t;
#else
	#error "__ARM_FEATURE_SVE_BITS not defined"
#endif

typedef float  vec_f32_t __attribute__((may_alias));


//------------------------------------------------------------------------------------------------------------------------------------------
//- Set - Load - Store
//------------------------------------------------------------------------------------------------------------------------------------------

#define vec_elem_get_f32_16(vec, index)                    ( ((vec_alias_float32_t *) &vec)[index] )
#define vec_elem_get_f32_8(vec, index)                     ( ((vec_alias_float32_t *) &vec)[index] )
#define vec_elem_get_f32_4(vec, index)                     ( ((vec_alias_float32_t *) &vec)[index] )
#define vec_elem_get_f32_2(vec, index)                     ( ((vec_alias_float32_t *) &vec)[index] )
#define vec_elem_get_f32_1(vec, index)                     vec

#define vec_elem_set_f32_16(vec, index, expr)              do { ((vec_alias_float32_t *) &vec)[index] = (expr); } while (0)
#define vec_elem_set_f32_8(vec, index, expr)               do { ((vec_alias_float32_t *) &vec)[index] = (expr); } while (0)
#define vec_elem_set_f32_4(vec, index, expr)               do { ((vec_alias_float32_t *) &vec)[index] = (expr); } while (0)
#define vec_elem_set_f32_2(vec, index, expr)               do { ((vec_alias_float32_t *) &vec)[index] = (expr); } while (0)
#define vec_elem_set_f32_1(vec, index, expr)               do { vec = (expr); } while (0)

#define vec_array_f32_16(vec)                              ((vec_alias_float32_t *) &vec)
#define vec_array_f32_8(vec)                               ((vec_alias_float32_t *) &vec)
#define vec_array_f32_4(vec)                               ((vec_alias_float32_t *) &vec)
#define vec_array_f32_2(vec)                               ((vec_alias_float32_t *) &vec)
#define vec_array_f32_1(vec)                               ((vec_alias_float32_t *) &vec)

#define vec_set1_f32_16(val)                               svdup_n_f32(val)
#define vec_set1_f32_8(val)                                svdup_n_f32(val)
#define vec_set1_f32_4(val)                                svdup_n_f32(val)
#define vec_set1_f32_2(val)                                svdup_n_f32(val)
#define vec_set1_f32_1(val)                                val

#define vec_set_iter_f32_16(iter, expr)                    ({ vec_f32_16_t  _buf; vec_loop_stmt(  16, iter, ((vec_alias_float32_t *) &_buf)[iter] = (expr););  _buf; })
#define vec_set_iter_f32_8(iter, expr)                     ({ vec_f32_8_t   _buf; vec_loop_stmt(   8, iter, ((vec_alias_float32_t *) &_buf)[iter] = (expr););  _buf; })
#define vec_set_iter_f32_4(iter, expr)                     ({ vec_f32_4_t   _buf; vec_loop_stmt(   4, iter, ((vec_alias_float32_t *) &_buf)[iter] = (expr););  _buf; })
#define vec_set_iter_f32_2(iter, expr)                     ({ vec_f32_2_t   _buf; vec_loop_stmt(   2, iter, ((vec_alias_float32_t *) &_buf)[iter] = (expr););  _buf; })
#define vec_set_iter_f32_1(iter, expr)                     ( (vec_alias_float32_t) vec_iter_expr_1(iter, 0, (expr)) )

#define vec_loadu_f32_16(ptr)                              svld1_f32(svptrue_pat_b32(SV_VL16), ((float*) (ptr)))
#define vec_loadu_f32_8(ptr)                               svld1_f32(svptrue_pat_b32(SV_VL8), ((float*) (ptr)))
#define vec_loadu_f32_4(ptr)                               svld1_f32(svptrue_pat_b32(SV_VL4), ((float*) (ptr)))
#define vec_loadu_f32_2(ptr)                               svld1_f32(svptrue_pat_b32(SV_VL2), ((float*) (ptr)))
#define vec_loadu_f32_1(ptr)                               (*((vec_alias_float32_t*) (ptr)))

#define vec_loadu_maskedz_f32_16(ptr, mask)                svld1_f32(mask, ((float*) (ptr)))
#define vec_loadu_maskedz_f32_8(ptr, mask)                 svld1_f32(mask, ((float*) (ptr)))
#define vec_loadu_maskedz_f32_4(ptr, mask)                 svld1_f32(mask, ((float*) (ptr)))
#define vec_loadu_maskedz_f32_2(ptr, mask)                 svld1_f32(mask, ((float*) (ptr)))
#define vec_loadu_maskedz_f32_1(ptr, mask)                 ( (mask) ? (*((vec_alias_float32_t *) (ptr))) : 0 )

#define vec_storeu_f32_16(ptr, vec)                        svst1_f32(svptrue_pat_b32(SV_VL16), ((float*) (ptr)), vec)
#define vec_storeu_f32_8(ptr, vec)                         svst1_f32(svptrue_pat_b32(SV_VL8), ((float*) (ptr)), vec)
#define vec_storeu_f32_4(ptr, vec)                         svst1_f32(svptrue_pat_b32(SV_VL4), ((float*) (ptr)), vec)
#define vec_storeu_f32_2(ptr, vec)                         svst1_f32(svptrue_pat_b32(SV_VL2), ((float*) (ptr)), vec)
#define vec_storeu_f32_1(ptr, vec)                         do { (*((vec_alias_float32_t*) (ptr))) = (vec); } while (0)

#define vec_storeu_masked_f32_16(ptr, vec, mask)           svst1_f32(mask, ((float*) (ptr)), vec)
#define vec_storeu_masked_f32_8(ptr, vec, mask)            svst1_f32(mask, ((float*) (ptr)), vec)
#define vec_storeu_masked_f32_4(ptr, vec, mask)            svst1_f32(mask, ((float*) (ptr)), vec)
#define vec_storeu_masked_f32_2(ptr, vec, mask)            svst1_f32(mask, ((float*) (ptr)), vec)
#define vec_storeu_masked_f32_1(ptr, vec, mask)            do { if (mask) (*((vec_alias_float32_t *) (ptr))) = (vec); } while (0)

#define vec_gather_f32_i32_16(ptr, idx)                    svld1_gather_s32index_f32(svptrue_b32(), (vec_alias_float32_t *) (ptr), idx)
#define vec_gather_f32_i32_8(ptr, idx)                     svld1_gather_s32index_f32(svptrue_b32(), (vec_alias_float32_t *) (ptr), idx)
#define vec_gather_f32_i32_4(ptr, idx)                     svld1_gather_s32index_f32(svptrue_b32(), (vec_alias_float32_t *) (ptr), idx)
#define vec_gather_f32_i32_2(ptr, idx)                     svld1_gather_s32index_f32(svptrue_b32(), (vec_alias_float32_t *) (ptr), idx)
#define vec_gather_f32_i32_1(ptr, idx)                     ( ((vec_alias_float32_t *) (ptr))[idx] )

// #define vec_gather_f32_i64_16(ptr, idx)                    svld1_gather_s32index_f32(svptrue_b32(), (vec_alias_float32_t *) (ptr), idx)
// #define vec_gather_f32_i64_8(ptr, idx)                     svld1_gather_s32index_f32(svptrue_b32(), (vec_alias_float32_t *) (ptr), idx)
// #define vec_gather_f32_i64_4(ptr, idx)                     svld1_gather_s32index_f32(svptrue_b32(), (vec_alias_float32_t *) (ptr), idx)
// #define vec_gather_f32_i64_2(ptr, idx)                     svld1_gather_s32index_f32(svptrue_b32(), (vec_alias_float32_t *) (ptr), idx)
// #define vec_gather_f32_i64_1(ptr, idx)                     ( ((vec_alias_float32_t *) (ptr))[idx] )


//------------------------------------------------------------------------------------------------------------------------------------------
//- Operations
//------------------------------------------------------------------------------------------------------------------------------------------

#define vec_add_f32_16(a, b)                               svadd_f32_z(svptrue_b32(), a, b)
#define vec_add_f32_8(a, b)                                svadd_f32_z(svptrue_b32(), a, b)
#define vec_add_f32_4(a, b)                                svadd_f32_z(svptrue_b32(), a, b)
#define vec_add_f32_2(a, b)                                svadd_f32_z(svptrue_b32(), a, b)
#define vec_add_f32_1(a, b)                                (a + b)

#define vec_sub_f32_16(a, b)                               svsub_f32_z(svptrue_b32(), a, b)
#define vec_sub_f32_8(a, b)                                svsub_f32_z(svptrue_b32(), a, b)
#define vec_sub_f32_4(a, b)                                svsub_f32_z(svptrue_b32(), a, b)
#define vec_sub_f32_2(a, b)                                svsub_f32_z(svptrue_b32(), a, b)
#define vec_sub_f32_1(a, b)                                (a - b)

#define vec_mul_f32_16(a, b)                               svmul_f32_z(svptrue_b32(), a, b)
#define vec_mul_f32_8(a, b)                                svmul_f32_z(svptrue_b32(), a, b)
#define vec_mul_f32_4(a, b)                                svmul_f32_z(svptrue_b32(), a, b)
#define vec_mul_f32_2(a, b)                                svmul_f32_z(svptrue_b32(), a, b)
#define vec_mul_f32_1(a, b)                                (a * b)

#define vec_div_f32_16(a, b)                               svdiv_f32_z(svptrue_pat_b32(SV_VL16), a, b)
#define vec_div_f32_8(a, b)                                svdiv_f32_z(svptrue_pat_b32(SV_VL8), a, b)
#define vec_div_f32_4(a, b)                                svdiv_f32_z(svptrue_pat_b32(SV_VL4), a, b)
#define vec_div_f32_2(a, b)                                svdiv_f32_z(svptrue_pat_b32(SV_VL2), a, b)
#define vec_div_f32_1(a, b)                                (a / b)

// Returns a*b + c
#define vec_fmadd_f32_16(a, b, c)                          svmad_f32_z(svptrue_b32(), a, b, c)
#define vec_fmadd_f32_8(a, b, c)                           svmad_f32_z(svptrue_b32(), a, b, c)
#define vec_fmadd_f32_4(a, b, c)                           svmad_f32_z(svptrue_b32(), a, b, c)
#define vec_fmadd_f32_2(a, b, c)                           svmad_f32_z(svptrue_b32(), a, b, c)
#define vec_fmadd_f32_1(a, b, c)                           (a * b + c)


#define vec_reduce_add_f32_16(a)                           svaddv_f32(svptrue_pat_b32(SV_VL16), a)
#define vec_reduce_add_f32_8(a)                            svaddv_f32(svptrue_pat_b32(SV_VL8), a)
#define vec_reduce_add_f32_4(a)                            svaddv_f32(svptrue_pat_b32(SV_VL4), a)
#define vec_reduce_add_f32_2(a)                            svaddv_f32(svptrue_pat_b32(SV_VL2), a)
#define vec_reduce_add_f32_1(a)                            (a)


//------------------------------------------------------------------------------------------------------------------------------------------
//- Compare
//------------------------------------------------------------------------------------------------------------------------------------------

#define vec_cmpeq_f32_16(a, b)                             svcmpeq_f32(svptrue_b32(), a, b)
#define vec_cmpeq_f32_8(a, b)                              svcmpeq_f32(svptrue_b32(), a, b)
#define vec_cmpeq_f32_4(a, b)                              svcmpeq_f32(svptrue_b32(), a, b)
#define vec_cmpeq_f32_2(a, b)                              svcmpeq_f32(svptrue_b32(), a, b)
#define vec_cmpeq_f32_1(a, b)                              (a == b)

#define vec_cmpgt_f32_16(a, b)                             svcmpgt_f32(svptrue_b32(), a, b)
#define vec_cmpgt_f32_8(a, b)                              svcmpgt_f32(svptrue_b32(), a, b)
#define vec_cmpgt_f32_4(a, b)                              svcmpgt_f32(svptrue_b32(), a, b)
#define vec_cmpgt_f32_2(a, b)                              svcmpgt_f32(svptrue_b32(), a, b)
#define vec_cmpgt_f32_1(a, b)                              (a > b)


#endif /* VECTORIZATION_ARM_SVE_F32_H */
