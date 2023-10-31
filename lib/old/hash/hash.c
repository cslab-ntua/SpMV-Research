#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>

#include "hash.h"

#include "macros/macrolib.h"
#include "genlib.h"


uint64_t
hash_k_and_r(const void * buf, size_t N)
{
	const unsigned char * str = buf;
	uint64_t hash = 0;
	uint64_t i;
	uint64_t c;
	for (i=0;i<N;i++)
	{
		c = str[i];
		hash += c;
	}
	return hash;
}


uint64_t
hash_djb2(const void * buf, size_t N)
{
	const unsigned char * str = buf;
	uint64_t hash = 5381;
	uint64_t i;
	uint64_t c;
	for (i=0;i<N;i++)
	{
		c = str[i];
		hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
	}
	return hash;
}


//==========================================================================================================================================
//= Xorshift
//==========================================================================================================================================


/* 2003 - Xorshift RNGs - George Marsaglia
 * https://www.jstatsoft.org/article/view/v008i14
 *
 * Any one of these can be used to provide a xorshift RNG with period 2^64 - 1, for a total of 8*275 = 2200 choices:
 *     1) y^=y<<a; y^=y>>b; y^=y<<c;
 *     2) y^=y<<c; y^=y>>b; y^=y<<a;
 *     3) y^=y>>a; y^=y<<b; y^=y>>c;
 *     4) y^=y>>c; y^=y<<b; y^=y>>a;
 *     5) y^=y<<a; y^=y<<c; y^=y>>b;
 *     6) y^=y<<c; y^=y<<a; y^=y>>b;
 *     7) y^=y>>a; y^=y>>c; y^=y<<b;
 *     8) y^=y>>c; y^=y>>a; y^=y<<b;
 *
 * Not all are equally good for hashing!
 * e.g. 36 { 3,29,40}  >>  0 { 1, 1,54}
 * Why?
 * Due to modulo with N (size of hashtable)?
 */
static const unsigned int R[][3] = {
	/* 0   */  { 1, 1,54}, { 1, 1,55}, { 1, 3,45}, { 1, 7, 9}, { 1, 7,44}, { 1, 7,46}, { 1, 9,50}, { 1,11,35}, { 1,11,50},
	/* 9   */  { 1,13,45}, { 1,15, 4}, { 1,15,63}, { 1,19, 6}, { 1,19,16}, { 1,23,14}, { 1,23,29}, { 1,29,34}, { 1,35, 5},
	/* 18  */  { 1,35,11}, { 1,35,34}, { 1,45,37}, { 1,51,13}, { 1,53, 3}, { 1,59,14}, { 2,13,23}, { 2,31,51}, { 2,31,53},
	/* 27  */  { 2,43,27}, { 2,47,49}, { 3, 1,11}, { 3, 5,21}, { 3,13,59}, { 3,21,31}, { 3,25,20}, { 3,25,31}, { 3,25,56},
	/* 36  */  { 3,29,40}, { 3,29,47}, { 3,29,49}, { 3,35,14}, { 3,37,17}, { 3,43, 4}, { 3,43, 6}, { 3,43,11}, { 3,51,16},
	/* 45  */  { 3,53, 7}, { 3,61,17}, { 3,61,26}, { 4, 7,19}, { 4, 9,13}, { 4,15,51}, { 4,15,53}, { 4,29,45}, { 4,29,49},
	/* 54  */  { 4,31,33}, { 4,35,15}, { 4,35,21}, { 4,37,11}, { 4,37,21}, { 4,41,19}, { 4,41,45}, { 4,43,21}, { 4,43,31},
	/* 63  */  { 4,53, 7}, { 5, 9,23}, { 5,11,54}, { 5,15,27}, { 5,17,11}, { 5,23,36}, { 5,33,29}, { 5,41,20}, { 5,45,16},
	/* 72  */  { 5,47,23}, { 5,53,20}, { 5,59,33}, { 5,59,35}, { 5,59,63}, { 6, 1,17}, { 6, 3,49}, { 6,17,47}, { 6,23,27},
	/* 81  */  { 6,27, 7}, { 6,43,21}, { 6,49,29}, { 6,55,17}, { 7, 5,41}, { 7, 5,47}, { 7, 5,55}, { 7, 7,20}, { 7, 9,38},
	/* 90  */  { 7,11,10}, { 7,11,35}, { 7,13,58}, { 7,19,17}, { 7,19,54}, { 7,23, 8}, { 7,25,58}, { 7,27,59}, { 7,33, 8},
	/* 99  */  { 7,41,40}, { 7,43,28}, { 7,51,24}, { 7,57,12}, { 8, 5,59}, { 8, 9,25}, { 8,13,25}, { 8,13,61}, { 8,15,21},
	/* 108 */  { 8,25,59}, { 8,29,19}, { 8,31,17}, { 8,37,21}, { 8,51,21}, { 9, 1,27}, { 9, 5,36}, { 9, 5,43}, { 9, 7,18},
	/* 117 */  { 9,19,18}, { 9,21,11}, { 9,21,20}, { 9,21,40}, { 9,23,57}, { 9,27,10}, { 9,29,12}, { 9,29,37}, { 9,37,31},
	/* 126 */  { 9,41,45}, {10, 7,33}, {10,27,59}, {10,53,13}, {11, 5,32}, {11, 5,34}, {11, 5,43}, {11, 5,45}, {11, 9,14},
	/* 135 */  {11, 9,34}, {11,13,40}, {11,15,37}, {11,23,42}, {11,23,56}, {11,25,48}, {11,27,26}, {11,29,14}, {11,31,18},
	/* 144 */  {11,53,23}, {12, 1,31}, {12, 3,13}, {12, 3,49}, {12, 7,13}, {12,11,47}, {12,25,27}, {12,39,49}, {12,43,19},
	/* 153 */  {13, 3,40}, {13, 3,53}, {13, 7,17}, {13, 9,15}, {13, 9,50}, {13,13,19}, {13,17,43}, {13,19,28}, {13,19,47},
	/* 162 */  {13,21,18}, {13,21,49}, {13,29,35}, {13,35,30}, {13,35,38}, {13,47,23}, {13,51,21}, {14,13,17}, {14,15,19},
	/* 171 */  {14,23,33}, {14,31,45}, {14,47,15}, {15, 1,19}, {15, 5,37}, {15,13,28}, {15,13,52}, {15,17,27}, {15,19,63},
	/* 180 */  {15,21,46}, {15,23,23}, {15,45,17}, {15,47,16}, {15,49,26}, {16, 5,17}, {16, 7,39}, {16,11,19}, {16,11,27},
	/* 189 */  {16,13,55}, {16,21,35}, {16,25,43}, {16,27,53}, {16,47,17}, {17,15,58}, {17,23,29}, {17,23,51}, {17,23,52},
	/* 198 */  {17,27,22}, {17,45,22}, {17,47,28}, {17,47,29}, {17,47,54}, {18, 1,25}, {18, 3,43}, {18,19,19}, {18,25,21},
	/* 207 */  {18,41,23}, {19, 7,36}, {19, 7,55}, {19,13,37}, {19,15,46}, {19,21,52}, {19,25,20}, {19,41,21}, {19,43,27},
	/* 216 */  {20, 1,31}, {20, 5,29}, {21, 1,27}, {21, 9,29}, {21,13,52}, {21,15,28}, {21,15,29}, {21,17,24}, {21,17,30},
	/* 225 */  {21,17,48}, {21,21,32}, {21,21,34}, {21,21,37}, {21,21,38}, {21,21,40}, {21,21,41}, {21,21,43}, {21,41,23},
	/* 234 */  {22, 3,39}, {23, 9,38}, {23, 9,48}, {23, 9,57}, {23,13,38}, {23,13,58}, {23,13,61}, {23,17,25}, {23,17,54},
	/* 243 */  {23,17,56}, {23,17,62}, {23,41,34}, {23,41,51}, {24, 9,35}, {24,11,29}, {24,25,25}, {24,31,35}, {25, 7,46},
	/* 252 */  {25, 7,49}, {25, 9,39}, {25,11,57}, {25,13,29}, {25,13,39}, {25,13,62}, {25,15,47}, {25,21,44}, {25,27,27},
	/* 261 */  {25,27,53}, {25,33,36}, {25,39,54}, {28, 9,55}, {28,11,53}, {29,27,37}, {31, 1,51}, {31,25,37}, {31,27,35},
	/* 270 */  {33,31,43}, {33,31,55}, {43,21,46}, {49,15,61}, {55, 9,56}
};


#define xorshift_mix(h, r1, r2, r3)    \
({                                     \
	uint64_t _h = h;               \
	_h ^= _h >> r1;                \
	_h ^= _h << r2;                \
	_h ^= _h >> r3;                \
})


#define xorshift_mix_table(h, pos)                           \
({                                                           \
	xorshift_mix(h, R[pos][0], R[pos][1], R[pos][2]);    \
})


#define xorshift_mix_const(h)          \
({                                     \
	xorshift_mix_table(h, 272);    \
})


#define xorshift_fasthash64_mix(h)        \
({                                        \
	uint64_t _h = h;                  \
	(_h) ^= (_h) >> 23;               \
	(_h) *= 0x2127599bf4325c37ULL;    \
	(_h) ^= (_h) >> 47;               \
})


/* Operators:
 *     Multiplication has the good property that every bit of the arguments affect all the bits of the result.
 *     But it has the bad property that the commonly used 0 always results 0,
 *     no matter the other argument (bad for random number generators).
 *
 *     For xor, each bit affects only one bit of the result.
 *     But it is stable, in the sense that there is no zero element like multiplication.
 *
 * When seed is 0 it could degenerate the hash function to the constant 0 if used with the value.
 * If the user wants to implement something like a random number generator,
 * he can use the seed to utilize the multiplication.
 *
 * Using an initial xor just so that 0 doesn't map to 0.
 *
 * Multiplication in general is an unfair mapping, i.e. prime * N -> only multiples of 'prime'.
 * Multiplication modulo n is a fair mapping if the constant factor is coprime to n (therefor a generator).
 * But we also want it to be as fair as possible also for small sets, not just for the whole Z.
 * e.g. x*p mod n with small p will give the multiples of p first before exceeding n and cycling over.
 * Therefore it maybe be better to select big coprimes, that frequently cycle n.
 */
uint64_t
xorshift64_int(const uint64_t v, uint64_t seed, int variant)
{
	uint64_t hash;
	// Random large (> 2^63) coprimes to 2^64 (i.e. odd).
	const uint64_t c1 = 0xE83DA855883556E9ULL;  // [17 984395110334677337]
	const uint64_t c2 = 0xC5E492A87CBD0AD7ULL;  // [7 11 13337 13885483673003]
	const uint64_t c3 = 0xA6AC178D209C2587ULL;  // [83 16451 8795744793919]
	// const uint64_t c4 = 0x880355f21e6d1965ULL;  // [17 127 4539495929814379]
	// const uint64_t c5 = 0x2127599bf4325c37ULL;  // [7 11 41 9619459 78665929]

	hash = v;

	hash ^= seed;
	hash ^= c1;
	hash *= c2;
	hash = xorshift_mix_table(hash, 272); // By doing a known good shuffling we can safely use any other given variant later.
	hash *= c3;
	hash = xorshift_mix_table(hash, variant);
	// hash = xorshift_mix_const(hash);
	// hash = xorshift_fasthash64_mix(hash);

	// hash = v;
	// hash *= c2;
	// hash = xorshift_mix_table(hash, variant);
	// hash ^= seed ^ c2;
	// hash *= c2;
	// hash = xorshift_mix_table(hash, variant);
	// hash = xorshift_fasthash64_mix(hash);

	return hash;
} 


uint64_t
xorshift64_int_bounded(const uint64_t v, uint64_t seed, int variant, uint64_t min, uint64_t max)
{
	__uint128_t range = max - min;
	uint64_t hash = xorshift64_int(v, seed, variant);
	hash = (((__uint128_t) hash) * (range)) >> 64;
	// hash %= range;
	hash += min;
	return hash;
}


uint64_t
xorshift64(const void * buf, size_t len, uint64_t seed)
{
	const uint64_t m = 0x880355f21e6d1965ULL;
	const uint64_t * pos = (const uint64_t *) buf;
	const uint64_t * end = pos + (len / 8);
	const unsigned char * pos2;
	uint64_t h = xorshift_mix_const(seed ^ (len * m));
	uint64_t v;

	while (pos != end) {
		v  = *pos++;
		h ^= xorshift_mix_const(v);
	}

	pos2 = (const unsigned char *) pos;
	v = 0;
	switch (len & 7) {
	case 7: v ^= (uint64_t) pos2[6] << 48; /* fallthrough */
	case 6: v ^= (uint64_t) pos2[5] << 40; /* fallthrough */
	case 5: v ^= (uint64_t) pos2[4] << 32; /* fallthrough */
	case 4: v ^= (uint64_t) pos2[3] << 24; /* fallthrough */
	case 3: v ^= (uint64_t) pos2[2] << 16; /* fallthrough */
	case 2: v ^= (uint64_t) pos2[1] << 8;  /* fallthrough */
	case 1: v ^= (uint64_t) pos2[0];
		h ^= xorshift_mix_const(v);
	}

	return xorshift_mix_const(h);
} 


//==========================================================================================================================================
//= Fasthash
//==========================================================================================================================================


// Compression function for Merkle-Damgard construction.
#define fasthash64_mix(h)                \
({                                       \
	(h) ^= (h) >> 23;                \
	(h) *= 0x2127599bf4325c37ULL;    \
	(h) ^= (h) >> 47;                \
})

uint64_t
fasthash64(const void * buf, size_t len, uint64_t seed)
{
	const uint64_t m = 0x880355f21e6d1965ULL;
	const uint64_t * pos = (const uint64_t *) buf;
	const uint64_t * end = pos + (len / 8);
	const unsigned char * pos2;
	uint64_t h = seed ^ (len * m);
	uint64_t v;

	while (pos != end) {
		v  = *pos++;
		h ^= fasthash64_mix(v);
		h *= m;
	}

	pos2 = (const unsigned char*) pos;
	v = 0;

	switch (len & 7) {
	case 7: v ^= (uint64_t)pos2[6] << 48; /* fallthrough */
	case 6: v ^= (uint64_t)pos2[5] << 40; /* fallthrough */
	case 5: v ^= (uint64_t)pos2[4] << 32; /* fallthrough */
	case 4: v ^= (uint64_t)pos2[3] << 24; /* fallthrough */
	case 3: v ^= (uint64_t)pos2[2] << 16; /* fallthrough */
	case 2: v ^= (uint64_t)pos2[1] << 8;  /* fallthrough */
	case 1: v ^= (uint64_t)pos2[0];
		h ^= fasthash64_mix(v);
		h *= m;
	}

	return fasthash64_mix(h);
} 

uint32_t
fasthash32(const void * buf, size_t len, uint32_t seed)
{
	// the following trick converts the 64-bit hashcode to Fermat
	// residue, which shall retain information from both the higher
	// and lower parts of hashcode.
        uint64_t h = fasthash64(buf, len, seed);
	return h - (h >> 31);
}

