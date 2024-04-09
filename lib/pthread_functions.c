#ifndef _GNU_SOURCE
	#define _GNU_SOURCE
#endif

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sched.h>
#include <pthread.h>

#include "macros/cpp_defines.h"
#include "debug.h"

#include "pthread_functions.h"


/*
 * For the scheduling functions we need to put
 *     #define _GNU_SOURCE
 * BEFORE including any other header.
 */

void safe_pthread_create(pthread_t * thread, const pthread_attr_t * attr, void * (*start_routine) (void *), void * arg)
{
	int ret;
	ret = pthread_create(thread, attr, start_routine, arg);
	if (__builtin_expect(ret, 0))
		error_pthread(ret, "pthread_create");
}


void safe_pthread_join(pthread_t thread, void ** retval)
{
	int ret;
	ret = pthread_join(thread, retval);
	if (__builtin_expect(ret, 0))
		error_pthread(ret, "pthread_join");
}


//==========================================================================================================================================
//------------------------------------------------------------------------------------------------------------------------------------------
//-                                                            Thread Attributes                                                           -
//------------------------------------------------------------------------------------------------------------------------------------------
//==========================================================================================================================================


/* 
 * The pthread_attr_init() function initializes the thread attributes object pointed to by attr with default attribute values.
 * Calling pthread_attr_init() on a thread attributes object that has already been initialized results in undefined behavior.
 *
 * When a thread attributes object is no longer required, it should be destroyed using the pthread_attr_destroy() function.
 * Destroying a thread attributes object has no effect on threads that were created using that object.
 */

void safe_pthread_attr_init(pthread_attr_t * attr)
{
	int ret;
	ret = pthread_attr_init(attr);
	if (__builtin_expect(ret, 0))
		error_pthread(ret, "pthread_attr_init");
}


void safe_pthread_attr_destroy(pthread_attr_t * attr)
{
	int ret;
	if (attr == NULL)
		return;
	ret = pthread_attr_destroy(attr);
	if (__builtin_expect(ret, 0))
		error_pthread(ret, "pthread_attr_destroy");
}


void safe_pthread_getattr_np(pthread_t thread, pthread_attr_t * attr)
{
	int ret;
	ret = pthread_getattr_np(thread, attr);
	if (__builtin_expect(ret, 0))
		error_pthread(ret, "pthread_getattr_np");
}


//==========================================================================================================================================
//------------------------------------------------------------------------------------------------------------------------------------------
//-                                                            Thread Affinity                                                             -
//------------------------------------------------------------------------------------------------------------------------------------------
//==========================================================================================================================================


//==========================================================================================================================================
//= Handle Affinity Throught Attributes (At Thread Creation)
//==========================================================================================================================================


/* 
 * The pthread_attr_setaffinity_np() function sets the CPU affinity mask attribute of the thread 
 * attributes object referred to by attr to the value specified in cpuset.
 * This attribute determines the CPU affinity mask of a thread created using the thread attributes object attr.
 *
 * The pthread_attr_getaffinity_np() function returns the CPU affinity  mask attribute of the thread
 * attributes object referred to by attr in the buffer pointed to by cpuset.
 *
 * The argument cpusetsize is the length (in bytes) of the buffer pointed to by cpuset.
 * Typically, this argument would be specified as * sizeof(cpu_set_t).
 */

void safe_pthread_attr_setaffinity_np(pthread_attr_t * attr, size_t cpusetsize, const cpu_set_t * cpuset)
{
	int ret;
	ret = pthread_attr_setaffinity_np(attr, cpusetsize, cpuset);
	if (__builtin_expect(ret, 0))
		error_pthread(ret, "pthread_attr_setaffinity_np");
}


void safe_pthread_attr_getaffinity_np(const pthread_attr_t * attr, size_t cpusetsize, cpu_set_t * cpuset)
{
	int ret;
	ret = pthread_attr_getaffinity_np(attr, cpusetsize, cpuset);
	if (__builtin_expect(ret, 0))
		error_pthread(ret, "pthread_attr_getaffinity_np");
}


void set_affinity_attr(pthread_attr_t * attr, int cpu)
{
	cpu_set_t cpuset;

	CPU_ZERO(&cpuset);
	CPU_SET(cpu, &cpuset);
	safe_pthread_attr_setaffinity_np(attr, sizeof(cpuset), &cpuset);
}


//==========================================================================================================================================
//= Handle Affinity Online
//==========================================================================================================================================


/*
 * The pthread_setaffinity_np() function sets the CPU affinity mask of
 * the thread thread to the CPU set pointed to by cpuset.  If the call
 * is successful, and the thread is not currently running on one of the
 * CPUs in cpuset, then it is migrated to one of those CPUs.
 */

void safe_pthread_setaffinity_np(pthread_t thread, size_t cpusetsize, const cpu_set_t * cpuset)
{
	int ret;
	ret = pthread_setaffinity_np(thread, cpusetsize, cpuset);
	if (__builtin_expect(ret, 0))
		error_pthread(ret, "pthread_setaffinity_np");
}


void safe_pthread_getaffinity_np(pthread_t thread, size_t cpusetsize, cpu_set_t * cpuset)
{
	int ret;
	ret = pthread_getaffinity_np(thread, cpusetsize, cpuset);
	if (__builtin_expect(ret, 0))
		error_pthread(ret, "pthread_setaffinity_np");
}


void set_affinity(pthread_t thread, int cpu)
{
	cpu_set_t cpuset;

	CPU_ZERO(&cpuset);
	CPU_SET(cpu, &cpuset);
	safe_pthread_setaffinity_np(thread, sizeof(cpuset), &cpuset);
}


int get_affinity(pthread_t thread, int ** cpus_out)
{
	int * cpus;
	cpu_set_t cpuset;
	int i, j;
	safe_pthread_getaffinity_np(thread, sizeof(cpuset), &cpuset);
	cpus = (typeof(cpus)) malloc(CPU_SETSIZE * sizeof(*cpus));
	for (i=0,j=0;i<CPU_SETSIZE;i++)
		if (CPU_ISSET(i, &cpuset))
			cpus[j++] = i;
	*cpus_out = cpus;
	return j;
}


//==========================================================================================================================================
//------------------------------------------------------------------------------------------------------------------------------------------
//-                                                            Pthread Barrier                                                             -
//------------------------------------------------------------------------------------------------------------------------------------------
//==========================================================================================================================================


/*
 * C++ does not have standard support for 'restrict', but many compilers have equivalents that usually work in both C++ and C,
 * such as the GCC's and Clang's '__restrict__', and Visual C++'s __declspec(restrict).
 * In addition, '__restrict' is supported by those three compilers.
 *
 * int pthread_barrier_init(pthread_barrier_t *restrict barrier, const pthread_barrierattr_t *restrict attr, unsigned count);
 *
 *     The pthread_barrier_init() function shall allocate any resources required to use the barrier referenced by barrier
 *     and shall initialize the barrier with attributes referenced by attr.
 *     If attr is NULL, the default barrier attributes shall be used; the effect is the same as passing the address 
 *     of a default barrier attributes object.
 *     The results are undefined if pthread_barrier_init() is called when any thread is blocked on the barrier 
 *     (that is, has not returned from the pthread_barrier_wait() call).
 *     The results are undefined if a barrier is used without first being initialized.
 *     The results are undefined if pthread_barrier_init() is called specifying an already initialized barrier.
 *
 *     The count argument specifies the number of threads that must call pthread_barrier_wait() before any of them successfully return from the call.
 *     The value specified by count must be greater than zero.
 *     If the pthread_barrier_init() function fails, the barrier shall not be initialized and the contents of barrier are undefined.
 *
 *     Only the object referenced by barrier may be used for performing synchronization.
 *     The result of referring to copies of that object in calls to pthread_barrier_destroy() or pthread_barrier_wait() is undefined. 
 *
 * int pthread_barrier_wait(pthread_barrier_t *barrier);
 *
 *     The pthread_barrier_wait() function shall synchronize participating threads at the barrier referenced by barrier.
 *     The calling thread shall block until the required number of threads have called pthread_barrier_wait() specifying the barrier.
 *
 *     When the required number of threads have called pthread_barrier_wait() specifying the barrier,
 *     the constant  PTHREAD_BARRIER_SERIAL_THREAD shall be returned to one unspecified thread
 *     and zero shall be returned to each of the remaining threads.
 */

void safe_pthread_barrier_init(pthread_barrier_t * __restrict__ barrier, const pthread_barrierattr_t * __restrict__ attr, unsigned count)
{
	int ret;
	ret = pthread_barrier_init(barrier, attr, count);
	if (__builtin_expect(ret, 0))
		error_pthread(ret, "pthread_barrier_init");
}


void safe_pthread_barrier_destroy(pthread_barrier_t * barrier)
{
	int ret;
	if (barrier == NULL)
		return;
	ret = pthread_barrier_destroy(barrier);
	if (__builtin_expect(ret, 0))
		error_pthread(ret, "pthread_barrier_destroy");
}


int safe_pthread_barrier_wait(pthread_barrier_t * barrier)
{
	int ret;
	ret = pthread_barrier_wait(barrier);
	if (__builtin_expect(ret && (ret != PTHREAD_BARRIER_SERIAL_THREAD), 0))
		error_pthread(ret, "pthread_barrier_wait");
	return ret;
}


//==========================================================================================================================================
//------------------------------------------------------------------------------------------------------------------------------------------
//-                                                          Pthread Spinlock                                                              -
//------------------------------------------------------------------------------------------------------------------------------------------
//==========================================================================================================================================


void safe_pthread_spin_init(pthread_spinlock_t * lock, int pshared)
{
	int ret;
	ret = pthread_spin_init(lock, pshared);      // Use PTHREAD_PROCESS_PRIVATE as 'pshared' if the lock is private to the process.
	if (__builtin_expect(ret, 0))
		error_pthread(ret, "pthread_spin_init");
}


void safe_pthread_spin_destroy(pthread_spinlock_t * lock)
{
	int ret;
	if (lock == NULL)
		return;
	ret = pthread_spin_destroy(lock);
	if (__builtin_expect(ret, 0))
		error_pthread(ret, "pthread_spin_destroy");
}


void safe_pthread_spin_lock(pthread_spinlock_t * lock)
{
	int ret;
	ret = pthread_spin_lock(lock);
	if (__builtin_expect(ret, 0))
		error_pthread(ret, "pthread_spin_lock");
}


void safe_pthread_spin_trylock(pthread_spinlock_t * lock)
{
	int ret;
	ret = pthread_spin_trylock(lock);
	if (__builtin_expect(ret, 0))
		error_pthread(ret, "pthread_spin_trylock");
}


void safe_pthread_spin_unlock(pthread_spinlock_t * lock)
{
	int ret;
	ret = pthread_spin_unlock(lock);
	if (__builtin_expect(ret, 0))
		error_pthread(ret, "pthread_spin_unlock");
}


//==========================================================================================================================================
//------------------------------------------------------------------------------------------------------------------------------------------
//-                                                            Thread Stack                                                                -
//------------------------------------------------------------------------------------------------------------------------------------------
//==========================================================================================================================================


void safe_pthread_attr_setstack(pthread_attr_t * attr, void * stackaddr, size_t stacksize)
{
	int ret;
	ret = pthread_attr_setstack(attr, stackaddr, stacksize);
	if (__builtin_expect(ret, 0))
		error_pthread(ret, "pthread_attr_setstack");
}


void safe_pthread_attr_getstack(pthread_attr_t * attr, void ** stackaddr, size_t * stacksize)
{
	int ret;
	ret = pthread_attr_getstack(attr, stackaddr, stacksize);
	if (__builtin_expect(ret, 0))
		error_pthread(ret, "pthread_attr_getstack");
}


void safe_pthread_attr_setstacksize(pthread_attr_t * attr, size_t stacksize)
{
	int ret;
	ret = pthread_attr_setstacksize(attr, stacksize);
	if (__builtin_expect(ret, 0))
		error_pthread(ret, "pthread_attr_setstacksize");
}


void safe_pthread_attr_getstacksize(const pthread_attr_t * attr, size_t * stacksize)
{
	int ret;
	ret = pthread_attr_getstacksize(attr, stacksize);
	if (__builtin_expect(ret, 0))
		error_pthread(ret, "pthread_attr_getstacksize");
}

