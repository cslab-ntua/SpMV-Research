#include <stdlib.h>
#include <stdio.h>

#include "macros/cpp_defines.h"
#include "spmv_kernel.h"
#include "partitioning_strategies.h"

// --- Helper struct & compare functions for sorting rows by NNZ ---
struct RowSize {
	long id;
	long nnz;
};

static int compareRowSizeAscending(const void * a, const void * b) {
	return ((RowSize*)a)->nnz - ((RowSize*)b)->nnz;
}

static int compareRowSizeDescending(const void * a, const void * b) {
	return ((RowSize*)b)->nnz - ((RowSize*)a)->nnz;
}

static int compareRowID(const void * a, const void * b) {
	if (((RowSize*)a)->id < ((RowSize*)b)->id) return -1;
	if (((RowSize*)a)->id > ((RowSize*)b)->id) return 1;
	return 0;
}

// =============================================================================
// Partitioning Strategy Helpers
//
// Each function determines how many rows (m_cpu) to assign to the CPU part
// of the hybrid SpMV.  The remainder (m - m_cpu) goes to the GPU.
// =============================================================================

// --- Fixed-ratio split (sequential rows) ---
long get_split_fixed_ratio(INT_T * row_ptr, long m, long total_nnz, double ratio) {
	// ratio defines GPU portion. CPU gets (1 - ratio)
	long target_nnz_cpu = (long)(total_nnz * (1.0 - ratio));
	long m_cpu = 0;
	while (m_cpu < m && row_ptr[m_cpu+1] < target_nnz_cpu) {
		m_cpu++;
	}
	return m_cpu;
}

// --- LLC-budget split (sequential rows) ---
long get_split_llc_budget(INT_T * row_ptr, long m, long n) {
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

// --- Shortest-rows LLC split (row reordering) ---
long get_split_shortest_rows_llc(INT_T * row_ptr, long m, long n, INT_T * row_map) {
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
	qsort(rows, m, sizeof(RowSize), compareRowSizeAscending);

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

// =============================================================================
// New ratio-driven strategies with row reordering
//
// "SHORTEST_ROWS" = GPU gets the shortest rows; CPU gets the longest.
//   → Sort descending so CPU (front) gets longest, GPU (tail) gets shortest.
// "LONGEST_ROWS"  = GPU gets the longest rows;  CPU gets the shortest.
//   → Sort ascending so CPU (front) gets shortest, GPU (tail) gets longest.
// =============================================================================

// --- Helper: generic sorted split ---
// Sorts rows, assigns CPU rows from the front until target_nnz_cpu is met.
// Returns m_cpu. row_map is populated with the sorted row IDs.
static long split_sorted(RowSize * rows, long m, long target_nnz_cpu, INT_T * row_map) {
	long m_cpu = 0;
	long cumulative_nnz = 0;
	for (long i = 0; i < m; i++) {
		cumulative_nnz += rows[i].nnz;
		if (cumulative_nnz <= target_nnz_cpu) {
			m_cpu = i + 1;
		} else {
			break;
		}
	}

	// Populate row_map (CPU rows first, then GPU rows)
	for (long i = 0; i < m; i++) row_map[i] = rows[i].id;

	return m_cpu;
}

// --- GPU gets shortest rows, sorted order within partitions ---
long get_split_shortest_rows_sorted(INT_T * row_ptr, long m, long total_nnz, double ratio, INT_T * row_map) {
	long target_nnz_cpu = (long)(total_nnz * (1.0 - ratio));

	RowSize * rows = (RowSize*) malloc(m * sizeof(RowSize));
	for (long i = 0; i < m; i++) {
		rows[i].id = i;
		rows[i].nnz = row_ptr[i+1] - row_ptr[i];
	}
	// Sort descending: CPU (front) gets longest rows, GPU (tail) gets shortest.
	qsort(rows, m, sizeof(RowSize), compareRowSizeDescending);

	long m_cpu = split_sorted(rows, m, target_nnz_cpu, row_map);

	free(rows);
	return m_cpu;
}

// --- GPU gets longest rows, sorted order within partitions ---
long get_split_longest_rows_sorted(INT_T * row_ptr, long m, long total_nnz, double ratio, INT_T * row_map) {
	long target_nnz_cpu = (long)(total_nnz * (1.0 - ratio));

	RowSize * rows = (RowSize*) malloc(m * sizeof(RowSize));
	for (long i = 0; i < m; i++) {
		rows[i].id = i;
		rows[i].nnz = row_ptr[i+1] - row_ptr[i];
	}
	// Sort ascending: CPU (front) gets shortest rows, GPU (tail) gets longest.
	qsort(rows, m, sizeof(RowSize), compareRowSizeAscending);

	long m_cpu = split_sorted(rows, m, target_nnz_cpu, row_map);

	free(rows);
	return m_cpu;
}

// --- GPU gets shortest rows, original order restored within partitions ---
long get_split_shortest_rows_original_order(INT_T * row_ptr, long m, long total_nnz, double ratio, INT_T * row_map) {
	long target_nnz_cpu = (long)(total_nnz * (1.0 - ratio));

	RowSize * rows = (RowSize*) malloc(m * sizeof(RowSize));
	for (long i = 0; i < m; i++) {
		rows[i].id = i;
		rows[i].nnz = row_ptr[i+1] - row_ptr[i];
	}
	// Sort descending: CPU (front) gets longest rows, GPU (tail) gets shortest.
	qsort(rows, m, sizeof(RowSize), compareRowSizeDescending);

	long m_cpu = split_sorted(rows, m, target_nnz_cpu, row_map);

	// Restore original order within each partition
	qsort(rows, m_cpu, sizeof(RowSize), compareRowID);
	qsort(rows + m_cpu, m - m_cpu, sizeof(RowSize), compareRowID);
	for (long i = 0; i < m; i++) row_map[i] = rows[i].id;

	free(rows);
	return m_cpu;
}

// --- GPU gets longest rows, original order restored within partitions ---
long get_split_longest_rows_original_order(INT_T * row_ptr, long m, long total_nnz, double ratio, INT_T * row_map) {
	long target_nnz_cpu = (long)(total_nnz * (1.0 - ratio));

	RowSize * rows = (RowSize*) malloc(m * sizeof(RowSize));
	for (long i = 0; i < m; i++) {
		rows[i].id = i;
		rows[i].nnz = row_ptr[i+1] - row_ptr[i];
	}
	// Sort ascending: CPU (front) gets shortest rows, GPU (tail) gets longest.
	qsort(rows, m, sizeof(RowSize), compareRowSizeAscending);

	long m_cpu = split_sorted(rows, m, target_nnz_cpu, row_map);

	// Restore original order within each partition
	qsort(rows, m_cpu, sizeof(RowSize), compareRowID);
	qsort(rows + m_cpu, m - m_cpu, sizeof(RowSize), compareRowID);
	for (long i = 0; i < m; i++) row_map[i] = rows[i].id;

	free(rows);
	return m_cpu;
}
