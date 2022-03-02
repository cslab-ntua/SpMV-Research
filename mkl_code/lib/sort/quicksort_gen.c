#include "quicksort_gen.h"


#ifndef QUICKSORT_GEN_C
#define QUICKSORT_GEN_C

#endif /* QUICKSORT_GEN_C */


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//------------------------------------------------------------------------------------------------------------------------------------------
//-                                                              Templates                                                                 -
//------------------------------------------------------------------------------------------------------------------------------------------
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


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
//= User Functions Declarations
//==========================================================================================================================================


/* compare(a, b)
 * increasing order:
 *	(a > b) ? 1 : (a < b) ? -1 : 0
 * decreasing order:
 *	(a < b) ? 1 : (a > b) ? -1 : 0
 */
#undef cmp
#define cmp  QUICKSORT_GEN_EXPAND(cmp)
static int cmp(_TYPE_V a, _TYPE_V b, _TYPE_AD * aux_data);


//==========================================================================================================================================
//= Generic Code
//==========================================================================================================================================


#undef swap
#define swap  QUICKSORT_GEN_EXPAND(swap)
static inline
void
swap(_TYPE_V * A, _TYPE_I i, _TYPE_I j)
{
	_TYPE_V tmp;
	tmp = A[j];
	A[j] = A[i];
	A[i] = tmp;
}


#undef partition
#define partition  QUICKSORT_GEN_EXPAND(partition)
static inline
_TYPE_I
partition(_TYPE_V * A, _TYPE_I s, _TYPE_I e, _TYPE_AD * aux_data)
{
	_TYPE_V pivot;
	swap(A, s, (s + e) / 2);    // A somewhat better pivot than simply the first element.
	pivot = A[s];
	_TYPE_I i_s = s+1, i_e = e;
	while (i_s < i_e)
	{
		if (cmp(A[i_s], pivot, aux_data) > 0)
		{
			swap(A, i_s, i_e);
			i_e--;
		}
		else
		{
			i_s++;
		}
	}
	if (cmp(A[s], A[i_s], aux_data) > 0)
		swap(A, s, i_s);
	return i_s;
}


#undef quicksort
#define quicksort  QUICKSORT_GEN_EXPAND(quicksort)
void
quicksort(_TYPE_V * A, _TYPE_I N, _TYPE_AD * aux_data)
{
	_TYPE_I * partitions;
	_TYPE_I s, m, e;
	_TYPE_I i;

	partitions = (typeof(partitions)) malloc(N * sizeof(*partitions));

	s = 0;
	e = N - 1;
	m = 0;
	i = 0;
	if (N < 2)
		return;
	while (1)
	{
		m = partition(A, s, e, aux_data);
		partitions[i++] = s;
		s = m;
		while (s >= e)
		{
			if (s == 0)
			{
				free(partitions);
				return;
			}
			i--;
			e--;
			s = partitions[i];
		}
	}
}

