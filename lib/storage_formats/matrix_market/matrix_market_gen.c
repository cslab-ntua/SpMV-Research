#include "macros/macrolib.h"
#include "parallel_util.h"

/* 
 * Additional variants are defined for cases in which symmetries can be used to significantly reduce the size of the data: symmetric, skew-symmetric and Hermitian.
 * In these cases, only entries in the lower triangular portion need be supplied.
 * In the skew-symmetric case the diagonal entries are zero, and hence they too are omitted.
 */

#undef  generic_name_expand
#define generic_name_expand(name)  CONCAT(name, SUFFIX)


#ifndef MATRIX_MARKET_GEN_C
#define MATRIX_MARKET_GEN_C

#include "data_structures/vector/vector_gen_undef.h"
#define VECTOR_GEN_TYPE_1  char
#define VECTOR_GEN_SUFFIX  _mtx_c
#include "data_structures/vector/vector_gen.c"

#endif /* MATRIX_MARKET_GEN_C */


//==========================================================================================================================================
//------------------------------------------------------------------------------------------------------------------------------------------
//-                                                             Parse Data                                                                 -
//------------------------------------------------------------------------------------------------------------------------------------------
//==========================================================================================================================================


#undef  mtx_parse_array_format
#define mtx_parse_array_format  generic_name_expand(mtx_parse_array_format)
static
void
mtx_parse_array_format(char ** lines, long * lengths, struct Matrix_Market * MTX)
{
	// const int symmetric = MTX->symmetric || MTX->skew_symmetric;
	TYPE * V = (typeof(V)) MTX->V;
	// int complex_weights = (strcmp(MTX->field, "complex") == 0);
	long M, N;

	M = MTX->m;
	N = MTX->n;
	_Pragma("omp parallel")
	{
		char * ptr;
		long i, j, k, len;
		_Pragma("omp for")
		for (i=0;i<M;i++)
		{
			ptr = lines[i];
			k = 0;
			for (j=0;j<N;j++)
			{
				len = gen_strtonum(ptr + k, lengths[i] - k, &V[i*N + j]);
				if (len == 0)
					error("Error parsing MARKET matrix '%s': badly formed or missing value at matrix position (%ld, %ld)\n", MTX->filename, i, j);
				k += len;
			}
		}
	}
}


#undef  mtx_parse_coordinate_format
#define mtx_parse_coordinate_format  generic_name_expand(mtx_parse_coordinate_format)
static
void
mtx_parse_coordinate_format(char ** lines, long * lengths, struct Matrix_Market * MTX, long expand_symmetry)
{
	int num_threads = safe_omp_get_num_threads_external();
	const int symmetric = MTX->symmetric || MTX->skew_symmetric;
	int * R = MTX->R;
	int * C = MTX->C;
	TYPE * V = (typeof(V)) MTX->V;
	int complex_weights = (strcmp(MTX->field, "complex") == 0);
	int non_diag_total = 0;
	int offsets[num_threads];
	int num_lines = MTX->nnz_sym;

	_Pragma("omp parallel")
	{
		int tnum = omp_get_thread_num();
		long i, i_s, i_e, j, len;
		char * ptr;
		long non_diag = 0;

		loop_partitioner_balance_iterations(num_threads, tnum, 0, num_lines, &i_s, &i_e);

		for (i=i_s;i<i_e;i++)
		{
			ptr = lines[i];
			j = 0;
			len = gen_strtonum(ptr + j, lengths[i] - j, &R[i]);
			// if (len == 0)
				// error("Error parsing MARKET matrix '%s': badly formed or missing row index at edge %ld\n", MTX->filename, i);
			j += len;
			R[i]--;   // From base-1 to base-0.
			len = gen_strtonum(ptr + j, lengths[i] - j, &C[i]);
			// if (len == 0)
				// error("Error parsing MARKET matrix '%s': badly formed or missing column index at edge %ld\n", MTX->filename, i);
			j += len;
			C[i]--;   // From base-1 to base-0.
			if (V != NULL)
			{
				len = gen_strtonum(ptr + j, lengths[i] - j, &V[i]);
				// if (len == 0)
					// error("Error parsing MARKET matrix '%s': badly formed or missing value at edge %ld\n", MTX->filename, i);
				j += len;
			}
			if (C[i] != R[i])
				non_diag++;
		}

		if (expand_symmetry && symmetric)
		{
			__atomic_fetch_add(&non_diag_total, non_diag, __ATOMIC_RELAXED);
			__atomic_store_n(&(offsets[tnum]), non_diag, __ATOMIC_RELAXED);

			_Pragma("omp barrier")

			_Pragma("omp single")
			{
				long a = 0, tmp;
				long diag = num_lines - non_diag_total;
				MTX->nnz = 2*non_diag_total + diag;
				for (i=0;i<num_threads;i++)
				{
					tmp = offsets[i];
					offsets[i] = a;
					a += tmp;
				}
			}

			_Pragma("omp barrier")

			long i;
			long j = num_lines + offsets[tnum];
			for (i=i_s;i<i_e;i++)
			{
				if (C[i] != R[i])
				{
					R[j] = C[i];
					C[j] = R[i];
					if (V != NULL)
					{
						if (complex_weights)
							V[j] = (MTX->skew_symmetric) ? -conj(V[i]) : conj(V[i]);
						else
							V[j] = (MTX->skew_symmetric) ? -V[i] : V[i];
					}
					j++;
				}
			}
		}
	}
}


#undef  mtx_parse_data
#define mtx_parse_data  generic_name_expand(mtx_parse_data)
static
void
mtx_parse_data(char ** lines, long * lengths, struct Matrix_Market * MTX, long expand_symmetry)
{
	if (!strcmp(MTX->format, "coordinate"))
		mtx_parse_coordinate_format(lines, lengths, MTX, expand_symmetry);
	else
		mtx_parse_array_format(lines, lengths, MTX);
}


//==========================================================================================================================================
//------------------------------------------------------------------------------------------------------------------------------------------
//-                                                          Convert to String                                                             -
//------------------------------------------------------------------------------------------------------------------------------------------
//==========================================================================================================================================


/*
#undef  mtx_to_string
#define mtx_to_string  generic_name_expand(mtx_to_string)
static
struct Vector *
mtx_to_string(struct Matrix_Market * MTX)
{
	int * R = MTX->R;
	int * C = MTX->C;
	TYPE * V = MTX->V;
	struct Vector * v;
	int buf_n = 10000, len;
	char buf[buf_n];
	long i;

	v = vector_new(0);

	len = snprintf(buf, buf_n, "%ld %ld %ld\n", MTX->m, MTX->n, MTX->nnz);
	vector_push_back_array(v, buf, len);

	for (i=0;i<MTX->nnz;i++)
	{
		len = 0;
		len += gen_numtostr(buf+len, buf_n-len, "", R[i] + 1);  // Base 1 arrays.
		buf[len++] = ' ';
		len += gen_numtostr(buf+len, buf_n-len, "", C[i] + 1);  // Base 1 arrays.
		if (strcmp(MTX->field, "pattern") != 0)
		{
			buf[len++] = ' ';
			len += gen_numtostr(buf+len, buf_n-len, "", V[i]);
		}
		len += snprintf(buf+len, buf_n-len, "\n");
		vector_push_back_array(v, buf, len);
	}
	return v;
} */


// Returns a 'page aligned' character array.

#undef  mtx_to_string_par
#define mtx_to_string_par  generic_name_expand(mtx_to_string_par)
static
long
mtx_to_string_par(struct Matrix_Market * MTX, char ** str_ptr)
{
	int num_threads = safe_omp_get_num_threads_external();
	int * R = MTX->R;
	int * C = MTX->C;
	TYPE * V = MTX->V;
	long offsets[num_threads];
	char * str;
	long str_len;

	#pragma omp parallel
	{
		int tnum = omp_get_thread_num();
		struct Vector * v;
		int buf_n = 10000, len;
		char buf[buf_n];
		long i, i_s, i_e;

		loop_partitioner_balance_iterations(num_threads, tnum, 0, MTX->nnz, &i_s, &i_e);

		v = vector_new(0);

		if (tnum == 0)
		{
			len = snprintf(buf, buf_n, "%ld %ld %ld\n", MTX->m, MTX->n, MTX->nnz);
			vector_push_back_array(v, buf, len);
		}

		for (i=i_s;i<i_e;i++)
		{
			len = 0;
			len += gen_numtostr(buf+len, buf_n-len, "", R[i] + 1);  // Base 1 arrays.
			buf[len++] = ' ';
			len += gen_numtostr(buf+len, buf_n-len, "", C[i] + 1);  // Base 1 arrays.
			if (strcmp(MTX->field, "pattern") != 0)
			{
				buf[len++] = ' ';
				len += gen_numtostr(buf+len, buf_n-len, "", V[i]);
			}
			len += snprintf(buf+len, buf_n-len, "\n");
			vector_push_back_array(v, buf, len);
		}

		offsets[tnum] = v->size;

		#pragma omp barrier
		#pragma omp single
		{
			long tmp;
			str_len = 0;
			for (i=0;i<num_threads;i++)
			{
				tmp = offsets[i];
				offsets[i] = str_len;
				str_len += tmp;
			}
			// str = malloc(str_len);
			str = aligned_alloc(sysconf(_SC_PAGESIZE), str_len);
		}
		#pragma omp barrier

		for (i=0;i<v->size;i++)
		{
			str[offsets[tnum] + i] = v->data[i];
		}

		vector_destroy(&v);
	}

	*str_ptr = str;
	return str_len;
}

