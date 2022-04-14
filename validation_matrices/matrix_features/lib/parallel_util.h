#ifndef PARALLEL_UTILS_H
#define PARALLEL_UTILS_H

#include <omp.h>
#include "macros/macrolib.h"


// Sets the local start and end iterations of the worker.
// End is exclusive.
static inline
void
loop_partitioner_base(long num_workers, long worker_pos, long start, long end,
		long * local_start_out, long * local_end_out, long * num_iterations_out, long * sign_out)
{
	long len         = end - start;
	long per_t_len   = len / num_workers;
	long rem         = len - per_t_len * num_workers;
	long sgn = (len > 0) ? 1 : -1;
	if (rem != 0)
	{
		if (worker_pos < sgn * rem)
		{
			per_t_len += sgn;
			rem = 0;
		}
	}
	*local_start_out = start + per_t_len * worker_pos + rem;
	*local_end_out   = *local_start_out + per_t_len;
	*num_iterations_out = sgn * per_t_len;
	*sign_out = sgn;
}


// Generic interface for the integer pointers and optional arguments.

#define loop_partitioner_balance_iterations(num_workers, worker_pos, start, end, local_start_ptr, local_end_ptr, ... /* num_iterations_ptr, sign_ptr */)    \
do {                                                                                                                                                        \
	long _l_s, _l_e, _num_iters, _sgn;                                                                                                                  \
	__auto_type _num_iters_ptr = DEFAULT_ARG_1(&_num_iters, ##__VA_ARGS__);                                                                             \
	__auto_type _sgn_ptr = DEFAULT_ARG_2(&_sgn, ##__VA_ARGS__);                                                                                         \
	loop_partitioner_base(num_workers, worker_pos, start, end, &_l_s, &_l_e, &_num_iters, &_sgn);                                                       \
	*local_start_ptr = _l_s;                                                                                                                            \
	*local_end_ptr = _l_e;                                                                                                                              \
	if (_num_iters_ptr != NULL)                                                                                                                         \
		*_num_iters_ptr = _num_iters;                                                                                                               \
	if (_sgn_ptr != NULL)                                                                                                                               \
		*_sgn_ptr = _sgn;                                                                                                                           \
} while (0)


//==========================================================================================================================================
//= Balance Loops
//==========================================================================================================================================


#if 0

#define loop_partitioner_balance_partial_sums(num_workers, worker_pos, Sums, N, total_sum, local_start_indexes, local_end_indexes, ... /* order_decreasing */)    \
do {                                                                                                                                                              \
	long _num_threads = omp_get_max_threads();                                                                                                                \
	long _tnum = omp_get_thread_num();                                                                                                                        \
	long _order_decreasing = DEFAULT_ARG_1(0, ##__VA_ARGS__);                                                                                                 \
	long _i_s, _i_e;                                                                                                                                          \
	__auto_type __target = total_sum;                                                                                                                         \
                                                                                                                                                                  \
	if (_order_decreasing)                                                                                                                                    \
	{                                                                                                                                                         \
		__target = (total_sum * (_num_threads - 1 - _tnum)) / _num_threads;                                                                               \
		_i_s = binary_search(Sums, N-1, 0, __target, NULL, NULL);                                                                                         \
	}                                                                                                                                                         \
	else                                                                                                                                                      \
	{                                                                                                                                                         \
		__target = (total_sum * _tnum) / _num_threads;                                                                                                    \
		_i_s = binary_search(Sums, 0, N-1, __target, NULL, NULL);                                                                                         \
	}                                                                                                                                                         \
                                                                                                                                                                  \
	/* if (_i_s < _tnum)                                                                                                                                      \
		_i_s = (_tnum < total_sum) ? _tnum : total_sum - 1; */                                                                                            \
                                                                                                                                                                  \
	__atomic_store_n(&(local_start_indexes[_tnum]), _i_s, __ATOMIC_RELAXED);                                                                                  \
                                                                                                                                                                  \
	_Pragma("omp barrier");                                                                                                                                   \
                                                                                                                                                                  \
	if (_tnum == 0)                                                                                                                                           \
		local_start_indexes[0] = 0;                                                                                                                       \
                                                                                                                                                                  \
	if (_tnum == _num_threads - 1)                                                                                                                            \
		_i_e = N;                                                                                                                                         \
	else                                                                                                                                                      \
		_i_e = local_start_indexes[_tnum+1];                                                                                                              \
	local_end_indexes[_tnum] = _i_e;                                                                                                                          \
} while (0)

#else


/*
 * Notes:
 * - End is exclusive.
 * - 'order_decreasing' is for the partial sums 'Sums' only for the binary search.
 *   The indexes returned are always: start <= end.
 * - 'num_workers' and 'worker_pos' are always integers, so we transform them to 'long',
 *   to avoid overflows in the multiplication if 'total_sum' is 'int' or smaller.
 */

#define loop_partitioner_balance_partial_sums(num_workers, worker_pos, Sums, N, total_sum, local_start_ptr, local_end_ptr, ... /* order_decreasing */)    \
do {                                                                                                                                                      \
	long __attribute__((unused)) _order_decreasing = DEFAULT_ARG_1(0, ##__VA_ARGS__);                                                                 \
	long _i_s, _i_e;                                                                                                                                  \
	__auto_type _target = total_sum;                                                                                                                  \
	__auto_type _target_next = total_sum;                                                                                                             \
	_target = (total_sum * ((long) worker_pos)) / ((long) num_workers);                                                                               \
	_target_next = (total_sum * ((long) worker_pos+1)) / ((long) num_workers);                                                                        \
                                                                                                                                                          \
	if (worker_pos == 0)                                                                                                                              \
		_i_s = 0;                                                                                                                                 \
	else                                                                                                                                              \
		_i_s = binary_search(Sums, 0, N-1, _target, NULL, NULL);                                                                                  \
                                                                                                                                                          \
	if (worker_pos == num_workers - 1)                                                                                                                \
		_i_e = N;                                                                                                                                 \
	else                                                                                                                                              \
		_i_e = binary_search(Sums, 0, N-1, _target_next, NULL, NULL);                                                                             \
                                                                                                                                                          \
	*local_start_ptr = _i_s;                                                                                                                          \
	*local_end_ptr = _i_e;                                                                                                                            \
} while (0)

#endif


#define _loop_partitioner_balance_cmp(target, A, i)                                                                                            \
({                                                                                                                                             \
	(target > A[i] + _loop_partitioner_balance_i_vs_work * i) ? 1 : (target < A[i] + _loop_partitioner_balance_i_vs_work * i) ? -1 : 0;    \
})

#define _loop_partitioner_balance_dist(target, A, i)                     \
({                                                                       \
	ABS(target - A[i] - _loop_partitioner_balance_i_vs_work * i);    \
})

#define loop_partitioner_balance(num_workers, worker_pos, i_vs_work, Sums, N, total_sum, local_start_ptr, local_end_ptr, ... /* order_decreasing */)    \
do {                                                                                                                                                    \
	long __attribute__((unused)) _order_decreasing = DEFAULT_ARG_1(0, ##__VA_ARGS__);                                                               \
	long _i_s, _i_e;                                                                                                                                \
	__auto_type _target = total_sum;                                                                                                                \
	__auto_type _target_next = total_sum;                                                                                                           \
	double _loop_partitioner_balance_i_vs_work = i_vs_work;                                                                                         \
	_target = ((total_sum + _loop_partitioner_balance_i_vs_work * N) * ((long) worker_pos)) / ((long) num_workers);                                 \
	_target_next = ((total_sum + _loop_partitioner_balance_i_vs_work * N) * ((long) worker_pos+1)) / ((long) num_workers);                          \
                                                                                                                                                        \
	if (worker_pos == 0)                                                                                                                            \
		_i_s = 0;                                                                                                                               \
	else                                                                                                                                            \
		_i_s = binary_search(Sums, 0, N-1, _target, NULL, NULL, _loop_partitioner_balance_cmp, _loop_partitioner_balance_dist);                 \
                                                                                                                                                        \
	if (worker_pos == num_workers - 1)                                                                                                              \
		_i_e = N;                                                                                                                               \
	else                                                                                                                                            \
		_i_e = binary_search(Sums, 0, N-1, _target_next, NULL, NULL, _loop_partitioner_balance_cmp, _loop_partitioner_balance_dist);            \
                                                                                                                                                        \
	*local_start_ptr = _i_s;                                                                                                                        \
	*local_end_ptr = _i_e;                                                                                                                          \
} while (0)


#endif /* PARALLEL_UTILS_H */

