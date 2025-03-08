#ifndef LOCK_COMMON_H
#define LOCK_COMMON_H

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
void lock_cpu_relax()
{
	#ifdef __x86_64__
		// __asm volatile ("pause" : : : "memory");
		__asm volatile ("rep; pause" : : : "memory");   // relax
		// __asm volatile ("rep; nop" : : : "memory");
	#else
		for (volatile int i = 0; i < 1000; ++i); // Adjust the loop count as needed
	#endif

}


#endif /* LOCK_COMMON_H */

