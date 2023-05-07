#include <stdlib.h>
#include <stdio.h>

#include "macros/macrolib.h"

#include "quicksort_gen.h"


//==========================================================================================================================================
//= User Functions Declarations
//==========================================================================================================================================


/* compare(a, b)
 * increasing order:
 *	(a > b) ? 1 : (a < b) ? -1 : 0
 * decreasing order:
 *	(a < b) ? 1 : (a > b) ? -1 : 0
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


/* Expected partitioning outcome,
 * for s <= i <= e:
 *     i <  i_s  :  A[i] <= pivot
 *     i >= i_s  :  A[i] >= pivot
 */
#undef  partition
#define partition  QUICKSORT_GEN_EXPAND(partition)
static inline
long
partition(_TYPE_V * A, long s, long e, _TYPE_AD * aux_data)
{
	_TYPE_V pivot;
	long pivot_pos;
	long i_s, i_e;

	pivot_pos = (s+e)/2;    // A somewhat better pivot than simply the first element. Also works well when already sorted.

	/* We need to always advance at least by one.
	 * In order to do we can always at least advance by a value equal to the pivot.
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

	if (quicksort_cmp(pivot, A[i_s], aux_data) > 0)
		SWAP(&A[s], &A[i_s]);

	/* Balancing
	 *
	 * We need to address the possibility of mostly equal values, which would degrade to O(N^2).
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
		else if (i_s < m) // From and right of i_s: A[i] >= pivot.
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

		// if (e-s == 1)
		// {
			// if (quicksort_cmp(A[s], A[s+1], aux_data) > 0)
				// SWAP(&A[s], &A[s+1]);
			// i--;
			// e -= 2;
			// s = partitions[i];
			// continue;
		// }

		// if (e-s == 2)
		// {
			// if (quicksort_cmp(A[s], A[s+1], aux_data) > 0)
				// SWAP(&A[s], &A[s+1]);
			// if (quicksort_cmp(A[s+1], A[s+2], aux_data) > 0)
				// SWAP(&A[s+1], &A[s+2]);
			// if (quicksort_cmp(A[s], A[s+1], aux_data) > 0)
				// SWAP(&A[s], &A[s+1]);
			// i--;
			// e -= 3;
			// s = partitions[i];
			// continue;
		// }

		m = partition(A, s, e, aux_data);
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

