///
/// \author Anastasiadis Petros (panastas@cslab.ece.ntua.gr)
///
/// \brief A wrapper for the cuSP library for SpMV
///

#include "cuSP.hpp"

#include <cmath>
#include <iostream>
#include <limits>
#include <map>
#include <string>

#include <unistd.h>
#include "gpu_utils.hpp"

cuSP_wrap *cuSP_desc() {
  cuSP_wrap *wrapper = (cuSP_wrap *)malloc(sizeof(cuSP_wrap));
  wrapper->flag = 1;
  return wrapper;
}

void SpmvOperator::cuSP_check_compatibility() {
  print_op();
  if (0) massert(0, "cuSP_check_compatibility -> SpmvOperator not compatible");
}

void SpmvOperator::cuSP_init() {
  cuSP_check_compatibility();
  free_lib_struct();
  lib_struct = cuSP_desc();
  lib = SPMV_LIBRARY_CUSP;
  mem_convert(SPMV_MEMTYPE_CUSP);
  print_op();
}

void cuSP_free(cuSP_wrap *tmp) { free(tmp); }

void SpmvOperator::vec_alloc_cuSP(void *x) {
  switch (value_type) {
    case (SPMV_VALUETYPE_DOUBLE): {
      cusp::array1d<double, cusp::host_memory> tmp_x(m);
      cusp::array1d<double, cusp::host_memory> tmp_y(n);
      for (int i = 0; i < m; i++) {
        tmp_x[i] = ((double *)x)[i];
        tmp_y[i] = 0;
      }
      x = new cusp::array1d<double, cusp::device_memory>(tmp_x);
      y = new cusp::array1d<double, cusp::device_memory>(tmp_y);
      gpu_mem_bytes += (m + n) * sizeof(double);
    } break;
    case (SPMV_VALUETYPE_FLOAT): {
      cusp::array1d<float, cusp::host_memory> tmp_x(m);
      cusp::array1d<float, cusp::host_memory> tmp_y(n);
      for (int i = 0; i < m; i++) {
        tmp_x[i] = ((float *)x)[i];
        tmp_y[i] = 0;
      }
      x = new cusp::array1d<float, cusp::device_memory>(tmp_x);
      y = new cusp::array1d<float, cusp::device_memory>(tmp_y);
      gpu_mem_bytes += (m + n) * sizeof(float);
    } break;
    case (SPMV_VALUETYPE_INT): {
      cusp::array1d<int, cusp::host_memory> tmp_x(m);
      cusp::array1d<int, cusp::host_memory> tmp_y(n);
      for (int i = 0; i < m; i++) {
        tmp_x[i] = ((int *)x)[i];
        tmp_y[i] = 0;
      }
      x = new cusp::array1d<int, cusp::device_memory>(tmp_x);
      y = new cusp::array1d<int, cusp::device_memory>(tmp_y);
      gpu_mem_bytes += (m + n) * sizeof(int);
    } break;
    case (SPMV_VALUETYPE_BINARY):
    default:
      massert(0, "SpmvOperator::vec_alloc_cuSP: Value type not supported");
      break;
  }
  massert(x && y, "SpmvOperator::vec_alloc_cuSP: Vector Alloc failed");
}

void *SpmvOperator::spmv_data_copy_cuSP() {
  massert(0, "SpmvOperator::Spmv_data_copy_cuSP -> Not implemented");
  return NULL;
}

void *SpmvOperator::spmv_data_subcopy_cuSP(int start, int nzc) {
  ddebug(" -> SpmvOperator::spmv_data_subcopy_cuSP()\n");
  massert(0, "SpmvOperator::spmv_data_subcopy_cuSP -> Not implemented");
  ddebug(" <- SpmvOperator::spmv_data_subcopy_cuSP()\n");
  return NULL;
}

void *SpmvOperator::y_copy_cuSP() {
  void *out;
  switch (value_type) {
    case (SPMV_VALUETYPE_DOUBLE): {
      out = (double *)malloc(n * sizeof(double));
      cusp::array1d<double, cusp::host_memory> tmp_y(
          *((cusp::array1d<double, cusp::device_memory> *)y));
      for (int i = 0; i < n; i++) ((double *)out)[i] = tmp_y[i];
    } break;
    case (SPMV_VALUETYPE_FLOAT): {
      out = (float *)malloc(n * sizeof(float));
      cusp::array1d<float, cusp::host_memory> tmp_y(
          *((cusp::array1d<float, cusp::device_memory> *)y));
      for (int i = 0; i < n; i++) ((float *)out)[i] = tmp_y[i];
    } break;
    case (SPMV_VALUETYPE_INT): {
      out = (int *)malloc(n * sizeof(int));
      cusp::array1d<int, cusp::host_memory> tmp_y(
          *((cusp::array1d<int, cusp::device_memory> *)y));
      for (int i = 0; i < n; i++) ((int *)out)[i] = tmp_y[i];
    } break;
    case (SPMV_VALUETYPE_BINARY):
    default:
      massert(0, "SpmvOperator::vec_alloc_cuSP: Value type not supported");
      break;
  }

  return out;
}

void vec_free_cuSP(void *dataptr) {
  massert(0, "vec_free_cuSP -> Not implemented");
}

void SpmvOperator::spmv_free_cuSP() {
  massert(0, "SpmvOperator::spmv_free_cuSP -> Not implemented");
}

void SpmvOperator::mem_convert_cuSP() {
  massert(0, "SpmvOperator::mem_convert_cuSP -> Not implemented");
}

void SpmvOperator::format_convert_cuSP(SpmvFormat target_format) {
  massert(0, "SpmvOperator::format_convert_cuSP -> Not implemented");
}

/*

void cuSP_convert(SpmvOperator *op, SpmvFormat format) {
  massert(op->value_type == SPMV_VALUETYPE_DOUBLE,
          "cuSP_convert: unsupported SpMV value datatype");
  cuSP_wrap *wrapper = (cuSP_wrap *)op->util->lib_struct;

  cusp::coo_matrix<int, double, cusp::host_memory> tmp(op->size->n, op->size->m,
                                                       op->size->nz);
  switch (op->mem_alloc) {
    case (SPMV_MEMTYPE_HOST):
    case (SPMV_MEMTYPE_UNIFIED):
      massert(op->format_struct->format == SPMV_FORMAT_COO,
              "cuSP_convert: unsupported SpMV format conversion");
      for (int i = 0; i < op->size->nz; i++) {
        tmp.values[i] =
            ((double *)((SpmvCooData *)op->format_struct->data)->values)[i];
        tmp.row_indices[i] =
            ((SpmvCooData *)op->format_struct->data)->rowInd[i];
        tmp.column_indices[i] =
            ((SpmvCooData *)op->format_struct->data)->colInd[i];
      }
      break;
    case (SPMV_MEMTYPE_CUSP): {
      if (op->format_struct->format == format) {
        debug(
            "cuSP_convert: Spmv struct already in the correct mem_alloc and "
            "format\n");
        return;
      }
      switch (op->format_struct->format) {
        case (SPMV_FORMAT_COO):
          tmp = *(cusp::coo_matrix<int, double, cusp::device_memory> *)
                     op->format_struct->data;
          break;
        case (SPMV_FORMAT_CSR):
          tmp = *(cusp::csr_matrix<int, double, cusp::device_memory> *)
                     op->format_struct->data;
          break;
        case (SPMV_FORMAT_DIA):
          tmp = *(cusp::dia_matrix<int, double, cusp::device_memory> *)
                     op->format_struct->data;
          break;
        case (SPMV_FORMAT_ELL):
          tmp = *(cusp::ell_matrix<int, double, cusp::device_memory> *)
                     op->format_struct->data;
          break;
        case (SPMV_FORMAT_HYB):
          tmp = *(cusp::hyb_matrix<int, double, cusp::device_memory> *)
                     op->format_struct->data;
          break;
        default:
          massert(false, "cuSP_convert: Unsupported format");
          break;
      }
    } break;
    default:
      massert(false, "cuSP_convert: Unsupported memory allocation");
      break;
  }

  /// SpmvFree(op->format_struct, op->mem_alloc);

  /*
  for (int i = 0; i < op->size->nz; i++) printf("%d ", tmp.row_indices[i]);
  printf("\n");
  for (int i = 0; i < op->size->nz; i++) printf("%d ", tmp.column_indices[i]);
  printf("\n");
  for (int i = 0; i < op->size->nz; i++) printf("%lf ", tmp.values[i]);
  printf("\n");

  switch (format) {
    case (SPMV_FORMAT_COO): {
      op->format_struct->data =
          new cusp::coo_matrix<int, double, cusp::device_memory>(tmp);
      wrapper->flag = 1;
    } break;
    case (SPMV_FORMAT_CSR): {
      op->format_struct->data =
          new cusp::csr_matrix<int, double, cusp::device_memory>(tmp);
      wrapper->flag = 1;
    } break;
    case (SPMV_FORMAT_DIA): {
      try {
        op->format_struct->data =
            new cusp::dia_matrix<int, double, cusp::device_memory>(tmp);
      } catch (cusp::format_conversion_exception) {
        wrapper->flag = 0;
        debug("cuSP_convert: Refusing to convert to DIA format\n");
        return;
      }
    } break;
    case (SPMV_FORMAT_ELL): {
      try {
        op->format_struct->data =
            new cusp::ell_matrix<int, double, cusp::device_memory>(tmp);
      } catch (cusp::format_conversion_exception) {
        wrapper->flag = 0;
        debug("cuSP_convert: Refusing to convert to ELL format\n");
        return;
      }
    } break;
    case (SPMV_FORMAT_HYB): {
      op->format_struct->data =
          new cusp::hyb_matrix<int, double, cusp::device_memory>(tmp);
      wrapper->flag = 1;
    } break;
    default:
      massert(false, "cuSP_convert: Unsupported format");
      break;
  }
  op->format_struct->format = format;
}

void cuSP_coo(SpmvOperator *op) {
  cusp::multiply(
      *((cusp::coo_matrix<int, double, cusp::device_memory> *)
            op->format_struct->data),
      *((cusp::array1d<double, cusp::device_memory> *)op->vectors->x),
      *((cusp::array1d<double, cusp::device_memory> *)op->vectors->y));
}

void cuSP_csr_vector(SpmvOperator *op) {
  cusp::multiply(
      *((cusp::csr_matrix<int, double, cusp::device_memory> *)
            op->format_struct->data),
      *((cusp::array1d<double, cusp::device_memory> *)op->vectors->x),
      *((cusp::array1d<double, cusp::device_memory> *)op->vectors->y));
}

void cuSP_csr_scalar(SpmvOperator *op) {
  cusp::system::cuda::detail::spmv_csr_scalar(
      *((cusp::csr_matrix<int, double, cusp::device_memory> *)
            op->format_struct->data),
      *((cusp::array1d<double, cusp::device_memory> *)op->vectors->x),
      *((cusp::array1d<double, cusp::device_memory> *)op->vectors->y));
}

void cuSP_dia(SpmvOperator *op) {
  cusp::multiply(
      *((cusp::dia_matrix<int, double, cusp::device_memory> *)
            op->format_struct->data),
      *((cusp::array1d<double, cusp::device_memory> *)op->vectors->x),
      *((cusp::array1d<double, cusp::device_memory> *)op->vectors->y));
}

void cuSP_ell(SpmvOperator *op) {
  cusp::multiply(
      *((cusp::ell_matrix<int, double, cusp::device_memory> *)
            op->format_struct->data),
      *((cusp::array1d<double, cusp::device_memory> *)op->vectors->x),
      *((cusp::array1d<double, cusp::device_memory> *)op->vectors->y));
}

void cuSP_hyb(SpmvOperator *op) {
  cusp::multiply(
      *((cusp::hyb_matrix<int, double, cusp::device_memory> *)
            op->format_struct->data),
      *((cusp::array1d<double, cusp::device_memory> *)op->vectors->x),
      *((cusp::array1d<double, cusp::device_memory> *)op->vectors->y));
}
*/
