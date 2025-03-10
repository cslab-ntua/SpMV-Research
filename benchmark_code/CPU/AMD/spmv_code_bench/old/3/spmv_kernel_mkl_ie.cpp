#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

#include <mkl.h>

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
	INT_T * row_ptr;      // the usual rowptr (of size m+1)
	INT_T * ja;      // the colidx of each NNZ (of size nnz)
	sparse_matrix_t A;
	matrix_descr descr;

	CSRArrays(long m, long n, long nnz) : Matrix_Format(m, n, nnz)
	{
		a = NULL;
		row_ptr = NULL;
		ja= NULL;
	}

	~CSRArrays()
	{
		free(a);
		free(row_ptr);
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
csr_to_format(INT_T * row_ptr, INT_T * col_ind, ValueTypeReference * values, long m, long n, long nnz, long symmetric, long symmetry_expanded)
{
	struct CSRArrays * csr = new CSRArrays(m, n, nnz);
	double time, time_balance;

	mkl_verbose(1);

	csr->format_name = (char *) "MKL_IE";

	csr->row_ptr = (typeof(csr->row_ptr)) aligned_alloc(64, (m+1) * sizeof(*csr->row_ptr));
	csr->ja = (typeof(csr->ja)) aligned_alloc(64, nnz * sizeof(*csr->ja));
	csr->a = (typeof(csr->a)) aligned_alloc(64, nnz * sizeof(*csr->a));
	#pragma omp parallel for
	for (long i=0;i<m+1;i++)
		csr->row_ptr[i] = row_ptr[i];
	#pragma omp parallel for
	for(long i=0;i<nnz;i++)
	{
		csr->a[i]=values[i];
		csr->ja[i]=col_ind[i];
	}

	time = time_it(1,
		// The input arrays provided are left unchanged except for the call to mkl_sparse_order, which performs ordering of column indexes of the matrix.
		// To avoid any changes to the input data, use mkl_sparse_copy.
		#if DOUBLE == 0
			mkl_sparse_s_create_csr(&csr->A, SPARSE_INDEX_BASE_ZERO, m, n, csr->row_ptr, csr->row_ptr+1, csr->ja, csr->a);
		#elif DOUBLE == 1
			mkl_sparse_d_create_csr(&csr->A, SPARSE_INDEX_BASE_ZERO, m, n, csr->row_ptr, csr->row_ptr+1, csr->ja, csr->a);
		#endif
		mkl_sparse_order(csr->A);  // Sort the columns.
	);
	printf("mkl mkl_sparse_s_create_csr + mkl_sparse_order time = %g\n", time);

	if (symmetric && !symmetry_expanded)
	{
		csr->descr.type = SPARSE_MATRIX_TYPE_SYMMETRIC;
		csr->descr.mode = SPARSE_FILL_MODE_LOWER;  // mtx symmetric have elements only on or below the main diagonal (lower triangular)
		csr->descr.diag = SPARSE_DIAG_NON_UNIT;
	}
	else
	{
		csr->descr.type = SPARSE_MATRIX_TYPE_GENERAL;
	}
	const sparse_operation_t operation = SPARSE_OPERATION_NON_TRANSPOSE;
	const INT_T expected_calls = 128;

	// Using SPARSE_MEMORY_AGGRESSIVE policy for some reason gives libgomp error 'out of memory' at 128 theads.
	//     SPARSE_MEMORY_NONE
	//         Routine can allocate memory only for auxiliary structures (such as for workload balancing); the amount of memory is proportional to vector size.
	//     SPARSE_MEMORY_AGGRESSIVE
	//         Default.
	//         Routine can allocate memory up to the size of matrix A for converting into the appropriate sparse format.
	// const sparse_memory_usage_t policy = SPARSE_MEMORY_AGGRESSIVE;
	const sparse_memory_usage_t policy = SPARSE_MEMORY_NONE;

	time_balance = time_it(1,
		mkl_sparse_set_mv_hint(csr->A, operation, csr->descr, expected_calls);
		mkl_sparse_set_memory_hint(csr->A, policy);
		mkl_sparse_optimize(csr->A);
	);
	printf("mkl optimize time = %g\n", time_balance);

	#if 0
		for (i=0;i<csr_m;i++)
		{
			for (long j=csr->row_ptr[i];j<csr->row_ptr[i+1];j++)
			{
				if (j < csr->row_ptr[i+1]-1 && csr->ja[j] >= csr->ja[j+1])
					error("%ld: unsorted columns: %d >= %d", i, csr->ja[j], csr->ja[j+1]);
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


void
compute_sparse_mv(CSRArrays * csr, ValueType * x , ValueType * y)
{
	long ret;
        #if DOUBLE == 0
		ret = mkl_sparse_s_mv(SPARSE_OPERATION_NON_TRANSPOSE, 1.0f, csr->A, csr->descr, x, 0.0f, y);
        #elif DOUBLE == 1
		ret = mkl_sparse_d_mv(SPARSE_OPERATION_NON_TRANSPOSE, 1.0f, csr->A, csr->descr, x, 0.0f, y);
        #endif
	if (ret != SPARSE_STATUS_SUCCESS)
		error("execution failed");
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

