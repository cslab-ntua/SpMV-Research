#ifndef SUPERNODALTOOLS_H
#define SUPERNODALTOOLS_H

#include "def.h"

namespace supernodaltools {
    enum MatrixType {CSC_TYPE, CSR_TYPE};

    class BCSCMatrix {
        sym_lib::BCSC *M;
    public:
        /// finds all the different supernodes / blocks in the matrix
        int supernodes(sym_lib::CSC *A, int limit, bool isLower);

        /// calculate the total number of nonzero in the matrix including zero padding
        int calcSize(sym_lib::CSC *A);

        /// Fill in the row and numerical values for blocked matrix
        void createFormat(sym_lib::CSC *A);

        // Extract a supernodal CSC from a BCSC
        sym_lib::CSC *compressed_BCSC_to_CSC();

        /// Generates the BCSC arrays from A
        /// \param nA
        /// \param A
        void generateBCSC(sym_lib::CSC *A) {
            M->nnz = calcSize(A);
            M->i = new int[M->nnz]();
            M->x = new double[M->nnz]();
            M->nrows = new int[M->nodes]();
            createFormat(A);
        }

    public:
        BCSCMatrix(sym_lib::CSC *A) {
            M = new sym_lib::BCSC(A);
            M->nodes = supernodes(A, A->n, true);
            generateBCSC(A);
        }

        BCSCMatrix(sym_lib::CSC *A, int nodes, int *supernodes) {
            M = new sym_lib::BCSC(A);
            M->nodes = nodes;
            M->supernodes = new int[nodes+1]();
            std::memcpy(M->supernodes, supernodes, sizeof(int) * (nodes+1));
            generateBCSC(A);
        }

        sym_lib::BCSC *getBCSC() { return M; }

        ~BCSCMatrix() {
            delete(M);
        }
    };

    class SuperNodal{
    private:
        MatrixType input_type;
        //The size of each supernode can be drove by supernodes_ptr[i + 1] - supernodes_ptr[i]
        std::vector<int> supernodes_ptr;
        //Stores the number of columns or rows of each off-diagonal block of each supernode
        std::vector<int> off_diag_size;
        //Stores the supernode index for each row/column
        std::vector<int> node_to_sup;
        //Stores the number of supernodes and the number of dependency edges between supernodes
        int num_supernodes, num_task_dep_nz;
        //Get the average supernode size
        double average_group_size;
        //Limit the maximum size of supernodes
        int limit;
        //Pointer to the input matrix
        const sym_lib::CSR* A_CSR;
        const sym_lib::CSC* A_CSC;
        const sym_lib::CSC* Dep_DAG;
        //Dependency DAG index and pointer
        std::vector<int> DAG_ptr;
        std::vector<int> DAG_set;
        ///\Description This function will iterate the input matrix with CSR format and create the supernodes
        ///It fills the supernode_ptr vector and off_diag_size vector
        void createCSRSupernode();
        ///\Description This function will iterate the input matrix with CSC format and create the supernodes
        ///It fills the supernode_ptr vector and off_diag_size vector
        void createCSCSupernode();
        ///\Description This function will create the dependency DAG between supernodes
        /// Based on the CSR format input matrix
        void createDepDAG();
    public:
        ///\Description This class will create a dependency
        /// DAG and a list of nodes that can create a 1D supernodal blocks
        ///\param type: The type of input matrix. It can be CSC_TYPE or CSR_TYPE
        ///\param A_CSC: The input matrix in CSC format (if type == CSC_TYPE)
        ///\param A_CSR: The input matrix in CSC format (if type == CSR_TYPE)
        ///\param Dep_DAG: The dependency DAG based on CSC format (normally it is the CSC format of the matrix)
        ///\param limit: Shows the maximum size of a supernode. If it is less than 1, then
        /// It will automatically set to maximum size (n)
        SuperNodal(MatrixType type,const sym_lib::CSC* A_CSC,const sym_lib::CSR* A_CSR, const sym_lib::CSC* Dep_DAG, int limit = 0);

        ///\Description: This function will return the Dependency DAG of the supernodes in CSC format
        ///\return Lp: the pointer of the dependency DAG between supernodes (CSC format)
        ///\return Li: the edges or rows in the adjacency matrix of dependency DAG (CSC format)
        ///\return num_nodes: number of nodes in the dependency DAG
        ///\return nnz: number of edges in the dependency DAG
        void getDependencyDAGCSCformat(std::vector<int>& Lp, std::vector<int>& Li, int &num_nodes, int &nnz);
        //Helper Functions
        int getNumSuperNodes(){return num_supernodes;}
        //Average Supernode
        double getAvgSuperNode(){
            double super_node_size = 0;
            for(int i = 0; i < num_supernodes; i++){
                super_node_size += supernodes_ptr[i + 1] - supernodes_ptr[i];
            }
            return super_node_size / num_supernodes;
        }



        int* getSuperNodeGroups(){
            return supernodes_ptr.data();
        }

        void getSuperNodeGroups(std::vector<int>& sup){
            sup = supernodes_ptr;
        }

    ~SuperNodal(){
            A_CSR = nullptr;
            A_CSC = nullptr;
            Dep_DAG = nullptr;
        }
    };
}
#endif
