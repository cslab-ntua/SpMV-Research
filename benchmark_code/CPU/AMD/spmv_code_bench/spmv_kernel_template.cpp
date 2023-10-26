#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

#include "macros/cpp_defines.h"

#include "spmv_bench_common.h"
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

	New_Array(INT_T * ia, INT_T * ja, ValueType * a, long m, long n, long nnz) : Matrix_Format(m, n, nnz), ia(ia), ja(ja), a(a)
	{

	}

	~New_Array()
	{

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
csr_to_format(INT_T * row_ptr, INT_T * col_ind, ValueType * values, long m, long n, long nnz)
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
New_Array::statistics_print_data(char * buf, long buf_n)
{
}

