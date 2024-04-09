//
// Created by kazem on 7/6/21.
//

#ifndef SPARSE_AVX512_DEMO_TRACEIR_H
#define SPARSE_AVX512_DEMO_TRACEIR_H

namespace sparse_avx{
 enum TRACE_OP{
  ADD, SUB, DIV, AddM
 };

 struct Tuple{
  // one offset for each operands
  int *memory_trace;

  Tuple(int *mt);
  virtual ~Tuple();

  virtual TRACE_OP get_id()=0;
 };

 struct Add: public Tuple{

  Add(int *mt);
  TRACE_OP get_id() override;
 };

 struct AddMul: public Tuple{

  AddMul(int *mt);
  TRACE_OP get_id() override;
 };

 struct Div: public Tuple{

  Div(int *mt);
  TRACE_OP get_id() override;
 };

}


#endif //SPARSE_AVX512_DEMO_TRACEIR_H
