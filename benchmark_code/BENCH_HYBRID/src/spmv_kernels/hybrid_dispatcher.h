#ifndef HYBRID_DISPATCHER_H
#define HYBRID_DISPATCHER_H

#include "spmv_kernel.h"
#include <nvtx3/nvToolsExt.h>
#include <cuda_runtime.h>

struct Hybrid_Arrays : Matrix_Format {
    Matrix_Format * cpu_part;
    Matrix_Format * gpu_part;
    long m_cpu;
    long m_gpu;
    INT_T * row_map; 

    // Independent timing stats. Update: these are no longer used. The code now stores cpu and gpu times like for the non-hybrid case in an array in order to extract the median later for reporting.
    // double time_cpu_total;
    // double time_gpu_total;
    long call_count;
    
    int original_threads;
    int new_threads;

    Hybrid_Arrays(long m, long n, long nnz, long m_cpu) 
        : Matrix_Format(m, n, nnz), m_cpu(m_cpu),
        //   time_cpu_total(0), time_gpu_total(0), 
          call_count(0) {
        m_gpu = m - m_cpu;
        // get original ordering of rows (it will change later due to splitting in CPU and GPU parts)
        row_map = (INT_T *) malloc(m * sizeof(INT_T));
        for (long i = 0; i < m; i++) row_map[i] = i; 
    }

    ~Hybrid_Arrays() {
        delete cpu_part;
        delete gpu_part;
        if (row_map) free(row_map);
    }

    // Standard SpMV (sequential call of both, isolated vectors)
    void spmv(ValueType * x, ValueType * y) override;

    // Independent calls for benchmarking
    void cpu_spmv(ValueType * x, ValueType * y);
    void gpu_spmv(ValueType * x, ValueType * y);
    void gpu_spmv_sync(ValueType * x, ValueType * y);

    void statistics_start() override;
    int statistics_print_data(char * buf, long buf_n) override;

    void set_last_iteration(bool is_last) override {
        if (gpu_part) gpu_part->set_last_iteration(is_last);
    }
};

#endif
