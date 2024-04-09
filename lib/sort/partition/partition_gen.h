#if !defined(PARTITION_GEN_TYPE_1)
	#error "PARTITION_GEN_TYPE_1 not defined: value type"
#elif !defined(PARTITION_GEN_TYPE_2)
	#error "PARTITION_GEN_TYPE_2 not defined: index type"
#elif !defined(PARTITION_GEN_TYPE_3)
	#error "PARTITION_GEN_TYPE_3 not defined: auxiliary data value type"
#elif !defined(PARTITION_GEN_SUFFIX)
	#error "PARTITION_GEN_SUFFIX not defined"
#endif

#include "macros/cpp_defines.h"
#include "macros/macrolib.h"


#define PARTITION_GEN_EXPAND(name)  CONCAT(name, PARTITION_GEN_SUFFIX)
#define PARTITION_GEN_EXPAND_TYPE(name)  CONCAT(PARTITION_GEN_, PARTITION_GEN_EXPAND(name))

#undef  _TYPE_V
#define _TYPE_V  PARTITION_GEN_EXPAND_TYPE(_TYPE_V)
typedef PARTITION_GEN_TYPE_1  _TYPE_V;

#undef  _TYPE_I
#define _TYPE_I  PARTITION_GEN_EXPAND_TYPE(_TYPE_I)
typedef PARTITION_GEN_TYPE_2  _TYPE_I;

#undef  _TYPE_AD
#define _TYPE_AD  PARTITION_GEN_EXPAND_TYPE(_TYPE_AD)
typedef PARTITION_GEN_TYPE_3  _TYPE_AD;


//==========================================================================================================================================
//= Functions
//==========================================================================================================================================


#undef  partition_serial
#define partition_serial  PARTITION_GEN_EXPAND(partition_serial)
long partition_serial(_TYPE_V pivot, _TYPE_V * A, long i_start, long i_end, _TYPE_AD * aux_data);

#undef  partition_auto_serial
#define partition_auto_serial  PARTITION_GEN_EXPAND(partition_auto_serial)
long partition_auto_serial(_TYPE_V * A, long i_start, long i_end, _TYPE_AD * aux_data);


#undef  partition_concurrent
#define partition_concurrent  PARTITION_GEN_EXPAND(partition_concurrent)
long partition_concurrent(_TYPE_V pivot, _TYPE_V * A, long i_start, long i_end, _TYPE_AD * aux_data, const int inplace, _TYPE_V * buf);

#undef  partition_auto_concurrent
#define partition_auto_concurrent  PARTITION_GEN_EXPAND(partition_auto_concurrent)
long partition_auto_concurrent(_TYPE_V * A, long i_start, long i_end, _TYPE_AD * aux_data, const int inplace, _TYPE_V * buf);

