///
/// \author Anastasiadis Petros (panastas@cslab.ece.ntua.gr)
///
/// \brief Nvidia CuSPARSE wrapper for benchmarking
///

#include <cstdio>
#include "cuSPARSE.hpp"

cuSPARSE_wrap *cuSPARSE_desc() {
  ddebug(" -> cuSPARSE_desc()\n");
  cuSPARSE_wrap *tmp = (cuSPARSE_wrap *)malloc(sizeof(cuSPARSE_wrap));
  tmp->target_mem = SPMV_MEMTYPE_HOST;
  int double_device = -1, unified_device = -1, nDevices = 0;
  cudaGetDeviceCount(&nDevices);
  for (int i = 0; i < nDevices; i++) {
    cudaGetDeviceProperties(&tmp->properties, i);
    if (tmp->properties.major >= 2) double_device = i;
    if (tmp->properties.major >= 3) unified_device = i;
    if ((unified_device + 1) && (double_device + 1)) break;
  }
  massert(nDevices, "cuSPARSE_desc: No CUDA device found");
  if (unified_device + 1)
    tmp->target_mem = SPMV_MEMTYPE_UNIFIED;
  else if (double_device < 0)
    massert(0, "cuSPARSE_desc: Device does not support double values");
  else
    tmp->target_mem = SPMV_MEMTYPE_DEVICE;
  /// TODO: Add multiple gpu support?
  cudaSetDevice(unified_device);
  // printf("Using Device: %d\n", unified_device);
  cudaStreamCreate(&tmp->stream);
  cusparseCreate(&tmp->handle);
  cusparseSetStream(tmp->handle, tmp->stream);
  cusparseCreateMatDescr(&tmp->descA);
  cusparseSetMatType(tmp->descA, CUSPARSE_MATRIX_TYPE_GENERAL);
  cusparseSetMatIndexBase(tmp->descA, CUSPARSE_INDEX_BASE_ZERO);
  cusparseCreateMatDescr(&tmp->descB);
  cusparseSetMatType(tmp->descB, CUSPARSE_MATRIX_TYPE_GENERAL);
  cusparseSetMatIndexBase(tmp->descB, CUSPARSE_INDEX_BASE_ZERO);
  tmp->dir = CUSPARSE_DIRECTION_COLUMN;
  cudaDeviceSynchronize();
  ddebug("  <- cuSPARSE_desc()\n");
  return tmp;
}

void SpmvOperator::cuSPARSE_check_compatibility() {
  ddebug(" -> SpmvOperator::cuSPARSE_check_compatibility()\n");
  if (0)
    massert(0,
            "SpmvOperator::cuSPARSE_check_compatibility -> SpmvOperator not "
            "compatible");
  ddebug(" <- SpmvOperator::cuSPARSE_check_compatibility()\n");
}

void SpmvOperator::cuSPARSE_init() {
  ddebug(" -> SpmvOperator::cuSPARSE_init()\n");
  cuSPARSE_check_compatibility();
  free_lib_struct();
  lib_struct = cuSPARSE_desc();
  lib = SPMV_LIBRARY_CUSPARSE;
  mem_convert(((cuSPARSE_wrap *)lib_struct)->target_mem);
  ddebug(" <- SpmvOperator::cuSPARSE_init()\n");
}

void cuSPARSE_free(cuSPARSE_wrap *tmp) {
  ddebug(" -> cuSPARSE_free(wrapper)\n");
  cudaStreamDestroy(tmp->stream);
  cusparseDestroy(tmp->handle);
  cusparseDestroyMatDescr(tmp->descA);
  cusparseDestroyMatDescr(tmp->descB);
  cudaDeviceSynchronize();
  free(tmp);
  ddebug(" <- cuSPARSE_free(wrapper)\n");
}

void SpmvOperator::format_convert_uni_coo2csr() {
  ddebug(" -> SpmvOperator::format_convert_uni_coo2csr()\n");
  SpmvCooData *coo_data = (SpmvCooData *)format_data;
  SpmvCsrData *csr_data = (SpmvCsrData *)malloc(sizeof(SpmvCsrData));
  csr_data->colInd = coo_data->colInd;
  csr_data->values = coo_data->values;
  cudaMallocManaged(&csr_data->rowPtr, (m + 1) * sizeof(int));

  timer = csecond();
  cusparseXcoo2csr(((cuSPARSE_wrap *)lib_struct)->handle, coo_data->rowInd, nz,
                   m, csr_data->rowPtr, CUSPARSE_INDEX_BASE_ZERO);
  cudaDeviceSynchronize();
  timer = csecond() - timer;

  gpu_free(coo_data->rowInd);
  cudaCheckErrors();
  format_data = csr_data;
  format = SPMV_FORMAT_CSR;
  ddebug(" <- SpmvOperator::format_convert_uni_coo2csr()\n");
}

void SpmvOperator::format_convert_uni_csr2hyb() {
  ddebug(" -> SpmvOperator::format_convert_uni_csr2hyb()\n");
  SpmvCsrData *csr_data = (SpmvCsrData *)format_data;
  SpmvHybData *hyb_data = (SpmvHybData *)malloc(sizeof(SpmvHybData));
  timer = csecond();
  cusparseCreateHybMat(&hyb_data->hybMatrix);
  switch (value_type) {
    case (SPMV_VALUETYPE_FLOAT): {
      cusparseScsr2hyb(((cuSPARSE_wrap *)lib_struct)->handle, m, n,
                       ((cuSPARSE_wrap *)lib_struct)->descA,
                       (float *)csr_data->values, csr_data->rowPtr,
                       csr_data->colInd, hyb_data->hybMatrix, 0,
                       CUSPARSE_HYB_PARTITION_AUTO); // == HYB. CUSPARSE_HYB_PARTITION_MAX); // == ELL. 
    } break;
    case (SPMV_VALUETYPE_DOUBLE): {
      cusparseDcsr2hyb(((cuSPARSE_wrap *)lib_struct)->handle, m, n,
                       ((cuSPARSE_wrap *)lib_struct)->descA,
                       (double *)csr_data->values, csr_data->rowPtr,
                       csr_data->colInd, hyb_data->hybMatrix, 0,
                       CUSPARSE_HYB_PARTITION_AUTO); // == HYB. CUSPARSE_HYB_PARTITION_MAX); // == ELL.
    } break;
    default:
      massert(false,
              "SpmvOperator::format_convert_uni_csr2hyb() -> Unsupported SpMV "
              "value datatype");
  }
  cudaDeviceSynchronize();
  timer = csecond() - timer;
  spmv_free();
  cudaCheckErrors();
  format_data = hyb_data;
  format = SPMV_FORMAT_HYB;
  ddebug(" <- SpmvOperator::format_convert_uni_csr2hyb()\n");
}

void SpmvOperator::format_convert_uni_csr2bsr() {
  ddebug(" -> SpmvOperator::format_convert_uni_csr2bsr()\n");
  SpmvCsrData *csr_data = (SpmvCsrData *)format_data;
  SpmvBsrData *bsr_data = (SpmvBsrData *)malloc(sizeof(SpmvBsrData));
  const int nb = (n + bsr_blockDim - 1) / bsr_blockDim;
  const int mb = (m + bsr_blockDim - 1) / bsr_blockDim;
  bsr_data->blockDim = bsr_blockDim;
  timer = csecond();
  cudaMallocManaged(&bsr_data->rowPtr, (mb + 1) * sizeof(int));

  cusparseXcsr2bsrNnz(
      ((cuSPARSE_wrap *)lib_struct)->handle, ((cuSPARSE_wrap *)lib_struct)->dir,
      m, n, ((cuSPARSE_wrap *)lib_struct)->descA, csr_data->rowPtr,
      csr_data->colInd, bsr_data->blockDim,
      ((cuSPARSE_wrap *)lib_struct)->descB, bsr_data->rowPtr, &bsr_data->nnzb);
  cudaDeviceSynchronize();
  cudaMallocManaged(&bsr_data->colInd, bsr_data->nnzb * sizeof(int));

  switch (value_type) {
    case (SPMV_VALUETYPE_FLOAT): {
      cudaMallocManaged(&bsr_data->values,
                        (bsr_data->blockDim * bsr_data->blockDim) *
                            bsr_data->nnzb * sizeof(float));
      cusparseScsr2bsr(
          ((cuSPARSE_wrap *)lib_struct)->handle,
          ((cuSPARSE_wrap *)lib_struct)->dir, m, n,
          ((cuSPARSE_wrap *)lib_struct)->descA, (float *)csr_data->values,
          csr_data->rowPtr, csr_data->colInd, bsr_data->blockDim,
          ((cuSPARSE_wrap *)lib_struct)->descB, (float *)bsr_data->values,
          bsr_data->rowPtr, bsr_data->colInd);
    } break;
    case (SPMV_VALUETYPE_DOUBLE): {
      cudaMallocManaged(&bsr_data->values,
                        (bsr_data->blockDim * bsr_data->blockDim) *
                            bsr_data->nnzb * sizeof(double));
      cusparseDcsr2bsr(
          ((cuSPARSE_wrap *)lib_struct)->handle,
          ((cuSPARSE_wrap *)lib_struct)->dir, m, n,
          ((cuSPARSE_wrap *)lib_struct)->descA, (double *)csr_data->values,
          csr_data->rowPtr, csr_data->colInd, bsr_data->blockDim,
          ((cuSPARSE_wrap *)lib_struct)->descB, (double *)bsr_data->values,
          bsr_data->rowPtr, bsr_data->colInd);
    } break;
    default:
      massert(false,
              "SpmvOperator::format_convert_uni_csr2bsr -> Unsupported SpMV "
              "value datatype");
  }
  cudaDeviceSynchronize();
  cudaCheckErrors();
  timer = csecond() - timer;
  spmv_free();
  cudaCheckErrors();
  format_data = bsr_data;
  format = SPMV_FORMAT_BSR;

  /// BSR vector padding TODO: This is kind of a cheat, maybe n, m should change
  /// too and vec_alloc_uni used for this
  void *xp, *yp;
  switch (value_type) {
    case (SPMV_VALUETYPE_FLOAT): {
      cudaMallocManaged(&xp, nb * bsr_data->blockDim * sizeof(float));
      cudaMallocManaged(&yp, mb * bsr_data->blockDim * sizeof(float));
      vec_copy<float>((float *)xp, (float *)x, m, mb * bsr_data->blockDim - m);
      for (int i = 0; i < m; i++) ((float *)yp)[i] = 0;
    } break;
    case (SPMV_VALUETYPE_DOUBLE): {
      cudaMallocManaged(&xp, nb * bsr_data->blockDim * sizeof(double));
      cudaMallocManaged(&yp, mb * bsr_data->blockDim * sizeof(double));
      vec_copy<double>((double *)xp, (double *)x, m,
                       mb * bsr_data->blockDim - m);
      for (int i = 0; i < m; i++) ((double *)yp)[i] = 0;
    } break;
    default:
      massert(false,
              "SpmvOperator::format_convert_uni_csr2bsr -> Unsupported SpMV "
              "value datatype");
  }
  massert(xp && yp,
          "SpmvOperator::format_convert_uni_csr2bsr -> Padded Vector Unified "
          "Alloc failed");
  gpu_free(x);
  gpu_free(y);
  x = xp;
  y = yp;
  ddebug(" <- SpmvOperator::format_convert_uni_csr2bsr()\n");
}

void SpmvOperator::format_convert_uni_csr2coo() {
  ddebug(" -> SpmvOperator::format_convert_uni_csr2coo()\n");
  SpmvCsrData *csr_data = (SpmvCsrData *)format_data;
  SpmvCooData *coo_data = (SpmvCooData *)malloc(sizeof(SpmvCooData));
  coo_data->colInd = csr_data->colInd;
  coo_data->values = csr_data->values;
  cudaMallocManaged(&coo_data->rowInd, nz * sizeof(int));

  timer = csecond();
  cusparseXcsr2coo(((cuSPARSE_wrap *)lib_struct)->handle, csr_data->rowPtr, nz,
                   m, coo_data->rowInd, CUSPARSE_INDEX_BASE_ZERO);
  cudaDeviceSynchronize();
  timer = csecond() - timer;

  gpu_free(csr_data->rowPtr);
  cudaCheckErrors();
  format_data = coo_data;
  format = SPMV_FORMAT_COO;
  ddebug(" <- SpmvOperator::format_convert_uni_csr2coo()\n");
}

void SpmvOperator::format_convert_uni_bsr2csr() {
  ddebug(" -> SpmvOperator::format_convert_uni_bsr2csr()\n");
massert(false, "SpmvOperator::format_convert_uni_bsr2csr -> Implementation is not correct");
  SpmvBsrData *bsr_data = (SpmvBsrData *)format_data;
  SpmvCsrData *csr_data = (SpmvCsrData *)malloc(sizeof(SpmvCsrData));
  const int nb = (n + bsr_data->blockDim - 1) / bsr_data->blockDim;
  const int mb = (m + bsr_data->blockDim - 1) / bsr_data->blockDim;

  timer = csecond();
  cudaMallocManaged(&csr_data->rowPtr, (m + 1) * sizeof(int));
  cudaMallocManaged(&csr_data->colInd, nz * sizeof(int));

  switch (value_type) {
    case (SPMV_VALUETYPE_FLOAT): {
      cudaMallocManaged(&csr_data->values,
                        nz * sizeof(float));
      cusparseSbsr2csr(
          ((cuSPARSE_wrap *)lib_struct)->handle,
          ((cuSPARSE_wrap *)lib_struct)->dir, mb, nb, 
          ((cuSPARSE_wrap *)lib_struct)->descA, (float *)bsr_data->values,
          bsr_data->rowPtr, bsr_data->colInd, bsr_data->blockDim, 
          ((cuSPARSE_wrap *)lib_struct)->descB, (float *)csr_data->values,
          csr_data->rowPtr, csr_data->colInd);
    } break;
    case (SPMV_VALUETYPE_DOUBLE): {
      cudaMallocManaged(&csr_data->values,
                        nz * sizeof(double));
      cusparseDbsr2csr(
          ((cuSPARSE_wrap *)lib_struct)->handle,
          ((cuSPARSE_wrap *)lib_struct)->dir, mb, nb, 
          ((cuSPARSE_wrap *)lib_struct)->descA, (double *)bsr_data->values,
          bsr_data->rowPtr, bsr_data->colInd, bsr_data->blockDim, 
          ((cuSPARSE_wrap *)lib_struct)->descB, (double *)csr_data->values,
          csr_data->rowPtr, csr_data->colInd);
    } break;
    default:
      massert(false,
              "SpmvOperator::format_convert_uni_bsr2csr -> Unsupported SpMV "
              "value datatype");
  }
  cudaDeviceSynchronize();
  timer = csecond() - timer;
  spmv_free();
  cudaCheckErrors();
  format_data = csr_data;
  format = SPMV_FORMAT_CSR;


  ddebug(" <- SpmvOperator::format_convert_uni_bsr2csr()\n");
}

void SpmvOperator::format_convert_uni_bsr2bsr() {
  ddebug(" -> SpmvOperator::format_convert_uni_bsr2bsr()\n");
massert(false, "SpmvOperator::format_convert_uni_bsr2bsr -> Implementation is not correct");
  SpmvBsrData *bsr_data = (SpmvBsrData *)format_data;
  SpmvBsrData *bsr_data1 = (SpmvBsrData *)malloc(sizeof(SpmvBsrData));
  const int nb = (n + bsr_data->blockDim - 1) / bsr_data->blockDim;
  const int mb = (m + bsr_data->blockDim - 1) / bsr_data->blockDim;

  timer = csecond();
  int bufferSize, *nnzTotalDevHostPtr;
  void *pBuffer;

  switch (value_type) {
    case (SPMV_VALUETYPE_FLOAT): {
     cusparseSgebsr2gebsr_bufferSize(((cuSPARSE_wrap *)lib_struct)->handle, ((cuSPARSE_wrap *)lib_struct)->dir, mb, nb, bsr_data->nnzb,
    ((cuSPARSE_wrap *)lib_struct)->descA, (float *)bsr_data->values , bsr_data->rowPtr, bsr_data->colInd,
    bsr_data->blockDim, bsr_data->blockDim,
    bsr_blockDim, bsr_blockDim,
    &bufferSize);
    } break;
    case (SPMV_VALUETYPE_DOUBLE): {
      cusparseDgebsr2gebsr_bufferSize(((cuSPARSE_wrap *)lib_struct)->handle, ((cuSPARSE_wrap *)lib_struct)->dir, mb, nb, bsr_data->nnzb,
    ((cuSPARSE_wrap *)lib_struct)->descA, (double *)bsr_data->values , bsr_data->rowPtr, bsr_data->colInd,
    bsr_data->blockDim, bsr_data->blockDim,
    bsr_blockDim, bsr_blockDim,
    &bufferSize);
    } break;
    default:
      massert(false,
              "SpmvOperator::format_convert_uni_csr2bsr -> Unsupported SpMV "
              "value datatype");
  }
cudaDeviceSynchronize();
cudaMallocManaged(&pBuffer, bufferSize);

  bsr_data1->blockDim = bsr_blockDim;
  const int nb1 = (nb* bsr_data->blockDim + bsr_data1->blockDim - 1) / bsr_data1->blockDim;
  const int mb1 = (mb* bsr_data->blockDim + bsr_data1->blockDim - 1) / bsr_data1->blockDim;
  cudaMallocManaged(&bsr_data1->rowPtr, (mb + 1) * sizeof(int));


cusparseXgebsr2gebsrNnz( ((cuSPARSE_wrap *)lib_struct)->handle, ((cuSPARSE_wrap *)lib_struct)->dir,
      mb, nb, bsr_data->nnzb, ((cuSPARSE_wrap *)lib_struct)->descA, bsr_data->rowPtr,
      bsr_data->colInd, bsr_data->blockDim, bsr_data->blockDim,
((cuSPARSE_wrap *)lib_struct)->descB, bsr_data1->rowPtr, bsr_data1->blockDim, bsr_data1->blockDim, nnzTotalDevHostPtr, pBuffer);
cudaDeviceSynchronize();

if (NULL != nnzTotalDevHostPtr) bsr_data1->nnzb = *nnzTotalDevHostPtr;
else bsr_data1->nnzb =  bsr_data1->rowPtr[mb1] - bsr_data1->rowPtr[0];

  cudaMallocManaged(&bsr_data1->colInd, bsr_data1->nnzb * sizeof(int));

  switch (value_type) {
    case (SPMV_VALUETYPE_FLOAT): {
      cudaMallocManaged(&bsr_data1->values,
                        (bsr_data1->blockDim * bsr_data1->blockDim) *
                            bsr_data1->nnzb * sizeof(float));
      cusparseSgebsr2gebsr(
          ((cuSPARSE_wrap *)lib_struct)->handle,
          ((cuSPARSE_wrap *)lib_struct)->dir, mb, nb, bsr_data->nnzb,
          ((cuSPARSE_wrap *)lib_struct)->descA, (float *)bsr_data->values,
          bsr_data->rowPtr, bsr_data->colInd, bsr_data->blockDim, bsr_data->blockDim,
          ((cuSPARSE_wrap *)lib_struct)->descB, (float *)bsr_data1->values,
          bsr_data1->rowPtr, bsr_data1->colInd, bsr_data1->blockDim, bsr_data1->blockDim, pBuffer);
    } break;
    case (SPMV_VALUETYPE_DOUBLE): {
      cudaMallocManaged(&bsr_data1->values,
                        (bsr_data1->blockDim * bsr_data1->blockDim) *
                            bsr_data1->nnzb * sizeof(double));
      cusparseDgebsr2gebsr(
          ((cuSPARSE_wrap *)lib_struct)->handle,
          ((cuSPARSE_wrap *)lib_struct)->dir, mb, nb, bsr_data->nnzb,
          ((cuSPARSE_wrap *)lib_struct)->descA, (double *)bsr_data->values,
          bsr_data->rowPtr, bsr_data->colInd, bsr_data->blockDim, bsr_data->blockDim,
          ((cuSPARSE_wrap *)lib_struct)->descB, (double *)bsr_data1->values,
          bsr_data1->rowPtr, bsr_data1->colInd, bsr_data1->blockDim, bsr_data1->blockDim, pBuffer);
    } break;
    default:
      massert(false,
              "SpmvOperator::format_convert_uni_bsr2bsr -> Unsupported SpMV "
              "value datatype");
  }
  cudaDeviceSynchronize();
  timer = csecond() - timer;
  spmv_free();
  cudaCheckErrors();
  format_data = bsr_data1;
  format = SPMV_FORMAT_BSR;

  /// BSR vector padding TODO: This is kind of a cheat, maybe n, m should change
  /// too and vec_alloc_uni used for this
  void *xp, *yp;
  switch (value_type) {
    case (SPMV_VALUETYPE_FLOAT): {
      cudaMallocManaged(&xp, nb1 * bsr_data1->blockDim * sizeof(float));
      cudaMallocManaged(&yp, mb1 * bsr_data1->blockDim * sizeof(float));
      vec_copy<float>((float *)xp, (float *)x, nb* bsr_data->blockDim, nb1 * bsr_data1->blockDim - nb* bsr_data->blockDim);
      for (int i = 0; i < mb* bsr_data->blockDim; i++) ((float *)yp)[i] = 0;
    } break;
    case (SPMV_VALUETYPE_DOUBLE): {
      cudaMallocManaged(&xp, nb1 * bsr_data1->blockDim * sizeof(double));
      cudaMallocManaged(&yp, mb1 * bsr_data1->blockDim * sizeof(double));
      vec_copy<double>((double *)xp, (double *)x, nb* bsr_data->blockDim,
                       nb1 * bsr_data1->blockDim - nb* bsr_data->blockDim);
      for (int i = 0; i < mb* bsr_data->blockDim; i++) ((double *)yp)[i] = 0;
    } break;
    default:
      massert(false,
              "SpmvOperator::format_convert_uni_bsr2bsr -> Unsupported SpMV "
              "value datatype");
  }
  massert(xp && yp,
          "SpmvOperator::format_convert_uni_bsr2bsr -> Padded Vector Unified "
          "Alloc failed");
  gpu_free(x);
  gpu_free(y);
  x = xp;
  y = yp;
  ddebug(" <- SpmvOperator::format_convert_uni_bsr2bsr()\n");
}

void SpmvOperator::format_convert_uni_hyb2csr() {
  ddebug(" -> SpmvOperator::format_convert_uni_hyb2csr()\n");
  SpmvHybData *hyb_data = (SpmvHybData *)format_data;
  SpmvCsrData *csr_data = (SpmvCsrData *)malloc(sizeof(SpmvCsrData));

  cudaMallocManaged(&csr_data->rowPtr, (m + 1) * sizeof(int));
  cudaMallocManaged(&csr_data->colInd, nz * sizeof(int));

timer = csecond();
switch (value_type) {
    case (SPMV_VALUETYPE_FLOAT): {
      cudaMallocManaged(&csr_data->values, nz * sizeof(float));
cusparseShyb2csr(((cuSPARSE_wrap *)lib_struct)->handle, ((cuSPARSE_wrap *)lib_struct)->descA, 
                 hyb_data->hybMatrix, (float *) csr_data->values, (int *) csr_data->rowPtr, (int *) csr_data->colInd);
    } break;
    case (SPMV_VALUETYPE_DOUBLE): {
      cudaMallocManaged(&csr_data->values,  nz * sizeof(double));
cusparseDhyb2csr(((cuSPARSE_wrap *)lib_struct)->handle, ((cuSPARSE_wrap *)lib_struct)->descA, 
                 hyb_data->hybMatrix, (double *) csr_data->values, (int *) csr_data->rowPtr, (int *) csr_data->colInd);
    } break;
    default:
      massert(false,
              "SpmvOperator::format_convert_uni_hyb2csr -> Unsupported SpMV "
              "value datatype");
  }
  cudaDeviceSynchronize();
  timer = csecond() - timer;
spmv_free();
  cudaCheckErrors();
  format_data = csr_data;
  format = SPMV_FORMAT_CSR;
  ddebug(" <- SpmvOperator::format_convert_uni_hyb2csr()\n");
}

void SpmvOperator::cuSPARSE_csr() {
  ddebug(" -> SpmvOperator::cuSPARSE_csr()\n");
  massert(format == SPMV_FORMAT_CSR,
          "SpmvOperator::cuSPARSE_csr -> Wrong input format");

  SpmvCsrData *data = (SpmvCsrData *)format_data;
  switch (value_type) {
    case (SPMV_VALUETYPE_FLOAT): {
      const float alf = 1.0;
      const float beta = 0;
      cusparseScsrmv(((cuSPARSE_wrap *)lib_struct)->handle,
                     CUSPARSE_OPERATION_NON_TRANSPOSE, m, n, nz, &alf,
                     ((cuSPARSE_wrap *)lib_struct)->descA,
                     (float *)data->values, data->rowPtr, data->colInd,
                     (float *)x, &beta, (float *)y);
    } break;
    case (SPMV_VALUETYPE_DOUBLE): {
      const double alf = 1.0;
      const double beta = 0;
      cusparseDcsrmv(((cuSPARSE_wrap *)lib_struct)->handle,
                     CUSPARSE_OPERATION_NON_TRANSPOSE, m, n, nz, &alf,
                     ((cuSPARSE_wrap *)lib_struct)->descA,
                     (double *)data->values, data->rowPtr, data->colInd,
                     (double *)x, &beta, (double *)y);
    } break;
    case (SPMV_VALUETYPE_INT):
    case (SPMV_VALUETYPE_BINARY):
    default:
      massert(false,
              "SpmvOperator::cuSPARSE_csr -> Unsupported SpMV value datatype");
  }
  ddebug(" <- SpmvOperator::cuSPARSE_csr()\n");
}

void SpmvOperator::cuSPARSE_hyb() {
  ddebug(" -> SpmvOperator::cuSPARSE_hyb()\n");
  massert(format == SPMV_FORMAT_HYB,
          "SpmvOperator::cuSPARSE_hyb -> Wrong input format");

  SpmvHybData *data = (SpmvHybData *)format_data;

  switch (value_type) {
    case (SPMV_VALUETYPE_FLOAT): {
      const float alf = 1.0;
      const float beta = 0;
      cusparseShybmv(((cuSPARSE_wrap *)lib_struct)->handle,
                     CUSPARSE_OPERATION_NON_TRANSPOSE, &alf,
                     ((cuSPARSE_wrap *)lib_struct)->descA, data->hybMatrix,
                     (float *)x, &beta, (float *)y);
      break;
    }
    case (SPMV_VALUETYPE_DOUBLE): {
      const double alf = 1.0;
      const double beta = 0;
      cusparseDhybmv(((cuSPARSE_wrap *)lib_struct)->handle,
                     CUSPARSE_OPERATION_NON_TRANSPOSE, &alf,
                     ((cuSPARSE_wrap *)lib_struct)->descA, data->hybMatrix,
                     (double *)x, &beta, (double *)y);
      break;
    }
    case (SPMV_VALUETYPE_INT):
    case (SPMV_VALUETYPE_BINARY):
    default:
      massert(false,
              "SpmvOperator::cuSPARSE_hyb -> Unsupported SpMV value datatype");
  }
  ddebug(" <- SpmvOperator::cuSPARSE_hyb()\n");
}

void SpmvOperator::cuSPARSE_bsr() {
  ddebug(" -> SpmvOperator::cuSPARSE_bsr()\n");
  massert(format == SPMV_FORMAT_BSR,
          "SpmvOperator::cuSPARSE_bsr -> Wrong input format");

  SpmvBsrData *data = (SpmvBsrData *)format_data;
  const int nb = (n + data->blockDim - 1) / data->blockDim;
  const int mb = (m + data->blockDim - 1) / data->blockDim;

  switch (value_type) {
    case (SPMV_VALUETYPE_FLOAT): {
      const float alf = 1.0;
      const float beta = 0;
      cusparseSbsrmv(((cuSPARSE_wrap *)lib_struct)->handle,
                     ((cuSPARSE_wrap *)lib_struct)->dir,
                     CUSPARSE_OPERATION_NON_TRANSPOSE, mb, nb, data->nnzb, &alf,
                     ((cuSPARSE_wrap *)lib_struct)->descA,
                     (float *)data->values, data->rowPtr, data->colInd,
                     data->blockDim, (float *)x, &beta, (float *)y);
    } break;
    case (SPMV_VALUETYPE_DOUBLE): {
      const double alf = 1.0;
      const double beta = 0;
      cusparseDbsrmv(((cuSPARSE_wrap *)lib_struct)->handle,
                     ((cuSPARSE_wrap *)lib_struct)->dir,
                     CUSPARSE_OPERATION_NON_TRANSPOSE, mb, nb, data->nnzb, &alf,
                     ((cuSPARSE_wrap *)lib_struct)->descA,
                     (double *)data->values, data->rowPtr, data->colInd,
                     data->blockDim, (double *)x, &beta, (double *)y);
    } break;
    case (SPMV_VALUETYPE_INT):
    case (SPMV_VALUETYPE_BINARY):
    default:
      massert(false,
              "SpmvOperator::cuSPARSE_bsr -> Unsupported SpMV value datatype");
  }
  ddebug(" <- SpmvOperator::cuSPARSE_bsr()\n");
}
