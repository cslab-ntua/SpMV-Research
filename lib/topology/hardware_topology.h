#ifndef HARDWARE_TOPOLOGY_H
#define HARDWARE_TOPOLOGY_H

#include "macros/cpp_defines.h"


enum cache_types {
	TOPOHW_CT_D = 0,
	TOPOHW_CT_I,
	TOPOHW_CT_U,

	TOPOHW_NUM_CACHE_TYPES
};


struct topohw_cache {
	long level;
	char * type_str;
	enum cache_types type;
	long cache_class_id;
	long cache_class_local_cache_id;

	long coherency_line_size;
	long number_of_sets;

	long num_shared_cpus;
	long representative_cpu_id;
	long * shared_cpu_list;

	long size;

	long ways_of_associativity;
};


/* Cache Class
 */
struct topohw_cache_class {
	long id;
	long level;
	enum cache_types type;
	long num_cpus;
	long * cpu_id_list;
	long num_caches;
	struct topohw_cache * caches;
};


/* CPU
 * /sys/devices/system/cpu/cpuX
 */
struct topohw_cpu {
	long id_sysfs;                           // Logical core id, as reported by sysfs.
	long id;                                 // Logical core id, corrected to contiguous.
	long core_id_sysfs;                      // Physical core id, as reported by sysfs.
	long core_id;                            // Physical core id, corrected to contiguous.

	// Caches
	long num_caches;
	long * cache_id_list;
	struct topohw_cache ** caches_ptrs;

	// Hyperthreading
	long num_core_cpus;
	long * core_cpus_list;                   // Deprecated name: "thread_siblings_list".

	// Package cpus
	long physical_package_id;
	long num_package_cpus;
	long * package_cpus_list;                // Deprecated name: "core_siblings_list".

	// Memory node
	long mem_node_id;
};


/* Memory Node
 * /sys/devices/system/node/nodeX
 */
struct topohw_mem_node {
	long id_sysfs;
	long id;

	long num_cpus;
	long num_cores;
	long * cpu_id_sysfs_list;
	long * cpu_id_list;
};


struct topohw_topology {
	long * dict_cpu_id_sysfs_to_cpu_id;      // Transform from sysfs to contiguous numbers.
	long * dict_core_id_sysfs_to_core_id;    // Transform from sysfs to contiguous numbers.

	long num_cpus;                           // Logical number of cpus.
	long num_cores;                          // Physical number of cpus (cores).
	long cpu_id_sysfs_max;
	long core_id_sysfs_max;
	long * cpu_id_sysfs_list;
	struct topohw_cpu * cpus;

	long max_cache_level;
	long max_common_cache_level;
	long num_cache_classes;   // Cache class is a combination of cache level and cache type (Data, Instruction, Unified).
	struct topohw_cache_class * cache_classes;

	long num_mem_nodes;
	long mem_node_id_sysfs_max;
	long * mem_node_id_sysfs_list;
	struct topohw_mem_node * mem_nodes;
};


//==========================================================================================================================================
//------------------------------------------------------------------------------------------------------------------------------------------
//-                                                             Functions                                                                  -
//------------------------------------------------------------------------------------------------------------------------------------------
//==========================================================================================================================================


/* We use 'num' instead of 'id' in function names to be consistent with openmp functions.
 */


struct topohw_topology * topohw_get_topology();


//==========================================================================================================================================
//= CPUs
//==========================================================================================================================================


long topohw_get_core_num(long cpu_id);
long topohw_get_num_cpus();
long topohw_get_num_cores();
long topohw_get_num_core_cpus(long cpu_id);
long topohw_get_core_cpus_list(long cpu_id, const long ** core_cpus_list_ret);
long topohw_get_mem_node_num(long cpu_id);


//==========================================================================================================================================
//= Cache Classes
//==========================================================================================================================================


long topohw_get_cache_class_size(long level, enum cache_types type);
long topohw_get_cache_class_num_caches(long level, enum cache_types type);


//==========================================================================================================================================
//= Caches
//==========================================================================================================================================


long topohw_get_cache_size(long cpu_id, long level, enum cache_types type);
long topohw_get_cache_cpus(long cpu_id, long level, enum cache_types type, const long ** cpu_id_list_ret);


//==========================================================================================================================================
//= Memory Nodes
//==========================================================================================================================================


long topohw_get_num_mem_nodes();
long topohw_get_num_mem_node_cpus(long cpu_id);
long topohw_get_num_mem_node_cores(long cpu_id);
long topohw_get_mem_node_cpus_list(long cpu_id, const long ** cpu_id_list_ret);


//==========================================================================================================================================
//= Print Topology
//==========================================================================================================================================


void topohw_print_topology();


#endif /* HARDWARE_TOPOLOGY_H */

