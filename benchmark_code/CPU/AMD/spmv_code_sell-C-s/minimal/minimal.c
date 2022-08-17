#include <ghost.h>
#include <stdio.h>

#define N 4

static int diag(ghost_gidx row, ghost_lidx *rowlen, ghost_gidx *col, void *val, __attribute__((unused)) void *arg)
{
    *rowlen = 1;
    col[0] = row;
    ((double *)val)[0] = (double)(row+1);
    
    return 0;
}

int main(int argc, char **argv) 
{
    ghost_sparsemat *A;
    ghost_densemat *y, *x;
    double zero = 0.;
    char *Astr, *xstr, *ystr;
    
    ghost_sparsemat_traits mtraits = GHOST_SPARSEMAT_TRAITS_INITIALIZER;
    mtraits.datatype = (ghost_datatype)(GHOST_DT_REAL|GHOST_DT_DOUBLE);

    ghost_init(argc,argv);

    // create matrix source
    ghost_sparsemat_src_rowfunc matsrc = GHOST_SPARSEMAT_SRC_ROWFUNC_INITIALIZER;
    matsrc.func = diag;
    matsrc.maxrowlen = 1;
    matsrc.gnrows = N;

    // create sparse matrix A from row-wise function    
    ghost_sparsemat_create(&A, NULL, &mtraits, 1);
    ghost_sparsemat_init_rowfunc(A,&matsrc,MPI_COMM_WORLD,1.);

    // create and initialize input vector x and output vector y
    ghost_densemat_create(&x, A->context->col_map, GHOST_DENSEMAT_TRAITS_INITIALIZER);
    ghost_densemat_create(&y, A->context->row_map, GHOST_DENSEMAT_TRAITS_INITIALIZER);
    ghost_densemat_init_rand(x);      // x = random
    ghost_densemat_init_val(y,&zero); // y = 0

    // compute y = A*x
    ghost_spmv(y,A,x,GHOST_SPMV_OPTS_INITIALIZER);
   
    // print y, A and x 
    ghost_sparsemat_string(&Astr,A,1);
    ghost_densemat_string(&xstr,x);
    ghost_densemat_string(&ystr,y);
    printf("%s\n=\n%s\n*\n%s\n",ystr,Astr,xstr);
 
    // clean up 
    free(Astr); 
    free(xstr);
    free(ystr);
    ghost_sparsemat_destroy(A);
    ghost_densemat_destroy(x);
    ghost_densemat_destroy(y);

    ghost_finalize();

    return 0;
}

