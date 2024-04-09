//
// Created by cetinicz on 2021-08-14.
//

#include "DDTUtils.h"
#include "GenericCodelets.h"
#include "GenericKernels.h"
#include "ParseMatrixMarket.h"
#include "TestPSC.h"

#include <tuple>

#define NUM_KERNEL_TEST 50

inline double hsum_double_avx(__m256d v) {
    __m128d vlow = _mm256_castpd256_pd128(v);
    __m128d vhigh = _mm256_extractf128_pd(v, 1);  // high 128
    vlow = _mm_add_pd(vlow, vhigh);      // reduce down to 128

    __m128d high64 = _mm_unpackhi_pd(vlow, vlow);
    return _mm_cvtsd_f64(_mm_add_sd(vlow, high64));  // reduce to scalar
}

void psc_t1_csr_baseline(double* y, const double* x, const double* Ax, const int* offsets, int blockDim, int blocksPerRow, int numBlockRows) {
    for (int i = 0; i < numBlockRows; ++i) {
        for (int j = 0; j < blocksPerRow; ++j) {
            DDT::psc_t1_2D4R(y,Ax, x, offsets+i*blocksPerRow*blockDim+j*blockDim, i*blockDim, i*blockDim+blockDim, j*blockDim, j*blockDim+blockDim);
        }
    }
}

void psc_t1_vb_csr_baseline(double* y, const double* x, const double* Ax, const int* offsets, int rowBlockDim, int colBlockDim, int blocksPerRow, int numBlockRows) {
    for (int i = 0; i < numBlockRows; ++i) {
        for (int j = 0; j < blocksPerRow; ++j) {
            DDT::psc_t1_2D4R(y,Ax, x, offsets+i*blocksPerRow*rowBlockDim+j*rowBlockDim, i*rowBlockDim, i*rowBlockDim+rowBlockDim, j*colBlockDim, j*colBlockDim+colBlockDim);
        }
    }
}

void psc_id_6_csr_baseline(const double** Lxp, const int* yp, double* y, const double* x, int cols, int rows) {
        for (int i = 0; i < cols; ++i) {
            auto Lx = Lxp[i];
            auto xv = _mm256_set1_pd(x[i]);
            int j = 0;
            for (; j < rows - 3; j += 4) {
                auto lxv = _mm256_loadu_pd(Lx + j);
                auto r0 = _mm256_mul_pd(lxv, xv);
                y[yp[j + 0]] += r0[0];
                y[yp[j + 1]] += r0[1];
                y[yp[j + 2]] += r0[2];
                y[yp[j + 3]] += r0[3];
            }
            for (; j < rows; ++j) {
                y[yp[j]] += Lx[j] * x[i];
            }
        }
}

void psc_t3_csr_baseline(const int*Ap, const int*Ai, double* y, const double* x, const double* Ax, int n) {
    for (int i = 0; i < n; ++i) {
        int j = Ap[i];
        auto r0 = _mm256_setzero_pd();
        for (; j < Ap[i+1]-3; j+=4) {
            auto axv0 = _mm256_loadu_pd(Ax+j);
            auto xv0 = _mm256_set_pd( x[Ai[j+3]],x[Ai[j+2]],x[Ai[j+1]],x[Ai[j+0]]);
            r0 = _mm256_fmadd_pd(axv0, xv0, r0);
        }
        double tail = 0.;
        for (; j < Ap[i+1]; ++j) {
            tail += Ax[j] * x[Ai[j]];
        }
        y[i] = hsum_double_avx(r0) + tail;
    }
}

void psc_t1_csr_vb_baseline(
        int*Ap,
        double*y,
        double*x,
        double* Ax,
        int majorBlockDim,
        int minorBlockDim0,
        int minorBlockDim1,
        int numBlockRows) {
    auto nzbr = majorBlockDim *majorBlockDim  +
                minorBlockDim0*minorBlockDim0 +
                minorBlockDim1*minorBlockDim1;
    for (int i = 0; i < numBlockRows; ++i) {
        // Major block
        DDT::psc_t2_2D4C(y,Ax,x,Ap+i*majorBlockDim,i*nzbr,0,i*majorBlockDim,i*majorBlockDim+majorBlockDim,0);
        // Minor block 0
        DDT::psc_t2_2D4C(y,Ax,x,Ap+i*majorBlockDim,i*nzbr+majorBlockDim,0,i*majorBlockDim,i*majorBlockDim+majorBlockDim,0);
        // Minor block 1
        DDT::psc_t2_2D4C(y,Ax,x,Ap+i*majorBlockDim,i*nzbr,0,i*majorBlockDim,i*majorBlockDim+majorBlockDim,0);
    }
}

DDT::Matrix generateVariableBlockedPSCT1Matrix(int majorBlockDim, int minorBlockDim0, int minorBlockDim1, int numBlockRows) {
    assert(majorBlockDim > (minorBlockDim0 + minorBlockDim1));

    int nnz = numBlockRows*(majorBlockDim*majorBlockDim + minorBlockDim0*minorBlockDim0 + minorBlockDim1*minorBlockDim1);

    auto Ap = new int[numBlockRows*majorBlockDim]();
    auto Ai = new int[nnz]();
    auto Ax = new double[nnz]();

    for (int i = 0; i < nnz; ++i) {
        Ax[i] = 3.3;
    }
    int cnt = 0, i = 0;
    for (int i = 0; i < numBlockRows; i++) {
        for (int j = 0; j < minorBlockDim0; ++j) {
            Ap[i+j] = majorBlockDim + minorBlockDim0;
        }
        for (int j = 0; j < minorBlockDim1; ++j) {
            Ap[i+j] = majorBlockDim + minorBlockDim1;
        }
    }

    DDT::Matrix m{};

    m.Lx = Ax;
    m.Li = Ai;
    m.Lp = Ap;

    m.r = majorBlockDim*numBlockRows;
    m.c = majorBlockDim + std::max(minorBlockDim0, minorBlockDim1);
    m.nz = nnz;

    return m;
}

DDT::Matrix generateBlockedPSCT1SparseMatrix(int numBlocksPerRow, int blockDim, int numBlockRows) {
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

double TEST_MULTI_PSC_T1_Baseline_SPMV(int majorBlockDim, int minorBlockDim0, int minorBlockDim1, int numBlockRows) {
    auto m = generateVariableBlockedPSCT1Matrix(majorBlockDim, minorBlockDim0, minorBlockDim1, numBlockRows);

    auto y = new double[m.r]();
    auto x = new double[m.c]();

    auto t0 = std::chrono::steady_clock::now();
    for (int i = 0; i < NUM_KERNEL_TEST; ++i) {
        spmv_csr_baseline(m.r, m.Li, m.Lp, m.Lx, x, y);
    }
    auto t1 = std::chrono::steady_clock::now();

    auto t2 = std::chrono::steady_clock::now();
    for (int i = 0; i < NUM_KERNEL_TEST; ++i) {
        psc_t1_csr_vb_baseline(m.Li,y, x, m.Lx, majorBlockDim, minorBlockDim0, minorBlockDim1, numBlockRows);
    }
    auto t3 = std::chrono::steady_clock::now();

    delete[] y;
    delete[] x;

    return std::chrono::duration_cast<std::chrono::duration<double>>(t1-t0).count() / std::chrono::duration_cast<std::chrono::duration<double>>(t3-t2).count();
}

double TEST_PSC_T1_Baseline_SPMV(int blockDim, int blocksPerRow, int numBlockRows) {
    auto m = generateBlockedPSCT1SparseMatrix(blockDim, blocksPerRow, numBlockRows);

    auto y = new double[m.r]();
    auto x = new double[m.c]();

    // Generate T1 offsets
    auto offsets = new int[numBlockRows*blocksPerRow*blockDim]();
    for (int i = 0; i < numBlockRows; ++i) {
        for (int j = 0; j < blocksPerRow; ++j) {
            for (int k = 0; k < blockDim; ++k) {
                offsets[i*blocksPerRow+j*blockDim+k] = i*blockDim*blocksPerRow + k*blockDim*blocksPerRow+j*blockDim;
            }
        }
    }

    auto t0 = std::chrono::steady_clock::now();
    for (int i = 0; i < NUM_KERNEL_TEST; ++i) {
        spmv_csr_baseline(m.r, m.Li, m.Lp, m.Lx, x, y);
    }
    auto t1 = std::chrono::steady_clock::now();

    auto t2 = std::chrono::steady_clock::now();
    for (int i = 0; i < NUM_KERNEL_TEST; ++i) {
        psc_t1_csr_baseline(y, x, m.Lx, offsets, blockDim, blocksPerRow, numBlockRows);
    }
    auto t3 = std::chrono::steady_clock::now();

    delete[] y;
    delete[] x;
    delete[] offsets;

    return std::chrono::duration_cast<std::chrono::duration<double>>(t1-t0).count() / std::chrono::duration_cast<std::chrono::duration<double>>(t3-t2).count();
}

void psc_t2_csr_baseline(const int*Ai, double*y, const double*x, const double* Ax, int rowBlockSize, int numColNNZ, int numBlockedPsc) {
    int nnzPerBlock = rowBlockSize * numColNNZ;
    for (int i = 0; i < numBlockedPsc; ++i) {
        DDT::psc_t2_2DC(y, Ax, x, Ai+nnzPerBlock*i,  numColNNZ,nnzPerBlock*i, i*rowBlockSize, i*rowBlockSize+rowBlockSize, numColNNZ,0);
    }
}

void fsc_psc_baseline(double*y,const double*Ax,const double*x,int colSegmentSize, int rowBlockSize,int numBlockedPsc, int numColSegments) {
    int blockSize = rowBlockSize*numColSegments*colSegmentSize;
    for (int i = 0; i < numBlockedPsc; ++i) {
        for (int j = 0; j < numColSegments; ++j) {
            int k = 0;
            for (; k < rowBlockSize-1; k+=2) {
                DDT::fsc_t2_2DC(
                        y,
                        Ax,
                        x,
                        colSegmentSize*numColSegments,
                        i*blockSize+j*colSegmentSize+k*colSegmentSize*numColSegments,
                        i*rowBlockSize+k,
                        i*rowBlockSize+k+2,
                        j*colSegmentSize,
                        j*colSegmentSize+colSegmentSize,
                        0
                        );
            }
            for (; k < rowBlockSize; ++k) {
                DDT::fsc_t2_2DC(
                y,
                Ax,
                x,
                colSegmentSize*numColSegments,
                i*blockSize+j*colSegmentSize+k*colSegmentSize*numColSegments,
                i*rowBlockSize+k,
                i*rowBlockSize+k+1,
                j*colSegmentSize,
                j*colSegmentSize+colSegmentSize,
                0
                );
            }
//            DDT::fsc_t2_2DC(y, Ax, x,  colSegmentSize*numColSegments,i*blockSize+j*colSegmentSize, i*rowBlockSize, i*rowBlockSize+rowBlockSize, j*colSegmentSize, j*colSegmentSize+colSegmentSize, 0);
        }
    }
}

DDT::Matrix generateBlockedPSC2SparseMatrix(int rowBlockSize, int colGap, int numColSegments, int numColNNZ, int numBlockedPsc) {
    assert((numColNNZ % numColSegments) == 0);

    int nnz = numColNNZ * rowBlockSize * numBlockedPsc;
    auto Ax = new double[nnz]();
    auto Ai = new int[nnz]();
    auto Ap = new int[rowBlockSize*numBlockedPsc]();
    int colBandSize = numColNNZ / numColSegments;

    for (int i = 0; i < nnz; ++i) {
        Ax[i] = 3.3;
    }
    int cnt = 0;
    for (int ii = 0; ii < rowBlockSize*numBlockedPsc; ++ii) {
        for (int i = 0; i < numColSegments; ++i) {
            for (int j = 0; j < colBandSize; ++j) {
                Ai[cnt++] = i*colBandSize + j + i*colGap;
            }
        }
    }
    for (int ii = 0; ii < rowBlockSize*numBlockedPsc+1; ++ii) {
        Ap[ii] = ii * numColNNZ;
    }

    DDT::Matrix m{};
    m.r = rowBlockSize*numBlockedPsc;
    m.c = numColNNZ + numColSegments*colGap;
    m.nz = nnz;

    m.Lx = Ax;
    m.Li = Ai;
    m.Lp = Ap;

    return m;
}

DDT::Matrix generateBlockedPSC2SparseMatrixCSC(int rowBlockSize, int colGap, int numColSegments, int numColNNZ, int numBlockedPsc) {
    assert((numColNNZ % numColSegments) == 0);

    int nnz = numColNNZ * rowBlockSize * numBlockedPsc;
    int colBandSize = numColNNZ / numColSegments;
    int cols = (numColSegments-1)*colGap+numColNNZ;

    auto Ax = new double[nnz]();
    auto Ai = new int[nnz]();
    auto Ap = new int[cols+1]();

    for (int i = 0; i < nnz; ++i) {
        Ax[i] = 3.3;
    }
    int cnt = 0;
    for (int i = 0; i < numColSegments; ++i) {
        for (int j = 0; j < colBandSize; ++j) {
            for (int ii = 0; ii < rowBlockSize*numBlockedPsc; ++ii) {
                Ai[cnt++] = ii;
            }
        }
    }
    assert(cnt == nnz);

    int assertCount = 0, colCnt = 0, realCols = 0;
    for (int ii = 0; ii < numColSegments; ++ii) {
        for (int i = 0; i < colBandSize; ++i) {
            Ap[colCnt++] = realCols++ * rowBlockSize * numBlockedPsc;
        }
        if (ii < numColSegments-1) {
            for (int i = 0; i < colGap; ++i) {
                Ap[colCnt] = Ap[colCnt - 1];
                colCnt++;
                assertCount++;
            }
        }
    }
    Ap[colCnt] = Ap[colCnt-1] + rowBlockSize * numBlockedPsc;
    assert(assertCount == colGap*(numColSegments-1));
    assert(colCnt == cols);

    DDT::Matrix m{};
    m.r = rowBlockSize*numBlockedPsc;
    m.c = cols;
    m.nz = nnz;

    m.Lx = Ax;
    m.Li = Ai;
    m.Lp = Ap;

    return m;
}


double TEST_PSC_T2_Baseline_SPMV(int rowBlockSize, int colGap, int numColSegments, int numColNNZ, int numBlockedPsc) {
    auto m = generateBlockedPSC2SparseMatrix(rowBlockSize, colGap, numColSegments, numColNNZ, numBlockedPsc);

    auto y = new double[m.r]();
    auto x = new double[m.c]();

    auto t0 = std::chrono::steady_clock::now();
    for (int i = 0; i < NUM_KERNEL_TEST; ++i) {
        spmv_csr_baseline(m.r, m.Li, m.Lp, m.Lx, x, y);
    }
    auto t1 = std::chrono::steady_clock::now();

    auto t2 = std::chrono::steady_clock::now();
    for (int i = 0; i < NUM_KERNEL_TEST; ++i) {
        psc_t2_csr_baseline(m.Li, y, x, m.Lx, rowBlockSize, numColNNZ, numBlockedPsc);
    }
    auto t3 = std::chrono::steady_clock::now();

    delete[] y;
    delete[] x;

    return std::chrono::duration_cast<std::chrono::duration<double>>(t1-t0).count() / std::chrono::duration_cast<std::chrono::duration<double>>(t3-t2).count();
}

DDT::Matrix generatePSC3SparseMatrix(int maxRow, int maxCol, int nnzPerRow) {
    int nnz = nnzPerRow * maxRow;

    auto Ap = new int[maxRow]();
    auto Ai = new int[nnz]();
    auto Ax = new double[nnz]();

    for (int i = 0; i < nnz; ++i) {
        Ax[i] = 3.3;
    }
    for (int i = 0, AiCnt = 0; i < maxRow; ++i) {
        int cnt = nnzPerRow;
        int seed = (rand()*maxCol) % maxCol;
        while (cnt--) {
            Ai[AiCnt++] = (seed + i * 3) % maxCol;
        }
    }
    for (int i = 0; i < maxRow+1; ++i) {
        Ap[i] = nnzPerRow*i;
    }

    DDT::Matrix m{};
    m.r = maxRow;
    m.c = maxCol;
    m.nz = nnz;

    m.Lx = Ax;
    m.Li = Ai;
    m.Lp = Ap;

    return m;
}

double TEST_PSC_T3_Baseline_SPMV(int maxRow, int maxCol, int nnzPerRow) {
    auto m = generatePSC3SparseMatrix(maxRow,  maxCol,  nnzPerRow);

    auto y = new double[m.r]();
    auto x = new double[m.c]();

    auto t0 = std::chrono::steady_clock::now();
    for (int i = 0; i < NUM_KERNEL_TEST; ++i) {
        spmv_csr_baseline(m.r, m.Li, m.Lp, m.Lx, x, y);
    }
    auto t1 = std::chrono::steady_clock::now();

    auto t2 = std::chrono::steady_clock::now();
    for (int i = 0; i < NUM_KERNEL_TEST; ++i) {
        psc_t3_csr_baseline(m.Lp, m.Li, y, x, m.Lx, m.r);
    }
    auto t3 = std::chrono::steady_clock::now();

    delete[] y;
    delete[] x;

    return std::chrono::duration_cast<std::chrono::duration<double>>(t1-t0).count() / std::chrono::duration_cast<std::chrono::duration<double>>(t3-t2).count();
}

std::tuple<double,double,double,double,double> TEST_FSC_PSC_SPMV(int rowBlockSize, int colGap, int numColSegments, int numColNNZ, int numBlockedPsc) {
    auto m = generateBlockedPSC2SparseMatrix(rowBlockSize, colGap, numColSegments, numColNNZ, numBlockedPsc);
    auto cscM = generateBlockedPSC2SparseMatrixCSC(rowBlockSize, colGap, numColSegments, numColNNZ, numBlockedPsc);

//    for (int i = 0; i < cscM.c; ++i) {
//        for (int j = cscM.Lp[i]; j < cscM.Lp[i+1]; ++j) {
//            std::cout << "(" << cscM.Li[j] << "," << i << ")\n";
//        }
//    }
    auto y = new double[m.r]();
    auto yTrue = new double[m.r]();
    auto x = new double[m.c]();

    auto resetVector = [&]() {
        for (int i = 0; i < m.r; ++i) {
            y[i] = 0;
        }
        for (int i = 0; i < m.c; ++i) {
            x[i] = 1;
        }
    };



    // Generate offsets
    auto offsets = new int[numBlockedPsc*numColSegments*rowBlockSize]();
    for (int i = 0; i < numBlockedPsc; ++i) {
        for (int j = 0; j < numColSegments; ++j) {
            for (int k = 0; k < rowBlockSize; ++k) {
                offsets[k+j*rowBlockSize+i*rowBlockSize*numColSegments] = k*(numColNNZ)+j*(numColNNZ / numColSegments)+i*rowBlockSize*numColNNZ;
            }
        }
    }

    double t0_time = 0.;
    for (int i = 0; i < NUM_KERNEL_TEST; ++i) {
        resetVector();
        auto t0 = std::chrono::steady_clock::now();
        spmv_csr_baseline(m.r, m.Li, m.Lp, m.Lx, x, y);
        auto t1 = std::chrono::steady_clock::now();
        t0_time += DDT::getTimeDifference(t0,t1);
        if (i == 0) {
            std::memcpy(yTrue,y,sizeof(double)*m.r);
        }
    }
    t0_time /= NUM_KERNEL_TEST;

    double t0_time_csc = 0.;
    for (int i = 0; i < NUM_KERNEL_TEST; ++i) {
        resetVector();
        auto t0 = std::chrono::steady_clock::now();
        spmv_csc_baseline(cscM.c, cscM.Li, cscM.Lp, cscM.Lx, x, y);
        auto t1 = std::chrono::steady_clock::now();
        t0_time_csc += DDT::getTimeDifference(t0,t1);
        if (!verifyVector(m.r,y,yTrue)) {
            throw std::runtime_error("Error: CSC Baseline incorrect...");
        }
    }
    t0_time_csc /= NUM_KERNEL_TEST;


    double t1_time = 0.;
    for (int i = 0; i < NUM_KERNEL_TEST; ++i) {
        resetVector();
        auto t0 = std::chrono::steady_clock::now();
        fsc_psc_baseline(y,m.Lx,x, numColNNZ / numColSegments, rowBlockSize,numBlockedPsc, numColSegments);
        auto t1 = std::chrono::steady_clock::now();
        t1_time += DDT::getTimeDifference(t0,t1);
        if (!verifyVector(m.r,y,yTrue)) {
            throw std::runtime_error("Error: FSC Baseline incorrect...");
        }
    }
    t1_time /= NUM_KERNEL_TEST;

    double t2_time = 0.;
    for (int i = 0; i < NUM_KERNEL_TEST; ++i) {
        resetVector();
        auto t0 = std::chrono::steady_clock::now();
        psc_t1_vb_csr_baseline(y, x, m.Lx, offsets, rowBlockSize, numColNNZ/numColSegments, numColSegments, numBlockedPsc);
        auto t1 = std::chrono::steady_clock::now();
        t2_time += DDT::getTimeDifference(t0,t1);
        if (!verifyVector(m.r,y,yTrue)) {
            throw std::runtime_error("Error: PSC_1 Baseline incorrect...");
        }
    }
    t2_time /= NUM_KERNEL_TEST;

    double t3_time = 0.;
    for (int i = 0; i < NUM_KERNEL_TEST; ++i) {
        resetVector();
        auto t0 = std::chrono::steady_clock::now();
        psc_t2_csr_baseline(m.Li, y, x, m.Lx, rowBlockSize, numColNNZ, numBlockedPsc);
        auto t1 = std::chrono::steady_clock::now();
        t3_time += DDT::getTimeDifference(t0,t1);
        if (!verifyVector(m.r,y,yTrue)) {
            throw std::runtime_error("Error: PSC_2 Baseline incorrect...");
        }
    }
    t3_time /= NUM_KERNEL_TEST;

    double t4_time = 0.;
    for (int i = 0; i < NUM_KERNEL_TEST; ++i) {
        resetVector();
        auto t0 = std::chrono::steady_clock::now();
        psc_t3_csr_baseline(m.Lp, m.Li, y, x, m.Lx, m.r);
        auto t1 = std::chrono::steady_clock::now();
        t4_time += DDT::getTimeDifference(t0,t1);
        if (!verifyVector(m.r,y,yTrue)) {
            throw std::runtime_error("Error: PSC_3 Baseline incorrect...");
        }
    }
    t4_time /= NUM_KERNEL_TEST;

    // Create Lxp and yp
    const auto** Lxp = new const double*[cscM.c];
    for (int i = 0; i < cscM.c; ++i) {
        Lxp[i] = m.Lx + m.Lp[i];
    }
    auto ypp = new int[cscM.r];
    for (int i = 0; i < cscM.r; ++i) {
        ypp[i] = i;
    }

    double t5_time = 0.;
    for (int i = 0; i < NUM_KERNEL_TEST; ++i) {
        resetVector();
        auto t0 = std::chrono::steady_clock::now();
        psc_id_6_csr_baseline(Lxp, ypp, y, x, cscM.c-colGap*(numColSegments-1), cscM.r);
        auto t1 = std::chrono::steady_clock::now();
        t5_time += DDT::getTimeDifference(t0,t1);
        if (!verifyVector(m.r,y,yTrue)) {
            throw std::runtime_error("Error: PSC_ID_6 Baseline incorrect...");
        }
    }
    t5_time /= NUM_KERNEL_TEST;

    delete[] ypp;
    delete[] Lxp;

    delete[] y;
    delete[] x;
    delete[] offsets;

    return std::make_tuple(
        t0_time / t1_time,
        t0_time / t2_time,
        t0_time / t3_time,
        t0_time / t4_time,
        t0_time_csc / t5_time
        );
}