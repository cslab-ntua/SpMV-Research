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

	#include "aux/csr_converter.h"

#ifdef __cplusplus
}
#endif

// #include "spmv_kernel_div_kernels_rf_ord2_buf.h"
// #include "spmv_kernel_div_kernels_rf_ord2.h"
// #include "spmv_kernel_div_kernels_ord2.h"
// #include "spmv_kernel_div_kernels_store_sign_rf.h"
// #include "spmv_kernel_div_kernels_store_sign.h"
// #include "spmv_kernel_div_kernels_rf.h"
// #include "spmv_kernel_div_kernels.h"

// #include "spmv_kernel_div_kernels_predict.h"
// #include "spmv_kernel_div_kernels_predict_shifts.h"
// #include "spmv_kernel_div_kernels_predict_0_base_naive_prev_value.h"
// #include "spmv_kernel_div_kernels_predict_1_ratio.h"

// #include "spmv_kernel_div_kernels_0.h"
// #include "spmv_kernel_div_kernels_0_size_of_fraction.h"
// #include "spmv_kernel_div_kernels_1_total_entry_size.h"
// #include "spmv_kernel_div_kernels_2_y_buf.h"
// #include "spmv_kernel_div_kernels_3_coord_bytes.h"
// #include "spmv_kernel_div_kernels_4_abs.h"
// #include "spmv_kernel_div_kernels_5_all_bytes.h"
// #include "spmv_kernel_div_kernels_6_vector.h"
// #include "spmv_kernel_div_kernels_7_vector_lanes.h"
// #include "spmv_kernel_div_kernels_8_vector_lanes_y_buf.h"
// #include "spmv_kernel_div_kernels_9_vps.h"
// #include "spmv_kernel_div_kernels_10_select.h"
// #include "spmv_kernel_div_kernels_11_int_sort_abs.h"
// #include "spmv_kernel_div_kernels_12_int_sort_abs_sgn.h"
// #include "spmv_kernel_div_kernels_13_int_sort_abs_znp.h"
// #include "spmv_kernel_div_kernels_14_int_sort_abs_znp_avx_coords.h"
// #include "spmv_kernel_div_kernels_15_int_sort_abs_znp_avx_coords_multadd_select.h"
// #include "spmv_kernel_div_kernels_16_int_sort_abs_znp_multadd_select.h"
// #include "spmv_kernel_div_kernels_18_int_sort_abs_nzp_r0_select_dense_select_2_shifts.h"
// #include "spmv_kernel_div_kernels_23_int_sort_abs_nzp_r0_select_dense_select.h"

// #include "spmv_kernel_div_kernels_24_no_initial_values.h"
// #include "spmv_kernel_div_kernels_25_rf.h"
// #include "spmv_kernel_div_kernels_26_rf_vector.h"

#if defined(DIV_TYPE_RF)
	#include "spmv_kernel_div_kernels_rf.h"
#elif defined(DIV_TYPE_SELECT)
	#include "spmv_kernel_div_kernels_select.h"
#elif defined(DIV_TYPE_ORD2)
	#include "spmv_kernel_div_kernels_rf_ord2_buf.h"
	// #include "spmv_kernel_div_kernels_rf_ord2.h"
#elif defined(DIV_TYPE_COLS_SORT)
	#include "spmv_kernel_div_kernels_cols_sort.h"
#elif defined(DIV_TYPE_SYM_RF_LOCAL)
	// #include "spmv_kernel_div_sym_kernels_rf_local.h"
	// #include "spmv_kernel_div_sym_kernels_rf_local_split.h"
	// #include "spmv_kernel_div_sym_kernels_rf_local_split_coords.h"
	// #include "spmv_kernel_div_sym_kernels_rf_local_split_coords_sort_cols.h"
	#include "spmv_kernel_div_sym_kernels_rf_local_split_new_packet.h"
	// #include "spmv_kernel_div_sym_kernels_rf_local_split_new_packet_accum.h"
#else
	#include "spmv_kernel_div_kernels.h"
#endif


// Account for a fraction of the cache to leave space for indices, etc.
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

extern int prefetch_distance;

// Statistics

double * thread_time_compute;

extern uint64_t * t_row_bits_accum;
extern uint64_t * t_col_bits_accum;
extern uint64_t * t_row_col_bytes_accum;

double time_compress;
extern long num_loops_out;


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//------------------------------------------------------------------------------------------------------------------------------------------
//-                                                                SpMV                                                                    -
//------------------------------------------------------------------------------------------------------------------------------------------
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


struct CSRCVSArrays : Matrix_Format
{
	INT_T * row_ptr;                             // the usual rowptr (of size m+1)
	INT_T * ja;                                  // the colidx of each NNZ (of size nnz)
	ValueType * a;

	INT_T * t_i_s;
	INT_T * t_i_e;

	long * t_num_packets;                        // number of compressed data packets
	long * t_compr_data_size;                    // size of the compressed data
	unsigned char ** t_compr_data;               // the compressed values
	long * t_num_vals;

	long * t_finished;
	long * t_num_packet_groups;
	int ** t_packet_group_pos;
	long * t_num_packet_groups_left;
	INT_T * t_pg_e;

	double error_matrix;

	double matrix_mae, matrix_max_ae, matrix_mse, matrix_mape, matrix_smape;
	double matrix_lnQ_error, matrix_mlare, matrix_gmare;

	void calculate_matrix_compression_error(INT_T * csr_row_ptr_new, INT_T * csr_ja_new, ValueType * csr_a_new);

	CSRCVSArrays(INT_T * row_ptr, INT_T * ja, ValueType * a, long m, long n, long nnz) : Matrix_Format(m, n, nnz), row_ptr(row_ptr), ja(ja), a(a)
	{
		int num_threads = omp_get_max_threads();
		double time_balance;
		long compr_data_size, i;

		INT_T * coo_ia_new;
		INT_T * coo_ja_new;
		ValueType * coo_a_new;

		INT_T * csr_row_ptr_new;
		INT_T * csr_ja_new;
		ValueType * csr_a_new;

		t_i_s = (typeof(t_i_s)) malloc(num_threads * sizeof(*t_i_s));
		t_i_e = (typeof(t_i_e)) malloc(num_threads * sizeof(*t_i_e));

		thread_time_compute = (typeof(thread_time_compute)) malloc(num_threads * sizeof(*thread_time_compute));

		coo_ia_new = (typeof(coo_ia_new)) aligned_alloc(64, nnz * sizeof(*coo_ia_new));
		coo_ja_new = (typeof(coo_ja_new)) aligned_alloc(64, nnz * sizeof(*coo_ja_new));
		coo_a_new = (typeof(coo_a_new)) aligned_alloc(64, nnz * sizeof(*coo_a_new));

		csr_row_ptr_new= (typeof(csr_row_ptr_new)) aligned_alloc(64, (m+1) * sizeof(*csr_row_ptr_new));
		csr_ja_new = (typeof(csr_ja_new)) aligned_alloc(64, nnz * sizeof(*csr_ja_new));
		csr_a_new = (typeof(csr_a_new)) aligned_alloc(64, nnz * sizeof(*csr_a_new));

		_Pragma("omp parallel for")
		for (long i=0;i<nnz;i++)
		{
			coo_a_new[i] = 0.0;
			coo_ia_new[i] = 0;
			coo_ja_new[i] = 0;
			csr_a_new[i] = 0.0;
			csr_ja_new[i] = 0;
		}
		_Pragma("omp parallel for")
		for (long i=0;i<m+1;i++)
			csr_row_ptr_new[i] = 0;

		time_balance = time_it(1,
			_Pragma("omp parallel")
			{
				int tnum = omp_get_thread_num();
				loop_partitioner_balance_prefix_sums(num_threads, tnum, row_ptr, m, nnz, &t_i_s[tnum], &t_i_e[tnum]);
			}
		);
		printf("balance time = %g\n", time_balance);
		t_num_packets = (typeof(t_num_packets)) aligned_alloc(64, num_threads * sizeof(*t_num_packets));
		t_compr_data_size = (typeof(t_compr_data_size)) aligned_alloc(64, num_threads * sizeof(*t_compr_data_size));
		t_compr_data = (typeof(t_compr_data)) aligned_alloc(64, num_threads * sizeof(*t_compr_data));
		t_num_vals = (typeof(t_num_vals)) aligned_alloc(64, num_threads * sizeof(*t_num_vals));
		t_finished = (typeof(t_finished)) aligned_alloc(64, num_threads * sizeof(*t_finished));
		t_num_packet_groups = (typeof(t_num_packet_groups)) aligned_alloc(64, num_threads * sizeof(*t_num_packet_groups));
		t_packet_group_pos = (typeof(t_packet_group_pos)) aligned_alloc(64, num_threads * sizeof(*t_packet_group_pos));
		t_num_packet_groups_left = (typeof(t_num_packet_groups_left)) aligned_alloc(64, num_threads * sizeof(*t_num_packet_groups_left));
		t_pg_e = (typeof(t_pg_e)) aligned_alloc(64, num_threads * sizeof(*t_pg_e));
		time_compress = time_it(1,
			long num_packet_vals;

			num_packet_vals = get_num_uncompressed_packet_vals();
			compress_init_sort_diff(a, nnz, num_packet_vals);

			_Pragma("omp parallel")
			{
				int tnum = omp_get_thread_num();
				unsigned char * data;
				long t_nnz, max_data_size;
				__attribute__((unused)) long p, i, i_s, i_e, j, j_s, j_e, k;
				long num_vals, num_vals_new, num_vals_dc, num_vals_total=0, num_vals_total_dc=0;
				long num_packets;
				long num_packet_groups;
				long pos, size;
				long upper_boundary;
				long pos_decompress = 0;
				int * packet_group_pos;

				i_s = t_i_s[tnum];
				i_e = t_i_e[tnum];
				j_s = row_ptr[i_s];
				j_e = row_ptr[i_e];

				_Pragma("omp single nowait")
				{
					printf("Number of packet vals = %ld\n", num_packet_vals);
				}

				t_nnz = j_e - j_s;
				num_packets = (t_nnz + num_packet_vals - 1) / num_packet_vals;
				// long leftover = t_nnz % num_packet_vals;

				packet_group_pos = (typeof(packet_group_pos)) malloc(t_nnz * sizeof(*packet_group_pos));

				max_data_size = t_nnz * (sizeof(*row_ptr) + sizeof(*ja) + 2 * sizeof(ValueType));    // We assume worst case scenario:  compr_data = 2 * data
				data = (typeof(data)) aligned_alloc(64, max_data_size);

				pos = 0;
				i = i_s;
				p = 0;
				num_packet_groups = 0;
				j = j_s;
				pos_decompress = j_s;
				memset(packet_group_pos, 0, t_nnz * sizeof(*packet_group_pos));
				while (1)
				{
					if (j >= j_e)
						break;

					// Correctness testing for macros_binary_search().
					long tmp = i;
					while (row_ptr[tmp] < j)
						tmp++;
					if (j != row_ptr[tmp])
						tmp--;

					/* Index boundaries are inclusive. 'upper_boundary' is certainly the first row after the rows belonging to the packet (last packet row can be partial).
					 *
					 * This removes leading empty rows.
					 */
					macros_binary_search(row_ptr, i_s, i_e, j, NULL, &upper_boundary);
					i = upper_boundary;
					if (j != row_ptr[i])
						i--;

					if (i != tmp)
						error("wrong i = %ld, correct i = %ld", i, tmp);

					// If it starts at the beginning of a row then start a new packet group.
					if (j == row_ptr[i])
						num_packet_groups++;

					num_vals = (j + num_packet_vals <= j_e) ? num_packet_vals : j_e - j;
					size = compress_kernel_sort_diff(row_ptr, ja, &a[j], i, i_s, i_e, j, &data[pos], num_vals, &num_vals_new);
					num_vals = num_vals_new;
					num_vals_total += num_vals;

					packet_group_pos[num_packet_groups-1] += size;

					// if (j < 298396 && j + num_vals > 298396)
					// {
						// long k = i;
						// while (row_ptr[k] < 298396)
						// {
							// printf("test i=%ld row_ptr[i]=%d row_ptr[i+1]=%d num_vals=%ld\n", k, row_ptr[k], row_ptr[k+1], row_ptr[k+1] - row_ptr[k]);
							// k++;
						// }
					// }

					// for (;i<i_e;i++)
					// {
						// if (row_ptr[i] >= j+num_vals)
							// break;
					// }

					// if (tnum == 0)
						// printf("%d: i=[%ld,%ld] , j=%ld[%ld,%ld] , p=%ld , num_vals=%ld , packet_i=[%d, %d] (%d) , t_nnz=%ld\n",
								// tnum, i_s, i_e, j, j_s, j_e, p, num_vals, packet_i_s[p], packet_i_e[p], packet_i_e[p] - packet_i_s[p], t_nnz);

					long s = decompress_kernel_sort_diff(&coo_ia_new[pos_decompress], &coo_ja_new[pos_decompress], &coo_a_new[pos_decompress], &num_vals_dc, &data[pos], i_s, i_e);
					if (s != size)
						error("size compress = %ld != size decompress = %ld", size, s);
					num_vals_total_dc += num_vals_dc;
					pos += size;
					if (pos > max_data_size)
						error("data buffer overflow");

					j += num_vals;
					pos_decompress += num_vals_dc;
					p++;

					// if (tnum == 1 && p > 100)
					// {
						// ValueType v1, v2;
						// for (long z=0;z<num_vals;z++)
						// {
							// v1 = a[j+z];
							// v2 = coo_a_new[j+z];
							// if (v1 != v2)
								// printf("\t");
							// printf("a[%ld]=%lf , coo_a_new[%ld]=%lf\n", z, v1, z, v2);
						// }
						// exit(0);
					// }

				}
				if (num_vals_total_dc != num_vals_total)
					error("tnum=%d: num_vals_total_dc = %ld != num_vals_total = %ld", tnum, num_vals_total_dc, num_vals_total);
				num_packets = p;
				t_compr_data_size[tnum] = pos;
				t_compr_data[tnum] = data;
				t_num_packets[tnum] = num_packets;
				t_num_vals[tnum] = num_vals_total;

				t_num_packet_groups[tnum] = num_packet_groups;
				long tmp, sum;
				sum = 0;
				for (k=0;k<num_packet_groups;k++)
				{
					tmp = packet_group_pos[k];
					packet_group_pos[k] = sum;
					sum += tmp;
				}
				packet_group_pos[num_packet_groups] = sum;

				t_packet_group_pos[tnum] = (typeof(&(**t_packet_group_pos))) malloc((num_packet_groups+1) * sizeof(**t_packet_group_pos));
				for (k=0;k<num_packet_groups+1;k++)
					t_packet_group_pos[tnum][k] = packet_group_pos[k];

				free(packet_group_pos);
			}
		);
		printf("compression time = %g\n", time_compress);

		// error("test");

		long nnz_new= 0;
		compr_data_size = 0;
		for (i=0;i<num_threads;i++)
		{
			compr_data_size += t_compr_data_size[i];
			nnz_new += t_num_vals[i];
		}
		mem_footprint = compr_data_size;
		if (nnz_new != nnz)
			error("nnz_new = %ld != nnz = %ld", nnz_new, nnz);

		#pragma omp parallel
		{
			long i;
			#pragma omp for
			for (i=0;i<nnz;i++)
			{
				if ((coo_ia_new[i] < 0) || (coo_ia_new[i] >= m))
					error("coo_ia_new[%ld] = %d", i, coo_ia_new[i]);
				if ((coo_ja_new[i] < 0) || (coo_ja_new[i] >= n))
					error("coo_ja_new[%ld] = %d", i, coo_ja_new[i]);
			}
		}

		coo_to_csr(coo_ia_new, coo_ja_new, coo_a_new, m, n, nnz, csr_row_ptr_new, csr_ja_new, csr_a_new, 1, 0);

		calculate_matrix_compression_error(csr_row_ptr_new, csr_ja_new, csr_a_new);
		free(coo_ia_new);
		free(coo_ja_new);
		free(coo_a_new);
		free(csr_row_ptr_new);
		free(csr_ja_new);
		free(csr_a_new);
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
		free(row_ptr);
		free(ja);
		free(t_i_s);
		free(t_i_e);
	}

	void spmv(ValueType * x, ValueType * y);
	void statistics_start();
	int statistics_print_data(__attribute__((unused)) char * buf, __attribute__((unused)) long buf_n);
};


struct Matrix_Format *
csr_to_format(INT_T * row_ptr, INT_T * col_ind, ValueType * values, long m, long n, long nnz, int symmetric)
{
	if (symmetric)
		error("symmetric matrices not supported by this format, expand symmetry");
	struct CSRCVSArrays * csr = new CSRCVSArrays(row_ptr, col_ind, values, m, n, nnz);
	#if defined(DIV_TYPE_RF)
		csr->format_name = (char *) "CSR_CV_STREAM_RF";
	#elif defined(DIV_TYPE_SELECT)
		csr->format_name = (char *) "CSR_CV_STREAM_SELECT";
	#elif defined(DIV_TYPE_ORD2)
		csr->format_name = (char *) "CSR_CV_STREAM_RF_ORD2";
	#elif defined(DIV_TYPE_COLS_SORT)
		csr->format_name = (char *) "CSR_CV_STREAM_COLS_SORT";
	#elif defined(DIV_TYPE_SYM_RF_LOCAL)
		csr->format_name = (char *) "CSR_SYM_CV_STREAM_RF_LOCAL";
	#else
		csr->format_name = (char *) "CSR_CV_STREAM";
	#endif
	return csr;
}


//==========================================================================================================================================
//= Method Validation - Errors
//==========================================================================================================================================


void
CSRCVSArrays::calculate_matrix_compression_error(INT_T * csr_row_ptr_new, INT_T * csr_ja_new, ValueType * csr_a_new)
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
				if (csr_a_new[j] != a[j])
					printf("csr_a_new[%ld]=%g, a[%ld]=%g\n", j, csr_a_new[j], j, a[j]);
			}
		}


		double mae, max_ae, mse, mape, smape;
		double lnQ_error, mlare, gmare;
		array_mae_concurrent(a, csr_a_new, nnz, &mae, val_to_double);
		array_max_ae_concurrent(a, csr_a_new, nnz, &max_ae, val_to_double);
		array_mse_concurrent(a, csr_a_new, nnz, &mse, val_to_double);
		array_mape_concurrent(a, csr_a_new, nnz, &mape, val_to_double);
		array_smape_concurrent(a, csr_a_new, nnz, &smape, val_to_double);
		array_lnQ_error_concurrent(a, csr_a_new, nnz, &lnQ_error, val_to_double);
		array_mlare_concurrent(a, csr_a_new, nnz, &mlare, val_to_double);
		array_gmare_concurrent(a, csr_a_new, nnz, &gmare, val_to_double);
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
		// if (a[z] != csr_a_new[z])
		// {
			// printf("a[%ld]=%lf , csr_a_new[%ld]=%lf\n", z, a[z], z, csr_a_new[z]);
			// printf("a[%ld]=%064lb , csr_a_new[%ld]=%064lb\n", z, ((uint64_t *) a)[z], z, ((uint64_t *) csr_a_new)[z]);
		// }
	// }
}


//==========================================================================================================================================
//= SpMV Kernel
//==========================================================================================================================================


void compute_div(CSRCVSArrays * restrict csr, ValueType * restrict x , ValueType * restrict y);


void
CSRCVSArrays::spmv(ValueType * x, ValueType * y)
{
	compute_div(this, x, y);
}


long
steal_work(CSRCVSArrays * restrict csr, int num_threads, int tnum,
		int * tnum_target_out, long * p_out, long * num_packet_groups_out)
{
	long finished;
	long all_finished;
	int tnum_target;
	long num_packet_groups_left;
	long work_max, work, work_prev;
	long i;
	tnum_target = -1;
	work_max = 0;
	while (1)
	{
		all_finished = 1;
		for (i=0;i<num_threads;i++)
		{
			if (i == tnum)
				continue;
			finished = __atomic_load_n(&csr->t_finished[i], __ATOMIC_SEQ_CST);
			// if (tnum == 0)
				// printf("%ld : %ld %ld\n", i, finished, all_finished);
			if (finished == 1)
				continue;
			all_finished = 0;
			num_packet_groups_left = __atomic_load_n(&csr->t_num_packet_groups_left[i], __ATOMIC_SEQ_CST);
			if (num_packet_groups_left > work_max)
			{
				finished = __atomic_load_n(&csr->t_finished[i], __ATOMIC_SEQ_CST);
				if (finished == 1)
					continue;
				tnum_target = i;
				work_max = num_packet_groups_left;
			}
		}
		if (all_finished)
			return all_finished;
		if (tnum_target >= 0)
		{
			work = work_max / 2;
			if (work <= 0)
				continue;
			work_prev = __atomic_fetch_sub(&csr->t_num_packet_groups_left[tnum_target], work, __ATOMIC_SEQ_CST);
			if (work_prev < work)
				work = work_prev;
			printf("tnum=%d , tnum_target=%d , work_prev=%ld , work=%ld\n", tnum, tnum_target, work_prev, work);
			*tnum_target_out = tnum_target;
			*p_out = __atomic_sub_fetch(&csr->t_pg_e[tnum_target], work, __ATOMIC_SEQ_CST);
			*num_packet_groups_out = work;
			break;
		}
	}
	return all_finished;
}

void
compute_div(CSRCVSArrays * restrict csr, ValueType * restrict x, ValueType * restrict y)
{
	int num_threads = omp_get_max_threads();
	#pragma omp parallel
	{
		int tnum = omp_get_thread_num();
		int tnum_target;
		long num_packet_groups = csr->t_num_packet_groups[tnum];
		long * num_packet_groups_left_ptr = &csr->t_num_packet_groups_left[tnum];
		int * packet_group_pos = csr->t_packet_group_pos[tnum];
		long * finished_ptr = &csr->t_finished[tnum];
		long num_packet_groups_left;
		unsigned char * data = csr->t_compr_data[tnum];
		long pos, p, i, i_s, i_e;
		long all_finished;

		__atomic_store_n(finished_ptr, 0, __ATOMIC_RELAXED);
		__atomic_store_n(num_packet_groups_left_ptr, num_packet_groups, __ATOMIC_RELAXED);
		i_s = csr->t_i_s[tnum];
		i_e = csr->t_i_e[tnum];
		__atomic_store_n(&csr->t_pg_e[tnum], num_packet_groups, __ATOMIC_RELAXED);
		i = i_s;
		pos = 0;

		#pragma omp barrier

		thread_time_compute[tnum] += time_it(1,
			for (i=i_s;i<i_e;i++)
				y[i] = 0;
			p = 0;
			while (1)
			{
				num_packet_groups_left = __atomic_fetch_sub(num_packet_groups_left_ptr, 1, __ATOMIC_SEQ_CST);
				if (__builtin_expect(num_packet_groups_left <= 0, 0))
				{
					__atomic_store_n(finished_ptr, 1, __ATOMIC_SEQ_CST);
					// sleep(1);
					if (tnum == 0)
					{
						all_finished = steal_work(csr, num_threads, tnum, &tnum_target, &p, &num_packet_groups);
						if (all_finished == 1)
							break;
						__atomic_store_n(num_packet_groups_left_ptr, num_packet_groups, __ATOMIC_SEQ_CST);
						data = csr->t_compr_data[tnum_target];
						packet_group_pos = csr->t_packet_group_pos[tnum_target];
						pos = packet_group_pos[p];
						__atomic_store_n(finished_ptr, 0, __ATOMIC_SEQ_CST);
					}
					else
						break;
					// break;
				}
				while (pos != packet_group_pos[p+1])
				{
					pos += decompress_and_compute_kernel_sort_diff(&(data[pos]), x, y, i_s, i_e);
				}
				p++;
			}
			printf("%d out\n", tnum);
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
	len += snprintf(buf + len, buf_n - len, ",%s", "compression_time");
	len += snprintf(buf + len, buf_n - len, ",%s", "compression_spmv_loops");
	len += snprintf(buf + len, buf_n - len, ",%s", "tolerance");
	return len;
}


int
CSRCVSArrays::statistics_print_data(char * buf, long buf_n)
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

	double compression_spmv_loops = time_compress / (time_max / num_loops_out);

	double tolerance = atof(getenv("VC_TOLERANCE"));

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
	len += snprintf(buf + len, buf_n - len, ",%.4lf",  time_compress);
	len += snprintf(buf + len, buf_n - len, ",%.4lf",  compression_spmv_loops);
	len += snprintf(buf + len, buf_n - len, ",%g",  tolerance);
	return len;
}

