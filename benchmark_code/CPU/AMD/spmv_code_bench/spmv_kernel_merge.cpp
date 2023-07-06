#include <stdlib.h>
#include <stdio.h>
#include <omp.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdio>
#include <fstream>
#include <sstream>
#include <limits>

#include "merge/sparse_matrix.h"
#include "merge/utils.h"

#include "macros/cpp_defines.h"

#include "spmv_bench_common.h"
#include "spmv_kernel.h"

using namespace std;

// https://github.com/dumerrill/merge-spmv


struct MERGEArrays : Matrix_Format
{
	ValueType * a;   // the values (of size NNZ)
	INT_T * ia;      // the usual rowptr (of size m+1)
	INT_T * ja;      // the colidx of each NNZ (of size nnz)
	int num_threads;

	MERGEArrays(long m, long n, long nnz) : Matrix_Format(m, n, nnz)
	{
		a = NULL;
		ia = NULL;
		ja= NULL;
		num_threads = omp_get_max_threads();
	}

	~MERGEArrays()
	{
		free(a);
		free(ia);
		free(ja);
	}

	void spmv(ValueType * x, ValueType * y);
	void statistics_start();
	int statistics_print(__attribute__((unused)) char * buf, __attribute__((unused)) long buf_n);
};


void compute_merge(MERGEArrays * merge, ValueType * x , ValueType * y);


void
MERGEArrays::spmv(ValueType * x, ValueType * y)
{
	compute_merge(this, x, y);
}


struct Matrix_Format *
csr_to_format(INT_T * row_ptr, INT_T * col_ind, ValueType * values, long m, long n, long nnz)
{
	struct MERGEArrays * merge = new MERGEArrays(m, n, nnz);
	merge->format_name = (char *) "MERGE";
	merge->ia = row_ptr;
	merge->ja = col_ind;
	merge->a = values;
	merge->mem_footprint = nnz * (sizeof(ValueType) + sizeof(INT_T)) + (m+1) * sizeof(INT_T);

	return merge;
}


//---------------------------------------------------------------------
// Utility types
//---------------------------------------------------------------------

struct int2
{
	int x;
	int y;
};

/**
 * Counting iterator
 */

struct CountingInputIterator
{
	// Required iterator traits
	typedef CountingInputIterator               self_type;              ///< My own type
	typedef INT_T                               difference_type;        ///< Type to express the result of subtracting one iterator from another
	typedef ValueType                           value_type;             ///< The type of the element the iterator can point to
	typedef ValueType*                          pointer;                ///< The type of a pointer to an element the iterator can point to
	typedef ValueType                           reference;              ///< The type of a reference to an element the iterator can point to
	typedef std::random_access_iterator_tag     iterator_category;      ///< The iterator category

	ValueType val;

	/// Constructor
	inline CountingInputIterator(
		const ValueType &val)          ///< Starting value for the iterator instance to report
	:
		val(val)
	{}

	/// Postfix increment
	inline self_type operator++(int)
	{
		self_type retval = *this;
		val++;
		return retval;
	}

	/// Prefix increment
	inline self_type operator++()
	{
		val++;
		return *this;
	}

	/// Indirection
	inline reference operator*() const
	{
		return val;
	}

	/// Addition
	template <typename Distance>
	inline self_type operator+(Distance n) const
	{
		self_type retval(val + n);
		return retval;
	}

	/// Addition assignment
	template <typename Distance>
	inline self_type& operator+=(Distance n)
	{
		val += n;
		return *this;
	}

	/// Subtraction
	template <typename Distance>
	inline self_type operator-(Distance n) const
	{
		self_type retval(val - n);
		return retval;
	}

	/// Subtraction assignment
	template <typename Distance>
	inline self_type& operator-=(Distance n)
	{
		val -= n;
		return *this;
	}

	/// Distance
	inline difference_type operator-(self_type other) const
	{
		return val - other.val;
	}

	/// Array subscript
	template <typename Distance>
	inline reference operator[](Distance n) const
	{
		return val + n;
	}

	/// Structure dereference
	inline pointer operator->()
	{
		return &val;
	}

	/// Equal to
	inline bool operator==(const self_type& rhs)
	{
		return (val == rhs.val);
	}

	/// Not equal to
	inline bool operator!=(const self_type& rhs)
	{
		return (val != rhs.val);
	}

	/// ostream operator
	friend std::ostream& operator<<(std::ostream& os, const self_type& itr)
	{
		os << "[" << itr.val << "]";
		return os;
	}
};



//---------------------------------------------------------------------
// MergePath Search
//---------------------------------------------------------------------


/**
 * Computes the begin offsets into A and B for the specific diagonal
 */
template <
	typename AIteratorT,
	typename BIteratorT,
	typename CoordinateT>
inline void MergePathSearch(
	INT_T           diagonal,           ///< [in]The diagonal to search
	AIteratorT      a,                  ///< [in]List A
	BIteratorT      b,                  ///< [in]List B
	INT_T           a_len,              ///< [in]Length of A
	INT_T           b_len,              ///< [in]Length of B
	CoordinateT&    path_coordinate)    ///< [out] (x,y) coordinate where diagonal intersects the merge path
{
	INT_T x_min = std::max(diagonal - b_len, 0);
	INT_T x_max = std::min(diagonal, a_len);

	while (x_min < x_max)
	{
		INT_T x_pivot = (x_min + x_max) >> 1;
		if (a[x_pivot] <= b[diagonal - x_pivot - 1])
			x_min = x_pivot + 1;    // Contract range up A (down B)
		else
			x_max = x_pivot;        // Contract range down A (up B)
	}

	path_coordinate.x = std::min(x_min, a_len);
	path_coordinate.y = diagonal - x_min;
}



void OmpMergeCsrmv(
	int                         num_threads,
	INT_T *         __restrict  row_end_offsets,    ///< Merge list A (row end-offsets)
	INT_T *         __restrict  column_indices,
	ValueType *     __restrict  values,
	ValueType *     __restrict  vector_x,
	ValueType *     __restrict  vector_y_out,
	INT_T                       num_rows,
	INT_T                       num_nonzeros
	)
{
	// Temporary storage for inter-thread fix-up after load-balanced work
	INT_T     row_carry_out[256];     // The last row-id each worked on by each thread when it finished its path segment
	ValueType      value_carry_out[256];   // The running total within each thread when it finished its path segment

	#pragma omp parallel for schedule(static) num_threads(num_threads)
	for (int tid = 0; tid < num_threads; tid++)
	{
		// Merge list B (NZ indices)
		CountingInputIterator  nonzero_indices(0);

		INT_T num_merge_items     = num_rows + num_nonzeros;                          // Merge path total length
		INT_T items_per_thread    = (num_merge_items + num_threads - 1) / num_threads;    // Merge items per thread

		// Find starting and ending MergePath coordinates (row-idx, nonzero-idx) for each thread
		int2    thread_coord;
		int2    thread_coord_end;
		int     start_diagonal      = std::min(items_per_thread * tid, num_merge_items);
		int     end_diagonal        = std::min(start_diagonal + items_per_thread, num_merge_items);

		MergePathSearch(start_diagonal, row_end_offsets, nonzero_indices, num_rows, num_nonzeros, thread_coord);
		MergePathSearch(end_diagonal, row_end_offsets, nonzero_indices, num_rows, num_nonzeros, thread_coord_end);

		// Consume whole rows
		for (; thread_coord.x < thread_coord_end.x; ++thread_coord.x)
		{
			ValueType running_total = 0.0;
			for (; thread_coord.y < row_end_offsets[thread_coord.x]; ++thread_coord.y)
			{
				running_total += values[thread_coord.y] * vector_x[column_indices[thread_coord.y]];
			}

			vector_y_out[thread_coord.x] = running_total;
		}

		// Consume partial portion of thread's last row
		ValueType running_total = 0.0;
		for (; thread_coord.y < thread_coord_end.y; ++thread_coord.y)
		{
			running_total += values[thread_coord.y] * vector_x[column_indices[thread_coord.y]];
		}

		// Save carry-outs
		row_carry_out[tid] = thread_coord_end.x;
		value_carry_out[tid] = running_total;
	}

	// Carry-out fix-up (rows spanning multiple threads)
	for (int tid = 0; tid < num_threads - 1; ++tid)
	{
		if (row_carry_out[tid] < num_rows)
			vector_y_out[row_carry_out[tid]] += value_carry_out[tid];
	}
}


void
compute_merge(MERGEArrays * merge, ValueType * x , ValueType * y)
{
	OmpMergeCsrmv(merge->num_threads, merge->ia + 1, merge->ja, merge->a, x, y, merge->m, merge->nnz);
}


//==========================================================================================================================================
//= Print Statistics
//==========================================================================================================================================


void
MERGEArrays::statistics_start()
{
}


int
MERGEArrays::statistics_print(__attribute__((unused)) char * buf, __attribute__((unused)) long buf_n)
{
	return 0;
}

