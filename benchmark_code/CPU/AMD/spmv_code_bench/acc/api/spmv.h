//
// Created by genshen on 2021/7/15.
//

#ifndef SPMV_ACC_SPMV_H
#define SPMV_ACC_SPMV_H

#include "building_config.h"
#include "types.h"

/**
 *
 * @param trans Matrix transpose. current only support operation_none.
 * @param alpha alpha and beta in y = alpha*A*x + beta*y
 * @param h_rowptr row offset on host size.
 * @param d_csr_desc csr description.
 * @param dx vector x.
 * @param dy vector y.
 */
void sparse_csr_spmv(int trans, const int alpha, const int beta, const csr_desc<int, double> h_csr_desc,
                     const csr_desc<int, double> d_csr_desc, const double *dx, double *dy);

/**
 * @deprecated This api is only available on ROCm platform.
 * It may not support CUDA platform due to the possible lack of UVM (Unified Virtual Memory) .
 */
void sparse_spmv(int htrans, const int halpha, const int hbeta, int hm, int hn, const int *rowptr, const int *colindex,
                 const double *value, const double *x, double *y);

#endif // SPMV_ACC_SPMV_H
