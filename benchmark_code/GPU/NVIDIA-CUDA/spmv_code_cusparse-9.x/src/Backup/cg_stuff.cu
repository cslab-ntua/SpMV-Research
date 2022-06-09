///
/// \author Anastasiadis Petros (panastas@cslab.ece.ntua.gr)
///
///

#include <cstdio>
#include "cuSP.hpp"
#include "cuSPARSE.hpp"
/// TODO: If openmp include is put before cuSP it doesn't compile :P
#include <numeric>
#include "OpenMP.hpp"

double dot(double *v1, double *v2, int size) {
  register double sum = 0;
  for (int i = 0; i < size; i++) sum += v1[i] * v2[i];
  return sum;
}

double dot_r2(double *v1, int size) {
  register double sum = 0;
  for (int i = 0; i < size; i++) sum += v1[i] * v1[i];
  return sum;
}

bool breakdown(double inner, double *v, double *w, int size) {
  ddebug(" -> breakdown(inner,v,w,size)\n");

  /// This was copied from miniFE
  // This is code that was copied from Aztec, and originally written
  // by my hero, Ray Tuminaro.
  //
  // Assuming that inner = <v,w> (inner product of v and w),
  // v and w are considered orthogonal if
  //  |inner| < 100 * ||v||_2 * ||w||_2 * epsilon

  bool tmp = 0;
  double vnorm = sqrt(dot(v, v, size));
  double wnorm = sqrt(dot(w, w, size));
  tmp = abs(inner) <=
        100 * vnorm * wnorm * std::numeric_limits<double>::epsilon();
  if (tmp) debug("breakdown(inner,v,w,size) -> Returns breakdown\n");
  ddebug(" <- breakdown(inner,v,w,size)\n");
  return tmp;
}

void waxpby(double *dest, double a, double *x, double b, double *y, int size) {
  for (int i = 0; i < size; i++) dest[i] = a * x[i] + b * y[i];
}

void daxpby(double a, double *x, double b, double *y, int size) {
  for (int i = 0; i < size; i++) y[i] = a * x[i] + b * y[i];
}

void spmv(SpmvOperator &op) {
  SpmvCsrData *data = (SpmvCsrData *)op.format_data;
  spmv_csr(data->rowPtr, data->colInd, (double *)data->values, (double *)op.x,
           (double *)op.y, op.n);
}

double *conjugate_gradient_generic(SpmvOperator &op, double *b, double *x,
                                   double tolerance) {
  double *r, *p, *AxP, alpha, beta, rt, rt_old, *timers, r_norm, tmp, break_tol,
      p_ap_dot;
  vec_print<double>(b, 10, "b");
  vec_print<double>(x, 10, "x0");

  timers = (double *)calloc(5, sizeof(double));
  cudaMallocManaged(&r, op.n * sizeof(double));
  AxP = (double *)op.y;
  p = (double *)op.x;
  timers[1] = csecond() - timers[1];
  waxpby(p, 1.0, x, 0.0, x, op.m);
  vec_print<double>(p, 10, "p");
  tmp = csecond();
  timers[1] = tmp - timers[1];
  timers[0] = tmp - timers[0];
  spmv(op);
  vec_print<double>(AxP, 10, "AxP");
  tmp = csecond();
  timers[0] = tmp - timers[0];
  timers[1] = tmp - timers[1];
  waxpby(r, 1.0, b, -1.0, AxP, op.n);
  vec_print<double>(r, 10, "r");
  tmp = csecond();
  timers[1] = tmp - timers[1];
  timers[2] = tmp - timers[2];
  rt = dot_r2(r, op.n);
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
      waxpby(p, 1.0, r, 0.0, r, op.n);
      vec_print<double>(p, 10, "p");
      timers[1] = csecond() - timers[1];
    } else {
      timers[2] = csecond() - timers[2];
      rt_old = rt;
      rt = dot_r2(r, op.n);
      beta = rt / rt_old;
      tmp = csecond();
      timers[2] = tmp - timers[2];
      timers[1] = tmp - timers[1];
      daxpby(1.0, r, beta, p, op.m);
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
    spmv(op);
    vec_print<double>(AxP, 10, "AxP");
    tmp = csecond();
    timers[0] = tmp - timers[0];
    timers[2] = tmp - timers[2];
    p_ap_dot = dot(AxP, p, op.m);
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
    daxpby(alpha, p, 1.0, x, op.m);
    vec_print<double>(x, 10, "x");
    daxpby(-alpha, AxP, 1.0, r, op.m);
    vec_print<double>(r, 10, "r");
    timers[1] = csecond() - timers[1];
  }
  timers[4] = NR_ITER;
  return timers;
}

double *conjugate_gradient_naive(SpmvOperator &op, double *b) {
  double *rk, *pk, alpha, beta, dotr_r, dotr_rn, *timers, tmp;
  SpmvCsrData *data = (SpmvCsrData *)op.format_data;

  timers = (double *)malloc(5 * sizeof(double));
  rk = (double *)malloc(op.n * sizeof(double));
  cudaMallocManaged(&pk, op.n * sizeof(double));

  timers[0] = timers[1] = timers[2] = timers[3] = timers[4] = 0;
  timers[0] = csecond() - timers[0];
  spmv_csr(data->rowPtr, data->colInd, (double *)data->values, (double *)op.x,
           (double *)op.y, op.n);
  timers[0] = csecond() - timers[0];
  for (int i = 0; i < op.n; i++) pk[i] = rk[i] = b[i] - ((double *)op.y)[i];

  for (int k = 0; k < NR_ITER; k++) {
    timers[0] = csecond() - timers[0];
    spmv_csr(data->rowPtr, data->colInd, (double *)data->values, pk,
             (double *)op.y, op.n);
    tmp = csecond();
    timers[0] = tmp - timers[0];
    timers[1] = tmp - timers[1];
    dotr_r = dot(rk, rk, op.n);
    alpha = dotr_r / dot(pk, (double *)op.y, op.n);
    tmp = csecond();
    timers[1] = tmp - timers[1];
    timers[2] = tmp - timers[2];
    for (int i = 0; i < op.n; i++) pk[i] = pk[i] * alpha;
    for (int i = 0; i < op.n; i++)
      ((double *)op.y)[i] = -((double *)op.y)[i] * alpha;
    tmp = csecond();
    timers[2] = tmp - timers[2];
    timers[3] = tmp - timers[3];
    vec_add<double>((double *)op.x, pk, op.n);
    vec_add<double>((double *)rk, (double *)op.y, op.n);
    tmp = csecond();
    timers[3] = tmp - timers[3];
    timers[1] = tmp - timers[1];
    dotr_rn = dot(rk, rk, op.n);
    if (sqrt(dotr_rn) < 1e-10) {
      timers[1] = tmp - timers[1];
      timers[4] = k + 1;
      return timers;
    }
    beta = dotr_rn / dotr_r;
    tmp = csecond();
    timers[1] = tmp - timers[1];
    timers[2] = tmp - timers[2];
    for (int i = 0; i < op.n; i++) pk[i] = pk[i] * beta / alpha;
    tmp = csecond();
    timers[2] = tmp - timers[2];
    timers[3] = tmp - timers[3];
    vec_add<double>((double *)pk, rk, op.n);
    timers[3] = tmp - timers[3];
  }
  timers[4] = NR_ITER;
  return timers;
}

double *conjugate_gradient_openmp(SpmvOperator &op, double *b) {
  double *rk, *pk, alpha, beta, dotr_r, dotr_rn, *timers, tmp;
  void *tmp_ptr;
  SpmvCsrData *data = (SpmvCsrData *)op.format_data;

  timers = (double *)malloc(5 * sizeof(double));
  rk = (double *)malloc(op.n * sizeof(double));
  cudaMallocManaged(&pk, op.n * sizeof(double));

  timers[0] = timers[1] = timers[2] = timers[3] = timers[4] = 0;
  timers[0] = csecond() - timers[0];
  op.openmp_csr();
  timers[0] = csecond() - timers[0];
  for (int i = 0; i < op.n; i++) pk[i] = rk[i] = b[i] - ((double *)op.y)[i];

  for (int k = 0; k < NR_ITER; k++) {
    timers[0] = csecond() - timers[0];
    tmp_ptr = op.x;
    op.x = pk;
    op.openmp_csr();
    op.x = tmp_ptr;
    tmp = csecond();
    timers[0] = tmp - timers[0];
    timers[1] = tmp - timers[1];
    dotr_r = dot(rk, rk, op.n);
    alpha = dotr_r / dot(pk, (double *)op.y, op.n);
    tmp = csecond();
    timers[1] = tmp - timers[1];
    timers[2] = tmp - timers[2];
    for (int i = 0; i < op.n; i++) pk[i] = pk[i] * alpha;
    for (int i = 0; i < op.n; i++)
      ((double *)op.y)[i] = -((double *)op.y)[i] * alpha;
    tmp = csecond();
    timers[2] = tmp - timers[2];
    timers[3] = tmp - timers[3];
    vec_add<double>((double *)op.x, pk, op.n);
    vec_add<double>((double *)rk, (double *)op.y, op.n);
    tmp = csecond();
    timers[3] = tmp - timers[3];
    timers[1] = tmp - timers[1];
    dotr_rn = dot(rk, rk, op.n);
    if (sqrt(dotr_rn) < 1e-10) {
      timers[1] = tmp - timers[1];
      timers[4] = k;
      return timers;
    }
    beta = dotr_rn / dotr_r;
    tmp = csecond();
    timers[1] = tmp - timers[1];
    timers[2] = tmp - timers[2];
    for (int i = 0; i < op.n; i++) pk[i] = pk[i] * beta / alpha;
    tmp = csecond();
    timers[2] = tmp - timers[2];
    timers[3] = tmp - timers[3];
    vec_add<double>((double *)pk, rk, op.n);
    timers[3] = tmp - timers[3];
  }
  timers[4] = NR_ITER;
  return timers;
}

double *conjugate_gradient_cuSPARSE(SpmvOperator &op, double *b) {
  double *rk, *pk, alpha, beta, dotr_r, dotr_rn, *timers, tmp;
  void *tmp_ptr;
  SpmvCsrData *data = (SpmvCsrData *)op.format_data;

  timers = (double *)malloc(5 * sizeof(double));
  rk = (double *)malloc(op.n * sizeof(double));
  cudaMallocManaged(&pk, op.n * sizeof(double));

  timers[0] = timers[1] = timers[2] = timers[3] = timers[4] = 0;
  timers[0] = csecond() - timers[0];
  op.cuSPARSE_hyb();
  cudaDeviceSynchronize();
  timers[0] = csecond() - timers[0];
  for (int i = 0; i < op.n; i++) pk[i] = rk[i] = b[i] - ((double *)op.y)[i];

  for (int k = 0; k < NR_ITER; k++) {
    timers[0] = csecond() - timers[0];
    tmp_ptr = op.x;
    op.x = pk;
    op.cuSPARSE_hyb();
    cudaDeviceSynchronize();
    op.x = tmp_ptr;
    tmp = csecond();
    timers[0] = tmp - timers[0];
    timers[1] = tmp - timers[1];
    dotr_r = dot(rk, rk, op.n);
    alpha = dotr_r / dot(pk, (double *)op.y, op.n);
    tmp = csecond();
    timers[1] = tmp - timers[1];
    timers[2] = tmp - timers[2];
    for (int i = 0; i < op.n; i++) pk[i] = pk[i] * alpha;
    for (int i = 0; i < op.n; i++)
      ((double *)op.y)[i] = -((double *)op.y)[i] * alpha;
    tmp = csecond();
    timers[2] = tmp - timers[2];
    timers[3] = tmp - timers[3];
    vec_add<double>((double *)op.x, pk, op.n);
    vec_add<double>((double *)rk, (double *)op.y, op.n);
    tmp = csecond();
    timers[3] = tmp - timers[3];
    timers[1] = tmp - timers[1];
    dotr_rn = dot(rk, rk, op.n);
    if (sqrt(dotr_rn) < 1e-10) {
      timers[1] = tmp - timers[1];
      timers[4] = k;
      return timers;
    }
    beta = dotr_rn / dotr_r;
    tmp = csecond();
    timers[1] = tmp - timers[1];
    timers[2] = tmp - timers[2];
    for (int i = 0; i < op.n; i++) pk[i] = pk[i] * beta / alpha;
    tmp = csecond();
    timers[2] = tmp - timers[2];
    timers[3] = tmp - timers[3];
    vec_add<double>((double *)pk, rk, op.n);
    timers[3] = tmp - timers[3];
  }
  timers[4] = NR_ITER;
  return timers;
}

// vec_print<int>(data->rowInd, op.nz, "rowInd");
// vec_print<int>(data->colInd, op.nz, "colInd");
// vec_print<double>((double*)data->values, op.nz, "values");

/// Execute OpenMP csr

// Warmup
for (int i = 0; i < 100; i++) openmp_op.openmp_csr();

// Run OpenMP csr
openmp_op.timer = csecond();
for (int i = 0; i < NR_ITER; i++) {
  openmp_op.openmp_csr();
  y_out = openmp_op.y;
  openmp_op.y = openmp_op.x;
  openmp_op.x = y_out;
}
openmp_op.timer = csecond() - openmp_op.timer;
printf("openmp_csr: ");
report_results(openmp_op.timer, openmp_op.flops, openmp_op.bytes);

vec_print<double>((double *)y_out, 100, "openmp_op.y");

// Warmup
for (int i = 0; i < 100; i++) mkl_op.mkl_csr();

// Run Mkl csr
mkl_op.timer = csecond();
for (int i = 0; i < NR_ITER; i++) {
  mkl_op.mkl_csr();
  y_out = mkl_op.y;
  mkl_op.y = mkl_op.x;
  mkl_op.x = y_out;
  /*if ( i%10 == 0 ){
       exc_timer = csecond();
       openmp_op.vec_alloc(x);
       exc_timer = csecond() - exc_timer;
  } */
}
mkl_op.timer = csecond() - mkl_op.timer - exc_timer;
printf("mkl_csr: ");
report_results(mkl_op.timer, mkl_op.flops, mkl_op.bytes);
y_out = mkl_op.y_get_copy();
check_result<double>((double *)y_out, (double *)op.y, mkl_op.n);
cpu_timer = mkl_op.timer;

/*
  /// Execute csr5 cuda csr
  SpmvOperator csr5_op(cuSPARSE_op);
  csr5_op.format_convert(SPMV_FORMAT_CSR);
  csr5_op.cuCSR5_init();

  // Warmup
  for (int i = 0; i < 100; i++) csr5_op.cuCSR5_csr();
  cudaDeviceSynchronize();

  // Run csr5 cuda
  csr5_op.timer = csecond();
  for (int i = 0; i < NR_ITER; i++) {
    csr5_op.cuCSR5_csr();
    cudaDeviceSynchronize();

    //y_out = csr5_op.y;
    //csr5_op.y = csr5_op.x;
    //csr5_op.x = y_out;
    /*if ( i%10 == 0 ){
         exc_timer = csecond();

         exc_timer = csecond() - exc_timer;
    }
  }
  csr5_op.timer = csecond() - csr5_op.timer - exc_timer;
  printf("cuCSR5_csr: ");
  report_results(csr5_op.timer, csr5_op.flops, csr5_op.bytes);
  y_out = csr5_op.y_get_copy();
  check_result<double>((double *)y_out, (double *)op.y, csr5_op.n);

  /// Execute csr5 avx512 csr
  SpmvOperator csr5_op(cuSPARSE_op);
  csr5_op.format_convert(SPMV_FORMAT_CSR);
  csr5_op.mem_convert(SPMV_MEMTYPE_HOST);
  csr5_op.avx512CSR5_init();

  // Warmup
  for (int i = 0; i < 100; i++) csr5_op.avx512CSR5_csr();

  // Run csr5 cuda
  csr5_op.timer = csecond();
  for (int i = 0; i < NR_ITER; i++) {
    csr5_op.avx512CSR5_csr();


    //y_out = csr5_op.y;
    //csr5_op.y = csr5_op.x;
    //csr5_op.x = y_out;
    /*if ( i%10 == 0 ){
         exc_timer = csecond();

         exc_timer = csecond() - exc_timer;
    }
  }
  csr5_op.timer = csecond() - csr5_op.timer - exc_timer;
  printf("avx512CSR5_csr: ");
  report_results(csr5_op.timer, csr5_op.flops, csr5_op.bytes);
vec_init((double*)csr5_op.y, csr5_op.n, 0);
csr5_op.avx512CSR5_csr();
  y_out = csr5_op.y_get_copy();
  check_result<double>((double *)y_out, (double *)op.y, csr5_op.n);
*/

/// Execute cuSPARSE csr
cuSPARSE_op.format_convert(SPMV_FORMAT_CSR);

// Warmup
for (int i = 0; i < 100; i++) cuSPARSE_op.cuSPARSE_csr();
cudaDeviceSynchronize();

// Run cuSPARSE csr
cuSPARSE_op.timer = csecond();
for (int i = 0; i < NR_ITER; i++) {
  cuSPARSE_op.cuSPARSE_csr();
  cudaDeviceSynchronize();
  y_out = cuSPARSE_op.y;
  cuSPARSE_op.y = cuSPARSE_op.x;
  cuSPARSE_op.x = y_out;
  /*if ( i%10 == 0 ){
       exc_timer = csecond();
       cuSPARSE_op.vec_alloc(x);
       exc_timer = csecond() - exc_timer;
  } */
}
cuSPARSE_op.timer = csecond() - cuSPARSE_op.timer - exc_timer;
printf("cuSPARSE_csr: ");
report_results(cuSPARSE_op.timer, cuSPARSE_op.flops, cuSPARSE_op.bytes);
y_out = cuSPARSE_op.y_get_copy();
check_result<double>((double *)y_out, (double *)op.y, openmp_op.n);
gpu_timer = cuSPARSE_op.timer;

/// Execute cuSPARSE hyb
cuSPARSE_op.format_convert(SPMV_FORMAT_HYB);
cuSPARSE_op.vec_alloc(x);
// Warmup
for (int i = 0; i < 100; i++) cuSPARSE_op.cuSPARSE_hyb();
cudaDeviceSynchronize();

// Run cuSPARSE hyb
cuSPARSE_op.timer = csecond();
for (int i = 0; i < NR_ITER; i++) {
  cuSPARSE_op.cuSPARSE_hyb();
  cudaDeviceSynchronize();
  y_out = cuSPARSE_op.y;
  cuSPARSE_op.y = cuSPARSE_op.x;
  cuSPARSE_op.x = y_out;
  /*if ( i%10 == 0 ){
       exc_timer = csecond();
       cuSPARSE_op.vec_alloc(x);
       exc_timer = csecond() - exc_timer;
  } */
}
cuSPARSE_op.timer = csecond() - cuSPARSE_op.timer - exc_timer;
printf("cuSPARSE_hyb: ");
report_results(cuSPARSE_op.timer, cuSPARSE_op.flops, cuSPARSE_op.bytes);
y_out = cuSPARSE_op.y_get_copy();
check_result<double>((double *)y_out, (double *)op.y, openmp_op.n);
