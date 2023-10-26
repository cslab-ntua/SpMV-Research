#include "mkl.h"
#include "mkl_blas.h"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <string.h>
#include <sys/time.h>
#include <omp.h>

#define MICRO_IN_SEC 1000000.00
#define FIELD_LENGTH 128
#define floatType double
#define PADDING 64
#define TEST_TIME 1000
#define SPMV_WARMUP_TIME 300
#define THREAD_NUM 24

using namespace std;

double microtime()
{
    int tv_sec, tv_usec;
    double time;
    struct timeval tv;
    struct timezone tz;
    gettimeofday(&tv, &tz);

    return (tv.tv_sec + tv.tv_usec / MICRO_IN_SEC) * 1000; // ms
}

struct Coordinate
{
    int x;
    int y;
    float val;
};

inline int coordcmp(const void *v1, const void *v2)
{
    struct Coordinate *c1 = (struct Coordinate *)v1;
    struct Coordinate *c2 = (struct Coordinate *)v2;

    if (c1->x != c2->x)
    {
        return (c1->x - c2->x);
    }
    else
    {
        return (c1->y - c2->y);
    }
}

void readMatrix(string filename, floatType **val_ptr, MKL_INT **cols_ptr,
                MKL_INT **rowDelimiters_ptr, int *n, int *numRows, int *numCols)
{
    std::string line;
    char id[FIELD_LENGTH];
    char object[FIELD_LENGTH];
    char format[FIELD_LENGTH];
    char field[FIELD_LENGTH];
    char symmetry[FIELD_LENGTH];

    std::ifstream mfs(filename);
    if (!mfs.good())
    {
        std::cerr << "Error: unable to open matrix file " << filename << std::endl;
        exit(1);
    }

    int symmetric = 0;
    int pattern = 0;
    int field_complex = 0;

    int nRows, nCols, nElements;

    struct Coordinate *coords;

    // read matrix header
    if (getline(mfs, line).eof())
    {
        std::cerr << "Error: file " << filename << " does not store a matrix" << std::endl;
        exit(1);
    }

    sscanf(line.c_str(), "%s %s %s %s %s", id, object, format, field, symmetry);

    if (strcmp(object, "matrix") != 0)
    {
        fprintf(stderr, "Error: file %s does not store a matrix\n", filename.c_str());
        exit(1);
    }

    if (strcmp(format, "coordinate") != 0)
    {
        fprintf(stderr, "Error: matrix representation is dense\n");
        exit(1);
    }

    if (strcmp(field, "pattern") == 0)
    {
        pattern = 1;
    }

    if (strcmp(field, "complex") == 0)
    {
        field_complex = 1;
    }

    if (strcmp(symmetry, "symmetric") == 0)
    {
        symmetric = 1;
    }

    while (!getline(mfs, line).eof())
    {
        if (line[0] != '%')
        {
            break;
        }
    }

    // read the matrix size and number of non-zero elements
    sscanf(line.c_str(), "%d %d %d", &nRows, &nCols, &nElements);

    int valSize = nElements * sizeof(struct Coordinate);

    if (symmetric)
    {
        valSize *= 2;
    }

    //    coords = new Coordinate[valSize];
    coords = (struct Coordinate *)malloc(valSize);

    int index = 0;
    float xx99 = 0;
    while (!getline(mfs, line).eof())
    {
        if (pattern)
        {
            sscanf(line.c_str(), "%d %d", &coords[index].x, &coords[index].y);
            coords[index].val = index % 13;
        }
        else if (field_complex)
        {
            // read the value from file
            sscanf(line.c_str(), "%d %d %f %f", &coords[index].x, &coords[index].y,
                   &coords[index].val, &xx99);
        }
        else
        {
            // read the value from file
            sscanf(line.c_str(), "%d %d %f", &coords[index].x, &coords[index].y,
                   &coords[index].val);
        }

        coords[index].x -= 1;
        coords[index].y -= 1;
        index++;

        // add the mirror element if not on main diagonal
        if (symmetric && coords[index - 1].x != coords[index - 1].y)
        {
            coords[index].x = coords[index - 1].y;
            coords[index].y = coords[index - 1].x;
            coords[index].val = coords[index - 1].val;
            index++;
        }
    }

    nElements = index;

    std::cout << "===========================================================================" << std::endl;
    std::cout << "=========*********  Informations of the sparse matrix   *********==========" << std::endl;
    std::cout << std::endl;
    std::cout << "     Number of Rows is :" << nRows << std::endl;
    std::cout << "  Number of Columns is :" << nCols << std::endl;
    std::cout << " Number of Elements is :" << nElements << std::endl;
    std::cout << std::endl;
    std::cout << "===========================================================================" << std::endl;

    std::cout << "............ Converting the Raw matrix to CSR ................." << std::endl;

    // sort the elements
    qsort(coords, nElements, sizeof(struct Coordinate), coordcmp);

    // create CSR data structures
    *n = nElements;
    *numRows = nRows;
    *numCols = nCols;

    *val_ptr = (floatType *)mkl_malloc(sizeof(floatType) * nElements, PADDING);
    *cols_ptr = (MKL_INT *)mkl_malloc(sizeof(MKL_INT) * nElements, PADDING);
    *rowDelimiters_ptr = (MKL_INT *)mkl_malloc(sizeof(MKL_INT) * (nRows + 1), PADDING);

    floatType *val = *val_ptr;
    int *cols = *cols_ptr;
    int *rowDelimiters = *rowDelimiters_ptr;

    val[0] = coords[0].val;
    cols[0] = coords[0].y;
    rowDelimiters[0] = 0;

    int row = 1, i = 1;
    for (int i = 1; i < nElements; i++)
    {
        val[i] = coords[i].val;
        cols[i] = coords[i].y;
        if (coords[i].x != coords[i - 1].x)
        {
            rowDelimiters[row++] = i;
        }
    }

    rowDelimiters[row] = nElements;

    free(coords);
}

void spmv_csr(floatType *h_val, int *h_cols, int *h_rowDelimiters, int &numRows, floatType *x, floatType *y)
{
    int i, row;
    double sum;
#pragma omp parallel private(i, row, sum)
    {
#pragma omp for schedule(static) nowait
        for (row = 0; row < numRows; row++)
        {
            sum = 0;
            for (i = h_rowDelimiters[row]; i < h_rowDelimiters[row + 1]; i += 1)
            {
                sum += h_val[i] * x[h_cols[i]];
            }
            y[row] = sum;
        }
    }
}

void test_file(string filename, double *pre_processing_time, double *mkl_time)
{
    // mkl_set_num_threads(1);

    floatType *h_val;
    MKL_INT *h_cols;
    MKL_INT *h_rowDelimiters;

    int nItems;
    int numRows;
    int numCols;

    floatType *x;
    floatType *y;
    floatType *csr_y;

    //====================>get time matire<========================
    readMatrix(filename, &h_val, &h_cols, &h_rowDelimiters, &nItems, &numRows, &numCols);

    x = (floatType *)mkl_malloc(sizeof(floatType) * numCols, PADDING);
    for (int i = 0; i < numCols; i++)
    {
        x[i] = i % 10;
    }

    y = (floatType *)mkl_malloc(sizeof(floatType) * numRows, PADDING);
    memset(y, 0, sizeof(floatType) * numRows);

    //===================>convect to mkl_csr<======================
    sparse_matrix_t A;
    mkl_sparse_d_create_csr(&A, SPARSE_INDEX_BASE_ZERO, numRows, numCols, h_rowDelimiters, &h_rowDelimiters[1], h_cols, h_val);

    matrix_descr descr;
    descr.type = SPARSE_MATRIX_TYPE_GENERAL;

    //===================>pre-processing<======================
    double kk0 = microtime();

    mkl_sparse_set_mv_hint(A, SPARSE_OPERATION_NON_TRANSPOSE, descr, 200);
    mkl_sparse_optimize(A);

    *pre_processing_time = microtime() - kk0;

    std::cout << "The pre-processing time in mkl is " << *pre_processing_time << " ms." << endl;

    //===================>warm up<======================
    for (int i = 0; i < SPMV_WARMUP_TIME; i++)
    {
        mkl_sparse_d_mv(SPARSE_OPERATION_NON_TRANSPOSE, 1, A, descr, x, 0, y);
    }

    //====================>spmv<========================
    kk0 = microtime();

    for (int i = 0; i < TEST_TIME; i++)
    {
        mkl_sparse_d_mv(SPARSE_OPERATION_NON_TRANSPOSE, 1, A, descr, x, 0, y);
    }

    *mkl_time = (microtime() - kk0) / TEST_TIME;

    std::cout << "The average SpMV Time of mkl is " << *mkl_time << " ms." << endl;

    //====================>accuracy<========================
    csr_y = (floatType *)aligned_alloc(64, sizeof(floatType) * numRows);
    memset(csr_y, 0, sizeof(floatType) * numRows);

    spmv_csr(h_val, h_cols, h_rowDelimiters, numRows, x, csr_y);

    int error_count = 0;
    for (int i = 0; i < numRows; i++)
    {
        if (abs(csr_y[i] - y[i]) >= 10e-9)
        {
            error_count++;
        }
    }

    cout << "The spmv error count is: " << error_count << endl;

    //================================================================
    mkl_sparse_destroy(A);
    free(csr_y);
    mkl_free(h_cols);
    mkl_free(h_val);
    mkl_free(h_rowDelimiters);
    mkl_free(x);
    mkl_free(y);
}

int main()
{
    mkl_set_num_threads(THREAD_NUM);
    cout << "mkl_opt, max thread:" << THREAD_NUM << endl;

    string input_file_path = "./in/matrix101.txt";
    string output_file_path = "./out/matrix_mkl_opt_0416_thread24.txt";
    string matrix_dir_path = "../matrix101/";
    // string start_file = "TSOPF_RS_b300_c1.mtx";

    std::ifstream ifs(input_file_path, std::ifstream::in);
    std::ofstream ofs(output_file_path, std::ostream::app);

    double mkl_time, pre_processing_time;
    string file_name, file_path;

    // while (file_name != start_file)
    // {
    //     ifs >> file_name;
    // }

    // cout << file_name << endl;
    // file_path = matrix_dir_path + file_name;
    // test_file(file_path, &mkl_time);
    // ofs << file_name << " " << mkl_time << endl;

    while (ifs >> file_name)
    {
        cout << file_name << endl;
        file_path = matrix_dir_path + file_name;
        test_file(file_path, &pre_processing_time, &mkl_time);
        ofs << file_name << " " << mkl_time << " " << pre_processing_time << " " << endl;
    }

    ofs << endl;

    // test_file("../matrix101/IEEE8500node_A_td_0.mtx", &mkl_time);
    // test_file("../matrix101/test1.mtx", &mkl_time);
    // test_file("../matrix101/test2.mtx", &mkl_time);
    // cout << mkl_time << endl;
}

// icc mkl.cpp -std=c++11 -L${MKLROOT}/lib/intel64 -lmkl_intel_lp64 -lmkl_intel_thread -lmkl_core -liomp5 -lpthread -lm -ldl -o mkl
// icc mkl.cpp -std=c++11 -Wl,--start-group ${MKLROOT}/lib/intel64/libmkl_intel_lp64.a ${MKLROOT}/lib/intel64/libmkl_intel_thread.a ${MKLROOT}/lib/intel64/libmkl_core.a -Wl,--end-group -liomp5 -lpthread -lm -ldl -o mkl