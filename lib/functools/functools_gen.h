#if !defined(FUNCTOOLS_GEN_TYPE_1)
	#error "FUNCTOOLS_GEN_TYPE_1 not defined: input values type"
#elif !defined(FUNCTOOLS_GEN_TYPE_2)
	#error "FUNCTOOLS_GEN_TYPE_2 not defined: type of the result of the map() function"
#elif !defined(FUNCTOOLS_GEN_SUFFIX)
	#error "FUNCTOOLS_GEN_SUFFIX not defined"
#endif

#include "macros/cpp_defines.h"
#include "macros/macrolib.h"


#define FUNCTOOLS_GEN_EXPAND(name)  CONCAT(name, FUNCTOOLS_GEN_SUFFIX)

#undef  _TYPE_IN
#define _TYPE_IN  FUNCTOOLS_GEN_EXPAND(_TYPE_IN)
typedef FUNCTOOLS_GEN_TYPE_1  _TYPE_IN;

#undef  _TYPE_OUT
#define _TYPE_OUT  FUNCTOOLS_GEN_EXPAND(_TYPE_OUT)
typedef FUNCTOOLS_GEN_TYPE_2  _TYPE_OUT;


//==========================================================================================================================================
//= Functions
//==========================================================================================================================================


//------------------------------------------------------------------------------------------------------------------------------------------
//- Reduce
//------------------------------------------------------------------------------------------------------------------------------------------


#undef  reduce_segment_serial
#define reduce_segment_serial  FUNCTOOLS_GEN_EXPAND(reduce_segment_serial)
_TYPE_OUT reduce_segment_serial(_TYPE_IN * A, long i_start, long i_end, _TYPE_OUT zero, const int backwards);

#undef  reduce_segment_concurrent
#define reduce_segment_concurrent  FUNCTOOLS_GEN_EXPAND(reduce_segment_concurrent)
_TYPE_OUT reduce_segment_concurrent(_TYPE_IN * A, long i_start, long i_end, _TYPE_OUT zero, const int backwards);

#undef  reduce_segment
#define reduce_segment  FUNCTOOLS_GEN_EXPAND(reduce_segment)
_TYPE_OUT reduce_segment(_TYPE_IN * A, long i_start, long i_end, _TYPE_OUT zero, const int backwards);


#undef  reduce_serial
#define reduce_serial  FUNCTOOLS_GEN_EXPAND(reduce_serial)
_TYPE_OUT reduce_serial(_TYPE_IN * A, long N, _TYPE_OUT zero, const int backwards);

#undef  reduce_concurrent
#define reduce_concurrent  FUNCTOOLS_GEN_EXPAND(reduce_concurrent)
_TYPE_OUT reduce_concurrent(_TYPE_IN * A, long N, _TYPE_OUT zero, const int backwards);

#undef  reduce
#define reduce  FUNCTOOLS_GEN_EXPAND(reduce)
_TYPE_OUT reduce(_TYPE_IN * A, long N, _TYPE_OUT zero, const int backwards);


//------------------------------------------------------------------------------------------------------------------------------------------
//- Scan Reduce
//------------------------------------------------------------------------------------------------------------------------------------------


/* A = P is valid.
 *
 * exclusive:  First element of P is zero, else it is the first element of A (i.e. the i-th input element is included in the i-th sum).
 */


#undef  scan_reduce_segment_serial
#define scan_reduce_segment_serial  FUNCTOOLS_GEN_EXPAND(scan_reduce_segment_serial)
_TYPE_OUT scan_reduce_segment_serial(_TYPE_IN * A, _TYPE_OUT * P, long i_s, long i_e, _TYPE_OUT zero, const int exclusive, const int backwards);

#undef  scan_reduce_segment_concurrent
#define scan_reduce_segment_concurrent  FUNCTOOLS_GEN_EXPAND(scan_reduce_segment_concurrent)
_TYPE_OUT scan_reduce_segment_concurrent(_TYPE_IN * A, _TYPE_OUT * P, long i_start, long i_end, _TYPE_OUT zero, const int exclusive, const int backwards);

#undef  scan_reduce_segment
#define scan_reduce_segment  FUNCTOOLS_GEN_EXPAND(scan_reduce_segment)
_TYPE_OUT scan_reduce_segment(_TYPE_IN * A, _TYPE_OUT * P, long i_start, long i_end, _TYPE_OUT zero, const int exclusive, const int backwards);


#undef  scan_reduce_serial
#define scan_reduce_serial  FUNCTOOLS_GEN_EXPAND(scan_reduce_serial)
_TYPE_OUT scan_reduce_serial(_TYPE_IN * A, _TYPE_OUT * P, long N, _TYPE_OUT zero, const int exclusive, const int backwards);

#undef  scan_reduce_concurrent
#define scan_reduce_concurrent  FUNCTOOLS_GEN_EXPAND(scan_reduce_concurrent)
_TYPE_OUT scan_reduce_concurrent(_TYPE_IN * A, _TYPE_OUT * P, long N, _TYPE_OUT zero, const int exclusive, const int backwards);

#undef  scan_reduce
#define scan_reduce  FUNCTOOLS_GEN_EXPAND(scan_reduce)
_TYPE_OUT scan_reduce(_TYPE_IN * A, _TYPE_OUT * P, long N, _TYPE_OUT zero, const int exclusive, const int backwards);

