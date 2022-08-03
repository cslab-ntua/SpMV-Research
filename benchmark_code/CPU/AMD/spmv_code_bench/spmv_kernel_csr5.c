#include <stdlib.h>
#include <stdio.h>
#include <omp.h>
#include <iostream>
#include <cmath>

#include "csr5/anonymouslib_avx2.h"
#include "csr5/mmio.h"

#include "macros/cpp_defines.h"

#include "spmv_bench_common.h"
#include "spmv_kernel.h"

using namespace std;


struct CSR5Arrays : Matrix_Format
{
	anonymouslibHandle<int, unsigned int, ValueType> * A;

	CSR5Arrays(long m, long n, long nnz) : Matrix_Format(m, n, nnz)
	{
	}

	~CSR5Arrays()
	{
		A->destroy();
	}

	void spmv(ValueType * x, ValueType * y);
};


void compute_csr5(CSR5Arrays * csr5, ValueType * x , ValueType * y);


void
CSR5Arrays::spmv(ValueType * x, ValueType * y)
{
	compute_csr5(this, x, y);
}


struct Matrix_Format *
csr_to_format(INT_T * row_ptr, INT_T * col_ind, ValueType * values, long m, long n, long nnz)
{
	struct CSR5Arrays * csr5 = new CSR5Arrays(m, n, nnz);
	__attribute__((unused)) int err = 0;
	csr5->format_name = (char *) "CSR5";

	csr5->A = new anonymouslibHandle<int, unsigned int, ValueType>(m, n);
	err = csr5->A->inputCSR(nnz, row_ptr, col_ind, values);
	//cout << "inputCSR err = " << err << endl;

	int sigma = ANONYMOUSLIB_CSR5_SIGMA;         // defined in common_avx2.h
	// int sigma = nnz / (8*ANONYMOUSLIB_CSR5_OMEGA);
	csr5->A->setSigma(sigma);

	err = csr5->A->asCSR5();

	return csr5;
}


void
compute_csr5(CSR5Arrays * csr5, ValueType * x , ValueType * y)
{
	ValueType alpha = 1.0;
	__attribute__((unused)) int err = 0;
	err = csr5->A->setX(x);
	//cout << "setX err = " << err << endl;
	err = csr5->A->spmv(alpha, y);
}

