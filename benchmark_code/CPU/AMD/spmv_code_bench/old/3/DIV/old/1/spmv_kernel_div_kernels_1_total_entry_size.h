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
static int ** t_rows;
static int ** t_cols;
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
		t_rows[tnum] = (int *) malloc(window_size * sizeof(**t_rows));
		t_cols[tnum] = (int *) malloc(window_size * sizeof(**t_cols));
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
	int * rows_diff = t_rows[tnum];
	int * cols = t_cols[tnum];
	int * permutation = t_permutation[tnum];
	int * rev_permutation = t_rev_permutation[tnum];
	struct Bit_Stream bs;
	uint64_t trailing_zeros = 0;
	uint64_t len = 0;
	double diff = 0;
	long num_bits = 0, entry_bits_total = 0;
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
		if (cols[k] != ja[j_s+permutation[k]])
			error("col index");
	}
	// printf("i=[%ld, %ld], j_s=[%ld, %ld], row_diff_max=%ld , row_bits=%ld , col_diff_max=%ld , col_bits=%ld\n", i_s, i, j_s, j, row_diff_max, row_bits, col_diff_max, col_bits);

	bitstream_init_write(&bs, buf);

	bitstream_write_unsafe_cast(&bs, num_vals, 32);
	num_bits += 32;
	bitstream_write_unsafe_cast(&bs, i_s, 32);
	num_bits += 32;
	bitstream_write_unsafe_cast(&bs, col_min, 32);
	num_bits += 32;

	bitstream_write_unsafe_cast(&bs, row_bits, 6);  // max 32
	num_bits += 6;
	bitstream_write_unsafe_cast(&bs, col_bits, 6);
	num_bits += 6;

	row_diff = rows_diff[0];
	col_diff = cols[0] - col_min;
	bitstream_write_unsafe_cast(&bs, row_diff, row_bits);
	num_bits += row_bits;
	bitstream_write_unsafe_cast(&bs, col_diff, col_bits);
	num_bits += col_bits;

	bitstream_write_unsafe_cast(&bs, window[0], 64);
	num_bits += 64;

	for (i=1;i<num_vals;i++)
	{
		entry_bits_total = row_bits + col_bits;

		diff = window[i] - window[i-1];
		if (diff != 0)
		{
			entry_bits_total += 12;
			ptr = &diff;   // C++ cancer.
			bits = *((uint64_t *) ptr);
			trailing_zeros = __builtin_ctzl(bits);
			len = 52 - trailing_zeros;
			if (trailing_zeros < 52)
				entry_bits_total += len;
		}
		bitstream_write_unsafe_cast(&bs, entry_bits_total, 8);
		num_bits += 8;
		long test = num_bits + entry_bits_total;

		row_diff = rows_diff[i];
		col_diff = cols[i] - col_min;
		bitstream_write_unsafe_cast(&bs, row_diff, row_bits);
		num_bits += row_bits;
		bitstream_write_unsafe_cast(&bs, col_diff, col_bits);
		num_bits += col_bits;

		if (diff != 0)
		{
			if (trailing_zeros >= 52)
			{
				bitstream_write_unsafe_cast(&bs, bits >> 52, 12);
				num_bits += 12;
			}
			else
			{
				bits >>= trailing_zeros;
				bitstream_write_unsafe_cast(&bs, bits, 12 + len);
				num_bits += 12 + len;
			}
		}
		if (test != num_bits)
			error("test=%ld num_bits=%ld", test, num_bits);
	}
	if (num_bits & 7)
	{
		len = 8 - (num_bits & 7);
		bitstream_write_unsafe_cast(&bs, 0ULL, len);
		num_bits += len;
	}
	bitstream_write_flush(&bs);

	if (num_vals_out != NULL)
		*num_vals_out = num_vals;

	return num_bits / 8;

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
	long len = 0;
	double diff = 0;
	double val = 0;
	long num_vals = 0;
	uint64_t row_diff = 0, row_min = 0, col = 0, col_diff = 0, col_min = 0;
	uint64_t row_bits = 0, col_bits = 0;
	uint64_t num_bits = 0,  num_bits_base = 0, entry_bits_total = 0;
	uint64_t bits;
	void * ptr;
	double x_val;
	long i;

	bitstream_init_read(&bs, buf);

	bitstream_read_unsafe_57_cast(&bs, &num_vals, 32);
	num_bits += 32;
	bitstream_read_unsafe_57_cast(&bs, &row_min, 32);
	num_bits += 32;
	bitstream_read_unsafe_57_cast(&bs, &col_min, 32);
	num_bits += 32;

	bitstream_read_unsafe_57_cast(&bs, &row_bits, 6);
	num_bits += 6;
	bitstream_read_unsafe_57_cast(&bs, &col_bits, 6);
	num_bits += 6;

	bitstream_read_unsafe_57_cast(&bs, &row_diff, row_bits);
	num_bits += row_bits;
	bitstream_read_unsafe_57_cast(&bs, &col_diff, col_bits);
	num_bits += col_bits;

	row = row_diff + row_min;
	col = col_diff + col_min;

	bitstream_read_unsafe_cast(&bs, &val, 64);
	num_bits += 64;

	const uint64_t rowcol_bits = row_bits + col_bits;

	if (validate)
		window[0] = val;
	else
		y[row] += val * x[col];

	for (i=1;i<num_vals;i++)
	{
		bitstream_read_unsafe_57_base_cast(&bs, num_bits, &entry_bits_total, 8);
		num_bits_base = num_bits + 8;
		num_bits += 8 + entry_bits_total;

		// bitstream_read_unsafe_57_base_cast(&bs, num_bits_base, &row_diff, row_bits);
		// num_bits_base += row_bits;
		// bitstream_read_unsafe_57_base_cast(&bs, num_bits_base, &col_diff, col_bits);
		// num_bits_base += col_bits;
		// row = row_diff + row_min;
		// col = col_diff + col_min;

		// if (row_diff != rows_diff[i])
			// error("row");
		// if (col != cols[i])
			// error("col");

		bitstream_read_unsafe_57_base_cast(&bs, num_bits_base, &bits, rowcol_bits);
		row_diff = __builtin_ia32_bextr_u64(bits, (row_bits << 8));
		col_diff = __builtin_ia32_bextr_u64(bits, (col_bits << 8) | row_bits);
		row = row_diff + row_min;
		col = col_diff + col_min;
		num_bits_base += rowcol_bits;

		if (!validate)
		{
			x_val = x[col];
		}

		len = entry_bits_total - row_bits - col_bits - 12;
		if (len >= 0)
		{
			uint64_t exp;
			bits = 0;
			if (len > 0)
			{
				bitstream_read_unsafe_57_base_cast(&bs, num_bits_base, &bits, len);
				num_bits_base += len;
			}
			bitstream_read_unsafe_57_base_cast(&bs, num_bits_base, &exp, 12);
			exp <<= 64 - 12;
			bits <<= 64 - 12 - len;
			bits = bits | exp;

			ptr = &bits;
			diff = *((double *) ptr);
			val = val + diff;
		}

		if (validate)
			window[i] = val;
		else
		{
			// y[row] += val * x[col];
			y[row] += val * x_val;
		}
	}

	if (num_vals_out != NULL)
		*num_vals_out = num_vals;
	return (num_bits + 7) / 8;
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

