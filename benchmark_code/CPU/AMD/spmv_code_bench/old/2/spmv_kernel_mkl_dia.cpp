#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <omp.h>

#include <mkl.h>

#include "macros/cpp_defines.h"

#include "spmv_bench_common.h"
#include "spmv_kernel.h"


struct DIAArrays : Matrix_Format
{
	ValueType * val;   // the NNZ values - may include zeros (of size lval*ndiag)*/
	INT_T * idiag;     // distance from the diagonal (of size ndiag)
	INT_T lval;        // leading where the diagonals are stored >= m,  which is the declared leading dimension in the calling (sub)programs
	INT_T ndiag;       // number of diagonals that have at least one nnz

	DIAArrays(long m, long n, long nnz) : Matrix_Format(m, n, nnz)
	{
		val = NULL;
		idiag = NULL;
	}

	~DIAArrays()
	{
		free(val);
		free(idiag);
	}

	void spmv(ValueType * x, ValueType * y);
	void statistics_start();
	int statistics_print_data(__attribute__((unused)) char * buf, __attribute__((unused)) long buf_n);
};


void compute_dia(DIAArrays * dia , ValueType * x , ValueType * y);


void
DIAArrays::spmv(ValueType * x, ValueType * y)
{
	compute_dia(this, x, y);
}


struct Matrix_Format *
csr_to_format(INT_T * row_ptr, INT_T * col_ind, ValueType * values, long m, long n, long nnz, int symmetric)
{
	if (symmetric)
		error("symmetric matrices not supported by this format, expand symmetry");
	struct DIAArrays * dia = new DIAArrays(m, n, nnz);
	dia->format_name = (char *) "MKL_DIA";
	dia->mem_footprint = dia->ndiag*dia->lval*sizeof(ValueType) + dia->ndiag*sizeof(INT_T);
	INT_T job[6] = {0,//If job(1)=0, the matrix in the CSR format is converted to the diagonal format;
		0,//If job(2)=0, zero-based indexing for the matrix in CSR format is used;
		1,//if job(3)=1, one-based indexing for the matrix in the diagonal format is used.
		0,
		0,
		10//If job(6)=10, diagonals are selected internally, and acsr_rem, ja_rem, ia_rem are not filled in for the output storage.
	};
	dia->lval = dia->m;
	dia->ndiag = 0;
	// We need to count the number of diagonals with NNZ
	{
		unsigned* usedDiag = new unsigned[m*2-1];
		memset(usedDiag, 0, sizeof(unsigned)*(m*2-1));
		for(int idxRow = 0 ; idxRow < m ; ++idxRow){
			for(int idxVal = row_ptr[idxRow] ; idxVal < row_ptr[idxRow+1] ; ++idxVal){
				const int idxCol = col_ind[idxVal];
				const int diag = m-idxRow+idxCol-1;
				assert(0 <= diag && diag < m*2-1);
				if(usedDiag[diag] == 0){
					usedDiag[diag] = 1;
					dia->ndiag += 1;
				}
			}
		}
		delete[] usedDiag;
	}
	// Allocate the working arrays
	dia->val = (ValueType *) aligned_alloc(64, dia->ndiag*dia->lval * sizeof(ValueType));
	dia->idiag = (INT_T *) aligned_alloc(64, dia->ndiag * sizeof(INT_T));
	// const double mem_footprint = dia->ndiag*dia->lval*sizeof(ValueType) + dia->ndiag*sizeof(INT_T);
	// std::cout << mem_footprint/(1024*1024) << "\n";
	// exit(0);
	#pragma omp parallel for
	for (int i=0;i<dia->ndiag*dia->lval;i++)
		dia->val[i] = 0.0;
	#pragma omp parallel for
	for (int i=0;i<dia->ndiag;i++)
		dia->idiag[i] = 0;
	INT_T info;
	#if DOUBLE == 0
		mkl_scsrdia(job, &dia->m, values, col_ind, row_ptr, dia->val, &dia->lval, dia->idiag, &dia->ndiag, NULL, NULL, NULL, &info);
	#elif DOUBLE == 1
		mkl_dcsrdia(job, &dia->m, values, col_ind, row_ptr, dia->val, &dia->lval, dia->idiag, &dia->ndiag, NULL, NULL, NULL, &info);
	#endif
	return dia;
}


/** https://software.intel.com/fr-fr/node/520806#FCB5B469-8AA1-4CFB-88BE-E2F22E9E2AF0 */
// ValueType *val;    //< the NNZ values - may include zeros (of size lval*ndiag)*/
// MKL_INT *idiag;    //< distance from the diagonal (of size ndiag)
// MKL_INT lval;      //< leading where the diagonals are stored >= m,  which is the declared leading dimension in the calling (sub)programs
// MKL_INT ndiag;     //< number of diagonals that have at least one nnz
void
compute_dia(DIAArrays * dia , ValueType * x , ValueType * y)
{
	char transa = 'N';
	#if DOUBLE == 0
		mkl_sdiagemv(&transa, &dia->m , dia->val , &dia->lval , dia->idiag , &dia->ndiag , x , y);
	#elif DOUBLE == 1
		mkl_ddiagemv(&transa, &dia->m , dia->val , &dia->lval , dia->idiag , &dia->ndiag , x , y);
	#endif
}


//==========================================================================================================================================
//= Print Statistics
//==========================================================================================================================================


void
DIAArrays::statistics_start()
{
}


int
statistics_print_labels(__attribute__((unused)) char * buf, __attribute__((unused)) long buf_n)
{
	return 0;
}


int
DIAArrays::statistics_print_data(__attribute__((unused)) char * buf, __attribute__((unused)) long buf_n)
{
	return 0;
}

