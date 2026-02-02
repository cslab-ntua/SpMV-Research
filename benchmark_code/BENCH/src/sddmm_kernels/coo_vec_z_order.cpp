#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

#include "macros/cpp_defines.h"

#include "sddmm_kernel.h"

#ifdef __cplusplus
extern "C"{
#endif
	#include "macros/macrolib.h"
	#include "time_it.h"
	#include "parallel_util.h"

	#include "bit_ops.h"

	// Samplesort
	#include "sort/samplesort/samplesort_gen_undef.h"
	#define SAMPLESORT_GEN_TYPE_1  INT_T
	#define SAMPLESORT_GEN_TYPE_2  INT_T
	#define SAMPLESORT_GEN_TYPE_3  INT_T
	#define SAMPLESORT_GEN_TYPE_4  uint64_t
	#define SAMPLESORT_GEN_SUFFIX  CONCAT(_COO_VEC_CPP, COO_VEC_CPP_SUFFIX)
	#include "sort/samplesort/samplesort_gen.c"
	static inline
	int
	samplesort_cmp(INT_T a, INT_T b, uint64_t * z_pos)
	{
		return (z_pos[a] > z_pos[b]) ? 1 : (z_pos[a] < z_pos[b]) ? -1 : 0;
	}

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


int
z_order_comp(const void * a_ptr, const void * b_ptr, void * aux)
{
	INT_T a = *((INT_T *) a_ptr);
	INT_T b = *((INT_T *) b_ptr);
	uint64_t * z_pos = (uint64_t *) aux;
	return (z_pos[a] > z_pos[b]) ? 1 : (z_pos[a] < z_pos[b]) ? -1 : 0;
}

void
z_order_permutation_serial(INT_T * row_ind, INT_T * col_ind, ValueTypeReference * values, long nnz,
		INT_T ** row_ind_ret, INT_T ** col_ind_ret, ValueType ** a_ret)
{
	uint64_t * z_pos = (typeof(z_pos)) aligned_alloc(64, nnz * sizeof(*z_pos));
	INT_T * rev_permutation = (typeof(rev_permutation)) aligned_alloc(64, nnz * sizeof(*rev_permutation));
	long i;
	for (i=0;i<nnz;i++)
	{
		z_pos[i] = bits_u32_interleave(row_ind[i], col_ind[i]);
		rev_permutation[i] = i;
	}

	qsort_r(rev_permutation, nnz, sizeof(*rev_permutation), z_order_comp, (void *) z_pos);

	INT_T * tmp_row_ind = (typeof(tmp_row_ind)) aligned_alloc(64, nnz * sizeof(*tmp_row_ind));
	INT_T * tmp_col_ind = (typeof(tmp_col_ind)) aligned_alloc(64, nnz * sizeof(*tmp_col_ind));
	ValueType * tmp_a = (typeof(tmp_a)) aligned_alloc(64, nnz * sizeof(*tmp_a));
	for (i=0;i<nnz;i++)
	{
		tmp_row_ind[i] = row_ind[rev_permutation[i]];
		tmp_col_ind[i] = col_ind[rev_permutation[i]];
		tmp_a[i] = values[rev_permutation[i]];
	}

	free(z_pos);
	free(rev_permutation);
	*row_ind_ret = tmp_row_ind;
	*col_ind_ret = tmp_col_ind;
	*a_ret = tmp_a;
}


void
z_order_permutation(INT_T * row_ind, INT_T * col_ind, ValueTypeReference * values, long nnz,
		INT_T ** row_ind_ret, INT_T ** col_ind_ret, ValueType ** a_ret)
{
	uint64_t * z_pos = (typeof(z_pos)) aligned_alloc(64, nnz * sizeof(*z_pos));
	INT_T * rev_permutation = (typeof(rev_permutation)) aligned_alloc(64, nnz * sizeof(*rev_permutation));
	#pragma omp parallel
	{
		long i;
		#pragma omp for
		for (i=0;i<nnz;i++)
		{
			z_pos[i] = bits_u32_interleave(row_ind[i], col_ind[i]);
			rev_permutation[i] = i;
		}
	}

	samplesort(rev_permutation, nnz, z_pos);

	INT_T * tmp_row_ind = (typeof(tmp_row_ind)) aligned_alloc(64, nnz * sizeof(*tmp_row_ind));
	INT_T * tmp_col_ind = (typeof(tmp_col_ind)) aligned_alloc(64, nnz * sizeof(*tmp_col_ind));
	ValueType * tmp_a = (typeof(tmp_a)) aligned_alloc(64, nnz * sizeof(*tmp_a));
	#pragma omp parallel
	{
		long i;
		#pragma omp for
		for (i=0;i<nnz;i++)
		{
			tmp_row_ind[i] = row_ind[rev_permutation[i]];
			tmp_col_ind[i] = col_ind[rev_permutation[i]];
			tmp_a[i] = values[rev_permutation[i]];
		}
	}

	free(z_pos);
	free(rev_permutation);
	*row_ind_ret = tmp_row_ind;
	*col_ind_ret = tmp_col_ind;
	*a_ret = tmp_a;
}


struct COO : Matrix_Format
{
	INT_T * row_ind;
	INT_T * col_ind;
	ValueType * a;

	struct thread_data ** tds;

	COO(INT_T * row_ptr, INT_T * col_ind_in, ValueTypeReference * values, long m, long n, long nnz) : Matrix_Format(m, n, nnz)
	{
		int num_threads = omp_get_max_threads();
		double time_balance;

		printf("VEC_LEN = %d\n", VEC_LEN);

		row_ind = (typeof(row_ind)) aligned_alloc(64, nnz * sizeof(*row_ind));
		col_ind = (typeof(col_ind)) aligned_alloc(64, nnz * sizeof(*col_ind));
		a = (typeof(a)) aligned_alloc(64, nnz * sizeof(*a));
		#pragma omp parallel
		{
			long i, j, j_s, j_e;
			#pragma omp for
			for (i=0;i<m;i++)
			{
				j_s = row_ptr[i];
				j_e = row_ptr[i+1];
				for (j=j_s;j<j_e;j++)
				{
					row_ind[j] = i;
					col_ind[j] = col_ind_in[j];
					a[j] = values[j];
				}
			}
		}

		INT_T * tmp_row_ind;
		INT_T * tmp_col_ind;
		ValueType * tmp_a;
		z_order_permutation(row_ind, col_ind, a, nnz, &tmp_row_ind, &tmp_col_ind, &tmp_a);
		free(row_ind);
		free(col_ind);
		free(a);
		row_ind = tmp_row_ind;
		col_ind = tmp_col_ind;
		a = tmp_a;


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

					// loop_partitioner_balance_prefix_sums(num_threads, tnum, row_ptr, m, nnz, &td->i_s, &td->i_e);
					// td->j_s = row_ptr[td->i_s];
					// td->j_e = row_ptr[td->i_e];
					long lower_boundary;
					loop_partitioner_balance_iterations(num_threads, tnum, 0, nnz, &td->j_s, &td->j_e);
					macros_binary_search(row_ptr, 0, m, td->j_s, &lower_boundary, NULL);           // Index boundaries are inclusive.
					td->i_s = lower_boundary;
					_Pragma("omp barrier")
					if (tnum == num_threads - 1)   // If we calculate each thread's boundaries individually some empty rows might be unassigned.
						td->i_e = m;
					else
						td->i_e = td->i_s + 1;

				}
			}
		);
		printf("balance time = %g\n", time_balance);
	}

	~COO()
	{
		free(a);
		free(row_ind);
		free(col_ind);
		free(tds);
	}

	void sddmm(long K, ValueType * A, ValueType * B, ValueType * C);
	void statistics_start();
	int statistics_print_data(char * buf, long buf_n);
};


// static __attribute__((always_inline)) inline
// ValueType
// dot(long K, ValueType * x, ValueType * y)
// {
	// long i;
	// ValueType sum = 0;
	// for (i=0;i<K;i++)
	// {
		// sum += x[i] * y[i];
	// }
	// return sum;
// }


static __attribute__((always_inline)) inline
ValueType
dot(long K, ValueType * x, ValueType * y)
{
	long i, i_mul;
	ValueType sum;
	vec_t(VTF, VEC_LEN) v_x, v_y, v_sum;
	i_mul = K - K % VEC_LEN;
	v_sum = vec_set1(VTF, VEC_LEN, 0);
	for (i=0;i<i_mul;i+=VEC_LEN)
	{
		v_x = vec_loadu(VTF, VEC_LEN, &x[i]);
		v_y = vec_loadu(VTF, VEC_LEN, &y[i]);
		v_sum = vec_fmadd(VTF, VEC_LEN, v_x, v_y, v_sum);
	}
	sum = vec_reduce_add(VTF, VEC_LEN, v_sum);
	for (i=i_mul;i<K;i++)
		sum += x[i] * y[i];
	return sum;
}


void
compute_csr(COO * restrict csr, long K, ValueType * A, ValueType * B, ValueType * C)
{
	#pragma omp parallel
	{
		int tnum = omp_get_thread_num();
		struct thread_data * td = csr->tds[tnum];
		long j, j_s, j_e;
		j_s = td->j_s;
		j_e = td->j_e;
		for (j=j_s;j<j_e;j++)
		{
			C[j] = dot(K, &A[csr->row_ind[j] * K], &B[csr->col_ind[j] * K]);
		}
	}
}


void
COO::sddmm(long K, ValueType * A, ValueType * B, ValueType * C)
{
	compute_csr(this, K, A, B, C);
}


struct Matrix_Format *
csr_to_format(INT_T * row_ind, INT_T * col_ind, ValueTypeReference * values, long m, long n, long nnz, long symmetric, long symmetry_expanded)
{
	if (symmetric && !symmetry_expanded)
		error("symmetric matrices have to be expanded to be supported by this format");
	struct COO * csr = new COO(row_ind, col_ind, values, m, n, nnz);
	csr->mem_footprint = nnz * (sizeof(ValueType) + 2*sizeof(INT_T));
	csr->format_name = (char *) "Custom_COO_VEC_Z_ORDER";
	return csr;
}


//==========================================================================================================================================
//= Print Statistics
//==========================================================================================================================================


void
COO::statistics_start()
{
}


int
statistics_print_labels(__attribute__((unused)) char * buf, __attribute__((unused)) long buf_n)
{
	return 0;
}


int
COO::statistics_print_data(__attribute__((unused)) char * buf, __attribute__((unused)) long buf_n)
{
	return 0;
}

