//
// Created by cetinicz on 2021-08-14.
//

#include <string>

#ifndef DDT_RUNTESTS_H
#define DDT_RUNTESTS_H

const std::string TEST_FSC_BASELINE_SPMV_HEADER = "blockDim,blocksPerRow,numBlockRows,Speedup";
/**
 * struct { blockDim, blocksPerRow, numBlockRows }
 * @param blockDim
 * @param blocksPerRow
 * @param numBlockRows
 */
static int TEST_FSC_BASELINE_SPMV_CONFIG[][3] = {
        {1,1,1000},{1,2,1000},{1,4,1000},
        {1,8,1000},{1,16,1000},{1,32,1000},
        {2,1,1000},{2,2,1000},{2,4,1000},
        {2,8,1000},{2,16,1000},{2,32,1000},
        {4,1,1000},{4,2,1000},{4,4,1000},
        {4,8,1000},{4,16,1000},{4,32,1000},
        {8,1,1000},{8,2,1000},{8,4,1000},
        {8,8,1000},{8,16,1000},{8,32,1000},
        {16,1,1000},{16,2,1000},{16,4,1000},
        {16,8,1000},{16,16,1000},{16,32,1000},
};

const std::string TEST_PSC_T1_BASELINE_SPMV_HEADER = "blockDim,blocksPerRow,numBlockRows,Speedup";
/**
 * struct { blockDim, blocksPerRow, numBlockRows }
 * @param blockDim
 * @param blocksPerRow
 * @param numBlockRows
 */
static int TEST_PSC_T1_BASELINE_SPMV_CONFIG[][3] = {
        {1,1,1000},{1,2,1000},{1,4,1000},
        {1,8,1000},{1,16,1000},{1,32,1000},
        {2,1,1000},{2,2,1000},{2,4,1000},
        {2,8,1000},{2,16,1000},{2,32,1000},
        {4,1,1000},{4,2,1000},{4,4,1000},
        {4,8,1000},{4,16,1000},{4,32,1000},
        {8,1,1000},{8,2,1000},{8,4,1000},
        {8,8,1000},{8,16,1000},{8,32,1000},
        {16,1,1000},{16,2,1000},{16,4,1000},
        {16,8,1000},{16,16,1000},{16,32,1000},
};

const std::string TEST_PSC_T2_BASELINE_SPMV_HEADER = "rowBlockSize,colGap,numColSegments,numColNNZ,numBlockedPSC,Speedup";
/**
 * struct { rowBlockSize, colGap, numColSegments, numColNNZ, numBlockedPSC }
 * @param rowBlockSize
 * @param colGap
 * @param numColSegments
 * @param numColNNZ
 * @param numBlockedPSC
 */
static int TEST_PSC_T2_BASELINE_SPMV_CONFIG[][5] = {
        {1,0,1,1,1000},{1,0,1,2,1000},{1,0,1,4,1000},
        {1,0,1,8,1000},{1,0,1,16,1000},{1,0,1,32,1000},
                       {1,0,2,2,1000},{1,0,2,4,1000},
        {1,0,2,8,1000},{1,0,2,16,1000},{1,0,2,32,1000},
                                        {1,0,4,4,1000},
        {1,0,4,8,1000},{1,0,4,16,1000},{1,0,4,32,1000},
        {1,0,8,8,1000},{1,0,8,16,1000},{1,0,8,32,1000},
                       {1,0,16,16,1000},{1,0,16,32,1000},
                       {1,0,32,32,1000},

        {2,0,1,1,1000},{2,0,1,2,1000},{2,0,1,4,1000},
        {2,0,1,8,1000},{2,0,1,16,1000},{2,0,1,32,1000},
                       {2,0,2,2,1000},{2,0,2,4,1000},
        {2,0,2,8,1000},{2,0,2,16,1000},{2,0,2,32,1000},
                                       {2,0,4,4,1000},
        {2,0,4,8,1000},{2,0,4,16,1000},{2,0,4,32,1000},
        {2,0,8,8,1000},{2,0,8,16,1000},{2,0,8,32,1000},
                       {2,0,16,16,1000},{2,0,16,32,1000},
                       {2,0,32,32,1000},

        {4,0,1,1,1000},{4,0,1,2,1000},{4,0,1,4,1000},
        {4,0,1,8,1000},{4,0,1,16,1000},{4,0,1,32,1000},
        {4,0,2,2,1000},{4,0,2,4,1000},
        {4,0,2,8,1000},{4,0,2,16,1000},{4,0,2,32,1000},
        {4,0,4,4,1000},
        {4,0,4,8,1000},{4,0,4,16,1000},{4,0,4,32,1000},

        {4,0,8,8,1000},{4,0,8,16,1000},{4,0,8,32,1000},
        {4,0,16,16,1000},{4,0,16,32,1000},
        {4,0,32,32,1000},

        {1,1,1,1,1000},{1,1,1,2,1000},{1,1,1,4,1000},
        {1,1,1,8,1000},{1,1,1,16,1000},{1,1,1,32,1000},
        {1,1,2,2,1000},{1,1,2,4,1000},
        {1,1,2,8,1000},{1,1,2,16,1000},{1,1,2,32,1000},
        {1,1,4,4,1000},
        {1,1,4,8,1000},{1,1,4,16,1000},{1,1,4,32,1000},
        {1,1,8,8,1000},{1,1,8,16,1000},{1,1,8,32,1000},
        {1,1,16,16,1000},{1,1,16,32,1000},
        {1,1,32,32,1000},

        {2,1,1,1,1000},{2,1,1,2,1000},{2,1,1,4,1000},
        {2,1,1,8,1000},{2,1,1,16,1000},{2,1,1,32,1000},
        {2,1,2,2,1000},{2,1,2,4,1000},
        {2,1,2,8,1000},{2,1,2,16,1000},{2,1,2,32,1000},
        {2,1,4,4,1000},
        {2,1,4,8,1000},{2,1,4,16,1000},{2,1,4,32,1000},
        {2,1,8,8,1000},{2,1,8,16,1000},{2,1,8,32,1000},
        {2,1,16,16,1000},{2,1,16,32,1000},
        {2,1,32,32,1000},

        {4,1,1,1,1000},{4,1,1,2,1000},{4,1,1,4,1000},
        {4,1,1,8,1000},{4,1,1,16,1000},{4,1,1,32,1000},
        {4,1,2,2,1000},{4,1,2,4,1000},
        {4,1,2,8,1000},{4,1,2,16,1000},{4,1,2,32,1000},
        {4,1,4,4,1000},
        {4,1,4,8,1000},{4,1,4,16,1000},{4,1,4,32,1000},
        {4,1,8,8,1000},{4,1,8,16,1000},{4,1,8,32,1000},
        {4,1,16,16,1000},{4,1,16,32,1000},
        {4,1,32,32,1000},


        {1,2,1,1,1000},{1,2,1,2,1000},{1,2,1,4,1000},
        {1,2,1,8,1000},{1,2,1,16,1000},{1,2,1,32,1000},
        {1,2,2,2,1000},{1,2,2,4,1000},
        {1,2,2,8,1000},{1,2,2,16,1000},{1,2,2,32,1000},
        {1,2,4,4,1000},
        {1,2,4,8,1000},{1,2,4,16,1000},{1,2,4,32,1000},
        {1,2,8,8,1000},{1,2,8,16,1000},{1,2,8,32,1000},
        {1,2,16,16,1000},{1,2,16,32,1000},
        {1,2,32,32,1000},

        {2,2,1,1,1000},{2,2,1,2,1000},{2,2,1,4,1000},
        {2,2,1,8,1000},{2,2,1,16,1000},{2,2,1,32,1000},
        {2,2,2,2,1000},{2,2,2,4,1000},
        {2,2,2,8,1000},{2,2,2,16,1000},{2,2,2,32,1000},
        {2,2,4,4,1000},
        {2,2,4,8,1000},{2,2,4,16,1000},{2,2,4,32,1000},
        {2,2,8,8,1000},{2,2,8,16,1000},{2,2,8,32,1000},
        {2,2,16,16,1000},{2,2,16,32,1000},
        {2,2,32,32,1000},{4,2,1,1,1000},{4,2,1,2,1000},
        {4,2,1,4,1000},{4,2,1,8,1000},{4,2,1,16,1000},
        {4,2,1,32,1000},{4,2,2,2,1000},{4,2,2,4,1000},
        {4,2,2,8,1000},{4,2,2,16,1000},{4,2,2,32,1000},
        {4,2,4,4,1000},
        {4,2,4,8,1000},{4,2,4,16,1000},{4,2,4,32,1000},
        {4,2,8,8,1000},{4,2,8,16,1000},{4,2,8,32,1000},
        {4,2,16,16,1000},{4,2,16,32,1000},
        {4,2,32,32,1000},
};

const std::string TEST_PSC_T3_BASELINE_SPMV_HEADER = "maxRow,maxCol,nnzPerRow,Speedup";
/**
 * struct { maxRow, maxCol, nnzPerRow }
 * @param maxRow
 * @param maxCol
 * @param nnzPerRow
 */
static int TEST_PSC_T3_BASELINE_SPMV_CONFIG[][3] = {
        {1,1000,1},{1,1000,2},{1,1000,4},
        {1,1000,8},{1,1000,16},{1,1000,32},
        {2,1000,1},{2,1000,2},{2,1000,4},
        {2,1000,8},{2,1000,16},{2,1000,32},
        {4,1000,1},{4,1000,2},{4,1000,4},
        {4,1000,8},{4,1000,16},{4,1000,32},
        {8,1000,1},{8,1000,2},{8,1000,4},
        {8,1000,8},{8,1000,16},{8,1000,32},
        {10,1000,1},{10,1000,2},{10,1000,4},
        {10,1000,8},{10,1000,16},{10,1000,32},
        {100,1000,1},{100,1000,2},{100,1000,4},
        {100,1000,8},{100,1000,16},{100,1000,32},
        {1000,1,1},{1000,1,2},{1000,1,4},
        {1000,1,8},{1000,1,16},{1000,1,32},
        {1000,2,1},{1000,2,2},{1000,2,4},
        {1000,2,8},{1000,2,16},{1000,2,32},
        {1000,4,1},{1000,4,2},{1000,4,4},
        {1000,4,8},{1000,4,16},{1000,4,32},
        {1000,8,1},{1000,8,2},{1000,8,4},
        {1000,8,8},{1000,8,16},{1000,8,32},
        {1000,10,1},{1000,10,2},{1000,10,4},
        {1000,10,8},{1000,10,16},{1000,10,32},
        {1000,100,1},{1000,100,2},{1000,100,4},
        {1000,100,8},{1000,100,16},{1000,100,32},
        {1000,1000,1},{1000,1000,2},{1000,1000,4},
        {1000,1000,8},{1000,1000,16},{1000,1000,32},
};

const std::string TEST_FSC_PSC_SPMV_HEADER = "rowBlockSize,colGap,numColSegments,numColNNZ,numBlockedPSC,Speedup_FSC,Speedup_PSC_1,Speedup_PSC_2, Speedup_PSC_3, Speedup_PSC_ID_6";
/**
 * struct { rowBlockSize, colGap, numColSegments, numColNNZ, numBlockedPSC }
 * @param rowBlockSize
 * @param colGap
 * @param numColSegments
 * @param numColNNZ
 * @param numBlockedPSC
 */
static int TEST_FSC_PSC_SPMV_CONFIG[][5] = {
        {1,2,1,1,1000},
        {2,2,2,4,1000},
        {4,2,4,16,1000},
        {8,2,8,64,1000},
        {16,2,16,256,1000},
        {32,2,32,1024,1000},
        {64,0,64,4096,1000},
//        {128,0,128,16384,1000},
//        {1,0,1,1,1000},{1,0,1,2,1000},{1,0,1,4,1000},
//        {1,0,1,8,1000},{1,0,1,16,1000},{1,0,1,32,1000},
//        {1,0,2,2,1000},{1,0,2,4,1000},
//        {1,0,2,8,1000},{1,0,2,16,1000},{1,0,2,32,1000},
//        {1,0,4,4,1000},
//        {1,0,4,8,1000},{1,0,4,16,1000},{1,0,4,32,1000},
//        {1,0,8,8,1000},{1,0,8,16,1000},{1,0,8,32,1000},
//        {1,0,16,16,1000},{1,0,16,32,1000},
//        {1,0,32,32,1000},
//
//        {2,0,1,1,1000},{2,0,1,2,1000},{2,0,1,4,1000},
//        {2,0,1,8,1000},{2,0,1,16,1000},{2,0,1,32,1000},
//        {2,0,2,2,1000},{2,0,2,4,1000},
//        {2,0,2,8,1000},{2,0,2,16,1000},{2,0,2,32,1000},
//        {2,0,4,4,1000},
//        {2,0,4,8,1000},{2,0,4,16,1000},{2,0,4,32,1000},
//        {2,0,8,8,1000},{2,0,8,16,1000},{2,0,8,32,1000},
//        {2,0,16,16,1000},{2,0,16,32,1000},
//        {2,0,32,32,1000},
//
//        {4,0,1,1,1000},{4,0,1,2,1000},{4,0,1,4,1000},
//        {4,0,1,8,1000},{4,0,1,16,1000},{4,0,1,32,1000},
//        {4,0,2,2,1000},{4,0,2,4,1000},
//        {4,0,2,8,1000},{4,0,2,16,1000},{4,0,2,32,1000},
//        {4,0,4,4,1000},
//        {4,0,4,8,1000},{4,0,4,16,1000},{4,0,4,32,1000},
//        {4,0,8,8,1000},{4,0,8,16,1000},{4,0,8,32,1000},
//        {4,0,16,16,1000},{4,0,16,32,1000},
//        {4,0,32,32,1000},
//
//        {8,0,1,1,1000},{8,0,1,2,1000},{8,0,1,4,1000},
//        {8,0,1,8,1000},{8,0,1,16,1000},{8,0,1,32,1000},
//        {8,0,2,2,1000},{8,0,2,4,1000},
//        {8,0,2,8,1000},{8,0,2,16,1000},{8,0,2,32,1000},
//        {8,0,4,4,1000},
//        {8,0,4,8,1000},{8,0,4,16,1000},{8,0,4,32,1000},
//        {8,0,8,8,1000},{8,0,8,16,1000},{8,0,8,32,1000},
//        {8,0,16,16,1000},{8,0,16,32,1000},
//        {8,0,32,32,1000},
//
//        {16,0,1,1,1000},{16,0,1,2,1000},{16,0,1,4,1000},
//        {16,0,1,8,1000},{16,0,1,16,1000},{16,0,1,32,1000},
//        {16,0,2,2,1000},{16,0,2,4,1000},
//        {16,0,2,8,1000},{16,0,2,16,1000},{16,0,2,32,1000},
//        {16,0,4,4,1000},
//        {16,0,4,8,1000},{16,0,4,16,1000},{16,0,4,32,1000},
//        {16,0,8,8,1000},{16,0,8,16,1000},{16,0,8,32,1000},
//        {16,0,16,16,1000},{16,0,16,32,1000},
//        {16,0,32,32,1000},
//
//        {1,1,1,1,1000},{1,1,1,2,1000},{1,1,1,4,1000},
//        {1,1,1,8,1000},{1,1,1,16,1000},{1,1,1,32,1000},
//        {1,1,2,2,1000},{1,1,2,4,1000},
//        {1,1,2,8,1000},{1,1,2,16,1000},{1,1,2,32,1000},
//        {1,1,4,4,1000},
//        {1,1,4,8,1000},{1,1,4,16,1000},{1,1,4,32,1000},
//        {1,1,8,8,1000},{1,1,8,16,1000},{1,1,8,32,1000},
//        {1,1,16,16,1000},{1,1,16,32,1000},
//        {1,1,32,32,1000},
//
//        {2,1,1,1,1000},{2,1,1,2,1000},{2,1,1,4,1000},
//        {2,1,1,8,1000},{2,1,1,16,1000},{2,1,1,32,1000},
//        {2,1,2,2,1000},{2,1,2,4,1000},
//        {2,1,2,8,1000},{2,1,2,16,1000},{2,1,2,32,1000},
//        {2,1,4,4,1000},
//        {2,1,4,8,1000},{2,1,4,16,1000},{2,1,4,32,1000},
//        {2,1,8,8,1000},{2,1,8,16,1000},{2,1,8,32,1000},
//        {2,1,16,16,1000},{2,1,16,32,1000},
//        {2,1,32,32,1000},
//
//        {4,1,1,1,1000},{4,1,1,2,1000},{4,1,1,4,1000},
//        {4,1,1,8,1000},{4,1,1,16,1000},{4,1,1,32,1000},
//        {4,1,2,2,1000},{4,1,2,4,1000},
//        {4,1,2,8,1000},{4,1,2,16,1000},{4,1,2,32,1000},
//        {4,1,4,4,1000},
//        {4,1,4,8,1000},{4,1,4,16,1000},{4,1,4,32,1000},
//        {4,1,8,8,1000},{4,1,8,16,1000},{4,1,8,32,1000},
//        {4,1,16,16,1000},{4,1,16,32,1000},
//        {4,1,32,32,1000},
//
//        {8,1,1,1,1000},{8,1,1,2,1000},{8,1,1,4,1000},
//        {8,1,1,8,1000},{8,1,1,16,1000},{8,1,1,32,1000},
//        {8,1,2,2,1000},{8,1,2,4,1000},
//        {8,1,2,8,1000},{8,1,2,16,1000},{8,1,2,32,1000},
//        {8,1,4,4,1000},
//        {8,1,4,8,1000},{8,1,4,16,1000},{8,1,4,32,1000},
//        {8,1,8,8,1000},{8,1,8,16,1000},{8,1,8,32,1000},
//        {8,1,16,16,1000},{8,1,16,32,1000},
//        {8,1,32,32,1000},
//
//        {16,1,1,1,1000},{16,1,1,2,1000},{16,1,1,4,1000},
//        {16,1,1,8,1000},{16,1,1,16,1000},{16,1,1,32,1000},
//        {16,1,2,2,1000},{16,1,2,4,1000},
//        {16,1,2,8,1000},{16,1,2,16,1000},{16,1,2,32,1000},
//        {16,1,4,4,1000},
//        {16,1,4,8,1000},{16,1,4,16,1000},{16,1,4,32,1000},
//        {16,1,8,8,1000},{16,1,8,16,1000},{16,1,8,32,1000},
//        {16,1,16,16,1000},{16,1,16,32,1000},
//        {16,1,32,32,1000},
//
//
//        {1,2,1,1,1000},{1,2,1,2,1000},{1,2,1,4,1000},
//        {1,2,1,8,1000},{1,2,1,16,1000},{1,2,1,32,1000},
//        {1,2,2,2,1000},{1,2,2,4,1000},
//        {1,2,2,8,1000},{1,2,2,16,1000},{1,2,2,32,1000},
//        {1,2,4,4,1000},
//        {1,2,4,8,1000},{1,2,4,16,1000},{1,2,4,32,1000},
//        {1,2,8,8,1000},{1,2,8,16,1000},{1,2,8,32,1000},
//        {1,2,16,16,1000},{1,2,16,32,1000},
//        {1,2,32,32,1000},
//
//        {2,2,1,1,1000},{2,2,1,2,1000},{2,2,1,4,1000},
//        {2,2,1,8,1000},{2,2,1,16,1000},{2,2,1,32,1000},
//        {2,2,2,2,1000},{2,2,2,4,1000},
//        {2,2,2,8,1000},{2,2,2,16,1000},{2,2,2,32,1000},
//        {2,2,4,4,1000},
//        {2,2,4,8,1000},{2,2,4,16,1000},{2,2,4,32,1000},
//        {2,2,8,8,1000},{2,2,8,16,1000},{2,2,8,32,1000},
//        {2,2,16,16,1000},{2,2,16,32,1000},
//        {2,2,32,32,1000},{4,2,1,1,1000},{4,2,1,2,1000},
//
//        {4,2,1,4,1000},{4,2,1,8,1000},{4,2,1,16,1000},
//        {4,2,1,32,1000},{4,2,2,2,1000},{4,2,2,4,1000},
//        {4,2,2,8,1000},{4,2,2,16,1000},{4,2,2,32,1000},
//        {4,2,4,4,1000},
//        {4,2,4,8,1000},{4,2,4,16,1000},{4,2,4,32,1000},
//        {4,2,8,8,1000},{4,2,8,16,1000},{4,2,8,32,1000},
//        {4,2,16,16,1000},{4,2,16,32,1000},
//        {4,2,32,32,1000},
//
//        {8,2,1,4,1000},{8,2,1,8,1000},{8,2,1,16,1000},
//        {8,2,1,32,1000},{8,2,2,2,1000},{8,2,2,4,1000},
//        {8,2,2,8,1000},{8,2,2,16,1000},{8,2,2,32,1000},
//        {8,2,4,4,1000},
//        {8,2,4,8,1000},{8,2,4,16,1000},{8,2,4,32,1000},
//        {8,2,8,8,1000},{8,2,8,16,1000},{8,2,8,32,1000},
//        {8,2,16,16,1000},{8,2,16,32,1000},
//        {8,2,32,32,1000},
//
//
//        {16,2,1,4,1000},{16,2,1,8,1000},{16,2,1,16,1000},
//        {16,2,1,32,1000},{16,2,2,2,1000},{16,2,2,4,1000},
//        {16,2,2,8,1000},{16,2,2,16,1000},{16,2,2,32,1000},
//        {16,2,4,4,1000},
//        {16,2,4,8,1000},{16,2,4,16,1000},{16,2,4,32,1000},
//        {16,2,8,8,1000},{16,2,8,16,1000},{16,2,8,32,1000},
//        {16,2,16,16,1000},{16,2,16,32,1000},
//        {16,2,32,32,1000},
};

#endif//DDT_RUNTESTS_H