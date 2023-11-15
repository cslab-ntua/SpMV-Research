#include <stdlib.h>
#include <stdio.h>

#include "macros/macrolib.h"
#include "debug.h"
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
}


/* #undef  csr_value_
#define csr_value_  CSR_UTIL_GEN_EXPAND(csr_value_)
static
void
csr_value_absolute_values(char * title_base, double * vals, long m, long n, long nnz, int do_plot, long num_pixels_x, long num_pixels_y)
{
	long buf_n = strlen(title_base) + 1 + 1000;
	char buf[buf_n], buf_title[buf_n];
	long num_bins = 1024;

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
} */


// Sorting.
#undef  csr_value_differences_of_sorted_subsets
#define csr_value_differences_of_sorted_subsets  CSR_UTIL_GEN_EXPAND(csr_value_differences_of_sorted_subsets)
static inline
void
csr_value_differences_of_sorted_subsets(double * vals, long nnz)
{
	double * vals_diff_sorted_window;
	double vals_diff_sorted_window_abs_min, vals_diff_sorted_window_abs_max, vals_diff_sorted_window_abs_avg, vals_diff_sorted_window_abs_std, vals_diff_sorted_window_ctz_avg, vals_diff_sorted_window_ctz_std;
	// double vals_diff_sorted_window_grouped_ctz_avg;
	long vals_diff_sorted_window_unique_num, vals_diff_sorted_window_exp_unique_num;
	double ctz_corrected, ctz_corrected_best;
	long best_k, best_ctz_group_size;
	long window_size;
	long k;
	vals_diff_sorted_window = (typeof(vals_diff_sorted_window)) malloc(nnz * sizeof(*vals_diff_sorted_window));
	ctz_corrected_best = 0;
	best_k = 0;
	best_ctz_group_size = 1;
	// k = 21;
	for (k=4;k<=24;k++)
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
	__attribute__((unused)) long i, j, k, l;

	long do_print_features = 0;
	// long do_print_features = 1;

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

		// snprintf(buf, buf_n, "%s_values_heatmap.png", title_base);
		// snprintf(buf_title, buf_n, "%s: values heatmap", title_base);
		// figure_simple_plot(buf, num_pixels_x, num_pixels_y, (col_idx, row_idx, vals, nnz, 0),
			// figure_enable_legend(_fig);
			// figure_set_title(_fig, buf_title);
			// figure_axes_flip_y(_fig);
			// figure_set_bounds_x(_fig, 0, n);
			// figure_set_bounds_y(_fig, 0, m);
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
			// figure_series_type_histogram(_s, num_bins, 1);
			// figure_series_type_barplot(_s);
		// );
		// snprintf(buf, buf_n, "%s_values_exp_distribution.png", title_base);
		// snprintf(buf_title, buf_n, "%s: values exponent distribution", title_base);
		// figure_simple_plot(buf, num_pixels_x, num_pixels_y, (NULL, vals, NULL, nnz, 0, , get_double_exponent),
			// figure_enable_legend(_fig);
			// figure_set_title(_fig, buf_title);
			// figure_series_type_histogram(_s, 0, 1); // Integer mode.
			// figure_series_type_barplot(_s);
		// );
		// snprintf(buf, buf_n, "%s_values_frac_distribution.png", title_base);
		// snprintf(buf_title, buf_n, "%s: values fraction distribution", title_base);
		// figure_simple_plot(buf, num_pixels_x, num_pixels_y, (NULL, vals, NULL, nnz, 0, , get_double_fraction),
			// figure_enable_legend(_fig);
			// figure_set_title(_fig, buf_title);
			// figure_series_type_histogram(_s, num_bins, 1);
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

		k = 14;
		long window_size = 1ULL << k;
		double * window = (typeof(window)) malloc(window_size * sizeof(*window));
		long size, gap;
		gap = nnz / 10;
		for (l=0;l<10;l++)
		{
			double val, val_prev;
			i = l * gap;
			size = (window_size < nnz - i) ?  window_size : nnz - i;
			window[0] = 0;
			for (j=1;j<size;j++)
			{
				val = vals[i+j];
				val_prev = vals[i+j-1];
				if (val == 0 || val_prev == 0)
					window[j] = 0;
				else
				{
					window[j] = fabs(val / val_prev);
				}
			}
			quicksort(window, size, NULL);
			snprintf(buf, buf_n, "%s_values_sorted_ratio_abs_log2_window_%ld.png", title_base, l);
			snprintf(buf_title, buf_n, "%s: values sorted: ratio, absolute, log2 - window [%ld, %ld)", title_base, i, i+size);
			figure_simple_plot(buf, num_pixels_x, num_pixels_y, (NULL, window, NULL, size, 0, , get_double_abs_log2),
				figure_enable_legend(_fig);
				figure_set_title(_fig, buf_title);
			);
		}
		free(window);


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
		// long window_size = 1ULL << k;
		// double * window = (typeof(window)) malloc(window_size * sizeof(*window));
		// long size, gap;
		// gap = nnz / 10;
		// for (l=0;l<10;l++)
		// {
			// double val, val_prev, diff;
			// i = l * gap;
			// size = (window_size < nnz - i) ?  window_size : nnz - i;
			// window[0] = 0;
			// for (j=1;j<size;j++)
			// {
				// val = vals[i+j];
				// val_prev = vals[i+j-1];
				// diff = val - val_prev;
				// if (diff == 0 || val_prev == 0)
					// window[j] = 0;
				// else
				// {
					// window[j] = fabs(diff / val_prev);
				// }
			// }
			// quicksort(window, size, NULL);
			// snprintf(buf, buf_n, "%s_values_sorted_diff_fraction_abs_log2_window_%ld.png", title_base, l);
			// snprintf(buf_title, buf_n, "%s: values sorted: diff, fraction, absolute, log2 - window [%ld, %ld)", title_base, i, i+size);
			// figure_simple_plot(buf, num_pixels_x, num_pixels_y, (NULL, window, NULL, size, 0, , get_double_abs_log2),
				// figure_enable_legend(_fig);
				// figure_set_title(_fig, buf_title);
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
			quicksort(window, size, NULL);
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

	}

	// csr_value_differences_of_neighbors(title_base, vals, m, n, nnz, do_plot, num_pixels_x, num_pixels_y);
	// csr_value_differences_of_sorted_subsets(vals, nnz);

	// csr_value_distances_from_cluster_centers(title_base, vals, vals_sorted, m, n, nnz, do_plot, num_pixels_x, num_pixels_y);

	free(vals);
	free(vals_sorted);
	free(vals_sorted_diff);
	free(vals_sorted_ratio_abs);
	free(vals_sorted_diff_fraction_abs);
	free(row_idx);
}


//==========================================================================================================================================
//= Matrix transformations
//==========================================================================================================================================


typedef struct {
	int row_sizes;
	int original_position;
} RowInfo;

static inline
int compare(const void *a, const void *b) {
	return ((RowInfo *)b)->row_sizes - ((RowInfo *)a)->row_sizes;
}

static
void sort_rows(_TYPE_I *row_ptr, int m, _TYPE_I *sorted_row_sizes, _TYPE_I *original_positions)
{
	RowInfo *rows = (RowInfo *)malloc(m * sizeof(RowInfo));

	// Extract row sizes and original positions
	// #pragma omp parallel for
	for (int i = 0; i < m; i++) {
		rows[i].row_sizes = row_ptr[i + 1] - row_ptr[i];
		rows[i].original_position = i;
	}

	// Sort rows by descending order of row sizes
	qsort(rows, m, sizeof(RowInfo), compare);

	// Store sorted row sizes and original positions
	// #pragma omp parallel for
	for (int i = 0; i < m; i++) {
		sorted_row_sizes[i] = rows[i].row_sizes;
		original_positions[i] = rows[i].original_position;
	}

	free(rows);
}

/*
static
void sort_rows(_TYPE_I *row_ptr, int m, _TYPE_I *sorted_row_sizes, _TYPE_I *original_positions) {
	// Extract row sizes and original positions
	for (int i = 0; i < m; i++) {
		sorted_row_sizes[i] = row_ptr[i + 1] - row_ptr[i];
		original_positions[i] = i;
	}

	// Bubble sort rows by descending order of row sizes
	for (int i = 0; i < m - 1; i++) {
		for (int j = 0; j < m - i - 1; j++) {
			if (sorted_row_sizes[j] < sorted_row_sizes[j + 1]) {
				// Swap sorted_row_sizes
				int temp = sorted_row_sizes[j];
				sorted_row_sizes[j] = sorted_row_sizes[j + 1];
				sorted_row_sizes[j + 1] = temp;

				// Swap original_positions
				temp = original_positions[j];
				original_positions[j] = original_positions[j + 1];
				original_positions[j + 1] = temp;
			}
		}
	}
}
*/

#undef  csr_sort_by_row_size
#define csr_sort_by_row_size  CSR_UTIL_GEN_EXPAND(csr_sort_by_row_size)
void
csr_sort_by_row_size(long m, _TYPE_I * row_ptr, _TYPE_I * col_idx, _TYPE_V * values, _TYPE_I * row_ptr_s, _TYPE_I * col_idx_s, _TYPE_V * values_s)
{
	_TYPE_I *sorted_row_sizes = (_TYPE_I *)malloc(m * sizeof(_TYPE_I));
	_TYPE_I *original_positions = (_TYPE_I *)malloc(m * sizeof(_TYPE_I));

	sort_rows(row_ptr, m, sorted_row_sizes, original_positions);

	// printf("Sorted Row Sizes = [ ");
	// for (int i = 0; i < m; i++) printf("%d ", sorted_row_sizes[i]);
	// printf("]\n");

	// printf("\nOriginal Positions = [ ");
	// for (int i = 0; i < m; i++) printf("%d ", original_positions[i]);
	// printf("]\n");

	// Create a temporary array to keep track of row_ptr_s updates
	_TYPE_I *row_ptr_temp = (_TYPE_I *)malloc((m + 1) * sizeof(_TYPE_I));
	row_ptr_temp[0] = 0;
	row_ptr_s[0] = 0;

	// Update row_ptr_s based on sorted row sizes
	for (int i = 0; i < m; i++) {
		row_ptr_s[i + 1] = row_ptr_s[i] + sorted_row_sizes[i];
		row_ptr_temp[i + 1] = row_ptr_s[i + 1];
	}

	// Rearrange col_idx and val arrays based on original positions
	for (int i = 0; i < m; i++) {
		int orig_pos = original_positions[i];
		for (int j = row_ptr[orig_pos]; j < row_ptr[orig_pos + 1]; j++) {
			col_idx_s[row_ptr_temp[i]] = col_idx[j];
			values_s[row_ptr_temp[i]] = values[j];
			row_ptr_temp[i]++;
		}
	}

	free(row_ptr_temp);
	free(sorted_row_sizes);
	free(original_positions);
}

// "distant_mark" is a 0-1 flag matrix, which holds which nonzeros are distant from others, in order to isolate them later
#undef  csr_mark_distant_nonzeros
#define csr_mark_distant_nonzeros  CSR_UTIL_GEN_EXPAND(csr_mark_distant_nonzeros)
long csr_mark_distant_nonzeros(_TYPE_I * row_ptr, _TYPE_I * col_idx, long m, long max_distance, _TYPE_I * distant_mark)
{
	#pragma omp parallel
	{
		long i = 0, j = 0, k = 0;
		#pragma omp for
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
					if(distance > max_distance){
						distant_mark[j] = 1;
						break;
					}
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
						if(distance > max_distance){
							distant_mark[j] = 1;
							break;
						}
					}
				}

				if(flag)
					row_distance++;
				else
					distant_mark[j] = 0;
			}
		}
	}
	long count=0;
	#pragma omp parallel
	{
		long i = 0;
		#pragma omp for reduction(+:count)
		for(i=0; i<row_ptr[m]; i++)
			count+=distant_mark[i];
	}
	return count;
}

#undef  csr_separate_close_distant
#define csr_separate_close_distant  CSR_UTIL_GEN_EXPAND(csr_separate_close_distant)
void
csr_separate_close_distant(_TYPE_I *row_ptr, _TYPE_I *col_idx, _TYPE_V *values, _TYPE_I *distant_mark, __attribute__((unused)) long nnz, long m, 
					   _TYPE_I *row_ptr_close, _TYPE_I *col_idx_close, _TYPE_V *values_close, _TYPE_I *row_ptr_distant, _TYPE_I *col_idx_distant, _TYPE_V *values_distant)
{
	// Initialize row_ptr for "close" and "distant"
	row_ptr_close[0] = 0;
	row_ptr_distant[0] = 0;

	// Distribute elements based on distant_mark
	int close_idx = 0;
	int distant_idx = 0;

	for (int i = 0; i < m; i++) {
		for (int j = row_ptr[i]; j < row_ptr[i + 1]; j++) {
			if (distant_mark[j] == 0) {
				// "close" element
				col_idx_close[close_idx] = col_idx[j];
				values_close[close_idx] = values[j];
				close_idx++;
			} else {
				// "distant" element
				col_idx_distant[distant_idx] = col_idx[j];
				values_distant[distant_idx] = values[j];
				distant_idx++;
			}
		}

		// Update row_ptr for "close" and "distant"
		row_ptr_close[i + 1] = close_idx;
		row_ptr_distant[i + 1] = distant_idx;
	}
}

#undef  csr_shuffle_matrix
#define csr_shuffle_matrix  CSR_UTIL_GEN_EXPAND(csr_shuffle_matrix)
void
csr_shuffle_matrix(long m, _TYPE_I *row_ptr, _TYPE_I *col_idx, _TYPE_V *values, _TYPE_I *row_ptr_shuffle, _TYPE_I *col_idx_shuffle, _TYPE_V *values_shuffle)
{
	_TYPE_I i;
	// Create an array of indices representing row order
	_TYPE_I* row_indices = (_TYPE_I*)malloc(sizeof(_TYPE_I) * m);
	
	// parallelize as much as possible (not much needed here though...)
	#pragma omp parallel for
	for (i = 0; i < m; i++)
		row_indices[i] = i;

	// Shuffle the row indices using Fisher-Yates shuffle algorithm
	srand(time(NULL));
	for (i = m - 1; i > 0; i--) {
		_TYPE_I j = rand() % (i + 1);
		_TYPE_I temp = row_indices[i];
		row_indices[i] = row_indices[j];
		row_indices[j] = temp;
	}

	// _TYPE_I start_idx = 0;
	row_ptr_shuffle[0] = 0;

	// cannot parallelize this one, (read from row_ptr_shuffle[new_row], write to row_ptr_shuffle[new_row+1])
	for(i = 0; i < m; i++) {
		_TYPE_I old_row = row_indices[i];
		_TYPE_I new_row = i;
		_TYPE_I row_size = row_ptr[old_row + 1] - row_ptr[old_row];
		_TYPE_I start_idx = row_ptr_shuffle[new_row];
		row_ptr_shuffle[new_row + 1] = start_idx + row_size;
	}

	// that's why i did it in separate loops, can utilize parallelization here!
	// Increased parallelization opportunity here (with the loop partitioner of dgal!) Perhaps I will do it one day...
	#pragma omp parallel for
	for(i = 0; i < m; i++) {
		_TYPE_I old_row = row_indices[i];
		_TYPE_I new_row = i;
		_TYPE_I row_size = row_ptr[old_row + 1] - row_ptr[old_row];
		_TYPE_I start_old_idx = row_ptr[old_row];
		_TYPE_I start_idx     = row_ptr_shuffle[new_row];
		for(_TYPE_I j = 0; j < row_size; j++) {
			_TYPE_I old_idx = start_old_idx + j;
			_TYPE_I new_idx = start_idx + j;
			col_idx_shuffle[new_idx] = col_idx[old_idx];
			values_shuffle[new_idx] = values[old_idx];
		}
		// row_ptr_shuffle[new_row + 1] = start_idx + row_size;
	}
	free(row_indices);
}

/*
#undef  csr_extract_row_cross
#define csr_extract_row_cross  CSR_UTIL_GEN_EXPAND(csr_extract_row_cross)
void
csr_extract_row_cross(_TYPE_I *row_ptr, _TYPE_I *col_idx, __attribute__((unused)) _TYPE_V *val, int m, int n, int nnz, int window_width, 
					  int *num_windows_out, float **row_cross_out, int plot, _TYPE_I **rc_r_out, _TYPE_I **rc_c_out, float **rc_v_out)
{
	int num_windows = (n-1 + window_width) / window_width;
	printf("m = %d, n = %d, num_windows = %d\n", m, n, num_windows);
	long unsigned rc_elements = m * num_windows;
	float * row_cross = (typeof(row_cross)) calloc(rc_elements, sizeof(*row_cross));
	double row_cross_mem_foot = (m * 1.0 * num_windows * sizeof(*row_cross))/(1024*1024*1.0);
	printf("memory footprint of row_cross = %.2lf MB\n", row_cross_mem_foot);

	int num_threads = omp_get_max_threads();

	_TYPE_I * thread_i_s = (_TYPE_I *) malloc(num_threads * sizeof(*thread_i_s));
	_TYPE_I * thread_i_e = (_TYPE_I *) malloc(num_threads * sizeof(*thread_i_e));

	double time_row_cross = time_it(1,
	_Pragma("omp parallel")
	{
		int tnum = omp_get_thread_num();
		loop_partitioner_balance_prefix_sums(num_threads, tnum, row_ptr, m, nnz, &thread_i_s[tnum], &thread_i_e[tnum]);
		_Pragma("omp barrier")

		// _Pragma("omp single")
		// {
		// 	for(int tnum = 0; tnum < num_threads; tnum++)
		// 		printf("tnum = %2d\trows = (%d-%d) %d\t\tnonzeros = %d\n", tnum, thread_i_s[tnum], thread_i_e[tnum], thread_i_e[tnum] - thread_i_s[tnum], row_ptr[thread_i_e[tnum]] - row_ptr[thread_i_s[tnum]]);
		// }

		for(_TYPE_I i=thread_i_s[tnum]; i<thread_i_e[tnum]; i++){
			// printf("tnum=%d\ti=%d\n", tnum, i);
			for(_TYPE_I j=row_ptr[i]; j<row_ptr[i+1]; j++){
				_TYPE_I cw_loc = col_idx[j] / window_width; // col window location
				long unsigned rc_ind = i * num_windows + cw_loc;
				// printf("tnum = %d, i * num_windows + cw_loc = %lu\n", tnum, i * num_windows + cw_loc);
				row_cross[rc_ind]+=1.0;
			}
		}
	}
	);
	printf("time for row_cross = %lf\n", time_row_cross);

	if(plot){
		long unsigned rc_elements_nz = 0;
		double time_reduction = time_it(1,
		_Pragma("omp parallel")
		{
		_Pragma("omp for reduction(+:rc_elements_nz)")
		for(int i=0; i<m; i++){
			for(int j=0; j<num_windows; j++){
				long unsigned rc_ind = i * num_windows + j;
				if(row_cross[rc_ind]!=0) 
					rc_elements_nz++;
			}
		}
		}
		);
		printf("time for row_cross_reduction = %lf\n", time_reduction);
		printf("rc_elements_nz = %lu\n", rc_elements_nz);

		_TYPE_I * rc_r, * rc_c;
		float * rc_v;
		// rc_r = (typeof(rc_r)) malloc(rc_elements_nz * sizeof(*rc_r));
		rc_r = (typeof(rc_r)) malloc((m+1) * sizeof(*rc_r));
		rc_c = (typeof(rc_c)) malloc(rc_elements_nz * sizeof(*rc_c));
		rc_v = (typeof(rc_v)) malloc(rc_elements_nz * sizeof(*rc_v));
		double row_cross_compr_mem_foot = ((m+1) * sizeof(*rc_r) + rc_elements_nz * sizeof(*rc_c) + rc_elements_nz * sizeof(*rc_v))/(1024*1024*1.0);
		printf("memory footprint of row_cross (compressed) = %.2lf MB\n", row_cross_compr_mem_foot);
		// printf("compression ratio = %.2lf%\n", row_cross_compr_mem_foot/row_cross_mem_foot * 100);

		// in order to make it faster, perhaps calculate rc_r beforehand and use the dgal partitioner for rc_c, rc_v
		int cnt=0;
		rc_r[0] = 0;
		double time_final = time_it(1,
		for(int i=0; i<m; i++){
			for(int j=0; j<num_windows; j++){
				long unsigned rc_ind = i * num_windows + j;
				if(row_cross[rc_ind]!=0){
					// rc_r[cnt] = i;
					rc_c[cnt] = j;
					rc_v[cnt] = row_cross[rc_ind];
					cnt++;
				}
				rc_r[i+1] = cnt;
			}
		}
		);
		printf("time_final = %lf\n", time_final);
		if (rc_r_out != NULL)
			*rc_r_out = rc_r;
		if (rc_c_out != NULL)
			*rc_c_out = rc_c;
		if (rc_v_out != NULL)
			*rc_v_out = rc_v;
	}

	free(thread_i_s);
	free(thread_i_e);

	*num_windows_out = num_windows;
	if (row_cross_out != NULL)
		*row_cross_out = row_cross;
}
*/

#undef  csr_extract_row_cross2
#define csr_extract_row_cross2  CSR_UTIL_GEN_EXPAND(csr_extract_row_cross2)
void
csr_extract_row_cross2(_TYPE_I *row_ptr, _TYPE_I *col_idx, __attribute__((unused)) _TYPE_V *val, int m, int n, int nnz, int window_width, 
					   int *num_windows_out, _TYPE_I **rc_r_out, _TYPE_I **rc_c_out, float **rc_v_out)
{
	int num_windows = (n-1 + window_width) / window_width;
	printf("m = %d, n = %d, num_windows = %d\n", m, n, num_windows);
	int num_threads = omp_get_max_threads();

	_TYPE_I * thread_i_s = (_TYPE_I *) malloc(num_threads * sizeof(*thread_i_s));
	_TYPE_I * thread_i_e = (_TYPE_I *) malloc(num_threads * sizeof(*thread_i_e));
	long unsigned  * thread_rc_elements_nz = (long unsigned  *) malloc(num_threads * sizeof(*thread_rc_elements_nz));

	double time_row_cross = time_it(1,
	_Pragma("omp parallel")
	{
		int tnum = omp_get_thread_num();
		loop_partitioner_balance_prefix_sums(num_threads, tnum, row_ptr, m, nnz, &thread_i_s[tnum], &thread_i_e[tnum]);
		_Pragma("omp barrier")
		thread_rc_elements_nz[tnum] = 0;
		for(_TYPE_I i=thread_i_s[tnum]; i<thread_i_e[tnum]; i++){
			long unsigned * local_row_cross = (typeof(local_row_cross)) calloc(num_windows, sizeof(*local_row_cross));
			for(_TYPE_I j=row_ptr[i]; j<row_ptr[i+1]; j++){
				_TYPE_I cw_loc = col_idx[j] / window_width; // col window location
				local_row_cross[cw_loc] = 1; // if already found in cw_loc, just overwrite it, no problem with that... we just want to identify how many windows for sparse struct later
			}
			for(int k=0;k<num_windows; k++)
				thread_rc_elements_nz[tnum] += local_row_cross[k];
			free(local_row_cross);
		}
	}
	);
	printf("time for row_cross = %lf\n", time_row_cross);
	long unsigned rc_elements_nz = 0;
	_Pragma("omp parallel")
	{
		_Pragma("omp for reduction(+:rc_elements_nz)")
		for(int k=0;k<num_threads;k++){
			// printf("thread_rc_elements_nz[%d] = %lu\n", k, thread_rc_elements_nz[k]);
			rc_elements_nz += thread_rc_elements_nz[k];
		}
	}
	printf("rc_elements_nz = %lu\n", rc_elements_nz);

	_TYPE_I * rc_r, * rc_c;
	float * rc_v;
	// rc_r = (typeof(rc_r)) malloc(rc_elements_nz * sizeof(*rc_r));
	rc_r = (typeof(rc_r)) malloc((m+1) * sizeof(*rc_r));
	rc_c = (typeof(rc_c)) malloc(rc_elements_nz * sizeof(*rc_c));
	rc_v = (typeof(rc_v)) malloc(rc_elements_nz * sizeof(*rc_v));
	double row_cross_compr_mem_foot = ((m+1) * sizeof(*rc_r) + rc_elements_nz * sizeof(*rc_c) + rc_elements_nz * sizeof(*rc_v))/(1024*1024*1.0);
	printf("memory footprint of row_cross (compressed) = %.2lf MB\n", row_cross_compr_mem_foot);

	// in order to make it faster, perhaps calculate rc_r beforehand and use the dgal partitioner for rc_c, rc_v
	int cnt=0;
	rc_r[0] = 0;
	double time_final = time_it(1,
	for(int i=0; i<m; i++){
		long unsigned * local_row_cross = (typeof(local_row_cross)) calloc(num_windows, sizeof(*local_row_cross));
		for(_TYPE_I j=row_ptr[i]; j<row_ptr[i+1]; j++){
			_TYPE_I cw_loc = col_idx[j] / window_width; // col window location
			local_row_cross[cw_loc]++; // if already found in cw_loc, just overwrite it, no problem with that... we just want to identify how many windows for sparse struct later
		}
		for(int k=0;k<num_windows; k++){
			if(local_row_cross[k] != 0){
				rc_c[cnt] = k;
				if(rc_c[cnt] > num_windows)
					printf("rc_c[%d] = %d\n", cnt, rc_c[cnt]);
				rc_v[cnt] = local_row_cross[k];
				cnt++;
			}
		}
		rc_r[i+1] = cnt;
		free(local_row_cross);
	}
	);
	// printf("time_final = %lf\n", time_final);
	*num_windows_out = num_windows;
	if (rc_r_out != NULL)
		*rc_r_out = rc_r;
	if (rc_c_out != NULL)
		*rc_c_out = rc_c;
	if (rc_v_out != NULL)
		*rc_v_out = rc_v;

	free(thread_i_s);
	free(thread_i_e);

}

#undef  csr_extract_row_cross_char
#define csr_extract_row_cross_char  CSR_UTIL_GEN_EXPAND(csr_extract_row_cross_char)
void
csr_extract_row_cross_char(_TYPE_I *row_ptr, _TYPE_I *col_idx, __attribute__((unused)) _TYPE_V *val, int m, int n, int nnz, int window_width, 
					  int *num_windows_out, unsigned char **row_cross_out, int plot, _TYPE_I **rc_r_out, _TYPE_I **rc_c_out, float **rc_v_out)
{
	int num_windows = (n-1 + window_width) / window_width;
	printf("m = %d, n = %d, num_windows = %d\n", m, n, num_windows);
	long unsigned rc_elements = m * num_windows;
	unsigned char * row_cross = (typeof(row_cross)) calloc(rc_elements, sizeof(*row_cross));
	double row_cross_mem_foot = (m * 1.0 * num_windows * sizeof(*row_cross))/(1024*1024*1.0);
	printf("memory footprint of row_cross = %.2lf MB\n", row_cross_mem_foot);

	int num_threads = omp_get_max_threads();

	_TYPE_I * thread_i_s = (_TYPE_I *) malloc(num_threads * sizeof(*thread_i_s));
	_TYPE_I * thread_i_e = (_TYPE_I *) malloc(num_threads * sizeof(*thread_i_e));
	
	unsigned char * row_data = malloc(m);
	double time_row_cross = time_it(1,
	_Pragma("omp parallel")
	{
		// struct BitStream * bs;

		int tnum = omp_get_thread_num();
		loop_partitioner_balance_prefix_sums(num_threads, tnum, row_ptr, m, nnz, &thread_i_s[tnum], &thread_i_e[tnum]);
		_Pragma("omp barrier")
		
		// _Pragma("omp single"){
		// 	for(int tnum = 0; tnum < num_threads; tnum++)
		// 		printf("tnum = %2d\trows = %d\t\tnonzeros = %d\n", tnum, thread_i_e[tnum] - thread_i_s[tnum], row_ptr[thread_i_e[tnum]] - row_ptr[thread_i_s[tnum]], cnt);
		// }

		for(_TYPE_I i=thread_i_s[tnum]; i<thread_i_e[tnum]; i++){
			for(int k=0; k<num_windows; k++){
				// row_cross[i * num_windows + k] = 0;
				row_cross[i * num_windows + k] = '0';
			}
		}
		for(_TYPE_I i=thread_i_s[tnum]; i<thread_i_e[tnum]; i++){
			// printf("tnum=%d\ti=%d\n", tnum, i);
			// int shit[num_windows];
			// int * calloc (shi)
			for(_TYPE_I j=row_ptr[i]; j<row_ptr[i+1]; j++){
				_TYPE_I cw_loc = col_idx[j] / window_width; // col window location
				long unsigned rc_ind = i * num_windows + cw_loc;
				// printf("tnum = %d, i * num_windows + cw_loc = %lu\n", tnum, i * num_windows + cw_loc, cnt);
				// row_cross[rc_ind] = 1;
				row_cross[rc_ind] = '1';
			}
			// unsigned char * tmp_row_data = malloc((num_windows+7)/8);
			// bitstream_init_write(bs, tmp_row_data, (num_windows+7)/8);
			// for(j=0;j<num_windows;j++){
			// 	bitstream_write(bs, shit[j], 1);
			// }
			// row_data[i] = tmp_row_data;
		}
	}
	);
	// printf("time for row_cross = %lf\n", time_row_cross);

	if(plot){
		long unsigned rc_elements_nz = 0;
		double time_reduction = time_it(1,
		_Pragma("omp parallel")
		{
		_Pragma("omp for reduction(+:rc_elements_nz)")
		for(int i=0; i<m; i++){
			for(int j=0; j<num_windows; j++){
				long unsigned rc_ind = i * num_windows + j;
				if(row_cross[rc_ind]!=0) 
					rc_elements_nz++;
			}
		}
		}
		);
		printf("time for row_cross_reduction = %lf\n", time_reduction);
		printf("rc_elements_nz = %lu\n", rc_elements_nz);

		_TYPE_I * rc_r, * rc_c;
		float * rc_v;
		// rc_r = (typeof(rc_r)) malloc(rc_elements_nz * sizeof(*rc_r));
		rc_r = (typeof(rc_r)) malloc((m+1) * sizeof(*rc_r));
		rc_c = (typeof(rc_c)) malloc(rc_elements_nz * sizeof(*rc_c));
		rc_v = (typeof(rc_v)) malloc(rc_elements_nz * sizeof(*rc_v));
		double row_cross_compr_mem_foot = ((m+1) * sizeof(*rc_r) + rc_elements_nz * sizeof(*rc_c) + rc_elements_nz * sizeof(*rc_v))/(1024*1024*1.0);
		printf("memory footprint of row_cross (compressed) = %.2lf MB\n", row_cross_compr_mem_foot);
		// printf("compression ratio = %.2lf%\n", row_cross_compr_mem_foot/row_cross_mem_foot * 100);

		// in order to make it faster, perhaps calculate rc_r beforehand and use the dgal partitioner for rc_c, rc_v
		int cnt=0;
		rc_r[0] = 0;
		double time_final = time_it(1,
		for(int i=0; i<m; i++){
			for(int j=0; j<num_windows; j++){
				long unsigned rc_ind = i * num_windows + j;
				if(row_cross[rc_ind]!=0){
					// rc_r[cnt] = i;
					rc_c[cnt] = j;
					rc_v[cnt] = 1.0;
					cnt++;
				}
				rc_r[i+1] = cnt;
			}
		}
		);
		printf("time_final = %lf\n", time_final);
		if (rc_r_out != NULL)
			*rc_r_out = rc_r;
		if (rc_c_out != NULL)
			*rc_c_out = rc_c;
		if (rc_v_out != NULL)
			*rc_v_out = rc_v;
	}

	free(thread_i_s);
	free(thread_i_e);

	*num_windows_out = num_windows;
	if (row_cross_out != NULL)
		*row_cross_out = row_cross;
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
		figure_set_bounds_x(_fig, 0, n);
		figure_set_bounds_y(_fig, 0, m);
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
		figure_series_type_histogram(_s, 0, 1);
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
		figure_series_type_histogram(_s, 0, 1);
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
		figure_series_type_histogram(_s, 0, 1);
		figure_series_type_barplot(_s);
	);

	free(num_neigh);
}


//==========================================================================================================================================
//= Includes Undefs
//==========================================================================================================================================


#include "sort/samplesort/samplesort_gen_pop.h"
#include "sort/quicksort/quicksort_gen_pop.h"
#include "functools/functools_gen_pop.h"

