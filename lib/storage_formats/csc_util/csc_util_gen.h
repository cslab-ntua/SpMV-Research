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


// #undef  csc_row_neighbours
// #define csc_row_neighbours  CSC_UTIL_GEN_EXPAND(csc_row_neighbours)
// void csc_row_neighbours(_TYPE_I * row_idx, _TYPE_I * col_ptr, long m, long n, long nnz, long window_size, _TYPE_I ** num_neigh_out);


// #undef  csc_cross_row_similarity
// #define csc_cross_row_similarity  CSC_UTIL_GEN_EXPAND(csc_cross_row_similarity)
// double csc_cross_row_similarity(_TYPE_I * row_idx, _TYPE_I * col_ptr, long m, long n, long nnz, long window_size);

// #undef  csc_cross_row_neighbours
// #define csc_cross_row_neighbours  CSC_UTIL_GEN_EXPAND(csc_cross_row_neighbours)
// double csc_cross_row_neighbours(_TYPE_I * row_idx, _TYPE_I * col_ptr, long m, long n, long nnz, long window_size);


// #undef  csc_value_features
// #define csc_value_features  CSC_UTIL_GEN_EXPAND(csc_value_features)
// void csc_value_features(char * title_base, _TYPE_I * row_idx, _TYPE_I * col_ptr, _TYPE_V * values, long m, long n, long nnz, int do_plot);

// #undef  csc_matrix_features
// #define csc_matrix_features  CSC_UTIL_GEN_EXPAND(csc_matrix_features)
// void csc_matrix_features(char * title_base, _TYPE_I * row_idx, _TYPE_I * col_ptr, long m, long n, long nnz, int do_plot);

// #undef  csc_matrix_features_validation
// #define csc_matrix_features_validation  CSC_UTIL_GEN_EXPAND(csc_matrix_features_validation)
// void csc_matrix_features_validation(char * title_base, _TYPE_I * row_idx, _TYPE_I * col_ptr, long m, long n, long nnz);

#undef  csc_shuffle_matrix
#define csc_shuffle_matrix  CSC_UTIL_GEN_EXPAND(csc_shuffle_matrix)
void csc_shuffle_matrix(long n, _TYPE_I *row_idx, _TYPE_I *col_ptr, _TYPE_V *values, _TYPE_I *row_idx_shuffle, _TYPE_I *col_ptr_shuffle, _TYPE_V *values_shuffle);

// #undef  csc_extract_col_cross
// #define csc_extract_col_cross  CSC_UTIL_GEN_EXPAND(csc_extract_col_cross)
// void csc_extract_col_cross(_TYPE_I *row_idx, _TYPE_I *col_ptr, __attribute__((unused)) _TYPE_V *val, int m, int n, int nnz, int window_width, 
// 						   int *num_windows_out, float **col_cross_out, int plot, _TYPE_I **cc_r_out, _TYPE_I **cc_c_out, float **cc_v_out);

#undef  csc_extract_col_cross2
#define csc_extract_col_cross2  CSC_UTIL_GEN_EXPAND(csc_extract_col_cross2)
void csc_extract_col_cross2(_TYPE_I *row_idx, _TYPE_I *col_ptr, __attribute__((unused)) _TYPE_V *val, int m, int n, int nnz, int window_width, 
							int *num_windows_out, _TYPE_I **cc_r_out, _TYPE_I **cc_c_out, float **cc_v_out);

#undef  csc_extract_col_cross2_batch
#define csc_extract_col_cross2_batch  CSC_UTIL_GEN_EXPAND(csc_extract_col_cross2_batch)
void csc_extract_col_cross2_batch(_TYPE_I *row_idx, _TYPE_I *col_ptr, __attribute__((unused)) _TYPE_V *val, int m, int n, int nnz, int window_width, int batch, 
								  int *num_windows_out, _TYPE_I **cc_r_out, _TYPE_I **cc_c_out, float **cc_v_out);

#undef  csc_extract_col_cross_char2
#define csc_extract_col_cross_char2  CSC_UTIL_GEN_EXPAND(csc_extract_col_cross_char2)
void csc_extract_col_cross_char2(_TYPE_I *row_idx, _TYPE_I *col_ptr, __attribute__((unused)) _TYPE_V *val, int m, int n, int nnz, int window_width, 
							int *num_windows_out, _TYPE_I **cc_r_out, _TYPE_I **cc_c_out, unsigned char **cc_v_out);

#undef  csc_extract_col_cross_char2_batch
#define csc_extract_col_cross_char2_batch  CSC_UTIL_GEN_EXPAND(csc_extract_col_cross_char2_batch)
void csc_extract_col_cross_char2_batch(_TYPE_I *row_idx, _TYPE_I *col_ptr, __attribute__((unused)) _TYPE_V *val, int m, int n, int nnz, int window_width, int batch, 
									   int *num_windows_out, _TYPE_I **cc_r_out, _TYPE_I **cc_c_out, unsigned char **cc_v_out);

#undef  csc_reorder_matrix_by_col
#define csc_reorder_matrix_by_col  CSC_UTIL_GEN_EXPAND(csc_reorder_matrix_by_col)
void csc_reorder_matrix_by_col(__attribute__((unused)) int m, int n, __attribute__((unused)) int nnz, _TYPE_I * row_idx, _TYPE_I * col_ptr, _TYPE_V * val, 
							   int * membership, _TYPE_I * row_idx_reorder, _TYPE_I * col_ptr_reorder, _TYPE_V * val_reorder, _TYPE_I * original_col_positions);

#undef  csc_reorder_matrix_by_col_batch
#define csc_reorder_matrix_by_col_batch  CSC_UTIL_GEN_EXPAND(csc_reorder_matrix_by_col_batch)
void csc_reorder_matrix_by_col_batch(__attribute__((unused)) int m, int n, __attribute__((unused)) int nnz, int batch, _TYPE_I * row_idx, _TYPE_I * col_ptr, _TYPE_V * val, 
									 int * membership, _TYPE_I * row_idx_reorder, _TYPE_I * col_ptr_reorder, _TYPE_V * val_reorder, _TYPE_I * original_col_positions);

#undef  csc_kmeans_reorder_col
#define csc_kmeans_reorder_col  CSC_UTIL_GEN_EXPAND(csc_kmeans_reorder_col)
void csc_kmeans_reorder_col(_TYPE_I * row_idx, _TYPE_I * col_ptr, _TYPE_V * val, 
							_TYPE_I * row_idx_reorder_c, _TYPE_I * col_ptr_reorder_c, _TYPE_V * val_reorder_c, _TYPE_I ** original_col_positions,
							int m, int n, int nnz, 
							int numClusters, float threshold, int loop_threshold, int num_windows, _TYPE_I * cc_r, _TYPE_I * cc_c, float * cc_v);

#undef  csc_kmeans_reorder_col_batch
#define csc_kmeans_reorder_col_batch  CSC_UTIL_GEN_EXPAND(csc_kmeans_reorder_col_batch)
void csc_kmeans_reorder_col_batch(_TYPE_I * row_idx, _TYPE_I * col_ptr, _TYPE_V * val, 
								  _TYPE_I * row_idx_reorder_c, _TYPE_I * col_ptr_reorder_c, _TYPE_V * val_reorder_c, _TYPE_I ** original_col_positions,
								  int m, int n, int nnz, int batch, 
								  int numClusters, float threshold, int loop_threshold, int num_windows, _TYPE_I * cc_r, _TYPE_I * cc_c, float * cc_v);

#undef  csc_kmeans_char_reorder_col
#define csc_kmeans_char_reorder_col  CSC_UTIL_GEN_EXPAND(csc_kmeans_char_reorder_col)
void csc_kmeans_char_reorder_col(_TYPE_I * row_idx, _TYPE_I * col_ptr, _TYPE_V * val, 
								 _TYPE_I * row_idx_reorder_c, _TYPE_I * col_ptr_reorder_c, _TYPE_V * val_reorder_c, _TYPE_I ** original_col_positions,
								 int m, int n, int nnz, 
								 int numClusters, float threshold, int loop_threshold, int num_windows, _TYPE_I * cc_r, _TYPE_I * cc_c, unsigned char * cc_v);

#undef  csc_kmeans_char_reorder_col_batch
#define csc_kmeans_char_reorder_col_batch  CSC_UTIL_GEN_EXPAND(csc_kmeans_char_reorder_col_batch)
void csc_kmeans_char_reorder_col_batch(_TYPE_I * row_idx, _TYPE_I * col_ptr, _TYPE_V * val, 
									   _TYPE_I * row_idx_reorder_c, _TYPE_I * col_ptr_reorder_c, _TYPE_V * val_reorder_c, _TYPE_I ** original_col_positions,
									   int m, int n, int nnz, int batch, 
									   int numClusters, float threshold, int loop_threshold, int num_windows, _TYPE_I * cc_r, _TYPE_I * cc_c, unsigned char * cc_v);

#undef  csc_save_to_mtx
#define csc_save_to_mtx  CSC_UTIL_GEN_EXPAND(csc_save_to_mtx)
void csc_save_to_mtx(_TYPE_I * row_idx, _TYPE_I * col_ptr, _TYPE_V * val, int num_rows, int num_cols, const char* filename);

//==========================================================================================================================================
//= Ploting
//==========================================================================================================================================


#undef  csc_plot
#define csc_plot  CSC_UTIL_GEN_EXPAND(csc_plot)
void csc_plot(char * title_base, _TYPE_I * row_idx, _TYPE_I * col_ptr, _TYPE_V * val, long m, long n, long nnz, int enable_legend, long num_pixels_x, long num_pixels_y);

