#ifndef SPMV_KERNELS_H
#define SPMV_KERNELS_H


void compute_sparse_mv(sparse_matrix_t A, ValueType * x , ValueType * y, matrix_descr descr)
{
        #if DOUBLE == 0
		mkl_sparse_s_mv(SPARSE_OPERATION_NON_TRANSPOSE, 1.0f, A, descr, x, 0.0f, y);
        #elif DOUBLE == 1
		mkl_sparse_d_mv(SPARSE_OPERATION_NON_TRANSPOSE, 1.0f, A, descr, x, 0.0f, y);
        #endif
}


void compute_csr(CSRArrays * csr, ValueType * x , ValueType * y)
{
	char transa = 'N';
	#if DOUBLE == 0
	mkl_cspblas_scsrgemv(&transa, &csr->m , csr->a , csr->ia , csr->ja , x , y);
	#elif DOUBLE == 1
	mkl_cspblas_dcsrgemv(&transa, &csr->m , csr->a , csr->ia , csr->ja , x , y);
	#endif
}

void compute_csr_custom(CSRArrays * csr, ValueType * x , ValueType * y)
{
	#if !defined(CUSTOM_VECTOR)

		// #if defined(NAIVE) || defined(PROC_BENCH)

		// #pragma omp parallel
		// {
			// ValueType sum;
			// long i, j;
			// PRAGMA(omp for schedule(static))
			// for (i=0;i<csr->m;i++)
			// {
				// sum = 0;
				// for (j=csr->ia[i];j<csr->ia[i+1];j++)
					// sum += csr->a[j] * x[csr->ja[j]];
				// y[i] = sum;
			// }
		// }

		// #else

		#pragma omp parallel
		{
			int tnum = omp_get_thread_num();
			ValueType sum;
			long i, i_s, i_e, j, j_s, j_e;
			i_s = thread_i_s[tnum];
			i_e = thread_i_e[tnum];

			#ifdef TIME_BARRIER
			double time;
			time = time_it(1,
			#endif

				for (i=i_s;i<i_e;i++)
				{
					j_s = csr->ia[i];
					j_e = csr->ia[i+1];
					if (j_s == j_e)
						continue;
					sum = 0;
					for (j=j_s;j<j_e;j++)
						sum += csr->a[j] * x[csr->ja[j]];
					y[i] = sum;
				}

			#ifdef TIME_BARRIER
			);
			thread_time_compute[tnum] += time;
			time = time_it(1,
				_Pragma("omp barrier")
			);
			thread_time_barrier[tnum] += time;
			#endif
		}

		// #endif

	#else

		#pragma omp parallel
		{
			int tnum = omp_get_thread_num();
			long i, i_s, i_e, j, j_s, j_e, k, j_vector;
			const long mask = ~(((long) VECTOR_ELEM_NUM) - 1);      // VECTOR_ELEM_NUM is a power of 2.
			Vector_Value_t zero = {0};
			__attribute__((unused)) Vector_Value_t v_a, v_x = zero, v_mul = zero, v_sum = zero;
			__attribute__((unused)) ValueType sum = 0;
			i_s = thread_i_s[tnum];
			i_e = thread_i_e[tnum];

			#ifdef TIME_BARRIER
			double time;
			time = time_it(1,
			#endif
				for (i=i_s;i<i_e;i++)
				{
					v_sum = zero;
					y[i] = 0;
					j_s = csr->ia[i];
					j_e = csr->ia[i+1];
					if (j_s == j_e)
						continue;
					j = j_s;
					j_vector = j_s + ((j_e - j_s) & mask);
					// for (j=j_s;j<j_e-VECTOR_ELEM_NUM;j+=VECTOR_ELEM_NUM)
					for (j=j_s;j<j_vector;j+=VECTOR_ELEM_NUM)
					{
						v_a = *(Vector_Value_t *) &csr->a[j];
						PRAGMA(GCC unroll VECTOR_ELEM_NUM)
						PRAGMA(GCC ivdep)
						for (k=0;k<VECTOR_ELEM_NUM;k++)
						{
							// v_x[k] = x[csr->ja[j+k]];
							v_mul[k] = v_a[k] * x[csr->ja[j+k]];
							// v_mul[k] = csr->a[j+k] * x[csr->ja[j+k]];
						}

						v_sum += v_mul;
						// v_sum += v_a * v_x;

						// for (k=1;k<VECTOR_ELEM_NUM;k++)
						// {
							// v_sum[0] += v_sum[k];
							// v_sum[k] = 0;
						// }

					}

					for (;j<j_e;j++)
						v_sum[0] += csr->a[j] * x[csr->ja[j]];

					for (j=1;j<VECTOR_ELEM_NUM;j++)
						v_sum[0] += v_sum[j];

					// for (k=0;k<j_e-j;k++)
						// v_sum[k] += csr->a[j] * x[csr->ja[j+k]];
					// __attribute__((vector_size (32))) long mask = {1, 0, 3, 2};
					// v_sum += __builtin_shuffle(v_sum, mask);
					// v_sum[0] += v_sum[2];

					y[i] = v_sum[0];
				}

			#ifdef TIME_BARRIER
			);
			thread_time_compute[tnum] += time;
			#endif

			#ifdef TIME_BARRIER
			time = time_it(1,
				_Pragma("omp barrier")
			);
			thread_time_barrier[tnum] += time;
			#endif
		}

	#endif
}


/** https://software.intel.com/fr-fr/node/520816#366F2854-A2C0-4661-8CE7-F478F8E6B613 */
void compute_bcsr(BCSRArrays * bcsr, ValueType * x , ValueType * y )
{
    char transa = 'N';
    #if DOUBLE == 0
    mkl_cspblas_sbsrgemv(&transa, &bcsr->nbBlockRows , &bcsr->lb , bcsr->a , bcsr->ia , bcsr->ja , x , y);
    #elif DOUBLE == 1
    mkl_cspblas_dbsrgemv(&transa, &bcsr->nbBlockRows , &bcsr->lb , bcsr->a , bcsr->ia , bcsr->ja , x , y);
    #endif
}


/** https://software.intel.com/fr-fr/node/520806#FCB5B469-8AA1-4CFB-88BE-E2F22E9E2AF0 */
// ValueType *val;    //< the NNZ values - may include zeros (of size lval*ndiag)*/
// MKL_INT *idiag;    //< distance from the diagonal (of size ndiag)
// MKL_INT lval;      //< leading where the diagonals are stored >= m,  which is the declared leading dimension in the calling (sub)programs
// MKL_INT ndiag;     //< number of diagonals that have at least one nnz
void compute_dia(DIAArrays * dia , ValueType * x , ValueType * y)
{
	char transa = 'N';
	#if DOUBLE == 0
	mkl_sdiagemv(&transa, &dia->m , dia->val , &dia->lval , dia->idiag , &dia->ndiag , x , y);
	#elif DOUBLE == 1
	mkl_ddiagemv(&transa, &dia->m , dia->val , &dia->lval , dia->idiag , &dia->ndiag , x , y);
	#endif
}


void compute_dia_custom(DIAArrays * dia , ValueType * x , ValueType * y)
{
	// MKL_INT * offsets = (MKL_INT *) mkl_malloc(dia->ndiag * sizeof(*offsets), 64);
	// long i;
	// printf("ndiag = %d , lval = %d\n", dia->ndiag, dia->lval);
	// for (i=0;i<dia->ndiag;i++)
	// {
		// offsets[i] = (dia->idiag[i] + dia->lval) % dia->lval;
		// printf("i=%ld , idiag=%d , offset=%d\n", i, dia->idiag[i], offsets[i]);
	// }
	// for (i=0;i<dia->lval;i++)
	// {
		// for (long j=0;j<dia->ndiag;j++)
			// printf("%lf ", dia->val[i*dia->ndiag + j]);
		// printf("\n");
	// }
	#pragma omp parallel
	{
		long i, j;
		MKL_INT col;

		#pragma omp for schedule(static)
		// #pragma omp for schedule(dynamic, 1024)
		// #pragma omp for schedule(hierarchical, 64)
		for (i=0;i<dia->lval;i++)
		{
			y[i] = 0;
			for (j=0;j<dia->ndiag;j++)
			{
				// col = (offsets[j] + i) % dia->lval;
				col = dia->idiag[j] + i;
				if (col < 0 || col >= dia->lval)
				{
					// printf("%d\n", col);
					continue;
				}
				y[i] += dia->val[i*dia->ndiag + j] * x[col];
			}
		}
	}
	// mkl_free(offsets);
}


void compute_csc(CSCArrays * csc, ValueType * x , ValueType * y)
{
    char transa = 'N';
    ValueType alpha = 1, beta = 0;
    char matdescra[7] = "G--C--";

    #if DOUBLE == 0
    mkl_scscmv(&transa, &csc->m, &csc->n, &alpha, matdescra, csc->a, csc->ja, csc->ia, csc->ia+1, x, &beta, y);
    #elif DOUBLE == 1
    mkl_dcscmv(&transa, &csc->m, &csc->n, &alpha, matdescra, csc->a, csc->ja, csc->ia, csc->ia+1, x, &beta, y);
    #endif
}


/** see https://software.intel.com/fr-fr/node/520817#38F0A87C-7884-4A96-B83E-CEE88290580F */
void compute_coo(COOArrays * coo, ValueType * x , ValueType * y)
{
    char transa = 'N';
    #if DOUBLE == 0
    mkl_cspblas_scoogemv(&transa, &coo->m, coo->val, coo->rowind, coo->colind, &coo->nnz, x, y);
    #elif DOUBLE == 1
    mkl_cspblas_dcoogemv(&transa, &coo->m, coo->val, coo->rowind, coo->colind, &coo->nnz, x, y);
    #endif
}


void compute_ldu(LDUArrays * ldu, ValueType * x , ValueType * y)
{
	long i;
	long row, col;
	for (i=0;i<ldu->m;i++)
		y[i] = ldu->diag[i] * x[i];
	for (i=0;i<ldu->upper_n;i++)
	{
		row = ldu->row_idx[i];
		col = ldu->col_idx[i];
		y[row] += ldu->upper[i] * x[col];
		y[col] += ldu->lower[i] * x[row];
	}
}



void compute_ell_par(ELLArrays * ell, ValueType * x , ValueType * y)
{
	#pragma omp parallel
	{
		ValueType sum;
		long i, j, j_s, j_e;
		PRAGMA(omp for schedule(static))
		for (i=0;i<ell->m;i++)
		{
			j_s = i * ell->width;
			j_e = (i + 1) * ell->width;
			sum = 0;
			for (j=j_s;j<j_e;j++)
				sum += ell->a[j] * x[ell->ja[j]];
			y[i] = sum;
		}
	}
}


void compute_ell(ELLArrays * ell, ValueType * x , ValueType * y)
{
	ValueType sum;
	long i, j, j_s, j_e;
	for (i=0;i<ell->m;i++)
	{
		j_s = i * ell->width;
		j_e = (i + 1) * ell->width;
		sum = 0;
		for (j=j_s;j<j_e;j++)
			sum += ell->a[j] * x[ell->ja[j]];
		y[i] = sum;
	}
}


void compute_ell_v(ELLArrays * ell, ValueType * x , ValueType * y)
{
	long i, i_vector, j, j_s, j_e, k;
	const long mask = ~(((long) VECTOR_ELEM_NUM) - 1);      // VECTOR_ELEM_NUM is a power of 2.
	Vector_Value_t zero = {0};
	__attribute__((unused)) Vector_Value_t v_a = zero, v_x = zero, v_mul = zero, v_sum = zero;
	__attribute__((unused)) ValueType sum = 0;
	i_vector = ell->m & mask;
	for (i=0;i<i_vector;i+=VECTOR_ELEM_NUM)
	{
		v_sum = zero;
		j_s = i * ell->width;
		j_e = (i + 1) * ell->width;
		for (j=j_s;j<j_e;j++)
		{

			PRAGMA(GCC unroll VECTOR_ELEM_NUM)
			PRAGMA(GCC ivdep)
			for (k=0;k<VECTOR_ELEM_NUM;k++)
			{
				v_mul[k] = ell->a[j+k*ell->width] * x[ell->ja[j+k*ell->width]];
			}
			v_sum += v_mul;

			// PRAGMA(GCC unroll VECTOR_ELEM_NUM)
			// PRAGMA(GCC ivdep)
			// for (k=0;k<VECTOR_ELEM_NUM;k++)
			// {
				// v_a[k] = ell->a[j+k*ell->width] ;
				// v_x[k] = x[ell->ja[j+k*ell->width]];
			// }
			// v_sum += v_a * v_x;

		}
		*((Vector_Value_t *)&y[i]) = v_sum;
	}
	for (i=i_vector;i<ell->m;i++)
	{
	printf("i_vector = %ld\n", i_vector);
		j_s = i * ell->width;
		j_e = (i + 1) * ell->width;
		sum = 0;
		for (j=j_s;j<j_e;j++)
			sum += ell->a[j] * x[ell->ja[j]];
		y[i] = sum;
	}
}


void compute_ell_v_hor(ELLArrays * ell, ValueType * x , ValueType * y)
{
	long i, j, j_s, j_e, k, j_vector_width, j_vector;
	const long mask = ~(((long) VECTOR_ELEM_NUM) - 1);      // VECTOR_ELEM_NUM is a power of 2.
	Vector_Value_t zero = {0};
	__attribute__((unused)) Vector_Value_t v_a, v_x = zero, v_mul = zero, v_sum = zero;
	__attribute__((unused)) ValueType sum = 0;
	j_vector_width = ell->width & mask;
	for (i=0;i<ell->m;i++)
	{
		v_sum = zero;
		j_s = i * ell->width;
		j_e = (i + 1) * ell->width;
		j_vector = j_s + j_vector_width;
		for (j=j_s;j<j_vector;j+=VECTOR_ELEM_NUM)
		{
			v_a = *(Vector_Value_t *) &ell->a[j];
			PRAGMA(GCC unroll VECTOR_ELEM_NUM)
			PRAGMA(GCC ivdep)
			for (k=0;k<VECTOR_ELEM_NUM;k++)
			{
				v_mul[k] = v_a[k] * x[ell->ja[j+k]];
			}
			v_sum += v_mul;
		}
		for (;j<j_e;j++)
			v_sum[0] += ell->a[j] * x[ell->ja[j]];
		for (j=1;j<VECTOR_ELEM_NUM;j++)
			v_sum[0] += v_sum[j];
		y[i] = v_sum[0];
	}
}


// void compute_ell(ELLArrays * ell, ValueType * x , ValueType * y)
// {
	// long i, j, row;
	// for (i=0;i<ell->n;i++)
		// y[i] = 0;
	// for (i=0;i<ell->width;i++)
	// {
		// PRAGMA(GCC ivdep)
		// for (row=0,j=i*ell->m;j<(i + 1)*ell->m;row++,j++)
			// y[row] += ell->a[j] * x[ell->ja[j]];
	// }
// }


// void compute_ell(ELLArrays * ell, ValueType * x , ValueType * y)
// {
	// ValueType sum;
	// long i, j, j_s, j_e;
	// Vector_Value_t zero = {0};
	// Vector_Value_t v_a, v_x = zero, v_mul = zero, v_sum = zero;
	// for (i=0;i<ell->m;i+=VECTOR_ELEM_NUM)
	// {
		// j_s = i * ell->width;
		// j_e = (i + 1) * ell->width;
		// v_sum = zero;
		// for (j=j_s;j<j_e;j++)
			// sum += ell->a[j] * x[ell->ja[j]];
		// y[i] = sum;
	// }
// }


#endif /* SPMV_KERNELS_H */

