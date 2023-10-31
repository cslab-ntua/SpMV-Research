#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <papi.h>

#include "macros/macrolib.h"
#include "macros/constants.h"
#include "debug.h"
#include "papi_functions.h"

#include "papi_bench_functions.h"


int PAPI_BENCH_NUM_EVENTS;
char ** PAPI_BENCH_EVENT_NAMES;


void
papi_bench_initialization()
{
	safe_PAPI_library_init(PAPI_VER_CURRENT);
	safe_PAPI_thread_init();
}


void
papi_bench_initialize_eventset(int * eventset)
{
	safe_papi_init_eventset(eventset, PAPI_BENCH_EVENT_NAMES, PAPI_BENCH_NUM_EVENTS);
}


void
papi_bench_cleanup_eventset(int * eventset)
{
	long long tmp[PAPI_BENCH_NUM_EVENTS];
	safe_PAPI_stop(*eventset, tmp);
	safe_PAPI_cleanup_eventset(*eventset);
	safe_PAPI_destroy_eventset(eventset);
}


void
papi_bench_shutdown()
{
	safe_PAPI_shutdown();
}


//===========================================================================================================================================
//= Parse CLI arguments
//===========================================================================================================================================


static int
count_char_occurences(char c, char *str)
{
	size_t i, n=0;
	for (i=0;i<strlen(str);i++)
		if (str[i] == c)
			n++;
	return n;
}


static int
tokenize_string(char *in, char ***out_ptr, char delim)
{
	size_t i, j, n;
	char *str;
	char **out;
	str = strdup(in);
	n = count_char_occurences(delim, str) + 1;
	out = (char **) malloc(n * sizeof(char *));
	out[0] = str;
	for (i=0,j=0;i<strlen(str);i++)
		if (str[i] == delim)
		{
			str[i] = '\0';
			out[++j] = str + i + 1;
		}
	*out_ptr = out;
	return n;
}


void
papi_bench_init_with_cli_args(int argc, char **argv)
{
	char *str = NULL;
	int i;

	for (i=0;i<argc;i++)
		if (!strcmp("--papi_events", argv[i]) && (i+1<argc))
		{
			str = argv[i+1];
			break;
		}
	if (str == NULL)
		error("No events given (--events comma,separated,list).");

	PAPI_BENCH_NUM_EVENTS = tokenize_string(str, &PAPI_BENCH_EVENT_NAMES, ',');
	if (PAPI_BENCH_NUM_EVENTS == PAPI_BENCH_MAX_EVENTS)
		error("Events are more than max (%d).", PAPI_BENCH_MAX_EVENTS);

	papi_bench_initialization();
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
--------------------------------------------------------------------------------------------------------------------------------------------
-                                                         OMP Benchmark Template                                                           -
--------------------------------------------------------------------------------------------------------------------------------------------
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


#include <omp.h>


#define get_thread_num()                                   \
({                                                         \
	int tnum;                                          \
	if (omp_get_level() > 1)                           \
		tnum = omp_get_ancestor_thread_num(1);     \
	else                                               \
		tnum = omp_get_thread_num();               \
	tnum;                                              \
})


struct Papi_Bench_Thread_Private_Data * PAPI_BENCH_TPD;


void
papi_bench_template_init(int argc, char **argv)
{
	int threads = safe_omp_get_num_threads_external();
	int i;
	PAPI_BENCH_TPD = (typeof(PAPI_BENCH_TPD)) malloc(threads * sizeof(*PAPI_BENCH_TPD));
	papi_bench_init_with_cli_args(argc, argv);
	_Pragma("omp parallel")
	{
		int tnum = get_thread_num();
		struct Papi_Bench_Thread_Private_Data * pbtpd = &PAPI_BENCH_TPD[tnum];
		papi_bench_initialize_eventset(&pbtpd->eventset);
		for (i=0;i<PAPI_BENCH_NUM_EVENTS;i++)
			pbtpd->papi_val[i] = 0;
	}
}


void
papi_bench_template_shutdown()
{
	_Pragma("omp parallel")
	{
		int tnum = get_thread_num();
		struct Papi_Bench_Thread_Private_Data * pbtpd = &PAPI_BENCH_TPD[tnum];
		papi_bench_cleanup_eventset(&pbtpd->eventset);
	}
	papi_bench_shutdown();
}


void
papi_bench_template_read_start(int tnum)
{
	struct Papi_Bench_Thread_Private_Data * pbtpd = &PAPI_BENCH_TPD[tnum];
	// Reset event cpu counters, to reduce chances of overflow.
	safe_PAPI_reset(pbtpd->eventset);
	safe_PAPI_read(pbtpd->eventset, pbtpd->val1);
}


void
papi_bench_template_read_end(int tnum)
{
	struct Papi_Bench_Thread_Private_Data * pbtpd = &PAPI_BENCH_TPD[tnum];
	safe_PAPI_read(pbtpd->eventset, pbtpd->val2);
	for (int i=0;i<PAPI_BENCH_NUM_EVENTS;i++)
		pbtpd->papi_val[i] += pbtpd->val2[i] - pbtpd->val1[i];
}


char *
papi_bench_template_get_vals_string(int tnum)
{
	struct Papi_Bench_Thread_Private_Data * pbtpd = &PAPI_BENCH_TPD[tnum];
	char buf[200];
	int len = 0;
	buf[0] = '\0';
	for (int i=0;i<PAPI_BENCH_NUM_EVENTS;i++)
		len += snprintf(buf + len, sizeof(buf) - len, "%s:%llu , ", PAPI_BENCH_EVENT_NAMES[i], pbtpd->papi_val[i]);
	if ((PAPI_BENCH_NUM_EVENTS > 0) && (len > 2))
		buf[len - 3] = '\0';
	return strdup(buf);
}

char *
papi_bench_template_get_total_vals_string()
{
	int threads = safe_omp_get_num_threads_external();
	long long total_papi_vals[PAPI_BENCH_NUM_EVENTS];
	char buf[200];
	int len = 0;
	int i, j;
	buf[0] = '\0';
	for (i=0;i<PAPI_BENCH_NUM_EVENTS;i++)
		total_papi_vals[i] = 0;
	for (j=0;j<threads;j++)
		for (i=0;i<PAPI_BENCH_NUM_EVENTS;i++)
			total_papi_vals[i] += PAPI_BENCH_TPD[j].papi_val[i];
	for (i=0;i<PAPI_BENCH_NUM_EVENTS;i++)
		len += snprintf(buf + len, sizeof(buf) - len, "%s:%llu , ", PAPI_BENCH_EVENT_NAMES[i], total_papi_vals[i]);
	if ((PAPI_BENCH_NUM_EVENTS > 0) && (len > 2))
		buf[len - 3] = '\0';
	return strdup(buf);
}

