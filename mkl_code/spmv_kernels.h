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


ValueType *
transpose(ValueType * a, MKL_INT m, MKL_INT n)
{
	ValueType * t = (ValueType *) mkl_malloc(m*n * sizeof(*t), 64);
	#pragma omp parallel
	{
		long i, j;
		#pragma omp for schedule(static)
		for (j=0;j<n;j++)
		{
			for (i=0;i<m;i++)
				t[j*m + i] = a[i*n + j];
		}
	}
	// for (i=0;i<n;i++)
	// {
		// for (long j=0;j<m;j++)
			// printf("%lf ", a[i*m + j]);
		// printf("\n");
	// }
	return t;
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


// void compute_ldu_csr(LDUArrays * ldu, ValueType * x , ValueType * y)
// {
	// for (i=0;i<csr->m;i++)
	// {
		// y[i] = csr->diag[i] * x[i];
		// for (j=csr->ia[i];j<csr->ia[i+1];j++)
		// {
			// y[i] += csr->U[j] * x[csr->ja[j]];
			// y[i] += csr->L[csr->ja[j]] * x[j];
		// }
	// }
// }

/* void compute_ldu(LDUArrays * ldu, ValueType * x , ValueType * y)
{
	int num_threads = omp_get_max_threads();
	#pragma omp parallel
	{
		long tnum = omp_get_thread_num();
		ValueType sum;
		long i, i_s, i_e;
		long row, col;
		long row_s, row_e;

		#pragma omp for schedule(static)
		for (i=0;i<ldu->m;i++)
			y[i] = ldu->diag[i] * x[i];

		loop_partitioner_balance_iterations(num_threads, tnum, 0, ldu->upper_n, &i_s, &i_e);
		row_s = -1;
		row_e = -1;
		if (i_s < ldu->upper_n)
		{
			row_s = ldu->row_idx[i_s];
			row_e = ldu->row_idx[i_e-1];
			if (row_s == row_e)                       // Ending row is not the starting row.
				row_e = -1;
		}
		__atomic_store_n(&thread_partial_sums_row_s[tnum], row_s, __ATOMIC_RELAXED);
		__atomic_store_n(&thread_partial_sums_row_e[tnum], row_e, __ATOMIC_RELAXED);

		thread_partial_sums_s[tnum] = 0;
		thread_partial_sums_e[tnum] = 0;
		thread_partial_sums_row_s[tnum] = -1;
		thread_partial_sums_row_e[tnum] = -1;

		i = i_s;
		row = row_s;

		sum = 0;
		for (;i<i_e;i++)
		{
			row = ldu->row_idx[i];
			if (row != row_s)
				break;
			col = ldu->col_idx[i];
			sum += ldu->upper[i] * x[col];
		}
		__atomic_store(&thread_partial_sums_s[tnum], &sum, __ATOMIC_RELAXED);

		sum = 0;
		for (;i<i_e;i++)
		{
			if (row != ldu->row_idx[i])
			{
				y[row] = sum;
				row = ldu->row_idx[i];
				sum = 0;
			}
			col = ldu->col_idx[i];
			sum += ldu->upper[i] * x[col];
		}

		__atomic_store(&thread_partial_sums_e[tnum], &sum, __ATOMIC_RELAXED);
	}
} */


#endif /* SPMV_KERNELS_H */

