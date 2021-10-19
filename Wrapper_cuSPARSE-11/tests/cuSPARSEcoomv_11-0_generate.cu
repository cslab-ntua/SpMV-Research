///
/// \author Anastasiadis Petros (panastas@cslab.ece.ntua.gr)
///
/// \brief A benchmark script for SpMV implementations
///


#include <cstdio>
#include <gpu_utils.hpp>
#include <numeric>
#include <spmv_utils.hpp>
#include "cuSPARSE.hpp"
#include <iostream>
#include <fstream>

//From cuda 11 - cuSPARSE
#include <cuda_runtime_api.h> // cudaMalloc, cudaMemcpy, etc.
#include <cusparse.h>         // cusparseSpMV
#include <stdio.h>            // printf
#include <stdlib.h>           // EXIT_FAILURE

#include "nvem.hpp"


#define CHECK_CUDA(func)                                                       \
{                                                                              \
    cudaError_t status = (func);                                               \
    if (status != cudaSuccess) {                                               \
        printf("CUDA API failed at line %d with error: %s (%d)\n",             \
               __LINE__, cudaGetErrorString(status), status);                  \
        return EXIT_FAILURE;                                                   \
    }                                                                          \
}

#define CHECK_CUSPARSE(func)                                                   \
{                                                                              \
    cusparseStatus_t status = (func);                                          \
    if (status != CUSPARSE_STATUS_SUCCESS) {                                   \
        printf("CUSPARSE API failed at line %d with error: %s (%d)\n",         \
               __LINE__, cusparseGetErrorString(status), status);              \
        return EXIT_FAILURE;                                                   \
    }                                                                          \
}

/* definition to expand macro then apply to pragma message */
#define VALUE_TO_STRING(x) #x
#define VALUE(x) VALUE_TO_STRING(x)
#define VAR_NAME_VALUE(var) #var "="  VALUE(var)

/* Some example here */
#pragma message(VAR_NAME_VALUE(VALUE_TYPE_AX))
#pragma message(VAR_NAME_VALUE(VALUE_TYPE_Y))
#pragma message(VAR_NAME_VALUE(VALUE_TYPE_COMP))

//Add here any supported combinations. CUDA data types I hate you for this. 
cudaDataType CUDA_VALUE_TYPE_AX, CUDA_VALUE_TYPE_Y, CUDA_VALUE_TYPE_COMP;
void cpp_compargs_to_cuda_dtype(){
	if (std::is_same<VALUE_TYPE_AX, int8_t>::value) CUDA_VALUE_TYPE_AX = CUDA_R_8I;
	else if (std::is_same<VALUE_TYPE_AX, int>::value) CUDA_VALUE_TYPE_AX = CUDA_R_32I;
	else if (std::is_same<VALUE_TYPE_AX, float>::value) CUDA_VALUE_TYPE_AX = CUDA_R_32F;
	else if (std::is_same<VALUE_TYPE_AX, double>::value) CUDA_VALUE_TYPE_AX = CUDA_R_64F;
	else massert(0, "cpp_compargs_to_cuda_dtype: Invalid/not implemented VALUE_TYPE_AX");
	
	if (std::is_same<VALUE_TYPE_Y, int>::value) CUDA_VALUE_TYPE_Y = CUDA_R_32I;
	else if (std::is_same<VALUE_TYPE_Y, float>::value) CUDA_VALUE_TYPE_Y = CUDA_R_32F;
	else if (std::is_same<VALUE_TYPE_Y, double>::value) CUDA_VALUE_TYPE_Y = CUDA_R_64F;
	else massert(0, "cpp_compargs_to_cuda_dtype: Invalid/not implemented VALUE_TYPE_Y");
	
	if (std::is_same<VALUE_TYPE_COMP, int>::value) CUDA_VALUE_TYPE_COMP = CUDA_R_32I;
	else if (std::is_same<VALUE_TYPE_COMP, float>::value) CUDA_VALUE_TYPE_COMP = CUDA_R_32F;
	else if (std::is_same<VALUE_TYPE_COMP, double>::value) CUDA_VALUE_TYPE_COMP = CUDA_R_64F;
	else massert(0, "cpp_compargs_to_cuda_dtype: Invalid/not implemented VALUE_TYPE_COMP");
	cout << "CUDA_VALUE_TYPE_AX: " << CUDA_VALUE_TYPE_AX << ", CUDA_VALUE_TYPE_Y: " << CUDA_VALUE_TYPE_Y << ", CUDA_VALUE_TYPE_COMP: " << CUDA_VALUE_TYPE_COMP << endl;
}

int main(int argc, char **argv) {
	/// Check Input
	massert(argc == 9,
		  "Incorrect arguments.\nUsage:  ./Executable logfilename Mpakos_7_parameters_with_spaces");
		            
	// Set/Check for device
	int device_id = 0;
	cudaSetDevice(device_id);
	cudaGetDevice(&device_id);
	cudaDeviceProp deviceProp;
	cudaGetDeviceProperties(&deviceProp, device_id);
	cout << "Device [" <<  device_id << "] " << deviceProp.name << ", " << " @ " << deviceProp.clockRate * 1e-3f << "MHz. " << endl;

	char *outfile = argv[1];
	int start_of_matrix_generation_args = 2, verbose = 0;
#ifdef DDEBUG
	verbose = 1;
#endif
	double cpu_timer, gpu_timer, exc_timer = 0, trans_timer[4] = {0, 0, 0, 0}, gflops_s = -1.0;

	/// Mix C & C++ file inputs, because...?
	ofstream foutp;
	foutp.open(outfile, ios::out | ios::app ); 
	massert(foutp.is_open() , "Invalid output File");
	// print_devices();

	exc_timer = csecond();
	SpmvOperator op(argc, argv, start_of_matrix_generation_args, verbose);
	exc_timer = csecond() - exc_timer;

	fprintf(stdout,
	  "File=%s ( distribution = %s, placement = %s, diagonal_factor = %lf, seed = %d ) -> Input time=%lf s\n\t\
	  nr_rows(m)=%d, nr_cols(n)=%d, bytes = %d, density =%lf, mem_footprint = %lf MB, mem_range=%s\n\t\
	  nr_nnzs=%d, avg_nnz_per_row=%lf, std_nnz_per_row=%lf\n\t\
	  avg_bandwidth=%lf, std_bandwidth = %lf\n\t\
	  avg_scattering=%lf, std_scattering=%lf\n",
	  op.mtx_name, op.distribution, op.placement, op.diagonal_factor, op.seed, exc_timer, 
	  op.m, op.n, op.bytes, op.density, op.A_mem_footprint, op.mem_range,
	  op.nz, op.avg_nz_row,  op.std_nz_row, 
	  op.avg_bandwidth,  op.std_bandwidth, 
	  op.avg_scattering,  op.std_scattering );
	  
	VALUE_TYPE_AX *x = (VALUE_TYPE_AX *)malloc(op.n * sizeof(VALUE_TYPE_AX));
	VALUE_TYPE_Y *out = (VALUE_TYPE_Y *)calloc(op.m, sizeof(VALUE_TYPE_Y));
	vec_init_rand<VALUE_TYPE_AX>(x, op.n, 0);
	op.vec_alloc((VALUE_TYPE_AX*)x);

	op.cuSPARSE_init();
	cpp_compargs_to_cuda_dtype();
	SpmvCsrData *data = (SpmvCsrData *)op.format_data;
		   
    VALUE_TYPE_COMP alpha = 1.0;
    VALUE_TYPE_COMP beta = 0.0;
    //--------------------------------------------------------------------------
    // Device memory management
    int   *dA_csrOffsets, *dA_columns;
    VALUE_TYPE_AX *dA_values, *dX;
    VALUE_TYPE_Y *dY;
    CHECK_CUDA( cudaMalloc((void**) &dA_csrOffsets,
                           (op.m + 1) * sizeof(int)) )
    CHECK_CUDA( cudaMalloc((void**) &dA_columns, op.nz * sizeof(int))        )
    CHECK_CUDA( cudaMalloc((void**) &dA_values,  op.nz * sizeof(VALUE_TYPE_AX))      )
    CHECK_CUDA( cudaMalloc((void**) &dX,         op.n * sizeof(VALUE_TYPE_AX)) )
    CHECK_CUDA( cudaMalloc((void**) &dY,         op.m * sizeof(VALUE_TYPE_Y)) )

    CHECK_CUDA( cudaMemcpy(dA_csrOffsets, data->rowPtr,
                           (op.m + 1) * sizeof(int),
                           cudaMemcpyHostToDevice) )
    CHECK_CUDA( cudaMemcpy(dA_columns, data->colInd, op.nz * sizeof(int),
                           cudaMemcpyHostToDevice) )
    CHECK_CUDA( cudaMemcpy(dA_values, data->values, op.nz * sizeof(VALUE_TYPE_AX),
                           cudaMemcpyHostToDevice) )
    CHECK_CUDA( cudaMemcpy(dX, op.x, op.n * sizeof(VALUE_TYPE_AX),
                           cudaMemcpyHostToDevice) )
    CHECK_CUDA( cudaMemcpy(dY, op.y, op.m * sizeof(VALUE_TYPE_Y),
                           cudaMemcpyHostToDevice) )
    //--------------------------------------------------------------------------
    // CUSPARSE APIs
    cusparseHandle_t     handle = NULL;
    cusparseSpMatDescr_t matA;
    cusparseDnVecDescr_t vecX, vecY;
    void*                dBuffer    = NULL;
    size_t               bufferSize = 0;
    CHECK_CUSPARSE( cusparseCreate(&handle) )
    // Convert CSR input to COO
    int   *dA_rows; 
    CHECK_CUDA( cudaMalloc((void**) &dA_rows,
                           op.nz * sizeof(int)) )
    CHECK_CUSPARSE( cusparseXcsr2coo(handle, dA_csrOffsets, op.nz, op.m, dA_rows, CUSPARSE_INDEX_BASE_ZERO))
    CHECK_CUDA( cudaFree(dA_csrOffsets) )
    // Create sparse matrix A in Coo format
    CHECK_CUSPARSE( cusparseCreateCoo(&matA, op.m, op.n, op.nz,
                                      dA_rows, dA_columns, dA_values,
                                      CUSPARSE_INDEX_32I,
                                      CUSPARSE_INDEX_BASE_ZERO, CUDA_VALUE_TYPE_AX) )
    // Create dense vector X
    CHECK_CUSPARSE( cusparseCreateDnVec(&vecX, op.n, dX, CUDA_VALUE_TYPE_AX) )
    // Create dense vector y
    CHECK_CUSPARSE( cusparseCreateDnVec(&vecY, op.m, dY, CUDA_VALUE_TYPE_Y) )
    // allocate an external buffer if needed
    CHECK_CUSPARSE( cusparseSpMV_bufferSize(
                                 handle, CUSPARSE_OPERATION_NON_TRANSPOSE,
                                 &alpha, matA, vecX, &beta, vecY, CUDA_VALUE_TYPE_COMP,
                                 CUSPARSE_MV_ALG_DEFAULT, &bufferSize) )
    CHECK_CUDA( cudaMalloc(&dBuffer, bufferSize) )
    
#ifdef TEST
  
	fprintf(stdout,"Serial-CSR: ");
	op.timer = csecond();
	spmv_csr<VALUE_TYPE_AX, VALUE_TYPE_Y, VALUE_TYPE_COMP>(data->rowPtr, data->colInd, (VALUE_TYPE_AX *) data->values, (VALUE_TYPE_AX *)op.x,
		   (VALUE_TYPE_Y*) out, op.m);
	op.timer = csecond() - op.timer;
	report_results(op.timer * NR_ITER, op.flops, op.bytes);
	fprintf(stdout,"\n");

	fprintf(stdout,"\nRunning tests.. \n");


	fprintf(stdout,"Testing cuSPARSE_coo...\t");
    // execute SpMV
    CHECK_CUSPARSE( cusparseSpMV(handle, CUSPARSE_OPERATION_NON_TRANSPOSE,
                                 &alpha, matA, vecX, &beta, vecY, CUDA_VALUE_TYPE_COMP,
                                 CUSPARSE_MV_ALG_DEFAULT, dBuffer) )
	cudaDeviceSynchronize();
	// device result check
    CHECK_CUDA( cudaMemcpy(op.y, dY, op.m * sizeof(VALUE_TYPE_Y),
                           cudaMemcpyDeviceToHost) )
	check_result<VALUE_TYPE_Y>((VALUE_TYPE_Y*)op.y, out, op.m);


#endif

	// Warmup
	for (int i = 0; i < 100; i++)     CHECK_CUSPARSE( cusparseSpMV(handle, CUSPARSE_OPERATION_NON_TRANSPOSE,
                                 &alpha, matA, vecX, &beta, vecY, CUDA_VALUE_TYPE_COMP,
                                 CUSPARSE_MV_ALG_DEFAULT, dBuffer) )
	cudaDeviceSynchronize();

	// Run cuSPARSE coo
	fprintf(stdout,"Timing cuSPARSE_coo...\n");
	char powa_filename[256];
	sprintf(powa_filename, "cuSPARSECoomv_11-0_generate_cudatype-%d_format-COO.log", CUDA_VALUE_TYPE_AX);
	NvemStartMeasure(device_id, powa_filename, 1); // Set to 1 for NVEM log messages. 
	op.timer = csecond();
	for (int i = 0; i < NR_ITER; i++) {
			CHECK_CUSPARSE( cusparseSpMV(handle, CUSPARSE_OPERATION_NON_TRANSPOSE,
                                 &alpha, matA, vecX, &beta, vecY, CUDA_VALUE_TYPE_COMP,
                                 CUSPARSE_MV_ALG_DEFAULT, dBuffer) )
			cudaDeviceSynchronize();
	}
	cudaCheckErrors();
	op.timer = (csecond() - op.timer)/NR_ITER;
	unsigned int extra_itter = 0;
	if (op.timer*NR_ITER < 1.0){
		extra_itter = ((unsigned int) 1.0/op.timer) - NR_ITER;
		fprintf(stdout,"Performing extra %d itter for more power measurments (min benchmark time : 1s)...\n", extra_itter);
		for (int i = 0; i <  extra_itter; i++) {
			CHECK_CUSPARSE( cusparseSpMV(handle, CUSPARSE_OPERATION_NON_TRANSPOSE,
                                 &alpha, matA, vecX, &beta, vecY, CUDA_VALUE_TYPE_COMP,
                                 CUSPARSE_MV_ALG_DEFAULT, dBuffer) )
			cudaDeviceSynchronize();
		}
		cudaCheckErrors();
	}
	NvemStats_p nvem_data = NvemStopMeasure(device_id, "Energy measure cuSPARSEcoomv_11-0_generate");
	gflops_s = op.flops*1e-9/op.timer;
	double W_avg = nvem_data->W_avg, J_estimated = nvem_data->J_estimated/(NR_ITER+extra_itter); 
	fprintf(stdout, "cuSPARSE_coo11: t = %lf ms (%lf Gflops/s ). Average Watts = %lf, Estimated Joules = %lf\n", op.timer*1000, gflops_s, W_avg, J_estimated);
	foutp << op.mtx_name << "," << op.distribution << "," << op.placement << "," << op.diagonal_factor << "," << op.seed <<
	"," << op.m << "," << op.n << "," << op.nz << "," << op.density << 
	"," << op.A_mem_footprint << "," << op.mem_range << "," << op.avg_nz_row << "," << op.std_nz_row <<
	"," << op.avg_bandwidth << "," << op.std_bandwidth <<
	"," << op.avg_scattering << "," << op.std_scattering <<
	"," << "cuSPARSE_coo11" <<  "," << op.timer << "," << gflops_s << "," << W_avg <<  "," << J_estimated << endl;

    // destroy matrix/vector descriptors
    CHECK_CUSPARSE( cusparseDestroySpMat(matA) )
    CHECK_CUSPARSE( cusparseDestroyDnVec(vecX) )
    CHECK_CUSPARSE( cusparseDestroyDnVec(vecY) )
    CHECK_CUSPARSE( cusparseDestroy(handle) )
    //--------------------------------------------------------------------------

    // device memory deallocation
    CHECK_CUDA( cudaFree(dBuffer) )
    CHECK_CUDA( cudaFree(dA_columns) )
    CHECK_CUDA( cudaFree(dA_values) )
    CHECK_CUDA( cudaFree(dX) )
    CHECK_CUDA( cudaFree(dY) )
    
	foutp.close();
	return EXIT_SUCCESS;

}
