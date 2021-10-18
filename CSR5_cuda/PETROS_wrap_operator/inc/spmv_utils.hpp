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
#include <iostream>
#include <limits>
#include <type_traits>
#include "spmv_operator.hpp"
#include <type_traits>
using namespace std;

//#define DDEBUG
//#define DEBUG

/// Print message if DDEBUG is defined
void ddebug(const char *message);

/// Print message if DEBUG is defined
void debug(const char *message);

/// An accurate timer for benchmarking
double csecond(void);

/// Assert with error message (suggested for error checking)
void massert(bool condi, const char *msg);

/// A serial naive CSR implementation
template <typename VALUETYPE_AX, typename VALUETYPE_Y, typename VALUETYPE_COMP>
void spmv_csr(int *csrPtr, int *csrCol, VALUETYPE_AX *csrVal, VALUETYPE_AX *x, VALUETYPE_Y *ys,
              int n);
    
void spmv_csr_f(int *csrPtr, int *csrCol, float *csrVal, float *x, float *ys,
              int n);

/// A serial naive COO implementation
template <typename VALUETYPE>
void spmv_coo(int *csrInd, int *csrCol, VALUETYPE *csrVal, VALUETYPE *x, VALUETYPE *ys,
              int nz);

int get_num_threads(); 

/// Checks if first n elements of two (host memory visible) vectors are equal
template <typename VALUETYPE>
int vec_equals(VALUETYPE *v1, VALUETYPE *v2, int n, VALUETYPE eps);

/// Prints vector 'v1' after 'message'
template <typename VALUETYPE>
void vec_print(VALUETYPE *v1, int n, const char *message);

/// Copies vector 'v1' to 'v' with padding 'np'
template <typename VALUETYPE>
void vec_copy(VALUETYPE *v, VALUETYPE *v1, int n, int np);

/// Adds vector 'src' to 'dest'
template <typename VALUETYPE>
inline void vec_add(VALUETYPE *dest, VALUETYPE *src, int n);

/// DEPRICATED - vec_alloc is recomended - kept for legacy reasons (maybe remade
/// later on)
template <typename VALUETYPE>
void vec_init(VALUETYPE *v, int n, VALUETYPE val);

template <typename VALUETYPE>
void vec_init_rand(VALUETYPE *v, int n, int np);

/// Check if first 'n' elements of vector 'test' are (almost) equal to 'origin'
template <typename VALUETYPE>
void check_result(VALUETYPE *test, VALUETYPE *orig, int n);

/// Report benchmark results in a presentable manner TODO: Maybe improve?
void report_results(double timer, int flops, size_t bytes);

/// Report bandwidth of timed memory transaction
void report_bandwidth(double timer, size_t bytes);

/// Find min element in double vector and its position
double min_elem(double *arr, int size, int *pos);


#endif
