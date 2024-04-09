#include <iostream>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <pthread.h>
#include <sstream>

#include <unistd.h>

#include "spmv_bench_common.h"

#ifdef __cplusplus
extern "C"{
#endif

	#include "macros/cpp_defines.h"
	#include "macros/macrolib.h"
	#include "time_it.h"
	#include "parallel_util.h"
	#include "pthread_functions.h"
	#include "matrix_util.h"
	#include "array_metrics.h"

	#include "string_util.h"
	#include "io.h"
	#include "parallel_io.h"
	#include "storage_formats/matrix_market/matrix_market.h"
	#include "storage_formats/openfoam/openfoam_matrix.h"
	#include "read_mtx.h"

	#include "aux/csr_converter_double.h"

	#include "aux/csr_util.h"

	#include "monitoring/power/rapl.h"

	#include "artificial_matrix_generation.h"

#ifdef __cplusplus
}
#endif

#include "spmv_kernel.h"


#ifndef BLOCK_SIZE
	#define BLOCK_SIZE  64
#endif


int prefetch_distance = 8;

long num_loops_out;



void
vector_pw_add(ValueType * y, ValueType * x1, ValueType a, ValueType * x2, long N)
{
	long i;
	#pragma omp for
	for (i=0;i<N;i++)
		y[i] = x1[i] + a * x2[i];
}


ValueType
reduce_add(ValueType a, ValueType b)
{
	return a + b;
}

ValueType
vector_dot(ValueType * x1, ValueType * x2, long N)
{
	// int tnum = omp_get_thread_num();
	ValueType total = 0;
	ValueType partial = 0;
	long i;
	partial = 0;
	#pragma omp for
	for (i=0;i<N;i++)
		partial += x1[i] * x2[i];
	omp_thread_reduce_global(reduce_add, partial, 0.0, 1, 0, , &total);
	return total;
}


ValueType
vector_norm(ValueType * x, long N)
{
	return sqrt(vector_dot(x, x, N));
}


//==========================================================================================================================================
//= Preconditioned CG
//==========================================================================================================================================


void
preconditioned_cg(
		struct Matrix_Format * MF,
		int * row_ptr, int * col_idx, ValueType * vals, 
		long m, __attribute__((unused)) long n, __attribute__((unused)) long nnz, ValueType * b, ValueType * x_res_out, long max_iterations)
{
	ValueType * rk;
	ValueType * rk_explicit;
	ValueType * pk;
	ValueType * zk;
	ValueType * x;
	ValueType * x_best;
	ValueType * buf_A_dot_pk;   // Column buffer (size of number of rows).
	ValueType * C;
	long i, j;

	rk = (typeof(rk)) malloc(m * sizeof(*rk));
	rk_explicit = (typeof(rk_explicit)) malloc(m * sizeof(*rk_explicit));
	pk = (typeof(pk)) malloc(m * sizeof(*pk));
	zk = (typeof(zk)) malloc(m * sizeof(*zk));
	buf_A_dot_pk = (typeof(buf_A_dot_pk)) malloc(m * sizeof(*buf_A_dot_pk));

	// Jacobi preconditioner (diagonal of A).
	C = (typeof(C)) malloc(m * sizeof(*C));
	for (i=0;i<m;i++)
	{
		C[i] = 0;
		for (j=row_ptr[i];j<row_ptr[i+1];j++)
		{
			if (i == col_idx[j])
			{
				C[i] = vals[j];
				break;
			}
		}
		if (C[i] == 0)
			error("bad C");
	}

	x = (typeof(x)) malloc(n * sizeof(*x));
	x_best = (typeof(x_best)) malloc(n * sizeof(*x_best));

	for (i=0;i<m;i++)
	{
		x[i] = 0;
		// x[i] = b[i];

		x_best[i] = x[i];
	}

	// r0 = b - A*x0
	MF->spmv(x, buf_A_dot_pk);
	#pragma omp parallel
	{
		vector_pw_add(rk, b, -1, buf_A_dot_pk, m);
	}

	// solve C*z0 = r0
	for (i=0;i<m;i++)
		zk[i] = rk[i] / C[i];

	// p0 = z0
	memcpy(pk, zk, m * sizeof(*pk));

	double eps, eps_counter, err, err_explicit, err_best;
	eps = 1.0e-15;
	eps_counter = 1.0e-7;

	#pragma omp parallel
	{
		double t_err = vector_norm(rk, m);
		double b_norm = vector_norm(b, m);
		#pragma omp single
		{
			err = t_err;
			eps = eps * b_norm;
			eps_counter = eps_counter * b_norm;
		}
	}
	printf("eps = %g eps_counter = %g\n", eps, eps_counter);

	long k = 0;
	long restart_k = 100;
	// long stop_counter = 0;
	err_explicit = err;
	err_best = err;
	while (k < max_iterations)
	{
		// until rk.dot(rk) < eps

		// Periodically calculate explicit residual.
		if ((k > 0) && !(k % restart_k))
		{
			MF->spmv(x, buf_A_dot_pk);
			#pragma omp parallel
			{
				vector_pw_add(rk_explicit, b, -1, buf_A_dot_pk, m);
				double t_err = vector_norm(rk_explicit, m);
				#pragma omp single
					err_explicit = t_err;
			}
			if (err_explicit < err_best)
			{
				#pragma omp parallel
				{
					long i;
					#pragma omp for
					for (i=0;i<n;i++)
						x_best[i] = x[i];
				}
				err_best = err_explicit;
			}
		}

		#pragma omp parallel
		{
			double t_err = vector_norm(rk, m);
			#pragma omp single
				err = t_err;
		}

		// If explicit and implicit residuals are too different restart the CG, but only when in the starting phase.
		if ((k > 0) && !(k % restart_k))
		{
			if ((err_best > eps_counter) && (err_explicit / err > 1e3))
			{
				memcpy(rk, rk_explicit, m * sizeof(*rk));

				// solve C*z0 = r0
				#pragma omp parallel
				{
					long i;
					#pragma omp for
					for (i=0;i<m;i++)
						zk[i] = rk[i] / C[i];
				}

				// p0 = z0
				memcpy(pk, zk, m * sizeof(*pk));
			}
		}

		if (err < eps)
			break;

		// printf("k = %-10ld error = %-12.4g error_best = %-12.4g stop_counter = %ld\n", k, err, err_best, stop_counter);
		printf("k = %-10ld error = %-12.4g error_explicit = %-12.4g error_best = %-12.4g\n", k, err, err_explicit, err_best);
		// print_vector_summary(x, m);
		// printf("\n");

		// A * pk
		MF->spmv(pk, buf_A_dot_pk);

		#pragma omp parallel
		{
			ValueType ak, bk;

			// double old_zk_dot_pk = vector_dot(zk, pk, m);
			ValueType old_zk_dot_rk = vector_dot(zk, rk, m);

			/* ak = (zk * rk) / (pk * A * pk) */
			ak = vector_dot(zk, rk, m) / vector_dot(pk, buf_A_dot_pk, m);

			// x = x + ak * pk
			vector_pw_add(x, x, ak, pk, m);

			// rk = rk - ak * A * pk
			vector_pw_add(rk, rk, -ak, buf_A_dot_pk, m);

			// solve C*zk = rk
			long i;
			#pragma omp for
			for (i=0;i<m;i++)
				zk[i] = rk[i] / C[i];

			// bk = (zk * rk) / (zk * pk)
			// bk = vector_dot(zk, rk, m) / old_zk_dot_pk;
			bk = vector_dot(zk, rk, m) / old_zk_dot_rk;
			// if (err < 1.0e-3)
				// bk = 0;

			// pk = zk + bk * pk
			vector_pw_add(pk, zk, bk, pk, m);
		}

		k++;
	}

	MF->spmv(x, buf_A_dot_pk);
	#pragma omp parallel
	{
		vector_pw_add(rk_explicit, b, -1, buf_A_dot_pk, m);
		double t_err = vector_norm(rk_explicit, m);
		#pragma omp single
			err_explicit = t_err;
	}
	if (err_explicit < err_best)
	{
		#pragma omp parallel
		{
			long i;
			#pragma omp for
			for (i=0;i<n;i++)
				x_best[i] = x[i];
		}
		err_best = err_explicit;
	}

	for (i=0;i<n;i++)
		x_res_out[i] = x_best[i];

	num_loops_out = k;

	free(rk);
	free(pk);
	free(buf_A_dot_pk);
	free(x);
}


//==========================================================================================================================================
//= Compute
//==========================================================================================================================================


void
compute(char * matrix_name,
		INT_T * csr_ia, INT_T * csr_ja, ValueType * csr_a,
		INT_T csr_m, INT_T csr_n, INT_T csr_nnz,
		struct Matrix_Format * MF,
		ValueType * b, ValueType * x,
		long max_num_loops,
		long print_labels)
{
	__attribute__((unused)) double time;
	long buf_n = 10000;
	char buf[buf_n + 1];
	long i;
	double J_estimated, W_avg;
	double err;

	num_loops_out = 1;

	if (!print_labels)
	{
		#ifdef PRINT_STATISTICS
			MF->statistics_start();
		#endif

		/*****************************************************************************************/
		struct RAPL_Register * regs;
		long regs_n;
		char * reg_ids;

		reg_ids = NULL;
		reg_ids = (char *) getenv("RAPL_REGISTERS");

		rapl_open(reg_ids, &regs, &regs_n);
		/*****************************************************************************************/

		time = 0;
		rapl_read_start(regs, regs_n);

		time += time_it(1,
			preconditioned_cg(MF, csr_ia, csr_ja, csr_a, csr_m, csr_n, csr_nnz, b, x, max_num_loops);
		);

		rapl_read_end(regs, regs_n);

		/*****************************************************************************************/
		J_estimated = 0;
		for (i=0;i<regs_n;i++){
			// printf("'%s' total joule = %g\n", regs[i].type, ((double) regs[i].uj_accum) / 1000000);
			J_estimated += ((double) regs[i].uj_accum) / 1e6;
		}
		rapl_close(regs, regs_n);
		free(regs);
		W_avg = J_estimated / time;
		// printf("J_estimated = %lf\tW_avg = %lf\n", J_estimated, W_avg);
		/*****************************************************************************************/

		//=============================================================================
		//= Output section.
		//=============================================================================

		ValueType * vec = (typeof(vec)) malloc(csr_n * sizeof(*vec));
		MF->spmv(x, vec);
		#pragma omp parallel
		{
			vector_pw_add(vec, b, -1, vec, csr_n);
			err = vector_norm(vec, csr_n);
		}
		printf("error = %-12.4g\n", err);

	}

	if (print_labels)
	{
		i = 0;
		i += snprintf(buf + i, buf_n - i, "%s", "matrix_name");
		i += snprintf(buf + i, buf_n - i, ",%s", "num_threads");
		i += snprintf(buf + i, buf_n - i, ",%s", "csr_m");
		i += snprintf(buf + i, buf_n - i, ",%s", "csr_n");
		i += snprintf(buf + i, buf_n - i, ",%s", "csr_nnz");
		i += snprintf(buf + i, buf_n - i, ",%s", "time");
		i += snprintf(buf + i, buf_n - i, ",%s", "error");
		i += snprintf(buf + i, buf_n - i, ",%s", "num_iterations");
		i += snprintf(buf + i, buf_n - i, ",%s", "csr_mem_footprint");
		i += snprintf(buf + i, buf_n - i, ",%s", "W_avg");
		i += snprintf(buf + i, buf_n - i, ",%s", "J_estimated");
		i += snprintf(buf + i, buf_n - i, ",%s", "format_name");
		i += snprintf(buf + i, buf_n - i, ",%s", "m");
		i += snprintf(buf + i, buf_n - i, ",%s", "n");
		i += snprintf(buf + i, buf_n - i, ",%s", "nnz");
		i += snprintf(buf + i, buf_n - i, ",%s", "mem_footprint");
		i += snprintf(buf + i, buf_n - i, ",%s", "mem_ratio");
		#ifdef PRINT_STATISTICS
			i += statistics_print_labels(buf + i, buf_n - i);
		#endif
		buf[i] = '\0';
		fprintf(stderr, "%s\n", buf);
		return;
	}
	i = 0;
	i += snprintf(buf + i, buf_n - i, "%s", matrix_name);
	i += snprintf(buf + i, buf_n - i, ",%d", omp_get_max_threads());
	i += snprintf(buf + i, buf_n - i, ",%u", csr_m);
	i += snprintf(buf + i, buf_n - i, ",%u", csr_n);
	i += snprintf(buf + i, buf_n - i, ",%u", csr_nnz);
	i += snprintf(buf + i, buf_n - i, ",%lf", time);
	i += snprintf(buf + i, buf_n - i, ",%g", err);
	i += snprintf(buf + i, buf_n - i, ",%ld", num_loops_out);
	i += snprintf(buf + i, buf_n - i, ",%lf", MF->csr_mem_footprint / (1024*1024));
	i += snprintf(buf + i, buf_n - i, ",%lf", W_avg);
	i += snprintf(buf + i, buf_n - i, ",%lf", J_estimated);
	i += snprintf(buf + i, buf_n - i, ",%s", MF->format_name);
	i += snprintf(buf + i, buf_n - i, ",%u", MF->m);
	i += snprintf(buf + i, buf_n - i, ",%u", MF->n);
	i += snprintf(buf + i, buf_n - i, ",%u", MF->nnz);
	i += snprintf(buf + i, buf_n - i, ",%lf", MF->mem_footprint / (1024*1024));
	i += snprintf(buf + i, buf_n - i, ",%lf", MF->mem_footprint / MF->csr_mem_footprint);
	#ifdef PRINT_STATISTICS
		i += MF->statistics_print_data(buf + i, buf_n - i);
	#endif
	buf[i] = '\0';
	fprintf(stderr, "%s\n", buf);
}


//==========================================================================================================================================
//= Main
//==========================================================================================================================================

int
main(int argc, char **argv)
{
	__attribute__((unused)) int num_threads;

	struct Matrix_Market * MTX, * MTX_b;
	double * mtx_val = NULL;
	INT_T * mtx_rowind = NULL;
	INT_T * mtx_colind = NULL;
	INT_T mtx_m = 0;
	INT_T mtx_n = 0;
	INT_T mtx_nnz = 0;

	double * csr_a_ref = NULL;

	ValueType * csr_a = NULL; // values (of size NNZ)
	INT_T * csr_ia = NULL;    // rowptr (of size m+1)
	INT_T * csr_ja = NULL;    // colidx of each NNZ (of size nnz)
	INT_T csr_m = 0;
	INT_T csr_n = 0;
	INT_T csr_nnz = 0;

	struct Matrix_Format * MF;   // Real matrices.
	ValueType * b;
	ValueType * x;
	char matrix_name[1000];
	__attribute__((unused)) double time;
	__attribute__((unused)) long i, j;

	// Wake omp up from eternal slumber.
	#pragma omp parallel
	{
		num_threads = omp_get_max_threads();
	}
	printf("max threads %d\n", num_threads);

	// Just print the labels and exit.
	if (argc == 1)
	{
		compute(NULL, NULL, NULL, NULL, 0, 0, 0, NULL, NULL, NULL, 0, 1);
		return 0;
	}

	char * file_in, * file_b;
	i = 1;
	file_in = argv[i++];
	snprintf(matrix_name, sizeof(matrix_name), "%s", file_in);

	time = time_it(1,
		// create_coo_matrix(file_in, &mtx_val, &mtx_rowind, &mtx_colind, &mtx_m, &mtx_n, &mtx_nnz);
		long expand_symmetry = 1;
		long pattern_dummy_vals = 1;
		MTX = mtx_read(file_in, expand_symmetry, pattern_dummy_vals);
		mtx_rowind = MTX->R;
		mtx_colind = MTX->C;
		mtx_m = MTX->m;
		mtx_n = MTX->n;
		mtx_nnz = MTX->nnz;
		if (!strcmp(MTX->field, "integer"))
		{
			mtx_val = (typeof(mtx_val)) malloc(mtx_nnz * sizeof(*mtx_val));
			_Pragma("omp parallel for")
			for (long i=0;i<mtx_nnz;i++)
			{
				mtx_val[i] = ((int *) MTX->V)[i];
			}
			free(MTX->V);
			MTX->V = NULL;
		}
		else if (!strcmp(MTX->field, "complex"))
		{
			mtx_val = (typeof(mtx_val)) malloc(mtx_nnz * sizeof(*mtx_val));
			_Pragma("omp parallel for")
			for (long i=0;i<mtx_nnz;i++)
			{
				#if DOUBLE == 0
					mtx_val[i] = cabsf(((complex ValueType *) MTX->V)[i]);
				#else
					mtx_val[i] = cabs(((complex ValueType *) MTX->V)[i]);
				#endif
			}
			free(MTX->V);
			MTX->V = NULL;
		}
		else
			mtx_val = (double *) MTX->V;
	);
	printf("time read: %lf\n", time);

	time = time_it(1,
		csr_a_ref = (typeof(csr_a_ref)) aligned_alloc(64, (mtx_nnz + VECTOR_ELEM_NUM) * sizeof(*csr_a_ref));
		csr_a = (typeof(csr_a)) aligned_alloc(64, (mtx_nnz + VECTOR_ELEM_NUM) * sizeof(*csr_a));
		csr_ja = (typeof(csr_ja)) aligned_alloc(64, (mtx_nnz + VECTOR_ELEM_NUM) * sizeof(*csr_ja));
		csr_ia = (typeof(csr_ia)) aligned_alloc(64, (mtx_m+1 + VECTOR_ELEM_NUM) * sizeof(*csr_ia));
		csr_m = mtx_m;
		csr_n = mtx_n;
		csr_nnz = mtx_nnz;
		_Pragma("omp parallel for")
		for (long i=0;i<mtx_nnz + VECTOR_ELEM_NUM;i++)
		{
			csr_a_ref[i] = 0.0;
			csr_ja[i] = 0;
		}
		_Pragma("omp parallel for")
		for (long i=0;i<mtx_m+1 + VECTOR_ELEM_NUM;i++)
			csr_ia[i] = 0;
		coo_to_csr(mtx_rowind, mtx_colind, mtx_val, mtx_m, mtx_n, mtx_nnz, csr_ia, csr_ja, csr_a_ref, 1, 0);
		_Pragma("omp parallel for")
		for (long i=0;i<mtx_nnz + VECTOR_ELEM_NUM;i++)
			csr_a[i] = (ValueType) csr_a_ref[i];
	);
	printf("time coo to csr: %lf\n", time);
	mtx_destroy(&MTX);


	long len_ext = 4;
	long len = strlen(file_in) - len_ext;
	long file_b_n = len + 100;
	file_b = (typeof(file_b)) malloc(file_b_n * sizeof(*file_b));
	memcpy(file_b, file_in, len);
	snprintf(file_b+len, file_b_n-len, "_b.mtx");
	printf("%s\n", file_b);

	if (stat_isreg(file_b))
	{
		time = time_it(1,
			MTX_b = mtx_read(file_b, 1, 1);
			b = (ValueType *) MTX_b->V;
		);
		printf("read vector file time = %lf\n", time);
		MTX_b->V = NULL;
		mtx_destroy(&MTX_b);
	}
	else
	{
		b = (typeof(b)) malloc(csr_n * sizeof(*b));
		for (i=0;i<csr_n;i++)
			b[i] = 1;
	}

	x = (typeof(x)) aligned_alloc(64, csr_n * sizeof(*x));
	#pragma omp parallel for
	for(int i=0;i<csr_n;++i)
		x[i] = 0;

	time = time_it(1,
		MF = csr_to_format(csr_ia, csr_ja, csr_a, csr_m, csr_n, csr_nnz);
	);
	printf("time convert to format: %lf\n", time);

	long max_num_loops;
	// max_num_loops = 1000;
	max_num_loops = 100000;

	prefetch_distance = 1;
	time = time_it(1,
		// for (i=0;i<5;i++)
		{
			compute(matrix_name,
				csr_ia, csr_ja, csr_a,
				csr_m, csr_n, csr_nnz,
				MF, b, x, max_num_loops, 0);
			prefetch_distance++;
		}
	);
	if (atoi(getenv("COOLDOWN")) == 1)
	{
		printf("time total = %g, sleeping\n", time);
		usleep((long) (time * 1000000));
	}

	free(b);
	free(x);
	return 0;
}



