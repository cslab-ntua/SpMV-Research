#ifndef VECTORIZATION_ARM_SVE_I64_H
#define VECTORIZATION_ARM_SVE_I64_H

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <arm_sve.h>

#include "macros/cpp_defines.h"
#include "macros/macrolib.h"

#include "vectorization/vectorization_util.h"
#include "vectorization_arm_sve_m64.h"


typedef int64_t    __attribute__((may_alias))  vec_alias_int64_t;

/* Without the arm_sve_vector_bits attribute, the sve types have many limitations:
 *     - no sizeof().
 *     - can't include in structs or unions.
 *     - can't use pointers to address them.
 * */
#if __ARM_FEATURE_SVE_BITS==512
	#define vec_len_default_i64  8
	#define vec_len_default_i64_f2  4
	#define vec_pred_true_i64  svptrue_pat_b64(SV_VL8)
	typedef int64_t    __attribute__((may_alias))  vec_i64_1_t;
	typedef svint64_t  __attribute__((arm_sve_vector_bits(512)))  vec_i64_2_t;
	typedef svint64_t  __attribute__((arm_sve_vector_bits(512)))  vec_i64_4_t;
	typedef svint64_t  __attribute__((arm_sve_vector_bits(512)))  vec_i64_8_t;

	typedef svuint64_t  __attribute__((arm_sve_vector_bits(512))) vec_perm_p64_1_t;
	typedef svuint64_t  __attribute__((arm_sve_vector_bits(512))) vec_perm_p64_2_t;
	typedef svuint64_t  __attribute__((arm_sve_vector_bits(512))) vec_perm_p64_4_t;
	typedef svuint64_t  __attribute__((arm_sve_vector_bits(512))) vec_perm_p64_8_t;
#elif __ARM_FEATURE_SVE_BITS==256
	#define vec_len_default_i64  4
	#define vec_len_default_i64_f2  2
	#define vec_pred_true_i64  svptrue_pat_b64(SV_VL4)
	typedef int64_t    __attribute__((may_alias))  vec_i64_1_t;
	typedef svint64_t  __attribute__((arm_sve_vector_bits(256)))  vec_i64_2_t;
	typedef svint64_t  __attribute__((arm_sve_vector_bits(256)))  vec_i64_4_t;
	typedef svint64_t  __attribute__((arm_sve_vector_bits(256)))  vec_i64_8_t;

	typedef svuint64_t  __attribute__((arm_sve_vector_bits(256))) vec_perm_p64_1_t;
	typedef svuint64_t  __attribute__((arm_sve_vector_bits(256))) vec_perm_p64_2_t;
	typedef svuint64_t  __attribute__((arm_sve_vector_bits(256))) vec_perm_p64_4_t;
	typedef svuint64_t  __attribute__((arm_sve_vector_bits(256))) vec_perm_p64_8_t;
#elif __ARM_FEATURE_SVE_BITS==128
	#define vec_len_default_i64  2
	#define vec_len_default_i64_f2  1
	#define vec_pred_true_i64  svptrue_pat_b64(SV_VL2)
	typedef int64_t    __attribute__((may_alias))  vec_i64_1_t;
	typedef svint64_t  __attribute__((arm_sve_vector_bits(128)))  vec_i64_2_t;
	typedef svint64_t  __attribute__((arm_sve_vector_bits(128)))  vec_i64_4_t;
	typedef svint64_t  __attribute__((arm_sve_vector_bits(128)))  vec_i64_8_t;

	typedef svuint64_t  __attribute__((arm_sve_vector_bits(128))) vec_perm_p64_1_t;
	typedef svuint64_t  __attribute__((arm_sve_vector_bits(128))) vec_perm_p64_2_t;
	typedef svuint64_t  __attribute__((arm_sve_vector_bits(128))) vec_perm_p64_4_t;
	typedef svuint64_t  __attribute__((arm_sve_vector_bits(128))) vec_perm_p64_8_t;
#else
	#error "__ARM_FEATURE_SVE_BITS not defined"
#endif

typedef uint64_t  vec_perm_elem_p64_1_t;
typedef uint64_t  vec_perm_elem_p64_2_t;
typedef uint64_t  vec_perm_elem_p64_4_t;
typedef uint64_t  vec_perm_elem_p64_8_t;

typedef uint64_t  vec_i64_t __attribute__((may_alias));


//------------------------------------------------------------------------------------------------------------------------------------------
//- Set - Load - Store
//------------------------------------------------------------------------------------------------------------------------------------------

#define vec_elem_get_i64_8(vec, index)                     ( ((vec_alias_int64_t *) &vec)[index] )
#define vec_elem_get_i64_4(vec, index)                     ( ((vec_alias_int64_t *) &vec)[index] )
#define vec_elem_get_i64_2(vec, index)                     ( ((vec_alias_int64_t *) &vec)[index] )
#define vec_elem_get_i64_1(vec, index)                     vec

#define vec_elem_set_i64_8(vec, index, expr)               do { ((vec_alias_int64_t *) &vec)[index] = (expr); } while (0)
#define vec_elem_set_i64_4(vec, index, expr)               do { ((vec_alias_int64_t *) &vec)[index] = (expr); } while (0)
#define vec_elem_set_i64_2(vec, index, expr)               do { ((vec_alias_int64_t *) &vec)[index] = (expr); } while (0)
#define vec_elem_set_i64_1(vec, index, expr)               do { vec = (expr); } while (0)

#define vec_array_i64_8(vec)                               ((vec_alias_int64_t *) &vec)
#define vec_array_i64_4(vec)                               ((vec_alias_int64_t *) &vec)
#define vec_array_i64_2(vec)                               ((vec_alias_int64_t *) &vec)
#define vec_array_i64_1(vec)                               ((vec_alias_int64_t *) &vec)

#define vec_set1_i64_8(val)                                svdup_n_s64(val)
#define vec_set1_i64_4(val)                                svdup_n_s64(val)
#define vec_set1_i64_2(val)                                svdup_n_s64(val)
#define vec_set1_i64_1(val)                                val

#define vec_set_iter_i64_8(iter, expr)                     ({ vec_i64_8_t  _buf; vec_loop_stmt(   8, iter, ((vec_alias_int64_t *) &_buf)[iter] = (expr););  _buf; })
#define vec_set_iter_i64_4(iter, expr)                     ({ vec_i64_4_t  _buf; vec_loop_stmt(   4, iter, ((vec_alias_int64_t *) &_buf)[iter] = (expr););  _buf; })
#define vec_set_iter_i64_2(iter, expr)                     ({ vec_i64_2_t  _buf; vec_loop_stmt(   2, iter, ((vec_alias_int64_t *) &_buf)[iter] = (expr););  _buf; })
#define vec_set_iter_i64_1(iter, expr)                     ( (int64_t) vec_iter_expr_1(iter, 0, (expr)) )

#define vec_set_iter_p64_8(iter, expr)                     ({ vec_i64_8_t  _buf; vec_loop_stmt(   8, iter, ((vec_alias_int64_t *) &_buf)[iter] = (expr););  _buf; })
#define vec_set_iter_p64_4(iter, expr)                     ({ vec_i64_4_t  _buf; vec_loop_stmt(   4, iter, ((vec_alias_int64_t *) &_buf)[iter] = (expr););  _buf; })
#define vec_set_iter_p64_2(iter, expr)                     ({ vec_i64_2_t  _buf; vec_loop_stmt(   2, iter, ((vec_alias_int64_t *) &_buf)[iter] = (expr););  _buf; })
#define vec_set_iter_p64_1(iter, expr)                     (   (int64_t) vec_iter_expr_1(iter, 0, (expr)) )

#define vec_loadu_i64_8(ptr)                               svld1_s64(svptrue_pat_b64(SV_VL8), ((int64_t*) (ptr)))
#define vec_loadu_i64_4(ptr)                               svld1_s64(svptrue_pat_b64(SV_VL4), ((int64_t*) (ptr)))
#define vec_loadu_i64_2(ptr)                               svld1_s64(svptrue_pat_b64(SV_VL2), ((int64_t*) (ptr)))
#define vec_loadu_i64_1(ptr)                               (*((vec_alias_int64_t*) (ptr)))

#define vec_loadu_maskedz_i64_8(ptr, mask)                 svld1_s64(mask, ((int64_t*) (ptr)))
#define vec_loadu_maskedz_i64_4(ptr, mask)                 svld1_s64(mask, ((int64_t*) (ptr)))
#define vec_loadu_maskedz_i64_2(ptr, mask)                 svld1_s64(mask, ((int64_t*) (ptr)))
#define vec_loadu_maskedz_i64_1(ptr, mask)                 ( (mask) ? (*((vec_alias_int64_t *) (ptr))) : 0 )

#define vec_storeu_i64_8(ptr, vec)                         svst1_s64(svptrue_pat_b64(SV_VL8), ((int64_t*) (ptr)), vec)
#define vec_storeu_i64_4(ptr, vec)                         svst1_s64(svptrue_pat_b64(SV_VL4), ((int64_t*) (ptr)), vec)
#define vec_storeu_i64_2(ptr, vec)                         svst1_s64(svptrue_pat_b64(SV_VL2), ((int64_t*) (ptr)), vec)
#define vec_storeu_i64_1(ptr, vec)                         do { (*((vec_alias_int64_t*) (ptr))) = (vec); } while (0)

#define vec_storeu_masked_i64_8(ptr, vec, mask)            svst1_s64(mask, ((int64_t*) (ptr)), vec)
#define vec_storeu_masked_i64_4(ptr, vec, mask)            svst1_s64(mask, ((int64_t*) (ptr)), vec)
#define vec_storeu_masked_i64_2(ptr, vec, mask)            svst1_s64(mask, ((int64_t*) (ptr)), vec)
#define vec_storeu_masked_i64_1(ptr, vec, mask)            do { if (mask) (*((vec_alias_int64_t *) (ptr))) = (vec); } while (0)

#define vec_gather_i64_i32_8(ptr, idx)                     svld1_gather_s64index_s64(svptrue_b64(), (vec_alias_int64_t *) (ptr), svunpklo_u64(idx))
#define vec_gather_i64_i32_4(ptr, idx)                     svld1_gather_s64index_s64(svptrue_b64(), (vec_alias_int64_t *) (ptr), svunpklo_u64(idx))
#define vec_gather_i64_i32_2(ptr, idx)                     svld1_gather_s64index_s64(svptrue_b64(), (vec_alias_int64_t *) (ptr), svunpklo_u64(idx))
#define vec_gather_i64_i32_1(ptr, idx)                     ( ((vec_alias_int64_t *) (ptr))[idx] )

#define vec_gather_i64_i64_8(ptr, idx)                     svld1_gather_s64index_s64(svptrue_b64(), (vec_alias_int64_t *) (ptr), idx)
#define vec_gather_i64_i64_4(ptr, idx)                     svld1_gather_s64index_s64(svptrue_b64(), (vec_alias_int64_t *) (ptr), idx)
#define vec_gather_i64_i64_2(ptr, idx)                     svld1_gather_s64index_s64(svptrue_b64(), (vec_alias_int64_t *) (ptr), idx)
#define vec_gather_i64_i64_1(ptr, idx)                     ( ((vec_alias_int64_t *) (ptr))[idx] )


//------------------------------------------------------------------------------------------------------------------------------------------
//- Operations
//------------------------------------------------------------------------------------------------------------------------------------------

#define vec_add_i64_8(a, b)                                svadd_s64_z(svptrue_b64(), a, b)
#define vec_add_i64_4(a, b)                                svadd_s64_z(svptrue_b64(), a, b)
#define vec_add_i64_2(a, b)                                svadd_s64_z(svptrue_b64(), a, b)
#define vec_add_i64_1(a, b)                                (a + b)

#define vec_sub_i64_8(a, b)                                svsub_s64_z(svptrue_b64(), a, b)
#define vec_sub_i64_4(a, b)                                svsub_s64_z(svptrue_b64(), a, b)
#define vec_sub_i64_2(a, b)                                svsub_s64_z(svptrue_b64(), a, b)
#define vec_sub_i64_1(a, b)                                (a - b)

#define vec_mul_i64_8(a, b)                                svmul_s64_z(svptrue_b64(), a, b)
#define vec_mul_i64_4(a, b)                                svmul_s64_z(svptrue_b64(), a, b)
#define vec_mul_i64_2(a, b)                                svmul_s64_z(svptrue_b64(), a, b)
#define vec_mul_i64_1(a, b)                                (a * b)

#define vec_div_i64_8(a, b)                                svdiv_s64_z(svptrue_b64(), a, b)
#define vec_div_i64_4(a, b)                                svdiv_s64_z(svptrue_b64(), a, b)
#define vec_div_i64_2(a, b)                                svdiv_s64_z(svptrue_b64(), a, b)
#define vec_div_i64_1(a, b)                                (a / b)


#define vec_and_i64_8(a, b)                                svand_s64_z(svptrue_b64(), a, b)
#define vec_and_i64_4(a, b)                                svand_s64_z(svptrue_b64(), a, b)
#define vec_and_i64_2(a, b)                                svand_s64_z(svptrue_b64(), a, b)
#define vec_and_i64_1(a, b)                                (a & b)

#define vec_or_i64_8(a, b)                                 svorr_s64_z(svptrue_b64(), a, b)
#define vec_or_i64_4(a, b)                                 svorr_s64_z(svptrue_b64(), a, b)
#define vec_or_i64_2(a, b)                                 svorr_s64_z(svptrue_b64(), a, b)
#define vec_or_i64_1(a, b)                                 (a | b)

#define vec_not_i64_8(a)                                   svnot_s64_z(svptrue_b64(), a)
#define vec_not_i64_4(a)                                   svnot_s64_z(svptrue_b64(), a)
#define vec_not_i64_2(a)                                   svnot_s64_z(svptrue_b64(), a)
#define vec_not_i64_1(a)                                   (a ^ -1)

#define vec_xor_i64_8(a, b)                                sveor_s64_z(svptrue_b64(), a, b)
#define vec_xor_i64_4(a, b)                                sveor_s64_z(svptrue_b64(), a, b)
#define vec_xor_i64_2(a, b)                                sveor_s64_z(svptrue_b64(), a, b)
#define vec_xor_i64_1(a, b)                                (a ^ b)


//------------------------------------------------------------------------------------------------------------------------------------------
//- Shift
//------------------------------------------------------------------------------------------------------------------------------------------

#define vec_slli_i64_8(a, imm8)                            svlsl_n_s64_z(svptrue_b64(), a, imm8)
#define vec_slli_i64_4(a, imm8)                            svlsl_n_s64_z(svptrue_b64(), a, imm8)
#define vec_slli_i64_2(a, imm8)                            svlsl_n_s64_z(svptrue_b64(), a, imm8)
#define vec_slli_i64_1(a, imm8)                            ((imm8 < 64) ? ((uint64_t) a)<<imm8 : 0)

#define vec_srli_i64_8(a, imm8)                            svreinterpret_s64_u64(svlsr_n_u64_z(svptrue_b64(), svreinterpret_u64_s64(a), imm8))
#define vec_srli_i64_4(a, imm8)                            svreinterpret_s64_u64(svlsr_n_u64_z(svptrue_b64(), svreinterpret_u64_s64(a), imm8))
#define vec_srli_i64_2(a, imm8)                            svreinterpret_s64_u64(svlsr_n_u64_z(svptrue_b64(), svreinterpret_u64_s64(a), imm8))
#define vec_srli_i64_1(a, imm8)                            ((imm8 < 64) ? ((uint64_t) a)>>imm8 : 0)

#define vec_srai_i64_8(a, imm8)                            svasr_n_s64_z(svptrue_b64(), a, imm8)
#define vec_srai_i64_4(a, imm8)                            svasr_n_s64_z(svptrue_b64(), a, imm8)
#define vec_srai_i64_2(a, imm8)                            svasr_n_s64_z(svptrue_b64(), a, imm8)
#define vec_srai_i64_1(a, imm8)                            ((imm8 < 64) ? ((int64_t) a)>>imm8 : (((int64_t) a) < 0) ? -1 : 0)

#define vec_sllv_i64_8(a, count)                           svlsl_s64_z(svptrue_b64(), a, svreinterpret_u64_s64(count))
#define vec_sllv_i64_4(a, count)                           svlsl_s64_z(svptrue_b64(), a, svreinterpret_u64_s64(count))
#define vec_sllv_i64_2(a, count)                           svlsl_s64_z(svptrue_b64(), a, svreinterpret_u64_s64(count))
#define vec_sllv_i64_1(a, count)                           ((count < 64) ? ((uint64_t) a)<<count : 0)

#define vec_srlv_i64_8(a, count)                           svreinterpret_s64_u64(svlsr_u64_z(svptrue_b64(), svreinterpret_u64_s64(a), svreinterpret_u64_s64(count)))
#define vec_srlv_i64_4(a, count)                           svreinterpret_s64_u64(svlsr_u64_z(svptrue_b64(), svreinterpret_u64_s64(a), svreinterpret_u64_s64(count)))
#define vec_srlv_i64_2(a, count)                           svreinterpret_s64_u64(svlsr_u64_z(svptrue_b64(), svreinterpret_u64_s64(a), svreinterpret_u64_s64(count)))
#define vec_srlv_i64_1(a, count)                           ((count < 64) ? ((uint64_t) a)>>count : 0)

#define vec_srav_i64_8(a, count)                           svasr_s64_z(svptrue_b64(), a, svreinterpret_u64_s64(count))
#define vec_srav_i64_4(a, count)                           svasr_s64_z(svptrue_b64(), a, svreinterpret_u64_s64(count))
#define vec_srav_i64_2(a, count)                           svasr_s64_z(svptrue_b64(), a, svreinterpret_u64_s64(count))
#define vec_srav_i64_1(a, count)                           ((count < 64) ? ((int64_t) a)>>count : (((int64_t) a) < 0) ? -1 : 0)


//------------------------------------------------------------------------------------------------------------------------------------------
//- Compare
//------------------------------------------------------------------------------------------------------------------------------------------

#define vec_cmpeq_i64_8(a, b)                              svcmpeq_s64(svptrue_b64(), a, b)
#define vec_cmpeq_i64_4(a, b)                              svcmpeq_s64(svptrue_b64(), a, b)
#define vec_cmpeq_i64_2(a, b)                              svcmpeq_s64(svptrue_b64(), a, b)
#define vec_cmpeq_i64_1(a, b)                              (a == b)

#define vec_cmpgt_i64_8(a, b)                              svcmpgt_s64(svptrue_b64(), a, b)
#define vec_cmpgt_i64_4(a, b)                              svcmpgt_s64(svptrue_b64(), a, b)
#define vec_cmpgt_i64_2(a, b)                              svcmpgt_s64(svptrue_b64(), a, b)
#define vec_cmpgt_i64_1(a, b)                              (a > b)


//------------------------------------------------------------------------------------------------------------------------------------------
//- Shuffle - Permute
//------------------------------------------------------------------------------------------------------------------------------------------

#define vec_permute_i64_8(a, idx)                          svtbl_s64(a, idx)
#define vec_permute_i64_4(a, idx)                          svtbl_s64(a, idx)
#define vec_permute_i64_2(a, idx)                          svtbl_s64(a, idx)
#define vec_permute_i64_1(a, idx)                          a


#endif /* VECTORIZATION_ARM_SVE_I64_H */
