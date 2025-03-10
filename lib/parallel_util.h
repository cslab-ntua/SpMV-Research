#ifndef PARALLEL_UTILS_H
#define PARALLEL_UTILS_H

#include "macros/cpp_defines.h"
#include "macros/macrolib.h"


//==========================================================================================================================================
//------------------------------------------------------------------------------------------------------------------------------------------
//-                                                         Loop Partitioning                                                              -
//------------------------------------------------------------------------------------------------------------------------------------------
//==========================================================================================================================================


//==========================================================================================================================================
//= Balance Loops
//==========================================================================================================================================


/* Sets the local start and end iterations of the worker.
 * - End is exclusive.
 */

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


/* Generic interface for the integer pointers and optional arguments.
 * - End is exclusive.
 */

#define loop_partitioner_balance_iterations(_num_workers, _worker_pos, _start, _end, _local_start_ptr, _local_end_ptr, ... /* _increment, _num_iterations_ptr */)    \
do {                                                                                                                                                                 \
	/* We can't rename va args, so they need to be first and start with _. */                                                                                    \
	long _increment = DEFAULT_ARG_1((((_start) < (_end)) ? 1 : -1), ##__VA_ARGS__);                                                                              \
	long _unused = 0;                                                                                                                                            \
	__auto_type _num_iterations_ptr = DEFAULT_ARG_2(&_unused, ##__VA_ARGS__);                                                                                    \
	RENAME((_num_workers, num_workers), (_worker_pos, worker_pos), (_start, start), (_end, end),                                                                 \
			(_local_start_ptr, local_start_ptr), (_local_end_ptr, local_end_ptr),                                                                        \
			(_increment, increment), (_num_iterations_ptr, num_iterations_ptr));                                                                         \
	long num_iters = 0;                                                                                                                                          \
	long l_s, l_e;                                                                                                                                               \
	loop_partitioner_balance_iterations_base(num_workers, worker_pos, start, end, increment, &l_s, &l_e, &num_iters);                                            \
	if (local_start_ptr == NULL)                                                                                                                                 \
		error("'local_start_ptr' is NULL");                                                                                                                  \
	if (local_end_ptr == NULL)                                                                                                                                   \
		error("'local_end_ptr' is NULL");                                                                                                                    \
	*local_start_ptr = l_s;                                                                                                                                      \
	*local_end_ptr = l_e;                                                                                                                                        \
	if (num_iterations_ptr != NULL)                                                                                                                              \
		*num_iterations_ptr = num_iters;                                                                                                                     \
} while (0)


//==========================================================================================================================================
//= Balance Prefix Sums
//==========================================================================================================================================


/* Notes:
 * - End is exclusive.
 *   'Sums[N]' is NEVER touched, so the 'total_sum' has to be given explicitly.
 * - 'Sums' is a prefix sum array of the iteration weights.
 *   It can start from any number, but the 'total_sum' has to be correct, i.e., Sums[N] - Sums[0].
 * - 'order_decreasing': Whether the prefix sums are in reverse order. The indices returned are always: start <= end.
 * - 'num_workers' and 'worker_pos' are always integers, so we transform them to 'long',
 *   to avoid overflows in the multiplication if 'total_sum' is 'int' or smaller.
 */

#define loop_partitioner_balance_prefix_sums(_num_workers, _worker_pos, _Sums, _N, _total_sum, _local_start_ptr, _local_end_ptr, ... /* _order_decreasing */)    \
do {                                                                                                                                                             \
	long _order_decreasing = DEFAULT_ARG_1(0, ##__VA_ARGS__);                                                                                                \
	RENAME((_num_workers, num_workers, long), (_worker_pos, worker_pos, long), (_Sums, Sums), (_N, N),                                                       \
	       (_total_sum, total_sum), (_local_start_ptr, local_start_ptr), (_local_end_ptr, local_end_ptr), (_order_decreasing, order_decreasing));            \
	long i_s, i_e;                                                                                                                                           \
	long index_lower_value, index_upper_value;                                                                                                               \
	typeof(total_sum) target, target_next;                                                                                                                   \
                                                                                                                                                                 \
	if (N < 1)                                                                                                                                               \
		error("Empty Sums array.");                                                                                                                      \
	target = Sums[0] + (total_sum * worker_pos) / num_workers;                                                                                               \
	target_next = Sums[0] + (total_sum * (worker_pos+1)) / num_workers;                                                                                      \
	index_lower_value = (order_decreasing) ? N-1 : 0;                                                                                                        \
	index_upper_value = (order_decreasing) ? 0 : N-1;                                                                                                        \
                                                                                                                                                                 \
	if (worker_pos == 0)                                                                                                                                     \
		i_s = (order_decreasing) ? N : 0;                                                                                                                \
	else                                                                                                                                                     \
		i_s = macros_binary_search(Sums, index_lower_value, index_upper_value, target, NULL, NULL);                                                      \
                                                                                                                                                                 \
	if (worker_pos == num_workers - 1)                                                                                                                       \
		i_e = (order_decreasing) ? 0 : N;                                                                                                                \
	else                                                                                                                                                     \
		i_e = macros_binary_search(Sums, index_lower_value, index_upper_value, target_next, NULL, NULL);                                                 \
                                                                                                                                                                 \
	*local_start_ptr = (order_decreasing) ? i_e : i_s;                                                                                                       \
	*local_end_ptr = (order_decreasing) ? i_s : i_e;                                                                                                         \
} while (0)


//==========================================================================================================================================
//= Balance Mixed
//==========================================================================================================================================


#define loop_partitioner_balance_cmp(target, A, i)                                   \
({                                                                                   \
	long iters = (order_decreasing) ? N - 1 - i : i;                             \
	double iters_cost = iter_vs_work * iters;                                    \
	(target > A[i] + iters_cost) ? 1 : (target < A[i] + iters_cost) ? -1 : 0;    \
})

#define loop_partitioner_balance_dist(target, A, i)         \
({                                                          \
	long iters = (order_decreasing) ? N - 1 - i : i;    \
	double iters_cost = iter_vs_work * iters;           \
	macros_abs(target - A[i] - iters_cost);             \
})

/* Necessary to cast target values to 'double', because multiplying with 'iter_vs_work' might overflow the actual type.
 */
#define loop_partitioner_balance(_num_workers, _worker_pos, _iter_vs_work, _Sums, _N, _total_sum, _local_start_ptr, _local_end_ptr, ... /* _order_decreasing */)                 \
do {                                                                                                                                                                             \
	long _order_decreasing = DEFAULT_ARG_1(0, ##__VA_ARGS__);                                                                                                                \
	RENAME((_num_workers, num_workers, long), (_worker_pos, worker_pos, long), (_iter_vs_work, iter_vs_work, double), (_Sums, Sums),                                         \
	       (_N, N), (_total_sum, total_sum), (_local_start_ptr, local_start_ptr), (_local_end_ptr, local_end_ptr), (_order_decreasing, order_decreasing));                   \
	long i_s, i_e;                                                                                                                                                           \
	long index_lower_value, index_upper_value;                                                                                                                               \
	double target, target_next;                                                                                                                                              \
                                                                                                                                                                                 \
	if (N < 1)                                                                                                                                                               \
		error("Empty Sums array.");                                                                                                                                      \
	target = Sums[0] + ((total_sum + iter_vs_work * N) * worker_pos) / num_workers;                                                                                          \
	target_next = Sums[0] + ((total_sum + iter_vs_work * N) * (worker_pos+1)) / num_workers;                                                                                 \
	index_lower_value = (order_decreasing) ? N-1 : 0;                                                                                                                        \
	index_upper_value = (order_decreasing) ? 0 : N-1;                                                                                                                        \
                                                                                                                                                                                 \
	if (worker_pos == 0)                                                                                                                                                     \
		i_s = (order_decreasing) ? N : 0;                                                                                                                                \
	else                                                                                                                                                                     \
		i_s = macros_binary_search(Sums, index_lower_value, index_upper_value, target, NULL, NULL, loop_partitioner_balance_cmp, loop_partitioner_balance_dist);         \
                                                                                                                                                                                 \
	if (worker_pos == num_workers - 1)                                                                                                                                       \
		i_e = (order_decreasing) ? 0 : N;                                                                                                                                \
	else                                                                                                                                                                     \
		i_e = macros_binary_search(Sums, index_lower_value, index_upper_value, target_next, NULL, NULL, loop_partitioner_balance_cmp, loop_partitioner_balance_dist);    \
                                                                                                                                                                                 \
	*local_start_ptr = (order_decreasing) ? i_e : i_s;                                                                                                                       \
	*local_end_ptr = (order_decreasing) ? i_s : i_e;                                                                                                                         \
} while (0)


#endif /* PARALLEL_UTILS_H */

