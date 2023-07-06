#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

#include "aoclsparse_mat_structures.h"
#include "aoclsparse_descr.h"
#include "aoclsparse.h"

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


struct CSRArrays : Matrix_Format
{
	ValueType * a;   // the values (of size NNZ)
	INT_T * ia;      // the usual rowptr (of size m+1)
	INT_T * ja;      // the colidx of each NNZ (of size nnz)
	aoclsparse_matrix A;
	aoclsparse_mat_descr descr; // aoclsparse_matrix_type_general
	aoclsparse_operation trans; // aoclsparse_operation_none

	CSRArrays(long m, long n, long nnz) : Matrix_Format(m, n, nnz)
	{
		a = NULL;
		ia = NULL;
		ja= NULL;
		aoclsparse_create_mat_descr(&descr);
	}

	~CSRArrays()
	{
		free(a);
		free(ia);
		free(ja);
		aoclsparse_destroy_mat_descr(descr);
	}

	void spmv(ValueType * x, ValueType * y);
	void statistics_start();
	int statistics_print(__attribute__((unused)) char * buf, __attribute__((unused)) long buf_n);
};


void compute_sparse_mv(CSRArrays * csr, ValueType * x , ValueType * y);


void
CSRArrays::spmv(ValueType * x, ValueType * y)
{
	compute_sparse_mv(this, x, y);
}


struct Matrix_Format *
csr_to_format(INT_T * row_ptr, INT_T * col_ind, ValueType * values, long m, long n, long nnz)
{
	struct CSRArrays * csr = new CSRArrays(m, n, nnz);
	double time, time_balance;

	aoclsparse_index_base base = aoclsparse_index_base_zero;

	csr->format_name = (char *) "AOCL_OPTMV";
	csr->ia = row_ptr;
	csr->ja = col_ind;
	csr->a = values;

	time = time_it(1,
		// The input arrays provided are left unchanged except for the call to mkl_sparse_order, which performs ordering of column indexes of the matrix.
		// To avoid any changes to the input data, use mkl_sparse_copy.
		#if DOUBLE == 0
			aoclsparse_create_scsr(csr->A, base, m, n, nnz, row_ptr, col_ind, values);
		#elif DOUBLE == 1
			aoclsparse_create_dcsr(csr->A, base, m, n, nnz, row_ptr, col_ind, values);
		#endif

		// mkl_sparse_order(csr->A);  // Sort the columns.
	);
	printf("aocl aoclsparse_create_csr time = %g\n", time);

	aoclsparse_set_mat_index_base(csr->descr, base);	
	csr->trans = aoclsparse_operation_none;

	const INT_T expected_calls = 128;

	time_balance = time_it(1,
		//to identify hint id(which routine is to be executed, destroyed later)
    		// aoclsparse_set_mv_hint(csr->A, csr->trans, csr->descr, 0);
    		aoclsparse_set_mv_hint(csr->A, csr->trans, csr->descr, expected_calls);
    		aoclsparse_optimize(csr->A);
	);
	printf("aocl optimize time = %g\n", time_balance);

	#if 0
		for (long i=0;i<m;i++)
		{
			for (long j=row_ptr[i];j<row_ptr[i+1];j++)
			{
				if (j < row_ptr[i+1]-1 && col_ind[j] >= col_ind[j+1])
					error("%ld: unsorted columns: %d >= %d", i, col_ind[j], col_ind[j+1]);
			}
		}
	#endif

	// _Pragma("omp parallel")
	// {
	// 	printf("max_threads=%d , num_threads=%d , tnum=%d\n", omp_get_max_threads(), omp_get_num_threads(), omp_get_thread_num());
	// }
	// printf("out\n");

	// for (long i=0;i<m;i++)
	// {
	// 	long row_curr = row_ptr[i];
	// 	long row_curr2 = csr->A->csr_mat->csr_row_ptr[i];
	// 	printf("row_curr = %ld vs row_curr2 = %ld\n", row_curr, row_curr2);

	// 	for (long j=row_ptr[i];j<row_ptr[i+1];j++)
	// 	{
	// 		if (j < row_ptr[i+1]-1 && col_ind[j] >= col_ind[j+1])
	// 			error("%ld: unsorted columns: %d >= %d", i, col_ind[j], col_ind[j+1]);
	// 	}
	// }

	csr->mem_footprint = nnz * (sizeof(ValueType) + sizeof(INT_T)) + (m+1) * sizeof(INT_T);
	return csr;
}


void
compute_sparse_mv(CSRArrays * csr, ValueType * x , ValueType * y)
{
	ValueType alpha = 1.0;
	ValueType beta = 0;

	#if DOUBLE == 0
		aoclsparse_smv(csr->trans, &alpha, csr->A, csr->descr, x, &beta, y);
        #elif DOUBLE == 1
		aoclsparse_dmv(csr->trans, &alpha, csr->A, csr->descr, x, &beta, y);
        #endif
}


//==========================================================================================================================================
//= Print Statistics
//==========================================================================================================================================


void
CSRArrays::statistics_start()
{
}


int
CSRArrays::statistics_print(__attribute__((unused)) char * buf, __attribute__((unused)) long buf_n)
{
	return 0;
}

