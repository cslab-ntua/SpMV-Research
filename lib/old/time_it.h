#ifndef TIME_IT_H
#define TIME_IT_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>

#include "macros/cpp_defines.h"


static __attribute__ ((unused))
uint64_t
time_it_clock_gettime_overhead()
{
	struct timespec t1, t2;
	uint64_t tmp, overhead = ~0;
	int i;
	int repeat = 100000;
	
	for (i=0;i<repeat;i++)
	{
		
		clock_gettime(CLOCK_MONOTONIC, &t1);
		asm volatile("");
		clock_gettime(CLOCK_MONOTONIC, &t2);
		tmp = (t2.tv_sec-t1.tv_sec) * 1000000000LL + t2.tv_nsec - t1.tv_nsec;
		
		if (tmp < overhead)
			overhead = tmp;
	}
	return overhead;
}

#define start_timer(id)                                                          \
	struct timespec __##id##t1, __##id##t2;                                  \
	do clock_gettime(CLOCK_MONOTONIC, &__##id##t1); while (0)

#define stop_timer(id)                                                                                                                 \
({                                                                                                                                     \
	clock_gettime(CLOCK_MONOTONIC, &__##id##t2);                                                                                   \
	((double) ((__##id##t2.tv_sec-__##id##t1.tv_sec) * 1000000000LL + __##id##t2.tv_nsec - __##id##t1.tv_nsec)) / 1000000000LL;    \
})


#define time_it(times, ...)                            \
({                                                     \
	__auto_type __times = times;                   \
	typeof(__times) __i;                           \
	                                               \
	start_timer(time_it);                          \
	for (__i=0;__i<__times;__i++)                  \
	{                                              \
		__VA_ARGS__                            \
	}                                              \
	stop_timer(time_it);                           \
})


#endif /* TIME_IT_H */

