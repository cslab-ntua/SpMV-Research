#ifndef MATRIX_MARKET_H
#define MATRIX_MARKET_H

#include "macros/cpp_defines.h"
#include <complex.h>
#ifdef __cplusplus
	#define complex  _Complex
#endif


#ifndef MATRIX_MARKET_FLOAT_T
	#define MATRIX_MARKET_FLOAT_T  double
#endif


/* Matrix in COO format.
 *
 * format : array (dense) or coordinate (COO)
 * field  : weight type -> real, integer, complex, pattern (none)
 *
 * symmetric          : Matrix has some kind of symmetry.
 * standard_symmetry  : Equal to its transpose matrix.
 * skew_symmetry      : Equal to the negative of its transpose matrix.
 *                      The elements of the main diagonal are all zero.
 * hermitian_symmetry : Equal to the conjugate transpose.
 * symmetry_expanded  : Whether the symmetric elements are included or not.
 *                      Always TRUE for general matrices.
 *
 * m   : num rows
 * n   : num columns
 * nnz : num of non-zeros
 *
 * R : row indexes
 * C : column indexes
 * V : values
 */
struct Matrix_Market {
	char * filename;

	char * format;
	char * field;

	int symmetric;
	int standard_symmetry;
	int skew_symmetry;
	int hermitian_symmetry;
	int symmetry_expanded;

	long m;
	long n;
	long nnz;
	long nnz_sym;

	long nnz_diag;
	long nnz_non_diag;

	int * R;
	int * C;

	void * V;
};


void mtx_init(struct Matrix_Market * obj);
struct Matrix_Market * mtx_new();
void mtx_clean(struct Matrix_Market * obj);
void mtx_destroy(struct Matrix_Market ** obj_ptr);


double (* mtx_functor_get_value_as_double(struct Matrix_Market * MTX)) (void *, long);

struct Matrix_Market * mtx_read(char * filename, long expand_symmetry, long pattern_dummy_vals);
void mtx_write(struct Matrix_Market * MTX, char * filename);

void mtx_values_convert_to_real(struct Matrix_Market * MTX);

void mtx_plot(struct Matrix_Market * MTX, char * filename);
void mtx_plot_density(struct Matrix_Market * MTX, char * filename);


#endif /* MATRIX_MARKET_H */

