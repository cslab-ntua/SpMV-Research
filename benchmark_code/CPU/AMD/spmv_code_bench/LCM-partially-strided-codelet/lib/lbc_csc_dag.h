//
// Created by kazem on 7/24/21.
//

#ifndef DDT_LBC_CSC_DAG_H
#define DDT_LBC_CSC_DAG_H

namespace sym_lib{

 void lbc_config(int n, int nnz, int num_threads, int &lp, int &cp, int &ic,
                 bool &b_pack);

 int get_coarse_Level_set_DAG_CSC03_parallel(
   size_t n, int *lC, int *lR, int &finaLevelNo, int *&finaLevelPtr, int &partNo,
   int *&finalPartPtr, int *&finalNodePtr, int innerParts, int minLevelDist,
   int divRate, double *nodeCost, int numThreads, bool binPacking = true);

}




#endif //DDT_LBC_CSC_DAG_H
