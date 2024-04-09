//
// Created by kazem on 7/8/21.
//

#ifndef SPARSE_AVX512_DEMO_TRACE_H
#define SPARSE_AVX512_DEMO_TRACE_H

#include "DDTDef.h"
#include "TraceIR.h"

#include <vector>

namespace sparse_avx{

 class Trace {
 public:
  Tuple **_tuple;
  int *_mem_addr;
  int *_op_codes;
  int _num_trace;
  int _trace_threshold;
  int _num_partitions;
  int _num_hlevels;
  int** _iter_pt;
  int _ni;
  DDT::PatternDAG* _c;

  bool _pre_alloc;

 public:
  Trace(int n);

  Trace(int n, int *ma, int *oc, int np, int ni);

  ~Trace();

  int* MemAddr();

  void print();


 };

 void free_trace_list(Trace** list);
 void free_trace_list(Trace*** list, int dim1, const std::vector<int>& dim2);


}



#endif //SPARSE_AVX512_DEMO_TRACE_H
