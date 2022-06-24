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
  massert(false,"SpmvOperator::format_convert_uni_csr2hyb -> Not available/implemented for cuda11.0");
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


      cudaMallocManaged(&bsr_data->values,
                        (bsr_data->blockDim * bsr_data->blockDim) *
                            bsr_data->nnzb * sizeof(VALUE_TYPE_AX));
#if VALUE_TYPE_AX == float
      cusparseScsr2bsr(
          ((cuSPARSE_wrap *)lib_struct)->handle,
          ((cuSPARSE_wrap *)lib_struct)->dir, m, n,
          ((cuSPARSE_wrap *)lib_struct)->descA, (float *)csr_data->values,
          csr_data->rowPtr, csr_data->colInd, bsr_data->blockDim,
          ((cuSPARSE_wrap *)lib_struct)->descB, (float *)bsr_data->values,
          bsr_data->rowPtr, bsr_data->colInd);
#elif VALUE_TYPE_AX == double    
      cusparseDcsr2bsr(
          ((cuSPARSE_wrap *)lib_struct)->handle,
          ((cuSPARSE_wrap *)lib_struct)->dir, m, n,
          ((cuSPARSE_wrap *)lib_struct)->descA, (double *)csr_data->values,
          csr_data->rowPtr, csr_data->colInd, bsr_data->blockDim,
          ((cuSPARSE_wrap *)lib_struct)->descB, (double *)bsr_data->values,
          bsr_data->rowPtr, bsr_data->colInd);
#else
		massert(0, "SpmvOperator::format_convert_uni_csr2bsr(): Unsupported VALUE_TYPE_AX");
#endif

  cudaDeviceSynchronize();
  cudaCheckErrors();
  timer = csecond() - timer;
  spmv_free();
  cudaCheckErrors();
  format_data = bsr_data;
  format = SPMV_FORMAT_BSR;

  /// BSR vector padding TODO: This is kind of a cheat, maybe n, m should change
  /// too and vec_alloc_uni used for this
  VALUE_TYPE_AX *xp;
  VALUE_TYPE_Y *yp;

      cudaMallocManaged(&xp, nb * bsr_data->blockDim * sizeof(VALUE_TYPE_AX));
      cudaMallocManaged(&yp, mb * bsr_data->blockDim * sizeof(VALUE_TYPE_Y));
      vec_copy<VALUE_TYPE_AX>((VALUE_TYPE_AX *)xp, (VALUE_TYPE_AX *)x, m, mb * bsr_data->blockDim - m);
      for (int i = 0; i < m; i++) ((VALUE_TYPE_Y *)yp)[i] = 0;
 
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

      cudaMallocManaged(&csr_data->values,
                        nz * sizeof(VALUE_TYPE_AX));
                        
#if VALUE_TYPE_AX == float
      cusparseSbsr2csr(
          ((cuSPARSE_wrap *)lib_struct)->handle,
          ((cuSPARSE_wrap *)lib_struct)->dir, mb, nb, 
          ((cuSPARSE_wrap *)lib_struct)->descA, (float *)bsr_data->values,
          bsr_data->rowPtr, bsr_data->colInd, bsr_data->blockDim, 
          ((cuSPARSE_wrap *)lib_struct)->descB, (float *)csr_data->values,
          csr_data->rowPtr, csr_data->colInd);
#elif VALUE_TYPE_AX == double    
      cusparseDbsr2csr(
          ((cuSPARSE_wrap *)lib_struct)->handle,
          ((cuSPARSE_wrap *)lib_struct)->dir, mb, nb, 
          ((cuSPARSE_wrap *)lib_struct)->descA, (double *)bsr_data->values,
          bsr_data->rowPtr, bsr_data->colInd, bsr_data->blockDim, 
          ((cuSPARSE_wrap *)lib_struct)->descB, (double *)csr_data->values,
          csr_data->rowPtr, csr_data->colInd);
#else
		massert(0, "SpmvOperator::format_convert_uni_csr2bsr(): Unsupported VALUE_TYPE_AX");
#endif

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
  massert(false,"SpmvOperator::format_convert_uni_bsr2bsr -> Not available/implemented for cuda11.0");
  ddebug(" <- SpmvOperator::format_convert_uni_bsr2bsr()\n");
}

void SpmvOperator::format_convert_uni_hyb2csr() {
  ddebug(" -> SpmvOperator::format_convert_uni_hyb2csr()\n");
  massert(false,"SpmvOperator::format_convert_uni_hyb2csr -> Not available/implemented for cuda11.0");
  ddebug(" <- SpmvOperator::format_convert_uni_hyb2csr()\n");
}

void SpmvOperator::cuSPARSE_csr() {
  ddebug(" -> SpmvOperator::cuSPARSE_csr()\n");
  massert(false,"SpmvOperator::cuSPARSE_csr -> Not available/implemented for cuda11.0");
  ddebug(" <- SpmvOperator::cuSPARSE_csr()\n");
}

void SpmvOperator::cuSPARSE_hyb() {
  ddebug(" -> SpmvOperator::cuSPARSE_hyb()\n");
  massert(false,"SpmvOperator::cuSPARSE_hyb -> Not available/implemented for cuda11.0");
  ddebug(" <- SpmvOperator::cuSPARSE_hyb()\n");
}

void SpmvOperator::cuSPARSE_bsr() {
  ddebug(" -> SpmvOperator::cuSPARSE_bsr()\n");
  massert(false,"SpmvOperator::cuSPARSE_bsr -> Not available/implemented for cuda11.0");
  ddebug(" <- SpmvOperator::cuSPARSE_bsr()\n");
}
