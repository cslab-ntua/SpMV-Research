#ifndef READ_MTX_H
#define READ_MTX_H

#include <iostream>
#include <cstring> // memset
#include <cstdlib> // for random generation
#include <cstdio>  // for printf
#include <cmath>
#include <omp.h>   // just for timer

// #include <stdio.h>
#include <string>
#include <fstream>

// #include <cassert> // for assert
// #include <iterator>
// #include <algorithm>
// #include <queue>
// #include <set>
// #include <list>
// #include <sys/time.h>

#include "common.h"

#include "macros/cpp_defines.h"
#include "debug.h"


/**
 * Builds a MARKET COO sparse from the given file.
 */
void create_coo_matrix(const std::string & market_filename,
	ValueType ** V_out, INT_T ** R_out, INT_T ** C_out, INT_T * m_ptr, INT_T * n_ptr, INT_T * nnz_ptr)
{
	INT_T nr_rows=0, nr_cols=0, nr_nnzs=0;
	ValueType * V = NULL;
	INT_T * R = NULL, * C = NULL;

	std::ifstream ifs;
	ifs.open(market_filename.c_str(), std::ifstream::in);
	if (!ifs.good())
		error("Error opening file\n");

	bool    array = false;
	bool    symmetric = false;
	bool    skew = false;
	INT_T     current_nz = -1;
	char    line[1024];

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
			// std::cout << "*m_ptr " << *m_ptr << " *n_ptr " << *n_ptr << " *nnz_ptr " << *nnz_ptr << "\n";
			if ((!array) && (nparsed == 3))
			{
				if (symmetric)
					nr_nnzs *= 2;

				// V = new ValueType[nr_nnzs];
				// R = new int[nr_nnzs];
				// C = new int[nr_nnzs];

				V = (ValueType *) aligned_alloc(64, nr_nnzs * sizeof(ValueType));
				R = (INT_T *) aligned_alloc(64, nr_nnzs * sizeof(INT_T));
				C = (INT_T *) aligned_alloc(64, nr_nnzs * sizeof(INT_T));

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

				V[current_nz] = val;
				R[current_nz] = row - 1;
				C[current_nz] = col-1;
				// if(current_nz<10)
				// std::cout << " molis diavasa to " << current_nz << " stoixeio " << V[current_nz] << " me ta stoixeia : (" << R[current_nz] << ","<< C[current_nz] <<")\n";
			}

			current_nz++;

			// Adjust nonzero count (nonzeros along the diagonal aren't reversed)
			if (symmetric && (row != col))
			{
				V[current_nz] = val * (skew ? -1 : 1);
				R[current_nz] = col - 1;
				C[current_nz] = row -1;
				current_nz++;
			}
		}
	}

	*V_out = V;
	*R_out = R;
	*C_out = C;
	*m_ptr = nr_rows;
	*n_ptr = nr_cols;
	*nnz_ptr = current_nz;

	ifs.close();
}


#endif /* READ_MTX_H */

