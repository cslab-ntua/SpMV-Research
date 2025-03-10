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
	#include "io.h"
	#include "parallel_io.h"
	#include "storage_formats/matrix_market/matrix_market.h"
	#include "storage_formats/openfoam/openfoam_matrix.h"

	#include "aux/csr_converter.h"
	#include "aux/csr_converter_double.h"

	#include "aux/csr_util.h"

	#include "monitoring/power/rapl.h"

	#include "artificial_matrix_generation.h"

	#include "aux/rcm.h"

	#include "data_structures/dynamic_array/dynamic_array_gen_undef.h"
	#define DYNAMIC_ARRAY_GEN_TYPE_1  double
	#define DYNAMIC_ARRAY_GEN_SUFFIX  _spmv_bench_d
	#include "data_structures/dynamic_array/dynamic_array_gen.c"

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

long num_loops_out;


// Utils macro
#define Min(x,y) ((x)<(y)?(x):(y))
#define Max(x,y) ((x)>(y)?(x):(y))
#define Abs(x) ((x)>(0)?(x):-(x))


// #define ReferenceType  double
// #define ReferenceType  long double
#define ReferenceType  _Float128

/* ldoor, mkl_ie, 8 threads:
 *     ValueType | ReferenceType       | Errors
 *     double    | double              | errors spmv: mae=2.0679e-10, max_ae=7.45058e-08, mse=1.11396e-18, mape=3.7028e-17, smape=1.8514e-17
 *     double    | double + kahan      | errors spmv: mae=1.20597e-10, max_ae=4.47035e-08, mse=3.30276e-19, mape=2.11222e-17, smape=1.05611e-17
 *     double    | long double         | errors spmv: mae=1.11432e-10, max_ae=4.47035e-08, mse=3.05508e-19, mape=1.14059e-17, smape=5.70295e-18
 *     double    | long double + kahan | errors spmv: mae=1.11426e-10, max_ae=4.47035e-08, mse=3.05491e-19, mape=1.14059e-17, smape=5.70295e-18
 *     double    | _Float128          | errors spmv: mae=1.11425e-10, max_ae=4.47035e-08, mse=3.05482e-19, mape=1.14059e-17, smape=5.70295e-18
 *     double    | _Float128 + kahan  | errors spmv: mae=1.11425e-10, max_ae=4.47035e-08, mse=3.05482e-19, mape=1.14059e-17, smape=5.70295e-18
 *
 *     double    | double              | errors spmv: mae=2.01305e-10, max_ae=7.45058e-08, mse=1.04495e-18, mape=6.95171e-17, smape=3.47585e-17
 *     double    | double + kahan:     | errors spmv: mae=1.47387e-10, max_ae=5.96046e-08, mse=5.21976e-19, mape=5.22525e-17, smape=2.61262e-17
 *     double    | _Float128          | errors spmv: mae=1.39996e-10, max_ae=5.96046e-08, mse=4.99829e-19, mape=4.049e-17, smape=2.0245e-17
 *     double    | _Float128 + kahan  | errors spmv: mae=1.39996e-10, max_ae=5.96046e-08, mse=4.99829e-19, mape=4.049e-17, smape=2.0245e-17
 *
 *     float     | double              | errors spmv: mae=0.0628685, max_ae=21.1667, mse=0.0826114, mape=1.63995e-08, smape=8.20012e-09
 *     float     | long double         | errors spmv: mae=0.0628685, max_ae=21.1667, mse=0.0826114, mape=1.63995e-08, smape=8.20012e-09
 *     float     | _Float128          | errors spmv: mae=0.0628685, max_ae=21.1667, mse=0.0826114, mape=1.63995e-08, smape=8.20012e-09
 */

static inline
double
reference_to_double(void * A, long i)
{
	return (double) ((ReferenceType *) A)[i];
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
check_accuracy(char * buf, long buf_n, INT_T * csr_ia, INT_T * csr_ja, double * csr_a_ref, INT_T csr_m, __attribute__((unused)) INT_T csr_n, __attribute__((unused)) INT_T csr_nnz, double * x_ref, ValueType * y, int csr_symmetric)
{
	__attribute__((unused)) ReferenceType epsilon_relaxed = 1e-4;
	#if DOUBLE == 0
		ReferenceType epsilon = 1e-7;
	#elif DOUBLE == 1
		ReferenceType epsilon = 1e-10;
	#endif
	ReferenceType * y_gold = (typeof(y_gold)) malloc(csr_m * sizeof(*y_gold));
	ReferenceType * y_test = (typeof(y_test)) malloc(csr_m * sizeof(*y_test));
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

	if (csr_symmetric)
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
			ReferenceType sum;
			long i, j;
			#pragma omp for
			for (i=0;i<csr_m;i++)
			{
				ReferenceType val, tmp, compensation;
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

	ReferenceType maxDiff = 0, diff;
	// int cnt=0;
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
		// 	printf("error: i=%ld/%d , a=%.10g f=%.10g\n", i, csr_m-1, (double) y_gold[i], (double) y_test[i]);
		// if(i<5)
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


int
qsort_cmp(const void * a_ptr, const void * b_ptr)
{
	double a = *((double *) a_ptr);
	double b = *((double *) b_ptr);
	return (a > b) ? 1 : (a < b) ? -1 : 0;
}

void
compute(char * matrix_name,
		__attribute__((unused)) INT_T * csr_ia, __attribute__((unused)) INT_T * csr_ja, __attribute__((unused)) ValueType * csr_a, double * csr_a_ref, INT_T csr_m, INT_T csr_n, INT_T csr_nnz, __attribute__((unused)) INT_T csr_nnz_diag, __attribute__((unused)) INT_T csr_nnz_non_diag, __attribute__((unused)) int csr_symmetric,
		struct Matrix_Format * MF,
		csr_matrix * AM,
		ValueType * x, double * x_ref, ValueType * y,
		long min_num_loops,
		long print_labels, __attribute__((unused)) long clear_caches)
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

	long L3_cache_block_n = 4 * LEVEL3_CACHE_SIZE;
	unsigned char * L3_cache_block = (typeof(L3_cache_block)) malloc(L3_cache_block_n * sizeof(*L3_cache_block));

	if (!print_labels)
	{
		// Warm up cpu.
		__attribute__((unused)) volatile double warmup_total;
		long A_warmup_n = (1<<20) * num_threads;
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

		// /* Calculate number of loops so that the total running time is at least 1 second for stability reasons
		// (some cpus show frequency inconsistencies when running times are too small). */
		// long num_calc_loops_runs_1 = 5;
		// long num_calc_loops_runs_2;
		// time_after_warm_up = 0;
		// time_after_warm_up += time_it(1,
			// for (i=0;i<num_calc_loops_runs_1;i++)
				// MF->spmv(x, y);
		// );
		// num_calc_loops_runs_2 = 0.1 / (time_after_warm_up / num_calc_loops_runs_1);
		// if (num_calc_loops_runs_2 < 5)
			// num_calc_loops_runs_2 = 5;
		// time_after_warm_up += time_it(1,
			// for (i=0;i<num_calc_loops_runs_2;i++)
				// MF->spmv(x, y);
		// );
		// printf("time after warm up %lf\n", time_after_warm_up);
		// num_loops = 1.0 / (time_after_warm_up / (num_calc_loops_runs_1 + num_calc_loops_runs_2));
		// if (num_loops < min_num_loops)
			// num_loops = min_num_loops;
		// num_loops_out = num_loops;
		// printf("number of loops = %ld\n", num_loops);

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

		time_total = 0;
		num_loops = 0;
		dynarray * da_iter_times = dynarray_new(10 * min_num_loops);
		while (time_total < 1.0 || num_loops < min_num_loops)
		{
			if (__builtin_expect(clear_caches, 0))
			{
				_Pragma("omp parallel")
				{
					long i;
					_Pragma("omp for")
					for (i=0;i<L3_cache_block_n;i++)
					{
						L3_cache_block[i] = i;
					}
				}
			}

			rapl_read_start(regs, regs_n);

			time_iter = time_it(1,
				MF->spmv(x, y);
			);

			rapl_read_end(regs, regs_n);

			dynarray_push_back(da_iter_times, time_iter);
			time_total += time_iter;
			num_loops++;
		}
		num_loops_out = num_loops;
		printf("number of loops = %ld\n", num_loops);
		long iter_times_n;
		double * iter_times;
		iter_times_n = dynarray_export_array(da_iter_times, &iter_times);
		if (iter_times_n != num_loops)
			error("dynamic array size not equal to number of loops: %ld != %ld", iter_times_n, num_loops);
		qsort(iter_times, num_loops, sizeof(*iter_times), qsort_cmp);
		time_min = iter_times[0];
		time_median = iter_times[num_loops/2];
		time_max = iter_times[num_loops-1];
		printf("time iter: min=%g, median=%g, max=%g\n", time_min, time_median, time_max);
		free(iter_times);
		dynarray_destroy(&da_iter_times);

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

		//=============================================================================
		//= Output section.
		//=============================================================================

		#ifdef KEEP_SYMMETRY
			csr_nnz = 2*csr_nnz_non_diag + csr_nnz_diag;
		#endif
		// Use 'csr_nnz' to be sure we have the initial nnz (there is no coo for artificial AM).
		// gflops = csr_nnz / time_total * num_loops * 2 * 1e-9;
		gflops = csr_nnz / time_median * 2 * 1e-9;
		printf("GFLOPS = %lf (%s)\n", gflops, getenv("PROGG"));
	}

	if (!use_artificial_matrices)
	{
		if (print_labels)
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
		i += snprintf(buf + i, buf_n - i, "%s", matrix_name);
		if (use_processes)
		{
			i += snprintf(buf + i, buf_n - i, ",%d", num_procs);
		}
		else
		{
			i += snprintf(buf + i, buf_n - i, ",%d", omp_get_max_threads());
		}
		i += snprintf(buf + i, buf_n - i, ",%u", csr_m);
		i += snprintf(buf + i, buf_n - i, ",%u", csr_n);
		i += snprintf(buf + i, buf_n - i, ",%u", csr_nnz);
		i += snprintf(buf + i, buf_n - i, ",%u", csr_symmetric);
		i += snprintf(buf + i, buf_n - i, ",%lf", time_total);
		i += snprintf(buf + i, buf_n - i, ",%lf", time_min);
		i += snprintf(buf + i, buf_n - i, ",%lf", time_median);
		i += snprintf(buf + i, buf_n - i, ",%lf", time_max);
		i += snprintf(buf + i, buf_n - i, ",%lf", gflops);
		i += snprintf(buf + i, buf_n - i, ",%lf", MF->csr_mem_footprint / (1024*1024));
		i += snprintf(buf + i, buf_n - i, ",%lf", W_avg);
		i += snprintf(buf + i, buf_n - i, ",%lf", J_estimated);
		i += snprintf(buf + i, buf_n - i, ",%s", MF->format_name);
		i += snprintf(buf + i, buf_n - i, ",%u", MF->m);
		i += snprintf(buf + i, buf_n - i, ",%u", MF->n);
		i += snprintf(buf + i, buf_n - i, ",%u", MF->nnz);
		i += snprintf(buf + i, buf_n - i, ",%lf", MF->mem_footprint / (1024*1024));
		i += snprintf(buf + i, buf_n - i, ",%lf", MF->mem_footprint / MF->csr_mem_footprint);
		i += snprintf(buf + i, buf_n - i, ",%ld", num_loops);
		i += check_accuracy(buf + i, buf_n - i, csr_ia, csr_ja, csr_a_ref, csr_m, csr_n, csr_nnz, x_ref, y, csr_symmetric);
		#ifdef PRINT_STATISTICS
			i += MF->statistics_print_data(buf + i, buf_n - i);
		#endif
		buf[i] = '\0';
		fprintf(stderr, "%s\n", buf);
	}
	else
	{
		if (print_labels)
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


//==========================================================================================================================================
//= Main
//==========================================================================================================================================

int
main(int argc, char **argv)
{
	__attribute__((unused)) int num_threads;

	struct Matrix_Market * MTX = NULL;
	double * coo_val = NULL;   // MATRIX_MARKET_FLOAT_T is always double, as reference for error calculation.
	INT_T * coo_rowind = NULL;
	INT_T * coo_colind = NULL;
	INT_T coo_m = 0;
	INT_T coo_n = 0;
	INT_T coo_nnz = 0;
	INT_T coo_nnz_diag = 0;
	INT_T coo_nnz_non_diag = 0;
	INT_T coo_symmetric = 0;
	double * x_ref;

	double * csr_a_ref = NULL;

	ValueType * csr_a = NULL; // values (of size NNZ)
	INT_T * csr_ia = NULL;    // rowptr (of size m+1)
	INT_T * csr_ja = NULL;    // colidx of each NNZ (of size nnz)
	INT_T csr_m = 0;
	INT_T csr_n = 0;
	INT_T csr_nnz = 0;
	INT_T csr_nnz_diag = 0;
	INT_T csr_nnz_non_diag = 0;
	int csr_symmetric = 0;
	long expand_symmetry = 1;

	struct Matrix_Format * MF;   // Real matrices.
	csr_matrix * AM = NULL;
	ValueType * x;
	ValueType * y;
	char matrix_name[1000];
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
		compute(NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL, 0, 1, 0);
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
		char * file_in;
		i = 1;
		file_in = argv[i++];
		snprintf(matrix_name, sizeof(matrix_name), "%s", file_in);

		time = time_it(1,
			if (stat_isdir(file_in))   // OpenFoam format
			{
				int nnz_non_diag, N;
				int * rowind, * colind;
				read_openfoam_matrix_dir(file_in, &rowind, &colind, &N, &nnz_non_diag);
				coo_m = N;
				coo_n = N;
				coo_nnz = N + nnz_non_diag;
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
				#ifdef KEEP_SYMMETRY
					expand_symmetry = 0;
				#else
					expand_symmetry = 1;
				#endif
				long pattern_dummy_vals = 1;
				MTX = mtx_read(file_in, expand_symmetry, pattern_dummy_vals);
				coo_rowind = MTX->R;
				coo_colind = MTX->C;
				coo_m = MTX->m;
				coo_n = MTX->n;
				coo_nnz_diag = MTX->nnz_diag;
				coo_nnz_non_diag = MTX->nnz_non_diag;
				coo_symmetric = MTX->symmetric;
				#ifdef KEEP_SYMMETRY
					coo_nnz = MTX->nnz_sym;
				#else
					coo_nnz = MTX->nnz;
				#endif
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
			csr_a_ref = (typeof(csr_a_ref)) aligned_alloc(64, (coo_nnz + VECTOR_ELEM_NUM) * sizeof(*csr_a_ref));
			csr_a = (typeof(csr_a)) aligned_alloc(64, (coo_nnz + VECTOR_ELEM_NUM) * sizeof(*csr_a));
			csr_ja = (typeof(csr_ja)) aligned_alloc(64, (coo_nnz + VECTOR_ELEM_NUM) * sizeof(*csr_ja));
			csr_ia = (typeof(csr_ia)) aligned_alloc(64, (coo_m+1 + VECTOR_ELEM_NUM) * sizeof(*csr_ia));
			csr_m = coo_m;
			csr_n = coo_n;
			csr_nnz = coo_nnz;
			csr_nnz_diag = coo_nnz_diag;
			csr_nnz_non_diag = coo_nnz_non_diag;
			csr_symmetric = coo_symmetric;
			_Pragma("omp parallel for")
			for (long i=0;i<coo_nnz + VECTOR_ELEM_NUM;i++)
			{
				csr_a_ref[i] = 0.0;
				csr_ja[i] = 0;
			}
			_Pragma("omp parallel for")
			for (long i=0;i<coo_m+1 + VECTOR_ELEM_NUM;i++)
				csr_ia[i] = 0;
			coo_to_csr(coo_rowind, coo_colind, coo_val, coo_m, coo_n, coo_nnz, csr_ia, csr_ja, csr_a_ref, 1, 0);
			free(coo_rowind);
			free(coo_colind);
			free(coo_val);
			_Pragma("omp parallel for")
			for (long i=0;i<coo_nnz + VECTOR_ELEM_NUM;i++)
				csr_a[i] = (ValueType) csr_a_ref[i];

			// _Pragma("omp parallel for")
			// for (long i=0;i<csr_m;i++)
			// {
				// if (csr_ia[i+1] - csr_ia[i] == 0)
					// error("test");
			// }
			// exit(0);

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

		csr_ia = (typeof(csr_ia)) aligned_alloc(64, (csr_m+1 + VECTOR_ELEM_NUM) * sizeof(*csr_ia));
		#pragma omp parallel for
		for (long i=0;i<csr_m+1;i++)
			csr_ia[i] = AM->row_ptr[i];
		free(AM->row_ptr);
		AM->row_ptr = NULL;

		csr_a = (typeof(csr_a)) aligned_alloc(64, (csr_nnz + VECTOR_ELEM_NUM) * sizeof(*csr_a));
		csr_ja = (typeof(csr_ja)) aligned_alloc(64, (csr_nnz + VECTOR_ELEM_NUM) * sizeof(*csr_ja));
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

	if (atoi(getenv("USE_RCM_REORDERING")) == 1)
	{
		time = time_it(1,
			// if (!csr_symmetric)
				// error("RCM is only applicable to symmetric matrices");

			int * permutation;
			int * row_ptr_new = NULL;
			int * col_idx_new = NULL;
			ValueType * values_new = NULL;
			int nnz_new, nnz_diag;

			if (!expand_symmetry)
			{
				printf("expanding symmetry for rcm\n");
				csr_expand_symmetric_csr_converter(csr_ia, csr_ja, csr_a, csr_m, csr_n, csr_nnz, &row_ptr_new, &col_idx_new, &values_new, &nnz_new, &nnz_diag, 1);
				printf("nnz_old=%d nnz_new=%d csr_nnz_diag=%d csr_nnz_non_diag=%d nnz_diag=%d \n", csr_nnz, nnz_new, csr_nnz_diag, csr_nnz_non_diag, nnz_diag);
				csr_nnz = nnz_new;
				free(csr_ia);
				free(csr_ja);
				free(csr_a);
				csr_ia = row_ptr_new;
				csr_ja = col_idx_new;
				csr_a = values_new;
			}

			reverse_cuthill_mckee(csr_ia, csr_ja, csr_a, csr_m, csr_n, csr_nnz, &row_ptr_new, &col_idx_new, &values_new, &permutation);
			printf("nnz_old=%d csr_nnz_diag=%d csr_nnz_non_diag=%d \n", csr_nnz, csr_nnz_diag, csr_nnz_non_diag);
			free(csr_ia);
			free(csr_ja);
			free(csr_a);
			csr_ia = row_ptr_new;
			csr_ja = col_idx_new;
			csr_a = values_new;
			if (csr_n != csr_m)
				error("csr_n != csr_m");
			for (i=0;i<csr_m;i++)
			{
				if (csr_ia[i] > csr_ia[i+1])
					error("csr_ia[%d]=%d > csr_ia[%d]=%d", i, csr_ia[i], i+1, csr_ia[i+1]);
			}
			for (i=0;i<=csr_m;i++)
			{
				if (csr_ia[i] < 0 || csr_ia[i] > csr_nnz)
					error("csr_ia[%d]=%d >= csr_nnz", i, csr_ia[i]);
			}
			for (i=0;i<csr_nnz;i++)
			{
				if (csr_ja[i] < 0 || csr_ja[i] >= csr_n)
					error("csr_ja[%d]=%d >= csr_n", i, csr_ja[i]);
			}

			if (!expand_symmetry)
			{
				printf("dropping upper matrix after rcm\n");
				csr_drop_upper_csr_converter(csr_ia, csr_ja, csr_a, csr_m, csr_n, csr_nnz, &row_ptr_new, &col_idx_new, &values_new, &nnz_new, NULL, 1);
				csr_nnz = nnz_new;
				free(csr_ia);
				free(csr_ja);
				free(csr_a);
				csr_ia = row_ptr_new;
				csr_ja = col_idx_new;
				csr_a = values_new;
			}
		);
		printf("time rcm reordering: %lf\n", time);
	}

	time = time_it(1,
		MF = csr_to_format(csr_ia, csr_ja, csr_a, csr_m, csr_n, csr_nnz, csr_symmetric);
	);
	printf("time convert to format: %lf\n", time);

	// for(int i=0;i<coo_nnz;i++)
	// 	csr_ja[i]=0;

	x_ref = (typeof(x_ref)) aligned_alloc(64, csr_n * sizeof(*x_ref));
	x = (typeof(x)) aligned_alloc(64, csr_n * sizeof(*x));
	#pragma omp parallel for
	for(int i=0;i<csr_n;++i)
	{
		x_ref[i] = 1.0;
		x[i] = x_ref[i];
	}
	y = (typeof(y)) aligned_alloc(64, csr_m * sizeof(sizeof(*y)));
	#pragma omp parallel for
	for(long i=0;i<csr_m;i++)
		y[i] = 1.0;

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
				csr_ja[i] = 0;                      // idx0 - Remove X access pattern dependency.
				// csr_ja[i] = i % csr_n;              // idx_serial - Remove X access pattern dependency.
				// csr_ja[i] = i_s + (i % i_per_t);    // idx_t_local - Remove X access pattern dependency.
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
		long num_pixels = 1024;
		long num_pixels_x = (csr_n < num_pixels) ? csr_n : num_pixels;
		long num_pixels_y = (csr_m < num_pixels) ? csr_m : num_pixels;

		printf("ploting : %s\n", filename_base);
		csr_plot(filename_base, csr_ia, csr_ja, csr_a, csr_m, csr_n, csr_nnz, 0, num_pixels_x, num_pixels_y);
		return 0;
	}

	long min_num_loops;
	min_num_loops = 128;

	int clear_caches = atoi(getenv("CLEAR_CACHES"));
	prefetch_distance = 1;
	time = time_it(1,
		// for (i=0;i<5;i++)
		{
			// printf("prefetch_distance = %d\n", prefetch_distance);
			compute(matrix_name,
				csr_ia, csr_ja, csr_a, csr_a_ref, csr_m, csr_n, csr_nnz, csr_nnz_diag, csr_nnz_non_diag, csr_symmetric,
				MF, AM, x, x_ref, y, min_num_loops, 0, clear_caches);
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

