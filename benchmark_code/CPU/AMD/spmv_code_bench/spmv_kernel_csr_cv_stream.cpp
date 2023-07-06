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

	#include "spmv_kernel_csr_cv_stream_compression_kernels.h"
	// #include "spmv_kernel_csr_cv_stream_compression_kernels_0_size_of_fraction.h"
	// #include "spmv_kernel_csr_cv_stream_compression_kernels_1_total_entry_size.h"
	// #include "spmv_kernel_csr_cv_stream_compression_kernels_2_y_buf.h"
	// #include "spmv_kernel_csr_cv_stream_compression_kernels_3_coord_bytes.h"
	// #include "spmv_kernel_csr_cv_stream_compression_kernels_4_abs.h"
	// #include "spmv_kernel_csr_cv_stream_compression_kernels_all_bytes.h"
#ifdef __cplusplus
}
#endif


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
	return atol(getenv("CSRCV_NUM_PACKET_VALS"));
}

INT_T * thread_i_s = NULL;
INT_T * thread_i_e = NULL;

extern int prefetch_distance;

// Statistics

double * thread_time_compute;

extern uint64_t * t_row_bits_accum;
extern uint64_t * t_col_bits_accum;
extern uint64_t * t_row_col_bytes_accum;


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//------------------------------------------------------------------------------------------------------------------------------------------
//-                                                                SpMV                                                                    -
//------------------------------------------------------------------------------------------------------------------------------------------
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


static inline
void
compress_init(ValueType * vals, const long num_vals, const long packet_size)
{
	compress_init_sort_diff(vals, num_vals, packet_size);
}


static inline
long
compress(INT_T * ia, INT_T * ja, ValueType * vals, long i_s, long j_s, unsigned char * buf, const long num_vals, long * num_vals_out)
{
	return compress_kernel_sort_diff(ia, ja, vals, i_s, j_s, buf, num_vals, num_vals_out);
}


static inline
long
decompress_and_compute(unsigned char * restrict buf, ValueType * restrict x, ValueType * restrict y)
{
	return decompress_and_compute_kernel_sort_diff(buf, x, y);
}


static inline
long
decompress(ValueType * vals, unsigned char * restrict buf)
{
	return decompress_kernel_sort_diff(vals, buf);
}


struct CSRCVSArrays : Matrix_Format
{
	INT_T * ia;                                  // the usual rowptr (of size m+1)
	INT_T * ja;                                  // the colidx of each NNZ (of size nnz)
	long * t_compr_data_size;                    // size of the compressed data
	unsigned char ** t_compr_data;               // the compressed values
	long * t_num_packets;                        // number of compressed data packets
	double * a;

	double error_matrix;

	void calculate_matrix_compression_error(ValueType * a_new);

	CSRCVSArrays(INT_T * ia, INT_T * ja, ValueType * a, long m, long n, long nnz) : Matrix_Format(m, n, nnz), ia(ia), ja(ja), a(a)
	{
		int num_threads = omp_get_max_threads();
		double time_balance, time_compress;
		long compr_data_size, i;
		double * a_new;

		thread_i_s = (typeof(thread_i_s)) malloc(num_threads * sizeof(*thread_i_s));
		thread_i_e = (typeof(thread_i_e)) malloc(num_threads * sizeof(*thread_i_e));

		thread_time_compute = (typeof(thread_time_compute)) malloc(num_threads * sizeof(*thread_time_compute));

		a_new = (typeof(a_new)) aligned_alloc(64, nnz * sizeof(*a_new));

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
		time_compress = time_it(1,
			long num_packet_vals;

			num_packet_vals = get_num_uncompressed_packet_vals();
			compress_init(a, nnz, num_packet_vals);

			_Pragma("omp parallel")
			{
				int tnum = omp_get_thread_num();
				unsigned char * data;
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

				max_data_size = t_nnz * (sizeof(*ia) + sizeof(*ja) + 2 * sizeof(ValueType));    // We assume worst case scenario:  compr_data = 2 * data
				data = (typeof(data)) aligned_alloc(64, max_data_size);

				pos = 0;
				i = i_s;
				for (p=0,j=j_s;p<num_packets;p++,j+=num_packet_vals)
				{
					// Correctness testing for binary_search().
					long tmp = i;
					while (ia[tmp] < j)
						tmp++;
					if (j != ia[tmp])
						tmp--;

					// Index boundaries are inclusive. 'upper_boundary' is certainly the first row after the rows belonging to the packet (last packet row can be partial).
					binary_search(ia, i_s, i_e, j, NULL, &upper_boundary);
					i = upper_boundary;
					if (j != ia[i])
						i--;

					if (i != tmp)
						error("wrong i = %ld, correct i = %ld", i, tmp);

					num_vals = (j + num_packet_vals <= j_e) ? num_packet_vals : j_e - j;
					size = compress(ia, ja, &a[j], i, j, &data[pos], num_vals, &num_vals);

					// for (;i<i_e;i++)
					// {
						// if (ia[i] >= j+num_vals)
							// break;
					// }
					// if (tnum == 0)
						// printf("%d: i=[%ld,%ld] , j=%ld[%ld,%ld] , p=%ld , num_vals=%ld , packet_i=[%d, %d] (%d) , t_nnz=%ld\n",
								// tnum, i_s, i_e, j, j_s, j_e, p, num_vals, packet_i_s[p], packet_i_e[p], packet_i_e[p] - packet_i_s[p], t_nnz);

					long s = decompress(&a_new[j], &data[pos]);
					if (s != size)
						error("size compress = %ld != size decompress = %ld", size, s);
					pos += size;
				}
				t_compr_data_size[tnum] = pos;
				t_compr_data[tnum] = data;
				t_num_packets[tnum] = num_packets;
			}
		);
		printf("compression time = %g\n", time_compress);

		// for (i=0;i<num_threads;i++)
		// {
			// printf("row_diff_max=%ld , col_diff_max=%ld\n", t_total_row_diff_max[i], t_total_col_diff_max[i]);
		// }

		compr_data_size = 0;
		for (i=0;i<num_threads;i++)
			compr_data_size += t_compr_data_size[i];
		mem_footprint = compr_data_size;

		calculate_matrix_compression_error(a_new);
		free(a_new);
		// exit(0);
	}

	~CSRCVSArrays()
	{
		int num_threads = omp_get_max_threads();
		long i;
		for (i=0;i<num_threads;i++)
		{
			free(t_compr_data[i]);
		}
		free(t_compr_data);
		free(ia);
		free(ja);
		free(thread_i_s);
		free(thread_i_e);
	}

	void spmv(ValueType * x, ValueType * y);
	void statistics_start();
	int statistics_print(__attribute__((unused)) char * buf, __attribute__((unused)) long buf_n);
};


struct Matrix_Format *
csr_to_format(INT_T * row_ptr, INT_T * col_ind, ValueType * values, long m, long n, long nnz)
{
	struct CSRCVSArrays * csr = new CSRCVSArrays(row_ptr, col_ind, values, m, n, nnz);
	csr->format_name = (char *) "CSR_CVS";
	return csr;
}


//==========================================================================================================================================
//= Method Validation - Errors
//==========================================================================================================================================


void
CSRCVSArrays::calculate_matrix_compression_error(double * a_new)
{
	#pragma omp parallel
	{
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
	// for (long z=0;z<nnz;z++)
	// {
		// printf("a[%ld]=%lf , a_new[%ld]=%lf\n", z, a[z], z, a_new[z]);
		// if (a[z] != a_new[z])
		// {
			// printf("a[%ld]=%lf , a_new[%ld]=%lf\n", z, a[z], z, a_new[z]);
			// printf("a[%ld]=%064lb , a_new[%ld]=%064lb\n", z, ((uint64_t *) a)[z], z, ((uint64_t *) a_new)[z]);
		// }
	// }
}


//==========================================================================================================================================
//= SpMV Kernel
//==========================================================================================================================================


void compute_csr_cv_stream(CSRCVSArrays * restrict csr, ValueType * restrict x , ValueType * restrict y);


void
CSRCVSArrays::spmv(ValueType * x, ValueType * y)
{
	compute_csr_cv_stream(this, x, y);
}


void
compute_csr_cv_stream(CSRCVSArrays * restrict csr, ValueType * restrict x, ValueType * restrict y)
{
	// int num_threads = omp_get_max_threads();
	#pragma omp parallel
	{
		int tnum = omp_get_thread_num();
		long num_packets = csr->t_num_packets[tnum];
		unsigned char * data = csr->t_compr_data[tnum];
		long pos, p, i, i_s, i_e;
		i_s = thread_i_s[tnum];
		i_e = thread_i_e[tnum];
		i = i_s;
		pos = 0;
		thread_time_compute[tnum] += time_it(1,
			for (i=i_s;i<i_e;i++)
				y[i] = 0;
			for (p=0;p<num_packets;p++)
			{
				pos += decompress_and_compute(&(data[pos]), x, y);
			}
		);
	}
}


//==========================================================================================================================================
//= Print Statistics
//==========================================================================================================================================


void
CSRCVSArrays::statistics_start()
{
	int num_threads = omp_get_max_threads();
	long i;
	for (i=0;i<num_threads;i++)
	{
		thread_time_compute[i] = 0;
	}
}


int
CSRCVSArrays::statistics_print(char * buf, long buf_n)
{
	int num_threads = omp_get_max_threads();

	long num_packets = 0;

	double time_max = 0, time_avg = 0;

	double data_size_max = 0, data_size_avg = 0;

	double row_bits_avg = 0;
	double col_bits_avg = 0;
	double row_col_bytes_avg = 0;

	long i, len;

	for (i=0;i<num_threads;i++)
	{
		num_packets += t_num_packets[i];

		time_avg += thread_time_compute[i];
		if (thread_time_compute[i] > time_max)
			time_max = thread_time_compute[i];

		data_size_avg += t_compr_data_size[i];
		if (t_compr_data_size[i] > data_size_max)
			data_size_max = t_compr_data_size[i];

		row_bits_avg += t_row_bits_accum[i];
		col_bits_avg += t_col_bits_accum[i];
		row_col_bytes_avg += t_row_col_bytes_accum[i];
	}
	time_avg /= num_threads;
	data_size_avg /= num_threads;
	row_bits_avg /= num_packets;
	col_bits_avg /= num_packets;
	row_col_bytes_avg /= num_packets;
	len = 0;
	len += snprintf(buf + len, buf_n - len, ",%.2lf", (time_max - time_avg) / time_avg * 100); // unbalance time
	len += snprintf(buf + len, buf_n - len, ",%.2lf",  (data_size_max - data_size_avg) / data_size_avg * 100); // unbalance size
	len += snprintf(buf + len, buf_n - len, ",%.4lf",  row_bits_avg);
	len += snprintf(buf + len, buf_n - len, ",%.4lf",  col_bits_avg);
	len += snprintf(buf + len, buf_n - len, ",%.4lf",  row_col_bytes_avg);
	return len;
}

