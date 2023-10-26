#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <unistd.h>

#include "macros/cpp_defines.h"


#ifndef NANO
	#define NANO 1000000000LL
#endif


//==========================================================================================================================================
// System
//==========================================================================================================================================

/*
 * long sysconf(int name);
 *     get configuration information at run time
 *
 * These values also exist, but may not be standard.
 *     _SC_PHYS_PAGES
 *         The number of pages of physical memory. Note that it is possible for the product of 
 *         this value and the value of _SC_PAGESIZE to overflow.
 *     _SC_AVPHYS_PAGES
 *         The number of currently available pages of physical memory.
 *     _SC_NPROCESSORS_CONF
 *         The number of processors configured (it includes hyperthreading).
 *     _SC_NPROCESSORS_ONLN
 *         The number of processors currently online (available).
 *
 *     _SC_LEVEL1_ICACHE_SIZE,
 *     _SC_LEVEL1_ICACHE_ASSOC,
 *     _SC_LEVEL1_ICACHE_LINESIZE,
 *     _SC_LEVEL1_DCACHE_SIZE,
 *     _SC_LEVEL1_DCACHE_ASSOC,
 *     _SC_LEVEL1_DCACHE_LINESIZE,
 *     _SC_LEVEL2_CACHE_SIZE,
 *     _SC_LEVEL2_CACHE_ASSOC,
 *     _SC_LEVEL2_CACHE_LINESIZE,
 *     _SC_LEVEL3_CACHE_SIZE,
 *     _SC_LEVEL3_CACHE_ASSOC,
 *     _SC_LEVEL3_CACHE_LINESIZE,
 *     _SC_LEVEL4_CACHE_SIZE,
 *     _SC_LEVEL4_CACHE_ASSOC,
 *     _SC_LEVEL4_CACHE_LINESIZE,
 *
 * See all system configuration with the command: "getconf -a".
 * See all sysconf arguments in '/usr/include/bits/confname.h'.
 */

#define GET_VIRTUAL_CORES()  (sysconf(_SC_NPROCESSORS_CONF))
#define GET_DL1_CACHE_SIZE()  (sysconf(_SC_LEVEL1_DCACHE_SIZE))
#define GET_PAGE_SIZE()  (sysconf(_SC_PAGESIZE))

// We can't use sysconf() for structs because it is calculated at runtime.
// #define CACHE_LINE_SIZE  sysconf(_SC_LEVEL1_DCACHE_LINESIZE)
#ifndef CACHE_LINE_SIZE
	#define CACHE_LINE_SIZE  64
#endif


#endif /* CONSTANTS_H */

