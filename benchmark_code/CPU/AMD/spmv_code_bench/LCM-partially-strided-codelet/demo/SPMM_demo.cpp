//
// Created by cetinicz on 2021-10-30.
//

#include "SPMM_demo_utils.h"
#include <Input.h>
#include <def.h>
#include <sparse_io.h>
#include <sparse_utilities.h>
#include <SparseMatrixIO.h>

#ifdef PAPI
#include "Profiler.h"
#endif
template<class type>
sym_lib::CSR *convert_dcsr_scsr(type A){
 int r = A.r;
 int c = A.c;
 int nnz = A.nz;
 sym_lib::CSR *scsr = new sym_lib::CSR(r,c,nnz);
 sym_lib::copy_vector(0,r, A.Lp, scsr->p);
 sym_lib::copy_vector(0, nnz, A.Li, scsr->i);
 sym_lib::copy_vector(0, nnz, A.Lx, scsr->x);
 return scsr;
}

int main(int argc, char *argv[]) {
    auto config = DDT::parseInput(argc, argv);
    //auto A = sym_lib::read_mtx(config.matrixPath);
    auto As = DDT::readSparseMatrix<DDT::CSR>(config.matrixPath);
    auto B = convert_dcsr_scsr<DDT::Matrix>(As);
    sym_lib::CSC *A = new sym_lib::CSC(B->m, B->n, B->nnz);
    //sym_lib::csr_to_csc(B);/ FIXME: this is not correct
#ifdef PROFILE
    /// Profiling
    int event_limit = 1, instance_per_run = 5;
    auto event_list = sym_lib::get_available_counter_codes();
#endif

    int bRows = B->n;
    int bCols = config.bMatrixCols;

    auto final_solution = new double[B->m*bCols]();

#ifdef PROFILE
    auto *sps = new sparse_avx::SpMMSerial(B, A_full, bRows, bCols, "SpMM Serial");
    auto spmm_baseline = sps->evaluate();
    double *sol_spmm = sps->solution();
    std::copy(sol_spmm,sol_spmm+A_full->m*bCols,final_solution);
    delete sps;

    auto *spmm_profiler = new sym_lib::ProfilerWrapper<sparse_avx::SpMMTiledParallel>(event_list, event_limit, 1,B, A_full, final_solution, bRows, bCols,config.mTileSize,config.nTileSize,"SpMM Tiled Parallel");
    spmm_profiler->d->set_num_threads(config.nThread);
    spmm_profiler->profile(config.nThread);
    if (config.header) {
      std::cout << "Matrix,mTileSize,nTileSize,Matrix B Columns,";spmm_profiler->print_headers();std::cout << '\n';
    }
    std::cout << config.matrixPath << "," << config.mTileSize << "," << config.nTileSize << "," << config.bMatrixCols << ",";
    spmm_profiler->print_counters();
    std::cout << "\n";
#else

    auto *sps = new sparse_avx::SpMMSerial(B, A, bRows, bCols, "SpMM Serial");
    auto spmm_baseline = sps->evaluate();
    double *sol_spmm = sps->solution();
    std::copy(sol_spmm,sol_spmm+B->m*bCols,final_solution);
    delete sps;

    auto *spsp = new sparse_avx::SpMMParallel(B, A, final_solution, bRows, bCols, "SpMM Parallel");
    spsp->set_num_threads(config.nThread);
    auto spmm_parallel_baseline = spsp->evaluate();
    auto spmm_parallel_baseline_elapsed = spmm_parallel_baseline.elapsed_time;
    delete spsp;

#ifdef PERMUTED
    auto *spPermuted = new sparse_avx::SpMMPermutedParallel(B, A_full, final_solution, bRows, bCols, "SpMM Permuted Parallel");
    spPermuted->set_num_threads(config.nThread);
    auto spmm_permuted_parallel_baseline = spPermuted->evaluate();
    auto spmm_permuted_parallel_baseline_elapsed = spmm_permuted_parallel_baseline.elapsed_time;
    delete spPermuted;
#endif

    auto *sp_tiled = new sparse_avx::SpMMTiledParallel(B, A, final_solution, bRows, bCols,config.mTileSize,config.nTileSize,"SpMM Tiled Parallel");
    sp_tiled->set_num_threads(config.nThread);
    auto spmm_tiled_parallel_baseline = sp_tiled->evaluate();
    auto spmm_tiled_parallel_baseline_elapsed = spmm_tiled_parallel_baseline.elapsed_time;
    delete sp_tiled;

    auto *spmkl = new sparse_avx::SpMMMKL(B, A, final_solution, bRows, bCols, "SpMM MKL");
    spmkl->set_num_threads(config.nThread);
    auto spmm_mkl_eval = spmkl->evaluate();
    auto spmm_mkl_eval_elapsed = spmm_mkl_eval.elapsed_time;
    auto spmm_mkl_analysis_elapsed = spmkl->get_analysis_bw();
    delete spmkl;
/*
    auto *spddtser = new sparse_avx::SpMMDDT(B, A, final_solution, config, bRows, bCols, "SpMM DDT");
    spddtser->set_num_threads(1);
    auto spmm_ddt_eval_ser = spddtser->evaluate();
    auto spmm_ddt_eval_elapsed_ser = spmm_ddt_eval_ser.elapsed_time;
    auto spmm_ddt_analysis_elapsed = spddtser->get_analysis_bw();
    delete spddtser;
*/
    auto *spddt = new sparse_avx::SpMMDDT(B, A, final_solution, config, bRows, bCols, "SpMM DDT");
    spddt->set_num_threads(config.nThread);
    auto spmm_ddt_eval = spddt->evaluate();
    auto spmm_ddt_eval_elapsed = spmm_ddt_eval.elapsed_time;
    auto spmm_ddtp_analysis_elapsed = spddt->get_analysis_bw();
    delete spddt;


    if (config.header) {
      std::cout << "Matrix,nRows,nCols,NNZ,mTileSize,nTileSize,bCols,SpMM "
        "Baseline,SpMM Parallel,SpMM Tiled Parallel,SpMM MKL,SpMM DDT,SpMM DDT Analysis";

#ifdef PERMUTED
      std::cout << ",SpMM Permuted Parallel"
#endif
        std::cout << "\n";
    }
    std::cout << config.matrixPath << "," << B->m << "," << B->n << "," << B->nnz << "," << config.mTileSize
    << "," << config.nTileSize << "," << config.bMatrixCols << "," <<spmm_baseline.elapsed_time << ","
    << spmm_parallel_baseline_elapsed << "," << spmm_tiled_parallel_baseline_elapsed << ","
    << spmm_mkl_eval_elapsed << "," //<< spmm_ddt_eval_elapsed_ser<< "," 
    << spmm_ddt_eval_elapsed <<","
    << spmm_ddtp_analysis_elapsed.elapsed_time;

#ifdef PERMUTED
    std::cout << "," << spmm_permuted_parallel_baseline_elapsed;
#endif
    std::cout << "\n";
#endif

    delete A;
    delete B;
    delete[] final_solution;
}
