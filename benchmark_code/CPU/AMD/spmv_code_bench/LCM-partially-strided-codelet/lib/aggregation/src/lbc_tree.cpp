//
// Created by kazem on 2/6/20.
//

#include <sparse_utilities.h>
#include <sparse_inspector.h>
#include <lbc_utils.h>

namespace sym_lib {
 int get_coarse_levelSet_tree(size_t n,
                              const int *eTree,
                              int &finaLevelNo,
                              int *&finaLevelPtr,
                              int &partNo,
                              int *&finalPartPtr,
                              int *&finalNodePtr,
                              int innerParts,
                              int minLevelDist,
                              int divRate,
                              double *nodeCost) {
  finalPartPtr = new int[n]();
  finalNodePtr = new int[n];
//  auto *nodeColWidth = new int[n]();
  partNo = 0;
  finaLevelPtr = new int[n + 1];
  //parLevelSet = new int[n];
/*  for (int m = 0; m < n; ++m) {//Number of columns in each SN
   nodeColWidth[m] = blk2col[m + 1] - blk2col[m];
  }*/

#if 0
  std::cout<<"nodeCost:\n";
  for (int i1 = 0; i1 < n; ++i1) {
   std::cout<<nodeCost[i1]<<",";
  }
  std::cout<<"\n";
#endif



  double overallCost = 0;
  auto nodeHeight = new int[n]();
  auto slackNumber = new int[n]();
  std::vector<int> criticalLeaves;
  int *levelPtr = new int[n + 1]();
  auto levelSet = new int[n]();
  int *node2Level = new int[n]();
  auto childPtrSubtree = new int[n + 1](),
    childNoSubtree = new int[n](),
    nChildTmp = new int[n]();
  double fracThr = 0.2;
  int curNumOfPart = 0;
  finaLevelPtr[0] = 0;
  std::vector <std::vector<int>> newLeveledParList, mergedLeveledParList;
/* for (int i = 0; i < partitionNo; ++i) {
  partitionMinLevels[i]=INT_MAX;
 }*/
//#pragma omp parallel for
  for (int i = 0; i < n; ++i) {
   nChildTmp[i] = 0;
  }
  for (int l = 0; l < n; ++l) {
   int p = eTree[l];
   if (p >= 0) {
    nChildTmp[p]++;
   }
  }
  //Find the number of connected parts
  int treeParts = 0, treePartsOrig = 0;
  for (int j = 0; j < n; ++j) {
   if (eTree[j] < 0)
    treePartsOrig++;
  }



  //int levelNo = build_level_set_tree(n, eTree, levelPtr, levelSet);
  int levelNo = build_level_set_tree_efficient(n, eTree, nChildTmp,
                                               levelPtr, levelSet, node2Level);

  //Finding node levels in original ETree
  //TODO also find the unweighted version of eTree, there might be more levels ignoring them.
  //timing_measurement t2;
  //t2.start_timer();
  //int originalHeight = get_tree_height_efficient(n, eTree, nChildTmp);
  int originalHeight = levelNo;
  //t2.measure_elapsed_time();
  //std::cout<<"==: "<<t2.elapsed_time<<"\n";
  //std::cout<<"*** : "<<originalHeight<<"\n";
  //int originalHeight2 = get_tree_height(n, eTree, nChildTmp);
  //assert(originalHeight==originalHeight2);
  if (originalHeight == 0 && n == 1) {// e.g., one super-node
   finaLevelNo = 1;
   finaLevelPtr[0] = 0;
   finaLevelPtr[1] = 1;
   partNo = 1;
   finalPartPtr[0] = 0;
   finalPartPtr[1] = 1;
   finalNodePtr[0] = 0;
   return finaLevelNo;
  }
#if 0
  for (int i = 0; i < n; ++i) {
   nodeHeight[i] = get_node_depth(i, n, eTree);
   if (nodeHeight[i] == originalHeight)
    criticalLeaves.push_back(i);
   assert(nodeHeight[i] >= 0);
  }
#endif
  //Computing subtree cost in Etree
  double *subTreeCost = compute_subtree_cost(n, eTree, nodeCost);

  //Computing slack number for each node
  int cntSlack = 0, costSlack = 0;
  std::vector <std::vector<int>> slackGroups(originalHeight + 1);
  std::vector <std::vector<int>> slackedLevelSet(originalHeight + 1);
  //First compute node2level

  //std::cout<<"--;"<<levelNo<<","<<n<<";";
/*  for (int i = 0; i < levelNo; ++i) {
   for (int j = levelPtr[i]; j < levelPtr[i + 1]; ++j) {
    assert(levelSet[j] < static_cast<int>(n) && levelSet[j] >= 0);
    node2Level[levelSet[j]] = i;
   // std::cout<<levelSet[j]<<",";
   }
  // std::cout<<"\n";
  }*/
  //computing the cost of each level,
  auto levelCost = new double[levelNo]();
  auto curLevelCost = new double[levelNo]();
  auto levelParCost = new double[levelNo]();
  for (int i = 0; i < n; ++i) {
   assert(node2Level[i] < levelNo && node2Level[i] >= 0);
   levelCost[node2Level[i]] += nodeCost[i];
   overallCost += nodeCost[i];
  }
  //Find the nodes with slack and stores it in slackgroups
#if 0
  int maxLevelWithSlack = 0;
  for (int i = 0; i < n; ++i) {
   slackNumber[i] = originalHeight - nodeHeight[i] - node2Level[i];
   assert(slackNumber[i] <= originalHeight);
   //Postpone the node i, to higher levels

   if (slackNumber[i] > 0) {
    slackGroups[node2Level[i] + slackNumber[i]].push_back(i);
    if (node2Level[i] + slackNumber[i] > maxLevelWithSlack)
     maxLevelWithSlack = node2Level[i] + slackNumber[i];
   } else {//We can't postpone these nodes
    slackedLevelSet[node2Level[i]].push_back(i);
    curLevelCost[node2Level[i]] += nodeCost[i];
   }
   //slackGroups[slackNumber[i]].push_back(i);
  }
#endif

//Finding the height partitioning
  int *partition2Level = new int[levelNo + 1]();
  std::vector<int> innerPartsSize;
  int lClusterCnt = height_partitioning(levelNo,
                                       levelPtr, NULL, originalHeight,
                                       innerParts, minLevelDist,
                                       divRate, innerPartsSize, slackGroups,
                                       subTreeCost, partition2Level);

  //
#if 0
  //Moving the slack nodes around and make a new level-set
  makeSlackedLevelSet(n,lClusterCnt,partition2Level,originalHeight,
  slackGroups,slackedLevelSet,node2Level);
#if 0
  bool *nodesExistVerif = new bool[n]();
  for (int i = 0; i < slackedLevelSet.size(); ++i) {
   for (int j = 0; j < slackedLevelSet[i].size(); ++j) {
    assert(slackedLevelSet[i][j] < n);
    nodesExistVerif[slackedLevelSet[i][j]] = true;
   }
  }
  for (int i = 0; i < n; ++i) {
   assert(nodesExistVerif[i] == true);
  }
  delete []nodesExistVerif;
#endif
  //Creating new level-set after Slack number applied.
  int curElement=0;
  levelPtr[0]=0;
  for (int i = 0; i < slackedLevelSet.size(); ++i) {
   for (int j = 0; j < slackedLevelSet[i].size(); ++j) {
    assert(slackedLevelSet[i][j] < n && slackedLevelSet[i][j]>=0);
    node2Level[slackedLevelSet[i][j]]=i;
    levelSet[curElement++]=slackedLevelSet[i][j];
   }
   levelPtr[i+1]=curElement;
  }
  //If slackgroup is not empty yet, distribute it in levels evenly
#endif
  //recomputing levelcost
/*  for (int i = 0; i < levelNo; ++i) levelCost[i] = 0;
  overallCost = 0;
  for (int i = 0; i < n; ++i) {
   assert(node2Level[i] < levelNo);
   levelCost[node2Level[i]] += nodeCost[i];
   overallCost += nodeCost[i];
  }*/



  //TODO:  removing the nodes that have many edge cuts

  //TODO: partition and reorder the nodes within each level partition
  //TODO: First make the partition that has nodes with slack zero
  //Create a tree for each level partition
  auto tmpTree = new int[n];
  auto outCost = new double[n](),
    newOutCost = new double[n]();
  auto outNode2Par = new int[n]();
  int outSize = 0;
  double ccc = 0;

//#pragma omp parallel for
  for (int i = 0; i < n; ++i)
   tmpTree[i] = -2;
//timing_measurement tt;
//tt.start_timer();
  for (int l = 0; l < lClusterCnt; ++l) {//for each leveled partition
 //  tt.measure_elapsed_time();
   int curLeveledParCost = 0;
   innerParts = innerPartsSize[l];
   //Create tmpTree for the partition l
   for (int i = partition2Level[l]; i < partition2Level[l + 1]; ++i) {
    curLeveledParCost += levelCost[i];
    for (int j = levelPtr[i]; j < levelPtr[i + 1]; ++j) {
     int curNode = levelSet[j];
     int par = eTree[curNode];
     //if par<0 then it is root and there is no parent so, the level is max.
     int levelOfPar = par >= 0 ? node2Level[par] : originalHeight + 1;
     assert(levelOfPar <= static_cast<int>(n) && levelOfPar >= 0);
     if (levelOfPar < partition2Level[l + 1] &&
         l + 1 <= lClusterCnt &&
         levelOfPar >= partition2Level[l]) {
      //The parent is in the same partition
      tmpTree[curNode] = eTree[curNode];//copy only those in the partition
      ccc += nodeCost[curNode];
     } else {
      tmpTree[curNode] = -1; //The edge-cut happens here
     }
    }
   }
   //Here we have the ETree for the lth leveled partition

   //Now compute the number of children for the etree of current partition
   for (int i = 0; i < n; ++i) nChildTmp[i] = 0;
   populate_children(n, tmpTree, childPtrSubtree, childNoSubtree, nChildTmp);
#if 0
   for (int m = 0; m < n; ++m) {
             std::cout<<nChildTmp[m]<<";";
         }
         std::cout<<"\n";
#endif
   //partition it using the partitioning function call
   outSize = 0;
//#pragma omp parallel for
   for (int k = 0; k < n; ++k) {
    outNode2Par[k] = 0;
    outCost[k] = 0;
   }
   treeParts = 0;
//#pragma omp parallel for reduction(+:treeParts)
   for (int j = 0; j < n; ++j) {
    if (tmpTree[j] == -1) {
     treeParts++;
    }
   }
#if 0
   std::cout<<treeParts<<";*;";
   for (int i = 0; i < n; ++i) {
             std::cout<<tmpTree[i]<<";";
         }
         std::cout<<"\n";
#endif
   int outinnerParts = 0;
/*  if(l == 1){//If there is a tree left,
   // if it is a forest, need to be ran in parallel
   //levelParCostThresh=3*curLeveledParCost;
   innerParts=1;
  }*/
   int levelParCostThresh = curLeveledParCost / innerParts;
   levelParCostThresh += (0.1 * levelParCostThresh);
   post_order_spliting(n, tmpTree, nodeCost, childPtrSubtree, childNoSubtree,
                     nChildTmp, curLeveledParCost, innerParts,
                     outSize, outCost, outNode2Par, newLeveledParList);
/*   double tmpp = 0;
   for (int i = 0; i < newLeveledParList.size(); ++i) {
    for (int j = 0; j < newLeveledParList[i].size(); ++j) {
     assert(newLeveledParList[i][j] < n);
     tmpp += nodeCost[newLeveledParList[i][j]];
    }
   }*/
   //assert(tmpp == curLeveledParCost);
   //the partitioning is found, now we need merge some of those if the number is larger than
   //inner parts. It should be typically bigger since the input is a forest.
   // merging the final partitioning

   mergedLeveledParList.resize(innerParts);
   if (newLeveledParList.size() > innerParts) {
    outinnerParts = worst_fit_bin_pack(newLeveledParList, outCost,
                                    mergedLeveledParList, newOutCost,
                                    levelParCostThresh, innerPartsSize[l]);
    assert(outinnerParts <= innerParts);
   } else {
    mergedLeveledParList.erase(mergedLeveledParList.begin(), mergedLeveledParList.end());
    mergedLeveledParList = newLeveledParList;
    outinnerParts = newLeveledParList.size();
#if 0
    if(outinnerParts>1) {
     for (int ii = 0; ii < outinnerParts; ++ii) {
      std::cout << outCost[ii] << ";";
     }
     for (int ii = outinnerParts; ii < innerParts; ++ii) {
      std::cout << "0;";
     }
    }
#endif
   }
   //This is for mapping
/*  if(l==0)
  swapWSet(mergedLeveledParList,1,4);*/
   //resetting temporary ETree
   for (int j = 0; j < n; ++j) {
    tmpTree[j] = -2;
   }
   //putting the inner partitions into the final level set
   // int *finaLevelPtr, int *finalPartPtr, int *finalNodePtr
   double curPartCost = 0;
   finaLevelPtr[l + 1] = finaLevelPtr[l] + outinnerParts;

   for (int i = 0; i < outinnerParts; ++i) {
    int curPartElem = 0;
    curPartCost = 0;
    for (int j = 0; j < mergedLeveledParList[i].size(); ++j) {
     curPartCost += nodeCost[mergedLeveledParList[i][j]];
     finalNodePtr[finalPartPtr[curNumOfPart] + curPartElem] = mergedLeveledParList[i][j];
     outNode2Par[mergedLeveledParList[i][j]] = curNumOfPart;
     curPartElem++;
    }
#if 0
    //std::cout<<"parts: "<<newLeveledParList.size()<<","<<curPartCost<<", ";
    std::cout<<curPartCost<<", ";
#endif
    finalPartPtr[curNumOfPart + 1] = finalPartPtr[curNumOfPart] + curPartElem;
    curNumOfPart++;
   }
#if 0
   std::cout<<"\n";
#endif
#if 0
   int cc=0;
         for (int m = 0; m < mergedLeveledParList.size(); ++m) {
             std::cout<<"Partition# "<<m<<": ";
             for (int n = 0; n < mergedLeveledParList[m].size(); ++n) {
                 std::cout<<mergedLeveledParList[m][n]<<";";
                 cc+=nodeCost[mergedLeveledParList[m][n]];
             }
             std::cout<<"\n";
         }
         std::cout<<"\n"<<cc<<"\n";
#endif

   //Cleaning the current sets.
   for (int i = 0; i < mergedLeveledParList.size(); ++i) {
    mergedLeveledParList[i].erase(mergedLeveledParList[i].begin(), mergedLeveledParList[i].end());
   }
   mergedLeveledParList.erase(mergedLeveledParList.begin(), mergedLeveledParList.end());
   for (int i = 0; i < newLeveledParList.size(); ++i) {
    newLeveledParList[i].erase(newLeveledParList[i].begin(), newLeveledParList[i].end());
   }
   newLeveledParList.erase(newLeveledParList.begin(), newLeveledParList.end());
   //std::cout<<"parts: "<<outinnerParts<<"\n";
//  std::cout<<outinnerParts<<",";
//t.measure_elapsed_time();
  }
//  tt.measure_elapsed_time();
//  tt.print_t_array();
//  std::cout<<"\n";
  //t.print_t_array();

  finaLevelNo = lClusterCnt;
  if (false) {//Verification of the set.
   bool *checkExist = new bool[n];
   for (int i = 0; i < n; ++i) checkExist[i] = false;
   for (int i = 0; i < lClusterCnt; ++i) {
    for (int k = finaLevelPtr[i]; k < finaLevelPtr[i + 1]; ++k) {
     for (int j = finalPartPtr[k]; j < finalPartPtr[k + 1]; ++j) {
      assert(checkExist[finalNodePtr[j]] == false);
      int par = eTree[finalNodePtr[j]] >= 0 ? eTree[finalNodePtr[j]] : n - 1;
      assert(outNode2Par[finalNodePtr[j]] <= outNode2Par[par]);
      checkExist[finalNodePtr[j]] = true;
     }
    }
   }
   for (int i = 0; i < n; ++i) {
    assert(checkExist[i] == true);
   }
   delete[] checkExist;
  }
  partNo = finaLevelPtr[finaLevelNo];
  delete[] nodeHeight;
  delete[] slackNumber;
  delete [] node2Level;
  delete[] tmpTree;
  delete[] outCost;
  delete[] newOutCost;
  delete[] outNode2Par;
  delete[] childPtrSubtree;
  delete[] childNoSubtree;
  delete[] nChildTmp;
  delete[] levelCost;
  delete[] levelParCost;
  delete[]  curLevelCost;
  delete[] partition2Level;
  delete[] subTreeCost;
  delete[] levelPtr;
  delete[] levelSet;


 // delete[]nodeColWidth;
  return finaLevelNo;

 }

}