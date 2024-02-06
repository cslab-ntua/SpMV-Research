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
#include "kmeans/kmeans.h"
#include "kmeans/kmeans_char.h"

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
	for (int j = 0; j < num_cols; j++) {
		for (int i = col_ptr[j]; i < col_ptr[j + 1]; i++) {
			fprintf(file, "%d %d %.15g\n", row_idx[i] + 1, j + 1, val[i]);
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


//==========================================================================================================================================
//= Includes Undefs
//==========================================================================================================================================


#include "sort/samplesort/samplesort_gen_pop.h"
#include "sort/quicksort/quicksort_gen_pop.h"
#include "functools/functools_gen_pop.h"

