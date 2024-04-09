//
// Created by cetinicz on 2021-08-15.
//

#ifndef DDT_GENERICKERNELS_H
#define DDT_GENERICKERNELS_H

#include <iostream>

inline void spmv_csr_baseline(int n, const int* Ai, const int* Ap, const double* Ax, const double*x, double*y) {
    for (int i = 0; i < n; ++i) {
        for (int j = Ap[i]; j < Ap[i+1]; ++j) {
            y[i] += Ax[j] * x[Ai[j]];
        }
    }
}

inline void spmv_csc_baseline(int n, const int* Ai, const int* Ap, const double* Ax, const double*x, double*y) {
    for (int i = 0; i < n; ++i) {
        for (int j = Ap[i]; j < Ap[i+1]; ++j) {
            y[Ai[j]] += Ax[j] * x[i];
        }
    }
}

inline bool verifyVector(int n, double* y, double* correct_y) {
    for (int i = 0; i < n; ++i) {
        if (std::abs(y[i] - correct_y[i]) > 0.0001) {
            std::cout << i << ": " << y[i] << "," << correct_y[i] << std::endl;
            return false;
        }
    }
    return true;
}

#endif//DDT_GENERICKERNELS_H
