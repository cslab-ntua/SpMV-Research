#ifndef VECTORIZATION_ARM_SVE_I32_H
#define VECTORIZATION_ARM_SVE_I32_H

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <arm_sve.h>

#include "macros/cpp_defines.h"
#include "macros/macrolib.h"

#include "vectorization/vectorization_util.h"
#include "vectorization_arm_sve_m32.h"


typedef int32_t    __attribute__((may_alias))  vec_alias_int32_t;

/* Without the arm_sve_vector_bits attribute, the sve types have many limitations:
 *     - no sizeof().
 *     - can't include in structs or unions.
 *     - can't use pointers to address them.
 * */
#if __ARM_FEATURE_SVE_BITS==512
	#define vec_len_default_i32  16
	#define vec_pred_true_i32  svptrue_pat_b32(SV_VL16)
	typedef int32_t    __attribute__((may_alias))  vec_i32_1_t;
	typedef svint32_t  __attribute__((arm_sve_vector_bits(512)))  vec_i32_2_t;
	typedef svint32_t  __attribute__((arm_sve_vector_bits(512)))  vec_i32_4_t;
	typedef svint32_t  __attribute__((arm_sve_vector_bits(512)))  vec_i32_8_t;
	typedef svint32_t  __attribute__((arm_sve_vector_bits(512)))  vec_i32_16_t;

	typedef svuint32_t  __attribute__((arm_sve_vector_bits(512))) vec_perm_p32_1_t;
	typedef svuint32_t  __attribute__((arm_sve_vector_bits(512))) vec_perm_p32_2_t;
	typedef svuint32_t  __attribute__((arm_sve_vector_bits(512))) vec_perm_p32_4_t;
	typedef svuint32_t  __attribute__((arm_sve_vector_bits(512))) vec_perm_p32_8_t;
	typedef svuint32_t  __attribute__((arm_sve_vector_bits(512))) vec_perm_p32_16_t;
#elif __ARM_FEATURE_SVE_BITS==256
	#define vec_len_default_i32  8
	#define vec_pred_true_i32  svptrue_pat_b32(SV_VL8)
	typedef int32_t    __attribute__((may_alias))  vec_i32_1_t;
	typedef svint32_t  __attribute__((arm_sve_vector_bits(256)))  vec_i32_2_t;
	typedef svint32_t  __attribute__((arm_sve_vector_bits(256)))  vec_i32_4_t;
	typedef svint32_t  __attribute__((arm_sve_vector_bits(256)))  vec_i32_8_t;
	typedef svint32_t  __attribute__((arm_sve_vector_bits(256)))  vec_i32_16_t;

	typedef svuint32_t  __attribute__((arm_sve_vector_bits(256))) vec_perm_p32_1_t;
	typedef svuint32_t  __attribute__((arm_sve_vector_bits(256))) vec_perm_p32_2_t;
	typedef svuint32_t  __attribute__((arm_sve_vector_bits(256))) vec_perm_p32_4_t;
	typedef svuint32_t  __attribute__((arm_sve_vector_bits(256))) vec_perm_p32_8_t;
	typedef svuint32_t  __attribute__((arm_sve_vector_bits(256))) vec_perm_p32_16_t;
#elif __ARM_FEATURE_SVE_BITS==128
	#define vec_len_default_i32  4
	#define vec_pred_true_i32  svptrue_pat_b32(SV_VL4)
	typedef int32_t    __attribute__((may_alias))  vec_i32_1_t;
	typedef svint32_t  __attribute__((arm_sve_vector_bits(128)))  vec_i32_2_t;
	typedef svint32_t  __attribute__((arm_sve_vector_bits(128)))  vec_i32_4_t;
	typedef svint32_t  __attribute__((arm_sve_vector_bits(128)))  vec_i32_8_t;
	typedef svint32_t  __attribute__((arm_sve_vector_bits(128)))  vec_i32_16_t;

	typedef svuint32_t  __attribute__((arm_sve_vector_bits(128))) vec_perm_p32_1_t;
	typedef svuint32_t  __attribute__((arm_sve_vector_bits(128))) vec_perm_p32_2_t;
	typedef svuint32_t  __attribute__((arm_sve_vector_bits(128))) vec_perm_p32_4_t;
	typedef svuint32_t  __attribute__((arm_sve_vector_bits(128))) vec_perm_p32_8_t;
	typedef svuint32_t  __attribute__((arm_sve_vector_bits(128))) vec_perm_p32_16_t;
#else
	#error "__ARM_FEATURE_SVE_BITS not defined"
#endif

typedef uint32_t  vec_perm_elem_p32_1_t;
typedef uint32_t  vec_perm_elem_p32_2_t;
typedef uint32_t  vec_perm_elem_p32_4_t;
typedef uint32_t  vec_perm_elem_p32_8_t;
typedef uint32_t  vec_perm_elem_p32_16_t;

typedef uint32_t  vec_i32_t __attribute__((may_alias));


//------------------------------------------------------------------------------------------------------------------------------------------
//- Set - Load - Store
//------------------------------------------------------------------------------------------------------------------------------------------

#define vec_elem_get_i32_16(vec, index)                    ( ((vec_alias_int32_t *) &vec)[index] )
#define vec_elem_get_i32_8(vec, index)                     ( ((vec_alias_int32_t *) &vec)[index] )
#define vec_elem_get_i32_4(vec, index)                     ( ((vec_alias_int32_t *) &vec)[index] )
#define vec_elem_get_i32_2(vec, index)                     ( ((vec_alias_int32_t *) &vec)[index] )
#define vec_elem_get_i32_1(vec, index)                     vec

#define vec_elem_set_i32_16(vec, index, expr)              do { ((vec_alias_int32_t *) &vec)[index] = (expr); } while (0)
#define vec_elem_set_i32_8(vec, index, expr)               do { ((vec_alias_int32_t *) &vec)[index] = (expr); } while (0)
#define vec_elem_set_i32_4(vec, index, expr)               do { ((vec_alias_int32_t *) &vec)[index] = (expr); } while (0)
#define vec_elem_set_i32_2(vec, index, expr)               do { ((vec_alias_int32_t *) &vec)[index] = (expr); } while (0)
#define vec_elem_set_i32_1(vec, index, expr)               do { vec = (expr); } while (0)

#define vec_array_i32_16(vec)                              ((vec_alias_int32_t *) &vec)
#define vec_array_i32_8(vec)                               ((vec_alias_int32_t *) &vec)
#define vec_array_i32_4(vec)                               ((vec_alias_int32_t *) &vec)
#define vec_array_i32_2(vec)                               ((vec_alias_int32_t *) &vec)
#define vec_array_i32_1(vec)                               ((vec_alias_int32_t *) &vec)

#define vec_set1_i32_16(val)                               svdup_n_s32(val)
#define vec_set1_i32_8(val)                                svdup_n_s32(val)
#define vec_set1_i32_4(val)                                svdup_n_s32(val)
#define vec_set1_i32_2(val)                                svdup_n_s32(val)
#define vec_set1_i32_1(val)                                val

#define vec_set_iter_i32_16(iter, expr)                    ({ vec_i32_16_t  _buf; vec_loop_stmt( 16, iter, ((vec_alias_int32_t *) &_buf)[iter] = (expr););  _buf; })
#define vec_set_iter_i32_8(iter, expr)                     ({ vec_i32_8_t   _buf; vec_loop_stmt(  8, iter, ((vec_alias_int32_t *) &_buf)[iter] = (expr););  _buf; })
#define vec_set_iter_i32_4(iter, expr)                     ({ vec_i32_4_t   _buf; vec_loop_stmt(  4, iter, ((vec_alias_int32_t *) &_buf)[iter] = (expr););  _buf; })
#define vec_set_iter_i32_2(iter, expr)                     ({ vec_i32_2_t   _buf; vec_loop_stmt(  2, iter, ((vec_alias_int32_t *) &_buf)[iter] = (expr););  _buf; })
#define vec_set_iter_i32_1(iter, expr)                     (  ( vec_alias_int32_t) vec_iter_expr_1(iter, 0, (expr)) )

#define vec_set_iter_p32_16(iter, expr)                    ({ vec_i32_16_t  _buf; vec_loop_stmt( 16, iter, ((vec_alias_int32_t *) &_buf)[iter] = (expr););  _buf; })
#define vec_set_iter_p32_8(iter, expr)                     ({ vec_i32_8_t   _buf; vec_loop_stmt(  8, iter, ((vec_alias_int32_t *) &_buf)[iter] = (expr););  _buf; })
#define vec_set_iter_p32_4(iter, expr)                     ({ vec_i32_4_t   _buf; vec_loop_stmt(  4, iter, ((vec_alias_int32_t *) &_buf)[iter] = (expr););  _buf; })
#define vec_set_iter_p32_2(iter, expr)                     ({ vec_i32_2_t   _buf; vec_loop_stmt(  2, iter, ((vec_alias_int32_t *) &_buf)[iter] = (expr););  _buf; })
#define vec_set_iter_p32_1(iter, expr)                     (  ( vec_alias_int32_t) vec_iter_expr_1(iter, 0, (expr)) )

#define vec_loadu_i32_16(ptr)                              svld1_s32(svptrue_pat_b32(SV_VL16), ((int32_t*) (ptr)))
#define vec_loadu_i32_8(ptr)                               svld1_s32(svptrue_pat_b32(SV_VL8), ((int32_t*) (ptr)))
#define vec_loadu_i32_4(ptr)                               svld1_s32(svptrue_pat_b32(SV_VL4), ((int32_t*) (ptr)))
#define vec_loadu_i32_2(ptr)                               svld1_s32(svptrue_pat_b32(SV_VL2), ((int32_t*) (ptr)))
#define vec_loadu_i32_1(ptr)                               (*((vec_alias_int32_t*) (ptr)))

#define vec_loadu_maskedz_i32_16(ptr, mask)                svld1_s32(mask, ((int32_t*) (ptr)))
#define vec_loadu_maskedz_i32_8(ptr, mask)                 svld1_s32(mask, ((int32_t*) (ptr)))
#define vec_loadu_maskedz_i32_4(ptr, mask)                 svld1_s32(mask, ((int32_t*) (ptr)))
#define vec_loadu_maskedz_i32_2(ptr, mask)                 svld1_s32(mask, ((int32_t*) (ptr)))
#define vec_loadu_maskedz_i32_1(ptr, mask)                 ( (mask) ? (*((vec_alias_int32_t *) (ptr))) : 0 )

#define vec_storeu_i32_16(ptr, vec)                        svst1_s32(svptrue_pat_b32(SV_VL16), ((int32_t*) (ptr)), vec)
#define vec_storeu_i32_8(ptr, vec)                         svst1_s32(svptrue_pat_b32(SV_VL8), ((int32_t*) (ptr)), vec)
#define vec_storeu_i32_4(ptr, vec)                         svst1_s32(svptrue_pat_b32(SV_VL4), ((int32_t*) (ptr)), vec)
#define vec_storeu_i32_2(ptr, vec)                         svst1_s32(svptrue_pat_b32(SV_VL2), ((int32_t*) (ptr)), vec)
#define vec_storeu_i32_1(ptr, vec)                         do { (*((vec_alias_int32_t*) (ptr))) = (vec); } while (0)

#define vec_storeu_masked_i32_16(ptr, vec, mask)           svst1_s32(mask, ((int32_t*) (ptr)), vec)
#define vec_storeu_masked_i32_8(ptr, vec, mask)            svst1_s32(mask, ((int32_t*) (ptr)), vec)
#define vec_storeu_masked_i32_4(ptr, vec, mask)            svst1_s32(mask, ((int32_t*) (ptr)), vec)
#define vec_storeu_masked_i32_2(ptr, vec, mask)            svst1_s32(mask, ((int32_t*) (ptr)), vec)
#define vec_storeu_masked_i32_1(ptr, vec, mask)            do { if (mask) (*((vec_alias_int32_t *) (ptr))) = (vec); } while (0)

#define vec_gather_i32_i32_16(ptr, idx)                    svld1_gather_s32index_s32(svptrue_b32(), (vec_alias_int32_t *) (ptr), idx)
#define vec_gather_i32_i32_8(ptr, idx)                     svld1_gather_s32index_s32(svptrue_b32(), (vec_alias_int32_t *) (ptr), idx)
#define vec_gather_i32_i32_4(ptr, idx)                     svld1_gather_s32index_s32(svptrue_b32(), (vec_alias_int32_t *) (ptr), idx)
#define vec_gather_i32_i32_2(ptr, idx)                     svld1_gather_s32index_s32(svptrue_b32(), (vec_alias_int32_t *) (ptr), idx)
#define vec_gather_i32_i32_1(ptr, idx)                     ( ((vec_alias_int32_t *) (ptr))[idx] )

// #define vec_gather_i32_i64_256(ptr, idx)         svld1_gather_s64index_s32(svptrue_b32(), (vec_alias_int32_t *) (ptr), idx)
// #define vec_gather_i32_i64_16(ptr, idx)          svld1_gather_s64index_s32(svptrue_b32(), (vec_alias_int32_t *) (ptr), idx)
// #define vec_gather_i32_i64_8(ptr, idx)           svld1_gather_s64index_s32(svptrue_b32(), (vec_alias_int32_t *) (ptr), idx)
// #define vec_gather_i32_i64_4(ptr, idx)           svld1_gather_s64index_s32(svptrue_b32(), (vec_alias_int32_t *) (ptr), idx)
// #define vec_gather_i32_i64_2(ptr, idx)           svld1_gather_s64index_s32(svptrue_b32(), (vec_alias_int32_t *) (ptr), idx)
// #define vec_gather_i32_i64_1(ptr, idx)           ( ((vec_alias_int32_t *) (ptr))[idx] )


//------------------------------------------------------------------------------------------------------------------------------------------
//- Operations
//------------------------------------------------------------------------------------------------------------------------------------------

#define vec_add_i32_16(a, b)                               svadd_s32_z(svptrue_b32(), a, b)
#define vec_add_i32_8(a, b)                                svadd_s32_z(svptrue_b32(), a, b)
#define vec_add_i32_4(a, b)                                svadd_s32_z(svptrue_b32(), a, b)
#define vec_add_i32_2(a, b)                                svadd_s32_z(svptrue_b32(), a, b)
#define vec_add_i32_1(a, b)                                (a + b)

#define vec_sub_i32_16(a, b)                                svsub_s32_z(svptrue_b32(), a, b)
#define vec_sub_i32_8(a, b)                                svsub_s32_z(svptrue_b32(), a, b)
#define vec_sub_i32_4(a, b)                                svsub_s32_z(svptrue_b32(), a, b)
#define vec_sub_i32_2(a, b)                                svsub_s32_z(svptrue_b32(), a, b)
#define vec_sub_i32_1(a, b)                                (a - b)

#define vec_mul_i32_16(a, b)                               svmul_s32_z(svptrue_b32(), a, b)
#define vec_mul_i32_8(a, b)                                svmul_s32_z(svptrue_b32(), a, b)
#define vec_mul_i32_4(a, b)                                svmul_s32_z(svptrue_b32(), a, b)
#define vec_mul_i32_2(a, b)                                svmul_s32_z(svptrue_b32(), a, b)
#define vec_mul_i32_1(a, b)                                (a * b)

#define vec_div_i32_16(a, b)                               svdiv_s32_z(svptrue_b32(), a, b)
#define vec_div_i32_8(a, b)                                svdiv_s32_z(svptrue_b32(), a, b)
#define vec_div_i32_4(a, b)                                svdiv_s32_z(svptrue_b32(), a, b)
#define vec_div_i32_2(a, b)                                svdiv_s32_z(svptrue_b32(), a, b)
#define vec_div_i32_1(a, b)                                (a / b)


#define vec_and_i32_16(a, b)                               svand_s32_z(svptrue_b32(), a, b)
#define vec_and_i32_8(a, b)                                svand_s32_z(svptrue_b32(), a, b)
#define vec_and_i32_4(a, b)                                svand_s32_z(svptrue_b32(), a, b)
#define vec_and_i32_2(a, b)                                svand_s32_z(svptrue_b32(), a, b)
#define vec_and_i32_1(a, b)                                (a & b)

#define vec_or_i32_16(a, b)                                svorr_s32_z(svptrue_b32(), a, b)
#define vec_or_i32_8(a, b)                                 svorr_s32_z(svptrue_b32(), a, b)
#define vec_or_i32_4(a, b)                                 svorr_s32_z(svptrue_b32(), a, b)
#define vec_or_i32_2(a, b)                                 svorr_s32_z(svptrue_b32(), a, b)
#define vec_or_i32_1(a, b)                                 (a | b)

#define vec_not_i32_16(a)                                  svnot_s32_z(svptrue_b32(), a)
#define vec_not_i32_8(a)                                   svnot_s32_z(svptrue_b32(), a)
#define vec_not_i32_4(a)                                   svnot_s32_z(svptrue_b32(), a)
#define vec_not_i32_2(a)                                   svnot_s32_z(svptrue_b32(), a)
#define vec_not_i32_1(a)                                   (a ^ -1)

#define vec_xor_i32_16(a, b)                               sveor_s32_z(svptrue_b32(), a, b)
#define vec_xor_i32_8(a, b)                                sveor_s32_z(svptrue_b32(), a, b)
#define vec_xor_i32_4(a, b)                                sveor_s32_z(svptrue_b32(), a, b)
#define vec_xor_i32_2(a, b)                                sveor_s32_z(svptrue_b32(), a, b)
#define vec_xor_i32_1(a, b)                                (a ^ b)


//------------------------------------------------------------------------------------------------------------------------------------------
//- Shift
//------------------------------------------------------------------------------------------------------------------------------------------

#define vec_slli_i32_16(a, imm8)                           svlsl_n_s32_z(svptrue_b32(), a, imm8)
#define vec_slli_i32_8(a, imm8)                            svlsl_n_s32_z(svptrue_b32(), a, imm8)
#define vec_slli_i32_4(a, imm8)                            svlsl_n_s32_z(svptrue_b32(), a, imm8)
#define vec_slli_i32_2(a, imm8)                            svlsl_n_s32_z(svptrue_b32(), a, imm8)
#define vec_slli_i32_1(a, imm8)                            ((imm8 < 32) ? ((uint32_t) a)<<imm8 : 0)

#define vec_srli_i32_16(a, imm8)                           svreinterpret_s32_u32(svlsr_n_u32_z(svptrue_b32(), svreinterpret_u32_s32(a), imm8))
#define vec_srli_i32_8(a, imm8)                            svreinterpret_s32_u32(svlsr_n_u32_z(svptrue_b32(),  svreinterpret_u32_s32(a), imm8))
#define vec_srli_i32_4(a, imm8)                            svreinterpret_s32_u32(svlsr_n_u32_z(svptrue_b32(),  svreinterpret_u32_s32(a), imm8))
#define vec_srli_i32_2(a, imm8)                            svreinterpret_s32_u32(svlsr_n_u32_z(svptrue_b32(),  svreinterpret_u32_s32(a), imm8))
#define vec_srli_i32_1(a, imm8)                            ((imm8 < 32) ? ((uint32_t) a)>>imm8 : 0)

#define vec_srai_i32_16(a, imm8)                           svasr_n_s32_z(svptrue_b32(), a, imm8)
#define vec_srai_i32_8(a, imm8)                            svasr_n_s32_z(svptrue_b32(), a, imm8)
#define vec_srai_i32_4(a, imm8)                            svasr_n_s32_z(svptrue_b32(), a, imm8)
#define vec_srai_i32_2(a, imm8)                            svasr_n_s32_z(svptrue_b32(), a, imm8)
#define vec_srai_i32_1(a, imm8)                            ((imm8 < 32) ? ((int32_t) a)>>imm8 : (((int32_t) a) < 0) ? -1 : 0)


#define vec_sllv_i32_16(a, count)                          svlsl_s32_z(svptrue_b32(), a, svreinterpret_u32_s32(count))
#define vec_sllv_i32_8(a, count)                           svlsl_s32_z(svptrue_b32(), a, svreinterpret_u32_s32(count))
#define vec_sllv_i32_4(a, count)                           svlsl_s32_z(svptrue_b32(), a, svreinterpret_u32_s32(count))
#define vec_sllv_i32_2(a, count)                           svlsl_s32_z(svptrue_b32(), a, svreinterpret_u32_s32(count))
#define vec_sllv_i32_1(a, count)                           ((count < 32) ? ((uint32_t) a)<<count : 0)

#define vec_srlv_i32_16(a, count)                          svreinterpret_s32_u32(svlsr_u32_z(svptrue_b32(), svreinterpret_u32_s32(a), svreinterpret_u32_s32(count)))
#define vec_srlv_i32_8(a, count)                           svreinterpret_s32_u32(svlsr_u32_z(svptrue_b32(),  svreinterpret_u32_s32(a), svreinterpret_u32_s32(count)))
#define vec_srlv_i32_4(a, count)                           svreinterpret_s32_u32(svlsr_u32_z(svptrue_b32(),  svreinterpret_u32_s32(a), svreinterpret_u32_s32(count)))
#define vec_srlv_i32_2(a, count)                           svreinterpret_s32_u32(svlsr_u32_z(svptrue_b32(),  svreinterpret_u32_s32(a), svreinterpret_u32_s32(count)))
#define vec_srlv_i32_1(a, count)                           ((count < 32) ? ((uint32_t) a)>>count : 0)

#define vec_srav_i32_16(a, count)                          svasr_s32_z(svptrue_b32(), a, svreinterpret_u32_s32(count))
#define vec_srav_i32_8(a, count)                           svasr_s32_z(svptrue_b32(), a, svreinterpret_u32_s32(count))
#define vec_srav_i32_4(a, count)                           svasr_s32_z(svptrue_b32(), a, svreinterpret_u32_s32(count))
#define vec_srav_i32_2(a, count)                           svasr_s32_z(svptrue_b32(), a, svreinterpret_u32_s32(count))
#define vec_srav_i32_1(a, count)                           ((count < 32) ? ((int32_t) a)>>count : (((int32_t) a) < 0) ? -1 : 0)


//------------------------------------------------------------------------------------------------------------------------------------------
//- Compare
//------------------------------------------------------------------------------------------------------------------------------------------

#define vec_cmpeq_i32_16(a, b)                             svcmpeq_s32(svptrue_b32(), a, b)
#define vec_cmpeq_i32_8(a, b)                              svcmpeq_s32(svptrue_b32(), a, b)
#define vec_cmpeq_i32_4(a, b)                              svcmpeq_s32(svptrue_b32(), a, b)
#define vec_cmpeq_i32_2(a, b)                              svcmpeq_s32(svptrue_b32(), a, b)
#define vec_cmpeq_i32_1(a, b)                              (a == b)

#define vec_cmpgt_i32_16(a, b)                             svcmpgt_s32(svptrue_b32(), a, b)
#define vec_cmpgt_i32_8(a, b)                              svcmpgt_s32(svptrue_b32(), a, b)
#define vec_cmpgt_i32_4(a, b)                              svcmpgt_s32(svptrue_b32(), a, b)
#define vec_cmpgt_i32_2(a, b)                              svcmpgt_s32(svptrue_b32(), a, b)
#define vec_cmpgt_i32_1(a, b)                              (a > b)


//------------------------------------------------------------------------------------------------------------------------------------------
//- Shuffle - Permute
//------------------------------------------------------------------------------------------------------------------------------------------

#define vec_permute_i32_16(a, idx)                         svtbl_s32(a, idx)
#define vec_permute_i32_8(a, idx)                          svtbl_s32(a, idx)
#define vec_permute_i32_4(a, idx)                          svtbl_s32(a, idx)
#define vec_permute_i32_2(a, idx)                          svtbl_s32(a, idx)
#define vec_permute_i32_1(a, idx)                          a

#endif /* VECTORIZATION_ARM_SVE_I32_H */
