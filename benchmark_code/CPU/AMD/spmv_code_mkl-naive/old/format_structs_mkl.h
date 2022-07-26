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

#include <iostream>


#include "common.h"

#ifdef __cplusplus
extern "C"{
#endif

#include "debug.h"
#include "io.h"
#include "macros/cpp_defines.h"
#include "parallel_util.h"
#include "artificial_matrix_generation.h"

#include "aux/csr_converter.h"

#ifdef __cplusplus
}
#endif


#ifndef INT_T
	#define INT_T  MKL_INT
	// #define INT_T  int32_t
#endif

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


typedef ValueType  Vector2_Value_t  __attribute__((vector_size(16), aligned(1)));

typedef ValueType  Vector4_Value_t  __attribute__((vector_size(32), aligned(1)));
// typedef ValueType  Vector4_Value_t  __attribute__((vector_size(32)));

// Number of elements for the vectorization function.
#define VECTOR_ELEM_NUM  ((int) (sizeof(Vector4_Value_t) / sizeof(ValueType)))

// typedef INT_T  Vector_Index_t  __attribute__((vector_size(VECTOR_ELEM_NUM*sizeof(INT_T)), aligned(1)));
typedef INT_T  Vector_Index_t  __attribute__((vector_size(VECTOR_ELEM_NUM*sizeof(INT_T))));


//////////////////////////////////////////////////////////////////////////
// COO format
//////////////////////////////////////////////////////////////////////////


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
	INT_T     current_nz = -1;
	char    line[1024];

	INT_T nr_rows=0, nr_cols=0, nr_nnzs=0;
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
			INT_T nparsed = sscanf(line, "%d %d %d", &nr_rows, &nr_cols, &nr_nnzs);
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

				coo->val = (ValueType *) aligned_alloc(64, nr_nnzs * sizeof(ValueType));
				coo->rowind = (INT_T *) aligned_alloc(64, nr_nnzs * sizeof(INT_T));
				coo->colind = (INT_T *) aligned_alloc(64, nr_nnzs * sizeof(INT_T));

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

			INT_T row=1, col=1;
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


/** See https://software.intel.com/fr-fr/node/520849#449CA855-CE5B-4061-B003-70D078CA5E05 */
void COO_to_CSR(COOArrays * coo, CSRArrays * csr)
{
	csr->m = coo->m;
	csr->n = coo->n;
	csr->nnz = coo->nnz;
	csr->a = (ValueType *) aligned_alloc(128, (csr->nnz + VECTOR_ELEM_NUM) * sizeof(ValueType));
	csr->ja = (INT_T *) aligned_alloc(128, (csr->nnz + VECTOR_ELEM_NUM) * sizeof(INT_T));
	csr->ia = (INT_T *) aligned_alloc(128, (csr->m+1 + VECTOR_ELEM_NUM) * sizeof(INT_T));

	// const double mem_footprint = csr->nnz*(sizeof(ValueType)+sizeof(INT_T)) + (csr->m+1)*sizeof(INT_T);
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

	coo_to_csr(coo->rowind, coo->colind, coo->val, coo->m, coo->n, coo->nnz, csr->ia, csr->ja, csr->a, 1);

	// INT_T job[6] = {1,//if job(1)=1, the matrix in the coordinate format is converted to the CSR format.
		// 0,//If job(2)=0, zero-based indexing for the matrix in CSR format is used;
		// 0,//If job(3)=0, zero-based indexing for the matrix in coordinate format is used;
		// 0,
		// coo->nnz,//job(5)=nnz - sets number of the non-zero elements of the matrix A if job(1)=1.
		// 0 //If job(6)=0, all arrays acsr, ja, ia are filled in for the output storage.
	// };
	// INT_T nnz = coo->nnz;
	// INT_T info;
	// #if DOUBLE == 0
		// mkl_scsrcoo(job, &coo->m, csr->a, csr->ja, csr->ia, &nnz, coo->val, coo->rowind, coo->colind, &info);
	// #elif DOUBLE == 1
		// mkl_dcsrcoo(job, &coo->m, csr->a, csr->ja, csr->ia, &nnz, coo->val, coo->rowind, coo->colind, &info);
	// #endif

	/* sparse_matrix_t A;
	mkl_sparse_d_create_csr(&A, SPARSE_INDEX_BASE_ZERO, csr->m, csr->n, csr->ia, csr->ia+1, csr->ja, csr->a);
	mkl_sparse_order(A);
	CSRArrays * csr2 = (typeof(csr2)) malloc(sizeof(*csr2));
	csr2->m = coo->m;
	csr2->n = coo->n;
	csr2->nnz = coo->nnz;
	csr2->a = (ValueType *) mkl_malloc((csr2->nnz + VECTOR_ELEM_NUM) * sizeof(ValueType), 64);
	csr2->ja = (INT_T *) mkl_malloc((csr2->nnz + VECTOR_ELEM_NUM) * sizeof(INT_T), 64);
	csr2->ia = (INT_T *) mkl_malloc((csr2->m+1 + VECTOR_ELEM_NUM) * sizeof(INT_T), 64);
	coo_to_csr_fully_sorted(coo->rowind, coo->colind, coo->val, coo->m, coo->n, coo->nnz, csr2->ia, csr2->ja, csr2->a);
	for (int i=0;i<csr->m+1 + VECTOR_ELEM_NUM;i++)
	{
		if (csr->ia[i] != csr2->ia[i])
		{
			printf("%d: different ia: %d %d\n", i, csr->ia[i], csr2->ia[i]);
			exit(1);
		}
	}
	for (int i=0;i<csr->m+1 + VECTOR_ELEM_NUM;i++)
	{
		for (int j=csr->ia[i];j<csr->ia[i+1];j++)
		{
			// if (csr->a[i] != csr2->a[i])
			// {
				// printf("%d,%d: different a: %lf %lf\n", i, j, csr->a[j], csr2->a[j]);
				// exit(1);
			// }
			if (csr->ja[j] != csr2->ja[j])
			{
				printf("%d,%d: different ja: %d %d\n", i, j, csr->ja[j], csr2->ja[j]);
				for (int j=csr->ia[i];j<csr->ia[i+1];j++)
					printf("%d ", csr->ja[j]);
				printf("\n");
				for (int j=csr->ia[i];j<csr->ia[i+1];j++)
					printf("%d ", csr2->ja[j]);
				printf("\n\n");
				exit(1);
			}
		}
	} */

}


//////////////////////////////////////////////////////////////////////////
// CSC format
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
// BCSR format
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
// DIA format
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
// LDU format
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
// ELLPACK format
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
// ELLPACK Symmetric format
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
// Utils
//////////////////////////////////////////////////////////////////////////


/** Simply return the max relative diff */
void
CheckAccuracy(COOArrays * coo, ValueType * x, ValueType * y)
{
	#if DOUBLE == 0
		// ValueType epsilon = 1e-5;
		ValueType epsilon = 1e-7;
	#elif DOUBLE == 1
		// ValueType epsilon = 1e-8;
		ValueType epsilon = 1e-10;
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

	for (INT_T curr_nnz = 0; curr_nnz < coo->nnz; ++curr_nnz)
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
			// if (Abs((y_gold[idx]-y[idx])/y_gold[idx]) > epsilon)
				// printf("Error: %g != %g , diff=%g , diff_frac=%g\n", y_gold[idx], y[idx], Abs(y_gold[idx]-y[idx]), Abs((y_gold[idx]-y[idx])/y_gold[idx]));
			// maxDiff = Max(maxDiff, Abs((y_gold[idx]-y[idx])/y_gold[idx]));
			maxDiff = Max(maxDiff, Abs(y_gold[idx]-y[idx]));
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

