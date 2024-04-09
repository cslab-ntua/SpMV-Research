//
// Created by lwilkinson on 11/2/21.
//

#ifndef DDT_SPARSEMATRIXIO_H
#define DDT_SPARSEMATRIXIO_H

#include "ParseMatrixMarket.h"
#include "ParseSMTX.h"

namespace DDT {

template<class type>
Matrix readSparseMatrix(const std::string &path) {
  std::string file_ext = path.substr(path.find_last_of(".") + 1);

  if (file_ext == "smtx") {
    return DDT::SMTX::readSparseMatrix<type>(path);
  } else if (file_ext == "mtx") {
    return DDT::MatrixMarket::readSparseMatrix<type>(path);
  } else {
    throw std::invalid_argument( "Matrix extension not supported" );
  }
}

} // namespace DDT

#endif //DDT_SPARSEMATRIXIO_H
