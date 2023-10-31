#if !defined(SAMPLESORT_GEN_TYPE_1)
	#error "SAMPLESORT_GEN_TYPE_1 not defined: value type"
#elif !defined(SAMPLESORT_GEN_TYPE_2)
	#error "SAMPLESORT_GEN_TYPE_2 not defined: index type"
#elif !defined(SAMPLESORT_GEN_TYPE_3)
	#error "SAMPLESORT_GEN_TYPE_3 not defined: bucket index type"
#elif !defined(SAMPLESORT_GEN_TYPE_4)
	#error "SAMPLESORT_GEN_TYPE_4 not defined: auxiliary data value type"
#elif !defined(SAMPLESORT_GEN_SUFFIX)
	#error "SAMPLESORT_GEN_SUFFIX not defined"
#endif

#include "macros/cpp_defines.h"
#include "macros/macrolib.h"


#define SAMPLESORT_GEN_EXPAND(name)  CONCAT(name, SAMPLESORT_GEN_SUFFIX)

#undef  _TYPE_V
#define _TYPE_V  SAMPLESORT_GEN_EXPAND(_TYPE_V)
typedef SAMPLESORT_GEN_TYPE_1  _TYPE_V;

#undef  _TYPE_I
#define _TYPE_I  SAMPLESORT_GEN_EXPAND(_TYPE_I)
typedef SAMPLESORT_GEN_TYPE_2  _TYPE_I;

#undef  _TYPE_BUCKET_I
#define _TYPE_BUCKET_I  SAMPLESORT_GEN_EXPAND(_TYPE_BUCKET_I)
typedef SAMPLESORT_GEN_TYPE_3  _TYPE_BUCKET_I;

#undef  _TYPE_AD
#define _TYPE_AD  SAMPLESORT_GEN_EXPAND(_TYPE_AD)
typedef SAMPLESORT_GEN_TYPE_4  _TYPE_AD;


//==========================================================================================================================================
//= Functions
//==========================================================================================================================================


#undef  samplesort_concurrent
#define samplesort_concurrent  SAMPLESORT_GEN_EXPAND(samplesort_concurrent)
void samplesort_concurrent(_TYPE_V * A, long N, _TYPE_AD * aux_data);

#undef  samplesort
#define samplesort  SAMPLESORT_GEN_EXPAND(samplesort)
void samplesort(_TYPE_V * A, long N, _TYPE_AD * aux_data);

