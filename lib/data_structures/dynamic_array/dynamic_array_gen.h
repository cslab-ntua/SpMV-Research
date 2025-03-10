#if !defined(DYNAMIC_ARRAY_GEN_TYPE_1)
	#error "DYNAMIC_ARRAY_GEN_TYPE_1 not defined: data type"
#elif !defined(DYNAMIC_ARRAY_GEN_SUFFIX)
	#error "DYNAMIC_ARRAY_GEN_SUFFIX not defined"
#endif

#ifndef _GNU_SOURCE
	#error "please define _GNU_SOURCE at the top level"
#endif
#include "macros/cpp_defines.h"
#include "macros/macrolib.h"
#include "macros/constants.h"


#define DYNAMIC_ARRAY_GEN_EXPAND(name)  CONCAT(name, DYNAMIC_ARRAY_GEN_SUFFIX)
#define DYNAMIC_ARRAY_GEN_EXPAND_TYPE(name)  CONCAT(DYNAMIC_ARRAY_GEN_, DYNAMIC_ARRAY_GEN_EXPAND(name))

#undef  _TYPE
#define _TYPE  DYNAMIC_ARRAY_GEN_EXPAND_TYPE(_TYPE)
typedef DYNAMIC_ARRAY_GEN_TYPE_1  _TYPE;


//==========================================================================================================================================
//= Structs
//==========================================================================================================================================


#undef  dynarray
#define dynarray  DYNAMIC_ARRAY_GEN_EXPAND(dynarray)
struct dynarray {
	long page_size;

	long capacity;  // max number of BYTES
	long max_size;  // max number of items
	_TYPE * data;
	long size __attribute__ ((aligned(CACHE_LINE_SIZE)));      // current number of items

	long semaphore; // transaction completion semaphore

	char padding[0] __attribute__ ((aligned(CACHE_LINE_SIZE)));
};


//==========================================================================================================================================
//= Functions
//==========================================================================================================================================


//------------------------------------------------------------------------------------------------------------------------------------------
//- Constructors - Destructors
//------------------------------------------------------------------------------------------------------------------------------------------

#undef  dynarray_init
#define dynarray_init  DYNAMIC_ARRAY_GEN_EXPAND(dynarray_init)
void dynarray_init(struct dynarray * da, long new_capacity);

#undef  dynarray_new
#define dynarray_new  DYNAMIC_ARRAY_GEN_EXPAND(dynarray_new)
struct dynarray * dynarray_new(long new_capacity);

#undef  dynarray_clean
#define dynarray_clean  DYNAMIC_ARRAY_GEN_EXPAND(dynarray_clean)
void dynarray_clean(struct dynarray * da);

#undef  dynarray_destroy
#define dynarray_destroy  DYNAMIC_ARRAY_GEN_EXPAND(dynarray_destroy)
void dynarray_destroy(struct dynarray ** v_ptr);

#undef  dynarray_resize
#define dynarray_resize  DYNAMIC_ARRAY_GEN_EXPAND(dynarray_resize)
void dynarray_resize(struct dynarray * da, long new_capacity);


//------------------------------------------------------------------------------------------------------------------------------------------
//- Functionality
//------------------------------------------------------------------------------------------------------------------------------------------

#undef  dynarray_clear
#define dynarray_clear  DYNAMIC_ARRAY_GEN_EXPAND(dynarray_clear)
void dynarray_clear(struct dynarray * restrict da);


#undef  dynarray_set
#define dynarray_set  DYNAMIC_ARRAY_GEN_EXPAND(dynarray_set)
void dynarray_set(struct dynarray * restrict da, long pos, _TYPE elem);

#undef  dynarray_set_safe
#define dynarray_set_safe  DYNAMIC_ARRAY_GEN_EXPAND(dynarray_set_safe)
void dynarray_set_safe(struct dynarray * restrict da, long pos, _TYPE elem, _TYPE empty_value);


#undef  dynarray_push_back
#define dynarray_push_back  DYNAMIC_ARRAY_GEN_EXPAND(dynarray_push_back)
void dynarray_push_back(struct dynarray * da, _TYPE elem);

#undef   dynarray_push_back_atomic
#define dynarray_push_back_atomic  DYNAMIC_ARRAY_GEN_EXPAND(dynarray_push_back_atomic)
void dynarray_push_back_atomic(struct dynarray * restrict da, _TYPE elem);


#undef  dynarray_push_back_array
#define dynarray_push_back_array  DYNAMIC_ARRAY_GEN_EXPAND(dynarray_push_back_array)
void dynarray_push_back_array(struct dynarray * restrict da, _TYPE * restrict array, long n);


#undef  dynarray_export_array
#define dynarray_export_array  DYNAMIC_ARRAY_GEN_EXPAND(dynarray_export_array)
long dynarray_export_array(struct dynarray * restrict da, _TYPE ** array_ret);

