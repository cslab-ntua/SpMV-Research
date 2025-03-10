#include <stdlib.h>
#include <stdio.h>
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
void
csr_sort_columns(_TYPE_I * row_ptr, _TYPE_I * col_idx, _TYPE_V * values, long m, long n, long nnz)
{
	int num_threads = safe_omp_get_num_threads_external();
	_TYPE_I * permutation, * C;
	_TYPE_V * V;
	long thread_i_s[num_threads];
	long thread_i_e[num_threads];

	permutation = (typeof(permutation)) malloc(nnz * sizeof(*permutation));
	C = (typeof(C)) malloc(nnz * sizeof(*C));
	V = (values != NULL) ? (typeof(V)) malloc(nnz * sizeof(*V)) : NULL;
	#pragma omp parallel
	{
		int tnum = omp_get_thread_num();
		long i, i_s, i_e, j;
		long degree;
		_TYPE_I * buf_permutation  = (typeof(buf_permutation)) malloc(n * sizeof(*buf_permutation));
		_TYPE_I * buf_offsets      = (typeof(buf_offsets)) malloc((n+1)*sizeof(*buf_offsets));
		_TYPE_I * qsort_partitions = (typeof(qsort_partitions)) malloc(m * sizeof(*qsort_partitions));

		loop_partitioner_balance_prefix_sums(num_threads, tnum, row_ptr, m, nnz, &thread_i_s[tnum], &thread_i_e[tnum]);
		i_s = thread_i_s[tnum];
		i_e = thread_i_e[tnum];

		long pos;
		#pragma omp for
		for (i=0;i<nnz;i++)
		{
			permutation[i] = i;
			C[i] = col_idx[i];
			if (values != NULL)
				V[i] = values[i];
		}

		for (i=i_s;i<i_e;i++)
		{
			degree = row_ptr[i+1] - row_ptr[i];
			if (degree == 0)
				continue;

			if (degree > n/5)
			{
				bucketsort_stable_recalculate_bucket_serial(&col_idx[row_ptr[i]], degree, n, NULL, buf_permutation, buf_offsets);
				for (j=row_ptr[i];j<row_ptr[i+1];j++)
				{
					pos = row_ptr[i] + buf_permutation[j - row_ptr[i]];
					col_idx[pos] = C[j];
					if (values != NULL)
						values[pos] = V[j];
				}
			}
			else
			{
				quicksort(&permutation[row_ptr[i]], degree, col_idx, qsort_partitions);
				for (j=row_ptr[i];j<row_ptr[i+1];j++)
				{
					pos = permutation[j];
					col_idx[j] = C[pos];
					if (values != NULL)
						values[j] = V[pos];
				}
			}
		}

		free(buf_permutation);
		free(buf_offsets);
		free(qsort_partitions);
	}
	free(permutation);
	free(C);
	free(V);
}


// An implementation from scratch (without bucketsort ...) doesn't seem to be any faster.
#undef  coo_to_csr
#define coo_to_csr  CSR_GEN_EXPAND(coo_to_csr)
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
//= Save To File
//==========================================================================================================================================


#undef  csr_save_to_mtx
#define csr_save_to_mtx  CSR_GEN_EXPAND(csr_save_to_mtx)
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
//= Expand Symmetric Matrix
//==========================================================================================================================================


#undef  csr_expand_symmetric
#define csr_expand_symmetric  CSR_GEN_EXPAND(csr_expand_symmetric)
void
csr_expand_symmetric(_TYPE_I * row_ptr, _TYPE_I * col_idx, _TYPE_V * values, long m, long n, [[gnu::unused]] long nnz, _TYPE_I ** row_ptr_ret, _TYPE_I ** col_idx_ret, _TYPE_V ** values_ret, long * nnz_out, long * nnz_diag_out, const int sort_columns)
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
				__atomic_fetch_add(&row_ptr_new[i], 1, __ATOMIC_RELAXED);
				if (i < col)
					error("upper nonzero found: row=%d col=%d", i, col);
				if (i != col)
				{
					__atomic_fetch_add(&row_ptr_new[col], 1, __ATOMIC_RELAXED);
					nnz_non_diag_t++;
				}
				else
				{
					nnz_diag_t++;
				}
			}
		}
		__atomic_fetch_add(&nnz_diag, nnz_diag_t, __ATOMIC_RELAXED);
		__atomic_fetch_add(&nnz_non_diag, nnz_non_diag_t, __ATOMIC_RELAXED);
	}
	nnz_new = nnz_diag + 2*nnz_non_diag;
	col_idx_new = (typeof(col_idx_new)) malloc(nnz_new * sizeof(*col_idx_new));
	if (values != NULL)
		values_new = (typeof(values_new)) malloc(nnz_new * sizeof(*values_new));
	else
		values_new = NULL;
	scan_reduce(row_ptr_new, row_ptr_new, m+1, 0, 0, 0);
	_Pragma("omp parallel")
	{
		long i, j, pos, col;
		_Pragma("omp for")
		for (i=0;i<m;i++)
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
//= Remove Elements Above Diagonal
//==========================================================================================================================================


#undef  csr_drop_upper
#define csr_drop_upper  CSR_GEN_EXPAND(csr_drop_upper)
void csr_drop_upper(_TYPE_I * row_ptr, _TYPE_I * col_idx, _TYPE_V * values, long m, long n, [[gnu::unused]] long nnz, _TYPE_I ** row_ptr_ret, _TYPE_I ** col_idx_ret, _TYPE_V ** values_ret, long * nnz_out, long * nnz_diag_out, const int sort_columns)
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
	col_idx_new = (typeof(col_idx_new)) malloc(nnz_new * sizeof(*col_idx_new));
	if (values != NULL)
		values_new = (typeof(values_new)) malloc(nnz_new * sizeof(*values_new));
	else
		values_new = NULL;
	scan_reduce(row_ptr_new, row_ptr_new, m+1, 0, 0, 0);
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
//= Includes Undefs
//==========================================================================================================================================


#include "sort/bucketsort/bucketsort_gen_pop.h"
#include "sort/quicksort/quicksort_gen_pop.h"
#include "functools/functools_gen_pop.h"

