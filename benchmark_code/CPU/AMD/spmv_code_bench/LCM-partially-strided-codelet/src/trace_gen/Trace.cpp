//
// Created by kazem on 7/8/21.
//

#include <iostream>
#include "Trace.h"

namespace sparse_avx{

 Trace::Trace(int n):_num_trace(n) {
  _mem_addr = new int[3*n]();
  _op_codes = new int[n]();
  //_tuple = new Tuple*[n];
  _trace_threshold = 1e5;
  _pre_alloc = false;
 }

 Trace::Trace(int n, int *ma, int *oc, int np, int ni):_num_trace(n),_mem_addr(ma),
  _op_codes(oc), _num_partitions(np), _ni(ni) {
  _iter_pt = new int*[ni+1]();
  _c = new DDT::PatternDAG[n]();
  _pre_alloc = true;
 }

 Trace::~Trace() {
  if(!_pre_alloc){
   delete []_mem_addr;
   delete []_op_codes;
  }
/*
  for (int i = 0; i < _num_trace; ++i) {
   delete _tuple[i];
  }
*/
 }


 int* Trace::MemAddr(){
  return _mem_addr;
 }

 void Trace::print() {
  for (int i = 0; i < _num_trace; ++i) {
   std::cout<<_op_codes[i] <<" : "<<_mem_addr[i*3]<<","<<_mem_addr[i*3+1]<<","
                                                      ""<<_mem_addr[i*3+2]<<"\n";
  }
 }

 void free_trace_list(Trace** list){
  int list_size = list[0]->_num_partitions;
  delete []list[0]->_op_codes;
  delete []list[0]->_mem_addr;
  for (int i = 0; i < list_size; ++i) {
   delete list[i];
  }
  delete []list;
 }

 void free_trace_list(Trace*** list, int dim1, const std::vector<int>& dim2){
  delete []list[0][0]->_op_codes;
  delete []list[0][0]->_mem_addr;
  for (int i = 0; i < dim1; ++i) {
   for (int j = 0; j < dim2[i]; ++j) {
    delete list[i][j];
   }
  }
  for (int i = 0; i < dim1; ++i) {
    delete list[i];
  }
  delete []list;
 }
}
