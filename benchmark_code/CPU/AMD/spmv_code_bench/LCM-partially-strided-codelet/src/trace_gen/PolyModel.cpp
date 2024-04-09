//
// Created by kazem on 7/6/21.
//

#include "PolyModel.h"
#include "Trace.h"


namespace sparse_avx{

 Trace* SectionPolyModel::generate_trace(){//TODO write it using stack
  Trace *t = NULL;
  return t;
 }

 Trace** SectionPolyModel::generate_trace(int num_threads){//TODO write it using
  // stack
  Trace **t = NULL;
  return t;
 }

 Trace*** SectionPolyModel::generate_3d_trace(int num_threads) {//TODO write it
  // using
  // stack
  Trace ***t = NULL;
  return t;
 }

 SectionPolyModel::SectionPolyModel() {

 }

 SectionPolyModel::~SectionPolyModel() {
 }
 /* void SectionPolyModel::generate_trace() {
  int loop_count;
  int *loop_cnt = new int[loop_cnt]();
  int *loop_indent = new int[loop_cnt]();
  int *cur_ub, *cur_lb;
  int *num_iters = new int[loop_count]();
  for (int i = 0; i < loop_count; ++i) {
   num_iters[i] = (ub[i] - lb[i])/loop_indent[i];
  }
  for (int i = 0; i < loop_count; ++i) {
   for (cur_lb[i] = lb[i]; cur_ub[i] < ub[i]; cur_ub[i] +=
     loop_indent[i]) {
    for (loop_cnt[] = 0; j < ; ++j) {

    }
   }
  }
 }*/

 void SectionPolyModel::set_runtime_args(std::initializer_list<Expr> exprs ){
  _runtime_args.insert(_runtime_args.begin(), exprs.begin(), exprs.end());
 }


 int closest_row(int nnz_num, const int *Ap, int init_row=0){
  int i = init_row;
  while ( Ap[i] <= nnz_num )
   i++;
  return i-1;
 }
}