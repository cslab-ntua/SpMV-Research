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
static double ** t_window;
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
compress_init_sort_diff(__attribute__((unused)) ValueType * vals, __attribute__((unused)) const long num_vals, const long packet_size)
{
	int num_threads = omp_get_max_threads();
	bits_u64_required_bits_for_binary_representation(packet_size - 1, &window_size_bits, &window_size);    // Maximum representable number we want is 'packet_size - 1'.
	printf("window_size = %ld , window_size_bits = %ld\n", window_size, window_size_bits);
	t_window = (typeof(t_window)) malloc(num_threads * sizeof(*t_window));
	t_rows_diff = (typeof(t_rows_diff)) malloc(num_threads * sizeof(*t_rows_diff));
	t_cols = (typeof(t_cols)) malloc(num_threads * sizeof(*t_cols));
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
		t_window[tnum] = (typeof(&(**t_window))) malloc(window_size * sizeof(**t_window));
		t_rows_diff[tnum] = (typeof(&(**t_rows_diff))) malloc(window_size * sizeof(**t_rows_diff));
		t_cols[tnum] = (typeof(&(**t_cols))) malloc(window_size * sizeof(**t_cols));
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
compress_kernel_sort_diff(INT_T * ia, INT_T * ja, ValueType * vals, long i_s, long j_s, unsigned char * buf, const long num_vals, long * num_vals_out)
{
	int tnum = omp_get_thread_num();
	double * window = t_window[tnum];
	unsigned int * rows_diff = t_rows_diff[tnum];
	unsigned int * cols = t_cols[tnum];
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
	long i, j, k;
	for (k=0;k<num_vals;k++)
		permutation[k] = k;
	quicksort(permutation, num_vals, vals, t_qsort_partitions[tnum]);
	for (k=0;k<num_vals;k++)
		rev_permutation[permutation[k]] = k;
	col_min = ja[j_s];
	col_max = ja[j_s];
	for (k=0,i=i_s,j=j_s; k<num_vals; k++,j++)
	{
		if (j >= ia[i+1])
			i++;
		window[rev_permutation[k]] = vals[k];
		rows_diff[rev_permutation[k]] = i - i_s;
		col = ja[j];
		cols[rev_permutation[k]] = col;
		if (col < col_min)
			col_min = col;
		if (col > col_max)
			col_max = col;
	}
	row_diff_max = i - i_s;
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

	for (k=0;k<num_vals;k++)
	{
		if (window[k] != vals[permutation[k]])
			error("wrong");
		if (rows_diff[k] + i_s > i)
			error("row index");
		if (cols[k] != (unsigned int) ja[j_s+permutation[k]])
			error("col index");
	}
	// if (tnum == 0) fprintf(stderr, "i=[%ld, %ld], j_s=[%ld, %ld], row_diff_max=%ld , row_bits=%ld , col_diff_max=%ld , col_bits=%ld\n", i_s, i, j_s, j, row_diff_max, row_bits, col_diff_max, col_bits);

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

	unsigned char * data_coords = &data_intro[data_intro_bytes];

	const uint64_t coords_bytes = row_col_bytes;

	const uint64_t data_coords_bytes = coords_bytes * num_vals;

	unsigned char * data_val_lens = &data_coords[data_coords_bytes];
	const uint64_t data_val_lens_bytes = num_vals;
	// if (tnum == 0) fprintf(stderr, "c: num_vals=%ld, row_min=%ld, col_min=%ld, row_bits=%ld, col_bits=%ld, val=%lf\n", num_vals, i_s, col_min, row_bits, col_bits, window[0]);

	unsigned char * bytestream = &data_val_lens[data_val_lens_bytes];

	for (i=0;i<num_vals;i++)
	{
		uint64_t coords;
		row_diff = rows_diff[i];
		col_diff = cols[i] - col_min;
		coords = row_diff | (col_diff << row_bits);
		*((uint64_t *) &data_coords[i*coords_bytes]) = coords;
	}

	bytestream_init_write(&Bs, bytestream);
	num_bytes = 0;

	double val_prev = window[0];
	for (i=1;i<num_vals;i++)
	{
		// uint64_t sign = window[i] ;
		// uint64_t bits_buf = 0;
		diff = window[i] - val_prev;
		val_prev = window[i];
		// if (diff + val_prev != window[i] && val_prev > 0)
		// {
			// double val = diff + val_prev;
			// error("\nwindow[i]   = %g %064lb\nval_prev = %g %064lb\ndiff        = %g %064lb\ndiff + val_prev = %g %064lb",
				// window[i]  , ((uint64_t *) window)[i],
				// val_prev, ((uint64_t *) window)[i-1],
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
			trailing_zeros = __builtin_ctzl(bits.u);
			uint64_t trailing_zero_bytes = trailing_zeros >> 3;
			len = 8 - trailing_zero_bytes;
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
	ValueType * x_rel;
	ValueType * y_rel;
	double * window = t_window[tnum];
	uint64_t row_rel = 0;
	struct Byte_Stream Bs;
	uint64_t len = 0;
	double diff = 0;
	double val = 0;
	long num_vals = 0;
	uint64_t row_min = 0, col_rel = 0, col_min = 0;
	uint64_t row_bits = 0, col_bits = 0;
	union {
		double d;
		uint64_t u;
	} bits;
	double x_val;
	long i;

	unsigned char * data_intro = buf;
	uint64_t data_intro_bytes = 0;
	num_vals = *((uint32_t *) &data_intro[data_intro_bytes]);
	data_intro_bytes += 4;

	row_min = *((uint32_t *) &data_intro[data_intro_bytes]);
	y_rel = y + row_min;
	data_intro_bytes += 4;
	col_min = *((uint32_t *) &data_intro[data_intro_bytes]);
	x_rel = x + col_min;
	data_intro_bytes += 4;

	row_bits = data_intro[data_intro_bytes];
	data_intro_bytes += 1;
	col_bits = data_intro[data_intro_bytes];
	data_intro_bytes += 1;

	val = *((double *) &data_intro[data_intro_bytes]);
	data_intro_bytes += 8;
	// if (tnum == 0) fprintf(stdout, "d: num_vals=%ld, row_min=%ld, col_min=%ld, row_bits=%ld, col_bits=%ld, val=%lf\n", num_vals, row_min, col_min, row_bits, col_bits, val);

	unsigned char * data_coords = &data_intro[data_intro_bytes];

	const uint64_t coords_bytes = (row_bits + col_bits + 7) >> 3;

	const uint64_t data_coords_bytes = coords_bytes * num_vals;

	unsigned char * data_val_lens = &data_coords[data_coords_bytes];
	const uint64_t data_val_lens_bytes = num_vals;

	unsigned char * bytestream = &data_val_lens[data_val_lens_bytes];

	uint64_t coords;
	coords = *((uint64_t *) &data_coords[0]);
	row_rel = bits_u64_extract(coords, 0, row_bits);
	col_rel = bits_u64_extract(coords, row_bits, col_bits);

	if (validate)
		window[0] = val;
	else
		y_rel[row_rel] += val * x_rel[col_rel];

	bytestream_init_read(&Bs, bytestream);

	for (i=1;i<num_vals;i++)
	{
		// uint64_t bits_buf = 0;
		coords = *((uint64_t *) &data_coords[i*coords_bytes]);
		row_rel = bits_u64_extract(coords, 0, row_bits);
		col_rel = bits_u64_extract(coords, row_bits, col_bits);

		if (!validate)
		{
			// x_val = 1;
			// x_val = x_rel[col_min];
			x_val = x_rel[col_rel];
		}

		len = data_val_lens[i];
		bits.u = 0;
		if (len > 0)
		{
			bytestream_read_unsafe_cast(&Bs, &bits.u, len);

			bits.u <<= (8 - len) << 3;
			diff = bits.d;
			val = val + diff;
		}
		// bits_buf = ((uint64_t *) bits);

		// if (tnum == 0) fprintf(stderr, "row_rel=%ld, col_rel=%ld, len=%ld, bit_pos=%ld, bits_buf=%064lb, bits=%064lb, diff=%g, val=%g\n", row_rel, col_rel, len, Bs.len_bits, bits_buf, bits.u, diff, val);

		if (validate)
			window[i] = val;
		else
		{
			// y_rel[row_rel] += val * x_rel[col_rel];
			y_rel[row_rel] += val * x_val;
		}
	}

	// if (tnum == 0) fprintf(stdout, "d: data_intro=%p (%ld), data_coords=%p (%ld), data_val_lens=%p (%ld), bytestream=%p (%ld), \n", data_intro, data_intro_bytes, data_coords, data_coords_bytes, data_val_lens, data_val_lens_bytes, bytestream, ((Bs.len_bits + 7) >> 3));

	if (num_vals_out != NULL)
		*num_vals_out = num_vals;
	return data_intro_bytes + data_coords_bytes + data_val_lens_bytes + Bs.len;
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
	for (i=0;i<num_vals;i++)
	{
		vals[permutation[i]] = window[i];
	}
	return bytes;

}

