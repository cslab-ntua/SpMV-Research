#ifndef VECTORIZATION_ARM_SVE_F64_H
#define VECTORIZATION_ARM_SVE_F64_H

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <arm_sve.h>

#include "macros/cpp_defines.h"
#include "macros/macrolib.h"

#include "vectorization/vectorization_util.h"
#include "vectorization_arm_sve_m64.h"


typedef double       __attribute__((may_alias))  vec_alias_float64_t;

/* Without the arm_sve_vector_bits attribute, the sve types have many limitations:
 *     - no sizeof().
 *     - can't include in structs or unions.
 *     - can't use pointers to address them.
 * */
#if __ARM_FEATURE_SVE_BITS==512
	#define vec_len_default_f64  8
	#define vec_len_default_f64_f2  4
	#define vec_pred_true_f64  svptrue_pat_b64(SV_VL8)
	typedef double       __attribute__((may_alias))  vec_f64_1_t;
	typedef svfloat64_t  __attribute__((arm_sve_vector_bits(512)))  vec_f64_2_t;
	typedef svfloat64_t  __attribute__((arm_sve_vector_bits(512)))  vec_f64_4_t;
	typedef svfloat64_t  __attribute__((arm_sve_vector_bits(512)))  vec_f64_8_t;
#elif __ARM_FEATURE_SVE_BITS==256
	#define vec_len_default_f64  4
	#define vec_len_default_f64_f2  2
	#define vec_pred_true_f64  svptrue_pat_b64(SV_VL4)
	typedef double       __attribute__((may_alias))  vec_f64_1_t;
	typedef svfloat64_t  __attribute__((arm_sve_vector_bits(256)))  vec_f64_2_t;
	typedef svfloat64_t  __attribute__((arm_sve_vector_bits(256)))  vec_f64_4_t;
	typedef svfloat64_t  __attribute__((arm_sve_vector_bits(256)))  vec_f64_8_t;
#elif __ARM_FEATURE_SVE_BITS==128
	#define vec_len_default_f64  2
	#define vec_len_default_f64_f2  1
	#define vec_pred_true_f64  svptrue_pat_b64(SV_VL2)
	typedef double       __attribute__((may_alias))  vec_f64_1_t;
	typedef svfloat64_t  __attribute__((arm_sve_vector_bits(128)))  vec_f64_2_t;
	typedef svfloat64_t  __attribute__((arm_sve_vector_bits(128)))  vec_f64_4_t;
	typedef svfloat64_t  __attribute__((arm_sve_vector_bits(128)))  vec_f64_8_t;
#else
	#error "__ARM_FEATURE_SVE_BITS not defined"
#endif

typedef double  vec_f64_t __attribute__((may_alias));


//------------------------------------------------------------------------------------------------------------------------------------------
//- Set - Load - Store
//------------------------------------------------------------------------------------------------------------------------------------------

#define vec_elem_get_f64_8(vec, index)                     ( ((vec_alias_float64_t *) &vec)[index] )
#define vec_elem_get_f64_4(vec, index)                     ( ((vec_alias_float64_t *) &vec)[index] )
#define vec_elem_get_f64_2(vec, index)                     ( ((vec_alias_float64_t *) &vec)[index] )
#define vec_elem_get_f64_1(vec, index)                     vec

#define vec_elem_set_f64_8(vec, index, expr)               do { ((vec_alias_float64_t *) &vec)[index] = (expr); } while (0)
#define vec_elem_set_f64_4(vec, index, expr)               do { ((vec_alias_float64_t *) &vec)[index] = (expr); } while (0)
#define vec_elem_set_f64_2(vec, index, expr)               do { ((vec_alias_float64_t *) &vec)[index] = (expr); } while (0)
#define vec_elem_set_f64_1(vec, index, expr)               do { vec = (expr); } while (0)

#define vec_array_f64_8(vec)                               ((vec_alias_float64_t *) &vec)
#define vec_array_f64_4(vec)                               ((vec_alias_float64_t *) &vec)
#define vec_array_f64_2(vec)                               ((vec_alias_float64_t *) &vec)
#define vec_array_f64_1(vec)                               ((vec_alias_float64_t *) &vec)

#define vec_set1_f64_8(val)                                svdup_n_f64(val)
#define vec_set1_f64_4(val)                                svdup_n_f64(val)
#define vec_set1_f64_2(val)                                svdup_n_f64(val)
#define vec_set1_f64_1(val)                                val

#define vec_set_iter_f64_8(iter, expr)                     ({ vec_f64_8_t   _buf; vec_loop_stmt(  8, iter, ((vec_alias_float64_t *) &_buf)[iter] = (expr););  _buf; })
#define vec_set_iter_f64_4(iter, expr)                     ({ vec_f64_4_t   _buf; vec_loop_stmt(  4, iter, ((vec_alias_float64_t *) &_buf)[iter] = (expr););  _buf; })
#define vec_set_iter_f64_2(iter, expr)                     ({ vec_f64_2_t   _buf; vec_loop_stmt(  2, iter, ((vec_alias_float64_t *) &_buf)[iter] = (expr););  _buf; })
#define vec_set_iter_f64_1(iter, expr)                     ( (vec_alias_float64_t) vec_iter_expr_1(iter, 0, (expr)) )

#define vec_loadu_f64_8(ptr)                               svld1_f64(svptrue_pat_b64(SV_VL8), ((double*) (ptr)))
#define vec_loadu_f64_4(ptr)                               svld1_f64(svptrue_pat_b64(SV_VL4), ((double*) (ptr)))
#define vec_loadu_f64_2(ptr)                               svld1_f64(svptrue_pat_b64(SV_VL2), ((double*) (ptr)))
#define vec_loadu_f64_1(ptr)                               (*((vec_alias_float64_t*) (ptr)))

#define vec_loadu_maskedz_f64_8(ptr, mask)                 svld1_f64(mask, ((double*) (ptr)))
#define vec_loadu_maskedz_f64_4(ptr, mask)                 svld1_f64(mask, ((double*) (ptr)))
#define vec_loadu_maskedz_f64_2(ptr, mask)                 svld1_f64(mask, ((double*) (ptr)))
#define vec_loadu_maskedz_f64_1(ptr, mask)                 ( (mask) ? (*((vec_alias_float64_t *) (ptr))) : 0 )

#define vec_storeu_f64_8(ptr, vec)                         svst1_f64(svptrue_pat_b64(SV_VL8), ((double*) (ptr)), vec)
#define vec_storeu_f64_4(ptr, vec)                         svst1_f64(svptrue_pat_b64(SV_VL4), ((double*) (ptr)), vec)
#define vec_storeu_f64_2(ptr, vec)                         svst1_f64(svptrue_pat_b64(SV_VL2), ((double*) (ptr)), vec)
#define vec_storeu_f64_1(ptr, vec)                         do { (*((vec_alias_float64_t*) (ptr))) = (vec); } while (0)

#define vec_storeu_masked_f64_8(ptr, vec, mask)            svst1_f64(mask, ((double*) (ptr)), vec)
#define vec_storeu_masked_f64_4(ptr, vec, mask)            svst1_f64(mask, ((double*) (ptr)), vec)
#define vec_storeu_masked_f64_2(ptr, vec, mask)            svst1_f64(mask, ((double*) (ptr)), vec)
#define vec_storeu_masked_f64_1(ptr, vec, mask)            do { if (mask) (*((vec_alias_float64_t *) (ptr))) = (vec); } while (0)

#define vec_gather_f64_i32_8(ptr, idx)                     svld1_gather_s64index_f64(svptrue_b64(), (vec_alias_float64_t *) (ptr), svunpklo_u64(idx))
#define vec_gather_f64_i32_4(ptr, idx)                     svld1_gather_s64index_f64(svptrue_b64(), (vec_alias_float64_t *) (ptr), svunpklo_u64(idx))
#define vec_gather_f64_i32_2(ptr, idx)                     svld1_gather_s64index_f64(svptrue_b64(), (vec_alias_float64_t *) (ptr), svunpklo_u64(idx))
#define vec_gather_f64_i32_1(ptr, idx)                     ( ((vec_alias_float64_t *) (ptr))[idx] )

#define vec_gather_f64_i64_8(ptr, idx)                     svld1_gather_s64index_f64(svptrue_b64(), (vec_alias_float64_t *) (ptr), idx)
#define vec_gather_f64_i64_4(ptr, idx)                     svld1_gather_s64index_f64(svptrue_b64(), (vec_alias_float64_t *) (ptr), idx)
#define vec_gather_f64_i64_2(ptr, idx)                     svld1_gather_s64index_f64(svptrue_b64(), (vec_alias_float64_t *) (ptr), idx)
#define vec_gather_f64_i64_1(ptr, idx)                     ( ((vec_alias_float64_t *) (ptr))[idx] )


//------------------------------------------------------------------------------------------------------------------------------------------
//- Operations
//------------------------------------------------------------------------------------------------------------------------------------------

#define vec_add_f64_8(a, b)                                svadd_f64_z(svptrue_b64(), a, b)
#define vec_add_f64_4(a, b)                                svadd_f64_z(svptrue_b64(), a, b)
#define vec_add_f64_2(a, b)                                svadd_f64_z(svptrue_b64(), a, b)
#define vec_add_f64_1(a, b)                                (a + b)

#define vec_sub_f64_8(a, b)                                svsub_f64_z(svptrue_b64(), a, b)
#define vec_sub_f64_4(a, b)                                svsub_f64_z(svptrue_b64(), a, b)
#define vec_sub_f64_2(a, b)                                svsub_f64_z(svptrue_b64(), a, b)
#define vec_sub_f64_1(a, b)                                (a - b)

#define vec_mul_f64_8(a, b)                                svmul_f64_z(svptrue_b64(), a, b)
#define vec_mul_f64_4(a, b)                                svmul_f64_z(svptrue_b64(), a, b)
#define vec_mul_f64_2(a, b)                                svmul_f64_z(svptrue_b64(), a, b)
#define vec_mul_f64_1(a, b)                                (a * b)

#define vec_div_f64_8(a, b)                                svdiv_f64_z(svptrue_pat_b64(SV_VL8), a, b)
#define vec_div_f64_4(a, b)                                svdiv_f64_z(svptrue_pat_b64(SV_VL4), a, b)
#define vec_div_f64_2(a, b)                                svdiv_f64_z(svptrue_pat_b64(SV_VL2), a, b)
#define vec_div_f64_1(a, b)                                (a / b)

// Returns a*b + c
#define vec_fmadd_f64_8(a, b, c)                           svmad_f64_z(svptrue_b64(), a, b, c)
#define vec_fmadd_f64_4(a, b, c)                           svmad_f64_z(svptrue_b64(), a, b, c)
#define vec_fmadd_f64_2(a, b, c)                           svmad_f64_z(svptrue_b64(), a, b, c)
#define vec_fmadd_f64_1(a, b, c)                           (a * b + c)


#define vec_reduce_add_f64_8(a)                            svaddv_f64(svptrue_pat_b64(SV_VL8), a)
#define vec_reduce_add_f64_4(a)                            svaddv_f64(svptrue_pat_b64(SV_VL4), a)
#define vec_reduce_add_f64_2(a)                            svaddv_f64(svptrue_pat_b64(SV_VL2), a)
#define vec_reduce_add_f64_1(a)                            (a)


//------------------------------------------------------------------------------------------------------------------------------------------
//- Compare
//------------------------------------------------------------------------------------------------------------------------------------------

#define vec_cmpeq_f64_8(a, b)                              svcmpeq_f64(svptrue_b64(), a, b)
#define vec_cmpeq_f64_4(a, b)                              svcmpeq_f64(svptrue_b64(), a, b)
#define vec_cmpeq_f64_2(a, b)                              svcmpeq_f64(svptrue_b64(), a, b)
#define vec_cmpeq_f64_1(a, b)                              (a == b)

#define vec_cmpgt_f64_8(a, b)                              svcmpgt_f64(svptrue_b64(), a, b)
#define vec_cmpgt_f64_4(a, b)                              svcmpgt_f64(svptrue_b64(), a, b)
#define vec_cmpgt_f64_2(a, b)                              svcmpgt_f64(svptrue_b64(), a, b)
#define vec_cmpgt_f64_1(a, b)                              (a > b)


#endif /* VECTORIZATION_ARM_SVE_F64_H */
