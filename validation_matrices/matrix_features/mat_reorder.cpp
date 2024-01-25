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

	int artificial = 0;
	if(argc>13)
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


		_TYPE_I * rc_r, * rc_c;
		float * rc_v;
		// _TYPE_I * cc_r, * cc_c;
		// float * cc_v;

		_TYPE_I * row_ptr_reorder_r; //for CSR format
		_TYPE_I * col_idx_reorder_r;
		_TYPE_V * val_reorder_r;
		_TYPE_I * original_row_positions; // this will be used to reorder output (y) vector elements, in order to produce same result as pre-reordered matrix

		row_ptr_reorder_r = (typeof(row_ptr_reorder_r)) malloc((m+1) * sizeof(*row_ptr_reorder_r));
		col_idx_reorder_r = (typeof(col_idx_reorder_r)) malloc(nnz * sizeof(*col_idx_reorder_r));
		val_reorder_r = (typeof(val_reorder_r)) malloc(nnz * sizeof(*val_reorder_r));

		long buf_n = 1000;
		char buf[buf_n];

		char * path, * filename, * filename_base;
		str_path_split_path(file_in, strlen(file_in) + 1, buf, buf_n, &path, &filename);
		replace_substring(filename, ".mtx", "");

		int numClusters = atoi(argv[i++]);
		int method = atoi(argv[i++]);

		char reorder_file[buf_n];
		if(method==0) // PaToH
			sprintf(reorder_file, "/various/pmpakos/sparcity-reordering/Matrix-Partitioning-Utility-master/PaToH/reorderings/PaToH_%s.mtx_cutpart_k%d_speed_s14_partvec.txt", filename, numClusters);
		else // METIS
			sprintf(reorder_file, "/various/pmpakos/sparcity-reordering/Matrix-Partitioning-Utility-master/METIS/reorderings/%s_metis_edge-cut_part%d.txt", filename, numClusters);
		
		csr_kmeans_reorder_by_file(reorder_file, row_ptr, col_idx, val, row_ptr_reorder_r, col_idx_reorder_r, val_reorder_r, &original_row_positions, m, n, nnz, numClusters, rc_r, rc_c, rc_v);
		free(rc_r);
		free(rc_c);
		free(rc_v);

		char * membership_suffix;
		membership_suffix = (typeof(membership_suffix)) malloc(100*sizeof(*membership_suffix));
		if(method==0) // PaToH
			sprintf(membership_suffix, "patoh_%d", numClusters);
		else // METIS
			sprintf(membership_suffix, "metis_%d", numClusters);
		feature_plot_store_matrix(file_in, membership_suffix, 1, store, m, n, nnz, row_ptr_reorder_r, col_idx_reorder_r, val_reorder_r);
		free(membership_suffix);

		free(row_ptr_reorder_r);
		free(col_idx_reorder_r);
		free(val_reorder_r);

		/********************************************************************************************************************************/
		free(mtx_rowind);
		free(mtx_colind);
		filename = file_in;
	}
	
	num_pixels_x = (n < num_pixels) ? n : num_pixels;
	num_pixels_y = (m < num_pixels) ? m : num_pixels;

	feature_plot_store_matrix(file_in, "", plot, 0, m, n, nnz, row_ptr, col_idx, val);

	/*********************************************************************************************/
	free(row_ptr);
	free(col_idx);
	free(val);

	free(filename);
	return 0;
}
