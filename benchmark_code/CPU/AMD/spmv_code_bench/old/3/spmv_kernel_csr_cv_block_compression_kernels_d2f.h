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


#define FORMAT_SUBNAME  "D2F"


void
compress_kernel_init(__attribute__((unused)) ValueType * vals, __attribute__((unused)) const long num_vals, __attribute__((unused)) const long packet_size)
{
}


static inline
long
compress_kernel(ValueType * vals, unsigned char * buf, const long num_vals)
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
decompress_kernel(ValueType * vals, unsigned char * buf, long * num_vals_out)
{
	long i;
	long num_vals = *((int *) buf);
	buf += sizeof(int);
	for (i=0;i<num_vals;i++)
		vals[i] = (ValueType) ((float *) buf)[i];
	*num_vals_out = num_vals;
	return sizeof(int) + num_vals * sizeof(float);
}

