#include <iostream>
#include <algorithm>
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

#include "spmv_kernels/spmv_kernel.h"

#ifdef CUDA_KERNEL
	#include <nvtx3/nvToolsExt.h>
	#include <cuda_runtime.h>
	#include <cuda_profiler_api.h>
#endif

#ifdef HYBRID
	#include "spmv_kernels/hybrid_dispatcher.h"
#endif

#include <numaif.h>
#include <malloc.h> // Required for malloc_usable_size

// Helper function to read current process RSS in KB
long getCurrentRSS_KB() {
    long resident_pages = 0;
    FILE* fp = fopen("/proc/self/statm", "r");
    if (fp != NULL) {
        long dummy_size;
        if (fscanf(fp, "%ld %ld", &dummy_size, &resident_pages) != 2) {
            resident_pages = 0;
        }
        fclose(fp);
    }
    return resident_pages * sysconf(_SC_PAGESIZE) / 1024;
}

void checkResidency(void* ptr, const char* var_name) {
    long current_rss = getCurrentRSS_KB();
    
    // Query glibc for the size of the dynamically allocated block
    size_t alloc_size = malloc_usable_size(ptr);
    double size_mb = (double)alloc_size / (1024.0 * 1024.0);
    
    if (alloc_size == 0) return;

    char* c_ptr = (char*)ptr;
    long page_size = sysconf(_SC_PAGESIZE);
    
    int cpu_pages = 0;
    int gpu_pages = 0;
    int other_pages = 0;
    
    size_t num_pages = (alloc_size + page_size - 1) / page_size;
    
    int first_node = -1;
    int mid_node = -1;
    int last_node = -1;
    
    // size_t transition_offset = 0;
    // bool transition_found = false;
    // int previous_node = -1;
    
    char* vis_array = (char*)malloc(num_pages + 1);
    if (!vis_array) return;
    vis_array[num_pages] = '\0';
    
    for (size_t i = 0; i < num_pages; i++) {
        int node = -1;
        void* page_addr = c_ptr + (i * page_size);
        
        int status = get_mempolicy(&node, NULL, 0, page_addr, MPOL_F_NODE | MPOL_F_ADDR);
        if (status == 0) {
            if (node == 0) { cpu_pages++; vis_array[i] = '*'; }
            else if (node == 1) { gpu_pages++; vis_array[i] = '_'; }
            else { other_pages++; vis_array[i] = '?'; }
            
            if (i == 0) first_node = node;
            if (i == num_pages / 2) mid_node = node;
            if (i == num_pages - 1) last_node = node;
            
            // // Record the first time the node changes
            // if (!transition_found && previous_node != -1 && node != previous_node) {
            //     transition_offset = i * page_size;
            //     transition_found = true;
            // }
            // previous_node = node;
        } else {
            vis_array[i] = '?';
        }
    }
    
    int total_counted = cpu_pages + gpu_pages + other_pages;
    if (total_counted > 0) {
        double cpu_pct = (double)cpu_pages / total_counted * 100.0;
        double gpu_pct = (double)gpu_pages / total_counted * 100.0;
        
        printf("Variable %s (size: %.2f MB) NUMA Nodes -> Start: %d | Mid: %d | End: %d  | CPU RSS: %ld KB\n", 
               var_name, size_mb, first_node, mid_node, last_node, current_rss);
               
        printf("    -> [Page Distribution %s] CPU (Node 0): %.2f%% | GPU (Node 1): %.2f%%\n", var_name, cpu_pct, gpu_pct);
               
        // if (transition_found) {
        //     double boundary_mb = (double)transition_offset / (1024.0 * 1024.0);
        //     printf("    -> [Residency Transition] Node changes from %d to %d at offset: %zu bytes (%.2f MB)\n", 
        //            first_node, last_node, transition_offset, boundary_mb);
        // }
        
        if (num_pages <= 256) {
            printf("    -> [Visualization %s]: %s\n", var_name, vis_array);
        } else {
            int vis_len = 128;
            char vis_buf[129];
            vis_buf[vis_len] = '\0';
            for (int i = 0; i < vis_len; i++) {
                size_t start_idx = i * num_pages / vis_len;
                size_t end_idx = (i + 1) * num_pages / vis_len;
                int cpu_cnt = 0, gpu_cnt = 0;
                for (size_t j = start_idx; j < end_idx; j++) {
                    if (vis_array[j] == '*') cpu_cnt++;
                    else if (vis_array[j] == '_') gpu_cnt++;
                }
                if (cpu_cnt >= gpu_cnt && cpu_cnt > 0) vis_buf[i] = '*';
                else if (gpu_cnt > cpu_cnt) vis_buf[i] = '_';
                else vis_buf[i] = '?';
            }
            printf("    -> [Visualization %s (Downsampled %zu pages to 128 chars)]: %s\n", var_name, num_pages, vis_buf);
        }
    } else {
        printf("Variable %s (size: %.2f MB) has no physical page allocated | CPU RSS: %ld KB\n", 
               var_name, size_mb, current_rss);
    }
    
    free(vis_array);
}


extern int num_procs;

long num_loops_out;

// Utils macro
#define Min(x,y) ((x)<(y)?(x):(y))
#define Max(x,y) ((x)>(y)?(x):(y))
#define Abs(x) ((x)>(0)?(x):-(x))

#ifndef CHECK_ACCURACY
	#define CHECK_ACCURACY 0
	// #define CHECK_ACCURACY 1
#endif


// #define ValueTypeValidation  double
// #define ValueTypeValidation  long double
// #define ValueTypeValidation  __float128
#define ValueTypeValidation  _Float128

/* ldoor, mkl_ie, 8 threads:
 *     ValueType | ValueTypeValidation       | Errors
 *     double    | double              | errors spmv: mae=2.0679e-10, max_ae=7.45058e-08, mse=1.11396e-18, mape=3.7028e-17, smape=1.8514e-17
 *     double    | double + kahan      | errors spmv: mae=1.20597e-10, max_ae=4.47035e-08, mse=3.30276e-19, mape=2.11222e-17, smape=1.05611e-17
 *     double    | long double         | errors spmv: mae=1.11432e-10, max_ae=4.47035e-08, mse=3.05508e-19, mape=1.14059e-17, smape=5.70295e-18
 *     double    | long double + kahan | errors spmv: mae=1.11426e-10, max_ae=4.47035e-08, mse=3.05491e-19, mape=1.14059e-17, smape=5.70295e-18
 *     double    | __float128          | errors spmv: mae=1.11425e-10, max_ae=4.47035e-08, mse=3.05482e-19, mape=1.14059e-17, smape=5.70295e-18
 *     double    | __float128 + kahan  | errors spmv: mae=1.11425e-10, max_ae=4.47035e-08, mse=3.05482e-19, mape=1.14059e-17, smape=5.70295e-18
 *
 *     double    | double              | errors spmv: mae=2.01305e-10, max_ae=7.45058e-08, mse=1.04495e-18, mape=6.95171e-17, smape=3.47585e-17
 *     double    | double + kahan:     | errors spmv: mae=1.47387e-10, max_ae=5.96046e-08, mse=5.21976e-19, mape=5.22525e-17, smape=2.61262e-17
 *     double    | __float128          | errors spmv: mae=1.39996e-10, max_ae=5.96046e-08, mse=4.99829e-19, mape=4.049e-17, smape=2.0245e-17
 *     double    | __float128 + kahan  | errors spmv: mae=1.39996e-10, max_ae=5.96046e-08, mse=4.99829e-19, mape=4.049e-17, smape=2.0245e-17
 *
 *     float     | double              | errors spmv: mae=0.0628685, max_ae=21.1667, mse=0.0826114, mape=1.63995e-08, smape=8.20012e-09
 *     float     | long double         | errors spmv: mae=0.0628685, max_ae=21.1667, mse=0.0826114, mape=1.63995e-08, smape=8.20012e-09
 *     float     | __float128          | errors spmv: mae=0.0628685, max_ae=21.1667, mse=0.0826114, mape=1.63995e-08, smape=8.20012e-09
 */

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
check_accuracy(char * buf, long buf_n,
		struct CSR_reference_s * csr,
		ValueTypeReference * x_ref, ValueType * y,
		long symmetric, long expanded_symmetry, long num_loops = 1)
{
	__attribute__((unused)) ValueTypeValidation epsilon_relaxed = 1e-4;
	#if DOUBLE == 0
		ValueTypeValidation epsilon = 1e-7;
	#elif DOUBLE == 1
		ValueTypeValidation epsilon = 1e-10;
	#endif
	ValueTypeValidation * y_gold = (typeof(y_gold)) malloc(csr->m * sizeof(*y_gold));
	ValueTypeValidation * y_test = (typeof(y_test)) malloc(csr->m * sizeof(*y_test));
	long i;

	#pragma omp parallel
	{
		long i;
		#pragma omp for
		for(i=0;i<csr->m;i++)
		{
			y_gold[i] = 0;
			y_test[i] = y[i];
		}
	}

	for (long iter = 0; iter < num_loops; iter++)
	{
		if (symmetric && !expanded_symmetry)
		{
			long i, j, col;
			for (i=0;i<csr->m;i++)
			{
				for (j=csr->ia[i];j<csr->ia[i+1];j++)
				{
					col = csr->ja[j];
					y_gold[i] += csr->a_ref[j] * x_ref[col];
					if (i != col)
						y_gold[col] += csr->a_ref[j] * x_ref[i];
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
				for (i=0;i<csr->m;i++)
				{
					ValueTypeValidation val, tmp, compensation;
					compensation = 0;
					sum = 0;
					for (j=csr->ia[i];j<csr->ia[i+1];j++)
					{
						val = csr->a_ref[j] * x_ref[csr->ja[j]] - compensation;
						tmp = sum + val;
						compensation = (tmp - sum) - val;
						sum = tmp;
					}
					y_gold[i] = sum;
				}
			}
		}

		if (iter < num_loops - 1)
		{
			long copy_elements = (csr->m < csr->n) ? csr->m : csr->n;
			#pragma omp parallel for
			for (long c = 0; c < copy_elements; c++) {
				x_ref[c] = static_cast<double>(y_gold[c]);
			}
			#pragma omp parallel for
			for(long i=0;i<csr->m;i++) {
				y_gold[i] = 0;
			}
		}
	}

	ValueTypeValidation maxDiff = 0, diff;
	// long cnt=0;
	for(i=0;i<csr->m;i++)
	{
		diff = Abs(y_gold[i] - y_test[i]);
		// maxDiff = Max(maxDiff, diff);
		if (y_gold[i] > epsilon)
		{
			diff = diff / abs(y_gold[i]);
			maxDiff = Max(maxDiff, diff);
		}
		// if (diff > epsilon_relaxed)
			// printf("error: i=%ld/%ld , a=%.10g f=%.10g\n", i, csr->m-1, (double) y_gold[i], (double) y_test[i]);
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
		array_mae_concurrent(y_gold, y_test, csr->m, &mae, reference_to_double);
		array_max_ae_concurrent(y_gold, y_test, csr->m, &max_ae, reference_to_double);
		array_mse_concurrent(y_gold, y_test, csr->m, &mse, reference_to_double);
		array_mape_concurrent(y_gold, y_test, csr->m, &mape, reference_to_double);
		array_smape_concurrent(y_gold, y_test, csr->m, &smape, reference_to_double);
		array_lnQ_error_concurrent(y_gold, y_test, csr->m, &lnQ_error, reference_to_double);
		array_mlare_concurrent(y_gold, y_test, csr->m, &mlare, reference_to_double);
		array_gmare_concurrent(y_gold, y_test, csr->m, &gmare, reference_to_double);
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

	// for (i=0;i<csr->m;i++)
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

// To make it work with pointer swapping, I had to change how x and y are passed by reference (from pass-by-value to pass-by-reference-to-pointer)
// before change: void compute(struct CSR_reference_s * csr, struct Matrix_Format * MF, ValueType *x, ValueType *y, ...)
void
compute(struct CSR_reference_s * csr, struct Matrix_Format * MF,
		ValueType *&x, ValueTypeReference * x_ref, ValueType *&y,
		long min_num_loops, double min_runtime, long print_labels_and_exit)
{
	int num_threads = omp_get_max_threads();
	int use_processes = atoi(getenv("USE_PROCESSES"));
	long num_loops;
	double gflops;
	__attribute__((unused)) double time_total, time_iter, time_min, time_max, time_median, time_warm_up, time_after_warm_up;
	#ifdef HYBRID
		__attribute__((unused)) double time_total_cpu, time_total_gpu, time_min_cpu, time_min_gpu, time_max_cpu, time_max_gpu, time_median_cpu, time_median_gpu;
	#endif
	long buf_n = 10000;
	char buf[buf_n + 1];
	long i, j;
	double J_estimated, W_avg;
	int use_artificial_matrices = atoi(getenv("USE_ARTIFICIAL_MATRICES"));
	#ifdef HYBRID
		Hybrid_Arrays * HA = dynamic_cast<Hybrid_Arrays*>(MF);
	#endif
	if (!print_labels_and_exit)
	{
		long copy_elements;
		#ifdef HYBRID
			copy_elements = (HA->m < HA->n) ? HA->m : HA->n; 
		#else
			copy_elements = (MF->m < MF->n) ? MF->m : MF->n;
		#endif

		// Warm up.
		time_warm_up = time_it(1,
			#ifdef CUDA_KERNEL
			{		
				#ifdef HYBRID
					for(int i=0;i<10;i++)
						HA->cpu_spmv(x, y);
					for(int i=0;i<1000;i++)
						HA->gpu_spmv_sync(x, y);

					// for(int i=0;i<1000;i++) {
					// 	HA->spmv(x, y);
					// 	// if (csr->m == csr->n) std::swap(x, y);
					// 	// else std::copy(y, y + copy_elements, x);
					// }
				#else
					for(int i=0;i<1000;i++) {
						MF->spmv(x, y);
						MF->synchronize();
						// if (csr->m == csr->n) std::swap(x, y);
						// else std::copy(y, y + copy_elements, x);
					}
				#endif
			}
			#else
			{
				__attribute__((unused)) volatile double warmup_total;
				long A_warmup_n = (1ULL<<20) * num_threads;
				double * A_warmup;
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

				// Warm up caches.
				MF->spmv(x, y);
			}
			#endif
		);

		printf("time warm up %lf\n", time_warm_up);

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

		// volatile unsigned long * L3_cache_block;
		// long L3_cache_block_n = topohw_get_cache_size(0, 3, TOPOHW_CT_U)  / sizeof(*L3_cache_block);
		// if (L3_cache_block_n == 0)
		// 	L3_cache_block_n = (1ULL<<20) * num_threads;
		// L3_cache_block = (typeof(L3_cache_block)) malloc(L3_cache_block_n * sizeof(*L3_cache_block));
		// int clear_caches = atoi(getenv("CLEAR_CACHES"));
		time_total = 0;
		num_loops = 0;
		dynarray_d * da_iter_times = dynarray_new_d(10 * min_num_loops);
		#ifdef HYBRID
			dynarray_d * da_iter_times_cpu = dynarray_new_d(10 * min_num_loops);
			dynarray_d * da_iter_times_gpu = dynarray_new_d(10 * min_num_loops);
			time_total_cpu = 0;
			time_total_gpu = 0;
		#endif

		#ifdef CUDA_KERNEL
			int profiling_call = atoi(getenv("PROFILING_CALL"));
			if (profiling_call){
				nvtxRangePushA("SpMV_Main_Loop");
				cudaProfilerStart();
			}
		#else
			int profiling_call = 0;
		#endif

		while (time_total < min_runtime || num_loops < min_num_loops)
		{
			// if (__builtin_expect(clear_caches, 0))
			// {
			// 	if (num_loops >= min_num_loops)
			// 		break;
			// 	_Pragma("omp parallel")
			// 	{
			// 		long i;
			// 		_Pragma("omp for")
			// 		for (i=0;i<L3_cache_block_n;i++)
			// 			L3_cache_block[i] = 0;
			// 	}
			// }

			// rapl_read_start(regs, regs_n);

			#ifdef SDV_TRACING
				char region_name[] = "COMPUTATION-SpMV";
				trace_begin_region(region_name);
			#endif
			
			bool is_final_loop = false;
			if (time_total >= min_runtime && num_loops == min_num_loops - 1) {
				is_final_loop = true;
			}
			
			// char residency_string[100];
			// sprintf(residency_string, "x (During Hybrid Kernel) - Iteration %ld BEFORE", num_loops);
			// checkResidency(x, residency_string);
			// sprintf(residency_string, "y (During Hybrid Kernel) - Iteration %ld BEFORE", num_loops);
			// checkResidency(y, residency_string);

			#ifdef HYBRID
				HA->set_last_iteration(is_final_loop);
				time_iter = time_it(1, 
					HA->spmv(x, y);
				);
				// Individual partial timings are now handled inside HA->spmv methods
				// and aggregated in time_cpu_total / time_gpu_total.
				// Update: this has been removed. Now we store cpu and gpu times like for the non-hybrid case.
				// In order to take the median of each later for reporting.
			#else
				MF->set_last_iteration(is_final_loop);
				time_iter = time_it(1,
					MF->spmv(x, y);
					MF->synchronize();
				);
			#endif

			// sprintf(residency_string, "x (During Hybrid Kernel) - Iteration %ld AFTER", num_loops);
			// checkResidency(x, residency_string);
			// sprintf(residency_string, "y (During Hybrid Kernel) - Iteration %ld AFTER", num_loops);
			// checkResidency(y, residency_string);

			// Zero-cost pointer swap eliminates the O(N) host memory bandwidth copy on square matrices!
			// perform pointer swapping, only when it is an intermediate iteration. If last, no need to do it 
			// since we will print the final result next.
			if(!is_final_loop){
				if (csr->m == csr->n) std::swap(x, y);
				else std::copy(y, y + copy_elements, x);
			}

			#ifdef SDV_TRACING
				trace_end_region(region_name);
			#endif

			// rapl_read_end(regs, regs_n);

			dynarray_push_back_d(da_iter_times, time_iter);
			#ifdef HYBRID
				double time_iter_cpu, time_iter_gpu;
				time_iter_cpu = HA->cpu_part->get_last_duration();
				time_iter_gpu = HA->gpu_part->get_last_duration();
				dynarray_push_back_d(da_iter_times_cpu, time_iter_cpu);
				dynarray_push_back_d(da_iter_times_gpu, time_iter_gpu);
				time_total_cpu += time_iter_cpu;
				time_total_gpu += time_iter_gpu;
			#endif
			time_total += time_iter;
			num_loops++;
		}
		#ifdef CUDA_KERNEL
			if (profiling_call){
				cudaProfilerStop();
				nvtxRangePop();
			}
		#endif

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
		
		#ifdef HYBRID
			long iter_times_cpu_n;
			double * iter_times_cpu;
			iter_times_cpu_n = dynarray_export_array_d(da_iter_times_cpu, &iter_times_cpu);
			if (iter_times_cpu_n != num_loops)
				error("dynamic array size not equal to number of loops: %ld != %ld", iter_times_cpu_n, num_loops);
			qsort(iter_times_cpu, num_loops, sizeof(*iter_times_cpu), qsort_cmp);
			time_min_cpu = iter_times_cpu[0];
			time_median_cpu = iter_times_cpu[num_loops/2];
			time_max_cpu = iter_times_cpu[num_loops-1];
			printf("time iter cpu: min=%g, median=%g, max=%g\n", time_min_cpu, time_median_cpu, time_max_cpu);
			free(iter_times_cpu);
			dynarray_destroy_d(&da_iter_times_cpu);
			long iter_times_gpu_n;
			double * iter_times_gpu;
			iter_times_gpu_n = dynarray_export_array_d(da_iter_times_gpu, &iter_times_gpu);
			if (iter_times_gpu_n != num_loops)
				error("dynamic array size not equal to number of loops: %ld != %ld", iter_times_gpu_n, num_loops);
			qsort(iter_times_gpu, num_loops, sizeof(*iter_times_gpu), qsort_cmp);
			time_min_gpu = iter_times_gpu[0];
			time_median_gpu = iter_times_gpu[num_loops/2];
			time_max_gpu = iter_times_gpu[num_loops-1];
			printf("time iter gpu: min=%g, median=%g, max=%g\n", time_min_gpu, time_median_gpu, time_max_gpu);
			free(iter_times_gpu);
			dynarray_destroy_d(&da_iter_times_gpu);
		#endif

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

		#ifdef WORK_REMOVAL
			gflops = MF->nnz / time_median * 2 * 1e-9;
		#else
			gflops = csr->nnz_matrix / time_median * 2 * 1e-9;
		#endif
		printf("GFLOPS = %lf (%s) (time = %.6lf ms)\n", gflops, getenv("PROGG"), time_median*1e3);
		#ifdef HYBRID
			// double gflops_cpu = (HA->cpu_part->m > 0) ? (HA->cpu_part->nnz * 2.0 / time_cpu_average * 1e-6) : 0;
			// double gflops_gpu = (HA->gpu_part->m > 0) ? (HA->gpu_part->nnz * 2.0 / time_gpu_average * 1e-6) : 0;
			// printf("   CPU Part: %lf GFLOPS (time average: %.6lf ms, %ld rows, %ld nnz)\n", gflops_cpu, time_cpu_average, HA->cpu_part->m, HA->cpu_part->nnz);
			// printf("   GPU Part: %lf GFLOPS (time average: %.6lf ms, %ld rows, %ld nnz)\n", gflops_gpu, time_gpu_average, HA->gpu_part->m, HA->gpu_part->nnz);
			
			double gflops_cpu = (HA->cpu_part->m > 0) ? (HA->cpu_part->nnz * 2.0 / time_median_cpu * 1e-6) : 0;
			double gflops_gpu = (HA->gpu_part->m > 0) ? (HA->gpu_part->nnz * 2.0 / time_median_gpu * 1e-6) : 0;
			printf("   CPU Part: %lf GFLOPS (time median: %.6lf ms, %ld rows, %ld nnz)\n", gflops_cpu, time_median_cpu, HA->cpu_part->m, HA->cpu_part->nnz);
			printf("   GPU Part: %lf GFLOPS (time median: %.6lf ms, %ld rows, %ld nnz)\n", gflops_gpu, time_median_gpu, HA->gpu_part->m, HA->gpu_part->nnz);
		#endif
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
			i += snprintf(buf + i, buf_n - i, ",%s", "csr_nnz_matrix");
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
			#if CHECK_ACCURACY
				i += check_accuracy_labels(buf + i, buf_n - i);
			#endif
			#ifdef PRINT_STATISTICS
				#ifdef HYBRID
					i += snprintf(buf + i, buf_n - i, ",%s", "time_cpu");
					i += snprintf(buf + i, buf_n - i, ",%s", "time_cpu_iter_min");
					i += snprintf(buf + i, buf_n - i, ",%s", "time_cpu_iter_median");
					i += snprintf(buf + i, buf_n - i, ",%s", "time_cpu_iter_max");
					i += snprintf(buf + i, buf_n - i, ",%s", "time_gpu");
					i += snprintf(buf + i, buf_n - i, ",%s", "time_gpu_iter_min");
					i += snprintf(buf + i, buf_n - i, ",%s", "time_gpu_iter_median");
					i += snprintf(buf + i, buf_n - i, ",%s", "time_gpu_iter_max");
				#else
					i += statistics_print_labels(buf + i, buf_n - i);
				#endif
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
		i += snprintf(buf + i, buf_n - i, ",%lu", csr->nnz_matrix);
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
		#if CHECK_ACCURACY
			i += check_accuracy(buf + i, buf_n - i, csr, x_ref, y, csr->symmetric, csr->expanded_symmetry, num_loops);
		#endif
		#ifdef PRINT_STATISTICS
			#ifdef HYBRID
				i += snprintf(buf + i, buf_n - i, ",%lf", time_total_cpu);
				i += snprintf(buf + i, buf_n - i, ",%lf", time_min_cpu);
				i += snprintf(buf + i, buf_n - i, ",%lf", time_median_cpu);
				i += snprintf(buf + i, buf_n - i, ",%lf", time_max_cpu);
				i += snprintf(buf + i, buf_n - i, ",%lf", time_total_gpu);
				i += snprintf(buf + i, buf_n - i, ",%lf", time_min_gpu);
				i += snprintf(buf + i, buf_n - i, ",%lf", time_median_gpu);
				i += snprintf(buf + i, buf_n - i, ",%lf", time_max_gpu);
			#else
				i += MF->statistics_print_data(buf + i, buf_n - i);
			#endif
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

	if (print_labels_and_exit == 1)
	{
		// Had to change this when changing the pass-by-reference of x and y to pass-by-value
		// compute(NULL, NULL, NULL, NULL, NULL, 0, 0, 1);
		ValueType * dummyX = NULL, * dummyY = NULL;
		compute(NULL, NULL, dummyX, NULL, dummyY, 0, 0, 1);
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

	// // Define a bitmask representing NUMA Node 1
	// unsigned long nodemask = (1UL << 1);
	// Get the OS Page Size for the Grace CPU (usually 4096 or 65536)
	// long page_size = sysconf(_SC_PAGESIZE);
	// printf("page_size = %ld\n", page_size);

	long max_mn = (csr->m > csr->n) ? csr->m : csr->n;
	x_ref = (typeof(x_ref)) aligned_alloc(64, max_mn * sizeof(*x_ref));
	#if defined(VECTOR_ALLOC_EXPLICIT)
		// EXPLICIT: system alloc + pin for async transfers
		x = (typeof(x)) aligned_alloc(64, (max_mn+64) * sizeof(*x));
		#ifdef CUDA_KERNEL
			cudaHostRegister(x, (max_mn+64) * sizeof(*x), cudaHostRegisterDefault);
		#endif
	#elif defined(VECTOR_ALLOC_MALLOCHOST)
		#ifdef CUDA_KERNEL
			cudaMallocHost(&x, (max_mn+64) * sizeof(*x));
		#else
			x = (typeof(x)) aligned_alloc(64, (max_mn+64) * sizeof(*x));
		#endif
	#elif defined(VECTOR_ALLOC_MANAGED)
		#ifdef CUDA_KERNEL
			cudaMallocManaged(&x, (max_mn+64) * sizeof(*x));
		#else
			x = (typeof(x)) aligned_alloc(64, (max_mn+64) * sizeof(*x));
		#endif
	#elif defined(VECTOR_ALLOC_MALLOC)
		// MALLOC: plain system memory (relies on ATS/HMM)
		x = (typeof(x)) aligned_alloc(64, (max_mn+64) * sizeof(*x));

		// size_t raw_size_x = (max_mn + 64) * sizeof(*x);
		// size_t rounded_size_x = (raw_size_x + page_size - 1) / page_size * page_size;
		// x = (typeof(x)) aligned_alloc(page_size, rounded_size_x);

		// int ret_x = mbind(x, rounded_size_x, MPOL_BIND, &nodemask, sizeof(nodemask)*8, 0);
		// if (ret_x != 0) {
 		// 	printf("ERROR: mbind failed for x. Reason: %s\n", strerror(errno));
		// }
	#else
		// Default fallback (no VECTOR_ALLOC flag): behave like EXPLICIT
		x = (typeof(x)) aligned_alloc(64, (max_mn+64) * sizeof(*x));
		#ifdef CUDA_KERNEL
			cudaHostRegister(x, (max_mn+64) * sizeof(*x), cudaHostRegisterDefault);
		#endif
	#endif

	#pragma omp parallel
	{
		int tnum = omp_get_thread_num();
		struct Random_State * rs = random_new(tnum);
		#pragma omp for
		for(long i=0;i<max_mn;++i)
		{
			x_ref[i] = 1.0;
			// x_ref[i] = random_uniform(rs, 0, 1);
			x[i] = x_ref[i];
		}
		random_destroy(&rs);
	}

	#if (defined(VECTOR_ALLOC_MANAGED) || defined(VECTOR_ALLOC_MALLOC)) && defined(CUDA_KERNEL)
	{
		// Prefetch x to GPU so it resides on GPU HBM before kernels start.
		cudaMemLocation prefetch_loc_x;
		prefetch_loc_x.type = cudaMemLocationTypeDevice;
		prefetch_loc_x.id = 0;
		cudaMemPrefetchAsync(x, max_mn * sizeof(*x), prefetch_loc_x, 0);
		cudaDeviceSynchronize();
	}
	#endif

	#if defined(VECTOR_ALLOC_EXPLICIT)
		y = (typeof(y)) aligned_alloc(64, (max_mn+64) * sizeof(*y));
		#ifdef CUDA_KERNEL
			cudaHostRegister(y, (max_mn+64) * sizeof(*y), cudaHostRegisterDefault);
		#endif
	#elif defined(VECTOR_ALLOC_MALLOCHOST)
		#ifdef CUDA_KERNEL
			cudaMallocHost(&y, (max_mn+64) * sizeof(*y));
		#else
			y = (typeof(y)) aligned_alloc(64, (max_mn+64) * sizeof(*y));
		#endif
	#elif defined(VECTOR_ALLOC_MANAGED)
		#ifdef CUDA_KERNEL
			cudaMallocManaged(&y, (max_mn+64) * sizeof(*y));
		#else
			y = (typeof(y)) aligned_alloc(64, (max_mn+64) * sizeof(*y));
		#endif
	#elif defined(VECTOR_ALLOC_MALLOC)
		y = (typeof(y)) aligned_alloc(64, (max_mn+64) * sizeof(*y));

		// size_t raw_size_y = (max_mn + 64) * sizeof(*y);
		// size_t rounded_size_y = (raw_size_y + page_size - 1) / page_size * page_size;
		// y = (typeof(y)) aligned_alloc(page_size, rounded_size_y);
		// int ret_y = mbind(y, rounded_size_y, MPOL_BIND, &nodemask, sizeof(nodemask)*8, 0);
		// if (ret_y != 0) {
 		// 	printf("ERROR: mbind failed for y. Reason: %s\n", strerror(errno));
		// }
	#else
		// Default fallback: EXPLICIT
		y = (typeof(y)) aligned_alloc(64, (max_mn+64) * sizeof(*y));
		#ifdef CUDA_KERNEL
			cudaHostRegister(y, (max_mn+64) * sizeof(*y), cudaHostRegisterDefault);
		#endif
	#endif


	#pragma omp parallel for
	for(long i=0;i<max_mn;i++)
		y[i] = 0.0;    // Test whether the format zeros rows with no nnz.

	#if (defined(VECTOR_ALLOC_MANAGED) || defined(VECTOR_ALLOC_MALLOC)) && defined(CUDA_KERNEL)
	{
		// Prefetch y to GPU so it resides on GPU HBM before kernels start.
		cudaMemLocation prefetch_loc_y;
		prefetch_loc_y.type = cudaMemLocationTypeDevice;
		prefetch_loc_y.id = 0;
		cudaMemPrefetchAsync(y, (max_mn+64) * sizeof(*y), prefetch_loc_y, 0);
		cudaDeviceSynchronize();
	}
	#endif

	long min_num_loops;
	#ifdef SDV_TRACING
		min_num_loops = 1;
	#else
		// min_num_loops = 1;
		// min_num_loops = 4;
		// min_num_loops = 64;
		min_num_loops = 128;
		// min_num_loops = 256;
	#endif

	double min_runtime;
	#ifdef SDV_TRACING
		min_runtime = 0;
	#else
		min_runtime = 0;
		// min_runtime = 2.0;
	#endif

	// checkResidency(x, "x (Pre-Kernel)");
	// checkResidency(y, "y (Pre-Kernel)");

	compute(csr, MF, x, x_ref, y, min_num_loops, min_runtime, 0);

	// checkResidency(x, "x (Post-Kernel)");
	// checkResidency(y, "y (Post-Kernel)");

	#ifdef CUDA_KERNEL
		#if defined(VECTOR_ALLOC_EXPLICIT)
			cudaHostUnregister(x);
			cudaHostUnregister(y);
			free(x);
			free(y);
		#elif defined(VECTOR_ALLOC_MALLOCHOST)
			cudaFreeHost(x);
			cudaFreeHost(y);
		#elif defined(VECTOR_ALLOC_MANAGED)
			cudaFree(x);
			cudaFree(y);
		#elif defined(VECTOR_ALLOC_MALLOC)
			free(x);
			free(y);
		#else
			// Default fallback: EXPLICIT
			cudaHostUnregister(x);
			cudaHostUnregister(y);
			free(x);
			free(y);
		#endif
	#else
		free(x);
		free(y);
	#endif
}
