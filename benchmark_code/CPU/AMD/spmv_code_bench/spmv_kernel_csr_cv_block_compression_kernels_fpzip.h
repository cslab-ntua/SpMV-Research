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

	#include "fpzip.h"
#ifdef __cplusplus
}
#endif


#define FORMAT_SUBNAME  "FPZIP"


void
compress_kernel_init(__attribute__((unused)) ValueType * vals, __attribute__((unused)) const long num_vals, __attribute__((unused)) const long packet_size)
{
}


static inline
long
compress_base(double * vals, unsigned char * data, long * N_ptr, int decompress)
{
	long N;

	long * num_vals_ptr = (long *) data;
	data += sizeof(*num_vals_ptr);
	long * num_bytes_ptr = (long *) data;
	data += sizeof(*num_bytes_ptr);
	unsigned char * comp_data = data;

	if (decompress)
		N = *num_vals_ptr;
	else
	{
		N = *N_ptr;
		*num_vals_ptr = *N_ptr;
	}

	if (decompress) {
		FPZ * fpz = fpzip_read_from_buffer(comp_data);
		fpz->type = FPZIP_TYPE_DOUBLE;
		fpz->prec = 0;
		fpz->nx = N;
		fpz->ny = 1;
		fpz->nz = 1;
		fpz->nf = 1;
		if (!fpzip_read(fpz, vals))
			error("decompression failed");
		fpzip_read_close(fpz);
	}
	else {
		size_t bound = N* sizeof(double) * 2;
		FPZ * fpz = fpzip_write_to_buffer(comp_data, bound);
		fpz->type = FPZIP_TYPE_DOUBLE;
		fpz->prec = 0;
		fpz->nx = N;
		fpz->ny = 1;
		fpz->nz = 1;
		fpz->nf = 1;
		*num_bytes_ptr = fpzip_write(fpz, vals);
		if (!*num_bytes_ptr) {
			error("compression failed");
		}
		fpzip_write_close(fpz);
	}

	if (decompress)
		*N_ptr = N;

	long total_bytes = sizeof(*num_vals_ptr) + sizeof(*num_bytes_ptr) + *num_bytes_ptr;
	return total_bytes;
}


static inline
long
compress_kernel(double * vals, unsigned char * buf, long num_vals)
{
	/* compress_base or decompress array */
	return compress_base(vals, buf, &num_vals, 0);
}


static inline
long
decompress_kernel(double * vals, unsigned char * buf, long * num_vals_out)
{
	/* compress_base or decompress array */
	return compress_base(vals, buf, num_vals_out, 1);
}

