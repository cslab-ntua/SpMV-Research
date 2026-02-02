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
#ifdef __cplusplus
}
#endif


#include "read_mtx.h"

// #include "util.h"
// #include "matrix_util.h"

void slice_it(char * filename_base, int * row_ptr, int * col_idx, int m, int n, int nnz, int num_slices, int row_slicing)
{
	for (int s = 0; s < num_slices; s++) {
		int slice_start_row, slice_end_row;

		if (row_slicing) {
			// Option 1: Equal Number of Rows
			int rows_per_slice = (m + num_slices - 1) / num_slices;
			slice_start_row = s * rows_per_slice;
			slice_end_row = (s + 1) * rows_per_slice;
			if (slice_end_row > m) 
				slice_end_row = m;

		} else {
			// Option 2: Equal Number of Non-zeros (NNZ)
			int target_nnz_per_slice = nnz / num_slices;

			// Find start row for this slice
			if (s == 0) {
				slice_start_row = 0;
			} else {
				// Previous slice's end is this slice's start
				// (Calculated iteratively or stored)
				slice_start_row = slice_end_row;
			}

			// Binary search or linear scan row_ptr to find row that hits target NNZ
			// Linear scan example for simplicity:
			int current_nnz = 0;
			int r = (s == 0) ? 0 : slice_start_row; 
			while (r < m && current_nnz < target_nnz_per_slice) {
				current_nnz += row_ptr[r + 1] - row_ptr[r];
				r++;
			}
			slice_end_row = r;
			if (s == num_slices - 1) slice_end_row = m; // Last slice takes remainder

			// Update next start row for the next iteration
			// slice_start_row = slice_end_row; // Logic handled by loop state
		}

		// Calculate slice dimensions
		int slice_m = slice_end_row - slice_start_row;
		int slice_nnz = row_ptr[slice_end_row] - row_ptr[slice_start_row];

		// Create a slice-specific name for output
		long buf_n = 1000;
		char slice_name[buf_n];
		snprintf(slice_name, buf_n, "%s_slice_%d", filename_base, s);
		printf("slice %d (%s): rows %d to %d, rows = %d, nnz = %d\n", s, slice_name, slice_start_row, slice_end_row - 1, slice_m, slice_nnz);

		// Call validation on the slice
		// row_ptr + slice_start_row: shifts the pointer to the start of the slice
		// col_idx + row_ptr[slice_start_row]: shifts to the first column index of the slice
		if (slice_m > 0) {
			int * tmp_row_ptr = (int *) malloc((slice_m + 1) * sizeof(int));
			for (int r = 0; r <= slice_m; r++) tmp_row_ptr[r] = row_ptr[slice_start_row + r] - row_ptr[slice_start_row];
			int * tmp_col_idx = (int *) malloc(slice_nnz * sizeof(int));
			for (int idx = 0; idx < slice_nnz; idx++) tmp_col_idx[idx] = col_idx[row_ptr[slice_start_row] + idx];
			csr_matrix_features_validation(slice_name, tmp_row_ptr, tmp_col_idx, slice_m, n, slice_nnz);

			free(tmp_row_ptr);
			free(tmp_col_idx);
		}
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

	char * file_in;
	char * path, * filename, * filename_base;

	i = 1;
	file_in = argv[i++];

	str_path_split_path(file_in, strlen(file_in) + 1, buf, buf_n, &path, &filename);
	path = strdup(path);
	filename = strdup(filename);

	str_path_split_ext(filename, strlen(filename) + 1, buf, buf_n, &filename_base, NULL);
	filename_base = strdup(filename_base);
	snprintf(buf, buf_n, "figures_new/%s", filename_base);
	char * file_fig;
	file_fig = strdup(buf);

	time = time_it(1,
		create_coo_matrix(file_in, &mtx_val, &mtx_rowind, &mtx_colind, &m, &n, &nnz);
	);
	printf("time create_coo_matrix = %lf\n", time);

	row_ptr = (typeof(row_ptr)) malloc((m+1) * sizeof(*row_ptr));
	col_idx = (typeof(col_idx)) malloc(nnz * sizeof(*col_idx));
	val = (typeof(val)) malloc(nnz * sizeof(*val));
	printf("memory footprint = %.2f MB\n", (nnz * (sizeof(*col_idx)+ sizeof(*val)) + (m+1) * sizeof(*row_ptr))/(1024*1024.0));


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

	// time = time_it(1,
	//	csr_plot_f(file_fig, row_ptr, col_idx, val, m, n, nnz, 0, num_pixels_x, num_pixels_y);
	// 	csr_row_size_histogram_plot(file_fig, row_ptr, col_idx, val, m, n, nnz, 1, 1024, 1024);
	// 	csr_num_neigh_histogram_plot(file_fig, row_ptr, col_idx, val, m, n, nnz, 1, 1, num_pixels_x, num_pixels_y);
	// 	csr_cross_row_similarity_histogram_plot(file_fig, row_ptr, col_idx, val, m, n, nnz, 1, 1, num_pixels_x, num_pixels_y);
	// );
	// printf("time plot = %lf\n", time);

	csr_matrix_features_validation(filename_base, row_ptr, col_idx, m, n, nnz);

	// --- Slicing Configuration ---
	int num_slices = 10;
	int row_slicing = 1; // 1 for equal rows per slice, 0 for equal non-zeros per slice
	// -----------------------------
	// slice_it(filename_base, row_ptr, col_idx, m, n, nnz, num_slices, row_slicing)
	// slice_it(filename_base, row_ptr, col_idx, m, n, nnz, 10, 1);
	slice_it(filename_base, row_ptr, col_idx, m, n, nnz, 10, 0);


	free(mtx_rowind);
	free(mtx_colind);
	free(mtx_val);

	free(row_ptr);
	free(col_idx);
	free(val);

	return 0;
}
