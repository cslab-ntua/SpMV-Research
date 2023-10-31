#ifndef BIT_OPS_H
#define BIT_OPS_H

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <x86intrin.h>

#include "macros/cpp_defines.h"
#include "debug.h"

/* Naive shifting can result problems.
 * Platforms can degrade a shift to a nop instruction.
 * Usually shift amounts are interpreted mod 32/64.
 * e.g. shift by 64 can be a nop, and the result would not be 0!
 */


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

/* Loading mask from array involves shift and add, but it's done in the Address Generation Unit (AGU) as part of the load pipeline of the CPU.
 */

static inline
uint64_t
bits_set_high_64(uint64_t v, unsigned long pos)
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
	return v | width_to_mask_table[pos % 64];
}


static inline
uint64_t
bits_unset_high_64(uint64_t v, unsigned long pos)
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
	return v & width_to_mask_table[pos % 64];
}


static inline
uint64_t
bits_set_low_64(uint64_t v, unsigned long pos)
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
	return v | width_to_mask_table[pos % 64];
}


static inline
uint64_t
bits_unset_low_64(uint64_t v, unsigned long pos)
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
	return v & width_to_mask_table[pos % 64];
}


//==========================================================================================================================================
//= Extract bits
//==========================================================================================================================================


/* unsigned __int64 _bextr_u64 (unsigned __int64 a, unsigned int start, unsigned int len)
 *     Extract contiguous bits from unsigned 64-bit integer a, and store the result in dst.
 *     Extract the number of bits specified by len, starting at the bit specified by start.
 */

static inline
uint64_t
bits_extract_64(uint64_t v, uint64_t start_pos, uint64_t num_bits)
{
	// unsigned char c[8];
	// *((uint64_t *) c) = start_pos;
	// c[1] = num_bits;
	// return __builtin_ia32_bextr_u64(val, *((uint64_t *) c));

	// return bits_unset_high_64(v >> (start_pos % 64), num_bits);

	return __builtin_ia32_bextr_u64(v, (num_bits << 8) | start_pos);
}



#endif /* BIT_OPS_H */

