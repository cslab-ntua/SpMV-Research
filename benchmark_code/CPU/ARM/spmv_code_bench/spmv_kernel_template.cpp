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


// extern INT_T * thread_i_s;
// extern INT_T * thread_i_e;

// extern INT_T * thread_j_s;
// extern INT_T * thread_j_e;

// extern ValueType * thread_v_s;
// extern ValueType * thread_v_e;

extern int prefetch_distance;


struct New_Array : Matrix_Format
{

	New_Array(INT_T * ia, INT_T * ja, ValueType * a, long m, long n, long nnz) : Matrix_Format(m, n, nnz), ia(ia), ja(ja), a(a)
	{

	}

	~New_Array()
	{

	}

	void spmv(ValueType * x, ValueType * y);
};

void
New_Array::spmv(ValueType * x, ValueType * y)
{
	
}


struct Matrix_Format *
csr_to_format(INT_T * row_ptr, INT_T * col_ind, ValueType * values, long m, long n, long nnz)
{

}
