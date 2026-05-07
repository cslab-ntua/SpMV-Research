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
	#include "time_it_tsc.h"
	#include "parallel_util.h"

	// #define VEC_FORCE

	// #define VEC_X86_512
	// #define VEC_X86_256
	// #define VEC_X86_128
	// #define VEC_ARM_SVE

	#if DOUBLE == 0
		#define VTI   i32
		#define VTF   f32
		#define VTM   m32
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
		#define VTM   m64
		#define VEC_SCALE_SHIFT  3
		#define VEC_LEN  vec_len_default_f64
		// #define VEC_LEN  1
	#endif

	#include "vectorization/vectorization_gen.h"
#ifdef __cplusplus
}
#endif


struct thread_data {
	long i_s;
	long i_e;

	long j_s;
	long j_e;

	// ValueType v_s;
	ValueType v_e;
};

static struct thread_data ** tds;


extern int prefetch_distance;


struct CSR : Matrix_Format
{
	INT_T * row_ptr;      // the usual rowptr (of size m+1)
	INT_T * ja;      // the colidx of each NNZ (of size nnz)
	ValueType * a;   // the values (of size NNZ)
	double last_duration;

	CSR(INT_T * row_ptr_in, INT_T * col_ind, ValueTypeReference * values, long m, long n, long nnz) : Matrix_Format(m, n, nnz), last_duration(0)
	{
		int num_threads = omp_get_max_threads();
		double time_balance;

		printf("VEC_LEN = %d\n", VEC_LEN);

		row_ptr = (typeof(row_ptr)) aligned_alloc(64, (m+1) * sizeof(*row_ptr));
		ja = (typeof(ja)) aligned_alloc(64, nnz * sizeof(*ja));
		a = (typeof(a)) aligned_alloc(64, nnz * sizeof(*a));
		#pragma omp parallel for
		for (long i=0;i<m+1;i++)
			row_ptr[i] = row_ptr_in[i];
		#pragma omp parallel for
		for(long i=0;i<nnz;i++)
		{
			a[i]=values[i];
			ja[i]=col_ind[i];
		}

		tds = (typeof(tds)) aligned_alloc(64, num_threads * sizeof(*tds));

		time_balance = time_it(1,
			_Pragma("omp parallel")
			{
				int tnum = omp_get_thread_num();
				struct thread_data * td;
				int use_processes = atoi(getenv("USE_PROCESSES"));
				td = (typeof(td)) aligned_alloc(64, sizeof(*td));
				tds[tnum] = td;
				if (use_processes)
				{
					loop_partitioner_balance_iterations(num_threads, tnum, 0, m, &td->i_s, &td->i_e);
				}
				else
				{
					#ifdef CUSTOM_X86_VECTOR_PERFECT_NNZ_BALANCE
						long lower_boundary;
						loop_partitioner_balance_iterations(num_threads, tnum, 0, nnz, &td->j_s, &td->j_e);
						macros_binary_search(row_ptr, 0, m, td->j_s, &lower_boundary, NULL);           // Index boundaries are inclusive.
						td->i_s = lower_boundary;
						_Pragma("omp barrier")
						if (tnum == num_threads - 1)   // If we calculate each thread's boundaries individually some empty rows might be unassigned.
							td->i_e = m;
						else
							td->i_e = td->i_s + 1;
					#else
						loop_partitioner_balance_prefix_sums(num_threads, tnum, row_ptr, m, nnz, &td->i_s, &td->i_e);
						// loop_partitioner_balance(num_threads, tnum, 2, row_ptr, m, nnz, &td->i_s, &td->i_e);
					#endif
				}
			}
		);
		printf("balance time = %g\n", time_balance);
	}

	~CSR()
	{
		free(a);
		free(row_ptr);
		free(ja);
		free(tds);
	}

	void spmv(ValueType * x, ValueType * y) override;
	double get_last_duration() override { return last_duration; }
	void statistics_start() override;
	int statistics_print_data(__attribute__((unused)) char * buf, __attribute__((unused)) long buf_n) override;
};


void compute_csr_vector(CSR * restrict csr, ValueType * restrict x , ValueType * restrict y);
void compute_csr_vector_x86_perfect_nnz_balance(CSR * restrict csr, ValueType * restrict x , ValueType * restrict y);


void
CSR::spmv(ValueType * x, ValueType * y)
{
	struct timespec ts_s, ts_e;
	clock_gettime(CLOCK_MONOTONIC_RAW, &ts_s);
	#if defined(CSR_VECTOR)
		compute_csr_vector(this, x, y);
	#elif defined(CUSTOM_X86_VECTOR_PERFECT_NNZ_BALANCE)
		compute_csr_vector_x86_perfect_nnz_balance(this, x, y);
	#endif
	clock_gettime(CLOCK_MONOTONIC_RAW, &ts_e);
	last_duration = ((ts_e.tv_sec - ts_s.tv_sec) + (ts_e.tv_nsec - ts_s.tv_nsec) / 1e9) * 1000.0;
}


struct Matrix_Format *
csr_vec_to_format(INT_T * row_ptr, INT_T * col_ind, ValueTypeReference * values, long m, long n, long nnz, long symmetric, long symmetry_expanded)
{
	if (symmetric && !symmetry_expanded)
		error("symmetric matrices have to be expanded to be supported by this format");
	struct CSR * csr = new CSR(row_ptr, col_ind, values, m, n, nnz);
	csr->mem_footprint = nnz * (sizeof(ValueType) + sizeof(INT_T)) + (m+1) * sizeof(INT_T);
	#if defined(CSR_VECTOR)
		csr->format_name = (char *) "Custom_CSR_VEC";
	#elif defined(CUSTOM_X86_VECTOR_PERFECT_NNZ_BALANCE)
		csr->format_name = (char *) "Custom_CSR_PBV_x86";
	#endif
	return csr;
}


//==========================================================================================================================================
//= CSR x86
//==========================================================================================================================================


__attribute__((hot,pure))
static inline
double
subkernel_row_csr_vec(INT_T * restrict ja, ValueType * restrict a, ValueType * restrict x, long j_s, long j_e)
{
	long j, j_e_vector;
	const long mask = ~(((long) VEC_LEN) - 1); // Minimum number of elements for the vectorized code (power of 2).
	vec_t(VTF, VEC_LEN) v_a, v_x, v_sum;
	ValueType sum = 0;
	v_sum = vec_set1(VTF, VEC_LEN, 0);
	sum = 0;
	j_e_vector = j_s + ((j_e - j_s) & mask);
	for (j=j_s;j<j_e_vector;j+=VEC_LEN)
	{
		v_a = vec_loadu(VTF, VEC_LEN, &a[j]);
		v_x = vec_set_iter(VTF, VEC_LEN, iter, x[ja[j+iter]]);
		v_sum = vec_fmadd(VTF, VEC_LEN, v_a, v_x, v_sum);
	}
	// if (j_e_vector < j_e)
	// {
		// v_m = vec_mask_firstN(m64, VEC_LEN, j_e - j_e_vector);
		// v_a = vec_maskz_loadu(VTF, VEC_LEN, &a[j], v_m);
		// v_x = vec_set1(VTF, VEC_LEN, 0);
		// for (i=0;i<j_e-j_e_vector;i++)
			// vec_array(VTF, VEC_LEN, v_x)[i] = x[ja[j+i]];
		// v_sum = vec_fmadd(VTF, VEC_LEN, v_a, v_x, v_sum);
	// }
	sum = vec_reduce_add(VTF, VEC_LEN, v_sum);
	for (j=j_e_vector;j<j_e;j++)
		sum += a[j] * x[ja[j]];
	return sum;
}


void
subkernel_csr_vec(INT_T * restrict row_ptr, INT_T * restrict ja, ValueType * restrict a, ValueType * restrict x, ValueType * restrict y, long i_s, long i_e)
{
	long i, j_s, j_e;
	j_e = row_ptr[i_s];
	for (i=i_s;i<i_e;i++)
	{
		y[i] = 0;
		j_s = j_e;
		j_e = row_ptr[i+1];
		if (j_s == j_e)
			continue;
		y[i] = subkernel_row_csr_vec(ja, a, x, j_s, j_e);
	}
}


void
subkernel_csr_vec_density(INT_T * restrict row_ptr, INT_T * restrict ja, ValueType * restrict a, ValueType * restrict x, ValueType * restrict y, long i_s, long i_e)
{
	// double density;
	// if (i_s >= i_e)
		// return;
	// density = ((double) row_ptr[i_e] - row_ptr[i_s]) / (i_e - i_s);
	// if (density < 4)
	// {
		// subkernel_csr_scalar(row_ptr, ja, a, x, y, i_s, i_e);
	// }
	// else if (density < 8)
	// {
		// subkernel_csr_vector_x86_128d(row_ptr, ja, a, x, y, i_s, i_e);
	// }
	// else if (density < 16)
	// {
		// subkernel_csr_vector_x86_256d(row_ptr, ja, a, x, y, i_s, i_e);
	// }
	// else
	// {
		// subkernel_csr_vec(row_ptr, ja, a, x, y, i_s, i_e);
	// }
	subkernel_csr_vec(row_ptr, ja, a, x, y, i_s, i_e);
}


void
compute_csr_vector(CSR * restrict csr, ValueType * restrict x, ValueType * restrict y)
{
	#pragma omp parallel
	{
		int tnum = omp_get_thread_num();
		struct thread_data * td = tds[tnum];
		long i_s, i_e;
		i_s = td->i_s;
		i_e = td->i_e;
		// subkernel_csr_scalar(csr->row_ptr, csr->ja, csr->a, x, y, i_s, i_e);
		// subkernel_csr_vec(csr->row_ptr, csr->ja, csr->a, x, y, i_s, i_e);
		subkernel_csr_vec_density(csr->row_ptr, csr->ja, csr->a, x, y, i_s, i_e);
	}
}


//==========================================================================================================================================
//= CSR x86 Perfect NNZ Balance
//==========================================================================================================================================


/* void
compute_csr_vector_x86_perfect_nnz_balance(CSR * restrict csr, ValueType * restrict x, ValueType * restrict y)
{
	int num_threads = omp_get_max_threads();
	long t;
	#pragma omp parallel
	{
		int tnum = omp_get_thread_num();
		struct thread_data * td = tds[tnum];
		long i, i_s, i_e, j_s, j_e;
		td->v_s = 0;
		td->v_e = 0;

		i_s = td->i_s;
		i_e = td->i_e;

		i = i_s;
		y[i] = 0;
		j_s = csr->row_ptr[i];
		j_e = csr->row_ptr[i+1];
		if (td->j_s > j_s)
			j_s = td->j_s;
		if (td->j_e < j_e)
			j_e = td->j_e;
		if (j_s < j_e)
		{
			td->v_s = subkernel_row_csr_vector_x86(csr->ja, csr->a, x, j_s, j_e);
		}

		subkernel_csr_vec_density(csr->row_ptr, csr->ja, csr->a, x, y, i_s+1, i_e-1);

		i = i_e-1;
		if (i > i_s)
		{
			y[i] = 0;
			j_s = csr->row_ptr[i];
			j_e = csr->row_ptr[i+1];
			if (td->j_s > j_s)
				j_s = td->j_s;
			if (td->j_e < j_e)
				j_e = td->j_e;
			if (j_s < j_e)
				td->v_e = subkernel_row_csr_vector_x86(csr->ja, csr->a, x, j_s, j_e);
		}
	}
	for (t=0;t<num_threads;t++)
	{
		y[tds[t]->i_s] += tds[t]->v_s;
		if (tds[t]->i_e - 1 > tds[t]->i_s)
			y[tds[t]->i_e - 1] += tds[t]->v_e;
	}
} */


void
compute_csr_vector_x86_perfect_nnz_balance(CSR * restrict csr, ValueType * restrict x, ValueType * restrict y)
{
	int num_threads = omp_get_max_threads();
	long t;
	#pragma omp parallel
	{
		int tnum = omp_get_thread_num();
		struct thread_data * td = tds[tnum];
		long i, i_s, i_e, j, j_s, j_e;

		i_s = td->i_s;
		i_e = td->i_e;

		if (i_e - 1 >= 0)
			y[i_e - 1] = 0;

		#pragma omp barrier

		ValueType sum;
		j_s = td->j_s;

		j = j_s;
		for (i=i_s;i<i_e-1;i++)
		{
			j_e = csr->row_ptr[i+1];
			// sum = 0;
			// for (;j<j_e;j++)
			// {
				// sum += csr->a[j] * x[csr->ja[j]];
			// }
			// y[i] = sum;

			// y[i] = subkernel_row_csr_scalar(csr, x, j, j_e);
			// y[i] = subkernel_row_csr_vector_x86_512d(csr->ja, csr->a, x, j, j_e);

			// long degree = j_e - j;
			// if (degree < 4)
			// {
				// y[i] = subkernel_row_csr_scalar(csr->ja, csr->a, x, j, j_e);
			// }
			// else if (degree < 8)
			// {
				// y[i] = subkernel_row_csr_vector_x86_128d(csr->ja, csr->a, x, j, j_e);
			// }
			// else if (degree < 16)
			// {
				// y[i] = subkernel_row_csr_vector_x86_256d(csr->ja, csr->a, x, j, j_e);
			// }
			// else
			// {
				// y[i] = subkernel_row_csr_vector_x86_512d(csr->ja, csr->a, x, j, j_e);
			// }
			y[i] = subkernel_row_csr_vec(csr->ja, csr->a, x, j, j_e);

			j = j_e;
		}

		i = i_e - 1;
		j =  csr->row_ptr[i];
		if (j_s > j)
			j = j_s;
		j_e = td->j_e;
		sum = 0;
		for (;j<j_e;j++)
		{
			sum += csr->a[j] * x[csr->ja[j]];
		}
		td->v_e = sum;
	}
	for (t=0;t<num_threads;t++)
	{
		if (tds[t]->i_e - 1 < csr->m)
			y[tds[t]->i_e - 1] += tds[t]->v_e;
	}
}


//==========================================================================================================================================
//= Print Statistics
//==========================================================================================================================================


void
CSR::statistics_start()
{
}


int
csr_vec_statistics_print_labels(__attribute__((unused)) char * buf, __attribute__((unused)) long buf_n)
{
	return 0;
}


int
CSR::statistics_print_data(__attribute__((unused)) char * buf, __attribute__((unused)) long buf_n)
{
	// int num_threads = omp_get_max_threads();
	// long i, i_s, i_e;
	// for (i=0;i<num_threads;i++)
	// {
		// i_s = tds[i]->i_s;
		// i_e = tds[i]->i_e;
		// printf("%3ld: i=[%8ld, %8ld] (%8ld) , nnz=%8d\n", i, i_s, i_e, i_e - i_s, row_ptr[i_e] - row_ptr[i_s]);
	// }
	return 0;
}


#ifndef HYBRID
struct Matrix_Format *
csr_to_format(INT_T * row_ptr, INT_T * col_ind, ValueTypeReference * values, long m, long n, long nnz, long symmetric, long symmetry_expanded)
{
	return csr_vec_to_format(row_ptr, col_ind, values, m, n, nnz, symmetric, symmetry_expanded);
}

int
statistics_print_labels(__attribute__((unused)) char * buf, __attribute__((unused)) long buf_n)
{
	return csr_vec_statistics_print_labels(buf, buf_n);
}
#endif
