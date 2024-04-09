//
// Created by kazem on 7/16/21.
//

#ifndef DDT_SPTRSV_DEMO_UTILS_H
#define DDT_SPTRSV_DEMO_UTILS_H

#include "FusionDemo.h"

#include "Analyzer.h"
#include "DDT.h"
#include "Executor.h"
#include "Input.h"
#include "Inspector.h"
#include "PatternMatching.h"

#include <immintrin.h>

#include <iostream>
#include <SpTRSVModel.h>
#include <lbc.h>
#include <sparse_inspector.h>

#include <lbc_csc_dag.h>
#ifdef MKL
#include <mkl.h>
#include <mkl_spblas.h>
#include <mkl_types.h>
#include <sparse_io.h>
#endif


namespace sparse_avx{

    typedef union
    {
        __m128i v;
        int d[4];
    } v4if_t;
    typedef union
    {
        __m256d v;
        double d[4];
    } v4df_t;
    inline double hsum_double_avx(__m256d v) {
        __m128d vlow = _mm256_castpd256_pd128(v);
        __m128d vhigh = _mm256_extractf128_pd(v, 1);  // high 128
        vlow = _mm_add_pd(vlow, vhigh);      // reduce down to 128

        __m128d high64 = _mm_unpackhi_pd(vlow, vlow);
        return _mm_cvtsd_f64(_mm_add_sd(vlow, high64));  // reduce to scalar
    }

#define set1(x,a,ind) _mm256_set_pd(x[a[ind+3]],x[a[ind+2]],x[a[ind+1]],x[a[ind]])

    void sptrsv_csr_vec256_1(int n, const int *Ap, const int *Ai, const double *Ax,
                              double *x, double *y) {
        for (int i = 0; i < n; i++) {
            auto r0 = _mm256_setzero_pd();
            int j = Ap[i];
            for (; j < Ap[i+1]-4; j+=4) {
                auto ax0 = _mm256_loadu_pd(Ax+j);
                auto x0 = set1(x,Ai,j);
                r0 = _mm256_fmadd_pd(ax0,x0,r0);
            }
            double tail = hsum_double_avx(r0);
            for (; j < Ap[i+1]-1; j++) {
                tail += Ax[j] * x[Ai[j]];
            }
            x[i] -= tail;
            x[i] /= Ax[Ap[i+1]-1];
        }
    }

    void sptrsv_csr_vec256_2(int n, const int *Ap, const int *Ai, const double *Ax,
                             double *x, double *y) {
        int i = 0;
        for (; i < n-1; i+=2) {
            auto r0 = _mm256_setzero_pd();
            auto r1 = _mm256_setzero_pd();
            int j0 = Ap[i], j1 = Ap[i+1];
            for (; j0 < Ap[i+1]-4 && j1 < Ap[i+2]-5; j0+=4, j1+=4) {
                auto ax0 = _mm256_loadu_pd(Ax+j0);
                auto ax1 = _mm256_loadu_pd(Ax+j1);
                auto x0 = set1(x,Ai,j0);
                auto x1 = set1(x,Ai,j1);
                r0 = _mm256_fmadd_pd(ax0,x0,r0);
                r1 = _mm256_fmadd_pd(ax1,x1,r1);
            }

            auto h0 = _mm256_hadd_pd(r0,r1);
            auto h1 = _mm_add_pd(_mm256_castpd256_pd128(h0),_mm256_extractf128_pd(h0, 1));

            for (; j0 < Ap[i+1]-1; j0++) {
                h1[0] += Ax[j0] * x[Ai[j0]];
            }
            x[i] -= h1[0];
            x[i] /= Ax[Ap[i+1]-1];

            for (; j1 < Ap[i+2]-1; j1++) {
                h1[1] += Ax[j1] * x[Ai[j1]];
            }
            x[i+1] -= h1[1];
            x[i+1] /= Ax[Ap[i+2]-1];
        }
        if (i < n-1) {
            for (int j = Ap[i]; j < Ap[i + 1] - 1; ++j) {
                x[i] -= Ax[j] * x[Ai[j]];
            }
            x[i] /= Ax[Ap[i + 1] - 1];
        }
    }

    ///// SPTRSV
 void sptrsv_csc(int n, int *Lp, int *Li, double *Lx, double *x) {
  int i, j;
  for (i = 0; i < n; i++) {
   x[i] /= Lx[Lp[i]];
   for (j = Lp[i] + 1; j < Lp[i + 1]; j++) {
    x[Li[j]] -= Lx[j] * x[i];
   }
  }
 }

 void sptrsv_csr(int n, int *Lp, int *Li, double *Lx, double *x) {
  int i, j;
  for (i = 0; i < n; i++) {
   for (j = Lp[i]; j < Lp[i + 1] - 1; j++) {
    x[i] -= Lx[j] * x[Li[j]];
   }
   x[i] /= Lx[Lp[i + 1] - 1];
  }
 }

 void sptrsv_csr_levelset(int n, const int *Lp, const int *Li, const double *Lx,
                          double *x,
                          int levels, const int *levelPtr,
                          const int *levelSet, int nThreads) {
  for (int l = 0; l < levels; l++) {
#pragma omp  parallel for schedule(auto) num_threads(nThreads)
   for (int k = levelPtr[l]; k < levelPtr[l + 1]; ++k) {
    int i = levelSet[k];
    auto r0 = _mm256_setzero_pd();
    int j = Lp[i];
    for (; j < Lp[i+1]-4; j+=4) {
     auto ax0 = _mm256_loadu_pd(Lx+j);
     auto x0 = set1(x,Li,j);
     r0 = _mm256_fmadd_pd(ax0,x0,r0);
    }
    double tail = hsum_double_avx(r0);
    for (; j < Lp[i+1]-1; j++) {
     tail += Lx[j] * x[Li[j]];
    }
    x[i] -= tail;
    x[i] /= Lx[Lp[i+1]-1];
   }
  }
 }
 void sptrsv_csr_levelset_novec(int n, const int *Lp, const int *Li, const
 double *Lx, double *x, int levels, const int *levelPtr,const int *levelSet, int nThreads) {
  for (int l = 0; l < levels; l++) {
#pragma omp  parallel for default(shared) schedule(auto) num_threads(nThreads)
   for (int k = levelPtr[l]; k < levelPtr[l + 1]; ++k) {
    int i = levelSet[k];
    for (int j = Lp[i]; j < Lp[i + 1] - 1; j++) {
     x[i] -= Lx[j] * x[Li[j]];
    }
    x[i] /= Lx[Lp[i + 1] - 1];
   }
  }
 }


 void sptrsv_csr_lbc_vec_2(int n, int *Lp, int *Li, double *Lx, double *x,
                           int level_no, int *level_ptr,
                     int *par_ptr, int *partition, int nThreads) {
    for (int i1 = 0; i1 < level_no; ++i1) {
#pragma omp  parallel num_threads(nThreads)
        {
#pragma omp for schedule(auto)
            for (int j1 = level_ptr[i1]; j1 < level_ptr[i1 + 1]; ++j1) {
                int k1 = par_ptr[j1];
                for (; k1 < par_ptr[j1 + 1]-1 && partition[k1+1]-partition[k1] == 1; k1+=2) {
                    auto ii0 = partition[k1];
                    auto ii1 = partition[k1+1];
                        auto r0 = _mm256_setzero_pd();
                        auto r1 = _mm256_setzero_pd();
                        int j0 = Lp[ii0], ji1 = Lp[ii1];
                        for (; j0 < Lp[ii0+1]-4 && ji1 < Lp[ii1+1]-5; j0+=4, ji1+=4) {
                            auto ax0 = _mm256_loadu_pd(Lx+j0);
                            auto ax1 = _mm256_loadu_pd(Lx+ji1);
                            auto x0 = set1(x,Li,j0);
                            auto x1 = set1(x,Li,ji1);
                            r0 = _mm256_fmadd_pd(ax0,x0,r0);
                            r1 = _mm256_fmadd_pd(ax1,x1,r1);
                        }

                        auto h0 = _mm256_hadd_pd(r0,r1);
                        auto h1 = _mm_add_pd(_mm256_castpd256_pd128(h0),_mm256_extractf128_pd(h0, 1));

                        for (; j0 < Lp[ii0+1]-1; j0++) {
                            h1[0] += Lx[j0] * x[Li[j0]];
                        }
                        x[ii0] -= h1[0];
                        x[ii0] /= Lx[Lp[ii0+1]-1];

                        for (; ji1 < Lp[ii1+1]-1; ji1++) {
                            h1[1] += Lx[ji1] * x[Li[ji1]];
                        }
                        x[ii1] -= h1[1];
                        x[ii1] /= Lx[Lp[ii1+1]-1];
                    }
                    for (; k1 < par_ptr[j1 + 1]; k1++) {
                        int i = partition[k1];
                        auto r0 = _mm256_setzero_pd();
                        int j = Lp[i];
                        for (; j < Lp[i+1]-4; j+=4) {
                            auto ax0 = _mm256_loadu_pd(Lx+j);
                            auto x0 = set1(x,Li,j);
                            r0 = _mm256_fmadd_pd(ax0,x0,r0);
                        }
                        double tail = hsum_double_avx(r0);
                        for (; j < Lp[i+1]-1; j++) {
                            tail += Lx[j] * x[Li[j]];
                        }
                        x[i] -= tail;
                        x[i] /= Lx[Lp[i+1]-1];
                    }

                }
            }
        }
    }

void sptrsv_csr_lbc(int n, int *Lp, int *Li, double *Lx, double *x,
                        int level_no, int *level_ptr,
                        int *par_ptr, int *partition, int nThreads) {
        for (int i1 = 0; i1 < level_no; ++i1) {
#pragma omp  parallel num_threads(nThreads)
            {
#pragma omp for schedule(auto)
                for (int j1 = level_ptr[i1]; j1 < level_ptr[i1 + 1]; ++j1) {
                    for (int k1 = par_ptr[j1]; k1 < par_ptr[j1 + 1]; ++k1) {
                        int i = partition[k1];
                        for (int j = Lp[i]; j < Lp[i + 1] - 1; j++) {
                            x[i] -= Lx[j] * x[Li[j]];
                        }
                        x[i] /= Lx[Lp[i + 1] - 1];
                    }
                }
            }
        }
    }


 class SpTRSVSerial : public sym_lib::FusionDemo {
 protected:
  sym_lib::timing_measurement fused_code() override {
   std::copy(x_in_, x_in_+n_, x_);
   sym_lib::timing_measurement t1;
   t1.start_timer();
   sparse_avx::sptrsv_csr(n_, L1_csr_->p, L1_csr_->i, L1_csr_->x, x_);
   t1.measure_elapsed_time();
   //copy_vector(0,n_,x_in_,x_);
   return t1;
  }

 public:
  SpTRSVSerial(sym_lib::CSR *L, sym_lib::CSC *L_csc,
             double *correct_x,
             std::string name) :
    FusionDemo(L_csc->n, name) {
   L1_csr_ = L;
   L1_csc_ = L_csc;
   correct_x_ = correct_x;
  };

  ~SpTRSVSerial() override = default;
 };

    class SpTRSVSerialVec1 : public SpTRSVSerial {
    protected:
        sym_lib::timing_measurement fused_code() override {
            std::copy(x_in_, x_in_+n_, x_);
            sym_lib::timing_measurement t1;
            t1.start_timer();
            sparse_avx::sptrsv_csr_vec256_1(n_, L1_csr_->p, L1_csr_->i, L1_csr_->x, x_,x_);
            t1.measure_elapsed_time();
            //copy_vector(0,n_,x_in_,x_);
            return t1;
        }

    public:
        SpTRSVSerialVec1(sym_lib::CSR *L, sym_lib::CSC *L_csc,
                         double *correct_x,
                         std::string name) :
                SpTRSVSerial(L, L_csc, correct_x, name) {
            L1_csr_ = L;
            L1_csc_ = L_csc;
            correct_x_ = correct_x;
        };

        ~SpTRSVSerialVec1() override = default;
    };

    class SpTRSVSerialVec2 : public SpTRSVSerial {
    protected:
        sym_lib::timing_measurement fused_code() override {
            std::copy(x_in_, x_in_+n_, x_);
            sym_lib::timing_measurement t1;
            t1.start_timer();
            sparse_avx::sptrsv_csr_vec256_2(n_, L1_csr_->p, L1_csr_->i, L1_csr_->x, x_,x_);
            t1.measure_elapsed_time();
            //copy_vector(0,n_,x_in_,x_);
            return t1;
        }

    public:
        SpTRSVSerialVec2(sym_lib::CSR *L, sym_lib::CSC *L_csc,
                         double *correct_x,
                         std::string name) :
                SpTRSVSerial(L, L_csc, correct_x, name) {
            L1_csr_ = L;
            L1_csc_ = L_csc;
            correct_x_ = correct_x;
        };

        ~SpTRSVSerialVec2() override = default;
    };


    class SptrsvLevelSet : public SpTRSVSerial {
 protected:
  int *level_set, *level_ptr, level_no;
  void build_set() override {

   level_no = sym_lib::build_levelSet_CSC(L1_csc_->n, L1_csc_->p, L1_csc_->i,
                                 level_ptr, level_set);
  }

  sym_lib::timing_measurement fused_code() override {
   sym_lib::timing_measurement t1;
   t1.start_timer();
   sptrsv_csr_levelset(n_, L1_csr_->p, L1_csr_->i, L1_csr_->x, x_in_,
                       level_no, level_ptr, level_set, num_threads_);
   t1.measure_elapsed_time();
   sym_lib::copy_vector(0,n_,x_in_,x_);
   return t1;
  }

 public:
  SptrsvLevelSet (sym_lib::CSR *L, sym_lib::CSC *L_csc,
                  double *correct_x, std::string name) :
    SpTRSVSerial(L, L_csc, correct_x, name) {
   L1_csr_ = L;
   L1_csc_ = L_csc;
   correct_x_ = correct_x;
  };

  ~SptrsvLevelSet () override {
   delete []level_ptr;
   delete []level_set;
  };
 };


 class SptrsvLevelSetNovec : public SptrsvLevelSet {
 protected:
  sym_lib::timing_measurement fused_code() override {
   sym_lib::timing_measurement t1;
   t1.start_timer();
   sptrsv_csr_levelset_novec(n_, L1_csr_->p, L1_csr_->i, L1_csr_->x, x_in_,
                       level_no, level_ptr, level_set, num_threads_);
   t1.measure_elapsed_time();
   sym_lib::copy_vector(0,n_,x_in_,x_);
   return t1;
  }

 public:
  SptrsvLevelSetNovec (sym_lib::CSR *L, sym_lib::CSC *L_csc,
                  double *correct_x, std::string name) :
    SptrsvLevelSet(L, L_csc, correct_x, name) {
  };

  ~SptrsvLevelSetNovec () override = default;
 };

#ifdef MKL
 class SpTRSVMKL : public SpTRSVSerial {
     MKL_INT* LLI;
     matrix_descr d;
     sparse_matrix_t m;
     int num_threads;
     sym_lib::timing_measurement analysis_bw;

     void build_set() override {
         if (LLI != nullptr) {
             delete LLI;
         }
         analysis_bw.start_timer();
         sparse_operation_t opr = SPARSE_OPERATION_NON_TRANSPOSE;
         d.type = SPARSE_MATRIX_TYPE_TRIANGULAR;
         d.diag = SPARSE_DIAG_NON_UNIT;
         d.mode = SPARSE_FILL_MODE_LOWER;

         MKL_INT expected_calls = 5;

         LLI = new MKL_INT[this->L1_csr_->m+1]();
         for (int l = 0; l < this->L1_csr_->m+1; ++l) {
             LLI[l] = this->L1_csr_->p[l];
         }

         mkl_sparse_d_create_csr(&m, SPARSE_INDEX_BASE_ZERO, this->L1_csr_->m, this->L1_csr_->n,
                                 LLI, LLI+1, this->L1_csr_->i, this->L1_csr_->x);
         mkl_sparse_set_mv_hint(m, opr, d, expected_calls);

         mkl_set_num_threads(num_threads);
         analysis_bw.measure_elapsed_time();
     }
     sym_lib::timing_measurement fused_code() override {
         sym_lib::timing_measurement t1;
         t1.start_timer();
         mkl_sparse_d_trsv(SPARSE_OPERATION_NON_TRANSPOSE, 1, m, this->d, this->x_in_, this->x_);
         t1.measure_elapsed_time();
         sym_lib::copy_vector(0,n_,x_in_,x_);
         return t1;
     }



     ~SpTRSVMKL() override {
         delete[] this->LLI;
     }
 public:
     SpTRSVMKL(int nThreads, sym_lib::CSR *L, sym_lib::CSC *L_csc,
     double *correct_x,
             std::string name) :
             SpTRSVSerial(L, L_csc, correct_x, name), num_threads(nThreads), LLI(nullptr) {};
     sym_lib::timing_measurement get_analysis_bw(){return analysis_bw;}
 };
#endif

 class SpTRSVParallel : public SpTRSVSerial {
 protected:

  int final_level_no, *fina_level_ptr, *final_part_ptr, *final_node_ptr;
  int part_no;
  int lp_, cp_, ic_;
  bool b_pack;
  int tuning;
  sym_lib::timing_measurement analysis_bw;

  void build_set() override {
  if (final_level_no != 0) {
      return;
  }
  analysis_bw.start_timer();
   auto *cost = new double[n_]();
   for (int i = 0; i < n_; ++i) {
    cost[i] = L1_csr_->p[i+1] - L1_csr_->p[i];
   }
   if(tuning == 0){
    sym_lib::lbc_config(n_, L1_csc_->nnz, lp_, lp_, ic_, cp_,
                        b_pack); // Fixme: inconsistent naming for
    // ic/cp in lbc
   }
#define OLD
#ifdef OLD
      if (ic_ > 0) {
          sym_lib::get_coarse_levelSet_DAG_CSC_tree(n_, L1_csc_->p, L1_csc_->i,this->L1_csc_->stype,
                                               final_level_no,
                                               fina_level_ptr,part_no,
                                               final_part_ptr,final_node_ptr,
                                               lp_,ic_, cp_, cost);
      } else {
          // Build level set in-place
          final_level_no = sym_lib::build_levelSet_CSC(n_, L1_csc_->p, L1_csc_->i, final_part_ptr, final_node_ptr);
          fina_level_ptr = new int[final_level_no + 1];
          for (int i = 0; i < final_level_no+1; i++) {
              fina_level_ptr[i] = i;
          }
      }

#else
   sym_lib::get_coarse_Level_set_DAG_CSC03_parallel(n_, L1_csc_->p, L1_csc_->i,
                                                    final_level_no,
                                                    fina_level_ptr,part_no,
                                                    final_part_ptr,final_node_ptr,
                                                    lp_,ic_, cp_, cost,lp_,
                                                    b_pack);
#endif
   if (true) {
    auto part_no = fina_level_ptr[final_level_no];
    // Sorting the w partitions
    for (int i = 0; i < part_no; ++i) {
     std::sort(final_node_ptr + final_part_ptr[i],
               final_node_ptr + final_part_ptr[i + 1]);
    }
   }

   delete []cost;
   analysis_bw.measure_elapsed_time();
  }

  sym_lib::timing_measurement fused_code() override {
   sym_lib::timing_measurement t1;
   t1.start_timer();
   sptrsv_csr_lbc(n_, L1_csr_->p, L1_csr_->i, L1_csr_->x, x_in_,
                  final_level_no, fina_level_ptr,
                  final_part_ptr, final_node_ptr, num_threads_);
   t1.measure_elapsed_time();
   sym_lib::copy_vector(0,n_,x_in_,x_);
   return t1;
  }

 public:
  SpTRSVParallel(sym_lib::CSR *L, sym_lib::CSC *L_csc,
               double *correct_x,
               std::string name, int lp, int cp, int ic, int bp, int tun) :
    SpTRSVSerial(L, L_csc, correct_x, name), lp_(lp), cp_(cp), ic_(ic), part_no(0), final_level_no(0),
    b_pack(bp), tuning(tun) {
   L1_csr_ = L;
   L1_csc_ = L_csc;
   correct_x_ = correct_x;
  };

  sym_lib::timing_measurement get_analysis_bw(){return analysis_bw;}

  ~SpTRSVParallel(){
   delete []fina_level_ptr;
   delete []final_node_ptr;
   delete []final_part_ptr;
  }

  int CP(){return cp_;}
  int BP(){return b_pack;}
 };

    class SpTRSVParallelVec2 : public SpTRSVSerial {
    protected:

        int final_level_no, *fina_level_ptr, *final_part_ptr, *final_node_ptr;
        int part_no;
        int lp_, cp_, ic_;
        bool b_pack;
        int tuning;
        void build_set() override {
            auto *cost = new double[n_]();
            for (int i = 0; i < n_; ++i) {
                cost[i] = L1_csr_->p[i+1] - L1_csr_->p[i];
            }
//            if(tuning == 0){
//             sym_lib::lbc_config(n_, L1_csc_->nnz, lp_, lp_, ic_, cp_,
//                                 b_pack); // Fixme: inconsistent naming for
//                                 // ic/cp in lbc
//            }
#define OLD
#ifdef OLD
         sym_lib::get_coarse_levelSet_DAG_CSC(n_, L1_csc_->p, L1_csc_->i,
                                              final_level_no,
                                              fina_level_ptr,part_no,
                                              final_part_ptr,final_node_ptr,
                                              lp_,ic_, cp_, cost);
#else
         sym_lib::get_coarse_Level_set_DAG_CSC03_parallel(n_, L1_csc_->p, L1_csc_->i,
                                                          final_level_no,
                                                          fina_level_ptr,part_no,
                                                          final_part_ptr,final_node_ptr,
                                                          lp_,ic_, cp_, cost,
                                                          lp_, b_pack);
#endif
         if (true) {
          auto part_no = fina_level_ptr[final_level_no];
          // Sorting the w partitions
          for (int i = 0; i < part_no; ++i) {
           std::sort(final_node_ptr + final_part_ptr[i],
                     final_node_ptr + final_part_ptr[i + 1]);
          }
         }


            delete []cost;
        }

        sym_lib::timing_measurement fused_code() override {
            sym_lib::timing_measurement t1;
            t1.start_timer();
            sparse_avx::sptrsv_csr_lbc_vec_2(n_, L1_csr_->p, L1_csr_->i, L1_csr_->x, x_in_,
                           final_level_no, fina_level_ptr,
                           final_part_ptr, final_node_ptr, num_threads_);
            t1.measure_elapsed_time();
            sym_lib::copy_vector(0,n_,x_in_,x_);
            return t1;
        }

    public:
        SpTRSVParallelVec2(sym_lib::CSR *L, sym_lib::CSC *L_csc,
                       double *correct_x,
                       std::string name, int lp, int cp, int ic, int bp, int
                       tun) :
                SpTRSVSerial(L, L_csc, correct_x, name), lp_(lp), cp_(cp),
                ic_(ic), tuning(tun) {
            L1_csr_ = L;
            L1_csc_ = L_csc;
            correct_x_ = correct_x;
            b_pack = bp;
        };

        ~SpTRSVParallelVec2(){
            delete []fina_level_ptr;
            delete []final_node_ptr;
            delete []final_part_ptr;
        }
    };


 class SpTRSVDDT : public SpTRSVSerial {
 protected:
  bool memoryAllocated = false;
  DDT::Config config;
  std::vector<DDT::Codelet*>* cl;
  DDT::GlobalObject d;
  sym_lib::timing_measurement analysis_breakdown;
  int lp_, cp_, ic_;

  void build_set() override {
      if (memoryAllocated) {
          return;
      }
      analysis_breakdown.start_timer();
      // Allocate memory and generate global object
      if (config.nThread != 1) {
          d = DDT::init(this->L1_csr_, this->L1_csc_,config);
          memoryAllocated = true;
      } else {
          d = DDT::init(this->L1_csr_, config);
          memoryAllocated = true;
      }
   if (config.nThread == 1) {
       this->cl = new std::vector<DDT::Codelet*>();
       DDT::inspectSerialTrace(d, this->cl, config);
   } else {
       DDT::inspectParallelTrace(d, config);
       if (config.analyze) { DDT::analyzeData(d, d.sm->_cl, config); }
   }
   analysis_breakdown.measure_elapsed_time();
  }

  sym_lib::timing_measurement fused_code() override {
   std::copy(x_in_, x_in_+n_, x_);
   sym_lib::timing_measurement t1;
   DDT::Args args; args.x = x_; args.y = x_;
   args.r = L1_csr_->m; args.Lp=L1_csr_->p; args.Li=L1_csr_->i;
   args.Lx = L1_csr_->x;


   // Execute codes
   if (config.nThread == 1) {
       t1.start_timer();
       DDT::executeCodelets(this->cl, config, args);
       t1.measure_elapsed_time();
   } else {
       t1.start_timer();
       DDT::executeParallelCodelets(d, config, args);
       t1.measure_elapsed_time();
   }


   //copy_vector(0,n_,x_in_,x_);
   return t1;
  }

 public:
  SpTRSVDDT(sym_lib::CSR *L, sym_lib::CSC *L_csc,
          double *correct_x, DDT::Config &conf,
          std::string name, int lp, int cp, int ic) :
    SpTRSVSerial(L, L_csc, correct_x, name), config(conf), lp_(lp), cp_(cp), cl(nullptr),
    ic_(ic) {
  };

  sym_lib::timing_measurement get_analysis_bw(){return analysis_breakdown;}

  ~SpTRSVDDT(){
    DDT::free(d);
    delete[] cl;
//   DDT::free(cl);
  }
 };
};

#endif //DDT_SPTRSV_DEMO_UTILS_H
