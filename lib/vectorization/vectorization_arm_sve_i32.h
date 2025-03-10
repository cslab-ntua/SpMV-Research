#ifndef VECTORIZATION_ARM_SVE_I32_H
#define VECTORIZATION_ARM_SVE_I32_H

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
	#define vec_len_default_i32  16
	#define vec_pred_true_i32  svptrue_pat_b32(SV_VL16)
	typedef svint32_t  vec_i32_16_t  __attribute__((arm_sve_vector_bits(512)));
	typedef svint32_t  vec_i32_8_t  __attribute__((arm_sve_vector_bits(512)));
	typedef svint32_t  vec_i32_4_t  __attribute__((arm_sve_vector_bits(512)));
	typedef svint32_t  vec_i32_2_t  __attribute__((arm_sve_vector_bits(512)));
	#define vec_i32_1_t   int32_t
#elif __ARM_FEATURE_SVE_BITS==256
	#define vec_len_default_i32  8
	#define vec_pred_true_i32  svptrue_pat_b32(SV_VL8)
	typedef svint32_t  vec_i32_16_t  __attribute__((arm_sve_vector_bits(256)));
	typedef svint32_t  vec_i32_8_t  __attribute__((arm_sve_vector_bits(256)));
	typedef svint32_t  vec_i32_4_t  __attribute__((arm_sve_vector_bits(256)));
	typedef svint32_t  vec_i32_2_t  __attribute__((arm_sve_vector_bits(256)));
	#define vec_i32_1_t   int32_t
#elif __ARM_FEATURE_SVE_BITS==128
	#define vec_len_default_i32  4
	#define vec_pred_true_i32  svptrue_pat_b32(SV_VL4)
	typedef svint32_t  vec_i32_16_t  __attribute__((arm_sve_vector_bits(128)));
	typedef svint32_t  vec_i32_8_t  __attribute__((arm_sve_vector_bits(128)));
	typedef svint32_t  vec_i32_4_t  __attribute__((arm_sve_vector_bits(128)));
	typedef svint32_t  vec_i32_2_t  __attribute__((arm_sve_vector_bits(128)));
	#define vec_i32_1_t   int32_t
#else
	#error "__ARM_FEATURE_SVE_BITS not defined"
#endif

typedef uint32_t  vec_i32_t [[gnu::may_alias]];


#define vec_array_i32_16(val)                    ({vec_i32_16_t  * _tmp = &val; (vec_i32_t *) _tmp;})
#define vec_array_i32_8(val)                     ({vec_i32_8_t  * _tmp = &val; (vec_i32_t *) _tmp;})
#define vec_array_i32_4(val)                     ({vec_i32_4_t  * _tmp = &val; (vec_i32_t *) _tmp;})
#define vec_array_i32_2(val)                     ({vec_i32_2_t  * _tmp = &val; (vec_i32_t *) _tmp;})
#define vec_array_i32_1(val)                     ({vec_i32_1_t  * _tmp = &val; (vec_i32_t *) _tmp;})

#define vec_set1_i32_16(val)                     svdup_n_s32(val)
#define vec_set1_i32_8(val)                      svdup_n_s32(val)
#define vec_set1_i32_4(val)                      svdup_n_s32(val)
#define vec_set1_i32_2(val)                      svdup_n_s32(val)
#define vec_set1_i32_1(val)                      val

#define vec_set_iter_i32_16(iter, expr)         ({int32_t buf_a[16]; for (long iter=0;iter<16;iter++) buf_a[iter] = expr; svld1_s32(svptrue_pat_b32(SV_VL16), ((int32_t*)(&buf_a)));})
#define vec_set_iter_i32_8(iter, expr)          ({int32_t buf_a[8]; for (long iter=0;iter<8;iter++) buf_a[iter] = expr; svld1_s32(svptrue_pat_b32(SV_VL8), ((int32_t*)(&buf_a)));})
#define vec_set_iter_i32_4(iter, expr)          ({int32_t buf_a[4]; for (long iter=0;iter<4;iter++) buf_a[iter] = expr; svld1_s32(svptrue_pat_b32(SV_VL4), ((int32_t*)(&buf_a)));})
#define vec_set_iter_i32_2(iter, expr)          ({int32_t buf_a[2]; for (long iter=0;iter<2;iter++) buf_a[iter] = expr; svld1_s32(svptrue_pat_b32(SV_VL2), ((int32_t*)(&buf_a)));})
#define vec_set_iter_i32_1(iter, expr)           vec_iter_expr_1(iter, 0, expr)


#define vec_loadu_i32_16(ptr)                    svld1_s32(svptrue_pat_b32(SV_VL16), ((int32_t*)(ptr)))
#define vec_loadu_i32_8(ptr)                     svld1_s32(svptrue_pat_b32(SV_VL8), ((int32_t*)(ptr)))
#define vec_loadu_i32_4(ptr)                     svld1_s32(svptrue_pat_b32(SV_VL4), ((int32_t*)(ptr)))
#define vec_loadu_i32_2(ptr)                     svld1_s32(svptrue_pat_b32(SV_VL2), ((int32_t*)(ptr)))
#define vec_loadu_i32_1(ptr)                     (*((vec_i32_1_t*)(ptr)))

#define vec_storeu_i32_16(ptr, vec)              svst1_s32(svptrue_pat_b32(SV_VL16), ((int32_t*)(ptr)), vec)
#define vec_storeu_i32_8(ptr, vec)               svst1_s32(svptrue_pat_b32(SV_VL8), ((int32_t*)(ptr)), vec)
#define vec_storeu_i32_4(ptr, vec)               svst1_s32(svptrue_pat_b32(SV_VL4), ((int32_t*)(ptr)), vec)
#define vec_storeu_i32_2(ptr, vec)               svst1_s32(svptrue_pat_b32(SV_VL2), ((int32_t*)(ptr)), vec)
#define vec_storeu_i32_1(ptr, vec)               do { (*((vec_i32_1_t*)(ptr))) = (vec); } while (0)


#define vec_add_i32_16(a, b)                     svadd_s32_z(svptrue_b32(), a, b)
#define vec_add_i32_8(a, b)                      svadd_s32_z(svptrue_b32(), a, b)
#define vec_add_i32_4(a, b)                      svadd_s32_z(svptrue_b32(), a, b)
#define vec_add_i32_2(a, b)                      svadd_s32_z(svptrue_b32(), a, b)
#define vec_add_i32_1(a, b)                      (a + b)

#define vec_sub_i32_16(a, b)                      svsub_s32_z(svptrue_b32(), a, b)
#define vec_sub_i32_8(a, b)                      svsub_s32_z(svptrue_b32(), a, b)
#define vec_sub_i32_4(a, b)                      svsub_s32_z(svptrue_b32(), a, b)
#define vec_sub_i32_2(a, b)                      svsub_s32_z(svptrue_b32(), a, b)
#define vec_sub_i32_1(a, b)                      (a - b)

#define vec_mul_i32_16(a, b)                     svmul_s32_z(svptrue_b32(), a, b)
#define vec_mul_i32_8(a, b)                      svmul_s32_z(svptrue_b32(), a, b)
#define vec_mul_i32_4(a, b)                      svmul_s32_z(svptrue_b32(), a, b)
#define vec_mul_i32_2(a, b)                      svmul_s32_z(svptrue_b32(), a, b)
#define vec_mul_i32_1(a, b)                      (a * b)

#define vec_div_i32_16(a, b)                     svdiv_s32_z(svptrue_pat_b32(SV_VL16), a, b)
#define vec_div_i32_8(a, b)                      svdiv_s32_z(svptrue_pat_b32(SV_VL8), a, b)
#define vec_div_i32_4(a, b)                      svdiv_s32_z(svptrue_pat_b32(SV_VL4), a, b)
#define vec_div_i32_2(a, b)                      svdiv_s32_z(svptrue_pat_b32(SV_VL2), a, b)
#define vec_div_i32_1(a, b)                      (a / b)


#define vec_and_i32_16(a, b)                     svand_s32_z(svptrue_b32(), a, b)
#define vec_and_i32_8(a, b)                      svand_s32_z(svptrue_b32(), a, b)
#define vec_and_i32_4(a, b)                      svand_s32_z(svptrue_b32(), a, b)
#define vec_and_i32_2(a, b)                      svand_s32_z(svptrue_b32(), a, b)
#define vec_and_i32_1(a, b)                      (a & b)

#define vec_or_i32_16(a, b)                      svorr_s32_z(svptrue_b32(), a, b)
#define vec_or_i32_8(a, b)                       svorr_s32_z(svptrue_b32(), a, b)
#define vec_or_i32_4(a, b)                       svorr_s32_z(svptrue_b32(), a, b)
#define vec_or_i32_2(a, b)                       svorr_s32_z(svptrue_b32(), a, b)
#define vec_or_i32_1(a, b)                       (a | b)


#define vec_slli_i32_16(a, imm8)                                       svlsl_n_s32_z(svptrue_b32(),                        a, imm8)
#define vec_slli_i32_8(a, imm8)                                        svlsl_n_s32_z(svptrue_b32(),                         a, imm8)
#define vec_slli_i32_4(a, imm8)                                        svlsl_n_s32_z(svptrue_b32(),                         a, imm8)
#define vec_slli_i32_2(a, imm8)                                        svlsl_n_s32_z(svptrue_b32(),                         a, imm8)
#define vec_slli_i32_1(a, imm8)                  ((imm8 < 32) ? ((uint32_t) a)<<imm8 : 0)

#define vec_srli_i32_16(a, imm8)                 svreinterpret_s32_u32(svlsr_n_u32_z(svptrue_b32(), svreinterpret_u32_s32(a), imm8))
#define vec_srli_i32_8(a, imm8)                  svreinterpret_s32_u32(svlsr_n_u32_z(svptrue_b32(),  svreinterpret_u32_s32(a), imm8))
#define vec_srli_i32_4(a, imm8)                  svreinterpret_s32_u32(svlsr_n_u32_z(svptrue_b32(),  svreinterpret_u32_s32(a), imm8))
#define vec_srli_i32_2(a, imm8)                  svreinterpret_s32_u32(svlsr_n_u32_z(svptrue_b32(),  svreinterpret_u32_s32(a), imm8))
#define vec_srli_i32_1(a, imm8)                  ((imm8 < 32) ? ((uint32_t) a)>>imm8 : 0)

#define vec_srai_i32_16(a, imm8)                                       svasr_n_s32_z(svptrue_b32(),                        a, imm8)
#define vec_srai_i32_8(a, imm8)                                        svasr_n_s32_z(svptrue_b32(),                         a, imm8)
#define vec_srai_i32_4(a, imm8)                                        svasr_n_s32_z(svptrue_b32(),                         a, imm8)
#define vec_srai_i32_2(a, imm8)                                        svasr_n_s32_z(svptrue_b32(),                         a, imm8)
#define vec_srai_i32_1(a, imm8)                  ((imm8 < 32) ? ((int32_t) a)>>imm8 : (((int32_t) a) < 0) ? -1 : 0)


#define vec_sllv_i32_16(a, count)                                      svlsl_s32_z(svptrue_b32(),                        a, svreinterpret_u32_s32(count))
#define vec_sllv_i32_8(a, count)                                       svlsl_s32_z(svptrue_b32(),                         a, svreinterpret_u32_s32(count))
#define vec_sllv_i32_4(a, count)                                       svlsl_s32_z(svptrue_b32(),                         a, svreinterpret_u32_s32(count))
#define vec_sllv_i32_2(a, count)                                       svlsl_s32_z(svptrue_b32(),                         a, svreinterpret_u32_s32(count))
#define vec_sllv_i32_1(a, count)                 ((count < 32) ? ((uint32_t) a)<<count : 0)

#define vec_srlv_i32_16(a, count)                svreinterpret_s32_u32(svlsr_u32_z(svptrue_b32(), svreinterpret_u32_s32(a), svreinterpret_u32_s32(count)))
#define vec_srlv_i32_8(a, count)                 svreinterpret_s32_u32(svlsr_u32_z(svptrue_b32(),  svreinterpret_u32_s32(a), svreinterpret_u32_s32(count)))
#define vec_srlv_i32_4(a, count)                 svreinterpret_s32_u32(svlsr_u32_z(svptrue_b32(),  svreinterpret_u32_s32(a), svreinterpret_u32_s32(count)))
#define vec_srlv_i32_2(a, count)                 svreinterpret_s32_u32(svlsr_u32_z(svptrue_b32(),  svreinterpret_u32_s32(a), svreinterpret_u32_s32(count)))
#define vec_srlv_i32_1(a, count)                 ((count < 32) ? ((uint32_t) a)>>count : 0)

#define vec_srav_i32_16(a, count)                                      svasr_s32_z(svptrue_b32(),                        a, svreinterpret_u32_s32(count))
#define vec_srav_i32_8(a, count)                                       svasr_s32_z(svptrue_b32(),                         a, svreinterpret_u32_s32(count))
#define vec_srav_i32_4(a, count)                                       svasr_s32_z(svptrue_b32(),                         a, svreinterpret_u32_s32(count))
#define vec_srav_i32_2(a, count)                                       svasr_s32_z(svptrue_b32(),                         a, svreinterpret_u32_s32(count))
#define vec_srav_i32_1(a, count)                 ((count < 32) ? ((int32_t) a)>>count : (((int32_t) a) < 0) ? -1 : 0)


#endif /* VECTORIZATION_ARM_SVE_I32_H */
