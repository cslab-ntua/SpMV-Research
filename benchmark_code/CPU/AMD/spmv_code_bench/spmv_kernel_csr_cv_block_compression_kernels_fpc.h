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
#ifdef __cplusplus
}
#endif


#define FORMAT_SUBNAME  "FPC"


//==========================================================================================================================================
//= FPC
//==========================================================================================================================================


void
compress_kernel_init(__attribute__((unused)) ValueType * vals, __attribute__((unused)) const long num_vals, __attribute__((unused)) const long packet_size)
{
}


// #define PREDSIZEM1_LOG  8
#define PREDSIZEM1_LOG  10


static const unsigned long long mask[8] = {
	0x0000000000000000LL,
	0x00000000000000ffLL,
	0x000000000000ffffLL,
	0x0000000000ffffffLL,
	0x000000ffffffffffLL,
	0x0000ffffffffffffLL,
	0x00ffffffffffffffLL,
	0xffffffffffffffffLL
};

static inline
long
compress_kernel(double * vals, unsigned char * buf, const long num_vals)
{
	uint64_t * data_buf = (typeof(data_buf)) vals;
	long i, out, hash, dhash, code, bcode = 0;
	int64_t val, lastval, stride, pred1, pred2, xor1, xor2;
	static __thread int64_t *fcm = NULL, *dfcm = NULL;
	long predsizem1;

	// assert(0 == ((long)buf & 0x7));

	buf[0] = PREDSIZEM1_LOG;
	buf++;

	predsizem1 = (1L << PREDSIZEM1_LOG) - 1;  // size = 2 ^ PREDSIZEM1_LOG

	hash = 0;
	dhash = 0;
	lastval = 0;
	pred1 = 0;
	pred2 = 0;
	if (fcm == NULL)
	{
		fcm = (int64_t *) malloc((predsizem1+1) * 8);
		dfcm = (int64_t *) malloc((predsizem1+1) * 8);
	}
	memset(fcm, 0, (predsizem1+1) * 8);
	memset(dfcm, 0, (predsizem1+1) * 8);

	// printf("val = %g\n", vals[0]);
	// printf("val = %ld\n", data_buf[0]);
	// printf("mem: %p %p\n", vals, data_buf);
	val = data_buf[0];
	// printf("val = %g\n", vals[0]);

	out = 6 + ((num_vals + 1) >> 1);
	*((int64_t *)&buf[(out >> 3) << 3]) = 0;
	for (i = 0; i < num_vals; i += 2) {
		xor1 = val ^ pred1;
		fcm[hash] = val;
		hash = ((hash << 6) ^ ((uint64_t)val >> 48)) & predsizem1;
		pred1 = fcm[hash];

		stride = val - lastval;
		xor2 = val ^ (lastval + pred2);
		lastval = val;
		val = data_buf[i + 1];
		dfcm[dhash] = stride;
		dhash = ((dhash << 2) ^ ((uint64_t)stride >> 40)) & predsizem1;
		pred2 = dfcm[dhash];

		code = 0;
		if ((uint64_t)xor1 > (uint64_t)xor2) {
			code = 0x80;
			xor1 = xor2;
		}
		bcode = 7;                      // 8 bytes
		if (0 == (xor1 >> 56))
			bcode = 6;              // 7 bytes
		if (0 == (xor1 >> 48))
			bcode = 5;              // 6 bytes
		if (0 == (xor1 >> 40))
			bcode = 4;              // 5 bytes
		if (0 == (xor1 >> 24))
			bcode = 3;              // 3 bytes
		if (0 == (xor1 >> 16))
			bcode = 2;              // 2 bytes
		if (0 == (xor1 >> 8))
			bcode = 1;              // 1 byte
		if (0 == xor1)
			bcode = 0;              // 0 bytes
		code |= bcode << 4;
		*((int64_t *)&buf[(out >> 3) << 3]) |= xor1 << ((out & 0x7) << 3);
		if (0 == (out & 0x7))
			xor1 = 0;
		*((int64_t *)&buf[((out >> 3) << 3) + 8]) = (uint64_t)xor1 >> (64 - ((out & 0x7) << 3));
		out += bcode + (bcode >> 2);

		xor1 = val ^ pred1;
		fcm[hash] = val;
		hash = ((hash << 6) ^ ((uint64_t)val >> 48)) & predsizem1;
		pred1 = fcm[hash];

		stride = val - lastval;
		xor2 = val ^ (lastval + pred2);
		lastval = val;
		val = data_buf[i + 2];
		dfcm[dhash] = stride;
		dhash = ((dhash << 2) ^ ((uint64_t)stride >> 40)) & predsizem1;
		pred2 = dfcm[dhash];

		if ((uint64_t)xor1 > (uint64_t)xor2) {
			code |= 0x8;
			xor1 = xor2;
		}
		bcode = 7;                      // 8 bytes
		if (0 == (xor1 >> 56))
			bcode = 6;              // 7 bytes
		if (0 == (xor1 >> 48))
			bcode = 5;              // 6 bytes
		if (0 == (xor1 >> 40))
			bcode = 4;              // 5 bytes
		if (0 == (xor1 >> 24))
			bcode = 3;              // 3 bytes
		if (0 == (xor1 >> 16))
			bcode = 2;              // 2 bytes
		if (0 == (xor1 >> 8))
			bcode = 1;              // 1 byte
		if (0 == xor1)
			bcode = 0;              // 0 bytes
		*((int64_t *)&buf[(out >> 3) << 3]) |= xor1 << ((out & 0x7) << 3);
		if (0 == (out & 0x7))
			xor1 = 0;
		*((int64_t *)&buf[((out >> 3) << 3) + 8]) = (uint64_t)xor1 >> (64 - ((out & 0x7) << 3));
		out += bcode + (bcode >> 2);

		buf[6 + (i >> 1)] = code | bcode;
	}

	if (num_vals & 1) {
		out -= bcode + (bcode >> 2);
	}

	buf[0] = num_vals;
	buf[1] = num_vals >> 8;
	buf[2] = num_vals >> 16;
	buf[3] = out;
	buf[4] = out >> 8;
	buf[5] = out >> 16;

	return out + 1;  // + 1 at the start of the buffer (PREDSIZEM1_LOG)
}


static inline
long
decompress_kernel(double * vals, unsigned char * buf, long * num_vals_out)
{
	uint64_t * data_buf = (typeof(data_buf)) vals;
	long i, in, num_vals, hash, dhash, code, bcode, predsizem1_log, predsizem1, tmp;
	int64_t val, lastval, stride, pred1, pred2, next;
	static __thread int64_t *fcm = NULL, *dfcm = NULL;

	// assert(0 == ((long)buf & 0x7));

	predsizem1_log = buf[0];
	predsizem1 = (1L << predsizem1_log) - 1;

	hash = 0;
	dhash = 0;
	lastval = 0;
	pred1 = 0;
	pred2 = 0;
	if (fcm == NULL)
	{
		fcm = (int64_t *) malloc((predsizem1+1) * 8);
		dfcm = (int64_t *) malloc((predsizem1+1) * 8);
	}
	memset(fcm, 0, (predsizem1+1) * 8);
	memset(dfcm, 0, (predsizem1+1) * 8);

	num_vals = buf[3];
	num_vals = (num_vals << 8) | buf[2];
	num_vals = (num_vals << 8) | buf[1];
	*num_vals_out = num_vals;

	// in = buf[6];
	// in = (in << 8) | buf[5];
	// in = (in << 8) | buf[4];

	buf += 7;

	in = (num_vals + 1) >> 1;
	for (i = 0; i < num_vals; i += 2) {
		code = buf[i >> 1];

		val = *((int64_t *)&buf[(in >> 3) << 3]);
		next = *((int64_t *)&buf[((in >> 3) << 3) + 8]);
		tmp = (in & 0x7) << 3;
		val = (uint64_t)val >> tmp;
		next <<= 64 - tmp;
		if (0 == tmp)
			next = 0;
		val |= next;

		bcode = (code >> 4) & 0x7;
		val &= mask[bcode];
		in += bcode + (bcode >> 2);

		if (0 != (code & 0x80))
			pred1 = pred2;
		val ^= pred1;

		fcm[hash] = val;
		hash = ((hash << 6) ^ ((uint64_t)val >> 48)) & predsizem1;
		pred1 = fcm[hash];

		stride = val - lastval;
		dfcm[dhash] = stride;
		dhash = ((dhash << 2) ^ ((uint64_t)stride >> 40)) & predsizem1;
		pred2 = val + dfcm[dhash];
		lastval = val;

		data_buf[i] = val;

		val = *((int64_t *)&buf[(in >> 3) << 3]);
		next = *((int64_t *)&buf[((in >> 3) << 3) + 8]);
		tmp = (in & 0x7) << 3;
		val = (uint64_t)val >> tmp;
		next <<= 64 - tmp;
		if (0 == tmp)
			next = 0;
		val |= next;

		bcode = code & 0x7;
		val &= mask[bcode];
		in += bcode + (bcode >> 2);

		if (0 != (code & 0x8))
			pred1 = pred2;
		val ^= pred1;

		fcm[hash] = val;
		hash = ((hash << 6) ^ ((uint64_t)val >> 48)) & predsizem1;
		pred1 = fcm[hash];

		stride = val - lastval;
		dfcm[dhash] = stride;
		dhash = ((dhash << 2) ^ ((uint64_t)stride >> 40)) & predsizem1;
		pred2 = val + dfcm[dhash];
		lastval = val;

		data_buf[i + 1] = val;
	}

	return in + 7;
}


//==========================================================================================================================================
//= Sorted Delta
//==========================================================================================================================================


void
find_bits_of_binary_representation(uint64_t max_val, uint64_t * pow2_out, uint64_t * bits_out)
{
	uint64_t bits, pow2;
	bits = 64 - __builtin_clzl(max_val);
	pow2 = 1ULL << bits;
	*bits_out = bits;
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
static int ** t_indexes;
static int ** t_qsort_partitions;

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
	find_bits_of_binary_representation(packet_size - 1, &window_size, &window_size_bits);    // Maximum representable number we want is 'packet_size - 1'.
	printf("window_size = %ld , window_size_bits = %ld\n", window_size, window_size_bits);
	t_window = (typeof(t_window)) malloc(num_threads * sizeof(*t_window));
	t_indexes = (typeof(t_indexes)) malloc(num_threads * sizeof(*t_indexes));
	t_qsort_partitions = (typeof(t_qsort_partitions)) malloc(num_threads * sizeof(*t_qsort_partitions));
	#pragma omp parallel
	{
		int tnum = omp_get_thread_num();
		t_window[tnum] = (double *) malloc(window_size * sizeof(**t_window));
		t_indexes[tnum] = (int *) malloc(window_size * sizeof(**t_indexes));
		t_qsort_partitions[tnum] = (int *) malloc(window_size * sizeof(**t_qsort_partitions));
	}


	// double t_window[num_vals];
	// int t_qsort_partitions[num_vals];
	// long i, j, k;
	// exp_table = (typeof(exp_table)) malloc(exp_table_n * sizeof(*exp_table));
	// for (j=0;j<num_vals;j++)
		// t_window[j] = vals[j];
	// quicksort_no_malloc(t_window, num_vals, NULL, t_qsort_partitions);
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
// decompress_init_sd(ValueType * vals, const long num_vals, const long packet_size)
// {
// }


static inline
long
compress_kernel_sort_diff(ValueType * vals, unsigned char * buf, const long num_vals)
{
	int tnum = omp_get_thread_num();
	double * window = t_window[tnum];
	int * indexes = t_indexes[tnum];
	struct Bit_Stream bs;
	uint64_t trailing_zeros = 0;
	uint64_t len = 0;
	double diff = 0;
	long num_bits = 0;
	// long exp_table_n = 1<<12;
	void * ptr;
	uint64_t bits = 0;
	uint64_t index = 0;
	long i;
	for (i=0;i<num_vals;i++)
	{
		indexes[i] = i;
	}
	quicksort(indexes, num_vals, vals, t_qsort_partitions[tnum]);
	for (i=0;i<num_vals;i++)
	{
		window[i] = vals[indexes[i]];
	}

	bitstream_init_write(&bs, buf, 2 * sizeof(ValueType) * num_vals);

	bitstream_write_unsafe(&bs, num_vals, 32);
	num_bits += 32;

	index = indexes[0];
	bitstream_write_unsafe(&bs, index, window_size_bits);
	num_bits += window_size_bits;
	bitstream_write_unsafe(&bs, window[0], 64);
	num_bits += 64;
	for (i=1;i<num_vals;i++)
	{
		index = indexes[i];
		bitstream_write_unsafe(&bs, index, window_size_bits);
		num_bits += window_size_bits;

		diff = window[i] - window[i-1];
		if (diff == 0)
		{
			bitstream_write_unsafe(&bs, 53ULL, 6);   // Special 'len' code for zero value.
			num_bits += 6;
		}
		else
		{
			ptr = &diff;   // C++ cancer.
			bits = *((uint64_t *) ptr);
			trailing_zeros = __builtin_ctzl(bits);
			if (trailing_zeros >= 52)
			{
				bitstream_write_unsafe(&bs, 0ULL, 6);
				num_bits += 6;
				bitstream_write_unsafe(&bs, bits >> 52, 12);
				num_bits += 12;
			}
			else
			{
				len = 52 - trailing_zeros;        // 64 can't be represented with a 6-bit number.
				bitstream_write_unsafe(&bs, len, 6);
				num_bits += 6;
				bits >>= trailing_zeros;
				bitstream_write_unsafe(&bs, bits, 12 + len);
				bits <<= 64 - len - 12;
				num_bits += 12 + len;
			}
		}
	}
	if (num_bits & 7)
	{
		len = 8 - (num_bits & 7);
		bitstream_write_unsafe(&bs, 0ULL, len);
		num_bits += len;
	}
	bitstream_write_flush(&bs);

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
decompress_kernel_sort_diff(ValueType * vals, unsigned char * buf, long * num_vals_out)
{
	uint64_t index = 0;
	struct Bit_Stream bs;
	uint64_t len = 0;
	double diff = 0;
	double val_prev = 0;
	long num_vals = 0;
	long num_bits = 0;
	uint64_t bits;
	void * ptr;
	long i;

	bitstream_init_read(&bs, buf, 2 * sizeof(ValueType) * num_vals);

	bitstream_read_unsafe(&bs, (uint64_t *) &num_vals, 32);
	num_bits += 32;

	bitstream_read_unsafe(&bs, &index, window_size_bits);
	bitstream_read_unsafe(&bs, (uint64_t *) &val_prev, 64);
	num_bits += 64;
	vals[index] = val_prev;

	for (i=1;i<num_vals;i++)
	{
		bitstream_read_unsafe(&bs, &index, window_size_bits);
		bitstream_read_unsafe(&bs, &len, 6);
		if (len == 53)   // Special 'len' code for zero value.
		{
			vals[index] = val_prev;
		}
		else
		{
			len += 12;
			bitstream_read_unsafe(&bs, &bits, len);
			num_bits += len;
			bits <<= 64 - len;
			ptr = &bits;
			diff = *((double *) ptr);
			val_prev = val_prev + diff;
			vals[index] = val_prev;
		}
	}

	num_bits += num_vals * window_size_bits;
	num_bits += (num_vals-1) * 6;

	*num_vals_out = num_vals;
	return (num_bits + 7) / 8;
}

