#ifndef PARTITIONING_STRATEGIES_H
#define PARTITIONING_STRATEGIES_H

#include "spmv_kernel.h"

// --- Partitioning Strategy Helpers ---
// These functions determine how to split rows between CPU and GPU.
// Each returns m_cpu: the number of rows assigned to the CPU.
// CPU rows occupy row_map[0..m_cpu-1], GPU rows occupy row_map[m_cpu..m-1].

// Fixed-ratio split: CPU gets (1 - ratio) of the total NNZ sequentially.
// `ratio` is the GPU portion (e.g. 0.65 means 65% GPU, 35% CPU).
long get_split_fixed_ratio(INT_T * row_ptr, long m, long total_nnz, double ratio);

// --- LLC-driven splits ---

// LLC-budget split: assigns rows to CPU sequentially until the matrix
// footprint (row_ptr + col_ind + values for those rows) fills the LLC.
long get_split_llc_budget(INT_T * row_ptr, long m, long n);

// Shortest-rows LLC split: sorts rows by NNZ (ascending), assigns the
// shortest rows to CPU until the LLC budget is exhausted.
// Populates row_map[] with the sorted order (CPU rows first, then GPU).
long get_split_shortest_rows_llc(INT_T * row_ptr, long m, long n, INT_T * row_map);

// --- Ratio-driven strategies with row reordering ---
// "SHORTEST_ROWS" = GPU gets the shortest rows; CPU gets the longest.
// "LONGEST_ROWS"  = GPU gets the longest rows;  CPU gets the shortest.
// "_SORTED"       = rows within each partition retain NNZ-sorted order.
// "_ORIGINAL"     = rows within each partition are restored to original IDs.

// GPU gets shortest rows, sorted order within partitions.
long get_split_shortest_rows_sorted(INT_T * row_ptr, long m, long total_nnz, double ratio, INT_T * row_map);

// GPU gets longest rows, sorted order within partitions.
long get_split_longest_rows_sorted(INT_T * row_ptr, long m, long total_nnz, double ratio, INT_T * row_map);

// GPU gets shortest rows, original order restored within partitions.
long get_split_shortest_rows_original_order(INT_T * row_ptr, long m, long total_nnz, double ratio, INT_T * row_map);

// GPU gets longest rows, original order restored within partitions.
long get_split_longest_rows_original_order(INT_T * row_ptr, long m, long total_nnz, double ratio, INT_T * row_map);

#endif
