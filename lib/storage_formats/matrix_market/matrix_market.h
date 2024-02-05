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


/*
 * Matrix in COO format.
 *
 * field: weight type -> real, integer, complex, pattern (none)
 *
 * m: num rows
 * n: num columns
 * nnz: num of non-zeros
 *
 * R: row indexes
 * C: column indexes
 * V: values
 */
struct Matrix_Market {
	char * filename;

	char * format;
	char * field;
	int symmetric;
	int skew_symmetric;

	long m;
	long n;
	long nnz;
	long nnz_sym;

	int * R;
	int * C;

	void * V;
};


void mtx_init(struct Matrix_Market * obj);
struct Matrix_Market * mtx_new();
void mtx_clean(struct Matrix_Market * obj);
void mtx_destroy(struct Matrix_Market ** obj_ptr);


double (* mtx_functor_get_value(struct Matrix_Market * MTX)) (void *, long);

struct Matrix_Market * mtx_read(char * filename, long expand_symmetry, long pattern_dummy_vals);
void mtx_write(struct Matrix_Market * MTX, char * filename);
void mtx_plot(struct Matrix_Market * MTX, char * filename);
void mtx_plot_density(struct Matrix_Market * MTX, char * filename);


#endif /* MATRIX_MARKET_H */

