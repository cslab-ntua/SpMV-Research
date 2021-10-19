///
/// \author Anastasiadis Petros (panastas@cslab.ece.ntua.gr)
///
/// \brief A library wrapper for the CSR5 library
///
#ifndef MKL_H
#define MKL_H

#include <gpu_utils.hpp>
#include <spmv_utils.hpp>

#include <cmath>
#include <iostream>

typedef struct {
  bool numavail;
  int sockets;
} mkl_wrap;

void *numalloc(size_t bytes, int socket, mkl_wrap *wrapper);

mkl_wrap *mkl_desc();

void mkl_free(mkl_wrap *tmp);

#endif
