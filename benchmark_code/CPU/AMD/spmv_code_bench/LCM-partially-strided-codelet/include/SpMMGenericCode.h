//
// Created by cetinicz on 2021-10-30.
//

#ifndef DDT_SPMMGENERICCODE_H
#define DDT_SPMMGENERICCODE_H

#include "DDT.h"
#include "DDTCodelets.h"
#include <vector>

namespace DDT {
    void spmm_generic(const int n, const int *Ap, const int *Ai,
                      const double *Ax, const double *Bx, double *Cx, int cbb, int cbd,
                      const std::vector<Codelet *> *lst,
                      const DDT::Config &cfg);
    void fsc_t2_2DC_gemm(double *Cx, const double *Ax, const double *Bx,
                         const int axi, const int axo, const int lb,
                         const int ub, const int cbl, const int cbu,
                         const int co, const int cbb, const int cbd);

    void psc_t1_2D4R_gemm(double *Cx, const double *Ax, const double *Bx,
                         const int *offset, int lb, int ub, int lbc, int ubc,
                         const int cbb, const int cbd);

    void psc_t2_2DC_gemm(double *Cx, const double *Ax, const double *Bx,
                         const int *offset, const int axi, const int axo,
                         const int lb, const int ub, const int cb,
                         const int cof, const int cbb, const int cbd);

    void psc_t3_1D1R_gemm(double *Cx, const double *Ax, const int *Ai,
                          const double *Bx, const int *offset, int lb, int fnl,
                          int cw, const int cbb, const int cbd);
}
#endif //DDT_SPMMGENERICCODE_H
