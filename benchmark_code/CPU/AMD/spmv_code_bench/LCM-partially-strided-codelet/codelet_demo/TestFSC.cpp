//
// Created by cetinicz on 2021-08-14.
//

#include "ParseMatrixMarket.h"
#include "GenericCodelets.h"
#include "GenericKernels.h"
#include "TestFSC.h"

#define NUM_KERNEL_TEST 5

void fsc_codelet_baseline(double*y, const double*x, const double*Ax, int numBlocksPerRow, int numBlockRows, int blockDim) {
    for (int i = 0; i < numBlockRows; ++i) {
        for (int j = 0; j < numBlocksPerRow; ++j) {
            DDT::fsc_t2_2DC(y, Ax, x,  blockDim*numBlocksPerRow, j*blockDim + i*numBlocksPerRow*blockDim*blockDim,i*blockDim, i*blockDim+blockDim, j*blockDim, j*blockDim+blockDim, 0);
        }
    }
}

DDT::Matrix generateBlockedFSCSparseMatrix(int numBlocksPerRow, int blockDim, int numBlockRows) {
    auto nnz = blockDim*blockDim * numBlocksPerRow * numBlockRows;
    auto Ax = new double[nnz]();
    auto Ai = new int[nnz]();
    auto Ap = new int[numBlockRows*blockDim]();

    for (int i = 0; i < nnz; ++i) {
        Ax[i] = 3.39;
        Ai[i] = i % (numBlocksPerRow*blockDim);
    }
    for (int i = 0; i < numBlockRows*blockDim+1; ++i) {
        Ap[i] = numBlocksPerRow*blockDim*i;
    }
    DDT::Matrix m{};
    m.Lp = Ap;
    m.Li = Ai;
    m.Lx = Ax;

    m.r = numBlockRows*blockDim;
    m.c = numBlocksPerRow*blockDim;
    m.nz = nnz;

    return m;
}

/**
 * @brief Tests FSC codelets for given block configuration
 *
 * @param numBlocksPerRow Number of blocks per row in the matrix
 * @param blockDim Size of block width and height
 * @param numBlockRows Number of blocks
 *
 * @return Speedup of FSC codelet execution over baseline CSR SpMV
 */
double TEST_FSC_Baseline_SPMV(int numBlocksPerRow, int blockDim, int numBlockRows) {
    auto m = generateBlockedFSCSparseMatrix(numBlocksPerRow,blockDim,numBlockRows);

    auto y = new double[m.r]();
    auto x = new double[m.c]();
    std::fill(x,x+m.c,1);

    auto t0 = std::chrono::steady_clock::now();
    for (int i = 0; i < NUM_KERNEL_TEST; ++i) {
        spmv_csr_baseline(m.r, m.Li, m.Lp, m.Lx, x, y);
    }
    auto t1 = std::chrono::steady_clock::now();

    auto t2 = std::chrono::steady_clock::now();
    for (int i = 0; i < NUM_KERNEL_TEST; ++i) {
        fsc_codelet_baseline(y, x, m.Lx, numBlocksPerRow, numBlockRows, blockDim);
    }
    auto t3 = std::chrono::steady_clock::now();

    delete[] y;
    delete[] x;

    return std::chrono::duration_cast<std::chrono::duration<double>>(t1-t0).count() / std::chrono::duration_cast<std::chrono::duration<double>>(t3-t2).count();
}
