#ifndef TOPOLOGY_H
#define TOPOLOGY_H

#include "macros/cpp_defines.h"

long topo_get_core_num_logical();
long topo_get_core_num_physical();
long topo_get_num_cores_logical();
long topo_get_num_cores_physical();

long topo_get_node_num();
long topo_get_node_cores_list(long ** cores_out);
long topo_get_num_node_cores_logical();
long topo_get_num_node_cores_physical();
long topo_get_num_nodes();


#endif /* TOPOLOGY_H */

