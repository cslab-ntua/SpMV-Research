#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

#include <mkl.h>

#include "macros/cpp_defines.h"

#include "common.h"
#include "spmv_kernel.h"

#ifdef __cplusplus
extern "C"{
#endif
	#include "macros/macrolib.h"
	#include "time_it.h"
	#include "parallel_util.h"
#ifdef __cplusplus
}
#endif


struct COOArrays : Matrix_Format
{
	ValueType * val;    // the values (size = nnz)
	INT_T * rowind;     // the row indexes (size = nnz)
	INT_T * colind;     // the col indexes (size = nnz)

	COOArrays(long m, long n, long nnz) : Matrix_Format(m, n, nnz)
	{
		val = NULL;
		rowind = NULL;
		colind = NULL;
	}

	~COOArrays(){
		free(val);
		free(rowind);
		free(colind);
	}

	void spmv(ValueType * x, ValueType * y);
};


void compute_coo(COOArrays * coo, ValueType * x , ValueType * y);


void
COOArrays::spmv(ValueType * x, ValueType * y)
{
	compute_coo(this, x, y);
}


struct Matrix_Format *
csr_to_format(INT_T * row_ptr, INT_T * col_ind, ValueType * values, long m, long n, long nnz)
{
	struct COOArrays * coo = new COOArrays(m, n, nnz);
	coo->format_name = (char *) "MKL_COO";
	coo->mem_footprint = nnz * (sizeof(ValueType) + 2 * sizeof(INT_T));
	coo->rowind = (typeof(coo->rowind)) aligned_alloc(64, nnz * sizeof(*coo->rowind));
	coo->colind = (typeof(coo->colind)) aligned_alloc(64, nnz * sizeof(*coo->colind));
	coo->val = (typeof(coo->val)) aligned_alloc(64, nnz * sizeof(*coo->val));
	#pragma omp parallel
	{
		long i, j, j_s, j_e;
		#pragma omp for
		for (i=0;i<nnz;i++)
		{
			coo->rowind = 0;
			coo->colind = 0;
			coo->val = 0;
		}
		#pragma omp for
		for (i=0;i<m;i++)
		{
			j_s = row_ptr[i];
			j_e = row_ptr[i+1];
			for (j=j_s;j<j_e;j++)
			{
				coo->rowind[j] = i;
				coo->colind[j] = col_ind[j];
				coo->val[j] = values[j];
			}
		}
	}
	return coo;
}


/** see https://software.intel.com/fr-fr/node/520817#38F0A87C-7884-4A96-B83E-CEE88290580F */
void compute_coo(COOArrays * coo, ValueType * x , ValueType * y)
{
    char transa = 'N';
    #if DOUBLE == 0
	    mkl_cspblas_scoogemv(&transa, &coo->m, coo->val, coo->rowind, coo->colind, &coo->nnz, x, y);
    #elif DOUBLE == 1
	    mkl_cspblas_dcoogemv(&transa, &coo->m, coo->val, coo->rowind, coo->colind, &coo->nnz, x, y);
    #endif
}

