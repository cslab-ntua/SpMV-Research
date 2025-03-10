#ifndef _GNU_SOURCE
	#error "Define _GNU_SOURCE at the top level to compile this library."
#endif
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <fcntl.h>
#include <dirent.h>
#include <regex.h>
#include <pthread.h>
#include <omp.h>
#include <sched.h>

#include "debug.h"
#include "string_util.h"
#include "pthread_functions.h"
#include "omp_functions.h"
#include "lock/lock_util.h"
#include "io.h"
#include "time_it.h"

#include "hardware_topology.h"
#include "topology_util.h"


static struct topohw_topology * TP = NULL;


//==========================================================================================================================================
//------------------------------------------------------------------------------------------------------------------------------------------
//-                                                           Create Topology                                                              -
//------------------------------------------------------------------------------------------------------------------------------------------
//==========================================================================================================================================


static inline
enum cache_types
string_to_cache_type(char * type_str)
{
	enum cache_types type;
	switch (type_str[0]) {
	case 'D':
		type = TOPOHW_CT_D; break;
	case 'I':
		type = TOPOHW_CT_I; break;
	case 'U':
		type = TOPOHW_CT_U; break;
	default:
		error("Unsupported cache type: %s", type_str);
		type = -1;
	};
	return type;
}
static inline
enum cache_types
num_to_cache_type(long num)
{
	enum cache_types type;
	switch (num) {
	case TOPOHW_CT_D:
		type = TOPOHW_CT_D; break;
	case TOPOHW_CT_I:
		type = TOPOHW_CT_I; break;
	case TOPOHW_CT_U:
		type = TOPOHW_CT_U; break;
	default:
		error("Unsupported cache type: %ld", num);
		type = -1;
	};
	return type;
}


static inline
void
parse_cache_data(long cpu_id_sysfs, long id_sysfs, struct topohw_cache * cache)
{
	long buf_n = 1000;
	char buf[buf_n];
	long i;

	// long level;
	snprintf(buf, buf_n, "/sys/devices/system/cpu/cpu%ld/cache/index%ld/level", cpu_id_sysfs, id_sysfs);
	cache->level = parse_file_int(buf);

	// long coherency_line_size;
	snprintf(buf, buf_n, "/sys/devices/system/cpu/cpu%ld/cache/index%ld/coherency_line_size", cpu_id_sysfs, id_sysfs);
	cache->coherency_line_size = parse_file_int(buf);

	// long number_of_sets;
	snprintf(buf, buf_n, "/sys/devices/system/cpu/cpu%ld/cache/index%ld/number_of_sets", cpu_id_sysfs, id_sysfs);
	cache->number_of_sets = parse_file_int(buf);

	// long num_shared_cpus;
	// long * shared_cpu_list;
	snprintf(buf, buf_n, "/sys/devices/system/cpu/cpu%ld/cache/index%ld/shared_cpu_map", cpu_id_sysfs, id_sysfs);
	cache->num_shared_cpus = parse_file_hex_num_list(buf, &cache->shared_cpu_list);

	// long size;
	snprintf(buf, buf_n, "/sys/devices/system/cpu/cpu%ld/cache/index%ld/size", cpu_id_sysfs, id_sysfs);
	cache->size = parse_file_int_human_format(buf);

	// char * type_str;
	snprintf(buf, buf_n, "/sys/devices/system/cpu/cpu%ld/cache/index%ld/type", cpu_id_sysfs, id_sysfs);
	cache->type_str = parse_file_string(buf);
	for (i=0;i<(long)strlen(cache->type_str);i++)
	{
		if (cache->type_str[i] == '\n')
			cache->type_str[i] = '\0';
	}
	cache->type = string_to_cache_type(cache->type_str);

	// long ways_of_associativity;
	snprintf(buf, buf_n, "/sys/devices/system/cpu/cpu%ld/cache/index%ld/ways_of_associativity", cpu_id_sysfs, id_sysfs);
	cache->ways_of_associativity = parse_file_int(buf);

	long cpu_id_rep = cache->shared_cpu_list[0];
	for (i=0;i<cache->num_shared_cpus;i++)
	{
		if (cache->shared_cpu_list[i] < cpu_id_rep)
			cpu_id_rep = cache->shared_cpu_list[i];
	}
	cache->representative_cpu_id = cpu_id_rep;
}


static inline
void
parse_cpu_data(long id_sysfs, struct topohw_cpu * cpu)
{
	long buf_n = 1000;
	char buf[buf_n];

	long num_caches;
	struct topohw_cache ** caches_ptrs;
	long * cache_id_list;

	long core_id_sysfs;
	long i;

	cpu->id_sysfs = id_sysfs;

	// long core_id_sysfs;
	snprintf(buf, buf_n, "/sys/devices/system/cpu/cpu%ld/topology/core_id", id_sysfs);
	core_id_sysfs = parse_file_int(buf);
	cpu->core_id_sysfs = core_id_sysfs;

	// long num_core_cpus;
	// long * core_cpus_list;
	snprintf(buf, buf_n, "/sys/devices/system/cpu/cpu%ld/topology/core_cpus", id_sysfs);
	cpu->num_core_cpus = parse_file_hex_num_list(buf, &cpu->core_cpus_list);

	// long physical_package_id;
	snprintf(buf, buf_n, "/sys/devices/system/cpu/cpu%ld/topology/physical_package_id", id_sysfs);
	cpu->physical_package_id = parse_file_int(buf);
	// long num_package_cpus;
	// long * package_cpus_list;      // Deprecated name: "core_siblings_list".
	snprintf(buf, buf_n, "/sys/devices/system/cpu/cpu%ld/topology/package_cpus", id_sysfs);
	cpu->num_package_cpus = parse_file_hex_num_list(buf, &cpu->package_cpus_list);

	// long num_caches;
	// struct topohw_cache * caches_ptrs;
	snprintf(buf, buf_n, "/sys/devices/system/cpu/cpu%ld/cache", id_sysfs);
	num_caches = find_numbered_files_list(buf, "index", &cache_id_list);
	cpu->cache_id_list = cache_id_list;
	caches_ptrs = (typeof(caches_ptrs)) malloc(num_caches * sizeof(*caches_ptrs));
	for (i=0;i<num_caches;i++)
		caches_ptrs[i] = (typeof(*caches_ptrs)) malloc(sizeof(**caches_ptrs));
	cpu->num_caches = num_caches;
	cpu->caches_ptrs = caches_ptrs;
	for (i=0;i<num_caches;i++)
		parse_cache_data(id_sysfs, cache_id_list[i], caches_ptrs[i]);
}


static inline
void
parse_mem_node_data(long id_sysfs, struct topohw_mem_node * mem_node)
{
	long buf_n = 1000;
	char buf[buf_n];

	long num_cpus;                 // Logical number of cpus.
	long * cpu_id_sysfs_list;

	mem_node->id_sysfs = id_sysfs;

	snprintf(buf, buf_n, "/sys/devices/system/node/node%ld", id_sysfs);
	num_cpus = find_numbered_files_list(buf, "cpu", &cpu_id_sysfs_list);
	mem_node->num_cpus = num_cpus;
	mem_node->cpu_id_sysfs_list = cpu_id_sysfs_list;
}


static inline
long
get_cpu_id_sysfs(void * A, long i)
{
	struct topohw_cpu * cpus = A;
	return cpus[i].id_sysfs;
}


static inline
long
get_core_id_sysfs(void * A, long i)
{
	struct topohw_cpu * cpus = A;
	return cpus[i].core_id_sysfs;
}


struct topohw_topology *
topohw_get_topology()
{
	static uint64_t create_lock = 0;

	struct topohw_topology * tp = TP;

	struct topohw_cpu * cpus, * cpu;
	struct topohw_cache * cache;
	long * cpu_id_sysfs_list;
	long cpu_id_sysfs_max;
	long core_id_sysfs_max;

	struct topohw_cache_class * cache_classes, * cache_class;

	long num_mem_nodes;
	struct topohw_mem_node * mem_nodes, * mem_node;
	long * mem_node_id_sysfs_list;
	long mem_node_id_sysfs_max;

	uint64_t prev;
	long num_cpus;
	long cpu_id, core_id;
	long num_cache_classes;
	long i, j, k, l;

	if (tp != NULL)
		return tp;

	/* Use a lock to protect against multiple accesses. */
	prev = __atomic_exchange_n(&create_lock, 1, __ATOMIC_SEQ_CST);
	if (prev == 1)
	{
		while (1)
		{
			if (__atomic_load_n(&TP, __ATOMIC_RELAXED) != NULL)
				return TP;
			lock_cpu_relax();
		}
	}
	// printf("topohw %d level=%d\n", omp_get_thread_num(), omp_get_level());

	tp = (typeof(tp)) malloc(sizeof(*tp));

	//----------------------------------------------------------------------------------------------------------------------------------
	//- CPUs
	//----------------------------------------------------------------------------------------------------------------------------------

	num_cpus = find_numbered_files_list("/sys/devices/system/cpu", "cpu", &cpu_id_sysfs_list);
	cpus = (typeof(cpus)) malloc(num_cpus * sizeof(*cpus));
	tp->num_cpus = num_cpus;
	tp->cpu_id_sysfs_list = cpu_id_sysfs_list;
	tp->cpus = cpus;
	cpu_id_sysfs_max = -1;
	core_id_sysfs_max = -1;
	for (i=0;i<tp->num_cpus;i++)
	{
		cpu = &cpus[i];
		parse_cpu_data(cpu_id_sysfs_list[i], cpu);
	}
	for (i=0;i<tp->num_cpus;i++)
	{
		cpu = &cpus[i];
		if (cpu_id_sysfs_list[i] > cpu_id_sysfs_max)
			cpu_id_sysfs_max = cpu_id_sysfs_list[i];
		if (cpu->core_id_sysfs > core_id_sysfs_max)
			core_id_sysfs_max = cpu->core_id_sysfs;
	}
	tp->cpu_id_sysfs_max = cpu_id_sysfs_max;
	tp->core_id_sysfs_max = core_id_sysfs_max;

	/* Contiguous CPU ids and core ids.
	 *
	 * CPU core ids can be non-contiguous.
	 * This usually results from one of two things:
	 *     - The CPU actually has more cores, and the firmware is disabling some of them (or the manufacturer disabled some).
	 *     - The firmware is leaving space in the various tables that the OS reads this data from for the possibility of hot-plugging bigger CPU's.
	 * We need to fix them.
	 */
	tp->num_cpus = get_random_to_contiguous_numbering_translation(cpus, tp->num_cpus, cpu_id_sysfs_max, get_cpu_id_sysfs, &tp->dict_cpu_id_sysfs_to_cpu_id);
	if (tp->num_cpus != num_cpus)
		error("cpus with same cpu_id");
	tp->num_cores = get_random_to_contiguous_numbering_translation(cpus, tp->num_cpus, core_id_sysfs_max, get_core_id_sysfs, &tp->dict_core_id_sysfs_to_core_id);
	/* Translate CPU data, from sysfs to contiguous numbers. */
	for (i=0;i<tp->num_cpus;i++)
	{
		cpu = &cpus[i];
		cpu->id = i;
		cpu->core_id = tp->dict_core_id_sysfs_to_core_id[cpu->core_id_sysfs];
		translate(cpu->core_cpus_list, cpu->num_core_cpus, tp->dict_cpu_id_sysfs_to_cpu_id, cpu->core_cpus_list);
		for (j=0;j<cpu->num_caches;j++)
		{
			cache = cpu->caches_ptrs[j];
			translate(cache->shared_cpu_list, cache->num_shared_cpus, tp->dict_cpu_id_sysfs_to_cpu_id, cache->shared_cpu_list);
		}
	}

	//----------------------------------------------------------------------------------------------------------------------------------
	//- Cache Classes / Cache Nodes
	//----------------------------------------------------------------------------------------------------------------------------------

	/* Find max number of cache classes.
	 * Search all CPUs, in case of heterogeneous cores.
	 */
	long max_cache_level = 0;
	long max_common_cache_level = -1;
	for (i=0;i<tp->num_cpus;i++)
	{
		cpu = &cpus[i];
		long max = 0;
		cache = cpu->caches_ptrs[0];
		for (j=0;j<cpu->num_caches;j++)
		{
			cache = cpu->caches_ptrs[j];
			if (cache->level > max)
				max = cache->level;
		}
		if (max > max_cache_level)
			max_cache_level = cache->level;
		if (max_common_cache_level < 0 || max < max_common_cache_level)
			max_common_cache_level = max;
	}
	tp->max_cache_level = max_cache_level;
	tp->max_common_cache_level = max_common_cache_level;
	long * num_cpus_per_cache_class;
	num_cpus_per_cache_class = (typeof(num_cpus_per_cache_class)) malloc(TOPOHW_NUM_CACHE_TYPES*(max_cache_level+1) * sizeof(*num_cpus_per_cache_class));
	memset(num_cpus_per_cache_class, 0, TOPOHW_NUM_CACHE_TYPES*(max_cache_level+1)*sizeof(*num_cpus_per_cache_class));
	for (i=0;i<tp->num_cpus;i++)
	{
		cpu = &cpus[i];
		for (j=0;j<cpu->num_caches;j++)
		{
			cache = cpu->caches_ptrs[j];
			num_cpus_per_cache_class[TOPOHW_NUM_CACHE_TYPES*cache->level + cache->type]++;
		}
	}
	num_cache_classes = 0;
	for (i=0;i<max_cache_level+1;i++)
	{
		for (j=0;j<TOPOHW_NUM_CACHE_TYPES;j++)
		{
			num_cpus = num_cpus_per_cache_class[TOPOHW_NUM_CACHE_TYPES*i+j];
			if (num_cpus > 0)
			{
				// printf("i=%ld j=%ld num_cpus_per_cache_class[TOPOHW_NUM_CACHE_TYPES*i+j]=%ld\n", i, j, num_cpus_per_cache_class[TOPOHW_NUM_CACHE_TYPES*i+j]);
				num_cache_classes++;
			}
		}
	}
	tp->num_cache_classes = num_cache_classes;
	tp->cache_classes = (typeof(cache_classes)) malloc(tp->num_cache_classes * sizeof(*cache_classes));
	k = 0;
	for (i=0;i<max_cache_level+1;i++)
	{
		for (j=0;j<TOPOHW_NUM_CACHE_TYPES;j++)
		{
			num_cpus = num_cpus_per_cache_class[TOPOHW_NUM_CACHE_TYPES*i+j];
			if (num_cpus > 0)
			{
				cache_class = &tp->cache_classes[k];
				cache_class->id = k;
				cache_class->level = i;
				cache_class->type = num_to_cache_type(j);   // Use function for error checking.
				cache_class->num_cpus = 0;
				cache_class->cpu_id_list = (typeof(cache_class->cpu_id_list)) malloc(num_cpus * sizeof(*cache_class->cpu_id_list));
				k++;
			}
		}
	}
	free(num_cpus_per_cache_class);

	/* Find cpus and caches for each cache class. */
	long ** dict_cache_class_is_rep_cpu;
	dict_cache_class_is_rep_cpu = (typeof(dict_cache_class_is_rep_cpu)) malloc(tp->num_cache_classes * sizeof(*dict_cache_class_is_rep_cpu));
	for (i=0;i<tp->num_cache_classes;i++)
	{
		dict_cache_class_is_rep_cpu[i] = (typeof(*dict_cache_class_is_rep_cpu)) malloc(tp->num_cpus * sizeof(**dict_cache_class_is_rep_cpu));
		memset(dict_cache_class_is_rep_cpu[i], 0, tp->num_cpus*sizeof(**dict_cache_class_is_rep_cpu));
	}
	for (i=0;i<tp->num_cpus;i++)
	{
		cpu = &cpus[i];
		for (j=0;j<cpu->num_caches;j++)
		{
			cache = cpu->caches_ptrs[j];
			cache_class = &tp->cache_classes[0];
			for (k=0;k<tp->num_cache_classes;k++)
			{
				cache_class = &tp->cache_classes[k];
				if (cache->level == cache_class->level && cache->type == cache_class->type)
					break;
			}
			cache->cache_class_id = k;
			dict_cache_class_is_rep_cpu[k][cache->representative_cpu_id] = 1;
			cache_class->cpu_id_list[cache_class->num_cpus] = cpu->id;
			cache_class->num_cpus++;
		}
	}
	for (i=0;i<tp->num_cache_classes;i++)
	{
		cache_class = &tp->cache_classes[i];
		cache_class->num_caches = 0;
		for (j=0;j<tp->num_cpus;j++)
		{
			if (dict_cache_class_is_rep_cpu[i][j])
				cache_class->num_caches++;
		}
		cache_class->caches = (typeof(cache_class->caches)) malloc(cache_class->num_caches * sizeof(*cache_class->caches));
		cache_class->num_caches = 0;
		for (j=0;j<tp->num_cpus;j++)
		{
			if (dict_cache_class_is_rep_cpu[i][j])
			{
				cpu = &cpus[j];
				cache = cpu->caches_ptrs[0];
				for (k=0;k<cpu->num_caches;k++)
				{
					cache = cpu->caches_ptrs[k];
					if (cache->cache_class_id == i)
						break;
				}
				cache->cache_class_local_cache_id = cache_class->num_caches;
				cache_class->caches[cache_class->num_caches] = *cache;
				cache_class->num_caches++;
			}
		}
	}

	/* Free cpu caches and link them to central instances in cache_classes. */
	for (i=0;i<tp->num_cache_classes;i++)
	{
		cache_class = &tp->cache_classes[i];
		for (j=0;j<cache_class->num_caches;j++)
		{
			cache = &cache_class->caches[j];
			for (k=0;k<cache->num_shared_cpus;k++)
			{
				cpu = &tp->cpus[cache->shared_cpu_list[k]];
				for (l=0;l<cpu->num_caches;l++)
				{
					if (cpu->caches_ptrs[l]->cache_class_id == i)
					{
						free(cpu->caches_ptrs[l]);
						cpu->caches_ptrs[l] = cache;
						break;
					}
				}
			}
		}
	}

	//----------------------------------------------------------------------------------------------------------------------------------
	//- Memory Nodes
	//----------------------------------------------------------------------------------------------------------------------------------

	num_mem_nodes = find_numbered_files_list("/sys/devices/system/node", "node", &mem_node_id_sysfs_list);
	mem_nodes = (typeof(mem_nodes)) malloc(num_mem_nodes * sizeof(*mem_nodes));
	mem_node_id_sysfs_max = -1;
	for (i=0;i<num_mem_nodes;i++)
	{
		parse_mem_node_data(mem_node_id_sysfs_list[i], &mem_nodes[i]);
		if (mem_node_id_sysfs_list[i] > mem_node_id_sysfs_max)
			mem_node_id_sysfs_max = mem_node_id_sysfs_list[i];
	}
	tp->mem_node_id_sysfs_max = mem_node_id_sysfs_max;
	/* Remove empty memory nodes.
	 * There are actually machines with empty memory nodes (no CPUs).
	 */
	j = 0;
	for (i=0;i<num_mem_nodes;i++)
	{
		mem_node = &mem_nodes[i];
		if (mem_node->num_cpus == 0)
			continue;
		mem_node_id_sysfs_list[j] = mem_node_id_sysfs_list[i];
		mem_nodes[j] = mem_nodes[i];
		j++;
	}
	num_mem_nodes = j;
	tp->mem_node_id_sysfs_list = mem_node_id_sysfs_list;
	tp->num_mem_nodes = num_mem_nodes;
	tp->mem_nodes = mem_nodes;
	/* Translate mem_node data, from sysfs to contiguous numbers. */
	for (i=0;i<num_mem_nodes;i++)
	{
		mem_node = &mem_nodes[i];
		mem_node->id = i;
		mem_node->cpu_id_list = (typeof(mem_node->cpu_id_list)) malloc(mem_node->num_cpus * sizeof(*mem_node->cpu_id_list));
		translate(mem_node->cpu_id_sysfs_list, mem_node->num_cpus, tp->dict_cpu_id_sysfs_to_cpu_id, mem_node->cpu_id_list);
	}
	/* Find the number of cores. */
	long * dict;
	dict = (typeof(dict)) malloc((cpu_id_sysfs_max+1) * sizeof(*dict));
	for (i=0;i<num_mem_nodes;i++)
	{
		mem_node = &mem_nodes[i];
		for (j=0;j<cpu_id_sysfs_max+1;j++)
			dict[j] = -1;
		k = 0;
		for (j=0;j<mem_node->num_cpus;j++)
		{
			cpu_id = mem_node->cpu_id_list[j];
			core_id = tp->cpus[cpu_id].core_id;
			if (dict[core_id] < 0)
				k++;
			dict[core_id] = 1;
		}
		mem_node->num_cores = k;
	}
	/* Fill CPU memory node info. */
	for (i=0;i<num_mem_nodes;i++)
	{
		mem_node = &mem_nodes[i];
		for (j=0;j<mem_node->num_cpus;j++)
		{
			cpu_id = mem_node->cpu_id_list[j];
			cpu = &cpus[cpu_id];
			cpu->mem_node_id = i;
		}
	}

	__atomic_store_n(&TP, tp, __ATOMIC_SEQ_CST);
	return tp;
}


//==========================================================================================================================================
//------------------------------------------------------------------------------------------------------------------------------------------
//-                                                         Exported Functions                                                             -
//------------------------------------------------------------------------------------------------------------------------------------------
//==========================================================================================================================================


//==========================================================================================================================================
//= CPUs
//==========================================================================================================================================


long
topohw_get_core_num(long cpu_id)
{
	struct topohw_topology * tp = topohw_get_topology();
	long core_id = tp->cpus[cpu_id].core_id;
	return core_id;
}


long
topohw_get_num_cpus()
{
	struct topohw_topology * tp = topohw_get_topology();
	long num_cpus = tp->num_cpus;
	return num_cpus;
}


long
topohw_get_num_cores()
{
	struct topohw_topology * tp = topohw_get_topology();
	long num_cores = tp->num_cores;
	return num_cores;
}


long
topohw_get_num_core_cpus(long cpu_id)
{
	struct topohw_topology * tp = topohw_get_topology();
	return tp->cpus[cpu_id].num_core_cpus;
}


long
topohw_get_core_cpus_list(long cpu_id, const long ** core_cpus_list_ret)
{
	struct topohw_topology * tp = topohw_get_topology();
	long num_core_cpus = tp->cpus[cpu_id].num_core_cpus;
	long * core_cpus_list = tp->cpus[cpu_id].core_cpus_list;
	if (core_cpus_list_ret != NULL)
		*core_cpus_list_ret = core_cpus_list;
	return num_core_cpus;
}


long
topohw_get_mem_node_num(long cpu_id)
{
	struct topohw_topology * tp = topohw_get_topology();
	return tp->cpus[cpu_id].mem_node_id;
}


//==========================================================================================================================================
//= Cache Classes
//==========================================================================================================================================


static inline
struct topohw_cache_class *
get_cache_class(long level, enum cache_types type)
{
	struct topohw_topology * tp = topohw_get_topology();
	struct topohw_cache_class * cache_class;
	long i;
	for (i=0;i<tp->num_cache_classes;i++)
	{
		cache_class = &tp->cache_classes[i];
		if (cache_class->level == level)
		{
			if (cache_class->type == type)
			{
				return cache_class;
			}
		}
	}
	return NULL;
}


long
topohw_get_cache_class_size(long level, enum cache_types type)
{
	struct topohw_cache_class * cache_class = get_cache_class(level, type);
	long size = 0;
	long i;
	if (cache_class == NULL)
		return 0;
	for (i=0;i<cache_class->num_caches;i++)
	{
		size += cache_class->caches[i].size;
	}
	return size;
}


long
topohw_get_cache_class_num_caches(long level, enum cache_types type)
{
	struct topohw_cache_class * cache_class = get_cache_class(level, type);
	if (cache_class == NULL)
		return 0;
	return cache_class->num_caches;
}


//==========================================================================================================================================
//= Caches
//==========================================================================================================================================


static inline
struct topohw_cache *
get_cache(long cpu_id, long level, enum cache_types type)
{
	struct topohw_topology * tp = topohw_get_topology();
	struct topohw_cpu * cpu;
	struct topohw_cache * cache;
	long i;
	cpu = &tp->cpus[cpu_id];
	for (i=0;i<cpu->num_caches;i++)
	{
		cache = cpu->caches_ptrs[i];
		if (cache->level == level)
		{
			if (cache->type == type)
			{
				return cache;
			}
		}
	}
	return NULL;
}


long
topohw_get_cache_size(long cpu_id, long level, enum cache_types type)
{
	struct topohw_cache * cache = get_cache(cpu_id, level, type);
	return (cache != NULL) ? cache->size : 0;
}


long
topohw_get_cache_cpus(long cpu_id, long level, enum cache_types type, const long ** cpu_id_list_ret)
{
	struct topohw_cache * cache = get_cache(cpu_id, level, type);
	if (cache != NULL)
	{
		if (cpu_id_list_ret != NULL)
			*cpu_id_list_ret = cache->shared_cpu_list;
		return cache->num_shared_cpus;
	}
	else
	{
		if (cpu_id_list_ret != NULL)
			*cpu_id_list_ret = NULL;
		return 0;
	}
}


//==========================================================================================================================================
//= Memory Nodes
//==========================================================================================================================================


long
topohw_get_num_mem_nodes()
{
	struct topohw_topology * tp = topohw_get_topology();
	return tp->num_mem_nodes;
}


long
topohw_get_num_mem_node_cpus(long cpu_id)
{
	struct topohw_topology * tp = topohw_get_topology();
	long mem_node_id = tp->cpus[cpu_id].mem_node_id;
	return tp->mem_nodes[mem_node_id].num_cpus;
}


long
topohw_get_num_mem_node_cores(long cpu_id)
{
	struct topohw_topology * tp = topohw_get_topology();
	long mem_node_id = tp->cpus[cpu_id].mem_node_id;
	return tp->mem_nodes[mem_node_id].num_cores;
}


long
topohw_get_mem_node_cpus_list(long cpu_id, const long ** cpu_id_list_ret)
{
	struct topohw_topology * tp = topohw_get_topology();
	long mem_node_id = tp->cpus[cpu_id].mem_node_id;
	if (cpu_id_list_ret != NULL)
		*cpu_id_list_ret = tp->mem_nodes[mem_node_id].cpu_id_list;
	return tp->mem_nodes[mem_node_id].num_cpus;
}


//==========================================================================================================================================
//= Print Topology
//==========================================================================================================================================


void
topohw_print_topology()
{
	struct topohw_topology * tp = topohw_get_topology();
	struct topohw_cpu * cpu;
	struct topohw_cache * cache;
	struct topohw_cache_class * cache_class;
	struct topohw_mem_node * mem_node;
	long i, j, k;

	printf("\nCPUs\n");
	printf("num_cpus=%ld, num_cores=%ld, cpu_id_sysfs_max=%ld\n", tp->num_cpus, tp->num_cores, tp->cpu_id_sysfs_max);
	for (i=0;i<tp->num_cpus;i++)
	{
		cpu = &tp->cpus[i];
		printf("    id_sysfs=%ld, id=%ld, core_id_sysfs=%ld, core_id=%ld, mem_node_id=%ld\n", cpu->id_sysfs, cpu->id, cpu->core_id_sysfs, cpu->core_id, cpu->mem_node_id);

		// Hyperthreading
		printf("        num_core_cpus=%ld\n", cpu->num_core_cpus);
		printf("            ");
		for (j=0;j<cpu->num_core_cpus;j++)
		{
			printf("%ld ", cpu->core_cpus_list[j]);
		}
		printf("\n");

		printf("        caches:\n");
		for (j=0;j<cpu->num_caches;j++)
		{
			cache = cpu->caches_ptrs[j];
			printf("            level=%ld, type_str=%s, type=%d, cache_class_id=%ld, cache_class_local_cache_id=%ld, coherency_line_size=%ld, number_of_sets=%ld, size=%ld, ways_of_associativity=%ld\n",
					cache->level, cache->type_str, cache->type, cache->cache_class_id, cache->cache_class_local_cache_id, cache->coherency_line_size, cache->number_of_sets, cache->size, cache->ways_of_associativity);
			printf("            num_shared_cpus=%ld, representative_cpu_id=%ld\n", cache->num_shared_cpus, cache->representative_cpu_id);
			printf("                ");
			for (k=0;k<cache->num_shared_cpus;k++)
			{
				printf("%ld ", cache->shared_cpu_list[k]);
			}
			printf("\n");
		}
	}

	printf("\nCache Nodes\n");
	printf("max_cache_level=%ld, max_common_cache_level=%ld, num_cache_classes=%ld\n", tp->max_cache_level, tp->max_common_cache_level, tp->num_cache_classes);
	for (i=0;i<tp->num_cache_classes;i++)
	{
		cache_class = &tp->cache_classes[i];
		printf("    id=%ld, level=%ld, type=%d, num_cpus=%ld, num_caches=%ld\n", cache_class->id, cache_class->level, cache_class->type, cache_class->num_cpus, cache_class->num_caches);
		printf("        caches:\n");
		for (j=0;j<cache_class->num_caches;j++)
		{
			cache = &cache_class->caches[j];
			printf("            level=%ld, type_str=%s, type=%d, cache_class_id=%ld, cache_class_local_cache_id=%ld, coherency_line_size=%ld, number_of_sets=%ld, size=%ld, ways_of_associativity=%ld\n",
					cache->level, cache->type_str, cache->type, cache->cache_class_id, cache->cache_class_local_cache_id, cache->coherency_line_size, cache->number_of_sets, cache->size, cache->ways_of_associativity);
			printf("            num_shared_cpus=%ld, representative_cpu_id=%ld\n", cache->num_shared_cpus, cache->representative_cpu_id);
			printf("                ");
			for (k=0;k<cache->num_shared_cpus;k++)
			{
				printf("%ld ", cache->shared_cpu_list[k]);
			}
			printf("\n");
		}
	}

	printf("\nMemory Nodes\n");
	printf("num_mem_nodes=%ld, mem_node_id_sysfs_max=%ld\n", tp->num_mem_nodes, tp->mem_node_id_sysfs_max);
	for (i=0;i<tp->num_mem_nodes;i++)
	{
		mem_node = &tp->mem_nodes[i];
		printf("    id_sysfs=%ld, id=%ld, num_cpus=%ld, num_cores=%ld\n", mem_node->id_sysfs, mem_node->id, mem_node->num_cpus, mem_node->num_cores);
		printf("        ");
		for (j=0;j<mem_node->num_cpus;j++)
		{
			printf("%ld ", mem_node->cpu_id_sysfs_list[j]);
		}
		printf("\n");
	}

}

