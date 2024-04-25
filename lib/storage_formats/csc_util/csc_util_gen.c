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

#include "csc_util_gen.h"


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
#define FUNCTOOLS_GEN_SUFFIX  CONCAT(_CSC_UTIL_GEN_add_d, CSC_UTIL_GEN_SUFFIX)
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
#define QUICKSORT_GEN_SUFFIX  CONCAT(_CSC_UTIL_GEN_d, CSC_UTIL_GEN_SUFFIX)
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
#define quicksort_sign(A, N, aux, partitions_buf) CONCAT(quicksort_CSC_UTIL_GEN_d_sign, CSC_UTIL_GEN_SUFFIX)(A, N, aux, partitions_buf)


//------------------------------------------------------------------------------------------------------------------------------------------
//- Samplesort
//------------------------------------------------------------------------------------------------------------------------------------------

#include "sort/samplesort/samplesort_gen_push.h"
#define SAMPLESORT_GEN_TYPE_1  double
#define SAMPLESORT_GEN_TYPE_2  int
#define SAMPLESORT_GEN_TYPE_3  int
#define SAMPLESORT_GEN_TYPE_4  void
#define SAMPLESORT_GEN_SUFFIX  CONCAT(_CSC_UTIL_GEN_d, CSC_UTIL_GEN_SUFFIX)
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
#define _TYPE_V  CSC_UTIL_GEN_EXPAND(_TYPE_V)
typedef CSC_UTIL_GEN_TYPE_1  _TYPE_V;

#undef  _TYPE_I
#define _TYPE_I  CSC_UTIL_GEN_EXPAND(_TYPE_I)
typedef CSC_UTIL_GEN_TYPE_2  _TYPE_I;


//==========================================================================================================================================
//------------------------------------------------------------------------------------------------------------------------------------------
//-                                                              Templates                                                                 -
//------------------------------------------------------------------------------------------------------------------------------------------
//==========================================================================================================================================


//==========================================================================================================================================
//= Structural Features
//==========================================================================================================================================


// Expand col indices.
#undef  csc_col_indices
#define csc_col_indices  CSC_UTIL_GEN_EXPAND(csc_col_indices)
void
csc_col_indices(__attribute__((unused)) _TYPE_I * row_idx, _TYPE_I * col_ptr, __attribute__((unused)) long m, long n, long nnz,
		_TYPE_I ** col_idx_out)
{
	_TYPE_I * col_idx;
	col_idx = (typeof(col_idx)) malloc(nnz * sizeof(*col_idx));
	#pragma omp parallel
	{
		long i, j;
		#pragma omp for
		for (j=0;j<n;j++)
			for (i=col_ptr[j];i<col_ptr[j+1];i++)
				col_idx[i] = j;
	}
	*col_idx_out = col_idx;
}

/* Notes: 
 *     - 'window_size': Distance from left and right.
 */
#undef  csc_col_neighbours
#define csc_col_neighbours  CSC_UTIL_GEN_EXPAND(csc_col_neighbours)
void
csc_col_neighbours(_TYPE_I * row_idx, _TYPE_I * col_ptr, __attribute__((unused)) long m, long n, long nnz, long window_size,
		_TYPE_I ** num_neigh_out)
{
	int num_threads = safe_omp_get_num_threads_external();
	_TYPE_I * num_neigh = (typeof(num_neigh)) malloc(nnz * sizeof(*num_neigh));
	#pragma omp parallel
	{
		int tnum = omp_get_thread_num();
		long i, i_s, i_e, j, k;
		loop_partitioner_balance_prefix_sums(num_threads, tnum, col_ptr, n, nnz, &i_s, &i_e);
		#pragma omp for
		for (i=0;i<nnz;i++)
			num_neigh[i] = 0;
		for (i=i_s;i<i_e;i++)
		{
			for (j=col_ptr[i];j<col_ptr[i+1];j++)
			{
				for (k=j+1;k<col_ptr[i+1];k++)
				{
					if (row_idx[k] - row_idx[j] > window_size)
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

#undef  csc_cross_col_neighbours
#define csc_cross_col_neighbours  CSC_UTIL_GEN_EXPAND(csc_cross_col_neighbours)
void
csc_cross_col_neighbours(_TYPE_I * row_idx, _TYPE_I * col_ptr, __attribute__((unused)) long m, long n, __attribute__((unused)) long nnz, long window_size,
		_TYPE_I **ccs_neigh_out)
{
	_TYPE_I * ccs_neigh = (typeof(ccs_neigh)) malloc(nnz * sizeof(*ccs_neigh));
	#pragma omp parallel
	{
		long i, j, k, k_s, k_e, l;
		long degree, row_diff;
		#pragma omp for
		for (i=0;i<nnz;i++)
			ccs_neigh[i] = 0;

		#pragma omp for
		for (i=0;i<n;i++)
		{
			degree = col_ptr[i+1] - col_ptr[i];
			if (degree <= 0)
				continue;
			for (l=i+1;l<n;l++)       // Find next non-empty col.
				if (col_ptr[l+1] - col_ptr[l] > 0)
					break;
			if (l < n)
			{
				k_s = col_ptr[l];
				k_e = col_ptr[l+1];
				k = k_s;
				for (j=col_ptr[i];j<col_ptr[i+1];j++)
				{
					while (k < k_e)
					{
						row_diff = row_idx[k] - row_idx[j];
						if (labs(row_diff) <= window_size)
						{
							ccs_neigh[j]+=1;
							// do not break, in order to register all neighbours
							// break; 
						}
						if (row_diff <= 0)
							k++;
						else
							break;   // went outside of area to examine
					}
				}
			}
		}
	}
	if (ccs_neigh_out != NULL)
		*ccs_neigh_out = ccs_neigh;
	else
		free(ccs_neigh);
}

#undef  csc_save_to_mtx
#define csc_save_to_mtx  CSC_UTIL_GEN_EXPAND(csc_save_to_mtx)
void
csc_save_to_mtx(_TYPE_I * row_idx, _TYPE_I * col_ptr, _TYPE_V * val, int num_rows, int num_cols, const char* filename)
{
	printf("Storing: %s\n", filename);
	FILE* file;
	file = fopen(filename, "w");
	if (file == NULL) {
		printf("Error opening file %s\n", filename);
		return;
	}

	fprintf(file, "%%%%MatrixMarket matrix coordinate real general\n");
	fprintf(file, "%d %d %d\n", num_rows, num_cols, col_ptr[num_cols]);
	long buf_n = 1000;
	char buf[buf_n];
	long k;
	for (int i = 0; i < num_cols; i++) {
		for (int j = col_ptr[i]; j < col_ptr[i + 1]; j++) {
			k = 0;
			k += snprintf(buf, buf_n-k, "%d %d %.15g", row_idx[j] + 1, i + 1, val[j]);
			fprintf(file, "%s\n", buf);
		}
	}
	fclose(file);
}


//==========================================================================================================================================
//= Ploting
//==========================================================================================================================================


#undef  csc_plot
#define csc_plot  CSC_UTIL_GEN_EXPAND(csc_plot)
void
csc_plot(char * title_base, _TYPE_I * row_idx, _TYPE_I * col_ptr, __attribute__((unused)) _TYPE_V * val, long m, long n, long nnz, int enable_legend, long num_pixels_x, long num_pixels_y)
{
	_TYPE_I * col_idx;
	long buf_n = strlen(title_base) + 1 + 1000;
	char buf[buf_n], buf_title[buf_n];

	csc_col_indices(row_idx, col_ptr, m, n, nnz, &col_idx);
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
	free(col_idx);
}

#undef  csc_col_size_histogram_plot
#define csc_col_size_histogram_plot  CSC_UTIL_GEN_EXPAND(csc_col_size_histogram_plot)
void
csc_col_size_histogram_plot(char * title_base, __attribute__((unused)) _TYPE_I * row_idx, _TYPE_I * col_ptr, __attribute__((unused)) _TYPE_V * val, __attribute__((unused)) long m, long n, __attribute__((unused)) long nnz,
		int enable_legend, long num_pixels_x, long num_pixels_y)
{
	_TYPE_I * degrees_cols;
	long buf_n = strlen(title_base) + 1 + 1000;
	char buf[buf_n], buf_title[buf_n];

	degrees_cols = (typeof(degrees_cols)) malloc(n * sizeof(*degrees_cols));
	#pragma omp parallel for
	for(int i=0; i<n; i++)
		degrees_cols[i] = col_ptr[i+1] - col_ptr[i];
	
	// Degree histogram. this can be commented out
	double nnz_per_col_min, nnz_per_col_max, nnz_per_col_avg, nnz_per_col_std;
	array_min_max(degrees_cols, n, &nnz_per_col_min, NULL, &nnz_per_col_max, NULL);
	array_mean(degrees_cols, n, &nnz_per_col_avg);
	array_std(degrees_cols, n, &nnz_per_col_std);

	printf("nnz_per_col_max = %.0lf\n", nnz_per_col_max);
	printf("nnz_per_col_min = %.0lf\n", nnz_per_col_min);
	printf("nnz_per_col_avg = %.4lf\n", nnz_per_col_avg);
	printf("nnz_per_col_std = %.4lf\n", nnz_per_col_std);

	/*	
	int *col_size_hist = (int*) calloc((int)nnz_per_col_max+1, sizeof(int));
	for(int i=0;i<n;i++)
		col_size_hist[degrees_cols[i]]++;
	for (int i = 0; i < (int)nnz_per_col_max+1; ++i){
		printf("%d %d", i, col_size_hist[i]);
		if(col_size_hist[i]*100.0/n > 0.5)
			printf(" ( %.4f )", col_size_hist[i]*100.0/n);
		printf("\n");
	}
	free(col_size_hist);
	*/
	
	snprintf(buf, buf_n, "%s_col_size_distribution.png", title_base);
	snprintf(buf_title, buf_n, "%s: col size distribution", title_base);
	figure_simple_plot(buf, num_pixels_x, num_pixels_y, (NULL, degrees_cols, NULL, n, 0),
		if (enable_legend)
			figure_enable_legend(_fig);
		figure_set_title(_fig, buf_title);
		figure_series_type_histogram(_s, 0, NULL, 1);
		figure_series_type_barplot(_s);
	);

	free(degrees_cols);
}

#undef  csc_nnz_size_batch_n_bar_plot
#define csc_nnz_size_batch_n_bar_plot  CSC_UTIL_GEN_EXPAND(csc_nnz_size_batch_n_bar_plot)
void
csc_nnz_size_batch_n_bar_plot(__attribute__((unused)) char * title_base, __attribute__((unused)) _TYPE_I * row_idx, _TYPE_I * col_ptr, __attribute__((unused)) _TYPE_V * val, __attribute__((unused)) long m, long n, __attribute__((unused)) long nnz, int batch_n,
		__attribute__((unused)) int enable_legend, __attribute__((unused)) long num_pixels_x, __attribute__((unused)) long num_pixels_y)
{

	// _TYPE_I * degrees_cols;
	// degrees_cols = (typeof(degrees_cols)) malloc(n * sizeof(*degrees_cols));
	// #pragma omp parallel for
	// for(int i=0; i<n; i++)
	// 	degrees_cols[i] = col_ptr[i+1] - col_ptr[i];
	// _TYPE_I empty_cols = 0;
	// for(int i=0; i<n; i++)
	// 	if(degrees_cols[i] == 0)
	// 		empty_cols++;
	// printf("Empty cols: %ld\n", empty_cols);

	_TYPE_I * nnz_part;
	_TYPE_I n_b = (n-1 + batch_n)/batch_n;
	nnz_part = (typeof(nnz_part)) malloc(n_b * sizeof(*nnz_part));

	_Pragma("omp parallel")
	{
		_Pragma("omp for")
		for(int i=0; i<n_b; i++){
			_TYPE_I col_s = i*batch_n;
			// col_f is a variable that is either (i+1)*batch_n or n, whatever is smaller
			_TYPE_I col_f = (i+1)*batch_n > n ? n : (i+1)*batch_n;
			nnz_part[i] = col_ptr[col_f] - col_ptr[col_s];
		}
	}

	printf("n = %ld, batch_n = %d, n_b = %d\n", n, batch_n, n_b);
	// for(int i=0; i<10; i++)
	// 	printf("%d/%d\t-> %d values (%.4f\%)\n", i+1, n_b, nnz_part[i], nnz_part[i]*100.0/nnz);

	int empty_col_cnt = 0;
	for(int i=0; i<n_b; i++)
		if(nnz_part[i]==0)
			empty_col_cnt++;
	printf("empty_col_cnt = %d\n", empty_col_cnt);

	// Degree histogram. this can be commented out
	double nnz_part_min, nnz_part_max, nnz_part_avg, nnz_part_std;
	array_min_max(nnz_part, n_b, &nnz_part_min, NULL, &nnz_part_max, NULL);
	array_mean(nnz_part, n_b, &nnz_part_avg);
	array_std(nnz_part, n_b, &nnz_part_std);
	printf("%d batches of %d cols\n", n_b, batch_n);
	printf("nnz_part_max = %.4lf\n", nnz_part_max);
	printf("nnz_part_min = %.4lf\n", nnz_part_min);
	printf("nnz_part_avg = %.4lf\n", nnz_part_avg);
	printf("nnz_part_std = %.4lf\n", nnz_part_std);

	free(nnz_part);
}

#undef  csc_num_neigh_histogram_plot
#define csc_num_neigh_histogram_plot  CSC_UTIL_GEN_EXPAND(csc_num_neigh_histogram_plot)
void
csc_num_neigh_histogram_plot(char * title_base, _TYPE_I * row_idx, _TYPE_I * col_ptr, __attribute__((unused)) _TYPE_V * val, long m, long n, long nnz, int window_size,
		int enable_legend, long num_pixels_x, long num_pixels_y)
{
	_TYPE_I *num_neigh;
	long buf_n = strlen(title_base) + 1 + 1000;
	char buf[buf_n], buf_title[buf_n];

	// extract num_neigh per row
	csc_col_neighbours(row_idx, col_ptr, m, n, nnz, window_size, &num_neigh);

	// num_neigh (per row) histogram. this can be commented out
	double num_neigh_min, num_neigh_max;
	array_min_max(num_neigh, nnz, &num_neigh_min, NULL, &num_neigh_max, NULL);
	
	int *num_neigh_hist = (int*) calloc((int)num_neigh_max+1, sizeof(int));
	for(int i=0;i<nnz;i++)
		num_neigh_hist[num_neigh[i]]++;
	for (int i = 0; i < (int)num_neigh_max+1; ++i)
		printf("%d: %d (%.4f%%)\n", i, num_neigh_hist[i], num_neigh_hist[i]*100.0/nnz);
	free(num_neigh_hist);
	
	snprintf(buf, buf_n, "%s_num_neigh_col_distribution.png", title_base);
	snprintf(buf_title, buf_n, "%s: num_neigh_col distribution", title_base);
	figure_simple_plot(buf, num_pixels_x, num_pixels_y, (NULL, num_neigh, NULL, nnz, 0),
		if (enable_legend)
			figure_enable_legend(_fig);
		figure_set_title(_fig, buf_title);
		figure_series_type_histogram(_s, 0, NULL, 1);
		figure_series_type_barplot(_s);
	);

	free(num_neigh);
}

#undef  csc_cross_col_similarity_histogram_plot
#define csc_cross_col_similarity_histogram_plot  CSC_UTIL_GEN_EXPAND(csc_cross_col_similarity_histogram_plot)
void
csc_cross_col_similarity_histogram_plot(char * title_base, _TYPE_I * row_idx, _TYPE_I * col_ptr, __attribute__((unused)) _TYPE_V * val, long m, long n, long nnz, int window_size,
		int enable_legend, long num_pixels_x, long num_pixels_y)
{
	_TYPE_I *ccs_neigh;
	long buf_n = strlen(title_base) + 1 + 1000;
	char buf[buf_n], buf_title[buf_n];

	// extract cross_col_similarity per nnz
	csc_cross_col_neighbours(row_idx, col_ptr, m, n, nnz, window_size, &ccs_neigh);

	// cross_col_similarity (per nnz) histogram. this can be commented out
	double ccs_neigh_min, ccs_neigh_max;
	array_min_max(ccs_neigh, nnz, &ccs_neigh_min, NULL, &ccs_neigh_max, NULL);
	
	int *ccs_neigh_hist = (int*) calloc((int)ccs_neigh_max+1, sizeof(int));
	for(int i=0;i<nnz;i++)
		ccs_neigh_hist[ccs_neigh[i]]++;
	for (int i = 0; i < (int)ccs_neigh_max+1; ++i)
		printf("%d: %d (%.4f%%)\n", i, ccs_neigh_hist[i], ccs_neigh_hist[i]*100.0/nnz);
	free(ccs_neigh_hist);
	
	snprintf(buf, buf_n, "%s_ccs_neigh_distribution.png", title_base);
	snprintf(buf_title, buf_n, "%s: ccs_neigh distribution", title_base);
	figure_simple_plot(buf, num_pixels_x, num_pixels_y, (NULL, ccs_neigh, NULL, nnz, 0),
		if (enable_legend)
			figure_enable_legend(_fig);
		figure_set_title(_fig, buf_title);
		figure_series_type_histogram(_s, 0, NULL, 1);
		figure_series_type_barplot(_s);
	);

	free(ccs_neigh);
}

//==========================================================================================================================================
//= Includes Undefs
//==========================================================================================================================================


#include "sort/samplesort/samplesort_gen_pop.h"
#include "sort/quicksort/quicksort_gen_pop.h"
#include "functools/functools_gen_pop.h"

