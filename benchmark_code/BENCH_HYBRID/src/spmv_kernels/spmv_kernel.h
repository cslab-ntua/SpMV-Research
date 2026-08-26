#ifndef SPMV_KERNELS_H
#define SPMV_KERNELS_H

#include "macros/cpp_defines.h"



struct Matrix_Format
{
	char * format_name;
	long m;                         // num rows
	long n;                         // num columns
	long nnz;                       // num non-zeros
	double mem_footprint;
	double csr_mem_footprint;

	virtual void spmv(ValueType * x, ValueType * y) = 0;
	// SpMV operating directly on device pointers — no host↔device vector copies.
	// GPU formats override this; CPU-only formats use the default which errors.
	virtual void spmv_gpu(ValueType * x_d, ValueType * y_d)
	{
		fprintf(stderr, "ERROR: spmv_gpu() not implemented for format '%s'\n", format_name);
		exit(1);
	}
	virtual void synchronize() {}
	virtual void set_last_iteration(bool /* is_last */) {} // So that we do not get an unnecessary warning about unused variable
	virtual double get_last_duration() { return 0; } // Returns duration in milliseconds
	virtual void statistics_start() = 0;
	virtual int statistics_print_data(char * buf, long buf_n) = 0;

	// Issue proactive asynchronous array push for next iteration
	virtual void issue_h2d_for_next_iteration(ValueType * /* y */) {}

	Matrix_Format(long m, long n, long nnz) : m(m), n(n), nnz(nnz)
	{
		csr_mem_footprint = nnz * (sizeof(ValueType) + sizeof(INT_T)) + (m+1) * sizeof(INT_T);
	}
	virtual ~Matrix_Format() {}
};


struct Matrix_Format * csr_to_format(INT_T * row_ptr, INT_T * col_ind, ValueTypeReference * values, long m, long n, long nnz, long symmetric, long symmetry_expanded);
int statistics_print_labels(char * buf, long buf_n);


#endif /* SPMV_KERNELS_H */

