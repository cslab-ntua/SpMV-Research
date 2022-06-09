///
/// \author Anastasiadis Petros (panastas@cslab.ece.ntua.gr)
///
/// \brief Nvidia CuSPARSE wrapper for benchmarking
///
#ifndef CUSP_H
#define CUSP_H
#include <cusp/array1d.h>
#include <cusp/multiply.h>
#include <cusp/system/cuda/detail/multiply/csr_scalar.h>
#include <gpu_utils.hpp>
#include <spmv_utils.hpp>

// FIXME: Stuff added or remove??
typedef struct { int flag; } cuSP_wrap;

/// Create a library descriptor for the cuSPARSE library
/// \return the descriptor
cuSP_wrap *cuSP_desc();

/// Initalize a cuSP library descriptor and change op to a compatible form
void cuSP_init(SpmvOperator *op);

/// A function for allocating cuSP vectors x and y(=0) in 'op'
void vec_alloc_cuSP(SpmvOperator *op, void **x);

/// TODO: ALL sub-functions are not implemented
/// Copy the Spmv struct of 'op' residing in cuSP memory
void *Spmv_data_copy_cuSP(SpmvOperator *op);

/// Free a cuSP vector pointed by dataptr
void vec_free_cuSP(void *dataptr);

/// Free cuSP_wrap struct
void cuSP_free(cuSP_wrap *tmp);

// FIXME: generalize, remove or keep???
double *cuSP_get_y(SpmvOperator *op);

// FIXME: generalize + adapt
void cuSP_convert(SpmvOperator *op, SpmvFormat format);

// FIXME: generalize or remove??
SpmvOperator *cuSP_op(SpmvOperator *op, SpmvFormat format);

// FIXME: add massert check
void cuSP_coo(SpmvOperator *op);

// FIXME: add massert check
void cuSP_csr_vector(SpmvOperator *op);

// FIXME: add massert check
void cuSP_csr_scalar(SpmvOperator *op);

// FIXME: add massert check
void cuSP_dia(SpmvOperator *op);

// FIXME: add massert check
void cuSP_ell(SpmvOperator *op);

// FIXME: add massert check
void cuSP_hyb(SpmvOperator *op);

#endif
