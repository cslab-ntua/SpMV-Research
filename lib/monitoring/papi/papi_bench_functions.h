#ifndef PAPI_BENCH_FUNCTIONS_H
#define PAPI_BENCH_FUNCTIONS_H

#include <papi.h>

#include "macros/cpp_defines.h"
#include "macros/constants.h"
#include "papi_functions.h"


#define PAPI_BENCH_MAX_EVENTS 8


extern int PAPI_BENCH_NUM_EVENTS;
extern char ** PAPI_BENCH_EVENT_NAMES;

void papi_bench_initialization();
void papi_bench_initialize_eventset(int * eventset);
void papi_bench_init_with_cli_args(int argc, char **argv);
void papi_bench_cleanup_eventset(int * eventset);
void papi_bench_shutdown();


//===========================================================================================================================================
//= OMP Benchmark Template
//===========================================================================================================================================


struct Papi_Bench_Thread_Private_Data {
	int eventset;
	long long val1[PAPI_BENCH_MAX_EVENTS];
	long long val2[PAPI_BENCH_MAX_EVENTS];
	long long papi_val[PAPI_BENCH_MAX_EVENTS];

	char padding[0] __attribute__ ((aligned (CACHE_LINE_SIZE)));
} __attribute__ ((aligned (CACHE_LINE_SIZE)));


extern struct Papi_Bench_Thread_Private_Data * PAPI_BENCH_TPD;

void papi_bench_template_init(int argc, char **argv);
void papi_bench_template_shutdown();
void papi_bench_template_read_start(int tnum);
void papi_bench_template_read_end(int tnum);
char * papi_bench_template_get_vals_string(int tnum);
char * papi_bench_template_get_total_vals_string();


#endif /* PAPI_BENCH_FUNCTIONS_H */

