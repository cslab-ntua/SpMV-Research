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


struct ELLArrays : Matrix_Format
{
	INT_T width;  //< max nnz per row
	ValueType *a;   //< the values (of size NNZ)
	INT_T *ja;    //< the colidx of each NNZ (of size nnz)

	ELLArrays(long m, long n, long nnz) : Matrix_Format(m, n, nnz)
	{
		a = NULL;
		ja= NULL;
	}

	~ELLArrays()
	{
		free(a);
		free(ja);
	}

	void spmv(ValueType * x, ValueType * y);
	void statistics_start();
	int statistics_print_data(__attribute__((unused)) char * buf, __attribute__((unused)) long buf_n);
};


/* struct ELLSYMArrays : Matrix_Format
{
	INT_T width;  //< max nnz per row
	INT_T upper_n;
	ValueType * diag;
	ValueType * upper;       // upper diagonal
	ValueType * lower;       // lower diagonal
	INT_T * ja;    //< the colidx of each NNZ (of size nnz)

	ELLSYMArrays(){
		diag = NULL;
		upper = NULL;
		lower = NULL;
		ja = NULL;
	}

	~ELLSYMArrays()
	{
		free(diag);
		free(upper);
		free(lower);
		free(ja);
	}

	void spmv(ValueType * x, ValueType * y);
}; */


void compute_ell_par(ELLArrays * ell, ValueType * x , ValueType * y);
void compute_ell(ELLArrays * ell, ValueType * x , ValueType * y);
void compute_ell_unroll(ELLArrays * ell, ValueType * x , ValueType * y);
void compute_ell_v(ELLArrays * ell, ValueType * x , ValueType * y);
void compute_ell_v_hor(ELLArrays * ell, ValueType * x , ValueType * y);
void compute_ell_v_hor_split(ELLArrays * ell, ValueType * x , ValueType * y);
void compute_ell_transposed(ELLArrays * ell, ValueType * x , ValueType * y);
void compute_ell_transposed_v(ELLArrays * ell, ValueType * x , ValueType * y);


void
ELLArrays::spmv(ValueType * x, ValueType * y)
{
	#ifndef __XLC__
		// compute_ell(this, x, y);
		// compute_ell_v(this, x, y);
		// compute_ell_v_hor(this, x, y);
		// compute_ell_unroll(this, x, y);
		// compute_ell_v_hor_split(this, x, y);
		// compute_ell_transposed(this, x, y);
		compute_ell_transposed_v(this, x, y);
	#else
		compute_ell(this, x, y);
	#endif
}


template<typename T>
T *
transpose(T * a, INT_T m, INT_T n)
{
	T * t = (T *) aligned_alloc(64, m*n * sizeof(*t));
	#pragma omp parallel
	{
		long i, j;
		#pragma omp for schedule(static)
		for (j=0;j<n;j++)
		{
			for (i=0;i<m;i++)
				t[j*m + i] = a[i*n + j];
		}
	}
	return t;
}


struct Matrix_Format *
csr_to_format(INT_T * row_ptr, INT_T * col_ind, ValueType * values, long m, long n, long nnz, int symmetric)
{
	if (symmetric)
		error("symmetric matrices not supported by this format, expand symmetry");
	ELLArrays * ell = new ELLArrays(m, n, nnz);
	long i, j, j_s, j_e;
	long degree;
	long max_nnz_per_row;

	ell->format_name = (char *) "ELL";
	ell->mem_footprint = m * ell->width * (sizeof(ValueType) + sizeof(INT_T));
	// ell->m = m;
	ell->m = ((m + VECTOR_ELEM_NUM - 1) / VECTOR_ELEM_NUM) * VECTOR_ELEM_NUM;

	ell->n = n;
	ell->nnz = nnz;

	max_nnz_per_row = 0;
	for (i=0;i<ell->m;i++)
	{
		degree = row_ptr[i+1] - row_ptr[i];
		if (degree > max_nnz_per_row)
			max_nnz_per_row = degree;
	}
	printf("max degree = %ld\n", max_nnz_per_row);

	ell->width = max_nnz_per_row;
	// ell->width = ((max_nnz_per_row + VECTOR_ELEM_NUM - 1) / VECTOR_ELEM_NUM) * VECTOR_ELEM_NUM;

	printf("width = %d\n", ell->width);

	ell->a = (ValueType *) aligned_alloc(64, ell->m * ell->width * sizeof(ValueType));
	ell->ja = (INT_T *) aligned_alloc(64, ell->m * ell->width * sizeof(INT_T));
	#pragma omp parallel
	{
		long i, j, k;
		#pragma omp for
		for (i=0;i<m;i++)
		{
			k = i * ell->width;
			for (j=row_ptr[i];j<row_ptr[i+1];j++,k++)
			{
				ell->a[k] = values[j];
				ell->ja[k] = col_ind[j];
			}
			for (;k<(i+1)*ell->width;k++)
			{
				ell->a[k] = 0;
				ell->ja[k] = 0;
			}
		}
	}
	for (i=m;i<ell->m;i++)
	{
		j_s = i * ell->width;
		j_e = (i + 1) * ell->width;
		for (j=j_s;j<j_e;j++)
		{
			ell->a[j] = 0;
			ell->ja[j] = 0;
		}
	}

	ValueType * a = transpose<ValueType>(ell->a, ell->m, ell->width);
	INT_T * ja = transpose<INT_T>(ell->ja, ell->m, ell->width);
	free(ell->a);
	free(ell->ja);
	ell->a = a;
	ell->ja = ja;
	return ell;
}


//==========================================================================================================================================
//= ELLPACK
//==========================================================================================================================================


/*
struct Matrix_Format *
csr_to_format_sym(INT_T * row_ptr, INT_T * col_ind, ValueType * values, long m, long n, long nnz)
{
	long num_threads = omp_get_max_threads();
	long t_upper_n[num_threads];
	long degree;
	long max_nnz_per_row;

	ellsym->m = m;
	ellsym->n = n;
	ellsym->nnz = nnz;
	ellsym->upper_n = 0;

	// ell->m = m;
	ell->m = ((m + VECTOR_ELEM_NUM - 1) / VECTOR_ELEM_NUM) * VECTOR_ELEM_NUM;

	ell->n = n;
	ell->nnz = nnz;

	max_nnz_per_row = 0;
	for (i=0;i<ell->m;i++)
	{
		degree = row_ptr[i+1] - row_ptr[i];
		if (degree > max_nnz_per_row)
			max_nnz_per_row = degree;
	}
	printf("max degree = %ld\n", max_nnz_per_row);

	ell->width = max_nnz_per_row;

	printf("width = %d\n", ell->width);

	ellsym->diag = (typeof(ellsym->diag)) malloc(ellsym->m * sizeof(*ellsym->diag));
	#pragma omp parallel
	{
		long tnum = omp_get_thread_num();
		long upper_base = 0;
		long i, j, i_s, i_e;

		loop_partitioner_balance_iterations(num_threads, tnum, 0, ellsym->m, &i_s, &i_e);

		for (i=i_s;i<i_e;i++)
			for (j=row_ptr[i];j<row_ptr[i+1];j++)
				if (i < col_ind[j])
					upper_base++;
		t_upper_n[tnum] = upper_base;

		#pragma omp barrier
		#pragma omp single
		{
			long tmp = 0;
			upper_base = 0;
			for (i=0;i<num_threads;i++)
			{
				tmp = t_upper_n[i];
				t_upper_n[i] = upper_base;
				upper_base += tmp;
			}
			ellsym->upper_n = upper_base;
			ellsym->nnz = ellsym->m + 2 * upper_base;
			ellsym->upper = (typeof(ellsym->upper)) malloc(ellsym->upper_n * sizeof(*ellsym->upper));
			ellsym->lower = (typeof(ellsym->lower)) malloc(ellsym->upper_n * sizeof(*ellsym->lower));
			ellsym->row_idx = (typeof(ellsym->row_idx)) malloc(ellsym->upper_n * sizeof(*ellsym->row_idx));
			ellsym->col_idx = (typeof(ellsym->col_idx)) malloc(ellsym->upper_n * sizeof(*ellsym->col_idx));
		}
		#pragma omp barrier

		upper_base = t_upper_n[tnum];

		for (i=i_s;i<i_e;i++)
			for (j=row_ptr[i];j<row_ptr[i+1];j++)
			{
				if (i == col_ind[j])
					ellsym->diag[i] = values[j];
				else if (i < col_ind[j])
				{
					ellsym->upper[upper_base] = values[j];
					ellsym->lower[upper_base] = values[j];           // Symmetrical elements will be in the same position in the 'upper' and 'lower' arrays.
					ellsym->row_idx[upper_base] = i;
					ellsym->col_idx[upper_base] = col_ind[j];
					upper_base++;
				}
			}
	}
} */


void
compute_ell_par(ELLArrays * ell, ValueType * x , ValueType * y)
{
	#pragma omp parallel
	{
		ValueType sum;
		long i, j, j_s, j_e;
		const INT_T width = ell->width;
		PRAGMA(omp for schedule(static))
		for (i=0;i<ell->m;i++)
		{
			j_s = i * width;
			j_e = (i + 1) * width;
			sum = 0;
			for (j=j_s;j<j_e;j++)
				sum += ell->a[j] * x[ell->ja[j]];
			y[i] = sum;
		}
	}
}


void
compute_ell(ELLArrays * ell, ValueType * x , ValueType * y)
{
	ValueType sum;
	long i, j, j_s, j_e;
	const INT_T width = ell->width;
	for (i=0;i<ell->m;i++)
	{
		j_s = i * width;
		j_e = (i + 1) * width;
		sum = 0;
		for (j=j_s;j<j_e;j++)
			sum += ell->a[j] * x[ell->ja[j]];
		y[i] = sum;
	}
}


void
compute_ell_unroll(ELLArrays * ell, ValueType * x , ValueType * y)
{
	ValueType sum;
	long i, j, j_s, j_e, j_unroll, j_unroll_width;
	long unroll = 4;
	const long mask = ~(((long) unroll) - 1);      // unroll is a power of 2.
	const INT_T width = ell->width;
	j_unroll_width = width & mask;
	for (i=0;i<ell->m;i++)
	{
		j_s = i * width;
		j_e = (i + 1) * width;
		j_unroll = j_s + j_unroll_width;
		sum = 0;
		for (j=j_s;j<j_unroll;j+=unroll)
		{
			sum += ell->a[j+0] * x[ell->ja[j+0]];
			sum += ell->a[j+1] * x[ell->ja[j+1]];
			sum += ell->a[j+2] * x[ell->ja[j+2]];
			sum += ell->a[j+3] * x[ell->ja[j+3]];
		}
		for (;j<j_e;j++)
			sum += ell->a[j] * x[ell->ja[j]];
		y[i] = sum;
	}
}


void
compute_ell_transposed(ELLArrays * ell, ValueType * x , ValueType * y)
{
	long i, j, row;
	const INT_T width = ell->width;
	for (i=0;i<ell->n;i++)
		y[i] = 0;
	for (i=0;i<ell->width;i++)
	{
		PRAGMA(GCC ivdep)
		for (row=0,j=i*ell->m;j<(i + 1)*ell->m;row++,j++)
			y[row] += ell->a[j] * x[ell->ja[j]];
	}
}


#ifndef __XLC__

void
compute_ell_v(ELLArrays * ell, ValueType * x , ValueType * y)
{
	long i, i_vector, j, j_s, j_e, k;
	const long mask = ~(((long) VECTOR_ELEM_NUM) - 1);      // VECTOR_ELEM_NUM is a power of 2.
	Vector_Value_t zero = {0};
	__attribute__((unused)) Vector_Value_t v_a = zero, v_x = zero, v_mul = zero, v_sum = zero;
	__attribute__((unused)) ValueType sum = 0;
	const INT_T width = ell->width;
	i_vector = ell->m & mask;
	for (i=0;i<i_vector;i+=VECTOR_ELEM_NUM)
	{
		v_sum = zero;
		j_s = i * width;
		j_e = (i + 1) * width;
		for (j=j_s;j<j_e;j++)
		{

			PRAGMA(GCC unroll VECTOR_ELEM_NUM)
			PRAGMA(GCC ivdep)
			for (k=0;k<VECTOR_ELEM_NUM;k++)
			{
				v_mul[k] = ell->a[j+k*width] * x[ell->ja[j+k*width]];
			}
			v_sum += v_mul;

			// PRAGMA(GCC unroll VECTOR_ELEM_NUM)
			// PRAGMA(GCC ivdep)
			// for (k=0;k<VECTOR_ELEM_NUM;k++)
			// {
				// v_a[k] = ell->a[j+k*width] ;
				// v_x[k] = x[ell->ja[j+k*width]];
			// }
			// v_sum += v_a * v_x;

		}
		*((Vector_Value_t *)&y[i]) = v_sum;
	}
	for (i=i_vector;i<ell->m;i++)
	{
		j_s = i * width;
		j_e = (i + 1) * width;
		sum = 0;
		for (j=j_s;j<j_e;j++)
			sum += ell->a[j] * x[ell->ja[j]];
		y[i] = sum;
	}
}


void
compute_ell_v_hor(ELLArrays * ell, ValueType * x , ValueType * y)
{
	long i, j, j_s, j_e, k, j_vector_width, j_e_vector;
	const long mask = ~(((long) VECTOR_ELEM_NUM) - 1);      // VECTOR_ELEM_NUM is a power of 2.
	Vector_Value_t zero = {0};
	__attribute__((unused)) Vector_Value_t v_a, v_x = zero, v_mul = zero, v_sum = zero;
	__attribute__((unused)) ValueType sum = 0;
	const INT_T width = ell->width;
	j_vector_width = width & mask;
	for (i=0;i<ell->m;i++)
	{
		v_sum = zero;
		j_s = i * width;
		j_e = (i + 1) * width;
		j_e_vector = j_s + j_vector_width;
		for (j=j_s;j<j_e_vector;j+=VECTOR_ELEM_NUM)
		{
			v_a = *(Vector_Value_t *) &ell->a[j];
			PRAGMA(GCC unroll VECTOR_ELEM_NUM)
			PRAGMA(GCC ivdep)
			for (k=0;k<VECTOR_ELEM_NUM;k++)
			{
				v_mul[k] = v_a[k] * x[ell->ja[j+k]];
			}
			v_sum += v_mul;
		}
		for (;j<j_e;j++)
			v_sum[0] += ell->a[j] * x[ell->ja[j]];
		for (j=1;j<VECTOR_ELEM_NUM;j++)
			v_sum[0] += v_sum[j];
		y[i] = v_sum[0];
	}
}


// Twice slower than compute_ell_v_hor.
void
compute_ell_v_hor_split(ELLArrays * ell, ValueType * x , ValueType * y)
{
	long i, j, j_s, j_e, k, j_vector_width, j_e_vector;
	const long mask = ~(((long) VECTOR_ELEM_NUM) - 1);      // VECTOR_ELEM_NUM is a power of 2.
	Vector_Value_t zero = {0};
	__attribute__((unused)) Vector_Value_t v_a, v_x = zero, v_mul = zero, v_sum = zero;
	__attribute__((unused)) ValueType sum = 0;
	const INT_T width = ell->width;
	j_vector_width = width & mask;
	for (i=0;i<ell->m;i++)
	{
		v_sum = zero;
		j_s = i * width;
		j_e_vector = j_s + j_vector_width;
		for (j=j_s;j<j_e_vector;j+=VECTOR_ELEM_NUM)
		{
			v_a = *(Vector_Value_t *) &ell->a[j];
			PRAGMA(GCC unroll VECTOR_ELEM_NUM)
			PRAGMA(GCC ivdep)
			for (k=0;k<VECTOR_ELEM_NUM;k++)
			{
				v_mul[k] = v_a[k] * x[ell->ja[j+k]];
			}
			v_sum += v_mul;
		}
		for (j=1;j<VECTOR_ELEM_NUM;j++)
			v_sum[0] += v_sum[j];
		y[i] = v_sum[0];
	}
	for (i=0;i<ell->m;i++)
	{
		j_s = i * width;
		j_e = (i + 1) * width;
		j_e_vector = j_s + j_vector_width;
		for (j=j_e_vector;j<j_e;j++)
			y[i] += ell->a[j] * x[ell->ja[j]];
	}
}


void
compute_ell_transposed_v(ELLArrays * ell, ValueType * x , ValueType * y)
{
	long i, i_vector, j, j_s, j_e, k;
	const long mask = ~(((long) VECTOR_ELEM_NUM) - 1);      // VECTOR_ELEM_NUM is a power of 2.
	Vector_Value_t zero = {0};
	__attribute__((unused)) Vector_Value_t v_a = zero, v_x = zero, v_mul = zero, v_sum = zero;
	__attribute__((unused)) ValueType sum = 0;
	const INT_T width = ell->width;
	i_vector = ell->m & mask;
	PRAGMA(GCC unroll VECTOR_ELEM_NUM)
	PRAGMA(GCC ivdep)
	for (i=0;i<i_vector;i+=VECTOR_ELEM_NUM)
	{
		v_sum = zero;
		j_s = i;
		j_e = i + (width) * ell->m;
		for (j=j_s;j<j_e;j+=ell->m)
		{

			PRAGMA(GCC unroll VECTOR_ELEM_NUM)
			PRAGMA(GCC ivdep)
			for (k=0;k<VECTOR_ELEM_NUM;k++)
			{
				v_mul[k] = ell->a[j+k] * x[ell->ja[j+k]];
			}
			v_sum += v_mul;

			// v_a = *(Vector_Value_t *) &ell->a[j];
			// PRAGMA(GCC unroll VECTOR_ELEM_NUM)
			// PRAGMA(GCC ivdep)
			// for (k=0;k<VECTOR_ELEM_NUM;k++)
			// {
				// v_x[k] = x[ell->ja[j+k]];
			// }
			// v_sum += v_a * v_x;

		}
		*((Vector_Value_t *)&y[i]) = v_sum;
	}
	for (i=i_vector;i<ell->m;i++)
	{
		j_s = i * ell->m;
		j_e = (i + 1) * ell->m;
		sum = 0;
		for (j=j_s;j<j_e;j++)
			sum += ell->a[j] * x[ell->ja[j]];
		y[i] = sum;
	}
}


#endif /* __XLC__ */


//==========================================================================================================================================
//= Print Statistics
//==========================================================================================================================================


void
ELLArrays::statistics_start()
{
}


int
statistics_print_labels(__attribute__((unused)) char * buf, __attribute__((unused)) long buf_n)
{
	return 0;
}


int
ELLArrays::statistics_print_data(__attribute__((unused)) char * buf, __attribute__((unused)) long buf_n)
{
	return 0;
}

