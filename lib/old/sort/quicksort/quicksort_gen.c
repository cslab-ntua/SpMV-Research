#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

#include "macros/macrolib.h"
#include "debug.h"
#include "parallel_util.h"
#include "omp_functions.h"

#include "quicksort_gen.h"


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
#undef  quicksort_cmp
#define quicksort_cmp  QUICKSORT_GEN_EXPAND(quicksort_cmp)
static int quicksort_cmp(_TYPE_V a, _TYPE_V b, _TYPE_AD * aux_data);


//==========================================================================================================================================
//= Includes
//==========================================================================================================================================


//==========================================================================================================================================
//= Local Defines
//==========================================================================================================================================


#undef  _TYPE_V
#define _TYPE_V  QUICKSORT_GEN_EXPAND(_TYPE_V)
typedef QUICKSORT_GEN_TYPE_1  _TYPE_V;

#undef  _TYPE_I
#define _TYPE_I  QUICKSORT_GEN_EXPAND(_TYPE_I)
typedef QUICKSORT_GEN_TYPE_2  _TYPE_I;

#undef  _TYPE_AD
#define _TYPE_AD  QUICKSORT_GEN_EXPAND(_TYPE_AD)
typedef QUICKSORT_GEN_TYPE_3  _TYPE_AD;


//==========================================================================================================================================
//------------------------------------------------------------------------------------------------------------------------------------------
//-                                                              Templates                                                                 -
//------------------------------------------------------------------------------------------------------------------------------------------
//==========================================================================================================================================


//==========================================================================================================================================
//= Partition
//==========================================================================================================================================


/* 'e' end index is inclusive.
 *
 * Expected partitioning outcome,
 * for s <= i <= e:
 *     i <  i_s  :  A[i] <= pivot
 *     i >= i_s  :  A[i] >= pivot
 */
#undef  quicksort_partition_serial
#define quicksort_partition_serial  QUICKSORT_GEN_EXPAND(quicksort_partition_serial)
long
quicksort_partition_serial(_TYPE_V * A, long s, long e, _TYPE_AD * aux_data)
{
	_TYPE_V pivot;
	long pivot_pos;
	long i_s, i_e;

	if (s == e)
		return s+1;

	pivot_pos = (s+e)/2;    // A somewhat better pivot than simply the first element. Also works well when already sorted.

	/* In order for the left part to be non-empty we always at least advance by a value equal to the pivot.
	 * In order for the right part to be non-empty the value at i_s always belongs to the right part.
	 */
	SWAP(&A[s], &A[pivot_pos]);
	pivot = A[s];
	i_s = s+1;
	i_e = e;

	while (1)
	{
		while (i_s < i_e && (quicksort_cmp(A[i_s], pivot, aux_data) <= 0))
			i_s++;
		while (i_s < i_e && (quicksort_cmp(A[i_e], pivot, aux_data) >= 0))
			i_e--;
		if (i_s >= i_e)
			break;
		SWAP(&A[i_s], &A[i_e]);
		i_s++;
		i_e--;
	}

	/* Since the value at 'i_s' always belongs to the right part,
	 * if the pivot is the max value we have to swap with it for when there are only two values in the segment.
	 */
	if (quicksort_cmp(pivot, A[i_s], aux_data) > 0)
		SWAP(&A[s], &A[i_s]);

	/* Balancing
	 *
	 * We need to address the possibility of mostly equal values, which would degrade complexity to O(N^2).
	 *
	 * Move 'i_s' towards the middle by swapping with values equal to the pivot.
	 * At position i_s: A[i_s] >= pivot.
	 */
	long i, m;
	double dist;
	m = (s+e)/2;
	if (m < s+1)
		return i_s;
	dist = labs(i_s - m);
	if (dist >= 0.4 * (e - s))  // If very imbalanced.
	{
		if (i_s > m)   // Left of i_s: A[i] <= pivot.
		{
			i = s;
			while (i_s > m && i < i_s)
			{
				if (quicksort_cmp(A[i_s-1], pivot, aux_data) == 0)
				{
					i_s--;
					continue;
				}
				if (quicksort_cmp(A[i], pivot, aux_data) == 0)
				{
					SWAP(&A[i], &A[i_s-1]);
					i_s--;
				}
				i++;
			}
		}
		else if (i_s < m) // From, and right of i_s: A[i] >= pivot.
		{
			i = e;
			while (i_s < m && i > i_s)
			{
				if (quicksort_cmp(A[i_s], pivot, aux_data) == 0)
				{
					i_s++;
					continue;
				}
				if (quicksort_cmp(A[i], pivot, aux_data) == 0)
				{
					SWAP(&A[i], &A[i_s]);
					i_s++;
				}
				i--;
			}
		}
	}

	return i_s;
}


/* 'i_end' end index is exclusive.
 */
#undef  quicksort_partition_non_equal
#define quicksort_partition_non_equal  QUICKSORT_GEN_EXPAND(quicksort_partition_non_equal)
static inline
void
quicksort_partition_non_equal(_TYPE_V pivot, _TYPE_V * A, _TYPE_V * buf, long i_start, long i_end, _TYPE_AD * aux_data, long * num_lower_out, long * num_equal_out, long * num_higher_out)
{
	long i, i_s, i_e;
	i_s = i_start;
	i_e = i_end;
	for (i=i_start;i<i_end;i++)
	{
		if (quicksort_cmp(A[i], pivot, aux_data) < 0)
			buf[i_s++] = A[i];
		else if (quicksort_cmp(A[i], pivot, aux_data) > 0)
			buf[--i_e] = A[i];
	}
	*num_lower_out = i_s - i_start;
	*num_higher_out = i_end - i_e;
	*num_equal_out = (i_end - i_start) - *num_lower_out - *num_higher_out;
}


/* 'i_end' end index is inclusive.
 *
 * Expected partitioning outcome,
 * for i_start <= i <= i_end:
 *     i <  m  :  buf[i] <= pivot
 *     i >= m  :  buf[i] >= pivot
 */
#undef  quicksort_partition_concurrent
#define quicksort_partition_concurrent  QUICKSORT_GEN_EXPAND(quicksort_partition_concurrent)
long
quicksort_partition_concurrent(_TYPE_V * A, _TYPE_V * buf, long i_start, long i_end, _TYPE_AD * aux_data)
{
	int num_threads = safe_omp_get_num_threads();
	int tnum = omp_get_thread_num();

	static long * t_lower_n;
	static long * t_higher_n;
	static long * t_equal_n;

	long lower_n, higher_n, equal_n;

	_TYPE_V pivot;
	long pivot_pos;
	long i, i_s, i_e, j;
	long l=0, h=0, e=0, el=0, eh=0, d, tmp;

	static long m = 0;

	if (i_start == i_end)
		return i_start + 1;

	pivot_pos = (i_start+i_end)/2;    // A somewhat better pivot than simply the first element. Also works well when already sorted.
	pivot = A[pivot_pos];

	i_end++;   // Make 'i_end' exclusive, to work with 'loop_partitioner_balance_iterations'.

	#pragma omp single nowait
	{
		t_lower_n = (typeof(t_lower_n)) malloc(num_threads * sizeof(*t_lower_n));
		t_higher_n = (typeof(t_higher_n)) malloc(num_threads * sizeof(*t_higher_n));
		t_equal_n = (typeof(t_equal_n)) malloc(num_threads * sizeof(*t_equal_n));
		for (i=0;i<num_threads;i++)
		{
			t_lower_n[i] = 0;
			t_higher_n[i] = 0;
			t_equal_n[i] = 0;
		}
	}

	#pragma omp barrier

	loop_partitioner_balance_iterations(num_threads, tnum, i_start, i_end, &i_s, &i_e);

	quicksort_partition_non_equal(pivot, A, buf, i_s, i_e, aux_data, &lower_n, &equal_n, &higher_n);

	t_lower_n[tnum] = lower_n;
	t_higher_n[tnum] = higher_n;
	t_equal_n[tnum] = equal_n;

	// printf("%d: %ld %ld %ld\n", tnum, t_lower_n[tnum], t_higher_n[tnum], t_equal_n[tnum]);
	#pragma omp barrier

	#pragma omp single nowait
	{
		for (i=0;i<num_threads;i++)
		{
			l += t_lower_n[i];
			h += t_higher_n[i];
			e += t_equal_n[i];
		}

		// In case 'e - d' is an odd number, favor the smallest part.
		// This ensures that no part is empty, unless when there is only one element (caught at the beginning).
		if (l < h)
		{
			d = h - l;
			el = (d < e) ? d + ((e - d + 1) / 2) : e;
			eh = e - el;
		}
		else
		{
			d = l - h;
			eh = (d < e) ? d + ((e - d + 1) / 2) : e;
			el = e - eh;
		}

		m = i_start + l + el;
		// printf("[%ld,%ld](%ld), pivot=%d, m=%ld, %ld, %ld, %ld(%ld,%ld)\n", i_start, i_end, i_end-i_start, ((int *) aux_data)[pivot], m, l, h, e, el, eh);

		h = i_start + l + e;
		e = i_start + l;
		l = i_start;
		for (i=0;i<num_threads;i++)
		{
			tmp = t_lower_n[i];
			t_lower_n[i] = l;
			l += tmp;

			tmp = t_equal_n[i];
			t_equal_n[i] = e;
			e += tmp;

			tmp = t_higher_n[i];
			t_higher_n[i] = h;
			h += tmp;
		}
		// printf("%d: %ld %ld\n", tnum, t_lower_n[tnum], t_higher_n[tnum]);
	}

	#pragma omp barrier

	j = t_lower_n[tnum];
	for (i=i_s;i<i_s+lower_n;i++)
		A[j++] = buf[i];

	j = t_equal_n[tnum];
	for (i=0;i<equal_n;i++)
		A[j++] = pivot;

	j = t_higher_n[tnum];
	for (i=i_e-higher_n;i<i_e;i++)
		A[j++] = buf[i];

	#pragma omp barrier

	#pragma omp single nowait
	{
		free(t_lower_n);
		free(t_higher_n);
		free(t_equal_n);
		// for (i=i_start;i<i_end;i++)
		// {
			// printf("%d, ", ((int *) aux_data)[buf[i]]);
		// }
		// printf("\n");
		// for (i=i_start;i<i_end;i++)
		// {
			// printf("%d, ", ((int *) aux_data)[A[i]]);
		// }
		// printf("\n");
	}

	#pragma omp barrier

	return m;
}


//==========================================================================================================================================
//= Quicksort
//==========================================================================================================================================


#undef  quicksort_no_malloc
#define quicksort_no_malloc  QUICKSORT_GEN_EXPAND(quicksort_no_malloc)
void
quicksort_no_malloc(_TYPE_V * A, long N, _TYPE_AD * aux_data, _TYPE_I * partitions)
{
	long s, m, e;
	long i;
	if (N < 2)
		return;
	s = 0;
	e = N - 1;
	m = 0;
	i = 0;
	while (1)
	{
		while (s >= e)
		{
			if (s == 0)
				return;
			i--;
			e--;
			s = partitions[i];
		}
		m = quicksort_partition_serial(A, s, e, aux_data);
		partitions[i++] = s;
		s = m;
	}
}


#undef  quicksort
#define quicksort  QUICKSORT_GEN_EXPAND(quicksort)
void
quicksort(_TYPE_V * A, long N, _TYPE_AD * aux_data)
{
	_TYPE_I * partitions;
	if (N < 2)
		return;
	partitions = (typeof(partitions)) malloc(N * sizeof(*partitions));
	quicksort_no_malloc(A, N, aux_data, partitions);
	free(partitions);
}


#undef  quicksort_no_malloc_parallel
#define quicksort_no_malloc_parallel  QUICKSORT_GEN_EXPAND(quicksort_no_malloc_parallel)
void
quicksort_no_malloc_parallel(_TYPE_V * A, _TYPE_V * buf, long N, _TYPE_AD * aux_data, _TYPE_I * partitions)
{
	long s, m, e;
	long i;
	if (N < 2)
		return;
	s = 0;
	e = N - 1;
	m = 0;
	i = 0;
	while (1)
	{
		while (s >= e)
		{
			if (s == 0)
				return;
			i--;
			e--;
			s = partitions[i];
		}
		if (__builtin_expect(e - s > 1 << 10, 0))
		{
			#pragma omp parallel
			{
				m = quicksort_partition_concurrent(A, buf, s, e, aux_data);
			}
		}
		else
		{
			m = quicksort_partition_serial(A, s, e, aux_data);
		}
		partitions[i++] = s;
		s = m;
	}
}


#undef  quicksort_parallel
#define quicksort_parallel  QUICKSORT_GEN_EXPAND(quicksort_parallel)
void
quicksort_parallel(_TYPE_V * A, long N, _TYPE_AD * aux_data)
{
	_TYPE_V * buf;
	_TYPE_I * partitions;
	if (N < 2)
		return;
	buf = (typeof(buf)) malloc(N * sizeof(*buf));
	partitions = (typeof(partitions)) malloc(N * sizeof(*partitions));
	quicksort_no_malloc_parallel(A, buf, N, aux_data, partitions);
	free(buf);
	free(partitions);
}


#undef  insertionsort
#define insertionsort  QUICKSORT_GEN_EXPAND(insertionsort)
void
insertionsort(_TYPE_V * A, long N, _TYPE_AD * aux_data)
{
	long i, i_e;
	for (i_e=N-1;i_e>0;i_e--)
	{
		for (i=0;i<i_e;i++)
		{
			if (quicksort_cmp(A[i], A[i+1], aux_data) > 0)
				SWAP(&A[i], &A[i+1]);
		}
	}
}

