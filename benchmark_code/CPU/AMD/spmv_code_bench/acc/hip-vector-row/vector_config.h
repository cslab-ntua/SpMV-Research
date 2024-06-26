//
// Created by genshen on 2021/6/28.
//

#ifndef SPMV_ACC_VEOTR_ROW_CONFIG_H
#define SPMV_ACC_VEOTR_ROW_CONFIG_H

#define GLOBAL_LOAD_X2 // if defined, we load 2 double or 2 int in each loop.

// #define ASYNC_LOAD // asynchronously load data from device memory.

#define MEMORY_ACCESS_Y_COALESCING

// memory coalescing for adaptive vector row (with data block dividing), default is disabled.
//#define ADAPTIVE_Y_MEMORY_COALESCING

#endif // SPMV_ACC_VEOTR_ROW_CONFIG_H
