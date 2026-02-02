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
	// #define VEC_RISCV_SVV

	#if DOUBLE == 0
		#define VTI   i32
		#define VTF   f32
		#define VEC_LEN  BLOCK_SIZE
	#elif DOUBLE == 1
		#define VTI   i64
		#define VTF   f64
		#define VEC_LEN  BLOCK_SIZE
	#endif

	// #include "vectorization.h"
	// #include "vectorization/vectorization_gen.h"

#ifdef __cplusplus
}
#endif


__device__
uint64_t
bfe64(uint64_t x, uint32_t start, uint32_t width) {
	uint64_t result;
	asm("bfe.u64 %0, %1, %2, %3;" : "=l"(result) : "l"(x), "r"(start), "r"(width));
	// uint64_t mask = (1ULL << width) - 1;
	// result = (x >> start) & mask;
	// printf("%ld\n", result);
	return result;
}


__device__
static __attribute__((always_inline)) inline
uint16_t
read_int16_t(unsigned char * buf)
{
	uint16_t val;
	memcpy(&val, buf, sizeof(val));
	return val;
}

__device__
static __attribute__((always_inline)) inline
uint32_t
read_int32_t(unsigned char * buf)
{
	uint32_t val;
	memcpy(&val, buf, sizeof(val));
	return val;
}

__device__
static __attribute__((always_inline)) inline
uint64_t
read_int64_t(unsigned char * buf)
{
	uint64_t val;
	memcpy(&val, buf, sizeof(val));
	return val;
}

__device__
static __attribute__((always_inline)) inline
uint64_t
read_t(unsigned char * buf, uint64_t bytes)
{
	uint64_t val;
	memcpy(&val, buf, bytes);
	return val;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//------------------------------------------------------------------------------------------------------------------------------------------
//-                                                      Gather Coordinates Dense                                                          -
//------------------------------------------------------------------------------------------------------------------------------------------
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


__device__
static __attribute__((always_inline)) inline
void
gather_coords_dense(long i, unsigned char * data_coords, const uint64_t coords_bytes, uint64_t row_bits, uint64_t col_bits, uint64_t * row_rel_out, uint64_t * col_rel_out, const long num_vals)
{
	/* The row + col bytes can be more than 4 in total, so we need uint64_t.
	 */
	const uint64_t row_bits_mask = (1ULL<<row_bits) - 1;   // here '-' has precedence over '<<'!
	uint64_t coords;
	coords = read_int64_t(&data_coords[i*coords_bytes]);
	// coords = read_t(&data_coords[i*coords_bytes], coords_bytes);
	*row_rel_out = coords & row_bits_mask;
	*col_rel_out = bfe64(coords, row_bits, col_bits);
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//------------------------------------------------------------------------------------------------------------------------------------------
//-                                               Gather Coordinates Sparse - Row Bytes 0                                                  -
//------------------------------------------------------------------------------------------------------------------------------------------
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


__device__
static __attribute__((always_inline)) inline
void
gather_coords_sparse_r0_c1(long i, unsigned char * data_coords, __attribute__((unused)) const uint64_t coords_bytes, __attribute__((unused)) uint64_t row_bits, __attribute__((unused)) uint64_t col_bits, uint64_t * row_rel_out, uint64_t * col_rel_out, const long num_vals)
{
	// error("test 0,1");
	uint64_t col_rel;
	col_rel = ((uint8_t *) data_coords)[i];
	*row_rel_out = 0;
	*col_rel_out = col_rel;
}

__device__
static __attribute__((always_inline)) inline
void
gather_coords_sparse_r0_c2(long i, unsigned char * data_coords, __attribute__((unused)) const uint64_t coords_bytes, __attribute__((unused)) uint64_t row_bits, __attribute__((unused)) uint64_t col_bits, uint64_t * row_rel_out, uint64_t * col_rel_out, const long num_vals)
{
	// error("test 0,2");
	uint64_t col_rel;
	col_rel = ((uint16_t *) data_coords)[i];
	*row_rel_out = 0;
	*col_rel_out = col_rel;
}

__device__
static __attribute__((always_inline)) inline
void
gather_coords_sparse_r0_c4(long i, unsigned char * data_coords, __attribute__((unused)) const uint64_t coords_bytes, __attribute__((unused)) uint64_t row_bits, __attribute__((unused)) uint64_t col_bits, uint64_t * row_rel_out, uint64_t * col_rel_out, const long num_vals)
{
	// error("test 0,4");
	uint64_t col_rel;
	col_rel = ((uint32_t *) data_coords)[i];
	*row_rel_out = 0;
	*col_rel_out = col_rel;
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//------------------------------------------------------------------------------------------------------------------------------------------
//-                                               Gather Coordinates Sparse - Row Bytes 1                                                  -
//------------------------------------------------------------------------------------------------------------------------------------------
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


__device__
static __attribute__((always_inline)) inline
void
gather_coords_sparse_r1_c1(long i, unsigned char * data_coords, __attribute__((unused)) const uint64_t coords_bytes, __attribute__((unused)) uint64_t row_bits, __attribute__((unused)) uint64_t col_bits, uint64_t * row_rel_out, uint64_t * col_rel_out, const long num_vals)
{
	// error("test 1\n");
	unsigned char * data_cols = data_coords;
	const uint64_t data_cols_bytes = num_vals;
	unsigned char * data_rows = &data_cols[data_cols_bytes];
	uint64_t row_rel, col_rel;
	row_rel = ((uint8_t *) data_rows)[i];
	col_rel = ((uint8_t *) data_cols)[i];
	*row_rel_out = row_rel;
	*col_rel_out = col_rel;
}

__device__
static __attribute__((always_inline)) inline
void
gather_coords_sparse_r1_c2(long i, unsigned char * data_coords, __attribute__((unused)) const uint64_t coords_bytes, __attribute__((unused)) uint64_t row_bits, __attribute__((unused)) uint64_t col_bits, uint64_t * row_rel_out, uint64_t * col_rel_out, const long num_vals)
{
	// error("test 2");
	unsigned char * data_cols = data_coords;
	const uint64_t data_cols_bytes = 2 * num_vals;
	unsigned char * data_rows = &data_cols[data_cols_bytes];
	uint64_t row_rel, col_rel;
	row_rel = ((uint8_t *) data_rows)[i];
	col_rel = ((uint16_t *) data_cols)[i];
	*row_rel_out = row_rel;
	*col_rel_out = col_rel;
}

__device__
static __attribute__((always_inline)) inline
void
gather_coords_sparse_r1_c3(long i, unsigned char * data_coords, __attribute__((unused)) const uint64_t coords_bytes, __attribute__((unused)) uint64_t row_bits, __attribute__((unused)) uint64_t col_bits, uint64_t * row_rel_out, uint64_t * col_rel_out, const long num_vals)
{
	// error("test 3");
	const uint64_t row_bits_mask = (1ULL<<8) - 1;   // here '-' has precedence over '<<'!
	uint64_t coords;
	coords = ((uint32_t *) data_coords)[i];
	*row_rel_out = coords & row_bits_mask;
	*col_rel_out = bfe64(coords, 8, col_bits);
}

__device__
static __attribute__((always_inline)) inline
void
gather_coords_sparse_r1_c4(long i, unsigned char * data_coords, __attribute__((unused)) const uint64_t coords_bytes, __attribute__((unused)) uint64_t row_bits, __attribute__((unused)) uint64_t col_bits, uint64_t * row_rel_out, uint64_t * col_rel_out, const long num_vals)
{
	// error("test 4");
	unsigned char * data_cols = data_coords;
	const uint64_t data_cols_bytes = 4 * num_vals;
	unsigned char * data_rows = &data_cols[data_cols_bytes];
	uint64_t row_rel, col_rel;
	row_rel = ((uint8_t *) data_rows)[i];
	col_rel = ((uint32_t *) data_cols)[i];
	*row_rel_out = row_rel;
	*col_rel_out = col_rel;
}


#endif /* SPMV_KERNEL_CSR_CV_STREAM_GATHER_COORDS_H */

