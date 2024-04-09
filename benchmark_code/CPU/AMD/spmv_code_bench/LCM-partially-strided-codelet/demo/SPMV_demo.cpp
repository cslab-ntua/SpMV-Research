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
	if (argc == 1)
	{
		std::cerr << "matrix_name,num_threads,csr_m,csr_n,csr_nnz,time,gflops,csr_mem_footprint,format_name";

		std::cerr << ",prefer_fsc,size_cutoff,col_threshold";
		std::cerr << ",SpMV Base";
		std::cerr << ",SpMV DDT Parallel Executor";
		std::cerr << ",SpMV DDT Parallel gflops";
		std::cerr << ",SPMV Analysis";
		std::cerr << "\n";
		return 0;
	}

	auto config = DDT::parseInput(argc, argv);
	long i, j;

	auto A = sym_lib::read_mtx(config.matrixPath);
	sym_lib::CSR *B = sym_lib::csc_to_csr(A);


	auto *sps = new SpMVSerial(B, A, NULLPNTR, "Baseline SpMV");
	auto spmv_baseline = sps->evaluate();
	double *sol_spmv = sps->solution();



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

	double csr_mem_footprint = B->nnz * (sizeof(double) + sizeof(int)) + (B->m+1) * sizeof(int);

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

