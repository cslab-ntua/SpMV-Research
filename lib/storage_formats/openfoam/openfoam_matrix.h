#ifndef OPENFOAM_MATRIX_H
#define OPENFOAM_MATRIX_H

#include "macros/cpp_defines.h"


void read_openfoam_matrix(char * filename_owner, char * filename_neigh, int ** rowind_out, int ** colind_out, long * N_out, long * nnz_non_diag_out);
void read_openfoam_matrix_dir(char * dir, int ** rowind_out, int ** colind_out, long * N_out, long * nnz_non_diag_out);


#endif /* OPENFOAM_MATRIX_H */

