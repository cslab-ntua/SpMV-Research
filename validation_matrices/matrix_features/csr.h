#ifndef CSR_H
#define CSR_H


#ifndef CSR_GEN_H_F
#define CSR_GEN_H_F
#include "storage_formats/csr/csr_gen_undef.h"
#define CSR_GEN_TYPE_1  ValueType
#define CSR_GEN_TYPE_2  int
#define CSR_GEN_SUFFIX  _f
#include "storage_formats/csr/csr_gen.h"
#endif /* CSR_GEN_H_F */


#ifndef CSR_UTIL_GEN_H_F
#define CSR_UTIL_GEN_H_F
#include "storage_formats/csr_util/csr_util_gen_undef.h"
#define CSR_UTIL_GEN_TYPE_1  ValueType
#define CSR_UTIL_GEN_TYPE_2  int
#define CSR_UTIL_GEN_SUFFIX  _f
#include "storage_formats/csr_util/csr_util_gen.h"
#endif /* CSR_UTIL_GEN_H_F */


#ifndef CSR_REORDER_GEN_H_F
#define CSR_REORDER_GEN_H_F
#include "storage_formats/csr_reorder/csr_reorder_gen_undef.h"
#define CSR_REORDER_GEN_TYPE_1  ValueType
#define CSR_REORDER_GEN_TYPE_2  int
#define CSR_REORDER_GEN_SUFFIX  _f
#include "storage_formats/csr_reorder/csr_reorder_gen.h"
#endif /* CSR_REORDER_GEN_H_F */


#endif /* CSR_H */

