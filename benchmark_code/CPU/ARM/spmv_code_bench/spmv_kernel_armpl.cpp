#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

#include "armpl.h"

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
	armpl_spmat_t A;

	CSRArrays(long m, long n, long nnz) : Matrix_Format(m, n, nnz)
	{
		a = NULL;
		ia = NULL;
		ja = NULL;
	}

	~CSRArrays(){
		free(a);
		free(ia);
		free(ja);
	}

	void spmv(ValueType * x, ValueType * y);
	void statistics_start();
	int statistics_print_data(__attribute__((unused)) char * buf, __attribute__((unused)) long buf_n);
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
	
	armpl_status_t info;
	int creation_flags = 0;

	csr->format_name = (char *) "ARMPL";
	csr->ia = row_ptr;
	csr->ja = col_ind;
	csr->a = values;
	time = time_it(1,
		// The input arrays provided are left unchanged except for the call to mkl_sparse_order, which performs ordering of column indexes of the matrix.
		#if DOUBLE == 0
			info = armpl_spmat_create_csr_s(&csr->A, m, n, row_ptr, col_ind, values, creation_flags);
			if (info!=ARMPL_STATUS_SUCCESS) printf("ERROR: armpl_spmat_create_csr_s returned %d\n", info);
		#elif DOUBLE == 1
			info = armpl_spmat_create_csr_d(&csr->A, m, n, row_ptr, col_ind, values, creation_flags);
			if (info!=ARMPL_STATUS_SUCCESS) printf("ERROR: armpl_spmat_create_csr_d returned %d\n", info);
		#endif
	);
	printf("armpl armpl_spmat_create_csr_d = %g\n", time);

	time_balance = time_it(1,
		/* Supply any pertinent information that is known about the matrix */
		info = armpl_spmat_hint(csr->A, ARMPL_SPARSE_HINT_STRUCTURE, ARMPL_SPARSE_STRUCTURE_UNSTRUCTURED);
		if (info!=ARMPL_STATUS_SUCCESS) printf("ERROR: armpl_spmat_hint returned %d\n", info);

		/* Supply any hints that are about the SpMV calculations to be performed */
		info = armpl_spmat_hint(csr->A, ARMPL_SPARSE_HINT_SPMV_OPERATION, ARMPL_SPARSE_OPERATION_NOTRANS);
		if (info!=ARMPL_STATUS_SUCCESS) printf("ERROR: armpl_spmat_hint returned %d\n", info);

		info = armpl_spmat_hint(csr->A, ARMPL_SPARSE_HINT_SPMV_INVOCATIONS, ARMPL_SPARSE_INVOCATIONS_MANY);
		if (info!=ARMPL_STATUS_SUCCESS) printf("ERROR: armpl_spmat_hint returned %d\n", info);

		/* Call an optimization process that will learn from the hints you have previously supplied */
		info = armpl_spmv_optimize(csr->A);
		if (info!=ARMPL_STATUS_SUCCESS) printf("ERROR: armpl_spmv_optimize returned %d\n", info);
	);
	printf("armpl optimize time = %g\n", time_balance);

	#if 0
		for (i=0;i<csr_m;i++)
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
		// printf("max_threads=%d , num_threads=%d , tnum=%d\n", omp_get_max_threads(), omp_get_num_threads(), omp_get_thread_num());
	// }
	// printf("out\n");

	csr->mem_footprint = nnz * (sizeof(ValueType) + sizeof(INT_T)) + (m+1) * sizeof(INT_T);
	return csr;
}


void compute_sparse_mv(CSRArrays * csr, ValueType * x , ValueType * y)
{
	armpl_status_t info;
        #if DOUBLE == 0
		info = armpl_spmv_exec_s(ARMPL_SPARSE_OPERATION_NOTRANS, 1.0f, csr->A, x, 0.0f, y);
		if (info!=ARMPL_STATUS_SUCCESS) printf("ERROR: armpl_spmv_exec_s warm_up returned %d\n", info);
        #elif DOUBLE == 1
		info = armpl_spmv_exec_d(ARMPL_SPARSE_OPERATION_NOTRANS, 1.0f, csr->A, x, 0.0f, y);
		if (info!=ARMPL_STATUS_SUCCESS) printf("ERROR: armpl_spmv_exec_d warm_up returned %d\n", info);
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
statistics_print_labels(__attribute__((unused)) char * buf, __attribute__((unused)) long buf_n)
{
	return 0;
}


int
CSRArrays::statistics_print_data(__attribute__((unused)) char * buf, __attribute__((unused)) long buf_n)
{
	return 0;
}

