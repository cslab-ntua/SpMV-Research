#ifndef SPMV_KERNELS_H
#define SPMV_KERNELS_H

#include "macros/cpp_defines.h"

#include "spmv_bench_common.h"


struct Matrix_Format
{
	char * format_name;
	INT_T m;                         // num rows
	INT_T n;                         // num columns
	INT_T nnz;                       // num non-zeros
	double mem_footprint;
	double csr_mem_footprint;

	virtual void spmv(ValueType * x, ValueType * y) = 0;
	virtual void statistics_start() = 0;
	virtual int statistics_print_data(char * buf, long buf_n) = 0;

	Matrix_Format(long m, long n, long nnz) : m(m), n(n), nnz(nnz)
	{
		csr_mem_footprint = nnz * (sizeof(ValueType) + sizeof(INT_T)) + (m+1) * sizeof(INT_T);
	}
};


struct Matrix_Format * csr_to_format(INT_T * row_ptr, INT_T * col_ind, ValueType * values, long m, long n, long nnz, int symmetric);
int statistics_print_labels(char * buf, long buf_n);


#endif /* SPMV_KERNELS_H */

