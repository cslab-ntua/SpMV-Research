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
	massert(argc == 4 || argc == 3,
	  "Incorrect arguments.\nUsage:\t./Executable logfilename Matrix_name.mtx [blockdim]");
	  
	// Set/Check for device
	int device_id = 0, blockdim = 4; 
	cudaSetDevice(device_id);
	cudaGetDevice(&device_id);
	cudaDeviceProp deviceProp;
	cudaGetDeviceProperties(&deviceProp, device_id);
	cout << "Device [" <<  device_id << "] " << deviceProp.name << ", " << " @ " << deviceProp.clockRate * 1e-3f << "MHz. " << endl;

	char *name = argv[2], *outfile = argv[1];
	if (argc == 4) blockdim = atoi(argv[3]);
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
	  "File=%s ( distribution = %s, placement = %s, seed = %d ) -> Input time=%lf s\n\t\
	  nr_rows(m)=%d, nr_cols(n)=%d, bytes = %d, density =%lf, mem_footprint = %lf MB, mem_range=%s\n\t\
	  nr_nnzs=%d, avg_nnz_per_row=%lf, std_nnz_per_row=%lf\n\t\
	  avg_bw=%lf, std_bw = %lf, avg_bw_scaled = %lf, std_bw_scaled = %lf\n\t\
	  avg_sc=%lf, std_sc=%lf, avg_sc_scaled = %lf, std_sc_scaled = %lf\
	  \n\t, skew =%lf, avg_num_neighbours =%lf, cross_row_similarity =%lf\n",
	  op.mtx_name, op.distribution, op.placement, op.seed, exc_timer, 
	  op.m, op.n, op.bytes, op.density, op.mem_footprint, op.mem_range,
	  op.nz, op.avg_nnz_per_row,  op.std_nnz_per_row, 
	  op.avg_bw,  op.std_bw, op.avg_bw_scaled, op.std_bw_scaled,
	  op.avg_sc,  op.std_sc, op.avg_sc_scaled, op.std_sc_scaled, 
	  op.skew, op.avg_num_neighbours, op.cross_row_similarity);
		
	VALUE_TYPE_AX *x = (VALUE_TYPE_AX *)malloc(op.n * sizeof(VALUE_TYPE_AX));
	VALUE_TYPE_AX *out = (VALUE_TYPE_AX *)malloc(op.m * sizeof(VALUE_TYPE_AX));
	vec_init_rand<VALUE_TYPE_AX>(x, op.n, 0);
	op.vec_alloc((VALUE_TYPE_AX*)x);

	SpmvOperator bsr_op(op);
	bsr_op.cuSPARSE_init();
  	/// Convert to cuSPARSE bsr
	bsr_op.bsr_blockDim = blockdim;
	bsr_op.format_convert(SPMV_FORMAT_BSR);
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


	fprintf(stdout,"Testing cuSPARSE_bsr...\t");

	bsr_op.cuSPARSE_bsr();
	cudaDeviceSynchronize();
	cudaMemcpy(out, bsr_op.y, op.m * sizeof(VALUE_TYPE_AX), cudaMemcpyDeviceToHost);
	check_result<VALUE_TYPE_AX>((VALUE_TYPE_AX *)out, (VALUE_TYPE_AX *)op.y, bsr_op.m);


#endif
	
	// Warmup
	for (int i = 0; i < 100; i++) bsr_op.cuSPARSE_bsr();
	cudaDeviceSynchronize();

	// Run cuSPARSE csr
	fprintf(stdout,"Timing cuSPARSE_bsr...\n");
	short CUDA_VALUE_TYPE_AX;
	if (std::is_same<VALUE_TYPE_AX, float>::value)  CUDA_VALUE_TYPE_AX = 0;
	else if (std::is_same<VALUE_TYPE_AX, double>::value) CUDA_VALUE_TYPE_AX = 1;
	char powa_filename[256];
	sprintf(powa_filename, "cuSPARSE_bsrmv_9-2_mtx_cudatype-%d_format-CSR.log", CUDA_VALUE_TYPE_AX);
	NvemStartMeasure(device_id, powa_filename, 0); // Set to 1 for NVEM log messages. 
	op.timer = csecond();
	for (int i = 0; i < NR_ITER; i++) {
			bsr_op.cuSPARSE_bsr();
			cudaDeviceSynchronize();
	}
	cudaCheckErrors();
	op.timer = (csecond() - op.timer)/NR_ITER;
	unsigned int extra_itter = 0; 
	if (op.timer*NR_ITER < 1.0){
		extra_itter = ((unsigned int) 1.0/op.timer) - NR_ITER;
		fprintf(stdout,"Performing extra %d itter for more power measurments (min benchmark time : 1s)...\n", extra_itter);
		for (int i = 0; i <  extra_itter; i++) {
			bsr_op.cuSPARSE_bsr();
			cudaDeviceSynchronize();
		}
		cudaCheckErrors();
	}
	NvemStats_p nvem_data = NvemStopMeasure(device_id, "Energy measure cuSPARSE_bsrmv_9-2_mtx");
	gflops_s = op.flops*1e-9/op.timer;
	double W_avg = nvem_data->W_avg, J_estimated = nvem_data->J_estimated/(NR_ITER+extra_itter); 
	fprintf(stdout, "cuSPARSE_bsr9-2: t = %lf ms (%lf Gflops/s ). Average Watts = %lf, Estimated Joules = %lf\n", op.timer*1000, gflops_s, W_avg, J_estimated);
	foutp << op.mtx_name << "," << op.distribution << "," << op.placement << "," << op.seed <<
	"," << op.m << "," << op.n << "," << op.nz << "," << op.density << 
	"," << op.mem_footprint << "," << op.mem_range << "," << op.avg_nnz_per_row << "," << op.std_nnz_per_row <<
	"," << op.avg_bw << "," << op.std_bw <<
	"," << op.avg_bw_scaled << "," << op.std_bw_scaled <<
	"," << op.avg_sc << "," << op.std_sc <<
	"," << op.avg_sc_scaled << "," << op.std_sc_scaled <<
	"," << op.skew << "," << op.avg_num_neighbours << "," << op.cross_row_similarity <<
	"," << "cuSPARSE_bsr9-2_" << blockdim <<  "," << op.timer << "," << gflops_s << "," << W_avg <<  "," << J_estimated << endl;

	foutp.close();

}
