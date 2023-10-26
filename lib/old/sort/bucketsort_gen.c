#include <stdlib.h>
#include <stdio.h>

#include "bucketsort_gen.h"
// #include "parallel_util.h"


//==========================================================================================================================================
//= User Functions Declarations
//==========================================================================================================================================


#undef  bucketsort_find_bucket
#define bucketsort_find_bucket  BUCKETSORT_GEN_EXPAND(bucketsort_find_bucket)
static _TYPE_BUCKET_I bucketsort_find_bucket(_TYPE_V a, _TYPE_AD * aux_data);


//==========================================================================================================================================
//= Includes
//==========================================================================================================================================


#include "functools_gen_push.h"
#define FUNCTOOLS_GEN_TYPE_1  BUCKETSORT_GEN_TYPE_2
#define FUNCTOOLS_GEN_TYPE_2  BUCKETSORT_GEN_TYPE_2
#define FUNCTOOLS_GEN_SUFFIX  CONCAT(_BUCKETSORT_GEN, BUCKETSORT_GEN_SUFFIX)
#include "functools_gen.c"

static inline
BUCKETSORT_GEN_TYPE_2
functools_map_fun(BUCKETSORT_GEN_TYPE_2 * A, long i)
{
	return A[i];
}

static inline
BUCKETSORT_GEN_TYPE_2
functools_reduce_fun(BUCKETSORT_GEN_TYPE_2 a, BUCKETSORT_GEN_TYPE_2 b)
{
	return a + b;
}


//==========================================================================================================================================
//= Local Defines
//==========================================================================================================================================


#undef  _TYPE_V
#define _TYPE_V  BUCKETSORT_GEN_EXPAND(_TYPE_V)
typedef BUCKETSORT_GEN_TYPE_1  _TYPE_V;

#undef  _TYPE_I
#define _TYPE_I  BUCKETSORT_GEN_EXPAND(_TYPE_I)
typedef BUCKETSORT_GEN_TYPE_2  _TYPE_I;

#undef  _TYPE_BUCKET_I
#define _TYPE_BUCKET_I  BUCKETSORT_GEN_EXPAND(_TYPE_BUCKET_I)
typedef BUCKETSORT_GEN_TYPE_3  _TYPE_BUCKET_I;

#undef  _TYPE_AD
#define _TYPE_AD  BUCKETSORT_GEN_EXPAND(_TYPE_AD)
typedef BUCKETSORT_GEN_TYPE_4  _TYPE_AD;


//==========================================================================================================================================
//------------------------------------------------------------------------------------------------------------------------------------------
//-                                                              Templates                                                                 -
//------------------------------------------------------------------------------------------------------------------------------------------
//==========================================================================================================================================


/*
 * Array 'A' is not altered.
 * The permutation is returned:
 *     A[i] -> A[permutation[i]]
 *     i.e. to sort the array: A[permutation[i]] = A[i];
 *
 * A serial bucketsort is sometimes faster than the parallel version on a consumer system (Ryzen 3700X)
 * for simple access patterns (e.g. direct access to the array and not through indirection from an array of indexes),
 * most likely because it is absolutely memory bound, and atomics become too expensive.
 *
 * Saving the bucket ids to an array doesn't seem to deteriorate performance.
 *
 * scan_reduce:
 *     - It doesn't seem to affect performance (compared to reimplementing the add-reduction here).
 *     - To num_buckets+1, so that offsets[num_buckets] = N.
 *     - start_from_zero=0 (start from the first element), to sub 1 for each element after and end up with usable offsets to return.
 */


/* TODO: efficient bucketsort for unique only values */

// #undef  bucketsort_unique_serial
// #define bucketsort_unique_serial  BUCKETSORT_GEN_EXPAND(bucketsort_unique_serial)
// void
// bucketsort_unique_serial(_TYPE_V * A, long N, _TYPE_BUCKET_I num_buckets, _TYPE_AD * aux_data,
		// _TYPE_I * permutation_out, _TYPE_I * offsets_out)
// {
	// _TYPE_I * offsets;
	// _TYPE_BUCKET_I b;
	// long i;
	// offsets     = (offsets_out == NULL)     ? offsets_out     : (typeof(offsets)) malloc((num_buckets+1) * sizeof(*offsets));
	// for (i=0;i<num_buckets+1;i++)
		// offsets[i] = 0;
	// for (i=0;i<N;i++)
	// {
		// b = bucketsort_find_bucket(A[i], aux_data);
		// A_bucket_id[i] = b;
		// offsets[b]++;
		// permutation_out[i] = 0;
	// }
	// scan_reduce_serial(offsets, offsets, num_buckets+1, 0, 0, 0);
	// for (i=N-1;i>=0;i--)
	// {
		// b = A_bucket_id[i];
		// offsets[b]--;
		// permutation_out[i] = offsets[b];
	// }
	// if (offsets_out == NULL)
		// free(offsets);
	// if (A_bucket_id_out == NULL)
		// free(A_bucket_id);
// }


#undef  bucketsort_stable_serial
#define bucketsort_stable_serial  BUCKETSORT_GEN_EXPAND(bucketsort_stable_serial)
void
bucketsort_stable_serial(_TYPE_V * restrict A, long N, _TYPE_BUCKET_I num_buckets, _TYPE_AD * restrict aux_data,
		_TYPE_I * restrict permutation_out, _TYPE_I * restrict offsets_out, _TYPE_BUCKET_I * restrict A_bucket_id_out)
{
	_TYPE_I * offsets;
	_TYPE_BUCKET_I * A_bucket_id;
	_TYPE_BUCKET_I b;
	long i;
	offsets = (offsets_out != NULL) ? offsets_out : (typeof(offsets)) malloc((num_buckets+1) * sizeof(*offsets));
	A_bucket_id = (A_bucket_id_out != NULL) ? A_bucket_id_out : (typeof(A_bucket_id)) malloc((N) * sizeof(*A_bucket_id));
	for (i=0;i<num_buckets+1;i++)
		offsets[i] = 0;
	for (i=0;i<N;i++)
	{
		b = bucketsort_find_bucket(A[i], aux_data);
		A_bucket_id[i] = b;
		offsets[b]++;
	}
	scan_reduce_serial(offsets, offsets, num_buckets+1, 0, 0, 0);       // scan_reduce_serial(_TYPE_IN * A, _TYPE_OUT * P, long N, _TYPE_OUT zero, const int start_from_zero, const int backwards);
	for (i=N-1;i>=0;i--)     // In reverse order because we put them from the end of each bucket.
	{
		b = A_bucket_id[i];
		offsets[b]--;
		permutation_out[i] = offsets[b];
	}
	if (offsets_out == NULL)
		free(offsets);
	if (A_bucket_id_out == NULL)
		free(A_bucket_id);
}


#undef  bucketsort
#define bucketsort  BUCKETSORT_GEN_EXPAND(bucketsort)
void
bucketsort(_TYPE_V * restrict A, long N, _TYPE_BUCKET_I num_buckets, _TYPE_AD * restrict aux_data,
		_TYPE_I * restrict permutation_out, _TYPE_I * restrict offsets_out, _TYPE_BUCKET_I * restrict A_bucket_id_out)
{
	_TYPE_I * offsets;
	_TYPE_BUCKET_I * A_bucket_id;
	offsets = (offsets_out != NULL) ? offsets_out : (typeof(offsets)) malloc((num_buckets+1) * sizeof(*offsets));
	A_bucket_id = (A_bucket_id_out != NULL) ? A_bucket_id_out : (typeof(A_bucket_id)) malloc(N * sizeof(*A_bucket_id));
	#pragma omp parallel
	{
		_TYPE_BUCKET_I b;
		long i, pos;
		#pragma omp for
		for (i=0;i<num_buckets+1;i++)
			offsets[i] = 0;
		#pragma omp for
		for (i=0;i<N;i++)
		{
			b = bucketsort_find_bucket(A[i], aux_data);
			A_bucket_id[i] = b;
			__atomic_fetch_add(&offsets[b], 1, __ATOMIC_RELAXED);
			// offsets[b]++;
		}
		scan_reduce_concurrent(offsets, offsets, num_buckets+1, 0, 0, 0);
		#pragma omp for
		for (i=0;i<N;i++)
		{
			b = A_bucket_id[i];
			// b = bucketsort_find_bucket(A[i], aux_data);
			pos = __atomic_sub_fetch(&offsets[b], 1, __ATOMIC_RELAXED);
			// pos = --offsets[b];
			permutation_out[i] = pos;
		}
	}
	if (offsets_out == NULL)
		free(offsets);
	if (A_bucket_id_out == NULL)
		free(A_bucket_id);
}


// Sort the bucket (another nested bucketsort) according to the thread numbers (lower tnums added lower indexes).
#undef  sort_bucket
#define sort_bucket  BUCKETSORT_GEN_EXPAND(sort_bucket)
static inline
void
sort_bucket(_TYPE_I * restrict permutation, long N, int num_threads, unsigned short * owner)
{
	_TYPE_I * buf;
	_TYPE_I offsets[num_threads + 1];
	_TYPE_BUCKET_I b;
	long i;
	buf = (typeof(buf)) malloc(N * sizeof(*buf));
	for (i=0;i<num_threads+1;i++)
		offsets[i] = 0;
	for (i=0;i<N;i++)
	{
		buf[i] = permutation[i];
		b = owner[i];
		offsets[b]++;
	}
	scan_reduce_serial(offsets, offsets, num_threads+1, 0, 0, 0);
	for (i=0;i<N;i++)     // Per thread elements are in reverse order, so we need to reverse again, unlike bucketsort_stable_serial().
	{
		b = owner[i];
		offsets[b]--;
		permutation[offsets[b]] = buf[i];
	}
	free(buf);
}

#undef  bucketsort_stable
#define bucketsort_stable  BUCKETSORT_GEN_EXPAND(bucketsort_stable)
void
bucketsort_stable(_TYPE_V * restrict A, long N, _TYPE_BUCKET_I num_buckets, _TYPE_AD * restrict aux_data,
		_TYPE_I * restrict permutation_out, _TYPE_I * restrict offsets_out, _TYPE_BUCKET_I * restrict A_bucket_id_out)
{
	_TYPE_I * permutation_reverse;
	_TYPE_I * offsets;
	_TYPE_BUCKET_I * A_bucket_id;
	unsigned short * owner_tnum;
	permutation_reverse = (typeof(permutation_reverse)) malloc(N * sizeof(*permutation_reverse));
	offsets = (offsets_out != NULL) ? offsets_out : (typeof(offsets)) malloc((num_buckets+1) * sizeof(*offsets));
	A_bucket_id = (A_bucket_id_out != NULL) ? A_bucket_id_out : (typeof(A_bucket_id)) malloc(N * sizeof(*A_bucket_id));
	owner_tnum = (typeof(owner_tnum)) malloc(N * sizeof(*owner_tnum));
	#pragma omp parallel
	{
		int num_threads = safe_omp_get_num_threads();
		int tnum = omp_get_thread_num();
		_TYPE_BUCKET_I b, b_n;
		long i, i_s, i_e, pos;
		#pragma omp for
		for (i=0;i<num_buckets+1;i++)
			offsets[i] = 0;
		#pragma omp for
		for (i=0;i<N;i++)
		{
			b = bucketsort_find_bucket(A[i], aux_data);
			A_bucket_id[i] = b;
			__atomic_fetch_add(&offsets[b], 1, __ATOMIC_RELAXED);
		}
		scan_reduce_concurrent(offsets, offsets, num_buckets+1, 0, 0, 0);
		#pragma omp for
		for (i=0;i<N;i++)
		{
			b = A_bucket_id[i];
			pos = __atomic_sub_fetch(&offsets[b], 1, __ATOMIC_RELAXED);
			permutation_out[i] = pos;
			permutation_reverse[pos] = i;
			owner_tnum[pos] = tnum;
		}
		loop_partitioner_balance_prefix_sums(num_threads, tnum, offsets, num_buckets, N, &i_s, &i_e);
		for (i=i_s;i<i_e;i++)
		{
			b_n = offsets[i+1] - offsets[i];
			pos = offsets[i];
			sort_bucket(&permutation_reverse[pos], b_n, num_threads, &owner_tnum[pos]);
		}
		#pragma omp barrier
		#pragma omp for
		for (i=0;i<N;i++)
			permutation_out[permutation_reverse[i]] = i;
	}
	free(permutation_reverse);
	if (offsets_out == NULL)
		free(offsets);
	if (A_bucket_id_out == NULL)
		free(A_bucket_id);
}


//==========================================================================================================================================
//= Includes Undefs
//==========================================================================================================================================


#include "functools_gen_pop.h"

