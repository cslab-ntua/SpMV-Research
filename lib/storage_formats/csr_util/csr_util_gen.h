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
#define CSR_UTIL_GEN_EXPAND_TYPE(name)  CONCAT(CSR_UTIL_GEN_, CONCAT(name, CSR_UTIL_GEN_SUFFIX))

#undef  _TYPE_V
#define _TYPE_V  CSR_UTIL_GEN_EXPAND_TYPE(_TYPE_V)
typedef CSR_UTIL_GEN_TYPE_1  _TYPE_V;

#undef  _TYPE_I
#define _TYPE_I  CSR_UTIL_GEN_EXPAND_TYPE(_TYPE_I)
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

#undef  csr_bandwidth_histogram_plot
#define csr_bandwidth_histogram_plot  CSR_UTIL_GEN_EXPAND(csr_bandwidth_histogram_plot)
void csr_bandwidth_histogram_plot(char * title_base, _TYPE_I * row_ptr, _TYPE_I * col_idx, __attribute__((unused)) _TYPE_V * val, long m, __attribute__((unused)) long n, __attribute__((unused)) long nnz, int enable_legend, long num_pixels_x, long num_pixels_y);

#undef  csr_bandwidth_batch_nnz_bar_plot
#define csr_bandwidth_batch_nnz_bar_plot  CSR_UTIL_GEN_EXPAND(csr_bandwidth_batch_nnz_bar_plot)
void csr_bandwidth_batch_nnz_bar_plot(char * title_base, __attribute__((unused)) _TYPE_I * row_ptr, _TYPE_I * col_idx, __attribute__((unused)) _TYPE_V * val, __attribute__((unused)) long m, long n, __attribute__((unused)) long nnz, int batch_nnz, int enable_legend, long num_pixels_x, long num_pixels_y);


//==========================================================================================================================================
//= Quantize
//==========================================================================================================================================


/* We expect sorted columns in each row.
 */
#undef  csr_quantize_columns
#define csr_quantize_columns  CSR_UTIL_GEN_EXPAND(csr_quantize_columns)
void csr_quantize_columns(_TYPE_I * row_ptr, _TYPE_I * col_idx, long m, [[gnu::unused]] long n, long nnz, long window_len, _TYPE_I ** q_row_ptr_out, _TYPE_I ** q_col_idx_out, _TYPE_I ** q_frequencies_out, _TYPE_I * q_nnz_out);


//==========================================================================================================================================
//= Quantize
//==========================================================================================================================================


#undef  csr_reorder_rows
#define csr_reorder_rows  CSR_UTIL_GEN_EXPAND(csr_reorder_rows)
void csr_reorder_rows(_TYPE_I * permutation, _TYPE_I * row_ptr, _TYPE_I * col_idx, _TYPE_V * values, long m, [[gnu::unused]] long n, long nnz, _TYPE_I * reordered_row_ptr, _TYPE_I * reordered_col_idx, _TYPE_V * reordered_values);

