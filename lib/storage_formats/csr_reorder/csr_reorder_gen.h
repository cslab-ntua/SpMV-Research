#if !defined(CSR_REORDER_GEN_TYPE_1)
	#error "CSR_REORDER_GEN_TYPE_1 not defined: value type"
#elif !defined(CSR_REORDER_GEN_TYPE_2)
	#error "CSR_REORDER_GEN_TYPE_2 not defined: index type"
#elif !defined(CSR_REORDER_GEN_SUFFIX)
	#error "CSR_REORDER_GEN_SUFFIX not defined"
#endif

#include "macros/cpp_defines.h"
#include "macros/macrolib.h"


#define CSR_REORDER_GEN_EXPAND(name)  CONCAT(name, CSR_REORDER_GEN_SUFFIX)

#undef  _TYPE_V
#define _TYPE_V  CSR_REORDER_GEN_EXPAND(_TYPE_V)
typedef CSR_REORDER_GEN_TYPE_1  _TYPE_V;

#undef  _TYPE_I
#define _TYPE_I  CSR_REORDER_GEN_EXPAND(_TYPE_I)
typedef CSR_REORDER_GEN_TYPE_2  _TYPE_I;


//==========================================================================================================================================
//------------------------------------------------------------------------------------------------------------------------------------------
//-                                                              Functions                                                                 -
//------------------------------------------------------------------------------------------------------------------------------------------
//==========================================================================================================================================

//==========================================================================================================================================
//= Matrix transformations
//==========================================================================================================================================

#undef  csr_sort_by_row_size
#define csr_sort_by_row_size  CSR_REORDER_GEN_EXPAND(csr_sort_by_row_size)
void csr_sort_by_row_size(long m, _TYPE_I * row_ptr, _TYPE_I * col_idx, _TYPE_V * values, _TYPE_I * row_ptr_s, _TYPE_I * col_idx_s, _TYPE_V * values_s);

// "distant_mark" is a 0-1 flag matrix, which holds which nonzeros are distant from others, in order to isolate them later
#undef  csr_mark_distant_nonzeros
#define csr_mark_distant_nonzeros  CSR_REORDER_GEN_EXPAND(csr_mark_distant_nonzeros)
long csr_mark_distant_nonzeros(_TYPE_I * row_ptr, _TYPE_I * col_idx, long m, long max_distance, _TYPE_I * distant_mark);

#undef  csr_separate_close_distant
#define csr_separate_close_distant  CSR_REORDER_GEN_EXPAND(csr_separate_close_distant)
void csr_separate_close_distant(_TYPE_I *row_ptr, _TYPE_I *col_idx, _TYPE_V *values, _TYPE_I *distant_mark, long nnz, long m, _TYPE_I *row_ptr_close, _TYPE_I *col_idx_close, _TYPE_V *values_close, _TYPE_I *row_ptr_distant, _TYPE_I *col_idx_distant, _TYPE_V *values_distant);

#undef  csr_separate_low_high
#define csr_separate_low_high  CSR_REORDER_GEN_EXPAND(csr_separate_low_high)
void csr_separate_low_high(_TYPE_I *row_ptr, _TYPE_I *col_idx, _TYPE_V *values, long nnz, long m, _TYPE_I *row_low_out, _TYPE_I **row_ptr_low_out, _TYPE_I **col_idx_low_out, _TYPE_V **values_low_out, _TYPE_I **row_ptr_high_out, _TYPE_I **col_idx_high_out, _TYPE_V **values_high_out);

#undef  csr_shuffle_matrix
#define csr_shuffle_matrix  CSR_REORDER_GEN_EXPAND(csr_shuffle_matrix)
void csr_shuffle_matrix(long m, _TYPE_I *row_ptr, _TYPE_I *col_idx, _TYPE_V *values, _TYPE_I *row_ptr_shuffle, _TYPE_I *col_idx_shuffle, _TYPE_V *values_shuffle);

// #undef  csr_extract_row_cross
// #define csr_extract_row_cross  CSR_REORDER_GEN_EXPAND(csr_extract_row_cross)
// void csr_extract_row_cross(_TYPE_I *row_ptr, _TYPE_I *col_idx, _TYPE_V *val, int m, int n, int nnz, int window_width,
// 		int *num_windows_out, float **row_cross_out, int plot, _TYPE_I **rc_r_out, _TYPE_I **rc_c_out, float **rc_v_out);

#undef  csr_extract_row_cross2
#define csr_extract_row_cross2  CSR_REORDER_GEN_EXPAND(csr_extract_row_cross2)
void csr_extract_row_cross2(_TYPE_I *row_ptr, _TYPE_I *col_idx, _TYPE_V *val, int m, int n, int nnz, int window_width, 
							int *num_windows_out, _TYPE_I **rc_r_out, _TYPE_I **rc_c_out, float **rc_v_out);

#undef  csr_extract_row_cross2_batch
#define csr_extract_row_cross2_batch  CSR_REORDER_GEN_EXPAND(csr_extract_row_cross2_batch)
void csr_extract_row_cross2_batch(_TYPE_I *row_ptr, _TYPE_I *col_idx, _TYPE_V *val, int m, int n, int nnz, int window_width, int batch, 
								  int *num_windows_out, _TYPE_I **rc_r_out, _TYPE_I **rc_c_out, float **rc_v_out);

#undef  csr_extract_row_cross_char
#define csr_extract_row_cross_char  CSR_REORDER_GEN_EXPAND(csr_extract_row_cross_char)
void csr_extract_row_cross_char(_TYPE_I *row_ptr, _TYPE_I *col_idx, _TYPE_V *val, int m, int n, int nnz, int window_width,
		int *num_windows_out, unsigned char **row_cross_out, int plot, _TYPE_I **rc_r_out, _TYPE_I **rc_c_out, float **rc_v_out);

#undef  csr_extract_row_cross_char2
#define csr_extract_row_cross_char2  CSR_REORDER_GEN_EXPAND(csr_extract_row_cross_char2)
void csr_extract_row_cross_char2(_TYPE_I *row_ptr, _TYPE_I *col_idx, _TYPE_V *val, int m, int n, int nnz, int window_width, 
		int *num_windows_out, _TYPE_I **rc_r_out, _TYPE_I **rc_c_out, unsigned char **rc_v_out);

#undef  csr_extract_row_cross_char2_batch
#define csr_extract_row_cross_char2_batch  CSR_REORDER_GEN_EXPAND(csr_extract_row_cross_char2_batch)
void csr_extract_row_cross_char2_batch(_TYPE_I *row_ptr, _TYPE_I *col_idx, _TYPE_V *val, int m, int n, int nnz, int window_width, int batch, 
									   int *num_windows_out, _TYPE_I **rc_r_out, _TYPE_I **rc_c_out, unsigned char **rc_v_out);

#undef  csr_reorder_matrix_by_row
#define csr_reorder_matrix_by_row  CSR_REORDER_GEN_EXPAND(csr_reorder_matrix_by_row)
void csr_reorder_matrix_by_row(int m, int n, int nnz, _TYPE_I * row_ptr, _TYPE_I * col_idx, _TYPE_V * val, 
							   int * membership, _TYPE_I * row_ptr_reorder, _TYPE_I * col_idx_reorder, _TYPE_V * val_reorder, _TYPE_I * original_row_positions);

#undef  csr_reorder_matrix_by_row_batch
#define csr_reorder_matrix_by_row_batch  CSR_REORDER_GEN_EXPAND(csr_reorder_matrix_by_row_batch)
void csr_reorder_matrix_by_row_batch(int m, int n, int nnz, int batch, _TYPE_I * row_ptr, _TYPE_I * col_idx, _TYPE_V * val, 
									 int * membership, _TYPE_I * row_ptr_reorder, _TYPE_I * col_idx_reorder, _TYPE_V * val_reorder, _TYPE_I * original_row_positions);

#undef  csr_kmeans_reorder_row
#define csr_kmeans_reorder_row  CSR_REORDER_GEN_EXPAND(csr_kmeans_reorder_row)
void csr_kmeans_reorder_row(_TYPE_I * row_ptr, _TYPE_I * col_idx, _TYPE_V * val, 
							_TYPE_I * row_ptr_reorder_r, _TYPE_I * col_idx_reorder_r, _TYPE_V * val_reorder_r, _TYPE_I ** original_row_positions,
							int m, int n, int nnz, 
							int numClusters, float threshold, int loop_threshold, int num_windows, int seed, _TYPE_I * rc_r, _TYPE_I * rc_c, float * rc_v);

#undef  csr_kmeans_reorder_by_file
#define csr_kmeans_reorder_by_file  CSR_REORDER_GEN_EXPAND(csr_kmeans_reorder_by_file)
void csr_kmeans_reorder_by_file(char* reorder_file, 
								_TYPE_I * row_ptr, _TYPE_I * col_idx, _TYPE_V * val, 
								_TYPE_I * row_ptr_reorder_r, _TYPE_I * col_idx_reorder_r, _TYPE_V * val_reorder_r, _TYPE_I ** original_row_positions,
								int m, int n, int nnz);

#undef  csr_kmeans_reorder_row_batch
#define csr_kmeans_reorder_row_batch  CSR_REORDER_GEN_EXPAND(csr_kmeans_reorder_row_batch)
void csr_kmeans_reorder_row_batch(_TYPE_I * row_ptr, _TYPE_I * col_idx, _TYPE_V * val, 
								  _TYPE_I * row_ptr_reorder_r, _TYPE_I * col_idx_reorder_r, _TYPE_V * val_reorder_r, _TYPE_I ** original_row_positions, 
								  int m, int n, int nnz, int batch, 
								  int numClusters, float threshold, int loop_threshold, int num_windows, int seed, _TYPE_I * rc_r, _TYPE_I * rc_c, float * rc_v);

#undef  csr_extract_subgroups
#define csr_extract_subgroups  CSR_REORDER_GEN_EXPAND(csr_extract_subgroups)
void csr_extract_subgroups(_TYPE_I * row_ptr, _TYPE_I * col_idx, _TYPE_V * val,
						   int m, int n, int nnz, float threshold);

#undef  csr_kmeans_char_reorder_row
#define csr_kmeans_char_reorder_row  CSR_REORDER_GEN_EXPAND(csr_kmeans_char_reorder_row)
void csr_kmeans_char_reorder_row(_TYPE_I * row_ptr, _TYPE_I * col_idx, _TYPE_V * val, 
								 _TYPE_I * row_ptr_reorder_r, _TYPE_I * col_idx_reorder_r, _TYPE_V * val_reorder_r, _TYPE_I ** original_row_positions, 
								 int m, int n, int nnz, 
								 int numClusters, float threshold, int loop_threshold, int num_windows, int seed, _TYPE_I * rc_r, _TYPE_I * rc_c, unsigned char * rc_v);

#undef  csr_kmeans_char_reorder_row_batch
#define csr_kmeans_char_reorder_row_batch  CSR_REORDER_GEN_EXPAND(csr_kmeans_char_reorder_row_batch)
void csr_kmeans_char_reorder_row_batch(_TYPE_I * row_ptr, _TYPE_I * col_idx, _TYPE_V * val, 
									   _TYPE_I * row_ptr_reorder_r, _TYPE_I * col_idx_reorder_r, _TYPE_V * val_reorder_r, _TYPE_I ** original_row_positions, 
									   int m, int n, int nnz, int batch, 
									   int numClusters, float threshold, int loop_threshold, int num_windows, int seed, _TYPE_I * rc_r, _TYPE_I * rc_c, unsigned char * rc_v);
