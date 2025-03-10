#ifndef VECTORIZATION_ARM_SVE_F64_H
#define VECTORIZATION_ARM_SVE_F64_H

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <arm_sve.h>

#include "macros/cpp_defines.h"
#include "macros/macrolib.h"

#include "vectorization/vectorization_util.h"


/* Without the arm_sve_vector_bits attribute, the sve types have many limitations:
 *     - no sizeof().
 *     - can't include in structs or unions.
 *     - can't use pointers to address them.
 * */
#if __ARM_FEATURE_SVE_BITS==512
	#define vec_len_default_f64  8
	#define vec_pred_true_f64  svptrue_pat_b64(SV_VL8)
	typedef svfloat64_t  vec_f64_8_t  __attribute__((arm_sve_vector_bits(512)));
	typedef svfloat64_t  vec_f64_4_t  __attribute__((arm_sve_vector_bits(512)));
	typedef svfloat64_t  vec_f64_2_t  __attribute__((arm_sve_vector_bits(512)));
	#define vec_f64_1_t  double
#elif __ARM_FEATURE_SVE_BITS==256
	#define vec_len_default_f64  4
	#define vec_pred_true_f64  svptrue_pat_b64(SV_VL4)
	typedef svfloat64_t  vec_f64_8_t  __attribute__((arm_sve_vector_bits(256)));
	typedef svfloat64_t  vec_f64_4_t  __attribute__((arm_sve_vector_bits(256)));
	typedef svfloat64_t  vec_f64_2_t  __attribute__((arm_sve_vector_bits(256)));
	#define vec_f64_1_t  double
#elif __ARM_FEATURE_SVE_BITS==128
	#define vec_len_default_f64  2
	#define vec_pred_true_f64  svptrue_pat_b64(SV_VL2)
	typedef svfloat64_t  vec_f64_8_t  __attribute__((arm_sve_vector_bits(128)));
	typedef svfloat64_t  vec_f64_4_t  __attribute__((arm_sve_vector_bits(128)));
	typedef svfloat64_t  vec_f64_2_t  __attribute__((arm_sve_vector_bits(128)));
	#define vec_f64_1_t  double
#else
	#error "__ARM_FEATURE_SVE_BITS not defined"
#endif

typedef double  vec_f64_t [[gnu::may_alias]];


#define vec_array_f64_8(val)                     ({vec_f64_8_t  * _tmp = &val; (vec_f64_t *) _tmp;})
#define vec_array_f64_4(val)                     ({vec_f64_4_t  * _tmp = &val; (vec_f64_t *) _tmp;})
#define vec_array_f64_2(val)                     ({vec_f64_2_t  * _tmp = &val; (vec_f64_t *) _tmp;})
#define vec_array_f64_1(val)                     ({vec_f64_1_t  * _tmp = &val; (vec_f64_t *) _tmp;})

#define vec_set1_f64_8(val)                      svdup_n_f64(val)
#define vec_set1_f64_4(val)                      svdup_n_f64(val)
#define vec_set1_f64_2(val)                      svdup_n_f64(val)
#define vec_set1_f64_1(val)                      val

#define vec_set_iter_f64_8(iter, expr)           ({double buf_a[8]; for (long iter=0;iter<8;iter++) buf_a[iter] = expr; svld1_f64(svptrue_pat_b64(SV_VL8), ((double*)(&buf_a)));})
#define vec_set_iter_f64_4(iter, expr)           ({double buf_a[4]; for (long iter=0;iter<4;iter++) buf_a[iter] = expr; svld1_f64(svptrue_pat_b64(SV_VL4), ((double*)(&buf_a)));})
#define vec_set_iter_f64_2(iter, expr)           ({double buf_a[2]; for (long iter=0;iter<2;iter++) buf_a[iter] = expr; svld1_f64(svptrue_pat_b64(SV_VL2), ((double*)(&buf_a)));})
#define vec_set_iter_f64_1(iter, expr)           vec_iter_expr_1(iter, 0, expr)


#define vec_loadu_f64_8(ptr)                     svld1_f64(svptrue_pat_b64(SV_VL8), ((double*)(ptr)))
#define vec_loadu_f64_4(ptr)                     svld1_f64(svptrue_pat_b64(SV_VL4), ((double*)(ptr)))
#define vec_loadu_f64_2(ptr)                     svld1_f64(svptrue_pat_b64(SV_VL2), ((double*)(ptr)))
#define vec_loadu_f64_1(ptr)                     (*((vec_f64_1_t*)(ptr)))

#define vec_storeu_f64_8(ptr, vec)               svst1_f64(svptrue_pat_b64(SV_VL8), ((double*)(ptr)), vec)
#define vec_storeu_f64_4(ptr, vec)               svst1_f64(svptrue_pat_b64(SV_VL4), ((double*)(ptr)), vec)
#define vec_storeu_f64_2(ptr, vec)               svst1_f64(svptrue_pat_b64(SV_VL2), ((double*)(ptr)), vec)
#define vec_storeu_f64_1(ptr, vec)               do { (*((vec_f64_1_t*)(ptr))) = (vec); } while (0)


#define vec_add_f64_8(a, b)                      svadd_f64_z(svptrue_b64(), a, b)
#define vec_add_f64_4(a, b)                      svadd_f64_z(svptrue_b64(), a, b)
#define vec_add_f64_2(a, b)                      svadd_f64_z(svptrue_b64(), a, b)
#define vec_add_f64_1(a, b)                      (a + b)

#define vec_sub_f64_8(a, b)                      svsub_f64_z(svptrue_b64(), a, b)
#define vec_sub_f64_4(a, b)                      svsub_f64_z(svptrue_b64(), a, b)
#define vec_sub_f64_2(a, b)                      svsub_f64_z(svptrue_b64(), a, b)
#define vec_sub_f64_1(a, b)                      (a - b)

#define vec_mul_f64_8(a, b)                      svmul_f64_z(svptrue_b64(), a, b)
#define vec_mul_f64_4(a, b)                      svmul_f64_z(svptrue_b64(), a, b)
#define vec_mul_f64_2(a, b)                      svmul_f64_z(svptrue_b64(), a, b)
#define vec_mul_f64_1(a, b)                      (a * b)

#define vec_div_f64_8(a, b)                      svdiv_f64_z(svptrue_pat_b64(SV_VL8), a, b)
#define vec_div_f64_4(a, b)                      svdiv_f64_z(svptrue_pat_b64(SV_VL4), a, b)
#define vec_div_f64_2(a, b)                      svdiv_f64_z(svptrue_pat_b64(SV_VL2), a, b)
#define vec_div_f64_1(a, b)                      (a / b)

// Returns a*b + c
#define vec_fmadd_f64_8(a, b, c)                 svmad_f64_z(svptrue_b64(), a, b, c)
#define vec_fmadd_f64_4(a, b, c)                 svmad_f64_z(svptrue_b64(), a, b, c)
#define vec_fmadd_f64_2(a, b, c)                 svmad_f64_z(svptrue_b64(), a, b, c)
#define vec_fmadd_f64_1(a, b, c)                 (a * b + c)


#define vec_reduce_add_f64_8(a)                  svaddv_f64(svptrue_pat_b64(SV_VL8), a)
#define vec_reduce_add_f64_4(a)                  svaddv_f64(svptrue_pat_b64(SV_VL4), a)
#define vec_reduce_add_f64_2(a)                  svaddv_f64(svptrue_pat_b64(SV_VL2), a)
#define vec_reduce_add_f64_1(a)                  (a)


#endif /* VECTORIZATION_ARM_SVE_F64_H */
