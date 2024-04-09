//
// Created by cetinicz on 2021-08-14.
//

#ifndef DDT_TESTPSC_H
#define DDT_TESTPSC_H

double TEST_PSC_T1_Baseline_SPMV(int blockDim, int blocksPerRow, int numBlockRows);

double TEST_PSC_T2_Baseline_SPMV(int rowBlockSize, int colGap, int numColSegments, int numColNNZ, int numBlockedPsc);

double TEST_PSC_T3_Baseline_SPMV(int maxRow, int maxCol, int nnzPerRow);

std::tuple<double,double,double,double,double> TEST_FSC_PSC_SPMV(int rowBlockSize, int colGap, int numColSegments, int numColNNZ, int numBlockedPsc);

#endif//DDT_TESTPSC_H
