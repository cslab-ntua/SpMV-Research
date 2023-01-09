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


static
double
thread_reduce_global_add(double partial)
{
	double ret;
	inline double add(double a, double b) { return a + b; }
	omp_thread_reduce_global(add, partial, 0.0, 0, NULL, &ret);
	return ret;
}

// static
// double
// thread_reduce_global_multiply(double partial)
// {
	// double ret;
	// inline double multiply(double a, double b) { return a * b; }
	// omp_thread_reduce_global(multiply, partial, 0.0, 0, NULL, &ret);
	// return ret;
// }

// static
// double
// thread_reduce_global_min(double partial)
// {
	// double ret;
	// inline double min(double a, double b) { return (a < b) ? a : b; }
	// omp_thread_reduce_global(min, partial, 0.0, 0, NULL, &ret);
	// return ret;
// }

static
double
thread_reduce_global_max(double partial)
{
	double ret;
	inline double max(double a, double b) { return (a > b) ? a : b; }
	omp_thread_reduce_global(max, partial, 0.0, 0, NULL, &ret);
	return ret;
}

static
long
thread_reduce_global_min_idx(void * A, long idx, double (* get_val_as_double)(void * A, long i))
{
	long ret;
	inline long min_idx(long idx1, long idx2)
	{
		double a, b;
		a = get_val_as_double(A, idx1);
		b = get_val_as_double(A, idx2);
		return (a < b) ? idx1 : idx2;
	}
	omp_thread_reduce_global(min_idx, idx, 0, 0, NULL, &ret);
	return ret;
}

static
long
thread_reduce_global_max_idx(void * A, long idx, double (* get_val_as_double)(void * A, long i))
{
	long ret;
	inline long max_idx(long idx1, long idx2)
	{
		double a, b;
		a = get_val_as_double(A, idx1);
		b = get_val_as_double(A, idx2);
		return (a > b) ? idx1 : idx2;
	}
	omp_thread_reduce_global(max_idx, idx, 0, 0, NULL, &ret);
	return ret;
}


//==========================================================================================================================================
//= P-Norm
//==========================================================================================================================================


static inline
double
pnorm_reduction(void * A, long i_start, long i_end, double p, double (* get_val_as_double)(void * A, long i))
{
	double sum = 0;
	long i;
	if (p < 1)
		error("invalid p = %g, valid values: p >= 1", p);
	for (i=i_start;i<i_end;i++)
		sum += pow(fabs(get_val_as_double(A, i)), p);
	return sum;
}

static inline
double
pnorm_output(double agg, double p)
{
	if (p < 1)
		error("invalid p = %g, valid values: p >= 1", p);
	return pow(agg, 1/p);
}

double
ARRAY_METRICS_pnorm_serial(void * A, long i_start, long i_end, double p, double (* get_val_as_double)(void * A, long i))
{
	double agg = pnorm_reduction(A, i_start, i_end, p, get_val_as_double);
	return pnorm_output(agg, p);
}

double
ARRAY_METRICS_pnorm_parallel(void * A, long i_start, long i_end, double p, double (* get_val_as_double)(void * A, long i))
{
	int num_threads = safe_omp_get_num_threads();
	int tnum = omp_get_thread_num();
	double agg;
	long i_s, i_e;
	assert_omp_nesting_level(1);
	loop_partitioner_balance_iterations(num_threads, tnum, i_start, i_end, &i_s, &i_e);
	agg = pnorm_reduction(A, i_s, i_e, p, get_val_as_double);
	agg = thread_reduce_global_add(agg);
	return pnorm_output(agg, p);
}

double
ARRAY_METRICS_pnorm(void * A, long i_start, long i_end, double p, double (* get_val_as_double)(void * A, long i))
{
	double ret = 0;
	if (omp_get_level() > 0)
		return ARRAY_METRICS_pnorm_serial(A, i_start, i_end, p, get_val_as_double);
	#pragma omp parallel
	{
		double agg = ARRAY_METRICS_pnorm_parallel(A, i_start, i_end, p, get_val_as_double);
		#pragma omp single nowait
		ret = agg;
	}
	return ret;
}


//==========================================================================================================================================
//= Min - Max
//==========================================================================================================================================


void
ARRAY_METRICS_min_max_idx_serial(void * A, long i_start, long i_end, long * min_idx_out, long * max_idx_out, double (* get_val_as_double)(void * A, long i))
{
	long min_idx, max_idx;
	double min, max;
	long i;
	double agg;
	min_idx = max_idx = 0;
	min = max = get_val_as_double(A, 0);
	for (i=i_start;i<i_end;i++)
	{
		agg = get_val_as_double(A, i);
		if (agg < min)
		{
			min_idx = i;
			min = agg;
		}
		if (agg > max)
		{
			max_idx = i;
			max = agg;
		}
	}
	arg_return(min_idx_out, min_idx);
	arg_return(max_idx_out, max_idx);
}

void
ARRAY_METRICS_min_max_idx_parallel(void * A, long i_start, long i_end, long * min_idx_out, long * max_idx_out, double (* get_val_as_double)(void * A, long i))
{
	int num_threads = safe_omp_get_num_threads();
	int tnum = omp_get_thread_num();
	long min_idx, max_idx;
	long i_s, i_e;
	assert_omp_nesting_level(1);
	loop_partitioner_balance_iterations(num_threads, tnum, i_start, i_end, &i_s, &i_e);
	ARRAY_METRICS_min_max_idx_serial(A, i_s, i_e, &min_idx, &max_idx, get_val_as_double);
	min_idx = thread_reduce_global_min_idx(A, min_idx, get_val_as_double);
	max_idx = thread_reduce_global_max_idx(A, max_idx, get_val_as_double);
	arg_return(min_idx_out, min_idx);
	arg_return(max_idx_out, max_idx);
}

void
ARRAY_METRICS_min_max_idx(void * A, long i_start, long i_end, long * min_idx_out, long * max_idx_out, double (* get_val_as_double)(void * A, long i))
{
	if (omp_get_level() > 0)
	{
		ARRAY_METRICS_min_max_idx_serial(A, i_start, i_end, min_idx_out, max_idx_out, get_val_as_double);
		return;
	}
	#pragma omp parallel
	{
		long t_min_idx, t_max_idx;
		ARRAY_METRICS_min_max_idx_parallel(A, i_start, i_end, &t_min_idx, &t_max_idx, get_val_as_double);
		#pragma omp single nowait
		{
			arg_return(min_idx_out, t_min_idx);
			arg_return(max_idx_out, t_max_idx);
		}
	}
}

void
ARRAY_METRICS_min_max_serial(void * A, long i_start, long i_end, double * min_out, double * max_out, double (* get_val_as_double)(void * A, long i))
{
	long min_idx, max_idx;
	ARRAY_METRICS_min_max_idx_serial(A, i_start, i_end, &min_idx, &max_idx, get_val_as_double);
	arg_return(min_out, get_val_as_double(A, min_idx));
	arg_return(max_out, get_val_as_double(A, max_idx));
}

void
ARRAY_METRICS_min_max_parallel(void * A, long i_start, long i_end, double * min_out, double * max_out, double (* get_val_as_double)(void * A, long i))
{
	long min_idx, max_idx;
	ARRAY_METRICS_min_max_idx_parallel(A, i_start, i_end, &min_idx, &max_idx, get_val_as_double);
	arg_return(min_out, get_val_as_double(A, min_idx));
	arg_return(max_out, get_val_as_double(A, max_idx));
}

void
ARRAY_METRICS_min_max(void * A, long i_start, long i_end, double * min_out, double * max_out, double (* get_val_as_double)(void * A, long i))
{
	long min_idx, max_idx;
	ARRAY_METRICS_min_max_idx(A, i_start, i_end, &min_idx, &max_idx, get_val_as_double);
	arg_return(min_out, get_val_as_double(A, min_idx));
	arg_return(max_out, get_val_as_double(A, max_idx));
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//------------------------------------------------------------------------------------------------------------------------------------------
//-                                                          Central Tendency                                                              -
//------------------------------------------------------------------------------------------------------------------------------------------
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


//==========================================================================================================================================
//= Mean
//==========================================================================================================================================


static inline
double
mean_reduction(void * A, long i_start, long i_end, double (* get_val_as_double)(void * A, long i))
{
	double sum = 0;
	long i;
	for (i=i_start;i<i_end;i++)
		sum += get_val_as_double(A, i);
	return sum;
}

static inline
double
mean_output(double agg, long N)
{
	return agg / N;
}

double
ARRAY_METRICS_mean_serial(void * A, long i_start, long i_end, double (* get_val_as_double)(void * A, long i))
{
	double agg = mean_reduction(A, i_start, i_end, get_val_as_double);
	return mean_output(agg, i_end - i_start);
}

double
ARRAY_METRICS_mean_parallel(void * A, long i_start, long i_end, double (* get_val_as_double)(void * A, long i))
{
	int num_threads = safe_omp_get_num_threads();
	int tnum = omp_get_thread_num();
	double agg;
	long i_s, i_e;
	assert_omp_nesting_level(1);
	loop_partitioner_balance_iterations(num_threads, tnum, i_start, i_end, &i_s, &i_e);
	agg = mean_reduction(A, i_s, i_e, get_val_as_double);
	agg = thread_reduce_global_add(agg);
	return mean_output(agg, i_end - i_start);
}

double
ARRAY_METRICS_mean(void * A, long i_start, long i_end, double (* get_val_as_double)(void * A, long i))
{
	double ret = 0;
	if (omp_get_level() > 0)
		return ARRAY_METRICS_mean_serial(A, i_start, i_end, get_val_as_double);
	#pragma omp parallel
	{
		double agg = ARRAY_METRICS_mean_parallel(A, i_start, i_end, get_val_as_double);
		#pragma omp single nowait
		ret = agg;
	}
	return ret;
}


//==========================================================================================================================================
//= Geometric Mean
//==========================================================================================================================================


static inline
double
gmean_reduction(void * A, long i_start, long i_end, double (* get_val_as_double)(void * A, long i))
{
	double sum = 0;
	long i;
	for (i=i_start;i<i_end;i++)
		sum += log(get_val_as_double(A, i));   // 'log()' is slower but a lot more stable than multiplication.
	return sum;
}

static inline
double
gmean_output(double agg, long N)
{
	return exp(agg / N);
}

double
ARRAY_METRICS_gmean_serial(void * A, long i_start, long i_end, double (* get_val_as_double)(void * A, long i))
{
	double agg = gmean_reduction(A, i_start, i_end, get_val_as_double);
	return gmean_output(agg, i_end - i_start);
}

double
ARRAY_METRICS_gmean_parallel(void * A, long i_start, long i_end, double (* get_val_as_double)(void * A, long i))
{
	int num_threads = safe_omp_get_num_threads();
	int tnum = omp_get_thread_num();
	double agg;
	long i_s, i_e;
	assert_omp_nesting_level(1);
	loop_partitioner_balance_iterations(num_threads, tnum, i_start, i_end, &i_s, &i_e);
	agg = gmean_reduction(A, i_s, i_e, get_val_as_double);
	agg = thread_reduce_global_add(agg);
	return gmean_output(agg, i_end - i_start);
}

double
ARRAY_METRICS_gmean(void * A, long i_start, long i_end, double (* get_val_as_double)(void * A, long i))
{
	double ret = 0;
	if (omp_get_level() > 0)
		return ARRAY_METRICS_gmean_serial(A, i_start, i_end, get_val_as_double);
	#pragma omp parallel
	{
		double agg = ARRAY_METRICS_gmean_parallel(A, i_start, i_end, get_val_as_double);
		#pragma omp single nowait
		ret = agg;
	}
	return ret;
}


//==========================================================================================================================================
//= Harmonic Mean
//==========================================================================================================================================


static inline
double
hmean_reduction(void * A, long i_start, long i_end, double (* get_val_as_double)(void * A, long i))
{
	double sum = 0;
	long i;
	for (i=i_start;i<i_end;i++)
		sum += 1 / get_val_as_double(A, i);
	return sum;
}

static inline
double
hmean_output(double agg, long N)
{
	return 1 / (agg / N);
}

double
ARRAY_METRICS_hmean_serial(void * A, long i_start, long i_end, double (* get_val_as_double)(void * A, long i))
{
	double agg = hmean_reduction(A, i_start, i_end, get_val_as_double);
	return hmean_output(agg, i_end - i_start);
}

double
ARRAY_METRICS_hmean_parallel(void * A, long i_start, long i_end, double (* get_val_as_double)(void * A, long i))
{
	int num_threads = safe_omp_get_num_threads();
	int tnum = omp_get_thread_num();
	double agg;
	long i_s, i_e;
	assert_omp_nesting_level(1);
	loop_partitioner_balance_iterations(num_threads, tnum, i_start, i_end, &i_s, &i_e);
	agg = hmean_reduction(A, i_s, i_e, get_val_as_double);
	agg = thread_reduce_global_add(agg);
	return hmean_output(agg, i_end - i_start);
}

double
ARRAY_METRICS_hmean(void * A, long i_start, long i_end, double (* get_val_as_double)(void * A, long i))
{
	double ret = 0;
	if (omp_get_level() > 0)
		return ARRAY_METRICS_hmean_serial(A, i_start, i_end, get_val_as_double);
	#pragma omp parallel
	{
		double agg = ARRAY_METRICS_hmean_parallel(A, i_start, i_end, get_val_as_double);
		#pragma omp single nowait
		ret = agg;
	}
	return ret;
}


//==========================================================================================================================================
//= Root Mean Square
//==========================================================================================================================================


static inline
double
rms_reduction(void * A, long i_start, long i_end, double (* get_val_as_double)(void * A, long i))
{
	double agg, sum = 0;
	long i;
	for (i=i_start;i<i_end;i++)
	{
		agg = get_val_as_double(A, i);
		sum += agg * agg;
	}
	return sum;
}

static inline
double
rms_output(double agg, long N)
{
	return sqrt(agg / N);
}

double
ARRAY_METRICS_rms_serial(void * A, long i_start, long i_end, double (* get_val_as_double)(void * A, long i))
{
	double agg = rms_reduction(A, i_start, i_end, get_val_as_double);
	return rms_output(agg, i_end - i_start);
}

double
ARRAY_METRICS_rms_parallel(void * A, long i_start, long i_end, double (* get_val_as_double)(void * A, long i))
{
	int num_threads = safe_omp_get_num_threads();
	int tnum = omp_get_thread_num();
	double agg;
	long i_s, i_e;
	assert_omp_nesting_level(1);
	loop_partitioner_balance_iterations(num_threads, tnum, i_start, i_end, &i_s, &i_e);
	agg = rms_reduction(A, i_s, i_e, get_val_as_double);
	agg = thread_reduce_global_add(agg);
	return rms_output(agg, i_end - i_start);
}

double
ARRAY_METRICS_rms(void * A, long i_start, long i_end, double (* get_val_as_double)(void * A, long i))
{
	double ret = 0;
	if (omp_get_level() > 0)
		return ARRAY_METRICS_rms_serial(A, i_start, i_end, get_val_as_double);
	#pragma omp parallel
	{
		double agg = ARRAY_METRICS_rms_parallel(A, i_start, i_end, get_val_as_double);
		#pragma omp single nowait
		ret = agg;
	}
	return ret;
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//------------------------------------------------------------------------------------------------------------------------------------------
//-                                                        Dispersion - Moments                                                            -
//------------------------------------------------------------------------------------------------------------------------------------------
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


//==========================================================================================================================================
//= Mean Absolute Deviation
//==========================================================================================================================================


static inline
double
mad_reduction(void * A, long i_start, long i_end, double mean, double (* get_val_as_double)(void * A, long i))
{
	double sum = 0;
	long i;
	for (i=i_start;i<i_end;i++)
		sum += fabs(get_val_as_double(A, i) - mean);
	return sum;
}

static inline
double
mad_output(double agg, long N)
{
	return agg / N;
}

double
ARRAY_METRICS_mad_serial(void * A, long i_start, long i_end, double mean, double (* get_val_as_double)(void * A, long i))
{
	double agg = mad_reduction(A, i_start, i_end, mean, get_val_as_double);
	return mad_output(agg, i_end - i_start);
}

double
ARRAY_METRICS_mad_parallel(void * A, long i_start, long i_end, double mean, double (* get_val_as_double)(void * A, long i))
{
	int num_threads = safe_omp_get_num_threads();
	int tnum = omp_get_thread_num();
	double agg;
	long i_s, i_e;
	assert_omp_nesting_level(1);
	loop_partitioner_balance_iterations(num_threads, tnum, i_start, i_end, &i_s, &i_e);
	agg = mad_reduction(A, i_s, i_e, mean, get_val_as_double);
	agg = thread_reduce_global_add(agg);
	return mad_output(agg, i_end - i_start);
}

double
ARRAY_METRICS_mad(void * A, long i_start, long i_end, double mean, double (* get_val_as_double)(void * A, long i))
{
	double ret = 0;
	if (omp_get_level() > 0)
		return ARRAY_METRICS_mad_serial(A, i_start, i_end, mean, get_val_as_double);
	#pragma omp parallel
	{
		double agg = ARRAY_METRICS_mad_parallel(A, i_start, i_end, mean, get_val_as_double);
		#pragma omp single nowait
		ret = agg;
	}
	return ret;
}


//==========================================================================================================================================
//= Variance - Standard Deviation
//==========================================================================================================================================


static inline
double
var_reduction(void * A, long i_start, long i_end, double mean, double (* get_val_as_double)(void * A, long i))
{
	double agg, sum = 0;
	long i;
	for (i=i_start;i<i_end;i++)
	{
		agg = get_val_as_double(A, i) - mean;
		sum += agg * agg;
	}
	return sum;
}

static inline
double
var_output(double agg, long N)
{
	return agg / N;
}

double
ARRAY_METRICS_var_serial(void * A, long i_start, long i_end, double mean, double (* get_val_as_double)(void * A, long i))
{
	double agg = var_reduction(A, i_start, i_end, mean, get_val_as_double);
	return var_output(agg, i_end - i_start);
}

double
ARRAY_METRICS_var_parallel(void * A, long i_start, long i_end, double mean, double (* get_val_as_double)(void * A, long i))
{
	int num_threads = safe_omp_get_num_threads();
	int tnum = omp_get_thread_num();
	double agg;
	long i_s, i_e;
	assert_omp_nesting_level(1);
	loop_partitioner_balance_iterations(num_threads, tnum, i_start, i_end, &i_s, &i_e);
	agg = var_reduction(A, i_s, i_e, mean, get_val_as_double);
	agg = thread_reduce_global_add(agg);
	return var_output(agg, i_end - i_start);
}

double
ARRAY_METRICS_var(void * A, long i_start, long i_end, double mean, double (* get_val_as_double)(void * A, long i))
{
	double ret = 0;
	if (omp_get_level() > 0)
		return ARRAY_METRICS_var_serial(A, i_start, i_end, mean, get_val_as_double);
	#pragma omp parallel
	{
		double agg = ARRAY_METRICS_var_parallel(A, i_start, i_end, mean, get_val_as_double);
		#pragma omp single nowait
		ret = agg;
	}
	return ret;
}


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


static inline
double
mae_reduction(void * A, void * F, long i_start, long i_end, double (* get_val_as_double)(void * A, long i))
{
	double sum = 0;
	long i;
	for (i=i_start;i<i_end;i++)
		sum += fabs(get_val_as_double(A, i) - get_val_as_double(F, i));
	return sum;
}

static inline
double
mae_output(double agg, long N)
{
	return agg / N;
}

double
ARRAY_METRICS_mae_serial(void * A, void * F, long i_start, long i_end, double (* get_val_as_double)(void * A, long i))
{
	double agg = mae_reduction(A, F, i_start, i_end, get_val_as_double);
	return mae_output(agg, i_end - i_start);
}

double
ARRAY_METRICS_mae_parallel(void * A, void * F, long i_start, long i_end, double (* get_val_as_double)(void * A, long i))
{
	int num_threads = safe_omp_get_num_threads();
	int tnum = omp_get_thread_num();
	double agg;
	long i_s, i_e;
	assert_omp_nesting_level(1);
	loop_partitioner_balance_iterations(num_threads, tnum, i_start, i_end, &i_s, &i_e);
	agg = mae_reduction(A, F, i_s, i_e, get_val_as_double);
	agg = thread_reduce_global_add(agg);
	return mae_output(agg, i_end - i_start);
}

double
ARRAY_METRICS_mae(void * A, void * F, long i_start, long i_end, double (* get_val_as_double)(void * A, long i))
{
	double ret;
	if (omp_get_level() > 0)
		return ARRAY_METRICS_mae_serial(A, F, i_start, i_end, get_val_as_double);
	#pragma omp parallel
	{
		double agg = ARRAY_METRICS_mae_parallel(A, F, i_start, i_end, get_val_as_double);
		#pragma omp single nowait
		ret = agg;
	}
	return ret;
}

//==========================================================================================================================================
//= Max Absolute Error
//==========================================================================================================================================


static inline
double
max_ae_reduction(void * A, void * F, long i_start, long i_end, double (* get_val_as_double)(void * A, long i))
{
	double agg, max = 0;
	long i;
	for (i=i_start;i<i_end;i++)
	{
		agg = fabs(get_val_as_double(A, i) - get_val_as_double(F, i));
		if (agg > max)
			max = agg;
	}
	return max;
}

static inline
double
max_ae_output(double agg)
{
	return agg;
}

double
ARRAY_METRICS_max_ae_serial(void * A, void * F, long i_start, long i_end, double (* get_val_as_double)(void * A, long i))
{
	double agg = max_ae_reduction(A, F, i_start, i_end, get_val_as_double);
	return max_ae_output(agg);
}

double
ARRAY_METRICS_max_ae_parallel(void * A, void * F, long i_start, long i_end, double (* get_val_as_double)(void * A, long i))
{
	int num_threads = safe_omp_get_num_threads();
	int tnum = omp_get_thread_num();
	double agg;
	long i_s, i_e;
	assert_omp_nesting_level(1);
	loop_partitioner_balance_iterations(num_threads, tnum, i_start, i_end, &i_s, &i_e);
	agg = max_ae_reduction(A, F, i_s, i_e, get_val_as_double);
	agg = thread_reduce_global_max(agg);
	return max_ae_output(agg);
}

double
ARRAY_METRICS_max_ae(void * A, void * F, long i_start, long i_end, double (* get_val_as_double)(void * A, long i))
{
	double ret;
	if (omp_get_level() > 0)
		return ARRAY_METRICS_max_ae_serial(A, F, i_start, i_end, get_val_as_double);
	#pragma omp parallel
	{
		double agg = ARRAY_METRICS_max_ae_parallel(A, F, i_start, i_end, get_val_as_double);
		#pragma omp single nowait
		ret = agg;
	}
	return ret;
}


//==========================================================================================================================================
//= Mean Squared Error
//==========================================================================================================================================


static inline
double
mse_reduction(void * A, void * F, long i_start, long i_end, double (* get_val_as_double)(void * A, long i))
{
	double a, f, sum = 0;
	long i;
	for (i=i_start;i<i_end;i++)
	{
		a = get_val_as_double(A, i);
		f = get_val_as_double(F, i);
		sum += (a - f) * (a - f);
	}
	return sum;
}

static inline
double
mse_output(double agg, long N)
{
	return 100.0 * agg / N;
}

double
ARRAY_METRICS_mse_serial(void * A, void * F, long i_start, long i_end, double (* get_val_as_double)(void * A, long i))
{
	double agg = mse_reduction(A, F, i_start, i_end, get_val_as_double);
	return mse_output(agg, i_end - i_start);
}

double
ARRAY_METRICS_mse_parallel(void * A, void * F, long i_start, long i_end, double (* get_val_as_double)(void * A, long i))
{
	int num_threads = safe_omp_get_num_threads();
	int tnum = omp_get_thread_num();
	double agg;
	long i_s, i_e;
	assert_omp_nesting_level(1);
	loop_partitioner_balance_iterations(num_threads, tnum, i_start, i_end, &i_s, &i_e);
	agg = mse_reduction(A, F, i_s, i_e, get_val_as_double);
	agg = thread_reduce_global_add(agg);
	return mse_output(agg, i_end - i_start);
}

double
ARRAY_METRICS_mse(void * A, void * F, long i_start, long i_end, double (* get_val_as_double)(void * A, long i))
{
	double ret;
	if (omp_get_level() > 0)
		return ARRAY_METRICS_mse_serial(A, F, i_start, i_end, get_val_as_double);
	#pragma omp parallel
	{
		double agg = ARRAY_METRICS_mse_parallel(A, F, i_start, i_end, get_val_as_double);
		#pragma omp single nowait
		ret = agg;
	}
	return ret;
}


//==========================================================================================================================================
//= Mean Absolute Relative Error
//==========================================================================================================================================


static inline
double
mare_reduction(void * A, void * F, long i_start, long i_end, double (* get_val_as_double)(void * A, long i))
{
	double a, f, ae, sum = 0;
	long i;
	for (i=i_start;i<i_end;i++)
	{
		a = get_val_as_double(A, i);
		f = get_val_as_double(F, i);
		ae = fabs(a - f);
		if (ae <= 2 * DBL_EPSILON)
			continue;
		sum += ae / fabs(a);
	}
	return sum;
}

static inline
double
mare_output(double agg, long N)
{
	return agg / N;
}

double
ARRAY_METRICS_mare_serial(void * A, void * F, long i_start, long i_end, double (* get_val_as_double)(void * A, long i))
{
	double agg = mare_reduction(A, F, i_start, i_end, get_val_as_double);
	return mare_output(agg, i_end - i_start);
}

double
ARRAY_METRICS_mare_parallel(void * A, void * F, long i_start, long i_end, double (* get_val_as_double)(void * A, long i))
{
	int num_threads = safe_omp_get_num_threads();
	int tnum = omp_get_thread_num();
	double agg;
	long i_s, i_e;
	assert_omp_nesting_level(1);
	loop_partitioner_balance_iterations(num_threads, tnum, i_start, i_end, &i_s, &i_e);
	agg = mare_reduction(A, F, i_s, i_e, get_val_as_double);
	agg = thread_reduce_global_add(agg);
	return mare_output(agg, i_end - i_start);
}

double
ARRAY_METRICS_mare(void * A, void * F, long i_start, long i_end, double (* get_val_as_double)(void * A, long i))
{
	double ret;
	if (omp_get_level() > 0)
		return ARRAY_METRICS_mare_serial(A, F, i_start, i_end, get_val_as_double);
	#pragma omp parallel
	{
		double agg = ARRAY_METRICS_mare_parallel(A, F, i_start, i_end, get_val_as_double);
		#pragma omp single nowait
		ret = agg;
	}
	return ret;
}


//==========================================================================================================================================
//= Mean Absolute Percentage Error
//==========================================================================================================================================


double
ARRAY_METRICS_mape_serial(void * A, void * F, long i_start, long i_end, double (* get_val_as_double)(void * A, long i))
{
	return 100 * ARRAY_METRICS_mare_serial(A, F, i_start, i_end, get_val_as_double);
}

double
ARRAY_METRICS_mape_parallel(void * A, void * F, long i_start, long i_end, double (* get_val_as_double)(void * A, long i))
{
	return 100 * ARRAY_METRICS_mare_parallel(A, F, i_start, i_end, get_val_as_double);
}

double
ARRAY_METRICS_mape(void * A, void * F, long i_start, long i_end, double (* get_val_as_double)(void * A, long i))
{
	return 100 * ARRAY_METRICS_mare(A, F, i_start, i_end, get_val_as_double);
}


//==========================================================================================================================================
//= Symmetric Mean Absolute Relative Error
//==========================================================================================================================================


/* 
 * 0 <= smare <= 1
 */

static inline
double
smare_reduction(void * A, void * F, long i_start, long i_end, double (* get_val_as_double)(void * A, long i))
{
	double a, f, ae, sum = 0;
	long i;
	for (i=i_start;i<i_end;i++)
	{
		a = get_val_as_double(A, i);
		f = get_val_as_double(F, i);
		ae = fabs(a - f);
		if (ae <= 2 * DBL_EPSILON)
			continue;
		sum += ae / (fabs(a) + fabs(f));
	}
	return sum;
}

static inline
double
smare_output(double agg, long N)
{
	return agg / N;
}

double
ARRAY_METRICS_smare_serial(void * A, void * F, long i_start, long i_end, double (* get_val_as_double)(void * A, long i))
{
	double agg = smare_reduction(A, F, i_start, i_end, get_val_as_double);
	return smare_output(agg, i_end - i_start);
}

double
ARRAY_METRICS_smare_parallel(void * A, void * F, long i_start, long i_end, double (* get_val_as_double)(void * A, long i))
{
	int num_threads = safe_omp_get_num_threads();
	int tnum = omp_get_thread_num();
	double agg;
	long i_s, i_e;
	assert_omp_nesting_level(1);
	loop_partitioner_balance_iterations(num_threads, tnum, i_start, i_end, &i_s, &i_e);
	agg = smare_reduction(A, F, i_s, i_e, get_val_as_double);
	agg = thread_reduce_global_add(agg);
	return smare_output(agg, i_end - i_start);
}

double
ARRAY_METRICS_smare(void * A, void * F, long i_start, long i_end, double (* get_val_as_double)(void * A, long i))
{
	double ret;
	if (omp_get_level() > 0)
		return ARRAY_METRICS_smare_serial(A, F, i_start, i_end, get_val_as_double);
	#pragma omp parallel
	{
		double agg = ARRAY_METRICS_smare_parallel(A, F, i_start, i_end, get_val_as_double);
		#pragma omp single nowait
		ret = agg;
	}
	return ret;
}


//==========================================================================================================================================
//= Symmetric Mean Absolute Percentage Error
//==========================================================================================================================================


/* 0 <= smape <= 100
 */

double
ARRAY_METRICS_smape_serial(void * A, void * F, long i_start, long i_end, double (* get_val_as_double)(void * A, long i))
{
	return 100 * ARRAY_METRICS_smare_serial(A, F, i_start, i_end, get_val_as_double);
}

double
ARRAY_METRICS_smape_parallel(void * A, void * F, long i_start, long i_end, double (* get_val_as_double)(void * A, long i))
{
	return 100 * ARRAY_METRICS_smare_parallel(A, F, i_start, i_end, get_val_as_double);
}

double
ARRAY_METRICS_smape(void * A, void * F, long i_start, long i_end, double (* get_val_as_double)(void * A, long i))
{
	return 100 * ARRAY_METRICS_smare(A, F, i_start, i_end, get_val_as_double);
}


//==========================================================================================================================================
//= Q Error - Relative Error
//==========================================================================================================================================


static inline
double
Q_error_reduction(void * A, void * F, long i_start, long i_end, double (* get_val_as_double)(void * A, long i))
{
	double a, f, sum = 0;
	long i;
	for (i=i_start;i<i_end;i++)
	{
		a = get_val_as_double(A, i);
		f = get_val_as_double(F, i);
		sum += f / a;
	}
	return sum;
}

static inline
double
Q_error_output(double agg, long N)
{
	return agg / N;
}

double
ARRAY_METRICS_Q_error_serial(void * A, void * F, long i_start, long i_end, double (* get_val_as_double)(void * A, long i))
{
	double agg = Q_error_reduction(A, F, i_start, i_end, get_val_as_double);
	return Q_error_output(agg, i_end - i_start);
}

double
ARRAY_METRICS_Q_error_parallel(void * A, void * F, long i_start, long i_end, double (* get_val_as_double)(void * A, long i))
{
	int num_threads = safe_omp_get_num_threads();
	int tnum = omp_get_thread_num();
	double agg;
	long i_s, i_e;
	assert_omp_nesting_level(1);
	loop_partitioner_balance_iterations(num_threads, tnum, i_start, i_end, &i_s, &i_e);
	agg = Q_error_reduction(A, F, i_s, i_e, get_val_as_double);
	agg = thread_reduce_global_add(agg);
	return Q_error_output(agg, i_end - i_start);
}

double
ARRAY_METRICS_Q_error(void * A, void * F, long i_start, long i_end, double (* get_val_as_double)(void * A, long i))
{
	double ret;
	if (omp_get_level() > 0)
		return ARRAY_METRICS_Q_error_serial(A, F, i_start, i_end, get_val_as_double);
	#pragma omp parallel
	{
		double agg = ARRAY_METRICS_Q_error_parallel(A, F, i_start, i_end, get_val_as_double);
		#pragma omp single nowait
		ret = agg;
	}
	return ret;
}


//==========================================================================================================================================
//= Ln Q Error - Log Relative Error
//==========================================================================================================================================


static inline
double
lnQ_error_reduction(void * A, void * F, long i_start, long i_end, double (* get_val_as_double)(void * A, long i))
{
	double a, f, sum = 0;
	long i;
	for (i=i_start;i<i_end;i++)
	{
		a = get_val_as_double(A, i);
		f = get_val_as_double(F, i);
		sum += log(f) - log(a);
	}
	return sum;
}

static inline
double
lnQ_error_output(double agg, long N)
{
	return agg / N;
}

double
ARRAY_METRICS_lnQ_error_serial(void * A, void * F, long i_start, long i_end, double (* get_val_as_double)(void * A, long i))
{
	double agg = lnQ_error_reduction(A, F, i_start, i_end, get_val_as_double);
	return lnQ_error_output(agg, i_end - i_start);
}

double
ARRAY_METRICS_lnQ_error_parallel(void * A, void * F, long i_start, long i_end, double (* get_val_as_double)(void * A, long i))
{
	int num_threads = safe_omp_get_num_threads();
	int tnum = omp_get_thread_num();
	double agg;
	long i_s, i_e;
	assert_omp_nesting_level(1);
	loop_partitioner_balance_iterations(num_threads, tnum, i_start, i_end, &i_s, &i_e);
	agg = lnQ_error_reduction(A, F, i_s, i_e, get_val_as_double);
	agg = thread_reduce_global_add(agg);
	return lnQ_error_output(agg, i_end - i_start);
}

double
ARRAY_METRICS_lnQ_error(void * A, void * F, long i_start, long i_end, double (* get_val_as_double)(void * A, long i))
{
	double ret;
	if (omp_get_level() > 0)
		return ARRAY_METRICS_lnQ_error_serial(A, F, i_start, i_end, get_val_as_double);
	#pragma omp parallel
	{
		double agg = ARRAY_METRICS_lnQ_error_parallel(A, F, i_start, i_end, get_val_as_double);
		#pragma omp single nowait
		ret = agg;
	}
	return ret;
}


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
	free(P);
	return fabs(P[pos].idx - P[pos+1].idx);
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

