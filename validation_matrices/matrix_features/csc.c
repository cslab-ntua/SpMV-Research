
#include "storage_formats/csc/csc_gen_undef.h"
#define CSC_GEN_TYPE_1  ValueType
#define CSC_GEN_TYPE_2  int
#define CSC_GEN_SUFFIX  _f
#include "storage_formats/csc/csc_gen.c"

#include "storage_formats/csc_util/csc_util_gen_undef.h"
#define CSC_UTIL_GEN_TYPE_1  ValueType
#define CSC_UTIL_GEN_TYPE_2  int
#define CSC_UTIL_GEN_SUFFIX  _f
#include "storage_formats/csc_util/csc_util_gen.c"

#include "storage_formats/csc_reorder/csc_reorder_gen_undef.h"
#define CSC_REORDER_GEN_TYPE_1  ValueType
#define CSC_REORDER_GEN_TYPE_2  int
#define CSC_REORDER_GEN_SUFFIX  _f
#include "storage_formats/csc_reorder/csc_reorder_gen.c"
