#include "bucketsort_gen.h"
// #include "parallel_util.h"


#ifndef BUCKETSORT_GEN_C
#define BUCKETSORT_GEN_C

#endif /* BUCKETSORT_GEN_C */


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//------------------------------------------------------------------------------------------------------------------------------------------
//-                                                              Templates                                                                 -
//------------------------------------------------------------------------------------------------------------------------------------------
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


//==========================================================================================================================================
//= User Functions Declarations
//==========================================================================================================================================


#undef  bucketsort_find_bucket
#define bucketsort_find_bucket  BUCKETSORT_GEN_EXPAND(bucketsort_find_bucket)
static _TYPE_BUCKET_I bucketsort_find_bucket(_TYPE_V a, _TYPE_AD * aux_data);


//==========================================================================================================================================
//= Includes
//==========================================================================================================================================


#include "functools_gen_undef.h"
#define FUNCTOOLS_GEN_TYPE_1  BUCKETSORT_GEN_TYPE_2
#define FUNCTOOLS_GEN_SUFFIX  CONCAT(_BUCKETSORT_GEN, BUCKETSORT_GEN_SUFFIX)
#include "functools_gen.c"

static inline
void
functools_reduce_fun(BUCKETSORT_GEN_TYPE_2 * partial, BUCKETSORT_GEN_TYPE_2 * x)
{
	*partial += *x;
}

static inline
void
functools_set_value(BUCKETSORT_GEN_TYPE_2 * x, BUCKETSORT_GEN_TYPE_2 val)
{
	*x = val;
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
//= Generic Code
//==========================================================================================================================================


/*
 * Array 'A' is not altered.
 * The permutation is returned:
 *     A[i] -> A[permutation[i]]
 *     i.e. to sort the array: A[permutation[i]] = A[i];
 *
 * A serial bucketsort seems to be faster than the parallel version on a consumer system (Ryzen 3700X)
 * for simple access patterns (e.g. direct access to the array and not through indirection from an array of indexes),
 * most likely because it is absolutely memory bound, and atomics become too expensive.
 *
 * Saving the bucket ids to an array doesn't seem to deteriorate performance.
 *
 * scan:
 *     - It doesn't seem to affect performance (compared to reimplementing the add-reduction here).
 *     - To num_buckets+1, so that offsets[num_buckets] = N.
 *     - start_from_zero=0 (start from the first element), to sub 1 after and end up with usable offsets to return.
 */

// #undef  bucketsort_unique_serial
// #define bucketsort_unique_serial  BUCKETSORT_GEN_EXPAND(bucketsort_unique_serial)
// void
// bucketsort_unique_serial(_TYPE_V * A, _TYPE_I N, _TYPE_BUCKET_I num_buckets, _TYPE_AD * aux_data,
		// _TYPE_I * permutation_out, _TYPE_I * offsets_out)
// {
	// _TYPE_I * offsets;
	// _TYPE_BUCKET_I b;
	// _TYPE_I i;
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
	// scan_serial(offsets, offsets, num_buckets+1, 0, 0);
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
bucketsort_stable_serial(_TYPE_V * A, _TYPE_I N, _TYPE_BUCKET_I num_buckets, _TYPE_AD * aux_data,
		_TYPE_I * permutation_out, _TYPE_I * offsets_out, _TYPE_BUCKET_I * A_bucket_id_out)
{
	_TYPE_I * offsets;
	_TYPE_BUCKET_I * A_bucket_id;
	_TYPE_BUCKET_I b;
	_TYPE_I i;
	offsets = (offsets_out != NULL) ? offsets_out : (typeof(offsets)) malloc((num_buckets+1) * sizeof(*offsets));
	A_bucket_id = (A_bucket_id_out != NULL) ? A_bucket_id_out : (typeof(A_bucket_id)) malloc((N) * sizeof(*A_bucket_id));
	for (i=0;i<num_buckets+1;i++)
		offsets[i] = 0;
	for (i=0;i<N;i++)
	{
		b = bucketsort_find_bucket(A[i], aux_data);
		A_bucket_id[i] = b;
		offsets[b]++;
		permutation_out[i] = 0;
	}
	scan_serial(offsets, offsets, num_buckets+1, 0, 0);
	for (i=N-1;i>=0;i--)
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
bucketsort(_TYPE_V * A, _TYPE_I N, _TYPE_BUCKET_I num_buckets, _TYPE_AD * aux_data,
		_TYPE_I * permutation_out, _TYPE_I * offsets_out, _TYPE_BUCKET_I * A_bucket_id_out)
{
	_TYPE_I * offsets;
	_TYPE_BUCKET_I * A_bucket_id;
	A_bucket_id = (typeof(A_bucket_id)) malloc(N * sizeof(*A_bucket_id));
	offsets = (offsets_out != NULL) ? offsets_out : (typeof(offsets)) malloc((num_buckets+1) * sizeof(*offsets));
	A_bucket_id = (A_bucket_id_out != NULL) ? A_bucket_id_out : (typeof(A_bucket_id)) malloc((N) * sizeof(*A_bucket_id));
	#pragma omp parallel
	{
		_TYPE_BUCKET_I b;
		_TYPE_I i;
		#pragma omp for schedule(static)
		for (i=0;i<num_buckets+1;i++)
			offsets[i] = 0;
		#pragma omp for schedule(static)
		for (i=0;i<N;i++)
		{
			b = bucketsort_find_bucket(A[i], aux_data);
			A_bucket_id[i] = b;
			__atomic_fetch_add(&offsets[b], 1, __ATOMIC_RELAXED);
			// offsets[b]++;
			permutation_out[i] = 0;
		}
	}
	// scan_serial(offsets, offsets, num_buckets+1, 0, 0);
	scan(offsets, offsets, num_buckets+1, 0, 0);
	#pragma omp parallel
	{
		_TYPE_BUCKET_I b;
		_TYPE_I pos, i;
		#pragma omp for schedule(static)
		for (i=0;i<N;i++)
		{
			b = A_bucket_id[i];
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


/* #undef  bucketsort2
#define bucketsort2  BUCKETSORT_GEN_EXPAND(bucketsort2)
_TYPE_I *
bucketsort2(_TYPE_V * A, _TYPE_I N, _TYPE_BUCKET_I num_buckets, _TYPE_AD * aux_data, _TYPE_I ** offsets_out)
{
	int num_threads = safe_omp_get_num_threads_next_par_region();
	_TYPE_I * permutation;
	_TYPE_I * offsets;
	_TYPE_I t_base[num_threads];
	_TYPE_BUCKET_I * A_bucket_id;

	permutation = malloc(N * sizeof(*permutation));
	A_bucket_id = malloc(N * sizeof(*A_bucket_id));
	if (offsets_out == NULL)
		offsets = malloc((num_buckets+1) * sizeof(*offsets));
	else
		offsets = offsets_out;

	offsets[num_buckets] = N;

	#pragma omp parallel
	{
		_TYPE_I base;
		int tnum = omp_get_thread_num();
		_TYPE_I i, i_s, i_e;
		_TYPE_I sum, tmp, pos;
		_TYPE_BUCKET_I b;

		loop_partitioner_balance_iterations(num_threads, tnum, 0, num_buckets, &i_s, &i_e);

		for (i=i_s;i<i_e;i++)
			offsets[i] = 0;

		#pragma omp barrier

		#pragma omp for schedule(static)
		for (i=0;i<N;i++)
		{
			b = bucketsort_find_bucket(A[i], aux_data);
			A_bucket_id[i] = b;
			__atomic_fetch_add(&offsets[b], 1, __ATOMIC_RELAXED);
			// offsets[b]++;
			permutation[i] = 0;
		}

		#pragma omp barrier

		base = 0;
		for (i=i_s;i<i_e;i++)
			base += offsets[i];

		__atomic_store_n(&t_base[tnum], base, __ATOMIC_RELAXED);

		#pragma omp barrier

		sum = 0;
		#pragma omp single
		{
			for (i=0;i<num_threads;i++)
			{
				tmp = t_base[i];
				__atomic_store_n(&t_base[i], sum, __ATOMIC_RELAXED);
				sum += tmp;
			}
		}

		#pragma omp barrier

		sum = t_base[tnum];
		for (i=i_s;i<i_e;i++)
		{
			sum += offsets[i];
			offsets[i] = sum;
		}

		#pragma omp barrier

		#pragma omp for schedule(static)
		for (i=0;i<N;i++)
		{
			// b = bucketsort_find_bucket(A[i], aux_data);
			b = A_bucket_id[i];
			pos = __atomic_sub_fetch(&offsets[b], 1, __ATOMIC_RELAXED);
			// pos = offsets[b]--;
			permutation[i] = pos;
		}
	}

	if (offsets_out == NULL)
		free(offsets);

	return permutation;
} */


//==========================================================================================================================================
//= Includes Undefs
//==========================================================================================================================================


#include "functools_gen_undef.h"

