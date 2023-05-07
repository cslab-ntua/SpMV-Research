#ifndef HASH_H
#define HASH_H

#include <stdint.h>

#include "macros/cpp_defines.h"


uint64_t hash_k_and_r(const void * buf, size_t N);

uint64_t hash_djb2(const void * buf, size_t N);

uint64_t xorshift64_int(const uint64_t v, uint64_t seed, int variant);
uint64_t xorshift64_int_bounded(const uint64_t v, uint64_t seed, int variant, uint64_t min, uint64_t max);
uint64_t xorshift64(const void * buf, size_t len, uint64_t seed);

uint64_t fasthash64(const void * buf, size_t len, uint64_t seed);
uint32_t fasthash32(const void * buf, size_t len, uint32_t seed);


#endif /* HASH_H */

