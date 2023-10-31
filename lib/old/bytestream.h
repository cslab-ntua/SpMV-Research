#ifndef BYTESTREAM_H
#define BYTESTREAM_H

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "macros/cpp_defines.h"
#include "macros/macrolib.h"


struct Byte_Stream {
	union {
		uint64_t * u;
		unsigned char * c;
	} data;
	long len;
};


void bytestream_init_write(struct Byte_Stream * bs, unsigned char * data);
void bytestream_init_read(struct Byte_Stream * bs, unsigned char * data);

void bytestream_write_unsafe(struct Byte_Stream * bs, uint64_t val, long num_bytes);
#define bytestream_write_unsafe_cast(bs, val, num_bytes)                 \
do {                                                                     \
	__auto_type _val = val;                                          \
	void * _ptr = &_val;                                             \
	_Static_assert(sizeof(val) == 8);                                \
	bytestream_write_unsafe(bs, *((uint64_t *) _ptr), num_bytes);    \
} while (0)

void bytestream_read_unsafe(struct Byte_Stream * bs, uint64_t * val_out, long num_bytes);
#define bytestream_read_unsafe_cast(bs, val_out, num_bytes)          \
do {                                                                 \
	void * _ptr = val_out;                                       \
	_Static_assert(sizeof(*val_out) == 8);                       \
	bytestream_read_unsafe(bs, (uint64_t *) _ptr, num_bytes);    \
} while (0)


#endif /* BYTESTREAM_H */

