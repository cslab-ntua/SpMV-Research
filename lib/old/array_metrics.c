#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <float.h>
#include <math.h>
#include <omp.h>

#include "macros/cpp_defines.h"
#include "macros/macrolib.h"
#include "debug.h"
#include "omp_functions.h"
#include "parallel_util.h"
#include "plot/plot.h"

#include "array_metrics.h"

// Quicksort
#include "sort/quicksort/quicksort_gen_undef.h"
#define QUICKSORT_GEN_TYPE_1  double
#define QUICKSORT_GEN_TYPE_2  long
#define QUICKSORT_GEN_TYPE_3  void
#define QUICKSORT_GEN_SUFFIX  _d_l_v
#include "sort/quicksort/quicksort_gen.c"
static inline
int
quicksort_cmp(double a, double b, __attribute__((unused)) void * unused)
{
	return (a > b) ? 1 : (a < b) ? -1 : 0;
}

// Samplesort
#include "sort/samplesort/samplesort_gen_undef.h"
#define SAMPLESORT_GEN_TYPE_1  double
#define SAMPLESORT_GEN_TYPE_2  long
#define SAMPLESORT_GEN_TYPE_3  short
#define SAMPLESORT_GEN_TYPE_4  void
#define SAMPLESORT_GEN_SUFFIX  _d_l_s_v
#include "sort/samplesort/samplesort_gen.c"
static inline
int
samplesort_cmp(double a, double b, __attribute__((unused)) void * unused)
{
	return (a > b) ? 1 : (a < b) ? -1 : 0;
}


/*
 * max >= rms >= mean >= gmean >= hmean >= min
 */


//==========================================================================================================================================
//= Functions Templates Macros
//==========================================================================================================================================


#define metric_functions_templates(_name, _i_start, _i_end, _get_val_as_double, _tupple_decl_args, _tupple_call_args)                      \
                                                                                                                                           \
	void                                                                                                                               \
	ARRAY_METRICS_ ## _name ## _serial(UNPACK(_tupple_decl_args))                                                                      \
	{                                                                                                                                  \
		struct ARRAY_METRICS_ ## _name ## _s _s_global;                                                                            \
		long _i;                                                                                                                   \
		ARRAY_METRICS_ ## _name ## _init(0, &_s_global, UNPACK(_tupple_call_args));                                                \
		for (_i=_i_start;_i<_i_end;_i++)                                                                                           \
			ARRAY_METRICS_ ## _name ## _process(&_s_global, _i, _get_val_as_double);                                           \
		ARRAY_METRICS_ ## _name ## _finalize(&_s_global);                                                                          \
		ARRAY_METRICS_ ## _name ## _output(&_s_global, &_s_global);                                                                \
	}                                                                                                                                  \
                                                                                                                                           \
	static                                                                                                                             \
	void                                                                                                                               \
	ARRAY_METRICS_ ## _name ## _concurrent_base(                                                                                       \
			struct ARRAY_METRICS_ ## _name ## _s * _s_local_ptr,                                                               \
			struct ARRAY_METRICS_ ## _name ## _s * _s_global_ptr,                                                              \
			UNPACK(_tupple_decl_args))                                                                                         \
	{                                                                                                                                  \
		int _num_threads = safe_omp_get_num_threads();                                                                             \
		int _tnum = omp_get_thread_num();                                                                                          \
		long _i, _i_s, _i_e;                                                                                                       \
		assert_omp_nesting_level(1);                                                                                               \
		loop_partitioner_balance_iterations(_num_threads, _tnum, _i_start, _i_end, &_i_s, &_i_e);                                  \
		ARRAY_METRICS_ ## _name ## _init(1, _s_local_ptr, UNPACK(_tupple_call_args));                                              \
		for (_i=_i_s;_i<_i_e;_i++)                                                                                                 \
			ARRAY_METRICS_ ## _name ## _process(_s_local_ptr, _i, _get_val_as_double);                                         \
		 /* zero is unused for total, so we just pass _s_local_ptr*/                                                               \
		omp_thread_reduce_global(ARRAY_METRICS_ ## _name ## _combine, *_s_local_ptr, *_s_local_ptr, 1, 0, NULL, _s_global_ptr);    \
		ARRAY_METRICS_ ## _name ## _finalize(_s_global_ptr);                                                                       \
	}                                                                                                                                  \
                                                                                                                                           \
	void                                                                                                                               \
	ARRAY_METRICS_ ## _name ## _concurrent(UNPACK(_tupple_decl_args))                                                                  \
	{                                                                                                                                  \
		struct ARRAY_METRICS_ ## _name ## _s _s_local;                                                                             \
		struct ARRAY_METRICS_ ## _name ## _s _s_global;                                                                            \
		ARRAY_METRICS_ ## _name ## _concurrent_base(&_s_local, &_s_global, UNPACK(_tupple_call_args));                             \
		ARRAY_METRICS_ ## _name ## _output(&_s_local, &_s_global);                                                                 \
	}                                                                                                                                  \
                                                                                                                                           \
	void                                                                                                                               \
	ARRAY_METRICS_ ## _name(UNPACK(_tupple_decl_args))                                                                                 \
	{                                                                                                                                  \
		if (omp_get_level() > 0)                                                                                                   \
		{                                                                                                                          \
			ARRAY_METRICS_ ## _name ## _serial(UNPACK(_tupple_call_args));                                                     \
		}                                                                                                                          \
		else                                                                                                                       \
		{                                                                                                                          \
			_Pragma("omp parallel")                                                                                            \
			{                                                                                                                  \
				struct ARRAY_METRICS_ ## _name ## _s _s_local;                                                             \
				struct ARRAY_METRICS_ ## _name ## _s _s_global;                                                            \
				ARRAY_METRICS_ ## _name ## _concurrent_base(&_s_local, &_s_global, UNPACK(_tupple_call_args));             \
				_Pragma("omp single nowait")                                                                               \
				{                                                                                                          \
					ARRAY_METRICS_ ## _name ## _output(&_s_global, &_s_global);                                        \
				}                                                                                                          \
			}                                                                                                                  \
		}                                                                                                                          \
	}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//------------------------------------------------------------------------------------------------------------------------------------------
//-                                                              Subsets                                                                   -
//------------------------------------------------------------------------------------------------------------------------------------------
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


//==========================================================================================================================================
//= Greater Than
//==========================================================================================================================================


#if 0
inline
void
ARRAY_METRICS_greater_than_init(__attribute__((unused)) int concurrent, struct ARRAY_METRICS_greater_than_s * s, void * A, long i_start, long i_end, double p, double * result_out, __attribute__((unused)) double (* get_val_as_double)(void * A, long i))
{
	if (result_out == NULL)
		error("'result_out' return parameter is NULL");
	s->A = A;
	s->i_start = i_start;
	s->i_end = i_end;
	s->p = p;
	s->partial = 0;
	s->result_out = result_out;
}

inline
void
ARRAY_METRICS_greater_than_process(struct ARRAY_METRICS_greater_than_s * s, long i, double (* get_val_as_double)(void * A, long i))
{
	if (s->p < 1)
		error("invalid p = %g, valid values: p >= 1", s->p);
	s->partial += pow(fabs(get_val_as_double(s->A, i)), s->p);
}

inline
struct ARRAY_METRICS_greater_than_s
ARRAY_METRICS_greater_than_combine(struct ARRAY_METRICS_greater_than_s s1, struct ARRAY_METRICS_greater_than_s s2)
{
	struct ARRAY_METRICS_greater_than_s s_ret;
	s_ret = s1;
	s_ret.partial = s1.partial + s2.partial;
	return s_ret;
}

inline
void
ARRAY_METRICS_greater_than_finalize(__attribute__((unused)) struct ARRAY_METRICS_greater_than_s * s)
{
}

inline
void
ARRAY_METRICS_greater_than_output(struct ARRAY_METRICS_greater_than_s * s_local, struct ARRAY_METRICS_greater_than_s * s_global)
{
	if (s->p < 1)
		error("invalid p = %g, valid values: p >= 1", s->p);
	*s->result_out = pow(s->partial, 1/s->p);
}

metric_functions_templates(greater_than, i_start, i_end, get_val_as_double,
	(void * A, long i_start, long i_end, double p, double * result_out, double (* get_val_as_double)(void * A, long i)),
	(A, i_start, i_end, p, result_out, get_val_as_double)
)
#endif


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//------------------------------------------------------------------------------------------------------------------------------------------
//-                                                               Norms                                                                    -
//------------------------------------------------------------------------------------------------------------------------------------------
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


//==========================================================================================================================================
//= P-Norm
//==========================================================================================================================================


inline
void
ARRAY_METRICS_pnorm_init(__attribute__((unused)) int concurrent, struct ARRAY_METRICS_pnorm_s * s, void * A, long i_start, long i_end, double p, double * result_out, __attribute__((unused)) double (* get_val_as_double)(void * A, long i))
{
	if (result_out == NULL)
		error("'result_out' return parameter is NULL");
	s->A = A;
	s->i_start = i_start;
	s->i_end = i_end;
	s->p = p;
	s->partial = 0;
	s->result_out = result_out;
}

inline
void
ARRAY_METRICS_pnorm_process(struct ARRAY_METRICS_pnorm_s * s, long i, double (* get_val_as_double)(void * A, long i))
{
	if (s->p < 1)
		error("invalid p = %g, valid values: p >= 1", s->p);
	s->partial += pow(fabs(get_val_as_double(s->A, i)), s->p);
}

inline
struct ARRAY_METRICS_pnorm_s
ARRAY_METRICS_pnorm_combine(struct ARRAY_METRICS_pnorm_s s1, struct ARRAY_METRICS_pnorm_s s2)
{
	struct ARRAY_METRICS_pnorm_s s_ret;
	s_ret = s1;
	s_ret.partial = s1.partial + s2.partial;
	return s_ret;
}

inline
void
ARRAY_METRICS_pnorm_finalize(__attribute__((unused)) struct ARRAY_METRICS_pnorm_s * s)
{
}

inline
void
ARRAY_METRICS_pnorm_output(struct ARRAY_METRICS_pnorm_s * s_local, struct ARRAY_METRICS_pnorm_s * s_global)
{
	if (s_global->p < 1)
		error("invalid p = %g, valid values: p >= 1", s_global->p);
	*s_local->result_out = pow(s_global->partial, 1/s_global->p);
}

metric_functions_templates(pnorm, i_start, i_end, get_val_as_double,
	(void * A, long i_start, long i_end, double p, double * result_out, double (* get_val_as_double)(void * A, long i)),
	(A, i_start, i_end, p, result_out, get_val_as_double)
)


//==========================================================================================================================================
//= Min
//==========================================================================================================================================


inline
void
ARRAY_METRICS_min_init(__attribute__((unused)) int concurrent, struct ARRAY_METRICS_min_s * s, void * A, long i_start, long i_end, double * min_out, long * min_idx_out, __attribute__((unused)) double (* get_val_as_double)(void * A, long i))
{
	if (min_out == NULL && min_idx_out == NULL)
		error("All return parameters are NULL");
	s->A = A;
	s->i_start = i_start;
	s->i_end = i_end;
	s->min = get_val_as_double(s->A, 0);
	s->min_idx = 0;
	s->min_out = min_out;
	s->min_idx_out = min_idx_out;
}

inline
void
ARRAY_METRICS_min_process(struct ARRAY_METRICS_min_s * s, long i, double (* get_val_as_double)(void * A, long i))
{
	double val = get_val_as_double(s->A, i);
	if (val < s->min)
	{
		s->min = val;
		s->min_idx = i;
	}
}

inline
struct ARRAY_METRICS_min_s
ARRAY_METRICS_min_combine(struct ARRAY_METRICS_min_s s1, struct ARRAY_METRICS_min_s s2)
{
	struct ARRAY_METRICS_min_s s_ret;
	s_ret = s1;
	if (s2.min < s_ret.min)
	{
		s_ret.min = s2.min;
		s_ret.min_idx = s2.min_idx;
	}
	return s_ret;
}

inline
void
ARRAY_METRICS_min_finalize(__attribute__((unused)) struct ARRAY_METRICS_min_s * s)
{
}


inline
void
ARRAY_METRICS_min_output(struct ARRAY_METRICS_min_s * s_local, struct ARRAY_METRICS_min_s * s_global)
{
	if (s_local->min_out != NULL)
		*s_local->min_out = s_global->min;
	if (s_local->min_idx_out != NULL)
		*s_local->min_idx_out = s_global->min_idx;
}

metric_functions_templates(min, i_start, i_end, get_val_as_double,
	(void * A, long i_start, long i_end, double * min_out, long * min_idx_out, double (* get_val_as_double)(void * A, long i)),
	(A, i_start, i_end, min_out, min_idx_out, get_val_as_double)
)


//==========================================================================================================================================
//= Max
//==========================================================================================================================================


inline
void
ARRAY_METRICS_max_init(__attribute__((unused)) int concurrent, struct ARRAY_METRICS_max_s * s, void * A, long i_start, long i_end, double * max_out, long * max_idx_out, __attribute__((unused)) double (* get_val_as_double)(void * A, long i))
{
	if (max_out == NULL && max_idx_out == NULL)
		error("All return parameters are NULL");
	s->A = A;
	s->i_start = i_start;
	s->i_end = i_end;
	s->max = get_val_as_double(s->A, 0);
	s->max_idx = 0;
	s->max_out = max_out;
	s->max_idx_out = max_idx_out;
}

inline
void
ARRAY_METRICS_max_process(struct ARRAY_METRICS_max_s * s, long i, double (* get_val_as_double)(void * A, long i))
{
	double val = get_val_as_double(s->A, i);
	if (val > s->max)
	{
		s->max = val;
		s->max_idx = i;
	}
}

inline
struct ARRAY_METRICS_max_s
ARRAY_METRICS_max_combine(struct ARRAY_METRICS_max_s s1, struct ARRAY_METRICS_max_s s2)
{
	struct ARRAY_METRICS_max_s s_ret;
	s_ret = s1;
	if (s2.max > s_ret.max)
	{
		s_ret.max = s2.max;
		s_ret.max_idx = s2.max_idx;
	}
	return s_ret;
}

inline
void
ARRAY_METRICS_max_finalize(__attribute__((unused)) struct ARRAY_METRICS_max_s * s)
{
}


inline
void
ARRAY_METRICS_max_output(struct ARRAY_METRICS_max_s * s_local, struct ARRAY_METRICS_max_s * s_global)
{
	if (s_local->max_out != NULL)
		*s_local->max_out = s_global->max;
	if (s_local->max_idx_out != NULL)
		*s_local->max_idx_out = s_global->max_idx;
}

metric_functions_templates(max, i_start, i_end, get_val_as_double,
	(void * A, long i_start, long i_end, double * max_out, long * max_idx_out, double (* get_val_as_double)(void * A, long i)),
	(A, i_start, i_end, max_out, max_idx_out, get_val_as_double)
)


//==========================================================================================================================================
//= Min - Max
//==========================================================================================================================================


inline
void
ARRAY_METRICS_min_max_init(__attribute__((unused)) int concurrent, struct ARRAY_METRICS_min_max_s * s, void * A, long i_start, long i_end, double * min_out, long * min_idx_out, double * max_out, long * max_idx_out, __attribute__((unused)) double (* get_val_as_double)(void * A, long i))
{
	if (max_out == NULL && max_idx_out == NULL && min_out == NULL && min_idx_out == NULL)
		error("All return parameters are NULL");
	s->A = A;
	s->i_start = i_start;
	s->i_end = i_end;
	s->min = get_val_as_double(s->A, 0);
	s->min_idx = 0;
	s->min_out = min_out;
	s->min_idx_out = min_idx_out;
	s->max = get_val_as_double(s->A, 0);
	s->max_idx = 0;
	s->max_out = max_out;
	s->max_idx_out = max_idx_out;
}

inline
void
ARRAY_METRICS_min_max_process(struct ARRAY_METRICS_min_max_s * s, long i, double (* get_val_as_double)(void * A, long i))
{
	double val = get_val_as_double(s->A, i);
	if (val < s->min)
	{
		s->min= val;
		s->min_idx = i;
	}
	else if (val > s->max)
	{
		s->max = val;
		s->max_idx = i;
	}
}

inline
struct ARRAY_METRICS_min_max_s
ARRAY_METRICS_min_max_combine(struct ARRAY_METRICS_min_max_s s1, struct ARRAY_METRICS_min_max_s s2)
{
	struct ARRAY_METRICS_min_max_s s_ret;
	s_ret = s1;
	if (s2.min < s_ret.min)
	{
		s_ret.min = s2.min;
		s_ret.min_idx = s2.min_idx;
	}
	else if (s2.max > s_ret.max)
	{
		s_ret.max = s2.max;
		s_ret.max_idx = s2.max_idx;
	}
	return s_ret;
}

inline
void
ARRAY_METRICS_min_max_finalize(__attribute__((unused)) struct ARRAY_METRICS_min_max_s * s)
{
}


inline
void
ARRAY_METRICS_min_max_output(struct ARRAY_METRICS_min_max_s * s_local, struct ARRAY_METRICS_min_max_s * s_global)
{
	if (s_local->min_out != NULL)
		*s_local->min_out = s_global->min;
	if (s_local->min_idx_out != NULL)
		*s_local->min_idx_out = s_global->min_idx;
	if (s_local->max_out != NULL)
		*s_local->max_out = s_global->max;
	if (s_local->max_idx_out != NULL)
		*s_local->max_idx_out = s_global->max_idx;
}

metric_functions_templates(min_max, i_start, i_end, get_val_as_double,
	(void * A, long i_start, long i_end, double * min_out, long * min_idx_out, double * max_out, long * max_idx_out, double (* get_val_as_double)(void * A, long i)),
	(A, i_start, i_end, min_out, min_idx_out, max_out, max_idx_out, get_val_as_double)
)


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//------------------------------------------------------------------------------------------------------------------------------------------
//-                                                               Moments                                                                  -
//------------------------------------------------------------------------------------------------------------------------------------------
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


//==========================================================================================================================================
//= Moments
//==========================================================================================================================================


inline
void
ARRAY_METRICS_moment_init(__attribute__((unused)) int concurrent, struct ARRAY_METRICS_moment_s * s, void * A, long i_start, long i_end, double n, double center, double * result_out, __attribute__((unused)) double (* get_val_as_double)(void * A, long i))
{
	if (result_out == NULL)
		error("'result_out' return parameter is NULL");
	s->A = A;
	s->i_start = i_start;
	s->i_end = i_end;
	s->n = n;
	s->center = center;
	s->partial = 0;
	s->result_out = result_out;
}

inline
void
ARRAY_METRICS_moment_process(struct ARRAY_METRICS_moment_s * s, long i, double (* get_val_as_double)(void * A, long i))
{
	double val = get_val_as_double(s->A, i) - s->center;
	double res;
	long j;
	if (s->n <= 10)   // The branch is about as fast as 'var', i.e. ~ 1/5 the time of pow(val, n).
	{
		res = val;
		for (j=1;j<s->n;j++)
		{
			res *= val;
		}
		s->partial += res;
	}
	else
		s->partial += pow(val, s->n);
}

inline
struct ARRAY_METRICS_moment_s
ARRAY_METRICS_moment_combine(struct ARRAY_METRICS_moment_s s1, struct ARRAY_METRICS_moment_s s2)
{
	struct ARRAY_METRICS_moment_s s_ret;
	s_ret = s1;
	s_ret.partial = s1.partial + s2.partial;
	return s_ret;
}

inline
void
ARRAY_METRICS_moment_finalize(__attribute__((unused)) struct ARRAY_METRICS_moment_s * s)
{
}


inline
void
ARRAY_METRICS_moment_output(struct ARRAY_METRICS_moment_s * s_local, struct ARRAY_METRICS_moment_s * s_global)
{
	long N = s_global->i_end - s_global->i_start;
	*s_local->result_out = s_global->partial / N;
}

metric_functions_templates(moment, i_start, i_end, get_val_as_double,
	(void * A, long i_start, long i_end, double n, double center, double * result_out, double (* get_val_as_double)(void * A, long i)),
	(A, i_start, i_end, n, center, result_out, get_val_as_double)
)


//==========================================================================================================================================
//= Standardized Moments
//==========================================================================================================================================


inline
void
ARRAY_METRICS_moment_standardized_init(__attribute__((unused)) int concurrent, struct ARRAY_METRICS_moment_standardized_s * s, void * A, long i_start, long i_end, double n, double center, double std, double * result_out, __attribute__((unused)) double (* get_val_as_double)(void * A, long i))
{
	if (result_out == NULL)
		error("'result_out' return parameter is NULL");
	s->A = A;
	s->i_start = i_start;
	s->i_end = i_end;
	s->n = n;
	s->center = center;
	s->std = std;
	s->partial = 0;
	s->result_out = result_out;
}

inline
void
ARRAY_METRICS_moment_standardized_process(struct ARRAY_METRICS_moment_standardized_s * s, long i, double (* get_val_as_double)(void * A, long i))
{
	double val = get_val_as_double(s->A, i) - s->center;
	double res;
	long j;
	if (s->n <= 10)   // The branch is about as fast as 'var', i.e. ~ 1/5 the time of pow(val, n).
	{
		res = val / s->std;
		for (j=1;j<s->n;j++)
		{
			res *= val / s->std;
		}
		s->partial += res ;
	}
	else
		s->partial += pow(val, s->n) /  pow(s->std, s->n);
}

inline
struct ARRAY_METRICS_moment_standardized_s
ARRAY_METRICS_moment_standardized_combine(struct ARRAY_METRICS_moment_standardized_s s1, struct ARRAY_METRICS_moment_standardized_s s2)
{
	struct ARRAY_METRICS_moment_standardized_s s_ret;
	s_ret = s1;
	s_ret.partial = s1.partial + s2.partial;
	return s_ret;
}

inline
void
ARRAY_METRICS_moment_standardized_finalize(__attribute__((unused)) struct ARRAY_METRICS_moment_standardized_s * s)
{
}


inline
void
ARRAY_METRICS_moment_standardized_output(struct ARRAY_METRICS_moment_standardized_s * s_local, struct ARRAY_METRICS_moment_standardized_s * s_global)
{
	long N = s_global->i_end - s_global->i_start;
	*s_local->result_out = s_global->partial / N;
}

metric_functions_templates(moment_standardized, i_start, i_end, get_val_as_double,
	(void * A, long i_start, long i_end, double n, double center, double std, double * result_out, double (* get_val_as_double)(void * A, long i)),
	(A, i_start, i_end, n, center, std, result_out, get_val_as_double)
)


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//------------------------------------------------------------------------------------------------------------------------------------------
//-                                                     Central Tendency - Moments                                                         -
//------------------------------------------------------------------------------------------------------------------------------------------
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


//==========================================================================================================================================
//= Mean
//==========================================================================================================================================


inline
void
ARRAY_METRICS_mean_init(__attribute__((unused)) int concurrent, struct ARRAY_METRICS_mean_s * s, void * A, long i_start, long i_end, double * result_out, __attribute__((unused)) double (* get_val_as_double)(void * A, long i))
{
	if (result_out == NULL)
		error("'result_out' return parameter is NULL");
	s->A = A;
	s->i_start = i_start;
	s->i_end = i_end;
	s->partial = 0;
	s->result_out = result_out;
}

inline
void
ARRAY_METRICS_mean_process(struct ARRAY_METRICS_mean_s * s, long i, double (* get_val_as_double)(void * A, long i))
{
	s->partial += get_val_as_double(s->A, i);
}

inline
struct ARRAY_METRICS_mean_s
ARRAY_METRICS_mean_combine(struct ARRAY_METRICS_mean_s s1, struct ARRAY_METRICS_mean_s s2)
{
	struct ARRAY_METRICS_mean_s s_ret;
	s_ret = s1;
	s_ret.partial = s1.partial + s2.partial;
	return s_ret;
}

inline
void
ARRAY_METRICS_mean_finalize(__attribute__((unused)) struct ARRAY_METRICS_mean_s * s)
{
}


inline
void
ARRAY_METRICS_mean_output(struct ARRAY_METRICS_mean_s * s_local, struct ARRAY_METRICS_mean_s * s_global)
{
	long N = s_global->i_end - s_global->i_start;
	*s_local->result_out = s_global->partial / N;
}

metric_functions_templates(mean, i_start, i_end, get_val_as_double,
	(void * A, long i_start, long i_end, double * result_out, double (* get_val_as_double)(void * A, long i)),
	(A, i_start, i_end, result_out, get_val_as_double)
)


//==========================================================================================================================================
//= Mean of Absolute Values
//==========================================================================================================================================


inline
void
ARRAY_METRICS_mean_abs_init(__attribute__((unused)) int concurrent, struct ARRAY_METRICS_mean_abs_s * s, void * A, long i_start, long i_end, double * result_out, __attribute__((unused)) double (* get_val_as_double)(void * A, long i))
{
	if (result_out == NULL)
		error("'result_out' return parameter is NULL");
	s->A = A;
	s->i_start = i_start;
	s->i_end = i_end;
	s->partial = 0;
	s->result_out = result_out;
}

inline
void
ARRAY_METRICS_mean_abs_process(struct ARRAY_METRICS_mean_abs_s * s, long i, double (* get_val_as_double)(void * A, long i))
{
	s->partial += fabs(get_val_as_double(s->A, i));
}

inline
struct ARRAY_METRICS_mean_abs_s
ARRAY_METRICS_mean_abs_combine(struct ARRAY_METRICS_mean_abs_s s1, struct ARRAY_METRICS_mean_abs_s s2)
{
	struct ARRAY_METRICS_mean_abs_s s_ret;
	s_ret = s1;
	s_ret.partial = s1.partial + s2.partial;
	return s_ret;
}

inline
void
ARRAY_METRICS_mean_abs_finalize(__attribute__((unused)) struct ARRAY_METRICS_mean_abs_s * s)
{
}


inline
void
ARRAY_METRICS_mean_abs_output(struct ARRAY_METRICS_mean_abs_s * s_local, struct ARRAY_METRICS_mean_abs_s * s_global)
{
	long N = s_global->i_end - s_global->i_start;
	*s_local->result_out = s_global->partial / N;
}

metric_functions_templates(mean_abs, i_start, i_end, get_val_as_double,
	(void * A, long i_start, long i_end, double * result_out, double (* get_val_as_double)(void * A, long i)),
	(A, i_start, i_end, result_out, get_val_as_double)
)


//==========================================================================================================================================
//= Geometric Mean
//==========================================================================================================================================


/* Restricted to positive numbers only (negative numbers have imaginary roots).
 */

inline
void
ARRAY_METRICS_gmean_init(__attribute__((unused)) int concurrent, struct ARRAY_METRICS_gmean_s * s, void * A, long i_start, long i_end, double * result_out, __attribute__((unused)) double (* get_val_as_double)(void * A, long i))
{
	if (result_out == NULL)
		error("'result_out' return parameter is NULL");
	s->A = A;
	s->i_start = i_start;
	s->i_end = i_end;
	s->partial = 0;
	s->result_out = result_out;
}

inline
void
ARRAY_METRICS_gmean_process(struct ARRAY_METRICS_gmean_s * s, long i, double (* get_val_as_double)(void * A, long i))
{
	s->partial += log(fabs(get_val_as_double(s->A, i)));
}

inline
struct ARRAY_METRICS_gmean_s
ARRAY_METRICS_gmean_combine(struct ARRAY_METRICS_gmean_s s1, struct ARRAY_METRICS_gmean_s s2)
{
	struct ARRAY_METRICS_gmean_s s_ret;
	s_ret = s1;
	s_ret.partial = s1.partial + s2.partial;
	return s_ret;
}

inline
void
ARRAY_METRICS_gmean_finalize(__attribute__((unused)) struct ARRAY_METRICS_gmean_s * s)
{
}


inline
void
ARRAY_METRICS_gmean_output(struct ARRAY_METRICS_gmean_s * s_local, struct ARRAY_METRICS_gmean_s * s_global)
{
	long N = s_global->i_end - s_global->i_start;
	*s_local->result_out = exp(s_global->partial / N);
}

metric_functions_templates(gmean, i_start, i_end, get_val_as_double,
	(void * A, long i_start, long i_end, double * result_out, double (* get_val_as_double)(void * A, long i)),
	(A, i_start, i_end, result_out, get_val_as_double)
)


//==========================================================================================================================================
//= Harmonic Mean
//==========================================================================================================================================


inline
void
ARRAY_METRICS_hmean_init(__attribute__((unused)) int concurrent, struct ARRAY_METRICS_hmean_s * s, void * A, long i_start, long i_end, double * result_out, __attribute__((unused)) double (* get_val_as_double)(void * A, long i))
{
	if (result_out == NULL)
		error("'result_out' return parameter is NULL");
	s->A = A;
	s->i_start = i_start;
	s->i_end = i_end;
	s->partial = 0;
	s->result_out = result_out;
}

inline
void
ARRAY_METRICS_hmean_process(struct ARRAY_METRICS_hmean_s * s, long i, double (* get_val_as_double)(void * A, long i))
{
	s->partial += 1 / get_val_as_double(s->A, i);
}

inline
struct ARRAY_METRICS_hmean_s
ARRAY_METRICS_hmean_combine(struct ARRAY_METRICS_hmean_s s1, struct ARRAY_METRICS_hmean_s s2)
{
	struct ARRAY_METRICS_hmean_s s_ret;
	s_ret = s1;
	s_ret.partial = s1.partial + s2.partial;
	return s_ret;
}

inline
void
ARRAY_METRICS_hmean_finalize(__attribute__((unused)) struct ARRAY_METRICS_hmean_s * s)
{
}


inline
void
ARRAY_METRICS_hmean_output(struct ARRAY_METRICS_hmean_s * s_local, struct ARRAY_METRICS_hmean_s * s_global)
{
	long N = s_global->i_end - s_global->i_start;
	*s_local->result_out = 1 / (s_global->partial / N);
}

metric_functions_templates(hmean, i_start, i_end, get_val_as_double,
	(void * A, long i_start, long i_end, double * result_out, double (* get_val_as_double)(void * A, long i)),
	(A, i_start, i_end, result_out, get_val_as_double)
)


//==========================================================================================================================================
//= Root Mean Square
//==========================================================================================================================================


inline
void
ARRAY_METRICS_rms_init(__attribute__((unused)) int concurrent, struct ARRAY_METRICS_rms_s * s, void * A, long i_start, long i_end, double * result_out, __attribute__((unused)) double (* get_val_as_double)(void * A, long i))
{
	if (result_out == NULL)
		error("'result_out' return parameter is NULL");
	s->A = A;
	s->i_start = i_start;
	s->i_end = i_end;
	s->partial = 0;
	s->result_out = result_out;
}

inline
void
ARRAY_METRICS_rms_process(struct ARRAY_METRICS_rms_s * s, long i, double (* get_val_as_double)(void * A, long i))
{
	double val = get_val_as_double(s->A, i);
	s->partial += val*val;
}

inline
struct ARRAY_METRICS_rms_s
ARRAY_METRICS_rms_combine(struct ARRAY_METRICS_rms_s s1, struct ARRAY_METRICS_rms_s s2)
{
	struct ARRAY_METRICS_rms_s s_ret;
	s_ret = s1;
	s_ret.partial = s1.partial + s2.partial;
	return s_ret;
}

inline
void
ARRAY_METRICS_rms_finalize(__attribute__((unused)) struct ARRAY_METRICS_rms_s * s)
{
}


inline
void
ARRAY_METRICS_rms_output(struct ARRAY_METRICS_rms_s * s_local, struct ARRAY_METRICS_rms_s * s_global)
{
	long N = s_global->i_end - s_global->i_start;
	*s_local->result_out = sqrt(s_global->partial / N);
}

metric_functions_templates(rms, i_start, i_end, get_val_as_double,
	(void * A, long i_start, long i_end, double * result_out, double (* get_val_as_double)(void * A, long i)),
	(A, i_start, i_end, result_out, get_val_as_double)
)


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//------------------------------------------------------------------------------------------------------------------------------------------
//-                                                   Quantile - Percentile - Median                                                       -
//------------------------------------------------------------------------------------------------------------------------------------------
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


// Given a vector V of length N, the median of V is
// the middle value of a sorted copy of V, V_sorted (i.e. V_sorted[(N-1)/2]) when N is odd,
// and the average of the two middle values of V_sorted when N is even.


//==========================================================================================================================================
//= Percentile
//==========================================================================================================================================


static inline
void
ARRAY_METRICS_quantile_method(const char * method, long N, double q, long * idx_out, double * frac_out)
{
	double virt_idx, m, diff, zero_threshold;
	long idx;
	double frac;

	/* R. J. Hyndman and Y. Fan, "Sample quantiles in statistical packages"
	 *
	 * numpy "quantile": https://numpy.org/doc/stable/reference/generated/numpy.quantile.html
	 */

	if (method == NULL)
		method = "median_unbiased";

	if (!strcmp(method, "inverted_cdf"))   // 1: (discontinuous) floor
		m = 0;
	else if (!strcmp(method, "averaged_inverted_cdf"))   // 2: (discontinuous) average
		m = 0;
	else if (!strcmp(method, "closest_observation"))   // 3: (discontinuous) nearest
		m = -1/2;
	else if (!strcmp(method, "interpolated_inverted_cdf"))   // 4: (continuous) interpolate the step function of method 1
		m = 0;
	else if (!strcmp(method, "hazen"))   // 5: (continuous) Hazen
		m = 1.0/2;
	else if (!strcmp(method, "weibull"))   // 6: (continuous) Weibull
		m = q;
	else if (!strcmp(method, "linear"))   // 7: (continuous) linear
		m = 1 - q;
	else if (!strcmp(method, "median_unbiased"))   // 8: (continuous) (approximately) median unbiased
		m = 1.0/3 * q + 1.0/3;
	else if (!strcmp(method, "normal_unbiased"))   // 9: (continuous)
		m = 1.0/4 * q + 3.0/8;
	else
		error("unknown quantile calculation method");

	virt_idx = (N - 1) * q + m;
	idx = virt_idx;

	diff = virt_idx - floor(virt_idx);
	// zero_threshold = 1.0e-9;
	zero_threshold = 10 * DBL_EPSILON;
	if (!strcmp(method, "inverted_cdf"))   // 1
		frac = (diff < zero_threshold) ? 0 : 1;
	else if (!strcmp(method, "averaged_inverted_cdf"))   // 2
		frac = (diff < zero_threshold) ? 1/2 : 1;
	else if (!strcmp(method, "closest_observation"))   // 3
		frac = ((diff < zero_threshold) && (idx % 2 == 0)) ? 0 : 1;
	else // continuous
		frac = virt_idx - idx;

	*idx_out = idx;
	*frac_out = frac;
}


void
ARRAY_METRICS_quantile_serial(void * A, long i_start, long i_end, double q, const char * method, double * val_out, double (* get_val_as_double)(void * A, long i))
{
	static double * vals;
	long N = i_end - i_start;
	double g;
	double val;
	long i, j;
	if (val_out == NULL)
		error("'val_out' return parameter is NULL");

	ARRAY_METRICS_quantile_method(method, N, q, &j, &g);
	j += i_start;

	if (j == N - 1)
	{
		ARRAY_METRICS_max_concurrent(A, i_start, i_end, val_out, NULL, get_val_as_double);
		return;
	}

	vals = (typeof(vals)) malloc(N * sizeof(*vals));
	for (i=i_start;i<i_end;i++)
	{
		vals[i-i_start] = get_val_as_double(A, i);
	}

	quicksort(vals, N, NULL);

	val = (1 - g) * vals[j] + g * vals[(j + 1) < N ? j + 1 : j];

	*val_out = val;

	free(vals);
}


void
ARRAY_METRICS_quantile_concurrent(void * A, long i_start, long i_end, double q, const char * method, double * val_out, double (* get_val_as_double)(void * A, long i))
{
	static double * vals;
	static double * buf;
	long N = i_end - i_start;
	double g;
	double val;
	double min, max;
	long i, j, m;
	if (val_out == NULL)
		error("'val_out' return parameter is NULL");

	ARRAY_METRICS_quantile_method(method, N, q, &j, &g);
	j += i_start;

	if (j == N - 1)
	{
		ARRAY_METRICS_max_concurrent(A, i_start, i_end, val_out, NULL, get_val_as_double);
		return;
	}

	#pragma omp single nowait
	{
		vals = (typeof(vals)) malloc(N * sizeof(*vals));
		buf = (typeof(buf)) malloc(N * sizeof(*buf));
	}
	#pragma omp barrier
	#pragma omp for
	for (i=i_start;i<i_end;i++)
	{
		vals[i-i_start] = get_val_as_double(A, i);
	}

	// samplesort_concurrent(vals, N, NULL);
	// val = (1 - g) * vals[j] + g * vals[(j + 1) < N ? j + 1 : j];

	long i_s = i_start;
	long i_e = i_end;
	while (1)
	{
		m = quicksort_partition_concurrent(vals, buf, i_s, i_e, NULL);   // 'm' is the position of the first element of the right part.
		// #pragma omp single
		// {
			// printf("i:[%10ld,%10ld,%10ld], j=%10ld, pivot=%g\n", i_s, m, i_e, j, vals[m]);
		// }
		if (m == j+1)
		{
			if (m == i_s)   // Left part has at least one element.
				error("m == i_s");
			if (m == i_e)   // Right part has at least one element.
				error("m == i_e");
			ARRAY_METRICS_max_concurrent(vals, i_s, m, &max, NULL, gen_d2d);
			ARRAY_METRICS_min_concurrent(vals, m, i_e, &min, NULL, gen_d2d);
			// printf("test j+1: i:[%ld,%ld,%ld], j=%ld, min=%g, max=%g, g=%g\n", i_s, m, i_e, j, min, max, g);
			val = (1 - g) * max + g * min;
			break;
		}
		if (j < m)
		{
			if (m == i_s)   // Left part has at least one element.
				error("m == i_s");
			if (m == i_s + 1)
			{
				// printf("test <\n");
				ARRAY_METRICS_min_concurrent(vals, m, i_e, &min, NULL, gen_d2d);
				val = (1 - g) * vals[i_s] + g * min;
				break;
			}
			i_e = m;
			continue;
		}
		else
		{
			if (m == i_e)   // Right part has at least one element.
				error("m == i_e");
			if (m == i_e - 1)
			{
				// printf("test >\n");
				ARRAY_METRICS_max_concurrent(vals, i_s, m, &max, NULL, gen_d2d);
				val = (1 - g) * max + g * vals[m];
				break;
			}
			i_s = m;
		}
	}

	*val_out = val;

	#pragma omp barrier
	#pragma omp single nowait
	{
		free(vals);
		free(buf);
	}
	#pragma omp barrier
}


void
ARRAY_METRICS_quantile(void * A, long i_start, long i_end, double q, const char * method, double * val_out, double (* get_val_as_double)(void * A, long i))
{
	if (val_out == NULL)
		error("'val_out' return parameter is NULL");
	if (omp_get_level() > 0)
	{
		ARRAY_METRICS_quantile_serial(A, i_start, i_end, q, method, val_out, get_val_as_double);
	}
	else
	{
		_Pragma("omp parallel")
		{
			double val;
			ARRAY_METRICS_quantile_concurrent(A, i_start, i_end, q, method, &val, get_val_as_double);
			_Pragma("omp single nowait")
			{
				*val_out = val;
			}
		}
	}
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//------------------------------------------------------------------------------------------------------------------------------------------
//-                                                    Dispersion - Moments                                                        -
//------------------------------------------------------------------------------------------------------------------------------------------
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


//==========================================================================================================================================
//= Mean Absolute Deviation
//==========================================================================================================================================


inline
void
ARRAY_METRICS_mad_init(__attribute__((unused)) int concurrent, struct ARRAY_METRICS_mad_s * s, void * A, long i_start, long i_end, double mean, double * result_out, __attribute__((unused)) double (* get_val_as_double)(void * A, long i))
{
	if (result_out == NULL)
		error("'result_out' return parameter is NULL");
	s->A = A;
	s->i_start = i_start;
	s->i_end = i_end;
	s->mean = mean;
	s->partial = 0;
	s->result_out = result_out;
}

inline
void
ARRAY_METRICS_mad_process(struct ARRAY_METRICS_mad_s * s, long i, double (* get_val_as_double)(void * A, long i))
{
	s->partial += fabs(get_val_as_double(s->A, i) - s->mean);
}

inline
struct ARRAY_METRICS_mad_s
ARRAY_METRICS_mad_combine(struct ARRAY_METRICS_mad_s s1, struct ARRAY_METRICS_mad_s s2)
{
	struct ARRAY_METRICS_mad_s s_ret;
	s_ret = s1;
	s_ret.partial = s1.partial + s2.partial;
	return s_ret;
}

inline
void
ARRAY_METRICS_mad_finalize(__attribute__((unused)) struct ARRAY_METRICS_mad_s * s)
{
}


inline
void
ARRAY_METRICS_mad_output(struct ARRAY_METRICS_mad_s * s_local, struct ARRAY_METRICS_mad_s * s_global)
{
	long N = s_global->i_end - s_global->i_start;
	*s_local->result_out = s_global->partial / N;
}

metric_functions_templates(mad, i_start, i_end, get_val_as_double,
	(void * A, long i_start, long i_end, double mean, double * result_out, double (* get_val_as_double)(void * A, long i)),
	(A, i_start, i_end, mean, result_out, get_val_as_double)
)


//==========================================================================================================================================
//= Variance - One Pass
//==========================================================================================================================================


/* The variance is invariant with respect to changes in a location parameter, a property which can be used to avoid the catastrophic cancellation in this formula.
 *     Var(X-K) = Var(X)
 * with K any constant, which leads to the new formula:
 *     Var = ( Sum_1_N{ (x_i - K)^2 } ) / N - ( (Sum_1_N{ x_i - K })^2 ) /  N
 *         = rms[X-K]^2 - E[X-K]^2
 * the closer K is to the mean value the more accurate the result will be, but just choosing a value inside the samples range will guarantee the desired stability.
 * If the values (x_i - K) are small then there are no problems with the sum of its squares, on the contrary, if they are large it necessarily means that the variance is large as well.
 * In any case the second term in the formula is always smaller than the first one therefore no cancellation may occur.
 */

inline
void
ARRAY_METRICS_var_init(__attribute__((unused)) int concurrent, struct ARRAY_METRICS_var_s * s, void * A, long i_start, long i_end, double * result_out, __attribute__((unused)) double (* get_val_as_double)(void * A, long i))
{
	long N = i_end - i_start;
	double avg = 0;
	long samples = 100;
	long i;
	if (result_out == NULL)
		error("'result_out' return parameter is NULL");
	for (i=0;i<samples;i++)
		avg += get_val_as_double(A, (i * N / samples));
	avg /= samples;
	s->A = A;
	s->i_start = i_start;
	s->i_end = i_end;
	s->stability_factor = avg;
	s->shifted_mean = 0;
	s->shifted_ms = 0;
	s->result_out = result_out;
}

inline
void
ARRAY_METRICS_var_process(struct ARRAY_METRICS_var_s * s, long i, double (* get_val_as_double)(void * A, long i))
{
	double val = get_val_as_double(s->A, i) - s->stability_factor;
	s->shifted_mean += val;
	s->shifted_ms += val * val;
}

inline
struct ARRAY_METRICS_var_s
ARRAY_METRICS_var_combine(struct ARRAY_METRICS_var_s s1, struct ARRAY_METRICS_var_s s2)
{
	struct ARRAY_METRICS_var_s s_ret;
	s_ret = s1;
	s_ret.shifted_mean = s1.shifted_mean + s2.shifted_mean;
	s_ret.shifted_ms = s1.shifted_ms + s2.shifted_ms;
	return s_ret;
}

inline
void
ARRAY_METRICS_var_finalize(__attribute__((unused)) struct ARRAY_METRICS_var_s * s)
{
}


inline
void
ARRAY_METRICS_var_output(struct ARRAY_METRICS_var_s * s_local, struct ARRAY_METRICS_var_s * s_global)
{
	long N = s_global->i_end - s_global->i_start;
	double shifted_mean = s_global->shifted_mean / N;
	double shifted_ms = s_global->shifted_ms / N;
	*s_local->result_out = shifted_ms - (shifted_mean*shifted_mean);
}

metric_functions_templates(var, i_start, i_end, get_val_as_double,
	(void * A, long i_start, long i_end, double * result_out, double (* get_val_as_double)(void * A, long i)),
	(A, i_start, i_end, result_out, get_val_as_double)
)


//==========================================================================================================================================
//= Variance - Given Mean
//==========================================================================================================================================


inline
void
ARRAY_METRICS_var_orig_init(__attribute__((unused)) int concurrent, struct ARRAY_METRICS_var_orig_s * s, void * A, long i_start, long i_end, double mean, double * result_out, __attribute__((unused)) double (* get_val_as_double)(void * A, long i))
{
	if (result_out == NULL)
		error("'result_out' return parameter is NULL");
	s->A = A;
	s->i_start = i_start;
	s->i_end = i_end;
	s->mean = mean;
	s->partial = 0;
	s->result_out = result_out;
}

inline
void
ARRAY_METRICS_var_orig_process(struct ARRAY_METRICS_var_orig_s * s, long i, double (* get_val_as_double)(void * A, long i))
{
	double val = get_val_as_double(s->A, i) - s->mean;
	s->partial += val * val;
}

inline
struct ARRAY_METRICS_var_orig_s
ARRAY_METRICS_var_orig_combine(struct ARRAY_METRICS_var_orig_s s1, struct ARRAY_METRICS_var_orig_s s2)
{
	struct ARRAY_METRICS_var_orig_s s_ret;
	s_ret = s1;
	s_ret.partial = s1.partial + s2.partial;
	return s_ret;
}

inline
void
ARRAY_METRICS_var_orig_finalize(__attribute__((unused)) struct ARRAY_METRICS_var_orig_s * s)
{
}


inline
void
ARRAY_METRICS_var_orig_output(struct ARRAY_METRICS_var_orig_s * s_local, struct ARRAY_METRICS_var_orig_s * s_global)
{
	long N = s_global->i_end - s_global->i_start;
	*s_local->result_out = s_global->partial / N;
}

metric_functions_templates(var_orig, i_start, i_end, get_val_as_double,
	(void * A, long i_start, long i_end, double mean, double * result_out, double (* get_val_as_double)(void * A, long i)),
	(A, i_start, i_end, mean, result_out, get_val_as_double)
)


//==========================================================================================================================================
//= Standard Deviation
//==========================================================================================================================================


inline
void
ARRAY_METRICS_std_init(__attribute__((unused)) int concurrent, struct ARRAY_METRICS_std_s * s, void * A, long i_start, long i_end, double * result_out, __attribute__((unused)) double (* get_val_as_double)(void * A, long i))
{
	ARRAY_METRICS_var_init(concurrent, &s->var_s, A, i_start, i_end, result_out, get_val_as_double);
}

inline
void
ARRAY_METRICS_std_process(struct ARRAY_METRICS_std_s * s, long i, double (* get_val_as_double)(void * A, long i))
{
	ARRAY_METRICS_var_process(&s->var_s, i, get_val_as_double);
}

inline
struct ARRAY_METRICS_std_s
ARRAY_METRICS_std_combine(struct ARRAY_METRICS_std_s s1, struct ARRAY_METRICS_std_s s2)
{
	struct ARRAY_METRICS_std_s s_ret;
	s_ret.var_s = ARRAY_METRICS_var_combine(s1.var_s, s2.var_s);
	return s_ret;
}

inline
void
ARRAY_METRICS_std_finalize(__attribute__((unused)) struct ARRAY_METRICS_std_s * s)
{
}


inline
void
ARRAY_METRICS_std_output(struct ARRAY_METRICS_std_s * s_local, struct ARRAY_METRICS_std_s * s_global)
{
	ARRAY_METRICS_var_output(&s_local->var_s, &s_global->var_s);
	*s_local->var_s.result_out = sqrt(*s_local->var_s.result_out);
}

metric_functions_templates(std, i_start, i_end, get_val_as_double,
	(void * A, long i_start, long i_end, double * result_out, double (* get_val_as_double)(void * A, long i)),
	(A, i_start, i_end, result_out, get_val_as_double)
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


inline
void
ARRAY_METRICS_mae_init(__attribute__((unused)) int concurrent, struct ARRAY_METRICS_mae_s * s, void * A, void * F, long i_start, long i_end, double * result_out, __attribute__((unused)) double (* get_val_as_double)(void * A, long i))
{
	if (result_out == NULL)
		error("'result_out' return parameter is NULL");
	s->A = A;
	s->F = F;
	s->i_start = i_start;
	s->i_end = i_end;
	s->partial = 0;
	s->result_out = result_out;
}

inline
void
ARRAY_METRICS_mae_process(struct ARRAY_METRICS_mae_s * s, long i, double (* get_val_as_double)(void * A, long i))
{
	s->partial += fabs(get_val_as_double(s->A, i) - get_val_as_double(s->F, i));
}

inline
struct ARRAY_METRICS_mae_s
ARRAY_METRICS_mae_combine(struct ARRAY_METRICS_mae_s s1, struct ARRAY_METRICS_mae_s s2)
{
	struct ARRAY_METRICS_mae_s s_ret;
	s_ret = s1;
	s_ret.partial = s1.partial + s2.partial;
	return s_ret;
}

inline
void
ARRAY_METRICS_mae_finalize(__attribute__((unused)) struct ARRAY_METRICS_mae_s * s)
{
}


inline
void
ARRAY_METRICS_mae_output(struct ARRAY_METRICS_mae_s * s_local, struct ARRAY_METRICS_mae_s * s_global)
{
	long N = s_global->i_end - s_global->i_start;
	*s_local->result_out = s_global->partial / N;
}

metric_functions_templates(mae, i_start, i_end, get_val_as_double,
	(void * A, void * F, long i_start, long i_end, double * result_out, double (* get_val_as_double)(void * A, long i)),
	(A, F, i_start, i_end, result_out, get_val_as_double)
)


//==========================================================================================================================================
//= Max Absolute Error
//==========================================================================================================================================


inline
void
ARRAY_METRICS_max_ae_init(__attribute__((unused)) int concurrent, struct ARRAY_METRICS_max_ae_s * s, void * A, void * F, long i_start, long i_end, double * result_out, __attribute__((unused)) double (* get_val_as_double)(void * A, long i))
{
	if (result_out == NULL)
		error("'result_out' return parameter is NULL");
	s->A = A;
	s->F = F;
	s->i_start = i_start;
	s->i_end = i_end;
	s->partial = 0;
	s->result_out = result_out;
}

inline
void
ARRAY_METRICS_max_ae_process(struct ARRAY_METRICS_max_ae_s * s, long i, double (* get_val_as_double)(void * A, long i))
{
	double val = fabs(get_val_as_double(s->A, i) - get_val_as_double(s->F, i));
	if (val > s->partial)
		s->partial = val;
}

inline
struct ARRAY_METRICS_max_ae_s
ARRAY_METRICS_max_ae_combine(struct ARRAY_METRICS_max_ae_s s1, struct ARRAY_METRICS_max_ae_s s2)
{
	struct ARRAY_METRICS_max_ae_s s_ret;
	s_ret = s1;
	if (s2.partial > s_ret.partial)
		s_ret.partial = s2.partial;
	return s_ret;
}

inline
void
ARRAY_METRICS_max_ae_finalize(__attribute__((unused)) struct ARRAY_METRICS_max_ae_s * s)
{
}


inline
void
ARRAY_METRICS_max_ae_output(struct ARRAY_METRICS_max_ae_s * s_local, struct ARRAY_METRICS_max_ae_s * s_global)
{
	*s_local->result_out = s_global->partial;
}

metric_functions_templates(max_ae, i_start, i_end, get_val_as_double,
	(void * A, void * F, long i_start, long i_end, double * result_out, double (* get_val_as_double)(void * A, long i)),
	(A, F, i_start, i_end, result_out, get_val_as_double)
)


//==========================================================================================================================================
//= Mean Squared Error
//==========================================================================================================================================


inline
void
ARRAY_METRICS_mse_init(__attribute__((unused)) int concurrent, struct ARRAY_METRICS_mse_s * s, void * A, void * F, long i_start, long i_end, double * result_out, __attribute__((unused)) double (* get_val_as_double)(void * A, long i))
{
	if (result_out == NULL)
		error("'result_out' return parameter is NULL");
	s->A = A;
	s->F = F;
	s->i_start = i_start;
	s->i_end = i_end;
	s->partial = 0;
	s->result_out = result_out;
}

inline
void
ARRAY_METRICS_mse_process(struct ARRAY_METRICS_mse_s * s, long i, double (* get_val_as_double)(void * A, long i))
{
	double val = get_val_as_double(s->A, i) - get_val_as_double(s->F, i);
	s->partial += val * val;
}

inline
struct ARRAY_METRICS_mse_s
ARRAY_METRICS_mse_combine(struct ARRAY_METRICS_mse_s s1, struct ARRAY_METRICS_mse_s s2)
{
	struct ARRAY_METRICS_mse_s s_ret;
	s_ret = s1;
	s_ret.partial = s1.partial + s2.partial;
	return s_ret;
}

inline
void
ARRAY_METRICS_mse_finalize(__attribute__((unused)) struct ARRAY_METRICS_mse_s * s)
{
}


inline
void
ARRAY_METRICS_mse_output(struct ARRAY_METRICS_mse_s * s_local, struct ARRAY_METRICS_mse_s * s_global)
{
	long N = s_global->i_end - s_global->i_start;
	*s_local->result_out = s_global->partial / N;
}

metric_functions_templates(mse, i_start, i_end, get_val_as_double,
	(void * A, void * F, long i_start, long i_end, double * result_out, double (* get_val_as_double)(void * A, long i)),
	(A, F, i_start, i_end, result_out, get_val_as_double)
)


//==========================================================================================================================================
//= Mean Absolute Relative Error
//==========================================================================================================================================


inline
void
ARRAY_METRICS_mare_init(__attribute__((unused)) int concurrent, struct ARRAY_METRICS_mare_s * s, void * A, void * F, long i_start, long i_end, double * result_out, __attribute__((unused)) double (* get_val_as_double)(void * A, long i))
{
	if (result_out == NULL)
		error("'result_out' return parameter is NULL");
	s->A = A;
	s->F = F;
	s->i_start = i_start;
	s->i_end = i_end;
	s->partial = 0;
	s->result_out = result_out;
}

inline
void
ARRAY_METRICS_mare_process(struct ARRAY_METRICS_mare_s * s, long i, double (* get_val_as_double)(void * A, long i))
{
	double a, f, ae;
	a = get_val_as_double(s->A, i);
	f = get_val_as_double(s->F, i);
	ae = fabs(a - f);
	s->partial = (ae <= 2 * DBL_EPSILON) ? 0 : ae / fabs(a);
}

inline
struct ARRAY_METRICS_mare_s
ARRAY_METRICS_mare_combine(struct ARRAY_METRICS_mare_s s1, struct ARRAY_METRICS_mare_s s2)
{
	struct ARRAY_METRICS_mare_s s_ret;
	s_ret = s1;
	s_ret.partial = s1.partial + s2.partial;
	return s_ret;
}

inline
void
ARRAY_METRICS_mare_finalize(__attribute__((unused)) struct ARRAY_METRICS_mare_s * s)
{
}


inline
void
ARRAY_METRICS_mare_output(struct ARRAY_METRICS_mare_s * s_local, struct ARRAY_METRICS_mare_s * s_global)
{
	long N = s_global->i_end - s_global->i_start;
	*s_local->result_out = s_global->partial / N;
}

metric_functions_templates(mare, i_start, i_end, get_val_as_double,
	(void * A, void * F, long i_start, long i_end, double * result_out, double (* get_val_as_double)(void * A, long i)),
	(A, F, i_start, i_end, result_out, get_val_as_double)
)


//==========================================================================================================================================
//= Mean Absolute Percentage Error
//==========================================================================================================================================


inline
void
ARRAY_METRICS_mape_init(__attribute__((unused)) int concurrent, struct ARRAY_METRICS_mape_s * s, void * A, void * F, long i_start, long i_end, double * result_out, __attribute__((unused)) double (* get_val_as_double)(void * A, long i))
{
	if (result_out == NULL)
		error("'result_out' return parameter is NULL");
	s->A = A;
	s->F = F;
	s->i_start = i_start;
	s->i_end = i_end;
	s->partial = 0;
	s->result_out = result_out;
}

inline
void
ARRAY_METRICS_mape_process(struct ARRAY_METRICS_mape_s * s, long i, double (* get_val_as_double)(void * A, long i))
{
	double a, f, ae;
	a = get_val_as_double(s->A, i);
	f = get_val_as_double(s->F, i);
	ae = fabs(a - f);
	s->partial = (ae <= 2 * DBL_EPSILON) ? 0 : ae / fabs(a);
}

inline
struct ARRAY_METRICS_mape_s
ARRAY_METRICS_mape_combine(struct ARRAY_METRICS_mape_s s1, struct ARRAY_METRICS_mape_s s2)
{
	struct ARRAY_METRICS_mape_s s_ret;
	s_ret = s1;
	s_ret.partial = s1.partial + s2.partial;
	return s_ret;
}

inline
void
ARRAY_METRICS_mape_finalize(__attribute__((unused)) struct ARRAY_METRICS_mape_s * s)
{
}


inline
void
ARRAY_METRICS_mape_output(struct ARRAY_METRICS_mape_s * s_local, struct ARRAY_METRICS_mape_s * s_global)
{
	long N = s_global->i_end - s_global->i_start;
	*s_local->result_out = 100.0 * s_global->partial / N;
}

metric_functions_templates(mape, i_start, i_end, get_val_as_double,
	(void * A, void * F, long i_start, long i_end, double * result_out, double (* get_val_as_double)(void * A, long i)),
	(A, F, i_start, i_end, result_out, get_val_as_double)
)


//==========================================================================================================================================
//= Symmetric Mean Absolute Relative Error
//==========================================================================================================================================


/* 0 <= smare <= 1
 */

inline
void
ARRAY_METRICS_smare_init(__attribute__((unused)) int concurrent, struct ARRAY_METRICS_smare_s * s, void * A, void * F, long i_start, long i_end, double * result_out, __attribute__((unused)) double (* get_val_as_double)(void * A, long i))
{
	if (result_out == NULL)
		error("'result_out' return parameter is NULL");
	s->A = A;
	s->F = F;
	s->i_start = i_start;
	s->i_end = i_end;
	s->partial = 0;
	s->result_out = result_out;
}

inline
void
ARRAY_METRICS_smare_process(struct ARRAY_METRICS_smare_s * s, long i, double (* get_val_as_double)(void * A, long i))
{
	double a, f, ae;
	a = get_val_as_double(s->A, i);
	f = get_val_as_double(s->F, i);
	ae = fabs(a - f);
	s->partial = (ae <= 2 * DBL_EPSILON) ? 0 : ae / (fabs(a) + fabs(f));
}

inline
struct ARRAY_METRICS_smare_s
ARRAY_METRICS_smare_combine(struct ARRAY_METRICS_smare_s s1, struct ARRAY_METRICS_smare_s s2)
{
	struct ARRAY_METRICS_smare_s s_ret;
	s_ret = s1;
	s_ret.partial = s1.partial + s2.partial;
	return s_ret;
}

inline
void
ARRAY_METRICS_smare_finalize(__attribute__((unused)) struct ARRAY_METRICS_smare_s * s)
{
}


inline
void
ARRAY_METRICS_smare_output(struct ARRAY_METRICS_smare_s * s_local, struct ARRAY_METRICS_smare_s * s_global)
{
	long N = s_global->i_end - s_global->i_start;
	*s_local->result_out = s_global->partial / N;
}

metric_functions_templates(smare, i_start, i_end, get_val_as_double,
	(void * A, void * F, long i_start, long i_end, double * result_out, double (* get_val_as_double)(void * A, long i)),
	(A, F, i_start, i_end, result_out, get_val_as_double)
)


//==========================================================================================================================================
//= Symmetric Mean Absolute Percentage Error
//==========================================================================================================================================


/* 0 <= smape <= 100
 */

inline
void
ARRAY_METRICS_smape_init(__attribute__((unused)) int concurrent, struct ARRAY_METRICS_smape_s * s, void * A, void * F, long i_start, long i_end, double * result_out, __attribute__((unused)) double (* get_val_as_double)(void * A, long i))
{
	if (result_out == NULL)
		error("'result_out' return parameter is NULL");
	s->A = A;
	s->F = F;
	s->i_start = i_start;
	s->i_end = i_end;
	s->partial = 0;
	s->result_out = result_out;
}

inline
void
ARRAY_METRICS_smape_process(struct ARRAY_METRICS_smape_s * s, long i, double (* get_val_as_double)(void * A, long i))
{
	double a, f, ae;
	a = get_val_as_double(s->A, i);
	f = get_val_as_double(s->F, i);
	ae = fabs(a - f);
	s->partial = (ae <= 2 * DBL_EPSILON) ? 0 : ae / (fabs(a) + fabs(f));
}

inline
struct ARRAY_METRICS_smape_s
ARRAY_METRICS_smape_combine(struct ARRAY_METRICS_smape_s s1, struct ARRAY_METRICS_smape_s s2)
{
	struct ARRAY_METRICS_smape_s s_ret;
	s_ret = s1;
	s_ret.partial = s1.partial + s2.partial;
	return s_ret;
}

inline
void
ARRAY_METRICS_smape_finalize(__attribute__((unused)) struct ARRAY_METRICS_smape_s * s)
{
}


inline
void
ARRAY_METRICS_smape_output(struct ARRAY_METRICS_smape_s * s_local, struct ARRAY_METRICS_smape_s * s_global)
{
	long N = s_global->i_end - s_global->i_start;
	*s_local->result_out = 100.0 * s_global->partial / N;
}

metric_functions_templates(smape, i_start, i_end, get_val_as_double,
	(void * A, void * F, long i_start, long i_end, double * result_out, double (* get_val_as_double)(void * A, long i)),
	(A, F, i_start, i_end, result_out, get_val_as_double)
)


//==========================================================================================================================================
//= Q Error - Relative Error
//==========================================================================================================================================


inline
void
ARRAY_METRICS_Q_error_init(__attribute__((unused)) int concurrent, struct ARRAY_METRICS_Q_error_s * s, void * A, void * F, long i_start, long i_end, double * result_out, __attribute__((unused)) double (* get_val_as_double)(void * A, long i))
{
	if (result_out == NULL)
		error("'result_out' return parameter is NULL");
	s->A = A;
	s->F = F;
	s->i_start = i_start;
	s->i_end = i_end;
	s->partial = 0;
	s->result_out = result_out;
}

inline
void
ARRAY_METRICS_Q_error_process(struct ARRAY_METRICS_Q_error_s * s, long i, double (* get_val_as_double)(void * A, long i))
{
	double a, f;
	a = get_val_as_double(s->A, i);
	f = get_val_as_double(s->F, i);
	s->partial += fabs(f) / fabs(a);
}

inline
struct ARRAY_METRICS_Q_error_s
ARRAY_METRICS_Q_error_combine(struct ARRAY_METRICS_Q_error_s s1, struct ARRAY_METRICS_Q_error_s s2)
{
	struct ARRAY_METRICS_Q_error_s s_ret;
	s_ret = s1;
	s_ret.partial = s1.partial + s2.partial;
	return s_ret;
}

inline
void
ARRAY_METRICS_Q_error_finalize(__attribute__((unused)) struct ARRAY_METRICS_Q_error_s * s)
{
}


inline
void
ARRAY_METRICS_Q_error_output(struct ARRAY_METRICS_Q_error_s * s_local, struct ARRAY_METRICS_Q_error_s * s_global)
{
	long N = s_global->i_end - s_global->i_start;
	*s_local->result_out = s_global->partial / N;
}

metric_functions_templates(Q_error, i_start, i_end, get_val_as_double,
	(void * A, void * F, long i_start, long i_end, double * result_out, double (* get_val_as_double)(void * A, long i)),
	(A, F, i_start, i_end, result_out, get_val_as_double)
)


//==========================================================================================================================================
//= Ln Q Error - Log Relative Error
//==========================================================================================================================================


inline
void
ARRAY_METRICS_lnQ_error_init(__attribute__((unused)) int concurrent, struct ARRAY_METRICS_lnQ_error_s * s, void * A, void * F, long i_start, long i_end, double * result_out, __attribute__((unused)) double (* get_val_as_double)(void * A, long i))
{
	if (result_out == NULL)
		error("'result_out' return parameter is NULL");
	s->A = A;
	s->F = F;
	s->i_start = i_start;
	s->i_end = i_end;
	s->partial = 0;
	s->result_out = result_out;
}

inline
void
ARRAY_METRICS_lnQ_error_process(struct ARRAY_METRICS_lnQ_error_s * s, long i, double (* get_val_as_double)(void * A, long i))
{
	double a, f;
	a = get_val_as_double(s->A, i);
	f = get_val_as_double(s->F, i);
	s->partial += log(fabs(f)) - log(fabs(a));
}

inline
struct ARRAY_METRICS_lnQ_error_s
ARRAY_METRICS_lnQ_error_combine(struct ARRAY_METRICS_lnQ_error_s s1, struct ARRAY_METRICS_lnQ_error_s s2)
{
	struct ARRAY_METRICS_lnQ_error_s s_ret;
	s_ret = s1;
	s_ret.partial = s1.partial + s2.partial;
	return s_ret;
}

inline
void
ARRAY_METRICS_lnQ_error_finalize(__attribute__((unused)) struct ARRAY_METRICS_lnQ_error_s * s)
{
}


inline
void
ARRAY_METRICS_lnQ_error_output(struct ARRAY_METRICS_lnQ_error_s * s_local, struct ARRAY_METRICS_lnQ_error_s * s_global)
{
	long N = s_global->i_end - s_global->i_start;
	*s_local->result_out = s_global->partial / N;
}

metric_functions_templates(lnQ_error, i_start, i_end, get_val_as_double,
	(void * A, void * F, long i_start, long i_end, double * result_out, double (* get_val_as_double)(void * A, long i)),
	(A, F, i_start, i_end, result_out, get_val_as_double)
)


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//------------------------------------------------------------------------------------------------------------------------------------------
//-                                                         Bit Characteristics                                                            -
//------------------------------------------------------------------------------------------------------------------------------------------
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


inline
void
ARRAY_METRICS_mean_trailing_zeros_init(__attribute__((unused)) int concurrent, struct ARRAY_METRICS_mean_trailing_zeros_s * s, void * A, long i_start, long i_end, double * result_out, __attribute__((unused)) double (* get_val_as_double)(void * A, long i))
{
	if (result_out == NULL)
		error("'result_out' return parameter is NULL");
	s->A = A;
	s->i_start = i_start;
	s->i_end = i_end;
	s->partial = 0;
	s->result_out = result_out;
}

inline
void
ARRAY_METRICS_mean_trailing_zeros_process(struct ARRAY_METRICS_mean_trailing_zeros_s * s, long i, double (* get_val_as_double)(void * A, long i))
{
	union {
		double d;
		uint64_t u;
	} val;
	val.d = get_val_as_double(s->A, i);
	s->partial += __builtin_ctzl(val.u);
}

inline
struct ARRAY_METRICS_mean_trailing_zeros_s
ARRAY_METRICS_mean_trailing_zeros_combine(struct ARRAY_METRICS_mean_trailing_zeros_s s1, struct ARRAY_METRICS_mean_trailing_zeros_s s2)
{
	struct ARRAY_METRICS_mean_trailing_zeros_s s_ret;
	s_ret = s1;
	s_ret.partial = s1.partial + s2.partial;
	return s_ret;
}

inline
void
ARRAY_METRICS_mean_trailing_zeros_finalize(__attribute__((unused)) struct ARRAY_METRICS_mean_trailing_zeros_s * s)
{
}


inline
void
ARRAY_METRICS_mean_trailing_zeros_output(struct ARRAY_METRICS_mean_trailing_zeros_s * s_local, struct ARRAY_METRICS_mean_trailing_zeros_s * s_global)
{
	long N = s_global->i_end - s_global->i_start;
	*s_local->result_out = s_global->partial / N;
}

metric_functions_templates(mean_trailing_zeros, i_start, i_end, get_val_as_double,
	(void * A, long i_start, long i_end, double * result_out, double (* get_val_as_double)(void * A, long i)),
	(A, i_start, i_end, result_out, get_val_as_double)
)


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//------------------------------------------------------------------------------------------------------------------------------------------
//-                                                           Unique Values                                                                -
//------------------------------------------------------------------------------------------------------------------------------------------
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


#include "data_structures/hashtable/hashtable_gen_undef.h"
#define HASHTABLE_GEN_VALUE_SAME_AS_KEY  1
#define HASHTABLE_GEN_KEY_IS_REF  0
#define HASHTABLE_GEN_TYPE_1  double
#undef  HASHTABLE_GEN_TYPE_2
#define HASHTABLE_GEN_TYPE_3  short
#define HASHTABLE_GEN_SUFFIX  _array_metrics
#include "data_structures/hashtable/hashtable_gen.c"


//==========================================================================================================================================
//= Number of Unique Values
//==========================================================================================================================================


inline
void
ARRAY_METRICS_unique_num_init(int concurrent, struct ARRAY_METRICS_unique_num_s * s, void * A, long i_start, long i_end, long * num_vals_out, __attribute__((unused)) double (* get_val_as_double)(void * A, long i))
{
	if (num_vals_out == NULL)
		error("'num_vals_out' return parameter is NULL");
	long N = i_end - i_start;
	s->A = A;
	s->i_start = i_start;
	s->i_end = i_end;
	s->concurrent = concurrent;
	s->num_vals = 0;
	s->num_vals_out = num_vals_out;
	s->ht = (s->concurrent) ? hashtable_new_concurrent(N) : hashtable_new_serial(N);
}

inline
void
ARRAY_METRICS_unique_num_process(struct ARRAY_METRICS_unique_num_s * s, long i, double (* get_val_as_double)(void * A, long i))
{
	struct hashtable * ht = s->ht;
	if (s->concurrent)
		hashtable_insert_concurrent(ht, get_val_as_double(s->A, i));
	else
		hashtable_insert_serial(ht, get_val_as_double(s->A, i));
}

inline
struct ARRAY_METRICS_unique_num_s
ARRAY_METRICS_unique_num_combine(struct ARRAY_METRICS_unique_num_s s1, __attribute__((unused)) struct ARRAY_METRICS_unique_num_s s2)
{
	return s1;
}

inline
void
ARRAY_METRICS_unique_num_finalize(struct ARRAY_METRICS_unique_num_s * s)
{
	struct hashtable * ht = s->ht;
	if (s->concurrent)
	{
		s->num_vals = hashtable_num_entries_concurrent(ht);
		hashtable_destroy_concurrent(&ht);
	}
	else
	{
		s->num_vals = hashtable_num_entries_serial(ht);
		hashtable_destroy_serial(&ht);
	}
}

inline
void
ARRAY_METRICS_unique_num_output(struct ARRAY_METRICS_unique_num_s * s_local, struct ARRAY_METRICS_unique_num_s * s_global)
{
	*s_local->num_vals_out = s_global->num_vals;
}

metric_functions_templates(unique_num, i_start, i_end, get_val_as_double,
	(void * A, long i_start, long i_end, long * num_vals_out, double (* get_val_as_double)(void * A, long i)),
	(A, i_start, i_end, num_vals_out, get_val_as_double)
)


//==========================================================================================================================================
//= Unique Values
//==========================================================================================================================================


inline
void
ARRAY_METRICS_unique_init(int concurrent, struct ARRAY_METRICS_unique_s * s, void * A, long i_start, long i_end, double ** vals_out, long * num_vals_out, __attribute__((unused)) double (* get_val_as_double)(void * A, long i))
{
	if (vals_out == NULL)
		error("'vals_out' return parameter is NULL");
	if (num_vals_out == NULL)
		error("'num_vals_out' return parameter is NULL");
	long N = i_end - i_start;
	s->A = A;
	s->i_start = i_start;
	s->i_end = i_end;
	s->concurrent = concurrent;
	s->vals = NULL;
	s->num_vals = 0;
	s->vals_out = vals_out;
	s->num_vals_out = num_vals_out;
	s->ht = (s->concurrent) ? hashtable_new_concurrent(N) : hashtable_new_serial(N);
}

inline
void
ARRAY_METRICS_unique_process(struct ARRAY_METRICS_unique_s * s, long i, double (* get_val_as_double)(void * A, long i))
{
	struct hashtable * ht = s->ht;
	if (s->concurrent)
		hashtable_insert_concurrent(ht, get_val_as_double(s->A, i));
	else
		hashtable_insert_serial(ht, get_val_as_double(s->A, i));
}

inline
struct ARRAY_METRICS_unique_s
ARRAY_METRICS_unique_combine(struct ARRAY_METRICS_unique_s s1, __attribute__((unused)) struct ARRAY_METRICS_unique_s s2)
{
	return s1;
}

inline
void
ARRAY_METRICS_unique_finalize(struct ARRAY_METRICS_unique_s * s)
{
	struct hashtable * ht = s->ht;
	if (s->concurrent)
	{
		hashtable_entries_concurrent(ht, &s->vals, &s->num_vals);
		hashtable_destroy_concurrent(&ht);
	}
	else
	{
		hashtable_entries_serial(ht, &s->vals, &s->num_vals);
		hashtable_destroy_serial(&ht);
	}
}

inline
void
ARRAY_METRICS_unique_output(struct ARRAY_METRICS_unique_s * s_local, struct ARRAY_METRICS_unique_s * s_global)
{
	*s_local->vals_out = s_global->vals;
	*s_local->num_vals_out = s_global->num_vals;
}

metric_functions_templates(unique, i_start, i_end, get_val_as_double,
	(void * A, long i_start, long i_end, double ** vals_out, long * num_vals_out, double (* get_val_as_double)(void * A, long i)),
	(A, i_start, i_end, vals_out, num_vals_out, get_val_as_double)
)


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//------------------------------------------------------------------------------------------------------------------------------------------
//-                                                      Windowed Aggregate Metric                                                         -
//------------------------------------------------------------------------------------------------------------------------------------------
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


/* We can't assume that the zero element is 0 (e.g. max() has zero element negative infinity),
 * so we have to set the aggregate to the first value.
 */

double
ARRAY_METRICS_windowed_aggregate_serial(void * A, long i_start, long i_end, long window_len,
		double (* get_window_aggregate_as_double)(void * A, long i, long len),
		double (* reduce_fun)(double a, double b)
		)
{
	double agg;
	long i, len;
	if (i_end <= i_start)
		error("array length is non-positive");
	if (window_len <= 0)
		error("window length is non-positive");
	i = i_start;
	len = (i_end - i) < window_len ? (i_end - i) : window_len;
	agg = get_window_aggregate_as_double(A, i, len);
	i+=window_len;
	for (;i<i_end;i+=window_len)
	{
		len = (i_end - i) < window_len ? (i_end - i) : window_len;
		agg = reduce_fun(agg, get_window_aggregate_as_double(A, i, len));
	}
	return agg;
}


double
ARRAY_METRICS_windowed_aggregate_concurrent(void * A, long i_start, long i_end, long window_len,
		double (* get_window_aggregate_as_double)(void * A, long i, long len),
		double (* reduce_fun)(double a, double b)
		)
{
	int num_threads = safe_omp_get_num_threads();
	int tnum = omp_get_thread_num();
	double agg;
	long i, i_s, i_e, len;
	if (i_end <= i_start)
		error("array length is non-positive");
	if (window_len <= 0)
		error("window length is non-positive");
	loop_partitioner_balance_iterations(num_threads, tnum, i_start, i_end, &i_s, &i_e, window_len);
	i = i_s;
	len = (i_end - i) < window_len ? (i_end - i) : window_len;
	agg = get_window_aggregate_as_double(A, i, len);
	i+=window_len;
	for (;i<i_e;i+=window_len)
	{
		len = (i_e - i) < window_len ? (i_e - i) : window_len;
		agg = reduce_fun(agg, get_window_aggregate_as_double(A, i, len));
	}
	omp_thread_reduce_global(reduce_fun, agg, 0, 1, 0, NULL, &agg);
	return agg;
}


double
ARRAY_METRICS_windowed_aggregate(void * A, long i_start, long i_end, long window_len,
		double (* get_window_aggregate_as_double)(void * A, long i, long len),
		double (* reduce_fun)(double a, double b)
		)
{
	long ret;
	if (omp_get_level() > 0)
		return ARRAY_METRICS_windowed_aggregate_serial(A, i_start, i_end, window_len, get_window_aggregate_as_double, reduce_fun);
	_Pragma("omp parallel")
	{
		long agg = ARRAY_METRICS_windowed_aggregate_concurrent(A, i_start, i_end, window_len, get_window_aggregate_as_double, reduce_fun);
		_Pragma("omp single nowait")
		{
			ret = agg;
		}
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


int
ARRAY_METRICS_closest_pair_idx_cmpfunc(const void * a, const void * b)
{
	struct pair * p1 = (struct pair *) a;
	struct pair * p2 = (struct pair *) b;
	return (p1->val > p1->val) ? 1 : p1->val < p2->val ? -1 : 0;
}


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
	qsort(P, N, sizeof(*P), ARRAY_METRICS_closest_pair_idx_cmpfunc);
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


#include "sort/samplesort/samplesort_gen_undef.h"
#define SAMPLESORT_GEN_TYPE_1  struct pair
#define SAMPLESORT_GEN_TYPE_2  long
#define SAMPLESORT_GEN_TYPE_3  short
#define SAMPLESORT_GEN_TYPE_4  void
#define SAMPLESORT_GEN_SUFFIX  _p_l_s_v
#include "sort/samplesort/samplesort_gen.c"
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


//==========================================================================================================================================
//= Undefs
//==========================================================================================================================================


#undef metric_functions_templates

