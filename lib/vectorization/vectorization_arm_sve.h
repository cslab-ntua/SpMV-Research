#ifndef VECTORIZATION_ARM_SVE_H
#define VECTORIZATION_ARM_SVE_H

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <arm_sve.h>

#include "macros/cpp_defines.h"
#include "macros/macrolib.h"


#define vec_i32_sve_t  svint32_t
#define vec_i64_sve_t  svint64_t
#define vec_f32_sve_t  svfloat32_t
#define vec_f64_sve_t  svfloat64_t

#define vec_len_i32_sve  ((long) svlen_s32(*((svint32_t *) NULL)))
#define vec_len_i64_sve  ((long) svlen_s64(*((svint64_t *) NULL)))
#define vec_len_f32_sve  ((long) svlen_f32(*((svfloat32_t *) NULL)))
#define vec_len_f64_sve  ((long) svlen_f64(*((svfloat64_t *) NULL)))


//==========================================================================================================================================
//------------------------------------------------------------------------------------------------------------------------------------------
//-                                                              Integers                                                                  -
//------------------------------------------------------------------------------------------------------------------------------------------
//==========================================================================================================================================


#define vec_set1_i32_sve(val)                    svdup_n_s32(val)
#define vec_set1_i64_sve(val)                    svdup_n_s64(val)

#define vec_set_iter_i32_sve(iter, expr)         ({int32_t buf_a[vec_len_i32_sve]; for (long iter=0;iter<vec_len_i32_sve;iter++) buf_a[iter] = expr; svld1_s32(svptrue_b32(), ((int32_t*)(&buf_a)));})
#define vec_set_iter_i64_sve(iter, expr)         ({int64_t buf_a[vec_len_i64_sve]; for (long iter=0;iter<vec_len_i64_sve;iter++) buf_a[iter] = expr; svld1_s64(svptrue_b64(), ((int64_t*)(&buf_a)));})


#define vec_loadu_i32_sve(ptr)                   svld1_s32(svptrue_b32(), ((int32_t*)(ptr)))
#define vec_loadu_i64_sve(ptr)                   svld1_s64(svptrue_b64(), ((int64_t*)(ptr)))

#define vec_storeu_i32_sve(ptr, vec)             svst1_s32(svptrue_b32(), ((int32_t*)(ptr)), vec)
#define vec_storeu_i64_sve(ptr, vec)             svst1_s64(svptrue_b64(), ((int64_t*)(ptr)), vec)


#define vec_add_i32_sve(a, b)                    svadd_s32_z(svptrue_b32(), a, b)
#define vec_add_i64_sve(a, b)                    svadd_s64_z(svptrue_b64(), a, b)

#define vec_sub_i32_sve(a, b)                    svsub_s32_z(svptrue_b32(), a, b)
#define vec_sub_i64_sve(a, b)                    svsub_s64_z(svptrue_b64(), a, b)

#define vec_mul_i32_sve(a, b)                    svmul_s32_z(svptrue_b32(), a, b)
#define vec_mul_i64_sve(a, b)                    svmul_s64_z(svptrue_b64(), a, b)

#define vec_div_i32_sve(a, b)                    svdiv_s32_z(svptrue_b32(), a, b)
#define vec_div_i64_sve(a, b)                    svdiv_s64_z(svptrue_b64(), a, b)


#define vec_and_i32_sve(a, b)                    svand_s32_z(svptrue_b32(), a, b)
#define vec_and_i64_sve(a, b)                    svand_s64_z(svptrue_b64(), a, b)

#define vec_or_i32_sve(a, b)                     svorr_s32_z(svptrue_b32(), a, b)
#define vec_or_i64_sve(a, b)                     svorr_s64_z(svptrue_b64(), a, b)

#define vec_slli_i32_sve(a, imm8)                                      svlsl_n_s32_z(svptrue_b32(),                        a, imm8)
#define vec_slli_i64_sve(a, imm8)                                      svlsl_n_s64_z(svptrue_b64(),                        a, imm8)
#define vec_srli_i32_sve(a, imm8)                svreinterpret_s32_u32(svlsr_n_u32_z(svptrue_b32(), svreinterpret_u32_s32(a), imm8))
#define vec_srli_i64_sve(a, imm8)                svreinterpret_s64_u64(svlsr_n_u64_z(svptrue_b64(), svreinterpret_u64_s64(a), imm8))
#define vec_srai_i32_sve(a, imm8)                                      svasr_n_s32_z(svptrue_b32(),                        a, imm8)
#define vec_srai_i64_sve(a, imm8)                                      svasr_n_s64_z(svptrue_b64(),                        a, imm8)

#define vec_sllv_i32_sve(a, count)                                     svlsl_s32_z(svptrue_b32(),                        a, svreinterpret_u32_s32(count))
#define vec_sllv_i64_sve(a, count)                                     svlsl_s64_z(svptrue_b64(),                        a, svreinterpret_u64_s64(count))
#define vec_srlv_i32_sve(a, count)               svreinterpret_s32_u32(svlsr_u32_z(svptrue_b32(), svreinterpret_u32_s32(a), svreinterpret_u32_s32(count)))
#define vec_srlv_i64_sve(a, count)               svreinterpret_s64_u64(svlsr_u64_z(svptrue_b64(), svreinterpret_u64_s64(a), svreinterpret_u64_s64(count)))
#define vec_srav_i32_sve(a, count)                                     svasr_s32_z(svptrue_b32(),                        a, svreinterpret_u32_s32(count))
#define vec_srav_i64_sve(a, count)                                     svasr_s64_z(svptrue_b64(),                        a, svreinterpret_u64_s64(count))


//==========================================================================================================================================
//------------------------------------------------------------------------------------------------------------------------------------------
//-                                                               Floats                                                                   -
//------------------------------------------------------------------------------------------------------------------------------------------
//==========================================================================================================================================


#define vec_set1_f32_sve(val)                    svdup_n_f32(val)
#define vec_set1_f64_sve(val)                    svdup_n_f64(val)

#define vec_set_iter_f32_sve(iter, expr)         ({float buf_a[vec_len_f32_sve]; for (long iter=0;iter<vec_len_f32_sve;iter++) buf_a[iter] = expr; svld1_f32(svptrue_b32(), ((float*)(&buf_a)));})
#define vec_set_iter_f64_sve(iter, expr)         ({double buf_a[vec_len_f64_sve]; for (long iter=0;iter<vec_len_f64_sve;iter++) buf_a[iter] = expr; svld1_f64(svptrue_b64(), ((double*)(&buf_a)));})


#define vec_loadu_f32_sve(ptr)                   svld1_f32(svptrue_b32(), ((float*)(ptr)))
#define vec_loadu_f64_sve(ptr)                   svld1_f64(svptrue_b64(), ((double*)(ptr)))

#define vec_storeu_f32_sve(ptr, vec)             svst1_f32(svptrue_b32(), ((float*)(ptr)), vec)
#define vec_storeu_f64_sve(ptr, vec)             svst1_f64(svptrue_b64(), ((double*)(ptr)), vec)


#define vec_add_f32_sve(a, b)                    svadd_f32_z(svptrue_b32(), a, b)
#define vec_add_f64_sve(a, b)                    svadd_f64_z(svptrue_b64(), a, b)

#define vec_sub_f32_sve(a, b)                    svsub_f32_z(svptrue_b32(), a, b)
#define vec_sub_f64_sve(a, b)                    svsub_f64_z(svptrue_b64(), a, b)

#define vec_mul_f32_sve(a, b)                    svmul_f32_z(svptrue_b32(), a, b)
#define vec_mul_f64_sve(a, b)                    svmul_f64_z(svptrue_b64(), a, b)

#define vec_div_f32_sve(a, b)                    svdiv_f32_z(svptrue_b32(), a, b)
#define vec_div_f64_sve(a, b)                    svdiv_f64_z(svptrue_b64(), a, b)

// Returns a*b + c
#define vec_fmadd_f32_sve(a, b, c)               svmad_f32_z(svptrue_b32(), a, b, c)
#define vec_fmadd_f64_sve(a, b, c)               svmad_f64_z(svptrue_b64(), a, b, c)



#define vec_reduce_add_f32_sve(a)                svaddv_f32(svptrue_b64(), a)
#define vec_reduce_add_f64_sve(a)                svaddv_f64(svptrue_b64(), a)


#endif /* VECTORIZATION_ARM_SVE_H */
