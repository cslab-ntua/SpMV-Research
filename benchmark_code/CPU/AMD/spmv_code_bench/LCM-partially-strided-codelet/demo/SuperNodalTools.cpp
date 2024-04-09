#include "SuperNodalTools.h"
#include <unordered_set>

namespace supernodaltools{
    int BCSCMatrix::supernodes(sym_lib::CSC *A, int limit, bool isLower) {
        int n = A->n;

        int num_nodes = 0;
        int *temp_nodes = new int[n]();

        if (limit == 0){
            limit = n;
        }
        int max = 0;
        int pre, cur;

        if (isLower) {
            for (int i = 0; i < n; i++) {
                // start of new block
                temp_nodes[num_nodes] = i;
                num_nodes++;

                pre = i;
                cur = i + 1;

                int sim = 0;
                while (cur < n && sim < limit) {
                    int diff = cur - pre;

                    // compare number of off diagonals
                    int off_pre = A->p[pre + 1] - A->p[pre] - diff;
                    int off_cur = A->p[cur + 1] - A->p[cur];

                    if (off_pre != off_cur)
                        break;

                    // now examine row pattern
                    bool col_sim = true;
                    for (int j = 0; j < off_cur; j++) {
                        int pre_index = A->p[pre] + diff + j;
                        int cur_index = A->p[cur] + j;
                        if (A->i[pre_index] != A->i[cur_index]) {
                            col_sim = false;
                            break;
                        }
                    }

                    if (col_sim) {
                        sim++;
                        cur++;
                    } else {
                        break;
                    }
                }
                i = pre + sim;
                if (sim > max){
                    max = sim;
                }
            }
        } else {
            for (int i = n - 1; i >= 0; i--) {
                // start of new block
                temp_nodes[num_nodes] = i;
                num_nodes++;

                pre = i;
                cur = i - 1;

                int sim = 0;
                while (cur >= 0 && sim < limit) {
                    int diff = pre - cur;

                    // compare number of off diagonals
                    int off_pre = A->p[pre + 1] - A->p[pre] - diff;
                    int off_cur = A->p[cur + 1] - A->p[cur];

                    if (off_pre != off_cur)
                        break;

                    // now examine row pattern
                    bool col_sim = true;
                    for (int j = 0; j < off_cur; j++) {
                        int pre_index = A->p[pre + 1] - diff - j - 1;
                        int cur_index = A->p[cur + 1] - j - 1;

                        if (A->i[pre_index] != A->i[cur_index]) {
                            col_sim = false;
                            break;
                        }
                    }

                    if (col_sim) {
                        sim++;
                        cur--;
                    } else
                        break;
                }
                i = pre - sim;

                if (sim > max)
                    max = sim;
            }
        }

        M->supernodes = new int[num_nodes + 1];
        std::memcpy(M->supernodes, temp_nodes, sizeof(int) * num_nodes);
        if (isLower){
            M->supernodes[num_nodes] = n;
        } else {
            M->supernodes[num_nodes] = -1;
        }

        delete[]temp_nodes;
        return num_nodes;
    }

    int BCSCMatrix::calcSize(sym_lib::CSC *A) {
        int i, j, p;
        int n = A->n;
        int *Ap = A->p;

        int nnz = 0;
        bool *in_block = new bool[n]();
        int *temprows = new int[n]();
        M->p = new int[n + 1]();

        for (i = 0; i < M->nodes; i++) {
            M->p[i] = nnz;

            // find the entire block size
            int nrow = 0;
            int width = M->supernodes[i + 1] - M->supernodes[i];
            for (j = M->supernodes[i]; j < M->supernodes[i + 1]; j++) {
                for (p = Ap[j]; p < Ap[j + 1]; p++) {
                    int row_i = A->i[p];

                    if (!in_block[row_i]) {
                        in_block[row_i] = true;
                        nnz += width;
                        temprows[nrow] = row_i;
                        nrow++;
                    }
                }
            }
            for (j = 0; j < nrow; j++)
                in_block[temprows[j]] = false;
        }
        M->p[M->nodes] = nnz;

        delete[]temprows;
        delete[]in_block;
        return nnz;
    }

    void BCSCMatrix::createFormat(sym_lib::CSC *A) {
        int i, j, p;
        int counter = 0;

        int n = A->n;
        int *Ap = A->p;
        int *Ai = A->i;
        double *Ax = A->x;

        // temporary dynamic array to store Ai and Ax
        int *tempvec = new int[n]();
        bool *in_block = new bool[n]();
        std::priority_queue<int, std::vector<int>, std::greater<int>> queue;

        for (i = 0; i < M->nodes; i++) {
            // find the entire block size
            int nrow = 0;
            for (j = M->supernodes[i]; j < M->supernodes[i + 1]; j++) {
                for (p = Ap[j]; p < Ap[j + 1]; p++) {
                    int row_i = Ai[p];

                    if (!in_block[row_i]) {
                        in_block[row_i] = true;
                        queue.push(row_i);
                        nrow++;
                    }
                }
            }
            M->nrows[i] = nrow;

            // obtain order of the rows
            int temp_cnt = 0;
            while (!queue.empty()) {
                int row_i = queue.top();
                queue.pop();
                in_block[row_i] = false;
                tempvec[temp_cnt] = row_i;
                temp_cnt++;
            }

            // initialize temp_Ai and temp_Ax
            for (j = M->supernodes[i]; j < M->supernodes[i + 1]; j++) {
                int index = Ap[j];
                for (p = 0; p < temp_cnt; p++) {
                    int row_i = tempvec[p];
                    M->i[counter] = row_i;
                    if (row_i == Ai[index] && index < Ap[j + 1]) {
                        M->x[counter] = Ax[index];
                        index++;
                    } else {
                        M->x[counter] = 0.0;
                    }
                    counter++;
                }
            }
        }

        delete[]tempvec;
        delete[]in_block;
    }

    sym_lib::CSC *BCSCMatrix::compressed_BCSC_to_CSC() {
        int nodes = M->nodes;
        const int *supernodes = M->supernodes;

        int counter = 0;
        auto supers = new int[M->n](); // A, B, C have same supernodal structure
        for (int i = 0; i < nodes; i++) {
            while (counter >= supernodes[i] && counter < supernodes[i + 1]) {
                supers[counter] = i;
                counter++;
            }
        }

        int nnz = 0;
        auto inplace = new bool[nodes]();

        int count = 0;
        auto temp = new int[nodes]();

        for (int i = 0; i < nodes; i++) {
            for (int r = 0; r < M->nrows[i]; r++) {
                int row = supers[M->i[M->p[i] + r]];
                if (!inplace[row]) {
                    inplace[row] = true;
                    temp[count] = row;
                    count++;
                    nnz++;
                }
            }
            for (int j = 0; j < count; j++)
                inplace[temp[j]] = false;
            count = 0;
        }

        sym_lib::CSC *Acsc = new sym_lib::CSC(nodes, nodes, nnz);
        auto Ap = Acsc->p;
        auto Ai = Acsc->i;

        int p = 1, i = 0;
        Ap[0] = 0;
        for (int j = 0; j < nodes; j++) {
            Ap[p] = Ap[p - 1];
            for (int r = 0; r < M->nrows[j]; r++) {
                int row = supers[M->i[M->p[j] + r]];
                if (!inplace[row]) {
                    inplace[row] = true;
                    temp[count] = row;
                    count++;

                    Ap[p]++;
                    Ai[i] = row;
                    i++;
                }
            }
            for (int k = 0; k < count; k++)
                inplace[temp[k]] = false;
            count = 0;

            Ai[i] = j + nodes;
            Ap[p]++;
            i++;
            p++;
        }

        delete[]inplace;
        delete[]supers;
        delete[]temp;
        return Acsc;
    }

    SuperNodal::SuperNodal(MatrixType type, const sym_lib::CSC* A_CSC, const sym_lib::CSR* A_CSR, const sym_lib::CSC* Dep_DAG, int limit) {
        this->input_type = type;
        this->A_CSR = A_CSR;
        this->A_CSC = A_CSC;
        assert(A_CSC != nullptr); // There is a bug in using Dep DAG that I don't understand
        this->Dep_DAG = A_CSC;
        this->num_supernodes = 0;
        this->num_task_dep_nz = 0;
        //Checking stuff
        if(input_type == CSC_TYPE){
            assert(A_CSC != nullptr);
            if(limit < 1){ this->limit = A_CSC->n; }
            node_to_sup.resize(A_CSC->n, 0);
        } else if(input_type == CSR_TYPE){
            assert(A_CSR != nullptr);
            if(limit < 1){ this->limit = A_CSR->n; }
            node_to_sup.resize(A_CSR->n, 0);
        } else {
            std::cerr << "The input type is not defined" << std::endl;
            return;
        }
        if(limit > 0){
            this->limit = limit;
        }

        //Generating the supernode
        if(input_type == CSR_TYPE){
            createCSRSupernode();
        } else if(input_type == CSC_TYPE){
            createCSCSupernode();
        } else {
            std::cerr << "The input type is not defined" << std::endl;
            return;
        }
        createDepDAG();
    }

    void SuperNodal::createCSRSupernode() {
        //For now it only support lower matrices
        auto n = A_CSR -> n;
        auto Lp = A_CSR -> p;
        auto Li = A_CSR -> i;
        //It is a CSC/CSR like format
        this->supernodes_ptr.push_back(0);
        for (int row = 0; row < n; ) {
            int width = 1;
            int first = row;
            int last = row + 1;
            while (last < n && width < limit) {
                int diff = last - first;
                // compare number of off diagonals
                int off_last  = Lp[last + 1] - Lp[last] - diff;
                int off_first = Lp[first + 1] - Lp[first];
                // If you don't understand this line
                // get back and read about supernode .. don't waste your time
                if (off_first != off_last)
                    break;
                assert(off_first > 0 && off_last > 0);
                // now examine column pattern
                bool can_form_supernode = true;
                for (int col = 0; col < off_first; col++) {
                    if (Li[Lp[first] + col] != Li[Lp[last] + col]) {
                        can_form_supernode = false;
                        break;
                    }
                }
                if (can_form_supernode) {
                    //Increase the width of the supernode
                    width++;
                    //Lets check the next row
                    last++;
                } else {
                    //Lets go to the next row, this super node is done
                    break;
                }
            }
            //The starting point of the next supernode
            row = first + width;
            assert(row <= n);
            this->supernodes_ptr.push_back(row);
            assert(Lp[first + 1] - Lp[first] - 1 >= 0);
            this->off_diag_size.push_back(Lp[first + 1] - Lp[first] - 1);
        }
        //The end of the nodes should be equal to n
        assert(supernodes_ptr.back() == n);
        //Storing the number of nodes
        this->num_supernodes = supernodes_ptr.size() - 1;

        //Create the node to sup array
        for(int sup = 0; sup < num_supernodes; sup++){
            for(int node = supernodes_ptr[sup]; node < supernodes_ptr[sup + 1]; node++){
                node_to_sup[node] = sup;
            }
        }
    }

    void SuperNodal::createCSCSupernode() {
        //For now it only support lower matrices
        auto n = A_CSC -> n;
        auto Lp = A_CSC -> p;
        auto Li = A_CSC -> i;
        //It is a CSC/CSR like format
        this->supernodes_ptr.push_back(0);
        for (int col = 0; col < n; ) {
            int width = 1;
            int first = col;
            int last = col + 1;
            while (last < n && width < limit) {
                int diff = last - first;
                // compare number of off diagonals
                int off_last  = Lp[last + 1] - Lp[last];
                int off_first = Lp[first + 1] - Lp[first] - diff;
                // If you don't understand this line
                // get back and read about supernode .. don't waste your time
                if (off_first != off_last)
                    break;
                assert(off_first > 0 && off_last > 0);
                // now examine column pattern
                bool can_form_supernode = true;
                for (int row = 0; row < off_first; row++) {
                    if (Li[Lp[first] + row + diff] != Li[Lp[last] + row]) {
                        can_form_supernode = false;
                        break;
                    }
                }
                if (can_form_supernode) {
                    //Increase the width of the supernode
                    width++;
                    //Lets check the next row
                    last++;
                } else {
                    //Lets go to the next row, this super node is done
                    break;
                }
            }
            //The starting point of the next supernode
            col = first + width;
            assert(col <= n);
            this->supernodes_ptr.push_back(col);
            assert(Lp[first + 1] - Lp[first] - width >= 0);
            this->off_diag_size.push_back(Lp[first + 1] - Lp[first] - width - 1);
        }
        //The end of the nodes should be equal to n
        assert(supernodes_ptr.back() == n);
        //Storing the number of nodes
        this->num_supernodes = supernodes_ptr.size() - 1;

        //Create the node to sup array
        for(int sup = 0; sup < num_supernodes; sup++){
            for(int node = supernodes_ptr[sup]; node < supernodes_ptr[sup + 1]; node++){
                node_to_sup[node] = sup;
            }
        }
    }


    void SuperNodal::getDependencyDAGCSCformat(std::vector<int>& Lp, std::vector<int>& Li, int &num_nodes, int &nnz) {
        Lp = this->DAG_ptr;
        Li = this->DAG_set;
        num_nodes = this->num_supernodes;
        nnz = this->num_task_dep_nz;
    }

    void SuperNodal::createDepDAG() {
        auto Lp = Dep_DAG->p;
        auto Li = Dep_DAG->i;
        DAG_set.resize(A_CSR->nnz);
        DAG_ptr.reserve(A_CSR->n);
        DAG_ptr.push_back(0);
        for(int supernode = 0; supernode <  num_supernodes; supernode++){
            int supernode_begin = supernodes_ptr[supernode];
            int supernode_end = supernodes_ptr[supernode + 1];
            std::unordered_set<int> child_buffer;
            assert(Lp[supernode_end] - Lp[supernode_begin] > 0);
            child_buffer.reserve(Lp[supernode_end] - Lp[supernode_begin]);
            //add children of the new aggregated node group_idx
            for(int child_idx = Lp[supernode_begin]; child_idx < Lp[supernode_end]; child_idx++){
                if(Li[child_idx] >= supernode_end) {
                    child_buffer.insert(node_to_sup[Li[child_idx]]);
                }
            }
            child_buffer.insert(supernode);
            //Add the new edges to the DAG_set and adjust DAG_ptr accordingly
            std::vector<int> children;
            int back = DAG_ptr.back();
            DAG_ptr.push_back(back + child_buffer.size());
            int start = DAG_ptr[supernode];
            int end = DAG_ptr[supernode + 1];
            std::copy(child_buffer.begin(), child_buffer.end(), &DAG_set[start]);
            std::sort(&DAG_set[start], &DAG_set[end]);
        }
        //Adjusting the size
        assert(DAG_ptr.size() == num_supernodes + 1);
        num_task_dep_nz = DAG_ptr.back();
        DAG_set.resize(num_task_dep_nz);
    }
}

