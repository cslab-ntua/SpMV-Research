#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

#include "macros/macrolib.h"
#include "debug.h"
#include "parallel_util.h"
#include "omp_functions.h"

#include "quicksort_blocked_gen.h"

// static long do_print = 0;
// static long do_print = 1;

// static long exit_after_one = 0;
// static long exit_after_one = 1;


/* Quicksort sorts inplace.
 *
 * If we sort the indices of an array A (0, ..., len(A)-1) then the result
 * will be the REVERSE permutation:
 *     A[reverse_permutation[i]] -> A_sorted[i]
 *     i.e. to sort the array: A_sorted[i] = A[reverse_permutation[i]];
 *
 */


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
#undef  quicksort_blocked_cmp
#define quicksort_blocked_cmp  QUICKSORT_BLOCKED_GEN_EXPAND(quicksort_blocked_cmp)
static int quicksort_blocked_cmp(_TYPE_V a, _TYPE_V b, _TYPE_AD * aux_data);

#undef  quicksort_blocked_buffer_and_partition_block
#define quicksort_blocked_buffer_and_partition_block  QUICKSORT_BLOCKED_GEN_EXPAND(quicksort_blocked_buffer_and_partition_block)
static void quicksort_blocked_buffer_and_partition_block(_TYPE_V pivot, _TYPE_V * A, long i_start, _TYPE_AD * aux_data, const long equal_to_right_part, _TYPE_V * buf_out, long * i_buf_l_ptr, long * i_buf_r_ptr);


//==========================================================================================================================================
//= Includes
//==========================================================================================================================================


#include "sort/partition_blocked/partition_blocked_gen_push.h"
#define PARTITION_BLOCKED_GEN_TYPE_1  QUICKSORT_BLOCKED_GEN_TYPE_1
#define PARTITION_BLOCKED_GEN_TYPE_2  QUICKSORT_BLOCKED_GEN_TYPE_2
#define PARTITION_BLOCKED_GEN_TYPE_3  QUICKSORT_BLOCKED_GEN_TYPE_3
#define PARTITION_BLOCKED_GEN_VECTOR_LEN  QUICKSORT_BLOCKED_GEN_VECTOR_LEN
#define PARTITION_BLOCKED_GEN_SUFFIX  CONCAT(_QUICKSORT_BLOCKED_GEN, QUICKSORT_BLOCKED_GEN_SUFFIX)
#include "sort/partition_blocked/partition_blocked_gen.c"
static inline
int
partition_blocked_cmp(_TYPE_V a, _TYPE_V b, _TYPE_AD * aux_data)
{
	return quicksort_blocked_cmp(a, b, aux_data);
}
#undef  partition_blocked_buffer_and_partition_block
#define partition_blocked_buffer_and_partition_block  PARTITION_BLOCKED_GEN_EXPAND(partition_blocked_buffer_and_partition_block)
static inline
void
partition_blocked_buffer_and_partition_block(_TYPE_V pivot, _TYPE_V * A, long i_start, _TYPE_AD * aux_data, const long equal_to_right_part, _TYPE_V * buf_out, long * i_buf_l_ptr, long * i_buf_r_ptr)
{
	quicksort_blocked_buffer_and_partition_block(pivot, A, i_start, aux_data, equal_to_right_part, buf_out, i_buf_l_ptr, i_buf_r_ptr);
}


//==========================================================================================================================================
//= Local Defines
//==========================================================================================================================================


#undef  _TYPE_V
#define _TYPE_V  QUICKSORT_BLOCKED_GEN_EXPAND_TYPE(_TYPE_V)
typedef QUICKSORT_BLOCKED_GEN_TYPE_1  _TYPE_V;

#undef  _TYPE_I
#define _TYPE_I  QUICKSORT_BLOCKED_GEN_EXPAND_TYPE(_TYPE_I)
typedef QUICKSORT_BLOCKED_GEN_TYPE_2  _TYPE_I;

#undef  _TYPE_AD
#define _TYPE_AD  QUICKSORT_BLOCKED_GEN_EXPAND_TYPE(_TYPE_AD)
typedef QUICKSORT_BLOCKED_GEN_TYPE_3  _TYPE_AD;


//==========================================================================================================================================
//------------------------------------------------------------------------------------------------------------------------------------------
//-                                                              Templates                                                                 -
//------------------------------------------------------------------------------------------------------------------------------------------
//==========================================================================================================================================


//==========================================================================================================================================
//= Quicksort
//==========================================================================================================================================


/* We use a DFS-like traversal for better cache utilization. */
#undef  quicksort_blocked_no_malloc
#define quicksort_blocked_no_malloc  QUICKSORT_BLOCKED_GEN_EXPAND(quicksort_blocked_no_malloc)
static inline
void
quicksort_blocked_no_malloc(_TYPE_V * A, long N, _TYPE_AD * aux_data, _TYPE_I * partitions_buf)
{
	long s, m, e;
	long i;

	size_t statelen = 256;
	char statebuf[statelen];
	struct random_data buf;

	if (N < 2)
		return;

	buf.state = NULL;
	initstate_r(0, statebuf, statelen, &buf);         // Before calling this function, the buf.state field must be initialized to NULL.
	srandom_r(0, &buf);

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
			s = partitions_buf[i];
		}
		m = partition_blocked_auto_serial(A, s, e+1, aux_data);
		partitions_buf[i++] = s;
		s = m;
	}
}


#undef  quicksort_blocked
#define quicksort_blocked  QUICKSORT_BLOCKED_GEN_EXPAND(quicksort_blocked)
void
quicksort_blocked(_TYPE_V * A, long N, _TYPE_AD * aux_data, _TYPE_I * partitions_buf)
{
	_TYPE_I * partitions_buf_tmp;
	if (N < 2)
		return;
	partitions_buf_tmp = (partitions_buf != NULL) ? partitions_buf : (typeof(partitions_buf_tmp)) malloc(N * sizeof(*partitions_buf_tmp));
	quicksort_blocked_no_malloc(A, N, aux_data, partitions_buf_tmp);
	if (partitions_buf == NULL)
		free(partitions_buf_tmp);
}

