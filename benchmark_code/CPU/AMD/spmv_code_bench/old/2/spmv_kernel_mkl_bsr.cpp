#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <omp.h>

#include <mkl.h>

#include "macros/cpp_defines.h"

#include "spmv_bench_common.h"
#include "spmv_kernel.h"


#ifndef BLOCK_SIZE
	#define BLOCK_SIZE  8
#endif


#ifndef BLOCK_SIZE
	#define BLOCK_SIZE  8
#endif


struct BCSRArrays : Matrix_Format
{
	INT_T nbBlocks;
	INT_T nbBlockRows;
	INT_T lb;              /*size of blocks*/
	INT_T ldabsr;          /*leading >= lb*lb*/
	ValueType *a;          /*values(m*lb*lb)*/
	INT_T *ia;             /*i(m+1)*/
	INT_T *ja;             /*j(m+1)*/
	INT_T allocatedBlocks;

	BCSRArrays(long m, long n, long nnz) : Matrix_Format(m, n, nnz)
	{
		a = NULL;
		ia = NULL;
		ja = NULL;
	}

	~BCSRArrays()
	{
		free(a);
		free(ia);
		free(ja);
	}

	void spmv(ValueType * x, ValueType * y);
	void statistics_start();
	int statistics_print_data(__attribute__((unused)) char * buf, __attribute__((unused)) long buf_n);
};


void compute_bcsr(BCSRArrays * bcsr, ValueType * x, ValueType * y);


void
BCSRArrays::spmv(ValueType * x, ValueType * y)
{
	compute_bcsr(this, x, y);
}


/** https://software.intel.com/fr-fr/node/520850#3A22B45C-4604-4444-B6FE-205A5CD4E667 */
struct Matrix_Format *
csr_to_format(INT_T * row_ptr, INT_T * col_ind, ValueType * values, long m, long n, long nnz, int symmetric)
{
	if (symmetric)
		error("symmetric matrices not supported by this format, expand symmetry");
	BCSRArrays * bcsr = new BCSRArrays(m, n, nnz);
	bcsr->format_name = (char *) "MKL_BSR";
	bcsr->mem_footprint = bcsr->nbBlocks*bcsr->lb*bcsr->lb*sizeof(ValueType) + (bcsr->nbBlockRows+1)*sizeof(INT_T) + bcsr->nbBlocks*sizeof(INT_T);
	INT_T job[6] = {
		0,    //If job(1)=0, the matrix in the CSR format is converted to the BSR format;
		0,    //If job(2)=0, zero-based indexing for the matrix in CSR format is used;
		0,    //If job(3)=0, zero-based indexing for the matrix in the BSR format is used;
		0,
		0,
		1     //If job(6)>0, all output arrays absr, jab, and iab are filled in for the output storage.
	};
	bcsr->nbBlocks = 0;

	// We need to count the number of blocks (aligned!).
	const INT_T maxBlockPerRow = (bcsr->n+BLOCK_SIZE-1)/BLOCK_SIZE;
	unsigned* usedBlocks = new unsigned[maxBlockPerRow];
	for(int idxRow = 0 ; idxRow < m ; ++idxRow){
		if(idxRow%BLOCK_SIZE == 0){
			memset(usedBlocks, 0, sizeof(unsigned)*maxBlockPerRow);
		}
		for(int idxVal = row_ptr[idxRow] ; idxVal < row_ptr[idxRow+1] ; idxVal++){
			const int idxCol = col_ind[idxVal];
			if(usedBlocks[idxCol/BLOCK_SIZE] == 0){
				usedBlocks[idxCol/BLOCK_SIZE] = 1;
				bcsr->nbBlocks += 1;
			}
		}
	}
	delete[] usedBlocks;

	bcsr->nbBlockRows = (bcsr->m+BLOCK_SIZE-1)/BLOCK_SIZE;
	bcsr->lb = BLOCK_SIZE;
	bcsr->ldabsr = bcsr->lb*bcsr->lb;
	bcsr->a = (ValueType *) aligned_alloc(64, bcsr->nbBlocks*bcsr->lb*bcsr->lb * sizeof(ValueType));
	bcsr->ia = (INT_T *) aligned_alloc(64, (bcsr->nbBlockRows+1) * sizeof(INT_T));
	bcsr->ja = (INT_T *) aligned_alloc(64, bcsr->nbBlocks * sizeof(INT_T));

	// printf("%d %d\n", bcsr->nbBlocks*bcsr->lb*bcsr->lb, nnz);

	// const double mem_footprint = bcsr->nbBlocks*BLOCK_SIZE*BLOCK_SIZE*sizeof(ValueType) + (bcsr->nbBlockRows+1)*sizeof(INT_T) + bcsr->nbBlocks*sizeof(INT_T);
	// std::cout << mem_footprint/(1024*1024) << "\n";
	// exit(0);

	#pragma omp parallel for
	for (int i=0;i<bcsr->nbBlocks;i++)
	{
		bcsr->a[i] = 0.0;
		bcsr->ja[i] = 0;
	}
	#pragma omp parallel for
	for (int i=0;i<bcsr->nbBlockRows+1;i++)
	{
		bcsr->ia[i] = 0;
	}

	// printf("lalala\n");
	INT_T info;
	#if DOUBLE == 0
		mkl_scsrbsr(job, &bcsr->m, &bcsr->lb, &bcsr->ldabsr, values, col_ind, row_ptr, bcsr->a, bcsr->ja, bcsr->ia, &info);
	#elif DOUBLE == 1
		mkl_dcsrbsr(job, &bcsr->m, &bcsr->lb, &bcsr->ldabsr, values, col_ind, row_ptr, bcsr->a, bcsr->ja, bcsr->ia, &info);
	#endif
	// printf("lalala\n");
	assert(bcsr->ia[bcsr->nbBlockRows] == bcsr->nbBlocks);
	return bcsr;
}


/** https://software.intel.com/fr-fr/node/520816#366F2854-A2C0-4661-8CE7-F478F8E6B613 */
void
compute_bcsr(BCSRArrays * bcsr, ValueType * x , ValueType * y )
{
    char transa = 'N';
    #if DOUBLE == 0
	    mkl_cspblas_sbsrgemv(&transa, &bcsr->nbBlockRows , &bcsr->lb , bcsr->a , bcsr->ia , bcsr->ja , x , y);
    #elif DOUBLE == 1
	    mkl_cspblas_dbsrgemv(&transa, &bcsr->nbBlockRows , &bcsr->lb , bcsr->a , bcsr->ia , bcsr->ja , x , y);
    #endif
}


//==========================================================================================================================================
//= Print Statistics
//==========================================================================================================================================


void
BCSRArrays::statistics_start()
{
}


int
statistics_print_labels(__attribute__((unused)) char * buf, __attribute__((unused)) long buf_n)
{
	return 0;
}


int
BCSRArrays::statistics_print_data(__attribute__((unused)) char * buf, __attribute__((unused)) long buf_n)
{
	return 0;
}

