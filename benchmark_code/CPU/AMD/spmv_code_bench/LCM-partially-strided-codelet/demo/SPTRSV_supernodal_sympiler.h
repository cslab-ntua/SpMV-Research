//
// Created by Zachary Cetinic on 2021-08-24.
//

#ifndef DDT_SPTRSV_SUPERNODAL_SYMPILER_H
#define DDT_SPTRSV_SUPERNODAL_SYMPILER_H

#include "BLAS.h"
#include "SPTRSV_demo_utils.h"
#include "SuperNodalTools.h"
#include <lbc_utils.h>
#include <sparse_io.h>

#include "omp.h"

namespace sym_lib {
    int parallel_cc_behrooz(int *lC, int *lR, int dfsLevel, int ubLevel, int *node2Level,
                    int *levelPtr, int *levelSet, int *node2partition,
                    double *outCost, double *nodeCost, int &curLeveledParCost,
                    int numNodes, int *nodes, int numThreads) {
#pragma omp parallel for num_threads(numThreads)
        for (int i = 0; i < numNodes; ++i) {
            int node = nodes[i];
            node2partition[node] = i;
        }

        bool change = true;
        while (change) {
            change = false;

#pragma omp parallel for num_threads(numThreads)
            for (int i = 0; i < numNodes; ++i) {
                int u = nodes[i];
                // Now we go over all neighbors of u
                for (int r = lC[u]; r < lC[u + 1]; ++r) {
                    int v = lR[r];

                    int level = node2Level[v];
                    if (level < dfsLevel || level >= ubLevel)
                        continue;

                    int comp_u = node2partition[u];
                    int comp_v = node2partition[v];
                    if (comp_u == comp_v)
                        continue;
                    int high_comp = comp_u > comp_v ? comp_u : comp_v;
                    int low_comp = comp_u + (comp_v - high_comp);
                    if (high_comp == node2partition[nodes[high_comp]]) {
                        change = true;
                        node2partition[nodes[high_comp]] = low_comp;
                    }
                }
            }

#pragma omp parallel for num_threads(numThreads)
for (int i = 0; i < numNodes; ++i) {
                int node = nodes[i];
                while (node2partition[node] != node2partition[nodes[node2partition[node]]]) {
                    node2partition[node] = node2partition[nodes[node2partition[node]]];
                }
            }
        }

        int max_cc = -1;
#pragma omp parallel for num_threads(numThreads) reduction(max : max_cc)
        for (int i = 0; i < numNodes; ++i) {
            int node = nodes[i];
            int cc = node2partition[node];
            outCost[cc] += nodeCost[node];
            max_cc = std::max(max_cc, cc + 1);
        }

        return max_cc;
    }
    int worst_fit_bin_pack_behrooz(const std::vector <std::vector<int>> &newLeveledParList,
                           double *inCost,
                           std::vector <std::vector<int>> &mergedLeveledParList,
                           double *outCost, int costThreshold, int numOfBins) {

        int lClusterCnt = 0;
        int partNo = newLeveledParList.size();
        //Sorting the subtree list
        std::vector<subTree> partList(partNo);
        for (int i = 0; i < partNo; ++i) {
            partList[i].cost = inCost[i];
            partList[i].nodeList.insert(partList[i].nodeList.begin(),
                                        newLeveledParList[i].begin(), newLeveledParList[i].end());
        }
        std::sort(partList.begin(), partList.end(), cmp_cost);
        int minBin = 0;
        for (int i = 0; i < partNo; i++) {
            minBin = find_min(outCost, numOfBins);
            outCost[minBin] += partList[i].cost;
            mergedLeveledParList[minBin].insert(
                    mergedLeveledParList[minBin].end(),
                    partList[i].nodeList.begin(), partList[i].nodeList.end());
        }
        return numOfBins;
    }
    void process_l_partition_behrooz(
            int l, int n, int *lC, int *lR, int *partition2Level, int *inDegree,
            int *levelPtr, int *levelSet, int *node2partition, int *node2Level,
            int *nodesAtCurLevel, double *outCost, double *newOutCost, int originalHeight,
            double *nodeCost, std::vector<std::vector<int>> &newLeveledParList,
            bool *visited, int innerParts, std::vector<int> &innerPartsSize,
            int *outinnerPartsList,
            std::vector<std::vector<std::vector<int>>> &mergedLeveledParListByL,
            bool binPacking, int numThreadsForCC, int &totalCC) {
        memset(inDegree, 0, n * sizeof(int));

        int lbLevel = partition2Level[l] - 1;
        int ubLevel = partition2Level[l + 1];
        int dfsLevel = partition2Level[l];
        int curLeveledParCost = 0;
        int numNodesAtCurLevel = 0;

        // Setting up inDegree and nodes in current level
        for (int ii = dfsLevel; ii < ubLevel; ++ii) {
            for (int j = levelPtr[ii]; j < levelPtr[ii + 1]; ++j) {
                int x = levelSet[j];
                nodesAtCurLevel[numNodesAtCurLevel++] = x;
                for (int r = lC[x]; r < lC[x + 1]; ++r) {
                    int cn = lR[r];
                    inDegree[cn]++;
                }
            }
        }

        int cc = parallel_cc_behrooz(lC, lR, dfsLevel, ubLevel, node2Level, levelPtr, levelSet,
                             node2partition, outCost, nodeCost, curLeveledParCost,
                             numNodesAtCurLevel, nodesAtCurLevel, numThreadsForCC);

        // Reset all marked node in the DAG
        for (int j = levelPtr[lbLevel > 0 ? lbLevel : 0]; j < levelPtr[lbLevel + 1];
        ++j) {
            int curNode = levelSet[j];
            visited[curNode] = true;
        }
        // Marking upper bound
        for (int ii = ubLevel; ii < originalHeight; ++ii) {
            for (int j = levelPtr[ii]; j < levelPtr[ii + 1]; ++j) {
                int curNode = levelSet[j];
                visited[curNode] = true; // Make it ready for mod-BFS
            }
        }

        // Topological sort of each cc, the fastest way, FIXME: make it more
        // local
        std::vector<int> extraDim;
        for (int i = 0; i < cc; ++i) {
            newLeveledParList.push_back(extraDim);
        }
        modified_BFS_CSC(n, lC, lR, inDegree, visited, node2partition, levelPtr,
                         levelSet, dfsLevel, newLeveledParList);

        // Marking upper bound
        for (int ii = ubLevel; ii < originalHeight; ++ii) {
            for (int j = levelPtr[ii]; j < levelPtr[ii + 1]; ++j) {
                int curNode = levelSet[j];
                visited[curNode] = false; // Make it ready for mod-BFS
            }
        }
        // Bin packing and form W-partitions

        int outinnerParts = 0;
        totalCC += newLeveledParList.size();
        mergedLeveledParListByL[l].resize(innerPartsSize[l]); // FIXME
        if (binPacking && newLeveledParList.size() > innerPartsSize[l]) {
            int levelParCostThresh = curLeveledParCost / innerParts;
            levelParCostThresh += (0.1 * levelParCostThresh);
            outinnerParts =
                    worst_fit_bin_pack_behrooz(newLeveledParList, outCost, mergedLeveledParListByL[l],
                                       newOutCost, levelParCostThresh, innerPartsSize[l]);
        } else {
            mergedLeveledParListByL[l].erase(mergedLeveledParListByL[l].begin(),
                                             mergedLeveledParListByL[l].end());
            mergedLeveledParListByL[l] = newLeveledParList;
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

        outinnerPartsList[l] = outinnerParts;

        // Cleaning the current sets.
        for (int i = 0; i < newLeveledParList.size(); ++i) {
            newLeveledParList[i].erase(newLeveledParList[i].begin(),
                                       newLeveledParList[i].end());
        }
        newLeveledParList.erase(newLeveledParList.begin(), newLeveledParList.end());
    }
    int height_partitioning_DAG_trng_behrooz(int levelNo, int *levelPtr, int *node2Level,
                                     int originalHeight, int innerParts,
                                     int minLevelDist, int divRate,
                                     std::vector<int> &innerPartsSize,
                                     std::vector <std::vector<int>> &slackGroups,
                                     double *subTreeCost, int *partition2Level,
                                     bool sw) {
        int last_level = 0;
        for(int i = levelNo-1; i >= 0; i--) {
            int size = levelPtr[i+1] - levelPtr[i];
            if(size == 1)
                continue;
            else {
                last_level = i+1;
                break;
            }
        }
        int lClusterCnt = 0;
        int *accuSlackGroups = new int[levelNo];
        if (levelNo <= minLevelDist) {
            partition2Level[0] = 0;
            partition2Level[1] = levelNo;
            innerPartsSize.push_back(1);
            lClusterCnt = 1;
            delete []accuSlackGroups;
            return lClusterCnt;
        }
        //assign the nodes_ in normal level set
        for (int i = 0; i < levelNo; ++i) {
            accuSlackGroups[i] = levelPtr[i + 1] - levelPtr[i];
            assert(accuSlackGroups[i] >= 0);
        }
        int nthreads = innerParts>1 ? innerParts : omp_get_max_threads();

        partition2Level[0] = 0;

        /*if(minLevelDist<=0)
         minLevelDist=2;//default parameter*/
        int tmp = minLevelDist;
        if (tmp > partition2Level[lClusterCnt] && tmp < levelNo) {
            //Due to tuning parameter we need this check
            assert(lClusterCnt < levelNo);
            partition2Level[++lClusterCnt] = tmp;
            int size;
            if(accuSlackGroups[tmp-1] / 2 >= nthreads)
                size = nthreads;
            else if(accuSlackGroups[tmp-1] / 2 > 1)
                size = accuSlackGroups[tmp-1] / 2;
            else
                size = 1;
            innerPartsSize.push_back(size);
        }
        tmp += divRate;
        while (tmp < last_level) {//originalHeight - 1) {
            //Ensures a certain number of level in each partition
            int size;
            if(accuSlackGroups[tmp-1] >= nthreads)
                size = nthreads;
            else if(accuSlackGroups[tmp-1] > 1)
                size = accuSlackGroups[tmp-1];
            else
                size = 1;
            assert(lClusterCnt < levelNo);
            innerPartsSize.push_back(size);
            partition2Level[++lClusterCnt] = tmp;
            tmp += divRate;
        }
        partition2Level[++lClusterCnt] = originalHeight + 1;
        innerPartsSize.push_back(1);//The last partition has one element TODO is this true??
        delete[]accuSlackGroups;
        return lClusterCnt;
    }
    int make_partitions_parallel_behrooz(int n, int *lC, int *lR, int *finaLevelPtr,
                                 int *finalNodePtr, int *finalPartPtr,
                                 int innerParts, int originalHeight,
                                 double *nodeCost, int *node2Level,
                                 std::vector<int> &innerPartsSize, int lClusterCnt,
                                 int *partition2Level, int *levelPtr, int *levelSet,
                                 int numThreads, bool binPacking) {
        if (numThreads == -1) {
            numThreads = omp_get_num_threads();
        }

        std::vector<int> largePartitions;
        std::vector<int> smallPartitions;
        for (int l = 0; l < lClusterCnt; ++l) {
            int nodesAtLevel = 0;
            int lbLevel = partition2Level[l] - 1;
            int ubLevel = partition2Level[l + 1];
            int dfsLevel = partition2Level[l];

            for (int ii = dfsLevel; ii < ubLevel; ++ii) {
                for (int j = levelPtr[ii]; j < levelPtr[ii + 1]; ++j) {
                    int x = levelSet[j];
                    nodesAtLevel++;
                }
            }

            if (nodesAtLevel > 250000) {
                largePartitions.push_back(l);
            } else {
                smallPartitions.push_back(l);
            }
        }

        int totalCC = 0;
        int outinnerPartsList[lClusterCnt];
        std::vector<std::vector<std::vector<int>>> mergedLeveledParListByL;
        mergedLeveledParListByL.resize(lClusterCnt);

        {
            bool *visited = new bool[n]();
            int *xi = new int[2 * n];
            double *outCost = new double[n];
            double *newOutCost = new double[n];
            int *node2partition = new int[n];
            int *inDegree = new int[n];
            std::vector<std::vector<int>> newLeveledParList;
            int *nodesAtCurLevel = new int[n];

            memset(outCost, 0.0, n * sizeof(double));
            memset(newOutCost, 0.0, n * sizeof(double));

#pragma omp for schedule(dynamic, 1)
            for (int i = 0; i < largePartitions.size(); ++i) {
                process_l_partition_behrooz(
                        largePartitions[i], n, lC, lR, partition2Level, inDegree, levelPtr,
                        levelSet, node2partition, node2Level, nodesAtCurLevel, outCost, newOutCost,
                        originalHeight, nodeCost, newLeveledParList, visited, innerParts,
                        innerPartsSize, outinnerPartsList, mergedLeveledParListByL, binPacking,
                        numThreads, totalCC);
            }

            delete[] visited;
            delete[] xi;
            delete[] outCost;
            delete[] newOutCost;
            delete[] node2partition;
            delete[] inDegree;
            delete[] nodesAtCurLevel;
        }

#pragma omp parallel num_threads(numThreads) reduction(+ : totalCC)
{
            bool *visited = new bool[n]();
            int *xi = new int[2 * n];
            double *outCost = new double[n];
            double *newOutCost = new double[n];
            int *node2partition = new int[n];
            int *inDegree = new int[n];
            std::vector<std::vector<int>> newLeveledParList;
            int *nodesAtCurLevel = new int[n];

            memset(outCost, 0.0, n * sizeof(double));
            memset(newOutCost, 0.0, n * sizeof(double));

#pragma omp for schedule(dynamic, 1)
            for (int i = 0; i < smallPartitions.size(); ++i) {
                process_l_partition_behrooz(smallPartitions[i], n, lC, lR, partition2Level, inDegree,
                                    levelPtr, levelSet, node2partition, node2Level,
                                    nodesAtCurLevel, outCost, newOutCost, originalHeight,
                                    nodeCost, newLeveledParList, visited, innerParts,
                                    innerPartsSize, outinnerPartsList,
                                    mergedLeveledParListByL, binPacking, 1, totalCC);
            }

            delete[] visited;
            delete[] xi;
            delete[] outCost;
            delete[] newOutCost;
            delete[] node2partition;
            delete[] inDegree;
            delete[] nodesAtCurLevel;
        }

        int curNumOfPart = 0;
        // combining the state from all the partitions
        for (int l = 0; l < lClusterCnt; ++l) {
            int outinnerParts = outinnerPartsList[l];
            double curPartCost = 0;
            finaLevelPtr[l + 1] = finaLevelPtr[l] + outinnerParts;
            for (int i = 0; i < outinnerParts; ++i) {
                int curPartElem = 0;
                curPartCost = 0;
                for (int j = 0; j < mergedLeveledParListByL[l][i].size(); ++j) {
                    curPartCost += nodeCost[mergedLeveledParListByL[l][i][j]];
                    finalNodePtr[finalPartPtr[curNumOfPart] + curPartElem] =
                            mergedLeveledParListByL[l][i][j];
                    curPartElem++;
                }
#if 0
                std::cout<<curPartCost<<", ";
#endif
                finalPartPtr[curNumOfPart + 1] = finalPartPtr[curNumOfPart] + curPartElem;
                curNumOfPart++;
            }
        }
        return totalCC;
    }
    int get_coarse_Level_set_DAG_CSC03_parallel_NOLEVELSET_V2_behrooz(
            size_t n, int *lC, int *lR, int &finaLevelNo,
            int* finaLevelPtr, int &partNo,
            int* finalPartPtr, int* finalNodePtr,
            int innerParts, int minLevelDist,
            int divRate, int numThreads,
            double *nodeCost,
            int levelNo, int* levelSet, int* levelPtr,
            int* node2Level, bool binPacking = false){
        bool *isChanged = new bool[n]();
        int curNumOfPart = 0;
        std::vector<int> remainingNodes, remainingTmp;
        int clusterCnt = 0;
        int originalHeight = 0;
        finaLevelPtr[0] = 0;

        int averageCC = 0;
#if 0
        for (int i = 0; i < levelNo; ++i) {
            std::cout<<i<<"::";
            for (int j = levelPtr[i]; j < levelPtr[i+1]; ++j) {
                std::cout<<levelSet[j]<<";";
            }
            std::cout<<"\n";
        }
#endif

// H-partitioning
//    int *partition2Level = new int[levelNo + 1]();
        auto* partition2Level = new int[levelNo + 1]();
        std::vector<int> innerPartsSize;
        originalHeight = levelNo;
        std::vector<std::vector<int>> slackGroups(originalHeight + 1);
        std::vector<std::vector<int>> slackedLevelSet(originalHeight + 1);
        int lClusterCnt = height_partitioning_DAG_trng_behrooz(
                levelNo, levelPtr, NULL, originalHeight, innerParts, minLevelDist, divRate,
                innerPartsSize, slackGroups, NULL, partition2Level,1);

        make_partitions_parallel_behrooz(n, lC, lR, finaLevelPtr, finalNodePtr, finalPartPtr,
                                 innerParts, originalHeight, nodeCost, node2Level,
                                 innerPartsSize, lClusterCnt, partition2Level,
                                 levelPtr, levelSet, numThreads, binPacking);

        finaLevelNo = lClusterCnt;
        if (true) { // Verification of the set.
            bool *checkExist = new bool[n];
            for (int i = 0; i < n; ++i)
                checkExist[i] = false;
            for (int i = 0; i < lClusterCnt; ++i) {
                for (int k = finaLevelPtr[i]; k < finaLevelPtr[i + 1]; ++k) {
                    for (int j = finalPartPtr[k]; j < finalPartPtr[k + 1]; ++j) {
                        assert(checkExist[finalNodePtr[j]] == false);
                        checkExist[finalNodePtr[j]] = true;
                    }
                }
            }
            for (int i = 0; i < n; ++i) {
                assert(checkExist[i] == true);
            }
            delete[] checkExist;
        }
        //    delete[] partition2Level;
        delete[] isChanged;
        delete[] partition2Level;

        return averageCC / lClusterCnt;
    }
    int build_levelSet_CSC_V2(size_t n, const int *Lp, const int *Li,
                              int *levelPtr, int *levelSet) {
        int begin = 0, end = n - 1;
        int cur_level = 0, cur_levelCol = 0;
        int *inDegree = new int[n]();
        bool *visited = new bool[n]();
        for (int i = 0; i < Lp[n];
             ++i) {//O(nnz) -> but not catch efficient. This code should work well
            // on millions of none zeros to enjoy a gain in the parformance. Maybe we can find another way. Behrooz
            inDegree[Li[i]]++;// Isn't it the nnz in each row? or the rowptr[x + 1] - rowptr[x] in CSR?
        }
        //print_vec("dd\n",0,n,inDegree);
        while (begin <= end) {
            for (int i = begin; i <= end; ++i) {      //For level cur_level
                if (inDegree[i] == 1 && !visited[i]) {//if no incoming edge
                    visited[i] = true;
                    levelSet[cur_levelCol] = i;//add it to current level
                    cur_levelCol++;//Adding to level-set - This is a cnt for the current level. Behrooz
                }
            }
            cur_level++;//all nodes_ with zero indegree are processed.
            //assert(cur_level < n);
            if (cur_level >= n) return -1;// The input graph has a cycle
            levelPtr[cur_level] =
                    cur_levelCol;// The levelPtr starts from level 1. Behrooz
            while (inDegree[begin] == 1)// Why? Behrooz
            {
                begin++;
                if (begin >= n) break;
            }
            while (inDegree[end] == 1 &&
                   begin <= end)// The same why as above. Behrooz
                end--;
            //Updating degrees after removing the nodes_
            for (int l = levelPtr[cur_level - 1]; l < levelPtr[cur_level];
                 ++l)// I don't get this part. Behrooz
            {
                int cc = levelSet[l];
                for (int j = Lp[cc]; j < Lp[cc + 1]; ++j) {
                    if (Li[j] != cc)      //skip diagonals
                        inDegree[Li[j]]--;//removing corresponding edges
                }
            }
            //print_vec("dd\n",0,n,inDegree);
        }
        delete[] inDegree;
        delete[] visited;
        return cur_level;//return number of levels
    }

    /*
     * @brief Computing Data Dependency Graph of the kernel and store it in CSC format
     * @param n Number of Nodes in the DAG
     * @param DAG_ptr The pointer array in CSC format
     * @param DAG_set The index array in CSC format. It stores the child of each node
     * @return LevelPtr the pointer array in CSC format that store pointers to the nodes inside a level
     * @return LevelSet the nodes are sorted based on level in this array
    */
    timing_measurement computingLevelSet_CSC(int n,
                                             const std::vector<int> &DAG_ptr,
                                             const std::vector<int> &DAG_set,
                                             std::vector<int> &LevelPtr,
                                             std::vector<int> &LevelSet,
                                             int &nlevels) {
        timing_measurement LevelSet_time;
        LevelSet_time.start_timer();
        LevelPtr.resize(n + 1);
        LevelSet.resize(n);
        nlevels = build_levelSet_CSC_V2(n, DAG_ptr.data(), DAG_set.data(),
                                        LevelPtr.data(), LevelSet.data());
        LevelSet_time.measure_elapsed_time();
        return LevelSet_time;
    }

    /*
     * @brief assign the nodes' level to each node and store it in Node2Level
     * @param LevelPtr The pointer array in CSC format
     * @param LevelSet The index array in CSC format
     * @param nlevels Number of levels
     * @param num_nodes
     * @return Node2Level array that stores the data
     */
    timing_measurement computingNode2Level(const std::vector<int> &LevelPtr,
                                           const std::vector<int> &LevelSet,
                                           int nlevels, int num_nodes,
                                           std::vector<int> &Node2Level) {
        timing_measurement Node2Level_time;
        Node2Level_time.start_timer();
        Node2Level.resize(num_nodes);
        for (int lvl = 0; lvl < nlevels; ++lvl) {
            for (int j = LevelPtr[lvl]; j < LevelPtr[lvl + 1]; ++j) {
                int node = LevelSet[j];
                Node2Level[node] = lvl;
            }
        }

        Node2Level_time.measure_elapsed_time();
        return Node2Level_time;
    }

    //================================ SpTrSv Block Vectorized LBC LL ================================
    class SpTrSv_LL_Blocked_LBC : public sparse_avx::SpTRSVSerial {
    protected:
        int nthreads;
        sym_lib::timing_measurement Scheduling_time1, Scheduling_time2;
        std::vector<int> blocked_level_set, blocked_level_ptr,
                blocked_node_to_level;
        int blocked_levelNo, num_supernodes, num_dep_edges;
        std::vector<int> supernode_ptr;
        std::vector<int> LBC_level_ptr, LBC_w_ptr, LBC_node_ptr;
        int LBC_level_no;
        int num_w_part;
        int lp_, cp_, ic_;
        std::vector<int> DAG_ptr;
        std::vector<int> DAG_set;
        std::vector<double> cost;
        supernodaltools::SuperNodal *supernode_obj;

        void build_set() override {
            Scheduling_time2.start_timer();
            assert(L1_csc_ != nullptr);
            assert(L1_csr_ != nullptr);
            LBC_level_ptr.resize(n_ + 1);
            LBC_w_ptr.resize(n_ + 1);
            LBC_node_ptr.resize(n_);

            assert(ic_ > 0);
            get_coarse_Level_set_DAG_CSC03_parallel_NOLEVELSET_V2_behrooz(
                    num_supernodes, DAG_ptr.data(), DAG_set.data(),
                    LBC_level_no, LBC_level_ptr.data(), num_w_part,
                    LBC_w_ptr.data(), LBC_node_ptr.data(), lp_, cp_, ic_,
                    nthreads, cost.data(), blocked_levelNo,
                    blocked_level_set.data(), blocked_level_ptr.data(),
                    blocked_node_to_level.data(), false);
            Scheduling_time2.measure_elapsed_time();
        }

        sym_lib::timing_measurement fused_code() override {
            //sym_lib::rhs_init(L1_csc_->n, L1_csc_->p, L1_csc_->i, L1_csc_->x, x_); // x is b
            sym_lib::timing_measurement t1;
            auto Lp = L1_csr_->p;
            auto Li = L1_csr_->i;
            auto Lx = L1_csr_->x;
            auto x = x_in_;

            t1.start_timer();
#pragma omp parallel
            {
                std::vector<double> tempvec(n_);
                for (int lvl = 0; lvl < LBC_level_no; ++lvl) {
#pragma omp for schedule(static)
                    for (int w_ptr = LBC_level_ptr[lvl];
                         w_ptr < LBC_level_ptr[lvl + 1]; ++w_ptr) {
                        for (int super_ptr = LBC_w_ptr[w_ptr];
                             super_ptr < LBC_w_ptr[w_ptr + 1]; super_ptr++) {
                            int super = LBC_node_ptr[super_ptr];
                            int start_row = supernode_ptr[super];
                            int end_row = supernode_ptr[super + 1];
                            //ncol is the number of columns in the off-diagonal block
                            int num_off_diag_col =
                                    Lp[start_row + 1] - Lp[start_row] - 1;
                            assert(num_off_diag_col >= 0);
                            int nrows = end_row - start_row;
                            assert(nrows > 0);
                            //Solving the independent part
                            //Copy x[Li[col]] into a continues buffer
                            for (int col_ptr = Lp[supernode_ptr[super]], k = 0;
                                 col_ptr <
                                 Lp[supernode_ptr[super]] + num_off_diag_col;
                                 col_ptr++, k++) {
                                tempvec[k] = x[Li[col_ptr]];
                            }
                            custom_blas::SpTrSv_MatVecCSR_BLAS(
                                    nrows, num_off_diag_col, &Lx[Lp[start_row]],
                                    tempvec.data(), &x[start_row]);
                            custom_blas::SpTrSv_LSolveCSR_BLAS(
                                    nrows, num_off_diag_col,
                                    &Lx[Lp[start_row] + num_off_diag_col],
                                    &x[start_row]);
                        }
                    }
                }
            }
            t1.measure_elapsed_time();
            sym_lib::copy_vector(0, n_, x_in_, x_);
            return t1;
        }

    public:
        /*
   * @brief Class constructor
   * @param L Sparse Matrix with CSR format
   * @param L_csc Sparse Matrix with CSC format
   * @param correct_x The right answer for x
   * @param name The name of the algorithm
   * @param nt number of threads
   */
        SpTrSv_LL_Blocked_LBC(sym_lib::CSR *L, sym_lib::CSC *L_csc,
                              double *correct_x, std::string name, int lp)
            : SpTRSVSerial(L, L_csc, correct_x, name) {
            L1_csr_ = L;
            L1_csc_ = L_csc;
            correct_x_ = correct_x;
            lp_ = lp;
            nthreads = lp;


            //Calculating The Dependency DAG and the levelset
            sym_lib::timing_measurement block_LBC_time;
            Scheduling_time1.start_timer();
            supernodaltools::SuperNodal super_node_obj(
                    supernodaltools::CSR_TYPE, L1_csc_, L1_csr_, L1_csc_, 0);

            super_node_obj.getDependencyDAGCSCformat(
                    DAG_ptr, DAG_set, num_supernodes, num_dep_edges);

            sym_lib::computingLevelSet_CSC(num_supernodes, DAG_ptr, DAG_set,
                                           blocked_level_ptr, blocked_level_set,
                                           blocked_levelNo);

            sym_lib::computingNode2Level(blocked_level_ptr, blocked_level_set,
                                         blocked_levelNo, num_supernodes,
                                         blocked_node_to_level);

            cost.resize(num_supernodes, 0);
            super_node_obj.getSuperNodeGroups(supernode_ptr);
            for (int super = 0; super < num_supernodes; ++super) {
                for (int i = supernode_ptr[super]; i < supernode_ptr[super + 1];
                     i++) {
                    cost[super] += L1_csr_->p[i + 1] - L1_csr_->p[i];
                }
            }
            Scheduling_time1.start_timer();
        };

        double getSchedulingTime() {
            return Scheduling_time1.elapsed_time +
                   Scheduling_time2.elapsed_time;
        }

        void setP2_P3(int p2, int p3) {
            this->cp_ = p2;
            this->ic_ = p3;
        }
        ~SpTrSv_LL_Blocked_LBC() = default;
    };
}// namespace sym_lib
#endif// DDT_SPTRSV_SUPERNODAL_SYMPILER_H
