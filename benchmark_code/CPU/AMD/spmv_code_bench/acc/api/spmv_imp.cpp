//
// Created by genshen on 2021/11/13.
//

#include <iostream>

#include "spmv.h"
#include "types.h"

void sparse_spmv(int htrans, const int halpha, const int hbeta, int hm, int hn, const int *rowptr, const int *colindex,
                 const double *value, const double *x, double *y) {
  // printf("Warning: this api is deprecated, which may cause crash if UVM (Unified Virtual Memory) is not supported.\n");

  csr_desc<int, double> d_csr_desc(hm, hn, rowptr[hm], rowptr, colindex, value);

  // use device csr arrays as host csr arrays.
  sparse_csr_spmv(htrans, halpha, hbeta, d_csr_desc, d_csr_desc, x, y);
}
