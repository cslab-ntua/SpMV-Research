#include "vector_gen.h"


#ifndef VECTOR_GEN_C
#define VECTOR_GEN_C

#endif /* VECTOR_GEN_C */


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//------------------------------------------------------------------------------------------------------------------------------------------
//-                                                              Templates                                                                 -
//------------------------------------------------------------------------------------------------------------------------------------------
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


#undef  _TYPE
#define _TYPE  VECTOR_GEN_EXPAND(_TYPE)
typedef VECTOR_GEN_TYPE_1  _TYPE;


//==========================================================================================================================================
//= User Functions Declarations
//==========================================================================================================================================


//==========================================================================================================================================
//= Generic Code
//==========================================================================================================================================


#undef vector_init
#define vector_init  VECTOR_GEN_EXPAND(vector_init)
void
vector_init(struct Vector * v, long bytes)
{
	v->page_size = sysconf(_SC_PAGESIZE);
	v->capacity = v->page_size * ((bytes / v->page_size) + 1); // + 1, to never be 0.
	v->capacity = v->page_size * ((bytes + v->page_size - 1) / v->page_size);
	if (v->capacity == 0)
		v->capacity = v->page_size;
	v->max_size = v->capacity / sizeof(*(v->data));
	v->size = 0;
	v->data = safe_mmap_annon(v->capacity);
}


#undef vector_new
#define vector_new  VECTOR_GEN_EXPAND(vector_new)
struct Vector *
vector_new(long bytes)
{
	struct Vector * v = malloc(sizeof(*v));
	vector_init(v, bytes);
	return v;
}


#undef vector_destroy
#define vector_destroy  VECTOR_GEN_EXPAND(vector_destroy)
void
vector_destroy(struct Vector * v)
{
	safe_munmap(v->data, v->capacity);
	free(v);
}


#undef vector_resize
#define vector_resize  VECTOR_GEN_EXPAND(vector_resize)
inline
void
vector_resize(struct Vector * v, long new_capacity)
{
	_TYPE * buf;
	new_capacity = v->page_size * ((new_capacity + v->page_size - 1) / v->page_size);
	if (new_capacity == 0)
		new_capacity = v->page_size;
	buf = safe_mmap_annon(new_capacity);
	v->data = safe_mremap_fixed(v->data, v->capacity, new_capacity, buf);
	v->capacity = new_capacity;
	v->max_size = new_capacity / sizeof(*(v->data));
	if (v->size > v->max_size)
		v->size = v->max_size;
}


#undef vector_push_back
#define vector_push_back  VECTOR_GEN_EXPAND(vector_push_back)
inline
void
vector_push_back(struct Vector * restrict v, _TYPE elem)
{
	if (v->size + 1 <= v->max_size)
	{
		v->data[v->size++] = elem;
	}
	else
	{
		vector_resize(v, 2 * v->capacity);
		v->data[v->size++] = elem;
	}
}


#undef vector_push_back_array
#define vector_push_back_array  VECTOR_GEN_EXPAND(vector_push_back_array)
inline
void
vector_push_back_array(struct Vector * restrict v, _TYPE * restrict array, long n)
{
	long i;
	if (v->size + n > v->max_size)
	{
		long new_capacity = 2 * v->capacity;
		long bytes = (v->size + n) * sizeof(*(v->data));
		while (bytes > new_capacity)
			new_capacity *= 2;
		vector_resize(v, new_capacity);
	}
	for (i=0;i<n;i++)
		v->data[v->size + i] = array[i];
	v->size += n;
}

