#ifndef READ_MTX_H
#define READ_MTX_H

#include <string>


void create_coo_matrix(const std::string & market_filename, ValueType ** V_out, int ** R_out, int ** C_out, int * m_ptr, int * n_ptr, int * nnz_ptr);


#endif /* READ_MTX_H */

