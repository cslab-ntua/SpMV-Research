#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <omp.h>
#include <math.h>

#include "macros/macrolib.h"
#include "debug.h"
#include "genlib.h"
#include "time_it.h"
#include "parallel_util.h"
#include "omp_functions.h"

#include "csr_gen.h"


//==========================================================================================================================================
//= User Functions Declarations
//==========================================================================================================================================


//==========================================================================================================================================
//= Includes
//==========================================================================================================================================

// Reduce Add
#include "functools/functools_gen_push.h"
#define FUNCTOOLS_GEN_TYPE_1  int
#define FUNCTOOLS_GEN_TYPE_2  int
#define FUNCTOOLS_GEN_SUFFIX  CONCAT(_CSR_GEN_add_i, CSR_GEN_SUFFIX)
#include "functools/functools_gen.c"
__attribute__((pure))
static inline
int
functools_map_fun(int * A, long i)
{
	return A[i];
}
__attribute__((pure))
static inline
int
functools_reduce_fun(int a, int b)
{
	return a + b;
}


// Quicksort
#include "sort/quicksort/quicksort_gen_push.h"
#define QUICKSORT_GEN_TYPE_1  CSR_GEN_TYPE_2
#define QUICKSORT_GEN_TYPE_2  CSR_GEN_TYPE_2
#define QUICKSORT_GEN_TYPE_3  CSR_GEN_TYPE_2
#define QUICKSORT_GEN_SUFFIX  CONCAT(_QS_CSR_GEN, CSR_GEN_SUFFIX)
#include "sort/quicksort/quicksort_gen.c"
static inline
int
quicksort_cmp(CSR_GEN_TYPE_2 a, CSR_GEN_TYPE_2 b, CSR_GEN_TYPE_2 * sorting_keys)
{
	return (sorting_keys[a] > sorting_keys[b]) ? 1 : (sorting_keys[a] < sorting_keys[b]) ? -1 : 0;
}


// Bucketsort
#include "sort/bucketsort/bucketsort_gen_push.h"
#define BUCKETSORT_GEN_TYPE_1  CSR_GEN_TYPE_2
#define BUCKETSORT_GEN_TYPE_2  CSR_GEN_TYPE_2
#define BUCKETSORT_GEN_TYPE_3  CSR_GEN_TYPE_2
#define BUCKETSORT_GEN_TYPE_4  void
#define BUCKETSORT_GEN_SUFFIX  CONCAT(_BS_CSR_GEN, CSR_GEN_SUFFIX)
#include "sort/bucketsort/bucketsort_gen.c"
static inline
CSR_GEN_TYPE_2
bucketsort_find_bucket(CSR_GEN_TYPE_2 * A, long i, __attribute__((unused)) void * unused)
{
	return A[i];
}


/* #undef  csr_ht_key
#define csr_ht_key  CSR_GEN_EXPAND(csr_ht_key)
struct csr_ht_key {
	CSR_GEN_TYPE_2 r;
	CSR_GEN_TYPE_2 c;
};
#include "data_structures/hashtable/hashtable_gen_push.h"
#define HASHTABLE_GEN_VALUE_SAME_AS_KEY  1
#define HASHTABLE_GEN_KEY_IS_REF  0
#define HASHTABLE_GEN_TYPE_1  struct csr_ht_key
#define HASHTABLE_GEN_TYPE_3  int
#define HASHTABLE_GEN_KEY_IS_STRUCT  1
#define HASHTABLE_GEN_SUFFIX  CONCAT(_HT_CSR_GEN, CSR_GEN_SUFFIX)
#include "data_structures/hashtable/hashtable_gen.c"
static inline
int
hashtable_test_equal_keys_basic_type(struct csr_ht_key key_1, struct csr_ht_key key_2)
{
	if ((key_1.r == key_2.r) && (key_1.c == key_2.c))
	{
		// printf("(%d, %d) (%d, %d)\n", key_1.r, key_1.c, key_2.r, key_2.c);
		return 1;
	}
	return 0;
} */


//==========================================================================================================================================
//= Local Defines
//==========================================================================================================================================


#undef  _TYPE_V
#define _TYPE_V  CSR_GEN_EXPAND_TYPE(_TYPE_V)
typedef CSR_GEN_TYPE_1  _TYPE_V;

#undef  _TYPE_I
#define _TYPE_I  CSR_GEN_EXPAND_TYPE(_TYPE_I)
typedef CSR_GEN_TYPE_2  _TYPE_I;


//==========================================================================================================================================
//------------------------------------------------------------------------------------------------------------------------------------------
//-                                                              Templates                                                                 -
//------------------------------------------------------------------------------------------------------------------------------------------
//==========================================================================================================================================


#undef  csr_sort_columns
#define csr_sort_columns  CSR_GEN_EXPAND(csr_sort_columns)
CSR_GEN_FUNCTION_ATTRIBUTES
void
csr_sort_columns(_TYPE_I * row_ptr, _TYPE_I * col_idx, _TYPE_V * values, long m, long n, long nnz)
{
	long num_threads = safe_omp_get_num_threads_external();
	#pragma omp parallel
	{
		long tnum = omp_get_thread_num();
		long i, i_s, i_e, j, j_s, k;
		long degree;
		long pos;
		loop_partitioner_balance_prefix_sums(num_threads, tnum, row_ptr, m, nnz, &i_s, &i_e);
		__attribute__((cleanup(cleanup_free))) _TYPE_I * rev_permutation  = (typeof(rev_permutation)) malloc(n * sizeof(*rev_permutation));
		__attribute__((cleanup(cleanup_free))) _TYPE_I * buf_permutation  = (typeof(buf_permutation)) malloc(n * sizeof(*buf_permutation));
		__attribute__((cleanup(cleanup_free))) _TYPE_I * buf_offsets      = (typeof(buf_offsets)) malloc((n+1)*sizeof(*buf_offsets));
		__attribute__((cleanup(cleanup_free))) _TYPE_I * qsort_partitions = (typeof(qsort_partitions)) malloc(n * sizeof(*qsort_partitions));
		__attribute__((cleanup(cleanup_free))) _TYPE_I * c_buf            = (typeof(c_buf)) malloc(n * sizeof(*c_buf));
		__attribute__((cleanup(cleanup_free))) _TYPE_V * v_buf            = (typeof(v_buf)) malloc(n * sizeof(*v_buf));
		for (i=i_s;i<i_e;i++)
		{
			degree = row_ptr[i+1] - row_ptr[i];
			if (degree == 0)
				continue;
			j_s = row_ptr[i];
			for (k=0;k<degree;k++)
			{
				c_buf[k] = col_idx[j_s + k];
				if (values != NULL)
					v_buf[k] = values[j_s + k];
			}
			if (degree > n/5)
			{
				bucketsort_stable_recalculate_bucket_serial(&col_idx[j_s], degree, n, NULL, buf_permutation, buf_offsets);
				for (k=0;k<degree;k++)
				{
					pos = j_s + buf_permutation[k];
					col_idx[pos] = c_buf[k];
					if (values != NULL)
						values[pos] = v_buf[k];
				}
			}
			else
			{
				for (k=0;k<degree;k++)
					rev_permutation[k] = k;
				quicksort(rev_permutation, degree, &col_idx[j_s], qsort_partitions);
				for (k=0,j=j_s;k<degree;k++,j++)
				{
					pos = rev_permutation[k];
					col_idx[j] = c_buf[pos];
					if (values != NULL)
						values[j] = v_buf[pos];
				}
			}
		}
	}
}


// An implementation from scratch (without bucketsort ...) doesn't seem to be any faster.
#undef  coo_to_csr
#define coo_to_csr  CSR_GEN_EXPAND(coo_to_csr)
CSR_GEN_FUNCTION_ATTRIBUTES
void
coo_to_csr(_TYPE_I * R, _TYPE_I * C, _TYPE_V * V, long m, long n, long nnz, _TYPE_I * row_ptr, _TYPE_I * col_idx, _TYPE_V * values, const int sort_columns, const int transpose)
{
	_TYPE_I * permutation = (typeof(permutation)) malloc(nnz * sizeof(*permutation));
	if (!transpose)
		bucketsort_recalculate_bucket(R, nnz, m, NULL, permutation, row_ptr);
	else
		bucketsort_recalculate_bucket(C, nnz, n, NULL, permutation, row_ptr);
	#pragma omp parallel
	{
		long i;
		long pos;
		#pragma omp for
		for (i=0;i<nnz;i++)
		{
			pos = permutation[i];
			if (!transpose)
				col_idx[pos] = C[i];
			else
				col_idx[pos] = R[i];
			if (V != NULL)
				values[pos] = V[i];
		}
	}
	free(permutation);
	if (sort_columns)
	{
		if (!transpose)
			csr_sort_columns(row_ptr, col_idx, values, m, n, nnz);
		else
			csr_sort_columns(row_ptr, col_idx, values, n, m, nnz);
	}
}


//==========================================================================================================================================
//= Transpose
//==========================================================================================================================================


#undef  csr_transpose_blocking
#define csr_transpose_blocking  CSR_GEN_EXPAND(csr_transpose_blocking)
CSR_GEN_FUNCTION_ATTRIBUTES
void
csr_transpose_blocking(_TYPE_I * row_ptr, _TYPE_I * col_idx, _TYPE_V * values, long m, long n, long nnz, _TYPE_I ** row_ptr_ret, _TYPE_I ** col_idx_ret, _TYPE_V ** values_ret, const int sort_columns)
{
	long num_threads = safe_omp_get_num_threads_external();
	_TYPE_I * row_ptr_new;
	_TYPE_I * col_idx_new;
	_TYPE_V * values_new = NULL;
	if (row_ptr_ret == NULL)
		error("return variable is NULL: row_ptr_ret");
	if (col_idx_ret == NULL)
		error("return variable is NULL: col_idx_ret");
	if (values != NULL && values_ret == NULL)
		error("return variable is NULL: values_ret");
	row_ptr_new = (typeof(row_ptr_new)) malloc((n+1) * sizeof(*row_ptr_new));
	const long bs = 128;
	_Pragma("omp parallel")
	{
		long tnum = omp_get_thread_num();
		long i, ii, ii_e, i_s, i_e, j, col, b, d;
		long degree, degree_max;
		_Pragma("omp for")
		for (i=0;i<=n;i++)
			row_ptr_new[i] = 0;
		loop_partitioner_balance_prefix_sums(num_threads, tnum, row_ptr, m, nnz, &i_s, &i_e);
		long mod = (i_e - i_s) % bs;
		ii_e = i_e - mod;
		for (ii=i_s;ii<ii_e;ii+=bs)
		{
			degree_max = 0;
			for (b=0;b<bs;b++)
			{
				i = ii + b;
				degree = row_ptr[i+1] - row_ptr[i];
				if (degree > degree_max)
					degree_max = degree;
			}
			for (d=0;d<degree_max;d++)
			{
				for (b=0;b<bs;b++)
				{
					i = ii + b;
					j = row_ptr[i] + d;
					if (j < row_ptr[i+1])
					{
						col = col_idx[j];
						__atomic_fetch_add(&row_ptr_new[col], 1, __ATOMIC_RELAXED);
					}
				}
			}
		}
		for (i=ii_e;i<i_e;i++)
		{
			for (j=row_ptr[i];j<row_ptr[i+1];j++)
			{
				col = col_idx[j];
				__atomic_fetch_add(&row_ptr_new[col], 1, __ATOMIC_RELAXED);
			}
		}
	}
	scan_reduce(row_ptr_new, row_ptr_new, n+1, 0, 0, 0);
	col_idx_new = (typeof(col_idx_new)) malloc(nnz * sizeof(*col_idx_new));
	if (values != NULL)
		values_new = (typeof(values_new)) malloc(nnz * sizeof(*values_new));
	else
		values_new = NULL;
	_Pragma("omp parallel")
	{
		long tnum = omp_get_thread_num();
		long i, ii, ii_e, i_s, i_e, j, pos, col, b, d;
		long degree, degree_max;
		loop_partitioner_balance_prefix_sums(num_threads, tnum, row_ptr, m, nnz, &i_s, &i_e);
		long mod = (i_e - i_s) % bs;
		ii_e = i_e - mod;
		for (ii=i_s;ii<ii_e;ii+=bs)
		{
			degree_max = 0;
			for (b=0;b<bs;b++)
			{
				i = ii + b;
				degree = row_ptr[i+1] - row_ptr[i];
				if (degree > degree_max)
					degree_max = degree;
			}
			for (d=0;d<degree_max;d++)
			{
				for (b=0;b<bs;b++)
				{
					i = ii + b;
					j = row_ptr[i] + d;
					if (j < row_ptr[i+1])
					{
						col = col_idx[j];
						pos = __atomic_sub_fetch(&row_ptr_new[col], 1, __ATOMIC_RELAXED);
						col_idx_new[pos] = i;
						if (values != NULL)
							values_new[pos] = values[j];
					}
				}
			}
		}
		for (i=ii_e;i<i_e;i++)
		{
			for (j=row_ptr[i];j<row_ptr[i+1];j++)
			{
				col = col_idx[j];
				pos = __atomic_sub_fetch(&row_ptr_new[col], 1, __ATOMIC_RELAXED);
				col_idx_new[pos] = i;
				if (values != NULL)
					values_new[pos] = values[j];
			}
		}
	}
	if (sort_columns)
	{
		csr_sort_columns(row_ptr_new, col_idx_new, values_new, n, m, nnz);
	}
	*row_ptr_ret = row_ptr_new;
	*col_idx_ret = col_idx_new;
	if (values != NULL)
		*values_ret = values_new;
}


#undef  csr_transpose
#define csr_transpose  CSR_GEN_EXPAND(csr_transpose)
CSR_GEN_FUNCTION_ATTRIBUTES
void
csr_transpose(_TYPE_I * row_ptr, _TYPE_I * col_idx, _TYPE_V * values, long m, long n, long nnz, _TYPE_I ** row_ptr_ret, _TYPE_I ** col_idx_ret, _TYPE_V ** values_ret, const int sort_columns)
{
	long num_threads = safe_omp_get_num_threads_external();
	_TYPE_I * row_ptr_new;
	_TYPE_I * col_idx_new;
	_TYPE_V * values_new = NULL;
	if (row_ptr_ret == NULL)
		error("return variable is NULL: row_ptr_ret");
	if (col_idx_ret == NULL)
		error("return variable is NULL: col_idx_ret");
	if (values != NULL && values_ret == NULL)
		error("return variable is NULL: values_ret");
	row_ptr_new = (typeof(row_ptr_new)) malloc((n+1) * sizeof(*row_ptr_new));
	_Pragma("omp parallel")
	{
		long tnum = omp_get_thread_num();
		long i, i_s, i_e, j, col;
		_Pragma("omp for")
		for (i=0;i<=n;i++)
			row_ptr_new[i] = 0;
		loop_partitioner_balance_prefix_sums(num_threads, tnum, row_ptr, m, nnz, &i_s, &i_e);
		for (i=i_s;i<i_e;i++)
		{
			for (j=row_ptr[i];j<row_ptr[i+1];j++)
			{
				col = col_idx[j];
				__atomic_fetch_add(&row_ptr_new[col], 1, __ATOMIC_RELAXED);
			}
		}
	}
	scan_reduce(row_ptr_new, row_ptr_new, n+1, 0, 0, 0);
	col_idx_new = (typeof(col_idx_new)) malloc(nnz * sizeof(*col_idx_new));
	if (values != NULL)
		values_new = (typeof(values_new)) malloc(nnz * sizeof(*values_new));
	else
		values_new = NULL;
	_Pragma("omp parallel")
	{
		long tnum = omp_get_thread_num();
		long i, i_s, i_e, j, pos, col;
		loop_partitioner_balance_prefix_sums(num_threads, tnum, row_ptr, m, nnz, &i_s, &i_e);
		for (i=i_s;i<i_e;i++)
		{
			for (j=row_ptr[i];j<row_ptr[i+1];j++)
			{
				col = col_idx[j];
				pos = __atomic_sub_fetch(&row_ptr_new[col], 1, __ATOMIC_RELAXED);
				col_idx_new[pos] = i;
				if (values != NULL)
					values_new[pos] = values[j];
			}
		}
	}
	if (sort_columns)
	{
		csr_sort_columns(row_ptr_new, col_idx_new, values_new, n, m, nnz);
	}
	*row_ptr_ret = row_ptr_new;
	*col_idx_ret = col_idx_new;
	if (values != NULL)
		*values_ret = values_new;
}


//==========================================================================================================================================
//= Split To Lower And Strictly Upper Triangular
//==========================================================================================================================================


#undef  csr_split_to_lower_and_strictly_upper_triangular
#define csr_split_to_lower_and_strictly_upper_triangular  CSR_GEN_EXPAND(csr_split_to_lower_and_strictly_upper_triangular)
CSR_GEN_FUNCTION_ATTRIBUTES
void
csr_split_to_lower_and_strictly_upper_triangular(_TYPE_I * row_ptr, _TYPE_I * col_idx, _TYPE_V * values, long m, long n, long nnz,
		_TYPE_I ** row_ptr_upper_ret, _TYPE_I ** col_idx_upper_ret, _TYPE_V ** values_upper_ret, long * nnz_upper_out,
		_TYPE_I ** row_ptr_lower_ret, _TYPE_I ** col_idx_lower_ret, _TYPE_V ** values_lower_ret, long * nnz_lower_out,
		const int sort_columns, const int transpose_upper, const int transpose_lower)
{
	long num_threads = safe_omp_get_num_threads_external();
	_TYPE_I * row_ptr_upper;
	_TYPE_I * col_idx_upper;
	_TYPE_V * values_upper;
	_TYPE_I * row_ptr_lower;
	_TYPE_I * col_idx_lower;
	_TYPE_V * values_lower;
	long nnz_upper;
	long nnz_lower;
	long m_upper = transpose_upper ? n : m;
	long m_lower = transpose_lower ? n : m;
	long n_upper = transpose_upper ? m : n;
	long n_lower = transpose_lower ? m : n;
	if (nnz_upper_out == NULL)
		error("return variable is NULL: nnz_upper_out");
	if (row_ptr_upper_ret == NULL)
		error("return variable is NULL: row_ptr_upper_ret");
	if (col_idx_upper_ret == NULL)
		error("return variable is NULL: col_idx_upper_ret");
	if (values != NULL && values_upper_ret == NULL)
		error("return variable is NULL: values_upper_ret");
	if (nnz_lower_out == NULL)
		error("return variable is NULL: nnz_lower_out");
	if (row_ptr_lower_ret == NULL)
		error("return variable is NULL: row_ptr_lower_ret");
	if (col_idx_lower_ret == NULL)
		error("return variable is NULL: col_idx_lower_ret");
	if (values != NULL && values_lower_ret == NULL)
		error("return variable is NULL: values_lower_ret");
	row_ptr_upper = (typeof(row_ptr_upper)) malloc((m_upper+1) * sizeof(*row_ptr_upper));
	row_ptr_lower = (typeof(row_ptr_lower)) malloc((m_lower+1) * sizeof(*row_ptr_lower));
	_Pragma("omp parallel")
	{
		long tnum = omp_get_thread_num();
		long i, i_s, i_e, j, col;
		_Pragma("omp for")
		for (i=0;i<=m_upper;i++)
			row_ptr_upper[i] = 0;
		_Pragma("omp for")
		for (i=0;i<=m_lower;i++)
			row_ptr_lower[i] = 0;
		loop_partitioner_balance_prefix_sums(num_threads, tnum, row_ptr, m, nnz, &i_s, &i_e);
		for (i=i_s;i<i_e;i++)
		{
			for (j=row_ptr[i];j<row_ptr[i+1];j++)
			{
				col = col_idx[j];
				if (i < col)  // strictly upper
				{
					if (transpose_upper)
					{
						__atomic_fetch_add(&row_ptr_upper[col], 1, __ATOMIC_RELAXED);
					}
					else
					{
						row_ptr_upper[i]++;
					}
				}
				else  // lower
				{
					if (transpose_lower)
					{
						__atomic_fetch_add(&row_ptr_lower[col], 1, __ATOMIC_RELAXED);
					}
					else
					{
						row_ptr_lower[i]++;
					}
				}
			}
		}
	}
	scan_reduce(row_ptr_upper, row_ptr_upper, m_upper+1, 0, 0, 0);
	scan_reduce(row_ptr_lower, row_ptr_lower, m_lower+1, 0, 0, 0);
	nnz_upper = row_ptr_upper[m_upper];
	nnz_lower = row_ptr_lower[m_lower];
	col_idx_upper = (typeof(col_idx_upper)) malloc(nnz_upper * sizeof(*col_idx_upper));
	col_idx_lower = (typeof(col_idx_lower)) malloc(nnz_lower * sizeof(*col_idx_lower));
	if (values != NULL)
	{
		values_upper = (typeof(values_upper)) malloc(nnz_upper * sizeof(*values_upper));
		values_lower = (typeof(values_lower)) malloc(nnz_lower * sizeof(*values_lower));
	}
	else
	{
		values_upper = NULL;
		values_lower = NULL;
	}
	_Pragma("omp parallel")
	{
		long tnum = omp_get_thread_num();
		long i, i_s, i_e, j, col, pos;
		loop_partitioner_balance_prefix_sums(num_threads, tnum, row_ptr, m, nnz, &i_s, &i_e);
		for (i=i_s;i<i_e;i++)
		{
			for (j=row_ptr[i];j<row_ptr[i+1];j++)
			{
				col = col_idx[j];
				if (i < col)  // strictly upper
				{
					if (transpose_upper)
					{
						pos = __atomic_sub_fetch(&row_ptr_upper[col], 1, __ATOMIC_RELAXED);
						col_idx_upper[pos] = i;
					}
					else
					{
						row_ptr_upper[i]--;
						pos = row_ptr_upper[i];
						col_idx_upper[pos] = col;
					}
					if (values != NULL)
						values_upper[pos] = values[j];
				}
				else  // lower
				{
					if (transpose_lower)
					{
						pos = __atomic_sub_fetch(&row_ptr_lower[col], 1, __ATOMIC_RELAXED);
						col_idx_lower[pos] = i;
					}
					else
					{
						row_ptr_lower[i]--;
						pos = row_ptr_lower[i];
						col_idx_lower[pos] = col;
					}
					if (values != NULL)
						values_lower[pos] = values[j];
				}
			}
		}
	}
	if (sort_columns)
	{
		csr_sort_columns(row_ptr_upper, col_idx_upper, values_upper, m_upper, n_upper, nnz_upper);
		csr_sort_columns(row_ptr_lower, col_idx_lower, values_lower, m_lower, n_lower, nnz_lower);
	}
	*row_ptr_upper_ret = row_ptr_upper;
	*col_idx_upper_ret = col_idx_upper;
	if (values != NULL)
		*values_upper_ret = values_upper;
	*nnz_upper_out = nnz_upper;
	*row_ptr_lower_ret = row_ptr_lower;
	*col_idx_lower_ret = col_idx_lower;
	if (values != NULL)
		*values_lower_ret = values_lower;
	*nnz_lower_out = nnz_lower;
}


//==========================================================================================================================================
//= Expand Symmetric Matrix
//==========================================================================================================================================


#undef  csr_expand_symmetric
#define csr_expand_symmetric  CSR_GEN_EXPAND(csr_expand_symmetric)
CSR_GEN_FUNCTION_ATTRIBUTES
void
csr_expand_symmetric(_TYPE_I * row_ptr, _TYPE_I * col_idx, _TYPE_V * values, long m, long n, long nnz, _TYPE_I ** row_ptr_ret, _TYPE_I ** col_idx_ret, _TYPE_V ** values_ret, long * nnz_out, long * nnz_diag_out, const int sort_columns)
{
	long num_threads = safe_omp_get_num_threads_external();
	_TYPE_I * row_ptr_new;
	_TYPE_I * col_idx_new;
	_TYPE_V * values_new = NULL;
	long nnz_diag = 0, nnz_non_diag_half = 0, nnz_new;
	if (m != n)
		error("matrix is not square: m=%ld, n=%ld", m, n);
	if (nnz_out == NULL)
		error("return variable is NULL: nnz_out");
	if (row_ptr_ret == NULL)
		error("return variable is NULL: row_ptr_ret");
	if (col_idx_ret == NULL)
		error("return variable is NULL: col_idx_ret");
	if (values != NULL && values_ret == NULL)
		error("return variable is NULL: values_ret");
	row_ptr_new = (typeof(row_ptr_new)) malloc((m+1) * sizeof(*row_ptr_new));
	_Pragma("omp parallel")
	{
		long tnum = omp_get_thread_num();
		long i, i_s, i_e, j, col;
		long nnz_diag_t = 0;
		_Pragma("omp for")
		for (i=0;i<=m;i++)
			row_ptr_new[i] = row_ptr[i+1] - row_ptr[i];
		_Pragma("omp single")
			row_ptr_new[m] = 0;
		loop_partitioner_balance_prefix_sums(num_threads, tnum, row_ptr, m, nnz, &i_s, &i_e);
		for (i=i_s;i<i_e;i++)
		{
			for (j=row_ptr[i];j<row_ptr[i+1];j++)
			{
				col = col_idx[j];
				if (i < col)
					error("upper nonzero found: row=%d col=%d", i, col);
				if (i == col)
				{
					nnz_diag_t++;
				}
				else
				{
					__atomic_fetch_add(&row_ptr_new[col], 1, __ATOMIC_RELAXED);
				}
			}
		}
		__atomic_fetch_add(&nnz_diag, nnz_diag_t, __ATOMIC_RELAXED);
	}
	nnz_non_diag_half = nnz - nnz_diag;
	nnz_new = nnz_diag + 2*nnz_non_diag_half;
	scan_reduce(row_ptr_new, row_ptr_new, m+1, 0, 0, 0);
	col_idx_new = (typeof(col_idx_new)) malloc(nnz_new * sizeof(*col_idx_new));
	if (values != NULL)
		values_new = (typeof(values_new)) malloc(nnz_new * sizeof(*values_new));
	else
		values_new = NULL;
	_Pragma("omp parallel")
	{
		long tnum = omp_get_thread_num();
		long i, i_s, i_e, j, pos, col;
		loop_partitioner_balance_prefix_sums(num_threads, tnum, row_ptr, m, nnz, &i_s, &i_e);
		for (i=i_s;i<i_e;i++)
		{
			for (j=row_ptr[i];j<row_ptr[i+1];j++)
			{
				col = col_idx[j];
				pos = __atomic_sub_fetch(&row_ptr_new[i], 1, __ATOMIC_RELAXED);
				col_idx_new[pos] = col;
				if (values != NULL)
					values_new[pos] = values[j];
				if (i != col)
				{
					pos = __atomic_sub_fetch(&row_ptr_new[col], 1, __ATOMIC_RELAXED);
					col_idx_new[pos] = i;
					if (values != NULL)
						values_new[pos] = values[j];
				}
			}
		}
	}
	if (sort_columns)
	{
		csr_sort_columns(row_ptr_new, col_idx_new, values_new, m, n, nnz_new);
	}
	*row_ptr_ret = row_ptr_new;
	*col_idx_ret = col_idx_new;
	if (values != NULL)
		*values_ret = values_new;
	*nnz_out = nnz_new;
	if (nnz_diag_out != NULL)
		*nnz_diag_out = nnz_diag;
}


//==========================================================================================================================================
//= Matrix Symmetrization
//==========================================================================================================================================


#undef  csr_symmetrize
#define csr_symmetrize  CSR_GEN_EXPAND(csr_symmetrize)
CSR_GEN_FUNCTION_ATTRIBUTES
void
csr_symmetrize(_TYPE_I * row_ptr, _TYPE_I * col_idx, _TYPE_V * values, long m, long n, long nnz, _TYPE_I ** row_ptr_ret, _TYPE_I ** col_idx_ret, _TYPE_V ** values_ret, long * nnz_out, long * nnz_diag_out,
		__attribute__((unused)) const int sort_columns /* Returned matrix is always sorted due to the construction process. */ )
{
	long num_threads = safe_omp_get_num_threads_external();
	_TYPE_I * row_ptr_new;
	_TYPE_I * col_idx_new;
	_TYPE_V * values_new = NULL;
	long nnz_diag = 0, nnz_new;
	if (m != n)
		error("matrix is not square: m=%ld, n=%ld", m, n);
	if (nnz_out == NULL)
		error("return variable is NULL: nnz_out");
	if (row_ptr_ret == NULL)
		error("return variable is NULL: row_ptr_ret");
	if (col_idx_ret == NULL)
		error("return variable is NULL: col_idx_ret");
	if (values != NULL && values_ret == NULL)
		error("return variable is NULL: values_ret");
	row_ptr_new = (typeof(row_ptr_new)) malloc((m+1) * sizeof(*row_ptr_new));
	_TYPE_I * row_ptr_tr = NULL;
	_TYPE_I * col_idx_tr = NULL;
	_TYPE_V * values_tr = NULL;
	csr_sort_columns(row_ptr, col_idx, values, m, n, nnz);
	csr_transpose(row_ptr, col_idx, values, m, n, nnz, &row_ptr_tr, &col_idx_tr, &values_tr, 1);
	_Pragma("omp parallel")
	{
		long tnum = omp_get_thread_num();
		long i, i_s, i_e, j, j_tr, col, col_tr;
		long nnz_diag_t = 0;
		_Pragma("omp for")
		for (i=0;i<=m;i++)
			row_ptr_new[i] = 0;
		loop_partitioner_balance_prefix_sums(num_threads, tnum, row_ptr, m, nnz, &i_s, &i_e);
		for (i=i_s;i<i_e;i++)
		{
			j = row_ptr[i];
			j_tr = row_ptr_tr[i];
			while (1)
			{
				if ((j >= row_ptr[i+1]) || (j_tr >= row_ptr_tr[i+1]))
					break;
				col = col_idx[j];
				col_tr = col_idx_tr[j_tr];
				if (col < col_tr)
				{
					row_ptr_new[i]++;
					j++;
				}
				else if (col > col_tr)
				{
					row_ptr_new[i]++;
					j_tr++;
				}
				else
				{
					row_ptr_new[i]++;
					j++;
					j_tr++;
					if (i == col)
						nnz_diag_t++;
				}
			}
			row_ptr_new[i] += row_ptr[i+1] - j;
			row_ptr_new[i] += row_ptr_tr[i+1] - j_tr;
		}
		__atomic_fetch_add(&nnz_diag, nnz_diag_t, __ATOMIC_RELAXED);
	}
	scan_reduce(row_ptr_new, row_ptr_new, m+1, 0, 1, 0);
	nnz_new = row_ptr_new[m];
	col_idx_new = (typeof(col_idx_new)) malloc(nnz_new * sizeof(*col_idx_new));
	if (values != NULL)
		values_new = (typeof(values_new)) malloc(nnz_new * sizeof(*values_new));
	else
		values_new = NULL;
	_Pragma("omp parallel")
	{
		long tnum = omp_get_thread_num();
		long i, i_s, i_e, j, j_tr, j_new, col, col_tr;
		loop_partitioner_balance_prefix_sums(num_threads, tnum, row_ptr, m, nnz, &i_s, &i_e);
		for (i=i_s;i<i_e;i++)
		{
			j_new = row_ptr_new[i];
			j = row_ptr[i];
			j_tr = row_ptr_tr[i];
			while (1)
			{
				if ((j >= row_ptr[i+1]) || (j_tr >= row_ptr_tr[i+1]))
					break;
				col = col_idx[j];
				col_tr = col_idx_tr[j_tr];
				if (col < col_tr)
				{
					col_idx_new[j_new] = col_idx[j];
					if (values != NULL)
						values_new[j_new] = values[j];
					j++;
				}
				else if (col > col_tr)
				{
					col_idx_new[j_new] = col_idx_tr[j_tr];
					if (values != NULL)
						values_new[j_new] = values_tr[j];
					j_tr++;
				}
				else
				{
					col_idx_new[j_new] = col_idx[j];
					if (values != NULL)
						values_new[j_new] = values[j];
					j++;
					j_tr++;
				}
				j_new++;
			}
			while (j < row_ptr[i+1])
			{
				col_idx_new[j_new] = col_idx[j];
				if (values != NULL)
					values_new[j_new] = values[j];
				j++;
				j_new++;
			}
			while (j_tr < row_ptr_tr[i+1])
			{
				col_idx_new[j_new] = col_idx_tr[j_tr];
				if (values != NULL)
					values_new[j_new] = values_tr[j];
				j_tr++;
				j_new++;
			}
		}
	}
	// _Pragma("omp parallel")
	// {
		// long tnum = omp_get_thread_num();
		// long i, i_s, i_e, j;
		// loop_partitioner_balance_prefix_sums(num_threads, tnum, row_ptr, m, nnz, &i_s, &i_e);
		// for (i=i_s;i<i_e;i++)
		// {
			// for (j=row_ptr_new[i]+1;j<row_ptr_new[i+1];j++)
			// {
				// if (col_idx_new[j] <= col_idx_new[j-1])
					// error("test");
			// }
		// }
	// }
	free(row_ptr_tr);
	free(col_idx_tr);
	free(values_tr);
	*row_ptr_ret = row_ptr_new;
	*col_idx_ret = col_idx_new;
	if (values != NULL)
		*values_ret = values_new;
	*nnz_out = nnz_new;
	if (nnz_diag_out != NULL)
		*nnz_diag_out = nnz_diag;
}


/* #undef  csr_symmetrize_to_key
#define csr_symmetrize_to_key  CSR_GEN_EXPAND(csr_symmetrize_to_key)
static inline
struct csr_ht_key
csr_symmetrize_to_key(int r, int c)
{
	struct csr_ht_key key = {.r=r, .c=c};
	return key;
}


#undef  csr_symmetrize_dict
#define csr_symmetrize_dict  CSR_GEN_EXPAND(csr_symmetrize_dict)
CSR_GEN_FUNCTION_ATTRIBUTES
void
csr_symmetrize_dict(_TYPE_I * row_ptr, _TYPE_I * col_idx, _TYPE_V * values, long m, long n, long nnz, _TYPE_I ** row_ptr_ret, _TYPE_I ** col_idx_ret, _TYPE_V ** values_ret, long * nnz_out, long * nnz_diag_out, const int sort_columns)
{
	long num_threads = safe_omp_get_num_threads_external();
	_TYPE_I * row_ptr_new;
	_TYPE_I * col_idx_new;
	_TYPE_V * values_new = NULL;
	long nnz_diag = 0, nnz_non_diag = 0, nnz_new;
	__attribute__((cleanup(hashtable_destroy))) struct hashtable * ht = NULL;
	if (m != n)
		error("matrix is not square: m=%ld, n=%ld", m, n);
	if (nnz_out == NULL)
		error("return variable is NULL: nnz_out");
	if (row_ptr_ret == NULL)
		error("return variable is NULL: row_ptr_ret");
	if (col_idx_ret == NULL)
		error("return variable is NULL: col_idx_ret");
	if (values != NULL && values_ret == NULL)
		error("return variable is NULL: values_ret");
	row_ptr_new = (typeof(row_ptr_new)) malloc((m+1) * sizeof(*row_ptr_new));
	ht = hashtable_new(1*nnz);
	_Pragma("omp parallel")
	{
		long tnum = omp_get_thread_num();
		long i, i_s, i_e, j, col;
		long nnz_diag_t = 0, nnz_non_diag_t = 0;
		_Pragma("omp for")
		for (i=0;i<m;i++)
			row_ptr_new[i] = row_ptr[i+1] - row_ptr[i];
		_Pragma("omp single")
			row_ptr_new[m] = 0;
		loop_partitioner_balance_prefix_sums(num_threads, tnum, row_ptr, m, nnz, &i_s, &i_e);
		for (i=i_s;i<i_e;i++)
		{
			for (j=row_ptr[i];j<row_ptr[i+1];j++)
			{
				col = col_idx[j];
				if (i == col)
				{
					nnz_diag_t++;
				}
				else
				{
					nnz_non_diag_t++;
					hashtable_insert_concurrent(ht, csr_symmetrize_to_key(i, col));
				}
			}
		}
		_Pragma("omp barrier")
		for (i=i_s;i<i_e;i++)
		{
			for (j=row_ptr[i];j<row_ptr[i+1];j++)
			{
				col = col_idx[j];
				if ((i != col) && (!hashtable_contains(ht, csr_symmetrize_to_key(col, i))))
				{
					nnz_non_diag_t++;
					__atomic_fetch_add(&row_ptr_new[col], 1, __ATOMIC_RELAXED);
				}
			}
		}
		__atomic_fetch_add(&nnz_diag, nnz_diag_t, __ATOMIC_RELAXED);
		__atomic_fetch_add(&nnz_non_diag, nnz_non_diag_t, __ATOMIC_RELAXED);
	}
	nnz_new = nnz_diag + nnz_non_diag;
	scan_reduce(row_ptr_new, row_ptr_new, m+1, 0, 0, 0);
	col_idx_new = (typeof(col_idx_new)) malloc(nnz_new * sizeof(*col_idx_new));
	if (values != NULL)
		values_new = (typeof(values_new)) malloc(nnz_new * sizeof(*values_new));
	else
		values_new = NULL;
	_Pragma("omp parallel")
	{
		long tnum = omp_get_thread_num();
		long i, i_s, i_e, j, pos, col;
		loop_partitioner_balance_prefix_sums(num_threads, tnum, row_ptr, m, nnz, &i_s, &i_e);
		for (i=i_s;i<i_e;i++)
		{
			for (j=row_ptr[i];j<row_ptr[i+1];j++)
			{
				col = col_idx[j];
				pos = __atomic_sub_fetch(&row_ptr_new[i], 1, __ATOMIC_RELAXED);
				col_idx_new[pos] = col;
				if (values != NULL)
					values_new[pos] = values[j];
				if ((i != col) && (!hashtable_contains(ht, csr_symmetrize_to_key(col, i))))
				{
					pos = __atomic_sub_fetch(&row_ptr_new[col], 1, __ATOMIC_RELAXED);
					col_idx_new[pos] = i;
					if (values != NULL)
						values_new[pos] = values[j];
				}
			}
		}
	}
	if (sort_columns)
	{
		csr_sort_columns(row_ptr_new, col_idx_new, values_new, m, n, nnz_new);
	}
	*row_ptr_ret = row_ptr_new;
	*col_idx_ret = col_idx_new;
	if (values != NULL)
		*values_ret = values_new;
	*nnz_out = nnz_new;
	if (nnz_diag_out != NULL)
		*nnz_diag_out = nnz_diag;
} */


//==========================================================================================================================================
//= Remove Elements Above Diagonal
//==========================================================================================================================================


#undef  csr_drop_upper
#define csr_drop_upper  CSR_GEN_EXPAND(csr_drop_upper)
CSR_GEN_FUNCTION_ATTRIBUTES
void
csr_drop_upper(_TYPE_I * row_ptr, _TYPE_I * col_idx, _TYPE_V * values, long m, long n, __attribute__((unused)) long nnz, _TYPE_I ** row_ptr_ret, _TYPE_I ** col_idx_ret, _TYPE_V ** values_ret, long * nnz_out, long * nnz_diag_out, const int sort_columns)
{
	_TYPE_I * row_ptr_new;
	_TYPE_I * col_idx_new;
	_TYPE_V * values_new = NULL;
	long nnz_diag = 0, nnz_non_diag = 0, nnz_new;
	if (nnz_out == NULL)
		error("return variable is NULL: nnz_out");
	if (row_ptr_ret == NULL)
		error("return variable is NULL: row_ptr_ret");
	if (col_idx_ret == NULL)
		error("return variable is NULL: col_idx_ret");
	if (values != NULL && values_ret == NULL)
		error("return variable is NULL: values_ret");
	row_ptr_new = (typeof(row_ptr_new)) malloc((m+1) * sizeof(*row_ptr_new));
	_Pragma("omp parallel")
	{
		long i, j, col;
		long nnz_diag_t = 0, nnz_non_diag_t = 0;
		_Pragma("omp for")
		for (i=0;i<=m;i++)
			row_ptr_new[i] = 0;
		_Pragma("omp for")
		for (i=0;i<m;i++)
		{
			for (j=row_ptr[i];j<row_ptr[i+1];j++)
			{
				col = col_idx[j];
				if (i >= col)
				{
					row_ptr_new[i]++;
					if (i == col)
						nnz_diag_t++;
					else
						nnz_non_diag_t++;
				}
			}
		}
		__atomic_fetch_add(&nnz_diag, nnz_diag_t, __ATOMIC_RELAXED);
		__atomic_fetch_add(&nnz_non_diag, nnz_non_diag_t, __ATOMIC_RELAXED);
	}
	nnz_new = nnz_diag + nnz_non_diag;
	scan_reduce(row_ptr_new, row_ptr_new, m+1, 0, 0, 0);
	col_idx_new = (typeof(col_idx_new)) malloc(nnz_new * sizeof(*col_idx_new));
	if (values != NULL)
		values_new = (typeof(values_new)) malloc(nnz_new * sizeof(*values_new));
	else
		values_new = NULL;
	_Pragma("omp parallel")
	{
		long i, j, pos, col;
		_Pragma("omp for")
		for (i=0;i<m;i++)
		{
			for (j=row_ptr[i];j<row_ptr[i+1];j++)
			{
				col = col_idx[j];
				if (i >= col)
				{
					row_ptr_new[i]--;
					pos = row_ptr_new[i];
					col_idx_new[pos] = col;
					if (values != NULL)
						values_new[pos] = values[j];
				}
			}
		}
	}
	if (sort_columns)
	{
		csr_sort_columns(row_ptr_new, col_idx_new, values_new, m, n, nnz_new);
	}
	*row_ptr_ret = row_ptr_new;
	*col_idx_ret = col_idx_new;
	if (values != NULL)
		*values_ret = values_new;
	*nnz_out = nnz_new;
	if (nnz_diag_out != NULL)
		*nnz_diag_out = nnz_diag;
}


//==========================================================================================================================================
//= IO
//==========================================================================================================================================


#undef  csr_save_to_mtx
#define csr_save_to_mtx  CSR_GEN_EXPAND(csr_save_to_mtx)
CSR_GEN_FUNCTION_ATTRIBUTES
void
csr_save_to_mtx(_TYPE_I * row_ptr, _TYPE_I * col_idx, _TYPE_V * values, long m, long n, const char* filename)
{
	FILE * file;
	long i, j;
	file = fopen(filename, "w");
	if (file == NULL) {
		printf("Error opening file %s\n", filename);
		return;
	}
	fprintf(file, "%%%%MatrixMarket matrix coordinate real general\n");
	fprintf(file, "%ld %ld %d\n", m, n, row_ptr[m]);
	long buf_n = 10000;
	char buf[buf_n];
	long k;
	for (i=0;i<m;i++)
	{
		for (j=row_ptr[i];j<row_ptr[i+1];j++)
		{
			k = 0;
			k += snprintf(buf+k, buf_n-k, "%ld %d ", i + 1, col_idx[j] + 1);
			gen_numtostr(buf+k, buf_n-k, ".15", values[j]);
			fprintf(file, "%s\n", buf);
		}
	}
	fclose(file);
}


//==========================================================================================================================================
//= Includes Undefs
//==========================================================================================================================================


/* #include "data_structures/hashtable/hashtable_gen_pop.h" */
#include "sort/bucketsort/bucketsort_gen_pop.h"
#include "sort/quicksort/quicksort_gen_pop.h"
#include "functools/functools_gen_pop.h"

