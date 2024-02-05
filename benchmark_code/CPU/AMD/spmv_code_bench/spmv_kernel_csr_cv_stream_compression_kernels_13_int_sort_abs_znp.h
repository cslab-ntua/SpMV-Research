#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
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
#ifdef __cplusplus
}
#endif


//==========================================================================================================================================
//= Sorted Delta
//==========================================================================================================================================


//------------------------------------------------------------------------------------------------------------------------------------------
//- Quicksort
//------------------------------------------------------------------------------------------------------------------------------------------

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


static inline
int
quicksort_cmp(int a, int b, double * vals)
{
	double va=vals[a], vb=vals[b];
	if (va*vb < 0)
		return va > 0 ? va : vb;
	return (fabs(va) > fabs(vb)) ? 1 : (fabs(va) < fabs(vb)) ? -1 : 0;
}


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
select_num_vals(INT_T * ia, long i_s, long j_s, long num_vals, long force_row_index_lte_1_byte)
{
	long i_e, j_e;
	long num_rows;
	long num_rows_max = 255;
	long num_vals_ret;
	long num_vals_min = 64;
	j_e = j_s + num_vals;
	i_e = i_s;
	while (ia[i_e] < j_e)
		i_e++;
	num_rows = i_e - i_s;
	if (num_rows > num_rows_max)
		i_e = i_s + num_rows_max;
	num_vals_ret = ia[i_e] - j_s;
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


static inline
long
compress_kernel_sort_diff(INT_T * ia, INT_T * ja, ValueType * vals_unpadded, long i_s, long j_s, unsigned char * buf, long num_vals, long * num_vals_out)
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
	uint64_t leading_zeros = 0;
	uint64_t trailing_zeros = 0;
	uint64_t trailing_zero_bits_div4 = 0;
	uint64_t len_bits = 0;
	uint64_t len = 0;
	union {
		double d;
		uint64_t u;
	} diff;
	uint64_t row_diff = 0, row_diff_max = 0, col = 0, col_diff = 0, col_min = 0, col_max = 0, col_diff_max = 0;
	uint64_t row_bits = 0, col_bits = 0;
	uint64_t row_col_bytes = 0;
	long i, row_max, j, k, l;   // row_max is the last row (i.e. inclusive: num_rows = row_max + 1 - i_s)

	long force_row_index_lte_1_byte = 1;
	long force_row_index_rounding = 0;

	num_vals = select_num_vals(ia, i_s, j_s, num_vals, force_row_index_lte_1_byte);

	long div_padded = (num_vals + 3) / 4;
	long num_vals_padded = div_padded * 4;

	for (k=0,i=i_s,j=j_s; k<num_vals; k++,j++)
	{
		if (j >= ia[i+1])
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
		rows_diff[rev_permutation[k]] = rows[k] - i_s;
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
		rows_diff[rev_permutation[k]] = row_max - i_s;   // in the last row
		cols[rev_permutation[k]] = col_min;   // use col_min so that we don't change col_diff_max
	}
	row_diff_max = row_max - i_s;
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
		if (rows_diff[k] + i_s > row_max)
			error("row index");
		if (permutation[k] >= num_vals) // extra fake values
			continue;
		if (cols[k] != (unsigned int) ja[j_s+permutation[k]])
			error("col index");
	}
	// if (tnum == 0) fprintf(stderr, "i=[%ld, %ld], j_s=[%ld, %ld], row_diff_max=%ld , row_bits=%ld , col_diff_max=%ld , col_bits=%ld\n", i_s, row_max, j_s, j, row_diff_max, row_bits, col_diff_max, col_bits);

	unsigned char * data_intro = buf;
	uint64_t data_intro_bytes = 0;

	// Should always be the first data in the packet.
	data_intro[data_intro_bytes] = row_bits;
	data_intro_bytes += 1;
	data_intro[data_intro_bytes] = col_bits;
	data_intro_bytes += 1;

	*((uint32_t *) &data_intro[data_intro_bytes]) = num_vals;
	data_intro_bytes += 4;
	*((uint32_t *) &data_intro[data_intro_bytes]) = i_s;
	data_intro_bytes += 4;
	*((uint32_t *) &data_intro[data_intro_bytes]) = col_min;
	data_intro_bytes += 4;

	*((uint32_t *) &data_intro[data_intro_bytes]) = row_diff_max + 1;
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

	unsigned char * data_val_lens = &data_coords[data_coords_bytes];
	const uint64_t data_val_lens_bytes = num_vals_padded;
	// if (tnum == 0) fprintf(stderr, "c: num_vals=%ld, num_vals_padded=%ld, row_min=%ld, col_min=%ld, row_bits=%ld, col_bits=%ld, val=%lf\n", num_vals, num_vals_padded, i_s, col_min, row_bits, col_bits, window[0]);

	// unsigned char * bytestream = &data_val_lens[data_val_lens_bytes];

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

	double tolerance = atof(getenv("VC_TOLERANCE"));
	union {
		double d;
		uint64_t u;
	} val_prev[4], val;
	val_prev[0].d = window[0];
	val_prev[1].d = window[1];
	val_prev[2].d = window[2];
	val_prev[3].d = window[3];
	for (i=4;i<num_vals_padded;i++)
	{
		val.d = window[i];
		diff.u = val.u - val_prev[i%4].u;

		if (tolerance > 0)
		{
			diff.u = compress_diff_lossy(window[i], val_prev[i%4].d, diff.d, tolerance);
		}

		/* Instead of setting to 'window[i]', we emulate the procedure of the decompression.
		 * This accounts for the accumulation of the error that is being passed down.
		 *
		 * Note:
		 *     Maybe check if this is optimized out (e.g. -ffast-math, but if user has such flags, it doesn't really make a difference).
		 */
		val_prev[i%4].u += diff.u;
		double error = fabs(val.d - val_prev[i%4].d) / fabs(val.d);
		if (error > tolerance)
			error("test");

		if (diff.u == 0)
		{
			len = 0;
			data_val_lens[i] = 0;
		}
		else
		{
			/* Differences  can actually be negative due to the rounding errors (when lossy)!
			 *
			 * For the length values (0-8) 4 bits are needed.
			 *
			 * For the trailing zero bits values (0-63, 64 isn't needed) 6 bits are needed, but we only have 4 bits space remaining.
			 * so we keep shifts in strides of 4 (0, 4, 8, 12, ..., 60).
			 */
			uint64_t rem_tz;
			uint64_t buf_diff = diff.u;
			leading_zeros = __builtin_clzl(diff.u);
			trailing_zeros = __builtin_ctzl(diff.u);
			rem_tz = trailing_zeros & 3ULL;

			len_bits = 64ULL - leading_zeros - trailing_zeros;
			len = (len_bits + 7ULL) >> 3ULL;
			uint64_t rem = (len << 3ULL) - len_bits;

			if (rem >= rem_tz)
			{
				rem -= rem_tz;
				trailing_zeros -= rem_tz;
			}

			if (rem <= leading_zeros)
			{
				leading_zeros -= rem;
				rem = 0;
			}
			else
			{
				rem -= leading_zeros;
				leading_zeros = 0;
			}
			trailing_zeros -= rem;

			trailing_zero_bits_div4 = trailing_zeros >> 2ULL;
			trailing_zeros = trailing_zero_bits_div4 << 2ULL;

			len_bits = 64ULL - leading_zeros - trailing_zeros;
			len = (len_bits + 7ULL) >> 3ULL;
			if (len > 8)
				error("len > 8");

			diff.u >>= trailing_zeros;
			bytestream_write_unsafe_cast(&Bs[i%4], diff.u, len);
			len |=  (trailing_zero_bits_div4 << 4ULL);
			data_val_lens[i] = len;

			uint64_t shift = (len >> 2ULL) & (~3ULL);
			if (shift != trailing_zeros)
				error("shift: %ld %ld %ld", len, shift, trailing_zero_bits_div4);
			len &= 15ULL;
			uint64_t mask = (1ULL << (len << 3ULL)) - 1ULL;
			diff.u &= mask;
			diff.u <<= shift;
			if (diff.u != buf_diff)
				error("test");
		}
	}
	data_val_lanes_len[0] = Bs[0].len;
	data_val_lanes_len[1] = Bs[1].len;
	data_val_lanes_len[2] = Bs[2].len;
	data_val_lanes_len[3] = Bs[3].len;

	unsigned char * bytestream = &data_val_lens[data_val_lens_bytes];
	for (i=0;i<Bs[0].len;i++)
		bytestream[i] = data_val_lanes[0][i];
	bytestream += Bs[0].len;
	for (i=0;i<Bs[1].len;i++)
		bytestream[i] = data_val_lanes[1][i];
	bytestream += Bs[1].len;
	for (i=0;i<Bs[2].len;i++)
		bytestream[i] = data_val_lanes[2][i];
	bytestream += Bs[2].len;
	for (i=0;i<Bs[3].len;i++)
		bytestream[i] = data_val_lanes[3][i];
	bytestream += Bs[3].len;

	if (num_vals_out != NULL)
		*num_vals_out = num_vals;

	return data_intro_bytes + data_coords_bytes + data_val_lens_bytes + Bs[0].len + Bs[1].len + Bs[2].len + Bs[3].len;
}


static inline
void
gather_coords_dense(long i, unsigned char * data_coords, const uint64_t coords_bytes, uint64_t row_bits, uint64_t col_bits, __m256i * row_rel_out, __m256i * col_rel_out)
{
	const __m256i row_bits_mask = _mm256_set1_epi64x((1ULL<<row_bits) - 1);   // here '-' has precedence over '<<'!
	// const __m256i col_bits_mask = _mm256_set1_epi64x((1ULL<<col_bits) - 1);
	__m256i coords;
	coords = _mm256_set_epi64x(*((uint64_t *) &data_coords[(i+3)*coords_bytes]), *((uint64_t *) &data_coords[(i+2)*coords_bytes]), *((uint64_t *) &data_coords[(i+1)*coords_bytes]), *((uint64_t *) &data_coords[i*coords_bytes]));
	// *row_rel_out = coords & row_bits_mask;
	*row_rel_out = _mm256_and_si256(coords, row_bits_mask);
	// *col_rel_out = _mm256_and_si256(_mm256_srli_epi64(coords, row_bits), col_bits_mask);
	// *col_rel_out = (coords >> row_bits) & col_bits_mask;
	// row_rel_out = _mm256_set_epi64x(bits_u64_extract(coords[3], 0, row_bits), bits_u64_extract(coords[2], 0, row_bits), bits_u64_extract(coords[1], 0, row_bits), bits_u64_extract(coords[0], 0, row_bits));
	*col_rel_out = _mm256_set_epi64x(bits_u64_extract(coords[3], row_bits, col_bits), bits_u64_extract(coords[2], row_bits, col_bits), bits_u64_extract(coords[1], row_bits, col_bits), bits_u64_extract(coords[0], row_bits, col_bits));
}

static inline
void
gather_coords_sparse_r0(long i, unsigned char * data_coords, const uint64_t coords_bytes, __attribute__((unused)) uint64_t row_bits, __attribute__((unused)) uint64_t col_bits, __m256i * row_rel_out, __m256i * col_rel_out)
{
	const __m256i col_bits_mask = _mm256_set1_epi64x((1ULL<<col_bits) - 1);
	__m256i col_rel;
	col_rel = _mm256_set_epi64x(data_coords[(i+3)*coords_bytes], data_coords[(i+2)*coords_bytes], data_coords[(i+1)*coords_bytes], data_coords[i*coords_bytes]);
	// col_rel = col_rel & col_bits_mask;
	col_rel = _mm256_and_si256(col_rel, col_bits_mask);
	*row_rel_out = _mm256_set1_epi64x(0);
	*col_rel_out = col_rel;
}

static inline
void
gather_coords_sparse_r1(long i, unsigned char * data_coords, const uint64_t coords_bytes, __attribute__((unused)) uint64_t row_bits, uint64_t col_bits, __m256i * row_rel_out, __m256i * col_rel_out)
{
	const __m256i col_bits_mask = _mm256_set1_epi64x((1ULL<<col_bits) - 1);
	__m256i row_rel, col_rel;
	row_rel = _mm256_set_epi64x(data_coords[(i+3)*coords_bytes], data_coords[(i+2)*coords_bytes], data_coords[(i+1)*coords_bytes], data_coords[i*coords_bytes]);
	col_rel = _mm256_set_epi64x(*((uint64_t *) &data_coords[(i+3)*coords_bytes + 1]), *((uint64_t *) &data_coords[(i+2)*coords_bytes + 1]), *((uint64_t *) &data_coords[(i+1)*coords_bytes + 1]), *((uint64_t *) &data_coords[i*coords_bytes + 1]));
	// col_rel = col_rel & col_bits_mask;
	col_rel = _mm256_and_si256(col_rel, col_bits_mask);
	*row_rel_out = row_rel;
	*col_rel_out = col_rel;
}

static inline
void
gather_coords_sparse_r1_c1(long i, unsigned char * data_coords, const uint64_t coords_bytes, __attribute__((unused)) uint64_t row_bits, __attribute__((unused)) uint64_t col_bits, __m256i * row_rel_out, __m256i * col_rel_out)
{
	__m256i row_rel, col_rel;
	row_rel = _mm256_set_epi64x(data_coords[(i+3)*coords_bytes], data_coords[(i+2)*coords_bytes], data_coords[(i+1)*coords_bytes], data_coords[i*coords_bytes]);
	col_rel = _mm256_set_epi64x(data_coords[(i+3)*coords_bytes + 1], data_coords[(i+2)*coords_bytes + 1], data_coords[(i+1)*coords_bytes + 1], data_coords[i*coords_bytes + 1]);
	*row_rel_out = row_rel;
	*col_rel_out = col_rel;
}

static inline
void
gather_coords_sparse_r1_c2(long i, unsigned char * data_coords, const uint64_t coords_bytes, __attribute__((unused)) uint64_t row_bits, __attribute__((unused)) uint64_t col_bits, __m256i * row_rel_out, __m256i * col_rel_out)
{
	__m256i row_rel, col_rel;
	row_rel = _mm256_set_epi64x(data_coords[(i+3)*coords_bytes], data_coords[(i+2)*coords_bytes], data_coords[(i+1)*coords_bytes], data_coords[i*coords_bytes]);
	col_rel = _mm256_set_epi64x(*((uint16_t *) &data_coords[(i+3)*coords_bytes + 1]), *((uint16_t *) &data_coords[(i+2)*coords_bytes + 1]), *((uint16_t *) &data_coords[(i+1)*coords_bytes + 1]), *((uint16_t *) &data_coords[i*coords_bytes + 1]));
	*row_rel_out = row_rel;
	*col_rel_out = col_rel;
}

static inline
void
gather_coords_sparse_r1_c3(long i, unsigned char * data_coords, const uint64_t coords_bytes, __attribute__((unused)) uint64_t row_bits, __attribute__((unused)) uint64_t col_bits, __m256i * row_rel_out, __m256i * col_rel_out)
{
	const __m256i col_bits_mask = _mm256_set1_epi64x((1ULL<<24) - 1);
	__m256i row_rel, col_rel;
	row_rel = _mm256_set_epi64x(data_coords[(i+3)*coords_bytes], data_coords[(i+2)*coords_bytes], data_coords[(i+1)*coords_bytes], data_coords[i*coords_bytes]);
	col_rel = _mm256_set_epi64x(*((uint64_t *) &data_coords[(i+3)*coords_bytes + 1]), *((uint64_t *) &data_coords[(i+2)*coords_bytes + 1]), *((uint64_t *) &data_coords[(i+1)*coords_bytes + 1]), *((uint64_t *) &data_coords[i*coords_bytes + 1]));
	// col_rel = col_rel & col_bits_mask;
	col_rel = _mm256_and_si256(col_rel, col_bits_mask);
	*row_rel_out = row_rel;
	*col_rel_out = col_rel;
}

static inline
void
gather_coords_sparse_r1_c4(long i, unsigned char * data_coords, const uint64_t coords_bytes, __attribute__((unused)) uint64_t row_bits, __attribute__((unused)) uint64_t col_bits, __m256i * row_rel_out, __m256i * col_rel_out)
{
	__m256i row_rel, col_rel;
	row_rel = _mm256_set_epi64x(data_coords[(i+3)*coords_bytes], data_coords[(i+2)*coords_bytes], data_coords[(i+1)*coords_bytes], data_coords[i*coords_bytes]);
	col_rel = _mm256_set_epi64x(*((uint32_t *) &data_coords[(i+3)*coords_bytes + 1]), *((uint32_t *) &data_coords[(i+2)*coords_bytes + 1]), *((uint32_t *) &data_coords[(i+1)*coords_bytes + 1]), *((uint32_t *) &data_coords[i*coords_bytes + 1]));
	*row_rel_out = row_rel;
	*col_rel_out = col_rel;
}


static __attribute__((always_inline)) inline
long
decompress_and_compute_kernel_sort_diff_base(unsigned char * restrict buf, ValueType * restrict x, ValueType * restrict y, long * restrict num_vals_out, const int validate,
		void (* gather_coords)(long i, unsigned char * data_coords, const uint64_t coords_bytes, uint64_t row_bits, uint64_t col_bits, __m256i * row_rel_out, __m256i * col_rel_out))
{
	int tnum = omp_get_thread_num();
	ValueType * x_rel;
	ValueType * y_rel;
	double * window = t_window[tnum];
	__m256i row_rel, col_rel;
	__m256i len;
	// __m256d v_x, v_y;
	// __m256d v_tmp;
	long num_vals;
	uint64_t row_min, col_min;
	__attribute__((unused)) long num_rows;
	uint64_t row_bits, col_bits;
	union {
		__m256d d;
		__m256i u;
	} diff;
	long i;

	unsigned char * data_intro = buf;
	uint64_t data_intro_bytes = 0;

	row_bits = data_intro[data_intro_bytes];
	data_intro_bytes += 1;
	col_bits = data_intro[data_intro_bytes];
	data_intro_bytes += 1;

	num_vals = *((uint32_t *) &data_intro[data_intro_bytes]);
	data_intro_bytes += 4;

	long div_padded = (num_vals + 3) / 4;
	long num_vals_padded = div_padded * 4;

	row_min = *((uint32_t *) &data_intro[data_intro_bytes]);
	y_rel = y + row_min;
	data_intro_bytes += 4;
	col_min = *((uint32_t *) &data_intro[data_intro_bytes]);
	x_rel = x + col_min;
	data_intro_bytes += 4;

	num_rows = *((uint32_t *) &data_intro[data_intro_bytes]);
	data_intro_bytes += 4;
	// double (* y_buf)[num_rows] = (typeof(y_buf)) t_y_buf[tnum];

	union {
		__m256d d;
		__m256i u;
	} val;
	val.d = _mm256_loadu_pd((double *) &data_intro[data_intro_bytes]);
	data_intro_bytes += 4*8;

	int * data_val_lanes_len;
	data_val_lanes_len = (typeof(data_val_lanes_len)) &(data_intro[data_intro_bytes]);
	data_intro_bytes += 4 * sizeof(*data_val_lanes_len);

	unsigned char * data_coords = &data_intro[data_intro_bytes];

	const uint64_t coords_bytes = (row_bits + col_bits + 7) >> 3;

	const uint64_t data_coords_bytes = coords_bytes * num_vals_padded;

	unsigned char * data_val_lens = &data_coords[data_coords_bytes];
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

		y_rel[row_rel[0]] += val.d[0] * x_rel[col_rel[0]];
		y_rel[row_rel[1]] += val.d[1] * x_rel[col_rel[1]];
		y_rel[row_rel[2]] += val.d[2] * x_rel[col_rel[2]];
		y_rel[row_rel[3]] += val.d[3] * x_rel[col_rel[3]];

		// y_buf[0][row_rel[0]] += val.d[0] * x_rel[col_rel[0]];
		// y_buf[1][row_rel[1]] += val.d[1] * x_rel[col_rel[1]];
		// y_buf[2][row_rel[2]] += val.d[2] * x_rel[col_rel[2]];
		// y_buf[3][row_rel[3]] += val.d[3] * x_rel[col_rel[3]];

	}

	// #pragma GCC unroll 4
	for (i=4;i<num_vals_padded;i+=4)
	{
		gather_coords(i, data_coords, coords_bytes, row_bits, col_bits, &row_rel, &col_rel);

		if (!validate)
		{
			// v_x = _mm256_set_pd(x_rel[col_rel[3]], x_rel[col_rel[2]], x_rel[col_rel[1]], x_rel[col_rel[0]]);
			// v_x = _mm256_i64gather_pd(x_rel, col_rel, 8);
		}

		len = _mm256_set_epi64x(data_val_lens[i+3], data_val_lens[i+2], data_val_lens[i+1], data_val_lens[i+0]);
		/* __m128i _mm_loadu_si64 (void const* mem_addr)
		 *     Load unaligned 64-bit integer from memory into the first element of dst.
		 *
		 * __m256i _mm256_cvtepu8_epi64 (__m128i a)
		 *     Zero extend packed unsigned 8-bit integers in the low 8 bytes of a to packed 64-bit integers, and store the results in dst.
		 */
		// len = _mm256_cvtepu8_epi64(_mm_loadu_si64(&data_val_lens[i]));

		// __m256i shift = (len >> 2ULL) & (~3ULL);
		__m256i shift = _mm256_and_si256(_mm256_srli_epi64(len, 2ULL), _mm256_set1_epi64x(~3ULL));
		// len &= 15ULL;
		len = _mm256_and_si256(len, _mm256_set1_epi64x(15ULL));

		diff.u = _mm256_set_epi64x(*((uint64_t *) data_val_lanes[3]), *((uint64_t *) data_val_lanes[2]), *((uint64_t *) data_val_lanes[1]), *((uint64_t *) data_val_lanes[0]));

		// data_val_lanes = data_val_lanes + len;
		data_val_lanes = _mm256_add_epi64(data_val_lanes, len);

		// __m256i mask = (1ULL << (len << 3ULL)) - 1ULL;
		__m256i mask = _mm256_sub_epi64(_mm256_sllv_epi64(_mm256_set1_epi64x(1ULL), _mm256_slli_epi64(len, 3ULL)), _mm256_set1_epi64x(1ULL));
		// diff.u &= mask;
		diff.u = _mm256_and_si256(diff.u, mask);
		// diff.u <<= shift;
		diff.u = _mm256_sllv_epi64(diff.u, shift);

		// val.u = val.u + diff.u;
		val.u = _mm256_add_epi64(val.u, diff.u);

		if (validate)
			_mm256_storeu_pd(&window[i], val.d);
		else
		{

			// v_x = _mm256_set_pd(x_rel[col_rel[3]], x_rel[col_rel[2]], x_rel[col_rel[1]], x_rel[col_rel[0]]);
			// v_tmp = val.d * v_x;
			// y_buf[row_rel[0]] += v_tmp[0];
			// y_buf[row_rel[1]] += v_tmp[1];
			// y_buf[row_rel[2]] += v_tmp[2];
			// y_buf[row_rel[3]] += v_tmp[3];

			y_rel[row_rel[0]] += val.d[0] * x_rel[col_rel[0]];
			y_rel[row_rel[1]] += val.d[1] * x_rel[col_rel[1]];
			y_rel[row_rel[2]] += val.d[2] * x_rel[col_rel[2]];
			y_rel[row_rel[3]] += val.d[3] * x_rel[col_rel[3]];

			// v_x = _mm256_set_pd(x_rel[col_rel[3]], x_rel[col_rel[2]], x_rel[col_rel[1]], x_rel[col_rel[0]]);
			// v_y = _mm256_set_pd(y_buf[3][row_rel[3]], y_buf[2][row_rel[2]], y_buf[1][row_rel[1]], y_buf[0][row_rel[0]]);
			// v_y = _mm256_fmadd_pd(val.d, v_x, v_y);
			// y_buf[0][row_rel[0]] = v_y[0];
			// y_buf[1][row_rel[1]] = v_y[1];
			// y_buf[2][row_rel[2]] = v_y[2];
			// y_buf[3][row_rel[3]] = v_y[3];

		}
	}

	// if (!validate)
	// {
		// long rem = num_rows % 4;
		// __m256d v_zero = {0, 0, 0, 0};
		// for (i=0;i<num_rows-rem;i+=4)
		// {
			// v_y = _mm256_loadu_pd(&y_rel[i]);
			// v_y = _mm256_add_pd(v_y, _mm256_loadu_pd(&y_buf[0][i]));
			// v_y = _mm256_add_pd(v_y, _mm256_loadu_pd(&y_buf[1][i]));
			// v_y = _mm256_add_pd(v_y, _mm256_loadu_pd(&y_buf[2][i]));
			// v_y = _mm256_add_pd(v_y, _mm256_loadu_pd(&y_buf[3][i]));
			// _mm256_storeu_pd(&y_buf[0][i], v_zero);
			// _mm256_storeu_pd(&y_buf[1][i], v_zero);
			// _mm256_storeu_pd(&y_buf[2][i], v_zero);
			// _mm256_storeu_pd(&y_buf[3][i], v_zero);
			// _mm256_storeu_pd(&y_rel[i], v_y);
		// }
		// for (i=num_rows-rem;i<num_rows;i++)
		// {
			// y_rel[i] += y_buf[3][i] + y_buf[2][i] + y_buf[1][i] + y_buf[0][i];
			// y_buf[0][i] = 0;
			// y_buf[1][i] = 0;
			// y_buf[2][i] = 0;
			// y_buf[3][i] = 0;
		// }
	// }

	// printf("%d: test 2\n", tnum);
	// if (tnum == 0) fprintf(stdout, "d: data_intro=%p (%ld), data_coords=%p (%ld), data_val_lens=%p (%ld), bytestream=%p (%ld), \n", data_intro, data_intro_bytes, data_coords, data_coords_bytes, data_val_lens, data_val_lens_bytes, bytestream, ((Bs.len_bits + 7) >> 3));

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
	row_bits = data_intro[0];
	col_bits = data_intro[1];

	if (__builtin_expect(row_bits == 8, 1))
	{
		// return decompress_and_compute_kernel_sort_diff_base(buf, x, y, num_vals_out, validate, gather_coords_sparse_r1);
		switch (col_bits) {
			case 8:
				return decompress_and_compute_kernel_sort_diff_base(buf, x, y, num_vals_out, validate, gather_coords_sparse_r1_c1);
			case 16:
				return decompress_and_compute_kernel_sort_diff_base(buf, x, y, num_vals_out, validate, gather_coords_sparse_r1_c2);
			case 24:
				return decompress_and_compute_kernel_sort_diff_base(buf, x, y, num_vals_out, validate, gather_coords_sparse_r1_c3);
			case 32:
				return decompress_and_compute_kernel_sort_diff_base(buf, x, y, num_vals_out, validate, gather_coords_sparse_r1_c4);
		}
	}

	/* Huge rows, or columns forming small rows with no stray elements. */
	// if (__builtin_expect(row_bits == 0, 0))
	if (row_bits == 0)
		return decompress_and_compute_kernel_sort_diff_base(buf, x, y, num_vals_out, validate, gather_coords_sparse_r0);

	return decompress_and_compute_kernel_sort_diff_base(buf, x, y, num_vals_out, validate, gather_coords_dense);

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

