//
// Created by kazem on 7/13/21.
//

#ifndef DDT_SPTRSVMODEL_H
#define DDT_SPTRSVMODEL_H

#include "DDTCodelets.h"
#include "PolyModel.h"

namespace sparse_avx{

 class SpTRSVModel : public SectionPolyModel {
  int _num_rows{}, _num_cols{}, _nnz{};
  int *_Ap{}, *_Ai{};
  int *_Apt{}, *_Ait{};

  int _final_part_no, *_final_level_ptr, *_final_part_ptr,
  *_final_node_ptr;
  int lp_, cp_, ic_;
  bool _bpack;
 protected:

  void iteration_space_prunning(int parts) override;
 public:
  SpTRSVModel();
  ~SpTRSVModel();
  SpTRSVModel(int n, int m, int nnz, int *Ap, int *Ai, int *Apt, int *Ait, int
  lp, int cp, int ip);
  SpTRSVModel(int n, int m, int nnz, int *Ap, int *Ai, int *Apt, int *Ait, int
  lp, int cp, int ip, bool bp);

  Trace* generate_trace() override;
  Trace** generate_trace(int num_threads) override;
  Trace*** generate_3d_trace(int num_threads) override;
  int _final_level_no;
  std::vector<int> _wp_bounds;
  std::vector<DDT::Codelet*>** _cl;
  int _tuning_mode;// zero disable
 };

}


#endif //DDT_SPTRSVMODEL_H
