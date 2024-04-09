//
// Created by cetinicz on 2021-10-30.
//

#ifndef DDT_SPMM_DEMO_UTILS_H
#define DDT_SPMM_DEMO_UTILS_H

#include <unordered_map>

#include "FusionDemo.h"
#include <DDT.h>
#include <DDTCodelets.h>
#include <Executor.h>

#include <math.h>

#ifdef ORTOOLS
#include "ortools/constraint_solver/routing.h"
#include "ortools/constraint_solver/routing_enums.pb.h"
#include "ortools/constraint_solver/routing_index_manager.h"
#include "ortools/constraint_solver/routing_parameters.h"
#endif

#ifdef MKL
#include <cmath>
#include <mkl.h>
#endif

#define loadmx(v,ai,o) \
_mm256_set_pd(v[ai[3]+o],v[ai[2]+o],v[ai[1]+o],v[ai[0]+o]);

namespace sparse_avx {
    void spmm_csr_csr_4x4_kernel(double* Cx, double* Ax, double* Bx, const int* Ai, int bCols);
    void spmm_csr_csc(double* Cx, double* Ax, double* Bx, int m, int* Ap, int* Ai, int bRows, int bCols) {
        for (int i = 0; i < m; ++i) {
            for (int k = 0; k < bCols; ++k) {
                for (int j = Ap[i]; j < Ap[i+1]; ++j) {
                    Cx[i+k*bRows] += Ax[j] * Bx[Ai[j]+k*bRows];
                }
            }
        }
    }

    void spmm_csr_csr(double* Cx, double* Ax, double* Bx, int m, int* Ap, int* Ai, int bRows, int bCols) {
#pragma omp parallel for
        for (int i = 0; i < m; ++i) {
             for (int j = Ap[i]; j < Ap[i+1]; ++j) {
                   for (int k = 0; k < bCols; ++k) {
                        Cx[i*bCols+k] += Ax[j] * Bx[Ai[j]*bCols+k];
                }
            }
        }
    }

    void spmm_csr_csc_parallel(double* Cx, double* Ax, double* Bx, int m, int* Ap, int* Ai, int bRows, int bCols, int nThreads) {
#pragma omp parallel for num_threads(nThreads)
        for (int i = 0; i < m; ++i) {
            for (int k = 0; k < bCols; ++k) {
                for (int j = Ap[i]; j < Ap[i+1]; ++j) {
                    Cx[i+k*bRows] += Ax[j] * Bx[Ai[j]+k*bRows];
                }
            }
        }
    }

    void spmm_csr_csr_parallel(double* Cx, double* Ax, double* Bx, int m, int* Ap, int* Ai, int bRows, int bCols, int nThreads) {
#pragma omp parallel for num_threads(nThreads)
        for (int i = 0; i < m; ++i) {
            for (int j = Ap[i]; j < Ap[i+1]; ++j) {
                for (int k = 0; k < bCols; ++k) {
                    Cx[i*bCols+k] += Ax[j] * Bx[Ai[j]*bCols+k];
                }
            }
        }
    }

    void spmm_csr_csr_2_4_parallel(double* Cx, double* Ax, double* Bx, int m, int* Ap, int* Ai, int bRows, int bCols, int nThreads) {
#pragma omp parallel for num_threads(nThreads)
        for (int i = 0; i < m-1; i+=2) {
            int j = Ap[i];
            int jj = Ap[i+1];

            auto cx0 = _mm256_setzero_pd();
            auto cx1 = _mm256_setzero_pd();
            for (; j < Ap[i+1]-3; j+=4) {
                auto a = _mm256_loadu_pd(Ax+j);

                auto bx0 = _mm256_load_pd(Bx+Ai[j]*bCols);
                auto av0 = _mm256_permute4x64_pd(a,0b00000000);
                auto bx1 = _mm256_load_pd(Bx+Ai[j+1]*bCols);
                auto av1 = _mm256_permute4x64_pd(a,0b01010101);
                auto bx2 = _mm256_load_pd(Bx+Ai[j+2]*bCols);
                auto av2 = _mm256_permute4x64_pd(a,0b10101010);
                auto bx3 = _mm256_load_pd(Bx+Ai[j+3]*bCols);
                auto av3 = _mm256_permute4x64_pd(a,0b11111111);

                cx0 = _mm256_fmadd_pd(av0,bx0,cx0);
                cx0 = _mm256_fmadd_pd(av1,bx1,cx0);
                cx0 = _mm256_fmadd_pd(av2,bx2,cx0);
                cx0 = _mm256_fmadd_pd(av3,bx3,cx0);
            }
            for (; j < Ap[i+1]; ++j) {
                auto ax0 = _mm256_set1_pd(Ax[j]);
                auto bx0 = _mm256_load_pd(Bx+Ai[j]*bCols);
                cx0 = _mm256_fmadd_pd(ax0,bx0,cx0);
            }
            for (; jj < Ap[i+2]-3; jj+=4) {
                auto a = _mm256_loadu_pd(Ax+jj);

                auto bx0 = _mm256_load_pd(Bx+Ai[jj]*bCols);
                auto av0 = _mm256_permute4x64_pd(a,0b00000000);
                auto bx1 = _mm256_load_pd(Bx+Ai[jj+1]*bCols);
                auto av1 = _mm256_permute4x64_pd(a,0b01010101);
                auto bx2 = _mm256_load_pd(Bx+Ai[jj+2]*bCols);
                auto av2 = _mm256_permute4x64_pd(a,0b10101010);
                auto bx3 = _mm256_load_pd(Bx+Ai[jj+3]*bCols);
                auto av3 = _mm256_permute4x64_pd(a,0b11111111);

                cx1 = _mm256_fmadd_pd(av0,bx0,cx1);
                cx1 = _mm256_fmadd_pd(av1,bx1,cx1);
                cx1 = _mm256_fmadd_pd(av2,bx2,cx1);
                cx1 = _mm256_fmadd_pd(av3,bx3,cx1);
            }
            for (; jj < Ap[i+2]; ++jj) {
                auto ax0 = _mm256_set1_pd(Ax[jj]);
                auto bx0 = _mm256_load_pd(Bx+Ai[jj]*bCols);
                cx1 = _mm256_fmadd_pd(ax0,bx0,cx1);
            }
            _mm256_store_pd(Cx+i*bCols,cx0);
            _mm256_store_pd(Cx+i*bCols+bCols,cx1);
        }
    }

    // 12x4
    void spmm_csr_csr_4_parallel(double* Cx, const double* Ax, const double* Bx, int m, const int* Ap, const int* Ai, int bRows, int bCols, int nThreads) {
//#pragma omp parallel for num_threads(nThreads)

// 12 vectors for output
// 3  vectors for broad casts
// 1  vector for the loads
        for (int i = 0; i < m; ++i) {
            int j = Ap[i];
            auto cx0 = _mm256_setzero_pd();
            for (; j < Ap[i+1]-3; j+=4) {
                auto a = _mm256_castsi256_pd(_mm256_lddqu_si256(
                        reinterpret_cast<const __m256i *>(Ax + j)));

                auto bx0 = _mm256_load_pd(Bx+Ai[j]*bCols);
                auto av0 = _mm256_permute4x64_pd(a,0b00000000);
                auto bx1 = _mm256_load_pd(Bx+Ai[j+1]*bCols);
                auto av1 = _mm256_permute4x64_pd(a,0b01010101);
                auto bx2 = _mm256_load_pd(Bx+Ai[j+2]*bCols);
                auto av2 = _mm256_permute4x64_pd(a,0b10101010);
                auto bx3 = _mm256_load_pd(Bx+Ai[j+3]*bCols);
                auto av3 = _mm256_permute4x64_pd(a,0b11111111);

                cx0 = _mm256_fmadd_pd(av0,bx0,cx0);
                cx0 = _mm256_fmadd_pd(av1,bx1,cx0);
                cx0 = _mm256_fmadd_pd(av2,bx2,cx0);
                cx0 = _mm256_fmadd_pd(av3,bx3,cx0);
            }
            for (; j < Ap[i+1]; ++j) {
                auto ax0 = _mm256_set1_pd(Ax[j]);
                auto bx0 = _mm256_load_pd(Bx+Ai[j]*bCols);
                cx0 = _mm256_fmadd_pd(ax0,bx0,cx0);
            }
            _mm256_stream_pd(Cx+i*bCols,cx0);
        }
    }

    void spmm_csr_csr_permuted_parallel(double* Cx, double* Ax, double* Bx, int m, int* Ap, int* Ai, int bRows, int bCols, int nThreads, const int* ip) {
#pragma omp parallel for num_threads(nThreads)
        for (int ii = 0; ii < m; ++ii) {
            auto i = ip[ii];
//            for (int j = Ap[ii]; j < Ap[ii+1]; ++j) {
//                for (int k = 0; k < bCols; ++k) {
//                    Cx[ii*bCols+k] += Ax[j] * Bx[Ai[j]*bCols+k];
//                }
//            }
        int j = Ap[i];
        for (; j < Ap[i+1]-3; j+=4) {
            int k = 0;
            for (; k < bCols-3; k+=4) {
                spmm_csr_csr_4x4_kernel(Cx+(i)*bCols+k,Ax+j,Bx+k,Ai+j,bCols);
            }
            for (; k < bCols; ++k) {
                Cx[(i)*bCols + k] += Ax[j] * Bx[Ai[j]*bCols+k];
                Cx[(i)*bCols + k] += Ax[j+1] * Bx[Ai[j]*bCols+k];
                Cx[(i)*bCols + k] += Ax[j+2] * Bx[Ai[j]*bCols+k];
                Cx[(i)*bCols + k] += Ax[j+3] * Bx[Ai[j]*bCols+k];
            }
        }
        for (; j < Ap[i + 1]; ++j) {
            for (int k = 0; k < bCols; ++k) {
                Cx[(i)*bCols + k] += Ax[j] * Bx[Ai[j]*bCols+k];
            }
        }
        }
    }

    void spmm_csr_csr_tiled_permuted_parallel(double* Cx, double* Ax, double* Bx, int m, int* Ap, int* Ai, int bRows, int bCols, int nThreads, const int* ip, const int* it, const int its) {
#pragma omp parallel for schedule(dynamic,1) num_threads(nThreads)
        for (int iii = 0; iii < its; ++iii) {
            for (int i = it[iii]; i < it[iii+1]; ++i) {
                auto ii = ip[i];
                for (int j = Ap[ii]; j < Ap[ii+1]; ++j) {
                    for (int k = 0; k < bCols; ++k) {
                        Cx[ii*bCols+k] += Ax[j] * Bx[Ai[j]*bCols+k];
                    }
                }
            }
        }
    }


    void spmm_csr_csc_tiled_parallel(double *Cx, double *Ax, double *Bx, int m,
                                 int *Ap, int *Ai, int bRows, int bCols,
                                 int nThreads) {
        const int mTileSize = 256;
        const int nTileSize = 32;

        int mTile = m / mTileSize;
        int nTile = bCols / nTileSize;
        int mTileRemainder = m % mTileSize;
        int nTileRemainder = bCols % nTileSize;

#pragma omp parallel for num_threads(nThreads)
        for (int ii = 0; ii < mTile; ++ii) {
            for (int kk = 0; kk < nTile; ++kk) {
                for (int i = 0; i < mTileSize; ++i) {
                    for (int k = 0; k < nTileSize; ++k) {
                        for (int j = Ap[i+ii*mTileSize]; j < Ap[i+ii*mTileSize + 1]; ++j) {
                            Cx[i+ii*mTileSize + (k+kk*nTileSize) * bRows] += Ax[j] * Bx[Ai[j] + (k+kk*nTileSize) * bRows];
                        }
                    }
                }
            }
            for (int k = bCols - nTileRemainder; k < bCols; ++k) {
                for (int i = 0; i < mTileSize; ++i) {
                    for (int j = Ap[i+ii*mTileSize]; j < Ap[i+ii*mTileSize + 1]; ++j) {
                        Cx[i+ii*mTileSize + k * bRows] += Ax[j] * Bx[Ai[j] + k * bRows];
                    }
                }
            }
        }
        for (int i = m-mTileRemainder; i < m; ++i) {
            for (int k = 0; k < bCols; ++k) {
                for (int j = Ap[i]; j < Ap[i + 1]; ++j) {
                    Cx[i + k * bRows] += Ax[j] * Bx[Ai[j] + k * bRows];
                }
            }
        }
    }
    void spmm_csr_csr_4x4_kernel(double* Cx, double* Ax, double* Bx, const int* Ai, int bCols) {
        // Get pointers
        auto bxp0 = Bx + Ai[0]*bCols;
        auto bxp1 = Bx + Ai[1]*bCols;
        auto bxp2 = Bx + Ai[2]*bCols;
        auto bxp3 = Bx + Ai[3]*bCols;

        // Pack vectors
        auto axv0 = _mm256_set1_pd(Ax[0]);
        auto axv1 = _mm256_set1_pd(Ax[1]);
        auto axv2 = _mm256_set1_pd(Ax[2]);
        auto axv3 = _mm256_set1_pd(Ax[3]);

        // Load vectors
        auto bxv0 = _mm256_loadu_pd(bxp0);
        auto bxv1 = _mm256_loadu_pd(bxp1);
        auto bxv2 = _mm256_loadu_pd(bxp2);
        auto bxv3 = _mm256_loadu_pd(bxp3);

        // Load output
        auto r0 = _mm256_loadu_pd(Cx);

        // Compute
        r0 = _mm256_fmadd_pd(axv0,bxv0,r0);
        r0 = _mm256_fmadd_pd(axv1,bxv1,r0);
        r0 = _mm256_fmadd_pd(axv2,bxv2,r0);
        r0 = _mm256_fmadd_pd(axv3,bxv3,r0);

        // Store
        _mm256_storeu_pd(Cx,r0);
    }
    int8_t mask0 = 0b00000000;
    int8_t mask1 = 0b01010101;
    int8_t mask2 = 0b10101010;
    int8_t mask3 = 0b11111111;

    void spmm_csr_csr_tiled_parallel(double *Cx, double *Ax, double *Bx, int m,
                                 int *Ap, int *Ai, int bRows, int bCols,
                                 int nThreads,int mts,int nts) {
      int mTileSize= mts;
      int nTileSize = nts;
      int mTile = m / mTileSize;
      int nTile = bCols / nTileSize;
      int mTileRemainder = m % mTileSize;
      int nTileRemainder = bCols % nTileSize;
#pragma omp parallel for schedule(dynamic,1) num_threads(nThreads)
      for (int ii = 0; ii < mTile; ++ii) {
        auto iOffset = ii*mTileSize;
        for (int kk = 0; kk < nTile; ++kk) {
          auto kOffset = kk*nTileSize;
          for (int i = 0; i < mTileSize; ++i) {
            int j = Ap[i+iOffset];
            for (; j < Ap[i+iOffset+1]-3; j+=4) {
              int k = 0;
              // Pack vectors
              auto av = _mm256_loadu_pd(Ax+j);
              auto axv0 = _mm256_permute4x64_pd(av,0b00000000);
              auto axv1 = _mm256_permute4x64_pd(av,0b01010101);
              auto axv2 = _mm256_permute4x64_pd(av,0b10101010);
              auto axv3 = _mm256_permute4x64_pd(av,0b11111111);

              for (; k < nTileSize-3; k+=4) {
                  // Get pointers
                  auto bxp0 = Bx + Ai[j]*bCols;
                  auto bxp1 = Bx + Ai[j+1]*bCols;
                  auto bxp2 = Bx + Ai[j+2]*bCols;
                  auto bxp3 = Bx + Ai[j+3]*bCols;

                  // Load vectors
                  auto bxv0 = _mm256_loadu_pd(bxp0);
                  auto bxv1 = _mm256_loadu_pd(bxp1);
                  auto bxv2 = _mm256_loadu_pd(bxp2);
                  auto bxv3 = _mm256_loadu_pd(bxp3);

                  // Load output
                  auto r0 = _mm256_loadu_pd(Cx+(i+iOffset)*bCols+kOffset + k);

                  // Compute
                  r0 = _mm256_fmadd_pd(axv0,bxv0,r0);
                  r0 = _mm256_fmadd_pd(axv1,bxv1,r0);
                  r0 = _mm256_fmadd_pd(axv2,bxv2,r0);
                  r0 = _mm256_fmadd_pd(axv3,bxv3,r0);

                  // Store
                  _mm256_storeu_pd(Cx+(i+iOffset)*bCols+kOffset + k,r0);
              }
              for (; k < nTileSize; ++k) {
                  // Mask store and shift
                Cx[(i+iOffset)*bCols+kOffset + k] += Ax[j] * Bx[Ai[j]*bCols+kOffset+k];
                Cx[(i+iOffset)*bCols+kOffset + k] += Ax[j+1] * Bx[Ai[j+1]*bCols+kOffset+k];
                Cx[(i+iOffset)*bCols+kOffset + k] += Ax[j+2] * Bx[Ai[j+2]*bCols+kOffset+k];
                Cx[(i+iOffset)*bCols+kOffset + k] += Ax[j+3] * Bx[Ai[j+3]*bCols+kOffset+k];
              }
            }
            for (; j < Ap[i+iOffset + 1]; ++j) {
              for (int k = 0; k < nTileSize; ++k) {
                Cx[(i+iOffset)*bCols+kOffset + k] += Ax[j] * Bx[Ai[j]*bCols+kOffset+k];
              }
            }
          }
        }
        // nTile Remainder
        if (bCols - nTileRemainder < bCols) {
            for (int i = 0; i < mTileSize; ++i) {
                int j = Ap[i + iOffset];
                for (; j < Ap[i + iOffset + 1]-3; j+=4) {
                    int k = bCols - nTileRemainder;
                    for (; k < bCols-3; k+=4) {
                        spmm_csr_csr_4x4_kernel(Cx+(i+iOffset)*bCols+k,Ax+j,Bx+k,Ai+j,bCols);
                    }
                    for (; k < nTileSize; ++k) {
                        Cx[(i+iOffset)*bCols + k] += Ax[j] * Bx[Ai[j]*bCols+k];
                        Cx[(i+iOffset)*bCols + k] += Ax[j+1] * Bx[Ai[j]*bCols+k];
                        Cx[(i+iOffset)*bCols + k] += Ax[j+2] * Bx[Ai[j]*bCols+k];
                        Cx[(i+iOffset)*bCols + k] += Ax[j+3] * Bx[Ai[j]*bCols+k];
                    }
                }
                for (; j < Ap[i + iOffset + 1]; ++j) {
                    for (int k = bCols - nTileRemainder; k < bCols; ++k) {
                        Cx[(i + iOffset) * bCols + k] +=
                                Ax[j] * Bx[Ai[j] * bCols + k];
                    }
                }
            }
        }
      }
      // mTile Remainder
      for (int i = m-mTileRemainder; i < m; ++i) {
        for (int j = Ap[i]; j < Ap[i+1]; ++j) {
          for (int k = 0; k < bCols; ++k) {
            Cx[i*bCols + k] += Ax[j] * Bx[Ai[j]*bCols+k];
          }
        }
      }
    }

    void spmm_csr_csr_permuted_tiled_parallel_vectorized(double *Cx, double *Ax, double *Bx,
                                     int *Ap, int *Ai, int bCols,
                                     int nThreads,int nts, const int* it, const int its, const int* ip) {
        int nTileSize = nts;
        int nTile = bCols / nTileSize;
        int nTileRemainder = bCols % nTileSize;
#pragma omp parallel for schedule(dynamic,1) num_threads(nThreads)
        for (int ii = 0; ii < its; ++ii) {
            for (int kk = 0; kk < nTile; ++kk) {
                auto kOffset = kk*nTileSize;
                for (int iti = it[ii]; iti < it[ii+1]; ++iti) {
                    const int i = ip[iti];
                    int j = Ap[i];
                    for (; j < Ap[i+1]-3; j+=4) {
                        int k = 0;
                        for (; k < nTileSize-3; k+=4) {
                            spmm_csr_csr_4x4_kernel(Cx+(i)*bCols+kOffset+k,Ax+j,Bx+kOffset+k,Ai+j,bCols);
                        }
                        for (; k < nTileSize; ++k) {
                            Cx[(i)*bCols+kOffset + k] += Ax[j] * Bx[Ai[j]*bCols+kOffset+k];
                            Cx[(i)*bCols+kOffset + k] += Ax[j+1] * Bx[Ai[j]*bCols+kOffset+k];
                            Cx[(i)*bCols+kOffset + k] += Ax[j+2] * Bx[Ai[j]*bCols+kOffset+k];
                            Cx[(i)*bCols+kOffset + k] += Ax[j+3] * Bx[Ai[j]*bCols+kOffset+k];
                        }
                    }
                    for (; j < Ap[i + 1]; ++j) {
                        for (int k = 0; k < nTileSize; ++k) {
                            Cx[(i)*bCols+kOffset + k] += Ax[j] * Bx[Ai[j]*bCols+kOffset+k];
                        }
                    }
                }
            }
            // nTile Remainder
            if (bCols - nTileRemainder < bCols) {
                for (int iti = it[ii]; iti < it[ii+1]; ++iti) {
                    const int i = ip[iti];
                    int j = Ap[i];
                    for (; j < Ap[i+ 1]-3; j+=4) {
                        int k = bCols - nTileRemainder;
                        for (; k < bCols-3; k+=4) {
                            spmm_csr_csr_4x4_kernel(Cx+(i)*bCols+k,Ax+j,Bx+k,Ai+j,bCols);
                        }
                        for (; k < nTileSize; ++k) {
                            Cx[(i)*bCols + k] += Ax[j] * Bx[Ai[j]*bCols+k];
                            Cx[(i)*bCols + k] += Ax[j+1] * Bx[Ai[j]*bCols+k];
                            Cx[(i)*bCols + k] += Ax[j+2] * Bx[Ai[j]*bCols+k];
                            Cx[(i)*bCols + k] += Ax[j+3] * Bx[Ai[j]*bCols+k];
                        }
                    }
                    for (; j < Ap[i + 1]; ++j) {
                        for (int k = bCols - nTileRemainder; k < bCols; ++k) {
                            Cx[(i) * bCols + k] +=
                                    Ax[j] * Bx[Ai[j] * bCols + k];
                        }
                    }
                }
            }
        }
    }

    class SpMMSerial : public sym_lib::FusionDemo {
      protected:
        double* Bx;
        int bRows, bCols;
        sym_lib::timing_measurement fused_code() override {
          sym_lib::timing_measurement t1;
          t1.start_timer();
          spmm_csr_csr(x_, L1_csr_->x, Bx, L1_csr_->m, L1_csr_->p, L1_csr_->i, bRows, bCols);
          t1.measure_elapsed_time();
          //            std::copy(x_,x_+L1_csr_->m*cbb,x_);
          return t1;
        }

      public:
        SpMMSerial(sym_lib::CSR *L, sym_lib::CSC *L_csc, int bRows, int bCols,
            std::string name)
          : FusionDemo(L->m*bCols, name) {
            L1_csr_ = L;
            L1_csc_ = L_csc;
            this->Bx = new double[bRows*bCols];

            for (int i = 0; i < bRows*bCols; i++) {
              this->Bx[i] = 1;
            }
            this->bRows = bRows;
            this->bCols = bCols;
          };

        ~SpMMSerial() override {
          delete[] Bx;
        }
    };

    class SpMMParallel : public SpMMSerial {
      protected:
        sym_lib::timing_measurement fused_code() override {
          sym_lib::timing_measurement t1;
          t1.start_timer();
          spmm_csr_csr_parallel(x_, L1_csr_->x, Bx, L1_csr_->m, L1_csr_->p, L1_csr_->i, bRows, bCols, num_threads_);
//          spmm_csr_csr_2_4_parallel(x_, L1_csr_->x, Bx, L1_csr_->m, L1_csr_->p, L1_csr_->i, bRows, bCols, num_threads_);
          t1.measure_elapsed_time();
          //copy_vector(0,n_,x_in_,x_);
          return t1;
        }

      public:
        SpMMParallel(sym_lib::CSR *L, sym_lib::CSC *L_csc, double *correct_cx, int bRows, int bCols,
            std::string name)
          : SpMMSerial(L,L_csc,bRows,bCols,name) {
            L1_csr_ = L;
            L1_csc_ = L_csc;
            correct_x_ = correct_cx;
          };

        ~SpMMParallel() override = default;
    };

#ifdef ORTOOLS
    class SpMMPermutedParallel : public SpMMSerial {
    protected:
        int* ip;
        std::vector<int> rowTile;
        void build_set() override {
            if (ip == nullptr)
                CalculateRowPermutation();
        }
        sym_lib::timing_measurement fused_code() override {
            sym_lib::timing_measurement t1;
            t1.start_timer();
            spmm_csr_csr_permuted_parallel(x_, L1_csr_->x, Bx, L1_csr_->m, L1_csr_->p, L1_csr_->i, bRows, bCols, num_threads_,ip);
//            spmm_csr_csr_tiled_permuted_parallel(x_, L1_csr_->x, Bx, L1_csr_->m, L1_csr_->p, L1_csr_->i, bRows, bCols, num_threads_,ip,rowTile.data(),rowTile.size());
//            spmm_csr_csr_permuted_tiled_parallel_vectorized(x_, L1_csr_->x, Bx, L1_csr_->p, L1_csr_->i, bCols, num_threads_,16, rowTile.data(),rowTile.size(),ip);
            t1.measure_elapsed_time();
            //copy_vector(0,n_,x_in_,x_);
            return t1;
        }

        enum EdgeCostStrategy {
          Geometric,
          MeanPoint
        };

        void CreateCubicNodes() {
          int m = L1_csr_->m;
          int* Lp = L1_csr_->p;
          const int* Li = L1_csr_->i;
          std::vector<std::tuple<int,int,int>> points(L1_csr_->nnz*bCols);
          std::vector<int> cubePosition = {0};

          int mTileSize= 8;
          int nTileSize = 8;
          int jTileSize = 8;
          int mTile = m / mTileSize;
          int nTile = bCols / nTileSize;
          int jTile = 0;
          int mTileRemainder = m % mTileSize;
          int nTileRemainder = bCols % nTileSize;

          for (int ii = 0; ii < mTile; ++ii) {
            auto iOffset = ii*mTileSize;
            for (int kk = 0; kk < nTile; ++kk) {
              auto kOffset = kk*nTileSize;
              bool moreJPoints = true;
              for (int i = 0; i < mTileSize; ++i) {
                  for (int k = 0; k < nTileSize; ++k) {
                  int j = Lp[i+iOffset]+jTile*jTileSize, jp = jTile*jTileSize;
                    for (; jp < std::min(jTile*jTileSize+jTileSize,Lp[i+iOffset+1]-Lp[i+iOffset]) && jp; ++j) {
//                      cubePosition.emplace_back((i+iOffset)*bCols+kOffset + k,j,Li[j]*bCols+kOffset+k);
                    }
                    moreJPoints = moreJPoints || jTile*jTileSize < (Lp[i+iOffset+1]-Lp[i+iOffset]);
                }
              }
            }
          }

          std::vector<std::vector<int>> distance_matrix(cubePosition.size());

          for (int i = 0; i < cubePosition.size(); ++i) {
           for (int j = 0; j < cubePosition.size(); ++j) {
              distance_matrix[i][j] = CalculateCubicEdgeCost(EdgeCostStrategy::Geometric,i,j,cubePosition,points);
            }
          }
        }


        int CalculateCubicEdgeCost(EdgeCostStrategy strategy, int i, int j, std::vector<int>& position, std::vector<std::tuple<int,int,int>>& points) {
          switch(strategy) {
            case Geometric: {

                            }
            case MeanPoint: {

                            }
            default: {
                       break;
                     }
          }
        }

        void CalculateRowPermutation() {
            int m = L1_csr_->m;
            int* Lp = L1_csr_->p;
            const int* Li = L1_csr_->i;
            ip = new int[m]();

            // Cache results
            std::vector<std::vector<int64_t>> distance_matrix;
            for (int i = 0; i < m; ++i) {
                std::vector<int64_t> innerMatrix(m,1000);
                distance_matrix.push_back(innerMatrix);
            }

            // Build edge cost set
            for (int i = 0; i < m; ++i) {
                for (int j = 0; j < m; ++j) {
                    int p0 = Lp[i];
                    int p1 = Lp[j];

                    int counter = 0;
                    while (p0 < Lp[i+1] && p1 < Lp[j+1]) {
                        if (Li[p0] == Li[p1]) {
                            counter++;
                            p0++;
                            p1++;
                        } else if (Li[p0] < Li[p1]) {
                            p0++;
                        } else {
                            p1++;
                        }
                    }
                    // Try to enforce spatial locality

                    // Try to enforce temporal locality
                    {
                        if (counter != 0) {
                            distance_matrix[i][j] =
                                    std::min(Lp[i + 1] - Lp[i],
                                             Lp[j + 1] - Lp[j]) -
                                    counter;
                        }  else {
                            distance_matrix[i][j] += std::min(
                                    Lp[i + 1] - Lp[i], Lp[j + 1] - Lp[j]);
                        }
//                        if (std::abs(j - i) < 10) { distance_matrix[i][j] /= 4; }


                        // Try to enforce locality constraints
                        if (std::abs(i - j) > 40) {
                            distance_matrix[i][j] += 100000;
                        }
                    }
                }
            }

            // Solve TSP (naive)
            operations_research::RoutingIndexManager manager(distance_matrix.size(), 1,operations_research::RoutingIndexManager::NodeIndex {0});
            operations_research::RoutingModel routing(manager);
            const int transit_callback_index = routing.RegisterTransitCallback(
                    [&distance_matrix, &manager](int64_t from_index, int64_t to_index) -> int64_t {
                        // Convert from routing variable Index to distance matrix NodeIndex.
                        auto from_node = manager.IndexToNode(from_index).value();
                        auto to_node = manager.IndexToNode(to_index).value();
                        return distance_matrix[from_node][to_node];
                    });

            routing.SetArcCostEvaluatorOfAllVehicles(transit_callback_index);
            operations_research::RoutingSearchParameters searchParameters = operations_research::DefaultRoutingSearchParameters();
            searchParameters.set_first_solution_strategy(
                    operations_research::FirstSolutionStrategy::PATH_CHEAPEST_ARC);
            const operations_research::Assignment* solution = routing.SolveWithParameters(searchParameters);

            int64_t index = routing.Start(0), cnt = 0;
            while (routing.IsEnd(index) == false) {
                ip[cnt++] = manager.IndexToNode(index).value();
                std::cout << ip[cnt-1] << ",";
                index = solution->Value(routing.NextVar(index));
            }
            std::cout << std::endl;

            // Greedy algorithm to tile
            rowTile = {0};
            std::set<int> uniqueColumns;
            int points = 0;
            const int L2_BYTES = 262144;
            const int L1_BYTES = 32000;
            for (int i = 0; i < m; ++i) {
                auto ii = ip[i];
                int counter = 0;
                for (int p0 = Lp[ii]; p0 < Lp[ii + 1]; ++p0, ++points) {
                    uniqueColumns.insert(Li[p0]);
                }
                if ((uniqueColumns.size()*bCols) >= L2_BYTES || L1_BYTES <= ((i-rowTile.back())*bCols + points)) {
                    uniqueColumns.clear();
                    rowTile.push_back(i);
                    points = 0;
                }
            }
            rowTile.push_back(m);


            // Sort final permutations
//            for (int ii = 0; ii < rowTile.size(); ++ii) {
//                std::sort(ip+rowTile[ii],ip+rowTile[ii+1]);
//            }

            for (int ii = 0; ii < rowTile.size(); ++ii) {
                std::cout << "(";
                for (int i = rowTile[ii]; i < rowTile[ii+1]; ++i) {
                    std::cout << ip[i] << ",";
                }
                std::cout << ") ->";
            }
            std::cout << std::endl;
            for (int ii = 0; ii < rowTile.size()-1; ++ii) {
                std::cout << rowTile[ii+1] - rowTile[ii] << " -> ";
            }
            std::cout << std::endl;
        }


    public:
        SpMMPermutedParallel(sym_lib::CSR *L, sym_lib::CSC *L_csc, double *correct_cx, int bRows, int bCols,
                     std::string name)
                     : ip(nullptr), SpMMSerial(L,L_csc,bRows,bCols,name) {
            L1_csr_ = L;
            L1_csc_ = L_csc;
            correct_x_ = correct_cx;
        };

        ~SpMMPermutedParallel() override = default;
    };
#endif

    class SpMMTiledParallel : public SpMMSerial {
      protected:
        int mTileSize;
        int nTileSize;
        sym_lib::timing_measurement fused_code() override {
          sym_lib::timing_measurement t1;
          if (bCols == 4) {
              t1.start_timer();
              spmm_csr_csr_4_parallel(x_, L1_csr_->x, Bx, L1_csr_->m, L1_csr_->p, L1_csr_->i, bRows, bCols, num_threads_);
              t1.measure_elapsed_time();
          } else {
              t1.start_timer();
              spmm_csr_csr_tiled_parallel(x_, L1_csr_->x, Bx, L1_csr_->m,
                                          L1_csr_->p, L1_csr_->i, bRows, bCols,
                                          num_threads_, mTileSize, nTileSize);
              t1.measure_elapsed_time();
          }
          //copy_vector(0,n_,x_in_,x_);
          return t1;
        }

      public:
        SpMMTiledParallel(sym_lib::CSR *L, sym_lib::CSC *L_csc, double *correct_cx, int bRows, int bCols,int mTileSize,int nTileSize,
            std::string name)
          : mTileSize(mTileSize), nTileSize(nTileSize), SpMMSerial(L,L_csc,bRows,bCols,name) {
            L1_csr_ = L;
            L1_csc_ = L_csc;
            correct_x_ = correct_cx;
          };
#ifdef ORTOOLS
        void CalculateMatrixTSP() {
            int m = L1_csr_->m;
            int* Lp = L1_csr_->p;
            const int* Li = L1_csr_->i;
            const int nnz = L1_csr_->nnz;

            int computations = bCols*L1_csr_->nnz;

            // Cache results
            std::vector<std::vector<int64_t>> distance_matrix;
            for (int i = 0; i < computations; ++i) {
                std::vector<int64_t> innerMatrix(computations,10000);
                distance_matrix.push_back(innerMatrix);
            }
            std::vector<std::string> strMapping(bCols*nnz);
            for (int i = 0; i < m; ++i) {
                for (int j = Lp[i]; j < Lp[i+1]; ++j) {
                    for (int k = 0; k < bCols; ++k) {
                        std::stringstream ss;
                        ss << "(" << i*bCols+k << "," << j << "," << Li[j]*bCols+k << ")";
                        strMapping[j*bCols+k] = ss.str();
                        for (int ii = 0; ii < m; ++ii) {
                            for (int jj = Lp[ii]; jj < Lp[ii + 1]; ++jj) {
                                for (int kk = 0; kk < bCols; ++kk) {
                                    int d0 = (ii*bCols + kk)-(i*bCols + k);
                                    int d1 = (jj)-(j);
                                    int d2 = (Li[jj]*bCols+kk)-(Li[j]*bCols+k);

                                    if ((d0 == 0 || d0 == 1 || d1 == 0 || d1 == 1 || d2 == 0 || d2 == 1) && (d0 >= 0 && d1 >= 0)) {
                                        distance_matrix[j*bCols+k][jj*bCols+kk] = std::sqrt(std::pow(d0,2) + std::pow(d1,2) + std::pow(d2,2));
                                    } else {
                                        distance_matrix[j*bCols+k][jj*bCols+kk] += std::sqrt(std::pow(d0,2) + std::pow(d1,2) + std::pow(d2,2));
                                    }
                                }
                            }
                        }
                    }
                }
            }

            // Solve TSP (naive)
            operations_research::RoutingIndexManager manager(distance_matrix.size(), 1,operations_research::RoutingIndexManager::NodeIndex {0});
            operations_research::RoutingModel routing(manager);

            const int transit_callback_index = routing.RegisterTransitCallback(
                    [&distance_matrix, &manager](int64_t from_index, int64_t to_index) -> int64_t {
                        // Convert from routing variable Index to distance matrix NodeIndex.
                        auto from_node = manager.IndexToNode(from_index).value();
                        auto to_node = manager.IndexToNode(to_index).value();
                        return distance_matrix[from_node][to_node];
                    });
            routing.SetArcCostEvaluatorOfAllVehicles(transit_callback_index);
            operations_research::RoutingSearchParameters searchParameters = operations_research::DefaultRoutingSearchParameters();
            searchParameters.set_first_solution_strategy(
                    operations_research::FirstSolutionStrategy::PATH_CHEAPEST_ARC);

            const operations_research::Assignment* solution = routing.SolveWithParameters(searchParameters);
            PrintSolution(manager, routing, *solution,strMapping);
        }

          void CalculateRowPermutation() {
              int m = L1_csr_->m;
              int* Lp = L1_csr_->p;
              const int* Li = L1_csr_->i;

              // Cache results
              std::vector<std::vector<int64_t>> distance_matrix;
              for (int i = 0; i < m; ++i) {
                  std::vector<int64_t> innerMatrix(m,1000);
                  distance_matrix.push_back(innerMatrix);
              }

              // Build edge cost set
              for (int i = 0; i < m; ++i) {
                  for (int j = 0; j < m; ++j) {
                      int p0 = Lp[i];
                      int p1 = Lp[j];

                      int counter = 0;
                      while (p0 < Lp[i+1] && p1 < Lp[j+1]) {
                          if (Li[p0] == Li[p1]) {
                              counter++;
                              p0++;
                              p1++;
                          } else if (Li[p0] < Li[p1]) {
                              p0++;
                          } else {
                              p1++;
                          }
                      }
                      if (counter != 0)
                        distance_matrix[i][j] = std::min(Lp[i+1]-Lp[i],Lp[j+1]-Lp[j]) - counter;
                  }
              }

              // Solve TSP (naive)
              operations_research::RoutingIndexManager manager(distance_matrix.size(), 1,operations_research::RoutingIndexManager::NodeIndex {0});
              operations_research::RoutingModel routing(manager);

              const int transit_callback_index = routing.RegisterTransitCallback(
                      [&distance_matrix, &manager](int64_t from_index, int64_t to_index) -> int64_t {
                          // Convert from routing variable Index to distance matrix NodeIndex.
                          auto from_node = manager.IndexToNode(from_index).value();
                          auto to_node = manager.IndexToNode(to_index).value();
                          return distance_matrix[from_node][to_node];
                      });
              routing.SetArcCostEvaluatorOfAllVehicles(transit_callback_index);
              operations_research::RoutingSearchParameters searchParameters = operations_research::DefaultRoutingSearchParameters();
              searchParameters.set_first_solution_strategy(
                      operations_research::FirstSolutionStrategy::PATH_CHEAPEST_ARC);

              const operations_research::Assignment* solution = routing.SolveWithParameters(searchParameters);
              PrintSolution(manager, routing, *solution, std::vector<std::string>{});
          }


          static void PrintSolution(const operations_research::RoutingIndexManager& manager,
                                    const operations_research::RoutingModel& routing, const operations_research::Assignment& solution, const std::vector<std::string>& strMapping) {
              // Inspect solution.
              LOG(INFO) << "Objective: " << solution.ObjectiveValue() << " miles";
              int64_t index = routing.Start(0);
              LOG(INFO) << "Route:";
              int64_t distance{0};
              std::stringstream route;
              while (routing.IsEnd(index) == false) {
                  route << strMapping[manager.IndexToNode(index).value()] << " -> ";
                  int64_t previous_index = index;
                  index = solution.Value(routing.NextVar(index));
                  distance += routing.GetArcCostForVehicle(previous_index, index, int64_t{0});
              }
              LOG(INFO) << route.str() << manager.IndexToNode(index).value();
              LOG(INFO) << "Route distance: " << distance << "miles";
              LOG(INFO) << "";
              LOG(INFO) << "Advanced usage:";
              LOG(INFO) << "Problem solved in " << routing.solver()->wall_time() << "ms";
          }
#endif

          void CalculateTileDensity() {
              // Calculate points per tile
              // Calculate spatial per tile
              // Calculate reuse per tile
              bRows, bCols, num_threads_;
              const int* Ap = L1_csr_->p;
              const int* Ai = L1_csr_->i;
              const int m = L1_csr_->m;

              int mTile = m / mTileSize;
              int nTile = bCols / nTileSize;
              int mTileRemainder = m % mTileSize;
              int nTileRemainder = bCols % nTileSize;

              typedef std::tuple<int,int,int,double,double,double,double> TileStats;
              const std::string tileHeader = "Tile Points,mTileSize,Unique Columns,Average Reuse,Average Reuse Deviation, Average Points Per Row, Average Points Per Row Deviation\n";

              auto calculateTileStatistics = [&Ap](int tilePoints,std::unordered_map<int,int>& sl,int mTileSize,int iOffset) -> TileStats {
                  double avgPointsPerTileRow = 0, avgPointsPerTileRowDeviation = 0;
                  for (int i = 0; i < mTileSize; ++i) {
                      avgPointsPerTileRow += Ap[i+iOffset+1] - Ap[i + iOffset];
                  }
                  avgPointsPerTileRow /= mTileSize;
                  for (int i = 0; i < mTileSize; ++i) {
                      avgPointsPerTileRowDeviation += std::pow((Ap[i+iOffset+1] - Ap[i + iOffset])-avgPointsPerTileRow,2);
                  }
                  avgPointsPerTileRowDeviation = std::sqrt(avgPointsPerTileRowDeviation/mTileSize);

                  double averageReuse = 0, averageReuseDeviation = 0;
                  for (const auto& key : sl) {
                      averageReuse += key.second;
                  }
                  averageReuse /= sl.size();
                  for (const auto key : sl) {
                      averageReuseDeviation += std::pow(key.second - averageReuse,2);
                  }
                  averageReuseDeviation = std::sqrt(averageReuseDeviation/sl.size());
                  return std::make_tuple(tilePoints, mTileSize, sl.size(), averageReuse, averageReuseDeviation, avgPointsPerTileRow, avgPointsPerTileRowDeviation);
              };

              std::vector<TileStats> tileData;
              for (int ii = 0; ii < mTile; ++ii) {
                  auto iOffset = ii * mTileSize;
                      for (int kk = 0; kk < nTile; ++kk) {
                          //-------- TILE SIZE START
                          {
                              int tilePoints = 0;
                              std::unordered_map<int,int> sl;
                          for (int i = 0; i < mTileSize; ++i) {
                              for (int j = Ap[i + iOffset];
                                   j < Ap[i + iOffset + 1]; ++j) {
                                  for (int k = 0; k < nTileSize; ++k) {
                                      // Add point to tile
                                      tilePoints++;
                                  }

                                  // Get tile temporal locality
                                  if (sl.find(Ai[j]) != sl.end())
                                    sl[Ai[j]] += 1;
                                  else
                                      sl.insert(std::make_pair(sl[Ai[j]],1));
                              }
                          }
                          tileData.emplace_back(calculateTileStatistics(tilePoints,sl,mTileSize,iOffset));
                      }
                  }
                  //-------- TILE SIZE STOP


                  //-------- TILE SIZE START
                  if ((bCols - nTileRemainder) < bCols) {
                      int tilePoints = 0;
                      std::unordered_map<int,int> sl;
                          for (int i = 0; i < mTileSize; ++i) {
                              for (int j = Ap[i + iOffset]; j < Ap[i + iOffset + 1];
                                   ++j) {
                                  for (int k = bCols - nTileRemainder; k < bCols; ++k) {
                                  // Add point to tile
                                    tilePoints++;
                                  }
                                  // Get tile temporal locality
                                  if (sl.find(Ai[j]) != sl.end())
                                      sl[Ai[j]] += 1;
                                  else
                                      sl.insert(std::make_pair(sl[Ai[j]],1));
                          }
                      }
                      tileData.emplace_back(calculateTileStatistics(tilePoints,sl,mTileSize,iOffset));
                  }
                  //-------- TILE SIZE STOP
              }


              //-------- TILE SIZE START
              {
                  int tilePoints = 0;
                  std::unordered_map<int,int> sl;
                  for (int i = m - mTileRemainder; i < m; ++i) {
                      for (int j = Ap[i]; j < Ap[i + 1]; ++j) {
                          for (int k = 0; k < bCols; ++k) {
                              // Add point to tile
                              tilePoints++;
                          }
                          // Get tile temporal locality
                          if (sl.find(Ai[j]) != sl.end())
                              sl[Ai[j]] += 1;
                          else
                              sl.insert(std::make_pair(sl[Ai[j]],1));
                      }
                  }
                  tileData.emplace_back(calculateTileStatistics(tilePoints,sl,mTileRemainder,m - mTileRemainder));
              }
              //-------- TILE SIZE STOP

              // Print Tile Stats
              std::ofstream tileDataFile("tile_data.csv");
              tileDataFile << tileHeader;
              for (auto const tile : tileData) {
                  tileDataFile << std::get<0>(tile) << "," <<
                               std::get<1>(tile) << "," <<
                               std::get<2>(tile) << "," <<
                               std::get<3>(tile) << "," <<
                               std::get<4>(tile) << "," <<
                               std::get<5>(tile) << "," <<
                               std::get<6>(tile) << '\n';
              }
              tileDataFile.close();
          }
          ~SpMMTiledParallel() override = default;
    };

    class SpMMDDT : public SpMMSerial {
      protected:
        std::vector<DDT::Codelet*>* cl;
        DDT::Config cfg;
        DDT::GlobalObject d;
        sym_lib::timing_measurement analysis_breakdown;

        void build_set() override {
          // Allocate memory and generate global object
          if (this->cl == nullptr) {
            this->cl = new std::vector<DDT::Codelet *>[cfg.nThread];
            analysis_breakdown.start_timer();
            d = DDT::init(this->L1_csr_, cfg);
            analysis_breakdown.start_timer();
            DDT::inspectSerialTrace(d, cl, cfg);
            analysis_breakdown.measure_elapsed_time();
          }
        }

        sym_lib::timing_measurement fused_code() override {
          sym_lib::timing_measurement t1;
          DDT::Args args;
          args.x = x_in_;
          args.y = x_;
          args.r = L1_csr_->m;
          args.Lp = L1_csr_->p;
          args.Li = L1_csr_->i;
          args.Lx = L1_csr_->x;
          args.bRows = bRows;
          args.bCols = bCols;
          args.Ax = L1_csr_->x;
          args.Bx = Bx;
          args.Cx = x_;
          t1.start_timer();
          DDT::executeCodelets(cl, cfg, args);
          t1.measure_elapsed_time();
          //copy_vector(0,n_,x_in_,x_);
          return t1;
        }

      public:
        SpMMDDT(sym_lib::CSR *L, sym_lib::CSC *L_csc, double *correct_cx, DDT::Config& cfg, int bRows, int bCols,
            std::string name)
          : cl(nullptr), SpMMSerial(L,L_csc,bRows,bCols,name) {
            L1_csr_ = L;
            L1_csc_ = L_csc;
            correct_x_ = correct_cx;
            this->cfg = cfg;
          };

        sym_lib::timing_measurement get_analysis_bw() {
         return analysis_breakdown;
        }

        ~SpMMDDT() override = default;
    };

    class SpMMMKL : public SpMMSerial {
      protected:
        sym_lib::timing_measurement analysis_breakdown;
        matrix_descr d;
        sparse_matrix_t m;
        MKL_INT *LLI;

        void build_set() override {
          if (LLI == nullptr) {
            analysis_breakdown.start_timer();
            d.type = SPARSE_MATRIX_TYPE_GENERAL;
            //         d.diag = SPARSE_DIAG_NON_UNIT;
            //         d.mode = SPARSE_FILL_MODE_FULL;

            MKL_INT expected_calls = 5;

            LLI = new MKL_INT[this->L1_csr_->m + 1]();
            for (int l = 0; l < this->L1_csr_->m + 1; ++l) {
              LLI[l] = this->L1_csr_->p[l];
            }

            mkl_sparse_d_create_csr(&m, SPARSE_INDEX_BASE_ZERO,
                this->L1_csr_->m, this->L1_csr_->n, LLI,
                LLI + 1, this->L1_csr_->i,
                this->L1_csr_->x);
            //         mkl_sparse_set_mv_hint(m, SPARSE_OPERATION_NON_TRANSPOSE, d, expected_calls);
            //         mkl_sparse_set_memory_hint(m, SPARSE_MEMORY_AGGRESSIVE);

            mkl_set_num_threads(num_threads_);
            mkl_set_num_threads_local(num_threads_);
            analysis_breakdown.measure_elapsed_time();
          }
        }

        sym_lib::timing_measurement fused_code() override {
          sym_lib::timing_measurement t1;
          t1.start_timer();
          mkl_sparse_d_mm(SPARSE_OPERATION_NON_TRANSPOSE, 1, m, this->d, SPARSE_LAYOUT_ROW_MAJOR, Bx, bCols, bCols, 0, x_, bCols);
          t1.measure_elapsed_time();
          return t1;
        }

      public:
        SpMMMKL(sym_lib::CSR *L, sym_lib::CSC *L_csc, double *correct_cx, int bRows, int bCols,
            std::string name)
          : LLI(nullptr), SpMMSerial(L,L_csc,bRows,bCols,name) {
            L1_csr_ = L;
            L1_csc_ = L_csc;
            correct_x_ = correct_cx;
          };


         sym_lib::timing_measurement get_analysis_bw() {
          return analysis_breakdown;
         }

     ~SpMMMKL() override {
          //            mkl_free(LLI);
        };
    };
}

#endif//DDT_SPMM_DEMO_UTILS_H
