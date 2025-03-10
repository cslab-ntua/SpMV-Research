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

	#include "zfp.h"
#ifdef __cplusplus
}
#endif


#define FORMAT_SUBNAME  "ZFP"


/* typedef enum {
 *    zfp_mode_null            = 0, // an invalid configuration of the 4 params
 *    zfp_mode_expert          = 1, // expert mode (4 params set manually)
 *    zfp_mode_fixed_rate      = 2, // fixed rate mode
 *    zfp_mode_fixed_precision = 3, // fixed precision mode
 *    zfp_mode_fixed_accuracy  = 4, // fixed accuracy mode
 *    zfp_mode_reversible      = 5  // reversible (lossless) mode
 * } zfp_mode;
 */


void
compress_kernel_init(__attribute__((unused)) ValueTypeReference * vals, __attribute__((unused)) const long num_vals, __attribute__((unused)) const long packet_size)
{
}


static inline
long
compress_base(double * vals, unsigned char * data, long * N_ptr, double tolerance, zfp_bool decompress)
{
	long N;
	zfp_type type;      /* array scalar type */
	zfp_field * field;  /* array meta data */
	zfp_stream * zfp;   /* compressed stream */
	size_t bufsize;     /* byte size of compressed buffer */
	bitstream * stream; /* bit stream to write to or read from */

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

	/* allocate meta data for the 1D array a[N] */
	type = zfp_type_double;
	field = zfp_field_1d(vals, type, N);

	/* allocate meta data for a compressed stream */
	zfp = zfp_stream_open(NULL);

	/* set compression mode and parameters via one of four functions */
	/*  zfp_stream_set_rate(zfp, rate, type, zfp_field_dimensionality(field), zfp_false); */
	/*  zfp_stream_set_precision(zfp, precision); */
	if (tolerance == 0)
	{
		zfp_stream_set_reversible(zfp);
	}
	else
	{
		zfp_stream_set_accuracy(zfp, tolerance);
	}

	/* set the zfp internal number of threads to one (i.e., no nested threads) */
	zfp_stream_set_omp_threads(zfp, 1);

	/* mazimum size of compressed data */
	bufsize = zfp_stream_maximum_size(zfp, field);
	// printf("bufsizemax=%ld\n", bufsize);

	/* associate bit stream with allocated buffer */
	stream = stream_open(comp_data, bufsize);
	zfp_stream_set_bit_stream(zfp, stream);
	zfp_stream_rewind(zfp);

	/* compress or decompress entire array */
	if (decompress) {
		/* read compressed stream and decompress and output array */
		if (!zfp_decompress(zfp, field)) {
			error("decompression failed");
		}
	}
	else {
		/* compress array and output compressed stream */
		*num_bytes_ptr = zfp_compress(zfp, field);
		if (!*num_bytes_ptr) {
			error("compression failed");
		}
	}

	/* important: flush any buffered compressed bits */
	stream_flush(stream);

	/* clean up */
	zfp_field_free(field);
	zfp_stream_close(zfp);
	stream_close(stream);

	if (decompress)
		*N_ptr = N;

	long total_bytes = sizeof(*num_vals_ptr) + sizeof(*num_bytes_ptr) + *num_bytes_ptr;
	return total_bytes;
}


static inline
long
compress_kernel(double * vals, unsigned char * buf, long num_vals)
{
	double tolerance;
	tolerance = 0;
	// tolerance = 1e-3;
	return compress_base(vals, buf, &num_vals, tolerance, 0);
}


static inline
long
decompress_kernel(double * vals, unsigned char * buf, long * num_vals_out)
{
	double tolerance;
	tolerance = 0;
	// tolerance = 1e-3;
	return compress_base(vals, buf, num_vals_out, tolerance, 1);
}

