//
// Created by cetinicz on 2021-10-30.
//

#include "SpMMGenericCode.h"
#include <vector>

#define loadbx(of, x, ind, xv, msk, cbd) \
xv = _mm256_set_pd(x[of[ind+3]+cbd], x[of[ind+2]+cbd], x[of[ind+1]+cbd], x[of[ind]+cbd]);

namespace DDT {
    inline double hsum_double_avx(__m256d v) {
        __m128d vlow = _mm256_castpd256_pd128(v);
        __m128d vhigh = _mm256_extractf128_pd(v, 1);// high 128
        vlow = _mm_add_pd(vlow, vhigh);             // reduce down to 128

        __m128d high64 = _mm_unpackhi_pd(vlow, vlow);
        return _mm_cvtsd_f64(_mm_add_sd(vlow, high64));// reduce to scalar
    }

    void spmm_generic(const int n, const int *Ap, const int *Ai,
                      const double *Ax, const double *Bx, double *Cx, int bRows, int bCols,
                      const std::vector<Codelet *> *lst,
                      const DDT::Config &cfg) {
        // Perform SpMV
        auto a = std::chrono::steady_clock::now();
#pragma omp parallel for schedule(dynamic,1) num_threads(cfg.nThread)
        for (int i = 0; i < cfg.nThread; i++) {
            for (const auto &c : lst[i]) {
                switch (c->get_type()) {
                    case CodeletType::TYPE_FSC:
                        fsc_t2_2DC_gemm(Cx, Ax, Bx, c->row_offset,
                                        c->first_nnz_loc, c->lbr,
                                        c->lbr + c->row_width, c->lbc,
                                        c->col_width + c->lbc, c->col_offset,bRows,bCols);
                        break;
                    case CodeletType::TYPE_PSC1:
                        psc_t1_2D4R_gemm(Cx, Ax, Bx, c->offsets, c->lbr,
                                         c->lbr + c->row_width, c->lbc,
                                         c->lbc + c->col_width,bRows,bCols);
                        break;
                    case CodeletType::TYPE_PSC2:
                        psc_t2_2DC_gemm(Cx, Ax, Bx, c->offsets, c->row_offset,
                                        c->first_nnz_loc, c->lbr,
                                        c->lbr + c->row_width, c->col_width,
                                        c->col_offset,bRows,bCols);
                        break;
                    case CodeletType::TYPE_PSC3:
                        psc_t3_1D1R_gemm(Cx, Ax, Ai, Bx, c->offsets, c->lbr,
                                         c->first_nnz_loc, c->col_width,bRows,bCols);
                        break;
                    default:
                        break;
                }
            }
        }
    }


    /**
* Computes y[lb:ub] += Ax[axo+axi:axo+axi*cb] * Bx[cbl:cbu]
*
* @param Cx output matrix solution - col storage
* @param Ax in nonzero locations
* @param Bx in the input matrix - col storage
* @param axi distance to next row in matrix
* @param axo distance to first element in matrix
* @param lb in lower bound of rows
* @param ub in upper bound of rows
* @param cbl in number of columns to compute
* @param cbu in number of columns to compute
* @param cbb in number of columns in matrix Bx
* @param cbd in number of rows in matrix Bx
*/
    void fsc_t2_2DC_gemm(double *Cx, const double *Ax, const double *Bx,
                         const int axi, const int axo, const int lb,
                         const int ub, const int cbl, const int cbu,
                         const int co, const int bRows, const int bCols) {
        auto ax0 = Ax+axo;
        auto bx0 = Bx+cbl*bCols;
        auto cx0 = Cx + lb*bCols;
        for (int i = 0; i < (ub-lb); ++i) {
                int j = 0;
                for (; j < (cbu - cbl) - 3; j+=4) {
                    // Load Ax
                    auto a = _mm256_loadu_pd(ax0+j+i*axi);
                    auto av0 = _mm256_permute4x64_pd(a,0b00000000);
                    auto av1 = _mm256_permute4x64_pd(a,0b01010101);
                    auto av2 = _mm256_permute4x64_pd(a,0b10101010);
                    auto av3 = _mm256_permute4x64_pd(a,0b11111111);
                    int k = 0;
                    for (; k < bCols-3; k+=4) {
                        // Load Cx
                        auto cv0 = _mm256_loadu_pd(cx0+i*bCols+k);

                        // Load Bx
                        auto bv0 = _mm256_loadu_pd(bx0+bCols*j+k);
                        auto bv1 = _mm256_loadu_pd(bx0+bCols*(j+1)+k);
                        auto bv2 = _mm256_loadu_pd(bx0+bCols*(j+2)+k);
                        auto bv3 = _mm256_loadu_pd(bx0+bCols*(j+3)+k);

                        // Multiply
                        cv0 = _mm256_fmadd_pd(av0,bv0,cv0);
                        cv0 = _mm256_fmadd_pd(av1,bv1,cv0);
                        cv0 = _mm256_fmadd_pd(av2,bv2,cv0);
                        cv0 = _mm256_fmadd_pd(av3,bv3,cv0);

                        // Store Cx
                        _mm256_storeu_pd(cx0+i*bCols+k,cv0);
                    }
                    for (; k < bCols; ++k) {
                        // Create mask
                        cx0[i*bCols+k] += ax0[j+i*axi] * bx0[bCols*(j+0)+k];
                        cx0[i*bCols+k] += ax0[j+i*axi+1] * bx0[bCols*(j+1)+k];
                        cx0[i*bCols+k] += ax0[j+i*axi+2]* bx0[bCols*(j+2)+k];
                        cx0[i*bCols+k] += ax0[j+i*axi+3] * bx0[bCols*(j+3)+k];
                    }
            }
            for (; j < (cbu - cbl); ++j) {
                for (int k = 0; k < bCols; ++k) {
                    cx0[i*bCols+k] += ax0[j+i*axi] * bx0[bCols*j+k];
                }
            }
        }
    }

    void psc_t1_2D4R_gemm(double *Cx, const double *Ax, const double *Bx,
                          const int *offset, int lb, int ub, int lbc, int ubc,
                          const int bRows, const int bCols) {
        v4df_t Lx_reg, Lx_reg2, Lx_reg3, Lx_reg4, result, result2, result3,
                result4, x_reg, x_reg2;

        int tii = (ub - lb) % 4;
        for (int i = lb, ii = 0; i < ub - tii; i += 4, ii += 4) {
            for (int kk = 0; kk < bCols; kk++) {
                result.v = _mm256_setzero_pd();
                result2.v = _mm256_setzero_pd();
                result3.v = _mm256_setzero_pd();
                result4.v = _mm256_setzero_pd();
                int ti = (ubc - lbc) % 4;
                for (int j = lbc, k = offset[ii], k1 = offset[ii + 1],
                         k2 = offset[ii + 2], k3 = offset[ii + 3];
                     j < ubc - ti; j += 4, k += 4, k1 += 4, k2 += 4, k3 += 4) {
                    x_reg.v = _mm256_loadu_pd((double *) (Bx + j + kk * bRows));
                    Lx_reg.v = _mm256_loadu_pd(
                            (double *) (Ax + k));// Skylake	7	0.5
                    Lx_reg2.v = _mm256_loadu_pd(
                            (double *) (Ax + k1));// Skylake	7
                    Lx_reg3.v = _mm256_loadu_pd(
                            (double *) (Ax + k2));// Skylake	7
                    Lx_reg4.v = _mm256_loadu_pd(
                            (double *) (Ax + k3));// Skylake	7

                    result.v = _mm256_fmadd_pd(Lx_reg.v, x_reg.v,
                                               result.v);//Skylake	4	0.5
                    result2.v = _mm256_fmadd_pd(Lx_reg2.v, x_reg.v,
                                                result2.v);//Skylake	4	0.5
                    result3.v = _mm256_fmadd_pd(Lx_reg3.v, x_reg.v,
                                                result3.v);//Skylake	4	0.5
                    result4.v = _mm256_fmadd_pd(Lx_reg4.v, x_reg.v,
                                                result4.v);//Skylake	4	0.5
                }
                double t0 = 0, t1 = 0, t2 = 0, t3 = 0;
                int jt = ubc - ti - lbc;
                for (int j = ubc - ti, k = offset[ii] + jt,
                         k1 = offset[ii + 1] + jt, k2 = offset[ii + 2] + jt,
                         k3 = offset[ii + 3] + jt;
                     j < ubc; j++, k++, k1++, k2++, k3++) {
                    double xj = Bx[j + k * bRows];
                    t0 += Ax[k] * xj;
                    t1 += Ax[k1] * xj;
                    t2 += Ax[k2] * xj;
                    t3 += Ax[k3] * xj;
                }
                auto h0 = _mm256_hadd_pd(result.v, result2.v);
                Cx[i + kk * bRows] += (h0[0] + h0[2] + t0);
                Cx[i + 1 + kk * bRows] += (h0[1] + h0[3] + t1);
                h0 = _mm256_hadd_pd(result3.v, result4.v);
                Cx[i + 2 + kk * bRows] += (h0[0] + h0[2] + t2);
                Cx[i + 3 + kk * bRows] += (h0[1] + h0[3] + t3);
            }
        }
        /** the rest **/
        for (int i = ub - tii, ii = ub - lb - tii; i < ub; i++, ii++) {
            for (int kk = 0; kk < bCols; kk++) {
                result.v = _mm256_setzero_pd();
                int ti = (ubc - lbc) % 4;
                for (int j = lbc, k = offset[i - lb]; j < ubc - ti;
                     j += 4, k += 4) {
                    x_reg.v = _mm256_loadu_pd((double *) (Bx + j + kk * bRows));
                    Lx_reg.v = _mm256_loadu_pd(
                            (double *) (Ax + k));// Skylake	7	0.5
                    result.v = _mm256_fmadd_pd(Lx_reg.v, x_reg.v,
                                               result.v);//Skylake	4	0.5
                }
                double t0 = 0;
                int jt = ubc - lbc - ti;
                for (int j = ubc - ti, k = offset[i - lb] + jt; j < ubc;
                     j++, k++) {
                    t0 += Ax[k] * Bx[j + kk * bRows];
                }
                auto h0 = hsum_double_avx(result.v);
                Cx[i + kk * bRows] += h0 + t0;
            }
        }
    }


    /**
* Computes y[lb:ub] += Lx[axo+axi:axo+axi*cb] * x[offset[0:cb]]
*
* @param Cx out solution
* @param Ax in nonzero locations
* @param Bx in the input vector
* @param offset in the starting point of each load from x
* @param axi distance to next row in matrix
* @param axo distance to first element in matrix
* @param lb in lower bound of rows
* @param ub in upper bound of rows
* @param cb in number of columns to compute
* @param cbb number of columns in matrix Bx
* @param cbd number of rows in matrix Bx
*/
    void psc_t2_2DC_gemm(double *Cx, const double *Ax, const double *Bx,
                         const int *offset, const int axi, const int axo,
                         const int lb, const int ub, const int cb,
                         const int cof, const int bRows, const int bCols) {

        auto x0 = Bx;
        auto cx = lb*bCols+Cx;
        for (int i = 0; i < (ub-lb); ++i) {
            auto ax0 = Ax + axo + axi * i;
            int j = 0;
            for (; j < cb-3; j+=4) {
                auto a = _mm256_loadu_pd(ax0+j);
                auto av0 = _mm256_permute4x64_pd(a,0b00000000);
                auto av1 = _mm256_permute4x64_pd(a,0b01010101);
                auto av2 = _mm256_permute4x64_pd(a,0b10101010);
                auto av3 = _mm256_permute4x64_pd(a,0b11111111);
                auto bv0 = x0+offset[j]*bCols;
                auto bv1 = x0+offset[j+1]*bCols;
                auto bv2 = x0+offset[j+2]*bCols;
                auto bv3 = x0+offset[j+3]*bCols;
                int k = 0;
                for (; k < bCols-3; k+=4) {
                    auto cx0 = _mm256_loadu_pd(cx+i*bCols+k);
                    cx0 = _mm256_fmadd_pd(_mm256_loadu_pd(bv0+k),av0,cx0);
                    cx0 = _mm256_fmadd_pd(_mm256_loadu_pd(bv1+k),av1,cx0);
                    cx0 = _mm256_fmadd_pd(_mm256_loadu_pd(bv2+k),av2,cx0);
                    cx0 = _mm256_fmadd_pd(_mm256_loadu_pd(bv3+k),av3,cx0);
                    _mm256_storeu_pd(cx+i*bCols+k,cx0);
                }
                for (; k < bCols; ++k) {
                    cx[i*bCols+k] += ax0[j] * bv0[k];
                    cx[i*bCols+k] += ax0[j+1] * bv1[k];
                    cx[i*bCols+k] += ax0[j+2] * bv2[k];
                    cx[i*bCols+k] += ax0[j+3] * bv3[k];
                }
            }
            for (; j < cb; ++j) {
                auto bv0 = x0+offset[j]*bCols;
                for (int k = 0; k < bCols; ++k) {
                    cx[i*bCols+k] += ax0[j] * bv0[k];
                }
            }
        }
    }


    void psc_t3_1D1R_gemm(double *Cx, const double *Ax, const int *Ai,
                          const double *Bx, const int *offset, int lb, int fnl,
                          int cw, const int bRows, const int bCols) {
        int i = lb;
        int k = fnl;
        auto cx = Cx+(i*bCols);
        for (; k < fnl+cw; ++k) {
            auto ax0 = _mm256_set1_pd(Ax[k]);
            int kk = 0;
            auto bc0 = Bx+Ai[k]*bCols;
            for (; kk < bCols-3; kk+=4) {
                auto cx0 = _mm256_loadu_pd(cx+kk);
                auto bx0 = _mm256_loadu_pd(bc0 + kk);
                cx0 = _mm256_fmadd_pd(ax0, bx0, cx0);
                _mm256_storeu_pd(cx+kk,cx0);
            }
            for (; kk < bCols; ++kk) {
                cx[kk] += Ax[k] * bc0[kk];
            }
        }
    }
}