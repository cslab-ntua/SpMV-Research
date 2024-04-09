/*
 * =====================================================================================
 *
 *       Filename:  DDT.h
 *
 *    Description:  Header file for DDT.cpp 
 *
 *        Version:  1.0
 *        Created:  2021-07-08 02:16:50 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Zachary Cetinic, 
 *   Organization:  University of Toronto
 *
 * =====================================================================================
 */

#ifndef DDT_DDT
#define DDT_DDT

#include "ParseMatrixMarket.h"
#include "SpTRSVModel.h"

#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace DDT {
	enum NumericalOperation {
		OP_SPMV,
		OP_SPTRS,
		OP_SPMM
	};

	enum StorageFormat {
		CSR_SF,
		CSC_SF
	};

	struct MemoryTrace {
		int** ip;
		int ips;
	};

	struct Config {
		std::string matrixPath;
		NumericalOperation op;
		int header;
		int nThread;
		StorageFormat sf;
		int coarsening;
		int bin_packing;
		int tuning_mode;
		int lim;
		_mm_hint hint;
		int prefetch_distance;
		bool analyze;
		bool analyzeCodelets;
		int mTileSize;
		int nTileSize;
		int bMatrixCols;
	};

	struct GlobalObject {
		MemoryTrace mt; // Object containing raw pointer memory trace
		PatternDAG* c;  // Directed-acyclic graph of formed codelets
		int* d;    // Array containing forward first order difference for trace
		int* o;    // Array containing memory required for storing offset in runtime codelets
		int onz;   // Number of offsets stored in @param o
		int* tb;   // Bounds on mt.ip for each parallel thread
		sparse_avx::SpTRSVModel* sm;
		sparse_avx::Trace*** t;
		bool* sp;  // Array to mark sparse iterations
		int* rd;   // Array to mark dimensions with reuse between iterations
		int* ipbt; // Array of types associated with pruned bound
		int* ipb;  // Array containing pruned bounds on mt.ip
		int ipbs;  // Current Size of ipb
		int* nci;  // Number of codelets at iteration
	};


	void printTuple(int* t, std::string&& s);

	template <typename M0, typename M1>
		DDT::GlobalObject allocateExternalSpTRSVMemoryTrace(const M0* m0, const
				M1* m1, const DDT::Config& cfg) {
			int lp = cfg.nThread, cp = cfg.coarsening, ic = cfg.coarsening, bp = cfg
				.bin_packing;
			auto *sm = new sparse_avx::SpTRSVModel(m0->m, m0->n, m0->nnz, m0->p,
					m0->i, m1->p, m1->i, lp, cp, ic,
					bp);
			sm->_tuning_mode = cfg.tuning_mode;
			auto trs = sm->generate_3d_trace(cfg.nThread);

			return GlobalObject{  {},  nullptr, nullptr, nullptr, 0, nullptr, sm, trs };
		}

	DDT::GlobalObject allocateSpTRSVMemoryTrace(const Matrix& m, int nThreads);

	DDT::GlobalObject allocateSpMVMemoryTrace(const Matrix& m, int nThreads);

	template <typename M0>
		DDT::GlobalObject init(const M0* m0, const DDT::Config& cfg) {
			// Convert matrix into regular form
			Matrix m{};
			// fprintf(stderr, "test DDT::init 1\n");
			DDT::MatrixMarket::copySymLibMatrix(m, m0);
			// fprintf(stderr, "test DDT::init 2\n");

			// Allocate memory and generate trace
			DDT::GlobalObject d;
			if (cfg.op == OP_SPMV || cfg.op == OP_SPMM) {
				// fprintf(stderr, "test DDT::init 2a\n");
				d = DDT::allocateSpMVMemoryTrace(m, cfg.nThread);
			} else if (cfg.op == OP_SPTRS) {
				// fprintf(stderr, "test DDT::init 2b\n");
				d = DDT::allocateSpTRSVMemoryTrace(m, cfg.nThread);
			} else {
				throw std::runtime_error("Error: Operation not currently supported");
			}
			// fprintf(stderr, "test DDT::init 3\n");

			return d;
		}

	template <typename M0, typename M1>
		DDT::GlobalObject init(const M0* m0, const M1* m1, const DDT::Config& cfg) {
			// Allocate memory and generate trace
			DDT::GlobalObject d;
			if (cfg.op == OP_SPTRS) {
				d = DDT::allocateExternalSpTRSVMemoryTrace(m0, m1, cfg);
			} else {
				throw std::runtime_error("Error: Operation not currently supported");
			}

			return d;
		}

	GlobalObject init(const DDT::Config& config);

	void free(DDT::GlobalObject d);

	/// Used for testing executor
	struct Args {
		double *x, *y;
		double *Ax,*Bx,*Cx;
		int bRows, bCols;
		int r; int* Lp; int* Li; double* Lx;
	};

}

#endif
