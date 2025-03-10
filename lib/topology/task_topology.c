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
#include "io.h"

#include "task_topology.h"
#include "topology_util.h"

#include "data_structures/dynamic_array/dynamic_array_gen_undef.h"
#define DYNAMIC_ARRAY_GEN_TYPE_1  long
#define DYNAMIC_ARRAY_GEN_SUFFIX  _task_topology_l
#include "data_structures/dynamic_array/dynamic_array_gen.c"


static struct topotk_topology * TP = NULL;


//==========================================================================================================================================
//------------------------------------------------------------------------------------------------------------------------------------------
//-                                                           Create Topology                                                              -
//------------------------------------------------------------------------------------------------------------------------------------------
//==========================================================================================================================================


static inline
void
parse_task_data(long tid_procfs, long id, struct topotk_task * task)
{
	long buf_n = 1000;
	char buf[buf_n];

	char * str_target = "Cpus_allowed:";
	long str_target_n = strlen(str_target);
	long str_n;
	char * str;
	long len;
	long i, i_e;

	task->tid = tid_procfs;
	task->id = id;

	snprintf(buf, buf_n, "/proc/self/task/%ld/status", tid_procfs);
	str_n = read_sysfs_file(buf, &str);
	i = 0;
	while (i < str_n)
	{
		len = str_find_eol(str+i, str_n-i);
		i_e = i + len;
		if ((len >= str_target_n) && (!strncmp(str+i, str_target, str_target_n)))
		{
			str[i_e] = '\0';
			i += str_target_n;
			i += str_find_non_ws(str+i, i_e - i);
			task->num_cpus_allowed = hex_chars_to_int_list(str+i, i_e - i, &task->cpus_allowed_list);
			break;
		}
		i = i_e + 1;
	}
	if (task->num_cpus_allowed == 1)
		task->cpu_id = task->cpus_allowed_list[0];
	else
		task->cpu_id = -1;
}


static inline
void
parse_task_node_data(long id, long node_id, struct topotk_task_node * task_node)
{
	task_node->id = id;
	task_node->node_id = node_id;
}


long
get_cpu_node_id_l1(struct topohw_cpu * cpu)
{
	struct topohw_cache * cache = NULL;
	long i;
	for (i=0;i<cpu->num_caches;i++)
	{
		cache = cpu->caches_ptrs[i];
		if (cache->level == 1 && cache->type == TOPOHW_CT_D)
			break;
	}
	if (cache == NULL)
		error("no cache found");
	return cache->cache_class_local_cache_id;
}
void
set_task_node_data_l1(struct topotk_task * task, long task_node_local_task_id, long task_node_id)
{
	task->task_l1_node_local_task_id = task_node_local_task_id;
	task->task_l1_node_id = task_node_id;
}

long
get_cpu_node_id_l2(struct topohw_cpu * cpu)
{
	struct topohw_cache * cache = NULL;
	long i;
	for (i=0;i<cpu->num_caches;i++)
	{
		cache = cpu->caches_ptrs[i];
		if (cache->level == 2 && cache->type == TOPOHW_CT_U)
			break;
	}
	if (cache == NULL)
		error("no cache found");
	return cache->cache_class_local_cache_id;
}
void
set_task_node_data_l2(struct topotk_task * task, long task_node_local_task_id, long task_node_id)
{
	task->task_l2_node_local_task_id = task_node_local_task_id;
	task->task_l2_node_id = task_node_id;
}

long
get_cpu_node_id_l3(struct topohw_cpu * cpu)
{
	struct topohw_cache * cache = NULL;
	long i;
	for (i=0;i<cpu->num_caches;i++)
	{
		cache = cpu->caches_ptrs[i];
		if (cache->level == 3 && cache->type == TOPOHW_CT_U)
			break;
	}
	if (cache == NULL)
		error("no cache found");
	return cache->cache_class_local_cache_id;
}
void
set_task_node_data_l3(struct topotk_task * task, long task_node_local_task_id, long task_node_id)
{
	task->task_l3_node_local_task_id = task_node_local_task_id;
	task->task_l3_node_id = task_node_id;
}

long
get_cpu_node_id_mem(struct topohw_cpu * cpu)
{
	return cpu->mem_node_id;
}
void
set_task_node_data_mem(struct topotk_task * task, long task_node_local_task_id, long task_node_id)
{
	task->task_mem_node_local_task_id = task_node_local_task_id;
	task->task_mem_node_id = task_node_id;
}


static
long
calculate_task_node(
	struct topotk_topology * tp,
	struct topohw_topology * tphw,
	long num_nodes,
	struct topotk_task_node ** task_nodes_ret,
	long (* get_cpu_node_id)(struct topohw_cpu * cpu),
	void (* set_task_node_data)(struct topotk_task * task, long task_node_local_task_id, long task_node_id)
		)
{
	struct dynarray ** das;
	long * dict_node_to_task_node;
	struct topotk_task * tasks = tp->tasks;
	long num_task_nodes;
	struct topotk_task_node * task_nodes, * task_node;
	long task_id;
	long node_id;
	long i, j, k;
	if (task_nodes_ret == NULL)
		error("'task_nodes_ret' is NULL");
	/* Find the tasks of each node. */
	das = (typeof(das)) malloc(num_nodes * sizeof(*das));
	dict_node_to_task_node = (typeof(dict_node_to_task_node)) malloc(num_nodes * sizeof(*dict_node_to_task_node));
	for (i=0;i<num_nodes;i++)
	{
		das[i] = dynarray_new(0);
		dict_node_to_task_node[i] = -1;
	}
	for (i=0;i<tphw->num_cpus;i++)
	{
		node_id = get_cpu_node_id(&(tphw->cpus[i]));
		for (j=0;j<tp->dict_cpu_id_to_num_tasks[i];j++)
		{
			task_id = tp->dict_cpu_id_to_task_id_list[i][j];
			dynarray_push_back(das[node_id], task_id);
		}
	}
	/* task nodes : all the nodes that have tasks */
	num_task_nodes = 0;
	for (i=0;i<num_nodes;i++)
	{
		if (das[i]->size > 0)
			num_task_nodes++;
	}
	task_nodes = (typeof(task_nodes)) malloc(num_task_nodes * sizeof(*task_nodes));
	j = 0;
	for (i=0;i<num_nodes;i++)
	{
		if (das[i]->size > 0)
		{
			task_node = &task_nodes[j];
			dict_node_to_task_node[i] = j;
			parse_task_node_data(j, i, task_node);
			task_node->num_tasks = dynarray_export_array(das[i], &task_node->task_id_list);
			for (k=0;k<task_node->num_tasks;k++)
			{
				task_id = task_node->task_id_list[k];
				set_task_node_data(&tasks[task_id], k, j);
			}
			j++;
		}
	}
	for (i=0;i<num_nodes;i++)
		dynarray_destroy(&das[i]);
	free(das);
	free(dict_node_to_task_node);
	*task_nodes_ret = task_nodes;
	return num_task_nodes;
}


struct topotk_topology *
topotk_get_topology()
{
	static uint64_t create_lock = 0;

	struct topotk_topology * tp = TP;
	struct topohw_topology * tphw = topohw_get_topology();

	long num_tasks;
	struct topotk_task * tasks, * task;
	long * task_tid_list;
	long task_tid_max;

	[[gnu::unused]] struct topohw_cpu * cpu;
	struct topohw_cache_class * cache_class;

	struct dynarray ** das;
	struct dynarray * da;
	uint64_t prev;
	long i;

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

	da = dynarray_new(0);

	//----------------------------------------------------------------------------------------------------------------------------------
	//- Tasks
	//----------------------------------------------------------------------------------------------------------------------------------

	if (omp_get_level() == 0)   // Openmp might not have been initialized yet, start a parallel region to enforce pinning.
	{
		#pragma omp parallel
		{
			__attribute__((unused)) volatile int i=0;
		}
	}

	num_tasks = find_numbered_files_list("/proc/self/task", "", &task_tid_list);
	tasks = (typeof(tasks)) malloc(num_tasks * sizeof(*tasks));
	task_tid_max = -1;
	for (i=0;i<num_tasks;i++)
	{
		// task = &tasks[i];
		parse_task_data(task_tid_list[i], i, &tasks[i]);
		if (task_tid_list[i] > task_tid_max)
			task_tid_max = task_tid_list[i];
	}
	tp->num_tasks = num_tasks;
	tp->tasks = tasks;
	tp->task_tid_list = task_tid_list;
	tp->task_tid_max = task_tid_max;

	/* Find all the tasks of each cpu. */
	das = (typeof(das)) malloc(tphw->num_cpus * sizeof(*das));
	tp->dict_cpu_id_to_num_tasks = (typeof(tp->dict_cpu_id_to_num_tasks)) malloc(tphw->num_cpus * sizeof(*tp->dict_cpu_id_to_num_tasks));
	tp->dict_cpu_id_to_task_id_list = (typeof(tp->dict_cpu_id_to_task_id_list)) malloc(tphw->num_cpus * sizeof(*tp->dict_cpu_id_to_task_id_list));
	for (i=0;i<tphw->num_cpus;i++)
		das[i] = dynarray_new(0);
	for (i=0;i<num_tasks;i++)
	{
		task = &tasks[i];
		dynarray_push_back(das[task->cpu_id], task->id);
	}
	for (i=0;i<tphw->num_cpus;i++)
	{
		tp->dict_cpu_id_to_num_tasks[i] = dynarray_export_array(das[i], &tp->dict_cpu_id_to_task_id_list[i]);
	}
	for (i=0;i<tphw->num_mem_nodes;i++)
		dynarray_destroy(&das[i]);
	free(das);

	//----------------------------------------------------------------------------------------------------------------------------------
	//- Task Nodes
	//----------------------------------------------------------------------------------------------------------------------------------

	long num_l1_nodes = 0;
	for (i=0;i<tphw->num_cache_classes;i++)
	{
		cache_class = &tphw->cache_classes[i];
		if (cache_class->level == 1 && cache_class->type == TOPOHW_CT_D)
			num_l1_nodes = cache_class->num_caches;
	}
	tp->num_task_l1_nodes = calculate_task_node(tp, tphw, num_l1_nodes, &tp->task_l1_nodes, get_cpu_node_id_l1, set_task_node_data_l1);

	long num_l2_nodes = 0;
	for (i=0;i<tphw->num_cache_classes;i++)
	{
		cache_class = &tphw->cache_classes[i];
		if (cache_class->level == 2 && cache_class->type == TOPOHW_CT_U)
			num_l2_nodes = cache_class->num_caches;
	}
	tp->num_task_l2_nodes = calculate_task_node(tp, tphw, num_l2_nodes, &tp->task_l2_nodes, get_cpu_node_id_l2, set_task_node_data_l2);

	long num_l3_nodes = 0;
	for (i=0;i<tphw->num_cache_classes;i++)
	{
		cache_class = &tphw->cache_classes[i];
		if (cache_class->level == 3 && cache_class->type == TOPOHW_CT_U)
			num_l3_nodes = cache_class->num_caches;
	}
	tp->num_task_l3_nodes = calculate_task_node(tp, tphw, num_l3_nodes, &tp->task_l3_nodes, get_cpu_node_id_l3, set_task_node_data_l3);

	tp->num_task_mem_nodes = calculate_task_node(tp, tphw, tphw->num_mem_nodes, &tp->task_mem_nodes, get_cpu_node_id_mem, set_task_node_data_mem);


	dynarray_destroy(&da);
	__atomic_store_n(&TP, tp, __ATOMIC_SEQ_CST);
	return tp;
}


//==========================================================================================================================================
//------------------------------------------------------------------------------------------------------------------------------------------
//-                                                         Exported Functions                                                             -
//------------------------------------------------------------------------------------------------------------------------------------------
//==========================================================================================================================================


//==========================================================================================================================================
//= Tasks
//==========================================================================================================================================


long
topotk_get_task_num()
{
	int tid = gettid();
	struct topotk_topology * tp = topotk_get_topology();
	struct topotk_task * task;
	static __thread long t_storage = -1;
	long task_id;
	long i;
	if (t_storage >= 0)
		return t_storage;
	task = &tp->tasks[0];
	for (i=0;i<tp->num_tasks;i++)
	{
		task = &tp->tasks[i];
		if (task->tid == tid)
			break;
	}
	if (i >= tp->num_tasks)
		error("task with tid=%d not found", tid);
	task_id = task->id;
	t_storage = task_id;
	return task_id;
}


long
topotk_get_num_tasks()
{
	struct topotk_topology * tp = topotk_get_topology();
	static __thread long t_storage = -1;
	long num_tasks;
	if (t_storage >= 0)
		return t_storage;
	num_tasks = tp->num_tasks;
	t_storage = num_tasks;
	return num_tasks;
}


//==========================================================================================================================================
//= CPUs
//==========================================================================================================================================


long
topotk_get_cpu_num()
{
	struct topotk_topology * tp = topotk_get_topology();
	int task_id = topotk_get_task_num();
	struct topotk_task * task = &tp->tasks[task_id];
	static __thread long t_storage = -1;
	long cpu_id;
	if (t_storage >= 0)
		return t_storage;
	cpu_id = task->cpu_id;
	if (cpu_id < 0)
		error("thread is not pinned to a specific cpu, can't define cpu number");
	t_storage = cpu_id;
	return cpu_id;
}


//==========================================================================================================================================
//= Cache Nodes
//==========================================================================================================================================


long
topotk_get_task_cache_node_num(long level)
{
	struct topotk_topology * tp = topotk_get_topology();
	int task_id = topotk_get_task_num();
	struct topotk_task * task;
	static __thread long t_storage[4] = {-1, -1, -1, -1};
	long task_node_id;
	if (t_storage[level] >= 0)
		return t_storage[level];
	task = &tp->tasks[task_id];
	task_node_id = (level == 1) ? task->task_l1_node_id : (level == 2) ? task->task_l2_node_id : (level == 3) ? task->task_l3_node_id : (error("unsupported level: %ld", level), 0);
	t_storage[level] = task_node_id;
	return task_node_id;
}


long
topotk_get_num_task_cache_nodes(long level)
{
	struct topotk_topology * tp = topotk_get_topology();
	return (level == 1) ? tp->num_task_l1_nodes : (level == 2) ? tp->num_task_l2_nodes : (level == 3) ? tp->num_task_l3_nodes : (error("unsupported level: %ld", level), 0);
}


long
topotk_get_num_task_cache_node_tasks(long level)
{
	struct topotk_topology * tp = topotk_get_topology();
	int task_id = topotk_get_task_num();
	struct topotk_task * task;
	struct topotk_task_node * task_nodes;
	static __thread long t_storage[4] = {-1, -1, -1, -1};
	long task_node_id, num_tasks;
	if (t_storage[level] >= 0)
		return t_storage[level];
	task = &tp->tasks[task_id];
	task_node_id = (level == 1) ? task->task_l1_node_id : (level == 2) ? task->task_l2_node_id : (level == 3) ? task->task_l3_node_id : (error("unsupported level: %ld", level), 0);
	task_nodes = (level == 1) ? tp->task_l1_nodes : (level == 2) ? tp->task_l2_nodes : (level == 3) ? tp->task_l3_nodes : (error("unsupported level: %ld", level), NULL);
	num_tasks = task_nodes[task_node_id].num_tasks;
	t_storage[level] = num_tasks;
	return num_tasks;
}


long
topotk_get_task_cache_node_local_task_num(long level)
{
	struct topotk_topology * tp = topotk_get_topology();
	int task_id = topotk_get_task_num();
	struct topotk_task * task;
	static __thread long t_storage[4] = {-1, -1, -1, -1};
	long task_node_local_task_id;
	if (t_storage[level] >= 0)
		return t_storage[level];
	task = &tp->tasks[task_id];
	task_node_local_task_id = (level == 1) ? task->task_l1_node_local_task_id : (level == 2) ? task->task_l2_node_local_task_id : (level == 3) ? task->task_l3_node_local_task_id : (error("unsupported level: %ld", level), 0);
	t_storage[level] = task_node_local_task_id;
	return task_node_local_task_id;
}


long
topotk_get_task_cache_node_tasks_list(long level, const long ** task_id_list_ret)
{
	struct topotk_topology * tp = topotk_get_topology();
	int task_id = topotk_get_task_num();
	struct topotk_task * task;
	struct topotk_task_node * task_nodes;
	static __thread long t_storage[4] = {-1, -1, -1, -1};
	static __thread const long * t_storage_list[4] = {NULL, NULL, NULL, NULL};
	const long * task_id_list = NULL;
	long task_node_id, num_tasks;
	if (t_storage[level] >= 0)
	{
		if (task_id_list_ret != NULL)
			*task_id_list_ret = t_storage_list[level];
		return t_storage[level];
	}
	task = &tp->tasks[task_id];
	task_node_id = (level == 1) ? task->task_l1_node_id : (level == 2) ? task->task_l2_node_id : (level == 3) ? task->task_l3_node_id : (error("unsupported level: %ld", level), 0);
	task_nodes = (level == 1) ? tp->task_l1_nodes : (level == 2) ? tp->task_l2_nodes : (level == 3) ? tp->task_l3_nodes : (error("unsupported level: %ld", level), NULL);
	num_tasks = task_nodes[task_node_id].num_tasks;
	task_id_list = task_nodes[task_node_id].task_id_list;
	t_storage[level] = num_tasks;
	t_storage_list[level] = task_id_list;
	if (task_id_list_ret != NULL)
		*task_id_list_ret = t_storage_list[level];
	return num_tasks;
}


//==========================================================================================================================================
//= Memory Nodes
//==========================================================================================================================================


long
topotk_get_task_mem_node_num()
{
	struct topotk_topology * tp = topotk_get_topology();
	int task_id = topotk_get_task_num();
	static __thread long t_storage = -1;
	long task_mem_node_id;
	if (t_storage >= 0)
		return t_storage;
	task_mem_node_id = tp->tasks[task_id].task_mem_node_id;
	t_storage = task_mem_node_id;
	return task_mem_node_id;
}


long
topotk_get_num_task_mem_nodes()
{
	struct topotk_topology * tp = topotk_get_topology();
	return tp->num_task_mem_nodes;
}


long
topotk_get_num_task_mem_node_tasks()
{
	struct topotk_topology * tp = topotk_get_topology();
	int task_id = topotk_get_task_num();
	static __thread long t_storage = -1;
	long task_mem_node_id, num_tasks;
	if (t_storage >= 0)
		return t_storage;
	task_mem_node_id = tp->tasks[task_id].task_mem_node_id;
	num_tasks = tp->task_mem_nodes[task_mem_node_id].num_tasks;
	t_storage = num_tasks;
	return num_tasks;
}


long
topotk_get_task_mem_node_local_task_num()
{
	struct topotk_topology * tp = topotk_get_topology();
	int task_id = topotk_get_task_num();
	static __thread long t_storage = -1;
	long task_mem_node_local_task_id;
	if (t_storage >= 0)
		return t_storage;
	task_mem_node_local_task_id = tp->tasks[task_id].task_mem_node_local_task_id;
	t_storage = task_mem_node_local_task_id;
	return task_mem_node_local_task_id;
}


long
topotk_get_task_mem_node_tasks_list(const long ** task_id_list_ret)
{
	struct topotk_topology * tp = topotk_get_topology();
	int task_id = topotk_get_task_num();
	static __thread long t_storage = -1;
	static __thread const long * t_storage_list = NULL;
	const long * task_id_list = NULL;
	long task_mem_node_id, num_tasks;
	if (t_storage >= 0)
	{
		if (task_id_list_ret != NULL)
			*task_id_list_ret = t_storage_list;
		return t_storage;
	}
	task_mem_node_id = tp->tasks[task_id].task_mem_node_id;
	num_tasks = tp->task_mem_nodes[task_mem_node_id].num_tasks;
	task_id_list = tp->task_mem_nodes[task_mem_node_id].task_id_list;
	t_storage = num_tasks;
	t_storage_list = task_id_list;
	if (task_id_list_ret != NULL)
		*task_id_list_ret = t_storage_list;
	return num_tasks;
}


//==========================================================================================================================================
//= Print Topology
//==========================================================================================================================================


void
topotk_print_topology()
{
	struct topohw_topology * tp_hw = topohw_get_topology();
	struct topotk_topology * tp_tk = topotk_get_topology();
	struct topotk_task * task;
	struct topotk_task_node * task_node;
	long i, j;

	printf("\nTasks\n");
	printf("num_tasks=%ld, task_tid_max=%ld\n", tp_tk->num_tasks, tp_tk->task_tid_max);
	printf("    ");
	for (i=0;i<tp_tk->num_tasks;i++)
	{
		printf("%ld ", tp_tk->task_tid_list[i]);
	}
	printf("\n");
	for (i=0;i<tp_tk->num_tasks;i++)
	{
		task = &tp_tk->tasks[i];
		printf("    id=%ld, cpu_id=%ld, task_l1_node_id=%ld, task_l2_node_id=%ld, task_l3_node_id=%ld, task_mem_node_id=%ld, task_l1_node_local_task_id=%ld, task_l2_node_local_task_id=%ld, task_l3_node_local_task_id=%ld, task_mem_node_local_task_id=%ld, num_cpus_allowed=%ld\n",
				task->id, task->cpu_id, task->task_l1_node_id, task->task_l2_node_id, task->task_l3_node_id, task->task_mem_node_id, task->task_l1_node_local_task_id, task->task_l2_node_local_task_id, task->task_l3_node_local_task_id, task->task_mem_node_local_task_id, task->num_cpus_allowed);
		printf("        ");
		for (j=0;j<task->num_cpus_allowed;j++)
		{
			printf("%ld ", task->cpus_allowed_list[j]);
		}
		printf("\n");
	}

	printf("tasks per cpu\n");
	for (i=0;i<tp_hw->num_cpus;i++)
	{
		printf("    cpu_id=%ld -> num_tasks=%ld : ", i, tp_tk->dict_cpu_id_to_num_tasks[i]);
		for (j=0;j<tp_tk->dict_cpu_id_to_num_tasks[i];j++)
		{
			printf("%ld ", tp_tk->dict_cpu_id_to_task_id_list[i][j]);
		}
		printf("\n");
	}

	printf("num_task_l1_nodes=%ld\n", tp_tk->num_task_l1_nodes);
	for (i=0;i<tp_tk->num_task_l1_nodes;i++)
	{
		task_node = &tp_tk->task_l1_nodes[i];
		printf("    id=%ld, l1_node_id=%ld, num_tasks=%ld\n", task_node->id, task_node->node_id, task_node->num_tasks);
		printf("        ");
		for (j=0;j<task_node->num_tasks;j++)
		{
			printf("%ld ", task_node->task_id_list[j]);
		}
		printf("\n");
	}

	printf("num_task_l2_nodes=%ld\n", tp_tk->num_task_l2_nodes);
	for (i=0;i<tp_tk->num_task_l2_nodes;i++)
	{
		task_node = &tp_tk->task_l2_nodes[i];
		printf("    id=%ld, l2_node_id=%ld, num_tasks=%ld\n", task_node->id, task_node->node_id, task_node->num_tasks);
		printf("        ");
		for (j=0;j<task_node->num_tasks;j++)
		{
			printf("%ld ", task_node->task_id_list[j]);
		}
		printf("\n");
	}

	printf("num_task_l3_nodes=%ld\n", tp_tk->num_task_l3_nodes);
	for (i=0;i<tp_tk->num_task_l3_nodes;i++)
	{
		task_node = &tp_tk->task_l3_nodes[i];
		printf("    id=%ld, l3_node_id=%ld, num_tasks=%ld\n", task_node->id, task_node->node_id, task_node->num_tasks);
		printf("        ");
		for (j=0;j<task_node->num_tasks;j++)
		{
			printf("%ld ", task_node->task_id_list[j]);
		}
		printf("\n");
	}

	printf("num_task_mem_nodes=%ld\n", tp_tk->num_task_mem_nodes);
	for (i=0;i<tp_tk->num_task_mem_nodes;i++)
	{
		task_node = &tp_tk->task_mem_nodes[i];
		printf("    id=%ld, mem_node_id=%ld, num_tasks=%ld\n", task_node->id, task_node->node_id, task_node->num_tasks);
		printf("        ");
		for (j=0;j<task_node->num_tasks;j++)
		{
			printf("%ld ", task_node->task_id_list[j]);
		}
		printf("\n");
	}

}

