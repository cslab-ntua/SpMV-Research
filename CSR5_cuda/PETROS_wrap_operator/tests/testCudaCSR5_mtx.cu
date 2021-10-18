#include <iostream>
#include <numeric>

#include "anonymouslib_cuda.h"

#include "gpu_utils.hpp"
#include "spmv_utils.hpp"
#include "cuSPARSE.hpp"
#include "nvem.hpp"

using namespace std;

#ifndef VALUE_TYPE_AX
#error
#endif

#ifndef VALUE_TYPE_Y
#error
#endif

#ifndef VALUE_TYPE_COMP
#error
#endif

#ifndef NR_ITER
#error
#endif
	
int main(int argc, char **argv) {
	/// Check Input
	massert(argc == 3,
	  "Incorrect arguments.\nUsage:\t./Executable logfilename Matrix_name.mtx");
	  
	// Set/Check for device
	int device_id = 0;
	cudaSetDevice(device_id);
	cudaGetDevice(&device_id);
	cudaDeviceProp deviceProp;
	cudaGetDeviceProperties(&deviceProp, device_id);
	cout << "Device [" <<  device_id << "] " << deviceProp.name << ", " << " @ " << deviceProp.clockRate * 1e-3f << "MHz. " << endl;

	char *name = argv[2], *outfile = argv[1];
	double cpu_timer, gpu_timer, exc_timer = 0, trans_timer[4] = {0, 0, 0, 0}, gflops_s = -1.0;

	FILE *fp = fopen(name, "r");
	massert(fp && strstr(name, ".mtx") && !fclose(fp), "Invalid .mtx File");

	/// Mix C & C++ file inputs, because...?
	ofstream foutp;
	foutp.open(outfile, ios::out | ios::app ); 
	massert(foutp.is_open() , "Invalid output File");
	// print_devices();

	exc_timer = csecond();
	SpmvOperator op(name);
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
	op.vec_alloc(x);
    
	op.cuSPARSE_init();
	SpmvCsrData *data = (SpmvCsrData *)op.format_data;
    VALUE_TYPE_COMP alpha = (VALUE_TYPE_COMP) 1.0;
    VALUE_TYPE_COMP beta = (VALUE_TYPE_COMP) 0.0;
    cout << "alpha: " << alpha << ", beta: " << beta << endl;

    srand(time(NULL));

    cout << " ( " << op.m << ", " << op.n << " ) nnz = " << op.nz << endl;

    double gb = getB<int, VALUE_TYPE_AX>(op.m, op.nz);
    double gflop = getFLOP<int>(op.nz);
    
     int err = 0;
    cudaError_t err_cuda = cudaSuccess;

    // Define pointers of matrix A, vector x and y
    int *d_csrRowPtrA;
    int *d_csrColIdxA;
    VALUE_TYPE_AX *d_csrValA;
    VALUE_TYPE_AX *dX;
    VALUE_TYPE_Y *dY;

    // Matrix A
    checkCudaErrors(cudaMalloc((void **)&d_csrRowPtrA, (op.m+1) * sizeof(int)));
    checkCudaErrors(cudaMalloc((void **)&d_csrColIdxA, op.nz  * sizeof(int)));
    checkCudaErrors(cudaMalloc((void **)&d_csrValA,    op.nz  * sizeof(VALUE_TYPE_AX)));

    checkCudaErrors(cudaMemcpy(d_csrRowPtrA, data->rowPtr, (op.m+1) * sizeof(int),   cudaMemcpyHostToDevice));
    checkCudaErrors(cudaMemcpy(d_csrColIdxA, data->colInd, op.nz  * sizeof(int),   cudaMemcpyHostToDevice));
    checkCudaErrors(cudaMemcpy(d_csrValA,    data->values,    op.nz  * sizeof(VALUE_TYPE_AX),   cudaMemcpyHostToDevice));

    // Vector x
    checkCudaErrors(cudaMalloc((void **)&dX, op.n * sizeof(VALUE_TYPE_AX)));
    checkCudaErrors(cudaMemcpy(dX, x, op.n * sizeof(VALUE_TYPE_AX), cudaMemcpyHostToDevice));

    // Vector y
    checkCudaErrors(cudaMalloc((void **)&dY, op.m  * sizeof(VALUE_TYPE_Y)));
    checkCudaErrors(cudaMemcpy(dY, out, op.m * sizeof(VALUE_TYPE_Y), cudaMemcpyHostToDevice));

    anonymouslibHandle<int, unsigned int, VALUE_TYPE_AX> A(op.m, op.n);
    err = A.inputCSR(op.nz, d_csrRowPtrA, d_csrColIdxA, d_csrValA);
    //cout << "inputCSR err = " << err << endl;

    err = A.setX(dX); // you only need to do it once!
    //cout << "setX err = " << err << endl;

    A.setSigma(ANONYMOUSLIB_AUTO_TUNED_SIGMA);

    // warmup device
    A.warmup();

    anonymouslib_timer asCSR5_timer;
    asCSR5_timer.start();

    err = A.asCSR5();

    cout << "CSR->CSR5 time = " << asCSR5_timer.stop() << " ms." << endl;
    //cout << "asCSR5 err = " << err << endl;
    
#ifdef TEST

	VALUE_TYPE_Y *out1 = (VALUE_TYPE_Y *)calloc(op.m, sizeof(VALUE_TYPE_Y));
	fprintf(stdout,"Serial-CSR: ");
	op.timer = csecond();
	spmv_csr<VALUE_TYPE_AX, VALUE_TYPE_Y, VALUE_TYPE_COMP>(data->rowPtr, data->colInd, data->values, x,
		   out1, op.m);
	op.timer = csecond() - op.timer;
	report_results(op.timer * NR_ITER, op.flops, op.bytes);
	fprintf(stdout,"\n");

	fprintf(stdout,"\nRunning tests.. \n");

	fprintf(stdout,"Testing CSR5_9_csr...\t");
    // execute SpMV
    err = A.spmv(alpha, dY);
    
	cudaDeviceSynchronize();
	// device result check
    cudaMemcpy(out, dY, op.m * sizeof(VALUE_TYPE_Y), cudaMemcpyDeviceToHost);
	cudaDeviceSynchronize();
	check_result<VALUE_TYPE_Y>((VALUE_TYPE_Y*)out, out1, op.m);
	free(out1);
#endif

    // warm up by running 50 times
    if (NR_ITER)
    {
        for (int i = 0; i < 50; i++)
            err = A.spmv(alpha, dY);
    }

    err_cuda = cudaDeviceSynchronize();

	short CUDA_VALUE_TYPE_AX;
	if (std::is_same<VALUE_TYPE_AX, float>::value)  CUDA_VALUE_TYPE_AX = 0;
	else if (std::is_same<VALUE_TYPE_AX, double>::value) CUDA_VALUE_TYPE_AX = 1;
	char powa_filename[256];
	sprintf(powa_filename, "CSR5_CUDA_SPMV_9.2_mtx_dtype-%d.log", CUDA_VALUE_TYPE_AX);
	NvemStartMeasure(device_id, powa_filename, 0); // Set to 1 for NVEM log messages. ;
	op.timer = csecond();

    // time spmv by running NR_ITER times
    for (int i = 0; i < NR_ITER; i++){
        err = A.spmv(alpha, dY);
    	err_cuda = cudaDeviceSynchronize();
    }
	op.timer = (csecond() - op.timer)/NR_ITER;
	unsigned int extra_itter = 0;
	if (op.timer*NR_ITER < 1.0){
		extra_itter = ((unsigned int) 1.0/op.timer) - NR_ITER;
		fprintf(stdout,"Performing extra %d itter for more power measurments (min benchmark time : 1s)...\n", extra_itter);
		for (int i = 0; i <  extra_itter; i++) {
        	err = A.spmv(alpha, dY);
    		err_cuda = cudaDeviceSynchronize();
		}
		cudaCheckErrors();
	}
    NvemStats_p nvem_data = NvemStopMeasure(device_id, "Energy measure CSR5_9_mtx");
	gflops_s = op.flops*1e-9/op.timer;
	double W_avg = nvem_data->W_avg, J_estimated = nvem_data->J_estimated/(NR_ITER+extra_itter); 
	fprintf(stdout, "CSR5_9: t = %lf ms (%lf Gflops/s ). Average Watts = %lf, Estimated Joules = %lf\n", op.timer*1000, gflops_s, W_avg, J_estimated);
	foutp << op.mtx_name << "," << op.distribution << "," << op.placement << "," << op.diagonal_factor << "," << op.seed <<
	"," << op.m << "," << op.n << "," << op.density << "," << op.A_mem_footprint << "," << op.mem_range << 
	"," << op.nz << "," << op.avg_nz_row << "," << op.std_nz_row <<
	"," << op.avg_bandwidth << "," << op.std_bandwidth <<
	"," << op.avg_scattering << "," << op.std_scattering <<
	"," << "CSR5_9" <<  "," << op.timer << "," << gflops_s << "," << W_avg <<  "," << J_estimated << endl;

    A.destroy();

    checkCudaErrors(cudaFree(d_csrRowPtrA));
    checkCudaErrors(cudaFree(d_csrColIdxA));
    checkCudaErrors(cudaFree(d_csrValA));
    checkCudaErrors(cudaFree(dX));
    checkCudaErrors(cudaFree(dY));
    foutp.close();
    // compare reference and anonymouslib results

    free(x);
    free(out);


    return 0;
}

