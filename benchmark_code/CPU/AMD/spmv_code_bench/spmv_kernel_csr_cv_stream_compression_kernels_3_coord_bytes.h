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
//= Id
//==========================================================================================================================================


// static inline
// long
// compress_kernel_id(INT_T * ia, INT_T * ja, ValueType * vals, unsigned char * buf, const long num_vals)
// {
	// long i;
	// *((int *) buf) = num_vals;
	// buf += sizeof(int);
	// for (i=0;i<num_vals;i++)
		// ((ValueType *) buf)[i] = vals[i];
	// return sizeof(int) + num_vals * sizeof(ValueType);
// }


// static inline
// long
// decompress_and_compute_kernel_id(unsigned char * buf, ValueType * restrict x, ValueType * restrict y, long * num_vals_out)
// {
	// long i;
	// long num_vals = *((int *) buf);
	// buf += sizeof(int);
	// for (i=0;i<num_vals;i++)
		// vals[i] = ((ValueType *) buf)[i];
	// *num_vals_out = num_vals;
	// return sizeof(int) + num_vals * sizeof(ValueType);
// }


//==========================================================================================================================================
//= Float Casting
//==========================================================================================================================================


// static inline
// long
// compress_kernel_float(INT_T * ia, INT_T * ja, ValueType * vals, unsigned char * buf, const long num_vals)
// {
	// long i;
	// *((int *) buf) = num_vals;
	// buf += sizeof(int);
	// for (i=0;i<num_vals;i++)
		// ((float *) buf)[i] = (float) vals[i];
	// return sizeof(int) + num_vals * sizeof(float);
// }


// static inline
// long
// decompress_and_compute_kernel_float(unsigned char * buf, ValueType * restrict x, ValueType * restrict y, long * num_vals_out)
// {
	// long i;
	// long num_vals = *((int *) buf);
	// buf += sizeof(int);
	// for (i=0;i<num_vals;i++)
		// vals[i] = (ValueType) ((float *) buf)[i];
	// *num_vals_out = num_vals;
	// return sizeof(int) + num_vals * sizeof(float);
// }


//==========================================================================================================================================
//= Sorted Delta
//==========================================================================================================================================


void
find_bits_of_binary_representation(uint64_t max_val, uint64_t * bits_out, uint64_t * pow2_out)
{
	uint64_t bits, pow2;
	bits = 64 - __builtin_clzl(max_val);
	pow2 = 1ULL << bits;
	if (bits_out != NULL)
		*bits_out = bits;
	if (pow2_out != NULL)
		*pow2_out = pow2;
}


//------------------------------------------------------------------------------------------------------------------------------------------
//- Quicksort
//------------------------------------------------------------------------------------------------------------------------------------------

static uint64_t window_size_bits;
static uint64_t window_size;
// static uint64_t exp_table_n = 1ULL << 12;
// static long ** exp_table;
static double ** t_window;
static int ** t_permutation;
static int ** t_rev_permutation;
static unsigned int ** t_rows;
static unsigned int ** t_cols;
static int ** t_qsort_partitions;

uint64_t * t_total_row_diff_max;
uint64_t * t_total_col_diff_max;

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
compress_init_sort_diff(__attribute__((unused)) ValueType * vals, __attribute__((unused)) const long num_vals, __attribute__((unused)) const long packet_size)
{
	int num_threads = omp_get_max_threads();
	find_bits_of_binary_representation(packet_size - 1, &window_size_bits, &window_size);    // Maximum representable number we want is 'packet_size - 1'.
	printf("window_size = %ld , window_size_bits = %ld\n", window_size, window_size_bits);
	t_window = (typeof(t_window)) malloc(num_threads * sizeof(*t_window));
	t_rows = (typeof(t_rows)) malloc(num_threads * sizeof(*t_rows));
	t_cols = (typeof(t_cols)) malloc(num_threads * sizeof(*t_cols));
	t_permutation = (typeof(t_permutation)) malloc(num_threads * sizeof(*t_permutation));
	t_rev_permutation = (typeof(t_rev_permutation)) malloc(num_threads * sizeof(*t_rev_permutation));
	t_qsort_partitions = (typeof(t_qsort_partitions)) malloc(num_threads * sizeof(*t_qsort_partitions));
	t_total_row_diff_max = (typeof(t_total_row_diff_max)) malloc(num_threads * sizeof(*t_total_row_diff_max));
	t_total_col_diff_max = (typeof(t_total_col_diff_max)) malloc(num_threads * sizeof(*t_total_col_diff_max));
	#pragma omp parallel
	{
		int tnum = omp_get_thread_num();
		t_window[tnum] = (double *) malloc(window_size * sizeof(**t_window));
		t_rows[tnum] = (unsigned int *) malloc(window_size * sizeof(**t_rows));
		t_cols[tnum] = (unsigned int *) malloc(window_size * sizeof(**t_cols));
		t_permutation[tnum] = (int *) malloc(window_size * sizeof(**t_permutation));
		t_rev_permutation[tnum] = (int *) malloc(window_size * sizeof(**t_rev_permutation));
		t_qsort_partitions[tnum] = (int *) malloc(window_size * sizeof(**t_qsort_partitions));
		t_total_row_diff_max[tnum] = 0;
		t_total_col_diff_max[tnum] = 0;
	}


	// double t_window[num_vals];
	// int t_qsort_partitions[num_vals];
	// long i, j, k;
	// exp_table = (typeof(exp_table)) malloc(exp_table_n * sizeof(*exp_table));
	// for (j=0;j<num_vals;j++)
		// t_window[j] = vals[j];
	// quicksort(t_window, num_vals, NULL, t_qsort_partitions);
	// vals_diff_sorted_window[0] = t_window[0];
	// for (j=1;j<num_vals;j++)
		// vals_diff_sorted_window[j] = t_window[j] - t_window[j-1];
	// for (j=0;j<exp_table_n;j++)
		// exp_table[j] = 0;
	// for (j=0;j<num_vals;j++)
		// exp_table[(int) get_double_exponent_bits(vals_diff_sorted_window, j)] = 1;
	// k = 0;
	// for (j=0;j<exp_table_n;j++)
	// {
		// if (exp_table[j])
			// k++;
	// }
	// free(exp_table);
}


// void
// decompress_and_compute_init_sd(ValueType * vals, const long num_vals, const long packet_size)
// {
// }


static inline
long
compress_kernel_sort_diff(INT_T * ia, INT_T * ja, ValueType * vals, long i_s, long j_s, unsigned char * buf, const long num_vals, long * num_vals_out)
{
	int tnum = omp_get_thread_num();
	double * window = t_window[tnum];
	unsigned int * rows_diff = t_rows[tnum];
	unsigned int * cols = t_cols[tnum];
	int * permutation = t_permutation[tnum];
	int * rev_permutation = t_rev_permutation[tnum];
	struct Bit_Stream bs;
	uint64_t trailing_zeros = 0;
	uint64_t len = 0;
	double diff = 0;
	long num_bits = 0;
	// long exp_table_n = 1<<12;
	void * ptr;
	uint64_t bits = 0;
	uint64_t row_diff = 0, row_diff_max = 0, col = 0, col_diff = 0, col_min = 0, col_max = 0, col_diff_max = 0;
	uint64_t row_bits = 0, col_bits = 0;
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
	find_bits_of_binary_representation(row_diff_max, &row_bits, NULL);
	col_diff_max = col_max - col_min;
	find_bits_of_binary_representation(col_diff_max, &col_bits, NULL);

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

	const uint64_t coords_bytes = (row_bits + col_bits + 7) >> 3;

	const uint64_t data_coords_bytes = coords_bytes * num_vals;

	unsigned char * data_val_lens = &data_coords[data_coords_bytes];
	const uint64_t data_val_lens_bytes = num_vals;
	// if (tnum == 0) fprintf(stderr, "c: num_vals=%ld, row_min=%ld, col_min=%ld, row_bits=%ld, col_bits=%ld, val=%lf\n", num_vals, i_s, col_min, row_bits, col_bits, window[0]);

	unsigned char * bitstream = &data_val_lens[data_val_lens_bytes];

	for (i=0;i<num_vals;i++)
	{
		uint64_t coords;
		row_diff = rows_diff[i];
		col_diff = cols[i] - col_min;
		coords = row_diff | (col_diff << row_bits);
		*((uint64_t *) &data_coords[i*coords_bytes]) = coords;
	}

	bitstream_init_write(&bs, bitstream);
	num_bits = 0;

	for (i=1;i<num_vals;i++)
	{
		// uint64_t sign = window[i] ;
		// uint64_t bits_buf = 0;
		diff = window[i] - window[i-1];
		// if (diff + window[i-1] != window[i] && window[i-1] > 0)
		// {
			// double val = diff + window[i-1];
			// error("\nwindow[i]   = %g %064lb\nwindow[i-1] = %g %064lb\ndiff        = %g %064lb\ndiff + window[i-1] = %g %064lb",
				// window[i]  , ((uint64_t *) window)[i],
				// window[i-1], ((uint64_t *) window)[i-1],
				// diff       , *((uint64_t *) &diff),
				// val        , *((uint64_t *) &val));
		// }
		bits = 0;
		if (diff == 0)
		{
			len = 53;
			data_val_lens[i] = 53;   // Special 'len' code for zero value.
		}
		else
		{
			ptr = &diff;   // C++ cancer.
			bits = *((uint64_t *) ptr);
			// bits_buf = bits;
			trailing_zeros = __builtin_ctzl(bits);
			if (trailing_zeros >= 52)
			{
				len = 0;
				data_val_lens[i] = 0;
				bits >>= 52;
				bitstream_write_unsafe_cast(&bs, bits, 11);  // Differences are always positive (sorted values).
				num_bits += 11;
			}
			else
			{
				len = 52 - trailing_zeros;        // 64 can't be represented with a 6-bit number.
				data_val_lens[i] = len;
				bits >>= trailing_zeros;
				bitstream_write_unsafe_cast(&bs, bits, 11 + len);
				num_bits += 11 + len;
			}
		}
		// if (tnum == 0) fprintf(stdout, "row=%ld, col=%d, len=%ld, bit_pos=%ld, bits_buf=%064lb, bits=%064lb, diff=%g, val=%g\n", rows_diff[i] + i_s, cols[i], len, num_bits, bits_buf, bits, diff, window[i]);
	}
	if (num_bits & 7)
	{
		len = 8 - (num_bits & 7);
		bitstream_write_unsafe_cast(&bs, 0ULL, len);
		num_bits += len;
	}
	bitstream_write_flush(&bs);

	// if (tnum == 0) fprintf(stderr, "c: data_intro=%p (%ld), data_coords=%p (%ld), data_val_lens=%p (%ld), bitstream=%p (%ld), \n", data_intro, data_intro_bytes, data_coords, data_coords_bytes, data_val_lens, data_val_lens_bytes, bitstream, (num_bits >> 3));

	if (num_vals_out != NULL)
		*num_vals_out = num_vals;
	return data_intro_bytes + data_coords_bytes + data_val_lens_bytes + (num_bits >> 3);

	// for (i=0;i<exp_table_n;i++)
		// exp_table[i] = 0;
	// for (i=0;i<num_vals;i++)
		// exp_table[(int) get_double_exponent_bits(vals_diff_sorted_window, i)] = 1;
	// k = 0;
	// for (i=0;i<exp_table_n;i++)
	// {
		// if (exp_table[i])
			// k++;
	// }
}


static inline
long
decompress_and_compute_kernel_sort_diff_base(unsigned char * restrict buf, ValueType * restrict x, ValueType * restrict y, long * restrict num_vals_out, const int validate)
{
	int tnum = omp_get_thread_num();
	double * window = t_window[tnum];
	// unsigned int * rows_diff = t_rows[tnum];
	// unsigned int * cols = t_cols[tnum];
	uint64_t row = 0;
	struct Bit_Stream bs;
	uint64_t len = 0;
	double diff = 0;
	double val = 0;
	long num_vals = 0;
	uint64_t row_min = 0, col = 0, col_min = 0;
	uint64_t row_bits = 0, col_bits = 0;
	uint64_t bits;
	void * ptr;
	double x_val;
	long i;

	unsigned char * data_intro = buf;
	uint64_t data_intro_bytes = 0;
	num_vals = *((uint32_t *) &data_intro[data_intro_bytes]);
	data_intro_bytes += 4;
	row_min = *((uint32_t *) &data_intro[data_intro_bytes]);
	data_intro_bytes += 4;
	col_min = *((uint32_t *) &data_intro[data_intro_bytes]);
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

	unsigned char * bitstream = &data_val_lens[data_val_lens_bytes];

	uint64_t coords;
	coords = *((uint64_t *) &data_coords[0]);
	row = bits_u64_extract(coords, 0, row_bits) + row_min;
	col = bits_u64_extract(coords, row_bits, col_bits) + col_min;

	if (validate)
		window[0] = val;
	else
		y[row] += val * x[col];

	bitstream_init_read(&bs, bitstream);

	for (i=1;i<num_vals;i++)
	{
		// uint64_t bits_buf = 0;
		coords = *((uint64_t *) &data_coords[i*coords_bytes]);
		row = bits_u64_extract(coords, 0, row_bits) + row_min;
		col = bits_u64_extract(coords, row_bits, col_bits) + col_min;

		if (!validate)
		{
			x_val = x[col];
		}

		len = data_val_lens[i];
		diff = 0;
		bits = 0;
		if (len != 53)   // Special 'len' code for zero value.
		{
			uint64_t exp;
			exp = 0;
			bits = 0;
			if (len > 0)
			{
				bitstream_read_unsafe_57_cast(&bs, &bits, len);
			}
			bitstream_read_unsafe_57_cast(&bs, &exp, 11);
			// bits_buf = bits;
			exp <<= 64 - 12;
			bits <<= 64 - 12 - len;
			bits = bits | exp;

			ptr = &bits;
			diff = *((double *) ptr);
			val = val + diff;
		}
		// if (tnum == 0) fprintf(stderr, "row=%ld, col=%ld, len=%ld, bit_pos=%ld, bits_buf=%064lb, bits=%064lb, diff=%g, val=%g\n", row, col, len, bs.len_bits, bits_buf, bits, diff, val);

		if (validate)
			window[i] = val;
		else
		{
			// y[row] += val * x[col];
			y[row] += val * x_val;
		}
	}

	// if (tnum == 0) fprintf(stdout, "d: data_intro=%p (%ld), data_coords=%p (%ld), data_val_lens=%p (%ld), bitstream=%p (%ld), \n", data_intro, data_intro_bytes, data_coords, data_coords_bytes, data_val_lens, data_val_lens_bytes, bitstream, ((bs.len_bits + 7) >> 3));

	if (num_vals_out != NULL)
		*num_vals_out = num_vals;
	return data_intro_bytes + data_coords_bytes + data_val_lens_bytes + ((bs.len_bits + 7) >> 3);
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

