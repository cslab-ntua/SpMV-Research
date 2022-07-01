/******************************************************************************
 * Copyright (c) 2011-2015, NVIDIA CORPORATION.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the NVIDIA CORPORATION nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NVIDIA CORPORATION BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIAeBILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************/

/******************************************************************************
 * How to build:
 *
 * VC++
 *      cl.exe mergebased_spmv.cpp /fp:strict /MT /O2 /openmp
 *
 * GCC (OMP is terrible)
 *      g++ mergebased_spmv.cpp -lm -ffloat-store -O3 -fopenmp
 *
 * Intel
 *      icpc mergebased_spmv.cpp -openmp -O3 -lrt -fno-alias -xHost -lnuma
 *      export KMP_AFFINITY=granularity=core,scatter
 *
 *
 ******************************************************************************/


//---------------------------------------------------------------------
// SpMV comparison tool
//---------------------------------------------------------------------


#include <omp.h>

#include <stdio.h>
#include <vector>
#include <algorithm>
#include <cstdio>
#include <fstream>
#include <sstream>
#include <iostream>
#include <limits>

#include "sparse_matrix.h"
#include "utils.h"

///////////////////////////////////////////
// added these for dgal matrix generator and power monitoring
#include "macros/cpp_defines.h"
#include "macros/macrolib.h"
#include "time_it.h"
#include "parallel_util.h"
#include "matrix_util.h"
#include "monitoring/power/rapl.h"
///////////////////////////////////////////

//---------------------------------------------------------------------
// Globals, constants, and type declarations
//---------------------------------------------------------------------

bool                    g_quiet             = false;        // Whether to display stats in CSV format
bool                    g_verbose           = false;        // Whether to display output to console
bool                    g_verbose2          = false;        // Whether to display input to console
int                     g_omp_threads       = -1;           // Number of openMP threads
int                     g_expected_calls    = 1000000;

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

// To integrate this code with the artificial matrix generator (That uses ValueType as keyword for double datatype of nonzero elements) 
// I had to change originally used ValueType typenames with ValueT (both in template and inside the struct/class of CountingInputIterator)

template <
	typename ValueT = ValueType,
	typename OffsetT = ptrdiff_t>
struct CountingInputIterator
{
	// Required iterator traits
	typedef CountingInputIterator               self_type;              ///< My own type
	typedef OffsetT                             difference_type;        ///< Type to express the result of subtracting one iterator from another
	typedef ValueT                           value_type;             ///< The type of the element the iterator can point to
	typedef ValueT*                          pointer;                ///< The type of a pointer to an element the iterator can point to
	typedef ValueT                           reference;              ///< The type of a reference to an element the iterator can point to
	typedef std::random_access_iterator_tag     iterator_category;      ///< The iterator category

	ValueT val;

	/// Constructor
	inline CountingInputIterator(
		const ValueT &val)          ///< Starting value for the iterator instance to report
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
	typename OffsetT,
	typename CoordinateT>
inline void MergePathSearch(
	OffsetT         diagonal,           ///< [in]The diagonal to search
	AIteratorT      a,                  ///< [in]List A
	BIteratorT      b,                  ///< [in]List B
	OffsetT         a_len,              ///< [in]Length of A
	OffsetT         b_len,              ///< [in]Length of B
	CoordinateT&    path_coordinate)    ///< [out] (x,y) coordinate where diagonal intersects the merge path
{
	OffsetT x_min = std::max(diagonal - b_len, 0);
	OffsetT x_max = std::min(diagonal, a_len);

	while (x_min < x_max)
	{
		OffsetT x_pivot = (x_min + x_max) >> 1;
		if (a[x_pivot] <= b[diagonal - x_pivot - 1])
			x_min = x_pivot + 1;    // Contract range up A (down B)
		else
			x_max = x_pivot;        // Contract range down A (up B)
	}

	path_coordinate.x = std::min(x_min, a_len);
	path_coordinate.y = diagonal - x_min;
}



//---------------------------------------------------------------------
// SpMV verification
//---------------------------------------------------------------------

// Compute reference SpMV y = Ax
template <
	typename ValueT,
	typename OffsetT>
void SpmvGold(
	CsrMatrix<ValueT, OffsetT>&     a,
	ValueT*                         vector_x,
	ValueT*                         vector_y_in,
	ValueT*                         vector_y_out,
	ValueT                          alpha,
	ValueT                          beta)
{
	for (OffsetT row = 0; row < a.num_rows; ++row)
	{
		ValueT partial = beta * vector_y_in[row];
		for (
			OffsetT offset = a.row_offsets[row];
			offset < a.row_offsets[row + 1];
			++offset)
		{
			partial += alpha * a.values[offset] * vector_x[a.column_indices[offset]];
		}
		vector_y_out[row] = partial;
	}
}



//---------------------------------------------------------------------
// CPU merge-based SpMV
//---------------------------------------------------------------------


/**
 * OpenMP CPU merge-based SpMV
 */
template <
	typename ValueT,
	typename OffsetT>
void OmpMergeCsrmv(
	int                             num_threads,
	CsrMatrix<ValueT, OffsetT>&     a,
	OffsetT*    __restrict        row_end_offsets,    ///< Merge list A (row end-offsets)
	OffsetT*    __restrict        column_indices,
	ValueT*     __restrict        values,
	ValueT*     __restrict        vector_x,
	ValueT*     __restrict        vector_y_out)
{
	// Temporary storage for inter-thread fix-up after load-balanced work
	OffsetT     row_carry_out[256];     // The last row-id each worked on by each thread when it finished its path segment
	ValueT      value_carry_out[256];   // The running total within each thread when it finished its path segment

	#pragma omp parallel for schedule(static) num_threads(num_threads)
	for (int tid = 0; tid < num_threads; tid++)
	{
		// Merge list B (NZ indices)
		CountingInputIterator<OffsetT>  nonzero_indices(0);

		OffsetT num_merge_items     = a.num_rows + a.num_nonzeros;                          // Merge path total length
		OffsetT items_per_thread    = (num_merge_items + num_threads - 1) / num_threads;    // Merge items per thread

		// Find starting and ending MergePath coordinates (row-idx, nonzero-idx) for each thread
		int2    thread_coord;
		int2    thread_coord_end;
		int     start_diagonal      = std::min(items_per_thread * tid, num_merge_items);
		int     end_diagonal        = std::min(start_diagonal + items_per_thread, num_merge_items);

		MergePathSearch(start_diagonal, row_end_offsets, nonzero_indices, a.num_rows, a.num_nonzeros, thread_coord);
		MergePathSearch(end_diagonal, row_end_offsets, nonzero_indices, a.num_rows, a.num_nonzeros, thread_coord_end);

		// Consume whole rows
		for (; thread_coord.x < thread_coord_end.x; ++thread_coord.x)
		{
			ValueT running_total = 0.0;
			for (; thread_coord.y < row_end_offsets[thread_coord.x]; ++thread_coord.y)
			{
				running_total += values[thread_coord.y] * vector_x[column_indices[thread_coord.y]];
			}

			vector_y_out[thread_coord.x] = running_total;
		}

		// Consume partial portion of thread's last row
		ValueT running_total = 0.0;
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
		if (row_carry_out[tid] < a.num_rows)
			vector_y_out[row_carry_out[tid]] += value_carry_out[tid];
	}
}


/**
 * Run OmpMergeCsrmv
 */
template <
	typename ValueT,
	typename OffsetT>
float TestOmpMergeCsrmv(
	CsrMatrix<ValueT, OffsetT>&     a,
	ValueT*                         vector_x,
	ValueT*                         reference_vector_y_out,
	ValueT*                         vector_y_out,
	int                             timing_iterations,
	float                           &time_warmup,
	float                           &time_afterwarmup,
	double                          &J_estimated,
	double                          &W_avg
	)
{
	CpuTimer timer;
	if (g_omp_threads == -1)
		g_omp_threads = omp_get_num_procs();
	int num_threads = g_omp_threads;

	if (!g_quiet)
		printf("\tUsing %d threads on %d procs\n", g_omp_threads, omp_get_num_procs());

	// Warmup/correctness
	memset(vector_y_out, -1, sizeof(ValueT) * a.num_rows);

	timer.Start();
	OmpMergeCsrmv(g_omp_threads, a, a.row_offsets + 1, a.column_indices, a.values, vector_x, vector_y_out);
	timer.Stop();
	time_warmup = timer.ElapsedMillis()/1000;

	if (!g_quiet)
	{
		// Check answer
		int compare = CompareResults(reference_vector_y_out, vector_y_out, a.num_rows, true);
		printf("\t%s\n", compare ? "FAIL" : "PASS"); fflush(stdout);
	}
 
	// Re-populate caches, etc.
	// Changed it to two warmups to be consistent with already implemented benchmarks
	timer.Start();
	OmpMergeCsrmv(g_omp_threads, a, a.row_offsets + 1, a.column_indices, a.values, vector_x, vector_y_out);
	timer.Stop();
	time_afterwarmup = timer.ElapsedMillis()/1000;

	/*****************************************************************************************/
	struct RAPL_Register * regs;
	long regs_n;
	char * reg_ids;

	reg_ids = NULL;
	reg_ids = (char *) "0,1"; // For Xeon(Gold1), these two are for package-0 and package-1E
	// reg_ids = (char *) "0:0";
	// reg_ids = (char *) "0,0:0";

	rapl_open(reg_ids, &regs, &regs_n);
	/*****************************************************************************************/

	// Timing
	timer.Start();
	for(int it = 0; it < timing_iterations; ++it)
	{
		rapl_read_start(regs, regs_n);

		OmpMergeCsrmv(g_omp_threads, a, a.row_offsets + 1, a.column_indices, a.values, vector_x, vector_y_out);

		rapl_read_end(regs, regs_n);
	}
	timer.Stop();

	float elapsed_time = timer.ElapsedMillis()/1000;

	/*****************************************************************************************/
	J_estimated = 0;
	for (int i=0;i<regs_n;i++){
		// printf("'%s' total joule = %g\n", regs[i].type, ((double) regs[i].uj_accum) / 1000000);
		J_estimated += ((double) regs[i].uj_accum) / 1e6;
	}
	rapl_close(regs, regs_n);
	free(regs);
	W_avg = J_estimated / elapsed_time;
	printf("J_estimated = %lf\tW_avg = %lf\n", J_estimated, W_avg);
	/*****************************************************************************************/

	return elapsed_time / timing_iterations;
}

//---------------------------------------------------------------------
// Test generation
//---------------------------------------------------------------------

/**
 * Display perf
 */
template <typename ValueT, typename OffsetT>
void DisplayPerf(
	double                          setup_time,
	double                          avg_time,
	CsrMatrix<ValueT, OffsetT>&     csr_matr)
{
	double nz_throughput, effective_bandwidth;
	size_t total_bytes = (csr_matr.num_nonzeros * (sizeof(ValueT) * 2 + sizeof(OffsetT))) + (csr_matr.num_rows) * (sizeof(OffsetT) + sizeof(ValueT));

	nz_throughput       = double(csr_matr.num_nonzeros) / avg_time / 1.0e9;
	effective_bandwidth = double(total_bytes) / avg_time / 1.0e9;

	if (!g_quiet)
		printf("fp%d: %.4f setup ms, %.4f avg s, %.5f gflops, %.3lf effective GB/s\n", int(sizeof(ValueT) * 8), setup_time, avg_time, 2 * nz_throughput, effective_bandwidth);
	else
		printf("%.5f, %.5f, %.6f, %.3lf, ", setup_time, avg_time, 2 * nz_throughput, effective_bandwidth);
	fflush(stdout);
}


/**
 * Run tests
 */
template <
	typename ValueT,
	typename OffsetT>
void RunTests(
	ValueT              alpha,
	ValueT              beta,
	const std::string&  mtx_filename,
	std::vector<std::string>&  parameters,
	int                 grid2d,
	int                 grid3d,
	int                 wheel,
	int                 dense,
	int                 timing_iterations,
	CommandLineArgs&    args)
{
	// Initialize matrix in COO form
	CooMatrix<ValueT, OffsetT> coo_matrix;

	csr_matrix *csr;
	CsrMatrix<ValueT, OffsetT> csr_matr;

	double time_balance = 0;
	CpuTimer timer;

	timer.Start();
	if (!mtx_filename.empty())
	{        
		// Parse matrix market file
		coo_matrix.InitMarket(mtx_filename, 1.0, !g_quiet);

		if ((coo_matrix.num_rows == 1) || (coo_matrix.num_cols == 1) || (coo_matrix.num_nonzeros == 1))
		{
			if (!g_quiet) printf("Trivial dataset\n");
			exit(0);
		}
		printf("%s, ", mtx_filename.c_str()); fflush(stdout);

		time_balance = time_it(1,
			csr_matr.Init(coo_matrix, !g_quiet);
		);
	}
	else{
		// artificial matrix generation on the way!
		long nr_rows;
		long nr_cols;
		double avg_nnz_per_row, std_nnz_per_row;
		unsigned int seed;
		char * distribution;
		char * placement;
		double bw;
		double skew;
		double avg_num_neighbours;
		double cross_row_similarity;
		
		int cnt = 0;
		nr_rows = std::stoi(parameters[cnt++]);
		nr_cols = std::stoi(parameters[cnt++]);
		avg_nnz_per_row = std::stof(parameters[cnt++]);
		std_nnz_per_row = std::stof(parameters[cnt++]);
		distribution = (char*)parameters[cnt++].c_str();
		placement = (char*)parameters[cnt++].c_str();
		bw = std::stof(parameters[cnt++]);
		skew = std::stof(parameters[cnt++]);
		avg_num_neighbours = std::stof(parameters[cnt++]);
		cross_row_similarity = std::stof(parameters[cnt++]);
		seed = std::stoi(parameters[cnt++]);

		double time_generate = time_it(1,
			csr = artificial_matrix_generation(nr_rows, nr_cols, avg_nnz_per_row, std_nnz_per_row, distribution, seed, placement, bw, skew, avg_num_neighbours, cross_row_similarity);
		);

		fprintf(stdout, "%s,", csr->distribution);
		fprintf(stdout, "%s,", csr->placement);
		fprintf(stdout, "%u,", csr->seed);
		fprintf(stdout, "%u,", csr->nr_rows);
		fprintf(stdout, "%u,", csr->nr_cols);
		fprintf(stdout, "%u,", csr->nr_nzeros);
		fprintf(stdout, "%lf,", csr->density);
		fprintf(stdout, "%lf,", csr->mem_footprint);
		fprintf(stdout, "%s,", csr->mem_range);
		fprintf(stdout, "%lf,", csr->avg_nnz_per_row);
		fprintf(stdout, "%lf,", csr->std_nnz_per_row);
		fprintf(stdout, "%lf,", csr->avg_bw);
		fprintf(stdout, "%lf,", csr->std_bw);
		fprintf(stdout, "%lf,", csr->avg_bw_scaled);
		fprintf(stdout, "%lf,", csr->std_bw_scaled);
		fprintf(stdout, "%lf,", csr->avg_sc);
		fprintf(stdout, "%lf,", csr->std_sc);
		fprintf(stdout, "%lf,", csr->avg_sc_scaled);
		fprintf(stdout, "%lf,", csr->std_sc_scaled);
		fprintf(stdout, "%lf,", csr->skew);
		fprintf(stdout, "%lf,", csr->avg_num_neighbours);
		fprintf(stdout, "%lf\n", csr->cross_row_similarity);

		// convert csr representation of dgal to the merge-spmv CsrMatrix equivalent
		time_balance = time_it(1,
			csr_matr.InitArtificial(csr, !g_quiet);
		);
	}
	coo_matrix.Clear();
	timer.Stop();
	double setup_time = timer.ElapsedMillis()/1000;

	// Display matrix info
	csr_matr.Stats().Display(!g_quiet);
	if (!g_quiet)
	{
		// printf("\n");
		csr_matr.DisplayHistogram();
		// printf("\n");
		// if (g_verbose2)
		//     csr_matr.Display();
		// printf("\n");
	}
	fflush(stdout);

	// Determine # of timing iterations (aim to run 16 billion nonzeros through, total)
	if (timing_iterations == -1)
	{
		timing_iterations = std::min(200000ull, std::max(100ull, ((16ull << 30) / csr_matr.num_nonzeros)));
		if (!g_quiet)
			printf("\t%d timing iterations\n", timing_iterations);
	}

	// Allocate input and output vectors
	ValueT *vector_x, *vector_y_in, *reference_vector_y_out, *vector_y_out;
	vector_x                = (ValueT*) malloc(sizeof(ValueT) * csr_matr.num_cols);
	vector_y_in             = (ValueT*) malloc(sizeof(ValueT) * csr_matr.num_rows);
	reference_vector_y_out  = (ValueT*) malloc(sizeof(ValueT) * csr_matr.num_rows);
	vector_y_out            = (ValueT*) malloc(sizeof(ValueT) * csr_matr.num_rows);

	for (int col = 0; col < csr_matr.num_cols; ++col)
		vector_x[col] = 1.0;

	for (int row = 0; row < csr_matr.num_rows; ++row)
		vector_y_in[row] = 1.0;

	// Compute reference answer
	SpmvGold(csr_matr, vector_x, vector_y_in, reference_vector_y_out, alpha, beta);

	float avg_time, time_warmup, time_afterwarmup;

	// Merge SpMV
	// if (!g_quiet) printf("\n\n");
	printf("Merge CsrMV, "); fflush(stdout);
	double J_estimated, W_avg;
	avg_time = TestOmpMergeCsrmv(csr_matr, vector_x, reference_vector_y_out, vector_y_out, timing_iterations, time_warmup, time_afterwarmup, J_estimated, W_avg);
	DisplayPerf(setup_time, avg_time, csr_matr);

	double time = avg_time*timing_iterations;
	double mem_footprint = csr_matr.num_nonzeros*(sizeof(ValueT) + sizeof(OffsetT)) + (csr_matr.num_rows+1)*sizeof(OffsetT);
	double gflops = csr_matr.num_nonzeros / time * timing_iterations * 2 * 1e-9;

	if (!mtx_filename.empty())
	{
		fprintf(stderr, "%s,", mtx_filename.c_str());
		fprintf(stderr, "%d,", omp_get_max_threads());
		fprintf(stderr, "%u,", csr_matr.num_rows);
		fprintf(stderr, "%u,", csr_matr.num_cols);
		fprintf(stderr, "%u,", csr_matr.num_nonzeros);
		fprintf(stderr, "%lf,", time);
		fprintf(stderr, "%lf,", gflops);
		fprintf(stderr, "%lf,", mem_footprint/(1024*1024));
		fprintf(stderr, "%lf,", time_balance);
		fprintf(stderr, "%lf,", time_warmup);
		fprintf(stderr, "%lf\n", time_afterwarmup);
	}
	else
	{
		fprintf(stderr, "synthetic,");
		fprintf(stderr, "%s,", csr->distribution);
		fprintf(stderr, "%s,", csr->placement);
		fprintf(stderr, "%u,", csr->seed);
		fprintf(stderr, "%u,", csr->nr_rows);
		fprintf(stderr, "%u,", csr->nr_cols);
		fprintf(stderr, "%u,", csr->nr_nzeros);
		fprintf(stderr, "%lf,", csr->density);
		fprintf(stderr, "%lf,", csr->mem_footprint);
		fprintf(stderr, "%s,", csr->mem_range);
		fprintf(stderr, "%lf,", csr->avg_nnz_per_row);
		fprintf(stderr, "%lf,", csr->std_nnz_per_row);
		fprintf(stderr, "%lf,", csr->avg_bw);
		fprintf(stderr, "%lf,", csr->std_bw);
		fprintf(stderr, "%lf,", csr->avg_bw_scaled);
		fprintf(stderr, "%lf,", csr->std_bw_scaled);
		fprintf(stderr, "%lf,", csr->avg_sc);
		fprintf(stderr, "%lf,", csr->std_sc);
		fprintf(stderr, "%lf,", csr->avg_sc_scaled);
		fprintf(stderr, "%lf,", csr->std_sc_scaled);
		fprintf(stderr, "%lf,", csr->skew);
		fprintf(stderr, "%lf,", csr->avg_num_neighbours);
		fprintf(stderr, "%lf,", csr->cross_row_similarity);
		fprintf(stderr, "merge-spmv,");
		fprintf(stderr, "%lf,", time);
		fprintf(stderr, "%lf,", gflops);
		fprintf(stderr, "%lf,", W_avg);
		fprintf(stderr, "%lf\n", J_estimated);

		// if artificial matrix, need to free allocated memory for csr_matrix
		free_csr_matrix(csr);
	}

	// Cleanup
	if (vector_x)                   free(vector_x);
	if (vector_y_in)                free(vector_y_in);
	if (reference_vector_y_out)     free(reference_vector_y_out);
	if (vector_y_out)               free(vector_y_out);
}

/**
 * Main
 */
int main(int argc, char **argv)
{
	// Initialize command line
	CommandLineArgs args(argc, argv);
	if (args.CheckCmdLineFlag("help"))
	{
		printf(
			"%s "
			"[--quiet] "
			"[--v] "
			"[--threads=<OMP threads>] "
			"[--i=<timing iterations>] "
			"[--fp64 (default) | --fp32] "
			"[--alpha=<alpha scalar (default: 1.0)>] "
			"[--beta=<beta scalar (default: 0.0)>] "
			"\n\t"
				"--mtx=<matrix market file> "
			"\n\t"
				"--param=<artificial matrix parameters> "
			"\n", argv[0]);
		exit(0);
	}

	bool                fp32;
	std::string         mtx_filename;
	std::vector<std::string> parameters(11);
	int                 grid2d              = -1;
	int                 grid3d              = -1;
	int                 wheel               = -1;
	int                 dense               = -1;
	int                 timing_iterations   = -1;
	float               alpha               = 1.0;
	float               beta                = 0.0;

	// g_verbose = args.CheckCmdLineFlag("v");
	// g_verbose2 = args.CheckCmdLineFlag("v2");
	// g_quiet = args.CheckCmdLineFlag("quiet");

	fp32 = args.CheckCmdLineFlag("fp32");
	// args.GetCmdLineArgument("i", timing_iterations);
	timing_iterations = 128;
	args.GetCmdLineArgument("mtx", mtx_filename);
	
	args.GetCmdLineArguments("param", parameters);
	// for(int i=0; i<11;i++)
	//     std::cout << "parameter[" << i << "] = " << parameters[i] << "\n";

	// args.GetCmdLineArgument("grid2d", grid2d);
	// args.GetCmdLineArgument("grid3d", grid3d);
	// args.GetCmdLineArgument("dense", dense);
	// args.GetCmdLineArgument("alpha", alpha);
	// args.GetCmdLineArgument("beta", beta);
	
	// args.GetCmdLineArgument("threads", g_omp_threads);
	g_omp_threads = omp_get_max_threads();

	// Run test(s)
	if (fp32)
		RunTests<float, int>(alpha, beta, mtx_filename, parameters, grid2d, grid3d, wheel, dense, timing_iterations, args);
	else
		RunTests<double, int>(alpha, beta, mtx_filename, parameters, grid2d, grid3d, wheel, dense, timing_iterations, args);

	return 0;
}
