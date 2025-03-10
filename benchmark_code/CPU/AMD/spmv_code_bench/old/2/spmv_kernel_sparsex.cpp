#include <stdlib.h>
#include <stdio.h>
#include <string.h>
// #include <getopt.h>
#include <omp.h>


#include "macros/cpp_defines.h"

#include "spmv_bench_common.h"
#include "spmv_kernel.h"

#ifdef __cplusplus
extern "C"{
#endif
	// SparceX is C code!!! If I put it outside 'extern "C"' it fails compilation with undefined references.
	#include <sparsex/sparsex.h>

	#include "macros/macrolib.h"
	#include "time_it.h"
	#include "parallel_util.h"
#ifdef __cplusplus
}
#endif

#undef binary_search
#pragma push_macro("ValueType")
#undef ValueType
#include <sparsex/internals/CsxUtil.hpp>
#pragma pop_macro("ValueType")


struct CSXArrays : Matrix_Format
{
	spx_matrix_t * A;
	spx_input_t * input;
	spx_partition_t * parts;
	INT_T * row_ptr;      // the usual rowptr (of size m+1)
	INT_T * ja;      // the colidx of each NNZ (of size nnz)
	ValueType * a;   // the values (of size NNZ)
	double time_create_vector;

	CSXArrays(INT_T * row_ptr, INT_T * ja, ValueType * a, long m, long n, long nnz) : Matrix_Format(m, n, nnz), row_ptr(row_ptr), ja(ja), a(a)
	{
		time_create_vector = 0;
	}

	~CSXArrays()
	{
		spx_input_destroy(input);
		spx_mat_destroy(A);
		spx_partition_destroy(parts);
		// spx_vec_destroy(x);
		// spx_vec_destroy(y);
	}

	void spmv(ValueType * x, ValueType * y);
	void statistics_start();
	int statistics_print_data(__attribute__((unused)) char * buf, __attribute__((unused)) long buf_n);
};


/**
 * spx_vector_t * spx_vec_create_from_buff(spx_value_t * buff, spx_value_t **tuned, size_t size, const spx_partition_t *p, spx_vecmode_t mode);
 *
 *  Creates and returns a valid vector object, whose values are mapped to a
 *  user-defined array. If SPX_VEC_AS_IS is set, then the input buffer will
 *  be shared with the library and modifications will directly apply to it.
 *  In this case pointers \a buff and \a tuned point to the same memory
 *  location. If SPX_VEC_TUNE is selected, the buffer provided by the user
 *  might be copied into an optimally allocated buffer (depending on the
 *  platform) and \a tuned might point to this buffer. Thus, the user must
 *  always check whether \a buff equals \a tuned. If the buffer is actually
 *  tuned, then it should be used instead of the original. The common free()
 *  function applies to this buffer and will have to be explicitly called by
 *  the user.
 *
 *  @param[in] buff         the user-supplied buffer.
 *  @param[in] tuned        the tuned buffer.
 *  @param[in] size         the size of the buffer.
 *  @param[in] p            a partitioning handle.
 *  @param[in] mode         the vector mode (either \c SPX_VEC_AS_IS or
 *                          \c SPX_VEC_TUNE).
 *  @return                 a valid vector object.
 */
void
CSXArrays::spmv(ValueType * x, ValueType * y)
{
	const spx_value_t alpha = 1;
	spx_value_t * x_tuned, * y_tuned;
	spx_vector_t * x_v, * y_v;
	spx_vecmode_t mode;

	mode = SPX_VEC_AS_IS;   // Reuses the memory of the buffers, so y should be the correct output.
	// mode = SPX_VEC_TUNE;

	time_create_vector += time_it(1,
		x_v = spx_vec_create_from_buff(x, &x_tuned, n, parts, mode);
		y_v = spx_vec_create_from_buff(y, &y_tuned, m, parts, mode);
	);
	// printf("Cummulative time for vector creation: %g\n", time_create_vector);

	/* Reorder vectors */
	// spx_perm_t *p = SPX_INVALID_PERM;
	// if (enable_reordering) {
		// p = spx_mat_get_perm(A);
		// if (p == SPX_INVALID_PERM) {
			// SETERROR_0(SPX_ERR_PERM);
			// exit(1);
		// }
		// spx_vec_reorder(x, p);
	// }

	spx_matvec_mult(alpha, A, x_v, y_v);
}


/* Copied from sparsex/include/sparsex/matvec.c
 * (not in a header file for some reason)
 */
struct matrix {
  spx_index_t nrows, ncols, nnz;
  int symmetric;              /**< Flag that indicates whether the symmetric
				 version of CSX will be used */
  spx_perm_t *permutation;    /**< The permutation, in case the matrix has
				 been reordered */
  void *csx;                  /**< The tuned matrix representation, i.e. the
				 input matrix transformed to the CSX format */
};

struct Matrix_Format *
csr_to_format(INT_T * row_ptr, INT_T * col_ind, ValueType * values, long m, long n, long nnz, int symmetric)
{
	if (symmetric)
		error("symmetric matrices not supported by this format, expand symmetry");
	struct CSXArrays * csx = new CSXArrays(row_ptr, col_ind, values, m, n, nnz);
	spx_matrix_t * A = SPX_INVALID_MAT;
	spx_input_t * input;
	spx_partition_t * parts;
	// int enable_reordering = 0;
	double time;

	csx->mem_footprint = nnz * (sizeof(ValueType) + sizeof(INT_T)) + (m+1) * sizeof(INT_T);
	csx->format_name = (char *) "SparseX";

	// "$prog" "${prog_args[@]}" -t -o spx.rt.nr_threads=$t -o spx.rt.cpu_affinity=${mt_conf} -o spx.preproc.xform=all #-v
	// printf("num_threads = %s\n", getenv("OMP_NUM_THREADS"));
	// printf("affinity = %s\n", getenv("GOMP_CPU_AFFINITY"));
	spx_option_set("spx.rt.nr_threads", getenv("OMP_NUM_THREADS"));
	spx_option_set("spx.rt.cpu_affinity", getenv("GOMP_CPU_AFFINITY"));
	spx_option_set("spx.preproc.xform", "all");

	time = time_it(1,
		input = spx_input_load_csr(row_ptr, col_ind, values, m, n);
		if (input == SPX_INVALID_INPUT) {
			SETERROR_0(SPX_ERR_INPUT_MAT);
			exit(1);
		}

		/* Transform to CSX */
		// if (enable_reordering)
			// A = spx_mat_tune(input, SPX_MAT_REORDER);
		// else
			// A = spx_mat_tune(input);
		A = spx_mat_tune(input);
		if (A == SPX_INVALID_MAT) {
			SETERROR_0(SPX_ERR_TUNED_MAT);
			exit(1);
		}

		parts = spx_mat_get_partition(A);
		if (parts == SPX_INVALID_PART) {
			SETERROR_0(SPX_ERR_PART);
			exit(1);
		}
	);
	printf("time preprocessing: %g\n", time);

	spm_mt_t *spm_mt = (spm_mt_t *) A->csx;
	csx->mem_footprint = sparsex::csx::CsxSize<INT_T, ValueType>(spm_mt);

	csx->A = A;
	csx->input = input;
	csx->parts = parts;
	return csx;
}


//==========================================================================================================================================
//= Print Statistics
//==========================================================================================================================================


void
CSXArrays::statistics_start()
{
}


int
statistics_print_labels(__attribute__((unused)) char * buf, __attribute__((unused)) long buf_n)
{
	return 0;
}


int
CSXArrays::statistics_print_data(__attribute__((unused)) char * buf, __attribute__((unused)) long buf_n)
{
	return 0;
}

