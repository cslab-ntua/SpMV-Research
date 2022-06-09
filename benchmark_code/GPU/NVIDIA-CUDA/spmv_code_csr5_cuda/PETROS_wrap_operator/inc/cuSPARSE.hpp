///
/// \author Anastasiadis Petros (panastas@cslab.ece.ntua.gr)
///
/// \brief Nvidia CuSPARSE wrapper for benchmarking
///
#ifndef CUSPARSE_H
#define CUSPARSE_H

#include <gpu_utils.hpp>
#include <spmv_utils.hpp>

typedef struct {
  cudaStream_t stream;
  cudaDeviceProp properties;
  cusparseHandle_t handle;
  cusparseMatDescr_t descA;
  cusparseMatDescr_t descB;
  cusparseDirection_t dir;
  SpmvMemType target_mem;
} cuSPARSE_wrap;

/// Create a library descriptor for the cuSPARSE library
/// \return the descriptor
cuSPARSE_wrap *cuSPARSE_desc();

/// Free a cuSPARSE library descriptor
// TODO: Maybe free properties?
void cuSPARSE_free(cuSPARSE_wrap *tmp);

#endif
