#ifndef _GNU_SOURCE
	#define _GNU_SOURCE
#endif
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <pthread.h>
#include <omp.h>

#include "debug.h"
#include "string_util.h"
#include "pthread_functions.h"
#include "omp_functions.h"
#include "io.h"

#include "topology.h"


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//------------------------------------------------------------------------------------------------------------------------------------------
//-                                                             Utilities                                                                  -
//------------------------------------------------------------------------------------------------------------------------------------------
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


static
long
hex_chars_to_bin_chars(char * str_hex, long N, char ** str_bin_out)
{
	long buf_n = N * 4 + 1;
	char * buf;
	char c;
	long i, j;
	buf = (typeof(buf)) malloc(buf_n * sizeof(*buf));
	for (i=0,j=0;i<N;i++)
	{
		c = str_hex[i];
		j += str_char_hex_to_bin_unsafe(c, buf + j);
	}
	*str_bin_out = buf;
	return j;
}


static
long
parse_file_int(const char * filename)
{
	char * str;
	long num;
	read_sysfs_file(filename, &str);
	num = atol(str);
	free(str);
	return num;
}


static
long
bin_string_to_int_list(char * str_one_hot, long N, long ** num_list_out)
{
	long len;
	long * num_list;
	long i, j;
	len = strnlen(str_one_hot, N);
	num_list = (typeof(num_list)) malloc(len * sizeof(*num_list));
	for (i=len-1,j=0;i>=0;i--)
	{
		if (str_one_hot[i] == '1')
			num_list[j++] = len - 1 - i;
	}
	*num_list_out = num_list;
	return j;
}


static
long
parse_file_cpu_list_hex(const char * filename, long ** num_list_out)
{
	char * str_hex;
	char * str_bin;
	long * num_list;
	long len;
	len = read_sysfs_file(filename, &str_hex);
	len = hex_chars_to_bin_chars(str_hex, len, &str_bin);
	len = bin_string_to_int_list(str_bin, len, &num_list);
	free(str_hex);
	free(str_bin);
	if (num_list_out != NULL)
		*num_list_out = num_list;
	else
		free(num_list);
	return len;
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//------------------------------------------------------------------------------------------------------------------------------------------
//-                                                         Exported Functions                                                             -
//------------------------------------------------------------------------------------------------------------------------------------------
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


long
topo_get_core_num_logical()
{
	static __thread long t_storage = -1;
	long num_cores;
	int * cores;
	long core;
	if (t_storage >= 0)
		return t_storage;
	num_cores = get_affinity(pthread_self(), &cores);
	if ((num_cores != 1) && (omp_get_level() == 0))   // Openmp might not have been initialized yet, start a parallel region to enforce pinning.
	{
		#pragma omp parallel
		{
			__attribute__((unused)) volatile int i=0;
		}
		num_cores = get_affinity(pthread_self(), &cores);
	}
	if (num_cores != 1)
		error("thread is not pinned to a specific core, can't define node");
	core = cores[0];
	free(cores);
	t_storage = core;
	return core;
}


long
topo_get_core_num_physical()
{
	static __thread long t_storage = -1;
	long buf_n = 1000;
	char buf[buf_n];
	long * thread_siblings;
	long core;
	if (t_storage >= 0)
		return t_storage;
	core = topo_get_core_num_logical();
	snprintf(buf, buf_n, "/sys/devices/system/cpu/cpu%ld/topology/thread_siblings", core);
	parse_file_cpu_list_hex(buf, &thread_siblings);
	core = thread_siblings[0];
	free(thread_siblings);
	return core;
}


long
topo_get_num_cores_logical()
{
	static __thread long t_storage = -1;
	long buf_n = 1000;
	char buf[buf_n];
	long i;
	if (t_storage >= 0)
		return t_storage;
	i = 0;
	while (1)
	{
		snprintf(buf, buf_n, "/sys/devices/system/cpu/cpu%ld", i);
		if (!stat_isdir(buf))
			break;
		// printf("%s\n", buf);
		i++;
	}
	t_storage = i;
	return i;
}


long
topo_get_num_cores_physical()
{
	static __thread long t_storage = -1;
	long num_cores, threads_per_core;
	if (t_storage >= 0)
		return t_storage;
	num_cores = topo_get_num_cores_logical();
	if (num_cores <= 0)
		return 0;
	threads_per_core = parse_file_cpu_list_hex("/sys/devices/system/cpu/cpu0/topology/thread_siblings", NULL);
	t_storage = num_cores / threads_per_core;
	return t_storage;
}


long
topo_get_node_num()
{
	static __thread long t_storage = -1;
	long buf_n = 1000;
	char buf[buf_n];
	long core;
	long node_num;
	if (t_storage >= 0)
		return t_storage;
	core = topo_get_core_num_logical();
	snprintf(buf, buf_n, "/sys/devices/system/cpu/cpu%ld/topology/physical_package_id", core);
	node_num = parse_file_int(buf);
	t_storage = node_num;
	return node_num;
}


long
topo_get_node_cores_list(long ** cores_out)
{
	long buf_n = 1000;
	char buf[buf_n];
	long core;
	core = topo_get_core_num_logical();
	snprintf(buf, buf_n, "/sys/devices/system/cpu/cpu%ld/topology/core_siblings", core);
	return parse_file_cpu_list_hex(buf, cores_out);
}


long
topo_get_num_node_cores_logical()
{
	static __thread long t_storage = -1;
	long buf_n = 1000;
	char buf[buf_n];
	long num_cores, core;
	if (t_storage >= 0)
		return t_storage;
	core = topo_get_core_num_logical();
	snprintf(buf, buf_n, "/sys/devices/system/cpu/cpu%ld/topology/core_siblings", core);
	num_cores = parse_file_cpu_list_hex(buf, NULL);
	t_storage = num_cores;
	return num_cores;
}


long
topo_get_num_node_cores_physical()
{
	static __thread long t_storage = -1;
	long buf_n = 1000;
	char buf[buf_n];
	long core;
	long num_cores, threads_per_core;
	if (t_storage >= 0)
		return t_storage;
	core = topo_get_core_num_logical();
	snprintf(buf, buf_n, "/sys/devices/system/cpu/cpu%ld/topology/thread_siblings", core);
	threads_per_core = parse_file_cpu_list_hex(buf, NULL);
	num_cores = topo_get_num_node_cores_logical();
	t_storage = num_cores / threads_per_core;
	return t_storage;
}


long
topo_get_num_nodes()
{
	static __thread long t_storage = -1;
	long num_cores, num_node_cores;
	if (t_storage >= 0)
		return t_storage;
	num_cores = topo_get_num_cores_logical();
	num_node_cores = topo_get_num_node_cores_logical();
	t_storage = num_cores / num_node_cores;
	return t_storage;
}

