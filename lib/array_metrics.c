#ifndef _GNU_SOURCE
	#define _GNU_SOURCE
#endif
#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

#include "macros/cpp_defines.h"
#include "debug.h"
#include "macros/macrolib.h"
#include "genlib.h"
#include "omp_functions.h"
#include "parallel_util.h"

#include "array_metrics.h"


//==========================================================================================================================================
//= Min - Max
//==========================================================================================================================================


void
ARRAY_METRICS_min_max_idx_serial(void * A, long i_start, long i_end, long * min_idx_out, long * max_idx_out, double (* get_val_as_double)(void * A, long i))
{
	long min_idx, max_idx;
	double min, max;
	long i;
	double val;
	min_idx = max_idx = 0;
	min = max = get_val_as_double(A, 0);
	for (i=i_start;i<i_end;i++)
	{
		val = get_val_as_double(A, i);
		if (val < min)
		{
			min_idx = i;
			min = val;
		}
		if (val > max)
		{
			max_idx = i;
			max = val;
		}
	}
	if (min_idx_out != NULL)
		*min_idx_out = min_idx;
	if (max_idx_out != NULL)
		*max_idx_out = max_idx;
}


void
ARRAY_METRICS_min_max_idx(void * A, long i_start, long i_end, long * min_idx_out, long * max_idx_out, double (* get_val_as_double)(void * A, long i))
{
	int num_threads = safe_omp_get_num_threads_external();
	long min_idx_t[num_threads], max_idx_t[num_threads];
	double min_t[num_threads], max_t[num_threads];
	double min, max;
	long min_idx, max_idx;
	long i;
	if (omp_get_level() > 0)
	{
		ARRAY_METRICS_min_max_idx_serial(A, i_start, i_end, min_idx_out, max_idx_out, get_val_as_double);
		return;
	}
	#pragma omp parallel
	{
		int tnum = omp_get_thread_num();
		long i_s, i_e;
		loop_partitioner_balance_iterations(num_threads, tnum, i_start, i_end, &i_s, &i_e);
		ARRAY_METRICS_min_max_idx_serial(A, i_s, i_e, &min_idx_t[tnum], &max_idx_t[tnum], get_val_as_double);
		min_t[tnum] = get_val_as_double(A, min_idx_t[tnum]);
		max_t[tnum] = get_val_as_double(A, max_idx_t[tnum]);
	}
	min_idx = min_idx_t[0];
	max_idx = max_idx_t[0];
	min = min_t[0];
	max = max_t[0];
	for (i=1;i<num_threads;i++)
	{
		if (min_t[i] < min)
		{
			min = min_t[i];
			min_idx = min_idx_t[i];
		}
		if (max_t[i] > max)
		{
			max = max_t[i];
			max_idx = max_idx_t[i];
		}
	}
	if (min_idx_out != NULL)
		*min_idx_out = min_idx;
	if (max_idx_out != NULL)
		*max_idx_out = max_idx;
}


void
ARRAY_METRICS_min_max_serial(void * A, long i_start, long i_end, double * min_out, double * max_out, double (* get_val_as_double)(void * A, long i))
{
	long min_idx, max_idx;
	ARRAY_METRICS_min_max_idx_serial(A, i_start, i_end, &min_idx, &max_idx, get_val_as_double);
	if (min_out != NULL)
		*min_out = get_val_as_double(A, min_idx);
	if (max_out != NULL)
		*max_out = get_val_as_double(A, max_idx);
}


void
ARRAY_METRICS_min_max(void * A, long i_start, long i_end, double * min_out, double * max_out, double (* get_val_as_double)(void * A, long i))
{
	long min_idx, max_idx;
	ARRAY_METRICS_min_max_idx(A, i_start, i_end, &min_idx, &max_idx, get_val_as_double);
	if (min_out != NULL)
		*min_out = get_val_as_double(A, min_idx);
	if (max_out != NULL)
		*max_out = get_val_as_double(A, max_idx);
}


//==========================================================================================================================================
//= Mean
//==========================================================================================================================================


double
ARRAY_METRICS_mean_serial(void * A, long i_start, long i_end, double (* get_val_as_double)(void * A, long i))
{
	long N = i_end - i_start;
	double sum = 0;
	long i;
	for (i=i_start;i<i_end;i++)
		sum += get_val_as_double(A, i);
	return sum / N;
}


double
ARRAY_METRICS_mean(void * A, long i_start, long i_end, double (* get_val_as_double)(void * A, long i))
{
	int num_threads = safe_omp_get_num_threads_external();
	long N = i_end - i_start;
	double sums[num_threads];
	double sum = 0;
	long i;
	if (omp_get_level() > 0)
		return ARRAY_METRICS_mean_serial(A, i_start, i_end, get_val_as_double);
	#pragma omp parallel
	{
		int tnum = omp_get_thread_num();
		long i;
		double t_sum = 0;
		#pragma omp for
		for (i=i_start;i<i_end;i++)
			t_sum += get_val_as_double(A, i);
		sums[tnum] = t_sum;
	}
	for (i=0;i<num_threads;i++)
		sum += sums[i];
	return sum / N;
}


//==========================================================================================================================================
//= Variance - Standard Deviation
//==========================================================================================================================================


double
ARRAY_METRICS_var_serial(void * A, long i_start, long i_end, double mean, double (* get_val_as_double)(void * A, long i))
{
	long N = i_end - i_start;
	double sum = 0;
	long i;
	double tmp;
	for (i=i_start;i<i_end;i++)
	{
		tmp = get_val_as_double(A, i) - mean;
		sum += tmp * tmp;
	}
	return sum / N;
}


double
ARRAY_METRICS_var(void * A, long i_start, long i_end, double mean, double (* get_val_as_double)(void * A, long i))
{
	int num_threads = safe_omp_get_num_threads_external();
	long N = i_end - i_start;
	double sums[num_threads];
	double sum = 0;
	long i;
	if (omp_get_level() > 0)
		return ARRAY_METRICS_var_serial(A, i_start, i_end, mean, get_val_as_double);
	#pragma omp parallel
	{
		int tnum = omp_get_thread_num();
		long i;
		double t_sum = 0, tmp;
		#pragma omp for
		for (i=i_start;i<i_end;i++)
		{
			tmp = get_val_as_double(A, i) - mean;
			t_sum += tmp * tmp;
		}
		sums[tnum] = t_sum;
	}
	for (i=0;i<num_threads;i++)
		sum += sums[i];
	return sum / N;
}


//==========================================================================================================================================
//= Closest Pair Distance
//==========================================================================================================================================


struct pair {
	double val;
	long idx;
};


double
ARRAY_METRICS_closest_pair_idx_serial(void * A, long i_start, long i_end, long * idx1_out, long * idx2_out, double (* get_val_as_double)(void * A, long i))
{
	long N = i_end - i_start;
	struct pair * P = (typeof(P)) malloc(N * sizeof(*P));;
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

// All these give the same times, so compiler seems to be able to optimize them.
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
	struct pair * P = (typeof(P)) malloc(N * sizeof(*P));;
	double min_t[num_threads];
	long pos_t[num_threads];
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
		min_t[tnum] = min;
		pos_t[tnum] = pos;
	}
	min = min_t[0];
	pos = pos_t[0];
	for (i=1;i<num_threads;i++)
	{
		if (min_t[i] < min)
		{
			min = min_t[i];
			pos = pos_t[i];
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

