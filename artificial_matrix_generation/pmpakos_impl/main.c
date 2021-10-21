#include <stdio.h>
#include <stdlib.h>
#include "artificial_matrix_generation.h"

int main(int argc, char *argv[])
{
	int start_of_matrix_generation_args = 1;
	int verbose = 1; // 0 : printf nothing
	csr_matrix *matrix=NULL;
	matrix = artificial_matrix_generation(argc, argv, start_of_matrix_generation_args, verbose);

	if(matrix!=NULL){
		if(verbose){
			fprintf(stderr, "\n================================================= MATRIX INFO =================================================\n");
			fprintf(stderr, "matrix->nr_rows = %u\n",matrix->nr_rows);
			fprintf(stderr, "matrix->nr_cols = %u\n",matrix->nr_cols);
			fprintf(stderr, "matrix->nr_nzeros = %u\n",matrix->nr_nzeros);
			fprintf(stderr, "\n");
			fprintf(stderr, "matrix->density = %f\n",matrix->density);
			fprintf(stderr, "matrix->mem_footprint = %f\n",matrix->mem_footprint);
			fprintf(stderr, "matrix->precision = %d-bit precision\n",matrix->precision);
			fprintf(stderr, "matrix->mem_range = %s\n",matrix->mem_range);
			fprintf(stderr, "\n");
			fprintf(stderr, "matrix->avg_nnz_per_row = %f\n",matrix->avg_nnz_per_row);
			fprintf(stderr, "matrix->std_nnz_per_row = %f\n",matrix->std_nnz_per_row);
			fprintf(stderr, "\n");
			fprintf(stderr, "matrix->seed = %d\n",matrix->seed);
			fprintf(stderr, "matrix->distribution = %s\n",matrix->distribution);
			fprintf(stderr, "matrix->placement = %s\n",matrix->placement);
			fprintf(stderr, "matrix->diagonal_factor = %f\n",matrix->diagonal_factor);
			fprintf(stderr, "\n");
			fprintf(stderr, "matrix->avg_bw = %f\n",matrix->avg_bw);
			fprintf(stderr, "matrix->std_bw = %f\n",matrix->std_bw);
			fprintf(stderr, "matrix->avg_sc = %f\n",matrix->avg_sc);
			fprintf(stderr, "matrix->std_sc = %f\n",matrix->std_sc);
			fprintf(stderr, "\n");
			fprintf(stderr, "matrix->time1 = %f\n",matrix->time1);
			fprintf(stderr, "matrix->time2 = %f\n",matrix->time2);
			fprintf(stderr, "\n                              =====================================================                          \n\n");
			fprintf(stderr, "row_ptr = [");
			for(unsigned int i=0; i<matrix->nr_rows+1; i++){
				if(i==10)
					break;
				fprintf(stderr, "%u ", matrix->row_ptr[i]);
			}
			fprintf(stderr, "... ]\ncol_ind = [");
			for(unsigned int i=0; i<matrix->nr_nzeros; i++){
				if(i==10)
					break;
				fprintf(stderr, "%u ", matrix->col_ind[i]);
			}
			fprintf(stderr, "... ]\nvalues  = ["); // initialized with 14 different values
			for(unsigned int i=0; i<matrix->nr_nzeros; i++){
				if(i==10)
					break;
				fprintf(stderr, "%.4f ", matrix->values[i]);
			}
			fprintf(stderr, "... ]\n");
			fprintf(stderr, "\n===============================================================================================================\n");
		}		
	}
	else
		fprintf(stderr, "Didn't make it with the given matrix features. Try again.\n");

	delete_csr_matrix(matrix);
	return 0;
}
