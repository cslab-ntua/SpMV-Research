#include <iostream>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <pthread.h>
#include <sstream>

#include <unistd.h>

#include "bench_common.h"

#ifdef __cplusplus
extern "C"{
#endif

	#include "macros/cpp_defines.h"
	#include "macros/macrolib.h"
	#include "time_it.h"
	#include "parallel_util.h"
	#include "pthread_functions.h"
	#include "topology/hardware_topology.h"
	#include "matrix_util.h"
	#include "array_metrics.h"

	#include "string_util.h"
	#include "random.h"
	#include "io.h"
	#include "parallel_io.h"
	#include "storage_formats/matrix_market/matrix_market.h"
	#include "storage_formats/openfoam/openfoam_matrix.h"

	#include "aux/csr_converter_reference.h"

	#include "monitoring/power/rapl.h"

	#include "artificial_matrix_generation.h"

	#include "aux/rcm.h"

	#include "aux/dynamic_array.h"

	#include "functools/functools_gen_undef.h"
	#define FUNCTOOLS_GEN_TYPE_1  INT_T
	#define FUNCTOOLS_GEN_TYPE_2  INT_T
	#define FUNCTOOLS_GEN_SUFFIX  CONCAT(_BENCH_CPP, BENCH_CPP_SUFFIX)
	#include "functools/functools_gen.c"
	static _TYPE_OUT functools_map_fun(_TYPE_IN * A, long i)
	{
		return A[i];
	}
	static _TYPE_OUT functools_reduce_fun(_TYPE_OUT a, _TYPE_OUT b)
	{
		return a + b;
	}

#ifdef __cplusplus
}
#endif

#include "spmv_kernels/spmv_kernel.h"


int num_procs;
int process_custom_id;


void bench(struct CSR_reference_s * csr, struct Matrix_Format * MF, long print_labels_and_exit);


int
get_pinning_position_from_affinity_string(const char * range_string, long len, int target_pos)
{
	long pos = 0;
	int aff = -1;
	int n1, n2;
	long i;
	for (i=0;i<len;)
	{
		n1 = atoi(&range_string[i]);
		if (pos == target_pos)
		{
			aff = n1;
			break;
		}
		while ((i < len) && (range_string[i] != ',') && (range_string[i] != '-'))
			i++;
		if (i+1 >= len)
			break;
		if (range_string[i] == ',')
		{
			pos++;
			i++;
		}
		else
		{
			i++;
			n2 = atoi(&range_string[i]);
			if (n2 < n1)
				error("Bad affinity string format.");
			if (pos + n2 - n1 >= target_pos)
			{
				aff = n1 + target_pos - pos;
				break;
			}
			pos += n2 - n1 + 1;
			while ((i < len) && (range_string[i] != ','))
				i++;
			i++;
			if (i >= len)
				break;
		}
	}
	if (aff < 0)
		error("Bad affinity string format.");
	return aff;
}


//==========================================================================================================================================
//= Import Matrix
//==========================================================================================================================================


static
void
import_file(struct CSR_reference_s * csr)
{
	long symmetric = 0;
	long expand_symmetry = 1;
	#ifdef KEEP_SYMMETRY
		expand_symmetry = 0;
	#else
		expand_symmetry = 1;
	#endif

	struct Matrix_Market * MTX = NULL;
	ValueTypeReference * coo_val = NULL;   // MATRIX_MARKET_FLOAT_T is always double, as reference for error calculation.
	INT_T * coo_rowind = NULL;
	INT_T * coo_colind = NULL;
	long coo_m = 0;
	long coo_n = 0;
	long coo_nnz = 0;
	long coo_nnz_matrix = 0;
	long coo_nnz_diag = 0;
	long coo_nnz_non_diag = 0;

	double time;

	long buf_n = 1000;
	char buf[buf_n];
	snprintf(buf, buf_n, "%s", csr->filename);
	csr->matrix_name = strndup(buf, buf_n);

	time = time_it(1,
		if (stat_isdir(csr->filename))   // OpenFoam format
		{
			long nnz_non_diag, N;
			int * rowind, * colind;
			read_openfoam_matrix_dir(csr->filename, &rowind, &colind, &N, &nnz_non_diag);
			coo_m = N;
			coo_n = N;
			coo_nnz = N + nnz_non_diag;
			coo_nnz_matrix = N + 2*nnz_non_diag;
			coo_rowind = (typeof(coo_rowind)) aligned_alloc(64, coo_nnz * sizeof(*coo_rowind));
			coo_colind = (typeof(coo_colind)) aligned_alloc(64, coo_nnz * sizeof(*coo_colind));
			coo_val = (typeof(coo_val)) aligned_alloc(64, coo_nnz * sizeof(*coo_val));
			_Pragma("omp parallel for")
			for (long i=0;i<coo_nnz;i++)
			{
				coo_rowind[i] = rowind[i];
				coo_colind[i] = colind[i];
				coo_val[i] = 1.0;
			}
			free(rowind);
			free(colind);
		}
		else   // MTX format
		{
			long pattern_dummy_vals = 1;
			MTX = mtx_read(csr->filename, expand_symmetry, pattern_dummy_vals);
			coo_rowind = MTX->R;
			coo_colind = MTX->C;
			coo_m = MTX->m;
			coo_n = MTX->n;
			coo_nnz_diag = MTX->nnz_diag;
			coo_nnz_non_diag = MTX->nnz_non_diag;
			symmetric = MTX->symmetric;
			coo_nnz = MTX->nnz_stored;
			coo_nnz_matrix = MTX->nnz_matrix;
			mtx_values_convert_to_real(MTX);
			coo_val = (typeof(coo_val)) MTX->V;
			MTX->R = NULL;
			MTX->C = NULL;
			MTX->V = NULL;
			mtx_destroy(&MTX);
		}
	);
	printf("time read: %lf\n", time);

	time = time_it(1,
		csr->a_ref = (typeof(csr->a_ref)) aligned_alloc(64, coo_nnz * sizeof(*csr->a_ref));
		csr->ja = (typeof(csr->ja)) aligned_alloc(64, coo_nnz * sizeof(*csr->ja));
		csr->ia = (typeof(csr->ia)) aligned_alloc(64, (coo_m+1) * sizeof(*csr->ia));
		csr->m = coo_m;
		csr->n = coo_n;
		csr->nnz = coo_nnz;
		csr->nnz_matrix = coo_nnz_matrix;
		csr->nnz_diag = coo_nnz_diag;
		csr->nnz_non_diag = coo_nnz_non_diag;
		csr->symmetric = symmetric;
		csr->expanded_symmetry = expand_symmetry;
		_Pragma("omp parallel for")
		for (long i=0;i<coo_nnz;i++)
		{
			csr->a_ref[i] = 0.0;
			csr->ja[i] = 0;
		}
		_Pragma("omp parallel for")
		for (long i=0;i<coo_m+1;i++)
		csr->ia[i] = 0;
		coo_to_csr(coo_rowind, coo_colind, coo_val, coo_m, coo_n, coo_nnz, csr->ia, csr->ja, csr->a_ref, 1, 0);
		free(coo_rowind);
		free(coo_colind);
		free(coo_val);

		// _Pragma("omp parallel for")
		// for (long i=0;i<csr->m;i++)
		// {
			// if (csr->ia[i+1] - csr->ia[i] == 0)
				// error("test");
		// }
		// exit(0);

	);
	printf("time coo to csr: %lf\n", time);
}


static
void
import_artificial_matrix(struct CSR_reference_s * csr)
{
	struct csr_matrix * AM = NULL;
	double time;

	time = time_it(1,
		AM = artificial_matrix_generation(csr->AM_stats.nr_rows, csr->AM_stats.nr_cols, csr->AM_stats.avg_nnz_per_row,
			csr->AM_stats.std_nnz_per_row, csr->AM_stats.distribution, csr->AM_stats.seed, csr->AM_stats.placement,
			csr->AM_stats.avg_bw, csr->AM_stats.skew, csr->AM_stats.avg_num_neighbours, csr->AM_stats.cross_row_similarity);
	);
	printf("time generate artificial matrix: %lf\n", time);

	csr->m = AM->nr_rows;
	csr->n = AM->nr_cols;
	csr->nnz = AM->nr_nzeros;
	csr->nnz_matrix = AM->nr_nzeros;

	csr->ia = (typeof(csr->ia)) aligned_alloc(64, (csr->m+1) * sizeof(*csr->ia));
	#pragma omp parallel for
	for (long i=0;i<csr->m+1;i++)
		csr->ia[i] = AM->row_ptr[i];
	free(AM->row_ptr);
	AM->row_ptr = NULL;

	csr->a_ref = (typeof(csr->a_ref)) aligned_alloc(64, csr->nnz * sizeof(*csr->a_ref));
	csr->ja = (typeof(csr->ja)) aligned_alloc(64, csr->nnz * sizeof(*csr->ja));
	#pragma omp parallel for
	for (long i=0;i<csr->nnz;i++)
	{
		csr->a_ref[i] = AM->values[i];
		csr->ja[i] = AM->col_ind[i];
	}
	free(AM->values);
	AM->values = NULL;
	free(AM->col_ind);
	AM->col_ind = NULL;

	csr->AM_stats = *AM;

	free_csr_matrix(AM);
}


//==========================================================================================================================================
//= Transforms
//==========================================================================================================================================


void
rcm(struct CSR_reference_s * csr)
{
	long i;

	if (!csr->symmetric)
		error("RCM is only applicable to symmetric matrices");

	int * permutation;
	int * row_ptr_new = NULL;
	int * col_idx_new = NULL;
	ValueTypeReference * values_new = NULL;
	long nnz_new, nnz_diag;

	if (!csr->expanded_symmetry)
	{
		printf("expanding symmetry for rcm\n");
		csr_expand_symmetric(csr->ia, csr->ja, csr->a_ref, csr->m, csr->n, csr->nnz, &row_ptr_new, &col_idx_new, &values_new, &nnz_new, &nnz_diag, 1);
		printf("nnz_old=%ld nnz_new=%ld csr->nnz_diag=%ld csr->nnz_non_diag=%ld nnz_diag=%ld \n", csr->nnz, nnz_new, csr->nnz_diag, csr->nnz_non_diag, nnz_diag);
		csr->nnz = nnz_new;
		free(csr->ia);
		free(csr->ja);
		free(csr->a_ref);
		csr->ia = row_ptr_new;
		csr->ja = col_idx_new;
		csr->a_ref = values_new;
	}

	long sort_rows = 1;
	reverse_cuthill_mckee(csr->ia, csr->ja, csr->a_ref, csr->m, csr->n, csr->nnz, sort_rows, &row_ptr_new, &col_idx_new, &values_new, &permutation);
	printf("nnz_old=%ld csr->nnz_diag=%ld csr->nnz_non_diag=%ld \n", csr->nnz, csr->nnz_diag, csr->nnz_non_diag);
	free(csr->ia);
	free(csr->ja);
	free(csr->a_ref);
	csr->ia = row_ptr_new;
	csr->ja = col_idx_new;
	csr->a_ref = values_new;
	if (csr->n != csr->m)
		error("csr->n != csr->m");
	for (i=0;i<csr->m;i++)
	{
		if (csr->ia[i] > csr->ia[i+1])
			error("csr->ia[%d]=%d > csr->ia[%d]=%d", i, csr->ia[i], i+1, csr->ia[i+1]);
	}
	for (i=0;i<=csr->m;i++)
	{
		if (csr->ia[i] < 0 || csr->ia[i] > csr->nnz)
			error("csr->ia[%d]=%d >= csr->nnz", i, csr->ia[i]);
	}
	for (i=0;i<csr->nnz;i++)
	{
		if (csr->ja[i] < 0 || csr->ja[i] >= csr->n)
			error("csr->ja[%d]=%d >= csr->n", i, csr->ja[i]);
	}

	if (!csr->expanded_symmetry)
	{
		printf("dropping upper matrix after rcm\n");
		csr_drop_upper(csr->ia, csr->ja, csr->a_ref, csr->m, csr->n, csr->nnz, &row_ptr_new, &col_idx_new, &values_new, &nnz_new, NULL, 1);
		csr->nnz = nnz_new;
		free(csr->ia);
		free(csr->ja);
		free(csr->a_ref);
		csr->ia = row_ptr_new;
		csr->ja = col_idx_new;
		csr->a_ref = values_new;
	}
}


// Fix zero diagonal entries by replacing them with a random number in [0, 1).
void
fix_diagonal_zeros(struct CSR_reference_s * csr)
{
	int num_threads = omp_get_max_threads();
	INT_T * ia_new = (typeof(ia_new)) aligned_alloc(64, (csr->m + 1) * sizeof(ia_new));
	long diag_nnz_missing = 0;
	#pragma omp parallel
	{
		int tnum = omp_get_thread_num();
		struct Random_State * rs = random_new(tnum);
		long t_diag_nnz_missing = 0;
		long i, i_s, i_e, j;
		loop_partitioner_balance_prefix_sums(num_threads, tnum, csr->ia, csr->m, csr->nnz, &i_s, &i_e);
		#pragma omp for
		for (i=0;i<csr->m;i++)
		{
			ia_new[i] = 1;
		}
		ia_new[csr->m] = 0;
		for (i=i_s;i<i_e;i++)
		{
			for (j=csr->ia[i];j<csr->ia[i+1];j++)
			{
				if (i == csr->ja[j])
				{
					if (csr->a_ref[j] == 0)
						csr->a_ref[j] = random_uniform(rs, 0, 1);
					ia_new[i] = 0;
					break;
				}
			}
		}
		#pragma omp barrier
		#pragma omp for
		for (i=0;i<csr->m;i++)
		{
			if (ia_new[i] == 1)
				t_diag_nnz_missing++;
		}
		__atomic_fetch_add(&diag_nnz_missing, t_diag_nnz_missing, __ATOMIC_RELAXED);
		random_destroy(&rs);
	}
	if (diag_nnz_missing > 0)
	{
		printf("Adding %ld missing diagonal nonzeros\n", diag_nnz_missing);
		ValueTypeReference * a_new = (typeof(a_new)) aligned_alloc(64, (csr->nnz+diag_nnz_missing) * sizeof(a_new));
		INT_T * ja_new = (typeof(ja_new)) aligned_alloc(64, (csr->nnz+diag_nnz_missing) * sizeof(ja_new));
		scan_reduce(ia_new, ia_new, csr->m+1, 0, 1, 0);
		#pragma omp parallel
		{
			int tnum = omp_get_thread_num();
			struct Random_State * rs = random_new(tnum);
			long i, i_s, i_e, j, j_new, k;
			long degree_old, degree_new;
			loop_partitioner_balance_prefix_sums(num_threads, tnum, csr->ia, csr->m, csr->nnz, &i_s, &i_e);
			#pragma omp for
			for (i=0;i<csr->m+1;i++)
			{
				ia_new[i] += csr->ia[i];
			}
			for (i=i_s;i<i_e;i++)
			{
				degree_old = csr->ia[i+1] - csr->ia[i];
				degree_new = ia_new[i+1] - ia_new[i];
				if (degree_new < degree_old)
					error("new degree smaller than old");
				if (degree_new - degree_old > 1)
					error("added more than one nnz");
				for (k=0;k<degree_old;k++)
				{
					j = csr->ia[i] + k;
					j_new = ia_new[i] + k;
					ja_new[j_new] = csr->ja[j];
					a_new[j_new] = csr->a_ref[j];
				}
				if (degree_old != degree_new)
				{
					j_new = ia_new[i] + degree_new - 1;
					ja_new[j_new] = i;
					a_new[j_new] = random_uniform(rs, 0, 1);
				}
			}
			random_destroy(&rs);
		}

		csr->nnz_diag += diag_nnz_missing;
		csr->nnz += diag_nnz_missing;

		free(csr->a_ref);
		free(csr->ia);
		free(csr->ja);
		csr->a_ref = a_new;
		csr->ia = ia_new;
		csr->ja = ja_new;
	}
	else
	{
		free(ia_new);
	}
	printf("test\n");
}

//==========================================================================================================================================
//= Main
//==========================================================================================================================================


int
main(int argc, char **argv)
{
	int num_threads;
	struct CSR_reference_s * csr = (typeof(csr)) aligned_alloc(64, sizeof(*csr));

	csr->a_ref = NULL;
	csr->ia = NULL;
	csr->ja = NULL;
	csr->m = 0;
	csr->n = 0;
	csr->nnz = 0;
	csr->nnz_diag = 0;
	csr->nnz_non_diag = 0;
	csr->symmetric = 0;
	csr->expanded_symmetry = 1;
	#ifdef KEEP_SYMMETRY
		csr->expanded_symmetry = 0;
	#else
		csr->expanded_symmetry = 1;
	#endif

	struct Matrix_Format * MF;   // Real matrices.

	__attribute__((unused)) double time;
	__attribute__((unused)) long i, j;

	int use_artificial_matrices = atoi(getenv("USE_ARTIFICIAL_MATRICES"));

	// Wake omp up from eternal slumber.
	#pragma omp parallel
	{
		num_threads = omp_get_max_threads();
	}
	printf("max threads %d\n", num_threads);

	// Just print the labels and exit.
	if (argc == 1)
	{
		bench(NULL, NULL, 1);
		return 0;
	}

	int use_processes = atoi(getenv("USE_PROCESSES"));
	if (use_processes)
	{
		num_procs = atoi(getenv("NUM_PROCESSES"));
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
				char * gomp_aff_str = getenv("GOMP_CPU_AFFINITY");
				long len = strlen(gomp_aff_str);
				long buf_n = 1000;
				char buf[buf_n];
				process_custom_id = j;
				core = get_pinning_position_from_affinity_string(gomp_aff_str, len, j);
				tid = pthread_self();
				set_affinity(tid, core);
				snprintf(buf, buf_n, "%d", core);             // Also set environment variables for other libraries that might try to change affinity themselves.
				setenv("GOMP_CPU_AFFINITY", buf, 1);          // Setting 'XLSMPOPTS' has no effect after the program has started.
				// printf("%ld: affinity=%d\n", j, core);
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
	}

child_proc_label:

	// Read real matrix.
	if (!use_artificial_matrices)
	{
		csr->filename = strdup(argv[1]);
		import_file(csr);
	}
	else
	{
		long buf_n = 1000;
		char buf[buf_n];
		long i;
		i = 1;
		csr->AM_stats.nr_rows = atoi(argv[i++]);
		csr->AM_stats.nr_cols = atoi(argv[i++]);
		csr->AM_stats.avg_nnz_per_row = atof(argv[i++]);
		csr->AM_stats.std_nnz_per_row = atof(argv[i++]);
		csr->AM_stats.distribution = argv[i++];
		csr->AM_stats.placement = argv[i++];
		csr->AM_stats.avg_bw = atof(argv[i++]);
		csr->AM_stats.skew = atof(argv[i++]);
		csr->AM_stats.avg_num_neighbours = atof(argv[i++]);
		csr->AM_stats.cross_row_similarity = atof(argv[i++]);
		csr->AM_stats.seed = atoi(argv[i++]);
		import_artificial_matrix(csr);
		if (i < argc)
			snprintf(buf, buf_n, "%s_artificial", argv[i++]);
		else
			snprintf(buf, buf_n, "%d_%d_%d_%g_%g_%g_%g", csr->AM_stats.nr_rows, csr->AM_stats.nr_cols, csr->AM_stats.nr_nzeros, csr->AM_stats.avg_bw, csr->AM_stats.std_bw, csr->AM_stats.avg_sc, csr->AM_stats.std_sc);
		csr->matrix_name = strndup(buf, buf_n);
	}

	#ifdef FIX_DIAGONAL_ZEROS
		fix_diagonal_zeros(csr);
	#endif

	#if 0
		_Pragma("omp parallel")
		{
			int tnum = omp_get_thread_num();
			long i;
			long i_per_t = csr->n / num_threads;
			long i_s = tnum * i_per_t;

			// No operations.
			// _Pragma("omp parallel for")
			// for (i=0;i<csr->m+1;i++)
				// csr->ia[i] = 0;

			_Pragma("omp parallel for")
			for (i=0;i<csr->nnz;i++)
			{
				csr->ja[i] = 0;                      // idx0 - Remove X access pattern dependency.
				// csr->ja[i] = i % csr->n;              // idx_serial - Remove X access pattern dependency.
				// csr->ja[i] = i_s + (i % i_per_t);    // idx_t_local - Remove X access pattern dependency.
			}
		}
	#endif

	if (atoi(getenv("USE_RCM_REORDERING")) == 1)
	{
		time = time_it(1,
			rcm(csr);
		);
		printf("time rcm reordering: %lf\n", time);
	}

	time = time_it(1,
		MF = csr_to_format(csr->ia, csr->ja, csr->a_ref, csr->m, csr->n, csr->nnz, csr->symmetric, csr->expanded_symmetry);
	);
	printf("time convert to format: %lf\n", time);

	// Reallocate CSR arrays to ensure the format does not rely on them.
	{
		ValueTypeReference * a_tmp;
		INT_T * ia_tmp, * ja_tmp;

		a_tmp = (typeof(a_tmp)) aligned_alloc(64, csr->nnz * sizeof(*a_tmp));
		ia_tmp = (typeof(ia_tmp)) aligned_alloc(64, (csr->m+1) * sizeof(*ia_tmp));
		ja_tmp = (typeof(ja_tmp)) aligned_alloc(64, csr->nnz * sizeof(*ja_tmp));
		#pragma omp parallel for
		for (long i=0;i<csr->m+1;i++)
			ia_tmp[i] = csr->ia[i];
		free(csr->ia);
		csr->ia = ia_tmp;

		#pragma omp parallel for
		for(long i=0;i<csr->nnz;i++)
		{
			a_tmp[i] = csr->a_ref[i];
			ja_tmp[i] = csr->ja[i];
		}
		free(csr->a_ref);
		free(csr->ja);
		csr->a_ref = a_tmp;
		csr->ja = ja_tmp;
	}

	time = time_it(1,
		bench(csr, MF, 0);
	);
	if (atoi(getenv("COOLDOWN")) == 1)
	{
		printf("time total = %g, sleeping\n", time);
		usleep((long) (time * 1000000));
	}

	return 0;
}

