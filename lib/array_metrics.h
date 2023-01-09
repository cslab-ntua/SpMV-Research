#ifndef ARRAY_METRICS_H
#define ARRAY_METRICS_H

#include "macros/cpp_defines.h"
#include "macros/macrolib.h"
#include "genlib.h"


//==========================================================================================================================================
//= P-Norm
//==========================================================================================================================================


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
//= Min - Max
//==========================================================================================================================================


void ARRAY_METRICS_min_max_idx_serial(void * A, long i_start, long i_end, long * min_idx_out, long * max_idx_out, double (* get_val_as_double)(void * A, long i));
void ARRAY_METRICS_min_max_idx_parallel(void * A, long i_start, long i_end, long * min_idx_out, long * max_idx_out, double (* get_val_as_double)(void * A, long i));
void ARRAY_METRICS_min_max_idx(void * A, long i_start, long i_end, long * min_idx_out, long * max_idx_out, double (* get_val_as_double)(void * A, long i));

#define array_seg_min_max_idx_serial(A, i_start, i_end, min_idx_out, max_idx_out, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_min_max_idx_serial(A, i_start, i_end, min_idx_out, max_idx_out, DEFAULT_ARG_1(gen_functor_basic_type_to_double(A), ##__VA_ARGS__))

#define array_min_max_idx_serial(A, N, min_idx_out, max_idx_out, ... /* get_val_as_double() */)    \
	array_seg_min_max_idx_serial(A, 0, N, min_idx_out, max_idx_out, ##__VA_ARGS__)

#define array_seg_min_max_idx_parallel(A, i_start, i_end, min_idx_out, max_idx_out, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_min_max_idx_parallel(A, i_start, i_end, min_idx_out, max_idx_out, DEFAULT_ARG_1(gen_functor_basic_type_to_double(A), ##__VA_ARGS__))

#define array_min_max_idx_parallel(A, N, min_idx_out, max_idx_out, ... /* get_val_as_double() */)    \
	array_seg_min_max_idx_parallel(A, 0, N, min_idx_out, max_idx_out, ##__VA_ARGS__)

#define array_seg_min_max_idx(A, i_start, i_end, min_idx_out, max_idx_out, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_min_max_idx(A, i_start, i_end, min_idx_out, max_idx_out, DEFAULT_ARG_1(gen_functor_basic_type_to_double(A), ##__VA_ARGS__))

#define array_min_max_idx(A, N, min_idx_out, max_idx_out, ... /* get_val_as_double() */)    \
	array_seg_min_max_idx(A, 0, N, min_idx_out, max_idx_out, ##__VA_ARGS__)


void ARRAY_METRICS_min_max_serial(void * A, long i_start, long i_end, double * min_out, double * max_out, double (* get_val_as_double)(void * A, long i));
void ARRAY_METRICS_min_max_parallel(void * A, long i_start, long i_end, double * min_out, double * max_out, double (* get_val_as_double)(void * A, long i));
void ARRAY_METRICS_min_max(void * A, long i_start, long i_end, double * min_out, double * max_out, double (* get_val_as_double)(void * A, long i));

#define array_seg_min_max_serial(A, i_start, i_end, min_out, max_out, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_min_max_serial(A, i_start, i_end, min_out, max_out, DEFAULT_ARG_1(gen_functor_basic_type_to_double(A), ##__VA_ARGS__))

#define array_min_max_serial(A, N, min_out, max_out, ... /* get_val_as_double() */)    \
	array_seg_min_max_serial(A, 0, N, min_out, max_out, ##__VA_ARGS__)

#define array_seg_min_max_parallel(A, i_start, i_end, min_out, max_out, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_min_max_parallel(A, i_start, i_end, min_out, max_out, DEFAULT_ARG_1(gen_functor_basic_type_to_double(A), ##__VA_ARGS__))

#define array_min_max_parallel(A, N, min_out, max_out, ... /* get_val_as_double() */)    \
	array_seg_min_max_parallel(A, 0, N, min_out, max_out, ##__VA_ARGS__)

#define array_seg_min_max(A, i_start, i_end, min_out, max_out, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_min_max(A, i_start, i_end, min_out, max_out, DEFAULT_ARG_1(gen_functor_basic_type_to_double(A), ##__VA_ARGS__))

#define array_min_max(A, N, min_out, max_out, ... /* get_val_as_double() */)    \
	array_seg_min_max(A, 0, N, min_out, max_out, ##__VA_ARGS__)


//==========================================================================================================================================
//= Mean
//==========================================================================================================================================


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


//==========================================================================================================================================
//= Mean Absolute Deviation
//==========================================================================================================================================


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


double ARRAY_METRICS_var_serial(void * A, long i_start, long i_end, double mean, double (* get_val_as_double)(void * A, long i));
double ARRAY_METRICS_var_parallel(void * A, long i_start, long i_end, double mean, double (* get_val_as_double)(void * A, long i));
double ARRAY_METRICS_var(void * A, long i_start, long i_end, double mean, double (* get_val_as_double)(void * A, long i));

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
//= Mean Absolute Relative Error
//==========================================================================================================================================


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
//= Mean Squared Error
//==========================================================================================================================================


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
//= Q Error - Relative Error
//==========================================================================================================================================


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

