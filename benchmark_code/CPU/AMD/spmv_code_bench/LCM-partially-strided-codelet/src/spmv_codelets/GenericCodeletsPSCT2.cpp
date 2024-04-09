/*
 * =====================================================================================
 *
 *       Filename:  PSC2.cpp
 *
 *    Description:  Generic codes for partially strided codelet type 2
 *
 *        Version:  1.0
 *        Created:  2021-07-12 09:35:55 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Zachary Cetinic, 
 *   Organization:  University of Toronto
 *
 * =====================================================================================
 */
#include <immintrin.h>
#include <iostream>

namespace DDT {

#ifdef _mm256_i32gather_pd
#define loadvx(of, x, ind, xv, vindex) \
  auto (vindex) = _mm_loadu_si128(reinterpret_cast<const __m128i *>(of+ind)); \
  (xv) = _mm256_i32gather_pd(x, vindex, 8);
#else
#define loadvx(of, x, ind, xv, msk) \
  xv = _mm256_set_pd(x[of[ind+3]], x[of[ind+2]], x[of[ind+1]], x[of[ind]]);

#endif

    const __m256i MASK_3 = _mm256_set_epi64x(0, -1, -1, -1);

 inline double hsum_double_avx(__m256d v) {
  __m128d vlow = _mm256_castpd256_pd128(v);
  __m128d vhigh = _mm256_extractf128_pd(v, 1); // high 128
  vlow = _mm_add_pd(vlow, vhigh);     // reduce down to 128

  __m128d high64 = _mm_unpackhi_pd(vlow, vlow);
  return _mm_cvtsd_f64(_mm_add_sd(vlow, high64));  // reduce to scalar
 }

/**
 * Computes y[lb:ub] += Lx[axo+axi:axo+axi*cb] * x[offset[0:cb]]
 *
 * @param y out solution
 * @param Ax in nonzero locations
 * @param x in the input vector
 * @param offset in the starting point of each load from x
 * @param axi distance to next row in matrix
 * @param axo distance to first element in matrix
 * @param lb in lower bound of rows
 * @param ub in upper bound of rows
 * @param cb in number of columns to compute
 */
 void psc_t2_2DC(
   double *y,
   const double *Ax,
   const double *x,
   const int *offset,
   const int axi,
   const int axo,
   const int lb,
   const int ub,
   const int cb,
   const int cof
 ) {
  auto ax0 = Ax + axo + axi * 0;
  auto ax1 = Ax + axo + axi * 1;
  auto x0  = x;
  auto x1  = x0 + cof;

  int co = (ub-lb) % 2;
  for (int i = lb; i < ub-co; i += 2) {
   auto r0 = _mm256_setzero_pd();
   auto r1 = _mm256_setzero_pd();

   int j = 0;
   for (; j < cb-3; j += 4) {
    __m256d xv0, xv1;
    loadvx(offset, x0, j, xv0, msk0);
    loadvx(offset, x1, j, xv1, msk1);

    auto axv0 = _mm256_loadu_pd(ax0 + j);
    auto axv1 = _mm256_loadu_pd(ax1 + j);

    r0 = _mm256_fmadd_pd(axv0, xv0, r0);
    r1 = _mm256_fmadd_pd(axv1, xv1, r1);
   }

      // Compute tail
   __m128d tail = _mm_loadu_pd(y+i);

   for (; j < cb; j++) {
       tail[0] += ax0[j] * x0[offset[j]];
       tail[1] += ax1[j] * x1[offset[j]];
   }

   // H-Sum
   auto h0 = _mm256_hadd_pd(r0, r1);
   __m128d vlow = _mm256_castpd256_pd128(h0);
   __m128d vhigh = _mm256_extractf128_pd(h0, 1);  // high 128
   vlow = _mm_add_pd(vlow, vhigh);     // reduce down to 128
   vlow = _mm_add_pd(vlow, tail);

   // Store
   _mm_storeu_pd(y + i, vlow);

   // Load new addresses
   ax0 += axi * 2;
   ax1 += axi * 2;
   x0  += cof*2;
   x1  += cof*2;
  }

  if (co) {
      // Compute last iteration
      auto r0 = _mm256_setzero_pd();
      __m256d xv;
      int j = 0;
      for (; j < cb - 3; j += 4) {
          loadvx(offset, x0, j, xv, msk);
          auto axv0 = _mm256_loadu_pd(ax0 + j);
          r0 = _mm256_fmadd_pd(axv0, xv, r0);
      }

      // Compute tail
      double tail = 0.;
      for (; j < cb; j++) { tail += *(ax0 + j) * x0[offset[j]]; }

      // H-Sum
      y[ub - 1] += tail + hsum_double_avx(r0);
  }
 }

/**
 * Computes y[lb:ub] += Lx[axo+axi:axo+axi*4] * x[offset[0:4]]
 *
 * @param y
 * @param Ax
 * @param x
 * @param offset
 * @param axi
 * @param axo
 * @param lb
 * @param ub
 * @param cb
 */
 void psc_t2_2D4C(
   double *y,
   const double *Ax,
   const double *x,
   const int *offset,
   const int axi,
   const int axo,
   const int lb,
   const int ub,
   const int cb
 ) {
  auto ax0 = Ax + axo + axi * 0;
  auto ax1 = Ax + axo + axi * 1;

  __m256d xv;
  loadvx(offset, x, 0, xv, msk);
  for (int i = lb; i < ub; i += 2) {
   auto axv0 = _mm256_loadu_pd(ax0);
   auto axv1 = _mm256_loadu_pd(ax1);

   auto r0 = _mm256_mul_pd(axv0, xv);
   auto r1 = _mm256_mul_pd(axv1, xv);

   // H-Sum
   auto h0 = _mm256_hadd_pd(r0, r1);
   __m128d vlow = _mm256_castpd256_pd128(h0);
   __m128d vhigh = _mm256_extractf128_pd(h0, 1);  // high 128
   vlow = _mm_add_pd(vlow, vhigh);     // reduce down to 128

   // Store
   _mm_storeu_pd(y + i, vlow);

   // Load new addresses
   ax0 += axo * 2;
   ax1 += axo * 2;
  }

  // Compute last iteration
  auto r0 = _mm256_setzero_pd();
  int j = cb;
  for (; j < cb; j += 4) {
   auto axv0 = _mm256_loadu_pd(ax0 + j);
   r0 = _mm256_fmadd_pd(axv0, xv, r0);
  }

  // H-Sum
  y[ub - 1] = hsum_double_avx(r0);
 }

 /**
  * Executes fused PSC type 2 codelets
  *
  * @param cw
  * @param fnl
  * @param tsc
  * @param lb
  * @param ub
  * @param p
  * @param o
  * @param Ax
  * @param x
  * @param y
  */
 void f_psc_t2(int cw, int fnl, int tsc, int lb, int ub, int p, const int* o, const double* Ax, const double* x, double* y) {
     auto np = o;
     auto ch = o + p;
     auto cs = ch+tsc;
     auto cl = ch+tsc*p;
     auto axo = Ax + fnl;

     for (int i = 0; i < ub-lb; i += p) {
         for (int j = 0; j < p; ++j) {        // For each period
             auto r0 = _mm256_setzero_pd();
             double tail = 0.;

             auto chp = ch + np[j];
             auto csp = cs + np[j];
             auto clp = cl + np[j];

             for (int k = 0, axl = 0; k < np[j]; axl+=csp[k], ++k) { // For each set of line codelets
                 for (int l = 0; l < csp[k]; l++) {
                     tail += axo[axl+l] * x[clp[k]+chp[k]*i+l];
                 }
             }
             y[lb+i] += hsum_double_avx(r0) + tail;
         }
         axo += cw;
     }
 }
}
