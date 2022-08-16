#ifndef READ_MTX_H
#define READ_MTX_H

#include <string>

#include "spmv_bench_common.h"


void create_coo_matrix(const std::string & market_filename, ValueType ** V_out, INT_T ** R_out, INT_T ** C_out, INT_T * m_ptr, INT_T * n_ptr, INT_T * nnz_ptr);


#endif /* READ_MTX_H */

