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


#undef  partition
#define partition  QUICKSORT_GEN_EXPAND(partition)
static inline
_TYPE_I
partition(_TYPE_V * A, _TYPE_I s, _TYPE_I e, _TYPE_AD * aux_data)
{
	_TYPE_V pivot;
	_TYPE_I pivot_pos;

	pivot_pos = (s+e)/2;    // A somewhat better pivot than simply the first element. Also works well when already sorted.
	// pivot_pos = s + ((s ^ (e<<4)) % (e-s));

	SWAP(&A[s], &A[pivot_pos]);
	pivot = A[s];
	_TYPE_I i_s = s+1, i_e = e;

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

	if (quicksort_cmp(A[s], A[i_s], aux_data) > 0)
		SWAP(&A[s], &A[i_s]);
	return i_s;
}


#undef  insertionsort
#define insertionsort  QUICKSORT_GEN_EXPAND(insertionsort)
void
insertionsort(_TYPE_V * A, _TYPE_I N, _TYPE_AD * aux_data)
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
quicksort_no_malloc(_TYPE_V * A, _TYPE_I N, _TYPE_AD * aux_data, _TYPE_I * partitions)
{
	_TYPE_I s, m, e;
	_TYPE_I i;

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
quicksort(_TYPE_V * A, _TYPE_I N, _TYPE_AD * aux_data)
{
	_TYPE_I * partitions;
	if (N < 2)
		return;
	partitions = (typeof(partitions)) malloc(N * sizeof(*partitions));
	quicksort_no_malloc(A, N, aux_data, partitions);
	free(partitions);
}

