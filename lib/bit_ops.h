#ifndef BIT_OPS_H
#define BIT_OPS_H

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#ifdef __x86_64__
	#include <x86intrin.h>
#endif

#include "macros/cpp_defines.h"
#include "debug.h"

/* Notes:
 *   - Naive shifting can result problems.
 *     Shifting by variable length is surprisingly slow.
 *     Also, depending on the platform, a shift can degrade to a nop instruction:
 *         Usually (not always!) shift amounts are interpreted mod 32/64.
 *         Shifting by 64 may or may not be a nop, and the result might not be 0!
 */


static inline
uint64_t
bits_num_bits_to_num_bytes(uint64_t num_bits)
{
	return (num_bits + 7ULL) >> 3ULL;
}


//==========================================================================================================================================
//= Printing
//==========================================================================================================================================


static inline
void
bits_print_bytestream(unsigned char * data, long N)
{
	long i;
	for (i=N-1;i>=0;i--)
	{
		fprintf(stdout, "%08b", data[i]);
	}
	fprintf(stdout, "\n");
}


//==========================================================================================================================================
//= Set / Unset bits
//==========================================================================================================================================


/* Positions are mod 64.
 *
 * Loading mask from array involves shift and add, but it's done in the Address Generation Unit (AGU) as part of the load pipeline of the CPU.
 */

static inline
uint64_t
bits_u64_set_high(uint64_t v, unsigned long num_bits)
{
	static uint64_t width_to_mask_table[64] = {
		0xffffffffffffffff, 0xfffffffffffffffe, 0xfffffffffffffffc, 0xfffffffffffffff8, 0xfffffffffffffff0, 0xffffffffffffffe0, 0xffffffffffffffc0, 0xffffffffffffff80,
		0xffffffffffffff00, 0xfffffffffffffe00, 0xfffffffffffffc00, 0xfffffffffffff800, 0xfffffffffffff000, 0xffffffffffffe000, 0xffffffffffffc000, 0xffffffffffff8000,
		0xffffffffffff0000, 0xfffffffffffe0000, 0xfffffffffffc0000, 0xfffffffffff80000, 0xfffffffffff00000, 0xffffffffffe00000, 0xffffffffffc00000, 0xffffffffff800000,
		0xffffffffff000000, 0xfffffffffe000000, 0xfffffffffc000000, 0xfffffffff8000000, 0xfffffffff0000000, 0xffffffffe0000000, 0xffffffffc0000000, 0xffffffff80000000,
		0xffffffff00000000, 0xfffffffe00000000, 0xfffffffc00000000, 0xfffffff800000000, 0xfffffff000000000, 0xffffffe000000000, 0xffffffc000000000, 0xffffff8000000000,
		0xffffff0000000000, 0xfffffe0000000000, 0xfffffc0000000000, 0xfffff80000000000, 0xfffff00000000000, 0xffffe00000000000, 0xffffc00000000000, 0xffff800000000000,
		0xffff000000000000, 0xfffe000000000000, 0xfffc000000000000, 0xfff8000000000000, 0xfff0000000000000, 0xffe0000000000000, 0xffc0000000000000, 0xff80000000000000,
		0xff00000000000000, 0xfe00000000000000, 0xfc00000000000000, 0xf800000000000000, 0xf000000000000000, 0xe000000000000000, 0xc000000000000000, 0x8000000000000000,
	};
	return v | width_to_mask_table[num_bits % 64];
}


static inline
uint64_t
bits_u64_unset_high(uint64_t v, unsigned long num_bits)
{
	static uint64_t width_to_mask_table[64] = {
		0x0000000000000000, 0x0000000000000001, 0x0000000000000003, 0x0000000000000007, 0x000000000000000f, 0x000000000000001f, 0x000000000000003f, 0x000000000000007f,
		0x00000000000000ff, 0x00000000000001ff, 0x00000000000003ff, 0x00000000000007ff, 0x0000000000000fff, 0x0000000000001fff, 0x0000000000003fff, 0x0000000000007fff,
		0x000000000000ffff, 0x000000000001ffff, 0x000000000003ffff, 0x000000000007ffff, 0x00000000000fffff, 0x00000000001fffff, 0x00000000003fffff, 0x00000000007fffff,
		0x0000000000ffffff, 0x0000000001ffffff, 0x0000000003ffffff, 0x0000000007ffffff, 0x000000000fffffff, 0x000000001fffffff, 0x000000003fffffff, 0x000000007fffffff,
		0x00000000ffffffff, 0x00000001ffffffff, 0x00000003ffffffff, 0x00000007ffffffff, 0x0000000fffffffff, 0x0000001fffffffff, 0x0000003fffffffff, 0x0000007fffffffff,
		0x000000ffffffffff, 0x000001ffffffffff, 0x000003ffffffffff, 0x000007ffffffffff, 0x00000fffffffffff, 0x00001fffffffffff, 0x00003fffffffffff, 0x00007fffffffffff,
		0x0000ffffffffffff, 0x0001ffffffffffff, 0x0003ffffffffffff, 0x0007ffffffffffff, 0x000fffffffffffff, 0x001fffffffffffff, 0x003fffffffffffff, 0x007fffffffffffff,
		0x00ffffffffffffff, 0x01ffffffffffffff, 0x03ffffffffffffff, 0x07ffffffffffffff, 0x0fffffffffffffff, 0x1fffffffffffffff, 0x3fffffffffffffff, 0x7fffffffffffffff,
	};
	return v & width_to_mask_table[num_bits % 64];
}


static inline
uint64_t
bits_u64_set_low(uint64_t v, unsigned long num_bits)
{
	static uint64_t width_to_mask_table[64] = {
		0x0000000000000001, 0x0000000000000003, 0x0000000000000007, 0x000000000000000f, 0x000000000000001f, 0x000000000000003f, 0x000000000000007f, 0x00000000000000ff,
		0x00000000000001ff, 0x00000000000003ff, 0x00000000000007ff, 0x0000000000000fff, 0x0000000000001fff, 0x0000000000003fff, 0x0000000000007fff, 0x000000000000ffff,
		0x000000000001ffff, 0x000000000003ffff, 0x000000000007ffff, 0x00000000000fffff, 0x00000000001fffff, 0x00000000003fffff, 0x00000000007fffff, 0x0000000000ffffff,
		0x0000000001ffffff, 0x0000000003ffffff, 0x0000000007ffffff, 0x000000000fffffff, 0x000000001fffffff, 0x000000003fffffff, 0x000000007fffffff, 0x00000000ffffffff,
		0x00000001ffffffff, 0x00000003ffffffff, 0x00000007ffffffff, 0x0000000fffffffff, 0x0000001fffffffff, 0x0000003fffffffff, 0x0000007fffffffff, 0x000000ffffffffff,
		0x000001ffffffffff, 0x000003ffffffffff, 0x000007ffffffffff, 0x00000fffffffffff, 0x00001fffffffffff, 0x00003fffffffffff, 0x00007fffffffffff, 0x0000ffffffffffff,
		0x0001ffffffffffff, 0x0003ffffffffffff, 0x0007ffffffffffff, 0x000fffffffffffff, 0x001fffffffffffff, 0x003fffffffffffff, 0x007fffffffffffff, 0x00ffffffffffffff,
		0x01ffffffffffffff, 0x03ffffffffffffff, 0x07ffffffffffffff, 0x0fffffffffffffff, 0x1fffffffffffffff, 0x3fffffffffffffff, 0x7fffffffffffffff, 0xffffffffffffffff,
	};
	return v | width_to_mask_table[num_bits % 64];
}


static inline
uint64_t
bits_u64_unset_low(uint64_t v, unsigned long num_bits)
{
	static uint64_t width_to_mask_table[64] = {
		0xfffffffffffffffe, 0xfffffffffffffffc, 0xfffffffffffffff8, 0xfffffffffffffff0, 0xffffffffffffffe0, 0xffffffffffffffc0, 0xffffffffffffff80, 0xffffffffffffff00,
		0xfffffffffffffe00, 0xfffffffffffffc00, 0xfffffffffffff800, 0xfffffffffffff000, 0xffffffffffffe000, 0xffffffffffffc000, 0xffffffffffff8000, 0xffffffffffff0000,
		0xfffffffffffe0000, 0xfffffffffffc0000, 0xfffffffffff80000, 0xfffffffffff00000, 0xffffffffffe00000, 0xffffffffffc00000, 0xffffffffff800000, 0xffffffffff000000,
		0xfffffffffe000000, 0xfffffffffc000000, 0xfffffffff8000000, 0xfffffffff0000000, 0xffffffffe0000000, 0xffffffffc0000000, 0xffffffff80000000, 0xffffffff00000000,
		0xfffffffe00000000, 0xfffffffc00000000, 0xfffffff800000000, 0xfffffff000000000, 0xffffffe000000000, 0xffffffc000000000, 0xffffff8000000000, 0xffffff0000000000,
		0xfffffe0000000000, 0xfffffc0000000000, 0xfffff80000000000, 0xfffff00000000000, 0xffffe00000000000, 0xffffc00000000000, 0xffff800000000000, 0xffff000000000000,
		0xfffe000000000000, 0xfffc000000000000, 0xfff8000000000000, 0xfff0000000000000, 0xffe0000000000000, 0xffc0000000000000, 0xff80000000000000, 0xff00000000000000,
		0xfe00000000000000, 0xfc00000000000000, 0xf800000000000000, 0xf000000000000000, 0xe000000000000000, 0xc000000000000000, 0x8000000000000000, 0x0000000000000000,
	};
	return v & width_to_mask_table[num_bits % 64];
}


//==========================================================================================================================================
//= Extract bits
//==========================================================================================================================================


/* unsigned __int64 _bextr_u64 (unsigned __int64 a, unsigned int start, unsigned int len)
 *     Extract contiguous bits from unsigned 64-bit integer a, and store the result in dst.
 *     Extract the number of bits specified by len, starting at the bit specified by start.
 *
 * unsigned int _bextr_u32 (unsigned int a, unsigned int start, unsigned int len)
 *     Extract contiguous bits from unsigned 32-bit integer a, and store the result in dst.
 *     Extract the number of bits specified by len, starting at the bit specified by start.
 */

static inline
uint64_t
bits_u64_extract(uint64_t v, uint64_t start_pos, uint64_t num_bits)
{

	#ifdef __x86_64__
		// return __builtin_ia32_bextr_u64(v, (num_bits << 8) | start_pos);
		return _bextr_u64(v, start_pos, num_bits);
	#else
		return bits_u64_unset_high(v >> start_pos, num_bits);
	#endif
}


static inline
uint32_t
bits_u32_extract(uint32_t v, uint64_t start_pos, uint64_t num_bits)
{

	#ifdef __x86_64__
		// return __builtin_ia32_bextr_u32(v, (num_bits << 8) | start_pos);
		return _bextr_u32(v, start_pos, num_bits);
	#else
		return bits_u64_unset_high(v >> start_pos, num_bits);
	#endif
}


//==========================================================================================================================================
//= Required bits for binary representation of number
//==========================================================================================================================================


/* The required number of bits for the binary representation of the range [0, max_val].
 *
 * 'num_bits_out': Number of bits that can represent all values in range [0, max_val].
 * 
 * 'pow2_out': First power of 2 strictly bigger than 'max_val', or 0: *num_bits_out = (max_val == 0) ? 0 : 1ULL << (*num_bits_out).
 *             e.g. if max_val == 2 then pow2_out == 4.
 */
static inline
void
bits_u64_required_bits_for_binary_representation(uint64_t max_val,
		uint64_t * num_bits_out, uint64_t * pow2_out)
{
	uint64_t num_bits, pow2;
	if (max_val == 0)
	{
		num_bits = 0;
		pow2 = 0;
	}
	else
	{
		num_bits = 64 - __builtin_clzl(max_val);
		pow2 = 1ULL << num_bits;
	}
	if (num_bits_out != NULL)
		*num_bits_out = num_bits;
	if (pow2_out != NULL)
		*pow2_out = pow2;
}


//==========================================================================================================================================
//= Hamming Distance
//==========================================================================================================================================


static inline
long
bits_u64_popcnt(uint64_t v)
{
	#ifdef __x86_64__
		return __builtin_popcountll(v);
	#else
		long num = 0;
		uint64_t mask = 1;
		long i;
		for (i=0;i<64;i++)
		{
			if (mask & v)
				num++;
			mask <<= 1ULL;
		}
		return num;
	#endif
}


static inline
long
bits_hamming_distance(unsigned char * str1, unsigned char * str2, long N)
{
	long i;
	long div = N / 8;
	long rem = N % 8;
	uint64_t x, v1, v2;
	long dist = 0;
	for (i=0;i<div;i++)
	{
		v1 = ((uint64_t *) str1)[i];
		v2 = ((uint64_t *) str2)[i];
		x = v1 ^ v2;
		dist += bits_u64_popcnt(x);
	}
	x = 0;
	switch (rem) {
	case 7: x ^= (uint64_t) (str1[div*8 + 6] ^ str2[div*8 + 6]) << 48; /* fallthrough */
	case 6: x ^= (uint64_t) (str1[div*8 + 5] ^ str2[div*8 + 5]) << 40; /* fallthrough */
	case 5: x ^= (uint64_t) (str1[div*8 + 4] ^ str2[div*8 + 4]) << 32; /* fallthrough */
	case 4: x ^= (uint64_t) (str1[div*8 + 3] ^ str2[div*8 + 3]) << 24; /* fallthrough */
	case 3: x ^= (uint64_t) (str1[div*8 + 2] ^ str2[div*8 + 2]) << 16; /* fallthrough */
	case 2: x ^= (uint64_t) (str1[div*8 + 1] ^ str2[div*8 + 1]) << 8;  /* fallthrough */
	case 1: x ^= (uint64_t) (str1[div*8 + 0] ^ str2[div*8 + 0]);
	}
	dist += bits_u64_popcnt(x);
	return dist;
}


//==========================================================================================================================================
//= Mean
//==========================================================================================================================================


static inline
void
bits_mean(unsigned char * matrix, long N, long M, unsigned char * mean)
{
	unsigned int * mean_u;
	unsigned char c;
	long i, j;
	mean_u = (typeof(mean_u)) malloc(N * 8 * sizeof(*mean_u));
	for (i=0;i<N;i++)
		mean_u[i] = 0;
	for (i=0;i<M;i++)
	{
		unsigned char * str = &matrix[i*N];
		for (j=0;j<N;j++)
		{
			c = str[j];

			mean_u[j*8 + 0] += _bextr_u64(c, 0, 1);
			mean_u[j*8 + 1] += _bextr_u64(c, 1, 1);
			mean_u[j*8 + 2] += _bextr_u64(c, 2, 1);
			mean_u[j*8 + 3] += _bextr_u64(c, 3, 1);
			mean_u[j*8 + 4] += _bextr_u64(c, 4, 1);
			mean_u[j*8 + 5] += _bextr_u64(c, 5, 1);
			mean_u[j*8 + 6] += _bextr_u64(c, 6, 1);
			mean_u[j*8 + 7] += _bextr_u64(c, 7, 1);

			// const __m256i mask = _mm256_set_epi32(0x80U, 0x40U, 0x20U, 0x10U, 0x08U, 0x04U, 0x02U, 0x01U);
			// const __m256i shift = _mm256_set_epi32(7, 6, 5, 4, 3, 2, 1, 0);
			// __m256i c_v = _mm256_set1_epi32(c);
			// __m256i sum_v = _mm256_loadu_si256((__m256i *) &mean_u[j*8]);
			// c_v = _mm256_and_si256(c_v, mask);
			// c_v = _mm256_srlv_epi32(c_v, shift);
			// sum_v = _mm256_add_epi32(sum_v, c_v);
			// _mm256_storeu_si256((__m256i *) &mean_u[j*8], sum_v);

			// const __m256i mask = _mm256_set_epi32(0x80U, 0x40U, 0x20U, 0x10U, 0x08U, 0x04U, 0x02U, 0x01U);
			// const __m256i shift = _mm256_set_epi32(7, 6, 5, 4, 3, 2, 1, 0);
			// __m256i c_v = _mm256_set1_epi32(c);
			// __m256i sum_v = _mm256_loadu_si256((__m256i *) &mean_u[j*8]);
			// c_v = _mm256_and_si256(c_v, mask);
			// c_v = _mm256_cmpeq_epi32(c_v, mask);
			// sum_v = _mm256_sub_epi32(sum_v, c_v);
			// _mm256_storeu_si256((__m256i *) &mean_u[j*8], sum_v);

			// mean_u[j*8 + 0] += (c & 0x01U) >> 0;
			// mean_u[j*8 + 1] += (c & 0x02U) >> 1;
			// mean_u[j*8 + 2] += (c & 0x04U) >> 2;
			// mean_u[j*8 + 3] += (c & 0x08U) >> 3;
			// mean_u[j*8 + 4] += (c & 0x10U) >> 4;
			// mean_u[j*8 + 5] += (c & 0x20U) >> 5;
			// mean_u[j*8 + 6] += (c & 0x40U) >> 6;
			// mean_u[j*8 + 7] += (c & 0x80U) >> 7;

			// if (c & 0x01U)
				// mean_u[j*8 + 0]++;
			// if (c & 0x02U)
				// mean_u[j*8 + 1]++;
			// if (c & 0x04U)
				// mean_u[j*8 + 2]++;
			// if (c & 0x08U)
				// mean_u[j*8 + 3]++;
			// if (c & 0x10U)
				// mean_u[j*8 + 4]++;
			// if (c & 0x20U)
				// mean_u[j*8 + 5]++;
			// if (c & 0x40U)
				// mean_u[j*8 + 6]++;
			// if (c & 0x80U)
				// mean_u[j*8 + 7]++;

			// mean_u[j*8 + 0] += (c & 0x01U) != 0;
			// mean_u[j*8 + 1] += (c & 0x02U) != 0;
			// mean_u[j*8 + 2] += (c & 0x04U) != 0;
			// mean_u[j*8 + 3] += (c & 0x08U) != 0;
			// mean_u[j*8 + 4] += (c & 0x10U) != 0;
			// mean_u[j*8 + 5] += (c & 0x20U) != 0;
			// mean_u[j*8 + 6] += (c & 0x40U) != 0;
			// mean_u[j*8 + 7] += (c & 0x80U) != 0;

		}
	}
	for (j=0;j<N;j++)
	{
		c = 0;

		c |= ((2 * mean_u[j*8 + 0]) / M) << 0;
		c |= ((2 * mean_u[j*8 + 1]) / M) << 1;
		c |= ((2 * mean_u[j*8 + 2]) / M) << 2;
		c |= ((2 * mean_u[j*8 + 3]) / M) << 3;
		c |= ((2 * mean_u[j*8 + 4]) / M) << 4;
		c |= ((2 * mean_u[j*8 + 5]) / M) << 5;
		c |= ((2 * mean_u[j*8 + 6]) / M) << 6;
		c |= ((2 * mean_u[j*8 + 7]) / M) << 7;
		// c |= (2 * mean_u[j*8 + 0]) / (M << 0);
		// c |= (2 * mean_u[j*8 + 1]) / (M << 1);
		// c |= (2 * mean_u[j*8 + 2]) / (M << 2);
		// c |= (2 * mean_u[j*8 + 3]) / (M << 3);
		// c |= (2 * mean_u[j*8 + 4]) / (M << 4);
		// c |= (2 * mean_u[j*8 + 5]) / (M << 5);
		// c |= (2 * mean_u[j*8 + 6]) / (M << 6);
		// c |= (2 * mean_u[j*8 + 7]) / (M << 7);

		mean[j] = c;
	}
	// for (i=0;i<N;i++)
		// mean[i] = mean_u[i];
	if (N < 20)
	{
		for (i=N-1;i>=0;i--)
			printf("%d%d%d%d%d%d%d%d", mean_u[i*8+7], mean_u[i*8+6], mean_u[i*8+5], mean_u[i*8+4], mean_u[i*8+3], mean_u[i*8+2], mean_u[i*8+1], mean_u[i*8+0]);
		printf("\n");
	}
	free(mean_u);
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//------------------------------------------------------------------------------------------------------------------------------------------
//-                                                       Floating Point Numbers                                                           -
//------------------------------------------------------------------------------------------------------------------------------------------
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


//==========================================================================================================================================
//= Floats
//==========================================================================================================================================


//==========================================================================================================================================
//= Doubles
//==========================================================================================================================================


static inline
uint64_t
bits_double_get_sign(double val)
{
	union { uint64_t u; double d; } bits;
	bits.d = val;
	return bits_u64_extract(bits.u, 63, 1);
}

static inline
int64_t
bits_double_get_exponent(double val)
{
	union { uint64_t u; double d; } bits;
	bits.d = val;
	bits.u = bits_u64_extract(bits.u, 52, 11);
	if (bits.u)
		bits.u -= 1023;
	return (int64_t) bits.u;
}

static inline
uint64_t
bits_double_get_exponent_bits(double val)
{
	union { uint64_t u; double d; } bits;
	bits.d = val;
	return bits_u64_extract(bits.u, 52, 11);
}

static inline
uint64_t
bits_double_get_fraction(double val)
{
	union { uint64_t u; double d; } bits;
	bits.d = val;
	return bits_u64_extract(bits.u, 0, 52);
}

static inline
uint64_t
bits_double_get_upper_12_bits(double val)
{
	union { uint64_t u; double d; } bits;
	bits.d = val;
	return bits_u64_extract(bits.u, 52, 12);
}


#endif /* BIT_OPS_H */

