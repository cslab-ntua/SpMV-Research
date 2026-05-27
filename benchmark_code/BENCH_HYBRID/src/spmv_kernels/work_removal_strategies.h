#ifndef WORK_REMOVAL_STRATEGIES_H
#define WORK_REMOVAL_STRATEGIES_H

#include "spmv_kernel.h"

// =============================================================================
// Work-Removal Strategy Helpers
//
// These functions determine which rows to REMOVE (skip) from a standalone GPU
// execution.  Each returns m_removed: the number of rows excluded from the GPU.
//
// row_map layout after each call:
//   row_map[0 .. m_gpu-1]      = rows that STAY on the GPU   (m_gpu = m - m_removed)
//   row_map[m_gpu .. m-1]      = rows that are REMOVED (skipped)
// =============================================================================

// --- Ratio-driven row-length removal ---

// Remove the X% shortest rows (fewest NNZ).  GPU keeps the longest rows.
long get_removal_shortest_rows(INT_T * row_ptr, long m, long total_nnz,
                               double ratio, INT_T * row_map);

// Remove the X% longest rows (most NNZ).  GPU keeps the shortest rows.
long get_removal_longest_rows(INT_T * row_ptr, long m, long total_nnz,
                              double ratio, INT_T * row_map);

// --- Threshold-driven removal ---

// Remove all rows with NNZ < threshold.  GPU keeps rows with NNZ >= threshold.
long get_removal_below_threshold(INT_T * row_ptr, long m,
                                 long threshold, INT_T * row_map);

// Remove all rows with NNZ > threshold.  GPU keeps rows with NNZ <= threshold.
long get_removal_above_threshold(INT_T * row_ptr, long m,
                                 long threshold, INT_T * row_map);

// --- Outlier-based removal ---

// Remove rows whose NNZ > mean + k_sigma * stddev.
long get_removal_outlier_rows(INT_T * row_ptr, long m,
                              double k_sigma, INT_T * row_map);

// --- Contiguous block removal ---

// Remove a contiguous block of X% rows from the beginning.
long get_removal_contiguous_block(INT_T * row_ptr, long m, long total_nnz,
                                  double ratio, INT_T * row_map);

// --- Bad zones removal (GPU thread-block aware) ---

// Remove the X% worst zones (zones spanning the most rows).
// A zone is a contiguous group of rows whose padded NNZ totals ~640
// (NNZ_PER_THREAD * BLOCK_SIZE), emulating a GPU thread block.
long get_removal_bad_zones_rows(INT_T * row_ptr, long m, long total_nnz,
                                double ratio, INT_T * row_map);

// Remove the X% worst zones scored by row bandwidth (max_col - min_col).
// High bandwidth = scattered x-vector accesses = bad for GPU cache.
long get_removal_bad_zones_bandwidth(INT_T * row_ptr, INT_T * col_ind,
                                     long m, long total_nnz,
                                     double ratio, INT_T * row_map);

// Remove the X% worst zones scored by unique cache lines touched in x.
// More unique cache lines = higher L2 miss rate = bad for GPU.
long get_removal_bad_zones_cachelines(INT_T * row_ptr, INT_T * col_ind,
                                      long m, long total_nnz,
                                      double ratio, INT_T * row_map);

// Remove the X% worst zones scored by zero-padding waste ratio.
// High padding ratio = wasted GPU FLOPS on zero-padded elements.
long get_removal_bad_zones_padding(INT_T * row_ptr, long m, long total_nnz,
                                   double ratio, INT_T * row_map);

#endif
