#include <stdlib.h>
#include <stdio.h>

#include "macros/macrolib.h"
#include "string_util.h"
#include "array_metrics.h"
#include "plot/plot.h"

#include "csr_util_gen.h"
#include "time_it.h"


#ifndef CSR_UTIL_GEN_C
#define CSR_UTIL_GEN_C

#endif /* CSR_UTIL_GEN_C */


/* Notes: 
 *     - We assume that the matrix is FULLY sorted, i.e. both rows and columns.
 */

/*
 * features_all = ['A_mem_footprint',
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

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//------------------------------------------------------------------------------------------------------------------------------------------
//-                                                              Templates                                                                 -
//------------------------------------------------------------------------------------------------------------------------------------------
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


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
//= Generic Code
//==========================================================================================================================================


#undef  csr_degrees_bandwidths_scatters
#define csr_degrees_bandwidths_scatters  CSR_UTIL_GEN_EXPAND(csr_degrees_bandwidths_scatters)
void
csr_degrees_bandwidths_scatters(_TYPE_I * row_ptr, _TYPE_I * col_idx, long m, long n, __attribute__((unused)) long nnz,
		long ** degrees_rows_out, long ** degrees_cols_out, double ** bandwidths_out, double ** scatters_out)
{
	int num_threads = safe_omp_get_num_threads_external();
	long thread_i_s[num_threads];
	long thread_i_e[num_threads];
	long * degrees_rows = (typeof(degrees_rows)) malloc(m * sizeof(*degrees_rows));
	long * degrees_cols = (typeof(degrees_cols)) malloc(n * sizeof(*degrees_cols));
	double * bandwidths = (typeof(bandwidths)) malloc(m * sizeof(*bandwidths));
	double * scatters = (typeof(scatters)) malloc(m * sizeof(*scatters));
	#pragma omp parallel
	{
		int tnum = omp_get_thread_num();
		long i, i_s, i_e, j, j_s, j_e, degree;
		double b, s;
		loop_partitioner_balance_partial_sums(num_threads, tnum, row_ptr, m, nnz, &thread_i_s[tnum], &thread_i_e[tnum]);
		i_s = thread_i_s[tnum];
		i_e = thread_i_e[tnum];
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
	// for (i=0;i<m;i++)
		// printf("%lf\n", degrees_rows[i]);
	*degrees_rows_out = degrees_rows;
	*degrees_cols_out = degrees_cols;
	*bandwidths_out = bandwidths;
	*scatters_out = scatters;
}


#undef  csr_groups_per_row
#define csr_groups_per_row  CSR_UTIL_GEN_EXPAND(csr_groups_per_row)
void
csr_groups_per_row(_TYPE_I * row_ptr, _TYPE_I * col_idx, long m, __attribute__((unused)) long n, __attribute__((unused)) long nnz, long max_gap_size,
		long ** groups_per_row_out)
{
	long * groups_per_row = (typeof(groups_per_row)) malloc(m * sizeof(*groups_per_row));
	#pragma omp parallel
	{
		long i, j, k, degree;
		long num_groups;
		#pragma omp for
		for (i=0;i<m;i++)
		{
			groups_per_row[i] = 0;
			degree = row_ptr[i+1] - row_ptr[i];
			if (degree <= 0)
				continue;
			j = row_ptr[i];
			num_groups = 0;
			while (j < row_ptr[i+1])
			{
				k = j + 1;
				while ((k < row_ptr[i+1]) && (col_idx[k] - col_idx[k-1] <= max_gap_size + 1))   // distance 1 means gap 0
					k++;
				num_groups++;
				j = k;
			}
			groups_per_row[i] = num_groups;
		}
	}
	if (groups_per_row_out != NULL)
		*groups_per_row_out = groups_per_row;
}


#undef  csr_nnz_grouping
#define csr_nnz_grouping  CSR_UTIL_GEN_EXPAND(csr_nnz_grouping)
void
csr_nnz_grouping(_TYPE_I * row_ptr, _TYPE_I * col_idx, long m, __attribute__((unused)) long n, __attribute__((unused)) long nnz, long max_gap_size,
		long ** groups_per_row_out)
{
	long * groups_per_row = (typeof(groups_per_row)) malloc(m * sizeof(*groups_per_row));
	#pragma omp parallel
	{
		long i, j, k, degree;
		long num_groups;
		#pragma omp for
		for (i=0;i<m;i++)
		{
			groups_per_row[i] = 0;
			degree = row_ptr[i+1] - row_ptr[i];
			if (degree <= 0)
				continue;
			j = row_ptr[i];
			num_groups = 0;
			while (j < row_ptr[i+1])
			{
				k = j + 1;
				while ((k < row_ptr[i+1]) && (col_idx[k] - col_idx[k-1] <= max_gap_size + 1))   // distance 1 means gap 0
					k++;
				num_groups++;
				j = k;
			}
			groups_per_row[i] = num_groups;
		}
	}
	if (groups_per_row_out != NULL)
		*groups_per_row_out = groups_per_row;
}


/* Notes: 
 *     - 'window_size': Distance from left and right.
 */
#undef  csr_row_neighbours
#define csr_row_neighbours  CSR_UTIL_GEN_EXPAND(csr_row_neighbours)
double *
csr_row_neighbours(_TYPE_I * row_ptr, _TYPE_I * col_idx, long m, __attribute__((unused)) long n, long nnz, long window_size)
{
	int num_threads = safe_omp_get_num_threads_external();
	long thread_i_s[num_threads];
	long thread_i_e[num_threads];
	double * num_neigh = (typeof(num_neigh)) malloc(nnz * sizeof(*num_neigh));
	#pragma omp parallel
	{
		int tnum = omp_get_thread_num();
		long i, i_s, i_e, j, k;
		loop_partitioner_balance_partial_sums(num_threads, tnum, row_ptr, m, nnz, &thread_i_s[tnum], &thread_i_e[tnum]);
		i_s = thread_i_s[tnum];
		i_e = thread_i_e[tnum];
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
	return num_neigh;
}


/* Notes: 
 *     - 'window_size': Distance from left and right.
 */
#undef  csr_avg_row_neighbours
#define csr_avg_row_neighbours  CSR_UTIL_GEN_EXPAND(csr_avg_row_neighbours)
double
csr_avg_row_neighbours(_TYPE_I * row_ptr, _TYPE_I * col_idx, long m, __attribute__((unused)) long n, long nnz, long window_size)
{
	long total_num_neigh = 0;
	#pragma omp parallel
	{
		long i, j, k;
		long num_neigh = 0;
		#pragma omp for
		for (i=0;i<m;i++)
		{
			for (j=row_ptr[i];j<row_ptr[i+1];j++)
			{
				for (k=j+1;k<row_ptr[i+1];k++)
				{
					if (col_idx[k] - col_idx[j] > window_size)
						break;
					num_neigh += 2;
				}
			}
		}
		__atomic_fetch_add(&total_num_neigh, num_neigh, __ATOMIC_RELAXED);
	}
	return ((double) total_num_neigh) / nnz;
}


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
		__atomic_store(&t_row_similarity[tnum], &row_similarity, __ATOMIC_RELAXED);
		__atomic_fetch_add(&total_num_non_empty_rows, num_non_empty_rows, __ATOMIC_RELAXED);
	}
	if (total_num_non_empty_rows == 0)
		return 0;
	total_row_similarity = 0;
	for (long i=0;i<num_threads;i++)
		total_row_similarity += t_row_similarity[i];
	return total_row_similarity / total_num_non_empty_rows;
}


#undef  csr_cross_row_neighbours
#define csr_cross_row_neighbours  CSR_UTIL_GEN_EXPAND(csr_cross_row_neighbours)
double
csr_cross_row_neighbours(_TYPE_I * row_ptr, _TYPE_I * col_idx, long m, __attribute__((unused)) long n, __attribute__((unused)) long nnz, long window_size)
{
	double * num_neigh = (typeof(num_neigh)) malloc((m) * sizeof(*num_neigh));
	long num_non_empty_rows = 0;
	#pragma omp parallel
	{
		long i, j, k, k_s, k_e, l;
		long degree, curr_neigh, column_diff;
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
			__atomic_fetch_add(&num_non_empty_rows, 1, __ATOMIC_RELAXED);
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
	}
	double mean = array_mean(num_neigh, m);
	free(num_neigh);
	if (num_non_empty_rows == 0)
		return 0;
	return (mean * m) / num_non_empty_rows;
}


#undef  csr_matrix_features
#define csr_matrix_features  CSR_UTIL_GEN_EXPAND(csr_matrix_features)
void
csr_matrix_features(__attribute__((unused)) char * title_base, _TYPE_I * row_ptr, _TYPE_I * col_idx, long m, long n, long nnz)
{
	long * degrees_rows;
	long * degrees_cols;
	double * bandwidths;
	double * scatters;
	double nnz_per_row_min, nnz_per_row_max, nnz_per_row_mean, nnz_per_row_std;
	double min_degree_col, max_degree_col, nnz_per_col_mean, nnz_per_col_std;
	double bw_min, bw_max, bw_mean, bw_std;
	double sc_min, sc_max, sc_mean, sc_std;
	double groups_per_row_min, groups_per_row_max, groups_per_row_mean, groups_per_row_std;

	long window_size;
	double * num_neigh;
	double num_neigh_mean;
	__attribute__((unused)) double std_neigh;
	double cross_row_similarity_avg;
	long max_gap_size;
	long * groups_per_row;

	double time;
	long i;

	time = time_it(1,
		csr_degrees_bandwidths_scatters(row_ptr, col_idx, m, n, nnz, &degrees_rows, &degrees_cols, &bandwidths, &scatters);
	);
	printf("time csr_degrees_bandwidths_scatters = %lf\n", time);

	array_min_max(degrees_rows, m, &nnz_per_row_min, NULL, &nnz_per_row_max, NULL);
	nnz_per_row_mean = ((double) nnz) / m;
	nnz_per_row_std = array_std(degrees_rows, m, nnz_per_row_mean);

	array_min_max(degrees_cols, n, &min_degree_col, NULL, &max_degree_col, NULL);
	nnz_per_col_mean = ((double) nnz) / n;
	nnz_per_col_std = array_std(degrees_cols, n, nnz_per_col_mean);

	array_min_max(bandwidths, m, &bw_min, NULL, &bw_max, NULL);
	bw_mean = array_mean(bandwidths, m);
	bw_std = array_std(bandwidths, m, bw_mean);

	array_min_max(scatters, m, &sc_min, NULL, &sc_max, NULL);
	sc_mean = array_mean(scatters, m);
	sc_std = array_std(scatters, m, sc_mean);

	free(degrees_rows);
	free(bandwidths);
	free(scatters);

	// window_size = 64 / sizeof(double) / 2;
	// window_size = 8;
	// window_size = 4;
	window_size = 1;
	// printf("window size = %ld\n", window_size);

	time = time_it(1,
		num_neigh = csr_row_neighbours(row_ptr, col_idx, m, n, nnz, window_size);
	);
	printf("time csr_row_neighbours = %lf\n", time);

	num_neigh_mean = array_mean(num_neigh, nnz);
	std_neigh = array_std(num_neigh, nnz, num_neigh_mean);
	free(num_neigh);

	time = time_it(1,
		cross_row_similarity_avg = csr_cross_row_similarity(row_ptr, col_idx, m, n, nnz, window_size);
	);
	printf("time csr_cross_row_similarity = %lf\n", time);

	max_gap_size = 0;
	time = time_it(1,
		csr_groups_per_row(row_ptr, col_idx, m, n, nnz, max_gap_size, &groups_per_row);
	);
	printf("time csr_groups_per_row = %lf\n", time);
	array_min_max(groups_per_row, m, &groups_per_row_min, NULL, &groups_per_row_max, NULL);
	groups_per_row_mean = array_mean(groups_per_row, m);
	groups_per_row_std = array_std(groups_per_row, m, groups_per_row_mean);

	// clustering  = np.asarray([(ngroups[i]/nnz_per_row[i]) if (nnz_per_row[i]>0) else 0 for i in range(len(ngroups))])

	#if 0
		struct {
			unsigned char type;
			char * name;
			union {
				char * s;
				long i;
				double f;
			} val;
		} features[] = {
			{'s', "matrix", .val.s=title_base},
			{'i', "nr_rows", .val.i=m}, {'i', "nr_cols", .val.i=n}, {'i', "nr_nzeros", .val.i=nnz}, {'f', "density", .val.f = nnz / ((double) m*n)},

			{'f', "nnz-r-min", .val.f=nnz_per_row_min}, {'f', "nnz-r-max", .val.f=nnz_per_row_max}, {'f', "nnz-r-avg", .val.f=nnz_per_row_mean}, {'f', "nnz-r-std", .val.f=nnz_per_row_std},
			{'f', "nnz-c-min", .val.f=min_degree_col}, {'f', "nnz-c-max", .val.f=max_degree_col}, {'f', "nnz-c-avg", .val.f=nnz_per_col_mean}, {'f', "nnz-c-std", .val.f=nnz_per_col_std},

			{'f', "skew_coeff", .val.f = (nnz_per_row_max - nnz_per_row_mean) / nnz_per_row_mean},

			{'f', "bw-min", .val.f=bw_min}, {'f', "bw-max", .val.f=bw_max}, {'f', "bw-avg", .val.f=bw_mean}, {'f', "bw-std", .val.f=bw_std},
			{'f', "bw-scaled-min", .val.f = bw_min / n}, {'f', "bw-scaled-max", .val.f = bw_max / n}, {'f', "bw-scaled-avg", .val.f = bw_mean / n}, {'f', "bw-scaled-std", .val.f = bw_std / n},
			{'f', "sc-min", .val.f = sc_min}, {'f', "sc-max", .val.f = sc_max}, {'f', "sc-avg", .val.f = sc_mean}, {'f', "sc-std", .val.f = sc_std},
			{'f', "sc-scaled-min", .val.f = sc_min * n}, {'f', "sc-scaled-max", .val.f = sc_max * n}, {'f', "sc-scaled-avg", .val.f = sc_mean * n}, {'f', "sc-scaled-std", .val.f = sc_std * n},

			{'f', "nr_groups-r-min", .val.f = groups_per_row_min}, {'f', "nr_groups-r-max", .val.f = groups_per_row_max}, {'f', "nr_groups-r-avg", .val.f = groups_per_row_mean}, {'f', "nr_groups-r-std", .val.f = groups_per_row_std},

			// {'i', "num_groups", },
			// {'f', "nnz_per_cluster-avg", },

			{'f', "num-neigh-avg", .val.f = num_neigh_mean},
			{'f', "cross_row_sim-avg", .val.f = cross_row_similarity_avg},
		};
		long num_features = sizeof(features) / sizeof(*features);

		for (i=0;i<num_features;i++)
		{
			printf("%s = ", features[i].name);
			switch (features[i].type) {
				case 's': printf("%s", features[i].val.s); break;
				case 'i': printf("%ld", features[i].val.i); break;
				case 'f': printf("%lf", features[i].val.f); break;
			}
			printf("\n");
		}
	#endif

	// for (i=0;i<num_features;i++)
		// printf("%s\t", features[i].name);
	// printf("\b \b\n");
	// for (i=0;i<num_features;i++)
	// {
		// switch (features[i].type) {
			// case 's': printf("%s,", features[i].val.s); break;
			// case 'i': printf("%ld,", features[i].val.i); break;
			// case 'f': printf("%lf,", features[i].val.f); break;
		// }
	// }
	// printf("\b \b\n");


	// printf("matrix\tnr_rows\tnr_cols\tnr_nnzs\tdensity\tnnz-r-min\tnnz-r-max\tnnz-r-avg\tnnz-r-std\tskew_coeff\tbw-min\tbw-max\tbw-avg\tbw-std\tnum-neigh-avg\tcross_row_sim-avg\tnum_groups\tnnz_per_cluster-avg\n");
	// printf("%s\t", title_base);
	// printf("%ld\t", m);
	// printf("%ld\t", n);
	// printf("%ld\t", nnz);
	// printf("%lf\t", nnz / ((double) m*n));
	// printf("%lf\t", nnz_per_row_min);
	// printf("%lf\t", nnz_per_row_max);
	// printf("%lf\t", nnz_per_row_mean);
	// printf("%lf\t", nnz_per_row_std);
	// printf("%lf\t", (nnz_per_row_max - nnz_per_row_mean) / nnz_per_row_mean);
	// printf("%lf\t", bw_min / n);
	// printf("%lf\t", bw_max / n);
	// printf("%lf\t", bw_mean / n);
	// printf("%lf\t", bw_std / n);
	// printf("%lf\t", num_neigh_mean);
	// printf("%lf\t", cross_row_similarity_avg);
	// printf("%ld\t", total_groups);
	// printf("%lf\t", ((double) nnz) / total_groups);
	// printf("\n");

	/* Matrix features for artificial twins.
	 * Also print the csr mem footprint for easier sorting.
	 */
	#if 1
		double csr_mem_footprint = nnz * (sizeof(double) + sizeof(int)) + (m+1) * sizeof(int);
		fprintf(stderr, "%-15.5lf ", csr_mem_footprint / (1024*1024));
		fprintf(stderr, "['%s']='", title_base);
		fprintf(stderr, "%ld ", m);
		fprintf(stderr, "%ld ", n);
		fprintf(stderr, "%.10lf ", nnz_per_row_mean);
		fprintf(stderr, "%.10lf ", nnz_per_row_std);
		fprintf(stderr, "normal ");
		fprintf(stderr, "random ");
		fprintf(stderr, "%.10lf ", bw_mean / n);
		fprintf(stderr, "%.10lf ", (nnz_per_row_max - nnz_per_row_mean) / nnz_per_row_mean);
		fprintf(stderr, "%.10lf ", num_neigh_mean);
		fprintf(stderr, "%.10lf ", cross_row_similarity_avg);
		fprintf(stderr, "14 ");
		fprintf(stderr, "%s", title_base);
		fprintf(stderr, "'\n");
	#endif

}


//==========================================================================================================================================
//= Ploting
//==========================================================================================================================================


#undef  csr_plot
#define csr_plot  CSR_UTIL_GEN_EXPAND(csr_plot)
void
csr_plot(_TYPE_I * row_ptr, _TYPE_I * col_idx, __attribute__((unused)) _TYPE_V * val, long m, long n, long nnz, char * file_out)
{
	long num_pixels = 1024;
	long num_pixels_x, num_pixels_y;
	long buf_n = strlen(file_out) + 1 + 1000;
	char buf[buf_n], buf_title[buf_n];
	char * path, * filename, * filename_base;

	double get_row(void * A, long pos)
	{
		_TYPE_I * row_ptr = A;
		long i = binary_search(row_ptr, 0, m, pos, NULL, NULL);
		if (row_ptr[i] > pos)
			i--;
		return (double) i;
	}
	double get_col(void * A, long pos)
	{
		_TYPE_I * col_idx = A;
		return (double) col_idx[pos];
	}
	double get_degree(void * A, long i)
	{
		_TYPE_I * row_ptr = A;
		return (double) row_ptr[i+1] -  row_ptr[i];
	}

	if (n < 1024)
	{
		num_pixels_x = n;
		num_pixels_y = m;
	}
	else
	{
		num_pixels_x = num_pixels;
		num_pixels_y = num_pixels;
	}

	str_path_split_path(file_out, strlen(file_out) + 1, buf, buf_n, &path, &filename);
	path = strdup(path);
	filename = strdup(filename);

	str_path_split_ext(filename, strlen(filename) + 1, buf, buf_n, &filename_base, NULL);
	filename_base = strdup(filename_base);

	// printf("%s , %s , %s\n", path, filename, filename_base);

	// matrix
	// if (0)
	{
		snprintf(buf, buf_n, "%s/%s.png", path, filename_base);
		snprintf(buf_title, buf_n, "%s", filename_base);
		figure_simple_plot(buf, num_pixels_x, num_pixels_y, (col_idx, row_ptr, NULL, nnz, 0, get_col, get_row),
			figure_set_title(_fig, buf_title);
			figure_enable_legend(_fig);
			figure_axes_flip_y(_fig);
			figure_set_bounds_x(_fig, 0, n);
			figure_set_bounds_y(_fig, 0, m);
			figure_series_type_density_map(_s);
		);
	}

	// degree histogram
	if (0)
	{
		double min_degree, max_degree;
		long num_bins;
		array_min_max(row_ptr, m, &min_degree, NULL, &max_degree, NULL, get_degree);
		printf("min degree = %g , max degree = %g\n", min_degree, max_degree);
		// num_bins = 10000;
		num_bins = max_degree - min_degree + 1;
		snprintf(buf, buf_n, "%s/%s_degree_distribution.png", path, filename_base);
		snprintf(buf_title, buf_n, "%s: degree distribution", filename_base);
		figure_simple_plot(buf, num_pixels_x, num_pixels_y, (NULL, row_ptr, NULL, m, 0, NULL, get_degree),
			figure_set_bounds_x(_fig, 0, 100);
			figure_enable_legend(_fig);
			figure_set_title(_fig, buf_title);
			figure_series_type_histogram(_s, num_bins, 1);
			figure_series_type_barplot(_s);
		);
	}

	// neighbour distances frequencies
	/* if (0)
	{
		double * neigh_distances_freq_perc;
		int ignore_big_rows;
		long pos;
		double x_bound;
		ignore_big_rows = 0;
		// ignore_big_rows = 1;
		neigh_distances_freq_perc = csr_neighbours_distances_frequencies(row_ptr, col_idx, m, n, nnz, ignore_big_rows);
		x_bound = -1;
		// x_bound = 100;
		// x_bound = 1000;
		for (pos=n-1;pos>0;pos--)
			if (neigh_distances_freq_perc[pos] != 0)
				break;
		if (x_bound > 0)
			snprintf(buf, buf_n, "%s/%s_neigh_dist_freq_x_%d.png", path, filename_base, (int) x_bound);
		else
			snprintf(buf, buf_n, "%s/%s_neigh_dist_freq.png", path, filename_base);
		snprintf(buf_title, buf_n, "%s: neighbour distances frequencies (percentages)", filename_base);
		figure_simple_plot(buf, num_pixels_x, num_pixels_y, (NULL, neigh_distances_freq_perc, NULL, pos+1, 0),
			figure_enable_legend(_fig);
			figure_set_title(_fig, buf_title);
			if (x_bound > 0)
				figure_set_bounds_x(_fig, 0, x_bound);
			figure_series_type_barplot(_s);
		);
		free(neigh_distances_freq_perc);
	} */

	// cluster sizes frequencies
	/* if (0)
	{
		double * cluster_sizes_freq_perc;
		long max_gap_size;
		long total_groups;
		long pos;
		double x_bound;
		max_gap_size = 0;
		csr_cluster_sizes_frequencies(row_ptr, col_idx, m, n, nnz, max_gap_size, &cluster_sizes_freq_perc, &total_groups);
		// long total_groups_2 = csr_groups_per_row(row_ptr, col_idx, m, n, nnz, max_gap_size);
		fprintf(stderr, "%s %lf\n", filename_base, (nnz / (double) total_groups));
		x_bound = -1;
		// x_bound = 100;
		// x_bound = 1000;
		for (pos=n;pos>0;pos--)    // From n, not n-1, because cluster sizes are between 1 and n, so the array has n+1 elements.
			if (cluster_sizes_freq_perc[pos] != 0)
				break;
		if (x_bound > 0)
			snprintf(buf, buf_n, "%s/%s_cluster_sizes_freq_x_%d.png", path, filename_base, (int) x_bound);
		else
			snprintf(buf, buf_n, "%s/%s_cluster_sizes_freq.png", path, filename_base);
		snprintf(buf_title, buf_n, "%s: cluster sizes frequencies (percentages)", filename_base);
		figure_simple_plot(buf, num_pixels_x, num_pixels_y, (NULL, cluster_sizes_freq_perc, NULL, pos+1, 0),
			figure_enable_legend(_fig);
			figure_set_title(_fig, buf_title);
			if (x_bound > 0)
				figure_set_bounds_x(_fig, 0, x_bound);
			figure_series_type_barplot(_s);
		);
		free(cluster_sizes_freq_perc);
	} */

	free(path);
	free(filename);
	free(filename_base);
}

