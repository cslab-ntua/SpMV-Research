#ifndef _GNU_SOURCE
	#define _GNU_SOURCE
#endif
#include <stdlib.h>
#include <stdio.h>
#include <float.h>
#include <math.h>
#include <omp.h>

#include "macros/cpp_defines.h"
#include "macros/macrolib.h"
#include "debug.h"
#include "omp_functions.h"
#include "parallel_util.h"

#include "array_metrics.h"


/*
 * max >= rms >= mean >= gmean >= hmean >= min
 */


//==========================================================================================================================================
//= Reduce Functions
//==========================================================================================================================================


/* Inlining 'reduce_fun' seems to work.
 * Even if not inlining, the number of iterations is only equal to num_threads.
 *
 * omp_thread_reduce_global(_reduce_fun, _partial, _zero, _backwards, _local_result_ptr_ret, _total_result_ptr_ret);
 */


struct ARRAY_METRICS_ivp
ARRAY_METRICS_thread_reduce_ivp(struct ARRAY_METRICS_ivp (*reduce_fun)(struct ARRAY_METRICS_ivp, struct ARRAY_METRICS_ivp), struct ARRAY_METRICS_ivp partial, struct ARRAY_METRICS_ivp zero)
{
	struct ARRAY_METRICS_ivp total;
	omp_thread_reduce_global(reduce_fun, partial, zero, 0, NULL, &total);
	return total;
}

struct ARRAY_METRICS_2ivp
ARRAY_METRICS_thread_reduce_2ivp(struct ARRAY_METRICS_2ivp (*reduce_fun)(struct ARRAY_METRICS_2ivp, struct ARRAY_METRICS_2ivp), struct ARRAY_METRICS_2ivp partial, struct ARRAY_METRICS_2ivp zero)
{
	struct ARRAY_METRICS_2ivp total;
	omp_thread_reduce_global(reduce_fun, partial, zero, 0, NULL, &total);
	return total;
}

// Zero is NOT always 0.0 (e.g. if reduction is multiplication then zero == 1.0).
double
ARRAY_METRICS_thread_reduce_double(double (*reduce_fun)(double, double), double partial, double zero)
{
	double total;
	omp_thread_reduce_global(reduce_fun, partial, zero, 0, NULL, &total);
	return total;
}


//==========================================================================================================================================
//= Template Macros
//==========================================================================================================================================


#define metric_functions_template(_name, _type_internal, _type_output, _tupple_decl_args, _tupple_call_args)                        \
_type_output                                                                                                                        \
ARRAY_METRICS_ ## _name ## _serial(UNPACK(_tupple_decl_args))                                                                       \
{                                                                                                                                   \
	_type_internal _agg = ARRAY_METRICS_ ## _name ## _zero;                                                                     \
	long _i;                                                                                                                    \
	for (_i=i_start;_i<i_end;_i++)                                                                                              \
		_agg = ARRAY_METRICS_ ## _name ## _reduce(_agg, ARRAY_METRICS_ ## _name ## _map(_i, UNPACK(_tupple_call_args)));    \
	return ARRAY_METRICS_ ## _name ## _output(_agg, UNPACK(_tupple_call_args));                                                 \
}                                                                                                                                   \
_type_output                                                                                                                        \
ARRAY_METRICS_ ## _name ## _parallel(UNPACK(_tupple_decl_args))                                                                     \
{                                                                                                                                   \
	int _num_threads = safe_omp_get_num_threads();                                                                              \
	int _tnum = omp_get_thread_num();                                                                                           \
	_type_internal _agg = ARRAY_METRICS_ ## _name ## _zero;                                                                     \
	long _i, _i_s, _i_e;                                                                                                        \
	assert_omp_nesting_level(1);                                                                                                \
	loop_partitioner_balance_iterations(_num_threads, _tnum, i_start, i_end, &_i_s, &_i_e);                                     \
	for (_i=_i_s;_i<_i_e;_i++)                                                                                                  \
		_agg = ARRAY_METRICS_ ## _name ## _reduce(_agg, ARRAY_METRICS_ ## _name ## _map(_i, UNPACK(_tupple_call_args)));    \
	_agg = array_metrics_thread_reduce(ARRAY_METRICS_ ## _name ## _reduce, _agg, ARRAY_METRICS_ ## _name ## _zero);             \
	return ARRAY_METRICS_ ## _name ## _output(_agg, UNPACK(_tupple_call_args));                                                 \
}                                                                                                                                   \
_type_output                                                                                                                        \
ARRAY_METRICS_ ## _name(UNPACK(_tupple_decl_args))                                                                                  \
{                                                                                                                                   \
	_type_output _ret;                                                                                                          \
	if (omp_get_level() > 0)                                                                                                    \
		return ARRAY_METRICS_ ## _name ## _serial(UNPACK(_tupple_call_args));                                               \
	_Pragma("omp parallel")                                                                                                     \
	{                                                                                                                           \
		_type_output agg = ARRAY_METRICS_ ## _name ## _parallel(UNPACK(_tupple_call_args));                                 \
		_Pragma("omp single nowait")                                                                                        \
		_ret = agg;                                                                                                         \
	}                                                                                                                           \
	return _ret;                                                                                                                \
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//------------------------------------------------------------------------------------------------------------------------------------------
//-                                                               Norms                                                                    -
//------------------------------------------------------------------------------------------------------------------------------------------
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


//==========================================================================================================================================
//= P-Norm
//==========================================================================================================================================

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
inline
double
ARRAY_METRICS_pnorm_map(long i, void * A, long i_start, long i_end, double p, double (* get_val_as_double)(void * A, long i))
{
	if (p < 1)
		error("invalid p = %g, valid values: p >= 1", p);
	return pow(fabs(get_val_as_double(A, i)), p);
}

inline
double
ARRAY_METRICS_pnorm_reduce(double sum, double val)
{
	return sum + val;
}

inline
double
ARRAY_METRICS_pnorm_output(double agg, void * A, long i_start, long i_end, double p, double (* get_val_as_double)(void * A, long i))
{
	if (p < 1)
		error("invalid p = %g, valid values: p >= 1", p);
	return pow(agg, 1/p);
}
#pragma GCC diagnostic pop

metric_functions_template(pnorm, double, double,
	(void * A, long i_start, long i_end, double p, double (* get_val_as_double)(void * A, long i)),
	(A, i_start, i_end, p, get_val_as_double)
)


//==========================================================================================================================================
//= Min
//==========================================================================================================================================


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
inline
struct ARRAY_METRICS_ivp
ARRAY_METRICS_min_map(long i, void * A, long i_start, long i_end, long * min_idx_out, double (* get_val_as_double)(void * A, long i))
{
	struct ARRAY_METRICS_ivp pair = {.i=i, .val=get_val_as_double(A, i)};
	return pair;
}

inline
struct ARRAY_METRICS_ivp
ARRAY_METRICS_min_reduce(struct ARRAY_METRICS_ivp agg, struct ARRAY_METRICS_ivp val)
{
	return val.val < agg.val ? val : agg;
}

inline
double
ARRAY_METRICS_min_output(struct ARRAY_METRICS_ivp agg, void * A, long i_start, long i_end, long * min_idx_out, double (* get_val_as_double)(void * A, long i))
{
	if (min_idx_out != NULL)
		*min_idx_out = agg.i;
	return agg.val;
}
#pragma GCC diagnostic pop

metric_functions_template(min, struct ARRAY_METRICS_ivp, double,
	(void * A, long i_start, long i_end, long * min_idx_out, double (* get_val_as_double)(void * A, long i)),
	(A, i_start, i_end, min_idx_out, get_val_as_double)
)


//==========================================================================================================================================
//= Max
//==========================================================================================================================================


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
inline
struct ARRAY_METRICS_ivp
ARRAY_METRICS_max_map(long i, void * A, long i_start, long i_end, long * max_idx_out, double (* get_val_as_double)(void * A, long i))
{
	struct ARRAY_METRICS_ivp pair = {.i=i, .val=get_val_as_double(A, i)};
	return pair;
}

inline
struct ARRAY_METRICS_ivp
ARRAY_METRICS_max_reduce(struct ARRAY_METRICS_ivp agg, struct ARRAY_METRICS_ivp val)
{
	return val.val > agg.val ? val : agg;
}

inline
double
ARRAY_METRICS_max_output(struct ARRAY_METRICS_ivp agg, void * A, long i_start, long i_end, long * max_idx_out, double (* get_val_as_double)(void * A, long i))
{
	if (max_idx_out != NULL)
		*max_idx_out = agg.i;
	return agg.val;
}
#pragma GCC diagnostic pop

metric_functions_template(max, struct ARRAY_METRICS_ivp, double,
	(void * A, long i_start, long i_end, long * max_idx_out, double (* get_val_as_double)(void * A, long i)),
	(A, i_start, i_end, max_idx_out, get_val_as_double)
)


//==========================================================================================================================================
//= Min - Max
//==========================================================================================================================================


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
inline
struct ARRAY_METRICS_2ivp
ARRAY_METRICS_min_max_map(long i, void * A, long i_start, long i_end, double * min_out, long * min_idx_out, double * max_out, long * max_idx_out, double (* get_val_as_double)(void * A, long i))
{
	struct ARRAY_METRICS_2ivp pairs = {.i_1=i, .val_1=get_val_as_double(A, i), .i_2=i, .val_2=get_val_as_double(A, i)};
	return pairs;
}

inline
struct ARRAY_METRICS_2ivp
ARRAY_METRICS_min_max_reduce(struct ARRAY_METRICS_2ivp agg, struct ARRAY_METRICS_2ivp val)
{
	if (val.val_1 < agg.val_1)
	{
		agg.i_1 = val.i_1;
		agg.val_1 = val.val_1;
	}
	if (val.val_2 > agg.val_2)
	{
		agg.i_2 = val.i_2;
		agg.val_2 = val.val_2;
	}
	return agg;
}

inline
struct ARRAY_METRICS_2ivp
ARRAY_METRICS_min_max_output(struct ARRAY_METRICS_2ivp agg, void * A, long i_start, long i_end, double * min_out, long * min_idx_out, double * max_out, long * max_idx_out, double (* get_val_as_double)(void * A, long i))
{
	if (min_out != NULL)
		*min_out = agg.val_1;
	if (max_out != NULL)
		*max_out = agg.val_2;
	if (min_idx_out != NULL)
		*min_idx_out = agg.i_1;
	if (max_idx_out != NULL)
		*max_idx_out = agg.i_2;
	return agg;
}
#pragma GCC diagnostic pop

metric_functions_template(min_max, struct ARRAY_METRICS_2ivp, struct ARRAY_METRICS_2ivp,
	(void * A, long i_start, long i_end, double * min_out, long * min_idx_out, double * max_out, long * max_idx_out, double (* get_val_as_double)(void * A, long i)),
	(A, i_start, i_end, min_out, min_idx_out, max_out, max_idx_out, get_val_as_double)
)


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//------------------------------------------------------------------------------------------------------------------------------------------
//-                                                          Central Tendency                                                              -
//------------------------------------------------------------------------------------------------------------------------------------------
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


//==========================================================================================================================================
//= Mean
//==========================================================================================================================================


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
inline
double
ARRAY_METRICS_mean_map(long i, void * A, long i_start, long i_end, double (* get_val_as_double)(void * A, long i))
{
	return get_val_as_double(A, i);
}

inline
double
ARRAY_METRICS_mean_reduce(double sum, double val)
{
	return sum + val;
}

inline
double
ARRAY_METRICS_mean_output(double agg, void * A, long i_start, long i_end, double (* get_val_as_double)(void * A, long i))
{
	long N = i_end - i_start;
	return agg / N;
}
#pragma GCC diagnostic pop

metric_functions_template(mean, double, double,
	(void * A, long i_start, long i_end, double (* get_val_as_double)(void * A, long i)),
	(A, i_start, i_end, get_val_as_double)
)


//==========================================================================================================================================
//= Geometric Mean
//==========================================================================================================================================


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
inline
double
ARRAY_METRICS_gmean_map(long i, void * A, long i_start, long i_end, double (* get_val_as_double)(void * A, long i))
{
	return log(get_val_as_double(A, i));
}

inline
double
ARRAY_METRICS_gmean_reduce(double sum, double val)
{
	return sum + val;
}

inline
double
ARRAY_METRICS_gmean_output(double agg, void * A, long i_start, long i_end, double (* get_val_as_double)(void * A, long i))
{
	long N = i_end - i_start;
	return exp(agg / N);
}
#pragma GCC diagnostic pop

metric_functions_template(gmean, double, double,
	(void * A, long i_start, long i_end, double (* get_val_as_double)(void * A, long i)),
	(A, i_start, i_end, get_val_as_double)
)


//==========================================================================================================================================
//= Harmonic Mean
//==========================================================================================================================================


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
inline
double
ARRAY_METRICS_hmean_map(long i, void * A, long i_start, long i_end, double (* get_val_as_double)(void * A, long i))
{
	return 1 / get_val_as_double(A, i);
}

inline
double
ARRAY_METRICS_hmean_reduce(double sum, double val)
{
	return sum + val;
}

inline
double
ARRAY_METRICS_hmean_output(double agg, void * A, long i_start, long i_end, double (* get_val_as_double)(void * A, long i))
{
	long N = i_end - i_start;
	return 1 / (agg / N);
}
#pragma GCC diagnostic pop

metric_functions_template(hmean, double, double,
	(void * A, long i_start, long i_end, double (* get_val_as_double)(void * A, long i)),
	(A, i_start, i_end, get_val_as_double)
)


//==========================================================================================================================================
//= Root Mean Square
//==========================================================================================================================================


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
inline
double
ARRAY_METRICS_rms_map(long i, void * A, long i_start, long i_end, double (* get_val_as_double)(void * A, long i))
{
	double val = get_val_as_double(A, i);
	return val * val;
}

inline
double
ARRAY_METRICS_rms_reduce(double sum, double val)
{
	return sum + val;
}

inline
double
ARRAY_METRICS_rms_output(double agg, void * A, long i_start, long i_end, double (* get_val_as_double)(void * A, long i))
{
	long N = i_end - i_start;
	return sqrt(agg / N);
}
#pragma GCC diagnostic pop

metric_functions_template(rms, double, double,
	(void * A, long i_start, long i_end, double (* get_val_as_double)(void * A, long i)),
	(A, i_start, i_end, get_val_as_double)
)


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//------------------------------------------------------------------------------------------------------------------------------------------
//-                                                        Dispersion - Moments                                                            -
//------------------------------------------------------------------------------------------------------------------------------------------
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


//==========================================================================================================================================
//= Mean Absolute Deviation
//==========================================================================================================================================


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
inline
double
ARRAY_METRICS_mad_map(long i, void * A, long i_start, long i_end, double mean, double (* get_val_as_double)(void * A, long i))
{
	return fabs(get_val_as_double(A, i) - mean);
}

inline
double
ARRAY_METRICS_mad_reduce(double sum, double val)
{
	return sum + val;
}

inline
double
ARRAY_METRICS_mad_output(double agg, void * A, long i_start, long i_end, double mean, double (* get_val_as_double)(void * A, long i))
{
	long N = i_end - i_start;
	return agg / N;
}
#pragma GCC diagnostic pop

metric_functions_template(mad, double, double,
	(void * A, long i_start, long i_end, double mean, double (* get_val_as_double)(void * A, long i)),
	(A, i_start, i_end, mean, get_val_as_double)
)


//==========================================================================================================================================
//= Variance - Standard Deviation
//==========================================================================================================================================


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
inline
double
ARRAY_METRICS_var_map(long i, void * A, long i_start, long i_end, double mean, double (* get_val_as_double)(void * A, long i))
{
	double val = get_val_as_double(A, i) - mean;
	return val * val;
}

inline
double
ARRAY_METRICS_var_reduce(double sum, double val)
{
	return sum + val;
}

inline
double
ARRAY_METRICS_var_output(double agg, void * A, long i_start, long i_end, double mean, double (* get_val_as_double)(void * A, long i))
{
	long N = i_end - i_start;
	return agg / N;
}

inline
double
ARRAY_METRICS_std_map(long i, void * A, long i_start, long i_end, double mean, double (* get_val_as_double)(void * A, long i))
{
	return ARRAY_METRICS_var_map(i, A, i_start, i_end, mean, get_val_as_double);
}

inline
double
ARRAY_METRICS_std_reduce(double sum, double val)
{
	return ARRAY_METRICS_var_reduce(sum, val);
}

inline
double
ARRAY_METRICS_std_output(double agg, void * A, long i_start, long i_end, double mean, double (* get_val_as_double)(void * A, long i))
{
	return sqrt(ARRAY_METRICS_var_output(agg, A, i_start, i_end, mean, get_val_as_double));
}
#pragma GCC diagnostic pop

metric_functions_template(var, double, double,
	(void * A, long i_start, long i_end, double mean, double (* get_val_as_double)(void * A, long i)),
	(A, i_start, i_end, mean, get_val_as_double)
)


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


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
inline
double
ARRAY_METRICS_mae_map(long i, void * A, void * F, long i_start, long i_end, double (* get_val_as_double)(void * A, long i))
{
	return fabs(get_val_as_double(A, i) - get_val_as_double(F, i));
}

inline
double
ARRAY_METRICS_mae_reduce(double sum, double val)
{
	return sum + val;
}

inline
double
ARRAY_METRICS_mae_output(double agg, void * A, void * F, long i_start, long i_end, double (* get_val_as_double)(void * A, long i))
{
	long N = i_end - i_start;
	return agg / N;
}
#pragma GCC diagnostic pop

metric_functions_template(mae, double, double,
	(void * A, void * F, long i_start, long i_end, double (* get_val_as_double)(void * A, long i)),
	(A, F, i_start, i_end, get_val_as_double)
)


//==========================================================================================================================================
//= Max Absolute Error
//==========================================================================================================================================


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
inline
double
ARRAY_METRICS_max_ae_map(long i, void * A, void * F, long i_start, long i_end, double (* get_val_as_double)(void * A, long i))
{
	return fabs(get_val_as_double(A, i) - get_val_as_double(F, i));
}

inline
double
ARRAY_METRICS_max_ae_reduce(double sum, double val)
{
	return (val > sum) ? val : sum;
}

inline
double
ARRAY_METRICS_max_ae_output(double agg, void * A, void * F, long i_start, long i_end, double (* get_val_as_double)(void * A, long i))
{
	return agg;
}
#pragma GCC diagnostic pop

metric_functions_template(max_ae, double, double,
	(void * A, void * F, long i_start, long i_end, double (* get_val_as_double)(void * A, long i)),
	(A, F, i_start, i_end, get_val_as_double)
)


//==========================================================================================================================================
//= Mean Squared Error
//==========================================================================================================================================


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
inline
double
ARRAY_METRICS_mse_map(long i, void * A, void * F, long i_start, long i_end, double (* get_val_as_double)(void * A, long i))
{
	double val = get_val_as_double(A, i) - get_val_as_double(F, i);
	return val * val;
}

inline
double
ARRAY_METRICS_mse_reduce(double sum, double val)
{
	return val + sum;
}

inline
double
ARRAY_METRICS_mse_output(double agg, void * A, void * F, long i_start, long i_end, double (* get_val_as_double)(void * A, long i))
{
	long N = i_end - i_start;
	return 100 * agg / N;
}
#pragma GCC diagnostic pop

metric_functions_template(mse, double, double,
	(void * A, void * F, long i_start, long i_end, double (* get_val_as_double)(void * A, long i)),
	(A, F, i_start, i_end, get_val_as_double)
)


//==========================================================================================================================================
//= Mean Absolute Relative Error
//==========================================================================================================================================


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
inline
double
ARRAY_METRICS_mare_map(long i, void * A, void * F, long i_start, long i_end, double (* get_val_as_double)(void * A, long i))
{
	double a, f, ae;
	a = get_val_as_double(A, i);
	f = get_val_as_double(F, i);
	ae = fabs(a - f);
	if (ae <= 2 * DBL_EPSILON)
		return 0.0;
	return ae / fabs(a);
}

inline
double
ARRAY_METRICS_mare_reduce(double sum, double val)
{
	return val + sum;
}

inline
double
ARRAY_METRICS_mare_output(double agg, void * A, void * F, long i_start, long i_end, double (* get_val_as_double)(void * A, long i))
{
	long N = i_end - i_start;
	return agg / N;
}
#pragma GCC diagnostic pop

metric_functions_template(mare, double, double,
	(void * A, void * F, long i_start, long i_end, double (* get_val_as_double)(void * A, long i)),
	(A, F, i_start, i_end, get_val_as_double)
)


//==========================================================================================================================================
//= Mean Absolute Percentage Error
//==========================================================================================================================================


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
inline
double
ARRAY_METRICS_mape_map(long i, void * A, void * F, long i_start, long i_end, double (* get_val_as_double)(void * A, long i))
{
	return ARRAY_METRICS_mare_map(i, A, F, i_start, i_end, get_val_as_double);
}

inline
double
ARRAY_METRICS_mape_reduce(double sum, double val)
{
	return ARRAY_METRICS_mare_reduce(sum, val);
}

inline
double
ARRAY_METRICS_mape_output(double agg, void * A, void * F, long i_start, long i_end, double (* get_val_as_double)(void * A, long i))
{
	return 100 * ARRAY_METRICS_mare_output(agg, A, F, i_start, i_end, get_val_as_double);
}
#pragma GCC diagnostic pop

metric_functions_template(mape, double, double,
	(void * A, void * F, long i_start, long i_end, double (* get_val_as_double)(void * A, long i)),
	(A, F, i_start, i_end, get_val_as_double)
)


//==========================================================================================================================================
//= Symmetric Mean Absolute Relative Error
//==========================================================================================================================================


/* 0 <= smare <= 1
 */

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
inline
double
ARRAY_METRICS_smare_map(long i, void * A, void * F, long i_start, long i_end, double (* get_val_as_double)(void * A, long i))
{
	double a, f, ae;
	a = get_val_as_double(A, i);
	f = get_val_as_double(F, i);
	ae = fabs(a - f);
	if (ae <= 2 * DBL_EPSILON)
		return 0.0;
	return ae / (fabs(a) + fabs(f));
}

inline
double
ARRAY_METRICS_smare_reduce(double sum, double val)
{
	return val + sum;
}

inline
double
ARRAY_METRICS_smare_output(double agg, void * A, void * F, long i_start, long i_end, double (* get_val_as_double)(void * A, long i))
{
	long N = i_end - i_start;
	return agg / N;
}
#pragma GCC diagnostic pop

metric_functions_template(smare, double, double,
	(void * A, void * F, long i_start, long i_end, double (* get_val_as_double)(void * A, long i)),
	(A, F, i_start, i_end, get_val_as_double)
)


//==========================================================================================================================================
//= Symmetric Mean Absolute Percentage Error
//==========================================================================================================================================


/* 0 <= smape <= 100
 */

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
inline
double
ARRAY_METRICS_smape_map(long i, void * A, void * F, long i_start, long i_end, double (* get_val_as_double)(void * A, long i))
{
	return ARRAY_METRICS_smare_map(i, A, F, i_start, i_end, get_val_as_double);
}

inline
double
ARRAY_METRICS_smape_reduce(double sum, double val)
{
	return ARRAY_METRICS_smare_reduce(sum, val);
}

inline
double
ARRAY_METRICS_smape_output(double agg, void * A, void * F, long i_start, long i_end, double (* get_val_as_double)(void * A, long i))
{
	return 100 * ARRAY_METRICS_smare_output(agg, A, F, i_start, i_end, get_val_as_double);
}
#pragma GCC diagnostic pop

metric_functions_template(smape, double, double,
	(void * A, void * F, long i_start, long i_end, double (* get_val_as_double)(void * A, long i)),
	(A, F, i_start, i_end, get_val_as_double)
)


//==========================================================================================================================================
//= Q Error - Relative Error
//==========================================================================================================================================


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
inline
double
ARRAY_METRICS_Q_error_map(long i, void * A, void * F, long i_start, long i_end, double (* get_val_as_double)(void * A, long i))
{
	double a, f;
	a = get_val_as_double(A, i);
	f = get_val_as_double(F, i);
	return f / a;
}

inline
double
ARRAY_METRICS_Q_error_reduce(double sum, double val)
{
	return val + sum;
}

inline
double
ARRAY_METRICS_Q_error_output(double agg, void * A, void * F, long i_start, long i_end, double (* get_val_as_double)(void * A, long i))
{
	long N = i_end - i_start;
	return agg / N;
}
#pragma GCC diagnostic pop

metric_functions_template(Q_error, double, double,
	(void * A, void * F, long i_start, long i_end, double (* get_val_as_double)(void * A, long i)),
	(A, F, i_start, i_end, get_val_as_double)
)


//==========================================================================================================================================
//= Ln Q Error - Log Relative Error
//==========================================================================================================================================


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
inline
double
ARRAY_METRICS_lnQ_error_map(long i, void * A, void * F, long i_start, long i_end, double (* get_val_as_double)(void * A, long i))
{
	double a, f;
	a = get_val_as_double(A, i);
	f = get_val_as_double(F, i);
	return log(f) - log(a);
}

inline
double
ARRAY_METRICS_lnQ_error_reduce(double sum, double val)
{
	return val + sum;
}

inline
double
ARRAY_METRICS_lnQ_error_output(double agg, void * A, void * F, long i_start, long i_end, double (* get_val_as_double)(void * A, long i))
{
	long N = i_end - i_start;
	return agg / N;
}
#pragma GCC diagnostic pop

metric_functions_template(lnQ_error, double, double,
	(void * A, void * F, long i_start, long i_end, double (* get_val_as_double)(void * A, long i)),
	(A, F, i_start, i_end, get_val_as_double)
)


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//------------------------------------------------------------------------------------------------------------------------------------------
//-                                                       Closest Pair Distance                                                            -
//------------------------------------------------------------------------------------------------------------------------------------------
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


struct pair {
	double val;
	long idx;
};


double
ARRAY_METRICS_closest_pair_idx_serial(void * A, long i_start, long i_end, long * idx1_out, long * idx2_out, double (* get_val_as_double)(void * A, long i))
{
	long N = i_end - i_start;
	struct pair * P = (typeof(P)) malloc(N * sizeof(*P));
	double dist, min;
	long i, pos;
	if (N <= 1)
		error("closest pair can't be defined for arrays with less than 2 elements");
	for (i=0;i<N;i++)
	{
		P[i].val = get_val_as_double(A, i_start+i);
		P[i].idx = i;
	}
	int cmpfunc(const void * a, const void * b)
	{
		struct pair * p1 = (struct pair *) a;
		struct pair * p2 = (struct pair *) b;
		return (p1->val > p1->val) ? 1 : p1->val < p2->val ? -1 : 0;
	}
	qsort(P, N, sizeof(*P), cmpfunc);
	pos = 0;
	min = fabs(P[1].val - P[0].val);
	for (i=0;i<N-1;i++)
	{
		dist = fabs(P[i+1].val - P[i].val);
		if (dist < min)
		{
			min = dist;
			pos = i;
		}
	}
	if (idx1_out != NULL)
		*idx1_out = P[pos].idx;
	if (idx2_out != NULL)
		*idx2_out = P[pos+1].idx;
	dist = fabs(P[pos].idx - P[pos+1].idx);
	free(P);
	return dist;
}


#include "sort/samplesort_gen_undef.h"
#define SAMPLESORT_GEN_TYPE_1  struct pair
#define SAMPLESORT_GEN_TYPE_2  long
#define SAMPLESORT_GEN_TYPE_3  short
#define SAMPLESORT_GEN_TYPE_4  void
#define SAMPLESORT_GEN_SUFFIX  _d_l_s_v
#include "sort/samplesort_gen.c"

static inline
int
samplesort_cmp(struct pair p1, struct pair p2, __attribute__((unused)) void * unused)
{
	return (p1.val > p1.val) ? 1 : p1.val < p2.val ? -1 : 0;
}


double
ARRAY_METRICS_closest_pair_idx(void * A, long i_start, long i_end, long * idx1_out, long * idx2_out, double (* get_val_as_double)(void * A, long i))
{
	int num_threads = safe_omp_get_num_threads_external();
	long N = i_end - i_start;
	struct pair * P = (typeof(P)) malloc(N * sizeof(*P));
	double t_min[num_threads];
	long t_pos[num_threads];
	double dist, min;
	long i, pos;
	if (omp_get_level() > 0)
		return ARRAY_METRICS_closest_pair_idx_serial(A, i_start, i_end, idx1_out, idx2_out, get_val_as_double);
	if (N <= 1)
		error("closest pair can't be defined for arrays with less than 2 elements");
	#pragma omp parallel
	{
		long i;
		#pragma omp for
		for (i=0;i<N;i++)
		{
			P[i].val = get_val_as_double(A, i_start+i);
			P[i].idx = i;
		}
	}
	samplesort(P, N, NULL);
	#pragma omp parallel
	{
		int tnum = omp_get_thread_num();
		double dist, min;
		long i, pos;
		long i_s, i_e;
		loop_partitioner_balance_iterations(num_threads, tnum, 0, N-1, &i_s, &i_e);
		pos = i_s;
		min = fabs(P[i_s].val - P[i_s+1].val);
		for (i=i_s;i<i_e;i++)
		{
			dist = fabs(P[i+1].val - P[i].val);
			if (dist < min)
			{
				min = dist;
				pos = i;
			}
		}
		t_min[tnum] = min;
		t_pos[tnum] = pos;
	}
	min = t_min[0];
	pos = t_pos[0];
	for (i=1;i<num_threads;i++)
	{
		if (t_min[i] < min)
		{
			min = t_min[i];
			pos = t_pos[i];
		}
	}
	if (idx1_out != NULL)
		*idx1_out = P[pos].idx;
	if (idx2_out != NULL)
		*idx2_out = P[pos+1].idx;
	dist = fabs(P[pos].idx - P[pos+1].idx);
	free(P);
	return dist;
}

