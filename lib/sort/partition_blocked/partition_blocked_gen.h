#if !defined(PARTITION_BLOCKED_GEN_TYPE_1)
	#error "PARTITION_BLOCKED_GEN_TYPE_1 not defined: value type"
#elif !defined(PARTITION_BLOCKED_GEN_TYPE_2)
	#error "PARTITION_BLOCKED_GEN_TYPE_2 not defined: index type"
#elif !defined(PARTITION_BLOCKED_GEN_TYPE_3)
	#error "PARTITION_BLOCKED_GEN_TYPE_3 not defined: auxiliary data value type"
#elif !defined(PARTITION_BLOCKED_GEN_VECTOR_LEN)
	#error "PARTITION_BLOCKED_GEN_VECTOR_LEN not defined: vector length (number of elements)"
#elif !defined(PARTITION_BLOCKED_GEN_SUFFIX)
	#error "PARTITION_BLOCKED_GEN_SUFFIX not defined"
#endif

#include "macros/cpp_defines.h"
#include "macros/macrolib.h"


#define PARTITION_BLOCKED_GEN_EXPAND(name)  CONCAT(name, PARTITION_BLOCKED_GEN_SUFFIX)
#define PARTITION_BLOCKED_GEN_EXPAND_TYPE(name)  CONCAT(PARTITION_BLOCKED_GEN_, PARTITION_BLOCKED_GEN_EXPAND(name))

#undef  _TYPE_V
#define _TYPE_V  PARTITION_BLOCKED_GEN_EXPAND_TYPE(_TYPE_V)
typedef PARTITION_BLOCKED_GEN_TYPE_1  _TYPE_V;

#undef  _TYPE_I
#define _TYPE_I  PARTITION_BLOCKED_GEN_EXPAND_TYPE(_TYPE_I)
typedef PARTITION_BLOCKED_GEN_TYPE_2  _TYPE_I;

#undef  _TYPE_AD
#define _TYPE_AD  PARTITION_BLOCKED_GEN_EXPAND_TYPE(_TYPE_AD)
typedef PARTITION_BLOCKED_GEN_TYPE_3  _TYPE_AD;


//==========================================================================================================================================
//= Functions
//==========================================================================================================================================


#undef  partition_blocked_serial
#define partition_blocked_serial  PARTITION_BLOCKED_GEN_EXPAND(partition_blocked_serial)
long partition_blocked_serial(_TYPE_V pivot, _TYPE_V * A, long i_start, long i_end, _TYPE_AD * aux_data);

#undef  partition_blocked_auto_serial
#define partition_blocked_auto_serial  PARTITION_BLOCKED_GEN_EXPAND(partition_blocked_auto_serial)
long partition_blocked_auto_serial(_TYPE_V * A, long i_start, long i_end, _TYPE_AD * aux_data);

