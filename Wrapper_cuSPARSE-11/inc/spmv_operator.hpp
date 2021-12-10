///
/// \author Anastasiadis Petros (panastas@cslab.ece.ntua.gr)
///
/// \brief Some helpfull functions for SpMV
///
#ifndef SPMV_OPERATOR_H
#define SPMV_OPERATOR_H
#include <cusparse_v2.h>
#include <cstdint>
//#define NR_ITER 1000

typedef enum {
  SPMV_FORMAT_COO = 0,
  SPMV_FORMAT_CSR,
  SPMV_FORMAT_BSR,
  SPMV_FORMAT_DIA,
  SPMV_FORMAT_ELL,
  SPMV_FORMAT_HYB,
  SPMV_FORMAT_CSR5
} SpmvFormat;

typedef enum {
  SPMV_NONE = 0,
  SPMV_LIBRARY_CUSPARSE,
  SPMV_LIBRARY_OPENMP,
  SPMV_LIBRARY_CSR5_AVX512,
  SPMV_LIBRARY_CSR5_CUDA,
  SPMV_LIBRARY_MKL
} SpmvLib;

typedef enum {
  SPMV_MEMTYPE_HOST = 0,
  SPMV_MEMTYPE_DEVICE,
  SPMV_MEMTYPE_UNIFIED,
  SPMV_MEMTYPE_NUMA
} SpmvMemType;

typedef enum {
  OPERATOR_MASTER = 0,
  OPERATOR_COPY_SHARED,
  OPERATOR_COPY_CLEAN
} OperatorMode;

typedef struct {
  int *rowInd = NULL;
  int *colInd = NULL;
  VALUE_TYPE_AX* values = NULL;
} SpmvCooData;

typedef struct {
  int *rowPtr = NULL;
  int *colInd = NULL;
  VALUE_TYPE_AX* values = NULL;
} SpmvCsrData;

typedef struct {
  int blockDim;
  int nnzb;
  int *rowPtr = NULL;
  int *colInd = NULL;
  VALUE_TYPE_AX* values = NULL;
} SpmvBsrData;

class SpmvOperator {
  // private:
 public:
  // Matrix variables
  char *mtx_name;
  int n = 0;
  int m = 0;
  int nz = 0;
  int bsr_blockDim = 0;

  // Matrix Generation values
  char distribution[256], placement[256];
  int seed;

  // Statistic values
  double density, avg_nz_row, std_nz_row, avg_bandwidth, std_bandwidth, avg_scattering, std_scattering, bw_scaled, skew;
  
  // Operator variables TODO: Might be outdated in latest versions
  int bytes = 0;
  int flops = 0;
  size_t mem_bytes = 0;
  size_t gpu_mem_bytes = 0;
  double A_mem_footprint;
  char* mem_range;
  double timer;

  // Data
  void *format_data = NULL;
  VALUE_TYPE_AX *x = NULL;
  VALUE_TYPE_Y *y = NULL;

  // Helper struct for each lib wrapper
  void *lib_struct = NULL;

  void mtx_read_host();
  void mtx_read_uni();
  void mtx_read_device();
  void mtx_read_cuSP();
  
  void mtx_generate_host(int argc, char *argv[], int start_of_matrix_generation_args, int verbose);
  void mtx_generate_uni(int argc, char *argv[], int start_of_matrix_generation_args, int verbose);
  void mtx_generate_device(int argc, char *argv[], int start_of_matrix_generation_args, int verbose);

  /// A function for allocating vectors x and y(=0)

  void vec_alloc_host(VALUE_TYPE_AX *x);
  void vec_alloc_numa(VALUE_TYPE_AX *x);
  void vec_alloc_uni(VALUE_TYPE_AX *x);
  void vec_alloc_device(VALUE_TYPE_AX *x);
  void vec_alloc_cuSP(VALUE_TYPE_AX *x);

  void *spmv_data_copy_host();
  void *spmv_data_copy_numa();
  void *spmv_data_copy_uni();
  void *spmv_data_copy_device();
  void *spmv_data_copy_cuSP();

  void *spmv_data_subcopy_host(int *start, int *nzc, int mode);
  void *spmv_data_subcopy_uni(int *start, int *nzc, int mode);
  void *spmv_data_subcopy_device(int *start, int *nzc, int mode);
  void *spmv_data_subcopy_cuSP(int *start, int *nzc, int mode);

  void mem_convert_host();
  void mem_convert_uni();
  void mem_convert_numa();
  void mem_convert_device();
  void mem_convert_cuSP();

  /// TODO: Some sub-functions are not implemented
  void format_convert_host(SpmvFormat target_format);
  void format_convert_uni(SpmvFormat target_format);
  void format_convert_device(SpmvFormat target_format);
  void format_convert_cuSP(SpmvFormat target_format);

  void format_convert_uni_coo2csr();
  void format_convert_uni_csr2coo();
  void format_convert_uni_bsr2csr();
  void format_convert_uni_csr2bsr();
  void format_convert_uni_bsr2bsr();
  void format_convert_uni_hyb2csr();
  void format_convert_uni_csr2hyb();

  void spmv_free_host();
  void spmv_free_numa();
  void spmv_free_device();
  void spmv_free_cuSP();

  VALUE_TYPE_Y *y_copy_cuSP();

  // public:
  // Enums
  SpmvFormat format;
  SpmvLib lib;
  SpmvMemType mem_alloc;

  // Default constructor 
  SpmvOperator();

  // Constructor from .mtx
  SpmvOperator(char *matrix_name);

  // Constructor from Mpakos generator
  SpmvOperator(int argc, char *argv[], int start_of_matrix_generation_args, int verbose);
  
  // Copy constructor
  SpmvOperator(SpmvOperator &op);

  // Sub-copy constructor
  SpmvOperator(SpmvOperator &op, int start, int end, int mode);

  /// Destructor
  ~SpmvOperator();

  /// Prints useful operator info
  void print_op();

  /// Generalized mtx input function calling the correct reader to CSR
  void mtx_read();
  
  /// Generalized mtx generation function calling the correct generator to CSR
  void mtx_generate(int argc, char *argv[], int start_of_matrix_generation_args, int verbose);

  /// A function that counts how many vector elements are needed for an SpMV
  /// itteration
  int count_transactions();

  /// Generalized Alloc function for vectors x, y(=0)
  void vec_alloc(VALUE_TYPE_AX *x);

  /// Generalized copy function for SpmvFormatStruct data to SAME mem_alloc
  /// return a 'format_data' struct of format 'format' allocated in 'mem_alloc'
  /// memory
  void *spmv_data_get_copy();

  /// Generalized subcopy function for SpmvFormatStruct data to SAME mem_alloc
  /// return a 'format_data' struct of format 'format' allocated in 'mem_alloc'
  /// from start with size nzc
  /// memory
  void *spmv_data_get_subcopy(int *start, int *nzc, int mode);

  /// Generalized conversion function for different mem_alloc
  void mem_convert(SpmvMemType taret_mem);

  /// Generalized conversion function for different format
  // FIXME: Add the subfunctions
  void format_convert(SpmvFormat target_format);

  /// Generalized copy function for y data to HOST mem_alloc
  VALUE_TYPE_Y *y_get_copy();

  /// Generalized  free of a format struct
  void spmv_free();

  /// Generalized  free of a lib struct
  void free_lib_struct();

  /// Initialize the operator for using cuSPARSE
  void cuSPARSE_init();

  /// Initialize the operator for using cuSP
  void cuSP_init();

  /// Initialize the operator for using mkl
  void mkl_init();

  /// Initialize the operator for using cuda CSR5
  void cuCSR5_init();

  /// Initialize the operator for using avx512 CSR5
  void avx512CSR5_init();

  /// Checks if operator elements are compatible with cuSPARSE
  void cuSPARSE_check_compatibility();

  /// Checks if operator elements are compatible with cuSP
  void cuSP_check_compatibility();

  /// Checks if operator elements are compatible with openmp
  void openmp_check_compatibility();

  /// Checks if operator elements are compatible with mkl
  void mkl_check_compatibility();

  /// Checks if operator elements are compatible with CSR5
  void cuCSR5_check_compatibility();

  /// Checks if operator elements are compatible with CSR5
  void avx512CSR5_check_compatibility();

  /// Performs csr SpMV on a cuSPARSE initialized operator
  void cuSPARSE_csr();

  /// Performs bsr SpMV on a cuSPARSE initialized operator
  void cuSPARSE_bsr();

  /// Performs hyb SpMV on a cuSPARSE initialized operator
  void cuSPARSE_hyb();

  /// Initialize the operator for using OpenMP
  void openmp_init();

  /// Performs csr SpMV on a OpenMP initialized operator
  void openmp_csr();

  /// Performs csr mkl on a OpenMP initialized operator
  void mkl_csr();

  /// Performs csr SpMV on a cuda csr5 initialized operator
  void cuCSR5_csr();

  /// Performs csr SpMV on a avx512 csr5 initialized operator
  void avx512CSR5_csr();

  /// Generalized conversion function for different format, mem_alloc.
  // FIXME: for now split into format,mem conver, too many sub-functions to
  // implement
  void op_transmute(SpmvMemType target_mem);
  void device_convert(SpmvFormat format);
  void unified_convert(SpmvFormat format);
};

#endif
