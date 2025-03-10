#ifndef SPINLOCK_H
#define SPINLOCK_H

#include "macros/cpp_defines.h"
#include "lock/lock_util.h"


//==========================================================================================================================================
//= Clhlock
//==========================================================================================================================================


struct clhlock {
	struct lock_padded_int * nodes;
	struct lock_padded_ptr * t_curr;
	struct lock_padded_ptr * t_prev;
	struct lock_padded_ptr tail;
};


void
clhlock_init(struct clhlock * l, int num_threads)
{
	long i;
	l->nodes = (typeof((l->nodes))) malloc((num_threads + 1) * sizeof(*l->nodes));
	l->t_curr = (typeof((l->t_curr))) malloc(num_threads * sizeof(*l->t_curr));
	l->t_prev = (typeof((l->t_prev))) malloc(num_threads * sizeof(*l->t_prev));
	#pragma omp parallel for
	for (i=0;i<num_threads;i++)
	{
		l->nodes[i].val = LOCK_LOCKED;
		l->t_curr[i].ptr = &l->nodes[i];
		l->t_prev[i].ptr = NULL;
	}
	l->nodes[num_threads].val = LOCK_UNLOCKED;
	l->tail.ptr = &l->nodes[num_threads];
}


struct clhlock *
clhlock_new(int num_threads)
{
	struct clhlock * l = (typeof(l)) malloc(sizeof(*l));
	clhlock_init(l, num_threads);
	return l;
}


static inline
void
clhlock_lock(struct clhlock * l, int tnum)
{
	struct lock_padded_ptr * curr = &l->t_curr[tnum];
	struct lock_padded_ptr * prev = &l->t_prev[tnum];;
	__atomic_store_n(&curr->ptr->val, LOCK_LOCKED, __ATOMIC_RELAXED);
	prev->ptr = __atomic_exchange_n(&l->tail.ptr, curr->ptr, __ATOMIC_RELAXED);
	while (__atomic_load_n(&prev->ptr->val, __ATOMIC_ACQUIRE) == LOCK_LOCKED)
		lock_cpu_relax();
}


static inline
void
clhlock_unlock(struct clhlock * l, int tnum)
{
	struct lock_padded_ptr * curr = &l->t_curr[tnum];
	struct lock_padded_ptr * prev = &l->t_prev[tnum];;
	__atomic_store_n(&curr->ptr->val, LOCK_UNLOCKED, __ATOMIC_RELEASE);
	curr->ptr = prev->ptr;
}


//==========================================================================================================================================
//= Arraylock
//==========================================================================================================================================



struct arraylock {
	int num_threads;
	struct lock_padded_int * t_ids;
	struct lock_padded_int * t_flags;
	struct lock_padded_int * ticket;
};


void
arraylock_init(struct arraylock * l, int num_threads)
{
	long i;
	l->num_threads = num_threads;
	l->t_ids = (typeof((l->t_ids))) malloc(num_threads * sizeof(*l->t_ids));
	l->t_flags = (typeof((l->t_flags))) malloc(num_threads * sizeof(*l->t_flags));
	l->ticket = (typeof(l->ticket)) malloc(sizeof(*l->ticket));
	l->ticket->val = 0;
	#pragma omp parallel for
	for (i=0;i<num_threads;i++)
	{
		l->t_ids[i].val = 0;
		l->t_flags[i].val = LOCK_LOCKED;
	}
	l->t_flags[0].val = LOCK_UNLOCKED;
}


struct arraylock *
arraylock_new(int num_threads)
{
	struct arraylock * l = (typeof(l)) malloc(sizeof(*l));
	arraylock_init(l, num_threads);
	return l;
}


static inline
void
arraylock_lock(struct arraylock * l, int tnum)
{
	int32_t id;
	id = __atomic_fetch_add(&l->ticket->val, 1, __ATOMIC_ACQUIRE) % l->num_threads;
	l->t_ids[tnum].val = id;
	while (__atomic_load_n(&l->t_flags[id].val, __ATOMIC_RELAXED) == LOCK_LOCKED)
		lock_cpu_relax();
}


static inline
void
arraylock_unlock(struct arraylock * l, int tnum)
{
	int32_t id = l->t_ids[tnum].val;
	__atomic_store_n(&l->t_flags[id].val, LOCK_LOCKED, __ATOMIC_RELEASE);
	__atomic_store_n(&l->t_flags[(id + 1) % l->num_threads].val, LOCK_UNLOCKED, __ATOMIC_RELEASE);
}


//==========================================================================================================================================
//= Ticketlock
//==========================================================================================================================================


static inline
void
ticketlock_lock(int32_t * ticket, int32_t * current)
{
	int32_t val;
	val = __atomic_fetch_add(ticket, 1, __ATOMIC_ACQUIRE);
	while (val != __atomic_load_n(current, __ATOMIC_RELAXED))
		lock_cpu_relax();
}


static inline
void
ticketlock_unlock(int32_t * current)
{
	__atomic_fetch_add(current, 1, __ATOMIC_RELEASE);
}


//==========================================================================================================================================
//= Spinlock - TTAS
//==========================================================================================================================================


static inline
void
ttaslock_lock(int32_t * lock)
{
	while (1)
	{
		if (__atomic_load_n(lock, __ATOMIC_RELAXED) == LOCK_UNLOCKED)
		{
			if (__atomic_exchange_n(lock, LOCK_LOCKED, __ATOMIC_ACQUIRE) == LOCK_UNLOCKED)
				break;
		}
		lock_cpu_relax();
	}
}


static inline
void
ttaslock_unlock(int32_t * lock)
{
	__atomic_store_n(lock, LOCK_UNLOCKED, __ATOMIC_RELEASE);
}


#endif /* SPINLOCK_H */

