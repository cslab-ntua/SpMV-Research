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
	if(m!=n) {
		double ratio = n*1.0 / m;
		if((ratio>16.0) || (ratio<(1/16.0)))
			ratio=16.0;
		// in order to keep both below 1024
		if(ratio>1) // n > m
			num_pixels_y = (1/ratio) * num_pixels_x;
		else // m > n
			num_pixels_x = ratio * num_pixels_y;
	}


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
		printf("time for csr_save_to_mtx = %lf (%s)\n", time, file_new_mat);
	}
}

int main(int argc, char **argv)
{
	int n, m, nnz;
	ValueType * mtx_val;
	int * mtx_rowind;
	int * mtx_colind;

	int * row_ptr;
	int * col_idx;
	double * val;

	long buf_n = 1000;
	char buf[buf_n];
	double time;
	long i;

	if (argc >= 6)
		return 1;

	char * file_in, * file_fig;
	char * path, * filename, * filename_base;

	i = 1;
	file_in = argv[i++];

	str_path_split_path(file_in, strlen(file_in) + 1, buf, buf_n, &path, &filename);
	path = strdup(path);
	filename = strdup(filename);

	str_path_split_ext(filename, strlen(filename) + 1, buf, buf_n, &filename_base, NULL);
	filename_base = strdup(filename_base);
	snprintf(buf, buf_n, "figures/%s", filename_base);
	file_fig = strdup(buf);

	time = time_it(1,
		create_coo_matrix(file_in, &mtx_val, &mtx_rowind, &mtx_colind, &m, &n, &nnz);
	);
	printf("time create_coo_matrix = %lf\n", time);

	row_ptr = (typeof(row_ptr)) malloc((m+1) * sizeof(*row_ptr));
	col_idx = (typeof(col_idx)) malloc(nnz * sizeof(*col_idx));
	val = (typeof(val)) malloc(nnz * sizeof(*val));


	long num_pixels = 1024;
	long num_pixels_x = (n < num_pixels) ? n : num_pixels;
	long num_pixels_y = (m < num_pixels) ? m : num_pixels;
	if(m!=n) {
		double ratio = n*1.0 / m;
		if((ratio>16.0) || (ratio<(1/16.0)))
			ratio=16.0;
		// in order to keep both below 1024
		if(ratio>1) // n > m
			num_pixels_y = (1/ratio) * num_pixels_x;
		else // m > n
			num_pixels_x = ratio * num_pixels_y;
	}

	time = time_it(1,
		coo_to_csr(mtx_rowind, mtx_colind, mtx_val, m, n, nnz, row_ptr, col_idx, val, 1, 0);
	);
	printf("time coo_to_csr = %lf\n", time);

	// csr_matrix_features_validation(filename_base, row_ptr, col_idx, m, n, nnz);
	feature_plot_store_matrix(file_in, "", 1, 0, m, n, nnz, row_ptr, col_idx, val);
	

	/********************/
	/* COL SORTING PART */
	/********************/
	
	/*
	 * Convert to CSC representation
	 */

	_TYPE_I * row_idx; //for CSR format
	_TYPE_I * col_ptr;
	_TYPE_V * val_c;

	row_idx = (typeof(row_idx)) malloc(nnz * sizeof(*row_idx));
	col_ptr = (typeof(col_ptr)) malloc((n+1) * sizeof(*col_ptr));
	val_c = (typeof(val_c)) malloc(nnz * sizeof(*val_c));

	time = time_it(1,
		coo_to_csc(mtx_rowind, mtx_colind, mtx_val, m, n, nnz, row_idx, col_ptr, val_c, 1);
	);
	printf("time coo_to_csc = %lf\n", time);
	csc_plot_f(file_fig, row_idx, col_ptr, val_c, m, n, nnz, 0, num_pixels_x, num_pixels_y);

	/////////////////////////////////////////////////////////////////////////////////////////////
	// time = time_it(1,
	// 	printf("\n");
	// 	csr_num_neigh_histogram_plot(file_fig, row_ptr, col_idx, val, m, n, nnz, 1, 1, num_pixels_x, num_pixels_y);
	// 	printf("\n");
	// 	csr_cross_row_similarity_histogram_plot(file_fig, row_ptr, col_idx, val, m, n, nnz, 1, 1, num_pixels_x, num_pixels_y);
	// 	printf("\n");
	// 	csr_row_size_histogram_plot(file_fig, row_ptr, col_idx, val, m, n, nnz, 1, 1024, 1024);
	// 	printf("\n");
	// );
	// printf("time plot (CSR) = %lf\n", time);
	// time = time_it(1,
	// 	printf("\n");
	// 	csc_num_neigh_histogram_plot(file_fig, row_idx, col_ptr, val_c, m, n, nnz, 1, 1, num_pixels_x, num_pixels_y);
	// 	printf("\n");
	// 	csc_cross_col_similarity_histogram_plot(file_fig, row_idx, col_ptr, val_c, m, n, nnz, 1, 1, num_pixels_x, num_pixels_y);
	// 	printf("\n");
	// 	csc_col_size_histogram_plot(file_fig, row_idx, col_ptr, val_c, m, n, nnz, 1, 1024, 1024);
	// 	printf("\n");
	// );
	// printf("time plot (CSC) = %lf\n", time);
	/////////////////////////////////////////////////////////////////////////////////////////////

	/*
	 * Sort by col size
	 */

	_TYPE_I * row_idx_s;
	_TYPE_I * col_ptr_s;
	_TYPE_V * val_c_s;

	row_idx_s = (typeof(row_idx_s)) malloc(nnz * sizeof(*row_idx_s));
	col_ptr_s = (typeof(col_ptr_s)) malloc((n+1) * sizeof(*col_ptr_s));
	val_c_s = (typeof(val_c_s)) malloc(nnz * sizeof(*val_c_s));

	time = time_it(1,
		csc_sort_by_col_size(n, row_idx, col_ptr, val_c, row_idx_s, col_ptr_s, val_c_s);
	);
	printf("time sort_by_col_size = %lf\n", time);

	// // print sizes of 10 most popular columns
	// for(int i=0;i<10;i++)
	// 	printf("not so popular_col[%d] = %d\n", i, col_ptr[i+1]-col_ptr[i]);

	// // print sizes of 10 most popular columns
	// for(int i=0;i<10;i++)
	// 	printf("popular_col[%d] = %d\n", i, col_ptr_s[i+1]-col_ptr_s[i]);

	/*
	 * Convert sorted_cols back to CSR representation
	 */
	_TYPE_I * row_ptr_cs;
	_TYPE_I * col_idx_cs;
	_TYPE_V * val_cs;

	row_ptr_cs = (typeof(row_ptr_cs)) malloc((m+1) * sizeof(*row_ptr_cs));
	col_idx_cs = (typeof(col_idx_cs)) malloc(nnz * sizeof(*col_idx_cs));
	val_cs = (typeof(val_cs)) malloc(nnz * sizeof(*val_cs));

	_TYPE_I * col_indices_s;
	col_indices_s = (typeof(col_indices_s)) malloc(nnz * sizeof(*col_indices_s));

	time = time_it(1,
		csc_col_indices(row_idx_s, col_ptr_s, m, n, nnz, &col_indices_s);
		coo_to_csr(row_idx_s, col_indices_s, val_c_s, m, n, nnz, row_ptr_cs, col_idx_cs, val_cs, 1, 0);
	);
	printf("time csc_to_csr = %lf\n", time);

	feature_plot_store_matrix(file_in, "sorted_cols", 1, 0, m, n, nnz, row_ptr_cs, col_idx_cs, val_cs);

	/* Now, we have two representations of the col-sorted matrix
	 * row_ptr_cs, col_idx_cs, val_cs
	 * &
	 * row_idx_s, col_ptr_s, val_c_s
	 * and need to extract the "popularity zones"
	 * How to do it?
	 */

	// First, define what we mean by "popular columns"
	// Those that contain 20% of nonzeros?
	int col_limit[19];
	int p_index = 0;
	float flag=0.05;
	for(int i=0; i<n; i++){
		if((col_ptr_s[i+1] > nnz * flag) && p_index < 20){
			col_limit[p_index++]  = i;
			printf("flag = %.0f%% -> col_ptr_s[%d] = %d\n", flag*100.0, i+1, col_ptr_s[i+1]);
			flag += 0.05;
		}
	}

	// printf("nonzero distribution in columns of column-sorted matrix:\n");
	// for(int i=0; i<p_index; i++)
	// 	printf("(%2d) %d%% of nonzeros -> %d columns (%.2f%% of cols)\n", i, (i+1)*5, col_limit[i], col_limit[i]*100.0/n);

	printf("---\n%s -> ", file_fig);
	for(int i=0; i<p_index; i++)
		printf("%4d%%\t", (i+1)*5);
	printf("\n%s -> ", file_fig);
	for(int i=0; i<p_index; i++)
		printf("%2.2f\t", col_limit[i]*100.0/n);
	printf("\n%s -> ", file_fig);
	for(int i=0; i<p_index; i++)
		printf("%4d\t", col_limit[i]);
	printf("\n---\n");
	// col_limit[7] columns contain 40% of nonzeros...
	int pcg_select = 40;
	int col_select = col_limit[7];
	// col_limit[11] columns contain 60% of nonzeros...
	// int col_select = col_limit[11];

	printf("col_select = %d\n", col_select);


	int * pop_zone_score;
	pop_zone_score = (typeof(pop_zone_score)) calloc(m, sizeof(*pop_zone_score));

	for(int i=0; i<m; i++){
		for(int j=row_ptr_cs[i]; j<row_ptr_cs[i+1]; j++){
			if(col_idx_cs[j] < col_select)
				pop_zone_score[i] += 1;
		}
	}

	csr_pop_zone_score_histogram_plot(file_fig, pop_zone_score, m, col_select, 1, 1024, 1024);

	_TYPE_I * row_ptr_cs_reorder;
	_TYPE_I * col_idx_cs_reorder;
	_TYPE_V * val_cs_reorder;

	row_ptr_cs_reorder = (typeof(row_ptr_cs_reorder)) malloc((m+1) * sizeof(*row_ptr_cs_reorder));
	col_idx_cs_reorder = (typeof(col_idx_cs_reorder)) malloc(nnz * sizeof(*col_idx_cs_reorder));
	val_cs_reorder = (typeof(val_cs_reorder)) malloc(nnz * sizeof(*val_cs_reorder));

	_TYPE_I *original_row_positions;
	original_row_positions = (typeof(original_row_positions)) malloc(m * sizeof(* original_row_positions));
	double time_row_reorder = time_it(1,
		csr_reorder_matrix_by_row_batch(m, n, nnz, 1, row_ptr_cs, col_idx_cs, val_cs, pop_zone_score, row_ptr_cs_reorder, col_idx_cs_reorder, val_cs_reorder, original_row_positions);
	);
	printf("time_reorder_by_row = %lf\n", time_row_reorder);
	free(original_row_positions);
	/*
	for(int i=0; i<m; i++)
		printf("row %d: %d / %d\t(%.2f)\n", i, pop_zone_score[i], col_select, pop_zone_score[i]*100.0/col_select);
	printf("\n------------------------------------\n\n\n------------------------------------\n\n");

	int * pop_zone_score3;
	pop_zone_score3 = (typeof(pop_zone_score3)) calloc(m, sizeof(*pop_zone_score3));
	for(int i=0; i<m; i++){
		for(int j=row_ptr_cs_reorder[i]; j<row_ptr_cs_reorder[i+1]; j++){
			if(col_idx_cs_reorder[j] < col_select)
				pop_zone_score3[i] += 1;
		}
	}
	for(int i=0; i<m; i++)
		printf("row %d: %d / %d\t(%.2f)\n", i, pop_zone_score3[i], col_select, pop_zone_score3[i]*100.0/col_select);
	free(pop_zone_score3);
	*/

	char * membership_suffix;
	membership_suffix = (typeof(membership_suffix)) malloc(100*sizeof(*membership_suffix));
	sprintf(membership_suffix, "sorted_cols_pop_zone_%d", pcg_select);
	feature_plot_store_matrix(file_in, membership_suffix, 1, 0, m, n, nnz, row_ptr_cs_reorder, col_idx_cs_reorder, val_cs_reorder);
	free(membership_suffix);

	free(row_ptr_cs_reorder);
	free(col_idx_cs_reorder);
	free(val_cs_reorder);

	free(pop_zone_score);
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// if(0)
	{
		int window_width = 256;
		int num_windows_row;
		int batch = 1;

		// Extract row_cross
		_TYPE_I * rc_r, * rc_c;
		float * rc_v;

		double time_row_cross2 = time_it(1,
			csr_extract_row_cross2_batch(row_ptr_cs, col_idx_cs, val_cs, m, n, nnz, window_width, batch, &num_windows_row, &rc_r, &rc_c, &rc_v);
		);
		printf("time_row_cross2 = %lf\n", time_row_cross2);

		printf("num_windows_row = %d\n", num_windows_row);
		int col_select_window = (col_select-1+window_width)/window_width;
		printf("therefore col_select_window is %d\n", col_select_window);
		// In this col_select_window, we will tag each row, how many windows out of those it has nonzeros in.
		// the maximum is col_select_window (all of them)
		int m_batch = (m-1+batch)/batch;
		printf("m_batch = %d\n", m_batch);


		/*	
		for(int i=0;i<10;i++){
			printf("row %d: ", i);
			for(int j=rc_r[i];j<rc_r[i+1];j++){
				printf("%d(%.0f) ", rc_c[j], rc_v[j]);
			}
			printf("\n");
		}
		printf("...\n");
		for(int i=m-10;i<m;i++){
			printf("row %d: ", i);
			for(int j=rc_r[i];j<rc_r[i+1];j++){
				printf("%d(%.0f) ", rc_c[j], rc_v[j]);
			}
			printf("\n");
		}
		
		// for(int i=0; i<m_batch; i++){
		for(int i=0; i<5; i++){
			printf("row %d: %d [ ", i, rc_r[i+1]-rc_r[i]);
			for(int j=rc_r[i]; j<rc_r[i+1]; j++){
				printf("%d ", rc_c[j]);
			}
			printf("]\n");
			printf(">> row %d: %d [ ", i, rc_r[i+1]-rc_r[i]);
			for(int j=rc_r[i]; j<rc_r[i+1]; j++){
				printf("%.0f ", rc_v[j]);
			}
			printf("]\n");
		}
		printf("\n\n\n------------------------\n\n\n");
		for(int i=0; i<5; i++){
			printf("row %d: %d [ ", i, row_ptr_cs[i+1]-row_ptr_cs[i]);
			for(int j=row_ptr_cs[i]; j<row_ptr_cs[i+1]; j++){
				printf("%d ", col_idx_cs[j]);
			}
			printf("]\n");
			printf(">> row %d: %d [ ", i, row_ptr_cs[i+1]-row_ptr_cs[i]);
			for(int j=row_ptr_cs[i]; j<row_ptr_cs[i+1]; j++){
				printf("%.4f ", val_cs[j]);
			}
			printf("]\n");
		}
		*/
		
		int * pop_zone_score2;
		pop_zone_score2 = (typeof(pop_zone_score2)) calloc(m_batch, sizeof(*pop_zone_score2));

		for(int i=0; i<m_batch; i++){
			for(int j=rc_r[i]; j<rc_r[i+1]; j++){
				if(rc_c[j] < col_select_window)
					pop_zone_score2[i] += 1;
			}
		}

		// for(int i=0; i<m_batch; i++){
			// pop_zone_score2[i] = pop_zone_score2[i] * 100.0 / col_select_window;
			// printf("row %d: %d / %d\t(%.2f)\n", i, pop_zone_score2[i], col_select_window, pop_zone_score2[i]*100.0/col_select_window);
		// }

		csr_pop_zone_score_histogram_plot(file_fig, pop_zone_score2, m_batch, col_select_window, 1, 1024, 1024);

		free(pop_zone_score2);

		// free row-cross arrays
		free(rc_r);
		free(rc_c);
		free(rc_v);
	}
	/*
	 * Free allocated memory
	 */
	// original matrix (CSC representation)
	free(row_idx);
	free(col_ptr);
	free(val_c);

	// sort by col size (CSC representation)
	free(row_idx_s);
	free(col_ptr_s);
	free(val_c_s);

	// sort by col size (CSR representation)
	free(col_indices_s);
	free(row_ptr_cs);
	free(col_idx_cs);
	free(val_cs);
	/********************/

	// original matrix (CSR representation)
	free(row_ptr);
	free(col_idx);
	free(val);

	return 0;
}
