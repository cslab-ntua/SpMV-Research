//
// Created by kazem on 7/8/21.
//

#include <cassert>
#include <iostream>
#include "SpMVModel.h"

namespace sparse_avx{

 SpMVModel::SpMVModel():SectionPolyModel() {}

 SpMVModel::SpMVModel(int n, int m, int nnz, int *Ap, int *Ai):_num_rows
 (n),_num_cols(m), _nnz(nnz), _Ap(Ap), _Ai(Ai) {//FIXME:should be removed
 }

 Trace* SpMVModel::generate_trace() {
  auto *trace = new Trace(_nnz);
  std::fill_n(trace->_op_codes,_nnz, TRACE_OP::AddM);
#pragma omp parallel for default(none) shared(trace)
  for (int i = 0; i < _num_rows; ++i) {
   for (int j = _Ap[i]; j < _Ap[i+1]; ++j) {
    auto cur_adr = trace->_mem_addr + 3*j;
    cur_adr[0] = i; cur_adr[1] = j; cur_adr[2] = _Ai[j];
    //trace->_tuple[j] = new AddMul(cur_adr);
   }
  }
  return trace;
 }

 Trace** SpMVModel::generate_trace(int num_threads) {
  Trace** trace_list = new Trace*[num_threads];
  auto *tr_list_mm_array = new int[3*_nnz]();
  auto *tr_list_oc_array = new int[_nnz]();
  std::fill_n(tr_list_oc_array, _nnz, TRACE_OP::AddM);
  auto *nnz_bounds = new int[num_threads];
  std::vector<int> bnd_row_array(num_threads+1);
  int nnz_part = _nnz/num_threads;
  int bnd_row = closest_row(nnz_part, _Ap, 0);
  bnd_row_array[0] = 0;
  bnd_row_array[1] = bnd_row;
  nnz_bounds[0] = _Ap[bnd_row];
  trace_list[0] = new Trace(nnz_bounds[0], tr_list_mm_array, tr_list_oc_array,
                            num_threads,bnd_row_array[1]-0);
  for (int i = 1; i < num_threads-1; ++i) {
   nnz_bounds[i] = nnz_bounds[i-1] + nnz_part;
   bnd_row = closest_row(nnz_bounds[i], _Ap, bnd_row);
   bnd_row_array[i+1] = bnd_row;
   nnz_bounds[i] = _Ap[bnd_row];
   trace_list[i] = new Trace(nnz_bounds[i], tr_list_mm_array+3*nnz_bounds[i-1],
                             tr_list_oc_array+nnz_bounds[i-1], num_threads, bnd_row_array[i+1]-bnd_row_array[i]);
  }
  nnz_bounds[num_threads-1] = _nnz;
  bnd_row_array[num_threads] = _num_rows;
  trace_list[num_threads-1] = new Trace(_nnz-nnz_bounds[num_threads-2],
                                        tr_list_mm_array+3*nnz_bounds[num_threads-2],
                            tr_list_oc_array+nnz_bounds[num_threads-2], num_threads, bnd_row_array[num_threads]-bnd_row_array[num_threads-1]);
#pragma omp parallel for //default(none) shared(num_threads, bnd_row_array, \
  trace_list)
  for (int ii = 0; ii < num_threads; ++ii) {
   int cnt = 0;
   for (int i = bnd_row_array[ii]; i < bnd_row_array[ii+1]; ++i) {
    assert(i <_num_rows);
    for (int j = _Ap[i]; j < _Ap[i+1]; ++j) {
     auto cur_adr = trace_list[ii]->_mem_addr + 3*cnt;
     cur_adr[0] = i; cur_adr[1] = j; cur_adr[2] = _Ai[j];
     //trace->_tuple[j] = new AddMul(cur_adr);
     cnt++;
     //std::cout<<ii<<" - "<<cnt<<" : "<<i <<", "<<j<<", "<<_Ai[j]<<"\n";
    }
   }
  }
  delete []nnz_bounds;
  return trace_list;
 }

}