#include <iostream>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <pthread.h>
#include <sstream>

#include <unistd.h>

#include "spmv_bench_common.h"

#ifdef __cplusplus
extern "C"{
#endif

	#include "macros/cpp_defines.h"
	#include "macros/macrolib.h"
	#include "time_it.h"
	#include "parallel_util.h"
	#include "pthread_functions.h"
	#include "matrix_util.h"
	#include "array_metrics.h"

	#include "string_util.h"
	#include "parallel_io.h"
	#include "storage_formats/openfoam/openfoam_matrix.h"
	#include "aux/csr_converter_double.h"
	// #include "aux/csr_converter.h"
	#include "aux/csr_util.h"

	#include "monitoring/power/rapl.h"

	#include "artificial_matrix_generation.h"

	#include "read_mtx.h"

#ifdef __cplusplus
}
#endif

#include "spmv_kernel.h"


#ifndef BLOCK_SIZE
	#define BLOCK_SIZE  64
#endif


int prefetch_distance = 8;


int num_procs;
int process_custom_id;


// Utils macro
#define Min(x,y) ((x)<(y)?(x):(y))
#define Max(x,y) ((x)>(y)?(x):(y))
#define Abs(x) ((x)>(0)?(x):-(x))

/** Simply return the max relative diff */
void
CheckAccuracy(ValueType * val, INT_T * rowind, INT_T * colind, INT_T m, INT_T nnz, ValueType * x, ValueType * y)
{
	#if DOUBLE == 0
		ValueType epsilon = 1e-7;
	#elif DOUBLE == 1
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
	ValueType maxDiff = 0, diff;
	// int cnt=0;
	for(int idx = 0 ; idx < m ; idx++)
	{
		diff = Abs(y_gold[idx]-y[idx]);
		// maxDiff = Max(maxDiff, diff);
		if (y_gold[idx] > epsilon)
		{
			// printf("i=%d, y_gold=%lf , y=%lf, diff = %lf\n", idx, y_gold[idx], y[idx], diff);
			diff = diff / abs(y_gold[idx]);
			maxDiff = Max(maxDiff, diff);
			// printf("error: i=%d/%d , a=%g f=%g , error=%g , max_error=%g\n", idx, m, y_gold[idx], y[idx], diff, maxDiff);
		}
		// if (diff > epsilon)
		// 	printf("error: i=%d/%d , a=%g f=%g\n", idx, m, y_gold[idx], y[idx]);
		// std::cout << idx << ": " << y_gold[idx]-y[idx] << "\n";
		// if (y_gold[idx] != 0.0)
		// {
		// 	if (Abs((y_gold[idx]-y[idx])/y_gold[idx]) > epsilon)
		// 		printf("Error: %lf != %lf , diff=%lf , diff_frac=%lf\n", y_gold[idx], y[idx], Abs(y_gold[idx]-y[idx]), Abs((y_gold[idx]-y[idx])/y_gold[idx]));
		// 	maxDiff = Max(maxDiff, Abs((y_gold[idx]-y[idx])/y_gold[idx]));
		// 	maxDiff = Max(maxDiff, Abs(y_gold[idx]-y[idx]));
		// }
	}
	if(maxDiff > epsilon)
		std::cout << "Test failed! (" << maxDiff << ")\n";
	#pragma omp parallel
	// {
	// 	double mae, max_ae, mse, mape, smape;
	// 	mae = array_mae_concurrent(y_gold, y, m, val_to_double);
	// 	max_ae = array_max_ae_concurrent(y_gold, y, m, val_to_double);
	// 	mse = array_mse_concurrent(y_gold, y, m, val_to_double);
	// 	mape = array_mape_concurrent(y_gold, y, m, val_to_double);
	// 	smape = array_smape_concurrent(y_gold, y, m, val_to_double);
	// 	#pragma omp single
	// 	printf("errors spmv: mae=%g, max_ae=%g, mse=%g, mape=%g, smape=%g\n", mae, max_ae, mse, mape, smape);
	// }
	delete[] y_gold;
}


int
is_directory(const char *path)
{
    struct stat stats;
    stat(path, &stats);
    // Check for file existence
    if (S_ISDIR(stats.st_mode))
        return 1;
    return 0;
}


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

bool keep_running = true;
void *run_command(void *arg) {
	// char *command = (char *) arg;
	char str1[] = "export XRT_TOOLS_NEXTGEN=True; xbutil examine -r electrical -d ";
	char *str2;
	str2 = NULL;
	str2 = (char *) getenv("DEV_ID");
	char str3[] = " | grep '  Power  ' | cut -d ':' -f 2 | cut -d ' ' -f 2;";
	char command[strlen(str1) + strlen(str2) + strlen(str3) + 1];
	strcpy(command, str1);
	strcat(command, str2);
	strcat(command, str3);
	// printf("command = %s\n", command);

	// while (keep_running)
	//    system(command);

	int cnt = 0;
	double Watt_fpga_sum = 0;
	while (keep_running){
		char buffer[1024];
		FILE *pipe = popen(command,"r");
		if(pipe==NULL)
			printf("Failed to open pipe to command\n");
		while(fgets(buffer, sizeof(buffer), pipe) != NULL){
			cnt+=1;
			// printf("%s", buffer);
			Watt_fpga_sum+=atof(buffer);
		}
		pclose(pipe);
	}
	double Watt_fpga = Watt_fpga_sum / cnt;
	// printf("finally, cnt = %d, average Watt is Watt_fpga = %lf\n", cnt, Watt_fpga);
	pthread_exit((void*) &Watt_fpga);
}

void sig_handler(int signo) {
	if (signo == SIGINT) {
		printf("Stopping repetitive execution...\n");
		keep_running = false;
	}
}


void
compute(char * matrix_name,
		INT_T * mtx_rowind, INT_T * mtx_colind, ValueType * mtx_val, INT_T mtx_m, __attribute__((unused)) INT_T mtx_n, INT_T mtx_nnz,
		__attribute__((unused)) INT_T * csr_ia, __attribute__((unused)) INT_T * csr_ja, __attribute__((unused)) ValueType * csr_a, INT_T csr_m, INT_T csr_n, INT_T csr_nnz,
		struct Matrix_Format * MF,
		csr_matrix * AM,
		ValueType * x, ValueType * y,
		const int loop = 1)
{
	int num_threads = omp_get_max_threads();
	int use_processes = atoi(getenv("USE_PROCESSES"));
	double gflops;
	__attribute__((unused)) double time, time_warm_up, time_after_warm_up;
	long buf_n = 10000;
	char buf[buf_n + 1];
	long i;

	// Warm up cpu.
	__attribute__((unused)) volatile double warmup_total;
	long A_warmup_n = (1<<20) * num_threads;
	double * A_warmup;
	time_warm_up = time_it(1,
		A_warmup = (typeof(A_warmup)) malloc(A_warmup_n * sizeof(*A_warmup));
		_Pragma("omp parallel for")
		for (long i=0;i<A_warmup_n;i++)
			A_warmup[i] = 0;
		for (long j=0;j<16;j++)
		{
			_Pragma("omp parallel for")
			for (long i=1;i<A_warmup_n;i++)
			{
				A_warmup[i] += A_warmup[i-1] * 7 + 3;
			}
		}
		warmup_total = A_warmup[A_warmup_n];
		free(A_warmup);
	);
	// fprintf(stderr, "time warm up %lf , n = %ld, total = %lf\n", time_warm_up, A_warmup_n, warmup_total);

	// Warm up caches.
	// time_warm_up = time_it(1,
	// 	MF->spmv(x, y);
	// );

	// time_after_warm_up = time_it(1,
	// 	MF->spmv(x, y);
	// );

	if (use_processes)
		raise(SIGSTOP);

	// #ifdef PRINT_STATISTICS
	// 	MF->statistics_start();
	// #endif
	
	// send CL::Buffers to FPGA (matrix and x vector)
	MF->copy_to_device(x);

	/*****************************************************************************************/
	pthread_t thread;
	signal(SIGINT, sig_handler);
	pthread_create(&thread, NULL, run_command, (void *) NULL);
	/*****************************************************************************************/

	time = 0;
	for(int idxLoop = 0 ; idxLoop < loop ; ++idxLoop){
		time += time_it(1,
			MF->spmv(x, y);
		);
	}
	// sleep(2);

	/*****************************************************************************************/
	/* Signal thread to stop running */
	keep_running = false;
	double *w_fpga_ret;
	pthread_join(thread, (void **) &w_fpga_ret);
	double W_avg_fpga = *w_fpga_ret;
	double J_estimated_fpga = W_avg_fpga * time*1.0;
	// printf("J_estimated_fpga = %lf\tW_avg_fpga = %lf\n", J_estimated_fpga, W_avg_fpga);
	// printf("LOOPS = %d\n", atoi(getenv("LOOPS")));
	/*****************************************************************************************/

	// receive CL::Buffers from FPGA (y vector)
	MF->copy_from_device(y);
	
	//=============================================================================
	//= Output section.
	//=============================================================================

	std::stringstream stream;
	// gflops = csr_nnz / time * loop * 2 * 1e-9;    // Use csr_nnz to be sure we have the initial nnz (there is no coo for artificial AM).
	gflops = csr_nnz / time * atoi(getenv("LOOPS")) * 2 * 1e-9;    // Use csr_nnz to be sure we have the initial nnz (there is no coo for artificial AM).
	printf("time = %lf, gflops = %lf\n", time, gflops);
	if (AM == NULL)
	{
		i = 0;
		i += snprintf(buf + i, buf_n - i, "%s", matrix_name);
		if (use_processes)
			i += snprintf(buf + i, buf_n - i, ",%d", num_procs);
		else
			i += snprintf(buf + i, buf_n - i, ",%d", omp_get_max_threads());
		i += snprintf(buf + i, buf_n - i, ",%u", csr_m);
		i += snprintf(buf + i, buf_n - i, ",%u", csr_n);
		i += snprintf(buf + i, buf_n - i, ",%u", csr_nnz);
		i += snprintf(buf + i, buf_n - i, ",%lf", time);
		i += snprintf(buf + i, buf_n - i, ",%lf", gflops);
		i += snprintf(buf + i, buf_n - i, ",%lf", MF->csr_mem_footprint / (1024*1024));
		i += snprintf(buf + i, buf_n - i, ",%lf", W_avg_fpga);
		i += snprintf(buf + i, buf_n - i, ",%lf", J_estimated_fpga);
		i += snprintf(buf + i, buf_n - i, ",%s", MF->format_name);
		i += snprintf(buf + i, buf_n - i, ",%u", MF->m);
		i += snprintf(buf + i, buf_n - i, ",%u", MF->n);
		i += snprintf(buf + i, buf_n - i, ",%u", MF->nnz);
		i += snprintf(buf + i, buf_n - i, ",%lf", MF->mem_footprint / (1024*1024));
		i += snprintf(buf + i, buf_n - i, ",%lf", MF->mem_footprint / MF->csr_mem_footprint);
		i += snprintf(buf + i, buf_n - i, ",%ld", atol(getenv("CSRCV_NUM_PACKET_VALS")));
		// #ifdef PRINT_STATISTICS
		// 	i += MF->statistics_print(buf + i, buf_n - i);
		// #endif
		buf[i] = '\0';
		fprintf(stderr, "%s\n", buf);

		CheckAccuracy(mtx_val, mtx_rowind, mtx_colind, mtx_m, mtx_nnz, x, y);
	}
	else
	{
		i = 0;
		i += snprintf(buf + i, buf_n - i, "synthetic");
		i += snprintf(buf + i, buf_n - i, ",%s" , AM->distribution);
		i += snprintf(buf + i, buf_n - i, ",%s" , AM->placement);
		i += snprintf(buf + i, buf_n - i, ",%d" , AM->seed);
		i += snprintf(buf + i, buf_n - i, ",%u" , AM->nr_rows);
		i += snprintf(buf + i, buf_n - i, ",%u" , AM->nr_cols);
		i += snprintf(buf + i, buf_n - i, ",%u" , AM->nr_nzeros);
		i += snprintf(buf + i, buf_n - i, ",%lf", AM->density);
		i += snprintf(buf + i, buf_n - i, ",%lf", AM->mem_footprint);
		i += snprintf(buf + i, buf_n - i, ",%s" , AM->mem_range);
		i += snprintf(buf + i, buf_n - i, ",%lf", AM->avg_nnz_per_row);
		i += snprintf(buf + i, buf_n - i, ",%lf", AM->std_nnz_per_row);
		i += snprintf(buf + i, buf_n - i, ",%lf", AM->avg_bw);
		i += snprintf(buf + i, buf_n - i, ",%lf", AM->std_bw);
		i += snprintf(buf + i, buf_n - i, ",%lf", AM->avg_bw_scaled);
		i += snprintf(buf + i, buf_n - i, ",%lf", AM->std_bw_scaled);
		i += snprintf(buf + i, buf_n - i, ",%lf", AM->avg_sc);
		i += snprintf(buf + i, buf_n - i, ",%lf", AM->std_sc);
		i += snprintf(buf + i, buf_n - i, ",%lf", AM->avg_sc_scaled);
		i += snprintf(buf + i, buf_n - i, ",%lf", AM->std_sc_scaled);
		i += snprintf(buf + i, buf_n - i, ",%lf", AM->skew);
		i += snprintf(buf + i, buf_n - i, ",%lf", AM->avg_num_neighbours);
		i += snprintf(buf + i, buf_n - i, ",%lf", AM->cross_row_similarity);
		i += snprintf(buf + i, buf_n - i, ",%s" , MF->format_name);
		i += snprintf(buf + i, buf_n - i, ",%lf", time);
		i += snprintf(buf + i, buf_n - i, ",%lf", gflops);
		i += snprintf(buf + i, buf_n - i, ",%lf", W_avg_fpga);
		i += snprintf(buf + i, buf_n - i, ",%lf", J_estimated_fpga);
		buf[i] = '\0';
		fprintf(stderr, "%s\n", buf);
	}
}


//==========================================================================================================================================
//= Main
//==========================================================================================================================================

int
main(int argc, char **argv)
{
	__attribute__((unused)) int num_threads;

	ValueType * mtx_val = NULL;
	INT_T * mtx_rowind = NULL;
	INT_T * mtx_colind = NULL;
	INT_T mtx_m = 0;
	INT_T mtx_n = 0;
	INT_T mtx_nnz = 0;

	ValueType * csr_a = NULL; // values (of size NNZ)
	INT_T * csr_ia = NULL;    // rowptr (of size m+1)
	INT_T * csr_ja = NULL;    // colidx of each NNZ (of size nnz)
	INT_T csr_m = 0;
	INT_T csr_n = 0;
	INT_T csr_nnz = 0;

	struct Matrix_Format * MF;   // Real matrices.
	csr_matrix * AM = NULL;
	ValueType * x;
	ValueType * y;
	char matrix_name[1000];
	__attribute__((unused)) double time;
	__attribute__((unused)) long i, j;

	// Wake omp up from eternal slumber.
	#pragma omp parallel
	{
		num_threads = omp_get_max_threads();
	}
	printf("max threads %d\n", num_threads);

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

	if (argc < 6)
	{
		char * file_in;
		i = 1;
		file_in = argv[i++];
		snprintf(matrix_name, sizeof(matrix_name), "%s", file_in);

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
	}
	else
	{
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

			if (i < argc)
				snprintf(matrix_name, sizeof(matrix_name), "%s_artificial", argv[i++]);
			else
				snprintf(matrix_name, sizeof(matrix_name), "%d_%d_%d_%g_%g_%g_%g", AM->nr_rows, AM->nr_cols, AM->nr_nzeros, AM->avg_bw, AM->std_bw, AM->avg_sc, AM->std_sc);
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
	}

	int dimMultipleBlock, dimMultipleBlock_y;
	dimMultipleBlock = ((csr_m+BLOCK_SIZE-1)/BLOCK_SIZE)*BLOCK_SIZE;
	dimMultipleBlock_y = ((csr_n+BLOCK_SIZE-1)/BLOCK_SIZE)*BLOCK_SIZE;

	// The following had to change, so that the allocation of the host pointer is aligned with what XRT (and OpenCL) wants. (do it like in OOPS_malloc)
	// x = (ValueType *) aligned_alloc(64, dimMultipleBlock_y * sizeof(ValueType));
	// #pragma omp parallel for
	// for(int i=0;i<dimMultipleBlock_y;++i)
	// 	x[i] = 1.0;
	x = (ValueType *) aligned_alloc(sysconf(_SC_PAGESIZE),(size_t)(csr_n * sizeof(ValueType)));;
	#pragma omp parallel for
	for(int i=0;i<csr_n;++i)
		x[i] = 1.0;
	y = (ValueType *) aligned_alloc(64, dimMultipleBlock * sizeof(ValueType));
	#pragma omp parallel for
	for(long i=0;i<dimMultipleBlock;i++)
		y[i] = 0.0;

	#if 0
		_Pragma("omp parallel")
		{
			int tnum = omp_get_thread_num();
			long i;
			long i_per_t = csr_n / num_threads;
			long i_s = tnum * i_per_t;

			// No operations.
			// _Pragma("omp parallel for")
			// for (i=0;i<csr_m+1;i++)
				// csr_ia[i] = 0;

			_Pragma("omp parallel for")
			for (i=0;i<csr_nnz;i++)
			{
				// csr_ja[i] = 0;                      // idx0 - Remove X access pattern dependency.
				// csr_ja[i] = i % csr_n;              // idx_serial - Remove X access pattern dependency.
				csr_ja[i] = i_s + (i % i_per_t);    // idx_t_local - Remove X access pattern dependency.
			}
		}
	#endif

	long buf_n = strlen(matrix_name) + 1 + 1000;
	char buf[buf_n];
	char * path, * filename, * filename_base;
	str_path_split_path(matrix_name, strlen(matrix_name) + 1, buf, buf_n, &path, &filename);
	path = strdup(path);
	filename = strdup(filename);
	str_path_split_ext(filename, strlen(filename) + 1, buf, buf_n, &filename_base, NULL);
	filename_base = strdup(filename_base);
	if (0)
	{
		printf("ploting : %s\n", filename_base);
		csr_plot(filename_base, csr_ia, csr_ja, csr_a, csr_m, csr_n, csr_nnz, 0);
		return 0;
	}


	// The following change (inclusion of "x" in csr_to_format conversion) had to happen here for the FPGA, 
	// because "x" needs to be loaded beforehand to OpenCL buffer. Sorry for that.
	// MF = csr_to_format(csr_ia, csr_ja, csr_a, csr_m, csr_n, csr_nnz);
	MF = csr_to_format(csr_ia, csr_ja, csr_a, csr_m, csr_n, csr_nnz, x);

	prefetch_distance = 1;
	time = time_it(1,
		// for (i=0;i<5;i++)
		{
			// fprintf(stderr, "prefetch_distance = %d\n", prefetch_distance);
			compute(matrix_name, 
				mtx_rowind, mtx_colind, mtx_val, mtx_m, mtx_n, mtx_nnz,
				csr_ia, csr_ja, csr_a, csr_m, csr_n, csr_nnz,
				MF, AM, x, y);
			prefetch_distance++;
		}
	);
	if (atoi(getenv("COOLDOWN")) == 1)
	{
		printf("time total = %g, sleeping\n", time);
		usleep((long) (time * 1000000));
	}

	free_csr_matrix(AM);
	free(x);
	free(y);

	return 0;
}

