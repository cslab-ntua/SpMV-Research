#ifndef CUDA_UTIL_H
#define CUDA_UTIL_H

// https://stackoverflow.com/a/14038590
#define gpuCudaErrorCheck(ans) { gpuCudaAssert((ans), __FILE__, __LINE__); }
inline void gpuCudaAssert(cudaError_t code, const char *file, int line, bool abort=true)
{
	if (code != cudaSuccess) 
	{
		fprintf(stderr,"ERROR @ %s %s %d\n", cudaGetErrorString(code), file, line);
		if (abort) exit(code);
	}
}

__device__
static int
cuda_get_thread_coord_x()
{
	return blockDim.x * blockIdx.x + threadIdx.x;
}

__device__
static int
cuda_get_thread_coord_y()
{
	return blockDim.y * blockIdx.y + threadIdx.y;
}

__device__
static int
cuda_get_thread_coord_z()
{
	return blockDim.z * blockIdx.z + threadIdx.z;
}


__device__
static int
cuda_num_threads_x()
{
	return gridDim.x * blockDim.x;
}

__device__
static int
cuda_num_threads_y()
{
	return gridDim.y * blockDim.y;
}

__device__
static int
cuda_num_threads_z()
{
	return gridDim.z * blockDim.z;
}


//==========================================================================================================================================
//= Global Thread Ids
//==========================================================================================================================================


/* Two different logics:
 *
 * 1) ids are continuous inside each block.
 *    e.g.
 *    [ [ 0,  1] | [ 4,  5] ]
 *    [ [ 2,  3] | [ 6,  7] ]
 *    [ ---------|--------  ]
 *    [ [ 8,  9] | [12, 13] ]
 *    [ [10, 11] | [14, 15] ]
 *
 *    block 0 -> 0, 1, 2, 3
 *
 * 2) ids are continuous in the total thread grid.
 *    e.g.
 *    [ [ 0,  1] | [ 2,  3] ]
 *    [ [ 4,  5] | [ 6,  7] ]
 *    [ ---------|--------  ]
 *    [ [ 8,  9] | [10, 11] ]
 *    [ [12, 13] | [14, 15] ]
 *
 *    block 0 -> 0, 1, 4, 5
 */

// Block-continuous logic:
__device__
static int
cuda_get_thread_num_bc()
{
	// Threads per block
	int num_threads_pb = blockDim.x * blockDim.y * blockDim.z;
	// Block id in grid
	int bnum = gridDim.x * gridDim.y * blockIdx.z + gridDim.x * blockIdx.y + blockIdx.x;
	// Block-local thread id
	int tnum_b = blockDim.x * blockDim.y * threadIdx.z + blockDim.x * threadIdx.y + threadIdx.x;
	int tnum = bnum * num_threads_pb + tnum_b;
	return tnum;
}


// Grid-continuous logic
__device__
static int
cuda_get_thread_num_gc()
{
	int dim_x = cuda_num_threads_x();
	int dim_y = cuda_num_threads_y();
	int dim_z = cuda_num_threads_z();
	return dim_x * dim_y * cuda_get_thread_coord_z() + dim_x * cuda_get_thread_coord_y() + cuda_get_thread_coord_x();
}


__device__
static int
cuda_get_thread_num()
{
	return cuda_get_thread_num_bc();
}


#endif /* CUDA_UTIL_H */

