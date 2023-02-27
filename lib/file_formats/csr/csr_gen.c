#include <stdlib.h>
#include <stdio.h>
#include <omp.h>
#include <math.h>

#include "macros/macrolib.h"
#include "time_it.h"
#include "parallel_util.h"
#include "omp_functions.h"

#include "csr_gen.h"


//==========================================================================================================================================
//= User Functions Declarations
//==========================================================================================================================================


//==========================================================================================================================================
//= Includes
//==========================================================================================================================================


// Quicksort

#include "sort/quicksort_gen_undef.h"
#define QUICKSORT_GEN_TYPE_1  CSR_GEN_TYPE_2
#define QUICKSORT_GEN_TYPE_2  CSR_GEN_TYPE_2
#define QUICKSORT_GEN_TYPE_3  CSR_GEN_TYPE_2
#define QUICKSORT_GEN_SUFFIX  CONCAT(_QS_CSR_GEN, CSR_GEN_SUFFIX)
#include "sort/quicksort_gen.c"

static inline
int
quicksort_cmp(CSR_GEN_TYPE_2 a, CSR_GEN_TYPE_2 b, CSR_GEN_TYPE_2 * sorting_keys)
{
	return (sorting_keys[a] > sorting_keys[b]) ? 1 : (sorting_keys[a] < sorting_keys[b]) ? -1 : 0;
}


// Bucketsort

#include "sort/bucketsort_gen_undef.h"
#define BUCKETSORT_GEN_TYPE_1  CSR_GEN_TYPE_2
#define BUCKETSORT_GEN_TYPE_2  CSR_GEN_TYPE_2
#define BUCKETSORT_GEN_TYPE_3  CSR_GEN_TYPE_2
#define BUCKETSORT_GEN_TYPE_4  void
#define BUCKETSORT_GEN_SUFFIX  CONCAT(_BS_CSR_GEN, CSR_GEN_SUFFIX)
#include "sort/bucketsort_gen.c"

static inline
CSR_GEN_TYPE_2
bucketsort_find_bucket(CSR_GEN_TYPE_2 a, __attribute__((unused)) void * unused)
{
	return a;
}


// Samplesort

#include "sort/samplesort_gen_undef.h"
#define SAMPLESORT_GEN_TYPE_1  CSR_GEN_TYPE_2
#define SAMPLESORT_GEN_TYPE_2  CSR_GEN_TYPE_2
#define SAMPLESORT_GEN_TYPE_3  CSR_GEN_TYPE_2
#define SAMPLESORT_GEN_TYPE_4  CSR_GEN_TYPE_2 *
#define SAMPLESORT_GEN_SUFFIX  CONCAT(_SS_CSR_GEN, CSR_GEN_SUFFIX)
#include "sort/samplesort_gen.c"

static inline
int
samplesort_cmp(CSR_GEN_TYPE_2 a, CSR_GEN_TYPE_2 b, CSR_GEN_TYPE_2 ** sorting_keys)
{
	CSR_GEN_TYPE_2 * keys, * subkeys;
	keys = sorting_keys[0];
	subkeys = sorting_keys[1];
	return (keys[a] > keys[b]) ? 1 : (keys[a] < keys[b]) ? -1
		: (subkeys[a] > subkeys[b]) ? 1 : (subkeys[a] < subkeys[b]) ? -1 : 0;
}


// Scan

#include "functools_gen_undef.h"
#define FUNCTOOLS_GEN_TYPE_1  CSR_GEN_TYPE_2
#define FUNCTOOLS_GEN_TYPE_2  CSR_GEN_TYPE_2
#define FUNCTOOLS_GEN_SUFFIX  CONCAT(_FT_i_CSR_GEN, CSR_GEN_SUFFIX)
#include "functools_gen.c"

static inline
CSR_GEN_TYPE_2
functools_map_fun(CSR_GEN_TYPE_2 * A, long i)
{
	return A[i];
}

static inline
CSR_GEN_TYPE_2
functools_reduce_fun(CSR_GEN_TYPE_2 a, CSR_GEN_TYPE_2 b)
{
	return a + b;
}


//==========================================================================================================================================
//= Local Defines
//==========================================================================================================================================


#undef  _TYPE_V
#define _TYPE_V  CSR_GEN_EXPAND(_TYPE_V)
typedef CSR_GEN_TYPE_1  _TYPE_V;

#undef  _TYPE_I
#define _TYPE_I  CSR_GEN_EXPAND(_TYPE_I)
typedef CSR_GEN_TYPE_2  _TYPE_I;


#undef  insertionsort
#define insertionsort  CONCAT(insertionsort_QS_CSR_GEN, CSR_GEN_SUFFIX)
#undef  quicksort
#define quicksort  CONCAT(quicksort_QS_CSR_GEN, CSR_GEN_SUFFIX)
#undef  quicksort_no_malloc
#define quicksort_no_malloc  CONCAT(quicksort_no_malloc_QS_CSR_GEN, CSR_GEN_SUFFIX)


//==========================================================================================================================================
//------------------------------------------------------------------------------------------------------------------------------------------
//-                                                              Templates                                                                 -
//------------------------------------------------------------------------------------------------------------------------------------------
//==========================================================================================================================================


#undef  csr_sort_columns
#define csr_sort_columns  CSR_GEN_EXPAND(csr_sort_columns)
void
csr_sort_columns(_TYPE_I * row_ptr, _TYPE_I * col_idx, _TYPE_V * val, long m, long n, long nnz)
{
	int num_threads = safe_omp_get_num_threads_external();
	_TYPE_I * permutation, * C;
	_TYPE_V * V;
	long thread_i_s[num_threads];
	long thread_i_e[num_threads];

	permutation = (typeof(permutation)) malloc(nnz * sizeof(*permutation));
	C = (typeof(C)) malloc(nnz * sizeof(*C));
	V = (val != NULL) ? (typeof(V)) malloc(nnz * sizeof(*V)) : NULL;
	#pragma omp parallel
	{
		int tnum = omp_get_thread_num();
		long i, i_s, i_e, j;
		long degree;
		_TYPE_I * buf_permutation  = (typeof(buf_permutation)) malloc(n * sizeof(*buf_permutation));
		_TYPE_I * buf_offsets      = (typeof(buf_offsets)) malloc((n+1)*sizeof(*buf_offsets));
		_TYPE_I * buf_bucket_ids   = (typeof(buf_bucket_ids)) malloc((n+1)*sizeof(*buf_offsets));
		_TYPE_I * qsort_partitions = (typeof(qsort_partitions)) malloc(m * sizeof(*qsort_partitions));

		loop_partitioner_balance_partial_sums(num_threads, tnum, row_ptr, m, nnz, &thread_i_s[tnum], &thread_i_e[tnum]);
		i_s = thread_i_s[tnum];
		i_e = thread_i_e[tnum];

		_TYPE_I pos;
		#pragma omp for
		for (i=0;i<nnz;i++)
		{
			permutation[i] = i;
			C[i] = col_idx[i];
			if (val != NULL)
				V[i] = val[i];
		}

		for (i=i_s;i<i_e;i++)
		{
			degree = row_ptr[i+1] - row_ptr[i];
			if (degree == 0)
				continue;

			if (degree > n/5)
			{
				bucketsort_stable_serial(&col_idx[row_ptr[i]], degree, n, NULL, buf_permutation, buf_offsets, buf_bucket_ids);
				for (j=row_ptr[i];j<row_ptr[i+1];j++)
				{
					pos = row_ptr[i] + buf_permutation[j - row_ptr[i]];
					col_idx[pos] = C[j];
					if (val != NULL)
						val[pos] = V[j];
				}
			}
			else
			{
				// quicksort(&permutation[row_ptr[i]], degree, col_idx);
				quicksort_no_malloc(&permutation[row_ptr[i]], degree, col_idx, qsort_partitions);
				for (j=row_ptr[i];j<row_ptr[i+1];j++)
				{
					pos = permutation[j];
					col_idx[j] = C[pos];
					if (val != NULL)
						val[j] = V[pos];
				}
			}
		}

		free(buf_permutation);
		free(buf_offsets);
		free(buf_bucket_ids);
		free(qsort_partitions);
	}
	free(permutation);
	free(C);
	free(V);
}


// An implementation from scratch (without bucketsort ...) doesn't seem to be any faster.
#undef  coo_to_csr
#define coo_to_csr  CSR_GEN_EXPAND(coo_to_csr)
void
coo_to_csr(_TYPE_I * R, _TYPE_I * C, _TYPE_V * V, long m, long n, long nnz, _TYPE_I * row_ptr, _TYPE_I * col_idx, _TYPE_V * val, int sort_columns)
{
	_TYPE_I * permutation = (typeof(permutation)) malloc(nnz * sizeof(*permutation));

	// bucketsort_stable_serial(R, nnz, m, NULL, permutation, row_ptr, NULL);
	bucketsort(R, nnz, m, NULL, permutation, row_ptr, NULL);

	// _TYPE_I * permutation_base = (typeof(permutation_base)) malloc(nnz * sizeof(*permutation_base));
	// bucketsort(C, nnz, n, NULL, permutation_base, NULL, NULL);
	// _TYPE_I * r_buf = (typeof(r_buf)) malloc(nnz * sizeof(*r_buf));
	// #pragma omp parallel
	// {
		// long i;
		// _TYPE_I pos;
		// #pragma omp for
		// for (i=0;i<nnz;i++)
		// {
			// pos = permutation_base[i];
			// r_buf[pos] = R[i];
		// }
	// }
	// bucketsort_stable(r_buf, nnz, m, NULL, permutation, row_ptr, NULL);

	#pragma omp parallel
	{
		long i;
		_TYPE_I pos;
		#pragma omp for
		for (i=0;i<nnz;i++)
		{
			pos = permutation[i];
			// pos = permutation[permutation_base[i]];
			col_idx[pos] = C[i];
			if (V != NULL)
				val[pos] = V[i];
		}
	}
	free(permutation);

	// free(r_buf);
	// free(permutation_base);

	if (sort_columns)
		csr_sort_columns(row_ptr, col_idx, val, m, n, nnz);

}


#undef  coo_to_csr_fully_sorted
#define coo_to_csr_fully_sorted  CSR_GEN_EXPAND(coo_to_csr_fully_sorted)
void
coo_to_csr_fully_sorted(_TYPE_I * R, _TYPE_I * C, _TYPE_V * V, long m, __attribute__((unused)) long n, long nnz, _TYPE_I * row_ptr, _TYPE_I * col_idx, _TYPE_V * val)
{
	_TYPE_I * offsets;
	_TYPE_I * permutation;
	permutation = (typeof(permutation)) malloc(nnz * sizeof(*permutation));
	#pragma omp parallel
	{
		long i;
		#pragma omp for
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
		_TYPE_I pos;
		#pragma omp for
		for (i=0;i<m;i++)
			offsets[i] = 0;
		#pragma omp for
		for (i=0;i<nnz;i++)
			__atomic_fetch_add(&offsets[R[i]], 1, __ATOMIC_RELAXED);
		scan_reduce_parallel(offsets, row_ptr, m+1, 0, 1, 0);
		#pragma omp for
		for (i=0;i<nnz;i++)
		{
			pos = permutation[i];
			col_idx[i] = C[pos];
			if (V != NULL)
				val[i] = V[pos];
		}
	}
	free(offsets);
	free(permutation);
}


//==========================================================================================================================================
//= Includes Undefs
//==========================================================================================================================================


#include "sort/bucketsort_gen_undef.h"
#include "sort/quicksort_gen_undef.h"
#include "sort/samplesort_gen_undef.h"
#include "functools_gen_undef.h"

