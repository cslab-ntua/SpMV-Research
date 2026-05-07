#include <stdlib.h>
#include <stdio.h>

#include "macros/cpp_defines.h"
#include "spmv_kernel.h"
#include "hybrid_dispatcher.h"

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

// Forward-declare both sub-format initializers using the injected macro names.
struct Matrix_Format * CPU_KERNEL_FUNC(INT_T * row_ptr, INT_T * col_ind, ValueTypeReference * values, long m, long n, long nnz, long symmetric, long symmetry_expanded);
struct Matrix_Format * GPU_KERNEL_FUNC(INT_T * row_ptr, INT_T * col_ind, ValueTypeReference * values, long m, long n, long nnz, long symmetric, long symmetry_expanded, long m_cpu);

void Hybrid_Arrays::spmv(ValueType * x, ValueType * y) {
    // 1. Launch GPU kernel (Async). Writes its DtH directly to the pinned tail of y!
    gpu_part->spmv(x, y);

    // 2. Launch CPU kernel (Sync, overlaps with GPU). Writes to the pinned head of y.
	
	// REMINDER: remove nvtxRangePushA and nvtxRangePop when finished with profiling!
	// nvtxRangePushA("CPU_SpMV_Computation");
    cpu_part->spmv(x, y);
	// nvtxRangePop();

    // 3. CPU part completed successfully! Immediately initiate proactive HtD push for the next iteration.
    gpu_part->issue_h2d_for_next_iteration(y);

    // 4. Wait against the sync barrier for the GPU's kernel and overlapping DtH transfers to conclude.
    gpu_part->synchronize();

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
    
    // if (cpu_part) len += cpu_part->statistics_print_data(buf + len, buf_n - len);
    // if (gpu_part) len += gpu_part->statistics_print_data(buf + len, buf_n - len);
    return len;
}

// Forward declarations for label printing (injected names)
int CPU_KERNEL_STATS_FUNC(char * buf, long buf_n);
int GPU_KERNEL_STATS_FUNC(char * buf, long buf_n);

int statistics_print_labels(char * buf, long buf_n) {
    int len = 0;
    len += snprintf(buf + len, buf_n - len, ",%s,%s,%s,%s", 
                    "hybrid_cpu_time_total_ms", "hybrid_gpu_time_total_ms", 
                    "hybrid_cpu_time_avg_ms", "hybrid_gpu_time_avg_ms");
    len += CPU_KERNEL_STATS_FUNC(buf + len, buf_n - len);
    len += GPU_KERNEL_STATS_FUNC(buf + len, buf_n - len);
	return len;
}

// --- Partitioning Strategy Helpers ---

static long get_split_fixed_ratio(INT_T * row_ptr, long m, long total_nnz) {
	#ifndef HYBRID_RATIO
		#define HYBRID_RATIO 0.5
	#endif
	// HYBRID_RATIO defines GPU portion. CPU gets (1 - ratio)
	long target_nnz_cpu = (long)(total_nnz * (1.0 - HYBRID_RATIO));
	long m_cpu = 0;
	while (m_cpu < m && row_ptr[m_cpu+1] < target_nnz_cpu) {
		m_cpu++;
	}
	return m_cpu;
}

struct RowSize {
	long id;
	long nnz;
};

static int compareRowSize(const void * a, const void * b) {
	return ((RowSize*)a)->nnz - ((RowSize*)b)->nnz;
}

static long get_split_shortest_rows_llc(INT_T * row_ptr, long m, long n, INT_T * row_map) {
	const size_t LLC_LIMIT = 114UL * 1024 * 1024;
	size_t size_x = n * sizeof(ValueType);
	
	if (size_x >= LLC_LIMIT) {
		return m * 0.10; // Fallback
	}

	RowSize * rows = (RowSize*) malloc(m * sizeof(RowSize));
	for (long i = 0; i < m; i++) {
		rows[i].id = i;
		rows[i].nnz = row_ptr[i+1] - row_ptr[i];
	}
	qsort(rows, m, sizeof(RowSize), compareRowSize);

	size_t available_budget = LLC_LIMIT - size_x;
	long m_cpu = 0;
	long cumulative_nnz = 0;
	for (long i = 0; i < m; i++) {
		cumulative_nnz += rows[i].nnz;
		size_t footprint_upto_i = ((i + 1) * sizeof(INT_T)) + (cumulative_nnz * (sizeof(ValueType) + sizeof(INT_T)));
		if (footprint_upto_i <= available_budget) {
			m_cpu = i + 1;
		} else {
			break;
		}
	}

	// Reflect sorted order in row_map
	for (long i = 0; i < m; i++) row_map[i] = rows[i].id;
	free(rows);
	
	return m_cpu;
}

static long get_split_llc_budget(INT_T * row_ptr, long m, long n) {
	const size_t LLC_LIMIT = 114UL * 1024 * 1024;
	size_t size_x = n * sizeof(ValueType);
	
	if (size_x >= LLC_LIMIT) {
		return m * 0.10; // Fallback: 10% to CPU if x is too large
	}

	size_t available_budget = LLC_LIMIT - size_x;
	long m_cpu = 0;
	for (long i = 0; i < m; i++) {
		long nnz_upto_i = row_ptr[i+1] - row_ptr[0];
		size_t matrix_size_upto_i = ((i + 1) * sizeof(INT_T)) + (nnz_upto_i * (sizeof(ValueType) + sizeof(INT_T)));
		if (matrix_size_upto_i <= available_budget) {
			m_cpu = i + 1;
		} else {
			break;
		}
	}
	return m_cpu;
}

// --- Main Dispatcher ---

struct Matrix_Format *
csr_to_format(INT_T * row_ptr, INT_T * col_ind, ValueTypeReference * values, long m, long n, long nnz, long symmetric, long symmetry_expanded) {

	double time_total, time_cpu = 0, time_gpu = 0;
	long m_cpu = 0, m_gpu = 0;
	const char * strat_name = "unknown";
	Hybrid_Arrays * hybrid = NULL;

	time_total = time_it(1,
		// We temporarily create hybrid here to get its row_map
		hybrid = new Hybrid_Arrays(m, n, nnz, 0); 
		
		#if defined(STRAT_FIXED)
			m_cpu = get_split_fixed_ratio(row_ptr, m, nnz);
			strat_name = "FIXED_RATIO";
		#elif defined(STRAT_LLC)
			m_cpu = get_split_llc_budget(row_ptr, m, n);
			strat_name = "LLC_BUDGET";
		#elif defined(STRAT_SHORTEST_ROWS_LLC)
			m_cpu = get_split_shortest_rows_llc(row_ptr, m, n, hybrid->row_map);
			strat_name = "SHORTEST_ROWS_LLC";
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
			// Create CSR fragment for CPU
			INT_T * r_p = (INT_T *) malloc((m_cpu + 1) * sizeof(INT_T));
			INT_T * c_i = (INT_T *) malloc(nnz_cpu * sizeof(INT_T));
			ValueTypeReference * vals = (ValueTypeReference *) malloc(nnz_cpu * sizeof(ValueTypeReference));
			
			r_p[0] = 0;
			long curr_nnz = 0;
			for (long i = 0; i < m_cpu; i++) {
				long row = hybrid->row_map[i];
				long row_nnz = row_ptr[row+1] - row_ptr[row];
				for (long j = 0; j < row_nnz; j++) {
					c_i[curr_nnz + j] = col_ind[row_ptr[row] + j];
					vals[curr_nnz + j] = values[row_ptr[row] + j];
				}
				curr_nnz += row_nnz;
				r_p[i+1] = curr_nnz;
			}

			time_cpu = time_it(1,
				hybrid->cpu_part = CPU_KERNEL_FUNC(r_p, c_i, vals, m_cpu, n, nnz_cpu, symmetric, symmetry_expanded);
			);
			free(r_p); free(c_i); free(vals);
		}

		// Initialize GPU sub-format
		if (m_gpu > 0) {
			INT_T * r_p = (INT_T *) malloc((m_gpu + 1) * sizeof(INT_T));
			INT_T * c_i = (INT_T *) malloc(nnz_gpu * sizeof(INT_T));
			ValueTypeReference * vals = (ValueTypeReference *) malloc(nnz_gpu * sizeof(ValueTypeReference));
			
			r_p[0] = 0;
			long curr_nnz = 0;
			for (long i = 0; i < m_gpu; i++) {
				long row = hybrid->row_map[m_cpu + i];
				long row_nnz = row_ptr[row+1] - row_ptr[row];
				for (long j = 0; j < row_nnz; j++) {
					c_i[curr_nnz + j] = col_ind[row_ptr[row] + j];
					vals[curr_nnz + j] = values[row_ptr[row] + j];
				}
				curr_nnz += row_nnz;
				r_p[i+1] = curr_nnz;
			}

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
