//
// Created by cetinicz on 2021-07-17.
//

#ifndef DDT_GENERICCODELETSSPTRSV_H
#define DDT_GENERICCODELETSSPTRSV_H

#include "Inspector.h"

namespace DDT {
    void sptrsv_generic(const int n, const int* Lp, const int* Li, const double *Ax, double *x,
                        const std::vector<DDT::Codelet *>& lst,
                        const DDT::Config &cfg);
}

#endif//DDT_GENERICCODELETSSPTRSV_H
