#ifndef VECTORIZATION_ARM_SVE_I64_H
#define VECTORIZATION_ARM_SVE_I64_H

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
	#define vec_len_default_i64  8
	#define vec_pred_true_i64  svptrue_pat_b64(SV_VL8)
	typedef svint64_t  vec_i64_8_t  __attribute__((arm_sve_vector_bits(512)));
	typedef svint64_t  vec_i64_4_t  __attribute__((arm_sve_vector_bits(512)));
	typedef svint64_t  vec_i64_2_t  __attribute__((arm_sve_vector_bits(512)));
	#define vec_i64_1_t   int64_t
#elif __ARM_FEATURE_SVE_BITS==256
	#define vec_len_default_i64  4
	#define vec_pred_true_i64  svptrue_pat_b64(SV_VL4)
	typedef svint64_t  vec_i64_8_t  __attribute__((arm_sve_vector_bits(256)));
	typedef svint64_t  vec_i64_4_t  __attribute__((arm_sve_vector_bits(256)));
	typedef svint64_t  vec_i64_2_t  __attribute__((arm_sve_vector_bits(256)));
	#define vec_i64_1_t   int64_t
#elif __ARM_FEATURE_SVE_BITS==128
	#define vec_len_default_i64  2
	#define vec_pred_true_i64  svptrue_pat_b64(SV_VL2)
	typedef svint64_t  vec_i64_8_t  __attribute__((arm_sve_vector_bits(128)));
	typedef svint64_t  vec_i64_4_t  __attribute__((arm_sve_vector_bits(128)));
	typedef svint64_t  vec_i64_2_t  __attribute__((arm_sve_vector_bits(128)));
	#define vec_i64_1_t   int64_t
#else
	#error "__ARM_FEATURE_SVE_BITS not defined"
#endif

typedef uint64_t  vec_i64_t [[gnu::may_alias]];


#define vec_array_i64_8(val)                     ({vec_i64_8_t  * _tmp = &val; (vec_i64_t *) _tmp;})
#define vec_array_i64_4(val)                     ({vec_i64_4_t  * _tmp = &val; (vec_i64_t *) _tmp;})
#define vec_array_i64_2(val)                     ({vec_i64_2_t  * _tmp = &val; (vec_i64_t *) _tmp;})
#define vec_array_i64_1(val)                     ({vec_i64_1_t  * _tmp = &val; (vec_i64_t *) _tmp;})

#define vec_set1_i64_8(val)                      svdup_n_s64(val)
#define vec_set1_i64_4(val)                      svdup_n_s64(val)
#define vec_set1_i64_2(val)                      svdup_n_s64(val)
#define vec_set1_i64_1(val)                      val

#define vec_set_iter_i64_8(iter, expr)           ({int64_t buf_a[8]; for (long iter=0;iter<8;iter++) buf_a[iter] = expr; svld1_s64(svptrue_pat_b64(SV_VL8), ((int64_t*)(&buf_a)));})
#define vec_set_iter_i64_4(iter, expr)           ({int64_t buf_a[4]; for (long iter=0;iter<4;iter++) buf_a[iter] = expr; svld1_s64(svptrue_pat_b64(SV_VL4), ((int64_t*)(&buf_a)));})
#define vec_set_iter_i64_2(iter, expr)           ({int64_t buf_a[2]; for (long iter=0;iter<2;iter++) buf_a[iter] = expr; svld1_s64(svptrue_pat_b64(SV_VL2), ((int64_t*)(&buf_a)));})
#define vec_set_iter_i64_1(iter, expr)           vec_iter_expr_1(iter, 0, expr)


#define vec_loadu_i64_8(ptr)                     svld1_s64(svptrue_pat_b64(SV_VL8), ((int64_t*)(ptr)))
#define vec_loadu_i64_4(ptr)                     svld1_s64(svptrue_pat_b64(SV_VL4), ((int64_t*)(ptr)))
#define vec_loadu_i64_2(ptr)                     svld1_s64(svptrue_pat_b64(SV_VL2), ((int64_t*)(ptr)))
#define vec_loadu_i64_1(ptr)                     (*((vec_i64_1_t*)(ptr)))

#define vec_storeu_i64_8(ptr, vec)               svst1_s64(svptrue_pat_b64(SV_VL8), ((int64_t*)(ptr)), vec)
#define vec_storeu_i64_4(ptr, vec)               svst1_s64(svptrue_pat_b64(SV_VL4), ((int64_t*)(ptr)), vec)
#define vec_storeu_i64_2(ptr, vec)               svst1_s64(svptrue_pat_b64(SV_VL2), ((int64_t*)(ptr)), vec)
#define vec_storeu_i64_1(ptr, vec)               do { (*((vec_i64_1_t*)(ptr))) = (vec); } while (0)


#define vec_add_i64_8(a, b)                      svadd_s64_z(svptrue_b64(), a, b)
#define vec_add_i64_4(a, b)                      svadd_s64_z(svptrue_b64(), a, b)
#define vec_add_i64_2(a, b)                      svadd_s64_z(svptrue_b64(), a, b)
#define vec_add_i64_1(a, b)                      (a + b)

#define vec_sub_i64_8(a, b)                      svsub_s64_z(svptrue_b64(), a, b)
#define vec_sub_i64_4(a, b)                      svsub_s64_z(svptrue_b64(), a, b)
#define vec_sub_i64_2(a, b)                      svsub_s64_z(svptrue_b64(), a, b)
#define vec_sub_i64_1(a, b)                      (a - b)

#define vec_mul_i64_8(a, b)                      svmul_s64_z(svptrue_b64(), a, b)
#define vec_mul_i64_4(a, b)                      svmul_s64_z(svptrue_b64(), a, b)
#define vec_mul_i64_2(a, b)                      svmul_s64_z(svptrue_b64(), a, b)
#define vec_mul_i64_1(a, b)                      (a * b)

#define vec_div_i64_8(a, b)                      svdiv_s64_z(svptrue_pat_b64(SV_VL8), a, b)
#define vec_div_i64_4(a, b)                      svdiv_s64_z(svptrue_pat_b64(SV_VL4), a, b)
#define vec_div_i64_2(a, b)                      svdiv_s64_z(svptrue_pat_b64(SV_VL2), a, b)
#define vec_div_i64_1(a, b)                      (a / b)


#define vec_and_i64_8(a, b)                      svand_s64_z(svptrue_b64(), a, b)
#define vec_and_i64_4(a, b)                      svand_s64_z(svptrue_b64(), a, b)
#define vec_and_i64_2(a, b)                      svand_s64_z(svptrue_b64(), a, b)
#define vec_and_i64_1(a, b)                      (a & b)

#define vec_or_i64_8(a, b)                       svorr_s64_z(svptrue_b64(), a, b)
#define vec_or_i64_4(a, b)                       svorr_s64_z(svptrue_b64(), a, b)
#define vec_or_i64_2(a, b)                       svorr_s64_z(svptrue_b64(), a, b)
#define vec_or_i64_1(a, b)                       (a | b)


#define vec_slli_i64_8(a, imm8)                                        svlsl_n_s64_z(svptrue_b64(),                        a, imm8)
#define vec_slli_i64_4(a, imm8)                                        svlsl_n_s64_z(svptrue_b64(),                        a, imm8)
#define vec_slli_i64_2(a, imm8)                                        svlsl_n_s64_z(svptrue_b64(),                        a, imm8)
#define vec_slli_i64_1(a, imm8)                  ((imm8 < 64) ? ((uint64_t) a)<<imm8 : 0)

#define vec_srli_i64_8(a, imm8)                  svreinterpret_s64_u64(svlsr_n_u64_z(svptrue_b64(), svreinterpret_u64_s64(a), imm8))
#define vec_srli_i64_4(a, imm8)                  svreinterpret_s64_u64(svlsr_n_u64_z(svptrue_b64(), svreinterpret_u64_s64(a), imm8))
#define vec_srli_i64_2(a, imm8)                  svreinterpret_s64_u64(svlsr_n_u64_z(svptrue_b64(), svreinterpret_u64_s64(a), imm8))
#define vec_srli_i64_1(a, imm8)                  ((imm8 < 64) ? ((uint64_t) a)>>imm8 : 0)

#define vec_srai_i64_8(a, imm8)                                        svasr_n_s64_z(svptrue_b64(),                        a, imm8)
#define vec_srai_i64_4(a, imm8)                                        svasr_n_s64_z(svptrue_b64(),                        a, imm8)
#define vec_srai_i64_2(a, imm8)                                        svasr_n_s64_z(svptrue_b64(),                        a, imm8)
#define vec_srai_i64_1(a, imm8)                  ((imm8 < 64) ? ((int64_t) a)>>imm8 : (((int64_t) a) < 0) ? -1 : 0)


#define vec_sllv_i64_8(a, count)                                       svlsl_s64_z(svptrue_b64(),                        a, svreinterpret_u64_s64(count))
#define vec_sllv_i64_4(a, count)                                       svlsl_s64_z(svptrue_b64(),                        a, svreinterpret_u64_s64(count))
#define vec_sllv_i64_2(a, count)                                       svlsl_s64_z(svptrue_b64(),                        a, svreinterpret_u64_s64(count))
#define vec_sllv_i64_1(a, count)                 ((count < 64) ? ((uint64_t) a)<<count : 0)

#define vec_srlv_i64_8(a, count)                 svreinterpret_s64_u64(svlsr_u64_z(svptrue_b64(), svreinterpret_u64_s64(a), svreinterpret_u64_s64(count)))
#define vec_srlv_i64_4(a, count)                 svreinterpret_s64_u64(svlsr_u64_z(svptrue_b64(), svreinterpret_u64_s64(a), svreinterpret_u64_s64(count)))
#define vec_srlv_i64_2(a, count)                 svreinterpret_s64_u64(svlsr_u64_z(svptrue_b64(), svreinterpret_u64_s64(a), svreinterpret_u64_s64(count)))
#define vec_srlv_i64_1(a, count)                 ((count < 64) ? ((uint64_t) a)>>count : 0)

#define vec_srav_i64_8(a, count)                                       svasr_s64_z(svptrue_b64(),                        a, svreinterpret_u64_s64(count))
#define vec_srav_i64_4(a, count)                                       svasr_s64_z(svptrue_b64(),                        a, svreinterpret_u64_s64(count))
#define vec_srav_i64_2(a, count)                                       svasr_s64_z(svptrue_b64(),                        a, svreinterpret_u64_s64(count))
#define vec_srav_i64_1(a, count)                 ((count < 64) ? ((int64_t) a)>>count : (((int64_t) a) < 0) ? -1 : 0)


#endif /* VECTORIZATION_ARM_SVE_I64_H */
