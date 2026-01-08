#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

#include <cuda.h>

#include "macros/cpp_defines.h"

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

INT_T * thread_block_j_s = NULL;
INT_T * thread_block_j_e = NULL;


INT_T * thread_block_i_s_dev = NULL;
INT_T * thread_block_i_e_dev = NULL;

INT_T * thread_block_j_s_dev = NULL;
INT_T * thread_block_j_e_dev = NULL;


extern int prefetch_distance;

double * thread_time_compute, * thread_time_barrier;

void
cuda_push_duplicate_base(void ** dst_ptr, void * src, long bytes)
{
	cudaMalloc(dst_ptr, bytes);
	cudaMemcpy(*((char **) dst_ptr), src, bytes, cudaMemcpyHostToDevice);
}
#define cuda_push_duplicate(dst_ptr, src, bytes) cuda_push_duplicate_base((void **) dst_ptr, src, bytes)


struct CSRArrays : Matrix_Format
{
	INT_T * row_ptr;
	INT_T * ia;
	INT_T * ja;
	ValueType * a;

	INT_T * row_ptr_dev;
	INT_T * ia_dev;
	INT_T * ja_dev;
	ValueType * a_dev;

	ValueType * multres_dev;

	ValueType * x = NULL;
	ValueType * y = NULL;
	ValueType * x_dev = NULL;
	ValueType * y_dev = NULL;

	int max_smem_per_block, multiproc_count, max_threads_per_block, warp_size, max_threads_per_multiproc, max_block_dim_x, max_num_threads;
	int num_threads;
	int block_size;
	int num_blocks;

	CSRArrays(INT_T * row_ptr, INT_T * ja, ValueType * a, long m, long n, long nnz) : Matrix_Format(m, n, nnz), row_ptr(row_ptr), ja(ja), a(a)
	{
		double time_balance;
		long i;

		cudaDeviceGetAttribute(&max_smem_per_block, cudaDevAttrMaxSharedMemoryPerBlock, 0);
		cudaDeviceGetAttribute(&multiproc_count, cudaDevAttrMultiProcessorCount, 0);
		cudaDeviceGetAttribute(&max_threads_per_block, cudaDevAttrMaxThreadsPerBlock , 0);
		cudaDeviceGetAttribute(&warp_size, cudaDevAttrWarpSize , 0);
		cudaDeviceGetAttribute(&max_threads_per_multiproc, cudaDevAttrMaxThreadsPerMultiProcessor, 0);
		cudaDeviceGetAttribute(&max_block_dim_x, cudaDevAttrMaxBlockDimX, 0);
		max_num_threads = max_threads_per_multiproc * multiproc_count;
		printf("max_smem_per_block(bytes)=%d\n", max_smem_per_block);
		printf("multiproc_count=%d\n", multiproc_count);
		printf("max_threads_per_block=%d\n", max_threads_per_block);
		printf("warp_size=%d\n", warp_size);
		printf("max_threads_per_multiproc=%d\n", max_threads_per_multiproc);
		printf("max_block_dim_x=%d\n", max_block_dim_x);
		printf("max_num_threads=%d\n", max_num_threads);

		// block_size = 32;
		// block_size = 64;
		// block_size = 128;
		// block_size = 256;
		// block_size = 512;
		block_size = 1024;

		// num_threads = 128;
		// num_threads = 1ULL << 10;
		// num_threads = 1ULL << 12;
		// num_threads = 1ULL << 13;
		// num_threads = 1ULL << 14;
		// num_threads = 1ULL << 15;
		// num_threads = 1ULL << 16;
		// num_threads = 1ULL << 17;
		// num_threads = 1ULL << 20;
		// num_threads = 1ULL << 21;
		// num_threads = 1ULL << 22;
		// num_threads = 1ULL << 23;
		// num_threads = 1ULL << 24;
		// num_threads = nnz / 2;
		// num_threads = nnz / 3;
		num_threads = nnz / 4;
		// num_threads = nnz / 5;
		// num_threads = nnz / 8;
		// num_threads = nnz / 16;
		// num_threads = m;

		num_threads = ((num_threads + block_size - 1) / block_size) * block_size;

		num_blocks = num_threads / block_size;

		printf("num_threads=%d, block_size=%d, num_blocks=%d\n", num_threads, block_size, num_blocks);

		thread_block_i_s = (INT_T *) malloc(num_blocks * sizeof(*thread_block_i_s));
		thread_block_i_e = (INT_T *) malloc(num_blocks * sizeof(*thread_block_i_e));
		thread_block_j_s = (INT_T *) malloc(num_blocks * sizeof(*thread_block_j_s));
		thread_block_j_e = (INT_T *) malloc(num_blocks * sizeof(*thread_block_j_e));
		time_balance = time_it(1,
			for (i=0;i<num_blocks;i++)
			{

				// loop_partitioner_balance_iterations(num_blocks, i, 0, m, &thread_block_i_s[i], &thread_block_i_e[i]);
				// loop_partitioner_balance_prefix_sums(num_blocks, i, row_ptr, m, nnz, &thread_block_i_s[i], &thread_block_i_e[i]);
				// thread_block_j_s[i] = row_ptr[thread_block_i_s[i]];
				// thread_block_j_e[i] = row_ptr[thread_block_i_e[i]];

				long lower_boundary;
				loop_partitioner_balance_iterations(num_blocks, i, 0, nnz, &thread_block_j_s[i], &thread_block_j_e[i]);
				macros_binary_search(row_ptr, 0, m, thread_block_j_s[i], &lower_boundary, NULL);           // Index boundaries are inclusive.
				thread_block_i_s[i] = lower_boundary;
			}
			for (i=0;i<num_blocks;i++)
			{
				if (i == num_blocks - 1)   // If we calculate each thread's boundaries individually some empty rows might be unassigned.
					thread_block_i_e[i] = m;
				else
					thread_block_i_e[i] = thread_block_i_s[i+1] + 1;
				if ((thread_block_j_s[i] >= row_ptr[thread_block_i_e[i]]) || (thread_block_j_s[i] < row_ptr[thread_block_i_s[i]]))
					error("bad binary search of row start: i=%d j:[%d, %d] j=%d", thread_block_i_s[i], row_ptr[thread_block_i_s[i]], row_ptr[thread_block_i_e[i]], thread_block_j_s[i]);
			}
		);
		printf("balance time = %g\n", time_balance);

		ia = (typeof(ia)) malloc(nnz * sizeof(*ia));
		_Pragma("omp parallel")
		{
			long i, j;
			_Pragma("omp for")
			for (i=0;i<m;i++)
			{
				for (j=row_ptr[i];j<row_ptr[i+1];j++)
				{
					ia[j] = i;
				}
			}
		}

		cuda_push_duplicate(&row_ptr_dev, row_ptr, (m+1) * sizeof(*row_ptr_dev));
		cuda_push_duplicate(&ia_dev, ia, nnz * sizeof(*ia_dev));
		cuda_push_duplicate(&ja_dev, ja, nnz * sizeof(*ja_dev));
		cuda_push_duplicate(&a_dev, a, nnz * sizeof(*a_dev));
		cudaMalloc(&multres_dev, nnz * sizeof(*y_dev));

		cudaMalloc(&x_dev, n * sizeof(*x_dev));
		cudaMalloc(&y_dev, m * sizeof(*y_dev));

		cuda_push_duplicate(&thread_block_i_s_dev, thread_block_i_s, num_blocks * sizeof(*thread_block_i_s_dev));
		cuda_push_duplicate(&thread_block_i_e_dev, thread_block_i_e, num_blocks * sizeof(*thread_block_i_e_dev));
		cuda_push_duplicate(&thread_block_j_s_dev, thread_block_j_s, num_blocks * sizeof(*thread_block_j_s_dev));
		cuda_push_duplicate(&thread_block_j_e_dev, thread_block_j_e, num_blocks * sizeof(*thread_block_j_e_dev));

	}

	~CSRArrays()
	{
		free(a);
		free(row_ptr);
		free(ia);
		free(ja);
		free(thread_block_i_s);
		free(thread_block_i_e);

		cudaFree(row_ptr_dev);
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
csr_to_format(INT_T * row_ptr, INT_T * col_ind, ValueType * values, long m, long n, long nnz, int symmetric)
{
	if (symmetric)
		error("symmetric matrices not supported by this format, expand symmetry");
	struct CSRArrays * csr = new CSRArrays(row_ptr, col_ind, values, m, n, nnz);
	// for (long i=0;i<10;i++)
		// printf("%d\n", row_ptr[i]);
	csr->mem_footprint = nnz * (sizeof(ValueType) + sizeof(INT_T)) + (m+1) * sizeof(INT_T);
	csr->format_name = (char *) "Custom_CSR_CUDA_reduce";
	return csr;
}


//==========================================================================================================================================
//= CSR Custom
//==========================================================================================================================================


// __device__ int add(int a, int b)
// {
	// return a + b;
// }


__global__ void gpu_kernel_spmv_row_indices(INT_T * thread_block_i_s, INT_T * thread_block_i_e, INT_T * thread_block_j_s, INT_T * thread_block_j_e, INT_T * row_ptr, INT_T * ia, INT_T * ja, ValueType * a, ValueType * restrict x, ValueType * restrict y)
{
	extern __shared__ char sm[];
	int tidg = cuda_get_thread_num();
	int tidb = threadIdx.x;
	int block_id = blockIdx.x;
	int block_size = blockDim.x;
	ValueType * val_buf = (typeof(val_buf)) sm;
	INT_T * ia_buf = (typeof(ia_buf)) &sm[block_size * sizeof(ValueType)];
	INT_T * ja_rel;
	ValueType * a_rel;
	// int i, i_s, i_e, j, j_s, j_e, k;
	int i, j, j_s, j_e;
	// i_s = thread_block_i_s[block_id];
	// i_e = thread_block_i_e[block_id];
	j_s = thread_block_j_s[block_id];
	j_e = thread_block_j_e[block_id];
	int j_e_div = j_e - ((j_e-j_s) % block_size);
	for (j=j_s;j<j_e_div;j+=block_size)
	{
		ia_buf[tidb] = ia[j+tidb];
		ja_rel = &ja[j];
		a_rel = &a[j];
		val_buf[tidb] = a_rel[tidb] * x[ja_rel[tidb]];
		__syncthreads();
		for (i=1;i<block_size;i*=2)
		{
			if ((tidb & (2*i-1)) == i-1)
			{
				if (ia_buf[tidb] == ia_buf[tidb+i])
					val_buf[tidb+i] += val_buf[tidb];
				else
					y[ia_buf[tidb]] += val_buf[tidb];
			}
			__syncthreads();
		}
		if (tidb == 0)
			y[ia_buf[block_size-1]] += val_buf[block_size-1];
		__syncthreads();
	}
	if (tidb == 0)
	{
		for (j=j_e_div;j<j_e;j++)
		{
			y[ia[j]] += a[j] * x[ja[j]];
		}
	}
}


__global__ void gpu_kernel_spmv_gather_multiply(INT_T * thread_block_j_s, INT_T * thread_block_j_e, INT_T * ja, ValueType * a, ValueType * restrict x, ValueType * multres)
{
	int tidg = cuda_get_thread_num();
	int tidb = threadIdx.x;
	int block_id = blockIdx.x;
	int block_size = blockDim.x;
	int j, j_s, j_e;
	j_s = thread_block_j_s[block_id];
	j_e = thread_block_j_e[block_id];
	int j_e_div = j_e - ((j_e-j_s) % block_size);
	for (j=j_s;j<j_e_div;j+=block_size)
		multres[j+tidb] = a[j+tidb] * x[ja[j+tidb]];
	j = j_e_div + tidb;
	if (j < j_e)
		multres[j] = a[j] * x[ja[j]];
}


__global__ void gpu_kernel_spmv_row_indices_atomics(INT_T * thread_block_i_s, INT_T * thread_block_i_e, INT_T * thread_block_j_s, INT_T * thread_block_j_e, INT_T * row_ptr, INT_T * ia, INT_T * ja, ValueType * a, ValueType * restrict x, ValueType * restrict y)
{
	extern __shared__ char sm[];
	int tidg = cuda_get_thread_num();
	int tidb = threadIdx.x;
	int block_id = blockIdx.x;
	int block_size = blockDim.x;
	ValueType * val_buf = (typeof(val_buf)) sm;
	INT_T * ia_buf = (typeof(ia_buf)) &sm[block_size * sizeof(ValueType)];
	INT_T * ja_rel;
	ValueType * a_rel;
	__attribute__((unused)) int i, i_s, i_e, j, j_s, j_e, k, l, p;
	i_s = thread_block_i_s[block_id];
	i_e = thread_block_i_e[block_id];
	j_s = thread_block_j_s[block_id];
	j_e = thread_block_j_e[block_id];
	int j_e_div = j_e - ((j_e-j_s) % block_size);
	i = i_s;
	for (j=j_s;j<j_e_div;j+=block_size)
	{

		// if (tidb == 0)
		// {
			// for (l=j;l<j+block_size;l++)
			// {
				// while (l >= row_ptr[i+1])
					// i++;
				// ia_buf[l-j] = i;
			// }
		// }

		// l = j + tidb;
		// while (l >= row_ptr[i+1])
			// i++;
		// ia_buf[l-j] = i;
		ia_buf[tidb] = ia[j+tidb];

		ja_rel = &ja[j];
		a_rel = &a[j];
		val_buf[tidb] = a_rel[tidb] * x[ja_rel[tidb]];
		// atomicAdd(&y[ia_buf[tidb]], val_buf[tidb]);
		__syncthreads();
		i = ia_buf[block_size - 1];
		for (k=1;k<block_size;k*=2)
		{
			if ((tidb & (2*k-1)) == k-1)
			{
				ValueType val = val_buf[tidb];
				int row = ia_buf[tidb];
				if (row == ia_buf[tidb+k])
				{
					val_buf[tidb+k] += val;
					// val_buf[tidb] = 0;
				}
				else
				{
					atomicAdd(&y[row], val);
				}
			}
			__syncthreads();
		}
		if (tidb == 0)
			atomicAdd(&y[ia_buf[block_size-1]], val_buf[block_size-1]);
		// if (val_buf[tidb] != 0)
			// atomicAdd(&y[ia_buf[tidb]], val_buf[tidb]);
		__syncthreads();
	}
	/* if (tidb == 0)
	{
		for (j=j_e_div;j<j_e;j++)
		{
			// y[ia[j]] += a[j] * x[ja[j]];
			// atomicAdd(&y[ia[j]], a[j] * x[ja[j]]);

			while (j >= row_ptr[i+1])
				i++;
			atomicAdd(&y[i], a[j] * x[ja[j]]);
		}
	} */
	j = j_e_div + tidb;
	if (j < j_e)
	{
		// while (j >= row_ptr[i+1])
			// i++;
		// atomicAdd(&y[i], a[j] * x[ja[j]]);
		atomicAdd(&y[ia[j]], a[j] * x[ja[j]]);
	}
}


__global__ void gpu_kernel_spmv_row_indices_continuous(INT_T * thread_block_i_s, INT_T * thread_block_i_e, INT_T * thread_block_j_s, INT_T * thread_block_j_e, INT_T * row_ptr, INT_T * ia, INT_T * ja, ValueType * a, ValueType * restrict x, ValueType * restrict y)
{
	extern __shared__ char sm[];
	int tidg = cuda_get_thread_num();
	int tidb = threadIdx.x;
	int block_id = blockIdx.x;
	int block_size = blockDim.x;
	ValueType * val_buf = (typeof(val_buf)) sm;
	INT_T * ia_buf = (typeof(ia_buf)) &sm[block_size * sizeof(ValueType)];
	INT_T * ja_rel;
	ValueType * a_rel;
	__attribute__((unused)) int i, i_s, i_e, j, j_s, j_e, k, l, p;
	i_s = thread_block_i_s[block_id];
	i_e = thread_block_i_e[block_id];
	j_s = thread_block_j_s[block_id];
	j_e = thread_block_j_e[block_id];
	int total_j = j_e - j_s;
	int j_per_t = total_j / block_size;
	int mod = total_j % block_size;
	int j_l_s, j_l_e;
	j_l_s = j_s + tidb * (total_j / block_size);
	j_l_e = j_l_s + (total_j / block_size);
	if (tidb < mod)
	{
		j_l_s += tidb;
		j_l_e += tidb + 1;
	}
	else
	{
		j_l_s += mod;
		j_l_e += mod;
	}
	// int m = (i_e + i_s) / 2;
	// while (i_s < i_e)
	// {
		// if (j_l_s >= row_ptr[m])
		// {
			// i_s = m + 1;
		// }
		// else
		// {
			// i_e = m;
		// }
		// m = (i_e + i_s) / 2;
	// }
	// i = i_s - 1;
	i = ia[j_l_s];
	// if (tidb == block_size-1)
	// {
		// if (j_l_e != j_e)
		// {
			// printf("wrong");
		// }
	// }
	double sum = 0;
	int ptr_next = row_ptr[i+1];
	for (j=j_l_s;j<j_l_e;j++)
	{
		// if (ia[j] != i)
		// {
			// atomicAdd(&y[i], sum);
			// sum = 0;
			// i = ia[j];
		// }
		if (j >= ptr_next)
		{
			atomicAdd(&y[i], sum);
			// y[i] += sum;
			sum = 0;
			while (j >= ptr_next)
			{
				i++;
				ptr_next = row_ptr[i+1];
			}
			// i = ia[j];
		}
		// sum += a[j] * x[ja[j]];
		sum = __fma_rn(a[j], x[ja[j]], sum);
	}
	// if (j_l_s < j_l_e)
		// atomicAdd(&y[i], sum);
	val_buf[tidb] = sum;
	ia_buf[tidb] = i;
	__syncthreads();
	for (k=1;k<block_size;k*=2)
	{
		if ((tidb & (2*k-1)) == k-1)
		{
			ValueType val = val_buf[tidb];
			int row = ia_buf[tidb];
			if (row == ia_buf[tidb+k])
			{
				val_buf[tidb+k] += val;
				// val_buf[tidb] = 0;
			}
			else
			{
				atomicAdd(&y[row], val);
				// y[row] += val;
			}
		}
		__syncthreads();
	}
	if (tidb == 0)
		atomicAdd(&y[ia_buf[block_size-1]], val_buf[block_size-1]);
}


void
compute_csr(CSRArrays * restrict csr, ValueType * restrict x, ValueType * restrict y)
{
	// int num_threads = csr->num_threads;
	int block_size = csr->block_size;
	int num_blocks = csr->num_blocks;
	dim3 block_dims(block_size);
	dim3 grid_dims(num_blocks);
	// long shared_mem_size = block_size * (sizeof(ValueType));
	long shared_mem_size = block_size * (sizeof(ValueType) + sizeof(INT_T));

	if (csr->x == NULL)
	{
		csr->x = x;
		cudaMemcpy(csr->x_dev, csr->x, csr->n * sizeof(*csr->x), cudaMemcpyHostToDevice);
	}

	cudaMemset(csr->y_dev, 0, csr->m * sizeof(csr->y_dev));

	// gpu_kernel_spmv_gather_multiply<<<grid_dims, block_dims>>>(thread_block_j_s_dev, thread_block_j_e_dev, csr->ja_dev, csr->a_dev, csr->x_dev, csr->multres_dev);
	// gpu_kernel_spmv_row_indices<<<grid_dims, block_dims, shared_mem_size>>>(thread_block_i_s_dev, thread_block_i_e_dev, thread_block_j_s_dev, thread_block_j_e_dev, csr->row_ptr_dev, csr->ia_dev, csr->ja_dev, csr->a_dev, csr->x_dev, csr->y_dev);
	// gpu_kernel_spmv_row_indices_atomics<<<grid_dims, block_dims, shared_mem_size>>>(thread_block_i_s_dev, thread_block_i_e_dev, thread_block_j_s_dev, thread_block_j_e_dev, csr->row_ptr_dev, csr->ia_dev, csr->ja_dev, csr->a_dev, csr->x_dev, csr->y_dev);
	gpu_kernel_spmv_row_indices_continuous<<<grid_dims, block_dims, shared_mem_size>>>(thread_block_i_s_dev, thread_block_i_e_dev, thread_block_j_s_dev, thread_block_j_e_dev, csr->row_ptr_dev, csr->ia_dev, csr->ja_dev, csr->a_dev, csr->x_dev, csr->y_dev);

	cudaError_t err;
	err = cudaDeviceSynchronize();
	if (err != cudaSuccess)
		error("cudaDeviceSynchronize: %s\n", cudaGetErrorString(err));
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

