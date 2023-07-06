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
	#include "topology.h"
	#include "time_it.h"
	#include "parallel_util.h"
	#include "array_metrics.h"
	#include "x86_util.h"
	#include "spmv_subkernel_csr_x86_d.hpp"
#ifdef __cplusplus
}
#endif

#include "spmv_kernel_csr_vc_compression_kernels.h"


// Account for a fraction of the cache to leave space for indeces, etc.
static inline
long
get_num_uncompressed_packet_vals()
{
	// return LEVEL3_CACHE_SIZE / sizeof(ValueType) / omp_get_max_threads() / 4;
	// return LEVEL3_CACHE_SIZE / sizeof(ValueType) / omp_get_max_threads() / 64;
	// return LEVEL3_CACHE_SIZE / sizeof(ValueType) / omp_get_max_threads() / 128;
	// return LEVEL2_CACHE_SIZE / sizeof(ValueType) / omp_get_max_threads() / 4;
	// return LEVEL1_DCACHE_SIZE / sizeof(ValueType) / omp_get_max_threads() / 4;
	// return 512;
	return atol(getenv("CSRVC_NUM_PACKET_VALS"));
}


INT_T * thread_i_s = NULL;
INT_T * thread_i_e = NULL;

extern int prefetch_distance;


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//------------------------------------------------------------------------------------------------------------------------------------------
//-                                                                SpMV                                                                    -
//------------------------------------------------------------------------------------------------------------------------------------------
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
static
void
compress_init(ValueType * vals, const long num_vals, const long packet_size)
{
	compress_init_sd(vals, num_vals, packet_size);
}


// static
// void
// decompress_init(ValueType * vals, const long num_vals, const long packet_size)
// {
	// decompress_init_sd(vals, num_vals, packet_size);
// }
#pragma GCC diagnostic pop


static
long
compress(ValueType * vals, unsigned char * buf, const long num_vals)
{
	return compress_kernel_id(vals, buf, num_vals);
	// return compress_kernel_float(vals, buf, num_vals);
	// return compress_kernel_fpc(vals, buf, num_vals);
	// return compress_kernel_sd(vals, buf, num_vals);
}


static
long
decompress(ValueType * vals, unsigned char * buf, long * num_vals_out)
{
	return decompress_kernel_id(vals, buf, num_vals_out);
	// return decompress_kernel_float(vals, buf, num_vals_out);
	// return decompress_kernel_fpc(vals, buf, num_vals_out);
	// return decompress_kernel_sd(vals, buf, num_vals_out);
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


struct CSRVCArrays : Matrix_Format
{
	INT_T * ia;                                  // the usual rowptr (of size m+1)
	INT_T * ja;                                  // the colidx of each NNZ (of size nnz)
	long * t_compr_data_size;                    // size of the compressed data
	unsigned char ** t_compr_data;               // the compressed values
	long * t_num_packets;                        // number of compressed data packets
	struct packet_info_s ** t_packet_info;

	double error_matrix;

	double * t_time_total, * t_time_io, * t_time_decompress, * t_time_exec;

	void validate_grouping_method(ValueType * a);
	void calculate_matrix_compression_error(ValueType * a);

	CSRVCArrays(INT_T * ia, INT_T * ja, ValueType * a, long m, long n, long nnz) : Matrix_Format(m, n, nnz), ia(ia), ja(ja)
	{
		int num_threads = omp_get_max_threads();
		double time_balance, time_compress;
		long compr_data_size, i;

		thread_i_s = (INT_T *) malloc(num_threads * sizeof(*thread_i_s));
		thread_i_e = (INT_T *) malloc(num_threads * sizeof(*thread_i_e));

		time_balance = time_it(1,
			_Pragma("omp parallel")
			{
				int tnum = omp_get_thread_num();
				loop_partitioner_balance_prefix_sums(num_threads, tnum, ia, m, nnz, &thread_i_s[tnum], &thread_i_e[tnum]);
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
			compress_init(a, nnz, num_packet_vals);
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
				j_s = ia[i_s];
				j_e = ia[i_e];

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
				for (p=0,j=j_s;j<j_e;p++,j+=num_packet_vals)
				{
					num_vals = (j + num_packet_vals <= j_e) ? num_packet_vals : j_e - j;
					size = compress(&a[j], &data[pos], num_vals);
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
						// packet_info[p].i_s = j < ia[i] ? i-1 : i;  // Test for partial row.
					// }
					if (p == num_packets - 1)
						packet_info[p].i_e = i_e;
					else
					{
						// Index boundaries are inclusive. 'upper_boundary' is certainly the first row after the rows belonging to the packet (last packet row can be partial).
						binary_search(ia, i_s, i_e, j+num_vals, NULL, &upper_boundary);
						packet_info[p].i_e = upper_boundary;
					}
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

		calculate_matrix_compression_error(a);
		// validate_grouping_method(a);
	}

	~CSRVCArrays()
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
		free(ia);
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
	void statistics_print();
};


struct Matrix_Format *
csr_to_format(INT_T * row_ptr, INT_T * col_ind, ValueType * values, long m, long n, long nnz)
{
	struct CSRVCArrays * csr = new CSRVCArrays(row_ptr, col_ind, values, m, n, nnz);
	csr->format_name = (char *) "CSR_VC";
	return csr;
}


//==========================================================================================================================================
//= Method Validation - Errors
//==========================================================================================================================================


void
CSRVCArrays::validate_grouping_method(ValueType * a)
{
	ValueType * a_new;
	a_new = (typeof(a_new)) aligned_alloc(64, nnz * sizeof(*a_new));
	#pragma omp parallel
	{
		int tnum = omp_get_thread_num();
		long num_packet_vals = get_num_uncompressed_packet_vals();
		long num_vals;
		long num_packets = t_num_packets[tnum];
		unsigned char * data = t_compr_data[tnum];
		struct packet_info_s * packet_info = t_packet_info[tnum];
		long pos, p, i, i_s, i_e, j, j_e, j_packet_e, k;
		ValueType * vals;
		vals = (typeof(vals)) aligned_alloc(64, num_packet_vals * sizeof(*vals));
		i_s = thread_i_s[tnum];
		i_e = thread_i_e[tnum];
		i = i_s;
		j = ia[i_s];
		pos = 0;
		for (p=0;p<num_packets;p++)
		{
			pos += decompress(vals, &(data[pos]), &num_vals);
			// printf("%d: p=%ld , num_vals=%ld , packet_i=[%d, %d] (%d) , data_pos=%ld (%ld nnz)\n", tnum, p, num_vals, packet_i_s[p], packet_i_e[p], packet_i_e[p] - packet_i_s[p], pos, (pos-sizeof(int)*num_packets)/sizeof(ValueType));
			i_e = packet_info[p].i_e;
			k = 0;
			j_packet_e = j + num_vals;
			if (j > ia[i])   // Partial first row.
			{
				j_e = ia[i+1];
				if (j_e > j_packet_e)
					j_e = j_packet_e;
				a_new[j] = vals[k];
				// if (vals[k] != a[j]) printf("%d: a=%g != a_new=%g , at pos=%ld : row=%ld  col=%d\n", tnum, a[j], vals[k], j, i, ja[j]);
				i++;
			}
			for (;i<i_e-1;i++)  // Except last row.
			{
				j_e = ia[i+1];
				a_new[j] = vals[k];
				// if (vals[k] != a[j]) printf("%d: a=%g != a_new=%g , at pos=%ld : row=%ld  col=%d\n", tnum, a[j], vals[k], j, i, ja[j]);
			}
			// Last row might be partial.
			for (;j<j_packet_e;j++,k++)
			{
				a_new[j] = vals[k];
				// if (vals[k] != a[j]) printf("%d: a=%g != a_new=%g , at pos=%ld : row=%ld  col=%d\n", tnum, a[j], vals[k], j, i, ja[j]);
			}
		}
		free(vals);
		#pragma omp barrier
		#pragma omp for
		for (j=0;j<nnz;j++)
			if (a[j] != a_new[j])
				printf("%d: a=%g != a_new=%g , at pos=%ld : col=%d\n", tnum, a[j], a_new[j], j, ja[j]);
	}
	free(a_new);
}


void
CSRVCArrays::calculate_matrix_compression_error(ValueType * a)
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
		j = ia[i_s];
		pos = 0;
		for (p=0;p<num_packets;p++)
		{
			pos += decompress(&a_new[j], &(data[pos]), &num_vals);
			j += num_vals;
		}
		#pragma omp barrier
		// #pragma omp for
		// for (j=0;j<nnz;j++)
			// if (a[j] != a_new[j])
				// printf("%d: a=%g != a_new=%g , at pos=%ld : col=%d\n", tnum, a[j], a_new[j], j, ja[j]);
		double mae, max_ae, mse, mape, smape;
		mae = array_mae_concurrent(a, a_new, nnz, val_to_double);
		max_ae = array_max_ae_concurrent(a, a_new, nnz, val_to_double);
		mse = array_mse_concurrent(a, a_new, nnz, val_to_double);
		mape = array_mape_concurrent(a, a_new, nnz, val_to_double);
		smape = array_smape_concurrent(a, a_new, nnz, val_to_double);
		#pragma omp single
		{
			printf("errors matrix: mae=%g, max_ae=%g, mse=%g, mape=%g, smape=%g\n", mae, max_ae, mse, mape, smape);
		}
	}
	free(a_new);
}


//==========================================================================================================================================
//= Subkernels Single Row CSR
//==========================================================================================================================================


static inline
ValueType
subkernel_row_csr(ValueType * vals, CSRVCArrays * restrict csr, ValueType * restrict x, INT_T j_s, INT_T j_e)
{
	// return subkernel_row_csr_scalar(csr->ja, vals - j_s, csr, x, j_s, j_e);
	// return subkernel_row_csr_scalar_kahan(csr->ja, vals - j_s, csr, x, j_s, j_e);
	// return subkernel_row_csr_vector_x86_256d(csr->ja, vals - j_s, x, j_s, j_e);
	return subkernel_row_csr_vector_x86(csr->ja, vals - j_s, x, j_s, j_e);
}


//==========================================================================================================================================
//= SpMV Kernel
//==========================================================================================================================================


void compute_csr_vc(CSRVCArrays * restrict csr, ValueType * restrict x , ValueType * restrict y);


void
CSRVCArrays::spmv(ValueType * x, ValueType * y)
{
	compute_csr_vc(this, x, y);
}


void
compute_csr_vc(CSRVCArrays * restrict csr, ValueType * restrict x, ValueType * restrict y)
{
	int num_threads = omp_get_max_threads();
	static CSRVCArrays * csr_prev;
	static ValueType ** t_vals = NULL, ** t_vals_buf = NULL;
	static unsigned char ** t_packet_data = NULL;
	static long * t_num_vals = NULL;

	long num_packet_vals = get_num_uncompressed_packet_vals();
	long i;
	// printf("IN packet_vals=%ld, packets=%ld, nnz=%d, LLC_vals=%ld\n", num_packet_vals, csr->t_num_packets[0], csr->nnz, LEVEL3_CACHE_SIZE/sizeof(ValueType));
	if (t_vals == NULL || csr != csr_prev)
	{
		if (t_vals != NULL)
		{
			free(t_num_vals);
			for (i=0;i<num_threads;i++)
			{
				free(t_vals[i]);
				free(t_vals_buf[i]);
				free(t_packet_data[i]);
			}
			free(t_vals);
			free(t_packet_data);
			free(t_vals_buf);
		}
		t_num_vals = (typeof(t_num_vals)) aligned_alloc(64, num_threads * sizeof(*t_num_vals));
		t_vals = (typeof(t_vals)) aligned_alloc(64, num_threads * sizeof(*t_vals));
		t_vals_buf = (typeof(t_vals_buf)) aligned_alloc(64, num_threads * sizeof(*t_vals_buf));
		t_packet_data = (typeof(t_packet_data)) aligned_alloc(64, num_threads * sizeof(*t_packet_data));
		for (i=0;i<num_threads;i++)
		{
			t_num_vals[i] = 0;
			t_vals[i] = (ValueType *) aligned_alloc(64, num_packet_vals * sizeof(**t_vals));
			t_vals_buf[i] = (ValueType *) aligned_alloc(64, num_packet_vals * sizeof(**t_vals_buf));
			t_packet_data[i] = (unsigned char *) aligned_alloc(64, 2 * num_packet_vals * sizeof(ValueType));
		}
		csr_prev = csr;
	}
	#pragma omp parallel
	{
		int tnum = omp_get_thread_num();
		ValueType * vals = t_vals[tnum];
		#ifdef PRINT_STATISTICS
			__attribute__((unused)) unsigned char * packet_data = t_packet_data[tnum];
		#endif
		long num_vals;
		long num_packets = csr->t_num_packets[tnum];
		unsigned char * data = csr->t_compr_data[tnum];
		struct packet_info_s * packet_info = csr->t_packet_info[tnum];
		ValueType sum;
		long pos, p, i, i_s, i_e, j, j_e, j_packet_e, k;
		double time_total = 0, time_io = 0, time_decompress = 0, time_exec = 0;
		i_s = thread_i_s[tnum];
		i_e = thread_i_e[tnum];
		i = i_s;
		j = csr->ia[i_s];
		pos = 0;
		#pragma omp barrier
		#ifdef PRINT_STATISTICS
		time_total = time_it(1,
		#endif
		for (p=0;p<num_packets;p++)
		{
			#define STATISTICS_DECOMPRESS_TWICE 0
			// #define STATISTICS_DECOMPRESS_TWICE 1
			#ifdef PRINT_STATISTICS
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
			#else
				pos += decompress(vals, &(data[pos]), &num_vals);
			#endif
			#ifdef PRINT_STATISTICS
			time_exec += time_it(1,
			#endif
				// printf("%d: p=%ld , num_vals=%ld , packet_i=[%d, %d] (%d) , data_pos=%ld (%ld nnz)\n", tnum, p, num_vals, packet_i_s[p], packet_i_e[p], packet_i_e[p] - packet_i_s[p], pos, (pos-sizeof(int)*num_packets)/sizeof(ValueType));
				i_e = packet_info[p].i_e;
				k = 0;
				j_packet_e = j + num_vals;
				if (j > csr->ia[i])   // Partial first row.
				{
					j_e = csr->ia[i+1];
					if (j_e > j_packet_e)
						j_e = j_packet_e;
					sum = subkernel_row_csr(&vals[k], csr, x, j, j_e);
					k += j_e - j;
					j = j_e;
					y[i] += sum;
					i++;
				}
				// while (i < i_e && csr->ia[i+1] <= j_packet_e)
				// {
					// j_e = csr->ia[i+1];
					// sum = subkernel_row_csr(&vals[k], csr, x, j, j_e);
					// k += j_e - j;
					// j = j_e;
					// y[i] = sum;
					// i++;
				// }
				for (;i<i_e-1;i++)  // Except last row.
				{
					j_e = csr->ia[i+1];
					sum = subkernel_row_csr(&vals[k], csr, x, j, j_e);
					k += j_e - j;
					j = j_e;
					y[i] = sum;
				}
				// Last row might be partial.
				if (j < j_packet_e)
				{
					sum = subkernel_row_csr(&vals[k], csr, x, j, j_packet_e);
					k += j_packet_e - j;
					j = j_packet_e;
					y[i] = sum;
				}
			#ifdef PRINT_STATISTICS
			);
			#endif
		}
		#ifdef PRINT_STATISTICS
		);
		#endif
		#ifdef PRINT_STATISTICS
			csr->t_time_total[tnum] += time_total;
			csr->t_time_io[tnum] += time_io;
			// #if STATISTICS_DECOMPRESS_TWICE
				// csr->t_time_io[tnum] -= time_decompress;
			// #endif
			csr->t_time_decompress[tnum] += time_decompress;
			csr->t_time_exec[tnum] += time_exec;
		#endif
	}
}


//==========================================================================================================================================
//= Print Statistics
//==========================================================================================================================================


void
CSRVCArrays::statistics_start()
{
	int num_threads = omp_get_max_threads();
	long i;
	for (i=0;i<num_threads;i++)
	{
		t_time_io[i] = 0;
		t_time_decompress[i] = 0;
		t_time_exec[i] = 0;
	}
}


void
CSRVCArrays::statistics_print()
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
	tot_avg        = array_mean(t_time_total, num_threads, val_to_double);
	tot_max        = array_max(t_time_total, num_threads, NULL, val_to_double);
	io_avg         = array_mean(t_time_io, num_threads, val_to_double);
	io_max         = array_max(t_time_io, num_threads, NULL, val_to_double);
	decompress_avg = array_mean(t_time_decompress, num_threads, val_to_double);
	decompress_max = array_max(t_time_decompress, num_threads, NULL, val_to_double);
	exec_avg       = array_mean(t_time_exec, num_threads, val_to_double);
	exec_max       = array_max(t_time_exec, num_threads, NULL, val_to_double);
	printf("STATISTICS: %lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf\n", tot_avg, tot_max, io_avg, io_max, decompress_avg, decompress_max, exec_avg, exec_max);
}

