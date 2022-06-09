///
/// \author Anastasiadis Petros (panastas@cslab.ece.ntua.gr)
///
/// \brief Some helpfull functions for SpMV
///

#include <unistd.h>
#include <cassert>
#include <cstdio>
#include "cuSPARSE.hpp"

#include "gpu_utils.hpp"

/// SpmvOperator functions
SpmvOperator::SpmvOperator(int argc, char *argv[], int start_of_matrix_generation_args, int verbose) {
  ddebug(" -> SpmvOperator::SpmvOperator()\n");
  /// Default intialization values for an operator
  mem_bytes = 0;
  mem_bytes += sizeof(SpmvOperator);
  n = m = nz = bytes = flops = bsr_blockDim = 0;
  x = NULL;
  y = NULL;
  mem_alloc = SPMV_MEMTYPE_HOST;
  format_data = NULL;
  format = SPMV_FORMAT_CSR;
  lib_struct = NULL;
  lib = SPMV_NONE;
  mtx_name = "synthetic";
  mtx_generate(argc, argv, start_of_matrix_generation_args, verbose);
  bytes = sizeof(int) * (m + 1) + sizeof(int) * nz + sizeof(VALUE_TYPE_AX) * nz +
          sizeof(VALUE_TYPE_Y) * m + sizeof(VALUE_TYPE_AX) * n;
  flops = 2 * nz;
  ddebug(" <- SpmvOperator::SpmvOperator()\n");
}

SpmvOperator::SpmvOperator(char *matrix_name) {
  ddebug(" -> SpmvOperator::SpmvOperator(matrix_name)\n");
  /// Default intialization values for an operator
  mem_bytes = 0;
  mem_bytes += sizeof(SpmvOperator);
  mtx_name = matrix_name;
  n = m = nz = bytes = flops = bsr_blockDim = 0;
  x = NULL;
  y = NULL;
  mem_alloc = SPMV_MEMTYPE_HOST;
  format_data = NULL;
  format = SPMV_FORMAT_CSR;
  lib_struct = NULL;
  lib = SPMV_NONE;
  mtx_read();
  bytes = sizeof(int) * (m + 1) + sizeof(int) * nz + sizeof(VALUE_TYPE_AX) * nz +
          sizeof(VALUE_TYPE_Y) * m + sizeof(VALUE_TYPE_AX) * n;
  flops = 2 * nz;
  ddebug(" <- SpmvOperator::SpmvOperator(matrix_name)\n");
}

SpmvOperator::SpmvOperator(SpmvOperator &op) {
  ddebug(" -> SpmvOperator::SpmvOperator(copy)\n");
  lib = op.lib;
  // TODO: This switch could be replaced with an actual function
  switch (lib) {
    case (SPMV_LIBRARY_CUSPARSE):
      lib_struct = cuSPARSE_desc();
      debug(
          "SpmvOperator::SpmvOperator(copy) -> Generated new cuSPARSE_desc\n");
      break;
    case (SPMV_NONE):
      debug(
          "SpmvOperator::SpmvOperator(copy) -> warning... copying SPMV_NONE "
          "operator\n");
      break;
    case (SPMV_LIBRARY_OPENMP):
      lib_struct = NULL;
      break;
    default: {
      massert(false,
              "SpmvOperator::SpmvOperator(copy) -> Unreachable lib default "
              "reached\n");
      break;
    }
  }
  bsr_blockDim = op.bsr_blockDim;
  mem_alloc = op.mem_alloc;
  mtx_name = op.mtx_name;
  format = op.format;
  mem_alloc = op.mem_alloc;
  m = op.m;
  n = op.n;
  nz = op.nz;
  density =  op.density;
	//bytes = matrix->mem_footprint;
	avg_nnz_per_row = op.avg_nnz_per_row;
	std_nnz_per_row = op.std_nnz_per_row;
	avg_bw = op.avg_bw;
	std_bw = op.std_bw;
	avg_sc = op.avg_sc;
	std_sc = op.std_sc;
	distribution = op.distribution;
	placement = op.placement;
	skew = op.skew;
	avg_bw_scaled = op.avg_bw_scaled;
	std_bw_scaled = op.std_bw_scaled;
	avg_sc_scaled = op.avg_sc_scaled;
	std_sc_scaled = op.std_sc_scaled;
	avg_num_neighbours = op.avg_num_neighbours;
	cross_row_similarity = op.cross_row_similarity;
	mem_range = op.mem_range; 
	seed = op.seed;
  
  flops = op.flops;
  bytes = op.bytes;
  format_data = op.spmv_data_get_copy();
  debug("SpmvOperator::SpmvOperator(copy) -> Copied format struct\n");

  // TODO: This switch could be replaced with an actual function
  switch (op.mem_alloc) {
    case (SPMV_MEMTYPE_HOST):
    case (SPMV_MEMTYPE_UNIFIED):
      vec_alloc(op.x);
      debug("SpmvOperator::SpmvOperator(copy) -> Copied x vector\n");
      break;
    case (SPMV_MEMTYPE_DEVICE):
      massert(0,
              "SpmvOperator::SpmvOperator(copy) -> Copy from device vector not "
              "supported\n");
      break;
    default:
      massert(0,
              "SpmvOperator::SpmvOperator(copy) -> Unreachable mem_alloc "
              "default reached\n");
      break;
  }
  ddebug(" <- SpmvOperator::SpmvOperator(copy)\n");
}

SpmvOperator::SpmvOperator(SpmvOperator &op, int start, int end, int mode) {
  ddebug(" -> SpmvOperator::SpmvOperator(copy_op,start,end)\n");
  massert(start >= 0 && start < op.nz,
          "SpmvOperator::SpmvOperator(copy_op,start,end) -> start is not "
          "within accepted limits");
  massert(end > 0 && end <= op.nz,
          "SpmvOperator::SpmvOperator(copy_op,start,end) -> end is not within "
          "accepted limits");
  massert(end > start,
          "SpmvOperator::SpmvOperator(copy_op,start,end) -> end is not greater "
          "than start");
  op.format_convert(SPMV_FORMAT_COO);
  lib = op.lib;
  // TODO: This switch could be replaced with an actual function
  switch (lib) {
    case (SPMV_LIBRARY_CUSPARSE):
      lib_struct = cuSPARSE_desc();
      debug(
          "SpmvOperator::SpmvOperator(copy_op,start,end) -> Generated new "
          "cuSPARSE_desc\n");
      break;
    case (SPMV_NONE):
      debug(
          "SpmvOperator::SpmvOperator(copy_op,start,end) -> warning... copying "
          "SPMV_NONE "
          "operator\n");
      break;
    case (SPMV_LIBRARY_OPENMP):
      lib_struct = NULL;
      break;
    default: {
      massert(false,
              "SpmvOperator::SpmvOperator(copy_op,start,end) -> Unreachable "
              "lib default "
              "reached\n");
      break;
    }
  }
  bsr_blockDim = op.bsr_blockDim;
  mem_alloc = op.mem_alloc;
  mtx_name = op.mtx_name;
  format = op.format;
  mem_alloc = op.mem_alloc;
  // TODO: All the splitting mechanism will be defined here
  m = n = op.m;
  nz = end - start;
  
	distribution = op.distribution;
	placement = op.placement;
	avg_num_neighbours = 0;
	cross_row_similarity = 0;
	mem_range = op.mem_range; 
	skew = 0;
	avg_bw_scaled = 0;
	std_bw_scaled = 0;
	avg_sc_scaled = 0;
	std_sc_scaled = 0;
	seed = op.seed;
	density = 0; 
	avg_nnz_per_row = 0;
	std_nnz_per_row = 0;
	avg_bw = 0;
	std_bw = 0;
	avg_sc = 0;
	std_sc = 0;
	
  format_data = op.spmv_data_get_subcopy(&start, &nz, mode);
  /// FIXME:EXP
  /*
  if (mode == 0) n = ((SpmvCooData *)format_data)->rowInd[nz - 1] + 1;
  else  if (mode == 1) n = m - ((SpmvCooData *)op.format_data)->rowInd[op.nz -
  nz];

    bytes = 2 * sizeof(int) * n + 1 * sizeof(int) * nz + 2 * sizeof(double) * nz
  +
            2 * sizeof(double) * n;
    /// FLOPS
    flops = 2 * nz;
  */

  debug(
      "SpmvOperator::SpmvOperator(copy_op,start,end) -> Copied format "
      "struct\n");

  // TODO: This switch could be replaced with an actual function
  switch (op.mem_alloc) {
    case (SPMV_MEMTYPE_HOST):
    case (SPMV_MEMTYPE_UNIFIED):
      vec_alloc(op.x);
      debug(
          "SpmvOperator::SpmvOperator(copy_op,start,end) -> Copied x vector\n");
      break;
    case (SPMV_MEMTYPE_DEVICE):
      massert(0,
              "SpmvOperator::SpmvOperator(copy_op,start,end) -> Copy from "
              "device vector not "
              "supported\n");
      break;
    default:
      massert(0,
              "SpmvOperator::SpmvOperator(copy_op,start,end) -> Unreachable "
              "mem_alloc "
              "default reached\n");
      break;
  }
  if (mode == 0)
    n = ((SpmvCooData *)format_data)->rowInd[nz - 1] + 1;
  else if (mode == 1)
    n = m - ((SpmvCooData *)op.format_data)->rowInd[op.nz - nz];

  bytes = sizeof(int) * (m + 1) + sizeof(int) * nz + sizeof(VALUE_TYPE_AX) * nz +
          sizeof(VALUE_TYPE_Y) * m + sizeof(VALUE_TYPE_AX) * n;
  /// FLOPS
  flops = 2 * nz;

  ddebug(" <- SpmvOperator::SpmvOperator(copy_op,start,end)\n");
}

SpmvOperator::~SpmvOperator() {
  ddebug(" -> SpmvOperator::~SpmvOperator()\n");
  vec_free(x, n * sizeof(VALUE_TYPE_AX), mem_alloc);
  vec_free(y, m * sizeof(VALUE_TYPE_Y), mem_alloc);
  spmv_free();
  free(lib_struct);
  debug("SpmvOperator::~SpmvOperator -> Operator successfully destroyed\n");
  ddebug(" <- SpmvOperator::~SpmvOperator()\n");
}

void SpmvOperator::spmv_free_host() {
  ddebug(" -> SpmvOperator::spmv_free_host()\n");
  switch (format) {
    case (SPMV_FORMAT_COO): {
      SpmvCooData *data = (SpmvCooData *)format_data;
      free(data->rowInd);
      free(data->colInd);
      free(data->values);
    } break;
    case (SPMV_FORMAT_CSR): {
      SpmvCsrData *data = (SpmvCsrData *)format_data;
      free(data->rowPtr);
      free(data->colInd);
      free(data->values);
    } break;
    case (SPMV_FORMAT_BSR): {
      SpmvBsrData *data = (SpmvBsrData *)format_data;
      free(data->rowPtr);
      free(data->colInd);
      free(data->values);
    } break;
    case (SPMV_FORMAT_HYB): {
      massert(false,
              "SpmvOperator::Spmv_free_host -> Unsupported format = "
              "SPMV_FORMAT_HYB");
    } break;
    default:
      massert(false, "SpmvOperator::Spmv_free_host -> format default reached");
      break;
  }
  ddebug(" <- SpmvOperator::spmv_free_host()\n");
}

void SpmvOperator::spmv_free_numa() {
  ddebug(" -> SpmvOperator::spmv_free_numa()\n");
	massert(false, "SpmvOperator::spmv_free_numa -> No numa please");
/*
  switch (format) {
    case (SPMV_FORMAT_COO): {
      SpmvCooData *data = (SpmvCooData *)format_data;
      vec_free(data->rowInd, nz * sizeof(int), SPMV_MEMTYPE_NUMA);
      vec_free(data->colInd, nz * sizeof(int), SPMV_MEMTYPE_NUMA);
      vec_free(data->values, nz * sizeof(double), SPMV_MEMTYPE_NUMA);
    } break;
    case (SPMV_FORMAT_CSR): {
      SpmvCsrData *data = (SpmvCsrData *)format_data;
      vec_free(data->rowPtr, (n + 1) * sizeof(int), SPMV_MEMTYPE_NUMA);
      vec_free(data->colInd, nz * sizeof(int), SPMV_MEMTYPE_NUMA);
      vec_free(data->values, nz * sizeof(double), SPMV_MEMTYPE_NUMA);
    } break;
    case (SPMV_FORMAT_BSR): {
      SpmvBsrData *data = (SpmvBsrData *)format_data;
      const int nb = (n + data->blockDim - 1) / data->blockDim;
      const int mb = (m + data->blockDim - 1) / data->blockDim;
      vec_free(data->rowPtr, (nb + 1) * sizeof(int), SPMV_MEMTYPE_NUMA);
      vec_free(data->colInd, data->nnzb * sizeof(int), SPMV_MEMTYPE_NUMA);
      vec_free(data->values,
               (data->blockDim * data->blockDim) * data->nnzb * sizeof(double),
               SPMV_MEMTYPE_NUMA);
    } break;
    case (SPMV_FORMAT_HYB): {
      massert(false,
              "SpmvOperator::spmv_free_numa -> Unsupported format = "
              "SPMV_FORMAT_HYB");
    } break;
    default:
      massert(false, "SpmvOperator::spmv_free_numa -> format default reached");
      break;
  }
*/
  ddebug(" <- SpmvOperator::spmv_free_numa()\n");
}

void SpmvOperator::spmv_free_device() {
  ddebug(" -> SpmvOperator::spmv_free_device()\n");
  switch (format) {
    case (SPMV_FORMAT_COO): {
      SpmvCooData *data = (SpmvCooData *)format_data;
      gpu_free(data->rowInd);
      gpu_free(data->colInd);
      gpu_free(data->values);
    } break;
    case (SPMV_FORMAT_CSR): {
      SpmvCsrData *data = (SpmvCsrData *)format_data;
      gpu_free(data->rowPtr);
      gpu_free(data->colInd);
      gpu_free(data->values);
    } break;
    case (SPMV_FORMAT_BSR): {
      SpmvBsrData *data = (SpmvBsrData *)format_data;
      gpu_free(data->rowPtr);
      gpu_free(data->colInd);
      gpu_free(data->values);
    } break;
    case (SPMV_FORMAT_HYB): {
	  massert(false, "SpmvOperator::Spmv_free_device -> No SPMV_FORMAT_HYB free()");
    } break;
    case (SPMV_FORMAT_CSR5): {
      SpmvCsrData *data = (SpmvCsrData *)format_data;
      gpu_free(data->rowPtr);
      gpu_free(data->colInd);
      gpu_free(data->values);
      switch (lib) {
        case (SPMV_LIBRARY_CSR5_AVX512):
	  massert(false, "SpmvOperator::Spmv_free_device -> No aCSR5_free()");
          //aCSR5_free((aCSR5_wrap *)lib_struct);
          break;
        case (SPMV_LIBRARY_CSR5_CUDA):
	  massert(false, "SpmvOperator::Spmv_free_device -> No cuCSR5_free()");
          //cuCSR5_free((cuCSR5_wrap *)lib_struct);
          break;
        default:
          massert(false,
                  "SpmvOperator::Spmv_free_device -> CSR5 lib default reached "
                  "- something is wrong");
          break;
      }

    } break;
    default:
      massert(false,
              "SpmvOperator::Spmv_free_device -> format default reached");
      break;
  }
  ddebug(" <- SpmvOperator::spmv_free_device()\n");
}

void SpmvOperator::spmv_free() {
  ddebug(" -> SpmvOperator::spmv_free()\n");
  switch (mem_alloc) {
    case (SPMV_MEMTYPE_HOST): {
      spmv_free_host();
    } break;
    case (SPMV_MEMTYPE_NUMA): {
      spmv_free_numa();
    } break;
    case (SPMV_MEMTYPE_DEVICE):
    case (SPMV_MEMTYPE_UNIFIED): {
      spmv_free_device();
    } break;
    default:
      massert(false, "SpmvOperator::Spmv_free -> mem_alloc default reached");
      break;
  }
  ddebug(" <- SpmvOperator::spmv_free()\n");
}

// FIXME: Deprecated function for spliting, must change completely
SpmvOperator **split_nz(SpmvOperator *op, int div) {
  massert(0, "split_nz -> Not implemented\n");
}
/*
SpmvOperator **split_nz(SpmvOperator *op, int div) {
        massert(op->format == SPMV_FORMAT_COO, "split_nz -> Only coo format
supported\n");
  SpmvOperator **split_nz =
      (SpmvOperator **)malloc(div * sizeof(SpmvOperator *));
  const int nze = op->nz / div;
  printf("Initializing spliting with div= %d -> nze= %d\n", div, nze);
  for (int i = 0; i < div; i++) {
    split_nz[i] = new SpmvOperator(*op);
    split_nz[i]->nz = nze;
    split_nz[i]->bytes = split_nz[i]->bytes / div;
    split_nz[i]->flops = 2 * split_nz[i]->nz;
    if (i == div - 1) {
      split_nz[i]->nz = nze + op->nz % div;
      split_nz[i]->flops = 2 * split_nz[i]->nz;
    }
    SpmvCooData *tmp_data = (SpmvCooData *)split_nz[i]->format_data;
    SpmvCooData *cp_data = (SpmvCooData *)malloc(sizeof(SpmvCooData));
    cudaMallocManaged(&cp_data->rowInd, split_nz[i]->nz * sizeof(int));
    cudaMallocManaged(&cp_data->colInd, split_nz[i]->nz * sizeof(int));
    cudaMallocManaged(&cp_data->values, split_nz[i]->nz * sizeof(double));

    vec_copy_int(cp_data->rowInd, &(((int *)tmp_data->rowInd)[nze * i]),
                 split_nz[i]->nz, 0);
    vec_copy_int(cp_data->colInd, &(((int *)tmp_data->colInd)[nze * i]),
                 split_nz[i]->nz, 0);
    /// TODO: Update this for other value_types
    vec_copy(cp_data->values, &(((double *)tmp_data->values)[nze * i]),
             split_nz[i]->nz, 0, split_nz[i]->value_type);
    //SpmvFree(split_nz[i]->format_struct, split_nz[i]->mem_alloc);
    split_nz[i]->format_data = cp_data;
  }
  return split_nz;
}
*/

void SpmvOperator::vec_alloc(VALUE_TYPE_AX *x) {
  ddebug(" -> SpmvOperator::vec_alloc(x)\n");
  switch (mem_alloc) {
    case (SPMV_MEMTYPE_HOST):
      vec_alloc_host(x);
      break;
    case (SPMV_MEMTYPE_DEVICE):
      vec_alloc_device(x);
      break;
    case (SPMV_MEMTYPE_UNIFIED):
      vec_alloc_uni(x);
      break;
    case (SPMV_MEMTYPE_NUMA):
      vec_alloc_numa(x);
      break;
    default:
      massert(0,
              "SpmvOperator::vec_alloc -> Unreachable mem_alloc default "
              "reached");
      break;
  }
  ddebug(" <- SpmvOperator::vec_alloc(x)\n");
}

void SpmvOperator::vec_alloc_numa(VALUE_TYPE_AX *x_in) {
  ddebug(" -> SpmvOperator::vec_alloc_numa(x_in)\n");
        massert(false, "SpmvOperator::vec_alloc_numa -> No numa please");
  ddebug(" <- SpmvOperator::vec_alloc_numa(x_in)\n");
}

void SpmvOperator::vec_alloc_host(VALUE_TYPE_AX *x_in) {
  ddebug(" -> SpmvOperator::vec_alloc_host(x_in)\n");
  VALUE_TYPE_AX *x_tmp;
  VALUE_TYPE_Y *y_tmp;

      /// Allocate vectors x,y
      cudaHostAlloc(&x_tmp, n * sizeof(VALUE_TYPE_AX), cudaHostAllocDefault);
      cudaHostAlloc(&y_tmp, m * sizeof(VALUE_TYPE_Y), cudaHostAllocDefault);
      /// Initialize vector x to x_in (y was calloc'ed)
      vec_copy<VALUE_TYPE_AX>(x_tmp, x_in, n, 0);
      for (int i = 0; i < m; i++) y_tmp[i] = 0;


  massert(x_tmp && y_tmp,
          "SpmvOperator::vec_alloc_host -> Vector Alloc failed");

  /// Free previous vectors
  if (x && y) {
    debug(
        "SpmvOperator::vec_alloc_host -> previous x and y allocated, "
        "deleting...\n");
    vec_free(x, n * sizeof(VALUE_TYPE_AX), mem_alloc);
    vec_free(y, m * sizeof(VALUE_TYPE_Y), mem_alloc);
  } else mem_bytes += n * sizeof(VALUE_TYPE_AX) + m * sizeof(VALUE_TYPE_Y);


  x = x_tmp;
  y = y_tmp;
  ddebug(" <- SpmvOperator::vec_alloc_host(x_in)\n");
}

void SpmvOperator::vec_alloc_uni(VALUE_TYPE_AX *x_in) {
  ddebug(" -> SpmvOperator::vec_alloc_uni(x)\n");
  VALUE_TYPE_AX *x_tmp;
  VALUE_TYPE_Y *y_tmp;

  /// Allocate vectors x,y
  cudaMallocManaged(&x_tmp, n * sizeof(VALUE_TYPE_AX));
  cudaMallocManaged(&y_tmp, m * sizeof(VALUE_TYPE_Y));
  /// Initialize vectors (x,y) to (x_in,0)
  vec_copy<VALUE_TYPE_AX>(x_tmp, x_in, n, 0);
  for (int i = 0; i < m; i++) y_tmp[i] = 0;


  massert(x_tmp && y_tmp, "SpmvOperator::vec_alloc_uni -> Vector Alloc failed");

  /// Free previous vectors
  if (x && y) {
    debug(
        "SpmvOperator::vec_alloc_uni -> previous x and y allocated, "
        "deleting...\n");
    vec_free(x, n * sizeof(VALUE_TYPE_AX), mem_alloc);
    vec_free(y, m * sizeof(VALUE_TYPE_Y), mem_alloc);

  } else {
        mem_bytes += n * sizeof(VALUE_TYPE_AX) + m * sizeof(VALUE_TYPE_Y);
        gpu_mem_bytes += n * sizeof(VALUE_TYPE_AX) + m * sizeof(VALUE_TYPE_Y);

  }
  x = x_tmp;
  y = y_tmp;
  ddebug(" <- SpmvOperator::vec_alloc_uni(x)\n");
}

void SpmvOperator::vec_alloc_device(VALUE_TYPE_AX *x_in) {
  ddebug(" -> SpmvOperator::vec_alloc_device(x)\n");
  VALUE_TYPE_AX *x_tmp;
  VALUE_TYPE_Y *y_tmp;

	  /// Allocate vectors x,y
	  x_tmp = (VALUE_TYPE_AX *)gpu_alloc(n * sizeof(VALUE_TYPE_AX));
	  y_tmp = (VALUE_TYPE_Y *)gpu_alloc(m * sizeof(VALUE_TYPE_Y));
	  /// Initialize vectors (x,y) to (x_in,0)
	  copy_to_gpu(x_in, x_tmp, n * sizeof(VALUE_TYPE_AX));
	  cudaMemset(y_tmp, 0, m * sizeof(VALUE_TYPE_Y));

  cudaCheckErrors();

  /// Free previous vectors
  if (x && y) {
    debug(
        "SpmvOperator::vec_alloc_device -> previous x and y allocated, "
        "deleting...");
    vec_free(x, n * sizeof(VALUE_TYPE_AX), mem_alloc);
    vec_free(y, m * sizeof(VALUE_TYPE_Y), mem_alloc);
  } else gpu_mem_bytes += n * sizeof(VALUE_TYPE_AX) + m * sizeof(VALUE_TYPE_Y);
  x = x_tmp;
  y = y_tmp;
  ddebug(" <- SpmvOperator::vec_alloc_device(x)\n");
}

void *SpmvOperator::spmv_data_get_copy() {
  ddebug(" -> SpmvOperator::spmv_data_get_copy()\n");
  switch (mem_alloc) {
    case (SPMV_MEMTYPE_HOST):
      return spmv_data_copy_host();
    case (SPMV_MEMTYPE_DEVICE):
      return spmv_data_copy_device();
    case (SPMV_MEMTYPE_UNIFIED):
      return spmv_data_copy_uni();
    default:
      massert(0,
              "SpmvOperator::spmv_data_copy -> Unreachable mem_alloc "
              "default reached");
      break;
  }
  return NULL;
}

void *SpmvOperator::spmv_data_copy_uni() {
  ddebug(" -> SpmvOperator::spmv_data_get_copy_uni()\n");
  switch (format) {
    case (SPMV_FORMAT_COO): {
      SpmvCooData *data = (SpmvCooData *)format_data;
      SpmvCooData *cp_data = (SpmvCooData *)malloc(sizeof(SpmvCooData));
      if (data->rowInd && data->colInd && data->values) {
        cudaMallocManaged(&cp_data->rowInd, nz * sizeof(int));
        cudaMallocManaged(&cp_data->colInd, nz * sizeof(int));

        vec_copy<int>(cp_data->rowInd, data->rowInd, nz, 0);
        vec_copy<int>(cp_data->colInd, data->colInd, nz, 0);

        cudaMallocManaged(&cp_data->values, nz * sizeof(VALUE_TYPE_AX));
        vec_copy<VALUE_TYPE_AX>(cp_data->values, data->values, nz,0);
      } else
        debug(
            "SpmvOperator::spmv_data_copy_uni -> warning... empty Spmv struct, "
            "copied nothing\n");
      ddebug(" <- SpmvOperator::spmv_data_get_copy_uni()\n");
      return cp_data;
    }
    case (SPMV_FORMAT_CSR): {
      SpmvCsrData *data = (SpmvCsrData *)format_data;
      SpmvCsrData *cp_data = (SpmvCsrData *)malloc(sizeof(SpmvCsrData));
      if (data->rowPtr && data->colInd && data->values) {
        cudaMallocManaged(&cp_data->rowPtr, (m + 1) * sizeof(int));
        cudaMallocManaged(&cp_data->colInd, nz * sizeof(int));

        vec_copy<int>(cp_data->rowPtr, data->rowPtr, m + 1, 0);
        vec_copy<int>(cp_data->colInd, data->colInd, nz, 0);

        cudaMallocManaged(&cp_data->values, nz * sizeof(VALUE_TYPE_AX));
        vec_copy<VALUE_TYPE_AX>(cp_data->values, data->values, nz,0);

      } else
        debug(
            "SpmvOperator::spmv_data_copy_uni -> warning... empty Spmv struct, "
            "copied nothing\n");
      ddebug(" <- SpmvOperator::spmv_data_get_copy_uni()\n");
      return cp_data;
    }
    case (SPMV_FORMAT_BSR): {
      SpmvBsrData *data = (SpmvBsrData *)format_data;
      SpmvBsrData *cp_data = (SpmvBsrData *)malloc(sizeof(SpmvBsrData));
      const int nb = (n + data->blockDim - 1) / data->blockDim;
      const int mb = (m + data->blockDim - 1) / data->blockDim;
      if (data->rowPtr && data->colInd && data->values) {
        cudaMallocManaged(&cp_data->rowPtr, (nb + 1) * sizeof(int));
        cudaMallocManaged(&cp_data->colInd, data->nnzb * sizeof(int));

        vec_copy<int>(cp_data->rowPtr, data->rowPtr, nb + 1, 0);
        vec_copy<int>(cp_data->colInd, data->colInd, data->nnzb, 0);

        cudaMallocManaged(&cp_data->values,(data->blockDim * data->blockDim) * data->nnzb * sizeof(VALUE_TYPE_AX));
        vec_copy<VALUE_TYPE_AX>(cp_data->values, data->values,(data->blockDim * data->blockDim) * data->nnzb, 0);


        cp_data->nnzb = data->nnzb;
        cp_data->blockDim = data->blockDim;
      } else
        debug(
            "SpmvOperator::spmv_data_copy_uni -> warning... empty Spmv struct, "
            "copied nothing\n");
      ddebug(" <- SpmvOperator::spmv_data_get_copy_uni()\n");
      return cp_data;
    }
    case (SPMV_FORMAT_DIA):
      massert(0,
              "SpmvOperator::spmv_data_copy_uni -> SPMV_FORMAT_DIA not "
              "implemented");
    case (SPMV_FORMAT_ELL):
      massert(0,
              "SpmvOperator::spmv_data_copy_uni -> SPMV_FORMAT_ELL not "
              "implemented");
    case (SPMV_FORMAT_HYB):
      massert(0,
              "SpmvOperator::spmv_data_copy_uni -> SPMV_FORMAT_HYB not "
              "implemented");
    default:
      massert(0,
              "SpmvOperator::spmv_data_copy_uni -> Unreacheable format default "
              "reached");
      break;
  }
  ddebug(" <- SpmvOperator::spmv_data_get_copy_uni()\n");
  return NULL;
}

void *SpmvOperator::spmv_data_copy_device() {
  ddebug(" -> SpmvOperator::spmv_data_get_copy_device()\n");
  massert(0, "SpmvOperator::spmv_data_copy_device -> Not implemented");
  ddebug(" <- SpmvOperator::spmv_data_get_copy_device()\n");
  return NULL;
}

void *SpmvOperator::spmv_data_get_subcopy(int *start, int *nzc, int mode) {
  ddebug(" -> SpmvOperator::spmv_data_get_subcopy()\n");
  switch (mem_alloc) {
    case (SPMV_MEMTYPE_HOST):
      return spmv_data_subcopy_host(start, nzc, mode);
    case (SPMV_MEMTYPE_DEVICE):
      return spmv_data_subcopy_device(start, nzc, mode);
    case (SPMV_MEMTYPE_UNIFIED):
      return spmv_data_subcopy_uni(start, nzc, mode);
    default:
      massert(0,
              "SpmvOperator::spmv_data_subcopy -> Unreachable mem_alloc "
              "default reached");
      break;
  }
  return NULL;
}

void *SpmvOperator::spmv_data_subcopy_uni(int *start, int *nzc, int mode) {
  ddebug(" -> SpmvOperator::spmv_data_subcopy_uni()\n");
  switch (format) {
    case (SPMV_FORMAT_COO): {
      SpmvCooData *data = (SpmvCooData *)format_data;
      SpmvCooData *cp_data = (SpmvCooData *)malloc(sizeof(SpmvCooData));
      if (data->rowInd && data->colInd && data->values) {
        if (mode == 0)
          while (data->rowInd[*nzc - 1] == data->rowInd[*nzc]) (*nzc)++;
        else if (mode == 1)
          while (data->rowInd[*start - 1] == data->rowInd[*start]) {
            (*start)++;
            (*nzc)--;
          }
        else
          massert(false,
                  "SpmvOperator::spmv_data_subcopy_uni -> unsupported subcopy "
                  "mode");
        cudaMallocManaged(&cp_data->rowInd, *nzc * sizeof(int));
        cudaMallocManaged(&cp_data->colInd, *nzc * sizeof(int));

        vec_copy<int>(cp_data->rowInd, &(data->rowInd[*start]), *nzc, 0);
        vec_copy<int>(cp_data->colInd, &(data->colInd[*start]), *nzc, 0);

        if (mode == 1) {
          for (int i = 1; i < *nzc; i++)
            cp_data->rowInd[i] = cp_data->rowInd[i] - cp_data->rowInd[0];
          cp_data->rowInd[0] = 0;
        }
        cudaMallocManaged(&cp_data->values, *nzc * sizeof(VALUE_TYPE_AX));
        vec_copy<VALUE_TYPE_AX>(cp_data->values,&(data->values)[*start], *nzc, 0);


      } else
        debug(
            "SpmvOperator::spmv_data_subcopy_uni -> warning... empty Spmv "
            "struct, "
            "copied nothing\n");
      ddebug(" <- SpmvOperator::spmv_data_subcopy_uni()\n");
      return cp_data;
    }
    case (SPMV_FORMAT_CSR):
      massert(0,
              "SpmvOperator::spmv_data_subcopy_uni -> SPMV_FORMAT_DIA not "
              "implemented");
    case (SPMV_FORMAT_BSR):
      massert(0,
              "SpmvOperator::spmv_data_subcopy_uni -> SPMV_FORMAT_DIA not "
              "implemented");
    case (SPMV_FORMAT_DIA):
      massert(0,
              "SpmvOperator::spmv_data_subcopy_uni -> SPMV_FORMAT_DIA not "
              "implemented");
    case (SPMV_FORMAT_ELL):
      massert(0,
              "SpmvOperator::spmv_data_subcopy_uni -> SPMV_FORMAT_ELL not "
              "implemented");
    case (SPMV_FORMAT_HYB):
      massert(0,
              "SpmvOperator::spmv_data_subcopy_uni -> SPMV_FORMAT_HYB not "
              "implemented");
    default:
      massert(
          0,
          "SpmvOperator::spmv_data_subcopy_uni -> Unreacheable format default "
          "reached");
      break;
  }
  ddebug(" <- SpmvOperator::spmv_data_subcopy_uni()\n");
  return NULL;
}

void *SpmvOperator::spmv_data_subcopy_device(int *start, int *nzc, int mode) {
  ddebug(" -> SpmvOperator::spmv_data_subcopy_device()\n");
  massert(0, "SpmvOperator::spmv_data_subcopy_device -> Not implemented");
  ddebug(" <- SpmvOperator::spmv_data_subcopy_device()\n");
  return NULL;
}

void SpmvOperator::mem_convert(SpmvMemType target_mem) {
  ddebug(" -> SpmvOperator::mem_convert(target_mem)\n");
  switch (target_mem) {
    case (SPMV_MEMTYPE_HOST):
      mem_convert_host();
      break;
    case (SPMV_MEMTYPE_DEVICE):
      mem_convert_device();
      break;
    case (SPMV_MEMTYPE_UNIFIED):
      mem_convert_uni();
      break;
    case (SPMV_MEMTYPE_NUMA):
      mem_convert_numa();
      break;
    default:
      massert(0,
              "SpmvOperator::mem_convert -> Unreachable mem_alloc default "
              "reached");
      break;
  }
  ddebug(" <- SpmvOperator::mem_convert(target_mem)\n");
}

void SpmvOperator::mem_convert_uni() {
  ddebug(" -> SpmvOperator::mem_convert_uni()\n");
  void *newptr = NULL;
  switch (mem_alloc) {
    case (SPMV_MEMTYPE_HOST):
      newptr = spmv_data_copy_uni();
      spmv_free_host();
      format_data = newptr;
      vec_alloc_uni(x);
      break;
    case (SPMV_MEMTYPE_UNIFIED):
      debug(
          "SpmvOperator::mem_convert_uni -> Data already in unified memory\n");
      break;
    case (SPMV_MEMTYPE_DEVICE):
      debug(
          "SpmvOperator::mem_convert_uni -> warning... "
          "SpmvOperator::spmv_data_copy_uni from device is not properly "
          "tested\n");
      newptr = spmv_data_copy_uni();
      spmv_free_device();
      format_data = newptr;
      vec_alloc_uni(x);
      break;
    default:
      massert(0,
              "SpmvOperator::mem_convert_uni -> Unreachable mem_alloc default "
              "reached");
      break;
  }
  mem_alloc = SPMV_MEMTYPE_UNIFIED;
  ddebug(" <- SpmvOperator::mem_convert_uni()\n");
}

void SpmvOperator::mem_convert_host() {
  ddebug(" -> SpmvOperator::mem_convert_host()\n");
  void *newptr = NULL;
  switch (mem_alloc) {
    case (SPMV_MEMTYPE_HOST):
      debug("SpmvOperator::mem_convert_host -> Data already in host memory\n");
      break;
    case (SPMV_MEMTYPE_UNIFIED):
      newptr = spmv_data_copy_host();
      spmv_free_device();
      format_data = newptr;
      vec_alloc_host(x);
      break;
    case (SPMV_MEMTYPE_DEVICE):
      debug(
          "SpmvOperator::mem_convert_host -> warning... "
          "SpmvOperator::spmv_data_copy_uni from device is not properly "
          "tested\n");
      /// Convert to unified from device
      newptr = spmv_data_copy_uni();
      spmv_free_device();
      format_data = newptr;
      vec_alloc_uni(x);
      /// Convert to host from unified
      newptr = spmv_data_copy_host();
      spmv_free_device();
      format_data = newptr;
      vec_alloc_host(x);
      break;
    default:
      massert(0,
              "SpmvOperator::mem_convert_host -> Unreachable mem_alloc default "
              "reached");
      break;
  }
  mem_alloc = SPMV_MEMTYPE_HOST;
  ddebug(" <- SpmvOperator::mem_convert_host()\n");
}

void SpmvOperator::mem_convert_numa() {
  ddebug(" -> SpmvOperator::mem_convert_numa()\n");
  void *newptr = NULL;
  switch (mem_alloc) {
    case (SPMV_MEMTYPE_NUMA):
      debug("SpmvOperator::mem_convert_numa -> Data already in numa memory\n");
      break;
    case (SPMV_MEMTYPE_HOST):
      newptr = spmv_data_copy_numa();
      spmv_free_host();
      format_data = newptr;
      vec_alloc_numa(x);
      break;
    case (SPMV_MEMTYPE_UNIFIED):
      newptr = spmv_data_copy_numa();
      spmv_free_device();
      format_data = newptr;
      vec_alloc_numa(x);
      break;
    case (SPMV_MEMTYPE_DEVICE):
      debug(
          "SpmvOperator::mem_convert_numa -> warning... "
          "SpmvOperator::spmv_data_copy_uni from device is not properly "
          "tested\n");
      /// Convert to unified from device
      newptr = spmv_data_copy_uni();
      spmv_free_device();
      format_data = newptr;
      vec_alloc_uni(x);
      /// Convert to host from unified
      newptr = spmv_data_copy_numa();
      spmv_free_device();
      format_data = newptr;
      vec_alloc_numa(x);
      break;
    default:
      massert(0,
              "SpmvOperator::mem_convert_numa -> Unreachable mem_alloc default "
              "reached");
      break;
  }
  mem_alloc = SPMV_MEMTYPE_NUMA;
  ddebug(" <- SpmvOperator::mem_convert_numa()\n");
}

void SpmvOperator::mem_convert_device() {
  ddebug(" -> SpmvOperator::mem_convert_device()\n");
  void *newptr = NULL;
  switch (mem_alloc) {
    case (SPMV_MEMTYPE_DEVICE):
      debug(
          "SpmvOperator::mem_convert_device -> Data already in device memory");
      break;
    case (SPMV_MEMTYPE_UNIFIED):
      newptr = spmv_data_copy_device();
      spmv_free_device();
      format_data = newptr;
      vec_alloc_device(x);
    case (SPMV_MEMTYPE_HOST):
      /// Convert to unified from host
      newptr = spmv_data_copy_uni();
      spmv_free_device();
      format_data = newptr;
      vec_alloc_uni(x);
      /// Convert to device from unified
      debug(
          "SpmvOperator::mem_convert_device -> warning... "
          "SpmvOperator::spmv_data_copy_device from unified is not properly "
          "tested");
      newptr = spmv_data_copy_device();
      spmv_free_device();
      format_data = newptr;
      vec_alloc_device(x);
      break;
    default:
      massert(0,
              "SpmvOperator::mem_convert_device -> Unreachable mem_alloc "
              "default reached");
      break;
  }
  mem_alloc = SPMV_MEMTYPE_DEVICE;
  ddebug(" <- SpmvOperator::mem_convert_device()\n");
}

void SpmvOperator::format_convert(SpmvFormat target_format) {
  ddebug(" -> SpmvOperator::format_convert(target_format)\n");
  switch (mem_alloc) {
    case (SPMV_MEMTYPE_HOST):
      format_convert_host(target_format);
      break;
    case (SPMV_MEMTYPE_DEVICE):
      format_convert_device(target_format);
      break;
    case (SPMV_MEMTYPE_UNIFIED):
      format_convert_uni(target_format);
      break;
    default:
      massert(0,
              "SpmvOperator::format_convert_type -> Unreachable mem_alloc "
              "default reached");
      break;
  }
  ddebug(" <- SpmvOperator::format_convert(target_format)\n");
}

void SpmvOperator::format_convert_uni(SpmvFormat target_format) {
  ddebug(" -> SpmvOperator::format_convert_uni(target_format)\n");
  switch (target_format) {
    case (SPMV_FORMAT_COO): {
      switch (format) {
        case (SPMV_FORMAT_COO):
          debug(
              "SpmvOperator::format_convert_uni -> Struct already in the "
              "correct format\n");
          break;
        case (SPMV_FORMAT_CSR):
          format_convert_uni_csr2coo();
          break;
        case (SPMV_FORMAT_BSR):
          format_convert_uni_bsr2csr();
          format_convert_uni_csr2coo();
          break;
        case (SPMV_FORMAT_DIA):
          massert(0,
                  "SpmvOperator::format_convert_uni -> SPMV_FORMAT_DIA "
                  "source not supported");
          break;
        case (SPMV_FORMAT_ELL):
          massert(0,
                  "SpmvOperator::format_convert_uni -> SPMV_FORMAT_ELL source "
                  "not supported");
          break;
        case (SPMV_FORMAT_HYB):
          format_convert_uni_hyb2csr();
          format_convert_uni_csr2coo();
          break;
        default:
          massert(0,
                  "SpmvOperator::format_convert_uni -> Unreacheable source "
                  "format default reached");
          break;
      }
    } break;
    case (SPMV_FORMAT_CSR): {
      switch (format) {
        case (SPMV_FORMAT_COO):
          format_convert_uni_coo2csr();
          break;
        case (SPMV_FORMAT_CSR):
          debug(
              "SpmvOperator::format_convert_uni -> Struct already in the "
              "correct format");
          break;
        case (SPMV_FORMAT_BSR):
          format_convert_uni_bsr2csr();
          break;
        case (SPMV_FORMAT_DIA):
          massert(0,
                  "SpmvOperator::format_convert_uni -> SPMV_FORMAT_DIA "
                  "source not supported");
          break;
        case (SPMV_FORMAT_ELL):
          massert(0,
                  "SpmvOperator::format_convert_uni -> SPMV_FORMAT_ELL source "
                  "not supported");
          break;
        case (SPMV_FORMAT_HYB):
          format_convert_uni_hyb2csr();
          break;
        default:
          massert(0,
                  "SpmvOperator::format_convert_uni -> Unreacheable source "
                  "format default reached");
          break;
      }
    } break;
    case (SPMV_FORMAT_BSR): {
      switch (format) {
        case (SPMV_FORMAT_COO):
          format_convert_uni_coo2csr();
          format_convert_uni_csr2bsr();
          break;
        case (SPMV_FORMAT_CSR):
          format_convert_uni_csr2bsr();
          break;
        case (SPMV_FORMAT_BSR):
          if (bsr_blockDim != ((SpmvBsrData *)format_data)->blockDim)
            format_convert_uni_bsr2bsr();
          else
            debug(
                "SpmvOperator::format_convert_uni -> Already in the correct "
                "bsr format");
          break;
        case (SPMV_FORMAT_DIA):
          massert(0,
                  "SpmvOperator::format_convert_uni -> SPMV_FORMAT_DIA "
                  "source not supported");
          break;
        case (SPMV_FORMAT_ELL):
          massert(0,
                  "SpmvOperator::format_convert_uni -> SPMV_FORMAT_ELL source "
                  "not supported");
          break;
        case (SPMV_FORMAT_HYB):
          format_convert_uni_hyb2csr();
          format_convert_uni_csr2bsr();
          break;
        default:
          massert(0,
                  "SpmvOperator::format_convert_uni -> Unreacheable source "
                  "format default reached");
          break;
      }
    } break;
    case (SPMV_FORMAT_DIA):
      massert(0,
              "SpmvOperator::format_convert_uni -> SPMV_FORMAT_DIA target not "
              "supported");
      break;
    case (SPMV_FORMAT_ELL):
      massert(0,
              "SpmvOperator::format_convert_uni -> SPMV_FORMAT_ELL target not "
              "supported");
      break;
    case (SPMV_FORMAT_HYB): {
      switch (format) {
        case (SPMV_FORMAT_COO):
          format_convert_uni_coo2csr();
          format_convert_uni_csr2hyb();
          break;
        case (SPMV_FORMAT_CSR):
          format_convert_uni_csr2hyb();
          break;
        case (SPMV_FORMAT_BSR):
          format_convert_uni_bsr2csr();
          format_convert_uni_csr2hyb();
        case (SPMV_FORMAT_DIA):
          massert(0,
                  "SpmvOperator::format_convert_uni -> SPMV_FORMAT_DIA "
                  "source not supported");
          break;
        case (SPMV_FORMAT_ELL):
          massert(0,
                  "SpmvOperator::format_convert_uni -> SPMV_FORMAT_ELL source "
                  "not supported");
          break;
        case (SPMV_FORMAT_HYB):
          debug(
              "SpmvOperator::format_convert_uni -> Struct already in the "
              "correct format");
          break;
        default:
          massert(0,
                  "SpmvOperator::format_convert_uni -> Unreacheable source "
                  "format default reached");
          break;
      }
    } break;
    default:
      massert(0,
              "SpmvOperator::format_convert_uni -> Unreacheable target format "
              "default reached");
      break;
  }
  ddebug(" <- SpmvOperator::format_convert_uni(target_format)\n");
}

void SpmvOperator::format_convert_device(SpmvFormat target_format) {
  ddebug(" -> SpmvOperator::format_convert_device(target_format)\n");
  massert(0, "SpmvOperator::format_convert_device -> Not Implemented");
  ddebug(" <- SpmvOperator::format_convert_device(target_format)\n");
}

VALUE_TYPE_Y *SpmvOperator::y_get_copy() {
  ddebug(" -> SpmvOperator::y_get_copy()\n");
  VALUE_TYPE_Y *out;
  switch (mem_alloc) {
    case (SPMV_MEMTYPE_HOST):
    case (SPMV_MEMTYPE_NUMA):
    case (SPMV_MEMTYPE_UNIFIED):
		out = (VALUE_TYPE_Y *) malloc(m * sizeof(VALUE_TYPE_Y));
		vec_copy<VALUE_TYPE_Y>(out, y, m, 0);
		break;
    case (SPMV_MEMTYPE_DEVICE):
      out = (VALUE_TYPE_Y *)malloc(m * sizeof(VALUE_TYPE_Y));
      copy_from_gpu(out, y, m * sizeof(VALUE_TYPE_Y));
      cudaCheckErrors();
      break;
    default:
      massert(0, "SpmvOperator::y_get_copy op->mem_alloc type unsupported");
  }
  ddebug(" <- SpmvOperator::y_get_copy()\n");
  return out;
}

void SpmvOperator::free_lib_struct() {
  ddebug(" -> SpmvOperator::free_lib_struct()\n");
  switch (lib) {
    case (SPMV_NONE):
      debug(
          "SpmvOperator::free_lib_struct -> Tried to free SPMV_NONE "
          "lib_struct");
      break;
    case (SPMV_LIBRARY_CUSPARSE):
      cuSPARSE_free((cuSPARSE_wrap *)lib_struct);
      break;
    default:
      massert(
          0,
          "SpmvOperator::free_lib_struct -> lib unreachable default reached");
  }
  ddebug(" <- SpmvOperator::free_lib_struct()\n");
}

int SpmvOperator::count_transactions() {
  ddebug(" -> SpmvOperator::count_transactions()\n");
  massert(format_data != NULL, "count_transactions -> No format struct");
  int ctr = 0, *exists = (int *)calloc(n, sizeof(int));
  switch (format) {
    case (SPMV_FORMAT_COO): {
      SpmvCooData *data = (SpmvCooData *)format_data;
      for (int i = 0; i < nz; i++) exists[data->colInd[i]] = 1;
    } break;
    case (SPMV_FORMAT_CSR): {
      SpmvCsrData *data = (SpmvCsrData *)format_data;
      for (int i = 0; i < nz; i++) exists[data->colInd[i]] = 1;
    } break;
    default:
      free(exists);
      massert(false, "count_transactions -> Unsupported SpMV format");
      break;
  }
  for (int i = 0; i < n; i++)
    if (exists[i]) ctr++;
  //free(exists);
  ddebug(" <- SpmvOperator::count_transactions()\n");
  return ctr;
}
