#if !defined(VECTOR_GEN_TYPE_1)
	#error "VECTOR_GEN_TYPE_1 not defined: data type"
#elif !defined(VECTOR_GEN_SUFFIX)
	#error "VECTOR_GEN_SUFFIX not defined"
#endif

#include "macros/cpp_defines.h"
#include "macros/macrolib.h"
#include "macros/constants.h"


#define VECTOR_GEN_EXPAND(name)  CONCAT(name, VECTOR_GEN_SUFFIX)

#undef  _TYPE
#define _TYPE  VECTOR_GEN_EXPAND(_TYPE)
typedef VECTOR_GEN_TYPE_1  _TYPE;


//==========================================================================================================================================
//= Structs
//==========================================================================================================================================


#undef  Vector
#define Vector  VECTOR_GEN_EXPAND(Vector)
struct Vector {
	long page_size;

	long capacity;  // max bytes
	long max_size;  // max items
	_TYPE * data;
	long size __attribute__ ((aligned(CACHE_LINE_SIZE)));      // current items

	long semaphore; // transaction completion semaphore

	char padding[0] __attribute__ ((aligned(CACHE_LINE_SIZE)));
};


//==========================================================================================================================================
//= Functions
//==========================================================================================================================================


#undef  vector_init
#define vector_init  VECTOR_GEN_EXPAND(vector_init)
void vector_init(struct Vector * v, long bytes);

#undef  vector_new
#define vector_new  VECTOR_GEN_EXPAND(vector_new)
struct Vector * vector_new(long bytes);

#undef  vector_clean
#define vector_clean  VECTOR_GEN_EXPAND(vector_clean)
void vector_clean(struct Vector * v);

#undef  vector_destroy
#define vector_destroy  VECTOR_GEN_EXPAND(vector_destroy)
void vector_destroy(struct Vector ** v_ptr);


#undef  vector_resize
#define vector_resize  VECTOR_GEN_EXPAND(vector_resize)
void vector_resize(struct Vector * v, long new_capacity);

#undef  vector_push_back
#define vector_push_back  VECTOR_GEN_EXPAND(vector_push_back)
void vector_push_back(struct Vector * v, _TYPE elem);

#undef   vector_push_back_atomic
#define vector_push_back_atomic  VECTOR_GEN_EXPAND(vector_push_back_atomic)
void vector_push_back_atomic(struct Vector * restrict v, _TYPE elem);

#undef  vector_push_back_array
#define vector_push_back_array  VECTOR_GEN_EXPAND(vector_push_back_array)
void vector_push_back_array(struct Vector * v, _TYPE * elem, long n);

#undef  vector_set
#define vector_set  VECTOR_GEN_EXPAND(vector_set)
void vector_set(struct Vector * restrict v, long pos, _TYPE elem);

#undef  vector_set_safe
#define vector_set_safe  VECTOR_GEN_EXPAND(vector_set_safe)
void vector_set_safe(struct Vector * restrict v, long pos, _TYPE elem);

