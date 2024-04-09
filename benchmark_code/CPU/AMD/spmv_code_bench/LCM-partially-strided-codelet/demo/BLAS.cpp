#include "BLAS.h"

namespace custom_blas {
    void lsolve_BLAS(int dim, int num_col, double *M, double *b) {
        int k;
        double x0, x1, x2, x3, x4, x5, x6, x7;
        double *M0;
        register double *Mi0, *Mi1, *Mi2, *Mi3, *Mi4, *Mi5, *Mi6, *Mi7;

        register int col = 0;

        M0 = M;
        while (col < num_col - 7) {
            Mi0 = M0;
            Mi1 = Mi0 + dim + 1;
            Mi2 = Mi1 + dim + 1;
            Mi3 = Mi2 + dim + 1;
            Mi4 = Mi3 + dim + 1;
            Mi5 = Mi4 + dim + 1;
            Mi6 = Mi5 + dim + 1;
            Mi7 = Mi6 + dim + 1;

            x0 = b[col] / *Mi0++;
            x1 = (b[col + 1] - x0 * *Mi0++) / *Mi1++;
            x2 = (b[col + 2] - x0 * *Mi0++ - x1 * *Mi1++) / *Mi2++;
            x3 = (b[col + 3] - x0 * *Mi0++ - x1 * *Mi1++ - x2 * *Mi2++) / *Mi3++;
            x4 = (b[col + 4] - x0 * *Mi0++ - x1 * *Mi1++ - x2 * *Mi2++ - x3 * *Mi3++) /
                 *Mi4++;
            x5 = (b[col + 5] - x0 * *Mi0++ - x1 * *Mi1++ - x2 * *Mi2++ - x3 * *Mi3++ -
                  x4 * *Mi4++) / *Mi5++;
            x6 = (b[col + 6] - x0 * *Mi0++ - x1 * *Mi1++ - x2 * *Mi2++ - x3 * *Mi3++ -
                  x4 * *Mi4++ - x5 * *Mi5++) / *Mi6++;
            x7 = (b[col + 7] - x0 * *Mi0++ - x1 * *Mi1++ - x2 * *Mi2++ - x3 * *Mi3++ -
                  x4 * *Mi4++ - x5 * *Mi5++ -
                  x6 * *Mi6++) / *Mi7++;

            b[col++] = x0;
            b[col++] = x1;
            b[col++] = x2;
            b[col++] = x3;
            b[col++] = x4;
            b[col++] = x5;
            b[col++] = x6;
            b[col++] = x7;

            for (k = col; k < num_col; k++)
                b[k] = b[k] - x0 * *Mi0++ - x1 * *Mi1++ - x2 * *Mi2++ - x3 * *Mi3++
                       - x4 * *Mi4++ - x5 * *Mi5++ - x6 * *Mi6++ - x7 * *Mi7++;

            M0 += 8 * dim + 8;
        }

        while (col < num_col - 3) {
            Mi0 = M0;
            Mi1 = Mi0 + dim + 1;
            Mi2 = Mi1 + dim + 1;
            Mi3 = Mi2 + dim + 1;

            x0 = b[col] / *Mi0++;
            x1 = (b[col + 1] - x0 * *Mi0++) / *Mi1++;
            x2 = (b[col + 2] - x0 * *Mi0++ - x1 * *Mi1++) / *Mi2++;
            x3 = (b[col + 3] - x0 * *Mi0++ - x1 * *Mi1++ - x2 * *Mi2++) / *Mi3++;

            b[col++] = x0;
            b[col++] = x1;
            b[col++] = x2;
            b[col++] = x3;

            for (k = col; k < num_col; k++)
                b[k] = b[k] - x0 * *Mi0++ - x1 * *Mi1++ - x2 * *Mi2++ - x3 * *Mi3++;

            M0 += 4 * dim + 4;
        }

        if (col < num_col - 1) {
            Mi0 = M0;
            Mi1 = Mi0 + dim + 1;

            x0 = b[col] / *Mi0++;
            x1 = (b[col + 1] - x0 * *Mi0++) / *Mi1++;

            b[col++] = x0;
            b[col++] = x1;

            for (k = col; k < num_col; k++) {
                b[k] = b[k] - x0 * *Mi0++ - x1 * *Mi1++;
            }

            M0 += 2 * dim + 2;
        }

        if (col == num_col - 1) {
            Mi0 = M0;
            x0 = b[col] / *Mi0;
            b[col] = x0;
        }
    }

    void ltsolve_BLAS(int dim, int num_col, double *M, double *b) {
        int k;
        double x0, x1, x2, x3, x4, x5, x6, x7;
        double *M0, *M1, *M2, *M3, *M4, *M5, *M6, *M7;
        register double *Mi0, *Mi1, *Mi2, *Mi3, *Mi4, *Mi5, *Mi6, *Mi7;
        register int col = num_col - 1;

        M0 = M;
        while (col >= 7) {
            M1 = M0 - dim - 1;
            M2 = M1 - dim - 1;
            M3 = M2 - dim - 1;
            M4 = M3 - dim - 1;
            M5 = M4 - dim - 1;
            M6 = M5 - dim - 1;
            M7 = M6 - dim - 1;


            Mi0 = M0 + 1;
            Mi1 = M1 + 2;
            Mi2 = M2 + 3;
            Mi3 = M3 + 4;
            Mi4 = M4 + 5;
            Mi5 = M5 + 6;
            Mi6 = M6 + 7;
            Mi7 = M7 + 8;

            for (k = num_col - 1; k >= col + 1; k--) {
                b[col] -= *Mi0++ * b[k];
                b[col - 1] -= *Mi1++ * b[k];
                b[col - 2] -= *Mi2++ * b[k];
                b[col - 3] -= *Mi3++ * b[k];
                b[col - 4] -= *Mi4++ * b[k];
                b[col - 5] -= *Mi5++ * b[k];
                b[col - 6] -= *Mi6++ * b[k];
                b[col - 7] -= *Mi7++ * b[k];
            }

            x0 = b[col] / *M0;
            x1 = (b[col - 1] - x0 * *(M1 + 1)) / *M1;
            x2 = (b[col - 2] - x0 * *(M2 + 1) - x1 * *(M2 + 2)) / *M2;
            x3 = (b[col - 3] - x0 * *(M3 + 1) - x1 * *(M3 + 2) - x2 * *(M3 + 3)) / *M3;
            x4 = (b[col - 4] - x0 * *(M4 + 1) - x1 * *(M4 + 2) - x2 * *(M4 + 3) -
                  x3 * *(M4 + 4)) / *M4;
            x5 = (b[col - 5] - x0 * *(M5 + 1) - x1 * *(M5 + 2) - x2 * *(M5 + 3) -
                  x3 * *(M5 + 4) - x4 * *(M5 + 5)) / *M5;
            x6 = (b[col - 6] - x0 * *(M6 + 1) - x1 * *(M6 + 2) - x2 * *(M6 + 3) -
                  x3 * *(M6 + 4) - x4 * *(M6 + 5) - x5 * *(M6 + 6)) / *M6;
            x7 = (b[col - 7] - x0 * *(M7 + 1) - x1 * *(M7 + 2) - x2 * *(M7 + 3) -
                  x3 * *(M7 + 4) - x4 * *(M7 + 5) - x5 * *(M7 + 6) - x6 * *(M7 + 7)) /
                 *M7;

            b[col--] = x0;
            b[col--] = x1;
            b[col--] = x2;
            b[col--] = x3;
            b[col--] = x4;
            b[col--] = x5;
            b[col--] = x6;
            b[col--] = x7;

            M0 = M0 - 8 * dim - 8;
        }

        while (col >= 3) {
            M1 = M0 - dim - 1;
            M2 = M1 - dim - 1;
            M3 = M2 - dim - 1;

            Mi0 = M0 + 1;
            Mi1 = M1 + 2;
            Mi2 = M2 + 3;
            Mi3 = M3 + 4;

            for (k = num_col - 1; k >= col + 1; k--) {
                b[col] -= *Mi0++ * b[k];
                b[col - 1] -= *Mi1++ * b[k];
                b[col - 2] -= *Mi2++ * b[k];
                b[col - 3] -= *Mi3++ * b[k];
            }

            x0 = b[col] / *M0;
            x1 = (b[col - 1] - x0 * *(M1 + 1)) / *M1;
            x2 = (b[col - 2] - x0 * *(M2 + 1) - x1 * *(M2 + 2)) / *M2;
            x3 = (b[col - 3] - x0 * *(M3 + 1) - x1 * *(M3 + 2) - x2 * *(M3 + 3)) / *M3;

            b[col--] = x0;
            b[col--] = x1;
            b[col--] = x2;
            b[col--] = x3;

            M0 = M0 - 4 * dim - 4;
        }

        while (col >= 1) {
            M1 = M0 - dim - 1;
            Mi0 = M0 + 1;
            Mi1 = M1 + 2;

            for (k = num_col - 1; k >= col + 1; k--) {
                b[col] -= *Mi0++ * b[k];
                b[col - 1] -= *Mi1++ * b[k];
            }

            x0 = b[col] / *M0;
            x1 = (b[col - 1] - x0 * *(M1 + 1)) / *M1;

            b[col--] = x0;
            b[col--] = x1;

            M0 = M0 - 2 * dim - 2;
        }

        if (col == 0) {
            Mi0 = M0 + 1;
            for (k = num_col - 1; k >= 1; k--) {
                b[col] -= *Mi0++ * b[k];
            }

            x0 = b[col] / *M0;
            b[col--] = x0;

            M0 = M0 - dim - 1;
        }
    }

    void
    mattvec_BLAS(int dim, int nrow, int ncol, double *M, double *x, double *b) {
        double *M0 = M;
        register double *Mi0, *Mi1, *Mi2, *Mi3, *Mi4, *Mi5, *Mi6, *Mi7;
        register int col = 0;

        int k;

        while (col < ncol - 7) {
            Mi0 = M0;
            Mi1 = Mi0 + dim;
            Mi2 = Mi1 + dim;
            Mi3 = Mi2 + dim;
            Mi4 = Mi3 + dim;
            Mi5 = Mi4 + dim;
            Mi6 = Mi5 + dim;
            Mi7 = Mi6 + dim;

            for (k = 0; k < nrow; k++) {
                b[col] += *Mi0++ * x[k];
                b[col + 1] += *Mi1++ * x[k];
                b[col + 2] += *Mi2++ * x[k];
                b[col + 3] += *Mi3++ * x[k];
                b[col + 4] += *Mi4++ * x[k];
                b[col + 5] += *Mi5++ * x[k];
                b[col + 6] += *Mi6++ * x[k];
                b[col + 7] += *Mi7++ * x[k];
            }

            M0 += 8 * dim;
            col += 8;
        }

        while (col < ncol - 3) {
            Mi0 = M0;
            Mi1 = Mi0 + dim;
            Mi2 = Mi1 + dim;
            Mi3 = Mi2 + dim;

            for (k = 0; k < nrow; k++) {
                b[col] += *Mi0++ * x[k];
                b[col + 1] += *Mi1++ * x[k];
                b[col + 2] += *Mi2++ * x[k];
                b[col + 3] += *Mi3++ * x[k];
            }

            M0 += 4 * dim;
            col += 4;
        }

        while (col < ncol) {
            Mi0 = M0;
            for (k = 0; k < nrow; k++) {
                double tmp = *Mi0++ * x[k];

                b[col] += tmp;
            }
            M0 += dim;
            col++;
        }
    }

    void matvec_BLAS(int dim, int nrow, int ncol, double *M, double *x, double *b) {
        double x0, x1, x2, x3, x4, x5, x6, x7;
        double *M0 = M;
        register double *Mi0, *Mi1, *Mi2, *Mi3, *Mi4, *Mi5, *Mi6, *Mi7;
        register int col = 0;

        int k;

        while (col < ncol - 7) {
            Mi0 = M0;
            Mi1 = Mi0 + dim;
            Mi2 = Mi1 + dim;
            Mi3 = Mi2 + dim;
            Mi4 = Mi3 + dim;
            Mi5 = Mi4 + dim;
            Mi6 = Mi5 + dim;
            Mi7 = Mi6 + dim;

            x0 = x[col++];
            x1 = x[col++];
            x2 = x[col++];
            x3 = x[col++];
            x4 = x[col++];
            x5 = x[col++];
            x6 = x[col++];
            x7 = x[col++];

            for (k = 0; k < nrow; k++) {
                b[k] += x0 * *Mi0++ + x1 * *Mi1++ + x2 * *Mi2++ + x3 * *Mi3++
                        + x4 * *Mi4++ + x5 * *Mi5++ + x6 * *Mi6++ + x7 * *Mi7++;
            }

            M0 += 8 * dim;
        }

        while (col < ncol - 3) {
            Mi0 = M0;
            Mi1 = Mi0 + dim;
            Mi2 = Mi1 + dim;
            Mi3 = Mi2 + dim;

            x0 = x[col++];
            x1 = x[col++];
            x2 = x[col++];
            x3 = x[col++];

            for (k = 0; k < nrow; k++) {
                b[k] += x0 * *Mi0++ + x1 * *Mi1++ + x2 * *Mi2++ + x3 * *Mi3++;
            }

            M0 += 4 * dim;
        }

        while (col < ncol) {
            Mi0 = M0;
            x0 = x[col++];

            for (k = 0; k < nrow; k++)
                b[k] += x0 * *Mi0++;

            M0 += dim;
        }
    }


    double DDOT(int n, double *v0, double *v1) {
        register int i = 0;
        register double sum = 0.0;
        register double *M0 = v0;
        register double *M1 = v1;

        while (i < n - 4) {
            sum += *M0++ * *M1++ + *M0++ * *M1++ + *M0++ * *M1++ + *M0++ * *M1++;
            i += 4;
        }

        while (i < n) {
            sum += *M0++ * *M1++;
            i++;
        }
        return sum;
    }


// TODO: make this fast currently column majored
    void DGEMM(int N, int M, int K, const double *A, const double *B, double *C) {
        // A is N * M
        // B is M * K
        for (int i = 0; i < N; i++) {
            for (int k = 0; k < K; k++) {
                for (int j = 0; j < M; j++) {
                    C[i + k * N] += A[i + j * N] * B[j + k * M];
                }
            }
        }
    }


    void GEMV(int N, int M, const double *A, const double *x, double *b) {
        for (int i = 0; i < M; i++) {
            for (int j = 0; j < N; j++) {
                b[j] += A[j + i * N] * x[i];
            }
        }
    }


// TODO: verify
    void DAXPY(int n, double a, double *x, double *y, double *z) {
        register int i = 0;
        register double *M0 = x;
        register double *M1 = y;
        register double *M2 = z;

        while (i < n - 8) {
            M2[0] = a * *M0++ + *M1++;
            M2[1] = a * *M0++ + *M1++;
            M2[2] = a * *M0++ + *M1++;
            M2[3] = a * *M0++ + *M1++;
            M2[4] = a * *M0++ + *M1++;
            M2[5] = a * *M0++ + *M1++;
            M2[6] = a * *M0++ + *M1++;
            M2[7] = a * *M0++ + *M1++;
            M2 += 8;
            i += 8;
        }

        while (i < n - 4) {
            M2[0] = a * *M0++ + *M1++;
            M2[1] = a * *M0++ + *M1++;
            M2[2] = a * *M0++ + *M1++;
            M2[3] = a * *M0++ + *M1++;
            M2 += 4;
            i += 4;
        }

        while (i < n - 2) {
            M2[0] = a * *M0++ + *M1++;
            M2[1] = a * *M0++ + *M1++;
            M2 += 2;
            i += 2;
        }

        while (i < n) {
            M2[0] = a * *M0++ + *M1++;
            M2++;
            i++;
        }
    }


    void AtA(int N, int M, const double *A, double *C) {
        // resulting matrix is MxM
        for (int i = 0; i < M; i++) {
            for (int j = 0; j < M; j++) {
                for (int k = 0; k < N; k++) {
                    C[i + j * M] += A[k + i * N] * A[k + j * N];
                }
            }
        }
    }


    inline double hsum_double_avx(__m256d v) {
        __m128d vlow  = _mm256_castpd256_pd128(v);
        __m128d vhigh = _mm256_extractf128_pd(v, 1); // high 128
        vlow  = _mm_add_pd(vlow, vhigh);     // reduce down to 128

        __m128d high64 = _mm_unpackhi_pd(vlow, vlow);
        return  _mm_cvtsd_f64(_mm_add_sd(vlow, high64));  // reduce to scalar
    }

    void SpTrSv_MatVecCSR_BLAS(int nrow, int ncol, double *M, double *x, double *b) {
        int r = 0;
        for (; r < (nrow - 3); r+=4) {
            auto Mi0 = M;
            auto Mi1 = Mi0 + ncol + r + 1;
            auto Mi2 = Mi1 + ncol + r + 2;
            auto Mi3 = Mi2 + ncol + r + 3;
            M = Mi3 + ncol + r + 4;
            auto r0 = _mm256_setzero_pd();
            auto r1 = _mm256_setzero_pd();
            auto r2 = _mm256_setzero_pd();
            auto r3 = _mm256_setzero_pd();
            int j = 0;
            // Compute 4 rows and 4 cols at a time
            for (; j < ncol - 3; j += 4) {
                // Load vector
                auto v0 = _mm256_loadu_pd(x + j);
                // Load matrix
                auto c0 = _mm256_loadu_pd(Mi0);
                Mi0+=4;
                auto c1 = _mm256_loadu_pd(Mi1);
                Mi1+=4;
                auto c2 = _mm256_loadu_pd(Mi2);
                Mi2+=4;
                auto c3 = _mm256_loadu_pd(Mi3);
                Mi3+=4;
                // Perform compute
                r0 = _mm256_fmadd_pd(c0, v0, r0);
                r1 = _mm256_fmadd_pd(c1, v0, r1);
                r2 = _mm256_fmadd_pd(c2, v0, r2);
                r3 = _mm256_fmadd_pd(c3, v0, r3);
            }
            // Reduce
            auto b0 = hsum_double_avx(r0);
            auto b1 = hsum_double_avx(r1);
            auto b2 = hsum_double_avx(r2);
            auto b3 = hsum_double_avx(r3);

            // Perform remaining
            for (; j < ncol; j++) {
                b0 += *Mi0++ * x[j];
                b1 += *Mi1++ * x[j];
                b2 += *Mi2++ * x[j];
                b3 += *Mi3++ * x[j];
            }

            // Store
            b[r] -= b0;
            b[r + 1] -= b1;
            b[r + 2] -= b2;
            b[r + 3] -= b3;
        }
        //Remaining rows
        for (; r < nrow; r++) {
            for (int col = 0; col < ncol; col++) {
                b[r] -= M[col] * x[col];
            }
            M = M + ncol + r + 1;
        }
    }

    void SpTrSv_LSolveCSR_BLAS(int nrow, int ncol, double *M, double *b){
        int r = 0;
        while (r < nrow - 7) {
            auto Mi0 = M;
            auto Mi1 = Mi0 + ncol + r + 1;
            auto Mi2 = Mi1 + ncol + r + 2;
            auto Mi3 = Mi2 + ncol + r + 3;
            auto Mi4 = Mi3 + ncol + r + 4;
            auto Mi5 = Mi4 + ncol + r + 5;
            auto Mi6 = Mi5 + ncol + r + 6;
            auto Mi7 = Mi6 + ncol + r + 7;

            auto x0 = b[r] / Mi0[0];
            auto x1 = (b[r + 1] - x0 * Mi1[0]) / Mi1[1];
            auto x2 = (b[r + 2] - x0 * Mi2[0] - x1 * Mi2[1]) / Mi2[2];
            auto x3 = (b[r + 3] - x0 * Mi3[0] - x1 * Mi3[1] - x2 * Mi3[2]) / Mi3[3];
            auto x4 = (b[r + 4] - x0 * Mi4[0] - x1 * Mi4[1] - x2 * Mi4[2] - x3 * Mi4[3]) / Mi4[4];
            auto x5 = (b[r + 5] - x0 * Mi5[0] - x1 * Mi5[1] - x2 * Mi5[2] -
                       x3 * Mi5[3] - x4 * Mi5[4]) / Mi5[5];
            auto x6 = (b[r + 6] - x0 * Mi6[0] - x1 * Mi6[1] - x2 * Mi6[2] -
                       x3 * Mi6[3] - x4 * Mi6[4] - x5 * Mi6[5]) / *(Mi6 + 6);
            auto x7 = (b[r + 7] - x0 * Mi7[0] - x1 * Mi7[1] - x2 * Mi7[2] - x3 * Mi7[3] -
                       x4 * Mi7[4] - x5 * Mi7[5] - x6 * Mi7[6]) / Mi7[7];

            b[r++] = x0;
            b[r++] = x1;
            b[r++] = x2;
            b[r++] = x3;
            b[r++] = x4;
            b[r++] = x5;
            b[r++] = x6;
            b[r++] = x7;
            double* MiR = Mi7;
            for (int k = r; k < nrow; k++){
                MiR = MiR + ncol + k;
                b[k] = (b[k] - x0 * MiR[0] - x1 * MiR[1] - x2 * MiR[2] - x3 * MiR[3] -
                        x4 * MiR[4] - x5 * MiR[5] - x6 * MiR[6] - x7 * MiR[7]);
            }
            M = Mi7 + ncol + r + 8;
        }

        while (r < nrow - 3) {
            auto Mi0 = M;
            auto Mi1 = Mi0 + ncol + r + 1;
            auto Mi2 = Mi1 + ncol + r + 2;
            auto Mi3 = Mi2 + ncol + r + 3;
            // The window of dependence solve should move by 4
            auto x0 = b[r] / *Mi0;
            auto x1 = (b[r + 1] - x0 * Mi1[0]) / Mi1[1];
            auto x2 = (b[r + 2] - x0 * Mi2[0] - x1 * Mi2[1]) / Mi2[2];
            auto x3 = (b[r + 3] - x0 * Mi3[0] - x1 * Mi3[1] - x2 * Mi3[2]) / Mi3[3];

            b[r++] = x0;
            b[r++] = x1;
            b[r++] = x2;
            b[r++] = x3;
            double* MiR = Mi3;
            for (int k = r; k < nrow; k++){
                MiR = MiR + ncol + k;
                b[k] = b[k] - x0 * MiR[0] - x1 * MiR[1] - x2 * MiR[2] - x3 * MiR[3];
            }
            M = Mi3 + ncol + r + 4;
        }

        if (r < nrow - 1){
            auto Mi0 = M;
            auto Mi1 = Mi0 + ncol + r + 1;

            // The window of dependence solve should move by 2
            auto x0 = b[r] / Mi0[0];
            auto x1 = (b[r + 1] - x0 * Mi1[0]) / Mi1[1];
            b[r++] = x0;
            b[r++] = x1;

            double* MiR = Mi1;
            for (int k = r; k < nrow; k++){
                MiR = MiR + ncol + k;
                b[k] = b[k] - x0 * MiR[0] - x1 * MiR[1];
            }
            M = Mi1 + ncol + r + 2;
        }

        if (r == nrow - 1) {
            auto Mi0 = M;
            auto x0 = b[r] / *Mi0;
            b[r] = x0;
        }
    }

    void SpTrSv_MatVecCSC_BLAS(int nrow, int ncol, double *M, double *x, double *b) {
        int c = 0;
//        while (c < ncol - 7) {
//            auto Mi0 = M;
//            auto Mi1 = Mi0 + nrow + ncol - c - 1;
//            auto Mi2 = Mi1 + nrow + ncol - c - 2;
//            auto Mi3 = Mi2 + nrow + ncol - c - 3;
//            auto Mi4 = Mi3 + nrow + ncol - c - 4;
//            auto Mi5 = Mi4 + nrow + ncol - c - 5;
//            auto Mi6 = Mi5 + nrow + ncol - c - 6;
//            auto Mi7 = Mi6 + nrow + ncol - c - 7;
//            M = Mi7 + nrow + ncol - c - 8;
//
//           auto x0 = x[c++];
//           auto x1 = x[c++];
//           auto x2 = x[c++];
//           auto x3 = x[c++];
//           auto x4 = x[c++];
//           auto x5 = x[c++];
//           auto x6 = x[c++];
//           auto x7 = x[c++];
//
//            for (int k = 0; k < nrow; k++) {
//                b[k] += x0 * *Mi0++ + x1 * *Mi1++ + x2 * *Mi2++ + x3 * *Mi3++
//                        + x4 * *Mi4++ + x5 * *Mi5++ + x6 * *Mi6++ + x7 * *Mi7++;
//            }
//        }
//
//        while (c < ncol - 3) {
//            auto Mi0 = M;
//            auto Mi1 = Mi0 + nrow + ncol - c - 1;
//            auto Mi2 = Mi1 + nrow + ncol - c - 2;
//            auto Mi3 = Mi2 + nrow + ncol - c - 3;
//            M = Mi3 + nrow + ncol - c - 4;
//            auto x0 = x[c++];
//            auto x1 = x[c++];
//            auto x2 = x[c++];
//            auto x3 = x[c++];
//
//            for (int k = 0; k < nrow; k++) {
//                b[k] += x0 * *Mi0++ + x1 * *Mi1++ + x2 * *Mi2++ + x3 * *Mi3++;
//            }
//        }

        while (c < ncol) {
            auto Mi0 = M;
            M = Mi0 + nrow + ncol - c - 1;
            auto x0 = x[c++];
            for (int k = 0; k < nrow; k++)
                b[k] += x0 * *Mi0++;
        }
    }

    void SpTrSv_LSolveCSC_BLAS(int nrow, int ncol, double *M, double *b){
        int c = 0;
        while (c < ncol - 7) {
            auto Mi0 = M;
            auto Mi1 = Mi0 + nrow + ncol - c;
            auto Mi2 = Mi1 + nrow + ncol - c - 1;
            auto Mi3 = Mi2 + nrow + ncol - c - 2;
            auto Mi4 = Mi3 + nrow + ncol - c - 3;
            auto Mi5 = Mi4 + nrow + ncol - c - 4;
            auto Mi6 = Mi5 + nrow + ncol - c - 5;
            auto Mi7 = Mi6 + nrow + ncol - c - 6;
            M = Mi7 + nrow + ncol - c - 7;
            auto x0 = b[c] / *Mi0++;
            auto x1 = (b[c + 1] - x0 * *Mi0++) / *Mi1++;
            auto x2 = (b[c + 2] - x0 * *Mi0++ - x1 * *Mi1++) / *Mi2++;
            auto x3 = (b[c + 3] - x0 * *Mi0++ - x1 * *Mi1++ - x2 * *Mi2++) / *Mi3++;
            auto x4 = (b[c + 4] - x0 * *Mi0++ - x1 * *Mi1++ - x2 * *Mi2++ - x3 * *Mi3++) / *Mi4++;
            auto x5 = (b[c + 5] - x0 * *Mi0++ - x1 * *Mi1++ - x2 * *Mi2++ - x3 * *Mi3++ - x4 * *Mi4++) / *Mi5++;
            auto x6 = (b[c + 6] - x0 * *Mi0++ - x1 * *Mi1++ - x2 * *Mi2++ - x3 * *Mi3++ - x4 * *Mi4++ - x5 * *Mi5++) / *Mi6++;
            auto x7 = (b[c + 7] - x0 * *Mi0++ - x1 * *Mi1++ - x2 * *Mi2++ - x3 * *Mi3++ - x4 * *Mi4++ - x5 * *Mi5++ - x6 * *Mi6++) / *Mi7++;

            b[c++] = x0;
            b[c++] = x1;
            b[c++] = x2;
            b[c++] = x3;
            b[c++] = x4;
            b[c++] = x5;
            b[c++] = x6;
            b[c++] = x7;
            for (int k = c; k < ncol; k++){
                b[k] = b[k] - x0 * *Mi0++ - x1 * *Mi1++ - x2 * *Mi2++ - x3 * *Mi3++ -
                        x4 * *Mi4++ - x5 * *Mi5++ - x6 * *Mi6++ - x7 * *Mi7++;
            }

        }

        while (c < ncol - 3) {
            auto Mi0 = M;
            auto Mi1 = Mi0 + nrow + ncol - c;
            auto Mi2 = Mi1 + nrow + ncol - c - 1;
            auto Mi3 = Mi2 + nrow + ncol - c - 2;
            M = Mi3 + nrow + ncol - c - 3;

            auto x0 = b[c] / Mi0[0];
            auto x1 = (b[c + 1] - x0 * *Mi0++) / *Mi1++;
            auto x2 = (b[c + 2] - x0 * *Mi0++ - x1 * *Mi1++) / *Mi2++;
            auto x3 = (b[c + 3] - x0 * *Mi0++ - x1 * *Mi1++ - x2 * *Mi2++) / *Mi3++;

            b[c++] = x0;
            b[c++] = x1;
            b[c++] = x2;
            b[c++] = x3;

            for (int k = c; k < ncol; k++){
                b[k] = b[k] - x0 * *Mi0++ - x1 * *Mi1++ - x2 * *Mi2++ - x3 * *Mi3++;
            }

        }

        if (c < ncol - 1) {
            auto Mi0 = M;
            auto Mi1 = Mi0 + nrow + ncol - c;
            M = Mi1 + nrow + ncol - c - 1;
            auto x0 = b[c] / *Mi0++;
            auto x1 = (b[c + 1] - x0 * *Mi0++) / *Mi1++;

            b[c++] = x0;
            b[c++] = x1;

            for (int k = c; k < ncol; k++) {
                b[k] = b[k] - x0 * *Mi0++ - x1 * *Mi1++;
            }
        }

        if (c == ncol - 1) {
            b[c] = b[c] / *M;
        }
    }


}