#if !defined(CSR_GEN_TYPE_1)
	#error "CSR_GEN_TYPE_1 not defined: value type"
#elif !defined(CSR_GEN_TYPE_2)
	#error "CSR_GEN_TYPE_2 not defined: index type"
#elif !defined(CSR_GEN_SUFFIX)
	#error "CSR_GEN_SUFFIX not defined"
#elif !defined(CSR_GEN_FUNCTION_ATTRIBUTES)
	#define CSR_GEN_FUNCTION_ATTRIBUTES
#endif

#include "macros/cpp_defines.h"
#include "macros/macrolib.h"


#define CSR_GEN_EXPAND(name)  CONCAT(name, CSR_GEN_SUFFIX)
#define CSR_GEN_EXPAND_TYPE(name)  CONCAT(CSR_GEN_, CSR_GEN_EXPAND(name))

#undef  _TYPE_V
#define _TYPE_V  CSR_GEN_EXPAND_TYPE(_TYPE_V)
typedef CSR_GEN_TYPE_1  _TYPE_V;

#undef  _TYPE_I
#define _TYPE_I  CSR_GEN_EXPAND_TYPE(_TYPE_I)
typedef CSR_GEN_TYPE_2  _TYPE_I;


//==========================================================================================================================================
//------------------------------------------------------------------------------------------------------------------------------------------
//-                                                              Functions                                                                 -
//------------------------------------------------------------------------------------------------------------------------------------------
//==========================================================================================================================================


#undef  csr_sort_columns
#define csr_sort_columns  CSR_GEN_EXPAND(csr_sort_columns)
CSR_GEN_FUNCTION_ATTRIBUTES
void csr_sort_columns(_TYPE_I * row_ptr, _TYPE_I * col_idx, _TYPE_V * values, long m, long n, long nnz);

#undef  coo_to_csr
#define coo_to_csr  CSR_GEN_EXPAND(coo_to_csr)
CSR_GEN_FUNCTION_ATTRIBUTES
void coo_to_csr(_TYPE_I * R, _TYPE_I * C, _TYPE_V * V, long m, long n, long nnz, _TYPE_I * row_ptr, _TYPE_I * col_idx, _TYPE_V * values, const int sort_columns, const int transpose);

#undef  csr_transpose
#define csr_transpose  CSR_GEN_EXPAND(csr_transpose)
CSR_GEN_FUNCTION_ATTRIBUTES
void csr_transpose(_TYPE_I * row_ptr, _TYPE_I * col_idx, _TYPE_V * values, long m, long n, long nnz, _TYPE_I ** row_ptr_ret, _TYPE_I ** col_idx_ret, _TYPE_V ** values_ret, const int sort_columns);

#undef  csr_split_to_lower_and_strictly_upper_triangular
#define csr_split_to_lower_and_strictly_upper_triangular  CSR_GEN_EXPAND(csr_split_to_lower_and_strictly_upper_triangular)
CSR_GEN_FUNCTION_ATTRIBUTES
void csr_split_to_lower_and_strictly_upper_triangular(_TYPE_I * row_ptr, _TYPE_I * col_idx, _TYPE_V * values, long m, long n, long nnz,
		_TYPE_I ** row_ptr_upper_ret, _TYPE_I ** col_idx_upper_ret, _TYPE_V ** values_upper_ret, long * nnz_upper_out,
		_TYPE_I ** row_ptr_lower_ret, _TYPE_I ** col_idx_lower_ret, _TYPE_V ** values_lower_ret, long * nnz_lower_out,
		const int sort_columns, const int transpose_upper, const int transpose_lower);


//==========================================================================================================================================
//= Symmetric Matrices
//==========================================================================================================================================


// Expand a symmetric matrix that has only half the nnz stored.
#undef  csr_expand_symmetric
#define csr_expand_symmetric  CSR_GEN_EXPAND(csr_expand_symmetric)
CSR_GEN_FUNCTION_ATTRIBUTES
void csr_expand_symmetric(_TYPE_I * row_ptr, _TYPE_I * col_idx, _TYPE_V * values, long m, long n, long nnz, _TYPE_I ** row_ptr_ret, _TYPE_I ** col_idx_ret, _TYPE_V ** values_ret, long * nnz_out, long * nnz_diag_out, const int sort_columns);

// Returns a matrix that includes all missing symmetric nnz of the input matrix.
#undef  csr_symmetrize
#define csr_symmetrize  CSR_GEN_EXPAND(csr_symmetrize)
CSR_GEN_FUNCTION_ATTRIBUTES
void csr_symmetrize(_TYPE_I * row_ptr, _TYPE_I * col_idx, _TYPE_V * values, long m, long n, long nnz, _TYPE_I ** row_ptr_ret, _TYPE_I ** col_idx_ret, _TYPE_V ** values_ret, long * nnz_out, long * nnz_diag_out, const int sort_columns);

#undef  csr_drop_upper
#define csr_drop_upper  CSR_GEN_EXPAND(csr_drop_upper)
CSR_GEN_FUNCTION_ATTRIBUTES
void csr_drop_upper(_TYPE_I * row_ptr, _TYPE_I * col_idx, _TYPE_V * values, long m, long n, long nnz, _TYPE_I ** row_ptr_ret, _TYPE_I ** col_idx_ret, _TYPE_V ** values_ret, long * nnz_out, long * nnz_diag_out, const int sort_columns);


//==========================================================================================================================================
//= IO
//==========================================================================================================================================


#undef  csr_save_to_mtx
#define csr_save_to_mtx  CSR_GEN_EXPAND(csr_save_to_mtx)
CSR_GEN_FUNCTION_ATTRIBUTES
void csr_save_to_mtx(_TYPE_I * row_ptr, _TYPE_I * col_idx, _TYPE_V * val, long m, long n, const char* filename);


