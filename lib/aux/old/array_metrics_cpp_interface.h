#ifndef ARRAY_METRICS_H
#define ARRAY_METRICS_H

#include <math.h>

#include "macros/cpp_defines.h"
#include "macros/macrolib.h"
#include "genlib.h"


// Index-Value Pair
struct ARRAY_METRICS_ivp {
	long i;
	double val;
};

struct ARRAY_METRICS_2ivp {
	long i_1;
	double val_1;
	long i_2;
	double val_2;
};


struct ARRAY_METRICS_ivp ARRAY_METRICS_thread_reduce_ivp(struct ARRAY_METRICS_ivp (*reduce_fun)(struct ARRAY_METRICS_ivp, struct ARRAY_METRICS_ivp), struct ARRAY_METRICS_ivp partial, struct ARRAY_METRICS_ivp zero);

struct ARRAY_METRICS_2ivp ARRAY_METRICS_thread_reduce_2ivp(struct ARRAY_METRICS_2ivp (*reduce_fun)(struct ARRAY_METRICS_2ivp, struct ARRAY_METRICS_2ivp), struct ARRAY_METRICS_2ivp partial, struct ARRAY_METRICS_2ivp zero);

double ARRAY_METRICS_thread_reduce_double(double (*reduce_fun)(double, double), double partial, double zero);


// double array_metrics_thread_reduce(double (*reduce_fun)(double, double), double partial);
#define array_metrics_thread_reduce(reduce_fun, partial, zero)                          \
({                                                                                      \
	__auto_type __fun = _Generic((partial),                                         \
			struct ARRAY_METRICS_ivp: ARRAY_METRICS_thread_reduce_ivp,      \
			struct ARRAY_METRICS_2ivp: ARRAY_METRICS_thread_reduce_2ivp,    \
			default: ARRAY_METRICS_thread_reduce_double                     \
		);                                                                      \
	__fun(reduce_fun, partial, zero);                                               \
})


//==========================================================================================================================================
//= Array Metrics-Errors Macros
//==========================================================================================================================================


#define __array_metrics_struct_members(pos, get_val_as_double, static_args, var_ptr, kernel_name, ...)    \
	typeof(ARRAY_METRICS_ ## kernel_name ## _map (0, UNPACK(static_args), ##__VA_ARGS__, get_val_as_double)) kernel_name ## _ ## pos

#define __array_metrics_struct_init(pos, static_args, var_ptr, kernel_name, ...)    \
	__array_metrics_vals.kernel_name ## _ ## pos = ARRAY_METRICS_ ## kernel_name ## _zero;

#define __array_metrics_map_reduce(pos, i, get_val_as_double, static_args, var_ptr, kernel_name, ...)                                                                                                                                                \
do {                                                                                                                                                                                                                                                 \
	__array_metrics_vals.kernel_name ## _ ## pos = ARRAY_METRICS_ ## kernel_name ## _reduce (__array_metrics_vals.kernel_name ## _ ## pos, ARRAY_METRICS_ ## kernel_name ## _map (i, UNPACK(static_args), ##__VA_ARGS__, get_val_as_double));    \
} while (0)

#define __array_metrics_thread_reduce(pos, var_ptr, kernel_name, ...)                                                                                                                                                  \
do {                                                                                                                                                                                                                   \
	__array_metrics_vals.kernel_name ## _ ## pos = array_metrics_thread_reduce(ARRAY_METRICS_ ## kernel_name ## _reduce, __array_metrics_vals.kernel_name ## _ ## pos, ARRAY_METRICS_ ## kernel_name ## _zero);    \
} while (0)

#define __array_metrics_output(pos, get_val_as_double, static_args, var_ptr, kernel_name, ...)                                                                          \
do {                                                                                                                                                                    \
	*(var_ptr) = ARRAY_METRICS_ ## kernel_name ## _output (__array_metrics_vals.kernel_name ## _ ## pos, UNPACK(static_args), ##__VA_ARGS__, get_val_as_double);    \
} while (0)

#define __array_metrics(parallel, output_single, get_val_as_double, i_start, i_end, static_args, tuples_var_kernel_args)                                              \
do {                                                                                                                                                                  \
	struct {                                                                                                                                                      \
		FOREACH_AS_STMT_UNGUARDED(1, __array_metrics_struct_members, (get_val_as_double, static_args), tuples_var_kernel_args, )                              \
	} __array_metrics_vals;                                                                                                                                       \
	long __array_metrics_i, __array_metrics_i_s, __array_metrics_i_e;                                                                                             \
	FOREACH_AS_STMT_UNGUARDED(1, __array_metrics_struct_init, (static_args), tuples_var_kernel_args, )                                                            \
	if (parallel)                                                                                                                                                 \
		loop_partitioner_balance_iterations(safe_omp_get_num_threads(), omp_get_thread_num(), i_start, i_end, &__array_metrics_i_s, &__array_metrics_i_e);    \
	else                                                                                                                                                          \
	{                                                                                                                                                             \
		__array_metrics_i_s = i_start;                                                                                                                        \
		__array_metrics_i_e = i_end;                                                                                                                          \
	}                                                                                                                                                             \
	for (__array_metrics_i=__array_metrics_i_s;__array_metrics_i<__array_metrics_i_e;__array_metrics_i++)                                                         \
	{                                                                                                                                                             \
		FOREACH_AS_STMT_UNGUARDED(1, __array_metrics_map_reduce, (__array_metrics_i, get_val_as_double, static_args), tuples_var_kernel_args, )               \
	}                                                                                                                                                             \
	if (parallel)                                                                                                                                                 \
	{                                                                                                                                                             \
		FOREACH_AS_STMT_UNGUARDED(1, __array_metrics_thread_reduce, , tuples_var_kernel_args, )                                                               \
	}                                                                                                                                                             \
	if (parallel && output_single)                                                                                                                                \
	{                                                                                                                                                             \
		_Pragma("omp single nowait")                                                                                                                          \
		{                                                                                                                                                     \
			FOREACH_AS_STMT_UNGUARDED(1, __array_metrics_output, (get_val_as_double, static_args), tuples_var_kernel_args, )                              \
		}                                                                                                                                                     \
	}                                                                                                                                                             \
	else                                                                                                                                                          \
	{                                                                                                                                                             \
		FOREACH_AS_STMT_UNGUARDED(1, __array_metrics_output, (get_val_as_double, static_args), tuples_var_kernel_args, )                                      \
	}                                                                                                                                                             \
} while (0)


#define array_seg_metrics_serial(A, i_start, i_end, tuples_var_kernel_args, ...)    \
	__array_metrics(0, 0, DEFAULT_ARG_1(gen_functor_basic_type_to_double(A), ##__VA_ARGS__), i_start, i_end, (A, i_start, i_end), tuples_var_kernel_args)

#define array_metrics_serial(A, N, tuples_var_kernel_args, ...)    \
	__array_metrics(0, 0, DEFAULT_ARG_1(gen_functor_basic_type_to_double(A), ##__VA_ARGS__), 0, N, (A, 0, N), tuples_var_kernel_args)

#define array_seg_metrics_parallel(A, i_start, i_end, tuples_var_kernel_args, ...)    \
	__array_metrics(1, 0, DEFAULT_ARG_1(gen_functor_basic_type_to_double(A), ##__VA_ARGS__), i_start, i_end, (A, i_start, i_end), tuples_var_kernel_args)

#define array_metrics_parallel(A, N, tuples_var_kernel_args, ...)    \
	__array_metrics(1, 0, DEFAULT_ARG_1(gen_functor_basic_type_to_double(A), ##__VA_ARGS__), 0, N, (A, 0, N), tuples_var_kernel_args)

#define array_seg_metrics(A, i_start, i_end, tuples_var_kernel_args, ...)                                                                                                 \
do {                                                                                                                                                                      \
	_Pragma("omp parallel")                                                                                                                                           \
	{                                                                                                                                                                 \
		__array_metrics(1, 1, DEFAULT_ARG_1(gen_functor_basic_type_to_double(A), ##__VA_ARGS__), i_start, i_end, (A, i_start, i_end), tuples_var_kernel_args);    \
	}                                                                                                                                                                 \
} while (0)

#define array_metrics(A, N, tuples_var_kernel_args, ...)                                                                                              \
do {                                                                                                                                                  \
	_Pragma("omp parallel")                                                                                                                       \
	{                                                                                                                                             \
		__array_metrics(1, 1, DEFAULT_ARG_1(gen_functor_basic_type_to_double(A), ##__VA_ARGS__), 0, N, (A, 0, N), tuples_var_kernel_args);    \
	}                                                                                                                                             \
} while (0)


#define array_seg_errors_serial(A, F, i_start, i_end, tuples_var_kernel_args, ...)    \
	__array_metrics(0, 0, DEFAULT_ARG_1(gen_functor_basic_type_to_double(A), ##__VA_ARGS__), i_start, i_end, (A, F, i_start, i_end), tuples_var_kernel_args)

#define array_errors_serial(A, F, N, tuples_var_kernel_args, ...)    \
	__array_metrics(0, 0, DEFAULT_ARG_1(gen_functor_basic_type_to_double(A), ##__VA_ARGS__), 0, N, (A, F, 0, N), tuples_var_kernel_args)

#define array_seg_errors_parallel(A, F, i_start, i_end, tuples_var_kernel_args, ...)    \
	__array_metrics(1, 0, DEFAULT_ARG_1(gen_functor_basic_type_to_double(A), ##__VA_ARGS__), i_start, i_end, (A, F, i_start, i_end), tuples_var_kernel_args)

#define array_errors_parallel(A, F, N, tuples_var_kernel_args, ...)    \
	__array_metrics(1, 0, DEFAULT_ARG_1(gen_functor_basic_type_to_double(A), ##__VA_ARGS__), 0, N, (A, F, 0, N), tuples_var_kernel_args)

#define array_seg_errors(A, F, i_start, i_end, tuples_var_kernel_args, ...)                                                                                                  \
do {                                                                                                                                                                         \
	_Pragma("omp parallel")                                                                                                                                              \
	{                                                                                                                                                                    \
		__array_metrics(1, 1, DEFAULT_ARG_1(gen_functor_basic_type_to_double(A), ##__VA_ARGS__), i_start, i_end, (A, F, i_start, i_end), tuples_var_kernel_args);    \
	}                                                                                                                                                                    \
} while (0)

#define array_errors(A, F, N, tuples_var_kernel_args, ...)                                                                                               \
do {                                                                                                                                                     \
	_Pragma("omp parallel")                                                                                                                          \
	{                                                                                                                                                \
		__array_metrics(1, 1, DEFAULT_ARG_1(gen_functor_basic_type_to_double(A), ##__VA_ARGS__), 0, N, (A, F, 0, N), tuples_var_kernel_args);    \
	}                                                                                                                                                \
} while (0)


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//------------------------------------------------------------------------------------------------------------------------------------------
//-                                                               Norms                                                                    -
//------------------------------------------------------------------------------------------------------------------------------------------
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


//==========================================================================================================================================
//= P-Norm
//==========================================================================================================================================


static const double ARRAY_METRICS_pnorm_zero = 0.0;
double ARRAY_METRICS_pnorm_map(long i, void * A, long i_start, long i_end, double p, double (* get_val_as_double)(void * A, long i));
double ARRAY_METRICS_pnorm_reduce(double sum, double val);
double ARRAY_METRICS_pnorm_output(double agg, void * A, long i_start, long i_end, double p, double (* get_val_as_double)(void * A, long i));
double ARRAY_METRICS_pnorm_serial(void * A, long i_start, long i_end, double p, double (* get_val_as_double)(void * A, long i));
double ARRAY_METRICS_pnorm_parallel(void * A, long i_start, long i_end, double p, double (* get_val_as_double)(void * A, long i));
double ARRAY_METRICS_pnorm(void * A, long i_start, long i_end, double p, double (* get_val_as_double)(void * A, long i));

#define array_seg_pnorm_serial(A, i_start, i_end, p, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_pnorm_serial(A, i_start, i_end, p, DEFAULT_ARG_1(gen_functor_basic_type_to_double(A), ##__VA_ARGS__))

#define array_pnorm_serial(A, N, ... /* get_val_as_double() */)    \
	array_seg_pnorm_serial(A, 0, N, ##__VA_ARGS__)

#define array_seg_pnorm_parallel(A, i_start, i_end, p, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_pnorm_parallel(A, i_start, i_end, p, DEFAULT_ARG_1(gen_functor_basic_type_to_double(A), ##__VA_ARGS__))

#define array_pnorm_parallel(A, N, ... /* get_val_as_double() */)    \
	array_seg_pnorm_parallel(A, 0, N, ##__VA_ARGS__)

#define array_seg_pnorm(A, i_start, i_end, p, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_pnorm(A, i_start, i_end, p, DEFAULT_ARG_1(gen_functor_basic_type_to_double(A), ##__VA_ARGS__))

#define array_pnorm(A, N, ... /* get_val_as_double() */)    \
	array_seg_pnorm(A, 0, N, ##__VA_ARGS__)


//==========================================================================================================================================
//= Min
//==========================================================================================================================================


static const struct ARRAY_METRICS_ivp ARRAY_METRICS_min_zero = {.i=0, .val=INFINITY};
struct ARRAY_METRICS_ivp ARRAY_METRICS_min_map(long i, void * A, long i_start, long i_end, long * min_idx_out, double (* get_val_as_double)(void * A, long i));
struct ARRAY_METRICS_ivp ARRAY_METRICS_min_reduce(struct ARRAY_METRICS_ivp agg, struct ARRAY_METRICS_ivp val);
double ARRAY_METRICS_min_output(struct ARRAY_METRICS_ivp agg, void * A, long i_start, long i_end, long * min_idx_out, double (* get_val_as_double)(void * A, long i));
double ARRAY_METRICS_min_serial(void * A, long i_start, long i_end, long * min_idx_out, double (* get_val_as_double)(void * A, long i));
double ARRAY_METRICS_min_parallel(void * A, long i_start, long i_end, long * min_idx_out, double (* get_val_as_double)(void * A, long i));
double ARRAY_METRICS_min(void * A, long i_start, long i_end, long * min_idx_out, double (* get_val_as_double)(void * A, long i));

#define array_seg_min_serial(A, i_start, i_end, min_out, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_min_serial(A, i_start, i_end, min_out, DEFAULT_ARG_1(gen_functor_basic_type_to_double(A), ##__VA_ARGS__))

#define array_min_serial(A, N, min_out, ... /* get_val_as_double() */)    \
	array_seg_min_serial(A, 0, N, min_out, ##__VA_ARGS__)

#define array_seg_min_parallel(A, i_start, i_end, min_out, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_min_parallel(A, i_start, i_end, min_out, DEFAULT_ARG_1(gen_functor_basic_type_to_double(A), ##__VA_ARGS__))

#define array_min_parallel(A, N, min_out, ... /* get_val_as_double() */)    \
	array_seg_min_parallel(A, 0, N, min_out, ##__VA_ARGS__)

#define array_seg_min(A, i_start, i_end, min_out, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_min(A, i_start, i_end, min_out, DEFAULT_ARG_1(gen_functor_basic_type_to_double(A), ##__VA_ARGS__))

#define array_min(A, N, min_out, ... /* get_val_as_double() */)    \
	array_seg_min(A, 0, N, min_out, ##__VA_ARGS__)


//==========================================================================================================================================
//= Max
//==========================================================================================================================================


static const struct ARRAY_METRICS_ivp ARRAY_METRICS_max_zero = {.i=0, .val=-INFINITY};
struct ARRAY_METRICS_ivp ARRAY_METRICS_max_map(long i, void * A, long i_start, long i_end, long * max_idx_out, double (* get_val_as_double)(void * A, long i));
struct ARRAY_METRICS_ivp ARRAY_METRICS_max_reduce(struct ARRAY_METRICS_ivp agg, struct ARRAY_METRICS_ivp val);
double ARRAY_METRICS_max_output(struct ARRAY_METRICS_ivp agg, void * A, long i_start, long i_end, long * max_idx_out, double (* get_val_as_double)(void * A, long i));
double ARRAY_METRICS_max_serial(void * A, long i_start, long i_end, long * max_idx_out, double (* get_val_as_double)(void * A, long i));
double ARRAY_METRICS_max_parallel(void * A, long i_start, long i_end, long * max_idx_out, double (* get_val_as_double)(void * A, long i));
double ARRAY_METRICS_max(void * A, long i_start, long i_end, long * max_idx_out, double (* get_val_as_double)(void * A, long i));

#define array_seg_max_serial(A, i_start, i_end, max_out, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_max_serial(A, i_start, i_end, max_out, DEFAULT_ARG_1(gen_functor_basic_type_to_double(A), ##__VA_ARGS__))

#define array_max_serial(A, N, max_out, ... /* get_val_as_double() */)    \
	array_seg_max_serial(A, 0, N, max_out, ##__VA_ARGS__)

#define array_seg_max_parallel(A, i_start, i_end, max_out, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_max_parallel(A, i_start, i_end, max_out, DEFAULT_ARG_1(gen_functor_basic_type_to_double(A), ##__VA_ARGS__))

#define array_max_parallel(A, N, max_out, ... /* get_val_as_double() */)    \
	array_seg_max_parallel(A, 0, N, max_out, ##__VA_ARGS__)

#define array_seg_max(A, i_start, i_end, max_out, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_max(A, i_start, i_end, max_out, DEFAULT_ARG_1(gen_functor_basic_type_to_double(A), ##__VA_ARGS__))

#define array_max(A, N, max_out, ... /* get_val_as_double() */)    \
	array_seg_max(A, 0, N, max_out, ##__VA_ARGS__)


//==========================================================================================================================================
//= Min - Max
//==========================================================================================================================================


static const struct ARRAY_METRICS_2ivp ARRAY_METRICS_min_max_zero = {.i_1=0, .val_1=INFINITY, .i_2=0, .val_2=-INFINITY};
struct ARRAY_METRICS_2ivp ARRAY_METRICS_min_max_map(long i, void * A, long i_start, long i_end, double * min_out, long * min_idx_out, double * max_out, long * max_idx_out, double (* get_val_as_double)(void * A, long i));
struct ARRAY_METRICS_2ivp ARRAY_METRICS_min_max_reduce(struct ARRAY_METRICS_2ivp agg, struct ARRAY_METRICS_2ivp val);
struct ARRAY_METRICS_2ivp ARRAY_METRICS_min_max_output(struct ARRAY_METRICS_2ivp agg, void * A, long i_start, long i_end, double * min_out, long * min_idx_out, double * max_out, long * max_idx_out, double (* get_val_as_double)(void * A, long i));

struct ARRAY_METRICS_2ivp ARRAY_METRICS_min_max_serial(void * A, long i_start, long i_end, double * min_out, long * min_idx_out, double * max_out, long * max_idx_out, double (* get_val_as_double)(void * A, long i));
struct ARRAY_METRICS_2ivp ARRAY_METRICS_min_max_parallel(void * A, long i_start, long i_end, double * min_out, long * min_idx_out, double * max_out, long * max_idx_out, double (* get_val_as_double)(void * A, long i));
struct ARRAY_METRICS_2ivp ARRAY_METRICS_min_max(void * A, long i_start, long i_end, double * min_out, long * min_idx_out, double * max_out, long * max_idx_out, double (* get_val_as_double)(void * A, long i));

#define array_seg_min_max_serial(A, i_start, i_end, min_out, min_idx_out, max_out, max_idx_out, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_min_max_serial(A, i_start, i_end, min_out, min_idx_out, max_out, max_idx_out, DEFAULT_ARG_1(gen_functor_basic_type_to_double(A), ##__VA_ARGS__))

#define array_min_max_serial(A, N, min_out, min_idx_out, max_out, max_idx_out, ... /* get_val_as_double() */)    \
	array_seg_min_max_serial(A, 0, N, min_out, min_idx_out, max_out, max_idx_out, ##__VA_ARGS__)

#define array_seg_min_max_parallel(A, i_start, i_end, min_out, min_idx_out, max_out, max_idx_out, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_min_max_parallel(A, i_start, i_end, min_out, min_idx_out, max_out, max_idx_out, DEFAULT_ARG_1(gen_functor_basic_type_to_double(A), ##__VA_ARGS__))

#define array_min_max_parallel(A, N, min_out, min_idx_out, max_out, max_idx_out, ... /* get_val_as_double() */)    \
	array_seg_min_max_parallel(A, 0, N, min_out, min_idx_out, max_out, max_idx_out, ##__VA_ARGS__)

#define array_seg_min_max(A, i_start, i_end, min_out, min_idx_out, max_out, max_idx_out, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_min_max(A, i_start, i_end, min_out, min_idx_out, max_out, max_idx_out, DEFAULT_ARG_1(gen_functor_basic_type_to_double(A), ##__VA_ARGS__))

#define array_min_max(A, N, min_out, min_idx_out, max_out, max_idx_out, ... /* get_val_as_double() */)    \
	array_seg_min_max(A, 0, N, min_out, min_idx_out, max_out, max_idx_out, ##__VA_ARGS__)


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//------------------------------------------------------------------------------------------------------------------------------------------
//-                                                          Central Tendency                                                              -
//------------------------------------------------------------------------------------------------------------------------------------------
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


//==========================================================================================================================================
//= Mean
//==========================================================================================================================================


static const double ARRAY_METRICS_mean_zero = 0.0;
double ARRAY_METRICS_mean_map(long i, void * A, long i_start, long i_end, double (* get_val_as_double)(void * A, long i));
double ARRAY_METRICS_mean_reduce(double sum, double val);
double ARRAY_METRICS_mean_output(double agg, void * A, long i_start, long i_end, double (* get_val_as_double)(void * A, long i));
double ARRAY_METRICS_mean_serial(void * A, long i_start, long i_end, double (* get_val_as_double)(void * A, long i));
double ARRAY_METRICS_mean_parallel(void * A, long i_start, long i_end, double (* get_val_as_double)(void * A, long i));
double ARRAY_METRICS_mean(void * A, long i_start, long i_end, double (* get_val_as_double)(void * A, long i));

#define array_seg_mean_serial(A, i_start, i_end, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_mean_serial(A, i_start, i_end, DEFAULT_ARG_1(gen_functor_basic_type_to_double(A), ##__VA_ARGS__))

#define array_mean_serial(A, N, ... /* get_val_as_double() */)    \
	array_seg_mean_serial(A, 0, N, ##__VA_ARGS__)

#define array_seg_mean_parallel(A, i_start, i_end, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_mean_parallel(A, i_start, i_end, DEFAULT_ARG_1(gen_functor_basic_type_to_double(A), ##__VA_ARGS__))

#define array_mean_parallel(A, N, ... /* get_val_as_double() */)    \
	array_seg_mean_parallel(A, 0, N, ##__VA_ARGS__)

#define array_seg_mean(A, i_start, i_end, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_mean(A, i_start, i_end, DEFAULT_ARG_1(gen_functor_basic_type_to_double(A), ##__VA_ARGS__))

#define array_mean(A, N, ... /* get_val_as_double() */)    \
	array_seg_mean(A, 0, N, ##__VA_ARGS__)


//==========================================================================================================================================
//= Geometric Mean
//==========================================================================================================================================


static const double ARRAY_METRICS_gmean_zero = 0.0;
double ARRAY_METRICS_gmean_map(long i, void * A, long i_start, long i_end, double (* get_val_as_double)(void * A, long i));
double ARRAY_METRICS_gmean_reduce(double sum, double val);
double ARRAY_METRICS_gmean_output(double agg, void * A, long i_start, long i_end, double (* get_val_as_double)(void * A, long i));
double ARRAY_METRICS_gmean_serial(void * A, long i_start, long i_end, double (* get_val_as_double)(void * A, long i));
double ARRAY_METRICS_gmean_parallel(void * A, long i_start, long i_end, double (* get_val_as_double)(void * A, long i));
double ARRAY_METRICS_gmean(void * A, long i_start, long i_end, double (* get_val_as_double)(void * A, long i));

#define array_seg_gmean_serial(A, i_start, i_end, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_gmean_serial(A, i_start, i_end, DEFAULT_ARG_1(gen_functor_basic_type_to_double(A), ##__VA_ARGS__))

#define array_gmean_serial(A, N, ... /* get_val_as_double() */)    \
	array_seg_gmean_serial(A, 0, N, ##__VA_ARGS__)

#define array_seg_gmean_parallel(A, i_start, i_end, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_gmean_parallel(A, i_start, i_end, DEFAULT_ARG_1(gen_functor_basic_type_to_double(A), ##__VA_ARGS__))

#define array_gmean_parallel(A, N, ... /* get_val_as_double() */)    \
	array_seg_gmean_parallel(A, 0, N, ##__VA_ARGS__)

#define array_seg_gmean(A, i_start, i_end, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_gmean(A, i_start, i_end, DEFAULT_ARG_1(gen_functor_basic_type_to_double(A), ##__VA_ARGS__))

#define array_gmean(A, N, ... /* get_val_as_double() */)    \
	array_seg_gmean(A, 0, N, ##__VA_ARGS__)


//==========================================================================================================================================
//= Harmonic Mean
//==========================================================================================================================================


static const double ARRAY_METRICS_hmean_zero = 0.0;
double ARRAY_METRICS_hmean_map(long i, void * A, long i_start, long i_end, double (* get_val_as_double)(void * A, long i));
double ARRAY_METRICS_hmean_reduce(double sum, double val);
double ARRAY_METRICS_hmean_output(double agg, void * A, long i_start, long i_end, double (* get_val_as_double)(void * A, long i));
double ARRAY_METRICS_hmean_serial(void * A, long i_start, long i_end, double (* get_val_as_double)(void * A, long i));
double ARRAY_METRICS_hmean_parallel(void * A, long i_start, long i_end, double (* get_val_as_double)(void * A, long i));
double ARRAY_METRICS_hmean(void * A, long i_start, long i_end, double (* get_val_as_double)(void * A, long i));

#define array_seg_hmean_serial(A, i_start, i_end, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_hmean_serial(A, i_start, i_end, DEFAULT_ARG_1(gen_functor_basic_type_to_double(A), ##__VA_ARGS__))

#define array_hmean_serial(A, N, ... /* get_val_as_double() */)    \
	array_seg_hmean_serial(A, 0, N, ##__VA_ARGS__)

#define array_seg_hmean_parallel(A, i_start, i_end, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_hmean_parallel(A, i_start, i_end, DEFAULT_ARG_1(gen_functor_basic_type_to_double(A), ##__VA_ARGS__))

#define array_hmean_parallel(A, N, ... /* get_val_as_double() */)    \
	array_seg_hmean_parallel(A, 0, N, ##__VA_ARGS__)

#define array_seg_hmean(A, i_start, i_end, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_hmean(A, i_start, i_end, DEFAULT_ARG_1(gen_functor_basic_type_to_double(A), ##__VA_ARGS__))

#define array_hmean(A, N, ... /* get_val_as_double() */)    \
	array_seg_hmean(A, 0, N, ##__VA_ARGS__)


//==========================================================================================================================================
//= Root Mean Square
//==========================================================================================================================================


static const double ARRAY_METRICS_rms_zero = 0.0;
double ARRAY_METRICS_rms_map(long i, void * A, long i_start, long i_end, double (* get_val_as_double)(void * A, long i));
double ARRAY_METRICS_rms_reduce(double sum, double val);
double ARRAY_METRICS_rms_output(double agg, void * A, long i_start, long i_end, double (* get_val_as_double)(void * A, long i));
double ARRAY_METRICS_rms_serial(void * A, long i_start, long i_end, double (* get_val_as_double)(void * A, long i));
double ARRAY_METRICS_rms_parallel(void * A, long i_start, long i_end, double (* get_val_as_double)(void * A, long i));
double ARRAY_METRICS_rms(void * A, long i_start, long i_end, double (* get_val_as_double)(void * A, long i));

#define array_seg_rms_serial(A, i_start, i_end, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_rms_serial(A, i_start, i_end, DEFAULT_ARG_1(gen_functor_basic_type_to_double(A), ##__VA_ARGS__))

#define array_rms_serial(A, N, ... /* get_val_as_double() */)    \
	array_seg_rms_serial(A, 0, N, ##__VA_ARGS__)

#define array_seg_rms_parallel(A, i_start, i_end, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_rms_parallel(A, i_start, i_end, DEFAULT_ARG_1(gen_functor_basic_type_to_double(A), ##__VA_ARGS__))

#define array_rms_parallel(A, N, ... /* get_val_as_double() */)    \
	array_seg_rms_parallel(A, 0, N, ##__VA_ARGS__)

#define array_seg_rms(A, i_start, i_end, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_rms(A, i_start, i_end, DEFAULT_ARG_1(gen_functor_basic_type_to_double(A), ##__VA_ARGS__))

#define array_rms(A, N, ... /* get_val_as_double() */)    \
	array_seg_rms(A, 0, N, ##__VA_ARGS__)


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//------------------------------------------------------------------------------------------------------------------------------------------
//-                                                        Dispersion - Moments                                                            -
//------------------------------------------------------------------------------------------------------------------------------------------
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


//==========================================================================================================================================
//= Mean Absolute Deviation
//==========================================================================================================================================


static const double ARRAY_METRICS_mad_zero = 0.0;
double ARRAY_METRICS_mad_map(long i, void * A, long i_start, long i_end, double mean, double (* get_val_as_double)(void * A, long i));
double ARRAY_METRICS_mad_reduce(double sum, double val);
double ARRAY_METRICS_mad_output(double agg, void * A, long i_start, long i_end, double mean, double (* get_val_as_double)(void * A, long i));
double ARRAY_METRICS_mad_serial(void * A, long i_start, long i_end, double mean, double (* get_val_as_double)(void * A, long i));
double ARRAY_METRICS_mad_parallel(void * A, long i_start, long i_end, double mean, double (* get_val_as_double)(void * A, long i));
double ARRAY_METRICS_mad(void * A, long i_start, long i_end, double mean, double (* get_val_as_double)(void * A, long i));

#define array_seg_mad_serial(A, i_start, i_end, mean, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_mad_serial(A, i_start, i_end, mean, DEFAULT_ARG_1(gen_functor_basic_type_to_double(A), ##__VA_ARGS__))

#define array_mad_serial(A, N, mean, ... /* get_val_as_double() */)    \
	array_seg_mad_serial(A, 0, N, mean, ##__VA_ARGS__)

#define array_seg_mad_parallel(A, i_start, i_end, mean, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_mad_parallel(A, i_start, i_end, mean, DEFAULT_ARG_1(gen_functor_basic_type_to_double(A), ##__VA_ARGS__))

#define array_mad_parallel(A, N, mean, ... /* get_val_as_double() */)    \
	array_seg_mad_parallel(A, 0, N, mean, ##__VA_ARGS__)

#define array_seg_mad(A, i_start, i_end, mean, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_mad(A, i_start, i_end, mean, DEFAULT_ARG_1(gen_functor_basic_type_to_double(A), ##__VA_ARGS__))

#define array_mad(A, N, mean, ... /* get_val_as_double() */)    \
	array_seg_mad(A, 0, N, mean, ##__VA_ARGS__)


//==========================================================================================================================================
//= Variance - Standard Deviation
//==========================================================================================================================================


static const double ARRAY_METRICS_var_zero = 0.0;
double ARRAY_METRICS_var_map(long i, void * A, long i_start, long i_end, double mean, double (* get_val_as_double)(void * A, long i));
double ARRAY_METRICS_var_reduce(double sum, double val);
double ARRAY_METRICS_var_output(double agg, void * A, long i_start, long i_end, double mean, double (* get_val_as_double)(void * A, long i));
double ARRAY_METRICS_var_serial(void * A, long i_start, long i_end, double mean, double (* get_val_as_double)(void * A, long i));
double ARRAY_METRICS_var_parallel(void * A, long i_start, long i_end, double mean, double (* get_val_as_double)(void * A, long i));
double ARRAY_METRICS_var(void * A, long i_start, long i_end, double mean, double (* get_val_as_double)(void * A, long i));

static const double ARRAY_METRICS_std_zero = 0.0;
double ARRAY_METRICS_std_map(long i, void * A, long i_start, long i_end, double mean, double (* get_val_as_double)(void * A, long i));
double ARRAY_METRICS_std_reduce(double sum, double val);
double ARRAY_METRICS_std_output(double agg, void * A, long i_start, long i_end, double mean, double (* get_val_as_double)(void * A, long i));

#define array_seg_var_serial(A, i_start, i_end, mean, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_var_serial(A, i_start, i_end, mean, DEFAULT_ARG_1(gen_functor_basic_type_to_double(A), ##__VA_ARGS__))

#define array_var_serial(A, N, mean, ... /* get_val_as_double() */)    \
	array_seg_var_serial(A, 0, N, mean, ##__VA_ARGS__)

#define array_seg_var_parallel(A, i_start, i_end, mean, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_var_parallel(A, i_start, i_end, mean, DEFAULT_ARG_1(gen_functor_basic_type_to_double(A), ##__VA_ARGS__))

#define array_var_parallel(A, N, mean, ... /* get_val_as_double() */)    \
	array_seg_var_parallel(A, 0, N, mean, ##__VA_ARGS__)

#define array_seg_var(A, i_start, i_end, mean, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_var(A, i_start, i_end, mean, DEFAULT_ARG_1(gen_functor_basic_type_to_double(A), ##__VA_ARGS__))

#define array_var(A, N, mean, ... /* get_val_as_double() */)    \
	array_seg_var(A, 0, N, mean, ##__VA_ARGS__)


#define array_seg_std_serial(A, i_start, i_end, mean, ... /* get_val_as_double() */)    \
	sqrt(array_seg_var_serial(A, i_start, i_end, mean, ##__VA_ARGS__))

#define array_std_serial(A, N, mean, ... /* get_val_as_double() */)    \
	array_seg_std_serial(A, 0, N, mean, ##__VA_ARGS__)

#define array_seg_std_parallel(A, i_start, i_end, mean, ... /* get_val_as_double() */)    \
	sqrt(array_seg_var_parallel(A, i_start, i_end, mean, ##__VA_ARGS__))

#define array_std_parallel(A, N, mean, ... /* get_val_as_double() */)    \
	array_seg_std_parallel(A, 0, N, mean, ##__VA_ARGS__)

#define array_seg_std(A, i_start, i_end, mean, ... /* get_val_as_double() */)    \
	sqrt(array_seg_var(A, i_start, i_end, mean, ##__VA_ARGS__))

#define array_std(A, N, mean, ... /* get_val_as_double() */)    \
	array_seg_std(A, 0, N, mean, ##__VA_ARGS__)


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//------------------------------------------------------------------------------------------------------------------------------------------
//-                                                               Errors                                                                   -
//------------------------------------------------------------------------------------------------------------------------------------------
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


/*
 * A : Actual values
 * F : Forecast values
 */


//==========================================================================================================================================
//= Mean Absolute Error
//==========================================================================================================================================


static const double ARRAY_METRICS_mae_zero = 0.0;
double ARRAY_METRICS_mae_map(long i, void * A, void * F, long i_start, long i_end, double (* get_val_as_double)(void * A, long i));
double ARRAY_METRICS_mae_reduce(double sum, double val);
double ARRAY_METRICS_mae_output(double agg, void * A, void * F, long i_start, long i_end, double (* get_val_as_double)(void * A, long i));
double ARRAY_METRICS_mae_serial(void * A, void * F, long i_start, long i_end, double (* get_val_as_double)(void * A, long i));
double ARRAY_METRICS_mae_parallel(void * A, void * F, long i_start, long i_end, double (* get_val_as_double)(void * A, long i));
double ARRAY_METRICS_mae(void * A, void * F, long i_start, long i_end, double (* get_val_as_double)(void * A, long i));

#define array_seg_mae_serial(A, B, i_start, i_end, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_mae_serial(A, B, i_start, i_end, DEFAULT_ARG_1(gen_functor_basic_type_to_double(A), ##__VA_ARGS__))

#define array_mae_serial(A, N, ... /* get_val_as_double() */)    \
	array_seg_mae_serial(A, B, 0, N, ##__VA_ARGS__)

#define array_seg_mae_parallel(A, B, i_start, i_end, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_mae_parallel(A, B, i_start, i_end, DEFAULT_ARG_1(gen_functor_basic_type_to_double(A), ##__VA_ARGS__))

#define array_mae_parallel(A, B, N, ... /* get_val_as_double() */)    \
	array_seg_mae_parallel(A, B, 0, N, ##__VA_ARGS__)

#define array_seg_mae(A, B, i_start, i_end, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_mae(A, B, i_start, i_end, DEFAULT_ARG_1(gen_functor_basic_type_to_double(A), ##__VA_ARGS__))

#define array_mae(A, B, N, ... /* get_val_as_double() */)    \
	array_seg_mae(A, B, 0, N, ##__VA_ARGS__)


//==========================================================================================================================================
//= Max Absolute Error
//==========================================================================================================================================


static const double ARRAY_METRICS_max_ae_zero = 0.0;
double ARRAY_METRICS_max_ae_map(long i, void * A, void * F, long i_start, long i_end, double (* get_val_as_double)(void * A, long i));
double ARRAY_METRICS_max_ae_reduce(double sum, double val);
double ARRAY_METRICS_max_ae_output(double agg, void * A, void * F, long i_start, long i_end, double (* get_val_as_double)(void * A, long i));
double ARRAY_METRICS_max_ae_serial(void * A, void * F, long i_start, long i_end, double (* get_val_as_double)(void * A, long i));
double ARRAY_METRICS_max_ae_parallel(void * A, void * F, long i_start, long i_end, double (* get_val_as_double)(void * A, long i));
double ARRAY_METRICS_max_ae(void * A, void * F, long i_start, long i_end, double (* get_val_as_double)(void * A, long i));

#define array_seg_max_ae_serial(A, B, i_start, i_end, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_max_ae_serial(A, B, i_start, i_end, DEFAULT_ARG_1(gen_functor_basic_type_to_double(A), ##__VA_ARGS__))

#define array_max_ae_serial(A, N, ... /* get_val_as_double() */)    \
	array_seg_max_ae_serial(A, B, 0, N, ##__VA_ARGS__)

#define array_seg_max_ae_parallel(A, B, i_start, i_end, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_max_ae_parallel(A, B, i_start, i_end, DEFAULT_ARG_1(gen_functor_basic_type_to_double(A), ##__VA_ARGS__))

#define array_max_ae_parallel(A, B, N, ... /* get_val_as_double() */)    \
	array_seg_max_ae_parallel(A, B, 0, N, ##__VA_ARGS__)

#define array_seg_max_ae(A, B, i_start, i_end, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_max_ae(A, B, i_start, i_end, DEFAULT_ARG_1(gen_functor_basic_type_to_double(A), ##__VA_ARGS__))

#define array_max_ae(A, B, N, ... /* get_val_as_double() */)    \
	array_seg_max_ae(A, B, 0, N, ##__VA_ARGS__)


//==========================================================================================================================================
//= Mean Squared Error
//==========================================================================================================================================


static const double ARRAY_METRICS_mse_zero = 0.0;
double ARRAY_METRICS_mse_map(long i, void * A, void * F, long i_start, long i_end, double (* get_val_as_double)(void * A, long i));
double ARRAY_METRICS_mse_reduce(double sum, double val);
double ARRAY_METRICS_mse_output(double agg, void * A, void * F, long i_start, long i_end, double (* get_val_as_double)(void * A, long i));
double ARRAY_METRICS_mse_serial(void * A, void * F, long i_start, long i_end, double (* get_val_as_double)(void * A, long i));
double ARRAY_METRICS_mse_parallel(void * A, void * F, long i_start, long i_end, double (* get_val_as_double)(void * A, long i));
double ARRAY_METRICS_mse(void * A, void * F, long i_start, long i_end, double (* get_val_as_double)(void * A, long i));

#define array_seg_mse_serial(A, B, i_start, i_end, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_mse_serial(A, B, i_start, i_end, DEFAULT_ARG_1(gen_functor_basic_type_to_double(A), ##__VA_ARGS__))

#define array_mse_serial(A, N, ... /* get_val_as_double() */)    \
	array_seg_mse_serial(A, B, 0, N, ##__VA_ARGS__)

#define array_seg_mse_parallel(A, B, i_start, i_end, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_mse_parallel(A, B, i_start, i_end, DEFAULT_ARG_1(gen_functor_basic_type_to_double(A), ##__VA_ARGS__))

#define array_mse_parallel(A, B, N, ... /* get_val_as_double() */)    \
	array_seg_mse_parallel(A, B, 0, N, ##__VA_ARGS__)

#define array_seg_mse(A, B, i_start, i_end, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_mse(A, B, i_start, i_end, DEFAULT_ARG_1(gen_functor_basic_type_to_double(A), ##__VA_ARGS__))

#define array_mse(A, B, N, ... /* get_val_as_double() */)    \
	array_seg_mse(A, B, 0, N, ##__VA_ARGS__)


//==========================================================================================================================================
//= Mean Absolute Relative Error
//==========================================================================================================================================


static const double ARRAY_METRICS_mare_zero = 0.0;
double ARRAY_METRICS_mare_map(long i, void * A, void * F, long i_start, long i_end, double (* get_val_as_double)(void * A, long i));
double ARRAY_METRICS_mare_reduce(double sum, double val);
double ARRAY_METRICS_mare_output(double agg, void * A, void * F, long i_start, long i_end, double (* get_val_as_double)(void * A, long i));
double ARRAY_METRICS_mare_serial(void * A, void * F, long i_start, long i_end, double (* get_val_as_double)(void * A, long i));
double ARRAY_METRICS_mare_parallel(void * A, void * F, long i_start, long i_end, double (* get_val_as_double)(void * A, long i));
double ARRAY_METRICS_mare(void * A, void * F, long i_start, long i_end, double (* get_val_as_double)(void * A, long i));

#define array_seg_mare_serial(A, B, i_start, i_end, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_mare_serial(A, B, i_start, i_end, DEFAULT_ARG_1(gen_functor_basic_type_to_double(A), ##__VA_ARGS__))

#define array_mare_serial(A, N, ... /* get_val_as_double() */)    \
	array_seg_mare_serial(A, B, 0, N, ##__VA_ARGS__)

#define array_seg_mare_parallel(A, B, i_start, i_end, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_mare_parallel(A, B, i_start, i_end, DEFAULT_ARG_1(gen_functor_basic_type_to_double(A), ##__VA_ARGS__))

#define array_mare_parallel(A, B, N, ... /* get_val_as_double() */)    \
	array_seg_mare_parallel(A, B, 0, N, ##__VA_ARGS__)

#define array_seg_mare(A, B, i_start, i_end, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_mare(A, B, i_start, i_end, DEFAULT_ARG_1(gen_functor_basic_type_to_double(A), ##__VA_ARGS__))

#define array_mare(A, B, N, ... /* get_val_as_double() */)    \
	array_seg_mare(A, B, 0, N, ##__VA_ARGS__)


//==========================================================================================================================================
//= Mean Absolute Percentage Error
//==========================================================================================================================================


static const double ARRAY_METRICS_mape_zero = 0.0;
double ARRAY_METRICS_mape_map(long i, void * A, void * F, long i_start, long i_end, double (* get_val_as_double)(void * A, long i));
double ARRAY_METRICS_mape_reduce(double sum, double val);
double ARRAY_METRICS_mape_output(double agg, void * A, void * F, long i_start, long i_end, double (* get_val_as_double)(void * A, long i));
double ARRAY_METRICS_mape_serial(void * A, void * F, long i_start, long i_end, double (* get_val_as_double)(void * A, long i));
double ARRAY_METRICS_mape_parallel(void * A, void * F, long i_start, long i_end, double (* get_val_as_double)(void * A, long i));
double ARRAY_METRICS_mape(void * A, void * F, long i_start, long i_end, double (* get_val_as_double)(void * A, long i));

#define array_seg_mape_serial(A, B, i_start, i_end, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_mape_serial(A, B, i_start, i_end, DEFAULT_ARG_1(gen_functor_basic_type_to_double(A), ##__VA_ARGS__))

#define array_mape_serial(A, N, ... /* get_val_as_double() */)    \
	array_seg_mape_serial(A, B, 0, N, ##__VA_ARGS__)

#define array_seg_mape_parallel(A, B, i_start, i_end, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_mape_parallel(A, B, i_start, i_end, DEFAULT_ARG_1(gen_functor_basic_type_to_double(A), ##__VA_ARGS__))

#define array_mape_parallel(A, B, N, ... /* get_val_as_double() */)    \
	array_seg_mape_parallel(A, B, 0, N, ##__VA_ARGS__)

#define array_seg_mape(A, B, i_start, i_end, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_mape(A, B, i_start, i_end, DEFAULT_ARG_1(gen_functor_basic_type_to_double(A), ##__VA_ARGS__))

#define array_mape(A, B, N, ... /* get_val_as_double() */)    \
	array_seg_mape(A, B, 0, N, ##__VA_ARGS__)


//==========================================================================================================================================
//= Symmetric Mean Absolute Relative Error
//==========================================================================================================================================


static const double ARRAY_METRICS_smare_zero = 0.0;
double ARRAY_METRICS_smare_map(long i, void * A, void * F, long i_start, long i_end, double (* get_val_as_double)(void * A, long i));
double ARRAY_METRICS_smare_reduce(double sum, double val);
double ARRAY_METRICS_smare_output(double agg, void * A, void * F, long i_start, long i_end, double (* get_val_as_double)(void * A, long i));
double ARRAY_METRICS_smare_serial(void * A, void * F, long i_start, long i_end, double (* get_val_as_double)(void * A, long i));
double ARRAY_METRICS_smare_parallel(void * A, void * F, long i_start, long i_end, double (* get_val_as_double)(void * A, long i));
double ARRAY_METRICS_smare(void * A, void * F, long i_start, long i_end, double (* get_val_as_double)(void * A, long i));

#define array_seg_smare_serial(A, B, i_start, i_end, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_smare_serial(A, B, i_start, i_end, DEFAULT_ARG_1(gen_functor_basic_type_to_double(A), ##__VA_ARGS__))

#define array_smare_serial(A, N, ... /* get_val_as_double() */)    \
	array_seg_smare_serial(A, B, 0, N, ##__VA_ARGS__)

#define array_seg_smare_parallel(A, B, i_start, i_end, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_smare_parallel(A, B, i_start, i_end, DEFAULT_ARG_1(gen_functor_basic_type_to_double(A), ##__VA_ARGS__))

#define array_smare_parallel(A, B, N, ... /* get_val_as_double() */)    \
	array_seg_smare_parallel(A, B, 0, N, ##__VA_ARGS__)

#define array_seg_smare(A, B, i_start, i_end, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_smare(A, B, i_start, i_end, DEFAULT_ARG_1(gen_functor_basic_type_to_double(A), ##__VA_ARGS__))

#define array_smare(A, B, N, ... /* get_val_as_double() */)    \
	array_seg_smare(A, B, 0, N, ##__VA_ARGS__)


//==========================================================================================================================================
//= Symmetric Mean Absolute Percentage Error
//==========================================================================================================================================


static const double ARRAY_METRICS_smape_zero = 0.0;
double ARRAY_METRICS_smape_map(long i, void * A, void * F, long i_start, long i_end, double (* get_val_as_double)(void * A, long i));
double ARRAY_METRICS_smape_reduce(double sum, double val);
double ARRAY_METRICS_smape_output(double agg, void * A, void * F, long i_start, long i_end, double (* get_val_as_double)(void * A, long i));
double ARRAY_METRICS_smape_serial(void * A, void * F, long i_start, long i_end, double (* get_val_as_double)(void * A, long i));
double ARRAY_METRICS_smape_parallel(void * A, void * F, long i_start, long i_end, double (* get_val_as_double)(void * A, long i));
double ARRAY_METRICS_smape(void * A, void * F, long i_start, long i_end, double (* get_val_as_double)(void * A, long i));

#define array_seg_smape_serial(A, B, i_start, i_end, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_smape_serial(A, B, i_start, i_end, DEFAULT_ARG_1(gen_functor_basic_type_to_double(A), ##__VA_ARGS__))

#define array_smape_serial(A, N, ... /* get_val_as_double() */)    \
	array_seg_smape_serial(A, B, 0, N, ##__VA_ARGS__)

#define array_seg_smape_parallel(A, B, i_start, i_end, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_smape_parallel(A, B, i_start, i_end, DEFAULT_ARG_1(gen_functor_basic_type_to_double(A), ##__VA_ARGS__))

#define array_smape_parallel(A, B, N, ... /* get_val_as_double() */)    \
	array_seg_smape_parallel(A, B, 0, N, ##__VA_ARGS__)

#define array_seg_smape(A, B, i_start, i_end, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_smape(A, B, i_start, i_end, DEFAULT_ARG_1(gen_functor_basic_type_to_double(A), ##__VA_ARGS__))

#define array_smape(A, B, N, ... /* get_val_as_double() */)    \
	array_seg_smape(A, B, 0, N, ##__VA_ARGS__)


//==========================================================================================================================================
//= Q Error - Relative Error
//==========================================================================================================================================


static const double ARRAY_METRICS_Q_error_zero = 0.0;
double ARRAY_METRICS_Q_error_map(long i, void * A, void * F, long i_start, long i_end, double (* get_val_as_double)(void * A, long i));
double ARRAY_METRICS_Q_error_reduce(double sum, double val);
double ARRAY_METRICS_Q_error_output(double agg, void * A, void * F, long i_start, long i_end, double (* get_val_as_double)(void * A, long i));
double ARRAY_METRICS_Q_error_serial(void * A, void * F, long i_start, long i_end, double (* get_val_as_double)(void * A, long i));
double ARRAY_METRICS_Q_error_parallel(void * A, void * F, long i_start, long i_end, double (* get_val_as_double)(void * A, long i));
double ARRAY_METRICS_Q_error(void * A, void * F, long i_start, long i_end, double (* get_val_as_double)(void * A, long i));

#define array_seg_Q_error_serial(A, B, i_start, i_end, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_Q_error_serial(A, B, i_start, i_end, DEFAULT_ARG_1(gen_functor_basic_type_to_double(A), ##__VA_ARGS__))

#define array_Q_error_serial(A, N, ... /* get_val_as_double() */)    \
	array_seg_Q_error_serial(A, B, 0, N, ##__VA_ARGS__)

#define array_seg_Q_error_parallel(A, B, i_start, i_end, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_Q_error_parallel(A, B, i_start, i_end, DEFAULT_ARG_1(gen_functor_basic_type_to_double(A), ##__VA_ARGS__))

#define array_Q_error_parallel(A, B, N, ... /* get_val_as_double() */)    \
	array_seg_Q_error_parallel(A, B, 0, N, ##__VA_ARGS__)

#define array_seg_Q_error(A, B, i_start, i_end, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_Q_error(A, B, i_start, i_end, DEFAULT_ARG_1(gen_functor_basic_type_to_double(A), ##__VA_ARGS__))

#define array_Q_error(A, B, N, ... /* get_val_as_double() */)    \
	array_seg_Q_error(A, B, 0, N, ##__VA_ARGS__)


//==========================================================================================================================================
//= Ln Q Error - Log Relative Error
//==========================================================================================================================================


static const double ARRAY_METRICS_lnQ_error_zero = 0.0;
double ARRAY_METRICS_lnQ_error_map(long i, void * A, void * F, long i_start, long i_end, double (* get_val_as_double)(void * A, long i));
double ARRAY_METRICS_lnQ_error_reduce(double sum, double val);
double ARRAY_METRICS_lnQ_error_output(double agg, void * A, void * F, long i_start, long i_end, double (* get_val_as_double)(void * A, long i));
double ARRAY_METRICS_lnQ_error_serial(void * A, void * F, long i_start, long i_end, double (* get_val_as_double)(void * A, long i));
double ARRAY_METRICS_lnQ_error_parallel(void * A, void * F, long i_start, long i_end, double (* get_val_as_double)(void * A, long i));
double ARRAY_METRICS_lnQ_error(void * A, void * F, long i_start, long i_end, double (* get_val_as_double)(void * A, long i));

#define array_seg_lnQ_error_serial(A, B, i_start, i_end, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_lnQ_error_serial(A, B, i_start, i_end, DEFAULT_ARG_1(gen_functor_basic_type_to_double(A), ##__VA_ARGS__))

#define array_lnQ_error_serial(A, N, ... /* get_val_as_double() */)    \
	array_seg_lnQ_error_serial(A, B, 0, N, ##__VA_ARGS__)

#define array_seg_lnQ_error_parallel(A, B, i_start, i_end, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_lnQ_error_parallel(A, B, i_start, i_end, DEFAULT_ARG_1(gen_functor_basic_type_to_double(A), ##__VA_ARGS__))

#define array_lnQ_error_parallel(A, B, N, ... /* get_val_as_double() */)    \
	array_seg_lnQ_error_parallel(A, B, 0, N, ##__VA_ARGS__)

#define array_seg_lnQ_error(A, B, i_start, i_end, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_lnQ_error(A, B, i_start, i_end, DEFAULT_ARG_1(gen_functor_basic_type_to_double(A), ##__VA_ARGS__))

#define array_lnQ_error(A, B, N, ... /* get_val_as_double() */)    \
	array_seg_lnQ_error(A, B, 0, N, ##__VA_ARGS__)


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//------------------------------------------------------------------------------------------------------------------------------------------
//-                                                       Closest Pair Distance                                                            -
//------------------------------------------------------------------------------------------------------------------------------------------
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


double ARRAY_METRICS_closest_pair_idx_serial(void * A, long i_start, long i_end, long * idx1_out, long * idx2_out, double (* get_val_as_double)(void * A, long i));
double ARRAY_METRICS_closest_pair_idx(void * A, long i_start, long i_end, long * idx1_out, long * idx2_out, double (* get_val_as_double)(void * A, long i));

#define array_seg_closest_pair_idx_serial(A, i_start, i_end, idx1_out, idx2_out, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_closest_pair_idx_serial(A, i_start, i_end, idx1_out, idx2_out, DEFAULT_ARG_1(gen_functor_basic_type_to_double(A), ##__VA_ARGS__))

#define array_closest_pair_idx_serial(A, N, idx1_out, idx2_out, ... /* get_val_as_double() */)    \
	array_seg_closest_pair_idx_serial(A, 0, N, idx1_out, idx2_out, ##__VA_ARGS__)

#define array_seg_closest_pair_idx(A, i_start, i_end, idx1_out, idx2_out, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_closest_pair_idx(A, i_start, i_end, idx1_out, idx2_out, DEFAULT_ARG_1(gen_functor_basic_type_to_double(A), ##__VA_ARGS__))

#define array_closest_pair_idx(A, N, idx1_out, idx2_out, ... /* get_val_as_double() */)    \
	array_seg_closest_pair_idx(A, 0, N, idx1_out, idx2_out, ##__VA_ARGS__)


#endif /* ARRAY_METRICS_H */

