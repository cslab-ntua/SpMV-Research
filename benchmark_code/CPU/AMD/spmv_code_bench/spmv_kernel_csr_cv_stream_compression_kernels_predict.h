#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <float.h>
#include <string.h>
#include <assert.h>
#include <omp.h>
#include <x86intrin.h>

#include "macros/cpp_defines.h"

#ifdef __cplusplus
extern "C"{
#endif
	#include "bit_ops.h"
	#include "bitstream.h"
	#include "bytestream.h"

	#include "spmv_kernel_csr_cv_stream_gather_coords.h"
#ifdef __cplusplus
}
#endif

#define MAX_BLOCK_SIZE  64
// #define MAX_BLOCK_SIZE  128
// #define MAX_BLOCK_SIZE  256


static inline
double
d2d(void * A, long i)
{
	return ((double *) A)[i];
}


static inline
double
d2fabsd(void * A, long i)
{
	return fabs(((double *) A)[i]);
}


//==========================================================================================================================================
//= Sorted Delta
//==========================================================================================================================================


static uint64_t window_size_bits;
static uint64_t window_size;
static double ** t_vals;
static double ** t_rows;
static double ** t_window;
static unsigned char *** t_data_lanes;
static int ** t_permutation_orig;
static int ** t_permutation;
static int ** t_rev_permutation;
static unsigned int ** t_rows_diff;
static unsigned int ** t_cols;
static int ** t_qsort_partitions;
static double ** t_y_buf;

uint64_t * t_row_bits_accum;
uint64_t * t_col_bits_accum;
uint64_t * t_row_col_bytes_accum;


//------------------------------------------------------------------------------------------------------------------------------------------
//- Quicksort
//------------------------------------------------------------------------------------------------------------------------------------------

#ifdef __cplusplus
extern "C"{
#endif
	#include "sort/quicksort/quicksort_gen_undef.h"
	#define QUICKSORT_GEN_TYPE_1  int
	#define QUICKSORT_GEN_TYPE_2  int
	#define QUICKSORT_GEN_TYPE_3  double
	#define QUICKSORT_GEN_SUFFIX  i_i_d
	#include "sort/quicksort/quicksort_gen.c"
#ifdef __cplusplus
}
#endif

// static inline
// int
// quicksort_cmp(int a, int b, double * vals)
// {
	// return (vals[a] > vals[b]) ? 1 : (vals[a] < vals[b]) ? -1 : 0;
// }

// static inline
// int
// quicksort_cmp(int a, int b, double * vals)
// {
	// double va=vals[a], vb=vals[b];
	// if (va*vb < 0)
		// return va > 0 ? va : vb;
	// return (fabs(va) > fabs(vb)) ? 1 : (fabs(va) < fabs(vb)) ? -1 : 0;
// }

static inline
int
quicksort_cmp(int a, int b, double * vals)
{
	double va=vals[a], vb=vals[b];
	if (va*vb <= 0)
		return (va > vb) ? 1 : (va < vb) ? -1 : 0;
	return (fabs(va) > fabs(vb)) ? 1 : (fabs(va) < fabs(vb)) ? -1 : 0;
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//------------------------------------------------------------------------------------------------------------------------------------------
//-                                                             Compression                                                                -
//------------------------------------------------------------------------------------------------------------------------------------------
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


void
compress_init_sort_diff(__attribute__((unused)) ValueType * vals, __attribute__((unused)) const long num_vals, long packet_size)
{
	int num_threads = omp_get_max_threads();
	packet_size = packet_size + packet_size % 4;

	bits_u64_required_bits_for_binary_representation(packet_size - 1, &window_size_bits, &window_size);    // Maximum representable number we want is 'packet_size - 1'.
	printf("window_size = %ld , window_size_bits = %ld\n", window_size, window_size_bits);
	t_vals = (typeof(t_vals)) malloc(num_threads * sizeof(*t_vals));
	t_rows = (typeof(t_rows)) malloc(num_threads * sizeof(*t_rows));
	t_window = (typeof(t_window)) malloc(num_threads * sizeof(*t_window));
	t_data_lanes = (typeof(t_data_lanes)) malloc(num_threads * sizeof(*t_data_lanes));
	t_permutation_orig = (typeof(t_permutation_orig)) malloc(num_threads * sizeof(*t_permutation_orig));
	t_permutation = (typeof(t_permutation)) malloc(num_threads * sizeof(*t_permutation));
	t_rev_permutation = (typeof(t_rev_permutation)) malloc(num_threads * sizeof(*t_rev_permutation));
	t_rows_diff = (typeof(t_rows_diff)) malloc(num_threads * sizeof(*t_rows_diff));
	t_cols = (typeof(t_cols)) malloc(num_threads * sizeof(*t_cols));
	t_qsort_partitions = (typeof(t_qsort_partitions)) malloc(num_threads * sizeof(*t_qsort_partitions));
	t_row_bits_accum = (typeof(t_row_bits_accum)) malloc(num_threads * sizeof(*t_row_bits_accum));
	t_col_bits_accum = (typeof(t_col_bits_accum)) malloc(num_threads * sizeof(*t_col_bits_accum));
	t_row_col_bytes_accum = (typeof(t_row_col_bytes_accum)) malloc(num_threads * sizeof(*t_row_col_bytes_accum));
	t_y_buf = (typeof(t_y_buf)) malloc(num_threads * sizeof(*t_y_buf));
	#pragma omp parallel
	{
		int tnum = omp_get_thread_num();
		long i;
		t_vals[tnum] = (typeof(&(**t_vals))) malloc(window_size * sizeof(**t_vals));
		t_rows[tnum] = (typeof(&(**t_rows))) malloc(window_size * sizeof(**t_rows));
		t_window[tnum] = (typeof(&(**t_window))) malloc(window_size * sizeof(**t_window));
		t_data_lanes[tnum] = (typeof(&(**t_data_lanes))) malloc(4 * sizeof(**t_data_lanes));
		for (i=0;i<4;i++)
		{
			t_data_lanes[tnum][i] = (typeof(&(**t_data_lanes[i]))) malloc(8*window_size * sizeof(**t_data_lanes[i]));
		}
		t_permutation_orig[tnum] = (typeof(&(**t_permutation_orig))) malloc(window_size * sizeof(**t_permutation_orig));
		t_permutation[tnum] = (typeof(&(**t_permutation))) malloc(window_size * sizeof(**t_permutation));
		t_rev_permutation[tnum] = (typeof(&(**t_rev_permutation))) malloc(window_size * sizeof(**t_rev_permutation));
		t_rows_diff[tnum] = (typeof(&(**t_rows_diff))) malloc(window_size * sizeof(**t_rows_diff));
		t_cols[tnum] = (typeof(&(**t_cols))) malloc(window_size * sizeof(**t_cols));
		t_qsort_partitions[tnum] = (typeof(&(**t_qsort_partitions))) malloc(window_size * sizeof(**t_qsort_partitions));
		t_row_bits_accum[tnum] = 0;
		t_col_bits_accum[tnum] = 0;
		t_row_col_bytes_accum[tnum] = 0;
		t_y_buf[tnum] = (typeof(&(**t_y_buf))) malloc(4 * window_size * sizeof(**t_y_buf));
		for (i=0;i<(long)(4*window_size);i++)
		{
			t_y_buf[tnum][i] = 0;
		}
	}
}


static inline
long
select_num_vals(INT_T * row_ptr, long i_s, long j_s, long num_vals, long force_row_index_lte_1_byte)
{
	long i_e, j_e;
	long num_rows;
	long num_rows_max = 255;
	long num_vals_ret;
	long num_vals_min = 64;
	j_e = j_s + num_vals;
	i_e = i_s;
	while (row_ptr[i_e] < j_e)
		i_e++;
	num_rows = i_e - i_s;
	if (num_rows > num_rows_max)
		i_e = i_s + num_rows_max;
	num_vals_ret = row_ptr[i_e] - j_s;
	if (!force_row_index_lte_1_byte)
	{
		if (num_vals_ret < num_vals_min)
			num_vals_ret = num_vals_min;
	}
	if (num_vals_ret > num_vals)
		num_vals_ret = num_vals;
	return num_vals_ret;
}


static inline
uint64_t
compress_diff_lossy(double val_d, double val_prev_d, double diff_d, double tolerance)
{
	uint64_t trailing_zeros;
	uint64_t pow2, mask_inv;
	union {
		double d;
		uint64_t u;
	} val, val_prev, diff, val_new;
	double error;
	long i;
	val.d = val_d;
	val_prev.d = val_prev_d;
	diff.d = diff_d;
	if (tolerance == 0)
		return diff.u;
	if (val.u == 0)
		return diff.u;
	trailing_zeros = (diff.u == 0) ? 64 : __builtin_ctzl(diff.u);
	if (trailing_zeros >= 53)  // Only the exponent remains.
		return diff.u;
	pow2 = 2;
	mask_inv = 1;
	for (i=1;i<54;i++)
	{
		diff.u &= ~mask_inv;
		val_new.u = val_prev.u + diff.u;
		error = fabs(val.d - val_new.d) / fabs(val.d);
		if (error > tolerance)
			break;
		pow2 <<= 1;
		mask_inv = pow2 - 1;
	}
	mask_inv >>= 1;
	diff.d = diff_d;
	diff.u &= ~mask_inv;
	return diff.u;
}


/*
 * 'i_s' is certain to be the first non-empty row.
 */
static inline
long
compress_kernel_sort_diff(INT_T * row_ptr, INT_T * ja, ValueType * vals_unpadded, long i_s, long j_s, unsigned char * buf, long num_vals, long * num_vals_out)
{
	int tnum = omp_get_thread_num();
	ValueType * vals = t_vals[tnum];
	ValueType * rows = t_rows[tnum];
	double * window = t_window[tnum];
	unsigned char ** data_val_lanes = t_data_lanes[tnum];
	int * data_val_lanes_len;
	unsigned int * rows_diff = t_rows_diff[tnum];
	unsigned int * cols = t_cols[tnum];
	int * permutation_orig = t_permutation_orig[tnum];
	int * permutation = t_permutation[tnum];
	int * rev_permutation = t_rev_permutation[tnum];
	struct Byte_Stream Bs[4];
	uint64_t leading_sign_bits = 0;
	uint64_t trailing_zeros = 0;
	uint64_t trailing_zero_bits_div4 = 0;
	uint64_t len_bits = 0;
	uint64_t len = 0;
	union {
		double d;
		uint64_t u;
	} diff;
	uint64_t row_min, row_max, row_diff = 0, row_diff_max = 0, col = 0, col_min = 0, col_max = 0, col_diff = 0, col_diff_max = 0;   // row_max is the last row (i.e., inclusive: num_rows = row_max + 1 - i_s)
	uint64_t row_bits = 0, col_bits = 0;
	uint64_t row_col_bytes = 0;
	long i, j, k, l;

	double tolerance = atof(getenv("VC_TOLERANCE"));

	long force_row_index_lte_1_byte = 1;
	long force_row_index_rounding = 0;

	row_min = i_s;   // 'i_s' is certain to be the first non-empty row.

	num_vals = select_num_vals(row_ptr, row_min, j_s, num_vals, force_row_index_lte_1_byte);

	long div_padded = (num_vals + 3) / 4;
	long num_vals_padded = div_padded * 4;

	for (k=0,i=row_min,j=j_s; k<num_vals; k++,j++)
	{
		if (j >= row_ptr[i+1])
			i++;
		vals[k] = vals_unpadded[k];
		rows[k] = i;
	}
	row_max = i;
	for (;k<num_vals_padded;k++)
		vals[k] = 0;

	for (k=0;k<num_vals_padded;k++)
		permutation_orig[k] = k;
	quicksort(permutation_orig, num_vals_padded, vals, t_qsort_partitions[tnum]);
	for (k=0;k<num_vals_padded;k++)
	{
		l = (k % 4) * div_padded + k / 4;   // interleave index
		permutation[k] = permutation_orig[l];
	}
	for (k=0;k<num_vals_padded;k++)
		rev_permutation[permutation[k]] = k;
	col_min = ja[j_s];
	col_max = ja[j_s];
	for (k=0,j=j_s;k<num_vals;k++,j++)
	{
		window[rev_permutation[k]] = vals[k];
		rows_diff[rev_permutation[k]] = rows[k] - row_min;
		col = ja[j];
		cols[rev_permutation[k]] = col;
		if (col < col_min)
			col_min = col;
		if (col > col_max)
			col_max = col;
	}
	for (;k<num_vals_padded;k++)   // fake nnz in the last row
	{
		window[rev_permutation[k]] = 0;   // zero values
		rows_diff[rev_permutation[k]] = row_max - row_min;   // in the last row
		cols[rev_permutation[k]] = col_min;   // use col_min so that we don't change col_diff_max
	}
	row_diff_max = row_max - row_min;
	bits_u64_required_bits_for_binary_representation(row_diff_max, &row_bits, NULL);
	if (force_row_index_lte_1_byte)
	{
		if (row_bits > 8)
			error("row_bits > 8 (%ld)", row_bits);
	}
	col_diff_max = col_max - col_min;
	bits_u64_required_bits_for_binary_representation(col_diff_max, &col_bits, NULL);
	row_col_bytes = (row_bits + col_bits + 7) >> 3;

	/* Optimize row and col bits to bytes if possible.
	 * It should not increase 'row_col_bytes', unless 'force_row_index_rounding' is set.
	 */
	if (row_bits % 8)
	{
		uint64_t cmpl = 8 - (row_bits % 8);
		uint64_t row_col_bytes_new = ((row_bits + cmpl) + col_bits + 7) >> 3;
		if (!force_row_index_rounding)
		{
			if (row_col_bytes_new == row_col_bytes)
				row_bits += cmpl;
		}
		else
		{
			row_bits += cmpl;
			row_col_bytes = row_col_bytes_new;
		}
	}
	col_bits = (row_col_bytes << 3) - row_bits;
	if (col_bits > 32)
		error("col_bits > 32 (%ld)", col_bits);

	t_row_bits_accum[tnum] += row_bits;
	t_col_bits_accum[tnum] += col_bits;
	t_row_col_bytes_accum[tnum] += row_col_bytes;

	for (k=0;k<num_vals_padded;k++)
	{
		if (window[k] != vals[permutation[k]])
			error("wrong");
		if (rows_diff[k] + row_min > row_max)
			error("row index");
		if (permutation[k] >= num_vals) // extra fake values
			continue;
		if (cols[k] != (unsigned int) ja[j_s+permutation[k]])
			error("col index");
	}
	// if (tnum == 0) fprintf(stderr, "i=[%ld, %ld], j_s=[%ld, %ld], row_diff_max=%ld , row_bits=%ld , col_diff_max=%ld , col_bits=%ld\n", row_min, row_max, j_s, j, row_diff_max, row_bits, col_diff_max, col_bits);

	unsigned char * data_intro = buf;
	uint64_t data_intro_bytes = 0;

	// Should always be the first data in the packet.
	data_intro[data_intro_bytes] = row_bits;
	data_intro_bytes += 1;
	data_intro[data_intro_bytes] = col_bits;
	data_intro_bytes += 1;

	*((uint32_t *) &data_intro[data_intro_bytes]) = num_vals;
	data_intro_bytes += 4;
	*((uint32_t *) &data_intro[data_intro_bytes]) = row_diff_max + 1;   // Number of rows.
	data_intro_bytes += 4;

	*((uint32_t *) &data_intro[data_intro_bytes]) = row_min;
	data_intro_bytes += 4;
	*((uint32_t *) &data_intro[data_intro_bytes]) = col_min;
	data_intro_bytes += 4;

	*((double *) &data_intro[data_intro_bytes]) = window[0];
	data_intro_bytes += 8;
	*((double *) &data_intro[data_intro_bytes]) = window[1];
	data_intro_bytes += 8;
	*((double *) &data_intro[data_intro_bytes]) = window[2];
	data_intro_bytes += 8;
	*((double *) &data_intro[data_intro_bytes]) = window[3];
	data_intro_bytes += 8;

	data_val_lanes_len = (typeof(data_val_lanes_len)) &(data_intro[data_intro_bytes]);
	data_intro_bytes += 4 * sizeof(*data_val_lanes_len);

	unsigned char * data_coords = &data_intro[data_intro_bytes];
	const uint64_t coords_bytes = row_col_bytes;
	const uint64_t data_coords_bytes = coords_bytes * num_vals_padded;

	unsigned char * data_val_lens = &((unsigned char *) data_coords)[data_coords_bytes];
	const uint64_t data_val_lens_bytes = num_vals_padded;

	for (i=0;i<num_vals_padded;i++)
	{
		uint64_t coords;
		row_diff = rows_diff[i];
		col_diff = cols[i] - col_min;
		coords = row_diff | (col_diff << row_bits);
		*((uint64_t *) &data_coords[i*coords_bytes]) = coords;
	}

	bytestream_init_write(&Bs[0], data_val_lanes[0]);
	bytestream_init_write(&Bs[1], data_val_lanes[1]);
	bytestream_init_write(&Bs[2], data_val_lanes[2]);
	bytestream_init_write(&Bs[3], data_val_lanes[3]);

	union {
		double d;
		uint64_t u;
	} val_prev[4], val, val_predicted, lane_run_avg[4];
	val_prev[0].d = window[0];
	val_prev[1].d = window[1];
	val_prev[2].d = window[2];
	val_prev[3].d = window[3];
	// lane_run_avg[0].d = lane_run_avg[1].d = lane_run_avg[2].d = lane_run_avg[3].d = 1.0;
	lane_run_avg[0].d = lane_run_avg[1].d = lane_run_avg[2].d = lane_run_avg[3].d = 0.0;
	for (i=4;i<num_vals_padded;i++)
	{
		val.d = window[i];

		// val_predicted.d = val_prev[i%4].d;
		// val_predicted.d = lane_run_avg[i%4].d * val_prev[i%4].d;
		// val_predicted.d = lane_run_avg[i%4].d + val_prev[i%4].d;
		// val_predicted.u = lane_run_avg[i%4].u + val_prev[i%4].u;
		val_predicted.d = lane_run_avg[i%4].d * val_prev[i%4].d;

		// lane_run_avg[i%4].d = (lane_run_avg[i%4].d + val.d - val_prev[i%4].d) / 2;
		// lane_run_avg[i%4].d = val.d - val_prev[i%4].d;
		// lane_run_avg[i%4].u = val.u - val_prev[i%4].u;
		// lane_run_avg[i%4].u = (lane_run_avg[i%4].u + val.u - val_prev[i%4].u) / 2;
		lane_run_avg[i%4].d = val.d / val_prev[i%4].d;
		// lane_run_avg[i%4].d = (lane_run_avg[i%4].d + val.d / val_prev[i%4].d) / 2;

		// lane_run_avg[i%4].u = lane_run_avg[i%4].u & (-(1ULL << 50));

		diff.u = val.u - val_predicted.u;

		if (tolerance > 0)
		{
			diff.u = compress_diff_lossy(window[i], val_predicted.d, diff.d, tolerance);
		}

		/* Instead of setting to 'window[i]', we emulate the procedure of the decompression.
		 * This accounts for the accumulation of the error that is being passed down.
		 *
		 * Note:
		 *     Maybe check if this is optimized out (e.g. -ffast-math, but if user has such flags, it doesn't really make a difference).
		 */
		val_prev[i%4].u = val_predicted.u + diff.u;
		double error = fabs(val.d - val_prev[i%4].d) / fabs(val.d);
		if (error > tolerance)
			error("error > tolerance: %lf > %lf", error, tolerance);

		if (diff.u == 0)
		{
			len = 0;
			data_val_lens[i] = 0;
		}
		else
		{
			/* For the length values (0-8) 4 bits are needed.
			 *
			 * For the trailing zero bits values (0-63, 64 isn't needed) 6 bits are needed, but we only have 4 bits space remaining.
			 * so we keep shifts in strides of 4 (0, 4, 8, 12, ..., 60).
			 */
			uint64_t rem_tz;
			uint64_t buf_diff = diff.u;
			leading_sign_bits = __builtin_clrsbl(diff.u);   // Number of leading redundant sign bits, i.e., without counting the first sign bit.
			trailing_zeros = __builtin_ctzl(diff.u);
			rem_tz = trailing_zeros & 3ULL;

			len_bits = 64ULL - leading_sign_bits - trailing_zeros;
			len = (len_bits + 7ULL) >> 3ULL;
			uint64_t rem = (len << 3ULL) - len_bits;

			if (rem >= rem_tz)
			{
				rem -= rem_tz;
				trailing_zeros -= rem_tz;
			}

			if (rem <= leading_sign_bits)
			{
				leading_sign_bits -= rem;
				rem = 0;
			}
			else
			{
				rem -= leading_sign_bits;
				leading_sign_bits = 0;
			}
			trailing_zeros -= rem;

			trailing_zero_bits_div4 = trailing_zeros >> 2ULL;
			trailing_zeros = trailing_zero_bits_div4 << 2ULL;

			len_bits = 64ULL - leading_sign_bits - trailing_zeros;
			len = (len_bits + 7ULL) >> 3ULL;
			if (len > 8)
				error("len > 8");

			diff.u = ((int64_t) diff.u) >> trailing_zeros;   // Arithmetic shift right.
			bytestream_write_unsafe_cast(&Bs[i%4], diff.u, len);
			len |=  (trailing_zero_bits_div4 << 4ULL);
			data_val_lens[i] = len;

			// Test.
			uint64_t tz = (len >> 2ULL) & (~3ULL);
			if (tz != trailing_zeros)
				error("tz: %ld %ld %ld", len, tz, trailing_zero_bits_div4);
			len &= 15ULL;
			len_bits = len << 3ULL;

			// uint64_t mask = (1ULL << (len << 3ULL)) - 1ULL;
			// diff.u &= mask;
			// diff.u <<= tz;
			// if (diff.u != buf_diff)
				// error("test");

			uint64_t pow2 = (1ULL << len_bits);
			if (len_bits == 64)
				pow2 = 0;
			uint64_t msb = pow2 >> 1ULL;
			uint64_t sign = ~((diff.u & msb) - 1ULL);
			uint64_t mask = pow2 - 1ULL;
			// diff.u &= mask;
			// diff.u = diff.u | sign;
			// diff.u <<= tz;

			{
				const __m256i one = _mm256_set1_epi64x(1ULL);
				union {
					__m256d d;
					__m256i u;
				} v_diff;
				__m256i v_sign;
				v_diff.u = _mm256_set1_epi64x(diff.u);
				__m256i v_len_bits;
				v_len_bits = _mm256_set1_epi64x(len_bits);
				__m256i v_shift;
				v_shift = _mm256_set1_epi64x(tz);


				// __m256i v_pow2 = (1ULL << v_len_bits);
				__m256i v_pow2 = _mm256_sllv_epi64(one, v_len_bits);

				// __m256i v_msb = v_pow2 >> 1ULL;
				__m256i v_msb = _mm256_srli_epi64(v_pow2, 1ULL);
				// v_sign = ~((v_diff.u & v_msb) - 1ULL);
				v_sign = _mm256_and_si256(v_diff.u, v_msb);
				v_sign = _mm256_sub_epi64(v_sign, one);
				v_sign = _mm256_xor_si256(v_sign, _mm256_cmpeq_epi32(v_sign, v_sign));

				// __m256i v_mask = v_pow2 - 1ULL;
				__m256i v_mask = _mm256_sub_epi64(v_pow2, one);
				// v_diff.u &= v_mask;
				v_diff.u = _mm256_and_si256(v_diff.u, v_mask);

				// v_diff.u = v_diff.u | v_sign;   // AFTER v_mask, BEFORE v_shift.
				v_diff.u = _mm256_or_si256(v_diff.u, v_sign);

				// v_diff.u <<= v_shift;
				v_diff.u = _mm256_sllv_epi64(v_diff.u, v_shift);

				if (pow2 != (uint64_t) v_pow2[0])
					error("pow2 != v_pow2[0] : %064lb != %064lb", pow2, v_pow2[0]);
				if (mask != (uint64_t) v_mask[0])
					error("mask != v_mask[0] : %064lb != %064lb", mask, v_mask[0]);
				if (msb != (uint64_t) v_msb[0])
					error("msb != v_msb[0] : %064lb != %064lb", msb, v_msb[0]);
				if (sign != (uint64_t) v_sign[0])
					error("sign != v_sign[0] : %064lb != %064lb", sign, v_sign[0]);
				diff.u = v_diff.u[0];
				pow2 = v_pow2[0];
				mask = v_mask[0];
				msb = v_msb[0];
				sign = v_sign[0];
			}


			// if (trailing_zeros)
				// error("diff.u != buf_diff: %064lb != %064lb\n%ld, %ld, %ld\n", diff.u, buf_diff, len, len_bits, trailing_zeros);

			if (diff.u != buf_diff)
			{
				error("diff.u != buf_diff: %064lb != %064lb\n%ld, %ld, %ld, %ld\n%064lb\n%064lb\n%064lb\n%064lb\n%064lb\n", diff.u, buf_diff, len, len_bits, leading_sign_bits, trailing_zeros,
						buf_diff,
						pow2,
						mask,
						msb,
						sign);
			}

		}
	}
	data_val_lanes_len[0] = Bs[0].len;
	data_val_lanes_len[1] = Bs[1].len;
	data_val_lanes_len[2] = Bs[2].len;
	data_val_lanes_len[3] = Bs[3].len;

	unsigned char * bytestream = &data_val_lens[data_val_lens_bytes];
	memcpy(bytestream, data_val_lanes[0], Bs[0].len);
	bytestream += Bs[0].len;
	memcpy(bytestream, data_val_lanes[1], Bs[1].len);
	bytestream += Bs[1].len;
	memcpy(bytestream, data_val_lanes[2], Bs[2].len);
	bytestream += Bs[2].len;
	memcpy(bytestream, data_val_lanes[3], Bs[3].len);
	bytestream += Bs[3].len;

	if (num_vals_out != NULL)
		*num_vals_out = num_vals;

	return data_intro_bytes + data_coords_bytes + data_val_lens_bytes + Bs[0].len + Bs[1].len + Bs[2].len + Bs[3].len;
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//------------------------------------------------------------------------------------------------------------------------------------------
//-                                                            Decompression                                                               -
//------------------------------------------------------------------------------------------------------------------------------------------
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


//==========================================================================================================================================
//= Multiply Add
//==========================================================================================================================================


static __attribute__((always_inline)) inline
void
mult_add_serial(ValueType * x_rel, ValueType * y_rel, __m256d val, __m256i row_rel, __m256i col_rel)
{
	y_rel[row_rel[0]] += val[0] * x_rel[col_rel[0]];
	y_rel[row_rel[1]] += val[1] * x_rel[col_rel[1]];
	y_rel[row_rel[2]] += val[2] * x_rel[col_rel[2]];
	y_rel[row_rel[3]] += val[3] * x_rel[col_rel[3]];
}


static __attribute__((always_inline)) inline
// __m256d
// mult_add_r0(__m256d v_res, ValueType * x_rel, __m256d val, __m256i col_rel)
double
mult_add_r0(double v_res, ValueType * x_rel, __m256d val, __m256i col_rel)
{

	// __m256d v_x;
	// v_x = _mm256_set_pd(x_rel[col_rel[3]], x_rel[col_rel[2]], x_rel[col_rel[1]], x_rel[col_rel[0]]);
	// v_res = _mm256_fmadd_pd(val, v_x, v_res);
	// v_res[0] += val[0] * x_rel[col_rel[0]];
	// v_res[1] += val[1] * x_rel[col_rel[1]];
	// v_res[2] += val[2] * x_rel[col_rel[2]];
	// v_res[3] += val[3] * x_rel[col_rel[3]];

	v_res += val[0] * x_rel[col_rel[0]] + val[1] * x_rel[col_rel[1]] + val[2] * x_rel[col_rel[2]] + val[3] * x_rel[col_rel[3]];

	return v_res;
}

static __attribute__((always_inline)) inline
// double
// mult_add_r0_finalize(__m256d v_res, ValueType y_rel)
double
mult_add_r0_finalize(double v_res, ValueType y_rel)
{
	// return y_rel + hsum256_pd(v_res);
	return y_rel + v_res;
}


static __attribute__((always_inline)) inline
void
mult_add_ybuf(long num_rows, double * y_buf_in, ValueType * x_rel, __m256d val, __m256i row_rel, __m256i col_rel)
{
	__m256d v_x, v_y;
	double (* y_buf)[num_rows] = (typeof(y_buf)) y_buf_in;
	v_x = _mm256_set_pd(x_rel[col_rel[3]], x_rel[col_rel[2]], x_rel[col_rel[1]], x_rel[col_rel[0]]);
	v_y = _mm256_set_pd(y_buf[3][row_rel[3]], y_buf[2][row_rel[2]], y_buf[1][row_rel[1]], y_buf[0][row_rel[0]]);
	v_y = _mm256_fmadd_pd(val, v_x, v_y);
	y_buf[0][row_rel[0]] = v_y[0];
	y_buf[1][row_rel[1]] = v_y[1];
	y_buf[2][row_rel[2]] = v_y[2];
	y_buf[3][row_rel[3]] = v_y[3];
}

static __attribute__((always_inline)) inline
void
mult_add_ybuf_finalize(long num_rows, double * y_buf_in, ValueType * y_rel)
{
	__m256d v_y;
	long rem = num_rows % 4;
	double (* y_buf)[num_rows] = (typeof(y_buf)) y_buf_in;
	__m256d v_zero = {0, 0, 0, 0};
	long i;
	for (i=0;i<num_rows-rem;i+=4)
	{
		v_y = _mm256_loadu_pd(&y_rel[i]);
		v_y = _mm256_add_pd(v_y, _mm256_loadu_pd(&y_buf[0][i]));
		v_y = _mm256_add_pd(v_y, _mm256_loadu_pd(&y_buf[1][i]));
		v_y = _mm256_add_pd(v_y, _mm256_loadu_pd(&y_buf[2][i]));
		v_y = _mm256_add_pd(v_y, _mm256_loadu_pd(&y_buf[3][i]));
		_mm256_storeu_pd(&y_buf[0][i], v_zero);
		_mm256_storeu_pd(&y_buf[1][i], v_zero);
		_mm256_storeu_pd(&y_buf[2][i], v_zero);
		_mm256_storeu_pd(&y_buf[3][i], v_zero);
		_mm256_storeu_pd(&y_rel[i], v_y);
	}
	for (i=num_rows-rem;i<num_rows;i++)
	{
		y_rel[i] += y_buf[3][i] + y_buf[2][i] + y_buf[1][i] + y_buf[0][i];
		y_buf[0][i] = 0;
		y_buf[1][i] = 0;
		y_buf[2][i] = 0;
		y_buf[3][i] = 0;
	}
}


//==========================================================================================================================================
//= Decompress
//==========================================================================================================================================


static __attribute__((always_inline)) inline
long
decompress_and_compute_kernel_sort_diff_base(unsigned char * restrict buf, ValueType * restrict x, ValueType * restrict y, long * restrict num_vals_out, const int validate,
		void (* gather_coords)(long i, unsigned char * data_coords, const uint64_t coords_bytes, uint64_t row_bits, uint64_t col_bits, __m256i * row_rel_out, __m256i * col_rel_out),
		const int mult_add_select
		)
{
	int tnum = omp_get_thread_num();
	ValueType * x_rel;
	ValueType * y_rel;
	double * window = t_window[tnum];
	__m256i row_rel, col_rel;
	__m256i len, len_bits;
	long num_vals;
	uint64_t row_min, col_min;
	long num_rows;
	uint64_t row_bits, col_bits;
	union {
		__m256d d;
		__m256i u;
	} diff;
	__m256i sign;
	long i;
	// __m256d mult_add_r0_res = {0, 0, 0, 0};
	double mult_add_r0_res = 0;

	unsigned char * data_intro = buf;
	uint64_t data_intro_bytes = 0;

	row_bits = data_intro[data_intro_bytes];
	data_intro_bytes += 1;
	col_bits = data_intro[data_intro_bytes];
	data_intro_bytes += 1;

	num_vals = *((uint32_t *) &data_intro[data_intro_bytes]);
	data_intro_bytes += 4;
	num_rows = *((uint32_t *) &data_intro[data_intro_bytes]);
	data_intro_bytes += 4;

	long div_padded = (num_vals + 3) / 4;
	long num_vals_padded = div_padded * 4;

	row_min = *((uint32_t *) &data_intro[data_intro_bytes]);
	y_rel = y + row_min;
	data_intro_bytes += 4;
	col_min = *((uint32_t *) &data_intro[data_intro_bytes]);
	x_rel = x + col_min;
	data_intro_bytes += 4;

	double * y_buf = t_y_buf[tnum];

	union {
		__m256d d;
		__m256i u;
	} val, val_prev;
	val.d = _mm256_loadu_pd((double *) &data_intro[data_intro_bytes]);
	data_intro_bytes += 4*8;

	int * data_val_lanes_len;
	data_val_lanes_len = (typeof(data_val_lanes_len)) &(data_intro[data_intro_bytes]);
	data_intro_bytes += 4 * sizeof(*data_val_lanes_len);

	unsigned char * data_coords = &data_intro[data_intro_bytes];
	const uint64_t coords_bytes = (row_bits + col_bits + 7) >> 3;
	const uint64_t data_coords_bytes = coords_bytes * num_vals_padded;

	unsigned char * data_val_lens = &((unsigned char *) data_coords)[data_coords_bytes];
	const uint64_t data_val_lens_bytes = num_vals_padded;

	__m256i data_val_lanes;
	data_val_lanes[0] = (long long) &data_val_lens[data_val_lens_bytes];
	data_val_lanes[1] = data_val_lanes[0] + data_val_lanes_len[0];
	data_val_lanes[2] = data_val_lanes[1] + data_val_lanes_len[1];
	data_val_lanes[3] = data_val_lanes[2] + data_val_lanes_len[2];

	i = 0;
	gather_coords(i, data_coords, coords_bytes, row_bits, col_bits, &row_rel, &col_rel);

	if (validate)
		_mm256_storeu_pd(&window[0], val.d);
	else
	{
		switch (mult_add_select) {
			case 0: mult_add_serial(x_rel, y_rel, val.d, row_rel, col_rel); break;
			case 1: mult_add_r0_res = mult_add_r0(mult_add_r0_res, x_rel, val.d, col_rel); break;
			case 2: mult_add_ybuf(num_rows, y_buf, x_rel, val.d, row_rel, col_rel); break;
		}
	}

	const __m256i one = _mm256_set1_epi64x(1ULL);
	// __m256d lane_run_avg = _mm256_set1_pd(1.0);
	__m256d lane_run_avg = _mm256_set1_pd(0.0);
	for (i=4;i<num_vals_padded;i+=4)
	{
		val_prev.u = val.u;

		// val.d = val.d * ratios;
		// val.d = _mm256_mul_pd(val.d, ratios);
		// val.d = _mm256_add_pd(val.d, ratios);

		val.d = lane_run_avg + val.d;

		gather_coords(i, data_coords, coords_bytes, row_bits, col_bits, &row_rel, &col_rel);

		if (!validate)
		{
			// v_x = _mm256_set_pd(x_rel[col_rel[3]], x_rel[col_rel[2]], x_rel[col_rel[1]], x_rel[col_rel[0]]);
			// v_x = _mm256_i64gather_pd(x_rel, col_rel, 8);
		}

		len = _mm256_set_epi64x(data_val_lens[i+3], data_val_lens[i+2], data_val_lens[i+1], data_val_lens[i+0]);
		/* __m256i _mm_loadu_si64 (void const* mem_addr)
		 *     Load unaligned 64-bit integer from memory into the first element of dst.
		 *
		 * __m256i _mm256_cvtepu8_epi64 (__m256i a)
		 *     Zero extend packed unsigned 8-bit integers in the low 8 bytes of a to packed 64-bit integers, and store the results in dst.
		 */
		// len = _mm256_cvtepu8_epi64(_mm_loadu_si64(&data_val_lens[i]));

		// __m256i tz = (len >> 2ULL) & (~3ULL);
		__m256i tz = _mm256_and_si256(_mm256_srli_epi64(len, 2ULL), _mm256_set1_epi64x(~3ULL));
		// len &= 15ULL;
		len = _mm256_and_si256(len, _mm256_set1_epi64x(15ULL));
		// len_bits = len << 3ULL;
		len_bits = _mm256_slli_epi64(len, 3ULL);

		diff.u = _mm256_set_epi64x(*((uint64_t *) data_val_lanes[3]), *((uint64_t *) data_val_lanes[2]), *((uint64_t *) data_val_lanes[1]), *((uint64_t *) data_val_lanes[0]));

		// data_val_lanes = data_val_lanes + len;
		data_val_lanes = _mm256_add_epi64(data_val_lanes, len);



		/* // __m256i mask = (1ULL << (len << 3ULL)) - 1ULL;
		__m256i mask = _mm256_sub_epi64(_mm256_sllv_epi64(_mm256_set1_epi64x(1ULL), _mm256_slli_epi64(len, 3ULL)), _mm256_set1_epi64x(1ULL));
		// diff.u &= mask;
		diff.u = _mm256_and_si256(diff.u, mask);
		// diff.u <<= tz;
		diff.u = _mm256_sllv_epi64(diff.u, tz); */



		// __m256i pow2 = (1ULL << len_bits);
		__m256i pow2 = _mm256_sllv_epi64(one, len_bits);

		// __m256i msb = pow2 >> 1ULL;
		__m256i msb = _mm256_srli_epi64(pow2, 1ULL);
		// sign = diff.u & msb;
		sign = _mm256_and_si256(diff.u, msb);
		// sign = -sign;
		sign = _mm256_sub_epi64(_mm256_xor_si256(sign, sign), sign);   // There is no negation, so we have to do it via subtracting from 0.

		// __m256i mask = pow2 - 1ULL;
		__m256i mask = _mm256_sub_epi64(pow2, one);
		// diff.u &= mask;
		diff.u = _mm256_and_si256(diff.u, mask);

		// diff.u = diff.u | sign;   // AFTER mask, BEFORE shift.
		diff.u = _mm256_or_si256(diff.u, sign);

		// diff.u <<= tz;
		diff.u = _mm256_sllv_epi64(diff.u, tz);



		// val.u = val.u + diff.u;
		val.u = _mm256_add_epi64(val.u, diff.u);

		// lane_run_avg = (lane_run_avg + val.d - val_prev.d) / 2;
		lane_run_avg = val.d - val_prev.d;

		if (validate)
			_mm256_storeu_pd(&window[i], val.d);
		else
		{
			switch (mult_add_select) {
				case 0: mult_add_serial(x_rel, y_rel, val.d, row_rel, col_rel); break;
				case 1: mult_add_r0_res = mult_add_r0(mult_add_r0_res, x_rel, val.d, col_rel); break;
				case 2: mult_add_ybuf(num_rows, y_buf, x_rel, val.d, row_rel, col_rel); break;
			}
		}
	}

	if (!validate)
	{
		switch (mult_add_select) {
			case 1: *y_rel = mult_add_r0_finalize(mult_add_r0_res, *y_rel); break;
			case 2: mult_add_ybuf_finalize(num_rows, y_buf, y_rel); break;
		}
	}

	if (num_vals_out != NULL)
		*num_vals_out = num_vals;
	return data_intro_bytes + data_coords_bytes + data_val_lens_bytes + data_val_lanes_len[0] + data_val_lanes_len[1] + data_val_lanes_len[2] + data_val_lanes_len[3];
}


static __attribute__((always_inline)) inline
long
decompress_and_compute_kernel_sort_diff_select(unsigned char * restrict buf, ValueType * restrict x, ValueType * restrict y, long * restrict num_vals_out, const int validate)
{
	uint64_t row_bits, col_bits;
	unsigned char * data_intro = buf;
	uint64_t data_intro_bytes = 0;
	__attribute__((unused)) long num_vals;
	__attribute__((unused)) long num_rows;

	row_bits = data_intro[data_intro_bytes];
	data_intro_bytes += 1;
	col_bits = data_intro[data_intro_bytes];
	data_intro_bytes += 1;

	num_vals = *((uint32_t *) &data_intro[data_intro_bytes]);
	data_intro_bytes += 4;
	num_rows = *((uint32_t *) &data_intro[data_intro_bytes]);
	data_intro_bytes += 4;

	const uint64_t coords_bytes = (row_bits + col_bits + 7) >> 3;

	if (__builtin_expect(row_bits == 8, 1))
	// if (row_bits == 8)
	{
		// if (0)
		if (1)
		// if (num_rows > 32)
		{
			switch (col_bits) {
				case 8:
					return decompress_and_compute_kernel_sort_diff_base(buf, x, y, num_vals_out, validate, gather_coords_sparse_r1_c1, 0);
				case 16:
					return decompress_and_compute_kernel_sort_diff_base(buf, x, y, num_vals_out, validate, gather_coords_sparse_r1_c2, 0);
				case 24:
					return decompress_and_compute_kernel_sort_diff_base(buf, x, y, num_vals_out, validate, gather_coords_sparse_r1_c3, 0);
				default:
					return decompress_and_compute_kernel_sort_diff_base(buf, x, y, num_vals_out, validate, gather_coords_sparse_r1_c4, 0);
			}
		}
		else
		{
			switch (col_bits) {
				case 8:
					return decompress_and_compute_kernel_sort_diff_base(buf, x, y, num_vals_out, validate, gather_coords_sparse_r1_c1, 2);
				case 16:
					return decompress_and_compute_kernel_sort_diff_base(buf, x, y, num_vals_out, validate, gather_coords_sparse_r1_c2, 2);
				case 24:
					return decompress_and_compute_kernel_sort_diff_base(buf, x, y, num_vals_out, validate, gather_coords_sparse_r1_c3, 2);
				case 32:
					return decompress_and_compute_kernel_sort_diff_base(buf, x, y, num_vals_out, validate, gather_coords_sparse_r1_c4, 2);
			}
		}
	}

	/* Huge rows, or columns forming small rows with no stray elements. */
	if (row_bits == 0)
	{

		// return decompress_and_compute_kernel_sort_diff_base(buf, x, y, num_vals_out, validate, gather_coords_sparse_r0, 0);
		switch (col_bits) {
			case 8:
				return decompress_and_compute_kernel_sort_diff_base(buf, x, y, num_vals_out, validate, gather_coords_sparse_r0_c1, 0);
			case 16:
				return decompress_and_compute_kernel_sort_diff_base(buf, x, y, num_vals_out, validate, gather_coords_sparse_r0_c2, 0);
			case 24:
				return decompress_and_compute_kernel_sort_diff_base(buf, x, y, num_vals_out, validate, gather_coords_sparse_r0_c3, 0);
			default:
				return decompress_and_compute_kernel_sort_diff_base(buf, x, y, num_vals_out, validate, gather_coords_sparse_r0_c4, 0);
		}

	}

	switch (coords_bytes) {
		case 1:
			return decompress_and_compute_kernel_sort_diff_base(buf, x, y, num_vals_out, validate, gather_coords_dense_1, 0);
		case 2:
			return decompress_and_compute_kernel_sort_diff_base(buf, x, y, num_vals_out, validate, gather_coords_dense_2, 0);
		case 3:
			return decompress_and_compute_kernel_sort_diff_base(buf, x, y, num_vals_out, validate, gather_coords_dense_3, 0);
		case 4:
			return decompress_and_compute_kernel_sort_diff_base(buf, x, y, num_vals_out, validate, gather_coords_dense_4, 0);
	}

	return decompress_and_compute_kernel_sort_diff_base(buf, x, y, num_vals_out, validate, gather_coords_dense, 0);

}



static inline
long
decompress_and_compute_kernel_sort_diff(unsigned char * restrict buf, ValueType * restrict x, ValueType * restrict y)
{
	return decompress_and_compute_kernel_sort_diff_select(buf, x, y, NULL, 0);
}


static
long
decompress_kernel_sort_diff(ValueType * vals, unsigned char * restrict buf)
{
	long num_vals;
	int tnum = omp_get_thread_num();
	double * window = t_window[tnum];
	int * permutation = t_permutation[tnum];
	long i, bytes;
	bytes = decompress_and_compute_kernel_sort_diff_select(buf, NULL, NULL, &num_vals, 1);

	long div_padded = (num_vals + 3) / 4;
	long num_vals_padded = div_padded * 4;

	for (i=0;i<num_vals_padded;i++)
	{
		if (permutation[i] < num_vals)
			vals[permutation[i]] = window[i];
	}
	return bytes;
}

