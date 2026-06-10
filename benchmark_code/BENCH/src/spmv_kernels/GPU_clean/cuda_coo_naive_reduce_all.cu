#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

#include <cuda.h>
#include <cooperative_groups.h>

#include "macros/cpp_defines.h"

#include "../spmv_kernel.h"

#ifdef __cplusplus
extern "C"{
#endif
	#include "macros/macrolib.h"
	#include "time_it.h"
	#include "parallel_util.h"
	#include "array_metrics.h"

	#include "cuda/cuda_util.h"
#ifdef __cplusplus
}
#endif


using namespace cooperative_groups;

#ifndef NNZ_PER_THREAD
	#define NNZ_PER_THREAD  6
#endif

#ifndef BLOCK_SIZE
	// #define BLOCK_SIZE  32
	// #define BLOCK_SIZE  64
	// #define BLOCK_SIZE  128
	// #define BLOCK_SIZE  256
	// #define BLOCK_SIZE  512
	#define BLOCK_SIZE  1024
#endif

#ifndef TIME_IT
	#define TIME_IT 0
#endif


void
cuda_push_duplicate_base(void ** dst_ptr, void * src, long bytes)
{
	cudaMalloc(dst_ptr, bytes);
	cudaMemcpy(*((char **) dst_ptr), src, bytes, cudaMemcpyHostToDevice);
}
#define cuda_push_duplicate(dst_ptr, src, bytes) cuda_push_duplicate_base((void **) dst_ptr, src, bytes)


template<typename T>
void
transpose(T * A, INT_T m, INT_T n)
{
	T * buf = (typeof(buf)) aligned_alloc(64, m*n * sizeof(*buf));
	long i, j;
	for (j=0;j<n;j++)
	{
		for (i=0;i<m;i++)
			buf[j*m + i] = A[i*n + j];
	}
	for (i=0;i<m*n;i++)
		A[i] = buf[i];
	free(buf);
}


struct CSRArrays : Matrix_Format
{
	INT_T * row_ptr_h;
	INT_T * ia_h;
	INT_T * ja_h;
	ValueType * a_h;

	INT_T * row_ptr_d;
	INT_T * ia_d;
	INT_T * ja_d;
	ValueType * a_d;

	ValueType * x = NULL;
	ValueType * y = NULL;
	ValueType * x_h = NULL;
	ValueType * y_h = NULL;
	ValueType * x_d = NULL;
	ValueType * y_d = NULL;

	int num_threads;
	int thread_block_size;
	int num_thread_blocks;

	CSRArrays(INT_T * row_ptr, INT_T * ja, ValueTypeReference * a, long m, long n, long nnz) : Matrix_Format(m, n, nnz)
	{
		double time;
		const long nnz_per_block = BLOCK_SIZE * NNZ_PER_THREAD;

		cuda_device_print_attributes();

		thread_block_size = BLOCK_SIZE;

		num_threads = (nnz + NNZ_PER_THREAD - 1) / NNZ_PER_THREAD;
		num_thread_blocks = (num_threads + BLOCK_SIZE - 1) / BLOCK_SIZE;
		num_threads = num_thread_blocks * BLOCK_SIZE;
		printf("num_threads=%d, thread_block_size=%d, num_thread_blocks=%d\n", num_threads, BLOCK_SIZE, num_thread_blocks);

		cuda_assert(cudaMalloc(&row_ptr_d, (m+1) * sizeof(*row_ptr_d)));
		cuda_assert(cudaMalloc(&ia_d, nnz * sizeof(*ia_d)));
		cuda_assert(cudaMalloc(&ja_d, nnz * sizeof(*ja_d)));
		cuda_assert(cudaMalloc(&a_d, nnz * sizeof(*a_d)));
		cuda_assert(cudaMalloc(&x_d, n * sizeof(*x_d)));
		cuda_assert(cudaMalloc(&y_d, m * sizeof(*y_d)));

		cuda_assert(cudaMallocHost(&row_ptr_h, (m+1) * sizeof(INT_T)));
		cuda_assert(cudaMallocHost(&ia_h, nnz * sizeof(INT_T)));
		cuda_assert(cudaMallocHost(&ja_h, nnz * sizeof(INT_T)));
		cuda_assert(cudaMallocHost(&a_h, nnz * sizeof(ValueType)));
		cuda_assert(cudaMallocHost(&x_h, n * sizeof(ValueType)));
		cuda_assert(cudaMallocHost(&y_h, m * sizeof(ValueType)));

		memcpy(row_ptr_h, row_ptr, (m + 1) * sizeof(INT_T));
		memcpy(ja_h, ja, nnz * sizeof(INT_T));
		memcpy(a_h, a, nnz * sizeof(ValueType));

		time = time_it(1,
			_Pragma("omp parallel")
			{
				long i, j, j_s, j_e;
				_Pragma("omp for")
				for (i=0;i<m;i++)
				{
					j_s = row_ptr[i];
					j_e = row_ptr[i+1];
					for (j=j_s;j<j_e;j++)
					{
						ia_h[j] = i;
					}
				}
			}
		);
		printf("find row indices = %g\n", time);

		cuda_assert(cudaMemcpy(row_ptr_d, row_ptr_h, (m+1) * sizeof(*row_ptr_d), cudaMemcpyHostToDevice));
		cuda_assert(cudaMemcpy(ia_d, ia_h, nnz * sizeof(*ia_d), cudaMemcpyHostToDevice));
		cuda_assert(cudaMemcpy(ja_d, ja_h, nnz * sizeof(*ja_d), cudaMemcpyHostToDevice));
		cuda_assert(cudaMemcpy(a_d, a_h, nnz * sizeof(*a_d), cudaMemcpyHostToDevice));
	}

	~CSRArrays()
	{
		cuda_assert(cudaFree(row_ptr_d));
		cuda_assert(cudaFree(ia_d));
		cuda_assert(cudaFree(ja_d));
		cuda_assert(cudaFree(a_d));
		cuda_assert(cudaFree(x_d));
		cuda_assert(cudaFree(y_d));

		cuda_assert(cudaFreeHost(row_ptr_h));
		cuda_assert(cudaFreeHost(ia_h));
		cuda_assert(cudaFreeHost(ja_h));
		cuda_assert(cudaFreeHost(a_h));
		cuda_assert(cudaFreeHost(x_h));
		cuda_assert(cudaFreeHost(y_h));
	}

	void spmv(ValueType * x, ValueType * y);
	void statistics_start();
	int statistics_print_data(__attribute__((unused)) char * buf, __attribute__((unused)) long buf_n);
};


void compute_csr(CSRArrays * restrict csr, ValueType * restrict x , ValueType * restrict y);

void
CSRArrays::spmv(ValueType * x, ValueType * y)
{
	compute_csr(this, x, y);
}


struct Matrix_Format *
csr_to_format(INT_T * row_ptr, INT_T * col_ind, ValueTypeReference * values, long m, long n, long nnz, long symmetric, long symmetry_expanded)
{
	if (symmetric && !symmetry_expanded)
		error("symmetric matrices not supported by this format, expand symmetry");
	struct CSRArrays * csr = new CSRArrays(row_ptr, col_ind, values, m, n, nnz);
	// for (long i=0;i<10;i++)
		// printf("%d\n", row_ptr[i]);
	csr->mem_footprint = nnz * (sizeof(ValueType) + sizeof(INT_T)) + (m+1) * sizeof(INT_T);
	char *format_name;
	format_name = (char *)malloc(100*sizeof(char));
	snprintf(format_name, 100, "Custom_CSR_CUDA_constant_nnz_per_thread_b%d_nnz%d", BLOCK_SIZE, NNZ_PER_THREAD);
	csr->format_name = format_name;
	return csr;
}


//==========================================================================================================================================
//= CSR Custom
//==========================================================================================================================================


template<typename T>
inline
__device__
T
binary_search_gpu(T * A, long s, long e, T target)
{
	long m;
	while (1)
	{
		m = (s + e) / 2;
		if (m == s || m == e)
			break;
		if (target > A[m])
			s = m;
		else
			e = m;
	}
	if (target == A[e])
		return e;
	return s;
}


//------------------------------------------------------------------------------------------------------------------------------------------
//- Reduce
//------------------------------------------------------------------------------------------------------------------------------------------


template <typename group_t>
inline
__device__
ValueType
reduce_warp_single_row(group_t g, ValueType val)
{
	// Use XOR mode to perform butterfly reduction
	for (int i=g.size()/2; i>=1; i/=2)
	{
		val += g.shfl_xor(val, i); // __shfl_xor_sync(0xffffffff, val, i, g.size());   // Total sum is same on all threads.
		// val += __shfl_down_sync(0xffffffff, val, i, g.size());   // Only thread 0 has the total sum.
	}
	return val;
}


template <typename group_t>
inline
__device__
void
reduce_warp(group_t g, INT_T row, ValueType val, ValueType * restrict y)
{
	const int tidw = g.thread_rank();   // Group lane.
	int mask_same_row = g.match_any(row);
	int k;
	#pragma unroll
	for (k=g.size()/2; k>=1; k/=2)
	{
		int tidl_next = tidw + k;
		ValueType val_next = g.shfl(val, tidl_next);
		if ((tidl_next < g.size()) && (mask_same_row & (1 << tidl_next)))
		{
			val += val_next;
		}
	}
	if (tidw == __ffs(mask_same_row) - 1)  // __ffs enumeration is 1-based.
		atomicAdd(&y[row], val);
}


inline
__device__
void
reduce_block(INT_T row, ValueType val, ValueType * restrict y)
{
	thread_block_tile<32> tile32 = tiled_partition<32>(this_thread_block());
	reduce_warp(tile32, row, val, y);
}


template<typename T, typename group_t>
static __attribute__((always_inline)) inline
__device__
T
warp_scan_reduce_inclusive(group_t g, T value)
{
	int lane = g.thread_rank();
	T prev;
	int i;
	#pragma unroll
	for (i=1;i<32;i*=2)
	{
		prev = g.shfl_up(value, i);
		if (lane >= i)
			value += prev;
	}
	return value;
}


template<typename T, typename group_t>
static __attribute__((always_inline)) inline
__device__
T
warp_scan_reduce_exclusive(group_t g, T value)
{
	return warp_scan_reduce_inclusive(g, value) - value;
}



template<typename T, typename group_t>
static __attribute__((always_inline)) inline
__device__
T
warp_segmented_reduce(group_t g, int color, T value, T * per_color_sums_ret)
{
	int lane = g.thread_rank();
	int i;
	int mask_same = g.match_any(color);
	ValueType sum = value;
	#pragma unroll
	for (i=g.size()/2; i>=1; i/=2)
	{
		int tidl_next = lane + i;
		ValueType val_next = g.shfl(sum, tidl_next);
		if ((tidl_next < g.size()) && (mask_same & (1 << tidl_next)))
		{
			sum += val_next;
		}
	}
	if (lane == __ffs(mask_same) - 1)
		atomicAdd(&per_color_sums_ret[color], sum);
	return sum;
}


template<typename T, typename group_t>
static __attribute__((always_inline)) inline
__device__
void
warp_reduce_colored(group_t g, int color, T value, T * per_color_sums_ret,
		uint8_t shared_rev_offsets[32])
{
	int lane = g.thread_rank();

	uint32_t mask_same = g.match_any(color);
	int num_same = __popc(mask_same);
	int lane_leader = __ffs(mask_same) - 1;  // __ffs enumeration is 1-based.

	int offset_local = __popc(mask_same & ((1 << lane) - 1));

	int offset_base = warp_scan_reduce_exclusive(g, (lane_leader == lane) ? num_same : 0);
	offset_base = g.shfl(offset_base, lane_leader);

	int offset = offset_base + offset_local;

	shared_rev_offsets[offset] = lane;

	g.sync();

	T value_new = g.shfl(value, shared_rev_offsets[lane]);
	int color_new = g.shfl(color, shared_rev_offsets[lane]);

	// Segmented reduce.
	warp_segmented_reduce(g, color_new, value_new, per_color_sums_ret);
}


//------------------------------------------------------------------------------------------------------------------------------------------
//- Spmv
//------------------------------------------------------------------------------------------------------------------------------------------


__device__
void
spmv_last_block(INT_T * ia, INT_T * ja, ValueType * a, long m, long n, long nnz, ValueType * restrict x, ValueType * restrict y)
{
	const int tid = cuda_get_thread_num_bc();
	const int tidb = threadIdx.x;
	const int block_id = blockIdx.x;
	const int nnz_per_block = BLOCK_SIZE * NNZ_PER_THREAD;
	__attribute__((unused)) int i, i_e, j, j_s, j_e, k;
	j_s = block_id * nnz_per_block + tidb * NNZ_PER_THREAD;
	j_e = j_s + NNZ_PER_THREAD;
	if (j_e > nnz)
		j_e = nnz;
	for (j=j_s;j<j_e;j++)
	{
		i = ia[j];
		atomicAdd(&y[i], a[j] * x[ja[j]]);
	}
}


template <typename group_t>
__device__
void
spmv_full_warp(group_t g, int j_s, int j_b_s, int j_w_s, INT_T * ia, INT_T * ja, ValueType * a, ValueType * restrict x, ValueType * restrict y,
		uint8_t shared_rev_offsets[32])
{
	const int tidl = g.thread_rank();   // Group lane.
	ValueType prod;
	int i, j, jj;
	for (j=j_s,jj=j_s;j<j_s+NNZ_PER_THREAD;j++,jj++)
	// for (j=j_s,jj=j_b_s+threadIdx.x;j<j_s+NNZ_PER_THREAD;j++,jj+=BLOCK_SIZE)
	// for (j=j_s,jj=j_w_s+tidl;j<j_s+NNZ_PER_THREAD;j++,jj+=g.size())
	{
		i = ia[jj];
		prod = a[jj] * x[ja[jj]];
		warp_reduce_colored(g, i, prod, y, shared_rev_offsets);
		// atomicAdd(&y[i], prod);
	}
}


__device__
void
spmv_full_block(INT_T * ia, INT_T * ja, ValueType * a, long m, long n, long nnz, ValueType * restrict x, ValueType * restrict y)
{
	extern __shared__ uint8_t shared_rev_offsets[];
	const int tid = cuda_get_thread_num_bc();
	const int tidw = threadIdx.x % 32;
	const int warp_id = threadIdx.x / 32;
	const int block_id = blockIdx.x;
	__attribute__((unused)) int i_s, i_e, j_s, j_b_s, j_w_s, k;
	j_b_s = block_id * BLOCK_SIZE * NNZ_PER_THREAD;
	j_w_s = j_b_s + warp_id * 32 * NNZ_PER_THREAD;
	j_s = j_w_s + tidw * NNZ_PER_THREAD;
	thread_block_tile<32> tile32 = tiled_partition<32>(this_thread_block());
	spmv_full_warp(tile32, j_s, j_b_s, j_w_s, ia, ja, a, x, y, &shared_rev_offsets[warp_id*32]);
}


__global__
void
gpu_kernel_spmv_row_indices_continuous(INT_T * ia, INT_T * ja, ValueType * a, long m, long n, long nnz, ValueType * restrict x, ValueType * restrict y)
{
	int grid_size = gridDim.x;
	int block_id = blockIdx.x;
	if (block_id == grid_size - 1)
		spmv_last_block(ia, ja, a, m, n, nnz, x, y);
	else
		spmv_full_block(ia, ja, a, m, n, nnz, x, y);
}


void
compute_csr(CSRArrays * restrict csr, ValueType * restrict x, ValueType * restrict y)
{
	dim3 block_dims(BLOCK_SIZE);
	dim3 grid_dims(csr->num_thread_blocks);
	// long shared_mem_size = BLOCK_SIZE * (sizeof(ValueType));
	// long shared_mem_size = BLOCK_SIZE * (sizeof(ValueType) + sizeof(INT_T));
	// long shared_mem_size = BLOCK_SIZE * NNZ_PER_THREAD * sizeof(INT_T);
	long shared_mem_size = BLOCK_SIZE * sizeof(uint8_t);
	// long shared_mem_size = 0;

	if (csr->x == NULL)
	{
		printf("Grid : {%d, %d, %d} blocks. Blocks : {%d, %d, %d} threads.\n", grid_dims.x, grid_dims.y, grid_dims.z, block_dims.x, block_dims.y, block_dims.z);
		csr->x = x;
		memcpy(csr->x_h, x, csr->n * sizeof(ValueType));
		cuda_assert(cudaMemcpy(csr->x_d, csr->x_h, csr->n * sizeof(*csr->x_d), cudaMemcpyHostToDevice));
	}

	cudaMemset(csr->y_d, 0, csr->m * sizeof(csr->y_d));

	// cudaFuncCachePreferNone:   no preference for shared memory or L1 (default);
	// cudaFuncCachePreferShared: prefer larger shared memory and smaller L1 cache;
	// cudaFuncCachePreferL1:     prefer larger L1 cache and smaller shared memory;
	cuda_assert(cudaFuncSetCacheConfig(gpu_kernel_spmv_row_indices_continuous, cudaFuncCachePreferL1));
	gpu_kernel_spmv_row_indices_continuous<<<grid_dims, block_dims, shared_mem_size>>>(csr->ia_d, csr->ja_d, csr->a_d, csr->m, csr->n, csr->nnz, csr->x_d, csr->y_d);
	cuda_assert(cudaPeekAtLastError());
	cuda_assert(cudaDeviceSynchronize());

	if (csr->y == NULL)
	{
		csr->y = y;

		cuda_assert(cudaMemcpy(csr->y_h, csr->y_d, csr->m * sizeof(*csr->y_d), cudaMemcpyDeviceToHost));
		memcpy(y, csr->y_h, csr->m * sizeof(ValueType));
	}
}


//==========================================================================================================================================
//= Print Statistics
//==========================================================================================================================================


void
CSRArrays::statistics_start()
{
}


int
statistics_print_labels(__attribute__((unused)) char * buf, __attribute__((unused)) long buf_n)
{
	return 0;
}


int
CSRArrays::statistics_print_data(__attribute__((unused)) char * buf, __attribute__((unused)) long buf_n)
{
	return 0;
}

