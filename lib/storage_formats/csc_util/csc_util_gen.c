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


// Expand col indexes.
#undef  csc_col_indexes
#define csc_col_indexes  CSC_UTIL_GEN_EXPAND(csc_col_indexes)
void
csc_col_indexes(__attribute__((unused)) _TYPE_I * row_idx, _TYPE_I * col_ptr, __attribute__((unused)) long m, long n, long nnz,
		_TYPE_I ** col_idx_out)
{
	_TYPE_I * col_idx;
	col_idx = (typeof(col_idx)) malloc(nnz * sizeof(*col_idx));
	#pragma omp parallel
	{
		long i, j;
		#pragma omp for
		for (j=0;j<n;j++){
			for (i=col_ptr[j];i<col_ptr[j+1];i++)
				col_idx[i] = j;
		}
	}
	*col_idx_out = col_idx;
}

//==========================================================================================================================================
//= Matrix transformations
//==========================================================================================================================================

#undef  csc_extract_col_cross
#define csc_extract_col_cross  CSC_UTIL_GEN_EXPAND(csc_extract_col_cross)
void
csc_extract_col_cross(_TYPE_I *row_idx, _TYPE_I *col_ptr, __attribute__((unused)) _TYPE_V *val, int m, int n, int nnz, int window_width, 
					  int *num_windows_out, float **col_cross_out, _TYPE_I **cc_r_out, _TYPE_I **cc_c_out, float **cc_v_out)
{
	int num_windows = (m-1 + window_width) / window_width;
	printf("m = %d, n = %d, num_windows = %d\n", m, n, num_windows);
	long unsigned cc_elements = num_windows * n;
	float * col_cross = (typeof(col_cross)) calloc(cc_elements, sizeof(*col_cross));
	double col_cross_mem_foot = (n * 1.0 * num_windows * sizeof(*col_cross))/(1024*1024*1.0);
	printf("memory footprint of col_cross = %.2lf MB\n", col_cross_mem_foot);

	int num_threads = omp_get_max_threads();

	_TYPE_I * thread_j_s = (_TYPE_I *) malloc(num_threads * sizeof(*thread_j_s));
	_TYPE_I * thread_j_e = (_TYPE_I *) malloc(num_threads * sizeof(*thread_j_e));
	_TYPE_I * thread_i_s = (_TYPE_I *) malloc(num_threads * sizeof(*thread_i_s));
	_TYPE_I * thread_i_e = (_TYPE_I *) malloc(num_threads * sizeof(*thread_i_e));

	double time_col_cross = time_it(1,
	_Pragma("omp parallel")
	{
		int tnum = omp_get_thread_num();
		loop_partitioner_balance_iterations(num_threads, tnum, 0, nnz, &thread_i_s[tnum], &thread_i_e[tnum]);
		long lower_boundary;
		binary_search(col_ptr, 0, n, thread_i_s[tnum], &lower_boundary, NULL);           // Index boundaries are inclusive.
		thread_j_s[tnum] = lower_boundary;
		_Pragma("omp barrier")
		if (tnum == num_threads - 1)   // If we calculate each thread's boundaries individually some empty rows might be unassigned.
			thread_j_e[tnum] = n;
		else
			thread_j_e[tnum] = thread_j_s[tnum+1] + 1;

		// _Pragma("omp single"){
		// 	for(int tnum = 0; tnum < num_threads; tnum++)
		// 		printf("tnum = %2d\trows = %d\t\tnonzeros = %d\n", tnum, thread_j_e[tnum] - thread_j_s[tnum], thread_i_e[tnum] - thread_i_s[tnum]);
		// }

		for(_TYPE_I i=thread_j_s[tnum]; i<thread_j_e[tnum]; i++){
			// printf("tnum=%d\ti=%d\n", tnum, i);
			for(_TYPE_I j=col_ptr[i]; j<col_ptr[i+1]; j++){
				_TYPE_I rw_loc = row_idx[j] / window_width; // row window location
				long unsigned cc_ind = i * num_windows + rw_loc;
				// printf("tnum = %d, i * num_windows + rw_loc = %lu\n", tnum, i * num_windows + rw_loc);
				col_cross[cc_ind]+=1.0;
			}
		}
	}
	);
	printf("time for col_cross = %lf\n", time_col_cross);

	long unsigned cc_elements_nz = 0;
	double time_reduction = time_it(1,
	_Pragma("omp parallel")
	{
	_Pragma("omp for reduction(+:cc_elements_nz)")
	for(int i=0; i<n; i++){
		for(int j=0; j<num_windows; j++){
			long unsigned cc_ind = i * num_windows + j;
			if(col_cross[cc_ind]!=0) 
				cc_elements_nz++;
		}
	}
	}
	);
	printf("time for col_cross_reduction = %lf\n", time_reduction);
	printf("cc_elements_nz = %lu\n", cc_elements_nz);

	_TYPE_I * cc_r, * cc_c;
	float * cc_v;
	cc_r = (typeof(cc_r)) malloc(cc_elements_nz * sizeof(*cc_r));
	// cc_c = (typeof(cc_c)) malloc(cc_elements_nz * sizeof(*cc_c));
	cc_c = (typeof(cc_c)) malloc((n+1) * sizeof(*cc_c));
	cc_v = (typeof(cc_v)) malloc(cc_elements_nz * sizeof(*cc_v));
	double col_cross_compr_mem_foot = (cc_elements_nz * sizeof(*cc_r) + (n+1) * sizeof(*cc_c) + cc_elements_nz * sizeof(*cc_v))/(1024*1024*1.0);
	printf("memory footprint of col_cross (compressed) = %.2lf MB\n", col_cross_compr_mem_foot);
	// printf("compression ratio = %.2lf%\n", col_cross_compr_mem_foot/col_cross_mem_foot * 100);

	// in order to make it faster, perhaps calculate cc_c beforehand and use the dgal partitioner for cc_r, cc_v
	int cnt=0;
	cc_c[0] = 0;
	double time_final = time_it(1,
	for(int j=0; j<n; j++){
		for(int i=0; i<num_windows; i++){
			long unsigned cc_ind = j * num_windows + i;
			if(col_cross[cc_ind]!=0){
				cc_r[cnt] = i;
				// cc_c[cnt] = j;
				cc_v[cnt] = col_cross[cc_ind] * 1.0;
				cnt++;
			}
			cc_c[j+1] = cnt;
		}
	}
	);
	printf("time_final = %lf\n", time_final);

	free(thread_j_s);
	free(thread_j_e);
	free(thread_i_s);
	free(thread_i_e);

	*num_windows_out = num_windows;
	if (col_cross_out != NULL)
		*col_cross_out = col_cross;
	if (cc_r_out != NULL)
		*cc_r_out = cc_r;
	if (cc_c_out != NULL)
		*cc_c_out = cc_c;
	if (cc_v_out != NULL)
		*cc_v_out = cc_v;
}

//==========================================================================================================================================
//= Ploting
//==========================================================================================================================================


#undef  csc_plot
#define csc_plot  CSC_UTIL_GEN_EXPAND(csc_plot)
void
csc_plot(char * title_base, _TYPE_I * row_idx, _TYPE_I * col_ptr, __attribute__((unused)) _TYPE_V * val, long m, long n, long nnz, int enable_legend)
{
	_TYPE_I * col_idx;
	long num_pixels = 1024;
	long num_pixels_x = (n < 1024) ? n : num_pixels;
	long num_pixels_y = (m < 1024) ? m : num_pixels;
	long buf_n = strlen(title_base) + 1 + 1000;
	char buf[buf_n], buf_title[buf_n];

	csc_col_indexes(row_idx, col_ptr, m, n, nnz, &col_idx);
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
	free(col_idx);
}


//==========================================================================================================================================
//= Includes Undefs
//==========================================================================================================================================


#include "sort/samplesort/samplesort_gen_pop.h"
#include "sort/quicksort/quicksort_gen_pop.h"
#include "functools/functools_gen_pop.h"

