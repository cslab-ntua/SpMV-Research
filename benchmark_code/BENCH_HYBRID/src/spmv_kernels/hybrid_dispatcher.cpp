#include <stdlib.h>
#include <stdio.h>

#include "macros/cpp_defines.h"
#include "spmv_kernel.h"
#include "hybrid_dispatcher.h"
#include "partitioning_strategies.h"
#include "csr_utils.h"

#ifdef __cplusplus
extern "C" {
#endif
    #include "macros/macrolib.h"
    #include "time_it.h"
#ifdef __cplusplus
}
#endif

// =============================================================================
// Kernel function names — injected by the Makefile via -D flags at compile time.
// The #ifndef defaults reproduce the original armpl + cuda_csr_transpose_expand_rows
// behaviour and ensure the file compiles without any -D flags.
// =============================================================================

#ifndef CPU_KERNEL_FUNC
#define CPU_KERNEL_FUNC      armpl_to_format
#endif
#ifndef CPU_KERNEL_STATS_FUNC
#define CPU_KERNEL_STATS_FUNC armpl_statistics_print_labels
#endif
#ifndef GPU_KERNEL_FUNC
#define GPU_KERNEL_FUNC      cuda_csr_transpose_expand_rows_to_format
#endif
#ifndef GPU_KERNEL_STATS_FUNC
#define GPU_KERNEL_STATS_FUNC cuda_csr_transpose_expand_rows_statistics_print_labels
#endif
#ifndef HYBRID_FORMAT_NAME
#define HYBRID_FORMAT_NAME   "Hybrid_ArmPL_CudaCSR_transpose_expand_rows"
#endif

/**************************************************************************/
// The following macros only for debugging purposes. Need to delete them later!!
// In order to run the x-shared version of hybrid, just comment all lines below.
// In order to run the x-local-cpu with the sparse-gather optimization, uncomment DIAG_CPU_LOCAL_X_OPT.
// #define DIAG_GPU_ONLY
// #define DIAG_CPU_ONLY
// #define DIAG_ANNOY_GPU
// #define DIAG_CPU_COLIND0
// #define DIAG_CPU_LOCAL_X
// #define DIAG_CPU_LOCAL_X_UNOPT
#define DIAG_CPU_LOCAL_X_OPT
/**************************************************************************/

// Forward-declare both sub-format initializers using the injected macro names.
struct Matrix_Format * CPU_KERNEL_FUNC(INT_T * row_ptr, INT_T * col_ind, ValueTypeReference * values, long m, long n, long nnz, long symmetric, long symmetry_expanded);
struct Matrix_Format * GPU_KERNEL_FUNC(INT_T * row_ptr, INT_T * col_ind, ValueTypeReference * values, long m, long n, long nnz, long symmetric, long symmetry_expanded, long m_cpu);

void Hybrid_Arrays::spmv(ValueType * x, ValueType * y) {

	// Gather the necessary x elements first
	#ifdef DIAG_CPU_LOCAL_X_UNOPT
		double transfer_time = time_it(1,
			memcpy(x_cpu_local, x, this->n * sizeof(ValueType));
		);
		this->last_transfer_time = transfer_time;
	#elif defined(DIAG_CPU_LOCAL_X_OPT)
		double transfer_time = time_it(1,
			_Pragma("omp parallel for")
			for (long i = 0; i < num_gather; i++) {
				x_cpu_local[i] = x[gather_indices[i]];
			}
		);
		this->last_transfer_time = transfer_time;
		// printf(">>> x_cpu_local transfer (gather) completed in %g ms (throughput: %g GB/s)\n", transfer_time * 1e3, (num_gather * sizeof(ValueType)) / (transfer_time * 1e9));
	#endif
	
	#ifndef DIAG_CPU_ONLY
		// 1. Launch GPU kernel (Async). Writes its DtH directly to the pinned tail of y!
		gpu_part->spmv(x, y);
	#endif

    // 2. Launch CPU kernel (Sync, overlaps with GPU). Writes to the pinned head of y.
	
	// REMINDER: remove nvtxRangePushA and nvtxRangePop when finished with profiling!
	// nvtxRangePushA("CPU_SpMV_Computation");
	#ifndef DIAG_GPU_ONLY
		#ifdef DIAG_ANNOY_GPU
			// TEST 3 (Annoyance): CPU generates cache misses over x while GPU computes
			volatile double dummy = 0;
			for (int iter = 0; iter < 32; iter++) {
				#pragma omp parallel for
				for (long i = 0; i < this->n; i+=16) {
					dummy += x[i];
					// for (long i = 0; i < this->n; i ++) {
					// dummy += x[(i*1052420489LL) % this->n];
				}
			}
		#elif defined(DIAG_CPU_LOCAL_X)
            // Lazy allocate and copy ONCE during the first warmup iteration
            if (this->x_cpu_isolated == NULL) {
                this->x_cpu_isolated = (ValueType*) malloc(this->n * sizeof(ValueType));
                double memcpy_time = time_it(1,
                    memcpy(this->x_cpu_isolated, x, this->n * sizeof(ValueType));
                );
				printf(">>> memcpy to local copy of x completed in %g us (throughput: %g GB/s)\n", memcpy_time * 1e6, (this->n * sizeof(ValueType)) / (memcpy_time * 1e9));

				ValueType * a, * b;
				a = (ValueType*) malloc(this->n*sizeof(ValueType));
				b = (ValueType*) malloc(this->n*sizeof(ValueType));
				for(long i=0; i<this->n; i++) {
					a[i] = i;
				}
                double memcpy_time2 = time_it(1,
                    memcpy(b, a, this->n * sizeof(ValueType));
                );
				printf(">>> memcpy2 to local copy of x completed in %g us (throughput: %g GB/s)\n", memcpy_time2 * 1e6, (this->n * sizeof(ValueType)) / (memcpy_time2 * 1e9));
				
            }
            // CPU reads from isolated buffer
            cpu_part->spmv(this->x_cpu_isolated, y);
		#elif defined(DIAG_CPU_LOCAL_X_UNOPT)
			// Run the kernel with the dense local vector
			cpu_part->spmv(this->x_cpu_local, y);
		#elif defined(DIAG_CPU_LOCAL_X_OPT)
			// Run the kernel with the compact, dense local vector
			cpu_part->spmv(this->x_cpu_local, y);
		#else
			cpu_part->spmv(x, y);
		#endif
	#endif
	// nvtxRangePop();

	#ifndef DIAG_CPU_ONLY
		// 3. CPU part completed successfully! Immediately initiate proactive HtD push for the next iteration.
		gpu_part->issue_h2d_for_next_iteration(y);

		// 4. Wait against the sync barrier for the GPU's kernel and overlapping DtH transfers to conclude.
		gpu_part->synchronize();
	#endif

    // // 5. Record hardware-measured durations (in milliseconds)
    // double t_cpu = (cpu_part && (m_cpu > 0)) ? cpu_part->get_last_duration() : 0;
	// double t_gpu = (gpu_part && (m_gpu > 0)) ? gpu_part->get_last_duration() : 0;
	// // printf("call_count = %ld, t_cpu = %lf, t_gpu = %lf\n", call_count, t_cpu, t_gpu);

	// time_cpu_total += t_cpu;
	// time_gpu_total += t_gpu;
	call_count++;

	// printf("Hybrid Iteration: CPU Hardware = %f ms, GPU Hardware = %f ms\n", t_cpu, t_gpu);
}

void Hybrid_Arrays::cpu_spmv(ValueType * x, ValueType * y) {
	if (cpu_part && (m_cpu > 0)) cpu_part->spmv(x, y);
}

void Hybrid_Arrays::gpu_spmv(ValueType * x, ValueType * y) {
	if (gpu_part && (m_gpu > 0)) gpu_part->spmv(x, y);
}

void Hybrid_Arrays::gpu_spmv_sync(ValueType * x, ValueType * y) {
	if (gpu_part && (m_gpu > 0)) {
		gpu_part->spmv(x, y);
		gpu_part->synchronize();
	}
}

void Hybrid_Arrays::statistics_start() {
    // time_cpu_total = 0;
    // time_gpu_total = 0;
    call_count = 0;
    if (cpu_part) cpu_part->statistics_start();
    if (gpu_part) gpu_part->statistics_start();
}

int Hybrid_Arrays::statistics_print_data(char * buf, long buf_n) {
    int len = 0;
    // double avg_cpu = (call_count > 0) ? (time_cpu_total / call_count) : 0;
    // double avg_gpu = (call_count > 0) ? (time_gpu_total / call_count) : 0;
    // len += snprintf(buf + len, buf_n - len, ",%g,%g,%g,%g", 
    //                 time_cpu_total, time_gpu_total, avg_cpu, avg_gpu);
    
    if (cpu_part) len += cpu_part->statistics_print_data(buf + len, buf_n - len);
    if (gpu_part) len += gpu_part->statistics_print_data(buf + len, buf_n - len);
    return len;
}

// Forward declarations for label printing (injected names)
int CPU_KERNEL_STATS_FUNC(char * buf, long buf_n);
int GPU_KERNEL_STATS_FUNC(char * buf, long buf_n);

int statistics_print_labels(char * buf, long buf_n) {
    int len = 0;
    len += snprintf(buf + len, buf_n - len, ",%s,%s,%s,%s", 
                    "hybrid_cpu_time_total_ms", "hybrid_gpu_time_total_ms", "hybrid_cpu_time_avg_ms", "hybrid_gpu_time_avg_ms");
    len += CPU_KERNEL_STATS_FUNC(buf + len, buf_n - len);
    len += GPU_KERNEL_STATS_FUNC(buf + len, buf_n - len);
	return len;
}

// --- Main Dispatcher ---

struct Matrix_Format *
csr_to_format(INT_T * row_ptr, INT_T * col_ind, ValueTypeReference * values, long m, long n, long nnz, long symmetric, long symmetry_expanded) {

	double time_total, time_cpu = 0, time_gpu = 0;
	long m_cpu = 0, m_gpu = 0;
	const char * strat_name = "unknown";
	Hybrid_Arrays * hybrid = NULL;

	// Default ratio for ratio-driven strategies (can be overridden by -DHYBRID_RATIO=...)
	#ifndef HYBRID_RATIO
		#define HYBRID_RATIO 0.5
	#endif

	time_total = time_it(1,
		// We temporarily create hybrid here to get its row_map
		hybrid = new Hybrid_Arrays(m, n, nnz, 0); 
		
		#if defined(STRAT_FIXED)
			m_cpu = get_split_fixed_ratio(row_ptr, m, nnz, HYBRID_RATIO);
			strat_name = "FIXED_RATIO";
		#elif defined(STRAT_LLC)
			m_cpu = get_split_llc_budget(row_ptr, m, n);
			strat_name = "LLC_BUDGET";
		#elif defined(STRAT_SHORTEST_ROWS_LLC)
			m_cpu = get_split_shortest_rows_llc(row_ptr, m, n, hybrid->row_map);
			strat_name = "SHORTEST_ROWS_LLC";
		#elif defined(STRAT_SHORTEST_ROWS_SORTED)
			m_cpu = get_split_shortest_rows_sorted(row_ptr, m, nnz, HYBRID_RATIO, hybrid->row_map);
			strat_name = "SHORTEST_ROWS_SORTED";
		#elif defined(STRAT_LONGEST_ROWS_SORTED)
			m_cpu = get_split_longest_rows_sorted(row_ptr, m, nnz, HYBRID_RATIO, hybrid->row_map);
			strat_name = "LONGEST_ROWS_SORTED";
		#elif defined(STRAT_SHORTEST_ROWS_ORIGINAL)
			m_cpu = get_split_shortest_rows_original_order(row_ptr, m, nnz, HYBRID_RATIO, hybrid->row_map);
			strat_name = "SHORTEST_ROWS_ORIGINAL_ORDER";
		#elif defined(STRAT_LONGEST_ROWS_ORIGINAL)
			m_cpu = get_split_longest_rows_original_order(row_ptr, m, nnz, HYBRID_RATIO, hybrid->row_map);
			strat_name = "LONGEST_ROWS_ORIGINAL_ORDER";
		#elif defined(STRAT_BAD_ZONES_ROWS)
			m_cpu = get_split_bad_zones_rows(row_ptr, m, nnz, HYBRID_RATIO, hybrid->row_map);
			strat_name = "BAD_ZONES_ROWS";
		#elif defined(STRAT_BAD_ZONES_BANDWIDTH)
			m_cpu = get_split_bad_zones_bandwidth(row_ptr, col_ind, m, nnz, HYBRID_RATIO, hybrid->row_map);
			strat_name = "BAD_ZONES_BANDWIDTH";
		#elif defined(STRAT_BAD_ZONES_CACHELINES)
			m_cpu = get_split_bad_zones_cachelines(row_ptr, col_ind, m, nnz, HYBRID_RATIO, hybrid->row_map);
			strat_name = "BAD_ZONES_CACHELINES";
		#elif defined(STRAT_BAD_ZONES_PADDING)
			m_cpu = get_split_bad_zones_padding(row_ptr, m, nnz, HYBRID_RATIO, hybrid->row_map);
			strat_name = "BAD_ZONES_PADDING";
		#elif defined(STRAT_MIN_X_ACCESS_CPU)
			m_cpu = find_row_set_with_minimal_x_vector_references(row_ptr, col_ind, m, n, nnz, HYBRID_RATIO, hybrid->row_map);
			strat_name = "MIN_X_ACCESS_CPU";
		#else
			m_cpu = m * 0.2; // Default 20/80
			strat_name = "DEFAULT_20_80";
		#endif


		m_gpu = m - m_cpu;
		hybrid->m_cpu = m_cpu;
		hybrid->m_gpu = m_gpu;

		// Calculate total NNZ for each part
		long nnz_cpu = 0;
		for (long i = 0; i < m_cpu; i++) nnz_cpu += row_ptr[hybrid->row_map[i]+1] - row_ptr[hybrid->row_map[i]];
		long nnz_gpu = nnz - nnz_cpu;

		printf("Hybrid Matrix Partition (%s):\n", strat_name);
		printf("   CPU Portion: %ld rows (%.2f%%), %ld NNZs (%.2f%%)\n", m_cpu, (double)m_cpu/m*100.0, nnz_cpu, (double)nnz_cpu/nnz*100.0);
		printf("   GPU Portion: %ld rows (%.2f%%), %ld NNZs (%.2f%%)\n", m_gpu, (double)m_gpu/m*100.0, nnz_gpu, (double)nnz_gpu/nnz*100.0);

		// Initialize CPU sub-format
		if (m_cpu > 0) {
			INT_T *r_p, *c_i; ValueTypeReference *vals;
			extract_csr_fragment(row_ptr, col_ind, values, hybrid->row_map, 0, m_cpu, nnz_cpu, &r_p, &c_i, &vals);
			
            #ifdef DIAG_CPU_COLIND0
				// TEST 4 (Zero-Traffic): Force all CPU memory reads to x[0]
				for (long i = 0; i < nnz_cpu; i++) {
					c_i[i] = 0;
				}
            #endif

			/*************** SPARSE GATHER SETUP ***************/
			#ifdef DIAG_CPU_LOCAL_X_UNOPT
				hybrid->x_cpu_local = (ValueType*) malloc(n * sizeof(ValueType));
			#elif defined(DIAG_CPU_LOCAL_X_OPT)
				// 1. Find all unique columns the CPU needs
				bool* needed_cols = (bool*) calloc(n, sizeof(bool));
				for (long i = 0; i < nnz_cpu; i++) needed_cols[c_i[i]] = true;
				
				hybrid->num_gather = 0;
				for (long i = 0; i < n; i++) if (needed_cols[i]) hybrid->num_gather++;
				
				hybrid->gather_indices = (long*) malloc(hybrid->num_gather * sizeof(long));
				hybrid->x_cpu_local = (ValueType*) malloc(hybrid->num_gather * sizeof(ValueType));
				
				// 2. Build the gather map and a reverse lookup
				long* reverse_map = (long*) malloc(n * sizeof(long));
				long idx = 0;
				for (long i = 0; i < n; i++) {
					if (needed_cols[i]) {
						hybrid->gather_indices[idx] = i;
						reverse_map[i] = idx;
						idx++;
					}
				}
				free(needed_cols);
				// 3. Remap the CPU partition's column indices to the local, dense 0-to-num_gather space
				for (long i = 0; i < nnz_cpu; i++) {
					c_i[i] = reverse_map[c_i[i]];
				}
				free(reverse_map);
				printf("CPU needs %ld elements (%.2lf MB) instead of %.2lf MB \n", hybrid->num_gather, (hybrid->num_gather * sizeof(ValueType)) / (1024*1024.0), (n * sizeof(ValueType)) / (1024*1024.0));
			#endif
			/*************** END SPARSE GATHER SETUP ***************/

			time_cpu = time_it(1,
				hybrid->cpu_part = CPU_KERNEL_FUNC(r_p, c_i, vals, m_cpu, n, nnz_cpu, symmetric, symmetry_expanded);
			);
			free(r_p); free(c_i); free(vals);
		}

		// Initialize GPU sub-format
		if (m_gpu > 0) {
			INT_T *r_p, *c_i; ValueTypeReference *vals;
			extract_csr_fragment(row_ptr, col_ind, values, hybrid->row_map, m_cpu, m_gpu, nnz_gpu, &r_p, &c_i, &vals);
			time_gpu = time_it(1,
				hybrid->gpu_part = GPU_KERNEL_FUNC(r_p, c_i, vals, m_gpu, n, nnz_gpu, symmetric, symmetry_expanded, m_cpu);
			);
			free(r_p); free(c_i); free(vals);
		}
		
		hybrid->format_name = (char*)HYBRID_FORMAT_NAME;
	);

	printf("Hybrid conversion times: Total = %g s, CPU = %g s, GPU = %g s\n", time_total, time_cpu, time_gpu);
	return hybrid;
}
