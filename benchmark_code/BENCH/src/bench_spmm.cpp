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

#ifdef SDV_TRACING
	#include "sdv_tracing.h"
#endif

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
	#include "monitoring/power/rapl.h"

	#include "aux/csr_converter_reference.h"
	#include "aux/dynamic_array.h"

	#include "artificial_matrix_generation.h"

#ifdef __cplusplus
}
#endif

#include "spmm_kernels/spmm_kernel.h"


extern int num_procs;

long num_loops_out;


// Utils macro
#define Min(x,y) ((x)<(y)?(x):(y))
#define Max(x,y) ((x)>(y)?(x):(y))
#define Abs(x) ((x)>(0)?(x):-(x))


// #define ValueTypeValidation  double
// #define ValueTypeValidation  long double
// #define ValueTypeValidation  __float128
#define ValueTypeValidation  _Float128

static inline
double
reference_to_double(void * A, long i)
{
	return (double) ((ValueTypeValidation *) A)[i];
}


long
check_accuracy_labels(char * buf, long buf_n)
{
	long len = 0;
	len += snprintf(buf + len, buf_n - len, ",%s", "spmv_mae");
	len += snprintf(buf + len, buf_n - len, ",%s", "spmv_max_ae");
	len += snprintf(buf + len, buf_n - len, ",%s", "spmv_mse");
	len += snprintf(buf + len, buf_n - len, ",%s", "spmv_mape");
	len += snprintf(buf + len, buf_n - len, ",%s", "spmv_smape");
	len += snprintf(buf + len, buf_n - len, ",%s", "spmv_lnQ_error");
	len += snprintf(buf + len, buf_n - len, ",%s", "spmv_mlare");
	len += snprintf(buf + len, buf_n - len, ",%s", "spmv_gmare");
	return len;
}

long
check_accuracy(char * buf, long buf_n, INT_T * csr_ia, INT_T * csr_ja, ValueTypeReference * csr_a_ref,
		long csr_m, __attribute__((unused)) long csr_n, __attribute__((unused)) long csr_nnz,
		ValueTypeReference * x_ref, ValueType * y, long symmetric, long expanded_symmetry)
{
	__attribute__((unused)) ValueTypeValidation epsilon_relaxed = 1e-4;
	#if DOUBLE == 0
		ValueTypeValidation epsilon = 1e-7;
	#elif DOUBLE == 1
		ValueTypeValidation epsilon = 1e-10;
	#endif
	ValueTypeValidation * y_gold = (typeof(y_gold)) malloc(csr_m * sizeof(*y_gold));
	ValueTypeValidation * y_test = (typeof(y_test)) malloc(csr_m * sizeof(*y_test));
	long i;

	#pragma omp parallel
	{
		long i;
		#pragma omp for
		for(i=0;i<csr_m;i++)
		{
			y_gold[i] = 0;
			y_test[i] = y[i];
		}
	}

	if (symmetric && !expanded_symmetry)
	{
		long i, j, col;
		for (i=0;i<csr_m;i++)
		{
			for (j=csr_ia[i];j<csr_ia[i+1];j++)
			{
				col = csr_ja[j];
				y_gold[i] += csr_a_ref[j] * x_ref[col];
				if (i != col)
					y_gold[col] += csr_a_ref[j] * x_ref[i];
			}
		}
	}
	else
	{
		#pragma omp parallel
		{
			ValueTypeValidation sum;
			long i, j;
			#pragma omp for
			for (i=0;i<csr_m;i++)
			{
				ValueTypeValidation val, tmp, compensation;
				compensation = 0;
				sum = 0;
				for (j=csr_ia[i];j<csr_ia[i+1];j++)
				{
					val = csr_a_ref[j] * x_ref[csr_ja[j]] - compensation;
					tmp = sum + val;
					compensation = (tmp - sum) - val;
					sum = tmp;
				}
				y_gold[i] = sum;
			}
		}
	}

	ValueTypeValidation maxDiff = 0, diff;
	// long cnt=0;
	for(i=0;i<csr_m;i++)
	{
		diff = Abs(y_gold[i] - y_test[i]);
		// maxDiff = Max(maxDiff, diff);
		if (y_gold[i] > epsilon)
		{
			diff = diff / abs(y_gold[i]);
			maxDiff = Max(maxDiff, diff);
		}
		// if (diff > epsilon_relaxed)
			// printf("error: i=%ld/%ld , a=%.10g f=%.10g\n", i, csr_m-1, (double) y_gold[i], (double) y_test[i]);
		// if(i<5)
		// if((double)y_gold[i]-(double)y_test[i])
		// 	printf("y_gold[%ld] = %.4lf, y_test[%ld] = %.4lf\n", i, (double)y_gold[i], i, (double)y_test[i]);
		// std::cout << i << ": " << y_gold[i]-y_test[i] << "\n";
		// if (y_gold[i] != 0.0)
		// {
			// if (Abs((y_gold[i]-y_test[i])/y_gold[i]) > epsilon)
				// printf("Error: %g != %g , diff=%g , diff_frac=%g\n", y_gold[i], y_test[i], Abs(y_gold[i]-y_test[i]), Abs((y_gold[i]-y_test[i])/y_gold[i]));
			// maxDiff = Max(maxDiff, Abs((y_gold[i]-y_test[i])/y_gold[i]));
			// maxDiff = Max(maxDiff, Abs(y_gold[i]-y_test[i]));
		// }
	}
	if(maxDiff > epsilon)
		printf("Test failed! (%g)\n", reference_to_double(&maxDiff, 0));
	long len = 0;
	#pragma omp parallel
	{
		double mae, max_ae, mse, mape, smape;
		double lnQ_error, mlare, gmare;
		array_mae_concurrent(y_gold, y_test, csr_m, &mae, reference_to_double);
		array_max_ae_concurrent(y_gold, y_test, csr_m, &max_ae, reference_to_double);
		array_mse_concurrent(y_gold, y_test, csr_m, &mse, reference_to_double);
		array_mape_concurrent(y_gold, y_test, csr_m, &mape, reference_to_double);
		array_smape_concurrent(y_gold, y_test, csr_m, &smape, reference_to_double);
		array_lnQ_error_concurrent(y_gold, y_test, csr_m, &lnQ_error, reference_to_double);
		array_mlare_concurrent(y_gold, y_test, csr_m, &mlare, reference_to_double);
		array_gmare_concurrent(y_gold, y_test, csr_m, &gmare, reference_to_double);
		#pragma omp single
		{
			printf("errors spmv: mae=%g, max_ae=%g, mse=%g, mape=%g, smape=%g, lnQ_error=%g, mlare=%g, gmare=%g\n", mae, max_ae, mse, mape, smape, lnQ_error, mlare, gmare);
			len += snprintf(buf + len, buf_n - len, ",%g", mae);
			len += snprintf(buf + len, buf_n - len, ",%g", max_ae);
			len += snprintf(buf + len, buf_n - len, ",%g", mse);
			len += snprintf(buf + len, buf_n - len, ",%g", mape);
			len += snprintf(buf + len, buf_n - len, ",%g", smape);
			len += snprintf(buf + len, buf_n - len, ",%g", lnQ_error);
			len += snprintf(buf + len, buf_n - len, ",%g", mlare);
			len += snprintf(buf + len, buf_n - len, ",%g", gmare);
		}
	}

	// for (i=0;i<csr_m;i++)
	// {
		// printf("%g\n", y[i]);
	// }

	free(y_gold);
	free(y_test);
	return len;
}



int
qsort_cmp(const void * a_ptr, const void * b_ptr)
{
	double a = *((double *) a_ptr);
	double b = *((double *) b_ptr);
	return (a > b) ? 1 : (a < b) ? -1 : 0;
}

void
compute(struct CSR_reference_s * csr, struct Matrix_Format * MF,
		long K, ValueType * x, ValueTypeReference * x_ref, ValueType * y,
		long min_num_loops, double min_runtime, long print_labels_and_exit)
{
	int num_threads = omp_get_max_threads();
	int use_processes = atoi(getenv("USE_PROCESSES"));
	long num_loops;
	double gflops;
	__attribute__((unused)) double time_total, time_iter, time_min, time_max, time_median, time_warm_up, time_after_warm_up;
	long buf_n = 10000;
	char buf[buf_n + 1];
	long i, j;
	double J_estimated, W_avg;
	int use_artificial_matrices = atoi(getenv("USE_ARTIFICIAL_MATRICES"));

	if (!print_labels_and_exit)
	{
		// Warm up cpu.
		__attribute__((unused)) volatile double warmup_total;
		long A_warmup_n = (1ULL<<20) * num_threads;
		double * A_warmup;
		time_warm_up = time_it(1,
			A_warmup = (typeof(A_warmup)) malloc(A_warmup_n * sizeof(*A_warmup));
			_Pragma("omp parallel for")
			for (long i=0;i<A_warmup_n;i++)
				A_warmup[i] = 0;
			for (j=0;j<16;j++)
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
		printf("time warm up %lf\n", time_warm_up);

		int gpu_kernel = atoi(getenv("GPU_KERNEL"));
		if (gpu_kernel)
		{
			time_warm_up = time_it(1,
				for(int i=0;i<1000;i++)
					MF->spmv(x, y);
			);
		}
		else
		{
			// Warm up caches.
			time_warm_up = time_it(1,
				MF->spmv(x, y);
			);			
		}

		if (use_processes)
			raise(SIGSTOP);

		#ifdef PRINT_STATISTICS
			MF->statistics_start();
		#endif

		/*****************************************************************************************/
		struct RAPL_Register * regs;
		long regs_n;
		char * reg_ids;

		reg_ids = NULL;
		reg_ids = (char *) getenv("RAPL_REGISTERS");

		rapl_open(reg_ids, &regs, &regs_n);
		/*****************************************************************************************/

		#ifdef SDV_TRACING
			printf("SDV tracing enabled\n");
			trace_enable(); 
		#endif

		volatile unsigned long * L3_cache_block;
		long L3_cache_block_n = topohw_get_cache_size(0, 3, TOPOHW_CT_U)  / sizeof(*L3_cache_block);
		if (L3_cache_block_n == 0)
			L3_cache_block_n = (1ULL<<20) * num_threads;
		L3_cache_block = (typeof(L3_cache_block)) malloc(L3_cache_block_n * sizeof(*L3_cache_block));
		int clear_caches = atoi(getenv("CLEAR_CACHES"));
		time_total = 0;
		num_loops = 0;
		dynarray_d * da_iter_times = dynarray_new_d(10 * min_num_loops);
		while (time_total < min_runtime || num_loops < min_num_loops)
		{
			if (__builtin_expect(clear_caches, 0))
			{
				if (num_loops >= min_num_loops)
					break;
				_Pragma("omp parallel")
				{
					long i;
					_Pragma("omp for")
					for (i=0;i<L3_cache_block_n;i++)
						L3_cache_block[i] = 0;
				}
			}

			rapl_read_start(regs, regs_n);

			#ifdef SDV_TRACING
				char region_name[] = "COMPUTATION-SpMM";
				trace_begin_region(region_name);
			#endif

			time_iter = time_it(1,
				MF->spmv(x, y);
			);

			#ifdef SDV_TRACING
				trace_end_region(region_name);
			#endif

			rapl_read_end(regs, regs_n);

			dynarray_push_back_d(da_iter_times, time_iter);
			time_total += time_iter;
			num_loops++;
		}
		num_loops_out = num_loops;
		printf("number of loops = %ld\n", num_loops);
		long iter_times_n;
		double * iter_times;
		iter_times_n = dynarray_export_array_d(da_iter_times, &iter_times);
		if (iter_times_n != num_loops)
			error("dynamic array size not equal to number of loops: %ld != %ld", iter_times_n, num_loops);
		qsort(iter_times, num_loops, sizeof(*iter_times), qsort_cmp);
		time_min = iter_times[0];
		time_median = iter_times[num_loops/2];
		time_max = iter_times[num_loops-1];
		printf("time iter: min=%g, median=%g, max=%g\n", time_min, time_median, time_max);
		free(iter_times);
		dynarray_destroy_d(&da_iter_times);

		#ifdef SDV_TRACING
			printf("SDV tracing disabled\n");
			trace_disable(); 
		#endif

		/*****************************************************************************************/
		J_estimated = 0;
		for (i=0;i<regs_n;i++){
			// printf("'%s' total joule = %g\n", regs[i].type, ((double) regs[i].uj_accum) / 1000000);
			J_estimated += ((double) regs[i].uj_accum) / 1e6;
		}
		rapl_close(regs, regs_n);
		free(regs);
		W_avg = J_estimated / time_total;
		// printf("J_estimated = %lf\tW_avg = %lf\n", J_estimated, W_avg);
		/*****************************************************************************************/

		// gflops = csr->nnz_matrix / time_total * num_loops * 2 * 1e-9;
		gflops = csr->nnz_matrix / time_median * 2 * 1e-9;
		printf("GFLOPS = %lf (%s)\n", gflops, getenv("PROGG"));
	}

	//=============================================================================
	//= Output section.
	//=============================================================================

	if (!use_artificial_matrices)
	{
		if (print_labels_and_exit)
		{
			i = 0;
			i += snprintf(buf + i, buf_n - i, "%s", "matrix_name");
			if (use_processes)
			{
				i += snprintf(buf + i, buf_n - i, ",%s", "num_procs");
			}
			else
			{
				i += snprintf(buf + i, buf_n - i, ",%s", "num_threads");
			}
			i += snprintf(buf + i, buf_n - i, ",%s", "csr_m");
			i += snprintf(buf + i, buf_n - i, ",%s", "csr_n");
			i += snprintf(buf + i, buf_n - i, ",%s", "csr_nnz");
			i += snprintf(buf + i, buf_n - i, ",%s", "symmetry");
			i += snprintf(buf + i, buf_n - i, ",%s", "time");
			i += snprintf(buf + i, buf_n - i, ",%s", "time_iter_min");
			i += snprintf(buf + i, buf_n - i, ",%s", "time_iter_median");
			i += snprintf(buf + i, buf_n - i, ",%s", "time_iter_max");
			i += snprintf(buf + i, buf_n - i, ",%s", "gflops");
			i += snprintf(buf + i, buf_n - i, ",%s", "csr_mem_footprint");
			i += snprintf(buf + i, buf_n - i, ",%s", "W_avg");
			i += snprintf(buf + i, buf_n - i, ",%s", "J_estimated");
			i += snprintf(buf + i, buf_n - i, ",%s", "format_name");
			i += snprintf(buf + i, buf_n - i, ",%s", "m");
			i += snprintf(buf + i, buf_n - i, ",%s", "n");
			i += snprintf(buf + i, buf_n - i, ",%s", "nnz");
			i += snprintf(buf + i, buf_n - i, ",%s", "mem_footprint");
			i += snprintf(buf + i, buf_n - i, ",%s", "mem_ratio");
			i += snprintf(buf + i, buf_n - i, ",%s", "num_loops");
			i += check_accuracy_labels(buf + i, buf_n - i);
			#ifdef PRINT_STATISTICS
				i += statistics_print_labels(buf + i, buf_n - i);
			#endif
			buf[i] = '\0';
			fprintf(stderr, "%s\n", buf);
			return;
		}
		i = 0;
		i += snprintf(buf + i, buf_n - i, "%s", csr->matrix_name);
		if (use_processes)
		{
			i += snprintf(buf + i, buf_n - i, ",%d", num_procs);
		}
		else
		{
			i += snprintf(buf + i, buf_n - i, ",%d", omp_get_max_threads());
		}
		i += snprintf(buf + i, buf_n - i, ",%lu", csr->m);
		i += snprintf(buf + i, buf_n - i, ",%lu", csr->n);
		i += snprintf(buf + i, buf_n - i, ",%lu", csr->nnz);
		i += snprintf(buf + i, buf_n - i, ",%lu", csr->symmetric);
		i += snprintf(buf + i, buf_n - i, ",%lf", time_total);
		i += snprintf(buf + i, buf_n - i, ",%lf", time_min);
		i += snprintf(buf + i, buf_n - i, ",%lf", time_median);
		i += snprintf(buf + i, buf_n - i, ",%lf", time_max);
		i += snprintf(buf + i, buf_n - i, ",%lf", gflops);
		i += snprintf(buf + i, buf_n - i, ",%lf", MF->csr_mem_footprint / (1024*1024));
		i += snprintf(buf + i, buf_n - i, ",%lf", W_avg);
		i += snprintf(buf + i, buf_n - i, ",%lf", J_estimated);
		i += snprintf(buf + i, buf_n - i, ",%s", MF->format_name);
		i += snprintf(buf + i, buf_n - i, ",%lu", MF->m);
		i += snprintf(buf + i, buf_n - i, ",%lu", MF->n);
		i += snprintf(buf + i, buf_n - i, ",%lu", MF->nnz);
		i += snprintf(buf + i, buf_n - i, ",%lf", MF->mem_footprint / (1024*1024));
		i += snprintf(buf + i, buf_n - i, ",%lf", MF->mem_footprint / MF->csr_mem_footprint);
		i += snprintf(buf + i, buf_n - i, ",%ld", num_loops);
		i += check_accuracy(buf + i, buf_n - i, csr->ia, csr->ja, csr->a_ref, csr->m, csr->n, csr->nnz, x_ref, y, csr->symmetric, csr->expanded_symmetry);
		#ifdef PRINT_STATISTICS
			i += MF->statistics_print_data(buf + i, buf_n - i);
		#endif
		buf[i] = '\0';
		fprintf(stderr, "%s\n", buf);
	}
	else
	{
		if (print_labels_and_exit)
		{
			i = 0;
			i += snprintf(buf + i, buf_n - i, "%s",  "matrix_name");
			i += snprintf(buf + i, buf_n - i, ",%s", "distribution");
			i += snprintf(buf + i, buf_n - i, ",%s", "placement");
			i += snprintf(buf + i, buf_n - i, ",%s", "seed");
			i += snprintf(buf + i, buf_n - i, ",%s", "nr_rows");
			i += snprintf(buf + i, buf_n - i, ",%s", "nr_cols");
			i += snprintf(buf + i, buf_n - i, ",%s", "nr_nzeros");
			i += snprintf(buf + i, buf_n - i, ",%s", "density");
			i += snprintf(buf + i, buf_n - i, ",%s", "mem_footprint");
			i += snprintf(buf + i, buf_n - i, ",%s", "mem_range");
			i += snprintf(buf + i, buf_n - i, ",%s", "avg_nnz_per_row");
			i += snprintf(buf + i, buf_n - i, ",%s", "std_nnz_per_row");
			i += snprintf(buf + i, buf_n - i, ",%s", "avg_bw");
			i += snprintf(buf + i, buf_n - i, ",%s", "std_bw");
			i += snprintf(buf + i, buf_n - i, ",%s", "avg_bw_scaled");
			i += snprintf(buf + i, buf_n - i, ",%s", "std_bw_scaled");
			i += snprintf(buf + i, buf_n - i, ",%s", "avg_sc");
			i += snprintf(buf + i, buf_n - i, ",%s", "std_sc");
			i += snprintf(buf + i, buf_n - i, ",%s", "avg_sc_scaled");
			i += snprintf(buf + i, buf_n - i, ",%s", "std_sc_scaled");
			i += snprintf(buf + i, buf_n - i, ",%s", "skew");
			i += snprintf(buf + i, buf_n - i, ",%s", "avg_num_neighbours");
			i += snprintf(buf + i, buf_n - i, ",%s", "cross_row_similarity");
			i += snprintf(buf + i, buf_n - i, ",%s", "format_name");
			i += snprintf(buf + i, buf_n - i, ",%s", "time");
			i += snprintf(buf + i, buf_n - i, ",%s", "gflops");
			i += snprintf(buf + i, buf_n - i, ",%s", "W_avg");
			i += snprintf(buf + i, buf_n - i, ",%s", "J_estimated");
			#ifdef PRINT_STATISTICS
				i += statistics_print_labels(buf + i, buf_n - i);
			#endif
			buf[i] = '\0';
			fprintf(stderr, "%s\n", buf);
			return;
		}
		i = 0;
		i += snprintf(buf + i, buf_n - i, "synthetic");
		i += snprintf(buf + i, buf_n - i, ",%s" , csr->AM_stats.distribution);
		i += snprintf(buf + i, buf_n - i, ",%s" , csr->AM_stats.placement);
		i += snprintf(buf + i, buf_n - i, ",%d" , csr->AM_stats.seed);
		i += snprintf(buf + i, buf_n - i, ",%u" , csr->AM_stats.nr_rows);
		i += snprintf(buf + i, buf_n - i, ",%u" , csr->AM_stats.nr_cols);
		i += snprintf(buf + i, buf_n - i, ",%u" , csr->AM_stats.nr_nzeros);
		i += snprintf(buf + i, buf_n - i, ",%lf", csr->AM_stats.density);
		i += snprintf(buf + i, buf_n - i, ",%lf", csr->AM_stats.mem_footprint);
		i += snprintf(buf + i, buf_n - i, ",%s" , csr->AM_stats.mem_range);
		i += snprintf(buf + i, buf_n - i, ",%lf", csr->AM_stats.avg_nnz_per_row);
		i += snprintf(buf + i, buf_n - i, ",%lf", csr->AM_stats.std_nnz_per_row);
		i += snprintf(buf + i, buf_n - i, ",%lf", csr->AM_stats.avg_bw);
		i += snprintf(buf + i, buf_n - i, ",%lf", csr->AM_stats.std_bw);
		i += snprintf(buf + i, buf_n - i, ",%lf", csr->AM_stats.avg_bw_scaled);
		i += snprintf(buf + i, buf_n - i, ",%lf", csr->AM_stats.std_bw_scaled);
		i += snprintf(buf + i, buf_n - i, ",%lf", csr->AM_stats.avg_sc);
		i += snprintf(buf + i, buf_n - i, ",%lf", csr->AM_stats.std_sc);
		i += snprintf(buf + i, buf_n - i, ",%lf", csr->AM_stats.avg_sc_scaled);
		i += snprintf(buf + i, buf_n - i, ",%lf", csr->AM_stats.std_sc_scaled);
		i += snprintf(buf + i, buf_n - i, ",%lf", csr->AM_stats.skew);
		i += snprintf(buf + i, buf_n - i, ",%lf", csr->AM_stats.avg_num_neighbours);
		i += snprintf(buf + i, buf_n - i, ",%lf", csr->AM_stats.cross_row_similarity);
		i += snprintf(buf + i, buf_n - i, ",%s" , MF->format_name);
		i += snprintf(buf + i, buf_n - i, ",%lf", time_total);
		i += snprintf(buf + i, buf_n - i, ",%lf", gflops);
		i += snprintf(buf + i, buf_n - i, ",%lf", W_avg);
		i += snprintf(buf + i, buf_n - i, ",%lf", J_estimated);
		#ifdef PRINT_STATISTICS
			i += MF->statistics_print_data(buf + i, buf_n - i);
		#endif
		buf[i] = '\0';
		fprintf(stderr, "%s\n", buf);
	}
}


void
bench(struct CSR_reference_s * csr, struct Matrix_Format * MF, long print_labels_and_exit)
{
	ValueTypeReference * x_ref;
	ValueType * x;
	ValueType * y;

	long K = atoi(getenv("K_DIM"));

	if (print_labels_and_exit == 1)
	{
		compute(NULL, NULL, 0, NULL, NULL, NULL, 0, 0, 1);
		return;
	}

	#ifdef SDV_TRACING
	{
		// int values[] = {0, 1};
		// const char* valueNames[] = {"Other", "Kernel"};

		// trace_name_event_and_values(1000, "code_region", 2, values, valueNames);

		/* 
			The above are not needed after 2025-09 update on SDV trace tool! 
			Now we just need to mark the region that will be profiled through 
			trace_begin_region("NAME") ... trace_end_region("NAME")
		*/

		trace_init();
		trace_disable();
	}
	#endif

	x_ref = (typeof(x_ref)) aligned_alloc(64, csr->n * K * sizeof(*x_ref));
	x = (typeof(x)) aligned_alloc(64, csr->n * K * sizeof(*x));
	#pragma omp parallel for
	for(long i=0;i<csr->n * K;++i)
	{
		x_ref[i] = 1.0;
		x[i] = x_ref[i];
	}
	y = (typeof(y)) aligned_alloc(64, csr->m * K * sizeof(*y));
	#pragma omp parallel for
	for(long i=0;i<csr->m;i++)
		y[i] = 1.0;    // Test whether the format zeros rows with no nnz.

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

	long min_num_loops;
	#ifdef SDV_TRACING
		min_num_loops = 1;
	#else
		// min_num_loops = 256;
		min_num_loops = 64;
	#endif

	double min_runtime;
	#ifdef SDV_TRACING
		min_runtime = 0;
	#else
		// min_runtime = 0;
		min_runtime = 2.0;
	#endif

	compute(csr, MF, K, x, x_ref, y, min_num_loops, min_runtime, 0);

	free(x);
	free(y);
}

