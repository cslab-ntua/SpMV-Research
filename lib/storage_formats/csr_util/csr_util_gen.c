#include <stdlib.h>
#include <stdio.h>

#include "macros/macrolib.h"
#include "debug.h"
#include "genlib.h"
#include "omp_functions.h"
#include "parallel_util.h"
#include "string_util.h"
#include "array_metrics.h"
#include "bit_ops.h"
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


// Reduce Add
#include "functools/functools_gen_push.h"
#define FUNCTOOLS_GEN_TYPE_1  int
#define FUNCTOOLS_GEN_TYPE_2  int
#define FUNCTOOLS_GEN_SUFFIX  CONCAT(_CSR_UTIL_GEN_add_i, CSR_UTIL_GEN_SUFFIX)
#include "functools/functools_gen.c"
__attribute__((pure))
static inline
int
functools_map_fun(int * A, long i)
{
	return A[i];
}
__attribute__((pure))
static inline
int
functools_reduce_fun(int a, int b)
{
	return a + b;
}


// Quicksort
#include "sort/quicksort/quicksort_gen_push.h"
#define QUICKSORT_GEN_TYPE_1  double
#define QUICKSORT_GEN_TYPE_2  int
#define QUICKSORT_GEN_TYPE_3  void
#define QUICKSORT_GEN_SUFFIX  CONCAT(_CSR_UTIL_GEN_d_sign, CSR_UTIL_GEN_SUFFIX)
#include "sort/quicksort/quicksort_gen.c"
static inline
int
quicksort_cmp(double a, double b, __attribute__((unused)) void * aux)
{
	if (a*b <= 0)
		return (a > b) ? 1 : (a < b) ? -1 : 0;
	return (fabs(a) > fabs(b)) ? 1 : (fabs(a) < fabs(b)) ? -1 : 0;
}
#undef quicksort_sign
#define quicksort_sign(A, N, aux, partitions_buf) CONCAT(quicksort_CSR_UTIL_GEN_d_sign, CSR_UTIL_GEN_SUFFIX)(A, N, aux, partitions_buf)


// Samplesort
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
#define _TYPE_V  CSR_UTIL_GEN_EXPAND_TYPE(_TYPE_V)
typedef CSR_UTIL_GEN_TYPE_1  _TYPE_V;

#undef  _TYPE_I
#define _TYPE_I  CSR_UTIL_GEN_EXPAND_TYPE(_TYPE_I)
typedef CSR_UTIL_GEN_TYPE_2  _TYPE_I;


//==========================================================================================================================================
//------------------------------------------------------------------------------------------------------------------------------------------
//-                                                              Templates                                                                 -
//------------------------------------------------------------------------------------------------------------------------------------------
//==========================================================================================================================================


//==========================================================================================================================================
//= Helpers
//==========================================================================================================================================


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

#undef  get_double_log2
#define get_double_log2  CSR_UTIL_GEN_EXPAND(get_double_log2)
static inline
double
get_double_log2(void * A, long i)
{
	double val = ((double *) A)[i];
	return (val == 0) ? 0 : log2(val);
}

#undef  get_double_abs_log2
#define get_double_abs_log2  CSR_UTIL_GEN_EXPAND(get_double_abs_log2)
static inline
double
get_double_abs_log2(void * A, long i)
{
	double val = fabs(((double *) A)[i]);
	return (val == 0) ? 0 : log2(val);
	// return (val == 0) ? -1022 : log2(val);  // If 0 return the smallest exponent for double floating point values instead of -infinite.
}

#undef  get_double_sign
#define get_double_sign  CSR_UTIL_GEN_EXPAND(get_double_sign)
static inline
double
get_double_sign(void * A, long i)
{
	return bits_double_get_sign(((double *) A)[i]);
}

#undef  get_double_exponent
#define get_double_exponent  CSR_UTIL_GEN_EXPAND(get_double_exponent)
static inline
double
get_double_exponent(void * A, long i)
{
	return bits_double_get_exponent(((double *) A)[i]);
}

#undef  get_double_exponent_bits
#define get_double_exponent_bits  CSR_UTIL_GEN_EXPAND(get_double_exponent_bits)
static inline
double
get_double_exponent_bits(void * A, long i)
{
	return bits_double_get_exponent_bits(((double *) A)[i]);
}

#undef  get_double_fraction
#define get_double_fraction  CSR_UTIL_GEN_EXPAND(get_double_fraction)
static inline
double
get_double_fraction(void * A, long i)
{
	return bits_double_get_fraction(((double *) A)[i]);
}

#undef  get_double_upper_12_bits
#define get_double_upper_12_bits  CSR_UTIL_GEN_EXPAND(get_double_upper_12_bits)
static inline
double
get_double_upper_12_bits(void * A, long i)
{
	return bits_double_get_upper_12_bits(((double *) A)[i]);
}

#undef  get_double_surrounding_zeros
#define get_double_surrounding_zeros  CSR_UTIL_GEN_EXPAND(get_double_surrounding_zeros)
static inline
double
get_double_surrounding_zeros(void * A, long i)
{
	uint64_t u = ((uint64_t *) A)[i];
	return u == 0 ? 64 : __builtin_ctzl(u) + __builtin_clzl(u);
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


//==========================================================================================================================================
//= Structural Features
//==========================================================================================================================================


// Expand row indices.
#undef  csr_row_indices
#define csr_row_indices  CSR_UTIL_GEN_EXPAND(csr_row_indices)
void
csr_row_indices(_TYPE_I * row_ptr, __attribute__((unused)) _TYPE_I * col_idx, long m, __attribute__((unused)) long n, long nnz,
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


// Returns how many rows in matrix have less than "nnz_threshold" nonzeros
#undef  csr_count_short_rows
#define csr_count_short_rows  CSR_UTIL_GEN_EXPAND(csr_count_short_rows)
long 
csr_count_short_rows(_TYPE_I * row_ptr, long m, long nnz_threshold)
{
	long count = 0;
	long count_nnz = 0;
	// #pragma omp parallel
	{
		long i = 0;
		// #pragma omp for reduction(+:count,count_nnz)
		for (i=0;i<m;i++){
			if ((row_ptr[i + 1] - row_ptr[i]) < nnz_threshold){
				count++;
				count_nnz+=row_ptr[i + 1] - row_ptr[i];
			}
		}
	}
	printf("\tout of %d nonzeros, %ld (%.2lf %%) belong to short rows (fewer than %ld nonzeros)\n", row_ptr[m], count_nnz, count_nnz*100.0/row_ptr[m], nnz_threshold);
	printf("\tout of %ld rows, %ld (%.2lf %%) are short rows (fewer than %ld nonzeros)\n", m, count, count*100.0/m, nnz_threshold);
	
	double A_mem_footprint_original = ((m+1) * sizeof(int) + (row_ptr[m]) * (sizeof(int) + sizeof(double)))/(1024.0*1024);
	double A_mem_footprint_short = ((m+1) * sizeof(int) + (count_nnz) * (sizeof(int) + sizeof(double)))/(1024.0*1024);
	double A_mem_footprint_big = ((m+1) * sizeof(int) + (row_ptr[m] - count_nnz) * (sizeof(int) + sizeof(double)))/(1024.0*1024);
	printf("A_mem_footprint_original = %lf MB\n", A_mem_footprint_original);
	printf("A_mem_footprint_short = %lf MB\n", A_mem_footprint_short);
	printf("A_mem_footprint_big = %lf MB\n", A_mem_footprint_big);
	return count;
}

// Returns how many nonzeros are "distant" from others within same row (depending on "max_distance")
#undef  csr_count_distant_nonzeros
#define csr_count_distant_nonzeros  CSR_UTIL_GEN_EXPAND(csr_count_distant_nonzeros)
long 
csr_count_distant_nonzeros(_TYPE_I * row_ptr, _TYPE_I * col_idx, long m, long max_distance)
{
	long count = 0;
	#pragma omp parallel
	{
		long i = 0, j = 0, k = 0;
		#pragma omp for reduction(+:count)
		for (i=0;i<m;i++){
			long row_start = row_ptr[i];
			long row_end = row_ptr[i+1];
			long row_distance = 0;

			for(j=row_start;j<row_end;j++){
				// flag whether it is a distant nonzero or not. set to 1 (nnz distant from others)
				int flag = 1; 

				long min_distance = max_distance + 1; // initialize with maximum distance

				// Check left neighbors
				for(k = j-1; k >= row_start; k--) {
					long distance = col_idx[j]-col_idx[k];
					if(distance <= min_distance){
						// if element found closer than the maximum distance that is set, then this nonzero is not distant from others. 
						min_distance = distance;
						// no need to check other nonzeros, since col_idx is sorted. set flag to 0 and proceed to next nonzero.
						flag = 0;
						break;
					}
					// if distance is greater than max allowed distance, then this nonzero is distant from its left neighbors.
					if(distance > max_distance)
						break;
				}
				if(flag==1){
					// if not yet decided from left neighbors if nonzero is distant, have to check right neighbors too.
					// Check right neighbors
					for(k = j+1; k < row_end; k++) {
						long distance = col_idx[k] - col_idx[j];
						if(distance <= min_distance){
							// if element found closer than the maximum distance that is set, then this nonzero is not distant from others. 
							min_distance = distance;
							// no need to check other nonzeros, since col_idx is sorted. set flag to 0 and proceed to next nonzero.
							flag = 0;
							break;
						}
						// if distance is greater than max allowed distance, then this nonzero is distant from its right neighbors.
						if(distance > max_distance)
							break;
					}
				}

				if(flag)
					row_distance++;
			}
			count += row_distance;
		}
	}

	return count;
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
		long col, col_min, col_max;
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
			col_min = col_max = col_idx[j_s];
			for (j=j_s;j<j_e;j++)
			{
				col = col_idx[j];
				__atomic_fetch_add(&degrees_cols[col], 1, __ATOMIC_RELAXED);
				if (col < col_min)
					col_min = col;
				else if (col > col_max)
					col_max = col;
			}
			b = col_max - col_min;
			bandwidths[i] = b;
			s = (b > 0) ? degree / b : 0;
			scatters[i] = s;
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
csr_cross_row_neighbours(_TYPE_I * row_ptr, _TYPE_I * col_idx, long m, __attribute__((unused)) long n, __attribute__((unused)) long nnz, long window_size, _TYPE_I *crs_row)
{
	double * num_neigh = (typeof(num_neigh)) malloc((m) * sizeof(*num_neigh));
	long total_num_non_empty_rows = 0;
	#pragma omp parallel
	{
		long i, j, k, k_s, k_e, l;
		long degree, curr_neigh, column_diff;
		long num_non_empty_rows = 0;
		#pragma omp for
		for (i=0;i<m;i++){
			num_neigh[i] = 0;
			crs_row[i] = 0;
		}
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
				crs_row[i] = curr_neigh;
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


//------------------------------------------------------------------------------------------------------------------------------------------
//- Structural Features - Print
//------------------------------------------------------------------------------------------------------------------------------------------


#undef  csr_matrix_features
#define csr_matrix_features  CSR_UTIL_GEN_EXPAND(csr_matrix_features)
void
csr_matrix_features(char * title_base, _TYPE_I * row_ptr, _TYPE_I * col_idx, long m, long n, long nnz, int do_plot, long num_pixels_x, long num_pixels_y)
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

	long buf_n = strlen(title_base) + 1 + 1000;
	char buf[buf_n], buf_title[buf_n];
	double time;

	time = time_it(1,
		csr_row_indices(row_ptr, col_idx, m, n, nnz, &row_idx);
	);
	printf("time row indices = %lf\n", time);
	// Matrix structure, density map.
	if (do_plot)
	{
		snprintf(buf, buf_n, "%s.png", title_base);
		snprintf(buf_title, buf_n, "%s", title_base);
		figure_simple_plot(buf, num_pixels_x, num_pixels_y, (col_idx, row_idx, NULL, nnz, 0),
			figure_enable_legend(_fig);
			figure_set_title(_fig, buf_title);
			figure_axes_flip_y(_fig);
			figure_set_bounds_x(_fig, 0, n-1);
			figure_set_bounds_y(_fig, 0, m-1);
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
			figure_series_type_histogram(_s, 0, NULL, 1);
			figure_series_type_barplot(_s);
		);
		snprintf(buf, buf_n, "%s_degree_distribution_cumulative_sum.png", title_base);
		snprintf(buf_title, buf_n, "%s: degree distribution cumulative sum", title_base);
		figure_simple_plot(buf, num_pixels_x, num_pixels_y, (NULL, degrees_rows, NULL, m, 0),
			figure_enable_legend(_fig);
			figure_set_title(_fig, buf_title);
			figure_series_type_histogram(_s, 0, NULL, 1, 1);
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
			figure_series_type_histogram(_s, 0, NULL, 1);
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
			figure_series_type_histogram(_s, 0, NULL, 1);
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
		csr_row_indices(row_ptr, col_idx, m, n, nnz, &row_idx);
	);
	printf("time row indices = %lf\n", time);

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
//= Value Features
//==========================================================================================================================================


// Value differences of neighbouring nnz.
#undef  csr_value_differences_of_neighbors
#define csr_value_differences_of_neighbors  CSR_UTIL_GEN_EXPAND(csr_value_differences_of_neighbors)
static inline
void
csr_value_differences_of_neighbors(char * title_base, double * vals, __attribute__((unused)) long m, __attribute__((unused)) long n, long nnz, int do_plot, long num_pixels_x, long num_pixels_y)
{
	long buf_n = strlen(title_base) + 1 + 1000;
	char buf[buf_n], buf_title[buf_n];
	long num_bins = 1024;

	double * vals_diff;
	double vals_diff_min, vals_diff_max, vals_diff_avg, vals_diff_std, vals_diff_sz_avg, vals_diff_sz_std;
	// double vals_diff_grouped_sz_avg;
	double vals_diff_abs_min, vals_diff_abs_max, vals_diff_abs_avg, vals_diff_abs_std;
	double vals_diff_exp_min, vals_diff_exp_max, vals_diff_exp_avg, vals_diff_exp_std;

	vals_diff = (typeof(vals_diff)) malloc(nnz * sizeof(*vals_diff));
	vals_diff[0] = vals[0];
	#pragma omp parallel
	{
		long i;
		#pragma omp for
		for (i=1;i<nnz;i++)
		{
			union {
				uint64_t u;
				double f;
			} val, val_prev, diff;
			val.f = vals[i];
			val_prev.f = vals[i-1];
			diff.u = val.u - val_prev.u;
			vals_diff[i] = diff.f;
		}
	}
	array_min_max(vals_diff, nnz, &vals_diff_min, NULL, &vals_diff_max, NULL);
	array_mean(vals_diff, nnz, &vals_diff_avg);
	array_std(vals_diff, nnz, &vals_diff_std);
	array_mean(vals_diff, nnz, &vals_diff_sz_avg, get_double_surrounding_zeros);
	array_std(vals_diff, nnz, &vals_diff_sz_std, get_double_surrounding_zeros);
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
			figure_series_type_histogram(_s, num_bins, NULL, 1);
			figure_series_type_barplot(_s);
		);
		snprintf(buf, buf_n, "%s_values_diff_exp_distribution.png", title_base);
		snprintf(buf_title, buf_n, "%s: values differences exponent distribution", title_base);
		figure_simple_plot(buf, num_pixels_x, num_pixels_y, (NULL, vals_diff, NULL, nnz, 0, , get_double_exponent),
			figure_enable_legend(_fig);
			figure_set_title(_fig, buf_title);
			figure_series_type_histogram(_s, 0, NULL, 1); // Integer mode.
			figure_series_type_barplot(_s);
		);
		snprintf(buf, buf_n, "%s_values_diff_frac_distribution.png", title_base);
		snprintf(buf_title, buf_n, "%s: values differences fraction distribution", title_base);
		figure_simple_plot(buf, num_pixels_x, num_pixels_y, (NULL, vals_diff, NULL, nnz, 0, , get_double_fraction),
			figure_enable_legend(_fig);
			figure_set_title(_fig, buf_title);
			figure_series_type_histogram(_s, num_bins, NULL, 1);
			figure_series_type_barplot(_s);
		);
	}
	fprintf(stderr, "vals_diff min = %g\n", vals_diff_min);
	fprintf(stderr, "vals_diff max = %g\n", vals_diff_max);
	fprintf(stderr, "vals_diff avg = %g\n", vals_diff_avg);
	fprintf(stderr, "vals_diff std = %g\n", vals_diff_std);
	fprintf(stderr, "vals_diff sz avg = %.3lf\n", vals_diff_sz_avg);
	fprintf(stderr, "vals_diff sz std = %.3lf\n", vals_diff_sz_std);
	/* for (sz_group_size=4;sz_group_size<=16;sz_group_size+=4)
	{
		array_mean_of_windowed_sz_min(vals_diff, nnz, sz_group_size, &vals_diff_grouped_sz_avg);
		fprintf(stderr, "vals_diff grouped (%ld) sz avg of min = %.3lf\n", sz_group_size, vals_diff_grouped_sz_avg);
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
}


// Sorting.
#undef  csr_value_differences_of_sorted_subsets
#define csr_value_differences_of_sorted_subsets  CSR_UTIL_GEN_EXPAND(csr_value_differences_of_sorted_subsets)
static inline
void
csr_value_differences_of_sorted_subsets(double * vals, long nnz, long deduplicate)
{
	double * diffs;
	double avg;
	long window_size;
	long k;
	long nnz_dedup;
	diffs = (typeof(diffs)) malloc(nnz * sizeof(*diffs));
	for (k=4;k<=24;k++)
	{
		nnz_dedup = 0;
		window_size = 1 << k;
		#pragma omp parallel
		{
			double * window = (typeof(window)) malloc(window_size * sizeof(*window));
			int * qsort_partitions = (typeof(qsort_partitions)) malloc(window_size * sizeof(*qsort_partitions));
			long i, j, j_e, k;
			long t_nnz_dedup = 0;
			union {
				uint64_t u;
				double f;
			} val, val_prev, diff;
			#pragma omp for
			for (i=0;i<nnz;i+=window_size)
			{
				j_e = i + window_size;
				if (j_e > nnz)
					j_e = nnz;
				for (j=i;j<j_e;j++)
					window[j-i] = vals[j];
				quicksort_sign(window, j_e-i, NULL, qsort_partitions);
				if (deduplicate)
				{
					val.u = -1LL;   // a value with no zeros, so that 'get_double_surrounding_zeros()' returns zero;
					for (j=i;j<j_e;j++)
						diffs[j] = val.f;
					k = 0;
					j = i;
					while (j<j_e)
					{
						window[k] = window[j-i];
						k++;
						j++;
						for (;j<j_e;j++)
						{
							if (window[j-i] != window[j-1-i])
								break;
						}
					}
					j_e = i + k;
					t_nnz_dedup += k;
				}
				diffs[i] = window[0];
				for (j=i+1;j<j_e;j++)
				{
					val.f = window[j-i];
					val_prev.f = window[j-1-i];
					diff.u = val.u - val_prev.u;
					diffs[j] = diff.f;
				}
			}
			__atomic_fetch_add(&nnz_dedup, t_nnz_dedup, __ATOMIC_RELAXED);
			free(window);
			free(qsort_partitions);
		}
		array_mean(diffs, nnz, &avg, get_double_surrounding_zeros);
		if (deduplicate)
			avg = (avg * nnz) / nnz_dedup;
		fprintf(stderr, "vals_diff_sorted_window (2^%ld) sz avg = %.3lf\n", k, avg);
		fprintf(stderr, "vals_diff_sorted_window (2^%ld) UF = %g\n", k, ((double) nnz_dedup) / nnz);
	}
	free(diffs);
}


// Clustering - Distance from cluster centers.
#undef  csr_value_distances_from_cluster_centers
#define csr_value_distances_from_cluster_centers  CSR_UTIL_GEN_EXPAND(csr_value_distances_from_cluster_centers)
static inline
void
csr_value_distances_from_cluster_centers(char * title_base, double * vals, double * vals_sorted, __attribute__((unused)) long m, __attribute__((unused)) long n, long nnz, int do_plot, long num_pixels_x, long num_pixels_y)
{
	long buf_n = strlen(title_base) + 1 + 1000;
	char buf[buf_n], buf_title[buf_n];
	// long num_bins = 1024;
	double time;

	double vals_min;
	// double vals_grouped_ctz_avg;
	// double vals_abs_min, vals_abs_max, vals_abs_avg, vals_abs_std;
	// double vals_exp_min, vals_exp_max, vals_exp_avg, vals_exp_std;
	// long vals_unique_num, vals_sgnexp_unique_num;
	// uint64_t vals_unique_bits_num;

	// long window_size;
	long k;
	__attribute__((unused)) long i, i_s, i_e;

	double * vals_sorted_shifted_by_min;
	double * vals_cluster_centers_diffs;
	double vals_cluster_centers_diffs_min, vals_cluster_centers_diffs_max, vals_cluster_centers_diffs_avg, vals_cluster_centers_diffs_std, vals_cluster_centers_diffs_ctz_avg, vals_cluster_centers_diffs_ctz_std, vals_cluster_centers_diffs_grouped_ctz_avg;
	long best_k, ctz_group_size, best_ctz_group_size;
	double ctz_corrected, ctz_corrected_best;

	array_min_max(vals, nnz, &vals_min, NULL, NULL, NULL);

	vals_cluster_centers_diffs = (typeof(vals_cluster_centers_diffs)) malloc(nnz * sizeof(*vals_cluster_centers_diffs));
	vals_sorted_shifted_by_min = (typeof(vals_sorted_shifted_by_min)) malloc(nnz * sizeof(*vals_sorted_shifted_by_min));
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
				long center_pos = macros_binary_search(cluster_centers, 0, num_clusters - 1, vals[i], NULL, NULL);
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
			vals_cluster_centers_diffs_grouped_ctz_avg = array_mean_of_windowed_ctz_min(vals_cluster_centers_diffs, nnz, ctz_group_size);
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
			s = figure_add_series(fig, vals, NULL, NULL, nnz, 0, , get_double_zero);
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
	free(vals_cluster_centers_diffs);
}


#undef  csr_value_features
#define csr_value_features  CSR_UTIL_GEN_EXPAND(csr_value_features)
void
csr_value_features(char * title_base, _TYPE_I * row_ptr, _TYPE_I * col_idx, _TYPE_V * values, long m, long n, long nnz, int do_plot, long num_pixels_x, long num_pixels_y)
{
	_TYPE_I * row_idx;
	double * vals;
	double * vals_sorted;
	double * vals_sorted_diff;
	double * vals_sorted_ratio_abs;
	double * vals_sorted_diff_fraction_abs;

	long buf_n = strlen(title_base) + 1 + 1000;
	char buf[buf_n], buf_title[buf_n];
	__attribute__((unused)) double time;
	__attribute__((unused)) long num_bins = 1024;

	double vals_min, vals_max, vals_avg, vals_std, vals_ctz_avg, vals_ctz_std;
	// double vals_grouped_ctz_avg;
	double vals_abs_min, vals_abs_max, vals_abs_avg, vals_abs_std;
	double vals_exp_min, vals_exp_max, vals_exp_avg, vals_exp_std;
	long vals_unique_num, vals_sgnexp_unique_num;
	uint64_t vals_unique_bits_num;
	__attribute__((unused)) long i, j, k, l, s, t;

	// long do_print_features = 0;
	long do_print_features = 1;

	if (nnz == 0)
		error("empty array");

	csr_row_indices(row_ptr, col_idx, m, n, nnz, &row_idx);

	vals = (typeof(vals)) malloc(nnz * sizeof(*vals));
	vals_sorted = (typeof(vals_sorted)) malloc(nnz * sizeof(*vals_sorted));
	vals_sorted_diff = (typeof(vals_sorted_diff)) malloc(nnz * sizeof(*vals_sorted_diff));
	vals_sorted_ratio_abs = (typeof(vals_sorted_ratio_abs)) malloc(nnz * sizeof(*vals_sorted_ratio_abs));
	vals_sorted_diff_fraction_abs = (typeof(vals_sorted_diff_fraction_abs)) malloc(nnz * sizeof(*vals_sorted_diff_fraction_abs));
	#pragma omp parallel
	{
		long i;
		#pragma omp for
		for (i=0;i<nnz;i++)
			vals[i] = (double) values[i];
	}
	if (do_print_features)
	{
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
		bits_u64_required_bits_for_binary_representation(vals_unique_num, &vals_unique_bits_num, NULL);
	}
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
	vals_sorted_diff[0] = 0;
	vals_sorted_ratio_abs[0] = 0;
	vals_sorted_diff_fraction_abs[0] = 0;
	#pragma omp parallel
	{
		double val, val_prev, diff;
		long i;
		#pragma omp for
		for (i=1;i<nnz;i++)
		{
			val_prev = vals_sorted[i-1];
			val = vals_sorted[i];
			diff = val - val_prev;
			vals_sorted_diff[i] = diff;
			if (val == 0 || val_prev == 0)
				vals_sorted_ratio_abs[i] = 0;
			else
			{
				vals_sorted_ratio_abs[i] = fabs(val / val_prev);
			}
			if (diff == 0 || val_prev == 0)
				vals_sorted_diff_fraction_abs[i] = 0;
			else
			{
				vals_sorted_diff_fraction_abs[i] = fabs(diff / val_prev);
			}
		}
	}

	if (do_print_features)
	{
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
		fprintf(stderr, "vals unique fraction = %g\n", ((double) vals_unique_num) / ((double) nnz));
		fprintf(stderr, "vals unique index size bits = %ld\n", vals_unique_bits_num);
		fprintf(stderr, "vals unique index size bytes = %ld\n", bits_num_bits_to_num_bytes(vals_unique_bits_num));
	}

	if (do_plot)
	{
		long size, size_sub, gap;
		long window_size, subwindow_size;
		double * window, * subwindow;

		// snprintf(buf, buf_n, "%s_values_heatmap.png", title_base);
		// snprintf(buf_title, buf_n, "%s: values heatmap", title_base);
		// figure_simple_plot(buf, num_pixels_x, num_pixels_y, (col_idx, row_idx, vals, nnz, 0),
			// figure_enable_legend(_fig);
			// figure_set_title(_fig, buf_title);
			// figure_axes_flip_y(_fig);
			// figure_set_bounds_x(_fig, 0, n-1);
			// figure_set_bounds_y(_fig, 0, m-1);
		// );
		// snprintf(buf, buf_n, "%s_values.png", title_base);
		// snprintf(buf_title, buf_n, "%s: values (row-major order)", title_base);
		// figure_simple_plot(buf, num_pixels_x, num_pixels_y, (NULL, vals, NULL, nnz, 0),
			// figure_enable_legend(_fig);
			// figure_set_title(_fig, buf_title);
		// );
		// snprintf(buf, buf_n, "%s_values_1D.png", title_base);
		// snprintf(buf_title, buf_n, "%s: values 1D", title_base);
		// figure_simple_plot(buf, num_pixels_x, num_pixels_y, (values, NULL, NULL, nnz, 0, , get_double_zero),
			// figure_enable_legend(_fig);
			// figure_set_title(_fig, buf_title);
			// figure_set_bounds_y(_fig, -1, 1);
			// figure_series_type_density_map(_s);
		// );
		// snprintf(buf, buf_n, "%s_values_distribution.png", title_base);
		// snprintf(buf_title, buf_n, "%s: values distribution", title_base);
		// figure_simple_plot(buf, num_pixels_x, num_pixels_y, (NULL, vals, NULL, nnz, 0),
			// figure_enable_legend(_fig);
			// figure_set_title(_fig, buf_title);
			// figure_series_type_histogram(_s, num_bins, NULL, 1);
			// figure_series_type_barplot(_s);
		// );
		// snprintf(buf, buf_n, "%s_values_exp_distribution.png", title_base);
		// snprintf(buf_title, buf_n, "%s: values exponent distribution", title_base);
		// figure_simple_plot(buf, num_pixels_x, num_pixels_y, (NULL, vals, NULL, nnz, 0, , get_double_exponent),
			// figure_enable_legend(_fig);
			// figure_set_title(_fig, buf_title);
			// figure_series_type_histogram(_s, 0, NULL, 1); // Integer mode.
			// figure_series_type_barplot(_s);
		// );
		// snprintf(buf, buf_n, "%s_values_frac_distribution.png", title_base);
		// snprintf(buf_title, buf_n, "%s: values fraction distribution", title_base);
		// figure_simple_plot(buf, num_pixels_x, num_pixels_y, (NULL, vals, NULL, nnz, 0, , get_double_fraction),
			// figure_enable_legend(_fig);
			// figure_set_title(_fig, buf_title);
			// figure_series_type_histogram(_s, num_bins, NULL, 1);
			// figure_series_type_barplot(_s);
		// );

		// snprintf(buf, buf_n, "%s_values_sorted.png", title_base);
		// snprintf(buf_title, buf_n, "%s: values sorted", title_base);
		// figure_simple_plot(buf, num_pixels_x, num_pixels_y, (NULL, vals_sorted, NULL, nnz, 0),
			// figure_enable_legend(_fig);
			// figure_set_title(_fig, buf_title);
		// );
		// snprintf(buf, buf_n, "%s_values_sorted_abs_log2.png", title_base);
		// snprintf(buf_title, buf_n, "%s: values sorted: absolute, log2", title_base);
		// figure_simple_plot(buf, num_pixels_x, num_pixels_y, (NULL, vals_sorted, NULL, nnz, 0, , get_double_abs_log2),
			// figure_enable_legend(_fig);
			// figure_set_title(_fig, buf_title);
		// );
		// snprintf(buf, buf_n, "%s_values_sorted_diff.png", title_base);
		// snprintf(buf_title, buf_n, "%s: values sorted diff", title_base);
		// figure_simple_plot(buf, num_pixels_x, num_pixels_y, (NULL, vals_sorted_diff, NULL, nnz, 0),
			// figure_enable_legend(_fig);
			// figure_set_title(_fig, buf_title);
		// );
		// snprintf(buf, buf_n, "%s_values_sorted_diff_abs_log2.png", title_base);
		// snprintf(buf_title, buf_n, "%s: values sorted: diff, absolute, log2", title_base);
		// figure_simple_plot(buf, num_pixels_x, num_pixels_y, (NULL, vals_sorted_diff, NULL, nnz, 0, , get_double_abs_log2),
			// figure_enable_legend(_fig);
			// figure_set_title(_fig, buf_title);
		// );
		// snprintf(buf, buf_n, "%s_values_sorted_diff_abs_log2_bounded_median_curve.png", title_base);
		// snprintf(buf_title, buf_n, "%s: values sorted: diff, absolute, log2 - bounded median curve", title_base);
		// figure_simple_plot(buf, num_pixels_x, num_pixels_y, (NULL, vals_sorted_diff, NULL, nnz, 0, , get_double_abs_log2),
			// figure_enable_legend(_fig);
			// figure_set_title(_fig, buf_title);
			// figure_series_type_bounded_median_curve(_s, 0);
		// );

		// k = 14;
		// window_size = 1ULL << k;
		// window = (typeof(window)) malloc(window_size * sizeof(*window));
		// gap = nnz / 10;
		// for (l=0;l<10;l++)
		// {
			// double val, val_prev, diff;
			// i = l * gap;
			// size = (window_size < nnz - i) ?  window_size : nnz - i;
			// for (j=0;j<size;j++)
				// window[j] = vals[i+j];
			// quicksort(window, size, NULL, NULL);
			// for (j=size-1;j>=1;j--)
			// {
				// val = window[j];
				// val_prev = window[j-1];
				// diff = val - val_prev;
				// if (diff == 0 || val_prev == 0)
					// window[j] = 0;
				// else
				// {
					// window[j] = val - val_prev;
				// }
			// }
			// window[0] = 0;
			// snprintf(buf, buf_n, "%s_values_sorted_diff_abs_log2_window_%ld.png", title_base, l);
			// snprintf(buf_title, buf_n, "%s: values sorted: diff, absolute, log2 - window [%ld, %ld)", title_base, i, i+size);
			// figure_simple_plot(buf, num_pixels_x, num_pixels_y, (NULL, window, NULL, size, 0, , get_double_abs_log2),
				// figure_enable_legend(_fig);
				// figure_set_title(_fig, buf_title);
			// );
			// snprintf(buf, buf_n, "%s_values_sorted_diff_abs_log2_bounded_median_curve_window_%ld.png", title_base, l);
			// snprintf(buf_title, buf_n, "%s: values sorted: diff, absolute, log2 - bounded median curve - window [%ld, %ld)", title_base, i, i+size);
			// figure_simple_plot(buf, num_pixels_x, num_pixels_y, (NULL, window, NULL, size, 0, , get_double_abs_log2),
				// figure_enable_legend(_fig);
				// figure_set_title(_fig, buf_title);
				// figure_series_set_dot_size_pixels(_s, 4);
				// figure_series_type_bounded_median_curve(_s, 0);
			// );
		// }
		// free(window);


		// snprintf(buf, buf_n, "%s_values_sorted_ratio_abs.png", title_base);
		// snprintf(buf_title, buf_n, "%s: values sorted: ratio, absolute", title_base);
		// figure_simple_plot(buf, num_pixels_x, num_pixels_y, (NULL, vals_sorted_ratio_abs, NULL, nnz, 0),
			// figure_enable_legend(_fig);
			// figure_set_title(_fig, buf_title);
		// );
		// snprintf(buf, buf_n, "%s_values_sorted_ratio_abs_bounded_median_curve.png", title_base);
		// snprintf(buf_title, buf_n, "%s: values sorted: ratio, absolute - bounded median curve", title_base);
		// figure_simple_plot(buf, num_pixels_x, num_pixels_y, (NULL, vals_sorted_ratio_abs, NULL, nnz, 0),
			// figure_enable_legend(_fig);
			// figure_set_title(_fig, buf_title);
			// figure_series_type_bounded_median_curve(_s, 0);
		// );
		// snprintf(buf, buf_n, "%s_values_sorted_ratio_abs_log2.png", title_base);
		// snprintf(buf_title, buf_n, "%s: values sorted: ratio, absolute, log2", title_base);
		// figure_simple_plot(buf, num_pixels_x, num_pixels_y, (NULL, vals_sorted_ratio_abs, NULL, nnz, 0, , get_double_abs_log2),
			// figure_enable_legend(_fig);
			// figure_set_title(_fig, buf_title);
		// );
		// snprintf(buf, buf_n, "%s_values_sorted_ratio_abs_log2_bounded_median_curve.png", title_base);
		// snprintf(buf_title, buf_n, "%s: values sorted: ratio, absolute, log2 - bounded median curve", title_base);
		// figure_simple_plot(buf, num_pixels_x, num_pixels_y, (NULL, vals_sorted_ratio_abs, NULL, nnz, 0, , get_double_abs_log2),
			// figure_enable_legend(_fig);
			// figure_set_title(_fig, buf_title);
			// figure_series_type_bounded_median_curve(_s, 0);
		// );
		// double bound;
		// long div2;
		// for (div2=1,bound=1.0;div2<21;div2++,bound/=2.0)
		// {
			// snprintf(buf, buf_n, "%s_values_sorted_ratio_abs_log2_zoomed_y_2^-%02ld.png", title_base, div2);
			// snprintf(buf_title, buf_n, "%s: values sorted: ratio, absolute, log2 - zoomed y axis: [0, 2^-%ld]", title_base, div2);
			// figure_simple_plot(buf, num_pixels_x, num_pixels_y, (NULL, vals_sorted_ratio_abs, NULL, nnz, 0, , get_double_abs_log2),
				// figure_enable_legend(_fig);
				// figure_set_title(_fig, buf_title);
				// figure_set_bounds_y(_fig, -bound, bound);
			// );
			// snprintf(buf, buf_n, "%s_values_sorted_ratio_abs_log2_bounded_median_curve_zoomed_y_2^-%02ld.png", title_base, div2);
			// snprintf(buf_title, buf_n, "%s: values sorted: ratio, absolute, log2 - zoomed y axis: [0, 2^-%ld] - bounded median curve", title_base, div2);
			// figure_simple_plot(buf, num_pixels_x, num_pixels_y, (NULL, vals_sorted_ratio_abs, NULL, nnz, 0, , get_double_abs_log2),
				// figure_enable_legend(_fig);
				// figure_set_title(_fig, buf_title);
				// figure_set_bounds_y(_fig, -bound, bound);
				// figure_series_set_dot_size_pixels(_s, 4);
				// figure_series_type_bounded_median_curve(_s, 0);
			// );
		// }

		// k = 14;
		// window_size = 1ULL << k;
		// window = (typeof(window)) malloc(window_size * sizeof(*window));
		// gap = nnz / 10;
		// for (l=0;l<10;l++)
		// {
			// double val, val_prev;
			// i = l * gap;
			// size = (window_size < nnz - i) ?  window_size : nnz - i;
			// for (j=0;j<size;j++)
				// window[j] = vals[i+j];
			// quicksort(window, size, NULL, NULL);
			// for (j=size-1;j>=1;j--)
			// {
				// val = window[j];
				// val_prev = window[j-1];
				// if (val == 0 || val_prev == 0)
					// window[j] = 0;
				// else
				// {
					// window[j] = fabs(val / val_prev);
				// }
			// }
			// window[0] = 0;
			// snprintf(buf, buf_n, "%s_values_sorted_ratio_abs_log2_window_%ld.png", title_base, l);
			// snprintf(buf_title, buf_n, "%s: values sorted: ratio, absolute, log2 - window [%ld, %ld)", title_base, i, i+size);
			// figure_simple_plot(buf, num_pixels_x, num_pixels_y, (NULL, window, NULL, size, 0, , get_double_abs_log2),
				// figure_enable_legend(_fig);
				// figure_set_title(_fig, buf_title);
			// );
			// snprintf(buf, buf_n, "%s_values_sorted_ratio_abs_log2_bounded_median_curve_window_%ld.png", title_base, l);
			// snprintf(buf_title, buf_n, "%s: values sorted: ratio, absolute, log2 - bounded median curve - window [%ld, %ld)", title_base, i, i+size);
			// figure_simple_plot(buf, num_pixels_x, num_pixels_y, (NULL, window, NULL, size, 0, , get_double_abs_log2),
				// figure_enable_legend(_fig);
				// figure_set_title(_fig, buf_title);
				// figure_series_set_dot_size_pixels(_s, 4);
				// figure_series_type_bounded_median_curve(_s, 0);
			// );
			// long div2 = 9;
			// double bound = 1.0 / (1ULL << div2);
			// snprintf(buf, buf_n, "%s_values_sorted_ratio_abs_log2_bounded_median_curve_window_%ld_zoomed_y_2^-%02ld.png", title_base, l, div2);
			// snprintf(buf_title, buf_n, "%s: values sorted: ratio, absolute, log2 - zoomed y axis: [0, 2^-%ld] - bounded median curve - window [%ld, %ld)", title_base, div2, i, i+size);
			// figure_simple_plot(buf, num_pixels_x, num_pixels_y, (NULL, window, NULL, size, 0, , get_double_abs_log2),
				// figure_enable_legend(_fig);
				// figure_set_title(_fig, buf_title);
				// figure_set_bounds_y(_fig, -bound, bound);
				// figure_series_set_dot_size_pixels(_s, 4);
				// figure_series_type_bounded_median_curve(_s, 0);
			// );
		// }
		// free(window);


		// snprintf(buf, buf_n, "%s_values_sorted_diff_fraction_abs.png", title_base);
		// snprintf(buf_title, buf_n, "%s: values sorted: diff, fraction, absolute", title_base);
		// figure_simple_plot(buf, num_pixels_x, num_pixels_y, (NULL, vals_sorted_diff_fraction_abs, NULL, nnz, 0),
			// figure_enable_legend(_fig);
			// figure_set_title(_fig, buf_title);
		// );
		// snprintf(buf, buf_n, "%s_values_sorted_diff_fraction_abs_bounded_median_curve.png", title_base);
		// snprintf(buf_title, buf_n, "%s: values sorted: diff, fraction, absolute - bounded median curve", title_base);
		// figure_simple_plot(buf, num_pixels_x, num_pixels_y, (NULL, vals_sorted_diff_fraction_abs, NULL, nnz, 0),
			// figure_enable_legend(_fig);
			// figure_set_title(_fig, buf_title);
			// figure_series_set_dot_size_pixels(_s, 4);
			// figure_series_type_bounded_median_curve(_s, 0);
		// );
		// snprintf(buf, buf_n, "%s_values_sorted_diff_fraction_abs_log2.png", title_base);
		// snprintf(buf_title, buf_n, "%s: values sorted: diff, fraction, absolute, log2", title_base);
		// figure_simple_plot(buf, num_pixels_x, num_pixels_y, (NULL, vals_sorted_diff_fraction_abs, NULL, nnz, 0, , get_double_abs_log2),
			// figure_enable_legend(_fig);
			// figure_set_title(_fig, buf_title);
		// );
		// snprintf(buf, buf_n, "%s_values_sorted_diff_fraction_abs_log2_bounded_median_curve.png", title_base);
		// snprintf(buf_title, buf_n, "%s: values sorted: diff, fraction, absolute, log2 - bounded median curve", title_base);
		// figure_simple_plot(buf, num_pixels_x, num_pixels_y, (NULL, vals_sorted_diff_fraction_abs, NULL, nnz, 0, , get_double_abs_log2),
			// figure_enable_legend(_fig);
			// figure_set_title(_fig, buf_title);
			// figure_series_set_dot_size_pixels(_s, 4);
			// figure_series_type_bounded_median_curve(_s, 0);
		// );

		// k = 14;
		// window_size = 1ULL << k;
		// window = (typeof(window)) malloc(window_size * sizeof(*window));
		// gap = nnz / 10;
		// for (l=0;l<10;l++)
		// {
			// double val, val_prev, diff;
			// i = l * gap;
			// size = (window_size < nnz - i) ?  window_size : nnz - i;
			// for (j=0;j<size;j++)
				// window[j] = vals[i+j];
			// quicksort(window, size, NULL, NULL);
			// for (j=size-1;j>=1;j--)
			// {
				// val = window[j];
				// val_prev = window[j-1];
				// diff = val - val_prev;
				// if (diff == 0 || val_prev == 0)
					// window[j] = 0;
				// else
				// {
					// window[j] = fabs(diff / val_prev);
				// }
			// }
			// window[0] = 0;
			// snprintf(buf, buf_n, "%s_values_sorted_diff_fraction_abs_log2_window_%ld.png", title_base, l);
			// snprintf(buf_title, buf_n, "%s: values sorted: diff, fraction, absolute, log2 - window [%ld, %ld)", title_base, i, i+size);
			// figure_simple_plot(buf, num_pixels_x, num_pixels_y, (NULL, window, NULL, size, 0, , get_double_abs_log2),
				// figure_enable_legend(_fig);
				// figure_set_title(_fig, buf_title);
			// );
			// snprintf(buf, buf_n, "%s_values_sorted_diff_fraction_abs_log2_bounded_median_curve_window_%ld.png", title_base, l);
			// snprintf(buf_title, buf_n, "%s: values sorted: diff, fraction, absolute, log2 - bounded median curve - window [%ld, %ld)", title_base, i, i+size);
			// figure_simple_plot(buf, num_pixels_x, num_pixels_y, (NULL, window, NULL, size, 0, , get_double_abs_log2),
				// figure_enable_legend(_fig);
				// figure_set_title(_fig, buf_title);
				// figure_series_set_dot_size_pixels(_s, 4);
				// figure_series_type_bounded_median_curve(_s, 0);
			// );
		// }
		// free(window);


		/* k = 14;
		long window_size = 1ULL << k;
		double * window = (typeof(window)) malloc(window_size * sizeof(*window));
		long size, gap;
		// Print 10 sample windows spread in the nnz.
		gap = nnz / 10;
		for (l=0;l<10;l++)
		{
			i = l * gap;
			size = (window_size < nnz - i) ?  window_size : nnz - i;
			for (j=0;j<size;j++)
			{
				window[j] = vals[i+j];
			}
			quicksort(window, size, NULL, NULL);
			snprintf(buf, buf_n, "%s_values_sorted_window_%ld.png", title_base, l);
			snprintf(buf_title, buf_n, "%s: values sorted window [%ld, %ld)", title_base, i, i+size);
			figure_simple_plot(buf, num_pixels_x, num_pixels_y, (NULL, window, NULL, size, 0),
				figure_enable_legend(_fig);
				figure_set_title(_fig, buf_title);
			);
			snprintf(buf, buf_n, "%s_values_sorted_log2_window_%ld.png", title_base, l);
			snprintf(buf_title, buf_n, "%s: values sorted log2 window [%ld, %ld)", title_base, i, i+size);
			figure_simple_plot(buf, num_pixels_x, num_pixels_y, (NULL, window, NULL, size, 0, , get_double_abs_log2),
				figure_enable_legend(_fig);
				figure_set_title(_fig, buf_title);
			);
		}
		free(window); */

		k = 14;
		window_size = 1ULL << k;
		subwindow_size = 1ULL << 6;
		window = (typeof(window)) malloc(window_size * sizeof(*window));
		subwindow = (typeof(subwindow)) malloc(window_size * sizeof(*subwindow));
		gap = nnz / 5;
		for (l=0;l<5;l++)
		{
			double val, val_prev;
			i = l * gap;
			size = (window_size < nnz - i) ?  window_size : nnz - i;
			for (j=0;j<size;j++)
				window[j] = vals[i+j];
			quicksort(window, size, NULL, NULL);
			for (s=0;s<5;s++)
			{
				t = s * window_size / 5;
				size_sub = (subwindow_size < nnz - t) ?  subwindow_size : nnz - t;
				for (j=0;j<size_sub;j++)
				{
					subwindow[j] = window[t+j];
				}
				snprintf(buf, buf_n, "%s_values_sorted_subwindow_%ld_%ld.png", title_base, l, s);
				snprintf(buf_title, buf_n, "%s: values sorted - window [%ld, %ld) subwindow [%ld, %ld)", title_base, i, i+size, t, t+size_sub);
				figure_simple_plot(buf, num_pixels_x, num_pixels_y, (NULL, subwindow, NULL, size_sub, 0),
					figure_enable_legend(_fig);
					figure_set_title(_fig, buf_title);
					figure_series_set_dot_size_pixels(_s, 4);
				);
				for (j=1;j<size_sub;j++)
				{
					val = window[t+j];
					val_prev = window[t+j-1];
					subwindow[j] = val - val_prev;
				}
				subwindow[0] = 0;
				snprintf(buf, buf_n, "%s_values_sorted_diff_subwindow_%ld_%ld.png", title_base, l, s);
				snprintf(buf_title, buf_n, "%s: values sorted: diff - window [%ld, %ld) subwindow [%ld, %ld)", title_base, i, i+size, t, t+size_sub);
				figure_simple_plot(buf, num_pixels_x, num_pixels_y, (NULL, subwindow, NULL, size_sub, 0),
					figure_enable_legend(_fig);
					figure_set_title(_fig, buf_title);
					figure_series_set_dot_size_pixels(_s, 4);
				);
				for (j=1;j<size_sub;j++)
				{
					val = window[t+j];
					val_prev = window[t+j-1];
					if (val == 0 || val_prev == 0)
						subwindow[j] = 0;
					else
						subwindow[j] = val / val_prev;
				}
				subwindow[0] = 1;
				snprintf(buf, buf_n, "%s_values_sorted_ratio_subwindow_%ld_%ld.png", title_base, l, s);
				snprintf(buf_title, buf_n, "%s: values sorted: ratio - window [%ld, %ld) subwindow [%ld, %ld)", title_base, i, i+size, t, t+size_sub);
				figure_simple_plot(buf, num_pixels_x, num_pixels_y, (NULL, subwindow, NULL, size_sub, 0),
					figure_enable_legend(_fig);
					figure_set_title(_fig, buf_title);
					figure_series_set_dot_size_pixels(_s, 4);
				);
			}
		}
		free(window);
		free(subwindow);

	}

	if (do_print_features)
	{
		csr_value_differences_of_neighbors(title_base, vals, m, n, nnz, do_plot, num_pixels_x, num_pixels_y);

		// long deduplicate = 0;
		long deduplicate = 1;
		csr_value_differences_of_sorted_subsets(vals, nnz, deduplicate);

		// csr_value_distances_from_cluster_centers(title_base, vals, vals_sorted, m, n, nnz, do_plot, num_pixels_x, num_pixels_y);
	}

	free(vals);
	free(vals_sorted);
	free(vals_sorted_diff);
	free(vals_sorted_ratio_abs);
	free(vals_sorted_diff_fraction_abs);
	free(row_idx);
}


//==========================================================================================================================================
//= Ploting
//==========================================================================================================================================


#undef  csr_plot
#define csr_plot  CSR_UTIL_GEN_EXPAND(csr_plot)
void
csr_plot(char * title_base, _TYPE_I * row_ptr, _TYPE_I * col_idx, __attribute__((unused)) _TYPE_V * val, long m, long n, long nnz, int enable_legend, long num_pixels_x, long num_pixels_y)
{
	_TYPE_I * row_idx;
	long buf_n = strlen(title_base) + 1 + 1000;
	char buf[buf_n], buf_title[buf_n];

	csr_row_indices(row_ptr, col_idx, m, n, nnz, &row_idx);
	snprintf(buf, buf_n, "%s.png", title_base);
	snprintf(buf_title, buf_n, "%s", title_base);
	figure_simple_plot(buf, num_pixels_x, num_pixels_y, (col_idx, row_idx, NULL, nnz, 0),
		if (enable_legend)
			figure_enable_legend(_fig);
		figure_set_title(_fig, buf_title);
		figure_axes_flip_y(_fig);
		figure_set_bounds_x(_fig, 0, n-1);
		figure_set_bounds_y(_fig, 0, m-1);
		figure_series_type_density_map(_s);
	);
	free(row_idx);
}


#undef  csr_row_size_histogram_plot
#define csr_row_size_histogram_plot  CSR_UTIL_GEN_EXPAND(csr_row_size_histogram_plot)
void
csr_row_size_histogram_plot(char * title_base, _TYPE_I * row_ptr, __attribute__((unused)) _TYPE_I * col_idx, __attribute__((unused)) _TYPE_V * val, long m, __attribute__((unused)) long n, __attribute__((unused)) long nnz,
		int enable_legend, long num_pixels_x, long num_pixels_y)
{
	_TYPE_I * degrees_rows;
	long buf_n = strlen(title_base) + 1 + 1000;
	char buf[buf_n], buf_title[buf_n];

	degrees_rows = (typeof(degrees_rows)) malloc(m * sizeof(*degrees_rows));
	#pragma omp parallel for
	for(int i=0; i<m; i++)
		degrees_rows[i] = row_ptr[i+1] - row_ptr[i];

	/*	
	// Degree histogram. this can be commented out
	double nnz_per_row_min, nnz_per_row_max, nnz_per_row_avg, nnz_per_row_std;
	array_min_max(degrees_rows, m, &nnz_per_row_min, NULL, &nnz_per_row_max, NULL);
	printf("nnz_per_row_max = %lf\n", nnz_per_row_max);
	int *row_size_hist = (int*) calloc((int)nnz_per_row_max+1, sizeof(int));
	for(int i=0;i<m;i++)
		row_size_hist[degrees_rows[i]]++;
	for (int i = 0; i < (int)nnz_per_row_max+1; ++i)
		printf("%d %d\n", i, row_size_hist[i]);
	free(row_size_hist);
	*/
	
	snprintf(buf, buf_n, "%s_row_size_distribution.png", title_base);
	snprintf(buf_title, buf_n, "%s: row size distribution", title_base);
	figure_simple_plot(buf, num_pixels_x, num_pixels_y, (NULL, degrees_rows, NULL, m, 0),
		if (enable_legend)
			figure_enable_legend(_fig);
		figure_set_title(_fig, buf_title);
		figure_series_type_histogram(_s, 0, NULL, 1);
		figure_series_type_barplot(_s);
	);

	free(degrees_rows);
}

#undef  csr_cross_row_similarity_histogram_plot
#define csr_cross_row_similarity_histogram_plot  CSR_UTIL_GEN_EXPAND(csr_cross_row_similarity_histogram_plot)
void
csr_cross_row_similarity_histogram_plot(char * title_base, _TYPE_I * row_ptr, _TYPE_I * col_idx, __attribute__((unused)) _TYPE_V * val, long m, long n, long nnz, int window_size,
		int enable_legend, long num_pixels_x, long num_pixels_y)
{
	_TYPE_I *crs_row;
	long buf_n = strlen(title_base) + 1 + 1000;
	char buf[buf_n], buf_title[buf_n];

	// extract cross_row_similarity per row
	crs_row = (typeof(crs_row)) malloc(m * sizeof(*crs_row));
	csr_cross_row_neighbours(row_ptr, col_idx, m, n, nnz, window_size, crs_row);

	// cross_row_similarity (per row) histogram. this can be commented out
	double crs_row_min, crs_row_max;
	array_min_max(crs_row, m, &crs_row_min, NULL, &crs_row_max, NULL);
	// printf("crs_row_max = %lf\n", crs_row_max);
	int *crs_row_hist = (int*) calloc((int)crs_row_max+1, sizeof(int));
	for(int i=0;i<m;i++)
		crs_row_hist[crs_row[i]]++;
	// for (int i = 0; i < (int)crs_row_max+1; ++i)
	// 	printf("%d: %d (%.3f%)\n", i, crs_row_hist[i], crs_row_hist[i]*100.0/m);
	free(crs_row_hist);

	snprintf(buf, buf_n, "%s_crs_row_distribution.png", title_base);
	snprintf(buf_title, buf_n, "%s: crs_row distribution", title_base);
	figure_simple_plot(buf, num_pixels_x, num_pixels_y, (NULL, crs_row, NULL, m, 0),
		if (enable_legend)
			figure_enable_legend(_fig);
		figure_set_title(_fig, buf_title);
		figure_series_type_histogram(_s, 0, NULL, 1);
		figure_series_type_barplot(_s);
	);

	free(crs_row);
}


#undef  csr_num_neigh_histogram_plot
#define csr_num_neigh_histogram_plot  CSR_UTIL_GEN_EXPAND(csr_num_neigh_histogram_plot)
void
csr_num_neigh_histogram_plot(char * title_base, _TYPE_I * row_ptr, _TYPE_I * col_idx, __attribute__((unused)) _TYPE_V * val, long m, long n, long nnz, int window_size,
		int enable_legend, long num_pixels_x, long num_pixels_y)
{
	_TYPE_I *num_neigh;
	long buf_n = strlen(title_base) + 1 + 1000;
	char buf[buf_n], buf_title[buf_n];

	// extract num_neigh per row
	csr_row_neighbours(row_ptr, col_idx, m, n, nnz, window_size, &num_neigh);

	// num_neigh (per row) histogram. this can be commented out
	double num_neigh_min, num_neigh_max;
	array_min_max(num_neigh, m, &num_neigh_min, NULL, &num_neigh_max, NULL);
	// printf("num_neigh_max = %lf\n", num_neigh_max);
	int *num_neigh_hist = (int*) calloc((int)num_neigh_max+1, sizeof(int));
	for(int i=0;i<m;i++)
		num_neigh_hist[num_neigh[i]]++;
	// for (int i = 0; i < (int)num_neigh_max+1; ++i)
	// 	printf("%d: %d (%.3f%)\n", i, num_neigh_hist[i], num_neigh_hist[i]*100.0/m);
	free(num_neigh_hist);

	snprintf(buf, buf_n, "%s_num_neigh_distribution.png", title_base);
	snprintf(buf_title, buf_n, "%s: num_neigh distribution", title_base);
	figure_simple_plot(buf, num_pixels_x, num_pixels_y, (NULL, num_neigh, NULL, m, 0),
		if (enable_legend)
			figure_enable_legend(_fig);
		figure_set_title(_fig, buf_title);
		figure_series_type_histogram(_s, 0, NULL, 1);
		figure_series_type_barplot(_s);
	);

	free(num_neigh);
}


#undef  csr_bandwidth_histogram_plot
#define csr_bandwidth_histogram_plot  CSR_UTIL_GEN_EXPAND(csr_bandwidth_histogram_plot)
void
csr_bandwidth_histogram_plot(char * title_base, _TYPE_I * row_ptr, _TYPE_I * col_idx, __attribute__((unused)) _TYPE_V * val, long m, __attribute__((unused)) long n, __attribute__((unused)) long nnz,
		int enable_legend, long num_pixels_x, long num_pixels_y)
{
	_TYPE_I * bandwidth_rows;
	long buf_n = strlen(title_base) + 1 + 1000;
	char buf[buf_n], buf_title[buf_n];

	// TODO: need to fix this, so that it account for non_empty_rows only when calculating bw values.
	bandwidth_rows = (typeof(bandwidth_rows)) malloc(m * sizeof(*bandwidth_rows));
	#pragma omp parallel for
	for(int i=0; i<m; i++){
		if((row_ptr[i+1] - row_ptr[i]) == 0)
			bandwidth_rows[i] = 0;
		else
			bandwidth_rows[i] = col_idx[row_ptr[i+1]-1] - col_idx[row_ptr[i]];
	}

	// Degree histogram. this can be commented out
	double bandwidth_min, bandwidth_max, bandwidth_avg, bandwidth_std;
	array_min_max(bandwidth_rows, m, &bandwidth_min, NULL, &bandwidth_max, NULL);
	array_mean(bandwidth_rows, m, &bandwidth_avg);
	array_std(bandwidth_rows, m, &bandwidth_std);
	printf("bandwidth_max = %.0lf\n", bandwidth_max);
	printf("bandwidth_min = %.0lf\n", bandwidth_min);
	printf("bandwidth_avg = %.4lf\n", bandwidth_avg);
	printf("bandwidth_std = %.4lf\n", bandwidth_std);
	/*	
	int *bandwidth_hist = (int*) calloc((int)bandwidth_max+1, sizeof(int));

	for(int i=0;i<m;i++)
		bandwidth_hist[bandwidth_rows[i]]++;
	for (int i = 0; i < (int)bandwidth_max+1; ++i){
		printf("%d %d", i, bandwidth_hist[i]);
		if(bandwidth_hist[i]*100.0/m > 0.5)
			printf(" ( %.4f )", bandwidth_hist[i]*100.0/m);
		printf("\n");
	}
	free(bandwidth_hist);
	*/	
	snprintf(buf, buf_n, "%s_bandwidth_distribution.png", title_base);
	snprintf(buf_title, buf_n, "%s: bandwidth distribution", title_base);
	figure_simple_plot(buf, num_pixels_x, num_pixels_y, (NULL, bandwidth_rows, NULL, m, 0),
		if (enable_legend)
			figure_enable_legend(_fig);
		figure_set_title(_fig, buf_title);
		figure_series_type_histogram(_s, 0, NULL, 1);
		figure_series_type_barplot(_s);
	);

	free(bandwidth_rows);
}


#undef  csr_bandwidth_batch_nnz_bar_plot
#define csr_bandwidth_batch_nnz_bar_plot  CSR_UTIL_GEN_EXPAND(csr_bandwidth_batch_nnz_bar_plot)
void
csr_bandwidth_batch_nnz_bar_plot(char * title_base, __attribute__((unused)) _TYPE_I * row_ptr, _TYPE_I * col_idx, __attribute__((unused)) _TYPE_V * val, __attribute__((unused)) long m, long n, __attribute__((unused)) long nnz, int batch_nnz, 
		int enable_legend, long num_pixels_x, long num_pixels_y)
{
	_TYPE_I * row_idx;
	csr_row_indices(row_ptr, col_idx, m, n, nnz, &row_idx);

	float * x_part_size, * y_part_size, * x_unique_values;
	int * x_part_s, * x_part_f;
	long buf_n = strlen(title_base) + 1 + 1000;
	char buf[buf_n], buf_title[buf_n];

	_TYPE_I nnz_b = (nnz+batch_nnz-1)/batch_nnz;
	x_part_size = (typeof(x_part_size)) malloc(nnz_b * sizeof(*x_part_size));
	// y_part_size = (typeof(y_part_size)) malloc(nnz_b * sizeof(*y_part_size));
	
	x_unique_values = (typeof(x_unique_values)) calloc(nnz_b, sizeof(*x_unique_values));
	x_part_s = (typeof(x_part_s)) malloc(nnz_b * sizeof(*x_part_s));
	x_part_f = (typeof(x_part_f)) malloc(nnz_b * sizeof(*x_part_f));
	
	// cuCSR processes nonzeros in wavefronts of 432 thread blocks -> 108 SMs -> 4 thread blocks per SM
	// Each SM is assigned with 4 thread blocks for load_balancing_kernel
	// Each thread block consists of 512 threads, and each thread is assigned with 4 nonzeros
	// Therefore, 4*512*4 = 8192 nonzeros per SM
	// For each window of 8192 nonzeros, I have to see which part of x vector is requested, and try to minimize it as much as possible
	// so that it fits in the L1 cache
	double time_it1 = time_it(1,
	_Pragma("omp parallel")
	{
		_Pragma("omp for")
		for(int i=0; i<nnz_b; i++){
			int min_col_idx = n+1;
			int max_col_idx = -1;
			int min_row_idx = m+1;
			int max_row_idx = -1;
			for(int ii=i*batch_nnz; ii<(i+1)*batch_nnz && ii<nnz; ii++){
				if(col_idx[ii] < min_col_idx)
					min_col_idx = col_idx[ii];
				if(col_idx[ii] > max_col_idx)
					max_col_idx = col_idx[ii];
				if(row_idx[ii] < min_row_idx)
					min_row_idx = row_idx[ii];
				if(row_idx[ii] > max_row_idx)
					max_row_idx = row_idx[ii];
			}
			int col_width = max_col_idx - min_col_idx + 1;
			x_part_size[i] = col_width * 8.0 / 1024;
			// int row_width = max_row_idx - min_row_idx + 1;
			// y_part_size[i] = row_width * 8.0 / 1024;

			x_part_s[i] = min_col_idx;
			x_part_f[i] = max_col_idx;
			// printf("%d/%d\tmin_row_idx = %d, max_row_idx = %d ( %d rows )\t/ min_col_idx = %d, max_col_idx = %d ( width = %d )\t-> %.2lf KB of x vector\n", i+1, nnz_b, min_row_idx, max_row_idx, row_width, min_col_idx, max_col_idx, col_width, x_part_size[i]);
		}
	}
	);

	double time_it2 = time_it(1,
	_Pragma("omp parallel")
	{
		_Pragma("omp for")
		for(int i=0; i<nnz_b; i++){
			int * x_hash_table;
			x_hash_table = (typeof(x_hash_table)) calloc(x_part_f[i] - x_part_s[i] + 1, sizeof(*x_hash_table));

			for(int ii=i*batch_nnz; ii<(i+1)*batch_nnz && ii<nnz; ii++)
				x_hash_table[col_idx[ii]-x_part_s[i]]+=1;

			// now count how many values of x_hash_table are larger than zero
			for(int ii=0; ii<x_part_f[i]-x_part_s[i]+1; ii++)
				if(x_hash_table[ii] > 0)
					x_unique_values[i]+=1;
			free(x_hash_table);
		}
	}
	);
	printf("time_it1 = %.4lf, time_it2 = %.4lf\n", time_it1, time_it2);

	for(int i=0;i<50;i++)
		printf("nnz block %d: x_part_size = %.0lf width, x_unique_values = %.0lf (%.4lf%)\n", i, x_part_size[i]*1024/8, x_unique_values[i], x_unique_values[i]*100.0/(x_part_f[i] - x_part_s[i] + 1));

	// Degree histogram. this can be commented out
	double x_part_size_min, x_part_size_max, x_part_size_avg, x_part_size_std;
	array_min_max(x_part_size, nnz_b, &x_part_size_min, NULL, &x_part_size_max, NULL);
	array_mean(x_part_size, nnz_b, &x_part_size_avg);
	array_std(x_part_size, nnz_b, &x_part_size_std);
	printf("%d batches of %d nonzeros\n", nnz_b, batch_nnz);
	printf("x_part_size_max = %.4lf KB\n", x_part_size_max);
	printf("x_part_size_min = %.4lf KB\n", x_part_size_min);
	printf("x_part_size_avg = %.4lf KB (while x vector = %.4lf -> %.2lf\%)\n", x_part_size_avg, n * 8.0 / 1024, x_part_size_avg * 100.0 / (n * 8.0 / 1024));
	printf("x_part_size_std = %.4lf\n", x_part_size_std);

	double x_unique_values_min, x_unique_values_max, x_unique_values_avg, x_unique_values_std;
	array_min_max(x_unique_values, nnz_b, &x_unique_values_min, NULL, &x_unique_values_max, NULL);
	array_mean(x_unique_values, nnz_b, &x_unique_values_avg);
	array_std(x_unique_values, nnz_b, &x_unique_values_std);
	printf("\n");
	printf("x_unique_values_max = %.4lf\n", x_unique_values_max);
	printf("x_unique_values_min = %.4lf\n", x_unique_values_min);
	printf("x_unique_values_avg = %.4lf (while x part width avg = %.4lf -> %.2lf\%)\t(%.2lf\% out of 8192 (max 100%) - different for each nonzero)\n", x_unique_values_avg, x_part_size_avg*1024/8, x_unique_values_avg * 100.0 / (x_part_size_avg*1024/8), x_unique_values_avg * 100.0 / batch_nnz);
	printf("x_unique_values_std = %.4lf\n", x_unique_values_std);

	// Degree histogram. this can be commented out
	// double y_part_size_min, y_part_size_max, y_part_size_avg, y_part_size_std;
	// array_min_max(y_part_size, nnz_b, &y_part_size_min, NULL, &y_part_size_max, NULL);
	// array_mean(y_part_size, nnz_b, &y_part_size_avg);
	// array_std(y_part_size, nnz_b, &y_part_size_std);
	// printf("\n");
	// printf("y_part_size_max = %.4lf KB\n", y_part_size_max);
	// printf("y_part_size_min = %.4lf KB\n", y_part_size_min);
	// printf("y_part_size_avg = %.4lf KB\n", y_part_size_avg);
	// printf("y_part_size_std = %.4lf\n", y_part_size_std);

	/*
	int *bandwidth_hist = (int*) calloc((int)x_part_size_max+1, sizeof(int));
	for(int i=0;i<nnz_b;i++)
		bandwidth_hist[x_part_size[i]]++;
	for (int i = 0; i < (int)x_part_size_max+1; ++i){
		printf("%d %d", i, bandwidth_hist[i]);
		if(bandwidth_hist[i]*100.0/nnz_b > 0.5)
			printf(" ( %.4f )", bandwidth_hist[i]*100.0/nnz_b);
		printf("\n");
	}
	free(bandwidth_hist);
	*/

	// snprintf(buf, buf_n, "%s_x_part_size_distribution.png", title_base);
	// snprintf(buf_title, buf_n, "%s: bandwidth batch_nnz distribution", title_base);
	// figure_simple_plot(buf, num_pixels_x, num_pixels_y, (NULL, x_part_size, NULL, nnz_b, 0),
	// 	if (enable_legend)
	// 		figure_enable_legend(_fig);
	// 	figure_set_title(_fig, buf_title);
	// 	// figure_series_type_histogram(_s, 0, NULL, 1);
	// 	figure_series_type_barplot(_s);
	// );

	// snprintf(buf, buf_n, "%s_y_part_size_distribution.png", title_base);
	// snprintf(buf_title, buf_n, "%s: bandwidth batch_nnz distribution", title_base);
	// figure_simple_plot(buf, num_pixels_x, num_pixels_y, (NULL, y_part_size, NULL, nnz_b, 0),
	// 	if (enable_legend)
	// 		figure_enable_legend(_fig);
	// 	figure_set_title(_fig, buf_title);
	// 	// figure_series_type_histogram(_s, 0, NULL, 1);
	// 	figure_series_type_barplot(_s);
	// );

	free(x_part_size);
	// free(y_part_size);
	free(x_part_s);
	free(x_part_f);
	free(x_unique_values);
	free(row_idx);
}




//==========================================================================================================================================
//= Quantize
//==========================================================================================================================================


/* We expect sorted columns in each row.
 */
#undef  csr_quantize_columns
#define csr_quantize_columns  CSR_UTIL_GEN_EXPAND(csr_quantize_columns)
void
csr_quantize_columns(_TYPE_I * row_ptr, _TYPE_I * col_idx, long m, [[gnu::unused]] long n, long nnz, long window_len,
		_TYPE_I ** q_row_ptr_out, _TYPE_I ** q_col_idx_out, int ** q_frequencies_out, int * q_nnz_out)
{
	int num_threads = omp_get_max_threads();
	long q_nnz;

	int * q_row_ptr = (typeof(q_row_ptr)) malloc((m+1) * sizeof(*q_row_ptr));

	printf("1\n");
	_Pragma("omp parallel")
	{
		int tnum = omp_get_thread_num();
		long win, win_prev;
		long i, i_s, i_e, j;
		loop_partitioner_balance_prefix_sums(num_threads, tnum, row_ptr, m, nnz, &i_s, &i_e);
		for (i=i_s;i<i_e;i++)
		{
			win_prev = -1;
			q_row_ptr[i] = 0;
			for (j=row_ptr[i];j<row_ptr[i+1];j++)
			{
				win = col_idx[j] / window_len;
				if (win != win_prev)
				{
					q_row_ptr[i]++;
					win_prev = win;
				}
			}
		}
	}
	q_row_ptr[m] = 0;

	printf("2\n");
	q_nnz = scan_reduce(q_row_ptr, q_row_ptr, m+1, 0, 1, 0);

	int * q_col_idx = (typeof(q_col_idx)) malloc(q_nnz * sizeof(*q_col_idx));
	int * q_frequencies = (typeof(q_frequencies)) malloc(q_nnz * sizeof(*q_frequencies));

	printf("3\n");
	_Pragma("omp parallel")
	{
		int tnum = omp_get_thread_num();
		long win, win_prev;
		long i, i_s, i_e, j, w_j;
		loop_partitioner_balance_prefix_sums(num_threads, tnum, row_ptr, m, nnz, &i_s, &i_e);
		printf("3a\n");
		for (i=i_s;i<i_e;i++)
		{
			win_prev = -1;
			w_j = q_row_ptr[i] - 1;
			for (j=row_ptr[i];j<row_ptr[i+1];j++)
			{
				win = col_idx[j] / window_len;
				if (win != win_prev)
				{
					w_j++;
					q_col_idx[w_j] = win;
					q_frequencies[w_j] = 0;
					win_prev = win;
				}
				q_frequencies[w_j]++;
			}
		}
	}
	printf("4\n");

	*q_row_ptr_out = q_row_ptr;
	*q_col_idx_out = q_col_idx;
	*q_frequencies_out = q_frequencies;

	*q_nnz_out = q_nnz;
}


//==========================================================================================================================================
//= Includes Undefs
//==========================================================================================================================================


#include "sort/samplesort/samplesort_gen_pop.h"
#include "sort/quicksort/quicksort_gen_pop.h"
#include "functools/functools_gen_pop.h"

