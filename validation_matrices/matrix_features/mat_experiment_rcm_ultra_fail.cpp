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
		// int window_size = 8192;
		// csr_cross_row_similarity_histogram_plot(file_fig, row_ptr, col_idx, val, m, n, nnz, window_size, 0);
		// csr_num_neigh_histogram_plot(file_fig, row_ptr, col_idx, val, m, n, nnz, window_size, 0);
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





	// TODO: need to fix this, so that it account for non_empty_rows only when calculating bw values.
	_TYPE_I * bandwidth_rows = (typeof(bandwidth_rows)) malloc(m * sizeof(*bandwidth_rows));
	#pragma omp parallel for
	for(int i=0; i<m; i++){
		if((row_ptr[i+1] - row_ptr[i]) == 0)
			bandwidth_rows[i] = 0;
		else
			bandwidth_rows[i] = col_idx[row_ptr[i+1]-1] - col_idx[row_ptr[i]];
	}
	double bandwidth_max = -1;
	for(int i=0; i<m; i++){
		if(bandwidth_max < bandwidth_rows[i])
			bandwidth_max = bandwidth_rows[i];
	}

	int num_bins = 10;
	int * bins = (typeof(bins))calloc(num_bins,sizeof(*bins));
	int * bins2 = (typeof(bins2))calloc(num_bins,sizeof(*bins2));
	double bin_size = bandwidth_max / num_bins;

	int * row_marking = (typeof(row_marking))calloc(m, sizeof(*row_marking));
	int low_row_cnt = 0, low_nnz_cnt = 0;
	// store = 0;
	store = 1;

	for(int i=0; i<m; i++){
		int bin_index = bandwidth_rows[i] / bin_size;
		if(bin_index == num_bins) bin_index = num_bins-1;
		bins[bin_index]++;
		bins2[bin_index] += row_ptr[i+1]-row_ptr[i];
		// for the last 3 bins
		if(bin_index > num_bins-4){
			if(row_ptr[i+1]-row_ptr[i] < 32){
				low_row_cnt++;
				low_nnz_cnt+= row_ptr[i+1]-row_ptr[i];
				row_marking[i] = 1;
			}
		}
	}
	printf("low row count = %d\n", low_row_cnt);
	printf("low nnz count = %d\n", low_nnz_cnt);
	printf("their memory footprint is %.2lf MB\n", 1.0*((low_row_cnt+1)*4 + low_nnz_cnt*(8+4))/(1024*1024));
	printf("histogram of bandwidth\n");
	for(int i=0; i<num_bins; i++) printf("Bin %d (%.0f - %.0f): rows %d (%.2lf %)\n", i, i*bin_size*100.0/bandwidth_max, (i+1)*bin_size*100.0/bandwidth_max, bins[i], bins[i]*100.0/m);
	free(bandwidth_rows);
	free(bins);
	free(bins2);

	_TYPE_I * nnz_marking = (typeof(nnz_marking))calloc(nnz, sizeof(*nnz_marking));
	int nnz_marked = 0;
	double time_mark = time_it(1,
	for(int i=0; i<m; i++){
		if(row_marking[i] == 1){
			for(int j=row_ptr[i]; j<row_ptr[i+1]; j++){
				nnz_marking[j] = 1;
				if(i != col_idx[j]){
					// printf("(%d, %d) = %lf\n", i, col_idx[j], val[j]);
					// find its symmetric
					for(int k=row_ptr[col_idx[j]]; k<row_ptr[col_idx[j]+1]; k++){
						if(col_idx[k] == i){
							// printf("(%d, %d) = %lf (found its symmetric!)\n", col_idx[j], col_idx[k], val[k]);
							nnz_marking[k] = 1;
							break;
						}
					}
				}
			}
		}
	}
	);
	printf("time for marking = %lf ms\n", time_mark*1000);
	#pragma omp parallel for reduction(+:nnz_marked)
	for(int i=0; i<nnz; i++){
		if(nnz_marking[i] == 1)
			nnz_marked++;
	}
	printf("nnz_marked = %d\n", nnz_marked);

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
	free(row_marking);
	free(nnz_marking);


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

	membership_suffix = (typeof(membership_suffix)) malloc(100*sizeof(*membership_suffix));
	sprintf(membership_suffix, "bw_GPU");
	feature_plot_store_matrix(file_in, membership_suffix, 1, store, m, n, row_ptr_gpu[m], row_ptr_gpu, col_idx_gpu, val_gpu);
	free(membership_suffix);

	membership_suffix = (typeof(membership_suffix)) malloc(100*sizeof(*membership_suffix));
	sprintf(membership_suffix, "bw_CPU");
	feature_plot_store_matrix(file_in, membership_suffix, 1, store, m, n, row_ptr_cpu[m], row_ptr_cpu, col_idx_cpu, val_cpu);
	free(membership_suffix);



	_TYPE_I * row_ptr_gpu_rcm, * row_ptr_cpu_rcm;
	_TYPE_I * col_idx_gpu_rcm, * col_idx_cpu_rcm;
	_TYPE_V * val_gpu_rcm, * val_cpu_rcm;

	_TYPE_I * permutation_gpu;
	_TYPE_I * permutation_cpu;
	reverse_cuthill_mckee(row_ptr_gpu, col_idx_gpu, val_gpu, m, n, row_ptr_gpu[m], &row_ptr_gpu_rcm, &col_idx_gpu_rcm, &val_gpu_rcm, &permutation_gpu);
	reverse_cuthill_mckee(row_ptr_cpu, col_idx_cpu, val_cpu, m, n, row_ptr_cpu[m], &row_ptr_cpu_rcm, &col_idx_cpu_rcm, &val_cpu_rcm, &permutation_cpu);

	free(permutation_gpu);
	free(permutation_cpu);

	membership_suffix = (typeof(membership_suffix)) malloc(100*sizeof(*membership_suffix));
	sprintf(membership_suffix, "bw_GPU_RCM");
	feature_plot_store_matrix(file_in, membership_suffix, 1, store, m, n, row_ptr_gpu_rcm[m], row_ptr_gpu_rcm, col_idx_gpu_rcm, val_gpu_rcm);
	free(membership_suffix);

	membership_suffix = (typeof(membership_suffix)) malloc(100*sizeof(*membership_suffix));
	sprintf(membership_suffix, "bw_CPU_RCM");
	feature_plot_store_matrix(file_in, membership_suffix, 1, store, m, n, row_ptr_cpu_rcm[m], row_ptr_cpu_rcm, col_idx_cpu_rcm, val_cpu_rcm);
	free(membership_suffix);


	free(row_ptr_gpu); free(col_idx_gpu); free(val_gpu);
	free(row_ptr_cpu); free(col_idx_cpu); free(val_cpu);

	free(row_ptr_gpu_rcm); free(col_idx_gpu_rcm); free(val_gpu_rcm);
	free(row_ptr_cpu_rcm); free(col_idx_cpu_rcm); free(val_cpu_rcm);

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
