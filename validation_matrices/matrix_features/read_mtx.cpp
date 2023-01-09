#include <iostream>
#include <cstring> // memset
#include <cstdlib> // for random generation
#include <cstdio>  // for printf
#include <cmath>
#include <omp.h>   // just for timer

#include <string>
#include <fstream>


#ifdef __cplusplus
extern "C"{
#endif
	#include "macros/cpp_defines.h"
	#include "debug.h"
	#include "io.h"
	#include "genlib.h"
	#include "parallel_util.h"
	#include "string_util.h"
	#include "parallel_io.h"
#ifdef __cplusplus
}
#endif

#include "read_mtx.h"


/**
 * Builds a MARKET COO sparse from the given file.
 */
void
create_coo_matrix(const std::string & market_filename,
	ValueType ** V_out, int ** R_out, int ** C_out, int * m_ptr, int * n_ptr, int * nnz_ptr)
{
	int num_threads = omp_get_max_threads();
	struct File_Atoms * A;
	char ** lines;

	int nr_rows=0, nr_cols=0, nr_nnzs=0;
	int * R = NULL, * C = NULL;
	ValueType * V = NULL;
	bool array = false;
	bool symmetric = false;
	bool skew = false;

	int num_lines;
	int non_diag_total = 0;
	int offsets[num_threads];
	long i;

	A = (typeof(A)) malloc(sizeof(*A));
	file_to_lines(A, market_filename.c_str(), 0);
	lines = A->atoms;

	// std::ifstream ifs;
	// ifs.open(market_filename.c_str(), std::ifstream::in);
	// if (!ifs.good())
		// error("Error opening file\n");

	i = 0;

	// Banner
	if (strstr(lines[0], "%%MatrixMarket") != NULL)
	{
		symmetric   = (strstr(lines[0], "symmetric") != NULL);
		skew        = (strstr(lines[0], "skew") != NULL);
		array       = (strstr(lines[0], "array") != NULL);
		// printf("(symmetric: %d, skew: %d, array: %d)\n", symmetric, skew, array);
		// fflush(stdout);
	}

	while (lines[i][0] == '%')     // Comments.
		i++;

	// Problem description
	int nparsed = sscanf(lines[i], "%d %d %d", &nr_rows, &nr_cols, &num_lines);
	nr_nnzs = num_lines;
	// std::cout << "rows " << nr_rows << ", cols " << nr_cols << ", nnz " << num_lines << "\n";
	if ((!array) && (nparsed == 3))
	{
		if (symmetric)
			nr_nnzs *= 2;
		V = (ValueType *) aligned_alloc(64, nr_nnzs * sizeof(ValueType));
		R = (int *) aligned_alloc(64, nr_nnzs * sizeof(int));
		C = (int *) aligned_alloc(64, nr_nnzs * sizeof(int));
	}
	else
		error("Error parsing MARKET matrix: invalid problem description: %s\n", lines[i]);
	i++;

	if (num_lines != A->num_atoms - i)
		error("Error parsing MARKET matrix: remaining number of file lines (%ld) don't match the number of non-zeros (%d)\n", A->num_atoms - i, num_lines);

	lines = &lines[i];
	_Pragma("omp parallel")
	{
		int tnum = omp_get_thread_num();
		long i, i_s, i_e, j;
		long non_diag = 0;
		int row=1, col=1;
		ValueType val=1.0;
		char * l, * t;

		loop_partitioner_balance_iterations(num_threads, tnum, 0, num_lines, &i_s, &i_e);

		// Parse nonzero (note: using strtol and strtod is 2x faster than sscanf or istream parsing)
		for (i=i_s;i<i_e;i++)
		{
			l = lines[i];
			row = strtol(l, &t, 0);
			if (t == l)
				error("Error parsing MARKET matrix: badly formed row at edge %ld\n", i);
			l = t;
			col = strtol(l, &t, 0);
			if (t == l)
				error("Error parsing MARKET matrix: badly formed col at edge %ld\n", i);
			l = t;
			#if DOUBLE == 0
				val = strtof(l, &t);
			#elif DOUBLE == 1
				val = strtod(l, &t);
			#endif
			if (t == l)                // Type of values is pattern (i.e. no values).
				val = 1.0;
			R[i] = row - 1;
			C[i] = col-1;
			V[i] = val;
			if (symmetric && (row != col))
				non_diag++;
		}

		if (symmetric)
		{
			__atomic_fetch_add(&non_diag_total, non_diag, __ATOMIC_RELAXED);
			__atomic_store_n(&(offsets[tnum]), non_diag, __ATOMIC_RELAXED);

			_Pragma("omp barrier")

			_Pragma("omp single")
			{
				long a = 0, tmp;
				long diag = num_lines - non_diag_total;
				nr_nnzs = 2*non_diag_total + diag;
				for (i=0;i<num_threads;i++)
				{
					tmp = offsets[i];
					offsets[i] = a;
					a += tmp;
				}
				// std::cout << "rows " << nr_rows << ", cols " << nr_cols << ", nnz " << nr_nnzs << "\n";
			}

			_Pragma("omp barrier")

			j = num_lines + offsets[tnum];
			for (i=i_s;i<i_e;i++)
			{
				if (C[i] != R[i])
				{
					R[j] = C[i];
					C[j] = R[i];
					V[j] = skew ? -V[i] : V[i];
					j++;
				}
			}
		}
	}

	*V_out = V;
	*R_out = R;
	*C_out = C;
	*m_ptr = nr_rows;
	*n_ptr = nr_cols;
	*nnz_ptr = nr_nnzs;

	file_atoms_destroy(&A);
}

