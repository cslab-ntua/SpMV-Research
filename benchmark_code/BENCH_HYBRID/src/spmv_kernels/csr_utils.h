#ifndef CSR_UTILS_H
#define CSR_UTILS_H

#include <stdlib.h>
#include "spmv_kernel.h"

// Shared helper: extract a compact CSR sub-matrix from a row_map slice.
// Reads rows row_map[start .. start+count-1] from the full CSR and builds
// a self-contained CSR fragment.  Caller must free the three output arrays.
static inline void extract_csr_fragment(
    INT_T * row_ptr, INT_T * col_ind, ValueTypeReference * values,
    INT_T * row_map, long start, long count, long total_nnz,
    INT_T ** out_row_ptr, INT_T ** out_col_ind, ValueTypeReference ** out_values)
{
	INT_T * r_p = (INT_T *) malloc((count + 1) * sizeof(INT_T));
	INT_T * c_i = (INT_T *) malloc(total_nnz * sizeof(INT_T));
	ValueTypeReference * vals = (ValueTypeReference *) malloc(total_nnz * sizeof(ValueTypeReference));

	r_p[0] = 0;
	long curr_nnz = 0;
	for (long i = 0; i < count; i++) {
		long row = row_map[start + i];
		long row_nnz = row_ptr[row+1] - row_ptr[row];
		for (long j = 0; j < row_nnz; j++) {
			c_i[curr_nnz + j] = col_ind[row_ptr[row] + j];
			vals[curr_nnz + j] = values[row_ptr[row] + j];
		}
		curr_nnz += row_nnz;
		r_p[i+1] = curr_nnz;
	}

	*out_row_ptr = r_p;
	*out_col_ind = c_i;
	*out_values  = vals;
}

#endif
