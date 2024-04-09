//
// Created by kazem on 2/15/22.
//

#include <iostream>

namespace Example{

 void spmv_csr(int n, const int *Ap, const int *Ai, const double *Ax,
               const double *x, double *y) {
  int cnt = 0;
  for (int i = 0; i < n; i++) {
   for (int j = Ap[i]; j < Ap[i + 1]; j++) {
    //y[i] += Ax[j] * x[Ai[j]];
    std::cout<<cnt<<": "<<i<<","<<j<<","<<Ai[j]<<",MULADD"<<"\n";
    cnt++;
   }
  }
 }


 void sptrsv_csr(int n, int *Lp, int *Li, double *Lx, double *x) {
  int i, j, cnt=0;
  for (i = 0; i < n; i++) {
   for (j = Lp[i]; j < Lp[i + 1] - 1; j++) {
    // x[i] -= Lx[j] * x[Li[j]];
    std::cout<<cnt<<": "<<i<<","<<j<<","<<j<<",MULADD"<<"\n";
    cnt++;
   }
//   x[i] /= Lx[Lp[i + 1] - 1];
   std::cout<<cnt<<": "<<i<<","<<i<<","<<Lp[i+1]-1<<",DIV"<<"\n";
   cnt++;
  }
 }


 void spmm_csr_csr(double* Cx, double* Ax, double* Bx, int m, int* Ap, int* Ai, int bRows, int bCols) {
  int cnt = 0;
  for (int i = 0; i < m; ++i) {
   for (int j = Ap[i]; j < Ap[i+1]; ++j) {
    for (int k = 0; k < bCols; ++k) {
     //Cx[i*bCols+k] += Ax[j] * Bx[Ai[j]*bCols+k];
     std::cout<<cnt<<":"<<i*bCols+k<<","<<j<<","<<Ai[j]*bCols+k<<",MULADD\n";
     cnt++;
    }
   }
  }
 }

}
using namespace Example;
int main(){
 int n = 10;
 int n_col = 3;
 int nnz = 18;
 int Ap[11] = {0,1,2,3,4,5,8,11,12,15,18};
 int Ai[18] = {0,1,2,3,4,0,3,5,1,4,6,7,0,3,8,2,6,9};
 double *Ax = new double[nnz];
 std::fill_n(Ax, nnz, 1.0);
 double *x = new double [n]();
 double *y = new double [n]();
 std::fill_n(y, n, 1.0);
 double *Bx = new double [n*n_col]();
 std::fill_n(Bx, n+n_col, 1.0);
 double *Cx = new double [n*n_col]();

 std::cout<<"SpMV\n";
 spmv_csr(n, Ap, Ai, Ax, x, y);

 std::cout<<"\nSpTRSV\n";
 sptrsv_csr(n, Ap, Ai, Ax, x);

 std::cout<<"\nSpMM\n";
 spmm_csr_csr(Cx, Ax, Bx, n, Ap, Ai, n, n_col);

 delete []Cx;
 delete []Bx;
 delete []Ax;
 delete []x;
 delete []y;
 return 0;
}
