#if !defined(QUICKSORT_BLOCKED_GEN_TYPE_1)
	#error "QUICKSORT_BLOCKED_GEN_TYPE_1 not defined: value type"
#elif !defined(QUICKSORT_BLOCKED_GEN_TYPE_2)
	#error "QUICKSORT_BLOCKED_GEN_TYPE_2 not defined: index type"
#elif !defined(QUICKSORT_BLOCKED_GEN_TYPE_3)
	#error "QUICKSORT_BLOCKED_GEN_TYPE_3 not defined: auxiliary data value type"
#elif !defined(QUICKSORT_BLOCKED_GEN_VECTOR_LEN)
	#error "QUICKSORT_BLOCKED_GEN_VECTOR_LEN not defined: vector length (number of elements)"
#elif !defined(QUICKSORT_BLOCKED_GEN_SUFFIX)
	#error "QUICKSORT_BLOCKED_GEN_SUFFIX not defined"
#endif

#include "macros/cpp_defines.h"
#include "macros/macrolib.h"


#define QUICKSORT_BLOCKED_GEN_EXPAND(name)  CONCAT(name, QUICKSORT_BLOCKED_GEN_SUFFIX)
#define QUICKSORT_BLOCKED_GEN_EXPAND_TYPE(name)  CONCAT(QUICKSORT_BLOCKED_GEN_, QUICKSORT_BLOCKED_GEN_EXPAND(name))

#undef  _TYPE_V
#define _TYPE_V  QUICKSORT_BLOCKED_GEN_EXPAND_TYPE(_TYPE_V)
typedef QUICKSORT_BLOCKED_GEN_TYPE_1  _TYPE_V;

#undef  _TYPE_I
#define _TYPE_I  QUICKSORT_BLOCKED_GEN_EXPAND_TYPE(_TYPE_I)
typedef QUICKSORT_BLOCKED_GEN_TYPE_2  _TYPE_I;

#undef  _TYPE_AD
#define _TYPE_AD  QUICKSORT_BLOCKED_GEN_EXPAND_TYPE(_TYPE_AD)
typedef QUICKSORT_BLOCKED_GEN_TYPE_3  _TYPE_AD;


//==========================================================================================================================================
//= Functions
//==========================================================================================================================================


//------------------------------------------------------------------------------------------------------------------------------------------
//- Quicksort
//------------------------------------------------------------------------------------------------------------------------------------------

#undef  quicksort_blocked
#define quicksort_blocked  QUICKSORT_BLOCKED_GEN_EXPAND(quicksort_blocked)
void quicksort_blocked(_TYPE_V * A, long N, _TYPE_AD * aux_data, _TYPE_I * partitions_buf);

