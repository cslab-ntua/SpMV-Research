#ifndef  VECTORIZATION_GEN_H
#define  VECTORIZATION_GEN_H

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "macros/cpp_defines.h"
#include "macros/macrolib.h"

// #include "vectorization/vectorization_gen_length.h"

#define VEC_EXPAND(fun, ...)  fun(__VA_ARGS__)

#define _VEC_CONCAT(a, b)  a ## b
#define _VEC_CONCAT_EXPAND(a, b)  _VEC_CONCAT(a, b)
#define VEC_CONCAT(a, b)  _VEC_CONCAT_EXPAND(a, b)


#if defined(__x86_64__)
#elif defined(__aarch64__)
#endif


/* We can force specific vector extensions by defining 'VEC_FORCE' and the vector extensions we want.
 */

#ifndef VEC_FORCE
	#undef VEC_X86_512
	#undef VEC_X86_256
	#undef VEC_X86_128
	#undef VEC_ARM_SVE

	#if defined(__AVX512F__)
		/* AVX512 is slow. */
		#define VEC_X86_512
		// #define VEC_X86_256
	#elif defined(__AVX2__)
		#define VEC_X86_256
	#elif defined(__AVX__)
		#define VEC_X86_128
	#elif defined(__ARM_FEATURE_SVE)
		#define VEC_ARM_SVE
	#endif

#endif


#if defined(VEC_X86_512)
	#include "vectorization_x86_avx512_i32.h"
	#include "vectorization_x86_avx512_i64.h"
	#include "vectorization_x86_avx512_f32.h"
	#include "vectorization_x86_avx512_f64.h"
#elif defined(VEC_X86_256)
	#include "vectorization_x86_avx256_i32.h"
	#include "vectorization_x86_avx256_i64.h"
	#include "vectorization_x86_avx256_f32.h"
	#include "vectorization_x86_avx256_f64.h"
#elif defined(VEC_X86_128)
	#include "vectorization_x86_avx128_i32.h"
	#include "vectorization_x86_avx128_i64.h"
	#include "vectorization_x86_avx128_f32.h"
	#include "vectorization_x86_avx128_f64.h"
#elif defined(VEC_ARM_SVE)
	#include "vectorization_arm_sve_i32.h"
	#include "vectorization_arm_sve_i64.h"
	#include "vectorization_arm_sve_f32.h"
	#include "vectorization_arm_sve_f64.h"
#else
#endif


#define VEC_LEN_DEFAULT(suffix)  VEC_CONCAT(vec_len_default_, suffix)

#define VEC_FORM_NAME(name, suffix, vs)  VEC_CONCAT(VEC_CONCAT(VEC_CONCAT(name, suffix), _), DEFAULT_ARG(VEC_LEN_DEFAULT(suffix), vs))

#define VEC_CALL(name, suffix, vs, ...)  VEC_EXPAND(VEC_FORM_NAME(name, suffix, vs), __VA_ARGS__)


//==========================================================================================================================================
//------------------------------------------------------------------------------------------------------------------------------------------
//-                                                              Generics                                                                  -
//------------------------------------------------------------------------------------------------------------------------------------------
//==========================================================================================================================================


#define vec_t(suffix, vs)                            VEC_CONCAT(VEC_FORM_NAME(vec_, suffix, vs), _t)

// #define vec_len(suffix, vs)                          VEC_FORM_NAME(vec_len_, suffix, vs)


#define vec_array(suffix, vs, val)                   VEC_CALL(vec_array_, suffix, vs, val)

#define vec_set1(suffix, vs, val)                    VEC_CALL(vec_set1_, suffix, vs, val)

#define vec_set_iter(suffix, vs, iter, expr)         VEC_CALL(vec_set_iter_, suffix, vs, iter, expr)


#define vec_loadu(suffix, vs, ptr)                   VEC_CALL(vec_loadu_, suffix, vs, ptr)

#define vec_storeu(suffix, vs, ptr, vec)             VEC_CALL(vec_storeu_, suffix, vs, ptr, vec)


#define vec_add(suffix, vs, a, b)                    VEC_CALL(vec_add_, suffix, vs, a, b)

#define vec_sub(suffix, vs, a, b)                    VEC_CALL(vec_sub_, suffix, vs, a, b)

#define vec_mul(suffix, vs, a, b)                    VEC_CALL(vec_mul_, suffix, vs, a, b)

#define vec_div(suffix, vs, a, b)                    VEC_CALL(vec_div_, suffix, vs, a, b)


//==========================================================================================================================================
//------------------------------------------------------------------------------------------------------------------------------------------
//-                                                              Integers                                                                  -
//------------------------------------------------------------------------------------------------------------------------------------------
//==========================================================================================================================================


#define vec_and(suffix, vs, a, b)                    VEC_CALL(vec_and_, suffix, vs, a, b)

#define vec_or(suffix, vs, a, b)                     VEC_CALL(vec_or_, suffix, vs, a, b)

#define vec_slli(suffix, vs, a, imm8)                VEC_CALL(vec_slli_, suffix, vs, a, imm8)
#define vec_srli(suffix, vs, a, imm8)                VEC_CALL(vec_srli_, suffix, vs, a, imm8)
#define vec_srai(suffix, vs, a, imm8)                VEC_CALL(vec_srai_, suffix, vs, a, imm8)

#define vec_sllv(suffix, vs, a, count)               VEC_CALL(vec_sllv_, suffix, vs, a, count)
#define vec_srlv(suffix, vs, a, count)               VEC_CALL(vec_srlv_, suffix, vs, a, count)
#define vec_srav(suffix, vs, a, count)               VEC_CALL(vec_srav_, suffix, vs, a, count)


//==========================================================================================================================================
//------------------------------------------------------------------------------------------------------------------------------------------
//-                                                               Floats                                                                   -
//------------------------------------------------------------------------------------------------------------------------------------------
//==========================================================================================================================================


// Returns a*b + c
#define vec_fmadd(suffix, vs, a, b, c)               VEC_CALL(vec_fmadd_, suffix, vs, a, b, c)

#define vec_reduce_add(suffix, vs, a)                VEC_CALL(vec_reduce_add_, suffix, vs, a)


#endif /*  VECTORIZATION_GEN_H */

