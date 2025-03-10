#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <omp.h>
#include <x86intrin.h>

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
	#include "vectorization.h"

	#include "spmv_kernel_div_gather_coords.h"
	// #include "spmv_kernel_div_gather_coords_avx2.h"
#ifdef __cplusplus
}
#endif


#if DOUBLE == 0
	#define ValueType_cast_int  uint32_t
#elif DOUBLE == 1
	#define ValueType_cast_int  uint64_t
#endif


//==========================================================================================================================================
//= Sorted Delta
//==========================================================================================================================================


static uint64_t window_size_bits;
static uint64_t window_size;
static double ** t_window;
static double ** t_window_buf;
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
static unsigned int ** t_rows_diff;
static unsigned int ** t_cols;
static unsigned int ** t_cols_buf;
static double ** t_window_sym;
static unsigned int ** t_rows_sym;
static unsigned int ** t_cols_sym;
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

// static inline
// int
// quicksort_cmp(int a, int b, struct cmp_data * aux)
// {
	// unsigned int * cols = aux->cols;
	// double * vals = aux->vals;
	// unsigned int ca=cols[a], cb=cols[b];
	// double va=vals[a], vb=vals[b];
	// int ret;
	// if (va*vb <= 0)
		// ret = (va > vb) ? 1 : (va < vb) ? -1 : 0;
	// else
		// ret = (fabs(va) > fabs(vb)) ? 1 : (fabs(va) < fabs(vb)) ? -1 : 0;
	// if (ret == 0)
		// ret =  (ca > cb) ? 1 : (ca < cb) ? -1 : 0;
	// return ret;
// }


static inline
int
quicksort_cmp(int a, int b, struct cmp_data * aux)
{
	unsigned int * rows = aux->rows;
	unsigned int * cols = aux->cols;
	double * vals = aux->vals;
	int ra=rows[a], rb=rows[b];
	int ca=cols[a], cb=cols[b];
	[[gnu::unused]] int ra_mul = ra / 8, rb_mul = rb / 8;
	[[gnu::unused]] int ca_mul = ca / 256, cb_mul = cb / 256;
	double va=vals[a], vb=vals[b];
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
	// ret =  (ca > cb) ? 1 : (ca < cb) ? -1 : 0;
	// if (ret == 0)
	// {
		// if (va*vb <= 0)
			// ret = (va > vb) ? 1 : (va < vb) ? -1 : 0;
		// else
			// ret = (fabs(va) > fabs(vb)) ? 1 : (fabs(va) < fabs(vb)) ? -1 : 0;
	// }
	return ret;
}


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


void
compress_init_sort_diff(__attribute__((unused)) ValueType * vals, __attribute__((unused)) const long num_vals, long packet_size)
{
	int num_threads = omp_get_max_threads();
	packet_size = packet_size + packet_size % vec_len_pd;

	bits_u64_required_bits_for_binary_representation(packet_size - 1, &window_size_bits, &window_size);    // Maximum representable number we want is 'packet_size - 1'.
	printf("window_size = %ld , window_size_bits = %ld\n", window_size, window_size_bits);
	t_window = (typeof(t_window)) malloc(num_threads * sizeof(*t_window));
	t_window_buf = (typeof(t_window_buf)) malloc(num_threads * sizeof(*t_window_buf));
	t_window_rf = (typeof(t_window_rf)) malloc(num_threads * sizeof(*t_window_rf));
	t_window_rf_buf = (typeof(t_window_rf_buf)) malloc(num_threads * sizeof(*t_window_rf_buf));
	t_data_lanes = (typeof(t_data_lanes)) malloc(num_threads * sizeof(*t_data_lanes));
	t_permutation = (typeof(t_permutation)) malloc(num_threads * sizeof(*t_permutation));
	t_rev_permutation = (typeof(t_rev_permutation)) malloc(num_threads * sizeof(*t_rev_permutation));
	t_permutation_buf = (typeof(t_permutation_buf)) malloc(num_threads * sizeof(*t_permutation_buf));
	t_rev_permutation_buf = (typeof(t_rev_permutation_buf)) malloc(num_threads * sizeof(*t_rev_permutation_buf));
	t_permutation_buf_2 = (typeof(t_permutation_buf_2)) malloc(num_threads * sizeof(*t_permutation_buf_2));
	t_rev_permutation_buf_2 = (typeof(t_rev_permutation_buf_2)) malloc(num_threads * sizeof(*t_rev_permutation_buf_2));
	t_offsets_rf = (typeof(t_offsets_rf)) malloc(num_threads * sizeof(*t_offsets_rf));
	t_rfs = (typeof(t_rfs)) malloc(num_threads * sizeof(*t_rfs));
	t_num_unique_vals_per_rf = (typeof(t_num_unique_vals_per_rf)) malloc(num_threads * sizeof(*t_num_unique_vals_per_rf));
	t_rows = (typeof(t_rows)) malloc(num_threads * sizeof(*t_rows));
	t_rows_diff = (typeof(t_rows_diff)) malloc(num_threads * sizeof(*t_rows_diff));
	t_cols = (typeof(t_cols)) malloc(num_threads * sizeof(*t_cols));
	t_cols_buf = (typeof(t_cols_buf)) malloc(num_threads * sizeof(*t_cols_buf));
	t_window_sym = (typeof(t_window_sym)) malloc(num_threads * sizeof(*t_window_sym));
	t_rows_sym = (typeof(t_rows_sym)) malloc(num_threads * sizeof(*t_rows_sym));
	t_cols_sym = (typeof(t_cols_sym)) malloc(num_threads * sizeof(*t_cols_sym));
	t_qsort_partitions = (typeof(t_qsort_partitions)) malloc(num_threads * sizeof(*t_qsort_partitions));
	t_row_bits_accum = (typeof(t_row_bits_accum)) malloc(num_threads * sizeof(*t_row_bits_accum));
	t_col_bits_accum = (typeof(t_col_bits_accum)) malloc(num_threads * sizeof(*t_col_bits_accum));
	t_row_col_bytes_accum = (typeof(t_row_col_bytes_accum)) malloc(num_threads * sizeof(*t_row_col_bytes_accum));
	#pragma omp parallel
	{
		int tnum = omp_get_thread_num();
		long i;
		t_window[tnum] = (typeof(&(**t_window))) malloc(window_size * sizeof(**t_window));
		t_window_buf[tnum] = (typeof(&(**t_window_buf))) malloc(window_size * sizeof(**t_window_buf));
		t_window_rf[tnum] = (typeof(&(**t_window_rf))) malloc(window_size * sizeof(**t_window_rf));
		t_window_rf_buf[tnum] = (typeof(&(**t_window_rf_buf))) malloc(window_size * sizeof(**t_window_rf_buf));
		t_data_lanes[tnum] = (typeof(&(**t_data_lanes))) malloc(vec_len_pd * sizeof(**t_data_lanes));
		for (i=0;i<vec_len_pd;i++)
		{
			t_data_lanes[tnum][i] = (typeof(&(**t_data_lanes[i]))) malloc(8*window_size * sizeof(**t_data_lanes[i]));
		}
		t_permutation[tnum] = (typeof(&(**t_permutation))) malloc(window_size * sizeof(**t_permutation));
		t_rev_permutation[tnum] = (typeof(&(**t_rev_permutation))) malloc(window_size * sizeof(**t_rev_permutation));
		t_permutation_buf[tnum] = (typeof(&(**t_permutation_buf))) malloc(window_size * sizeof(**t_permutation_buf));
		t_rev_permutation_buf[tnum] = (typeof(&(**t_rev_permutation_buf))) malloc(window_size * sizeof(**t_rev_permutation_buf));
		t_permutation_buf_2[tnum] = (typeof(&(**t_permutation_buf_2))) malloc(window_size * sizeof(**t_permutation_buf_2));
		t_rev_permutation_buf_2[tnum] = (typeof(&(**t_rev_permutation_buf_2))) malloc(window_size * sizeof(**t_rev_permutation_buf_2));
		t_offsets_rf[tnum] = (typeof(&(**t_offsets_rf))) malloc((window_size + 1) * sizeof(**t_offsets_rf));   // These are offsets, so we need + 1.
		t_rfs[tnum] = (typeof(&(**t_rfs))) malloc(window_size * sizeof(**t_rfs));
		t_num_unique_vals_per_rf[tnum] = (typeof(&(**t_num_unique_vals_per_rf))) malloc(window_size * sizeof(**t_num_unique_vals_per_rf));
		t_rows[tnum] = (typeof(&(**t_rows))) malloc(window_size * sizeof(**t_rows));
		t_rows_diff[tnum] = (typeof(&(**t_rows_diff))) malloc(window_size * sizeof(**t_rows_diff));
		t_cols[tnum] = (typeof(&(**t_cols))) malloc(window_size * sizeof(**t_cols));
		t_cols_buf[tnum] = (typeof(&(**t_cols_buf))) malloc(window_size * sizeof(**t_cols_buf));
		t_window_sym[tnum] = (typeof(&(**t_window_sym))) malloc(window_size * sizeof(**t_window_sym));
		t_rows_sym[tnum] = (typeof(&(**t_rows_sym))) malloc(window_size * sizeof(**t_rows_sym));
		t_cols_sym[tnum] = (typeof(&(**t_cols_sym))) malloc(window_size * sizeof(**t_cols_sym));
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


/*
 * 'i_s' is certain to be the first non-empty row.
 */
static inline
long
compress_kernel_sort_diff(INT_T * row_ptr, INT_T * ja, ValueType * vals, long i_s, long i_t_s, long i_t_e, long j_s, unsigned char * buf, long num_vals_suggested, long * num_vals_out)
{
	int tnum = omp_get_thread_num();
	long num_vals = num_vals_suggested;
	long num_vals_unique;
	long num_vals_sym;
	double * window = t_window[tnum];
	double * window_buf = t_window_buf[tnum];
	double * window_sym = t_window_sym[tnum];
	int * window_rf = t_window_rf[tnum];
	int * window_rf_buf = t_window_rf_buf[tnum];
	unsigned char ** data_val_lanes = t_data_lanes[tnum];
	int * data_val_lanes_size;
	unsigned int * rows = t_rows[tnum];
	unsigned int * rows_diff = t_rows_diff[tnum];
	unsigned int * cols = t_cols[tnum];
	unsigned int * cols_buf = t_cols_buf[tnum];
	unsigned int * rows_sym = t_rows_sym[tnum];
	unsigned int * cols_sym = t_cols_sym[tnum];
	int * permutation = t_permutation[tnum];
	int * rev_permutation = t_rev_permutation[tnum];
	// int * permutation_buf = t_permutation_buf[tnum];
	int * rev_permutation_buf = t_rev_permutation_buf[tnum];
	int * permutation_buf_2 = t_permutation_buf_2[tnum];
	int * rev_permutation_buf_2 = t_rev_permutation_buf_2[tnum];
	int * offsets_rf = t_offsets_rf[tnum];
	int * rfs = t_rfs[tnum];
	int * num_unique_vals_per_rf = t_num_unique_vals_per_rf[tnum];
	struct Byte_Stream Bs[vec_len_pd];
	uint64_t len_bits = 0;
	uint64_t len = 0;
	uint64_t row_min, row_max, row_diff = 0, row_diff_max = 0, col = 0, col_min = 0, col_max = 0, col_diff = 0, col_diff_max = 0;   // row_max is the last row (i.e., inclusive: num_rows = row_max + 1 - i_s)
	uint64_t row_bits = 0, col_bits = 0;
	uint64_t row_col_bytes = 0;
	long num_rfs;
	long num_rf_vals, num_rf_vals_unique;
	long i, j, k, k_s, l, l_s, l_e;
	long rf;
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
		num_vals = num_vals_suggested;
		force_row_index_lte_1_byte = 0;
		// error("empty packet");
	}

	long num_vals_consumed = num_vals;
	num_vals_sym = 0;
	k = 0;
	for (l=0,i=row_min,j=j_s; l<num_vals; l++,j++)
	{
		double val;
		while (j >= row_ptr[i+1])
			i++;
		rev_permutation[k] = k;
		if (tolerance > 0)
			val = reduce_precision(vals[l], tolerance);
		else
			val = vals[l];
		col = ja[j];
		if (col <  (uint64_t) i)  // Extract the symmetric thread local nnz below the diagonal.
		{
			if ((col >= (uint64_t) i_t_s) && (col < (uint64_t) i_t_e))
			// if ((col >= (uint64_t) i_t_s) && (col < (uint64_t) i_t_e) && (labs(col - i) < 256))
			{
				window_sym[num_vals_sym] = val;
				rows_sym[num_vals_sym] = i;
				cols_sym[num_vals_sym] = ja[j];
				num_vals_sym++;
				continue;
			}
		}
		if (col > (uint64_t) i)  // Filter out the symmetric thread local nnz above the diagonal.
		{
			if ((col >= (uint64_t) i_t_s) && (col < (uint64_t) i_t_e))
			// if ((col >= (uint64_t) i_t_s) && (col < (uint64_t) i_t_e) && (labs(col - i) < 256))
				continue;
		}
		window_buf[k] = val;
		rows[k] = i;
		cols_buf[k] = ja[j];
		k++;
	}
	num_vals = k;
	row_max = i;
	// printf("%d: num_vals=%ld num_vals_sym=%ld\n", tnum, num_vals, num_vals_sym);

	/* Sort the packet values. */
	struct cmp_data compare_data;
	compare_data.vals = window_buf;
	compare_data.cols = cols_buf;
	compare_data.rows = rows;
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
		test_permutation(num_vals, window_buf, window, rev_permutation, (char *) "quicksort");

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

	// if (tolerance == 0)
		// test_permutation(num_vals, vals, window, rev_permutation, (char *) "bucketsort");

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

		permutation_interleave(window + k, window_buf + k, num_rf_vals_unique, vec_len_pd, 1, 0, 1);
		permutation_interleave(window_rf + k, window_rf_buf + k, num_rf_vals_unique, vec_len_pd, 1, 0, 1);
		permutation_interleave(rev_permutation + l_s, rev_permutation_buf + l_s, num_rf_vals, vec_len_pd, rf, 0, 1);

		k += num_rf_vals_unique;
	}
	macros_swap(&window, &window_buf);
	macros_swap(&window_rf, &window_rf_buf);
	macros_swap(&rev_permutation, &rev_permutation_buf);

	for (k=0;k<num_vals;k++)
		permutation[rev_permutation[k]] = k;
	col_min = cols_buf[0];
	col_max = cols_buf[0];
	for (k=0,j=j_s;k<num_vals;k++,j++)
	{
		rows_diff[permutation[k]] = rows[k] - row_min;
		col = cols_buf[k];
		cols[permutation[k]] = col;
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
		if (rows_diff[k] + row_min > row_max)
			error("row index");
		if (rev_permutation[k] >= num_vals) // extra fake values
			continue;
		if (cols[k] != (unsigned int) cols_buf[rev_permutation[k]])
			error("col index");
	}

	unsigned char * data_intro = buf;
	uint64_t data_intro_bytes = 0;

	/* Should always be the first data in the packet. */
	data_intro[data_intro_bytes] = row_bits;
	data_intro_bytes += 1;
	data_intro[data_intro_bytes] = col_bits;
	data_intro_bytes += 1;

	*((uint32_t *) &data_intro[data_intro_bytes]) = num_vals;
	data_intro_bytes += sizeof(uint32_t);
	*((uint32_t *) &data_intro[data_intro_bytes]) = num_vals_sym;
	data_intro_bytes += sizeof(uint32_t);
	*((uint32_t *) &data_intro[data_intro_bytes]) = num_vals_unique;
	data_intro_bytes += sizeof(uint32_t);
	*((uint32_t *) &data_intro[data_intro_bytes]) = row_diff_max + 1;   // Number of rows.
	data_intro_bytes += sizeof(uint32_t);

	*((uint32_t *) &data_intro[data_intro_bytes]) = row_min;
	data_intro_bytes += sizeof(uint32_t);
	*((uint32_t *) &data_intro[data_intro_bytes]) = col_min;
	data_intro_bytes += sizeof(uint32_t);

	*((uint32_t *) &data_intro[data_intro_bytes]) = num_rfs;
	data_intro_bytes += sizeof(uint32_t);

	data_val_lanes_size = (typeof(data_val_lanes_size)) &(data_intro[data_intro_bytes]);
	data_intro_bytes += vec_len_pd * sizeof(*data_val_lanes_size);

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
	const uint64_t coords_bytes = row_col_bytes;
	const uint64_t data_coords_bytes = coords_bytes * num_vals;

	unsigned char * data_val_lens = &data_coords[data_coords_bytes];
	const uint64_t data_val_lens_bytes = num_vals_unique;

	for (i=0;i<num_vals;i++)
	{
		uint64_t coords;
		row_diff = rows_diff[i];
		col_diff = cols[i] - col_min;
		coords = row_diff | (col_diff << row_bits);
		*((uint64_t *) &data_coords[i*coords_bytes]) = coords;
	}

	for (long iter=0;iter<vec_len_pd;iter++)
		bytestream_init_write(&Bs[iter], data_val_lanes[iter]);

	union {
		double d;
		uint64_t u;
	} val_prev[vec_len_pd], val, diff;
	k_s = 0;
	for (l=0;l<num_rfs;l++)
	{
		rf = rfs[l];
		num_rf_vals_unique = num_unique_vals_per_rf[l];
		for (long iter=0;iter<vec_len_pd;iter++)
			val_prev[iter].u = 0;
		for (i=0;i<num_rf_vals_unique;i++)
		{
			k = k_s + i;
			lane_id = i % vec_len_pd;
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
				data_val_lens[k] = len;

				uint64_t tz = (len >> 2ULL) & (~3ULL);
				if (tz != trailing_zeros)
					error("tz: %ld %ld %ld", len, tz, trailing_zero_bits_div4);
				len &= 15ULL;
				uint64_t mask = (len == 8) ? -1ULL : (1ULL << len_bits) - 1ULL;
				diff.u &= mask;
				diff.u <<= tz;
				if (diff.u != buf_diff)
					error("diff.u != buf_diff");
			}
		}
		k_s += num_rf_vals_unique;
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

	double * data_vals_sym = (double *) ptr;
	int * data_rows_sym = (int *) &data_vals_sym[num_vals_sym];
	int * data_cols_sym = (int *) &data_rows_sym[num_vals_sym];
	for (i=0;i<num_vals_sym;i++)
	{
		data_vals_sym[i] = window_sym[i];
		data_rows_sym[i] = rows_sym[i];
		data_cols_sym[i] = cols_sym[i];
	}

	if (num_vals_out != NULL)
		*num_vals_out = num_vals_consumed;

	return data_intro_bytes + data_rfs_bytes + data_num_unique_vals_per_rf_bytes + data_coords_bytes + data_val_lens_bytes + data_val_lanes_bytes + (sizeof(double) + 2*sizeof(int)) * num_vals_sym;
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
	long num_vals_sym;
	long num_vals_unique;
	uint64_t row_min, col_min;
	[[gnu::unused]] long num_rows;
	uint64_t row_bits, col_bits;
	long i, j, k, l;

	unsigned char * data_intro = buf;
	uint64_t data_intro_bytes = 0;

	row_bits = data_intro[data_intro_bytes];
	data_intro_bytes += 1;
	col_bits = data_intro[data_intro_bytes];
	data_intro_bytes += 1;

	num_vals = *((uint32_t *) &data_intro[data_intro_bytes]);
	data_intro_bytes += sizeof(uint32_t);
	num_vals_sym = *((uint32_t *) &data_intro[data_intro_bytes]);
	data_intro_bytes += sizeof(uint32_t);
	num_vals_unique = *((uint32_t *) &data_intro[data_intro_bytes]);
	data_intro_bytes += sizeof(uint32_t);
	num_rows = *((uint32_t *) &data_intro[data_intro_bytes]);
	data_intro_bytes += sizeof(uint32_t);

	row_min = *((uint32_t *) &data_intro[data_intro_bytes]);
	y_rel = y + row_min;
	data_intro_bytes += sizeof(uint32_t);
	col_min = *((uint32_t *) &data_intro[data_intro_bytes]);
	x_rel = x + col_min;
	data_intro_bytes += sizeof(uint32_t);

	long num_rfs = *((uint32_t *) &data_intro[data_intro_bytes]);
	data_intro_bytes += sizeof(uint32_t);

	int * data_val_lanes_size;
	data_val_lanes_size = (typeof(data_val_lanes_size)) &(data_intro[data_intro_bytes]);
	data_intro_bytes += vec_len_pd * sizeof(*data_val_lanes_size);

	uint32_t * data_rfs = (uint32_t *) &data_intro[data_intro_bytes];
	const uint64_t data_rfs_bytes = sizeof(*data_rfs) * num_rfs;

	uint32_t * data_num_unique_vals_per_rf = (uint32_t *) &((unsigned char *) data_rfs)[data_rfs_bytes];
	const uint64_t data_num_unique_vals_per_rf_bytes = sizeof(*data_num_unique_vals_per_rf) * num_rfs;

	unsigned char * data_coords = &((unsigned char *) data_num_unique_vals_per_rf)[data_num_unique_vals_per_rf_bytes];
	const uint64_t coords_bytes = (row_bits + col_bits + 7) >> 3;
	const uint64_t data_coords_bytes = coords_bytes * num_vals;

	unsigned char * data_val_lens = &data_coords[data_coords_bytes];
	const uint64_t data_val_lens_bytes = num_vals_unique;

	vec_i_t data_val_lanes;
	data_val_lanes[0] = (long long) &data_val_lens[data_val_lens_bytes];
	uint64_t data_val_lanes_bytes = data_val_lanes_size[0];
	for (long iter=1;iter<vec_len_pd;iter++)
	{
		data_val_lanes[iter] = data_val_lanes[iter-1] + data_val_lanes_size[iter-1];
		data_val_lanes_bytes += data_val_lanes_size[iter];
	}

	double * data_vals_sym = (double *) (data_val_lanes[0] + data_val_lanes_bytes);
	int * data_rows_sym = (int *) &data_vals_sym[num_vals_sym];
	int * data_cols_sym = (int *) &data_rows_sym[num_vals_sym];

	i = 0;

	for (l=0;l<num_rfs;l++)
	{
		union {
			vec_d_t d;
			vec_i_t u;
		} val;
		long rf = data_rfs[l];
		long num_rf_vals_unique = data_num_unique_vals_per_rf[l];
		long num_rf_vals_unique_div = num_rf_vals_unique / vec_len_pd;
		long num_rf_vals_unique_multiple = num_rf_vals_unique_div * vec_len_pd;
		val.u = vec_set1_epi64(0);
		for (i=0;i<num_rf_vals_unique_multiple;i+=vec_len_pd)
		{
			union {
				vec_d_t d;
				vec_i_t u;
			} diff;
			vec_i_t len;
			vec_i_t row_rel, col_rel;

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

			for (j=0;j<rf;j++)
			{
				k = i*rf + vec_len_pd*j;
				gather_coords_v(k, data_coords, coords_bytes, row_bits, col_bits, &row_rel, &col_rel);

				if (validate)
					vec_storeu_pd(&window[k], val.d);
				else
				{
					mult_add_serial(x_rel, y_rel, val.d, row_rel, col_rel);
				}
			}
		}

		for (i=num_rf_vals_unique_multiple;i<num_rf_vals_unique;i++)
		{
			union {
				double d;
				uint64_t u;
			} diff;
			uint64_t len;
			long lane_id = i % vec_len_pd;

			len = data_val_lens[i];

			uint64_t tz = (len >> 2ULL) & (~3ULL);
			len &= 15ULL;
			uint64_t len_bits = len << 3ULL;

			diff.u = *((uint64_t *) data_val_lanes[lane_id]);

			data_val_lanes[lane_id] = data_val_lanes[lane_id] + len;

			uint64_t mask = (len == 8) ? -1ULL : (1ULL << len_bits) - 1ULL;
			diff.u &= mask;
			diff.u <<= tz;

			val.u[lane_id] = val.u[lane_id] + diff.u;

			long rf_div = rf / vec_len_pd;
			long rf_multiple = rf_div * vec_len_pd;
			for (j=0;j<rf_multiple;j+=vec_len_pd)
			{
				vec_i_t row_rel, col_rel;
				vec_d_t val_buf = vec_set1_pd(val.d[lane_id]);
				k = i*rf + j;
				gather_coords_v(k, data_coords, coords_bytes, row_bits, col_bits, &row_rel, &col_rel);
				if (validate)
					vec_storeu_pd(&window[k], val_buf);
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
					window[k] = val.d[lane_id];
				else
				{
					y_rel[row_rel] += val.d[lane_id] * x_rel[col_rel];
				}
			}
		}

		data_val_lens += num_rf_vals_unique;
		data_coords += coords_bytes * rf * num_rf_vals_unique;
		window += rf * num_rf_vals_unique;
	}

	if (!validate)
	{
		if (num_vals_sym > 0)
		{
			long row = data_rows_sym[0], col;
			double val, sum = 0;
			for (i=0;i<num_vals_sym;i++)
			{
				val = data_vals_sym[i];
				col = data_cols_sym[i];
				if (data_rows_sym[i] != row)
				{
					y[row] += sum;
					row = data_rows_sym[i];
					sum = 0;
				}
				sum += val * x[col];
				y[col] += val * x[row];
			}
			y[row] += sum;
		}
	}

	if (num_vals_out != NULL)
		*num_vals_out = num_vals;
	return data_intro_bytes + data_rfs_bytes + data_num_unique_vals_per_rf_bytes + data_coords_bytes + data_val_lens_bytes + data_val_lanes_bytes + (sizeof(double) + 2*sizeof(int)) * num_vals_sym;
}


static __attribute__((always_inline)) inline
long
decompress_and_compute_kernel_sort_diff_select(unsigned char * restrict buf, ValueType * restrict x, ValueType * restrict y, long * restrict num_vals_out, const int validate)
{
	uint64_t row_bits, col_bits;
	unsigned char * data_intro = buf;
	uint64_t data_intro_bytes = 0;
	__attribute__((unused)) long num_vals;
	__attribute__((unused)) long num_vals_sym;
	__attribute__((unused)) long num_vals_unique;
	__attribute__((unused)) long num_rows;

	row_bits = data_intro[data_intro_bytes];
	data_intro_bytes += 1;
	col_bits = data_intro[data_intro_bytes];
	data_intro_bytes += 1;

	num_vals = *((uint32_t *) &data_intro[data_intro_bytes]);
	data_intro_bytes += sizeof(uint32_t);
	num_vals_sym = *((uint32_t *) &data_intro[data_intro_bytes]);
	data_intro_bytes += sizeof(uint32_t);
	num_vals_unique = *((uint32_t *) &data_intro[data_intro_bytes]);
	data_intro_bytes += sizeof(uint32_t);
	num_rows = *((uint32_t *) &data_intro[data_intro_bytes]);
	data_intro_bytes += sizeof(uint32_t);

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

