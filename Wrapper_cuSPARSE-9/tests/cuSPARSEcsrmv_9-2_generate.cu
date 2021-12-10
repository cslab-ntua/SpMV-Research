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

#include "nvem.hpp"

int main(int argc, char **argv) {
	/// Check Input
	massert(argc == 11,
		  "Incorrect arguments.\nUsage:  ./Executable logfilename Mpakos_9_parameters_with_spaces");
		            
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
	  "File=%s ( distribution = %s, placement = %s, seed = %d ) -> Input time=%lf s\n\t\
	  nr_rows(m)=%d, nr_cols(n)=%d, bytes = %d, density =%lf, mem_footprint = %lf MB, mem_range=%s\n\t\
	  nr_nnzs=%d, avg_nnz_per_row=%lf, std_nnz_per_row=%lf\n\t\
	  avg_bandwidth=%lf, std_bandwidth = %lf\n\t\
	  avg_scattering=%lf, std_scattering=%lf, bw_scaled = %lf, skew =%lf\n",
	  op.mtx_name, op.distribution, op.placement, op.seed, exc_timer, 
	  op.m, op.n, op.bytes, op.density, op.A_mem_footprint, op.mem_range,
	  op.nz, op.avg_nz_row,  op.std_nz_row, 
	  op.avg_bandwidth,  op.std_bandwidth, 
	  op.avg_scattering,  op.std_scattering, op.bw_scaled, op.skew);
		
	VALUE_TYPE_AX *x = (VALUE_TYPE_AX *)malloc(op.n * sizeof(VALUE_TYPE_AX));
	VALUE_TYPE_AX *out = (VALUE_TYPE_AX *)malloc(op.m * sizeof(VALUE_TYPE_AX));
	vec_init_rand<VALUE_TYPE_AX>(x, op.n, 0);
	op.vec_alloc((VALUE_TYPE_AX*)x);

	SpmvOperator cuSPARSE_op(op);
	cuSPARSE_op.cuSPARSE_init();
	cuSPARSE_op.format_convert(SPMV_FORMAT_CSR);
	cuSPARSE_op.mem_convert(SPMV_MEMTYPE_DEVICE);
  
#ifdef TEST
  
	fprintf(stdout,"Serial-CSR: ");
	op.timer = csecond();
	SpmvCsrData *data = (SpmvCsrData *)op.format_data;
	spmv_csr<VALUE_TYPE_AX>(data->rowPtr, data->colInd, (VALUE_TYPE_AX *) data->values, (VALUE_TYPE_AX *)op.x,
		   (VALUE_TYPE_AX *) op.y, op.m);
	op.timer = csecond() - op.timer;
	report_results(op.timer * NR_ITER, op.flops, op.bytes);
	fprintf(stdout,"\n");

	fprintf(stdout,"\nRunning tests.. \n");


	fprintf(stdout,"Testing cuSPARSE_csr...\t");

	cuSPARSE_op.cuSPARSE_csr();
	cudaDeviceSynchronize();
	cudaMemcpy(out, cuSPARSE_op.y, op.m * sizeof(VALUE_TYPE_AX), cudaMemcpyDeviceToHost);
	check_result<VALUE_TYPE_AX>((VALUE_TYPE_AX *)out, (VALUE_TYPE_AX *)op.y, cuSPARSE_op.m);


#endif
	
	// Warmup
	for (int i = 0; i < 100; i++) cuSPARSE_op.cuSPARSE_csr();
	cudaDeviceSynchronize();

	// Run cuSPARSE csr
	fprintf(stdout,"Timing cuSPARSE_csr...\n");
	short CUDA_VALUE_TYPE_AX;
	if (std::is_same<VALUE_TYPE_AX, float>::value)  CUDA_VALUE_TYPE_AX = 0;
	else if (std::is_same<VALUE_TYPE_AX, double>::value) CUDA_VALUE_TYPE_AX = 1;
	char powa_filename[256];
	sprintf(powa_filename, "cuSPARSEcsrmv_9-2_generate_cudatype-%d_format-CSR.log", CUDA_VALUE_TYPE_AX);
	NvemStartMeasure(device_id, powa_filename, 0); // Set to 1 for NVEM log messages. 
	op.timer = csecond();
	for (int i = 0; i < NR_ITER; i++) {
			cuSPARSE_op.cuSPARSE_csr();
			cudaDeviceSynchronize();
	}
	cudaCheckErrors();
	op.timer = (csecond() - op.timer)/NR_ITER;
	unsigned int extra_itter = 0; 
	if (op.timer*NR_ITER < 1.0){
		extra_itter = ((unsigned int) 1.0/op.timer) - NR_ITER;
		fprintf(stdout,"Performing extra %d itter for more power measurments (min benchmark time : 1s)...\n", extra_itter);
		for (int i = 0; i <  extra_itter; i++) {
			cuSPARSE_op.cuSPARSE_csr();
			cudaDeviceSynchronize();
		}
		cudaCheckErrors();
	}
	NvemStats_p nvem_data = NvemStopMeasure(device_id, "Energy measure cuSPARSEcsrmv_9-2_generate");
	gflops_s = op.flops*1e-9/op.timer;
	double W_avg = nvem_data->W_avg, J_estimated = nvem_data->J_estimated/(NR_ITER+extra_itter); 
	fprintf(stdout, "cuSPARSE_csr9-2: t = %lf ms (%lf Gflops/s ). Average Watts = %lf, Estimated Joules = %lf\n", op.timer*1000, gflops_s, W_avg, J_estimated);
	foutp << op.mtx_name << "," << op.distribution << "," << op.placement << "," << op.seed <<
	"," << op.m << "," << op.n << "," << op.nz << "," << op.density << 
	"," << op.A_mem_footprint << "," << op.mem_range << "," << op.avg_nz_row << "," << op.std_nz_row <<
	"," << op.avg_bandwidth << "," << op.std_bandwidth <<
	"," << op.avg_scattering << "," << op.std_scattering << "," << op.bw_scaled << "," << op.skew <<
	"," << "cuSPARSE_csr9-2" <<  "," << op.timer << "," << gflops_s << "," << W_avg <<  "," << J_estimated << endl;

	foutp.close();

}
