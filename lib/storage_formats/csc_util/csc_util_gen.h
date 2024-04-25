#if !defined(CSC_UTIL_GEN_TYPE_1)
	#error "CSC_UTIL_GEN_TYPE_1 not defined: value type"
#elif !defined(CSC_UTIL_GEN_TYPE_2)
	#error "CSC_UTIL_GEN_TYPE_2 not defined: index type"
#elif !defined(CSC_UTIL_GEN_SUFFIX)
	#error "CSC_UTIL_GEN_SUFFIX not defined"
#endif

#include "macros/cpp_defines.h"
#include "macros/macrolib.h"


#define CSC_UTIL_GEN_EXPAND(name)  CONCAT(name, CSC_UTIL_GEN_SUFFIX)

#undef  _TYPE_V
#define _TYPE_V  CSC_UTIL_GEN_EXPAND(_TYPE_V)
typedef CSC_UTIL_GEN_TYPE_1  _TYPE_V;

#undef  _TYPE_I
#define _TYPE_I  CSC_UTIL_GEN_EXPAND(_TYPE_I)
typedef CSC_UTIL_GEN_TYPE_2  _TYPE_I;


//==========================================================================================================================================
//= Functions
//==========================================================================================================================================


// Expand col indices.
#undef  csc_col_indices
#define csc_col_indices  CSC_UTIL_GEN_EXPAND(csc_col_indices)
void csc_col_indices(_TYPE_I * row_idx, _TYPE_I * col_ptr, long m, long n, long nnz, _TYPE_I ** col_idx_out);


// // Average nnz distances per row is: bandwidths / degrees_rows .
// #undef  csc_degrees_bandwidths_scatters
// #define csc_degrees_bandwidths_scatters  CSC_UTIL_GEN_EXPAND(csc_degrees_bandwidths_scatters)
// void csc_degrees_bandwidths_scatters(_TYPE_I * row_idx, _TYPE_I * col_ptr, long m, long n, long nnz,
// 		_TYPE_I ** degrees_rows_out, _TYPE_I ** degrees_cols_out, double ** bandwidths_out, double ** scatters_out);

// // #undef  csc_groups_per_col
// // #define csc_groups_per_col  CSC_UTIL_GEN_EXPAND(csc_groups_per_col)
// // void csc_groups_per_col(_TYPE_I * row_idx, _TYPE_I * col_ptr, long m, long n, long nnz, long max_gap_size, long ** groups_per_col_out);

// #undef  csc_column_distances_and_groupping
// #define csc_column_distances_and_groupping  CSC_UTIL_GEN_EXPAND(csc_column_distances_and_groupping)
// long csc_column_distances_and_groupping(_TYPE_I * row_idx, _TYPE_I * col_ptr, long m, long n, long nnz, long max_gap_size,
// 		_TYPE_I ** nnz_col_dist_out, _TYPE_I ** group_col_dist_out, _TYPE_I ** group_sizes_out, _TYPE_I ** groups_per_col_out);

#undef  csc_col_neighbours
#define csc_col_neighbours  CSC_UTIL_GEN_EXPAND(csc_col_neighbours)
void csc_col_neighbours(_TYPE_I * row_idx, _TYPE_I * col_ptr, long m, long n, long nnz, long window_size, _TYPE_I ** num_neigh_out);

#undef  csc_cross_col_neighbours
#define csc_cross_col_neighbours  CSC_UTIL_GEN_EXPAND(csc_cross_col_neighbours)
void csc_cross_col_neighbours(_TYPE_I * row_idx, _TYPE_I * col_ptr, long m, long n, long nnz, long window_size, _TYPE_I **ccs_neigh_out);

#undef  csc_cross_col_similarity
#define csc_cross_col_similarity  CSC_UTIL_GEN_EXPAND(csc_cross_col_similarity)
double csc_cross_col_similarity(_TYPE_I * row_idx, _TYPE_I * col_ptr, long m, long n, long nnz, long window_size);

// #undef  csc_value_features
// #define csc_value_features  CSC_UTIL_GEN_EXPAND(csc_value_features)
// void csc_value_features(char * title_base, _TYPE_I * row_idx, _TYPE_I * col_ptr, _TYPE_V * values, long m, long n, long nnz, int do_plot);

// #undef  csc_matrix_features
// #define csc_matrix_features  CSC_UTIL_GEN_EXPAND(csc_matrix_features)
// void csc_matrix_features(char * title_base, _TYPE_I * row_idx, _TYPE_I * col_ptr, long m, long n, long nnz, int do_plot);

// #undef  csc_matrix_features_validation
// #define csc_matrix_features_validation  CSC_UTIL_GEN_EXPAND(csc_matrix_features_validation)
// void csc_matrix_features_validation(char * title_base, _TYPE_I * row_idx, _TYPE_I * col_ptr, long m, long n, long nnz);

#undef  csc_save_to_mtx
#define csc_save_to_mtx  CSC_UTIL_GEN_EXPAND(csc_save_to_mtx)
void csc_save_to_mtx(_TYPE_I * row_idx, _TYPE_I * col_ptr, _TYPE_V * val, int num_rows, int num_cols, const char* filename);

//==========================================================================================================================================
//= Ploting
//==========================================================================================================================================


#undef  csc_plot
#define csc_plot  CSC_UTIL_GEN_EXPAND(csc_plot)
void csc_plot(char * title_base, _TYPE_I * row_idx, _TYPE_I * col_ptr, _TYPE_V * val, long m, long n, long nnz, int enable_legend, long num_pixels_x, long num_pixels_y);

#undef  csc_col_size_histogram_plot
#define csc_col_size_histogram_plot  CSC_UTIL_GEN_EXPAND(csc_col_size_histogram_plot)
void csc_col_size_histogram_plot(char * title_base, _TYPE_I * row_idx, _TYPE_I * col_ptr, _TYPE_V * val, long m, long n, long nnz, int enable_legend, long num_pixels_x, long num_pixels_y);

#undef  csc_nnz_size_batch_n_bar_plot
#define csc_nnz_size_batch_n_bar_plot  CSC_UTIL_GEN_EXPAND(csc_nnz_size_batch_n_bar_plot)
void csc_nnz_size_batch_n_bar_plot(char * title_base, _TYPE_I * row_idx, _TYPE_I * col_ptr, _TYPE_V * val, long m, long n, long nnz, int batch_n, int enable_legend, long num_pixels_x, long num_pixels_y);

#undef  csc_num_neigh_histogram_plot
#define csc_num_neigh_histogram_plot  CSC_UTIL_GEN_EXPAND(csc_num_neigh_histogram_plot)
void csc_num_neigh_histogram_plot(char * title_base, _TYPE_I * row_idx, _TYPE_I * col_ptr, _TYPE_V * val, long m, long n, long nnz, int window_size, int enable_legend, long num_pixels_x, long num_pixels_y);

#undef  csc_cross_col_similarity_histogram_plot
#define csc_cross_col_similarity_histogram_plot  CSC_UTIL_GEN_EXPAND(csc_cross_col_similarity_histogram_plot)
void csc_cross_col_similarity_histogram_plot(char * title_base, _TYPE_I * row_idx, _TYPE_I * col_ptr, _TYPE_V * val, long m, long n, long nnz, int window_size, int enable_legend, long num_pixels_x, long num_pixels_y);
