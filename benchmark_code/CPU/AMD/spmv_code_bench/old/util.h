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


#ifdef __cplusplus
extern "C"{
#endif

#include "debug.h"
#include "io.h"
#include "macros/cpp_defines.h"
#include "parallel_util.h"

#include "aux/csr_converter.h"

#ifdef __cplusplus
}
#endif


void COO_to_CSR(ValueType * V, INT_T * R, INT_T * C, INT_T * m_ptr, INT_T * n_ptr, INT_T * nnz_ptr)
		COOArrays * coo, CSRArrays * csr)
{
	csr->m = coo->m;
	csr->n = coo->n;
	csr->nnz = coo->nnz;
	csr->a = (ValueType *) aligned_alloc(64, (csr->nnz + VECTOR_ELEM_NUM) * sizeof(ValueType));
	csr->ja = (INT_T *) aligned_alloc(64, (csr->nnz + VECTOR_ELEM_NUM) * sizeof(INT_T));
	csr->ia = (INT_T *) aligned_alloc(64, (csr->m+1 + VECTOR_ELEM_NUM) * sizeof(INT_T));

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
}


#endif /* UTIL_H */

