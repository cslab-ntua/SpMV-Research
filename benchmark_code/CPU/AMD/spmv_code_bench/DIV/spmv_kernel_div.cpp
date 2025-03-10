#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

#include "macros/cpp_defines.h"

#include "../spmv_bench_common.h"
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

	#include "topology/hardware_topology.h"
	#include "topology/task_topology.h"

	#include "aux/csr_converter.h"

	#include "aux/dynamic_array.h"

#ifdef __cplusplus
}
#endif


#if defined(DIV_TYPE_ADAPT)
	static long DV_ENABLED = 1;
	static long RF_ENABLED = 1;
#endif


struct [[gnu::aligned(64)]] padded_int64_t {
	int64_t val [[gnu::aligned(64)]];
	char padding[0] [[gnu::aligned(64)]];
};


struct [[gnu::aligned(64)]] thread_data {
	long task_num;
	long num_tasks;
	long task_l3_node_num;
	long num_task_l3_nodes;
	long task_mem_node_num;
	long num_task_mem_nodes;
	long l3_size;
	long l3_size_total;

	long i_s;
	long i_e;

	long num_vals;
	long num_packets_stealable;                        // number of compressed data packets
	long compr_data_size_stealable;                    // size of the compressed data
	unsigned char * compr_data_stealable;              // the compressed values

	long num_packets_protected;                        // non-stealable
	long compr_data_size_protected;                    // non-stealable
	unsigned char * compr_data_protected;              // non-stealable

	long num_packet_groups;
	long * packet_group_pos;

	// Statistics

	double time_compute;

	uint64_t row_bits_accum;
	uint64_t col_bits_accum;
	uint64_t row_col_bytes_accum;

	double packet_unique_values_fraction_accum;

	// Stealing.

	long finished [[gnu::aligned(64)]];
	int tnum_owner;
	int task_l3_node_num_owner;
	int task_mem_node_num_owner;
	long pg_e;

	long pg_current [[gnu::aligned(64)]];

	long steal_lock [[gnu::aligned(64)]];

	char padding[0] [[gnu::aligned(64)]];
};

static struct thread_data ** tds;


static
void
init_thread_statistics(struct thread_data * td)
{
	td->time_compute = 0;
	td->row_bits_accum = 0;
	td->col_bits_accum = 0;
	td->row_col_bytes_accum = 0;
	td->packet_unique_values_fraction_accum = 0;
}


#if defined(DIV_TYPE_RF)
	#include "spmv_kernel_div_kernels_rf.h"
#elif defined(DIV_TYPE_ADAPT)
	#include "spmv_kernel_div_kernels_adapt.h"
#elif defined(DIV_TYPE_RF_CONST_SIZE_ROW)
	#include "spmv_kernel_div_kernels_rf_const_size_row.h"
#elif defined(DIV_TYPE_SELECT)
	#include "spmv_kernel_div_kernels_select.h"
#elif defined(DIV_TYPE_ORD2)
	#include "spmv_kernel_div_kernels_rf_ord2_buf.h"
	// #include "spmv_kernel_div_kernels_rf_ord2.h"
#elif defined(DIV_TYPE_COLS_SORT)
	#include "spmv_kernel_div_kernels_cols_sort.h"
#elif defined(DIV_TYPE_SYM_RF_LOCAL)
	#include "spmv_kernel_div_sym_kernels_rf_local_split_new_packet.h"
	// #include "spmv_kernel_div_sym_kernels_rf_local_split_new_packet_accum.h"
#else
	#include "spmv_kernel_div_kernels.h"
#endif


extern int prefetch_distance;

// Statistics

double time_compress;
extern long num_loops_out;


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


//------------------------------------------------------------------------------------------------------------------------------------------
//- Bucketsort
//------------------------------------------------------------------------------------------------------------------------------------------

#include "sort/bucketsort/bucketsort_gen_undef.h"
#define BUCKETSORT_GEN_TYPE_1  int
#define BUCKETSORT_GEN_TYPE_2  int
#define BUCKETSORT_GEN_TYPE_3  int
#define BUCKETSORT_GEN_TYPE_4  void
#define BUCKETSORT_GEN_SUFFIX  _packet_type
#include "sort/bucketsort/bucketsort_gen.c"
static inline
int
bucketsort_find_bucket(int * A, long i, [[gnu::unused]] void * unused)
{
		return A[i];
}

#include "sort/quicksort/quicksort_gen_undef.h"
#define QUICKSORT_GEN_TYPE_1  int
#define QUICKSORT_GEN_TYPE_2  int
#define QUICKSORT_GEN_TYPE_3  int
#define QUICKSORT_GEN_SUFFIX  _packet_type
#include "sort/quicksort/quicksort_gen.c"
static inline
int
quicksort_cmp(int a, int b, int * cols)
{
	return cols[a] > cols[b] ? 1 : cols[a] < cols[b] ? -1 : 0;
}


struct DIVArray : Matrix_Format
{
	INT_T * row_ptr;                             // the usual rowptr (of size m+1)
	INT_T * ja;                                  // the colidx of each NNZ (of size nnz)
	ValueTypeReference * a;
	long symmetric;
	long symmetry_expanded;

	ValueType * y_prot;    // For non-stealable nnz.

	ValueType * x_bench, * y_bench;

	struct topohw_topology * tp_hw = topohw_get_topology();
	struct topotk_topology * tp_tk = topotk_get_topology();

	struct padded_int64_t finished_threads;

	double matrix_mae, matrix_max_ae, matrix_mse, matrix_mape, matrix_smape;
	double matrix_lnQ_error, matrix_mlare, matrix_gmare;

	void calculate_matrix_compression_error(ValueType * a_conv, INT_T * csr_row_ptr_new, INT_T * csr_ja_new, ValueType * csr_a_new);

	DIVArray(INT_T * row_ptr, INT_T * ja, ValueTypeReference * a, long m, long n, long nnz, long symmetric, long symmetry_expanded) : Matrix_Format(m, n, nnz), row_ptr(row_ptr), ja(ja), a(a), symmetric(symmetric), symmetry_expanded(symmetry_expanded)
	{
		long num_threads = omp_get_max_threads();
		long num_tasks = topotk_get_num_tasks();
		long l3_size = topohw_get_cache_size(0, 3, TOPOHW_CT_U);
		double time_balance;
		long i;

		tds = (typeof(tds)) aligned_alloc(64, num_threads * sizeof(*tds));

		y_prot = (typeof(y_prot)) malloc(m * sizeof(*y_prot));

		time_balance = time_it(1,
			_Pragma("omp parallel")
			{
				long tnum = omp_get_thread_num();
				long task_num = topotk_get_task_num();
				struct thread_data * td;

				long task_l3_node_num = topotk_get_task_cache_node_num(3);
				long num_task_l3_nodes = topotk_get_num_task_cache_nodes(3);

				long task_mem_node_num = topotk_get_task_mem_node_num();
				long num_task_mem_nodes = topotk_get_num_task_mem_nodes();

				long i_s, i_e;

				loop_partitioner_balance_prefix_sums(num_threads, tnum, row_ptr, m, nnz, &i_s, &i_e);

				td = (typeof(td)) aligned_alloc(64, sizeof(*td));

				td->task_num = task_num;
				td->num_tasks = num_tasks;
				td->task_l3_node_num = task_l3_node_num;
				td->num_task_l3_nodes = num_task_l3_nodes;
				td->task_mem_node_num = task_mem_node_num;
				td->num_task_mem_nodes = num_task_mem_nodes;
				td->i_s = i_s;
				td->i_e = i_e;
				td->l3_size = l3_size;
				td->l3_size_total = num_task_l3_nodes * l3_size;

				tds[tnum] = td;

				init_thread_statistics(td);
			}
		);
		printf("balance time = %g\n", time_balance);

		time_compress = time_it(1,
			const long num_packet_vals = atol(getenv("CSRCV_NUM_PACKET_VALS"));
			compress_init_div(a, nnz, num_packet_vals);

			const long packet_reordering = atol(getenv("DIV_PACKET_REORDERING"));

			_Pragma("omp parallel")
			{
				long tnum = omp_get_thread_num();
				struct thread_data * td = tds[tnum];
				unsigned char * packet_buf_stealable;
				unsigned char * packet_buf_protected;
				long t_nnz;
				long max_packet_size;
				__attribute__((unused)) long ps, pp, i, i_s, i_e, j, j_s, j_e, k;
				long num_vals, num_vals_max, num_vals_total=0;
				long size_stealable, size_protected;
				dynarray_uc * da_compr_data_stealable;
				dynarray_uc * da_compr_data_protected;
				dynarray_l * da_packet_group_pos;
				dynarray_l * da_packet_sort_bucket;

				i_s = td->i_s;
				i_e = td->i_e;
				j_s = row_ptr[i_s];
				j_e = row_ptr[i_e];

				_Pragma("omp single")
				{
					printf("Number of packet vals = %ld\n", num_packet_vals);
				}

				t_nnz = j_e - j_s;

				long da_init_size = t_nnz * (sizeof(*row_ptr) + sizeof(*ja) + sizeof(ValueType));
				if (symmetric)
					da_init_size /= 2;
				da_compr_data_stealable = dynarray_new_uc(da_init_size);
				da_compr_data_protected = dynarray_new_uc(da_init_size);

				const long num_packets_estimated = t_nnz / num_packet_vals + 64;
				da_packet_group_pos = dynarray_new_l(num_packets_estimated);
				da_packet_sort_bucket = dynarray_new_l(num_packets_estimated);

				/* We also add 'sizeof(struct packet_header)' for the case of extremely small matrices. */
				max_packet_size = sizeof(struct packet_header) + num_packet_vals * (sizeof(*row_ptr) + sizeof(*ja) + 8 * sizeof(ValueType));
				packet_buf_stealable = (typeof(packet_buf_stealable)) aligned_alloc(64, max_packet_size);
				packet_buf_protected = (typeof(packet_buf_protected)) aligned_alloc(64, max_packet_size);

				#if defined(DIV_TYPE_ADAPT)
					adapt_statistics_concurrent(packet_buf_stealable, packet_buf_protected);
				#endif

				i = i_s;
				ps = 0;
				pp = 0;
				j = j_s;
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
					compress_kernel_div(row_ptr, ja, &a[j], symmetric, i, i_s, i_e, j, packet_buf_stealable, packet_buf_protected, num_vals_max, &num_vals, &size_stealable, &size_protected);
					if (size_stealable > max_packet_size)
						error("data buffer overflow");
					if (size_protected > max_packet_size)
						error("data buffer overflow");
					num_vals_total += num_vals;

					if (size_stealable > 0)
					{
						dynarray_push_back_array_uc(da_compr_data_stealable, packet_buf_stealable, size_stealable);
						ps++;
					}
					if (size_protected > 0)
					{
						dynarray_push_back_array_uc(da_compr_data_protected, packet_buf_protected, size_protected);
						pp++;
					}

					if (packet_reordering)
					{
						double col;
						// array_mean_serial(&ja[j], num_vals, &col, coord_to_double);
						array_quantile_serial(&ja[j], num_vals, 0.5, NULL, &col, coord_to_double);

						// if (tnum == 0)
						// {
							// double m1, m2, m3, m4, m5;
							// array_quantile_serial(&ja[j], num_vals, 0.0, NULL, &m1, coord_to_double);
							// array_quantile_serial(&ja[j], num_vals, 0.25, NULL, &m2, coord_to_double);
							// array_quantile_serial(&ja[j], num_vals, 0.5, NULL, &m3, coord_to_double);
							// array_quantile_serial(&ja[j], num_vals, 0.75, NULL, &m4, coord_to_double);
							// array_quantile_serial(&ja[j], num_vals, 1.0, NULL, &m5, coord_to_double);
							// printf("%ld %ld %ld %ld %ld\n", (long) m1, (long) m2, (long) m3, (long) m4, (long) m5);
						// }

						dynarray_push_back_l(da_packet_sort_bucket, (long) col);
					}

					// If it starts at the beginning of a row then start a new packet group.
					#if defined(DIV_TYPE_ADAPT)
						// long s;
						// if (symmetric)
						// {
							// s = get_packet_size(packet_buf_stealable);
							// if (s < size_stealable)
							// {
								// dynarray_push_back_l(da_packet_group_pos, 0);
								// da_packet_group_pos->data[da_packet_group_pos->size-1] += s;
								// s = get_packet_size(&packet_buf_stealable[s]);
								// p++;
							// }
							// dynarray_push_back_l(da_packet_group_pos, 0);
							// da_packet_group_pos->data[da_packet_group_pos->size-1] += s;
						// }
						// else
						{
							if (j == row_ptr[i])
								dynarray_push_back_l(da_packet_group_pos, 0);
							da_packet_group_pos->data[da_packet_group_pos->size-1] += size_stealable;
						}
					#else
						if (j == row_ptr[i])
							dynarray_push_back_l(da_packet_group_pos, 0);
						da_packet_group_pos->data[da_packet_group_pos->size-1] += size_stealable;
					#endif

					j += num_vals;

				}
				td->num_vals = num_vals_total;

				td->num_packets_stealable = ps;
				td->compr_data_size_stealable = da_compr_data_stealable->size;
				dynarray_export_array_uc(da_compr_data_stealable, &td->compr_data_stealable);   // Handles empty da correctly.
				dynarray_destroy_uc(&da_compr_data_stealable);

				td->num_packets_protected = pp;
				td->compr_data_size_protected = da_compr_data_protected->size;
				dynarray_export_array_uc(da_compr_data_protected, &td->compr_data_protected);
				dynarray_destroy_uc(&da_compr_data_protected);


				td->num_packet_groups = da_packet_group_pos->size;
				long tmp, sum;
				sum = 0;
				for (k=0;k<da_packet_group_pos->size;k++)
				{
					tmp = da_packet_group_pos->data[k];
					da_packet_group_pos->data[k] = sum;
					sum += tmp;
				}
				dynarray_push_back_l(da_packet_group_pos, sum);

				dynarray_export_array_l(da_packet_group_pos, &td->packet_group_pos);
				dynarray_destroy_l(&da_packet_group_pos);

				if (packet_reordering)
				{
					int * rev_permutation;
					int * bucket_id;
					rev_permutation = (typeof(rev_permutation)) malloc(td->num_packet_groups * sizeof(*rev_permutation));
					bucket_id = (typeof(bucket_id)) malloc(td->num_packet_groups * sizeof(*bucket_id));
					for (k=0;k<td->num_packet_groups;k++)
					{
						rev_permutation[k] = k;
						// bucket_id[k] = packet_type_id(&da_compr_data_stealable->data[td->packet_group_pos[k]]);
						bucket_id[k] = da_packet_sort_bucket->data[k];
					}
					quicksort(rev_permutation, td->num_packet_groups, bucket_id, NULL);
					// if (tnum == 0)
					// {
						// for (k=0;k<td->num_packet_groups;k++)
							// printf("%d %d\n", bucket_id[k], rev_permutation[k]);
					// }

					unsigned char * data_new = (typeof(data_new)) aligned_alloc(64, td->compr_data_size_stealable);
					long * packet_group_pos_new = (typeof(packet_group_pos_new)) malloc((td->num_packet_groups + 1) * sizeof(*packet_group_pos_new));
					long len;
					sum = 0;
					for (k=0;k<td->num_packet_groups;k++)
					{
						packet_group_pos_new[k] = sum;
						len = td->packet_group_pos[rev_permutation[k]+1] - td->packet_group_pos[rev_permutation[k]];
						sum += len;
						for (i=0;i<len;i++)
						{
							data_new[packet_group_pos_new[k] + i] = td->compr_data_stealable[td->packet_group_pos[rev_permutation[k]] + i];
						}
					}
					packet_group_pos_new[td->num_packet_groups] = sum;
					if (sum != td->compr_data_size_stealable)
						error("test");

					free(td->packet_group_pos);
					td->packet_group_pos = packet_group_pos_new;
					free(td->compr_data_stealable);
					td->compr_data_stealable = data_new;
					free(rev_permutation);
					free(bucket_id);
				}
			}
		);
		printf("compression time = %g\n", time_compress);

		long nnz_new= 0;
		mem_footprint = 0;
		for (i=0;i<num_threads;i++)
		{
			mem_footprint += tds[i]->compr_data_size_stealable;
			mem_footprint += tds[i]->compr_data_size_protected;
			mem_footprint += (tds[i]->num_packet_groups + 1) * sizeof(*tds[i]->packet_group_pos);
			nnz_new += tds[i]->num_vals;
		}
		if (nnz_new != nnz)
			error("nnz_new = %ld != nnz = %ld", nnz_new, nnz);

		/* Decompress and calculate compression error. */

		INT_T * coo_ia_new = (typeof(coo_ia_new)) aligned_alloc(64, nnz * sizeof(*coo_ia_new));
		INT_T * coo_ja_new = (typeof(coo_ja_new)) aligned_alloc(64, nnz * sizeof(*coo_ja_new));
		ValueType * coo_a_new = (typeof(coo_a_new)) aligned_alloc(64, nnz * sizeof(*coo_a_new));

		INT_T * csr_row_ptr_new= (typeof(csr_row_ptr_new)) aligned_alloc(64, (m+1) * sizeof(*csr_row_ptr_new));
		INT_T * csr_ja_new = (typeof(csr_ja_new)) aligned_alloc(64, nnz * sizeof(*csr_ja_new));
		ValueType * csr_a_new = (typeof(csr_a_new)) aligned_alloc(64, nnz * sizeof(*csr_a_new));

		_Pragma("omp parallel")
		{
			long tnum = omp_get_thread_num();
			long task_num = topotk_get_task_num();
			struct thread_data * td_self = tds[tnum];
			long * packet_group_pos = td_self->packet_group_pos;
			unsigned char * compr_data_stealable = td_self->compr_data_stealable;
			unsigned char * compr_data_protected = td_self->compr_data_protected;
			long pos, pg, pg_e, i, i_s, i_e, j_s;
			long pos_decompress;
			long num_vals_total_dc = 0;
			long num_vals;
			long size;

			pg_e = td_self->num_packet_groups;
			i_s = td_self->i_s;
			i_e = td_self->i_e;
			j_s = row_ptr[i_s];

			pg = 0;
			pos_decompress = j_s;
			num_vals_total_dc = 0;
			pos = 0;
			while (pos < td_self->compr_data_size_protected)
			{
				size = decompress_kernel_div(&coo_ia_new[pos_decompress], &coo_ja_new[pos_decompress], &coo_a_new[pos_decompress], &num_vals, &compr_data_protected[pos], i_s, i_e);
				pos_decompress += num_vals;
				num_vals_total_dc += num_vals;
				pos += size;
			}
			pos = 0;
			while (pg < pg_e)
			{
				pos = packet_group_pos[pg];
				while (pos < packet_group_pos[pg+1])
				{
					size = decompress_kernel_div(&coo_ia_new[pos_decompress], &coo_ja_new[pos_decompress], &coo_a_new[pos_decompress], &num_vals, &compr_data_stealable[pos], i_s, i_e);
					pos_decompress += num_vals;
					num_vals_total_dc += num_vals;
					pos += size;
				}
				if (pos != packet_group_pos[pg+1])
					error("wrong decompression size: pg pos = %ld != pos decompress = %ld", packet_group_pos[pg+1], pos);
				pg++;
			}
			if (num_vals_total_dc != td_self->num_vals)
				error("task_num=%d: num_vals_dc = %ld != num_vals = %ld", task_num, num_vals_total_dc, td_self->num_vals);

			#pragma omp barrier

			#pragma omp for
			for (i=0;i<nnz;i++)
			{
				if ((coo_ia_new[i] < 0) || (coo_ia_new[i] >= m))
					error("coo_ia_new[%ld] = %d", i, coo_ia_new[i]);
				if ((coo_ja_new[i] < 0) || (coo_ja_new[i] >= n))
					error("coo_ja_new[%ld] = %d, n=%ld", i, coo_ja_new[i], n);
			}
		}

		coo_to_csr(coo_ia_new, coo_ja_new, coo_a_new, m, n, nnz, csr_row_ptr_new, csr_ja_new, csr_a_new, 1, 0);

		ValueType * a_conv;
		a_conv = (typeof(a_conv)) malloc(nnz * sizeof(*a_conv));
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
		int num_threads = omp_get_max_threads();
		long i;
		for (i=0;i<num_threads;i++)
		{
			free(tds[i]->compr_data_stealable);
			free(tds[i]);
		}
		free(tds);
		free(row_ptr);
		free(ja);
	}


	#if defined(DIV_TYPE_ADAPT)
		struct adapt_statistics_data {
			double predicted_total_size;
			double predicted_unique_values_fraction;
			double time_memory;
			double time_memory_per_byte;
			double time_decompress;
			double time_decompress_per_byte;
			double ratio_time_decompress_to_mem;
		};

		void
		adapt_statistics_concurrent_base(struct adapt_statistics_data * stats, unsigned char * packet_buf_stealable, unsigned char * packet_buf_protected)
		{
			int num_threads = omp_get_max_threads();
			long tnum = omp_get_thread_num();
			struct thread_data * td = tds[tnum];
			double unique_values_fraction_accum = 0;
			long num_vals, num_vals_max;
			long num_vals_accum = 0;
			long size_stealable, size_protected;
			long size_accum, size_stealable_accum, size_protected_accum;
			long i, i_s, i_e, j, j_s, j_e, k, l;
			double time, time_dc;
			long test_n;

			const long num_packet_vals = atol(getenv("CSRCV_NUM_PACKET_VALS"));

			i_s = td->i_s;
			i_e = td->i_e;
			j_s = row_ptr[i_s];
			j_e = row_ptr[i_e];

			i = i_s;
			j = j_s;
			num_vals_max = (j + num_packet_vals <= j_e) ? num_packet_vals : j_e - j;

			size_stealable = 0;
			size_protected = 0;
			num_vals = 0;
			if (num_vals_max > 0)
			{
				compress_kernel_div(row_ptr, ja, &a[j], symmetric, i, i_s, i_e, j, packet_buf_stealable, packet_buf_protected, num_vals_max, &num_vals, &size_stealable, &size_protected);
			}
			omp_thread_reduce_global(reduce_add_long, size_stealable, 0, 0, 0, , &size_stealable_accum);
			omp_thread_reduce_global(reduce_add_long, size_protected, 0, 0, 0, , &size_protected_accum);
			size_accum = size_stealable_accum + size_protected_accum;
			omp_thread_reduce_global(reduce_add_long, num_vals, 0, 0, 0, , &num_vals_accum);

			stats->predicted_total_size = ((double) size_accum) / num_vals_accum * nnz;

			omp_thread_reduce_global(reduce_add_double, td->packet_unique_values_fraction_accum, 0, 0, 0, , &unique_values_fraction_accum);
			stats->predicted_unique_values_fraction = unique_values_fraction_accum / num_threads;

			test_n = 100;
			// test_n = 10;

			// _Pragma("omp barrier")
			// volatile double val = 0;
			// double time_mem=HUGE_VAL;
			// for (l=0;l<test_n;l++)
			// {
				// double * buf;
				// long buf_n = size_stealable / sizeof(*buf);
				// buf = (typeof(buf)) malloc(buf_n * sizeof(*buf));
				// time = time_it(1,
					// for (k=0;k<buf_n;k++)
					// {
						// val = buf[k];   // Read packet data from memory.
						// val = buf[k];   // Read x, mostly in cache.
						// buf[k] = val;   // Write to y, mostly in cache.
					// }
				// );
				// free((void *) buf);
				// if (time < time_mem)
					// time_mem = time;
			// }
			// omp_thread_reduce_global(reduce_add_double, time_mem, 0, 0, 0, , &stats->time_memory);
			// _Pragma("omp barrier")

			_Pragma("omp single")
			{
				x_bench = (typeof(x_bench)) malloc(n * sizeof(*x_bench));
				y_bench = (typeof(y_bench)) malloc(m * sizeof(*y_bench));
			}
			_Pragma("omp for")
			for (k=0;k<n;k++)
				x_bench[k] = 1;
			_Pragma("omp for")
			for (k=0;k<m;k++)
				y_bench[k] = 0;
			time_dc=HUGE_VAL;
			for (l=0;l<test_n;l++)
			{
				time = time_it(1,
					long size;
					size = 0;
					while (size < size_stealable)
					{
						size += decompress_and_compute_kernel_div(packet_buf_stealable, x_bench, y_bench, i_s, i_e);
					}
					size = 0;
					while (size < size_protected)
					{
						size += decompress_and_compute_kernel_div(packet_buf_protected, x_bench, y_bench, i_s, i_e);
					}
				);
				if (time < time_dc)
					time_dc = time;
			}
			_Pragma("omp barrier")
			_Pragma("omp single")
			{
				free(x_bench);
				free(y_bench);
			}
			omp_thread_reduce_global(reduce_add_double, time_dc, 0, 0, 0, , &stats->time_decompress);
			stats->time_decompress_per_byte = stats->time_decompress / size_accum;

			_Pragma("omp barrier")
		}

		void
		adapt_statistics_concurrent(unsigned char * packet_buf_stealable, unsigned char * packet_buf_protected)
		{
			long tnum = omp_get_thread_num();
			struct thread_data * td = tds[tnum];
			unsigned char * compr_data_stealable = td->compr_data_stealable;
			struct adapt_statistics_data stats;
			// long i;
			long i_s, i_e, j_s, j_e;

			i_s = td->i_s;
			i_e = td->i_e;
			j_s = row_ptr[i_s];
			j_e = row_ptr[i_e];

			long t_nnz = j_e - j_s;

			init_thread_statistics(td);

			_Pragma("omp single")
			{
				DV_ENABLED = 1;
			}
			adapt_statistics_concurrent_base(&stats, packet_buf_stealable, packet_buf_protected);

			_Pragma("omp barrier")
			double predicted_private_size = stats.predicted_total_size * t_nnz / nnz;
			long fraction_test = 10;
			long size_test = predicted_private_size / fraction_test;
			long size_test_accum;
			omp_thread_reduce_global(reduce_add_long, size_test, 0, 0, 0, , &size_test_accum);
			_Pragma("omp barrier")
			double time_mem;
			// long n_test = predicted_private_size / sizeof(long) / fraction_test;
			long n_test = predicted_private_size / fraction_test;
			time_mem = time_it(1,
				memset(compr_data_stealable, 0, n_test);
				// for (i=0;i<n_test;i++)
				// {
					// ((long *) compr_data_stealable)[i] = 0;
				// }
			);
			omp_thread_reduce_global(reduce_add_double, time_mem, 0, 0, 0, , &stats.time_memory);
			stats.time_memory_per_byte = stats.time_memory / size_test_accum;
			_Pragma("omp barrier")

			stats.ratio_time_decompress_to_mem = stats.time_decompress_per_byte / stats.time_memory_per_byte;
			_Pragma("omp single")
			{
				printf("size_test_accum=%lf\n", size_test_accum / 1024.0 / 1024.0);
				printf("DV_ENABLED=%ld , RF_ENABLED=%ld, symmetric=%ld\n", DV_ENABLED, RF_ENABLED, symmetric);
				printf("time_mem_per_byte=%g , time_dc_per_byte=%g , time_dc_per_byte/time_mem_per_byte=%g\n", stats.time_memory_per_byte, stats.time_decompress_per_byte, stats.ratio_time_decompress_to_mem);
				printf("predicted_total_size = %lf , l3_size_total = %lf , predicted_unique_values_fraction=%lf\n", stats.predicted_total_size / 1024.0 / 1024.0, td->l3_size_total / 1024.0 / 1024.0, stats.predicted_unique_values_fraction);
			}
			init_thread_statistics(td);


			_Pragma("omp single")
			{
				DV_ENABLED = 0;
			}
			adapt_statistics_concurrent_base(&stats, packet_buf_stealable, packet_buf_protected);
			stats.ratio_time_decompress_to_mem = stats.time_decompress_per_byte / stats.time_memory_per_byte;
			_Pragma("omp single")
			{
				printf("size_test_accum=%lf\n", size_test_accum / 1024.0 / 1024.0);
				printf("DV_ENABLED=%ld , RF_ENABLED=%ld, symmetric=%ld\n", DV_ENABLED, RF_ENABLED, symmetric);
				printf("time_mem_per_byte=%g , time_dc_per_byte=%g , time_dc_per_byte/time_mem_per_byte=%g\n", stats.time_memory_per_byte, stats.time_decompress_per_byte, stats.ratio_time_decompress_to_mem);
				printf("predicted_total_size = %lf , l3_size_total = %lf , predicted_unique_values_fraction=%lf\n", stats.predicted_total_size / 1024.0 / 1024.0, td->l3_size_total / 1024.0 / 1024.0, stats.predicted_unique_values_fraction);
			}
			init_thread_statistics(td);

			if (symmetric)
			{
				_Pragma("omp barrier")
				_Pragma("omp single")
				{
					symmetric = 0;
				}

				_Pragma("omp single")
				{
					DV_ENABLED = 1;
				}
				adapt_statistics_concurrent_base(&stats, packet_buf_stealable, packet_buf_protected);
				stats.ratio_time_decompress_to_mem = stats.time_decompress_per_byte / stats.time_memory_per_byte;
				_Pragma("omp single")
				{
					printf("size_test_accum=%lf\n", size_test_accum / 1024.0 / 1024.0);
					printf("DV_ENABLED=%ld , RF_ENABLED=%ld, symmetric=%ld\n", DV_ENABLED, RF_ENABLED, symmetric);
					printf("time_mem_per_byte=%g , time_dc_per_byte=%g , time_dc_per_byte/time_mem_per_byte=%g\n", stats.time_memory_per_byte, stats.time_decompress_per_byte, stats.ratio_time_decompress_to_mem);
					printf("predicted_total_size = %lf , l3_size_total = %lf , predicted_unique_values_fraction=%lf\n", stats.predicted_total_size / 1024.0 / 1024.0, td->l3_size_total / 1024.0 / 1024.0, stats.predicted_unique_values_fraction);
				}
				init_thread_statistics(td);

				_Pragma("omp single")
				{
					DV_ENABLED = 0;
				}
				adapt_statistics_concurrent_base(&stats, packet_buf_stealable, packet_buf_protected);
				stats.ratio_time_decompress_to_mem = stats.time_decompress_per_byte / stats.time_memory_per_byte;
				_Pragma("omp single")
				{
					printf("size_test_accum=%lf\n", size_test_accum / 1024.0 / 1024.0);
					printf("DV_ENABLED=%ld , RF_ENABLED=%ld, symmetric=%ld\n", DV_ENABLED, RF_ENABLED, symmetric);
					printf("time_mem_per_byte=%g , time_dc_per_byte=%g , time_dc_per_byte/time_mem_per_byte=%g\n", stats.time_memory_per_byte, stats.time_decompress_per_byte, stats.ratio_time_decompress_to_mem);
					printf("predicted_total_size = %lf , l3_size_total = %lf , predicted_unique_values_fraction=%lf\n", stats.predicted_total_size / 1024.0 / 1024.0, td->l3_size_total / 1024.0 / 1024.0, stats.predicted_unique_values_fraction);
				}
				init_thread_statistics(td);

				_Pragma("omp single")
				{
					symmetric = 1;
				}
			}

			_Pragma("omp single")
			{
				// if (stats.predicted_total_size < 1.5 * l3_size_total)
					// DV_ENABLED = 0;
				// if (stats.predicted_unique_values_fraction < 0.1)
				// if (stats.predicted_total_size < 1.5 * l3_size_total)
					// symmetric = 0;
				printf("symmetric=%ld\n", symmetric);
				DV_ENABLED = 1;
				// DV_ENABLED = 0;
				// symmetric = 0;
			}
			init_thread_statistics(td);
		}
	#endif


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
		csr->format_name = (char *) "DIV_RF";
	#elif defined(DIV_TYPE_ADAPT)
		csr->format_name = (char *) "DIV_ADAPT";
	#elif defined(DIV_TYPE_RF_CONST_SIZE_ROW)
		csr->format_name = (char *) "DIV_RF_CONST_SIZE_ROW";
	#elif defined(DIV_TYPE_SELECT)
		csr->format_name = (char *) "DIV_SELECT";
	#elif defined(DIV_TYPE_ORD2)
		csr->format_name = (char *) "DIV_RF_ORD2";
	#elif defined(DIV_TYPE_COLS_SORT)
		csr->format_name = (char *) "DIV_COLS_SORT";
	#elif defined(DIV_TYPE_SYM_RF_LOCAL)
		csr->format_name = (char *) "DIV_RF_SYM";
	#else
		csr->format_name = (char *) "DIV";
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


static inline
long
steal_sched_l3_node_only(struct thread_data ** tds, [[gnu::unused]] int num_threads, long tnum,
		long * tnum_target_out, long * work_out)
{
	long task_l3_node_num = tds[tnum]->task_l3_node_num;
	struct thread_data * td_target;
	long finished, all_finished;
	long tnum_target;
	long work_max, work;
	long i;
	tnum_target = -1;
	work_max = 0;
	all_finished = 1;
	for (i=0;i<num_threads;i++)
	{
		if (i == tnum)
			continue;
		td_target = tds[i];
		finished = __atomic_load_n(&td_target->finished, __ATOMIC_SEQ_CST);
		if (finished == 1)
			continue;
		if (td_target->task_l3_node_num_owner != task_l3_node_num)
			continue;
		all_finished = 0;
		if (td_target->steal_lock)
			continue;
		work = td_target->pg_e - td_target->pg_current;
		if (work < 4)
			continue;
		if (work > work_max)
		{
			tnum_target = i;
			work_max = work;
		}
	}
	*tnum_target_out = tnum_target;
	*work_out = work_max;
	return all_finished;
}

static inline
long
steal_sched_mem_node_only(struct thread_data ** tds, [[gnu::unused]] int num_threads, long tnum,
		long * tnum_target_out, long * work_out)
{
	long task_mem_node_num = tds[tnum]->task_mem_node_num;
	struct thread_data * td_target;
	long finished, all_finished;
	long tnum_target;
	long work_max, work;
	long i;
	tnum_target = -1;
	work_max = 0;
	all_finished = 1;
	for (i=0;i<num_threads;i++)
	{
		if (i == tnum)
			continue;
		td_target = tds[i];
		finished = __atomic_load_n(&td_target->finished, __ATOMIC_SEQ_CST);
		if (finished == 1)
			continue;
		if (td_target->task_mem_node_num_owner != task_mem_node_num)
			continue;
		all_finished = 0;
		if (td_target->steal_lock)
			continue;
		work = td_target->pg_e - td_target->pg_current;
		if (work < 4)
			continue;
		if (work > work_max)
		{
			tnum_target = i;
			work_max = work;
		}
	}
	*tnum_target_out = tnum_target;
	*work_out = work_max;
	return all_finished;
}

static inline
long
steal_sched_default(struct thread_data ** tds, int num_threads, long tnum,
		long * tnum_target_out, long * work_out)
{
	struct thread_data * td_target;
	long finished, all_finished;
	long tnum_target;
	long work_max, work;
	long i;
	tnum_target = -1;
	work_max = 0;
	all_finished = 1;
	for (i=0;i<num_threads;i++)
	{
		if (i == tnum)
			continue;
		td_target = tds[i];
		finished = __atomic_load_n(&td_target->finished, __ATOMIC_SEQ_CST);
		// if (tnum == 0)
			// printf("%ld : %ld %ld\n", i, finished, all_finished);
		if (finished == 1)
			continue;
		all_finished = 0;
		if (td_target->steal_lock)
			continue;
		work = td_target->pg_e - td_target->pg_current;
		if (work < 4)
			continue;
		if (work > work_max)
		{
			tnum_target = i;
			work_max = work;
		}
	}
	*tnum_target_out = tnum_target;
	*work_out = work_max;
	return all_finished;
}

static inline
long
steal_work(DIVArray * restrict csr, int num_threads, long tnum, long * flag_local_work_finished_ptr)
{
	struct thread_data * td_self = tds[tnum];
	struct thread_data * td_target;
	long tnum_owner;
	long task_l3_node_num_owner;
	long task_mem_node_num_owner;
	long num_finished;
	long all_finished;
	long tnum_target;
	long work_target, work, pg, pg_e;

	while (1)
	{
		if (__atomic_exchange_n(&td_self->steal_lock, 1, __ATOMIC_SEQ_CST) == 0)
			break;

		num_finished = __atomic_load_n(&csr->finished_threads.val, __ATOMIC_SEQ_CST);
		if (num_finished >= num_threads)
			return 1;

		lock_cpu_relax();
	}

	while (1)
	{
		tnum_target = -1;
		work_target = 0;
		all_finished = 0;

		if (!*flag_local_work_finished_ptr)
		{
			// long local_finished = steal_sched_mem_node_only(tds, num_threads, tnum, &tnum_target, &work_target);
			long local_finished = steal_sched_l3_node_only(tds, num_threads, tnum, &tnum_target, &work_target);
			if (local_finished)
			{
				// *flag_local_work_finished_ptr = 1;
				// continue;
				return 1;
			}
		}
		else
		{
			all_finished = steal_sched_default(tds, num_threads, tnum, &tnum_target, &work_target);
		}

		if (all_finished)
			return 1;

		if (tnum_target < 0)
			continue;
		work = 7 * work_target / 16;
		// work = work_target / 2;
		// work = 0;
		if (work <= 0)
			continue;

		td_target = tds[tnum_target];
		if (__atomic_exchange_n(&td_target->steal_lock, 1, __ATOMIC_SEQ_CST))
			continue;

		tnum_owner = td_target->tnum_owner;
		task_l3_node_num_owner = td_target->task_l3_node_num_owner;
		task_mem_node_num_owner = td_target->task_mem_node_num_owner;
		pg = __atomic_fetch_add(&td_target->pg_current, work, __ATOMIC_SEQ_CST);
		if (pg >= td_target->pg_e)
			goto steal_work_retry;
		if (work > td_target->pg_e - pg)
			work = td_target->pg_e - pg;
		pg_e = pg + work;
		// printf("tnum=%ld, task_l3_node_num=%ld, task_mem_node_num=%ld, tnum_target=%ld, tnum_owner=%ld, task_l3_node_num_owner=%ld, task_mem_node_num_owner=%ld, pg=%ld, pg_e=%ld, work_target=%ld, work=%ld\n", tnum, td_self->task_l3_node_num, td_self->task_mem_node_num, tnum_target, tnum_owner, task_l3_node_num_owner, task_mem_node_num_owner, pg, pg_e, work_target, work);
		__atomic_store_n(&td_self->tnum_owner, tnum_owner, __ATOMIC_SEQ_CST);
		__atomic_store_n(&td_self->task_l3_node_num_owner, task_l3_node_num_owner, __ATOMIC_SEQ_CST);
		__atomic_store_n(&td_self->task_mem_node_num_owner, task_mem_node_num_owner, __ATOMIC_SEQ_CST);
		__atomic_store_n(&td_self->pg_current, pg, __ATOMIC_SEQ_CST);
		__atomic_store_n(&td_self->pg_e, pg_e, __ATOMIC_SEQ_CST);

		__atomic_store_n(&td_target->steal_lock, 0, __ATOMIC_SEQ_CST);
		break;

steal_work_retry:
		__atomic_store_n(&td_target->steal_lock, 0, __ATOMIC_SEQ_CST);
	}

	__atomic_store_n(&td_self->steal_lock, 0, __ATOMIC_SEQ_CST);
	return 0;
}

void
compute_div(DIVArray * restrict csr, ValueType * restrict x, ValueType * restrict y)
{
	long num_threads = omp_get_max_threads();
	csr->finished_threads.val = 0;
	#pragma omp parallel
	{
		long tnum = omp_get_thread_num();
		struct thread_data * td_self = tds[tnum];
		long task_l3_node_num = td_self->task_l3_node_num;
		long task_mem_node_num = td_self->task_mem_node_num;
		struct thread_data * td_owner;
		long * packet_group_pos = td_self->packet_group_pos;
		unsigned char * compr_data_stealable = td_self->compr_data_stealable;
		unsigned char * compr_data_protected = td_self->compr_data_protected;
		long pos, pg, pg_e, i, i_s, i_e;
		long flag_local_work_finished;
		long all_finished;

		long stealing_enabled;
		#if defined(DIV_TYPE_SYM_RF_LOCAL)
			// stealing_enabled = 0;
			stealing_enabled = atoi(getenv("DIV_STEALING_ENABLED"));
		#elif defined(DIV_TYPE_ADAPT)
			if (csr->symmetric)
				stealing_enabled = 0;
			else
				stealing_enabled = atoi(getenv("DIV_STEALING_ENABLED"));
		#else
			stealing_enabled = atoi(getenv("DIV_STEALING_ENABLED"));
		#endif

		flag_local_work_finished = 0;
		__atomic_store_n(&td_self->finished, 0, __ATOMIC_RELAXED);
		__atomic_store_n(&td_self->steal_lock, 1, __ATOMIC_RELAXED);
		__atomic_store_n(&td_self->tnum_owner, tnum, __ATOMIC_RELAXED);
		__atomic_store_n(&td_self->task_l3_node_num_owner, task_l3_node_num, __ATOMIC_RELAXED);
		__atomic_store_n(&td_self->task_mem_node_num_owner, task_mem_node_num, __ATOMIC_RELAXED);
		__atomic_store_n(&td_self->pg_current, 0, __ATOMIC_RELAXED);
		pg_e = td_self->num_packet_groups;
		__atomic_store_n(&td_self->pg_e, pg_e, __ATOMIC_RELAXED);

		i_s = td_self->i_s;
		i_e = td_self->i_e;
		i = i_s;
		pos = 0;

		for (i=i_s;i<i_e;i++)
		{
			y[i] = 0;
		}

		// if (td_self->compr_data_size_protected > 0)
		// {
			// for (i=i_s;i<i_e;i++)
				// csr->y_prot[i] = 0;
		// }

		_Pragma("omp barrier")

		td_self->time_compute += time_it(1,
			pos = 0;
			while (pos < td_self->compr_data_size_protected)
			{
				// pos += decompress_and_compute_kernel_div(&(compr_data_protected[pos]), x, csr->y_prot, i_s, i_e);
				pos += decompress_and_compute_kernel_div(&(compr_data_protected[pos]), x, y, i_s, i_e);
			}
			__atomic_store_n(&td_self->steal_lock, 0, __ATOMIC_RELAXED);
			pos = 0;
			while (1)
			{
				while (1)
				{
					pg = __atomic_fetch_add(&td_self->pg_current, 1, __ATOMIC_SEQ_CST);
					if (__builtin_expect(pg >= pg_e, 0))
						break;
					pos = packet_group_pos[pg];
					while (pos != packet_group_pos[pg+1])
					{
						pos += decompress_and_compute_kernel_div(&(compr_data_stealable[pos]), x, y, i_s, i_e);
					}
				}

				__atomic_store_n(&td_self->finished, 1, __ATOMIC_SEQ_CST);
				__atomic_fetch_add(&csr->finished_threads.val, 1, __ATOMIC_SEQ_CST);

				if (!stealing_enabled)
					break;

				all_finished = steal_work(csr, num_threads, tnum, &flag_local_work_finished);
				if (all_finished == 1)
					break;
				td_owner = tds[td_self->tnum_owner];
				pg_e = td_self->pg_e;
				compr_data_stealable = td_owner->compr_data_stealable;
				packet_group_pos = td_owner->packet_group_pos;

				__atomic_fetch_sub(&csr->finished_threads.val, 1, __ATOMIC_SEQ_CST);
				__atomic_store_n(&td_self->finished, 0, __ATOMIC_SEQ_CST);

				i_s = td_owner->i_s;
				i_e = td_owner->i_e;
			}

			// _Pragma("omp barrier")
			// i_s = td_self->i_s;
			// i_e = td_self->i_e;
			// if (td_self->compr_data_size_protected > 0)
			// {
				// for (i=i_s;i<i_e;i++)
					// y[i] += csr->y_prot[i];
			// }

		);
	}
}


//==========================================================================================================================================
//= Print Statistics
//==========================================================================================================================================


void
DIVArray::statistics_start()
{
	int num_threads = omp_get_max_threads();
	long i;
	for (i=0;i<num_threads;i++)
	{
		tds[i]->time_compute = 0;
	}
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
	len += snprintf(buf + len, buf_n - len, ",%s", "unbalance_time");
	len += snprintf(buf + len, buf_n - len, ",%s", "unbalance_size");
	len += snprintf(buf + len, buf_n - len, ",%s", "row_bits_avg");
	len += snprintf(buf + len, buf_n - len, ",%s", "col_bits_avg");
	len += snprintf(buf + len, buf_n - len, ",%s", "row_col_bytes_avg");
	len += snprintf(buf + len, buf_n - len, ",%s", "packet_unique_values_fraction_avg");
	len += snprintf(buf + len, buf_n - len, ",%s", "compression_time");
	len += snprintf(buf + len, buf_n - len, ",%s", "compression_spmv_loops");
	len += snprintf(buf + len, buf_n - len, ",%s", "tolerance");
	#if defined(DIV_TYPE_ADAPT)
		/* len += snprintf(buf + len, buf_n - len, ",%s", "predicted_mem_footprint"); */
		len += snprintf(buf + len, buf_n - len, ",%s", "DV_ENABLED");
		len += snprintf(buf + len, buf_n - len, ",%s", "RF_ENABLED");
	#endif
	return len;
}


int
DIVArray::statistics_print_data(char * buf, long buf_n)
{
	int num_threads = omp_get_max_threads();

	long num_packets_stealable = 0;

	double time_max = 0, time_avg = 0;

	double data_size_max = 0, data_size_avg = 0;

	double row_bits_avg = 0;
	double col_bits_avg = 0;
	double row_col_bytes_avg = 0;
	double packet_unique_values_fraction_avg = 0;

	long i, len;

	for (i=0;i<num_threads;i++)
	{
		num_packets_stealable += tds[i]->num_packets_stealable;

		time_avg += tds[i]->time_compute;
		if (tds[i]->time_compute > time_max)
			time_max = tds[i]->time_compute;

		data_size_avg += tds[i]->compr_data_size_stealable;
		if (tds[i]->compr_data_size_stealable > data_size_max)
			data_size_max = tds[i]->compr_data_size_stealable;

		row_bits_avg += tds[i]->row_bits_accum;
		col_bits_avg += tds[i]->col_bits_accum;
		row_col_bytes_avg += tds[i]->row_col_bytes_accum;
		packet_unique_values_fraction_avg += tds[i]->packet_unique_values_fraction_accum;
	}
	time_avg /= num_threads;
	data_size_avg /= num_threads;
	row_bits_avg /= nnz;
	col_bits_avg /= nnz;
	row_col_bytes_avg /= nnz;
	packet_unique_values_fraction_avg /= num_packets_stealable;

	double compression_spmv_loops = time_compress / (time_max / num_loops_out);

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
	len += snprintf(buf + len, buf_n - len, ",%.2lf", (time_max - time_avg) / time_avg * 100); // unbalance time
	len += snprintf(buf + len, buf_n - len, ",%.2lf",  (data_size_max - data_size_avg) / data_size_avg * 100); // unbalance size
	len += snprintf(buf + len, buf_n - len, ",%.4lf",  row_bits_avg);
	len += snprintf(buf + len, buf_n - len, ",%.4lf",  col_bits_avg);
	len += snprintf(buf + len, buf_n - len, ",%.4lf",  row_col_bytes_avg);
	len += snprintf(buf + len, buf_n - len, ",%.4lf",  packet_unique_values_fraction_avg);
	len += snprintf(buf + len, buf_n - len, ",%.4lf",  time_compress);
	len += snprintf(buf + len, buf_n - len, ",%.4lf",  compression_spmv_loops);
	len += snprintf(buf + len, buf_n - len, ",%g",  tolerance);
	#if defined(DIV_TYPE_ADAPT)
		// len += snprintf(buf + len, buf_n - len, ",%.4lf", tds[0]->predicted_total_size / 1024 / 1024);   // predicted total size
		len += snprintf(buf + len, buf_n - len, ",%ld", DV_ENABLED);
		len += snprintf(buf + len, buf_n - len, ",%ld", RF_ENABLED);
	#endif
	return len;
}

