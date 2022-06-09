///
/// \author Anastasiadis Petros (panastas@cslab.ece.ntua.gr)
///
/// \brief Some helpfull functions for SpMV
///

#include <time.h>

#include <stdio.h>
#include <stdlib.h>

#include <iostream>
using namespace std;

#include "mmio.h"
#include "spmv_utils.hpp"
//#include "mkl.hpp"

void ddebug(const char *message) {
#ifdef DDEBUG
#define DEBUG
  printf("%s", message);
#endif
}

void debug(const char *message) {
#ifdef DEBUG
  printf("%s", message);
#endif
}

double csecond(void) {
  ddebug(" -> csecond()\n");
  struct timespec tms;

  if (clock_gettime(CLOCK_REALTIME, &tms)) {
    return (0.0);
  }
  /// seconds, multiplied with 1 million
  int64_t micros = tms.tv_sec * 1000000;
  /// Add full microseconds
  micros += tms.tv_nsec / 1000;
  /// round up if necessary
  if (tms.tv_nsec % 1000 >= 500) {
    ++micros;
  }
  ddebug(" <- csecond()\n");
  return ((double)micros / 1000000.0);
}

void massert(bool condi, const char *msg) {
  if (!condi) {
    printf("Error: %s\n", msg);
    exit(1);
  }
}

void spmv_csr(int *csrPtr, int *csrCol, double *csrVal, double *x, double *ys,
              int n) {
  ddebug(" -> spmv_csr()\n");
  int i, j;
  for (i = 0; i < n; ++i) {
    register double yi = 0;
    for (j = csrPtr[i]; j < csrPtr[i + 1]; j++) yi += csrVal[j] * x[csrCol[j]];
    ys[i] = yi;
  }
  ddebug(" <- spmv_csr()\n");
}

void spmv_coo(int *csrInd, int *csrCol, double *csrVal, double *x, double *ys,
              int nz) {
  int i;
  for (i = 0; i < nz; i++) ys[csrInd[i]] += csrVal[i] * x[csrCol[i]];
}

void spmv_coo_f(int *csrInd, int *csrCol, float *csrVal, float *x, float *ys,
              int nz) {
  int i;
  for (i = 0; i < nz; i++) ys[csrInd[i]] += csrVal[i] * x[csrCol[i]];
}

void report_results(double timer, int flops, size_t bytes) {
  double time = timer / NUM_RUN;
  double Gflops = flops / (time * 1.e9);
  double Gbytes = bytes / (time * 1.e9);
  fprintf(stderr,"%lf ms ( %.2lf Gflops/s %.2lf Gbytes/s)", 1000.0 * time, Gflops,
         Gbytes);
}

void report_bandwidth(double timer, size_t bytes) {
  double time = timer / NUM_RUN;
  double Gbytes = bytes / (time * 1.e9);
  printf("%lf ms ( %.2lf Gbytes/s)", 1000.0 * time, Gbytes);
}

double min_elem(double *arr, int size, int *pos) {
  ddebug(" -> min_elem()\n");
  double minimum = arr[0];
  *pos = 0;
  for (int i = 1; i < size; i++)
    if (arr[i] < minimum) {
      minimum = arr[i];
      *pos = i;
    }
  ddebug(" <- min_elem()\n");
  return minimum;
}

void vec_init(double *v, int n, double val) {
  ddebug(" -> vec_init(v, n, val)\n");
  int i;
  for (i = 0; i < n; ++i) {
    v[i] = val;
  }
  ddebug(" <- vec_init(v, n, val)\n");
}

void vec_init_rand(double *v, int n, int np) {
  ddebug(" -> vec_init_rand(v, n, np)\n");
  srand48(42);  // should only be called once
  int i;
  for (i = 0; i < n; ++i) {
    v[i] = (double)drand48();
  }
  for (i = n; i < n + np; ++i) {
    v[i] = 0.0;
  }
  ddebug(" <- vec_init_rand(v, n, np)\n");
}

double serial_dot(double *v1, double *v2, int size) {
  register double sum = 0;
  for (int i = 0; i < size; i++) sum += v1[i] * v2[i];
  return sum;
}

double serial_dot_r2(double *v1, int size) {
  register double sum = 0;
  for (int i = 0; i < size; i++) sum += v1[i] * v1[i];
  return sum;
}

bool breakdown(double inner, double *v, double *w, int size) {
  ddebug(" -> breakdown(inner,v,w,size)\n");

  /// This was copied from miniFE
  // This is code that was copied from Aztec, and originally written
  // by my(his) hero, Ray Tuminaro.
  //
  // Assuming that inner = <v,w> (inner product of v and w),
  // v and w are considered orthogonal if
  //  |inner| < 100 * ||v||_2 * ||w||_2 * epsilon

  bool tmp = 0;
  double vnorm = sqrt(serial_dot(v, v, size));
  double wnorm = sqrt(serial_dot(w, w, size));
  tmp = abs(inner) <=
        100 * vnorm * wnorm * std::numeric_limits<double>::epsilon();
  if (tmp) debug("breakdown(inner,v,w,size) -> Returns breakdown\n");
  ddebug(" <- breakdown(inner,v,w,size)\n");
  return tmp;
}

void serial_waxpby(double *dest, double a, double *x, double b, double *y,
                   int size) {
  for (int i = 0; i < size; i++) dest[i] = a * x[i] + b * y[i];
}

void serial_daxpby(double a, double *x, double b, double *y, int size) {
  for (int i = 0; i < size; i++) y[i] = a * x[i] + b * y[i];
}

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

void mergeSortAux(int *X, int *Y, VALUE_TYPE *Z, int n, int *tmp_X, int *tmp_Y, VALUE_TYPE *tmp_Z){
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
   while (i<n/2) { /* finish up lower half */
      tmp_X[ti] = X[i];
      tmp_Y[ti] = Y[i];
      tmp_Z[ti] = Z[i];
      ti++; i++;
   }
   while (j<n) { /* finish up upper half */
      tmp_X[ti] = X[j];
      tmp_Y[ti] = Y[j];
      tmp_Z[ti] = Z[j];
      ti++; j++;
   }
   memcpy(X, tmp_X, n*sizeof(int));
   memcpy(Y, tmp_Y, n*sizeof(int));
   memcpy(Z, tmp_Z, n*sizeof(int));
} 

void mergeSort(int *X, int *Y, VALUE_TYPE *Z, int n, int *tmp_X, int *tmp_Y, VALUE_TYPE *tmp_Z)
{
   if (n < 2) return;

   #pragma omp task shared(X) if (n > TASK_SIZE)
   mergeSort(X, Y, Z, n/2, tmp_X, tmp_Y, tmp_Z);

   #pragma omp task shared(X) if (n > TASK_SIZE)
   mergeSort(X+(n/2), Y+(n/2), Z+(n/2), n-(n/2), tmp_X + n/2, tmp_Y + n/2, tmp_Z + n/2);

   #pragma omp taskwait
   mergeSortAux(X, Y, Z, n, tmp_X, tmp_Y, tmp_Z);
}

SpmvCsrData* mtx_read_csr(char* filename){

    int m, n, nnzA;
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
    if ((f = fopen(filename, "r")) == NULL)
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
    csr_output->m = m;
	csr_output->n = n;
	csr_output->nz = nnzA;
  	csr_output->rowPtr = csrRowPtrA;
  	csr_output->colInd = csrColIdxA;
  	csr_output->values = csrValA;
  	
  	return csr_output;
}

SpmvCooData* mtx_read_coo(char* mtx_name){
  int ret_code, nz1, *I, *J, ctr;
  VALUE_TYPE *val;
  MM_typecode matcode;
  FILE *f;
  int i;

  if ((f = fopen(mtx_name, "r")) == NULL)
    massert(0, "mtx_read_coo -> Failed to open mtx file");


  if (mm_read_banner(f, &matcode) != 0)
    massert(0,
            "mtx_read_coo -> Could not process Matrix Market "
            "banner");

  //  This is how one can screen matrix types if their application
  //  only supports a subset of the Matrix Market data types. 

  massert(mm_is_valid(matcode),
          "mtx_read_coo -> mm_is_valid(matcode) returned false");
  massert(!mm_is_complex(matcode),
          "mtx_read_coo -> Complex Matrices not supported");
  massert(mm_is_sparse(matcode),
          "mtx_read_coo -> Dense Matrices not supported");

  /* find out size of sparse matrix .... */

  SpmvCooData *data = (SpmvCooData *)malloc(sizeof(SpmvCooData));

  if ((ret_code = mm_read_mtx_crd_size(f, &data->m, &data->n, &nz1)) != 0)
    massert(0,"mtx_read_coo -> Error in finding size of mtx matrix");

  massert(data->n == data->m, "mtx_read_coo -> Only square Matrices supported in this version");

  I = (int *)malloc(nz1 * sizeof(int));
  J = (int *)malloc(nz1 * sizeof(int));
  val = (VALUE_TYPE *)malloc(nz1 * sizeof(VALUE_TYPE));
  data->nz = nz1;

  /* NOTE: when reading in doubles, ANSI C requires the use of the "l"  */
  /*   specifier as in "%lg", "%lf", "%le", otherwise errors will occur */
  /*  (ANSI C X3.159-1989, Sec. 4.9.6.2, p. 136 lines 13-15)            */

  for (i = 0; i < nz1; i++) {
    if (mm_is_pattern(matcode)) {
      fscanf(f, "%d %d\n", &(I[i]), &(J[i]));
      val[i] = 1.0;
    } else
      fscanf(f, "%d %d %lf\n", &(I[i]), &(J[i]), &(val[i]));
    if (mm_is_symmetric(matcode) && (I[i] != J[i])) data->nz++;
  }

  data->rowInd = (int *)malloc(data->nz * sizeof(int));
  data->colInd = (int *)malloc(data->nz * sizeof(int));
  data->values = malloc(data->nz * sizeof(VALUE_TYPE));
  //mem_bytes += (data->nz) * sizeof(double) + (2 * data->nz) * sizeof(int);
  VALUE_TYPE *values = (VALUE_TYPE *)data->values;

  ctr = nz1;
  for (i = 0; i < nz1; i++) {
    data->rowInd[i] = I[i];
    data->colInd[i] = J[i];
    values[i] = val[i];
    data->rowInd[i]--; /* adjust from 1-based to 0-based */
    data->colInd[i]--;
    if (mm_is_symmetric(matcode) && (data->rowInd[i] != data->colInd[i])) {
      data->rowInd[ctr] = data->colInd[i];
      data->colInd[ctr] = data->rowInd[i];
      values[ctr] = values[i];
      ctr++;
    }
  }

  if (f != stdin) fclose(f);

  int *tmp_X = (int *)malloc(data->nz * sizeof(int));
  int *tmp_Y = (int *)malloc(data->nz * sizeof(int));
  VALUE_TYPE *tmp_Z = (VALUE_TYPE *)malloc(data->nz * sizeof(VALUE_TYPE));

  if (!isArraySorted(data->rowInd, data->nz)) mergeSort(data->rowInd, data->colInd, values, data->nz - 1, tmp_X, tmp_Y, tmp_Z);
  ctr = 0;
  for (i = 1; i < data->nz; i++)
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
  // vec_print<int>(data->rowInd, data->nz, "rowInd");
  // vec_print<int>(data->colInd, data->nz, "colInd");
  // vec_print<double>((double*)data->values, data->nz, "values");
  massert(data->rowInd && data->colInd && data->values,
          "mtx_read_coo -> Format Struct Alloc failed");
  ddebug(" <- mtx_read_coo()\n");
  return data;
}

SpmvCsrData* sortedcoo2csrhost(SpmvCooData* sorted_coo_input)
{
	SpmvCsrData* csr_output = (SpmvCsrData *) malloc(sizeof(SpmvCsrData));
	csr_output->m = sorted_coo_input->m;
	csr_output->n = sorted_coo_input->n;
	csr_output->nz = sorted_coo_input->nz;
  	csr_output->rowPtr = (int *)malloc((csr_output->m + 1) * sizeof(int));
  	csr_output->colInd = (int *)malloc(csr_output->nz * sizeof(int));
  	csr_output->values = malloc(csr_output->nz * sizeof(VALUE_TYPE));

	memcpy(csr_output->colInd, sorted_coo_input->colInd, csr_output->nz * sizeof(int));
	memcpy(csr_output->values, sorted_coo_input->values, csr_output->nz * sizeof(VALUE_TYPE));
	for (int i = 0; i < csr_output->nz; i++)
		csr_output->rowPtr[sorted_coo_input->colInd[i] + 1]++;
	for (int i = 0; i < csr_output->m; i++)
    		csr_output->rowPtr[i + 1] += csr_output->rowPtr[i];

	return csr_output;
}

