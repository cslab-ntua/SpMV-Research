#ifndef SPMV_BENCH_COMMON_H
#define SPMV_BENCH_COMMON_H

#include <stdint.h>
#include <math.h>

#include "macros/cpp_defines.h"


#ifndef INT_T
	#define INT_T  int32_t
#endif

#ifndef ValueType
	#define ValueType  double
#endif


static inline
double
val_to_double(void * A, long i)
{
	return (double) ((ValueType *) A)[i];
}


static inline
double
coord_to_double(void * A, long i)
{
	return (double) ((INT_T *) A)[i];
}


#endif /* SPMV_BENCH_COMMON_H */

