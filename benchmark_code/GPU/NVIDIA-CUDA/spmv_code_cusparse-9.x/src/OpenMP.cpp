///
/// \author Anastasiadis Petros (panastas@cslab.ece.ntua.gr)
///
/// \brief Some OpenMP implementations for the SpMV kernel
///

#include "OpenMP.hpp"

#include <cmath>
#include <iostream>
#include <limits>
#include <map>
#include <string>
using namespace std;
#include <unistd.h>

void SpmvOperator::openmp_check_compatibility() {
  ddebug(" -> SpmvOperator::openmp_check_compatibility()\n");
  massert(value_type == SPMV_VALUETYPE_DOUBLE || value_type == SPMV_VALUETYPE_FLOAT,
          "SpmvOperator::openmp_check_compatibility -> unsupported SpMV value "
          "datatype");
  massert(
      format == SPMV_FORMAT_CSR,
      "SpmvOperator::openmp_check_compatibility -> unsupported SpMV format");
  massert(
      mem_alloc == SPMV_MEMTYPE_HOST || mem_alloc == SPMV_MEMTYPE_UNIFIED,
      "SpmvOperator::openmp_check_compatibility -> unsupported SpMV mem_alloc");
  if (0)
    massert(0,
            "SpmvOperator::openmp_check_compatibility -> SpmvOperator not "
            "compatible");
  ddebug(" <- SpmvOperator::openmp_check_compatibility()\n");
}

void SpmvOperator::openmp_init() {
  ddebug(" -> SpmvOperator::openmp_init()\n");
  openmp_check_compatibility();
  free_lib_struct();
  lib_struct = NULL;
  lib = SPMV_LIBRARY_OPENMP;
  ddebug(" <- SpmvOperator::openmp_init()\n");
}

void SpmvOperator::openmp_csr() {
  ddebug(" -> SpmvOperator::openmp_csr()\n");
  switch(value_type){
	case(SPMV_VALUETYPE_DOUBLE):
	{
  		double alpha = 1.0;
  		int *rowPtr = (int *)((SpmvCsrData *)format_data)->rowPtr,
      		*colInd = (int *)((SpmvCsrData *)format_data)->colInd;
  		double *values = (double *)((SpmvCsrData *)format_data)->values,
         		*x0 = (double *)x, *y0 = (double *)y;
		// cout << "Max threads: " << omp_get_max_threads() << endl;
		#pragma omp parallel for
  		for (int i = 0; i < m; i++) {
    			double sum = 0;
    		for (int j = rowPtr[i]; j < rowPtr[i + 1]; j++)
      			sum += alpha * x0[colInd[j]] * values[j];
    			y0[i] = sum;
  		}
		break;
	}
	case(SPMV_VALUETYPE_FLOAT):
	{
  		float alpha = 1.0;
  		int *rowPtr = (int *)((SpmvCsrData *)format_data)->rowPtr,
      		*colInd = (int *)((SpmvCsrData *)format_data)->colInd;
  		float *values = (float *)((SpmvCsrData *)format_data)->values,
         		*x0 = (float *)x, *y0 = (float *)y;
		// cout << "Max threads: " << omp_get_max_threads() << endl;
		#pragma omp parallel for
  		for (int i = 0; i < m; i++) {
    			float sum = 0;
    		for (int j = rowPtr[i]; j < rowPtr[i + 1]; j++)
      			sum += alpha * x0[colInd[j]] * values[j];
    			y0[i] = sum;
  		}
		break;
	}
	default:
		      massert(0, "SpmvOperator::openmp_csr(): Valuetype not implemented\n");
	}
  ddebug(" <- SpmvOperator::openmp_csr()\n");
}

int get_num_threads()
{
	int res = -1; 
	# pragma omp parallel
	{
		res = omp_get_num_threads();
	}
	return res;
}
	
