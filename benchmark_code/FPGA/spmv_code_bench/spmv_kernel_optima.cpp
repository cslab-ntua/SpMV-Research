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
	#include "array_metrics.h"
#ifdef __cplusplus
}
#endif

/************* OPTIMA RELATED ADDITIONS *************/
#include "optima/optima_spmv/src/oops.hpp"
#include "optima/optima_spmv/src/xcl2.hpp"

void calculate_min_max_mean_std_skew(int *arr, int n, double *min, double *max, double *avg, double *std, double *skew){
    // Initialize variables for min, max, sum, and sum of squares
    *min = (double)arr[0];
    *max = (double)arr[0];
    int sum = arr[0];
    double sum_of_squares = (double)arr[0] * (double)arr[0];

    // Loop through the array to calculate min, max, sum, and sum of squares
    for (int i = 1; i < n; i++) {
        if ((double)arr[i] < *min) {
            *min = (double)arr[i];
        }
        if ((double)arr[i] > *max) {
            *max = (double)arr[i];
        }
        sum += arr[i];
        sum_of_squares += (double)arr[i] * (double)arr[i];
    }

    // Calculate the average and standard deviation
    *avg = (double)sum / n;
    double variance = (sum_of_squares / n) - ((*avg) * (*avg));
    *std = sqrt(variance);
    *skew = ((*max) - (*avg)) / (*avg);
}

struct OPTIMAArrays : Matrix_Format
{
	INT_T * ia;      // the usual rowptr (of size m+1)
	INT_T * ja;      // the colidx of each NNZ (of size nnz)
	ValueType * a;   // the values (of size NNZ)

	long num_loops;

	/************* OPTIMA RELATED ADDITIONS *************/
	INT_T *ia_padded;
	ValueType *coef_padded;
	ColType *ja_padded;

	// INT_T m_padded;                // padded num rows
	// INT_T n_padded;                // padded num columns
	INT_T nnz_padded;              // padded num non-zeros

	INT_T *stripe_nterm;
	INT_T *stripe_nrows;
	INT_T *stripe_start_row;
	INT_T *stripe_start_index;

	int nstripe;

	// program_device related
	cl::Context context;
	cl::CommandQueue queue; // Command Queue for selected device
	cl::Program program;
	cl::Kernel *kernels;

	ValueType** _coef;
	ValueType** _y;
	ColType** _ja;

	// cl::Buffer *buffer_coef;
	// cl::Buffer *buffer_ja;
	// cl::Buffer *buffer_x;
	// cl::Buffer *buffer_y;
	cl::Buffer buffer_coef[MAX_CU];
	cl::Buffer buffer_ja[MAX_CU];
	cl::Buffer buffer_x[MAX_CU];
	cl::Buffer buffer_y[MAX_CU];

	/****************************************************/
	

	OPTIMAArrays(INT_T * ia, INT_T * ja, ValueType * a, long m, long n, long nnz) : Matrix_Format(m, n, nnz), ia(ia), ja(ja), a(a)
	{
		nstripe = atoi(getenv("CU"));

		// allocate padded buffers
		int padding_factor = 64 / sizeof(ValueType); // 64 is 512-bits, which is the width of the HBM interface
		// printf("padding_factor = %d\n", padding_factor);
		nnz_padded = nnz + m * padding_factor;
		ia_padded  = (INT_T*) OOPS_malloc((size_t)((m+1) * sizeof(INT_T)));
		ja_padded   = (ColType*) OOPS_malloc((size_t)(nnz_padded * sizeof(ColType)));
		coef_padded = (ValueType*) OOPS_malloc((size_t)(nnz_padded * sizeof(ValueType)));

		// printf("ia_padded %p\n", (void*)ia_padded);
		// printf("ja_padded %p\n",  (void*)ja_padded);
		// printf("coef_padded %p\n\n", (void*)coef_padded);

		// program_device_start(&queue, &context, &program);
		kernels = (cl::Kernel *)OOPS_malloc((size_t)(nstripe * sizeof(cl::Kernel)));
		program_device(&queue, &context, &program, kernels);

		/*******************************************************************************************/
		_coef = (ValueType**) OOPS_malloc((size_t)(nstripe * sizeof(ValueType*)));
		_y = (ValueType**) OOPS_malloc((size_t)(nstripe * sizeof(ValueType*)));
		_ja = (ColType**) OOPS_malloc((size_t)(nstripe * sizeof(ColType*)));

		stripe_nterm = (INT_T *) OOPS_malloc((size_t)(nstripe * sizeof(INT_T)));
		stripe_nrows = (INT_T *) OOPS_malloc((size_t)(nstripe * sizeof(INT_T)));
		stripe_start_row = (INT_T *) OOPS_malloc((size_t)(nstripe * sizeof(INT_T)));
		stripe_start_index = (INT_T *) OOPS_malloc((size_t)(nstripe * sizeof(INT_T)));

		// buffer_coef = (cl::Buffer *) OOPS_malloc((size_t)(nstripe * sizeof(cl::Buffer)));
		// buffer_ja = (cl::Buffer *) OOPS_malloc((size_t)(nstripe * sizeof(cl::Buffer))); 
		// buffer_x = (cl::Buffer *) OOPS_malloc((size_t)(nstripe * sizeof(cl::Buffer)));
		// buffer_y = (cl::Buffer *) OOPS_malloc((size_t)(nstripe * sizeof(cl::Buffer)));

		/*******************************************************************************************/

		/*************** THIS SHOULD MOVE FROM HERE. This is just a constructor, ffs ***************/
		double time_preproc = time_it(1,
		{
			INT_T * irow_padded = (INT_T*) OOPS_malloc((size_t)(nnz_padded * sizeof(INT_T)));
			INT_T * ja_padded_int = (INT_T*) OOPS_malloc((size_t)( nnz_padded * sizeof(INT_T))); // this is needed for padding to be more fast

			// irow_padded initialization
			for (int i = 0; i < nnz_padded; i++)
				irow_padded[i] = -1;

			// allocate scratch buffers
			int *nt_add = (int*) OOPS_malloc((size_t)(m * sizeof(int)));

			// get number of new adding for each row
			for (int i = 0; i < m; i++) {
				int nt_row = ia[i+1]-ia[i];
				int resto  = nt_row%padding_factor;
				int mult   = int(nt_row/padding_factor);
				if ( resto == 0 ) {
					nt_add[i] = 0;
				} else {
					int nt_row_p  = (mult+1)*padding_factor;
					nt_add[i] = nt_row_p - nt_row;
					// printf("nt_add %d %d %d %d %d\n",i,nt_row,resto,mult,nt_add[i]);
				}
			}

			// copy entries
			for (int i = 0; i < m; i++) {
				int nt_row = ia[i+1]-ia[i];
				int jj = ia[i]+padding_factor*i;
				for (int j = 0; j < nt_row; j++) {
					irow_padded[jj+j] = i;
					ja_padded_int[jj+j] = ja[ia[i]+j];
					coef_padded[jj+j] = a[ia[i]+j];
				}
			}

			// right padding
			for (int i = 0; i < m/2; i++) {
				int nt_row = ia[i+1]-ia[i];

				int icol = i;
				for (int j = 0; j < nt_add[i]; j++) {
					bool find_icol = true;
					while(find_icol) {
						icol++;
						if ( icol > m-1 ) {
							printf("****ERROR PAD 1****\n");
							exit(-1);
						}
						// check if icol is already in the i-row
						int pos = bin_search(icol,nt_row,&(ja[ia[i]]));
						if (icol != ja[ia[i]+pos] ) find_icol = false;
					}
					// add zero term on icol-col of i-row
					int jj = ia[i]+padding_factor*i+nt_row+j;
					irow_padded[jj] = i;
					ja_padded[jj].range(0,30)   = icol;
					coef_padded[jj] = 0.;
				}

				// sorting
				if ( nt_add[i] > 0 ) {
					int jj = ia[i]+padding_factor*i;
					heapsort_2v(&(ja_padded_int[jj]),&(coef_padded[jj]),nt_row+nt_add[i]);
				}
			}

			// left padding
			for (int i = m-1; i >= m/2; i--) {
				int nt_row = ia[i+1]-ia[i];
				int icol = i;
				for (int j = 0; j < nt_add[i]; j++) {
					bool find_icol = true;
					while(find_icol) {
						icol--;
						if ( icol > m-1 ) {
							printf("****ERROR PAD 2****\n");
							exit(-2);
						}
						// check if icol is already in the i-row
						int pos = bin_search(icol,nt_row,&(ja[ia[i]]));
						if (icol != ja[ia[i]+pos] ) find_icol = false;
					}

					// add zero term on icol-col of i-row
					int jj = ia[i]+padding_factor*i+nt_row+j;
					irow_padded[jj] = i;
					ja_padded_int[jj]   = icol;
					coef_padded[jj] = 0.;
				}
				// sorting
				if ( nt_add[i] > 0 ) {
					int jj = ia[i]+padding_factor*i;
					heapsort_2v(&(ja_padded_int[jj]),&(coef_padded[jj]),nt_row+nt_add[i]);
				}
			}

			// compaction
			INT_T ntermA_wp = 0;
			for (int i = 0; i < m; i++) {
				int nt_row = ia[i+1]-ia[i];
				int jj = ia[i]+padding_factor*i;
				for (int j = 0; j < nt_row+nt_add[i]; j++) {
					irow_padded[ntermA_wp+j] = irow_padded[jj+j];
					ja_padded_int[ntermA_wp+j]   = ja_padded_int[jj+j];
					coef_padded[ntermA_wp+j] = coef_padded[jj+j];
				}
				ntermA_wp += nt_row+nt_add[i];
			}
			free(nt_add);

			// Assembly iat for the Compressed Sparse Row (CSR) format
			// Note that the matrix is symmetric so CSR format == CSC format
			ia_padded[0] = 0;
			{
				int j = 0;
				for ( int i = 0; i < ntermA_wp; i++) {
				 ja_padded[i].range(0,30) = ja_padded_int[i];
				 if( irow_padded[i] > j ) {
						ia_padded[j+1] = i;
						ja_padded[i-1].range(31,31) = 1;
						j++;
					}
					else{
						ja_padded[i-1].range(31,31) = 0;
					}
				}
			}
			ia_padded[m] = ntermA_wp;
			ja_padded[ntermA_wp-1].range(31,31) = 1;
			nnz_padded=ntermA_wp;
			
			free(irow_padded);
			free(ja_padded_int);
		}
		);
		printf("Preprocessing (optima) time: %lf seconds\n", time_preproc);
		/*******************************************************************************************/

		// For each Compute Unit (nstripe), specify the number of elements each Compute Unit will be assigned with
		int stripesize = m/nstripe;
		int remainder  = m%nstripe;
		double time_rowbal = time_it(1,
			for (int i = 0; i < nstripe; i++) {
				// set-up stripe info
				if ( i <= remainder ) {
					stripe_nrows[i]        = stripesize + 1;
					stripe_start_row[i]    = i*stripe_nrows[i];
					stripe_start_index[i]  = ia_padded[stripe_start_row[i]];
					if ( i == remainder ) stripe_nrows[i]--;
				} else {
					stripe_nrows[i]        = stripesize;
					stripe_start_row[i]    = i*stripe_nrows[i] + remainder;
					stripe_start_index[i]  = ia_padded[stripe_start_row[i]];
				}
				stripe_nterm[i] = ia_padded[stripe_start_row[i]+stripe_nrows[i]] - stripe_start_index[i];
				printf("ROW_BAL: stripe = %d\tnrows = %d\tnterm = %d\n", i, stripe_nrows[i], stripe_nterm[i]);
			}
		);
		double nterm_min, nterm_max, nterm_avg, nterm_std, nterm_skew;
		calculate_min_max_mean_std_skew(stripe_nterm, nstripe, &nterm_min, &nterm_max, &nterm_avg, &nterm_std, &nterm_skew);

		if(nterm_std/nterm_avg > 1000){
			printf("ROW_BAL: nterm_min = %lf, nterm_max = %lf, nterm_avg = %lf, nterm_std = %lf, nterm_skew = %lf\n", nterm_min, nterm_max, nterm_avg, nterm_std, nterm_skew);
			printf("Need to do NNZ_BAL and not ROW_BAL\n");
			double time_nnzbal = time_it(1,
			{
				int stripesize2 = nnz_padded/nstripe;
				// printf("NNZ_BAL NOW\tit should be %d nonzeros per CU\n", stripesize2);
				int curr_nterm = 0;
				int curr_nrow = 0;
				int stripe_id = 0;
				stripe_start_row[0] = 0;
				stripe_start_index[0] = 0;
				for(int i=0; i<m; i++){
					// if not in last stripe
					if(stripe_id < (nstripe-1))
					{
						curr_nrow++;
						curr_nterm += (ia_padded[i+1] - ia_padded[i]); 
						if(curr_nterm > stripesize2){
							// means we have to cut-off the specific row and move to next stripe
							// next stripe will begin from next row
							// perhaps check one row forward (for better load balancing) - or cut off right before row changes (but need to go back? somehow)
							
							stripe_nrows[stripe_id] = curr_nrow;
							stripe_start_row[stripe_id+1] = i+1;
							stripe_start_index[stripe_id+1] = ia_padded[stripe_start_row[stripe_id+1]];
							// printf("stripe_id = %d\tcurr_nterm = %d\tstripe_nrows = %d\tstripe_start_row = %d\tstripe_start_index = %d\n", stripe_id, curr_nterm, stripe_nrows[stripe_id], stripe_start_row[stripe_id], stripe_start_index[stripe_id]);

							stripe_nterm[stripe_id] = ia_padded[stripe_start_row[stripe_id]+stripe_nrows[stripe_id]] - stripe_start_index[stripe_id];
							// printf("NNZ_BAL stripe = %d\tnrows = %d\tnterm = %d\n", stripe_id, stripe_nrows[stripe_id], stripe_nterm[stripe_id]);

							// reset and move on to next stripe
							curr_nrow = 0;
							curr_nterm = 0;
							stripe_id++;
						}
					}
					else{
						// we are in the last stripe
						// just get what is remaining
						// stripe_start_row will be ok
						// need to handle the rest of them
						stripe_nrows[stripe_id] = m - i;// + 1;
						stripe_nterm[stripe_id] = ia_padded[stripe_start_row[stripe_id]+stripe_nrows[stripe_id]] - stripe_start_index[stripe_id];
						printf("NNZ_BAL: stripe = %d\tnrows = %d\tnterm = %d\n", stripe_id, stripe_nrows[stripe_id], stripe_nterm[stripe_id]);
						break;
					}
				}
			}
			);
			calculate_min_max_mean_std_skew(stripe_nterm, nstripe, &nterm_min, &nterm_max, &nterm_avg, &nterm_std, &nterm_skew);
			printf("NNZ_BAL: nterm_min = %lf, nterm_max = %lf, nterm_avg = %lf, nterm_std = %lf, nterm_skew = %lf\n", nterm_min, nterm_max, nterm_avg, nterm_std, nterm_skew);
			// printf("time_rowbal vs time_nnzbal = %lf vs %lf\n", time_rowbal, time_nnzbal);
		}


		/*******************************************************************************************/

		for (int i = 0; i < nstripe; i++) {
			if(stripe_nterm[i]!=0){
				_coef[i]= (ValueType*) OOPS_malloc((size_t)(stripe_nterm[i]*sizeof(ValueType)));
				memcpy(_coef[i],coef_padded+stripe_start_index[i],stripe_nterm[i]*sizeof(ValueType));

				_ja[i]= (ColType*) OOPS_malloc((size_t)(stripe_nterm[i]*sizeof(ColType)));
				memcpy(_ja[i],ja_padded+stripe_start_index[i],stripe_nterm[i]*sizeof(ColType));

				_y[i]= (ValueType*) OOPS_malloc((size_t)(stripe_nrows[i]*sizeof(ValueType)));

				{
					/*{
						std::cout<< "ja col=[";
						// for ( int j = 0; j < stripe_nterm[i]; j++) {
						for ( int j = 0; j < 100; j++) {
							std::cout<< " " << _ja[i][j].range(0,30);
						}
						std::cout<< " ]\n\n";
					}
					{
						cout<< "ja row=[";
						for ( int j = 0; j < stripe_nterm[i]; j++) {
							std::cout<< " " << _ja[i][j].range(31,31);
						}
						std::cout<< " ]\n\n";
					}
					{
						std::cout<< "coef=[";
						// for ( int j = 0; j < stripe_nterm[i]; j++) {
						for ( int j = 0; j < 100; j++) {
							std::cout << std::setprecision(6) << std::setw(7) << " " << _coef[i][j];
						}
						std::cout<< " ]\n\n";
					}
					{
						cout<< "x=[";
						for ( int j = 0; j < nrows; j++) {
							std::cout<< " " << x[j];
						}
						std::cout<< " ]\n\n";
					}
					{
						cout<< "b=[";
						for ( int j = 0; j < stripe_nrows[i]; j++) {
							std::cout<< " " << _b[j];
						}
						std::cout<< " ]\n\n";
					}*/
				}

				// printf("_coef[%d] %p\n", i, (void*)&(_coef[i]));
				// printf("_ja[%d] %p\n", i, (void*)&(_ja[i]));
				// printf("_y[%d] %p\n\n", i, (void*)&(_y[i]));
			}
		}
	}

	~OPTIMAArrays()
	{
		free(a);
		free(ia);
		free(ja);

		free(ia_padded);
		free(ja_padded);
		free(coef_padded);

		for (int i = 0; i < nstripe; i++){
			free(_coef[i]);
			free(_y[i]);
			free(_ja[i]);
		}
		free(_coef);
		free(_y);
		free(_ja);
		
		free(stripe_nterm);
		free(stripe_nrows);
		free(stripe_start_row);
		free(stripe_start_index);

		// free(buffer_coef);
		// free(buffer_ja);
		// free(buffer_x);
		// free(buffer_y);
	}

	void spmv(ValueType * x, ValueType * y);

	void copy_to_device(ValueType * x)
	{
		cl_int err;
		for (int i = 0; i < nstripe; i++) {
			if(stripe_nterm[i]!=0){
				OCL_CHECK(err, buffer_x[i] = cl::Buffer(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,    n*sizeof(ValueType), x, &err));
				OCL_CHECK(err, buffer_y[i] = cl::Buffer(context, CL_MEM_USE_HOST_PTR | CL_MEM_WRITE_ONLY,   stripe_nrows[i]*sizeof(ValueType), _y[i], &err));
				OCL_CHECK(err, buffer_coef[i] = cl::Buffer(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, stripe_nterm[i]*sizeof(ValueType), _coef[i], &err));
				OCL_CHECK(err, buffer_ja[i]   = cl::Buffer(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, stripe_nterm[i]*sizeof(ColType), _ja[i], &err));

				printf("buffer_coef[%d] %4.3lf MB\n", i, stripe_nterm[i]*sizeof(ValueType)/(1024*1024.0));
				printf("buffer_ja[%d]   %4.3lf MB\n", i, stripe_nterm[i]*sizeof(ColType)/(1024*1024.0));
				printf("buffer_x[%d]    %4.3lf MB\n", i, n*sizeof(ValueType)/(1024*1024.0));
				printf("buffer_y[%d]    %4.3lf MB\n\n", i, stripe_nrows[i]*sizeof(ValueType)/(1024*1024.0));

				// printf("kernels[%d] %p\n", i, (void*)&(kernels[i]));
				// printf("buffer_coef[%d] %p\n", i, (void*)&(buffer_coef[i]));
				// printf("buffer_ja[%d] %p\n", i, (void*)&(buffer_ja[i]));
				// printf("buffer_x[%d] %p\n", i, (void*)&(buffer_x[i]));
				// printf("buffer_y[%d] %p\n\n", i, (void*)&(buffer_y[i]));

				// Assuming cl_double data type, which typically requires 8-byte alignment
				// size_t doubleAlignment = 8;
				// size_t uint32Alignment = 4;
				// bool aligned1 = isBufferAligned(buffer_coef[i], doubleAlignment);
				// bool aligned2 = isBufferAligned(buffer_ja[i], uint32Alignment);
				// bool aligned3 = isBufferAligned(buffer_x[i], doubleAlignment);
				// bool aligned4 = isBufferAligned(buffer_y[i], doubleAlignment);
				// std::cout << "Is the buffer_coef aligned for cl_double?  " << (aligned1 ? "Yes" : "No") << std::endl;
				// std::cout << "Is the buffer_ja   aligned for cl_uint32t? " << (aligned2 ? "Yes" : "No") << std::endl;
				// std::cout << "Is the buffer_x    aligned for cl_double?  " << (aligned3 ? "Yes" : "No") << std::endl;
				// std::cout << "Is the buffer_y    aligned for cl_double?  " << (aligned4 ? "Yes" : "No") << std::endl;
			}
		}

		//set the kernel Arguments
		int ret_setarg = 0;
		for (int i = 0; i < nstripe; i++) {
			if(stripe_nterm[i]!=0){
				int narg=0;
				printf("Kernel %d nterm %d nrows %d\n", i, stripe_nterm[i], stripe_nrows[i]);
				ret_setarg = kernels[i].setArg(narg++,atoi(getenv("LOOPS")));
				ret_setarg = kernels[i].setArg(narg++,stripe_nrows[i]);
				ret_setarg = kernels[i].setArg(narg++,stripe_nterm[i]);
				ret_setarg = kernels[i].setArg(narg++,buffer_ja[i]);
				ret_setarg = kernels[i].setArg(narg++,buffer_coef[i]);
				ret_setarg = kernels[i].setArg(narg++,buffer_x[i]);
				ret_setarg = kernels[i].setArg(narg++,buffer_y[i]);

				// https://xilinx.github.io/Vitis_Accel_Examples/2019.2/html/data_transfer.html
				// After creating buffer using Host Mem Pointer, clEnqueueMigrateMemObjects can be used for immediate migration
				// of data without considering the fact that data is actually needed or not by kernel operation.
				
				OCL_CHECK(err, err = queue.enqueueMigrateMemObjects({buffer_coef[i],buffer_ja[i],buffer_x[i]}, 0));
			}
		}
		queue.finish();
	}

	void copy_from_device(ValueType * y)
	{
		for (int i = 0; i < nstripe; i++){
			if(stripe_nterm[i]!=0){
				queue.enqueueMigrateMemObjects({buffer_y[i]},1);
			}
		}
		queue.finish();
		for (int i = 0; i < nstripe; i++){
			if(stripe_nterm[i]!=0){
				// printf("i=%d, stripe_nterm = %d\n", i, stripe_nterm[i]);
				memcpy(y+stripe_start_row[i],_y[i],stripe_nrows[i]*sizeof(ValueType));
				/*{
					std::cout<< "y=[";
					// for ( int j = 0; j < stripe_nrows[i]; j++) {
					for ( int j = stripe_start_row[i]; j < 100+stripe_start_row[i]; j++) {
						std::cout << std::setprecision(6) << std::setw(7) << " " << y[j];
					}
					std::cout<< " ]\n\n";
				}*/
			}
		}

		// After transfer of result, finalize the kernel and release everything (cl::Buffers are released automatically by destructor)
		clReleaseProgram(program.get());
		clReleaseContext(context.get());
		clReleaseCommandQueue(queue.get());
	}

	void statistics_start();
	int statistics_print(__attribute__((unused)) char * buf, __attribute__((unused)) long buf_n);
};


void compute_optima(OPTIMAArrays * restrict csr, ValueType * restrict x , ValueType * restrict y);

void
OPTIMAArrays::spmv(ValueType * x, ValueType * y)
{
	num_loops++;
	compute_optima(this, x, y);
}

struct Matrix_Format *
csr_to_format(INT_T * row_ptr, INT_T * col_ind, ValueType * values, long m, long n, long nnz, ValueType * x)
{
	struct OPTIMAArrays * optima_matrix = new OPTIMAArrays(row_ptr, col_ind, values, m, n, nnz);

	/************* OPTIMA RELATED ADDITIONS *************/
	// convert_csr_to_optima(
	// row_ptr, col_ind, values,
	// optima_matrix->ia_padded, optima_matrix->ja_padded, optima_matrix->coef_padded,
	// m, &(optima_matrix->nnz_padded));
	printf("\tnt increasing [%%]: %3.1f \n",double(optima_matrix->nnz_padded-optima_matrix->nnz)*100./double(optima_matrix->nnz));
	/****************************************************/

	optima_matrix->mem_footprint = optima_matrix->nnz_padded * (sizeof(ValueType) + sizeof(INT_T)) + (optima_matrix->m+1) * sizeof(INT_T);
	optima_matrix->format_name = (char *) "OPTIMA_SpMV";

	return optima_matrix;
}

//==========================================================================================================================================
//= Subkernels CSR
//==========================================================================================================================================

void
compute_optima(OPTIMAArrays * restrict csr, ValueType * restrict x, ValueType * restrict y)
{
	for (int i = 0; i < csr->nstripe; i++){
		if(csr->stripe_nterm[i]!=0){
			csr->queue.enqueueTask(csr->kernels[i]);
		}
	}
	csr->queue.finish();	
}

//==========================================================================================================================================
//= Print Statistics
//==========================================================================================================================================


void
OPTIMAArrays::statistics_start()
{
}


int
OPTIMAArrays::statistics_print(__attribute__((unused)) char * buf, __attribute__((unused)) long buf_n)
{
	return 0;
}

