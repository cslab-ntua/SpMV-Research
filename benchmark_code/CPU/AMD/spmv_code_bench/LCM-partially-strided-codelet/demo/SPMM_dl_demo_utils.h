//
// Created by kazem on 6/17/22.
//

#ifndef DDT_SPMM_DL_DEMO_UTILS_H
#define DDT_SPMM_DL_DEMO_UTILS_H
#include "FusionDemo.h"
#include "SparseMatrixIO.h"
#include <DDT.h>
#include <DDTCodelets.h>
#include <Executor.h>
#include <sparse_io.h>

#include <mkl.h>

int tmp = 0;

template<class type>
sym_lib::CSR *convert_dcsr_scsr(type A){
 int r = A.r;
 int c = A.c;
 int nnz = A.nz;
 sym_lib::CSR *scsr = new sym_lib::CSR(r,c,nnz);
 sym_lib::copy_vector(0,r, A.Lp, scsr->p);
 sym_lib::copy_vector(0, nnz, A.Li, scsr->i);
 sym_lib::copy_vector(0, nnz, A.Lx, scsr->x);
 scsr->stype = A.stype;
 return scsr;
}

struct spmm_config{// mxn -> A in C = A*B
 int m_tile, bcol_tile, n_tile;
 spmm_config():m_tile(64),bcol_tile(64),n_tile(64){}
};

class GEMMSpMM : public sym_lib::FusionDemo {
protected:
 float * Bx;
 int aRows, aCols, bCols;
 float *d_A, *Cx;
 float *correct_cx;
 spmm_config SC;

 void setting_up() override{
  auto dense_size = aRows * aCols;
  d_A = new float[dense_size]();
  sym_lib::compressed2dense<float,double>(L1_csr_->m, L1_csr_->n, L1_csr_->p,
                                          L1_csr_->i, L1_csr_->x,
                                          L1_csr_->stype, d_A);
  //sym_lib::print_vec("A: \n", 0, tmp, d_A);
  //sym_lib::print_vec("\n B: \n", 0, tmp, Bx);
 }

 sym_lib::timing_measurement fused_code() override {
  mkl_set_num_threads(num_threads_);
  mkl_set_num_threads_local(num_threads_);
  std::fill_n(Cx, aRows*bCols, 0);
  sym_lib::timing_measurement t1;
  t1.start_timer();
  cblas_sgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans,
                           aRows,  bCols, aCols,
                           1.,              // alpha
                           d_A,aCols,  // lda = t.k()
                           Bx, bCols,  // ldb = t.n()
                           0.,              // beta
                           Cx,bCols   // ldc = t.n()
  );
  t1.measure_elapsed_time();
  // sym_lib::print_vec("\n C: \n", 0, tmp, Cx);
  //            std::copy(x_,x_+L1_csr_->m*cbb,x_);
  return t1;
 }

 void testing() override{
  if(correct_cx)
   if (!sym_lib::is_equal<float>(0, aCols*bCols, correct_cx, Cx, 1e-6))
     PRINT_LOG(name_ + " code != reference solution.\n");
 }

public:
 GEMMSpMM(sym_lib::CSR *L, sym_lib::CSC *L_csc, int num_threads, int bCols,
            float *correct, spmm_config sc, std::string name)
   : FusionDemo(1, name) {
  L1_csr_ = L;
  L1_csc_ = L_csc;
  this->aRows = L_csc->m;
  this->aCols = L_csc->n;
  this->bCols = bCols;
  correct_cx = d_A = NULLPNTR;
  this->Bx = new float[aCols*bCols]();
  this->Cx = new float[aCols*bCols]();
  for (int i = 0; i < aCols*bCols; i++) {
   this->Bx[i] = 1;
  }
  num_threads_ = num_threads;
  num_test_ = 15;
  correct_cx = correct;
  SC =sc;
 };

 ~GEMMSpMM() override {
  delete[] Bx;
  delete[] Cx;
 }

 float *get_Cx(){
  return Cx;
 }
};

void gemm_base(const int m, const int bcol, const int n, const float *a,
                  const float *b, float *c){
 for(int i=0; i<m; i++){
  for(int j=0; j<bcol; j++){
   c[i*bcol+j] = 0;
   for(int k=0; k<n; k++)
    c[i*bcol+j]+=a[i*n+k]*b[k*bcol+j];//c[i][j]+=a[i][k]*b[k][j];
  }
 }
}

void gemm_tuned_C(const int m, const int bcol, const int n, const float *a,
                  const float *b, float *c, spmm_config sc) {
 const int mt = sc.m_tile, nt = sc.n_tile, bct = sc.bcol_tile;
 const int M = m / mt, Mr = m % mt;
 const int N = n / nt, Nr = n % nt;
 const int bCol = bcol / bct, bColr = bCol % bct;

 for (int i0 = 0; i0 < M; i0++) {
  for (int j0 = 0; j0 < bCol; j0++) {
   for (int k0 = 0; k0 < N; k0++) {
    /// GEMM
/*    for (int i = i0 * mt; i < (i0 + 1) * mt; i++) {
     for (int j = j0 * bct; j < (j0 + 1) * bct; j++) {
     // c[i * bcol + j] = 0;
      for (int k = k0 * nt; k < (k0 + 1) * nt; k++) {
       c[i * bcol + j] +=
         a[i * n + k] * b[k * bcol + j]; //c[i][j]+=a[i][k]*b[k][j];
      }
     }
    }*/
    const float *a_tile = a+(i0*mt*n + k0*nt);
    const float *b_tile = b+(k0*nt*bcol + j0*bct);
    float *c_tile = c + (i0*mt*bcol + j0*bct);
    cblas_sgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans,
                mt,  bct, nt,
                1.,              // alpha
                a_tile,n,  // lda = t.k()
                b_tile, bcol,  // ldb = t.n()
                1.,              // beta
                c_tile,bcol   // ldc = t.n()
    );
   }// k0
   /// remainder columns
   /// GEMM
/*
   for (int i = i0 * mt; i < (i0 + 1) * mt; i++) {
      for (int j = j0 * bct; j < (j0 + 1) * bct; j++) {
       // c[i * bcol + j] = 0;
       for(int k=n-Nr; k<n; k++){
        c[i * bcol + j] +=
          a[i * n + k] * b[k * bcol + j]; //c[i][j]+=a[i][k]*b[k][j];
      }
     }
    }
*/
   int k0 = n-Nr;
   const float *a_tile = a+(i0*mt*n + k0);
   const float *b_tile = b+(k0*bcol + j0*bct);
   float *c_tile = c + (i0*mt*bcol + j0*bct);
   cblas_sgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans,
               mt,  bct, Nr,
               1.,              // alpha
               a_tile,n,  // lda = t.k()
               b_tile, bcol,  // ldb = t.n()
               1.,              // beta
               c_tile,bcol   // ldc = t.n()
   );
  }


 }

 /// Tile remainder rows
 // GEMM
/*
 for(int i=m-Mr; i<m; i++){
  for(int j=0; j<bcol; j++){
   //c[i*bcol+j] = 0;
   for(int k=0; k<n; k++)
    c[i*bcol+j]+=a[i*n+k]*b[k*bcol+j];//c[i][j]+=a[i][k]*b[k][j];
  }
 }
*/

 int i0 = m-Mr;
 const float *a_tile = a+(i0*n);
 const float *b_tile = b;
 float *c_tile = c + (i0*bcol );
 cblas_sgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans,
             Mr,  bcol, n,
             1.,              // alpha
             a_tile,n,  // lda = t.k()
             b_tile, bcol,  // ldb = t.n()
             1.,              // beta
             c_tile,bcol   // ldc = t.n()
 );
}

class GEMMSpMMTuned : public GEMMSpMM{
 sym_lib::timing_measurement fused_code() override {
  mkl_set_num_threads(num_threads_);
  mkl_set_num_threads_local(num_threads_);
  std::fill_n(Cx, aRows*bCols, 0);
  sym_lib::timing_measurement t1;
  t1.start_timer();
  gemm_tuned_C(aRows, bCols, aCols, d_A, Bx, Cx, SC);
  t1.measure_elapsed_time();
  //sym_lib::print_vec("\n C2: \n", 0, tmp, Cx);
  return t1;
 }

public:
 GEMMSpMMTuned(sym_lib::CSR *L, sym_lib::CSC *L_csc, int num_threads, int bCols,
   float *correct, spmm_config sc, std::string name): GEMMSpMM(L,L_csc,
                                                            num_threads,bCols,
                                              correct, sc, name){}
};

inline constexpr const char *mkl_error_string(sparse_status_t status) {
 switch (status) {
  case SPARSE_STATUS_SUCCESS: return "SPARSE_STATUS_SUCCESS";
  case SPARSE_STATUS_NOT_INITIALIZED: return "SPARSE_STATUS_NOT_INITIALIZED";
  case SPARSE_STATUS_ALLOC_FAILED: return "SPARSE_STATUS_ALLOC_FAILED";
  case SPARSE_STATUS_INVALID_VALUE: return "SPARSE_STATUS_INVALID_VALUE";
  case SPARSE_STATUS_EXECUTION_FAILED: return "SPARSE_STATUS_EXECUTION_FAILED";
  case SPARSE_STATUS_INTERNAL_ERROR: return "SPARSE_STATUS_INTERNAL_ERROR";
  case SPARSE_STATUS_NOT_SUPPORTED: return "SPARSE_STATUS_NOT_SUPPORTED";
  default: return "Unknown";
 }
}

class SpMMMKL : public GEMMSpMM {
protected:
 sym_lib::timing_measurement analysis_breakdown;
 matrix_descr d;
 sparse_matrix_t m;
 MKL_INT *LLI;
 float *Ax;

 void build_set() override {
  //if (LLI == nullptr) {
   analysis_breakdown.start_timer();
   d.type = SPARSE_MATRIX_TYPE_GENERAL;
   //         d.diag = SPARSE_DIAG_NON_UNIT;
   //         d.mode = SPARSE_FILL_MODE_FULL;

   MKL_INT expected_calls = 5;
   LLI = new MKL_INT[this->L1_csr_->m + 1]();
   for (int l = 0; l < this->L1_csr_->m + 1; ++l) {
    LLI[l] = this->L1_csr_->p[l];
   }
   Ax = new float[L1_csr_->nnz]();
   for (int i = 0; i < L1_csr_->nnz; ++i) {
    Ax[i] = (float)L1_csr_->x[i];
   }

   auto stat = mkl_sparse_s_create_csr(&m, SPARSE_INDEX_BASE_ZERO,
                           this->L1_csr_->m, this->L1_csr_->n, LLI,
                           LLI + 1, this->L1_csr_->i, Ax);
   //         mkl_sparse_set_mv_hint(m, SPARSE_OPERATION_NON_TRANSPOSE, d, expected_calls);
   //         mkl_sparse_set_memory_hint(m, SPARSE_MEMORY_AGGRESSIVE);
   analysis_breakdown.measure_elapsed_time();
//  }
 }

 sym_lib::timing_measurement fused_code() override {
  sym_lib::timing_measurement t1;
  mkl_set_num_threads(num_threads_);
  mkl_set_num_threads_local(num_threads_);
  std::fill_n(Cx, aRows*bCols, 0);
  t1.start_timer();
  auto stat = mkl_sparse_s_mm(SPARSE_OPERATION_NON_TRANSPOSE, 1, m, this->d,
                  SPARSE_LAYOUT_ROW_MAJOR, Bx, bCols,
                  bCols, 0, Cx, bCols);
  t1.measure_elapsed_time();

  std::cout<<"SPMM MKL : "<<mkl_error_string(stat)<<"\n";
  return t1;
 }

public:
 SpMMMKL(sym_lib::CSR *L, sym_lib::CSC *L_csc, int num_threads, int bCols,
 float *correct, spmm_config sc, std::string name): GEMMSpMM(L,L_csc,
   num_threads,bCols,
   correct, sc, name){
  LLI = NULLPNTR;
  L1_csr_ = L;
  L1_csc_ = L_csc;
 }

 sym_lib::timing_measurement get_analysis_bw() {
  return analysis_breakdown;
 }

 ~SpMMMKL() override {
  //            mkl_free(LLI);
  delete []Ax;
 };
};

class SpMMPadded : public GEMMSpMM{
protected:
 matrix_descr d;
 sparse_matrix_t m;
 MKL_INT *LLI;
 float *Ax;
 void build_set() override {
  auto As = DDT::readSparseMatrix<DDT::CSR>("/home/kazem/Downloads/multihead_fully_70_sparse_reordered.mtx");
  auto B = convert_dcsr_scsr<DDT::Matrix>(As);
  d_A = new float[208*512]();
  L1_csr_ = B;
  //if (LLI == nullptr) {
   d.type = SPARSE_MATRIX_TYPE_GENERAL;
   //         d.diag = SPARSE_DIAG_NON_UNIT;
   //         d.mode = SPARSE_FILL_MODE_FULL;

   MKL_INT expected_calls = 5;
   LLI = new MKL_INT[this->L1_csr_->m + 1]();
   for (int l = 0; l < this->L1_csr_->m + 1; ++l) {
    LLI[l] = this->L1_csr_->p[l];
   }
   Ax = new float[L1_csr_->nnz]();
   for (int i = 0; i < L1_csr_->nnz; ++i) {
    Ax[i] = (float)L1_csr_->x[i];
   }

   auto status = mkl_sparse_s_create_csr(&m, SPARSE_INDEX_BASE_ZERO,
                           this->L1_csr_->m, this->L1_csr_->n, LLI,
                           LLI + 1, this->L1_csr_->i, Ax);

  // std::cout<<mkl_error_string(status)<<"\n";
   //         mkl_sparse_set_mv_hint(m, SPARSE_OPERATION_NON_TRANSPOSE, d, expected_calls);
   //         mkl_sparse_set_memory_hint(m, SPARSE_MEMORY_AGGRESSIVE);
 // }
 }


 sym_lib::timing_measurement fused_code() override {
  sym_lib::timing_measurement t1, tt;
  mkl_set_num_threads(num_threads_);
  mkl_set_num_threads_local(num_threads_);
  std::fill_n(Cx, aRows*bCols, 0);
  tt.start_timer();
  t1.start_timer();
  auto status = mkl_sparse_s_mm(SPARSE_OPERATION_NON_TRANSPOSE, 1, m, this->d,
                  SPARSE_LAYOUT_ROW_MAJOR, Bx, bCols,
                  bCols, 0, Cx, bCols);
  tt.start_timer();
  cblas_sgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans,
              aRows,  bCols, 207,
              1.,              // alpha
              d_A,207,  // lda = t.k()
              Bx, bCols,  // ldb = t.n()
              1.,              // beta
              Cx,bCols   // ldc = t.n()
  );
  t1.measure_elapsed_time();
  tt.measure_elapsed_time();
  tt.print_t_array();
  std::cout<<"--> : "<<mkl_error_string(status)<<"\n";
  return t1;
 }

public:
 SpMMPadded(sym_lib::CSR *L, sym_lib::CSC *L_csc, int num_threads, int bCols,
               float *correct, spmm_config sc, std::string name): GEMMSpMM(L,L_csc,
                                                                           num_threads,bCols,
                                                                           correct, sc, name){}
};
#endif //DDT_SPMM_DL_DEMO_UTILS_H
