#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "macros/cpp_defines.h"
#include "spmv_kernel.h"
#include "work_removal_strategies.h"

// =============================================================================
// Helper struct & compare functions for sorting rows by NNZ
// (same pattern as partitioning_strategies.cpp)
// =============================================================================

struct RowInfo {
	long id;
	long nnz;
};

static int compareRowInfoAscending(const void * a, const void * b) {
	long diff = ((RowInfo*)a)->nnz - ((RowInfo*)b)->nnz;
	return (diff > 0) ? 1 : (diff < 0) ? -1 : 0;
}

static int compareRowInfoDescending(const void * a, const void * b) {
	long diff = ((RowInfo*)b)->nnz - ((RowInfo*)a)->nnz;
	return (diff > 0) ? 1 : (diff < 0) ? -1 : 0;
}

// =============================================================================
// Work-Removal Strategy Implementations
//
// Convention: each function populates row_map such that
//   row_map[0 .. m_gpu-1]   = rows that STAY on GPU
//   row_map[m_gpu .. m-1]   = rows that are REMOVED
// and returns m_removed = m - m_gpu.
// =============================================================================


// --- Remove the X% shortest workload (fewest NNZ) ---
// Sort ascending; remove the shortest rows until they sum to X% of workload (nonzeros).
// GPU keeps the remaining (longest) rows, in original matrix order.
long get_removal_shortest_rows(INT_T * row_ptr, long m, long total_nnz,
                               double ratio, INT_T * row_map)
{
	long target_nnz = (long)(total_nnz * ratio);
	long accum_nnz = 0;
	long m_removed = 0;

	RowInfo * rows = (RowInfo*) malloc(m * sizeof(RowInfo));
	for (long i = 0; i < m; i++) {
		rows[i].id = i;
		rows[i].nnz = row_ptr[i+1] - row_ptr[i];
	}
	// Sort ascending by NNZ
	qsort(rows, m, sizeof(RowInfo), compareRowInfoAscending);

	// Mark the shortest rows for removal until we reach the target NNZ
	char * keep = (char *) malloc(m * sizeof(char));
	for (long i = 0; i < m; i++) keep[i] = 1;
	for (long i = 0; i < m; i++) {
		if (accum_nnz >= target_nnz) {
			break;
		}
		keep[rows[i].id] = 0;
		accum_nnz += rows[i].nnz;
		m_removed++;
	}

	long m_gpu = m - m_removed;

	// Populate row_map in original matrix order: GPU rows first, then removed
	long gpu_idx = 0, rem_idx = 0;
	for (long i = 0; i < m; i++) {
		if (keep[i])
			row_map[gpu_idx++] = i;
		else
			row_map[m_gpu + rem_idx++] = i;
	}

	free(keep);
	free(rows);
	return m_removed;
}


// --- Remove the X% longest workload (most NNZ) ---
// Sort descending; remove the longest rows until they sum to X% of workload (nonzeros).
// GPU keeps the remaining (shortest) rows, in original matrix order.
long get_removal_longest_rows(INT_T * row_ptr, long m, long total_nnz,
                              double ratio, INT_T * row_map)
{
	long target_nnz = (long)(total_nnz * ratio);
	long accum_nnz = 0;
	long m_removed = 0;

	RowInfo * rows = (RowInfo*) malloc(m * sizeof(RowInfo));
	for (long i = 0; i < m; i++) {
		rows[i].id = i;
		rows[i].nnz = row_ptr[i+1] - row_ptr[i];
	}
	// Sort descending by NNZ
	qsort(rows, m, sizeof(RowInfo), compareRowInfoDescending);

	// Mark the longest rows for removal until we reach the target NNZ
	char * keep = (char *) malloc(m * sizeof(char));
	for (long i = 0; i < m; i++) keep[i] = 1;
	for (long i = 0; i < m; i++) {
		if (accum_nnz >= target_nnz) {
			break;
		}
		keep[rows[i].id] = 0;
		accum_nnz += rows[i].nnz;
		m_removed++;
	}

	long m_gpu = m - m_removed;

	// Populate row_map in original matrix order: GPU rows first, then removed
	long gpu_idx = 0, rem_idx = 0;
	for (long i = 0; i < m; i++) {
		if (keep[i])
			row_map[gpu_idx++] = i;
		else
			row_map[m_gpu + rem_idx++] = i;
	}

	free(keep);
	free(rows);
	return m_removed;
}


// --- Remove all rows with NNZ < threshold ---
// GPU keeps rows with NNZ >= threshold.
long get_removal_below_threshold(INT_T * row_ptr, long m,
                                 long threshold, INT_T * row_map)
{
	long m_gpu = 0;
	long m_removed = 0;

	// Two-pass: first count, then fill row_map
	for (long i = 0; i < m; i++) {
		long nnz_i = row_ptr[i+1] - row_ptr[i];
		if (nnz_i >= threshold)
			m_gpu++;
		else
			m_removed++;
	}

	// Fill row_map: GPU rows first, then removed rows
	long gpu_idx = 0, rem_idx = 0;
	for (long i = 0; i < m; i++) {
		long nnz_i = row_ptr[i+1] - row_ptr[i];
		if (nnz_i >= threshold)
			row_map[gpu_idx++] = i;
		else
			row_map[m_gpu + rem_idx++] = i;
	}

	return m_removed;
}


// --- Remove all rows with NNZ > threshold ---
// GPU keeps rows with NNZ <= threshold.
long get_removal_above_threshold(INT_T * row_ptr, long m,
                                 long threshold, INT_T * row_map)
{
	long m_gpu = 0;
	long m_removed = 0;

	for (long i = 0; i < m; i++) {
		long nnz_i = row_ptr[i+1] - row_ptr[i];
		if (nnz_i <= threshold)
			m_gpu++;
		else
			m_removed++;
	}

	long gpu_idx = 0, rem_idx = 0;
	for (long i = 0; i < m; i++) {
		long nnz_i = row_ptr[i+1] - row_ptr[i];
		if (nnz_i <= threshold)
			row_map[gpu_idx++] = i;
		else
			row_map[m_gpu + rem_idx++] = i;
	}

	return m_removed;
}


// --- Remove outlier rows (NNZ > mean + k_sigma * stddev) ---
long get_removal_outlier_rows(INT_T * row_ptr, long m,
                              double k_sigma, INT_T * row_map)
{
	// Compute mean and stddev of row NNZs
	double sum = 0;
	double sum_sq = 0;
	for (long i = 0; i < m; i++) {
		double nnz_i = (double)(row_ptr[i+1] - row_ptr[i]);
		sum += nnz_i;
		sum_sq += nnz_i * nnz_i;
	}
	double mean = sum / m;
	double variance = (sum_sq / m) - (mean * mean);
	double stddev = sqrt(variance > 0 ? variance : 0);
	double threshold = mean + k_sigma * stddev;

	printf("   Outlier detection: mean=%.2f, stddev=%.2f, threshold (k=%.1f)=%.2f\n",
	       mean, stddev, k_sigma, threshold);

	long m_gpu = 0;
	long m_removed = 0;

	for (long i = 0; i < m; i++) {
		long nnz_i = row_ptr[i+1] - row_ptr[i];
		if ((double)nnz_i <= threshold)
			m_gpu++;
		else
			m_removed++;
	}

	long gpu_idx = 0, rem_idx = 0;
	for (long i = 0; i < m; i++) {
		long nnz_i = row_ptr[i+1] - row_ptr[i];
		if ((double)nnz_i <= threshold)
			row_map[gpu_idx++] = i;
		else
			row_map[m_gpu + rem_idx++] = i;
	}

	return m_removed;
}


// --- Remove a contiguous block of X% workload (NNZs) from the beginning ---
// GPU keeps the remaining rows at the end.
long get_removal_contiguous_block(INT_T * row_ptr, long m, long total_nnz,
                                  double ratio, INT_T * row_map)
{
	long target_nnz = (long)(total_nnz * ratio);
	long accum_nnz = 0;
	long m_removed = 0;

	for (long i = 0; i < m; i++) {
		if (accum_nnz >= target_nnz) {
			break;
		}
		accum_nnz += row_ptr[i+1] - row_ptr[i];
		m_removed++;
	}

	long m_gpu = m - m_removed;

	// GPU rows: rows [m_removed .. m-1] (the tail of the matrix)
	for (long i = 0; i < m_gpu; i++)
		row_map[i] = m_removed + i;

	// Removed rows: rows [0 .. m_removed-1] (the head of the matrix)
	for (long i = 0; i < m_removed; i++)
		row_map[m_gpu + i] = i;

	return m_removed;
}


// --- Remove the X% worst zones (GPU thread-block aware) ---
// A "zone" emulates a GPU thread block: contiguous rows whose combined
// padded NNZ totals ~ZONE_TARGET_NNZ (= NNZ_PER_THREAD * BLOCK_SIZE = 640).
//
// Each row's NNZ is padded to the next multiple of NNZ_PER_THREAD (=5),
// exactly matching cuda_csr_transpose_expand_rows.cu's expand logic.
//
// Score of a zone = num_rows it spans.  High row count = sparse/divergent = bad.

#define ZONE_NNZ_PER_THREAD 5
#define ZONE_BLOCK_SIZE     128
#define ZONE_TARGET_NNZ     (ZONE_NNZ_PER_THREAD * ZONE_BLOCK_SIZE)  // 640

struct Zone {
	long start_row;
	long end_row;
	long num_rows;
	double score;    // generic score: higher = worse
};

static int compareZoneScoreDescending(const void * a, const void * b) {
	double diff = ((Zone*)b)->score - ((Zone*)a)->score;
	return (diff > 0) ? 1 : (diff < 0) ? -1 : 0;
}

// Helper: build zones from row_ptr using GPU padded NNZ logic.
// Returns number of zones created.  Caller must free zones.
static long build_zones(INT_T * row_ptr, long m, Zone ** out_zones)
{
	Zone * zones = (Zone *) malloc(m * sizeof(Zone));
	long num_zones = 0;
	long zone_start = 0;
	long current_padded_nnz = 0;

	for (long i = 0; i < m; i++) {
		long row_nnz = row_ptr[i+1] - row_ptr[i];
		// Emulate GPU zero-padding: round up to next multiple of NNZ_PER_THREAD
		long padded = ZONE_NNZ_PER_THREAD * ((row_nnz + ZONE_NNZ_PER_THREAD - 1) / ZONE_NNZ_PER_THREAD);
		current_padded_nnz += padded;

		if (current_padded_nnz >= ZONE_TARGET_NNZ || i == m - 1) {
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

// Helper: given zones sorted by score (descending), remove zones until X% of NNZ
// is removed.  Populates row_map in original order.  Returns m_removed.
static long remove_worst_zones(Zone * zones, long num_zones,
                               INT_T * row_ptr, long m, long total_nnz,
                               double ratio, INT_T * row_map,
                               const char * metric_name)
{
	long target_nnz = (long)(total_nnz * ratio);
	long accum_nnz = 0;
	long zones_to_remove = 0;

	// Use a boolean keep array
	char * keep = (char *) malloc(m * sizeof(char));
	for (long i = 0; i < m; i++) keep[i] = 1;

	long m_removed = 0;
	for (long z = 0; z < num_zones; z++) {
		if (accum_nnz >= target_nnz)
			break;
		long zone_nnz = row_ptr[zones[z].end_row + 1] - row_ptr[zones[z].start_row];
		for (long r = zones[z].start_row; r <= zones[z].end_row; r++) {
			keep[r] = 0;
			m_removed++;
		}
		accum_nnz += zone_nnz;
		zones_to_remove++;
	}

	printf("   Bad zones (%s): removing %ld/%ld zones → %ld rows removed (%.2f%%), workload removed: %ld/%ld NNZ (%.2f%%)\n",
	       metric_name, zones_to_remove, num_zones, m_removed, (double)m_removed / m * 100.0, accum_nnz, total_nnz, (double)accum_nnz / total_nnz * 100.0);

	long gpu_idx = 0, rem_idx = 0;
	long m_gpu = m - m_removed;
	for (long i = 0; i < m; i++) {
		if (keep[i])
			row_map[gpu_idx++] = i;
		else
			row_map[m_gpu + rem_idx++] = i;
	}

	free(keep);
	return m_removed;
}


// --- Bad zones scored by ROW COUNT ---
long get_removal_bad_zones_rows(INT_T * row_ptr, long m, long total_nnz, double ratio, INT_T * row_map)
{
	Zone * zones;
	long num_zones = build_zones(row_ptr, m, &zones);

	printf("   Bad zones (rows): %ld zones identified (target padded NNZ/zone = %d)\n", num_zones, ZONE_TARGET_NNZ);

	// Score = num_rows (higher = more sparse = worse)
	for (long z = 0; z < num_zones; z++)
		zones[z].score = (double)zones[z].num_rows;

	qsort(zones, num_zones, sizeof(Zone), compareZoneScoreDescending);

	long m_removed = remove_worst_zones(zones, num_zones, row_ptr, m, total_nnz, ratio, row_map, "rows");
	free(zones);
	return m_removed;
}


// --- Bad zones scored by BANDWIDTH (max_col - min_col) ---
long get_removal_bad_zones_bandwidth(INT_T * row_ptr, INT_T * col_ind, long m, long total_nnz, double ratio, INT_T * row_map)
{
	Zone * zones;
	long num_zones = build_zones(row_ptr, m, &zones);

	printf("   Bad zones (bandwidth): %ld zones identified (target padded NNZ/zone = %d)\n", num_zones, ZONE_TARGET_NNZ);

	// Score = max_col - min_col across all NNZ in the zone
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

	long m_removed = remove_worst_zones(zones, num_zones, row_ptr, m, total_nnz, ratio, row_map, "bandwidth");
	free(zones);
	return m_removed;
}


// --- Bad zones scored by UNIQUE CACHE LINES touched in x ---
// GPU L2 cache line = 128 bytes; for double (8 bytes) → 16 elements per cache line.
#define CACHELINE_ELEMENTS 16

static int compareLong(const void * a, const void * b) {
	long diff = *(const long *)a - *(const long *)b;
	return (diff > 0) ? 1 : (diff < 0) ? -1 : 0;
}

long get_removal_bad_zones_cachelines(INT_T * row_ptr, INT_T * col_ind, long m, long total_nnz, double ratio, INT_T * row_map)
{
	Zone * zones;
	long num_zones = build_zones(row_ptr, m, &zones);

	printf("   Bad zones (cachelines): %ld zones identified (target padded NNZ/zone = %d)\n", num_zones, ZONE_TARGET_NNZ);

	// Temporary buffer for cache line IDs within a zone
	// Max NNZ in a zone is bounded; use a generous buffer
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
		// Sort and count unique
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

	long m_removed = remove_worst_zones(zones, num_zones, row_ptr, m, total_nnz, ratio, row_map, "cachelines");
	free(zones);
	return m_removed;
}


// --- Bad zones scored by ZERO-PADDING RATIO ---
long get_removal_bad_zones_padding(INT_T * row_ptr, long m, long total_nnz, double ratio, INT_T * row_map)
{
	Zone * zones;
	long num_zones = build_zones(row_ptr, m, &zones);

	printf("   Bad zones (padding): %ld zones identified (target padded NNZ/zone = %d)\n", num_zones, ZONE_TARGET_NNZ);

	// Score = (padded_nnz - actual_nnz) / padded_nnz  (higher = more waste)
	for (long z = 0; z < num_zones; z++) {
		long actual = 0;
		long padded = 0;
		for (long r = zones[z].start_row; r <= zones[z].end_row; r++) {
			long row_nnz = row_ptr[r+1] - row_ptr[r];
			actual += row_nnz;
			padded += ZONE_NNZ_PER_THREAD * ((row_nnz + ZONE_NNZ_PER_THREAD - 1) / ZONE_NNZ_PER_THREAD);
		}
		zones[z].score = (actual > 0) ? (double)(padded - actual) / actual : 0.0;
	}

	qsort(zones, num_zones, sizeof(Zone), compareZoneScoreDescending);

	long m_removed = remove_worst_zones(zones, num_zones, row_ptr, m, total_nnz, ratio, row_map, "padding");
	free(zones);
	return m_removed;
}
