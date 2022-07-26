#ifndef SPMV_KERNELS_H
#define SPMV_KERNELS_H

#include "macros/cpp_defines.h"


struct Matrix_Format
{
	char * format_name;
	INT_T m;                         // num rows
	INT_T n;                         // num columns
	INT_T nnz;                       // num non-zeros
	double mem_footprint;

	virtual void spmv(ValueType * x, ValueType * y) = 0;

	Matrix_Format(long m, long n, long nnz) : m(m), n(n), nnz(nnz) {}
};


struct Matrix_Format * csr_to_format(INT_T * row_ptr, INT_T * col_ind, ValueType * values, long m, long n, long nnz);


#endif /* SPMV_KERNELS_H */

