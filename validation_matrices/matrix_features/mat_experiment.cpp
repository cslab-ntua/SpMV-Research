#include <stdio.h>
#include <stdlib.h>

// #include "read_coo_file.h"

#include "macros/cpp_defines.h"

#ifdef __cplusplus
extern "C"{
#endif
	#include "debug.h"
	#include "time_it.h"
	#include "string_util.h"
	#include "csr.h"
	#include "csc.h"
#ifdef __cplusplus
}
#endif

#include "read_mtx.h"
#include "parallel_util.h"

// Utils macro
#define Min(x,y) ((x)<(y)?(x):(y))
#define Max(x,y) ((x)>(y)?(x):(y))
#define Abs(x) ((x)>(0)?(x):-(x))

void replace_substring(char* str, const char* find, const char* replace)
{
	char* pos = strstr(str, find);
	if (pos != NULL) {
		size_t find_len = strlen(find);
		size_t replace_len = strlen(replace);
		size_t tail_len = strlen(pos + find_len);

		memmove(pos + replace_len, pos + find_len, tail_len + 1);
		memcpy(pos, replace, replace_len);
	}
}

char * fig_name_gen(const char * file_basename, const char * replace_str)
{
	long buf_n = 1000;
	char buf[buf_n];

	char * path, * filename, * filename_base;
	str_path_split_path(file_basename, strlen(file_basename) + 1, buf, buf_n, &path, &filename);

	path = strdup(path);
	filename = strdup(filename);
	char file_new[1000];
	char replace[1000];

	sprintf(file_new, "%s", file_basename);
	sprintf(replace, "_%s.mtx", replace_str);
	// replace_substring(file_new, ".mtx", replace);
	if(replace_str == NULL || replace_str[0] == '\0')
		replace_substring(file_new, ".mtx", "");
	else{
		replace_substring(file_new, ".mtx", "_|");
		replace_substring(file_new, "|", replace_str);
	}


	char * file_fig;
	str_path_split_path(file_new, strlen(file_new) + 1, buf, buf_n, &path, &filename);
	
	path = strdup(path);
	filename = strdup(filename);
	str_path_split_ext(filename, strlen(filename) + 1, buf, buf_n, &filename_base, NULL);
	filename_base = strdup(filename_base);
	snprintf(buf, buf_n, "figures/%s", filename_base);
	file_fig = strdup(buf);
	return file_fig;
}

char * mtx_name_gen(const char * file_basename, const char * replace_str)
{
	long buf_n = 1000;
	char buf[buf_n];

	char * path, * filename;
	str_path_split_path(file_basename, strlen(file_basename) + 1, buf, buf_n, &path, &filename);
	path = strdup(path);
	filename = strdup(filename);
	char file_new[1000];
	char replace[1000];

	sprintf(file_new, "%s", file_basename);
	sprintf(replace, "_%s.mtx", replace_str);
	replace_substring(file_new, ".mtx", replace);
	char * file_mtx;
	str_path_split_path(file_new, strlen(file_new) + 1, buf, buf_n, &path, &filename);
	
	path = strdup(path);
	filename = strdup(filename);
	snprintf(buf, buf_n, "matrices/%s", filename);
	file_mtx = strdup(buf);
	return file_mtx;
}

void feature_plot_store_matrix(char *file_in, const char *replace_str, int plot, int store, long m, long n, long nnz, _TYPE_I* row_ptr, _TYPE_I* col_idx, _TYPE_V* val)
{	
	int buf_n = 1000;
	char buf[buf_n];
	char * filename;

	long num_pixels = 1024;
	long num_pixels_x = (n < num_pixels) ? n : num_pixels;
	long num_pixels_y = (m < num_pixels) ? m : num_pixels;

	str_path_split_path(file_in, strlen(file_in) + 1, buf, buf_n, NULL, &filename);
	if(replace_str == NULL || replace_str[0] == '\0')
		replace_substring(filename, ".mtx", "");
	else{
		replace_substring(filename, ".mtx", "_|");
		replace_substring(filename, "|", replace_str);
	}
	
	csr_matrix_features_validation(filename, row_ptr, col_idx, m, n, nnz);

	if(plot){
		char * file_fig = fig_name_gen(file_in, replace_str);
		csr_plot_f(file_fig, row_ptr, col_idx, val, m, n, nnz, 0, num_pixels_x, num_pixels_y);

		// several other plotting options are here...
		// csr_row_size_histogram_plot(file_fig, row_ptr, col_idx, val, m, n, nnz, 0);
		// int window_size = 8192;
		// csr_cross_row_similarity_histogram_plot(file_fig, row_ptr, col_idx, val, m, n, nnz, window_size, 0);
		// csr_num_neigh_histogram_plot(file_fig, row_ptr, col_idx, val, m, n, nnz, window_size, 0);
	}

	if(store){
		char * file_new_mat = mtx_name_gen(file_in, replace_str);
		double time = time_it(1,
		csr_save_to_mtx(row_ptr, col_idx, val, m, n, file_new_mat);
		);
		printf("time for csr_save_to_mtx = %lf\n", time);
	}
}

// Comparison function for qsort
int compare_int(const void *a, const void *b) {
	return (*(int *)a - *(int *)b);
}

void generate_high_only_same_col_ind_matrix(int nr_rows, int nr_cols, int high_nnz, int row_ratio, int col_window, int seed, 
	_TYPE_I * row_ptr, _TYPE_I * col_idx, _TYPE_V * val) 
{
	double density = ((double) high_nnz) / ((double) nr_cols);
	printf("%lf%%\n", density * 100);

	int total_nnz = nr_rows * high_nnz;
	double mem_footprint = (total_nnz * (sizeof(_TYPE_V) + sizeof(_TYPE_I)) + (nr_rows + 1) * sizeof(_TYPE_I)) / (1024.0 * 1024);
	printf("nr_rows %d\ttotal_nnz %d\tmem_footprint %.2f MB\n", nr_rows, total_nnz, mem_footprint);

	// int step_size = (N - col_window) / (row_ratio - 1);
	int step_size = (nr_cols - col_window) / (row_ratio);
	int step_size_row = nr_rows / (row_ratio);
	printf("step_size = %d\n", step_size);
	printf("step_size_row = %d\n", step_size_row);

	row_ptr[0] = 0;
	for (int i = 0; i < row_ratio; i++) {
		_TYPE_I start_row = i * step_size_row + 1;
		if(i==row_ratio-1)
			step_size_row = nr_rows - start_row + 1;
		// printf("thread_num = %d\ti = %d\ti = %d\t ( %d - %d)\n", thread_num, i, i, start_row, start_row+step_size_row);

		_TYPE_I array_range[col_window];
		for (int j = 0; j < col_window; j++)
			array_range[j] = i * step_size + j;

		srand(seed + i * 10);
		for (int j = 0; j < high_nnz; j++) {
			int rand_index = rand() % col_window;
			int temp = array_range[rand_index];
			for(int k = 0; k < j; k++){
				if(temp == array_range[k])
					srand(seed + i + 8);
				rand_index = rand() % col_window;
				temp = array_range[rand_index];
			}
			array_range[rand_index] = array_range[j];
			array_range[j] = temp;
		}
		qsort(array_range, high_nnz, sizeof(_TYPE_I), compare_int);

		for(int k = 0; k < step_size_row; k++){
			for (int j = 0; j < high_nnz; j++) {
				_TYPE_I index = (start_row + k - 1) * high_nnz + j;
				col_idx[index] = array_range[j];
				val[index]  = ((_TYPE_V)rand() / RAND_MAX);
			}
			row_ptr[start_row + k] = (start_row + k) * high_nnz;
		}
	}
}

int main(int argc, char **argv)
{
	int n, m, nnz;
	ValueType * mtx_val;
	int * mtx_rowind;
	int * mtx_colind;

	_TYPE_I * row_ptr; //for CSR format
	_TYPE_I * col_idx;
	_TYPE_V * val;

	_TYPE_I * row_ptr_init; //for CSR format
	_TYPE_I * col_idx_init;
	_TYPE_V * val_init;


	int i;

	char *file_in_init;
	file_in_init = (char*) malloc(100*sizeof(char));
	char *file_in;
	file_in = (char*) malloc(100*sizeof(char));
	char *filename;
	filename = (char*) malloc(100*sizeof(char));

	i=2;
	int plot = atoi(argv[i++]);
	int store = atoi(argv[i++]);
	long nnz_threshold = atoi(argv[i++]);
	int split_matrix = atoi(argv[i++]);
	int sort_rows = atoi(argv[i++]);
	int separate=atoi(argv[i++]);
	int max_distance = atoi(argv[i++]);
	int shuffle = atoi(argv[i++]);
	int seed = atoi(argv[i++]);
	
	int artificial = 0;
	if(argc>15)
		artificial = atoi(argv[i++]);

	long num_pixels = 1024;
	long num_pixels_x = num_pixels;
	long num_pixels_y = num_pixels;

	if(!artificial) {
		file_in_init = argv[1];
		create_coo_matrix(file_in_init, &mtx_val, &mtx_rowind, &mtx_colind, &m, &n, &nnz);
		num_pixels_x = (n < num_pixels) ? n : num_pixels;
		num_pixels_y = (m < num_pixels) ? m : num_pixels;
		// row_ptr = (typeof(row_ptr)) malloc((m+1) * sizeof(*row_ptr));
		// col_idx = (typeof(col_idx)) malloc(nnz * sizeof(*col_idx));
		// val = (typeof(val)) malloc(nnz * sizeof(*val));
		row_ptr_init = (typeof(row_ptr_init)) malloc((m+1) * sizeof(*row_ptr_init));
		col_idx_init = (typeof(col_idx_init)) malloc(nnz * sizeof(*col_idx_init));
		val_init = (typeof(val_init)) malloc(nnz * sizeof(*val_init));

		// coo_to_csr(mtx_rowind, mtx_colind, mtx_val, m, n, nnz, row_ptr, col_idx, val, 1, 0);
		// csr_plot_f(fig_name_gen(file_in_init, ""),  row_ptr, col_idx, val, m, n, nnz, 0, num_pixels_x, num_pixels_y);
		coo_to_csr(mtx_rowind, mtx_colind, mtx_val, m, n, nnz, row_ptr_init, col_idx_init, val_init, 1, 0);
		csr_plot_f(fig_name_gen(file_in_init, ""),  row_ptr_init, col_idx_init, val_init, m, n, nnz, 0, num_pixels_x, num_pixels_y);

		row_ptr = row_ptr_init;
		col_idx = col_idx_init;
		val     = val_init;
		strcpy(file_in, file_in_init);

		if(shuffle){
			_TYPE_I * row_ptr_shuffle;
			_TYPE_I * col_idx_shuffle;
			_TYPE_V * val_shuffle;

			row_ptr_shuffle = (typeof(row_ptr_shuffle)) malloc((m+1) * sizeof(*row_ptr_shuffle));
			col_idx_shuffle = (typeof(col_idx_shuffle)) malloc(nnz * sizeof(*col_idx_shuffle));
			val_shuffle = (typeof(val_shuffle)) malloc(nnz * sizeof(*val_shuffle));
			csr_shuffle_matrix(m, row_ptr_init, col_idx_init, val_init, row_ptr_shuffle, col_idx_shuffle, val_shuffle);

			feature_plot_store_matrix(file_in, "shuffle", 1, 0, m, n, nnz, row_ptr_shuffle, col_idx_shuffle, val_shuffle);

			row_ptr = row_ptr_shuffle;
			col_idx = col_idx_shuffle;
			val     = val_shuffle;

			// expand file_in to contain "shuffle" string too
			char *dot = strrchr(file_in, '.');
			if(dot != NULL)
				strcpy(dot, "_shuffle.mtx");
			else
				strcat(file_in, "_shuffle.mtx");
		}
		
		/********************************************************************************************************************************/










		_TYPE_I * rc_r, * rc_c;
		float * rc_v;

		int window_width;
		int num_windows_row;
		window_width = atoi(argv[i++]);
		int batch = atoi(argv[i++]);

		double time_row_cross2 = time_it(1,
		csr_extract_row_cross2_batch(row_ptr, col_idx, val, m, n, nnz, window_width, batch, &num_windows_row, &rc_r, &rc_c, &rc_v);
		);
		printf("time_row_cross2 = %lf\n", time_row_cross2);

		_TYPE_I * row_ptr_reorder_r; //for CSR format
		_TYPE_I * col_idx_reorder_r;
		_TYPE_V * val_reorder_r;
		_TYPE_I * original_row_positions; // this will be used to reorder output (y) vector elements, in order to produce same result as pre-reordered matrix

		row_ptr_reorder_r = (typeof(row_ptr_reorder_r)) malloc((m+1) * sizeof(*row_ptr_reorder_r));
		col_idx_reorder_r = (typeof(col_idx_reorder_r)) malloc(nnz * sizeof(*col_idx_reorder_r));
		val_reorder_r = (typeof(val_reorder_r)) malloc(nnz * sizeof(*val_reorder_r));

		int numClusters = atoi(argv[i++]);
		float threshold = atof(argv[i++]);;
		int loop_threshold = 150;
		double time_row_reorder = time_it(1,
		csr_kmeans_reorder_row_batch(row_ptr, col_idx, val, row_ptr_reorder_r, col_idx_reorder_r, val_reorder_r, &original_row_positions, m, n, nnz, batch, numClusters, threshold, loop_threshold, num_windows_row, seed, rc_r, rc_c, rc_v);
		);
		printf("time_row_reorder (total) = %lf\n", time_row_reorder);
		free(rc_r);
		free(rc_c);
		free(rc_v);






		// for(int i=0;i<10;i++)
		// 	printf("original_row_positions %d -> %d\n", i, original_row_positions[i]);


















		char * membership_suffix;
		membership_suffix = (typeof(membership_suffix)) malloc(100*sizeof(*membership_suffix));
		sprintf(membership_suffix, "row_reordered_window_%d_batch_%d_clusters_%d", window_width, batch, numClusters);
		feature_plot_store_matrix(file_in, membership_suffix, 1, store, m, n, nnz, row_ptr_reorder_r, col_idx_reorder_r, val_reorder_r);
		free(membership_suffix);

		_TYPE_I * row_indices_reorder_r; //for CSC format
		_TYPE_I * row_idx_reorder_r; //for CSR format
		_TYPE_I * col_ptr_reorder_r;
		_TYPE_V * val_c_reorder_r;

		row_indices_reorder_r = (typeof(row_indices_reorder_r)) malloc(nnz * sizeof(*row_indices_reorder_r));
		csr_row_indices(row_ptr_reorder_r, col_idx_reorder_r, m, n, nnz, &row_indices_reorder_r);
		free(row_ptr_reorder_r);

		row_idx_reorder_r = (typeof(row_idx_reorder_r)) malloc(nnz * sizeof(*row_idx_reorder_r));
		col_ptr_reorder_r = (typeof(col_ptr_reorder_r)) malloc((n+1) * sizeof(*col_ptr_reorder_r));
		val_c_reorder_r = (typeof(val_c_reorder_r)) malloc(nnz * sizeof(*val_c_reorder_r));
		coo_to_csc(row_indices_reorder_r, col_idx_reorder_r, val_reorder_r, m, n, nnz, row_idx_reorder_r, col_ptr_reorder_r, val_c_reorder_r, 1);
		free(row_indices_reorder_r);
		free(col_idx_reorder_r);
		free(val_reorder_r);

		int num_windows_col;
		_TYPE_I * cc_r, * cc_c;
		float * cc_v;

		double time_col_cross2 = time_it(1,
		csc_extract_col_cross2_batch(row_idx_reorder_r, col_ptr_reorder_r, val_c_reorder_r, m, n, nnz, window_width, batch, &num_windows_col, &cc_r, &cc_c, &cc_v);
		);
		printf("time_col_cross2 = %lf\n", time_col_cross2);

		_TYPE_I * row_idx_reorder_rc; //for CSC format
		_TYPE_I * col_indices_reorder_rc; 
		_TYPE_I * col_ptr_reorder_rc;
		_TYPE_V * val_c_reorder_rc;
		_TYPE_I * original_col_positions; // this will be used to reorder input (x) vector elements, in order to produce same result as pre-reordered matrix
		
		row_idx_reorder_rc = (typeof(row_idx_reorder_rc)) malloc(nnz * sizeof(*row_idx_reorder_rc));
		col_ptr_reorder_rc = (typeof(col_ptr_reorder_rc)) malloc((n+1) * sizeof(*col_ptr_reorder_rc));
		val_c_reorder_rc = (typeof(val_c_reorder_rc)) malloc(nnz * sizeof(*val_c_reorder_rc));

		double time_col_reorder = time_it(1,
		csc_kmeans_reorder_col_batch(row_idx_reorder_r, col_ptr_reorder_r, val_c_reorder_r, row_idx_reorder_rc, col_ptr_reorder_rc, val_c_reorder_rc, &original_col_positions, m, n, nnz, batch, numClusters, threshold, loop_threshold, num_windows_col, cc_r, cc_c, cc_v);
		);
		printf("time_col_reorder (total) = %lf\n", time_col_reorder);
		free(cc_r);
		free(cc_c);
		free(cc_v);

		// for(int i=0;i<10;i++)
		// 	printf("original_col_positions %d -> %d\n", i, original_col_positions[i]);

		free(row_idx_reorder_r);
		free(col_ptr_reorder_r);
		free(val_c_reorder_r);

		membership_suffix = (typeof(membership_suffix)) malloc(100*sizeof(*membership_suffix));
		sprintf(membership_suffix, "row_col_reordered_window_%d_batch_%d_clusters_%d", window_width, batch, numClusters);
		csc_plot_f(fig_name_gen(file_in, membership_suffix),  row_idx_reorder_rc, col_ptr_reorder_rc, val_c_reorder_rc, m, n, nnz, 0, num_pixels_x, num_pixels_y);

		col_indices_reorder_rc = (typeof(col_indices_reorder_rc)) malloc(nnz * sizeof(*col_indices_reorder_rc));
		csc_col_indices(row_idx_reorder_rc, col_ptr_reorder_rc, m, n, nnz, &col_indices_reorder_rc);
		free(col_ptr_reorder_rc);

		_TYPE_I * row_ptr_reorder_rc; //for CSR format
		_TYPE_I * col_idx_reorder_rc;
		_TYPE_V * val_reorder_rc;

		row_ptr_reorder_rc = (typeof(row_ptr_reorder_rc)) malloc((m+1) * sizeof(*row_ptr_reorder_rc));
		col_idx_reorder_rc = (typeof(col_idx_reorder_rc)) malloc(nnz * sizeof(*col_idx_reorder_rc));
		val_reorder_rc = (typeof(val_reorder_rc)) malloc(nnz * sizeof(*val_reorder_rc));

		coo_to_csr(row_idx_reorder_rc, col_indices_reorder_rc, val_c_reorder_rc, m, n, nnz, row_ptr_reorder_rc, col_idx_reorder_rc, val_reorder_rc, 1, 0);
		free(row_idx_reorder_rc);
		free(col_indices_reorder_rc);
		free(val_c_reorder_rc);

		feature_plot_store_matrix(file_in, membership_suffix, 1, store, m, n, nnz, row_ptr_reorder_rc, col_idx_reorder_rc, val_reorder_rc);
		free(membership_suffix);

		/********************************************************************************************************/
		/* Verify correctness of method... We have to reorder the x and y vectors accordingly
		 * x_reordered[i] = x[original_col_positions[i]]
		 * y_reordered[i] = y[original_row_positions[i]]
		 * regarding the final result, we have to do the reverse of this in an SpMV benchmark code in order to obtain the correct result
		 */
		if(0) {
			_TYPE_V * x, * y_gold;
			x = (typeof(x)) malloc(n * sizeof(*x));
			srand(14);
			for(int i=0; i<n; i++)
				x[i] = ((_TYPE_V)rand() / RAND_MAX);

			y_gold = (typeof(y_gold)) malloc(m * sizeof(*y_gold));
			_TYPE_V sum = 0;
			#pragma omp for
			for (int i=0;i<m;i++)
			{
				y_gold[i] = 0;
				_TYPE_V value, tmp, compensation;
				compensation = 0;
				sum = 0;
				for (int j=row_ptr[i];j<row_ptr[i+1];j++)
				{
					value = val[j] * x[col_idx[j]] - compensation;
					tmp = sum + value;
					compensation = (tmp - sum) - value;
					sum = tmp;
				}
				y_gold[i] = sum;
			}

			_TYPE_V * x_reordered, * y_reordered;
			x_reordered = (typeof(x_reordered)) malloc(n * sizeof(*x_reordered));
			for(int i=0; i<n; i++)
				x_reordered[i] = x[original_col_positions[i]];

			y_reordered = (typeof(y_reordered)) malloc(m * sizeof(*y_reordered));
			sum = 0;
			#pragma omp for
			for (int i=0;i<m;i++)
			{
				y_reordered[i] = 0;
				_TYPE_V value, tmp, compensation;
				compensation = 0;
				sum = 0;
				for (int j=row_ptr_reorder_rc[i];j<row_ptr_reorder_rc[i+1];j++)
				{
					value = val_reorder_rc[j] * x_reordered[col_idx_reorder_rc[j]] - compensation;
					tmp = sum + value;
					compensation = (tmp - sum) - value;
					sum = tmp;
				}
				y_reordered[i] = sum;
			}

			_TYPE_V epsilon = 1e-10, maxDiff = 0, diff;
			for(int i=0;i<m;i++)
			{
				_TYPE_V y_gold_reordered = y_gold[original_row_positions[i]];
				diff = Abs(y_gold_reordered - y_reordered[i]);
				if (y_gold_reordered > epsilon)
				{
					diff = diff / abs(y_gold_reordered);
					maxDiff = Max(maxDiff, diff);
				}
			}
			if(maxDiff > epsilon)
				printf("Test failed!\n");
			else
				printf("Test passed!\n");

			free(x);
			free(x_reordered);
			free(y_gold);
			free(y_reordered);
		}
		/********************************************************************************************************/

		free(row_ptr_reorder_rc);
		free(col_idx_reorder_rc);
		free(val_reorder_rc);

		free(original_row_positions);
		free(original_col_positions);

		/********************************************************************************************************************************/
		free(mtx_rowind);
		free(mtx_colind);
		filename = file_in;
	}
	else {
		m = atoi(argv[i++]);
		n = atoi(argv[i++]);
		int high_nnz = atoi(argv[i++]);
		int row_ratio = atoi(argv[i++]);
		int col_window = atoi(argv[i++]);
		int seed = 14;

		nnz = m*high_nnz;

		row_ptr = (typeof(row_ptr)) malloc((m+1) * sizeof(*row_ptr));
		col_idx = (typeof(col_idx)) malloc(nnz * sizeof(*col_idx));
		val = (typeof(val)) malloc(nnz * sizeof(*val));

		double time = time_it(1,
			generate_high_only_same_col_ind_matrix(m, n, high_nnz, row_ratio, col_window, seed, row_ptr, col_idx, val);
		);
		printf("time for generate_high_only_same_col_ind_matrix = %lf\n", time);

		sprintf(filename, "high_same_col_ind_N%d_r%d_cw%d_nnz%d_s%d.mtx", n, row_ratio, col_window, high_nnz, seed);
		printf("%s\n", filename);

		file_in = filename;
	}

	num_pixels_x = (n < num_pixels) ? n : num_pixels;
	num_pixels_y = (m < num_pixels) ? m : num_pixels;

	feature_plot_store_matrix(file_in, "", plot, 0, m, n, nnz, row_ptr, col_idx, val);
	printf("mem footprint %lf\n", (sizeof(_TYPE_I)*(m+1) + (sizeof(_TYPE_V)+sizeof(_TYPE_I))*nnz)/(1024.0*1024));

	/*********************************************************************************************/
	if(0){
		printf("total nonzeros = %d\n", nnz);
		printf("row_ptr = [ ");
		// for(i=0; i<m+1; i++){
		int threshold=5;
		for(i=m-threshold; i<m+1; i++){
			printf("%d ", row_ptr[i]);
		}
		printf("]\n");
		printf("col_idx = [ ");
		// for(i=0; i<m; i++){
		for(i=m-threshold; i<m+1; i++){
			for(int j=row_ptr[i]; j<row_ptr[i+1]; j++){
				printf("%d ", col_idx[j]);
			}
			printf("| ");
		}
		printf(" ]\n\n");
		printf("values = [ ");
		// for(i=0; i<m; i++){
		for(i=threshold; i<m+1; i++){
			for(int j=row_ptr[i]; j<row_ptr[i+1]; j++){
				// printf("%ld ", col_idx[j]);
				printf("values[%d]=%lf; ", j, val[j]);
			}
			// printf("| ");
		}
		printf(" ]\n\n");
	}

	/*********************************************************************************************/
	if(split_matrix){
		if(nnz_threshold != 0){
			printf("nnz_threshold = %ld\n", nnz_threshold);
			csr_count_short_rows(row_ptr, m, nnz_threshold);
		}

		_TYPE_I * row_ptr_cpu, * row_ptr_gpu;
		_TYPE_I * col_idx_cpu, * col_idx_gpu;
		_TYPE_V * val_cpu, * val_gpu;

		row_ptr_cpu = (typeof(row_ptr)) malloc((m+1) * sizeof(*row_ptr));
		row_ptr_cpu[0] = 0;
		col_idx_cpu = (typeof(col_idx)) malloc(nnz * sizeof(*col_idx));
		val_cpu = (typeof(val)) malloc(nnz * sizeof(*val));

		row_ptr_gpu = (typeof(row_ptr)) malloc((m+1) * sizeof(*row_ptr));
		row_ptr_gpu[0] = 0;
		col_idx_gpu = (typeof(col_idx)) malloc(nnz * sizeof(*col_idx));
		val_gpu = (typeof(val)) malloc(nnz * sizeof(*val));

		long k1=0, k2=0, count_cpu=0, count_gpu=0;
		// #pragma omp parallel
		{
			// #pragma omp for
			for(int i=0; i<m; i++){
				long row_nonzeros = row_ptr[i+1] - row_ptr[i];
				// gpu part
				if(row_nonzeros >= nnz_threshold){
					count_gpu++;
					for(int j=row_ptr[i]; j<row_ptr[i+1]; j++, k1++){
						col_idx_gpu[k1] = col_idx[j];
						val_gpu[k1] = val[j];
					}
					row_ptr_gpu[i+1] = k1;
					row_ptr_cpu[i+1] = row_ptr_cpu[i];
				}
				// cpu part
				else{
					count_cpu++;
					for(int j=row_ptr[i]; j<row_ptr[i+1]; j++, k2++){
						col_idx_cpu[k2] = col_idx[j];
						val_cpu[k2] = val[j];
					}
					row_ptr_cpu[i+1] = k2;
					row_ptr_gpu[i+1] = row_ptr_gpu[i];
				}
			}
		}
		// printf("count_cpu = %ld, row_ptr_cpu[m] = %d, k2 = %ld\n", count_cpu, row_ptr_cpu[m], k2);
		// printf("count_gpu = %ld, row_ptr_gpu[m] = %d, k1 = %ld\n", count_gpu, row_ptr_gpu[m], k1);
		printf("original : %d nonzeros, (%.2lf MB)\n", nnz, (sizeof(int)*(m+1) + (sizeof(double)+sizeof(int))*nnz)/(1024.0*1024));
		printf("CPU part : %d nonzeros, (%.2lf MB)\n", row_ptr_cpu[m], (sizeof(int)*(m+1) + (sizeof(double)+sizeof(int))*row_ptr_cpu[m])/(1024.0*1024));
		printf("GPU part : %d nonzeros, (%.2lf MB)\n", row_ptr_gpu[m], (sizeof(int)*(m+1) + (sizeof(double)+sizeof(int))*row_ptr_gpu[m])/(1024.0*1024));

		feature_plot_store_matrix(file_in, "cpu", plot, 0, m, n, row_ptr_cpu[m], row_ptr_cpu, col_idx_cpu, val_cpu);
		free(row_ptr_cpu);
		free(col_idx_cpu);
		free(val_cpu);
		
		feature_plot_store_matrix(file_in, "gpu", plot, 0, m, n, row_ptr_gpu[m], row_ptr_gpu, col_idx_gpu, val_gpu);
		free(row_ptr_gpu);
		free(col_idx_gpu);
		free(val_gpu);
	}

	/*********************************************************************************************/
	if(sort_rows){
		_TYPE_I * row_ptr_s;
		_TYPE_I * col_idx_s;
		_TYPE_V * val_s;

		row_ptr_s = (typeof(row_ptr_s)) malloc((m+1) * sizeof(*row_ptr_s));
		col_idx_s = (typeof(col_idx_s)) malloc(nnz * sizeof(*col_idx_s));
		val_s = (typeof(val_s)) malloc(nnz * sizeof(*val_s));
		printf("---> csr_sort_by_row_size START\n");
		csr_sort_by_row_size(m, row_ptr, col_idx, val, row_ptr_s, col_idx_s, val_s);
		printf("---> csr_sort_by_row_size   END\n");

		int * row_sizes_s = (typeof(row_sizes_s)) malloc((m) * sizeof(*row_sizes_s));
		#pragma omp parallel for
		for(i=0; i<m; i++){
			row_sizes_s[i] = row_ptr_s[i+1] - row_ptr_s[i];
		}

		feature_plot_store_matrix(file_in, "sorted_rows", plot, store, m, n, nnz, row_ptr_s, col_idx_s, val_s);
		free(row_ptr_s);
		free(col_idx_s);
		free(val_s);
		free(row_sizes_s);
	}

	/*********************************************************************************************/
	if(separate){
		_TYPE_I * row_ptr_close, * row_ptr_distant;
		_TYPE_I * col_idx_close, * col_idx_distant;
		_TYPE_V * val_close, * val_distant;

		int * distant_mark = (int *) calloc(nnz, sizeof(int));
		long dist_nnz = csr_mark_distant_nonzeros(row_ptr, col_idx, m, max_distance, distant_mark);
		printf("dist_nnz = %ld (%.2f)\n", dist_nnz, dist_nnz*100.0/nnz);

		row_ptr_close = (typeof(row_ptr)) malloc((m+1) * sizeof(*row_ptr));
		col_idx_close = (typeof(col_idx)) malloc((nnz-dist_nnz) * sizeof(*col_idx));
		val_close = (typeof(val)) malloc((nnz-dist_nnz) * sizeof(*val));
		
		row_ptr_distant = (typeof(row_ptr)) malloc((m+1) * sizeof(*row_ptr));
		col_idx_distant = (typeof(col_idx)) malloc(dist_nnz * sizeof(*col_idx));
		val_distant = (typeof(val)) malloc(dist_nnz * sizeof(*val));

		// Separate the CSR data into "close" and "distant"
		double time = time_it(1,
			csr_separate_close_distant(row_ptr, col_idx, val, distant_mark, nnz, m,
								   row_ptr_close, col_idx_close, val_close,
								   row_ptr_distant, col_idx_distant, val_distant);
		);
		printf("time for csr_separate_close_distant = %lf\n", time);
		free(distant_mark);
		
		printf("Size of  original matrix: %.2f MB\n", ((m+1)*sizeof(_TYPE_I) + nnz*(sizeof(_TYPE_I)+sizeof(_TYPE_V)))/(1024.0*1024));
		printf("Size of   \"close\" matrix: %.2f MB\n", ((m+1)*sizeof(_TYPE_I) + (nnz-dist_nnz)*(sizeof(_TYPE_I)+sizeof(_TYPE_V)))/(1024.0*1024));
		printf("Size of \"distant\" matrix: %.2f MB\n\n\n", ((m+1)*sizeof(_TYPE_I) + dist_nnz*(sizeof(_TYPE_I)+sizeof(_TYPE_V)))/(1024.0*1024));

		feature_plot_store_matrix(file_in, "close", plot, 0, m, n, nnz-dist_nnz, row_ptr_close, col_idx_close, val_close);
		free(row_ptr_close);
		free(col_idx_close);
		free(val_close);

		feature_plot_store_matrix(file_in, "distant", plot, 0, m, n, dist_nnz, row_ptr_distant, col_idx_distant, val_distant);
		free(row_ptr_distant);
		free(col_idx_distant);
		free(val_distant);
	}

	/*********************************************************************************************/
	// if(shuffle){
	// 	_TYPE_I * row_ptr_shuffle;
	// 	_TYPE_I * col_idx_shuffle;
	// 	_TYPE_V * val_shuffle;

	// 	row_ptr_shuffle = (typeof(row_ptr_shuffle)) malloc((m+1) * sizeof(*row_ptr_shuffle));
	// 	col_idx_shuffle = (typeof(col_idx_shuffle)) malloc(nnz * sizeof(*col_idx_shuffle));
	// 	val_shuffle = (typeof(val_shuffle)) malloc(nnz * sizeof(*val_shuffle));
	// 	printf("---> csr_shuffle_matrix START\n");
	// 	double time = time_it(1,
	// 		csr_shuffle_matrix(m, row_ptr, col_idx, val, row_ptr_shuffle, col_idx_shuffle, val_shuffle);
	// 	);
	// 	printf("time for shuffle = %lf\n", time);
	// 	printf("---> csr_shuffle_matrix   END\n");

	// 	feature_plot_store_matrix(file_in, "shuffle", plot, store, m, n, nnz, row_ptr_shuffle, col_idx_shuffle, val_shuffle);

	// 	free(row_ptr_shuffle);
	// 	free(col_idx_shuffle);
	// 	free(val_shuffle);
	// }

	/*********************************************************************************************/
	free(row_ptr);
	free(col_idx);
	free(val);

	free(filename);
	return 0;
}
