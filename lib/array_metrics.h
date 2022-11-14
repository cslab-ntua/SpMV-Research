#ifndef ARRAY_METRICS_H
#define ARRAY_METRICS_H

#include "macros/cpp_defines.h"
#include "macros/macrolib.h"
#include "genlib.h"


//==========================================================================================================================================
//= Min - Max
//==========================================================================================================================================


void ARRAY_METRICS_min_max_idx_serial(void * A, long i_start, long i_end, long * min_idx_out, long * max_idx_out, double (* get_val_as_double)(void * A, long i));
void ARRAY_METRICS_min_max_idx(void * A, long i_start, long i_end, long * min_idx_out, long * max_idx_out, double (* get_val_as_double)(void * A, long i));
void ARRAY_METRICS_min_max_serial(void * A, long i_start, long i_end, double * min_out, double * max_out, double (* get_val_as_double)(void * A, long i));
void ARRAY_METRICS_min_max(void * A, long i_start, long i_end, double * min_out, double * max_out, double (* get_val_as_double)(void * A, long i));

#define array_seg_min_max_idx_serial(A, i_start, i_end, min_idx_out, max_idx_out, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_min_max_idx_serial(A, i_start, i_end, min_idx_out, max_idx_out, DEFAULT_ARG_1(gen_functor_basic_type_to_double(A), ##__VA_ARGS__))

#define array_seg_min_max_idx(A, i_start, i_end, min_idx_out, max_idx_out, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_min_max_idx(A, i_start, i_end, min_idx_out, max_idx_out, DEFAULT_ARG_1(gen_functor_basic_type_to_double(A), ##__VA_ARGS__))

#define array_min_max_idx_serial(A, N, min_idx_out, max_idx_out, ... /* get_val_as_double() */)    \
	array_seg_min_max_idx_serial(A, 0, N, min_idx_out, max_idx_out, ##__VA_ARGS__)

#define array_min_max_idx(A, N, min_idx_out, max_idx_out, ... /* get_val_as_double() */)    \
	array_seg_min_max_idx(A, 0, N, min_idx_out, max_idx_out, ##__VA_ARGS__)


#define array_seg_min_max_serial(A, i_start, i_end, min_out, max_out, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_min_max_serial(A, i_start, i_end, min_out, max_out, DEFAULT_ARG_1(gen_functor_basic_type_to_double(A), ##__VA_ARGS__))

#define array_seg_min_max(A, i_start, i_end, min_out, max_out, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_min_max(A, i_start, i_end, min_out, max_out, DEFAULT_ARG_1(gen_functor_basic_type_to_double(A), ##__VA_ARGS__))

#define array_min_max_serial(A, N, min_out, max_out, ... /* get_val_as_double() */)    \
	array_seg_min_max_serial(A, 0, N, min_out, max_out, ##__VA_ARGS__)

#define array_min_max(A, N, min_out, max_out, ... /* get_val_as_double() */)    \
	array_seg_min_max(A, 0, N, min_out, max_out, ##__VA_ARGS__)


//==========================================================================================================================================
//= Mean
//==========================================================================================================================================


double ARRAY_METRICS_mean_serial(void * A, long i_start, long i_end, double (* get_val_as_double)(void * A, long i));
double ARRAY_METRICS_mean(void * A, long i_start, long i_end, double (* get_val_as_double)(void * A, long i));

#define array_seg_mean_serial(A, i_start, i_end, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_mean_serial(A, i_start, i_end, DEFAULT_ARG_1(gen_functor_basic_type_to_double(A), ##__VA_ARGS__))

#define array_seg_mean(A, i_start, i_end, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_mean(A, i_start, i_end, DEFAULT_ARG_1(gen_functor_basic_type_to_double(A), ##__VA_ARGS__))

#define array_mean_serial(A, N, ... /* get_val_as_double() */)    \
	array_seg_mean_serial(A, 0, N, ##__VA_ARGS__)

#define array_mean(A, N, ... /* get_val_as_double() */)    \
	array_seg_mean(A, 0, N, ##__VA_ARGS__)


//==========================================================================================================================================
//= Variance - Standard Deviation
//==========================================================================================================================================


double ARRAY_METRICS_var_serial(void * A, long i_start, long i_end, double mean, double (* get_val_as_double)(void * A, long i));
double ARRAY_METRICS_var(void * A, long i_start, long i_end, double mean, double (* get_val_as_double)(void * A, long i));

#define array_seg_var_serial(A, i_start, i_end, mean, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_var_serial(A, i_start, i_end, mean, DEFAULT_ARG_1(gen_functor_basic_type_to_double(A), ##__VA_ARGS__))

#define array_seg_var(A, i_start, i_end, mean, ... /* get_val_as_double() */)    \
	ARRAY_METRICS_var(A, i_start, i_end, mean, DEFAULT_ARG_1(gen_functor_basic_type_to_double(A), ##__VA_ARGS__))

#define array_var_serial(A, N, mean, ... /* get_val_as_double() */)    \
	array_seg_var_serial(A, 0, N, mean, ##__VA_ARGS__)

#define array_var(A, N, mean, ... /* get_val_as_double() */)    \
	array_seg_var(A, 0, N, mean, ##__VA_ARGS__)


#define array_seg_std_serial(A, i_start, i_end, mean, ... /* get_val_as_double() */)    \
	sqrt(array_seg_var_serial(A, i_start, i_end, mean, ##__VA_ARGS__))

#define array_seg_std(A, i_start, i_end, mean, ... /* get_val_as_double() */)    \
	sqrt(array_seg_var(A, i_start, i_end, mean, ##__VA_ARGS__))

#define array_std_serial(A, N, mean, ... /* get_val_as_double() */)    \
	array_seg_std_serial(A, 0, N, mean, ##__VA_ARGS__)

#define array_std(A, N, mean, ... /* get_val_as_double() */)    \
	array_seg_std(A, 0, N, mean, ##__VA_ARGS__)


//==========================================================================================================================================
//= Closest Pair Distance
//==========================================================================================================================================


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

