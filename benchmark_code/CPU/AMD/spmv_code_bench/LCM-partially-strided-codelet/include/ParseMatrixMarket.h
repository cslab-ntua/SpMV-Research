//
// Created by cetinicz on 2021-07-07.
//

#ifndef DDT_PARSEMATRIXMARKET_H
#define DDT_PARSEMATRIXMARKET_H


#include <algorithm>
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

#include <metis_interface.h>
#include <sparse_utilities.h>

#include "MatrixUtils.h"

namespace DDT {
namespace MatrixMarket {

template<typename T>
void copySymLibMatrix(Matrix &m, T symLibMat) {
  // Convert matrix back into regular format
  m.nz = symLibMat->nnz;
  m.r = symLibMat->m;
  m.c = symLibMat->n;

  delete m.Lp;
  delete m.Lx;
  delete m.Li;

  m.Lp = new int[m.r + 1]();
  m.Lx = new double[m.nz]();
  m.Li = new int[m.nz]();

  std::copy(symLibMat->p, symLibMat->p + symLibMat->m + 1, m.Lp);
  std::copy(symLibMat->i, symLibMat->i + symLibMat->nnz, m.Li);
  std::copy(symLibMat->x, symLibMat->x + symLibMat->nnz, m.Lx);
}

template<typename type>
auto reorderSparseMatrix(const CSC &m) {
  // Organize data into sympiler based format
  auto symMat = new sym_lib::CSC(m.r, m.c, m.nz, m.Lp, m.Li, m.Lx);
  symMat->stype = 1;
  int *perm;

  // Permute matrix into new configuration
  sym_lib::CSC *A_full = sym_lib::make_full(symMat);
  sym_lib::metis_perm_general(A_full, perm);
  sym_lib::CSC *Lt = transpose_symmetric(A_full, perm);
  sym_lib::CSC *L1_ord = transpose_symmetric(Lt, NULLPNTR);

  Matrix nm;
  if (std::is_same_v<CSR, type>) {
    auto csr = sym_lib::csc_to_csr(L1_ord);
    nm = CSR(csr->m, csr->n, csr->nnz);
    copySymLibMatrix(nm, csr);
  } else if (std::is_same_v<CSC, type>) {
    nm = CSR(L1_ord->m, L1_ord->n, L1_ord->nnz);
    copySymLibMatrix(nm, L1_ord);
  } else {
    throw std::runtime_error("Error: Unsupported matrix type in template instruction");
  }

  // Clean up memory
  delete Lt;
  delete[]perm;
  delete symMat;
  delete L1_ord;

  return nm;
}

template<class type>
Matrix readSparseMatrix(const std::string &path) {
  std::ifstream file;
  file.open(path, std::ios_base::in);
  if (!file.is_open()) {
    std::cout << "File could not be found..." << std::endl;
    exit(1);
  }
  RawMatrix mat;

  int rows, cols, nnz;
  std::string line;
  bool parsed = false;
  bool sym = false;
  if (file.is_open()) {
    std::stringstream ss;
    std::getline(file, line);
    ss << line;
    // Junk
    std::getline(ss, line, ' ');
    // Matrix
    std::getline(ss, line, ' ');
    // Type
    std::getline(ss, line, ' ');
    if (line != "coordinate") {
      std::cout << "Can only process real matrices..." << std::endl;
      exit(1);
    }
    std::getline(ss, line, ' ');

    // Symmetric
    std::getline(ss, line, ' ');
    if (line == "symmetric") {
      sym = true;
    }

    ss.clear();

    while (std::getline(file, line)) {
      if (line[0] == '%') { continue; }
      if (!parsed) {
        ss << line;
        ss >> rows >> cols >> nnz;
        parsed = true;
        mat.reserve(sym ? nnz * 2 - rows : nnz);
        ss.clear();
        break;
      }
    }
    for (int i = 0; i < nnz; i++) {
      std::getline(file, line);
      std::tuple<int, int, double> t;
      ss << line;
      int row, col;
      double value;
      ss >> row >> col >> value;
      mat.emplace_back(std::make_tuple(row - 1, col - 1, value));
      if (sym && col != row) {
        mat.emplace_back(std::make_tuple(col - 1, row - 1, value));
      }
      ss.clear();
    }
  }
  file.close();

#ifdef METISA
  auto ccc = CSC( rows, cols, sym ? nnz*2-rows : nnz, mat);
  if (std::is_same_v<type, CSR>) {
      return reorderSparseMatrix<CSR>(ccc);
  } else if (std::is_same_v<type, CSC>) {
      return reorderSparseMatrix<CSC>(ccc);
  } else {
      throw std::runtime_error("Error: Matrix storage format not supported");
  }
#endif

  if (std::is_same_v<type, CSR>) {
    return CSR(rows, cols, sym ? nnz * 2 - rows : nnz, mat);
  } else if (std::is_same_v<type, CSC>) {
    return CSC(rows, cols, sym ? nnz * 2 - rows : nnz, mat);
  } else {
    throw std::runtime_error("Error: Matrix storage format not supported");
  }
}

} // namespace MatrixMarket
} // namespace DDT

#endif  //DDT_PARSEMATRIXMARKET_H
