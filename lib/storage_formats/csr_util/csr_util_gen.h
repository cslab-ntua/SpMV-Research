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
//------------------------------------------------------------------------------------------------------------------------------------------
//-                                                              Functions                                                                 -
//------------------------------------------------------------------------------------------------------------------------------------------
//==========================================================================================================================================



//==========================================================================================================================================
//= Structural Features
//==========================================================================================================================================


// Expand row indices.
#undef  csr_row_indices
#define csr_row_indices  CSR_UTIL_GEN_EXPAND(csr_row_indices)
void csr_row_indices(_TYPE_I * row_ptr, _TYPE_I * col_idx, long m, long n, long nnz, _TYPE_I ** row_idx_out);

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


//------------------------------------------------------------------------------------------------------------------------------------------
//- Structural Features - Print
//------------------------------------------------------------------------------------------------------------------------------------------


#undef  csr_matrix_features
#define csr_matrix_features  CSR_UTIL_GEN_EXPAND(csr_matrix_features)
void csr_matrix_features(char * title_base, _TYPE_I * row_ptr, _TYPE_I * col_idx, long m, long n, long nnz, int do_plot, long num_pixels_x, long num_pixels_y);

#undef  csr_matrix_features_validation
#define csr_matrix_features_validation  CSR_UTIL_GEN_EXPAND(csr_matrix_features_validation)
void csr_matrix_features_validation(char * title_base, _TYPE_I * row_ptr, _TYPE_I * col_idx, long m, long n, long nnz);


//==========================================================================================================================================
//= Value Features
//==========================================================================================================================================


#undef  csr_value_features
#define csr_value_features  CSR_UTIL_GEN_EXPAND(csr_value_features)
void csr_value_features(char * title_base, _TYPE_I * row_ptr, _TYPE_I * col_idx, _TYPE_V * values, long m, long n, long nnz, int do_plot, long num_pixels_x, long num_pixels_y);


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

// #undef  csr_extract_row_cross
// #define csr_extract_row_cross  CSR_UTIL_GEN_EXPAND(csr_extract_row_cross)
// void csr_extract_row_cross(_TYPE_I *row_ptr, _TYPE_I *col_idx, __attribute__((unused)) _TYPE_V *val, int m, int n, int nnz, int window_width,
// 		int *num_windows_out, float **row_cross_out, int plot, _TYPE_I **rc_r_out, _TYPE_I **rc_c_out, float **rc_v_out);

#undef  csr_extract_row_cross2
#define csr_extract_row_cross2  CSR_UTIL_GEN_EXPAND(csr_extract_row_cross2)
void csr_extract_row_cross2(_TYPE_I *row_ptr, _TYPE_I *col_idx, __attribute__((unused)) _TYPE_V *val, int m, int n, int nnz, int window_width, 
							int *num_windows_out, _TYPE_I **rc_r_out, _TYPE_I **rc_c_out, float **rc_v_out);

#undef  csr_extract_row_cross2_batch
#define csr_extract_row_cross2_batch  CSR_UTIL_GEN_EXPAND(csr_extract_row_cross2_batch)
void csr_extract_row_cross2_batch(_TYPE_I *row_ptr, _TYPE_I *col_idx, __attribute__((unused)) _TYPE_V *val, int m, int n, int nnz, int window_width, int batch, 
								  int *num_windows_out, _TYPE_I **rc_r_out, _TYPE_I **rc_c_out, float **rc_v_out);

#undef  csr_extract_row_cross_char
#define csr_extract_row_cross_char  CSR_UTIL_GEN_EXPAND(csr_extract_row_cross_char)
void csr_extract_row_cross_char(_TYPE_I *row_ptr, _TYPE_I *col_idx, __attribute__((unused)) _TYPE_V *val, int m, int n, int nnz, int window_width,
		int *num_windows_out, unsigned char **row_cross_out, int plot, _TYPE_I **rc_r_out, _TYPE_I **rc_c_out, float **rc_v_out);

#undef  csr_extract_row_cross_char2
#define csr_extract_row_cross_char2  CSR_UTIL_GEN_EXPAND(csr_extract_row_cross_char2)
void csr_extract_row_cross_char2(_TYPE_I *row_ptr, _TYPE_I *col_idx, __attribute__((unused)) _TYPE_V *val, int m, int n, int nnz, int window_width, 
		int *num_windows_out, _TYPE_I **rc_r_out, _TYPE_I **rc_c_out, unsigned char **rc_v_out);

#undef  csr_extract_row_cross_char2_batch
#define csr_extract_row_cross_char2_batch  CSR_UTIL_GEN_EXPAND(csr_extract_row_cross_char2_batch)
void csr_extract_row_cross_char2_batch(_TYPE_I *row_ptr, _TYPE_I *col_idx, __attribute__((unused)) _TYPE_V *val, int m, int n, int nnz, int window_width, int batch, 
									   int *num_windows_out, _TYPE_I **rc_r_out, _TYPE_I **rc_c_out, unsigned char **rc_v_out);

#undef  csr_reorder_matrix_by_row
#define csr_reorder_matrix_by_row  CSR_UTIL_GEN_EXPAND(csr_reorder_matrix_by_row)
void csr_reorder_matrix_by_row(int m, __attribute__((unused)) int n, __attribute__((unused)) int nnz, _TYPE_I * row_ptr, _TYPE_I * col_idx, _TYPE_V * val, 
							   int * membership, _TYPE_I * row_ptr_reorder, _TYPE_I * col_idx_reorder, _TYPE_V * val_reorder, _TYPE_I * original_row_positions);

#undef  csr_reorder_matrix_by_row_batch
#define csr_reorder_matrix_by_row_batch  CSR_UTIL_GEN_EXPAND(csr_reorder_matrix_by_row_batch)
void csr_reorder_matrix_by_row_batch(int m, __attribute__((unused)) int n, __attribute__((unused)) int nnz, int batch, _TYPE_I * row_ptr, _TYPE_I * col_idx, _TYPE_V * val, 
									 int * membership, _TYPE_I * row_ptr_reorder, _TYPE_I * col_idx_reorder, _TYPE_V * val_reorder, _TYPE_I * original_row_positions);

#undef  csr_kmeans_reorder_row
#define csr_kmeans_reorder_row  CSR_UTIL_GEN_EXPAND(csr_kmeans_reorder_row)
void csr_kmeans_reorder_row(_TYPE_I * row_ptr, _TYPE_I * col_idx, _TYPE_V * val, 
							_TYPE_I * row_ptr_reorder_r, _TYPE_I * col_idx_reorder_r, _TYPE_V * val_reorder_r, _TYPE_I ** original_row_positions,
							int m, int n, int nnz, 
							int numClusters, float threshold, int loop_threshold, int num_windows, int seed, _TYPE_I * rc_r, _TYPE_I * rc_c, float * rc_v);

#undef  csr_kmeans_reorder_by_file
#define csr_kmeans_reorder_by_file  CSR_UTIL_GEN_EXPAND(csr_kmeans_reorder_by_file)
void csr_kmeans_reorder_by_file(char* reorder_file, 
								_TYPE_I * row_ptr, _TYPE_I * col_idx, _TYPE_V * val, 
								_TYPE_I * row_ptr_reorder_r, _TYPE_I * col_idx_reorder_r, _TYPE_V * val_reorder_r, _TYPE_I ** original_row_positions,
								int m, int n, int nnz);

#undef  csr_kmeans_reorder_row_batch
#define csr_kmeans_reorder_row_batch  CSR_UTIL_GEN_EXPAND(csr_kmeans_reorder_row_batch)
void csr_kmeans_reorder_row_batch(_TYPE_I * row_ptr, _TYPE_I * col_idx, _TYPE_V * val, 
								  _TYPE_I * row_ptr_reorder_r, _TYPE_I * col_idx_reorder_r, _TYPE_V * val_reorder_r, _TYPE_I ** original_row_positions, 
								  int m, int n, int nnz, int batch, 
								  int numClusters, float threshold, int loop_threshold, int num_windows, int seed, _TYPE_I * rc_r, _TYPE_I * rc_c, float * rc_v);

#undef  csr_kmeans_char_reorder_row
#define csr_kmeans_char_reorder_row  CSR_UTIL_GEN_EXPAND(csr_kmeans_char_reorder_row)
void csr_kmeans_char_reorder_row(_TYPE_I * row_ptr, _TYPE_I * col_idx, _TYPE_V * val, 
								 _TYPE_I * row_ptr_reorder_r, _TYPE_I * col_idx_reorder_r, _TYPE_V * val_reorder_r, _TYPE_I ** original_row_positions, 
								 int m, int n, int nnz, 
								 int numClusters, float threshold, int loop_threshold, int num_windows, int seed, _TYPE_I * rc_r, _TYPE_I * rc_c, unsigned char * rc_v);

#undef  csr_kmeans_char_reorder_row_batch
#define csr_kmeans_char_reorder_row_batch  CSR_UTIL_GEN_EXPAND(csr_kmeans_char_reorder_row_batch)
void csr_kmeans_char_reorder_row_batch(_TYPE_I * row_ptr, _TYPE_I * col_idx, _TYPE_V * val, 
									   _TYPE_I * row_ptr_reorder_r, _TYPE_I * col_idx_reorder_r, _TYPE_V * val_reorder_r, _TYPE_I ** original_row_positions, 
									   int m, int n, int nnz, int batch, 
									   int numClusters, float threshold, int loop_threshold, int num_windows, int seed, _TYPE_I * rc_r, _TYPE_I * rc_c, unsigned char * rc_v);

#undef  csr_save_to_mtx
#define csr_save_to_mtx  CSR_UTIL_GEN_EXPAND(csr_save_to_mtx)
void csr_save_to_mtx(_TYPE_I * row_ptr, _TYPE_I * col_idx, _TYPE_V * val, int num_rows, int num_cols, const char* filename);

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

