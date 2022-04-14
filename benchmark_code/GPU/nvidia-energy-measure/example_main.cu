#include "nvem.hpp"
#include "nvem_helpers.hpp"
#include "cublas.h"

#define ITER 10000
int main(int args, char* argv[]){
	char* f_out = "Power_data.txt";
	
	int dev_id, verbose = 1; 
	// Uncomment to run example in other than default device
	//cudaSetDevice(1);
	cudaGetDevice(&dev_id);
	

	// Do stuff in GPU with dev_id
	int N = 1<<22; 
	double *x, *y;
	x = (double*) malloc (N* sizeof(double));
	for (int i = 0; i < N; i++) x[i] = 1.0;
	y = (double*) calloc (N, sizeof(double));
	

	double *d_x, *d_y; 
	double timer = csecond();
	NvemStartMeasure(dev_id, f_out, verbose);
	cudaMalloc((void**)&d_x, N*sizeof(double));
	cudaMalloc((void**)&d_y, N*sizeof(double));
	cudaDeviceSynchronize();	
	NvemStats_p nvem_data_init = NvemStopMeasure(dev_id, "Vector Init GPU");
	
	massert(CUBLAS_STATUS_SUCCESS == cublasInit(), "cublasInit() failed");
	//Warmup - TODO: Energy measurements after warmup for a 'hot' GPU state. 
	for (int itt = 0; itt< ITER; itt++) cublasDaxpy(N, 2.0, d_x, 1, d_y, 1);
	cudaDeviceSynchronize();
	
	NvemStartMeasure(dev_id, f_out, verbose);
	massert(CUBLAS_STATUS_SUCCESS == cublasSetVector(N, sizeof(double), x, 1, d_x, 1), "cublasSetVector(x) failed");
	massert(CUBLAS_STATUS_SUCCESS == cublasSetVector(N, sizeof(double), y, 1, d_y, 1), "cublasSetVector(y) failed");
	cudaDeviceSynchronize();	
	NvemStats_p nvem_data_h2d = NvemStopMeasure(dev_id, " H2D x,y");
	
	NvemStartMeasure(dev_id, f_out, verbose);
	// Perform ITER SAXPYs on 100M elements
	for (int itt = 0; itt< ITER; itt++) cublasDaxpy(N, 2.0, d_x, 1, d_y, 1);
	cudaDeviceSynchronize();	
	NvemStats_p nvem_data_exec = NvemStopMeasure(dev_id, "Daxpy execution");
	
	NvemStartMeasure(dev_id, f_out, verbose);
	massert(CUBLAS_STATUS_SUCCESS == cublasGetVector(N, sizeof(double), d_y, 1, y, 1), "cublasGetVector(y) failed");
	massert(CUBLAS_STATUS_SUCCESS == cublasShutdown(), "cublasShutdown() failed");
	cudaDeviceSynchronize();	
	NvemStats_p nvem_data_d2h = NvemStopMeasure(dev_id, "D2H y");
	timer = csecond() - timer;
	fprintf(stdout, "Stuff executed for total of %lf s\n", timer);

	return 1;
}
