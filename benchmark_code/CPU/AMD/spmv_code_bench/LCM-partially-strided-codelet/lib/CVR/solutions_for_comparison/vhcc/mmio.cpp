/* 
 *   Adapted from:
 *   Matrix Market I/O library for ANSI C
 *
 *   See http://math.nist.gov/MatrixMarket for details.
 *
 *
 */

#include <vector>
#include <algorithm>
#include <iostream>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "mem.h"
#include "mmio.h"

using namespace std;

/* Read sparse matrix from file.
   Note that the row and column entries are BASE 0 */
int mm_read_sparse_matrix(const char *fname, int *M, int *N, int *nz, int **I, int **J, double **val)
{
  FILE *f;
  MM_typecode matcode;
  int _M, _N, _nz;
  int i;
  double *_val;
  int *_I, *_J;
 
  if ((f = fopen(fname, "r")) == NULL)
    return -1;
 
  if (mm_read_banner(f, &matcode) != 0) {
    printf("mm_read_unsymetric: Could not process Matrix Market banner ");
    printf(" in file [%s]\n", fname);
    return -1;
  }
 
	if (!((mm_is_real(matcode) || mm_is_pattern(matcode) || mm_is_integer(matcode)) && mm_is_matrix(matcode) && mm_is_sparse(matcode))) {
    fprintf(stderr, "Sorry, this application does not support ");
    fprintf(stderr, "Market Market type: [%s]\n", mm_typecode_to_str(matcode));
    return -1;
  }
 
  /* find out size of sparse matrix: M, N, nz .... */
  if (mm_read_mtx_crd_size(f, &_M, &_N, &_nz) !=0) {
    fprintf(stderr, "Could not parse matrix size.\n");
    return -1;
  }
 
  /* reserve memory for matrices */
  _I = (int *)MALLOC(_nz * sizeof(int));
  _J = (int *)MALLOC(_nz * sizeof(int));
  _val = (double *)MALLOC(_nz * sizeof(double));

  _M ++;
  _N ++;

  *M = _M;
  *N = _N;
  *nz = _nz;
  *val = _val;
  *I = _I;
  *J = _J;
 
  /* NOTE: when reading in doubles, ANSI C requires the use of the "l"  */
  /*   specifier as in "%lg", "%lf", "%le", otherwise errors will occur */
  /*  (ANSI C X3.159-1989, Sec. 4.9.6.2, p. 136 lines 13-15)            */
  int min_row_idx = _M;
  int min_col_idx = _N;
  int max_row_idx = -1;
  int max_col_idx = -1;

	if (mm_is_real(matcode)) {
		for (i=0; i<_nz; i++) {
			fscanf(f, "%d %d %lg\n", &_I[i], &_J[i], &_val[i]);
//			_I[i]--;  // adjust from 1-based to 0-based 
//			_J[i]--;

			min_row_idx = _I[i] < min_row_idx ? _I[i] : min_row_idx;
			min_col_idx = _J[i] < min_col_idx ? _J[i] : min_col_idx;
			max_row_idx = _I[i] > max_row_idx ? _I[i] : max_row_idx;
			max_col_idx = _J[i] > max_col_idx ? _J[i] : max_col_idx;
		}
	} else if (mm_is_pattern(matcode)) {
		for (i=0; i<_nz; i++) {
			fscanf(f, "%d %d\n", &_I[i], &_J[i]);
//			_I[i]--;  /* adjust from 1-based to 0-based */
			_I[i];  /* adjust from 1-based to 0-based */
//			_J[i]--;
			_J[i];
			_val[i] = 1;

			min_row_idx = _I[i] < min_row_idx ? _I[i] : min_row_idx;
			min_col_idx = _J[i] < min_col_idx ? _J[i] : min_col_idx;
			max_row_idx = _I[i] > max_row_idx ? _I[i] : max_row_idx;
			max_col_idx = _J[i] > max_col_idx ? _J[i] : max_col_idx;
		}

	} else if (mm_is_integer(matcode)) {
		for (i=0; i<_nz; i++) {
			int v;
			fscanf(f, "%d %d %d\n", &_I[i], &_J[i], &v);
//			_I[i]--;  // adjust from 1-based to 0-based 
//			_J[i]--;
			_val[i] = v;

			min_row_idx = _I[i] < min_row_idx ? _I[i] : min_row_idx;
			min_col_idx = _J[i] < min_col_idx ? _J[i] : min_col_idx;
			max_row_idx = _I[i] > max_row_idx ? _I[i] : max_row_idx;
			max_col_idx = _J[i] > max_col_idx ? _J[i] : max_col_idx;
		}
	}

  fclose(f);

  /* sanity check */
  if (min_row_idx < 0) { printf("Invalid row index < 1, %d\n", min_row_idx); return -1; }
  if (min_col_idx < 0) { printf("Invalid col index < 1, %d\n", min_col_idx); return -1; }
  if (max_row_idx >= _M) { printf("Invalid row index >= num rows, %d\n", max_row_idx); return -1; }
  if (max_col_idx >= _N) { printf("Invalid col index >= num cols, %d\n", max_col_idx); return -1; }

  /* expand symmetric matrix to general */
  if (mm_is_skew(matcode)) {
    printf("Skew matrix not supported\n");
    return -1;
  } else if (mm_is_hermitian(matcode)) {
    printf("Hermitian matrix not supported\n");
    return -1;
  } else if (mm_is_symmetric(matcode)) {
    int num_off_diag = 0;
    for (i = 0; i < _nz; ++i) {
      if (_I[i] != _J[i])
				num_off_diag++;
    }
    
    int c = 0;
    int nnz = _nz + num_off_diag;
    int *newI = (int *)MALLOC(nnz * sizeof(int));
    int *newJ = (int *)MALLOC(nnz * sizeof(int));
    double *newVal = (double *)MALLOC(nnz * sizeof(double));
    for (i = 0; i < _nz; ++i) {
      newI[c] = _I[i];
      newJ[c] = _J[i];
      newVal[c] = _val[i];
      c++;

      if (_I[i] != _J[i]) {
				newI[c] = _J[i];
				newJ[c] = _I[i];
				newVal[c] = _val[i];
				c++;
      }
    }
    
    *nz = nnz;
    *I = newI;
    *J = newJ;
    *val = newVal;

    FREE(_I);
    FREE(_J);
    FREE(_val);

    _nz = nnz;
    _I = newI;
    _J = newJ;
    _val = newVal;
  }
  
  /* sort by (row,col) */
  vector<int> idx(_nz);

  for (i = 0; i < _nz; ++i) { idx[i] = i; };
  row_col_sorter rwsorter(_I, _J);

  std::cout<<" stable_sorting..........."<<std::endl;
  stable_sort(idx.begin(), idx.end(), rwsorter);
  std::cout<<"stable sorting ended........"<<std::endl;

  int *tmpI = (int *)MALLOC(_nz * sizeof(int));
  int *tmpJ = (int *)MALLOC(_nz * sizeof(int));
  double *tmpVal = (double *)MALLOC(_nz * sizeof(double));
  for (i = 0; i < _nz; ++i) {
    tmpI[i] = _I[idx[i]];
    tmpJ[i] = _J[idx[i]];
    tmpVal[i] = _val[idx[i]];
  }
  
  FREE(_I);
  FREE(_J);
  FREE(_val);
  *I = tmpI;
  *J = tmpJ;
  *val = tmpVal;

  return 0;
}

int mm_read_unsymmetric_sparse(const char *fname, int *M, int *N, int *nz, double **val, int **I, int **J)
{
  FILE *f;
  MM_typecode matcode;
  int _M, _N, _nz;
  int i;
  double *_val;
  int *_I, *_J;
 
  if ((f = fopen(fname, "r")) == NULL)
    return -1;
 
  if (mm_read_banner(f, &matcode) != 0) {
    printf("mm_read_unsymetric: Could not process Matrix Market banner ");
    printf(" in file [%s]\n", fname);
    return -1;
  }
 
  if (!(mm_is_real(matcode) && mm_is_matrix(matcode) && mm_is_sparse(matcode))) {
    fprintf(stderr, "Sorry, this application does not support ");
    fprintf(stderr, "Market Market type: [%s]\n", mm_typecode_to_str(matcode));
    return -1;
  }
 
  /* find out size of sparse matrix: M, N, nz .... */
  if (mm_read_mtx_crd_size(f, &_M, &_N, &_nz) !=0) {
    fprintf(stderr, "read_unsymmetric_sparse(): could not parse matrix size.\n");
    return -1;
  }
 
  *M = _M;
  *N = _N;
  *nz = _nz;
 
  /* reseve memory for matrices */
  _I = (int *)MALLOC(_nz * sizeof(int));
  _J = (int *)MALLOC(_nz * sizeof(int));
  _val = (double *)MALLOC(_nz * sizeof(double));
 
  *val = _val;
  *I = _I;
  *J = _J;
 
  /* NOTE: when reading in doubles, ANSI C requires the use of the "l"  */
  /*   specifier as in "%lg", "%lf", "%le", otherwise errors will occur */
  /*  (ANSI C X3.159-1989, Sec. 4.9.6.2, p. 136 lines 13-15)            */
  for (i=0; i<_nz; i++) {
    fscanf(f, "%d %d %lg\n", &_I[i], &_J[i], &_val[i]);
    _I[i]--;  /* adjust from 1-based to 0-based */
    _J[i]--;
  }
  fclose(f);
 
  return 0;
}

int mm_is_valid(MM_typecode matcode)
{
  if (!mm_is_matrix(matcode)) return 0;
  if (mm_is_dense(matcode) && mm_is_pattern(matcode)) return 0;
  if (mm_is_real(matcode) && mm_is_hermitian(matcode)) return 0;
  if (mm_is_pattern(matcode) && (mm_is_hermitian(matcode) || mm_is_skew(matcode))) return 0;
  return 1;
}

int mm_read_banner(FILE *f, MM_typecode *matcode)
{
  char line[MM_MAX_LINE_LENGTH];
  char banner[MM_MAX_TOKEN_LENGTH];
  char mtx[MM_MAX_TOKEN_LENGTH]; 
  char crd[MM_MAX_TOKEN_LENGTH];
  char data_type[MM_MAX_TOKEN_LENGTH];
  char storage_scheme[MM_MAX_TOKEN_LENGTH];
  char *p;

  mm_clear_typecode(matcode);  

  if (fgets(line, MM_MAX_LINE_LENGTH, f) == NULL)
    return MM_PREMATURE_EOF;

  if (sscanf(line, "%s %s %s %s %s", banner, mtx, crd, data_type, storage_scheme) != 5)
    return MM_PREMATURE_EOF;

  for (p=mtx; *p!='\0'; *p=tolower(*p),p++);  /* convert to lower case */
  for (p=crd; *p!='\0'; *p=tolower(*p),p++);  
  for (p=data_type; *p!='\0'; *p=tolower(*p),p++);
  for (p=storage_scheme; *p!='\0'; *p=tolower(*p),p++);

  /* check for banner */
  if (strncmp(banner, MatrixMarketBanner, strlen(MatrixMarketBanner)) != 0)
    return MM_NO_HEADER;

  /* first field should be "matrix" */
  if (strcmp(mtx, MM_MTX_STR) != 0)
    return  MM_UNSUPPORTED_TYPE;

  mm_set_matrix(matcode);

  /* second field describes whether this is a sparse matrix (in coordinate storgae) or a dense array */
  if (strcmp(crd, MM_SPARSE_STR) == 0) {
    mm_set_sparse(matcode);
  } else if (strcmp(crd, MM_DENSE_STR) == 0) {
    mm_set_dense(matcode);
  } else {
    return MM_UNSUPPORTED_TYPE;
  }
    
  /* third field */
  if (strcmp(data_type, MM_REAL_STR) == 0) {
    mm_set_real(matcode);
  } else if (strcmp(data_type, MM_COMPLEX_STR) == 0) {
    mm_set_complex(matcode);
  } else if (strcmp(data_type, MM_PATTERN_STR) == 0) {
    mm_set_pattern(matcode);
  } else if (strcmp(data_type, MM_INT_STR) == 0) {
    mm_set_integer(matcode);
  } else {
    return MM_UNSUPPORTED_TYPE;
  }

  /* fourth field */
  if (strcmp(storage_scheme, MM_GENERAL_STR) == 0) {
    mm_set_general(matcode);
  } else if (strcmp(storage_scheme, MM_SYMM_STR) == 0) {
    mm_set_symmetric(matcode);
  } else if (strcmp(storage_scheme, MM_HERM_STR) == 0) {
    mm_set_hermitian(matcode);
  } else if (strcmp(storage_scheme, MM_SKEW_STR) == 0) {
    mm_set_skew(matcode);
  } else {
    return MM_UNSUPPORTED_TYPE;
  }
        
  return 0;
}

int mm_write_mtx_crd_size(FILE *f, int M, int N, int nz)
{
  if (fprintf(f, "%d %d %d\n", M, N, nz) != 3)
    return MM_COULD_NOT_WRITE_FILE;
  else
    return 0;
}

int mm_read_mtx_crd_size(FILE *f, int *M, int *N, int *nz)
{
  char line[MM_MAX_LINE_LENGTH];
  int num_items_read;

  /* set return null parameter values, in case we exit with errors */
  *M = *N = *nz = 0;

  /* now continue scanning until you reach the end-of-comments */
  do {
    if (fgets(line,MM_MAX_LINE_LENGTH,f) == NULL)
			return MM_PREMATURE_EOF;
  } while (line[0] == '%');

  /* line[] is either blank or has M,N, nz */
  if (sscanf(line, "%d %d %d", M, N, nz) == 3) {
    return 0;
  } else {
    do { 
      num_items_read = fscanf(f, "%d %d %d", M, N, nz); 
      if (num_items_read == EOF) return MM_PREMATURE_EOF;
    } while (num_items_read != 3);
  }

  printf(" in mm_read:  m = %d, n = %d, nz = %d\n", M, N, nz);

  return 0;
}


int mm_read_mtx_array_size(FILE *f, int *M, int *N)
{
  char line[MM_MAX_LINE_LENGTH];
  int num_items_read;
  /* set return null parameter values, in case we exit with errors */
  *M = *N = 0;
	
  /* now continue scanning until you reach the end-of-comments */
  do {
    if (fgets(line,MM_MAX_LINE_LENGTH,f) == NULL)
      return MM_PREMATURE_EOF;
  } while (line[0] == '%');

  /* line[] is either blank or has M,N, nz */
  if (sscanf(line, "%d %d", M, N) == 2) {
    return 0;
        
  } else { /* we have a blank line */
    do { 
      num_items_read = fscanf(f, "%d %d", M, N); 
      if (num_items_read == EOF) return MM_PREMATURE_EOF;
    } while (num_items_read != 2);
  }

  return 0;
}

int mm_write_mtx_array_size(FILE *f, int M, int N)
{
  if (fprintf(f, "%d %d\n", M, N) != 2)
    return MM_COULD_NOT_WRITE_FILE;
  else
    return 0;
}



/*-------------------------------------------------------------------------*/

/******************************************************************/
/* use when I[], J[], and val[]J, and val[] are already allocated */
/******************************************************************/
int mm_read_mtx_crd_data(FILE *f, int M, int N, int nz, int I[], int J[], double val[], MM_typecode matcode)
{
  int i;
  if (mm_is_complex(matcode)) {
    for (i=0; i<nz; i++) {
      if (fscanf(f, "%d %d %lg %lg", &I[i], &J[i], &val[2*i], &val[2*i+1]) != 4) return MM_PREMATURE_EOF;
    }
  } else if (mm_is_real(matcode)) {
    for (i=0; i<nz; i++) {
      if (fscanf(f, "%d %d %lg\n", &I[i], &J[i], &val[i]) != 3) return MM_PREMATURE_EOF;
    }
  } else if (mm_is_pattern(matcode)) {
    for (i=0; i<nz; i++) {
			if (fscanf(f, "%d %d", &I[i], &J[i]) != 2) return MM_PREMATURE_EOF;
    }
  } else {
    return MM_UNSUPPORTED_TYPE;
  }

  return 0;
}

int mm_read_mtx_crd_entry(FILE *f, int *I, int *J, double *real, double *imag, MM_typecode matcode)
{
  if (mm_is_complex(matcode)) {
    if (fscanf(f, "%d %d %lg %lg", I, J, real, imag) != 4) return MM_PREMATURE_EOF;
  } else if (mm_is_real(matcode)) {
    if (fscanf(f, "%d %d %lg\n", I, J, real) != 3) return MM_PREMATURE_EOF;
  } else if (mm_is_pattern(matcode)) {
		if (fscanf(f, "%d %d", I, J) != 2) return MM_PREMATURE_EOF;
  } else {
    return MM_UNSUPPORTED_TYPE;
  }

  return 0;
        
}


/************************************************************************
    mm_read_mtx_crd()  fills M, N, nz, array of values, and return
                        type code, e.g. 'MCRS'

                        if matrix is complex, values[] is of size 2*nz,
                            (nz pairs of real/imaginary values)
************************************************************************/
int mm_read_mtx_crd(char *fname, int *M, int *N, int *nz, int **I, int **J, double **val, MM_typecode *matcode)
{
  int ret_code;
  FILE *f;

  if (strcmp(fname, "stdin") == 0) {
    f=stdin;
  } else {
    if ((f = fopen(fname, "r")) == NULL)
      return MM_COULD_NOT_READ_FILE;
  }

  if ((ret_code = mm_read_banner(f, matcode)) != 0)
    return ret_code;

  if (!(mm_is_valid(*matcode) && mm_is_sparse(*matcode) && mm_is_matrix(*matcode)))
    return MM_UNSUPPORTED_TYPE;

  if ((ret_code = mm_read_mtx_crd_size(f, M, N, nz)) != 0)
    return ret_code;

  *I = (int *)  MALLOC(*nz * sizeof(int));
  *J = (int *)  MALLOC(*nz * sizeof(int));
  *val = NULL;

  if (mm_is_complex(*matcode)) {
    *val = (double *) MALLOC(*nz * 2 * sizeof(double));
    ret_code = mm_read_mtx_crd_data(f, *M, *N, *nz, *I, *J, *val, *matcode);
    if (ret_code != 0)
      return ret_code;
  } else if (mm_is_real(*matcode)) {
    *val = (double *) MALLOC(*nz * sizeof(double));
    ret_code = mm_read_mtx_crd_data(f, *M, *N, *nz, *I, *J, *val, *matcode);
    if (ret_code != 0)
      return ret_code;
  } else if (mm_is_pattern(*matcode)) {
    ret_code = mm_read_mtx_crd_data(f, *M, *N, *nz, *I, *J, *val, *matcode);
    if (ret_code != 0)
      return ret_code;
  }

  if (f != stdin) 
    fclose(f);
  
  return 0;
}

int mm_write_banner(FILE *f, MM_typecode matcode)
{
  char *str = mm_typecode_to_str(matcode);
  int ret_code;

  ret_code = fprintf(f, "%s %s\n", MatrixMarketBanner, str);
  FREE(str);
  if (ret_code !=2 )
    return MM_COULD_NOT_WRITE_FILE;
  else
    return 0;
}

int mm_write_mtx_crd(char fname[], int M, int N, int nz, int I[], int J[], double val[], MM_typecode matcode)
{
  FILE *f;
  int i;

  if (strcmp(fname, "stdout") == 0) {
    f = stdout;
  } else {
    if ((f = fopen(fname, "w")) == NULL)
      return MM_COULD_NOT_WRITE_FILE;
  }
    
  /* print banner followed by typecode */
  fprintf(f, "%s ", MatrixMarketBanner);
  fprintf(f, "%s\n", mm_typecode_to_str(matcode));

  /* print matrix sizes and nonzeros */
  fprintf(f, "%d %d %d\n", M, N, nz);

  /* print values */
  if (mm_is_pattern(matcode)) {
    for (i=0; i<nz; i++) {
      fprintf(f, "%d %d\n", I[i], J[i]);
    }
  } else if (mm_is_real(matcode)) {
    for (i=0; i<nz; i++) {
      fprintf(f, "%d %d %20.16g\n", I[i], J[i], val[i]);
    }
  } else if (mm_is_complex(matcode)) {
    for (i=0; i<nz; i++) {
			fprintf(f, "%d %d %20.16g %20.16g\n", I[i], J[i], val[2*i], val[2*i+1]);
    }
  } else {
    if (f != stdout) 
      fclose(f);
    return MM_UNSUPPORTED_TYPE;
  }

  if (f !=stdout) 
    fclose(f);

  return 0;
}
  

/**
 *  Create a new copy of a string s.  mm_strdup() is a common routine, but
 *  not part of ANSI C, so it is included here.  Used by mm_typecode_to_str().
 *
 */
char *mm_strdup(const char *s)
{
  int len = strlen(s);
  char *s2 = (char *) MALLOC((len+1)*sizeof(char));
  return strcpy(s2, s);
}

char  *mm_typecode_to_str(MM_typecode matcode)
{
  char buffer[MM_MAX_LINE_LENGTH];
  char *types[4];
  char *mm_strdup(const char *);
  int error =0;

  /* check for MTX type */
  if (mm_is_matrix(matcode)) 
    types[0] = MM_MTX_STR;
  else
    error=1;

  /* check for CRD or ARR matrix */
  if (mm_is_sparse(matcode)) {
    types[1] = MM_SPARSE_STR;
  } else if (mm_is_dense(matcode)) {
    types[1] = MM_DENSE_STR;
  } else {
    return NULL;
  }

  /* check for element data type */
  if (mm_is_real(matcode)) {
    types[2] = MM_REAL_STR;
  } else if (mm_is_complex(matcode)) {
    types[2] = MM_COMPLEX_STR;
  } else if (mm_is_pattern(matcode)) {
    types[2] = MM_PATTERN_STR;
  } else if (mm_is_integer(matcode)) {
    types[2] = MM_INT_STR;
  } else {
    return NULL;
  }

  /* check for symmetry type */
  if (mm_is_general(matcode)) {
    types[3] = MM_GENERAL_STR;
  } else if (mm_is_symmetric(matcode)) {
    types[3] = MM_SYMM_STR;
  } else if (mm_is_hermitian(matcode)) {
    types[3] = MM_HERM_STR;
  } else if (mm_is_skew(matcode)) {
    types[3] = MM_SKEW_STR;
  } else {
    return NULL;
  }

  sprintf(buffer,"%s %s %s %s", types[0], types[1], types[2], types[3]);
  return mm_strdup(buffer);
}

