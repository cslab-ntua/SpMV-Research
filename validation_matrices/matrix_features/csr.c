
#include "storage_formats/csr/csr_gen_undef.h"
#define CSR_GEN_TYPE_1  ValueType
#define CSR_GEN_TYPE_2  int
#define CSR_GEN_SUFFIX  _f
#include "storage_formats/csr/csr_gen.c"

#include "storage_formats/csr_util/csr_util_gen_undef.h"
#define CSR_UTIL_GEN_TYPE_1  ValueType
#define CSR_UTIL_GEN_TYPE_2  int
#define CSR_UTIL_GEN_SUFFIX  _f
#include "storage_formats/csr_util/csr_util_gen.c"
