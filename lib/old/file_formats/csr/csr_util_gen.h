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
double csr_cross_row_neighbours(_TYPE_I * row_ptr, _TYPE_I * col_idx, long m, long n, long nnz, long window_size);


#undef  csr_value_features
#define csr_value_features  CSR_UTIL_GEN_EXPAND(csr_value_features)
void csr_value_features(char * title_base, _TYPE_I * row_ptr, _TYPE_I * col_idx, _TYPE_V * values, long m, long n, long nnz, int do_plot);

#undef  csr_matrix_features
#define csr_matrix_features  CSR_UTIL_GEN_EXPAND(csr_matrix_features)
void csr_matrix_features(char * title_base, _TYPE_I * row_ptr, _TYPE_I * col_idx, long m, long n, long nnz, int do_plot);

#undef  csr_matrix_features_validation
#define csr_matrix_features_validation  CSR_UTIL_GEN_EXPAND(csr_matrix_features_validation)
void csr_matrix_features_validation(char * title_base, _TYPE_I * row_ptr, _TYPE_I * col_idx, long m, long n, long nnz);

#undef  csr_plot
#define csr_plot  CSR_UTIL_GEN_EXPAND(csr_plot)
void csr_plot(char * title_base, _TYPE_I * row_ptr, _TYPE_I * col_idx, _TYPE_V * val, long m, long n, long nnz, int enable_legend);

