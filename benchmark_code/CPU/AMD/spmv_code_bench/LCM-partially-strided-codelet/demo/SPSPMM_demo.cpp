//
// Created by cetinicz on 2021-10-30.
//

#include "SPSPMM_demo_utils.h"
#include <Input.h>
#include <def.h>
#include <sparse_io.h>
#include <sparse_utilities.h>


int main(int argc, char *argv[]) {
    auto config = DDT::parseInput(argc, argv);
    auto A = sym_lib::read_mtx(config.matrixPath);
    sym_lib::CSC *A_full = NULLPNTR;
    sym_lib::CSR *B = NULLPNTR, *L_csr = NULLPNTR;
    if (A->stype < 0) {
        A_full = sym_lib::make_full(A);
        B = sym_lib::csc_to_csr(A_full);
    } else {
        A_full = A;
        B = sym_lib::csc_to_csr(A);
    }

    auto *sps = new sparse_avx::SpSpMMSerial(B, A_full, "SpSpMM Serial");
    sps->set_num_threads(config.nThread);
    auto spmm_baseline = sps->evaluate();
    double *sol_spmm = sps->solution();

    auto spmm_baseline_elapsed = spmm_baseline.elapsed_time;

    auto *spmkl = new sparse_avx::SpSpMMMKL(B, A_full, sol_spmm, "SpSpMM MKL");
    spmkl->set_num_threads(config.nThread);
    auto spmm_mkl_eval = spmkl->evaluate();

    std::cout << "SpMM Baseline, SpMM Parallel Baseline, SpMM MKL" << "\n";
    std::cout << spmm_baseline_elapsed << "," << spmm_mkl_eval.elapsed_time << "\n";
}