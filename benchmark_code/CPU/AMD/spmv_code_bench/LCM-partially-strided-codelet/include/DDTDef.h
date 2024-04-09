//
// Created by kazem on 7/12/21.
//

#ifndef DDT_DDTDEF_H
#define DDT_DDTDEF_H

#include <immintrin.h>
namespace DDT {
  inline int clt_width = 0;
  inline bool prefer_fsc = false;
  inline int  col_th   = 8;

#ifdef __AVX2__
 typedef union
 {
  __m256d v;
  double d[4];
 } v4df_t;

 typedef union
 {
  __m128i v;
  int d[4];
 } v4if_t;

 union vector128
 {
  __m128i     i128;
  __m128d     d128;
  __m128      f128;
 };

    static int TPR = 3;

    enum CodeletType {
        TYPE_FSC = 0,
        TYPE_PSC1 = 1,
        TYPE_PSC2 = 2,
        TYPE_PSC3 = 3,
        TYPE_PSC3_V1 = 4,
        TYPE_PSC3_V2 = 5,
        TYPE_F_PSC = 6
    };

    const int MAX_CODELETS_PER_ITERATION = 100;
    const int MIN_LIM_ITERATIONS = 40;
    const int SP_ITER_THRESHOLD = 4;
    const int MAX_LIM = 4;

    struct PatternDAG {
        int sz;
        int* ct;
        int* pt;
        CodeletType t;
    };

#endif

#ifdef __AVX512CD__
 typedef union
 {
  __m512d v;
  double d[8];
 } v5df_t;
#endif
}

#endif //DDT_DDTDEF_H
