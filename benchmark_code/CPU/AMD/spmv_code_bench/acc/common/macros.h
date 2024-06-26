//
// Created by genshen on 2021/11/12.
//

#ifndef SPMV_ACC_COMMON_MACROS_H
#define SPMV_ACC_COMMON_MACROS_H

#include "../api/types.h"

#define VAR_FROM_CSR_DESC(csr_desc)                                                                                    \
  const int m = csr_desc.rows;                                                                                         \
  const int *rowptr = csr_desc.row_ptr;                                                                                \
  const int *colindex = csr_desc.col_index;                                                                            \
  const double *value = csr_desc.values;

#endif // SPMV_ACC_COMMON_MACROS_H
