#ifndef UTIL_H
#define UTIL_H

#include <mkl.h>

#include <cstring> // memset
#include <cassert> // for assert
#include <omp.h>   // just for timer
#include <cstdlib> // for random generation
#include <cstdio>  // for printf

#include <cmath>

#include <iterator>
#include <string>
#include <algorithm>
#include <queue>
#include <set>
#include <list>
#include <fstream>
#include <stdio.h>
#include <sys/time.h>

// Utils macro
#define Min(x,y) ((x)<(y)?(x):(y))
#define Max(x,y) ((x)>(y)?(x):(y))
#define Abs(x) ((x)>(0)?(x):-(x))

#include <iostream>


#include "debug.h"
#include "io.h"
#include "macros/cpp_defines.h"
#include "parallel_util.h"
#include "artificial_matrix_generation.h"


#if DOUBLE == 0
	#define ValueType  float
#elif DOUBLE == 1
	#define ValueType  double
#endif

/* #define error(...)                       \
do {                                     \
	fprintf(stderr, __VA_ARGS__);    \
	exit(1);                         \
} while (0) */


typedef ValueType  Vector_Value_t  __attribute__((vector_size(32), aligned(1)));
// typedef ValueType  Vector_Value_t  __attribute__((vector_size(32)));

// Number of elements for the vectorization function.
#define VECTOR_ELEM_NUM  ((int) (sizeof(Vector_Value_t) / sizeof(ValueType)))

// typedef MKL_INT  Vector_Index_t  __attribute__((vector_size(VECTOR_ELEM_NUM*sizeof(MKL_INT)), aligned(1)));
typedef MKL_INT  Vector_Index_t  __attribute__((vector_size(VECTOR_ELEM_NUM*sizeof(MKL_INT))));


template<typename T>
T *
transpose(T * a, MKL_INT m, MKL_INT n)
{
	T * t = (T *) mkl_malloc(m*n * sizeof(*t), 64);
	#pragma omp parallel
	{
		long i, j;
		#pragma omp for schedule(static)
		for (j=0;j<n;j++)
		{
			for (i=0;i<m;i++)
				t[j*m + i] = a[i*n + j];
		}
	}
	return t;
}


//////////////////////////////////////////////////////////////////////////
// COO format
//////////////////////////////////////////////////////////////////////////


struct COOArrays
{
	MKL_INT m;         //< rows
	MKL_INT n;         //< columns
	MKL_INT nnz;       //< the number of nnz inside the matrix
	ValueType *val;    //< the values (size = nnz)
	MKL_INT *rowind;   //< the row indexes (size = nnz)
	MKL_INT *colind;   //< the col indexes (size = nnz)

	/** simply set ptr to null */
	COOArrays(){
		val = NULL;
		rowind = NULL;
		colind = NULL;
	}

	/** delete ptr */
	~COOArrays(){
		// delete[] val;
		// delete[] rowind;
		// delete[] colind;
		mkl_free(val);
		mkl_free(rowind);
		mkl_free(colind);
	}
};


/**
 * Builds a MARKET COO sparse from the given file.
 */
void create_coo_matrix(const std::string & market_filename, COOArrays * coo)
{
	std::ifstream ifs;
	ifs.open(market_filename.c_str(), std::ifstream::in);
	if (!ifs.good())
		error("Error opening file\n");

	bool    array = false;
	bool    symmetric = false;
	bool    skew = false;
	MKL_INT     current_nz = -1;
	char    line[1024];

	MKL_INT nr_rows=0, nr_cols=0, nr_nnzs=0;
	while (true)
	{
		ifs.getline(line, 1024);
		if (!ifs.good())
		{
			// Done
			break;
		}
		if (line[0] == '%')
		{
			// Comment
			if (line[1] == '%')
			{
				// Banner
				symmetric   = (strstr(line, "symmetric") != NULL);
				skew        = (strstr(line, "skew") != NULL);
				array       = (strstr(line, "array") != NULL);
				// printf("(symmetric: %d, skew: %d, array: %d) ", symmetric, skew, array); fflush(stdout);
			}
		}
		else if (current_nz == -1)
		{
			// Problem description
			MKL_INT nparsed = sscanf(line, "%d %d %d", &nr_rows, &nr_cols, &nr_nnzs);
			coo->m = nr_rows;
			coo->n = nr_cols;
			coo->nnz = nr_nnzs;
			// std::cout << "coo->m " << coo->m << " coo->n " << coo->n << " coo->nnz " << coo->nnz << "\n";
			if ((!array) && (nparsed == 3))
			{
				if (symmetric)
					nr_nnzs *= 2;

				// Allocate coo matrix

				// coo->val = new ValueType[nr_nnzs];
				// coo->rowind = new int[nr_nnzs];
				// coo->colind = new int[nr_nnzs];

				coo->val = (ValueType *) mkl_malloc(nr_nnzs * sizeof(ValueType), 64);
				coo->rowind = (MKL_INT *) mkl_malloc(nr_nnzs * sizeof(MKL_INT), 64);
				coo->colind = (MKL_INT *) mkl_malloc(nr_nnzs * sizeof(MKL_INT), 64);

				current_nz = 0;
			}
			else
				error("Error parsing MARKET matrix: invalid problem description: %s\n", line);
		}
		else
		{
			// Edge
			if (current_nz >= nr_nnzs)
				error("Error parsing MARKET matrix: encountered more than %d nr_nnzs\n", nr_nnzs);

			MKL_INT row=1, col=1;
			ValueType val=1.0;

			if (!array)
			{
				// Parse nonzero (note: using strtol and strtod is 2x faster than sscanf or istream parsing)
				char *l = line;
				char *t = NULL;

				// parse row
				row = strtol(l, &t, 0);
				if (t == l)
					error("Error parsing MARKET matrix: badly formed row at edge %d\n", current_nz);
				l = t;

				// parse col
				col = strtol(l, &t, 0);
				if (t == l)
					error("Error parsing MARKET matrix: badly formed col at edge %d\n", current_nz);
				l = t;

				// parse val
				#if DOUBLE == 0
					val = strtof(l, &t);
				#elif DOUBLE == 1
					val = strtod(l, &t);
				#endif

				if (t == l)
				{
					val = 1.0;
				}

				coo->val[current_nz] = val;
				coo->rowind[current_nz] = row - 1;
				coo->colind[current_nz] = col-1;
				// if(current_nz<10)
				// std::cout << " molis diavasa to " << current_nz << " stoixeio " << coo->val[current_nz] << " me ta stoixeia : (" << coo->rowind[current_nz] << ","<< coo->colind[current_nz] <<")\n";
			}

			current_nz++;

			// Adjust nonzero count (nonzeros along the diagonal aren't reversed)
			if (symmetric && (row != col))
			{
				coo->val[current_nz] = val * (skew ? -1 : 1);
				coo->rowind[current_nz] = col - 1;
				coo->colind[current_nz] = row -1;
				current_nz++;
			}
		}
	}

	coo->nnz = current_nz;
	ifs.close();
}


//////////////////////////////////////////////////////////////////////////
// CSR format
//////////////////////////////////////////////////////////////////////////


struct CSRArrays
{
	MKL_INT m;      //< rows
	MKL_INT n;      //< columns
	MKL_INT nnz;    //< the number of nnz (== ia[m])
	ValueType *a;   //< the values (of size NNZ)
	MKL_INT *ia;    //< the usual rowptr (of size m+1)
	MKL_INT *ja;    //< the colidx of each NNZ (of size nnz)

	CSRArrays(){
		a = NULL;
		ia = NULL;
		ja= NULL;
	}

	~CSRArrays(){
		mkl_free(a);
		mkl_free(ia);
		mkl_free(ja);
	}
};


/** See https://software.intel.com/fr-fr/node/520849#449CA855-CE5B-4061-B003-70D078CA5E05 */
void COO_to_CSR(COOArrays * coo, CSRArrays * csr)
{
	MKL_INT job[6] = {1,//if job(1)=1, the matrix in the coordinate format is converted to the CSR format.
		0,//If job(2)=0, zero-based indexing for the matrix in CSR format is used;
		0,//If job(3)=0, zero-based indexing for the matrix in coordinate format is used;
		0,
		coo->nnz,//job(5)=nnz - sets number of the non-zero elements of the matrix A if job(1)=1.
		0 //If job(6)=0, all arrays acsr, ja, ia are filled in for the output storage.
	};
	// Init csr
	csr->m = coo->m;
	csr->n = coo->n;
	csr->nnz = coo->nnz;
	csr->a = (ValueType *) mkl_malloc((csr->nnz + VECTOR_ELEM_NUM) * sizeof(ValueType), 64);
	csr->ja = (MKL_INT *) mkl_malloc((csr->nnz + VECTOR_ELEM_NUM) * sizeof(MKL_INT), 64);
	csr->ia = (MKL_INT *) mkl_malloc((csr->m+1 + VECTOR_ELEM_NUM) * sizeof(MKL_INT), 64);
	MKL_INT nnz = coo->nnz;
	MKL_INT info;

	// const double mem_footprint = csr->nnz*(sizeof(ValueType)+sizeof(MKL_INT)) + (csr->m+1)*sizeof(MKL_INT);
	// std::cout << mem_footprint/(1024*1024) << "\n";
	// exit(0);

	#pragma omp parallel for
	for (int i=0;i<csr->nnz + VECTOR_ELEM_NUM;i++)
	{
		csr->a[i] = 0.0;
		csr->ja[i] = 0;
	}
	#pragma omp parallel for
	for (int i=0;i<csr->m+1 + VECTOR_ELEM_NUM;i++)
		csr->ia[i] = 0;

	#if DOUBLE == 0
		mkl_scsrcoo(job, &coo->m, csr->a, csr->ja, csr->ia, &nnz, coo->val, coo->rowind, coo->colind, &info);
	#elif DOUBLE == 1
		mkl_dcsrcoo(job, &coo->m, csr->a, csr->ja, csr->ia, &nnz, coo->val, coo->rowind, coo->colind, &info);
	#endif
}


//////////////////////////////////////////////////////////////////////////
// CSC format
//////////////////////////////////////////////////////////////////////////


struct CSCArrays
{
	MKL_INT m;      //< rows
	MKL_INT n;      //< columns
	MKL_INT nnz;    //< the number of nnz (== ia[m])
	ValueType *a;   //< the values (of size NNZ)
	MKL_INT *ia;    //< the usual colptr (of size m+1)
	MKL_INT *ja;    //< the rowidx of each NNZ (of size nnz)

	CSCArrays(){
		a = NULL;
		ia = NULL;
		ja= NULL;
	}

	~CSCArrays(){
		mkl_free(a);
		mkl_free(ia);
		mkl_free(ja);
	}
};


void CSR_to_CSC(CSRArrays * csr, CSCArrays* csc)
{
	MKL_INT job[6] = {0, //If job(1)=0, the matrix in the CSR format is converted to the CSC format;
		0, //If job(2)=0, zero-based indexing for the matrix in CSR format is used;
		0, //If job(3)=0, zero-based indexing for the matrix in CSC format is used;
		0,
		0,
		1  //If job(6)≠0, all output arrays acsc, ja1, and ia1 are filled in for the output storage.
	};
	// Init csc
	csc->m = csr->m;
	csc->n = csr->n;
	csc->nnz = csr->nnz;
	csc->a = (ValueType *) mkl_malloc(csc->nnz * sizeof(ValueType), 64);
	csc->ia = (MKL_INT *) mkl_malloc((csc->n+1) * sizeof(MKL_INT), 64);
	csc->ja = (MKL_INT *) mkl_malloc(csc->nnz * sizeof(MKL_INT), 64);
	MKL_INT info;

	#if DOUBLE == 0
		mkl_scsrcsc(job, &csr->m, csr->a, csr->ja, csr->ia, csc->a, csc->ja, csc->ia, &info);
	#elif DOUBLE == 1
		mkl_dcsrcsc(job, &csr->m, csr->a, csr->ja, csr->ia, csc->a, csc->ja, csc->ia, &info);
	#endif
}


//////////////////////////////////////////////////////////////////////////
// BCSR format
//////////////////////////////////////////////////////////////////////////


struct BCSRArrays
{
	MKL_INT m;
	MKL_INT n;
	MKL_INT nnz;
	MKL_INT nbBlocks;
	MKL_INT nbBlockRows;
	MKL_INT lb;              /*size of blocks*/
	MKL_INT ldabsr;          /*leading >= lb*lb*/
	ValueType *a;            /*values(m*lb*lb)*/
	MKL_INT *ia;             /*i(m+1)*/
	MKL_INT *ja;             /*j(m+1)*/
	MKL_INT allocatedBlocks;

	BCSRArrays(){
		a = NULL;
		ia = NULL;
		ja = NULL;
	}

	~BCSRArrays(){
		mkl_free(a);
		mkl_free(ia);
		mkl_free(ja);
	}
};


/** https://software.intel.com/fr-fr/node/520850#3A22B45C-4604-4444-B6FE-205A5CD4E667 */
void CSR_to_BCSR(CSRArrays * csr, BCSRArrays* bcsr, const int blockSize)
{
	MKL_INT job[6] = {0,//If job(1)=0, the matrix in the CSR format is converted to the BSR format;
		0,//If job(2)=0, zero-based indexing for the matrix in CSR format is used;
		0,//If job(3)=0, zero-based indexing for the matrix in the BSR format is used;
		0,
		0,
		1 //If job(6)>0, all output arrays absr, jab, and iab are filled in for the output storage.
	};
	bcsr->m = csr->m;
	bcsr->n = csr->n;
	bcsr->nnz = csr->nnz;
	bcsr->nbBlocks = 0;

	// We need to count the number of blocks (aligned!)
	{
		const MKL_INT maxBlockPerRow = (bcsr->n+blockSize-1)/blockSize;
		unsigned* usedBlocks = new unsigned[maxBlockPerRow];

		for(int idxRow = 0 ; idxRow < csr->m ; ++idxRow){
			if(idxRow%blockSize == 0){
				memset(usedBlocks, 0, sizeof(unsigned)*maxBlockPerRow);
			}
			for(int idxVal = csr->ia[idxRow] ; idxVal < csr->ia[idxRow+1] ; idxVal++){
				const int idxCol = csr->ja[idxVal];
				if(usedBlocks[idxCol/blockSize] == 0){
					usedBlocks[idxCol/blockSize] = 1;
					bcsr->nbBlocks += 1;
				}
			}
		}

		delete[] usedBlocks;
	}

	bcsr->nbBlockRows = (bcsr->m+blockSize-1)/blockSize;
	bcsr->lb = blockSize;
	bcsr->ldabsr = bcsr->lb*bcsr->lb;
	bcsr->a = (ValueType *) mkl_malloc(bcsr->nbBlocks*bcsr->lb*bcsr->lb * sizeof(ValueType), 64);
	bcsr->ia = (MKL_INT *) mkl_malloc((bcsr->nbBlockRows+1) * sizeof(MKL_INT), 64);
	bcsr->ja = (MKL_INT *) mkl_malloc(bcsr->nbBlocks * sizeof(MKL_INT), 64);

	// printf("%d %d\n", bcsr->nbBlocks*bcsr->lb*bcsr->lb, csr->nnz);

	// const double mem_footprint = bcsr->nbBlocks*blockSize*blockSize*sizeof(ValueType) + (bcsr->nbBlockRows+1)*sizeof(MKL_INT) + bcsr->nbBlocks*sizeof(MKL_INT);
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
	MKL_INT info;
	#if DOUBLE == 0
		mkl_scsrbsr(job, &csr->m, &bcsr->lb, &bcsr->ldabsr, csr->a, csr->ja, csr->ia, bcsr->a, bcsr->ja, bcsr->ia, &info);
	#elif DOUBLE == 1
		mkl_dcsrbsr(job, &csr->m, &bcsr->lb, &bcsr->ldabsr, csr->a, csr->ja, csr->ia, bcsr->a, bcsr->ja, bcsr->ia, &info);
	#endif
	// printf("lalala\n");
	assert(bcsr->ia[bcsr->nbBlockRows] == bcsr->nbBlocks);
}


//////////////////////////////////////////////////////////////////////////
// DIA format
//////////////////////////////////////////////////////////////////////////


struct DIAArrays
{
	MKL_INT m;         //< the dimension of the matrix
	MKL_INT n;         //< the dimension of the matrix
	MKL_INT nnz;       //< the number of nnz inside the matrix
	ValueType *val;    //< the NNZ values - may include zeros (of size lval*ndiag)*/
	MKL_INT *idiag;    //< distance from the diagonal (of size ndiag)
	MKL_INT lval;      //< leading where the diagonals are stored >= m,  which is the declared leading dimension in the calling (sub)programs
	MKL_INT ndiag;     //< number of diagonals that have at least one nnz

	DIAArrays(){
		val = NULL;
		idiag = NULL;
	}

	~DIAArrays(){
		mkl_free(val);
		mkl_free(idiag);
	}
};


/** https://software.intel.com/fr-fr/node/520852#00B3CA58-E0E4-4ED9-B42B-BC6338AB461D */
void CSR_to_DIA(CSRArrays * csr, DIAArrays* dia)
{
	MKL_INT job[6] = {0,//If job(1)=0, the matrix in the CSR format is converted to the diagonal format;
		0,//If job(2)=0, zero-based indexing for the matrix in CSR format is used;
		1,//if job(3)=1, one-based indexing for the matrix in the diagonal format is used.
		0,
		0,
		10//If job(6)=10, diagonals are selected internally, and acsr_rem, ja_rem, ia_rem are not filled in for the output storage.
	};
	dia->m = csr->m;
	dia->n = csr->n;
	dia->nnz = csr->nnz;
	dia->lval = dia->m;
	dia->ndiag = 0;
	// We need to count the number of diagonals with NNZ
	{
		unsigned* usedDiag = new unsigned[csr->m*2-1];
		memset(usedDiag, 0, sizeof(unsigned)*(csr->m*2-1));

		for(int idxRow = 0 ; idxRow < csr->m ; ++idxRow){
			for(int idxVal = csr->ia[idxRow] ; idxVal < csr->ia[idxRow+1] ; ++idxVal){
				const int idxCol = csr->ja[idxVal];
				const int diag = csr->m-idxRow+idxCol-1;
				assert(0 <= diag && diag < csr->m*2-1);
				if(usedDiag[diag] == 0){
					usedDiag[diag] = 1;
					dia->ndiag += 1;
				}
			}
		}

		delete[] usedDiag;
	}
	// Allocate the working arrays
	dia->val = (ValueType *) mkl_malloc(dia->ndiag*dia->lval * sizeof(ValueType), 64);
	dia->idiag = (MKL_INT *) mkl_malloc(dia->ndiag * sizeof(MKL_INT), 64);

	// const double mem_footprint = dia->ndiag*dia->lval*sizeof(ValueType) + dia->ndiag*sizeof(MKL_INT);
	// std::cout << mem_footprint/(1024*1024) << "\n";
	// exit(0);

	#pragma omp parallel for
	for (int i=0;i<dia->ndiag*dia->lval;i++)
		dia->val[i] = 0.0;
	#pragma omp parallel for
	for (int i=0;i<dia->ndiag;i++)
		dia->idiag[i] = 0;

	MKL_INT info;
	#if DOUBLE == 0
		mkl_scsrdia(job, &csr->m,csr->a, csr->ja, csr->ia, dia->val, &dia->lval, dia->idiag, &dia->ndiag, NULL, NULL, NULL, &info);
	#elif DOUBLE == 1
		mkl_dcsrdia(job, &csr->m,csr->a, csr->ja, csr->ia, dia->val, &dia->lval, dia->idiag, &dia->ndiag, NULL, NULL, NULL, &info);
	#endif
}


//////////////////////////////////////////////////////////////////////////
// LDU format
//////////////////////////////////////////////////////////////////////////


// Only for structurally symmetric matrices.
// Symmetrical elements will be in the same position in the 'upper' and 'lower' arrays.
// We also assume value symmetry for convenience in the conversion.
struct LDUArrays
{
	MKL_INT m;
	MKL_INT n;
	MKL_INT nnz;
	MKL_INT upper_n;
	ValueType * diag;
	ValueType * upper;       // upper diagonal in COO format
	ValueType * lower;       // lower diagonal in COO format
	MKL_INT * row_idx;       // row indexes for upper (column for the lower)
	MKL_INT * col_idx;       // column indexes for upper (row for the lower)

	LDUArrays(){
		diag = NULL;
		upper = NULL;
		row_idx = NULL;
		col_idx = NULL;
	}

	~LDUArrays(){
		free(diag);
		free(upper);
		free(lower);
		free(row_idx);
		free(col_idx);
	}
};


void CSR_to_LDU(CSRArrays * csr, LDUArrays * ldu)
{
	long num_threads = omp_get_max_threads();
	long t_upper_n[num_threads];
	ldu->m = csr->m;
	ldu->n = csr->n;
	ldu->upper_n = 0;
	ldu->diag = (typeof(ldu->diag)) malloc(ldu->m * sizeof(*ldu->diag));
	#pragma omp parallel
	{
		long tnum = omp_get_thread_num();
		long upper_base = 0;
		long i, j, i_s, i_e;

		loop_partitioner_balance_iterations(num_threads, tnum, 0, ldu->m, &i_s, &i_e);

		for (i=i_s;i<i_e;i++)
			for (j=csr->ia[i];j<csr->ia[i+1];j++)
				if (i < csr->ja[j])
					upper_base++;
		t_upper_n[tnum] = upper_base;

		#pragma omp barrier
		#pragma omp single
		{
			long tmp = 0;
			upper_base = 0;
			for (i=0;i<num_threads;i++)
			{
				tmp = t_upper_n[i];
				t_upper_n[i] = upper_base;
				upper_base += tmp;
			}
			ldu->upper_n = upper_base;
			ldu->nnz = ldu->m + 2 * upper_base;
			ldu->upper = (typeof(ldu->upper)) malloc(ldu->upper_n * sizeof(*ldu->upper));
			ldu->lower = (typeof(ldu->lower)) malloc(ldu->upper_n * sizeof(*ldu->lower));
			ldu->row_idx = (typeof(ldu->row_idx)) malloc(ldu->upper_n * sizeof(*ldu->row_idx));
			ldu->col_idx = (typeof(ldu->col_idx)) malloc(ldu->upper_n * sizeof(*ldu->col_idx));
		}
		#pragma omp barrier

		upper_base = t_upper_n[tnum];
		
		for (i=i_s;i<i_e;i++)
			for (j=csr->ia[i];j<csr->ia[i+1];j++)
			{
				if (i == csr->ja[j])
					ldu->diag[i] = csr->a[j];
				else if (i < csr->ja[j])
				{
					ldu->upper[upper_base] = csr->a[j];
					ldu->lower[upper_base] = csr->a[j];           // Symmetrical elements will be in the same position in the 'upper' and 'lower' arrays.
					ldu->row_idx[upper_base] = i;
					ldu->col_idx[upper_base] = csr->ja[j];
					upper_base++;
				}
			}
	}
}


//////////////////////////////////////////////////////////////////////////
// ELLPACK format
//////////////////////////////////////////////////////////////////////////


struct ELLArrays
{
	MKL_INT m;      //< rows
	MKL_INT n;      //< columns
	MKL_INT width;  //< max nnz per row
	MKL_INT nnz;    //< the number of nnz (== ia[m])
	ValueType *a;   //< the values (of size NNZ)
	MKL_INT *ja;    //< the colidx of each NNZ (of size nnz)

	ELLArrays(){
		a = NULL;
		ja= NULL;
	}

	~ELLArrays(){
		mkl_free(a);
		mkl_free(ja);
	}
};


void CSR_to_ELL(CSRArrays * csr, ELLArrays * ell)
{
	long i, j, j_s, j_e;
	long degree;
	long max_nnz_per_row;

	// ell->m = csr->m;
	ell->m = ((csr->m + VECTOR_ELEM_NUM - 1) / VECTOR_ELEM_NUM) * VECTOR_ELEM_NUM;

	ell->n = csr->n;
	ell->nnz = csr->nnz;

	max_nnz_per_row = 0;
	for (i=0;i<ell->m;i++)
	{
		degree = csr->ia[i+1] - csr->ia[i];
		if (degree > max_nnz_per_row)
			max_nnz_per_row = degree;
	}
	printf("max degree = %ld\n", max_nnz_per_row);

	ell->width = max_nnz_per_row;
	// ell->width = ((max_nnz_per_row + VECTOR_ELEM_NUM - 1) / VECTOR_ELEM_NUM) * VECTOR_ELEM_NUM;

	printf("width = %d\n", ell->width);

	ell->a = (ValueType *) mkl_malloc(ell->m * ell->width * sizeof(ValueType), 64);
	ell->ja = (MKL_INT *) mkl_malloc(ell->m * ell->width * sizeof(MKL_INT), 64);
	#pragma omp parallel
	{
		long i, j, k;
		#pragma omp for
		for (i=0;i<csr->m;i++)
		{
			k = i * ell->width;
			for (j=csr->ia[i];j<csr->ia[i+1];j++,k++)
			{
				ell->a[k] = csr->a[j];
				ell->ja[k] = csr->ja[j];
			}
			for (;k<(i+1)*ell->width;k++)
			{
				ell->a[k] = 0;
				ell->ja[k] = 0;
			}
		}
	}
	for (i=csr->m;i<ell->m;i++)
	{
		j_s = i * ell->width;
		j_e = (i + 1) * ell->width;
		for (j=j_s;j<j_e;j++)
		{
			ell->a[j] = 0;
			ell->ja[j] = 0;
		}
	}

	// ValueType * a = transpose<ValueType>(ell->a, ell->m, ell->width);
	// MKL_INT * ja = transpose<MKL_INT>(ell->ja, ell->m, ell->width);
	// mkl_free(ell->a);
	// mkl_free(ell->ja);
	// ell->a = a;
	// ell->ja = ja;

}


//////////////////////////////////////////////////////////////////////////
// Utils
//////////////////////////////////////////////////////////////////////////


/** Simply return the max relative diff */
void
CheckAccuracy(COOArrays * coo, ValueType * x, ValueType * y)
{
	#if DOUBLE == 0
		ValueType epsilon = 1e-5;
	#elif DOUBLE == 1
		ValueType epsilon = 1e-8;
	#endif
	int i, j;

	// ValueType val, tmp;
	// ValueType* kahan = new ValueType[coo->m * sizeof(*kahan)];
	ValueType* y_gold = new ValueType[coo->m * sizeof(*y_gold)];
	for(i=0;i<coo->m;i++)
	{
		// kahan[i] = 0;
		y_gold[i] = 0;
	}

	for (MKL_INT curr_nnz = 0; curr_nnz < coo->nnz; ++curr_nnz)
	{
		i = coo->rowind[curr_nnz];
		j = coo->colind[curr_nnz];

		y_gold[i] += x[j] * coo->val[curr_nnz];

		// val = x[j] * coo->val[curr_nnz] - kahan[i];
		// tmp = y_gold[i] + val;
		// kahan[i] = (tmp - y_gold[i]) - val;
		// y_gold[i] = tmp;

	}

	ValueType maxDiff = 0;
	// int cnt=0;
	for(int idx = 0 ; idx < coo->m ; idx++) {

		maxDiff = Max(maxDiff, Abs(y_gold[idx]-y[idx]));
		// std::cout << idx << ": " << y_gold[idx]-y[idx] << "\n";
		if (y_gold[idx] != 0.0) {
			maxDiff = Max(maxDiff, Abs((y_gold[idx]-y[idx])/y_gold[idx]));
			// maxDiff = Max(maxDiff, Abs(y_gold[idx]-y[idx]));
		}

		// if(maxDiff>epsilon)
		// {
		// cnt++;
		// if(cnt<10)
		// std::cout << "maxDiff = " << maxDiff << " at " << idx << " : " << y_gold[idx] << " vs " << y[idx] << "\n";
		// }
		// if(y_gold[idx] != 0.0){
		// maxDiff = Max(maxDiff, Abs((y_gold[idx]-y[idx])/y_gold[idx]));
		// cnt++;
		// if(cnt<10)
		// std::cout << "maxDiff = " << maxDiff << " vs " << Abs((y_gold[idx]-y[idx])/y_gold[idx]) << "\n";
		// }
	}
	// std::cout << "\n";
	if(maxDiff>epsilon)
		std::cout << "Test failed! (" << maxDiff << ")\n";
	delete[] y_gold;
}


#endif /* UTIL_H */

