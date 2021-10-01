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
#include "artificial_matrix_generation.h"

/*
int isArraySorted(int* s, int n) {
  int a = 1, i = 0;

  while (a == 1  && i < n - 1) {
	if (s[i] > s[i+1]) a = 0;
    	i++;
  }

  if (a == 1)
    return 1;
  else
    return 0;
}

void mergeSortAux(int *X, int *Y, double *Z, int n, int *tmp_X, int *tmp_Y, double *tmp_Z){
   int i = 0;
   int j = n/2;
   int ti = 0;

   while (i<n/2 && j<n) {
      if (X[i] < X[j]) {
         tmp_X[ti] = X[i];
         tmp_Y[ti] = Y[i];
         tmp_Z[ti] = Z[i];
         ti++; i++;
      } else {
         tmp_X[ti] = X[j];
         tmp_Y[ti] = Y[j];
         tmp_Z[ti] = Z[j];
         ti++; j++;
      }
   }
   while (i<n/2) { 
      tmp_X[ti] = X[i];
      tmp_Y[ti] = Y[i];
      tmp_Z[ti] = Z[i];
      ti++; i++;
   }
   while (j<n) { 
      tmp_X[ti] = X[j];
      tmp_Y[ti] = Y[j];
      tmp_Z[ti] = Z[j];
      ti++; j++;
   }
   memcpy(X, tmp_X, n*sizeof(int));
   memcpy(Y, tmp_Y, n*sizeof(int));
   memcpy(Z, tmp_Z, n*sizeof(int));
} 

void mergeSort(int *X, int *Y, double *Z, int n, int *tmp_X, int *tmp_Y, double *tmp_Z)
{
   if (n < 2) return;

   #pragma omp task shared(X) if (n > TASK_SIZE)
   mergeSort(X, Y, Z, n/2, tmp_X, tmp_Y, tmp_Z);

   #pragma omp task shared(X) if (n > TASK_SIZE)
   mergeSort(X+(n/2), Y+(n/2), Z+(n/2), n-(n/2), tmp_X + n/2, tmp_Y + n/2, tmp_Z + n/2);

   #pragma omp taskwait
   mergeSortAux(X, Y, Z, n, tmp_X, tmp_Y, tmp_Z);
}


int partition(int *a, int *b, double *c, int l, int r) {
  int pivot, i, j, t;
  double t1;
  pivot = a[l];
  i = l;
  j = r + 1;

  while (1) {
    do
      ++i;
    while (a[i] <= pivot && i <= r);
    do
      --j;
    while (a[j] > pivot);
    if (i >= j) break;
    t = a[i];
    a[i] = a[j];
    a[j] = t;
    t = b[i];
    b[i] = b[j];
    b[j] = t;
    t1 = c[i];
    c[i] = c[j];
    c[j] = t1;
  }
  t = a[l];
  a[l] = a[j];
  a[j] = t;
  t = b[l];
  b[l] = b[j];
  b[j] = t;
  t1 = c[l];
  c[l] = c[j];
  c[j] = t1;
  return j;
}

void quickSort(int *a, int *b, double *c, int l, int r) {
  int j;
  if (l < r) {  // divide and conquer
    j = partition(a, b, c, l, r);
    quickSort(a, b, c, l, j - 1);
    quickSort(a, b, c, j + 1, r);
  }
}
*/
void SpmvOperator::mtx_generate_host(){
    ddebug(" -> SpmvOperator::mtx_generate_host()\n");
	csr_matrix *matrix=NULL;
	//matrix =  artificial_matrix_generation(argc, argv, start_of_matrix_generation_args, verbose);
  	//format_data = csr_output;
  	ddebug(" <- SpmvOperator::mtx_generate_host()\n");
}

void SpmvOperator::mtx_read_host(){
    ddebug(" -> SpmvOperator::mtx_read_host()\n");
    
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

    nnzA = csrRowPtrA_counter[m];
    csrRowPtrA = (int *)malloc((m+1) * sizeof(int));
    memcpy(csrRowPtrA, csrRowPtrA_counter, (m+1) * sizeof(int));
    memset(csrRowPtrA_counter, 0, (m+1) * sizeof(int));

    csrColIdxA = (int *)malloc(nnzA * sizeof(int));
    csrValA    = (VALUE_TYPE *)malloc(nnzA * sizeof(VALUE_TYPE));

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
  	csr_output->rowPtr = csrRowPtrA;
  	csr_output->colInd = csrColIdxA;
  	csr_output->values = csrValA;
  	
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

void SpmvOperator::mtx_generate() {
  ddebug(" -> SpmvOperator::mtx_generate()\n");
  massert(!format_data, "SpmvOperator::mtx_generate -> format_data not empty");
  switch (mem_alloc) {
    case (SPMV_MEMTYPE_HOST):
      mtx_generate_host();
      break;
    case (SPMV_MEMTYPE_DEVICE):
      mtx_generate_device();
      break;
    case (SPMV_MEMTYPE_UNIFIED):
      mtx_generate_uni();
      break;
    default:
      massert(0, "SpmvOperator::mtx_generate -> Unreachable default reached");
      break;
  }
  ddebug(" <- SpmvOperator::mtx_generate()\n");
}
