#ifndef STANDALONE_DISPATCHER_H
#define STANDALONE_DISPATCHER_H

#include "spmv_kernel.h"
#include <cuda_runtime.h>

// =============================================================================
// Standalone_Arrays: a Matrix_Format wrapper that removes specific rows from
// GPU execution.  The removed rows are simply not computed (skipped).
//
// Architecturally simpler than Hybrid_Arrays: no CPU sub-format, no concurrent
// CPU/GPU overlap.  The key difference from a plain standalone GPU run is that
// a subset of rows is excluded via row_map and a reduced sub-matrix is built.
// =============================================================================

struct Standalone_Arrays : Matrix_Format {
    Matrix_Format * gpu_part;    // The reduced GPU sub-matrix (only m_gpu rows)
    long m_original;             // Original full matrix row count
    long m_removed;              // Number of rows removed (skipped)
    long m_gpu;                  // Rows remaining on GPU  (m_original - m_removed)
    long nnz_gpu;                // NNZ in the GPU sub-matrix
    long nnz_removed;            // NNZ in the removed rows
    INT_T * row_map;             // row_map[0..m_gpu-1]   = original row IDs kept on GPU
                                 // row_map[m_gpu..m-1]   = original row IDs removed

    Standalone_Arrays(long m, long n, long nnz)
        : Matrix_Format(m, n, nnz), gpu_part(NULL),
          m_original(m), m_removed(0), m_gpu(m),
          nnz_gpu(nnz), nnz_removed(0),
          row_map(NULL)
    {
        row_map = (INT_T *) malloc(m * sizeof(INT_T));
        for (long i = 0; i < m; i++) row_map[i] = i;
    }

    ~Standalone_Arrays() {
        if (gpu_part) delete gpu_part;
        if (row_map) free(row_map);
    }

    // SpMV: run GPU on reduced matrix, scatter results into full y.
    void spmv(ValueType * x, ValueType * y) override;

    void synchronize() override;

    void set_last_iteration(bool is_last) override {
        if (gpu_part) gpu_part->set_last_iteration(is_last);
    }

    double get_last_duration() override {
        return gpu_part ? gpu_part->get_last_duration() : 0;
    }

    void statistics_start() override;
    int statistics_print_data(char * buf, long buf_n) override;
};

#endif
