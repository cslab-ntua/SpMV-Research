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
#include "kmeans.h"
#include "kmeans_char.h"

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

#undef  csc_shuffle_matrix
#define csc_shuffle_matrix  CSC_UTIL_GEN_EXPAND(csc_shuffle_matrix)
void
csc_shuffle_matrix(long n, _TYPE_I *row_idx, _TYPE_I *col_ptr, _TYPE_V *values, _TYPE_I *row_idx_shuffle, _TYPE_I *col_ptr_shuffle, _TYPE_V *values_shuffle)
{
	_TYPE_I i;
	// Create an array of indices representing row order
	_TYPE_I* col_indices = (_TYPE_I*)malloc(sizeof(_TYPE_I) * n);
	
	// parallelize as much as possible (not much needed here though...)
	#pragma omp parallel for
	for (i = 0; i < n; i++)
		col_indices[i] = i;

	// Shuffle the row indices using Fisher-Yates shuffle algorithm
	srand(time(NULL));
	for (i = n - 1; i > 0; i--) {
		_TYPE_I j = rand() % (i + 1);
		_TYPE_I temp = col_indices[i];
		col_indices[i] = col_indices[j];
		col_indices[j] = temp;
	}

	// _TYPE_I start_idx = 0;
	col_ptr_shuffle[0] = 0;

	// cannot parallelize this one, (read from col_ptr_shuffle[new_row], write to col_ptr_shuffle[new_row+1])
	for(i = 0; i < n; i++) {
		_TYPE_I old_row = col_indices[i];
		_TYPE_I new_row = i;
		_TYPE_I row_size = col_ptr[old_row + 1] - col_ptr[old_row];
		_TYPE_I start_idx = col_ptr_shuffle[new_row];
		col_ptr_shuffle[new_row + 1] = start_idx + row_size;
	}

	// that's why i did it in separate loops, can utilize parallelization here!
	// Increased parallelization opportunity here (with the loop partitioner of dgal!) Perhaps I will do it one day...
	#pragma omp parallel for
	for(i = 0; i < n; i++) {
		_TYPE_I old_row = col_indices[i];
		_TYPE_I new_row = i;
		_TYPE_I row_size = col_ptr[old_row + 1] - col_ptr[old_row];
		_TYPE_I start_old_idx = col_ptr[old_row];
		_TYPE_I start_idx     = col_ptr_shuffle[new_row];
		for(_TYPE_I j = 0; j < row_size; j++) {
			_TYPE_I old_idx = start_old_idx + j;
			_TYPE_I new_idx = start_idx + j;
			row_idx_shuffle[new_idx] = row_idx[old_idx];
			values_shuffle[new_idx] = values[old_idx];
		}
		// col_ptr_shuffle[new_row + 1] = start_idx + row_size;
	}
	free(col_indices);
}

/*
#undef  csc_extract_col_cross
#define csc_extract_col_cross  CSC_UTIL_GEN_EXPAND(csc_extract_col_cross)
void
csc_extract_col_cross(_TYPE_I *row_idx, _TYPE_I *col_ptr, __attribute__((unused)) _TYPE_V *val, int m, int n, int nnz, int window_width, 
					  int *num_windows_out, float **col_cross_out, int plot, _TYPE_I **cc_r_out, _TYPE_I **cc_c_out, float **cc_v_out)
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

	double time_col_cross = time_it(1,
	_Pragma("omp parallel")
	{
		int tnum = omp_get_thread_num();
		loop_partitioner_balance_prefix_sums(num_threads, tnum, col_ptr, n, nnz, &thread_j_s[tnum], &thread_j_e[tnum]);
		_Pragma("omp barrier")

		// _Pragma("omp single"){
		// 	for(int tnum = 0; tnum < num_threads; tnum++)
		// 		printf("tnum = %2d\trows = %d\t\tnonzeros = %d\n", tnum, thread_j_e[tnum] - thread_j_s[tnum], col_ptr[thread_j_e[tnum]] - col_ptr[thread_j_s[tnum]]);
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

	if(plot){
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
		if (cc_r_out != NULL)
			*cc_r_out = cc_r;
		if (cc_c_out != NULL)
			*cc_c_out = cc_c;
		if (cc_v_out != NULL)
			*cc_v_out = cc_v;
	}
	free(thread_j_s);
	free(thread_j_e);

	*num_windows_out = num_windows;
	if (col_cross_out != NULL)
		*col_cross_out = col_cross;
}
*/

#undef  csc_extract_col_cross2
#define csc_extract_col_cross2  CSC_UTIL_GEN_EXPAND(csc_extract_col_cross2)
void
csc_extract_col_cross2(_TYPE_I *row_idx, _TYPE_I *col_ptr, __attribute__((unused)) _TYPE_V *val, int m, int n, int nnz, int window_width, 
					   int *num_windows_out, _TYPE_I **cc_r_out, _TYPE_I **cc_c_out, float **cc_v_out)
{
	int num_windows = (m-1 + window_width) / window_width;
	printf("m = %d, n = %d, window_width = %d\n", m, n, window_width);
	int num_threads = omp_get_max_threads();

	_TYPE_I * thread_j_s = (_TYPE_I *) malloc(num_threads * sizeof(*thread_j_s));
	_TYPE_I * thread_j_e = (_TYPE_I *) malloc(num_threads * sizeof(*thread_j_e));
	long unsigned  * thread_cc_elements_nz = (long unsigned  *) malloc(num_threads * sizeof(*thread_cc_elements_nz));
	long unsigned  * cc_c_elem = (long unsigned  *) calloc(n, sizeof(*cc_c_elem));

	_Pragma("omp parallel")
	{
		int tnum = omp_get_thread_num();
		loop_partitioner_balance_prefix_sums(num_threads, tnum, col_ptr, n, nnz, &thread_j_s[tnum], &thread_j_e[tnum]);
		_Pragma("omp barrier")
		thread_cc_elements_nz[tnum] = 0;
		for(_TYPE_I i=thread_j_s[tnum]; i<thread_j_e[tnum]; i++){
			long unsigned * local_col_cross = (typeof(local_col_cross)) calloc(num_windows, sizeof(*local_col_cross));
			for(_TYPE_I j=col_ptr[i]; j<col_ptr[i+1]; j++){
				_TYPE_I rw_loc = row_idx[j] / window_width; // row window location
				local_col_cross[rw_loc] = 1; // if already found in rw_loc, just overwrite it, no problem with that... we just want to identify how many windows for sparse struct later
			}
			for(int k=0;k<num_windows; k++){
				thread_cc_elements_nz[tnum] += local_col_cross[k];
				cc_c_elem[i] += local_col_cross[k];
			}
			free(local_col_cross);
		}
	}
	long unsigned cc_elements_nz = 0;
	_Pragma("omp parallel for reduction(+:cc_elements_nz)")
	for(int k=0;k<num_threads;k++)
		cc_elements_nz += thread_cc_elements_nz[k];

	_TYPE_I * cc_r, * cc_c;
	float * cc_v;
	cc_r = (typeof(cc_r)) malloc(cc_elements_nz * sizeof(*cc_r));
	cc_c = (typeof(cc_c)) malloc((n+1) * sizeof(*cc_c));
	cc_v = (typeof(cc_v)) malloc(cc_elements_nz * sizeof(*cc_v));
	// double col_cross_compr_mem_foot = (cc_elements_nz * sizeof(*cc_r) + (n+1) * sizeof(*cc_c) + cc_elements_nz * sizeof(*cc_v))/(1024*1024*1.0);
	// printf("memory footprint of col_cross (compressed) = %.2lf MB\n", col_cross_compr_mem_foot);

	cc_c[0]=0;
	for(int i=1;i<n+1;i++)
		cc_c[i] = cc_c[i-1]+cc_c_elem[i-1];
	free(cc_c_elem);

	_Pragma("omp parallel")
	{
		int tnum = omp_get_thread_num();
		loop_partitioner_balance_prefix_sums(num_threads, tnum, cc_c, n, cc_c[n], &thread_j_s[tnum], &thread_j_e[tnum]);
		_Pragma("omp barrier")
		// _Pragma("omp single")
		// {
		// 	for(int tnum = 0; tnum < num_threads; tnum++)
		// 		printf("tnum = %2d\tcols = (%d-%d) %d\t\tnonzeros = %d\n", tnum, thread_j_s[tnum], thread_j_e[tnum], thread_j_e[tnum] - thread_j_s[tnum], cc_c[thread_j_e[tnum]] - cc_c[thread_j_s[tnum]]);
		// }
		for(_TYPE_I i=thread_j_s[tnum]; i<thread_j_e[tnum]; i++){
			long unsigned * local_col_cross = (typeof(local_col_cross)) calloc(num_windows, sizeof(*local_col_cross));
			for(_TYPE_I j=col_ptr[i]; j<col_ptr[i+1]; j++){
				_TYPE_I rw_loc = row_idx[j] / window_width; // col window location
				local_col_cross[rw_loc]++; // if already found in rw_loc, just overwrite it, no problem with that... we just want to identify how many windows for sparse struct later
			}
			int col_offset = cc_c[i];
			int cnt = 0;
			for(int k=0;k<num_windows; k++){
				if(local_col_cross[k] != 0){
					cc_r[col_offset + cnt] = k;
					cc_v[col_offset + cnt] = local_col_cross[k];
					cnt++;
				}
			}
			free(local_col_cross);
		}
	}
	*num_windows_out = num_windows;
	if (cc_r_out != NULL)
		*cc_r_out = cc_r;
	if (cc_c_out != NULL)
		*cc_c_out = cc_c;
	if (cc_v_out != NULL)
		*cc_v_out = cc_v;

	free(thread_j_s);
	free(thread_j_e);
}

#undef  csc_extract_col_cross2_batch
#define csc_extract_col_cross2_batch  CSC_UTIL_GEN_EXPAND(csc_extract_col_cross2_batch)
void
csc_extract_col_cross2_batch(_TYPE_I *row_idx, _TYPE_I *col_ptr, __attribute__((unused)) _TYPE_V *val, int m, int n, int nnz, int window_width, int batch, 
							 int *num_windows_out, _TYPE_I **cc_r_out, _TYPE_I **cc_c_out, float **cc_v_out)
{
	int num_windows = (m-1 + window_width) / window_width;
	printf("m = %d, n = %d, window_width = %d, batch = %d\n", m, n, window_width, batch);
	int num_threads = omp_get_max_threads();

	_TYPE_I * thread_j_s = (_TYPE_I *) malloc(num_threads * sizeof(*thread_j_s));
	_TYPE_I * thread_j_e = (_TYPE_I *) malloc(num_threads * sizeof(*thread_j_e));
	long unsigned  * thread_cc_elements_nz = (long unsigned  *) malloc(num_threads * sizeof(*thread_cc_elements_nz));
	int n_batch = (n-1 + batch) / batch; 
	long unsigned  * cc_c_elem = (long unsigned  *) calloc(n_batch, sizeof(*cc_c_elem));

	// batched-cols col_ptr array. it will be used for loop partitioner only, so that cols with same batch-index are not assigned to different threads, causing conflict on rc_r_elem array.
	_TYPE_I * col_ptr_b = (_TYPE_I *) malloc((n_batch+1) * sizeof(*col_ptr_b));
	col_ptr_b[0] = 0;
	_Pragma("omp parallel for")
	for(int i=0;i<n+1;i+=batch)
		col_ptr_b[i/batch] = col_ptr[i];
	col_ptr_b[n_batch] = col_ptr[n];

	_Pragma("omp parallel")
	{
		int tnum = omp_get_thread_num();
		loop_partitioner_balance_prefix_sums(num_threads, tnum, col_ptr_b, n_batch, nnz, &thread_j_s[tnum], &thread_j_e[tnum]);
		_Pragma("omp barrier")
		// _Pragma("omp single")
		// {
		// 	for(int tnum = 0; tnum < num_threads; tnum++)
		// 		printf("tnum = %2d\tcols = (%d-%d) %d\n", tnum, thread_j_s[tnum], thread_j_e[tnum], thread_j_e[tnum] - thread_j_s[tnum]);
		// }
		thread_cc_elements_nz[tnum] = 0;
		for(_TYPE_I i=thread_j_s[tnum]; i<thread_j_e[tnum]; i++){
			long unsigned * local_col_cross = (typeof(local_col_cross)) calloc(num_windows, sizeof(*local_col_cross));
			// printf("i = %d, start = %d, end = %d\n", i, i*batch, ((i+1)*batch < n) ? (i+1)*batch-1 : n-1);
			for(_TYPE_I ii=i*batch; ii < (i+1)*batch && ii < n; ii++){
				for(_TYPE_I j=col_ptr[ii]; j<col_ptr[ii+1]; j++){
					_TYPE_I rw_loc = row_idx[j] / window_width; // row window location
					local_col_cross[rw_loc] = 1; // if already found in rw_loc, just overwrite it, no problem with that... we just want to identify how many windows for sparse struct later
				}
			}
			for(int k=0;k<num_windows; k++){
				thread_cc_elements_nz[tnum] += local_col_cross[k];
				cc_c_elem[i] += local_col_cross[k];
			}
			free(local_col_cross);
		}
	}
	free(col_ptr_b);
	long unsigned cc_elements_nz = 0;
	_Pragma("omp parallel for reduction(+:cc_elements_nz)")
	for(int k=0;k<num_threads;k++)
		cc_elements_nz += thread_cc_elements_nz[k];

	_TYPE_I * cc_r, * cc_c;
	float * cc_v;
	cc_r = (typeof(cc_r)) malloc(cc_elements_nz * sizeof(*cc_r));
	cc_c = (typeof(cc_c)) malloc((n_batch+1) * sizeof(*cc_c));
	cc_v = (typeof(cc_v)) malloc(cc_elements_nz * sizeof(*cc_v));
	// double col_cross_compr_mem_foot = (cc_elements_nz * sizeof(*cc_r) + (n_batch+1) * sizeof(*cc_c) + cc_elements_nz * sizeof(*cc_v))/(1024*1024*1.0);
	// printf("memory footprint of col_cross (compressed) = %.2lf MB\n", col_cross_compr_mem_foot);

	cc_c[0]=0;
	for(int i=1;i<n_batch+1;i++)
		cc_c[i] = cc_c[i-1]+cc_c_elem[i-1];
	free(cc_c_elem);

	_Pragma("omp parallel")
	{
		int tnum = omp_get_thread_num();
		loop_partitioner_balance_prefix_sums(num_threads, tnum, cc_c, n_batch, cc_c[n_batch], &thread_j_s[tnum], &thread_j_e[tnum]);
		_Pragma("omp barrier")
		// _Pragma("omp single")
		// {
		// 	for(int tnum = 0; tnum < num_threads; tnum++)
		// 		printf("tnum = %2d\tcols = (%d-%d) %d\t\tnonzeros = %d\n", tnum, thread_j_s[tnum], thread_j_e[tnum], thread_j_e[tnum] - thread_j_s[tnum], cc_c[thread_j_e[tnum]] - cc_c[thread_j_s[tnum]]);
		// }
		for(_TYPE_I i=thread_j_s[tnum]; i<thread_j_e[tnum]; i++){
			long unsigned * local_col_cross = (typeof(local_col_cross)) calloc(num_windows, sizeof(*local_col_cross));
			for(_TYPE_I ii=i*batch; ii < (i+1)*batch && ii < n; ii++){
				for(_TYPE_I j=col_ptr[ii]; j<col_ptr[ii+1]; j++){
					_TYPE_I rw_loc = row_idx[j] / window_width; // col window location
					local_col_cross[rw_loc]++; // if already found in rw_loc, just overwrite it, no problem with that... we just want to identify how many windows for sparse struct later
				}
			}
			int col_offset = cc_c[i];
			int cnt = 0;
			for(int k=0;k<num_windows; k++){
				if(local_col_cross[k] != 0){
					cc_r[col_offset + cnt] = k;
					cc_v[col_offset + cnt] = local_col_cross[k];
					cnt++;
				}
			}
			free(local_col_cross);
		}
	}
	*num_windows_out = num_windows;
	if (cc_r_out != NULL)
		*cc_r_out = cc_r;
	if (cc_c_out != NULL)
		*cc_c_out = cc_c;
	if (cc_v_out != NULL)
		*cc_v_out = cc_v;

	free(thread_j_s);
	free(thread_j_e);
}

#undef  csc_extract_col_cross_char2
#define csc_extract_col_cross_char2  CSC_UTIL_GEN_EXPAND(csc_extract_col_cross_char2)
void
csc_extract_col_cross_char2(_TYPE_I *row_idx, _TYPE_I *col_ptr, __attribute__((unused)) _TYPE_V *val, int m, int n, int nnz, int window_width, 
							int *num_windows_out, _TYPE_I **cc_r_out, _TYPE_I **cc_c_out, unsigned char **cc_v_out)
{
	int num_windows = (m-1 + window_width) / window_width;
	printf("m = %d, n = %d, window_width = %d\n", m, n, window_width);
	int num_threads = omp_get_max_threads();

	_TYPE_I * thread_j_s = (_TYPE_I *) malloc(num_threads * sizeof(*thread_j_s));
	_TYPE_I * thread_j_e = (_TYPE_I *) malloc(num_threads * sizeof(*thread_j_e));
	long unsigned  * thread_cc_elements_nz = (long unsigned  *) malloc(num_threads * sizeof(*thread_cc_elements_nz));
	long unsigned  * cc_c_elem = (long unsigned  *) calloc(n, sizeof(*cc_c_elem));

	// double time_col_cross = time_it(1,
	_Pragma("omp parallel")
	{
		int tnum = omp_get_thread_num();
		loop_partitioner_balance_prefix_sums(num_threads, tnum, col_ptr, n, nnz, &thread_j_s[tnum], &thread_j_e[tnum]);
		_Pragma("omp barrier")
		thread_cc_elements_nz[tnum] = 0;
		for(_TYPE_I i=thread_j_s[tnum]; i<thread_j_e[tnum]; i++){
			long unsigned * local_col_cross = (typeof(local_col_cross)) calloc(num_windows, sizeof(*local_col_cross));
			for(_TYPE_I j=col_ptr[i]; j<col_ptr[i+1]; j++){
				_TYPE_I rw_loc = row_idx[j] / window_width; // row window location
				local_col_cross[rw_loc] = 1; // if already found in rw_loc, just overwrite it, no problem with that... we just want to identify how many windows for sparse struct later
			}
			for(int k=0;k<num_windows; k++){
				thread_cc_elements_nz[tnum] += local_col_cross[k];
				cc_c_elem[i] += local_col_cross[k];
			}
			free(local_col_cross);
		}
	}
	// );
	// printf("time for col_cross = %lf\n", time_col_cross);
	long unsigned cc_elements_nz = 0;
	_Pragma("omp parallel for reduction(+:cc_elements_nz)")
	for(int k=0;k<num_threads;k++){
		// printf("thread_cc_elements_nz[%d] = %lu\n", k, thread_cc_elements_nz[k]);
		cc_elements_nz += thread_cc_elements_nz[k];
	}
	printf("cc_elements_nz = %lu (%.3f %)\n", cc_elements_nz, ((cc_elements_nz*1.0)/nnz)*100);

	_TYPE_I * cc_r, * cc_c;
	unsigned char * cc_v;
	cc_r = (typeof(cc_r)) malloc(cc_elements_nz * sizeof(*cc_r));
	cc_c = (typeof(cc_c)) malloc((n+1) * sizeof(*cc_c));
	cc_v = (typeof(cc_v)) malloc(cc_elements_nz * sizeof(*cc_v));
	// double col_cross_compr_mem_foot = (cc_elements_nz * sizeof(*cc_r) + (n+1) * sizeof(*cc_c) + cc_elements_nz * sizeof(*cc_v))/(1024*1024*1.0);
	// printf("memory footprint of col_cross (compressed) = %.2lf MB\n", col_cross_compr_mem_foot);

	cc_c[0]=0;
	for(int i=1;i<n+1;i++)
		cc_c[i] = cc_c[i-1]+cc_c_elem[i-1];
	free(cc_c_elem);

	// double time_final = time_it(1,
	_Pragma("omp parallel")
	{
		int tnum = omp_get_thread_num();
		loop_partitioner_balance_prefix_sums(num_threads, tnum, cc_c, n, cc_c[n], &thread_j_s[tnum], &thread_j_e[tnum]);
		_Pragma("omp barrier")
		// _Pragma("omp single")
		// {
		// 	for(int tnum = 0; tnum < num_threads; tnum++)
		// 		printf("tnum = %2d\tcols = (%d-%d) %d\t\tnonzeros = %d\n", tnum, thread_j_s[tnum], thread_j_e[tnum], thread_j_e[tnum] - thread_j_s[tnum], cc_c[thread_j_e[tnum]] - cc_c[thread_j_s[tnum]]);
		// }
		for(_TYPE_I i=thread_j_s[tnum]; i<thread_j_e[tnum]; i++){
			long unsigned * local_col_cross = (typeof(local_col_cross)) calloc(num_windows, sizeof(*local_col_cross));
			for(_TYPE_I j=col_ptr[i]; j<col_ptr[i+1]; j++){
				_TYPE_I rw_loc = row_idx[j] / window_width; // col window location
				local_col_cross[rw_loc]++; // if already found in rw_loc, just overwrite it, no problem with that... we just want to identify how many windows for sparse struct later
			}
			int col_offset = cc_c[i];
			int cnt = 0;
			for(int k=0;k<num_windows; k++){
				if(local_col_cross[k] != 0){
					cc_r[col_offset + cnt] = k;
					cc_v[col_offset + cnt] = 1;
					cnt++;
				}
			}
			free(local_col_cross);
		}
	}
	// );
	// printf("time_final = %lf\n", time_final);
	*num_windows_out = num_windows;
	if (cc_r_out != NULL)
		*cc_r_out = cc_r;
	if (cc_c_out != NULL)
		*cc_c_out = cc_c;
	if (cc_v_out != NULL)
		*cc_v_out = cc_v;

	free(thread_j_s);
	free(thread_j_e);
}

#undef  csc_extract_col_cross_char2_batch
#define csc_extract_col_cross_char2_batch  CSC_UTIL_GEN_EXPAND(csc_extract_col_cross_char2_batch)
void
csc_extract_col_cross_char2_batch(_TYPE_I *row_idx, _TYPE_I *col_ptr, __attribute__((unused)) _TYPE_V *val, int m, int n, int nnz, int window_width, int batch, 
								  int *num_windows_out, _TYPE_I **cc_r_out, _TYPE_I **cc_c_out, unsigned char **cc_v_out)
{
	int num_windows = (m-1 + window_width) / window_width;
	printf("m = %d, n = %d, window_width = %d, batch = %d\n", m, n, window_width, batch);
	int num_threads = omp_get_max_threads();

	_TYPE_I * thread_j_s = (_TYPE_I *) malloc(num_threads * sizeof(*thread_j_s));
	_TYPE_I * thread_j_e = (_TYPE_I *) malloc(num_threads * sizeof(*thread_j_e));
	long unsigned  * thread_cc_elements_nz = (long unsigned  *) malloc(num_threads * sizeof(*thread_cc_elements_nz));
	int n_batch = (n-1 + batch) / batch; 
	long unsigned  * cc_c_elem = (long unsigned  *) calloc(n_batch, sizeof(*cc_c_elem));

	// batched-cols col_ptr array. it will be used for loop partitioner only, so that cols with same batch-index are not assigned to different threads, causing conflict on rc_r_elem array.
	_TYPE_I * col_ptr_b = (_TYPE_I *) malloc((n_batch+1) * sizeof(*col_ptr_b));
	col_ptr_b[0] = 0;
	_Pragma("omp parallel for")
	for(int i=0;i<n+1;i+=batch)
		col_ptr_b[i/batch] = col_ptr[i];
	col_ptr_b[n_batch] = col_ptr[n];

	_Pragma("omp parallel")
	{
		int tnum = omp_get_thread_num();
		loop_partitioner_balance_prefix_sums(num_threads, tnum, col_ptr_b, n_batch, nnz, &thread_j_s[tnum], &thread_j_e[tnum]);
		_Pragma("omp barrier")
		// _Pragma("omp single")
		// {
		// 	for(int tnum = 0; tnum < num_threads; tnum++)
		// 		printf("tnum = %2d\tcols = (%d-%d) %d\n", tnum, thread_j_s[tnum], thread_j_e[tnum], thread_j_e[tnum] - thread_j_s[tnum]);
		// }
		thread_cc_elements_nz[tnum] = 0;
		for(_TYPE_I i=thread_j_s[tnum]; i<thread_j_e[tnum]; i++){
			long unsigned * local_col_cross = (typeof(local_col_cross)) calloc(num_windows, sizeof(*local_col_cross));
			// printf("i = %d, start = %d, end = %d\n", i, i*batch, ((i+1)*batch < n) ? (i+1)*batch-1 : n-1);
			for(_TYPE_I ii=i*batch; ii < (i+1)*batch && ii < n; ii++){
				for(_TYPE_I j=col_ptr[ii]; j<col_ptr[ii+1]; j++){
					_TYPE_I rw_loc = row_idx[j] / window_width; // row window location
					local_col_cross[rw_loc] = 1; // if already found in rw_loc, just overwrite it, no problem with that... we just want to identify how many windows for sparse struct later
				}
			}
			for(int k=0;k<num_windows; k++){
				thread_cc_elements_nz[tnum] += local_col_cross[k];
				cc_c_elem[i] += local_col_cross[k];
			}
			free(local_col_cross);
		}
	}
	free(col_ptr_b);
	long unsigned cc_elements_nz = 0;
	_Pragma("omp parallel for reduction(+:cc_elements_nz)")
	for(int k=0;k<num_threads;k++)
		cc_elements_nz += thread_cc_elements_nz[k];

	_TYPE_I * cc_r, * cc_c;
	unsigned char * cc_v;
	cc_r = (typeof(cc_r)) malloc(cc_elements_nz * sizeof(*cc_r));
	cc_c = (typeof(cc_c)) malloc((n_batch+1) * sizeof(*cc_c));
	cc_v = (typeof(cc_v)) malloc(cc_elements_nz * sizeof(*cc_v));
	// double col_cross_compr_mem_foot = (cc_elements_nz * sizeof(*cc_r) + (n_batch+1) * sizeof(*cc_c) + cc_elements_nz * sizeof(*cc_v))/(1024*1024*1.0);
	// printf("memory footprint of col_cross (compressed) = %.2lf MB\n", col_cross_compr_mem_foot);

	cc_c[0]=0;
	for(int i=1;i<n_batch+1;i++)
		cc_c[i] = cc_c[i-1]+cc_c_elem[i-1];
	free(cc_c_elem);

	_Pragma("omp parallel")
	{
		int tnum = omp_get_thread_num();
		loop_partitioner_balance_prefix_sums(num_threads, tnum, cc_c, n_batch, cc_c[n_batch], &thread_j_s[tnum], &thread_j_e[tnum]);
		_Pragma("omp barrier")
		// _Pragma("omp single")
		// {
		// 	for(int tnum = 0; tnum < num_threads; tnum++)
		// 		printf("tnum = %2d\tcols = (%d-%d) %d\t\tnonzeros = %d\n", tnum, thread_j_s[tnum], thread_j_e[tnum], thread_j_e[tnum] - thread_j_s[tnum], cc_c[thread_j_e[tnum]] - cc_c[thread_j_s[tnum]]);
		// }
		for(_TYPE_I i=thread_j_s[tnum]; i<thread_j_e[tnum]; i++){
			long unsigned * local_col_cross = (typeof(local_col_cross)) calloc(num_windows, sizeof(*local_col_cross));
			for(_TYPE_I ii=i*batch; ii < (i+1)*batch && ii < n; ii++){
				for(_TYPE_I j=col_ptr[ii]; j<col_ptr[ii+1]; j++){
					_TYPE_I rw_loc = row_idx[j] / window_width; // col window location
					local_col_cross[rw_loc]++; // if already found in rw_loc, just overwrite it, no problem with that... we just want to identify how many windows for sparse struct later
				}
			}
			int col_offset = cc_c[i];
			int cnt = 0;
			for(int k=0;k<num_windows; k++){
				if(local_col_cross[k] != 0){
					cc_r[col_offset + cnt] = k;
					cc_v[col_offset + cnt] = 1; // local_col_cross[k]; // let's check how it performs when using - update: shit
					cnt++;
				}
			}
			free(local_col_cross);
		}
	}
	*num_windows_out = num_windows;
	if (cc_r_out != NULL)
		*cc_r_out = cc_r;
	if (cc_c_out != NULL)
		*cc_c_out = cc_c;
	if (cc_v_out != NULL)
		*cc_v_out = cc_v;

	free(thread_j_s);
	free(thread_j_e);
}

typedef struct {
	int col;
	int family_id;
} ColFamily;

int compareColFamily(const void *a, const void *b) {
	return ((ColFamily *)a)->family_id - ((ColFamily *)b)->family_id;
}

#undef  csc_reorder_matrix_by_col
#define csc_reorder_matrix_by_col  CSC_UTIL_GEN_EXPAND(csc_reorder_matrix_by_col)
void
csc_reorder_matrix_by_col(__attribute__((unused)) int m, int n, __attribute__((unused)) int nnz, _TYPE_I * row_idx, _TYPE_I * col_ptr, _TYPE_V * val, 
						  int * membership, _TYPE_I * row_idx_reorder, _TYPE_I * col_ptr_reorder, _TYPE_V * val_reorder, _TYPE_I * original_col_positions)
{
	// Create an auxiliary array to store col indices and family IDs
	ColFamily* col_families = (ColFamily*)malloc(n * sizeof(ColFamily));

	for (int i = 0; i < n; i++) {
		col_families[i].col = i;
		col_families[i].family_id = membership[i];
	}

	// Sort the auxiliary array based on family IDs
	qsort(col_families, n, sizeof(ColFamily), compareColFamily);

	col_ptr_reorder[0] = 0;
	int cnt = 0;

	for (int i = 0; i < n; i++) {
		int original_col = col_families[i].col;
		original_col_positions[i] = original_col;
		// if(i<10) printf("col: %d, membership; %d\n", col_families[i].col, col_families[i].family_id);
		
		for (int j = col_ptr[original_col]; j < col_ptr[original_col + 1]; j++) {
		row_idx_reorder[cnt] = row_idx[j];
		val_reorder[cnt] = val[j];
		cnt++;
		}
		col_ptr_reorder[i+1] = cnt;
	}
	free(col_families);
}

#undef  csc_reorder_matrix_by_col_batch
#define csc_reorder_matrix_by_col_batch  CSC_UTIL_GEN_EXPAND(csc_reorder_matrix_by_col_batch)
void
csc_reorder_matrix_by_col_batch(__attribute__((unused)) int m, int n, __attribute__((unused)) int nnz, int batch, _TYPE_I * row_idx, _TYPE_I * col_ptr, _TYPE_V * val, 
								int * membership, _TYPE_I * row_idx_reorder, _TYPE_I * col_ptr_reorder, _TYPE_V * val_reorder, _TYPE_I * original_col_positions)
{
	int n_batch = (n-1 + batch) / batch; 
	// Create an auxiliary array to store col indices and family IDs
	ColFamily* col_families = (ColFamily*)malloc(n_batch * sizeof(ColFamily));

	for (int i = 0; i < n_batch; i++) {
		col_families[i].col = i;
		col_families[i].family_id = membership[i];
	}

	// Sort the auxiliary array based on family IDs
	qsort(col_families, n_batch, sizeof(ColFamily), compareColFamily);

	col_ptr_reorder[0] = 0;
	int cnt = 0, cnt2 = 0;

	for (int ii = 0; ii < n_batch; ii++) {
		int original_col = col_families[ii].col;
		// printf("ii: %d, row: %d ( %d - %d ), membership: %d\n", ii, original_col, original_col*batch, ((original_col+1)*batch  < n) ? (original_col+1)*batch-1 : n-1, col_families[ii].family_id);		
		for(int i = original_col*batch; i < (original_col+1)*batch && i < n; i++)
		{
			for (int j = col_ptr[i]; j < col_ptr[i + 1]; j++) {
				row_idx_reorder[cnt] = row_idx[j];
				val_reorder[cnt] = val[j];
				cnt++;
			}
			col_ptr_reorder[cnt2+1] = cnt;
			original_col_positions[cnt2] = i;
			// printf("finished with col = %d, nnz = %d\n", cnt2, cnt);
			cnt2++;
		}
	}
	free(col_families);
}

/*****************************************/
/* csc_kmeans_reorder_col util functions */
void random_selection_csc(_TYPE_I * start_cc_r, _TYPE_I * start_cc_c, float * start_cc_v, float * target_array, int numObjs, int numClusters, int numCoords)
{
	int * indices = (int *) malloc(numObjs * sizeof(int));
	for(int i = 0; i < numObjs; i++)
		indices[i] = i;
	srand(14);
	for(int i = 0; i < numClusters ; i++) {
		int k = rand() % (numObjs - i);
		float *start_array = (typeof(start_array)) calloc(numCoords, sizeof(*start_array));
		for(int j = start_cc_c[indices[k]]; j < start_cc_c[indices[k]+1]; j++)
			start_array[start_cc_r[j]] = start_cc_v[j];

		for(int j = 0; j < numCoords; j++)
			target_array[i * numCoords + j] = start_array[j];
			// target_array[i * numCoords + j] = start_array[indices[k] * numCoords + j];
		indices[k] = indices[numCoords - i - 1];
		free(start_array);
	}
	free(indices);
	// for (int i=0; i<numClusters; i++){
	// 	printf("cluster[%3d] = [ ", i);
	// 	for (int j=0; j<numCoords; j++)
	// 		printf("%2.0f ", target_array[i*numCoords + j]);
	// 	printf("]\n");
	// }
}

#undef  csc_kmeans_reorder_col
#define csc_kmeans_reorder_col  CSC_UTIL_GEN_EXPAND(csc_kmeans_reorder_col)
void 
csc_kmeans_reorder_col(_TYPE_I * row_idx, _TYPE_I * col_ptr, _TYPE_V * val, 
					   _TYPE_I * row_idx_reorder_c, _TYPE_I * col_ptr_reorder_c, _TYPE_V * val_reorder_c, _TYPE_I ** original_col_positions,
					   int m, int n, int nnz, 
					   int numClusters, float threshold, int loop_threshold, int num_windows, _TYPE_I * cc_r, _TYPE_I * cc_c, float * cc_v)
{
	int numObjs = n;
	int numCoords = num_windows;
	int * membership = (typeof(membership)) malloc(numObjs * sizeof(*membership));

	float * clusters = (typeof(clusters)) malloc(numClusters * numCoords * sizeof(*clusters));
	random_selection_csc(cc_r, cc_c, cc_v, clusters, numObjs, numClusters, numCoords);

	int type = 0; // cosine similarity
	double time_kmeans2_csc = time_it(1,
	// kmeans(row_cross, numCoords, numObjs, numClusters, threshold, loop_threshold, membership, clusters, type);
	kmeans2_csc(cc_r, cc_c, cc_v, numCoords, numObjs, numClusters, threshold, loop_threshold, membership, clusters, type);
	);
	printf("\ntime_kmeans2_csc = %lf (numObjs = %d, numCoords = %d, numClusters = %d)\n", time_kmeans2_csc, numObjs, numCoords, numClusters);

	// for (int i=0; i<numClusters; i++){
	// 	printf("cluster[%3d] = [ ", i);
	// 	for (int j=0; j<numCoords; j++)
	// 		printf("%2.0f ", clusters[i*numCoords + j]);
	// 	printf("]\n");
	// }

	// char * binary_string[numClusters];
	binary_string_with_id clusters_binary[numClusters];

	for(int i = 0; i < numClusters; i++){
		clusters_binary[i].binary_string = convert_float_to_binary_string(&clusters[i * numCoords], numCoords);
		clusters_binary[i].id = i;
		// printf("cluster_b[%3d]: %s\n", i, clusters_binary[i].binary_string);
	}
	free(clusters);

	// Sort the array of binary strings
	qsort(clusters_binary, numClusters, sizeof(binary_string_with_id), compare_binary_strings);

	// printf("after sorting...\n");
	// for (int i = 0; i < numClusters; i++)
	// 	printf("cluster_b[%3d]: %s (%d)\n", i, clusters_binary[i].binary_string, clusters_binary[i].id);

	for (int i = 0; i < numClusters; i++)
		free(clusters_binary[i].binary_string);

	int * ordered_membership = (typeof(ordered_membership)) malloc(numClusters * sizeof(*ordered_membership));
	for (int i = 0; i < numClusters; i++)
		ordered_membership[clusters_binary[i].id] = i;

	for(int i = 0; i < numObjs; i++)
		membership[i] = ordered_membership[membership[i]];
	free(ordered_membership);

	*original_col_positions = (typeof(*original_col_positions)) malloc(n * sizeof(**original_col_positions));
	double time_col_reorder = time_it(1,
	csc_reorder_matrix_by_col(m, n, nnz, row_idx, col_ptr, val, membership, row_idx_reorder_c, col_ptr_reorder_c, val_reorder_c, *original_col_positions);
	);
	printf("time_reorder_by_col = %lf\n", time_col_reorder);

	free(membership);
}

#undef  csc_kmeans_reorder_col_batch
#define csc_kmeans_reorder_col_batch  CSC_UTIL_GEN_EXPAND(csc_kmeans_reorder_col_batch)
void 
csc_kmeans_reorder_col_batch(_TYPE_I * row_idx, _TYPE_I * col_ptr, _TYPE_V * val, 
							 _TYPE_I * row_idx_reorder_c, _TYPE_I * col_ptr_reorder_c, _TYPE_V * val_reorder_c, _TYPE_I ** original_col_positions,
							 int m, int n, int nnz, int batch, 
							 int numClusters, float threshold, int loop_threshold, int num_windows, _TYPE_I * cc_r, _TYPE_I * cc_c, float * cc_v)
{
	int n_batch = (n-1 + batch) / batch; 
	int numObjs = n_batch;
	int numCoords = num_windows;
	int * membership = (typeof(membership)) malloc(numObjs * sizeof(*membership));

	float * clusters = (typeof(clusters)) malloc(numClusters * numCoords * sizeof(*clusters));
	random_selection_csc(cc_r, cc_c, cc_v, clusters, numObjs, numClusters, numCoords);

	int type = 0; // cosine similarity
	double time_kmeans2_csc = time_it(1,
	// kmeans(row_cross, numCoords, numObjs, numClusters, threshold, loop_threshold, membership, clusters, type);
	kmeans2_csc(cc_r, cc_c, cc_v, numCoords, numObjs, numClusters, threshold, loop_threshold, membership, clusters, type);
	);
	printf("\ntime_kmeans2_csc = %lf (numObjs = %d, numCoords = %d, numClusters = %d)\n", time_kmeans2_csc, numObjs, numCoords, numClusters);

	// for (int i=0; i<numClusters; i++){
	// 	printf("cluster[%3d] = [ ", i);
	// 	for (int j=0; j<numCoords; j++)
	// 		printf("%2.0f ", clusters[i*numCoords + j]);
	// 	printf("]\n");
	// }

	// char * binary_string[numClusters];
	binary_string_with_id clusters_binary[numClusters];

	for(int i = 0; i < numClusters; i++){
		clusters_binary[i].binary_string = convert_float_to_binary_string(&clusters[i * numCoords], numCoords);
		clusters_binary[i].id = i;
		// printf("cluster_b[%3d]: %s\n", i, clusters_binary[i].binary_string);
	}
	free(clusters);

	// Sort the array of binary strings
	qsort(clusters_binary, numClusters, sizeof(binary_string_with_id), compare_binary_strings);

	// printf("after sorting...\n");
	// for (int i = 0; i < numClusters; i++)
	// 	printf("cluster_b[%3d]: %s (%d)\n", i, clusters_binary[i].binary_string, clusters_binary[i].id);

	for (int i = 0; i < numClusters; i++)
		free(clusters_binary[i].binary_string);

	int * ordered_membership = (typeof(ordered_membership)) malloc(numClusters * sizeof(*ordered_membership));
	for (int i = 0; i < numClusters; i++)
		ordered_membership[clusters_binary[i].id] = i;

	for(int i = 0; i < numObjs; i++)
		membership[i] = ordered_membership[membership[i]];
	free(ordered_membership);

	*original_col_positions = (typeof(*original_col_positions)) malloc(n * sizeof(**original_col_positions));
	double time_col_reorder = time_it(1,
	csc_reorder_matrix_by_col_batch(m, n, nnz, batch, row_idx, col_ptr, val, membership, row_idx_reorder_c, col_ptr_reorder_c, val_reorder_c, *original_col_positions);
	);
	printf("time_reorder_by_col = %lf\n", time_col_reorder);

	free(membership);
}

/* csc_kmeans_reorder_col util functions */
void random_selection_char_csc(_TYPE_I * start_cc_r, _TYPE_I * start_cc_c, unsigned char * start_cc_v, unsigned char * target_array, int numObjs, int numClusters, int numCoords)
{
	int * indices = (int *) malloc(numObjs * sizeof(int));
	for(int i = 0; i < numObjs; i++)
		indices[i] = i;
	srand(14);
	for(int i = 0; i < numClusters ; i++) {
		int k = rand() % (numObjs - i);
		unsigned char *start_array = (typeof(start_array)) calloc(numCoords, sizeof(*start_array));
		for(int j = start_cc_c[indices[k]]; j < start_cc_c[indices[k]+1]; j++)
			start_array[start_cc_r[j]] = start_cc_v[j];

		for(int j = 0; j < numCoords; j++)
			target_array[i * numCoords + j] = start_array[j];
			// target_array[i * numCoords + j] = start_array[indices[k] * numCoords + j];
		indices[k] = indices[numCoords - i - 1];
		free(start_array);
	}
	free(indices);
	// for (int i=0; i<numClusters; i++){
	// 	printf("cluster[%3d] = [ ", i);
	// 	for (int j=0; j<numCoords; j++)
	// 		printf("%2.0f ", target_array[i*numCoords + j]);
	// 	printf("]\n");
	// }
}

#undef  csc_kmeans_char_reorder_col
#define csc_kmeans_char_reorder_col  CSC_UTIL_GEN_EXPAND(csc_kmeans_char_reorder_col)
void 
csc_kmeans_char_reorder_col(_TYPE_I * row_idx, _TYPE_I * col_ptr, _TYPE_V * val, 
							_TYPE_I * row_idx_reorder_c, _TYPE_I * col_ptr_reorder_c, _TYPE_V * val_reorder_c, _TYPE_I ** original_col_positions,
							int m, int n, int nnz, 
							int numClusters, float threshold, int loop_threshold, int num_windows, _TYPE_I * cc_r, _TYPE_I * cc_c, unsigned char * cc_v)
{
	int numObjs = n;
	int numCoords = num_windows;
	int * membership = (typeof(membership)) malloc(numObjs * sizeof(*membership));

	unsigned char * clusters = (typeof(clusters)) malloc(numClusters * numCoords * sizeof(*clusters));
	random_selection_char_csc(cc_r, cc_c, cc_v, clusters, numObjs, numClusters, numCoords);

	int type = 0; // cosine similarity
	double time_kmeans2_csc = time_it(1,
	// kmeans(row_cross, numCoords, numObjs, numClusters, threshold, loop_threshold, membership, clusters, type);
	kmeans_char2_csc(cc_r, cc_c, cc_v, numCoords, numObjs, numClusters, threshold, loop_threshold, membership, clusters, type);
	);
	printf("\ntime_kmeans2_csc = %lf (numObjs = %d, numCoords = %d, numClusters = %d)\n", time_kmeans2_csc, numObjs, numCoords, numClusters);

	// for (int i=0; i<numClusters; i++){
	// 	printf("cluster[%3d] = [ ", i);
	// 	for (int j=0; j<numCoords; j++)
	// 		printf("%2.0f ", clusters[i*numCoords + j]);
	// 	printf("]\n");
	// }

	// char * binary_string[numClusters];
	binary_string_with_id clusters_binary[numClusters];

	for(int i = 0; i < numClusters; i++){
		clusters_binary[i].binary_string = convert_unsigned_char_to_binary_string(&clusters[i * numCoords], numCoords);
		clusters_binary[i].id = i;
		// printf("cluster_b[%3d]: %s\n", i, clusters_binary[i].binary_string);
	}
	free(clusters);

	// Sort the array of binary strings
	qsort(clusters_binary, numClusters, sizeof(binary_string_with_id), compare_binary_strings);

	// printf("after sorting...\n");
	// for (int i = 0; i < numClusters; i++)
	// 	printf("cluster_b[%3d]: %s (%d)\n", i, clusters_binary[i].binary_string, clusters_binary[i].id);

	for (int i = 0; i < numClusters; i++)
		free(clusters_binary[i].binary_string);

	int * ordered_membership = (typeof(ordered_membership)) malloc(numClusters * sizeof(*ordered_membership));
	for (int i = 0; i < numClusters; i++)
		ordered_membership[clusters_binary[i].id] = i;

	for(int i = 0; i < numObjs; i++)
		membership[i] = ordered_membership[membership[i]];
	free(ordered_membership);

	*original_col_positions = (typeof(*original_col_positions)) malloc(n * sizeof(**original_col_positions));
	double time_col_reorder = time_it(1,
	csc_reorder_matrix_by_col(m, n, nnz, row_idx, col_ptr, val, membership, row_idx_reorder_c, col_ptr_reorder_c, val_reorder_c, *original_col_positions);
	);
	printf("time_reorder_by_col = %lf\n", time_col_reorder);

	free(membership);
}

#undef  csc_kmeans_char_reorder_col_batch
#define csc_kmeans_char_reorder_col_batch  CSC_UTIL_GEN_EXPAND(csc_kmeans_char_reorder_col_batch)
void 
csc_kmeans_char_reorder_col_batch(_TYPE_I * row_idx, _TYPE_I * col_ptr, _TYPE_V * val, 
								  _TYPE_I * row_idx_reorder_c, _TYPE_I * col_ptr_reorder_c, _TYPE_V * val_reorder_c, _TYPE_I ** original_col_positions,
								  int m, int n, int nnz, int batch, 
								  int numClusters, float threshold, int loop_threshold, int num_windows, _TYPE_I * cc_r, _TYPE_I * cc_c, unsigned char * cc_v)
{
	int n_batch = (n-1 + batch) / batch; 
	int numObjs = n_batch;
	int numCoords = num_windows;
	int * membership = (typeof(membership)) malloc(numObjs * sizeof(*membership));

	unsigned char * clusters = (typeof(clusters)) malloc(numClusters * numCoords * sizeof(*clusters));
	random_selection_char_csc(cc_r, cc_c, cc_v, clusters, numObjs, numClusters, numCoords);

	int type = 0; // cosine similarity
	double time_kmeans2_csc = time_it(1,
	// kmeans(row_cross, numCoords, numObjs, numClusters, threshold, loop_threshold, membership, clusters, type);
	kmeans_char2_csc(cc_r, cc_c, cc_v, numCoords, numObjs, numClusters, threshold, loop_threshold, membership, clusters, type);
	);
	printf("\ntime_kmeans2_csc = %lf (numObjs = %d, numCoords = %d, numClusters = %d)\n", time_kmeans2_csc, numObjs, numCoords, numClusters);

	// for (int i=0; i<numClusters; i++){
	// 	printf("cluster[%3d] = [ ", i);
	// 	for (int j=0; j<numCoords; j++)
	// 		printf("%2.0f ", clusters[i*numCoords + j]);
	// 	printf("]\n");
	// }

	// char * binary_string[numClusters];
	binary_string_with_id clusters_binary[numClusters];

	for(int i = 0; i < numClusters; i++){
		clusters_binary[i].binary_string = convert_unsigned_char_to_binary_string(&clusters[i * numCoords], numCoords);
		clusters_binary[i].id = i;
		// printf("cluster_b[%3d]: %s\n", i, clusters_binary[i].binary_string);
	}
	free(clusters);

	// Sort the array of binary strings
	qsort(clusters_binary, numClusters, sizeof(binary_string_with_id), compare_binary_strings);

	// printf("after sorting...\n");
	// for (int i = 0; i < numClusters; i++)
	// 	printf("cluster_b[%3d]: %s (%d)\n", i, clusters_binary[i].binary_string, clusters_binary[i].id);

	for (int i = 0; i < numClusters; i++)
		free(clusters_binary[i].binary_string);

	int * ordered_membership = (typeof(ordered_membership)) malloc(numClusters * sizeof(*ordered_membership));
	for (int i = 0; i < numClusters; i++)
		ordered_membership[clusters_binary[i].id] = i;

	for(int i = 0; i < numObjs; i++)
		membership[i] = ordered_membership[membership[i]];
	free(ordered_membership);

	*original_col_positions = (typeof(*original_col_positions)) malloc(n * sizeof(**original_col_positions));
	double time_col_reorder = time_it(1,
	csc_reorder_matrix_by_col_batch(m, n, nnz, batch, row_idx, col_ptr, val, membership, row_idx_reorder_c, col_ptr_reorder_c, val_reorder_c, *original_col_positions);
	);
	printf("time_reorder_by_col = %lf\n", time_col_reorder);

	free(membership);
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

