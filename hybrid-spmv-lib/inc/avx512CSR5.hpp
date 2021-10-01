///
/// \author Anastasiadis Petros (panastas@cslab.ece.ntua.gr)
///
/// \brief A library wrapper for the CSR5 library
///
#ifndef CSR5_H
#define CSR5_H

#include <gpu_utils.hpp>
#include <spmv_utils.hpp>

#include <cmath>
#include <iostream>

typedef struct {
  int _csr5_sigma;
  int _csr5_omega;
  int _bit_y_offset;
  int _bit_scansum_offset;
  int _num_packet;
  int _tail_partition_start;
  int _p;
  unsigned int *_csr5_partition_pointer;
  unsigned int *_csr5_partition_descriptor;
  int _num_offsets;
  int *_csr5_partition_descriptor_offset_pointer;
  int *_csr5_partition_descriptor_offset;
  double *_temp_calibrator;
  int num_thread;
} aCSR5_wrap;

aCSR5_wrap *aCSR5_desc();

// void vec_alloc_CSR5(SpmvOperator *op, void **x);
// void CSR5_convert(SpmvOperator *op, SpmvFormat format);

void aCSR5_free(aCSR5_wrap *tmp);

#endif
