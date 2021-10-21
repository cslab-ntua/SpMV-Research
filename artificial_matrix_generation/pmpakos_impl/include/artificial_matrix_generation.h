#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include <Python.h>
#define PY_SSIZE_T_CLEAN

typedef VALUE_TYPE_AX ValueType;
const int VALUE_TYPE_BIT_WIDTH=8*sizeof(VALUE_TYPE_AX);

typedef unsigned int IndexType;
#define INDEX_FMT  "u"
#define INDEX_TYPE_BIT_WIDTH 32

typedef struct csr_matrix {
	IndexType *row_ptr;
	IndexType *col_ind;
	ValueType *values;

	unsigned int nr_rows;
	unsigned int nr_cols;
	unsigned int nr_nzeros;

	double density;
	double mem_footprint;
	int precision;
	char mem_range[128];

	double avg_nnz_per_row;
	double std_nnz_per_row;

	int seed;
	char distribution[128];
	char placement[128];
	double diagonal_factor;

	double avg_bw;
	double std_bw;
	double avg_sc;
	double std_sc;

	double time1; // time to create the nonzeros per row vector
	double time2; // time to crate the col_ind vector (using nonzeros/row vector from previous step)
} csr_matrix;

double getTimestamp();

csr_matrix *create_csr_matrix(unsigned int nr_rows, unsigned int nr_cols, unsigned int nr_nzeros,
							  double density, char mem_range[128],
							  double avg_nnz_per_row, double std_nnz_per_row,
							  int seed, char *distribution, char *placement, double diagonal_factor,
							  double avg_bw, double std_bw, double avg_sc, double std_sc,
							  double time1, double time2);

void delete_csr_matrix(csr_matrix *matrix);

csr_matrix *artificial_matrix_generation(int argc, char *argv[], int starting_point, int verbose);
