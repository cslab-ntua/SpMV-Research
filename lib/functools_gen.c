#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

#include "macros/macrolib.h"
#include "parallel_util.h"

#include "functools_gen.h"


#ifndef FUNCTOOLS_GEN_C
#define FUNCTOOLS_GEN_C

#endif /* FUNCTOOLS_GEN_C */


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//------------------------------------------------------------------------------------------------------------------------------------------
//-                                                              Templates                                                                 -
//------------------------------------------------------------------------------------------------------------------------------------------
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


//==========================================================================================================================================
//= User Functions Declarations
//==========================================================================================================================================


#undef  functools_reduce_fun
#define functools_reduce_fun  FUNCTOOLS_GEN_EXPAND(functools_reduce_fun)
static void functools_reduce_fun(_TYPE * partial_result, _TYPE * x);

#undef  functools_set_value
#define functools_set_value  FUNCTOOLS_GEN_EXPAND(functools_set_value)
static void functools_set_value(_TYPE * x, _TYPE val);


//==========================================================================================================================================
//= Includes
//==========================================================================================================================================


//==========================================================================================================================================
//= Local Defines
//==========================================================================================================================================


#undef  _TYPE
#define _TYPE  FUNCTOOLS_GEN_EXPAND(_TYPE)
typedef FUNCTOOLS_GEN_TYPE_1  _TYPE;


#undef  reduce_fun
#define reduce_fun  functools_reduce_fun

#undef  set_value
#define set_value  functools_set_value


//==========================================================================================================================================
//= Generic Code
//==========================================================================================================================================


#undef  reduce_serial
#define reduce_serial  FUNCTOOLS_GEN_EXPAND(reduce_serial)
_TYPE
reduce_serial(_TYPE * restrict A, long N, _TYPE zero, const int backwards)
{
	_TYPE total;
	long i;
	set_value(&total, zero);
	if (backwards)
		for (i=N-1;i>=0;i--)
			reduce_fun(&total, &(A[i]));
	else
		for (i=0;i<N;i++)
			reduce_fun(&total, &(A[i]));
	return total;
}


// Parallel reduction can only be valid with commutative reduce operations (forward == backward direction).
#undef  reduce
#define reduce  FUNCTOOLS_GEN_EXPAND(reduce)
_TYPE
reduce(_TYPE * restrict A, long N, _TYPE zero)
{
	int num_threads = safe_omp_get_num_threads_external();
	_TYPE t_partial[num_threads];
	_TYPE total;
	long i;
	if (omp_get_level() > 0)
		return reduce_serial(A, N, zero, 0);
	#pragma omp parallel
	{
		int tnum = omp_get_thread_num();
		_TYPE partial;
		long i;
		set_value(&partial, zero);
		#pragma omp for
		for (i=0;i<N;i++)
			reduce_fun(&partial, &(A[i]));
		set_value(&(t_partial[tnum]), partial);
	}
	__atomic_thread_fence(__ATOMIC_SEQ_CST);
	set_value(&total, zero);
	// total = reduce_serial(t_partial, num_threads, zero, 0);
	for (i=0;i<num_threads;i++)
		reduce_fun(&total, &t_partial[i]);
	return total;
}


/*
 * start_from_zero:  First element of P is zero, else it is the first element of A.
 *
 * Notes:
 *     - P could also be A, so we should save/use A[i] before setting P[i].
 */

#undef  scan_reduce_serial
#define scan_reduce_serial  FUNCTOOLS_GEN_EXPAND(scan_reduce_serial)
_TYPE
scan_reduce_serial(_TYPE * A, _TYPE * P, long N, _TYPE zero, const int start_from_zero, const int backwards)
{
	_TYPE partial;
	_TYPE tmp;
	long i;
	if (N == 0)
		return zero;
	set_value(&partial, zero);
	if (backwards)
		for (i=N-1;i>=0;i--)
		{
			if (start_from_zero)
			{
				set_value(&tmp, partial);
				reduce_fun(&partial, &A[i]);
				set_value(&(P[i]), tmp);
			}
			else
			{
				reduce_fun(&partial, &A[i]);
				set_value(&(P[i]), partial);
			}
		}
	else
		for (i=0;i<N;i++)
		{
			if (start_from_zero)
			{
				set_value(&tmp, partial);
				reduce_fun(&partial, &A[i]);
				set_value(&(P[i]), tmp);
			}
			else
			{
				reduce_fun(&partial, &A[i]);
				set_value(&(P[i]), partial);
			}
		}
	return partial;
}


// Parallel reduction can only be valid with commutative reduce operations (forward == backward direction).
#undef  scan_reduce
#define scan_reduce  FUNCTOOLS_GEN_EXPAND(scan_reduce)
_TYPE
scan_reduce(_TYPE * A, _TYPE * P, long N, _TYPE zero, const int start_from_zero)
{
	int num_threads = safe_omp_get_num_threads_external();
	_TYPE t_base[num_threads];
	_TYPE total;

	if (N == 0)
		return zero;
	if (omp_get_level() > 0)
		return scan_reduce_serial(A, P, N, zero, start_from_zero, 0);
	#pragma omp parallel
	{
		int tnum = omp_get_thread_num();
		_TYPE partial;
		_TYPE tmp;
		long i_s, i_e;
		long i;

		loop_partitioner_balance_iterations(num_threads, tnum, 0, N, &i_s, &i_e);

		set_value(&partial, zero);
		for (i=i_s;i<i_e;i++)
			reduce_fun(&partial, &(A[i]));

		set_value(&(t_base[tnum]), partial);

		#pragma omp barrier
		__atomic_thread_fence(__ATOMIC_SEQ_CST);

		set_value(&partial, zero);
		#pragma omp single
		{
			for (i=0;i<num_threads;i++)
			{
				set_value(&tmp, t_base[i]);
				set_value(&(t_base[i]), partial);
				reduce_fun(&partial, &tmp);
			}
			set_value(&total, partial);
		}

		#pragma omp barrier
		__atomic_thread_fence(__ATOMIC_SEQ_CST);

		scan_reduce_serial(&A[i_s], &P[i_s], i_e-i_s, t_base[tnum], start_from_zero, 0);
	}

	return total;
}

