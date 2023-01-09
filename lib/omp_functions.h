#ifndef OMP_FUNCTIONS_H
#define OMP_FUNCTIONS_H

#include <unistd.h>
#include <omp.h>

#include "macros/cpp_defines.h"
#include "macros/macrolib.h"
#include "debug.h"


#undef  assert_omp_nesting_level
#define assert_omp_nesting_level(level)                                                                \
do {                                                                                                   \
	if (omp_get_level() != level)                                                                  \
		error("invalid omp nesting level: required=%d, actual=%d", level, omp_get_level());    \
} while (0)


int safe_omp_get_num_threads_internal();
int safe_omp_get_num_threads_external();
int safe_omp_get_num_threads();
int safe_omp_get_thread_num_initial();

int get_affinity_from_GOMP_CPU_AFFINITY(int tnum);
void print_affinity();


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//------------------------------------------------------------------------------------------------------------------------------------------
//-                                                     Thread Parallel Reduction                                                          -
//------------------------------------------------------------------------------------------------------------------------------------------
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


/* We go through the trouble of defining a nested function in order to have more sensible error messages
 * in the case of argument type mismatches.
 *
 * The return type of 'reduce_fun()' is the anchor for all the other argument types.
 *
 * reduce_fun()          : T reduce_fun(T a, T b);
 * _local_result_ptr_ret : returns the partial reduction of all the previous threads until the calling thread.
 * _total_result_ptr_ret : returns the total reduction result.
 */


#undef  omp_thread_reduce_local
#define omp_thread_reduce_local(_reduce_fun, _map_fun, _A, _i_start, _i_end, _zero, _backwards,    \
		_local_result_ptr_ret, _total_result_ptr_ret)                                      \
do {                                                                                               \
} while (0)


#if 0
#undef  omp_thread_reduce_global
#define omp_thread_reduce_global(_reduce_fun, _partial, _zero, _backwards,                                                   \
		_local_result_ptr_ret, _total_result_ptr_ret)                                                                \
do {                                                                                                                         \
	RENAME(                                                                                                              \
		(_reduce_fun, reduce_fun),                                                                                   \
		(_partial, partial, typeof(_reduce_fun(_zero, _zero))),                                                      \
		(_zero, zero, typeof(_reduce_fun(_zero, _zero))),                                                            \
		(_backwards, backwards, int),                                                                                \
		(_local_result_ptr_ret, local_result_ptr_ret, typeof(typeof(_reduce_fun(_zero, _zero)) *)),                  \
		(_total_result_ptr_ret, total_result_ptr_ret, typeof(typeof(_reduce_fun(_zero, _zero)) *))                   \
	);                                                                                                                   \
	static int t_num_threads = 0;                                                                                        \
	static typeof(partial) * t_partial = NULL;                                                                           \
	int num_threads = safe_omp_get_num_threads();                                                                        \
	int tnum = omp_get_thread_num();                                                                                     \
	typeof(partial) local, total;                                                                                        \
	long i;                                                                                                              \
	assert_omp_nesting_level(1);                                                                                         \
	/* Basically a single barrier point. */                                                                              \
	/* Without __builtin_expect() it would always enter the 'if' and call the barrier (slow). */                         \
	if (__builtin_expect(t_num_threads < num_threads, 0)) /* Test BEFORE the barrier. */                                 \
	{                                                                                                                    \
		_Pragma("omp barrier")                                                                                       \
		/* 'single' has an implicit barrier! */                                                                      \
		_Pragma("omp single nowait")                                                                                 \
		{                                                                                                            \
			t_num_threads = num_threads;                                                                         \
			free(t_partial);                                                                                     \
			t_partial = (typeof(t_partial)) malloc(num_threads * sizeof(t_partial));                             \
		}                                                                                                            \
	}                                                                                                                    \
	_Pragma("omp barrier")                                                                                               \
	t_partial[tnum] = partial;                                                                                           \
	/* __atomic_thread_fence(__ATOMIC_SEQ_CST); */                                                                       \
	_Pragma("omp barrier")                                                                                               \
	local = zero;                                                                                                        \
	total = zero;                                                                                                        \
	if (backwards)                                                                                                       \
	{                                                                                                                    \
		for (i=num_threads-1;i>=0;i--)                                                                               \
		{                                                                                                            \
			if (i == tnum)                                                                                       \
				local = total;                                                                               \
			total = reduce_fun(t_partial[i], total);                                                             \
		}                                                                                                            \
	}                                                                                                                    \
	else                                                                                                                 \
	{                                                                                                                    \
		for (i=0;i<num_threads;i++)                                                                                  \
		{                                                                                                            \
			if (i == tnum)                                                                                       \
				local = total;                                                                               \
			total = reduce_fun(total, t_partial[i]);                                                             \
		}                                                                                                            \
	}                                                                                                                    \
	arg_return(local_result_ptr_ret, local);                                                                             \
	arg_return(total_result_ptr_ret, total);                                                                             \
} while (0)
#endif


#undef  omp_thread_reduce_global
#define omp_thread_reduce_global(_reduce_fun, _partial, _zero, _backwards,                                                                \
		_local_result_ptr_ret, _total_result_ptr_ret)                                                                             \
do {                                                                                                                                      \
	RENAME((_reduce_fun, reduce_fun), (_partial, partial), (_zero, zero), (_backwards, backwards),                                    \
			(_local_result_ptr_ret, local_result_ptr_ret), (_total_result_ptr_ret, total_result_ptr_ret));                    \
	typeof(reduce_fun(zero, zero)) T;                                                                                                 \
	void omp_thread_reduce_global(typeof(T) (*reduce_fun)(typeof(T), typeof(T)), typeof(T) partial, typeof(T) zero, int backwards,    \
			typeof(&T) local_result_ptr_ret, typeof(&T) total_result_ptr_ret)                                                 \
	{                                                                                                                                 \
		static int t_num_threads = 0;                                                                                             \
		static typeof(T) * t_partial = NULL;                                                                                      \
		int num_threads = safe_omp_get_num_threads();                                                                             \
		int tnum = omp_get_thread_num();                                                                                          \
		typeof(T) local, total;                                                                                                   \
		long i;                                                                                                                   \
		assert_omp_nesting_level(1);                                                                                              \
		/* Basically a single barrier point. */                                                                                   \
		/* Without __builtin_expect() it would always enter the 'if' and call the barrier (slow). */                              \
		if (__builtin_expect(t_num_threads < num_threads, 0)) /* Test BEFORE the barrier. */                                      \
		{                                                                                                                         \
			_Pragma("omp barrier")                                                                                            \
			/* 'single' has an implicit barrier! */                                                                           \
			_Pragma("omp single nowait")                                                                                      \
			{                                                                                                                 \
				t_num_threads = num_threads;                                                                              \
				free(t_partial);                                                                                          \
				t_partial = (typeof(t_partial)) malloc(num_threads * sizeof(t_partial));                                  \
			}                                                                                                                 \
		}                                                                                                                         \
		_Pragma("omp barrier")                                                                                                    \
		t_partial[tnum] = partial;                                                                                                \
		/* __atomic_thread_fence(__ATOMIC_SEQ_CST); */                                                                            \
		_Pragma("omp barrier")                                                                                                    \
		local = zero;                                                                                                             \
		total = zero;                                                                                                             \
		if (backwards)                                                                                                            \
		{                                                                                                                         \
			for (i=num_threads-1;i>=0;i--)                                                                                    \
			{                                                                                                                 \
				if (i == tnum)                                                                                            \
					local = total;                                                                                    \
				total = reduce_fun(t_partial[i], total);                                                                  \
			}                                                                                                                 \
		}                                                                                                                         \
		else                                                                                                                      \
		{                                                                                                                         \
			for (i=0;i<num_threads;i++)                                                                                       \
			{                                                                                                                 \
				if (i == tnum)                                                                                            \
					local = total;                                                                                    \
				total = reduce_fun(total, t_partial[i]);                                                                  \
			}                                                                                                                 \
		}                                                                                                                         \
		if (local_result_ptr_ret != NULL)                                                                                         \
			*local_result_ptr_ret = (local);                                                                                  \
		if (total_result_ptr_ret != NULL)                                                                                         \
			*total_result_ptr_ret = (total);                                                                                  \
	}                                                                                                                                 \
	omp_thread_reduce_global(reduce_fun, partial, zero, backwards, local_result_ptr_ret, total_result_ptr_ret);                       \
} while (0)


#endif /* OMP_FUNCTIONS_H */

