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
	{
		return 1;
	}

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

	time = time_it(1,
		csr_plot_f(file_fig, row_ptr, col_idx, val, m, n, nnz, 0, num_pixels_x, num_pixels_y);
	);
	printf("time plot = %lf\n", time);

	csr_matrix_features_validation(filename_base, row_ptr, col_idx, m, n, nnz);

	return 0;
}

