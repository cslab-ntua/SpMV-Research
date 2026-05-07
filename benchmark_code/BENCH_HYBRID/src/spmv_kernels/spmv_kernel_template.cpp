#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

#include "macros/cpp_defines.h"

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


struct New_Array : Matrix_Format
{
	INT_T * row_ptr;      // the usual rowptr (of size m+1)
	INT_T * col_ind;      // the colidx of each NNZ (of size nnz)
	ValueType * values;   // the values (of size NNZ)

	New_Array(INT_T * row_ptr_in, INT_T * col_ind_in, ValueTypeReference * values_in, long m, long n, long nnz) : Matrix_Format(m, n, nnz)
	{
		row_ptr = (typeof(row_ptr)) aligned_alloc(64, (m+1) * sizeof(*row_ptr));
		col_ind = (typeof(col_ind)) aligned_alloc(64, nnz * sizeof(*col_ind));
		values = (typeof(values)) aligned_alloc(64, nnz * sizeof(*values));
		#pragma omp parallel for
		for (long i=0;i<m+1;i++)
			row_ptr[i] = row_ptr_in[i];
		#pragma omp parallel for
		for(long i=0;i<nnz;i++)
		{
			values[i]=values_in[i];
			col_ind[i]=col_ind_in[i];
		}


	}

	~New_Array()
	{
		free(row_ptr);
		free(col_ind);
		free(values);
	}

	void spmv(ValueType * x, ValueType * y);
	void statistics_start();
	int statistics_print_data(char * buf, long buf_n);
};


void
New_Array::spmv(ValueType * x, ValueType * y)
{
	
}


struct Matrix_Format *
csr_to_format(INT_T * row_ptr, INT_T * col_ind, ValueTypeReference * values, long m, long n, long nnz, long symmetric, long symmetry_expanded)
{

}


//==========================================================================================================================================
//= Print Statistics
//==========================================================================================================================================


void
New_Array::statistics_start()
{
}


int
statistics_print_labels(__attribute__((unused)) char * buf, __attribute__((unused)) long buf_n)
{
	return 0;
}


int
New_Array::statistics_print_data(__attribute__((unused)) char * buf, __attribute__((unused)) long buf_n)
{
	return 0;
}

