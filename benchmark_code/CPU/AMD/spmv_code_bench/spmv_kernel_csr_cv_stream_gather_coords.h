#ifndef SPMV_KERNEL_CSR_CV_STREAM_GATHER_COORDS_H
#define SPMV_KERNEL_CSR_CV_STREAM_GATHER_COORDS_H

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
	#include "bit_ops.h"
	#include "bitstream.h"
	#include "bytestream.h"

	#include "vectorization.h"
#ifdef __cplusplus
}
#endif


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//------------------------------------------------------------------------------------------------------------------------------------------
//-                                                      Gather Coordinates Dense                                                          -
//------------------------------------------------------------------------------------------------------------------------------------------
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


static __attribute__((always_inline)) inline
void
gather_coords_dense(long i, unsigned char * data_coords, const uint64_t coords_bytes, uint64_t row_bits, uint64_t col_bits, uint64_t * row_rel_out, uint64_t * col_rel_out)
{
	/* The row + col bytes can be more than 4 in total, so we need uint64_t.
	 */
	const uint64_t row_bits_mask = (1ULL<<row_bits) - 1;   // here '-' has precedence over '<<'!
	uint64_t coords;
	coords = *((uint64_t *) &data_coords[i*coords_bytes]);
	*row_rel_out = coords & row_bits_mask;
	*col_rel_out = _bextr_u64(coords, row_bits, col_bits);
}

static __attribute__((always_inline)) inline
void
gather_coords_dense_v(long i, unsigned char * data_coords, const uint64_t coords_bytes, uint64_t row_bits, uint64_t col_bits, vec_i_t * row_rel_out, vec_i_t * col_rel_out)
{
	// error("test d");
	/* The row + col bytes can be more than 4 in total, so we need uint64_t.
	 */
	const vec_i_t row_bits_mask = vec_set1_epi64((1ULL<<row_bits) - 1);   // here '-' has precedence over '<<'!
	vec_i_t coords;
	coords = vec_set_iter_epi64(iter, *((uint64_t *) &data_coords[(i+iter)*coords_bytes]));
	// *row_rel_out = coords & row_bits_mask;
	*row_rel_out = vec_and_si(coords, row_bits_mask);
	*col_rel_out = vec_set_iter_epi64(iter, _bextr_u64(coords[iter], row_bits, col_bits));
}


static __attribute__((always_inline)) inline
void
gather_coords_dense_1(long i, unsigned char * data_coords, __attribute__((unused)) const uint64_t coords_bytes, uint64_t row_bits, uint64_t col_bits, uint64_t * row_rel_out, uint64_t * col_rel_out)
{
	const uint64_t row_bits_mask = (1ULL<<row_bits) - 1;   // here '-' has precedence over '<<'!
	uint64_t coords;
	coords = *((uint8_t *) &data_coords[i*1]);
	*row_rel_out = coords & row_bits_mask;
	*col_rel_out = _bextr_u64(coords, row_bits, col_bits);
}

static __attribute__((always_inline)) inline
void
gather_coords_dense_1_v(long i, unsigned char * data_coords, __attribute__((unused)) const uint64_t coords_bytes, uint64_t row_bits, uint64_t col_bits, vec_i_t * row_rel_out, vec_i_t * col_rel_out)
{
	// error("test d1");
	const vec_i_t row_bits_mask = vec_set1_epi64((1ULL<<row_bits) - 1);   // here '-' has precedence over '<<'!
	vec_i_t coords;
	coords = vec_set_iter_epi64(iter, *((uint8_t *) &data_coords[(i+iter)*1]));
	// *row_rel_out = coords & row_bits_mask;
	*row_rel_out = vec_and_si(coords, row_bits_mask);
	*col_rel_out = vec_set_iter_epi64(iter, _bextr_u64(coords[iter], row_bits, col_bits));
}


static __attribute__((always_inline)) inline
void
gather_coords_dense_2(long i, unsigned char * data_coords, __attribute__((unused)) const uint64_t coords_bytes, uint64_t row_bits, uint64_t col_bits, uint64_t * row_rel_out, uint64_t * col_rel_out)
{
	const uint64_t row_bits_mask = (1ULL<<row_bits) - 1;   // here '-' has precedence over '<<'!
	uint64_t coords;
	coords = *((uint16_t *) &data_coords[i*2]);
	*row_rel_out = coords & row_bits_mask;
	*col_rel_out = _bextr_u64(coords, row_bits, col_bits);
}

static __attribute__((always_inline)) inline
void
gather_coords_dense_2_v(long i, unsigned char * data_coords, __attribute__((unused)) const uint64_t coords_bytes, uint64_t row_bits, uint64_t col_bits, vec_i_t * row_rel_out, vec_i_t * col_rel_out)
{
	// error("test d2");
	const vec_i_t row_bits_mask = vec_set1_epi64((1ULL<<row_bits) - 1);   // here '-' has precedence over '<<'!
	vec_i_t coords;
	coords = vec_set_iter_epi64(iter, *((uint16_t *) &data_coords[(i+iter)*2]));
	// *row_rel_out = coords & row_bits_mask;
	*row_rel_out = vec_and_si(coords, row_bits_mask);
	*col_rel_out = vec_set_iter_epi64(iter, _bextr_u64(coords[iter], row_bits, col_bits));
}


static __attribute__((always_inline)) inline
void
gather_coords_dense_3(long i, unsigned char * data_coords, __attribute__((unused)) const uint64_t coords_bytes, uint64_t row_bits, uint64_t col_bits, uint64_t * row_rel_out, uint64_t * col_rel_out)
{
	const uint64_t row_bits_mask = (1ULL<<row_bits) - 1;   // here '-' has precedence over '<<'!
	uint64_t coords;
	coords = *((uint32_t *) &data_coords[i*3]);
	*row_rel_out = coords & row_bits_mask;
	*col_rel_out = _bextr_u64(coords, row_bits, col_bits);
}

static __attribute__((always_inline)) inline
void
gather_coords_dense_3_v(long i, unsigned char * data_coords, __attribute__((unused)) const uint64_t coords_bytes, uint64_t row_bits, uint64_t col_bits, vec_i_t * row_rel_out, vec_i_t * col_rel_out)
{
	// error("test d3");
	const vec_i_t row_bits_mask = vec_set1_epi64((1ULL<<row_bits) - 1);   // here '-' has precedence over '<<'!
	vec_i_t coords;
	coords = vec_set_iter_epi64(iter, *((uint32_t *) &data_coords[(i+iter)*3]));
	// *row_rel_out = coords & row_bits_mask;
	*row_rel_out = vec_and_si(coords, row_bits_mask);
	*col_rel_out = vec_set_iter_epi64(iter, _bextr_u64(coords[iter], row_bits, col_bits));
}


static __attribute__((always_inline)) inline
void
gather_coords_dense_4(long i, unsigned char * data_coords, __attribute__((unused)) const uint64_t coords_bytes, uint64_t row_bits, uint64_t col_bits, uint64_t * row_rel_out, uint64_t * col_rel_out)
{
	const uint64_t row_bits_mask = (1ULL<<row_bits) - 1;   // here '-' has precedence over '<<'!
	uint64_t coords;
	coords = *((uint32_t *) &data_coords[i*4]);
	*row_rel_out = coords & row_bits_mask;
	*col_rel_out = _bextr_u64(coords, row_bits, col_bits);
}

static __attribute__((always_inline)) inline
void
gather_coords_dense_4_v(long i, unsigned char * data_coords, __attribute__((unused)) const uint64_t coords_bytes, uint64_t row_bits, uint64_t col_bits, vec_i_t * row_rel_out, vec_i_t * col_rel_out)
{
	// error("test d4");
	const vec_i_t row_bits_mask = vec_set1_epi64((1ULL<<row_bits) - 1);   // here '-' has precedence over '<<'!
	vec_i_t coords;
	coords = vec_set_iter_epi64(iter, *((uint32_t *) &data_coords[(i+iter)*4]));
	// *row_rel_out = coords & row_bits_mask;
	*row_rel_out = vec_and_si(coords, row_bits_mask);
	*col_rel_out = vec_set_iter_epi64(iter, _bextr_u64(coords[iter], row_bits, col_bits));
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//------------------------------------------------------------------------------------------------------------------------------------------
//-                                               Gather Coordinates Sparse - Row Bits 0                                                   -
//------------------------------------------------------------------------------------------------------------------------------------------
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


static __attribute__((always_inline)) inline
void
gather_coords_sparse_r0(long i, unsigned char * data_coords, const uint64_t coords_bytes, __attribute__((unused)) uint64_t row_bits, __attribute__((unused)) uint64_t col_bits, uint64_t * row_rel_out, uint64_t * col_rel_out)
{
	// error("test 0");
	const uint64_t col_bits_mask = (1ULL<<col_bits) - 1;
	uint64_t col_rel;
	col_rel = *((uint32_t *) &data_coords[i*coords_bytes]);
	col_rel = col_rel & col_bits_mask;
	*row_rel_out = 0;
	*col_rel_out = col_rel;
}

static __attribute__((always_inline)) inline
void
gather_coords_sparse_r0_v(long i, unsigned char * data_coords, const uint64_t coords_bytes, __attribute__((unused)) uint64_t row_bits, __attribute__((unused)) uint64_t col_bits, vec_i_t * row_rel_out, vec_i_t * col_rel_out)
{
	// error("test 0");
	const vec_i_t col_bits_mask = vec_set1_epi64((1ULL<<col_bits) - 1);
	vec_i_t col_rel;
	col_rel = vec_set_iter_epi64(iter, *((uint32_t *) &data_coords[(i+iter)*coords_bytes]));
	// col_rel = col_rel & col_bits_mask;
	col_rel = vec_and_si(col_rel, col_bits_mask);
	*row_rel_out = vec_set1_epi64(0);
	*col_rel_out = col_rel;
}


static __attribute__((always_inline)) inline
void
gather_coords_sparse_r0_c1(long i, unsigned char * data_coords, __attribute__((unused)) const uint64_t coords_bytes, __attribute__((unused)) uint64_t row_bits, __attribute__((unused)) uint64_t col_bits, uint64_t * row_rel_out, uint64_t * col_rel_out)
{
	// error("test 0,1");
	uint64_t col_rel;
	col_rel = *((uint8_t *) &data_coords[i*1]);
	*row_rel_out = 0;
	*col_rel_out = col_rel;
}

static __attribute__((always_inline)) inline
void
gather_coords_sparse_r0_c1_v(long i, unsigned char * data_coords, __attribute__((unused)) const uint64_t coords_bytes, __attribute__((unused)) uint64_t row_bits, __attribute__((unused)) uint64_t col_bits, vec_i_t * row_rel_out, vec_i_t * col_rel_out)
{
	// error("test 0,1");
	vec_i_t col_rel;
	col_rel = vec_set_iter_epi64(iter, *((uint8_t *) &data_coords[(i+iter)*1]));
	*row_rel_out = vec_set1_epi64(0);
	*col_rel_out = col_rel;
}


static __attribute__((always_inline)) inline
void
gather_coords_sparse_r0_c2(long i, unsigned char * data_coords, __attribute__((unused)) const uint64_t coords_bytes, __attribute__((unused)) uint64_t row_bits, __attribute__((unused)) uint64_t col_bits, uint64_t * row_rel_out, uint64_t * col_rel_out)
{
	// error("test 0,2");
	uint64_t col_rel;
	col_rel = *((uint16_t *) &data_coords[i*2]);
	*row_rel_out = 0;
	*col_rel_out = col_rel;
}

static __attribute__((always_inline)) inline
void
gather_coords_sparse_r0_c2_v(long i, unsigned char * data_coords, __attribute__((unused)) const uint64_t coords_bytes, __attribute__((unused)) uint64_t row_bits, __attribute__((unused)) uint64_t col_bits, vec_i_t * row_rel_out, vec_i_t * col_rel_out)
{
	// error("test 0,2");
	vec_i_t col_rel;
	col_rel = vec_set_iter_epi64(iter, *((uint16_t *) &data_coords[(i+iter)*2]));
	*row_rel_out = vec_set1_epi64(0);
	*col_rel_out = col_rel;
}


static __attribute__((always_inline)) inline
void
gather_coords_sparse_r0_c3(long i, unsigned char * data_coords, __attribute__((unused)) const uint64_t coords_bytes, __attribute__((unused)) uint64_t row_bits, __attribute__((unused)) uint64_t col_bits, uint64_t * row_rel_out, uint64_t * col_rel_out)
{
	// error("test 0,3");
	const uint64_t col_bits_mask = (1ULL<<col_bits) - 1;
	uint64_t col_rel;
	col_rel = *((uint32_t *) &data_coords[i*3]);
	col_rel = col_rel & col_bits_mask;
	*row_rel_out = 0;
	*col_rel_out = col_rel;
}

static __attribute__((always_inline)) inline
void
gather_coords_sparse_r0_c3_v(long i, unsigned char * data_coords, __attribute__((unused)) const uint64_t coords_bytes, __attribute__((unused)) uint64_t row_bits, __attribute__((unused)) uint64_t col_bits, vec_i_t * row_rel_out, vec_i_t * col_rel_out)
{
	// error("test 0,3");
	const vec_i_t col_bits_mask = vec_set1_epi64((1ULL<<24) - 1);
	vec_i_t col_rel;
	col_rel = vec_set_iter_epi64(iter, *((uint32_t *) &data_coords[(i+iter)*3]));
	// col_rel = col_rel & col_bits_mask;
	col_rel = vec_and_si(col_rel, col_bits_mask);
	*row_rel_out = vec_set1_epi64(0);
	*col_rel_out = col_rel;
}


static __attribute__((always_inline)) inline
void
gather_coords_sparse_r0_c4(long i, unsigned char * data_coords, __attribute__((unused)) const uint64_t coords_bytes, __attribute__((unused)) uint64_t row_bits, __attribute__((unused)) uint64_t col_bits, uint64_t * row_rel_out, uint64_t * col_rel_out)
{
	// error("test 0,4");
	uint64_t col_rel;
	col_rel = *((uint32_t *) &data_coords[i*4]);
	*row_rel_out = 0;
	*col_rel_out = col_rel;
}

static __attribute__((always_inline)) inline
void
gather_coords_sparse_r0_c4_v(long i, unsigned char * data_coords, __attribute__((unused)) const uint64_t coords_bytes, __attribute__((unused)) uint64_t row_bits, __attribute__((unused)) uint64_t col_bits, vec_i_t * row_rel_out, vec_i_t * col_rel_out)
{
	// error("test 0,4");
	vec_i_t col_rel;
	col_rel = vec_set_iter_epi64(iter, *((uint32_t *) &data_coords[(i+iter)*4]));
	*row_rel_out = vec_set1_epi64(0);
	*col_rel_out = col_rel;
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//------------------------------------------------------------------------------------------------------------------------------------------
//-                                               Gather Coordinates Sparse - Row Bits 1                                                   -
//------------------------------------------------------------------------------------------------------------------------------------------
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


static __attribute__((always_inline)) inline
void
gather_coords_sparse_r1(long i, unsigned char * data_coords, const uint64_t coords_bytes, __attribute__((unused)) uint64_t row_bits, uint64_t col_bits, uint64_t * row_rel_out, uint64_t * col_rel_out)
{
	const uint64_t col_bits_mask = (1ULL<<col_bits) - 1;
	uint64_t row_rel, col_rel;
	row_rel = *((uint8_t *) &data_coords[i*coords_bytes]);
	col_rel = *((uint32_t *) &data_coords[i*coords_bytes + 1]);
	col_rel = col_rel & col_bits_mask;
	*row_rel_out = row_rel;
	*col_rel_out = col_rel;
}

static __attribute__((always_inline)) inline
void
gather_coords_sparse_r1_v(long i, unsigned char * data_coords, const uint64_t coords_bytes, __attribute__((unused)) uint64_t row_bits, uint64_t col_bits, vec_i_t * row_rel_out, vec_i_t * col_rel_out)
{
	const vec_i_t col_bits_mask = vec_set1_epi64((1ULL<<col_bits) - 1);
	vec_i_t row_rel, col_rel;
	row_rel = vec_set_iter_epi64(iter, *((uint8_t *) &data_coords[(i+iter)*coords_bytes]));
	col_rel = vec_set_iter_epi64(iter, *((uint32_t *) &data_coords[(i+iter)*coords_bytes + 1]));
	// col_rel = col_rel & col_bits_mask;
	col_rel = vec_and_si(col_rel, col_bits_mask);
	*row_rel_out = row_rel;
	*col_rel_out = col_rel;
}


static __attribute__((always_inline)) inline
void
gather_coords_sparse_r1_c1(long i, unsigned char * data_coords, __attribute__((unused)) const uint64_t coords_bytes, __attribute__((unused)) uint64_t row_bits, __attribute__((unused)) uint64_t col_bits, uint64_t * row_rel_out, uint64_t * col_rel_out)
{
	// error("test 1\n");
	uint64_t row_rel, col_rel;
	row_rel = *((uint8_t *) &data_coords[i*2]);
	col_rel = *((uint8_t *) &data_coords[i*2 + 1]);
	*row_rel_out = row_rel;
	*col_rel_out = col_rel;
}

static __attribute__((always_inline)) inline
void
gather_coords_sparse_r1_c1_v(long i, unsigned char * data_coords, __attribute__((unused)) const uint64_t coords_bytes, __attribute__((unused)) uint64_t row_bits, __attribute__((unused)) uint64_t col_bits, vec_i_t * row_rel_out, vec_i_t * col_rel_out)
{
	// error("test 1\n");
	vec_i_t row_rel, col_rel;
	row_rel = vec_set_iter_epi64(iter, *((uint8_t *) &data_coords[(i+iter)*2]));
	col_rel = vec_set_iter_epi64(iter, *((uint8_t *) &data_coords[(i+iter)*2 + 1]));
	*row_rel_out = row_rel;
	*col_rel_out = col_rel;
}


static __attribute__((always_inline)) inline
void
gather_coords_sparse_r1_c2(long i, unsigned char * data_coords, __attribute__((unused)) const uint64_t coords_bytes, __attribute__((unused)) uint64_t row_bits, __attribute__((unused)) uint64_t col_bits, uint64_t * row_rel_out, uint64_t * col_rel_out)
{
	// error("test 2");
	uint64_t row_rel, col_rel;
	row_rel = *((uint8_t *) &data_coords[i*3]);
	col_rel = *((uint16_t *) &data_coords[i*3 + 1]);
	*row_rel_out = row_rel;
	*col_rel_out = col_rel;
}

static __attribute__((always_inline)) inline
void
gather_coords_sparse_r1_c2_v(long i, unsigned char * data_coords, __attribute__((unused)) const uint64_t coords_bytes, __attribute__((unused)) uint64_t row_bits, __attribute__((unused)) uint64_t col_bits, vec_i_t * row_rel_out, vec_i_t * col_rel_out)
{
	// error("test 2");
	vec_i_t row_rel, col_rel;
	row_rel = vec_set_iter_epi64(iter, *((uint8_t *) &data_coords[(i+iter)*3]));
	col_rel = vec_set_iter_epi64(iter, *((uint16_t *) &data_coords[(i+iter)*3 + 1]));
	*row_rel_out = row_rel;
	*col_rel_out = col_rel;
}


static __attribute__((always_inline)) inline
void
gather_coords_sparse_r1_c3(long i, unsigned char * data_coords, __attribute__((unused)) const uint64_t coords_bytes, __attribute__((unused)) uint64_t row_bits, __attribute__((unused)) uint64_t col_bits, uint64_t * row_rel_out, uint64_t * col_rel_out)
{
	// error("test 3");
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
gather_coords_sparse_r1_c3_v(long i, unsigned char * data_coords, __attribute__((unused)) const uint64_t coords_bytes, __attribute__((unused)) uint64_t row_bits, __attribute__((unused)) uint64_t col_bits, vec_i_t * row_rel_out, vec_i_t * col_rel_out)
{
	// error("test 3");
	const vec_i_t col_bits_mask = vec_set1_epi64((1ULL<<24) - 1);
	vec_i_t row_rel, col_rel;
	row_rel = vec_set_iter_epi64(iter, *((uint8_t *) &data_coords[(i+iter)*4]));
	col_rel = vec_set_iter_epi64(iter, *((uint32_t *) &data_coords[(i+iter)*4 + 1]));
	// col_rel = col_rel & col_bits_mask;
	col_rel = vec_and_si(col_rel, col_bits_mask);
	*row_rel_out = row_rel;
	*col_rel_out = col_rel;
}


static __attribute__((always_inline)) inline
void
gather_coords_sparse_r1_c4(long i, unsigned char * data_coords, __attribute__((unused)) const uint64_t coords_bytes, __attribute__((unused)) uint64_t row_bits, __attribute__((unused)) uint64_t col_bits, uint64_t * row_rel_out, uint64_t * col_rel_out)
{
	// error("test 4");
	uint64_t row_rel, col_rel;
	row_rel = *((uint8_t *) &data_coords[i*5]);
	col_rel = *((uint32_t *) &data_coords[i*5 + 1]);
	*row_rel_out = row_rel;
	*col_rel_out = col_rel;
}

static __attribute__((always_inline)) inline
void
gather_coords_sparse_r1_c4_v(long i, unsigned char * data_coords, __attribute__((unused)) const uint64_t coords_bytes, __attribute__((unused)) uint64_t row_bits, __attribute__((unused)) uint64_t col_bits, vec_i_t * row_rel_out, vec_i_t * col_rel_out)
{
	// error("test 4");
	vec_i_t row_rel, col_rel;
	row_rel = vec_set_iter_epi64(iter, *((uint8_t *) &data_coords[(i+iter)*5]));
	col_rel = vec_set_iter_epi64(iter, *((uint32_t *) &data_coords[(i+iter)*5 + 1]));
	*row_rel_out = row_rel;
	*col_rel_out = col_rel;
}


#endif /* SPMV_KERNEL_CSR_CV_STREAM_GATHER_COORDS_H */

