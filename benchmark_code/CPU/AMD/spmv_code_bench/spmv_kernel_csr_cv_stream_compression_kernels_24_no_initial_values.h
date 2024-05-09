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

	#include "spmv_kernel_csr_cv_stream_gather_coords_avx2.h"
#ifdef __cplusplus
}
#endif


//==========================================================================================================================================
//= Sorted Delta
//==========================================================================================================================================


static uint64_t window_size_bits;
static uint64_t window_size;
static double ** t_vals_buf;
static double ** t_rows;
static double ** t_window;
static unsigned char *** t_data_lanes;
static int ** t_permutation_sort;
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
	t_vals_buf = (typeof(t_vals_buf)) malloc(num_threads * sizeof(*t_vals_buf));
	t_rows = (typeof(t_rows)) malloc(num_threads * sizeof(*t_rows));
	t_window = (typeof(t_window)) malloc(num_threads * sizeof(*t_window));
	t_data_lanes = (typeof(t_data_lanes)) malloc(num_threads * sizeof(*t_data_lanes));
	t_permutation_sort = (typeof(t_permutation_sort)) malloc(num_threads * sizeof(*t_permutation_sort));
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
		t_vals_buf[tnum] = (typeof(&(**t_vals_buf))) malloc(window_size * sizeof(**t_vals_buf));
		t_rows[tnum] = (typeof(&(**t_rows))) malloc(window_size * sizeof(**t_rows));
		t_window[tnum] = (typeof(&(**t_window))) malloc(window_size * sizeof(**t_window));
		t_data_lanes[tnum] = (typeof(&(**t_data_lanes))) malloc(4 * sizeof(**t_data_lanes));
		for (i=0;i<4;i++)
		{
			t_data_lanes[tnum][i] = (typeof(&(**t_data_lanes[i]))) malloc(8*window_size * sizeof(**t_data_lanes[i]));
		}
		t_permutation_sort[tnum] = (typeof(&(**t_permutation_sort))) malloc(window_size * sizeof(**t_permutation_sort));
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
compress_kernel_sort_diff(INT_T * row_ptr, INT_T * ja, ValueType * vals, long i_s, long j_s, unsigned char * buf, long num_vals, long * num_vals_out)
{
	int tnum = omp_get_thread_num();
	ValueType * vals_buf = t_vals_buf[tnum];
	ValueType * rows = t_rows[tnum];
	double * window = t_window[tnum];
	unsigned char ** data_val_lanes = t_data_lanes[tnum];
	int * data_val_lanes_size;
	unsigned int * rows_diff = t_rows_diff[tnum];
	unsigned int * cols = t_cols[tnum];
	int * permutation_sort = t_permutation_sort[tnum];
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
	uint64_t row_min, row_max, row_diff = 0, row_diff_max = 0, col = 0, col_min = 0, col_max = 0, col_diff = 0, col_diff_max = 0;   // row_max is the last row (i.e., inclusive: num_rows = row_max + 1 - i_s)
	uint64_t row_bits = 0, col_bits = 0;
	uint64_t row_col_bytes = 0;
	long i, j, k, l;
	long lane_id;

	long force_row_index_lte_1_byte = 1;
	long force_row_index_rounding = 0;

	row_min = i_s;   // 'i_s' is certain to be the first non-empty row.

	num_vals = select_num_vals(row_ptr, row_min, j_s, num_vals, force_row_index_lte_1_byte);

	long num_vals_mod = num_vals % 4;
	long num_vals_div = num_vals / 4;

	for (k=0,i=row_min,j=j_s; k<num_vals; k++,j++)
	{
		if (j >= row_ptr[i+1])
			i++;
		vals_buf[k] = vals[k];
		rows[k] = i;
	}
	row_max = i;

	for (k=0;k<num_vals;k++)
		permutation_sort[k] = k;
	quicksort(permutation_sort, num_vals, vals_buf, t_qsort_partitions[tnum]);
	for (k=0;k<num_vals;k++)
	{
		l = (k % 4) * num_vals_div + k / 4;   // interleave index
		l += (k % 4 < num_vals_mod) ? k % 4 : num_vals_mod;
		permutation[k] = permutation_sort[l];
	}
	for (k=0;k<num_vals;k++)
		rev_permutation[permutation[k]] = k;
	col_min = ja[j_s];
	col_max = ja[j_s];
	for (k=0,j=j_s;k<num_vals;k++,j++)
	{
		window[rev_permutation[k]] = vals_buf[k];
		rows_diff[rev_permutation[k]] = rows[k] - row_min;
		col = ja[j];
		cols[rev_permutation[k]] = col;
		if (col < col_min)
			col_min = col;
		if (col > col_max)
			col_max = col;
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

	for (k=0;k<num_vals;k++)
	{
		if (window[k] != vals_buf[permutation[k]])
			error("wrong");
		if (rows_diff[k] + row_min > row_max)
			error("row index");
		if (permutation[k] >= num_vals) // extra fake values
			continue;
		if (cols[k] != (unsigned int) ja[j_s+permutation[k]])
			error("col index");
	}

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

	data_val_lanes_size = (typeof(data_val_lanes_size)) &(data_intro[data_intro_bytes]);
	data_intro_bytes += 4 * sizeof(*data_val_lanes_size);

	unsigned char * data_coords = &data_intro[data_intro_bytes];
	const uint64_t coords_bytes = row_col_bytes;
	const uint64_t data_coords_bytes = coords_bytes * num_vals;

	unsigned char * data_val_lens = &data_coords[data_coords_bytes];
	const uint64_t data_val_lens_bytes = num_vals;

	for (i=0;i<num_vals;i++)
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
	} val_prev[4], val;
	val_prev[0].d = 0;
	val_prev[1].d = 0;
	val_prev[2].d = 0;
	val_prev[3].d = 0;
	double tolerance = atof(getenv("VC_TOLERANCE"));
	for (i=0;i<num_vals;i++)
	{
		lane_id = i % 4;
		val.d = window[i];
		diff.u = val.u - val_prev[lane_id].u;

		if (tolerance > 0)
		{
			diff.u = compress_diff_lossy(window[i], val_prev[lane_id].d, diff.d, tolerance);
		}

		/* Instead of setting to 'window[i]', we emulate the procedure of the decompression.
		 * This accounts for the accumulation of the error that is being passed down.
		 *
		 * Note:
		 *     Maybe check if this is optimized out (e.g. -ffast-math, but if user has such flags, it doesn't really make a difference).
		 */
		val_prev[lane_id].u += diff.u;
		double error = fabs(val.d - val_prev[lane_id].d) / fabs(val.d);
		if (error > tolerance)
			error("error tolerance exceeded");

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
			bytestream_write_unsafe_cast(&Bs[lane_id], diff.u, len);
			len |=  (trailing_zero_bits_div4 << 4ULL);
			data_val_lens[i] = len;

			uint64_t tz = (len >> 2ULL) & (~3ULL);
			if (tz != trailing_zeros)
				error("tz: %ld %ld %ld", len, tz, trailing_zero_bits_div4);
			len &= 15ULL;
			uint64_t mask = (1ULL << (len << 3ULL)) - 1ULL;
			diff.u &= mask;
			diff.u <<= tz;
			if (diff.u != buf_diff)
				error("test");
		}
	}
	data_val_lanes_size[0] = Bs[0].len;
	data_val_lanes_size[1] = Bs[1].len;
	data_val_lanes_size[2] = Bs[2].len;
	data_val_lanes_size[3] = Bs[3].len;

	unsigned char * ptr = &data_val_lens[data_val_lens_bytes];
	memcpy(ptr, data_val_lanes[0], Bs[0].len);
	ptr += Bs[0].len;
	memcpy(ptr, data_val_lanes[1], Bs[1].len);
	ptr += Bs[1].len;
	memcpy(ptr, data_val_lanes[2], Bs[2].len);
	ptr += Bs[2].len;
	memcpy(ptr, data_val_lanes[3], Bs[3].len);
	ptr += Bs[3].len;
	const uint64_t data_val_lanes_bytes = Bs[0].len + Bs[1].len + Bs[2].len + Bs[3].len;

	if (num_vals_out != NULL)
		*num_vals_out = num_vals;

	return data_intro_bytes + data_coords_bytes + data_val_lens_bytes + data_val_lanes_bytes;
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
		void (* gather_coords)(long i, unsigned char * data_coords, const uint64_t coords_bytes, uint64_t row_bits, uint64_t col_bits, uint64_t * row_rel_out, uint64_t * col_rel_out),
		void (* gather_coords_v)(long i, unsigned char * data_coords, const uint64_t coords_bytes, uint64_t row_bits, uint64_t col_bits, __m256i * row_rel_out, __m256i * col_rel_out),
		const int mult_add_select
		)
{
	int tnum = omp_get_thread_num();
	ValueType * x_rel;
	ValueType * y_rel;
	double * window = t_window[tnum];
	long num_vals;
	uint64_t row_min, col_min;
	long num_rows;
	uint64_t row_bits, col_bits;
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

	// long num_vals_mod = num_vals % 4;
	long num_vals_div = num_vals / 4;
	long num_vals_mul4 = num_vals_div * 4;

	row_min = *((uint32_t *) &data_intro[data_intro_bytes]);
	y_rel = y + row_min;
	data_intro_bytes += 4;
	col_min = *((uint32_t *) &data_intro[data_intro_bytes]);
	x_rel = x + col_min;
	data_intro_bytes += 4;

	double * y_buf = t_y_buf[tnum];

	int * data_val_lanes_size;
	data_val_lanes_size = (typeof(data_val_lanes_size)) &(data_intro[data_intro_bytes]);
	data_intro_bytes += 4 * sizeof(*data_val_lanes_size);

	unsigned char * data_coords = &data_intro[data_intro_bytes];
	const uint64_t coords_bytes = (row_bits + col_bits + 7) >> 3;
	const uint64_t data_coords_bytes = coords_bytes * num_vals;

	unsigned char * data_val_lens = &data_coords[data_coords_bytes];
	const uint64_t data_val_lens_bytes = num_vals;

	__m256i data_val_lanes;
	data_val_lanes[0] = (long long) &data_val_lens[data_val_lens_bytes];
	data_val_lanes[1] = data_val_lanes[0] + data_val_lanes_size[0];
	data_val_lanes[2] = data_val_lanes[1] + data_val_lanes_size[1];
	data_val_lanes[3] = data_val_lanes[2] + data_val_lanes_size[2];
	const uint64_t data_val_lanes_bytes = data_val_lanes_size[0] + data_val_lanes_size[1] + data_val_lanes_size[2] + data_val_lanes_size[3];

	i = 0;

	union {
		__m256d d;
		__m256i u;
	} val;
	val.u = _mm256_set1_epi64x(0);

	for (i=0;i<num_vals_mul4;i+=4)
	{
		union {
			__m256d d;
			__m256i u;
		} diff;
		__m256i len;
		__m256i row_rel, col_rel;

		gather_coords_v(i, data_coords, coords_bytes, row_bits, col_bits, &row_rel, &col_rel);

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
		__m256i len_bits = _mm256_slli_epi64(len, 3ULL);

		diff.u = _mm256_set_epi64x(*((uint64_t *) data_val_lanes[3]), *((uint64_t *) data_val_lanes[2]), *((uint64_t *) data_val_lanes[1]), *((uint64_t *) data_val_lanes[0]));

		// data_val_lanes = data_val_lanes + len;
		data_val_lanes = _mm256_add_epi64(data_val_lanes, len);

		// __m256i mask = (1ULL << len_bits) - 1ULL;
		__m256i mask = _mm256_sub_epi64(_mm256_sllv_epi64(_mm256_set1_epi64x(1ULL), len_bits), _mm256_set1_epi64x(1ULL));
		// diff.u &= mask;
		diff.u = _mm256_and_si256(diff.u, mask);
		// diff.u <<= tz;
		diff.u = _mm256_sllv_epi64(diff.u, tz);

		// val.u = val.u + diff.u;
		val.u = _mm256_add_epi64(val.u, diff.u);

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

	for (i=num_vals_mul4;i<num_vals;i++)
	{
		union {
			double d;
			uint64_t u;
		} diff;
		uint64_t len;
		uint64_t row_rel, col_rel;
		long lane_id = i % 4;

		gather_coords(i, data_coords, coords_bytes, row_bits, col_bits, &row_rel, &col_rel);

		len = data_val_lens[i];

		uint64_t tz = (len >> 2ULL) & (~3ULL);
		len &= 15ULL;
		uint64_t len_bits = len << 3ULL;

		diff.u = *((uint64_t *) data_val_lanes[lane_id]);

		data_val_lanes[lane_id] = data_val_lanes[lane_id] + len;

		uint64_t mask = (1ULL << len_bits) - 1ULL;
		diff.u &= mask;
		diff.u <<= tz;

		val.u[lane_id] = val.u[lane_id] + diff.u;

		if (validate)
			window[i] = val.d[lane_id];
		else
		{
			y_rel[row_rel] += val.d[lane_id] * x_rel[col_rel];
			// switch (mult_add_select) {
				// case 0: mult_add_serial(x_rel, y_rel, val.d[lane_id], row_rel, col_rel); break;
				// case 1: mult_add_r0_res = mult_add_r0(mult_add_r0_res, x_rel, val.d[lane_id], col_rel); break;
				// case 2: mult_add_ybuf(num_rows, y_buf, x_rel, val.d[lane_id], row_rel, col_rel); break;
			// }
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
	return data_intro_bytes + data_coords_bytes + data_val_lens_bytes + data_val_lanes_bytes;
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
					return decompress_and_compute_kernel_sort_diff_base(buf, x, y, num_vals_out, validate, gather_coords_sparse_r1_c1, gather_coords_sparse_r1_c1_v, 0);
				case 16:
					return decompress_and_compute_kernel_sort_diff_base(buf, x, y, num_vals_out, validate, gather_coords_sparse_r1_c2, gather_coords_sparse_r1_c2_v, 0);
				case 24:
					return decompress_and_compute_kernel_sort_diff_base(buf, x, y, num_vals_out, validate, gather_coords_sparse_r1_c3, gather_coords_sparse_r1_c3_v, 0);
				case 32:
					return decompress_and_compute_kernel_sort_diff_base(buf, x, y, num_vals_out, validate, gather_coords_sparse_r1_c4, gather_coords_sparse_r1_c4_v, 0);
			}
		}
		else
		{
			switch (col_bits) {
				case 8:
					return decompress_and_compute_kernel_sort_diff_base(buf, x, y, num_vals_out, validate, gather_coords_sparse_r1_c1, gather_coords_sparse_r1_c1_v, 2);
				case 16:
					return decompress_and_compute_kernel_sort_diff_base(buf, x, y, num_vals_out, validate, gather_coords_sparse_r1_c2, gather_coords_sparse_r1_c2_v, 2);
				case 24:
					return decompress_and_compute_kernel_sort_diff_base(buf, x, y, num_vals_out, validate, gather_coords_sparse_r1_c3, gather_coords_sparse_r1_c3_v, 2);
				case 32:
					return decompress_and_compute_kernel_sort_diff_base(buf, x, y, num_vals_out, validate, gather_coords_sparse_r1_c4, gather_coords_sparse_r1_c4_v, 2);
			}
		}
	}

	/* Huge rows. */
	if (row_bits == 0)
	{
		// return decompress_and_compute_kernel_sort_diff_base(buf, x, y, num_vals_out, validate, gather_coords_sparse_r0, gather_coords_sparse_r0_v, 0);
		switch (col_bits) {
			case 8:
				return decompress_and_compute_kernel_sort_diff_base(buf, x, y, num_vals_out, validate, gather_coords_sparse_r0_c1, gather_coords_sparse_r0_c1_v, 0);
			case 16:
				return decompress_and_compute_kernel_sort_diff_base(buf, x, y, num_vals_out, validate, gather_coords_sparse_r0_c2, gather_coords_sparse_r0_c2_v, 0);
			case 24:
				return decompress_and_compute_kernel_sort_diff_base(buf, x, y, num_vals_out, validate, gather_coords_sparse_r0_c3, gather_coords_sparse_r0_c3_v, 0);
			case 32:
				return decompress_and_compute_kernel_sort_diff_base(buf, x, y, num_vals_out, validate, gather_coords_sparse_r0_c4, gather_coords_sparse_r0_c4_v, 0);
		}
	}

	switch (coords_bytes) {
		case 1:
			return decompress_and_compute_kernel_sort_diff_base(buf, x, y, num_vals_out, validate, gather_coords_dense_1, gather_coords_dense_1_v, 0);
		case 2:
			return decompress_and_compute_kernel_sort_diff_base(buf, x, y, num_vals_out, validate, gather_coords_dense_2, gather_coords_dense_2_v, 0);
		case 3:
			return decompress_and_compute_kernel_sort_diff_base(buf, x, y, num_vals_out, validate, gather_coords_dense_3, gather_coords_dense_3_v, 0);
		case 4:
			return decompress_and_compute_kernel_sort_diff_base(buf, x, y, num_vals_out, validate, gather_coords_dense_4, gather_coords_dense_4_v, 0);
	}

	return decompress_and_compute_kernel_sort_diff_base(buf, x, y, num_vals_out, validate, gather_coords_dense, gather_coords_dense_v, 0);

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

	for (i=0;i<num_vals;i++)
	{
		vals[permutation[i]] = window[i];
	}
	return bytes;
}

