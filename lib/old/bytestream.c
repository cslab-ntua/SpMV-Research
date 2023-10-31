#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <x86intrin.h>

#include "bytestream.h"

#include "macros/cpp_defines.h"
#include "debug.h"


void
bytestream_init_write(struct Byte_Stream * bs, unsigned char * data)
{
	bs->data.c = data;
	bs->len = 0;
}


void
bytestream_init_read(struct Byte_Stream * bs, unsigned char * data)
{
	bs->data.c = data;
	bs->len = 0;
}


inline
void
bytestream_write_unsafe(struct Byte_Stream * bs, uint64_t val, long num_bytes)
{
	*((uint64_t *) &bs->data.c[bs->len]) = val;
	bs->len += num_bytes;
}


inline
void
bytestream_read_unsafe(struct Byte_Stream * bs, uint64_t * val_out, long num_bytes)
{
	uint64_t val;
	val = *((uint64_t *) (&bs->data.c[bs->len]));
	val = __builtin_ia32_bextr_u64(val, num_bytes << 11);   // 3 to bits + 8 to place in second byte
	bs->len += num_bytes;
	*val_out = val;

	// uint64_t val;
	// val = *((uint64_t *) (&bs->data.c[bs->len]));
	// switch (num_bytes) {
		// case 0:
			// val = 0;
			// break;
		// case 1:
			// val &= (1ULL<<8) - 1;
			// break;
		// case 2:
			// val &= (1ULL<<16) - 1;
			// break;
		// case 3:
			// val &= (1ULL<<24) - 1;
			// break;
		// case 4:
			// val &= (1ULL<<32) - 1;
			// break;
		// case 5:
			// val &= (1ULL<<40) - 1;
			// break;
		// case 6:
			// val &= (1ULL<<48) - 1;
			// break;
		// case 7:
			// val &= (1ULL<<56) - 1;
			// break;
	// }
	// bs->len += num_bytes;
	// *val_out = val;

	// union {
		// uint64_t u;
		// unsigned char c[8];
	// } val;
	// val.u = *((uint64_t *) (&bs->data.c[bs->len]));
	// long i;
	// for (i=0;i<8-num_bytes;i++)
		// val.c[7-i] = 0;
	// bs->len += num_bytes;
	// *val_out = val.u;

	// union {
		// uint64_t u;
		// unsigned char c[8];
	// } val;
	// val.u = 0;
	// long i;
	// for (i=0;i<num_bytes;i++)
		// val.c[i] = bs->data.c[bs->len+i];
	// bs->len += num_bytes;
	// *val_out = val.u;

}

