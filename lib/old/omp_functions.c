#ifndef _GNU_SOURCE
	#define _GNU_SOURCE
#endif

#include <unistd.h>
#include <string.h>
#include <omp.h>
#include <sys/types.h>  // In PowerPC the gettid() function in included this way.

#include "debug.h"
#include "pthread_functions.h"
#include "macros/cpp_defines.h"

#include "omp_functions.h"


// Number of threads of the current parallel region.
// The omp_get_num_threads routine returns the number of threads in the team that is executing the parallel region to which the routine region binds.
// If called from the sequential part of a program, this routine returns 1.
int
safe_omp_get_num_threads_internal()
{
	int num_threads;
	num_threads = omp_get_num_threads();
	return num_threads;
}


// Number of threads of a following parallel region.
int
safe_omp_get_num_threads_external()
{
	int num_threads;
	#pragma omp parallel
	{
		#pragma omp single
		num_threads = omp_get_num_threads();
	}
	return num_threads;
}


int
safe_omp_get_num_threads()
{
	if (omp_get_level() > 0)
		return safe_omp_get_num_threads_internal();
	else
		return safe_omp_get_num_threads_external();
}


int
safe_omp_get_thread_num_initial()
{
	int tnum;
	if (omp_get_level() > 1)
		tnum = omp_get_ancestor_thread_num(1);
	else
		tnum = omp_get_thread_num();
	return tnum;
}


int
get_affinity_from_GOMP_CPU_AFFINITY(int tnum)
{
	char * gomp_aff_str = getenv("GOMP_CPU_AFFINITY");
	long len = strlen(gomp_aff_str);
	long pos = 0;
	int aff = -1;
	int n1, n2;
	long i;
	for (i=0;i<len;)
	{
		n1 = atoi(&gomp_aff_str[i]);
		if (pos == tnum)
		{
			aff = n1;
			break;
		}
		while ((i < len) && (gomp_aff_str[i] != ',') && (gomp_aff_str[i] != '-'))
			i++;
		if (i+1 >= len)
			break;
		if (gomp_aff_str[i] == ',')
		{
			pos++;
			i++;
		}
		else
		{
			i++;
			n2 = atoi(&gomp_aff_str[i]);
			if (n2 < n1)
				error("Bad affinity string format.");
			if (pos + n2 - n1 >= tnum)
			{
				aff = n1 + tnum - pos;
				break;
			}
			pos += n2 - n1 + 1;
			while ((i < len) && (gomp_aff_str[i] != ','))
				i++;
			i++;
			if (i >= len)
				break;
		}
	}
	if (aff < 0)
		error("Bad affinity string format.");
	// printf("%d: %d\n", tnum, aff);
	return aff;
}


void
print_affinity()
{
	pthread_t tid = pthread_self();
	long buf_n = 10*1024;
	char buf[buf_n];
	int * cpus;
	int i, j=0,  count=0;
	count = get_affinity(tid, &cpus);
	// j += snprintf(buf+j, buf_n-j, "pid %d tid %d pthread_id 0x%lx omp_id %d bound to OS proc set {", getpid(), gettid(), tid, safe_omp_get_thread_num_initial());
	j += snprintf(buf+j, buf_n-j, "pid %d pthread_id 0x%lx omp_id %d bound to OS proc set {", getpid(), tid, safe_omp_get_thread_num_initial());
	for (i=0;i<count;i++)
		j += snprintf(buf+j, buf_n-j, "%d, ", cpus[i]);
	j -= 2;
	snprintf(buf+j, buf_n-j, "}\n");
	printf("%s", buf);
	free(cpus);
}

