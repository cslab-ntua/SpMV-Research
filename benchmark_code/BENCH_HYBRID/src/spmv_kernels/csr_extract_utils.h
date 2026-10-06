#ifndef CSR_EXTRACT_UTILS_H
#define CSR_EXTRACT_UTILS_H

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

// Shared helper: extract a vertical slice (columns) from a full CSR matrix.
// Retains all rows (0 .. m-1) but only includes elements where col_start <= col_ind < col_end.
// Column indices in the returned slice are shifted to be 0-based relative to col_start.
// Caller must free the three output arrays.
static inline void extract_csr_column_slice(
    const INT_T * row_ptr, const INT_T * col_ind, const ValueTypeReference * values,
    long m, long col_start, long col_end,
    INT_T ** out_row_ptr, INT_T ** out_col_ind, ValueTypeReference ** out_values,
    long * out_nnz)
{
    // 1. First pass: count nnz in the requested column range
    long nnz_slice = 0;
    for (long i = 0; i < m; i++) {
        for (long j = row_ptr[i]; j < row_ptr[i+1]; j++) {
            if (col_ind[j] >= col_start && col_ind[j] < col_end) {
                nnz_slice++;
            }
        }
    }

    // 2. Allocate memory
    INT_T * r_p = (INT_T *) malloc((m + 1) * sizeof(INT_T));
    INT_T * c_i = (INT_T *) malloc(nnz_slice * sizeof(INT_T));
    ValueTypeReference * vals = (ValueTypeReference *) malloc(nnz_slice * sizeof(ValueTypeReference));

    // 3. Second pass: copy the elements
    r_p[0] = 0;
    long curr_nnz = 0;
    for (long i = 0; i < m; i++) {
        for (long j = row_ptr[i]; j < row_ptr[i+1]; j++) {
            if (col_ind[j] >= col_start && col_ind[j] < col_end) {
                c_i[curr_nnz] = col_ind[j] - col_start; // Shift to 0-based for this slice
                vals[curr_nnz] = values[j];
                curr_nnz++;
            }
        }
        r_p[i+1] = curr_nnz;
    }

    *out_row_ptr = r_p;
    *out_col_ind = c_i;
    *out_values  = vals;
    *out_nnz     = nnz_slice;
}

#endif
