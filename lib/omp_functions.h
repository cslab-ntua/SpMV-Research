#ifndef OMP_FUNCTIONS_H
#define OMP_FUNCTIONS_H

#include <unistd.h>
#include <omp.h>

#include "macros/cpp_defines.h"


int safe_omp_get_num_threads_internal();
int safe_omp_get_num_threads_external();
int safe_omp_get_num_threads();
int safe_omp_get_thread_num_initial();

int get_affinity_from_GOMP_CPU_AFFINITY(int tnum);
void print_affinity();


#endif /* OMP_FUNCTIONS_H */

