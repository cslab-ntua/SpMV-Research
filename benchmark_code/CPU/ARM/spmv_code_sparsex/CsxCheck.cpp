/*
 * Copyright (C) 2011-2014, Computing Systems Laboratory (CSLab), NTUA.
 * Copyright (C) 2011-2012, Vasileios Karakasis
 * Copyright (C) 2011-2012, Theodoros Gkountouvas
 * Copyright (C) 2013-2014, Athena Elafrou
 * All rights reserved.
 *
 * This file is distributed under the BSD License. See LICENSE.txt for details.
 */

/*
 * \file CsxCheck.cpp
 * \brief Checking utilities
 *
 * \author Vasileios Karakasis
 * \author Theodoros Gkountouvas
 * \author Athena Elafrou
 * \date 2011&ndash;2014
 * \copyright This file is distributed under the BSD License. See LICENSE.txt
 * for details.
 */

#include <sparsex/internals/Config.hpp>
#include "CsxCheck.hpp"

using namespace sparsex::io;

static void csr_spmv(CSR<spx_uindex_t, spx_value_t> *spm, vector_t *in,
                     vector_t *out)
{
  spx_value_t *x = in->elements;
  spx_value_t *y = out->elements;
  const spx_value_t *values = spm->values_;
  const spx_uindex_t *row_ptr = spm->rowptr_;
  const spx_uindex_t *col_ind = spm->colind_;
  const spx_uindex_t row_start = 0;
  const spx_uindex_t row_end = spm->GetNrRows();
  register spx_value_t yr;
  spx_uindex_t i,j;

  for (i = row_start; i < row_end; i++) {
    yr = (spx_value_t) 0;
    for (j = row_ptr[i]; j < row_ptr[i+1]; j++)
      yr += (values[j] * x[col_ind[j]]);

    y[i] = yr;
  }
}

void check_result(vector_t *result, double alpha, vector_t *x,
                  char *matrix_file)
{
  CheckResult<spx_uindex_t, spx_value_t>(result, alpha, x, matrix_file);
}

void check_result_artificial(vector_t *result, double alpha, vector_t *x, 
                  spx_uindex_t *row_ptr, spx_uindex_t *col_ind, spx_value_t *values, spx_uindex_t nr_rows, spx_uindex_t nr_cols)

{
  CheckResultArtificial<spx_uindex_t, spx_value_t>(result, alpha, x, row_ptr, col_ind, values, nr_rows, nr_cols);
}
