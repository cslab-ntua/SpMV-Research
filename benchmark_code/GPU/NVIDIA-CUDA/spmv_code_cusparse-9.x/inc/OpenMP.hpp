///
/// \author Anastasiadis Petros (panastas@cslab.ece.ntua.gr)
///
/// \brief Some naive OpenMP implementations for SpMV
///
#ifndef MY_OPENMP_H
#define MY_OPENMP_H

#include <omp.h>

#include <spmv_utils.hpp>

void openmp_init();
void openmp_csr();

#endif
