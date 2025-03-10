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
static int ** t_permutation_orig;
static int ** t_permutation;
static int ** t_rev_permutation;
static unsigned int ** t_rows_diff;
static unsigned int ** t_cols;
static int ** t_qsort_partitions;

uint64_t * t_total_row_diff_max;
uint64_t * t_total_col_diff_max;

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
	t_rows_diff = (typeof(t_rows_diff)) malloc(num_threads * sizeof(*t_rows_diff));
	t_cols = (typeof(t_cols)) malloc(num_threads * sizeof(*t_cols));
	t_permutation_orig = (typeof(t_permutation_orig)) malloc(num_threads * sizeof(*t_permutation_orig));
	t_permutation = (typeof(t_permutation)) malloc(num_threads * sizeof(*t_permutation));
	t_rev_permutation = (typeof(t_rev_permutation)) malloc(num_threads * sizeof(*t_rev_permutation));
	t_qsort_partitions = (typeof(t_qsort_partitions)) malloc(num_threads * sizeof(*t_qsort_partitions));
	t_total_row_diff_max = (typeof(t_total_row_diff_max)) malloc(num_threads * sizeof(*t_total_row_diff_max));
	t_total_col_diff_max = (typeof(t_total_col_diff_max)) malloc(num_threads * sizeof(*t_total_col_diff_max));
	t_row_bits_accum = (typeof(t_row_bits_accum)) malloc(num_threads * sizeof(*t_row_bits_accum));
	t_col_bits_accum = (typeof(t_col_bits_accum)) malloc(num_threads * sizeof(*t_col_bits_accum));
	t_row_col_bytes_accum = (typeof(t_row_col_bytes_accum)) malloc(num_threads * sizeof(*t_row_col_bytes_accum));
	#pragma omp parallel
	{
		int tnum = omp_get_thread_num();
		t_vals[tnum] = (typeof(&(**t_vals))) malloc(window_size * sizeof(**t_vals));
		t_rows[tnum] = (typeof(&(**t_rows))) malloc(window_size * sizeof(**t_rows));
		t_window[tnum] = (typeof(&(**t_window))) malloc(window_size * sizeof(**t_window));
		t_rows_diff[tnum] = (typeof(&(**t_rows_diff))) malloc(window_size * sizeof(**t_rows_diff));
		t_cols[tnum] = (typeof(&(**t_cols))) malloc(window_size * sizeof(**t_cols));
		t_permutation_orig[tnum] = (typeof(&(**t_permutation_orig))) malloc(window_size * sizeof(**t_permutation_orig));
		t_permutation[tnum] = (typeof(&(**t_permutation))) malloc(window_size * sizeof(**t_permutation));
		t_rev_permutation[tnum] = (typeof(&(**t_rev_permutation))) malloc(window_size * sizeof(**t_rev_permutation));
		t_qsort_partitions[tnum] = (typeof(&(**t_qsort_partitions))) malloc(window_size * sizeof(**t_qsort_partitions));
		t_total_row_diff_max[tnum] = 0;
		t_total_col_diff_max[tnum] = 0;
		t_row_bits_accum[tnum] = 0;
		t_col_bits_accum[tnum] = 0;
		t_row_col_bytes_accum[tnum] = 0;
	}
}


static inline
long
compress_kernel_sort_diff(INT_T * ia, INT_T * ja, ValueType * vals_unpadded, long i_s, long j_s, unsigned char * buf, const long num_vals, long * num_vals_out)
{
	int tnum = omp_get_thread_num();
	ValueType * vals = t_vals[tnum];
	ValueType * rows = t_rows[tnum];
	double * window = t_window[tnum];
	unsigned int * rows_diff = t_rows_diff[tnum];
	unsigned int * cols = t_cols[tnum];
	int * permutation_orig = t_permutation_orig[tnum];
	int * permutation = t_permutation[tnum];
	int * rev_permutation = t_rev_permutation[tnum];
	struct Byte_Stream Bs;
	uint64_t trailing_zeros = 0;
	uint64_t len = 0;
	double diff = 0;
	long num_bytes = 0;
	union {
		double d;
		uint64_t u;
	} bits;
	uint64_t row_diff = 0, row_diff_max = 0, col = 0, col_diff = 0, col_min = 0, col_max = 0, col_diff_max = 0;
	uint64_t row_bits = 0, col_bits = 0;
	uint64_t row_col_bytes = 0;
	long i, row_max, j, k, l;   // row_max is the last row (i.e. inclusive: num_rows = row_max + 1 - i_s)

	long div_padded = (num_vals + 3) / 4;
	long num_vals_padded = div_padded * 4;
	// if (num_vals != num_vals_padded)
		// printf("nv=%ld nvp=%ld\n", num_vals, num_vals_padded);

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

	if (row_diff_max > t_total_row_diff_max[tnum])
	{
		t_total_row_diff_max[tnum] = row_diff_max;
	}
	if (col_diff_max > t_total_col_diff_max[tnum])
	{
		t_total_col_diff_max[tnum] = col_diff_max;
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
			// for (i=0;i<num_vals_padded;i++)
				// printf("%g\n", vals[i]);
			// printf("\n");
			// printf("\n");
			// printf("\n");
			// printf("\n");
		// }
	// }

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

	unsigned char * data_coords = &data_intro[data_intro_bytes];

	const uint64_t coords_bytes = row_col_bytes;

	const uint64_t data_coords_bytes = coords_bytes * num_vals_padded;

	unsigned char * data_val_lens = &data_coords[data_coords_bytes];
	const uint64_t data_val_lens_bytes = num_vals_padded;
	// if (tnum == 0) fprintf(stderr, "c: num_vals=%ld, num_vals_padded=%ld, row_min=%ld, col_min=%ld, row_bits=%ld, col_bits=%ld, val=%lf\n", num_vals, num_vals_padded, i_s, col_min, row_bits, col_bits, window[0]);

	unsigned char * bytestream = &data_val_lens[data_val_lens_bytes];

	for (i=0;i<num_vals_padded;i++)
	{
		uint64_t coords;
		row_diff = rows_diff[i];
		col_diff = cols[i] - col_min;
		coords = row_diff | (col_diff << row_bits);
		*((uint64_t *) &data_coords[i*coords_bytes]) = coords;
	}

	bytestream_init_write(&Bs, bytestream);
	num_bytes = 0;

	double val_prev[4];
	val_prev[0] = window[0];
	val_prev[1] = window[1];
	val_prev[2] = window[2];
	val_prev[3] = window[3];
	for (i=4;i<num_vals_padded;i++)
	{
		// uint64_t sign = window[i] ;
		// uint64_t bits_buf = 0;

		diff = window[i] - val_prev[i%4];
		// diff = window[i];
		val_prev[i%4] = window[i];
		// if (diff + val_prev[i%4] != window[i] && val_prev[i%4] > 0)
		// {
			// double val = diff + val_prev[i%4];
			// error("\nwindow[i]   = %g %064lb\nval_prev = %g %064lb\ndiff        = %g %064lb\ndiff + val_prev[i%4] = %g %064lb",
				// window[i]  , ((uint64_t *) window)[i],
				// val_prev[i%4], ((uint64_t *) window)[i-1],
				// diff       , *((uint64_t *) &diff),
				// val        , *((uint64_t *) &val));
		// }
		bits.u = 0;
		if (diff == 0)
		{
			len = 0;
			data_val_lens[i] = 0;
		}
		else
		{
			// Differences are always positive (sorted values) so we could ignore the last bit (shift left by 1),
			// but it will be a problem for decompress when trailing_zero_bytes == 0.
			bits.d = diff;
			// bits_buf = bits.u;
			trailing_zeros = (bits.u == 0) ? 64 : __builtin_ctzl(bits.u);
			uint64_t trailing_zero_bytes = trailing_zeros >> 3;
			len = 8 - trailing_zero_bytes;
			if (len > 8)
				error("bad len");
			// len = 8;
			data_val_lens[i] = len;
			bits.u >>= trailing_zero_bytes << 3;
			bytestream_write_unsafe_cast(&Bs, bits.u, len);
			num_bytes += len;
		}
		// if (tnum == 0) fprintf(stdout, "row=%ld, col=%d, len=%ld, bit_pos=%ld, bits_buf=%064lb, bits=%064lb, diff=%g, val=%g\n", rows_diff[i] + i_s, cols[i], len, num_bytes, bits_buf, bits, diff, window[i]);
	}

	// if (tnum == 0) fprintf(stderr, "c: data_intro=%p (%ld), data_coords=%p (%ld), data_val_lens=%p (%ld), bytestream=%p (%ld), \n", data_intro, data_intro_bytes, data_coords, data_coords_bytes, data_val_lens, data_val_lens_bytes, bytestream, (num_bytes >> 3));

	if (num_vals_out != NULL)
		*num_vals_out = num_vals;

	return data_intro_bytes + data_coords_bytes + data_val_lens_bytes + num_bytes;
}


static inline
long
decompress_and_compute_kernel_sort_diff_base(unsigned char * restrict buf, ValueType * restrict x, ValueType * restrict y, long * restrict num_vals_out, const int validate)
{
	int tnum = omp_get_thread_num();
	double * window = t_window[tnum];
	const __m256i zi = _mm256_set1_epi64x(0);
	const __m256d zd = _mm256_set1_pd(0);
	__m256i row = zi;
	__m256i idx;
	__m256i len = zi;
	__m256i shift_bits = zi;
	__m256d diff = zd;
	__m256d v_x, v_tmp;
	__m256d val = zd;
	long num_vals = 0;
	__m256i row_min = zi, col = zi, col_min = zi;
	uint64_t row_bits = 0, col_bits = 0;
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

	row_min = _mm256_set1_epi64x(*((uint32_t *) &data_intro[data_intro_bytes]));
	data_intro_bytes += 4;
	col_min = _mm256_set1_epi64x(*((uint32_t *) &data_intro[data_intro_bytes]));
	data_intro_bytes += 4;

	row_bits = data_intro[data_intro_bytes];
	data_intro_bytes += 1;
	col_bits = data_intro[data_intro_bytes];
	data_intro_bytes += 1;

	val = _mm256_loadu_pd((double *) &data_intro[data_intro_bytes]);
	data_intro_bytes += 4*8;
	// if (tnum == 0) fprintf(stdout, "d: num_vals=%ld, num_vals_padded=%ld, row_min=%ld, col_min=%ld, row_bits=%ld, col_bits=%ld, val=%lf\n", num_vals, num_vals_padded, row_min, col_min, row_bits, col_bits, val);

	unsigned char * data_coords = &data_intro[data_intro_bytes];

	const uint64_t coords_bytes = (row_bits + col_bits + 7) >> 3;

	const uint64_t data_coords_bytes = coords_bytes * num_vals_padded;

	unsigned char * data_val_lens = &data_coords[data_coords_bytes];
	const uint64_t data_val_lens_bytes = num_vals_padded;

	unsigned char * bytestream = &data_val_lens[data_val_lens_bytes];

	__m256i coords;
	__m256i row_bits_mask, col_bits_mask;
	row_bits_mask = _mm256_set1_epi64x((1<<row_bits) - 1);   // here '-' has precedence over '<<'!
	col_bits_mask = _mm256_set1_epi64x((1<<col_bits) - 1);

	__m256i idx_coords = _mm256_set_epi64x(3*coords_bytes, 2*coords_bytes, coords_bytes, 0);
	__m256i idx_step_coords = _mm256_set1_epi64x(4 * coords_bytes);

	coords = _mm256_i64gather_epi64((long long *) data_coords, idx_coords, 1);
	idx_coords = _mm256_add_epi64(idx_coords, idx_step_coords);

	row = _mm256_add_epi64(_mm256_and_si256(coords, row_bits_mask), row_min);
	col = _mm256_add_epi64(_mm256_and_si256(_mm256_srli_epi64(coords, row_bits), col_bits_mask), col_min);

	if (validate)
		_mm256_storeu_pd(&window[0], val);
	else
	{
		v_x = _mm256_i64gather_pd(x, col, 8);
		v_tmp = _mm256_mul_pd(val, v_x);
		y[row[0]] += v_tmp[0];
		y[row[1]] += v_tmp[1];
		y[row[2]] += v_tmp[2];
		y[row[3]] += v_tmp[3];
	}

	uint64_t bs_offset = 0;
	for (i=4;i<num_vals_padded;i+=4)
	{
		// coords = _mm256_set_epi64x(*((uint64_t *) &data_coords[(i+3)*coords_bytes]), *((uint64_t *) &data_coords[(i+2)*coords_bytes]), *((uint64_t *) &data_coords[(i+1)*coords_bytes]), *((uint64_t *) &data_coords[i*coords_bytes]));
		coords = _mm256_i64gather_epi64((long long *) data_coords, idx_coords, 1);
		idx_coords = _mm256_add_epi64(idx_coords, idx_step_coords);

		row = _mm256_add_epi64(_mm256_and_si256(coords, row_bits_mask), row_min);
		col = _mm256_add_epi64(_mm256_and_si256(_mm256_srli_epi64(coords, row_bits), col_bits_mask), col_min);

		if (!validate)
		{
			v_x = _mm256_i64gather_pd(x, col, 8);
		}

		uint64_t len0, len1, len2, len3;
		uint64_t sum1, sum2, sum3, sum4;
		len0 = data_val_lens[i+0];
		len1 = data_val_lens[i+1];
		len2 = data_val_lens[i+2];
		len3 = data_val_lens[i+3];
		len = _mm256_set_epi64x(len3, len2, len1, len0);

		sum1 = len0;
		sum2 = sum1 + len1;
		sum3 = sum2 + len2;
		sum4 = sum3 + len3;

		idx = _mm256_set_epi64x(sum3, sum2, sum1, 0);
		idx = _mm256_add_epi64(idx, _mm256_set1_epi64x(bs_offset));

		bits.u = _mm256_i64gather_epi64((long long *) bytestream, idx, 1);
		bs_offset += sum4;

		// bits.u <<= (8 - len) << 3;
		const __m256i c_8 = _mm256_set1_epi64x(8);
		shift_bits = _mm256_slli_epi64(_mm256_sub_epi64(c_8, len), 3);
		bits.u = _mm256_sllv_epi64(bits.u, shift_bits);
		// if (shift_bits[0] != (8 - len0) << 3)
			// error("shift");
		// if (shift_bits[1] != (8 - len1) << 3)
			// error("shift");
		// if (shift_bits[2] != (8 - len2) << 3)
			// error("shift");
		// if (shift_bits[3] != (8 - len3) << 3)
			// error("shift");
		// bits.u[0] <<= (8 - len0) << 3;
		// bits.u[1] <<= (8 - len1) << 3;
		// bits.u[2] <<= (8 - len2) << 3;
		// bits.u[3] <<= (8 - len3) << 3;

		diff = bits.d;
		val = val + diff;
		// val = diff;

		// if (tnum == 0) fprintf(stderr, "row=%ld, col=%ld, len=%ld, bit_pos=%ld, bits_buf=%064lb, bits=%064lb, diff=%g, val=%g\n", row, col, len, Bs.len_bits, bits_buf, bits.u, diff, val);

		if (validate)
			_mm256_storeu_pd(&window[i], val);
		else
		{
			v_tmp = _mm256_mul_pd(val, v_x);
			y[row[0]] += v_tmp[0];
			y[row[1]] += v_tmp[1];
			y[row[2]] += v_tmp[2];
			y[row[3]] += v_tmp[3];
		}
	}

	// if (tnum == 0) fprintf(stdout, "d: data_intro=%p (%ld), data_coords=%p (%ld), data_val_lens=%p (%ld), bytestream=%p (%ld), \n", data_intro, data_intro_bytes, data_coords, data_coords_bytes, data_val_lens, data_val_lens_bytes, bytestream, ((Bs.len_bits + 7) >> 3));

	if (num_vals_out != NULL)
		*num_vals_out = num_vals;
	return data_intro_bytes + data_coords_bytes + data_val_lens_bytes + bs_offset;
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

