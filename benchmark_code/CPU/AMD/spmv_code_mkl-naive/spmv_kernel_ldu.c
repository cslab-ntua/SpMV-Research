#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

#include "macros/cpp_defines.h"

#include "common.h"
#include "spmv_kernel.h"

#ifdef __cplusplus
extern "C"{
#endif
	#include "macros/macrolib.h"
	#include "time_it.h"
	#include "parallel_util.h"
#ifdef __cplusplus
}
#endif


// Only for structurally symmetric matrices.
// Symmetrical elements will be in the same position in the 'upper' and 'lower' arrays.
// We also assume value symmetry for convenience in the conversion.
struct LDUArrays : Matrix_Format
{
	INT_T upper_n;
	ValueType * diag;
	ValueType * upper;       // upper diagonal in COO format
	ValueType * lower;       // lower diagonal in COO format
	INT_T * row_idx;       // row indexes for upper (column for the lower)
	INT_T * col_idx;       // column indexes for upper (row for the lower)

	LDUArrays(long m, long n, long nnz) : Matrix_Format(m, n, nnz)
	{
		diag = NULL;
		upper = NULL;
		row_idx = NULL;
		col_idx = NULL;
	}

	~LDUArrays(){
		free(diag);
		free(upper);
		free(lower);
		free(row_idx);
		free(col_idx);
	}

	void spmv(ValueType * x, ValueType * y);
};


void compute_ldu(LDUArrays * ldu, ValueType * x , ValueType * y);


void
LDUArrays::spmv(ValueType * x, ValueType * y)
{
	compute_ldu(this, x, y);
}


struct Matrix_Format *
csr_to_format(INT_T * row_ptr, INT_T * col_ind, ValueType * values, long m, long n, long nnz)
{
	LDUArrays * ldu = new LDUArrays(m, n, nnz);
	long num_threads = omp_get_max_threads();
	long t_upper_n[num_threads];
	ldu->format_name = (char *) "LDU";
	ldu->mem_footprint = nnz * sizeof(ValueType) + ldu->upper_n * 2 * sizeof(INT_T);
	ldu->upper_n = 0;
	ldu->diag = (typeof(ldu->diag)) malloc(ldu->m * sizeof(*ldu->diag));
	#pragma omp parallel
	{
		long tnum = omp_get_thread_num();
		long upper_base = 0;
		long i, j, i_s, i_e;
		loop_partitioner_balance_iterations(num_threads, tnum, 0, ldu->m, &i_s, &i_e);
		for (i=i_s;i<i_e;i++)
			for (j=row_ptr[i];j<row_ptr[i+1];j++)
				if (i < col_ind[j])
					upper_base++;
		t_upper_n[tnum] = upper_base;
		#pragma omp barrier
		#pragma omp single
		{
			long tmp = 0;
			upper_base = 0;
			for (i=0;i<num_threads;i++)
			{
				tmp = t_upper_n[i];
				t_upper_n[i] = upper_base;
				upper_base += tmp;
			}
			ldu->upper_n = upper_base;
			ldu->nnz = ldu->m + 2 * upper_base;
			ldu->upper = (typeof(ldu->upper)) malloc(ldu->upper_n * sizeof(*ldu->upper));
			ldu->lower = (typeof(ldu->lower)) malloc(ldu->upper_n * sizeof(*ldu->lower));
			ldu->row_idx = (typeof(ldu->row_idx)) malloc(ldu->upper_n * sizeof(*ldu->row_idx));
			ldu->col_idx = (typeof(ldu->col_idx)) malloc(ldu->upper_n * sizeof(*ldu->col_idx));
		}
		#pragma omp barrier
		upper_base = t_upper_n[tnum];
		for (i=i_s;i<i_e;i++)
			for (j=row_ptr[i];j<row_ptr[i+1];j++)
			{
				if (i == col_ind[j])
					ldu->diag[i] = values[j];
				else if (i < col_ind[j])
				{
					ldu->upper[upper_base] = values[j];
					ldu->lower[upper_base] = values[j];           // Symmetrical elements will be in the same position in the 'upper' and 'lower' arrays.
					ldu->row_idx[upper_base] = i;
					ldu->col_idx[upper_base] = col_ind[j];
					upper_base++;
				}
			}
	}
	return ldu;
}


void compute_ldu(LDUArrays * ldu, ValueType * x , ValueType * y)
{
	long i;
	long row, col;
	for (i=0;i<ldu->m;i++)
		y[i] = ldu->diag[i] * x[i];
	for (i=0;i<ldu->upper_n;i++)
	{
		row = ldu->row_idx[i];
		col = ldu->col_idx[i];
		y[row] += ldu->upper[i] * x[col];
		y[col] += ldu->lower[i] * x[row];
	}
}

