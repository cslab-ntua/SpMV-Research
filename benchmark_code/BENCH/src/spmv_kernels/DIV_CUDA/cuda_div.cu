#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

#include <cuda.h>
#include <cooperative_groups.h>
// #include <cuda_pipeline_primitives.h>
using namespace cooperative_groups;

#include "macros/cpp_defines.h"

#include "../spmv_kernel.h"

// #include <x86intrin.h>

#ifdef __cplusplus
extern "C" {
#endif
	#include "macros/macrolib.h"
	#include "time_it.h"
	#include "parallel_util.h"
	#include "array_metrics.h"
	// #include "x86_util.h"

	#include "lock/lock_util.h"

	#include "aux/csr_converter.h"

	#include "aux/dynamic_array.h"

	#include "cuda/cuda_util.h"
#ifdef __cplusplus
}
#endif


#undef MEMORY_ALIGNMENT
#define MEMORY_ALIGNMENT  128

#undef MEMORY_ALIGNMENT_PACKET
#define MEMORY_ALIGNMENT_PACKET  8


#undef BLOCK_SIZE
// #define BLOCK_SIZE  32
// #define BLOCK_SIZE  64
#define BLOCK_SIZE  128
// #define BLOCK_SIZE  256
// #define BLOCK_SIZE  512
// #define BLOCK_SIZE  1024


#if defined(DIV_TYPE_RF)
	#include "div_kernels_rf.h"
#else
	#include "cuda_div_kernels.h"
#endif


extern int prefetch_distance;

// Statistics

double time_compress;
extern long num_loops_out;


static inline
double
val_to_double(void * A, long i)
{
	return (double) ((ValueType *) A)[i];
}

static inline
double
coord_to_double(void * A, long i)
{
	return (double) ((INT_T *) A)[i];
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//------------------------------------------------------------------------------------------------------------------------------------------
//-                                                                SpMV                                                                    -
//------------------------------------------------------------------------------------------------------------------------------------------
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


long
reduce_add_long(long a, long b)
{
	return a + b;
}

double
reduce_add_double(double a, double b)
{
	return a + b;
}

double
reduce_min_double(double a, double b)
{
	return a < b ? a : b;
}


struct DIVArray : Matrix_Format
{
	INT_T * row_ptr;                             // the usual rowptr (of size m+1)
	INT_T * ja;                                  // the colidx of each NNZ (of size nnz)
	ValueTypeReference * a;
	long symmetric;
	long symmetry_expanded;

	ValueType * x = NULL;
	ValueType * y = NULL;
	ValueType * x_d = NULL;
	ValueType * y_d = NULL;

	double matrix_mae, matrix_max_ae, matrix_mse, matrix_mape, matrix_smape;
	double matrix_lnQ_error, matrix_mlare, matrix_gmare;

	int num_threads;
	int thread_block_size;
	int num_thread_blocks;

	long num_packets;

	long * packet_data_offsets;
	long * packet_data_offsets_d;

	long * packet_nnz_offsets;
	long * packet_nnz_offsets_d;

	long compr_data_size;
	unsigned char * compr_data;
	unsigned char * compr_data_d;

	void calculate_matrix_compression_error(ValueType * a_conv, INT_T * csr_row_ptr_new, INT_T * csr_ja_new, ValueType * csr_a_new);

	DIVArray(INT_T * row_ptr, INT_T * ja, ValueTypeReference * a, long m, long n, long nnz, long symmetric, long symmetry_expanded) : Matrix_Format(m, n, nnz), row_ptr(row_ptr), ja(ja), a(a), symmetric(symmetric), symmetry_expanded(symmetry_expanded)
	{
		time_compress = time_it(1,
			const long num_packet_vals = atol(getenv("CSRCV_NUM_PACKET_VALS"));
			compress_init_div(a, nnz, num_packet_vals);

			/* cudaError_t cudaMalloc (void ** devPtr, size_t size)
			 *     Allocates size bytes of linear memory on the device and returns in *devPtr a pointer to the allocated memory.
			 *     The allocated memory is suitably aligned for any kind of variable.
			 *     The memory is not cleared. cudaMalloc() returns cudaErrorMemoryAllocation in case of failure.
			 *
			 *     The pointers which are allocated by using any of the CUDA Runtime's device memory allocation functions, e.g, cudaMalloc or cudaMallocPitch,
			 *     are guaranteed to be 256 byte aligned, i.e. the address is a multiple of 256.
			 */
			gpuCudaErrorCheck(cudaMalloc(&x_d, n * sizeof(*x_d)));
			gpuCudaErrorCheck(cudaMalloc(&y_d, m * sizeof(*y_d)));

			_Pragma("omp parallel")
			{
				long num_threads = omp_get_max_threads();
				long tnum = omp_get_thread_num();
				unsigned char * packet_buf;
				long max_packet_size;
				__attribute__((unused)) long i, i_s, i_e, j, j_s, j_e, k;
				long num_vals, num_vals_max;
				long size;
				long pos;
				long t_num_packets;

				loop_partitioner_balance_prefix_sums(num_threads, tnum, row_ptr, m, nnz, &i_s, &i_e);

				j_s = row_ptr[i_s];
				j_e = row_ptr[i_e];


				long t_nnz;
				t_nnz = j_e - j_s;
				const long num_packets_estimated = t_nnz / num_packet_vals + 64;
				dynarray_l * da_packet_data_offsets;
				dynarray_l * da_packet_nnz_offsets;
				dynarray_uc * da_compr_data;

				da_packet_data_offsets = dynarray_new_l(num_packets_estimated * sizeof(*(da_packet_data_offsets->data)));
				da_packet_nnz_offsets = dynarray_new_l(num_packets_estimated * sizeof(*(da_packet_nnz_offsets->data)));

				long da_init_size = t_nnz * (sizeof(*row_ptr) + sizeof(*ja) + sizeof(ValueType));
				if (symmetric)
					da_init_size /= 2;
				da_compr_data = dynarray_new_uc(da_init_size);


				_Pragma("omp single")
				{
					printf("Number of packet vals = %ld\n", num_packet_vals);
				}

				/* We also add 'sizeof(struct packet_header)' for the case of extremely small matrices. */
				max_packet_size = sizeof(struct packet_header) + num_packet_vals * (sizeof(*row_ptr) + sizeof(*ja) + 8 * sizeof(ValueType));
				packet_buf = (typeof(packet_buf)) aligned_alloc(MEMORY_ALIGNMENT, max_packet_size);

				i = i_s;
				t_num_packets = 0;
				j = j_s;
				long nnz_offset = 0;
				while (j < j_e)
				{
					while (row_ptr[i] < j)
						i++;
					if (row_ptr[i] != j)
						i--;
					// Remove empty rows.
					while (row_ptr[i] == row_ptr[i+1])
						i++;

					num_vals_max = (j + num_packet_vals <= j_e) ? num_packet_vals : j_e - j;
					compress_kernel_div(row_ptr, ja, &a[j], symmetric, i, i_s, i_e, j, packet_buf, num_vals_max, &num_vals, &size);
					if (size > max_packet_size)
						error("data buffer overflow");

					if (size > 0)
					{
						pos = dynarray_push_back_array_aligned_uc(da_compr_data, packet_buf, size, MEMORY_ALIGNMENT_PACKET);
						dynarray_push_back_l(da_packet_data_offsets, pos);
						dynarray_push_back_l(da_packet_nnz_offsets, nnz_offset);
						nnz_offset += num_vals;
						t_num_packets++;
					}

					j += num_vals;
				}

				while (da_compr_data->size % MEMORY_ALIGNMENT_PACKET)   // Padding for correct alignment of the data of the next thread.
				{
					dynarray_push_back_uc(da_compr_data, 0);
				}

				// omp_thread_reduce_global(_reduce_fun, _partial, _zero, exclusive, _backwards, _local_result_ptr_ret, _total_result_ptr_ret)

				long num_packets_prefix_sum;
				omp_thread_reduce_global(reduce_add_long, t_num_packets, 0, 1, 0, &num_packets_prefix_sum, &num_packets);

				long num_vals_prefix_sum;
				omp_thread_reduce_global(reduce_add_long, t_nnz, 0, 1, 0, &num_vals_prefix_sum, );
				for (i=0;i<t_num_packets;i++)
					da_packet_nnz_offsets->data[i] += num_vals_prefix_sum;

				long compr_data_size_prefix_sum;
				omp_thread_reduce_global(reduce_add_long, da_compr_data->size, 0, 1, 0, &compr_data_size_prefix_sum, &compr_data_size);
				for (i=0;i<t_num_packets;i++)
					da_packet_data_offsets->data[i] += compr_data_size_prefix_sum;

				_Pragma("omp single")
				{
					packet_data_offsets = (typeof(packet_data_offsets)) aligned_alloc(MEMORY_ALIGNMENT, num_packets *  sizeof(*packet_data_offsets));
					packet_nnz_offsets = (typeof(packet_nnz_offsets)) aligned_alloc(MEMORY_ALIGNMENT, num_packets * sizeof(*packet_nnz_offsets));
					compr_data = (typeof(compr_data)) aligned_alloc(MEMORY_ALIGNMENT, compr_data_size);
				}

				dynarray_copy_to_array_l(da_packet_data_offsets, &packet_data_offsets[num_packets_prefix_sum]);
				dynarray_destroy_l(&da_packet_data_offsets);

				dynarray_copy_to_array_l(da_packet_nnz_offsets, &packet_nnz_offsets[num_packets_prefix_sum]);
				dynarray_destroy_l(&da_packet_nnz_offsets);

				dynarray_copy_to_array_uc(da_compr_data, &compr_data[compr_data_size_prefix_sum]);
				dynarray_destroy_uc(&da_compr_data);

			}
		);
		printf("compression time = %g\n", time_compress);

		// for (long i=0;i<num_packets;i++)
		// {
			// printf("%5ld: %10ld %10ld\n", i, packet_data_offsets[i], packet_nnz_offsets[i]);
		// }

		thread_block_size = BLOCK_SIZE;
		num_thread_blocks = num_packets;
		num_threads = num_thread_blocks * thread_block_size;

		gpuCudaErrorCheck(cudaMalloc(&packet_data_offsets_d, num_packets * sizeof(*packet_data_offsets)));
		gpuCudaErrorCheck(cudaMemcpy(packet_data_offsets_d, packet_data_offsets, num_packets * sizeof(*packet_data_offsets), cudaMemcpyHostToDevice));

		gpuCudaErrorCheck(cudaMalloc(&packet_nnz_offsets_d, num_packets * sizeof(*packet_nnz_offsets)));
		gpuCudaErrorCheck(cudaMemcpy(packet_nnz_offsets_d, packet_nnz_offsets, num_packets * sizeof(*packet_nnz_offsets), cudaMemcpyHostToDevice));

		gpuCudaErrorCheck(cudaMalloc(&compr_data_d, compr_data_size));
		gpuCudaErrorCheck(cudaMemcpy(compr_data_d, compr_data, compr_data_size, cudaMemcpyHostToDevice));


		mem_footprint = compr_data_size;


		/* Decompress and calculate compression error. */

		INT_T * coo_ia_new_d;
		INT_T * coo_ja_new_d;
		ValueType * coo_a_new_d;
		gpuCudaErrorCheck(cudaMalloc(&coo_ia_new_d, nnz * sizeof(*coo_ia_new_d)));
		gpuCudaErrorCheck(cudaMalloc(&coo_ja_new_d, nnz * sizeof(*coo_ja_new_d)));
		gpuCudaErrorCheck(cudaMalloc(&coo_a_new_d, nnz * sizeof(*coo_a_new_d)));

		INT_T * coo_ia_new = (typeof(coo_ia_new)) aligned_alloc(MEMORY_ALIGNMENT, nnz * sizeof(*coo_ia_new));
		INT_T * coo_ja_new = (typeof(coo_ja_new)) aligned_alloc(MEMORY_ALIGNMENT, nnz * sizeof(*coo_ja_new));
		ValueType * coo_a_new = (typeof(coo_a_new)) aligned_alloc(MEMORY_ALIGNMENT, nnz * sizeof(*coo_a_new));

		INT_T * csr_row_ptr_new = (typeof(csr_row_ptr_new)) aligned_alloc(MEMORY_ALIGNMENT, (m+1) * sizeof(*csr_row_ptr_new));
		INT_T * csr_ja_new = (typeof(csr_ja_new)) aligned_alloc(MEMORY_ALIGNMENT, nnz * sizeof(*csr_ja_new));
		ValueType * csr_a_new = (typeof(csr_a_new)) aligned_alloc(MEMORY_ALIGNMENT, nnz * sizeof(*csr_a_new));

		dim3 block_dims(BLOCK_SIZE);
		dim3 grid_dims(num_thread_blocks);
		// long shared_mem_size = BLOCK_SIZE * (sizeof(ValueType));
		// long shared_mem_size = BLOCK_SIZE * (sizeof(ValueType) + sizeof(INT_T));
		// long shared_mem_size = 256 * sizeof(ValueType);
		long shared_mem_size = BLOCK_SIZE * sizeof(uint8_t);
		// long shared_mem_size = 0;
		decompress_kernel_div<<<grid_dims, block_dims, shared_mem_size>>>(coo_ia_new_d, coo_ja_new_d, coo_a_new_d, compr_data_d, packet_data_offsets_d, packet_nnz_offsets_d);

		gpuCudaErrorCheck(cudaDeviceSynchronize());

		gpuCudaErrorCheck(cudaMemcpy(coo_ia_new, coo_ia_new_d, nnz * sizeof(*coo_ia_new_d), cudaMemcpyDeviceToHost));
		gpuCudaErrorCheck(cudaMemcpy(coo_ja_new, coo_ja_new_d, nnz * sizeof(*coo_ja_new_d), cudaMemcpyDeviceToHost));
		gpuCudaErrorCheck(cudaMemcpy(coo_a_new, coo_a_new_d, nnz * sizeof(*coo_a_new_d), cudaMemcpyDeviceToHost));

		gpuCudaErrorCheck(cudaDeviceSynchronize());

		gpuCudaErrorCheck(cudaFree(coo_ia_new_d));
		gpuCudaErrorCheck(cudaFree(coo_ja_new_d));
		gpuCudaErrorCheck(cudaFree(coo_a_new_d));

		_Pragma("omp parallel")
		{
			long i;
			#pragma omp for
			for (i=0;i<nnz;i++)
			{
				if ((coo_ia_new[i] < 0) || (coo_ia_new[i] >= m))
					error("coo_ia_new[%ld] = %d", i, coo_ia_new[i]);
				if ((coo_ja_new[i] < 0) || (coo_ja_new[i] >= n))
					error("coo_ja_new[%ld] = %d, n=%ld", i, coo_ja_new[i], n);
			}
		}

		// for (long i=0;i<nnz;i++)
			// printf("%10d %10d %g\n", coo_ia_new[i], coo_ja_new[i], coo_a_new[i]);

		coo_to_csr(coo_ia_new, coo_ja_new, coo_a_new, m, n, nnz, csr_row_ptr_new, csr_ja_new, csr_a_new, 1, 0);

		ValueType * a_conv;
		a_conv = (typeof(a_conv)) aligned_alloc(MEMORY_ALIGNMENT, nnz * sizeof(*a_conv));
		#pragma omp parallel for
		for (long i=0;i<nnz;i++)
		{
			a_conv[i] = a[i];
		}
		calculate_matrix_compression_error(a_conv, csr_row_ptr_new, csr_ja_new, csr_a_new);
		free(a_conv);

		free(coo_ia_new);
		free(coo_ja_new);
		free(coo_a_new);
		free(csr_row_ptr_new);
		free(csr_ja_new);
		free(csr_a_new);
		// exit(0);
	}


	~DIVArray()
	{
		free(row_ptr);
		free(ja);
	}


	void spmv(ValueType * x, ValueType * y);
	void statistics_start();
	int statistics_print_data(__attribute__((unused)) char * buf, __attribute__((unused)) long buf_n);
};


struct Matrix_Format *
csr_to_format(INT_T * row_ptr, INT_T * col_ind, ValueTypeReference * values, long m, long n, long nnz, long symmetric, long symmetry_expanded)
{
	if (symmetric && !symmetry_expanded)
		error("symmetric matrices have to be expanded to be supported by this format");
	struct DIVArray * csr = new DIVArray(row_ptr, col_ind, values, m, n, nnz, symmetric, symmetry_expanded);
	#if defined(DIV_TYPE_RF)
		csr->format_name = (char *) "DIV_CUDA_RF";
	#else
		csr->format_name = (char *) "DIV_CUDA";
	#endif
	return csr;
}


//==========================================================================================================================================
//= Method Validation - Errors
//==========================================================================================================================================


void
DIVArray::calculate_matrix_compression_error(ValueType * a_conv, INT_T * csr_row_ptr_new, INT_T * csr_ja_new, ValueType * csr_a_new)
{
	#pragma omp parallel
	{
		long i, j;

		#pragma omp for
		for (i=0;i<m;i++)
		{
			if (csr_row_ptr_new[i] != row_ptr[i])
				error("csr_row_ptr_new[%ld]=%ld , row_ptr[%ld]=%ld", i, csr_row_ptr_new[i], i, row_ptr[i]);
			if (csr_row_ptr_new[i+1] != row_ptr[i+1])
				error("csr_row_ptr_new[%ld+1]=%ld , row_ptr[%ld+1]=%ld", i, csr_row_ptr_new[i+1], i, row_ptr[i+1]);
			for (j=csr_row_ptr_new[i];j<csr_row_ptr_new[i+1];j++)
			{
				if (csr_ja_new[j] != ja[j])
					error("csr_ja_new[%ld]=%ld , ja[%ld]=%ld", j, csr_ja_new[j], j, ja[j]);
				if (csr_a_new[j] != a_conv[j])
					printf("csr_a_new[%ld]=%g, a_conv[%ld]=%g   (row=%ld, col=%d)\n", j, csr_a_new[j], j, a_conv[j], i, ja[j]);
			}
		}


		double mae, max_ae, mse, mape, smape;
		double lnQ_error, mlare, gmare;
		array_mae_concurrent(a_conv, csr_a_new, nnz, &mae, val_to_double);
		array_max_ae_concurrent(a_conv, csr_a_new, nnz, &max_ae, val_to_double);
		array_mse_concurrent(a_conv, csr_a_new, nnz, &mse, val_to_double);
		array_mape_concurrent(a_conv, csr_a_new, nnz, &mape, val_to_double);
		array_smape_concurrent(a_conv, csr_a_new, nnz, &smape, val_to_double);
		array_lnQ_error_concurrent(a_conv, csr_a_new, nnz, &lnQ_error, val_to_double);
		array_mlare_concurrent(a_conv, csr_a_new, nnz, &mlare, val_to_double);
		array_gmare_concurrent(a_conv, csr_a_new, nnz, &gmare, val_to_double);
		#pragma omp single
		{
			matrix_mae = mae;
			matrix_max_ae = max_ae;
			matrix_mse = mse;
			matrix_mape = mape;
			matrix_smape = smape;
			matrix_lnQ_error = lnQ_error;
			matrix_mlare = mlare;
			matrix_gmare = gmare;
			printf("errors matrix: mae=%g, max_ae=%g, mse=%g, mape=%g, smape=%g, lnQ_error=%g, mlare=%g, gmare=%g\n", mae, max_ae, mse, mape, smape, lnQ_error, mlare, gmare);
		}
	}
	// for (long z=0;z<nnz;z++)
	// {
		// if (a_conv[z] != csr_a_new[z])
		// {
			// printf("a_conv[%ld]=%lf , csr_a_new[%ld]=%lf\n", z, a_conv[z], z, csr_a_new[z]);
			// printf("a_conv[%ld]=%064lb , csr_a_new[%ld]=%064lb\n", z, ((uint64_t *) a_conv)[z], z, ((uint64_t *) csr_a_new)[z]);
		// }
	// }
}


//==========================================================================================================================================
//= SpMV Kernel
//==========================================================================================================================================


void compute_div(DIVArray * restrict csr, ValueType * restrict x , ValueType * restrict y);


void
DIVArray::spmv(ValueType * x, ValueType * y)
{
	compute_div(this, x, y);
}


void
compute_div(DIVArray * restrict csr, ValueType * restrict x, ValueType * restrict y)
{
	dim3 block_dims(BLOCK_SIZE);
	dim3 grid_dims(csr->num_thread_blocks);
	// long shared_mem_size = BLOCK_SIZE * (sizeof(ValueType));
	// long shared_mem_size = BLOCK_SIZE * (sizeof(ValueType) + sizeof(INT_T));
	// long shared_mem_size = 256 * sizeof(ValueType);
	long shared_mem_size = BLOCK_SIZE * sizeof(uint8_t);
	// long shared_mem_size = 0;

	if (csr->x == NULL)
	{
		printf("Grid : {%d, %d, %d} blocks. Blocks : {%d, %d, %d} threads.\n", grid_dims.x, grid_dims.y, grid_dims.z, block_dims.x, block_dims.y, block_dims.z);
		csr->x = x;
		gpuCudaErrorCheck(cudaMemcpy(csr->x_d, x, csr->n * sizeof(*csr->x_d), cudaMemcpyHostToDevice));
	}

	gpuCudaErrorCheck(cudaMemset(csr->y_d, 0, csr->m * sizeof(*csr->y_d)));

	decompress_and_compute_kernel_div<<<grid_dims, block_dims, shared_mem_size>>>(csr->compr_data_d, csr->packet_data_offsets_d, csr->x_d, csr->y_d);
	gpuCudaErrorCheck(cudaDeviceSynchronize());

	if (csr->y == NULL)
	{
		csr->y = y;
		gpuCudaErrorCheck(cudaMemcpy(y, csr->y_d, csr->m * sizeof(*csr->y_d), cudaMemcpyDeviceToHost));
	}
}


//==========================================================================================================================================
//= Print Statistics
//==========================================================================================================================================


void
DIVArray::statistics_start()
{
}


int
statistics_print_labels(char * buf, long buf_n)
{
	long len = 0;
	len += snprintf(buf + len, buf_n - len, ",%s", "CSRCV_NUM_PACKET_VALS");
	len += snprintf(buf + len, buf_n - len, ",%s", "matrix_mae");
	len += snprintf(buf + len, buf_n - len, ",%s", "matrix_max_ae");
	len += snprintf(buf + len, buf_n - len, ",%s", "matrix_mse");
	len += snprintf(buf + len, buf_n - len, ",%s", "matrix_mape");
	len += snprintf(buf + len, buf_n - len, ",%s", "matrix_smape");
	len += snprintf(buf + len, buf_n - len, ",%s", "matrix_lnQ_error");
	len += snprintf(buf + len, buf_n - len, ",%s", "matrix_mlare");
	len += snprintf(buf + len, buf_n - len, ",%s", "matrix_gmare");
	len += snprintf(buf + len, buf_n - len, ",%s", "unbalance_size");
	len += snprintf(buf + len, buf_n - len, ",%s", "compression_time");
	len += snprintf(buf + len, buf_n - len, ",%s", "tolerance");
	return len;
}


int
DIVArray::statistics_print_data(char * buf, long buf_n)
{
	int num_threads = omp_get_max_threads();

	double data_size_max = 0, data_size_avg = 0;

	long len;

	data_size_avg /= num_threads;

	double tolerance = atof(getenv("DIV_VC_TOLERANCE"));

	len = 0;
	len += snprintf(buf + len, buf_n - len, ",%ld", atol(getenv("CSRCV_NUM_PACKET_VALS")));
	len += snprintf(buf + len, buf_n - len, ",%g", matrix_mae);
	len += snprintf(buf + len, buf_n - len, ",%g", matrix_max_ae);
	len += snprintf(buf + len, buf_n - len, ",%g", matrix_mse);
	len += snprintf(buf + len, buf_n - len, ",%g", matrix_mape);
	len += snprintf(buf + len, buf_n - len, ",%g", matrix_smape);
	len += snprintf(buf + len, buf_n - len, ",%g", matrix_lnQ_error);
	len += snprintf(buf + len, buf_n - len, ",%g", matrix_mlare);
	len += snprintf(buf + len, buf_n - len, ",%g", matrix_gmare);
	len += snprintf(buf + len, buf_n - len, ",%.2lf",  (data_size_max - data_size_avg) / data_size_avg * 100); // unbalance size
	len += snprintf(buf + len, buf_n - len, ",%.4lf",  time_compress);
	len += snprintf(buf + len, buf_n - len, ",%g",  tolerance);
	return len;
}

