#include <iostream>
#include <cmath>

#include "anonymouslib_avx2.h"

#include "mmio.h"

#include "io.h"
#include "artificial_matrix_generation.h"

#include "monitoring/power/rapl.h"

using namespace std;

#ifndef VALUE_TYPE
#define VALUE_TYPE double
#endif

#ifndef NUM_RUN
#define NUM_RUN 128
#endif

csr_matrix * AM = NULL;

int call_anonymouslib(char  *filename, int m, int n, int nnzA,
		int *csrRowPtrA, int *csrColIdxA, VALUE_TYPE *csrValA,
		VALUE_TYPE *x, VALUE_TYPE *y, VALUE_TYPE alpha)
{
	int err = 0;

	memset(y, 0, sizeof(VALUE_TYPE) * m);

	double gb = getB<int, VALUE_TYPE>(m, nnzA);
	double gflop = getFLOP<int>(nnzA);

	anonymouslibHandle<int, unsigned int, VALUE_TYPE> A(m, n);
	err = A.inputCSR(nnzA, csrRowPtrA, csrColIdxA, csrValA);
	//cout << "inputCSR err = " << err << endl;

	err = A.setX(x); // you only need to do it once!
	//cout << "setX err = " << err << endl;

	VALUE_TYPE *y_bench = (VALUE_TYPE *)malloc(m * sizeof(VALUE_TYPE));

	int sigma = ANONYMOUSLIB_CSR5_SIGMA; //nnzA/(8*ANONYMOUSLIB_CSR5_OMEGA);
	A.setSigma(sigma);

	for (int i = 0; i < 5; i++)
	{
		err = A.asCSR5();
		err = A.asCSR();
	}

	anonymouslib_timer asCSR5_timer;
	asCSR5_timer.start();

	err = A.asCSR5();

	cout << "CSR->CSR5 time = " << asCSR5_timer.stop() << " ms." << endl;
	//cout << "asCSR5 err = " << err << endl;

	// check correctness by running 1 time
	err = A.spmv(alpha, y);
	//cout << "spmv err = " << err << endl;

	if (NUM_RUN)
	{
		// warm up by running 50 times
		for (int i = 0; i < 50; i++) {
			memset(y,0,sizeof(double )*m);
			err = A.spmv(alpha, y);
		}

		long i;

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

		double CSR5Spmv_time = 0;
		anonymouslib_timer CSR5Spmv_timer;
		for (i = 0; i < NUM_RUN; i++) {
			rapl_read_start(regs, regs_n);

			CSR5Spmv_timer.start();
			err = A.spmv(alpha, y_bench);
			CSR5Spmv_time += CSR5Spmv_timer.stop();

			rapl_read_end(regs, regs_n);
		}
		// double CSR5Spmv_time = CSR5Spmv_timer.stop() / (double)NUM_RUN;
		CSR5Spmv_time /= 1000;	// msec to sec


		/*****************************************************************************************/
		double J_estimated = 0;
		for (int i=0;i<regs_n;i++){
			// printf("'%s' total joule = %g\n", regs[i].type, ((double) regs[i].uj_accum) / 1000000);
			J_estimated += ((double) regs[i].uj_accum) / 1e6;
		}
		rapl_close(regs, regs_n);
		free(regs);
		double W_avg = J_estimated / CSR5Spmv_time;
		// printf("J_estimated = %lf\tW_avg = %lf\n", J_estimated, W_avg);
		/*****************************************************************************************/

		double gflops = gflop / (1.0e+9 * CSR5Spmv_time) * NUM_RUN;
		const double mem_footprint = nnzA*(sizeof(VALUE_TYPE)+sizeof(int)) + (m+1)*sizeof(int);

		// double bw = gb/(1.0e+9 * CSR5Spmv_time) * NUM_RUN;
		// cout << "CSR5-based SpMV time = " << CSR5Spmv_time << " ms. Bandwidth = " << bw << " GB/s. GFlops = " << gflops  << " GFlops." << endl;


		if (AM != NULL)
		{
			std::cerr << "synthetic" << "," << AM->distribution << "," << AM->placement << "," << AM->seed
				<< "," << AM->nr_rows << "," << AM->nr_cols << "," << AM->nr_nzeros
				<< "," << AM->density << "," << AM->mem_footprint << "," << AM->mem_range
				<< "," << AM->avg_nnz_per_row << "," << AM->std_nnz_per_row
				<< "," << AM->avg_bw << "," << AM->std_bw
				<< "," << AM->avg_bw_scaled << "," << AM->std_bw_scaled
				<< "," << AM->avg_sc << "," << AM->std_sc
				<< "," << AM->avg_sc_scaled << "," << AM->std_sc_scaled
				<< "," << AM->skew
				<< "," << AM->avg_num_neighbours << "," << AM->cross_row_similarity
				<< "," << "CSR5" <<  "," << CSR5Spmv_time << "," << gflops << "," << W_avg << "," << J_estimated
				<< "\n";
		}
		else
		{
			std::cerr << filename << "," << omp_get_max_threads()
				<< "," << m << "," << n << "," << nnzA
				<< "," << CSR5Spmv_time << "," << gflops << "," << mem_footprint/(1024*1024)
				<< "," << W_avg << "," << J_estimated
				<< "\n";
		}
	}

	free(y_bench);

	A.destroy();

	return err;
}

int main(int argc, char ** argv)
{
	// report precision of floating-point
	cout << "------------------------------------------------------" << endl;
	char  *precision;
	if (sizeof(VALUE_TYPE) == 4)
	{
		precision = "32-bit Single Precision";
	}
	else if (sizeof(VALUE_TYPE) == 8)
	{
		precision = "64-bit Double Precision";
	}
	else
	{
		cout << "Wrong precision. Program exit!" << endl;
		return 0;
	}

	cout << "PRECISION = " << precision << endl;
	cout << "------------------------------------------------------" << endl;

	int m, n, nnzA;
	int *csrRowPtrA;
	int *csrColIdxA;
	VALUE_TYPE *csrValA;

	//ex: ./spmv webbase-1M.mtx
	int argi = 1;

	char  *filename;
	if(argc < 3)
	{
		filename = argv[argi];
		argi++;
		cout << "--------------" << filename << "--------------" << endl;

		// read matrix from mtx file
		int ret_code;
		MM_typecode matcode;
		FILE *f;

		int nnzA_mtx_report;
		int isInteger = 0, isReal = 0, isPattern = 0, isSymmetric = 0;

		// load matrix
		if ((f = fopen(filename, "r")) == NULL)
			return -1;

		if (mm_read_banner(f, &matcode) != 0)
		{
			cout << "Could not process Matrix Market banner." << endl;
			return -2;
		}

		if ( mm_is_complex( matcode ) )
		{
			cout <<"Sorry, data type 'COMPLEX' is not supported. " << endl;
			return -3;
		}

		if ( mm_is_pattern( matcode ) )  { isPattern = 1; /*cout << "type = Pattern" << endl;*/ }
		if ( mm_is_real ( matcode) )	{ isReal = 1; /*cout << "type = real" << endl;*/ }
		if ( mm_is_integer ( matcode ) ) { isInteger = 1; /*cout << "type = integer" << endl;*/ }

		/* find out size of sparse matrix .... */
		ret_code = mm_read_mtx_crd_size(f, &m, &n, &nnzA_mtx_report);
		if (ret_code != 0)
			return -4;

		if ( mm_is_symmetric( matcode ) || mm_is_hermitian( matcode ) )
		{
			isSymmetric = 1;
			//cout << "symmetric = true" << endl;
		}
		else
		{
			//cout << "symmetric = false" << endl;
		}

		int *csrRowPtrA_counter = (int *)malloc((m+1) * sizeof(int));
		memset(csrRowPtrA_counter, 0, (m+1) * sizeof(int));

		int *csrRowIdxA_tmp = (int *)malloc(nnzA_mtx_report * sizeof(int));
		int *csrColIdxA_tmp = (int *)malloc(nnzA_mtx_report * sizeof(int));
		VALUE_TYPE *csrValA_tmp	= (VALUE_TYPE *)malloc(nnzA_mtx_report * sizeof(VALUE_TYPE));

		/* NOTE: when reading in doubles, ANSI C requires the use of the "l"  */
		/*   specifier as in "%lg", "%lf", "%le", otherwise errors will occur */
		/*  (ANSI C X3.159-1989, Sec. 4.9.6.2, p. 136 lines 13-15)		  */

		for (int i = 0; i < nnzA_mtx_report; i++)
		{
			int idxi, idxj;
			double fval;
			int ival;

			if (isReal)
				fscanf(f, "%d %d %lg\n", &idxi, &idxj, &fval);
			else if (isInteger)
			{
				fscanf(f, "%d %d %d\n", &idxi, &idxj, &ival);
				fval = ival;
			}
			else if (isPattern)
			{
				fscanf(f, "%d %d\n", &idxi, &idxj);
				fval = 1.0;
			}

			// adjust from 1-based to 0-based
			idxi--;
			idxj--;

			csrRowPtrA_counter[idxi]++;
			csrRowIdxA_tmp[i] = idxi;
			csrColIdxA_tmp[i] = idxj;
			csrValA_tmp[i] = fval;
		}

		if (f != stdin)
			fclose(f);

		if (isSymmetric)
		{
			for (int i = 0; i < nnzA_mtx_report; i++)
			{
				if (csrRowIdxA_tmp[i] != csrColIdxA_tmp[i])
					csrRowPtrA_counter[csrColIdxA_tmp[i]]++;
			}
		}

		// exclusive scan for csrRowPtrA_counter
		int old_val, new_val;

		old_val = csrRowPtrA_counter[0];
		csrRowPtrA_counter[0] = 0;
		for (int i = 1; i <= m; i++)
		{
			new_val = csrRowPtrA_counter[i];
			csrRowPtrA_counter[i] = old_val + csrRowPtrA_counter[i-1];
			old_val = new_val;
		}

		nnzA = csrRowPtrA_counter[m];
		csrRowPtrA = (int *)_mm_malloc((m+1) * sizeof(int), ANONYMOUSLIB_X86_CACHELINE);
		memcpy(csrRowPtrA, csrRowPtrA_counter, (m+1) * sizeof(int));
		memset(csrRowPtrA_counter, 0, (m+1) * sizeof(int));

		csrColIdxA = (int *)_mm_malloc(nnzA * sizeof(int), ANONYMOUSLIB_X86_CACHELINE);
		csrValA	= (VALUE_TYPE *)_mm_malloc(nnzA * sizeof(VALUE_TYPE), ANONYMOUSLIB_X86_CACHELINE);

		if (isSymmetric)
		{
			for (int i = 0; i < nnzA_mtx_report; i++)
			{
				if (csrRowIdxA_tmp[i] != csrColIdxA_tmp[i])
				{
					int offset = csrRowPtrA[csrRowIdxA_tmp[i]] + csrRowPtrA_counter[csrRowIdxA_tmp[i]];
					csrColIdxA[offset] = csrColIdxA_tmp[i];
					csrValA[offset] = csrValA_tmp[i];
					csrRowPtrA_counter[csrRowIdxA_tmp[i]]++;

					offset = csrRowPtrA[csrColIdxA_tmp[i]] + csrRowPtrA_counter[csrColIdxA_tmp[i]];
					csrColIdxA[offset] = csrRowIdxA_tmp[i];
					csrValA[offset] = csrValA_tmp[i];
					csrRowPtrA_counter[csrColIdxA_tmp[i]]++;
				}
				else
				{
					int offset = csrRowPtrA[csrRowIdxA_tmp[i]] + csrRowPtrA_counter[csrRowIdxA_tmp[i]];
					csrColIdxA[offset] = csrColIdxA_tmp[i];
					csrValA[offset] = csrValA_tmp[i];
					csrRowPtrA_counter[csrRowIdxA_tmp[i]]++;
				}
			}
		}
		else
		{
			for (int i = 0; i < nnzA_mtx_report; i++)
			{
				int offset = csrRowPtrA[csrRowIdxA_tmp[i]] + csrRowPtrA_counter[csrRowIdxA_tmp[i]];
				csrColIdxA[offset] = csrColIdxA_tmp[i];
				csrValA[offset] = csrValA_tmp[i];
				csrRowPtrA_counter[csrRowIdxA_tmp[i]]++;
			}
		}

		// free tmp space
		free(csrColIdxA_tmp);
		free(csrValA_tmp);
		free(csrRowIdxA_tmp);
		free(csrRowPtrA_counter);

		srand(time(NULL));

		// set csrValA to 1, easy for checking floating-point results
		for (int i = 0; i < nnzA; i++)
		{
			csrValA[i] = rand() % 10;
		}
	}
	else
	{
		char buf[1000];

		// printf("max threads %d\n", omp_get_max_threads());
		// time = time_it(1,

			// int start_of_matrix_generation_args = 1;
			// int verbose = 1; // 0 : printf nothing
			// AM = artificial_matrix_generation(argc, argv, start_of_matrix_generation_args, verbose);
			// if (AM == NULL)
				// error("Didn't make it with the given matrix features. Try again.\n");

			long nr_rows, nr_cols, seed;
			double avg_nnz_per_row, std_nnz_per_row, bw, skew;
			double avg_num_neighbours;
			double cross_row_similarity;
			char * distribution, * placement;
			long i;

			i = 1;
			nr_rows = atoi(argv[i++]);
			nr_cols = atoi(argv[i++]);
			avg_nnz_per_row = atof(argv[i++]);
			std_nnz_per_row = atof(argv[i++]);
			distribution = argv[i++];
			placement = argv[i++];
			bw = atof(argv[i++]);
			skew = atof(argv[i++]);
			avg_num_neighbours = atof(argv[i++]);
			cross_row_similarity = atof(argv[i++]);
			seed = atoi(argv[i++]);

			AM = artificial_matrix_generation(nr_rows, nr_cols, avg_nnz_per_row, std_nnz_per_row, distribution, seed, placement, bw, skew, avg_num_neighbours, cross_row_similarity);

		// );
		// printf("time generate artificial matrix: %lf\n", time);

		m = AM->nr_rows;
		n = AM->nr_cols;
		nnzA = AM->nr_nzeros;

		csrRowPtrA = (typeof(csrRowPtrA)) malloc((m+1) * sizeof(*csrRowPtrA));
		#pragma omp parallel for
		for (long i=0;i<m+1;i++)
			csrRowPtrA[i] = AM->row_ptr[i];
		free(AM->row_ptr);
		AM->row_ptr = NULL;

		csrValA = (typeof(csrValA)) malloc(nnzA * sizeof(*csrValA));
		#pragma omp parallel for
		for (long i=0;i<nnzA;i++)
			csrValA[i] = AM->values[i];
		free(AM->values);
		AM->values = NULL;

		csrColIdxA = (typeof(csrColIdxA)) malloc(nnzA * sizeof(*csrColIdxA));
		#pragma omp parallel for
		for (long i=0;i<nnzA;i++)
			csrColIdxA[i] = AM->col_ind[i];
		free(AM->col_ind);
		AM->col_ind = NULL;

		snprintf(buf, sizeof(buf), "'%d_%d_%d_%g_%g_%g_%g'", AM->nr_rows, AM->nr_cols, AM->nr_nzeros, AM->avg_bw, AM->std_bw, AM->avg_sc, AM->std_sc);
		filename = strdup(buf);
	}

	cout << " ( " << m << ", " << n << " ) nnz = " << nnzA << endl;

	VALUE_TYPE *x = (VALUE_TYPE *)_mm_malloc(n * sizeof(VALUE_TYPE), ANONYMOUSLIB_X86_CACHELINE);
	for (int i = 0; i < n; i++)
		x[i] = rand() % 10;

	VALUE_TYPE *y = (VALUE_TYPE *)_mm_malloc(m * sizeof(VALUE_TYPE), ANONYMOUSLIB_X86_CACHELINE);
	VALUE_TYPE *y_ref = (VALUE_TYPE *)_mm_malloc(m * sizeof(VALUE_TYPE), ANONYMOUSLIB_X86_CACHELINE);

	double gb = getB<int, VALUE_TYPE>(m, nnzA);
	double gflop = getFLOP<int>(nnzA);

	VALUE_TYPE alpha = 1.0;

	// compute reference results on a cpu core
	anonymouslib_timer ref_timer;
	ref_timer.start();

	int ref_iter = 1;
	for (int iter = 0; iter < ref_iter; iter++)
	{
		for (int i = 0; i < m; i++)
		{
			VALUE_TYPE sum = 0;
			for (int j = csrRowPtrA[i]; j < csrRowPtrA[i+1]; j++)
				sum += x[csrColIdxA[j]] * csrValA[j] * alpha;
			y_ref[i] = sum;
		}
	}

	double ref_time = ref_timer.stop() / (double)ref_iter;
	cout << "cpu sequential time = " << ref_time
		<< " ms. Bandwidth = " << gb/(1.0e+6 * ref_time)
		<< " GB/s. GFlops = " << gflop/(1.0e+6 * ref_time)  << " GFlops." << endl << endl;

	// launch compute
	for (long i=0;i<128;i++)
		call_anonymouslib(filename, m, n, nnzA, csrRowPtrA, csrColIdxA, csrValA, x, y, alpha);

	// compare reference and anonymouslib results
	int error_count = 0;
	for (int i = 0; i < m; i++)
		if (abs(y_ref[i] - y[i]) > 0.01 * abs(y_ref[i]))
		{
			error_count++;
			//		  cout << "ROW [ " << i << " ], NNZ SPAN: "
			//			  << csrRowPtrA[i] << " - "
			//			  << csrRowPtrA[i+1]
			//			  << "\t ref = " << y_ref[i]
			//			  << ", \t csr5 = " << y[i]
			//			  << ", \t error = " << y_ref[i] - y[i]
			//			  << endl;
			//		  break;
		}

	if (error_count == 0)
		cout << "Check... PASS!" << endl;
	else
		cout << "Check... NO PASS! #Error = " << error_count << " out of " << m << " entries." << endl;

	cout << "------------------------------------------------------" << endl;

	_mm_free(csrRowPtrA);
	_mm_free(csrColIdxA);
	_mm_free(csrValA);
	_mm_free(x);
	_mm_free(y);
	_mm_free(y_ref);

	return 0;
}

