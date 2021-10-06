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

//From cuda 11 - cuSPARSE
#include <cuda_runtime_api.h> // cudaMalloc, cudaMemcpy, etc.
#include <cusparse.h>         // cusparseSpMV
#include <stdio.h>            // printf
#include <stdlib.h>           // EXIT_FAILURE

void SpmvOperator::mtx_read_uni(){
    ddebug(" -> SpmvOperator::mtx_read_uni()\n");
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

    if ( mm_is_pattern( matcode ) )  { isPattern = 1; /*cout << "type = Pattern" << endl;*/ }
    if ( mm_is_real ( matcode) )     { isReal = 1; /*cout << "type = real" << endl;*/ }
    if ( mm_is_integer ( matcode ) ) { isInteger = 1; /*cout << "type = integer" << endl;*/ }

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
        VALUE_TYPE_AX fval;
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

	cudaMallocManaged(&csrRowPtrA, (m+1) * sizeof(int));
	cudaMallocManaged(&csrColIdxA, nnzA * sizeof(int));
	cudaMallocManaged(&csrValA, nnzA * sizeof(VALUE_TYPE_AX));
	cudaDeviceSynchronize();
	cudaCheckErrors();
  
    nnzA = csrRowPtrA_counter[m];
    memcpy(csrRowPtrA, csrRowPtrA_counter, (m+1) * sizeof(int));
    memset(csrRowPtrA_counter, 0, (m+1) * sizeof(int));

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
    
    SpmvCsrData* csr_output = (SpmvCsrData *) malloc(sizeof(SpmvCsrData));
	nz = nnzA;
	mem_bytes += (nz) * sizeof(VALUE_TYPE_AX) + (2 * nz) * sizeof(int);
	gpu_mem_bytes += (nz) * sizeof(VALUE_TYPE_AX) + (2 * nz) * sizeof(int);
  	csr_output->rowPtr = csrRowPtrA;
  	csr_output->colInd = csrColIdxA;
  	csr_output->values = csrValA;
  	
  	format_data = csr_output;
  	ddebug(" <- SpmvOperator::mtx_read_uni()\n");
}

void SpmvOperator::mtx_read_device() {
  ddebug(" -> SpmvOperator::mtx_read_device()\n");
  massert(0, "SpmvOperator::mtx_read_device -> Not implemented");
  ddebug(" <- SpmvOperator::mtx_read_device()\n");
}

void SpmvOperator::mtx_generate_device(int argc, char *argv[], int start_of_matrix_generation_args, int verbose) {
  ddebug(" -> SpmvOperator::mtx_generate_device()\n");
  massert(0, "SpmvOperator::mtx_generate_device -> Not implemented");
  ddebug(" <- SpmvOperator::mtx_generate_device()\n");
}

void SpmvOperator::mtx_generate_uni(int argc, char *argv[], int start_of_matrix_generation_args, int verbose) {
  ddebug(" -> SpmvOperator::mtx_generate_uni()\n");
  massert(0, "SpmvOperator::mtx_generate_uni -> Not implemented");
  ddebug(" <- SpmvOperator::mtx_generate_uni()\n");
}
