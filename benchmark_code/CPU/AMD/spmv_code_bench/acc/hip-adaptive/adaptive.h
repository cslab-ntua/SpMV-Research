//
// Created by genshen on 2021/7/28.
//

#ifndef SPMV_ACC_ADAPTIVE_H
#define SPMV_ACC_ADAPTIVE_H

#include "api/types.h"

void adaptive_sparse_spmv(int trans, const int alpha, const int beta, const csr_desc<int, double> h_csr_desc,
                          const csr_desc<int, double> d_csr_desc, const double *x, double *y);

#endif // SPMV_ACC_ADAPTIVE_H
