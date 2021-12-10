///
/// \author Anastasiadis Petros (panastas@cslab.ece.ntua.gr)
///
/// \brief Input functions for .mtx
///

#include <stdio.h>
#include <stdlib.h>
#include "gpu_utils.hpp"
#include "mmio.h"
#include "spmv_utils.hpp"

#ifdef __cplusplus
extern "C"{
#endif 
#include "artificial_matrix_generation.h"
#ifdef __cplusplus
}
#endif

//From cuda 11 - cuSPARSE
#include <cuda_runtime_api.h> // cudaMalloc, cudaMemcpy, etc.
#include <cuda.h> // cudaMalloc, cudaMemcpy, etc.

void SpmvOperator::mtx_generate_host(int argc, char *argv[], int start_of_matrix_generation_args, int verbose){
    ddebug(" -> SpmvOperator::mtx_generate_host()\n");
	csr_matrix *matrix=NULL;
	// FIXME: MPAKOS generator 
	//matrix =  artificial_matrix_generation(argc, argv, start_of_matrix_generation_args, verbose);
	long nr_rows_in = atoi(argv[start_of_matrix_generation_args]);
	long nr_cols_in = atoi(argv[start_of_matrix_generation_args+1]);;
	double avg_nnz_per_row_in = strtod(argv[start_of_matrix_generation_args+2], NULL);
	double std_nnz_per_row_in = strtod(argv[start_of_matrix_generation_args+3], NULL);
	char * distribution_in = argv[start_of_matrix_generation_args+4];
	char * placement_in = argv[start_of_matrix_generation_args+5];
	double bw_scaled_in = strtod(argv[start_of_matrix_generation_args+6], NULL);
	double skew_in = strtod(argv[start_of_matrix_generation_args+7], NULL);
	unsigned int seed_in = atoi(argv[start_of_matrix_generation_args+8]);
	matrix = artificial_matrix_generation(nr_rows_in, nr_cols_in, avg_nnz_per_row_in, std_nnz_per_row_in, distribution_in, seed_in, placement_in, bw_scaled_in, skew_in);
	/*
		if(matrix!=NULL){
		if(verbose){
			fprintf(stderr, "\n================================================= MATRIX INFO =================================================\n");
			// fprintf(stderr, "matrix->Filename = %s\n",matrix->Filename);
			fprintf(stderr, "matrix->nr_nzeros = %u\n",matrix->nr_nzeros);
			fprintf(stderr, "matrix->nr_rows = %u\n",matrix->nr_rows);
			fprintf(stderr, "matrix->nr_cols = %u\n",matrix->nr_cols);
			fprintf(stderr, "matrix->density = %f\n",matrix->density);
			fprintf(stderr, "matrix->mem_footprint = %f\n",matrix->mem_footprint);
			fprintf(stderr, "matrix->avg_bw = %f\n",matrix->avg_bw);
			fprintf(stderr, "matrix->std_bw = %f\n",matrix->std_bw);
			fprintf(stderr, "matrix->avg_sc = %f\n",matrix->avg_sc);
			fprintf(stderr, "matrix->std_sc = %f\n",matrix->std_sc);
			fprintf(stderr, "matrix->time1 = %f\n",matrix->time1);
			fprintf(stderr, "matrix->time2 = %f\n",matrix->time2);
			fprintf(stderr, "\n                              =====================================================                          \n\n");
			fprintf(stderr, "row_ptr = [");
			for(unsigned int i=0; i<matrix->nr_rows+1; i++){
				if(i==10)
					break;
				fprintf(stderr, "%u ", matrix->row_ptr[i]);
			}
			fprintf(stderr, "... ]\ncol_ind = [");
			for(unsigned int i=0; i<matrix->nr_nzeros; i++){
				if(i==10)
					break;
				fprintf(stderr, "%u ", matrix->col_ind[i]);
			}
			fprintf(stderr, "... ]\nvalues  = ["); // initialized with 14 different values
			for(unsigned int i=0; i<matrix->nr_nzeros; i++){
				if(i==10)
					break;
				fprintf(stderr, "%.4f ", matrix->values[i]);
			}
			fprintf(stderr, "... ]\n");
			fprintf(stderr, "\n===============================================================================================================\n");
		}		
	}
	else
		fprintf(stderr, "Didn't make it with the given matrix features. Try again.\n");
	*/
  	m =  (int) matrix->nr_rows;
  	n =  (int) matrix->nr_cols;
  	nz = (int) matrix->nr_nzeros;
  	density =  matrix->density;
	//bytes = matrix->mem_footprint;
	avg_nz_row = matrix->avg_nnz_per_row;
	std_nz_row = matrix->std_nnz_per_row;
	avg_bandwidth = matrix->avg_bw;
	std_bandwidth = matrix->std_bw;
	avg_scattering = matrix->avg_sc;
	std_scattering = matrix->std_sc;
	strcpy(distribution, matrix->distribution);
	seed = matrix->seed;
	strcpy(placement, matrix->placement);
	avg_bw_scaled = matrix->avg_bw_scaled;
	std_bw_scaled = matrix->std_bw_scaled;
	avg_sc_scaled = matrix->avg_sc_scaled;
	std_sc_scaled = matrix->std_sc_scaled;
	skew = matrix->skew;
	A_mem_footprint = matrix->mem_footprint;
	mem_range = matrix->mem_range;
  	SpmvCsrData * csr_output = (SpmvCsrData *) malloc(sizeof(SpmvCsrData));
  	csr_output->rowPtr = (int*) matrix->row_ptr;
  	csr_output->colInd = (int*) matrix->col_ind;
  	if (std::is_same<VALUE_TYPE_AX, double>::value || std::is_same<VALUE_TYPE_AX, float>::value) csr_output->values = (VALUE_TYPE_AX*) matrix->values;
  	else{	
  		csr_output->values = (VALUE_TYPE_AX*) malloc ( nz * sizeof(VALUE_TYPE_AX));
  		for (int it=0; it < nz; it++){
  			if (std::is_same<VALUE_TYPE_AX, int8_t>::value) csr_output->values[it] = (VALUE_TYPE_AX) (((int)matrix->values[it])%256);
  			else csr_output->values[it] = (VALUE_TYPE_AX) matrix->values[it];
  		}
  	}
  	format_data = csr_output;
  	ddebug(" <- SpmvOperator::mtx_generate_host()\n");
}

void SpmvOperator::mtx_read_host(){
    ddebug(" -> SpmvOperator::mtx_read_host()\n");
    
    int nnzA;
    int *csrRowPtrA;
    int *csrColIdxA;
    VALUE_TYPE_AX *csrValA;
    
	// read matrix from mtx file
    int ret_code;
    MM_typecode matcode;
    FILE *f;

    int nnzA_mtx_report;
    int isInteger = 0, isReal = 0, isPattern = 0, isSymmetric = 0;
    // load matrix
    if ((f = fopen(mtx_name, "r")) == NULL)
        exit(1);

    if (mm_read_banner(f, &matcode) != 0)
    {
        cout << "Could not process Matrix Market banner." << endl;
        exit(2);
    }

    if ( mm_is_complex( matcode ) )
    {
        cout <<"Sorry, data type 'COMPLEX' is not supported. " << endl;
        exit(3);
    }

    if ( mm_is_pattern( matcode ) )  { isPattern = 1; cout << "type = Pattern" << endl; }
    if ( mm_is_real ( matcode) )     { isReal = 1; cout << "type = real" << endl; }
    if ( mm_is_integer ( matcode ) ) { isInteger = 1; cout << "type = integer" << endl; }

    /* find out size of sparse matrix .... */
    ret_code = mm_read_mtx_crd_size(f, &m, &n, &nnzA_mtx_report);
    if (ret_code != 0)
        exit(4);

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
    VALUE_TYPE_AX *csrValA_tmp    = (VALUE_TYPE_AX *)malloc(nnzA_mtx_report * sizeof(VALUE_TYPE_AX));

    /* NOTE: when reading in doubles, ANSI C requires the use of the "l"  */
    /*   specifier as in "%lg", "%lf", "%le", otherwise errors will occur */
    /*  (ANSI C X3.159-1989, Sec. 4.9.6.2, p. 136 lines 13-15)            */

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
  	    if (std::is_same<VALUE_TYPE_AX, int8_t>::value) csrValA_tmp[i] = (VALUE_TYPE_AX) (((int)fval)%256);
  		else csrValA_tmp[i] = (VALUE_TYPE_AX) fval;
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
    csrRowPtrA = (int *)malloc((m+1) * sizeof(int));
    memcpy(csrRowPtrA, csrRowPtrA_counter, (m+1) * sizeof(int));
    memset(csrRowPtrA_counter, 0, (m+1) * sizeof(int));

    csrColIdxA = (int *)malloc(nnzA * sizeof(int));
    csrValA    = (VALUE_TYPE_AX *)malloc(nnzA * sizeof(VALUE_TYPE_AX));

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
    //free(csrValA_tmp);
    free(csrRowIdxA_tmp);
    free(csrRowPtrA_counter);
    
    SpmvCsrData* csr_output = (SpmvCsrData *) malloc(sizeof(SpmvCsrData));
	nz = nnzA;
	mem_bytes += (nz) * sizeof(VALUE_TYPE_AX) + (2 * nz) * sizeof(int);
  	csr_output->rowPtr = csrRowPtrA;
  	csr_output->colInd = csrColIdxA;
  	csr_output->values = csrValA;
  	avg_nz_row = 0;
	std_nz_row = 0;
	avg_bandwidth = 0;
	std_bandwidth = 0;
	avg_scattering = 0;
	std_scattering = 0;
	density = 0; 
  	strcpy(distribution, "unused");
	strcpy(placement, "unused");
	A_mem_footprint = ((nz) * sizeof(VALUE_TYPE_AX) +  nz * sizeof(int) + (m+1)* sizeof(int))/ 1024.0 / 1024.0;
	mem_range = (char*) malloc (128*sizeof(char)); 
	strcpy(mem_range, "unused");
	avg_bw_scaled = 0;
	std_bw_scaled = 0;
	avg_sc_scaled = 0;
	std_sc_scaled = 0;
	skew = 0;
	seed = 0;
  	format_data = csr_output;
  	ddebug(" <- SpmvOperator::mtx_read_host()\n");
}

void SpmvOperator::mtx_read() {
  ddebug(" -> SpmvOperator::mtx_read()\n");
  massert(!format_data, "SpmvOperator::mtx_read -> format_data not empty");
  switch (mem_alloc) {
    case (SPMV_MEMTYPE_HOST):
      mtx_read_host();
      break;
    case (SPMV_MEMTYPE_DEVICE):
      mtx_read_device();
      break;
    case (SPMV_MEMTYPE_UNIFIED):
      mtx_read_uni();
      break;
    default:
      massert(0, "SpmvOperator::mtx_read -> Unreachable default reached");
      break;
  }
  ddebug(" <- SpmvOperator::mtx_read()\n");
}

void SpmvOperator::mtx_generate(int argc, char *argv[], int start_of_matrix_generation_args, int verbose) {
  ddebug(" -> SpmvOperator::mtx_generate()\n");
  massert(!format_data, "SpmvOperator::mtx_generate -> format_data not empty");
  switch (mem_alloc) {
    case (SPMV_MEMTYPE_HOST):
      mtx_generate_host(argc, argv, start_of_matrix_generation_args, verbose);
      break;
    case (SPMV_MEMTYPE_DEVICE):
      mtx_generate_device(argc, argv, start_of_matrix_generation_args, verbose);
      break;
    case (SPMV_MEMTYPE_UNIFIED):
      mtx_generate_uni(argc, argv, start_of_matrix_generation_args, verbose);
      break;
    default:
      massert(0, "SpmvOperator::mtx_generate -> Unreachable default reached");
      break;
  }
  ddebug(" <- SpmvOperator::mtx_generate()\n");
}
