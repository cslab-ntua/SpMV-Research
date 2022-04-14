#ifndef CSR_CONVERTER_H
#define CSR_CONVERTER_H

#include "parallel_util.h"
// #include "genlib.h"


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//------------------------------------------------------------------------------------------------------------------------------------------
//-                                                              Templates                                                                 -
//------------------------------------------------------------------------------------------------------------------------------------------
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


//==========================================================================================================================================
//= Includes
//==========================================================================================================================================


// Samplesort

#undef  SAMPLESORT_GEN_TYPE_1
#undef  SAMPLESORT_GEN_TYPE_2
#undef  SAMPLESORT_GEN_TYPE_3
#undef  SAMPLESORT_GEN_TYPE_4
#undef  SAMPLESORT_GEN_SUFFIX
#define SAMPLESORT_GEN_TYPE_1  int
#define SAMPLESORT_GEN_TYPE_2  int
#define SAMPLESORT_GEN_TYPE_3  int
#define SAMPLESORT_GEN_TYPE_4  int *
#define SAMPLESORT_GEN_SUFFIX  CSR_CONVERTER_H_SS
#include "sort/samplesort_gen.c"

static inline
int
cmp(int a, int b, int ** sorting_keys)
{
	int * keys, * subkeys;
	keys = sorting_keys[0];
	subkeys = sorting_keys[1];
	return (keys[a] > keys[b]) ? 1 : (keys[a] < keys[b]) ? -1
		: (subkeys[a] > subkeys[b]) ? 1 : (subkeys[a] < subkeys[b]) ? -1 : 0;
}


// Scan

#undef  FUNCTOOLS_GEN_TYPE_1
#undef  FUNCTOOLS_GEN_SUFFIX
#define FUNCTOOLS_GEN_TYPE_1  int
#define FUNCTOOLS_GEN_SUFFIX  CSR_CONVERTER_H_FT
#include "functools_gen.c"

static inline
void
reduce_fun(int * partial, int * x)
{
	*partial += *x;
}

static inline
void
set_value(int * x, int val)
{
	*x = val;
}


//==========================================================================================================================================
//= Local Defines
//==========================================================================================================================================


#undef  _TYPE_V
#define _TYPE_V  ValueType

#undef  _TYPE_I
#define _TYPE_I  int


//==========================================================================================================================================
//= User Functions Declarations
//==========================================================================================================================================


//==========================================================================================================================================
//= Generic Code
//==========================================================================================================================================


void
coo_to_csr_fully_sorted(_TYPE_I * R, _TYPE_I * C, _TYPE_V * V, long m, long n, long nnz, CSRArrays * csr)
{
	_TYPE_I * offsets;
	_TYPE_I * permutation;
	long i;

	csr->m = m;
	csr->n = n;
	csr->nnz = nnz;
	// csr->ja = (typeof(csr->ja)) malloc(nnz * sizeof(*csr->ja));
	// csr->a = (V != NULL) ? (typeof(csr->a)) malloc(nnz * sizeof(*csr->a)) : NULL;


	permutation = (typeof(permutation)) malloc(nnz * sizeof(*permutation));
	#pragma omp parallel
	{
		long i;
		#pragma omp for schedule(static)
		for (i=0;i<nnz;i++)
		{
			permutation[i] = i;
		}
	}

	int * data[2];
	data[0] = R;
	data[1] = C;
	samplesort(permutation, nnz, data);

	offsets = (typeof(offsets)) malloc((m+1) * sizeof(*offsets));
	#pragma omp parallel
	{
		long i;
		#pragma omp for schedule(static)
		for (i=0;i<m;i++)
			offsets[i] = 0;
		#pragma omp for schedule(static)
		for (i=0;i<nnz;i++)
			__atomic_fetch_add(&offsets[R[i]], 1, __ATOMIC_RELAXED);
	}
	scan(offsets, offsets, m+1, 0, 1);

	for (i=0;i<m+1;i++)
		csr->ia[i] = offsets[i];

	#pragma omp parallel
	{
		long i;
		_TYPE_I pos;
		#pragma omp for schedule(static)
		for (i=0;i<nnz;i++)
		{
			pos = permutation[i];
			csr->ja[i] = C[pos];
			if (V != NULL)
				csr->a[i] = V[pos];
		}
	}

	free(offsets);
	free(permutation);
}


#endif /* CSR_CONVERTER_H */

