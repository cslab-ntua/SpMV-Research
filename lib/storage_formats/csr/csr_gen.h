#if !defined(CSR_GEN_TYPE_1)
	#error "CSR_GEN_TYPE_1 not defined: value type"
#elif !defined(CSR_GEN_TYPE_2)
	#error "CSR_GEN_TYPE_2 not defined: index type"
#elif !defined(CSR_GEN_SUFFIX)
	#error "CSR_GEN_SUFFIX not defined"
#endif

#include "macros/cpp_defines.h"
#include "macros/macrolib.h"


#ifndef CSR_GEN_H
#define CSR_GEN_H
#endif /* CSR_GEN_H */


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
void csr_sort_columns(_TYPE_I * row_ptr, _TYPE_I * col_idx, _TYPE_V * values, long m, long n, long nnz);


#undef  coo_to_csr
#define coo_to_csr  CSR_GEN_EXPAND(coo_to_csr)
void coo_to_csr(_TYPE_I * R, _TYPE_I * C, _TYPE_V * V, long m, long n, long nnz, _TYPE_I * row_ptr, _TYPE_I * col_idx, _TYPE_V * values, int sort_columns, int transpose);


//==========================================================================================================================================
//= Save to file
//==========================================================================================================================================


#undef  csr_save_to_mtx
#define csr_save_to_mtx  CSR_GEN_EXPAND(csr_save_to_mtx)
void csr_save_to_mtx(_TYPE_I * row_ptr, _TYPE_I * col_idx, _TYPE_V * val, int num_rows, int num_cols, const char* filename);

