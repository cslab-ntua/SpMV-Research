//
// Created by kazem on 6/22/22.
//


#include <algorithm>
#include <iostream>

#include "SPMM_dl_demo_utils.h"
#include "utils.h"

int main(int argc, char *argv[]) {
 int aRows=32, aCols=64, bCols=16;
 aRows = atoi(argv[1]);
 aCols = atoi(argv[2]);
 bCols = atoi(argv[3]);

 spmm_config sc;
 sc.m_tile=atoi(argv[4]);
 sc.n_tile=atoi(argv[5]);
 sc.bcol_tile=atoi(argv[6]);
 int num_threads = atoi(argv[7]);

 mkl_set_num_threads(num_threads);
 mkl_set_num_threads_local(num_threads);
 auto *A = new float[aRows * aCols]();
 auto *B = new float[aCols * bCols]();
 auto *C = new float[aCols * bCols]();
 std::fill_n(A, aRows * aCols, 0.2);
 std::fill_n(B, aCols * bCols, 0.1);

 int num_test = 11;

 /// GEMM
 sym_lib::timing_measurement median_gemm;
 std::vector<sym_lib::timing_measurement> gemm_time_array;
 for (int i = 0; i < num_test; ++i) {
  sym_lib::timing_measurement t1;
  std::fill_n(C, aCols * bCols, 0);
  t1.start_timer();
  cblas_sgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans,
              aCols, bCols, aRows,
              1.,              // alpha
              A, aRows,  // lda = t.k()
              B, bCols,  // ldb = t.n()
              1.,              // beta
              C, bCols   // ldc = t.n()
  );
  t1.measure_elapsed_time();
  gemm_time_array.emplace_back(t1);
 }
 median_gemm = sym_lib::time_median(gemm_time_array);

 /// Tiled GEMM
 auto *Ct = new float[aCols * bCols]();
 sym_lib::timing_measurement median_t_gemm;
 std::vector<sym_lib::timing_measurement> t_gemm_time_array;
 for (int i = 0; i < num_test; ++i) {
  sym_lib::timing_measurement t1;
  std::fill_n(Ct, aCols * bCols, 0);
  t1.start_timer();
  gemm_tuned_C(aCols, bCols, aRows, A, B, Ct, sc);
  t1.measure_elapsed_time();
  t_gemm_time_array.emplace_back(t1);
 }
 median_t_gemm = sym_lib::time_median(t_gemm_time_array);

 for (int i = 0; i < aCols * bCols; ++i) {
  if(std::abs(C[i]-Ct[i]) > 1e-4)
   std::cout<<"error in "<<i<< " "<< C[i] <<" "<<Ct[i]<< "\n";
 }

 /// Print stats
 std::cout << aRows << "," << aCols << "," << bCols << ",";
 std::cout << sc.m_tile << "," << sc.n_tile << "," << sc.bcol_tile << ",";
 std::cout<< median_t_gemm.elapsed_time << ","<<median_gemm.elapsed_time<<",";


 delete []A;
 delete []B;
 delete []C;
 delete []Ct;
 return 1;
}