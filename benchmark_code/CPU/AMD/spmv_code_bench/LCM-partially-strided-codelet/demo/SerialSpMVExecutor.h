//
// Created by cetinicz on 2021-08-02.
//

#ifndef DDT_SERIALSPMVEXECUTOR_H
#define DDT_SERIALSPMVEXECUTOR_H

#include <iostream>
#include <cassert>
#include <immintrin.h>
#include <tuple>
#include <utility>
#include <cstdint>
typedef void (*FunctionPtr)();

inline double hsum_double_avx(__m256d v) {
    __m128d vlow = _mm256_castpd256_pd128(v);
    __m128d vhigh = _mm256_extractf128_pd(v, 1);  // high 128
    vlow = _mm_add_pd(vlow, vhigh);      // reduce down to 128

    __m128d high64 = _mm_unpackhi_pd(vlow, vlow);
    return _mm_cvtsd_f64(_mm_add_sd(vlow, high64));  // reduce to scalar
}

inline void hsum_double_avx_avx(__m256d v0, __m256d v1, __m128d v2, double* y, int i) {
    auto h0 = _mm256_hadd_pd(v0,v1);
    __m128d vlow = _mm256_castpd256_pd128(h0);
    __m128d vhigh = _mm256_extractf128_pd(h0, 1);  // high 128
    vlow = _mm_add_pd(vlow, vhigh);
    vlow = _mm_add_pd(vlow, v2);
    _mm_storeu_pd(y+i, vlow);
}

inline double hsum_add_double_avx(__m256d v, __m128d a) {
    __m128d vlow = _mm256_castpd256_pd128(v);
    __m128d vhigh = _mm256_extractf128_pd(v, 1);  // high 128
    vlow = _mm_add_pd(vlow, vhigh);      // reduce down to 128
    vlow = _mm_add_pd(vlow, a);

    __m128d high64 = _mm_unpackhi_pd(vlow, vlow);
    return _mm_cvtsd_f64(_mm_add_sd(vlow, high64));  // reduce to scalar
}

inline uint8_t* cword;
inline uint8_t *sp;
inline double *yp;
inline double *xp;
inline double *axp;
inline int *app;
inline int *axi;
inline int m;
inline FunctionPtr *fp;
inline FunctionPtr *mapped_fp;


void v1();

inline void v1_2() {
    auto p = axi[1];
    *yp += *axp * xp[*axi];
    *yp += axp[1] * xp[p + 0];
    *yp += axp[2] * xp[p + 1];
    axi += 3;
    axp += 3;
}

inline void v1_3() {
    auto p = axi[1];
    *yp += *axp * xp[*axi];
    *yp += axp[1] * xp[p + 0];
    *yp += axp[2] * xp[p + 1];
    *yp += axp[3] * xp[p + 2];
    axi += 4;
    axp += 4;
}

inline void v1_2_1() {
    auto p1 = axi[1];
    *yp += *axp * xp[*axi];
    *yp += axp[1] * xp[p1];
    *yp += axp[2] * xp[p1 + 1];
    *yp += axp[3] * xp[axi[3]];
    axi += 4;
    axp += 4;
}

inline void v1_1_3() {
    auto p1 = axi[1];
    *yp += *axp * xp[*axi];
    *yp += axp[1] * xp[p1];
    *yp += axp[2] * xp[p1 + 1];
    *yp += axp[3] * xp[p1 + 2];
    *yp += axp[4] * xp[axi[4]];
    axi += 5;
    axp += 5;
}

inline void v1_3_1() {
    auto p1 = axi[1];
    *yp += *axp * xp[*axi];
    *yp += axp[1] * xp[p1];
    *yp += axp[2] * xp[p1 + 1];
    *yp += axp[3] * xp[p1 + 2];
    *yp += axp[4] * xp[axi[4]];
    axi += 5;
    axp += 5;
}

inline void v1_2_1_1() {
    *yp += *axp++ * xp[*axi++];
    *yp += *axp++ * xp[*axi++];
    *yp += *axp++ * xp[*axi++];
    *yp += *axp++ * xp[*axi++];
    *yp += *axp++ * xp[*axi++];
}

inline void v1_1_2_1() {
    *yp += *axp++ * xp[*axi++];
    *yp += *axp++ * xp[*axi++];
    *yp += *axp++ * xp[*axi++];
    *yp += *axp++ * xp[*axi++];
    *yp += *axp++ * xp[*axi++];
}

inline void v1_1_3_1() {
    auto p = axi[2];
    *yp += axp[0] * xp[axi[0]];
    *yp += axp[1] * xp[axi[1]];
    *yp += axp[2] * xp[p];
    *yp += axp[3] * xp[p + 1];
    *yp += axp[4] * xp[p + 2];
    *yp += axp[5] * xp[axi[5]];
    axp += 6;
    axi += 6;
}

void v2();

inline void v2_() {
    auto p = *axi;
    *yp += axp[0] * xp[p];
    *yp += axp[1] * xp[p + 1];
    axi += 2;
    axp += 2;
}

inline void v2_1() {
    auto p = *axi;
    *yp += axp[0] * xp[p];
    *yp += axp[1] * xp[p + 1];

    *yp += axp[2] * xp[axi[2]];
    axi += 3;
    axp += 3;
}

void v3();
inline __m256i MASK_3 = _mm256_set_epi64x(0,-1,-1,-1);
inline void v3_() {
    auto o0 = axi[0];
    auto r0 = _mm256_setzero_pd();
    r0 = _mm256_fmadd_pd(_mm256_loadu_pd(axp), _mm256_maskload_pd(xp+o0, MASK_3), r0);
    *yp += hsum_double_avx(r0);
    axi += 3;
    axp += 3;
}
inline void v3_1() {
    auto o0 = axi[0], o1 = axi[3];
    auto r0 = _mm256_setzero_pd();
    r0 = _mm256_fmadd_pd(_mm256_loadu_pd(axp), _mm256_maskload_pd(xp+o0, MASK_3), r0);
    *yp += hsum_double_avx(r0) + axp[3] * xp[o1];

    axp += 4;
    axi += 4;
}

inline void v3_2_3() {
    int o0 = axi[0], o1 = axi[3], o2 = axi[5];

    auto r0 = _mm256_setzero_pd();
    r0 = _mm256_fmadd_pd(_mm256_loadu_pd(axp), _mm256_maskload_pd(xp+o0, MASK_3), r0);
    r0 = _mm256_fmadd_pd(_mm256_loadu_pd(axp+5), _mm256_maskload_pd(xp+o2, MASK_3), r0);

    *yp += hsum_add_double_avx(r0, _mm_mul_pd(_mm_loadu_pd(axp+3), _mm_loadu_pd(xp+o1)));
    axp+=8;
    axi+=8;
}

inline void v3_2_3_2_3() {
    int o0 = axi[0], o1 = axi[3], o2 = axi[5], o3 = axi[8], o4 = axi[10];

    auto r0 = _mm256_setzero_pd();
    auto r1 = _mm_setzero_pd();
    r0 = _mm256_fmadd_pd(_mm256_loadu_pd(axp), _mm256_maskload_pd(xp+o0, MASK_3), r0);
    r1 = _mm_fmadd_pd(_mm_loadu_pd(axp+3), _mm_loadu_pd(xp+o1), r1);
    r0 = _mm256_fmadd_pd(_mm256_loadu_pd(axp+5), _mm256_maskload_pd(xp+o2, MASK_3), r0);
    r1 = _mm_fmadd_pd(_mm_loadu_pd(axp+8), _mm_loadu_pd(xp+o3), r1);
    r0 = _mm256_fmadd_pd(_mm256_loadu_pd(axp+10), _mm256_maskload_pd(xp+o4, MASK_3), r0);

    *yp += hsum_add_double_avx(r0, r1);
    axi+=13;
    axp+=13;
}

inline void v5_3_5() {
    int o0 = axi[0], o1 = axi[5], o2 = axi[8];

    auto r0 = _mm256_setzero_pd();
    r0 = _mm256_fmadd_pd(_mm256_loadu_pd(axp), _mm256_loadu_pd(xp+o0), r0);
    double tail = axp[4] * xp[o0+4];
    r0 = _mm256_fmadd_pd(_mm256_loadu_pd(axp+5), _mm256_maskload_pd(xp+o1, MASK_3), r0);
    r0 = _mm256_fmadd_pd(_mm256_loadu_pd(axp+8), _mm256_loadu_pd(xp+o2), r0);
    tail += axp[12] * xp[o2+4];
    *yp += hsum_double_avx(r0) + tail;
    axp += 13;
    axi += 13;
}

void v4();

void v5();

inline void v5_3_5_3_5() {
    int o0 = axi[0], o1 = axi[5], o2 = axi[8], o3 = axi[13], o4 = axi[16];

    auto r0 = _mm256_setzero_pd();
    r0 = _mm256_fmadd_pd(_mm256_loadu_pd(axp), _mm256_loadu_pd(xp+o0), r0);
    double tail = axp[4] * xp[o0+4];
    r0 = _mm256_fmadd_pd(_mm256_loadu_pd(axp+5), _mm256_maskload_pd(xp+o1, MASK_3), r0);
    r0 = _mm256_fmadd_pd(_mm256_loadu_pd(axp+8), _mm256_loadu_pd(xp+o2), r0);
    tail += axp[12] * xp[o2+4];
    r0 = _mm256_fmadd_pd(_mm256_loadu_pd(axp+13), _mm256_maskload_pd(xp+o3, MASK_3), r0);
    r0 = _mm256_fmadd_pd(_mm256_loadu_pd(axp+16), _mm256_loadu_pd(xp+o4), r0);
    tail += axp[20] * xp[o4+4];
    *yp += hsum_double_avx(r0) + tail;

    axp += 21;
    axi += 21;
}

void v6();

void v7();

void v8();

void v9();

void v10();

void v11();

void v12();

void v13();

//template<int sRun, int... args>
//inline void ec() {
//    ec<sRun - 1>();
//    ec<args...>();
//}
//
//template<int sRun>
//inline void ec() {
//    if constexpr (sRun < 0) {
//        return;
//    }
//    if constexpr (sRun > 0) {
//        ec<sRun - 1>();
//    } else {
//        p1 = *axi;
//        *yp += axp[0] * xp[p1];
//    }
//    *yp += axp[sRun] * xp[p1 + sRun];
//}
//
//template<class none = void>
//inline void ec() {
//    p1 = *axi;
//    *yp += axp[0] * xp[p1];
//}
//
//template<int nOps>
//inline void vs();
//
//template<int nOps>
//inline void vs() {
//    *yp += *axp++ * xp[*axi++];
//    vs<nOps - 1>();
//}
//template<>
//inline void vs<0>() {}


inline void build_templates() {
//    mapped_fp = new FunctionPtr[256];
//    mapped_fp[0] = v0;
//    mapped_fp[1] = v1;
//    mapped_fp[2] = v2;
//    mapped_fp[3] = v3;
//    mapped_fp[4] = v4;
//    mapped_fp[5] = v5;
//    mapped_fp[6] = v6;
//    mapped_fp[7] = v7;
//    mapped_fp[8] = v8;
//    mapped_fp[9] = v9;
//    mapped_fp[10] = v10;
//    mapped_fp[11] = v11;
//    mapped_fp[12] = v12;
//    mapped_fp[13] = v13;
}

//void f0() { acc = acc ∗ 256 + arg; }
//void f1() { col = col + acc ∗ 256 + arg; acc = 0; *yp += *axp++ * xp[col]; col = col + 1; }
//void f2() { col = col + acc ∗ 256 + arg; acc = 0; *yp += *axp++ * xp[col]; col = col + 2; }
//void f3() { col = col + acc ∗ 256 + arg; acc = 0; *yp += *axp++ * xp[col]; col = col + 3; }
//void f4() { col = col + acc ∗ 256 + arg; acc = 0; *yp += *axp++ * xp[col]; col = col + 4; }
//void f5() { row = row + 1; col = 0; }


#endif//DDT_SERIALSPMVEXECUTOR_H
