#ifndef OMP_FUNCTIONS_H
#define OMP_FUNCTIONS_H

#include <omp.h>


static inline
int
safe_omp_get_num_threads_next_par_region()
{
	int num_threads;
	#pragma omp parallel
	{
		#pragma omp single
		num_threads = omp_get_num_threads();
	}
	return num_threads;
}


static inline
int
safe_omp_get_initial_thread_num()
{
	int tnum;
	if (omp_get_level() > 1)
		tnum = omp_get_ancestor_thread_num(1);
	else
		tnum = omp_get_thread_num();
	return tnum;
}




#endif /* OMP_FUNCTIONS_H */

