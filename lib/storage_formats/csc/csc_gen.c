#include <stdlib.h>
#include <stdio.h>
#include <omp.h>
#include <math.h>

#include "macros/macrolib.h"
#include "debug.h"
#include "genlib.h"
#include "time_it.h"
#include "parallel_util.h"
#include "omp_functions.h"

#include "csc_gen.h"


//==========================================================================================================================================
//= User Functions Declarations
//==========================================================================================================================================


//==========================================================================================================================================
//= Includes
//==========================================================================================================================================


// Quicksort

#include "sort/quicksort/quicksort_gen_push.h"
#define QUICKSORT_GEN_TYPE_1  CSC_GEN_TYPE_2
#define QUICKSORT_GEN_TYPE_2  CSC_GEN_TYPE_2
#define QUICKSORT_GEN_TYPE_3  CSC_GEN_TYPE_2
#define QUICKSORT_GEN_SUFFIX  CONCAT(_QS_CSC_GEN, CSC_GEN_SUFFIX)
#include "sort/quicksort/quicksort_gen.c"

static inline
int
quicksort_cmp(CSC_GEN_TYPE_2 a, CSC_GEN_TYPE_2 b, CSC_GEN_TYPE_2 * sorting_keys)
{
	return (sorting_keys[a] > sorting_keys[b]) ? 1 : (sorting_keys[a] < sorting_keys[b]) ? -1 : 0;
}


// Bucketsort

#include "sort/bucketsort/bucketsort_gen_push.h"
#define BUCKETSORT_GEN_TYPE_1  CSC_GEN_TYPE_2
#define BUCKETSORT_GEN_TYPE_2  CSC_GEN_TYPE_2
#define BUCKETSORT_GEN_TYPE_3  CSC_GEN_TYPE_2
#define BUCKETSORT_GEN_TYPE_4  void
#define BUCKETSORT_GEN_SUFFIX  CONCAT(_BS_CSC_GEN, CSC_GEN_SUFFIX)
#include "sort/bucketsort/bucketsort_gen.c"

static inline
CSC_GEN_TYPE_2
bucketsort_find_bucket(CSC_GEN_TYPE_2 * A, long i, __attribute__((unused)) void * unused)
{
	return A[i];
}


//==========================================================================================================================================
//= Local Defines
//==========================================================================================================================================


#undef  _TYPE_V
#define _TYPE_V  CSC_GEN_EXPAND_TYPE(_TYPE_V)
typedef CSC_GEN_TYPE_1  _TYPE_V;

#undef  _TYPE_I
#define _TYPE_I  CSC_GEN_EXPAND_TYPE(_TYPE_I)
typedef CSC_GEN_TYPE_2  _TYPE_I;


//==========================================================================================================================================
//------------------------------------------------------------------------------------------------------------------------------------------
//-                                                              Templates                                                                 -
//------------------------------------------------------------------------------------------------------------------------------------------
//==========================================================================================================================================


#undef  csc_sort_rows
#define csc_sort_rows  CSC_GEN_EXPAND(csc_sort_rows)
void
csc_sort_rows(_TYPE_I * row_idx, _TYPE_I * col_ptr, _TYPE_V * values, long m, long n, long nnz)
{
	int num_threads = safe_omp_get_num_threads_external();
	_TYPE_I * permutation, * R;
	_TYPE_V * V;
	long thread_i_s[num_threads];
	long thread_i_e[num_threads];

	permutation = (typeof(permutation)) malloc(nnz * sizeof(*permutation));
	R = (typeof(R)) malloc(nnz * sizeof(*R));
	V = (values != NULL) ? (typeof(V)) malloc(nnz * sizeof(*V)) : NULL;
	#pragma omp parallel
	{
		int tnum = omp_get_thread_num();
		long i, i_s, i_e, j;
		long degree;
		_TYPE_I * buf_permutation  = (typeof(buf_permutation)) malloc(m * sizeof(*buf_permutation));
		_TYPE_I * buf_offsets      = (typeof(buf_offsets)) malloc((m+1)*sizeof(*buf_offsets));
		_TYPE_I * buf_bucket_ids   = (typeof(buf_bucket_ids)) malloc((m+1)*sizeof(*buf_offsets));
		_TYPE_I * qsort_partitions = (typeof(qsort_partitions)) malloc(n * sizeof(*qsort_partitions));

		loop_partitioner_balance_prefix_sums(num_threads, tnum, col_ptr, n, nnz, &thread_i_s[tnum], &thread_i_e[tnum]);
		i_s = thread_i_s[tnum];
		i_e = thread_i_e[tnum];

		_TYPE_I pos;
		#pragma omp for
		for (i=0;i<nnz;i++)
		{
			permutation[i] = i;
			R[i] = row_idx[i];
			if (values != NULL)
				V[i] = values[i];
		}

		for (i=i_s;i<i_e;i++)
		{
			degree = col_ptr[i+1] - col_ptr[i];
			if (degree == 0)
				continue;

			if (degree > m/5)
			{
				bucketsort_stable_serial(&row_idx[col_ptr[i]], degree, m, NULL, buf_permutation, buf_offsets, buf_bucket_ids);
				for (j=col_ptr[i];j<col_ptr[i+1];j++)
				{
					pos = col_ptr[i] + buf_permutation[j - col_ptr[i]];
					row_idx[pos] = R[j];
					if (values != NULL)
						values[pos] = V[j];
				}
			}
			else
			{
				quicksort_no_malloc(&permutation[col_ptr[i]], degree, row_idx, qsort_partitions);
				for (j=col_ptr[i];j<col_ptr[i+1];j++)
				{
					pos = permutation[j];
					row_idx[j] = R[pos];
					if (values != NULL)
						values[j] = V[pos];
				}
			}
		}

		free(buf_permutation);
		free(buf_offsets);
		free(buf_bucket_ids);
		free(qsort_partitions);
	}
	free(permutation);
	free(R);
	free(V);
}


// An implementation from scratch (without bucketsort ...) doesn't seem to be any faster.
#undef  coo_to_csc
#define coo_to_csc  CSC_GEN_EXPAND(coo_to_csc)
void
coo_to_csc(_TYPE_I * R, _TYPE_I * C, _TYPE_V * V, long m, long n, long nnz, _TYPE_I * row_idx, _TYPE_I * col_ptr, _TYPE_V * values, int sort_rows)
{
	_TYPE_I * permutation = (typeof(permutation)) malloc(nnz * sizeof(*permutation));

	// bucketsort_stable_serial(R, nnz, m, NULL, permutation, row_ptr, NULL);
	bucketsort(C, nnz, n, NULL, permutation, col_ptr, NULL);

	#pragma omp parallel
	{
		long i;
		_TYPE_I pos;
		#pragma omp for
		for (i=0;i<nnz;i++)
		{
			pos = permutation[i];
			// pos = permutation[permutation_base[i]];
			row_idx[pos] = R[i];
			if (V != NULL)
				values[pos] = V[i];
		}
	}
	free(permutation);

	// free(r_buf);
	// free(permutation_base);

	if (sort_rows)
		csc_sort_rows(row_idx, col_ptr, values, m, n, nnz);

}


//==========================================================================================================================================
//= Includes Undefs
//==========================================================================================================================================


#include "sort/bucketsort/bucketsort_gen_pop.h"
#include "sort/quicksort/quicksort_gen_pop.h"

