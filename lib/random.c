#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <float.h>
#include <math.h>
#include <omp.h>

#include "random.h"

#include "debug.h"
#include "hash/hash.h"
#include "parallel_util.h"
#include "omp_functions.h"

/*
 * Notes:
 *     - POSIX.1-2008 marks rand_r() as obsolete.
 *       On older rand() implementations, and on current implementations on different systems, the lower-order bits are much less random than the higher-order bits.
 *       Do not use this function in applications intended to be portable when good randomness is needed.  (Use random(3) instead.)
 *
 *     - float.h:
 *       FLT_EPSILON
 *           This is the difference between 1 and the smallest floating point number of type float that is greater than 1.
 *           It’s supposed to be no greater than 1E-5.
 *       DBL_EPSILON , LDBL_EPSILON
 *           These are similar to FLT_EPSILON, but for the data types double and long double, respectively.
 *           The type of the macro’s value is the same as the type it describes.
 *           The values are not supposed to be greater than 1E-9.
 *
 *     - RANDOM_MAX_32     = 2^31 - 1
 *       RANDOM_MAX_32     = 00000000 00000000 00000000 00000000 01111111 11111111 11111111 11111111
 *       RANDOM_MAX_32 + 1 = 00000000 00000000 00000000 00000000 10000000 00000000 00000000 00000000
 *
 *     - Reproducibility:
 *       When calling 'random()' / 'random_r()' consecutively we need to take into
 *       account instruction reordering if we want reproducible results.
 *       E.g.:
 *           if
 *                next(random()) -> 1
 *                next(random()) -> 2
 *           then
 *               x = random() + random() * I
 *           could be one of the two:
 *               1 + 2 * I
 *           or 
 *               2 + 1 * I
 *           because no barriers are inserted.
 *
 *           This might also happen if we use '#pragma GCC ivdep' for the initialization loop of an array.
 *
 * long random(void);
 * void srandom(unsigned seed);
 * char *initstate(unsigned seed, char *state, size_t n);
 * char *setstate(char *state);
 *
 *     The  random() function uses a nonlinear additive feedback random number generator employing a default table of size 31 long integers to return successive pseudo-random numbers in the range from 0 to 2^31 - 1.
 *     The period of this random number generator is very large, approximately 16 * ((2^31) - 1).
 *
 *     The srandom() function sets its argument as the seed for a new sequence of pseudo-random integers to be returned by random().
 *     These sequences are repeatable by calling srandom() with the same seed value.
 *     If no seed value is provided, the random() function is  automatically seeded with a value of 1.
 *     
 *     The initstate() function allows a state array state to be initialized for use by random().
 *     The size of the state array n is used by initstate() to decide how sophisticated a random number generator it should use—the larger the state array, the better the random numbers will be.
 *     Current "optimal" values for the size of the state array n are 8, 32, 64, 128, and 256 bytes; other amounts will be rounded down to the nearest known amount.
 *     Using less than 8 bytes results in an error.
 *     seed is the seed for the initialization, which specifies a starting point for the random number sequence, and provides for restarting at the same point.
 *     
 *     The  setstate() function changes the state array used by the random() function.
 *     The state array state is used for random number generation until the next call to initstate() or setstate().
 *     state must first have been initialized using initstate() or be the result of a previous call of setstate().
 *
 *     RETURN VALUE
 *         The random() function returns a value between 0 and (2^31) - 1.  The srandom() function returns no value.
 *         The initstate() function returns a pointer to the previous state array.  On error, errno is set to indicate the cause.
 *         On success, setstate() returns a pointer to the previous state array.  On error, it returns NULL, with errno set to indicate the cause of the error.
 *
 * int random_r(struct random_data *buf, int32_t *result);
 * int srandom_r(unsigned int seed, struct random_data *buf);
 * int initstate_r(unsigned int seed, char *statebuf, size_t statelen, struct random_data *buf);
 * int setstate_r(char *statebuf, struct random_data *buf);
 *
 *     The  random_r()  function  is like random(3), except that instead of using state information maintained in a global variable,
 *     it uses the state information in the argument pointed to by buf, which must have been previously initialized by initstate_r().
 *     The generated random number is returned in the argument result.
 *
 *     The srandom_r() function is like srandom(3), except that it initializes the seed for the random number generator whose state is maintained in the object pointed to by buf,
 *     which must have been previously initialized by initstate_r(), instead of  the  seed  associated with the global state variable.
 *
 *     The  initstate_r()  function  is  like  initstate(3)  except  that it initializes the state in the object pointed to by buf, rather than initializing the global state variable.
 *     Before calling this function, the buf.state field must be initialized to NULL.
 *     The initstate_r() function records a pointer to the statebuf argument inside the structure pointed to by buf.
 *     Thus, statebuf should not be deallocated so long as buf is still in use.
 *     (So, statebuf should typically be allocated as a static variable, or allocated on the heap using malloc(3) or similar.)
 *
 *     The  setstate_r()  function is like setstate(3) except that it modifies the state in the object pointed to by buf, rather than modifying the global state variable.
 *     'state' must first have been initialized using initstate_r() or be the result of a previous call of setstate_r().
 *
 *     RETURN VALUE
 *         All of these functions return 0 on success.  On error, -1 is returned, with errno set to indicate the cause of the error.
 *
 * struct random_data
 * {
 * 	int32_t *fptr;               // Front pointer.
 * 	int32_t *rptr;               // Rear pointer.
 * 	int32_t *state;              // Array of state values.
 * 	int rand_type;               // Type of random number generator.
 * 	int rand_deg;                // Degree of random number generator.
 * 	int rand_sep;                // Distance between front and rear.
 * 	int32_t *end_ptr;            // Pointer behind state table.
 * };
 */


//==========================================================================================================================================
//= Constructors / Destructors
//==========================================================================================================================================


struct Random_State *
random_new(unsigned int seed)
{
	struct Random_State * rs;
	size_t statelen;
	char * statebuf;
	struct random_data * buf;

	statelen = 256;      // Run times between 64 and 256 are identical.

	buf = (typeof(buf)) calloc(1, sizeof(*buf));
	statebuf = (typeof(statebuf)) calloc(statelen, sizeof(*statebuf));

	buf->state = NULL;
	initstate_r(seed, statebuf, statelen, buf);         // Before calling this function, the buf.state field must be initialized to NULL.
	srandom_r(seed, buf);

	rs = (typeof(rs)) malloc(sizeof(*rs));
	rs->statelen = statelen;
	rs->statebuf = statebuf;
	rs->buf = buf;
	rs->seed = seed;

	// rs->xs_state = 0;
	rs->xs_seed = seed;
	rs->xs_variant = 0;
	rs->xs_state = xorshift64_int(seed, 0, rs->xs_variant);

	return rs;
}


void
random_clean(struct Random_State * rs)
{
	free(rs->statebuf);
	rs->statebuf = NULL;
	free(rs->buf);
	rs->buf = NULL;
}


void
random_destroy(struct Random_State ** rs_ptr)
{
	random_clean(*rs_ptr);
	free(*rs_ptr);
	*rs_ptr = NULL;
}


void
random_reseed(struct Random_State * rs, unsigned int seed)
{
	srandom_r(seed, rs->buf);
}


void
random_xs_reseed(struct Random_State * rs, unsigned int seed, int variant)
{
	rs->xs_seed = seed;
	rs->xs_variant = variant;
	rs->xs_state = xorshift64_int(seed, 0, rs->xs_variant);
}


//==========================================================================================================================================
//------------------------------------------------------------------------------------------------------------------------------------------
//-                                                      Random Number Generators                                                          -
//------------------------------------------------------------------------------------------------------------------------------------------
//==========================================================================================================================================


//==========================================================================================================================================
//= Uniform - Integers
//==========================================================================================================================================


int64_t
random_uniform_integer_32bit(struct Random_State * rs, long min, long max)
{
	if (min > max)
		error("min = %ld should be smaller than max = %ld", min, max);
	if (max - min > RANDOM_MAX_32)
		error("range given exceeds maximum 32 bit random integer supported: %ld > %ld", max - min, RANDOM_MAX_32);
	const uint64_t range = max - min;
	uint64_t range_divisible = RANDOM_MAX_32 - RANDOM_MAX_32 % range;   // Correct modulo bias.
	uint32_t val;
	uint64_t res;
	do {
		random_r(rs->buf, (int32_t *) &val);
	} while (__builtin_expect(val >= range_divisible, 0));
	res = val % range + min;
	return res;
}


int64_t
random_uniform_integer_64bit(struct Random_State * rs, long min, long max)
{
	if (min > max)
		error("min = %ld should be smaller than max = %ld", min, max);
	const uint64_t range = max - min;
	uint64_t range_divisible = RANDOM_MAX_64 - RANDOM_MAX_64 % range;   // Correct modulo bias.
	uint32_t val;
	uint64_t res;
	do {
		random_r(rs->buf, (int32_t *) &val);
		res = val;
		res <<= 31;
		random_r(rs->buf, (int32_t *) &val);
		res |= val;
	} while (__builtin_expect(res >= range_divisible, 0));
	res = res % range + min;
	return res;
}


// Return random longs from a uniform distribution over [min, max).
int64_t
random_uniform_integer(struct Random_State * rs, long min, long max)
{
	if (min > max)
		error("min = %ld should be smaller than max = %ld", min, max);
	const uint64_t range = max - min;
	uint64_t res;
	if (range >= RANDOM_MAX_32)
		res = random_uniform_integer_64bit(rs, min, max);
	else
		res = random_uniform_integer_32bit(rs, min, max);
	return res;
}


static inline
uint64_t
RANDOM_xorshift64(struct Random_State * rs)
{
	uint64_t hash;
	hash = xorshift64_int(rs->xs_state, 0, rs->xs_variant);
	// hash = xorshift64_int(0, rs->xs_state, rs->xs_variant);
	// rs->xs_state = hash;
	rs->xs_state++;
	return hash;
}


int64_t
random_xs_uniform_integer_32bit(struct Random_State * rs, long min, long max)
{
	if (min > max)
		error("min = %ld should be smaller than max = %ld", min, max);
	if (((uint64_t) max - min) > RANDOM_XS_MAX_32)
		error("range given exceeds maximum 32 bit random integer supported: %ld > %ld", max - min, RANDOM_MAX_32);
	const uint64_t umin = min, umax = max;
	const uint64_t range = umax - umin;
	const uint64_t range_divisible = RANDOM_XS_MAX_32 - RANDOM_XS_MAX_32 % range;   // Correct modulo bias.
	uint64_t hash;
	do {
		hash = RANDOM_xorshift64(rs);
		hash -= hash >> 32;   // Utilize the randomness of all 64 bits.
		hash = (uint32_t) hash;
	} while (__builtin_expect(hash >= range_divisible, 0));
	hash = (hash * range) >> 32;
	if (range > 0x7FFFFFFF)       // Degrade to 31 bits random number to keep range <= max SIGNED 32 bit integer.
		hash &= 0x7FFFFFFF;
	// hash %= range;
	hash += umin;
	return hash;
}


/* If (min < max < 0) or (0 > max > min), then unsigned min < unsigned max.
 * If (min < 0 < max) then we can't know.
 * But: range == (unsigned min - unsigned max) == (min - max) >= 0.
 * Therfor we need to pass them AS IS.
 * 
 * If range > (max_64_bit / 2) we degrade to 63 bit of randomness, i.e. an 63 bit positive number,
 * so that we can displace later by 'umin' without causing overflow.
 *
 * No real need for correction for modulo bias, range will need to be stupidly large,
 * but do it either way.
 */
int64_t
random_xs_uniform_integer_64bit(struct Random_State * rs, long min, long max)
{
	if (min > max)
		error("min = %ld should be smaller than max = %ld", min, max);
	const uint64_t umin = min, umax = max;
	const __uint128_t range = umax - umin;
	const uint64_t range_divisible = RANDOM_XS_MAX_64 - RANDOM_XS_MAX_64 % range;   // Correct modulo bias.
	uint64_t hash;
	do {
		hash = RANDOM_xorshift64(rs);
	} while (__builtin_expect(hash >= range_divisible, 0));
	hash = (((__uint128_t) hash) * range) >> 64;
	if (range > 0x7FFFFFFFFFFFFFFF)       // Degrade to 63 bits random number to keep range <= max SIGNED 64 bit integer.
		hash &= 0x7FFFFFFFFFFFFFFF;
	// hash %= range;
	hash += umin;
	return hash;
}


int64_t
random_xs_uniform_integer(struct Random_State * rs, long min, long max)
{
	return random_xs_uniform_integer_64bit(rs, min, max);
}


//==========================================================================================================================================
//= Uniform - Floats
//==========================================================================================================================================


// Return random doubles from a uniform distribution over [min, max).
double
random_uniform(struct Random_State * rs, double min, double max)
{
	if (min > max)
		error("min = %ld should be smaller than max = %ld", min, max);
	int32_t val;
	double res;
	random_r(rs->buf, &val);
	res = ((double) val) / ((double) (RANDOM_MAX_32 + 1L));
	res = (max - min) * res + min;
	return res;
}


// Return random doubles from a uniform distribution over [min, max).
double
random_xs_uniform(struct Random_State * rs, double min, double max)
{
	if (min > max)
		error("min = %ld should be smaller than max = %ld", min, max);
	double res;
	uint64_t val = RANDOM_xorshift64(rs);
	res = ((double) val) / ((double) (RANDOM_XS_MAX_64 + 1L));
	res = (max - min) * res + min;
	return res;
}


//==========================================================================================================================================
//= Normal - Floats
//==========================================================================================================================================


/* Box-Muller transform (wikipedia)
 */
double
random_normal(struct Random_State * rs, double mean, double std)
{
	double u1, u2;
	// Create two uniform random numbers, make sure u1 is greater than epsilon.
	u1 = random_uniform(rs, 2*DBL_EPSILON, 1);
	u2 = random_uniform(rs, 0, 1);
	// Compute z0 and z1.
	double magnitude = std * sqrt(-2.0 * log(u1));
	double z0  = magnitude * cos(2.0 * M_PI * u2) + mean;
	// double z1  = magnitude * sin(2.0 * M_PI * u2) + mean;
	// if (isnan(z0))
		// error("isnan %lf\n", z0);
	return z0;
}


//==========================================================================================================================================
//= Gamma - Floats
//==========================================================================================================================================


/* Marsaglia-Tsang fast gamma method
 *
 * Notes:
 *     - k:shape , theta:scale
 *     - k > 0 , theta > 0
 *     - mean = k * theta    (> 0)
 *     - var = theta^2 / k   (> 0)
 *
 *     - k = mean^2 / var =  mean^2 / std^2
 *       theta = var / mean =  std^2 / mean
 */
double
random_gamma(struct Random_State * rs, double k, double theta)
{
	if (k < 1)
	{
		double u = random_uniform(rs, 2*DBL_EPSILON, 1);          // u > 0 ( > DBL_EPSILON for better stability)
		return random_gamma(rs, 1.0 + k, theta) * pow(u, 1.0 / k);
	}
	double x, v, u;
	double d = k - 1.0 / 3.0;
	double c = (1.0 / 3.0) / sqrt(d);
	while (1)
	{
		do {
			x = random_normal(rs, 0, 1);
			v = 1.0 + c * x;
		} while (v <= 0);
		v = v * v * v;
		u = random_uniform(rs, 2*DBL_EPSILON, 1);
		if (u < 1 - 0.0331 * x * x * x * x) 
			break;
		if (log(u) < 0.5 * x * x + d * (1 - v + log(v)))
			break;
	}
	return theta * d * v;
}


//==========================================================================================================================================
//------------------------------------------------------------------------------------------------------------------------------------------
//-                                                          Random Permutation                                                            -
//------------------------------------------------------------------------------------------------------------------------------------------
//==========================================================================================================================================


// Return a random permutation of longs from a uniform distribution over [0, N).

static inline
void
RANDOM_permutation_serial_base(struct Random_State * rs, long * permutation_buf, long N)
{
	int64_t i, j;
	for (i=0;i<N;i++)
	{
		j = random_xs_uniform_integer(rs, 0, N);
		macros_swap(&permutation_buf[i], &permutation_buf[j]);
	}
}


void
random_permutation_serial(struct Random_State * rs, long * permutation_buf, long N)
{
	int64_t i;
	for (i=0;i<N;i++)
		permutation_buf[i] = i;
	RANDOM_permutation_serial_base(rs, permutation_buf, N);
}


// a) random (uniform) distribution of elements to threads
// b) serial random permutation of thread-local elements
void
random_permutation_parallel(struct Random_State * rs, long * permutation_buf, long N)
{
	int num_threads = safe_omp_get_num_threads();
	[[gnu::cleanup(cleanup_free)]] long * const offsets = (typeof(offsets)) malloc((num_threads * num_threads + 1) * sizeof(*offsets));
	#pragma omp parallel
	{
		int tnum = omp_get_thread_num();
		[[gnu::cleanup(cleanup_free)]] struct Random_State * rs_t = random_new(0);
		long counts[num_threads];
		long i, i_s, i_e, j, j_s, j_e;
		long N_t;
		long sum, prev;
		random_xs_reseed(rs_t, tnum + rs->seed, 0);
		for (i=0;i<num_threads;i++)
			counts[i] = 0;
		loop_partitioner_balance_iterations(num_threads, tnum, 0, N, &i_s, &i_e);
		for (i=i_s;i<i_e;i++)
		{
			j = random_xs_uniform_integer(rs_t, 0, num_threads);
			counts[j]++;
		}
		#pragma omp barrier
		for (i=0;i<num_threads;i++)
			offsets[i * num_threads + tnum] = counts[i];
		#pragma omp barrier
		#pragma omp single nowait
		{
			sum = 0;
			for (i=0;i<num_threads * num_threads + 1;i++)
			{
				prev = offsets[i];
				offsets[i] = sum;
				sum += prev;
			}
		}
		#pragma omp barrier
		j_s = offsets[tnum * num_threads];
		j_e = offsets[(tnum+1) * num_threads];
		N_t = j_e - j_s;
		#pragma omp barrier
		random_xs_reseed(rs_t, tnum + rs->seed, 0);
		for (i=i_s;i<i_e;i++)
		{
			j = random_xs_uniform_integer(rs_t, 0, num_threads);
			permutation_buf[offsets[j * num_threads + tnum]] = i;
			offsets[j * num_threads + tnum]++;
		}
		#pragma omp barrier
		RANDOM_permutation_serial_base(rs_t, &permutation_buf[j_s], N_t);
	}
}

