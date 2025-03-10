#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
<<<<<<< Updated upstream
#ifdef __x86_64__
	#include <x86intrin.h>
#endif
=======
>>>>>>> Stashed changes

#include "bytestream.h"

#include "macros/cpp_defines.h"
#include "debug.h"
#include "bit_ops.h"


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

#ifdef __aarch64__
uint64_t arm_bextr_u64(uint64_t val, uint64_t start_len) {
    uint8_t start = start_len & 0xFF;      // Extract start position from lower 8 bits
    uint8_t len = (start_len >> 8) & 0xFF; // Extract length from next 8 bits

    // Create a mask with `len` 1s at the `start` position
    uint64_t mask = ((1ULL << len) - 1) << start;

    // Extract the bits and shift them to the least significant position
    return (val & mask) >> start;
}
#endif

inline
void
bytestream_read_unsafe(struct Byte_Stream * bs, uint64_t * val_out, long num_bytes)
{
	uint64_t val;
	val = *((uint64_t *) (&bs->data.c[bs->len]));
<<<<<<< Updated upstream
	#ifdef __x86_64__
		val = __builtin_ia32_bextr_u64(val, num_bytes << 11);   // 3 to bits + 8 to place in second byte
	#else
		val = arm_bextr_u64(val, num_bytes << 11);
	#endif
=======
	val = bits_u64_extract(val, 0, num_bytes << 3);
>>>>>>> Stashed changes
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

