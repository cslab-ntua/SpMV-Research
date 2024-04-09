//
// Created by kazem on 7/13/21.
//
#include "SPMV_demo_utils.h"

#include "DDTDef.h"
#include <def.h>
#include <sparse_io.h>
#include <sparse_utilities.h>

// #define PROFILE

// #ifdef PAPI
// #include "Profiler.h"
// #endif

#define ELLPACK

using namespace sparse_avx;
int main(int argc, char *argv[]) {
    auto config = DDT::parseInput(argc, argv);
    auto A = sym_lib::read_mtx(config.matrixPath);
    sym_lib::CSR *B = sym_lib::csc_to_csr(A);

#ifdef PROFILE
    /// Profiling
    int event_limit = 1, instance_per_run = 5;
    auto event_list = sym_lib::get_available_counter_codes();
#endif

    auto *sps = new SpMVSerial(B, A, NULLPNTR, "Baseline SpMV");
    auto spmv_baseline = sps->evaluate();
    double *sol_spmv = sps->solution();

#ifndef PROFILE
//    auto spspre = new SpMVSerialPrefetch(B,A, sol_spmv, "Prefetched SpMV", config.hint, config.prefetch_distance);
//    auto sol_spmv_pre = spspre->evaluate();

    auto *spsp = new SpMVParallel(B, A, sol_spmv, "SpMV Parallel");
    spsp->set_num_threads(config.nThread);
    auto spmv_p = spsp->evaluate();
    delete spsp;


    auto spmvp = new SpMVVec1Parallel(B,A,sol_spmv, "SpMVVec1_Parallel");
    spmvp->set_num_threads(config.nThread);
    auto spmv1pe = spmvp->evaluate();
    delete spmvp;

    auto *spmv1 = new SpMVVec1(B, A, sol_spmv, "SpMVVec1_4");
    auto spmv1e = spmv1->evaluate();
    delete spmv1;

    auto *spmv1_8 = new SpMVVec1_8(B, A, sol_spmv, "SpMVVec1_8");
    auto spmv1_8e = spmv1_8->evaluate();
    delete spmv1_8;

    auto *spmv1_16 = new SpMVVec1_16(B, A, sol_spmv, "SpMVVec1_16");
    auto spmv1_16e = spmv1_16->evaluate();
    delete spmv1_16;

    auto *spmv2 = new SpMVVec2(B, A, sol_spmv, "SpMVVec2");
    auto spmv2e = spmv2->evaluate();
    delete spmv2;

#ifdef MKL
    auto mklspmvst = new SpMVMKL(1, B, A, sol_spmv, "MKL SPMV ST");
    auto mkl_exec_st = mklspmvst->evaluate();
    delete mklspmvst;

    auto mklspmvmt = new SpMVMKL(config.nThread, B, A, sol_spmv, "MKL SPMV MT");
    mklspmvmt->set_num_threads(config.nThread);
    auto mkl_exec_mt = mklspmvmt->evaluate();
    auto mkl_analysis_bw = mklspmvmt->get_analysis_bw();
    delete mklspmvmt;


#endif
#endif

#ifdef PROFILE
    auto matrixName = config.matrixPath;
    auto *ddt_profiler = new sym_lib::ProfilerWrapper<SpMVDDT>(event_list, event_limit, 1,B,A, sol_spmv, config, "SpMV DDT ST");
    ddt_profiler->profile(config.nThread);
    matrixName.erase(std::remove(matrixName.begin(), matrixName.end(), '\n'), matrixName.end());
    if (config.header) { std::cout << "Matrix,nRows,nCols,NNZ,Threads,KERNEL_TYPE,codelet_min_width, codelet_max_distance, only_fsc_codelets,"; ddt_profiler->print_headers(); std::cout << "\n"; }
    std::cout << matrixName << "," <<  B->m << "," << B->n << ","  << B->nnz << "," << config.nThread << "," << "DDT,";
    std::cout << DDT::clt_width << "," << DDT::col_th << "," << DDT::prefer_fsc << ",";
    ddt_profiler->print_counters();
    std::cout << "\n";

    auto *spmv_profiler = new sym_lib::ProfilerWrapper<SpMVParallel>(event_list, event_limit, 1,B,A, sol_spmv, "CSR SPMV BASELINE");
    spmv_profiler->d->set_num_threads(config.nThread);
    spmv_profiler->profile(config.nThread);
    std::cout << matrixName << "," <<  B->m << "," << B->n << ","  << B->nnz << "," << config.nThread << "," << "BASELINE,";
    std::cout << 0 << "," << 0 << "," << 0 << ",";
    spmv_profiler->print_counters();
    std::cout << "\n";

    auto *mkl_spmv_profiler = new sym_lib::ProfilerWrapper<SpMVMKL>(event_list, event_limit, 1, config.nThread,B,A, sol_spmv, "MKL");
    mkl_spmv_profiler->profile(config.nThread);
    std::cout << matrixName << "," <<  B->m << "," << B->n << ","  << B->nnz << "," << config.nThread << "," << "MKL,";
    std::cout << 0 << "," << 0 << "," << 0 << ",";
    mkl_spmv_profiler->print_counters();
    std::cout << "\n";

    auto *spmvp_profiler = new sym_lib::ProfilerWrapper<SpMVVec1Parallel>(event_list, event_limit, 1, B,A,sol_spmv, "SpMVVec1_Parallel");
    spmvp_profiler->d->set_num_threads(config.nThread);
    spmvp_profiler->profile(config.nThread);
    std::cout << matrixName << "," <<  B->m << "," << B->n << ","  << B->nnz << "," << config.nThread << "," << "GENERIC_VEC_SPMV,";
    std::cout << 0 << "," << 0 << "," << 0 << ",";
    spmvp_profiler->print_counters();
    std::cout << "\n";

    delete ddt_profiler;
    delete spmv_profiler;
    delete mkl_spmv_profiler;
    delete spmvp_profiler;

    exit(0);
#endif
#ifndef PROFILE

#ifdef __AVX512__
    auto cvrspmv = new SpMVDDT(B, A, sol_spmv, config, "SpMV CVR MT");
    auto cvr_execmt = cvrspmv->evaluate();
#endif

#ifdef ELLPACK
    auto ellpackspmv = new SpMVELL(B, A, sol_spmv, "SpMV ELLPACK Serial");
    ellpackspmv->set_num_threads(1);
    auto ellpackspmv_exec = ellpackspmv->evaluate();
    auto ellpackspmv_analysis = ellpackspmv->get_analysis_bw();

    auto ellpackspmv_parallel = new SpMVELL(B, A, sol_spmv, "SpMV ELLPACK Parallel");
    ellpackspmv_parallel->set_num_threads(config.nThread);
    auto ellpackspmv_exec_parallel = ellpackspmv_parallel->evaluate();
    auto ellpackspmv_analysis_parallel = ellpackspmv_parallel->get_analysis_bw();
#endif

#ifdef DIA
    auto diaspmv = new SpMVDIA(B, A, sol_spmv, "SpMV DIA");
    diaspmv->set_num_threads(config.nThread);
    auto diaspmv_exec = diaspmv->evaluate();
    auto diaspmv_analysis = diaspmv->get_analysis_bw();
#endif

    sym_lib::timing_measurement csr5spmv_execmt;
    sym_lib::timing_measurement csr5spmv_analysis;
    if (B->m == B->n) {
            auto csr5spmv = new SpMVCSR5(B, A, sol_spmv, "SpMV CSR5 MT");
            csr5spmv->set_num_threads(config.nThread);
            csr5spmv_execmt = csr5spmv->evaluate();
            csr5spmv_analysis = csr5spmv->get_analysis_bw();
            delete csr5spmv;
    }

    auto nThread = config.nThread;
    config.nThread = 1;
    auto *ddtspmvst = new SpMVDDT(B, A, sol_spmv, config, "SpMV DDT ST");
    auto ddt_execst = ddtspmvst->evaluate();
    delete ddtspmvst;

    config.nThread = nThread;
    auto *ddtspmvmt = new SpMVDDT(B, A, sol_spmv, config, "SpMV DDT MT");
    ddtspmvmt->set_num_threads(config.nThread);
    auto ddt_execmt = ddtspmvmt->evaluate();
    auto ddt_analysis = ddtspmvmt->get_analysis_bw();
    delete ddtspmvmt;


    if (config.header) {
        std::cout << "Matrix,nRows,nCols,NNZ,Threads, prefer_fsc, size_cutoff, col_threshold,";
        std::cout << "SpMV Base, SpMV Parallel Base, SpMV Vec 1_4 Parallel, SpMV Vec 1_4, SpMV Vec 1_8, SpMV Vec 1_16, SpMV Vec 2,";

#ifdef MKL
        std::cout << "SpMV MKL Serial Executor, SpMV MKL Parallel Executor,";
#endif
#ifdef __AVX512__
        std::cout << "SpMV CVR Parallel Executor,";
#endif
        if (B->m == B->n) {
            std::cout << "SpMVCSR5 Parallel Executor,";
        }
#ifdef DIA
        "DIA Parallel Executor,"
#endif
#ifdef ELLPACK
    std::cout << "ELLPACK Analysis Time,ELLPack Serial,ELLPack Parallel,"; 
#endif
        std::cout << "SpMVDDT Serial Executor,SpMV DDT Parallel Executor, MKL Analysis";
        if (B->m == B->n) { std::cout << ",CSR5 Analysis"; }
        std::cout << ", SPMV Analysis";
        std::cout << "\n";
    }

    std::cout << config.matrixPath << "," << B->m << "," << B->n << "," << B->nnz << "," << config.nThread
    << "," << DDT::prefer_fsc << "," << DDT::clt_width << "," << DDT::col_th << ","
    << spmv_baseline.elapsed_time << "," << spmv_p.elapsed_time << ",";
     std::cout << spmv1pe.elapsed_time << ",";
     std::cout << spmv1e.elapsed_time << "," << spmv2e.elapsed_time << ",";
     std::cout << spmv1_8e.elapsed_time << "," << spmv1_16e.elapsed_time << ",";
#ifdef MKL
     std::cout << mkl_exec_st.elapsed_time << "," << mkl_exec_mt.elapsed_time << ",";
#endif


#ifdef __AVX512__
     std::cout << cvr_execmt.elapsed_time << ",";
#endif
     if (B->m == B->n) {
         std::cout << csr5spmv_execmt.elapsed_time << ",";
     }
#ifdef DIA
    std::cout << diaspmv_exec.elapsed_time << ",";
#endif
    std::cout
              << ddt_execst.elapsed_time << ",";
                    std::cout << ddt_execmt.elapsed_time << ",";
#ifdef DIA
                std::cout << diaspmv_analysis.elapsed_time << ",";
#endif
                    std::cout << mkl_analysis_bw.elapsed_time << ",";
                    if (B->m == B->n) {
                        std::cout << csr5spmv_analysis.elapsed_time << ",";
                    }
                    std::cout << ddt_analysis.elapsed_time << ",";
#ifdef ELLPACK
    std::cout << ellpackspmv_exec.elapsed_time << "," << ellpackspmv_analysis.elapsed_time << "," 
              << ellpackspmv_exec_parallel.elapsed_time; 
#endif
    std::cout << "\n";


    delete A;
    delete B;
    delete sps;
#endif


    return 0;
}
