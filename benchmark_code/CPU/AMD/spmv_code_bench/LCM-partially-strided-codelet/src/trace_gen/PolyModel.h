//
// Created by kazem on 7/6/21.
//

#ifndef SPARSE_AVX512_DEMO_POLYMODEL_H
#define SPARSE_AVX512_DEMO_POLYMODEL_H

#include <vector>
#include <string>
#include <map>
#include "Trace.h"

namespace sparse_avx{

 struct Expr{
  std::string name;
  Expr(std::string n);
 };

 struct Double:public Expr{

 };

 //struct
 struct Statement{
  int op;// operation ID
  int id;// procedence order
  int scope_level;
  Expr *LHS; // LHS Var
  Expr *RHS1, *RHS2; // RHS1 and RHS2
 };


 class SectionPolyModel {

 protected:
  /// This provides a model of a loop nest (bounds and indents) and the
  /// operations inside the loop nest.
  /// Array of loop bounds, should be vars and immediate
  /// Each loop creates a new scope
  ///


  std::vector<Statement> _statement_list;

  /// mapping inputs to the kernels
  std::vector<Expr> _runtime_args;

  virtual void iteration_space_prunning(int parts){}

 public:
  SectionPolyModel();

  ~SectionPolyModel();

  virtual Trace* generate_trace();
  virtual Trace** generate_trace(int num_threads);
  virtual Trace*** generate_3d_trace(int num_threads);

  void set_runtime_args(std::initializer_list<Expr> exprs );


 };


 int closest_row(int nnz_num, const int *Ap, int init_row);

}


#endif //SPARSE_AVX512_DEMO_POLYMODEL_H
