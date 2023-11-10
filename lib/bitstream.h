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
	long size_u;
	long len_bits;

	long pos;           // 64 bit

	uint64_t buf;
	long buf_bit_pos;

	long buf_w_pos;
	uint64_t buf_w[8] __attribute__((aligned(64)));   // A cache line.
};


void bitstream_init_write(struct Bit_Stream * bs, unsigned char * data, long size);
void bitstream_init_read(struct Bit_Stream * bs, unsigned char * data, long size);


/* Unsafe:
 *	- assumes num_bits <= 64
 */
void bitstream_write_unsafe(struct Bit_Stream * bs, uint64_t val, long num_bits);

/* It is safe to resume writing after flushing the buffer. */
void bitstream_write_flush(struct Bit_Stream * bs);

/* Safe variation of write. */
void bitstream_write(struct Bit_Stream * bs, uint64_t val, long num_bits);

/* Unsafe:
 *	- doesn't check for 'data' array boundaries
 *	- assumes num_bits <= 64
 */
void bitstream_read_unsafe(struct Bit_Stream * bs, uint64_t * val_out, long num_bits);


/* Unsafe:
 *	- doesn't check for 'data' array boundaries
 *	- assumes num_bits <= 57
 */
void bitstream_read_unsafe_57_base(struct Bit_Stream * bs, long bit_pos, uint64_t * val_out, long num_bits);

/* Unsafe:
 *	- doesn't check for 'data' array boundaries
 *	- assumes num_bits <= 57
 */
void bitstream_read_unsafe_57(struct Bit_Stream * bs, uint64_t * val_out, long num_bits);


#endif /* BITSTREAM_H */

