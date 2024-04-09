//
// Created by kazem on 10/12/19.
//

#define DBG_LOG
#define CSV_LOG

#include <iostream>
#include <sparse_io.h>
#include <test_utils.h>
#include <sparse_utilities.h>
#include <lbc.h>

#ifdef METIS
#include <metis_interface.h>
#endif



using namespace sym_lib;

/// Evaluate LBC based on random matrices or a given MTX matrix/graph.
/// \return
int lbc_demo(int argc, char *argv[]);

int main(int argc, char *argv[]){
 int ret_val;
 ret_val = lbc_demo(argc,argv);
 return ret_val;
}



int lbc_demo(int argc, char *argv[]){
 CSC *L1_csc, *A = NULLPNTR;
 size_t n;
 int num_threads = 6;
 int p2 = -1, p3 = 4000; // LBC params
 int header = 0;
 int *perm;
 std::string matrix_name;
 std::vector<timing_measurement> time_array;
 if (argc < 2) {
  PRINT_LOG("Not enough input args, switching to random mode.\n");
  n = 16;
  double density = 0.2;
  matrix_name = "Random_" + std::to_string(n);
  A = random_square_sparse(n, density);
  if (A == NULLPNTR)
   return -1;
  L1_csc = make_half(A->n, A->p, A->i, A->x);
 } else {
  std::string f1 = argv[1];
  matrix_name = f1;
  L1_csc = read_mtx(f1);
  if (L1_csc == NULLPNTR)
   return -1;
  n = L1_csc->n;
 }
 if(argc >= 3)
  p2 = atoi(argv[2]);
 if(argc >= 4)
  p3 = atoi(argv[3]);
 /// Re-ordering L matrix
#ifdef METIS
 //We only reorder L since dependency matters more in l-solve.
 //perm = new int[n]();
 CSC *L1_csc_full = make_full(L1_csc);
 delete L1_csc;
 metis_perm_general(L1_csc_full, perm);
 L1_csc = make_half(L1_csc_full->n, L1_csc_full->p, L1_csc_full->i,
                    L1_csc_full->x);
 CSC *Lt = transpose_symmetric(L1_csc, perm);
 CSC *L1_ord = transpose_symmetric(Lt, NULLPNTR);
 delete L1_csc;
 L1_csc = L1_ord;
 delete Lt;
 delete L1_csc_full;
 delete[]perm;
#endif


 int final_level_no, *fina_level_ptr, *final_part_ptr, *final_node_ptr;
 int part_no;
 int lp = num_threads, cp = p2, ic= p3;

 auto *cost = new double[n]();
 for (int i = 0; i < n; ++i) {
  cost[i] = L1_csc->p[i+1] - L1_csc->p[i];
 }

 get_coarse_levelSet_DAG_CSC_tree(n, L1_csc->p, L1_csc->i, L1_csc->stype,
   final_level_no,
   fina_level_ptr,part_no,
   final_part_ptr,final_node_ptr,
   lp,cp, ic, cost);
 
 print_hlevel_set("HLevel set:\n", final_level_no, fina_level_ptr, final_part_ptr, final_node_ptr);

 delete []fina_level_ptr;
 delete []final_part_ptr;
 delete []final_node_ptr;
 delete []cost;
 delete A;
 delete L1_csc;
 return 0;
}
