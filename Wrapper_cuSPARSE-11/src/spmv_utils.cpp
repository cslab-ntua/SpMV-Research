///
/// \author Anastasiadis Petros (panastas@cslab.ece.ntua.gr)
///
/// \brief Some helpfull functions for SpMV
///

#include <time.h>
#include <cstdio>
#include <cstdlib>
#include <spmv_utils.hpp>

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

void SpmvOperator::print_op() {
  ddebug(" -> SpmvOperator::print_op()\n");
  printf("\nOperator: %s\n", mtx_name);
  printf("Bytes in memory: CPU = %lu(%lu MB), GPU = %lu(%lu MB)\n", mem_bytes,
         mem_bytes / (1024 * 1024), gpu_mem_bytes,
         gpu_mem_bytes / (1024 * 1024));

  printf("Size: (n =%d, m =%d, nz=%d)\n", n, m, nz);

  printf("Memory allocation type: ");
  switch (mem_alloc) {
    case (SPMV_MEMTYPE_HOST):
      printf("SPMV_MEMTYPE_HOST\n");
      break;
    case (SPMV_MEMTYPE_DEVICE):
      printf("SPMV_MEMTYPE_DEVICE\n");
      break;
    case (SPMV_MEMTYPE_UNIFIED):
      printf("SPMV_MEMTYPE_UNIFIED\n");
      break;
    default:
      massert(
          0, "SpmvOperator::print_op -> mem_alloc unreachable default reached");
  }

  printf("Format: ");
  switch (format) {
    case (SPMV_FORMAT_COO):
      printf("SPMV_FORMAT_COO\n");
      break;
    case (SPMV_FORMAT_CSR):
      printf("SPMV_FORMAT_CSR\n");
      break;
    case (SPMV_FORMAT_BSR):
      printf("SPMV_FORMAT_BSR\n");
      break;
    case (SPMV_FORMAT_DIA):
      printf("SPMV_FORMAT_DIA\n");
      break;
    case (SPMV_FORMAT_ELL):
      printf("SPMV_FORMAT_ELL\n");
      break;
    case (SPMV_FORMAT_HYB):
      printf("SPMV_FORMAT_HYB\n");
      break;
    default:
      massert(0,
              "SpmvOperator::print_op -> format unreachable default reached");
  }

  printf("Library: ");
  switch (lib) {
    case (SPMV_NONE):
      printf("SPMV_NONE\n");
      break;
    case (SPMV_LIBRARY_CUSPARSE):
      printf("SPMV_LIBRARY_CUSPARSE\n");
      break;
    case (SPMV_LIBRARY_OPENMP):
      printf("SPMV_LIBRARY_OPENMP\n");
      break;
    default:
      massert(0, "SpmvOperator::print_op -> lib unreachable default reached");
  }

  printf("Vectors: ");
  if (x && y)
    printf("Allocated in memory\n");
  else
    printf("Empty\n");

  printf("SpMV Struct: ");
  if (format_data)
    printf("Allocated in memory\n\n");
  else
    printf("Empty\n\n");
  ddebug(" <- SpmvOperator::print_op()\n");
}

double csecond_sp(void) {
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

template <typename VALUETYPE_AX, typename VALUETYPE_Y, typename VALUETYPE_COMP>
void spmv_csr(int *csrPtr, int *csrCol, VALUETYPE_AX *csrVal, VALUETYPE_AX *x, VALUETYPE_Y *ys,
              int m) {
  ddebug(" -> spmv_csr()\n");
  int i, j;
  for (i = 0; i < m; ++i) {
    VALUETYPE_COMP yi = 0;
    for (j = csrPtr[i]; j < csrPtr[i + 1]; j++) yi += ((VALUETYPE_COMP) csrVal[j]) * ((VALUETYPE_COMP) x[csrCol[j]]);
    ys[i] = yi;
  }
  ddebug(" <- spmv_csr()\n");
}
template void spmv_csr<double,double,double>(int *csrPtr, int *csrCol, double *csrVal, double *x, double *ys, int n);
template void spmv_csr<float,float,float>(int *csrPtr, int *csrCol, float *csrVal, float *x, float *ys, int n);
template void spmv_csr<int,int,int>(int *csrPtr, int *csrCol, int *csrVal, int *x, int *ys, int n);
template void spmv_csr<int8_t,int,int>(int *csrPtr, int *csrCol, int8_t *csrVal, int8_t *x, int *ys, int n);

template <typename VALUETYPE>
void spmv_coo(int *csrInd, int *csrCol, VALUETYPE *csrVal, VALUETYPE *x, VALUETYPE *ys,
              int nz) {
  int i;
  for (i = 0; i < nz; i++) ys[csrInd[i]] += csrVal[i] * x[csrCol[i]];
}

template void spmv_coo<double>(int *csrInd, int *csrCol, double *csrVal, double *x, double *ys, int nz);
template void spmv_coo<float>(int *csrInd, int *csrCol, float *csrVal, float *x, float *ys, int nz);


/// Checks if first n elements of two (host memory visible) vectors are equal
template <typename VALUETYPE>
int vec_equals(VALUETYPE *v1, VALUETYPE *v2, int n, VALUETYPE eps) {
  int i, k = 0;
  FILE *f = fopen("equals.out", "w");
  for (i = 0; i < n; ++i) {
    if ((v1[i] == 0 && v2[i] != 0) || ( v1[i] != 0 && fabs((v1[i] - v2[i])/v1[i]) > eps) ) {
      //cout << v1[i] << " - " << v2[i] << " = " << fabs(v1[i] - v2[i]) << endl;
      k++;
    }
  }
  fclose(f);
  return k;
}

template int vec_equals<double>(double *v1, double *v2, int n, double eps);
template int vec_equals<float>(float *v1, float *v2, int n, float eps);

/// Prints vector 'v1' after 'message'
template <typename VALUETYPE>
void vec_print(VALUETYPE *v1, int n, const char *message) {
  int i;
  printf("%s:", message);
  for (i = 0; i < n; ++i) cout << v1[i] << " ";
  printf("\n");
}

template void vec_print<double>(double *v1, int n, const char *message);
template void vec_print<float>(float *v1, int n, const char *message);

/// Copies vector 'v1' to 'v' with padding 'np'
template <typename VALUETYPE>
void vec_copy(VALUETYPE *v, VALUETYPE *v1, int n, int np) {
  int i;
#pragma omp parallel for
  for (i = 0; i < n; ++i) {
    v[i] = v1[i];
  }
#pragma omp parallel for
  for (i = n; i < n + np; ++i) {
    v[i] = 0;
  }
}

template void vec_copy<double>(double *v, double *v1, int n, int np);
template void vec_copy<float>(float *v, float *v1, int n, int np);
template void vec_copy<int>(int *v, int *v1, int n, int np);
template void vec_copy<int8_t>(int8_t *v, int8_t *v1, int n, int np);

/// Adds vector 'src' to 'dest'
template <typename VALUETYPE>
void vec_add(VALUETYPE *dest, VALUETYPE *src, int n) {
  int i;
#pragma omp parallel for
  for (i = 0; i < n; ++i) {
    dest[i] += src[i];
  }
}

template void vec_add<double>(double *dest, double *src, int n);
template void vec_add<float>(float *dest, float *src, int n);
template void vec_add<int>(int *dest, int *src, int n);
template void vec_add<int8_t>(int8_t *dest, int8_t *src, int n);

template <typename VALUETYPE>
void vec_init(VALUETYPE *v, int n, VALUETYPE val) {
  ddebug(" -> vec_init(v, n, val)\n");
  int i;
  for (i = 0; i < n; ++i) {
    v[i] = val;
  }
  ddebug(" <- vec_init(v, n, val)\n");
}
template void vec_init<double>(double *v, int n, double val);
template void vec_init<float>(float *v, int n, float val);


template <typename VALUETYPE>
void vec_init_rand(VALUETYPE *v, int n, int np) {
  ddebug(" -> vec_init_rand(v, n, np)\n");
  srand48(42);  // should only be called once
  int i;
  for (i = 0; i < n; ++i) {
    if (std::is_same<VALUETYPE, int8_t>::value) v[i] = rand() % 256;
    if (std::is_same<VALUETYPE, int32_t>::value) v[i] = rand();
    else v[i] = (VALUETYPE)drand48();
  }
  for (i = n; i < n + np; ++i) {
    v[i] = 0;
  }
  ddebug(" <- vec_init_rand(v, n, np)\n");
}

template void vec_init_rand<double>(double *v, int n, int np);
template void vec_init_rand<float>(float *v, int n, int np);
template void vec_init_rand<int8_t>(int8_t *v, int n, int np);
template void vec_init_rand<int32_t>(int32_t *v, int n, int np);

/// Check if first 'n' elements of vector 'test' are (almost) equal to 'origin'
template <typename VALUETYPE>
void check_result(VALUETYPE *test, VALUETYPE *orig, int n) {
  VALUETYPE eps;
  if (std::is_same<VALUETYPE, double>::value) eps = 0.00000001;
  else if (std::is_same<VALUETYPE, float>::value) eps = 0.0001;
  else if (std::is_same<VALUETYPE, int8_t>::value) eps = 0;
  else if (std::is_same<VALUETYPE, int32_t>::value) eps = 0;
  int i_fail = vec_equals<VALUETYPE>(test, orig, n, eps /*std::numeric_limits<VALUETYPE>::epsilon()*/);
  if (!i_fail)
    fprintf(stdout," PASSED TEST\n");
  else
    fprintf(stdout," FAILED %d times\n", i_fail);
}

template void check_result<double>(double *test, double *orig, int n);
template void check_result<float>(float *test, float *orig, int n);
template void check_result<int8_t>(int8_t *test, int8_t *orig, int n);
template void check_result<int32_t>(int32_t *test, int32_t *orig, int n);


void *SpmvOperator::spmv_data_copy_host() {
  ddebug(" -> SpmvOperator::spmv_data_copy_host()\n");
  switch (format) {
    case (SPMV_FORMAT_COO): {
      SpmvCooData *data = (SpmvCooData *)format_data;
      SpmvCooData *cp_data = (SpmvCooData *)malloc(sizeof(SpmvCooData));
      if (data->rowInd && data->colInd && data->values) {
        cp_data->rowInd = (int *)malloc(nz * sizeof(int));
        cp_data->colInd = (int *)malloc(nz * sizeof(int));

        vec_copy<int>(cp_data->rowInd, data->rowInd, nz, 0);
        vec_copy<int>(cp_data->colInd, data->colInd, nz, 0);


        cp_data->values = (VALUE_TYPE_AX*) malloc(nz * sizeof(VALUE_TYPE_AX));
        vec_copy<VALUE_TYPE_AX>(cp_data->values, data->values, nz, 0);


      } else
        debug(
            "SpmvOperator::spmv_data_copy_host -> warning... empty Spmv "
            "struct, copied nothing\n");
      ddebug(" <- SpmvOperator::spmv_data_copy_host(x_in)\n");
      return cp_data;
    }
    case (SPMV_FORMAT_CSR): {
      SpmvCsrData *data = (SpmvCsrData *)format_data;
      SpmvCsrData *cp_data = (SpmvCsrData *)malloc(sizeof(SpmvCsrData));
      if (data->rowPtr && data->colInd && data->values) {
        cp_data->rowPtr = (int *)malloc((m + 1) * sizeof(int));
        cp_data->colInd = (int *)malloc(nz * sizeof(int));

        vec_copy<int>(cp_data->rowPtr, data->rowPtr, m + 1, 0);
        vec_copy<int>(cp_data->colInd, data->colInd, nz, 0);


        cp_data->values = (VALUE_TYPE_AX*) malloc(nz * sizeof(VALUE_TYPE_AX));
        vec_copy<VALUE_TYPE_AX>(cp_data->values, data->values, nz, 0);


      } else
        debug(
            "SpmvOperator::spmv_data_copy_host -> warning... empty Spmv "
            "struct, copied nothing\n");
      ddebug(" <- SpmvOperator::spmv_data_copy_host(x_in)\n");
      return cp_data;
    }
    case (SPMV_FORMAT_BSR): {
      SpmvBsrData *data = (SpmvBsrData *)format_data;
      SpmvBsrData *cp_data = (SpmvBsrData *)malloc(sizeof(SpmvBsrData));
      const int nb = (n + data->blockDim - 1) / data->blockDim;
      // const int mb = (m + data->blockDim - 1) / data->blockDim;
      if (data->rowPtr && data->colInd && data->values) {
        cp_data->rowPtr = (int *)malloc((nb + 1) * sizeof(int));
        cp_data->colInd = (int *)malloc(data->nnzb * sizeof(int));

        vec_copy<int>(cp_data->rowPtr, data->rowPtr, nb + 1, 0);
        vec_copy<int>(cp_data->colInd, data->colInd, data->nnzb, 0);

 
        cp_data->values = (VALUE_TYPE_AX *) malloc((data->blockDim * data->blockDim) *
                                 data->nnzb * sizeof(VALUE_TYPE_AX));
        vec_copy<VALUE_TYPE_AX>(cp_data->values, data->values, (data->blockDim * data->blockDim) * data->nnzb, 0);


        cp_data->nnzb = data->nnzb;
        cp_data->blockDim = data->blockDim;
      } else
        debug(
            "SpmvOperator::spmv_data_copy_host -> warning... empty Spmv "
            "struct, copied nothing\n");
      ddebug(" <- SpmvOperator::spmv_data_copy_host(x_in)\n");
      return cp_data;
    }
    case (SPMV_FORMAT_DIA):
      massert(0,
              "SpmvOperator::spmv_data_copy_host -> SPMV_FORMAT_DIA not "
              "implemented");
    case (SPMV_FORMAT_ELL):
      massert(0,
              "SpmvOperator::spmv_data_copy_host -> SPMV_FORMAT_ELL not "
              "implemented");
    case (SPMV_FORMAT_HYB):
      massert(0,
              "SpmvOperator::spmv_data_copy_host -> SPMV_FORMAT_HYB not "
              "implemented");
    default:
      massert(0,
              "SpmvOperator::spmv_data_copy_host -> Unreacheable format "
              "default reached");
      break;
  }
  ddebug(" <- SpmvOperator::spmv_data_copy_host()\n");
  return NULL;
}

void *SpmvOperator::spmv_data_copy_numa() {
	massert(false,"SpmvOperator::spmv_data_copy_numa -> No numa please");
	return NULL;
}

void *SpmvOperator::spmv_data_subcopy_host(int *start, int *nzc, int mode) {
  ddebug(" -> SpmvOperator::spmv_data_subcopy_host()\n");
  massert(0, "SpmvOperator::spmv_data_subcopy_host -> Not implemented");
  ddebug(" <- SpmvOperator::spmv_data_subcopy_host()\n");
  return NULL;
}

void SpmvOperator::format_convert_host(SpmvFormat target_format) {
  ddebug(" -> SpmvOperator::format_convert_host(target_format)\n");
  massert(0, "SpmvOperator::format_convert_host -> Not Implemented");
  ddebug(" <- SpmvOperator::format_convert_host(target_format)\n");
}

void report_results(double timer, int flops, size_t bytes) {
  double time = timer / NR_ITER;
  double Gflops = flops / (time * 1.e9);
  double Gbytes = bytes / (time * 1.e9);
  fprintf(stdout,"%lf ms ( %.2lf Gflops/s %.2lf Gbytes/s)", 1000.0 * time, Gflops,
         Gbytes);
}

void report_bandwidth(double timer, size_t bytes) {
  double time = timer / NR_ITER;
  double Gbytes = bytes / (time * 1.e9);
  fprintf(stdout,"%lf ms ( %.2lf Gbytes/s)", 1000.0 * time, Gbytes);
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

void serial_spmv(SpmvOperator &op) {
  SpmvCsrData *data = (SpmvCsrData *)op.format_data;
  spmv_csr<double,double,double>(data->rowPtr, data->colInd, (double *)data->values, (double *)op.x,
           (double *)op.y, op.n);
}

double *conjugate_gradient_serial(SpmvOperator &op, double *b, double *x,
                                  double tolerance) {
  massert(0, "I am not updated");
  double *r, *p, *AxP, alpha, beta, rt, rt_old, *timers, r_norm, tmp, break_tol,
      p_ap_dot;
  vec_print<double>(b, 10, "b");
  vec_print<double>(x, 10, "x0");

  timers = (double *)calloc(5, sizeof(double));
  r = (double *)malloc(op.n * sizeof(double));
  AxP = (double *)op.y;
  p = (double *)op.x;
  timers[1] = csecond() - timers[1];
  serial_waxpby(p, 1.0, x, 0.0, x, op.m);
  vec_print<double>(p, 10, "p");
  tmp = csecond();
  timers[1] = tmp - timers[1];
  timers[0] = tmp - timers[0];
  serial_spmv(op);
  vec_print<double>(AxP, 10, "AxP");
  tmp = csecond();
  timers[0] = tmp - timers[0];
  timers[1] = tmp - timers[1];
  serial_waxpby(r, 1.0, b, -1.0, AxP, op.n);
  vec_print<double>(r, 10, "r");
  tmp = csecond();
  timers[1] = tmp - timers[1];
  timers[2] = tmp - timers[2];
  rt = serial_dot_r2(r, op.n);
  r_norm = sqrt(rt);
  timers[2] = csecond() - timers[2];
  break_tol = std::numeric_limits<double>::epsilon();

#ifdef DEBUG
  std::cout << "Initial Residual = " << r_norm << std::endl;
  std::cout << "break_tol = " << break_tol << std::endl;
  std::cout << "Starting CG Solve Phase..." << std::endl;
#endif

  for (int k = 1; k <= NR_ITER && r_norm > tolerance; ++k) {
    if (k == 1) {
      timers[1] = csecond() - timers[1];
      serial_waxpby(p, 1.0, r, 0.0, r, op.n);
      vec_print<double>(p, 10, "p");
      timers[1] = csecond() - timers[1];
    } else {
      timers[2] = csecond() - timers[2];
      rt_old = rt;
      rt = serial_dot_r2(r, op.n);
      beta = rt / rt_old;
      tmp = csecond();
      timers[2] = tmp - timers[2];
      timers[1] = tmp - timers[1];
      serial_daxpby(1.0, r, beta, p, op.m);
      vec_print<double>(r, 10, "r");
      timers[1] = csecond() - timers[1];
    }

    r_norm = sqrt(rt);

    // if ((k%50==0 || k==NR_ITER)) {
    std::cout << "Iteration = " << k << "   Residual = " << r_norm
              << "   p_ap_dot = " << p_ap_dot << std::endl;
    //}

    p_ap_dot = 0;
    timers[0] = csecond() - timers[0];
    serial_spmv(op);
    vec_print<double>(AxP, 10, "AxP");
    tmp = csecond();
    timers[0] = tmp - timers[0];
    timers[2] = tmp - timers[2];
    p_ap_dot = serial_dot(AxP, p, op.m);
    timers[2] = csecond() - timers[2];

    if (p_ap_dot < break_tol) {
      debug(
          "conjugate_gradient_generic(op, b, tolerance) -> testing "
          "breakdown\n");
      if (p_ap_dot < 0 || breakdown(p_ap_dot, AxP, p, op.m)) {
        std::cout << "conjugate_gradient_generic(op, b, tolerance) -> ERROR, "
                     "numerical breakdown!"
                  << std::endl;

        // update the timers before jumping out.
        timers[4] = k;
        return timers;
      } else
        break_tol = 0.1 * p_ap_dot;
    }
    alpha = rt / p_ap_dot;
    printf("alpha=%lf\n", alpha);
    timers[1] = csecond() - timers[1];
    serial_daxpby(alpha, p, 1.0, x, op.m);
    vec_print<double>(x, 10, "x");
    serial_daxpby(-alpha, AxP, 1.0, r, op.m);
    vec_print<double>(r, 10, "r");
    timers[1] = csecond() - timers[1];
  }
  timers[4] = NR_ITER;
  return timers;
}
