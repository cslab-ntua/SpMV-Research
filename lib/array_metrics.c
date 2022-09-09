#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

#include "macros/cpp_defines.h"
#include "macros/macrolib.h"
#include "genlib.h"
#include "omp_functions.h"
#include "parallel_util.h"

#include "array_metrics.h"


//==========================================================================================================================================
//= Min - Max
//==========================================================================================================================================


void
ARRAY_METRICS_min_max_serial(void * A, long i_start, long i_end, double * min_out, double * max_out, double (* get_val_as_double)(void * A, long i))
{
	double min, max;
	long i;
	double val;
	min = max = get_val_as_double(A, 0);
	for (i=i_start;i<i_end;i++)
	{
		val = get_val_as_double(A, i);
		if (val < min)
			min = val;
		if (val > max)
			max = val;
	}
	if (min_out != NULL)
		*min_out = min;
	if (max_out != NULL)
		*max_out = max;
}


void
ARRAY_METRICS_min_max(void * A, long i_start, long i_end, double * min_out, double * max_out, double (* get_val_as_double)(void * A, long i))
{
	int num_threads = safe_omp_get_num_threads_next_par_region();
	double min_t[num_threads], max_t[num_threads];
	long i;
	#pragma omp parallel
	{
		int tnum = omp_get_thread_num();
		long i_s, i_e;
		loop_partitioner_balance_iterations(num_threads, tnum, i_start, i_end, &i_s, &i_e);
		ARRAY_METRICS_min_max_serial(A, i_s, i_e, &min_t[tnum], &max_t[tnum], get_val_as_double);
	}
	*min_out = min_t[0];
	*max_out = max_t[0];
	for (i=0;i<num_threads;i++)
	{
		if (min_out != NULL)
			if (min_t[i] < *min_out)
				*min_out = min_t[i];
		if (max_out != NULL)
			if (max_t[i] > *max_out)
				*max_out = max_t[i];
	}
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
	int num_threads = safe_omp_get_num_threads_next_par_region();
	long N = i_end - i_start;
	double sums[num_threads];
	double sum = 0;
	long i;
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
	int num_threads = safe_omp_get_num_threads_next_par_region();
	long N = i_end - i_start;
	double sums[num_threads];
	double sum = 0;
	long i;
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


// double
// ARRAY_METRICS_closest_pair_distance(void * A, long i_start, long i_end)
// {
	// long N = i_end - i_start;
	// double * B = malloc(N * sizeof(*B));
	// double min;
	// if (N <= 1)
		// return 0;
	// int cmpfunc(const void * a, const void * b)
	// {
		// return (*(double *)a > *(double *)b) ? 1 : (*(double *)a < *(double *)b) ? -1 : 0;
	// }
	// for (i=0;i<N;i++)
		// B[i] = A[i+i_start];
	// qsort(B, N, sizeof(*B), cmpfunc);
	// min = abs(B[1] - B[0]);
	// for (i=0;i<N-1;i++)
	// {
		// dist = abs(B[i+1] - B[i]);
		// if (dist < min)
			// min = dist;
	// }
	// free(B);
	// return min;
// }

