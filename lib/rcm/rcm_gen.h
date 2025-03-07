#if !defined(RCM_GEN_TYPE_1)
	#error "RCM_GEN_TYPE_1 not defined: value type"
#elif !defined(RCM_GEN_TYPE_2)
	#error "RCM_GEN_TYPE_2 not defined: index type"
#elif !defined(RCM_GEN_SUFFIX)
	#error "RCM_GEN_SUFFIX not defined"
#endif

#include "macros/cpp_defines.h"
#include "macros/macrolib.h"


#define RCM_GEN_EXPAND(name)  CONCAT(name, RCM_GEN_SUFFIX)
#define RCM_GEN_EXPAND_TYPE(name)  CONCAT(RCM_GEN_, RCM_GEN_EXPAND(name))

#undef  _TYPE_V
#define _TYPE_V  RCM_GEN_EXPAND_TYPE(_TYPE_V)
typedef RCM_GEN_TYPE_1  _TYPE_V;

#undef  _TYPE_I
#define _TYPE_I  RCM_GEN_EXPAND_TYPE(_TYPE_I)
typedef RCM_GEN_TYPE_2  _TYPE_I;


//==========================================================================================================================================
//= Functions
//==========================================================================================================================================


#undef  reverse_cuthill_mckee
#define reverse_cuthill_mckee  RCM_GEN_EXPAND(reverse_cuthill_mckee)
void reverse_cuthill_mckee(_TYPE_I * row_ptr, _TYPE_I * col_idx, _TYPE_V * values, long m, long n, long nnz,
	_TYPE_I ** reordered_row_ptr_ret, _TYPE_I ** reordered_col_idx_ret, _TYPE_V ** reordered_values_ret, _TYPE_I ** permutation_ret);

