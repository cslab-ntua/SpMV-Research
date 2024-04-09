//
// Created by kazem on 7/8/21.
//

#ifndef SPARSE_AVX512_DEMO_SPMVMODEL_H
#define SPARSE_AVX512_DEMO_SPMVMODEL_H
#include "PolyModel.h"

namespace sparse_avx {

 class SpMVModel : public SectionPolyModel {
  int _num_rows{}, _num_cols{}, _nnz{};
  int *_Ap{}, *_Ai{};

 public:
  SpMVModel();
  SpMVModel(int n, int m, int nnz, int *Ap, int *Ai);

  Trace* generate_trace() override;
  Trace** generate_trace(int num_threads) override;
 };

}

#endif //SPARSE_AVX512_DEMO_SPMVMODEL_H
