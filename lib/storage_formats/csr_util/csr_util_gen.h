#if !defined(CSR_UTIL_GEN_TYPE_1)
	#error "CSR_UTIL_GEN_TYPE_1 not defined: value type"
#elif !defined(CSR_UTIL_GEN_TYPE_2)
	#error "CSR_UTIL_GEN_TYPE_2 not defined: index type"
#elif !defined(CSR_UTIL_GEN_SUFFIX)
	#error "CSR_UTIL_GEN_SUFFIX not defined"
#endif

#include "macros/cpp_defines.h"
#include "macros/macrolib.h"


#define CSR_UTIL_GEN_EXPAND(name)  CONCAT(name, CSR_UTIL_GEN_SUFFIX)

#undef  _TYPE_V
#define _TYPE_V  CSR_UTIL_GEN_EXPAND(_TYPE_V)
typedef CSR_UTIL_GEN_TYPE_1  _TYPE_V;

#undef  _TYPE_I
#define _TYPE_I  CSR_UTIL_GEN_EXPAND(_TYPE_I)
typedef CSR_UTIL_GEN_TYPE_2  _TYPE_I;


//==========================================================================================================================================
//= Functions
//==========================================================================================================================================


// Expand row indexes.
#undef  csr_row_indexes
#define csr_row_indexes  CSR_UTIL_GEN_EXPAND(csr_row_indexes)
void csr_row_indexes(_TYPE_I * row_ptr, _TYPE_I * col_idx, long m, long n, long nnz, _TYPE_I ** row_idx_out);

// Returns how many rows in matrix have less than "nnz_threshold" nonzeros
#undef  csr_count_short_rows
#define csr_count_short_rows  CSR_UTIL_GEN_EXPAND(csr_count_short_rows)
long  csr_count_short_rows(_TYPE_I * row_ptr, long m, long nnz_threshold);

// Returns how many nonzeros are "distant" from others within same row (depending on "max_distance")
#undef  csr_count_distant_nonzeros
#define csr_count_distant_nonzeros  CSR_UTIL_GEN_EXPAND(csr_count_distant_nonzeros)
long csr_count_distant_nonzeros(_TYPE_I * row_ptr, _TYPE_I * col_idx, long m, long max_distance);

// Average nnz distances per row is: bandwidths / degrees_rows .
#undef  csr_degrees_bandwidths_scatters
#define csr_degrees_bandwidths_scatters  CSR_UTIL_GEN_EXPAND(csr_degrees_bandwidths_scatters)
void csr_degrees_bandwidths_scatters(_TYPE_I * row_ptr, _TYPE_I * col_idx, long m, long n, long nnz,
		_TYPE_I ** degrees_rows_out, _TYPE_I ** degrees_cols_out, double ** bandwidths_out, double ** scatters_out);


// #undef  csr_groups_per_row
// #define csr_groups_per_row  CSR_UTIL_GEN_EXPAND(csr_groups_per_row)
// void csr_groups_per_row(_TYPE_I * row_ptr, _TYPE_I * col_idx, long m, long n, long nnz, long max_gap_size, long ** groups_per_row_out);

#undef  csr_column_distances_and_groupping
#define csr_column_distances_and_groupping  CSR_UTIL_GEN_EXPAND(csr_column_distances_and_groupping)
long csr_column_distances_and_groupping(_TYPE_I * row_ptr, _TYPE_I * col_idx, long m, long n, long nnz, long max_gap_size,
		_TYPE_I ** nnz_col_dist_out, _TYPE_I ** group_col_dist_out, _TYPE_I ** group_sizes_out, _TYPE_I ** groups_per_row_out);


#undef  csr_row_neighbours
#define csr_row_neighbours  CSR_UTIL_GEN_EXPAND(csr_row_neighbours)
void csr_row_neighbours(_TYPE_I * row_ptr, _TYPE_I * col_idx, long m, long n, long nnz, long window_size, _TYPE_I ** num_neigh_out);


#undef  csr_cross_row_similarity
#define csr_cross_row_similarity  CSR_UTIL_GEN_EXPAND(csr_cross_row_similarity)
double csr_cross_row_similarity(_TYPE_I * row_ptr, _TYPE_I * col_idx, long m, long n, long nnz, long window_size);

#undef  csr_cross_row_neighbours
#define csr_cross_row_neighbours  CSR_UTIL_GEN_EXPAND(csr_cross_row_neighbours)
double csr_cross_row_neighbours(_TYPE_I * row_ptr, _TYPE_I * col_idx, long m, long n, long nnz, long window_size, _TYPE_I * crs_row);


#undef  csr_value_features
#define csr_value_features  CSR_UTIL_GEN_EXPAND(csr_value_features)
void csr_value_features(char * title_base, _TYPE_I * row_ptr, _TYPE_I * col_idx, _TYPE_V * values, long m, long n, long nnz, int do_plot, long num_pixels_x, long num_pixels_y);

#undef  csr_matrix_features
#define csr_matrix_features  CSR_UTIL_GEN_EXPAND(csr_matrix_features)
void csr_matrix_features(char * title_base, _TYPE_I * row_ptr, _TYPE_I * col_idx, long m, long n, long nnz, int do_plot, long num_pixels_x, long num_pixels_y);

#undef  csr_matrix_features_validation
#define csr_matrix_features_validation  CSR_UTIL_GEN_EXPAND(csr_matrix_features_validation)
void csr_matrix_features_validation(char * title_base, _TYPE_I * row_ptr, _TYPE_I * col_idx, long m, long n, long nnz);


//==========================================================================================================================================
//= Matrix transformations
//==========================================================================================================================================


#undef  csr_sort_by_row_size
#define csr_sort_by_row_size  CSR_UTIL_GEN_EXPAND(csr_sort_by_row_size)
void csr_sort_by_row_size(long m, _TYPE_I * row_ptr, _TYPE_I * col_idx, _TYPE_V * values, _TYPE_I * row_ptr_s, _TYPE_I * col_idx_s, _TYPE_V * values_s);

// "distant_mark" is a 0-1 flag matrix, which holds which nonzeros are distant from others, in order to isolate them later
#undef  csr_mark_distant_nonzeros
#define csr_mark_distant_nonzeros  CSR_UTIL_GEN_EXPAND(csr_mark_distant_nonzeros)
long csr_mark_distant_nonzeros(_TYPE_I * row_ptr, _TYPE_I * col_idx, long m, long max_distance, _TYPE_I * distant_mark);

#undef  csr_separate_close_distant
#define csr_separate_close_distant  CSR_UTIL_GEN_EXPAND(csr_separate_close_distant)
void csr_separate_close_distant(_TYPE_I *row_ptr, _TYPE_I *col_idx, _TYPE_V *values, _TYPE_I *distant_mark, long nnz, long m, _TYPE_I *row_ptr_close, _TYPE_I *col_idx_close, _TYPE_V *values_close, _TYPE_I *row_ptr_distant, _TYPE_I *col_idx_distant, _TYPE_V *values_distant);

#undef  csr_shuffle_matrix
#define csr_shuffle_matrix  CSR_UTIL_GEN_EXPAND(csr_shuffle_matrix)
void csr_shuffle_matrix(long m, _TYPE_I *row_ptr, _TYPE_I *col_idx, _TYPE_V *values, _TYPE_I *row_ptr_shuffle, _TYPE_I *col_idx_shuffle, _TYPE_V *values_shuffle);

#undef  csr_extract_row_cross
#define csr_extract_row_cross  CSR_UTIL_GEN_EXPAND(csr_extract_row_cross)
void csr_extract_row_cross(_TYPE_I *row_ptr, _TYPE_I *col_idx, __attribute__((unused)) _TYPE_V *val, int m, int n, int nnz, int window_width, 
						   int *num_windows_out, float **row_cross_out, _TYPE_I **rc_r_out, _TYPE_I **rc_c_out, float **rc_v_out);


//==========================================================================================================================================
//= Ploting
//==========================================================================================================================================


#undef  csr_plot
#define csr_plot  CSR_UTIL_GEN_EXPAND(csr_plot)
void csr_plot(char * title_base, _TYPE_I * row_ptr, _TYPE_I * col_idx, _TYPE_V * val, long m, long n, long nnz, int enable_legend, long num_pixels_x, long num_pixels_y);

#undef  csr_row_size_histogram_plot
#define csr_row_size_histogram_plot  CSR_UTIL_GEN_EXPAND(csr_row_size_histogram_plot)
void csr_row_size_histogram_plot(char * title_base, _TYPE_I * row_ptr, __attribute__((unused)) _TYPE_I * col_idx, __attribute__((unused)) _TYPE_V * val, long m, long n, __attribute__((unused)) long nnz, int enable_legend, long num_pixels_x, long num_pixels_y);

#undef  csr_cross_row_similarity_histogram_plot
#define csr_cross_row_similarity_histogram_plot  CSR_UTIL_GEN_EXPAND(csr_cross_row_similarity_histogram_plot)
void csr_cross_row_similarity_histogram_plot(char * title_base, _TYPE_I * row_ptr, _TYPE_I * col_idx, __attribute__((unused)) _TYPE_V * val, long m, long n, long nnz, int window_size, int enable_legend, long num_pixels_x, long num_pixels_y);

#undef  csr_num_neigh_histogram_plot
#define csr_num_neigh_histogram_plot  CSR_UTIL_GEN_EXPAND(csr_num_neigh_histogram_plot)
void csr_num_neigh_histogram_plot(char * title_base, _TYPE_I * row_ptr, _TYPE_I * col_idx, __attribute__((unused)) _TYPE_V * val, long m, long n, long nnz, int window_size, int enable_legend, long num_pixels_x, long num_pixels_y);

