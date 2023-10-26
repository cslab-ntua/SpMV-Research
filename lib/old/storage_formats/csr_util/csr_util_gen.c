#include <stdlib.h>
#include <stdio.h>

#include "macros/macrolib.h"
#include "debug.h"
#include "omp_functions.h"
#include "parallel_util.h"
#include "string_util.h"
#include "array_metrics.h"
#include "plot/plot.h"
#include "time_it.h"

#include "csr_util_gen.h"


/* Notes: 
 *     - We assume that the matrix is FULLY sorted, i.e. both rows and columns.
 */

/* features_all = ['A_mem_footprint',
 *              'avg_nz_row',
 *              'skew_coeff',
 *              'avg_num_neighbours',
 *              'avg_cross_row_similarity',
 *
 *              'm','n','nz',
 *              'density',
 *              'min_nz_row', 'max_nz_row', 'std_nz_row',
 *              'min_nz_col', 'max_nz_col', 'avg_nz_col', 'std_nz_col',
 *              'min_bandwidth', 'max_bandwidth', 'avg_bandwidth', 'std_bandwidth',
 *              'min_scattering', 'max_scattering', 'avg_scattering', 'std_scattering',
 *              'min_ngroups', 'max_ngroups', 'avg_ngroups', 'std_ngroups',
 *              'min_dis', 'max_dis', 'avg_dis', 'std_dis',
 *              'min_clustering', 'max_clustering', 'avg_clustering', 'std_clustering',
 *              'min_num_neighbours','max_num_neighbours', 'std_num_neighbours',
 *              'min_cross_row_similarity', 'max_cross_row_similarity', 'std_cross_row_similarity'
 *         ]
 */

//==========================================================================================================================================
//= User Functions Declarations
//==========================================================================================================================================


//==========================================================================================================================================
//= Includes
//==========================================================================================================================================


//------------------------------------------------------------------------------------------------------------------------------------------
//- Reduce Add
//------------------------------------------------------------------------------------------------------------------------------------------

#include "functools/functools_gen_push.h"
#define FUNCTOOLS_GEN_TYPE_1  double
#define FUNCTOOLS_GEN_TYPE_2  double
#define FUNCTOOLS_GEN_SUFFIX  CONCAT(_CSR_UTIL_GEN_add_d, CSR_UTIL_GEN_SUFFIX)
#include "functools/functools_gen.c"

__attribute__((pure))
static inline
double
functools_map_fun(double * A, long i)
{
	return A[i];
}

__attribute__((pure))
static inline
double
functools_reduce_fun(double a, double b)
{
	return a + b;
}


//------------------------------------------------------------------------------------------------------------------------------------------
//- Quicksort
//------------------------------------------------------------------------------------------------------------------------------------------

#include "sort/quicksort/quicksort_gen_push.h"
#define QUICKSORT_GEN_TYPE_1  double
#define QUICKSORT_GEN_TYPE_2  int
#define QUICKSORT_GEN_TYPE_3  void
#define QUICKSORT_GEN_SUFFIX  CONCAT(_CSR_UTIL_GEN_d, CSR_UTIL_GEN_SUFFIX)
#include "sort/quicksort/quicksort_gen.c"

static inline
int
quicksort_cmp(double a, double b, __attribute__((unused)) void * aux)
{
	return (a > b) ? 1 : (a < b) ? -1 : 0;
}


//------------------------------------------------------------------------------------------------------------------------------------------
//- Samplesort
//------------------------------------------------------------------------------------------------------------------------------------------

#include "sort/samplesort/samplesort_gen_push.h"
#define SAMPLESORT_GEN_TYPE_1  double
#define SAMPLESORT_GEN_TYPE_2  int
#define SAMPLESORT_GEN_TYPE_3  int
#define SAMPLESORT_GEN_TYPE_4  void
#define SAMPLESORT_GEN_SUFFIX  CONCAT(_CSR_UTIL_GEN_d, CSR_UTIL_GEN_SUFFIX)
#include "sort/samplesort/samplesort_gen.c"

static inline
int
samplesort_cmp(double a, double b, __attribute__((unused)) void * aux)
{
	return (a > b) ? 1 : (a < b) ? -1 : 0;
}


//==========================================================================================================================================
//= Local Defines
//==========================================================================================================================================


#undef  _TYPE_V
#define _TYPE_V  CSR_UTIL_GEN_EXPAND(_TYPE_V)
typedef CSR_UTIL_GEN_TYPE_1  _TYPE_V;

#undef  _TYPE_I
#define _TYPE_I  CSR_UTIL_GEN_EXPAND(_TYPE_I)
typedef CSR_UTIL_GEN_TYPE_2  _TYPE_I;


//==========================================================================================================================================
//------------------------------------------------------------------------------------------------------------------------------------------
//-                                                              Templates                                                                 -
//------------------------------------------------------------------------------------------------------------------------------------------
//==========================================================================================================================================


//==========================================================================================================================================
//= Structural Features
//==========================================================================================================================================


// Expand row indexes.
#undef  csr_row_indexes
#define csr_row_indexes  CSR_UTIL_GEN_EXPAND(csr_row_indexes)
void
csr_row_indexes(_TYPE_I * row_ptr, __attribute__((unused)) _TYPE_I * col_idx, long m, __attribute__((unused)) long n, long nnz,
		_TYPE_I ** row_idx_out)
{
	_TYPE_I * row_idx;
	row_idx = (typeof(row_idx)) malloc(nnz * sizeof(*row_idx));
	#pragma omp parallel
	{
		long i, j;
		#pragma omp for
		for (i=0;i<m;i++)
			for (j=row_ptr[i];j<row_ptr[i+1];j++)
				row_idx[j] = i;
	}
	*row_idx_out = row_idx;
}


// Average nnz distances per row is: bandwidths / degrees_rows .
#undef  csr_degrees_bandwidths_scatters
#define csr_degrees_bandwidths_scatters  CSR_UTIL_GEN_EXPAND(csr_degrees_bandwidths_scatters)
void
csr_degrees_bandwidths_scatters(_TYPE_I * row_ptr, _TYPE_I * col_idx, long m, long n, long nnz,
		_TYPE_I ** degrees_rows_out, _TYPE_I ** degrees_cols_out, double ** bandwidths_out, double ** scatters_out)
{
	int num_threads = safe_omp_get_num_threads_external();
	_TYPE_I * degrees_rows = (typeof(degrees_rows)) malloc(m * sizeof(*degrees_rows));
	_TYPE_I * degrees_cols = (typeof(degrees_cols)) malloc(n * sizeof(*degrees_cols));
	double * bandwidths = (typeof(bandwidths)) malloc(m * sizeof(*bandwidths));
	double * scatters = (typeof(scatters)) malloc(m * sizeof(*scatters));
	#pragma omp parallel
	{
		int tnum = omp_get_thread_num();
		long i, i_s, i_e, j, j_s, j_e, degree;
		double b, s;
		loop_partitioner_balance_prefix_sums(num_threads, tnum, row_ptr, m, nnz, &i_s, &i_e);
		#pragma omp for
		for (j=0;j<n;j++)
			degrees_cols[j] = 0;
		for (i=i_s;i<i_e;i++)
		{
			j_s = row_ptr[i];
			j_e = row_ptr[i+1];
			degree = j_e - j_s;
			degrees_rows[i] = degree;
			bandwidths[i] = 0;
			scatters[i] = 0;
			if (degree == 0)
				continue;
			b = col_idx[j_e-1] - col_idx[j_s];
			bandwidths[i] = b;
			s = (b > 0) ? degree / b : 0;
			scatters[i] = s;
			for (j=j_s;j<j_e;j++)
				__atomic_fetch_add(&degrees_cols[col_idx[j]], 1, __ATOMIC_RELAXED);
		}
	}
	if (degrees_rows_out != NULL)
		*degrees_rows_out = degrees_rows;
	else
		free(degrees_rows);
	if (degrees_cols_out != NULL)
		*degrees_cols_out = degrees_cols;
	else
		free(degrees_cols);
	if (bandwidths_out != NULL)
		*bandwidths_out = bandwidths;
	else
		free(bandwidths);
	if (scatters_out != NULL)
		*scatters_out = scatters;
	else
		free(scatters);
}


// #undef  csr_groups_per_row
// #define csr_groups_per_row  CSR_UTIL_GEN_EXPAND(csr_groups_per_row)
// void
// csr_groups_per_row(_TYPE_I * row_ptr, _TYPE_I * col_idx, long m, __attribute__((unused)) long n, __attribute__((unused)) long nnz, long max_gap_size,
		// long ** groups_per_row_out)
// {
	// long * groups_per_row = (typeof(groups_per_row)) malloc(m * sizeof(*groups_per_row));
	// #pragma omp parallel
	// {
		// long i, j, k, degree;
		// long num_groups;
		// #pragma omp for
		// for (i=0;i<m;i++)
		// {
			// groups_per_row[i] = 0;
			// degree = row_ptr[i+1] - row_ptr[i];
			// if (degree <= 0)
				// continue;
			// j = row_ptr[i];
			// num_groups = 0;
			// while (j < row_ptr[i+1])
			// {
				// k = j + 1;
				// while ((k < row_ptr[i+1]) && (col_idx[k] - col_idx[k-1] <= max_gap_size + 1))   // distance 1 means gap 0
					// k++;
				// num_groups++;
				// j = k;
			// }
			// groups_per_row[i] = num_groups;
		// }
	// }
	// if (groups_per_row_out != NULL)
		// *groups_per_row_out = groups_per_row;
	// else
		// free(groups_per_row);
// }


/* Nonzero column distances are not confined inside each row, meaning for the last nnz of a row
 * we calculate the column distance (absolute value) with the first nonzero of the next non-empty row.
 * Same with group column distances.
 */

#undef  csr_column_distances_and_groupping
#define csr_column_distances_and_groupping  CSR_UTIL_GEN_EXPAND(csr_column_distances_and_groupping)
long
csr_column_distances_and_groupping(_TYPE_I * row_ptr, _TYPE_I * col_idx, long m, __attribute__((unused)) long n, long nnz, long max_gap_size,
		_TYPE_I ** nnz_col_dist_out, _TYPE_I ** group_col_dist_out, _TYPE_I ** group_sizes_out, _TYPE_I ** groups_per_row_out)
{
	int num_threads = safe_omp_get_num_threads_external();
	_TYPE_I * col_idx_ext = (typeof(col_idx_ext)) malloc((nnz+1) * sizeof(*col_idx_ext)); // Helper col_idx with one more item at the end so that differences are always valid.
	_TYPE_I * nnz_col_dist = (typeof(nnz_col_dist)) malloc(nnz * sizeof(*nnz_col_dist));
	_TYPE_I * group_col_dist_tmp = (typeof(group_col_dist_tmp)) malloc(nnz * sizeof(*group_col_dist_tmp));
	_TYPE_I * group_col_dist;
	_TYPE_I * group_sizes_tmp = (typeof(group_sizes_tmp)) malloc(nnz * sizeof(*group_sizes_tmp));
	_TYPE_I * group_sizes;
	_TYPE_I * groups_per_row = (typeof(groups_per_row)) malloc(m * sizeof(*groups_per_row));
	long t_nnz[num_threads];
	long t_total_groups[num_threads];
	long total_groups = 0;
	if (nnz == 0)
		error("nnz is zero");
	#pragma omp parallel
	{
		int tnum = omp_get_thread_num();
		long i, i_s, i_e, j, k, degree, dist;
		long row_groups;
		long total_groups_t = 0;
		long private_nnz;
		long base_nnz;
		#pragma omp for
		for (j=0;j<nnz;j++)
			col_idx_ext[j] = col_idx[j];
		col_idx_ext[nnz] = col_idx_ext[nnz-1];
		loop_partitioner_balance_prefix_sums(num_threads, tnum, row_ptr, m, nnz, &i_s, &i_e);
		private_nnz = row_ptr[i_e] - row_ptr[i_s];
		__atomic_store_n(&t_nnz[tnum], private_nnz, __ATOMIC_RELAXED);
		#pragma omp barrier
		#pragma omp single nowait
		{
			long total, tmp;
			total = 0;
			for (i=0;i<num_threads;i++)
			{
				tmp = t_nnz[i];
				t_nnz[i] = total;
				total += tmp;
			}
		}
		#pragma omp barrier
		base_nnz = __atomic_load_n(&t_nnz[tnum], __ATOMIC_RELAXED);
		for (i=i_s;i<i_e;i++)
		{
			groups_per_row[i] = 0;
			degree = row_ptr[i+1] - row_ptr[i];
			if (degree <= 0)
				continue;
			j = row_ptr[i];
			row_groups = 0;
			while (j < row_ptr[i+1])
			{
				k = 1;
				while (j+1 < row_ptr[i+1])   // distance 1 means gap 0
				{
					dist = abs(col_idx_ext[j+1] - col_idx_ext[j]);
					nnz_col_dist[j] = dist;
					if (dist > max_gap_size + 1)
						break;
					k++;
					j++;
				}
				dist = abs(col_idx_ext[j+1] - col_idx_ext[j]);
				nnz_col_dist[j] = dist;
				group_col_dist_tmp[base_nnz + total_groups_t] = dist;
				group_sizes_tmp[base_nnz + total_groups_t] = k;
				row_groups++;
				total_groups_t++;
				j++;
			}
			groups_per_row[i] = row_groups;
		}
		__atomic_store_n(&t_total_groups[tnum], total_groups_t, __ATOMIC_RELAXED);
		#pragma omp barrier
		#pragma omp single nowait
		{
			long tmp;
			total_groups = 0;
			for (i=0;i<num_threads;i++)
			{
				tmp = t_total_groups[i];
				t_total_groups[i] = total_groups;
				total_groups += tmp;
			}
			group_col_dist = (typeof(group_col_dist)) malloc(total_groups * sizeof(*group_col_dist));
			group_sizes = (typeof(group_sizes)) malloc(total_groups * sizeof(*group_sizes));
		}
		#pragma omp barrier
		// printf("%d: %ld %ld\n", tnum, t_total_groups[tnum], t_total_groups[tnum] + total_groups_t);
		for (i=0;i<total_groups_t;i++)
		{
			group_col_dist[t_total_groups[tnum] + i] = group_col_dist_tmp[base_nnz + i];
			group_sizes[t_total_groups[tnum] + i] = group_sizes_tmp[base_nnz + i];
		}
	}
	free(col_idx_ext);
	free(group_col_dist_tmp);
	free(group_sizes_tmp);
	if (nnz_col_dist_out != NULL)
		*nnz_col_dist_out = nnz_col_dist;
	if (group_col_dist_out != NULL)
		*group_col_dist_out = group_col_dist;
	if (group_sizes_out != NULL)
		*group_sizes_out = group_sizes;
	if (groups_per_row_out != NULL)
		*groups_per_row_out = groups_per_row;
	return total_groups;
}


/* Notes: 
 *     - 'window_size': Distance from left and right.
 */
#undef  csr_row_neighbours
#define csr_row_neighbours  CSR_UTIL_GEN_EXPAND(csr_row_neighbours)
void
csr_row_neighbours(_TYPE_I * row_ptr, _TYPE_I * col_idx, long m, __attribute__((unused)) long n, long nnz, long window_size,
		_TYPE_I ** num_neigh_out)
{
	int num_threads = safe_omp_get_num_threads_external();
	_TYPE_I * num_neigh = (typeof(num_neigh)) malloc(nnz * sizeof(*num_neigh));
	#pragma omp parallel
	{
		int tnum = omp_get_thread_num();
		long i, i_s, i_e, j, k;
		loop_partitioner_balance_prefix_sums(num_threads, tnum, row_ptr, m, nnz, &i_s, &i_e);
		#pragma omp for
		for (i=0;i<nnz;i++)
			num_neigh[i] = 0;
		for (i=i_s;i<i_e;i++)
		{
			for (j=row_ptr[i];j<row_ptr[i+1];j++)
			{
				for (k=j+1;k<row_ptr[i+1];k++)
				{
					if (col_idx[k] - col_idx[j] > window_size)
						break;
					num_neigh[j]++;
					num_neigh[k]++;
				}
			}
		}
	}
	if (num_neigh_out != NULL)
		*num_neigh_out = num_neigh;
	else
		free(num_neigh);
}


/* For similarities each non-empty row is equivalent, no matter the degree.
 */
#undef  csr_cross_row_similarity
#define csr_cross_row_similarity  CSR_UTIL_GEN_EXPAND(csr_cross_row_similarity)
double
csr_cross_row_similarity(_TYPE_I * row_ptr, _TYPE_I * col_idx, long m, __attribute__((unused)) long n, __attribute__((unused)) long nnz, long window_size)
{
	int num_threads = safe_omp_get_num_threads_external();
	double t_row_similarity[num_threads];
	double total_row_similarity;
	long total_num_non_empty_rows = 0;
	#pragma omp parallel
	{
		int tnum = omp_get_thread_num();
		long i, j, k, k_s, k_e, l;
		long degree, num_similarities, column_diff;
		double row_similarity = 0;
		long num_non_empty_rows = 0;
		#pragma omp for
		for (i=0;i<m;i++)
		{
			degree = row_ptr[i+1] - row_ptr[i];
			if (degree <= 0)
				continue;
			for (l=i+1;l<m;l++)       // Find next non-empty row.
				if (row_ptr[l+1] - row_ptr[l] > 0)
					break;
			num_non_empty_rows++;
			if (l < m)
			{
				k_s = row_ptr[l];
				k_e = row_ptr[l+1];
				num_similarities = 0;
				k = k_s;
				for (j=row_ptr[i];j<row_ptr[i+1];j++)
				{
					while (k < k_e)
					{
						column_diff = col_idx[k] - col_idx[j];
						if (labs(column_diff) <= window_size)
						{
							num_similarities++;
							break;
						}
						if (column_diff <= 0)
							k++;
						else
							break;   // went outside of area to examine
					}
				}
				row_similarity += ((double) num_similarities) / degree;
			}
		}
		__atomic_store(&t_row_similarity[tnum], &row_similarity, __ATOMIC_RELAXED);    // '__atomic_store_n' is for integer types only.
		__atomic_fetch_add(&total_num_non_empty_rows, num_non_empty_rows, __ATOMIC_RELAXED);
	}
	if (total_num_non_empty_rows == 0)
		return 0;
	total_row_similarity = 0;
	for (long i=0;i<num_threads;i++)
		total_row_similarity += t_row_similarity[i];
	return total_row_similarity / total_num_non_empty_rows;   // Neighbours are only between non-empty rows.
}


#undef  csr_cross_row_neighbours
#define csr_cross_row_neighbours  CSR_UTIL_GEN_EXPAND(csr_cross_row_neighbours)
double
csr_cross_row_neighbours(_TYPE_I * row_ptr, _TYPE_I * col_idx, long m, __attribute__((unused)) long n, __attribute__((unused)) long nnz, long window_size)
{
	double * num_neigh = (typeof(num_neigh)) malloc((m) * sizeof(*num_neigh));
	long total_num_non_empty_rows = 0;
	#pragma omp parallel
	{
		long i, j, k, k_s, k_e, l;
		long degree, curr_neigh, column_diff;
		long num_non_empty_rows = 0;
		#pragma omp for
		for (i=0;i<m;i++)
			num_neigh[i] = 0;
		#pragma omp for
		for (i=0;i<m;i++)
		{
			degree = row_ptr[i+1] - row_ptr[i];
			if (degree <= 0)
				continue;
			for (l=i+1;l<m;l++)       // Find next non-empty row.
				if (row_ptr[l+1] - row_ptr[l] > 0)
					break;
			num_non_empty_rows++;
			if (l < m)
			{
				k_s = row_ptr[l];
				k_e = row_ptr[l+1];
				curr_neigh = 0;
				k = k_s;
				for (j=row_ptr[i];j<row_ptr[i+1];j++)
				{
					while (k < k_e)
					{
						column_diff = col_idx[k] - col_idx[j];
						if (labs(column_diff) <= window_size)
						{
							curr_neigh++;
							break;
						}
						if (column_diff <= 0)
							k++;
						else
							break;   // went outside of area to examine
					}
				}
				num_neigh[i] = ((double) curr_neigh) / degree;
			}
		}
		__atomic_fetch_add(&total_num_non_empty_rows, num_non_empty_rows, __ATOMIC_RELAXED);
	}
	double mean;
	array_mean(num_neigh, m, &mean);
	free(num_neigh);
	if (total_num_non_empty_rows == 0)
		return 0;
	return (mean * m) / total_num_non_empty_rows;   // Neighbours are only between non-empty rows.
}


//==========================================================================================================================================
//= Value Features
//==========================================================================================================================================


// #undef  disassemble_double
// #define disassemble_double  CSR_UTIL_GEN_EXPAND(disassemble_double)
// void
// disassemble_double(double f, uint64_t * sign_out, uint64_t * exp_out, uint64_t * frac_out)
// {
	// static const uint64_t mask_double_sign = 1ULL<<63;
	// static const uint64_t mask_double_exp  = ((1ULL<<11) - 1) << 52;
	// static const uint64_t mask_double_frac = (1ULL<<52) - 1;
	// uint64_t u;
	// u = cast_to_type_unsafe(f, u);
	// *sign_out = (u & mask_double_sign) >> 63;
	// *exp_out = (u & mask_double_exp) >> 52;
	// *exp_out -= 1023;
	// *frac_out = u & mask_double_frac;
// }


#undef  get_double_zero
#define get_double_zero  CSR_UTIL_GEN_EXPAND(get_double_zero)
static inline
double
get_double_zero(__attribute__((unused)) void * A, __attribute__((unused)) long i)
{
	return 0;
}


#undef  get_double_abs
#define get_double_abs  CSR_UTIL_GEN_EXPAND(get_double_abs)
static inline
double
get_double_abs(void * A, long i)
{
	return fabs(((double *) A)[i]);
}


#undef  get_double_sign
#define get_double_sign  CSR_UTIL_GEN_EXPAND(get_double_sign)
static inline
double
get_double_sign(void * A, long i)
{
	static const uint64_t mask = 1ULL<<63;
	uint64_t u = ((uint64_t *) A)[i];
	u = (u & mask) >> 63;
	return u;
}


#undef  get_double_exponent
#define get_double_exponent  CSR_UTIL_GEN_EXPAND(get_double_exponent)
static inline
double
get_double_exponent(void * A, long i)
{
	static const uint64_t mask  = ((1ULL<<11) - 1) << 52;
	uint64_t u = ((uint64_t *) A)[i];
	u = (u & mask) >> 52;
	// u -= 1023;
	if (u)
		u -= 1023;
	return (int64_t) u;
}


#undef  get_double_exponent_bits
#define get_double_exponent_bits  CSR_UTIL_GEN_EXPAND(get_double_exponent_bits)
static inline
double
get_double_exponent_bits(void * A, long i)
{
	static const uint64_t mask  = ((1ULL<<11) - 1) << 52;
	uint64_t u = ((uint64_t *) A)[i];
	u = (u & mask) >> 52;
	return u;
}


#undef  get_double_fraction
#define get_double_fraction  CSR_UTIL_GEN_EXPAND(get_double_fraction)
static inline
double
get_double_fraction(void * A, long i)
{
	static const uint64_t mask = (1ULL<<52) - 1;
	uint64_t u = ((uint64_t *) A)[i];
	u = u & mask;
	return u;
}


#undef  get_double_upper_12_bits
#define get_double_upper_12_bits  CSR_UTIL_GEN_EXPAND(get_double_upper_12_bits)
static inline
double
get_double_upper_12_bits(void * A, long i)
{
	// static const uint64_t mask  = ((1ULL<<12) - 1) << 52;
	uint64_t u = ((uint64_t *) A)[i];
	// u = (u & mask) >> 52;
	return u >> 52;
}


#undef  get_double_trailing_zeros
#define get_double_trailing_zeros  CSR_UTIL_GEN_EXPAND(get_double_trailing_zeros)
static inline
double
get_double_trailing_zeros(void * A, long i)
{
	uint64_t u = ((uint64_t *) A)[i];
	return u == 0 ? 64 : __builtin_ctzl(u);
}

#undef  reduce_add
#define reduce_add  CSR_UTIL_GEN_EXPAND(reduce_add)
static inline
double
reduce_add(double a, double b)
{
	return a + b;
}

#undef  get_window_min_ctz_as_double
#define get_window_min_ctz_as_double  CSR_UTIL_GEN_EXPAND(get_window_min_ctz_as_double)
static inline
double
get_window_min_ctz_as_double(void * A, long i, long len)
{
	uint64_t min;
	uint64_t ctz;
	long j;
	min = get_double_trailing_zeros(A, i);
	for (j=1;j<len;j++)
	{
		ctz = get_double_trailing_zeros(A, i + j);
		if (ctz < min)
			min = ctz;
	}
	return min * len;
}

#undef  array_mean_of_windowed_ctz_min
#define array_mean_of_windowed_ctz_min  CSR_UTIL_GEN_EXPAND(array_mean_of_windowed_ctz_min)
static inline
double
array_mean_of_windowed_ctz_min(double * A, long N, long window_len)
{
	return array_windowed_aggregate(A, N, window_len, get_window_min_ctz_as_double, reduce_add) / N;
}

#undef  csr_value_features
#define csr_value_features  CSR_UTIL_GEN_EXPAND(csr_value_features)
void
csr_value_features(char * title_base, __attribute__((unused)) _TYPE_I * row_ptr, __attribute__((unused)) _TYPE_I * col_idx, _TYPE_V * values, __attribute__((unused)) long m, __attribute__((unused)) long n, long nnz, int do_plot)
{
	_TYPE_I * row_idx;
	double * vals;

	long num_pixels = 1024;
	long num_pixels_x = (n < 1024) ? n : num_pixels;
	long num_pixels_y = (n < 1024) ? m : num_pixels;
	long buf_n = strlen(title_base) + 1 + 1000;
	char buf[buf_n], buf_title[buf_n];
	__attribute__((unused)) double time;
	long num_bins;

	double vals_min, vals_max, vals_avg, vals_std, vals_ctz_avg, vals_ctz_std;
	// double vals_grouped_ctz_avg;
	double vals_abs_min, vals_abs_max, vals_abs_avg, vals_abs_std;
	double vals_exp_min, vals_exp_max, vals_exp_avg, vals_exp_std;
	long vals_unique_num, vals_sgnexp_unique_num;

	long window_size;
	long k;
	__attribute__((unused)) long i, i_s, i_e;

	// long ctz_group_size;

	csr_row_indexes(row_ptr, col_idx, m, n, nnz, &row_idx);

	num_bins = 1024;

	vals = (typeof(vals)) malloc(nnz * sizeof(*vals));
	#pragma omp parallel
	{
		long i;
		#pragma omp for
		for (i=0;i<nnz;i++)
			vals[i] = (double) values[i];
	}
	array_min_max(vals, nnz, &vals_min, NULL, &vals_max, NULL);
	array_mean(vals, nnz, &vals_avg);
	array_std(vals, nnz, &vals_std);
	array_mean(vals, nnz, &vals_ctz_avg, get_double_trailing_zeros);
	array_std(vals, nnz, &vals_ctz_std, get_double_trailing_zeros);
	array_min_max(vals, nnz, &vals_abs_min, NULL, &vals_abs_max, NULL, get_double_abs);
	array_mean(vals, nnz, &vals_abs_avg, get_double_abs);
	array_std(vals, nnz, &vals_abs_std, get_double_abs);
	array_min_max(vals, nnz, &vals_exp_min, NULL, &vals_exp_max, NULL, get_double_exponent);
	array_mean(vals, nnz, &vals_exp_avg, get_double_exponent);
	array_std(vals, nnz, &vals_exp_std, get_double_exponent);
	array_unique_num(vals, nnz, &vals_unique_num);
	array_unique_num(vals, nnz, &vals_sgnexp_unique_num, get_double_upper_12_bits);
	if (do_plot)
	{
		snprintf(buf, buf_n, "%s_values_heatmap.png", title_base);
		snprintf(buf_title, buf_n, "%s: values heatmap", title_base);
		figure_simple_plot(buf, num_pixels_x, num_pixels_y, (col_idx, row_ptr, vals, nnz, 0),
			figure_enable_legend(_fig);
			figure_set_title(_fig, buf_title);
			figure_axes_flip_y(_fig);
			figure_set_bounds_x(_fig, 0, n);
			figure_set_bounds_y(_fig, 0, m);
		);
		snprintf(buf, buf_n, "%s_values.png", title_base);
		snprintf(buf_title, buf_n, "%s: values (row-major order)", title_base);
		figure_simple_plot(buf, num_pixels_x, num_pixels_y, (NULL, vals, NULL, nnz, 0),
			figure_enable_legend(_fig);
			figure_set_title(_fig, buf_title);
		);
		snprintf(buf, buf_n, "%s_values_1D.png", title_base);
		snprintf(buf_title, buf_n, "%s: values 1D", title_base);
		figure_simple_plot(buf, num_pixels_x, num_pixels_y, (values, NULL, NULL, nnz, 0, , get_double_zero),
			figure_enable_legend(_fig);
			figure_set_title(_fig, buf_title);
			figure_set_bounds_y(_fig, -1, 1);
			figure_series_type_density_map(_s);
		);
		snprintf(buf, buf_n, "%s_values_distribution.png", title_base);
		snprintf(buf_title, buf_n, "%s: values distribution", title_base);
		figure_simple_plot(buf, num_pixels_x, num_pixels_y, (NULL, vals, NULL, nnz, 0),
			figure_enable_legend(_fig);
			figure_set_title(_fig, buf_title);
			figure_series_type_histogram(_s, num_bins, 1);
			figure_series_type_barplot(_s);
		);
		snprintf(buf, buf_n, "%s_values_exp_distribution.png", title_base);
		snprintf(buf_title, buf_n, "%s: values exponent distribution", title_base);
		figure_simple_plot(buf, num_pixels_x, num_pixels_y, (NULL, vals, NULL, nnz, 0, , get_double_exponent),
			figure_enable_legend(_fig);
			figure_set_title(_fig, buf_title);
			figure_series_type_histogram(_s, 0, 1); // Integer mode.
			figure_series_type_barplot(_s);
		);
		snprintf(buf, buf_n, "%s_values_frac_distribution.png", title_base);
		snprintf(buf_title, buf_n, "%s: values fraction distribution", title_base);
		figure_simple_plot(buf, num_pixels_x, num_pixels_y, (NULL, vals, NULL, nnz, 0, , get_double_fraction),
			figure_enable_legend(_fig);
			figure_set_title(_fig, buf_title);
			figure_series_type_histogram(_s, num_bins, 1);
			figure_series_type_barplot(_s);
		);
	}
	fprintf(stderr, "vals min = %g\n", vals_min);
	fprintf(stderr, "vals max = %g\n", vals_max);
	fprintf(stderr, "vals avg = %g\n", vals_avg);
	fprintf(stderr, "vals std = %g\n", vals_std);
	fprintf(stderr, "vals ctz avg = %.3lf\n", vals_ctz_avg);
	fprintf(stderr, "vals ctz std = %.3lf\n", vals_ctz_std);
	/* for (ctz_group_size=4;ctz_group_size<=16;ctz_group_size+=4)
	{
		array_mean_of_windowed_ctz_min(vals, nnz, &vals_grouped_ctz_avg, ctz_group_size);
		fprintf(stderr, "vals grouped (%ld) ctz avg of min = %.3lf\n", ctz_group_size, vals_grouped_ctz_avg);
	} */
	fprintf(stderr, "vals abs min = %g\n", vals_abs_min);
	fprintf(stderr, "vals abs max = %g\n", vals_abs_max);
	fprintf(stderr, "vals abs avg = %g\n", vals_abs_avg);
	fprintf(stderr, "vals abs std = %g\n", vals_abs_std);
	fprintf(stderr, "vals_exp min = %g\n", vals_exp_min);
	fprintf(stderr, "vals_exp max = %g\n", vals_exp_max);
	fprintf(stderr, "vals_exp avg = %g\n", vals_exp_avg);
	fprintf(stderr, "vals_exp std = %g\n", vals_exp_std);
	fprintf(stderr, "vals unique num = %ld\n", vals_unique_num);
	fprintf(stderr, "vals_sgnexp unique num = %ld\n", vals_sgnexp_unique_num);

	// Differences of neighbouring nnz
	double * vals_diff;
	double vals_diff_min, vals_diff_max, vals_diff_avg, vals_diff_std, vals_diff_ctz_avg, vals_diff_ctz_std;
	// double vals_diff_grouped_ctz_avg;
	double vals_diff_abs_min, vals_diff_abs_max, vals_diff_abs_avg, vals_diff_abs_std;
	double vals_diff_exp_min, vals_diff_exp_max, vals_diff_exp_avg, vals_diff_exp_std;
	vals_diff = (typeof(vals_diff)) malloc(nnz * sizeof(*vals_diff));
	vals_diff[0] = vals[0];
	#pragma omp parallel
	{
		long i;
		#pragma omp for
		for (i=1;i<nnz;i++)
			vals_diff[i] = vals[i] - vals[i-1];
	}
	array_min_max(vals_diff, nnz, &vals_diff_min, NULL, &vals_diff_max, NULL);
	array_mean(vals_diff, nnz, &vals_diff_avg);
	array_std(vals_diff, nnz, &vals_diff_std);
	array_mean(vals_diff, nnz, &vals_diff_ctz_avg, get_double_trailing_zeros);
	array_std(vals_diff, nnz, &vals_diff_ctz_std, get_double_trailing_zeros);
	array_min_max(vals_diff, nnz, &vals_diff_abs_min, NULL, &vals_diff_abs_max, NULL, get_double_abs);
	array_mean(vals_diff, nnz, &vals_diff_abs_avg, get_double_abs);
	array_std(vals_diff, nnz, &vals_diff_abs_std, get_double_abs);
	array_min_max(vals_diff, nnz, &vals_diff_exp_min, NULL, &vals_diff_exp_max, NULL, get_double_exponent);
	array_mean(vals_diff, nnz, &vals_diff_exp_avg, get_double_exponent);
	array_std(vals_diff, nnz, &vals_diff_exp_std, get_double_exponent);
	if (do_plot)
	{
		snprintf(buf, buf_n, "%s_values_diff.png", title_base);
		snprintf(buf_title, buf_n, "%s: values differences (row-major order)", title_base);
		figure_simple_plot(buf, num_pixels_x, num_pixels_y, (NULL, vals_diff, NULL, nnz, 0),
			figure_enable_legend(_fig);
			figure_set_title(_fig, buf_title);
		);
		snprintf(buf, buf_n, "%s_values_diff_distribution.png", title_base);
		snprintf(buf_title, buf_n, "%s: values differences distribution", title_base);
		figure_simple_plot(buf, num_pixels_x, num_pixels_y, (NULL, vals_diff, NULL, nnz, 0),
			figure_enable_legend(_fig);
			figure_set_title(_fig, buf_title);
			figure_series_type_histogram(_s, num_bins, 1);
			figure_series_type_barplot(_s);
		);
		snprintf(buf, buf_n, "%s_values_diff_exp_distribution.png", title_base);
		snprintf(buf_title, buf_n, "%s: values differences exponent distribution", title_base);
		figure_simple_plot(buf, num_pixels_x, num_pixels_y, (NULL, vals_diff, NULL, nnz, 0, , get_double_exponent),
			figure_enable_legend(_fig);
			figure_set_title(_fig, buf_title);
			figure_series_type_histogram(_s, 0, 1); // Integer mode.
			figure_series_type_barplot(_s);
		);
		snprintf(buf, buf_n, "%s_values_diff_frac_distribution.png", title_base);
		snprintf(buf_title, buf_n, "%s: values differences fraction distribution", title_base);
		figure_simple_plot(buf, num_pixels_x, num_pixels_y, (NULL, vals_diff, NULL, nnz, 0, , get_double_fraction),
			figure_enable_legend(_fig);
			figure_set_title(_fig, buf_title);
			figure_series_type_histogram(_s, num_bins, 1);
			figure_series_type_barplot(_s);
		);
	}
	fprintf(stderr, "vals_diff min = %g\n", vals_diff_min);
	fprintf(stderr, "vals_diff max = %g\n", vals_diff_max);
	fprintf(stderr, "vals_diff avg = %g\n", vals_diff_avg);
	fprintf(stderr, "vals_diff std = %g\n", vals_diff_std);
	fprintf(stderr, "vals_diff ctz avg = %.3lf\n", vals_diff_ctz_avg);
	fprintf(stderr, "vals_diff ctz std = %.3lf\n", vals_diff_ctz_std);
	/* for (ctz_group_size=4;ctz_group_size<=16;ctz_group_size+=4)
	{
		array_mean_of_windowed_ctz_min(vals_diff, nnz, ctz_group_size, &vals_diff_grouped_ctz_avg);
		fprintf(stderr, "vals_diff grouped (%ld) ctz avg of min = %.3lf\n", ctz_group_size, vals_diff_grouped_ctz_avg);
	} */
	fprintf(stderr, "vals_diff abs min = %g\n", vals_diff_abs_min);
	fprintf(stderr, "vals_diff abs max = %g\n", vals_diff_abs_max);
	fprintf(stderr, "vals_diff abs avg = %g\n", vals_diff_abs_avg);
	fprintf(stderr, "vals_diff abs std = %g\n", vals_diff_abs_std);
	fprintf(stderr, "vals_diff_exp min = %g\n", vals_diff_exp_min);
	fprintf(stderr, "vals_diff_exp max = %g\n", vals_diff_exp_max);
	fprintf(stderr, "vals_diff_exp avg = %g\n", vals_diff_exp_avg);
	fprintf(stderr, "vals_diff_exp std = %g\n", vals_diff_exp_std);
	free(vals_diff);

	#if 0
		double * abs_vals_diff;
		double abs_vals_diff_min, abs_vals_diff_max, abs_vals_diff_avg, abs_vals_diff_std, abs_vals_diff_ctz_avg, abs_vals_diff_ctz_std, abs_vals_diff_grouped_ctz_avg;
		double abs_vals_diff_abs_min, abs_vals_diff_abs_max, abs_vals_diff_abs_avg, abs_vals_diff_abs_std;
		abs_vals_diff = (typeof(abs_vals_diff)) malloc(nnz * sizeof(*abs_vals_diff));
		abs_vals_diff[0] = 0;
		#pragma omp parallel
		{
			long i;
			#pragma omp for
			for (i=1;i<nnz;i++)
				abs_vals_diff[i] = fabs(vals[i]) - fabs(vals[i-1]);
		}
		array_min_max(abs_vals_diff, nnz, &abs_vals_diff_min, NULL, &abs_vals_diff_max, NULL);
		array_mean(abs_vals_diff, nnz, &abs_vals_diff_avg);
		array_std(abs_vals_diff, nnz, &abs_vals_diff_std);
		array_mean(abs_vals_diff, nnz, &abs_vals_diff_ctz_avg, get_double_trailing_zeros);
		array_std(abs_vals_diff, nnz, &abs_vals_diff_ctz_std, get_double_trailing_zeros);
		array_min_max(abs_vals_diff, nnz, &abs_vals_diff_abs_min, NULL, &abs_vals_diff_abs_max, NULL, get_double_abs);
		array_mean(abs_vals_diff, nnz, &abs_vals_diff_abs_avg, get_double_abs);
		array_std(abs_vals_diff, nnz, &abs_vals_diff_abs_std, get_double_abs);
		if (do_plot)
		{
			double * running_avg;
			running_avg = (typeof(running_avg)) malloc(nnz * sizeof(*running_avg));
			scan_reduce(vals, running_avg, nnz, 0.0, 1, 0);
			#pragma omp parallel
			{
				long i;
				#pragma omp for
				for (i=0;i<nnz;i++)
					running_avg[i] /= (i + 1);
			}
			struct Figure * fig;
			struct Figure_Series * s;
			snprintf(buf, buf_n, "%s_values_running_avg.png", title_base);
			snprintf(buf_title, buf_n, "%s: values running average", title_base);
			fig = (typeof(fig)) malloc(sizeof(*fig));
			figure_init(fig, num_pixels_x, num_pixels_y);
			s = figure_add_series(fig, NULL, vals, NULL, nnz, 0);
			s = figure_add_series(fig, NULL, running_avg, NULL, nnz, 0);
			figure_series_set_color(s, 0xff, 0, 0);
			figure_enable_legend(fig);
			figure_set_title(fig, buf_title);
			figure_plot(fig, buf);
			figure_destroy(&fig);
			free(running_avg);
		}
		fprintf(stderr, "abs_vals_diff min = %g\n", abs_vals_diff_min);
		fprintf(stderr, "abs_vals_diff max = %g\n", abs_vals_diff_max);
		fprintf(stderr, "abs_vals_diff avg = %g\n", abs_vals_diff_avg);
		fprintf(stderr, "abs_vals_diff std = %g\n", abs_vals_diff_std);
		fprintf(stderr, "abs_vals_diff ctz avg = %.3lf\n", abs_vals_diff_ctz_avg);
		fprintf(stderr, "abs_vals_diff ctz std = %.3lf\n", abs_vals_diff_ctz_std);
		for (ctz_group_size=4;ctz_group_size<=16;ctz_group_size+=4)
		{
			array_mean_of_windowed_ctz_min(abs_vals_diff, nnz, ctz_group_size, &abs_vals_diff_grouped_ctz_avg);
			fprintf(stderr, "abs_vals_diff grouped (%ld) ctz avg of min = %.3lf\n", ctz_group_size, abs_vals_diff_grouped_ctz_avg);
		}
		fprintf(stderr, "abs_vals_diff abs min = %g\n", abs_vals_diff_abs_min);
		fprintf(stderr, "abs_vals_diff abs max = %g\n", abs_vals_diff_abs_max);
		fprintf(stderr, "abs_vals_diff abs avg = %g\n", abs_vals_diff_abs_avg);
		fprintf(stderr, "abs_vals_diff abs std = %g\n", abs_vals_diff_abs_std);
		free(abs_vals_diff);

		double * windowed_avg;
		double * dist_from_windowed_avg;
		double dist_from_windowed_avg_abs_min, dist_from_windowed_avg_abs_max, dist_from_windowed_avg_abs_avg, dist_from_windowed_avg_abs_std, dist_from_windowed_avg_ctz_avg, dist_from_windowed_avg_ctz_std, dist_from_windowed_avg_grouped_ctz_avg;
		window_size = 1 << 2;
		windowed_avg = (typeof(windowed_avg)) malloc(nnz * sizeof(*windowed_avg));
		dist_from_windowed_avg = (typeof(dist_from_windowed_avg)) malloc(nnz * sizeof(*dist_from_windowed_avg));
		#pragma omp parallel
		{
			double sum = 0;
			long i, j, j_e;
			#pragma omp for
			for (i=0;i<nnz;i+=window_size)
			{
				j_e = i + window_size;
				if (j_e > nnz)
					j_e = nnz;
				sum = 0;
				for (j=i;j<j_e;j++)
					sum += vals[j];
				sum /= j_e - i;
				for (j=i;j<j_e;j++)
				{
					windowed_avg[j] = sum;
					dist_from_windowed_avg[j] = sum - vals[j];
				}
			}
		}
		array_min_max(dist_from_windowed_avg, nnz, &dist_from_windowed_avg_abs_min, NULL, &dist_from_windowed_avg_abs_max, NULL, get_double_abs);
		array_mean(dist_from_windowed_avg, nnz, &dist_from_windowed_avg_abs_avg, get_double_abs);
		array_std(dist_from_windowed_avg, nnz, &dist_from_windowed_avg_abs_std, get_double_abs);
		array_mean(dist_from_windowed_avg, nnz, &dist_from_windowed_avg_ctz_avg, get_double_trailing_zeros);
		array_std(dist_from_windowed_avg, nnz, &dist_from_windowed_avg_ctz_std, get_double_trailing_zeros);
		if (do_plot)
		{
			struct Figure * fig;
			struct Figure_Series * s;
			snprintf(buf, buf_n, "%s_values_windowed_avg.png", title_base);
			snprintf(buf_title, buf_n, "%s: values windowed average", title_base);
			fig = (typeof(fig)) malloc(sizeof(*fig));
			figure_init(fig, num_pixels_x, num_pixels_y);
			s = figure_add_series(fig, NULL, vals, NULL, nnz, 0);
			s = figure_add_series(fig, NULL, windowed_avg, NULL, nnz, 0);
			figure_series_set_color(s, 0xff, 0, 0);
			figure_enable_legend(fig);
			figure_set_title(fig, buf_title);
			figure_plot(fig, buf);
			figure_destroy(&fig);
		}
		fprintf(stderr, "dist_from_windowed_avg abs min = %g\n", dist_from_windowed_avg_abs_min);
		fprintf(stderr, "dist_from_windowed_avg abs max = %g\n", dist_from_windowed_avg_abs_max);
		fprintf(stderr, "dist_from_windowed_avg abs avg = %g\n", dist_from_windowed_avg_abs_avg);
		fprintf(stderr, "dist_from_windowed_avg abs std = %g\n", dist_from_windowed_avg_abs_std);
		fprintf(stderr, "dist_from_windowed_avg ctz avg = %.3lf\n", dist_from_windowed_avg_ctz_avg);
		fprintf(stderr, "dist_from_windowed_avg ctz std = %.3lf\n", dist_from_windowed_avg_ctz_std);
		for (ctz_group_size=4;ctz_group_size<=16;ctz_group_size+=4)
		{
			array_mean_of_windowed_ctz_min(dist_from_windowed_avg, nnz, ctz_group_size, &dist_from_windowed_avg_grouped_ctz_avg);
			fprintf(stderr, "dist_from_windowed_avg grouped (%ld) ctz avg of min = %.3lf\n", ctz_group_size, dist_from_windowed_avg_grouped_ctz_avg);
		}
		free(windowed_avg);
		free(dist_from_windowed_avg);
	#endif

	// Sorting.
	double * vals_diff_sorted_window;
	double vals_diff_sorted_window_abs_min, vals_diff_sorted_window_abs_max, vals_diff_sorted_window_abs_avg, vals_diff_sorted_window_abs_std, vals_diff_sorted_window_ctz_avg, vals_diff_sorted_window_ctz_std;
	// double vals_diff_sorted_window_grouped_ctz_avg;
	long vals_diff_sorted_window_unique_num, vals_diff_sorted_window_exp_unique_num;
	double ctz_corrected, ctz_corrected_best;
	long best_k, best_ctz_group_size;
	vals_diff_sorted_window = (typeof(vals_diff_sorted_window)) malloc(nnz * sizeof(*vals_diff_sorted_window));
	ctz_corrected_best = 0;
	best_k = 0;
	best_ctz_group_size = 1;
	// k = 21;
	for (k=4;k<=20;k++)
	{
		long num_windows;
		double * windows_exp_unique_num;
		double windows_exp_unique_num_min, windows_exp_unique_num_max, windows_exp_unique_num_avg;
		window_size = 1 << k;
		num_windows = (nnz + window_size - 1) / window_size;
		windows_exp_unique_num = (typeof(windows_exp_unique_num)) malloc(num_windows * sizeof(*windows_exp_unique_num));
		#pragma omp parallel
		{
			double * window = (typeof(window)) malloc(window_size * sizeof(*window));
			int * qsort_partitions = (typeof(qsort_partitions)) malloc(window_size * sizeof(*qsort_partitions));
			long i, j, j_e, k;
			long exp_table_n = 1<<12;
			long * exp_table;
			exp_table = (typeof(exp_table)) malloc(exp_table_n * sizeof(*exp_table));
			#pragma omp for
			for (i=0;i<nnz;i+=window_size)
			{
				j_e = i + window_size;
				if (j_e > nnz)
					j_e = nnz;
				for (j=i;j<j_e;j++)
					window[j-i] = vals[j];
				quicksort_no_malloc(window, j_e-i, NULL, qsort_partitions);
				vals_diff_sorted_window[i] = window[0];
				for (j=i+1;j<j_e;j++)
					vals_diff_sorted_window[j] = window[j-i] - window[j-1-i];
				for (j=0;j<exp_table_n;j++)
					exp_table[j] = 0;
				for (j=i;j<j_e;j++)
					exp_table[(int) get_double_exponent_bits(vals_diff_sorted_window, j)] = 1;
				k = 0;
				for (j=0;j<exp_table_n;j++)
				{
					if (exp_table[j])
						k++;
				}
				windows_exp_unique_num[i / window_size] = k;
			}
			free(window);
			free(qsort_partitions);
			free(exp_table);
		}
		array_min_max(vals_diff_sorted_window, nnz, &vals_diff_sorted_window_abs_min, NULL, &vals_diff_sorted_window_abs_max, NULL, get_double_abs);
		array_mean(vals_diff_sorted_window, nnz, &vals_diff_sorted_window_abs_avg, get_double_abs);
		array_std(vals_diff_sorted_window, nnz, &vals_diff_sorted_window_abs_std, get_double_abs);
		array_mean(vals_diff_sorted_window, nnz, &vals_diff_sorted_window_ctz_avg, get_double_trailing_zeros);
		ctz_corrected = vals_diff_sorted_window_ctz_avg - k - 6;
		if (ctz_corrected > ctz_corrected_best)
		{
			ctz_corrected_best = ctz_corrected;
			best_k = k;
			best_ctz_group_size = 1;
		}
		array_std(vals_diff_sorted_window, nnz, &vals_diff_sorted_window_ctz_std, get_double_trailing_zeros);
		array_unique_num(vals_diff_sorted_window, nnz, &vals_diff_sorted_window_unique_num);
		array_unique_num(vals_diff_sorted_window, nnz, &vals_diff_sorted_window_exp_unique_num, get_double_exponent);
		array_min_max(windows_exp_unique_num, num_windows, &windows_exp_unique_num_min, NULL, &windows_exp_unique_num_max, NULL);
		array_mean(windows_exp_unique_num, num_windows, &windows_exp_unique_num_avg);
		fprintf(stderr, "vals_diff_sorted_window (2^%ld) abs min = %g\n", k, vals_diff_sorted_window_abs_min);
		fprintf(stderr, "vals_diff_sorted_window (2^%ld) abs max = %g\n", k, vals_diff_sorted_window_abs_max);
		fprintf(stderr, "vals_diff_sorted_window (2^%ld) abs avg = %g\n", k, vals_diff_sorted_window_abs_avg);
		fprintf(stderr, "vals_diff_sorted_window (2^%ld) abs std = %g\n", k, vals_diff_sorted_window_abs_std);
		fprintf(stderr, "vals_diff_sorted_window (2^%ld) ctz avg = %.3lf\n", k, vals_diff_sorted_window_ctz_avg);
		fprintf(stderr, "vals_diff_sorted_window (2^%ld) ctz avg corrected (- %ld - 6) = %.3lf\n", k, k, ctz_corrected);
		fprintf(stderr, "vals_diff_sorted_window (2^%ld) ctz std = %.3lf\n", k, vals_diff_sorted_window_ctz_std);
		/* for (ctz_group_size=4;ctz_group_size<=16;ctz_group_size+=4)
		{
			array_mean_of_windowed_ctz_min(vals_diff_sorted_window, nnz, ctz_group_size, &vals_diff_sorted_window_grouped_ctz_avg);
			ctz_corrected = vals_diff_sorted_window_grouped_ctz_avg - k - (6.0 / ctz_group_size);
			if (ctz_corrected > ctz_corrected_best)
			{
				ctz_corrected_best = ctz_corrected;
				best_k = k;
				best_ctz_group_size = ctz_group_size;
			}
			fprintf(stderr, "vals_diff_sorted_window (2^%ld) grouped (%ld) ctz avg of min - %ld - %.3lf = %.3lf\n", k, ctz_group_size, k, 6.0 / ctz_group_size, ctz_corrected);
		} */
		fprintf(stderr, "vals_diff_sorted_window (2^%ld) unique num = %ld\n", k, vals_diff_sorted_window_unique_num);
		fprintf(stderr, "vals_diff_sorted_window_exp (2^%ld) unique num = %ld\n", k, vals_diff_sorted_window_exp_unique_num);
		fprintf(stderr, "windows_exp (2^%ld) unique num min = %.3lf\n", k, windows_exp_unique_num_min);
		fprintf(stderr, "windows_exp (2^%ld) unique num max = %.3lf\n", k, windows_exp_unique_num_max);
		fprintf(stderr, "windows_exp (2^%ld) unique num avg = %.3lf\n", k, windows_exp_unique_num_avg);
		free(windows_exp_unique_num);
	}
	fprintf(stderr, "vals_diff_sorted_window best ctz avg corrected (- %ld - 6) = %.3lf\n", best_k, ctz_corrected_best);
	fprintf(stderr, "vals_diff_sorted_window best windows size = %ld\n", best_k);
	fprintf(stderr, "vals_diff_sorted_window best ctz group size = %ld\n", best_ctz_group_size);
	free(vals_diff_sorted_window);

	// Clustering - Distance from cluster centers.
	#if 0
	double * vals_sorted;
	double * vals_sorted_shifted_by_min;
	double * vals_cluster_centers_diffs;
	double vals_cluster_centers_diffs_min, vals_cluster_centers_diffs_max, vals_cluster_centers_diffs_avg, vals_cluster_centers_diffs_std, vals_cluster_centers_diffs_ctz_avg, vals_cluster_centers_diffs_ctz_std, vals_cluster_centers_diffs_grouped_ctz_avg;
	vals_sorted = (typeof(vals_sorted)) malloc(nnz * sizeof(*vals_sorted));
	vals_cluster_centers_diffs = (typeof(vals_cluster_centers_diffs)) malloc(nnz * sizeof(*vals_cluster_centers_diffs));
	vals_sorted_shifted_by_min = (typeof(vals_sorted_shifted_by_min)) malloc(nnz * sizeof(*vals_sorted_shifted_by_min));
	#pragma omp parallel
	{
		long i;
		#pragma omp for
		for (i=0;i<nnz;i++)
			vals_sorted[i] = vals[i];
	}
	time = time_it(1,
		samplesort(vals_sorted, nnz, NULL);
	);
	printf("time samplesort = %lf\n", time);
	if (do_plot)
	{
		snprintf(buf, buf_n, "%s_values_sorted.png", title_base);
		snprintf(buf_title, buf_n, "%s: values sorted", title_base);
		figure_simple_plot(buf, num_pixels_x, num_pixels_y, (NULL, vals_sorted, NULL, nnz, 0),
			figure_enable_legend(_fig);
			figure_set_title(_fig, buf_title);
		);
	}
	// We have to shift values by min because 'loop_partitioner_balance_prefix_sums' can only work with positive values.
	#pragma omp parallel
	{
		long i;
		#pragma omp for
		for (i=0;i<nnz;i++)
			vals_sorted_shifted_by_min[i] = vals_sorted[i] - vals_min;
	}
	ctz_corrected_best = 0;
	best_k = 0;
	best_ctz_group_size = 1;
	for (k=4;k<=14;k++)
	{
		long num_clusters = 1 << k;
		double cluster_centers[num_clusters];
		// printf("centers: ");
		for (i=0;i<num_clusters;i++)
		{
			loop_partitioner_balance_prefix_sums(2*num_clusters, 2*i, vals_sorted_shifted_by_min, nnz, vals_sorted_shifted_by_min[nnz-1], &i_s, &i_e);
			cluster_centers[i] = vals_sorted[i_e];
			// printf("%g, ", cluster_centers[i]);
		}
		// printf("\n");
		#pragma omp parallel
		{
			long i;
			#pragma omp for
			for (i=0;i<nnz;i++)
			{
				long center_pos = binary_search(cluster_centers, 0, num_clusters - 1, vals[i], NULL, NULL);
				double center = cluster_centers[center_pos];
				vals_cluster_centers_diffs[i] = vals[i] - center;
			}
		}
		array_min_max(vals_cluster_centers_diffs, nnz, &vals_cluster_centers_diffs_min, NULL, &vals_cluster_centers_diffs_max, NULL);
		array_mean(vals_cluster_centers_diffs, nnz, &vals_cluster_centers_diffs_avg);
		array_std(vals_cluster_centers_diffs, nnz, &vals_cluster_centers_diffs_std);
		array_mean(vals_cluster_centers_diffs, nnz, &vals_cluster_centers_diffs_ctz_avg, get_double_trailing_zeros);
		ctz_corrected = vals_cluster_centers_diffs_ctz_avg - k - 6;
		if (ctz_corrected > ctz_corrected_best)
		{
			ctz_corrected_best = ctz_corrected;
			best_k = k;
			best_ctz_group_size = 1;
		}
		best_ctz_group_size = 1;
		array_std(vals_cluster_centers_diffs, nnz, &vals_cluster_centers_diffs_ctz_std, get_double_trailing_zeros);
		fprintf(stderr, "vals_cluster_centers_diffs min = %g\n", vals_cluster_centers_diffs_min);
		fprintf(stderr, "vals_cluster_centers_diffs max = %g\n", vals_cluster_centers_diffs_max);
		fprintf(stderr, "vals_cluster_centers_diffs avg = %g\n", vals_cluster_centers_diffs_avg);
		fprintf(stderr, "vals_cluster_centers_diffs std = %g\n", vals_cluster_centers_diffs_std);
		fprintf(stderr, "vals_cluster_centers_diffs (2^%ld) ctz avg - %ld - 6 = %.3lf\n", k, k, ctz_corrected);
		fprintf(stderr, "vals_cluster_centers_diffs ctz std = %.3lf\n", vals_cluster_centers_diffs_ctz_std);
		for (ctz_group_size=4;ctz_group_size<=16;ctz_group_size+=4)
		{
			array_mean_of_windowed_ctz_min(vals_cluster_centers_diffs, nnz, ctz_group_size, &vals_cluster_centers_diffs_grouped_ctz_avg);
			ctz_corrected = vals_cluster_centers_diffs_grouped_ctz_avg - k - (6.0 / ctz_group_size);
			if (ctz_corrected > ctz_corrected_best)
			{
				ctz_corrected_best = ctz_corrected;
				best_k = k;
				best_ctz_group_size = 1;
			}
			fprintf(stderr, "vals_cluster_centers_diffs (2^%ld) grouped (%ld) ctz avg of min - %ld - %.3lf = %.3lf\n", k, ctz_group_size, k, 6.0 / ctz_group_size, ctz_corrected);
		}
		if (do_plot)
		{
			struct Figure * fig;
			struct Figure_Series * s;
			snprintf(buf, buf_n, "%s_values_1D_cluster_centers_2^%ld.png", title_base, k);
			snprintf(buf_title, buf_n, "%s: values 1D with cluster centers (2^%ld)", title_base, k);
			fig = (typeof(fig)) malloc(sizeof(*fig));
			figure_init(fig, num_pixels_x, num_pixels_y);
			s = figure_add_series(fig, values, NULL, NULL, nnz, 0, , get_double_zero);
			s = figure_add_series(fig, cluster_centers, NULL, NULL, num_clusters, 0, , get_double_zero);
			figure_series_set_dot_size_pixels(s, 3);
			figure_series_set_color(s, 0xff, 0, 0);
			figure_enable_legend(fig);
			figure_set_title(fig, buf_title);
			figure_set_bounds_y(fig, -1, 1);
			figure_plot(fig, buf);
			figure_destroy(&fig);
		}
	}
	fprintf(stderr, "vals_cluster_centers_diffs best ctz corrected avg = %.3lf\n", ctz_corrected_best);
	fprintf(stderr, "vals_cluster_centers_diffs best windows size = %ld\n", best_k);
	fprintf(stderr, "vals_cluster_centers_diffs best ctz group size = %ld\n", best_ctz_group_size);
	free(vals_sorted_shifted_by_min);
	free(vals_sorted);
	free(vals_cluster_centers_diffs);
	#endif

	free(vals);
	free(row_idx);
}


//==========================================================================================================================================
//= Print Features
//==========================================================================================================================================


#undef  csr_matrix_features
#define csr_matrix_features  CSR_UTIL_GEN_EXPAND(csr_matrix_features)
void
csr_matrix_features(char * title_base, _TYPE_I * row_ptr, _TYPE_I * col_idx, long m, long n, long nnz, int do_plot)
{
	_TYPE_I * row_idx;
	_TYPE_I * degrees_rows;
	_TYPE_I * degrees_cols;
	double * bandwidths;
	double * scatters;
	double mem_footprint = (nnz * (sizeof(_TYPE_I) + sizeof(_TYPE_V)) + (m + 1) * sizeof(_TYPE_I)) / ((double) 1024 * 1024);
	double nnz_per_row_min, nnz_per_row_max, nnz_per_row_avg, nnz_per_row_std;
	double nnz_per_col_min, nnz_per_col_max, nnz_per_col_avg, nnz_per_col_std;
	double bw_min, bw_max, bw_avg, bw_std;
	double sc_min, sc_max, sc_avg, sc_std;

	long window_size;
	_TYPE_I * num_neigh;
	double num_neigh_min, num_neigh_max, num_neigh_avg, num_neigh_std;
	double cross_row_similarity_avg;

	long max_gap_size;
	_TYPE_I * nnz_col_dist, * group_col_dist, * group_sizes, * groups_per_row;
	double nnz_col_dist_min, nnz_col_dist_max, nnz_col_dist_avg, nnz_col_dist_std;
	double group_col_dist_min, group_col_dist_max, group_col_dist_avg, group_col_dist_std;
	double group_sizes_min, group_sizes_max, group_sizes_avg, group_sizes_std;
	double groups_per_row_min, groups_per_row_max, groups_per_row_avg, groups_per_row_std;
	long num_groups;

	long num_pixels = 1024;
	long num_pixels_x = (n < 1024) ? n : num_pixels;
	long num_pixels_y = (n < 1024) ? m : num_pixels;
	long buf_n = strlen(title_base) + 1 + 1000;
	char buf[buf_n], buf_title[buf_n];
	double time;

	time = time_it(1,
		csr_row_indexes(row_ptr, col_idx, m, n, nnz, &row_idx);
	);
	printf("time row indexes = %lf\n", time);
	// Matrix.
	if (do_plot)
	{
		snprintf(buf, buf_n, "%s.png", title_base);
		snprintf(buf_title, buf_n, "%s", title_base);
		figure_simple_plot(buf, num_pixels_x, num_pixels_y, (col_idx, row_idx, NULL, nnz, 0),
			figure_enable_legend(_fig);
			figure_set_title(_fig, buf_title);
			figure_axes_flip_y(_fig);
			figure_set_bounds_x(_fig, 0, n);
			figure_set_bounds_y(_fig, 0, m);
			figure_series_type_density_map(_s);
		);
	}

	time = time_it(1,
		csr_degrees_bandwidths_scatters(row_ptr, col_idx, m, n, nnz, &degrees_rows, &degrees_cols, &bandwidths, &scatters);
	);
	printf("time csr_degrees_bandwidths_scatters = %lf\n", time);

	/* It's not worth it combining the array metrics, because min-max degrade the total performance of the pipeline,
	 * and std depends on mean and needs a separete loop either way. */

	array_min_max(degrees_rows, m, &nnz_per_row_min, NULL, &nnz_per_row_max, NULL);
	nnz_per_row_avg = ((double) nnz) / m;
	array_std(degrees_rows, m, &nnz_per_row_std);

	array_min_max(degrees_cols, n, &nnz_per_col_min, NULL, &nnz_per_col_max, NULL);
	nnz_per_col_avg = ((double) nnz) / n;
	array_std(degrees_cols, n, &nnz_per_col_std);

	array_min_max(bandwidths, m, &bw_min, NULL, &bw_max, NULL);
	array_mean(bandwidths, m, &bw_avg);
	array_std(bandwidths, m, &bw_std);

	array_min_max(scatters, m, &sc_min, NULL, &sc_max, NULL);
	array_mean(scatters, m, &sc_avg);
	array_std(scatters, m, &sc_std);

	// Degree histogram.
	if (do_plot)
	{
		snprintf(buf, buf_n, "%s_degree_distribution.png", title_base);
		snprintf(buf_title, buf_n, "%s: degree distribution", title_base);
		figure_simple_plot(buf, num_pixels_x, num_pixels_y, (NULL, degrees_rows, NULL, m, 0),
			figure_enable_legend(_fig);
			figure_set_title(_fig, buf_title);
			figure_series_type_histogram(_s, 0, 1);
			figure_series_type_barplot(_s);
		);
	}

	free(degrees_rows);
	free(bandwidths);
	free(scatters);

	// window_size = 64 / sizeof(_TYPE_V) / 2;
	// window_size = 8;
	// window_size = 4;
	window_size = 1;
	// printf("window size = %ld\n", window_size);

	time = time_it(1,
		csr_row_neighbours(row_ptr, col_idx, m, n, nnz, window_size, &num_neigh);
	);
	printf("time csr_row_neighbours = %lf\n", time);

	array_min_max(num_neigh, nnz, &num_neigh_min, NULL, &num_neigh_max, NULL);
	array_mean(num_neigh, nnz, &num_neigh_avg);
	array_std(num_neigh, nnz, &num_neigh_std);

	// Neighbours histogram.
	if (do_plot)
	{
		snprintf(buf, buf_n, "%s_num_neigh.png", title_base);
		snprintf(buf_title, buf_n, "%s: number of neighbours distribution", title_base);
		figure_simple_plot(buf, num_pixels_x, num_pixels_y, (NULL, num_neigh, NULL, nnz, 0),
			figure_enable_legend(_fig);
			figure_set_title(_fig, buf_title);
			figure_series_type_histogram(_s, 0, 1);
			figure_series_type_barplot(_s);
		);
	}

	free(num_neigh);

	time = time_it(1,
		cross_row_similarity_avg = csr_cross_row_similarity(row_ptr, col_idx, m, n, nnz, window_size);
	);
	printf("time csr_cross_row_similarity = %lf\n", time);

	max_gap_size = 0;
	time = time_it(1,
		num_groups = csr_column_distances_and_groupping(row_ptr, col_idx, m, n, nnz, max_gap_size, &nnz_col_dist, &group_col_dist, &group_sizes, &groups_per_row);
	);
	printf("time csr_column_distances_and_groupping = %lf\n", time);

	array_min_max(nnz_col_dist, nnz, &nnz_col_dist_min, NULL, &nnz_col_dist_max, NULL);
	array_mean(nnz_col_dist, nnz, &nnz_col_dist_avg);
	array_std(nnz_col_dist, nnz, &nnz_col_dist_std);

	array_min_max(group_col_dist, num_groups, &group_col_dist_min, NULL, &group_col_dist_max, NULL);
	array_mean(group_col_dist, num_groups, &group_col_dist_avg);
	array_std(group_col_dist, num_groups, &group_col_dist_std);

	array_min_max(group_sizes, num_groups, &group_sizes_min, NULL, &group_sizes_max, NULL);
	array_mean(group_sizes, num_groups, &group_sizes_avg);
	array_std(group_sizes, num_groups, &group_sizes_std);

	array_min_max(groups_per_row, m, &groups_per_row_min, NULL, &groups_per_row_max, NULL);
	array_mean(groups_per_row, m, &groups_per_row_avg);
	array_std(groups_per_row, m, &groups_per_row_std);

	// Groups per row histogram.
	if (do_plot)
	{
		snprintf(buf, buf_n, "%s_groups_per_row.png", title_base);
		snprintf(buf_title, buf_n, "%s: groups per row distribution", title_base);
		figure_simple_plot(buf, num_pixels_x, num_pixels_y, (NULL, groups_per_row, NULL, m, 0),
			figure_enable_legend(_fig);
			figure_set_title(_fig, buf_title);
			figure_series_type_histogram(_s, 0, 1);
			figure_series_type_barplot(_s);
		);
	}

	free(nnz_col_dist);
	free(group_col_dist);
	free(group_sizes);
	free(groups_per_row);

	fprintf(stderr, "matrix = %s\n", title_base);
	fprintf(stderr, "m = %ld\n", m);
	fprintf(stderr, "n = %ld\n", n);
	fprintf(stderr, "nnz = %ld\n", nnz);
	fprintf(stderr, "density = %g\n", nnz / ((double) m*n));
	fprintf(stderr, "mem_footprint (MB in CSR) = %lf\n", mem_footprint);
	fprintf(stderr, "nnz_per_row min = %lf\n", nnz_per_row_min);
	fprintf(stderr, "nnz_per_row max = %lf\n", nnz_per_row_max);
	fprintf(stderr, "nnz_per_row avg = %lf\n", nnz_per_row_avg);
	fprintf(stderr, "nnz_per_row std = %lf\n", nnz_per_row_std);
	fprintf(stderr, "nnz_per_col min = %lf\n", nnz_per_col_min);
	fprintf(stderr, "nnz_per_col max = %lf\n", nnz_per_col_max);
	fprintf(stderr, "nnz_per_col avg = %lf\n", nnz_per_col_avg);
	fprintf(stderr, "nnz_per_col std = %lf\n", nnz_per_col_std);
	fprintf(stderr, "skew = %lf\n", (nnz_per_row_max - nnz_per_row_avg) / nnz_per_row_avg);
	fprintf(stderr, "bw_scaled min = %lf\n", bw_min / n);
	fprintf(stderr, "bw_scaled max = %lf\n", bw_max / n);
	fprintf(stderr, "bw_scaled avg = %lf\n", bw_avg / n);
	fprintf(stderr, "bw_scaled std = %lf\n", bw_std / n);
	fprintf(stderr, "sc min = %lf\n", sc_min);
	fprintf(stderr, "sc max = %lf\n", sc_max);
	fprintf(stderr, "sc avg = %lf\n", sc_avg);
	fprintf(stderr, "sc std = %lf\n", sc_std);
	fprintf(stderr, "num_neigh min = %lf\n", num_neigh_min);
	fprintf(stderr, "num_neigh max = %lf\n", num_neigh_max);
	fprintf(stderr, "num_neigh avg = %lf\n", num_neigh_avg);
	fprintf(stderr, "num_neigh std = %lf\n", num_neigh_std);
	fprintf(stderr, "cross_row_similarity avg = %lf\n", cross_row_similarity_avg);
	fprintf(stderr, "nnz_col_dist min = %lf\n", nnz_col_dist_min);
	fprintf(stderr, "nnz_col_dist max = %lf\n", nnz_col_dist_max);
	fprintf(stderr, "nnz_col_dist avg = %lf\n", nnz_col_dist_avg);
	fprintf(stderr, "nnz_col_dist std = %lf\n", nnz_col_dist_std);
	fprintf(stderr, "group_col_dist min = %lf\n", group_col_dist_min);
	fprintf(stderr, "group_col_dist max = %lf\n", group_col_dist_max);
	fprintf(stderr, "group_col_dist avg = %lf\n", group_col_dist_avg);
	fprintf(stderr, "group_col_dist std = %lf\n", group_col_dist_std);
	fprintf(stderr, "group_sizes min = %lf\n", group_sizes_min);
	fprintf(stderr, "group_sizes max = %lf\n", group_sizes_max);
	fprintf(stderr, "group_sizes avg = %lf\n", group_sizes_avg);
	fprintf(stderr, "group_sizes std = %lf\n", group_sizes_std);
	fprintf(stderr, "groups_per_row min = %lf\n", groups_per_row_min);
	fprintf(stderr, "groups_per_row max = %lf\n", groups_per_row_max);
	fprintf(stderr, "groups_per_row avg = %lf\n", groups_per_row_avg);
	fprintf(stderr, "groups_per_row std = %lf\n", groups_per_row_std);
	fprintf(stderr, "num_groups = %ld\n", num_groups);
	fprintf(stderr, "nnz_per_cluster avg = %lf\n", nnz_per_row_avg / groups_per_row_avg);

	/* Matrix features for artificial twins.
	 * Also print the csr mem footprint for easier sorting.
	 */
	#if 0
		double csr_mem_footprint = nnz * (sizeof(double) + sizeof(int)) + (m+1) * sizeof(int);
		fprintf(stderr, "%-15.5lf ", csr_mem_footprint / (1024*1024));
		fprintf(stderr, "%s", title_base);
		fprintf(stderr, "\n");
	#endif
	#if 0
		double csr_mem_footprint = nnz * (sizeof(double) + sizeof(int)) + (m+1) * sizeof(int);
		fprintf(stderr, "%-15.5lf ", csr_mem_footprint / (1024*1024));
		fprintf(stderr, "['%s']='", title_base);
		fprintf(stderr, "%ld ", m);
		fprintf(stderr, "%ld ", n);
		fprintf(stderr, "%.10lf ", nnz_per_row_avg);
		fprintf(stderr, "%.10lf ", nnz_per_row_std);
		fprintf(stderr, "normal ");
		fprintf(stderr, "random ");
		fprintf(stderr, "%.10lf ", bw_avg / n);
		fprintf(stderr, "%.10lf ", (nnz_per_row_max - nnz_per_row_avg) / nnz_per_row_avg);
		fprintf(stderr, "%.10lf ", num_neigh_avg);
		fprintf(stderr, "%.10lf ", cross_row_similarity_avg);
		fprintf(stderr, "14 ");
		fprintf(stderr, "%s", title_base);
		fprintf(stderr, "'\n");
	#endif
	#if 0
		fprintf(stderr, "%s\t", title_base);
		fprintf(stderr, "%ld\t", m);
		fprintf(stderr, "%ld\t", n);
		fprintf(stderr, "%.10lf\t", nnz_per_row_avg);
		fprintf(stderr, "%.10lf\t", nnz_per_row_std);
		fprintf(stderr, "normal\t");
		fprintf(stderr, "random\t");
		fprintf(stderr, "%.10lf\t", bw_avg / n);
		fprintf(stderr, "%.10lf\t", (nnz_per_row_max - nnz_per_row_avg) / nnz_per_row_avg);
		fprintf(stderr, "%.10lf\t", num_neigh_avg);
		fprintf(stderr, "%.10lf\t", cross_row_similarity_avg);
		fprintf(stderr, "\n");
	#endif

	free(row_idx);
}


#undef  csr_matrix_features_validation
#define csr_matrix_features_validation  CSR_UTIL_GEN_EXPAND(csr_matrix_features_validation)
void
csr_matrix_features_validation(char * title_base, _TYPE_I * row_ptr, _TYPE_I * col_idx, long m, long n, long nnz)
{
	_TYPE_I * row_idx;
	_TYPE_I * degrees_rows;
	_TYPE_I * degrees_cols;
	double * bandwidths;
	double * scatters;
	double mem_footprint = (nnz * (sizeof(_TYPE_I) + sizeof(_TYPE_V)) + (m + 1) * sizeof(_TYPE_I)) / ((double) 1024 * 1024);
	double nnz_per_row_min, nnz_per_row_max, nnz_per_row_avg, nnz_per_row_std;
	double bw_avg;

	long window_size;
	_TYPE_I * num_neigh;
	double num_neigh_avg;
	double cross_row_similarity_avg;

	double time;

	time = time_it(1,
		csr_row_indexes(row_ptr, col_idx, m, n, nnz, &row_idx);
	);
	printf("time row indexes = %lf\n", time);

	time = time_it(1,
		csr_degrees_bandwidths_scatters(row_ptr, col_idx, m, n, nnz, &degrees_rows, &degrees_cols, &bandwidths, &scatters);
	);
	printf("time csr_degrees_bandwidths_scatters = %lf\n", time);

	/* It's not worth it combining the array metrics, because min-max degrade the total performance of the pipeline,
	 * and std depends on mean and needs a separete loop either way. */

	array_min_max(degrees_rows, m, &nnz_per_row_min, NULL, &nnz_per_row_max, NULL);
	nnz_per_row_avg = ((double) nnz) / m;
	array_std(degrees_rows, m, &nnz_per_row_std);

	array_mean(bandwidths, m, &bw_avg);

	free(degrees_rows);
	free(bandwidths);
	free(scatters);

	// window_size = 64 / sizeof(_TYPE_V) / 2;
	// window_size = 8;
	// window_size = 4;
	window_size = 1;
	// printf("window size = %ld\n", window_size);

	time = time_it(1,
		csr_row_neighbours(row_ptr, col_idx, m, n, nnz, window_size, &num_neigh);
	);
	printf("time csr_row_neighbours = %lf\n", time);

	array_mean(num_neigh, nnz, &num_neigh_avg);

	free(num_neigh);

	time = time_it(1,
		cross_row_similarity_avg = csr_cross_row_similarity(row_ptr, col_idx, m, n, nnz, window_size);
	);
	printf("time csr_cross_row_similarity = %lf\n", time);

	/* Matrix features for artificial twins.
	 * Also print the csr mem footprint for easier sorting.
	 */
	#if 1
		fprintf(stderr, "%-15.5lf ", mem_footprint);
		fprintf(stderr, "['%s']='", title_base);
		fprintf(stderr, "%ld ", m);
		fprintf(stderr, "%ld ", n);
		fprintf(stderr, "%.10lf ", nnz_per_row_avg);
		fprintf(stderr, "%.10lf ", nnz_per_row_std);
		fprintf(stderr, "normal ");
		fprintf(stderr, "random ");
		fprintf(stderr, "%.10lf ", bw_avg / n);
		fprintf(stderr, "%.10lf ", (nnz_per_row_max - nnz_per_row_avg) / nnz_per_row_avg);
		fprintf(stderr, "%.10lf ", num_neigh_avg);
		fprintf(stderr, "%.10lf ", cross_row_similarity_avg);
		fprintf(stderr, "14 ");
		fprintf(stderr, "%s", title_base);
		fprintf(stderr, "'\n");
	#endif

	free(row_idx);
}


//==========================================================================================================================================
//= Ploting
//==========================================================================================================================================


#undef  csr_plot
#define csr_plot  CSR_UTIL_GEN_EXPAND(csr_plot)
void
csr_plot(char * title_base, _TYPE_I * row_ptr, _TYPE_I * col_idx, __attribute__((unused)) _TYPE_V * val, long m, long n, long nnz, int enable_legend)
{
	_TYPE_I * row_idx;
	long num_pixels = 1024;
	long num_pixels_x = (n < 1024) ? n : num_pixels;
	long num_pixels_y = (n < 1024) ? m : num_pixels;
	long buf_n = strlen(title_base) + 1 + 1000;
	char buf[buf_n], buf_title[buf_n];

	csr_row_indexes(row_ptr, col_idx, m, n, nnz, &row_idx);
	snprintf(buf, buf_n, "%s.png", title_base);
	snprintf(buf_title, buf_n, "%s", title_base);
	figure_simple_plot(buf, num_pixels_x, num_pixels_y, (col_idx, row_idx, NULL, nnz, 0),
		if (enable_legend)
			figure_enable_legend(_fig);
		figure_set_title(_fig, buf_title);
		figure_axes_flip_y(_fig);
		figure_set_bounds_x(_fig, 0, n);
		figure_set_bounds_y(_fig, 0, m);
		figure_series_type_density_map(_s);
	);
	free(row_idx);
}


//==========================================================================================================================================
//= Includes Undefs
//==========================================================================================================================================


#include "sort/samplesort/samplesort_gen_pop.h"
#include "sort/quicksort/quicksort_gen_pop.h"
#include "functools/functools_gen_pop.h"

