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
	#include "kmeans.h"
	#include "kmeans_char.h"
	#include "bit_ops.h"
#ifdef __cplusplus
}
#endif


#include "read_mtx.h"

#include "parallel_util.h"

#include <omp.h>
// #include "util.h"
// #include "matrix_util.h"


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

	char * path, * filename, * filename_base;
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

void save_csr_to_mtx(const int* row_ptr, const int* col_idx, const double* val, int num_rows, int num_cols, const char* filename) 
{
	printf("Storing: %s\n", filename);
	FILE* file;
	file = fopen(filename, "w");
	if (file == NULL) {
		printf("Error opening file %s\n", filename);
		return;
	}

	fprintf(file, "%%%%MatrixMarket matrix coordinate real general\n");
	fprintf(file, "%d %d %d\n", num_rows, num_cols, row_ptr[num_rows]);
	for (int i = 0; i < num_rows; i++) {
		for (int j = row_ptr[i]; j < row_ptr[i + 1]; j++) {
			fprintf(file, "%d %d %.15g\n", i + 1, col_idx[j] + 1, val[j]);
		}
	}
	fclose(file);
}

void save_csc_to_mtx(const int* row_idx, const int* col_ptr, const double* val, int num_rows, int num_cols, const char* filename) 
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

void feature_plot_store_matrix(char *file_in, const char *replace_str, int plot, int store, long m, long n, long nnz, _TYPE_I* row_ptr, _TYPE_I* col_idx, _TYPE_V* val)
{	
	// char * file_mat = str_path_split_path(file_in, strlen(file_in) + 1, buf, buf_n, &path, &filename);
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
		save_csr_to_mtx(row_ptr, col_idx, val, m, n, file_new_mat);
		);
		printf("time for save_csr_to_mtx = %lf\n", time);
	}
}

// Comparison function for qsort
int compare(const void *a, const void *b) {
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
	// int num_threads = 1;//omp_get_max_threads();
	// #pragma omp parallel num_threads(num_threads) firstprivate(step_size_row)
	// {
		// int thread_num = omp_get_thread_num();
		// #pragma omp for
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
			qsort(array_range, high_nnz, sizeof(_TYPE_I), compare);

			for(int k = 0; k < step_size_row; k++){
				for (int j = 0; j < high_nnz; j++) {
					_TYPE_I index = (start_row + k - 1) * high_nnz + j;
					col_idx[index] = array_range[j];
					val[index]  = ((_TYPE_V)rand() / RAND_MAX);
				}
				row_ptr[start_row + k] = (start_row + k) * high_nnz;
			}
		}
	// }
}
void read_array_from_file(const char* filename, int* array, size_t arraySize)
{
	FILE* file = fopen(filename, "r");
	if (file == NULL) {
		perror("Error opening the file");
		return;
	}

	for (size_t i = 0; i < arraySize; i++) {
		if (fscanf(file, "%d,", &array[i]) != 1) {
			perror("Error reading from the file");
			fclose(file);
			return;
		}
	}

	fclose(file);
}

void write_array_to_file(const int *array, size_t size, const char *filename)
{
	FILE *file = fopen(filename, "w"); // Open the file for writing

	if (file == NULL) {
		perror("Error opening the file");
		return;
	}

	for (size_t i = 0; i < size; i++) {
		fprintf(file, "%d", array[i]); // Write the integer to the file

		// Add a comma and space unless it's the last element
		if (i < size - 1) {
			fprintf(file, ", ");
		}
	}

	fclose(file); // Close the file
}


typedef struct {
    int row;
    int family_id;
} RowFamily;

int compareRowFamily(const void *a, const void *b) {
    return ((RowFamily *)a)->family_id - ((RowFamily *)b)->family_id;
}

void reorder_matrix_by_row(int m, int n, int nnz, _TYPE_I * row_ptr, _TYPE_I * col_idx, _TYPE_V * val, 
					int * membership, _TYPE_I * row_ptr_reorder, _TYPE_I * col_idx_reorder, _TYPE_V * val_reorder)
{
	// Create an auxiliary array to store row indices and family IDs
	RowFamily* row_families = (RowFamily*)malloc(m * sizeof(RowFamily));

	for (int i = 0; i < m; i++) {
		row_families[i].row = i;
		row_families[i].family_id = membership[i];
	}

    // Sort the auxiliary array based on family IDs
    double time_qsort_membership = time_it(1,
    qsort(row_families, m, sizeof(RowFamily), compareRowFamily);
	);
	// printf("time_qsort_membership = %lf\n", time_qsort_membership);

	row_ptr_reorder[0] = 0;
    int cnt = 0;

    for (int i = 0; i < m; i++) {
        int original_row = row_families[i].row;
        // printf("row: %d, membership; %d\n", row_families[i].row, row_families[i].family_id);
        
        for (int j = row_ptr[original_row]; j < row_ptr[original_row + 1]; j++) {
            col_idx_reorder[cnt] = col_idx[j];
            val_reorder[cnt] = val[j];
            cnt++;
        }
        row_ptr_reorder[i+1] = cnt;
    }
}

typedef struct {
    int col;
    int family_id;
} ColFamily;

int compareColFamily(const void *a, const void *b) {
    return ((ColFamily *)a)->family_id - ((ColFamily *)b)->family_id;
}

void reorder_matrix_by_col(int m, int n, int nnz, _TYPE_I * row_idx, _TYPE_I * col_ptr, _TYPE_V * val, 
					int * membership, _TYPE_I * row_idx_reorder, _TYPE_I * col_ptr_reorder, _TYPE_V * val_reorder)
{
	// Create an auxiliary array to store row indices and family IDs
	ColFamily* col_families = (ColFamily*)malloc(n * sizeof(ColFamily));

	for (int i = 0; i < n; i++) {
		col_families[i].col = i;
		col_families[i].family_id = membership[i];
	}

    // Sort the auxiliary array based on family IDs
    double time_qsort_membership = time_it(1,
    qsort(col_families, n, sizeof(ColFamily), compareColFamily);
	);
	printf("time_qsort_membership = %lf\n", time_qsort_membership);

	col_ptr_reorder[0] = 0;
    int cnt = 0;

    for (int i = 0; i < n; i++) {
        int original_row = col_families[i].col;
        // printf("row: %d, membership; %d\n", col_families[i].col, col_families[i].family_id);
        
        for (int j = col_ptr[original_row]; j < col_ptr[original_row + 1]; j++) {
            row_idx_reorder[cnt] = row_idx[j];
            val_reorder[cnt] = val[j];
            cnt++;
        }
        col_ptr_reorder[i+1] = cnt;
    }
}

void random_selection(float * start_array, float *target_array, int numObjs, int numClusters, int numCoords)
{
	int * indices = (int *) malloc(numObjs * sizeof(int));
	for(int i = 0; i < numObjs; i++)
		indices[i] = i;
	srand(14);
	for(int i = 0; i < numClusters ; i++) {
		int k = rand() % (numObjs - i);
		for(int j = 0; j < numCoords; j++)
			target_array[i * numCoords + j] = start_array[indices[k] * numCoords + j];
		indices[k] = indices[numCoords - i - 1];
	}
	free(indices);
}

void random_selection_char(unsigned char * start_array, unsigned char *target_array, int numObjs, int numClusters, int numCoords)
{
	int * indices = (int *) malloc(numObjs * sizeof(int));
	for(int i = 0; i < numObjs; i++)
		indices[i] = i;
	srand(14);
	for(int i = 0; i < numClusters ; i++) {
		int k = rand() % (numObjs - i);
		for(int j = 0; j < numCoords; j++)
			target_array[i * numCoords + j] = start_array[indices[k] * numCoords + j];
		indices[k] = indices[numCoords - i - 1];
	}
	free(indices);
}


typedef struct {
    char *binary_string;
    int id;
} binary_string_with_id;

char* convert_to_binary_string(float* row, int numCols) {
	char* binary_string = (char*)malloc(numCols + 1); // +1 for the null terminator
	if (binary_string == NULL) {
		fprintf(stderr, "Memory allocation failed.\n");
		exit(1);
	}

	for (int j = 0; j < numCols; j++) {
		binary_string[j] = (row[j] != 0.0) ? '1' : '0';
	}
	binary_string[numCols] = '\0'; // Null-terminate the string

	return binary_string;
}

char* convert_to_binary_string2(unsigned char* row, int numCols) {
	char* binary_string = (char*)malloc(numCols + 1); // +1 for the null terminator
	if (binary_string == NULL) {
		fprintf(stderr, "Memory allocation failed.\n");
		exit(1);
	}

	for (int j = 0; j < numCols; j++) {
		binary_string[j] = row[j];
	}
	binary_string[numCols] = '\0'; // Null-terminate the string

	return binary_string;
}
// // Comparison function for qsort
// int compare_binary_strings(const void *a, const void *b) { // ascending order
// 	return strcmp(*(const char **)a, *(const char **)b);
// }

// Comparison function for qsort
int compare_binary_strings_descending(const void *a, const void *b) { // descending order
    return strcmp(((binary_string_with_id *)b)->binary_string, ((binary_string_with_id *)a)->binary_string);
}


typedef struct {
	unsigned char *cluster_char;
    int cluster_id;
} cluster_char_with_id;

unsigned char* assign_to_cluster_struct(unsigned char* row, int numCols) {
	unsigned char* cluster_string = (typeof(cluster_string))malloc((numCols + 1) * sizeof(*cluster_string)); // +1 for the null terminator
	if (cluster_string == NULL) {
		fprintf(stderr, "Memory allocation failed.\n");
		exit(1);
	}
	for (int j = 0; j < numCols; j++){
		// before:0 or 1, now: '0' or '1'. Why do I do this? 
		// In order to be able to really sort rows effectively otherwise, strcmp did not work effectively
		if(row[j]==0)
			cluster_string[j] = '0';
		else
			cluster_string[j] = '1';
	}
	cluster_string[numCols] = '\0'; // Null-terminate the string
	return cluster_string;
}

// Comparison function for qsort
int compare_unsigned_char_strings_descending(const void *a, const void *b) { // descending order
    return strcmp((char*)((cluster_char_with_id *)b)->cluster_char, (char*)((cluster_char_with_id *)a)->cluster_char);
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

	int i;

	char *file_in;
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
	if(argc>16)
		artificial = atoi(argv[i++]);

	long num_pixels = 1024;
	long num_pixels_x = num_pixels;
	long num_pixels_y = num_pixels;

	if(!artificial) {
		file_in = argv[1];
		create_coo_matrix(file_in, &mtx_val, &mtx_rowind, &mtx_colind, &m, &n, &nnz);
		num_pixels_x = (n < num_pixels) ? n : num_pixels;
		num_pixels_y = (m < num_pixels) ? m : num_pixels;
		row_ptr = (typeof(row_ptr)) malloc((m+1) * sizeof(*row_ptr));
		col_idx = (typeof(col_idx)) malloc(nnz * sizeof(*col_idx));
		val = (typeof(val)) malloc(nnz * sizeof(*val));
		coo_to_csr(mtx_rowind, mtx_colind, mtx_val, m, n, nnz, row_ptr, col_idx, val, 1, 0);
		csr_plot_f(fig_name_gen(file_in, ""),  row_ptr, col_idx, val, m, n, nnz, 0, num_pixels_x, num_pixels_y);

		/********************************************************************************************************************************/
		_TYPE_I * row_idx; //for CSC format
		_TYPE_I * col_ptr;
		_TYPE_V * val_c;

		row_idx = (typeof(row_idx)) malloc(nnz * sizeof(*row_idx));
		col_ptr = (typeof(col_ptr)) malloc((n+1) * sizeof(*col_ptr));
		val_c = (typeof(val_c)) malloc(nnz * sizeof(*val_c));
		coo_to_csc(mtx_rowind, mtx_colind, mtx_val, m, n, nnz, row_idx, col_ptr, val_c, 1);
		// csc_plot_f(fig_name_gen(file_in, "csc"),  row_idx, col_ptr, val_c, m, n, nnz, 0, num_pixels_x, num_pixels_y););

		// float * row_cross;
		// _TYPE_I * rc_r1, * rc_c1;
		// float * rc_v1;
		
		// float * col_cross;
		// _TYPE_I * cc_r1, * cc_c1;
		// float * cc_v1;

		_TYPE_I * rc_r, * rc_c;
		float * rc_v;

		// _TYPE_I * cc_r, * cc_c;
		// float * cc_v;

		int num_windows_row, num_windows_col, window_width;
		window_width = atoi(argv[i++]);

		int plot_extra = 1;
		// printf("------------------------------------------------------------------------\n");
		// double time_row_cross = time_it(1,
		// csr_extract_row_cross(row_ptr, col_idx, val, m, n, nnz, window_width, &num_windows_row, &row_cross, plot_extra, &rc_r1, &rc_c1, &rc_v1);
		// );
		// printf("time_row_cross = %lf s\n", time_row_cross);
		// csr_plot_f(fig_name_gen(file_in, "row_cross1"),  rc_r1, rc_c1, (_TYPE_V *) rc_v1, m, num_windows_row, rc_r1[m], 1, num_pixels_x, num_pixels_y);

		// double time_col_cross = time_it(1,
		// csc_extract_col_cross(row_idx, col_ptr, val_c, m, n, nnz, window_width, &num_windows_col, &col_cross, plot_extra, &cc_r1, &cc_c1, &cc_v1);
		// );
		// csc_plot_f(fig_name_gen(file_in, "col_cross1"),  cc_r1, cc_c1, (_TYPE_V *) cc_v1, num_windows_col, n, cc_c1[n], 0, num_pixels_x, num_pixels_y);
		// printf("time_col_cross = %lf s\n", time_col_cross);

		double time_row_cross2 = time_it(1,
		csr_extract_row_cross2(row_ptr, col_idx, val, m, n, nnz, window_width, &num_windows_row, &rc_r, &rc_c, &rc_v);
		);
		csr_plot_f(fig_name_gen(file_in, "row_cross"),  rc_r, rc_c, (_TYPE_V *) rc_v, m, num_windows_row, rc_r[m], 1, num_pixels_x, num_pixels_y);
		printf("time_row_cross2 = %lf s\n", time_row_cross2);

		
		// double time_col_cross2 = time_it(1,
		// csc_extract_col_cross2(row_idx, col_ptr, val_c, m, n, nnz, window_width, &num_windows_col, &cc_r, &cc_c, &cc_v);
		// );
		// csc_plot_f(fig_name_gen(file_in, "col_cross"),  cc_r, cc_c, (_TYPE_V *) cc_v, num_windows_col, n, cc_c[n], 0, num_pixels_x, num_pixels_y);
		// printf("time_col_cross2 = %lf s\n", time_col_cross2);

		return 0;

		/************************************************/
		// kmeans_char reordering
		/*
		if(1)
		{
			unsigned char * row_cross_char;
			// _TYPE_I * rc_r_char, * rc_c_char;
			// float * rc_v_char;
			double time_row_cross_char = time_it(1,
			csr_extract_row_cross_char(row_ptr, col_idx, val, m, n, nnz, window_width, &num_windows_row, &row_cross_char, 0, NULL, NULL, NULL);
			);
			// csr_plot_f(fig_name_gen(file_in, "row_cross_char"),  rc_r_char, rc_c_char, (_TYPE_V *) rc_v_char, m, num_windows_row, rc_r_char[m], 0, num_pixels_x, num_pixels_y);
			printf("time_row_cross_char = %lf s\n", time_row_cross_char);

			for(int i=0;i<10;i++){
				unsigned char * row = (typeof(row)) malloc((num_windows_row) * sizeof(*row));
				for(int j=0;j<num_windows_row;j++)
					row[j] = row_cross_char[i*num_windows_row + j];
				// printf("row[%d] = %s\n", i, row);

				unsigned char * row2 = (typeof(row)) malloc((num_windows_row) * sizeof(*row));
				for(int j=0;j<num_windows_row;j++)
					row2[j] = row_cross_char[(i+1)*num_windows_row + j];
				// printf("row2[%d] = %s\n", i, row);

				printf("row[%d]  = ", i);
				for(int j=0;j<num_windows_row;j++){
					printf("%d", row_cross_char[i*num_windows_row+j]);
				}
				printf("\n");
				printf("row2[%d] = ", i);
				for(int j=0;j<num_windows_row;j++){
					printf("%d", row_cross_char[(i+1)*num_windows_row+j]);
				}
				printf("\n");

				long bhd = bits_hamming_distance(row, row2, num_windows_row);
				printf("bhd is %lu\n", bhd);
				printf("\n");

				free(row);
				free(row2);
			}
			for(int i=0;i<5;i++){
				printf("row = %d\t", i);
				for(int j=row_ptr[i]; j<row_ptr[i+1]; j++){
					printf("%d ", col_idx[j]);
				}
				printf("\n");
			}
			
			int numObjs = m;
			int numClusters = atoi(argv[i++]);
			float threshold = 0.001;
			// float threshold = 0.1;
			long loop_threshold = 150;
			int * membership = (typeof(membership)) malloc(m * sizeof(*membership));
			int * membership_backup = (typeof(membership)) malloc(m * sizeof(*membership));
			int numCoords = num_windows_row;

			unsigned char * clusters_char = (typeof(clusters_char)) malloc(numClusters * numCoords * sizeof(*clusters_char));
			// for(int i = 0; i < numClusters ; i++)
			// 	for(int j = 0; j < numCoords; j++)
			// 		clusters_char[i * numCoords + j] = row_cross_char[i * numCoords + j];
			random_selection_char(row_cross_char, clusters_char, numObjs, numClusters, numCoords);


			int type = 0; // hamming distance
			double time_kmeans_char = time_it(1,
			kmeans_char(row_cross_char, numCoords, numObjs, numClusters, threshold, loop_threshold, membership, clusters_char, type);
			);
			printf("\ntime_kmeans_char = %lf (numObjs = %d, numCoords = %d, numClusters = %d)\n", time_kmeans_char, numObjs, numCoords, numClusters);

			
			int * clusterSize = (typeof(clusterSize)) calloc(numClusters, sizeof(*clusterSize));
			for(int i=0; i<m; i++)
				clusterSize[membership[i]]++;
			for(int i = 0; i < numClusters; i++){
				printf("cluster[%3d]\t%.4lf\%\t", i, (clusterSize[i]*1.0)/m*100);
				// for(int j = 0; j < numCoords; j++)
				// 	printf("%d", clusters_char[i * numCoords + j]);
				printf("\n");
			}
			free(clusterSize);
			

			char * membership_suffix = (typeof(membership_suffix)) malloc(100*sizeof(*membership_suffix));

			_TYPE_I * row_ptr_reorder; //for CSR format
			_TYPE_I * col_idx_reorder;
			_TYPE_V * val_reorder;

			row_ptr_reorder = (typeof(row_ptr_reorder)) malloc((m+1) * sizeof(*row_ptr_reorder));
			col_idx_reorder = (typeof(col_idx_reorder)) malloc(nnz * sizeof(*col_idx_reorder));
			val_reorder = (typeof(val_reorder)) malloc(nnz * sizeof(*val_reorder));

			double time_reorder_matrix_by_row = time_it(1,
			reorder_matrix_by_row(m, n, nnz, row_ptr, col_idx, val, 
								  membership, row_ptr_reorder, col_idx_reorder, val_reorder);
			);
			printf("time reorder_matrix_by_row = %lf\n", time_reorder_matrix_by_row);

			sprintf(membership_suffix, "member_row_%d_window_%d_%d_CHAR.txt", numClusters, window_width, numClusters);

			csr_plot_f(fig_name_gen(file_in, membership_suffix),  row_ptr_reorder, col_idx_reorder, val_reorder, m, n, nnz, 0, num_pixels_x, num_pixels_y);
			feature_plot_store_matrix(file_in, membership_suffix, plot, 0, m, n, nnz, row_ptr_reorder, col_idx_reorder, val_reorder);

			cluster_char_with_id clusters_struct[numClusters];

			for(int i = 0; i < numClusters; i++){
				// pass all clusters along with their ID to the cluster_char_with_id struct
				clusters_struct[i].cluster_char = assign_to_cluster_struct(&clusters_char[i * numCoords], numCoords);
				clusters_struct[i].cluster_id   = i;
			}

			// // Display the binary strings
			// for (int i = 0; i < numClusters; i++){
			// 	printf("cluster_c[%3d]: ", i);
			// 	for(int j = 0; j < numCoords; j++)
			// 		printf("%d", clusters_struct[i].cluster_char[j]);
			// 	printf("\n");
			// }

			// Sort the array of strings
			qsort(clusters_struct, numClusters, sizeof(cluster_char_with_id), compare_unsigned_char_strings_descending);

			// printf("after sorting...\n");
			// for (int i = 0; i < numClusters; i++){
			// 	printf("cluster_c[%3d]: ", i);
			// 	for(int j = 0; j < numCoords; j++)
			// 		printf("%d", clusters_struct[i].cluster_char[j]);
			// 	printf("(%d)\n", clusters_struct[i].cluster_id);
			// }

			// Free the dynamically allocated memory
			for (int i = 0; i < numClusters; i++)
				free(clusters_struct[i].cluster_char);

			// for (int i = 0; i < numClusters; i++)
			// 	printf("%d -> %d\n", i, clusters_struct[i].cluster_id);

			int * membership2 = (typeof(membership2)) malloc(numClusters * sizeof(*membership2));
			for (int i = 0; i < numClusters; i++)
				membership2[clusters_struct[i].cluster_id] = i;

			for(int i = 0; i < m; i++){
				membership_backup[i] = membership[i];
				membership[i] = membership2[membership[i]];
				// membership[i] = membership2[membership[i]];
			}

			double time_reorder_matrix_by_row2_2 = time_it(1,
			reorder_matrix_by_row(m, n, nnz, row_ptr, col_idx, val, 
								  membership, row_ptr_reorder, col_idx_reorder, val_reorder);
			);
			printf("time reorder_matrix_by_row2_2 = %lf\n", time_reorder_matrix_by_row2_2);

			sprintf(membership_suffix, "member_row_%d_window_%d_%d_CHAR_v2.txt", numClusters, window_width, numClusters);
			csr_plot_f(fig_name_gen(file_in, membership_suffix),  row_ptr_reorder, col_idx_reorder, val_reorder, m, n, nnz, 0, num_pixels_x, num_pixels_y);
			feature_plot_store_matrix(file_in, membership_suffix, plot, 1, m, n, nnz, row_ptr_reorder, col_idx_reorder, val_reorder);


			free(membership2);


			free(row_ptr_reorder);
			free(col_idx_reorder);
			free(val_reorder);

			free(membership);
			free(clusters_char);

			// free(membership2);
			// free(clusters2);

			free(row_cross_char);
			// free(rc_r_char);
			// free(rc_c_char);
			// free(rc_v_char);
		}
		*/
		/************************************************/


		int reorder_mode = 0;
		// if(reorder_mode == 0)
		// if(1)
		// {
		// 	int numObjs = m;
		// 	// int numClusters = 256;
		// 	int numClusters = atoi(argv[i++]);
		// 	float threshold = 0.001;
		// 	// float threshold = 0.1;
		// 	long loop_threshold = 300;
		// 	int * membership = (typeof(membership)) malloc(m * sizeof(*membership));
		// 	int * membership_backup = (typeof(membership)) malloc(m * sizeof(*membership));
		// 	int numCoords = num_windows_row;

		// 	float * clusters = (typeof(clusters)) malloc(numClusters * numCoords * sizeof(*clusters));
		// 	random_selection(row_cross, clusters, numObjs, numClusters, numCoords);
		// 	// for (int i=0; i<numClusters; i++)
		// 	// 	for (int j=0; j<numCoords; j++)
		// 	// 		clusters[i*numCoords + j] = row_cross[i*numCoords + j];

		// 	int type = 1; // cosine similarity
		// 	double time_kmeans = time_it(1,
		// 	kmeans(row_cross, numCoords, numObjs, numClusters, threshold, loop_threshold, membership, clusters, type);
		// 	);
		// 	printf("\ntime_kmeans = %lf (numObjs = %d, numCoords = %d, numClusters = %d)\n", time_kmeans, numObjs, numCoords, numClusters);


		// 	int numClusters2 = atoi(argv[i++]);
		// 	int * membership2 = (typeof(membership2)) malloc(numClusters * sizeof(*membership2));
		// 	float * clusters2 = (typeof(clusters2)) malloc(numClusters2 * numCoords * sizeof(*clusters2));

		// 	random_selection(clusters, clusters2, numClusters, numClusters2, numCoords);
		// 	type = 1; // cosine similarity
		// 	double time_kmeans2 = time_it(1,
		// 	kmeans(clusters, numCoords, numClusters, numClusters2, threshold, loop_threshold, membership2, clusters2, type);
		// 	);
		// 	printf("\ntime_kmeans2 = %lf (numObjs = %d, numCoords = %d, numClusters = %d)\n", time_kmeans2, numClusters, numCoords, numClusters2);

		// 	int * clusterSize2 = (typeof(clusterSize2)) calloc(numClusters2, sizeof(*clusterSize2));
		// 	for(int i=0; i<numClusters; i++)
		// 		clusterSize2[membership2[i]]++;
		// 	for(int i = 0; i < numClusters2; i++){
		// 		if(clusterSize2[i]!=0)
		// 			printf("cluster2[%3d]\t%.4lf\%\t", i, (clusterSize2[i]*1.0)/numClusters*100);
		// 		else
		// 			printf("cluster2[%3d]\t \t", i);
		// 		for(int j = 0; j < numCoords; j++){
		// 			if(clusters2[i * numCoords + j] != 0.0)
		// 				printf("%.2lf\t", clusters2[i * numCoords + j]);
		// 			else
		// 				printf(" \t");
		// 		}
		// 		printf("\n");
		// 	}
		// 	free(clusterSize2);


		// 	// char * binary_string[numClusters];
		// 	binary_string_with_id clusters_binary[numClusters];

		// 	for(int i = 0; i < numClusters; i++){
		// 		clusters_binary[i].binary_string = convert_to_binary_string(&clusters[i * numCoords], numCoords);
		// 		clusters_binary[i].id = i;
		// 		// binary_string[i] = convert_to_binary_string(&clusters[i * numCoords], numCoords);
		// 	}

		// 	// Display the binary strings
		// 	for (int i = 0; i < numClusters; i++){
		// 		printf("cluster_b[%3d]: %s\n", i, clusters_binary[i].binary_string);
		// 	}


		// 	// Sort the array of binary strings
		// 	// qsort(binary_string, numClusters, sizeof(const char *), compare_binary_strings);
		// 	qsort(clusters_binary, numClusters, sizeof(binary_string_with_id), compare_binary_strings_descending);

		// 	printf("after sorting...\n");
		// 	// Display the binary strings
		// 	for (int i = 0; i < numClusters; i++)
		// 		printf("cluster_b[%3d]: %s (%d)\n", i, clusters_binary[i].binary_string, clusters_binary[i].id);

		// 	// Free the dynamically allocated memory
		// 	for (int i = 0; i < numClusters; i++)
		// 		free(clusters_binary[i].binary_string);

		// 	// Free the dynamically allocated memory
		// 	for (int i = 0; i < numClusters; i++)
		// 		printf("%d -> %d\n", i, clusters_binary[i].id);

		// 	int * membership2_2 = (typeof(membership2_2)) malloc(numClusters * sizeof(*membership2_2));
		// 	for (int i = 0; i < numClusters; i++){
		// 		membership2_2[clusters_binary[i].id] = i;
		// 	}

		// 	char * membership_suffix = (typeof(membership_suffix)) malloc(100*sizeof(*membership_suffix));
		// 	// printf("reading from file %s\n", fig_name_gen(file_in, membership_suffix));
		// 	// read_array_from_file(fig_name_gen(file_in, membership_suffix), membership, m);

		// 	_TYPE_I * row_ptr_reorder; //for CSR format
		// 	_TYPE_I * col_idx_reorder;
		// 	_TYPE_V * val_reorder;

		// 	row_ptr_reorder = (typeof(row_ptr_reorder)) malloc((m+1) * sizeof(*row_ptr_reorder));
		// 	col_idx_reorder = (typeof(col_idx_reorder)) malloc(nnz * sizeof(*col_idx_reorder));
		// 	val_reorder = (typeof(val_reorder)) malloc(nnz * sizeof(*val_reorder));


		// 	double time_reorder_matrix_by_row = time_it(1,
		// 	reorder_matrix_by_row(m, n, nnz, row_ptr, col_idx, val, 
		// 						  membership, row_ptr_reorder, col_idx_reorder, val_reorder);
		// 	);
		// 	printf("time reorder_matrix_by_row = %lf\n", time_reorder_matrix_by_row);

		// 	sprintf(membership_suffix, "member_row_%d_window_%d_%d.txt", numClusters, window_width, numClusters);

		// 	csr_plot_f(fig_name_gen(file_in, membership_suffix),  row_ptr_reorder, col_idx_reorder, val_reorder, m, n, nnz, 0, num_pixels_x, num_pixels_y);
		// 	feature_plot_store_matrix(file_in, membership_suffix, plot, 0, m, n, nnz, row_ptr_reorder, col_idx_reorder, val_reorder);

		// 	free(row_ptr_reorder);
		// 	free(col_idx_reorder);
		// 	free(val_reorder);

		// 	free(membership);
		// 	free(clusters);
		// 	free(membership2);
		// 	free(clusters2);

		// }
		
		free(rc_r);
		free(rc_c);
		free(rc_v);
		// free(cc_r);
		// free(cc_c);
		// free(cc_v);

		// free(row_cross);
		// free(col_cross);

		// free(cross_row_ptr);
		// free(cross_col_idx);
		// free(cross_val);
		// free(cross_dense);

		free(row_idx);
		free(col_ptr);
		free(val_c);

		/********************************************************************************************************************************/
		free(mtx_rowind);
		free(mtx_colind);
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
		printf("dist_nnz = %ld (%.2f\%)\n", dist_nnz, dist_nnz*100.0/nnz);

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
	if(shuffle){
		_TYPE_I * row_ptr_shuffle;
		_TYPE_I * col_idx_shuffle;
		_TYPE_V * val_shuffle;

		row_ptr_shuffle = (typeof(row_ptr_shuffle)) malloc((m+1) * sizeof(*row_ptr_shuffle));
		col_idx_shuffle = (typeof(col_idx_shuffle)) malloc(nnz * sizeof(*col_idx_shuffle));
		val_shuffle = (typeof(val_shuffle)) malloc(nnz * sizeof(*val_shuffle));
		printf("---> csr_shuffle_matrix START\n");
		double time = time_it(1,
			csr_shuffle_matrix(m, row_ptr, col_idx, val, row_ptr_shuffle, col_idx_shuffle, val_shuffle);
		);
		printf("time for shuffle = %lf\n", time);
		printf("---> csr_shuffle_matrix   END\n");

		feature_plot_store_matrix(file_in, "shuffle", plot, store, m, n, nnz, row_ptr_shuffle, col_idx_shuffle, val_shuffle);

		free(row_ptr_shuffle);
		free(col_idx_shuffle);
		free(val_shuffle);
	}

	/*********************************************************************************************/

	free(row_ptr);
	free(col_idx);
	free(val);

	free(filename);
	return 0;
}
