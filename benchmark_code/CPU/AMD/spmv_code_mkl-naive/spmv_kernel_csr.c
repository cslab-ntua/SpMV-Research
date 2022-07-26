#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

#include "macros/cpp_defines.h"

#include "common.h"
#include "spmv_kernel.h"

#ifdef __cplusplus
extern "C"{
#endif
	#include "macros/macrolib.h"
	#include "time_it.h"
	#include "parallel_util.h"
#ifdef __cplusplus
}
#endif


INT_T * thread_i_s = NULL;
INT_T * thread_i_e = NULL;

INT_T * thread_j_s = NULL;
INT_T * thread_j_e = NULL;

ValueType * thread_v_s = NULL;
ValueType * thread_v_e = NULL;


struct CSRArrays : Matrix_Format
{
	INT_T * ia;      // the usual rowptr (of size m+1)
	INT_T * ja;      // the colidx of each NNZ (of size nnz)
	ValueType * a;   // the values (of size NNZ)

	CSRArrays(INT_T * ia, INT_T * ja, ValueType * a, long m, long n, long nnz) : Matrix_Format(m, n, nnz), ia(ia), ja(ja), a(a)
	{
		int num_threads = omp_get_max_threads();
		double time_balance;
		thread_i_s = (INT_T *) malloc(num_threads * sizeof(*thread_i_s));
		thread_i_e = (INT_T *) malloc(num_threads * sizeof(*thread_i_e));
		thread_j_s = (INT_T *) malloc(num_threads * sizeof(*thread_j_s));
		thread_j_e = (INT_T *) malloc(num_threads * sizeof(*thread_j_e));
		thread_v_s = (ValueType *) malloc(num_threads * sizeof(*thread_v_s));
		thread_v_e = (ValueType *) malloc(num_threads * sizeof(*thread_v_e));
		time_balance = time_it(1,
			_Pragma("omp parallel")
			{
				int tnum = omp_get_thread_num();
				#if defined(NAIVE) || defined(PROC_BENCH)
					loop_partitioner_balance_iterations(num_threads, tnum, 0, m, &thread_i_s[tnum], &thread_i_e[tnum]);
				#else
					loop_partitioner_balance_partial_sums(num_threads, tnum, ia, m, nnz, &thread_i_s[tnum], &thread_i_e[tnum]);
					// loop_partitioner_balance(num_threads, tnum, 2, ia, m, nnz, &thread_i_s[tnum], &thread_i_e[tnum]);
				#endif
				#ifdef CUSTOM_VECTOR_PERFECT_NNZ_BALANCE
					loop_partitioner_balance_iterations(num_threads, tnum, 0, nnz, &thread_j_s[tnum], &thread_j_e[tnum]);
				#endif
			}
			#ifdef CUSTOM_VECTOR_PERFECT_NNZ_BALANCE
			for (long i=0;i<num_threads;i++)
			{
				if (thread_j_s[i] < ia[thread_i_s[i]])
					thread_i_s[i] -= 1;
				if (thread_j_e[i] > ia[thread_i_e[i]])
					thread_i_e[i] += 1;
			}
			#endif
		);
		printf("balance time = %g\n", time_balance);
	}

	~CSRArrays()
	{
		free(a);
		free(ia);
		free(ja);
		free(thread_i_s);
		free(thread_i_e);
		free(thread_j_s);
		free(thread_j_e);
		free(thread_v_s);
		free(thread_v_e);
	}

	void spmv(ValueType * x, ValueType * y);
};


void compute_csr_custom(CSRArrays * restrict csr, ValueType * restrict x , ValueType * restrict y);
void compute_csr_custom_omp_prefetch(CSRArrays * restrict csr, ValueType * restrict x , ValueType * restrict y);
void compute_csr_custom_omp_simd(CSRArrays * restrict csr, ValueType * restrict x , ValueType * restrict y);
void compute_csr_custom_vector_x86(CSRArrays * restrict csr, ValueType * restrict x , ValueType * restrict y);
void compute_csr_custom_x86_queues(CSRArrays * restrict csr, ValueType * restrict x , ValueType * restrict y);
void compute_csr_custom_vector(CSRArrays * restrict csr, ValueType * restrict x , ValueType * restrict y);
void compute_csr_custom_perfect_nnz_balance(CSRArrays * restrict csr, ValueType * restrict x , ValueType * restrict y);


void
CSRArrays::spmv(ValueType * x, ValueType * y)
{
	#if defined(CUSTOM_PREFETCH)
		compute_csr_custom_omp_prefetch(this, x, y);
	#elif defined(CUSTOM_SIMD)
		compute_csr_custom_omp_simd(this, x, y);
	#elif defined(CUSTOM_VECTOR_X86)
		compute_csr_custom_vector_x86(this, x, y);
	#elif defined(CUSTOM_QUEUES)
		compute_csr_custom_x86_queues(this, x, y);
	#elif defined(CUSTOM_VECTOR)
		compute_csr_custom_vector(this, x, y);
	#elif defined(CUSTOM_VECTOR_PERFECT_NNZ_BALANCE)
		compute_csr_custom_perfect_nnz_balance(this, x, y);
	#else
		compute_csr_custom(this, x, y);
	#endif
}


struct Matrix_Format *
csr_to_format(INT_T * row_ptr, INT_T * col_ind, ValueType * values, long m, long n, long nnz)
{
	struct CSRArrays * csr = new CSRArrays(row_ptr, col_ind, values, m, n, nnz);
	csr->mem_footprint = nnz * (sizeof(ValueType) + sizeof(INT_T)) + (m+1) * sizeof(INT_T);
	#ifdef NAIVE
		csr->format_name = (char *) "Naive_CSR_CPU";
	#elif defined(CUSTOM_VECTOR_PERFECT_NNZ_BALANCE)
		csr->format_name = (char *) "Custom_CSR_PBV";
	#else
		#ifdef CUSTOM_VECTOR
			csr->format_name = (char *) "Custom_CSR_BV";
		#else
			csr->format_name = (char *) "Custom_CSR_B";
		#endif
	#endif
	return csr;
}


//==========================================================================================================================================
//= CSR Custom
//==========================================================================================================================================


void compute_csr_custom(CSRArrays * restrict csr, ValueType * restrict x , ValueType * restrict y)
{
	#pragma omp parallel
	{
		int tnum = omp_get_thread_num();
		ValueType sum;
		long i, i_s, i_e, j, j_s, j_e;
		i_s = thread_i_s[tnum];
		i_e = thread_i_e[tnum];

		#ifdef TIME_BARRIER
		double time;
		time = time_it(1,
		#endif

			for (i=i_s;i<i_e;i++)
			{
				j_s = csr->ia[i];
				j_e = csr->ia[i+1];
				if (j_s == j_e)
					continue;
				sum = 0;
				for (j=j_s;j<j_e;j++)
					sum += csr->a[j] * x[csr->ja[j]];
				y[i] = sum;
			}

		#ifdef TIME_BARRIER
		);
		thread_time_compute[tnum] += time;
		time = time_it(1,
			_Pragma("omp barrier")
		);
		thread_time_barrier[tnum] += time;
		#endif
	}
}


//==========================================================================================================================================
//= CSR Custom Vector Omp Prefetch
//==========================================================================================================================================


// prefetch distance for wikipedia-20051105.mtx on ryzen 3700x is optimized at 64 (!) with locality=3, for about +14% gflops.

static int prefetch_distance = 8;

void compute_csr_custom_omp_prefetch(CSRArrays * restrict csr, ValueType * restrict x , ValueType * restrict y)
{
	#pragma omp parallel
	{
		int tnum = omp_get_thread_num();
		ValueType sum;
		long i, i_s, i_e, j, j_s, j_e;
		i_s = thread_i_s[tnum];
		i_e = thread_i_e[tnum];
		for (i=i_s;i<i_e;i++)
		{
			j_s = csr->ia[i];
			j_e = csr->ia[i+1];
			if (j_s == j_e)
				continue;
			sum = 0;
			for (j=j_s;j<j_e;j++)
			{
				__builtin_prefetch(&csr->ja[j + prefetch_distance], 0, 3);
				__builtin_prefetch(&x[csr->ja[j + 2*prefetch_distance]], 0, 3);
				sum += csr->a[j] * x[csr->ja[j]];
			}
			y[i] = sum;
		}
	}
}


//==========================================================================================================================================
//= CSR Custom Vector Omp Simd
//==========================================================================================================================================


void compute_csr_custom_omp_simd(CSRArrays * restrict csr, ValueType * restrict x , ValueType * restrict y)
{
	#pragma omp parallel
	{
		int tnum = omp_get_thread_num();
		ValueType sum;
		long i, i_s, i_e, j, j_s, j_e;
		i_s = thread_i_s[tnum];
		i_e = thread_i_e[tnum];
		for (i=i_s;i<i_e;i++)
		{
			j_s = csr->ia[i];
			j_e = csr->ia[i+1];
			if (j_s == j_e)
				continue;
			sum = 0;
			#pragma omp simd reduction(+:sum)
			for (j=j_s;j<j_e;j++)
				sum += csr->a[j] * x[csr->ja[j]];
			y[i] = sum;
		}
	}
}


//==========================================================================================================================================
//= CSR Custom Vector x86
//==========================================================================================================================================


#include "immintrin.h"

// reduce add 4 double-precision numbers
__attribute__((const))
inline
double
hsum_avx(__m256d in256d)
{
	// double sum;
	// __m256d hsum = _mm256_add_pd(in256d, _mm256_permute2f128_pd(in256d, in256d, 0x1));
	// _mm_store_sd(&sum, _mm_hadd_pd( _mm256_castpd256_pd128(hsum), _mm256_castpd256_pd128(hsum) ) );
	// return sum;

	// __m256d temp = _mm256_hadd_pd(in256d, in256d);
	// return ((double*)&temp)[0] + ((double*)&temp)[2];

	__m256d temp = _mm256_hadd_pd(in256d, in256d);
	__m128d sum_high = _mm256_extractf128_pd(temp, 1);
	__m128d result = _mm_add_pd(sum_high, _mm256_castpd256_pd128(temp));
	return ((double*)&result)[0];
}

/* void compute_csr_custom_vector_x86(CSRArrays * restrict csr, ValueType * restrict x , ValueType * restrict y)
{
	#pragma omp parallel
	{
		int tnum = omp_get_thread_num();
		long i, i_s, i_e, j, j_s, j_e, j_s_vector, j_e_vector, k;
		const long mask = ~(((long) 4) - 1); // Minimum number of elements for the vectorized code (power of 2).
		__m256d v_a, v_x, v_sum, v_mul;
		__attribute__((aligned(32))) ValueType v256d[4] = {0};
		__attribute__((aligned(32))) ValueType x256d[4] = {0};
		ValueType sum = 0;
		i_s = thread_i_s[tnum];
		i_e = thread_i_e[tnum];
		i = i_s;
		j = csr->ia[i];
		j_e = csr->ia[i+1];
		// printf("%d: i=%ld, j=%ld\n", tnum, i_s, j);
		y[i] = 0;
		for (k=0;k<3;k++)
		{
			if ((j & 3) == 0)
				break;
			while (j >= j_e)
			{
				i++;
				y[i] = 0;
				j_e = csr->ia[i+1];
			}
			y[i] += csr->a[j] * x[csr->ja[j]];
			j++;
		}
		// printf("%d: i=%ld, j=%ld, %ld\n", tnum, i, j, j&3);
		// v_sum = _mm256_setzero_pd();
		v_sum = _mm256_set_pd(y[i], 0, 0, 0);
		y[i] = 0;
		while (i < i_e)
		{
			v_a = _mm256_load_pd(&csr->a[j]);
			v_x = _mm256_set_pd(x[csr->ja[j]], x[csr->ja[j+1]], x[csr->ja[j+2]], x[csr->ja[j+3]]);
			if (j + 4 <= j_e)
			{
				v_sum = _mm256_fmadd_pd(v_a, v_x, v_sum);
				j += 4;
				continue;
			}
			y[i] = hsum_avx(v_sum);
			_mm256_store_pd(v256d, _mm256_mul_pd(v_a, v_x));
			// _mm256_store_pd(v256d, v_a);
			// _mm256_store_pd(x256d, v_x);
			for (k=0;k<4;k++)
			{
				while (j+k >= j_e)
				{
					i++;
					if (i >= i_e)
						break;
					y[i] = 0;
					j_e = csr->ia[i+1];
				}
				// y[i] += v256d[k] * x256d[k];
				y[i] += v256d[k];
			}
			v_sum = _mm256_set_pd(y[i], 0, 0, 0);
			j += 4;
		}
	}
} */


void compute_csr_custom_vector_x86(CSRArrays * restrict csr, ValueType * restrict x , ValueType * restrict y)
{
	#pragma omp parallel
	{
		int tnum = omp_get_thread_num();
		long i, i_s, i_e, j, j_s, j_e, j_e_vector;
		const long mask = ~(((long) 4) - 1); // Minimum number of elements for the vectorized code (power of 2).
		__m256d v_a, v_x = _mm256_setzero_pd(), v_sum = _mm256_setzero_pd();
		__attribute__((aligned(32))) ValueType v256d[4] = {0};
		__attribute__((aligned(32))) ValueType x256d[4] = {0};
		ValueType sum = 0, sum_v = 0;
		i_s = thread_i_s[tnum];
		i_e = thread_i_e[tnum];
		for (i=i_s;i<i_e;i++)
		{
			y[i] = 0;
			j_s = csr->ia[i];
			j_e = csr->ia[i+1];
			if (j_s == j_e)
				continue;
			v_sum = _mm256_setzero_pd();
			sum = 0;
			sum_v = 0;
			j_e_vector = j_s + ((j_e - j_s) & mask);
			if (j_s != j_e_vector)
			{
				for (j=j_s;j<j_e_vector;j+=4)
				{
					// if ((j & 3) == 0)
						// v_a = _mm256_load_pd(&csr->a[j]);
					// else
					// {
						// v_a = _mm256_load_pd(&csr->a[j]);
						// v_a = _mm256_set_pd(csr->a[j], csr->a[j+1], csr->a[j+2], csr->a[j+3]);
						v256d[0] = csr->a[j];
						v256d[1] = csr->a[j+1];
						v256d[2] = csr->a[j+2];
						v256d[3] = csr->a[j+3];
						v_a = _mm256_load_pd(v256d);
					// }
					x256d[0] = x[csr->ja[j]];
					x256d[1] = x[csr->ja[j+1]];
					x256d[2] = x[csr->ja[j+2]];
					x256d[3] = x[csr->ja[j+3]];
					v_x = _mm256_load_pd(x256d);
					v_sum = _mm256_fmadd_pd(v_a, v_x, v_sum);
				}
				sum_v = hsum_avx(v_sum);
			}
			for (j=j_e_vector;j<j_e;j++)
				sum += csr->a[j] * x[csr->ja[j]];
			y[i] = sum + sum_v;
		}
	}
}


/* void compute_csr_custom_vector_x86_unroll(CSRArrays * csr, ValueType * x , ValueType * y)
{
	#pragma omp parallel
	{
		int tnum = omp_get_thread_num();
		long i, i_s, i_e, j, j_s, j_e, j_e_vector;
		const long mask = ~(((long) 8) - 1); // Minimum number of elements for the vectorized code (power of 2).
		__m256d v_a_1, v_a_2, v_x_1, v_x_2, v_sum_1, v_sum_2;
		ValueType sum = 0, sum_v = 0;
		i_s = thread_i_s[tnum];
		i_e = thread_i_e[tnum];
		for (i=i_s;i<i_e;i++)
		{
			y[i] = 0;
			j_s = csr->ia[i];
			j_e = csr->ia[i+1];
			if (j_s == j_e)
				continue;
			v_sum_1 = _mm256_setzero_pd();
			v_sum_2 = _mm256_setzero_pd();
			sum = 0;
			sum_v = 0;
			j_e_vector = j_s + ((j_e - j_s) & mask);
			if (j_s != j_e_vector)
			{
				for (j=j_s;j<j_e_vector;j+=8)
				{
					v_a_1 = _mm256_load_pd(&csr->a[j]);
					v_x_1 = _mm256_set_pd(x[csr->ja[j + 3]], x[csr->ja[j + 2]], x[csr->ja[j + 1]], x[csr->ja[j]]);
					v_sum_1 = _mm256_fmadd_pd(v_a_1, v_x_1, v_sum_1);
					v_a_2 = _mm256_load_pd(&csr->a[j+4]);
					v_x_2 = _mm256_set_pd(x[csr->ja[j+4 + 3]], x[csr->ja[j+4 + 2]], x[csr->ja[j+4 + 1]], x[csr->ja[j+4]]);
					v_sum_2 = _mm256_fmadd_pd(v_a_2, v_x_2, v_sum_2);
				}
				v_sum_1 = _mm256_add_pd(v_sum_1, v_sum_2);
				sum_v = hsum_avx(v_sum_1);
			}
			for (j=j_e_vector;j<j_e;j++)
				sum += csr->a[j] * x[csr->ja[j]];
			y[i] = sum + sum_v;
		}
	}
} */


/* void compute_csr_custom_vector2(CSRArrays * csr, ValueType * x , ValueType * y)
{
	#pragma omp parallel
	{
		int tnum = omp_get_thread_num();
		long i, i_s, i_e, j, j_s, j_e, k, j_e_vector;
		const long mask = ~(((long) VECTOR_ELEM_NUM) - 1);      // VECTOR_ELEM_NUM is a power of 2.
		Vector4_Value_t zero = {0};
		__attribute__((unused)) Vector4_Value_t v_a, v_x = zero, v_mul = zero, v_sum = zero;
		__attribute__((unused)) ValueType sum = 0;
		i_s = thread_i_s[tnum];
		i_e = thread_i_e[tnum];
		for (i=i_s;i<i_e;i++)
		{
			v_sum = zero;
			y[i] = 0;
			j_s = csr->ia[i];
			j_e = csr->ia[i+1];
			if (j_s == j_e)
				continue;
			v_a = *(Vector4_Value_t *) &csr->a[0];
			j = j_s;
			j_e_vector = j_s + ((j_e - j_s) & mask);
			for (j=j_s;j<j_e_vector;j+=VECTOR_ELEM_NUM)
			{
				v_a = *(Vector4_Value_t *) &csr->a[j];
				PRAGMA(GCC unroll VECTOR_ELEM_NUM)
				PRAGMA(GCC ivdep)
				for (k=0;k<VECTOR_ELEM_NUM;k++)
				{
					v_mul[k] = v_a[k] * x[csr->ja[j+k]];
				}
				v_sum += v_mul;
			}
			for (;j<j_e;j++)
				v_sum[0] += csr->a[j] * x[csr->ja[j]];
			PRAGMA(GCC unroll VECTOR_ELEM_NUM)
			for (j=1;j<VECTOR_ELEM_NUM;j++)
				v_sum[0] += v_sum[j];
			y[i] = v_sum[0];
		}
	}
} */


//==========================================================================================================================================
//= CSR Custom Queues
//==========================================================================================================================================


__attribute__((hot))
static inline
void
compute_csr_custom_x86_queues_vector(CSRArrays * restrict csr, ValueType * restrict x , ValueType * restrict y, int * qv_i, int * qv_j_s, int * qv_degree, int n)
{
	long i, j, j_s, j_e, k;
	__m256d v_a, v_x, v_sum;
	__attribute__((aligned(32))) ValueType v256d[4] = {0};
	__attribute__((aligned(32))) ValueType x256d[4] = {0};
	for (k=0;k<n;k++)
	{
		i = qv_i[k];
		j_s = qv_j_s[k];
		j_e = j_s + qv_degree[k];
		v_sum = _mm256_setzero_pd();
		for (j=j_s;j<j_e;j+=4)
		{
			v256d[0] = csr->a[j];
			v256d[1] = csr->a[j+1];
			v256d[2] = csr->a[j+2];
			v256d[3] = csr->a[j+3];
			v_a = _mm256_load_pd(v256d);
			x256d[0] = x[csr->ja[j]];
			x256d[1] = x[csr->ja[j+1]];
			x256d[2] = x[csr->ja[j+2]];
			x256d[3] = x[csr->ja[j+3]];
			v_x = _mm256_load_pd(x256d);
			v_sum = _mm256_fmadd_pd(v_a, v_x, v_sum);
		}
		y[i] = hsum_avx(v_sum);
	}
}


void compute_csr_custom_x86_queues(CSRArrays * restrict csr, ValueType * restrict x , ValueType * restrict y)
{
	#pragma omp parallel
	{
		int tnum = omp_get_thread_num();
		long i, i_s, i_e, j, j_s, j_e, k;
		// const long mask = ~(((long) 4) - 1); // Minimum number of elements for the vectorized code (power of 2).
		const long qv_n = 16;
		__attribute__((aligned(32))) int qv_i[qv_n],  qv_j_s[qv_n], qv_degree[qv_n];
		long iter_v = 0;
		long degree, degree_v, degree_s;
						__m256d v_a, v_x, v_sum;
						__attribute__((aligned(32))) ValueType v256d[4] = {0};
						__attribute__((aligned(32))) ValueType x256d[4] = {0};
		ValueType sum = 0;
		i_s = thread_i_s[tnum];
		i_e = thread_i_e[tnum];
		for (i=i_s;i<i_e;i++)
		{
			y[i] = 0;
			j_s = csr->ia[i];
			j_e = csr->ia[i+1];
			if (j_s == j_e)
				continue;
			degree = j_e - j_s;
			degree_v = degree & (~3);
			degree_s = degree & 3;
			if (degree_v != 0)
			{
				qv_i[iter_v] = i;
				qv_j_s[iter_v] = j_s;
				qv_degree[iter_v] = degree_v;
				iter_v++;
				if (iter_v == qv_n)
				{
					#pragma GCC unroll(qv_n)
					for (k=0;k<qv_n;k++)
					{
						i = qv_i[k];
						v_sum = _mm256_setzero_pd();
						for (j=qv_j_s[k];j<qv_j_s[k]+qv_degree[k];j+=4)
						{
							v256d[0] = csr->a[j];
							v256d[1] = csr->a[j+1];
							v256d[2] = csr->a[j+2];
							v256d[3] = csr->a[j+3];
							v_a = _mm256_load_pd(v256d);
							x256d[0] = x[csr->ja[j]];
							x256d[1] = x[csr->ja[j+1]];
							x256d[2] = x[csr->ja[j+2]];
							x256d[3] = x[csr->ja[j+3]];
							v_x = _mm256_load_pd(x256d);
							v_sum = _mm256_fmadd_pd(v_a, v_x, v_sum);
						}
						y[i] += hsum_avx(v_sum);
					}
					// compute_csr_custom_x86_queues_vector(csr, x, y, qv_i, qv_j_s, qv_degree, qv_n);
					iter_v = 0;
				}
			}
			if (degree_s != 0)
			{
				sum = 0;
				for (j=j_s+degree_v;j<j_e;j++)
					sum += csr->a[j] * x[csr->ja[j]];
				y[i] += sum;
			}
		}
	}
}


//==========================================================================================================================================
//= CSR Custom Vector GCC
//==========================================================================================================================================


void compute_csr_custom_vector(CSRArrays * restrict csr, ValueType * restrict x , ValueType * restrict y)
{
	#pragma omp parallel
	{
		int tnum = omp_get_thread_num();
		long i, i_s, i_e, j, j_s, j_e, k, j_e_vector;
		const long mask = ~(((long) VECTOR_ELEM_NUM) - 1);      // VECTOR_ELEM_NUM is a power of 2.
		// const long mask = ~(((long) 4) - 1); // Minimum number of elements for the vectorized code (power of 2).
		Vector4_Value_t zero = {0};
		__attribute__((unused)) Vector4_Value_t v_a, v_x = zero, v_mul = zero, v_sum = zero;
		ValueType sum = 0, sum_v = 0;
		i_s = thread_i_s[tnum];
		i_e = thread_i_e[tnum];
		for (i=i_s;i<i_e;i++)
		{
			y[i] = 0;
			j_s = csr->ia[i];
			j_e = csr->ia[i+1];
			if (j_s == j_e)
				continue;
			v_sum = zero;
			sum = 0;
			sum_v = 0;
			j_e_vector = j_s + ((j_e - j_s) & mask);
			if (j_s != j_e_vector)
			{
				for (j=j_s;j<j_e_vector;j+=VECTOR_ELEM_NUM)
				{
					v_a = *(Vector4_Value_t *) &csr->a[j];
					PRAGMA(GCC unroll VECTOR_ELEM_NUM)
					PRAGMA(GCC ivdep)
					for (k=0;k<VECTOR_ELEM_NUM;k++)
					{
						v_mul[k] = v_a[k] * x[csr->ja[j+k]];
					}
					v_sum += v_mul;
				}
				PRAGMA(GCC unroll VECTOR_ELEM_NUM)
				for (k=0;k<VECTOR_ELEM_NUM;k++)
					sum_v += v_sum[k];
			}
			for (j=j_e_vector;j<j_e;j++)
				sum += csr->a[j] * x[csr->ja[j]];
			y[i] = sum + sum_v;
		}
	}
}


//==========================================================================================================================================
//= CSR Custom Perfect NNZ Balance
//==========================================================================================================================================


__attribute__((hot,pure))
static inline
double
compute_csr_custom_line_vector(CSRArrays * csr, ValueType * x, INT_T j_s, INT_T j_e)
{
	long j, k, j_rem, rows;
	Vector4_Value_t zero = {0};
	__attribute__((unused)) Vector4_Value_t v_a, v_x, v_mul, v_sum;
	ValueType sum = 0;

	rows = j_e - j_s;
	if (rows <= 0)
		return 0;
	sum = 0;
	j_rem = j_s + rows % VECTOR_ELEM_NUM;
	for (j=j_s;j<j_rem;j++)
		sum += csr->a[j] * x[csr->ja[j]];
	if (rows >= VECTOR_ELEM_NUM)
	{
		v_sum = zero;
		v_mul = zero;
		for (j=j_rem;j<j_e;j+=VECTOR_ELEM_NUM)
		{
			v_a = *(Vector4_Value_t *) &csr->a[j];
			PRAGMA(GCC unroll VECTOR_ELEM_NUM)
			PRAGMA(GCC ivdep)
			for (k=0;k<VECTOR_ELEM_NUM;k++)
			{
				v_mul[k] = v_a[k] * x[csr->ja[j+k]];
			}
			v_sum += v_mul;
		}
		PRAGMA(GCC unroll VECTOR_ELEM_NUM)
		for (j=0;j<VECTOR_ELEM_NUM;j++)
			sum += v_sum[j];
	}
	return sum;
}


__attribute__((hot,pure))
static inline
double
compute_csr_custom_line_case_default(CSRArrays * csr, ValueType * x, INT_T j_s, INT_T j_e)
{
	long j;
	__m256d v_a, v_x = _mm256_setzero_pd(), v_sum = _mm256_setzero_pd();
	ValueType sum = 0;
	ValueType x256d0, x256d1, x256d2, x256d3;
	v_sum = _mm256_setzero_pd();
	sum = 0;
	for (j=j_s;j<j_e;j+=4)
	{
		v_a = _mm256_load_pd(&csr->a[j]);
		x256d0 = x[csr->ja[j]];
		x256d1 = x[csr->ja[j + 1]];
		x256d2 = x[csr->ja[j + 2]];
		x256d3 = x[csr->ja[j + 3]];
		v_x = _mm256_set_pd(x256d3, x256d2, x256d1, x256d0);
		v_sum = _mm256_fmadd_pd(v_a, v_x, v_sum);
	}
	sum = hsum_avx(v_sum);
	return sum;
}


// __attribute__((hot,pure))
// static inline
// double
// compute_csr_custom_8(CSRArrays * csr, ValueType * x, INT_T j)
// {
	// Vector4_Value_t * v_a = (Vector4_Value_t *) &csr->a[j];
	// INT_T * ja = &csr->ja[j];
	// Vector4_Value_t v_x1, v_x2;
	// Vector4_Value_t v_sum;
	// Vector4_Value_t tmp1, tmp2;
	// v_x1[0] = x[ja[0]];
	// v_x1[1] = x[ja[1]];
	// v_x1[2] = x[ja[2]];
	// v_x1[3] = x[ja[3]];
	// tmp1 = v_a[0] * v_x1;
	// v_x2[0] = x[ja[4]];
	// v_x2[1] = x[ja[5]];
	// v_x2[2] = x[ja[6]];
	// v_x2[3] = x[ja[7]];
	// tmp2 = v_a[1] * v_x2;
	// v_sum = tmp1 + tmp2;
	// return v_sum[0] + v_sum[1] + v_sum[2] + v_sum[3];
// }

__attribute__((hot,pure))
static inline
double
compute_csr_custom_7(CSRArrays * csr, ValueType * x, INT_T j)
{
	ValueType * a = &csr->a[j];
	INT_T * ja = &csr->ja[j];
	return a[0]*x[ja[0]] + a[1]*x[ja[1]] + a[2]*x[ja[2]] + a[3]*x[ja[3]] + a[4]*x[ja[4]] + a[5]*x[ja[5]] + a[6]*x[ja[6]];
}

__attribute__((hot,pure))
static inline
double
compute_csr_custom_6(CSRArrays * csr, ValueType * x, INT_T j)
{
	ValueType * a = &csr->a[j];
	INT_T * ja = &csr->ja[j];
	return a[0]*x[ja[0]] + a[1]*x[ja[1]] + a[2]*x[ja[2]] + a[3]*x[ja[3]] + a[4]*x[ja[4]] + a[5]*x[ja[5]];
}

__attribute__((hot,pure))
static inline
double
compute_csr_custom_5(CSRArrays * csr, ValueType * x, INT_T j)
{
	ValueType * a = &csr->a[j];
	INT_T * ja = &csr->ja[j];
	return a[0]*x[ja[0]] + a[1]*x[ja[1]] + a[2]*x[ja[2]] + a[3]*x[ja[3]] + a[4]*x[ja[4]];
}

__attribute__((hot,pure))
static inline
double
compute_csr_custom_4(CSRArrays * csr, ValueType * x, INT_T j)
{
	ValueType * a = &csr->a[j];
	INT_T * ja = &csr->ja[j];
	return a[0]*x[ja[0]] + a[1]*x[ja[1]] + a[2]*x[ja[2]] + a[3]*x[ja[3]];
}

__attribute__((hot,pure))
static inline
double
compute_csr_custom_3(CSRArrays * csr, ValueType * x, INT_T j)
{
	ValueType * a = &csr->a[j];
	INT_T * ja = &csr->ja[j];
	return a[0]*x[ja[0]] + a[1]*x[ja[1]] + a[2]*x[ja[2]];
}

__attribute__((hot,pure))
static inline
double
compute_csr_custom_2(CSRArrays * csr, ValueType * x, INT_T j)
{
	ValueType * a = &csr->a[j];
	INT_T * ja = &csr->ja[j];
	return a[0]*x[ja[0]] + a[1]*x[ja[1]];
}

__attribute__((hot,pure))
static inline
double
compute_csr_custom_1(CSRArrays * csr, ValueType * x, INT_T j)
{
	ValueType * a = &csr->a[j];
	INT_T * ja = &csr->ja[j];
	return a[0]*x[ja[0]];
}

__attribute__((hot,pure))
static inline
double
compute_csr_custom_line_case(CSRArrays * csr, ValueType * x, INT_T j_s, INT_T j_e)
{
	__label__ OUT_LABEL, CASE0_LABEL, CASE1_LABEL, CASE2_LABEL, CASE3_LABEL
		// , CASE4_LABEL, CASE5_LABEL, CASE6_LABEL, CASE7_LABEL, CASE8_LABEL
		;
	static const void * jump_table[9] = {
		[0] = &&CASE0_LABEL,
		[1] = &&CASE1_LABEL,
		[2] = &&CASE2_LABEL,
		[3] = &&CASE3_LABEL,
		// [4] = &&CASE4_LABEL,
		// [5] = &&CASE5_LABEL,
		// [6] = &&CASE6_LABEL,
		// [7] = &&CASE7_LABEL,
		// [8] = &&CASE8_LABEL,
	};
	const long mask = ~(((long) 4) - 1); // Minimum number of elements for the vectorized code (power of 2).
	ValueType sum, sum_v;
	long j_e_vector;

	sum_v = 0;
	j_e_vector = j_s + ((j_e - j_s) & mask);
	if (__builtin_expect((j_s != j_e_vector),0))
		sum_v = compute_csr_custom_line_case_default(csr, x, j_s, j_e_vector);

	sum = 0;
	goto *jump_table[j_e - j_e_vector];

	CASE0_LABEL: sum = 0; goto OUT_LABEL;
	CASE1_LABEL: sum = compute_csr_custom_1(csr, x, j_e_vector); goto OUT_LABEL;
	CASE2_LABEL: sum = compute_csr_custom_2(csr, x, j_e_vector); goto OUT_LABEL;
	CASE3_LABEL: sum = compute_csr_custom_3(csr, x, j_e_vector); goto OUT_LABEL;
	// CASE4_LABEL: sum = compute_csr_custom_4(csr, x, j_e_vector); goto OUT_LABEL;
	// CASE5_LABEL: sum = compute_csr_custom_5(csr, x, j_e_vector); goto OUT_LABEL;
	// CASE6_LABEL: sum = compute_csr_custom_6(csr, x, j_e_vector); goto OUT_LABEL;
	// CASE7_LABEL: sum = compute_csr_custom_7(csr, x, j_e_vector); goto OUT_LABEL;
	// CASE8_LABEL: sum = compute_csr_custom_8(csr, x, j_e_vector); goto OUT_LABEL;
	OUT_LABEL:

	// error("csr line");
	return sum + sum_v;
}


__attribute__((hot,pure))
static inline
double
compute_csr_custom_line(CSRArrays * csr, ValueType * x, INT_T j_s, INT_T j_e)
{
	return compute_csr_custom_line_case(csr, x, j_s, j_e);
	// return compute_csr_custom_line_vector(csr, x, j_s, j_e);
}


void
compute_csr_custom_perfect_nnz_balance(CSRArrays * restrict csr, ValueType * restrict x , ValueType * restrict y)
{
	int num_threads = omp_get_max_threads();
	long t;
	#pragma omp parallel
	{
		int tnum = omp_get_thread_num();
		long i, i_s, i_e, j_s, j_e;
		thread_v_s[tnum] = 0;
		thread_v_e[tnum] = 0;

		i_s = thread_i_s[tnum];
		i_e = thread_i_e[tnum];

		i = i_s;
		j_s = csr->ia[i];
		j_e = csr->ia[i+1];
		if (thread_j_s[tnum] > j_s)
			j_s = thread_j_s[tnum];
		if (thread_j_e[tnum] < j_e)
			j_e = thread_j_e[tnum];
		y[i] = 0;
		if (j_s < j_e)
		{
			// printf("%d, %ld, %ld\n", tnum, j_s, j_e);
			thread_v_s[tnum] = compute_csr_custom_line(csr, x, j_s, j_e);
		}

		PRAGMA(GCC ivdep)
		for (i=i_s+1;i<i_e-1;i++)
		{
			j_s = csr->ia[i];
			j_e = csr->ia[i+1];
			y[i] = compute_csr_custom_line(csr, x, j_s, j_e);
		}

		i = i_e-1;
		if (i > i_s)
		{
			j_s = csr->ia[i];
			j_e = csr->ia[i+1];
			if (thread_j_e[tnum] < j_e)
				j_e = thread_j_e[tnum];
			y[i] = 0;
			if (j_s < j_e)
				thread_v_e[tnum] = compute_csr_custom_line(csr, x, j_s, j_e);
		}
	}
	for (t=0;t<num_threads;t++)
	{
		y[thread_i_s[t]] += thread_v_s[t];
		if (thread_i_e[t] - 1 > 0)
			y[thread_i_e[t] - 1] += thread_v_e[t];
	}
}

