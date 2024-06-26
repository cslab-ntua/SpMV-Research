//
// Created by genshen on 2021/7/13.
//

#ifndef SPMV_ACC_THREAD_ROW_CONFIG_H
#define SPMV_ACC_THREAD_ROW_CONFIG_H

#define THREAD_ROW_GLOBAL_LOAD_X2 // if defined, we load 2 double or 2 int in each loop.

// remapping memory accessing mode of vector x.
// In most cases, the matrix is a diagonal matrix.
// Thus, it is better to access vector x with column first mode.
// If the macro below is enabled, the memory accessing mode of vector x is column-first,
// otherwise, the mode is row-first.
constexpr int THREAD_ROW_OPT_LEVEL_REMAP_VEC_X = 1;

// thread row at block level
constexpr int THREAD_ROW_OPT_LEVEL_BLOCK = 2;

// thread row at block level with vector x remapping
constexpr int THREAD_ROW_OPT_LEVEL_BLOCK_VEC_X = 3;

// thread row at block level with vector x remapping.
// but the block number is set to be `ceil(m/THREADS)`, where `THREADS` is thread number in HIP Block.
constexpr int THREAD_ROW_OPT_LEVEL_BLOCK_VEC_X_SINGLE = 4;

// the thread row optimization level
constexpr int THREAD_ROW_OPT_LEVEL = THREAD_ROW_OPT_LEVEL_BLOCK_VEC_X_SINGLE;

#endif // SPMV_ACC_THREAD_ROW_CONFIG_H
