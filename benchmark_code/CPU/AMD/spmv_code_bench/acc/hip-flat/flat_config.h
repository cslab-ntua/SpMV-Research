//
// Created by chu genshen on 2021/9/1.
//

#ifndef SPMV_ACC_FLAT_CONFIG_H
#define SPMV_ACC_FLAT_CONFIG_H

constexpr int FLAT_REDUCE_OPTION_VEC = 0;
constexpr int FLAT_REDUCE_OPTION_VEC_MEM_COALESCING = 1;
constexpr int FLAT_REDUCE_OPTION_DIRECT = 2;

constexpr int DEFAULT_FLAT_REDUCE_OPTION = FLAT_REDUCE_OPTION_VEC_MEM_COALESCING;

constexpr bool FLAT_ONE_PASS = true;
constexpr bool FLAT_ONE_PASS_ADAPTIVE = true;

#endif // SPMV_ACC_FLAT_CONFIG_H
