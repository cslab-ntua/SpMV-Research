#ifndef BLAS_H
#define BLAS_H

#include <cstdio>
#include <immintrin.h>

namespace custom_blas{
    void lsolve_BLAS(int dim, int num_col, double *M, double *b);

    void ltsolve_BLAS(int dim, int num_col, double *M, double *b);

    void SpTrSvCSRBlockMatVec(int dim, int nrow, int ncol, double *M, double *x, double *b);

    void SpTrSvCSRLSolve(int dim, int num_col, double *M, double *b);

    void matvec_BLAS(int dim, int nrow, int ncol, double *M, double *x, double *b);

    void mattvec_BLAS(int dim, int nrow, int ncol, double *M, double *x, double *b);

    double DDOT(int n, double *v0, double *v1);

    void DGEMM(int N, int M, int K, const double *A, const double *B, double *C);

    void GEMV(int N, int M, const double *A, const double *x, double *b);

    void DAXPY(int n, double a, double *x, double *y, double *z);

    void AtA(int N, int M, const double *A, double *C);


    ///\Description: This function is computing Matrix Vector Multiplication based on CSR format of M
    /// Where M is part of a supernode with no padding
    ///\param nrow: The number of row in the supernode
    ///\param ncol: The number of independent columns
    ///For example in the below example, the ncol for the supernode is equal to 1
    ///1 1
    ///1 1 1
    ///1 1 1 1
    ///1 1 1 1 1
    ///\param M: is the A in Ax=b computation (note that it is not rectangular, but we used a rectangular
    ///part of it)
    ///\param x: is the x in Ax=b
    ///\param b: is the b in Ax=b
    void SpTrSv_MatVecCSR_BLAS(int nrow, int ncol, double *M, double *x, double *b);

    ///\Description: This function is computing Matrix Vector Multiplication based on CSR format of M
    /// Where M is part of a supernode with no padding
    ///\param nrow: The number of row in the supernode
    ///\param ncol: The number of independent columns
    ///For example in the below example, the ncol for the supernode is equal to 1
    ///1 1
    ///1 1 1
    ///1 1 1 1
    ///1 1 1 1 1
    ///\param M: is the A in Ax=b computation (note that it is not rectangular)
    ///\param b: is the b in Ax=b
    void SpTrSv_LSolveCSR_BLAS(int nrow, int ncol, double *M, double *b);

    ///\Description: This function is computing Matrix Vector Multiplication based on CSR format of M
    /// Where M is part of a supernode with no padding
    ///\param nrow: The number of row in the supernode
    ///\param ncol: The number of independent columns
    ///For example in the below example, the ncol for the supernode is equal to 1
    ///1 1
    ///1 1 1
    ///1 1 1 1
    ///1 1 1 1 1
    ///\param M: is the A in Ax=b computation (note that it is not rectangular, but we used a rectangular
    ///part of it)
    ///\param x: is the x in Ax=b
    ///\param b: is the b in Ax=b
    void SpTrSv_MatVecCSC_BLAS(int nrow, int ncol, double *M, double *x, double *b);

    ///\Description: This function is computing Matrix Vector Multiplication based on CSR format of M
    /// Where M is part of a supernode with no padding
    ///\param nrow: The number of independent rows in the supernode
    ///\param ncol: The number of columns in the super node
    ///For example in the below example, the nrows for the supernode is equal to 1
    ///1
    ///1 1
    ///1 1 1
    ///1 1 1 1
    ///1 1 1 1
    ///\param M: is the A in Ax=b computation (note that it is not rectangular)
    ///\param b: is the b in Ax=b
    void SpTrSv_LSolveCSC_BLAS(int nrow, int ncol, double *M, double *b);
}
#endif