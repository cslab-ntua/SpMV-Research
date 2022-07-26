#ifndef COMMON_H
#define COMMON_H

#include "macros/cpp_defines.h"


#ifndef INT_T
	#define INT_T  int32_t
#endif

#if DOUBLE == 0
	#define ValueType  float
#elif DOUBLE == 1
	#define ValueType  double
#endif


typedef ValueType  Vector2_Value_t  __attribute__((vector_size(16), aligned(1)));

typedef ValueType  Vector4_Value_t  __attribute__((vector_size(32), aligned(1)));
// typedef ValueType  Vector4_Value_t  __attribute__((vector_size(32)));

// Number of elements for the vectorization function.
#define VECTOR_ELEM_NUM  ((int) (sizeof(Vector4_Value_t) / sizeof(ValueType)))

// typedef INT_T  Vector_Index_t  __attribute__((vector_size(VECTOR_ELEM_NUM*sizeof(INT_T)), aligned(1)));
typedef INT_T  Vector_Index_t  __attribute__((vector_size(VECTOR_ELEM_NUM*sizeof(INT_T))));


#endif /* COMMON_H */

