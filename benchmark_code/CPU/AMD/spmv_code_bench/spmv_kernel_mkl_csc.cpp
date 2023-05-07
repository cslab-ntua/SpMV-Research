#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

#include <mkl.h>

#include "macros/cpp_defines.h"

#include "spmv_bench_common.h"
#include "spmv_kernel.h"


struct CSCArrays : Matrix_Format
{
	ValueType * a;   // the values (of size NNZ)
	INT_T * ia;      // the usual colptr (of size m+1)
	INT_T * ja;      // the rowidx of each NNZ (of size nnz)

	CSCArrays(long m, long n, long nnz) : Matrix_Format(m, n, nnz)
	{
		a = NULL;
		ia = NULL;
		ja= NULL;
	}

	~CSCArrays()
	{
		free(a);
		free(ia);
		free(ja);
	}

	void spmv(ValueType * x, ValueType * y);
	void statistics_start();
	void statistics_print();
};


void compute_csc(CSCArrays * csc, ValueType * x, ValueType * y);


void
CSCArrays::spmv(ValueType * x, ValueType * y)
{
	compute_csc(this, x, y);
}


struct Matrix_Format *
csr_to_format(INT_T * row_ptr, INT_T * col_ind, ValueType * values, long m, long n, long nnz)
{
	CSCArrays * csc = new CSCArrays(m, n, nnz);
	csc->format_name = (char *) "MKL_CSC";
	csc->mem_footprint = nnz * (sizeof(ValueType) + sizeof(INT_T)) + (n+1) * sizeof(INT_T);
	INT_T job[6] = {
		0, // If job(1)=0, the matrix in the CSR format is converted to the CSC format;
		0, // If job(2)=0, zero-based indexing for the matrix in CSR format is used;
		0, // If job(3)=0, zero-based indexing for the matrix in CSC format is used;
		0,
		0,
		1  // If job(6)≠0, all output arrays acsc, ja1, and ia1 are filled in for the output storage.
	};
	// Init csc
	csc->a = (ValueType *) aligned_alloc(64, csc->nnz * sizeof(ValueType));
	csc->ia = (INT_T *) aligned_alloc(64, (csc->n+1) * sizeof(INT_T));
	csc->ja = (INT_T *) aligned_alloc(64, csc->nnz * sizeof(INT_T));
	INT_T info;

	#if DOUBLE == 0
		mkl_scsrcsc(job, &csc->m, values, col_ind, row_ptr, csc->a, csc->ja, csc->ia, &info);
	#elif DOUBLE == 1
		mkl_dcsrcsc(job, &csc->m, values, col_ind, row_ptr, csc->a, csc->ja, csc->ia, &info);
	#endif
	return csc;
}


void
compute_csc(CSCArrays * csc, ValueType * x, ValueType * y)
{
    char transa = 'N';
    ValueType alpha = 1, beta = 0;
    char matdescra[7] = "G--C--";

    #if DOUBLE == 0
	    mkl_scscmv(&transa, &csc->m, &csc->n, &alpha, matdescra, csc->a, csc->ja, csc->ia, csc->ia+1, x, &beta, y);
    #elif DOUBLE == 1
	    mkl_dcscmv(&transa, &csc->m, &csc->n, &alpha, matdescra, csc->a, csc->ja, csc->ia, csc->ia+1, x, &beta, y);
    #endif
}


//==========================================================================================================================================
//= Print Statistics
//==========================================================================================================================================


void
CSCArrays::statistics_start()
{
}


void
CSCArrays::statistics_print()
{
}

