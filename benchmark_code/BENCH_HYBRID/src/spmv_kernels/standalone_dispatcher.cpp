#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "macros/cpp_defines.h"
#include "spmv_kernel.h"
#include "standalone_dispatcher.h"
#include "work_removal_strategies.h"

#ifdef __cplusplus
extern "C" {
#endif
    #include "macros/macrolib.h"
    #include "time_it.h"
#ifdef __cplusplus
}
#endif

// =============================================================================
// GPU kernel function name — injected by the Makefile via -D flags.
// Default: cuda_csr_transpose_expand_rows (matches existing standalone builds).
// =============================================================================

#ifndef GPU_KERNEL_FUNC
#define GPU_KERNEL_FUNC      cuda_csr_transpose_expand_rows_to_format
#endif
#ifndef GPU_KERNEL_STATS_FUNC
#define GPU_KERNEL_STATS_FUNC cuda_csr_transpose_expand_rows_statistics_print_labels
#endif
#ifndef STANDALONE_FORMAT_NAME
#define STANDALONE_FORMAT_NAME "Standalone_CudaCSR_transpose_expand_rows"
#endif

// Forward-declare the GPU sub-format initializer.
// Note: m_cpu parameter is kept for interface compatibility with the GPU kernel.
// For standalone removal mode we pass -1 (same as plain standalone).
struct Matrix_Format * GPU_KERNEL_FUNC(INT_T * row_ptr, INT_T * col_ind, ValueTypeReference * values, long m, long n, long nnz, long symmetric, long symmetry_expanded, long m_cpu);

// =============================================================================
// Standalone_Arrays method implementations
// =============================================================================

void Standalone_Arrays::spmv(ValueType * x, ValueType * y) {
    if (!gpu_part || m_gpu <= 0) return;

    gpu_part->spmv(x, y);
}

void Standalone_Arrays::synchronize() {
    if (!gpu_part || m_gpu <= 0) return;

    gpu_part->synchronize();
}


void Standalone_Arrays::statistics_start() {
    if (gpu_part) gpu_part->statistics_start();
}

int Standalone_Arrays::statistics_print_data(char * buf, long buf_n) {
    int len = 0;
    if (gpu_part) len += gpu_part->statistics_print_data(buf + len, buf_n - len);
    return len;
}

// Forward declarations for label printing
int GPU_KERNEL_STATS_FUNC(char * buf, long buf_n);

int statistics_print_labels(char * buf, long buf_n) {
    return GPU_KERNEL_STATS_FUNC(buf, buf_n);
}

// =============================================================================
// Main Dispatcher — csr_to_format()
//
// Compile-time strategy selection via -D flags (same pattern as hybrid_dispatcher).
// =============================================================================

struct Matrix_Format *
csr_to_format(INT_T * row_ptr, INT_T * col_ind, ValueTypeReference * values, long m, long n, long nnz, long symmetric, long symmetry_expanded) {

    double time_total, time_gpu = 0;
    long m_removed = 0;
    long m_gpu = 0;
    const char * strat_name = "NONE";
    Standalone_Arrays * sa = NULL;

    // Default ratio / threshold / k for strategies (overridden by -D flags)
    #ifndef WORK_REMOVAL_RATIO
        #define WORK_REMOVAL_RATIO 0.10
    #endif
    #ifndef WORK_REMOVAL_THRESHOLD
        #define WORK_REMOVAL_THRESHOLD 8
    #endif
    #ifndef WORK_REMOVAL_OUTLIER_K
        #define WORK_REMOVAL_OUTLIER_K 2.0
    #endif

    time_total = time_it(1,
        sa = new Standalone_Arrays(m, n, nnz);

        // --- Strategy selection ---
        #if defined(STRAT_REMOVE_SHORTEST_ROWS)
            m_removed = get_removal_shortest_rows(row_ptr, m, nnz, WORK_REMOVAL_RATIO, sa->row_map);
            strat_name = "REMOVE_SHORTEST_ROWS";
        #elif defined(STRAT_REMOVE_LONGEST_ROWS)
            m_removed = get_removal_longest_rows(row_ptr, m, nnz, WORK_REMOVAL_RATIO, sa->row_map);
            strat_name = "REMOVE_LONGEST_ROWS";
        #elif defined(STRAT_REMOVE_BELOW_THRESHOLD)
            m_removed = get_removal_below_threshold(row_ptr, m, WORK_REMOVAL_THRESHOLD, sa->row_map);
            strat_name = "REMOVE_BELOW_THRESHOLD";
        #elif defined(STRAT_REMOVE_ABOVE_THRESHOLD)
            m_removed = get_removal_above_threshold(row_ptr, m, WORK_REMOVAL_THRESHOLD, sa->row_map);
            strat_name = "REMOVE_ABOVE_THRESHOLD";
        #elif defined(STRAT_REMOVE_OUTLIER_ROWS)
            m_removed = get_removal_outlier_rows(row_ptr, m, WORK_REMOVAL_OUTLIER_K, sa->row_map);
            strat_name = "REMOVE_OUTLIER_ROWS";
        #elif defined(STRAT_REMOVE_CONTIGUOUS_BLOCK)
            m_removed = get_removal_contiguous_block(row_ptr, m, nnz, WORK_REMOVAL_RATIO, sa->row_map);
            strat_name = "REMOVE_CONTIGUOUS_BLOCK";
        #elif defined(STRAT_REMOVE_BAD_ZONES_ROWS)
            m_removed = get_removal_bad_zones_rows(row_ptr, m, nnz, WORK_REMOVAL_RATIO, sa->row_map);
            strat_name = "REMOVE_BAD_ZONES_ROWS";
        #elif defined(STRAT_REMOVE_BAD_ZONES_BW)
            m_removed = get_removal_bad_zones_bandwidth(row_ptr, col_ind, m, nnz, WORK_REMOVAL_RATIO, sa->row_map);
            strat_name = "REMOVE_BAD_ZONES_BW";
        #elif defined(STRAT_REMOVE_BAD_ZONES_CL)
            m_removed = get_removal_bad_zones_cachelines(row_ptr, col_ind, m, nnz, WORK_REMOVAL_RATIO, sa->row_map);
            strat_name = "REMOVE_BAD_ZONES_CL";
        #elif defined(STRAT_REMOVE_BAD_ZONES_PAD)
            m_removed = get_removal_bad_zones_padding(row_ptr, m, nnz, WORK_REMOVAL_RATIO, sa->row_map);
            strat_name = "REMOVE_BAD_ZONES_PAD";
        #else
            // No removal — run full matrix on GPU (baseline)
            m_removed = 0;
            strat_name = "NONE";
        #endif

        m_gpu = m - m_removed;
        sa->m_removed = m_removed;
        sa->m_gpu = m_gpu;

        // Calculate NNZ for each part
        long nnz_gpu = 0;
        for (long i = 0; i < m_gpu; i++)
            nnz_gpu += row_ptr[sa->row_map[i]+1] - row_ptr[sa->row_map[i]];
        long nnz_removed = nnz - nnz_gpu;
        sa->nnz_gpu = nnz_gpu;
        sa->nnz_removed = nnz_removed;

        // Compute average NNZ for removed and GPU rows
        double avg_nnz_removed = (m_removed > 0) ? (double)nnz_removed / m_removed : 0;
        double avg_nnz_gpu = (m_gpu > 0) ? (double)nnz_gpu / m_gpu : 0;

        printf("Standalone Work Removal (%s):\n", strat_name);
        printf("   Removed: %ld rows (%.2f%%), %ld NNZs (%.2f%%), avg NNZ/row: %.2f\n",
               m_removed, (double)m_removed/m*100.0,
               nnz_removed, (double)nnz_removed/nnz*100.0,
               avg_nnz_removed);
        printf("   GPU:     %ld rows (%.2f%%), %ld NNZs (%.2f%%), avg NNZ/row: %.2f\n",
               m_gpu, (double)m_gpu/m*100.0,
               nnz_gpu, (double)nnz_gpu/nnz*100.0,
               avg_nnz_gpu);

        // Build the reduced GPU sub-matrix from the selected rows
        if (m_gpu > 0) {
            INT_T * r_p = (INT_T *) malloc((m_gpu + 1) * sizeof(INT_T));
            INT_T * c_i = (INT_T *) malloc(nnz_gpu * sizeof(INT_T));
            ValueTypeReference * vals = (ValueTypeReference *) malloc(nnz_gpu * sizeof(ValueTypeReference));

            r_p[0] = 0;
            long curr_nnz = 0;
            for (long i = 0; i < m_gpu; i++) {
                long row = sa->row_map[i];
                long row_nnz = row_ptr[row+1] - row_ptr[row];
                for (long j = 0; j < row_nnz; j++) {
                    c_i[curr_nnz + j] = col_ind[row_ptr[row] + j];
                    vals[curr_nnz + j] = values[row_ptr[row] + j];
                }
                curr_nnz += row_nnz;
                r_p[i+1] = curr_nnz;
            }

            time_gpu = time_it(1,
                // Pass m_cpu=-1 to signal standalone mode to the GPU kernel
                sa->gpu_part = GPU_KERNEL_FUNC(r_p, c_i, vals, m_gpu, n, nnz_gpu, symmetric, symmetry_expanded, -1);
            );
            free(r_p); free(c_i); free(vals);
        }

        // Update Matrix_Format fields to reflect the GPU sub-matrix
        sa->m = m_gpu;
        sa->nnz = nnz_gpu;
        sa->mem_footprint = nnz_gpu * (sizeof(ValueType) + sizeof(INT_T)) + (m_gpu+1) * sizeof(INT_T);
        sa->format_name = (char*)STANDALONE_FORMAT_NAME;
    );

    printf("Standalone conversion time: Total = %g s, GPU init = %g s\n", time_total, time_gpu);
    return sa;
}
