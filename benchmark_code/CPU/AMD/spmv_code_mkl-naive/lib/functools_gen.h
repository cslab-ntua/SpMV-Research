#if !defined(FUNCTOOLS_GEN_TYPE_1)
	#error "FUNCTOOLS_GEN_TYPE_1 not defined: value type"
#elif !defined(FUNCTOOLS_GEN_SUFFIX)
	#error "FUNCTOOLS_GEN_SUFFIX not defined"
#endif

#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

#include "parallel_util.h"
// #include "genlib.h"


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


#undef reduce
#define reduce  FUNCTOOLS_GEN_EXPAND(reduce)
_TYPE reduce(_TYPE * A, long N, _TYPE zero);

#undef scan_serial
#define scan_serial  FUNCTOOLS_GEN_EXPAND(scan_serial)
_TYPE scan_serial(_TYPE * A, _TYPE * P, long N, _TYPE zero, int start_from_zero);

#undef scan
#define scan  FUNCTOOLS_GEN_EXPAND(scan)
_TYPE scan(_TYPE * A, _TYPE * P, long N, _TYPE zero, int start_from_zero);

