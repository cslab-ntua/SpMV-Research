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
	
	#include "rcm.h"
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

	long num_pixels = 4096;
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
		// csr_row_size_histogram_plot(file_fig, row_ptr, col_idx, val, m, n, nnz, 1, num_pixels_x, num_pixels_y);
		// int window_size = 1024;
		// csr_num_neigh_histogram_plot(file_fig, row_ptr, col_idx, val, m, n, nnz, window_size, 1, num_pixels_x, num_pixels_y);
		// csr_cross_row_similarity_histogram_plot(file_fig, row_ptr, col_idx, val, m, n, nnz, window_size, 1, num_pixels_x, num_pixels_y);
		// csr_bandwidth_histogram_plot(file_fig, row_ptr, col_idx, val, m, n, nnz, 1, num_pixels_x, num_pixels_y);
	}

	if(store){
		char * file_new_mat = mtx_name_gen(file_in, replace_str);
		double time = time_it(1,
		csr_save_to_mtx(row_ptr, col_idx, val, m, n, file_new_mat);
		);
		printf("time for csr_save_to_mtx = %lf (%s)\n", time, file_new_mat);
	}
}

void feature_plot_store_csc_matrix(char *file_in, const char *replace_str, int plot, int store, long m, long n, long nnz, _TYPE_I* row_idx, _TYPE_I* col_ptr, _TYPE_V* val_c, int mode)
{
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

	double time = time_it(1,
		csc_col_indices(row_idx, col_ptr, m, n, nnz, &col_indices_s);
		coo_to_csr(row_idx, col_indices_s, val_c, m, n, nnz, row_ptr_cs, col_idx_cs, val_cs, 1, 0);
	);
	printf("time csc_to_csr (%s) = %lf ms\n", replace_str, time*1000);
	if(mode==0){
		// simply store the csr version of the csc matrix
		feature_plot_store_matrix(file_in, replace_str, plot, store, m, n, nnz, row_ptr_cs, col_idx_cs, val_cs);
	}
	else if(mode==1){
		// sort by row size, in order to discard empty rows later...
		_TYPE_I * row_ptr_s;
		_TYPE_I * col_idx_s;
		_TYPE_V * val_r_s;

		row_ptr_s = (typeof(row_ptr_s)) malloc((m+1) * sizeof(*row_ptr_s));
		col_idx_s = (typeof(col_idx_s)) malloc(nnz * sizeof(*col_idx_s));
		val_r_s = (typeof(val_r_s)) malloc(nnz * sizeof(*val_r_s));

		time = time_it(1,
			csr_sort_by_row_size(m, row_ptr_cs, col_idx_cs, val_cs, row_ptr_s, col_idx_s, val_r_s);
		);
		printf("time sort_by_row_size = %lf ms\n", time*1000);

		int m_empty = 0;
		time = time_it(1,
		for(int i=m; i>0; i--){
			if(row_ptr_s[i]-row_ptr_s[i-1] > 0) // non-empty column found
				break;
			else
				m_empty++;
		}
		);
		printf("(%s) m_empty = %d (%.2lf %) (%lf ms)\n", replace_str, m_empty, m_empty*100.0/m, time*1000);
		long m_fixed = m - m_empty;
		feature_plot_store_matrix(file_in, replace_str, plot, store, m_fixed, n, nnz, row_ptr_s, col_idx_s, val_r_s);
		
		free(row_ptr_s);
		free(col_idx_s);
		free(val_r_s);
	}
	else if(mode==2){
		// Just discard empty rows, no sorting to not break any structure that this matrix has left...
		int m_empty = 0;
		time = time_it(1,
		for(int i=0; i<m; i++){
			if(row_ptr_cs[i+1]-row_ptr_cs[i]==0) // non-empty column found
				m_empty++;
		}
		);
		long m_fixed = m - m_empty;

		_TYPE_I * row_ptr_d;
		_TYPE_I new_i = 0;
		row_ptr_d = (typeof(row_ptr_d)) malloc((m_fixed+1) * sizeof(*row_ptr_d));
		// Fill row_ptr_d with pointers to non-empty rows
		_TYPE_I idx = 0;
		for (_TYPE_I i = 0; i < m; i++) {
			if (row_ptr_cs[i] != row_ptr_cs[i + 1]) {
				row_ptr_d[idx++] = row_ptr_cs[i];
			}
		}
		// Add the final value for the last row pointer (for consistency)
		row_ptr_d[m_fixed] = row_ptr_cs[m];
		// printf("row_ptr_d[%ld]=%ld vs row_ptr_cs[%ld]=%ld\n", m_fixed, row_ptr_d[m_fixed], m, row_ptr_cs[m]);
		feature_plot_store_matrix(file_in, replace_str, plot, store, m_fixed, n, nnz, row_ptr_d, col_idx_cs, val_cs);
		
		free(row_ptr_d);
	}

	free(row_ptr_cs);
	free(col_idx_cs);
	free(val_cs);
	free(col_indices_s);

}

void csr_extract_neighbour_stats(_TYPE_I * row_ptr, _TYPE_I * col_idx, _TYPE_I m, _TYPE_I n, _TYPE_I nnz, int window_size, const char * title, int histogram_print, _TYPE_I **num_neigh_out, _TYPE_I **num_crs_neigh_out)
{
	if(histogram_print) printf("\n--------\ntype of matrix: %s\n", title);

	int num_bins;

	_TYPE_I * num_neigh;
	csr_row_neighbours(row_ptr, col_idx, m, n, nnz, window_size, &num_neigh);

	if(histogram_print){
		double neigh_max = -1;
		for(_TYPE_I i=0; i<nnz; i++){
			if(neigh_max < num_neigh[i])
				neigh_max = num_neigh[i];
		}
		if(neigh_max>0){
			_TYPE_I *neigh_hist = (_TYPE_I*) calloc((int)neigh_max, sizeof(_TYPE_I));
			num_bins = (int) neigh_max;
			printf("histogram of num_neigh (neigh_max = %.0lf)\n", neigh_max);
			csr_print_histogram(num_neigh, nnz, neigh_max, num_bins, neigh_hist);
			free(neigh_hist);
		}
	}

	_TYPE_I * num_crs_neigh;
	csr_cross_row_neighbours2(row_ptr, col_idx, m, n, nnz, window_size, &num_crs_neigh);
	
	if(histogram_print){
		double crs_neigh_max = -1;
		for(_TYPE_I i=0; i<nnz; i++){
			if(crs_neigh_max < num_crs_neigh[i])
				crs_neigh_max = num_crs_neigh[i];
		}
		if(crs_neigh_max>0){
			_TYPE_I *crs_neigh_hist = (_TYPE_I*) calloc((int)crs_neigh_max, sizeof(_TYPE_I));
			num_bins = (int) crs_neigh_max;
			printf("histogram of num_crs_neigh (crs_neigh_max = %.0lf)\n", crs_neigh_max);
			csr_print_histogram(num_crs_neigh, nnz, crs_neigh_max, num_bins, crs_neigh_hist);
			free(crs_neigh_hist);
		}
	}

	if(num_neigh_out != NULL)
		*num_neigh_out = num_neigh;
	else
		free(num_neigh);

	if(num_crs_neigh_out != NULL)
		*num_crs_neigh_out = num_crs_neigh;
	else
		free(num_crs_neigh);
}

void mark_em_nnz(_TYPE_I nnz, _TYPE_I * num_neigh, _TYPE_I * crs_num_neigh, int neigh_threshold_row, int neigh_threshold_col, _TYPE_I *nnz_marked_out, _TYPE_I **nnz_marking_out)
{
	int nnz_marked = 0;
	_TYPE_I * nnz_marking = (_TYPE_I*) calloc(nnz, sizeof(_TYPE_I));
	_Pragma("omp parallel for reduction(+:nnz_marked)")
	for(_TYPE_I i=0; i<nnz; i++){
		if((num_neigh[i]<=neigh_threshold_row) && (crs_num_neigh[i]<=neigh_threshold_col)){
			nnz_marked++;
			nnz_marking[i] = 1;
		}
	}
	printf("num_neigh <= %d && crs_num_neigh <= %d: %d (%.2lf \%)\n", neigh_threshold_row, neigh_threshold_col, nnz_marked, nnz_marked*1.0/nnz*100);
	*nnz_marked_out = nnz_marked;
	if(nnz_marking_out != NULL)
		*nnz_marking_out = nnz_marking;
}

void remove_marked_nnz(char *file_in, _TYPE_I * row_ptr, _TYPE_I * col_idx, _TYPE_V * val, _TYPE_I m, _TYPE_I n, _TYPE_I nnz, _TYPE_I * nnz_marking, _TYPE_I nnz_marked, int window_size, char * suffix, int neigh_threshold_row, int neigh_threshold_col, int store)
{
	// create a new csr representation, where only the not marked nonzeros will be passed
	_TYPE_I * h_row_idx_gpu, * h_row_idx_cpu;
	_TYPE_I * h_col_idx_gpu, * h_col_idx_cpu;
	_TYPE_V * h_val_gpu, * h_val_cpu;

	h_row_idx_gpu = (typeof(h_row_idx_gpu)) malloc((nnz-nnz_marked) * sizeof(*h_row_idx_gpu));
	h_col_idx_gpu = (typeof(h_col_idx_gpu)) malloc((nnz-nnz_marked) * sizeof(*h_col_idx_gpu));
	h_val_gpu = (typeof(h_val_gpu)) malloc((nnz-nnz_marked) * sizeof(*h_val_gpu));

	h_row_idx_cpu = (typeof(h_row_idx_cpu)) malloc(nnz_marked * sizeof(*h_row_idx_cpu));
	h_col_idx_cpu = (typeof(h_col_idx_cpu)) malloc(nnz_marked * sizeof(*h_col_idx_cpu));
	h_val_cpu = (typeof(h_val_cpu)) malloc(nnz_marked * sizeof(*h_val_cpu));

	_TYPE_I idx_gpu = 0, idx_cpu = 0;

	// time to pick the rows (and columns) that break it
	_TYPE_I *row_idx;
	csr_row_indices(row_ptr, col_idx, m, n, nnz, &row_idx);

	for(_TYPE_I i=0; i<nnz; i++){
		if(nnz_marking[i]==0) // it goes to GPU
		{
			h_row_idx_gpu[idx_gpu] = row_idx[i];
			h_col_idx_gpu[idx_gpu] = col_idx[i];
			h_val_gpu[idx_gpu] = val[i];
			idx_gpu++;
		}
		else // it goes to CPU
		{
			h_row_idx_cpu[idx_cpu] = row_idx[i];
			h_col_idx_cpu[idx_cpu] = col_idx[i];
			h_val_cpu[idx_cpu] = val[i];
			idx_cpu++;
		}
	}

	free(row_idx);
	// free(nnz_marking);

	_TYPE_I * row_ptr_gpu, * row_ptr_cpu;
	_TYPE_I * col_idx_gpu, * col_idx_cpu;
	_TYPE_V * val_gpu, * val_cpu;

	row_ptr_gpu = (typeof(row_ptr_gpu)) malloc((m+1) * sizeof(*row_ptr_gpu));
	col_idx_gpu = (typeof(col_idx_gpu)) malloc((nnz-nnz_marked) * sizeof(*col_idx_gpu));
	val_gpu = (typeof(val_gpu)) malloc((nnz-nnz_marked) * sizeof(*val_gpu));

	row_ptr_cpu = (typeof(row_ptr_cpu)) malloc((m+1) * sizeof(*row_ptr_cpu));
	col_idx_cpu = (typeof(col_idx_cpu)) malloc(nnz_marked * sizeof(*col_idx_cpu));
	val_cpu = (typeof(val_cpu)) malloc(nnz_marked * sizeof(*val_cpu));

	coo_to_csr(h_row_idx_gpu, h_col_idx_gpu, h_val_gpu, m, n, (nnz-nnz_marked), row_ptr_gpu, col_idx_gpu, val_gpu, 1, 0);
	coo_to_csr(h_row_idx_cpu, h_col_idx_cpu, h_val_cpu, m, n, nnz_marked, row_ptr_cpu, col_idx_cpu, val_cpu, 1, 0);

	free(h_row_idx_gpu); free(h_col_idx_gpu); free(h_val_gpu);
	free(h_row_idx_cpu); free(h_col_idx_cpu); free(h_val_cpu);

	char * membership_suffix;
	membership_suffix = (typeof(membership_suffix)) malloc(100*sizeof(*membership_suffix));
	sprintf(membership_suffix, "remove_n%d%d_GPU_ws%d%s", neigh_threshold_row, neigh_threshold_col, window_size, suffix);
	feature_plot_store_matrix(file_in, membership_suffix, 1, store, m, n, row_ptr_gpu[m], row_ptr_gpu, col_idx_gpu, val_gpu);
	free(membership_suffix);
	membership_suffix = (typeof(membership_suffix)) malloc(100*sizeof(*membership_suffix));
	sprintf(membership_suffix, "remove_n%d%d_CPU_ws%d%s", neigh_threshold_row, neigh_threshold_col, window_size, suffix);
	feature_plot_store_matrix(file_in, membership_suffix, 1, store, m, n, row_ptr_cpu[m], row_ptr_cpu, col_idx_cpu, val_cpu);
	free(membership_suffix);

	_TYPE_I * num_neigh_gpu, * crs_num_neigh_gpu;
	_TYPE_I * nnz_marking_gpu;
	_TYPE_I nnz_marked_gpu = 0;
	csr_extract_neighbour_stats(row_ptr_gpu, col_idx_gpu, m, n, row_ptr_gpu[m], window_size, "GPU", 1, &num_neigh_gpu, &crs_num_neigh_gpu);
	mark_em_nnz(row_ptr_gpu[m], num_neigh_gpu, crs_num_neigh_gpu, neigh_threshold_row, neigh_threshold_col, &nnz_marked_gpu, &nnz_marking_gpu);
	free(nnz_marking_gpu); 
	free(num_neigh_gpu); 
	free(crs_num_neigh_gpu);

	_TYPE_I * num_neigh_cpu, * crs_num_neigh_cpu;
	_TYPE_I * nnz_marking_cpu;
	_TYPE_I nnz_marked_cpu = 0;
	csr_extract_neighbour_stats(row_ptr_cpu, col_idx_cpu, m, n, row_ptr_cpu[m], window_size, "CPU", 1, &num_neigh_cpu, &crs_num_neigh_cpu);
	mark_em_nnz(row_ptr_cpu[m], num_neigh_cpu, crs_num_neigh_cpu, neigh_threshold_row, neigh_threshold_col, &nnz_marked_cpu, &nnz_marking_cpu);
	free(nnz_marking_cpu);
	free(num_neigh_cpu); 
	free(crs_num_neigh_cpu);
}

void inspect_and_extract(char *file_in, char * suffix, long m, long n, long nnz, _TYPE_I* row_ptr, _TYPE_I* col_idx, _TYPE_V* val)
{

	int window_size = atoi(getenv("WINDOW_SIZE"));
	int neigh_threshold_row = atoi(getenv("NEIGH_THRESHOLD_ROW"));
	int neigh_threshold_col = atoi(getenv("NEIGH_THRESHOLD_COL"));

	int histogram_print = 1;
	int store = atoi(getenv("STORE"));
	printf(">>> window_size = %d, neigh_threshold_row = %d, neigh_threshold_col = %d, store = %d\n", window_size, neigh_threshold_row, neigh_threshold_col, store);


	printf("------------------\n");
	_TYPE_I * num_neigh, * crs_num_neigh;
	_TYPE_I nnz_marked = 0;
	_TYPE_I * nnz_marking;

	csr_extract_neighbour_stats(row_ptr, col_idx, m, n, nnz, window_size, suffix, histogram_print, &num_neigh, &crs_num_neigh);
	mark_em_nnz(row_ptr[m], num_neigh, crs_num_neigh, neigh_threshold_row, neigh_threshold_col, &nnz_marked, &nnz_marking);

	remove_marked_nnz(file_in, row_ptr, col_idx, val, m, n, nnz, nnz_marking, nnz_marked, window_size, suffix, neigh_threshold_row, neigh_threshold_col, store);

	free(nnz_marking);
	free(num_neigh); 
	free(crs_num_neigh);
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
	// printf("time create_coo_matrix = %lf\n", time);

	row_ptr = (typeof(row_ptr)) malloc((m+1) * sizeof(*row_ptr));
	col_idx = (typeof(col_idx)) malloc(nnz * sizeof(*col_idx));
	val = (typeof(val)) malloc(nnz * sizeof(*val));


	long num_pixels = 4096;
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

	/*
	 * Convert to RCM 
	 */
	_TYPE_I * row_ptr_rcm; //for CSR format
	_TYPE_I * col_idx_rcm;
	_TYPE_V * val_rcm;
	_TYPE_I * permutation;
	reverse_cuthill_mckee(row_ptr, col_idx, val, m, n, nnz, &row_ptr_rcm, &col_idx_rcm, &val_rcm, &permutation);

	int store = 0;

	char * membership_suffix;
	membership_suffix = (typeof(membership_suffix)) malloc(100*sizeof(*membership_suffix));
	// sprintf(membership_suffix, "split_gpu_%.1lf", compute_ratio);
	sprintf(membership_suffix, "RCM");
	feature_plot_store_matrix(file_in, membership_suffix, 1, store, m, n, nnz, row_ptr_rcm, col_idx_rcm, val_rcm);
	free(membership_suffix);



	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// inspect_and_extract(file_in, "", m, n, nnz, row_ptr, col_idx, val);
	inspect_and_extract(file_in, "_RCM", m, n, nnz, row_ptr_rcm, col_idx_rcm, val_rcm);

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	free(row_ptr_rcm);
	free(col_idx_rcm);
	free(val_rcm);
	free(permutation);

	free(mtx_val);
	free(mtx_rowind);
	free(mtx_colind);

	// original matrix (CSR representation)
	free(row_ptr);
	free(col_idx);
	free(val);

	return 0;
}
