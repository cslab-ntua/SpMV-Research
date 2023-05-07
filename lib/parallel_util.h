#ifndef PARALLEL_UTILS_H
#define PARALLEL_UTILS_H

#include "macros/cpp_defines.h"
#include "macros/macrolib.h"


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//------------------------------------------------------------------------------------------------------------------------------------------
//-                                                         Loop Partitioning                                                              -
//------------------------------------------------------------------------------------------------------------------------------------------
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


//==========================================================================================================================================
//= Balance Loops
//==========================================================================================================================================


// Sets the local start and end iterations of the worker.
// End is exclusive.

// static inline
// void
// loop_partitioner_balance_iterations_base(long num_workers, long worker_pos, long start, long end,
		// long * local_start_out, long * local_end_out, long * num_iterations_out, long * sign_out)
// {
	// long len         = end - start;
	// long per_t_len   = len / num_workers;
	// long rem         = len - per_t_len * num_workers;
	// long sgn = (len > 0) ? 1 : -1;
	// if (rem != 0)
	// {
		// if (worker_pos < sgn * rem)
		// {
			// per_t_len += sgn;
			// rem = 0;
		// }
	// }
	// *local_start_out = start + per_t_len * worker_pos + rem;
	// *local_end_out   = *local_start_out + per_t_len;
	// *num_iterations_out = sgn * per_t_len;
	// *sign_out = sgn;
// }


static inline
void
loop_partitioner_balance_iterations_base(long num_workers, long worker_pos, long start, long end, long incr,
		long * local_start_out, long * local_end_out, long * num_iterations_out)
{
	long sgn = (incr > 0) ? 1 : -1;
	long len = (end - start + incr - sgn) / incr;
	if (len < 0)
		error("infinite loop");
	if (sgn*incr > len)  // If |incr| >= len then worker 0 gets all the work.
	{
		if (worker_pos == 0)
		{
			*local_start_out = start;
			*local_end_out = end;
			*num_iterations_out = len;
		}
		else
		{
			*local_start_out = end;
			*local_end_out = end;
			*num_iterations_out = 0;
		}
		return;
	}
	long per_t_len = len / num_workers;
	long rem       = len % num_workers;
	long local_start, local_end;
	long num_iterations;
	if (rem != 0)
	{
		if (worker_pos < rem)
		{
			per_t_len += 1;
			rem = 0;
		}
	}
	local_start = start + incr * per_t_len * worker_pos + incr * rem;
	local_end   = local_start + incr * per_t_len;
	if (worker_pos == num_workers - 1)
		local_end = end;
	num_iterations = sgn * (local_end - local_start);

	*local_start_out = local_start;
	*local_end_out = local_end;
	*num_iterations_out = num_iterations;
}


// Generic interface for the integer pointers and optional arguments.
// End is exclusive.

#if 0
#define loop_partitioner_balance_iterations(num_workers, worker_pos, start, end, local_start_ptr, local_end_ptr, ... /* num_iterations_ptr, sign_ptr */)    \
do {                                                                                                                                                        \
	long _l_s, _l_e, _num_iters, _sgn;                                                                                                                  \
	__auto_type _num_iters_ptr = DEFAULT_ARG_1(&_num_iters, ##__VA_ARGS__);                                                                             \
	__auto_type _sgn_ptr = DEFAULT_ARG_2(&_sgn, ##__VA_ARGS__);                                                                                         \
	loop_partitioner_balance_iterations_base(num_workers, worker_pos, start, end, &_l_s, &_l_e, &_num_iters, &_sgn);                                    \
	*local_start_ptr = _l_s;                                                                                                                            \
	*local_end_ptr = _l_e;                                                                                                                              \
	if (_num_iters_ptr != NULL)                                                                                                                         \
		*_num_iters_ptr = _num_iters;                                                                                                               \
	if (_sgn_ptr != NULL)                                                                                                                               \
		*_sgn_ptr = _sgn;                                                                                                                           \
} while (0)
#endif


#define loop_partitioner_balance_iterations(num_workers, worker_pos, start, end, local_start_ptr, local_end_ptr, ... /* increment, num_iterations_ptr */)    \
do {                                                                                                                                                         \
	long _l_s, _l_e, _num_iters;                                                                                                                         \
	long _incr = DEFAULT_ARG_1(((start < end) ? 1 : -1), ##__VA_ARGS__);                                                                                 \
	__auto_type _num_iters_ptr = DEFAULT_ARG_2(&_num_iters, ##__VA_ARGS__);                                                                              \
	loop_partitioner_balance_iterations_base(num_workers, worker_pos, start, end, _incr, &_l_s, &_l_e, &_num_iters);                                     \
	*local_start_ptr = _l_s;                                                                                                                             \
	*local_end_ptr = _l_e;                                                                                                                               \
	if (_num_iters_ptr != NULL)                                                                                                                          \
		*_num_iters_ptr = _num_iters;                                                                                                                \
} while (0)


//==========================================================================================================================================
//= Balance Prefix Sums
//==========================================================================================================================================


/* Notes:
 * - End is exclusive.
 * - 'order_decreasing': Whether the prefix sums are in reverse order. The indexes returned are always: start <= end.
 * - 'num_workers' and 'worker_pos' are always integers, so we transform them to 'long',
 *   to avoid overflows in the multiplication if 'total_sum' is 'int' or smaller.
 */

#define loop_partitioner_balance_prefix_sums(num_workers, worker_pos, Sums, N, total_sum, local_start_ptr, local_end_ptr, ... /* order_decreasing */)    \
do {                                                                                                                                                     \
	long __attribute__((unused)) _order_decreasing = DEFAULT_ARG_1(0, ##__VA_ARGS__);                                                                \
	long _i_s, _i_e;                                                                                                                                 \
	long _index_lower_value, _index_upper_value;                                                                                                     \
	__auto_type _target = total_sum;                                                                                                                 \
	__auto_type _target_next = total_sum;                                                                                                            \
	_target = (total_sum * ((long) worker_pos)) / ((long) num_workers);                                                                              \
	_target_next = (total_sum * ((long) worker_pos+1)) / ((long) num_workers);                                                                       \
	_index_lower_value = (_order_decreasing) ? N-1 : 0;                                                                                              \
	_index_upper_value = (_order_decreasing) ? 0 : N-1;                                                                                              \
                                                                                                                                                         \
	if (worker_pos == 0)                                                                                                                             \
		_i_s = (_order_decreasing) ? N : 0;                                                                                                      \
	else                                                                                                                                             \
		_i_s = binary_search(Sums, _index_lower_value, _index_upper_value, _target, NULL, NULL);                                                 \
                                                                                                                                                         \
	if (worker_pos == num_workers - 1)                                                                                                               \
		_i_e = (_order_decreasing) ? 0 : N;                                                                                                      \
	else                                                                                                                                             \
		_i_e = binary_search(Sums, _index_lower_value, _index_upper_value, _target_next, NULL, NULL);                                            \
                                                                                                                                                         \
	*local_start_ptr = (_order_decreasing) ? _i_e : _i_s;                                                                                            \
	*local_end_ptr = (_order_decreasing) ? _i_s : _i_e;                                                                                              \
} while (0)


//==========================================================================================================================================
//= Balance Mixed
//==========================================================================================================================================


#define _loop_partitioner_balance_cmp(target, A, i)                                    \
({                                                                                     \
	long _iters = (_order_decreasing) ? N - 1 - i : i;                             \
	double _iters_cost = _loop_partitioner_balance_i_vs_work * _iters;             \
	(target > A[i] + _iters_cost) ? 1 : (target < A[i] + _iters_cost) ? -1 : 0;    \
})

#define _loop_partitioner_balance_dist(target, A, i)                          \
({                                                                            \
	long _iters = (_order_decreasing) ? N - 1 - i : i;                    \
	double _iters_cost = _loop_partitioner_balance_i_vs_work * _iters;    \
	ABS(target - A[i] - _iters_cost);                                     \
})

/* Necessary to cast values to 'double', because multiplying with 'iter_vs_work' might overflow the actual type.
 */
#define loop_partitioner_balance(num_workers, worker_pos, iter_vs_work, Sums, N, total_sum, local_start_ptr, local_end_ptr, ... /* order_decreasing */)                         \
do {                                                                                                                                                                            \
	long _order_decreasing = DEFAULT_ARG_1(0, ##__VA_ARGS__);                                                                                                               \
	long _i_s, _i_e;                                                                                                                                                        \
	long _index_lower_value, _index_upper_value;                                                                                                                            \
	double _target = total_sum;                                                                                                                                             \
	double _target_next = total_sum;                                                                                                                                        \
	double _loop_partitioner_balance_i_vs_work = iter_vs_work;                                                                                                              \
	_target = ((total_sum + _loop_partitioner_balance_i_vs_work * N) * ((long) worker_pos)) / ((long) num_workers);                                                         \
	_target_next = ((total_sum + _loop_partitioner_balance_i_vs_work * N) * ((long) worker_pos+1)) / ((long) num_workers);                                                  \
	_index_lower_value = (_order_decreasing) ? N-1 : 0;                                                                                                                     \
	_index_upper_value = (_order_decreasing) ? 0 : N-1;                                                                                                                     \
                                                                                                                                                                                \
	if (worker_pos == 0)                                                                                                                                                    \
		_i_s = (_order_decreasing) ? N : 0;                                                                                                                             \
	else                                                                                                                                                                    \
		_i_s = binary_search(Sums, _index_lower_value, _index_upper_value, _target, NULL, NULL, _loop_partitioner_balance_cmp, _loop_partitioner_balance_dist);         \
                                                                                                                                                                                \
	if (worker_pos == num_workers - 1)                                                                                                                                      \
		_i_e = (_order_decreasing) ? 0 : N;                                                                                                                             \
	else                                                                                                                                                                    \
		_i_e = binary_search(Sums, _index_lower_value, _index_upper_value, _target_next, NULL, NULL, _loop_partitioner_balance_cmp, _loop_partitioner_balance_dist);    \
                                                                                                                                                                                \
	*local_start_ptr = (_order_decreasing) ? _i_e : _i_s;                                                                                                                   \
	*local_end_ptr = (_order_decreasing) ? _i_s : _i_e;                                                                                                                     \
} while (0)


#endif /* PARALLEL_UTILS_H */

