//
// Created by cetinicz on 2021-08-17.
//

#ifndef DDT_SPMV_CVR_H
#define DDT_SPMV_CVR_H

#define floatType double

namespace SPMV_CVR {
    struct ExecutorParameters {
        int Nthrds;
        int N_start;
        int N_step;
        int *vPack_Nblock;
        int *vPack_vec_record;
        int *vPack_nnz_rows;
        floatType *vPack_vec_vals;
        int *vPack_vec_cols;
        floatType *h_val;
        int *h_cols;
        int *vPack_vec_final;
        int *vPack_vec_final_2;
        int *vPack_split;
        floatType *refOut;
        int nItems;
        int numCols;
        int numRows;
        int omega;
        int *h_rowDelimiters;
        char *filename;
        floatType *h_vec;
        int Ntimes;
    };
    void build_set(ExecutorParameters& ep);

    void run_spmv(int Nthrds, int N_start, int N_step, int *vPack_Nblock,
                  int *vPack_vec_record, int *vPack_nnz_rows,
                  floatType *vPack_vec_vals, int *vPack_vec_cols,
                  floatType *h_val, int *h_cols, int *vPack_vec_final,
                  int *vPack_vec_final_2, int *vPack_split, floatType *refOut,
                  int nItems, int numRows, int omega, int *h_rowDelimiters,
                  char *filename, floatType *h_vec, int Ntimes);
}
#endif//DDT_SPMV_CVR_H
