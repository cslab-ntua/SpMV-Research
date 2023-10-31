#if !defined(BUCKETSORT_GEN_TYPE_1)
	#error "BUCKETSORT_GEN_TYPE_1 not defined: value type"
#elif !defined(BUCKETSORT_GEN_TYPE_2)
	#error "BUCKETSORT_GEN_TYPE_2 not defined: index type"
#elif !defined(BUCKETSORT_GEN_TYPE_3)
	#error "BUCKETSORT_GEN_TYPE_3 not defined: bucket index type"
#elif !defined(BUCKETSORT_GEN_TYPE_4)
	#error "BUCKETSORT_GEN_TYPE_4 not defined: auxiliary data value type"
#elif !defined(BUCKETSORT_GEN_SUFFIX)
	#error "BUCKETSORT_GEN_SUFFIX not defined"
#endif

#include "macros/cpp_defines.h"
#include "macros/macrolib.h"


#define BUCKETSORT_GEN_EXPAND(name)  CONCAT(name, BUCKETSORT_GEN_SUFFIX)

#undef  _TYPE_V
#define _TYPE_V  BUCKETSORT_GEN_EXPAND(_TYPE_V)
typedef BUCKETSORT_GEN_TYPE_1  _TYPE_V;

#undef  _TYPE_I
#define _TYPE_I  BUCKETSORT_GEN_EXPAND(_TYPE_I)
typedef BUCKETSORT_GEN_TYPE_2  _TYPE_I;

#undef  _TYPE_BUCKET_I
#define _TYPE_BUCKET_I  BUCKETSORT_GEN_EXPAND(_TYPE_BUCKET_I)
typedef BUCKETSORT_GEN_TYPE_3  _TYPE_BUCKET_I;

#undef  _TYPE_AD
#define _TYPE_AD  BUCKETSORT_GEN_EXPAND(_TYPE_AD)
typedef BUCKETSORT_GEN_TYPE_4  _TYPE_AD;


//==========================================================================================================================================
//= Functions
//==========================================================================================================================================


#undef  bucketsort_stable_serial
#define bucketsort_stable_serial  BUCKETSORT_GEN_EXPAND(bucketsort_stable_serial)
void bucketsort_stable_serial(_TYPE_V * A, long N, _TYPE_BUCKET_I num_buckets, _TYPE_AD * aux_data, _TYPE_I * permutation_out, _TYPE_I * offsets_out, _TYPE_BUCKET_I * A_bucket_id_out);

#undef  bucketsort
#define bucketsort  BUCKETSORT_GEN_EXPAND(bucketsort)
void bucketsort(_TYPE_V * A, long N, _TYPE_BUCKET_I num_buckets, _TYPE_AD * aux_data, _TYPE_I * permutation_out, _TYPE_I * offsets_out, _TYPE_BUCKET_I * A_bucket_id_out);

