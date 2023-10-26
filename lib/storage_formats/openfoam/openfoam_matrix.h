#ifndef OPENFOAM_MATRIX_H
#define OPENFOAM_MATRIX_H

#include "macros/cpp_defines.h"


void read_openfoam_matrix(const char * filename_owner, const char * filename_neigh, int ** rowind_out, int ** colind_out, int * N_out, int * nnz_non_diag_out);
void read_openfoam_matrix_dir(const char * dir, int ** rowind_out, int ** colind_out, int * N_out, int * nnz_non_diag_out);


#endif /* OPENFOAM_MATRIX_H */

