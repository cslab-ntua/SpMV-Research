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
#ifdef __cplusplus
}
#endif


extern INT_T * thread_i_s;
extern INT_T * thread_i_e;

extern INT_T * thread_j_s;
extern INT_T * thread_j_e;

extern ValueType * thread_v_s;
extern ValueType * thread_v_e;

extern int prefetch_distance;


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
				#if defined(PROC_BENCH)
					loop_partitioner_balance_iterations(num_threads, tnum, 0, m, &thread_i_s[tnum], &thread_i_e[tnum]);
				#else
					loop_partitioner_balance_partial_sums(num_threads, tnum, ia, m, nnz, &thread_i_s[tnum], &thread_i_e[tnum]);
					// loop_partitioner_balance(num_threads, tnum, 2, ia, m, nnz, &thread_i_s[tnum], &thread_i_e[tnum]);
				#endif
				#ifdef CUSTOM_X86_VECTOR_PERFECT_NNZ_BALANCE
					loop_partitioner_balance_iterations(num_threads, tnum, 0, nnz, &thread_j_s[tnum], &thread_j_e[tnum]);
				#endif
			}
			#ifdef CUSTOM_X86_VECTOR_PERFECT_NNZ_BALANCE
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


void compute_csr_vector_x86(CSRArrays * restrict csr, ValueType * restrict x , ValueType * restrict y);
void compute_csr_vector_x86_queues(CSRArrays * restrict csr, ValueType * restrict x , ValueType * restrict y);
void compute_csr_vector_x86_perfect_nnz_balance(CSRArrays * restrict csr, ValueType * restrict x , ValueType * restrict y);


void
CSRArrays::spmv(ValueType * x, ValueType * y)
{
	#if defined(CUSTOM_X86_VECTOR)
		compute_csr_vector_x86(this, x, y);
	#elif defined(CUSTOM_X86_VECTOR_QUEUES)
		compute_csr_vector_x86_queues(this, x, y);
	#elif defined(CUSTOM_X86_VECTOR_PERFECT_NNZ_BALANCE)
		compute_csr_vector_x86_perfect_nnz_balance(this, x, y);
	#endif
}


struct Matrix_Format *
csr_to_format(INT_T * row_ptr, INT_T * col_ind, ValueType * values, long m, long n, long nnz)
{
	struct CSRArrays * csr = new CSRArrays(row_ptr, col_ind, values, m, n, nnz);
	// for (long i=0;i<10;i++)
		// printf("%d\n", row_ptr[i]);
	csr->mem_footprint = nnz * (sizeof(ValueType) + sizeof(INT_T)) + (m+1) * sizeof(INT_T);
	#if defined(CUSTOM_X86_VECTOR)
		csr->format_name = (char *) "Custom_CSR_BV_x86";
	#elif defined(CUSTOM_X86_VECTOR_PERFECT_NNZ_BALANCE)
		csr->format_name = (char *) "Custom_CSR_PBV_x86";
	#elif defined(CUSTOM_X86_VECTOR_QUEUES)
		csr->format_name = (char *) "Custom_CSR_BV_x86_QUEUES";
	#endif
	return csr;
}


//==========================================================================================================================================
//= CSR Vector x86
//==========================================================================================================================================


#include <immintrin.h>


/*
	__m256i start256i, stop256i, mask256i;
	__m128i v_colind;
	v_colind = _mm_loadu_si128((__m128i const*)&csr->ja[j]);
	v_x = _mm256_set_pd(x[_mm_extract_epi32(v_colind,0)], x[_mm_extract_epi32(v_colind,1)], x[_mm_extract_epi32(v_colind,2)], x[_mm_extract_epi32(v_colind,3)]);
						
	v_sum2 = _mm256_setzero_pd();
	for (j=j_e_vector,k=0;j<j_e;j++,k++)
		v_sum_2[k] = csr->a[j] * x[csr->ja[j]];
	__m256d temp = _mm256_hadd_pd(v_sum, v_sum_2);
	__m128d sum_high = _mm256_extractf128_pd(temp, 1);
	__m128d result = _mm_add_pd(sum_high, _mm256_castpd256_pd128(temp));
	y[i] = hsum256_pd(v_sum);

	x256d[0] = x[csr->ja[j_e_vector]];
	x256d[1] = x[csr->ja[j_e_vector+1]];
	x256d[2] = x[csr->ja[j_e_vector+2]];
	x256d[3] = 0;
	v_x = _mm256_load_pd(x256d);
	start256i = _mm256_set1_epi64x(j_e_vector);
	stop256i = _mm256_set1_epi64x(j_e);
	start256i = _mm256_add_epi64(start256i, _mm256_set_epi64x(0, 1, 2, 3));
	mask256i = _mm256_cmpgt_epi64(stop256i, start256i);
	v_a = _mm256_maskload_pd(&csr->a[j], mask256i);
	v_sum = _mm256_fmadd_pd(v_a, v_x, v_sum);
	y[i] = hsum256_pd(v_sum);
*/


// Reduce add 2 double-precision numbers.
__attribute__((const))
inline
double
hsum128_pd(__m128d v_128d)
{
	__m128d high64 = _mm_unpackhi_pd(v_128d, v_128d);
	return  _mm_cvtsd_f64(_mm_add_sd(v_128d, high64));
}


// Reduce add 4 double-precision numbers.
__attribute__((const))
inline
double
hsum256_pd(__m256d v_256d)
{
	// double sum;
	// __m256d hsum = _mm256_add_pd(v_256d, _mm256_permute2f128_pd(v_256d, v_256d, 0x1));
	// _mm_store_sd(&sum, _mm_hadd_pd( _mm256_castpd256_pd128(hsum), _mm256_castpd256_pd128(hsum) ) );
	// return sum;

	// __m256d temp = _mm256_hadd_pd(v_256d, v_256d);
	// return ((double*)&temp)[0] + ((double*)&temp)[2];

	// __m256d temp = _mm256_hadd_pd(v_256d, v_256d);
	// __m128d sum_high = _mm256_extractf128_pd(temp, 1);
	// __m128d result = _mm_add_pd(sum_high, _mm256_castpd256_pd128(temp));
	// return ((double*)&result)[0];

	__m128d low_128d  = _mm256_castpd256_pd128(v_256d);   // Cast vector of type __m256d to type __m128d. This intrinsic is only used for compilation and does not generate any instructions, thus it has zero latency.
	__m128d high_128d = _mm256_extractf128_pd(v_256d, 1); // High 128: Extract 128 bits (composed of 2 packed double-precision (64-bit) floating-point elements) from a, selected with imm8, and store the result in dst.
	low_128d  = _mm_add_pd(low_128d, high_128d);          // Add low 128 and high 128.
	__m128d high64 = _mm_unpackhi_pd(low_128d, low_128d); // High 64: Unpack and interleave double-precision (64-bit) floating-point elements from the high half of a and b, and store the results in dst.
	return  _mm_cvtsd_f64(_mm_add_sd(low_128d, high64));  // Reduce to scalar.
}


// Reduce add 8 double-precision numbers.
__attribute__((const))
inline
double
hsum512_pd(__m512d v_512d)
{
	// __m256d low  = _mm512_castpd512_pd256(v_512d);
	// __m256d high = _mm512_extractf64x4_pd(v_512d, 1);
	// low  = _mm256_add_pd(low, high);                  // Add low 256 and high 256.
	// return hsum256_pd(low);
	return _mm512_reduce_add_pd(v_512d);
}


void
compute_csr_vector_x86_scalar(CSRArrays * restrict csr, ValueType * restrict x, ValueType * restrict y, long i_s, long i_e)
{
	ValueType sum;
	long i, j, j_s, j_e;
	for (i=i_s;i<i_e;i++)
	{
		j_s = csr->ia[i];
		j_e = csr->ia[i+1];
		if (j_s == j_e)
			continue;
		sum = 0;
		for (j=j_s;j<j_e;j++)
		{
			sum += csr->a[j] * x[csr->ja[j]];
		}
		y[i] = sum;
	}
}


void
compute_csr_vector_x86_128d(CSRArrays * restrict csr, ValueType * restrict x, ValueType * restrict y, long i_s, long i_e)
{
	#ifdef __AVX__
	long i, j, j_s, j_e, j_e_vector;
	const long mask = ~(((long) 2) - 1); // Minimum number of elements for the vectorized code (power of 2).
	__m128d v_a, v_x, v_sum;
	// __attribute__((aligned(32))) ValueType x128d[4] = {0};
	ValueType sum_v = 0;
	for (i=i_s;i<i_e;i++)
	{
		y[i] = 0;
		j_s = csr->ia[i];
		j_e = csr->ia[i+1];
		if (j_s == j_e)
			continue;
		v_sum = _mm_setzero_pd();
		sum_v = 0;
		j_e_vector = j_s + ((j_e - j_s) & mask);
		if (j_s != j_e_vector)
		{
			for (j=j_s;j<j_e_vector;j+=2)
			{
				v_a = _mm_loadu_pd(&csr->a[j]);   // unaligned load
				// x128d[0] = x[csr->ja[j]];
				// x128d[1] = x[csr->ja[j+1]];
				// v_x = _mm_load_pd(x128d);
				v_x = _mm_set_pd(x[csr->ja[j]], x[csr->ja[j+1]]);
				v_sum = _mm_fmadd_pd(v_a, v_x, v_sum);
			}
			sum_v = hsum128_pd(v_sum);
		}
		y[i] = sum_v;
		if (j_e != j_e_vector)
			y[i] += csr->a[j_e_vector] * x[csr->ja[j_e_vector]];
	}
	#else
		compute_csr_vector_x86_scalar(csr, x, y, i_s, i_e);
	#endif
}


void
compute_csr_vector_x86_256d(CSRArrays * restrict csr, ValueType * restrict x, ValueType * restrict y, long i_s, long i_e)
{
	#ifdef __AVX2__
	long i, j, j_s, j_e, j_e_vector;
	const long mask = ~(((long) 4) - 1); // Minimum number of elements for the vectorized code (power of 2).
	__m256d v_a, v_x, v_sum;
	ValueType sum = 0, sum_v = 0;
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
				v_a = _mm256_loadu_pd(&csr->a[j]);   // unaligned load
				v_x = _mm256_set_pd(x[csr->ja[j]], x[csr->ja[j+1]], x[csr->ja[j+2]], x[csr->ja[j+3]]);
				v_sum = _mm256_fmadd_pd(v_a, v_x, v_sum);
			}
			sum_v = hsum256_pd(v_sum);
		}
		for (j=j_e_vector;j<j_e;j++)
			sum += csr->a[j] * x[csr->ja[j]];
		y[i] = sum + sum_v;
	}
	#elif defined(__AVX__)
		compute_csr_vector_x86_128d(csr, x, y, i_s, i_e);
	#else
		compute_csr_vector_x86_scalar(csr, x, y, i_s, i_e);
	#endif
}


void
compute_csr_vector_x86_512d(CSRArrays * restrict csr, ValueType * restrict x, ValueType * restrict y, long i_s, long i_e)
{
	#ifdef __AVX512f__
	long i, j, j_s, j_e, j_e_vector;
	const long mask = ~(((long) 8) - 1); // Minimum number of elements for the vectorized code (power of 2).
	__m512d v_a, v_x, v_sum;
	ValueType sum = 0, sum_v = 0;
	for (i=i_s;i<i_e;i++)
	{
		y[i] = 0;
		j_s = csr->ia[i];
		j_e = csr->ia[i+1];
		if (j_s == j_e)
			continue;
		v_sum = _mm512_setzero_pd();
		sum = 0;
		sum_v = 0;
		j_e_vector = j_s + ((j_e - j_s) & mask);
		if (j_s != j_e_vector)
		{
			for (j=j_s;j<j_e_vector;j+=4)
			{
				v_a = _mm512_loadu_pd(&csr->a[j]);   // unaligned load
				v_x = _mm512_set_pd(x[csr->ja[j]], x[csr->ja[j+1]], x[csr->ja[j+2]], x[csr->ja[j+3]]);
				v_sum = _mm512_fmadd_pd(v_a, v_x, v_sum);
			}
			sum_v = hsum512_pd(v_sum);
		}
		for (j=j_e_vector;j<j_e;j++)
			sum += csr->a[j] * x[csr->ja[j]];
		y[i] = sum + sum_v;
	}
	#elif defined(__AVX2__)
		compute_csr_vector_x86_256d(csr, x, y, i_s, i_e);
	#elif defined(__AVX__)
		compute_csr_vector_x86_128d(csr, x, y, i_s, i_e);
	#else
		compute_csr_vector_x86_scalar(csr, x, y, i_s, i_e);
	#endif
}


void
compute_csr_vector_x86(CSRArrays * restrict csr, ValueType * restrict x, ValueType * restrict y)
{
	#pragma omp parallel
	{
		int tnum = omp_get_thread_num();
		long i_s, i_e;
		double density;
		i_s = thread_i_s[tnum];
		i_e = thread_i_e[tnum];
		if (i_s != i_e)
		{
			density = ((double) csr->ia[i_e] - csr->ia[i_s]) / (i_e - i_s);
			if (density < 4)
			{
				// printf("%d: scalar %lf\n", tnum, density);
				compute_csr_vector_x86_scalar(csr, x, y, i_s, i_e);
			}
			else if (density < 8)
			{
				// printf("%d: 128 %lf\n", tnum, density);
				compute_csr_vector_x86_128d(csr, x, y, i_s, i_e);
			}
			else if (density < 16)
			{
				// printf("%d: 256 %lf\n", tnum, density);
				compute_csr_vector_x86_256d(csr, x, y, i_s, i_e);
			}
			else
			{
				// printf("%d: 512 %lf\n", tnum, density);
				compute_csr_vector_x86_512d(csr, x, y, i_s, i_e);
			}
		}
	}
}


/* void compute_csr_vector_x86_unroll(CSRArrays * csr, ValueType * x , ValueType * y)
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
				sum_v = hsum256_pd(v_sum_1);
			}
			for (j=j_e_vector;j<j_e;j++)
				sum += csr->a[j] * x[csr->ja[j]];
			y[i] = sum + sum_v;
		}
	}
} */


//==========================================================================================================================================
//= CSR Queues
//==========================================================================================================================================


__attribute__((hot))
static inline
void
compute_csr_x86_queues_vector(CSRArrays * restrict csr, ValueType * restrict x, ValueType * restrict y, int * qv_i, int * qv_j_s, int * qv_degree, int n)
{
	long i, j, j_s, j_e, k;
	__m256d v_a, v_x, v_sum;
	__attribute__((aligned(32))) ValueType x256d[4] = {0};
	for (k=0;k<n;k++)
	{
		i = qv_i[k];
		j_s = qv_j_s[k];
		j_e = j_s + qv_degree[k];
		v_sum = _mm256_setzero_pd();
		for (j=j_s;j<j_e;j+=4)
		{
			v_a = _mm256_loadu_pd(&csr->a[j]);
			x256d[0] = x[csr->ja[j]];
			x256d[1] = x[csr->ja[j+1]];
			x256d[2] = x[csr->ja[j+2]];
			x256d[3] = x[csr->ja[j+3]];
			v_x = _mm256_load_pd(x256d);
			v_sum = _mm256_fmadd_pd(v_a, v_x, v_sum);
		}
		y[i] = hsum256_pd(v_sum);
	}
}


void
compute_csr_vector_x86_queues(CSRArrays * restrict csr, ValueType * restrict x, ValueType * restrict y)
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
							v_a = _mm256_loadu_pd(&csr->a[j]);
							x256d[0] = x[csr->ja[j]];
							x256d[1] = x[csr->ja[j+1]];
							x256d[2] = x[csr->ja[j+2]];
							x256d[3] = x[csr->ja[j+3]];
							v_x = _mm256_load_pd(x256d);
							v_sum = _mm256_fmadd_pd(v_a, v_x, v_sum);
						}
						y[i] += hsum256_pd(v_sum);
					}
					// compute_csr_x86_queues_vector(csr, x, y, qv_i, qv_j_s, qv_degree, qv_n);
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
//= CSR Perfect NNZ Balance
//==========================================================================================================================================


__attribute__((hot,pure))
static inline
double
compute_csr_line_vector(CSRArrays * restrict csr, ValueType * restrict x, INT_T j_s, INT_T j_e)
{
	long j, k, j_rem, rows;
	Vector_Value_t zero = {0};
	__attribute__((unused)) Vector_Value_t v_a, v_x, v_mul, v_sum;
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
			v_a = *(Vector_Value_t *) &csr->a[j];
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
compute_csr_line_case_default(CSRArrays * csr, ValueType * x, INT_T j_s, INT_T j_e)
{
	long j;
	__m256d v_a, v_x, v_sum;
	ValueType x256d0, x256d1, x256d2, x256d3;
	v_sum = _mm256_setzero_pd();
	for (j=j_s;j<j_e;j+=4)
	{
		v_a = _mm256_loadu_pd(&csr->a[j]);
		x256d0 = x[csr->ja[j]];
		x256d1 = x[csr->ja[j + 1]];
		x256d2 = x[csr->ja[j + 2]];
		x256d3 = x[csr->ja[j + 3]];
		v_x = _mm256_set_pd(x256d3, x256d2, x256d1, x256d0);
		v_sum = _mm256_fmadd_pd(v_a, v_x, v_sum);
	}
	return hsum256_pd(v_sum);
}


// __attribute__((hot,pure))
// static inline
// double
// compute_csr_8(CSRArrays * csr, ValueType * x, INT_T j)
// {
	// Vector_Value_t * v_a = (Vector_Value_t *) &csr->a[j];
	// INT_T * ja = &csr->ja[j];
	// Vector_Value_t v_x1, v_x2;
	// Vector_Value_t v_sum;
	// Vector_Value_t tmp1, tmp2;
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
compute_csr_line_case(CSRArrays * restrict csr, ValueType * restrict x, INT_T j_s, INT_T j_e)
{

	__label__ OUT_LABEL, CASE1_LABEL, CASE2_LABEL, CASE3_LABEL;
	static const void * jump_table[4] = {
		[0] = &&OUT_LABEL,
		[1] = &&CASE1_LABEL,
		[2] = &&CASE2_LABEL,
		[3] = &&CASE3_LABEL,
	};

	const long mask = ~(((long) 4) - 1); // Minimum number of elements for the vectorized code (power of 2).
	ValueType sum, sum_v;
	long j, j_e_vector, degree_rem;
	ValueType * a;
	INT_T * ja;

	sum = 0;
	sum_v = 0;

	j_e_vector = j_s + ((j_e - j_s) & mask);
	degree_rem = j_e - j_e_vector;

	if (j_s != j_e_vector)
		sum_v = compute_csr_line_case_default(csr, x, j_s, j_e_vector);

	for (j=j_e_vector;j<j_e;j++)
		sum += csr->a[j] * x[csr->ja[j]];

	a = &csr->a[j_e_vector];
	ja = &csr->ja[j_e_vector];

	goto *jump_table[degree_rem];
	CASE3_LABEL:
		sum += a[2]*x[ja[2]];
	CASE2_LABEL:
		sum += a[1]*x[ja[1]];
	CASE1_LABEL:
		sum += a[0]*x[ja[0]];
	OUT_LABEL:

	return sum + sum_v;
}


__attribute__((hot,pure))
static inline
double
compute_csr_line(CSRArrays * csr, ValueType * x, INT_T j_s, INT_T j_e)
{
	return compute_csr_line_case(csr, x, j_s, j_e);
	// return compute_csr_line_vector(csr, x, j_s, j_e);
}


void
compute_csr_vector_x86_perfect_nnz_balance(CSRArrays * restrict csr, ValueType * restrict x, ValueType * restrict y)
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
			thread_v_s[tnum] = compute_csr_line(csr, x, j_s, j_e);
		}

		PRAGMA(GCC ivdep)
		for (i=i_s+1;i<i_e-1;i++)
		{
			j_s = csr->ia[i];
			j_e = csr->ia[i+1];
			if (j_s == j_e)
				continue;
			y[i] = compute_csr_line(csr, x, j_s, j_e);
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
				thread_v_e[tnum] = compute_csr_line(csr, x, j_s, j_e);
		}
	}
	for (t=0;t<num_threads;t++)
	{
		y[thread_i_s[t]] += thread_v_s[t];
		if (thread_i_e[t] - 1 > 0)
			y[thread_i_e[t] - 1] += thread_v_e[t];
	}
}

