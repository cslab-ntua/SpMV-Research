#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <pthread.h>

#include "util.h"

#include "macros/cpp_defines.h"
#include "macros/macrolib.h"
#include "time_it.h"
#include "parallel_util.h"
#include "pthread_functions.h"
#include "matrix_util.h"


// #define VERBOSE
// #define PER_THREAD_STATS

#if !defined(USE_CUSTOM_CSR)
	#undef PER_THREAD_STATS
#endif

#ifndef BLOCK_SIZE
	#define BLOCK_SIZE  8
#endif


char * format_name;

MKL_INT * thread_i_s;
MKL_INT * thread_i_e;

double * thread_time_compute, * thread_time_barrier;


int num_procs;
int process_custom_id;

ValueType * x;
ValueType* y;

matrix_descr descr;
sparse_matrix_t A;
CSRArrays csr;
BCSRArrays bcsr;
DIAArrays dia;
CSCArrays csc;
COOArrays coo;
LDUArrays ldu;

ValueType * thread_partial_sums_s;
ValueType * thread_partial_sums_e;
MKL_INT * thread_partial_sums_row_s;
MKL_INT * thread_partial_sums_row_e;


#include "spmv_kernels.h"


void
spmv()
{
	#if defined(USE_MKL_IE)
		compute_sparse_mv(A, x, y, descr);
	#elif defined(USE_MKL_CSR)
		compute_csr(&csr, x, y);
	#elif defined(USE_CUSTOM_CSR)
		compute_csr_custom(&csr, x, y);
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
	#endif
}


void compute(csr_matrix * AM, const std::string& matrix_filename, const int loop = 128)
{
	int num_threads = omp_get_max_threads();
	int dimMultipleBlock, dimMultipleBlock_y;
	MKL_INT m, n, nnz;
	double mem_footprint;
	double gflops;
	__attribute__((unused)) double time_balance, time_warm_up, time_after_warm_up;
	long i;

	dimMultipleBlock = ((csr.m+BLOCK_SIZE-1)/BLOCK_SIZE)*BLOCK_SIZE;
	dimMultipleBlock_y = ((csr.n+BLOCK_SIZE-1)/BLOCK_SIZE)*BLOCK_SIZE;

	x = (ValueType *) mkl_malloc(dimMultipleBlock_y * sizeof(ValueType), 64);
	#pragma omp parallel for
	for(int idx = 0 ; idx < dimMultipleBlock_y ; ++idx)
		x[idx] = 1.0;

	y = (ValueType *) mkl_malloc(dimMultipleBlock * sizeof(ValueType), 64);
	#pragma omp parallel for
	for(long i=0;i<dimMultipleBlock;i++)
		y[i] = 0.0;

	thread_time_barrier = (double *) malloc(num_threads * sizeof(*thread_time_barrier));
	thread_time_compute = (double *) malloc(num_threads * sizeof(*thread_time_compute));

	thread_i_s = (MKL_INT *) malloc(num_threads * sizeof(*thread_i_s));
	thread_i_e = (MKL_INT *) malloc(num_threads * sizeof(*thread_i_e));
	thread_partial_sums_s = (ValueType *) malloc(num_threads * sizeof(*thread_partial_sums_s));
	thread_partial_sums_e = (ValueType *) malloc(num_threads * sizeof(*thread_partial_sums_e));
	thread_partial_sums_row_s = (MKL_INT *) malloc(num_threads * sizeof(*thread_partial_sums_row_s));
	thread_partial_sums_row_e = (MKL_INT *) malloc(num_threads * sizeof(*thread_partial_sums_row_e));

	// No operations.
	#if 0
		_Pragma("omp parallel")
		{
			long i;
			_Pragma("omp parallel for")
			for (i=0;i<csr.m+1;i++)
				csr.ia[i] = 0;
		}
	#endif

	// idx0 - Remove X access pattern dependency.
	#if 0
		_Pragma("omp parallel")
		{
			long i;
			_Pragma("omp parallel for")
			for (i=0;i<csr.nnz;i++)
				csr.ja[i] = 0;
		}
	#endif

	// idx_serial - Remove X access pattern dependency.
	#if 0
		_Pragma("omp parallel")
		{
			long i;
			_Pragma("omp parallel for")
			for (i=0;i<csr.nnz;i++)
				csr.ja[i] = i % csr.n;
		}
	#endif

	// idx_t_local - Remove X access pattern dependency.
	#if 0
		_Pragma("omp parallel")
		{
			int tnum = omp_get_thread_num();
			long i;
			long i_per_t = csr.n / num_threads;
			long i_s = tnum * i_per_t;
			_Pragma("omp parallel for")
			for (i=0;i<csr.nnz;i++)
				csr.ja[i] = i_s + (i % i_per_t);
		}
	#endif

	#if defined(USE_MKL_IE)
		format_name = (char *) "MKL_IE";
		#if DOUBLE == 0
			mkl_sparse_s_create_csr(&A, SPARSE_INDEX_BASE_ZERO, csr.m, csr.n, csr.ia, csr.ia+1, csr.ja, csr.a);
		#elif DOUBLE == 1
			mkl_sparse_d_create_csr(&A, SPARSE_INDEX_BASE_ZERO, csr.m, csr.n, csr.ia, csr.ia+1, csr.ja, csr.a);
		#endif
		descr.type = SPARSE_MATRIX_TYPE_GENERAL;
		const sparse_operation_t operation = SPARSE_OPERATION_NON_TRANSPOSE;
		const MKL_INT expected_calls = loop;

		// Using SPARSE_MEMORY_AGGRESSIVE policy for some reason gives libgomp error 'out of memory' at 128 theads.
		//     SPARSE_MEMORY_NONE
		//         Routine can allocate memory only for auxiliary structures (such as for workload balancing); the amount of memory is proportional to vector size.
		//     SPARSE_MEMORY_AGGRESSIVE
		//         Default.
		//         Routine can allocate memory up to the size of matrix A for converting into the appropriate sparse format.
		const sparse_memory_usage_t policy = SPARSE_MEMORY_NONE;

		time_balance = time_it(1,
			mkl_sparse_set_memory_hint (A, policy);
			mkl_sparse_set_mv_hint(A, operation, descr, expected_calls);
			mkl_sparse_optimize(A);
		);
		printf("mkl optimize time = %g\n", time_balance);
		m = csr.m;
		n = csr.n;
		nnz = csr.nnz;
		mem_footprint = nnz * (sizeof(ValueType) + sizeof(MKL_INT)) + (m+1) * sizeof(MKL_INT);
	#elif defined(USE_MKL_CSR)
		format_name = (char *) "MKL_CSR";
		m = csr.m;
		n = csr.n;
		nnz = csr.nnz;
		mem_footprint = nnz * (sizeof(ValueType) + sizeof(MKL_INT)) + (m+1) * sizeof(MKL_INT);
	#elif defined(USE_CUSTOM_CSR)
		#ifdef NAIVE
			format_name = (char *) "Naive_CSR_CPU";
		#else
			format_name = (char *) "Custom_CSR_Balanced";
		#endif
		m = csr.m;
		n = csr.n;
		nnz = csr.nnz;
		mem_footprint = nnz * (sizeof(ValueType) + sizeof(MKL_INT)) + (m+1) * sizeof(MKL_INT);
	#elif defined(USE_MKL_BSR)
		format_name = (char *) "MKL_BSR";
		CSR_to_BCSR(&csr, &bcsr, BLOCK_SIZE);
		m = bcsr.m;
		n = bcsr.n;
		nnz = bcsr.nnz;
		mem_footprint = bcsr.nbBlocks*bcsr.lb*bcsr.lb*sizeof(ValueType) + (bcsr.nbBlockRows+1)*sizeof(MKL_INT) + bcsr.nbBlocks*sizeof(MKL_INT);
	#elif defined(USE_MKL_DIA)
		format_name = (char *) "MKL_DIA";
		CSR_to_DIA(&csr, &dia);
		m = dia.m;
		n = dia.n;
		nnz = dia.nnz;
		mem_footprint = dia.ndiag*dia.lval*sizeof(ValueType) + dia.ndiag*sizeof(MKL_INT);
	#elif defined(USE_CUSTOM_DIA)
		format_name = (char *) "Custom_DIA";
		CSR_to_DIA(&csr, &dia);
		ValueType * t = transpose(dia.val, dia.ndiag, dia.lval);
		mkl_free(dia.val);
		dia.val = t;
		m = dia.m;
		n = dia.n;
		nnz = dia.nnz;
		mem_footprint = dia.ndiag*dia.lval*sizeof(ValueType) + dia.ndiag*sizeof(MKL_INT);
	#elif defined(USE_MKL_CSC)
		format_name = (char *) "MKL_CSC";
		CSR_to_CSC(&csr, &csc);
		m = csc.m;
		n = csc.n;
		nnz = csc.nnz;
		mem_footprint = nnz * (sizeof(ValueType) + sizeof(MKL_INT)) + (n+1) * sizeof(MKL_INT);
	#elif defined(USE_MKL_COO)
		format_name = (char *) "MKL_COO";
		m = coo.m;
		n = coo.n;
		nnz = coo.nnz;
		mem_footprint = nnz * (sizeof(ValueType) + 2 * sizeof(MKL_INT));
	#elif defined(USE_LDU)
		format_name = (char *) "LDU";
		CSR_to_LDU(&csr, &ldu);
		m = ldu.m;
		n = ldu.n;
		nnz = ldu.nnz;
		mem_footprint = nnz * sizeof(ValueType) + ldu.upper_n * 2 * sizeof(MKL_INT);
	#else
		format_name = (char *) "OTHER";
	#endif

	#if defined(USE_CUSTOM_CSR)
		time_balance = time_it(1,
			_Pragma("omp parallel")
			{
				int tnum = omp_get_thread_num();
				#if defined(NAIVE) || defined(PROC_BENCH)
					loop_partitioner_balance_iterations(num_threads, tnum, 0, m, &thread_i_s[tnum], &thread_i_e[tnum]);
				#else
					// loop_partitioner_balance_partial_sums(num_threads, tnum, csr.ia, csr.m, csr.nnz, &thread_i_s[tnum], &thread_i_e[tnum]);
					loop_partitioner_balance(num_threads, tnum, 2, csr.ia, csr.m, csr.nnz, &thread_i_s[tnum], &thread_i_e[tnum]);
				#endif
			}
		);
		printf("balance time = %g\n", time_balance);
	#else
		_Pragma("omp parallel")
		{
			int tnum = omp_get_thread_num();
			loop_partitioner_balance_iterations(num_threads, tnum, 0, m, &thread_i_s[tnum], &thread_i_e[tnum]);
		}
	#endif

	// warm up caches
	time_warm_up = time_it(1,
		spmv();
	);

	time_after_warm_up = time_it(1,
		spmv();
	);

	// Clear times after warmup.
	for (i=0;i<num_threads;i++)
	{
		thread_time_compute[i] = 0;
		thread_time_barrier[i] = 0;
	}

	#ifdef PROC_BENCH
		raise(SIGSTOP);
	#endif

	const double timeStart = omp_get_wtime();
	for(int idxLoop = 0 ; idxLoop < loop ; ++idxLoop)
		spmv();
	const double timeEnd = omp_get_wtime();

	double time = timeEnd-timeStart;


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
			nnz_per_t[i] = &(csr.a[csr.ia[i_e]]) - &(csr.a[csr.ia[i_s]]);
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

	gflops = nnz / time * loop * 2 * 1e-9;

	if (AM == NULL)
	{
		std::cout
		#ifdef PROC_BENCH
			<< "pnum_" << process_custom_id << "," << matrix_filename << "," << num_procs
		#else
			<< matrix_filename << "," << mkl_get_max_threads()
		#endif
			<< "," << m << "," << n << "," << nnz
			<< "," << time << "," << gflops << "," << mem_footprint/(1024*1024)
			<< "," << time_balance << "," << time_warm_up << "," << time_after_warm_up
		#ifdef PER_THREAD_STATS
			<< "," << iters_per_t_avg << "," << iters_per_t_std << "," << iters_per_t_balance
			<< "," << nnz_per_t_avg << "," << nnz_per_t_std << "," << nnz_per_t_balance
			<< "," << time_per_t_avg << "," << time_per_t_std << "," << time_per_t_balance
			<< "," << gflops_per_t_avg << "," << gflops_per_t_std << "," << gflops_per_t_balance
		#endif
			<< "\n";

		CheckAccuracy(&coo, x, y);
	}
	else
	{
		double W_avg = 250;
		double J_estimated = W_avg * time;
		std::cout << "synthetic" << "," << AM->distribution << "," << AM->placement << "," << AM->seed
			<< "," << AM->nr_rows << "," << AM->nr_cols << "," << AM->nr_nzeros
			<< "," << AM->density << "," << AM->mem_footprint << "," << AM->mem_range
			<< "," << AM->avg_nnz_per_row << "," << AM->std_nnz_per_row
			<< "," << AM->avg_bw << "," << AM->std_bw
			<< "," << AM->avg_bw_scaled << "," << AM->std_bw_scaled
			<< "," << AM->avg_sc << "," << AM->std_sc
			<< "," << AM->avg_sc_scaled << "," << AM->std_sc_scaled
			<< "," << AM->skew
			<< "," << format_name <<  "," << time << "," << gflops << "," << W_avg << "," << J_estimated
		#ifdef PER_THREAD_STATS
			<< "," << iters_per_t_avg << "," << iters_per_t_std << "," << iters_per_t_balance
			<< "," << nnz_per_t_avg << "," << nnz_per_t_std << "," << nnz_per_t_balance
			<< "," << time_per_t_avg << "," << time_per_t_std << "," << time_per_t_balance
			<< "," << gflops_per_t_avg << "," << gflops_per_t_std << "," << gflops_per_t_balance
		#endif
			<< "\n";
	}

	mkl_free(x);
	mkl_free(y);
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
			create_coo_matrix(file_in, &coo);
		);
		printf("time read: %lf\n", time);
		time = time_it(1,
			COO_to_CSR(&coo, &csr);
		);
		printf("time coo to csr: %lf\n", time);
		compute(NULL, file_in);
	}
	else
	{
		csr_matrix * AM = NULL;
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
			seed = atoi(argv[i++]);

			AM = artificial_matrix_generation(nr_rows, nr_cols, avg_nnz_per_row, std_nnz_per_row, distribution, seed, placement, bw, skew, avg_num_neighbours);

		);
		printf("time generate artificial matrix: %lf\n", time);

		csr.m = AM->nr_rows;
		csr.n = AM->nr_cols;
		csr.nnz = AM->nr_nzeros;

		csr.ia = (MKL_INT *) mkl_malloc((csr.m+1 + VECTOR_ELEM_NUM) * sizeof(MKL_INT), 64);
		#pragma omp parallel for
		for (long i=0;i<csr.m+1;i++)
			csr.ia[i] = AM->row_ptr[i];
		free(AM->row_ptr);
		AM->row_ptr = NULL;

		csr.a = (ValueType *) mkl_malloc((csr.nnz + VECTOR_ELEM_NUM) * sizeof(ValueType), 64);
		#pragma omp parallel for
		for (long i=0;i<csr.nnz;i++)
			csr.a[i] = AM->values[i];
		free(AM->values);
		AM->values = NULL;

		csr.ja = (MKL_INT *) mkl_malloc((csr.nnz + VECTOR_ELEM_NUM) * sizeof(MKL_INT), 64);
		#pragma omp parallel for
		for (long i=0;i<csr.nnz;i++)
			csr.ja[i] = AM->col_ind[i];
		free(AM->col_ind);
		AM->col_ind = NULL;

		snprintf(buf, sizeof(buf), "'%d_%d_%d_%g_%g_%g_%g'", AM->nr_rows, AM->nr_cols, AM->nr_nzeros, AM->avg_bw, AM->std_bw, AM->avg_sc, AM->std_sc);

		compute(AM, buf);

		free_csr_matrix(AM);
	}

	return 0;
}
