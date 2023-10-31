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
#define _TYPE_V  PARTITION_GEN_EXPAND(_TYPE_V)
typedef PARTITION_GEN_TYPE_1  _TYPE_V;

#undef  _TYPE_I
#define _TYPE_I  PARTITION_GEN_EXPAND(_TYPE_I)
typedef PARTITION_GEN_TYPE_2  _TYPE_I;

#undef  _TYPE_AD
#define _TYPE_AD  PARTITION_GEN_EXPAND(_TYPE_AD)
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
 * Expected partitioning outcome,
 * for i_start <= i <= i_end:
 *     i <  i_s  :  A[i] <= pivot
 *     i >= i_s  :  A[i] >= pivot
 */

#if 0
#undef  partition_serial_base
#define partition_serial_base  PARTITION_GEN_EXPAND(partition_serial_base)
__attribute__((always_inline))
static inline
long
partition_serial_base(_TYPE_V pivot, _TYPE_V * A, long i_start, long i_end, _TYPE_AD * aux_data)
{
	long i_s, i_e;
	long cmp, balance;

	if (i_start == i_end)
		return (partition_cmp(A[i_start], pivot, aux_data) < 0) ? i_start+1 : i_start;
	if (i_start > i_end)
		error("invalid bounds, i_start > i_end : %ld > %ld", i_start, i_end);

	i_s = i_start;
	i_e = i_end;

	balance = 0;
	while (1)
	{
		while (i_s < i_e)
		{
			cmp = partition_cmp(A[i_s], pivot, aux_data);
			if ((cmp > 0) || (cmp == 0 && balance > 0))
				break;
			i_s++;
			balance++;
		}
		while (i_s < i_e)
		{
			cmp = partition_cmp(A[i_e], pivot, aux_data);
			if ((cmp < 0) || (cmp == 0 && balance < 0))
				break;
			i_e--;
			balance--;
		}
		if (i_s >= i_e)
			break;
		SWAP(&A[i_s], &A[i_e]);
		i_s++;
		i_e--;
	}

	// If i_s == i_e then this element has not been examined.
	// If all elements are lower than the pivot then return a position after the last one.
	if (partition_cmp(A[i_s], pivot, aux_data) < 0)
		i_s++;

	return i_s;
}
#endif


#undef  partition_serial_base
#define partition_serial_base  PARTITION_GEN_EXPAND(partition_serial_base)
__attribute__((always_inline))
static inline
long
partition_serial_base(_TYPE_V pivot, _TYPE_V * A, long i_start, long i_end, _TYPE_AD * aux_data)
{
	long i_s, i_e;

	// if (i_start == i_end)
		// return (partition_cmp(A[i_start], pivot, aux_data) < 0) ? i_start+1 : i_start;
	if (i_start > i_end)
		error("invalid bounds, i_start > i_end : %ld > %ld", i_start, i_end);

	i_s = i_start;
	i_e = i_end;

	while (1)
	{
		while ((i_s < i_e) && (partition_cmp(A[i_s], pivot, aux_data) < 0))
			i_s++;
		while ((i_s < i_e) && (partition_cmp(A[i_e], pivot, aux_data) > 0))
			i_e--;
		if (i_s >= i_e)
			break;
		SWAP(&A[i_s], &A[i_e]);
		i_s++;
		i_e--;
	}

	// If i_s == i_e then this element has not been examined.
	// If all elements are lower than the pivot then return a position after the last one.
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
 */
#undef  partition_auto_serial
#define partition_auto_serial  PARTITION_GEN_EXPAND(partition_auto_serial)
long
partition_auto_serial(_TYPE_V * A, long i_start, long i_end, _TYPE_AD * aux_data, const int non_empty_left_seg)
{
	static __thread struct Random_State * rs = NULL;

	long m, pivot_pos;
	_TYPE_V pivot;

	if (i_start >= i_end)
		error("empty range, i_start >= i_end : %ld > %ld", i_start, i_end);

	if (__builtin_expect(rs == NULL, 0))
		rs = random_new(0);

	i_end--;  // Make 'i_end' inclusive.

	pivot_pos = (i_start+i_end)/2;    // A somewhat better pivot than simply the first element. Also works well when already sorted.
	// pivot_pos = random_uniform_integer_32bit(rs, i_start, i_end);
	// pivot_pos = random_xs_uniform_integer_64bit(rs, i_start, i_end);
	pivot = A[pivot_pos];

	if (!non_empty_left_seg)
		return partition_serial_base(pivot, A, i_start, i_end, aux_data);

	/* In order for the left part to be non-empty we always at least advance by an element equal to the pivot by reserving it at the left.
	 * In order for the right part to be non-empty, if the pivot is the maximum value we swap the reserved element at the end.
	 */
	SWAP(&A[i_start], &A[pivot_pos]);

	m = partition_serial_base(pivot, A, i_start+1, i_end, aux_data);

	/* Since the position at 'm' always be the first of the right part,
	 * we swap the reserved pivot at the beginning with the position m-1, for when the pivot is the max value and the right part is empty.
	 */
	SWAP(&A[i_start], &A[m-1]);

	/* Since the value at 'm' always belongs to the right part,
	 * if the pivot is the max value we have to swap with it for when there are only two values in the segment.
	 */
	// if (m > i_end)
		// m--;
	// if (partition_cmp(A[m], pivot, aux_data) < 0)
		// SWAP(&A[i_start], &A[m]);
	return m;
}


/* 'i_end' end index is exclusive.
 */
#if 0
#undef  partition_serial_random
#define partition_serial_random  PARTITION_GEN_EXPAND(partition_serial_random)
long
partition_serial_random(struct random_data * state, _TYPE_V * A, long i_start, long i_end, _TYPE_AD * aux_data)
{
	int32_t pivot_pos;
	long m;
	_TYPE_V pivot;

	if (i_start >= i_end)
		error("empty range, i_start >= i_end : %ld > %ld", i_start, i_end);

	i_end--;  // Make 'i_end' inclusive.

	random_r(state, &pivot_pos);
	pivot_pos %= i_end - i_start;
	pivot_pos += i_start;


	/* In order for the left part to be non-empty we always at least advance by an element equal to the pivot by reserving it at the left.
	 * In order for the right part to be non-empty, if the pivot is the maximum value we swap the reserved element at the end.
	 */
	pivot = A[pivot_pos];
	SWAP(&A[i_start], &A[pivot_pos]);

	m = partition_serial_base(pivot, A, i_start+1, i_end, aux_data);

	/* Since the position at 'm' always be the first of the right part,
	 * we swap the reserved pivot at the beginning with the position m-1, for when the pivot is the max value and the right part is empty.
	 */
	SWAP(&A[i_start], &A[m-1]);

	return m;
}
#endif


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
 *
 * Expected partitioning outcome,
 * for i_start <= i <= i_end:
 *     i <  m  :  buf[i] <= pivot
 *     i >= m  :  buf[i] >= pivot
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

		// if (do_print)
		// {
			// printf("g_realoc_num_l=%ld , g_realoc_num_h=%ld\n", g_realoc_num_l, g_realoc_num_h);
		// }

		// if (g_h - i_start != i_end - i_start)
			// error("g_h - g_l : %ld != %ld", g_h - g_l, i_end - i_start);
		if (g_realoc_num_l != g_realoc_num_h)
			error("low != high : %ld != %ld", g_realoc_num_l, g_realoc_num_h);

		// if (do_print)
		// {
			// printf("pivot=%d\n", pivot);
			// printf("g_l=%ld\n", g_l);
			// long tmp_l = 0;
			// long tmp_h = 0;
			// for (long i=0;i<num_threads;i++)
			// {
				// printf("%2ld: %10ld %10ld %10ld - l=[%10ld, %10ld] , h=[%10ld, %10ld]\n", i, t_s[i], t_m[i], t_e[i], t_l_s[i], t_l_e[i], t_h_s[i], t_h_e[i]);
				// tmp_l += t_m[i] - t_s[i];
				// tmp_h += t_e[i] - t_m[i];
			// }
			// printf("l_all=%ld h_all=%ld total_all=%ld\n", tmp_l, tmp_h, tmp_l+tmp_h);
			// tmp_l = 0;
			// tmp_h = 0;
			// for (long i=0;i<num_threads;i++)
			// {
				// tmp_l += t_l_e[i] - t_l_s[i];
				// tmp_h += t_h_e[i] - t_h_s[i];
			// }
			// printf("l=%ld h=%ld total=%ld\n", tmp_l, tmp_h, tmp_l+tmp_h);
		// }

		// if (do_print)
		// {
			// printf("AFTER LOCAL REORDER\n");
			// long N = atoi(getenv("N"));
			// printf("g_l = %ld\n", g_l);
			// printf(" ");
			// for (long i=0;i<N;i++)
			// {
				// printf("%2ld  ", i);
			// }
			// printf("\n");
			// printf("[");
			// for (long i=0;i<N;i++)
			// {
				// printf("%2d, ", A[i]);
			// }
			// printf("]\n");
			// printf("\n");
		// }

	}

	#pragma omp barrier

	long rel_pos_s=0, rel_pos_e=0;   // Positions if the chunks were concatenated.
	long realoc_num;
	long l_s=0, l_e=0;
	long h_s=0, h_e=0;
	long sum;
	loop_partitioner_balance_iterations(num_threads, tnum, 0, g_realoc_num_l, &rel_pos_s, &rel_pos_e);
	realoc_num = rel_pos_e - rel_pos_s;

	i = 0;
	sum = t_l_e[i] - t_l_s[i];
	while (sum <= rel_pos_s)
	{
		i++;
		sum += t_l_e[i] - t_l_s[i];
	}
	l_s = t_l_e[i] - (sum - rel_pos_s);
	l_e = t_l_e[i];

	j = 0;
	sum = t_h_e[j] - t_h_s[j];
	while (sum <= rel_pos_s)
	{
		j++;
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
			SWAP(&A[l_s], &A[h_s]);
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
//- Partition Concurrent - Exported Function
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
 */
#undef  partition_auto_concurrent
#define partition_auto_concurrent  PARTITION_GEN_EXPAND(partition_auto_concurrent)
long
partition_auto_concurrent(_TYPE_V * A, long i_start, long i_end, _TYPE_AD * aux_data, const int non_empty_left_seg, const int inplace, _TYPE_V * buf)
{
	static __thread struct Random_State * rs = NULL;

	long m, pivot_pos;
	_TYPE_V pivot;

	if (!inplace && buf == NULL)
		error("for non-inplace partitioning a buffer must be provided");

	if (i_start >= i_end)
		error("empty range, i_start >= i_end : %ld > %ld", i_start, i_end);

	if (__builtin_expect(rs == NULL, 0))
		rs = random_new(0);

	i_end--;  // Make 'i_end' inclusive.

	pivot_pos = (i_start+i_end)/2;    // A somewhat better pivot than simply the first element. Also works well when already sorted.
	// pivot_pos = random_uniform_integer_32bit(rs, i_start, i_end);
	// pivot_pos = random_xs_uniform_integer_64bit(rs, i_start, i_end);
	pivot = A[pivot_pos];

	if (!non_empty_left_seg)
	{
		if (!inplace)
			return partition_concurrent_base(pivot, A, buf, i_start, i_end, aux_data);
		else
			return partition_concurrent_inplace_base(pivot, A, i_start, i_end, aux_data);
	}

	/* In order for the left part to be non-empty we always at least advance by an element equal to the pivot by reserving it at the left.
	 * In order for the right part to be non-empty, if the pivot is the maximum value we swap the reserved element at the end.
	 */
	#pragma omp barrier
	#pragma omp single nowait
	{
		SWAP(&A[i_start], &A[pivot_pos]);
	}
	#pragma omp barrier

	if (!inplace)
		m = partition_concurrent_base(pivot, A, buf, i_start+1, i_end, aux_data);
	else
		m = partition_concurrent_inplace_base(pivot, A, i_start+1, i_end, aux_data);

	/* Since the value at 'm' always belongs to the right part,
	 * if the pivot is the max value we have to swap with it for when there are only two values in the segment.
	 */
	// if (m > i_end)
		// m--;

	#pragma omp barrier
	#pragma omp single nowait
	{
		SWAP(&A[i_start], &A[m-1]);
		// if (partition_cmp(A[m], pivot, aux_data) < 0)
			// SWAP(&A[i_start], &A[m]);
	}
	#pragma omp barrier

	return m;
}

