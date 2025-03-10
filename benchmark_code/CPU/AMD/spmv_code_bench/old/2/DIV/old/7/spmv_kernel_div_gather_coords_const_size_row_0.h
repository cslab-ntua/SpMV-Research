#ifndef SPMV_KERNEL_CSR_CV_STREAM_GATHER_COORDS_H
#define SPMV_KERNEL_CSR_CV_STREAM_GATHER_COORDS_H

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <omp.h>
// #include <x86intrin.h>

#include "macros/cpp_defines.h"

#ifdef __cplusplus
extern "C"{
#endif
	#include "bit_ops.h"
	#include "bitstream.h"
	#include "bytestream.h"

	// #define VEC_FORCE

	// #define VEC_X86_512
	// #define VEC_X86_256
	// #define VEC_X86_128
	// #define VEC_ARM_SVE

	#if DOUBLE == 0
		#define VTI   i32
		#define VTF   f32
		// #define VEC_LEN  1
		// #define VEC_LEN  vec_len_default_f32
		#define VEC_LEN  vec_len_default_f64
		// #define VEC_LEN  4
		// #define VEC_LEN  8
		// #define VEC_LEN  16
		// #define VEC_LEN  32
	#elif DOUBLE == 1
		#define VTI   i64
		#define VTF   f64
		#define VEC_LEN  vec_len_default_f64
		// #define VEC_LEN  1
	#endif

	// #include "vectorization.h"
	#include "vectorization/vectorization_gen.h"

#ifdef __cplusplus
}
#endif


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//------------------------------------------------------------------------------------------------------------------------------------------
//-                                               Gather Coordinates Sparse - Row Bytes 0                                                  -
//------------------------------------------------------------------------------------------------------------------------------------------
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


static __attribute__((always_inline)) inline
void
gather_coords_sparse_r0_c1(long i, unsigned char * data_coords, __attribute__((unused)) const uint64_t coords_bytes, __attribute__((unused)) uint64_t row_bits, __attribute__((unused)) uint64_t col_bits, uint64_t * row_rel_out, uint64_t * col_rel_out)
{
	uint64_t col_rel;
	col_rel = *((uint8_t *) &data_coords[i*1]);
	*row_rel_out = 0;
	*col_rel_out = col_rel;
}

static __attribute__((always_inline)) inline
void
gather_coords_sparse_r0_c1_v(long i, unsigned char * data_coords, __attribute__((unused)) const uint64_t coords_bytes, __attribute__((unused)) uint64_t row_bits, __attribute__((unused)) uint64_t col_bits, vec_t(i64, VEC_LEN) * row_rel_out, vec_t(i64, VEC_LEN) * col_rel_out)
{
	vec_t(i64, VEC_LEN) col_rel;
	col_rel = vec_set_iter(i64, VEC_LEN, iter, *((uint8_t *) &data_coords[(i+iter)*1]));
	*row_rel_out = vec_set1(i64, VEC_LEN, 0);
	*col_rel_out = col_rel;
}


static __attribute__((always_inline)) inline
void
gather_coords_sparse_r0_c2(long i, unsigned char * data_coords, __attribute__((unused)) const uint64_t coords_bytes, __attribute__((unused)) uint64_t row_bits, __attribute__((unused)) uint64_t col_bits, uint64_t * row_rel_out, uint64_t * col_rel_out)
{
	uint64_t col_rel;
	col_rel = *((uint16_t *) &data_coords[i*2]);
	*row_rel_out = 0;
	*col_rel_out = col_rel;
}

static __attribute__((always_inline)) inline
void
gather_coords_sparse_r0_c2_v(long i, unsigned char * data_coords, __attribute__((unused)) const uint64_t coords_bytes, __attribute__((unused)) uint64_t row_bits, __attribute__((unused)) uint64_t col_bits, vec_t(i64, VEC_LEN) * row_rel_out, vec_t(i64, VEC_LEN) * col_rel_out)
{
	vec_t(i64, VEC_LEN) col_rel;
	col_rel = vec_set_iter(i64, VEC_LEN, iter, *((uint16_t *) &data_coords[(i+iter)*2]));
	*row_rel_out = vec_set1(i64, VEC_LEN, 0);
	*col_rel_out = col_rel;
}


static __attribute__((always_inline)) inline
void
gather_coords_sparse_r0_c3(long i, unsigned char * data_coords, __attribute__((unused)) const uint64_t coords_bytes, __attribute__((unused)) uint64_t row_bits, __attribute__((unused)) uint64_t col_bits, uint64_t * row_rel_out, uint64_t * col_rel_out)
{
	const uint64_t col_bits_mask = (1ULL<<col_bits) - 1;
	uint64_t col_rel;
	col_rel = *((uint32_t *) &data_coords[i*3]);
	col_rel = col_rel & col_bits_mask;
	*row_rel_out = 0;
	*col_rel_out = col_rel;
}

static __attribute__((always_inline)) inline
void
gather_coords_sparse_r0_c3_v(long i, unsigned char * data_coords, __attribute__((unused)) const uint64_t coords_bytes, __attribute__((unused)) uint64_t row_bits, __attribute__((unused)) uint64_t col_bits, vec_t(i64, VEC_LEN) * row_rel_out, vec_t(i64, VEC_LEN) * col_rel_out)
{
	const vec_t(i64, VEC_LEN) col_bits_mask = vec_set1(i64, VEC_LEN, (1ULL<<24) - 1);
	vec_t(i64, VEC_LEN) col_rel;
	col_rel = vec_set_iter(i64, VEC_LEN, iter, *((uint32_t *) &data_coords[(i+iter)*3]));
	col_rel = vec_and(i64, VEC_LEN, col_rel, col_bits_mask);
	*row_rel_out = vec_set1(i64, VEC_LEN, 0);
	*col_rel_out = col_rel;
}


static __attribute__((always_inline)) inline
void
gather_coords_sparse_r0_c4(long i, unsigned char * data_coords, __attribute__((unused)) const uint64_t coords_bytes, __attribute__((unused)) uint64_t row_bits, __attribute__((unused)) uint64_t col_bits, uint64_t * row_rel_out, uint64_t * col_rel_out)
{
	uint64_t col_rel;
	col_rel = *((uint32_t *) &data_coords[i*4]);
	*row_rel_out = 0;
	*col_rel_out = col_rel;
}

static __attribute__((always_inline)) inline
void
gather_coords_sparse_r0_c4_v(long i, unsigned char * data_coords, __attribute__((unused)) const uint64_t coords_bytes, __attribute__((unused)) uint64_t row_bits, __attribute__((unused)) uint64_t col_bits, vec_t(i64, VEC_LEN) * row_rel_out, vec_t(i64, VEC_LEN) * col_rel_out)
{
	vec_t(i64, VEC_LEN) col_rel;
	col_rel = vec_set_iter(i64, VEC_LEN, iter, *((uint32_t *) &data_coords[(i+iter)*4]));
	*row_rel_out = vec_set1(i64, VEC_LEN, 0);
	*col_rel_out = col_rel;
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//------------------------------------------------------------------------------------------------------------------------------------------
//-                                               Gather Coordinates Sparse - Row Bytes 1                                                  -
//------------------------------------------------------------------------------------------------------------------------------------------
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


static __attribute__((always_inline)) inline
void
gather_coords_sparse_r1_c1(long i, unsigned char * data_coords, __attribute__((unused)) const uint64_t coords_bytes, __attribute__((unused)) uint64_t row_bits, __attribute__((unused)) uint64_t col_bits, uint64_t * row_rel_out, uint64_t * col_rel_out)
{
	uint64_t row_rel, col_rel;
	row_rel = *((uint8_t *) &data_coords[i*2]);
	col_rel = *((uint8_t *) &data_coords[i*2 + 1]);
	*row_rel_out = row_rel;
	*col_rel_out = col_rel;
}

static __attribute__((always_inline)) inline
void
gather_coords_sparse_r1_c1_v(long i, unsigned char * data_coords, __attribute__((unused)) const uint64_t coords_bytes, __attribute__((unused)) uint64_t row_bits, __attribute__((unused)) uint64_t col_bits, vec_t(i64, VEC_LEN) * row_rel_out, vec_t(i64, VEC_LEN) * col_rel_out)
{
	vec_t(i64, VEC_LEN) row_rel, col_rel;
	row_rel = vec_set_iter(i64, VEC_LEN, iter, *((uint8_t *) &data_coords[(i+iter)*2]));
	col_rel = vec_set_iter(i64, VEC_LEN, iter, *((uint8_t *) &data_coords[(i+iter)*2 + 1]));
	*row_rel_out = row_rel;
	*col_rel_out = col_rel;
}


static __attribute__((always_inline)) inline
void
gather_coords_sparse_r1_c2(long i, unsigned char * data_coords, __attribute__((unused)) const uint64_t coords_bytes, __attribute__((unused)) uint64_t row_bits, __attribute__((unused)) uint64_t col_bits, uint64_t * row_rel_out, uint64_t * col_rel_out)
{
	uint64_t row_rel, col_rel;
	row_rel = *((uint8_t *) &data_coords[i*3]);
	col_rel = *((uint16_t *) &data_coords[i*3 + 1]);
	*row_rel_out = row_rel;
	*col_rel_out = col_rel;
}

static __attribute__((always_inline)) inline
void
gather_coords_sparse_r1_c2_v(long i, unsigned char * data_coords, __attribute__((unused)) const uint64_t coords_bytes, __attribute__((unused)) uint64_t row_bits, __attribute__((unused)) uint64_t col_bits, vec_t(i64, VEC_LEN) * row_rel_out, vec_t(i64, VEC_LEN) * col_rel_out)
{
	vec_t(i64, VEC_LEN) row_rel, col_rel;
	row_rel = vec_set_iter(i64, VEC_LEN, iter, *((uint8_t *) &data_coords[(i+iter)*3]));
	col_rel = vec_set_iter(i64, VEC_LEN, iter, *((uint16_t *) &data_coords[(i+iter)*3 + 1]));
	*row_rel_out = row_rel;
	*col_rel_out = col_rel;
}


static __attribute__((always_inline)) inline
void
gather_coords_sparse_r1_c3(long i, unsigned char * data_coords, __attribute__((unused)) const uint64_t coords_bytes, __attribute__((unused)) uint64_t row_bits, __attribute__((unused)) uint64_t col_bits, uint64_t * row_rel_out, uint64_t * col_rel_out)
{
	const uint64_t col_bits_mask = (1ULL<<col_bits) - 1;
	uint64_t row_rel, col_rel;
	row_rel = *((uint8_t *) &data_coords[i*4]);
	col_rel = *((uint32_t *) &data_coords[i*4 + 1]);
	col_rel = col_rel & col_bits_mask;
	*row_rel_out = row_rel;
	*col_rel_out = col_rel;
}

static __attribute__((always_inline)) inline
void
gather_coords_sparse_r1_c3_v(long i, unsigned char * data_coords, __attribute__((unused)) const uint64_t coords_bytes, __attribute__((unused)) uint64_t row_bits, __attribute__((unused)) uint64_t col_bits, vec_t(i64, VEC_LEN) * row_rel_out, vec_t(i64, VEC_LEN) * col_rel_out)
{
	const vec_t(i64, VEC_LEN) col_bits_mask = vec_set1(i64, VEC_LEN, (1ULL<<24) - 1);
	vec_t(i64, VEC_LEN) row_rel, col_rel;
	row_rel = vec_set_iter(i64, VEC_LEN, iter, *((uint8_t *) &data_coords[(i+iter)*4]));
	col_rel = vec_set_iter(i64, VEC_LEN, iter, *((uint32_t *) &data_coords[(i+iter)*4 + 1]));
	col_rel = vec_and(i64, VEC_LEN, col_rel, col_bits_mask);
	*row_rel_out = row_rel;
	*col_rel_out = col_rel;
}


static __attribute__((always_inline)) inline
void
gather_coords_sparse_r1_c4(long i, unsigned char * data_coords, __attribute__((unused)) const uint64_t coords_bytes, __attribute__((unused)) uint64_t row_bits, __attribute__((unused)) uint64_t col_bits, uint64_t * row_rel_out, uint64_t * col_rel_out)
{
	uint64_t row_rel, col_rel;
	row_rel = *((uint8_t *) &data_coords[i*5]);
	col_rel = *((uint32_t *) &data_coords[i*5 + 1]);
	*row_rel_out = row_rel;
	*col_rel_out = col_rel;
}

static __attribute__((always_inline)) inline
void
gather_coords_sparse_r1_c4_v(long i, unsigned char * data_coords, __attribute__((unused)) const uint64_t coords_bytes, __attribute__((unused)) uint64_t row_bits, __attribute__((unused)) uint64_t col_bits, vec_t(i64, VEC_LEN) * row_rel_out, vec_t(i64, VEC_LEN) * col_rel_out)
{
	vec_t(i64, VEC_LEN) row_rel, col_rel;
	row_rel = vec_set_iter(i64, VEC_LEN, iter, *((uint8_t *) &data_coords[(i+iter)*5]));
	col_rel = vec_set_iter(i64, VEC_LEN, iter, *((uint32_t *) &data_coords[(i+iter)*5 + 1]));
	*row_rel_out = row_rel;
	*col_rel_out = col_rel;
}


#endif /* SPMV_KERNEL_CSR_CV_STREAM_GATHER_COORDS_H */

