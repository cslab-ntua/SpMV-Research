#ifndef RANDOM_H
#define RANDOM_H

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <float.h>

#include "macros/cpp_defines.h"


// Max return value of random() = 2^31 - 1.
#define RANDOM_MAX_32  0x7FFFFFFFLL

// Max value of two 31 bit random numbers concatenated.
#define RANDOM_MAX_64  0x3FFFFFFFFFFFFFFFLL

// Max xorshift 64 bit value.
#define RANDOM_XS_MAX_64  ((__uint128_t) 0xFFFFFFFFFFFFFFFFULL)
// Max xorshift 32 bit value.
#define RANDOM_XS_MAX_32  0xFFFFFFFFULL



struct Random_State {
	unsigned int seed;
	size_t statelen;
	char * statebuf;
	struct random_data * buf;

	uint64_t xs_seed;
	int xs_variant;
	uint64_t xs_state;

	char padding[0] __attribute__((aligned(64)));
} __attribute__((aligned(64)));


struct Random_State * random_new(unsigned int seed);
void random_clean(struct Random_State * rs);
void random_destroy(struct Random_State ** rs_ptr);

void random_reseed(struct Random_State * rs, unsigned int seed);
void random_xs_reseed(struct Random_State * rs, unsigned int seed, int variant);


// Return random longs from a uniform distribution over [min, max).
int64_t random_uniform_integer_32bit(struct Random_State * rs, long min, long max);
int64_t random_uniform_integer_64bit(struct Random_State * rs, long min, long max);
long random_uniform_integer(struct Random_State * rs, long min, long max);
// Return random longs from a uniform distribution over [min, max).
// Uses a fast xorshift implementation, but possibly gives a worse quality distribution.
int64_t random_xs_uniform_integer_32bit(struct Random_State * rs, long min, long max);
int64_t random_xs_uniform_integer_64bit(struct Random_State * rs, long min, long max);
int64_t random_xs_uniform_integer(struct Random_State * rs, long min, long max);

// Return random doubles from a uniform distribution over [min, max).
double random_uniform(struct Random_State * rs, double min, double max);
double random_xs_uniform(struct Random_State * rs, double min, double max);

/* Box-Muller transform.
 */
double random_normal(struct Random_State * rs, double mean, double std);

/* Marsaglia-Tsang fast gamma method.
 *
 * Notes:
 *     - k:shape , theta:scale
 *
 *     - k > 0 , theta > 0
 *
 *     - mean = k * theta    (> 0)
 *     - var = theta^2 / k   (> 0)
 *
 *     - k = mean^2 / var =  mean^2 / std^2
 *       theta = var / mean =  std^2 / mean
 */
double random_gamma(struct Random_State * rs, double k, double theta);

void random_permutation_serial(struct Random_State * rs, long * permutation_buf, long N);
void random_permutation_parallel(struct Random_State * rs, long * permutation_buf, long N);


#endif /* RANDOM_H */

