#ifndef VECTORIZATION_RISCV_SVV_M64_H
#define VECTORIZATION_RISCV_SVV_M64_H

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <riscv_vector.h>

#include "macros/cpp_defines.h"
#include "macros/macrolib.h"
#include "bit_ops.h"

#include "vectorization/vectorization_util.h"


typedef int64_t   __attribute__((may_alias))  vec_mask_m64_1_t;
typedef vbool64_t __attribute__((may_alias))  vec_mask_m64_2_t;
typedef vbool64_t __attribute__((may_alias))  vec_mask_m64_4_t;
typedef vbool64_t __attribute__((may_alias))  vec_mask_m64_8_t;
typedef vbool64_t __attribute__((may_alias))  vec_mask_m64_16_t;
typedef vbool64_t __attribute__((may_alias))  vec_mask_m64_256_t;

typedef uint64_t vec_mask_packed_m64_1_t;
typedef uint64_t vec_mask_packed_m64_2_t;
typedef uint64_t vec_mask_packed_m64_4_t;
typedef uint64_t vec_mask_packed_m64_8_t;
typedef uint64_t vec_mask_packed_m64_16_t;
typedef uint64_t vec_mask_packed_m64_256_t;


//------------------------------------------------------------------------------------------------------------------------------------------
//- Set - Load - Store
//------------------------------------------------------------------------------------------------------------------------------------------

// #define vec_elem_get_m64_256(vec, index)                   ({int64_t _buf[4] = {0}; __riscv_vsm_v_b64((uint8_t *) _buf, vec, 256); _buf[index];})
// #define vec_elem_get_m64_16(vec, index)                    ({int64_t _buf[1] = {0}; __riscv_vsm_v_b64((uint8_t *) _buf, vec,  16); _buf[index];})
// #define vec_elem_get_m64_8(vec, index)                     ({int64_t _buf[1] = {0}; __riscv_vsm_v_b64((uint8_t *) _buf, vec,   8); _buf[index];})
// #define vec_elem_get_m64_4(vec, index)                     ({int64_t _buf[1] = {0}; __riscv_vsm_v_b64((uint8_t *) _buf, vec,   4); _buf[index];})
// #define vec_elem_get_m64_2(vec, index)                     ({int64_t _buf[1] = {0}; __riscv_vsm_v_b64((uint8_t *) _buf, vec,   2); _buf[index];})
// #define vec_elem_get_m64_1(vec, index)                     vec

// #define vec_elem_set_m64_256(vec, index, expr)             do { int64_t _buf[4]; __riscv_vsm_v_b64((uint8_t *) _buf, vec, 256); _buf[index] = (int64_t) (expr); vec = __riscv_vlm_v_b64((const uint8_t *) _buf, 256); } while (0)
// #define vec_elem_set_m64_16(vec, index, expr)              do { int64_t _buf[1]; __riscv_vsm_v_b64((uint8_t *) _buf, vec,  16); _buf[index] = (int64_t) (expr); vec = __riscv_vlm_v_b64((const uint8_t *) _buf,  16); } while (0)
// #define vec_elem_set_m64_8(vec, index, expr)               do { int64_t _buf[1]; __riscv_vsm_v_b64((uint8_t *) _buf, vec,   8); _buf[index] = (int64_t) (expr); vec = __riscv_vlm_v_b64((const uint8_t *) _buf,   8); } while (0)
// #define vec_elem_set_m64_4(vec, index, expr)               do { int64_t _buf[1]; __riscv_vsm_v_b64((uint8_t *) _buf, vec,   4); _buf[index] = (int64_t) (expr); vec = __riscv_vlm_v_b64((const uint8_t *) _buf,   4); } while (0)
// #define vec_elem_set_m64_2(vec, index, expr)               do { int64_t _buf[1]; __riscv_vsm_v_b64((uint8_t *) _buf, vec,   2); _buf[index] = (int64_t) (expr); vec = __riscv_vlm_v_b64((const uint8_t *) _buf,   2); } while (0)
// #define vec_elem_set_m64_1(vec, index, expr)               do { vec = (expr); } while (0)

#define vec_mask_pack_m64_256(a)                           ({int64_t _buf[4]; __riscv_vsm_v_b64((uint8_t *) _buf, a, 256); _buf[0];})
#define vec_mask_pack_m64_16(a)                            ({int64_t _buf[1]; __riscv_vsm_v_b64((uint8_t *) _buf, a,  16); _buf[0];})
#define vec_mask_pack_m64_8(a)                             ({int64_t _buf[1]; __riscv_vsm_v_b64((uint8_t *) _buf, a,   8); _buf[0];})
#define vec_mask_pack_m64_4(a)                             ({int64_t _buf[1]; __riscv_vsm_v_b64((uint8_t *) _buf, a,   4); _buf[0];})
#define vec_mask_pack_m64_2(a)                             ({int64_t _buf[1]; __riscv_vsm_v_b64((uint8_t *) _buf, a,   2); _buf[0];})
#define vec_mask_pack_m64_1(a)                             a

#define vec_mask_packed_get_bit_m64_256(a, pos)            bits_u64_extract(a, pos, 1)
#define vec_mask_packed_get_bit_m64_16(a, pos)             bits_u64_extract(a, pos, 1)
#define vec_mask_packed_get_bit_m64_8(a, pos)              bits_u64_extract(a, pos, 1)
#define vec_mask_packed_get_bit_m64_4(a, pos)              bits_u64_extract(a, pos, 1)
#define vec_mask_packed_get_bit_m64_2(a, pos)              bits_u64_extract(a, pos, 1)
#define vec_mask_packed_get_bit_m64_1(a, pos)              bits_u64_extract(a, pos, 1)

#define vec_mask_loadu_packed_m64_256(ptr)                 __riscv_vlm_v_b64((const uint8_t *) ptr, 256)
#define vec_mask_loadu_packed_m64_16(ptr)                  __riscv_vlm_v_b64((const uint8_t *) ptr,  16)
#define vec_mask_loadu_packed_m64_8(ptr)                   __riscv_vlm_v_b64((const uint8_t *) ptr,   8)
#define vec_mask_loadu_packed_m64_4(ptr)                   __riscv_vlm_v_b64((const uint8_t *) ptr,   4)
#define vec_mask_loadu_packed_m64_2(ptr)                   __riscv_vlm_v_b64((const uint8_t *) ptr,   2)
#define vec_mask_loadu_packed_m64_1(ptr)                   ( *((uint64_t *) ptr) )

#define vec_mask_whilelt_m64_256(i, N)                     __riscv_vsetvl_e64m1(N-i)
#define vec_mask_whilelt_m64_16(i, N)                      __riscv_vsetvl_e64m1(N-i)
#define vec_mask_whilelt_m64_8(i, N)                       __riscv_vsetvl_e64m1(N-i)
#define vec_mask_whilelt_m64_4(i, N)                       __riscv_vsetvl_e64m1(N-i)
#define vec_mask_whilelt_m64_2(i, N)                       __riscv_vsetvl_e64m1(N-i)
#define vec_mask_whilelt_m64_1(i, N)                       ( (uint8_t) ((i < N) ? -1 : 0) )

#define vec_mask_firstN_m64_256(N)                         vec_loop_expr(vec_mask_m64_256_t, 2, _tmp, _i, _tmp[_i] = (uint8_t) (((1ULL << (N)) - 1) >> 8*_i);)
#define vec_mask_firstN_m64_16(N)                          vec_loop_expr(vec_mask_m64_16_t,  2, _tmp, _i, _tmp[_i] = (uint8_t) (((1ULL << (N)) - 1) >> 8*_i);)
#define vec_mask_firstN_m64_8(N)                           vec_mask_m64_8( (uint8_t) ((1ULL << (N)) - 1) )
#define vec_mask_firstN_m64_4(N)                           vec_mask_m64_4( (uint8_t) ((1ULL << (N)) - 1) )
#define vec_mask_firstN_m64_2(N)                           ({int64_t _buf[  2] = {0}; _buf[0] = (expr); __riscv_vlm_v_b64((const uint8_t *) _buf,   2);})
#define vec_mask_firstN_m64_1(N)                           ( (uint8_t) ((1ULL << (N)) - 1) )


//------------------------------------------------------------------------------------------------------------------------------------------
//- Operations
//------------------------------------------------------------------------------------------------------------------------------------------

#define vec_and_m64_256(a, b)                              __riscv_vmand_mm_b64(a, b, 256)
#define vec_and_m64_16(a, b)                               __riscv_vmand_mm_b64(a, b,  16)
#define vec_and_m64_8(a, b)                                __riscv_vmand_mm_b64(a, b,   8)
#define vec_and_m64_4(a, b)                                __riscv_vmand_mm_b64(a, b,   4)
#define vec_and_m64_2(a, b)                                __riscv_vmand_mm_b64(a, b,   2)
#define vec_and_m64_1(a, b)                                ( (uint8_t) (a & b) )

#define vec_or_m64_256(a, b)                               __riscv_vmor_mm_b64(a, b, 256)
#define vec_or_m64_16(a, b)                                __riscv_vmor_mm_b64(a, b,  16)
#define vec_or_m64_8(a, b)                                 __riscv_vmor_mm_b64(a, b,   8)
#define vec_or_m64_4(a, b)                                 __riscv_vmor_mm_b64(a, b,   4)
#define vec_or_m64_2(a, b)                                 __riscv_vmor_mm_b64(a, b,   2)
#define vec_or_m64_1(a, b)                                 ( (uint8_t) (a | b) )

#define vec_not_m64_256(a)                                 __riscv_vmnot_m_b64(a, 256)
#define vec_not_m64_16(a)                                  __riscv_vmnot_m_b64(a,  16)
#define vec_not_m64_8(a)                                   __riscv_vmnot_m_b64(a,   8)
#define vec_not_m64_4(a)                                   __riscv_vmnot_m_b64(a,   4)
#define vec_not_m64_2(a)                                   __riscv_vmnot_m_b64(a,   2)
#define vec_not_m64_1(a)                                   ( (uint8_t) (a ^ 1) )

#define vec_xor_m64_256(a, b)                              __riscv_vmxor_mm_b64(a, b, 256)
#define vec_xor_m64_16(a, b)                               __riscv_vmxor_mm_b64(a, b,  16)
#define vec_xor_m64_8(a, b)                                __riscv_vmxor_mm_b64(a, b,   8)
#define vec_xor_m64_4(a, b)                                __riscv_vmxor_mm_b64(a, b,   4)
#define vec_xor_m64_2(a, b)                                __riscv_vmxor_mm_b64(a, b,   2)
#define vec_xor_m64_1(a, b)                                ( (uint8_t) (a ^ b) )


#endif /* VECTORIZATION_RISCV_SVV_M64_H */

