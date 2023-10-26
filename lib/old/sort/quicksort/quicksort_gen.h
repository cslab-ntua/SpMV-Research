#if !defined(QUICKSORT_GEN_TYPE_1)
	#error "QUICKSORT_GEN_TYPE_1 not defined: value type"
#elif !defined(QUICKSORT_GEN_TYPE_2)
	#error "QUICKSORT_GEN_TYPE_2 not defined: index type"
#elif !defined(QUICKSORT_GEN_TYPE_3)
	#error "QUICKSORT_GEN_TYPE_3 not defined: auxiliary data value type"
#elif !defined(QUICKSORT_GEN_SUFFIX)
	#error "QUICKSORT_GEN_SUFFIX not defined"
#endif

#include "macros/cpp_defines.h"
#include "macros/macrolib.h"


#define QUICKSORT_GEN_EXPAND(name)  CONCAT(name, QUICKSORT_GEN_SUFFIX)

#undef  _TYPE_V
#define _TYPE_V  QUICKSORT_GEN_EXPAND(_TYPE_V)
typedef QUICKSORT_GEN_TYPE_1  _TYPE_V;

#undef  _TYPE_I
#define _TYPE_I  QUICKSORT_GEN_EXPAND(_TYPE_I)
typedef QUICKSORT_GEN_TYPE_2  _TYPE_I;

#undef  _TYPE_AD
#define _TYPE_AD  QUICKSORT_GEN_EXPAND(_TYPE_AD)
typedef QUICKSORT_GEN_TYPE_3  _TYPE_AD;


//==========================================================================================================================================
//= Functions
//==========================================================================================================================================


#undef  quicksort_partition_concurrent
#define quicksort_partition_concurrent  QUICKSORT_GEN_EXPAND(quicksort_partition_concurrent)
long quicksort_partition_concurrent(_TYPE_V * A, _TYPE_V * buf, long i_start, long i_end, _TYPE_AD * aux_data);

#undef  quicksort_partition
#define quicksort_partition  QUICKSORT_GEN_EXPAND(quicksort_partition)
long quicksort_partition(_TYPE_V * A, long s, long e, _TYPE_AD * aux_data);

#undef  quicksort_no_malloc
#define quicksort_no_malloc  QUICKSORT_GEN_EXPAND(quicksort_no_malloc)
void quicksort_no_malloc(_TYPE_V * A, long N, _TYPE_AD * aux_data, _TYPE_I * partitions);

#undef  quicksort
#define quicksort  QUICKSORT_GEN_EXPAND(quicksort)
void quicksort(_TYPE_V * A, long N, _TYPE_AD * aux_data);

#undef  insertionsort
#define insertionsort  QUICKSORT_GEN_EXPAND(insertionsort)
void insertionsort(_TYPE_V * A, long N, _TYPE_AD * aux_data);

