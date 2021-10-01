#include <iostream>

#include "spmv_utils.hpp"
#include "anonymouslib_cuda.h"

#include "mmio.h"
#include "nvmlPower.hpp"

using namespace std;

#ifndef VALUE_TYPE
#error
#endif

#ifndef NUM_RUN
#error
#endif

double call_anonymouslib(int m, int n, int nnzA,
                  int *csrRowPtrA, int *csrColIdxA, VALUE_TYPE *csrValA,
                  VALUE_TYPE *x, VALUE_TYPE *y, VALUE_TYPE alpha)
{
    int err = 0;
    cudaError_t err_cuda = cudaSuccess;

    // set device
    int device_id = 1;
    cudaSetDevice(device_id);
    cudaDeviceProp deviceProp;
    cudaGetDeviceProperties(&deviceProp, device_id);

    cout << "Device [" <<  device_id << "] " << deviceProp.name << ", " << " @ " << deviceProp.clockRate * 1e-3f << "MHz. " << endl;

    double gb = getB<int, VALUE_TYPE>(m, nnzA);
    double gflop = 2*nnzA;

    // Define pointers of matrix A, vector x and y
    int *d_csrRowPtrA;
    int *d_csrColIdxA;
    VALUE_TYPE *d_csrValA;
    VALUE_TYPE *d_x;
    VALUE_TYPE *d_y;

    // Matrix A
    checkCudaErrors(cudaMalloc((void **)&d_csrRowPtrA, (m+1) * sizeof(int)));
    checkCudaErrors(cudaMalloc((void **)&d_csrColIdxA, nnzA  * sizeof(int)));
    checkCudaErrors(cudaMalloc((void **)&d_csrValA,    nnzA  * sizeof(VALUE_TYPE)));

    checkCudaErrors(cudaMemcpy(d_csrRowPtrA, csrRowPtrA, (m+1) * sizeof(int),   cudaMemcpyHostToDevice));
    checkCudaErrors(cudaMemcpy(d_csrColIdxA, csrColIdxA, nnzA  * sizeof(int),   cudaMemcpyHostToDevice));
    checkCudaErrors(cudaMemcpy(d_csrValA,    csrValA,    nnzA  * sizeof(VALUE_TYPE),   cudaMemcpyHostToDevice));

    // Vector x
    checkCudaErrors(cudaMalloc((void **)&d_x, n * sizeof(VALUE_TYPE)));
    checkCudaErrors(cudaMemcpy(d_x, x, n * sizeof(VALUE_TYPE), cudaMemcpyHostToDevice));

    // Vector y
    checkCudaErrors(cudaMalloc((void **)&d_y, m  * sizeof(VALUE_TYPE)));
    checkCudaErrors(cudaMemset(d_y, 0, m * sizeof(VALUE_TYPE)));

    anonymouslibHandle<int, unsigned int, VALUE_TYPE> A(m, n);
    err = A.inputCSR(nnzA, d_csrRowPtrA, d_csrColIdxA, d_csrValA);
    //cout << "inputCSR err = " << err << endl;

    err = A.setX(d_x); // you only need to do it once!
    //cout << "setX err = " << err << endl;

    A.setSigma(ANONYMOUSLIB_AUTO_TUNED_SIGMA);

    // warmup device
    A.warmup();

    anonymouslib_timer asCSR5_timer;
    asCSR5_timer.start();

    err = A.asCSR5();

    cout << "CSR->CSR5 time = " << asCSR5_timer.stop() << " ms." << endl;
    //cout << "asCSR5 err = " << err << endl;

    // check correctness by running 1 time
    err = A.spmv(alpha, d_y);
    //cout << "spmv err = " << err << endl;
    checkCudaErrors(cudaMemcpy(y, d_y, m * sizeof(VALUE_TYPE), cudaMemcpyDeviceToHost));

    // warm up by running 50 times
    if (NUM_RUN)
    {
        for (int i = 0; i < 50; i++)
            err = A.spmv(alpha, d_y);
    }

    err_cuda = cudaDeviceSynchronize();

    nvmlAPIRun();
    double timer = csecond();

    // time spmv by running NUM_RUN times
    for (int i = 0; i < NUM_RUN; i++)
        err = A.spmv(alpha, d_y);
    err_cuda = cudaDeviceSynchronize();

    timer = (csecond() - timer)/NUM_RUN;
    nvmlAPIEnd();
    
    if (NUM_RUN)
        cout << "CSR5-based SpMV time = " << timer
             << " ms. Bandwidth = " << gb/(1.0e+9 * timer)
             << " GB/s. GFlops = " << gflop/(1.0e+9 * timer)  << " GFlops." << endl;

    A.destroy();

    checkCudaErrors(cudaFree(d_csrRowPtrA));
    checkCudaErrors(cudaFree(d_csrColIdxA));
    checkCudaErrors(cudaFree(d_csrValA));
    checkCudaErrors(cudaFree(d_x));
    checkCudaErrors(cudaFree(d_y));

    return timer;
}

int main(int argc, char ** argv)
{
    int argi = 2;

    char  *filename, *logname;
    massert(argc == argi + 1, "Wrong args. Usage ./ExecMe file.mtx logfilename");
    filename = argv[1];
    logname = argv[2];
    
    cout << "Filename: " << filename << endl;
    cout << "Logfile Name: " << logname << endl;

    /// Mix C & C++ file inputs, because...?
    ofstream foutp;
    foutp.open(logname, ios::out | ios::app ); 
    massert(foutp.is_open() , "Invalid output File");

    SpmvCsrData* csr_matrix = mtx_read_csr(filename);

    int m = csr_matrix->m, n = csr_matrix->n, nnzA = csr_matrix->nz;
    int *csrRowPtrA = csr_matrix->rowPtr;
    int *csrColIdxA = csr_matrix->colInd;
    VALUE_TYPE *csrValA = (VALUE_TYPE*) csr_matrix->values;


    srand(time(NULL));

/*
    // set csrValA to 1, easy for checking floating-point results
    for (int i = 0; i < nnzA; i++)
    {
        csrValA[i] = rand() % 10;
    }
*/

    cout << " ( " << m << ", " << n << " ) nnz = " << nnzA << endl;

    VALUE_TYPE *x = (VALUE_TYPE *)malloc(n * sizeof(VALUE_TYPE));
    for (int i = 0; i < n; i++)
        x[i] = rand() % 10;

    VALUE_TYPE *y = (VALUE_TYPE *)malloc(m * sizeof(VALUE_TYPE));
    VALUE_TYPE *y_ref = (VALUE_TYPE *)malloc(m * sizeof(VALUE_TYPE));

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
    double spmv_seconds = call_anonymouslib(m, n, nnzA, csrRowPtrA, csrColIdxA, csrValA, x, y, alpha);
    // compare reference and anonymouslib results
    int error_count = 0;
    for (int i = 0; i < m; i++)
        if (abs(y_ref[i] - y[i]) > 0.01 * abs(y_ref[i]))
        {
            error_count++;
//            cout << "ROW [ " << i << " ], NNZ SPAN: "
//                 << csrRowPtrA[i] << " - "
//                 << csrRowPtrA[i+1]
//                 << "\t ref = " << y_ref[i]
//                 << ", \t csr5 = " << y[i]
//                 << ", \t error = " << y_ref[i] - y[i]
//                 << endl;
//            break;

//            //if (abs(y_ref[i] - y[i]) > 0.00001)
//            //    cout << ", \t error = " << y_ref[i] - y[i] << endl;
//            //else
//            //    cout << ". \t CORRECT!" << endl;
        }

    if (error_count == 0)
        cout << "Check... PASS!" << endl;
    else
        cout << "Check... NO PASS! #Error = " << error_count << " out of " << m << " entries." << endl;

    double gflops_s = 2*nnzA/spmv_seconds*1e-9;
    foutp << filename << "," << "CSR5-cuda" << "," << m << "," << n << "," << nnzA << "," << spmv_seconds << "," << gflops_s << "\n";
    foutp.close();

    free(csrRowPtrA);
    free(csrColIdxA);
    free(csrValA);
    free(x);
    free(y);
    free(y_ref);


    return 0;
}

