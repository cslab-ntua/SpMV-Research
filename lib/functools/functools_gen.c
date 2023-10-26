#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <omp.h>

#include "macros/macrolib.h"
#include "parallel_util.h"
#include "omp_functions.h"
#include "time_it.h"
// #include "time_it_tsc.h"

#include "functools_gen.h"


/* Parallel reduction requires that the reduction operation be independent of the equivalent serial running partial result,
 * i.e. it requires Associativity: for all a, b, c in G: (a * b) * c = a * (b * c)
 *
 * e.g. This is not an acceptable reduction function:
 *     a*b = a^2 + b^2
 * because:
 *     (a*b)*c = (a^2+b^2)^2 + c^2
 *     a*(b*c) = a^2 + (b^2+c^2)^2
 *
 * Serial reduction does not need this, but it can trivially be reimplemented by the user (i.e. just a for loop)
 * and doesn't need an external library for that.
 * That is the reasoning behind the way the map() and reduce() functions are declared.
 */


//==========================================================================================================================================
//= User Functions Declarations
//==========================================================================================================================================


#undef  functools_map_fun
#define functools_map_fun  FUNCTOOLS_GEN_EXPAND(functools_map_fun)
__attribute__((pure))
static _TYPE_OUT functools_map_fun(_TYPE_IN * A, long i);

#undef  functools_reduce_fun
#define functools_reduce_fun  FUNCTOOLS_GEN_EXPAND(functools_reduce_fun)
__attribute__((pure))
static _TYPE_OUT functools_reduce_fun(_TYPE_OUT a, _TYPE_OUT b);


//==========================================================================================================================================
//= Includes
//==========================================================================================================================================


//==========================================================================================================================================
//= Local Defines
//==========================================================================================================================================


#undef  _TYPE_IN
#define _TYPE_IN  FUNCTOOLS_GEN_EXPAND(_TYPE_IN)
typedef FUNCTOOLS_GEN_TYPE_1  _TYPE_IN;

#undef  _TYPE_OUT
#define _TYPE_OUT  FUNCTOOLS_GEN_EXPAND(_TYPE_OUT)
typedef FUNCTOOLS_GEN_TYPE_2  _TYPE_OUT;


//==========================================================================================================================================
//------------------------------------------------------------------------------------------------------------------------------------------
//-                                                              Templates                                                                 -
//------------------------------------------------------------------------------------------------------------------------------------------
//==========================================================================================================================================


//------------------------------------------------------------------------------------------------------------------------------------------
//- Reduce
//------------------------------------------------------------------------------------------------------------------------------------------


#undef  reduce_segment_serial
#define reduce_segment_serial  FUNCTOOLS_GEN_EXPAND(reduce_segment_serial)
_TYPE_OUT
reduce_segment_serial(_TYPE_IN * A, long i_start, long i_end, _TYPE_OUT zero, const int backwards)
{
	_TYPE_OUT total, val;
	long i;
	if (i_start >= i_end)
		return zero;
	total = zero;
	if (backwards)
		for (i=i_end-1;i>=i_start;i--)
		{
			val = functools_map_fun(A, i);
			total = functools_reduce_fun(val, total);
		}
	else
		for (i=i_start;i<i_end;i++)
		{
			val = functools_map_fun(A, i);
			total = functools_reduce_fun(total, val);
		}
	return total;
}


#undef  reduce_segment_concurrent
#define reduce_segment_concurrent  FUNCTOOLS_GEN_EXPAND(reduce_segment_concurrent)
_TYPE_OUT
reduce_segment_concurrent(_TYPE_IN * A, long i_start, long i_end, _TYPE_OUT zero, const int backwards)
{
	int num_threads = safe_omp_get_num_threads();
	int tnum = omp_get_thread_num();
	_TYPE_OUT val, total;
	long i_s, i_e;
	if (i_start >= i_end)
		return zero;
	loop_partitioner_balance_iterations(num_threads, tnum, i_start, i_end, &i_s, &i_e);
	val = reduce_segment_serial(A, i_s, i_e, zero, backwards);
	omp_thread_reduce_global(functools_reduce_fun, val, zero, 1, backwards, NULL, &total);
	return total;
}


#undef  reduce_segment
#define reduce_segment  FUNCTOOLS_GEN_EXPAND(reduce_segment)
_TYPE_OUT
reduce_segment(_TYPE_IN * A, long i_start, long i_end, _TYPE_OUT zero, const int backwards)
{
	_TYPE_OUT ret = 0;
	if (omp_get_level() > 0)
		return reduce_segment_serial(A, i_start, i_end, zero, backwards);
	#pragma omp parallel
	{
		_TYPE_OUT val = reduce_segment_concurrent(A, i_start, i_end, zero, backwards);
		#pragma omp single nowait
		ret = val;
	}
	return ret;
}


#undef  reduce_serial
#define reduce_serial  FUNCTOOLS_GEN_EXPAND(reduce_serial)
_TYPE_OUT
reduce_serial(_TYPE_IN * A, long N, _TYPE_OUT zero, const int backwards)
{
	return reduce_segment_serial(A, 0, N, zero, backwards);
}

#undef  reduce_concurrent
#define reduce_concurrent  FUNCTOOLS_GEN_EXPAND(reduce_concurrent)
_TYPE_OUT
reduce_concurrent(_TYPE_IN * A, long N, _TYPE_OUT zero, const int backwards)
{
	return reduce_segment_concurrent(A, 0, N, zero, backwards);
}

#undef  reduce
#define reduce  FUNCTOOLS_GEN_EXPAND(reduce)
_TYPE_OUT
reduce(_TYPE_IN * A, long N, _TYPE_OUT zero, const int backwards)
{
	return reduce_segment(A, 0, N, zero, backwards);
}


//------------------------------------------------------------------------------------------------------------------------------------------
//- Scan Reduce
//------------------------------------------------------------------------------------------------------------------------------------------


/* exclusive:  First element of P is zero, else it is the first element of A.
 * Notes:
 *     - P could also be A, so we should save/use A[i] before setting P[i].
 */

#undef  scan_reduce_segment_serial
#define scan_reduce_segment_serial  FUNCTOOLS_GEN_EXPAND(scan_reduce_segment_serial)
_TYPE_OUT
scan_reduce_segment_serial(_TYPE_IN * A, _TYPE_OUT * P, long i_start, long i_end, _TYPE_OUT zero, const int exclusive, const int backwards)
{
	_TYPE_OUT partial;
	_TYPE_OUT val, tmp;
	long i;
	if (i_start >= i_end)
		return zero;
	partial = zero;
	if (backwards)
	{
		if (exclusive)
			for (i=i_end-1;i>=i_start;i--)
			{
				tmp = partial;
				val = functools_map_fun(A, i);
				partial = functools_reduce_fun(val, partial);
				P[i] = tmp;
			}
		else
			for (i=i_end-1;i>=i_start;i--)
			{
				val = functools_map_fun(A, i);
				partial = functools_reduce_fun(val, partial);
				P[i] = partial;
			}
	}
	else
	{
		if (exclusive)
			for (i=i_start;i<i_end;i++)
			{
				tmp = partial;
				val = functools_map_fun(A, i);
				partial = functools_reduce_fun(partial, val);
				P[i] = tmp;
			}
		else
			for (i=i_start;i<i_end;i++)
			{
				val = functools_map_fun(A, i);
				partial = functools_reduce_fun(partial, val);
				P[i] = partial;
			}
	}
	return partial;
}


/* map() has a relatively high cost compared to memory access.
 * Store the intermediate results.
 */
#undef  scan_reduce_segment_concurrent_compute_bound
#define scan_reduce_segment_concurrent_compute_bound  FUNCTOOLS_GEN_EXPAND(scan_reduce_segment_concurrent_compute_bound)
static
_TYPE_OUT
scan_reduce_segment_concurrent_compute_bound(_TYPE_IN * A, _TYPE_OUT * P, long i_start, long i_end, _TYPE_OUT zero, _TYPE_OUT offset, const int exclusive, const int backwards)
{
	int num_threads = safe_omp_get_num_threads();
	int tnum = omp_get_thread_num();
	_TYPE_OUT local, total;
	_TYPE_OUT partial;
	long i_s, i_e;
	long i;
	loop_partitioner_balance_iterations(num_threads, tnum, i_start, i_end, &i_s, &i_e);
	partial = scan_reduce_segment_serial(A, P, i_s, i_e, zero, exclusive, backwards);
	omp_thread_reduce_global(functools_reduce_fun, partial, zero, 1, backwards, &local, &total);
	// Only after the thread reduce we can add the offset, because else we would add num_threads * offset.
	if (backwards)
	{
		local = functools_reduce_fun(local, offset);
		total = functools_reduce_fun(total, offset);
	}
	else
	{
		local = functools_reduce_fun(offset, local);
		total = functools_reduce_fun(offset, total);
	}
	if (backwards)
		for (i=i_e-1;i>=i_s;i--)
			P[i] = functools_reduce_fun(local, P[i]);
	else
		for (i=i_s;i<i_e;i++)
			P[i] = functools_reduce_fun(P[i], local);
	#pragma omp barrier
	return total;
}


/* map() has a relatively low cost compared to memory access.
 * Reevaluate instead of storing intermediate results.
 */
#undef  scan_reduce_segment_concurrent_memory_bound
#define scan_reduce_segment_concurrent_memory_bound  FUNCTOOLS_GEN_EXPAND(scan_reduce_segment_concurrent_memory_bound)
static
_TYPE_OUT
scan_reduce_segment_concurrent_memory_bound(_TYPE_IN * A, _TYPE_OUT * P, long i_start, long i_end, _TYPE_OUT zero, _TYPE_OUT offset, const int exclusive, const int backwards)
{
	int num_threads = safe_omp_get_num_threads();
	int tnum = omp_get_thread_num();
	_TYPE_OUT local, total;
	_TYPE_OUT partial;
	long i_s, i_e;
	loop_partitioner_balance_iterations(num_threads, tnum, i_start, i_end, &i_s, &i_e);
	partial = reduce_segment_serial(A, i_s, i_e, zero, backwards);
	omp_thread_reduce_global(functools_reduce_fun, partial, zero, 1, backwards, &local, &total);
	// Only after the thread reduce we can add the offset, because else we would add num_threads * offset.
	if (backwards)
	{
		local = functools_reduce_fun(local, offset);
		total = functools_reduce_fun(total, offset);
	}
	else
	{
		local = functools_reduce_fun(offset, local);
		total = functools_reduce_fun(offset, total);
	}
	scan_reduce_segment_serial(A, P, i_s, i_e, local, exclusive, backwards);
	#pragma omp barrier
	return total;
}


#undef  scan_reduce_segment_concurrent
#define scan_reduce_segment_concurrent  FUNCTOOLS_GEN_EXPAND(scan_reduce_segment_concurrent)
_TYPE_OUT
scan_reduce_segment_concurrent(_TYPE_IN * A, _TYPE_OUT * P, long i_start, long i_end, _TYPE_OUT zero, const int exclusive, const int backwards)
{
	static long map_is_mem_bound = -1;
	static long block_size;

	int num_threads = safe_omp_get_num_threads();
	long N = i_end - i_start;

	assert_omp_nesting_level(1);

	if (i_start >= i_end)
		return zero;

	if (__builtin_expect(map_is_mem_bound < 0, 0))
	{
		#pragma omp single nowait
		{
			/* Fit blocks in L1 cache.
			 * We use the L1 because in some machines (e.g. ARM Neoverse-N1) the L2 is shared.
			 * The 'omp barrier' is a lot faster than one might fear.
			 */
			long l1_cache_size;
			l1_cache_size = sysconf(_SC_LEVEL1_DCACHE_SIZE);
			// l1_cache_size = sysconf(_SC_LEVEL2_CACHE_SIZE);
			// printf("block_size = %ld\n", block_size);
			block_size = l1_cache_size / sizeof(_TYPE_OUT) * num_threads;
		}

		int tnum = omp_get_thread_num();
		static double * t_time_mem, * t_time_map;
		__attribute__((unused)) volatile _TYPE_OUT tmp;
		double time=0;
		// double overhead=HUGE_VAL;
		double min_time_mem=HUGE_VAL, min_time_map=HUGE_VAL;
		long i, j;
		const long n_i = 10, n_j = (1000 < N) ? 1000 : N;
		#pragma omp barrier
		#pragma omp single nowait
		{
			t_time_mem = (typeof(t_time_mem)) malloc(num_threads * sizeof(*t_time_mem));
			t_time_map = (typeof(t_time_map)) malloc(num_threads * sizeof(*t_time_map));
		}
		#pragma omp barrier
		for (i=0;i<n_i;i++)
		{
			// time = time_it(1);
			// if (time < overhead)
				// overhead = time;
			time = time_it(1,
				tmp = zero;
				for (j=0;j<n_j;j++)
					tmp = functools_reduce_fun(tmp, tmp);
			);
			if (time < min_time_mem)
				min_time_mem = time;
			time = time_it(1,
				tmp = zero;
				for (j=0;j<n_j;j++)
					tmp = functools_reduce_fun(tmp, functools_map_fun(A, j));
			);
			if (time < min_time_map)
				min_time_map = time;
		}
		// t_time_mem[tnum] = (overhead < min_time_mem) ? min_time_mem - overhead : 1;
		// t_time_map[tnum] = (overhead < min_time_map) ? min_time_map - overhead : 1;
		t_time_mem[tnum] = min_time_mem;
		t_time_map[tnum] = min_time_map;
		#pragma omp barrier
		#pragma omp single nowait
		{
			min_time_mem = t_time_mem[0];
			min_time_map = t_time_map[0];
			for (i=1;i<num_threads;i++)
			{
				if (t_time_mem[i] < min_time_mem)
					min_time_mem = t_time_mem[i];
				if (t_time_map[i] < min_time_map)
					min_time_map = t_time_map[i];
			}
			free(t_time_mem);
			free(t_time_map);
			map_is_mem_bound = (min_time_map / min_time_mem < 10) ? 1 : 0;
			// printf("map_is_mem_bound = %ld, ratio = %lf, overhead = %lf , time mem = %lf , time map = %lf\n", map_is_mem_bound, min_time_map / min_time_mem, overhead, min_time_mem, min_time_map);
			// printf("time mem bound = %lf , time compute bound = %lf\n", 2*min_time_map, min_time_mem + min_time_map);
		}
		#pragma omp barrier
	}

	// return scan_reduce_segment_concurrent_memory_bound(A, P, i_start, i_end, zero, exclusive, backwards);
	// return scan_reduce_segment_concurrent_compute_bound(A, P, i_start, i_end, zero, exclusive, backwards);

	long i, i_s, i_e;
	_TYPE_OUT partial = zero;
	if (backwards)
	{
		for (i=i_end;i>=i_start;i-=block_size)  // Not from i_end-1 here, because we pass it as non-inclusive boundary.
		{
			i_s = i - block_size;
			if (i_s < i_start)
				i_s = i_start;
			if (map_is_mem_bound)
				partial = scan_reduce_segment_concurrent_memory_bound(A, P, i_s, i, zero, partial, exclusive, backwards);
			else
				partial = scan_reduce_segment_concurrent_compute_bound(A, P, i_s, i, zero, partial, exclusive, backwards);
		}
	}
	else
	{
		for (i=i_start;i<i_end;i+=block_size)
		{
			i_e = i + block_size;
			if (i_e > i_end)
				i_e = i_end;
			if (map_is_mem_bound)
				partial = scan_reduce_segment_concurrent_memory_bound(A, P, i, i_e, zero, partial, exclusive, backwards);
			else
				partial = scan_reduce_segment_concurrent_compute_bound(A, P, i, i_e, zero, partial, exclusive, backwards);
		}
	}
	return partial;
}


#undef  scan_reduce_segment
#define scan_reduce_segment  FUNCTOOLS_GEN_EXPAND(scan_reduce_segment)
_TYPE_OUT
scan_reduce_segment(_TYPE_IN * A, _TYPE_OUT * P, long i_start, long i_end, _TYPE_OUT zero, const int exclusive, const int backwards)
{
	_TYPE_OUT ret = 0;
	if (omp_get_level() > 0)
		return scan_reduce_segment_serial(A, P, i_start, i_end, zero, exclusive, backwards);
	#pragma omp parallel
	{
		_TYPE_OUT val = scan_reduce_segment_concurrent(A, P, i_start, i_end, zero, exclusive, backwards);
		#pragma omp single nowait
		ret = val;
	}
	return ret;
}


#undef  scan_reduce_serial
#define scan_reduce_serial  FUNCTOOLS_GEN_EXPAND(scan_reduce_serial)
_TYPE_OUT
scan_reduce_serial(_TYPE_IN * A, _TYPE_OUT * P, long N, _TYPE_OUT zero, const int exclusive, const int backwards)
{
	return scan_reduce_segment_serial(A, P, 0, N, zero, exclusive, backwards);
}

#undef  scan_reduce_concurrent
#define scan_reduce_concurrent  FUNCTOOLS_GEN_EXPAND(scan_reduce_concurrent)
_TYPE_OUT
scan_reduce_concurrent(_TYPE_IN * A, _TYPE_OUT * P, long N, _TYPE_OUT zero, const int exclusive, const int backwards)
{
	return scan_reduce_segment_concurrent(A, P, 0, N, zero, exclusive, backwards);
}

#undef  scan_reduce
#define scan_reduce  FUNCTOOLS_GEN_EXPAND(scan_reduce)
_TYPE_OUT
scan_reduce(_TYPE_IN * A, _TYPE_OUT * P, long N, _TYPE_OUT zero, const int exclusive, const int backwards)
{
	return scan_reduce_segment(A, P, 0, N, zero, exclusive, backwards);
}

