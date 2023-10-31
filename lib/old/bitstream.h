#ifndef BITSTREAM_H
#define BITSTREAM_H

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "macros/cpp_defines.h"
#include "macros/macrolib.h"


struct Bit_Stream {
	union {
		uint64_t * u;
		unsigned char * c;
	} data;
	long size;
	long len_bits;

	long pos;           // 64 bit

	uint64_t buf;
	long buf_bit_pos;

	long buf_w_pos;
	uint64_t buf_w[8];
};


void bitstream_init_write(struct Bit_Stream * bs, unsigned char * data);
void bitstream_init_read(struct Bit_Stream * bs, unsigned char * data);

void bitstream_write_unsafe(struct Bit_Stream * bs, uint64_t val, long num_bits);
#define bitstream_write_unsafe_cast(bs, val, num_bits)                 \
do {                                                                   \
	__auto_type _val = val;                                        \
	void * _ptr = &_val;                                           \
	_Static_assert(sizeof(val) == 8);                              \
	bitstream_write_unsafe(bs, *((uint64_t *) _ptr), num_bits);    \
} while (0)

// It is safe to resume writing after flushing the buffer.
void bitstream_write_flush(struct Bit_Stream * bs);

void bitstream_read_unsafe(struct Bit_Stream * bs, uint64_t * val_out, long num_bits);
#define bitstream_read_unsafe_cast(bs, val_out, num_bits)          \
do {                                                               \
	void * _ptr = val_out;                                     \
	_Static_assert(sizeof(*val_out) == 8);                     \
	bitstream_read_unsafe(bs, (uint64_t *) _ptr, num_bits);    \
} while (0)


void bitstream_read_unsafe_57_base(struct Bit_Stream * bs, long bit_pos, uint64_t * val_out, long num_bits);
#define bitstream_read_unsafe_57_base_cast(bs, bit_pos, val_out, num_bits)          \
do {                                                                                \
	void * _ptr = val_out;                                                      \
	_Static_assert(sizeof(*val_out) == 8);                                      \
	bitstream_read_unsafe_57_base(bs, bit_pos, (uint64_t *) _ptr, num_bits);    \
} while (0)

void bitstream_read_unsafe_57(struct Bit_Stream * bs, uint64_t * val_out, long num_bits);
#define bitstream_read_unsafe_57_cast(bs, val_out, num_bits)          \
do {                                                                  \
	void * _ptr = val_out;                                        \
	_Static_assert(sizeof(*val_out) == 8);                        \
	bitstream_read_unsafe_57(bs, (uint64_t *) _ptr, num_bits);    \
} while (0)

#endif /* BITSTREAM_H */

