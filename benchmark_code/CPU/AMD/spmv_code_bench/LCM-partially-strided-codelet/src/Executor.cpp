/*
 * =====================================================================================
 *
 *       Filename:  Executor.cpp
 *
 *    Description:  Executes patterns found in codes from a differentiated matrix 
 *
 *        Version:  1.0
 *        Created:  2021-07-13 09:25:02 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Zachary Cetinic, 
 *   Organization:  University of Toronto
 *
 * =====================================================================================
 */

#include "DDT.h"
#include "Executor.h"
#include "Inspector.h"
#include "SpMVGenericCode.h"
#include "SparseMatrixIO.h"

#include <chrono>
#include <vector>

namespace DDT {

	void executeSPMVCodelets(const std::vector<DDT::Codelet*>* cl, const DDT::Config& c) {
		// Read matrix
		auto m = readSparseMatrix<CSR>(c.matrixPath);

		// Setup memory
		auto x = new double[m.c]();
		for (int i = 0; i < m.c; i++) {
			x[i] = i;
		}
		auto y = new double[m.r]();

		// Execute SpMV
		DDT::spmv_generic(m.r, m.Lp, m.Li, m.Lx, x, y, cl, c);

		// Clean up memory
		delete[] x;
		delete[] y;
	}

	void executeSPMVCodelets(const std::vector<DDT::Codelet*>* cl, const
			DDT::Config& cfg, const int r, const int* Lp, const int *Li, const double*Lx,
			const double* x, double* y) {
		// Execute SpMV
		spmv_generic(r, Lp, Li, Lx, x, y, cl, cfg);
	}

	/**
	 * @brief Executes codelets found in a matrix performing a computation
	 *
	 * @param cl List of codelets to perform computation on
	 * @param c  Configuration object for setting up executor
	 */
	void executeCodelets(const std::vector<DDT::Codelet*>* cl, const DDT::Config& cfg) {
		switch (cfg.op) {
			case DDT::OP_SPMV:
				executeSPMVCodelets(cl, cfg);
				break;
			default:
				break;
		}
	}
	void executeCodelets(const std::vector<DDT::Codelet*>* cl, const DDT::Config& cfg, Args& args) {
		switch (cfg.op) {
			case DDT::OP_SPMV:
				executeSPMVCodelets(cl, cfg,args.r, args.Lp, args.Li, args.Lx, args.x, args.y);
				break;
			default:
				break;
		}
	}

	void executeParallelCodelets(const DDT::GlobalObject& d, const DDT::Config&
			cfg, Args& args) {
		switch (cfg.op) {
			case DDT::OP_SPMV:
				//                executeSPMVCodelets(cl, cfg,args.r, args.Lp, args.Li, args.Lx, args.x, args
				//                        .y);
				break;
			default:
				break;
		}
	}



}
