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

#include "csr_reorder_gen.h"


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
#define FUNCTOOLS_GEN_SUFFIX  CONCAT(_CSR_REORDER_GEN_add_d, CSR_REORDER_GEN_SUFFIX)
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
#define QUICKSORT_GEN_SUFFIX  CONCAT(_CSR_REORDER_GEN_d_sign, CSR_REORDER_GEN_SUFFIX)
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
#define quicksort_sign(A, N, aux, partitions_buf) CONCAT(quicksort_CSR_REORDER_GEN_d_sign, CSR_REORDER_GEN_SUFFIX)(A, N, aux, partitions_buf)


//------------------------------------------------------------------------------------------------------------------------------------------
//- Samplesort
//------------------------------------------------------------------------------------------------------------------------------------------

#include "sort/samplesort/samplesort_gen_push.h"
#define SAMPLESORT_GEN_TYPE_1  double
#define SAMPLESORT_GEN_TYPE_2  int
#define SAMPLESORT_GEN_TYPE_3  int
#define SAMPLESORT_GEN_TYPE_4  void
#define SAMPLESORT_GEN_SUFFIX  CONCAT(_CSR_REORDER_GEN_d, CSR_REORDER_GEN_SUFFIX)
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
#define _TYPE_V  CSR_REORDER_GEN_EXPAND(_TYPE_V)
typedef CSR_REORDER_GEN_TYPE_1  _TYPE_V;

#undef  _TYPE_I
#define _TYPE_I  CSR_REORDER_GEN_EXPAND(_TYPE_I)
typedef CSR_REORDER_GEN_TYPE_2  _TYPE_I;


//==========================================================================================================================================
//------------------------------------------------------------------------------------------------------------------------------------------
//-                                                              Templates                                                                 -
//------------------------------------------------------------------------------------------------------------------------------------------
//==========================================================================================================================================

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
void sort_rows(_TYPE_I *row_ptr, long m, _TYPE_I *sorted_row_sizes, _TYPE_I *original_positions)
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

#undef  csr_sort_by_row_size
#define csr_sort_by_row_size  CSR_REORDER_GEN_EXPAND(csr_sort_by_row_size)
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
#define csr_mark_distant_nonzeros  CSR_REORDER_GEN_EXPAND(csr_mark_distant_nonzeros)
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
#define csr_separate_close_distant  CSR_REORDER_GEN_EXPAND(csr_separate_close_distant)
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

#undef  csr_separate_low_high
#define csr_separate_low_high  CSR_REORDER_GEN_EXPAND(csr_separate_low_high)
void
csr_separate_low_high(_TYPE_I *row_ptr, _TYPE_I *col_idx, _TYPE_V *values, long nnz, long m,
					  _TYPE_I *row_low_out,
					  _TYPE_I **row_ptr_low_out, _TYPE_I **col_idx_low_out, _TYPE_V **values_low_out, 
					  _TYPE_I **row_ptr_high_out, _TYPE_I **col_idx_high_out, _TYPE_V **values_high_out)
{
	double nnz_per_row_avg = nnz*1.0/m;
	int cnt_l = 0, cnt_h = 0, nnz_l = 0, nnz_h = 0;

	for(int i=0;i<m;i++){
		int degree = row_ptr[i+1] - row_ptr[i];
		if(degree < nnz_per_row_avg){
			nnz_l += degree;
			cnt_l++;
		}
		else{
			nnz_h += degree;
			cnt_h++;
		}
	}
	printf("size of low:   %d rows, %d nonzeros, %.2lf MB\n", cnt_l, nnz_l, ((cnt_l+1)*sizeof(_TYPE_I) + nnz_l*(sizeof(_TYPE_V) + sizeof(_TYPE_I)))/(1024.0*1024.0));
	printf("size of high:  %d rows, %d nonzeros, %.2lf MB\n", cnt_h, nnz_h, ((cnt_h+1)*sizeof(_TYPE_I) + nnz_h*(sizeof(_TYPE_V) + sizeof(_TYPE_I)))/(1024.0*1024.0));
	printf("size of total: %ld rows, %ld nonzeros, %.2lf MB\n", m, nnz, ((m+1)*sizeof(_TYPE_I) + nnz*(sizeof(_TYPE_V) + sizeof(_TYPE_I)))/(1024.0*1024.0));

	_TYPE_I * row_ptr_low, * col_idx_low, * row_ptr_high, * col_idx_high;
	_TYPE_V * values_low, * values_high;
	row_ptr_low = (typeof(row_ptr_low)) malloc((cnt_l+1) * sizeof(*row_ptr_low));
	col_idx_low = (typeof(col_idx_low)) malloc(nnz_l * sizeof(*col_idx_low));
	values_low = (typeof(values_low)) malloc(nnz_l * sizeof(*values_low));
	
	row_ptr_high = (typeof(row_ptr_high)) malloc((cnt_h+1) * sizeof(*row_ptr_high));
	col_idx_high = (typeof(col_idx_high)) malloc(nnz_h * sizeof(*col_idx_high));
	values_high = (typeof(values_high)) malloc(nnz_h * sizeof(*values_high));

	// Initialize row_ptr for "low" and "high"
	row_ptr_low[0] = 0;
	row_ptr_high[0] = 0;

	// Distribute elements based on row size
	int low_idx = 0;
	int high_idx = 0;
	int i_l = 0, i_h = 0;

	for (int i = 0; i < m; i++) {
		int degree = row_ptr[i+1] - row_ptr[i];
		if (degree < nnz_per_row_avg) {
			for (int j = row_ptr[i]; j < row_ptr[i + 1]; j++) {
				// "low" element
				col_idx_low[low_idx] = col_idx[j];
				values_low[low_idx] = values[j];
				low_idx++;				
			}
			row_ptr_low[i_l + 1] = low_idx;
			i_l++;
		} else {
			for (int j = row_ptr[i]; j < row_ptr[i + 1]; j++) {
				// "high" element
				col_idx_high[high_idx] = col_idx[j];
				values_high[high_idx] = values[j];
				high_idx++;				
			}
			row_ptr_high[i_h + 1] = high_idx;
			i_h++;
		}
	}
	printf("low_idx = %d, high_idx = %d\n", i_l, i_h);
	printf("low_nnz = %d, high_nnz = %d\n", row_ptr_low[i_l], row_ptr_high[i_h]);

	if(*row_ptr_low_out != NULL)
		*row_ptr_low_out = row_ptr_low;
	else
		free(row_ptr_low);

	if(*col_idx_low_out != NULL)
		*col_idx_low_out = col_idx_low;
	else
		free(col_idx_low);

	if(*values_low_out != NULL)
		*values_low_out = values_low;
	else
		free(values_low);

	if(*row_ptr_high_out != NULL)
		*row_ptr_high_out = row_ptr_high;
	else
		free(row_ptr_high);

	if(*col_idx_high_out != NULL)
		*col_idx_high_out = col_idx_high;
	else
		free(col_idx_high);

	if(*values_high_out != NULL)
		*values_high_out = values_high;
	else
		free(values_high);

	*row_low_out = i_l;
}

#undef  csr_shuffle_matrix
#define csr_shuffle_matrix  CSR_REORDER_GEN_EXPAND(csr_shuffle_matrix)
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
		_TYPE_I start_idx	 = row_ptr_shuffle[new_row];
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
#define csr_extract_row_cross  CSR_REORDER_GEN_EXPAND(csr_extract_row_cross)
void
csr_extract_row_cross(_TYPE_I *row_ptr, _TYPE_I *col_idx, __attribute__((unused)) _TYPE_V *val, int m, int n, int nnz, int window_width, 
					  int *num_windows_out, float **row_cross_out, int plot, _TYPE_I **rc_r_out, _TYPE_I **rc_c_out, float **rc_v_out)
{
	int num_windows = (n-1 + window_width) / window_width;
	printf("m = %d, n = %d, window_width = %d\n", m, n, window_width);
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
		printf("rc_elements_nz = %lu\n", rc_elements_nz);

		_TYPE_I * rc_r, * rc_c;
		float * rc_v;
		// rc_r = (typeof(rc_r)) malloc(rc_elements_nz * sizeof(*rc_r));
		rc_r = (typeof(rc_r)) malloc((m+1) * sizeof(*rc_r));
		rc_c = (typeof(rc_c)) malloc(rc_elements_nz * sizeof(*rc_c));
		rc_v = (typeof(rc_v)) malloc(rc_elements_nz * sizeof(*rc_v));
		double row_cross_compr_mem_foot = ((m+1) * sizeof(*rc_r) + rc_elements_nz * sizeof(*rc_c) + rc_elements_nz * sizeof(*rc_v))/(1024*1024*1.0);
		// printf("memory footprint of row_cross (compressed) = %.2lf MB\n", row_cross_compr_mem_foot);
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
		// printf("time_final = %lf\n", time_final);
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
#define csr_extract_row_cross2  CSR_REORDER_GEN_EXPAND(csr_extract_row_cross2)
void
csr_extract_row_cross2(_TYPE_I *row_ptr, _TYPE_I *col_idx, __attribute__((unused)) _TYPE_V *val, int m, int n, int nnz, int window_width, 
					   int *num_windows_out, _TYPE_I **rc_r_out, _TYPE_I **rc_c_out, float **rc_v_out)
{
	int num_windows = (n-1 + window_width) / window_width;
	printf("m = %d, n = %d, window_width = %d\n", m, n, window_width);
	int num_threads = omp_get_max_threads();

	_TYPE_I * thread_i_s = (_TYPE_I *) malloc(num_threads * sizeof(*thread_i_s));
	_TYPE_I * thread_i_e = (_TYPE_I *) malloc(num_threads * sizeof(*thread_i_e));
	long unsigned  * thread_rc_elements_nz = (long unsigned  *) malloc(num_threads * sizeof(*thread_rc_elements_nz));
	long unsigned  * rc_r_elem = (long unsigned  *) calloc(m, sizeof(*rc_r_elem));

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
			for(int k=0;k<num_windows; k++){
				thread_rc_elements_nz[tnum] += local_row_cross[k];
				rc_r_elem[i] += local_row_cross[k];
			}
			free(local_row_cross);
		}
	}
	long unsigned rc_elements_nz = 0;
	_Pragma("omp parallel for reduction(+:rc_elements_nz)")
	for(int k=0;k<num_threads;k++)
		rc_elements_nz += thread_rc_elements_nz[k];

	_TYPE_I * rc_r, * rc_c;
	float * rc_v;
	rc_r = (typeof(rc_r)) malloc((m+1) * sizeof(*rc_r));
	rc_c = (typeof(rc_c)) malloc(rc_elements_nz * sizeof(*rc_c));
	rc_v = (typeof(rc_v)) malloc(rc_elements_nz * sizeof(*rc_v));
	// double row_cross_compr_mem_foot = ((m+1) * sizeof(*rc_r) + rc_elements_nz * sizeof(*rc_c) + rc_elements_nz * sizeof(*rc_v))/(1024*1024*1.0);
	// printf("memory footprint of row_cross (compressed) = %.2lf MB\n", row_cross_compr_mem_foot);

	rc_r[0]=0;
	for(int i=1;i<m+1;i++)
		rc_r[i] = rc_r[i-1]+rc_r_elem[i-1];
	free(rc_r_elem);

	_Pragma("omp parallel")
	{
		int tnum = omp_get_thread_num();
		loop_partitioner_balance_prefix_sums(num_threads, tnum, rc_r, m, rc_r[m], &thread_i_s[tnum], &thread_i_e[tnum]);
		_Pragma("omp barrier")
		// _Pragma("omp single")
		// {
		// 	for(int tnum = 0; tnum < num_threads; tnum++)
		// 		printf("tnum = %2d\trows = (%d-%d) %d\t\tnonzeros = %d\n", tnum, thread_i_s[tnum], thread_i_e[tnum], thread_i_e[tnum] - thread_i_s[tnum], rc_r[thread_i_e[tnum]] - rc_r[thread_i_s[tnum]]);
		// }
		for(_TYPE_I i=thread_i_s[tnum]; i<thread_i_e[tnum]; i++){
			long unsigned * local_row_cross = (typeof(local_row_cross)) calloc(num_windows, sizeof(*local_row_cross));
			for(_TYPE_I j=row_ptr[i]; j<row_ptr[i+1]; j++){
				_TYPE_I cw_loc = col_idx[j] / window_width; // col window location
				local_row_cross[cw_loc]++; // if already found in cw_loc, just overwrite it, no problem with that... we just want to identify how many windows for sparse struct later
			}
			int row_offset = rc_r[i];
			int cnt = 0;
			for(int k=0;k<num_windows; k++){
				if(local_row_cross[k] != 0){
					rc_c[row_offset + cnt] = k;
					rc_v[row_offset + cnt] = local_row_cross[k];
					cnt++;
				}
			}
			free(local_row_cross);
		}
	}
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

#undef  csr_extract_row_cross2_batch
#define csr_extract_row_cross2_batch  CSR_REORDER_GEN_EXPAND(csr_extract_row_cross2_batch)
void
csr_extract_row_cross2_batch(_TYPE_I *row_ptr, _TYPE_I *col_idx, __attribute__((unused)) _TYPE_V *val, int m, int n, int nnz, int window_width, int batch, 
							 int *num_windows_out, _TYPE_I **rc_r_out, _TYPE_I **rc_c_out, float **rc_v_out)
{
	int num_windows = (n-1 + window_width) / window_width;
	printf("m = %d, n = %d, window_width = %d, batch = %d\n", m, n, window_width, batch);
	int num_threads = omp_get_max_threads();

	_TYPE_I * thread_i_s = (_TYPE_I *) malloc(num_threads * sizeof(*thread_i_s));
	_TYPE_I * thread_i_e = (_TYPE_I *) malloc(num_threads * sizeof(*thread_i_e));
	long unsigned  * thread_rc_elements_nz = (long unsigned  *) malloc(num_threads * sizeof(*thread_rc_elements_nz));
	int m_batch = (m-1 + batch) / batch; 
	long unsigned  * rc_r_elem = (long unsigned  *) calloc(m_batch, sizeof(*rc_r_elem));

	// batched-rows row_ptr array. it will be used for loop partitioner only, so that rows with same batch-index are not assigned to different threads, causing conflict on rc_r_elem array.
	_TYPE_I * row_ptr_b = (_TYPE_I *) malloc((m_batch+1) * sizeof(*row_ptr_b));
	row_ptr_b[0] = 0;
	_Pragma("omp parallel for")
	for(int i=0;i<m+1;i+=batch)
		row_ptr_b[i/batch] = row_ptr[i];
	row_ptr_b[m_batch] = row_ptr[m];

	_Pragma("omp parallel")
	{
		int tnum = omp_get_thread_num();
		loop_partitioner_balance_prefix_sums(num_threads, tnum, row_ptr_b, m_batch, nnz, &thread_i_s[tnum], &thread_i_e[tnum]);
		_Pragma("omp barrier")
		// _Pragma("omp single")
		// {
		// 	for(int tnum = 0; tnum < num_threads; tnum++)
		// 		printf("tnum = %2d\trows = (%d-%d) %d\n", tnum, thread_i_s[tnum], thread_i_e[tnum], thread_i_e[tnum] - thread_i_s[tnum]);
		// }
		thread_rc_elements_nz[tnum] = 0;
		for(_TYPE_I i=thread_i_s[tnum]; i<thread_i_e[tnum]; i++){
			long unsigned * local_row_cross = (typeof(local_row_cross)) calloc(num_windows, sizeof(*local_row_cross));
			// printf("i = %d, start = %d, end = %d\n", i, i*batch, ((i+1)*batch < m) ? (i+1)*batch-1 : m-1);
			for(_TYPE_I ii=i*batch; ii < (i+1)*batch && ii < m; ii++){
				for(_TYPE_I j=row_ptr[ii]; j<row_ptr[ii+1]; j++){
					_TYPE_I cw_loc = col_idx[j] / window_width; // col window location
					local_row_cross[cw_loc] = 1; // if already found in cw_loc, just overwrite it, no problem with that... we just want to identify how many windows for sparse struct later
				}
			}
			for(int k=0;k<num_windows; k++){
				thread_rc_elements_nz[tnum] += local_row_cross[k];
				rc_r_elem[i] += local_row_cross[k];
			}
			free(local_row_cross);
		}
	}
	free(row_ptr_b);
	long unsigned rc_elements_nz = 0;
	_Pragma("omp parallel for reduction(+:rc_elements_nz)")
	for(int k=0;k<num_threads;k++)
		rc_elements_nz += thread_rc_elements_nz[k];

	_TYPE_I * rc_r, * rc_c;
	float * rc_v;
	rc_r = (typeof(rc_r)) malloc((m_batch+1) * sizeof(*rc_r));
	rc_c = (typeof(rc_c)) malloc(rc_elements_nz * sizeof(*rc_c));
	rc_v = (typeof(rc_v)) malloc(rc_elements_nz * sizeof(*rc_v));
	// double row_cross_compr_mem_foot = ((m_batch+1) * sizeof(*rc_r) + rc_elements_nz * sizeof(*rc_c) + rc_elements_nz * sizeof(*rc_v))/(1024*1024*1.0);
	// printf("memory footprint of row_cross (compressed) = %.2lf MB\n", row_cross_compr_mem_foot);

	rc_r[0]=0;
	for(int i=1;i<m_batch+1;i++)
		rc_r[i] = rc_r[i-1]+rc_r_elem[i-1];
	free(rc_r_elem);

	_Pragma("omp parallel")
	{
		int tnum = omp_get_thread_num();
		loop_partitioner_balance_prefix_sums(num_threads, tnum, rc_r, m_batch, rc_r[m_batch], &thread_i_s[tnum], &thread_i_e[tnum]);
		_Pragma("omp barrier")
		// _Pragma("omp single")
		// {
		// 	for(int tnum = 0; tnum < num_threads; tnum++)
		// 		printf("tnum = %2d\trows = (%d-%d) %d\t\tnonzeros = %d\n", tnum, thread_i_s[tnum], thread_i_e[tnum], thread_i_e[tnum] - thread_i_s[tnum], rc_r[thread_i_e[tnum]] - rc_r[thread_i_s[tnum]]);
		// }
		for(_TYPE_I i=thread_i_s[tnum]; i<thread_i_e[tnum]; i++){
			long unsigned * local_row_cross = (typeof(local_row_cross)) calloc(num_windows, sizeof(*local_row_cross));
			for(_TYPE_I ii=i*batch; ii < (i+1)*batch && ii < m; ii++){
				for(_TYPE_I j=row_ptr[ii]; j<row_ptr[ii+1]; j++){
					_TYPE_I cw_loc = col_idx[j] / window_width; // col window location
					local_row_cross[cw_loc]++; // if already found in cw_loc, just overwrite it, no problem with that... we just want to identify how many windows for sparse struct later
				}
			}
			int row_offset = rc_r[i];
			int cnt = 0;
			for(int k=0;k<num_windows; k++){
				if(local_row_cross[k] != 0){
					rc_c[row_offset + cnt] = k;
					rc_v[row_offset + cnt] = local_row_cross[k];
					cnt++;
				}
			}
			free(local_row_cross);
		}
	}
	*num_windows_out = num_windows;
	if (rc_r_out != NULL)
		*rc_r_out = rc_r;
	if (rc_c_out != NULL)
		*rc_c_out = rc_c;
	if (rc_v_out != NULL)
		*rc_v_out = rc_v;

	_TYPE_I * degrees_rows_batched = (_TYPE_I *) malloc(m_batch * sizeof(*degrees_rows_batched));
	#pragma omp parallel for
	for(int i=0; i<m_batch; i++)
		degrees_rows_batched[i] = rc_r[i+1] - rc_r[i];
	double deg_min, deg_max, deg_avg, deg_std;
	array_min_max(degrees_rows_batched, m_batch, &deg_min, NULL, &deg_max, NULL);
	array_mean(degrees_rows_batched, m_batch, &deg_avg);
	array_std(degrees_rows_batched, m_batch, &deg_std);
	printf("\n----\ndeg_max = %.4lf\n", deg_max);
	printf("deg_min = %.4lf\n", deg_min);
	printf("deg_avg = %.4lf\n", deg_avg);
	printf("deg_std = %.4lf\n----\n", deg_std);
	free(degrees_rows_batched);

	free(thread_i_s);
	free(thread_i_e);
}

#undef  csr_extract_row_cross_char
#define csr_extract_row_cross_char  CSR_REORDER_GEN_EXPAND(csr_extract_row_cross_char)
void
csr_extract_row_cross_char(_TYPE_I *row_ptr, _TYPE_I *col_idx, __attribute__((unused)) _TYPE_V *val, int m, int n, int nnz, int window_width, 
					  int *num_windows_out, unsigned char **row_cross_out, int plot, _TYPE_I **rc_r_out, _TYPE_I **rc_c_out, float **rc_v_out)
{
	int num_windows = (n-1 + window_width) / window_width;
	printf("m = %d, n = %d, num_windows = %d\n", m, n, num_windows);
	long unsigned rc_elements = m * num_windows;
	unsigned char * row_cross = (typeof(row_cross)) calloc(rc_elements, sizeof(*row_cross));
	// double row_cross_mem_foot = (m * 1.0 * num_windows * sizeof(*row_cross))/(1024*1024*1.0);
	// printf("memory footprint of row_cross = %.2lf MB\n", row_cross_mem_foot);

	int num_threads = omp_get_max_threads();

	_TYPE_I * thread_i_s = (_TYPE_I *) malloc(num_threads * sizeof(*thread_i_s));
	_TYPE_I * thread_i_e = (_TYPE_I *) malloc(num_threads * sizeof(*thread_i_e));
	
	
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
			for(_TYPE_I j=row_ptr[i]; j<row_ptr[i+1]; j++){
				_TYPE_I cw_loc = col_idx[j] / window_width; // col window location
				long unsigned rc_ind = i * num_windows + cw_loc;
				// row_cross[rc_ind] = 1;
				row_cross[rc_ind] = '1';
			}
		}
	}
	// );
	// printf("time for row_cross = %lf\n", time_row_cross);

	if(plot){
		long unsigned rc_elements_nz = 0;
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

		_TYPE_I * rc_r, * rc_c;
		float * rc_v;
		// rc_r = (typeof(rc_r)) malloc(rc_elements_nz * sizeof(*rc_r));
		rc_r = (typeof(rc_r)) malloc((m+1) * sizeof(*rc_r));
		rc_c = (typeof(rc_c)) malloc(rc_elements_nz * sizeof(*rc_c));
		rc_v = (typeof(rc_v)) malloc(rc_elements_nz * sizeof(*rc_v));
		// double row_cross_compr_mem_foot = ((m+1) * sizeof(*rc_r) + rc_elements_nz * sizeof(*rc_c) + rc_elements_nz * sizeof(*rc_v))/(1024*1024*1.0);
		// printf("memory footprint of row_cross (compressed) = %.2lf MB\n", row_cross_compr_mem_foot);
		// printf("compression ratio = %.2lf%\n", row_cross_compr_mem_foot/row_cross_mem_foot * 100);

		// in order to make it faster, perhaps calculate rc_r beforehand and use the dgal partitioner for rc_c, rc_v
		int cnt=0;
		rc_r[0] = 0;
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

#undef  csr_extract_row_cross_char2
#define csr_extract_row_cross_char2  CSR_REORDER_GEN_EXPAND(csr_extract_row_cross_char2)
void
csr_extract_row_cross_char2(_TYPE_I *row_ptr, _TYPE_I *col_idx, __attribute__((unused)) _TYPE_V *val, int m, int n, int nnz, int window_width, 
							int *num_windows_out, _TYPE_I **rc_r_out, _TYPE_I **rc_c_out, unsigned char **rc_v_out)
{
	int num_windows = (n-1 + window_width) / window_width;
	printf("m = %d, n = %d, window_width = %d\n", m, n, window_width);
	int num_threads = omp_get_max_threads();

	_TYPE_I * thread_i_s = (_TYPE_I *) malloc(num_threads * sizeof(*thread_i_s));
	_TYPE_I * thread_i_e = (_TYPE_I *) malloc(num_threads * sizeof(*thread_i_e));
	long unsigned  * thread_rc_elements_nz = (long unsigned  *) malloc(num_threads * sizeof(*thread_rc_elements_nz));
	long unsigned  * rc_r_elem = (long unsigned  *) calloc(m, sizeof(*rc_r_elem));
	
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
			for(int k=0;k<num_windows; k++){
				thread_rc_elements_nz[tnum] += local_row_cross[k];
				rc_r_elem[i] += local_row_cross[k];
			}
			free(local_row_cross);
		}
	}
	long unsigned rc_elements_nz = 0;
	_Pragma("omp parallel for reduction(+:rc_elements_nz)")
	for(int k=0;k<num_threads;k++)
		rc_elements_nz += thread_rc_elements_nz[k];

	_TYPE_I * rc_r, * rc_c;
	unsigned char * rc_v;
	rc_r = (typeof(rc_r)) malloc((m+1) * sizeof(*rc_r));
	rc_c = (typeof(rc_c)) malloc(rc_elements_nz * sizeof(*rc_c));
	rc_v = (typeof(rc_v)) malloc(rc_elements_nz * sizeof(*rc_v));
	// double row_cross_compr_mem_foot = ((m+1) * sizeof(*rc_r) + rc_elements_nz * sizeof(*rc_c) + rc_elements_nz * sizeof(*rc_v))/(1024*1024*1.0);
	// printf("memory footprint of row_cross (compressed) = %.2lf MB\n", row_cross_compr_mem_foot);

	rc_r[0]=0;
	for(int i=1;i<m+1;i++)
		rc_r[i] = rc_r[i-1]+rc_r_elem[i-1];
	free(rc_r_elem);

	_Pragma("omp parallel")
	{
		int tnum = omp_get_thread_num();
		loop_partitioner_balance_prefix_sums(num_threads, tnum, rc_r, m, rc_r[m], &thread_i_s[tnum], &thread_i_e[tnum]);
		_Pragma("omp barrier")
		// _Pragma("omp single")
		// {
		// 	for(int tnum = 0; tnum < num_threads; tnum++)
		// 		printf("tnum = %2d\trows = (%d-%d) %d\t\tnonzeros = %d\n", tnum, thread_i_s[tnum], thread_i_e[tnum], thread_i_e[tnum] - thread_i_s[tnum], rc_r[thread_i_e[tnum]] - rc_r[thread_i_s[tnum]]);
		// }
		for(_TYPE_I i=thread_i_s[tnum]; i<thread_i_e[tnum]; i++){
			long unsigned * local_row_cross = (typeof(local_row_cross)) calloc(num_windows, sizeof(*local_row_cross));
			for(_TYPE_I j=row_ptr[i]; j<row_ptr[i+1]; j++){
				_TYPE_I cw_loc = col_idx[j] / window_width; // col window location
				local_row_cross[cw_loc]++; // if already found in cw_loc, just overwrite it, no problem with that... we just want to identify how many windows for sparse struct later
			}
			int row_offset = rc_r[i];
			int cnt = 0;
			for(int k=0;k<num_windows; k++){
				if(local_row_cross[k] != 0){
					rc_c[row_offset + cnt] = k;
					rc_v[row_offset + cnt] = 1; // local_row_cross[k]; // let's check how it performs when using - update: shit
					cnt++;
				}
			}
			free(local_row_cross);
		}
	}
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

#undef  csr_extract_row_cross_char2_batch
#define csr_extract_row_cross_char2_batch  CSR_REORDER_GEN_EXPAND(csr_extract_row_cross_char2_batch)
void
csr_extract_row_cross_char2_batch(_TYPE_I *row_ptr, _TYPE_I *col_idx, __attribute__((unused)) _TYPE_V *val, int m, int n, int nnz, int window_width, int batch, 
								  int *num_windows_out, _TYPE_I **rc_r_out, _TYPE_I **rc_c_out, unsigned char **rc_v_out)
{
	int num_windows = (n-1 + window_width) / window_width;
	printf("m = %d, n = %d, window_width = %d, batch = %d\n", m, n, window_width, batch);
	int num_threads = omp_get_max_threads();

	_TYPE_I * thread_i_s = (_TYPE_I *) malloc(num_threads * sizeof(*thread_i_s));
	_TYPE_I * thread_i_e = (_TYPE_I *) malloc(num_threads * sizeof(*thread_i_e));
	long unsigned  * thread_rc_elements_nz = (long unsigned  *) malloc(num_threads * sizeof(*thread_rc_elements_nz));
	int m_batch = (m-1 + batch) / batch; 
	long unsigned  * rc_r_elem = (long unsigned  *) calloc(m_batch, sizeof(*rc_r_elem));

	// batched-rows row_ptr array. it will be used for loop partitioner only, so that rows with same batch-index are not assigned to different threads, causing conflict on rc_r_elem array.
	_TYPE_I * row_ptr_b = (_TYPE_I *) malloc((m_batch+1) * sizeof(*row_ptr_b));
	row_ptr_b[0] = 0;
	_Pragma("omp parallel for")
	for(int i=0;i<m+1;i+=batch)
		row_ptr_b[i/batch] = row_ptr[i];
	row_ptr_b[m_batch] = row_ptr[m];

	_Pragma("omp parallel")
	{
		int tnum = omp_get_thread_num();
		loop_partitioner_balance_prefix_sums(num_threads, tnum, row_ptr_b, m_batch, nnz, &thread_i_s[tnum], &thread_i_e[tnum]);
		_Pragma("omp barrier")
		// _Pragma("omp single")
		// {
		// 	for(int tnum = 0; tnum < num_threads; tnum++)
		// 		printf("tnum = %2d\trows = (%d-%d) %d\n", tnum, thread_i_s[tnum], thread_i_e[tnum], thread_i_e[tnum] - thread_i_s[tnum]);
		// }
		thread_rc_elements_nz[tnum] = 0;
		for(_TYPE_I i=thread_i_s[tnum]; i<thread_i_e[tnum]; i++){
			long unsigned * local_row_cross = (typeof(local_row_cross)) calloc(num_windows, sizeof(*local_row_cross));
			// printf("i = %d, start = %d, end = %d\n", i, i*batch, ((i+1)*batch < m) ? (i+1)*batch-1 : m-1);
			for(_TYPE_I ii=i*batch; ii < (i+1)*batch && ii < m; ii++){
				for(_TYPE_I j=row_ptr[ii]; j<row_ptr[ii+1]; j++){
					_TYPE_I cw_loc = col_idx[j] / window_width; // col window location
					local_row_cross[cw_loc] = 1; // if already found in cw_loc, just overwrite it, no problem with that... we just want to identify how many windows for sparse struct later
				}
			}
			for(int k=0;k<num_windows; k++){
				thread_rc_elements_nz[tnum] += local_row_cross[k];
				rc_r_elem[i] += local_row_cross[k];
			}
			free(local_row_cross);
		}
	}
	free(row_ptr_b);
	long unsigned rc_elements_nz = 0;
	_Pragma("omp parallel for reduction(+:rc_elements_nz)")
	for(int k=0;k<num_threads;k++)
		rc_elements_nz += thread_rc_elements_nz[k];

	_TYPE_I * rc_r, * rc_c;
	unsigned char * rc_v;
	rc_r = (typeof(rc_r)) malloc((m_batch+1) * sizeof(*rc_r));
	rc_c = (typeof(rc_c)) malloc(rc_elements_nz * sizeof(*rc_c));
	rc_v = (typeof(rc_v)) malloc(rc_elements_nz * sizeof(*rc_v));
	// double row_cross_compr_mem_foot = ((m_batch+1) * sizeof(*rc_r) + rc_elements_nz * sizeof(*rc_c) + rc_elements_nz * sizeof(*rc_v))/(1024*1024*1.0);
	// printf("memory footprint of row_cross (compressed) = %.2lf MB\n", row_cross_compr_mem_foot);

	rc_r[0]=0;
	for(int i=1;i<m_batch+1;i++)
		rc_r[i] = rc_r[i-1]+rc_r_elem[i-1];
	free(rc_r_elem);

	_Pragma("omp parallel")
	{
		int tnum = omp_get_thread_num();
		loop_partitioner_balance_prefix_sums(num_threads, tnum, rc_r, m_batch, rc_r[m_batch], &thread_i_s[tnum], &thread_i_e[tnum]);
		_Pragma("omp barrier")
		// _Pragma("omp single")
		// {
		// 	for(int tnum = 0; tnum < num_threads; tnum++)
		// 		printf("tnum = %2d\trows = (%d-%d) %d\t\tnonzeros = %d\n", tnum, thread_i_s[tnum], thread_i_e[tnum], thread_i_e[tnum] - thread_i_s[tnum], rc_r[thread_i_e[tnum]] - rc_r[thread_i_s[tnum]]);
		// }
		for(_TYPE_I i=thread_i_s[tnum]; i<thread_i_e[tnum]; i++){
			long unsigned * local_row_cross = (typeof(local_row_cross)) calloc(num_windows, sizeof(*local_row_cross));
			for(_TYPE_I ii=i*batch; ii < (i+1)*batch && ii < m; ii++){
				for(_TYPE_I j=row_ptr[ii]; j<row_ptr[ii+1]; j++){
					_TYPE_I cw_loc = col_idx[j] / window_width; // col window location
					local_row_cross[cw_loc]++; // if already found in cw_loc, just overwrite it, no problem with that... we just want to identify how many windows for sparse struct later
				}
			}
			int row_offset = rc_r[i];
			int cnt = 0;
			for(int k=0;k<num_windows; k++){
				if(local_row_cross[k] != 0){
					rc_c[row_offset + cnt] = k;
					rc_v[row_offset + cnt] = 1; // local_row_cross[k]; // let's check how it performs when using - update: shit
					cnt++;
				}
			}
			free(local_row_cross);
		}
	}
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

typedef struct {
	int row;
	int family_id;
} RowFamily;

int compareRowFamily(const void *a, const void *b) {
	return ((RowFamily *)b)->family_id - ((RowFamily *)a)->family_id;
	// return ((RowFamily *)a)->family_id - ((RowFamily *)b)->family_id;
}

#undef  csr_reorder_matrix_by_row
#define csr_reorder_matrix_by_row  CSR_REORDER_GEN_EXPAND(csr_reorder_matrix_by_row)
void
csr_reorder_matrix_by_row(int m, __attribute__((unused)) int n, __attribute__((unused)) int nnz, _TYPE_I * row_ptr, _TYPE_I * col_idx, _TYPE_V * val, 
					int * membership, _TYPE_I * row_ptr_reorder, _TYPE_I * col_idx_reorder, _TYPE_V * val_reorder, _TYPE_I * original_row_positions)
{
	// Create an auxiliary array to store row indices and family IDs
	RowFamily* row_families = (RowFamily*)malloc(m * sizeof(RowFamily));

	for (int i = 0; i < m; i++) {
		row_families[i].row = i;
		row_families[i].family_id = membership[i];
	}

	// Sort the auxiliary array based on family IDs
	qsort(row_families, m, sizeof(RowFamily), compareRowFamily);

	row_ptr_reorder[0] = 0;
	int cnt = 0;

	for (int i = 0; i < m; i++) {
		int original_row = row_families[i].row;
		original_row_positions[i] = original_row;
		// if(i<10) printf("row: %d, membership; %d\n", row_families[i].row, row_families[i].family_id);
		
		for (int j = row_ptr[original_row]; j < row_ptr[original_row + 1]; j++) {
			col_idx_reorder[cnt] = col_idx[j];
			val_reorder[cnt] = val[j];
			cnt++;
		}
		row_ptr_reorder[i+1] = cnt;
	}
	free(row_families);
}

#undef  csr_reorder_matrix_by_row_batch
#define csr_reorder_matrix_by_row_batch  CSR_REORDER_GEN_EXPAND(csr_reorder_matrix_by_row_batch)
void
csr_reorder_matrix_by_row_batch(int m, __attribute__((unused)) int n, __attribute__((unused)) int nnz, int batch, _TYPE_I * row_ptr, _TYPE_I * col_idx, _TYPE_V * val, 
								int * membership, _TYPE_I * row_ptr_reorder, _TYPE_I * col_idx_reorder, _TYPE_V * val_reorder, _TYPE_I * original_row_positions)
{
	int m_batch = (m-1 + batch) / batch; 
	// Create an auxiliary array to store row indices and family IDs
	RowFamily* row_families = (RowFamily*)malloc(m_batch * sizeof(RowFamily));

	for (int i = 0; i < m_batch; i++) {
		row_families[i].row = i;
		row_families[i].family_id = membership[i];
	}

	// Sort the auxiliary array based on family IDs
	qsort(row_families, m_batch, sizeof(RowFamily), compareRowFamily);

	row_ptr_reorder[0] = 0;
	int cnt = 0, cnt2 = 0;
	// int last = -1;
	for (int ii = 0; ii < m_batch; ii++) {
		int original_row = row_families[ii].row;
		// if(row_families[ii].family_id != last)
		// 	printf("i = %d, new cluster = %d\n", ii*batch, row_families[ii].family_id);
		// last = row_families[ii].family_id;

		// printf("ii: %d, row: %d ( %d - %d ), membership: %d\n", ii, original_row, original_row*batch, ((original_row+1)*batch  < m) ? (original_row+1)*batch-1 : m-1, row_families[ii].family_id);
		for(int i = original_row*batch; i < (original_row+1)*batch && i < m; i++)
		{
			for (int j = row_ptr[i]; j < row_ptr[i + 1]; j++) {
				col_idx_reorder[cnt] = col_idx[j];
				val_reorder[cnt] = val[j];
				cnt++;
			}
			row_ptr_reorder[cnt2+1] = cnt;
			original_row_positions[cnt2] = i;
			// printf("finished with row = %d, nnz = %d\n", cnt2, cnt);
			cnt2++;
		}
	}
	free(row_families);
}

/*****************************************/
/* csr_kmeans_reorder_row util functions */
void random_selection_csr(_TYPE_I * start_rc_r, _TYPE_I * start_rc_c, float * start_rc_v, float * target_array, int numObjs, int numClusters, int numCoords, int seed)
{
	int * indices = (int *) malloc(numObjs * sizeof(int));
	for(int i = 0; i < numObjs; i++)
		indices[i] = i;
	srand(seed);
	for(int i = 0; i < numClusters ; i++) {
		int k = rand() % (numObjs - i);
		printf("dialeksa k = %d\n", k);
		float *start_array = (typeof(start_array)) calloc(numCoords, sizeof(*start_array));
		for(int j = start_rc_r[indices[k]]; j < start_rc_r[indices[k]+1]; j++)
			start_array[start_rc_c[j]] = start_rc_v[j];

		for(int j = 0; j < numCoords; j++)
			target_array[i * numCoords + j] = start_array[j];
			// target_array[i * numCoords + j] = start_array[indices[k] * numCoords + j];
		indices[k] = indices[numCoords - i - 1];
		free(start_array);
	}
	free(indices);
	/*
	for (int i=0; i<numClusters; i++){
		printf("cluster[%3d] = [ ", i);
		for (int j=0; j<numCoords; j++){
			int decision = 0;
			if(target_array[i*numCoords + j]>0)
				decision=1;
			printf("%d", decision);
			// printf("%.0f ", target_array[i*numCoords + j]);
		}
		printf("]\n");
	}
	*/
}

void diagonal_selection_csr(float * target_array, int numClusters, int numCoords, float max_value)
{
	int width = (numCoords-1 + numClusters) / numClusters;
	for(int i = 0; i < numClusters; i++){
		for(int j = 0; j < numCoords; j++){
			if(j >= i*width && j < (i+1)*width)
				target_array[i * numCoords + j] = max_value;
			else
				target_array[i * numCoords + j] = 0;
		}
	}
	/*
	for (int i=0; i<numClusters; i++){
		printf("cluster[%3d] = [ ", i);
		for (int j=0; j<numCoords; j++){
			int decision = 0;
			if(target_array[i*numCoords + j]>0)
				decision=1;
			printf("%d", decision);
			// printf("%.0f ", target_array[i*numCoords + j]);
		}
		printf("]\n");
	}
	*/
}

#undef  csr_kmeans_reorder_row
#define csr_kmeans_reorder_row  CSR_REORDER_GEN_EXPAND(csr_kmeans_reorder_row)
void
csr_kmeans_reorder_row(_TYPE_I * row_ptr, _TYPE_I * col_idx, _TYPE_V * val, 
					   _TYPE_I * row_ptr_reorder_r, _TYPE_I * col_idx_reorder_r, _TYPE_V * val_reorder_r, _TYPE_I ** original_row_positions,
					   int m, int n, int nnz, 
					   int numClusters, float threshold, int loop_threshold, int num_windows, int seed, _TYPE_I * rc_r, _TYPE_I * rc_c, float * rc_v)
{
	int numObjs = m;
	int numCoords = num_windows;
	int * membership = (typeof(membership)) malloc(numObjs * sizeof(*membership));

	float * clusters = (typeof(clusters)) malloc(numClusters * numCoords * sizeof(*clusters));
	random_selection_csr(rc_r, rc_c, rc_v, clusters, numObjs, numClusters, numCoords, seed);
	// float max_value = -1;
	// for(int i=0; i<numObjs; i++)
	// 	if(max_value < rc_v[i])
	// 		max_value = rc_v[i];
	// printf("max_value = %lf\n", max_value);
	// diagonal_selection_csr(clusters, numClusters, numCoords, max_value);

	int type = 0; // cosine similarity
	double time_kmeans2_csr = time_it(1,
	// kmeans(row_cross, numCoords, numObjs, numClusters, threshold, loop_threshold, membership, clusters, type);
	kmeans2_csr(rc_r, rc_c, rc_v, numCoords, numObjs, numClusters, threshold, loop_threshold, membership, clusters, type);
	);
	printf("\ntime_kmeans2_csr = %lf (numObjs = %d, numCoords = %d, numClusters = %d)\n", time_kmeans2_csr, numObjs, numCoords, numClusters);

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

	for(int i = 0; i < numObjs; i++){
		membership[i] = ordered_membership[membership[i]];
		ordered_membership[membership[i]] = 0;
	}
	for(int i=0;i<numObjs;i++)
		ordered_membership[membership[i]]++;
	// for(int i=0;i<numClusters;i++)
	// 	printf("I = %d\t%d\t( %.2f%% )\n", i, ordered_membership[i], ordered_membership[i]*100.0/numObjs);
	free(ordered_membership);

	*original_row_positions = (typeof(*original_row_positions)) malloc(m * sizeof(**original_row_positions));
	csr_reorder_matrix_by_row(m, n, nnz, row_ptr, col_idx, val, membership, row_ptr_reorder_r, col_idx_reorder_r, val_reorder_r, *original_row_positions);

	free(membership);
}

#undef  csr_kmeans_reorder_by_file
#define csr_kmeans_reorder_by_file  CSR_REORDER_GEN_EXPAND(csr_kmeans_reorder_by_file)
void
csr_kmeans_reorder_by_file(char* reorder_file, 
						   _TYPE_I * row_ptr, _TYPE_I * col_idx, _TYPE_V * val, 
						   _TYPE_I * row_ptr_reorder_r, _TYPE_I * col_idx_reorder_r, _TYPE_V * val_reorder_r, _TYPE_I ** original_row_positions,
						   int m, int n, int nnz)
{
	int numObjs = m;
	int * membership = (typeof(membership)) malloc(numObjs * sizeof(*membership));
  	
  	FILE *file;
	file = fopen(reorder_file, "r");
	for (int i = 0; i < m; ++i)
		fscanf(file, "%d", &membership[i]);

	*original_row_positions = (typeof(*original_row_positions)) malloc(m * sizeof(**original_row_positions));
	csr_reorder_matrix_by_row(m, n, nnz, row_ptr, col_idx, val, membership, row_ptr_reorder_r, col_idx_reorder_r, val_reorder_r, *original_row_positions);

	free(membership);
}

/*
float dot_product(float *a, float *b, int size) {
	float dot = 0.0;
	for (int i = 0; i < size; ++i)
		dot += a[i] * b[i];
	return dot;
}

float magnitude(float *a, int size) {
	float mag = 0.0;
	for (int i = 0; i < size; ++i)
		mag += a[i] * a[i];
	return sqrt(mag);
}

float cosine_similarity(float *a, float *b, int size) {
	float dot = dot_product(a, b, size);
	float mag_a = magnitude(a, size);
	float mag_b = magnitude(b, size);
	
	if (mag_a == 0 || mag_b == 0)
		return 0.0;

	return dot / (mag_a * mag_b);
}
*/

#undef  csr_kmeans_reorder_row_batch
#define csr_kmeans_reorder_row_batch  CSR_REORDER_GEN_EXPAND(csr_kmeans_reorder_row_batch)
void
csr_kmeans_reorder_row_batch(_TYPE_I * row_ptr, _TYPE_I * col_idx, _TYPE_V * val, 
							 _TYPE_I * row_ptr_reorder_r, _TYPE_I * col_idx_reorder_r, _TYPE_V * val_reorder_r, _TYPE_I ** original_row_positions,
							 int m, int n, int nnz, int batch, 
							 int numClusters, float threshold, int loop_threshold, int num_windows, int seed, _TYPE_I * rc_r, _TYPE_I * rc_c, float * rc_v)
{
	int m_batch = (m-1 + batch) / batch; 
	int numObjs = m_batch;
	int numCoords = num_windows;
	int * membership = (typeof(membership)) malloc(numObjs * sizeof(*membership));

	float * clusters = (typeof(clusters)) malloc(numClusters * numCoords * sizeof(*clusters));
	random_selection_csr(rc_r, rc_c, rc_v, clusters, numObjs, numClusters, numCoords, seed);
	// float max_value = -1;
	// for(int i=0; i<numObjs; i++)
	// 	if(max_value < rc_v[i])
	// 		max_value = rc_v[i];
	// printf("max_value = %lf\n", max_value);
	// diagonal_selection_csr(clusters, numClusters, numCoords, max_value);

	int type = 0; // cosine similarity
	double time_kmeans2_csr = time_it(1,
	// kmeans(row_cross, numCoords, numObjs, numClusters, threshold, loop_threshold, membership, clusters, type);
	kmeans2_csr(rc_r, rc_c, rc_v, numCoords, numObjs, numClusters, threshold, loop_threshold, membership, clusters, type);
	);
	printf("\ntime_kmeans2_csr = %lf (numObjs = %d, numCoords = %d, numClusters = %d)\n", time_kmeans2_csr, numObjs, numCoords, numClusters);

	// Calculate cosine similarities
	// float similarities[numClusters][numClusters];
	// for (int i = 0; i < numClusters; ++i)
	// 	for (int j = 0; j < numClusters; ++j)
	// 		similarities[i][j] = cosine_similarity(&clusters[i*numCoords], &clusters[j*numCoords], numCoords);

	// for (int i=0; i<numClusters; i++){
	// 	printf("cluster[%3d] = [ ", i);
	// 	for (int j=0; j<numCoords; j++)
	// 		printf("%.2f ", clusters[i*numCoords + j]);
	// 	printf("]\n");
	// }

	// // char * binary_string[numClusters];
	// binary_string_with_id clusters_binary[numClusters];

	// for(int i = 0; i < numClusters; i++){
	// 	clusters_binary[i].binary_string = convert_float_to_binary_string(&clusters[i * numCoords], numCoords);
	// 	clusters_binary[i].id = i;
	// 	// printf("cluster_b[%3d]: %s\n", i, clusters_binary[i].binary_string);
	// }
	// free(clusters);

	// // Sort the array of binary strings
	// qsort(clusters_binary, numClusters, sizeof(binary_string_with_id), compare_binary_strings);

	// // printf("after sorting...\n");
	// // for (int i = 0; i < numClusters; i++)
	// // 	printf("cluster_b[%3d]: %s (%d)\n", i, clusters_binary[i].binary_string, clusters_binary[i].id);

	// for (int i = 0; i < numClusters; i++)
	// 	free(clusters_binary[i].binary_string);

	int * ordered_membership = (typeof(ordered_membership)) calloc(numClusters, sizeof(*ordered_membership));
	// for (int i = 0; i < numClusters; i++)
	// 	ordered_membership[clusters_binary[i].id] = i;

	// for(int i = 0; i < numObjs; i++){
	// 	membership[i] = ordered_membership[membership[i]];
	// 	ordered_membership[membership[i]] = 0;
	// }
	for(int i=0;i<numObjs;i++)
		ordered_membership[membership[i]]++;
	// for(int i=0;i<numClusters;i++)
	// 	printf("I = %d\t%d\t( %.2f%% )\n", i, ordered_membership[i], ordered_membership[i]*100.0/numObjs);
	free(ordered_membership);

	*original_row_positions = (typeof(*original_row_positions)) malloc(m * sizeof(**original_row_positions));
	double time_row_reorder = time_it(1,
	csr_reorder_matrix_by_row_batch(m, n, nnz, batch, row_ptr, col_idx, val, membership, row_ptr_reorder_r, col_idx_reorder_r, val_reorder_r, *original_row_positions);
	);
	printf("time_reorder_by_row = %lf\n", time_row_reorder);

	free(membership);
}

/*****************************************/
// Struct definition for row subgroups
typedef struct row_subgroup {
	int col_start;	// Starting column index
	int col_finish;	// Finishing column index
	int population;	// Number of rows that belong to the subgroup
} row_subgroup;


int belongs_to_row_subgroup(int col_start, int col_finish, row_subgroup subgroup, float threshold) {
    // Check if col_start and col_finish fall within the bounds of the subgroup
    if (col_start >= (subgroup.col_start*1.0*(1-threshold)) && col_finish <= (subgroup.col_finish*1.0*(1+threshold)))
        return 1; // Row belongs to subgroup
    return 0; // Row does not belong to subgroup
}

void update_or_create_row_subgroup(int row, int col_start, int col_finish, row_subgroup subgroups[], int * num_subgroups, float threshold){
	for (int i = 0; i < *num_subgroups; i++) {
		// Check if the row belongs to any existing subgroup
		if (belongs_to_row_subgroup(col_start, col_finish, subgroups[i], threshold)) {
			// Increment the population count of the subgroup
			subgroups[i].population++;
			if(col_start < subgroups[i].col_start)
				subgroups[i].col_start = col_start;
			if(col_finish > subgroups[i].col_finish)
				subgroups[i].col_finish = col_finish;
			return;
		}
	}

	// If the row doesn't belong to any existing subgroup, create a new one
	subgroups[*num_subgroups].col_start = col_start;
	subgroups[*num_subgroups].col_finish = col_finish;
	subgroups[*num_subgroups].population = 1; // Initialize population count to 1
	(*num_subgroups)++;
	printf("\t\t>>> row %d (start = %d, finish = %d) created a new subgroup (#%d)\n", row, col_start, col_finish, *num_subgroups);
}

#undef  csr_extract_subgroups
#define csr_extract_subgroups  CSR_REORDER_GEN_EXPAND(csr_extract_subgroups)
void
csr_extract_subgroups(_TYPE_I * row_ptr, _TYPE_I * col_idx, _TYPE_V * val, 
					  int m, int n, int nnz, float threshold)
{
	// Assuming a maximum of 1000 subgroups
	row_subgroup subgroups[1000];
	int extra_population = 0, extra_population2 = 0;

	// TODO: need to fix this, so that it account for non_empty_rows only when calculating bw values.
	_TYPE_I * bandwidth_rows = (typeof(bandwidth_rows)) malloc(m * sizeof(*bandwidth_rows));
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

	free(bandwidth_rows);

	double bandwidth_selection = bandwidth_avg;
	if(bandwidth_selection > 131072)
		bandwidth_selection = 131072;

	// subgroup[0] will be reserved for larger than avg bw rows
	int num_subgroups = 0;
	for(int i=0; i<m; i++){
		int col_start = col_idx[row_ptr[i]];
		int col_finish = col_idx[row_ptr[i+1] - 1];
		int row_size = row_ptr[i+1] - row_ptr[i];
		if(row_size < 1)
			extra_population2++;
		else{
			if((col_finish - col_start) > bandwidth_selection)
				extra_population++;
			else
				update_or_create_row_subgroup(i, col_start, col_finish, subgroups, &num_subgroups, threshold);
		}
	}
	printf("extra_population = %d (%.4lf \%)\n", extra_population, extra_population*100.0/m);
	printf("empty            = %d (%.4lf \%)\n", extra_population2, extra_population2*100.0/n);
	printf("in the end, there are %d subgroups\n", num_subgroups);
	for (int i = 0; i < num_subgroups; i++) 
		printf("subgroup #%d: %d - %d\t width %d\t(population %d - %.4lf %)\n", i+1, subgroups[i].col_start, subgroups[i].col_finish, subgroups[i].col_finish-subgroups[i].col_start ,subgroups[i].population, subgroups[i].population*100.0/m);
}

/*****************************************/

/* csr_kmeans_reorder_row util functions */
void random_selection_char_csr(_TYPE_I * start_rc_r, _TYPE_I * start_rc_c, unsigned char * start_rc_v, unsigned char * target_array, int numObjs, int numClusters, int numCoords, int seed)
{
	int * indices = (int *) malloc(numObjs * sizeof(int));
	for(int i = 0; i < numObjs; i++)
		indices[i] = i;
	srand(seed);
	for(int i = 0; i < numClusters ; i++) {
		int k = rand() % (numObjs - i);
		unsigned char *start_array = (typeof(start_array)) calloc(numCoords, sizeof(*start_array));
		for(int j = start_rc_r[indices[k]]; j < start_rc_r[indices[k]+1]; j++)
			start_array[start_rc_c[j]] = start_rc_v[j];

		for(int j = 0; j < numCoords; j++)
			target_array[i * numCoords + j] = start_array[j];
			// target_array[i * numCoords + j] = start_array[indices[k] * numCoords + j];
		indices[k] = indices[numCoords - i - 1];
		free(start_array);
	}
	free(indices);
	for (int i=0; i<numClusters; i++){
		printf("cluster[%3d] = [ ", i);
		for (int j=0; j<numCoords; j++)
			printf("%d", target_array[i*numCoords + j]);
		printf("]\n");
	}
}

#undef  csr_kmeans_char_reorder_row
#define csr_kmeans_char_reorder_row  CSR_REORDER_GEN_EXPAND(csr_kmeans_char_reorder_row)
void
csr_kmeans_char_reorder_row(_TYPE_I * row_ptr, _TYPE_I * col_idx, _TYPE_V * val, 
							_TYPE_I * row_ptr_reorder_r, _TYPE_I * col_idx_reorder_r, _TYPE_V * val_reorder_r, _TYPE_I ** original_row_positions,
							int m, int n, int nnz, 
							int numClusters, float threshold, int loop_threshold, int num_windows, int seed, _TYPE_I * rc_r, _TYPE_I * rc_c, unsigned char * rc_v)
{
	int numObjs = m;
	int numCoords = num_windows;
	int * membership = (typeof(membership)) malloc(numObjs * sizeof(*membership));

	unsigned char * clusters = (typeof(clusters)) malloc(numClusters * numCoords * sizeof(*clusters));
	random_selection_char_csr(rc_r, rc_c, rc_v, clusters, numObjs, numClusters, numCoords, seed);

	int type = 0; // cosine similarity
	double time_kmeans2_csr = time_it(1,
	// kmeans(row_cross, numCoords, numObjs, numClusters, threshold, loop_threshold, membership, clusters, type);
	kmeans_char2_csr(rc_r, rc_c, rc_v, numCoords, numObjs, numClusters, threshold, loop_threshold, membership, clusters, type);
	);
	printf("\ntime_kmeans2_csr = %lf (numObjs = %d, numCoords = %d, numClusters = %d)\n", time_kmeans2_csr, numObjs, numCoords, numClusters);

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

	for(int i = 0; i < numObjs; i++){
		membership[i] = ordered_membership[membership[i]];
		ordered_membership[membership[i]] = 0;
	}
	for(int i=0;i<numObjs;i++)
		ordered_membership[membership[i]]++;
	// for(int i=0;i<numClusters;i++)
	// 	printf("I = %d\t%d\t( %.2f%% )\n", i, ordered_membership[i], ordered_membership[i]*100.0/numObjs);
	free(ordered_membership);

	*original_row_positions = (typeof(*original_row_positions)) malloc(m * sizeof(**original_row_positions));
	double time_row_reorder = time_it(1,
	csr_reorder_matrix_by_row(m, n, nnz, row_ptr, col_idx, val, membership, row_ptr_reorder_r, col_idx_reorder_r, val_reorder_r, *original_row_positions);
	);
	printf("time_reorder_by_row = %lf\n", time_row_reorder);

	free(membership);
}

#undef  csr_kmeans_char_reorder_row_batch
#define csr_kmeans_char_reorder_row_batch  CSR_REORDER_GEN_EXPAND(csr_kmeans_char_reorder_row_batch)
void
csr_kmeans_char_reorder_row_batch(_TYPE_I * row_ptr, _TYPE_I * col_idx, _TYPE_V * val, 
								  _TYPE_I * row_ptr_reorder_r, _TYPE_I * col_idx_reorder_r, _TYPE_V * val_reorder_r, _TYPE_I ** original_row_positions,
								  int m, int n, int nnz, int batch, 
								  int numClusters, float threshold, int loop_threshold, int num_windows, int seed, _TYPE_I * rc_r, _TYPE_I * rc_c, unsigned char * rc_v)
{
	int m_batch = (m-1 + batch) / batch; 
	int numObjs = m_batch;
	int numCoords = num_windows;
	int * membership = (typeof(membership)) malloc(numObjs * sizeof(*membership));

	unsigned char * clusters = (typeof(clusters)) malloc(numClusters * numCoords * sizeof(*clusters));
	random_selection_char_csr(rc_r, rc_c, rc_v, clusters, numObjs, numClusters, numCoords, seed);

	int type = 0; // cosine similarity
	double time_kmeans2_csr = time_it(1,
	// kmeans(row_cross, numCoords, numObjs, numClusters, threshold, loop_threshold, membership, clusters, type);
	kmeans_char2_csr(rc_r, rc_c, rc_v, numCoords, numObjs, numClusters, threshold, loop_threshold, membership, clusters, type);
	);
	printf("\ntime_kmeans2_csr = %lf (numObjs = %d, numCoords = %d, numClusters = %d)\n", time_kmeans2_csr, numObjs, numCoords, numClusters);

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

	int * ordered_membership = (typeof(ordered_membership)) calloc(numClusters, sizeof(*ordered_membership));
	// for (int i = 0; i < numClusters; i++)
	// 	ordered_membership[clusters_binary[i].id] = i;

	// for(int i = 0; i < numObjs; i++){
	// 	membership[i] = ordered_membership[membership[i]];
	// 	ordered_membership[membership[i]] = 0;
	// }
	for(int i=0;i<numObjs;i++)
		ordered_membership[membership[i]]++;
	// for(int i=0;i<numClusters;i++)
	// 	printf("I = %d\t%d\t( %.2f%% )\n", i, ordered_membership[i], ordered_membership[i]*100.0/numObjs);
	free(ordered_membership);

	*original_row_positions = (typeof(*original_row_positions)) malloc(m * sizeof(**original_row_positions));
	double time_row_reorder = time_it(1,
	csr_reorder_matrix_by_row_batch(m, n, nnz, batch, row_ptr, col_idx, val, membership, row_ptr_reorder_r, col_idx_reorder_r, val_reorder_r, *original_row_positions);
	);
	printf("time_reorder_by_row = %lf\n", time_row_reorder);

	free(membership);
}

//==========================================================================================================================================
//= Includes Undefs
//==========================================================================================================================================


#include "sort/samplesort/samplesort_gen_pop.h"
#include "sort/quicksort/quicksort_gen_pop.h"
#include "functools/functools_gen_pop.h"

