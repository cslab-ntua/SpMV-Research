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

#include "nvmlPower.hpp"

int main(int argc, char **argv) {
	/// Check Input
	massert(argc == 3,
	  "Incorrect arguments.\nUsage:\t./Executable logfilename Matrix_name.mtx");
	  
	// Set/Check for device
	int device_id = 1;
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
	  nr_rows(m)=%d, nr_cols(n)=%d, bytes = %d, density =%lf\n\t\
	  nr_nnzs=%d, avg_nnz_per_row=%lf, std_nnz_per_row=%lf\n\t\
	  avg_bandwidth=%lf, std_bandwidth = %lf\n\t\
	  avg_scattering=%lf, std_scattering=%lf\n",
	  op.mtx_name, op.distribution, op.placement, op.diagonal_factor, op.seed, exc_timer, 
	  op.m, op.n, op.bytes, op.density, 
	  op.nz, op.avg_nz_row,  op.std_nz_row, 
	  op.avg_bandwidth,  op.std_bandwidth, 
	  op.avg_scattering,  op.std_scattering );
		
	VALUE_TYPE *x = (VALUE_TYPE *)malloc(op.n * sizeof(VALUE_TYPE));
	VALUE_TYPE *out = (VALUE_TYPE *)malloc(op.m * sizeof(VALUE_TYPE));
	vec_init_rand<VALUE_TYPE>(x, op.n, 0);
	op.vec_alloc((VALUE_TYPE*)x);

	SpmvOperator cuSPARSE_op(op);
	cuSPARSE_op.cuSPARSE_init();
  
#ifdef TEST
  
	fprintf(stdout,"Serial-CSR: ");
	op.timer = csecond();
	SpmvCsrData *data = (SpmvCsrData *)op.format_data;
	spmv_csr<VALUE_TYPE>(data->rowPtr, data->colInd, (VALUE_TYPE *) data->values, (VALUE_TYPE *)op.x,
		   (VALUE_TYPE *) op.y, op.m);
	op.timer = csecond() - op.timer;
	report_results(op.timer * NR_ITER, op.flops, op.bytes);
	fprintf(stdout,"\n");

	fprintf(stdout,"\nRunning tests.. \n");


	fprintf(stdout,"Testing cuSPARSE_csr...\t");
	cuSPARSE_op.format_convert(SPMV_FORMAT_CSR);
	cuSPARSE_op.cuSPARSE_csr();
	cudaDeviceSynchronize();
	check_result<VALUE_TYPE>((VALUE_TYPE *)cuSPARSE_op.y, (VALUE_TYPE *)op.y, cuSPARSE_op.m);


#endif

	/// Run cuSPARSE csr
	cuSPARSE_op.format_convert(SPMV_FORMAT_CSR);

	// Warmup
	for (int i = 0; i < 100; i++) cuSPARSE_op.cuSPARSE_csr();
	cudaDeviceSynchronize();

	// Run cuSPARSE csr
	fprintf(stdout,"Timing cuSPARSE_csr...\n");
	nvmlAPIRun();
	cuSPARSE_op.timer = csecond();
	for (int i = 0; i < NR_ITER; i++) {
			cuSPARSE_op.cuSPARSE_csr();
			cudaDeviceSynchronize();
	}
	cudaCheckErrors();
	cuSPARSE_op.timer = (csecond() - cuSPARSE_op.timer)/NR_ITER;
	if (cuSPARSE_op.timer*NR_ITER < 1.0){
		unsigned int extra_itter = ((unsigned int) 1.0/cuSPARSE_op.timer) - NR_ITER;
		fprintf(stdout,"Performing extra %d itter for more power measurments (min benchmark time : 1s)...\n", extra_itter);
		for (int i = 0; i <  extra_itter; i++) {
			cuSPARSE_op.cuSPARSE_csr();
			cudaDeviceSynchronize();
		}
		cudaCheckErrors();
	}
	nvmlAPIEnd();
	gflops_s = cuSPARSE_op.flops*1e-9/cuSPARSE_op.timer;
	fprintf(stdout, "cuSPARSE_csr: t = %lf ms (%lf Gflops/s )\n", cuSPARSE_op.timer, gflops_s);
	foutp << cuSPARSE_op.mtx_name << "," << cuSPARSE_op.distribution << "," << cuSPARSE_op.placement << "," << cuSPARSE_op.diagonal_factor << "," << cuSPARSE_op.seed <<
	"," << cuSPARSE_op.m << "," << cuSPARSE_op.n << "," << cuSPARSE_op.density << 
	"," << cuSPARSE_op.nz << "," << cuSPARSE_op.avg_nz_row << "," << cuSPARSE_op.std_nz_row <<
	"," << cuSPARSE_op.avg_bandwidth << "," << cuSPARSE_op.std_bandwidth <<
	"," << cuSPARSE_op.avg_scattering << "," << cuSPARSE_op.std_scattering <<
	"," << "cuSPARSE_csr" <<  "," << cuSPARSE_op.timer << "," << gflops_s << "\n";

	foutp.close();

}
