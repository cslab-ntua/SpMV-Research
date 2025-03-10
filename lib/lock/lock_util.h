#ifndef LOCK_UTIL_H
#define LOCK_UTIL_H

#include "macros/cpp_defines.h"


enum lock_state_t {
	LOCK_LOCKED = 1,
	LOCK_UNLOCKED = 0
};


struct lock_padded_int {
	int32_t val __attribute__((aligned(64)));
	char padding[0] __attribute__((aligned(64)));
} __attribute__((aligned(64)));


struct lock_padded_ptr {
	struct lock_padded_int * ptr __attribute__((aligned(64)));
	char padding[0] __attribute__((aligned(64)));
} __attribute__((aligned(64)));


static inline
void
lock_cpu_relax()
{
	#if defined(__x86_64__)
		asm volatile("rep; nop" : : : "memory");
	#elif defined(__aarch64__)
		asm volatile("yield" : : : "memory");
	#endif
}


#endif /* LOCK_UTIL_H */

