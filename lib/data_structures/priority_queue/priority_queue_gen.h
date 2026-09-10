#if !defined(PRIORITY_QUEUE_GEN_TYPE_1)
	#error "PRIORITY_QUEUE_GEN_TYPE_1 not defined: data type"
#elif !defined(PRIORITY_QUEUE_GEN_TYPE_2)
	#error "PRIORITY_QUEUE_GEN_TYPE_2 not defined: element position in the queue: integer type"
#elif !defined(PRIORITY_QUEUE_GEN_SUFFIX)
	#error "PRIORITY_QUEUE_GEN_SUFFIX not defined"
#elif !defined(PRIORITY_QUEUE_GEN_FUNCTION_ATTRIBUTES)
	#define PRIORITY_QUEUE_GEN_FUNCTION_ATTRIBUTES
#endif

#include "macros/cpp_defines.h"
#include "macros/macrolib.h"


#ifndef CACHE_LINE_SIZE
	#define CACHE_LINE_SIZE  64
#endif

#ifndef PQ_DEGREE
	#define PQ_DEGREE  4
#endif


#define PRIORITY_QUEUE_GEN_EXPAND(name)  CONCAT(name, PRIORITY_QUEUE_GEN_SUFFIX)
#define PRIORITY_QUEUE_GEN_EXPAND_TYPE(name)  CONCAT(PRIORITY_QUEUE_GEN_, PRIORITY_QUEUE_GEN_EXPAND(name))

#undef  _TYPE
#define _TYPE  PRIORITY_QUEUE_GEN_EXPAND_TYPE(_TYPE)
typedef PRIORITY_QUEUE_GEN_TYPE_1  _TYPE;

#undef  _TYPE_I
#define _TYPE_I  PRIORITY_QUEUE_GEN_EXPAND_TYPE(_TYPE_I)
typedef PRIORITY_QUEUE_GEN_TYPE_2  _TYPE_I;


//==========================================================================================================================================
//= Structs
//==========================================================================================================================================


#undef pq_node_data
#define pq_node_data  PRIORITY_QUEUE_GEN_EXPAND(pq_node_data)
struct pq_node_data {
	_TYPE data;
	_TYPE_I * position_tracker;
};
//	int64_t lock;
//	char padding[0] __attribute__ ((aligned (CACHE_LINE_SIZE)));
//} __attribute__ ((aligned (CACHE_LINE_SIZE)));


#undef pq_data
#define pq_data  PRIORITY_QUEUE_GEN_EXPAND(pq_data)
struct pq_data {
	long size;
	_TYPE_I num_elem;
	struct pq_node_data * tree;
	int degree;

	int num_threads;
};


//==========================================================================================================================================
//= Functions
//==========================================================================================================================================


#undef pq_init
#define pq_init  PRIORITY_QUEUE_GEN_EXPAND(pq_init)
PRIORITY_QUEUE_GEN_FUNCTION_ATTRIBUTES
void pq_init(struct pq_data * pq, _TYPE_I N);

#undef pq_clean
#define pq_clean  PRIORITY_QUEUE_GEN_EXPAND(pq_clean)
PRIORITY_QUEUE_GEN_FUNCTION_ATTRIBUTES
void pq_clean(struct pq_data * pq);

#undef pq_destroy
#define pq_destroy  PRIORITY_QUEUE_GEN_EXPAND(pq_destroy)
PRIORITY_QUEUE_GEN_FUNCTION_ATTRIBUTES
void pq_destroy(struct pq_data ** pq_ptr);

#undef pq_correct_up
#define pq_correct_up  PRIORITY_QUEUE_GEN_EXPAND(pq_correct_up)
PRIORITY_QUEUE_GEN_FUNCTION_ATTRIBUTES
void pq_correct_up(struct pq_data * pq, _TYPE_I i);

#undef pq_correct_down
#define pq_correct_down  PRIORITY_QUEUE_GEN_EXPAND(pq_correct_down)
PRIORITY_QUEUE_GEN_FUNCTION_ATTRIBUTES
void pq_correct_down(struct pq_data * pq, _TYPE_I i);

#undef pq_pop
#define pq_pop  PRIORITY_QUEUE_GEN_EXPAND(pq_pop)
PRIORITY_QUEUE_GEN_FUNCTION_ATTRIBUTES
int pq_pop(struct pq_data * pq, _TYPE * ret);

#undef pq_push
#define pq_push  PRIORITY_QUEUE_GEN_EXPAND(pq_push)
PRIORITY_QUEUE_GEN_FUNCTION_ATTRIBUTES
void pq_push(struct pq_data * pq, _TYPE data, _TYPE_I * position_tracker);

#undef pq_get_data
#define pq_get_data  PRIORITY_QUEUE_GEN_EXPAND(pq_get_data)
PRIORITY_QUEUE_GEN_FUNCTION_ATTRIBUTES
_TYPE pq_get_data(struct pq_data * pq, _TYPE_I position);

#undef pq_set_data
#define pq_set_data  PRIORITY_QUEUE_GEN_EXPAND(pq_set_data)
PRIORITY_QUEUE_GEN_FUNCTION_ATTRIBUTES
void pq_set_data(struct pq_data * pq, _TYPE_I position, _TYPE data);

