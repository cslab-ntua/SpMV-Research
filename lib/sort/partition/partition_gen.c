#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

#include "macros/macrolib.h"
#include "debug.h"
#include "parallel_util.h"
#include "omp_functions.h"
#include "random.h"

#include "partition_gen.h"

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
#undef  partition_cmp
#define partition_cmp  PARTITION_GEN_EXPAND(partition_cmp)
static int partition_cmp(_TYPE_V a, _TYPE_V b, _TYPE_AD * aux_data);


//==========================================================================================================================================
//= Includes
//==========================================================================================================================================


//==========================================================================================================================================
//= Local Defines
//==========================================================================================================================================


#undef  _TYPE_V
#define _TYPE_V  PARTITION_GEN_EXPAND_TYPE(_TYPE_V)
typedef PARTITION_GEN_TYPE_1  _TYPE_V;

#undef  _TYPE_I
#define _TYPE_I  PARTITION_GEN_EXPAND_TYPE(_TYPE_I)
typedef PARTITION_GEN_TYPE_2  _TYPE_I;

#undef  _TYPE_AD
#define _TYPE_AD  PARTITION_GEN_EXPAND_TYPE(_TYPE_AD)
typedef PARTITION_GEN_TYPE_3  _TYPE_AD;


//==========================================================================================================================================
//------------------------------------------------------------------------------------------------------------------------------------------
//-                                                              Templates                                                                 -
//------------------------------------------------------------------------------------------------------------------------------------------
//==========================================================================================================================================


//==========================================================================================================================================
//= Partition Serial
//==========================================================================================================================================


/* 'i_end' end index is inclusive.
 *
 * Returns the first element of the right partition, or N+1 if it is empty.
 */
#undef  partition_serial_base
#define partition_serial_base  PARTITION_GEN_EXPAND(partition_serial_base)
__attribute__((always_inline))
static inline
long
partition_serial_base(_TYPE_V pivot, _TYPE_V * A, long i_start, long i_end, _TYPE_AD * aux_data)
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
		while ((i_s < i_e) && (partition_cmp(A[i_s], pivot, aux_data) < 0))
			i_s++;
		while ((i_s < i_e) && (partition_cmp(A[i_e], pivot, aux_data) > 0))
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
	if (partition_cmp(A[i_s], pivot, aux_data) < 0)
		i_s++;

	return i_s;
}


//------------------------------------------------------------------------------------------------------------------------------------------
//- Partition Serial - Exported Function
//------------------------------------------------------------------------------------------------------------------------------------------


/* 'i_end' end index is exclusive.
 */
#undef  partition_serial
#define partition_serial  PARTITION_GEN_EXPAND(partition_serial)
long
partition_serial(_TYPE_V pivot, _TYPE_V * A, long i_start, long i_end, _TYPE_AD * aux_data)
{
	i_end--;  // Make 'i_end' inclusive.
	return partition_serial_base(pivot, A, i_start, i_end, aux_data);
}


/* 'i_end' end index is exclusive.
 *
 * Returns the first element of the right partition, or N+1 if it is empty.
 *
 * The return_index will always be a valid position, so the right part (i >= return_index) will always be non-empty.
 */
#undef  partition_auto_serial
#define partition_auto_serial  PARTITION_GEN_EXPAND(partition_auto_serial)
long
partition_auto_serial(_TYPE_V * A, long i_start, long i_end, _TYPE_AD * aux_data)
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

	if (partition_cmp(A[i_start], A[i_end], aux_data) > 0)
		macros_swap(&A[i_start], &A[i_end]);
	if (N == 2)
		return i_end;

	if (partition_cmp(A[i_start], A[pivot_pos], aux_data) > 0)
		macros_swap(&A[i_start], &A[pivot_pos]);

	if (partition_cmp(A[pivot_pos], A[i_end], aux_data) > 0)
		macros_swap(&A[pivot_pos], &A[i_end]);

	if (N == 3)
		return pivot_pos;   // Equal to pivot (first element >= pivot).

	return partition_serial_base(A[pivot_pos], A, i_start+1, i_end-1, aux_data);
}


//==========================================================================================================================================
//= Partition Concurrent
//==========================================================================================================================================


//------------------------------------------------------------------------------------------------------------------------------------------
//- Partition Concurrent - With Extra Buffer
//------------------------------------------------------------------------------------------------------------------------------------------


/* 'i_end' end index is exclusive.
 */
#undef  partition_concurrent_non_equal
#define partition_concurrent_non_equal  PARTITION_GEN_EXPAND(partition_concurrent_non_equal)
__attribute__((always_inline))
static inline
void
partition_concurrent_non_equal(_TYPE_V pivot, _TYPE_V * A, _TYPE_V * buf, long i_start, long i_end, _TYPE_AD * aux_data, long * num_lower_out, long * num_equal_out, long * num_higher_out)
{
	long i, i_s, i_e;
	i_s = i_start;
	i_e = i_end;
	for (i=i_start;i<i_end;i++)
	{
		if (partition_cmp(A[i], pivot, aux_data) < 0)
			buf[i_s++] = A[i];
		else if (partition_cmp(A[i], pivot, aux_data) > 0)
			buf[--i_e] = A[i];
	}
	*num_lower_out = i_s - i_start;
	*num_higher_out = i_end - i_e;
	*num_equal_out = (i_end - i_start) - *num_lower_out - *num_higher_out;
}


/* 'i_end' end index is inclusive.
 */
#undef  partition_concurrent_base
#define partition_concurrent_base  PARTITION_GEN_EXPAND(partition_concurrent_base)
__attribute__((always_inline))
static inline
long
partition_concurrent_base(_TYPE_V pivot, _TYPE_V * A, _TYPE_V * buf, long i_start, long i_end, _TYPE_AD * aux_data)
{
	int num_threads = safe_omp_get_num_threads();
	int tnum = omp_get_thread_num();

	static int num_threads_prev = 0;
	static long * t_lower_n = NULL;
	static long * t_higher_n = NULL;
	static long * t_equal_n = NULL;

	long lower_n, higher_n, equal_n;

	long i, i_s, i_e, j;
	long l=0, h=0, e=0, el=0, eh=0, d, tmp;

	static long m = 0;

	if (i_start == i_end)
		return (partition_cmp(A[i_start], pivot, aux_data) < 0) ? i_start + 1 : i_start;
	if (i_start > i_end)
		error("invalid bounds, i_start > i_end : %ld > %ld", i_start, i_end);

	i_end++;   // Make 'i_end' exclusive, to work with 'loop_partitioner_balance_iterations'.

	#pragma omp barrier

	#pragma omp single nowait
	{
		if (num_threads_prev < num_threads)
		{
			num_threads_prev = num_threads;
			t_lower_n = (typeof(t_lower_n)) malloc(num_threads * sizeof(*t_lower_n));
			t_higher_n = (typeof(t_higher_n)) malloc(num_threads * sizeof(*t_higher_n));
			t_equal_n = (typeof(t_equal_n)) malloc(num_threads * sizeof(*t_equal_n));
		}
	}

	#pragma omp barrier

	loop_partitioner_balance_iterations(num_threads, tnum, i_start, i_end, &i_s, &i_e);

	partition_concurrent_non_equal(pivot, A, buf, i_s, i_e, aux_data, &lower_n, &equal_n, &higher_n);

	t_lower_n[tnum] = lower_n;
	t_higher_n[tnum] = higher_n;
	t_equal_n[tnum] = equal_n;

	// printf("%d: %ld %ld %ld\n", tnum, t_lower_n[tnum], t_higher_n[tnum], t_equal_n[tnum]);

	#pragma omp barrier

	#pragma omp single nowait
	{
		l=0;
		h=0;
		e=0;
		for (i=0;i<num_threads;i++)
		{
			l += t_lower_n[i];
			h += t_higher_n[i];
			e += t_equal_n[i];
		}

		if (l < h)
		{
			d = h - l;
			el = (d < e) ? d + ((e - d) / 2) : e;
			eh = e - el;
		}
		else
		{
			d = l - h;
			eh = (d < e) ? d + ((e - d) / 2) : e;
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

	return m;
}


//------------------------------------------------------------------------------------------------------------------------------------------
//- Partition Concurrent - Inplace
//------------------------------------------------------------------------------------------------------------------------------------------


/* 'i_end' end index is inclusive.
 *
 * This version of inplace partitioning moves more data than a version with block by block progression,
 * where each thread atomically takes a block from the start and the end and processes them.
 * At worst case, it passes all the data one more time.
 * On average, it passes half the data one more time.
 *
 * Nevertheless, it also utilizes all the memory channels, instead of one/two at a time,
 * so it can have 4x or more the memory bandwidth (the common case of 8 total channels vs 2 channels of start - end).
 * It also has better NUMA properties.
 */
#undef  partition_concurrent_inplace_base
#define partition_concurrent_inplace_base  PARTITION_GEN_EXPAND(partition_concurrent_inplace_base)
__attribute__((always_inline))
static inline
long
partition_concurrent_inplace_base(_TYPE_V pivot, _TYPE_V * A, long i_start, long i_end, _TYPE_AD * aux_data)
{
	__label__ partition_concurrent_out;
	int num_threads = safe_omp_get_num_threads();
	int tnum = omp_get_thread_num();

	static int num_threads_prev = 0;
	static long g_l;
	static long g_h;
	static long g_realoc_num_l = 0;
	static long g_realoc_num_h = 0;

	static long * t_s = NULL, * t_e = NULL, * t_m = NULL;

	static long * t_l_s = NULL, * t_l_e = NULL;
	static long * t_h_s = NULL, * t_h_e = NULL;

	long i, i_s, i_e, j, m;

	if (i_start == i_end)
		return (partition_cmp(A[i_start], pivot, aux_data) < 0) ? i_start+1 : i_start;
	if (i_start > i_end)
		error("invalid bounds, i_start > i_end : %ld > %ld", i_start, i_end);

	#pragma omp barrier

	#pragma omp single nowait
	{
		g_l = 0;
		g_h = 0;
		if (num_threads_prev < num_threads)
		{
			num_threads_prev = num_threads;
			t_l_s = (typeof(t_l_s)) malloc(num_threads * sizeof(*t_l_s));
			t_l_e = (typeof(t_l_e)) malloc(num_threads * sizeof(*t_l_e));
			t_h_s = (typeof(t_h_s)) malloc(num_threads * sizeof(*t_h_s));
			t_h_e = (typeof(t_h_e)) malloc(num_threads * sizeof(*t_h_e));

			t_s = (typeof(t_s)) malloc(num_threads * sizeof(*t_s));
			t_e = (typeof(t_e)) malloc(num_threads * sizeof(*t_e));
			t_m = (typeof(t_m)) malloc(num_threads * sizeof(*t_m));
		}
	}

	#pragma omp barrier

	i_end++;   // Make 'i_end' exclusive, to work with 'loop_partitioner_balance_iterations'.

	loop_partitioner_balance_iterations(num_threads, tnum, i_start, i_end, &i_s, &i_e);
	if (i_s != i_e)
		m = partition_serial_base(pivot, A, i_s, i_e - 1, aux_data);
	else
		m = i_s;
	if (m < i_s)
		error("test");
	if (m > i_e)
		error("test");

	__atomic_fetch_add(&g_l, m-i_s, __ATOMIC_RELAXED);
	__atomic_fetch_add(&g_h, i_e-m, __ATOMIC_RELAXED);
	t_s[tnum] = i_s;
	t_e[tnum] = i_e;
	t_m[tnum] = m;

	#pragma omp barrier

	#pragma omp single nowait
	{
		g_l += i_start;
		g_h += g_l;
	}

	#pragma omp barrier

	t_l_s[tnum] = t_l_e[tnum] = 0;
	t_h_s[tnum] = t_h_e[tnum] = 0;
	if (m > g_l)
	{
		t_l_s[tnum] = (i_s > g_l) ? i_s : g_l;
		t_l_e[tnum] = m;
	}
	else if (m < g_l)
	{
		t_h_s[tnum] = m;
		t_h_e[tnum] = (i_e < g_l) ? i_e : g_l;
	}

	#pragma omp barrier

	#pragma omp single nowait
	{
		g_realoc_num_l = 0;
		g_realoc_num_h = 0;
		for (i=0;i<num_threads;i++)
		{
			g_realoc_num_l += t_l_e[i] - t_l_s[i];
			g_realoc_num_h += t_h_e[i] - t_h_s[i];
		}
		// if (g_h - i_start != i_end - i_start)
			// error("g_h - g_l : %ld != %ld", g_h - g_l, i_end - i_start);
		if (g_realoc_num_l != g_realoc_num_h)
			error("low != high : %ld != %ld", g_realoc_num_l, g_realoc_num_h);
	}

	#pragma omp barrier

	long rel_pos_s=0, rel_pos_e=0;   // Positions if the chunks were concatenated.
	long realoc_num;
	long l_s=0, l_e=0;
	long h_s=0, h_e=0;
	long sum;
	loop_partitioner_balance_iterations(num_threads, tnum, 0, g_realoc_num_l, &rel_pos_s, &rel_pos_e);
	realoc_num = rel_pos_e - rel_pos_s;

	if (realoc_num == 0)
		goto partition_concurrent_out;

	i = 0;
	sum = t_l_e[i] - t_l_s[i];
	while (sum <= rel_pos_s)
	{
		i++;
		if (i >= num_threads)
			error("exceeded available realoc boundaries, lower part");
		sum += t_l_e[i] - t_l_s[i];
	}
	l_s = t_l_e[i] - (sum - rel_pos_s);
	l_e = t_l_e[i];

	j = 0;
	sum = t_h_e[j] - t_h_s[j];
	while (sum <= rel_pos_s)
	{
		j++;
		if (j >= num_threads)
			error("exceeded available realoc boundaries, higher part");
		sum += t_h_e[j] - t_h_s[j];
	}
	h_s = t_h_e[j] - (sum - rel_pos_s);
	h_e = t_h_e[j];

	// if (do_print)
	// {
		// printf("%2d: rel_pos_s=%2ld, rel_pos_e=%2ld, realoc_num=%2ld of %2ld : l_s=%2ld, h_s=%2ld\n", tnum, rel_pos_s, rel_pos_e, realoc_num, g_realoc_num_l, l_s, h_s);
	// }

	while (realoc_num > 0)
	{
		while (l_s == l_e)
		{
			i++;
			if (i >= num_threads)
				goto partition_concurrent_out;
			l_s = t_l_s[i];
			l_e = t_l_e[i];
			if (l_e - l_s > realoc_num)
				l_e = l_s + realoc_num;
		}
		while (h_s == h_e)
		{
			j++;
			if (j >= num_threads)
				goto partition_concurrent_out;
			h_s = t_h_s[j];
			h_e = t_h_e[j];
			if (h_e - h_s > realoc_num)
				h_e = h_s + realoc_num;
		}
		while (realoc_num > 0)
		{
			if ((l_s == l_e) || (h_s == h_e))
				break;
			macros_swap(&A[l_s], &A[h_s]);
			l_s++;
			h_s++;
			realoc_num--;
		}
	}

	partition_concurrent_out:;

	#pragma omp barrier

	return g_l;
}


//------------------------------------------------------------------------------------------------------------------------------------------
//- Partition Concurrent - Exported Functions
//------------------------------------------------------------------------------------------------------------------------------------------


/* 'i_end' end index is exclusive.
 */
#undef  partition_concurrent
#define partition_concurrent  PARTITION_GEN_EXPAND(partition_concurrent)
long
partition_concurrent(_TYPE_V pivot, _TYPE_V * A, long i_start, long i_end, _TYPE_AD * aux_data, const int inplace, _TYPE_V * buf)
{
	i_end--;  // Make 'i_end' inclusive.
	if (!inplace)
		return partition_concurrent_base(pivot, A, buf, i_start, i_end, aux_data);
	else
		return partition_concurrent_inplace_base(pivot, A, i_start, i_end, aux_data);
}


/* 'i_end' end index is exclusive.
 *
 * The return_index will always be a valid position, so the right part (i >= return_index) will always be non-empty.
 */
#undef  partition_auto_concurrent
#define partition_auto_concurrent  PARTITION_GEN_EXPAND(partition_auto_concurrent)
long
partition_auto_concurrent(_TYPE_V * A, long i_start, long i_end, _TYPE_AD * aux_data, const int inplace, _TYPE_V * buf)
{
	static __thread struct Random_State * rs = NULL;

	long pivot_pos;

	if (!inplace && buf == NULL)
		error("for non-inplace partitioning a buffer must be provided");

	if (i_start >= i_end)
		error("empty range, i_start >= i_end : %ld > %ld", i_start, i_end);

	if (__builtin_expect(rs == NULL, 0))
		rs = random_new(0);

	if (i_end - i_start == 1)
		return i_start;
	else if (i_end - i_start == 2)
	{
		#pragma omp single nowait
		{
			if (partition_cmp(A[i_start], A[i_start+1], aux_data) > 0)
				macros_swap(&A[i_start], &A[i_start+1]);
		}
		#pragma omp barrier
		return i_start + 1;
	}

	i_end--;  // Make 'i_end' inclusive.

	pivot_pos = (i_start+i_end)/2;    // A somewhat better pivot than simply the first element. Also works well when already sorted.

	#pragma omp single nowait
	{
		if (partition_cmp(A[i_start], A[i_end], aux_data) > 0)
			macros_swap(&A[i_start], &A[i_end]);
		if (partition_cmp(A[i_start], A[pivot_pos], aux_data) > 0)
			macros_swap(&A[i_start], &A[pivot_pos]);
		if (partition_cmp(A[pivot_pos], A[i_end], aux_data) > 0)
			macros_swap(&A[pivot_pos], &A[i_end]);
	}

	#pragma omp barrier

	if (!inplace)
		return partition_concurrent_base(A[pivot_pos], A, buf, i_start+1, i_end-1, aux_data);
	else
		return partition_concurrent_inplace_base(A[pivot_pos], A, i_start+1, i_end-1, aux_data);
}

