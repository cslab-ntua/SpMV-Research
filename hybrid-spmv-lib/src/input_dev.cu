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

void SpmvOperator::mtx_read_uni(){
    ddebug(" -> SpmvOperator::mtx_read_uni()\n");
    int nnzA;
    int *csrRowPtrA;
    int *csrColIdxA;
    VALUE_TYPE *csrValA;
    
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
    VALUE_TYPE *csrValA_tmp    = (VALUE_TYPE *)malloc(nnzA_mtx_report * sizeof(VALUE_TYPE));

    /* NOTE: when reading in doubles, ANSI C requires the use of the "l"  */
    /*   specifier as in "%lg", "%lf", "%le", otherwise errors will occur */
    /*  (ANSI C X3.159-1989, Sec. 4.9.6.2, p. 136 lines 13-15)            */

    for (int i = 0; i < nnzA_mtx_report; i++)
    {
        int idxi, idxj;
        VALUE_TYPE fval;
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
	cudaMallocManaged(&csrValA, nnzA * sizeof(VALUE_TYPE));
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
	mem_bytes += (nz) * sizeof(VALUE_TYPE) + (2 * nz) * sizeof(int);
	gpu_mem_bytes += (nz) * sizeof(VALUE_TYPE) + (2 * nz) * sizeof(int);
  	csr_output->rowPtr = csrRowPtrA;
  	csr_output->colInd = csrColIdxA;
  	csr_output->values = csrValA;
  	
  	format_data = csr_output;
  	ddebug(" <- SpmvOperator::mtx_read_uni()\n");
}

/*void SpmvOperator::mtx_read_host() {
  ddebug(" -> SpmvOperator::mtx_read_host()\n");
  massert(value_type == SPMV_VALUE_TYPE_DOUBLE,
          "SpmvOperator::mtx_read_host -> only double value_type supported");
  int ret_code, nz1, *I, *J, ctr;
  double *val;
  MM_typecode matcode;
  FILE *f;
  int i;

  if ((f = fopen(mtx_name, "r")) == NULL)
    massert(0, "SpmvOperator::mtx_read_host -> Failed to open mtx file");


  if (mm_read_banner(f, &matcode) != 0)
    massert(0,
            "SpmvOperator::mtx_read_host -> Could not process Matrix Market "
            "banner");

  //  This is how one can screen matrix types if their application
  //  only supports a subset of the Matrix Market data types. 

  massert(mm_is_valid(matcode),
          "SpmvOperator::mtx_read_host -> mm_is_valid(matcode) returned false");
  massert(!mm_is_complex(matcode),
          "SpmvOperator::mtx_read_host -> Complex Matrices not supported");
  massert(mm_is_sparse(matcode),
          "SpmvOperator::mtx_read_host -> Dense Matrices not supported");


  if ((ret_code = mm_read_mtx_crd_size(f, &m, &n, &nz1)) != 0)
    massert(
        0,
        "SpmvOperator::mtx_read_host -> Error in finding size of mtx matrix");

  //massert(n == m, "SpmvOperator::mtx_read_host -> Only square Matrices supported in this version");

  I = (int *)malloc(nz1 * sizeof(int));
  J = (int *)malloc(nz1 * sizeof(int));
  val = (double *)malloc(nz1 * sizeof(double));
  nz = nz1;


  for (i = 0; i < nz1; i++) {
    if (mm_is_pattern(matcode)) {
      fscanf(f, "%d %d\n", &(I[i]), &(J[i]));
      val[i] = 1.0;
    } else
      fscanf(f, "%d %d %lf\n", &(I[i]), &(J[i]), &(val[i]));
    if (mm_is_symmetric(matcode) && (I[i] != J[i])) nz++;
  }
  SpmvCooData *data = (SpmvCooData *)malloc(sizeof(SpmvCooData));
  data->rowInd = (int *)malloc(nz * sizeof(int));
  data->colInd = (int *)malloc(nz * sizeof(int));
  data->values = malloc(nz * sizeof(double));
  mem_bytes += (nz) * sizeof(double) + (2 * nz) * sizeof(int);
  double *values = (double *)data->values;

  ctr = nz1;
  for (i = 0; i < nz1; i++) {
    data->rowInd[i] = I[i];
    data->colInd[i] = J[i];
    values[i] = val[i];
    data->rowInd[i]--;
    data->colInd[i]--;
    if (mm_is_symmetric(matcode) && (data->rowInd[i] != data->colInd[i])) {
      data->rowInd[ctr] = data->colInd[i];
      data->colInd[ctr] = data->rowInd[i];
      values[ctr] = values[i];
      ctr++;
    }
  }

  if (f != stdin) fclose(f);

  int *tmp_X = (int *)malloc(nz * sizeof(int));
  int *tmp_Y = (int *)malloc(nz * sizeof(int));
  double *tmp_Z = (double *)malloc(nz * sizeof(double));

  if (!isArraySorted(data->rowInd, nz)) mergeSort(data->rowInd, data->colInd, values, nz - 1, tmp_X, tmp_Y, tmp_Z);
  ctr = 0;
  for (i = 1; i < nz; i++)
    if (data->rowInd[i] > data->rowInd[i - 1]) {
      if (!isArraySorted(&(data->colInd[ctr]), i - ctr)) mergeSort(&(data->colInd[ctr]), &(data->rowInd[ctr]), &(values[ctr]),
                i - 1 - ctr, tmp_X, tmp_Y, tmp_Z);
      ctr = i;
    }
  if (!isArraySorted(&(data->colInd[ctr]), i - ctr)) mergeSort(&(data->colInd[ctr]), &(data->rowInd[ctr]), &(values[ctr]),
            i - 1 - ctr, tmp_X, tmp_Y, tmp_Z);
  free(I);
  free(J);
  free(val);
  // vec_print<int>(data->rowInd, nz, "rowInd");
  // vec_print<int>(data->colInd, nz, "colInd");
  // vec_print<double>((double*)data->values, nz, "values");
  massert(data->rowInd && data->colInd && data->values,
          "SpmvOperator::mtx_read_host -> Format Struct Alloc failed");
  format_data = data;
  ddebug(" <- SpmvOperator::mtx_read_host()\n");
}

void SpmvOperator::mtx_read_uni() {
  ddebug(" -> SpmvOperator::mtx_read_uni()\n");
  massert(value_type == SPMV_VALUE_TYPE_DOUBLE,
          "SpmvOperator::mtx_read_uni -> only double value_type supported");
  int ret_code, nz1, *I, *J, ctr;
  double *val;
  MM_typecode matcode;
  FILE *f;
  int i;

  if ((f = fopen(mtx_name, "r")) == NULL)
    massert(0, "SpmvOperator::mtx_read_uni -> Failed to open mtx file");

  if (mm_read_banner(f, &matcode) != 0)
    massert(
        0,
        "SpmvOperator::mtx_read_uni -> Could not process Matrix Market banner");


  massert(mm_is_valid(matcode),
          "SpmvOperator::mtx_read_host -> mm_is_valid(matcode) returned false");
  massert(!mm_is_complex(matcode),
          "SpmvOperator::mtx_read_host -> Complex Matrices not supported");
  massert(mm_is_sparse(matcode),
          "SpmvOperator::mtx_read_host -> Dense Matrices not supported");


  if ((ret_code = mm_read_mtx_crd_size(f, &m, &n, &nz1)) != 0)
    massert(
        0, "SpmvOperator::mtx_read_uni -> Error in finding size of mtx matrix");

  massert(n == m,
          "SpmvOperator::mtx_read_uni -> Only square Matrices supported in "
          "this version");

  I = (int *)malloc(nz1 * sizeof(int));
  J = (int *)malloc(nz1 * sizeof(int));
  val = (double *)malloc(nz1 * sizeof(double));
  nz = nz1;


  for (i = 0; i < nz1; i++) {
    if (mm_is_pattern(matcode)) {
      fscanf(f, "%d %d\n", &(I[i]), &(J[i]));
      val[i] = 1.0;
    } else
      fscanf(f, "%d %d %lf\n", &(I[i]), &(J[i]), &(val[i]));
    if (mm_is_symmetric(matcode) && (I[i] != J[i])) nz++;
  }
  SpmvCooData *data = (SpmvCooData *)malloc(sizeof(SpmvCooData));

  cudaMallocManaged(&data->rowInd, nz * sizeof(int));
  cudaMallocManaged(&data->colInd, nz * sizeof(int));
  cudaMallocManaged(&data->values, nz * sizeof(double));
  cudaDeviceSynchronize();
  cudaCheckErrors();
  mem_bytes += (nz) * sizeof(double) + (2 * nz) * sizeof(int);
  gpu_mem_bytes += (nz) * sizeof(double) + (2 * nz) * sizeof(int);
  double *values = (double *)data->values;

  ctr = nz1;
  for (i = 0; i < nz1; i++) {
    data->rowInd[i] = I[i];
    data->colInd[i] = J[i];
    values[i] = val[i];
    data->rowInd[i]--; 
    data->colInd[i]--;
    if (mm_is_symmetric(matcode) && (data->rowInd[i] != data->colInd[i])) {
      data->rowInd[ctr] = data->colInd[i];
      data->colInd[ctr] = data->rowInd[i];
      values[ctr] = values[i];
      ctr++;
    }
  }

  if (f != stdin) fclose(f);

  int *tmp_X = (int *)malloc(nz * sizeof(int));
  int *tmp_Y = (int *)malloc(nz * sizeof(int));
  double *tmp_Z = (double *)malloc(nz * sizeof(double));

  if (!isArraySorted(data->rowInd, nz)) mergeSort(data->rowInd, data->colInd, values, nz - 1, tmp_X, tmp_Y, tmp_Z);
  ctr = 0;
  for (i = 1; i < nz; i++)
    if (data->rowInd[i] > data->rowInd[i - 1]) {
      if (!isArraySorted(&(data->colInd[ctr]), i - ctr)) mergeSort(&(data->colInd[ctr]), &(data->rowInd[ctr]), &(values[ctr]),
                i - 1 - ctr, tmp_X, tmp_Y, tmp_Z);
      ctr = i;
    }

  free(I);
  free(J);
  free(val);
  massert(data->rowInd && data->colInd && data->values,
          "SpmvOperator::mtx_read_host -> Format Struct Alloc failed");
  format_data = data;
  ddebug(" <- SpmvOperator::mtx_read_uni()\n");
}
*/

void SpmvOperator::mtx_read_device() {
  ddebug(" -> SpmvOperator::mtx_read_device()\n");
  massert(0, "SpmvOperator::mtx_read_device -> Not implemented");
  ddebug(" <- SpmvOperator::mtx_read_device()\n");
}

void SpmvOperator::mtx_generate_device() {
  ddebug(" -> SpmvOperator::mtx_generate_device()\n");
  massert(0, "SpmvOperator::mtx_generate_device -> Not implemented");
  ddebug(" <- SpmvOperator::mtx_generate_device()\n");
}

void SpmvOperator::mtx_generate_uni() {
  ddebug(" -> SpmvOperator::mtx_generate_uni()\n");
  massert(0, "SpmvOperator::mtx_generate_uni -> Not implemented");
  ddebug(" <- SpmvOperator::mtx_generate_uni()\n");
}
