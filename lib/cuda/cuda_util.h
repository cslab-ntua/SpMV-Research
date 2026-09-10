#ifndef CUDA_UTIL_H
#define CUDA_UTIL_H

#include "debug.h"


// https://stackoverflow.com/a/14038590
#define gpuCudaErrorCheck(ans) { gpuCudaAssert((ans), __FILE__, __LINE__); }
static inline
void
gpuCudaAssert(cudaError_t code, const char * file, int line, bool abort=true)
{
	if (code != cudaSuccess) 
	{
		fprintf(stderr,"ERROR @ %s %s %d\n", cudaGetErrorString(code), file, line);
		if (abort)
			exit(code);
	}
}


#define cuda_assert(_code)                                                \
{                                                                         \
	cudaError_t __code = _code;                                       \
	if (__code != cudaSuccess)                                        \
	{                                                                 \
		error("CUDA ERROR: %s\n", cudaGetErrorString(__code));    \
	}                                                                 \
}


//==========================================================================================================================================
//= Thread Ids
//==========================================================================================================================================


__device__
static inline
int
cuda_get_thread_coord_x()
{
	return blockDim.x * blockIdx.x + threadIdx.x;
}

__device__
static inline
int
cuda_get_thread_coord_y()
{
	return blockDim.y * blockIdx.y + threadIdx.y;
}

__device__
static inline
int
cuda_get_thread_coord_z()
{
	return blockDim.z * blockIdx.z + threadIdx.z;
}


__device__
static inline
int
cuda_num_threads_x()
{
	return gridDim.x * blockDim.x;
}

__device__
static inline
int
cuda_num_threads_y()
{
	return gridDim.y * blockDim.y;
}

__device__
static inline
int
cuda_num_threads_z()
{
	return gridDim.z * blockDim.z;
}


/* Two different views:
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

// Block-continuous view.
__device__
static inline
int
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


// Grid-continuous view.
__device__
static inline
int
cuda_get_thread_num_gc()
{
	int dim_x = cuda_num_threads_x();
	int dim_y = cuda_num_threads_y();
	int dim_z = cuda_num_threads_z();
	return dim_x * dim_y * cuda_get_thread_coord_z() + dim_x * cuda_get_thread_coord_y() + cuda_get_thread_coord_x();
}


__device__
static inline
int
cuda_get_thread_num()
{
	return cuda_get_thread_num_bc();
}


//==========================================================================================================================================
//= GPU attributes
//==========================================================================================================================================


static inline
int
cuda_device_get_attribute(cudaDeviceAttr attr, int  device)
{
	int ret;
	cuda_assert(cudaDeviceGetAttribute(&ret, attr, device));
	return ret;
}


static inline
void
cuda_device_print_attributes()
{
	int max_smem_per_block, multiproc_count, max_threads_per_block, warp_size, max_threads_per_multiproc, max_block_dim_x, max_persistent_l2_cache, max_num_threads;
	cuda_assert(cudaDeviceGetAttribute(&max_smem_per_block, cudaDevAttrMaxSharedMemoryPerBlock, 0));
	cuda_assert(cudaDeviceGetAttribute(&multiproc_count, cudaDevAttrMultiProcessorCount, 0));
	cuda_assert(cudaDeviceGetAttribute(&max_threads_per_block, cudaDevAttrMaxThreadsPerBlock , 0));
	cuda_assert(cudaDeviceGetAttribute(&warp_size, cudaDevAttrWarpSize , 0));
	cuda_assert(cudaDeviceGetAttribute(&max_threads_per_multiproc, cudaDevAttrMaxThreadsPerMultiProcessor, 0));
	cuda_assert(cudaDeviceGetAttribute(&max_block_dim_x, cudaDevAttrMaxBlockDimX, 0));
	cuda_assert(cudaDeviceGetAttribute(&max_persistent_l2_cache, cudaDevAttrMaxPersistingL2CacheSize, 0));
	max_num_threads = max_threads_per_multiproc * multiproc_count;
	printf("max_smem_per_block(bytes)=%d\n", max_smem_per_block);
	printf("multiproc_count=%d\n", multiproc_count);
	printf("max_threads_per_block=%d\n", max_threads_per_block);
	printf("warp_size=%d\n", warp_size);
	printf("max_threads_per_multiproc=%d\n", max_threads_per_multiproc);
	printf("max_block_dim_x=%d\n", max_block_dim_x);
	printf("max_persistent_l2_cache=%d\n", max_persistent_l2_cache);
	printf("max_num_threads=%d\n", max_num_threads);
}


#endif /* CUDA_UTIL_H */

