#ifndef ARRAY_METRICS_H
#define ARRAY_METRICS_H

#include <math.h>

#include "macros/cpp_defines.h"
#include "macros/macrolib.h"

/* For CPP the user has to always give the value to double converter function,
 * because the genlib.h header file is C only.
 */
#ifndef __cplusplus
	#include "genlib.h"
#endif


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//------------------------------------------------------------------------------------------------------------------------------------------
//-                                                          Metrics-Errors Macros                                                         -
//------------------------------------------------------------------------------------------------------------------------------------------
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


/* 'var_ptr' argument can be ignored.
 */

#define __array_metrics_struct_members(pos, get_val_as_double, static_args, kernel_name, var_ptr, ...)    \
	typeof(ARRAY_METRICS_ ## kernel_name ## _map (0, UNPACK(static_args), ##__VA_ARGS__, get_val_as_double)) kernel_name ## _ ## pos

#define __array_metrics_struct_init(pos, static_args, kernel_name, var_ptr, ...)    \
	__array_metrics_vals.kernel_name ## _ ## pos = ARRAY_METRICS_ ## kernel_name ## _zero;

#define __array_metrics_map_reduce(pos, i, get_val_as_double, static_args, kernel_name, var_ptr, ...)                                                                               \
do {                                                                                                                                                                                \
	__array_metrics_vals.kernel_name ## _ ## pos =                                                                                                                              \
		ARRAY_METRICS_ ## kernel_name ## _reduce (                                                                                                                          \
				__array_metrics_vals.kernel_name ## _ ## pos, ARRAY_METRICS_ ## kernel_name ## _map (i, UNPACK(static_args), ##__VA_ARGS__, get_val_as_double));    \
} while (0)

#define __array_metrics_thread_reduce(pos, kernel_name, var_ptr, ...)                                                                                               \
do {                                                                                                                                                                \
	__array_metrics_vals.kernel_name ## _ ## pos =                                                                                                              \
		array_metrics_thread_reduce(                                                                                                                        \
				ARRAY_METRICS_ ## kernel_name ## _reduce, __array_metrics_vals.kernel_name ## _ ## pos, ARRAY_METRICS_ ## kernel_name ## _zero);    \
} while (0)

/* #define __array_metrics_output(pos, get_val_as_double, static_args, kernel_name, var_ptr, ...)                                           \
do {                                                                                                                                     \
	*(var_ptr) =                                                                                                                     \
		ARRAY_METRICS_ ## kernel_name ## _output (                                                                               \
				__array_metrics_vals.kernel_name ## _ ## pos, UNPACK(static_args), ##__VA_ARGS__, get_val_as_double);    \
} while (0) */

#define __array_metrics_output(pos, get_val_as_double, static_args, kernel_name, var_ptr, ...)                                                                                                                         \
do {                                                                                                                                                                                                                   \
	__attribute__((unused)) typeof(ARRAY_METRICS_ ## kernel_name ## _output(__array_metrics_vals.kernel_name ## _ ## pos, UNPACK(static_args), ##__VA_ARGS__, get_val_as_double)) __array_metrics_output_dummy;    \
	*(DEFAULT_ARG_1(&__array_metrics_output_dummy, var_ptr)) =                                                                                                                                                     \
		ARRAY_METRICS_ ## kernel_name ## _output (                                                                                                                                                             \
				__array_metrics_vals.kernel_name ## _ ## pos, UNPACK(static_args), ##__VA_ARGS__, get_val_as_double);                                                                                  \
} while (0)

#define __array_metrics(parallel, output_single, get_val_as_double, i_start, i_end, static_args, tuples_kernel_var_args)                                              \
do {                                                                                                                                                                  \
	struct {                                                                                                                                                      \
		FOREACH_AS_STMT_UNGUARDED(1, __array_metrics_struct_members, (get_val_as_double, static_args), tuples_kernel_var_args, )                              \
	} __array_metrics_vals;                                                                                                                                       \
	long __array_metrics_i, __array_metrics_i_s, __array_metrics_i_e;                                                                                             \
	FOREACH_AS_STMT_UNGUARDED(1, __array_metrics_struct_init, (static_args), tuples_kernel_var_args, )                                                            \
	if (parallel)                                                                                                                                                 \
		loop_partitioner_balance_iterations(safe_omp_get_num_threads(), omp_get_thread_num(), i_start, i_end, &__array_metrics_i_s, &__array_metrics_i_e);    \
	else                                                                                                                                                          \
	{                                                                                                                                                             \
		__array_metrics_i_s = i_start;                                                                                                                        \
		__array_metrics_i_e = i_end;                                                                                                                          \
	}                                                                                                                                                             \
	for (__array_metrics_i=__array_metrics_i_s;__array_metrics_i<__array_metrics_i_e;__array_metrics_i++)                                                         \
	{                                                                                                                                                             \
		FOREACH_AS_STMT_UNGUARDED(1, __array_metrics_map_reduce, (__array_metrics_i, get_val_as_double, static_args), tuples_kernel_var_args, )               \
	}                                                                                                                                                             \
	if (parallel)                                                                                                                                                 \
	{                                                                                                                                                             \
		FOREACH_AS_STMT_UNGUARDED(1, __array_metrics_thread_reduce, , tuples_kernel_var_args, )                                                               \
	}                                                                                                                                                             \
	if (parallel && output_single)                                                                                                                                \
	{                                                                                                                                                             \
		_Pragma("omp single nowait")                                                                                                                          \
		{                                                                                                                                                     \
			FOREACH_AS_STMT_UNGUARDED(1, __array_metrics_output, (get_val_as_double, static_args), tuples_kernel_var_args, )                              \
		}                                                                                                                                                     \
	}                                                                                                                                                             \
	else                                                                                                                                                          \
	{                                                                                                                                                             \
		FOREACH_AS_STMT_UNGUARDED(1, __array_metrics_output, (get_val_as_double, static_args), tuples_kernel_var_args, )                                      \
	}                                                                                                                                                             \
} while (0)


//------------------------------------------------------------------------------------------------------------------------------------------
//- Array Metrics
//------------------------------------------------------------------------------------------------------------------------------------------

#define array_seg_metrics_serial(A, i_start, i_end, tuples_kernel_var_args, ...)    \
	__array_metrics(0, 0, DEFAULT_ARG_1(gen_functor_convert_basic_type_to_double(A), ##__VA_ARGS__), i_start, i_end, (A, i_start, i_end), tuples_kernel_var_args)

#define array_metrics_serial(A, N, tuples_kernel_var_args, ...)    \
	__array_metrics(0, 0, DEFAULT_ARG_1(gen_functor_convert_basic_type_to_double(A), ##__VA_ARGS__), 0, N, (A, 0, N), tuples_kernel_var_args)

#define array_seg_metrics_concurrent(A, i_start, i_end, tuples_kernel_var_args, ...)    \
	__array_metrics(1, 0, DEFAULT_ARG_1(gen_functor_convert_basic_type_to_double(A), ##__VA_ARGS__), i_start, i_end, (A, i_start, i_end), tuples_kernel_var_args)

#define array_metrics_concurrent(A, N, tuples_kernel_var_args, ...)    \
	__array_metrics(1, 0, DEFAULT_ARG_1(gen_functor_convert_basic_type_to_double(A), ##__VA_ARGS__), 0, N, (A, 0, N), tuples_kernel_var_args)

#define array_seg_metrics(A, i_start, i_end, tuples_kernel_var_args, ...)                                                                                                                 \
do {                                                                                                                                                                                      \
	if (omp_get_level() > 0)                                                                                                                                                          \
		__array_metrics(0, 0, DEFAULT_ARG_1(gen_functor_convert_basic_type_to_double(A), ##__VA_ARGS__), i_start, i_end, (A, i_start, i_end), tuples_kernel_var_args);            \
	else                                                                                                                                                                              \
	{                                                                                                                                                                                 \
		_Pragma("omp parallel")                                                                                                                                                   \
		{                                                                                                                                                                         \
			__array_metrics(1, 1, DEFAULT_ARG_1(gen_functor_convert_basic_type_to_double(A), ##__VA_ARGS__), i_start, i_end, (A, i_start, i_end), tuples_kernel_var_args);    \
		}                                                                                                                                                                         \
	}                                                                                                                                                                                 \
} while (0)

#define array_metrics(A, N, tuples_kernel_var_args, ...)                    \
do {                                                                        \
	array_seg_metrics(A, 0, N, tuples_kernel_var_args, __VA_ARGS__);    \
} while (0)


//------------------------------------------------------------------------------------------------------------------------------------------
//- Array Errors
//------------------------------------------------------------------------------------------------------------------------------------------

#define array_seg_errors_serial(A, F, i_start, i_end, tuples_kernel_var_args, ...)    \
	__array_metrics(0, 0, DEFAULT_ARG_1(gen_functor_convert_basic_type_to_double(A), ##__VA_ARGS__), i_start, i_end, (A, F, i_start, i_end), tuples_kernel_var_args)

#define array_errors_serial(A, F, N, tuples_kernel_var_args, ...)    \
	__array_metrics(0, 0, DEFAULT_ARG_1(gen_functor_convert_basic_type_to_double(A), ##__VA_ARGS__), 0, N, (A, F, 0, N), tuples_kernel_var_args)

#define array_seg_errors_concurrent(A, F, i_start, i_end, tuples_kernel_var_args, ...)    \
	__array_metrics(1, 0, DEFAULT_ARG_1(gen_functor_convert_basic_type_to_double(A), ##__VA_ARGS__), i_start, i_end, (A, F, i_start, i_end), tuples_kernel_var_args)

#define array_errors_concurrent(A, F, N, tuples_kernel_var_args, ...)    \
	__array_metrics(1, 0, DEFAULT_ARG_1(gen_functor_convert_basic_type_to_double(A), ##__VA_ARGS__), 0, N, (A, F, 0, N), tuples_kernel_var_args)

#define array_seg_errors(A, F, i_start, i_end, tuples_kernel_var_args, ...)                                                                                                                  \
do {                                                                                                                                                                                         \
	if (omp_get_level() > 0)                                                                                                                                                             \
		__array_metrics(0, 0, DEFAULT_ARG_1(gen_functor_convert_basic_type_to_double(A), ##__VA_ARGS__), i_start, i_end, (A, F, i_start, i_end), tuples_kernel_var_args);            \
	else                                                                                                                                                                                 \
	{                                                                                                                                                                                    \
		_Pragma("omp parallel")                                                                                                                                                      \
		{                                                                                                                                                                            \
			__array_metrics(1, 1, DEFAULT_ARG_1(gen_functor_convert_basic_type_to_double(A), ##__VA_ARGS__), i_start, i_end, (A, F, i_start, i_end), tuples_kernel_var_args);    \
		}                                                                                                                                                                            \
	}                                                                                                                                                                                    \
} while (0)

#define array_errors(A, F, N, tuples_kernel_var_args, ...)                    \
do {                                                                          \
	array_seg_errors(A, F, 0, N, tuples_kernel_var_args, __VA_ARGS__);    \
} while (0)


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//------------------------------------------------------------------------------------------------------------------------------------------
//-                                                              Subsets                                                                   -
//------------------------------------------------------------------------------------------------------------------------------------------
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


//==========================================================================================================================================
//= Greater Than
//==========================================================================================================================================


#if 0
struct ARRAY_METRICS_greater_than_s {
	void * A;
	long i_start;
	long i_end;
	double threshold;
	double partial;
	double * result_out;
};

void ARRAY_METRICS_greater_than_init(int concurrent, struct ARRAY_METRICS_greater_than_s * s, void * A, long i_start, long i_end, double threshold, double * result_out, double (* get_val_as_double)(void * A, long i));
void ARRAY_METRICS_greater_than_process(struct ARRAY_METRICS_greater_than_s * s, long i, double (* get_val_as_double)(void * A, long i));
struct ARRAY_METRICS_greater_than_s ARRAY_METRICS_greater_than_combine(struct ARRAY_METRICS_greater_than_s s1, struct ARRAY_METRICS_greater_than_s s2);
void ARRAY_METRICS_greater_than_finalize(struct ARRAY_METRICS_greater_than_s * s);
void ARRAY_METRICS_greater_than_output(struct ARRAY_METRICS_greater_than_s * s_local, struct ARRAY_METRICS_greater_than_s * s)_global;

void ARRAY_METRICS_greater_than_serial(void * A, long i_start, long i_end, double threshold, double * result_out, double (* get_val_as_double)(void * A, long i));
void ARRAY_METRICS_greater_than_concurrent(void * A, long i_start, long i_end, double threshold, double * result_out, double (* get_val_as_double)(void * A, long i));
void ARRAY_METRICS_greater_than(void * A, long i_start, long i_end, double threshold, double * result_out, double (* get_val_as_double)(void * A, long i));

#define array_seg_greater_than_serial(A, i_start, i_end, threshold, result_out, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_greater_than_serial(A, i_start, i_end, threshold, result_out, DEFAULT_ARG_1(gen_functor_convert_basic_type_to_double(A), ##__VA_ARGS__))

#define array_greater_than_serial(A, N, threshold, result_out, ... /* get_val_as_double() */)    \
	array_seg_greater_than_serial(A, 0, N, threshold, result_out, ##__VA_ARGS__)

#define array_seg_greater_than_concurrent(A, i_start, i_end, threshold, result_out, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_greater_than_concurrent(A, i_start, i_end, threshold, result_out, DEFAULT_ARG_1(gen_functor_convert_basic_type_to_double(A), ##__VA_ARGS__))

#define array_greater_than_concurrent(A, N, threshold, result_out, ... /* get_val_as_double() */)    \
	array_seg_greater_than_concurrent(A, 0, N, threshold, result_out, ##__VA_ARGS__)

#define array_seg_greater_than(A, i_start, i_end, threshold, result_out, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_greater_than(A, i_start, i_end, threshold, result_out, DEFAULT_ARG_1(gen_functor_convert_basic_type_to_double(A), ##__VA_ARGS__))

#define array_greater_than(A, N, threshold, result_out, ... /* get_val_as_double() */)    \
	array_seg_greater_than(A, 0, N, threshold, result_out, ##__VA_ARGS__)
#endif


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//------------------------------------------------------------------------------------------------------------------------------------------
//-                                                               Norms                                                                    -
//------------------------------------------------------------------------------------------------------------------------------------------
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


//==========================================================================================================================================
//= P-Norm
//==========================================================================================================================================


struct ARRAY_METRICS_pnorm_s {
	void * A;
	long i_start;
	long i_end;
	double p;
	double partial;
	double * result_out;
};

void ARRAY_METRICS_pnorm_init(int concurrent, struct ARRAY_METRICS_pnorm_s * s, void * A, long i_start, long i_end, double p, double * result_out, double (* get_val_as_double)(void * A, long i));
void ARRAY_METRICS_pnorm_process(struct ARRAY_METRICS_pnorm_s * s, long i, double (* get_val_as_double)(void * A, long i));
struct ARRAY_METRICS_pnorm_s ARRAY_METRICS_pnorm_combine(struct ARRAY_METRICS_pnorm_s s1, struct ARRAY_METRICS_pnorm_s s2);
void ARRAY_METRICS_pnorm_finalize(struct ARRAY_METRICS_pnorm_s * s);
void ARRAY_METRICS_pnorm_output(struct ARRAY_METRICS_pnorm_s * s_local, struct ARRAY_METRICS_pnorm_s * s_global);

void ARRAY_METRICS_pnorm_serial(void * A, long i_start, long i_end, double p, double * result_out, double (* get_val_as_double)(void * A, long i));
void ARRAY_METRICS_pnorm_concurrent(void * A, long i_start, long i_end, double p, double * result_out, double (* get_val_as_double)(void * A, long i));
void ARRAY_METRICS_pnorm(void * A, long i_start, long i_end, double p, double * result_out, double (* get_val_as_double)(void * A, long i));

#define array_seg_pnorm_serial(A, i_start, i_end, p, result_out, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_pnorm_serial(A, i_start, i_end, p, result_out, DEFAULT_ARG_1(gen_functor_convert_basic_type_to_double(A), ##__VA_ARGS__))

#define array_pnorm_serial(A, N, p, result_out, ... /* get_val_as_double() */)    \
	array_seg_pnorm_serial(A, 0, N, p, result_out, ##__VA_ARGS__)

#define array_seg_pnorm_concurrent(A, i_start, i_end, p, result_out, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_pnorm_concurrent(A, i_start, i_end, p, result_out, DEFAULT_ARG_1(gen_functor_convert_basic_type_to_double(A), ##__VA_ARGS__))

#define array_pnorm_concurrent(A, N, p, result_out, ... /* get_val_as_double() */)    \
	array_seg_pnorm_concurrent(A, 0, N, p, result_out, ##__VA_ARGS__)

#define array_seg_pnorm(A, i_start, i_end, p, result_out, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_pnorm(A, i_start, i_end, p, result_out, DEFAULT_ARG_1(gen_functor_convert_basic_type_to_double(A), ##__VA_ARGS__))

#define array_pnorm(A, N, p, result_out, ... /* get_val_as_double() */)    \
	array_seg_pnorm(A, 0, N, p, result_out, ##__VA_ARGS__)


//==========================================================================================================================================
//= Min
//==========================================================================================================================================


struct ARRAY_METRICS_min_s {
	void * A;
	long i_start;
	long i_end;
	double min;
	long min_idx;
	double * min_out;
	long * min_idx_out;
};

void ARRAY_METRICS_min_init(int concurrent, struct ARRAY_METRICS_min_s * s, void * A, long i_start, long i_end, double * min_out, long * min_idx_out, double (* get_val_as_double)(void * A, long i));
void ARRAY_METRICS_min_process(struct ARRAY_METRICS_min_s * s, long i, double (* get_val_as_double)(void * A, long i));
struct ARRAY_METRICS_min_s ARRAY_METRICS_min_combine(struct ARRAY_METRICS_min_s s1, struct ARRAY_METRICS_min_s s2);
void ARRAY_METRICS_min_finalize(struct ARRAY_METRICS_min_s * s);
void ARRAY_METRICS_min_output(struct ARRAY_METRICS_min_s * s_local, struct ARRAY_METRICS_min_s * s_global);

void ARRAY_METRICS_min_serial(void * A, long i_start, long i_end, double * min_out, long * min_idx_out, double (* get_val_as_double)(void * A, long i));
void ARRAY_METRICS_min_concurrent(void * A, long i_start, long i_end, double * min_out, long * min_idx_out, double (* get_val_as_double)(void * A, long i));
void ARRAY_METRICS_min(void * A, long i_start, long i_end, double * min_out, long * min_idx_out, double (* get_val_as_double)(void * A, long i));

#define array_seg_min_serial(A, i_start, i_end, min_out, min_idx_out, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_min_serial(A, i_start, i_end, min_out, min_idx_out, DEFAULT_ARG_1(gen_functor_convert_basic_type_to_double(A), ##__VA_ARGS__))

#define array_min_serial(A, N, min_out, min_idx_out, ... /* get_val_as_double() */)    \
	array_seg_min_serial(A, 0, N, min_out, min_idx_out, ##__VA_ARGS__)

#define array_seg_min_concurrent(A, i_start, i_end, min_out, min_idx_out, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_min_concurrent(A, i_start, i_end, min_out, min_idx_out, DEFAULT_ARG_1(gen_functor_convert_basic_type_to_double(A), ##__VA_ARGS__))

#define array_min_concurrent(A, N, min_out, min_idx_out, ... /* get_val_as_double() */)    \
	array_seg_min_concurrent(A, 0, N, min_out, min_idx_out, ##__VA_ARGS__)

#define array_seg_min(A, i_start, i_end, min_out, min_idx_out, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_min(A, i_start, i_end, min_out, min_idx_out, DEFAULT_ARG_1(gen_functor_convert_basic_type_to_double(A), ##__VA_ARGS__))

#define array_min(A, N, min_out, min_idx_out, ... /* get_val_as_double() */)    \
	array_seg_min(A, 0, N, min_out, min_idx_out, ##__VA_ARGS__)


//==========================================================================================================================================
//= Max
//==========================================================================================================================================


struct ARRAY_METRICS_max_s {
	void * A;
	long i_start;
	long i_end;
	double max;
	long max_idx;
	double * max_out;
	long * max_idx_out;
};

void ARRAY_METRICS_max_init(int concurrent, struct ARRAY_METRICS_max_s * s, void * A, long i_start, long i_end, double * max_out, long * max_idx_out, double (* get_val_as_double)(void * A, long i));
void ARRAY_METRICS_max_process(struct ARRAY_METRICS_max_s * s, long i, double (* get_val_as_double)(void * A, long i));
struct ARRAY_METRICS_max_s ARRAY_METRICS_max_combine(struct ARRAY_METRICS_max_s s1, struct ARRAY_METRICS_max_s s2);
void ARRAY_METRICS_max_finalize(struct ARRAY_METRICS_max_s * s);
void ARRAY_METRICS_max_output(struct ARRAY_METRICS_max_s * s_local, struct ARRAY_METRICS_max_s * s_global);

void ARRAY_METRICS_max_serial(void * A, long i_start, long i_end, double * max_out, long * max_idx_out, double (* get_val_as_double)(void * A, long i));
void ARRAY_METRICS_max_concurrent(void * A, long i_start, long i_end, double * max_out, long * max_idx_out, double (* get_val_as_double)(void * A, long i));
void ARRAY_METRICS_max(void * A, long i_start, long i_end, double * max_out, long * max_idx_out, double (* get_val_as_double)(void * A, long i));

#define array_seg_max_serial(A, i_start, i_end, max_out, max_idx_out, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_max_serial(A, i_start, i_end, max_out, max_idx_out, DEFAULT_ARG_1(gen_functor_convert_basic_type_to_double(A), ##__VA_ARGS__))

#define array_max_serial(A, N, max_out, max_idx_out, ... /* get_val_as_double() */)    \
	array_seg_max_serial(A, 0, N, max_out, max_idx_out, ##__VA_ARGS__)

#define array_seg_max_concurrent(A, i_start, i_end, max_out, max_idx_out, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_max_concurrent(A, i_start, i_end, max_out, max_idx_out, DEFAULT_ARG_1(gen_functor_convert_basic_type_to_double(A), ##__VA_ARGS__))

#define array_max_concurrent(A, N, max_out, max_idx_out, ... /* get_val_as_double() */)    \
	array_seg_max_concurrent(A, 0, N, max_out, max_idx_out, ##__VA_ARGS__)

#define array_seg_max(A, i_start, i_end, max_out, max_idx_out, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_max(A, i_start, i_end, max_out, max_idx_out, DEFAULT_ARG_1(gen_functor_convert_basic_type_to_double(A), ##__VA_ARGS__))

#define array_max(A, N, max_out, max_idx_out, ... /* get_val_as_double() */)    \
	array_seg_max(A, 0, N, max_out, max_idx_out, ##__VA_ARGS__)


//==========================================================================================================================================
//= Min - Max
//==========================================================================================================================================


struct ARRAY_METRICS_min_max_s {
	void * A;
	long i_start;
	long i_end;
	double min;
	long min_idx;
	double max;
	long max_idx;
	double * min_out;
	long * min_idx_out;
	double * max_out;
	long * max_idx_out;
};

void ARRAY_METRICS_min_max_init(int concurrent, struct ARRAY_METRICS_min_max_s * s, void * A, long i_start, long i_end, double * min_out, long * min_idx_out, double * max_out, long * max_idx_out, double (* get_val_as_double)(void * A, long i));
void ARRAY_METRICS_min_max_process(struct ARRAY_METRICS_min_max_s * s, long i, double (* get_val_as_double)(void * A, long i));
struct ARRAY_METRICS_min_max_s ARRAY_METRICS_min_max_combine(struct ARRAY_METRICS_min_max_s s1, struct ARRAY_METRICS_min_max_s s2);
void ARRAY_METRICS_min_max_finalize(struct ARRAY_METRICS_min_max_s * s);
void ARRAY_METRICS_min_max_output(struct ARRAY_METRICS_min_max_s * s_local, struct ARRAY_METRICS_min_max_s * s_global);

void ARRAY_METRICS_min_max_serial(void * A, long i_start, long i_end, double * min_out, long * min_idx_out, double * max_out, long * max_idx_out, double (* get_val_as_double)(void * A, long i));
void ARRAY_METRICS_min_max_concurrent(void * A, long i_start, long i_end, double * min_out, long * min_idx_out, double * max_out, long * max_idx_out, double (* get_val_as_double)(void * A, long i));
void ARRAY_METRICS_min_max(void * A, long i_start, long i_end, double * min_out, long * min_idx_out, double * max_out, long * max_idx_out, double (* get_val_as_double)(void * A, long i));

#define array_seg_min_max_serial(A, i_start, i_end, min_out, min_idx_out, max_out, max_idx_out, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_min_max_serial(A, i_start, i_end, min_out, min_idx_out, max_out, max_idx_out, DEFAULT_ARG_1(gen_functor_convert_basic_type_to_double(A), ##__VA_ARGS__))

#define array_min_max_serial(A, N, min_out, min_idx_out, max_out, max_idx_out, ... /* get_val_as_double() */)    \
	array_seg_min_max_serial(A, 0, N, min_out, min_idx_out, max_out, max_idx_out, ##__VA_ARGS__)

#define array_seg_min_max_concurrent(A, i_start, i_end, min_out, min_idx_out, max_out, max_idx_out, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_min_max_concurrent(A, i_start, i_end, min_out, min_idx_out, max_out, max_idx_out, DEFAULT_ARG_1(gen_functor_convert_basic_type_to_double(A), ##__VA_ARGS__))

#define array_min_max_concurrent(A, N, min_out, min_idx_out, max_out, max_idx_out, ... /* get_val_as_double() */)    \
	array_seg_min_max_concurrent(A, 0, N, min_out, min_idx_out, max_out, max_idx_out, ##__VA_ARGS__)

#define array_seg_min_max(A, i_start, i_end, min_out, min_idx_out, max_out, max_idx_out, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_min_max(A, i_start, i_end, min_out, min_idx_out, max_out, max_idx_out, DEFAULT_ARG_1(gen_functor_convert_basic_type_to_double(A), ##__VA_ARGS__))

#define array_min_max(A, N, min_out, min_idx_out, max_out, max_idx_out, ... /* get_val_as_double() */)    \
	array_seg_min_max(A, 0, N, min_out, min_idx_out, max_out, max_idx_out, ##__VA_ARGS__)


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//------------------------------------------------------------------------------------------------------------------------------------------
//-                                                               Moments                                                                  -
//------------------------------------------------------------------------------------------------------------------------------------------
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


//==========================================================================================================================================
//= Moments
//==========================================================================================================================================


struct ARRAY_METRICS_moment_s {
	void * A;
	long i_start;
	long i_end;
	double n;
	double center;
	double partial;
	double * result_out;
};

void ARRAY_METRICS_moment_init(int concurrent, struct ARRAY_METRICS_moment_s * s, void * A, long i_start, long i_end, double n, double center, double * result_out, double (* get_val_as_double)(void * A, long i));
void ARRAY_METRICS_moment_process(struct ARRAY_METRICS_moment_s * s, long i, double (* get_val_as_double)(void * A, long i));
struct ARRAY_METRICS_moment_s ARRAY_METRICS_moment_combine(struct ARRAY_METRICS_moment_s s1, struct ARRAY_METRICS_moment_s s2);
void ARRAY_METRICS_moment_finalize(struct ARRAY_METRICS_moment_s * s);
void ARRAY_METRICS_moment_output(struct ARRAY_METRICS_moment_s * s_local, struct ARRAY_METRICS_moment_s * s_global);

void ARRAY_METRICS_moment_serial(void * A, long i_start, long i_end, double n, double center, double * result_out, double (* get_val_as_double)(void * A, long i));
void ARRAY_METRICS_moment_concurrent(void * A, long i_start, long i_end, double n, double center, double * result_out, double (* get_val_as_double)(void * A, long i));
void ARRAY_METRICS_moment(void * A, long i_start, long i_end, double n, double center, double * result_out, double (* get_val_as_double)(void * A, long i));

#define array_seg_moment_serial(A, i_start, i_end, n, center, result_out, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_moment_serial(A, i_start, i_end, n, center, result_out, DEFAULT_ARG_1(gen_functor_convert_basic_type_to_double(A), ##__VA_ARGS__))

#define array_moment_serial(A, N, n, center, result_out, ... /* get_val_as_double() */)    \
	array_seg_moment_serial(A, 0, N, n, center, result_out, ##__VA_ARGS__)

#define array_seg_moment_concurrent(A, i_start, i_end, n, center, result_out, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_moment_concurrent(A, i_start, i_end, n, center, result_out, DEFAULT_ARG_1(gen_functor_convert_basic_type_to_double(A), ##__VA_ARGS__))

#define array_moment_concurrent(A, N, n, center, result_out, ... /* get_val_as_double() */)    \
	array_seg_moment_concurrent(A, 0, N, n, center, result_out, ##__VA_ARGS__)

#define array_seg_moment(A, i_start, i_end, n, center, result_out, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_moment(A, i_start, i_end, n, center, result_out, DEFAULT_ARG_1(gen_functor_convert_basic_type_to_double(A), ##__VA_ARGS__))

#define array_moment(A, N, n, center, result_out, ... /* get_val_as_double() */)    \
	array_seg_moment(A, 0, N, n, center, result_out, ##__VA_ARGS__)


//==========================================================================================================================================
//= Standardized Moments
//==========================================================================================================================================


struct ARRAY_METRICS_moment_standardized_s {
	void * A;
	long i_start;
	long i_end;
	double n;
	double center;
	double std;
	double partial;
	double * result_out;
};

void ARRAY_METRICS_moment_standardized_init(int concurrent, struct ARRAY_METRICS_moment_standardized_s * s, void * A, long i_start, long i_end, double n, double center, double std, double * result_out, double (* get_val_as_double)(void * A, long i));
void ARRAY_METRICS_moment_standardized_process(struct ARRAY_METRICS_moment_standardized_s * s, long i, double (* get_val_as_double)(void * A, long i));
struct ARRAY_METRICS_moment_standardized_s ARRAY_METRICS_moment_standardized_combine(struct ARRAY_METRICS_moment_standardized_s s1, struct ARRAY_METRICS_moment_standardized_s s2);
void ARRAY_METRICS_moment_standardized_finalize(struct ARRAY_METRICS_moment_standardized_s * s);
void ARRAY_METRICS_moment_standardized_output(struct ARRAY_METRICS_moment_standardized_s * s_local, struct ARRAY_METRICS_moment_standardized_s * s_global);

void ARRAY_METRICS_moment_standardized_serial(void * A, long i_start, long i_end, double n, double center, double std, double * result_out, double (* get_val_as_double)(void * A, long i));
void ARRAY_METRICS_moment_standardized_concurrent(void * A, long i_start, long i_end, double n, double center, double std, double * result_out, double (* get_val_as_double)(void * A, long i));
void ARRAY_METRICS_moment_standardized(void * A, long i_start, long i_end, double n, double center, double std, double * result_out, double (* get_val_as_double)(void * A, long i));

#define array_seg_moment_standardized_serial(A, i_start, i_end, n, center, std, result_out, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_moment_standardized_serial(A, i_start, i_end, n, center, std, result_out, DEFAULT_ARG_1(gen_functor_convert_basic_type_to_double(A), ##__VA_ARGS__))

#define array_moment_standardized_serial(A, N, n, center, std, result_out, ... /* get_val_as_double() */)    \
	array_seg_moment_standardized_serial(A, 0, N, n, center, std, result_out, ##__VA_ARGS__)

#define array_seg_moment_standardized_concurrent(A, i_start, i_end, n, center, std, result_out, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_moment_standardized_concurrent(A, i_start, i_end, n, center, std, result_out, DEFAULT_ARG_1(gen_functor_convert_basic_type_to_double(A), ##__VA_ARGS__))

#define array_moment_standardized_concurrent(A, N, n, center, std, result_out, ... /* get_val_as_double() */)    \
	array_seg_moment_standardized_concurrent(A, 0, N, n, center, std, result_out, ##__VA_ARGS__)

#define array_seg_moment_standardized(A, i_start, i_end, n, center, std, result_out, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_moment_standardized(A, i_start, i_end, n, center, std, result_out, DEFAULT_ARG_1(gen_functor_convert_basic_type_to_double(A), ##__VA_ARGS__))

#define array_moment_standardized(A, N, n, center, std, result_out, ... /* get_val_as_double() */)    \
	array_seg_moment_standardized(A, 0, N, n, center, std, result_out, ##__VA_ARGS__)


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//------------------------------------------------------------------------------------------------------------------------------------------
//-                                                          Central Tendency                                                              -
//------------------------------------------------------------------------------------------------------------------------------------------
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


//==========================================================================================================================================
//= Mean
//==========================================================================================================================================


struct ARRAY_METRICS_mean_s {
	void * A;
	long i_start;
	long i_end;
	double partial;
	double * result_out;
};

void ARRAY_METRICS_mean_init(int concurrent, struct ARRAY_METRICS_mean_s * s, void * A, long i_start, long i_end, double * result_out, double (* get_val_as_double)(void * A, long i));
void ARRAY_METRICS_mean_process(struct ARRAY_METRICS_mean_s * s, long i, double (* get_val_as_double)(void * A, long i));
struct ARRAY_METRICS_mean_s ARRAY_METRICS_mean_combine(struct ARRAY_METRICS_mean_s s1, struct ARRAY_METRICS_mean_s s2);
void ARRAY_METRICS_mean_finalize(struct ARRAY_METRICS_mean_s * s);
void ARRAY_METRICS_mean_output(struct ARRAY_METRICS_mean_s * s_local, struct ARRAY_METRICS_mean_s * s_global);

void ARRAY_METRICS_mean_serial(void * A, long i_start, long i_end, double * result_out, double (* get_val_as_double)(void * A, long i));
void ARRAY_METRICS_mean_concurrent(void * A, long i_start, long i_end, double * result_out, double (* get_val_as_double)(void * A, long i));
void ARRAY_METRICS_mean(void * A, long i_start, long i_end, double * result_out, double (* get_val_as_double)(void * A, long i));

#define array_seg_mean_serial(A, i_start, i_end, result_out, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_mean_serial(A, i_start, i_end, result_out, DEFAULT_ARG_1(gen_functor_convert_basic_type_to_double(A), ##__VA_ARGS__))

#define array_mean_serial(A, N, result_out, ... /* get_val_as_double() */)    \
	array_seg_mean_serial(A, 0, N, result_out, ##__VA_ARGS__)

#define array_seg_mean_concurrent(A, i_start, i_end, result_out, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_mean_concurrent(A, i_start, i_end, result_out, DEFAULT_ARG_1(gen_functor_convert_basic_type_to_double(A), ##__VA_ARGS__))

#define array_mean_concurrent(A, N, result_out, ... /* get_val_as_double() */)    \
	array_seg_mean_concurrent(A, 0, N, result_out, ##__VA_ARGS__)

#define array_seg_mean(A, i_start, i_end, result_out, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_mean(A, i_start, i_end, result_out, DEFAULT_ARG_1(gen_functor_convert_basic_type_to_double(A), ##__VA_ARGS__))

#define array_mean(A, N, result_out, ... /* get_val_as_double() */)    \
	array_seg_mean(A, 0, N, result_out, ##__VA_ARGS__)


//==========================================================================================================================================
//= Mean of Absolute Values
//==========================================================================================================================================

struct ARRAY_METRICS_mean_abs_s {
	void * A;
	long i_start;
	long i_end;
	double partial;
	double * result_out;
};

void ARRAY_METRICS_mean_abs_init(int concurrent, struct ARRAY_METRICS_mean_abs_s * s, void * A, long i_start, long i_end, double * result_out, double (* get_val_as_double)(void * A, long i));
void ARRAY_METRICS_mean_abs_process(struct ARRAY_METRICS_mean_abs_s * s, long i, double (* get_val_as_double)(void * A, long i));
struct ARRAY_METRICS_mean_abs_s ARRAY_METRICS_mean_abs_combine(struct ARRAY_METRICS_mean_abs_s s1, struct ARRAY_METRICS_mean_abs_s s2);
void ARRAY_METRICS_mean_abs_finalize(struct ARRAY_METRICS_mean_abs_s * s);
void ARRAY_METRICS_mean_abs_output(struct ARRAY_METRICS_mean_abs_s * s_local, struct ARRAY_METRICS_mean_abs_s * s_global);

void ARRAY_METRICS_mean_abs_serial(void * A, long i_start, long i_end, double * result_out, double (* get_val_as_double)(void * A, long i));
void ARRAY_METRICS_mean_abs_concurrent(void * A, long i_start, long i_end, double * result_out, double (* get_val_as_double)(void * A, long i));
void ARRAY_METRICS_mean_abs(void * A, long i_start, long i_end, double * result_out, double (* get_val_as_double)(void * A, long i));

#define array_seg_mean_abs_serial(A, i_start, i_end, result_out, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_mean_abs_serial(A, i_start, i_end, result_out, DEFAULT_ARG_1(gen_functor_convert_basic_type_to_double(A), ##__VA_ARGS__))

#define array_mean_abs_serial(A, N, result_out, ... /* get_val_as_double() */)    \
	array_seg_mean_abs_serial(A, 0, N, result_out, ##__VA_ARGS__)

#define array_seg_mean_abs_concurrent(A, i_start, i_end, result_out, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_mean_abs_concurrent(A, i_start, i_end, result_out, DEFAULT_ARG_1(gen_functor_convert_basic_type_to_double(A), ##__VA_ARGS__))

#define array_mean_abs_concurrent(A, N, result_out, ... /* get_val_as_double() */)    \
	array_seg_mean_abs_concurrent(A, 0, N, result_out, ##__VA_ARGS__)

#define array_seg_mean_abs(A, i_start, i_end, result_out, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_mean_abs(A, i_start, i_end, result_out, DEFAULT_ARG_1(gen_functor_convert_basic_type_to_double(A), ##__VA_ARGS__))

#define array_mean_abs(A, N, result_out, ... /* get_val_as_double() */)    \
	array_seg_mean_abs(A, 0, N, result_out, ##__VA_ARGS__)


//==========================================================================================================================================
//= Geometric Mean
//==========================================================================================================================================


struct ARRAY_METRICS_gmean_s {
	void * A;
	long i_start;
	long i_end;
	double partial;
	double * result_out;
};

void ARRAY_METRICS_gmean_init(int concurrent, struct ARRAY_METRICS_gmean_s * s, void * A, long i_start, long i_end, double * result_out, double (* get_val_as_double)(void * A, long i));
void ARRAY_METRICS_gmean_process(struct ARRAY_METRICS_gmean_s * s, long i, double (* get_val_as_double)(void * A, long i));
struct ARRAY_METRICS_gmean_s ARRAY_METRICS_gmean_combine(struct ARRAY_METRICS_gmean_s s1, struct ARRAY_METRICS_gmean_s s2);
void ARRAY_METRICS_gmean_finalize(struct ARRAY_METRICS_gmean_s * s);
void ARRAY_METRICS_gmean_output(struct ARRAY_METRICS_gmean_s * s_local, struct ARRAY_METRICS_gmean_s * s_global);

void ARRAY_METRICS_gmean_serial(void * A, long i_start, long i_end, double * result_out, double (* get_val_as_double)(void * A, long i));
void ARRAY_METRICS_gmean_concurrent(void * A, long i_start, long i_end, double * result_out, double (* get_val_as_double)(void * A, long i));
void ARRAY_METRICS_gmean(void * A, long i_start, long i_end, double * result_out, double (* get_val_as_double)(void * A, long i));

#define array_seg_gmean_serial(A, i_start, i_end, result_out, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_gmean_serial(A, i_start, i_end, result_out, DEFAULT_ARG_1(gen_functor_convert_basic_type_to_double(A), ##__VA_ARGS__))

#define array_gmean_serial(A, N, result_out, ... /* get_val_as_double() */)    \
	array_seg_gmean_serial(A, 0, N, result_out, ##__VA_ARGS__)

#define array_seg_gmean_concurrent(A, i_start, i_end, result_out, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_gmean_concurrent(A, i_start, i_end, result_out, DEFAULT_ARG_1(gen_functor_convert_basic_type_to_double(A), ##__VA_ARGS__))

#define array_gmean_concurrent(A, N, result_out, ... /* get_val_as_double() */)    \
	array_seg_gmean_concurrent(A, 0, N, result_out, ##__VA_ARGS__)

#define array_seg_gmean(A, i_start, i_end, result_out, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_gmean(A, i_start, i_end, result_out, DEFAULT_ARG_1(gen_functor_convert_basic_type_to_double(A), ##__VA_ARGS__))

#define array_gmean(A, N, result_out, ... /* get_val_as_double() */)    \
	array_seg_gmean(A, 0, N, result_out, ##__VA_ARGS__)


//==========================================================================================================================================
//= Harmonic Mean
//==========================================================================================================================================


struct ARRAY_METRICS_hmean_s {
	void * A;
	long i_start;
	long i_end;
	double partial;
	double * result_out;
};

void ARRAY_METRICS_hmean_init(int concurrent, struct ARRAY_METRICS_hmean_s * s, void * A, long i_start, long i_end, double * result_out, double (* get_val_as_double)(void * A, long i));
void ARRAY_METRICS_hmean_process(struct ARRAY_METRICS_hmean_s * s, long i, double (* get_val_as_double)(void * A, long i));
struct ARRAY_METRICS_hmean_s ARRAY_METRICS_hmean_combine(struct ARRAY_METRICS_hmean_s s1, struct ARRAY_METRICS_hmean_s s2);
void ARRAY_METRICS_hmean_finalize(struct ARRAY_METRICS_hmean_s * s);
void ARRAY_METRICS_hmean_output(struct ARRAY_METRICS_hmean_s * s_local, struct ARRAY_METRICS_hmean_s * s_global);

void ARRAY_METRICS_hmean_serial(void * A, long i_start, long i_end, double * result_out, double (* get_val_as_double)(void * A, long i));
void ARRAY_METRICS_hmean_concurrent(void * A, long i_start, long i_end, double * result_out, double (* get_val_as_double)(void * A, long i));
void ARRAY_METRICS_hmean(void * A, long i_start, long i_end, double * result_out, double (* get_val_as_double)(void * A, long i));

#define array_seg_hmean_serial(A, i_start, i_end, result_out, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_hmean_serial(A, i_start, i_end, result_out, DEFAULT_ARG_1(gen_functor_convert_basic_type_to_double(A), ##__VA_ARGS__))

#define array_hmean_serial(A, N, result_out, ... /* get_val_as_double() */)    \
	array_seg_hmean_serial(A, 0, N, result_out, ##__VA_ARGS__)

#define array_seg_hmean_concurrent(A, i_start, i_end, result_out, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_hmean_concurrent(A, i_start, i_end, result_out, DEFAULT_ARG_1(gen_functor_convert_basic_type_to_double(A), ##__VA_ARGS__))

#define array_hmean_concurrent(A, N, result_out, ... /* get_val_as_double() */)    \
	array_seg_hmean_concurrent(A, 0, N, result_out, ##__VA_ARGS__)

#define array_seg_hmean(A, i_start, i_end, result_out, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_hmean(A, i_start, i_end, result_out, DEFAULT_ARG_1(gen_functor_convert_basic_type_to_double(A), ##__VA_ARGS__))

#define array_hmean(A, N, result_out, ... /* get_val_as_double() */)    \
	array_seg_hmean(A, 0, N, result_out, ##__VA_ARGS__)


//==========================================================================================================================================
//= Root Mean Square
//==========================================================================================================================================


struct ARRAY_METRICS_rms_s {
	void * A;
	long i_start;
	long i_end;
	double partial;
	double * result_out;
};

void ARRAY_METRICS_rms_init(int concurrent, struct ARRAY_METRICS_rms_s * s, void * A, long i_start, long i_end, double * result_out, double (* get_val_as_double)(void * A, long i));
void ARRAY_METRICS_rms_process(struct ARRAY_METRICS_rms_s * s, long i, double (* get_val_as_double)(void * A, long i));
struct ARRAY_METRICS_rms_s ARRAY_METRICS_rms_combine(struct ARRAY_METRICS_rms_s s1, struct ARRAY_METRICS_rms_s s2);
void ARRAY_METRICS_rms_finalize(struct ARRAY_METRICS_rms_s * s);
void ARRAY_METRICS_rms_output(struct ARRAY_METRICS_rms_s * s_local, struct ARRAY_METRICS_rms_s * s_global);

void ARRAY_METRICS_rms_serial(void * A, long i_start, long i_end, double * result_out, double (* get_val_as_double)(void * A, long i));
void ARRAY_METRICS_rms_concurrent(void * A, long i_start, long i_end, double * result_out, double (* get_val_as_double)(void * A, long i));
void ARRAY_METRICS_rms(void * A, long i_start, long i_end, double * result_out, double (* get_val_as_double)(void * A, long i));

#define array_seg_rms_serial(A, i_start, i_end, result_out, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_rms_serial(A, i_start, i_end, result_out, DEFAULT_ARG_1(gen_functor_convert_basic_type_to_double(A), ##__VA_ARGS__))

#define array_rms_serial(A, N, result_out, ... /* get_val_as_double() */)    \
	array_seg_rms_serial(A, 0, N, result_out, ##__VA_ARGS__)

#define array_seg_rms_concurrent(A, i_start, i_end, result_out, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_rms_concurrent(A, i_start, i_end, result_out, DEFAULT_ARG_1(gen_functor_convert_basic_type_to_double(A), ##__VA_ARGS__))

#define array_rms_concurrent(A, N, result_out, ... /* get_val_as_double() */)    \
	array_seg_rms_concurrent(A, 0, N, result_out, ##__VA_ARGS__)

#define array_seg_rms(A, i_start, i_end, result_out, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_rms(A, i_start, i_end, result_out, DEFAULT_ARG_1(gen_functor_convert_basic_type_to_double(A), ##__VA_ARGS__))

#define array_rms(A, N, result_out, ... /* get_val_as_double() */)    \
	array_seg_rms(A, 0, N, result_out, ##__VA_ARGS__)


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//------------------------------------------------------------------------------------------------------------------------------------------
//-                                                         Percentile - Median                                                            -
//------------------------------------------------------------------------------------------------------------------------------------------
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


//==========================================================================================================================================
//= Percentile
//==========================================================================================================================================


void ARRAY_METRICS_quantile_serial(void * A, long i_start, long i_end, double q, const char * method, double * val_out, double (* get_val_as_double)(void * A, long i));
void ARRAY_METRICS_quantile_concurrent(void * A, long i_start, long i_end, double q, const char * method, double * val_out, double (* get_val_as_double)(void * A, long i));
void ARRAY_METRICS_quantile(void * A, long i_start, long i_end, double q, const char * method, double * val_out, double (* get_val_as_double)(void * A, long i));

#define array_seg_quantile_serial(A, i_start, i_end, q, method, val_out, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_quantile_serial(A, i_start, i_end, q, method, val_out, DEFAULT_ARG_1(gen_functor_convert_basic_type_to_double(A), ##__VA_ARGS__))

#define array_quantile_serial(A, N, q, method, val_out, ... /* get_val_as_double() */)    \
	array_seg_quantile_serial(A, 0, N, q, method, val_out, ##__VA_ARGS__)

#define array_seg_quantile_concurrent(A, i_start, i_end, q, method, val_out, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_quantile_concurrent(A, i_start, i_end, q, method, val_out, DEFAULT_ARG_1(gen_functor_convert_basic_type_to_double(A), ##__VA_ARGS__))

#define array_quantile_concurrent(A, N, q, method, val_out, ... /* get_val_as_double() */)    \
	array_seg_quantile_concurrent(A, 0, N, q, method, val_out, ##__VA_ARGS__)

#define array_seg_quantile(A, i_start, i_end, q, method, val_out, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_quantile(A, i_start, i_end, q, method, val_out, DEFAULT_ARG_1(gen_functor_convert_basic_type_to_double(A), ##__VA_ARGS__))

#define array_quantile(A, N, q, method, val_out, ... /* get_val_as_double() */)    \
	array_seg_quantile(A, 0, N, q, method, val_out, ##__VA_ARGS__)


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//------------------------------------------------------------------------------------------------------------------------------------------
//-                                                        Dispersion - Moments                                                            -
//------------------------------------------------------------------------------------------------------------------------------------------
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


//==========================================================================================================================================
//= Mean Absolute Deviation
//==========================================================================================================================================


struct ARRAY_METRICS_mad_s {
	void * A;
	long i_start;
	long i_end;
	double mean;
	double partial;
	double * result_out;
};

void ARRAY_METRICS_mad_init(int concurrent, struct ARRAY_METRICS_mad_s * s, void * A, long i_start, long i_end, double mean, double * result_out, double (* get_val_as_double)(void * A, long i));
void ARRAY_METRICS_mad_process(struct ARRAY_METRICS_mad_s * s, long i, double (* get_val_as_double)(void * A, long i));
struct ARRAY_METRICS_mad_s ARRAY_METRICS_mad_combine(struct ARRAY_METRICS_mad_s s1, struct ARRAY_METRICS_mad_s s2);
void ARRAY_METRICS_mad_finalize(struct ARRAY_METRICS_mad_s * s);
void ARRAY_METRICS_mad_output(struct ARRAY_METRICS_mad_s * s_local, struct ARRAY_METRICS_mad_s * s_global);

void ARRAY_METRICS_mad_serial(void * A, long i_start, long i_end, double mean, double * result_out, double (* get_val_as_double)(void * A, long i));
void ARRAY_METRICS_mad_concurrent(void * A, long i_start, long i_end, double mean, double * result_out, double (* get_val_as_double)(void * A, long i));
void ARRAY_METRICS_mad(void * A, long i_start, long i_end, double mean, double * result_out, double (* get_val_as_double)(void * A, long i));

#define array_seg_mad_serial(A, i_start, i_end, mean, result_out, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_mad_serial(A, i_start, i_end, mean, result_out, DEFAULT_ARG_1(gen_functor_convert_basic_type_to_double(A), ##__VA_ARGS__))

#define array_mad_serial(A, N, mean, result_out, ... /* get_val_as_double() */)    \
	array_seg_mad_serial(A, 0, N, mean, result_out, ##__VA_ARGS__)

#define array_seg_mad_concurrent(A, i_start, i_end, mean, result_out, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_mad_concurrent(A, i_start, i_end, mean, result_out, DEFAULT_ARG_1(gen_functor_convert_basic_type_to_double(A), ##__VA_ARGS__))

#define array_mad_concurrent(A, N, mean, result_out, ... /* get_val_as_double() */)    \
	array_seg_mad_concurrent(A, 0, N, mean, result_out, ##__VA_ARGS__)

#define array_seg_mad(A, i_start, i_end, mean, result_out, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_mad(A, i_start, i_end, mean, result_out, DEFAULT_ARG_1(gen_functor_convert_basic_type_to_double(A), ##__VA_ARGS__))

#define array_mad(A, N, mean, result_out, ... /* get_val_as_double() */)    \
	array_seg_mad(A, 0, N, mean, result_out, ##__VA_ARGS__)


//==========================================================================================================================================
//= Variance - One Pass
//==========================================================================================================================================


struct ARRAY_METRICS_var_s {
	void * A;
	long i_start;
	long i_end;
	double stability_factor;
	double shifted_mean;
	double shifted_ms;
	double * result_out;
};

void ARRAY_METRICS_var_init(int concurrent, struct ARRAY_METRICS_var_s * s, void * A, long i_start, long i_end, double * result_out, double (* get_val_as_double)(void * A, long i));
void ARRAY_METRICS_var_process(struct ARRAY_METRICS_var_s * s, long i, double (* get_val_as_double)(void * A, long i));
struct ARRAY_METRICS_var_s ARRAY_METRICS_var_combine(struct ARRAY_METRICS_var_s s1, struct ARRAY_METRICS_var_s s2);
void ARRAY_METRICS_var_finalize(struct ARRAY_METRICS_var_s * s);
void ARRAY_METRICS_var_output(struct ARRAY_METRICS_var_s * s_local, struct ARRAY_METRICS_var_s * s_global);

void ARRAY_METRICS_var_serial(void * A, long i_start, long i_end, double * result_out, double (* get_val_as_double)(void * A, long i));
void ARRAY_METRICS_var_concurrent(void * A, long i_start, long i_end, double * result_out, double (* get_val_as_double)(void * A, long i));
void ARRAY_METRICS_var(void * A, long i_start, long i_end, double * result_out, double (* get_val_as_double)(void * A, long i));

#define array_seg_var_serial(A, i_start, i_end, result_out, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_var_serial(A, i_start, i_end, result_out, DEFAULT_ARG_1(gen_functor_convert_basic_type_to_double(A), ##__VA_ARGS__))

#define array_var_serial(A, N, result_out, ... /* get_val_as_double() */)    \
	array_seg_var_serial(A, 0, N, result_out, ##__VA_ARGS__)

#define array_seg_var_concurrent(A, i_start, i_end, result_out, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_var_concurrent(A, i_start, i_end, result_out, DEFAULT_ARG_1(gen_functor_convert_basic_type_to_double(A), ##__VA_ARGS__))

#define array_var_concurrent(A, N, result_out, ... /* get_val_as_double() */)    \
	array_seg_var_concurrent(A, 0, N, result_out, ##__VA_ARGS__)

#define array_seg_var(A, i_start, i_end, result_out, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_var(A, i_start, i_end, result_out, DEFAULT_ARG_1(gen_functor_convert_basic_type_to_double(A), ##__VA_ARGS__))

#define array_var(A, N, result_out, ... /* get_val_as_double() */)    \
	array_seg_var(A, 0, N, result_out, ##__VA_ARGS__)


//==========================================================================================================================================
//= Variance - Given Mean
//==========================================================================================================================================


struct ARRAY_METRICS_var_orig_s {
	void * A;
	long i_start;
	long i_end;
	double mean;
	double partial;
	double * result_out;
};

void ARRAY_METRICS_var_orig_init(int concurrent, struct ARRAY_METRICS_var_orig_s * s, void * A, long i_start, long i_end, double mean, double * result_out, double (* get_val_as_double)(void * A, long i));
void ARRAY_METRICS_var_orig_process(struct ARRAY_METRICS_var_orig_s * s, long i, double (* get_val_as_double)(void * A, long i));
struct ARRAY_METRICS_var_orig_s ARRAY_METRICS_var_orig_combine(struct ARRAY_METRICS_var_orig_s s1, struct ARRAY_METRICS_var_orig_s s2);
void ARRAY_METRICS_var_orig_finalize(struct ARRAY_METRICS_var_orig_s * s);
void ARRAY_METRICS_var_orig_output(struct ARRAY_METRICS_var_orig_s * s_local, struct ARRAY_METRICS_var_orig_s * s_global);

void ARRAY_METRICS_var_orig_serial(void * A, long i_start, long i_end, double mean, double * result_out, double (* get_val_as_double)(void * A, long i));
void ARRAY_METRICS_var_orig_concurrent(void * A, long i_start, long i_end, double mean, double * result_out, double (* get_val_as_double)(void * A, long i));
void ARRAY_METRICS_var_orig(void * A, long i_start, long i_end, double mean, double * result_out, double (* get_val_as_double)(void * A, long i));

#define array_seg_var_orig_serial(A, i_start, i_end, mean, result_out, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_var_orig_serial(A, i_start, i_end, mean, result_out, DEFAULT_ARG_1(gen_functor_convert_basic_type_to_double(A), ##__VA_ARGS__))

#define array_var_orig_serial(A, N, mean, result_out, ... /* get_val_as_double() */)    \
	array_seg_var_orig_serial(A, 0, N, mean, result_out, ##__VA_ARGS__)

#define array_seg_var_orig_concurrent(A, i_start, i_end, mean, result_out, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_var_orig_concurrent(A, i_start, i_end, mean, result_out, DEFAULT_ARG_1(gen_functor_convert_basic_type_to_double(A), ##__VA_ARGS__))

#define array_var_orig_concurrent(A, N, mean, result_out, ... /* get_val_as_double() */)    \
	array_seg_var_orig_concurrent(A, 0, N, mean, result_out, ##__VA_ARGS__)

#define array_seg_var_orig(A, i_start, i_end, mean, result_out, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_var_orig(A, i_start, i_end, mean, result_out, DEFAULT_ARG_1(gen_functor_convert_basic_type_to_double(A), ##__VA_ARGS__))

#define array_var_orig(A, N, mean, result_out, ... /* get_val_as_double() */)    \
	array_seg_var_orig(A, 0, N, mean, result_out, ##__VA_ARGS__)


//==========================================================================================================================================
//= Standard Deviation
//==========================================================================================================================================


struct ARRAY_METRICS_std_s {
	struct ARRAY_METRICS_var_s var_s;
};

void ARRAY_METRICS_std_init(int concurrent, struct ARRAY_METRICS_std_s * s, void * A, long i_start, long i_end, double * result_out, double (* get_val_as_double)(void * A, long i));
void ARRAY_METRICS_std_process(struct ARRAY_METRICS_std_s * s, long i, double (* get_val_as_double)(void * A, long i));
struct ARRAY_METRICS_std_s ARRAY_METRICS_std_combine(struct ARRAY_METRICS_std_s s1, struct ARRAY_METRICS_std_s s2);
void ARRAY_METRICS_std_finalize(struct ARRAY_METRICS_std_s * s);
void ARRAY_METRICS_std_output(struct ARRAY_METRICS_std_s * s_local, struct ARRAY_METRICS_std_s * s_global);

void ARRAY_METRICS_std_serial(void * A, long i_start, long i_end, double * result_out, double (* get_val_as_double)(void * A, long i));
void ARRAY_METRICS_std_concurrent(void * A, long i_start, long i_end, double * result_out, double (* get_val_as_double)(void * A, long i));
void ARRAY_METRICS_std(void * A, long i_start, long i_end, double * result_out, double (* get_val_as_double)(void * A, long i));

#define array_seg_std_serial(A, i_start, i_end, result_out, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_std_serial(A, i_start, i_end, result_out, DEFAULT_ARG_1(gen_functor_convert_basic_type_to_double(A), ##__VA_ARGS__))

#define array_std_serial(A, N, result_out, ... /* get_val_as_double() */)    \
	array_seg_std_serial(A, 0, N, result_out, ##__VA_ARGS__)

#define array_seg_std_concurrent(A, i_start, i_end, result_out, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_std_concurrent(A, i_start, i_end, result_out, DEFAULT_ARG_1(gen_functor_convert_basic_type_to_double(A), ##__VA_ARGS__))

#define array_std_concurrent(A, N, result_out, ... /* get_val_as_double() */)    \
	array_seg_std_concurrent(A, 0, N, result_out, ##__VA_ARGS__)

#define array_seg_std(A, i_start, i_end, result_out, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_std(A, i_start, i_end, result_out, DEFAULT_ARG_1(gen_functor_convert_basic_type_to_double(A), ##__VA_ARGS__))

#define array_std(A, N, result_out, ... /* get_val_as_double() */)    \
	array_seg_std(A, 0, N, result_out, ##__VA_ARGS__)


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


struct ARRAY_METRICS_mae_s {
	void * A;
	void * F;
	long i_start;
	long i_end;
	double partial;
	double * result_out;
};

void ARRAY_METRICS_mae_init(int concurrent, struct ARRAY_METRICS_mae_s * s, void * A, void * F, long i_start, long i_end, double * result_out, double (* get_val_as_double)(void * A, long i));
void ARRAY_METRICS_mae_process(struct ARRAY_METRICS_mae_s * s, long i, double (* get_val_as_double)(void * A, long i));
struct ARRAY_METRICS_mae_s ARRAY_METRICS_mae_combine(struct ARRAY_METRICS_mae_s s1, struct ARRAY_METRICS_mae_s s2);
void ARRAY_METRICS_mae_finalize(struct ARRAY_METRICS_mae_s * s);
void ARRAY_METRICS_mae_output(struct ARRAY_METRICS_mae_s * s_local, struct ARRAY_METRICS_mae_s * s_global);

void ARRAY_METRICS_mae_serial(void * A, void * F, long i_start, long i_end, double * result_out, double (* get_val_as_double)(void * A, long i));
void ARRAY_METRICS_mae_concurrent(void * A, void * F, long i_start, long i_end, double * result_out, double (* get_val_as_double)(void * A, long i));
void ARRAY_METRICS_mae(void * A, void * F, long i_start, long i_end, double * result_out, double (* get_val_as_double)(void * A, long i));

#define array_seg_mae_serial(A, F, i_start, i_end, result_out, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_mae_serial(A, F, i_start, i_end, result_out, DEFAULT_ARG_1(gen_functor_convert_basic_type_to_double(A), ##__VA_ARGS__))

#define array_mae_serial(A, F, N, result_out, ... /* get_val_as_double() */)    \
	array_seg_mae_serial(A, F, 0, N, result_out, ##__VA_ARGS__)

#define array_seg_mae_concurrent(A, F, i_start, i_end, result_out, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_mae_concurrent(A, F, i_start, i_end, result_out, DEFAULT_ARG_1(gen_functor_convert_basic_type_to_double(A), ##__VA_ARGS__))

#define array_mae_concurrent(A, F, N, result_out, ... /* get_val_as_double() */)    \
	array_seg_mae_concurrent(A, F, 0, N, result_out, ##__VA_ARGS__)

#define array_seg_mae(A, F, i_start, i_end, result_out, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_mae(A, F, i_start, i_end, result_out, DEFAULT_ARG_1(gen_functor_convert_basic_type_to_double(A), ##__VA_ARGS__))

#define array_mae(A, F, N, result_out, ... /* get_val_as_double() */)    \
	array_seg_mae(A, F, 0, N, result_out, ##__VA_ARGS__)


//==========================================================================================================================================
//= Max Absolute Error
//==========================================================================================================================================


struct ARRAY_METRICS_max_ae_s {
	void * A;
	void * F;
	long i_start;
	long i_end;
	double partial;
	double * result_out;
};

void ARRAY_METRICS_max_ae_init(int concurrent, struct ARRAY_METRICS_max_ae_s * s, void * A, void * F, long i_start, long i_end, double * result_out, double (* get_val_as_double)(void * A, long i));
void ARRAY_METRICS_max_ae_process(struct ARRAY_METRICS_max_ae_s * s, long i, double (* get_val_as_double)(void * A, long i));
struct ARRAY_METRICS_max_ae_s ARRAY_METRICS_max_ae_combine(struct ARRAY_METRICS_max_ae_s s1, struct ARRAY_METRICS_max_ae_s s2);
void ARRAY_METRICS_max_ae_finalize(struct ARRAY_METRICS_max_ae_s * s);
void ARRAY_METRICS_max_ae_output(struct ARRAY_METRICS_max_ae_s * s_local, struct ARRAY_METRICS_max_ae_s * s_global);

void ARRAY_METRICS_max_ae_serial(void * A, void * F, long i_start, long i_end, double * result_out, double (* get_val_as_double)(void * A, long i));
void ARRAY_METRICS_max_ae_concurrent(void * A, void * F, long i_start, long i_end, double * result_out, double (* get_val_as_double)(void * A, long i));
void ARRAY_METRICS_max_ae(void * A, void * F, long i_start, long i_end, double * result_out, double (* get_val_as_double)(void * A, long i));

#define array_seg_max_ae_serial(A, F, i_start, i_end, result_out, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_max_ae_serial(A, F, i_start, i_end, result_out, DEFAULT_ARG_1(gen_functor_convert_basic_type_to_double(A), ##__VA_ARGS__))

#define array_max_ae_serial(A, F, N, result_out, ... /* get_val_as_double() */)    \
	array_seg_max_ae_serial(A, F, 0, N, result_out, ##__VA_ARGS__)

#define array_seg_max_ae_concurrent(A, F, i_start, i_end, result_out, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_max_ae_concurrent(A, F, i_start, i_end, result_out, DEFAULT_ARG_1(gen_functor_convert_basic_type_to_double(A), ##__VA_ARGS__))

#define array_max_ae_concurrent(A, F, N, result_out, ... /* get_val_as_double() */)    \
	array_seg_max_ae_concurrent(A, F, 0, N, result_out, ##__VA_ARGS__)

#define array_seg_max_ae(A, F, i_start, i_end, result_out, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_max_ae(A, F, i_start, i_end, result_out, DEFAULT_ARG_1(gen_functor_convert_basic_type_to_double(A), ##__VA_ARGS__))

#define array_max_ae(A, F, N, result_out, ... /* get_val_as_double() */)    \
	array_seg_max_ae(A, F, 0, N, result_out, ##__VA_ARGS__)


//==========================================================================================================================================
//= Mean Squared Error
//==========================================================================================================================================


struct ARRAY_METRICS_mse_s {
	void * A;
	void * F;
	long i_start;
	long i_end;
	double partial;
	double * result_out;
};

void ARRAY_METRICS_mse_init(int concurrent, struct ARRAY_METRICS_mse_s * s, void * A, void * F, long i_start, long i_end, double * result_out, double (* get_val_as_double)(void * A, long i));
void ARRAY_METRICS_mse_process(struct ARRAY_METRICS_mse_s * s, long i, double (* get_val_as_double)(void * A, long i));
struct ARRAY_METRICS_mse_s ARRAY_METRICS_mse_combine(struct ARRAY_METRICS_mse_s s1, struct ARRAY_METRICS_mse_s s2);
void ARRAY_METRICS_mse_finalize(struct ARRAY_METRICS_mse_s * s);
void ARRAY_METRICS_mse_output(struct ARRAY_METRICS_mse_s * s_local, struct ARRAY_METRICS_mse_s * s_global);

void ARRAY_METRICS_mse_serial(void * A, void * F, long i_start, long i_end, double * result_out, double (* get_val_as_double)(void * A, long i));
void ARRAY_METRICS_mse_concurrent(void * A, void * F, long i_start, long i_end, double * result_out, double (* get_val_as_double)(void * A, long i));
void ARRAY_METRICS_mse(void * A, void * F, long i_start, long i_end, double * result_out, double (* get_val_as_double)(void * A, long i));

#define array_seg_mse_serial(A, F, i_start, i_end, result_out, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_mse_serial(A, F, i_start, i_end, result_out, DEFAULT_ARG_1(gen_functor_convert_basic_type_to_double(A), ##__VA_ARGS__))

#define array_mse_serial(A, F, N, result_out, ... /* get_val_as_double() */)    \
	array_seg_mse_serial(A, F, 0, N, result_out, ##__VA_ARGS__)

#define array_seg_mse_concurrent(A, F, i_start, i_end, result_out, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_mse_concurrent(A, F, i_start, i_end, result_out, DEFAULT_ARG_1(gen_functor_convert_basic_type_to_double(A), ##__VA_ARGS__))

#define array_mse_concurrent(A, F, N, result_out, ... /* get_val_as_double() */)    \
	array_seg_mse_concurrent(A, F, 0, N, result_out, ##__VA_ARGS__)

#define array_seg_mse(A, F, i_start, i_end, result_out, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_mse(A, F, i_start, i_end, result_out, DEFAULT_ARG_1(gen_functor_convert_basic_type_to_double(A), ##__VA_ARGS__))

#define array_mse(A, F, N, result_out, ... /* get_val_as_double() */)    \
	array_seg_mse(A, F, 0, N, result_out, ##__VA_ARGS__)


//==========================================================================================================================================
//= Mean Absolute Relative Error
//==========================================================================================================================================


struct ARRAY_METRICS_mare_s {
	void * A;
	void * F;
	long i_start;
	long i_end;
	double partial;
	double * result_out;
};

void ARRAY_METRICS_mare_init(int concurrent, struct ARRAY_METRICS_mare_s * s, void * A, void * F, long i_start, long i_end, double * result_out, double (* get_val_as_double)(void * A, long i));
void ARRAY_METRICS_mare_process(struct ARRAY_METRICS_mare_s * s, long i, double (* get_val_as_double)(void * A, long i));
struct ARRAY_METRICS_mare_s ARRAY_METRICS_mare_combine(struct ARRAY_METRICS_mare_s s1, struct ARRAY_METRICS_mare_s s2);
void ARRAY_METRICS_mare_finalize(struct ARRAY_METRICS_mare_s * s);
void ARRAY_METRICS_mare_output(struct ARRAY_METRICS_mare_s * s_local, struct ARRAY_METRICS_mare_s * s_global);

void ARRAY_METRICS_mare_serial(void * A, void * F, long i_start, long i_end, double * result_out, double (* get_val_as_double)(void * A, long i));
void ARRAY_METRICS_mare_concurrent(void * A, void * F, long i_start, long i_end, double * result_out, double (* get_val_as_double)(void * A, long i));
void ARRAY_METRICS_mare(void * A, void * F, long i_start, long i_end, double * result_out, double (* get_val_as_double)(void * A, long i));

#define array_seg_mare_serial(A, F, i_start, i_end, result_out, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_mare_serial(A, F, i_start, i_end, result_out, DEFAULT_ARG_1(gen_functor_convert_basic_type_to_double(A), ##__VA_ARGS__))

#define array_mare_serial(A, F, N, result_out, ... /* get_val_as_double() */)    \
	array_seg_mare_serial(A, F, 0, N, result_out, ##__VA_ARGS__)

#define array_seg_mare_concurrent(A, F, i_start, i_end, result_out, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_mare_concurrent(A, F, i_start, i_end, result_out, DEFAULT_ARG_1(gen_functor_convert_basic_type_to_double(A), ##__VA_ARGS__))

#define array_mare_concurrent(A, F, N, result_out, ... /* get_val_as_double() */)    \
	array_seg_mare_concurrent(A, F, 0, N, result_out, ##__VA_ARGS__)

#define array_seg_mare(A, F, i_start, i_end, result_out, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_mare(A, F, i_start, i_end, result_out, DEFAULT_ARG_1(gen_functor_convert_basic_type_to_double(A), ##__VA_ARGS__))

#define array_mare(A, F, N, result_out, ... /* get_val_as_double() */)    \
	array_seg_mare(A, F, 0, N, result_out, ##__VA_ARGS__)


//==========================================================================================================================================
//= Mean Absolute Percentage Error
//==========================================================================================================================================


struct ARRAY_METRICS_mape_s {
	void * A;
	void * F;
	long i_start;
	long i_end;
	double partial;
	double * result_out;
};

void ARRAY_METRICS_mape_init(int concurrent, struct ARRAY_METRICS_mape_s * s, void * A, void * F, long i_start, long i_end, double * result_out, double (* get_val_as_double)(void * A, long i));
void ARRAY_METRICS_mape_process(struct ARRAY_METRICS_mape_s * s, long i, double (* get_val_as_double)(void * A, long i));
struct ARRAY_METRICS_mape_s ARRAY_METRICS_mape_combine(struct ARRAY_METRICS_mape_s s1, struct ARRAY_METRICS_mape_s s2);
void ARRAY_METRICS_mape_finalize(struct ARRAY_METRICS_mape_s * s);
void ARRAY_METRICS_mape_output(struct ARRAY_METRICS_mape_s * s_local, struct ARRAY_METRICS_mape_s * s_global);

void ARRAY_METRICS_mape_serial(void * A, void * F, long i_start, long i_end, double * result_out, double (* get_val_as_double)(void * A, long i));
void ARRAY_METRICS_mape_concurrent(void * A, void * F, long i_start, long i_end, double * result_out, double (* get_val_as_double)(void * A, long i));
void ARRAY_METRICS_mape(void * A, void * F, long i_start, long i_end, double * result_out, double (* get_val_as_double)(void * A, long i));

#define array_seg_mape_serial(A, F, i_start, i_end, result_out, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_mape_serial(A, F, i_start, i_end, result_out, DEFAULT_ARG_1(gen_functor_convert_basic_type_to_double(A), ##__VA_ARGS__))

#define array_mape_serial(A, F, N, result_out, ... /* get_val_as_double() */)    \
	array_seg_mape_serial(A, F, 0, N, result_out, ##__VA_ARGS__)

#define array_seg_mape_concurrent(A, F, i_start, i_end, result_out, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_mape_concurrent(A, F, i_start, i_end, result_out, DEFAULT_ARG_1(gen_functor_convert_basic_type_to_double(A), ##__VA_ARGS__))

#define array_mape_concurrent(A, F, N, result_out, ... /* get_val_as_double() */)    \
	array_seg_mape_concurrent(A, F, 0, N, result_out, ##__VA_ARGS__)

#define array_seg_mape(A, F, i_start, i_end, result_out, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_mape(A, F, i_start, i_end, result_out, DEFAULT_ARG_1(gen_functor_convert_basic_type_to_double(A), ##__VA_ARGS__))

#define array_mape(A, F, N, result_out, ... /* get_val_as_double() */)    \
	array_seg_mape(A, F, 0, N, result_out, ##__VA_ARGS__)


//==========================================================================================================================================
//= Symmetric Mean Absolute Relative Error
//==========================================================================================================================================


struct ARRAY_METRICS_smare_s {
	void * A;
	void * F;
	long i_start;
	long i_end;
	double partial;
	double * result_out;
};

void ARRAY_METRICS_smare_init(int concurrent, struct ARRAY_METRICS_smare_s * s, void * A, void * F, long i_start, long i_end, double * result_out, double (* get_val_as_double)(void * A, long i));
void ARRAY_METRICS_smare_process(struct ARRAY_METRICS_smare_s * s, long i, double (* get_val_as_double)(void * A, long i));
struct ARRAY_METRICS_smare_s ARRAY_METRICS_smare_combine(struct ARRAY_METRICS_smare_s s1, struct ARRAY_METRICS_smare_s s2);
void ARRAY_METRICS_smare_finalize(struct ARRAY_METRICS_smare_s * s);
void ARRAY_METRICS_smare_output(struct ARRAY_METRICS_smare_s * s_local, struct ARRAY_METRICS_smare_s * s_global);

void ARRAY_METRICS_smare_serial(void * A, void * F, long i_start, long i_end, double * result_out, double (* get_val_as_double)(void * A, long i));
void ARRAY_METRICS_smare_concurrent(void * A, void * F, long i_start, long i_end, double * result_out, double (* get_val_as_double)(void * A, long i));
void ARRAY_METRICS_smare(void * A, void * F, long i_start, long i_end, double * result_out, double (* get_val_as_double)(void * A, long i));

#define array_seg_smare_serial(A, F, i_start, i_end, result_out, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_smare_serial(A, F, i_start, i_end, result_out, DEFAULT_ARG_1(gen_functor_convert_basic_type_to_double(A), ##__VA_ARGS__))

#define array_smare_serial(A, F, N, result_out, ... /* get_val_as_double() */)    \
	array_seg_smare_serial(A, F, 0, N, result_out, ##__VA_ARGS__)

#define array_seg_smare_concurrent(A, F, i_start, i_end, result_out, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_smare_concurrent(A, F, i_start, i_end, result_out, DEFAULT_ARG_1(gen_functor_convert_basic_type_to_double(A), ##__VA_ARGS__))

#define array_smare_concurrent(A, F, N, result_out, ... /* get_val_as_double() */)    \
	array_seg_smare_concurrent(A, F, 0, N, result_out, ##__VA_ARGS__)

#define array_seg_smare(A, F, i_start, i_end, result_out, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_smare(A, F, i_start, i_end, result_out, DEFAULT_ARG_1(gen_functor_convert_basic_type_to_double(A), ##__VA_ARGS__))

#define array_smare(A, F, N, result_out, ... /* get_val_as_double() */)    \
	array_seg_smare(A, F, 0, N, result_out, ##__VA_ARGS__)


//==========================================================================================================================================
//= Symmetric Mean Absolute Percentage Error
//==========================================================================================================================================


struct ARRAY_METRICS_smape_s {
	void * A;
	void * F;
	long i_start;
	long i_end;
	double partial;
	double * result_out;
};

void ARRAY_METRICS_smape_init(int concurrent, struct ARRAY_METRICS_smape_s * s, void * A, void * F, long i_start, long i_end, double * result_out, double (* get_val_as_double)(void * A, long i));
void ARRAY_METRICS_smape_process(struct ARRAY_METRICS_smape_s * s, long i, double (* get_val_as_double)(void * A, long i));
struct ARRAY_METRICS_smape_s ARRAY_METRICS_smape_combine(struct ARRAY_METRICS_smape_s s1, struct ARRAY_METRICS_smape_s s2);
void ARRAY_METRICS_smape_finalize(struct ARRAY_METRICS_smape_s * s);
void ARRAY_METRICS_smape_output(struct ARRAY_METRICS_smape_s * s_local, struct ARRAY_METRICS_smape_s * s_global);

void ARRAY_METRICS_smape_serial(void * A, void * F, long i_start, long i_end, double * result_out, double (* get_val_as_double)(void * A, long i));
void ARRAY_METRICS_smape_concurrent(void * A, void * F, long i_start, long i_end, double * result_out, double (* get_val_as_double)(void * A, long i));
void ARRAY_METRICS_smape(void * A, void * F, long i_start, long i_end, double * result_out, double (* get_val_as_double)(void * A, long i));

#define array_seg_smape_serial(A, F, i_start, i_end, result_out, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_smape_serial(A, F, i_start, i_end, result_out, DEFAULT_ARG_1(gen_functor_convert_basic_type_to_double(A), ##__VA_ARGS__))

#define array_smape_serial(A, F, N, result_out, ... /* get_val_as_double() */)    \
	array_seg_smape_serial(A, F, 0, N, result_out, ##__VA_ARGS__)

#define array_seg_smape_concurrent(A, F, i_start, i_end, result_out, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_smape_concurrent(A, F, i_start, i_end, result_out, DEFAULT_ARG_1(gen_functor_convert_basic_type_to_double(A), ##__VA_ARGS__))

#define array_smape_concurrent(A, F, N, result_out, ... /* get_val_as_double() */)    \
	array_seg_smape_concurrent(A, F, 0, N, result_out, ##__VA_ARGS__)

#define array_seg_smape(A, F, i_start, i_end, result_out, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_smape(A, F, i_start, i_end, result_out, DEFAULT_ARG_1(gen_functor_convert_basic_type_to_double(A), ##__VA_ARGS__))

#define array_smape(A, F, N, result_out, ... /* get_val_as_double() */)    \
	array_seg_smape(A, F, 0, N, result_out, ##__VA_ARGS__)


//==========================================================================================================================================
//= Q Error - Relative Error
//==========================================================================================================================================


struct ARRAY_METRICS_Q_error_s {
	void * A;
	void * F;
	long i_start;
	long i_end;
	double partial;
	double * result_out;
};

void ARRAY_METRICS_Q_error_init(int concurrent, struct ARRAY_METRICS_Q_error_s * s, void * A, void * F, long i_start, long i_end, double * result_out, double (* get_val_as_double)(void * A, long i));
void ARRAY_METRICS_Q_error_process(struct ARRAY_METRICS_Q_error_s * s, long i, double (* get_val_as_double)(void * A, long i));
struct ARRAY_METRICS_Q_error_s ARRAY_METRICS_Q_error_combine(struct ARRAY_METRICS_Q_error_s s1, struct ARRAY_METRICS_Q_error_s s2);
void ARRAY_METRICS_Q_error_finalize(struct ARRAY_METRICS_Q_error_s * s);
void ARRAY_METRICS_Q_error_output(struct ARRAY_METRICS_Q_error_s * s_local, struct ARRAY_METRICS_Q_error_s * s_global);

void ARRAY_METRICS_Q_error_serial(void * A, void * F, long i_start, long i_end, double * result_out, double (* get_val_as_double)(void * A, long i));
void ARRAY_METRICS_Q_error_concurrent(void * A, void * F, long i_start, long i_end, double * result_out, double (* get_val_as_double)(void * A, long i));
void ARRAY_METRICS_Q_error(void * A, void * F, long i_start, long i_end, double * result_out, double (* get_val_as_double)(void * A, long i));

#define array_seg_Q_error_serial(A, F, i_start, i_end, result_out, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_Q_error_serial(A, F, i_start, i_end, result_out, DEFAULT_ARG_1(gen_functor_convert_basic_type_to_double(A), ##__VA_ARGS__))

#define array_Q_error_serial(A, F, N, result_out, ... /* get_val_as_double() */)    \
	array_seg_Q_error_serial(A, F, 0, N, result_out, ##__VA_ARGS__)

#define array_seg_Q_error_concurrent(A, F, i_start, i_end, result_out, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_Q_error_concurrent(A, F, i_start, i_end, result_out, DEFAULT_ARG_1(gen_functor_convert_basic_type_to_double(A), ##__VA_ARGS__))

#define array_Q_error_concurrent(A, F, N, result_out, ... /* get_val_as_double() */)    \
	array_seg_Q_error_concurrent(A, F, 0, N, result_out, ##__VA_ARGS__)

#define array_seg_Q_error(A, F, i_start, i_end, result_out, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_Q_error(A, F, i_start, i_end, result_out, DEFAULT_ARG_1(gen_functor_convert_basic_type_to_double(A), ##__VA_ARGS__))

#define array_Q_error(A, F, N, result_out, ... /* get_val_as_double() */)    \
	array_seg_Q_error(A, F, 0, N, result_out, ##__VA_ARGS__)


//==========================================================================================================================================
//= Ln Q Error - Log Relative Error
//==========================================================================================================================================


struct ARRAY_METRICS_lnQ_error_s {
	void * A;
	void * F;
	long i_start;
	long i_end;
	double partial;
	double * result_out;
};

void ARRAY_METRICS_lnQ_error_init(int concurrent, struct ARRAY_METRICS_lnQ_error_s * s, void * A, void * F, long i_start, long i_end, double * result_out, double (* get_val_as_double)(void * A, long i));
void ARRAY_METRICS_lnQ_error_process(struct ARRAY_METRICS_lnQ_error_s * s, long i, double (* get_val_as_double)(void * A, long i));
struct ARRAY_METRICS_lnQ_error_s ARRAY_METRICS_lnQ_error_combine(struct ARRAY_METRICS_lnQ_error_s s1, struct ARRAY_METRICS_lnQ_error_s s2);
void ARRAY_METRICS_lnQ_error_finalize(struct ARRAY_METRICS_lnQ_error_s * s);
void ARRAY_METRICS_lnQ_error_output(struct ARRAY_METRICS_lnQ_error_s * s_local, struct ARRAY_METRICS_lnQ_error_s * s_global);

void ARRAY_METRICS_lnQ_error_serial(void * A, void * F, long i_start, long i_end, double * result_out, double (* get_val_as_double)(void * A, long i));
void ARRAY_METRICS_lnQ_error_concurrent(void * A, void * F, long i_start, long i_end, double * result_out, double (* get_val_as_double)(void * A, long i));
void ARRAY_METRICS_lnQ_error(void * A, void * F, long i_start, long i_end, double * result_out, double (* get_val_as_double)(void * A, long i));

#define array_seg_lnQ_error_serial(A, F, i_start, i_end, result_out, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_lnQ_error_serial(A, F, i_start, i_end, result_out, DEFAULT_ARG_1(gen_functor_convert_basic_type_to_double(A), ##__VA_ARGS__))

#define array_lnQ_error_serial(A, F, N, result_out, ... /* get_val_as_double() */)    \
	array_seg_lnQ_error_serial(A, F, 0, N, result_out, ##__VA_ARGS__)

#define array_seg_lnQ_error_concurrent(A, F, i_start, i_end, result_out, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_lnQ_error_concurrent(A, F, i_start, i_end, result_out, DEFAULT_ARG_1(gen_functor_convert_basic_type_to_double(A), ##__VA_ARGS__))

#define array_lnQ_error_concurrent(A, F, N, result_out, ... /* get_val_as_double() */)    \
	array_seg_lnQ_error_concurrent(A, F, 0, N, result_out, ##__VA_ARGS__)

#define array_seg_lnQ_error(A, F, i_start, i_end, result_out, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_lnQ_error(A, F, i_start, i_end, result_out, DEFAULT_ARG_1(gen_functor_convert_basic_type_to_double(A), ##__VA_ARGS__))

#define array_lnQ_error(A, F, N, result_out, ... /* get_val_as_double() */)    \
	array_seg_lnQ_error(A, F, 0, N, result_out, ##__VA_ARGS__)


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//------------------------------------------------------------------------------------------------------------------------------------------
//-                                                         Bit Characteristics                                                            -
//------------------------------------------------------------------------------------------------------------------------------------------
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


//==========================================================================================================================================
//= Mean
//==========================================================================================================================================


struct ARRAY_METRICS_mean_trailing_zeros_s {
	void * A;
	long i_start;
	long i_end;
	double partial;
	double * result_out;
};

void ARRAY_METRICS_mean_trailing_zeros_init(int concurrent, struct ARRAY_METRICS_mean_trailing_zeros_s * s, void * A, long i_start, long i_end, double * result_out, double (* get_val_as_double)(void * A, long i));
void ARRAY_METRICS_mean_trailing_zeros_process(struct ARRAY_METRICS_mean_trailing_zeros_s * s, long i, double (* get_val_as_double)(void * A, long i));
struct ARRAY_METRICS_mean_trailing_zeros_s ARRAY_METRICS_mean_trailing_zeros_combine(struct ARRAY_METRICS_mean_trailing_zeros_s s1, struct ARRAY_METRICS_mean_trailing_zeros_s s2);
void ARRAY_METRICS_mean_trailing_zeros_finalize(struct ARRAY_METRICS_mean_trailing_zeros_s * s);
void ARRAY_METRICS_mean_trailing_zeros_output(struct ARRAY_METRICS_mean_trailing_zeros_s * s_local, struct ARRAY_METRICS_mean_trailing_zeros_s * s_global);

void ARRAY_METRICS_mean_trailing_zeros_serial(void * A, long i_start, long i_end, double * result_out, double (* get_val_as_double)(void * A, long i));
void ARRAY_METRICS_mean_trailing_zeros_concurrent(void * A, long i_start, long i_end, double * result_out, double (* get_val_as_double)(void * A, long i));
void ARRAY_METRICS_mean_trailing_zeros(void * A, long i_start, long i_end, double * result_out, double (* get_val_as_double)(void * A, long i));

#define array_seg_mean_trailing_zeros_serial(A, i_start, i_end, result_out, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_mean_trailing_zeros_serial(A, i_start, i_end, result_out, DEFAULT_ARG_1(gen_functor_convert_basic_type_to_double(A), ##__VA_ARGS__))

#define array_mean_trailing_zeros_serial(A, N, result_out, ... /* get_val_as_double() */)    \
	array_seg_mean_trailing_zeros_serial(A, 0, N, result_out, ##__VA_ARGS__)

#define array_seg_mean_trailing_zeros_concurrent(A, i_start, i_end, result_out, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_mean_trailing_zeros_concurrent(A, i_start, i_end, result_out, DEFAULT_ARG_1(gen_functor_convert_basic_type_to_double(A), ##__VA_ARGS__))

#define array_mean_trailing_zeros_concurrent(A, N, result_out, ... /* get_val_as_double() */)    \
	array_seg_mean_trailing_zeros_concurrent(A, 0, N, result_out, ##__VA_ARGS__)

#define array_seg_mean_trailing_zeros(A, i_start, i_end, result_out, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_mean_trailing_zeros(A, i_start, i_end, result_out, DEFAULT_ARG_1(gen_functor_convert_basic_type_to_double(A), ##__VA_ARGS__))

#define array_mean_trailing_zeros(A, N, result_out, ... /* get_val_as_double() */)    \
	array_seg_mean_trailing_zeros(A, 0, N, result_out, ##__VA_ARGS__)


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//------------------------------------------------------------------------------------------------------------------------------------------
//-                                                           Unique Values                                                                -
//------------------------------------------------------------------------------------------------------------------------------------------
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


//==========================================================================================================================================
//= Number of Unique Values
//==========================================================================================================================================


struct ARRAY_METRICS_unique_num_s {
	void * A;
	long i_start;
	long i_end;
	int concurrent;
	void * ht;         // Hide the hashtable datatype
	long num_vals;
	long * num_vals_out;
};

void ARRAY_METRICS_unique_num_init(int concurrent, struct ARRAY_METRICS_unique_num_s * s, void * A, long i_start, long i_end, long * num_vals_out, double (* get_val_as_double)(void * A, long i));
void ARRAY_METRICS_unique_num_process(struct ARRAY_METRICS_unique_num_s * s, long i, double (* get_val_as_double)(void * A, long i));
struct ARRAY_METRICS_unique_num_s ARRAY_METRICS_unique_num_combine(struct ARRAY_METRICS_unique_num_s s1, struct ARRAY_METRICS_unique_num_s s2);
void ARRAY_METRICS_unique_num_finalize(struct ARRAY_METRICS_unique_num_s * s);
void ARRAY_METRICS_unique_num_output(struct ARRAY_METRICS_unique_num_s * s_local, struct ARRAY_METRICS_unique_num_s * s_global);

void ARRAY_METRICS_unique_num_serial(void * A, long i_start, long i_end, long * num_vals_out, double (* get_val_as_double)(void * A, long i));
void ARRAY_METRICS_unique_num_concurrent(void * A, long i_start, long i_end, long * num_vals_out, double (* get_val_as_double)(void * A, long i));
void ARRAY_METRICS_unique_num(void * A, long i_start, long i_end, long * num_vals_out, double (* get_val_as_double)(void * A, long i));

#define array_seg_unique_num_serial(A, i_start, i_end, num_vals_out, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_unique_num_serial(A, i_start, i_end, num_vals_out, DEFAULT_ARG_1(gen_functor_convert_basic_type_to_double(A), ##__VA_ARGS__))

#define array_unique_num_serial(A, N, num_vals_out, ... /* get_val_as_double() */)    \
	array_seg_unique_num_serial(A, 0, N, num_vals_out, ##__VA_ARGS__)

#define array_seg_unique_num_concurrent(A, i_start, i_end, num_vals_out, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_unique_num_concurrent(A, i_start, i_end, num_vals_out, DEFAULT_ARG_1(gen_functor_convert_basic_type_to_double(A), ##__VA_ARGS__))

#define array_unique_num_concurrent(A, N, num_vals_out, ... /* get_val_as_double() */)    \
	array_seg_unique_num_concurrent(A, 0, N, num_vals_out, ##__VA_ARGS__)

#define array_seg_unique_num(A, i_start, i_end, num_vals_out, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_unique_num(A, i_start, i_end, num_vals_out, DEFAULT_ARG_1(gen_functor_convert_basic_type_to_double(A), ##__VA_ARGS__))

#define array_unique_num(A, N, num_vals_out, ... /* get_val_as_double() */)    \
	array_seg_unique_num(A, 0, N, num_vals_out, ##__VA_ARGS__)


//==========================================================================================================================================
//= Unique Values
//==========================================================================================================================================


struct ARRAY_METRICS_unique_s {
	void * A;
	long i_start;
	long i_end;
	int concurrent;
	void * ht;         // Hide the hashtable datatype
	long num_vals;
	double * vals;
	long * num_vals_out;
	double ** vals_out;
};

void ARRAY_METRICS_unique_init(int concurrent, struct ARRAY_METRICS_unique_s * s, void * A, long i_start, long i_end, double ** vals_out, long * num_vals_out, double (* get_val_as_double)(void * A, long i));
void ARRAY_METRICS_unique_process(struct ARRAY_METRICS_unique_s * s, long i, double (* get_val_as_double)(void * A, long i));
struct ARRAY_METRICS_unique_s ARRAY_METRICS_unique_combine(struct ARRAY_METRICS_unique_s s1, struct ARRAY_METRICS_unique_s s2);
void ARRAY_METRICS_unique_finalize(struct ARRAY_METRICS_unique_s * s);
void ARRAY_METRICS_unique_output(struct ARRAY_METRICS_unique_s * s_local, struct ARRAY_METRICS_unique_s * s_global);

void ARRAY_METRICS_unique_serial(void * A, long i_start, long i_end, double ** vals_out, long * num_vals_out, double (* get_val_as_double)(void * A, long i));
void ARRAY_METRICS_unique_concurrent(void * A, long i_start, long i_end, double ** vals_out, long * num_vals_out, double (* get_val_as_double)(void * A, long i));
void ARRAY_METRICS_unique(void * A, long i_start, long i_end, double ** vals_out, long * num_vals_out, double (* get_val_as_double)(void * A, long i));

#define array_seg_unique_serial(A, i_start, i_end, vals_out, num_vals_out, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_unique_serial(A, i_start, i_end, vals_out, num_vals_out, DEFAULT_ARG_1(gen_functor_convert_basic_type_to_double(A), ##__VA_ARGS__))

#define array_unique_serial(A, N, vals_out, num_vals_out, ... /* get_val_as_double() */)    \
	array_seg_unique_serial(A, 0, N, vals_out, num_vals_out, ##__VA_ARGS__)

#define array_seg_unique_concurrent(A, i_start, i_end, vals_out, num_vals_out, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_unique_concurrent(A, i_start, i_end, vals_out, num_vals_out, DEFAULT_ARG_1(gen_functor_convert_basic_type_to_double(A), ##__VA_ARGS__))

#define array_unique_concurrent(A, N, vals_out, num_vals_out, ... /* get_val_as_double() */)    \
	array_seg_unique_concurrent(A, 0, N, vals_out, num_vals_out, ##__VA_ARGS__)

#define array_seg_unique(A, i_start, i_end, vals_out, num_vals_out, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_unique(A, i_start, i_end, vals_out, num_vals_out, DEFAULT_ARG_1(gen_functor_convert_basic_type_to_double(A), ##__VA_ARGS__))

#define array_unique(A, N, vals_out, num_vals_out, ... /* get_val_as_double() */)    \
	array_seg_unique(A, 0, N, vals_out, num_vals_out, ##__VA_ARGS__)


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//------------------------------------------------------------------------------------------------------------------------------------------
//-                                                      Windowed Aggregate Metric                                                         -
//------------------------------------------------------------------------------------------------------------------------------------------
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


double ARRAY_METRICS_windowed_aggregate_serial(void * A, long i_start, long i_end, long window_len,
		double (* get_window_aggregate_as_double)(void * A, long i, long len),
		double (* reduce_fun)(double a, double b));
double ARRAY_METRICS_windowed_aggregate_concurrent(void * A, long i_start, long i_end, long window_len,
		double (* get_window_aggregate_as_double)(void * A, long i, long len),
		double (* reduce_fun)(double a, double b));
double ARRAY_METRICS_windowed_aggregate(void * A, long i_start, long i_end, long window_len,
		double (* get_window_aggregate_as_double)(void * A, long i, long len),
		double (* reduce_fun)(double a, double b));


#define array_seg_windowed_aggregate_serial(A, i_start, i_end, window_len, get_window_aggregate_as_double, reduce_fun)    \
	ARRAY_METRICS_windowed_aggregate_serial(A, i_start, i_end, window_len, get_window_aggregate_as_double, reduce_fun)

#define array_windowed_aggregate_serial(A, N, window_len, get_window_aggregate_as_double, reduce_fun)    \
	array_seg_windowed_aggregate_serial(A, 0, N, window_len, get_window_aggregate_as_double, reduce_fun)

#define array_seg_windowed_aggregate_concurrent(A, i_start, i_end, window_len, get_window_aggregate_as_double, reduce_fun)    \
	ARRAY_METRICS_windowed_aggregate_concurrent(A, i_start, i_end, DEFAULT_ARG_1(A, i_start, i_end, window_len, get_window_aggregate_as_double, reduce_fun))

#define array_windowed_aggregate_concurrent(A, N, window_len, get_window_aggregate_as_double, reduce_fun)    \
	array_seg_windowed_aggregate_concurrent(A, 0, N, window_len, get_window_aggregate_as_double, reduce_fun)

#define array_seg_windowed_aggregate(A, i_start, i_end, window_len, get_window_aggregate_as_double, reduce_fun)    \
	ARRAY_METRICS_windowed_aggregate(A, i_start, i_end, window_len, get_window_aggregate_as_double, reduce_fun)

#define array_windowed_aggregate(A, N, window_len, get_window_aggregate_as_double, reduce_fun)    \
	array_seg_windowed_aggregate(A, 0, N, window_len, get_window_aggregate_as_double, reduce_fun)


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//------------------------------------------------------------------------------------------------------------------------------------------
//-                                                       Closest Pair Distance                                                            -
//------------------------------------------------------------------------------------------------------------------------------------------
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


double ARRAY_METRICS_closest_pair_idx_serial(void * A, long i_start, long i_end, long * idx1_out, long * idx2_out, double (* get_val_as_double)(void * A, long i));
double ARRAY_METRICS_closest_pair_idx(void * A, long i_start, long i_end, long * idx1_out, long * idx2_out, double (* get_val_as_double)(void * A, long i));

#define array_seg_closest_pair_idx_serial(A, i_start, i_end, idx1_out, idx2_out, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_closest_pair_idx_serial(A, i_start, i_end, idx1_out, idx2_out, DEFAULT_ARG_1(gen_functor_convert_basic_type_to_double(A), ##__VA_ARGS__))

#define array_closest_pair_idx_serial(A, N, idx1_out, idx2_out, ... /* get_val_as_double() */)    \
	array_seg_closest_pair_idx_serial(A, 0, N, idx1_out, idx2_out, ##__VA_ARGS__)

#define array_seg_closest_pair_idx(A, i_start, i_end, idx1_out, idx2_out, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_closest_pair_idx(A, i_start, i_end, idx1_out, idx2_out, DEFAULT_ARG_1(gen_functor_convert_basic_type_to_double(A), ##__VA_ARGS__))

#define array_closest_pair_idx(A, N, idx1_out, idx2_out, ... /* get_val_as_double() */)    \
	array_seg_closest_pair_idx(A, 0, N, idx1_out, idx2_out, ##__VA_ARGS__)


#endif /* ARRAY_METRICS_H */

