#ifndef TASK_TOPOLOGY_H
#define TASK_TOPOLOGY_H

#include "macros/cpp_defines.h"

#include "hardware_topology.h"


/* Execution Task
 * /proc/self/task
 */
struct topotk_task {
	long id;    // Task id, as contiguous numbers.
	long tid;   // Thread id.

	long cpu_id;

	// Affinity
	long num_cpus_allowed;
	long * cpus_allowed_list;

	long task_l1_node_local_task_id;
	long task_l1_node_id;

	long task_l2_node_local_task_id;
	long task_l2_node_id;

	long task_l3_node_local_task_id;
	long task_l3_node_id;

	long task_mem_node_id;
	long task_mem_node_local_task_id;   // Task id local to task memory node.
};


struct topotk_task_node {
	long id;
	long node_id;

	long num_tasks;
	long * task_id_list;
};


struct topotk_task_mem_node {
	long id;
	long mem_node_id;

	long num_tasks;
	long * task_id_list;
};


struct topotk_topology {
	long * dict_cpu_id_to_num_tasks;
	long ** dict_cpu_id_to_task_id_list;
	long * dict_mem_node_to_task_mem_node;

	long num_tasks;
	long task_tid_max;
	long * task_tid_list;
	struct topotk_task * tasks;

	long num_task_l1_nodes;
	struct topotk_task_node * task_l1_nodes;

	long num_task_l2_nodes;
	struct topotk_task_node * task_l2_nodes;

	long num_task_l3_nodes;
	struct topotk_task_node * task_l3_nodes;

	long num_task_mem_nodes;
	struct topotk_task_node * task_mem_nodes;
};


//==========================================================================================================================================
//------------------------------------------------------------------------------------------------------------------------------------------
//-                                                             Functions                                                                  -
//------------------------------------------------------------------------------------------------------------------------------------------
//==========================================================================================================================================


/* We use 'num' instead of 'id' in function names to be consistent with openmp functions.
 */


struct topotk_topology * topotk_get_topology();


//==========================================================================================================================================
//= Tasks
//==========================================================================================================================================


long topotk_get_task_num();
long topotk_get_num_tasks();


//==========================================================================================================================================
//= CPUs
//==========================================================================================================================================


long topotk_get_cpu_num();


//==========================================================================================================================================
//= Task Nodes
//==========================================================================================================================================


long topotk_get_task_cache_node_num(long level);
long topotk_get_num_task_cache_nodes(long level);
long topotk_get_num_task_cache_node_tasks(long level);
long topotk_get_task_cache_node_local_task_num(long level);
long topotk_get_task_cache_node_tasks_list(long level, const long ** task_id_list_ret);


long topotk_get_task_mem_node_num();
long topotk_get_num_task_mem_nodes();
long topotk_get_num_task_mem_node_tasks();
long topotk_get_task_mem_node_local_task_num();
long topotk_get_task_mem_node_tasks_list(const long ** task_id_list_ret);


//==========================================================================================================================================
//= Print Topology
//==========================================================================================================================================


void topotk_print_topology();


#endif /* TASK_TOPOLOGY_H */

