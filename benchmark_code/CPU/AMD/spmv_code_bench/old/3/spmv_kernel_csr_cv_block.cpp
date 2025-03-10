#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

#include "macros/cpp_defines.h"

#include "spmv_bench_common.h"
#include "spmv_kernel.h"

#include <immintrin.h>

#ifdef __cplusplus
extern "C"{
#endif
	#include "macros/macrolib.h"
	#include "time_it.h"
	#include "parallel_util.h"
	#include "array_metrics.h"
	#include "x86_util.h"
	#include "spmv_subkernel_csr_x86_d.hpp"
#ifdef __cplusplus
}
#endif


#if defined(COMPRESSION_KERNEL_ID)
	#include "spmv_kernel_csr_cv_block_compression_kernels_id.h"
#elif defined(COMPRESSION_KERNEL_D2F)
	#include "spmv_kernel_csr_cv_block_compression_kernels_d2f.h"
#elif defined(COMPRESSION_KERNEL_FPC)
	#include "spmv_kernel_csr_cv_block_compression_kernels_fpc.h"
#elif defined(COMPRESSION_KERNEL_ZFP)
	#include "spmv_kernel_csr_cv_block_compression_kernels_zfp.h"
#elif defined(COMPRESSION_KERNEL_FPZIP)
	#include "spmv_kernel_csr_cv_block_compression_kernels_fpzip.h"
#endif


// Account for a fraction of the cache to leave space for indices, etc.
static inline
long
get_num_uncompressed_packet_vals()
{
	return atol(getenv("CSRCV_NUM_PACKET_VALS"));
}


INT_T * thread_i_s = NULL;
INT_T * thread_i_e = NULL;

extern int prefetch_distance;


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//------------------------------------------------------------------------------------------------------------------------------------------
//-                                                                SpMV                                                                    -
//------------------------------------------------------------------------------------------------------------------------------------------
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


static inline
void
compress_init(ValueTypeReference * vals, const long num_vals, const long packet_size)
{
	// compress_init_sort_diff(vals, num_vals, packet_size);
	compress_kernel_init(vals, num_vals, packet_size);
}


static inline
long
compress(ValueTypeReference * vals, unsigned char * buf, const long num_vals)
{
	// return compress_kernel_id(vals, buf, num_vals);
	// return compress_kernel_float(vals, buf, num_vals);
	// return compress_kernel_sort_diff(vals, buf, num_vals);
	return compress_kernel(vals, buf, num_vals);
}


static inline
long
decompress(ValueType * vals, unsigned char * buf, long * num_vals_out)
{
	// return decompress_kernel_id(vals, buf, num_vals_out);
	// return decompress_kernel_float(vals, buf, num_vals_out);
	// return decompress_kernel_sort_diff(vals, buf, num_vals_out);
	return decompress_kernel(vals, buf, num_vals_out);
}


#define CACHE_LINE_SIZE  64
struct semaphore_s {
	long producer __attribute__ ((aligned (CACHE_LINE_SIZE)));
	long consumer __attribute__ ((aligned (CACHE_LINE_SIZE)));
	char padding[0] __attribute__ ((aligned (CACHE_LINE_SIZE)));
} __attribute__ ((aligned (CACHE_LINE_SIZE)));


struct packet_info_s {
	// INT_T i_s;
	INT_T i_e;
	int size;
};


struct CSRCVArrays : Matrix_Format
{
	INT_T * row_ptr;                                  // the usual rowptr (of size m+1)
	INT_T * ja;                                  // the colidx of each NNZ (of size nnz)
	long * t_compr_data_size;                    // size of the compressed data
	unsigned char ** t_compr_data;               // the compressed values
	long * t_num_packets;                        // number of compressed data packets
	struct packet_info_s ** t_packet_info;

	double error_matrix;

	double * t_time_total, * t_time_io, * t_time_decompress, * t_time_exec;

	// void validate_grouping_method(ValueType * a);
	void calculate_matrix_compression_error(ValueType * a);

	long statistics_enabled = 0;
	CSRCVArrays * csr_prev;
	ValueType ** t_vals = NULL, ** t_vals_buf = NULL;
	unsigned char ** t_packet_data = NULL;
	long * t_num_vals = NULL;

	CSRCVArrays(INT_T * row_ptr_in, INT_T * col_ind, ValueTypeReference * values, long m, long n, long nnz) : Matrix_Format(m, n, nnz)
	{
		int num_threads = omp_get_max_threads();
		double time_balance, time_compress;
		long compr_data_size, i;

		row_ptr = (typeof(row_ptr)) aligned_alloc(64, (m+1) * sizeof(*row_ptr));
		ja = (typeof(ja)) aligned_alloc(64, nnz * sizeof(*ja));
		#pragma omp parallel for
		for (long i=0;i<m+1;i++)
			row_ptr[i] = row_ptr_in[i];
		#pragma omp parallel for
		for(long i=0;i<nnz;i++)
		{
			ja[i]=col_ind[i];
		}

		thread_i_s = (INT_T *) malloc(num_threads * sizeof(*thread_i_s));
		thread_i_e = (INT_T *) malloc(num_threads * sizeof(*thread_i_e));

		time_balance = time_it(1,
			_Pragma("omp parallel")
			{
				int tnum = omp_get_thread_num();
				loop_partitioner_balance_prefix_sums(num_threads, tnum, row_ptr, m, nnz, &thread_i_s[tnum], &thread_i_e[tnum]);
			}
		);
		printf("balance time = %g\n", time_balance);
		t_compr_data_size = (typeof(t_compr_data_size)) aligned_alloc(64, num_threads * sizeof(*t_compr_data_size));
		t_compr_data = (typeof(t_compr_data)) aligned_alloc(64, num_threads * sizeof(*t_compr_data));
		t_num_packets = (typeof(t_num_packets)) aligned_alloc(64, num_threads * sizeof(*t_num_packets));
		t_packet_info = (typeof(t_packet_info)) aligned_alloc(64, num_threads * sizeof(*t_packet_info));
		#ifdef PRINT_STATISTICS
			t_time_total = (typeof(t_time_total)) aligned_alloc(64, num_threads * sizeof(*t_time_total));
			t_time_io = (typeof(t_time_io)) aligned_alloc(64, num_threads * sizeof(*t_time_io));
			t_time_decompress = (typeof(t_time_decompress)) aligned_alloc(64, num_threads * sizeof(*t_time_decompress));
			t_time_exec = (typeof(t_time_exec)) aligned_alloc(64, num_threads * sizeof(*t_time_exec));
		#endif
		time_compress = time_it(1,
			long num_packet_vals;
			num_packet_vals = get_num_uncompressed_packet_vals();
			compress_init(values, nnz, num_packet_vals);
			_Pragma("omp parallel")
			{
				int tnum = omp_get_thread_num();
				unsigned char * data;
				struct packet_info_s * packet_info;
				long t_nnz, max_data_size;
				__attribute__((unused)) long p, i, i_s, i_e, j, j_s, j_e;
				long num_vals;
				long num_packets;
				long pos, size;
				long upper_boundary;

				i_s = thread_i_s[tnum];
				i_e = thread_i_e[tnum];
				j_s = row_ptr[i_s];
				j_e = row_ptr[i_e];

				_Pragma("omp single nowait")
				{
					printf("Number of packet vals = %ld\n", num_packet_vals);
				}

				t_nnz = j_e - j_s;
				num_packets = (t_nnz + num_packet_vals - 1) / num_packet_vals;
				// long leftover = t_nnz % num_packet_vals;

				max_data_size = 2 * t_nnz  * sizeof(ValueType);    // We assume worst case scenario:  compr_data = 2 * data
				data = (typeof(data)) aligned_alloc(64, max_data_size);

				packet_info = (typeof(packet_info)) aligned_alloc(64, (num_packets+1) * sizeof(*packet_info));

				pos = 0;
				// i = i_s;
				for (p=0,j=j_s;p<num_packets;p++,j+=num_packet_vals)
				{
					num_vals = (j + num_packet_vals <= j_e) ? num_packet_vals : j_e - j;
					size = compress(&values[j], &data[pos], num_vals);
					pos += size;
					packet_info[p].size = size;
					// printf("%d: p=%ld , num_vals=%ld , packet_i=[%d, %d] (%d) , data_pos=%ld (%ld nnz)\n", tnum, p, num_vals, packet_i_s[p], packet_i_e[p], packet_i_e[p] - packet_i_s[p], pos, (pos-sizeof(int)*num_packets)/sizeof(ValueType));
					// if (p == 0)
					// {
						// packet_info[p].i_s = i_s;
					// }
					// else
					// {
						// i = packet_info[p-1].i_e;
						// packet_info[p].i_s = j < row_ptr[i] ? i-1 : i;  // Test for partial row.
					// }
					if (p == num_packets - 1)
						packet_info[p].i_e = i_e;
					else
					{
						// Index boundaries are inclusive. 'upper_boundary' is certainly the first row after the rows belonging to the packet (last packet row can be partial).
						macros_binary_search(row_ptr, i_s, i_e, j+num_vals, NULL, &upper_boundary);
						packet_info[p].i_e = upper_boundary;
					}
					// for (;i<i_e;i++)
					// {
						// if (row_ptr[i] >= j+num_vals)
							// break;
					// }
					// if (packet_info[p].i_e != i)
						// error("p.i_e=%ld , i=%ld", packet_info[p].i_e, i);
					// if (tnum == 0)
						// printf("%d: i=[%ld,%ld] , j=%ld[%ld,%ld] , p=%ld , num_vals=%ld , packet_i=[%d, %d] (%d) , t_nnz=%ld\n",
								// tnum, i_s, i_e, j, j_s, j_e, p, num_vals, packet_i_s[p], packet_i_e[p], packet_i_e[p] - packet_i_s[p], t_nnz);
				}
				t_compr_data_size[tnum] = pos;
				t_compr_data[tnum] = data;
				t_num_packets[tnum] = num_packets;
				t_packet_info[tnum] = packet_info;
			}
		);
		printf("compression time = %g\n", time_compress);

		compr_data_size = 0;
		for (i=0;i<num_threads;i++)
			compr_data_size += t_compr_data_size[i];
		mem_footprint = compr_data_size + nnz * sizeof(INT_T) + (m+1) * sizeof(INT_T);

		ValueType * a_conv;
		a_conv = (typeof(a_conv)) malloc(nnz * sizeof(*a_conv));
		#pragma omp parallel for
		for (long i=0;i<nnz;i++)
		{
			a_conv[i] = values[i];
		}
		calculate_matrix_compression_error(a_conv);
		free(a_conv);
	}

	~CSRCVArrays()
	{
		int num_threads = omp_get_max_threads();
		long i;
		for (i=0;i<num_threads;i++)
		{
			free(t_compr_data[i]);
			free(t_packet_info[i]);
		}
		free(t_compr_data);
		free(t_packet_info);
		free(row_ptr);
		free(ja);
		free(thread_i_s);
		free(thread_i_e);
		#ifdef PRINT_STATISTICS
			free(t_time_io);
			free(t_time_decompress);
			free(t_time_exec);
		#endif
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
	struct CSRCVArrays * csr = new CSRCVArrays(row_ptr, col_ind, values, m, n, nnz);
	csr->format_name = (char *) "CSR_CV_BLOCK_" FORMAT_SUBNAME;
	return csr;
}


//==========================================================================================================================================
//= Method Validation - Errors
//==========================================================================================================================================


void
CSRCVArrays::calculate_matrix_compression_error(ValueType * a)
{
	ValueType * a_new;
	a_new = (typeof(a_new)) aligned_alloc(64, nnz * sizeof(*a_new));
	#pragma omp parallel
	{
		int tnum = omp_get_thread_num();
		long num_vals;
		long num_packets = t_num_packets[tnum];
		unsigned char * data = t_compr_data[tnum];
		long pos, p, i_s, j;
		i_s = thread_i_s[tnum];
		j = row_ptr[i_s];
		pos = 0;
		for (p=0;p<num_packets;p++)
		{
			// printf("p=%ld/%ld\n", p, num_packets);
			pos += decompress(&a_new[j], &(data[pos]), &num_vals);
			j += num_vals;
		}
		#pragma omp barrier
		// #pragma omp for
		// for (j=0;j<nnz;j++)
			// if (a[j] != a_new[j])
				// printf("%d: a=%g != a_new=%g , at pos=%ld : col=%d\n", tnum, a[j], a_new[j], j, ja[j]);
		double mae, max_ae, mse, mape, smape;
		double lnQ_error, mlare, gmare;
		array_mae_concurrent(a, a_new, nnz, &mae, val_to_double);
		array_max_ae_concurrent(a, a_new, nnz, &max_ae, val_to_double);
		array_mse_concurrent(a, a_new, nnz, &mse, val_to_double);
		array_mape_concurrent(a, a_new, nnz, &mape, val_to_double);
		array_smape_concurrent(a, a_new, nnz, &smape, val_to_double);
		array_lnQ_error_concurrent(a, a_new, nnz, &lnQ_error, val_to_double);
		array_mlare_concurrent(a, a_new, nnz, &mlare, val_to_double);
		array_gmare_concurrent(a, a_new, nnz, &gmare, val_to_double);
		#pragma omp single
		{
			printf("errors matrix: mae=%g, max_ae=%g, mse=%g, mape=%g, smape=%g, lnQ_error=%g, mlare=%g, gmare=%g\n", mae, max_ae, mse, mape, smape, lnQ_error, mlare, gmare);
		}
	}
	// for (long z=0;z<nnz;z++)
		// if (a[z] != a_new[z])
		// {
			// printf("a[%ld]=%lf , a_new[%ld]=%lf\n", z, a[z], z, a_new[z]);
			// printf("a[%ld]=%064lb , a_new[%ld]=%064lb\n", z, ((uint64_t *) a)[z], z, ((uint64_t *) a_new)[z]);
		// }
	free(a_new);
}


//==========================================================================================================================================
//= Subkernels Single Row CSR
//==========================================================================================================================================


static inline
ValueType
subkernel_row_csr(INT_T * ja, ValueType * vals, ValueType * restrict x, INT_T j_s, INT_T j_e)
{
	// ValueType sum;
	// long j, k;
	// sum = 0;
	// for (j=j_s,k=0;j<j_e;j++,k++)
	// {
		// sum += vals[k] * x[ja[j]];
	// }
	// return sum;
	// return subkernel_row_csr_scalar(ja, vals - j_s, x, j_s, j_e);
	// return subkernel_row_csr_scalar_kahan(ja, vals - j_s, x, j_s, j_e);
	// return subkernel_row_csr_vector_x86_256d(ja, vals - j_s, x, j_s, j_e);
	return subkernel_row_csr_vector_x86(ja, vals - j_s, x, j_s, j_e);
}


//==========================================================================================================================================
//= SpMV Kernel
//==========================================================================================================================================


void compute_csr_cv(CSRCVArrays * restrict csr, ValueType * restrict x , ValueType * restrict y);


void
CSRCVArrays::spmv(ValueType * x, ValueType * y)
{
	compute_csr_cv(this, x, y);
}


void
compute_csr_cv_statistics(CSRCVArrays * restrict csr, ValueType * restrict x, ValueType * restrict y)
{
	#pragma omp parallel
	{
		int tnum = omp_get_thread_num();
		ValueType * vals = csr->t_vals[tnum];
		__attribute__((unused)) unsigned char * packet_data = csr->t_packet_data[tnum];
		long num_vals;
		long num_packets = csr->t_num_packets[tnum];
		unsigned char * data = csr->t_compr_data[tnum];
		struct packet_info_s * packet_info = csr->t_packet_info[tnum];
		ValueType sum;
		long pos, p, i, i_s, i_e, j, j_e, j_packet_e, k;
		i_s = thread_i_s[tnum];
		i_e = thread_i_e[tnum];
		i = i_s;
		j = csr->row_ptr[i_s];
		pos = 0;
		double time_total = 0, time_io = 0, time_decompress = 0, time_exec = 0;
		for (p=0;p<num_packets;p++)
		{
			#define STATISTICS_DECOMPRESS_TWICE 0
			// #define STATISTICS_DECOMPRESS_TWICE 1
			__attribute__((unused)) long l;
			time_io += time_it(1,
				#if STATISTICS_DECOMPRESS_TWICE
					decompress(vals, &(data[pos]), &num_vals);
				#else
					for (l=0;l<packet_info[p].size;l++)
						packet_data[l] = data[pos + l];
					// __atomic_thread_fence(__ATOMIC_SEQ_CST);
				#endif
			);
			time_decompress += time_it(1,
				#if STATISTICS_DECOMPRESS_TWICE
					decompress(vals, &(data[pos]), &num_vals);
				#else
					decompress(vals, packet_data, &num_vals);
				#endif
			);
			pos += packet_info[p].size;
			time_exec += time_it(1,
				// printf("%d: p=%ld , num_vals=%ld , packet_i=[%d, %d] (%d) , data_pos=%ld (%ld nnz)\n", tnum, p, num_vals, packet_i_s[p], packet_i_e[p], packet_i_e[p] - packet_i_s[p], pos, (pos-sizeof(int)*num_packets)/sizeof(ValueType));
				i_e = packet_info[p].i_e;
				k = 0;
				j_packet_e = j + num_vals;
				if (j > csr->row_ptr[i])   // Partial first row.
				{
					j_e = csr->row_ptr[i+1];
					if (j_e > j_packet_e)
						j_e = j_packet_e;
					// for (long z=j;z<j_e;z++)
					// {
						// if (csr->a[z] != vals[k+z-j])
							// error("row_ptr:[%d,%d] , packet=%ld , a[%ld]=%lf , vals[%ld]=%lf", csr->row_ptr[i], csr->row_ptr[i+1], p, z, csr->a[z], z-j, vals[k+z-j]);
					// }
					sum = subkernel_row_csr(csr->ja, &vals[k], x, j, j_e);
					k += j_e - j;
					j = j_e;
					y[i] += sum;
					if (j == csr->row_ptr[i+1])
						i++;
				}
				for (;i<i_e-1;i++)  // Except last row.
				{
					j_e = csr->row_ptr[i+1];
					// for (long z=j;z<j_e;z++)
					// {
						// if (csr->a[z] != vals[k+z-j])
							// error("row_ptr:[%d,%d] , packet=%ld , a[%ld]=%lf , vals[%ld]=%lf", csr->row_ptr[i], csr->row_ptr[i+1], p, z, csr->a[z], z-j, vals[k+z-j]);
					// }
					sum = subkernel_row_csr(csr->ja, &vals[k], x, j, j_e);
					k += j_e - j;
					j = j_e;
					y[i] = sum;
				}
				// Last row might be partial.
				if (j < j_packet_e)
				{
					// j_e = j_packet_e;
					// for (long z=j;z<j_e;z++)
					// {
						// if (csr->a[z] != vals[k+z-j])
							// error("row_ptr:[%d,%d] , packet=%ld , a[%ld]=%lf , vals[%ld]=%lf", csr->row_ptr[i], csr->row_ptr[i+1], p, z, csr->a[z], z-j, vals[k+z-j]);
					// }
					sum = subkernel_row_csr(csr->ja, &vals[k], x, j, j_packet_e);
					k += j_packet_e - j;
					j = j_packet_e;
					y[i] = sum;
					if (j == csr->row_ptr[i+1])
						i++;
				}
			);
		}
		time_total = time_io + time_decompress + time_exec;
		csr->t_time_total[tnum] += time_total;
		csr->t_time_io[tnum] += time_io;
		// #if STATISTICS_DECOMPRESS_TWICE
			// csr->t_time_io[tnum] -= time_decompress;
		// #endif
		csr->t_time_decompress[tnum] += time_decompress;
		csr->t_time_exec[tnum] += time_exec;
	}
}


void
compute_csr_cv_base(CSRCVArrays * restrict csr, ValueType * restrict x, ValueType * restrict y)
{
	#pragma omp parallel
	{
		int tnum = omp_get_thread_num();
		ValueType * vals = csr->t_vals[tnum];
		long num_vals;
		long num_packets = csr->t_num_packets[tnum];
		unsigned char * data = csr->t_compr_data[tnum];
		struct packet_info_s * packet_info = csr->t_packet_info[tnum];
		ValueType sum;
		long pos, p, i, i_s, i_e, j, j_e, j_packet_e, k;
		i_s = thread_i_s[tnum];
		i_e = thread_i_e[tnum];
		i = i_s;
		j = csr->row_ptr[i_s];
		pos = 0;
		for (p=0;p<num_packets;p++)
		{
			pos += decompress(vals, &(data[pos]), &num_vals);
			// printf("%d: p=%ld , num_vals=%ld , packet_i=[%d, %d] (%d) , data_pos=%ld (%ld nnz)\n", tnum, p, num_vals, packet_i_s[p], packet_i_e[p], packet_i_e[p] - packet_i_s[p], pos, (pos-sizeof(int)*num_packets)/sizeof(ValueType));
			i_e = packet_info[p].i_e;
			k = 0;
			j_packet_e = j + num_vals;
			if (j > csr->row_ptr[i])   // Partial first row.
			{
				j_e = csr->row_ptr[i+1];
				if (j_e > j_packet_e)
					j_e = j_packet_e;
				// for (long z=j;z<j_e;z++)
				// {
					// if (csr->a[z] != vals[k+z-j])
						// error("row_ptr:[%d,%d] , packet=%ld , a[%ld]=%lf , vals[%ld]=%lf", csr->row_ptr[i], csr->row_ptr[i+1], p, z, csr->a[z], z-j, vals[k+z-j]);
				// }
				sum = subkernel_row_csr(csr->ja, &vals[k], x, j, j_e);
				k += j_e - j;
				j = j_e;
				y[i] += sum;
				if (j == csr->row_ptr[i+1])
					i++;
			}
			for (;i<i_e-1;i++)  // Except last row.
			{
				j_e = csr->row_ptr[i+1];
				// for (long z=j;z<j_e;z++)
				// {
					// if (csr->a[z] != vals[k+z-j])
						// error("row_ptr:[%d,%d] , packet=%ld , a[%ld]=%lf , vals[%ld]=%lf", csr->row_ptr[i], csr->row_ptr[i+1], p, z, csr->a[z], z-j, vals[k+z-j]);
				// }
				sum = subkernel_row_csr(csr->ja, &vals[k], x, j, j_e);
				k += j_e - j;
				j = j_e;
				y[i] = sum;
			}
			// Last row might be partial.
			if (j < j_packet_e)
			{
				// j_e = j_packet_e;
				// for (long z=j;z<j_e;z++)
				// {
					// if (csr->a[z] != vals[k+z-j])
						// error("row_ptr:[%d,%d] , packet=%ld , a[%ld]=%lf , vals[%ld]=%lf", csr->row_ptr[i], csr->row_ptr[i+1], p, z, csr->a[z], z-j, vals[k+z-j]);
				// }
				sum = subkernel_row_csr(csr->ja, &vals[k], x, j, j_packet_e);
				k += j_packet_e - j;
				j = j_packet_e;
				y[i] = sum;
				if (j == csr->row_ptr[i+1])
					i++;
			}
		}
	}
}


void
compute_csr_cv(CSRCVArrays * restrict csr, ValueType * restrict x, ValueType * restrict y)
{
	int num_threads = omp_get_max_threads();
	long num_packet_vals = get_num_uncompressed_packet_vals();
	long i;
	// printf("IN packet_vals=%ld, packets=%ld, nnz=%d, LLC_vals=%ld\n", num_packet_vals, csr->t_num_packets[0], csr->nnz, LEVEL3_CACHE_SIZE/sizeof(ValueType));
	if (csr->t_vals == NULL || csr != csr->csr_prev)
	{
		if (csr->t_vals != NULL)
		{
			free(csr->t_num_vals);
			for (i=0;i<num_threads;i++)
			{
				free(csr->t_vals[i]);
				free(csr->t_vals_buf[i]);
				free(csr->t_packet_data[i]);
			}
			free(csr->t_vals);
			free(csr->t_vals_buf);
			free(csr->t_packet_data);
		}
		csr->t_num_vals = (typeof(csr->t_num_vals)) aligned_alloc(64, num_threads * sizeof(*csr->t_num_vals));
		csr->t_vals = (typeof(csr->t_vals)) aligned_alloc(64, num_threads * sizeof(*csr->t_vals));
		csr->t_vals_buf = (typeof(csr->t_vals_buf)) aligned_alloc(64, num_threads * sizeof(*csr->t_vals_buf));
		csr->t_packet_data = (typeof(csr->t_packet_data)) aligned_alloc(64, num_threads * sizeof(*csr->t_packet_data));
		for (i=0;i<num_threads;i++)
		{
			csr->t_num_vals[i] = 0;
			csr->t_vals[i] = (ValueType *) aligned_alloc(64, num_packet_vals * sizeof(**csr->t_vals));
			csr->t_vals_buf[i] = (ValueType *) aligned_alloc(64, num_packet_vals * sizeof(**csr->t_vals_buf));
			csr->t_packet_data[i] = (unsigned char *) aligned_alloc(64, 2 * num_packet_vals * sizeof(ValueType));
		}
		csr->csr_prev = csr;
	}
	if (csr->statistics_enabled)
	{
		compute_csr_cv_statistics(csr, x, y);
		csr->statistics_enabled = 0;
	}
	else
		compute_csr_cv_base(csr, x, y);
}


//==========================================================================================================================================
//= Print Statistics
//==========================================================================================================================================


void
CSRCVArrays::statistics_start()
{
	int num_threads = omp_get_max_threads();
	long i;
	statistics_enabled = 1;
	for (i=0;i<num_threads;i++)
	{
		t_time_io[i] = 0;
		t_time_decompress[i] = 0;
		t_time_exec[i] = 0;
	}
}


int
statistics_print_labels(__attribute__((unused)) char * buf, __attribute__((unused)) long buf_n)
{
	long len = 0;
	// printf("STATISTICS: %lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf\n", tot_avg, tot_max, io_avg, io_max, decompress_avg, decompress_max, exec_avg, exec_max);
	len += snprintf(buf + len, buf_n - len, ",%s", "CSRCV_NUM_PACKET_VALS");
	len += snprintf(buf + len, buf_n - len, ",%s", "time_io_frac_avg");
	len += snprintf(buf + len, buf_n - len, ",%s", "time_decompress_frac_avg");
	len += snprintf(buf + len, buf_n - len, ",%s", "time_execute_frac_avg");
	// len += snprintf(buf + len, buf_n - len, ",%s", "time_io_frac_max");
	// len += snprintf(buf + len, buf_n - len, ",%s", "time_decompress_frac_max");
	// len += snprintf(buf + len, buf_n - len, ",%s", "time_execute_frac_max");
	return len;
}


int
CSRCVArrays::statistics_print_data(char * buf, long buf_n)
{
	int num_threads = omp_get_max_threads();

	// double time_total = 0, time_io = 0, time_decompress = 0, time_exec = 0;
	// long i;
	// for (i=0;i<num_threads;i++)
	// {
		// time_total += t_time_total[i];
		// time_io += t_time_io[i];
		// time_decompress += t_time_decompress[i];
		// time_exec += t_time_exec[i];
	// }
	// time_total = time_io + time_decompress + time_exec;
	// printf("STATISTICS: %lf,%lf,%lf,%lf\n", time_total, time_io, time_decompress, time_exec);

	double tot_avg, tot_max, io_avg, io_max, decompress_avg, decompress_max, exec_avg, exec_max;
	long len;
	array_mean(t_time_total, num_threads, &tot_avg, val_to_double);
	array_max(t_time_total, num_threads, &tot_max, NULL, val_to_double);
	array_mean(t_time_io, num_threads, &io_avg, val_to_double);
	array_max(t_time_io, num_threads, &io_max, NULL, val_to_double);
	array_mean(t_time_decompress, num_threads, &decompress_avg, val_to_double);
	array_max(t_time_decompress, num_threads, &decompress_max, NULL, val_to_double);
	array_mean(t_time_exec, num_threads, &exec_avg, val_to_double);
	array_max(t_time_exec, num_threads, &exec_max, NULL, val_to_double);
	// printf("STATISTICS: %lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf\n", tot_avg, tot_max, io_avg, io_max, decompress_avg, decompress_max, exec_avg, exec_max);

	len = 0;
	len += snprintf(buf + len, buf_n - len, ",%ld", atol(getenv("CSRCV_NUM_PACKET_VALS")));
	len += snprintf(buf + len, buf_n - len, ",%.4lf", io_avg / tot_avg);
	len += snprintf(buf + len, buf_n - len, ",%.4lf", decompress_avg / tot_avg);
	len += snprintf(buf + len, buf_n - len, ",%.4lf", exec_avg / tot_avg);
	// len += snprintf(buf + len, buf_n - len, ",%.4lf", io_max / tot_max);
	// len += snprintf(buf + len, buf_n - len, ",%.4lf", decompress_max / tot_max);
	// len += snprintf(buf + len, buf_n - len, ",%.4lf", exec_max / tot_max);
	return len;
}

