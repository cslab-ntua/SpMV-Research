//
// Created by cetinicz on 2021-10-31.
//
#ifndef DDT_SPSPMM_DEMO_UTILS_H
#define DDT_SPSPMM_DEMO_UTILS_H

#include "FusionDemo.h"
#include "FastHash.h"
#include "SpSpCSR.h"

#ifdef MKL
#include <mkl.h>
#endif

namespace sparse_avx {
    Csr<double> *spspmm_csr(const Csr<double>* m1, const Csr<double>* m2,
                            vector<FastHash<int, double>*>& result_map, int nThreads) {
    double a;
//#pragma omp parallel for num_threads(nThreads)
    for (int i = 0; i < m1->num_rows; ++i) {
        for (int j = m1->rows[i]; j < m1->rows[i+1]; ++j) {
            int col = m1->cols[j];
            double val = m1->vals[j];

            for (int k = m2->rows[col]; k < m2->rows[col+1]; ++k) {
                int col2 = m2->cols[k];
                double val2 = m2->vals[k];
                double mul = val * val2;
//                a += mul;
                result_map[i]->Reduce(col2, mul);
            }
        }
    }
    // Recover csr from hash table.
    Csr<double>* res = new Csr<double>(result_map, m1->num_rows, m2->num_cols);
    return res;
}

    class SpSpMMSerial : public sym_lib::FusionDemo {
    protected:
        Csr<double>* Ax;
        Csr<double>* Bx;
        std::vector<FastHash<int, double>*> fh;

        void build_set() override {
            if (Ax == nullptr) {
                Ax = new Csr<double>(L1_csr_->m, L1_csr_->nnz, L1_csr_->nnz);
                std::copy(L1_csr_->x, L1_csr_->x + L1_csr_->nnz, Ax->vals);
                std::copy(L1_csr_->i, L1_csr_->i + L1_csr_->nnz, Ax->cols);
                std::copy(L1_csr_->p, L1_csr_->p + L1_csr_->m + 1, Ax->rows);
                Bx = new Csr<double>(L1_csr_->m, L1_csr_->n, L1_csr_->nnz);
                std::copy(L1_csr_->x, L1_csr_->x + L1_csr_->nnz, Bx->vals);
                std::copy(L1_csr_->i, L1_csr_->i + L1_csr_->nnz, Bx->cols);
                std::copy(L1_csr_->p, L1_csr_->p + L1_csr_->m + 1, Bx->rows);
                for (int i = 0; i < L1_csr_->m; i++) {
                    fh.emplace_back(new FastHash<int, double>(L1_csr_->m));
                }
            }
        }

        sym_lib::timing_measurement fused_code() override {
            sym_lib::timing_measurement t1;
            t1.start_timer();
            auto r = spspmm_csr(Ax,Bx,fh,num_threads_);
            t1.measure_elapsed_time();
            std::copy(r->vals,r->vals+r->nnz,x_);
            return t1;
        }

    public:
        SpSpMMSerial(sym_lib::CSR *L, sym_lib::CSC *L_csc,
                   std::string name)
                   : Ax(nullptr), Bx(nullptr), FusionDemo(L->m*L->n, name) {
            L1_csr_ = L;
            L1_csc_ = L_csc;
        };

        ~SpSpMMSerial() override {
            delete Ax;
            delete Bx;
            for(auto f : fh) {
                delete f;
            }
        };
    };

    class SpSpMMMKL : public SpSpMMSerial {
    protected:
        sparse_matrix_t MKL_Ax;
        sparse_matrix_t MKL_Bx;
        sparse_matrix_t MKL_Cx;

        sym_lib::timing_measurement analysis_breakdown;

        MKL_INT* LLI;

        void build_set() override {
            if (LLI == nullptr) {
                analysis_breakdown.start_timer();
                LLI = new MKL_INT[this->L1_csr_->m + 1]();
                for (int l = 0; l < this->L1_csr_->m + 1; ++l) {
                    LLI[l] = this->L1_csr_->p[l];
                }
                mkl_sparse_d_create_csr(&MKL_Ax, SPARSE_INDEX_BASE_ZERO,
                                        this->L1_csr_->m, this->L1_csr_->n, LLI,
                                        LLI + 1, this->L1_csr_->i,
                                        this->L1_csr_->x);
                matrix_descr descr;
                descr.type = SPARSE_MATRIX_TYPE_GENERAL;

                mkl_sparse_copy(MKL_Ax,descr,&MKL_Bx);
                mkl_set_num_threads(num_threads_);
                mkl_set_num_threads_local(num_threads_);
                analysis_breakdown.measure_elapsed_time();
            }
        }

        sym_lib::timing_measurement fused_code() override {
            sym_lib::timing_measurement t1;
            t1.start_timer();
            mkl_sparse_spmm(SPARSE_OPERATION_NON_TRANSPOSE,MKL_Ax,MKL_Bx,&MKL_Cx);
            t1.measure_elapsed_time();
            int rows, cols;
            int*Li;
            double* Lx;
            int* Lp0, *Lp1;
            sparse_index_base_t indexing;
            mkl_sparse_d_export_csr(MKL_Cx,&indexing,&rows,&cols,&Lp0,&Lp1,&Li,&Lx);
            std::copy(Lx,Lx+(Lp1-Lp0),x_);
            return t1;
        }

    public:
        SpSpMMMKL(sym_lib::CSR *L, sym_lib::CSC *L_csc, double* correct_x,
                     std::string name)
                     : LLI(nullptr), MKL_Cx(nullptr), SpSpMMSerial(L, L_csc,name) {
            L1_csr_ = L;
            L1_csc_ = L_csc;
            correct_x_ = correct_x;
        };
        ~SpSpMMMKL() override = default;
    };
}

#endif//DDT_SPSPMM_DEMO_UTILS_H
