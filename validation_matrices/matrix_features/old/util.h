#ifndef UTIL_H
#define UTIL_H

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


#ifdef __cplusplus
extern "C"{
#endif
	#include "macros/cpp_defines.h"
	#include "debug.h"
	#include "io.h"
	// #include "parallel_util.h"
#ifdef __cplusplus
}
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

// typedef int  Vector_Index_t  __attribute__((vector_size(VECTOR_ELEM_NUM*sizeof(int)), aligned(1)));
typedef int  Vector_Index_t  __attribute__((vector_size(VECTOR_ELEM_NUM*sizeof(int))));


template<typename T>
T *
transpose(T * a, int m, int n)
{
	T * t = (T *) malloc(m*n * sizeof(*t));
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
	int m;         //< rows
	int n;         //< columns
	int nnz;       //< the number of nnz inside the matrix
	ValueType *val;    //< the values (size = nnz)
	int *rowind;   //< the row indexes (size = nnz)
	int *colind;   //< the col indexes (size = nnz)

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
		free(val);
		free(rowind);
		free(colind);
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
	int     current_nz = -1;
	char    line[1024];

	int nr_rows=0, nr_cols=0, nr_nnzs=0;
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
			int nparsed = sscanf(line, "%d %d %d", &nr_rows, &nr_cols, &nr_nnzs);
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

				coo->val = (ValueType *) malloc(nr_nnzs * sizeof(ValueType));
				coo->rowind = (int *) malloc(nr_nnzs * sizeof(int));
				coo->colind = (int *) malloc(nr_nnzs * sizeof(int));

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

			int row=1, col=1;
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
	int m;      //< rows
	int n;      //< columns
	int nnz;    //< the number of nnz (== ia[m])
	ValueType *a;   //< the values (of size NNZ)
	int *ia;    //< the usual rowptr (of size m+1)
	int *ja;    //< the colidx of each NNZ (of size nnz)

	CSRArrays(){
		a = NULL;
		ia = NULL;
		ja= NULL;
	}

	~CSRArrays(){
		free(a);
		free(ia);
		free(ja);
	}
};


#include "csr_converter.h"


/** See https://software.intel.com/fr-fr/node/520849#449CA855-CE5B-4061-B003-70D078CA5E05 */
void COO_to_CSR(COOArrays * coo, CSRArrays * csr)
{
	// Init csr
	csr->m = coo->m;
	csr->n = coo->n;
	csr->nnz = coo->nnz;
	csr->a = (ValueType *) malloc((csr->nnz + VECTOR_ELEM_NUM) * sizeof(ValueType));
	csr->ja = (int *) malloc((csr->nnz + VECTOR_ELEM_NUM) * sizeof(int));
	csr->ia = (int *) malloc((csr->m+1 + VECTOR_ELEM_NUM) * sizeof(int));

	#pragma omp parallel for
	for (int i=0;i<csr->nnz + VECTOR_ELEM_NUM;i++)
	{
		csr->a[i] = 0.0;
		csr->ja[i] = 0;
	}
	#pragma omp parallel for
	for (int i=0;i<csr->m+1 + VECTOR_ELEM_NUM;i++)
		csr->ia[i] = 0;

	coo_to_csr_fully_sorted(coo->rowind, coo->colind, coo->val, coo->m, coo->n, coo->nnz, csr);
}


#endif /* UTIL_H */

