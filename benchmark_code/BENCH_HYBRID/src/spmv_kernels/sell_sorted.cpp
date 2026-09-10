#include <stdlib.h>
#include <stdio.h>
#include <omp.h>
#include <time.h>

#include "macros/cpp_defines.h"

#include "spmv_kernel.h"

#ifdef __cplusplus
extern "C"{
#endif
	#include "macros/macrolib.h"
	#include "time_it.h"
	#include "parallel_util.h"
	#include "array_metrics.h"

	// #define VEC_FORCE

	// #define VEC_X86_512
	// #define VEC_X86_256
	// #define VEC_X86_128
	// #define VEC_ARM_SVE

	#if DOUBLE == 0
		#define VTI   i32
		#define VTF   f32
		#define VEC_SCALE_SHIFT  2
		// #define VEC_LEN  1
		#define VEC_LEN  vec_len_default_f32
		// #define VEC_LEN  vec_len_default_f64
		// #define VEC_LEN  4
		// #define VEC_LEN  8
		// #define VEC_LEN  16
		// #define VEC_LEN  32
	#elif DOUBLE == 1
		#define VTI   i64
		#define VTF   f64
		#define VEC_SCALE_SHIFT  3
		#define VEC_LEN  vec_len_default_f64
		// #define VEC_LEN  1
	#endif

	// #include "vectorization.h"
	#include "vectorization/vectorization_gen.h"

	#include "functools/functools_gen_undef.h"
	#define FUNCTOOLS_GEN_TYPE_1  int
	#define FUNCTOOLS_GEN_TYPE_2  int
	#define FUNCTOOLS_GEN_SUFFIX  _i_i
	#include "functools/functools_gen.c"
	static inline
	int
	functools_map_fun(int * A, long i)
	{
		return A[i];
	}
	static inline
	int
	functools_reduce_fun(int a, int b)
	{
		return a + b;
	}

	#include "sort/bucketsort/bucketsort_gen_undef.h"
	#define BUCKETSORT_GEN_TYPE_1  int
	#define BUCKETSORT_GEN_TYPE_2  int
	#define BUCKETSORT_GEN_TYPE_3  int
	#define BUCKETSORT_GEN_TYPE_4  int
	#define BUCKETSORT_GEN_SUFFIX  _i_i_i_v
	#include "sort/bucketsort/bucketsort_gen.c"
	static inline
	int
	bucketsort_find_bucket(int * A, long i, __attribute__((unused)) int * degree_max_ptr)
	{
		return A[i+1] - A[i];   // Ascending order.
		// return *degree_max_ptr - (A[i+1] - A[i]);   // Descending order.
	}

#ifdef __cplusplus
}
#endif


struct thread_data {
	long ii_s;
	long ii_e;

	long i_s;
	long i_e;
};

static struct thread_data ** tds;


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


struct SELL_Sorted_Array : Matrix_Format
{
	ValueType * a;
	long num_row_clusters;
	INT_T * row_cluster_ptr;
	INT_T * ja;

	int * permutation;
	int * rev_permutation;

	long m_ext;
	long nnz_ext;

	double last_duration;

	SELL_Sorted_Array(INT_T * row_ptr, INT_T * col_ind, ValueTypeReference * values, long m, long n, long nnz) : Matrix_Format(m, n, nnz), last_duration(0)
	{
		long num_threads = omp_get_max_threads();

		tds = (typeof(tds)) aligned_alloc(64, num_threads * sizeof(*tds));

		printf("VEC_LEN = %d\n", VEC_LEN);

		num_row_clusters = (m + VEC_LEN - 1) / VEC_LEN;
		m_ext = num_row_clusters * VEC_LEN;

		row_cluster_ptr = (typeof(row_cluster_ptr)) aligned_alloc(64, (num_row_clusters+1) * sizeof(*row_cluster_ptr));

		permutation = (typeof(permutation)) aligned_alloc(64, m * sizeof(*permutation));
		rev_permutation = (typeof(rev_permutation)) aligned_alloc(64, m * sizeof(*rev_permutation));

		INT_T * row_ptr_reordered = (typeof(row_ptr_reordered)) aligned_alloc(64, (m+1) * sizeof(*row_ptr_reordered));
		INT_T * col_ind_reordered = (typeof(col_ind_reordered)) aligned_alloc(64, nnz * sizeof(*col_ind_reordered));
		ValueType * values_reordered = (typeof(values_reordered)) aligned_alloc(64, nnz * sizeof(*values_reordered));

		#pragma omp parallel
		{
			long tnum = omp_get_thread_num();
			struct thread_data * td;
			long i, ii_s, ii_e, i_s, i_e, j, k;

			td = (typeof(td)) aligned_alloc(64, sizeof(*td));
			tds[tnum] = td;

			#pragma omp for
			for (i=0;i<num_row_clusters;i++)
			{
				row_cluster_ptr[i] = row_ptr[i * VEC_LEN];
			}
			#pragma omp single
			{
				row_cluster_ptr[num_row_clusters] = row_ptr[m];
			}


			loop_partitioner_balance_prefix_sums(num_threads, tnum, row_cluster_ptr, num_row_clusters, nnz, &ii_s, &ii_e);
			i_s = ii_s * VEC_LEN;
			i_e = ii_e * VEC_LEN;
			if (tnum == num_threads - 1)
				i_e = m;
			td->ii_s = ii_s;
			td->ii_e = ii_e;
			td->i_s = i_s;
			td->i_e = i_e;
			// printf("%2ld: [%8ld %8ld] [%8ld %8ld] [%8ld %8ld] (%d) m=%ld\n", tnum, ii_s, ii_e, ii_s*VEC_LEN, ii_e*VEC_LEN, i_s, i_e, row_ptr[i_e] - row_ptr[i_s], m);

			int degree, degree_max = 0;
			for (i=i_s;i<i_e;i++)
			{
				degree = row_ptr[i+1] - row_ptr[i];
				if (degree > degree_max)
					degree_max = degree;
			}

			bucketsort_stable_recalculate_bucket_serial(&row_ptr[i_s], i_e-i_s, degree_max+1, &degree_max, &permutation[i_s], NULL);
			for (i=i_s;i<i_e;i++)
			{
				permutation[i] += i_s;
				rev_permutation[permutation[i]] = i;
			}

			for (i=i_s;i<i_e;i++)
			{
				row_ptr_reordered[permutation[i]] = row_ptr[i+1] - row_ptr[i];
				// if (tnum == 0)
					// printf("%d\n", row_ptr[i+1] - row_ptr[i]);
			}
			#pragma omp single
			{
				row_ptr_reordered[m] = 0;
			}
			scan_reduce_concurrent(row_ptr_reordered, row_ptr_reordered, m+1, 0, 1, 0);

			#pragma omp barrier

			k = row_ptr[i_s];
			for (i=i_s;i<i_e;i++)
			{
				for (j=row_ptr[rev_permutation[i]];j<row_ptr[rev_permutation[i]+1];j++,k++)
				{
					col_ind_reordered[k] = col_ind[j];
					values_reordered[k] = values[j];
				}
				// if (tnum == 0)
					// printf("%d\n", row_ptr[rev_permutation[i]+1] - row_ptr[rev_permutation[i]]);
			}
		}

		#pragma omp parallel
		{
			long tnum = omp_get_thread_num();
			struct thread_data * td = tds[tnum];
			long i, ii, j, jj, k, k_s, k_e;
			long ii_s, ii_e;
			long i_s, i_e;
			long degree;
			long col = 0;
			long width;

			ii_s = td->ii_s;
			ii_e = td->ii_e;
			i_s = td->i_s;
			i_e = td->i_e;

			for (i=i_s;i<i_e;i+=VEC_LEN)
			{
				width = 0;
				k_s = i;
				k_e = i + VEC_LEN;
				if (k_e > m)
					k_e = m;
				for (k=k_s;k<k_e;k++)
				{
					degree = row_ptr_reordered[k+1] - row_ptr_reordered[k];
					if (degree > width)
						width = degree;
				}
				row_cluster_ptr[i/VEC_LEN] = VEC_LEN * width;
			}
			#pragma omp single
			{
				row_cluster_ptr[num_row_clusters] = 0;
			}
			scan_reduce_concurrent(row_cluster_ptr, row_cluster_ptr, num_row_clusters+1, 0, 1, 0);   // scan_reduce_concurrent(_TYPE_IN * A, _TYPE_OUT * P, long N, _TYPE_OUT zero, const int exclusive, const int backwards);

			#pragma omp barrier

			#pragma omp single
			{
				nnz_ext = row_cluster_ptr[num_row_clusters];
				a = (typeof(a)) aligned_alloc(64, nnz_ext * sizeof(*a));
				ja = (typeof(ja)) aligned_alloc(64, nnz_ext * sizeof(*ja));
			}

			col = 0;
			for (ii=ii_s;ii<ii_e;ii++)
			{
				width = (row_cluster_ptr[ii+1] - row_cluster_ptr[ii]) / VEC_LEN;
				long i_c_s = VEC_LEN * ii;
				long i_c_e = i_c_s + VEC_LEN;
				if (i_c_e > m)
					i_c_e = m;
				jj = row_cluster_ptr[ii];
				for (i=i_c_s;i<i_c_e;i++)
				{
					for (j=row_ptr_reordered[i];j<row_ptr_reordered[i+1];j++,jj++)
					{
						a[jj] = values_reordered[j];
						col = col_ind_reordered[j];
						ja[jj] = col;
					}
					for (;j<row_ptr_reordered[i]+width;j++,jj++)   // Padding of smaller rows.
					{
						a[jj] = 0;
						ja[jj] = col;
					}
				}
				for (;jj<row_cluster_ptr[ii+1];jj++)   // Padding of missing rows for last row cluster.
				{
					a[jj] = 0;
					ja[jj] = col;
				}
				transpose(&a[row_cluster_ptr[ii]], VEC_LEN, width);
				transpose(&ja[row_cluster_ptr[ii]], VEC_LEN, width);
			}
		}

		mem_footprint = (num_row_clusters+1) * sizeof(INT_T) + nnz_ext * (sizeof(ValueType) + sizeof(INT_T)) + m * sizeof(INT_T);   // Plus the permutation array size.
	}

	~SELL_Sorted_Array()
	{
		free(a);
		free(ja);
	}

	void spmv(ValueType * x, ValueType * y) override;
	double get_last_duration() override { return last_duration; }
	void statistics_start() override;
	int statistics_print_data(__attribute__((unused)) char * buf, __attribute__((unused)) long buf_n) override;
};


void compute_sell(SELL_Sorted_Array * sell, ValueType * x , ValueType * y);


void
SELL_Sorted_Array::spmv(ValueType * x, ValueType * y)
{
	struct timespec ts_s, ts_e;
	clock_gettime(CLOCK_MONOTONIC_RAW, &ts_s);
	compute_sell(this, x, y);
	clock_gettime(CLOCK_MONOTONIC_RAW, &ts_e);
	last_duration = ((ts_e.tv_sec - ts_s.tv_sec) + (ts_e.tv_nsec - ts_s.tv_nsec) / 1e9) * 1000.0;
}


struct Matrix_Format *
sell_sorted_to_format(INT_T * row_ptr, INT_T * col_ind, ValueTypeReference * values, long m, long n, long nnz, long symmetric, long symmetry_expanded)
{
	if (symmetric && !symmetry_expanded)
		error("symmetric matrices have to be expanded to be supported by this format");
	struct SELL_Sorted_Array * sell = new SELL_Sorted_Array(row_ptr, col_ind, values, m, n, nnz);
	sell->format_name = (char *) "SELL_SORTED";
	return sell;
}


//==========================================================================================================================================
//= SELLPACK
//==========================================================================================================================================


void
compute_sell(SELL_Sorted_Array * sell, ValueType * x , ValueType * y)
{
	#pragma omp parallel
	{
		long tnum = omp_get_thread_num();
		struct thread_data * td = tds[tnum];
		vec_t(VTF, VEC_LEN) zero = vec_set1(VTF, VEC_LEN, 0);
		__attribute__((unused)) vec_t(VTF, VEC_LEN) val = zero, mul = zero, x_buf = zero, sum = zero;
		__attribute__((unused)) vec_t(i32, VEC_LEN) idx;
		long ii, ii_s, ii_e, jj, jj_s, jj_e;
		long i, k;
		__attribute__((unused)) long i_s, i_e;
		ii_s = td->ii_s;
		ii_e = td->ii_e;
		i_s = td->i_s;
		i_e = td->i_e;

		long ii_e_last = ii_e;
		if (ii_e * VEC_LEN > i_e)
			ii_e_last -= VEC_LEN;

		// #pragma GCC unroll 2
		for (ii=ii_s;ii<ii_e_last;ii++)
		{
			sum = vec_set1(VTF, VEC_LEN, 0);
			jj_s = sell->row_cluster_ptr[ii];
			jj_e = sell->row_cluster_ptr[ii+1];
			for (jj=jj_s;jj<jj_e;jj+=VEC_LEN)
			{

				// for (k=0;k<VEC_LEN;k++)
				// {
					// vec_array(VTF, VEC_LEN, sum)[k] += sell->a[jj+k] * x[sell->ja[jj+k]];
				// }

				// for (k=0;k<VEC_LEN;k++)
				// {
					// vec_array(VTF, VEC_LEN, mul)[k] = sell->a[jj+k] * x[sell->ja[jj+k]];
				// }
				// sum = vec_add(VTF, VEC_LEN, sum, mul);

				val = vec_loadu(VTF, VEC_LEN, &sell->a[jj]);

				// x_buf = vec_set_iter(VTF, VEC_LEN, iter, x[sell->ja[jj+iter]]);
				idx = vec_loadu(i32, VEC_LEN, &sell->ja[jj]);
				x_buf = vec_gather(VTF, i32, VEC_LEN, x, idx);
				// vint32mf2_t idx = __riscv_vle32_v_i32mf2(&sell->ja[jj], VEC_LEN);
				// x_buf = __riscv_vluxei32_v_f64m1(x, __riscv_vsll_vx_u32mf2(idx, VEC_SCALE_SHIFT, VEC_LEN),  VEC_LEN);

				sum = vec_fmadd(VTF, VEC_LEN, val, x_buf, sum);

			}
			i = VEC_LEN * ii;
			for (k=0;k<VEC_LEN;k++)
			{
				y[i + k] = vec_array(VTF, VEC_LEN, sum)[k];
				// y[sell->rev_permutation[i + k]] = vec_array(VTF, VEC_LEN, sum)[k];
			}

		}

		for (ii=ii_e_last;ii<ii_e;ii++)
		{
			sum = vec_set1(VTF, VEC_LEN, 0);
			jj_s = sell->row_cluster_ptr[ii];
			jj_e = sell->row_cluster_ptr[ii+1];
			for (jj=jj_s;jj<jj_e;jj+=VEC_LEN)
			{
				val = vec_loadu(VTF, VEC_LEN, &sell->a[jj]);
				x_buf = vec_set_iter(VTF, VEC_LEN, iter, x[sell->ja[jj+iter]]);
				sum = vec_fmadd(VTF, VEC_LEN, val, x_buf, sum);
			}
			i = VEC_LEN * ii;
			for (k=0;k<VEC_LEN;k++)
			{
				if (i+k < i_e)
					y[i + k] = vec_array(VTF, VEC_LEN, sum)[k];
					// y[sell->rev_permutation[i + k]] = vec_array(VTF, VEC_LEN, sum)[k];
			}
		}

	}
}


//==========================================================================================================================================
//= Print Statistics
//==========================================================================================================================================


void
SELL_Sorted_Array::statistics_start()
{
}


int
sell_sorted_statistics_print_labels(__attribute__((unused)) char * buf, __attribute__((unused)) long buf_n)
{
	return 0;
}


int
SELL_Sorted_Array::statistics_print_data(__attribute__((unused)) char * buf, __attribute__((unused)) long buf_n)
{
	return 0;
}

#ifndef HYBRID
struct Matrix_Format *
csr_to_format(INT_T * row_ptr, INT_T * col_ind, ValueTypeReference * values, long m, long n, long nnz, long symmetric, long symmetry_expanded)
{
	return sell_sorted_to_format(row_ptr, col_ind, values, m, n, nnz, symmetric, symmetry_expanded);
}

int
statistics_print_labels(char * buf, long buf_n)
{
	return sell_sorted_statistics_print_labels(buf, buf_n);
}
#endif
