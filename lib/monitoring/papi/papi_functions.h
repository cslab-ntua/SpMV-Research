#ifndef PAPI_FUNCTIONS_H
#define PAPI_FUNCTIONS_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
// #include <stdint.h>
#include <papi.h>

#include "macros/cpp_defines.h"
#include "macros/macrolib.h"
#include "debug.h"


/*
 * To have access to performance registers we need to have a value >= 2
 * in the /proc/sys/kernel/perf_event_paranoid file.
 * e.g. # echo 2 > /proc/sys/kernel/perf_event_paranoid
 *    -1 - Not paranoid at all
 *     0 - Disallow raw tracepoint access for unpriv
 *     1 - Disallow cpu events for unpriv
 *     2 - Disallow kernel profiling for unpriv
 */


//==========================================================================================================================================
//------------------------------------------------------------------------------------------------------------------------------------------
//-                                                             Read Counters                                                              -
//------------------------------------------------------------------------------------------------------------------------------------------
//==========================================================================================================================================


/*
 * PAPI_read() copies the counters of the indicated event set into the array values.
 * The counters continue counting after the read.
 *
 * PAPI_read_ts() copies the counters of the indicated event set into the array values.
 * It also places a real-time cycle timestamp into cyc.
 * The counters continue counting after the read.
 *
 * PAPI_accum() adds the counters of the indicated event set into the array values.
 * The counters are zeroed and continue counting after the operation.
 *
 */


static inline
void safe_PAPI_read(int EventSet, long long *values)
{
	int ret = PAPI_read(EventSet, values);
	if (ret != PAPI_OK)
		error("PAPI_ERROR in PAPI_read, eventset=%d: %s\n", EventSet, PAPI_strerror(ret));
}


static inline
void safe_PAPI_read_ts(int EventSet, long long *values, long long *cyc)
{
	int ret = PAPI_read_ts(EventSet, values, cyc);
	if (ret != PAPI_OK)
		error("PAPI_ERROR in PAPI_read_ts: %s\n", PAPI_strerror(ret));
}


static inline
void safe_PAPI_accum(int EventSet, long long *values)
{
	int ret = PAPI_accum(EventSet, values);
	if (ret != PAPI_OK)
		error("PAPI_ERROR in PAPI_accum: %s\n", PAPI_strerror(ret));
}


static inline
void safe_PAPI_reset(int EventSet)
{
	int ret = PAPI_reset(EventSet);
	if (ret != PAPI_OK)
		error("PAPI_ERROR in PAPI_reset: %s\n", PAPI_strerror(ret));
}


//==========================================================================================================================================
//------------------------------------------------------------------------------------------------------------------------------------------
//-                                                           Initialization                                                               -
//------------------------------------------------------------------------------------------------------------------------------------------
//==========================================================================================================================================


static inline
void safe_PAPI_library_init(int version)
{
	int ret = PAPI_library_init(version);
	if (ret != PAPI_VER_CURRENT && ret > 0)
		error("PAPI library version mismatch!\n");
	if (ret < 0)
		error("PAPI_ERROR in PAPI_library_init: %s\n", ret, PAPI_strerror(ret));
	ret = PAPI_is_initialized();
	if (ret != PAPI_LOW_LEVEL_INITED)
		error("PAPI_ERROR in PAPI_is_initialized: %s\n", ret, PAPI_strerror(ret));
}


static inline
void safe_PAPI_thread_init()
{
	int ret = PAPI_thread_init((unsigned long ( * )( void )) pthread_self);           // The safest method to pass is pthread_self.
	if (ret != PAPI_OK)
		error("PAPI_ERROR in PAPI_thread_init: %s\n", ret, PAPI_strerror(ret));
}


static inline
void safe_PAPI_create_eventset(int *EventSet)
{
	*EventSet = PAPI_NULL;
	int ret = PAPI_create_eventset(EventSet);
	if (ret != PAPI_OK)
		error("PAPI_ERROR in PAPI_create_eventset (%d): %s\n", *EventSet, PAPI_strerror(ret));
}


static inline
void safe_PAPI_event_name_to_code(char *EventName, int *EventCode)
{
	int ret = PAPI_event_name_to_code(EventName, EventCode);
	if (ret != PAPI_OK)
		error("PAPI_ERROR in PAPI_event_name_to_code (%s): %s\n", EventName, PAPI_strerror(ret));
}


static inline
void safe_PAPI_add_event(int EventSet, int EventCode)
{
	// int ret = PAPI_add_event(EventSet, EventCode);
	// if (ret != PAPI_OK)
		// error("PAPI_ERROR in PAPI_add_event (%x): %s\n", EventCode, PAPI_strerror(ret));

	int ret, i;
	for (i=0;i<100;i++)
	{
		ret = PAPI_add_event(EventSet, EventCode);
		if (ret == PAPI_OK)
			return;
	}
	error("PAPI_ERROR in PAPI_add_event (%x): %s\n", EventCode, PAPI_strerror(ret));
}


static inline
void safe_PAPI_start(int EventSet)
{
	int ret = PAPI_start(EventSet);
	if (ret != PAPI_OK)
		error("PAPI_ERROR in PAPI_start (%d): %s\n", PAPI_strerror(ret));
}


static inline
void safe_papi_init_eventset(int *EventSet, char **EventNames, int NumEvents)
{
	int i, code;
	safe_PAPI_create_eventset(EventSet);
	for (i=0;i<NumEvents;i++)
	{
		safe_PAPI_event_name_to_code(EventNames[i], &code);
		safe_PAPI_add_event(*EventSet, code);
	}
	safe_PAPI_start(*EventSet);
	// Reset event cpu counters, to reduce chances of overflow.
	safe_PAPI_reset(*EventSet);
}


//==========================================================================================================================================
//------------------------------------------------------------------------------------------------------------------------------------------
//-                                                               Shutdown                                                                 -
//------------------------------------------------------------------------------------------------------------------------------------------
//==========================================================================================================================================


static inline
void safe_PAPI_stop(int EventSet, long long *values)
{
	int ret = PAPI_stop(EventSet, values);
	if (ret != PAPI_OK)
		error("PAPI_ERROR in PAPI_stop: %s\n", PAPI_strerror(ret));
}


static inline
void safe_PAPI_cleanup_eventset(int EventSet)
{
	int ret = PAPI_cleanup_eventset(EventSet);
	if (ret != PAPI_OK)
		error("PAPI_ERROR in PAPI_cleanup_eventset: %s\n", PAPI_strerror(ret));
}


static inline
void safe_PAPI_destroy_eventset(int * EventSet)
{
	int ret;
	if (EventSet == NULL)
		return;
	ret = PAPI_destroy_eventset(EventSet);
	if (ret != PAPI_OK)
		error("PAPI_ERROR in PAPI_destroy_eventset: %s\n", PAPI_strerror(ret));
}


static inline
void safe_PAPI_shutdown()
{
	PAPI_shutdown();
}


#endif /* PAPI_FUNCTIONS_H */

