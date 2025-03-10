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


static uint64_t window_size_bits;
static uint64_t window_size;

struct thread_data_kernel {
	ValueType * window;
	ValueType * window_buf;
	INT_T * window_ia;
	INT_T * window_ja;
	int * window_rf;
	int * window_rf_buf;
	unsigned char ** data_lanes;
	int * permutation;
	int * rev_permutation;
	int * rev_permutation_total;
	int * rev_permutation_total_buf;
	int * offsets_rf;
	int * rfs;
	int * num_unique_vals_per_rf;
	unsigned int * rows;
	unsigned int * rows_buf;
	unsigned int * cols;
	unsigned int * cols_buf;
	ValueType * window_sym;
	unsigned int * rows_sym;
	unsigned int * cols_sym;
	int * qsort_partitions;
};

static struct thread_data_kernel ** tdks;


// struct [[gnu::packed]] packet_header {
struct packet_header {
	struct {
		uint8_t symmetric   : 1;
		uint8_t dv_enabled : 1;
		uint8_t rf_enabled  : 1;
	};

	uint32_t size;
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
//- Quicksort Symmetric
//------------------------------------------------------------------------------------------------------------------------------------------

#ifdef __cplusplus
extern "C"{
#endif
	#include "sort/quicksort/quicksort_gen_undef.h"
	#define QUICKSORT_GEN_TYPE_1  int
	#define QUICKSORT_GEN_TYPE_2  int
	#define QUICKSORT_GEN_TYPE_3  struct cmp_data
	#define QUICKSORT_GEN_SUFFIX  _vals_sym
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
		ret = (ca > cb) ? 1 : (ca < cb) ? -1 : 0;
	return ret;
}

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
	#define QUICKSORT_GEN_SUFFIX  _vals
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
		ret = (ca > cb) ? 1 : (ca < cb) ? -1 : 0;
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
				ret = (ca > cb) ? 1 : (ca < cb) ? -1 : 0;
		}
	}
	return ret;
} */


//------------------------------------------------------------------------------------------------------------------------------------------
//- Quicksort CSR
//------------------------------------------------------------------------------------------------------------------------------------------

#ifdef __cplusplus
extern "C"{
#endif
	#include "sort/quicksort/quicksort_gen_undef.h"
	#define QUICKSORT_GEN_TYPE_1  int
	#define QUICKSORT_GEN_TYPE_2  int
	#define QUICKSORT_GEN_TYPE_3  struct cmp_data
	#define QUICKSORT_GEN_SUFFIX  _csr
	#include "sort/quicksort/quicksort_gen.c"
#ifdef __cplusplus
}
#endif

static inline
int
quicksort_cmp(int a, int b, struct cmp_data * aux)
{
	unsigned int * rows = aux->rows;
	unsigned int * cols = aux->cols;
	int ra=rows[a], rb=rows[b];
	int ca=cols[a], cb=cols[b];
	int ret = 0;
	ret = (ra > rb) ? 1 : (ra < rb) ? -1 : 0;
	if (ret == 0)
		ret = (ca > cb) ? 1 : (ca < cb) ? -1 : 0;
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
bucketsort_find_bucket(int * A, long i, [[gnu::unused]] void * unused)
{
		return A[i];
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//------------------------------------------------------------------------------------------------------------------------------------------
//-                                                             Compression                                                                -
//------------------------------------------------------------------------------------------------------------------------------------------
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


#define __aligned_alloc(ptr, N)                                           \
do {                                                                      \
	ptr = (typeof(&(*ptr))) aligned_alloc(64, (N) * sizeof(*ptr));    \
} while (0)


void
compress_init_div(__attribute__((unused)) ValueTypeReference * vals, __attribute__((unused)) const long num_vals, long packet_size)
{
	int num_threads = omp_get_max_threads();
	packet_size = packet_size + packet_size % VEC_LEN;

	bits_u64_required_bits_for_binary_representation(packet_size - 1, &window_size_bits, &window_size);    // Maximum representable number we want is 'packet_size - 1'.
	printf("window_size = %ld , window_size_bits = %ld\n", window_size, window_size_bits);

	tdks = (typeof(tdks)) aligned_alloc(64, num_threads * sizeof(*tdks));
	_Pragma("omp parallel")
	{
		int tnum = omp_get_thread_num();
		struct thread_data * td = tds[tnum];
		struct thread_data_kernel * tdk;
		long i;
		tdk = (typeof(tdk)) aligned_alloc(64, sizeof(*tdk));

		__aligned_alloc(tdk->window, 2*window_size);
		__aligned_alloc(tdk->window_buf, window_size);
		__aligned_alloc(tdk->window_ia, 2*window_size);
		__aligned_alloc(tdk->window_ja, 2*window_size);
		__aligned_alloc(tdk->window_rf, window_size);
		__aligned_alloc(tdk->window_rf_buf, window_size);
		__aligned_alloc(tdk->data_lanes, VEC_LEN);
		__aligned_alloc(tdk->permutation, window_size);
		__aligned_alloc(tdk->rev_permutation, window_size);
		__aligned_alloc(tdk->rev_permutation_total, window_size);
		__aligned_alloc(tdk->rev_permutation_total_buf, window_size);
		__aligned_alloc(tdk->offsets_rf, (window_size + 1));   // These are offsets, so we need + 1.
		__aligned_alloc(tdk->rfs, window_size);
		__aligned_alloc(tdk->num_unique_vals_per_rf, window_size);
		__aligned_alloc(tdk->rows, window_size);
		__aligned_alloc(tdk->rows_buf, window_size);
		__aligned_alloc(tdk->cols, window_size);
		__aligned_alloc(tdk->cols_buf, window_size);
		__aligned_alloc(tdk->qsort_partitions, window_size);
		__aligned_alloc(tdk->window_sym, window_size);
		__aligned_alloc(tdk->rows_sym, window_size);
		__aligned_alloc(tdk->cols_sym, window_size);
		for (i=0;i<VEC_LEN;i++)
		{
			__aligned_alloc(tdk->data_lanes[i], 2*window_size*sizeof(ValueType));   // '*data_lanes[i]' is unsigned char.
		}

		td->row_bits_accum = 0;
		td->col_bits_accum = 0;
		td->row_col_bytes_accum = 0;
		td->packet_unique_values_fraction_accum = 0;

		tdks[tnum] = tdk;
	}
}


// static inline
// long
// select_num_vals(INT_T * row_ptr, long i_s, long j_s, long num_vals, long force_row_index_lte_1_byte)
// {
	// long i_e, j_e;
	// long num_rows;
	// long num_rows_max = 255;
	// long num_vals_ret;
	// j_e = j_s + num_vals;
	// i_e = i_s;
	// while (row_ptr[i_e] < j_e)
		// i_e++;
	// num_rows = i_e - i_s;
	// if (force_row_index_lte_1_byte)
	// {
		// if (num_rows > num_rows_max)
			// i_e = i_s + num_rows_max;
	// }
	// num_vals_ret = row_ptr[i_e] - j_s;
	// if (num_vals_ret > num_vals)
		// num_vals_ret = num_vals;
	// return num_vals_ret;
// }


/* This function tries to have a complete last row, while always returning at least one row.
 * 'i_s' is assumed to be non-empty.
 */
static inline
long
select_num_vals_complete_last_row(INT_T * row_ptr, long i_s, long j_s, long num_vals, long force_row_index_lte_1_byte)
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
	if (force_row_index_lte_1_byte)
	{
		if (num_rows > num_rows_max)
			i_e = i_s + num_rows_max;
	}
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
test_permutation(long num_vals, ValueTypeReference * vals, ValueType * window, int * rev_permutation, char * msg)
{
	int tnum = omp_get_thread_num();
	long i, j;
	for (j=0;j<num_vals;j++)
	{
		if (rev_permutation[j] < 0 || rev_permutation[j] > num_vals)
			error("permutation values out of bounds");
		if (window[j] != (ValueType) vals[rev_permutation[j]])
		{
			if (tnum == 0)
			{
				for (i=0;i<num_vals;i++)
				{
					printf("% 30.20g  % 30.20g\n", window[i], (ValueType) vals[rev_permutation[i]]);
				}
				printf("\n");
			}
			error("wrong permutation: %s", msg);
		}
	}
}


static inline
long
compress_kernel_div_base(unsigned char * buf,
		long num_vals,
		ValueType * window,
		unsigned int * rows,
		unsigned int * cols,
		long force_row_index_lte_1_byte,
		long force_row_index_rounding,
		long symmetric
		)
{
	int tnum = omp_get_thread_num();
	struct thread_data * td = tds[tnum];
	struct thread_data_kernel * tdk = tdks[tnum];
	long num_vals_unique;
	ValueType * window_buf = tdk->window_buf;
	int * window_rf = tdk->window_rf;
	int * window_rf_buf = tdk->window_rf_buf;
	struct packet_header * header;
	unsigned char ** data_val_lanes_buf = tdk->data_lanes;
	uint32_t * data_val_lanes_size;
	unsigned int * rows_buf = tdk->rows_buf;
	unsigned int * cols_buf = tdk->cols_buf;
	int * permutation = tdk->permutation;
	int * rev_permutation = tdk->rev_permutation;
	int * rev_permutation_total = tdk->rev_permutation_total;
	int * rev_permutation_total_buf = tdk->rev_permutation_total_buf;
	int * offsets_rf = tdk->offsets_rf;
	int * rfs = tdk->rfs;
	int * num_unique_vals_per_rf = tdk->num_unique_vals_per_rf;
	struct Byte_Stream Bs[VEC_LEN];
	uint64_t len_bits = 0;
	uint64_t len = 0;
	uint64_t row = 0, row_min, row_max, row_diff = 0, row_diff_max = 0, col = 0, col_min = 0, col_max = 0, col_diff = 0, col_diff_max = 0;   // row_max is the last row (i.e., inclusive: num_rows = row_max + 1 - i_s)
	uint64_t row_bits = 0, col_bits = 0;
	uint64_t coords;
	uint64_t coords_bytes = 0;
	long num_rfs;
	long num_rf_vals, num_rf_vals_unique;
	unsigned char * ptr_next;
	long i, j, k, k_s, l, l_s, l_e;
	long rf;
	long lane_id;

	double tolerance = atof(getenv("DIV_VC_TOLERANCE"));

	long dv_enabled = DV_ENABLED;
	long rf_enabled = RF_ENABLED;
	// if (symmetric)
		// dv_enabled = 0;

	row_min = rows[0];
	row_max = rows[0];
	col_min = cols[0];
	col_max = cols[0];
	for (k=0,i=row_min; k<num_vals; k++)
	{
		rev_permutation_total[k] = k;
		row = rows[k];
		if (row < row_min)
			row_min = row;
		if (row > row_max)
			row_max = row;
		col = cols[k];
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

	/* Sort the packet values. */
	struct cmp_data compare_data;
	compare_data.vals = window;
	compare_data.rows = rows;
	compare_data.cols = cols;
	for (k=0;k<num_vals;k++)
		rev_permutation[k] = k;
	if (symmetric)
		quicksort_vals_sym(rev_permutation, num_vals, &compare_data, tdk->qsort_partitions);
	else
		quicksort_vals(rev_permutation, num_vals, &compare_data, tdk->qsort_partitions);
	// integer_sort(&(rev_permutation[0]), &(rev_permutation[0])+num_vals, [window] (const int& x, const int& y) { return quicksort_cmp(x, y, window) < 0; });
	// std::sort(&(rev_permutation[0]), &(rev_permutation[0])+num_vals, [window] (const int& x, const int& y) { return quicksort_cmp(x, y, window) < 0; });
	for (k=0;k<num_vals;k++)
	{
		// printf("%d\n", rev_permutation[k]);
		window_buf[k] = window[rev_permutation[k]];
		rows_buf[k] = rows[rev_permutation[k]];
		cols_buf[k] = cols[rev_permutation[k]];
		rev_permutation_total_buf[k] = rev_permutation_total[rev_permutation[k]];
	}
	macros_swap(&window, &window_buf);
	macros_swap(&rows, &rows_buf);
	macros_swap(&cols, &cols_buf);
	macros_swap(&rev_permutation_total, &rev_permutation_total_buf);

	// if (tolerance == 0)
		// test_permutation(num_vals, vals, window, rev_permutation_total, (char *) "quicksort");

	/* Find replication factors, but keep all duplicates (needed because each one has different coordinates). */
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
			rf++;
		}
		for (l=k;l<j;l++)
			window_rf[l] = rf;
		if (rf > max_rf)
			max_rf = rf;
		k = j;
		num_vals_unique++;
	}

	// double unique_values_fraction = ((double) num_vals_unique) / ((double) num_vals);
	// printf("test = %f\n", unique_values_fraction);
	// if (unique_values_fraction < 0.05)
	// {
		// printf("test = %f\n", unique_values_fraction);
		// dv_enabled = 0;
	// }

	/* Stable sort by replication factors. */
	long num_buckets = max_rf + 1;
	bucketsort_stable_serial(window_rf, num_vals, num_buckets, NULL, permutation, offsets_rf, NULL);
	for (k=0;k<num_vals;k++)
	{
		window_buf[permutation[k]] = window[k];
		window_rf_buf[permutation[k]] = window_rf[k];
		rows_buf[permutation[k]] = rows[k];
		cols_buf[permutation[k]] = cols[k];
		rev_permutation_total_buf[permutation[k]] = rev_permutation_total[k];
	}
	macros_swap(&window, &window_buf);
	macros_swap(&window_rf, &window_rf_buf);
	macros_swap(&rows, &rows_buf);
	macros_swap(&cols, &cols_buf);
	macros_swap(&rev_permutation_total, &rev_permutation_total_buf);

	// if (tolerance == 0)
		// test_permutation(num_vals, vals, window, rev_permutation_total, (char *) "bucketsort");

	// /* If div disabled, sort rf==1 by the coordinates like CSR. */
	// if (!dv_enabled)
	// {
		// l_s = offsets_rf[1];
		// if (l_s != 0)
			// error("offsets_rf[1] != 0 : %ld", l_s);   // rf==0 doesn't make sense.
		// l_e = offsets_rf[2];
		// num_rf_vals = l_e - l_s;
		// if (num_rf_vals > 0)
		// {
			// for (k=0;k<num_vals;k++)
				// rev_permutation[k] = k;
			// compare_data.vals = window;
			// compare_data.rows = rows;
			// compare_data.cols = cols;
			// quicksort_csr(rev_permutation, num_rf_vals, &compare_data, tdk->qsort_partitions);
			// for (k=0;k<num_vals;k++)
			// {
				// window_buf[k] = window[rev_permutation[k]];
				// window_rf_buf[k] = window_rf[rev_permutation[k]];
				// rows_buf[k] = rows[rev_permutation[k]];
				// cols_buf[k] = cols[rev_permutation[k]];
				// rev_permutation_total_buf[k] = rev_permutation_total[rev_permutation[k]];
			// }
			// macros_swap(&window, &window_buf);
			// macros_swap(&window_rf, &window_rf_buf);
			// macros_swap(&rows, &rows_buf);
			// macros_swap(&cols, &cols_buf);
			// macros_swap(&rev_permutation_total, &rev_permutation_total_buf);
			// if (tolerance == 0)
				// test_permutation(num_vals, vals, window, rev_permutation_total, (char *) "quicksort rf 1");
		// }
	// }

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
	if (k != num_vals_unique)
		error("k=%ld != num_vals_unique=%ld", k, num_vals_unique);

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
		permutation_interleave(rows + l_s, rows_buf + l_s, num_rf_vals, VEC_LEN, rf, 0, 1);
		permutation_interleave(cols + l_s, cols_buf + l_s, num_rf_vals, VEC_LEN, rf, 0, 1);

		k += num_rf_vals_unique;
	}
	macros_swap(&window, &window_buf);
	macros_swap(&window_rf, &window_rf_buf);
	macros_swap(&rows, &rows_buf);
	macros_swap(&cols, &cols_buf);

	ptr_next = buf;

	header = (typeof(header)) ptr_next;
	uint64_t data_intro_bytes = sizeof(*header);
	ptr_next += data_intro_bytes;

	header->symmetric = symmetric;
	header->dv_enabled = dv_enabled;
	header->rf_enabled = rf_enabled;
	header->row_bits = row_bits;
	header->col_bits = col_bits;
	header->num_vals = num_vals;
	header->num_vals_unique = num_vals_unique;
	header->num_rows = row_diff_max + 1;
	header->row_min = row_min;
	header->col_min = col_min;
	header->num_rfs = num_rfs;

	data_val_lanes_size = header->data_val_lanes_size;

	uint32_t * data_rfs = (uint32_t *) ptr_next;
	const uint64_t data_rfs_bytes = sizeof(*data_rfs) * num_rfs;
	ptr_next += data_rfs_bytes;

	uint32_t * data_num_unique_vals_per_rf = (uint32_t *) ptr_next;
	const uint64_t data_num_unique_vals_per_rf_bytes = sizeof(*data_num_unique_vals_per_rf) * num_rfs;
	ptr_next += data_num_unique_vals_per_rf_bytes;

	for (i=0;i<num_rfs;i++)
	{
		data_rfs[i] = rfs[i];
		data_num_unique_vals_per_rf[i] = num_unique_vals_per_rf[i];
	}

	unsigned char * data_coords = ptr_next;
	const uint64_t data_coords_bytes = coords_bytes * num_vals;
	ptr_next += data_coords_bytes;

	for (i=0;i<num_vals;i++)
	{
		row_diff = rows[i] - row_min;
		col_diff = cols[i] - col_min;
		coords = row_diff | (col_diff << row_bits);
		memcpy(&data_coords[i*coords_bytes], &coords, coords_bytes);
	}

	if (!dv_enabled)
	{
		ValueType * data_vals = (ValueType *) ptr_next;
		const uint64_t data_vals_bytes = num_vals_unique * sizeof(*data_vals);
		ptr_next += data_vals_bytes;
		for (i=0;i<num_vals_unique;i++)
			data_vals[i] = window[i];
	}
	else
	{
		unsigned char * data_val_lens = ptr_next;
		const uint64_t data_val_lens_bytes = num_vals_unique;
		ptr_next += data_val_lens_bytes;

		for (long iter=0;iter<VEC_LEN;iter++)
			bytestream_init_write(&Bs[iter], data_val_lanes_buf[iter]);

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

		uint64_t data_val_lanes_bytes = 0;
		for (long iter=0;iter<VEC_LEN;iter++)
		{
			memcpy(ptr_next, data_val_lanes_buf[iter], Bs[iter].len);
			ptr_next += Bs[iter].len;
			data_val_lanes_bytes += Bs[iter].len;
		}
	}

	td->row_bits_accum += row_bits * num_vals;
	td->col_bits_accum += col_bits * num_vals;
	td->row_col_bytes_accum += coords_bytes * num_vals;
	td->packet_unique_values_fraction_accum += ((double) num_vals_unique) / ((double) num_vals);

	header->size = ptr_next - buf;
	return header->size;
}


/*
 * 'i_s' is certain to be the first non-empty row.
 */
static inline
void
compress_kernel_div(INT_T * row_ptr, INT_T * ja, ValueTypeReference * vals, [[gnu::unused]] long symmetric, long i_s, [[gnu::unused]] long i_t_s, [[gnu::unused]] long i_t_e, long j_s,
		unsigned char * buf_stealable, [[gnu::unused]] unsigned char * buf_protected, long num_vals, long * num_vals_out, long * size_stealable_out, long * size_protected_out)
{
	int tnum = omp_get_thread_num();
	struct thread_data_kernel * tdk = tdks[tnum];
	long num_vals_sym;
	ValueType * window = tdk->window;
	ValueType * window_sym = tdk->window_sym;
	unsigned int * rows = tdk->rows;
	unsigned int * cols = tdk->cols;
	unsigned int * rows_sym = tdk->rows_sym;
	unsigned int * cols_sym = tdk->cols_sym;
	uint64_t row_min, col = 0;
	long i, j, k, l;

	if (num_vals_out == NULL)
		error("num_vals_out == NULL");

	double tolerance = atof(getenv("DIV_VC_TOLERANCE"));

	long force_row_index_lte_1_byte = 1;
	long force_row_index_rounding = 0;

	if (num_vals <= 0)
		error("packet with no nnz requested");
	if (row_ptr[i_s+1] - row_ptr[i_s] == 0)
		error("empty first row");

	row_min = i_s;   // 'i_s' is certain to be the first non-empty row.

	// long num_vals = select_num_vals(row_ptr, row_min, j_s, num_vals, force_row_index_lte_1_byte);
	num_vals = select_num_vals_complete_last_row(row_ptr, row_min, j_s, num_vals, force_row_index_lte_1_byte);
	if (num_vals == 0)
		error("empty packet");

	*num_vals_out = num_vals;

	// long i_e = i_s;
	// long j_e = j_s + num_vals;
	// while (row_ptr[i_e] < j_e)
		// i_e++;

	num_vals_sym = 0;
	k = 0;
	for (l=0,i=row_min,j=j_s; l<num_vals; l++,j++)
	{
		ValueType val;
		while (j >= row_ptr[i+1])
			i++;
		if (tolerance > 0)
			val = reduce_precision(vals[l], tolerance);
		else
			val = vals[l];
		col = ja[j];
		if (symmetric)
		{
			if (col > (uint64_t) i)  // Filter out the symmetric thread local nnz above the diagonal.
			{
				if ((col >= (uint64_t) i_t_s) && (col < (uint64_t) i_t_e))
				// if ((col >= (uint64_t) i_s) && (col < (uint64_t) i_e))
				// if ((col >= (uint64_t) i_t_s) && (col < (uint64_t) i_t_e) && (labs(col - i) < 256))
				// if ((col >= (uint64_t) i_s) && (col < (uint64_t) i_s + 256))
					continue;
			}
			if (col <  (uint64_t) i)  // Extract the symmetric thread local nnz below the diagonal.
			{
				if ((col >= (uint64_t) i_t_s) && (col < (uint64_t) i_t_e))
				// if ((col >= (uint64_t) i_s) && (col < (uint64_t) i_e))
				// if ((col >= (uint64_t) i_t_s) && (col < (uint64_t) i_t_e) && (labs(col - i) < 256))
				// if ((col >= (uint64_t) i_s) && (col < (uint64_t) i_s + 256))
				{
					window_sym[num_vals_sym] = val;
					rows_sym[num_vals_sym] = i;
					cols_sym[num_vals_sym] = col;
					num_vals_sym++;
					continue;
				}
			}
		}
		window[k] = val;
		rows[k] = i;
		cols[k] = col;
		k++;
	}
	num_vals = k;
	// printf("num_vals=%ld, num_vals_sym=%ld\n", num_vals, num_vals_sym);

	unsigned char * data = buf_stealable;
	uint64_t data_bytes = 0;
	long packet_bytes = 0, packet_bytes_sym = 0;

	if (num_vals > 0)
	{
		packet_bytes = compress_kernel_div_base(&data[data_bytes],
			num_vals,
			window,
			rows,
			cols,
			force_row_index_lte_1_byte,
			force_row_index_rounding,
			0
			);
		data_bytes += packet_bytes;
	}

	if (num_vals_sym > 0)
	{
		// error("test");
		packet_bytes_sym = compress_kernel_div_base(&data[data_bytes],
			num_vals_sym,
			window_sym,
			rows_sym,
			cols_sym,
			force_row_index_lte_1_byte,
			force_row_index_rounding,
			1
			);
		data_bytes += packet_bytes_sym;
	}

	*size_stealable_out = data_bytes;
	*size_protected_out = 0;
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
mult_add_serial(ValueType * x_rel, ValueType * y_rel, [[gnu::unused]] ValueType * x, [[gnu::unused]] ValueType * y, ValueType val, uint64_t row_rel, uint64_t col_rel, [[gnu::unused]] uint64_t row, [[gnu::unused]] uint64_t col,
		ValueType ** window_ptr, INT_T ** window_ia_ptr, INT_T ** window_ja_ptr, const int validate)
{
	if (validate)
	{
		*((*window_ptr)++)    = val;
		*((*window_ia_ptr)++) = row;
		*((*window_ja_ptr)++) = col;
	}
	else
		y_rel[row_rel] += val * x_rel[col_rel];
}


static __attribute__((always_inline)) inline
void
mult_add_vector(ValueType * x_rel, ValueType * y_rel, [[gnu::unused]] ValueType * x, [[gnu::unused]] ValueType * y, vec_t(VTF, VEC_LEN) val, vec_t(i64, VEC_LEN) row_rel, vec_t(i64, VEC_LEN) col_rel, [[gnu::unused]] vec_t(i64, VEC_LEN) row, [[gnu::unused]] vec_t(i64, VEC_LEN) col,
		ValueType ** window_ptr, INT_T ** window_ia_ptr, INT_T ** window_ja_ptr, const int validate)
{
	if (validate)
	{
		for (long iter=0;iter<VEC_LEN;iter++)
		{
			*((*window_ptr)++)    = vec_array(VTF, VEC_LEN, val)[iter];
			*((*window_ia_ptr)++) = vec_array(i64, VEC_LEN, row)[iter];
			*((*window_ja_ptr)++) = vec_array(i64, VEC_LEN, col)[iter];
		}
	}
	else
	{
		PRAGMA(GCC unroll VEC_LEN)
		for (long iter=0;iter<VEC_LEN;iter++)
			y_rel[vec_array(i64, VEC_LEN, row_rel)[iter]] += vec_array(VTF, VEC_LEN, val)[iter] * x_rel[vec_array(i64, VEC_LEN, col_rel)[iter]];
	}
}


static __attribute__((always_inline)) inline
void
mult_add_serial_sym([[gnu::unused]] ValueType * x_rel, [[gnu::unused]] ValueType * y_rel, ValueType * x, ValueType * y, ValueType val, [[gnu::unused]] uint64_t row_rel, [[gnu::unused]] uint64_t col_rel, uint64_t row, uint64_t col,
		ValueType ** window_ptr, INT_T ** window_ia_ptr, INT_T ** window_ja_ptr, const int validate)
{
	if (validate)
	{
		*((*window_ptr)++)    = val;
		*((*window_ia_ptr)++) = row;
		*((*window_ja_ptr)++) = col;
		*((*window_ptr)++)    = val;
		*((*window_ia_ptr)++) = col;
		*((*window_ja_ptr)++) = row;
	}
	else
	{
		y[row] += val * x[col];
		y[col] += val * x[row];
	}
}


static __attribute__((always_inline)) inline
void
mult_add_vector_sym([[gnu::unused]] ValueType * x_rel, [[gnu::unused]] ValueType * y_rel, ValueType * x, ValueType * y, vec_t(VTF, VEC_LEN) val, [[gnu::unused]] vec_t(i64, VEC_LEN) row_rel, [[gnu::unused]] vec_t(i64, VEC_LEN) col_rel, vec_t(i64, VEC_LEN) row, vec_t(i64, VEC_LEN) col,
		ValueType ** window_ptr, INT_T ** window_ia_ptr, INT_T ** window_ja_ptr, const int validate)
{
	if (validate)
	{
		for (long iter=0;iter<VEC_LEN;iter++)
		{
			*((*window_ptr)++)    = vec_array(VTF, VEC_LEN, val)[iter];
			*((*window_ia_ptr)++) = vec_array(i64, VEC_LEN, row)[iter];
			*((*window_ja_ptr)++) = vec_array(i64, VEC_LEN, col)[iter];
			*((*window_ptr)++)    = vec_array(VTF, VEC_LEN, val)[iter];
			*((*window_ia_ptr)++) = vec_array(i64, VEC_LEN, col)[iter];
			*((*window_ja_ptr)++) = vec_array(i64, VEC_LEN, row)[iter];
		}
	}
	else
	{
		// Seems important for performance stability to split off the symmetric values to another loop.
		PRAGMA(GCC unroll VEC_LEN)
		for (long iter=0;iter<VEC_LEN;iter++)
			y[vec_array(i64, VEC_LEN, row)[iter]] += vec_array(VTF, VEC_LEN, val)[iter] * x[vec_array(i64, VEC_LEN, col)[iter]];
		PRAGMA(GCC unroll VEC_LEN)
		for (long iter=0;iter<VEC_LEN;iter++)
			y[vec_array(i64, VEC_LEN, col)[iter]] += vec_array(VTF, VEC_LEN, val)[iter] * x[vec_array(i64, VEC_LEN, row)[iter]];
	}
}


//==========================================================================================================================================
//= Decompress DIV_RF
//==========================================================================================================================================


static __attribute__((always_inline)) inline
long
decompress_and_compute_kernel_div_base(unsigned char * restrict buf, ValueType * restrict x, ValueType * restrict y, long * restrict num_vals_out, const int validate,
		const long dv_enabled,
		void (* gather_coords)(long i, unsigned char * data_coords, const uint64_t coords_bytes, uint64_t row_bits, uint64_t col_bits, uint64_t * row_rel_out, uint64_t * col_rel_out),
		void (* gather_coords_v)(long i, unsigned char * data_coords, const uint64_t coords_bytes, uint64_t row_bits, uint64_t col_bits, vec_t(i64, VEC_LEN) * row_rel_out, vec_t(i64, VEC_LEN) * col_rel_out),
		void (* mult_add)(ValueType * x_rel, ValueType * y_rel, ValueType * x, ValueType * y, ValueType val,
		                  uint64_t row_rel, uint64_t col_rel, uint64_t row, uint64_t col,
		                  ValueType ** window_ptr, INT_T ** window_ia_ptr, INT_T ** window_ja_ptr, const int validate),
		void (* mult_add_v)(ValueType * x_rel, ValueType * y_rel, ValueType * x, ValueType * y, vec_t(VTF, VEC_LEN) val,
		                    vec_t(i64, VEC_LEN) row_rel, vec_t(i64, VEC_LEN) col_rel, vec_t(i64, VEC_LEN) row, vec_t(i64, VEC_LEN) col,
		                    ValueType ** window_ptr, INT_T ** window_ia_ptr, INT_T ** window_ja_ptr, const int validate)
		)
{
	int tnum = omp_get_thread_num();
	struct thread_data_kernel * tdk = tdks[tnum];
	struct packet_header * header;
	ValueType * x_rel;
	ValueType * y_rel;
	ValueType * window = tdk->window;
	INT_T * window_ia = tdk->window_ia;
	INT_T * window_ja = tdk->window_ja;
	long num_vals;
	long num_vals_unique;
	uint64_t row_min, col_min;
	[[gnu::unused]] long num_rows;
	uint64_t row_bits, col_bits;
	long num_rfs;
	uint32_t * data_val_lanes_size;
	unsigned char * ptr_next;
	long i, j, k, l;

	ptr_next = buf;

	header = (typeof(header)) ptr_next;
	uint64_t data_intro_bytes = sizeof(*header);
	ptr_next += data_intro_bytes;

	row_bits = header->row_bits;
	col_bits = header->col_bits;
	num_vals = header->num_vals;
	num_vals_unique = header->num_vals_unique;
	num_rows = header->num_rows;
	row_min = header->row_min;
	col_min = header->col_min;
	num_rfs = header->num_rfs;

	const uint64_t coords_bytes = (row_bits + col_bits + 7) >> 3;

	data_val_lanes_size = header->data_val_lanes_size;

	y_rel = y + row_min;
	x_rel = x + col_min;

	uint32_t * data_rfs = (uint32_t *) ptr_next;
	const uint64_t data_rfs_bytes = sizeof(*data_rfs) * num_rfs;
	ptr_next += data_rfs_bytes;

	uint32_t * data_num_unique_vals_per_rf = (uint32_t *) ptr_next;
	const uint64_t data_num_unique_vals_per_rf_bytes = sizeof(*data_num_unique_vals_per_rf) * num_rfs;
	ptr_next += data_num_unique_vals_per_rf_bytes;

	unsigned char * data_coords = ptr_next;
	const uint64_t data_coords_bytes = coords_bytes * num_vals;
	ptr_next += data_coords_bytes;

	unsigned char * data_val_lens = NULL;
	uint64_t data_val_lens_bytes;
	vec_t(i64, VEC_LEN) data_val_lanes;
	uint64_t data_val_lanes_bytes;

	ValueType * data_vals = NULL;
	uint64_t data_vals_bytes;

	if (dv_enabled)
	{
		data_val_lens = ptr_next;
		data_val_lens_bytes = num_vals_unique;
		ptr_next += data_val_lens_bytes;

		vec_array(i64, VEC_LEN, data_val_lanes)[0] = (long long) ptr_next;
		data_val_lanes_bytes = data_val_lanes_size[0];
		for (long iter=1;iter<VEC_LEN;iter++)
		{
			vec_array(i64, VEC_LEN, data_val_lanes)[iter] = vec_array(i64, VEC_LEN, data_val_lanes)[iter-1] + data_val_lanes_size[iter-1];
			data_val_lanes_bytes += data_val_lanes_size[iter];
		}
		ptr_next += data_val_lanes_bytes;
	}
	else
	{
		data_vals = (ValueType *) ptr_next;
		data_vals_bytes = num_vals_unique * sizeof(*data_vals);
		ptr_next += data_vals_bytes;
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
			vec_t(i64, VEC_LEN) row_rel, col_rel;
			if (dv_enabled)
			{
				union {
					vec_t(VTF, VEC_LEN) d;
					vec_t(VTI, VEC_LEN) u;
				} diff;
				vec_t(i64, VEC_LEN) len_64;
				vec_t(VTI, VEC_LEN) len;

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
			}
			else
			{
				val.d = vec_loadu(VTF, VEC_LEN, &data_vals[i]);
			}

			for (j=0;j<rf;j++)
			{
				k = i*rf + VEC_LEN*j;
				gather_coords_v(k, data_coords, coords_bytes, row_bits, col_bits, &row_rel, &col_rel);

				vec_t(i64, VEC_LEN) row, col;
				row = vec_add(i64, VEC_LEN, row_rel, vec_set1(i64, VEC_LEN, row_min));
				col = vec_add(i64, VEC_LEN, col_rel, vec_set1(i64, VEC_LEN, col_min));

				mult_add_v(x_rel, y_rel, x, y, val.d, row_rel, col_rel, row, col, &window, &window_ia, &window_ja, validate);
			}
		}

		for (i=num_rf_vals_unique_multiple;i<num_rf_vals_unique;i++)
		{
			long lane_id;
			if (dv_enabled)
			{
				union {
					ValueType d;
					ValueTypeI u;
				} diff;
				uint64_t len;
				lane_id = i % VEC_LEN;

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
			}
			else
			{
				lane_id = 0;
				vec_array(VTF, VEC_LEN, val.d)[0] = data_vals[i];
			}

			long rf_div = rf / VEC_LEN;
			long rf_multiple = rf_div * VEC_LEN;
			for (j=0;j<rf_multiple;j+=VEC_LEN)
			{
				vec_t(i64, VEC_LEN) row_rel, col_rel;
				vec_t(i64, VEC_LEN) row, col;
				vec_t(VTF, VEC_LEN) val_buf = vec_set1(VTF, VEC_LEN, vec_array(VTF, VEC_LEN, val.d)[lane_id]);
				k = i*rf + j;
				gather_coords_v(k, data_coords, coords_bytes, row_bits, col_bits, &row_rel, &col_rel);
				row = vec_add(i64, VEC_LEN, row_rel, vec_set1(i64, VEC_LEN, row_min));
				col = vec_add(i64, VEC_LEN, col_rel, vec_set1(i64, VEC_LEN, col_min));
				mult_add_v(x_rel, y_rel, x, y, val_buf, row_rel, col_rel, row, col, &window, &window_ia, &window_ja, validate);
			}

			for (j=rf_multiple;j<rf;j++)
			{
				uint64_t row_rel, col_rel;
				uint64_t row, col;
				k = i*rf + j;
				gather_coords(k, data_coords, coords_bytes, row_bits, col_bits, &row_rel, &col_rel);
				row = row_rel + row_min;
				col = col_rel + col_min;
				mult_add(x_rel, y_rel, x, y, vec_array(VTF, VEC_LEN, val.d)[lane_id], row_rel, col_rel, row, col, &window, &window_ia, &window_ja, validate);
			}
		}

		data_val_lens += num_rf_vals_unique;
		data_vals += num_rf_vals_unique;
		data_coords += coords_bytes * rf * num_rf_vals_unique;
	}

	if (num_vals_out != NULL)
		*num_vals_out = num_vals;
	return ptr_next - buf;
}


static __attribute__((always_inline)) inline
long
decompress_and_compute_kernel_div_select(unsigned char * restrict buf, ValueType * restrict x, ValueType * restrict y, long * restrict num_vals_out, const int validate,
		const long dv_enabled,
		void (* mult_add)(ValueType * x_rel, ValueType * y_rel, ValueType * x, ValueType * y, ValueType val,
		                  uint64_t row_rel, uint64_t col_rel, uint64_t row, uint64_t col,
		                  ValueType ** window_ptr, INT_T ** window_ia_ptr, INT_T ** window_ja_ptr, const int validate),
		void (* mult_add_v)(ValueType * x_rel, ValueType * y_rel, ValueType * x, ValueType * y, vec_t(VTF, VEC_LEN) val,
		                    vec_t(i64, VEC_LEN) row_rel, vec_t(i64, VEC_LEN) col_rel, vec_t(i64, VEC_LEN) row, vec_t(i64, VEC_LEN) col,
		                    ValueType ** window_ptr, INT_T ** window_ia_ptr, INT_T ** window_ja_ptr, const int validate)
		)
{
	struct packet_header * header = (typeof(header)) buf;
	uint64_t row_bits, col_bits;

	row_bits = header->row_bits;
	col_bits = header->col_bits;

	const uint64_t coords_bytes = (row_bits + col_bits + 7) >> 3;

	if (__builtin_expect(row_bits == 8, 1))
	// if (row_bits == 8)
	{
		switch (col_bits) {
			case 8:
				return decompress_and_compute_kernel_div_base(buf, x, y, num_vals_out, validate, dv_enabled, gather_coords_sparse_r1_c1, gather_coords_sparse_r1_c1_v, mult_add, mult_add_v);
			case 16:
				return decompress_and_compute_kernel_div_base(buf, x, y, num_vals_out, validate, dv_enabled, gather_coords_sparse_r1_c2, gather_coords_sparse_r1_c2_v, mult_add, mult_add_v);
			case 24:
				return decompress_and_compute_kernel_div_base(buf, x, y, num_vals_out, validate, dv_enabled, gather_coords_sparse_r1_c3, gather_coords_sparse_r1_c3_v, mult_add, mult_add_v);
			case 32:
				return decompress_and_compute_kernel_div_base(buf, x, y, num_vals_out, validate, dv_enabled, gather_coords_sparse_r1_c4, gather_coords_sparse_r1_c4_v, mult_add, mult_add_v);
		}
	}

	/* Huge rows. */
	if (row_bits == 0)
	{
		switch (col_bits) {
			case 8:
				return decompress_and_compute_kernel_div_base(buf, x, y, num_vals_out, validate, dv_enabled, gather_coords_sparse_r0_c1, gather_coords_sparse_r0_c1_v, mult_add, mult_add_v);
			case 16:
				return decompress_and_compute_kernel_div_base(buf, x, y, num_vals_out, validate, dv_enabled, gather_coords_sparse_r0_c2, gather_coords_sparse_r0_c2_v, mult_add, mult_add_v);
			case 24:
				return decompress_and_compute_kernel_div_base(buf, x, y, num_vals_out, validate, dv_enabled, gather_coords_sparse_r0_c3, gather_coords_sparse_r0_c3_v, mult_add, mult_add_v);
			case 32:
				return decompress_and_compute_kernel_div_base(buf, x, y, num_vals_out, validate, dv_enabled, gather_coords_sparse_r0_c4, gather_coords_sparse_r0_c4_v, mult_add, mult_add_v);
		}
	}

	switch (coords_bytes) {
		case 1:
			return decompress_and_compute_kernel_div_base(buf, x, y, num_vals_out, validate, dv_enabled, gather_coords_dense_1, gather_coords_dense_1_v, mult_add, mult_add_v);
		case 2:
			return decompress_and_compute_kernel_div_base(buf, x, y, num_vals_out, validate, dv_enabled, gather_coords_dense_2, gather_coords_dense_2_v, mult_add, mult_add_v);
		case 3:
			return decompress_and_compute_kernel_div_base(buf, x, y, num_vals_out, validate, dv_enabled, gather_coords_dense_3, gather_coords_dense_3_v, mult_add, mult_add_v);
		case 4:
			return decompress_and_compute_kernel_div_base(buf, x, y, num_vals_out, validate, dv_enabled, gather_coords_dense_4, gather_coords_dense_4_v, mult_add, mult_add_v);
	}

	return decompress_and_compute_kernel_div_base(buf, x, y, num_vals_out, validate, dv_enabled, gather_coords_dense, gather_coords_dense_v, mult_add, mult_add_v);
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


long
get_packet_size(unsigned char * restrict buf)
{
	struct packet_header * header = (typeof(header)) buf;
	return header->size;
}


static inline
long
decompress_and_compute_kernel_div(unsigned char * restrict buf, ValueType * restrict x, ValueType * restrict y, [[gnu::unused]] long i_t_s, [[gnu::unused]] long i_t_e)
{
	struct packet_header * header = (typeof(header)) buf;
	const long symmetric = header->symmetric;
	const long dv_enabled = header->dv_enabled;
	const long rf_enabled = header->rf_enabled;

	if ((!dv_enabled) && rf_enabled)
	{
		if (symmetric)
			return decompress_and_compute_kernel_div_select(buf, x, y, NULL, 0, 0, mult_add_serial_sym, mult_add_vector_sym);
		else
			return decompress_and_compute_kernel_div_select(buf, x, y, NULL, 0, 0, mult_add_serial, mult_add_vector);
	}
	else if (dv_enabled && rf_enabled)
	{
		if (symmetric)
			return decompress_and_compute_kernel_div_select(buf, x, y, NULL, 0, 1, mult_add_serial_sym, mult_add_vector_sym);
		else
			return decompress_and_compute_kernel_div_select(buf, x, y, NULL, 0, 1, mult_add_serial, mult_add_vector);
	}

	error("bad packet type : dv_enabled=%ld rf_enabled=%ld", dv_enabled, rf_enabled);
	return -1;
}


static
long
decompress_kernel_div(INT_T * ia_out, INT_T * ja_out, ValueType * a_out, long * num_vals_out, unsigned char * restrict buf, [[gnu::unused]] long i_t_s, [[gnu::unused]] long i_t_e)
{
	long num_vals = 0;
	int tnum = omp_get_thread_num();
	struct thread_data_kernel * tdk = tdks[tnum];
	ValueType * window = tdk->window;
	INT_T * window_ia = tdk->window_ia;
	INT_T * window_ja = tdk->window_ja;
	long i, bytes;

	struct packet_header * header = (typeof(header)) buf;
	const long symmetric = header->symmetric;
	const long dv_enabled = header->dv_enabled;
	const long rf_enabled = header->rf_enabled;

	if ((!dv_enabled) && rf_enabled)
	{
		if (symmetric)
			bytes = decompress_and_compute_kernel_div_select(buf, NULL, NULL, &num_vals, 1, 0, mult_add_serial_sym, mult_add_vector_sym);
		else
			bytes = decompress_and_compute_kernel_div_select(buf, NULL, NULL, &num_vals, 1, 0, mult_add_serial, mult_add_vector);
	}
	else if (dv_enabled && rf_enabled)
	{
		if (symmetric)
			bytes = decompress_and_compute_kernel_div_select(buf, NULL, NULL, &num_vals, 1, 1, mult_add_serial_sym, mult_add_vector_sym);
		else
			bytes = decompress_and_compute_kernel_div_select(buf, NULL, NULL, &num_vals, 1, 1, mult_add_serial, mult_add_vector);
	}
	else
	{
		error("bad packet type : dv_enabled=%ld rf_enabled=%ld", dv_enabled, rf_enabled);
		if (num_vals_out != NULL)
			*num_vals_out = -1;
		return -1;
	}

	if (symmetric)
		num_vals *= 2;

	for (i=0;i<num_vals;i++)
	{
		a_out[i] = window[i];
		ia_out[i] = window_ia[i];
		ja_out[i] = window_ja[i];
	}

	*num_vals_out = num_vals;
	return bytes;
}


//==========================================================================================================================================
//= Packet Sorting
//==========================================================================================================================================


int
packet_type_id(unsigned char * restrict buf)
{
	if (buf == NULL)
		return 24;
	struct packet_header * header = (typeof(header)) buf;
	uint64_t row_bits, col_bits;
	[[gnu::unused]] const long symmetric = header->symmetric;
	row_bits = header->row_bits;
	col_bits = header->col_bits;
	const uint64_t coords_bytes = (row_bits + col_bits + 7) >> 3;
	long ret = 0;
	if (row_bits == 0)
	{
		switch (col_bits) {
			case  8: ret = 0; break;
			case 16: ret = 1; break;
			case 24: ret = 2; break;
			case 32: ret = 3; break;
		}
	}
	else if (row_bits == 8)
	{
		switch (col_bits) {
			case  8: ret = 4; break;
			case 16: ret = 5; break;
			case 24: ret = 6; break;
			case 32: ret = 7; break;
		}
		ret = 4;
	}
	else
	{
		switch (coords_bytes) {
			case 1: ret = 8; break;
			case 2: ret = 9; break;
			case 3: ret = 10; break;
			case 4: ret = 11; break;
		}
		ret = 5;
	}
	// if (!symmetric)
		// ret += 12;
	// if (symmetric)
		// ret += 12;
	return ret;
}

