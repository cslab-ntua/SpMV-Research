#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <omp.h>

#include "bitstream.h"

#include "macros/cpp_defines.h"
#include "debug.h"
#include "bit_ops.h"


void
bitstream_init_write(struct Bit_Stream * bs, unsigned char * data, long size)
{
	bs->data.c = data;
	bs->size = size;
	bs->size_u = size / 8;
	if (size % 8)
		error("size of 'data' array should be divisible by 8");
	bs->len_bits = 0;
	bs->pos = 0;

	bs->buf = 0;
	bs->buf_bit_pos = 0;

	bs->buf_w_pos = 0;
	bs->buf_w[0] = 0;
	bs->buf_w[1] = 0;
	bs->buf_w[2] = 0;
	bs->buf_w[3] = 0;
	bs->buf_w[4] = 0;
	bs->buf_w[5] = 0;
	bs->buf_w[6] = 0;
	bs->buf_w[7] = 0;
}


void
bitstream_init_read(struct Bit_Stream * bs, unsigned char * data, long size)
{
	bs->data.c = data;
	bs->size = size;
	bs->size_u = size / 8;
	if (size % 8)
		error("size of 'data' array should be divisible by 8");
	bs->len_bits = 0;
	bs->pos = 0;

	bs->buf = bs->data.u[0];
	bs->buf_bit_pos = 64;
}


inline
void
bitstream_write_unsafe(struct Bit_Stream * bs, uint64_t val, long num_bits)
{
	// int tnum = omp_get_thread_num();
	uint64_t buf = bs->buf;
	long rem;
	long i;
	// if (num_bits == 0)
		// return;
	buf |= val << bs->buf_bit_pos;
	bs->buf_bit_pos += num_bits;
	if (__builtin_expect(bs->buf_bit_pos >= 64, 0))
	{
		bs->buf_w[bs->buf_w_pos] = buf;
		bs->buf_w_pos++;
		if (__builtin_expect(bs->buf_w_pos >= 8, 0))
		{
			if (__builtin_expect(bs->pos+8 > bs->size_u, 0))
				error("no space in 'data' array");
			for (i=0;i<8;i++)
			{
				bs->data.u[bs->pos++] = bs->buf_w[i];
				bs->buf_w[i] = 0;
			}
			bs->buf_w_pos = 0;
		}
		buf = 0;
		rem = bs->buf_bit_pos & 63;
		// if (rem)
		if (__builtin_expect(rem, 0))
			buf = val >> (num_bits - rem);
		bs->buf_bit_pos = rem;
	}
	bs->buf = buf;
	bs->len_bits += num_bits;
}


// It is safe to resume writing after flushing the buffer.
void
bitstream_write_flush(struct Bit_Stream * bs)
{
	long i;
	bs->buf_w[bs->buf_w_pos] = bs->buf;
	if (__builtin_expect(bs->pos + bs->buf_w_pos > bs->size_u, 0))
		error("no space in 'data' array");
	for (i=0;i<bs->buf_w_pos;i++)
	{
		bs->data.u[bs->pos + i] = bs->buf_w[i];
	}
	if (bs->buf_bit_pos)
		bs->data.u[bs->pos + bs->buf_w_pos] = bs->buf_w[i];
}


inline
void
bitstream_write(struct Bit_Stream * bs, uint64_t val, long num_bits)
{
	if (num_bits > 64)
		error("can't write more than 64 bits at once");
	bitstream_write_unsafe(bs, val, num_bits);
}


inline
void
bitstream_read_unsafe(struct Bit_Stream * bs, uint64_t * val_out, long num_bits)
{
	long byte_pos = bs->len_bits >> 3;
	uint64_t val;
	uint64_t res;
	long rem = num_bits - 57;
	if (__builtin_expect(rem > 0, 0))
		num_bits = 57;
	val = *((uint64_t *) (&bs->data.c[byte_pos]));
	res = bits_u64_extract(val, bs->len_bits & 7, num_bits);
	// res = (val >> (bs->len_bits & 7)) & ((1 << num_bits) - 1);
	bs->len_bits += num_bits;
	if (__builtin_expect(rem > 0, 0))
	{
		byte_pos = bs->len_bits >> 3;
		val = *((uint64_t *) (&bs->data.c[byte_pos]));
		res |= bits_u64_extract(val, bs->len_bits & 7, rem) << 57;
		bs->len_bits += rem;
	}
	*val_out = res;
}


// num_bits <= 57
inline
void
bitstream_read_unsafe_57_base(struct Bit_Stream * bs, long bit_pos, uint64_t * val_out, long num_bits)
{
	long byte_pos = bit_pos >> 3;
	uint64_t val;
	val = *((uint64_t *) (&bs->data.c[byte_pos]));
	*val_out = bits_u64_extract(val, bit_pos & 7, num_bits);
}

// num_bits <= 57
inline
void
bitstream_read_unsafe_57(struct Bit_Stream * bs, uint64_t * val_out, long num_bits)
{
	bitstream_read_unsafe_57_base(bs, bs->len_bits, val_out, num_bits);
	bs->len_bits += num_bits;
}


/* inline
inline
void
bitstream_read_unsafe(struct Bit_Stream * bs, uint64_t * val_out, long num_bits)
{
	uint64_t buf = bs->buf;
	uint64_t val = 0;
	long rem, buf_bit_pos_prev;
	// if (num_bits == 0)
	// {
		// *val_out = 0;
		// return;
	// }
	val = buf;
	buf >>= num_bits;
	buf_bit_pos_prev = bs->buf_bit_pos;
	bs->buf_bit_pos -= num_bits;
	// if (bs->buf_bit_pos <= 0)
	if (__builtin_expect(bs->buf_bit_pos <= 0, 0))
	{
		// printf("test %ld\n", bs->buf_bit_pos);
		// if (__builtin_expect(bs->pos >= bs->size_u, 0))
			// error("OOM");
		rem = -bs->buf_bit_pos;
		bs->pos++;
		buf = bs->data.u[bs->pos];
		// if (rem)
		if (__builtin_expect(rem & 63, 0))
		{
			val |= buf << buf_bit_pos_prev;
			buf >>= rem;
		}
		bs->buf_bit_pos = 64 - rem;
	}
	bs->buf = buf;
	val = _bzhi_u64(val, num_bits);
	// val = bits_u64_unset_high(val, num_bits);
	*val_out = val;
}

void
bitstream_read_unsafe(struct Bit_Stream * bs, uint64_t * val_out, long num_bits)
{
	long byte_pos = bs->len_bits >> 3;
	uint64_t val1 = 0, val2 = 0;
	uint64_t res;
	val1 = *((uint64_t *) (&bs->data.c[byte_pos]));
	val1 >>= bs->len_bits & 7;
	val1 &= (1ULL << 56) -1;
	val2 =  *((uint64_t *) (&bs->data.c[byte_pos+1])); 
	val2 &= ~((1ULL << 56) -1);
	val2 <<= 8 - (bs->len_bits & 7);
	res = val1 | val2;
	res = _bzhi_u64(res, num_bits);
	bs->len_bits += num_bits;
	*val_out = res;
} */

/* inline
void
bitstream_read_unsafe(struct Bit_Stream * bs, uint64_t * val_out, long num_bits)
{
	long byte_pos = bs->len_bits >> 3;
	uint64_t val1 = 0, val2 = 0;
	uint64_t res;
	val1 = *((uint64_t *) (&bs->data.c[byte_pos]));
	val1 = __builtin_ia32_bextr_u64(val1, (56ULL << 8) | (bs->len_bits & 7));
	val2 = *((uint64_t *) (&bs->data.c[byte_pos+1]));
	val2 = __builtin_ia32_bextr_u64(val2, (56ULL << 8) | (bs->len_bits & 7));
	val2 <<= 8;
	res = val1 | val2;
	res = _bzhi_u64(res, num_bits);
	bs->len_bits += num_bits;
	*val_out = res;
} */

/* inline
void
bitstream_read_unsafe(struct Bit_Stream * bs, uint64_t * val_out, long num_bits)
{
	long byte_pos = bs->len_bits >> 3;
	uint64_t val1 = 0, val2 = 0;
	uint64_t res;
	val1 = *((uint64_t *) (&bs->data.c[byte_pos]));
	val1 >>= bs->len_bits & 7;
	val1 &= (1ULL << 56) -1;
	val2 = *((uint64_t *) (&bs->data.c[byte_pos+1]));
	val2 >>= bs->len_bits & 7;
	val2 <<= 8;
	res = val1 | val2;
	res = _bzhi_u64(res, num_bits);
	bs->len_bits += num_bits;
	*val_out = res;
} */

