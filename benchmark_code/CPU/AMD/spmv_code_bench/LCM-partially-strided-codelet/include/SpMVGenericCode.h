//
// Created by kazem on 7/12/21.
//

#ifndef DDT_SPMVGENERICCODE_H
#define DDT_SPMVGENERICCODE_H

#include "Inspector.h"

namespace DDT{
    void spmv_generic(const int n, const int *Ap, const int *Ai, const double
    *Ax, const double *x, double *y, const std::vector<Codelet*>* lst, const DDT::Config& cfg);
}

#endif //DDT_SPMVGENERICCODE_H
