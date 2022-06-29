#ifndef MATRIX_METRICS_H
#define MATRIX_METRICS_H

#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

#include "macros/cpp_defines.h"
#include "macros/macrolib.h"
#include "genlib.h"
#include "omp_functions.h"
#include "parallel_util.h"


__attribute__((unused))
static
void
MATRIX_METRICS_min_max(void * A, long N, double * min, double * max, double (* get_val_as_double)(void * A, long i))
{
	int num_threads = safe_omp_get_num_threads_next_par_region();
	double min_t[num_threads], max_t[num_threads];
	long i;
	#pragma omp parallel
	{
		int tnum = omp_get_thread_num();
		double min_partial, max_partial;
		long i;
		double val;
		min_partial = max_partial = get_val_as_double(A, 0);
		#pragma omp for schedule(static)
		for (i=0;i<N;i++)
		{
			val = get_val_as_double(A, i);
			if (val < min_partial)
				min_partial = val;
			if (val > max_partial)
				max_partial = val;
		}
		min_t[tnum] = min_partial;
		max_t[tnum] = max_partial;
	}
	*min = min_t[0];
	*max = max_t[0];
	for (i=0;i<num_threads;i++)
	{
		if (min_t[i] < *min)
			*min = min_t[i];
		if (max_t[i] > *max)
			*max = max_t[i];
	}
}

#define matrix_min_max(A, N, min_ptr, max_ptr, ... /* get_val_as_double() */)                                                   \
({                                                                                                                              \
	MATRIX_METRICS_min_max(A, N, min_ptr, max_ptr, DEFAULT_ARG_1(gen_basic_type_to_double_converter(A), ##__VA_ARGS__));    \
})


__attribute__((unused))
static
double
MATRIX_METRICS_mean(void * A, long N, double (* get_val_as_double)(void * A, long i))
{
	int num_threads = safe_omp_get_num_threads_next_par_region();
	double sums[num_threads];
	double sum = 0;
	long i;
	#pragma omp parallel
	{
		int tnum = omp_get_thread_num();
		long i;
		double t_sum = 0;
		#pragma omp for schedule(static)
		for (i=0;i<N;i++)
			t_sum += get_val_as_double(A, i);
		sums[tnum] = t_sum;
	}
	for (i=0;i<num_threads;i++)
		sum += sums[i];
	return sum / N;
}

#define matrix_mean(A, N, ... /* get_val_as_double() */)                                                   \
({                                                                                                         \
	MATRIX_METRICS_mean(A, N, DEFAULT_ARG_1(gen_basic_type_to_double_converter(A), ##__VA_ARGS__));    \
})


__attribute__((unused))
static
double
MATRIX_METRICS_var(void * A, long N, double mean, double (* get_val_as_double)(void * A, long i))
{
	int num_threads = safe_omp_get_num_threads_next_par_region();
	double sums[num_threads];
	double sum = 0;
	long i;
	#pragma omp parallel
	{
		int tnum = omp_get_thread_num();
		long i;
		double t_sum = 0, tmp;
		#pragma omp for schedule(static)
		for (i=0;i<N;i++)
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

#define matrix_var(A, N, mean, ... /* get_val_as_double() */)                                                   \
({                                                                                                              \
	MATRIX_METRICS_var(A, N, mean, DEFAULT_ARG_1(gen_basic_type_to_double_converter(A), ##__VA_ARGS__));    \
})

#define matrix_std(A, N, mean, ... /* get_val_as_double() */)    \
({                                                               \
	sqrt(matrix_var(A, N, mean, ##__VA_ARGS__));             \
})


// __attribute__((unused))
// static
// double
// MATRIX_METRICS_closest_pair_distance(void * A, long N)
// {
	// double * B = malloc(N * sizeof(*B));
	// double min;
	// if (N <= 1)
		// return 0;
	// int cmpfunc(const void * a, const void * b)
	// {
		// return (*(double *)a > *(double *)b) ? 1 : (*(double *)a < *(double *)b) ? -1 : 0;
	// }
	// for (i=0;i<N;i++)
		// B[i] = A[i];
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


#endif /* MATRIX_METRICS_H */

