#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <omp.h>

#include "macros/cpp_defines.h"


//==========================================================================================================================================
//= Id
//==========================================================================================================================================


static inline
long
compress_kernel_id(ValueType * vals, unsigned char * buf, const long num_vals)
{
	long i;
	*((int *) buf) = num_vals;
	buf += sizeof(int);
	for (i=0;i<num_vals;i++)
		((ValueType *) buf)[i] = vals[i];
	return sizeof(int) + num_vals * sizeof(ValueType);
}


static inline
long
decompress_kernel_id(ValueType * vals, unsigned char * buf, long * num_vals_out)
{
	long i;
	int num_vals = *((int *) buf);
	buf += sizeof(int);
	for (i=0;i<num_vals;i++)
		vals[i] = ((ValueType *) buf)[i];
	*num_vals_out = num_vals;
	return sizeof(int) + num_vals * sizeof(ValueType);
}


//==========================================================================================================================================
//= Float Casting
//==========================================================================================================================================


static inline
long
compress_kernel_float(ValueType * vals, unsigned char * buf, const long num_vals)
{
	long i;
	*((int *) buf) = num_vals;
	buf += sizeof(int);
	for (i=0;i<num_vals;i++)
		((float *) buf)[i] = (float) vals[i];
	return sizeof(int) + num_vals * sizeof(float);
}


static inline
long
decompress_kernel_float(ValueType * vals, unsigned char * buf, long * num_vals_out)
{
	long i;
	int num_vals = *((int *) buf);
	buf += sizeof(int);
	for (i=0;i<num_vals;i++)
		vals[i] = (ValueType) ((float *) buf)[i];
	*num_vals_out = num_vals;
	return sizeof(int) + num_vals * sizeof(float);
}


//==========================================================================================================================================
//= FPC
//==========================================================================================================================================


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
compress_kernel_fpc(double * vals, unsigned char * buf, const long num_vals)
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
decompress_kernel_fpc(double * vals, unsigned char * buf, long * num_vals_out)
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

