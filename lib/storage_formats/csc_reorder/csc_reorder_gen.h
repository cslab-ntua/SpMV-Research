#if !defined(CSC_REORDER_GEN_TYPE_1)
	#error "CSC_REORDER_GEN_TYPE_1 not defined: value type"
#elif !defined(CSC_REORDER_GEN_TYPE_2)
	#error "CSC_REORDER_GEN_TYPE_2 not defined: index type"
#elif !defined(CSC_REORDER_GEN_SUFFIX)
	#error "CSC_REORDER_GEN_SUFFIX not defined"
#endif

#include "macros/cpp_defines.h"
#include "macros/macrolib.h"


#define CSC_REORDER_GEN_EXPAND(name)  CONCAT(name, CSC_REORDER_GEN_SUFFIX)

#undef  _TYPE_V
#define _TYPE_V  CSC_REORDER_GEN_EXPAND(_TYPE_V)
typedef CSC_REORDER_GEN_TYPE_1  _TYPE_V;

#undef  _TYPE_I
#define _TYPE_I  CSC_REORDER_GEN_EXPAND(_TYPE_I)
typedef CSC_REORDER_GEN_TYPE_2  _TYPE_I;


//==========================================================================================================================================
//------------------------------------------------------------------------------------------------------------------------------------------
//-                                                              Functions                                                                 -
//------------------------------------------------------------------------------------------------------------------------------------------
//==========================================================================================================================================

//==========================================================================================================================================
//= Matrix transformations
//==========================================================================================================================================

#undef  csc_sort_by_col_size
#define csc_sort_by_col_size  CSC_REORDER_GEN_EXPAND(csc_sort_by_col_size)
void csc_sort_by_col_size(long n, _TYPE_I * row_idx, _TYPE_I * col_ptr, _TYPE_V * values, _TYPE_I * row_idx_s, _TYPE_I * col_ptr_s, _TYPE_V * values_s);

#undef  csc_shuffle_matrix
#define csc_shuffle_matrix  CSC_REORDER_GEN_EXPAND(csc_shuffle_matrix)
void csc_shuffle_matrix(long n, _TYPE_I *row_idx, _TYPE_I *col_ptr, _TYPE_V *values, _TYPE_I *row_idx_shuffle, _TYPE_I *col_ptr_shuffle, _TYPE_V *values_shuffle);

// #undef  csc_extract_col_cross
// #define csc_extract_col_cross  CSC_REORDER_GEN_EXPAND(csc_extract_col_cross)
// void csc_extract_col_cross(_TYPE_I *row_idx, _TYPE_I *col_ptr, __attribute__((unused)) _TYPE_V *val, int m, int n, int nnz, int window_width, 
// 						   int *num_windows_out, float **col_cross_out, int plot, _TYPE_I **cc_r_out, _TYPE_I **cc_c_out, float **cc_v_out);

#undef  csc_extract_col_cross2
#define csc_extract_col_cross2  CSC_REORDER_GEN_EXPAND(csc_extract_col_cross2)
void csc_extract_col_cross2(_TYPE_I *row_idx, _TYPE_I *col_ptr, __attribute__((unused)) _TYPE_V *val, int m, int n, int nnz, int window_width, 
							int *num_windows_out, _TYPE_I **cc_r_out, _TYPE_I **cc_c_out, float **cc_v_out);

#undef  csc_extract_col_cross2_batch
#define csc_extract_col_cross2_batch  CSC_REORDER_GEN_EXPAND(csc_extract_col_cross2_batch)
void csc_extract_col_cross2_batch(_TYPE_I *row_idx, _TYPE_I *col_ptr, __attribute__((unused)) _TYPE_V *val, int m, int n, int nnz, int window_width, int batch, 
								  int *num_windows_out, _TYPE_I **cc_r_out, _TYPE_I **cc_c_out, float **cc_v_out);

#undef  csc_extract_col_cross_char2
#define csc_extract_col_cross_char2  CSC_REORDER_GEN_EXPAND(csc_extract_col_cross_char2)
void csc_extract_col_cross_char2(_TYPE_I *row_idx, _TYPE_I *col_ptr, __attribute__((unused)) _TYPE_V *val, int m, int n, int nnz, int window_width, 
							int *num_windows_out, _TYPE_I **cc_r_out, _TYPE_I **cc_c_out, unsigned char **cc_v_out);

#undef  csc_extract_col_cross_char2_batch
#define csc_extract_col_cross_char2_batch  CSC_REORDER_GEN_EXPAND(csc_extract_col_cross_char2_batch)
void csc_extract_col_cross_char2_batch(_TYPE_I *row_idx, _TYPE_I *col_ptr, __attribute__((unused)) _TYPE_V *val, int m, int n, int nnz, int window_width, int batch, 
									   int *num_windows_out, _TYPE_I **cc_r_out, _TYPE_I **cc_c_out, unsigned char **cc_v_out);

#undef  csc_reorder_matrix_by_col
#define csc_reorder_matrix_by_col  CSC_REORDER_GEN_EXPAND(csc_reorder_matrix_by_col)
void csc_reorder_matrix_by_col(__attribute__((unused)) int m, int n, __attribute__((unused)) int nnz, _TYPE_I * row_idx, _TYPE_I * col_ptr, _TYPE_V * val, 
							   int * membership, _TYPE_I * row_idx_reorder, _TYPE_I * col_ptr_reorder, _TYPE_V * val_reorder, _TYPE_I * original_col_positions);

#undef  csc_reorder_matrix_by_col_batch
#define csc_reorder_matrix_by_col_batch  CSC_REORDER_GEN_EXPAND(csc_reorder_matrix_by_col_batch)
void csc_reorder_matrix_by_col_batch(__attribute__((unused)) int m, int n, __attribute__((unused)) int nnz, int batch, _TYPE_I * row_idx, _TYPE_I * col_ptr, _TYPE_V * val, 
									 int * membership, _TYPE_I * row_idx_reorder, _TYPE_I * col_ptr_reorder, _TYPE_V * val_reorder, _TYPE_I * original_col_positions);

#undef  csc_kmeans_reorder_col
#define csc_kmeans_reorder_col  CSC_REORDER_GEN_EXPAND(csc_kmeans_reorder_col)
void csc_kmeans_reorder_col(_TYPE_I * row_idx, _TYPE_I * col_ptr, _TYPE_V * val, 
							_TYPE_I * row_idx_reorder_c, _TYPE_I * col_ptr_reorder_c, _TYPE_V * val_reorder_c, _TYPE_I ** original_col_positions,
							int m, int n, int nnz, 
							int numClusters, float threshold, int loop_threshold, int num_windows, _TYPE_I * cc_r, _TYPE_I * cc_c, float * cc_v);

#undef  csc_kmeans_reorder_col_batch
#define csc_kmeans_reorder_col_batch  CSC_REORDER_GEN_EXPAND(csc_kmeans_reorder_col_batch)
void csc_kmeans_reorder_col_batch(_TYPE_I * row_idx, _TYPE_I * col_ptr, _TYPE_V * val, 
								  _TYPE_I * row_idx_reorder_c, _TYPE_I * col_ptr_reorder_c, _TYPE_V * val_reorder_c, _TYPE_I ** original_col_positions,
								  int m, int n, int nnz, int batch, 
								  int numClusters, float threshold, int loop_threshold, int num_windows, _TYPE_I * cc_r, _TYPE_I * cc_c, float * cc_v);

#undef  csc_extract_subgroups
#define csc_extract_subgroups  CSC_REORDER_GEN_EXPAND(csc_extract_subgroups)
void csc_extract_subgroups(_TYPE_I * row_idx, _TYPE_I * col_ptr, _TYPE_V * val, int m, int n, int nnz, float threshold);

#undef  csc_kmeans_char_reorder_col
#define csc_kmeans_char_reorder_col  CSC_REORDER_GEN_EXPAND(csc_kmeans_char_reorder_col)
void csc_kmeans_char_reorder_col(_TYPE_I * row_idx, _TYPE_I * col_ptr, _TYPE_V * val, 
								 _TYPE_I * row_idx_reorder_c, _TYPE_I * col_ptr_reorder_c, _TYPE_V * val_reorder_c, _TYPE_I ** original_col_positions,
								 int m, int n, int nnz, 
								 int numClusters, float threshold, int loop_threshold, int num_windows, _TYPE_I * cc_r, _TYPE_I * cc_c, unsigned char * cc_v);

#undef  csc_kmeans_char_reorder_col_batch
#define csc_kmeans_char_reorder_col_batch  CSC_REORDER_GEN_EXPAND(csc_kmeans_char_reorder_col_batch)
void csc_kmeans_char_reorder_col_batch(_TYPE_I * row_idx, _TYPE_I * col_ptr, _TYPE_V * val, 
									   _TYPE_I * row_idx_reorder_c, _TYPE_I * col_ptr_reorder_c, _TYPE_V * val_reorder_c, _TYPE_I ** original_col_positions,
									   int m, int n, int nnz, int batch, 
									   int numClusters, float threshold, int loop_threshold, int num_windows, _TYPE_I * cc_r, _TYPE_I * cc_c, unsigned char * cc_v);

#undef  csc_split_matrix_batch_n
#define csc_split_matrix_batch_n  CSC_REORDER_GEN_EXPAND(csc_split_matrix_batch_n)
void csc_split_matrix_batch_n(_TYPE_I * row_idx, _TYPE_I * col_ptr, _TYPE_V * val, 
	_TYPE_I *** row_idx_split_n_out, _TYPE_I *** col_ptr_split_n_out, _TYPE_V *** val_split_n_out, 
	_TYPE_I ** nnz_part_out, _TYPE_I ** n_part_out, 
	char *file_in,
	int m, int n, int nnz, int batch_n);