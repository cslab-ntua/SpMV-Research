#ifndef CONVERTERS_H
#define CONVERTERS_H

#include <iostream>
#include <omp.h>   // just for timer

#ifdef __cplusplus
extern "C"{
#endif

#include "debug.h"
#include "io.h"
#include "macros/cpp_defines.h"
#include "parallel_util.h"
#include "artificial_matrix_generation.h"

#include "aux/csr_converter.h"

#ifdef __cplusplus
}
#endif


#ifndef INT_T
	// #define INT_T  MKL_INT
	#define INT_T  int32_t
#endif

#if DOUBLE == 0
	#define ValueType  float
#elif DOUBLE == 1
	#define ValueType  double
#endif


typedef ValueType  Vector2_Value_t  __attribute__((vector_size(16), aligned(1)));

typedef ValueType  Vector4_Value_t  __attribute__((vector_size(32), aligned(1)));
// typedef ValueType  Vector4_Value_t  __attribute__((vector_size(32)));

// Number of elements for the vectorization function.
#define VECTOR_ELEM_NUM  ((int) (sizeof(Vector4_Value_t) / sizeof(ValueType)))

// typedef INT_T  Vector_Index_t  __attribute__((vector_size(VECTOR_ELEM_NUM*sizeof(INT_T)), aligned(1)));
typedef INT_T  Vector_Index_t  __attribute__((vector_size(VECTOR_ELEM_NUM*sizeof(INT_T))));


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


//////////////////////////////////////////////////////////////////////////
// COO format
//////////////////////////////////////////////////////////////////////////


struct COOArrays
{
	INT_T m;         //< rows
	INT_T n;         //< columns
	INT_T nnz;       //< the number of nnz inside the matrix
	ValueType *val;    //< the values (size = nnz)
	INT_T *rowind;   //< the row indexes (size = nnz)
	INT_T *colind;   //< the col indexes (size = nnz)

	/** simply set ptr to null */
	COOArrays(){
		val = NULL;
		rowind = NULL;
		colind = NULL;
	}

	/** delete ptr */
	~COOArrays(){
		// delete[] val;
		// delete[] rowind;
		// delete[] colind;
		free(val);
		free(rowind);
		free(colind);
	}
};


/**
 * Builds a MARKET COO sparse from the given file.
 */
void create_coo_matrix(const std::string & market_filename, COOArrays * coo)
{
	std::ifstream ifs;
	ifs.open(market_filename.c_str(), std::ifstream::in);
	if (!ifs.good())
		error("Error opening file\n");

	bool    array = false;
	bool    symmetric = false;
	bool    skew = false;
	INT_T     current_nz = -1;
	char    line[1024];

	INT_T nr_rows=0, nr_cols=0, nr_nnzs=0;
	while (true)
	{
		ifs.getline(line, 1024);
		if (!ifs.good())
		{
			// Done
			break;
		}
		if (line[0] == '%')
		{
			// Comment
			if (line[1] == '%')
			{
				// Banner
				symmetric   = (strstr(line, "symmetric") != NULL);
				skew        = (strstr(line, "skew") != NULL);
				array       = (strstr(line, "array") != NULL);
				// printf("(symmetric: %d, skew: %d, array: %d) ", symmetric, skew, array); fflush(stdout);
			}
		}
		else if (current_nz == -1)
		{
			// Problem description
			INT_T nparsed = sscanf(line, "%d %d %d", &nr_rows, &nr_cols, &nr_nnzs);
			coo->m = nr_rows;
			coo->n = nr_cols;
			coo->nnz = nr_nnzs;
			// std::cout << "coo->m " << coo->m << " coo->n " << coo->n << " coo->nnz " << coo->nnz << "\n";
			if ((!array) && (nparsed == 3))
			{
				if (symmetric)
					nr_nnzs *= 2;

				// Allocate coo matrix

				// coo->val = new ValueType[nr_nnzs];
				// coo->rowind = new int[nr_nnzs];
				// coo->colind = new int[nr_nnzs];

				coo->val = (ValueType *) aligned_alloc(64, nr_nnzs * sizeof(ValueType));
				coo->rowind = (INT_T *) aligned_alloc(64, nr_nnzs * sizeof(INT_T));
				coo->colind = (INT_T *) aligned_alloc(64, nr_nnzs * sizeof(INT_T));

				current_nz = 0;
			}
			else
				error("Error parsing MARKET matrix: invalid problem description: %s\n", line);
		}
		else
		{
			// Edge
			if (current_nz >= nr_nnzs)
				error("Error parsing MARKET matrix: encountered more than %d nr_nnzs\n", nr_nnzs);

			INT_T row=1, col=1;
			ValueType val=1.0;

			if (!array)
			{
				// Parse nonzero (note: using strtol and strtod is 2x faster than sscanf or istream parsing)
				char *l = line;
				char *t = NULL;

				// parse row
				row = strtol(l, &t, 0);
				if (t == l)
					error("Error parsing MARKET matrix: badly formed row at edge %d\n", current_nz);
				l = t;

				// parse col
				col = strtol(l, &t, 0);
				if (t == l)
					error("Error parsing MARKET matrix: badly formed col at edge %d\n", current_nz);
				l = t;

				// parse val
				#if DOUBLE == 0
					val = strtof(l, &t);
				#elif DOUBLE == 1
					val = strtod(l, &t);
				#endif

				if (t == l)
				{
					val = 1.0;
				}

				coo->val[current_nz] = val;
				coo->rowind[current_nz] = row - 1;
				coo->colind[current_nz] = col-1;
				// if(current_nz<10)
				// std::cout << " molis diavasa to " << current_nz << " stoixeio " << coo->val[current_nz] << " me ta stoixeia : (" << coo->rowind[current_nz] << ","<< coo->colind[current_nz] <<")\n";
			}

			current_nz++;

			// Adjust nonzero count (nonzeros along the diagonal aren't reversed)
			if (symmetric && (row != col))
			{
				coo->val[current_nz] = val * (skew ? -1 : 1);
				coo->rowind[current_nz] = col - 1;
				coo->colind[current_nz] = row -1;
				current_nz++;
			}
		}
	}

	coo->nnz = current_nz;
	ifs.close();
}


#endif /* CONVERTERS_H */

