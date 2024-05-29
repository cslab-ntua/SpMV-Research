#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <omp.h>

#include "debug.h"
#include "io.h"
#include "macros/macrolib.h"
#include "macros/constants.h"

#include "vector_gen.h"


//==========================================================================================================================================
//= User Functions Declarations
//==========================================================================================================================================


//==========================================================================================================================================
//= Includes
//==========================================================================================================================================


//==========================================================================================================================================
//= Local Defines
//==========================================================================================================================================


#undef  _TYPE
#define _TYPE  VECTOR_GEN_EXPAND_TYPE(_TYPE)
typedef VECTOR_GEN_TYPE_1  _TYPE;


//==========================================================================================================================================
//------------------------------------------------------------------------------------------------------------------------------------------
//-                                                              Templates                                                                 -
//------------------------------------------------------------------------------------------------------------------------------------------
//==========================================================================================================================================


//==========================================================================================================================================
//= Constructor - Destructor
//==========================================================================================================================================


#undef  vector_init
#define vector_init  VECTOR_GEN_EXPAND(vector_init)
void
vector_init(struct Vector * v, long bytes)
{
	v->page_size = GET_PAGE_SIZE();
	v->capacity = v->page_size * ((bytes / v->page_size) + 1); // + 1, to never be 0.
	v->capacity = v->page_size * ((bytes + v->page_size - 1) / v->page_size);
	if (v->capacity == 0)
		v->capacity = v->page_size;
	v->max_size = v->capacity / sizeof(*(v->data));
	v->size = 0;
	v->data = (typeof(v->data)) safe_mmap_annon(v->capacity);
}


#undef  vector_new
#define vector_new  VECTOR_GEN_EXPAND(vector_new)
struct Vector *
vector_new(long bytes)
{
	struct Vector * v = (typeof(v)) malloc(sizeof(*v));
	vector_init(v, bytes);
	return v;
}


#undef  vector_clean
#define vector_clean  VECTOR_GEN_EXPAND(vector_clean)
void
vector_clean(struct Vector * v)
{
	safe_munmap(v->data, v->capacity);
}


#undef  vector_destroy
#define vector_destroy  VECTOR_GEN_EXPAND(vector_destroy)
void
vector_destroy(struct Vector ** v_ptr)
{
	vector_clean(*v_ptr);
	free(*v_ptr);
	*v_ptr = NULL;
}


//==========================================================================================================================================
//= Resize
//==========================================================================================================================================


#undef  vector_resize_base
#define vector_resize_base  VECTOR_GEN_EXPAND(vector_resize_base)
static inline
void
vector_resize_base(struct Vector * v, long new_capacity)
{
	_TYPE * buf;
	new_capacity = v->page_size * ((new_capacity + v->page_size - 1) / v->page_size);
	if (new_capacity == 0)
		new_capacity = v->page_size;
	buf = (typeof(buf)) safe_mmap_annon(new_capacity);
	v->data = (typeof(v->data)) safe_mremap_fixed(v->data, v->capacity, new_capacity, buf);
	v->capacity = new_capacity;
}


#undef  vector_resize
#define vector_resize  VECTOR_GEN_EXPAND(vector_resize)
inline
void
vector_resize(struct Vector * v, long new_capacity)
{
	vector_resize_base(v, new_capacity);
	v->max_size = v->capacity / sizeof(*(v->data));
	if (v->size > v->max_size)
		v->size = v->max_size;
}


//==========================================================================================================================================
//= Set
//==========================================================================================================================================


// Set element at position WITHOUT bounds checking.
#undef  vector_set
#define vector_set  VECTOR_GEN_EXPAND(vector_set)
inline
void
vector_set(struct Vector * restrict v, long pos, _TYPE elem)
{
	v->data[pos] = elem;
}

// Set element at position with bounds checking.
#undef  vector_set_safe
#define vector_set_safe  VECTOR_GEN_EXPAND(vector_set_safe)
inline
void
vector_set_safe(struct Vector * restrict v, long pos, _TYPE elem)
{
	if (__builtin_expect(pos >= v->max_size, 0))
	{
		vector_resize(v, 2 * v->capacity);
		v->size = pos;                       // Possible unfilled positions will still count toward the size.
	}
	v->data[pos] = elem;
}


//==========================================================================================================================================
//= Push Back
//==========================================================================================================================================


/* We can only either always use atomic push back, or always use the regular one.
 */


#undef  vector_push_back
#define vector_push_back  VECTOR_GEN_EXPAND(vector_push_back)
inline
void
vector_push_back(struct Vector * restrict v, _TYPE elem)
{
	if (__builtin_expect(v->size >= v->max_size, 0))
		vector_resize(v, 2 * v->capacity);
	v->data[v->size++] = elem;
}


#undef  vector_push_back_atomic
#define vector_push_back_atomic  VECTOR_GEN_EXPAND(vector_push_back_atomic)
inline
void
vector_push_back_atomic(struct Vector * restrict v, _TYPE elem)
{
	long pos = __atomic_fetch_add(&v->size, 1, __ATOMIC_RELAXED);
	while (__builtin_expect(pos >= __atomic_load_n(&v->max_size, __ATOMIC_ACQUIRE), 0))  // Resize.
	{
		if (pos == v->max_size) // It is our responsibitily to resize.
		{
			// Wait all previous to complete.
			while (pos > __atomic_load_n(&v->semaphore, __ATOMIC_ACQUIRE)){
				#ifdef __x86_64__
					__asm volatile ("rep; pause" : : : "memory");   // relax
				#else
					for (volatile int i = 0; i < 1000; ++i); // Adjust the loop count as needed
				#endif
			}
			vector_resize_base(v, 2 * v->capacity);
			__atomic_store_n(&v->max_size, v->capacity / sizeof(*(v->data)), __ATOMIC_RELEASE);
			break;
		}
		#ifdef __x86_64__
			__asm volatile ("rep; pause" : : : "memory");   // relax
		#else
			for (volatile int i = 0; i < 1000; ++i); // Adjust the loop count as needed
		#endif
	}
	v->data[pos] = elem;
	__atomic_fetch_add(&v->semaphore, 1, __ATOMIC_RELEASE);
}


//==========================================================================================================================================
//= Push Back Array
//==========================================================================================================================================


#undef  vector_push_back_array
#define vector_push_back_array  VECTOR_GEN_EXPAND(vector_push_back_array)
inline
void
vector_push_back_array(struct Vector * restrict v, _TYPE * restrict array, long n)
{
	if (v->size + n > v->max_size)
	{
		long new_capacity = 2 * v->capacity;
		long bytes = (v->size + n) * sizeof(*(v->data));
		while (bytes > new_capacity)
			new_capacity *= 2;
		vector_resize(v, new_capacity);
	}
	memcpy(&(v->data[v->size]), array, n * sizeof(*array));
	v->size += n;
}

