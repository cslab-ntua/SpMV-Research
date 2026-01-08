#include <stdlib.h>
#include <stdio.h>
#include <omp.h>
#include <iostream>
#include <cmath>

#include "csr5_cuda/anonymouslib_cuda.h"

#include "macros/cpp_defines.h"

#include "spmv_kernel.h"

#ifdef __cplusplus
extern "C"{
#endif
	#include "macros/macrolib.h"
	#include "time_it.h"
	#include "parallel_util.h"
	#include "array_metrics.h"

	#include "cuda/cuda_util.h"
#ifdef __cplusplus
}
#endif

using namespace std;


struct CSR5Arrays : Matrix_Format
{
	anonymouslibHandle<int, unsigned int, ValueType> * A;

	INT_T * ia;      // the usual rowptr (of size m+1)
	INT_T * ja;      // the colidx of each NNZ (of size nnz)
	ValueType * a;   // the values (of size NNZ)
	INT_T * ia_d;
	INT_T * ja_d;
	ValueType * a_d;

	ValueType * x = NULL;
	ValueType * y = NULL;
	ValueType * x_d = NULL;
	ValueType * y_d = NULL;

	CSR5Arrays(INT_T * ia, INT_T * ja, ValueType * a, long m, long n, long nnz) : Matrix_Format(m, n, nnz), ia(ia), ja(ja), a(a)
	{
		// Matrix A
		gpuCudaErrorCheck(cudaMalloc(&ia_d, (m+1) * sizeof(INT_T)));
		gpuCudaErrorCheck(cudaMalloc(&ja_d, nnz  * sizeof(INT_T)));
		gpuCudaErrorCheck(cudaMalloc(&a_d,  nnz  * sizeof(ValueType)));

		gpuCudaErrorCheck(cudaMemcpy(ia_d, ia, (m+1) * sizeof(INT_T),   cudaMemcpyHostToDevice));
		gpuCudaErrorCheck(cudaMemcpy(ja_d, ja, nnz * sizeof(INT_T),     cudaMemcpyHostToDevice));
		gpuCudaErrorCheck(cudaMemcpy(a_d,   a, nnz * sizeof(ValueType), cudaMemcpyHostToDevice));

		gpuCudaErrorCheck(cudaMalloc(&x_d, n * sizeof(*x_d)));
		gpuCudaErrorCheck(cudaMalloc(&y_d, m * sizeof(*y_d)));

	}

	~CSR5Arrays()
	{
		free(ia);
		free(ja);
		free(a);
		A->destroy();
		gpuCudaErrorCheck(cudaFree(ia_d));
		gpuCudaErrorCheck(cudaFree(ja_d));
		gpuCudaErrorCheck(cudaFree(a_d));
		gpuCudaErrorCheck(cudaFree(x_d));
		gpuCudaErrorCheck(cudaFree(y_d));
	}

	void spmv(ValueType * x, ValueType * y);
	void statistics_start();
	int statistics_print_data(__attribute__((unused)) char * buf, __attribute__((unused)) long buf_n);
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
	struct CSR5Arrays * csr5 = new CSR5Arrays(row_ptr, col_ind, values, m, n, nnz);
	csr5->format_name = (char *) "CSR5_CUDA";

	csr5->A = new anonymouslibHandle<int, unsigned int, ValueType>(m, n);
	csr5->A->inputCSR(nnz, csr5->ia_d, csr5->ja_d, csr5->a_d);

	int sigma = ANONYMOUSLIB_AUTO_TUNED_SIGMA;         // defined in common_cuda.h
	// int sigma = nnz / (8*ANONYMOUSLIB_CSR5_OMEGA);
	csr5->A->setSigma(sigma);

	csr5->A->asCSR5();

	return csr5;
}


void
compute_csr5(CSR5Arrays * csr5, ValueType * x , ValueType * y)
{
	if (csr5->x == NULL)
	{
		csr5->x = x;
		gpuCudaErrorCheck(cudaMemcpy(csr5->x_d, x, csr5->n * sizeof(ValueType), cudaMemcpyHostToDevice));
		csr5->A->setX(csr5->x_d);
	}

	ValueType alpha = 1.0;
	csr5->A->spmv(alpha, csr5->y_d);
	gpuCudaErrorCheck(cudaDeviceSynchronize());
	
	if (csr5->y == NULL)
	{
		csr5->y = y;
		gpuCudaErrorCheck(cudaMemcpy(csr5->y, csr5->y_d, csr5->m * sizeof(ValueType), cudaMemcpyDeviceToHost));
	}

}


//==========================================================================================================================================
//= Print Statistics
//==========================================================================================================================================


void
CSR5Arrays::statistics_start()
{
}


int
statistics_print_labels(__attribute__((unused)) char * buf, __attribute__((unused)) long buf_n)
{
	return 0;
}


int
CSR5Arrays::statistics_print_data(__attribute__((unused)) char * buf, __attribute__((unused)) long buf_n)
{
	return 0;
}

