#ifndef GHOST_APPS_ESSEXAMPLES_H
#define GHOST_APPS_ESSEXAMPLES_H

#include <ghost.h>
#if HAVE_ESSEX_PHYSICS
#include <essex-physics/cheb_toolbox.h>
#endif

#define ESSEXAMPLES_PRINTRANK_ALL -1
#define ESSEXAMPLES_PRINTRANK_MIDDLE -2


#ifdef __cplusplus
extern "C" {
#endif

int essexamples_process_options(int argc, char **argv);
void essexamples_create_matrix(ghost_sparsemat **mat, ghost_context *ctx, ghost_sparsemat_traits *mtraits);
// void essexamples_create_artificial_matrix(ghost_sparsemat **mat, ghost_context *ctx, ghost_sparsemat_traits *mtraits, ghost_gidx nr_rows, ghost_gidx nr_cols, ghost_gidx *row_ptr, ghost_gidx *col_ind, double *values);
void essexamples_create_artificial_matrix(ghost_sparsemat **mat, ghost_context *ctx, ghost_sparsemat_traits *mtraits, ghost_gidx nr_rows, ghost_gidx nr_cols, ghost_lidx *row_ptr, ghost_lidx *col_ind, double *values);
// void essexamples_create_matrix_ft(ghost_sparsemat **mat, ghost_context *ctx, ghost_sparsemat_traits *mtraits, MPI_Comm * ftMpiComm);
void essexamples_print_usage();
void essexamples_print_info(ghost_sparsemat *mat, int printrank);
void essexamples_set_spmv_flags(ghost_spmv_flags *flags);
void essexamples_set_auto_permute(int val);
void essexamples_get_extremal_eig_range( double * low, double * high );
void essexamples_get_target_eig_range( double * low, double * high );
void essexamples_get_matstr(char **str);
void essexamples_get_randvecnum( int * R );
void essexamples_get_chebmoments( int * M );
void essexamples_get_output_file( char ** filename );
void essexamples_get_iterations(int *nIter);
void essexamples_create_densemat(ghost_densemat **dm, ghost_densemat_traits *traits, ghost_map *map);
void essexamples_get_cp_folder(char **p);
void essexamples_get_cp_freq(int * f);
void essexamples_get_restart(int * r);
void essexamples_get_verbose(int * v);
void essexamples_get_blockvecnum( int *vecnum );

#if HAVE_ESSEX_PHYSICS
void essexamples_get_cheb_mode( ChebLoop_Options * opt );
#endif

#ifdef __cplusplus
}
#endif

#endif
