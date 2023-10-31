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
 *
 *  1 -> swapped positions (b, a)
 * -1 -> as is (a, b)
 *  0 -> random (equality)
 *
 * increasing order:
 *	(a > b) ?  1 : (a < b) ? -1 : 0
 * decreasing order:
 *	(a > b) ? -1 : (a < b) ?  1 : 0
 */
#undef  samplesort_cmp
#define samplesort_cmp  SAMPLESORT_GEN_EXPAND(samplesort_cmp)
static int samplesort_cmp(_TYPE_V a, _TYPE_V b, _TYPE_AD * aux_data);


//==========================================================================================================================================
//= Includes
//==========================================================================================================================================


#include "quicksort_gen_push.h"
#define QUICKSORT_GEN_TYPE_1  SAMPLESORT_GEN_TYPE_1
#define QUICKSORT_GEN_TYPE_2  SAMPLESORT_GEN_TYPE_2
#define QUICKSORT_GEN_TYPE_3  SAMPLESORT_GEN_TYPE_4
#define QUICKSORT_GEN_SUFFIX  CONCAT(_SAMPLESORT_GEN, SAMPLESORT_GEN_SUFFIX)
#include "quicksort_gen.c"
static inline
int
quicksort_cmp(SAMPLESORT_GEN_TYPE_1 a, SAMPLESORT_GEN_TYPE_1 b, SAMPLESORT_GEN_TYPE_4 * aux_data)
{
	return samplesort_cmp(a, b, aux_data);
}


// #include "mergesort_gen_push.h"
// #define MERGESORT_GEN_TYPE_1  SAMPLESORT_GEN_TYPE_1
// #define MERGESORT_GEN_TYPE_2  SAMPLESORT_GEN_TYPE_2
// #define MERGESORT_GEN_TYPE_3  SAMPLESORT_GEN_TYPE_4
// #define MERGESORT_GEN_SUFFIX  CONCAT(_SAMPLESORT_GEN, SAMPLESORT_GEN_SUFFIX)
// #include "mergesort_gen.c"
// static inline
// int
// mergesort_cmp(SAMPLESORT_GEN_TYPE_1 a, SAMPLESORT_GEN_TYPE_1 b, SAMPLESORT_GEN_TYPE_4 * aux_data)
// {
	// return samplesort_cmp(a, b, aux_data);
// }


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


#undef  samplesort_concurrent
#define samplesort_concurrent  SAMPLESORT_GEN_EXPAND(samplesort_concurrent)
void
samplesort_concurrent(_TYPE_V * A, long N, _TYPE_AD * aux_data)
{
	int num_threads = safe_omp_get_num_threads();
	int tnum = omp_get_thread_num();

	const long num_splitters_default = 128 * num_threads;
	// long num_splitters_default = 15;
	// long num_splitters_default = 8 * next_pow_of_2(num_threads) - 1;
	// long num_splitters_default = 16 * next_pow_of_2(num_threads) - 1;
	// long num_splitters_default = 1024 * num_threads;

	const long num_buckets_default = num_splitters_default + 1;

	static long num_splitters;
	static long num_buckets;

	static _TYPE_V * splitters;
	static _TYPE_I * bucket_pos;   // Starting positions of buckets.
	static _TYPE_BUCKET_I * A_bucket_id;

	static _TYPE_V *** t_buckets;    // visible handles for the per-thread buckets : num_threads x num_buckets
	static _TYPE_I ** t_buckets_n;   // per-thread buckets sizes : num_threads x num_buckets

	_TYPE_V ** buckets;   // thread buckets : num_buckets x buckets_n[i]
	long * buckets_n;     // thread buckets sizes : num_buckets

	long priv_bucket_id_s, priv_bucket_id_e, priv_num_buckets;
	_TYPE_V ** priv_bucket;   // the private buckets of the thread : priv_num_buckets
	_TYPE_I * priv_bucket_n;

	long i, j, k, b_id;
	long i_s, i_e;
	long sum, tmp;
	_TYPE_BUCKET_I b;

	if (N < num_buckets_default)
	{
		#pragma omp single nowait
		{
			quicksort(A, N, aux_data);
		}
		#pragma omp barrier
		return;
	}

	#pragma omp single nowait
	{
		num_splitters = num_splitters_default;

		splitters = (typeof(splitters)) malloc(num_splitters * sizeof(*splitters));

		// printf("splitters: %ld\n", num_splitters);
		num_splitters = find_splitters(A, N, splitters, num_splitters, aux_data);
		// for (long i=0;i<num_splitters;i++)
		// {
			// printf("%g, ", splitters[i]);
		// }
		// printf("\n");
		// printf("num_splitters = %ld\n", num_splitters);
		// exit(0);

		num_buckets = num_splitters + 1;

		bucket_pos = (typeof(bucket_pos)) malloc((num_buckets+1) * sizeof(*bucket_pos));
		for (i=0;i<num_buckets;i++)
			bucket_pos[i] = 0;

		t_buckets = (typeof(t_buckets)) malloc(num_threads * sizeof(*t_buckets));
		t_buckets_n = (typeof(t_buckets_n)) malloc(num_threads * sizeof(*t_buckets_n));
		A_bucket_id = (typeof(A_bucket_id)) malloc(N * sizeof(*A_bucket_id));
		for (i=0;i<num_threads;i++)
		{
			t_buckets[i] = (typeof(*t_buckets)) malloc(num_buckets * sizeof(**t_buckets));
			t_buckets_n[i] = (typeof(*t_buckets_n)) malloc(num_buckets * sizeof(**t_buckets_n));
		}
	}

	#pragma omp barrier

	loop_partitioner_balance_iterations(num_threads, tnum, 0, N, &i_s, &i_e);
	// printf("%d: i=[%ld, %ld]\n", tnum, i_s, i_e);

	buckets = (typeof(buckets)) malloc(num_buckets * sizeof(*buckets));
	buckets_n = (typeof(buckets_n)) malloc(num_buckets * sizeof(*buckets_n));

	for (i=0;i<num_buckets;i++)
		buckets_n[i] = 0;
	for (i=i_s;i<i_e;i++)
	{
		#undef  binary_search_cmp
		#define binary_search_cmp(_target, _A, _i) ({ samplesort_cmp(_target, _A[_i], aux_data) > 0 ? 1 : 0; })
		b = binary_search_simple(splitters, 0, num_splitters-1, A[i], binary_search_cmp);
		#undef  binary_search_cmp

		A_bucket_id[i] = b;
		buckets_n[b]++;
	}
	for (i=0;i<num_buckets;i++)
	{
		buckets[i] = NULL;
		if (buckets_n[i] > 0)
			buckets[i] = (typeof(*buckets)) malloc(buckets_n[i] * sizeof(**buckets));   // These are freed via t_buckets by possibly another thread when no longer needed.
		t_buckets[tnum][i] = buckets[i];
		t_buckets_n[tnum][i] = buckets_n[i];
		__atomic_fetch_add(&bucket_pos[i], buckets_n[i], __ATOMIC_RELAXED);
	}
	for (i=i_s;i<i_e;i++)
	{
		b = A_bucket_id[i];
		buckets[b][--buckets_n[b]] = A[i];
	}

	#pragma omp barrier

	// Calculate bucket offsets in original array.
	#pragma omp single nowait
	{
		tmp = 0;
		sum = 0;
		for (i=0;i<num_buckets+1;i++)
		{
			tmp = bucket_pos[i];
			bucket_pos[i] = sum;
			sum += tmp;
		}
		free(A_bucket_id);
	}

	#pragma omp barrier

	loop_partitioner_balance_prefix_sums(num_threads, tnum, bucket_pos, num_buckets, bucket_pos[num_buckets], &priv_bucket_id_s, &priv_bucket_id_e);
	priv_num_buckets = priv_bucket_id_e - priv_bucket_id_s;
	// printf("%d: bucket_id=[%ld, %ld] (%ld)\n", tnum, priv_bucket_id_s, priv_bucket_id_e, priv_num_buckets);

	priv_bucket = (typeof(priv_bucket)) malloc(priv_num_buckets * sizeof(*priv_bucket));
	priv_bucket_n = (typeof(priv_bucket_n)) malloc(priv_num_buckets * sizeof(*priv_bucket_n));

	// Collect each bucket that belongs to the thread and sort it.
	_TYPE_I priv_bucket_n_max = 0;
	for (b=0;b<priv_num_buckets;b++)
	{
		b_id = priv_bucket_id_s + b;
		priv_bucket_n[b] = 0;
		for (i=0;i<num_threads;i++)
			priv_bucket_n[b] += t_buckets_n[i][b_id];
		if (priv_bucket_n[b] > priv_bucket_n_max)
			priv_bucket_n_max = priv_bucket_n[b];
	}
	_TYPE_I * partitions = (typeof(partitions)) malloc(priv_bucket_n_max * sizeof(*partitions));
	// _TYPE_V * buf = (typeof(buf)) malloc(priv_bucket_n_max * sizeof(*buf));
	for (b=0;b<priv_num_buckets;b++)
	{
		b_id = priv_bucket_id_s + b;
		// priv_bucket_n[b] = 0;
		// for (i=0;i<num_threads;i++)
			// priv_bucket_n[b] += t_buckets_n[i][b_id];
		priv_bucket[b] = NULL;
		if (priv_bucket_n[b] <= 0)
			continue;
		priv_bucket[b] = (typeof(*priv_bucket)) malloc(priv_bucket_n[b] * sizeof(**priv_bucket));
		k = 0;
		for (i=0;i<num_threads;i++)
		{
			for (j=0;j<t_buckets_n[i][b_id];j++)
				priv_bucket[b][k++] = t_buckets[i][b_id][j];
			free(t_buckets[i][b_id]);
		}

		// quicksort(priv_bucket[b], priv_bucket_n[b], aux_data);
		quicksort_no_malloc(priv_bucket[b], priv_bucket_n[b], aux_data, partitions);
		// mergesort(priv_bucket[b], priv_bucket_n[b], aux_data);
		// mergesort_no_malloc(priv_bucket[b], priv_bucket_n[b], aux_data, buf);

		// printf("%d: %d\n", b_id, priv_bucket_n[b]);
		// for (i=0;i<priv_bucket_n[b];i++)
			// printf("%d %g\n", tnum, B[priv_bucket[b][i]]);
	}
	// free(buf);
	free(partitions);

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

	free(priv_bucket);
	free(priv_bucket_n);

	free(buckets);
	free(buckets_n);

	#pragma omp barrier

	#pragma omp single nowait
	{
		free(splitters);
		free(bucket_pos);
		for (i=0;i<num_threads;i++)
		{
			free(t_buckets[i]);
			free(t_buckets_n[i]);
		}
		free(t_buckets);
		free(t_buckets_n);
	}

	#pragma omp barrier
}


#undef  samplesort
#define samplesort  SAMPLESORT_GEN_EXPAND(samplesort)
void
samplesort(_TYPE_V * A, long N, _TYPE_AD * aux_data)
{
	if (omp_get_level() > 0)
	{
		quicksort(A, N, aux_data);
		return;
	}
	_Pragma("omp parallel")
	{
		samplesort_concurrent(A, N, aux_data);
	}
}


//==========================================================================================================================================
//= Includes Undefs
//==========================================================================================================================================

#include "sort/quicksort_gen_pop.h"
// #include "sort/mergesort_gen_pop.h"

