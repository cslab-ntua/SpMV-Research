#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

#include "macros/macrolib.h"
#include "debug.h"
#include "parallel_util.h"
#include "omp_functions.h"

#include "quicksort_gen.h"

// static long do_print = 0;
// static long do_print = 1;

// static long exit_after_one = 0;
// static long exit_after_one = 1;


//==========================================================================================================================================
//= User Functions Declarations
//==========================================================================================================================================


/* compare(a, b)
 *
 *  1 -> swapped positions (b, a)
 * -1 -> as is (a, b)
 *  0 -> random (equality)
 *
 * increasing order:
 *	(a > b) ?  1 : (a < b) ? -1 : 0
 * decreasing order:
 *	(a > b) ? -1 : (a < b) ?  1 : 0
 */
#undef  quicksort_cmp
#define quicksort_cmp  QUICKSORT_GEN_EXPAND(quicksort_cmp)
static int quicksort_cmp(_TYPE_V a, _TYPE_V b, _TYPE_AD * aux_data);


//==========================================================================================================================================
//= Includes
//==========================================================================================================================================


#include "sort/partition/partition_gen_push.h"
#define PARTITION_GEN_TYPE_1  QUICKSORT_GEN_TYPE_1
#define PARTITION_GEN_TYPE_2  QUICKSORT_GEN_TYPE_2
#define PARTITION_GEN_TYPE_3  QUICKSORT_GEN_TYPE_3
#define PARTITION_GEN_SUFFIX  CONCAT(_QUICKSORT_GEN, QUICKSORT_GEN_SUFFIX)
#include "sort/partition/partition_gen.c"
static inline
int
partition_cmp(_TYPE_V a, _TYPE_V b, _TYPE_AD * aux_data)
{
	return quicksort_cmp(a, b, aux_data);
}


//==========================================================================================================================================
//= Local Defines
//==========================================================================================================================================


#undef  _TYPE_V
#define _TYPE_V  QUICKSORT_GEN_EXPAND(_TYPE_V)
typedef QUICKSORT_GEN_TYPE_1  _TYPE_V;

#undef  _TYPE_I
#define _TYPE_I  QUICKSORT_GEN_EXPAND(_TYPE_I)
typedef QUICKSORT_GEN_TYPE_2  _TYPE_I;

#undef  _TYPE_AD
#define _TYPE_AD  QUICKSORT_GEN_EXPAND(_TYPE_AD)
typedef QUICKSORT_GEN_TYPE_3  _TYPE_AD;


//==========================================================================================================================================
//------------------------------------------------------------------------------------------------------------------------------------------
//-                                                              Templates                                                                 -
//------------------------------------------------------------------------------------------------------------------------------------------
//==========================================================================================================================================


//==========================================================================================================================================
//= Quicksort
//==========================================================================================================================================


#undef  quicksort_no_malloc
#define quicksort_no_malloc  QUICKSORT_GEN_EXPAND(quicksort_no_malloc)
static inline
void
quicksort_no_malloc(_TYPE_V * A, long N, _TYPE_AD * aux_data, _TYPE_I * partitions_buf)
{
	long s, m, e;
	long i;

	size_t statelen = 256;
	char statebuf[statelen];
	struct random_data buf;

	if (N < 2)
		return;

	buf.state = NULL;
	initstate_r(0, statebuf, statelen, &buf);         // Before calling this function, the buf.state field must be initialized to NULL.
	srandom_r(0, &buf);

	s = 0;
	e = N - 1;
	m = 0;
	i = 0;
	while (1)
	{
		while (s >= e)
		{
			if (s == 0)
				return;
			i--;
			e--;
			s = partitions_buf[i];
		}
		m = partition_auto_serial(A, s, e+1, aux_data);
		partitions_buf[i++] = s;
		s = m;
	}
}


#undef  quicksort
#define quicksort  QUICKSORT_GEN_EXPAND(quicksort)
void
quicksort(_TYPE_V * A, long N, _TYPE_AD * aux_data, _TYPE_I * partitions_buf)
{
	_TYPE_I * partitions_buf_tmp;
	if (N < 2)
		return;
	partitions_buf_tmp = (partitions_buf != NULL) ? partitions_buf : (typeof(partitions_buf_tmp)) malloc(N * sizeof(*partitions_buf_tmp));
	quicksort_no_malloc(A, N, aux_data, partitions_buf_tmp);
	if (partitions_buf == NULL)
		free(partitions_buf_tmp);
}


#undef  quicksort_no_malloc_parallel
#define quicksort_no_malloc_parallel  QUICKSORT_GEN_EXPAND(quicksort_no_malloc_parallel)
static inline
void
quicksort_no_malloc_parallel(_TYPE_V * A, __attribute__((unused)) _TYPE_V * buf, long N, _TYPE_AD * aux_data, _TYPE_I * partitions_buf)
{
	long s, m, e;
	long i;
	if (N < 2)
		return;
	s = 0;
	e = N - 1;
	m = 0;
	i = 0;

		// if (do_print)
		// {
			// printf("INIT\n");
			// printf(" ");
			// for (long i=0;i<N;i++)
			// {
				// printf("%2ld  ", i);
			// }
			// printf("\n");
			// printf("[");
			// for (long i=0;i<N;i++)
			// {
				// printf("%2d, ", A[i]);
			// }
			// printf("]\n");
		// }

	while (1)
	{
		while (s >= e)
		{
			if (s == 0)
				return;
			i--;
			e--;
			s = partitions_buf[i];
		}

		// if (do_print)
		// {
			// printf("BEFORE\n");
			// printf("s = %ld , e = %ld\n", s, e);
			// printf("pivot_pos = %ld , pivot = %d\n", pivot_pos, pivot);
			// printf(" ");
			// for (long i=0;i<N;i++)
			// {
				// printf("%2ld  ", i);
			// }
			// printf("\n");
			// printf("[");
			// for (long i=0;i<N;i++)
			// {
				// printf("%2d, ", A[i]);
			// }
			// printf("]\n");
		// }

		if (__builtin_expect(e - s > (1LL << 10), 0))
		// if (1)
		{
			// printf("%ld\n", i);
			// fprintf(stderr, "in\n");
			#pragma omp parallel
			{
				if (buf == NULL)
					m = partition_auto_concurrent(A, s, e+1, aux_data, 1, NULL);
				else
					m = partition_auto_concurrent(A, s, e+1, aux_data, 0, buf);
			}

		}
		else
		{
			m = partition_auto_serial(A, s, e+1, aux_data);
		}

		// if (do_print)
		// {
			// printf("AFTER\n");
			// printf("pivot_pos = %ld , pivot = %d\n", pivot_pos, pivot);
			// printf("m = %ld\n", m);
			// printf(" ");
			// for (long i=0;i<N;i++)
			// {
				// printf("%2ld  ", i);
			// }
			// printf("\n");
			// printf("[");
			// for (long i=0;i<N;i++)
			// {
				// printf("%2d, ", A[i]);
			// }
			// printf("]\n");
		// }

		// if (do_print)
		// {
			// printf("AFTER 2\n");
			// printf("m = %ld\n", m);
			// printf(" ");
			// for (long i=0;i<N;i++)
			// {
				// printf("%2ld  ", i);
			// }
			// printf("\n");
			// printf("[");
			// for (long i=0;i<N;i++)
			// {
				// printf("%2d, ", A[i]);
			// }
			// printf("]\n");
			// printf("\n");
		// }

		// if (exit_after_one)
			// exit(0);

		partitions_buf[i++] = s;
		s = m;
	}
}


#undef  quicksort_parallel
#define quicksort_parallel  QUICKSORT_GEN_EXPAND(quicksort_parallel)
void
quicksort_parallel(_TYPE_V * A, long N, _TYPE_AD * aux_data, _TYPE_I * partitions_buf)
{
	_TYPE_V * buf;
	_TYPE_I * partitions_buf_tmp;
	if (N < 2)
		return;
	buf = (typeof(buf)) malloc(N * sizeof(*buf));
	partitions_buf_tmp = (partitions_buf != NULL) ? partitions_buf : (typeof(partitions_buf_tmp)) malloc(N * sizeof(*partitions_buf_tmp));
	quicksort_no_malloc_parallel(A, buf, N, aux_data, partitions_buf_tmp);
	free(buf);
	if (partitions_buf == NULL)
		free(partitions_buf_tmp);
}


#undef  quicksort_parallel_inplace
#define quicksort_parallel_inplace  QUICKSORT_GEN_EXPAND(quicksort_parallel_inplace)
void
quicksort_parallel_inplace(_TYPE_V * A, long N, _TYPE_AD * aux_data, _TYPE_I * partitions_buf)
{
	_TYPE_I * partitions_buf_tmp;
	if (N < 2)
		return;
	partitions_buf_tmp = (partitions_buf != NULL) ? partitions_buf : (typeof(partitions_buf_tmp)) malloc(N * sizeof(*partitions_buf_tmp));
	quicksort_no_malloc_parallel(A, NULL, N, aux_data, partitions_buf_tmp);
	if (partitions_buf == NULL)
		free(partitions_buf_tmp);
}


#undef  insertionsort
#define insertionsort  QUICKSORT_GEN_EXPAND(insertionsort)
void
insertionsort(_TYPE_V * A, long N, _TYPE_AD * aux_data)
{
	long i, i_e;
	for (i_e=N-1;i_e>0;i_e--)
	{
		for (i=0;i<i_e;i++)
		{
			if (quicksort_cmp(A[i], A[i+1], aux_data) > 0)
				SWAP(&A[i], &A[i+1]);
		}
	}
}

