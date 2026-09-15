#ifndef VECTORIZATION_RISCV_SVV_M64_H
#define VECTORIZATION_RISCV_SVV_M64_H

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <arm_sve.h>

#include "macros/cpp_defines.h"
#include "macros/macrolib.h"
#include "bit_ops.h"

#include "vectorization/vectorization_util.h"



typedef int64_t    __attribute__((may_alias))  vec_alias_int64_t;

/* Without the arm_sve_vector_bits attribute, the sve types have many limitations:
 *     - no sizeof().
 *     - can't include in structs or unions.
 *     - can't use pointers to address them.
 * */
#if __ARM_FEATURE_SVE_BITS==512
	typedef int64_t   __attribute__((may_alias))  vec_mask_m64_1_t;
	typedef svbool_t  __attribute__((arm_sve_vector_bits(512)))  vec_mask_m64_2_t;
	typedef svbool_t  __attribute__((arm_sve_vector_bits(512)))  vec_mask_m64_4_t;
	typedef svbool_t  __attribute__((arm_sve_vector_bits(512)))  vec_mask_m64_8_t;
#elif __ARM_FEATURE_SVE_BITS==256
	typedef int64_t   __attribute__((may_alias))  vec_mask_m64_1_t;
	typedef svbool_t  __attribute__((arm_sve_vector_bits(256)))  vec_mask_m64_2_t;
	typedef svbool_t  __attribute__((arm_sve_vector_bits(256)))  vec_mask_m64_4_t;
	typedef svbool_t  __attribute__((arm_sve_vector_bits(256)))  vec_mask_m64_8_t;
#elif __ARM_FEATURE_SVE_BITS==128
	typedef int64_t   __attribute__((may_alias))  vec_mask_m64_1_t;
	typedef svbool_t  __attribute__((arm_sve_vector_bits(128)))  vec_mask_m64_2_t;
	typedef svbool_t  __attribute__((arm_sve_vector_bits(128)))  vec_mask_m64_4_t;
	typedef svbool_t  __attribute__((arm_sve_vector_bits(128)))  vec_mask_m64_8_t;
#else
	#error "__ARM_FEATURE_SVE_BITS not defined"
#endif

typedef uint64_t vec_mask_packed_m64_1_t;
typedef uint64_t vec_mask_packed_m64_2_t;
typedef uint64_t vec_mask_packed_m64_4_t;
typedef uint64_t vec_mask_packed_m64_8_t;
typedef uint64_t vec_mask_packed_m64_16_t;
typedef uint64_t vec_mask_packed_m64_256_t;


//------------------------------------------------------------------------------------------------------------------------------------------
//- Set - Load - Store
//------------------------------------------------------------------------------------------------------------------------------------------

// #define vec_elem_get_m64_8(vec, index)                     ({int64_t _buf[1] = {0}; __riscv_vsm_v_b64((uint8_t *) _buf, vec,   8); _buf[index];})
// #define vec_elem_get_m64_4(vec, index)                     ({int64_t _buf[1] = {0}; __riscv_vsm_v_b64((uint8_t *) _buf, vec,   4); _buf[index];})
// #define vec_elem_get_m64_2(vec, index)                     ({int64_t _buf[1] = {0}; __riscv_vsm_v_b64((uint8_t *) _buf, vec,   2); _buf[index];})
// #define vec_elem_get_m64_1(vec, index)                     vec

// #define vec_elem_set_m64_8(vec, index, expr)               do { int64_t _buf[1]; __riscv_vsm_v_b64((uint8_t *) _buf, vec,   8); _buf[index] = (int64_t) (expr); vec = __riscv_vlm_v_b64((const uint8_t *) _buf,   8); } while (0)
// #define vec_elem_set_m64_4(vec, index, expr)               do { int64_t _buf[1]; __riscv_vsm_v_b64((uint8_t *) _buf, vec,   4); _buf[index] = (int64_t) (expr); vec = __riscv_vlm_v_b64((const uint8_t *) _buf,   4); } while (0)
// #define vec_elem_set_m64_2(vec, index, expr)               do { int64_t _buf[1]; __riscv_vsm_v_b64((uint8_t *) _buf, vec,   2); _buf[index] = (int64_t) (expr); vec = __riscv_vlm_v_b64((const uint8_t *) _buf,   2); } while (0)
// #define vec_elem_set_m64_1(vec, index, expr)               do { vec = (expr); } while (0)

/* sv_bool_t stores 1 bit per vector byte.
 * With 64 bits it encodes a max vector size of 512 bits, i.e., 64 bytes.
 *     1. Get the first 64 bits.
 *     2. For 64-bit elements we want every 8th bit, starting from 0, so 8 bits total.
 */
#define pack_svbool_b64(mask)                                         \
({                                                                    \
	uint64_t __sparse_mask = ((vec_alias_int64_t *) &mask)[0];    \
	(__sparse_mask * 0x0102040810204080ULL) >> 56;                \
})

#define vec_mask_pack_m64_8(a)                             pack_svbool_b64(a)
#define vec_mask_pack_m64_4(a)                             pack_svbool_b64(a)
#define vec_mask_pack_m64_2(a)                             pack_svbool_b64(a)
#define vec_mask_pack_m64_1(a)                             a

#define vec_mask_packed_get_bit_m64_8(a, pos)              bits_u64_extract(a, pos, 1)
#define vec_mask_packed_get_bit_m64_4(a, pos)              bits_u64_extract(a, pos, 1)
#define vec_mask_packed_get_bit_m64_2(a, pos)              bits_u64_extract(a, pos, 1)
#define vec_mask_packed_get_bit_m64_1(a, pos)              bits_u64_extract(a, pos, 1)

#define load_mask_packed_m64(mask)                                                               \
({                                                                                               \
    /* 1. Generate power-of-2 bit weights for 64-bit lanes: [1, 2, 4, 8, 16, ...] */             \
    svuint64_t __bit_weights = svlsl_u64_x(svptrue_b64(), svdup_n_u64(1), svindex_u64(0, 1));    \
    /* 2. Perform bitwise AND between each lane weight and the scalar mask */                    \
    svuint64_t __active_bits = svand_n_u64_x(svptrue_b64(), __bit_weights, mask);                \
    /* 3. Compare non-zero lanes to yield the svbool_t predicate */                              \
    svcmpne_n_u64(svptrue_b64(), __active_bits, 0);                                              \
})

#define vec_mask_loadu_packed_m64_8(ptr)                   load_mask_packed_m64( *((uint64_t *) ptr) )
#define vec_mask_loadu_packed_m64_4(ptr)                   load_mask_packed_m64( *((uint64_t *) ptr) )
#define vec_mask_loadu_packed_m64_2(ptr)                   load_mask_packed_m64( *((uint64_t *) ptr) )
#define vec_mask_loadu_packed_m64_1(ptr)                   ( *((uint64_t *) ptr) )

// #define vec_mask_whilelt_m64_8(i, N)                       __riscv_vsetvl_e64m1(N-i)
// #define vec_mask_whilelt_m64_4(i, N)                       __riscv_vsetvl_e64m1(N-i)
// #define vec_mask_whilelt_m64_2(i, N)                       __riscv_vsetvl_e64m1(N-i)
// #define vec_mask_whilelt_m64_1(i, N)                       ( (uint8_t) ((i < N) ? -1 : 0) )

// #define vec_mask_firstN_m64_8(N)                           vec_mask_m64_8( (uint8_t) ((1ULL << (N)) - 1) )
// #define vec_mask_firstN_m64_4(N)                           vec_mask_m64_4( (uint8_t) ((1ULL << (N)) - 1) )
// #define vec_mask_firstN_m64_2(N)                           ({int64_t _buf[  2] = {0}; _buf[0] = (expr); __riscv_vlm_v_b64((const uint8_t *) _buf,   2);})
// #define vec_mask_firstN_m64_1(N)                           ( (uint8_t) ((1ULL << (N)) - 1) )


//------------------------------------------------------------------------------------------------------------------------------------------
//- Operations
//------------------------------------------------------------------------------------------------------------------------------------------

#define vec_and_m64_8(a, b)                                svand_b_z(svptrue_b64(), a, b)
#define vec_and_m64_4(a, b)                                svand_b_z(svptrue_b64(), a, b)
#define vec_and_m64_2(a, b)                                svand_b_z(svptrue_b64(), a, b)
#define vec_and_m64_1(a, b)                                ( (uint8_t) (a & b) )

#define vec_or_m64_8(a, b)                                 svorr_b_z(svptrue_b64(), a, b)
#define vec_or_m64_4(a, b)                                 svorr_b_z(svptrue_b64(), a, b)
#define vec_or_m64_2(a, b)                                 svorr_b_z(svptrue_b64(), a, b)
#define vec_or_m64_1(a, b)                                 ( (uint8_t) (a | b) )

#define vec_not_m64_8(a)                                   svnot_b_z(svptrue_b64(), a)
#define vec_not_m64_4(a)                                   svnot_b_z(svptrue_b64(), a)
#define vec_not_m64_2(a)                                   svnot_b_z(svptrue_b64(), a)
#define vec_not_m64_1(a)                                   ( (uint8_t) (a ^ -1) )

#define vec_xor_m64_8(a, b)                                sveor_b_z(svptrue_b64(), a, b)
#define vec_xor_m64_4(a, b)                                sveor_b_z(svptrue_b64(), a, b)
#define vec_xor_m64_2(a, b)                                sveor_b_z(svptrue_b64(), a, b)
#define vec_xor_m64_1(a, b)                                ( (uint8_t) (a ^ b) )


#endif /* VECTORIZATION_RISCV_SVV_M64_H */

