/*
 * =====================================================================================
 *
 *       Filename:  Input.cpp
 *
 *    Description:  Parses input for DDT 
 *
 *        Version:  1.0
 *        Created:  2021-07-08 12:06:45 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Zachary Cetinic, 
 *   Organization:  University of Toronto
 *
 * =====================================================================================
 */
#include "DDTDef.h"
#include "DDT.h"
#include "Input.h"

#include <stdlib.h>

#include <iostream>
#include <omp.h>

#include "cxxopts.hpp"

namespace DDT {
	/**
	 * @brief Parses commandline input for the program
	 *
	 * @param argc 
	 * @param argv
	 */
	Config parseInput(int argc, char** argv) {
		cxxopts::Options options("DDT", "Generates vectorized code from memory streams");

		options.add_options()
			("h,help", "Prints help text")
			("m,matrix", "Path to matrix market file.", cxxopts::value<std::string>())
			("n,numerical_operation", "Numerical operation being performed on matrix.", cxxopts::value<std::string>())
			("s,storage_format", "Storage format for matrix", cxxopts::value<std::string>())
			("t,threads", "Number of parallel threads", cxxopts::value<int>()->default_value("1"))
			("c,coarsening", "coarsening levels", cxxopts::value<int>()->default_value("5"))
			("p,packing", "bin-packing", cxxopts::value<int>()->default_value("1"))
			("u,tuning", "Tuning enabled", cxxopts::value<int>()->default_value("0"))
			("iteration_limit", "Max length of periodic iteration space to find", cxxopts::value<int>()->default_value("0"))
			("prefer_fsc", "Keep current codelet as FSC when greater than clt_width", cxxopts::value<bool>()->default_value("false"))
			("col_th", "Max length of periodic iteration space to find", cxxopts::value<int>()->default_value("8"))
			("clt_width", "Max length of periodic iteration space to find", cxxopts::value<int>()->default_value("4"))
			("m_tile_size", "Row tile size for SpMM", cxxopts::value<int>()->default_value("64"))
			("n_tile_size", "Column tile size for SpMM", cxxopts::value<int>()->default_value("64"))
			("b_matrix_columns", "Number of columns in dense matrix for SpMM", cxxopts::value<int>()->default_value("256"))
			("hint", "Max length of periodic iteration space to find", cxxopts::value<int>()->default_value("0"))
			("prefetch_distance", "Max length of periodic iteration space to find", cxxopts::value<int>()->default_value("0"))
			("analyze", "Return analysis information instead of computing numerical method", cxxopts::value<bool>()->default_value("false"))
			("analyze_codelets", "Return analysis information for individual codelets instead of computing numerical method", cxxopts::value<bool>()->default_value("false"))
			("d,header", "prints header or not.");

		options.parse_positional({"matrix"});

		auto result = options.parse(argc, argv);

		if (result.count("help")) {
			std::cout << options.help() << std::endl;
			exit(0);
		}

		if (!result.count("matrix")) {
			std::cout << "'matrix' is manditory argument. Use --help" << std::endl;
			exit(0);
		}

		auto matrixPath = result["matrix"].as<std::string>();
		auto coarsening = result["coarsening"].as<int>();
		auto bpacking = result["packing"].as<int>();
		auto tuning_en = result["tuning"].as<int>();
		auto lim = result["iteration_limit"].as<int>();
		auto prefer_fsc = result["prefer_fsc"].as<bool>();
		auto col_th = result["col_th"].as<int>();
		auto clt_width = result["clt_width"].as<int>();
		auto mTileSize = result["m_tile_size"].as<int>();
		auto nTileSize = result["n_tile_size"].as<int>();
		auto bMatrixCols = result["b_matrix_columns"].as<int>();
		auto hint = result["hint"].as<int>();
		auto prefetch_distance = result["prefetch_distance"].as<int>();
		auto analyze = result["analyze"].as<bool>();
		auto analyzeCodelets = result["analyze_codelets"].as<bool>();

		assert(lim <= MAX_LIM);

		DDT::clt_width = clt_width;
		DDT::col_th = col_th;
		DDT::prefer_fsc = prefer_fsc;

		int header = 0;
		if (result.count("header")) {
			header = 1;
		}

		return Config{ matrixPath, OP_SPMV, header, omp_get_max_threads(), DDT::CSR_SF, coarsening, bpacking,
			tuning_en, lim,
			static_cast<_mm_hint>(hint), prefetch_distance, analyze, analyzeCodelets, mTileSize, nTileSize, bMatrixCols };
	}
}
