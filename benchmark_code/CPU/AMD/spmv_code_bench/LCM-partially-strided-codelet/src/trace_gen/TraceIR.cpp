//
// Created by kazem on 7/6/21.
//

#include "TraceIR.h"
namespace sparse_avx{


  Tuple::~Tuple() {
//  delete []memory_trace;
 }

 Tuple::Tuple(int *ms):memory_trace(ms) {}


 AddMul::AddMul(int *mt): Tuple(mt) {}

 TRACE_OP AddMul::get_id() {
  return TRACE_OP::AddM;
 }

}