#include <iostream>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <pthread.h>
#include <sstream>

#include "common.h"
#include "read_mtx.h"

#ifdef __cplusplus
extern "C"{
#endif
	#include "macros/cpp_defines.h"
	#include "macros/macrolib.h"
	#include "time_it.h"
	#include "parallel_util.h"
	#include "pthread_functions.h"
	#include "matrix_util.h"

	#include "string_util.h"
	#include "parallel_io.h"
	#include "file_formats/openfoam/openfoam_matrix.h"
	#include "aux/csr_converter.h"

	#include "monitoring/power/rapl.h"

	#include "artificial_matrix_generation.h"

#ifdef __cplusplus
}
#endif

#include "spmv_kernel.h"


// #define VERBOSE
// #define PER_THREAD_STATS

#if !defined(USE_CUSTOM_CSR)
	#undef PER_THREAD_STATS
#endif

#ifndef BLOCK_SIZE
	#define BLOCK_SIZE  8
#endif


extern INT_T * thread_i_s;
extern INT_T * thread_i_e;

extern INT_T * thread_j_s;
extern INT_T * thread_j_e;

extern ValueType * thread_v_s;
extern ValueType * thread_v_e;


double * thread_time_compute, * thread_time_barrier;

int num_procs;
int process_custom_id;


ValueType * mtx_val;
INT_T * mtx_rowind;
INT_T * mtx_colind;
INT_T mtx_m;
INT_T mtx_n;
INT_T mtx_nnz;

ValueType * csr_a; // values (of size NNZ)
INT_T * csr_ia;    // rowptr (of size m+1)
INT_T * csr_ja;    // colidx of each NNZ (of size nnz)
INT_T csr_m;
INT_T csr_n;
INT_T csr_nnz;


ValueType * x;
ValueType * y;


struct Matrix_Format * MF;   // Real matrices.


// Utils macro
#define Min(x,y) ((x)<(y)?(x):(y))
#define Max(x,y) ((x)>(y)?(x):(y))
#define Abs(x) ((x)>(0)?(x):-(x))

/** Simply return the max relative diff */
void
CheckAccuracy(ValueType * val, INT_T * rowind, INT_T * colind, INT_T m, INT_T nnz, ValueType * x, ValueType * y)
{
	#if DOUBLE == 0
		// ValueType epsilon = 1e-5;
		ValueType epsilon = 1e-7;
	#elif DOUBLE == 1
		// ValueType epsilon = 1e-8;
		ValueType epsilon = 1e-10;
	#endif
	int i, j;

	// ValueType val, tmp;
	// ValueType* kahan = new ValueType[m * sizeof(*kahan)];
	ValueType* y_gold = new ValueType[m * sizeof(*y_gold)];
	for(i=0;i<m;i++)
	{
		// kahan[i] = 0;
		y_gold[i] = 0;
	}

	for (INT_T curr_nnz = 0; curr_nnz < nnz; ++curr_nnz)
	{
		i = rowind[curr_nnz];
		j = colind[curr_nnz];

		y_gold[i] += x[j] * val[curr_nnz];

		// val = x[j] * val[curr_nnz] - kahan[i];
		// tmp = y_gold[i] + val;
		// kahan[i] = (tmp - y_gold[i]) - val;
		// y_gold[i] = tmp;

	}

	ValueType maxDiff = 0;
	// int cnt=0;
	for(int idx = 0 ; idx < m ; idx++) {

		maxDiff = Max(maxDiff, Abs(y_gold[idx]-y[idx]));
		// std::cout << idx << ": " << y_gold[idx]-y[idx] << "\n";
		if (y_gold[idx] != 0.0) {
			// if (Abs((y_gold[idx]-y[idx])/y_gold[idx]) > epsilon)
				// printf("Error: %g != %g , diff=%g , diff_frac=%g\n", y_gold[idx], y[idx], Abs(y_gold[idx]-y[idx]), Abs((y_gold[idx]-y[idx])/y_gold[idx]));
			// maxDiff = Max(maxDiff, Abs((y_gold[idx]-y[idx])/y_gold[idx]));
			maxDiff = Max(maxDiff, Abs(y_gold[idx]-y[idx]));
		}

		// if(maxDiff>epsilon)
		// {
		// cnt++;
		// if(cnt<10)
		// std::cout << "maxDiff = " << maxDiff << " at " << idx << " : " << y_gold[idx] << " vs " << y[idx] << "\n";
		// }
		// if(y_gold[idx] != 0.0){
		// maxDiff = Max(maxDiff, Abs((y_gold[idx]-y[idx])/y_gold[idx]));
		// cnt++;
		// if(cnt<10)
		// std::cout << "maxDiff = " << maxDiff << " vs " << Abs((y_gold[idx]-y[idx])/y_gold[idx]) << "\n";
		// }
	}
	// std::cout << "\n";
	if(maxDiff>epsilon)
		std::cout << "Test failed! (" << maxDiff << ")\n";
	delete[] y_gold;
}


int is_directory(const char *path)
{
    struct stat stats;
    stat(path, &stats);
    // Check for file existence
    if (S_ISDIR(stats.st_mode))
        return 1;
    return 0;
}

/* void
spmv()
{
	#if defined(USE_MKL_IE)
		compute_sparse_mv(A, x, y, descr);
	#elif defined(USE_MKL_CSR)
		compute_csr(&csr, x, y);
	#elif defined(USE_CUSTOM_CSR)
		#if defined(CUSTOM_VECTOR_PERFECT_NNZ_BALANCE)
			compute_csr_custom_perfect_nnz_balance(&csr, x, y);
		#elif defined(CUSTOM_VECTOR)
			compute_csr_custom_vector(&csr, x, y);
		#elif defined(CUSTOM_VECTOR_X86)
			compute_csr_custom_vector_x86(&csr, x, y);
		#elif defined(CUSTOM_SIMD)
			compute_csr_custom_omp_simd(&csr, x, y);
		#elif defined(CUSTOM_PREFETCH)
			compute_csr_custom_omp_prefetch(&csr, x, y);
		#elif defined(CUSTOM_QUEUES)
			compute_csr_custom_x86_queues(&csr, x, y);
		#else
			compute_csr_custom(&csr, x, y);
		#endif
	#elif defined(USE_MKL_BSR)
		compute_bcsr(&bcsr, x, y);
	#elif defined(USE_MKL_DIA)
		compute_dia(&dia, x, y);
	#elif defined(USE_CUSTOM_DIA)
		compute_dia_custom(&dia, x, y);
	#elif defined(USE_MKL_CSC)
		compute_csc(&csc, x, y);
	#elif defined(USE_MKL_COO)
		compute_coo(&coo, x, y);
	#elif defined(USE_LDU)
		compute_ldu(&ldu, x, y);
	#elif defined(USE_ELL)
		// compute_ell(&ell, x, y);
		// compute_ell_v(&ell, x, y);
		// compute_ell_v_hor(&ell, x, y);
		// compute_ell_unroll(&ell, x, y);
		// compute_ell_v_hor_split(&ell, x, y);
		// compute_ell_transposed(&ell, x, y);
		compute_ell_transposed_v(&ell, x, y);
	#endif
} */


void compute(csr_matrix * AM, const std::string& matrix_filename, const int loop = 128)
{
	__attribute__((unused)) int num_threads = omp_get_max_threads();
	int dimMultipleBlock, dimMultipleBlock_y;
	// INT_T m, n, nnz;
	// double mem_footprint;
	double gflops;
	__attribute__((unused)) double time, time_warm_up, time_after_warm_up;
	// long buf_n = 1000;
	// char buf[buf_n];

	dimMultipleBlock = ((csr_m+BLOCK_SIZE-1)/BLOCK_SIZE)*BLOCK_SIZE;
	dimMultipleBlock_y = ((csr_n+BLOCK_SIZE-1)/BLOCK_SIZE)*BLOCK_SIZE;

	x = (ValueType *) aligned_alloc(64, dimMultipleBlock_y * sizeof(ValueType));
	#pragma omp parallel for
	for(int idx = 0 ; idx < dimMultipleBlock_y ; ++idx)
		x[idx] = 1.0;

	y = (ValueType *) aligned_alloc(64, dimMultipleBlock * sizeof(ValueType));
	#pragma omp parallel for
	for(long i=0;i<dimMultipleBlock;i++)
		y[i] = 0.0;

	#if defined(PER_THREAD_STATS)
		thread_time_barrier = (double *) malloc(num_threads * sizeof(*thread_time_barrier));
		thread_time_compute = (double *) malloc(num_threads * sizeof(*thread_time_compute));
	#endif

	// No operations.
	#if 0
		_Pragma("omp parallel")
		{
			long i;
			_Pragma("omp parallel for")
			for (i=0;i<csr_m+1;i++)
				csr_ia[i] = 0;
		}
	#endif

	// idx0 - Remove X access pattern dependency.
	#if 0
		_Pragma("omp parallel")
		{
			long i;
			_Pragma("omp parallel for")
			for (i=0;i<csr_nnz;i++)
				csr_ja[i] = 0;
		}
	#endif

	// idx_serial - Remove X access pattern dependency.
	#if 0
		_Pragma("omp parallel")
		{
			long i;
			_Pragma("omp parallel for")
			for (i=0;i<csr_nnz;i++)
				csr_ja[i] = i % csr_n;
		}
	#endif

	// idx_t_local - Remove X access pattern dependency.
	#if 0
		_Pragma("omp parallel")
		{
			int tnum = omp_get_thread_num();
			long i;
			long i_per_t = csr_n / num_threads;
			long i_s = tnum * i_per_t;
			_Pragma("omp parallel for")
			for (i=0;i<csr_nnz;i++)
				csr_ja[i] = i_s + (i % i_per_t);
		}
	#endif

	/* #if defined(USE_MKL_IE)
		format_name = (char *) "MKL_IE";
		time = time_it(1,
			#if DOUBLE == 0
				mkl_sparse_s_create_csr(&A, SPARSE_INDEX_BASE_ZERO, csr_m, csr_n, csr_ia, csr_ia+1, csr_ja, csr_a);
			#elif DOUBLE == 1
				mkl_sparse_d_create_csr(&A, SPARSE_INDEX_BASE_ZERO, csr_m, csr_n, csr_ia, csr_ia+1, csr_ja, csr_a);
			#endif
			mkl_sparse_order(A);
		);
		printf("mkl mkl_sparse_s_create_csr + mkl_sparse_order time = %g\n", time);

		descr.type = SPARSE_MATRIX_TYPE_GENERAL;
		const sparse_operation_t operation = SPARSE_OPERATION_NON_TRANSPOSE;
		const INT_T expected_calls = loop;

		// Using SPARSE_MEMORY_AGGRESSIVE policy for some reason gives libgomp error 'out of memory' at 128 theads.
		//     SPARSE_MEMORY_NONE
		//         Routine can allocate memory only for auxiliary structures (such as for workload balancing); the amount of memory is proportional to vector size.
		//     SPARSE_MEMORY_AGGRESSIVE
		//         Default.
		//         Routine can allocate memory up to the size of matrix A for converting into the appropriate sparse format.
		// const sparse_memory_usage_t policy = SPARSE_MEMORY_AGGRESSIVE;
		const sparse_memory_usage_t policy = SPARSE_MEMORY_NONE;

		time_balance = time_it(1,
			mkl_sparse_set_mv_hint(A, operation, descr, expected_calls);
			mkl_sparse_set_memory_hint(A, policy);
			mkl_sparse_optimize(A);
		);
		printf("mkl optimize time = %g\n", time_balance);

		// Even with the 'optimize' it doesn't seem to sort the columns.
		#if 0
			for (i=0;i<csr_m;i++)
			{
				for (long j=csr_ia[i];j<csr_ia[i+1];j++)
				{
					if (j < csr_ia[i+1]-1 && csr_ja[j] >= csr_ja[j+1])
						error("%ld: unsorted columns: %d >= %d", i, csr_ja[j], csr_ja[j+1]);
				}
			}
		#endif

		// _Pragma("omp parallel")
		// {
			// printf("max_threads=%d , num_threads=%d , tnum=%d\n", omp_get_max_threads(), omp_get_num_threads(), omp_get_thread_num());
		// }
		// printf("out\n");

		m = csr_m;
		n = csr_n;
		nnz = csr_nnz;
		mem_footprint = nnz * (sizeof(ValueType) + sizeof(INT_T)) + (m+1) * sizeof(INT_T);
	#elif defined(USE_MKL_CSR)
		format_name = (char *) "MKL_CSR";
		m = csr_m;
		n = csr_n;
		nnz = csr_nnz;
		mem_footprint = nnz * (sizeof(ValueType) + sizeof(INT_T)) + (m+1) * sizeof(INT_T);
	#elif defined(USE_CUSTOM_CSR)
		#ifdef NAIVE
			format_name = (char *) "Naive_CSR_CPU";
		#elif defined(CUSTOM_VECTOR_PERFECT_NNZ_BALANCE)
			format_name = (char *) "Custom_CSR_PBV";
		#else
			#ifdef CUSTOM_VECTOR
				format_name = (char *) "Custom_CSR_BV";
			#else
				format_name = (char *) "Custom_CSR_B";
			#endif
		#endif
		m = csr_m;
		n = csr_n;
		nnz = csr_nnz;
		mem_footprint = nnz * (sizeof(ValueType) + sizeof(INT_T)) + (m+1) * sizeof(INT_T);
	#elif defined(USE_MKL_BSR)
		format_name = (char *) "MKL_BSR";
		CSR_to_BCSR(&csr, &bcsr, BLOCK_SIZE);
		m = bcsr.m;
		n = bcsr.n;
		nnz = bcsr.nnz;
		mem_footprint = bcsr.nbBlocks*bcsr.lb*bcsr.lb*sizeof(ValueType) + (bcsr.nbBlockRows+1)*sizeof(INT_T) + bcsr.nbBlocks*sizeof(INT_T);
	#elif defined(USE_MKL_DIA)
		format_name = (char *) "MKL_DIA";
		CSR_to_DIA(&csr, &dia);
		m = dia.m;
		n = dia.n;
		nnz = dia.nnz;
		mem_footprint = dia.ndiag*dia.lval*sizeof(ValueType) + dia.ndiag*sizeof(INT_T);
	#elif defined(USE_CUSTOM_DIA)
		format_name = (char *) "Custom_DIA";
		CSR_to_DIA(&csr, &dia);
		ValueType * t = transpose<ValueType>(dia.val, dia.ndiag, dia.lval);
		free(dia.val);
		dia.val = t;
		m = dia.m;
		n = dia.n;
		nnz = dia.nnz;
		mem_footprint = dia.ndiag*dia.lval*sizeof(ValueType) + dia.ndiag*sizeof(INT_T);
	#elif defined(USE_MKL_CSC)
		format_name = (char *) "MKL_CSC";
		CSR_to_CSC(&csr, &csc);
		m = csc.m;
		n = csc.n;
		nnz = csc.nnz;
		mem_footprint = nnz * (sizeof(ValueType) + sizeof(INT_T)) + (n+1) * sizeof(INT_T);
	#elif defined(USE_MKL_COO)
		format_name = (char *) "MKL_COO";
		m = coo.m;
		n = coo.n;
		nnz = coo.nnz;
		mem_footprint = nnz * (sizeof(ValueType) + 2 * sizeof(INT_T));
	#elif defined(USE_LDU)
		format_name = (char *) "LDU";
		CSR_to_LDU(&csr, &ldu);
		m = ldu.m;
		n = ldu.n;
		nnz = ldu.nnz;
		mem_footprint = nnz * sizeof(ValueType) + ldu.upper_n * 2 * sizeof(INT_T);
	#elif defined(USE_ELL)
		format_name = (char *) "ELL";
		CSR_to_ELL(&csr, &ell);
		m = ell.m;
		n = ell.n;
		nnz = ell.nnz;
		mem_footprint = m * ell.width * (sizeof(ValueType) + sizeof(INT_T));
	#else
		format_name = (char *) "OTHER";
	#endif */


	MF = csr_to_format(csr_ia, csr_ja, csr_a, csr_m, csr_n, csr_nnz);

	#if defined(PER_THREAD_STATS)
		for (i=0;i<num_threads;i++)
		{
			long rows, nnz;
			INT_T i_s, i_e, j_s, j_e;
			i_s = thread_i_s[i];
			i_e = thread_i_e[i];
			j_s = thread_j_s[i];
			j_e = thread_j_e[i];
			rows = i_e - i_s;
			nnz = csr_ia[i_e] - csr_ia[i_s];
			printf("%ld: rows=[%d(%d), %d(%d)]:%ld(%ld), nnz=[%d, %d]:%d\n", i, i_s, csr_ia[i_s], i_e, csr_ia[i_e], rows, nnz, j_s, j_e, j_e-j_s);
		}
	#endif

	// warm up caches
	time_warm_up = time_it(1,
		MF->spmv(x, y);
	);

	time_after_warm_up = time_it(1,
		MF->spmv(x, y);
	);

	#if defined(PER_THREAD_STATS)
		// Clear times after warmup.
		for (i=0;i<num_threads;i++)
		{
			thread_time_compute[i] = 0;
			thread_time_barrier[i] = 0;
		}
	#endif

	#ifdef PROC_BENCH
		raise(SIGSTOP);
	#endif

	/*****************************************************************************************/
	struct RAPL_Register * regs;
	long regs_n;
	char * reg_ids;

	reg_ids = NULL;
	reg_ids = (char *) "0,1"; // For Xeon(Gold1), these two are for package-0 and package-1E
	// reg_ids = (char *) "0:0";
	// reg_ids = (char *) "0,0:0";

	rapl_open(reg_ids, &regs, &regs_n);
	/*****************************************************************************************/

	time = 0;
	for(int idxLoop = 0 ; idxLoop < loop ; ++idxLoop){
		rapl_read_start(regs, regs_n);

		time += time_it(1,
			MF->spmv(x, y);
		);

		rapl_read_end(regs, regs_n);
	}

	/*****************************************************************************************/
	double J_estimated = 0;
	for (int i=0;i<regs_n;i++){
		// printf("'%s' total joule = %g\n", regs[i].type, ((double) regs[i].uj_accum) / 1000000);
		J_estimated += ((double) regs[i].uj_accum) / 1e6;
	}
	rapl_close(regs, regs_n);
	free(regs);
	double W_avg = J_estimated / time;
	// printf("J_estimated = %lf\tW_avg = %lf\n", J_estimated, W_avg);
	/*****************************************************************************************/

	//=============================================================================
	//= Output section.
	//=============================================================================

	#if defined(PER_THREAD_STATS)
		double iters_per_t[num_threads];
		double nnz_per_t[num_threads];
		__attribute__((unused)) double gflops_per_t[num_threads];
		double iters_per_t_min, iters_per_t_max, iters_per_t_avg, iters_per_t_std, iters_per_t_balance;
		double nnz_per_t_min, nnz_per_t_max, nnz_per_t_avg, nnz_per_t_std, nnz_per_t_balance;
		__attribute__((unused)) double time_per_t_min, time_per_t_max, time_per_t_avg, time_per_t_std, time_per_t_balance;
		__attribute__((unused)) double gflops_per_t_min, gflops_per_t_max, gflops_per_t_avg, gflops_per_t_std, gflops_per_t_balance;
		long i_s, i_e;

		for (i=0;i<num_threads;i++)
		{
			i_s = thread_i_s[i];
			i_e = thread_i_e[i];
			iters_per_t[i] = i_e - i_s;
			// nnz_per_t[i] = &(csr_a[csr_ia[i_e]]) - &(csr_a[csr_ia[i_s]]);
			nnz_per_t[i] = csr_ia[i_e] - csr_ia[i_s];
			gflops_per_t[i] = nnz_per_t[i] / thread_time_compute[i] * loop * 2 * 1e-9;   // Calculate before making nnz_per_t a ratio.
			iters_per_t[i] /= m;    // As a fraction of m.
			nnz_per_t[i] /= nnz;    // As a fraction of nnz.
		}

		matrix_min_max(iters_per_t, num_threads, &iters_per_t_min, &iters_per_t_max);
		iters_per_t_avg = matrix_mean(iters_per_t, num_threads);
		iters_per_t_std = matrix_std_base(iters_per_t, num_threads, iters_per_t_avg);
		iters_per_t_balance = iters_per_t_avg / iters_per_t_max;

		matrix_min_max(nnz_per_t, num_threads, &nnz_per_t_min, &nnz_per_t_max);
		nnz_per_t_avg = matrix_mean(nnz_per_t, num_threads);
		nnz_per_t_std = matrix_std_base(nnz_per_t, num_threads, nnz_per_t_avg);
		nnz_per_t_balance = nnz_per_t_avg / nnz_per_t_max;

		matrix_min_max(thread_time_compute, num_threads, &time_per_t_min, &time_per_t_max);
		time_per_t_avg = matrix_mean(thread_time_compute, num_threads);
		time_per_t_std = matrix_std_base(thread_time_compute, num_threads, time_per_t_avg);
		time_per_t_balance = time_per_t_avg / time_per_t_max;

		matrix_min_max(gflops_per_t, num_threads, &gflops_per_t_min, &gflops_per_t_max);
		gflops_per_t_avg = matrix_mean(gflops_per_t, num_threads);
		gflops_per_t_std = matrix_std_base(gflops_per_t, num_threads, gflops_per_t_avg);
		gflops_per_t_balance = gflops_per_t_avg / gflops_per_t_max;
	#endif

	#ifdef VERBOSE
	{
		printf("i:%g,%g,%g,%g,%g\n", iters_per_t_min, iters_per_t_max, iters_per_t_avg, iters_per_t_std, iters_per_t_balance);
		printf("nnz:%g,%g,%g,%g,%g\n", nnz_per_t_min, nnz_per_t_max, nnz_per_t_avg, nnz_per_t_std, nnz_per_t_balance);
		#if defined(PER_THREAD_STATS)
			printf("time:%g,%g,%g,%g,%g\n", time_per_t_min, time_per_t_max, time_per_t_avg, time_per_t_std, time_per_t_balance);
			printf("gflops:%g,%g,%g,%g,%g\n", gflops_per_t_min, gflops_per_t_max, gflops_per_t_avg, gflops_per_t_std, gflops_per_t_balance);
		#endif
		printf("tnum i_s i_e num_rows_frac nnz_frac\n");
		for (i=0;i<num_threads;i++)
		{
			i_s = thread_i_s[i];
			i_e = thread_i_e[i];
			printf("%ld %ld %ld %g %g\n", i, i_s, i_e, iters_per_t[i], nnz_per_t[i]);
		}
		#if defined(PER_THREAD_STATS)
			printf("tnum gflops compute barrier total barrier/compute%%\n");
			for (i=0;i<num_threads;i++)
			{
				double time_compute, time_barrier, time_total, percent;
				time_compute = thread_time_compute[i];
				time_barrier = thread_time_barrier[i];
				time_total = time_compute + time_barrier;
				percent = time_barrier / time_compute * 100;
				printf("%ld %g %g %g %g %g\n", i, gflops_per_t[i], time_compute, time_barrier, time_total, percent);
			}
		#endif
	}
	#endif

	gflops = csr_nnz / time * loop * 2 * 1e-9;    // Use csr_nnz to be sure we have the initial nnz (there is no coo for artificial AM).

	std::stringstream stream;
	if (AM == NULL)
	{
		stream
		#ifdef PROC_BENCH
			<< "pnum_" << process_custom_id << "," << matrix_filename << "," << num_procs
		#else
			<< matrix_filename << "," << omp_get_max_threads()
		#endif
			<< "," << MF->m << "," << MF->n << "," << MF->nnz
			<< "," << time << "," << gflops << "," << MF->mem_footprint/(1024*1024)
			<< "," << W_avg << "," << J_estimated
			// << "," << time_warm_up << "," << time_after_warm_up
		#ifdef PER_THREAD_STATS
			<< "," << iters_per_t_avg << "," << iters_per_t_std << "," << iters_per_t_balance
			<< "," << nnz_per_t_avg << "," << nnz_per_t_std << "," << nnz_per_t_balance
			<< "," << time_per_t_avg << "," << time_per_t_std << "," << time_per_t_balance
			<< "," << gflops_per_t_avg << "," << gflops_per_t_std << "," << gflops_per_t_balance
		#endif
			<< "\n";
		std::cerr << stream.str();

		CheckAccuracy(mtx_val, mtx_rowind, mtx_colind, mtx_m, mtx_nnz, x, y);
	}
	else
	{
		stream << "synthetic" << "," << AM->distribution << "," << AM->placement << "," << AM->seed
			<< "," << AM->nr_rows << "," << AM->nr_cols << "," << AM->nr_nzeros
			<< "," << AM->density << "," << AM->mem_footprint << "," << AM->mem_range
			<< "," << AM->avg_nnz_per_row << "," << AM->std_nnz_per_row
			<< "," << AM->avg_bw << "," << AM->std_bw
			<< "," << AM->avg_bw_scaled << "," << AM->std_bw_scaled
			<< "," << AM->avg_sc << "," << AM->std_sc
			<< "," << AM->avg_sc_scaled << "," << AM->std_sc_scaled
			<< "," << AM->skew
			<< "," << AM->avg_num_neighbours << "," << AM->cross_row_similarity
			<< "," << MF->format_name <<  "," << time << "," << gflops << "," << W_avg << "," << J_estimated
		#ifdef PER_THREAD_STATS
			<< "," << iters_per_t_avg << "," << iters_per_t_std << "," << iters_per_t_balance
			<< "," << nnz_per_t_avg << "," << nnz_per_t_std << "," << nnz_per_t_balance
			<< "," << time_per_t_avg << "," << time_per_t_std << "," << time_per_t_balance
			<< "," << gflops_per_t_avg << "," << gflops_per_t_std << "," << gflops_per_t_balance
		#endif
			<< "\n";
		std::cerr << stream.str();
	}

	free(x);
	free(y);

	#if defined(PER_THREAD_STATS)
		free(thread_time_barrier);
		free(thread_time_compute);
	#endif
}


//==========================================================================================================================================
//= main
//==========================================================================================================================================

int main(int argc, char **argv)
{
	__attribute__((unused)) int num_threads;
	__attribute__((unused)) double time;
	long i;

	// Wake omp up from eternal slumber.
	#pragma omp parallel
	{
		num_threads = omp_get_max_threads();
	}
	printf("max threads %d\n", num_threads);

	if (argc < 6)
	{
		char * file_in;
		i = 1;
		#ifdef PROC_BENCH
			num_procs = atoi(argv[i++]);

			pid_t pids[num_procs];
			pid_t pid;
			pthread_t tid;
			int core;
			long j;

			for (j=0;j<num_procs;j++)
			{
				pid = fork();
				if (pid == -1)
					error("fork");
				if (pid == 0)
				{
					process_custom_id = j;

					core = process_custom_id;
					// int max_cores = 128;
					// int cores_per_numa_node = 16;
					// int num_numa_nodes = max_cores / cores_per_numa_node;
					// core = (j*cores_per_numa_node + j/num_numa_nodes) % max_cores;    // cycle numa nodes
					// printf("%d -> %d\n", process_custom_id, core);

					tid = pthread_self();
					set_affinity(tid, core);
					goto child_proc_label;
				}
				pids[j] = pid;
			}
			tid = pthread_self();
			set_affinity(tid, 0);
			for (j=0;j<num_procs;j++)
				waitpid(-1, NULL, WUNTRACED);
			for (j=0;j<num_procs;j++)
				kill(pids[j], SIGCONT);
			for (j=0;j<num_procs;j++)
				waitpid(-1, NULL, WUNTRACED);
			exit(0);

child_proc_label:

		#endif

		file_in = argv[i++];
		time = time_it(1,
			if (is_directory(file_in))
			{
				int nnz_non_diag, N;
				int * rowind, * colind;
				read_openfoam_matrix_dir(file_in, &rowind, &colind, &N, &nnz_non_diag);
				mtx_m = N;
				mtx_n = N;
				mtx_nnz = N + nnz_non_diag;
				mtx_rowind = (INT_T *) aligned_alloc(64, mtx_nnz * sizeof(INT_T));
				mtx_colind = (INT_T *) aligned_alloc(64, mtx_nnz * sizeof(INT_T));
				mtx_val = (ValueType *) aligned_alloc(64, mtx_nnz * sizeof(ValueType));
				for (i=0;i<mtx_nnz;i++)
				{
					mtx_rowind[i] = rowind[i];
					mtx_colind[i] = colind[i];
					mtx_val[i] = 1.0;
				}
				free(rowind);
				free(colind);
			}
			else
				create_coo_matrix(file_in, &mtx_val, &mtx_rowind, &mtx_colind, &mtx_m, &mtx_n, &mtx_nnz);
		);
		printf("time read: %lf\n", time);
		time = time_it(1,
			csr_a = (ValueType *) aligned_alloc(64, (mtx_nnz + VECTOR_ELEM_NUM) * sizeof(ValueType));
			csr_ja = (INT_T *) aligned_alloc(64, (mtx_nnz + VECTOR_ELEM_NUM) * sizeof(INT_T));
			csr_ia = (INT_T *) aligned_alloc(64, (mtx_m+1 + VECTOR_ELEM_NUM) * sizeof(INT_T));
			csr_m = mtx_m;
			csr_n = mtx_n;
			csr_nnz = mtx_nnz;
			_Pragma("omp parallel for")
			for (int i=0;i<mtx_nnz + VECTOR_ELEM_NUM;i++)
			{
				csr_a[i] = 0.0;
				csr_ja[i] = 0;
			}
			_Pragma("omp parallel for")
			for (int i=0;i<mtx_m+1 + VECTOR_ELEM_NUM;i++)
				csr_ia[i] = 0;
			coo_to_csr(mtx_rowind, mtx_colind, mtx_val, mtx_m, mtx_n, mtx_nnz, csr_ia, csr_ja, csr_a, 1);
		);
		printf("time coo to csr: %lf\n", time);
		// for (prefetch_distance=1;prefetch_distance<=128;prefetch_distance++)
		// for (i=0;i<128;i++)
		{
			// fprintf(stderr, "prefetch_distance = %d\n", prefetch_distance);
			compute(NULL, file_in);
			// compute(NULL, file_in, 128 * 10);
		}
	}
	else
	{
		csr_matrix * AM = NULL;
		char buf[1000];

		time = time_it(1,

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
		printf("time generate artificial matrix: %lf\n", time);

		csr_m = AM->nr_rows;
		csr_n = AM->nr_cols;
		csr_nnz = AM->nr_nzeros;

		csr_ia = (INT_T *) aligned_alloc(64, (csr_m+1 + VECTOR_ELEM_NUM) * sizeof(INT_T));
		#pragma omp parallel for
		for (long i=0;i<csr_m+1;i++)
			csr_ia[i] = AM->row_ptr[i];
		free(AM->row_ptr);
		AM->row_ptr = NULL;

		csr_a = (ValueType *) aligned_alloc(64, (csr_nnz + VECTOR_ELEM_NUM) * sizeof(ValueType));
		csr_ja = (INT_T *) aligned_alloc(64, (csr_nnz + VECTOR_ELEM_NUM) * sizeof(INT_T));
		#pragma omp parallel for
		for (long i=0;i<csr_nnz;i++)
		{
			csr_a[i] = AM->values[i];
			csr_ja[i] = AM->col_ind[i];
		}
		free(AM->values);
		AM->values = NULL;
		free(AM->col_ind);
		AM->col_ind = NULL;

		snprintf(buf, sizeof(buf), "'%d_%d_%d_%g_%g_%g_%g'", AM->nr_rows, AM->nr_cols, AM->nr_nzeros, AM->avg_bw, AM->std_bw, AM->avg_sc, AM->std_sc);

		compute(AM, buf);
		// compute(AM, buf, 128 * 10);
		// for (i=0;i<1024;i++)
		// {
			// compute(AM, buf);
		// }

		free_csr_matrix(AM);
	}

	return 0;
}
