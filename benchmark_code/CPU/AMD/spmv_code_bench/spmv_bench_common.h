#ifndef SPMV_BENCH_COMMON_H
#define SPMV_BENCH_COMMON_H

#include <math.h>

#include "macros/cpp_defines.h"


#ifndef INT_T
	#define INT_T  int32_t
#endif

#ifndef ValueType
	#define ValueType  double
#endif


// #define VECTOR_SIZE  16
#define VECTOR_SIZE  32
// #define VECTOR_SIZE  64

#ifndef __XLC__

	typedef ValueType  Vector2_Value_t  __attribute__((vector_size(16), aligned(1)));
	typedef ValueType  Vector4_Value_t  __attribute__((vector_size(32), aligned(1)));
	// typedef ValueType  Vector4_Value_t  __attribute__((vector_size(32)));
	typedef ValueType  Vector8_Value_t  __attribute__((vector_size(64), aligned(1)));

	typedef ValueType  Vector_Value_t  __attribute__((vector_size(VECTOR_SIZE), aligned(1)));

	// Number of elements for the vectorization function.
	#define VECTOR_ELEM_NUM  ((int) (sizeof(Vector_Value_t) / sizeof(ValueType)))

	// typedef INT_T  Vector_Index_t  __attribute__((vector_size(VECTOR_ELEM_NUM*sizeof(INT_T)), aligned(1)));
	typedef INT_T  Vector_Index_t  __attribute__((vector_size(VECTOR_ELEM_NUM*sizeof(INT_T))));

#else

	#define VECTOR_ELEM_NUM  4

#endif


static inline
double
val_to_double(void * A, long i)
{
	return (double) ((ValueType *) A)[i];
}


#endif /* SPMV_BENCH_COMMON_H */

