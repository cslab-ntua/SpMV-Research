#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

#include "macros/macrolib.h"
#include "debug.h"
#include "parallel_util.h"
#include "omp_functions.h"

#include "samplesort_gen.h"


//==========================================================================================================================================
//= User Functions Declarations
//==========================================================================================================================================


/* compare(a, b)
 * increasing order:
 *	(a > b) ? 1 : (a < b) ? -1 : 0
 * decreasing order:
 *	(a < b) ? 1 : (a > b) ? -1 : 0
 */
#undef  samplesort_cmp
#define samplesort_cmp  SAMPLESORT_GEN_EXPAND(samplesort_cmp)
static int samplesort_cmp(_TYPE_V a, _TYPE_V b, _TYPE_AD * aux_data);


//==========================================================================================================================================
//= Includes
//==========================================================================================================================================


#include "sort/quicksort_gen_push.h"
#define QUICKSORT_GEN_TYPE_1  SAMPLESORT_GEN_TYPE_1
#define QUICKSORT_GEN_TYPE_2  SAMPLESORT_GEN_TYPE_2
#define QUICKSORT_GEN_TYPE_3  SAMPLESORT_GEN_TYPE_4
#define QUICKSORT_GEN_SUFFIX  CONCAT(_SAMPLESORT_GEN, SAMPLESORT_GEN_SUFFIX)
#include "sort/quicksort_gen.c"

static inline
int
quicksort_cmp(_TYPE_V a, _TYPE_V b, _TYPE_AD * aux_data)
{
	return samplesort_cmp(a, b, aux_data);
}


//==========================================================================================================================================
//= Local Defines
//==========================================================================================================================================


#undef  _TYPE_V
#define _TYPE_V  SAMPLESORT_GEN_EXPAND(_TYPE_V)
typedef SAMPLESORT_GEN_TYPE_1  _TYPE_V;

#undef  _TYPE_I
#define _TYPE_I  SAMPLESORT_GEN_EXPAND(_TYPE_I)
typedef SAMPLESORT_GEN_TYPE_2  _TYPE_I;

#undef  _TYPE_BUCKET_I
#define _TYPE_BUCKET_I  SAMPLESORT_GEN_EXPAND(_TYPE_BUCKET_I)
typedef SAMPLESORT_GEN_TYPE_3  _TYPE_BUCKET_I;

#undef  _TYPE_AD
#define _TYPE_AD  SAMPLESORT_GEN_EXPAND(_TYPE_AD)
typedef SAMPLESORT_GEN_TYPE_4  _TYPE_AD;


//==========================================================================================================================================
//------------------------------------------------------------------------------------------------------------------------------------------
//-                                                              Templates                                                                 -
//------------------------------------------------------------------------------------------------------------------------------------------
//==========================================================================================================================================


/* #undef  sorted_array_to_tree
#define sorted_array_to_tree  SAMPLESORT_GEN_EXPAND(sorted_array_to_tree)
static inline
void
sorted_array_to_tree(_TYPE_V * A, long N)
{
	_TYPE_V * BUF;
	long pow = 1;
	long i, j;
	long excess;   // Last level of the tree.
	long n;

	while (pow - 1 <= N)      // Max power of 2 <= N + 1.
		pow <<= 1;
	pow >>= 1;
	if (pow == 0)
		return;

	BUF = (typeof(BUF)) malloc(N * sizeof(*BUF));
	excess = N - (pow - 1);

	for (j=0,i=0;i<2*excess;j++,i+=2)
	{
		BUF[N-excess+j] = A[i];
		BUF[j] = A[i+1];
	}
	for (i=2*excess;i<N;j++,i++)
		BUF[j] = A[i];

	for (i=N-excess;i<N;i++)
		A[i] = BUF[i];

	n = N - excess;
	j = 0;
	while (pow != 1)
	{
		for (i=(pow>>1)-1;i<n;i+=pow,j++)
			A[j] = BUF[i];
		pow >>= 1;
	}
	free(BUF);
}


#undef  find_bucket_num_perfect_tree
#define find_bucket_num_perfect_tree  SAMPLESORT_GEN_EXPAND(find_bucket_num_perfect_tree)
static inline
_TYPE_BUCKET_I
find_bucket_num_perfect_tree(_TYPE_V val, _TYPE_V * splitters, long num_splitters, _TYPE_AD * aux_data)
{
	long i;
	long j = 0;
	for (i=1;i<=num_splitters;i*=2)   // == is only for when num_splitters = 1 (perfect binary tree: num_splitters = 2^k - 1).
		j = 2*j + 1 + (samplesort_cmp(val, splitters[j], aux_data) > 0 ? 1 : 0);
	j -= num_splitters;
	// if (j < 0)
		// error("bucket<0 , %d %g", j, B[val]);
	return j;
} */


/* #undef  next_pow_of_2
#define next_pow_of_2  SAMPLESORT_GEN_EXPAND(next_pow_of_2)
static inline
long
next_pow_of_2(long n)
{
	long i = 1;
	while (i < n)  // if it overflows
	{
		if (i == 0)
			error("next power of 2 overflows the integer");
		i <<= 1;
	}
	return i;
} */


/* 
 * Samples should be a mini representation of the whole array partitioning.
 * If Si splitters:
 *     0-x , S0 , x-2*x , S1 , ... , (n-1)*x-n*x , S(n-1) , n*x-(n+1)*x
 *     b0         b1                 b(n-1)                 bn
 */
#undef  find_splitters
#define find_splitters  SAMPLESORT_GEN_EXPAND(find_splitters)
static inline
long
find_splitters(_TYPE_V * A, long N, _TYPE_V * splitters, long num_splitters, _TYPE_AD * aux_data)
{
	// long samples_per_bucket = 10;
	// long samples_per_bucket = N / 100000;
	long samples_per_bucket = 10;
	long num_samples = samples_per_bucket * (num_splitters + 1);   // +1 to leave elems in the end for the last bucket.
	// printf("num_samples = %ld\n", num_samples);
	_TYPE_V * samples;
	long i, j, div;
	if (num_samples < 2 * num_splitters)
		num_samples = 2 * num_splitters;
	if ((samples_per_bucket == 0) || (num_samples > N))
	{
		samples_per_bucket = N / (num_splitters + 1);
		num_samples = N;
	}
	samples = (typeof(samples)) malloc(num_samples * sizeof(*samples));
	div = N/num_samples;
	for (i=0;i<num_samples;i++)
		samples[i] = A[i*div];
	// printf("quicksort samples\n");
	quicksort(samples, num_samples, aux_data);
	for (i=0,j=0;i<num_samples;i++)
	{
		if (samplesort_cmp(samples[i], samples[j], aux_data) == 0)
			continue;
		j++;
		samples[j] = samples[i];
	}
	num_samples = j;
	if (num_samples <= 1)
		error("too few samples");
	if (num_samples <= num_splitters)
		num_splitters = num_samples - 1;
	samples_per_bucket = num_samples / num_splitters;
	for (i=0;i<num_splitters;i++)
		splitters[i] = samples[samples_per_bucket*(i+1) - 1];   // +1 to leave elems in the begining for the first bucket.
	free(samples);
	return num_splitters;
}


#undef  samplesort
#define samplesort  SAMPLESORT_GEN_EXPAND(samplesort)
void
samplesort(_TYPE_V * A, long N, _TYPE_AD * aux_data)
{
	int num_threads = safe_omp_get_num_threads_external();
	// long num_splitters = 15;
	// long num_splitters = 8 * next_pow_of_2(num_threads) - 1;
	// long num_splitters = 16 * next_pow_of_2(num_threads) - 1;
	long num_splitters = 128 * num_threads;
	// long num_splitters = 1024 * num_threads;
	_TYPE_V splitters[num_splitters];
	long num_buckets = num_splitters + 1;
	_TYPE_I bucket_pos[num_buckets+1];     // Starting positions of buckets.
	_TYPE_BUCKET_I * A_bucket_id;

	_TYPE_V * t_buckets[num_threads][num_buckets];
	_TYPE_I t_bucket_n[num_threads][num_buckets];

	if ((num_threads < 2) || (N < num_buckets))
	{
		quicksort(A, N, aux_data);
		return;
	}

	A_bucket_id = (typeof(A_bucket_id)) malloc(N * sizeof(*A_bucket_id));

	// printf("splitters: %ld\n", num_splitters);
	num_splitters = find_splitters(A, N, splitters, num_splitters, aux_data);
	// for (long i=0;i<num_splitters;i++)
	// {
		// printf("%g, ", splitters[i]);
	// }
	// printf("\n");
	// printf("num_splitters = %ld\n", num_splitters);
	// exit(0);

	// printf("sorted_array_to_tree\n");
	// sorted_array_to_tree(splitters, num_splitters);

	#pragma omp parallel
	{
		int tnum = omp_get_thread_num();
		_TYPE_V * bucket[num_buckets];
		long bucket_n[num_buckets];
		long i_s, i_e;
		long i, j, k, b_id;
		long sum, tmp;
		_TYPE_BUCKET_I b;

		long priv_bucket_id_s, priv_bucket_id_e, priv_num_buckets;

		loop_partitioner_balance_iterations(num_threads, tnum, 0, N, &i_s, &i_e);
		loop_partitioner_balance_iterations(num_threads, tnum, 0, num_buckets, &priv_bucket_id_s, &priv_bucket_id_e, , &priv_num_buckets);
		// printf("%d: i=[%ld, %ld]\n", tnum, i_s, i_e);
		// printf("%d: bucket_id=[%ld, %ld] (%ld)\n", tnum, priv_bucket_id_s, priv_bucket_id_e, priv_num_buckets);

		_TYPE_V * priv_bucket[priv_num_buckets];     // We try to bring everything close (e.g. NUMA node) for the local sorting.
		_TYPE_I priv_bucket_n[priv_num_buckets];

		for (i=0;i<num_buckets;i++)
			bucket_n[i] = 0;
		for (i=i_s;i<i_e;i++)
		{
			// b = find_bucket_num_perfect_tree(A[i], splitters, num_splitters, aux_data);

			#undef  binary_search_cmp
			#define binary_search_cmp(_target, _A, _i) ({ samplesort_cmp(_target, _A[_i], aux_data) > 0 ? 1 : 0; })
			b = binary_search_simple(splitters, 0, num_splitters-1, A[i], binary_search_cmp);
			#undef  binary_search_cmp

			A_bucket_id[i] = b;
			bucket_n[b]++;
		}
		for (i=0;i<num_buckets;i++)
		{
			bucket[i] = NULL;
			if (bucket_n[i] > 0)
				bucket[i] = (typeof(*bucket)) malloc(bucket_n[i] * sizeof(**bucket));
			t_buckets[tnum][i] = bucket[i];
			__atomic_store_n(&(t_bucket_n[tnum][i]), bucket_n[i], __ATOMIC_RELAXED);
		}
		for (i=i_s;i<i_e;i++)
		{
			b = A_bucket_id[i];
			bucket[b][--bucket_n[b]] = A[i];
		}

		#pragma omp barrier

		#pragma omp single
		{
			free(A_bucket_id);
		}

		#pragma omp barrier

		// Collect each bucket that belongs to the thread and sort it.
		for (b=0;b<priv_num_buckets;b++)
		{
			b_id = priv_bucket_id_s + b;
			priv_bucket_n[b] = 0;
			for (i=0;i<num_threads;i++)
				priv_bucket_n[b] += t_bucket_n[i][b_id];
			// printf("%d: bucket %ld size = %d\n", tnum, b_id, priv_bucket_n[b]);
			priv_bucket[b] = NULL;
			if (priv_bucket_n[b] > 0)
				priv_bucket[b] = (typeof(*priv_bucket)) malloc(priv_bucket_n[b] * sizeof(**priv_bucket));
			k = 0;
			for (i=0;i<num_threads;i++)
			{
				for (j=0;j<t_bucket_n[i][b_id];j++)
					priv_bucket[b][k++] = t_buckets[i][b_id][j];
				free(t_buckets[i][b_id]);
			}
			__atomic_store_n(&(bucket_pos[b_id]), priv_bucket_n[b], __ATOMIC_RELAXED);

			quicksort(priv_bucket[b], priv_bucket_n[b], aux_data);

			// printf("%d: %d\n", b_id, priv_bucket_n[b]);
			// for (i=0;i<priv_bucket_n[b];i++)
				// printf("%d %g\n", tnum, B[priv_bucket[b][i]]);
		}

		#pragma omp barrier

		// Calculate bucket offsets in original array.
		#pragma omp single
		{
			tmp = 0;
			sum = 0;
			for (i=0;i<num_buckets+1;i++)
			{
				tmp = bucket_pos[i];
				__atomic_store_n(&(bucket_pos[i]), sum, __ATOMIC_RELAXED);
				sum += tmp;
			}
		}

		#pragma omp barrier

		// Reconstruct original array.
		for (b=0;b<priv_num_buckets;b++)
		{
			b_id = priv_bucket_id_s + b;
			j = bucket_pos[b_id];
			for (i=0;i<priv_bucket_n[b];i++)
				A[j + i] = priv_bucket[b][i];
		}

		for (b=0;b<priv_num_buckets;b++)
			free(priv_bucket[b]);
	}

}


//==========================================================================================================================================
//= Includes Undefs
//==========================================================================================================================================

#include "sort/quicksort_gen_pop.h"

