#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <omp.h>
// #include <x86intrin.h>

// #include <boost/sort/spreadsort/spreadsort.hpp>
// using namespace boost::sort::spreadsort;

#include "macros/cpp_defines.h"

#ifdef __cplusplus
extern "C"{
#endif
	#include "bit_ops.h"
	#include "bitstream.h"
	#include "bytestream.h"
	#include "macros/permutation.h"

	#include "spmv_kernel_div_gather_coords.h"
#ifdef __cplusplus
}
#endif


//==========================================================================================================================================
//= Sorted Delta
//==========================================================================================================================================


static uint64_t window_size_bits;
static uint64_t window_size;
static ValueType ** t_window;
static ValueType ** t_window_buf;
static INT_T ** t_window_ia;
static INT_T ** t_window_ja;
static int ** t_window_rf;
static int ** t_window_rf_buf;
static unsigned char *** t_data_lanes;
static int ** t_permutation;
static int ** t_rev_permutation;
static int ** t_permutation_buf;
static int ** t_rev_permutation_buf;
static int ** t_permutation_buf_2;
static int ** t_rev_permutation_buf_2;
static int ** t_offsets_rf;
static int ** t_rfs;
static int ** t_num_unique_vals_per_rf;
static unsigned int ** t_rows;
static unsigned int ** t_rows_buf;
static unsigned int ** t_cols;
static unsigned int ** t_cols_buf;
static int ** t_qsort_partitions;

uint64_t * t_row_bits_accum;
uint64_t * t_col_bits_accum;
uint64_t * t_row_col_bytes_accum;


// struct [[gnu::packed]] packet_header {
struct packet_header {
	uint32_t num_vals;
	uint32_t num_vals_unique;

	uint32_t num_rows;

	uint32_t row_min;
	uint32_t col_min;

	uint8_t row_bits;
	uint8_t col_bits;

	uint32_t num_rfs;
	uint32_t data_val_lanes_size[VEC_LEN];
};


struct cmp_data {
	unsigned int * rows;
	unsigned int * cols;
	ValueType * vals;
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
	ValueType * vals = aux->vals;
	int ca=cols[a], cb=cols[b];
	ValueType va=vals[a], vb=vals[b];
	int ret;
	if (va*vb <= 0)
		ret = (va > vb) ? 1 : (va < vb) ? -1 : 0;
	else
		ret = (fabs(va) > fabs(vb)) ? 1 : (fabs(va) < fabs(vb)) ? -1 : 0;
	if (ret == 0)
		ret =  (ca > cb) ? 1 : (ca < cb) ? -1 : 0;
	return ret;
}

/* static inline
int
quicksort_cmp(int a, int b, struct cmp_data * aux)
{
	[[gnu::unused]] unsigned int * rows = aux->rows;
	unsigned int * cols = aux->cols;
	ValueType * vals = aux->vals;
	int ra=rows[a], rb=rows[b];
	int ca=cols[a], cb=cols[b];
	[[gnu::unused]] int ra_mul = ra / 8, rb_mul = rb / 8;
	[[gnu::unused]] int ca_mul = ca / 256, cb_mul = cb / 256;
	ValueType va=vals[a], vb=vals[b];
	int ret = 0;
	// ret = (ra_mul > rb_mul) ? 1 : (ra_mul < rb_mul) ? -1 : 0;
	if (ret == 0)
	{
		ret = (ca_mul > cb_mul) ? 1 : (ca_mul < cb_mul) ? -1 : 0;
		if (ret == 0)
		{
			if (va*vb <= 0)
				ret = (va > vb) ? 1 : (va < vb) ? -1 : 0;
			else
				ret = (fabs(va) > fabs(vb)) ? 1 : (fabs(va) < fabs(vb)) ? -1 : 0;
			if (ret == 0)
				ret =  (ca > cb) ? 1 : (ca < cb) ? -1 : 0;
		}
	}
	return ret;
} */


//------------------------------------------------------------------------------------------------------------------------------------------
//- Bucketsort
//------------------------------------------------------------------------------------------------------------------------------------------

#include "sort/bucketsort/bucketsort_gen_undef.h"
#define BUCKETSORT_GEN_TYPE_1  int
#define BUCKETSORT_GEN_TYPE_2  int
#define BUCKETSORT_GEN_TYPE_3  int
#define BUCKETSORT_GEN_TYPE_4  void
#define BUCKETSORT_GEN_SUFFIX  _i_i_i_v
#include "sort/bucketsort/bucketsort_gen.c"

static inline
int
bucketsort_find_bucket(int a, __attribute__((unused)) void * unused)
{
	return a;
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//------------------------------------------------------------------------------------------------------------------------------------------
//-                                                             Compression                                                                -
//------------------------------------------------------------------------------------------------------------------------------------------
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


#define malloc_thread_local_2D(ptr, N)                                          \
do {                                                                            \
	int __num_threads = omp_get_max_threads();                              \
	/* ptr = (typeof(ptr)) malloc(__num_threads * sizeof(*ptr)); */         \
	ptr = (typeof(ptr)) aligned_alloc(64, __num_threads * sizeof(*ptr));    \
	_Pragma("omp parallel")                                                 \
	{                                                                       \
		int __tnum = omp_get_thread_num();                              \
		ptr[__tnum] = (typeof(&(**ptr))) malloc(N * sizeof(**ptr));     \
	}                                                                       \
} while (0)


void
compress_init_sort_diff(__attribute__((unused)) ValueType * vals, __attribute__((unused)) const long num_vals, long packet_size)
{
	int num_threads = omp_get_max_threads();
	packet_size = packet_size + packet_size % VEC_LEN;

	bits_u64_required_bits_for_binary_representation(packet_size - 1, &window_size_bits, &window_size);    // Maximum representable number we want is 'packet_size - 1'.
	printf("window_size = %ld , window_size_bits = %ld\n", window_size, window_size_bits);
	malloc_thread_local_2D(t_window, window_size);
	malloc_thread_local_2D(t_window_buf, window_size);
	malloc_thread_local_2D(t_window_ia, window_size);
	malloc_thread_local_2D(t_window_ja, window_size);
	malloc_thread_local_2D(t_window_rf, window_size);
	malloc_thread_local_2D(t_window_rf_buf, window_size);
	malloc_thread_local_2D(t_data_lanes, VEC_LEN);
	malloc_thread_local_2D(t_permutation, window_size);
	malloc_thread_local_2D(t_rev_permutation, window_size);
	malloc_thread_local_2D(t_permutation_buf, window_size);
	malloc_thread_local_2D(t_rev_permutation_buf, window_size);
	malloc_thread_local_2D(t_permutation_buf_2, window_size);
	malloc_thread_local_2D(t_rev_permutation_buf_2, window_size);
	malloc_thread_local_2D(t_offsets_rf, (window_size + 1));   // These are offsets, so we need + 1.
	malloc_thread_local_2D(t_rfs, window_size);
	malloc_thread_local_2D(t_num_unique_vals_per_rf, window_size);
	malloc_thread_local_2D(t_rows, window_size);
	malloc_thread_local_2D(t_rows_buf, window_size);
	malloc_thread_local_2D(t_cols, window_size);
	malloc_thread_local_2D(t_cols_buf, window_size);
	malloc_thread_local_2D(t_qsort_partitions, window_size);
	t_row_bits_accum = (typeof(t_row_bits_accum)) malloc(num_threads * sizeof(*t_row_bits_accum));
	t_col_bits_accum = (typeof(t_col_bits_accum)) malloc(num_threads * sizeof(*t_col_bits_accum));
	t_row_col_bytes_accum = (typeof(t_row_col_bytes_accum)) malloc(num_threads * sizeof(*t_row_col_bytes_accum));
	#pragma omp parallel
	{
		int tnum = omp_get_thread_num();
		long i;
		for (i=0;i<VEC_LEN;i++)
		{
			t_data_lanes[tnum][i] = (typeof(&(**t_data_lanes[i]))) malloc(8*window_size * sizeof(**t_data_lanes[i]));
		}
		t_row_bits_accum[tnum] = 0;
		t_col_bits_accum[tnum] = 0;
		t_row_col_bytes_accum[tnum] = 0;
	}
}


static inline
long
select_num_vals(INT_T * row_ptr, long i_s, long j_s, long num_vals)
{
	long i_e, j_e;
	long num_rows;
	long num_rows_max = 255;
	long num_vals_ret;
	j_e = j_s + num_vals;
	i_e = i_s;
	while (row_ptr[i_e] < j_e)
		i_e++;
	num_rows = i_e - i_s;
	if (num_rows > num_rows_max)
		i_e = i_s + num_rows_max;
	num_vals_ret = row_ptr[i_e] - j_s;
	if (num_vals_ret > num_vals)
		num_vals_ret = num_vals;
	return num_vals_ret;
}


static inline
long
select_num_vals_complete_last_row(INT_T * row_ptr, long i_s, long j_s, long num_vals)
{
	long i_e, j_e;
	long num_rows;
	long num_rows_max = 255;
	long num_vals_ret;
	j_e = j_s + num_vals;
	i_e = i_s;
	while (row_ptr[i_e] < j_e)
		i_e++;
	if ((j_e != row_ptr[i_e]) && (i_e - 1 > i_s))
	{
		i_e--;
		j_e = row_ptr[i_e];
	}
	num_rows = i_e - i_s;
	if (num_rows > num_rows_max)
		i_e = i_s + num_rows_max;
	num_vals_ret = row_ptr[i_e] - j_s;
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


void
test_permutation(long num_vals, ValueType * vals, ValueType * window, int * rev_permutation, char * msg)
{
	int tnum = omp_get_thread_num();
	long i, j;
	for (j=0;j<num_vals;j++)
	{
		if (rev_permutation[j] < 0 || rev_permutation[j] > num_vals)
			error("permutation values out of bounds");
		if (window[j] != vals[rev_permutation[j]])
		{
			if (tnum == 0)
			{
				for (i=0;i<num_vals;i++)
				{
					printf("% 30.20g  % 30.20g\n", window[i], vals[rev_permutation[i]]);
				}
				printf("\n");
			}
			error("wrong permutation: %s", msg);
		}
	}
}


static inline
long
compress_kernel_sort_diff(INT_T * row_ptr, INT_T * ja, ValueType * vals, long i_s, [[gnu::unused]] long i_t_s, [[gnu::unused]] long i_t_e, long j_s, unsigned char * buf, long num_vals, long * num_vals_out)
{
	int tnum = omp_get_thread_num();
	long num_vals_unique;
	ValueType * window = t_window[tnum];
	ValueType * window_buf = t_window_buf[tnum];
	int * window_rf = t_window_rf[tnum];
	int * window_rf_buf = t_window_rf_buf[tnum];
	struct packet_header * header;
	unsigned char ** data_val_lanes = t_data_lanes[tnum];
	uint32_t * data_val_lanes_size;
	unsigned int * rows = t_rows[tnum];
	unsigned int * rows_buf = t_rows_buf[tnum];
	unsigned int * cols = t_cols[tnum];
	unsigned int * cols_buf = t_cols_buf[tnum];
	int * permutation = t_permutation[tnum];
	int * rev_permutation = t_rev_permutation[tnum];
	// int * permutation_buf = t_permutation_buf[tnum];
	int * rev_permutation_buf = t_rev_permutation_buf[tnum];
	int * permutation_buf_2 = t_permutation_buf_2[tnum];
	int * rev_permutation_buf_2 = t_rev_permutation_buf_2[tnum];
	int * offsets_rf = t_offsets_rf[tnum];
	int * rfs = t_rfs[tnum];
	int * num_unique_vals_per_rf = t_num_unique_vals_per_rf[tnum];
	struct Byte_Stream Bs[VEC_LEN];
	uint64_t len_bits = 0;
	uint64_t len = 0;
	uint64_t row_min, row_max, row_diff = 0, row_diff_max = 0, col = 0, col_min = 0, col_max = 0, col_diff = 0, col_diff_max = 0;   // row_max is the last row (i.e., inclusive: num_rows = row_max + 1 - i_s)
	uint64_t row_bits = 0, col_bits = 0;
	uint64_t coords;
	uint64_t row_bytes = 0, col_bytes = 0, coords_bytes = 0;
	long num_rfs;
	long num_rf_vals, num_rf_vals_unique;
	long i, j, k, k_s, l, l_s, l_e;
	long rf;
	long lane_id;

	double tolerance = atof(getenv("VC_TOLERANCE"));

	// printf("vec_len = %d\n", VEC_LEN);

	if (num_vals <= 0)
		error("packet with no nnz requested");
	row_min = i_s;
	// while (row_ptr[row_min+1] - row_ptr[row_min] == 0)
		// row_min++;

	// long new_num_vals = select_num_vals(row_ptr, row_min, j_s, num_vals);
	long new_num_vals = select_num_vals_complete_last_row(row_ptr, row_min, j_s, num_vals);
	if (new_num_vals != 0)
		num_vals = new_num_vals;
	else
	{
		// error("empty packet");
	}

	col_min = ja[j_s];
	col_max = ja[j_s];
	for (k=0,i=row_min,j=j_s; k<num_vals; k++,j++)
	{
		while (j >= row_ptr[i+1])
			i++;
		rev_permutation[k] = k;
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
	if (row_bits > 8)
		error("row_bits > 8 (%ld)", row_bits);
	if (row_bits > 0)   // Only 8 or 0 bits.
		row_bits = 8;
	row_bytes = row_bits >> 3;

	col_diff_max = col_max - col_min;
	bits_u64_required_bits_for_binary_representation(col_diff_max, &col_bits, NULL);
	if (col_bits > 32)
		error("col_bits > 32 (%ld)", col_bits);
	if (col_bits == 0)
		col_bits = 8;
	col_bytes = (col_bits + 7) >> 3;   // Byte granularity.
	col_bits = col_bytes << 3;

	coords_bytes = row_bytes + col_bytes;

	t_row_bits_accum[tnum] += row_bits;
	t_col_bits_accum[tnum] += col_bits;
	t_row_col_bytes_accum[tnum] += coords_bytes;

	/* Sort the packet values. */
	struct cmp_data compare_data;
	compare_data.vals = window_buf;
	compare_data.rows = rows_buf;
	compare_data.cols = cols_buf;
	quicksort(rev_permutation, num_vals, &compare_data, t_qsort_partitions[tnum]);
	// integer_sort(&(rev_permutation[0]), &(rev_permutation[0])+num_vals, [window_buf] (const int& x, const int& y) { return quicksort_cmp(x, y, window_buf) < 0; });
	// std::sort(&(rev_permutation[0]), &(rev_permutation[0])+num_vals, [window_buf] (const int& x, const int& y) { return quicksort_cmp(x, y, window_buf) < 0; });
	for (k=0;k<num_vals;k++)
	{
		// printf("%d\n", rev_permutation[k]);
		window[k] = window_buf[rev_permutation[k]];
	}
	// exit(0);

	if (tolerance == 0)
		test_permutation(num_vals, vals, window, rev_permutation, (char *) "quicksort");

	/* Find replication factors, but keep all duplicates (needed because each one has different coordinates). */
	// long rf_threshold_max = (1ULL<<16) - 1;
	// long rf_threshold_min = 8;
	long max_rf = 1;
	num_vals_unique = 0;
	k = 0;
	j = 0;
	while (j<num_vals)
	{
		rf = 1;
		j++;
		for (;j<num_vals;j++)
		{
			if (window[j] != window[j-1])
				break;
			// if (k - j > rf_threshold_max)
				// break;
			rf++;
		}
		// if (rf < rf_threshold_min)
		// {
			// num_vals_unique += rf - 1;
			// rf = 1;
		// }
		for (l=k;l<j;l++)
		{
			window_rf[l] = rf;
		}
		if (rf > max_rf)
			max_rf = rf;
		k = j;
		num_vals_unique++;
	}

	/* Stable sort by replication factors. */
	long num_buckets = max_rf + 1;
	bucketsort_stable_serial(window_rf, num_vals, num_buckets, NULL, permutation_buf_2, offsets_rf, NULL);
	for (i=0;i<num_vals;i++)
		rev_permutation_buf_2[permutation_buf_2[i]] = i;

	for (k=0;k<num_vals;k++)
	{
		rev_permutation_buf[permutation_buf_2[k]] = rev_permutation[k];
		window_buf[permutation_buf_2[k]] = window[k];
		window_rf_buf[permutation_buf_2[k]] = window_rf[k];
	}
	macros_swap(&rev_permutation, &rev_permutation_buf);
	macros_swap(&window, &window_buf);
	macros_swap(&window_rf, &window_rf_buf);

	if (tolerance == 0)
		test_permutation(num_vals, vals, window, rev_permutation, (char *) "bucketsort");

	/* Filter out duplicate values. */
	k = 0;
	j = 0;
	while (j<num_vals)
	{
		window[k] = window[j];
		rf = window_rf[j];
		window_rf[k] = rf;
		k++;
		j += rf;
	}

	/* For each replication factor, split to lanes. */
	num_rfs = 0;
	k = 0;
	for (i=0;i<max_rf+1;i++)
	{
		rf = i;
		l_s = offsets_rf[i];
		l_e = offsets_rf[i+1];
		num_rf_vals = l_e - l_s;
		if (num_rf_vals == 0)
			continue;
		rfs[num_rfs] = rf;
		num_rf_vals_unique = num_rf_vals / rf;
		num_unique_vals_per_rf[num_rfs] = num_rf_vals_unique;
		num_rfs++;

		permutation_interleave(window + k, window_buf + k, num_rf_vals_unique, VEC_LEN, 1, 0, 1);
		permutation_interleave(window_rf + k, window_rf_buf + k, num_rf_vals_unique, VEC_LEN, 1, 0, 1);
		permutation_interleave(rev_permutation + l_s, rev_permutation_buf + l_s, num_rf_vals, VEC_LEN, rf, 0, 1);

		k += num_rf_vals_unique;
	}
	macros_swap(&window, &window_buf);
	macros_swap(&window_rf, &window_rf_buf);
	macros_swap(&rev_permutation, &rev_permutation_buf);

	for (k=0;k<num_vals;k++)
		permutation[rev_permutation[k]] = k;

	for (k=0;k<num_vals;k++)
	{
		rows[permutation[k]] = rows_buf[k];
		cols[permutation[k]] = cols_buf[k];
	}

	for (k=0;k<num_vals;k++)
	{
		if (rows[k] > row_max)
			error("row index");
		if (rev_permutation[k] >= num_vals) // extra fake values
			continue;
		if (cols[k] != (unsigned int) cols_buf[rev_permutation[k]])
			error("col index");
	}

	unsigned char * data_intro = buf;
	uint64_t data_intro_bytes = sizeof(*header);

	header = (typeof(header)) buf;

	header->row_bits = row_bits;
	header->col_bits = col_bits;
	header->num_vals = num_vals;
	header->num_vals_unique = num_vals_unique;
	header->num_rows = row_diff_max + 1;
	header->row_min = row_min;
	header->col_min = col_min;
	header->num_rfs = num_rfs;

	data_val_lanes_size = header->data_val_lanes_size;

	uint32_t * data_rfs = (uint32_t *) &data_intro[data_intro_bytes];
	const uint64_t data_rfs_bytes = sizeof(*data_rfs) * num_rfs;

	uint32_t * data_num_unique_vals_per_rf = (uint32_t *) &((unsigned char *) data_rfs)[data_rfs_bytes];
	const uint64_t data_num_unique_vals_per_rf_bytes = sizeof(*data_num_unique_vals_per_rf) * num_rfs;

	for (i=0;i<num_rfs;i++)
	{
		data_rfs[i] = rfs[i];
		data_num_unique_vals_per_rf[i] = num_unique_vals_per_rf[i];
	}

	unsigned char * data_coords = &((unsigned char *) data_num_unique_vals_per_rf)[data_num_unique_vals_per_rf_bytes];
	const uint64_t data_coords_bytes = coords_bytes * num_vals;

	unsigned char * data_val_lens = &data_coords[data_coords_bytes];
	const uint64_t data_val_lens_bytes = num_vals_unique;

	for (i=0;i<num_vals;i++)
	{
		row_diff = rows[i] - row_min;
		col_diff = cols[i] - col_min;
		coords = row_diff | (col_diff << row_bits);
		memcpy(&data_coords[i*coords_bytes], &coords, coords_bytes);
	}

	for (long iter=0;iter<VEC_LEN;iter++)
		bytestream_init_write(&Bs[iter], data_val_lanes[iter]);

	union {
		ValueType d;
		ValueTypeI u;
	} val_prev[VEC_LEN], val, diff;
	k_s = 0;
	for (l=0;l<num_rfs;l++)
	{
		rf = rfs[l];
		num_rf_vals_unique = num_unique_vals_per_rf[l];
		for (long iter=0;iter<VEC_LEN;iter++)
			val_prev[iter].u = 0;
		for (i=0;i<num_rf_vals_unique;i++)
		{
			k = k_s + i;
			lane_id = i % VEC_LEN;
			val.d = window[k];
			diff.u = val.u - val_prev[lane_id].u;

			val_prev[lane_id].u += diff.u;

			double error = fabs(val.d - val_prev[lane_id].d) / fabs(val.d);
			if (error > tolerance)
				error("tolerance exceeded");

			if (diff.u == 0)
			{
				len = 0;
				data_val_lens[k] = 0;
			}
			else
			{
				/* For the length values (0-8) 4 bits are needed.
				 *
				 * For the trailing zero bits values (0-63, 64 isn't needed) 6 bits are needed, but we only have 4 bits space remaining.
				 * so we keep shifts in strides of 4 (0, 4, 8, 12, ..., 60).
				 */
				uint64_t rem_tz;
				ValueTypeI buf_diff = diff.u;
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
				bytestream_write_unsafe_cast(&Bs[lane_id], (uint64_t) diff.u, len);   // Bytestream works with 'uint64_t' buffer (so we cast diff.u), and the actual size in bytes 'len'.
				len |=  (trailing_zero_bits_div4 << 4ULL);
				data_val_lens[k] = len;

				uint64_t tz = (len >> 2ULL) & (~3ULL);
				if (tz != trailing_zeros)
					error("tz: %ld %ld %ld", len, tz, trailing_zero_bits_div4);
				len &= 15ULL;
				ValueTypeI mask = (len == 8) ? -1ULL : (1ULL << len_bits) - 1ULL;
				diff.u &= mask;
				diff.u <<= tz;
				if (diff.u != buf_diff)
					error("diff.u != buf_diff");
			}
		}
		k_s += num_rf_vals_unique;
	}
	for (long iter=0;iter<VEC_LEN;iter++)
		data_val_lanes_size[iter] = Bs[iter].len;

	unsigned char * ptr = &data_val_lens[data_val_lens_bytes];
	uint64_t data_val_lanes_bytes = 0;
	for (long iter=0;iter<VEC_LEN;iter++)
	{
		memcpy(ptr, data_val_lanes[iter], Bs[iter].len);
		ptr += Bs[iter].len;
		data_val_lanes_bytes += Bs[iter].len;
	}

	if (num_vals_out != NULL)
		*num_vals_out = num_vals;

	return data_intro_bytes + data_rfs_bytes + data_num_unique_vals_per_rf_bytes + data_coords_bytes + data_val_lens_bytes + data_val_lanes_bytes;
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
mult_add_serial(ValueType * x_rel, ValueType * y_rel, vec_t(VTF, VEC_LEN) val, vec_t(i64, VEC_LEN) row_rel, vec_t(i64, VEC_LEN) col_rel)
{
	for (long iter=0;iter<VEC_LEN;iter++)
		y_rel[vec_array(i64, VEC_LEN, row_rel)[iter]] += vec_array(VTF, VEC_LEN, val)[iter] * x_rel[vec_array(i64, VEC_LEN, col_rel)[iter]];
}


//==========================================================================================================================================
//= Decompress
//==========================================================================================================================================


static __attribute__((always_inline)) inline
long
decompress_and_compute_kernel_sort_diff_base(unsigned char * restrict buf, ValueType * restrict x, ValueType * restrict y, long * restrict num_vals_out, const int validate,
		void (* gather_coords)(long i, unsigned char * data_coords, const uint64_t coords_bytes, uint64_t row_bits, uint64_t col_bits, uint64_t * row_rel_out, uint64_t * col_rel_out),
		void (* gather_coords_v)(long i, unsigned char * data_coords, const uint64_t coords_bytes, uint64_t row_bits, uint64_t col_bits, vec_t(i64, VEC_LEN) * row_rel_out, vec_t(i64, VEC_LEN) * col_rel_out)
		)
{
	int tnum = omp_get_thread_num();
	struct packet_header * header;
	ValueType * x_rel;
	ValueType * y_rel;
	ValueType * window = t_window[tnum];
	INT_T * window_ia = t_window_ia[tnum];
	INT_T * window_ja = t_window_ja[tnum];
	long num_vals;
	long num_vals_unique;
	uint64_t row_min, col_min;
	[[gnu::unused]] long num_rows;
	uint64_t row_bits, col_bits;
	long num_rfs;
	uint32_t * data_val_lanes_size;
	long i, j, k, l, r;

	unsigned char * data_intro = buf;
	uint64_t data_intro_bytes = sizeof(*header);

	header = (typeof(header)) buf;

	row_bits = header->row_bits;
	col_bits = header->col_bits;
	num_vals = header->num_vals;
	num_vals_unique = header->num_vals_unique;
	num_rows = header->num_rows;
	row_min = header->row_min;
	col_min = header->col_min;
	num_rfs = header->num_rfs;

	data_val_lanes_size = header->data_val_lanes_size;

	y_rel = y + row_min;
	x_rel = x + col_min;

	uint32_t * data_rfs = (uint32_t *) &data_intro[data_intro_bytes];
	const uint64_t data_rfs_bytes = sizeof(*data_rfs) * num_rfs;

	uint32_t * data_num_unique_vals_per_rf = (uint32_t *) &((unsigned char *) data_rfs)[data_rfs_bytes];
	const uint64_t data_num_unique_vals_per_rf_bytes = sizeof(*data_num_unique_vals_per_rf) * num_rfs;

	unsigned char * data_coords = &((unsigned char *) data_num_unique_vals_per_rf)[data_num_unique_vals_per_rf_bytes];
	const uint64_t coords_bytes = (row_bits + col_bits + 7) >> 3;
	const uint64_t data_coords_bytes = coords_bytes * num_vals;

	unsigned char * data_val_lens = &data_coords[data_coords_bytes];
	const uint64_t data_val_lens_bytes = num_vals_unique;

	vec_t(i64, VEC_LEN) data_val_lanes;
	vec_array(i64, VEC_LEN, data_val_lanes)[0] = (long long) &data_val_lens[data_val_lens_bytes];
	uint64_t data_val_lanes_bytes = data_val_lanes_size[0];
	for (long iter=1;iter<VEC_LEN;iter++)
	{
		vec_array(i64, VEC_LEN, data_val_lanes)[iter] = vec_array(i64, VEC_LEN, data_val_lanes)[iter-1] + data_val_lanes_size[iter-1];
		data_val_lanes_bytes += data_val_lanes_size[iter];
	}

	for (l=0;l<num_rfs;l++)
	{
		union {
			vec_t(VTF, VEC_LEN) d;
			vec_t(VTI, VEC_LEN) u;
		} val;
		long rf = data_rfs[l];
		long num_rf_vals_unique = data_num_unique_vals_per_rf[l];
		long num_rf_vals_unique_div = num_rf_vals_unique / VEC_LEN;
		long num_rf_vals_unique_multiple = num_rf_vals_unique_div * VEC_LEN;
		val.u = vec_set1(VTI, VEC_LEN, 0);
		for (i=0;i<num_rf_vals_unique_multiple;i+=VEC_LEN)
		{
			union {
				vec_t(VTF, VEC_LEN) d;
				vec_t(VTI, VEC_LEN) u;
			} diff;
			vec_t(i64, VEC_LEN) len_64;
			vec_t(VTI, VEC_LEN) len;
			vec_t(i64, VEC_LEN) row_rel, col_rel;

			len = vec_set_iter(VTI, VEC_LEN, iter, data_val_lens[i+iter]);

			// vec_t(VTI, VEC_LEN) tz = (len >> 2ULL) & (~3ULL);
			vec_t(VTI, VEC_LEN) tz = vec_and(VTI, VEC_LEN, vec_srli(VTI, VEC_LEN, len, 2ULL), vec_set1(VTI, VEC_LEN, (ValueTypeI) ~3ULL));
			// len &= 15ULL;
			len = vec_and(VTI, VEC_LEN, len, vec_set1(VTI, VEC_LEN, 15ULL));
			// len_bits = len << 3ULL;
			vec_t(VTI, VEC_LEN) len_bits = vec_slli(VTI, VEC_LEN, len, 3ULL);

			diff.u = vec_set_iter(VTI, VEC_LEN, iter, *((ValueTypeI *) vec_array(i64, VEC_LEN, data_val_lanes)[iter]));

			// data_val_lanes = data_val_lanes + len_64;
			len_64 = vec_set_iter(i64, VEC_LEN, iter, vec_array(VTI, VEC_LEN, len)[iter]);
			data_val_lanes = vec_add(i64, VEC_LEN, data_val_lanes, len_64);

			// vec_t(VTI, VEC_LEN) mask = (1ULL << len_bits) - 1ULL;
			vec_t(VTI, VEC_LEN) mask = vec_sub(VTI, VEC_LEN, vec_sllv(VTI, VEC_LEN, vec_set1(VTI, VEC_LEN, 1ULL), len_bits), vec_set1(VTI, VEC_LEN, 1ULL));
			// diff.u &= mask;
			diff.u = vec_and(VTI, VEC_LEN, diff.u, mask);
			// diff.u <<= tz;
			diff.u = vec_sllv(VTI, VEC_LEN, diff.u, tz);

			// val.u = val.u + diff.u;
			val.u = vec_add(VTI, VEC_LEN, val.u, diff.u);

			for (j=0;j<rf;j++)
			{
				k = i*rf + VEC_LEN*j;
				gather_coords_v(k, data_coords, coords_bytes, row_bits, col_bits, &row_rel, &col_rel);

				if (validate)
				{
					vec_storeu(VTF, VEC_LEN, &window[k], val.d);
					vec_t(i64, VEC_LEN) row, col;
					row = vec_add(i64, VEC_LEN, row_rel, vec_set1(i64, VEC_LEN, row_min));
					col = vec_add(i64, VEC_LEN, col_rel, vec_set1(i64, VEC_LEN, col_min));
					for (r=0;r<VEC_LEN;r++)
					{
						window_ia[k+r] = vec_array(i64, VEC_LEN, row)[r];
						window_ja[k+r] = vec_array(i64, VEC_LEN, col)[r];
					}
				}
				else
				{
					mult_add_serial(x_rel, y_rel, val.d, row_rel, col_rel);
				}
			}
		}

		for (i=num_rf_vals_unique_multiple;i<num_rf_vals_unique;i++)
		{
			union {
				ValueType d;
				ValueTypeI u;
			} diff;
			uint64_t len;
			long lane_id = i % VEC_LEN;

			len = data_val_lens[i];

			uint64_t tz = (len >> 2ULL) & (~3ULL);
			len &= 15ULL;
			uint64_t len_bits = len << 3ULL;

			diff.u = *((ValueTypeI *) vec_array(i64, VEC_LEN, data_val_lanes)[lane_id]);

			vec_array(i64, VEC_LEN, data_val_lanes)[lane_id] = vec_array(i64, VEC_LEN, data_val_lanes)[lane_id] + len;

			ValueTypeI mask = (len == 8) ? -1ULL : (1ULL << len_bits) - 1ULL;
			diff.u &= mask;
			diff.u <<= tz;

			vec_array(VTI, VEC_LEN, val.u)[lane_id] = vec_array(VTI, VEC_LEN, val.u)[lane_id] + diff.u;

			long rf_div = rf / VEC_LEN;
			long rf_multiple = rf_div * VEC_LEN;
			for (j=0;j<rf_multiple;j+=VEC_LEN)
			{
				vec_t(i64, VEC_LEN) row_rel, col_rel;
				vec_t(VTF, VEC_LEN) val_buf = vec_set1(VTF, VEC_LEN, vec_array(VTF, VEC_LEN, val.d)[lane_id]);
				k = i*rf + j;
				gather_coords_v(k, data_coords, coords_bytes, row_bits, col_bits, &row_rel, &col_rel);
				if (validate)
				{
					vec_storeu(VTF, VEC_LEN, &window[k], val_buf);
					vec_t(i64, VEC_LEN) row, col;
					row = vec_add(i64, VEC_LEN, row_rel, vec_set1(i64, VEC_LEN, row_min));
					col = vec_add(i64, VEC_LEN, col_rel, vec_set1(i64, VEC_LEN, col_min));
					for (r=0;r<VEC_LEN;r++)
					{
						window_ia[k+r] = vec_array(i64, VEC_LEN, row)[r];
						window_ja[k+r] = vec_array(i64, VEC_LEN, col)[r];
					}
				}
				else
				{
					mult_add_serial(x_rel, y_rel, val_buf, row_rel, col_rel);
				}
			}

			for (j=rf_multiple;j<rf;j++)
			{
				uint64_t row_rel, col_rel;
				k = i*rf + j;
				gather_coords(k, data_coords, coords_bytes, row_bits, col_bits, &row_rel, &col_rel);
				if (validate)
				{
					window[k] = vec_array(VTF, VEC_LEN, val.d)[lane_id];
					uint64_t row, col;
					row = row_rel + row_min;
					col = col_rel + col_min;
					window_ia[k] = row;
					window_ja[k] = col;
				}
				else
				{
					y_rel[row_rel] += vec_array(VTF, VEC_LEN, val.d)[lane_id] * x_rel[col_rel];
				}
			}
		}

		data_val_lens += num_rf_vals_unique;
		data_coords += coords_bytes * rf * num_rf_vals_unique;
		window += rf * num_rf_vals_unique;
		window_ia += rf * num_rf_vals_unique;
		window_ja += rf * num_rf_vals_unique;
	}

	if (num_vals_out != NULL)
		*num_vals_out = num_vals;
	return data_intro_bytes + data_rfs_bytes + data_num_unique_vals_per_rf_bytes + data_coords_bytes + data_val_lens_bytes + data_val_lanes_bytes;
}


static __attribute__((always_inline)) inline
long
decompress_and_compute_kernel_sort_diff_select(unsigned char * restrict buf, ValueType * restrict x, ValueType * restrict y, long * restrict num_vals_out, const int validate)
{
	struct packet_header * header = (typeof(header)) buf;
	uint64_t row_bits, col_bits;

	row_bits = header->row_bits;
	col_bits = header->col_bits;

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

	error("row_bits = %ld , col_bits == %ld", row_bits, col_bits);
	return -1;

}


void
get_packet_rows(unsigned char * restrict buf, long * i_s_ptr, long * i_e_ptr)
{
	struct packet_header * header = (typeof(header)) buf;
	uint64_t row_min;
	long num_rows;

	num_rows = header->num_rows;
	row_min = header->row_min;

	*i_s_ptr = row_min;
	*i_e_ptr = row_min + num_rows;
}


static inline
long
decompress_and_compute_kernel_sort_diff(unsigned char * restrict buf, ValueType * restrict x, ValueType * restrict y, [[gnu::unused]] long i_t_s, [[gnu::unused]] long i_t_e)
{
	return decompress_and_compute_kernel_sort_diff_select(buf, x, y, NULL, 0);
}


static
long
decompress_kernel_sort_diff(INT_T * ia_out, INT_T * ja_out, ValueType * a_out, long * num_vals_out, unsigned char * restrict buf, [[gnu::unused]] long i_t_s, [[gnu::unused]] long i_t_e)
{
	long num_vals;
	int tnum = omp_get_thread_num();
	ValueType * window = t_window[tnum];
	INT_T * window_ia = t_window_ia[tnum];
	INT_T * window_ja = t_window_ja[tnum];
	long i, bytes;
	bytes = decompress_and_compute_kernel_sort_diff_select(buf, NULL, NULL, &num_vals, 1);
	for (i=0;i<num_vals;i++)
	{
		a_out[i] = window[i];
		ia_out[i] = window_ia[i];
		ja_out[i] = window_ja[i];
	}
	*num_vals_out = num_vals;
	return bytes;
}

