#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

#include "macros/macrolib.h"
#include "debug.h"
#include "parallel_util.h"
#include "omp_functions.h"
#include "random.h"

#include "partition_blocked_gen.h"

// static long do_print = 0;
// static long do_print = 1;

// static long exit_after_one = 0;
// static long exit_after_one = 1;


/* The exported functions take an exclusive 'i_end' index.
 *
 * Partitioning outcome, for i_start <= i < i_end:
 *     i <  return_index  :  A[i] <= pivot
 *     i >= return_index  :  A[i] >= pivot
 *
 * The return_index will always be a valid position, so the right part (i >= return_index) will always be non-empty.
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
#undef  partition_blocked_cmp
#define partition_blocked_cmp  PARTITION_BLOCKED_GEN_EXPAND(partition_blocked_cmp)
static int partition_blocked_cmp(_TYPE_V a, _TYPE_V b, _TYPE_AD * aux_data);

/* Partition left  : element <= pivot
 * Partition right : element >= pivot
 *
 * 'place_equal_right' : Where to put the equal to the pivot values.
 *                         Avoids complexity degradation for data with very few unique values by alternating the placement of equal values.
 */
#undef  partition_blocked_buffer_and_partition_block
#define partition_blocked_buffer_and_partition_block  PARTITION_BLOCKED_GEN_EXPAND(partition_blocked_buffer_and_partition_block)
static void partition_blocked_buffer_and_partition_block(_TYPE_V pivot, _TYPE_V * A, long i_start, _TYPE_AD * aux_data, const long place_equal_right, _TYPE_V * buf_out, long * i_buf_l_ptr, long * i_buf_r_ptr);


//==========================================================================================================================================
//= Includes
//==========================================================================================================================================


//==========================================================================================================================================
//= Local Defines
//==========================================================================================================================================


#undef  _TYPE_V
#define _TYPE_V  PARTITION_BLOCKED_GEN_EXPAND_TYPE(_TYPE_V)
typedef PARTITION_BLOCKED_GEN_TYPE_1  _TYPE_V;

#undef  _TYPE_I
#define _TYPE_I  PARTITION_BLOCKED_GEN_EXPAND_TYPE(_TYPE_I)
typedef PARTITION_BLOCKED_GEN_TYPE_2  _TYPE_I;

#undef  _TYPE_AD
#define _TYPE_AD  PARTITION_BLOCKED_GEN_EXPAND_TYPE(_TYPE_AD)
typedef PARTITION_BLOCKED_GEN_TYPE_3  _TYPE_AD;

#undef  _VEC_LEN
#define _VEC_LEN  PARTITION_BLOCKED_GEN_VECTOR_LEN


//==========================================================================================================================================
//------------------------------------------------------------------------------------------------------------------------------------------
//-                                                              Templates                                                                 -
//------------------------------------------------------------------------------------------------------------------------------------------
//==========================================================================================================================================


//------------------------------------------------------------------------------------------------------------------------------------------
//- Partition Small Serial
//------------------------------------------------------------------------------------------------------------------------------------------


/* 'i_end' end index is inclusive.
 *
 * Returns the first element of the right partition, or N+1 if it is empty.
 */
#undef  partition_small_serial
#define partition_small_serial  PARTITION_BLOCKED_GEN_EXPAND(partition_small_serial)
__attribute__((always_inline))
static inline
long
partition_small_serial(_TYPE_V pivot, _TYPE_V * A, long i_start, long i_end, _TYPE_AD * aux_data)
{
	long i_s, i_e;

	if (i_start > i_end)
		error("invalid bounds, i_start > i_end : %ld > %ld", i_start, i_end);

	i_s = i_start;
	i_e = i_end;

	/* By always swapping and not ignoring equal to pivot values we
	 * get much better balanced partitioning when nearly all the values are the same
	 * and escape the algorithmic complexity degradation (of O(N^2)) for sorting algorithms.
	 *
	 * Testing showed this is generally faster than balancing the equal to pivot values.
	 */
	while (1)
	{
		while ((i_s < i_e) && (partition_blocked_cmp(A[i_s], pivot, aux_data) < 0))
			i_s++;
		while ((i_s < i_e) && (partition_blocked_cmp(A[i_e], pivot, aux_data) > 0))
			i_e--;
		if (i_s >= i_e)
			break;
		macros_swap(&A[i_s], &A[i_e]);
		i_s++;
		i_e--;
	}

	/* If i_s == i_e then this element has not been examined.
	 * If all elements are lower than the pivot then return a position after the last one.
	 */
	if (partition_blocked_cmp(A[i_s], pivot, aux_data) < 0)
		i_s++;

	return i_s;
}


//------------------------------------------------------------------------------------------------------------------------------------------
//- Partition Block Scalar
//------------------------------------------------------------------------------------------------------------------------------------------


/* 'i_end' is inclusive.
 * 'i_buf_r_ptr' grows to the left.
 */
#undef  partition_blocked_scalar
#define partition_blocked_scalar  PARTITION_BLOCKED_GEN_EXPAND(partition_blocked_scalar)
__attribute__((always_inline))
static inline
void
partition_blocked_scalar(_TYPE_V pivot, _TYPE_V * A, long i_start, long N, _TYPE_AD * aux_data, _TYPE_V * buf_out, long * i_buf_l_ptr, long * i_buf_r_ptr)
{
	long cmp;
	long i;
	for (i=0;i<N;i++)
	{
		cmp = partition_blocked_cmp(A[i_start + i], pivot, aux_data);
		if (cmp < 0)
		{
			buf_out[*i_buf_l_ptr] = A[i_start + i];
			(*i_buf_l_ptr)++;
		}
		else
		{
			buf_out[*i_buf_r_ptr] = A[i_start + i];
			(*i_buf_r_ptr)--;
		}
	}
}


//==========================================================================================================================================
//= Partition Blocked Serial
//==========================================================================================================================================


/* 'i_end' is inclusive.
 *
 * Returns the first element of the right partition, or N+1 if it is empty.
 */
#undef  partition_blocked_serial_base
#define partition_blocked_serial_base  PARTITION_BLOCKED_GEN_EXPAND(partition_blocked_serial_base)
__attribute__((always_inline))
static inline
long
partition_blocked_serial_base(_TYPE_V pivot, _TYPE_V * A, long i_start, long i_end, _TYPE_AD * aux_data)
{
	if (i_start > i_end)
		error("invalid bounds, i_start > i_end : %ld > %ld", i_start, i_end);

	long N = i_end - i_start + 1;   // i_end is inclusive
	long N_mod = N % _VEC_LEN;
	long N_div = N / _VEC_LEN;
	long buf_n = 3*_VEC_LEN + N_mod;   // We need 3x because 'partition_blocked_buffer_and_partition_block' always writes at both ends (garbage can overwrite data).
	_TYPE_V buf[buf_n + 100];
	long i_l, i_r, i_write_l, i_write_r, i_write_buf_l, i_write_buf_r;
	long i;

	if (N < 2 * _VEC_LEN)
	{
		return partition_small_serial(pivot, A, i_start, i_end, aux_data);
	}

	i_l = i_start;
	i_r = i_end;   // i_end is inclusive
	i_write_l = i_l;
	i_write_r = i_r;

	i_write_buf_l = 0;
	i_write_buf_r = buf_n - 1;
	partition_blocked_buffer_and_partition_block(pivot, A, i_l, aux_data, 1, buf, &i_write_buf_l, &i_write_buf_r);
	// printf("i_write_buf_l=%ld i_write_buf_r=%ld\n", i_write_buf_l, i_write_buf_r);
	i_l += _VEC_LEN;
	partition_blocked_buffer_and_partition_block(pivot, A, i_r - _VEC_LEN + 1, aux_data, 0, buf, &i_write_buf_l, &i_write_buf_r);
	// printf("i_write_buf_l=%ld i_write_buf_r=%ld\n", i_write_buf_l, i_write_buf_r);
	i_r -= _VEC_LEN;

	for (i=0;i<N_div-2;i++)   // 2 blocks already processed
	{
		if (i_l - i_write_l < _VEC_LEN)
		{
			partition_blocked_buffer_and_partition_block(pivot, A, i_l, aux_data, 1, A, &i_write_l, &i_write_r);
			i_l += _VEC_LEN;
		}
		else
		{
			partition_blocked_buffer_and_partition_block(pivot, A, i_r - _VEC_LEN + 1, aux_data, 0, A, &i_write_l, &i_write_r);
			i_r -= _VEC_LEN;
		}
	}

	partition_blocked_scalar(pivot, A, i_l, N_mod, aux_data, buf, &i_write_buf_l, &i_write_buf_r);

	// printf("i_write_l=%ld\n", i_write_l);
	// for (i=0;i<buf_n;i++)
		// printf("buf[%ld]=%2.0lf ", i, buf[i]);
	// printf("\n");
	for (i=0;i<i_write_buf_l;i++,i_write_l++)
		A[i_write_l] = buf[i];
	for (i=buf_n-1;i>i_write_buf_r;i--,i_write_r--)
		A[i_write_r] = buf[i];

	return i_write_l;
}


//------------------------------------------------------------------------------------------------------------------------------------------
//- Partition Serial - Exported Function
//------------------------------------------------------------------------------------------------------------------------------------------


/* 'i_end' end index is exclusive.
 */
#undef  partition_blocked_serial
#define partition_blocked_serial  PARTITION_BLOCKED_GEN_EXPAND(partition_blocked_serial)
long
partition_blocked_serial(_TYPE_V pivot, _TYPE_V * A, long i_start, long i_end, _TYPE_AD * aux_data)
{
	i_end--;  // Make 'i_end' inclusive.
	return partition_blocked_serial_base(pivot, A, i_start, i_end, aux_data);
}


/* 'i_end' end index is exclusive.
 *
 * Returns the first element of the right partition, or N+1 if it is empty.
 *
 * The return_index will always be a valid position, so the right part (i >= return_index) will always be non-empty.
 */
#undef  partition_blocked_auto_serial
#define partition_blocked_auto_serial  PARTITION_BLOCKED_GEN_EXPAND(partition_blocked_auto_serial)
long
partition_blocked_auto_serial(_TYPE_V * A, long i_start, long i_end, _TYPE_AD * aux_data)
{
	long pivot_pos;
	long N = i_end - i_start;

	if (i_start >= i_end)
		error("empty range, i_start >= i_end : %ld > %ld", i_start, i_end);

	if (N == 1)
		return i_start;

	// Make 'i_end' inclusive.
	i_end--;

	pivot_pos = (i_start + i_end) / 2;    // A somewhat better pivot than simply the first element. Also works well when already sorted.

	// Pivot is the median of the 3 elements: A[i_start], A[pivot_pos], A[i_end].

	if (partition_blocked_cmp(A[i_start], A[i_end], aux_data) > 0)
		macros_swap(&A[i_start], &A[i_end]);
	if (N == 2)
		return i_end;

	if (partition_blocked_cmp(A[i_start], A[pivot_pos], aux_data) > 0)
		macros_swap(&A[i_start], &A[pivot_pos]);

	if (partition_blocked_cmp(A[pivot_pos], A[i_end], aux_data) > 0)
		macros_swap(&A[pivot_pos], &A[i_end]);

	if (N == 3)
		return pivot_pos;   // Equal to pivot (first element >= pivot).

	return partition_blocked_serial_base(A[pivot_pos], A, i_start+1, i_end-1, aux_data);
}

