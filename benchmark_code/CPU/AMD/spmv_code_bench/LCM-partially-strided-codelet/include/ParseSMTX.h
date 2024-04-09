//
// Created by lwilkinson on 11/2/21.
//

#ifndef DDT_PARSESMTX_H
#define DDT_PARSESMTX_H

#include <assert.h>
#include "MatrixUtils.h"

namespace DDT {
namespace SMTX {

template<class type>
Matrix readSparseMatrix(const std::string &path) {
  std::ifstream file;
  file.open(path, std::ios_base::in);
  if (!file.is_open()) {
    std::cout << "File could not be found..." << std::endl;
    exit(1);
  }

  if (!std::is_same_v<type, CSR>) {
    throw std::runtime_error("Error: Matrix storage format not supported");
  }

  std::string line;
  int i;

  std::getline(file, line);
  std::replace(line.begin(), line.end(), ',', ' ');
  std::istringstream first_line(line);

  int rows, cols, nnz;
  first_line >> rows;
  first_line >> cols;
  first_line >> nnz;

  CSR csr(rows, cols, nnz);

  for (int i = 0; i < csr.r + 1; i++) {
    file >> csr.Lp[i];
  }

  // Go to next line
  char next;
  while (file.get(next)) { if (next == '\n') break; }

  // Read in col_indices
  for (int i = 0; i < csr.nz; i++) { file >> csr.Li[i]; }
  for (i = 0; i < csr.nz; i++) { csr.Lx[i] = 1.0f; }

  return std::move(csr);
}

} // namespace SMTX
} // namespace DDT

#endif //DDT_PARSESMTX_H
