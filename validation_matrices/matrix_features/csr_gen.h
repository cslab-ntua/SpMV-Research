#if !defined(CSR_GEN_TYPE_1)
	#error "CSR_GEN_TYPE_1 not defined: value type"
#elif !defined(CSR_GEN_TYPE_2)
	#error "CSR_GEN_TYPE_2 not defined: index type"
#elif !defined(CSR_GEN_SUFFIX)
	#error "CSR_GEN_SUFFIX not defined"
#endif

#include <stdlib.h>
#include <stdio.h>
#include <omp.h>
#include <math.h>

#include "parallel_util.h"
#include "genlib.h"
#include "matrix_metrics.h"
#include "plot/plot.h"


#ifndef CSR_GEN_H
#define CSR_GEN_H
#endif /* CSR_GEN_H */


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//------------------------------------------------------------------------------------------------------------------------------------------
//-                                                              Templates                                                                 -
//------------------------------------------------------------------------------------------------------------------------------------------
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


#define CSR_GEN_EXPAND(name)  CONCAT(name, CSR_GEN_SUFFIX)

#undef  _TYPE_V
#define _TYPE_V  CSR_GEN_EXPAND(_TYPE_V)
typedef CSR_GEN_TYPE_1  _TYPE_V;

#undef  _TYPE_I
#define _TYPE_I  CSR_GEN_EXPAND(_TYPE_I)
typedef CSR_GEN_TYPE_2  _TYPE_I;


/*
 * Matrix in CSR format.
 */
#undef  CSR
#define CSR  CSR_GEN_EXPAND(CSR)
struct CSR {
	long m;                   // number of rows
	long n;	                  // number of columns
	long nnz;                 // number of non-zeros

	_TYPE_I * R_offsets;      // row offsets
	_TYPE_I * C;              // column indexes

	_TYPE_V * V;              // values
};


#undef  csr_new
#define csr_new  CSR_GEN_EXPAND(csr_new)
static inline
struct CSR *
csr_new(long m, long n, long nnz, int no_values)
{
	struct CSR * csr;
	csr = (typeof(csr)) malloc(sizeof(*csr));
	csr->m = m;
	csr->n = n;
	csr->nnz = nnz;
	csr->R_offsets = (typeof(csr->R_offsets)) malloc((m+1) * sizeof(*csr->R_offsets));
	csr->C = (typeof(csr->C)) malloc(nnz * sizeof(*csr->C));
	csr->V = (!no_values) ? (typeof(csr->V)) malloc(nnz * sizeof(*csr->V)) : NULL;
	return csr;
}


#undef  coo_sort
#define coo_sort  CSR_GEN_EXPAND(coo_sort)
void coo_sort(_TYPE_I * R, _TYPE_I * C, long m, long n, long nnz,
		long * m_out, long * n_out,
		_TYPE_I ** permutation_rows_out, _TYPE_I ** permutation_cols_out,
		_TYPE_I ** reverse_permutation_rows_out, _TYPE_I ** reverse_permutation_cols_out);

#undef  coo_break_matrix
#define coo_break_matrix  CSR_GEN_EXPAND(coo_break_matrix)
long coo_break_matrix(_TYPE_I * R, _TYPE_I * C, long m, long n, long nnz);

#undef  coo_to_csr
#define coo_to_csr  CSR_GEN_EXPAND(coo_to_csr)
void coo_to_csr(_TYPE_I * R, _TYPE_I * C, _TYPE_V * V, long m, long n, long nnz, _TYPE_I * row_ptr, _TYPE_I * col_idx, _TYPE_V * val, int sort_columns);

#undef  coo_to_csr_fully_sorted
#define coo_to_csr_fully_sorted  CSR_GEN_EXPAND(coo_to_csr_fully_sorted)
void coo_to_csr_fully_sorted(_TYPE_I * R, _TYPE_I * C, _TYPE_V * V, long m, long n, long nnz, _TYPE_I * row_ptr, _TYPE_I * col_idx, _TYPE_V * val);

#undef  csr_matrix_features
#define csr_matrix_features  CSR_GEN_EXPAND(csr_matrix_features)
void csr_matrix_features(char * title_base, _TYPE_I * R_offsets, _TYPE_I * C, long m, long n, long nnz);

#undef  coo_matrix_features
#define coo_matrix_features  CSR_GEN_EXPAND(coo_matrix_features)
void coo_matrix_features(char * title_base, _TYPE_I * R, _TYPE_I * C, _TYPE_V * V, long m, long n, long nnz);

#undef  csr_plot
#define csr_plot  CSR_GEN_EXPAND(csr_plot)
void csr_plot(struct CSR * csr, char * file_out);

