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

	#include "spmv_kernel_div_gather_coords.h"
	// #include "spmv_kernel_div_gather_coords_avx2.h"
#ifdef __cplusplus
}
#endif


//==========================================================================================================================================
//= Sorted Delta
//==========================================================================================================================================


static uint64_t window_size_bits;
static uint64_t window_size;
static double ** t_window;
static double ** t_window_buf;
static unsigned char *** t_data_lanes;
static int ** t_rev_permutation_buf;
static int ** t_permutation;
static int ** t_rev_permutation;
static unsigned int ** t_rows;
static unsigned int ** t_rows_buf;
static unsigned int ** t_cols;
static unsigned int ** t_cols_buf;
static int ** t_qsort_partitions;

uint64_t * t_row_bits_accum;
uint64_t * t_col_bits_accum;
uint64_t * t_row_col_bytes_accum;

struct cmp_data {
	unsigned int * rows;
	unsigned int * cols;
	double * vals;
};


//------------------------------------------------------------------------------------------------------------------------------------------
//- Quicksort
//------------------------------------------------------------------------------------------------------------------------------------------

#ifdef __cplusplus
extern "C"{
#endif
	#include "sort/quicksort/quicksort_gen_undef.h"
	#define QUICKSORT_GEN_TYPE_1  int
	#define QUICKSORT_GEN_TYPE_2  int
	#define QUICKSORT_GEN_TYPE_3  struct cmp_data
	#define QUICKSORT_GEN_SUFFIX  i_i_d
	#include "sort/quicksort/quicksort_gen.c"
#ifdef __cplusplus
}
#endif

static inline
int
quicksort_cmp(int a, int b, struct cmp_data * aux)
{
	[[gnu::unused]] unsigned int * rows = aux->rows;
	unsigned int * cols = aux->cols;
	double * vals = aux->vals;
	int ca=cols[a], cb=cols[b];
	double va=vals[a], vb=vals[b];
	int ret;
	if (va*vb <= 0)
		ret = (va > vb) ? 1 : (va < vb) ? -1 : 0;
	else
		ret = (fabs(va) > fabs(vb)) ? 1 : (fabs(va) < fabs(vb)) ? -1 : 0;
	if (ret == 0)
		ret = (ca > cb) ? 1 : (ca < cb) ? -1 : 0;
	return ret;
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
	t_window_buf = (typeof(t_window_buf)) malloc(num_threads * sizeof(*t_window_buf));
	t_window = (typeof(t_window)) malloc(num_threads * sizeof(*t_window));
	t_data_lanes = (typeof(t_data_lanes)) malloc(num_threads * sizeof(*t_data_lanes));
	t_rev_permutation_buf = (typeof(t_rev_permutation_buf)) malloc(num_threads * sizeof(*t_rev_permutation_buf));
	t_permutation = (typeof(t_permutation)) malloc(num_threads * sizeof(*t_permutation));
	t_rev_permutation = (typeof(t_rev_permutation)) malloc(num_threads * sizeof(*t_rev_permutation));
	t_rows = (typeof(t_rows)) malloc(num_threads * sizeof(*t_rows));
	t_rows_buf = (typeof(t_rows_buf)) malloc(num_threads * sizeof(*t_rows_buf));
	t_cols = (typeof(t_cols)) malloc(num_threads * sizeof(*t_cols));
	t_cols_buf = (typeof(t_cols_buf)) malloc(num_threads * sizeof(*t_cols_buf));
	t_qsort_partitions = (typeof(t_qsort_partitions)) malloc(num_threads * sizeof(*t_qsort_partitions));
	t_row_bits_accum = (typeof(t_row_bits_accum)) malloc(num_threads * sizeof(*t_row_bits_accum));
	t_col_bits_accum = (typeof(t_col_bits_accum)) malloc(num_threads * sizeof(*t_col_bits_accum));
	t_row_col_bytes_accum = (typeof(t_row_col_bytes_accum)) malloc(num_threads * sizeof(*t_row_col_bytes_accum));
	#pragma omp parallel
	{
		int tnum = omp_get_thread_num();
		long i;
		t_window_buf[tnum] = (typeof(&(**t_window_buf))) malloc(window_size * sizeof(**t_window_buf));
		t_window[tnum] = (typeof(&(**t_window))) malloc(window_size * sizeof(**t_window));
		t_data_lanes[tnum] = (typeof(&(**t_data_lanes))) malloc(4 * sizeof(**t_data_lanes));
		for (i=0;i<4;i++)
		{
			t_data_lanes[tnum][i] = (typeof(&(**t_data_lanes[i]))) malloc(8*window_size * sizeof(**t_data_lanes[i]));
		}
		t_rev_permutation_buf[tnum] = (typeof(&(**t_rev_permutation_buf))) malloc(window_size * sizeof(**t_rev_permutation_buf));
		t_permutation[tnum] = (typeof(&(**t_permutation))) malloc(window_size * sizeof(**t_permutation));
		t_rev_permutation[tnum] = (typeof(&(**t_rev_permutation))) malloc(window_size * sizeof(**t_rev_permutation));
		t_rows[tnum] = (typeof(&(**t_rows))) malloc(window_size * sizeof(**t_rows));
		t_rows_buf[tnum] = (typeof(&(**t_rows_buf))) malloc(window_size * sizeof(**t_rows_buf));
		t_cols[tnum] = (typeof(&(**t_cols))) malloc(window_size * sizeof(**t_cols));
		t_cols_buf[tnum] = (typeof(&(**t_cols_buf))) malloc(window_size * sizeof(**t_cols_buf));
		t_qsort_partitions[tnum] = (typeof(&(**t_qsort_partitions))) malloc(window_size * sizeof(**t_qsort_partitions));
		t_row_bits_accum[tnum] = 0;
		t_col_bits_accum[tnum] = 0;
		t_row_col_bytes_accum[tnum] = 0;
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
double
reduce_precision(double val_d, double tolerance)
{
	uint64_t trailing_zeros, tz_l, tz_g;
	uint64_t num_bits_precision, num_bits_drop, pow2, mask_inv;
	union {
		double d;
		uint64_t u;
	} val, val_new_l, val_new_g;
	double error;
	val.d = val_d;
	if (tolerance >= 1.0)
		error("tolerance given is >= 1.0 : %g", tolerance);
	if (tolerance == 0)
		return val.d;
	if (val.u == 0)
		return val.d;
	trailing_zeros = (val.u == 0) ? 64 : __builtin_ctzl(val.u);
	if (trailing_zeros >= 53)  // The fraction is already 0.
		return val.d;
	num_bits_precision = ceil(fabs(log2(tolerance)));
	if (num_bits_precision >= 52)  // Precision needed is >= double.
		return val.d;
	num_bits_drop = 52 - num_bits_precision;
	// error("num_bits_precision=%ld num_bits_drop=%ld", num_bits_precision, num_bits_drop);
	pow2 = 1ULL << num_bits_drop;
	mask_inv = pow2 - 1;
	val_new_l.u = val.u & ~mask_inv;
	val_new_g.u = val.u + (pow2 - 1);
	val_new_g.u &= ~mask_inv;
	error = fabs(val.d - val_new_l.d) / fabs(val.d);
	if (error > tolerance)
		error("val_new_l exceeded tolerance : %g", error);
	error = fabs(val.d - val_new_g.d) / fabs(val.d);
	if (error > tolerance)
		error("val_new_g exceeded tolerance : %g", error);
	tz_l = (val_new_l.u == 0) ? 64 : __builtin_ctzl(val_new_l.u);
	tz_g = (val_new_g.u == 0) ? 64 : __builtin_ctzl(val_new_g.u);
	if (tz_l > tz_g)
		return val_new_l.d;
	else
		return val_new_g.d;
}


/*
 * 'i_s' is certain to be the first non-empty row.
 */
static inline
long
compress_kernel_sort_diff(INT_T * row_ptr, INT_T * ja, ValueType * vals, long i_s, [[gnu::unused]] long i_t_s, [[gnu::unused]] long i_t_e, long j_s, unsigned char * buf, long num_vals, long * num_vals_out)
{
	int tnum = omp_get_thread_num();
	double * window = t_window[tnum];
	double * window_buf = t_window_buf[tnum];
	unsigned char ** data_val_lanes = t_data_lanes[tnum];
	int * data_val_lanes_size;
	unsigned int * rows = t_rows[tnum];
	unsigned int * rows_buf = t_rows_buf[tnum];
	unsigned int * cols = t_cols[tnum];
	unsigned int * cols_buf = t_cols_buf[tnum];
	int * rev_permutation_sort = t_rev_permutation_buf[tnum];
	int * permutation = t_permutation[tnum];
	int * rev_permutation = t_rev_permutation[tnum];
	struct Byte_Stream Bs[vec_len_pd];
	uint64_t len_bits = 0;
	uint64_t len = 0;
	uint64_t row_min, row_max, row_diff = 0, row_diff_max = 0, col = 0, col_min = 0, col_max = 0, col_diff = 0, col_diff_max = 0;   // row_max is the last row (i.e., inclusive: num_rows = row_max + 1 - i_s)
	uint64_t row_bits = 0, col_bits = 0;
	uint64_t coords;
	uint64_t coords_bytes = 0;
	long i, j, k, l;
	long lane_id;

	double tolerance = atof(getenv("VC_TOLERANCE"));

	long force_row_index_lte_1_byte = 1;
	long force_row_index_rounding = 0;

	row_min = i_s;   // 'i_s' is certain to be the first non-empty row.

	long new_num_vals = select_num_vals(row_ptr, row_min, j_s, num_vals, force_row_index_lte_1_byte);
	if (new_num_vals != 0)
		num_vals = new_num_vals;
	else
	{
		force_row_index_lte_1_byte = 0;
		// error("empty packet");
	}

	long num_vals_mod = num_vals % vec_len_pd;
	long num_vals_div = num_vals / vec_len_pd;

	col_min = ja[j_s];
	col_max = ja[j_s];
	for (k=0,i=row_min,j=j_s; k<num_vals; k++,j++)
	{
		while (j >= row_ptr[i+1])
			i++;
		if (tolerance > 0)
			window_buf[k] = reduce_precision(vals[k], tolerance);
		else
			window_buf[k] = vals[k];
		col = ja[j];
		if (col < col_min)
			col_min = col;
		if (col > col_max)
			col_max = col;
		rows_buf[k] = i;
		cols_buf[k] = col;
	}
	row_max = i;
	row_diff_max = row_max - row_min;
	bits_u64_required_bits_for_binary_representation(row_diff_max, &row_bits, NULL);
	if (force_row_index_lte_1_byte)
	{
		if (row_bits > 8)
			error("row_bits > 8 (%ld)", row_bits);
	}
	col_diff_max = col_max - col_min;
	bits_u64_required_bits_for_binary_representation(col_diff_max, &col_bits, NULL);
	coords_bytes = (row_bits + col_bits + 7) >> 3;

	/* Optimize row and col bits to bytes if possible.
	 * It should not increase 'coords_bytes', unless 'force_row_index_rounding' is set.
	 */
	if (row_bits % 8)
	{
		uint64_t cmpl = 8 - (row_bits % 8);
		uint64_t row_col_bytes_new = ((row_bits + cmpl) + col_bits + 7) >> 3;
		if (!force_row_index_rounding)
		{
			if (row_col_bytes_new == coords_bytes)
				row_bits += cmpl;
		}
		else
		{
			row_bits += cmpl;
			coords_bytes = row_col_bytes_new;
		}
	}
	col_bits = (coords_bytes << 3) - row_bits;
	if (col_bits > 32)
		error("col_bits > 32 (%ld)", col_bits);

	t_row_bits_accum[tnum] += row_bits;
	t_col_bits_accum[tnum] += col_bits;
	t_row_col_bytes_accum[tnum] += coords_bytes;

	/* Sort the packet values. */
	for (k=0;k<num_vals;k++)
		rev_permutation_sort[k] = k;
	struct cmp_data compare_data;
	compare_data.vals = window_buf;
	compare_data.rows = rows_buf;
	compare_data.cols = cols_buf;
	quicksort(rev_permutation_sort, num_vals, &compare_data, t_qsort_partitions[tnum]);
	for (k=0;k<num_vals;k++)
	{
		l = (k % vec_len_pd) * num_vals_div + k / vec_len_pd;   // interleave index
		l += (k % vec_len_pd < num_vals_mod) ? k % vec_len_pd : num_vals_mod;
		rev_permutation[k] = rev_permutation_sort[l];
	}

	for (k=0;k<num_vals;k++)
		permutation[rev_permutation[k]] = k;

	for (k=0,j=j_s;k<num_vals;k++,j++)
	{
		window[permutation[k]] = window_buf[k];
		rows[permutation[k]] = rows_buf[k];
		cols[permutation[k]] = cols_buf[k];
	}

	for (k=0;k<num_vals;k++)
	{
		if (window[k] != window_buf[rev_permutation[k]])
			error("wrong");
		if (rows[k] > row_max)
			error("row index");
		if (rev_permutation[k] >= num_vals) // extra fake values
			continue;
		if (cols[k] != (unsigned int) cols_buf[rev_permutation[k]])
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
	data_intro_bytes += vec_len_pd * sizeof(*data_val_lanes_size);

	unsigned char * data_coords = &data_intro[data_intro_bytes];
	const uint64_t data_coords_bytes = coords_bytes * num_vals;

	unsigned char * data_val_lens = &data_coords[data_coords_bytes];
	const uint64_t data_val_lens_bytes = num_vals;

	for (i=0;i<num_vals;i++)
	{
		row_diff = rows[i] - row_min;
		col_diff = cols[i] - col_min;
		coords = row_diff | (col_diff << row_bits);
		memcpy(&data_coords[i*coords_bytes], &coords, coords_bytes);
	}

	for (long iter=0;iter<vec_len_pd;iter++)
		bytestream_init_write(&Bs[iter], data_val_lanes[iter]);

	union {
		double d;
		uint64_t u;
	} val_prev[vec_len_pd], val, diff;
	for (long iter=0;iter<vec_len_pd;iter++)
		val_prev[iter].u = 0;
	for (i=0;i<num_vals;i++)
	{
		lane_id = i % vec_len_pd;
		val.d = window[i];
		diff.u = val.u - val_prev[lane_id].u;

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
			/* For the length values (0-8) 4 bits are needed.
			 *
			 * For the trailing zero bits values (0-63, 64 isn't needed) 6 bits are needed, but we only have 4 bits space remaining.
			 * so we keep shifts in strides of 4 (0, 4, 8, 12, ..., 60).
			 */
			uint64_t rem_tz;
			uint64_t buf_diff = diff.u;
			uint64_t leading_zeros = 0;
			uint64_t trailing_zeros = 0;
			uint64_t trailing_zero_bits_div4 = 0;
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
	for (long iter=0;iter<vec_len_pd;iter++)
		data_val_lanes_size[iter] = Bs[iter].len;

	unsigned char * ptr = &data_val_lens[data_val_lens_bytes];
	uint64_t data_val_lanes_bytes = 0;
	for (long iter=0;iter<vec_len_pd;iter++)
	{
		memcpy(ptr, data_val_lanes[iter], Bs[iter].len);
		ptr += Bs[iter].len;
		data_val_lanes_bytes += Bs[iter].len;
	}

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
mult_add_serial(ValueType * x_rel, ValueType * y_rel, vec_d_t val, vec_i_t row_rel, vec_i_t col_rel)
{
	for (long iter=0;iter<vec_len_pd;iter++)
		y_rel[row_rel[iter]] += val[iter] * x_rel[col_rel[iter]];
}


//==========================================================================================================================================
//= Decompress
//==========================================================================================================================================


static __attribute__((always_inline)) inline
long
decompress_and_compute_kernel_sort_diff_base(unsigned char * restrict buf, ValueType * restrict x, ValueType * restrict y, long * restrict num_vals_out, const int validate,
		void (* gather_coords)(long i, unsigned char * data_coords, const uint64_t coords_bytes, uint64_t row_bits, uint64_t col_bits, uint64_t * row_rel_out, uint64_t * col_rel_out),
		void (* gather_coords_v)(long i, unsigned char * data_coords, const uint64_t coords_bytes, uint64_t row_bits, uint64_t col_bits, vec_i_t * row_rel_out, vec_i_t * col_rel_out)
		)
{
	int tnum = omp_get_thread_num();
	ValueType * x_rel;
	ValueType * y_rel;
	double * window = t_window[tnum];
	long num_vals;
	uint64_t row_min, col_min;
	[[gnu::unused]] long num_rows;
	uint64_t row_bits, col_bits;
	long i;

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

	long num_vals_div = num_vals / vec_len_pd;
	long num_vals_multiple = num_vals_div * vec_len_pd;

	row_min = *((uint32_t *) &data_intro[data_intro_bytes]);
	y_rel = y + row_min;
	data_intro_bytes += 4;
	col_min = *((uint32_t *) &data_intro[data_intro_bytes]);
	x_rel = x + col_min;
	data_intro_bytes += 4;

	int * data_val_lanes_size;
	data_val_lanes_size = (typeof(data_val_lanes_size)) &(data_intro[data_intro_bytes]);
	data_intro_bytes += vec_len_pd * sizeof(*data_val_lanes_size);

	unsigned char * data_coords = &data_intro[data_intro_bytes];
	const uint64_t coords_bytes = (row_bits + col_bits + 7) >> 3;
	const uint64_t data_coords_bytes = coords_bytes * num_vals;

	unsigned char * data_val_lens = &data_coords[data_coords_bytes];
	const uint64_t data_val_lens_bytes = num_vals;

	vec_i_t data_val_lanes;
	data_val_lanes[0] = (long long) &data_val_lens[data_val_lens_bytes];
	uint64_t data_val_lanes_bytes = data_val_lanes_size[0];
	for (long iter=1;iter<vec_len_pd;iter++)
	{
		data_val_lanes[iter] = data_val_lanes[iter-1] + data_val_lanes_size[iter-1];
		data_val_lanes_bytes += data_val_lanes_size[iter];
	}

	i = 0;

	union {
		vec_d_t d;
		vec_i_t u;
	} val;
	val.u = vec_set1_epi64(0);

	for (i=0;i<num_vals_multiple;i+=vec_len_pd)
	{
		union {
			vec_d_t d;
			vec_i_t u;
		} diff;
		vec_i_t len;
		vec_i_t row_rel, col_rel;

		gather_coords_v(i, data_coords, coords_bytes, row_bits, col_bits, &row_rel, &col_rel);

		len = vec_set_iter_epi64(iter, data_val_lens[i+iter]);

		// vec_i_t tz = (len >> 2ULL) & (~3ULL);
		vec_i_t tz = vec_and_si(vec_srli_epi64(len, 2ULL), vec_set1_epi64(~3ULL));
		// len &= 15ULL;
		len = vec_and_si(len, vec_set1_epi64(15ULL));
		// len_bits = len << 3ULL;
		vec_i_t len_bits = vec_slli_epi64(len, 3ULL);

		diff.u = vec_set_iter_epi64(iter, *((uint64_t *) data_val_lanes[iter]));

		// data_val_lanes = data_val_lanes + len;
		data_val_lanes = vec_add_epi64(data_val_lanes, len);

		// vec_i_t mask = (1ULL << len_bits) - 1ULL;
		vec_i_t mask = vec_sub_epi64(vec_sllv_epi64(vec_set1_epi64(1ULL), len_bits), vec_set1_epi64(1ULL));
		// diff.u &= mask;
		diff.u = vec_and_si(diff.u, mask);
		// diff.u <<= tz;
		diff.u = vec_sllv_epi64(diff.u, tz);

		// val.u = val.u + diff.u;
		val.u = vec_add_epi64(val.u, diff.u);

		if (validate)
			vec_storeu_pd(&window[i], val.d);
		else
		{
			mult_add_serial(x_rel, y_rel, val.d, row_rel, col_rel);
		}
	}

	for (i=num_vals_multiple;i<num_vals;i++)
	{
		union {
			double d;
			uint64_t u;
		} diff;
		uint64_t len;
		uint64_t row_rel, col_rel;
		long lane_id = i % vec_len_pd;

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
		switch (col_bits) {
			case 8:
				return decompress_and_compute_kernel_sort_diff_base(buf, x, y, num_vals_out, validate, gather_coords_sparse_r1_c1, gather_coords_sparse_r1_c1_v);
			case 16:
				return decompress_and_compute_kernel_sort_diff_base(buf, x, y, num_vals_out, validate, gather_coords_sparse_r1_c2, gather_coords_sparse_r1_c2_v);
			case 24:
				return decompress_and_compute_kernel_sort_diff_base(buf, x, y, num_vals_out, validate, gather_coords_sparse_r1_c3, gather_coords_sparse_r1_c3_v);
			case 32:
				return decompress_and_compute_kernel_sort_diff_base(buf, x, y, num_vals_out, validate, gather_coords_sparse_r1_c4, gather_coords_sparse_r1_c4_v);
		}
	}

	/* Huge rows. */
	if (row_bits == 0)
	{
		// return decompress_and_compute_kernel_sort_diff_base(buf, x, y, num_vals_out, validate, gather_coords_sparse_r0, gather_coords_sparse_r0_v, 0);
		switch (col_bits) {
			case 8:
				return decompress_and_compute_kernel_sort_diff_base(buf, x, y, num_vals_out, validate, gather_coords_sparse_r0_c1, gather_coords_sparse_r0_c1_v);
			case 16:
				return decompress_and_compute_kernel_sort_diff_base(buf, x, y, num_vals_out, validate, gather_coords_sparse_r0_c2, gather_coords_sparse_r0_c2_v);
			case 24:
				return decompress_and_compute_kernel_sort_diff_base(buf, x, y, num_vals_out, validate, gather_coords_sparse_r0_c3, gather_coords_sparse_r0_c3_v);
			case 32:
				return decompress_and_compute_kernel_sort_diff_base(buf, x, y, num_vals_out, validate, gather_coords_sparse_r0_c4, gather_coords_sparse_r0_c4_v);
		}
	}

	switch (coords_bytes) {
		case 1:
			return decompress_and_compute_kernel_sort_diff_base(buf, x, y, num_vals_out, validate, gather_coords_dense_1, gather_coords_dense_1_v);
		case 2:
			return decompress_and_compute_kernel_sort_diff_base(buf, x, y, num_vals_out, validate, gather_coords_dense_2, gather_coords_dense_2_v);
		case 3:
			return decompress_and_compute_kernel_sort_diff_base(buf, x, y, num_vals_out, validate, gather_coords_dense_3, gather_coords_dense_3_v);
		case 4:
			return decompress_and_compute_kernel_sort_diff_base(buf, x, y, num_vals_out, validate, gather_coords_dense_4, gather_coords_dense_4_v);
	}

	return decompress_and_compute_kernel_sort_diff_base(buf, x, y, num_vals_out, validate, gather_coords_dense, gather_coords_dense_v);

}



static inline
long
decompress_and_compute_kernel_sort_diff(unsigned char * restrict buf, ValueType * restrict x, ValueType * restrict y, [[gnu::unused]] long i_t_s, [[gnu::unused]] long i_t_e)
{
	return decompress_and_compute_kernel_sort_diff_select(buf, x, y, NULL, 0);
}


static
long
decompress_kernel_sort_diff(ValueType * vals, unsigned char * restrict buf, [[gnu::unused]] long i_t_s, [[gnu::unused]] long i_t_e)
{
	long num_vals;
	int tnum = omp_get_thread_num();
	double * window = t_window[tnum];
	int * rev_permutation = t_rev_permutation[tnum];
	long i, bytes;
	bytes = decompress_and_compute_kernel_sort_diff_select(buf, NULL, NULL, &num_vals, 1);

	for (i=0;i<num_vals;i++)
	{
		vals[rev_permutation[i]] = window[i];
	}
	return bytes;
}

