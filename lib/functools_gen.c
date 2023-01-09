#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

#include "macros/macrolib.h"
#include "omp_functions.h"
#include "parallel_util.h"
#include "time_it_tsc.h"

#include "functools_gen.h"


#ifndef FUNCTOOLS_GEN_C
#define FUNCTOOLS_GEN_C

#endif /* FUNCTOOLS_GEN_C */


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


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//------------------------------------------------------------------------------------------------------------------------------------------
//-                                                              Templates                                                                 -
//------------------------------------------------------------------------------------------------------------------------------------------
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


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
//= Generic Code
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


#undef  reduce_segment_parallel
#define reduce_segment_parallel  FUNCTOOLS_GEN_EXPAND(reduce_segment_parallel)
_TYPE_OUT
reduce_segment_parallel(_TYPE_IN * A, long i_start, long i_end, _TYPE_OUT zero, const int backwards)
{
	int num_threads = safe_omp_get_num_threads();
	int tnum = omp_get_thread_num();
	_TYPE_OUT val, total;
	long i_s, i_e;
	if (i_start >= i_end)
		return zero;
	loop_partitioner_balance_iterations(num_threads, tnum, i_start, i_end, &i_s, &i_e);
	val = reduce_segment_serial(A, i_s, i_e, zero, backwards);
	omp_thread_reduce_global(functools_reduce_fun, val, zero, backwards, NULL, &total);
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
		_TYPE_OUT val = reduce_segment_parallel(A, i_start, i_end, zero, backwards);
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

#undef  reduce_parallel
#define reduce_parallel  FUNCTOOLS_GEN_EXPAND(reduce_parallel)
_TYPE_OUT
reduce_parallel(_TYPE_IN * A, long N, _TYPE_OUT zero, const int backwards)
{
	return reduce_segment_parallel(A, 0, N, zero, backwards);
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


/* start_from_zero:  First element of P is zero, else it is the first element of A.
 * Notes:
 *     - P could also be A, so we should save/use A[i] before setting P[i].
 */
#undef  scan_reduce_segment_serial
#define scan_reduce_segment_serial  FUNCTOOLS_GEN_EXPAND(scan_reduce_segment_serial)
_TYPE_OUT
scan_reduce_segment_serial(_TYPE_IN * A, _TYPE_OUT * P, long i_start, long i_end, _TYPE_OUT zero, const int start_from_zero, const int backwards)
{
	_TYPE_OUT partial;
	_TYPE_OUT val, tmp;
	long i;
	if (i_start >= i_end)
		return zero;
	partial = zero;
	if (backwards)
	{
		if (start_from_zero)
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
		if (start_from_zero)
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


/* map() has a relatively high cost compared to memory access, store the intermediate results.
 */
#undef  scan_reduce_segment_parallel_compute_bound
#define scan_reduce_segment_parallel_compute_bound  FUNCTOOLS_GEN_EXPAND(scan_reduce_segment_parallel_compute_bound)
static
_TYPE_OUT
scan_reduce_segment_parallel_compute_bound(_TYPE_IN * A, _TYPE_OUT * P, long i_start, long i_end, _TYPE_OUT zero, const int start_from_zero, const int backwards)
{
	int num_threads = safe_omp_get_num_threads();
	int tnum = omp_get_thread_num();
	_TYPE_OUT local, total;
	_TYPE_OUT partial;
	long i_s, i_e;
	long i;
	if (i_start >= i_end)
		return zero;
	loop_partitioner_balance_iterations(num_threads, tnum, i_start, i_end, &i_s, &i_e);
	partial = scan_reduce_segment_serial(A, P, i_s, i_e, zero, start_from_zero, backwards);
	omp_thread_reduce_global(functools_reduce_fun, partial, zero, backwards, &local, &total);
	if (backwards)
		for (i=i_e-1;i>=i_s;i--)
			P[i] = functools_reduce_fun(local, P[i]);
	else
		for (i=i_s;i<i_e;i++)
			P[i] = functools_reduce_fun(P[i], local);
	#pragma omp barrier
	return total;
}


/* map() has a relatively low cost compared to memory access, reevaluate instead of storing.
 */
#undef  scan_reduce_segment_parallel_memory_bound
#define scan_reduce_segment_parallel_memory_bound  FUNCTOOLS_GEN_EXPAND(scan_reduce_segment_parallel_memory_bound)
static
_TYPE_OUT
scan_reduce_segment_parallel_memory_bound(_TYPE_IN * A, _TYPE_OUT * P, long i_start, long i_end, _TYPE_OUT zero, const int start_from_zero, const int backwards)
{
	int num_threads = safe_omp_get_num_threads();
	int tnum = omp_get_thread_num();
	_TYPE_OUT local, total;
	_TYPE_OUT partial, val;
	long i_s, i_e;
	long i;
	if (i_start >= i_end)
		return zero;
	loop_partitioner_balance_iterations(num_threads, tnum, i_start, i_end, &i_s, &i_e);
	partial = zero;
	if (backwards)
	{
		for (i=i_e-1;i>=i_s;i--)
		{
			val = functools_map_fun(A, i);
			partial = functools_reduce_fun(val, partial);
		}
	}
	else
	{
		for (i=i_s;i<i_e;i++)
		{
			val = functools_map_fun(A, i);
			partial = functools_reduce_fun(partial, val);
		}
	}
	omp_thread_reduce_global(functools_reduce_fun, partial, zero, backwards, &local, &total);
	scan_reduce_segment_serial(A, P, i_s, i_e, local, start_from_zero, backwards);
	#pragma omp barrier
	return total;
}


#undef  scan_reduce_segment_parallel
#define scan_reduce_segment_parallel  FUNCTOOLS_GEN_EXPAND(scan_reduce_segment_parallel)
_TYPE_OUT
scan_reduce_segment_parallel(_TYPE_IN * A, _TYPE_OUT * P, long i_start, long i_end, _TYPE_OUT zero, const int start_from_zero, const int backwards)
{
	long N = i_end - i_start;
	static long map_is_mem_bound = -1;
	assert_omp_nesting_level(1);
	if (__builtin_expect(map_is_mem_bound < 0, 0))
	{
		static uint64_t * t_cycles_mem, * t_cycles_map;
		int num_threads = safe_omp_get_num_threads();
		int tnum = omp_get_thread_num();
		__attribute__((unused)) volatile _TYPE_OUT tmp;
		uint64_t cycles=0;
		// uint64_t overhead=~0;
		uint64_t min_cycles_mem=~0, min_cycles_map=~0;
		long i, j;
		const long n_i = 10, n_j = (1000 < N) ? 1000 : N;
		#pragma omp barrier
		#pragma omp single nowait
		{
			t_cycles_mem = (typeof(t_cycles_mem)) malloc(num_threads * sizeof(*t_cycles_mem));
			t_cycles_map = (typeof(t_cycles_map)) malloc(num_threads * sizeof(*t_cycles_map));
		}
		#pragma omp barrier
		for (i=0;i<n_i;i++)
		{
			// cycles = time_it_tsc(1);
			// if (cycles < overhead)
				// overhead = cycles;
			cycles = time_it_tsc(1,
				tmp = zero;
				for (j=0;j<n_j;j++)
					tmp = functools_reduce_fun(tmp, tmp);
			);
			if (cycles < min_cycles_mem)
				min_cycles_mem = cycles;
			cycles = time_it_tsc(1,
				tmp = zero;
				for (j=0;j<n_j;j++)
					tmp = functools_reduce_fun(tmp, functools_map_fun(A, j));
			);
			if (cycles < min_cycles_map)
				min_cycles_map = cycles;
		}
		// t_cycles_mem[tnum] = (overhead < min_cycles_mem) ? min_cycles_mem - overhead : 1;
		// t_cycles_map[tnum] = (overhead < min_cycles_map) ? min_cycles_map - overhead : 1;
		t_cycles_mem[tnum] = min_cycles_mem;
		t_cycles_map[tnum] = min_cycles_map;
		#pragma omp barrier
		#pragma omp single nowait
		{
			min_cycles_mem = t_cycles_mem[0];
			min_cycles_map = t_cycles_map[0];
			for (i=1;i<num_threads;i++)
			{
				if (t_cycles_mem[i] < min_cycles_mem)
					min_cycles_mem = t_cycles_mem[i];
				if (t_cycles_map[i] < min_cycles_map)
					min_cycles_map = t_cycles_map[i];
			}
			free(t_cycles_mem);
			free(t_cycles_map);
			map_is_mem_bound = (min_cycles_map / min_cycles_mem < 10) ? 1 : 0;
			// printf("map_is_mem_bound = %ld, ratio = %ld, overhead = %ld , cycles mem = %ld , cycles map = %ld\n", map_is_mem_bound, min_cycles_map / min_cycles_mem, overhead, min_cycles_mem, min_cycles_map);
			// printf("cycles mem bound = %ld , cycles compute bound = %ld\n", 2*min_cycles_map, min_cycles_mem + min_cycles_map);
		}
		#pragma omp barrier
	}

	// return scan_reduce_segment_parallel_memory_bound(A, P, i_start, i_end, zero, start_from_zero, backwards);
	// return scan_reduce_segment_parallel_compute_bound(A, P, i_start, i_end, zero, start_from_zero, backwards);

	if (map_is_mem_bound)
		return scan_reduce_segment_parallel_memory_bound(A, P, i_start, i_end, zero, start_from_zero, backwards);
	else
		return scan_reduce_segment_parallel_compute_bound(A, P, i_start, i_end, zero, start_from_zero, backwards);
}


#undef  scan_reduce_segment
#define scan_reduce_segment  FUNCTOOLS_GEN_EXPAND(scan_reduce_segment)
_TYPE_OUT
scan_reduce_segment(_TYPE_IN * A, _TYPE_OUT * P, long i_start, long i_end, _TYPE_OUT zero, const int start_from_zero, const int backwards)
{
	_TYPE_OUT ret = 0;
	if (omp_get_level() > 0)
		return scan_reduce_segment_serial(A, P, i_start, i_end, zero, start_from_zero, backwards);
	#pragma omp parallel
	{
		_TYPE_OUT val = scan_reduce_segment_parallel(A, P, i_start, i_end, zero, start_from_zero, backwards);
		#pragma omp single nowait
		ret = val;
	}
	return ret;
}


#undef  scan_reduce_serial
#define scan_reduce_serial  FUNCTOOLS_GEN_EXPAND(scan_reduce_serial)
_TYPE_OUT
scan_reduce_serial(_TYPE_IN * A, _TYPE_OUT * P, long N, _TYPE_OUT zero, const int start_from_zero, const int backwards)
{
	return scan_reduce_segment_serial(A, P, 0, N, zero, start_from_zero, backwards);
}

#undef  scan_reduce_parallel
#define scan_reduce_parallel  FUNCTOOLS_GEN_EXPAND(scan_reduce_parallel)
_TYPE_OUT
scan_reduce_parallel(_TYPE_IN * A, _TYPE_OUT * P, long N, _TYPE_OUT zero, const int start_from_zero, const int backwards)
{
	return scan_reduce_segment_parallel(A, P, 0, N, zero, start_from_zero, backwards);
}

#undef  scan_reduce
#define scan_reduce  FUNCTOOLS_GEN_EXPAND(scan_reduce)
_TYPE_OUT
scan_reduce(_TYPE_IN * A, _TYPE_OUT * P, long N, _TYPE_OUT zero, const int start_from_zero, const int backwards)
{
	return scan_reduce_segment(A, P, 0, N, zero, start_from_zero, backwards);
}

