#if !defined(CSC_GEN_TYPE_1)
	#error "CSC_GEN_TYPE_1 not defined: value type"
#elif !defined(CSC_GEN_TYPE_2)
	#error "CSC_GEN_TYPE_2 not defined: index type"
#elif !defined(CSC_GEN_SUFFIX)
	#error "CSC_GEN_SUFFIX not defined"
#endif

#include "macros/cpp_defines.h"
#include "macros/macrolib.h"


#ifndef CSC_GEN_H
#define CSC_GEN_H
#endif /* CSC_GEN_H */


#define CSC_GEN_EXPAND(name)  CONCAT(name, CSC_GEN_SUFFIX)
#define CSC_GEN_EXPAND_TYPE(name)  CONCAT(CSC_GEN_, CSC_GEN_EXPAND(name))

#undef  _TYPE_V
#define _TYPE_V  CSC_GEN_EXPAND_TYPE(_TYPE_V)
typedef CSC_GEN_TYPE_1  _TYPE_V;

#undef  _TYPE_I
#define _TYPE_I  CSC_GEN_EXPAND_TYPE(_TYPE_I)
typedef CSC_GEN_TYPE_2  _TYPE_I;


//==========================================================================================================================================
//------------------------------------------------------------------------------------------------------------------------------------------
//-                                                              Functions                                                                 -
//------------------------------------------------------------------------------------------------------------------------------------------
//==========================================================================================================================================


#undef  csc_sort_rows
#define csc_sort_rows  CSC_GEN_EXPAND(csc_sort_rows)
void csc_sort_rows(_TYPE_I * row_idx, _TYPE_I * col_ptr, _TYPE_V * values, long m, long n, long nnz);


#undef  coo_to_csc
#define coo_to_csc  CSC_GEN_EXPAND(coo_to_csc)
void coo_to_csc(_TYPE_I * R, _TYPE_I * C, _TYPE_V * V, long m, long n, long nnz, _TYPE_I * row_idx, _TYPE_I * col_ptr, _TYPE_V * values, int sort_rows);

