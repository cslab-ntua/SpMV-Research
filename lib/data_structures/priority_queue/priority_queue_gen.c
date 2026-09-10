#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <omp.h>

#include "debug.h"
#include "macros/macrolib.h"
#include "parallel_util.h"
#include "omp_functions.h"

#include "priority_queue_gen.h"


//==========================================================================================================================================
//= User Functions Declarations
//==========================================================================================================================================


/* compare(a, b)
 * max queue:
 *	(a > b) ? 1 : (a < b) ? -1 : 0
 * min queue:
 *	(a < b) ? 1 : (a > b) ? -1 : 0
 */
#undef  pq_cmp
#define pq_cmp  PRIORITY_QUEUE_GEN_EXPAND(pq_cmp)
static int pq_cmp(_TYPE a, _TYPE b);


//==========================================================================================================================================
//= Includes
//==========================================================================================================================================


//==========================================================================================================================================
//= Local Defines
//==========================================================================================================================================


#undef  _TYPE
#define _TYPE  PRIORITY_QUEUE_GEN_EXPAND_TYPE(_TYPE)
typedef PRIORITY_QUEUE_GEN_TYPE_1  _TYPE;

#undef  _TYPE_I
#define _TYPE_I  PRIORITY_QUEUE_GEN_EXPAND_TYPE(_TYPE_I)
typedef PRIORITY_QUEUE_GEN_TYPE_2  _TYPE_I;


#undef  _SWAP
#define _SWAP(a, b)               \
do {                              \
	__auto_type __tmp = a;    \
	a = b;                    \
	b = __tmp;                \
} while (0)


//==========================================================================================================================================
//------------------------------------------------------------------------------------------------------------------------------------------
//-                                                              Templates                                                                 -
//------------------------------------------------------------------------------------------------------------------------------------------
//==========================================================================================================================================


#undef pq_parent
#define pq_parent  PRIORITY_QUEUE_GEN_EXPAND(pq_parent)
static inline
_TYPE_I
pq_parent(_TYPE_I i)
{
	_TYPE_I j = (i-1) / PQ_DEGREE;
	return (i == 0) ? -1 : j;
}


#undef pq_children
#define pq_children  PRIORITY_QUEUE_GEN_EXPAND(pq_children)
static inline
_TYPE_I
pq_children(struct pq_data * pq, _TYPE_I i)
{
	_TYPE_I j = PQ_DEGREE*i + 1;
	return (j < pq->num_elem) ? j : -1;
}


#undef pq_init
#define pq_init  PRIORITY_QUEUE_GEN_EXPAND(pq_init)
PRIORITY_QUEUE_GEN_FUNCTION_ATTRIBUTES
void
pq_init(struct pq_data * pq, _TYPE_I N)
{
	pq->size = N;
	pq->num_elem = 0;
	pq->tree = (typeof(pq->tree)) malloc(N * sizeof(*pq->tree));
	pq->degree = PQ_DEGREE;
	#if defined(_OPENMP)
		#pragma omp parallel
		{
			#pragma omp single
			{
				pq->num_threads = safe_omp_get_num_threads();
			}
		}
	#else
		pq->num_threads = 1;
	#endif
}


#undef pq_clean
#define pq_clean  PRIORITY_QUEUE_GEN_EXPAND(pq_clean)
PRIORITY_QUEUE_GEN_FUNCTION_ATTRIBUTES
void
pq_clean(struct pq_data * pq)
{
	free(pq->tree);
	pq->tree = NULL;
}


#undef pq_destroy
#define pq_destroy  PRIORITY_QUEUE_GEN_EXPAND(pq_destroy)
PRIORITY_QUEUE_GEN_FUNCTION_ATTRIBUTES
void
pq_destroy(struct pq_data ** pq_ptr)
{
	pq_clean(*pq_ptr);
	free(*pq_ptr);
	*pq_ptr = NULL;
}


#undef pq_correct_up
#define pq_correct_up  PRIORITY_QUEUE_GEN_EXPAND(pq_correct_up)
PRIORITY_QUEUE_GEN_FUNCTION_ATTRIBUTES
void
pq_correct_up(struct pq_data * pq, _TYPE_I i)
{
	struct pq_node_data * node, * parent;
	_TYPE_I j;
	node = &pq->tree[i];
	while (1)
	{
		j = pq_parent(i);
		if (j < 0)
			break;
		parent = &pq->tree[j];
		if (pq_cmp(node->data, parent->data) != 1)
			break;
		// printf("swap %ld %ld - val %ld %ld\n", i, j, node->data, parent->data);
		_SWAP(*node, *parent);
		_SWAP(*node->position_tracker, *parent->position_tracker);  // 'position_tracker's are pointers. Need to swap both pointers and values because they point to user addresses (for the user to track the element position in the queue).
		i = j;
		node = parent;
	}
}


#undef pq_max_child
#define pq_max_child  PRIORITY_QUEUE_GEN_EXPAND(pq_max_child)
static inline
_TYPE_I
pq_max_child(struct pq_data * pq, _TYPE_I i)
{
	struct pq_node_data * children, * max;
	_TYPE_I j, c, max_index;
	int degree = PQ_DEGREE;
	c = pq_children(pq, i);
	if (c < 0)
		return -1;
	children = &pq->tree[c];
	max = children;
	max_index = c;
	if (pq->num_elem - c <= degree)
		degree = pq->num_elem - c;

	for (j=1;j<degree;j++)
	{
		if (pq_cmp(children[j].data, max->data) == 1)
		{
			max_index = c + j;
			max = &children[j];
		}
	}
	return max_index;
}


#undef pq_correct_down
#define pq_correct_down  PRIORITY_QUEUE_GEN_EXPAND(pq_correct_down)
PRIORITY_QUEUE_GEN_FUNCTION_ATTRIBUTES
void
pq_correct_down(struct pq_data * pq, _TYPE_I i)
{
	struct pq_node_data * node, * max_child;
	_TYPE_I max_index;
	node = &pq->tree[i];
	while (1)
	{
		max_index = pq_max_child(pq, i);
		if (max_index < 0)
			break;
		max_child = &pq->tree[max_index];
		if (pq_cmp(max_child->data, node->data) != 1)
			break;
		_SWAP(*node, *max_child);
		_SWAP(*node->position_tracker, *max_child->position_tracker);  // 'position_tracker's are pointers. Need to swap both pointers and values because they point to user addresses (for the user to track the element position in the queue).
		i = max_index;
		node = max_child;
	}
}


#undef pq_pop
#define pq_pop  PRIORITY_QUEUE_GEN_EXPAND(pq_pop)
PRIORITY_QUEUE_GEN_FUNCTION_ATTRIBUTES
int
pq_pop(struct pq_data * pq, _TYPE * ret)
{
	_TYPE ret_data;
	if (pq->num_elem == 0)
		return -1;
	ret_data = pq->tree[0].data;
	pq->tree[0] = pq->tree[pq->num_elem - 1];
	*pq->tree[0].position_tracker = 0;
	pq->num_elem--;
	pq_correct_down(pq, 0);
	*ret = ret_data;
	return 0;
}


#undef pq_push
#define pq_push  PRIORITY_QUEUE_GEN_EXPAND(pq_push)
PRIORITY_QUEUE_GEN_FUNCTION_ATTRIBUTES
void
pq_push(struct pq_data * pq, _TYPE data, _TYPE_I * position_tracker)
{
	_TYPE_I i;
	pq->num_elem++;
	if (pq->num_elem > pq->size)
		error("no more space in queue");
	i = pq->num_elem - 1;
	*position_tracker = i;
	pq->tree[i].position_tracker = position_tracker;
	pq->tree[i].data = data;
	pq_correct_up(pq, i);
}


#undef pq_get_data
#define pq_get_data  PRIORITY_QUEUE_GEN_EXPAND(pq_get_data)
PRIORITY_QUEUE_GEN_FUNCTION_ATTRIBUTES
_TYPE
pq_get_data(struct pq_data * pq, _TYPE_I position)
{
	return pq->tree[position].data;
}


#undef pq_set_data
#define pq_set_data  PRIORITY_QUEUE_GEN_EXPAND(pq_set_data)
PRIORITY_QUEUE_GEN_FUNCTION_ATTRIBUTES
void
pq_set_data(struct pq_data * pq, _TYPE_I position, _TYPE data)
{
	pq->tree[position].data = data;
}

