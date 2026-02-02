#ifndef BENCH_COMMON_H
#define BENCH_COMMON_H

#include <stdint.h>
#include <math.h>

#include "macros/cpp_defines.h"

#ifdef __cplusplus
extern "C"{
#endif

	#include "artificial_matrix_generation.h"

#ifdef __cplusplus
}
#endif


#ifndef INT_T
	#define INT_T  int32_t
#endif

#ifndef ValueType
	#define ValueType  double
#endif

#ifndef ValueType
	#define ValueTypeReference  double
#endif


struct CSR_reference_s {
	char * filename;
	char * matrix_name;

	ValueTypeReference * a_ref;   // values (of size NNZ)
	INT_T * ia;                   // rowptr (of size m+1)
	INT_T * ja;                   // colidx of each NNZ (of size nnz)

	long symmetric;
	long expanded_symmetry;

	long m;
	long n;
	long nnz;           // nnz stored
	long nnz_matrix;    // actual nnz of the matrix

	long nnz_diag;
	long nnz_non_diag;

	struct csr_matrix AM_stats;
};


#endif /* BENCH_COMMON_H */

