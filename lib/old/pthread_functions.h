#ifndef PTHREAD_FUNCTIONS_H
#define PTHREAD_FUNCTIONS_H

#include <unistd.h>
#include <pthread.h>

#include "macros/cpp_defines.h"

void safe_pthread_create(pthread_t * thread, const pthread_attr_t * attr, void * (*start_routine) (void *), void * arg);
void safe_pthread_join(pthread_t thread, void ** retval);


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
--------------------------------------------------------------------------------------------------------------------------------------------
-                                                              Thread Attributes                                                           -
--------------------------------------------------------------------------------------------------------------------------------------------
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


void safe_pthread_attr_init(pthread_attr_t * attr);
void safe_pthread_attr_destroy(pthread_attr_t * attr);
void safe_pthread_getattr_np(pthread_t thread, pthread_attr_t * attr);


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
--------------------------------------------------------------------------------------------------------------------------------------------
-                                                              Thread Affinity                                                             -
--------------------------------------------------------------------------------------------------------------------------------------------
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


//==========================================================================================================================================
//= Handle Affinity Throught Attributes (At Thread Creation)
//==========================================================================================================================================


void safe_pthread_attr_setaffinity_np(pthread_attr_t * attr, size_t cpusetsize, const cpu_set_t * cpuset);
void safe_pthread_attr_getaffinity_np(const pthread_attr_t * attr, size_t cpusetsize, cpu_set_t * cpuset);
void set_affinity_attr(pthread_attr_t * attr, int cpu);


//==========================================================================================================================================
//= Handle Affinity Online
//==========================================================================================================================================


void safe_pthread_setaffinity_np(pthread_t thread, size_t cpusetsize, const cpu_set_t * cpuset);
void safe_pthread_getaffinity_np(pthread_t thread, size_t cpusetsize, cpu_set_t * cpuset);
void set_affinity(pthread_t thread, int cpu);
int get_affinity(pthread_t thread, int ** cpus_out);


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
--------------------------------------------------------------------------------------------------------------------------------------------
-                                                              Pthread Barrier                                                             -
--------------------------------------------------------------------------------------------------------------------------------------------
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


void safe_pthread_barrier_init(pthread_barrier_t * __restrict__ barrier, const pthread_barrierattr_t * __restrict__ attr, unsigned count);
void safe_pthread_barrier_destroy(pthread_barrier_t * barrier);
int safe_pthread_barrier_wait(pthread_barrier_t * barrier);


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
--------------------------------------------------------------------------------------------------------------------------------------------
-                                                            Pthread Spinlock                                                              -
--------------------------------------------------------------------------------------------------------------------------------------------
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


void safe_pthread_spin_init(pthread_spinlock_t * lock, int pshared); // Use PTHREAD_PROCESS_PRIVATE as 'pshared' if the lock is private to the process.
void safe_pthread_spin_destroy(pthread_spinlock_t * lock);
void safe_pthread_spin_lock(pthread_spinlock_t * lock);
void safe_pthread_spin_trylock(pthread_spinlock_t * lock);
void safe_pthread_spin_unlock(pthread_spinlock_t * lock);


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
--------------------------------------------------------------------------------------------------------------------------------------------
-                                                              Thread Stack                                                                -
--------------------------------------------------------------------------------------------------------------------------------------------
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


void safe_pthread_attr_setstack(pthread_attr_t * attr, void * stackaddr, size_t stacksize);
void safe_pthread_attr_getstack(pthread_attr_t * attr, void ** stackaddr, size_t * stacksize);
void safe_pthread_attr_setstacksize(pthread_attr_t * attr, size_t stacksize);
void safe_pthread_attr_getstacksize(const pthread_attr_t * attr, size_t * stacksize);


#endif /* PTHREAD_FUNCTIONS_H */

