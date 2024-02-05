#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

#include <cuda.h>

#include "macros/cpp_defines.h"

#include "spmv_bench_common.h"
#include "spmv_kernel.h"

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


INT_T * thread_block_i_s = NULL;
INT_T * thread_block_i_e = NULL;

INT_T * thread_i_s_dev = NULL;
INT_T * thread_i_e_dev = NULL;


extern int prefetch_distance;

double * thread_time_compute, * thread_time_barrier;


struct CSRArrays : Matrix_Format
{
	INT_T * ia;      // the usual rowptr (of size m+1)
	INT_T * ja;      // the colidx of each NNZ (of size nnz)
	ValueType * a;   // the values (of size NNZ)

	INT_T * ia_dev;
	INT_T * ja_dev;
	ValueType * a_dev;

	ValueType * x = NULL;
	ValueType * y = NULL;
	ValueType * x_dev = NULL;
	ValueType * y_dev = NULL;

	unsigned char * rel_row_idx;
	unsigned char * rel_row_idx_dev;

	int max_smem_per_block, multiproc_count, max_threads_per_block, warp_size, max_threads_per_multiproc, max_block_dim_x;
	int num_threads;
	int block_size;
	int num_blocks;

	// int row_cluster_size = 256;
	// int row_cluster_size = 192;
	// int row_cluster_size = 128;
	// int row_cluster_size = 64;
	// int row_cluster_size = 32;
	// int row_cluster_size = 16;
	// int row_cluster_size = 8;
	int row_cluster_size = 4;

	CSRArrays(INT_T * ia, INT_T * ja, ValueType * a, long m, long n, long nnz) : Matrix_Format(m, n, nnz), ia(ia), ja(ja), a(a)
	{
		double time_balance;
		long i;

		cudaDeviceGetAttribute(&max_smem_per_block, cudaDevAttrMaxSharedMemoryPerBlock, 0);
		cudaDeviceGetAttribute(&multiproc_count, cudaDevAttrMultiProcessorCount, 0);
		cudaDeviceGetAttribute(&max_threads_per_block, cudaDevAttrMaxThreadsPerBlock , 0);
		cudaDeviceGetAttribute(&warp_size, cudaDevAttrWarpSize , 0);
		cudaDeviceGetAttribute(&max_threads_per_multiproc, cudaDevAttrMaxThreadsPerMultiProcessor, 0);
		cudaDeviceGetAttribute(&max_block_dim_x, cudaDevAttrMaxBlockDimX, 0);
		printf("max_smem_per_block=%d\n", max_smem_per_block);
		printf("multiproc_count=%d\n", multiproc_count);
		printf("max_threads_per_block=%d\n", max_threads_per_block);
		printf("warp_size=%d\n", warp_size);
		printf("max_threads_per_multiproc=%d\n", max_threads_per_multiproc);
		printf("max_block_dim_x=%d\n", max_block_dim_x);

		// block_size = warp_size / 2;
		block_size = warp_size;
		// block_size = warp_size * 2;

		// num_threads = 128;
		// num_threads = 1ULL << 10;
		// num_threads = 3584;
		// num_threads = 1ULL << 12;
		// num_threads = 1ULL << 13;
		// num_threads = 1ULL << 14;
		// num_threads = 1ULL << 15;
		// num_threads = 1ULL << 16;
		// num_threads = 1ULL << 17;
		num_threads = 1ULL << 21;

		num_threads = ((num_threads + block_size - 1) / block_size) * block_size;

		num_blocks = num_threads / block_size;

		printf("num_threads=%d, block_size=%d, num_blocks=%d\n", num_threads, block_size, num_blocks);

		thread_block_i_s = (INT_T *) malloc(num_blocks * sizeof(*thread_block_i_s));
		thread_block_i_e = (INT_T *) malloc(num_blocks * sizeof(*thread_block_i_e));
		time_balance = time_it(1,
			for (i=0;i<num_blocks;i++)
			{
				// loop_partitioner_balance_iterations(num_blocks, i, 0, m, &thread_block_i_s[i], &thread_block_i_e[i]);
				loop_partitioner_balance_prefix_sums(num_blocks, i, ia, m, nnz, &thread_block_i_s[i], &thread_block_i_e[i]);
			}
		);
		printf("balance time = %g\n", time_balance);

		rel_row_idx = (typeof(rel_row_idx)) malloc(nnz * sizeof(*rel_row_idx));
		#pragma omp parallel
		{
			long i, i_s, i_e, i_rel, j, k;
			#pragma omp for
			for (k=0;k<num_blocks;k++)
			{
				i_s = thread_block_i_s[k];
				i_e = thread_block_i_e[k];
				for (i=i_s;i<i_e;i++)
				{
					i_rel = (i - i_s) % row_cluster_size;
					for (j=ia[i];j<ia[i+1];j++)
					{
						rel_row_idx[j] = i_rel;
					}
				}
			}
		}


		cudaMalloc(&ia_dev, (m+1) * sizeof(*ia_dev));
		cudaMemcpy(ia_dev, ia, (m+1) * sizeof(*ia_dev), cudaMemcpyHostToDevice);

		cudaMalloc(&ja_dev, nnz * sizeof(*ja_dev));
		cudaMemcpy(ja_dev, ja, nnz * sizeof(*ja_dev), cudaMemcpyHostToDevice);

		cudaMalloc(&a_dev, nnz * sizeof(*a_dev));
		cudaMemcpy(a_dev, a, nnz * sizeof(*a_dev), cudaMemcpyHostToDevice);

		cudaMalloc(&rel_row_idx_dev, nnz * sizeof(*rel_row_idx_dev));
		cudaMemcpy(rel_row_idx_dev, rel_row_idx, nnz * sizeof(*rel_row_idx_dev), cudaMemcpyHostToDevice);

		cudaMalloc(&x_dev, n * sizeof(*x_dev));

		cudaMalloc(&y_dev, m * sizeof(*y_dev));

		cudaMalloc(&thread_i_s_dev, num_blocks * sizeof(*thread_i_s_dev));
		cudaMemcpy(thread_i_s_dev, thread_block_i_s, num_blocks * sizeof(*thread_i_s_dev), cudaMemcpyHostToDevice);

		cudaMalloc(&thread_i_e_dev, num_blocks * sizeof(*thread_i_e_dev));
		cudaMemcpy(thread_i_e_dev, thread_block_i_e, num_blocks * sizeof(*thread_i_e_dev), cudaMemcpyHostToDevice);

	}

	~CSRArrays()
	{
		free(a);
		free(ia);
		free(ja);
		free(thread_block_i_s);
		free(thread_block_i_e);
		free(rel_row_idx);

		cudaFree(ia_dev);
		cudaFree(ja_dev);
		cudaFree(a_dev);
	}

	void spmv(ValueType * x, ValueType * y);
	void statistics_start();
	int statistics_print_data(__attribute__((unused)) char * buf, __attribute__((unused)) long buf_n);
};


void compute_csr(CSRArrays * restrict csr, ValueType * restrict x , ValueType * restrict y);
void compute_csr_kahan(CSRArrays * restrict csr, ValueType * restrict x, ValueType * restrict y);
void compute_csr_prefetch(CSRArrays * restrict csr, ValueType * restrict x , ValueType * restrict y);
void compute_csr_omp_simd(CSRArrays * restrict csr, ValueType * restrict x , ValueType * restrict y);
void compute_csr_vector(CSRArrays * restrict csr, ValueType * restrict x , ValueType * restrict y);
void compute_csr_vector_perfect_nnz_balance(CSRArrays * restrict csr, ValueType * restrict x , ValueType * restrict y);


void
CSRArrays::spmv(ValueType * x, ValueType * y)
{
	compute_csr(this, x, y);
}


struct Matrix_Format *
csr_to_format(INT_T * row_ptr, INT_T * col_ind, ValueType * values, long m, long n, long nnz)
{
	struct CSRArrays * csr = new CSRArrays(row_ptr, col_ind, values, m, n, nnz);
	// for (long i=0;i<10;i++)
		// printf("%d\n", row_ptr[i]);
	csr->mem_footprint = nnz * (sizeof(ValueType) + sizeof(INT_T)) + (m+1) * sizeof(INT_T);
	csr->format_name = (char *) "Custom_CSR_CUDA";
	return csr;
}


//==========================================================================================================================================
//= CSR Custom
//==========================================================================================================================================


// __device__ int add(int a, int b)
// {
	// return a + b;
// }


__global__ void gpu_kernel_csr_basic(INT_T * thread_block_i_s, INT_T * thread_block_i_e, INT_T * ia, INT_T * ja, ValueType * a, ValueType * restrict x, ValueType * restrict y, unsigned char * rel_row_idx, int row_cluster_size)
{
	extern __shared__ ValueType sdata[];
	int tgid = cuda_get_thread_num();
	int tbid = threadIdx.x;
	int block_id = blockIdx.x;
	int block_size = blockDim.x;
	long i, i_s, i_e, i_rel, i_rel_e, j, j_s, j_e, k;
	ValueType sum;
	for (i=0;i<row_cluster_size;i++)
		sdata[i*block_size + tbid] = 0;
	__syncthreads();
	i_s = thread_block_i_s[block_id];
	i_e = thread_block_i_e[block_id];
	// printf("%d,%d: bs=%d , bid=%d , %ld %ld\n", tgid, tbid, block_size, block_id, i_s, i_e);
	for (k=i_s;k<i_e;k+=row_cluster_size)
	{
		i_rel_e = k + row_cluster_size > i_e ? i_e - k : row_cluster_size;
		j_s = ia[k];
		j_e = ia[k+i_rel_e];
		for (j=j_s+tbid;j<j_e;j+=block_size)
		{
			i_rel = rel_row_idx[j];
			sdata[i_rel*block_size + tbid] += a[j] * x[ja[j]];
		}

		__syncthreads();

		for (i_rel=tbid;i_rel<i_rel_e;i_rel+=block_size)
		{
			sum = 0;
			for (j=0;j<block_size;j++)
			{
				sum += sdata[i_rel*block_size + j];
				sdata[i_rel*block_size + j] = 0;
			}
			y[k + i_rel] = sum;
		}

		// for (i_rel=0;i_rel<i_rel_e;i_rel++)
		// {
			// sum = 0;
			// for (j=1;j<block_size;j*=2)
			// {
				// if (tbid % (2*j) == 0)
				// {
					// sdata[i_rel*block_size + tbid] += sdata[i_rel*block_size + tbid + j];
					// sdata[i_rel*block_size + tbid + j] = 0;
				// }
				// __syncthreads();
			// }
			// if (tbid == 0)
			// {
				// y[k + i_rel] = sdata[i_rel*block_size];
				// sdata[i_rel*block_size] = 0;
			// }
		// }

		__syncthreads();
	}
}


void
compute_csr(CSRArrays * restrict csr, ValueType * restrict x, ValueType * restrict y)
{
	// int num_threads = csr->num_threads;
	int block_size = csr->block_size;
	int num_blocks = csr->num_blocks;
	dim3 block_dims(block_size);
	dim3 grid_dims(num_blocks);
	// long shared_mem_size = block_size * sizeof(*C_dev);

	if (csr->x == NULL)
	{
		csr->x = x;
		cudaMemcpy(csr->x_dev, csr->x, csr->n * sizeof(*csr->x), cudaMemcpyHostToDevice);
	}

	gpu_kernel_csr_basic<<<grid_dims, block_dims, (csr->row_cluster_size*block_size*sizeof(ValueType))>>>(thread_i_s_dev, thread_i_e_dev, csr->ia_dev, csr->ja_dev, csr->a_dev, csr->x_dev, csr->y_dev, csr->rel_row_idx_dev, csr->row_cluster_size);
	// gpu_kernel_csr_flat<<<grid_dims, block_dims>>>(thread_i_s_dev, thread_i_e_dev, csr->ia_dev, csr->ja_dev, csr->a_dev, csr->x_dev, csr->y_dev);

	cudaError_t err;
	err = cudaDeviceSynchronize();
	if (err != cudaSuccess)
		error("gpu kernel error: %s\n", cudaGetErrorString(err));
	err = cudaGetLastError();
	if (err != cudaSuccess)
		error("gpu kernel error: %s\n", cudaGetErrorString(err));

	if (csr->y == NULL)
	{
		csr->y = y;
		cudaMemcpy(csr->y, csr->y_dev, csr->m * sizeof(*csr->y), cudaMemcpyDeviceToHost);
	}

	// exit(0);
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

