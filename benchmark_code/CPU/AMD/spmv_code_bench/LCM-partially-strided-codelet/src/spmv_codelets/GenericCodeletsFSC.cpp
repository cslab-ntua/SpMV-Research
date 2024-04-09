/*
 * =====================================================================================
 *
 *       Filename:  GenericCodeletsFSC.cpp
 *
 *    Description:  Generic codelets for fully strided patterns 
 *
 *        Version:  1.0
 *        Created:  2021-07-12 12:56:27 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Zachary Cetinic, 
 *   Organization:  University of Toronto
 *
 * =====================================================================================
 */
#include "GenericCodelets.h"

#include <immintrin.h>
#include <iostream>

namespace DDT {


    inline double hsum_double_avx(__m256d v) {
        __m128d vlow = _mm256_castpd256_pd128(v);
        __m128d vhigh = _mm256_extractf128_pd(v, 1);// high 128
        vlow = _mm_add_pd(vlow, vhigh);             // reduce down to 128

        __m128d high64 = _mm_unpackhi_pd(vlow, vlow);
        return _mm_cvtsd_f64(_mm_add_sd(vlow, high64));// reduce to scalar
    }

    /**
 * Computes y[lb:ub] += Lx[axo+axi:axo+axi*cb] * x[cbl:cbu]
 *
 * @param y out solution
 * @param Ax in nonzero locations
 * @param x in the input vector
 * @param axi distance to next row in matrix
 * @param axo distance to first element in matrix
 * @param lb in lower bound of rows
 * @param ub in upper bound of rows
 * @param cbl in number of columns to compute
 * @param cbu in number of columns to compute
 */
    void fsc_t2_2DC(double *y, const double *Ax, const double *x, const int axi,
                    const int axo, const int lb, const int ub, const int cbl,
                    const int cbu, const int co) {
        auto ax0 = Ax + axo + axi * 0;
        auto ax1 = Ax + axo + axi * 1;
        auto x0 = x + cbl;
        auto x1 = x0 + co;

        int cr = (ub - lb) % 2;
        for (int i = lb; i < ub - 1; i += 2) {
            auto r0 = _mm256_setzero_pd();
            auto r1 = _mm256_setzero_pd();

            int j = 0;
            for (; j < (cbu - cbl) - 3; j += 4) {
                auto xv0 = _mm256_loadu_pd(x0 + j);
                auto xv1 = _mm256_loadu_pd(x1 + j);

                auto axv0 = _mm256_loadu_pd(ax0 + j);
                auto axv1 = _mm256_loadu_pd(ax1 + j);

                r0 = _mm256_fmadd_pd(axv0, xv0, r0);
                r1 = _mm256_fmadd_pd(axv1, xv1, r1);
            }

            // Compute tail
            __m128d tail = _mm_loadu_pd(y + i);
            for (; j < (cbu - cbl); j++) {
                tail[0] += ax0[j] * x0[j];
                tail[1] += ax1[j] * x1[j];
            }

            // H-Sum
            auto h0 = _mm256_hadd_pd(r0, r1);
            __m128d vlow = _mm256_castpd256_pd128(h0);
            __m128d vhigh = _mm256_extractf128_pd(h0, 1);// high 128
            vlow = _mm_add_pd(vlow, vhigh);              // reduce down to 128
            vlow = _mm_add_pd(vlow, tail);
            // Store
            _mm_storeu_pd(y + i, vlow);

            // Load new addresses
            ax0 += axi * 2;
            ax1 += axi * 2;
            x0 += co * 2;
            x1 += co * 2;
        }

        // Compute last iteration
        if (cr) {
            auto r0 = _mm256_setzero_pd();
            int j = 0;
            for (; j < (cbu - cbl) - 3; j += 4) {
                auto xv = _mm256_loadu_pd(x0 + j);
                auto axv0 = _mm256_loadu_pd(ax0 + j);
                r0 = _mm256_fmadd_pd(axv0, xv, r0);
            }

            // Compute tail
            double tail = 0.;
            for (; j < cbu - cbl; j++) { tail += *(ax0 + j) * x0[j]; }

            // H-Sum
            y[ub - 1] += tail + hsum_double_avx(r0);
        }
    }

}