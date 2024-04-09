//
// Created by kazem on 7/8/21.
//

#ifndef SPARSE_AVX512_DEMO_DAG_H
#define SPARSE_AVX512_DEMO_DAG_H


#include <vector>

namespace sparse_avx{
 class DAG {
  std::vector<std::vector<int>> _dag;
 public:
  DAG();
  DAG(int n);

  void insert_edge(int src, int dst);

 };
}



#endif //SPARSE_AVX512_DEMO_DAG_H
