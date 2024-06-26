//
// Created by genshen on 2021/4/15.
//

#ifndef SPMV_ACC_CSR_SPARSE_H
#define SPMV_ACC_CSR_SPARSE_H

#include "acc/api/types.h"
#include "acc/api/spmv.h"
#include "building_config.h"

#ifndef ACCELERATE_ENABLED
// create an empty definition `sparse_spmv`, if HIP is not enabled.
// just to make compiling passed.
void sparse_spmv(int htrans, const int halpha, const int hbeta, int hm, int hn, const int *hrowptr,
                 const int *hcolindex, const double *hvalue, const double *hx, double *hy) {}
#endif // ACCELERATE_ENABLED

#endif // SPMV_ACC_CSR_SPARSE_H
