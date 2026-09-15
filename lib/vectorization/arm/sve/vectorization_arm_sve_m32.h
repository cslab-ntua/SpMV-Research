#ifndef VECTORIZATION_RISCV_SVV_M32_H
#define VECTORIZATION_RISCV_SVV_M32_H

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <arm_sve.h>

#include "macros/cpp_defines.h"
#include "macros/macrolib.h"
#include "bit_ops.h"

#include "vectorization/vectorization_util.h"


typedef int32_t    __attribute__((may_alias))  vec_alias_int32_t;

/* Without the arm_sve_vector_bits attribute, the sve types have many limitations:
 *     - no sizeof().
 *     - can't include in structs or unions.
 *     - can't use pointers to address them.
 * */
#if __ARM_FEATURE_SVE_BITS==512
	typedef int32_t   __attribute__((may_alias))  vec_mask_m32_1_t;
	typedef svbool_t  __attribute__((arm_sve_vector_bits(512)))  vec_mask_m32_2_t;
	typedef svbool_t  __attribute__((arm_sve_vector_bits(512)))  vec_mask_m32_4_t;
	typedef svbool_t  __attribute__((arm_sve_vector_bits(512)))  vec_mask_m32_8_t;
	typedef svbool_t  __attribute__((arm_sve_vector_bits(512)))  vec_mask_m32_16_t;
#elif __ARM_FEATURE_SVE_BITS==256
	typedef int32_t   __attribute__((may_alias))  vec_mask_m32_1_t;
	typedef svbool_t  __attribute__((arm_sve_vector_bits(256)))  vec_mask_m32_2_t;
	typedef svbool_t  __attribute__((arm_sve_vector_bits(256)))  vec_mask_m32_4_t;
	typedef svbool_t  __attribute__((arm_sve_vector_bits(256)))  vec_mask_m32_8_t;
	typedef svbool_t  __attribute__((arm_sve_vector_bits(256)))  vec_mask_m32_16_t;
#elif __ARM_FEATURE_SVE_BITS==128
	typedef int32_t   __attribute__((may_alias))  vec_mask_m32_1_t;
	typedef svbool_t  __attribute__((arm_sve_vector_bits(128)))  vec_mask_m32_2_t;
	typedef svbool_t  __attribute__((arm_sve_vector_bits(128)))  vec_mask_m32_4_t;
	typedef svbool_t  __attribute__((arm_sve_vector_bits(128)))  vec_mask_m32_8_t;
	typedef svbool_t  __attribute__((arm_sve_vector_bits(128)))  vec_mask_m32_16_t;
#else
	#error "__ARM_FEATURE_SVE_BITS not defined"
#endif

typedef uint32_t vec_mask_packed_m32_1_t;
typedef uint32_t vec_mask_packed_m32_2_t;
typedef uint32_t vec_mask_packed_m32_4_t;
typedef uint32_t vec_mask_packed_m32_8_t;
typedef uint32_t vec_mask_packed_m32_16_t;


//------------------------------------------------------------------------------------------------------------------------------------------
//- Set - Load - Store
//------------------------------------------------------------------------------------------------------------------------------------------

// #define vec_elem_get_m32_16(vec, index)                    ({int32_t _buf[ 1] = {0}; __riscv_vsm_v_b32((uint8_t *) _buf, vec,  16); _buf[index];})
// #define vec_elem_get_m32_8(vec, index)                     ({int32_t _buf[ 1] = {0}; __riscv_vsm_v_b32((uint8_t *) _buf, vec,   8); _buf[index];})
// #define vec_elem_get_m32_4(vec, index)                     ({int32_t _buf[ 1] = {0}; __riscv_vsm_v_b32((uint8_t *) _buf, vec,   4); _buf[index];})
// #define vec_elem_get_m32_1(vec, index)                     vec

// #define vec_elem_set_m32_16(vec, index, expr)              do { int32_t _buf[ 1]; __riscv_vsm_v_b32((uint8_t *) _buf, vec,  16); _buf[index] = (int32_t) (expr); vec = __riscv_vlm_v_b32((const uint8_t *) _buf,  16); } while (0)
// #define vec_elem_set_m32_8(vec, index, expr)               do { int32_t _buf[ 1]; __riscv_vsm_v_b32((uint8_t *) _buf, vec,   8); _buf[index] = (int32_t) (expr); vec = __riscv_vlm_v_b32((const uint8_t *) _buf,   8); } while (0)
// #define vec_elem_set_m32_4(vec, index, expr)               do { int32_t _buf[ 1]; __riscv_vsm_v_b32((uint8_t *) _buf, vec,   4); _buf[index] = (int32_t) (expr); vec = __riscv_vlm_v_b32((const uint8_t *) _buf,   4); } while (0)
// #define vec_elem_set_m32_1(vec, index, expr)               do { vec = (expr); } while (0)

#if 0
#define pack_svbool_b32(mask)                                                                      \
({                                                                                                 \
	/* Since it is a predicate for 32-bit elements, only 1 out of every 4 bits are used. */    \
	/* Convert predicate directly to a vector of byte flags. */                                \
	/* Now only 1 out of 4 bytes are used. */                                                  \
	svuint8_t __byte_flags = svdup_n_u8_z(mask, 1);                                            \
	/* We compact by keeping only one every 4 bytes, i.e., bytes 0, 4, 8, 16, ... */           \
	__byte_flags = svcompact_u8(svptrue_b32(), __byte_flags);                                  \
	uint64_t __sparse_mask_0 = ((vec_alias_int64_t *) &__byte_flags)[0];                       \
	uint64_t __sparse_mask_1 = ((vec_alias_int64_t *) &__byte_flags)[1];                       \
	uint64_t __packed_0 = (__sparse_mask_0 * 0x0102040810204080ULL) >> 56;                     \
	uint64_t __packed_1 = (__sparse_mask_1 * 0x0102040810204080ULL) >> 56;                     \
	(uint32_t) ((__packed_1 << 8ULL) | __packed_0);                                            \
})
#endif

#define pack_svbool_b32(mask)                                                                    \
({                                                                                               \
    /* 1. Create power-of-2 bit weights for each lane: [1, 2, 4, 8, 16, 32, ...] */              \
    svuint32_t __bit_weights = svlsl_u32_x(svptrue_b32(), svdup_n_u32(1), svindex_u32(0, 1));    \
    /* 2. Horizontally sum only the active lanes directly into a scalar integer */               \
    svaddv_u32(mask, __bit_weights);                                                             \
})

#define vec_mask_pack_m32_16(a)                            pack_svbool_b32(a)
#define vec_mask_pack_m32_8(a)                             pack_svbool_b32(a)
#define vec_mask_pack_m32_4(a)                             pack_svbool_b32(a)
#define vec_mask_pack_m32_2(a)                             pack_svbool_b32(a)
#define vec_mask_pack_m32_1(a)                             a

#define vec_mask_packed_get_bit_m32_16(a, pos)             bits_u32_extract(a, pos, 1)
#define vec_mask_packed_get_bit_m32_8(a, pos)              bits_u32_extract(a, pos, 1)
#define vec_mask_packed_get_bit_m32_4(a, pos)              bits_u32_extract(a, pos, 1)
#define vec_mask_packed_get_bit_m32_2(a, pos)              bits_u32_extract(a, pos, 1)
#define vec_mask_packed_get_bit_m32_1(a, pos)              bits_u32_extract(a, pos, 1)

#define load_mask_packed_m32(mask)                                                               \
({                                                                                               \
    /* 1. Generate power-of-2 bit weights for 32-bit lanes: [1, 2, 4, 8, 16, ...] */             \
    svuint32_t __bit_weights = svlsl_u32_x(svptrue_b32(), svdup_n_u32(1), svindex_u32(0, 1));    \
    /* 2. Perform bitwise AND between each lane weight and the scalar mask */                    \
    svuint32_t __active_bits = svand_n_u32_x(svptrue_b32(), __bit_weights, mask);                \
    /* 3. Compare non-zero lanes to yield the svbool_t predicate */                              \
    svcmpne_n_u32(svptrue_b32(), __active_bits, 0);                                              \
})

#define vec_mask_loadu_packed_m32_16(ptr)                  load_mask_packed_m32( *((uint32_t *) ptr) )
#define vec_mask_loadu_packed_m32_8(ptr)                   load_mask_packed_m32( *((uint32_t *) ptr) )
#define vec_mask_loadu_packed_m32_4(ptr)                   load_mask_packed_m32( *((uint32_t *) ptr) )
#define vec_mask_loadu_packed_m32_2(ptr)                   load_mask_packed_m32( *((uint32_t *) ptr) )
#define vec_mask_loadu_packed_m32_1(ptr)                   ( *((uint32_t *) ptr) )

// #define vec_mask_whilelt_m32_16(i, N)                      __riscv_whilelt(i, N,  16)
// #define vec_mask_whilelt_m32_8(i, N)                       __riscv_whilelt(i, N,   8)
// #define vec_mask_whilelt_m32_4(i, N)                       __riscv_whilelt(i, N,   4)
// #define vec_mask_whilelt_m32_2(i, N)                       __riscv_whilelt(i, N,   2)
// #define vec_mask_whilelt_m32_1(i, N)                       ( (uint8_t) ((i < N) ? -1 : 0) )

// #define vec_mask_firstN_m32_16(N)                          vec_mask_m32_16( (uint16_t) ((1ULL << (N)) - 1) )
// #define vec_mask_firstN_m32_8(N)                           vec_mask_m32_8( (uint8_t) ((1ULL << (N)) - 1) )
// #define vec_mask_firstN_m32_4(N)                           vec_mask_m32_4( (uint8_t) ((1ULL << (N)) - 1) )
// #define vec_mask_firstN_m32_2(N)                           vec_mask_m32_2( (uint8_t) ((1ULL << (N)) - 1) )
// #define vec_mask_firstN_m32_1(N)                           ( (uint8_t) ((1ULL << (N)) - 1) )


//------------------------------------------------------------------------------------------------------------------------------------------
//- Operations
//------------------------------------------------------------------------------------------------------------------------------------------

#define vec_and_m32_16(a, b)                               svand_b_z(svptrue_b32(), a, b)
#define vec_and_m32_8(a, b)                                svand_b_z(svptrue_b32(), a, b)
#define vec_and_m32_4(a, b)                                svand_b_z(svptrue_b32(), a, b)
#define vec_and_m32_2(a, b)                                svand_b_z(svptrue_b32(), a, b)
#define vec_and_m32_1(a, b)                                ( (uint8_t) (a & b) )

#define vec_or_m32_16(a, b)                                svorr_b_z(svptrue_b32(), a, b)
#define vec_or_m32_8(a, b)                                 svorr_b_z(svptrue_b32(), a, b)
#define vec_or_m32_4(a, b)                                 svorr_b_z(svptrue_b32(), a, b)
#define vec_or_m32_2(a, b)                                 svorr_b_z(svptrue_b32(), a, b)
#define vec_or_m32_1(a, b)                                 ( (uint8_t) (a | b) )

#define vec_not_m32_16(a)                                  svnot_b_z(svptrue_b32(), a)
#define vec_not_m32_8(a)                                   svnot_b_z(svptrue_b32(), a)
#define vec_not_m32_4(a)                                   svnot_b_z(svptrue_b32(), a)
#define vec_not_m32_2(a)                                   svnot_b_z(svptrue_b32(), a)
#define vec_not_m32_1(a)                                   ( (uint8_t) (a ^ -1) )

#define vec_xor_m32_16(a, b)                               sveor_b_z(svptrue_b32(), a, b)
#define vec_xor_m32_8(a, b)                                sveor_b_z(svptrue_b32(), a, b)
#define vec_xor_m32_4(a, b)                                sveor_b_z(svptrue_b32(), a, b)
#define vec_xor_m32_2(a, b)                                sveor_b_z(svptrue_b32(), a, b)
#define vec_xor_m32_1(a, b)                                ( (uint8_t) (a ^ b) )


#endif /* VECTORIZATION_RISCV_SVV_M32_H */

