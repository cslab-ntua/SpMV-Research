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
	#include "random.h"
	#include "io.h"
	#include "parallel_io.h"
	#include "storage_formats/matrix_market/matrix_market.h"
	#include "storage_formats/openfoam/openfoam_matrix.h"
	// #include "vectorization.h"

	#include "aux/csr_converter_reference.h"

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


// void
// vector_pw_add(ValueType restrict * y, ValueType restrict * x1, ValueType a, ValueType restrict * x2, long N)
// {
	// vec_d_t v_a = vec_set1_pd(a);
	// long N_multiple;
	// long i;
	// N_multiple = (N/vec_len_pd) * vec_len_pd;
	// #pragma omp for
	// for (i=0;i<N_multiple;i+=vec_len_pd)
	// {
		// vec_storeu_pd(&y[i], vec_fmadd_pd(v_a, vec_loadu_pd(&x2[i]), vec_loadu_pd(&x1[i])));
	// }
	// #pragma omp single
	// {
		// for (i=N_multiple;i<N;i++)
			// y[i] = x1[i] + a * x2[i];
	// }
// }


void
vector_pw_add(ValueType restrict * y, ValueType restrict * x1, ValueType a, ValueType restrict * x2, long N)
{
	long i;
	#pragma omp for
	for (i=0;i<N;i++)
	{
		y[i] = x1[i] + a * x2[i];
	}
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
//= Preconditioned BiCGSTAB
//==========================================================================================================================================


/* Preconditioner: K == K1 * K2 ~= A
 * For the used Jacobi preconditioner we assume K2 == I and ignore it (i.e., K == K1).
 *
 * Algorithm:
 *
 * rk = b - A * xk
 * Choose an arbitrary vector r0_ such that (r0_, rk) != 0, e.g., r0_ = rk
 * s_pk_p = (r0_, rk)
 * pk = rk
 * For k = 1, 2, 3, ...
 *     y = inv(K2) * inv(K1) * pk
 *     v = A * y
 *     s_a = s_pk_p / (r0_, v)
 *     h = xk + s_a * y
 *     s = rk - s_a * v
 * 
 *     If h is accurate enough then xk = h and quit.
 * 
 *     z = inv(K2) * inv(K1) * s
 *     t = A * z
 *     s_w = (inv(K1) * t, inv(K1) * s) / (inv(K1) * t, inv(K1) * t)
 *     xk = h + s_w * z
 *     rk = s - s_w * t
 * 
 *     If xk is accurate enough then quit.
 * 
 *     s_pk = (r0_, rk)
 *     s_b = (s_pk / s_pk_p) * (s_a / s_w)
 *     pk = rk + s_b * (pk - s_w * v)
 *
 *     s_pk_p = s_pk
 */


void
preconditioned_bicgstab(
		struct Matrix_Format * MF,
		int * row_ptr, int * col_idx, ValueType * vals, 
		long m, __attribute__((unused)) long n, __attribute__((unused)) long nnz, ValueType * b, ValueType * x_res_out, long max_iterations)
{
	[[gnu::cleanup(cleanup_free)]] ValueType * r0_ = NULL;
	[[gnu::cleanup(cleanup_free)]] ValueType * rk = NULL;
	[[gnu::cleanup(cleanup_free)]] ValueType * rk_explicit = NULL;
	[[gnu::cleanup(cleanup_free)]] ValueType * pk = NULL;
	[[gnu::cleanup(cleanup_free)]] ValueType * xk = NULL;
	[[gnu::cleanup(cleanup_free)]] ValueType * x_best = NULL;
	[[gnu::cleanup(cleanup_free)]] ValueType * K = NULL;

	[[gnu::cleanup(cleanup_free)]] ValueType * z = NULL;
	[[gnu::cleanup(cleanup_free)]] ValueType * h = NULL;
	[[gnu::cleanup(cleanup_free)]] ValueType * s = NULL;
	[[gnu::cleanup(cleanup_free)]] ValueType * v = NULL;
	ValueType * y = NULL;
	ValueType * t = NULL;

	[[gnu::cleanup(cleanup_free)]] ValueType * buf = NULL;

	ValueType g_s_a, g_s_pk_p;

	r0_ = (typeof(r0_)) malloc(m * sizeof(*r0_));
	rk = (typeof(rk)) malloc(m * sizeof(*rk));
	rk_explicit = (typeof(rk_explicit)) malloc(m * sizeof(*rk_explicit));
	pk = (typeof(pk)) malloc(m * sizeof(*pk));
	z = (typeof(z)) malloc(m * sizeof(*z));
	h = (typeof(h)) malloc(m * sizeof(*h));
	s = (typeof(s)) malloc(m * sizeof(*s));
	v = (typeof(v)) malloc(m * sizeof(*v));
	buf = (typeof(buf)) malloc(m * sizeof(*buf));

	y = buf;
	t = buf;

	// Jacobi preconditioner (diagonal of A).
	K = (typeof(K)) malloc(m * sizeof(*K));
	#pragma omp parallel
	{
		long i, j;
		#pragma omp for
		for (i=0;i<m;i++)
		{
			K[i] = 0;
			for (j=row_ptr[i];j<row_ptr[i+1];j++)
			{
				if (i == col_idx[j])
				{
					K[i] = vals[j];
					break;
				}
			}
			if (K[i] == 0)
				error("bad K, zero in diagonal");
		}
	}

	xk = (typeof(xk)) malloc(n * sizeof(*xk));
	x_best = (typeof(x_best)) malloc(n * sizeof(*x_best));
	#pragma omp parallel
	{
		long i;
		#pragma omp for
		for (i=0;i<m;i++)
		{
			xk[i] = 0;
			// xk[i] = b[i];

			x_best[i] = xk[i];
		}
	}


	// rk = b - A * xk
	MF->spmv(xk, buf);
	#pragma omp parallel
	{
		ValueType tmp;
		long i;

		vector_pw_add(rk, b, -1, buf, m);

		// Choose an arbitrary vector r0_ such that (r0_, rk) != 0, e.g., r0_ = rk
		#pragma omp for
		for (i=0;i<m;i++)
			r0_[i] = rk[i];

		// s_pk_p = (r0_, rk)
		tmp = vector_dot(r0_, rk, m);
		#pragma omp single
		{
			g_s_pk_p = tmp;
		}

		// pk = rk
		#pragma omp for
		for (i=0;i<m;i++)
			pk[i] = rk[i];

	}

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
			MF->spmv(xk, buf);
			#pragma omp parallel
			{
				vector_pw_add(rk_explicit, b, -1, buf, m);
				double t_err = vector_norm(rk_explicit, m);
				#pragma omp single
				{
					err_explicit = t_err;
				}
			}
			if (err_explicit < err_best)
			{
				#pragma omp parallel
				{
					long i;
					#pragma omp for
					for (i=0;i<n;i++)
						x_best[i] = xk[i];
				}
				err_best = err_explicit;
			}
		}

		#pragma omp parallel
		{
			double t_err = vector_norm(rk, m);
			#pragma omp single
			{
				err = t_err;
			}
		}

		// if (err < eps)
			// break;

		// printf("k = %-10ld error = %-12.4g error_best = %-12.4g stop_counter = %ld\n", k, err, err_best, stop_counter);
		printf("k = %-10ld error = %-12.4g error_explicit = %-12.4g error_best = %-12.4g\n", k, err, err_explicit, err_best);
		// print_vector_summary(xk, m);
		// printf("\n");

		y = buf;

		#pragma omp parallel
		{
			long i;

			// y = inv(K2) * inv(K1) * pk
			#pragma omp for
			for (i=0;i<m;i++)
				y[i] = pk[i] / K[i];
		}

		// v = A * y
		MF->spmv(y, v);

		#pragma omp parallel
		{
			ValueType s_a=g_s_a, s_pk_p=g_s_pk_p;
			long i;

			// s_a = s_pk_p / (r0_, v)
			s_a = s_pk_p / vector_dot(r0_, v, m);
			#pragma omp single
			{
				g_s_a = s_a;
			}

			// h = xk + s_a * y
			vector_pw_add(h, xk, s_a, y, m);

			// s = rk - s_a * v
			vector_pw_add(s, rk, -s_a, v, m);

			// If h is accurate enough then xk = h and quit.

			// z = inv(K2) * inv(K1) * s
			#pragma omp for
			for (i=0;i<m;i++)
				z[i] = s[i] / K[i];
		}

		t = buf;

		// t = A * z
		MF->spmv(z, t);

		#pragma omp parallel
		{
			ValueType s_a=g_s_a, s_pk_p=g_s_pk_p, s_b, s_pk, s_w;
			long i;

			// s_w = (inv(K1) * t, inv(K1) * s) / (inv(K1) * t, inv(K1) * t)
			{
				// int tnum = omp_get_thread_num();
				ValueType total_1 = 0, total_2 = 0;
				ValueType partial_1 = 0, partial_2 = 0;
				ValueType val_1, val_2;
				partial_1 = 0;
				partial_2 = 0;
				#pragma omp for
				for (i=0;i<m;i++)
				{
					val_1 = t[i] / K[i];
					val_2 = s[i] / K[i];
					partial_1 += val_1 * val_2;
					partial_2 += val_1 * val_1;
				}
				omp_thread_reduce_global(reduce_add, partial_1, 0.0, 1, 0, , &total_1);
				omp_thread_reduce_global(reduce_add, partial_2, 0.0, 1, 0, , &total_2);
				s_w = total_1 / total_2;
			}

			// rk = s - s_w * t
			vector_pw_add(rk, s, -s_w, t, m);

			// xk = h + s_w * z
			vector_pw_add(xk, h, s_w, z, m);

			// If xk is accurate enough then quit.

			// s_pk = (r0_, rk)
			s_pk = vector_dot(r0_, rk, m);

			// s_b = (s_pk / s_pk_p) * (s_a / s_w)
			s_b = (s_pk / s_pk_p) * (s_a / s_w);

			// pk = rk + s_b * (pk - s_w * v)
			#pragma omp for
			for (i=0;i<m;i++)
			{
				pk[i] = rk[i] + s_b * (pk[i] - s_w * v[i]);
			}

			// s_pk_p = s_pk
			s_pk_p = s_pk;
			#pragma omp single
			{
				g_s_pk_p = s_pk_p;
			}
		}

		k++;
	}

	MF->spmv(xk, buf);
	#pragma omp parallel
	{
		vector_pw_add(rk_explicit, b, -1, buf, m);
		double t_err = vector_norm(rk_explicit, m);
		#pragma omp single
		{
			err_explicit = t_err;
		}
	}
	if (err_explicit < err_best)
	{
		#pragma omp parallel
		{
			long i;
			#pragma omp for
			for (i=0;i<n;i++)
				x_best[i] = xk[i];
		}
		err_best = err_explicit;
	}

	#pragma omp parallel
	{
		long i;
		for (i=0;i<n;i++)
			x_res_out[i] = x_best[i];
	}

	num_loops_out = k;
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
	int num_threads = omp_get_max_threads();
	__attribute__((unused)) double time;
	long buf_n = 10000;
	char buf[buf_n + 1];
	long i, j;
	double J_estimated, W_avg;
	double err;
	ValueType * vec = (typeof(vec)) malloc(csr_n * sizeof(*vec));

	num_loops_out = 1;

	if (!print_labels)
	{
		// Warm up cpu.
		__attribute__((unused)) volatile double warmup_total;
		long A_warmup_n = (1<<20) * num_threads;
		double * A_warmup;
		double time_warm_up = time_it(1,
			A_warmup = (typeof(A_warmup)) malloc(A_warmup_n * sizeof(*A_warmup));
			_Pragma("omp parallel for")
			for (long i=0;i<A_warmup_n;i++)
				A_warmup[i] = 0;
			for (j=0;j<16;j++)
			{
				_Pragma("omp parallel for")
				for (long i=1;i<A_warmup_n;i++)
				{
					A_warmup[i] += A_warmup[i-1] * 7 + 3;
				}
			}
			warmup_total = A_warmup[A_warmup_n];
			free(A_warmup);
		);
		printf("time warm up %lf\n", time_warm_up);

		// Warm up caches.
		MF->spmv(x, vec);



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
			preconditioned_bicgstab(MF, csr_ia, csr_ja, csr_a, csr_m, csr_n, csr_nnz, b, x, max_num_loops);
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
	i += snprintf(buf + i, buf_n - i, ",%lu", MF->m);
	i += snprintf(buf + i, buf_n - i, ",%lu", MF->n);
	i += snprintf(buf + i, buf_n - i, ",%lu", MF->nnz);
	i += snprintf(buf + i, buf_n - i, ",%lf", MF->mem_footprint / (1024*1024));
	i += snprintf(buf + i, buf_n - i, ",%lf", MF->mem_footprint / MF->csr_mem_footprint);
	#ifdef PRINT_STATISTICS
		i += MF->statistics_print_data(buf + i, buf_n - i);
	#endif
	buf[i] = '\0';
	fprintf(stderr, "%s\n", buf);

	free(vec);
}


//==========================================================================================================================================
//= Main
//==========================================================================================================================================

int
main(int argc, char **argv)
{
	__attribute__((unused)) int num_threads;

	struct Matrix_Market * MTX, * MTX_b;
	double * coo_val = NULL;
	INT_T * coo_rowind = NULL;
	INT_T * coo_colind = NULL;
	INT_T coo_m = 0;
	INT_T coo_n = 0;
	INT_T coo_nnz = 0;
	INT_T coo_nnz_diag = 0;
	INT_T coo_nnz_non_diag = 0;
	INT_T coo_symmetric = 0;
	long expand_symmetry = 1;

	double * csr_a_ref = NULL;

	ValueType * csr_a = NULL; // values (of size NNZ)
	INT_T * csr_ia = NULL;    // rowptr (of size m+1)
	INT_T * csr_ja = NULL;    // colidx of each NNZ (of size nnz)
	INT_T csr_m = 0;
	INT_T csr_n = 0;
	INT_T csr_nnz = 0;
	[[gnu::unused]] INT_T csr_nnz_diag = 0;
	[[gnu::unused]] INT_T csr_nnz_non_diag = 0;
	int csr_symmetric = 0;

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
		#ifdef KEEP_SYMMETRY
			expand_symmetry = 0;
		#else
			expand_symmetry = 1;
		#endif
		long pattern_dummy_vals = 1;
		MTX = mtx_read(file_in, expand_symmetry, pattern_dummy_vals);
		coo_rowind = MTX->R;
		coo_colind = MTX->C;
		coo_m = MTX->m;
		coo_n = MTX->n;
		coo_nnz_diag = MTX->nnz_diag;
		coo_nnz_non_diag = MTX->nnz_non_diag;
		coo_symmetric = MTX->symmetric;
		#ifdef KEEP_SYMMETRY
			coo_nnz = MTX->nnz_sym;
		#else
			coo_nnz = MTX->nnz;
		#endif
		mtx_values_convert_to_real(MTX);
		coo_val = (typeof(coo_val)) MTX->V;
	);
	printf("time read: %lf\n", time);

	if (coo_m != coo_n)
		error("the matrix must be square");

	// Fix zero diagonal entries by replacing them with a random number in [0, 1).
	if (atoi(getenv("CG_FIX_DIAGONAL_ZEROS")) == 1)
	{
		int * flags = (typeof(flags)) malloc(coo_m * sizeof(*flags));
		int diag_nnz_missing = 0;
		#pragma omp parallel
		{
			int tnum = omp_get_thread_num();
			struct Random_State * rs = random_new(tnum);
			int t_diag_nnz_missing = 0;
			long i;
			#pragma omp for
			for (i=0;i<coo_m;i++)
			{
				flags[i] = 0;
			}
			#pragma omp for
			for (i=0;i<coo_nnz;i++)
			{
				if (coo_rowind[i] == coo_colind[i])
				{
					if (coo_val[i] == 0)
					{
						coo_val[i] = random_uniform(rs, 0, 1);
					}
					flags[coo_rowind[i]] = 1;
				}
			}
			#pragma omp for
			for (i=0;i<coo_m;i++)
			{
				if (flags[i] == 0)
					t_diag_nnz_missing++;
			}
			__atomic_fetch_add(&diag_nnz_missing, t_diag_nnz_missing, __ATOMIC_RELAXED);
			random_destroy(&rs);
		}
		if (diag_nnz_missing > 0)
		{
			printf("Adding missing diagonal nonzeros\n");
			double * coo_val_new = (typeof(coo_val_new)) malloc((coo_nnz+diag_nnz_missing) * sizeof(coo_val_new));
			INT_T * coo_rowind_new = (typeof(coo_rowind_new)) malloc((coo_nnz+diag_nnz_missing) * sizeof(coo_rowind_new));
			INT_T * coo_colind_new = (typeof(coo_colind_new)) malloc((coo_nnz+diag_nnz_missing) * sizeof(coo_colind_new));
			long k = 0;
			#pragma omp parallel
			{
				int tnum = omp_get_thread_num();
				struct Random_State * rs = random_new(tnum);
				long i, j;
				#pragma omp for
				for (i=0;i<coo_nnz;i++)
				{
					coo_val_new[i] = coo_val[i];
					coo_rowind_new[i] = coo_rowind[i];
					coo_colind_new[i] = coo_colind[i];
				}
				#pragma omp for
				for (i=0;i<coo_m;i++)
				{
					if (flags[i] == 0)
					{
						j = __atomic_fetch_add(&k, 1, __ATOMIC_RELAXED);
						coo_val_new[coo_nnz+j] = random_uniform(rs, 0, 1);
						coo_rowind_new[coo_nnz+j] = i;
						coo_colind_new[coo_nnz+j] = i;
					}
				}
				random_destroy(&rs);
			}

			free(coo_val);
			free(coo_rowind);
			free(coo_colind);
			coo_val = coo_val_new;
			coo_rowind = coo_rowind_new;
			coo_colind = coo_colind_new;
			MTX->V = coo_val;
			MTX->R = coo_rowind;
			MTX->C = coo_colind;

			coo_nnz_diag += diag_nnz_missing;
			coo_nnz += diag_nnz_missing;
		}
		free(flags);
	}

	time = time_it(1,
		csr_a_ref = (typeof(csr_a_ref)) aligned_alloc(64, coo_nnz * sizeof(*csr_a_ref));
		csr_a = (typeof(csr_a)) aligned_alloc(64, coo_nnz * sizeof(*csr_a));
		csr_ja = (typeof(csr_ja)) aligned_alloc(64, coo_nnz * sizeof(*csr_ja));
		csr_ia = (typeof(csr_ia)) aligned_alloc(64, (coo_m+1) * sizeof(*csr_ia));
		csr_m = coo_m;
		csr_n = coo_n;
		csr_nnz = coo_nnz;
		csr_nnz_diag = coo_nnz_diag;
		csr_nnz_non_diag = coo_nnz_non_diag;
		csr_symmetric = coo_symmetric;
		_Pragma("omp parallel for")
		for (long i=0;i<coo_nnz;i++)
		{
			csr_a_ref[i] = 0.0;
			csr_ja[i] = 0;
		}
		_Pragma("omp parallel for")
		for (long i=0;i<coo_m+1;i++)
			csr_ia[i] = 0;
		coo_to_csr(coo_rowind, coo_colind, coo_val, coo_m, coo_n, coo_nnz, csr_ia, csr_ja, csr_a_ref, 1, 0);
		_Pragma("omp parallel for")
		for (long i=0;i<coo_nnz;i++)
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
		MF = csr_to_format(csr_ia, csr_ja, csr_a, csr_m, csr_n, csr_nnz, csr_symmetric, expand_symmetry);
	);
	printf("time convert to format: %lf\n", time);

	char * max_num_loops_list = getenv("CG_MAX_NUM_ITERS");
	if (max_num_loops_list == NULL)
		error("max_num_loops_list is empty");
	long max_num_loops_list_len = strlen(max_num_loops_list);
	long max_num_loops;

	prefetch_distance = 1;
	char * max_num_loops_str = max_num_loops_list;
	while (*max_num_loops_str != 0)
	{
		max_num_loops = atol(max_num_loops_str);
		#pragma omp parallel for
		for(int i=0;i<csr_n;++i)
			x[i] = 0;
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

		while (*max_num_loops_str != 0)
		{
			if (*max_num_loops_str == ' ')
			{
				while ((*max_num_loops_str != 0) && (*max_num_loops_str == ' '))
					max_num_loops_str++;
				break;
			}
			max_num_loops_str++;
		}

		if (atoi(getenv("COOLDOWN")) == 1)
		{
			printf("time total = %g, sleeping\n", time);
			usleep((long) (time * 1000000));
		}
	}

	free(b);
	free(x);
	return 0;
}

