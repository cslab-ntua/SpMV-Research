//
// Created by kazem on 6/17/22.
//
#include "SPMM_dl_demo_utils.h"
#include <Input.h>
#include <def.h>
#include <sparse_io.h>
#include <sparse_utilities.h>
#include <SparseMatrixIO.h>


#ifdef PAPI
#include "Profiler.h"
#endif


int main(int argc, char *argv[]) {
 auto config = DDT::parseInput(argc, argv);
 //auto A = sym_lib::read_mtx(config.matrixPath);
 auto As = DDT::readSparseMatrix<DDT::CSR>(config.matrixPath);
 auto B = convert_dcsr_scsr<DDT::Matrix>(As);
 sym_lib::CSC *A = new sym_lib::CSC(B->m, B->n, B->nnz);
#ifdef PROFILE
 /// Profiling
    int event_limit = 1, instance_per_run = 5;
    auto event_list = sym_lib::get_available_counter_codes();
#endif
 int bCols = config.bMatrixCols;
 auto final_solution = new float[B->n*bCols]();
 int num_thread = config.nThread;
 MKL_Set_Num_Threads(num_thread);
 MKL_Domain_Set_Num_Threads(num_thread, MKL_DOMAIN_BLAS);

 spmm_config sc_in{};
 sc_in.m_tile = config.mTileSize;
 sc_in.n_tile = config.nTileSize;

 auto *sps = new GEMMSpMM(B, A, num_thread, bCols, NULLPNTR, sc_in, "SpMM "
                                                                    "Serial");
 auto mkl_gemm = sps->evaluate();
 auto *sol_gemm = sps->get_Cx();
 std::copy(sol_gemm,sol_gemm+B->n*bCols,final_solution);
 delete sps;


 auto *gemmt = new GEMMSpMMTuned(B, A, num_thread, bCols, final_solution, sc_in,
                                 "SpMM Serial");
 auto mkl_gemm_t1 = gemmt->evaluate();
 delete gemmt;

 auto *spmmt = new SpMMMKL(B, A, num_thread, bCols, final_solution, sc_in,
                                 "SpMM MKL Serial");
 auto mkl_spmm = spmmt->evaluate();
 delete spmmt;

 auto *spmm_hybrid = new SpMMPadded(B, A, num_thread, bCols, final_solution, sc_in,
                           "SpMM MKL Hybrid");
 auto mkl_spmm_hybrid = spmm_hybrid->evaluate();
 delete spmm_hybrid;

 if (config.header) {
  std::cout << "Matrix,nRows,nCols,NNZ,mTileSize,nTileSize,"
               "Number of Threads,B Cols,MKL GEMM,GEMM Tuned1,"
               "MKL SpMM,";
  std::cout << "\n";
 }
 std::cout << config.matrixPath << "," << B->m << "," << B->n
           << "," << B->nnz << "," << config.mTileSize
           << "," << config.nTileSize << "," <<config.nThread  << "," << config
           .bMatrixCols << "," <<mkl_gemm.elapsed_time << ","<<
           mkl_gemm_t1.elapsed_time<<","<<mkl_spmm.elapsed_time<<","<<
           mkl_spmm_hybrid.elapsed_time<<",";
 std::cout << "\n";

 delete A;
 delete B;
 delete[] final_solution;
}
