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

static inline
int
quicksort_cmp(int a, int b, double * vals)
{
	return (vals[a] > vals[b]) ? 1 : (vals[a] < vals[b]) ? -1 : 0;
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
compress_kernel_sort_diff_select_num_vals(INT_T * ia, long i_s, long j_s, long num_vals)
{
	long i_e, j_e;
	long num_rows;
	long num_rows_max = 256;
	long num_vals_ret;
	long num_vals_min = (num_vals >= 64) ? 64 : num_vals;
	j_e = j_s + num_vals;
	i_e = i_s;
	while (ia[i_e] < j_e)
		i_e++;
	num_rows = i_e - i_s;
	if (num_rows > num_rows_max)
		i_e = i_s + num_rows_max;
	num_vals_ret = ia[i_e] - ia[i_s];
	if (num_vals_ret < num_vals_min)
		num_vals_ret = num_vals_min;
	if (num_vals_ret > num_vals)
		num_vals_ret = num_vals;
	return num_vals_ret;
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
	uint64_t trailing_zeros = 0;
	uint64_t len = 0;
	double diff = 0;
	union {
		double d;
		uint64_t u;
	} bits;
	uint64_t row_diff = 0, row_diff_max = 0, col = 0, col_diff = 0, col_min = 0, col_max = 0, col_diff_max = 0;
	uint64_t row_bits = 0, col_bits = 0;
	uint64_t row_col_bytes = 0;
	long i, row_max, j, k, l;   // row_max is the last row (i.e. inclusive: num_rows = row_max + 1 - i_s)

	num_vals = compress_kernel_sort_diff_select_num_vals(ia, i_s, j_s, num_vals);

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
	col_diff_max = col_max - col_min;
	bits_u64_required_bits_for_binary_representation(col_diff_max, &col_bits, NULL);
	row_col_bytes = (row_bits + col_bits + 7) >> 3;

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
	*((uint32_t *) &data_intro[data_intro_bytes]) = num_vals;
	data_intro_bytes += 4;
	*((uint32_t *) &data_intro[data_intro_bytes]) = i_s;
	data_intro_bytes += 4;
	*((uint32_t *) &data_intro[data_intro_bytes]) = col_min;
	data_intro_bytes += 4;

	*((uint32_t *) &data_intro[data_intro_bytes]) = row_diff_max + 1;
	data_intro_bytes += 4;

	data_intro[data_intro_bytes] = row_bits;
	data_intro_bytes += 1;
	data_intro[data_intro_bytes] = col_bits;
	data_intro_bytes += 1;

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

	double val_prev[4];
	val_prev[0] = window[0];
	val_prev[1] = window[1];
	val_prev[2] = window[2];
	val_prev[3] = window[3];
	for (i=4;i<num_vals_padded;i++)
	{
		diff = window[i] - val_prev[i%4];
		// diff = window[i];
		val_prev[i%4] = window[i];
		bits.u = 0;
		if (diff == 0)
		{
			len = 0;
			data_val_lens[i] = 0;
		}
		else
		{
			// Differences are always positive (sorted values) so we could ignore the last bit (shift left by 1),
			// but it will be a problem for decompression when trailing_zero_bytes == 0.
			bits.d = diff;
			trailing_zeros = (bits.u == 0) ? 64 : __builtin_ctzl(bits.u);
			uint64_t trailing_zero_bytes = trailing_zeros >> 3;
			len = 8 - trailing_zero_bytes;
			if (len > 8)
				error("bad len");
			// len = 8;
			data_val_lens[i] = len;
			bits.u >>= trailing_zero_bytes << 3;
			bytestream_write_unsafe_cast(&Bs[i%4], bits.u, len);
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
long
decompress_and_compute_kernel_sort_diff_base(unsigned char * restrict buf, ValueType * restrict x, ValueType * restrict y, long * restrict num_vals_out, const int validate)
{
	int tnum = omp_get_thread_num();
	ValueType * x_rel;
	ValueType * y_rel;
	double * window = t_window[tnum];
	double (* y_buf)[4] = (typeof(y_buf)) t_y_buf[tnum];
	__m256i row_rel, col_rel;
	// __m256i idx;
	__m256i len;
	__m256d diff;
	__m256d v_x, v_y;
	// __m256d v_tmp;
	__m256d val;
	long num_vals;
	uint64_t row_min, col_min;
	long num_rows;
	uint64_t row_bits, col_bits;
	union {
		__m256d d;
		__m256i u;
	} bits;
	long i;

	unsigned char * data_intro = buf;
	uint64_t data_intro_bytes = 0;
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

	row_bits = data_intro[data_intro_bytes];
	data_intro_bytes += 1;
	col_bits = data_intro[data_intro_bytes];
	data_intro_bytes += 1;

	val = _mm256_loadu_pd((double *) &data_intro[data_intro_bytes]);
	data_intro_bytes += 4*8;

	int * data_val_lanes_len;
	data_val_lanes_len = (typeof(data_val_lanes_len)) &(data_intro[data_intro_bytes]);
	data_intro_bytes += 4 * sizeof(*data_val_lanes_len);

	unsigned char * data_coords = &data_intro[data_intro_bytes];

	const uint64_t coords_bytes = (row_bits + col_bits + 7) >> 3;

	const uint64_t data_coords_bytes = coords_bytes * num_vals_padded;

	unsigned char * data_val_lens = &data_coords[data_coords_bytes];
	const uint64_t data_val_lens_bytes = num_vals_padded;

	// unsigned char * data_val_lanes[4];
	__m256i data_val_lanes;
	data_val_lanes[0] = (long long) &data_val_lens[data_val_lens_bytes];
	data_val_lanes[1] = data_val_lanes[0] + data_val_lanes_len[0];
	data_val_lanes[2] = data_val_lanes[1] + data_val_lanes_len[1];
	data_val_lanes[3] = data_val_lanes[2] + data_val_lanes_len[2];

	__m256i coords;
	__m256i row_bits_mask, col_bits_mask;
	row_bits_mask = _mm256_set1_epi64x((1<<row_bits) - 1);   // here '-' has precedence over '<<'!
	col_bits_mask = _mm256_set1_epi64x((1<<col_bits) - 1);

	i = 0;
	coords = _mm256_set_epi64x(*((uint64_t *) &data_coords[(i+3)*coords_bytes]), *((uint64_t *) &data_coords[(i+2)*coords_bytes]), *((uint64_t *) &data_coords[(i+1)*coords_bytes]), *((uint64_t *) &data_coords[i*coords_bytes]));

	row_rel = _mm256_and_si256(coords, row_bits_mask);
	col_rel = _mm256_and_si256(_mm256_srli_epi64(coords, row_bits), col_bits_mask);

	if (validate)
		_mm256_storeu_pd(&window[0], val);
	else
	{

		y_rel[row_rel[0]] += val[0] * x_rel[col_rel[0]];
		y_rel[row_rel[1]] += val[1] * x_rel[col_rel[1]];
		y_rel[row_rel[2]] += val[2] * x_rel[col_rel[2]];
		y_rel[row_rel[3]] += val[3] * x_rel[col_rel[3]];

		// y_buf[row_rel[0]][0] += val[0] * x_rel[col_rel[0]];
		// y_buf[row_rel[1]][1] += val[1] * x_rel[col_rel[1]];
		// y_buf[row_rel[2]][2] += val[2] * x_rel[col_rel[2]];
		// y_buf[row_rel[3]][3] += val[3] * x_rel[col_rel[3]];

	}

	// #pragma GCC unroll 4
	for (i=4;i<num_vals_padded;i+=4)
	{
		coords = _mm256_set_epi64x(*((uint64_t *) &data_coords[(i+3)*coords_bytes]), *((uint64_t *) &data_coords[(i+2)*coords_bytes]), *((uint64_t *) &data_coords[(i+1)*coords_bytes]), *((uint64_t *) &data_coords[i*coords_bytes]));

		row_rel = coords & row_bits_mask;
		// row_rel = _mm256_and_si256(coords, row_bits_mask);
		// col_rel = _mm256_and_si256(_mm256_srli_epi64(coords, row_bits), col_bits_mask);
		// col_rel = (coords >> row_bits) & col_bits_mask;
		// row_rel = _mm256_set_epi64x(bits_u64_extract(coords[3], 0, row_bits), bits_u64_extract(coords[2], 0, row_bits), bits_u64_extract(coords[1], 0, row_bits), bits_u64_extract(coords[0], 0, row_bits));
		col_rel = _mm256_set_epi64x(bits_u64_extract(coords[3], row_bits, col_bits), bits_u64_extract(coords[2], row_bits, col_bits), bits_u64_extract(coords[1], row_bits, col_bits), bits_u64_extract(coords[0], row_bits, col_bits));

		if (!validate)
		{
			// v_x = _mm256_set_pd(x_rel[col_rel[3]], x_rel[col_rel[2]], x_rel[col_rel[1]], x_rel[col_rel[0]]);
			// v_x = _mm256_i64gather_pd(x_rel, col_rel, 8);
		}

		len = _mm256_set_epi64x(data_val_lens[i+3], data_val_lens[i+2], data_val_lens[i+1], data_val_lens[i+0]);
		/* __m128i _mm_loadl_epi64 (__m128i const* mem_addr)
		 *     Load 64-bit integer from memory into the first element of dst.
		 *
		 * __m256i _mm256_cvtepu8_epi64 (__m128i a)
		 *     Zero extend packed unsigned 8-bit integers in the low 8 bytes of a to packed 64-bit integers, and store the results in dst.
		 */
		// len = _mm256_cvtepu8_epi64(_mm_loadl_epi64((__m128i *) &data_val_lens[i]));

		/* Test if all zeros (computes the bitwise and operation and sets ZF flag if all zeros)
		 * Fast if nearly always true.
		 * Slow otherwise.
		 */
		// if (!_mm256_testz_si256(len, len))
		{
			bits.u = _mm256_set_epi64x(*((uint64_t *) data_val_lanes[3]), *((uint64_t *) data_val_lanes[2]), *((uint64_t *) data_val_lanes[1]), *((uint64_t *) data_val_lanes[0]));

			data_val_lanes = data_val_lanes + len;

			bits.u <<= (8 - len) << 3;
			// const __m256i c_8 = _mm256_set1_epi64x(8);
			// __m256i shift_bits = _mm256_slli_epi64(_mm256_sub_epi64(c_8, len), 3);
			// bits.u = _mm256_sllv_epi64(bits.u, shift_bits);

			diff = bits.d;
			val = val + diff;
			// val = _mm256_add_pd(val, diff);
			// val = diff;
		}

		// if (tnum == 0) fprintf(stderr, "row_rel=%ld, col_rel=%ld, len=%ld, bit_pos=%ld, bits_buf=%064lb, bits=%064lb, diff=%g, val=%g\n", row_rel, col_rel, len, Bs.len_bits, bits_buf, bits.u, diff, val);

		if (validate)
			_mm256_storeu_pd(&window[i], val);
		else
		{

			// v_x = _mm256_set_pd(x_rel[col_rel[3]], x_rel[col_rel[2]], x_rel[col_rel[1]], x_rel[col_rel[0]]);
			// v_tmp = val * v_x;
			// y_buf[row_rel[0]] += v_tmp[0];
			// y_buf[row_rel[1]] += v_tmp[1];
			// y_buf[row_rel[2]] += v_tmp[2];
			// y_buf[row_rel[3]] += v_tmp[3];

			y_rel[row_rel[0]] += val[0] * x_rel[col_rel[0]];
			y_rel[row_rel[1]] += val[1] * x_rel[col_rel[1]];
			y_rel[row_rel[2]] += val[2] * x_rel[col_rel[2]];
			y_rel[row_rel[3]] += val[3] * x_rel[col_rel[3]];

			// v_x = _mm256_set_pd(x_rel[col_rel[3]], x_rel[col_rel[2]], x_rel[col_rel[1]], x_rel[col_rel[0]]);
			// v_y = _mm256_set_pd(y_buf[row_rel[3]][3], y_buf[row_rel[2]][2], y_buf[row_rel[1]][1], y_buf[row_rel[0]][0]);
			// v_y = _mm256_fmadd_pd(val, v_x, v_y);
			// y_buf[row_rel[0]][0] = v_y[0];
			// y_buf[row_rel[1]][1] = v_y[1];
			// y_buf[row_rel[2]][2] = v_y[2];
			// y_buf[row_rel[3]][3] = v_y[3];

		}
	}

	/* if (!validate)
	{
		for (i=0;i<num_rows;i++)
		{
			v_y = _mm256_loadu_pd(y_buf[i]);
			y_rel[i] += hsum256_pd(v_y);
			// _mm256_storeu_pd(y_buf[i], _mm256_set1_pd(0));
			// y_rel[i] += y_buf[i][0] + y_buf[i][1] + y_buf[i][2] + y_buf[i][3];
			y_buf[i][0] = 0;
			y_buf[i][1] = 0;
			y_buf[i][2] = 0;
			y_buf[i][3] = 0;
		}
	} */

	// printf("%d: test 2\n", tnum);
	// if (tnum == 0) fprintf(stdout, "d: data_intro=%p (%ld), data_coords=%p (%ld), data_val_lens=%p (%ld), bytestream=%p (%ld), \n", data_intro, data_intro_bytes, data_coords, data_coords_bytes, data_val_lens, data_val_lens_bytes, bytestream, ((Bs.len_bits + 7) >> 3));

	if (num_vals_out != NULL)
		*num_vals_out = num_vals;
	return data_intro_bytes + data_coords_bytes + data_val_lens_bytes + data_val_lanes_len[0] + data_val_lanes_len[1] + data_val_lanes_len[2] + data_val_lanes_len[3];
}


static inline
long
decompress_and_compute_kernel_sort_diff(unsigned char * restrict buf, ValueType * restrict x, ValueType * restrict y)
{
	return decompress_and_compute_kernel_sort_diff_base(buf, x, y, NULL, 0);
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
	bytes = decompress_and_compute_kernel_sort_diff_base(buf, NULL, NULL, &num_vals, 1);

	long div_padded = (num_vals + 3) / 4;
	long num_vals_padded = div_padded * 4;

	for (i=0;i<num_vals_padded;i++)
	{
		if (permutation[i] < num_vals)
			vals[permutation[i]] = window[i];
	}

	// static long once = 0;
	// #pragma omp master
	// {
		// if (!once)
		// {
			// once = 1;
			// printf("\n");
			// printf("\n");
			// printf("\n");
			// printf("\n");
			// for (i=0;i<num_vals_padded;i++)
				// printf("%g\n", window[i]);
			// printf("\n");
			// for (i=0;i<num_vals;i++)
				// printf("%g\n", vals[i]);
			// printf("\n");
			// printf("\n");
			// printf("\n");
			// printf("\n");
		// }
	// }

	return bytes;

}

