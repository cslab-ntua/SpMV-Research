//
// Created by Kazem on 7/12/21.
//

#include "SpMVGenericCode.h"
#include "GenericCodelets.h"

#include <vector>

namespace DDT {
    template<class type>
    bool is_float_equal(const type x, const type y, double absTol,
                        double relTol) {
        return std::abs(x - y) <=
               std::max(absTol, relTol * std::max(std::abs(x), std::abs(y)));
    }
    bool verifySpMV(const int n, const int *Ap, const int *Ai, const double *Ax,
                    const double *x, double *y) {
        // Allocate memory
        auto yy = new double[n]();

        // Perform SpMV
        for (int i = 0; i < n; i++) {
            for (int j = Ap[i]; j < Ap[i + 1]; j++) {
                yy[i] += Ax[j] * x[Ai[j]];
            }
        }
        const double eps = 1e-4;


        // Compare outputs
        bool wrong = false;
        for (int i = 0; i < n; i++) {
            if (!is_float_equal(yy[i], y[i], eps, eps)) {
                std::cout << "Wrong at 'i' = " << i << std::endl;
                std::cout << "(" << yy[i] << "," << y[i] << ")" << std::endl;
                wrong = true;
                exit(1);
            }
        }
        if (wrong) return false;

        // Clean up memory
        delete[] yy;

        return true;
    }



    void spmv_generic(const int n, const int *Ap, const int *Ai,
                      const double *Ax, const double *x, double *y,
                      const std::vector<Codelet *> *lst,
                      const DDT::Config &cfg) {
 // Perform SpMV
        auto a = std::chrono::steady_clock::now();
#pragma omp parallel for num_threads(cfg.nThread)
        for (int i = 0; i < cfg.nThread; i++) {
            for (const auto &c : lst[i]) {
                switch (c->get_type()) {
                    case CodeletType::TYPE_FSC:
                        fsc_t2_2DC(y, Ax, x, c->row_offset, c->first_nnz_loc,
                                   c->lbr, c->lbr + c->row_width, c->lbc,
                                   c->col_width + c->lbc, c->col_offset);
                        break;
                    case CodeletType::TYPE_PSC1:
                        psc_t1_2D4R(y, Ax, x, c->offsets, c->lbr,
                                    c->lbr + c->row_width, c->lbc,
                                    c->lbc + c->col_width);
                        break;
                    case CodeletType::TYPE_PSC2:
                        psc_t2_2DC(y, Ax, x, c->offsets, c->row_offset,
                                   c->first_nnz_loc, c->lbr,
                                   c->lbr + c->row_width, c->col_width,
                                   c->col_offset);
                        break;
                    case CodeletType::TYPE_PSC3:
                        psc_t3_1D1R(y, Ax, Ai, x, c->offsets, c->lbr,
                                    c->first_nnz_loc, c->col_width);
                        break;
                    case CodeletType::TYPE_F_PSC:
                        f_psc_t2(c->col_width, c->first_nnz_loc, c->col_offset,
                                 c->lbr, c->lbr + c->row_width, c->row_offset,
                                 c->offsets, Ax, x, y);
                        break;
                    default:
                        break;
                }
            }
        }
    }
}// namespace DDT
