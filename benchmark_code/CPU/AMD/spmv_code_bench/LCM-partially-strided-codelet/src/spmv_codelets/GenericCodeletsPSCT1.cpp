//
// Created by kazem on 7/12/21.
//
#include "DDTDef.h"
#include "GenericCodelets.h"
#include <iostream>

namespace DDT{
/**
 * Different implementations of PSC Type2 for SpMV ->
 * y[lb:ub]+=Ax[offset[lb]:offset[lb]+WDT, ...,
 * offset[ub-1]:offset[ub-1]+WDT]*x[lbc:ubc] where WDT=ubc-lbc
 *
 * @param y out solution
 * @param Ax in nonzero locations
 * @param x in the input vector
 * @param offset in the starting point of Ax in each row
 * @param lb in lower bound of rows
 * @param ub in upper bound of rows
 * @param lbc in lower bound of columns
 * @param ubc in upper bound of columns
 */
 void psc_t1_1D4C(double *y, const double *Ax, const double *x,
                         const int *offset, int lb, int ub, int lbc, int ubc){
  v4df_t Lx_reg, Lx_reg2, result, result2, x_reg, x_reg2;
  for (int i = lb, ii=0; i <ub; ++i, ++ii) {
   result.v = _mm256_setzero_pd();
   for (int j = lbc, k=offset[ii]; j < ubc; j+=4, k+=4) {
    //y[i] += Ax[k] * x[j];
    //_mm256_mask_i32gather_pd()
    // x_reg.d[0] = x[*aij]; /// TODO replaced with gather
    // x_reg.d[1] = x[*(aij+1)];
    // x_reg.d[2] = x[*(aij+2)];
    // x_reg.d[3] = x[*(aij+3)];
    //x_reg.v = _mm256_set_pd(x[j], x[j+1], x[j+2], x[j+3]);
    x_reg.v = _mm256_loadu_pd((double *) (x+j));
    Lx_reg.v = _mm256_loadu_pd((double *) (Ax + k)); // Skylake	7	0.5
    result.v = _mm256_fmadd_pd(Lx_reg.v,x_reg.v,result.v);//Skylake	4	0.5
    //x_reg2.v = _mm256_set_pd(x[*(aik)], x[*(aik+1)], x[*(aik+2)], x[*
    // (aik+3)]);
    //Lx_reg2.v = _mm256_loadu_pd((double *) (Ax + k + 4)); // Skylake	7	0.5
    //result2.v = _mm256_fmadd_pd(Lx_reg2.v,x_reg2.v,result2.v);//Skylake	4	0.5
   }
   auto h0 = _mm_hadd_pd(_mm256_extractf128_pd(result.v,0), _mm256_extractf128_pd(result.v,1));
   y[i] += (h0[0] + h0[1]);
  }
 }

 void psc_t1_1D8C(double *y, const double *Ax, const double *x,
                        const int *offset, int lb, int ub, int lbc, int ubc){
  v4df_t Lx_reg, Lx_reg2, result, result2, x_reg, x_reg2;
  for (int i = lb, ii=0; i <ub; ++i, ++ii) {
   result.v = _mm256_setzero_pd();
   for (int j = lbc, k=offset[ii]; j < ubc; j+=8, k+=8) {
    //y[i] += Ax[k] * x[j];
    //_mm256_mask_i32gather_pd()
    // x_reg.d[0] = x[*aij]; /// TODO replaced with gather
    // x_reg.d[1] = x[*(aij+1)];
    // x_reg.d[2] = x[*(aij+2)];
    // x_reg.d[3] = x[*(aij+3)];
    //x_reg.v = _mm256_set_pd(x[j], x[j+1], x[j+2], x[j+3]);
    x_reg.v = _mm256_loadu_pd((double *) (x+j));
    Lx_reg.v = _mm256_loadu_pd((double *) (Ax + k)); // Skylake	7	0.5
    result.v = _mm256_fmadd_pd(Lx_reg.v,x_reg.v,result.v);//Skylake	4	0.5
    //x_reg2.v = _mm256_set_pd(x[*(aik)], x[*(aik+1)], x[*(aik+2)], x[*
    // (aik+3)]);
    //Lx_reg2.v = _mm256_loadu_pd((double *) (Ax + k + 4)); // Skylake	7	0.5
    //result2.v = _mm256_fmadd_pd(Lx_reg2.v,x_reg2.v,result2.v);//Skylake	4	0.5
    x_reg2.v = _mm256_loadu_pd((double *) (x+4+j));
    Lx_reg2.v = _mm256_loadu_pd((double *) (Ax+4+k)); // Skylake	7	0.5
    result.v = _mm256_fmadd_pd(Lx_reg2.v,x_reg2.v,result.v);//Skylake	4	0.5
   }
   auto h0 = _mm_hadd_pd(_mm256_extractf128_pd(result.v,0), _mm256_extractf128_pd(result.v,1));
   y[i] += (h0[0] + h0[1]);
  }
 }

 void psc_t1_2D2R(double *y, const double *Ax, const double *x,
                            const int *offset, int lb, int ub, int lbc, int ubc){
  v4df_t Lx_reg, Lx_reg2, result, result2, x_reg, x_reg2;
  for (int i = lb, ii=0; i <ub; i+=2, ii+=2) {
   result.v = _mm256_setzero_pd();
   result2.v = _mm256_setzero_pd();
   for (int j = lbc, k=offset[ii], k1 = offset[ii+1]; j < ubc; j+=4, k+=4,
                                                               k1+=4) {
    //y[i] += Ax[k] * x[j];
    //_mm256_mask_i32gather_pd()
    // x_reg.d[0] = x[*aij]; /// TODO replaced with gather
    // x_reg.d[1] = x[*(aij+1)];
    // x_reg.d[2] = x[*(aij+2)];
    // x_reg.d[3] = x[*(aij+3)];
    //x_reg.v = _mm256_set_pd(x[j], x[j+1], x[j+2], x[j+3]);
    x_reg.v = _mm256_loadu_pd((double *) (x+j));
    Lx_reg.v = _mm256_loadu_pd((double *) (Ax + k)); // Skylake	7	0.5
    Lx_reg2.v = _mm256_loadu_pd((double *) (Ax + k1)); // Skylake	7
    // 	0.5
    result.v = _mm256_fmadd_pd(Lx_reg.v,x_reg.v,result.v);//Skylake	4	0.5
    result2.v = _mm256_fmadd_pd(Lx_reg2.v,x_reg.v,result2.v);//Skylake	4	0.5
   }
   //auto h0 = _mm_hadd_pd(_mm256_extractf128_pd(result.v,0),
   //                       _mm256_extractf128_pd(result.v,1));
   auto h0 = _mm256_hadd_pd(result.v, result2.v);
   y[i] += (h0[0] + h0[2]);
   y[i+1] += (h0[1] + h0[3]);
   //y[i] += (h0[0] + h0[1]);
  }
 }

 inline double hsum_double_avx(__m256d v) {
  __m128d vlow = _mm256_castpd256_pd128(v);
  __m128d vhigh = _mm256_extractf128_pd(v, 1); // high 128
  vlow = _mm_add_pd(vlow, vhigh);     // reduce down to 128

  __m128d high64 = _mm_unpackhi_pd(vlow, vlow);
  return _mm_cvtsd_f64(_mm_add_sd(vlow, high64));  // reduce to scalar
 }

 void psc_t1_2D4R(double *y, const double *Ax, const double *x,
                              const int *offset, int lb, int ub, int lbc, int ubc){
  v4df_t Lx_reg, Lx_reg2, Lx_reg3, Lx_reg4, result, result2, result3,
    result4, x_reg, x_reg2;

  int tii = (ub-lb)%4;
/*  for (int i = lb; i < ub-tii; i++) {
   for (int j = lbc; j < ubc; j++) {
    y[i] += Ax[offset[i-lb]+(j-lbc)] * x[j];
   }
  }*/
  for (int i=lb, ii=0; i < ub-tii; i+=4, ii+=4) {
   result.v = _mm256_setzero_pd();
   result2.v = _mm256_setzero_pd();
   result3.v = _mm256_setzero_pd();
   result4.v = _mm256_setzero_pd();
   int ti = (ubc-lbc)%4;
   for (int j = lbc, k=offset[ii], k1=offset[ii+1], k2=offset[ii+2],
          k3=offset[ii+3]; j < ubc-ti; j+=4, k+=4, k1+=4, k2+=4, k3+=4) {
    x_reg.v = _mm256_loadu_pd((double *) (x+j));
    Lx_reg.v = _mm256_loadu_pd((double *) (Ax + k)); // Skylake	7	0.5
    Lx_reg2.v = _mm256_loadu_pd((double *) (Ax + k1)); // Skylake	7
    Lx_reg3.v = _mm256_loadu_pd((double *) (Ax + k2)); // Skylake	7
    Lx_reg4.v = _mm256_loadu_pd((double *) (Ax +k3)); // Skylake	7

    result.v = _mm256_fmadd_pd(Lx_reg.v,x_reg.v,result.v);//Skylake	4	0.5
    result2.v = _mm256_fmadd_pd(Lx_reg2.v,x_reg.v,result2.v);//Skylake	4	0.5
    result3.v = _mm256_fmadd_pd(Lx_reg3.v,x_reg.v,result3.v);//Skylake	4	0.5
    result4.v = _mm256_fmadd_pd(Lx_reg4.v,x_reg.v,result4.v);//Skylake	4	0.5
   }
   double t0=0, t1=0, t2=0, t3=0;
   int jt = ubc-ti-lbc;
   for ( int j=ubc-ti, k=offset[ii]+jt, k1=offset[ii+1]+jt, k2=offset[ii+2]+jt,
           k3=offset[ii+3]+jt; j < ubc; j++, k++, k1++, k2++, k3++) {
    double xj = x[j];
    t0 += Ax[k] * xj;
    t1 += Ax[k1] * xj;
    t2 += Ax[k2] * xj;
    t3 += Ax[k3] * xj;
   }
   auto h0 = _mm256_hadd_pd(result.v, result2.v);
   y[i] += (h0[0] + h0[2] + t0);
   y[i+1] += (h0[1] + h0[3] + t1);
   h0 = _mm256_hadd_pd(result3.v, result4.v);
   y[i+2] += (h0[0] + h0[2] + t2);
   y[i+3] += (h0[1] + h0[3] + t3);
  }
  /** the rest **/
  for (int i = ub-tii, ii=ub-lb-tii; i < ub; i++, ii++) {
   result.v = _mm256_setzero_pd();
   int ti = (ubc-lbc) % 4;
   for (int j = lbc, k=offset[i-lb]; j < ubc-ti; j+=4, k+=4) {
    x_reg.v = _mm256_loadu_pd((double *) (x+j));
    Lx_reg.v = _mm256_loadu_pd((double *) (Ax + k)); // Skylake	7	0.5
    result.v = _mm256_fmadd_pd(Lx_reg.v,x_reg.v,result.v);//Skylake	4	0.5
   }
   double t0=0;
   int jt = ubc-lbc-ti;
   for (int j = ubc-ti, k=offset[i-lb]+jt; j < ubc; j++, k++) {
    t0 += Ax[k] * x[j];
   }
   auto h0 = hsum_double_avx(result.v);
   y[i] += h0 + t0;
  }
 }

 void psc_t1_2D4Rt(double *y, const double *Ax, const double *x,
                  const int *offset, int lb, int ub, int lbc, int ubc){
  // if (lb <= 14334 && 14334 < ub) {
  //     std::cout << lb << std::endl;
  //     std::cout << ub << std::endl;
  //     std::cout << offset[0] << std::endl;
  //     std::cout << "psc1 here" << std::endl;
  //     std::cout << std::endl;
  // }
  for (int i = lb; i < ub; i++) {
   for (int j = lbc; j < ubc; j++) {
    y[i] += Ax[offset[i-lb]+(j-lbc)] * x[j];
   }
  }
 }


}
