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
	ValueType * val;      // the NNZ values - may include zeros (of size lval*ndiag)*/
	INT_T * idiag;        // distance from the diagonal (of size ndiag)
	INT_T lval;           // leading where the diagonals are stored >= m,  which is the declared leading dimension in the calling (sub)programs
	INT_T ndiag;          // number of diagonals that have at least one nnz

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


void compute_dia_custom(DIAArrays * dia , ValueType * x , ValueType * y);


void
DIAArrays::spmv(ValueType * x, ValueType * y)
{
	compute_dia_custom(this, x, y);
}


struct Matrix_Format *
csr_to_format(INT_T * row_ptr, INT_T * col_ind, ValueTypeReference * values, long m, long n, long nnz, long symmetric, long symmetry_expanded)
{
	if (symmetric && !symmetry_expanded)
		error("symmetric matrices have to be expanded to be supported by this format");
	struct DIAArrays * dia = new DIAArrays(m, n, nnz);
	dia->format_name = (char *) "Custom_DIA";
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


//==========================================================================================================================================
//= DIA Custom
//==========================================================================================================================================


void
compute_dia_custom(DIAArrays * dia , ValueType * x , ValueType * y)
{
	// INT_T * offsets = (INT_T *) aligned_alloc(64, dia->ndiag * sizeof(*offsets));
	// long i;
	// printf("ndiag = %d , lval = %d\n", dia->ndiag, dia->lval);
	// for (i=0;i<dia->ndiag;i++)
	// {
		// offsets[i] = (dia->idiag[i] + dia->lval) % dia->lval;
		// printf("i=%ld , idiag=%d , offset=%d\n", i, dia->idiag[i], offsets[i]);
	// }
	// for (i=0;i<dia->lval;i++)
	// {
		// for (long j=0;j<dia->ndiag;j++)
			// printf("%lf ", dia->val[i*dia->ndiag + j]);
		// printf("\n");
	// }
	#pragma omp parallel
	{
		long i, j;
		INT_T col;

		#pragma omp for schedule(static)
		// #pragma omp for schedule(dynamic, 1024)
		// #pragma omp for schedule(hierarchical, 64)
		for (i=0;i<dia->lval;i++)
		{
			y[i] = 0;
			for (j=0;j<dia->ndiag;j++)
			{
				// col = (offsets[j] + i) % dia->lval;
				col = dia->idiag[j] + i;
				if (col < 0 || col >= dia->lval)
				{
					// printf("%d\n", col);
					continue;
				}
				y[i] += dia->val[i*dia->ndiag + j] * x[col];
			}
		}
	}
	// free(offsets);
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

