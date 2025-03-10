#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <omp.h>
#include <complex.h>

#include "omp_functions.h"
#include "io.h"
#include "time_it.h"
#include "string_util.h"
#include "storage_formats/matrix_market/matrix_market.h"

#include "rcm_gen.h"


//==========================================================================================================================================
//= User Functions Declarations
//==========================================================================================================================================


//==========================================================================================================================================
//= Includes
//==========================================================================================================================================


#include "storage_formats/csr/csr_gen_push.h"
#define CSR_GEN_TYPE_1  RCM_GEN_TYPE_1
#define CSR_GEN_TYPE_2  RCM_GEN_TYPE_2
#define CSR_GEN_SUFFIX  CONCAT(_RCM_GEN, RCM_GEN_SUFFIX)
#include "storage_formats/csr/csr_gen.c"


#include "storage_formats/csr_util/csr_util_gen_push.h"
#define CSR_UTIL_GEN_TYPE_1  RCM_GEN_TYPE_1
#define CSR_UTIL_GEN_TYPE_2  RCM_GEN_TYPE_2
#define CSR_UTIL_GEN_SUFFIX  CONCAT(_RCM_GEN, RCM_GEN_SUFFIX)
#include "storage_formats/csr_util/csr_util_gen.c"


#undef  sort_aux_s
#define sort_aux_s  RCM_GEN_EXPAND(sort_aux_s)
struct sort_aux_s {
	RCM_GEN_TYPE_2 * row_ptr;
	RCM_GEN_TYPE_2 * positions;
};
#include "sort/samplesort/samplesort_gen_push.h"
#define SAMPLESORT_GEN_TYPE_1  RCM_GEN_TYPE_2
#define SAMPLESORT_GEN_TYPE_2  RCM_GEN_TYPE_2
#define SAMPLESORT_GEN_TYPE_3  RCM_GEN_TYPE_2
#define SAMPLESORT_GEN_TYPE_4  struct sort_aux_s
#define SAMPLESORT_GEN_SUFFIX  CONCAT(_RCM_GEN, RCM_GEN_SUFFIX)
#include "sort/samplesort/samplesort_gen.c"
int
samplesort_cmp(RCM_GEN_TYPE_2 a, RCM_GEN_TYPE_2 b, struct sort_aux_s * aux)
{
	int ret = 0;
	RCM_GEN_TYPE_2 pos_a = aux->positions[a];
	RCM_GEN_TYPE_2 pos_b = aux->positions[b];
	RCM_GEN_TYPE_2 degree_a = aux->row_ptr[a+1] - aux->row_ptr[a];
	RCM_GEN_TYPE_2 degree_b = aux->row_ptr[b+1] - aux->row_ptr[b];
	ret = pos_a > pos_b ? 1 : pos_a < pos_b ? -1 : 0;
	// Tie break: ascending vertex degree.
	if (ret == 0)
		ret = degree_a > degree_b ? 1 : degree_a < degree_b ? -1 : 0;
	// Tie break: ascending vertex id.
	if (ret == 0)
		ret = a > b ? 1 : a < b ? -1 : 0;
	return ret;
}


//==========================================================================================================================================
//= Local Defines
//==========================================================================================================================================


#undef  _TYPE_V
#define _TYPE_V  RCM_GEN_EXPAND_TYPE(_TYPE_V)
typedef RCM_GEN_TYPE_1  _TYPE_V;

#undef  _TYPE_I
#define _TYPE_I  RCM_GEN_EXPAND_TYPE(_TYPE_I)
typedef RCM_GEN_TYPE_2  _TYPE_I;


//==========================================================================================================================================
//------------------------------------------------------------------------------------------------------------------------------------------
//-                                                              Templates                                                                 -
//------------------------------------------------------------------------------------------------------------------------------------------
//==========================================================================================================================================


#undef  kv_pair
#define kv_pair  RCM_GEN_EXPAND(kv_pair)
struct kv_pair {
	_TYPE_I key;
	_TYPE_I val;
};


#undef  reduce_min
#define reduce_min  RCM_GEN_EXPAND(reduce_min)
static inline
struct kv_pair
reduce_min(struct kv_pair x, struct kv_pair y)
{
	struct kv_pair ret = x;
	ret = (x.val < y.val) ? x : (x.val > y.val) ? y : (x.key < y.key) ? x : y;
	return ret;
}


#undef  reduce_sum
#define reduce_sum  RCM_GEN_EXPAND(reduce_sum)
static inline
_TYPE_I
reduce_sum(_TYPE_I x, _TYPE_I y)
{
	return x + y;
}


#undef  reverse_cuthill_mckee
#define reverse_cuthill_mckee  RCM_GEN_EXPAND(reverse_cuthill_mckee)
void
reverse_cuthill_mckee(_TYPE_I * row_ptr, _TYPE_I * col_idx, [[gnu::unused]] _TYPE_V * values, long m, [[gnu::unused]] long n, [[gnu::unused]] long nnz,
	_TYPE_I ** reordered_row_ptr_ret, _TYPE_I ** reordered_col_idx_ret, _TYPE_V ** reordered_values_ret, _TYPE_I ** permutation_ret)
{
	int num_threads = safe_omp_get_num_threads();
	long R_n = 0, R_n_prev = 0;
	[[gnu::cleanup(cleanup_free)]] _TYPE_I * R = (typeof(R)) malloc(m * sizeof(*R));   // Visited vertices.
	[[gnu::cleanup(cleanup_free)]] _TYPE_I * position_in_R = (typeof(R)) malloc(m * sizeof(*R));
	[[gnu::cleanup(cleanup_free)]] char * A_flag = (typeof(A_flag)) malloc(m * sizeof(*A_flag));   // New frontier membership flags for each vertex.
	[[gnu::cleanup(cleanup_free)]] _TYPE_I * A = (typeof(A)) malloc(m * sizeof(*A));   // New frontier.
	[[gnu::cleanup(cleanup_free)]] _TYPE_I * A_min_pred_pos = (typeof(A_min_pred_pos)) malloc(m * sizeof(*A_min_pred_pos));   // New frontier vertices neighbour with minimum position in R.
	[[gnu::unused]] long i;

	if (m <= 0)
		error("empty matrix");
	if (m != n)
		error("matrix must be symmetric");

	if (reordered_row_ptr_ret == NULL)
		error("return variable is NULL: reordered_row_ptr_ret");
	if (reordered_col_idx_ret == NULL)
		error("return variable is NULL: reordered_col_idx_ret");
	if (reordered_values_ret == NULL)
		error("return variable is NULL: reordered_values_ret");
	if (permutation_ret == NULL)
		error("return variable is NULL: permutation_ret");

	/* First remove nodes that have <= 1 degree, which would degrade execution to quadratic (find node with minimun degree). */
	#pragma omp parallel
	{
		long i, pos;
		_TYPE_I degree;
		#pragma omp for
		for (i=0;i<m;i++)
		{
			R[i] = 0;
			position_in_R[i] = m;
			A_min_pred_pos[i] = m;
		}

		#pragma omp for
		for (i=0;i<m;i++)
		{
			degree = row_ptr[i+1] - row_ptr[i];
			if (degree <= 1)
			{
				pos = __atomic_fetch_add(&R_n, 1, __ATOMIC_RELAXED);
				R[pos] = i;
				position_in_R[i] = pos;
			}
		}
	}

	struct kv_pair v_min_degree = {.key = 0, .val = row_ptr[1] - row_ptr[0]};
	_TYPE_I t_num_v_base[num_threads + 1];
	#pragma omp parallel
	{
		int tnum = omp_get_thread_num();
		long i, i_s, i_e, j, j_e;
		_TYPE_I row, col, pos;
		_TYPE_I num_v_partial;
		_TYPE_I num_v_total = 0;
		struct kv_pair v_min_degree_t;
		struct kv_pair v_min_degree_partial = {.key = 0, .val = row_ptr[1] - row_ptr[0]};
		_TYPE_I degree;
		while (1)
		{
			#ifdef RCM_VERBOSE
				if (tnum == 0)
				{
					// for (i=0;i<m;i++)
					// {
						// if (A_flag[i])
							// printf("%ld, ", i);
					// }
					// printf("\n");
					printf("R_n_prev=%ld R_n=%ld, num_v_total = %d\n", R_n_prev, R_n, num_v_total);
				}
			#endif

			if (R_n == m)
				break;

			if (num_v_total == 0)   // Find a new starting node for a new connected component (isolated subgraphs).
			{
				v_min_degree_partial.key = m;
				v_min_degree_partial.val = nnz;
				#pragma omp for
				for (i=0;i<m;i++)
				{
					if (position_in_R[i] < m)   // Not already visited.
						continue;
					degree = row_ptr[i+1] - row_ptr[i];
					if (degree < v_min_degree_partial.val)
					{
						v_min_degree_partial.key = i;
						v_min_degree_partial.val = degree;
					}
					else if ((degree == v_min_degree_partial.val) && (i < v_min_degree_partial.key))   // For deterministic runs.
					{
						v_min_degree_partial.key = i;
						v_min_degree_partial.val = degree;
					}
				}

				omp_thread_reduce_global(reduce_min, v_min_degree_partial, , 0, 0, , &v_min_degree_t);

				if (tnum == 0)
				{
					v_min_degree = v_min_degree_t;
					R[R_n] = v_min_degree.key;
					position_in_R[v_min_degree.key] = R_n;
					R_n++;

					#ifdef RCM_VERBOSE
						printf("min degree %d -> %d\n", v_min_degree.key, v_min_degree.val);
					#endif
				}

				#pragma omp barrier
			}


			#pragma omp for
			for (i=0;i<m;i++)
			{
				A_flag[i] = 0;
			}

			/* Find the new frontier from the previous one, which is the nodes in R from R_n_prev to R_n. */
			#pragma omp for
			for (i=R_n_prev;i<R_n;i++)
			{
				row = R[i];
				for (j=row_ptr[row];j<row_ptr[row+1];j++)
				{
					col = col_idx[j];
					if (position_in_R[col] >= m)   // If not visited.
					{
						A_flag[col] = 1;
					}
				}
			}

			loop_partitioner_balance_iterations(num_threads, tnum, 0, m, &i_s, &i_e);
			for (i=i_s;i<i_e;i++)
			{
				if (A_flag[i])
					break;
			}
			i_s = i;

			num_v_partial = 0;
			for (i=i_s;i<i_e;i++)
			{
				if (A_flag[i])
					num_v_partial++;
			}
			omp_thread_reduce_global(reduce_sum, num_v_partial, 0, 1, 0,  &t_num_v_base[tnum], &num_v_total);   // 'omp_thread_reduce_global' needs no extra barrier.

			if (tnum == 0)
				t_num_v_base[num_threads] = num_v_total;

			#pragma omp barrier

			// printf("%d: num_v_partial=%ld, t_num_v_base[tnum,tnum+1]=(%ld,%ld), num_v_total=%ld\n", tnum, num_v_partial, t_num_v_base[tnum], t_num_v_base[tnum+1], num_v_total);
			// #pragma omp barrier

			if (num_v_total == 0)
			{
				R_n_prev = R_n;
				continue;
			}

			j = t_num_v_base[tnum];
			j_e = t_num_v_base[tnum+1];
			for (i=i_s;i<i_e && j<j_e;i++)
			{
				if (A_flag[i])
				{
					A[j] = i;
					j++;
				}
			}
			if (j != j_e)
				error("test");
			#pragma omp barrier

			/* Find the minimum predecessors (the already-visited neighbor with the earliest position in R). */
			#pragma omp for
			for (i=0;i<num_v_total;i++)
			{
				_TYPE_I row = A[i];
				_TYPE_I min_pred_pos = m;
				for (j=row_ptr[row];j<row_ptr[row+1];j++)
				{
					col = col_idx[j];
					pos = position_in_R[col];
					if ((pos < m) && (pos < min_pred_pos))
					{
						min_pred_pos = pos;
					}
				}
				if (min_pred_pos >= R_n)
					error("no predecesor found");
				A_min_pred_pos[row] = min_pred_pos;
			}

			/* Sort A by the minimum predecessor, ascending. */
			struct sort_aux_s aux;
			aux.row_ptr = row_ptr;
			aux.positions = A_min_pred_pos;
			samplesort_concurrent(A, num_v_total, &aux);

			/* Insert A in R. */
			#pragma omp for
			for (i=0;i<num_v_total;i++)
			{
				row = A[i];
				pos = R_n + i;
				R[pos] = row;
				position_in_R[row] = pos;
			}

			if (tnum == 0)
			{
				R_n_prev = R_n;
				R_n += num_v_total;
			}

			#pragma omp barrier
		}
	}
	if (R_n != m)
		error("did not process whole matrix: m=%d, R_n=%ld", m, R_n);

	_TYPE_I * permutation = (typeof(permutation)) malloc(R_n * sizeof(*permutation));
	#pragma omp parallel
	{
		long i;
		#pragma omp for
		for (i=0;i<m;i++)
			permutation[i] = -1;
	}
	for (i=0;i<R_n;i++)
	{
		if (R[i] < 0 || R[i] >= m)
			error("not a permutation: invalid positions");
		if (permutation[R[i]] >= 0)
			error("not a permutation: same positions");
		permutation[R[i]] = m - 1 - i;   // Reverse Cuthill-Mckee.
	}
	for (i=0;i<m;i++)
	{
		if (permutation[i] < 0)
			error("not a permutation: missing positions");
	}

	_TYPE_I * reordered_row_ptr = (typeof(reordered_row_ptr)) malloc((m+1) * sizeof(*reordered_row_ptr));
	_TYPE_I * reordered_col_idx = (typeof(reordered_col_idx)) malloc(nnz * sizeof(*reordered_col_idx));
	_TYPE_V * reordered_values = (typeof(reordered_values)) malloc(nnz * sizeof(*reordered_values));

	csr_reorder_rows(permutation, row_ptr, col_idx, values, m, n, nnz, reordered_row_ptr, reordered_col_idx, reordered_values);

	#pragma omp parallel
	{
		long i;
		#pragma omp for
		for (i=0;i<nnz;i++)
		{
			reordered_col_idx[i] = permutation[reordered_col_idx[i]];
			if (reordered_col_idx[i] < 0 || reordered_col_idx[i] >= m)
				error("bad column index");
		}
	}

	*reordered_row_ptr_ret = reordered_row_ptr;
	*reordered_col_idx_ret = reordered_col_idx;
	*reordered_values_ret = reordered_values;
	*permutation_ret = permutation;
}


//==========================================================================================================================================
//= Includes Undefs
//==========================================================================================================================================


#include "storage_formats/csr/csr_gen_pop.h"
#include "storage_formats/csr_util/csr_util_gen_pop.h"
#include "sort/samplesort/samplesort_gen_pop.h"

