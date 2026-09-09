#include <stdlib.h>
#include <stdio.h>

#include "macros/cpp_defines.h"
#include "spmv_kernel.h"
#include "partitioning_strategies.h"

// LLC size for cache-aware strategies.  Override with -DLLC_LIMIT_BYTES=...
#ifndef LLC_LIMIT_BYTES
#define LLC_LIMIT_BYTES (114UL * 1024 * 1024)   // Default: 114 MB (GH200 Grace)
#endif

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
	const size_t LLC_LIMIT = LLC_LIMIT_BYTES;
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
	const size_t LLC_LIMIT = LLC_LIMIT_BYTES;
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

// =============================================================================
// Bad Zones Partitioning Strategies
//
// These methods identify contiguous blocks of rows (zones) that exhibit poor
// performance characteristics on the GPU (e.g., highly divergent, scattered
// memory accesses) and assign them to the CPU.
// =============================================================================

#define ZONE_BLOCK_SIZE 128

struct Zone {
	long start_row;
	long end_row;
	long num_rows;
	double score;
};

static int compareZoneScoreDescending(const void * a, const void * b) {
	double diff = ((Zone*)b)->score - ((Zone*)a)->score;
	return (diff > 0) ? 1 : (diff < 0) ? -1 : 0;
}

static int compareLong(const void * a, const void * b) {
	long diff = *(const long *)a - *(const long *)b;
	return (diff > 0) ? 1 : (diff < 0) ? -1 : 0;
}

static long build_zones(INT_T * row_ptr, long m, long total_nnz, Zone ** out_zones, long * out_nnz_per_thread, long * out_zone_target_nnz)
{
	double nnz_per_row = (double)total_nnz / m;
	long nnz_per_thread;
	if (nnz_per_row < 1) nnz_per_thread = 1;
	else if (nnz_per_row < 3) nnz_per_thread = (long)(nnz_per_row + 0.5);
	else if (nnz_per_row < 5) nnz_per_thread = (long)(nnz_per_row + 0.9);
	else nnz_per_thread = 5;
	
	long zone_target_nnz = nnz_per_thread * ZONE_BLOCK_SIZE;
	
	if (out_nnz_per_thread) *out_nnz_per_thread = nnz_per_thread;
	if (out_zone_target_nnz) *out_zone_target_nnz = zone_target_nnz;

	Zone * zones = (Zone *) malloc(m * sizeof(Zone));
	long num_zones = 0;
	long zone_start = 0;
	long current_padded_nnz = 0;

	for (long i = 0; i < m; i++) {
		long row_nnz = row_ptr[i+1] - row_ptr[i];
		long padded = nnz_per_thread * ((row_nnz + nnz_per_thread - 1) / nnz_per_thread);
		current_padded_nnz += padded;

		if (current_padded_nnz >= zone_target_nnz || i == m - 1) {
			zones[num_zones].start_row = zone_start;
			zones[num_zones].end_row   = i;
			zones[num_zones].num_rows  = i - zone_start + 1;
			zones[num_zones].score     = 0;
			num_zones++;
			zone_start = i + 1;
			current_padded_nnz = 0;
		}
	}

	*out_zones = zones;
	return num_zones;
}

// Given zones sorted by score (descending), splits zones until (1-ratio) of NNZ
// is assigned to the CPU. Populates row_map with CPU rows first, then GPU rows.
static long split_worst_zones(Zone * zones, long num_zones,
                               INT_T * row_ptr, long m, long total_nnz,
                               double ratio, INT_T * row_map,
                               const char * metric_name)
{
	// CPU gets (1 - ratio) of the workload
	long target_nnz = (long)(total_nnz * (1.0 - ratio));
	long accum_nnz = 0;
	long zones_to_remove = 0;

	// Use a boolean is_cpu array
	char * is_cpu = (char *) malloc(m * sizeof(char));
	for (long i = 0; i < m; i++) is_cpu[i] = 0;

	long m_cpu = 0;
	for (long z = 0; z < num_zones; z++) {
		if (accum_nnz >= target_nnz)
			break;
		long zone_nnz = row_ptr[zones[z].end_row + 1] - row_ptr[zones[z].start_row];
		for (long r = zones[z].start_row; r <= zones[z].end_row; r++) {
			is_cpu[r] = 1;
			m_cpu++;
		}
		accum_nnz += zone_nnz;
		zones_to_remove++;
	}

	printf("   Bad zones (%s): mapping %ld/%ld zones to CPU → %ld rows (%.2f%%), workload: %ld/%ld NNZ (%.2f%%)\n",
	       metric_name, zones_to_remove, num_zones, m_cpu, (double)m_cpu / m * 100.0, accum_nnz, total_nnz, (double)accum_nnz / total_nnz * 100.0);

	long cpu_idx = 0, gpu_idx = 0;
	for (long i = 0; i < m; i++) {
		if (is_cpu[i])
			row_map[cpu_idx++] = i;
		else
			row_map[m_cpu + gpu_idx++] = i;
	}

	free(is_cpu);
	return m_cpu;
}

long get_split_bad_zones_rows(INT_T * row_ptr, long m, long total_nnz, double ratio, INT_T * row_map)
{
	Zone * zones;
	long nnz_per_thread, zone_target_nnz;
	long num_zones = build_zones(row_ptr, m, total_nnz, &zones, &nnz_per_thread, &zone_target_nnz);

	for (long z = 0; z < num_zones; z++)
		zones[z].score = (double)zones[z].num_rows;

	qsort(zones, num_zones, sizeof(Zone), compareZoneScoreDescending);

	long m_cpu = split_worst_zones(zones, num_zones, row_ptr, m, total_nnz, ratio, row_map, "rows");
	free(zones);
	return m_cpu;
}

long get_split_bad_zones_bandwidth(INT_T * row_ptr, INT_T * col_ind, long m, long total_nnz, double ratio, INT_T * row_map)
{
	Zone * zones;
	long nnz_per_thread, zone_target_nnz;
	long num_zones = build_zones(row_ptr, m, total_nnz, &zones, &nnz_per_thread, &zone_target_nnz);

	for (long z = 0; z < num_zones; z++) {
		long min_col = col_ind[row_ptr[zones[z].start_row]];
		long max_col = min_col;
		for (long r = zones[z].start_row; r <= zones[z].end_row; r++) {
			for (long j = row_ptr[r]; j < row_ptr[r+1]; j++) {
				long c = col_ind[j];
				if (c < min_col) min_col = c;
				if (c > max_col) max_col = c;
			}
		}
		zones[z].score = (double)(max_col - min_col);
	}

	qsort(zones, num_zones, sizeof(Zone), compareZoneScoreDescending);

	long m_cpu = split_worst_zones(zones, num_zones, row_ptr, m, total_nnz, ratio, row_map, "bandwidth");
	free(zones);
	return m_cpu;
}

#define CACHELINE_ELEMENTS 16
long get_split_bad_zones_cachelines(INT_T * row_ptr, INT_T * col_ind, long m, long total_nnz, double ratio, INT_T * row_map)
{
	Zone * zones;
	long nnz_per_thread, zone_target_nnz;
	long num_zones = build_zones(row_ptr, m, total_nnz, &zones, &nnz_per_thread, &zone_target_nnz);

	long max_zone_nnz = 0;
	for (long z = 0; z < num_zones; z++) {
		long zone_nnz = row_ptr[zones[z].end_row + 1] - row_ptr[zones[z].start_row];
		if (zone_nnz > max_zone_nnz) max_zone_nnz = zone_nnz;
	}
	long * cl_ids = (long *) malloc(max_zone_nnz * sizeof(long));

	for (long z = 0; z < num_zones; z++) {
		long cnt = 0;
		for (long r = zones[z].start_row; r <= zones[z].end_row; r++) {
			for (long j = row_ptr[r]; j < row_ptr[r+1]; j++) {
				cl_ids[cnt++] = col_ind[j] / CACHELINE_ELEMENTS;
			}
		}
		qsort(cl_ids, cnt, sizeof(long), compareLong);
		long unique = (cnt > 0) ? 1 : 0;
		for (long i = 1; i < cnt; i++) {
			if (cl_ids[i] != cl_ids[i-1])
				unique++;
		}
		zones[z].score = (double)unique;
	}

	free(cl_ids);
	qsort(zones, num_zones, sizeof(Zone), compareZoneScoreDescending);

	long m_cpu = split_worst_zones(zones, num_zones, row_ptr, m, total_nnz, ratio, row_map, "cachelines");
	free(zones);
	return m_cpu;
}

long get_split_bad_zones_padding(INT_T * row_ptr, long m, long total_nnz, double ratio, INT_T * row_map)
{
	Zone * zones;
	long nnz_per_thread, zone_target_nnz;
	long num_zones = build_zones(row_ptr, m, total_nnz, &zones, &nnz_per_thread, &zone_target_nnz);

	for (long z = 0; z < num_zones; z++) {
		long actual = 0;
		long padded = 0;
		for (long r = zones[z].start_row; r <= zones[z].end_row; r++) {
			long row_nnz = row_ptr[r+1] - row_ptr[r];
			actual += row_nnz;
			padded += nnz_per_thread * ((row_nnz + nnz_per_thread - 1) / nnz_per_thread);
		}
		zones[z].score = (actual > 0) ? (double)(padded - actual) / actual : 0.0;
	}

	qsort(zones, num_zones, sizeof(Zone), compareZoneScoreDescending);

	long m_cpu = split_worst_zones(zones, num_zones, row_ptr, m, total_nnz, ratio, row_map, "padding");
	free(zones);
	return m_cpu;
}
