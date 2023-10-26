#ifndef TIME_IT_TSC_H
#define TIME_IT_TSC_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>

#include "macros/cpp_defines.h"


/*
 * Find out tsc frequency with dmesg:
 * 
 * dmesg | grep tsc
 *     [    0.004000] tsc: Detected 2672.726 MHz processor
 *     [    2.112070] tsc: Refined TSC clocksource calibration: 2672.592 MHz
 *     [    2.112083] clocksource: tsc: mask: 0xffffffffffffffff max_cycles: 0x26861b6054f, max_idle_ns: 440795311586 ns
 * 
 * 
 * 
 * Cycles to nanoseconds
 *     source clock frequency = SCF (Hz)
 *     
 *     
 *     sec = cycles / SCF
 *     ns = (cycles / SCF) * 1000000000
 *     
 *     ns = cycles * to_ns_scale / SCF
 *     
 *     Although we may still have enough bits to store the value of ns,
 *     in some cases, we may not have enough bits to store cycles * to_ns_scale,
 *     leading to an incorrect result.
 *     
 *     To avoid this, we can decompose 'cycles' into quotient and remainder
 *     of division by SCF.  Then:
 *     
 *     ns = (quot * SCF + rem) * to_ns_scale / SCF
 *        = quot * to_ns_scale + (rem * to_ns_scale) / SCF 
 */

/*
 * Volatile
 *     GCC’s optimizers sometimes discard asm statements if they determine there is no need for the output variables.
 *     Also, the optimizers may move code out of loops if they believe that the code will always return the same result
 *     (i.e. none of its input values change between calls).
 *     Using the volatile qualifier disables these optimizations.
 *     
 *     'asm' statements that have < no output operands >, including asm goto statements, are
 *     < implicitly volatile >.
 */


// static __inline__
// uint64_t rdtscp(void)
// {
// 	uint32_t lo, hi;
// 	asm volatile (
// 		"rdtscp"
// 		: "=a" (lo), "=d" (hi)    /* outputs */ 
// 		:                         /* inputs */
// 		: "%rcx"                  /* clobbers */ 
// 	);
// 	return (uint64_t)lo | (((uint64_t)hi) << 32);
// }


static __inline__
uint64_t
time_it_tsc_start()
{
	unsigned int cycles_low, cycles_high;
	asm volatile (
		"lfence               \n\t"                    /* alternative to cpuid, as used in kernel */
		// "cpuid                \n\t"                    /* serialize */
		"rdtsc                \n\t"                    /* read clock */
		"mov %%edx, %0        \n\t"
		"mov %%eax, %1        \n\t"
		: "=r" (cycles_high), "=r" (cycles_low)        /* outputs */ 
		:                                              /* inputs */
		: "%rax", "%rbx", "%rcx", "%rdx"               /* clobbers */ 
	);
	return ((uint64_t) cycles_high << 32) | cycles_low;
}


static __inline__
uint64_t
time_it_tsc_end()
{
	unsigned int cycles_low, cycles_high;
	asm volatile (
		"rdtscp               \n\t"                    /* read clock + serialize from above only */
		"mov %%edx, %0        \n\t"
		"mov %%eax, %1        \n\t"
		// "cpuid                \n\t"                    /* serialze from bellow too -- but outside clock region! */
		"lfence               \n\t"                    /* alternative to cpuid, as used in kernel */
		: "=r" (cycles_high), "=r" (cycles_low)
		:
		: "%rax", "%rbx", "%rcx", "%rdx"
	);
	return ((uint64_t) cycles_high << 32) | cycles_low;
}


static __attribute__((unused))
uint64_t
time_it_tsc_overhead()
{
	uint64_t t1, t2, overhead = ~0;
	int i;
	int repeats = 100000;
	
	for (i=0;i<repeats;i++)
	{
		t1 = time_it_tsc_start();
		asm volatile("");
		t2 = time_it_tsc_end();
		// printf("%lu\n", t2 - t1);
		if (t2 - t1 < overhead)
			overhead = t2 - t1;
	}
	return overhead;
}


#define start_timer_tsc(id)                 \
	uint64_t __##id##c1, __##id##c2;    \
	__##id##c1 = time_it_tsc_start();

#define stop_timer_tsc(id)                 \
({                                         \
	__##id##c2 = time_it_tsc_end();    \
	__##id##c2 - __##id##c1;           \
})


#define time_it_tsc(times, ...)          \
({                                       \
	__auto_type __times = times;     \
	typeof(__times) __i;             \
                                         \
	start_timer_tsc(time_it_tsc);    \
	for (__i=0;__i<__times;__i++)    \
	{                                \
		__VA_ARGS__              \
	}                                \
	stop_timer_tsc(time_it_tsc);     \
})


#define time_it_tsc_sec(cpu_khz, times, ...)                                                               \
({                                                                                                         \
	__auto_type __cpu_khz = cpu_khz;                                                                   \
	__auto_type __times = times;                                                                       \
	typeof(__times) __i;                                                                               \
	uint64_t __cycles;                                                                                 \
	uint64_t __nsec;                                                                                   \
                                                                                                           \
	start_timer_tsc(time_it_tsc);                                                                      \
	for (__i=0;__i<__times;__i++)                                                                      \
	{                                                                                                  \
		__VA_ARGS__                                                                                \
	}                                                                                                  \
	__cycles = stop_timer_tsc(time_it_tsc);                                                            \
	__nsec = (__cycles / __cpu_khz) * 1000000LL + ((__cycles % __cpu_khz) * 1000000LL) / __cpu_khz;    \
	((double) __nsec) / 1000000000LL;                                                                  \
})


#define tsc_cycles_2_nsec(cycles, cpu_khz)                                                          \
({                                                                                                  \
	uint64_t __cycles = cycles;                                                                 \
	__auto_type __cpu_khz = cpu_khz;                                                            \
	((__cycles / __cpu_khz) * 1000000LL + ((__cycles % __cpu_khz) * 1000000LL) / __cpu_khz);    \
})

#define tsc_nsec_2_cycles(nsec, cpu_khz)  (((nsec) * (cpu_khz) * 1000LL) / 1000000000LL)


#define tsc_cycles_2_sec(cycles, cpu_khz) (((double) tsc_cycles_2_nsec(cycles, cpu_khz)) / 1000000000LL)

#define tsc_sec_2_cycles(sec, cpu_khz) (tsc_nsec_2_cycles(sec * 1000000000LL,  cpu_khz))


#endif /* TIME_IT_TSC_H */

