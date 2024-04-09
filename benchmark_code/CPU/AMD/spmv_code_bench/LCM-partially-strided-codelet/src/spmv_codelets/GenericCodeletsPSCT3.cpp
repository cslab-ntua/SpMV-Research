//
// Created by kazem on 7/12/21.
//

#include "DDTDef.h"
#include <iostream>

namespace DDT {
/**
 * From any location
 * @param y
 * @param Ax
 * @param Ai
 * @param x
 * @param offset
 * @param lb
 * @param fnl
 * @param cw
 */
 void psc_t3_1DAnyR(double *y, const double *Ax, const int *Ai,  const double
 *x, const int *offset, int lb, int fnl, int cw) {
  v4df_t Lx_reg, Lx_reg2, result, result2, x_reg, x_reg2;
  int i = lb;
  result.v = _mm256_setzero_pd();
  int ti = cw % 4;
  int k = fnl;
  for (int j = 0; j < cw-ti; j+=4, k+=4) {
   x_reg.v = _mm256_set_pd(x[Ai[offset[j+3]]], x[Ai[offset[j+2]]],
                           x[Ai[offset[j+1]]], x[Ai[offset[j]]]);
   Lx_reg.v = _mm256_set_pd(Ax[offset[j+3]], Ax[offset[j+2]],
                           Ax[offset[j+1]], Ax[offset[j]]);
   //Lx_reg.v = _mm256_loadu_pd((double *) (Ax + k)); // Skylake	7	0.5
   result.v = _mm256_fmadd_pd(Lx_reg.v,x_reg.v,result.v);//Skylake	4	0.5
  }
  double tail = 0;
  for (int j = cw-ti; j < cw; ++j, ++k) {
   tail += (Ax[offset[j]] * x[Ai[offset[j]]]);
  }
  auto h0 = _mm_hadd_pd(_mm256_extractf128_pd(result.v,0), _mm256_extractf128_pd(result.v,1));
  y[i] += (h0[0] + h0[1] + tail);
 }


    inline double hsum_double_avx(__m256d v) {
        __m128d vlow = _mm256_castpd256_pd128(v);
        __m128d vhigh = _mm256_extractf128_pd(v, 1); // high 128
        vlow = _mm_add_pd(vlow, vhigh);     // reduce down to 128

        __m128d high64 = _mm_unpackhi_pd(vlow, vlow);
        return _mm_cvtsd_f64(_mm_add_sd(vlow, high64));  // reduce to scalar
    }

 void psc_t3_1D1R(double *y, const double *Ax, const int *Ai,  const double
 *x, const int *offset, int lb, int fnl, int cw) {
  v4df_t Lx_reg, Lx_reg2, result, result2, x_reg, x_reg2;
  int i = lb;
  result.v = _mm256_setzero_pd();
  int ti = cw % 4;
  int k = fnl;
  int j = 0;
  for (; j < cw-3; j+=4, k+=4) {
   x_reg.v = _mm256_set_pd(x[offset[j+3]], x[offset[j+2]],
                           x[offset[j+1]], x[offset[j]]);
   Lx_reg.v = _mm256_loadu_pd((double *) (Ax + k)); // Skylake	7	0.5
   result.v = _mm256_fmadd_pd(Lx_reg.v,x_reg.v,result.v);//Skylake	4	0.5
  }
  double tail = 0;
  for (; j < cw; ++j, ++k) {
   tail += (Ax[k] * x[offset[j]]);
  }
  auto h0 = hsum_double_avx(result.v);
  y[i] += h0 + tail;
 }
}
