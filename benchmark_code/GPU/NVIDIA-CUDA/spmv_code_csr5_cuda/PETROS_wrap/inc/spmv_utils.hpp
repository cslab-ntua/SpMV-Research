///
/// \author Anastasiadis Petros (panastas@cslab.ece.ntua.gr)
///
/// \brief Some helpfull functions for SpMV
///
#ifndef SPMV_UTILS_H
#define SPMV_UTILS_H
#include <omp.h>
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <iomanip>
#include <iostream>

//#define DDEBUG
//#define DEBUG

typedef struct {
  int *rowInd = NULL;
  int *colInd = NULL;
  void *values = NULL;
  int m, n, nz;
} SpmvCooData;

typedef struct {
  int *rowPtr = NULL;
  int *colInd = NULL;
  void *values = NULL;
  int m, n, nz;
} SpmvCsrData;

/// Print message if DDEBUG is defined
void ddebug(const char *message);

/// Print message if DEBUG is defined
void debug(const char *message);

/// An accurate timer for benchmarking
double csecond(void);

/// Assert with error message (suggested for error checking)
void massert(bool condi, const char *msg);

/// A serial naive CSR implementation
void spmv_csr(int *csrPtr, int *csrCol, double *csrVal, double *x, double *ys,
              int n);

/// A serial naive COO implementation
void spmv_coo(int *csrInd, int *csrCol, double *csrVal, double *x, double *ys,
              int nz);

void spmv_coo_f(int *csrInd, int *csrCol, float *csrVal, float *x, float *ys,
              int nz);

int get_num_threads(); 

/// Checks if first n elements of two (host memory visible) vectors are equal
template <typename VALUETYPE>
int vec_equals(VALUETYPE *v1, VALUETYPE *v2, int n, VALUETYPE eps) {
  int i, k = 0;
  FILE *f = fopen("equals.out", "w");
  for (i = 0; i < n; ++i) {
    if (fabs((v1[i] - v2[i])/v1[i]) > eps ) {
      fprintf(f, "%lf - %lf = %lf\n", v1[i], v2[i], fabs(v1[i] - v2[i]));
      k++;
    }
  }
  fclose(f);
  return k;
}

/// Prints vector 'v1' after 'message'
template <typename VALUETYPE>
void vec_print(VALUETYPE *v1, int n, const char *message) {
  int i;
  printf("%s:", message);
  for (i = 0; i < n; ++i) std::cout << v1[i] << " ";
  printf("\n");
}

/// Copies vector 'v1' to 'v' with padding 'np'
template <typename VALUETYPE>
void vec_copy(VALUETYPE *v, VALUETYPE *v1, int n, int np) {
  int i;
#pragma omp parallel for
  for (i = 0; i < n; ++i) {
    v[i] = v1[i];
  }
#pragma omp parallel for
  for (i = n; i < n + np; ++i) {
    v[i] = 0;
  }
}

/// Adds vector 'src' to 'dest'
template <typename VALUETYPE>
inline void vec_add(VALUETYPE *dest, VALUETYPE *src, int n) {
  int i;
#pragma omp parallel for
  for (i = 0; i < n; ++i) {
    dest[i] += src[i];
  }
}

/// Check if first 'n' elements of vector 'test' are (almost) equal to 'origin'
template <typename VALUETYPE>
void check_result(VALUETYPE *test, VALUETYPE *orig, int n) {
  VALUETYPE eps;
  if (std::is_same<VALUETYPE, double>::value) eps = 0.00000001;
  else if (std::is_same<VALUETYPE, float>::value) eps = 0.0001;
  int i_fail = vec_equals<VALUETYPE>(test, orig, n, eps /*std::numeric_limits<VALUETYPE>::epsilon()*/);
  if (!i_fail)
    fprintf(stderr," PASSED TEST\n");
  else
    fprintf(stderr," FAILED %d times\n", i_fail);
}

/// Report benchmark results in a presentable manner TODO: Maybe improve?
void report_results(double timer, int flops, size_t bytes);

/// Report bandwidth of timed memory transaction
void report_bandwidth(double timer, size_t bytes);

/// Find min element in double vector and its position
double min_elem(double *arr, int size, int *pos);

/// DEPRICATED - vec_alloc is recomended - kept for legacy reasons (maybe remade
/// later on)
void vec_init(double *v, int n, double val);
void vec_init_rand(double *v, int n, int np);

SpmvCooData* mtx_read_coo(char* mtx_name);

SpmvCsrData* mtx_read_csr(char* mtx_name);

SpmvCsrData* sortedcoo2csrhost(SpmvCooData* coo_input);

#endif
