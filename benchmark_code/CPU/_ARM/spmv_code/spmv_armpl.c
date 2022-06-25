/*
 * Double precision sparse matrix-vector multiplication example
 *
 * ARMPL version 21.1 Copyright ARM 2021
 */
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
// #include <pthread.h>

#include "util.h"

#include "macros/cpp_defines.h"
#include "macros/macrolib.h"
#include "time_it.h"
#include "parallel_util.h"
// #include "pthread_functions.h"
#include "matrix_util.h"

#include <stdio.h>
#include <stdlib.h>
#include "armpl.h"

ValueType * x;
ValueType* y;
CSRArrays csr;
COOArrays coo;

int main(int argc, char **argv)
{
	__attribute__((unused)) int num_threads;
	__attribute__((unused)) double time;
	__attribute__((unused)) double time_balance;
	__attribute__((unused)) double time_warm_up;
	__attribute__((unused)) double time_after_warm_up;
	long i;
	struct csr_matrix * AM = NULL;
	armpl_status_t info;

	// Wake omp up from eternal slumber.
	#pragma omp parallel
	{
		num_threads = omp_get_max_threads();
	}
	// printf("max threads %d\n", num_threads);

	if (argc < 6)
	{
		char * file_in;
		i = 1;

		file_in = argv[i++];
		time = time_it(1,
			create_coo_matrix(file_in, &coo);
		);
		printf("time read: %lf\n", time);
		time = time_it(1,
			COO_to_CSR(&coo, &csr);
		);
		printf("time coo to csr: %lf\n", time);
	}
	else
	{
		char buf[1000];

		time = time_it(1,

			// int start_of_matrix_generation_args = 1;
			// int verbose = 1; // 0 : printf nothing
			// AM = artificial_matrix_generation(argc, argv, start_of_matrix_generation_args, verbose);
			// if (AM == NULL)
				// error("Didn't make it with the given matrix features. Try again.\n");

			long nr_rows, nr_cols, seed;
			double avg_nnz_per_row, std_nnz_per_row, bw, skew;
			double avg_num_neighbours;
			double cross_row_similarity;
			char * distribution, * placement;
			long i;

			i = 1;
			nr_rows = atoi(argv[i++]);
			nr_cols = atoi(argv[i++]);
			avg_nnz_per_row = atof(argv[i++]);
			std_nnz_per_row = atof(argv[i++]);
			distribution = argv[i++];
			placement = argv[i++];
			bw = atof(argv[i++]);
			skew = atof(argv[i++]);
			avg_num_neighbours = atof(argv[i++]);
			cross_row_similarity = atof(argv[i++]);
			seed = atoi(argv[i++]);

			AM = artificial_matrix_generation(nr_rows, nr_cols, avg_nnz_per_row, std_nnz_per_row, distribution, seed, placement, bw, skew, avg_num_neighbours, cross_row_similarity);

		);
		// printf("time generate artificial matrix: %lf\n", time);

		csr.m = AM->nr_rows;
		csr.n = AM->nr_cols;
		csr.nnz = AM->nr_nzeros;

		csr.ia = (MKL_INT *) malloc((csr.m+1 + VECTOR_ELEM_NUM) * sizeof(MKL_INT));
		#pragma omp parallel for
		for (long i=0;i<csr.m+1;i++)
			csr.ia[i] = AM->row_ptr[i];
		free(AM->row_ptr);
		AM->row_ptr = NULL;

		csr.a = (ValueType *) malloc((csr.nnz + VECTOR_ELEM_NUM) * sizeof(ValueType));
		#pragma omp parallel for
		for (long i=0;i<csr.nnz;i++)
			csr.a[i] = AM->values[i];
		free(AM->values);
		AM->values = NULL;

		csr.ja = (MKL_INT *) malloc((csr.nnz + VECTOR_ELEM_NUM) * sizeof(MKL_INT));
		#pragma omp parallel for
		for (long i=0;i<csr.nnz;i++)
			csr.ja[i] = AM->col_ind[i];
		free(AM->col_ind);
		AM->col_ind = NULL;
	}

	/* 1. Set-up local CSR structure */
	armpl_spmat_t armpl_mat;
	const int iterations = 128;
	const double alpha = 1.0, beta = 0.0;

	int creation_flags = 0;

	/* 2. Set-up Arm Performance Libraries sparse matrix object */
	info = armpl_spmat_create_csr_d(&armpl_mat, csr.m, csr.n, csr.ia, csr.ja, csr.a, creation_flags);
	if (info!=ARMPL_STATUS_SUCCESS) printf("ERROR: armpl_spmat_create_csr_d returned %d\n", info);


	time_balance = time_it(1,
		/* 3a. Supply any pertinent information that is known about the matrix */
		info = armpl_spmat_hint(armpl_mat, ARMPL_SPARSE_HINT_STRUCTURE, ARMPL_SPARSE_STRUCTURE_UNSTRUCTURED);
		if (info!=ARMPL_STATUS_SUCCESS) printf("ERROR: armpl_spmat_hint returned %d\n", info);

		/* 3b. Supply any hints that are about the SpMV calculations to be performed */
		info = armpl_spmat_hint(armpl_mat, ARMPL_SPARSE_HINT_SPMV_OPERATION, ARMPL_SPARSE_OPERATION_NOTRANS);
		if (info!=ARMPL_STATUS_SUCCESS) printf("ERROR: armpl_spmat_hint returned %d\n", info);

		info = armpl_spmat_hint(armpl_mat, ARMPL_SPARSE_HINT_SPMV_INVOCATIONS, ARMPL_SPARSE_INVOCATIONS_MANY);
		if (info!=ARMPL_STATUS_SUCCESS) printf("ERROR: armpl_spmat_hint returned %d\n", info);

		/* 4. Call an optimization process that will learn from the hints you have previously supplied */
		info = armpl_spmv_optimize(armpl_mat);
		if (info!=ARMPL_STATUS_SUCCESS) printf("ERROR: armpl_spmv_optimize returned %d\n", info);
	);
	printf("arm optimize time = %g\n", time_balance);

	/* 5. Setup input and output vectors and then do SpMV and print result.*/
	x = (ValueType *)malloc(csr.n*sizeof(ValueType));
	for (int i=0; i<csr.n; i++)
		x[i] = 1.0;
	y = (ValueType *)malloc(csr.m*sizeof(ValueType));

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// long rapl_fds_n;
	// int * rapl_fds;
	// long * rapl_max_values;
	// rapl_open_files(&rapl_fds_n, &rapl_fds, &rapl_max_values);
	// long ujoule_s[rapl_fds_n] = {0}, ujoule_e[rapl_fds_n] = {0};
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// warm up caches
	time_warm_up = time_it(1,
		info = armpl_spmv_exec_d(ARMPL_SPARSE_OPERATION_NOTRANS, alpha, armpl_mat, x, beta, y);
		if (info!=ARMPL_STATUS_SUCCESS) printf("ERROR: armpl_spmv_exec_d warm_up returned %d\n", info);
	);
	// printf("time_warm_up	   = %lf\n", time_warm_up);
	time_after_warm_up = time_it(1,
		info = armpl_spmv_exec_d(ARMPL_SPARSE_OPERATION_NOTRANS, alpha, armpl_mat, x, beta, y);
		if (info!=ARMPL_STATUS_SUCCESS) printf("ERROR: armpl_spmv_exec_d after_warm_up returned %d\n", info);
	);
	// printf("time_after_warm_up = %lf\n", time_after_warm_up);
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// for (i=0;i<rapl_fds_n;i++)
	// {
	// 	if (read(rapl_fds[i], buf, buf_n) < 0)
	// 		error("read");
	// 	lseek(rapl_fds[i], 0, SEEK_SET);
	// 	ujoule_s[i] = atol(buf);
	// }
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	time = time_it(1,
		for (int i=0; i<iterations; i++) {
			info = armpl_spmv_exec_d(ARMPL_SPARSE_OPERATION_NOTRANS, alpha, armpl_mat, x, beta, y);
			if (info!=ARMPL_STATUS_SUCCESS) printf("ERROR: armpl_spmv_exec_d returned %d\n", info);
		}
	);
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// for (i=0;i<rapl_fds_n;i++)
	// {
	// 	if (read(rapl_fds[i], buf, buf_n) < 0)
	// 		error("read");
	// 	lseek(rapl_fds[i], 0, SEEK_SET);
	// 	ujoule_e[i] = atol(buf);
	// }
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/* 6. Destroy created matrix to free any memory created during the 'optimize' phase */
	info = armpl_spmat_destroy(armpl_mat);
	if (info!=ARMPL_STATUS_SUCCESS) printf("ERROR: armpl_spmat_destroy returned %d\n", info);

	double gflops = csr.nnz / time * iterations * 2 * 1e-9;

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// double J_estimated = 0;
	// for (i=0;i<rapl_fds_n;i++)
	// {
	// 	long diff = (ujoule_e[i] - ujoule_s[i] + rapl_max_values[i] + 1) % (rapl_max_values[i] + 1);   // micro joules
	// 	J_estimated += ((double) diff) / 1000000.0;
	// }
	// double W_avg = J_estimated / time;
	double W_avg = 250;
	double J_estimated = W_avg*time;
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	if (AM == NULL)
	{
		MKL_INT m, n, nnz;
		double mem_footprint;
		m = csr.m;
		n = csr.n;
		nnz = csr.nnz;
		mem_footprint = nnz * (sizeof(ValueType) + sizeof(MKL_INT)) + (m+1) * sizeof(MKL_INT);

		fprintf(stderr, "%s,", argv[1]);
		fprintf(stderr, "%d,", omp_get_max_threads());
		fprintf(stderr, "%u,", m);
		fprintf(stderr, "%u,", n);
		fprintf(stderr, "%u,", nnz);
		fprintf(stderr, "%lf,", time);
		fprintf(stderr, "%lf,", gflops);
		fprintf(stderr, "%lf,", mem_footprint/(1024*1024));
		fprintf(stderr, "%lf,", time_balance);
		fprintf(stderr, "%lf,", time_warm_up);
		fprintf(stderr, "%lf\n", time_after_warm_up);

		CheckAccuracy(&coo, x, y);
	}
	else
	{
		fprintf(stderr, "synthetic,");
		fprintf(stderr, "%s,", AM->distribution);
		fprintf(stderr, "%s,", AM->placement);
		fprintf(stderr, "%d,", AM->seed);
		fprintf(stderr, "%u,", AM->nr_rows);
		fprintf(stderr, "%u,", AM->nr_cols);
		fprintf(stderr, "%u,", AM->nr_nzeros);
		fprintf(stderr, "%lf,", AM->density);
		fprintf(stderr, "%lf,", AM->mem_footprint);
		fprintf(stderr, "%s,", AM->mem_range);
		fprintf(stderr, "%lf,", AM->avg_nnz_per_row);
		fprintf(stderr, "%lf,", AM->std_nnz_per_row);
		fprintf(stderr, "%lf,", AM->avg_bw);
		fprintf(stderr, "%lf,", AM->std_bw);
		fprintf(stderr, "%lf,", AM->avg_bw_scaled);
		fprintf(stderr, "%lf,", AM->std_bw_scaled);
		fprintf(stderr, "%lf,", AM->avg_sc);
		fprintf(stderr, "%lf,", AM->std_sc);
		fprintf(stderr, "%lf,", AM->avg_sc_scaled);
		fprintf(stderr, "%lf,", AM->std_sc_scaled);
		fprintf(stderr, "%lf,", AM->skew);
		fprintf(stderr, "%lf,", AM->avg_num_neighbours);
		fprintf(stderr, "%lf,", AM->cross_row_similarity);
		fprintf(stderr, "ARM_library,");
		fprintf(stderr, "%lf,",time);
		fprintf(stderr, "%lf,", gflops); 
		fprintf(stderr, "%lf,", W_avg);
		fprintf(stderr, "%lf\n", J_estimated);

		free_csr_matrix(AM);
	}

	/* 7. Free user allocated storage */
	free(x); free(y);

	return (int)info;	
}
