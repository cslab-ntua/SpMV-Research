#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

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
#ifdef __cplusplus
}
#endif


// Account for a fraction of the cache to leave space for indeces, etc.
static inline
long
get_num_uncompressed_packet_vals()
{
	return LEVEL3_CACHE_SIZE / sizeof(ValueType) / omp_get_max_threads() / 4;
}


extern INT_T * thread_i_s;
extern INT_T * thread_i_e;

extern INT_T * thread_j_s;
extern INT_T * thread_j_e;

extern ValueType * thread_v_s;
extern ValueType * thread_v_e;

extern int prefetch_distance;


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//------------------------------------------------------------------------------------------------------------------------------------------
//-                                                Compression / Decompression Kernels                                                     -
//------------------------------------------------------------------------------------------------------------------------------------------
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


static inline
long
compress_kernel_id(ValueType * vals, unsigned char * buf, const long num_vals)
{
	long i;
	*((int *) buf) = num_vals;
	buf += sizeof(int);
	for (i=0;i<num_vals;i++)
		((ValueType *) buf)[i] = vals[i];
	return sizeof(int) + num_vals * sizeof(ValueType);
}
static inline
long
decompress_kernel_id(ValueType * vals, unsigned char * buf, long * num_vals_out)
{
	long i;
	int num_vals = *((int *) buf);
	buf += sizeof(int);
	for (i=0;i<num_vals;i++)
		vals[i] = ((ValueType *) buf)[i];
	*num_vals_out = num_vals;
	return sizeof(int) + num_vals * sizeof(ValueType);
}


static inline
long
compress_kernel_float(ValueType * vals, unsigned char * buf, const long num_vals)
{
	long i;
	*((int *) buf) = num_vals;
	buf += sizeof(int);
	for (i=0;i<num_vals;i++)
		((float *) buf)[i] = (float) vals[i];
	return sizeof(int) + num_vals * sizeof(float);
}
static inline
long
decompress_kernel_float(ValueType * vals, unsigned char * buf, long * num_vals_out)
{
	long i;
	int num_vals = *((int *) buf);
	buf += sizeof(int);
	for (i=0;i<num_vals;i++)
		vals[i] = (ValueType) ((float *) buf)[i];
	*num_vals_out = num_vals;
	return sizeof(int) + num_vals * sizeof(float);
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//------------------------------------------------------------------------------------------------------------------------------------------
//-                                                                SpMV                                                                    -
//------------------------------------------------------------------------------------------------------------------------------------------
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


static
long
compress(ValueType * vals, unsigned char * buf, const long num_vals)
{
	// return compress_kernel_id(vals, buf, num_vals);
	return compress_kernel_float(vals, buf, num_vals);
}


static
long
decompress(ValueType * vals, unsigned char * buf, long * num_vals_out)
{
	// return decompress_kernel_id(vals, buf, num_vals_out);
	return decompress_kernel_float(vals, buf, num_vals_out);
}


struct CSRVCArrays : Matrix_Format
{
	INT_T * ia;                     // the usual rowptr (of size m+1)
	INT_T * ja;                     // the colidx of each NNZ (of size nnz)
	long * t_compr_data_size;       // size of the compressed data
	unsigned char ** t_compr_data;  // the compressed values
	long * t_num_packets;           // number of compressed data packets
	INT_T ** t_packet_i_s;
	INT_T ** t_packet_i_e;

	double error_matrix;

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
				loop_partitioner_balance_partial_sums(num_threads, tnum, ia, m, nnz, &thread_i_s[tnum], &thread_i_e[tnum]);
				// long i_s=thread_i_s[tnum], i_e=thread_i_e[tnum], t_nnz=ia[i_e]-ia[i_s];
				// printf("%d: i=[%ld, %ld] (%ld) , nnz=%ld\n", tnum, i_s, i_e, i_e - i_s, t_nnz);
			}
		);
		printf("balance time = %g\n", time_balance);
		t_compr_data_size = (typeof(t_compr_data_size)) aligned_alloc(64, num_threads * sizeof(*t_compr_data_size));
		t_compr_data = (typeof(t_compr_data)) aligned_alloc(64, num_threads * sizeof(*t_compr_data));
		t_num_packets = (typeof(t_num_packets)) aligned_alloc(64, num_threads * sizeof(*t_num_packets));
		t_packet_i_s = (typeof(t_packet_i_s)) aligned_alloc(64, num_threads * sizeof(*t_packet_i_s));
		t_packet_i_e = (typeof(t_packet_i_e)) aligned_alloc(64, num_threads * sizeof(*t_packet_i_e));
		time_compress = time_it(1,
			_Pragma("omp parallel")
			{
				int tnum = omp_get_thread_num();
				unsigned char * data;
				INT_T * packet_i_s, * packet_i_e;
				long t_nnz, max_data_size;
				long p, i, i_s, i_e, j, j_s, j_e;
				long num_packet_vals, num_vals;
				long num_packets;
				long pos;
				long upper_boundary;

				i_s = thread_i_s[tnum];
				i_e = thread_i_e[tnum];
				j_s = ia[i_s];
				j_e = ia[i_e];

				num_packet_vals = get_num_uncompressed_packet_vals();

				t_nnz = j_e - j_s;
				num_packets = (t_nnz + num_packet_vals - 1) / num_packet_vals;
				// long leftover = t_nnz % num_packet_vals;

				max_data_size = 2 * t_nnz  * sizeof(ValueType);    // We assume worst case scenario:  compr_data = 2 * data
				data = (typeof(data)) aligned_alloc(64, max_data_size);

				packet_i_s = (typeof(packet_i_s)) aligned_alloc(64, num_packets * sizeof(*packet_i_s));
				packet_i_e = (typeof(packet_i_e)) aligned_alloc(64, num_packets * sizeof(*packet_i_e));

				pos = 0;
				for (p=0,j=j_s;j<j_e;p++,j+=num_packet_vals)
				{
					num_vals = (j + num_packet_vals <= j_e) ? num_packet_vals : j_e - j;
					pos += compress(&a[j], &data[pos], num_vals);
					if (p == 0)
						packet_i_s[p] = i_s;
					else
					{
						i = packet_i_e[p-1];
						packet_i_s[p] = j < ia[i] ? i-1 : i;  // Test for partial row.
					}
					if (p == num_packets - 1)
						packet_i_e[p] = i_e;
					else
					{
						// Index boundaries are inclusive. 'upper_boundary' is certainly the first row after the rows belonging to the packet (last packet row can be partial).
						binary_search(ia, i_s, i_e, j+num_vals, NULL, &upper_boundary);
						packet_i_e[p] = upper_boundary;
					}
					// if (tnum == 0)
						// printf("%d: i=[%ld,%ld] , j=%ld[%ld,%ld] , p=%ld , num_vals=%ld , packet_i=[%d, %d] (%d) , t_nnz=%ld\n", tnum, i_s, i_e, j, j_s, j_e, p, num_vals, packet_i_s[p], packet_i_e[p], packet_i_e[p] - packet_i_s[p], t_nnz);
				}
				t_compr_data_size[tnum] = pos;
				t_compr_data[tnum] = data;
				t_num_packets[tnum] = num_packets;
				t_packet_i_s[tnum] = packet_i_s;
				t_packet_i_e[tnum] = packet_i_e;
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
			free(t_packet_i_s[i]);
			free(t_packet_i_e[i]);
		}
		free(t_compr_data);
		free(t_packet_i_s);
		free(t_packet_i_e);
		free(ia);
		free(ja);
		free(thread_i_s);
		free(thread_i_e);
	}

	void spmv(ValueType * x, ValueType * y);
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
		INT_T * packet_i_s = t_packet_i_s[tnum];
		INT_T * packet_i_e = t_packet_i_e[tnum];
		long pos, p, i, i_s, i_e, j, j_e, j_packet_e, k;
		ValueType * vals;
		vals = (typeof(vals)) aligned_alloc(64, num_packet_vals * sizeof(*vals));
		i_s = thread_i_s[tnum];
		i_e = thread_i_e[tnum];
		j = ia[i_s];
		pos = 0;
		for (p=0;p<num_packets;p++)
		{
			pos += decompress(vals, &(data[pos]), &num_vals);
			// printf("%d: p=%ld , num_vals=%ld , packet_i=[%d, %d] (%d) , data_pos=%ld (%ld nnz)\n", tnum, p, num_vals, packet_i_s[p], packet_i_e[p], packet_i_e[p] - packet_i_s[p], pos, (pos-sizeof(int)*num_packets)/sizeof(ValueType));
			i_s = packet_i_s[p];
			i_e = packet_i_e[p];
			if (i_s == i_e)
				continue;
			k = 0;
			i = i_s;
			j_packet_e = j + num_vals;
			if (j > ia[i_s])   // Partial first row.
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
		#pragma omp barrier
		#pragma omp for
		for (j=0;j<nnz;j++)
			if (a[j] != a_new[j])
				printf("%d: a=%g != a_new=%g , at pos=%ld : col=%d\n", tnum, a[j], a_new[j], j, ja[j]);
		free(vals);
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
		double mae, max_ae, mse, mape, smape;
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
		mae = array_mae_parallel(a, a_new, nnz);
		max_ae = array_max_ae_parallel(a, a_new, nnz);
		mse = array_mse_parallel(a, a_new, nnz);
		mape = array_mape_parallel(a, a_new, nnz);
		smape = array_smape_parallel(a, a_new, nnz);
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
subkernel_row_csr_scalar(ValueType * vals, CSRVCArrays * restrict csr, ValueType * restrict x, INT_T j_s, INT_T j_e)
{
	ValueType sum;
	long j, k;
	sum = 0;
	for (j=j_s,k=0;j<j_e;j++,k++)
		sum += vals[k] * x[csr->ja[j]];
	return sum;
}


static inline
ValueType
subkernel_row_csr_scalar_kahan(ValueType * vals, CSRVCArrays * restrict csr, ValueType * restrict x, INT_T j_s, INT_T j_e)
{
	ValueType sum, val, tmp, compensation = 0;
	long j, k;
	sum = 0;
	for (j=j_s,k=0;j<j_e;j++,k++)
	{
		val = vals[k] * x[csr->ja[j]] - compensation;
		tmp = sum + val;
		compensation = (tmp - sum) - val;
		sum = tmp;
	}
	return sum;
}


static inline
ValueType
subkernel_row_csr(ValueType * vals, CSRVCArrays * restrict csr, ValueType * restrict x, INT_T j_s, INT_T j_e)
{
	return subkernel_row_csr_scalar(vals, csr, x, j_s, j_e);
	// return subkernel_row_csr_scalar_kahan(vals, csr, x, j_s, j_e);
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
	#pragma omp parallel
	{
		int tnum = omp_get_thread_num();
		long num_packet_vals = get_num_uncompressed_packet_vals();
		long num_vals;
		long num_packets = csr->t_num_packets[tnum];
		unsigned char * data = csr->t_compr_data[tnum];
		INT_T * packet_i_s = csr->t_packet_i_s[tnum];
		INT_T * packet_i_e = csr->t_packet_i_e[tnum];
		ValueType sum;
		long pos, p, i, i_s, i_e, j, j_e, j_packet_e, k;
		ValueType * vals;
		vals = (typeof(vals)) aligned_alloc(64, num_packet_vals * sizeof(*vals));
		i_s = thread_i_s[tnum];
		i_e = thread_i_e[tnum];
		j = csr->ia[i_s];
		pos = 0;
		for (p=0;p<num_packets;p++)
		{
			pos += decompress(vals, &(data[pos]), &num_vals);
			i_s = packet_i_s[p];
			i_e = packet_i_e[p];
			// if (i_s == i_e)
				// continue;
			k = 0;
			i = i_s;
			j_packet_e = j + num_vals;
			if (j > csr->ia[i_s])   // Partial first row.
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
		}
		// free(vals);
	}
}

