//
// Created by kazem on 7/13/21.
//
#include "SPMV_demo_utils.h"

#include "DDTDef.h"
#include <def.h>
#include <sparse_io.h>
#include <sparse_utilities.h>
#include <omp.h>

#include <mmio.h>
#include "mmio.cpp"

#ifdef __cplusplus
extern "C"{
#endif
	#include "artificial_matrix_generation.h"
#ifdef __cplusplus
}
#endif


namespace sym_lib {

	CSC * read_mtx(std::string fname) {
		FILE *mf = fopen(fname.c_str(), "r");
		if (!mf) exit(1);

		MM_typecode mcode;
		if (mm_read_banner(mf, &mcode) != 0) {
			std::cerr << "Error processing matrix banner\n";
			fclose(mf);
			return nullptr;
		}

		int m, n, nnz;
		if (mm_read_mtx_crd_size(mf, &m, &n, &nnz) != 0) exit(1);
		int *I = new int[2*nnz]();
		int *J = new int[2*nnz]();
		int *degrees = new int[n+1]();
		double *X = new double[2*nnz]();

		int stype = mm_is_symmetric(mcode) ? -1 : 0;

		long i, j;

		#pragma omp parallel for
		for (long i=0;i<=n;i++)
		{
			degrees[i] = 0;
		}
		#pragma omp parallel for
		for (long i=0;i<nnz;i++)
		{
			I[i] = 0;
			J[i] = 0;
			X[i] = 0;
		}

		// Copy matrix data into COO format
		int row, col, pos;
		double val;
		j = 0;
		for (i = 0; i < nnz; i++) {
			if (fscanf(mf, "%d %d %lg\n", &row, &col, &val) == EOF) {
				std::cerr << "Failed to load matrix at " << i + 1 << "th element\n";
				fclose(mf);
				return nullptr;
			}
			row--;
			col--;

			I[j] = row;
			J[j] = col;
			X[j] = val;
			// printf("%d %d %g\n", row, col, val);
			degrees[col]++;
			j++;

			if (stype && (row != col))
			{
				I[j] = col;
				J[j] = row;
				X[j] = val;
				degrees[row]++;
				j++;
			}

		}
		nnz = j;

		CSC *A = new CSC(m, n, nnz);
		A->stype = 0;

		#pragma omp parallel for
		for (long i=0;i<=n;i++)
		{
			A->p[i] = 0;
		}
		#pragma omp parallel for
		for (long i=0;i<nnz;i++)
		{
			A->i[i] = 0;
			A->x[i] = 0;
		}

		for (i=1;i<=n;i++)
			degrees[i] += degrees[i-1];
		for (i=0;i<=n;i++)
			A->p[i] = degrees[i];

		for (i=0;i<nnz;i++)
		{
			row = I[i];
			col = J[i];
			val = X[i];
			pos = A->p[col] - 1;
			A->i[pos] = row;
			A->x[pos] = val;
			A->p[col]--;
		}
		// printf("%d %d\n", A->p[0], A->p[1]);

		A->p[n] = nnz;

		A->m = m;
		A->n = n;
		A->nnz = nnz;

		delete[]I;
		delete[]J;
		delete[]X;
		delete[]degrees;

		return A;
	}

CSC * read_mtx2(struct csr_matrix * csr) {
		int m, n, nnz;
		// if (mm_read_mtx_crd_size(mf, &m, &n, &nnz) != 0) exit(1);
		m = csr->nr_rows;
		n = csr->nr_cols;
		nnz = csr->nr_nzeros;
		int *I = new int[2*nnz]();
		int *J = new int[2*nnz]();
		int *degrees = new int[n+1]();
		double *X = new double[2*nnz]();

		int stype = 0;

		long i, j;

		#pragma omp parallel for
		for (long i=0;i<=n;i++)
		{
			degrees[i] = 0;
		}
		#pragma omp parallel for
		for (long i=0;i<nnz;i++)
		{
			I[i] = 0;
			J[i] = 0;
			X[i] = 0;
		}

		// Copy matrix data into COO format
		int row, col, pos;
		double val;
		j = 0;
		for(int row = 0; row < csr->nr_rows; row++){
			for(int idx = csr->row_ptr[row]; idx < csr->row_ptr[row+1]; idx++){
				col = csr->col_ind[idx];  // Column index
				val = csr->values[idx]; // Non-zero value

				// COO format: append row, column, and value
				I[j] = row;
				J[j] = col;
				X[j] = val;
				degrees[col]++;
				j++;
			}
		}
		// for (i = 0; i < nnz; i++) {
		// 	if (fscanf(mf, "%d %d %lg\n", &row, &col, &val) == EOF) {
		// 		std::cerr << "Failed to load matrix at " << i + 1 << "th element\n";
		// 		fclose(mf);
		// 		return nullptr;
		// 	}
		// 	row--;
		// 	col--;
		// 	I[j] = row;
		// 	J[j] = col;
		// 	X[j] = val;
		// 	// printf("%d %d %g\n", row, col, val);
		// 	degrees[col]++;
		// 	j++;
		// }
		nnz = j;

		CSC *A = new CSC(m, n, nnz);
		A->stype = 0;

		#pragma omp parallel for
		for (long i=0;i<=n;i++)
		{
			A->p[i] = 0;
		}
		#pragma omp parallel for
		for (long i=0;i<nnz;i++)
		{
			A->i[i] = 0;
			A->x[i] = 0;
		}

		for (i=1;i<=n;i++)
			degrees[i] += degrees[i-1];
		for (i=0;i<=n;i++)
			A->p[i] = degrees[i];

		for (i=0;i<nnz;i++)
		{
			row = I[i];
			col = J[i];
			val = X[i];
			pos = A->p[col] - 1;
			A->i[pos] = row;
			A->x[pos] = val;
			A->p[col]--;
		}
		// printf("%d %d\n", A->p[0], A->p[1]);

		A->p[n] = nnz;

		A->m = m;
		A->n = n;
		A->nnz = nnz;

		delete[]I;
		delete[]J;
		delete[]X;
		delete[]degrees;

		return A;
	}

	CSR* csc_to_csr(CSC* A) {
		// count row entries to generate row ptr
		int nnz = A->p[A->n];
		int *rowCnt = new int[A->m]();
		for (int i = 0; i < nnz; i++)
			rowCnt[A->i[i]]++;

		CSR *B = new CSR(A->m,A->n,A->nnz,A->is_pattern);
		int *rowptr = B->p; //new int[nrow + 1]();
		size_t ncol = B->n;
		size_t nrow = B->m;
		int counter = 0;
		#pragma omp parallel for
		for (long i=0;i<=nrow;i++)
		{
			B->p[i] = 0;
		}
		#pragma omp parallel for
		for (long i=0;i<nnz;i++)
		{
			B->i[i] = 0;
			B->x[i] = 0;
		}
		for (int i = 0; i < (int)nrow; i++) {
			rowptr[i] = counter;
			counter += rowCnt[i];
		}
		rowptr[nrow] = nnz;

		int *colind = B->i;
		double *values = B->x;

		memset(rowCnt, 0, sizeof(int) * nrow);
		for (int i = 0; i < (int)ncol; i++) {
			for (int j = A->p[i]; j < A->p[i + 1]; j++) {
				int row = A->i[j];
				int index = rowptr[row] + rowCnt[row];
				colind[index] = i;
				if(!B->is_pattern)
					values[index] = A->x[j];
				rowCnt[row]++;
			}
		}
		delete[]rowCnt;
		return B;
	}
}


using namespace sparse_avx;

int main(int argc, char *argv[]) {
	int artificial_flag = atoi(getenv("USE_ARTIFICIAL_MATRICES"));


	if(artificial_flag == 0){
		if (argc == 1){
			// std::cerr << "matrix_name,num_threads,csr_m,csr_n,csr_nnz,time,gflops,csr_mem_footprint,format_name";

			// std::cerr << ",prefer_fsc,size_cutoff,col_threshold";
			// std::cerr << ",SpMV Base";
			// std::cerr << ",SpMV DDT Parallel Executor";
			// std::cerr << ",SpMV DDT Parallel gflops";
			// std::cerr << ",SPMV Analysis";

			std::cerr << "matrix_name,num_threads,csr_m,csr_n,csr_nnz,time,gflops,csr_mem_footprint,W_avg,J_estimated,format_name,m,n,nnz,mem_footprint,mem_ratio,num_loops,spmv_mae,spmv_max_ae,spmv_mse,spmv_mape,spmv_smape,spmv_lnQ_error,spmv_mlare,spmv_gmare";
			std::cerr << "\n";
			return 0;
		}

		auto config = DDT::parseInput(argc, argv);
		long i, j;

		auto A = sym_lib::read_mtx(config.matrixPath);
		sym_lib::CSR *B = sym_lib::csc_to_csr(A);


		// auto *sps = new SpMVSerial(B, A, NULLPNTR, "Baseline SpMV");
		// auto spmv_baseline = sps->evaluate();
		// double *sol_spmv = sps->solution();
		double *sol_spmv;

		auto nThread = config.nThread;

		config.nThread = nThread;
		auto *ddtspmvmt = new SpMVDDT(B, A, sol_spmv, config, "SpMV DDT MT");
		ddtspmvmt->set_num_threads(config.nThread);
		auto ddt_execmt = ddtspmvmt->evaluate();
		auto ddt_analysis = ddtspmvmt->get_analysis_bw();
		delete ddtspmvmt;

		double gflops;

		std::cerr << config.matrixPath;
		std::cerr << "," << config.nThread;
		std::cerr << "," << B->m;
		std::cerr << "," << B->n;
		std::cerr << "," << B->nnz;

		std::cerr << "," << ddt_execmt.elapsed_time;
		gflops = B->nnz / ddt_execmt.elapsed_time * 2 * 1e-9;
		std::cerr << "," << gflops;
		// std::cerr << "GFLOPS = " << gflops << std::endl;

		double csr_mem_footprint = (B->nnz * (sizeof(double) + sizeof(int)) + (B->m+1) * sizeof(int))/(1024*1024.0);

		std::cerr << "," << csr_mem_footprint;
		std::cerr << "," << "0";
		std::cerr << "," << "0";
		std::cerr << "," << "LCM";

		std::cerr << "," << "0";
		std::cerr << "," << "0";
		std::cerr << "," << "0";
		std::cerr << "," << "0";
		std::cerr << "," << "0";
		std::cerr << "," << "0";
		std::cerr << "," << "0";
		std::cerr << "," << "0";
		std::cerr << "," << "0";
		std::cerr << "," << "0";
		std::cerr << "," << "0";
		std::cerr << "," << "0";
		std::cerr << "," << "0";
		std::cerr << "," << "0";

		// std::cerr << "," << DDT::prefer_fsc << "," << DDT::clt_width << "," << DDT::col_th;
		// std::cerr << "," << spmv_baseline.elapsed_time;

		// std::cerr << "," << ddt_execmt.elapsed_time;
		// gflops = B->nnz / ddt_execmt.elapsed_time * 2 * 1e-9;
		// std::cerr << "," << gflops;

		// std::cerr << "," << ddt_analysis.elapsed_time;
		std::cerr << "\n";

		delete A;
		delete B;
		// delete sps;

	}
	else{
		if (argc == 1){
			std::cerr << "matrix_name,distribution,placement,seed,nr_rows,nr_cols,nr_nzeros,density,mem_footprint,mem_range,avg_nnz_per_row,std_nnz_per_row,avg_bw,std_bw,avg_bw_scaled,std_bw_scaled,avg_sc,std_sc,avg_sc_scaled,std_sc_scaled,skew,avg_num_neighbours,cross_row_similarity,format_name,time,gflops,W_avg,J_estimated";

			std::cerr << "\n";
			return 0;
		}

		// Reading and converting arguments
		long nr_rows = atol(argv[1]);                // Convert to long
		long nr_cols = atol(argv[2]);                // Convert to long
		double avg_nnz_per_row = atof(argv[3]);      // Convert to double
		double std_nnz_per_row = atof(argv[4]);      // Convert to double
		char *distribution = argv[5];                // char* (string)
		char *placement = argv[6];                   // char* (string)
		double bw = atof(argv[7]);                   // Convert to double
		double skew = atof(argv[8]);                 // Convert to double
		double avg_num_neighbours = atof(argv[9]);   // Convert to double
		double cross_row_similarity = atof(argv[10]);// Convert to double
		long seed = atol(argv[11]);                  // Convert to long

		struct csr_matrix * csr;
		csr = artificial_matrix_generation(nr_rows, nr_cols, avg_nnz_per_row, std_nnz_per_row, distribution, seed, placement, bw, skew, avg_num_neighbours, cross_row_similarity);
		
		auto config = DDT::parseInput(argc, argv);
		long i, j;

		// don't mess with it now... feed it a CSR matrix -> convert it to COO -> convert it to CSC... anyway, it's a mess
		auto A = sym_lib::read_mtx2(csr);
		sym_lib::CSR *B = sym_lib::csc_to_csr(A);


		// auto *sps = new SpMVSerial(B, A, NULLPNTR, "Baseline SpMV");
		// auto spmv_baseline = sps->evaluate();
		// double *sol_spmv = sps->solution();
		double *sol_spmv;

		auto *ddtspmvmt = new SpMVDDT(B, A, sol_spmv, config, "SpMV DDT MT");
		ddtspmvmt->set_num_threads(config.nThread);
		auto ddt_execmt = ddtspmvmt->evaluate();
		delete ddtspmvmt;

		double gflops = csr->nr_nzeros / ddt_execmt.elapsed_time * 2 * 1e-9;

		std::cerr << "synthetic";
		std::cerr << "," << csr->distribution;
		std::cerr << "," << csr->placement;
		std::cerr << "," << csr->seed;
		std::cerr << "," << csr->nr_rows;
		std::cerr << "," << csr->nr_cols;
		std::cerr << "," << csr->nr_nzeros;
		std::cerr << "," << csr->density;
		std::cerr << "," << csr->mem_footprint;
		std::cerr << "," << csr->mem_range;
		std::cerr << "," << csr->avg_nnz_per_row;
		std::cerr << "," << csr->std_nnz_per_row;
		std::cerr << "," << csr->avg_bw;
		std::cerr << "," << csr->std_bw;
		std::cerr << "," << csr->avg_bw_scaled;
		std::cerr << "," << csr->std_bw_scaled;
		std::cerr << "," << csr->avg_sc;
		std::cerr << "," << csr->std_sc;
		std::cerr << "," << csr->avg_sc_scaled;
		std::cerr << "," << csr->std_sc_scaled;
		std::cerr << "," << csr->skew;
		std::cerr << "," << csr->avg_num_neighbours;
		std::cerr << "," << csr->cross_row_similarity;
		std::cerr << "," << "LCM";
		std::cerr << "," << ddt_execmt.elapsed_time;
		std::cerr << "," << gflops;
		std::cerr << "," << "0";
		std::cerr << "," << "0";
		std::cerr << "\n";

		free(csr);

		delete A;
		delete B;
		// delete sps;

	}


	auto A = sym_lib::read_mtx(config.matrixPath);
	sym_lib::CSR *B = sym_lib::csc_to_csr(A);


	auto *sps = new SpMVSerial(B, A, NULLPNTR, "Baseline SpMV");
	auto spmv_baseline = sps->evaluate();
	double *sol_spmv = sps->solution();



	auto nThread = config.nThread;

	// See file 'FusionDemo.cpp' for the spmv iterations.
	config.nThread = nThread;
	auto *ddtspmvmt = new SpMVDDT(B, A, sol_spmv, config, "SpMV DDT MT");
	ddtspmvmt->set_num_threads(config.nThread);
	auto ddt_execmt = ddtspmvmt->evaluate();
	auto ddt_analysis = ddtspmvmt->get_analysis_bw();
	delete ddtspmvmt;

	double gflops;

	std::cerr << config.matrixPath;
	std::cerr << "," << config.nThread;
	std::cerr << "," << B->m;
	std::cerr << "," << B->n;
	std::cerr << "," << B->nnz;

	std::cerr << "," << ddt_execmt.elapsed_time;
	gflops = B->nnz / ddt_execmt.elapsed_time * 2 * 1e-9;
	std::cerr << "," << gflops;

	double csr_mem_footprint = ((double) B->nnz * (sizeof(double) + sizeof(int)) + (B->m+1) * sizeof(int)) / 1024.0 / 1024.0;

	std::cerr << "," << csr_mem_footprint;
	std::cerr << "," << "LCM";


	std::cerr << "," << DDT::prefer_fsc << "," << DDT::clt_width << "," << DDT::col_th;
	std::cerr << "," << spmv_baseline.elapsed_time;

	std::cerr << "," << ddt_execmt.elapsed_time;
	gflops = B->nnz / ddt_execmt.elapsed_time * 2 * 1e-9;
	std::cerr << "," << gflops;

	std::cerr << "," << ddt_analysis.elapsed_time;
	std::cerr << "\n";


	delete A;
	delete B;
	delete sps;

	return 0;
}

