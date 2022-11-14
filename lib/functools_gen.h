#if !defined(FUNCTOOLS_GEN_TYPE_1)
	#error "FUNCTOOLS_GEN_TYPE_1 not defined: value type"
#elif !defined(FUNCTOOLS_GEN_SUFFIX)
	#error "FUNCTOOLS_GEN_SUFFIX not defined"
#endif

#include "macros/cpp_defines.h"
#include "macros/macrolib.h"


#ifndef FUNCTOOLS_GEN_H
#define FUNCTOOLS_GEN_H

#endif /* FUNCTOOLS_GEN_H */


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//------------------------------------------------------------------------------------------------------------------------------------------
//-                                                              Templates                                                                 -
//------------------------------------------------------------------------------------------------------------------------------------------
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


#define FUNCTOOLS_GEN_EXPAND(name)  CONCAT(name, FUNCTOOLS_GEN_SUFFIX)

#undef  _TYPE
#define _TYPE  FUNCTOOLS_GEN_EXPAND(_TYPE)
typedef FUNCTOOLS_GEN_TYPE_1  _TYPE;


#undef  reduce_serial
#define reduce_serial  FUNCTOOLS_GEN_EXPAND(reduce_serial)
_TYPE reduce_serial(_TYPE * restrict A, long N, _TYPE zero, const int backwards);

#undef  reduce
#define reduce  FUNCTOOLS_GEN_EXPAND(reduce)
_TYPE reduce(_TYPE * A, long N, _TYPE zero);

#undef  scan_reduce_serial
#define scan_reduce_serial  FUNCTOOLS_GEN_EXPAND(scan_reduce_serial)
_TYPE scan_reduce_serial(_TYPE * A, _TYPE * P, long N, _TYPE zero, const int start_from_zero, const int backwards);

#undef  scan_reduce
#define scan_reduce  FUNCTOOLS_GEN_EXPAND(scan_reduce)
_TYPE scan_reduce(_TYPE * A, _TYPE * P, long N, _TYPE zero, int start_from_zero);

