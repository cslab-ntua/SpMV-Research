//
// Created by cetinicz on 2021-08-02.
//

#include "SerialSpMVExecutor.h"
#include <immintrin.h>

inline double hsum_double_avx(__m256d v, __m128d tail) {
    __m128d vlow = _mm256_castpd256_pd128(v);
    __m128d vhigh = _mm256_extractf128_pd(v, 1);  // high 128
    vlow = _mm_add_pd(vlow, vhigh);      // reduce down to 128
    vlow = _mm_add_pd(vlow, tail);      // reduce down to 128

    __m128d high64 = _mm_unpackhi_pd(vlow, vlow);
    return _mm_cvtsd_f64(_mm_add_sd(vlow, high64));  // reduce to scalar
}

    void v1() { *yp += *axp++ * xp[*axi++]; }

    void v2() {
        *yp += *axp++ * xp[*axi++];
        *yp += *axp++ * xp[*axi++];
    }

    void v3() {
        *yp += *axp++ * xp[*axi++];
        *yp += *axp++ * xp[*axi++];
        *yp += *axp++ * xp[*axi++];
    }

    void v4() {
        *yp += *axp++ * xp[*axi++];
        *yp += *axp++ * xp[*axi++];
        *yp += *axp++ * xp[*axi++];
        *yp += *axp++ * xp[*axi++];
    }

    void v5() {
        *yp += *axp++ * xp[*axi++];
        *yp += *axp++ * xp[*axi++];
        *yp += *axp++ * xp[*axi++];
        *yp += *axp++ * xp[*axi++];
        *yp += *axp++ * xp[*axi++];
    }

    void v6() {
        *yp += *axp++ * xp[*axi++];
        *yp += *axp++ * xp[*axi++];
        *yp += *axp++ * xp[*axi++];
        *yp += *axp++ * xp[*axi++];
        *yp += *axp++ * xp[*axi++];
        *yp += *axp++ * xp[*axi++];
    }

    void v7() {
        *yp += *axp++ * xp[*axi++];
        *yp += *axp++ * xp[*axi++];
        *yp += *axp++ * xp[*axi++];
        *yp += *axp++ * xp[*axi++];
        *yp += *axp++ * xp[*axi++];
        *yp += *axp++ * xp[*axi++];
        *yp += *axp++ * xp[*axi++];
    }

    void v8() {
        *yp += *axp++ * xp[*axi++];
        *yp += *axp++ * xp[*axi++];
        *yp += *axp++ * xp[*axi++];
        *yp += *axp++ * xp[*axi++];
        *yp += *axp++ * xp[*axi++];
        *yp += *axp++ * xp[*axi++];
        *yp += *axp++ * xp[*axi++];
        *yp += *axp++ * xp[*axi++];
    }

    void v9() {
        *yp += *axp++ * xp[*axi++];
        *yp += *axp++ * xp[*axi++];
        *yp += *axp++ * xp[*axi++];
        *yp += *axp++ * xp[*axi++];
        *yp += *axp++ * xp[*axi++];
        *yp += *axp++ * xp[*axi++];
        *yp += *axp++ * xp[*axi++];
        *yp += *axp++ * xp[*axi++];
        *yp += *axp++ * xp[*axi++];
    }

    void v10() {
        *yp += *axp++ * xp[*axi++];
        *yp += *axp++ * xp[*axi++];
        *yp += *axp++ * xp[*axi++];
        *yp += *axp++ * xp[*axi++];
        *yp += *axp++ * xp[*axi++];
        *yp += *axp++ * xp[*axi++];
        *yp += *axp++ * xp[*axi++];
        *yp += *axp++ * xp[*axi++];
        *yp += *axp++ * xp[*axi++];
        *yp += *axp++ * xp[*axi++];
    }

    void v11() {
        *yp += *axp++ * xp[*axi++];
        *yp += *axp++ * xp[*axi++];
        *yp += *axp++ * xp[*axi++];
        *yp += *axp++ * xp[*axi++];
        *yp += *axp++ * xp[*axi++];
        *yp += *axp++ * xp[*axi++];
        *yp += *axp++ * xp[*axi++];
        *yp += *axp++ * xp[*axi++];
        *yp += *axp++ * xp[*axi++];
        *yp += *axp++ * xp[*axi++];
        *yp += *axp++ * xp[*axi++];
    }

    void v12() {
        *yp += *axp++ * xp[*axi++];
        *yp += *axp++ * xp[*axi++];
        *yp += *axp++ * xp[*axi++];
        *yp += *axp++ * xp[*axi++];
        *yp += *axp++ * xp[*axi++];
        *yp += *axp++ * xp[*axi++];
        *yp += *axp++ * xp[*axi++];
        *yp += *axp++ * xp[*axi++];
        *yp += *axp++ * xp[*axi++];
        *yp += *axp++ * xp[*axi++];
        *yp += *axp++ * xp[*axi++];
        *yp += *axp++ * xp[*axi++];
    }

    void v13() {
        *yp += *axp++ * xp[*axi++];
        *yp += *axp++ * xp[*axi++];
        *yp += *axp++ * xp[*axi++];
        *yp += *axp++ * xp[*axi++];
        *yp += *axp++ * xp[*axi++];
        *yp += *axp++ * xp[*axi++];
        *yp += *axp++ * xp[*axi++];
        *yp += *axp++ * xp[*axi++];
        *yp += *axp++ * xp[*axi++];
        *yp += *axp++ * xp[*axi++];
        *yp += *axp++ * xp[*axi++];
        *yp += *axp++ * xp[*axi++];
        *yp += *axp++ * xp[*axi++];
    }




