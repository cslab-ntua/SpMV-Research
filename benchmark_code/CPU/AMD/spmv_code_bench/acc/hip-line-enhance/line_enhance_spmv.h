//
// Created by chu genshen on 2021/10/2.
//

#ifndef SPMV_ACC_LINE_ENHANCE_SPMV_H
#define SPMV_ACC_LINE_ENHANCE_SPMV_H

#include "api/types.h"

void line_enhance_sparse_spmv(int trans, const int alpha, const int beta, const csr_desc<int, double> d_csr_desc,
                              const double *x, double *y);

/**
 * set kernel template parameters adaptively by matrix characteristic.
 */
void adaptive_enhance_sparse_spmv(int trans, const int alpha, const int beta, const csr_desc<int, double> d_csr_desc,
                                  const double *x, double *y);

#endif // SPMV_ACC_LINE_ENHANCE_SPMV_H
