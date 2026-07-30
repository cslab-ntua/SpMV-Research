#include <iostream>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <pthread.h>
#include <sstream>

#include <unistd.h>

#include <cuda.h>
#include <cublas_v2.h>

#include "bench_common.h"

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
	#include "monitoring/power/rapl.h"

	#include "aux/csr_converter_reference.h"
	#include "aux/csr_util.h"

	#include "artificial_matrix_generation.h"

	#include "cuda/cuda_util.h"

#ifdef __cplusplus
}
#endif

#include "spmv_kernels/spmv_kernel.h"


long num_loops_out;
double time_spmv_out;


//==========================================================================================================================================
//= GPU Vector Kernels
//==========================================================================================================================================


// y[i] = x1[i] + a * x2[i]
__global__ void
kernel_axpy(ValueType * y, ValueType * x1, ValueType a, ValueType * x2, long N)
{
	long i = blockIdx.x * blockDim.x + threadIdx.x;
	if (i < N)
		y[i] = x1[i] + a * x2[i];
}

static void
gpu_axpy(ValueType * y_d, ValueType * x1_d, ValueType a, ValueType * x2_d, long N)
{
	int block = 256;
	int grid = (N + block - 1) / block;
	kernel_axpy<<<grid, block>>>(y_d, x1_d, a, x2_d, N);
}


// y[i] = x[i] / d[i]
__global__ void
kernel_elem_div(ValueType * y, ValueType * x, ValueType * d, long N)
{
	long i = blockIdx.x * blockDim.x + threadIdx.x;
	if (i < N)
		y[i] = x[i] / d[i];
}

static void
gpu_elem_div(ValueType * y_d, ValueType * x_d, ValueType * d_d, long N)
{
	int block = 256;
	int grid = (N + block - 1) / block;
	kernel_elem_div<<<grid, block>>>(y_d, x_d, d_d, N);
}


// pk[i] = rk[i] + s_b * (pk[i] - s_w * v[i])
__global__ void
kernel_pk_update(ValueType * pk, ValueType * rk, ValueType * v, ValueType s_b, ValueType s_w, long N)
{
	long i = blockIdx.x * blockDim.x + threadIdx.x;
	if (i < N)
		pk[i] = rk[i] + s_b * (pk[i] - s_w * v[i]);
}

static void
gpu_pk_update(ValueType * pk_d, ValueType * rk_d, ValueType * v_d, ValueType s_b, ValueType s_w, long N)
{
	int block = 256;
	int grid = (N + block - 1) / block;
	kernel_pk_update<<<grid, block>>>(pk_d, rk_d, v_d, s_b, s_w, N);
}


// dst[i] = src[i]
__global__ void
kernel_copy(ValueType * dst, ValueType * src, long N)
{
	long i = blockIdx.x * blockDim.x + threadIdx.x;
	if (i < N)
		dst[i] = src[i];
}

static void
gpu_copy(ValueType * dst_d, ValueType * src_d, long N)
{
	int block = 256;
	int grid = (N + block - 1) / block;
	kernel_copy<<<grid, block>>>(dst_d, src_d, N);
}


// Fused s_w computation:
// s_w = sum( (t[i]/K[i]) * (s[i]/K[i]) ) / sum( (t[i]/K[i])^2 )
// Uses shared memory reduction within blocks, then a second pass or atomicAdd for block results.
// For simplicity, use two cublasDdot calls with a temporary scaled vector.

// Actually: compute into two temp scalars using cuBLAS on temp vectors.
// But that requires temp vectors. Instead, let's do a single custom reduction kernel.

// We'll use the simpler approach: compute t_scaled = t/K and s_scaled = s/K into
// existing buffer space, then use cublasDdot. We reuse h_d and z_d as temp (they are
// not needed at this point in the algorithm — s_w is computed between steps 7 and 9,
// and h/z are no longer read after step 10 but are overwritten next iteration anyway).
// Actually, h and z ARE read after s_w (steps 9 uses buf=t, step 10 uses h and z).
// So we can't reuse them. Let's just do a custom reduction.

// Block-level reduction for two partial sums.
__global__ void
kernel_sw_reduce(ValueType * t, ValueType * s, ValueType * K,
                 ValueType * partial_num, ValueType * partial_den, long N)
{
	extern __shared__ ValueType sdata[];
	ValueType * snum = sdata;
	ValueType * sden = sdata + blockDim.x;

	long i = blockIdx.x * blockDim.x + threadIdx.x;
	int tid = threadIdx.x;

	ValueType lnum = 0.0, lden = 0.0;
	// Grid-stride loop for large N.
	for (long idx = i; idx < N; idx += (long)gridDim.x * blockDim.x)
	{
		ValueType t_scaled = t[idx] / K[idx];
		ValueType s_scaled = s[idx] / K[idx];
		lnum += t_scaled * s_scaled;
		lden += t_scaled * t_scaled;
	}
	snum[tid] = lnum;
	sden[tid] = lden;
	__syncthreads();

	// Reduction in shared memory.
	for (int stride = blockDim.x / 2; stride > 0; stride >>= 1)
	{
		if (tid < stride)
		{
			snum[tid] += snum[tid + stride];
			sden[tid] += sden[tid + stride];
		}
		__syncthreads();
	}

	if (tid == 0)
	{
		partial_num[blockIdx.x] = snum[0];
		partial_den[blockIdx.x] = sden[0];
	}
}

// Second pass: reduce the block-level partials (small array).
__global__ void
kernel_sw_reduce_final(ValueType * partial_num, ValueType * partial_den,
                       ValueType * result_num, ValueType * result_den, int num_blocks)
{
	extern __shared__ ValueType sdata[];
	ValueType * snum = sdata;
	ValueType * sden = sdata + blockDim.x;

	int tid = threadIdx.x;
	ValueType lnum = 0.0, lden = 0.0;
	for (int idx = tid; idx < num_blocks; idx += blockDim.x)
	{
		lnum += partial_num[idx];
		lden += partial_den[idx];
	}
	snum[tid] = lnum;
	sden[tid] = lden;
	__syncthreads();

	for (int stride = blockDim.x / 2; stride > 0; stride >>= 1)
	{
		if (tid < stride)
		{
			snum[tid] += snum[tid + stride];
			sden[tid] += sden[tid + stride];
		}
		__syncthreads();
	}

	if (tid == 0)
	{
		*result_num = snum[0];
		*result_den = sden[0];
	}
}

// Host wrapper: computes s_w = numerator / denominator.
// partial_num_d and partial_den_d are pre-allocated device buffers of size >= grid.
static ValueType
gpu_compute_sw(ValueType * t_d, ValueType * s_d, ValueType * K_d, long N,
               ValueType * partial_num_d, ValueType * partial_den_d)
{
	int block = 256;
	int grid = (N + block - 1) / block;
	if (grid > 1024) grid = 1024;  // Cap grid size; grid-stride loop handles the rest.

	size_t smem = 2 * block * sizeof(ValueType);

	kernel_sw_reduce<<<grid, block, smem>>>(t_d, s_d, K_d, partial_num_d, partial_den_d, N);

	// Final reduction (single block).
	int final_block = 256;
	if (final_block > grid) final_block = grid;
	// Round down to power of 2 for clean reduction.
	int fb = 1;
	while (fb * 2 <= final_block) fb *= 2;
	final_block = fb;

	ValueType result_num, result_den;
	ValueType * result_num_d;
	ValueType * result_den_d;
	// Use the end of the partial arrays as temp for the scalar results.
	result_num_d = partial_num_d + grid;
	result_den_d = partial_den_d + grid;

	size_t smem2 = 2 * final_block * sizeof(ValueType);
	kernel_sw_reduce_final<<<1, final_block, smem2>>>(partial_num_d, partial_den_d,
	                                                   result_num_d, result_den_d, grid);

	gpuCudaErrorCheck(cudaMemcpy(&result_num, result_num_d, sizeof(ValueType), cudaMemcpyDeviceToHost));
	gpuCudaErrorCheck(cudaMemcpy(&result_den, result_den_d, sizeof(ValueType), cudaMemcpyDeviceToHost));

	return result_num / result_den;
}


//==========================================================================================================================================
//= Preconditioned BiCGSTAB (GPU)
//==========================================================================================================================================


void
preconditioned_bicgstab_gpu(
		struct Matrix_Format * MF,
		int * row_ptr, int * col_idx, ValueType * vals,
		long m, __attribute__((unused)) long n, __attribute__((unused)) long nnz, ValueType * b, ValueType * x_res_out, long max_iterations)
{
	// =====================================================================
	// Phase 1: Compute Jacobi preconditioner on CPU, then move everything to GPU.
	// =====================================================================

	ValueType * K_h = (ValueType *) malloc(m * sizeof(ValueType));
	#pragma omp parallel for
	for (long i = 0; i < m; i++)
	{
		K_h[i] = 0;
		for (int j = row_ptr[i]; j < row_ptr[i+1]; j++)
		{
			if (i == col_idx[j])
			{
				K_h[i] = vals[j];
				break;
			}
		}
		if (K_h[i] == 0)
			error("bad K, zero in diagonal");
	}

	// Allocate all device vectors.
	ValueType * b_d, * K_d;
	ValueType * r0__d, * rk_d, * rk_explicit_d, * pk_d;
	ValueType * xk_d, * x_best_d;
	ValueType * z_d, * h_d, * s_d, * v_d, * buf_d;

	gpuCudaErrorCheck(cudaMalloc(&b_d, m * sizeof(ValueType)));
	gpuCudaErrorCheck(cudaMalloc(&K_d, m * sizeof(ValueType)));
	gpuCudaErrorCheck(cudaMalloc(&r0__d, m * sizeof(ValueType)));
	gpuCudaErrorCheck(cudaMalloc(&rk_d, m * sizeof(ValueType)));
	gpuCudaErrorCheck(cudaMalloc(&rk_explicit_d, m * sizeof(ValueType)));
	gpuCudaErrorCheck(cudaMalloc(&pk_d, m * sizeof(ValueType)));
	gpuCudaErrorCheck(cudaMalloc(&xk_d, n * sizeof(ValueType)));
	gpuCudaErrorCheck(cudaMalloc(&x_best_d, n * sizeof(ValueType)));
	gpuCudaErrorCheck(cudaMalloc(&z_d, m * sizeof(ValueType)));
	gpuCudaErrorCheck(cudaMalloc(&h_d, m * sizeof(ValueType)));
	gpuCudaErrorCheck(cudaMalloc(&s_d, m * sizeof(ValueType)));
	gpuCudaErrorCheck(cudaMalloc(&v_d, m * sizeof(ValueType)));
	gpuCudaErrorCheck(cudaMalloc(&buf_d, m * sizeof(ValueType)));

	// Partial reduction buffers for s_w computation (1024 blocks + 2 scalars).
	ValueType * partial_num_d, * partial_den_d;
	gpuCudaErrorCheck(cudaMalloc(&partial_num_d, (1024 + 1) * sizeof(ValueType)));
	gpuCudaErrorCheck(cudaMalloc(&partial_den_d, (1024 + 1) * sizeof(ValueType)));

	// Transfer initial data H→D.
	gpuCudaErrorCheck(cudaMemcpy(b_d, b, m * sizeof(ValueType), cudaMemcpyHostToDevice));
	gpuCudaErrorCheck(cudaMemcpy(K_d, K_h, m * sizeof(ValueType), cudaMemcpyHostToDevice));
	gpuCudaErrorCheck(cudaMemset(xk_d, 0, n * sizeof(ValueType)));

	free(K_h);

	// cuBLAS handle for dot products and norms.
	cublasHandle_t cublas_handle;
	gpuCudaErrorCheck((cudaError_t) cublasCreate(&cublas_handle));

	time_spmv_out = 0;

	// =====================================================================
	// Phase 1b: Initial residual and setup (all on GPU).
	// =====================================================================

	// rk = b - A * xk  (xk is zero, so A*xk = 0, rk = b)
	MF->spmv_gpu(xk_d, buf_d);
	gpu_axpy(rk_d, b_d, -1.0, buf_d, m);
	gpuCudaErrorCheck(cudaDeviceSynchronize());

	// r0_ = rk
	gpu_copy(r0__d, rk_d, m);
	gpuCudaErrorCheck(cudaDeviceSynchronize());

	// s_pk_p = (r0_, rk)
	ValueType g_s_pk_p;
	#if DOUBLE == 1
		gpuCudaErrorCheck((cudaError_t) cublasDdot(cublas_handle, m, r0__d, 1, rk_d, 1, &g_s_pk_p));
	#else
		gpuCudaErrorCheck((cudaError_t) cublasSdot(cublas_handle, m, r0__d, 1, rk_d, 1, &g_s_pk_p));
	#endif

	// pk = rk
	gpu_copy(pk_d, rk_d, m);

	// x_best = xk (zeros)
	gpuCudaErrorCheck(cudaMemset(x_best_d, 0, n * sizeof(ValueType)));
	gpuCudaErrorCheck(cudaDeviceSynchronize());

	// Compute initial error and eps.
	ValueType err, b_norm;
	#if DOUBLE == 1
		gpuCudaErrorCheck((cudaError_t) cublasDnrm2(cublas_handle, m, rk_d, 1, &err));
		gpuCudaErrorCheck((cudaError_t) cublasDnrm2(cublas_handle, m, b_d, 1, &b_norm));
	#else
		gpuCudaErrorCheck((cudaError_t) cublasSnrm2(cublas_handle, m, rk_d, 1, &err));
		gpuCudaErrorCheck((cudaError_t) cublasSnrm2(cublas_handle, m, b_d, 1, &b_norm));
	#endif

	double eps = 1.0e-15 * b_norm;
	double eps_counter = 1.0e-7 * b_norm;
	printf("eps = %g eps_counter = %g\n", eps, eps_counter);

	// =====================================================================
	// Phase 2: Iterative loop — ZERO vector transfers.
	// =====================================================================

	long k = 0;
	long restart_k = 100;
	double spmv_time_accum = 0;
	double err_explicit = err;
	double err_best = err;

	ValueType * y_d = buf_d;  // alias
	ValueType * t_d;          // alias, set per iteration

	while (k < max_iterations)
	{
		// Periodically calculate explicit residual.
		if ((k > 0) && !(k % restart_k))
		{
			MF->spmv_gpu(xk_d, buf_d);
			gpu_axpy(rk_explicit_d, b_d, -1.0, buf_d, m);
			gpuCudaErrorCheck(cudaDeviceSynchronize());
			#if DOUBLE == 1
				gpuCudaErrorCheck((cudaError_t) cublasDnrm2(cublas_handle, m, rk_explicit_d, 1, &err_explicit));
			#else
				gpuCudaErrorCheck((cudaError_t) cublasSnrm2(cublas_handle, m, rk_explicit_d, 1, &err_explicit));
			#endif
			if (err_explicit < err_best)
			{
				gpu_copy(x_best_d, xk_d, n);
				gpuCudaErrorCheck(cudaDeviceSynchronize());
				err_best = err_explicit;
			}
		}

		// err = norm(rk)
		#if DOUBLE == 1
			gpuCudaErrorCheck((cudaError_t) cublasDnrm2(cublas_handle, m, rk_d, 1, &err));
		#else
			gpuCudaErrorCheck((cudaError_t) cublasSnrm2(cublas_handle, m, rk_d, 1, &err));
		#endif

		y_d = buf_d;

		// y = inv(K1) * pk  →  y[i] = pk[i] / K[i]
		gpu_elem_div(y_d, pk_d, K_d, m);
		gpuCudaErrorCheck(cudaDeviceSynchronize());

		// v = A * y
		time_spmv_out += time_it(1,
			MF->spmv_gpu(y_d, v_d);
		);

		// s_a = s_pk_p / (r0_, v)
		ValueType dot_r0_v;
		#if DOUBLE == 1
			gpuCudaErrorCheck((cudaError_t) cublasDdot(cublas_handle, m, r0__d, 1, v_d, 1, &dot_r0_v));
		#else
			gpuCudaErrorCheck((cudaError_t) cublasSdot(cublas_handle, m, r0__d, 1, v_d, 1, &dot_r0_v));
		#endif
		ValueType s_a = g_s_pk_p / dot_r0_v;

		// h = xk + s_a * y
		gpu_axpy(h_d, xk_d, s_a, y_d, m);

		// s = rk - s_a * v
		gpu_axpy(s_d, rk_d, -s_a, v_d, m);

		// z = inv(K1) * s  →  z[i] = s[i] / K[i]
		gpu_elem_div(z_d, s_d, K_d, m);
		gpuCudaErrorCheck(cudaDeviceSynchronize());

		t_d = buf_d;

		// t = A * z
		time_spmv_out += time_it(1,
			MF->spmv_gpu(z_d, t_d);
		);

		// s_w = (inv(K1)*t, inv(K1)*s) / (inv(K1)*t, inv(K1)*t)
		ValueType s_w = gpu_compute_sw(t_d, s_d, K_d, m, partial_num_d, partial_den_d);

		// rk = s - s_w * t
		gpu_axpy(rk_d, s_d, -s_w, t_d, m);

		// xk = h + s_w * z
		gpu_axpy(xk_d, h_d, s_w, z_d, m);

		// s_pk = (r0_, rk)
		ValueType s_pk;
		#if DOUBLE == 1
			gpuCudaErrorCheck((cudaError_t) cublasDdot(cublas_handle, m, r0__d, 1, rk_d, 1, &s_pk));
		#else
			gpuCudaErrorCheck((cudaError_t) cublasSdot(cublas_handle, m, r0__d, 1, rk_d, 1, &s_pk));
		#endif

		// s_b = (s_pk / s_pk_p) * (s_a / s_w)
		ValueType s_b = (s_pk / g_s_pk_p) * (s_a / s_w);

		// pk = rk + s_b * (pk - s_w * v)
		gpu_pk_update(pk_d, rk_d, v_d, s_b, s_w, m);
		gpuCudaErrorCheck(cudaDeviceSynchronize());

		// s_pk_p = s_pk
		g_s_pk_p = s_pk;

		k++;
	}

	// =====================================================================
	// Phase 3: Final explicit residual and transfer solution back.
	// =====================================================================

	MF->spmv_gpu(xk_d, buf_d);
	gpu_axpy(rk_explicit_d, b_d, -1.0, buf_d, m);
	gpuCudaErrorCheck(cudaDeviceSynchronize());
	#if DOUBLE == 1
		gpuCudaErrorCheck((cudaError_t) cublasDnrm2(cublas_handle, m, rk_explicit_d, 1, &err_explicit));
	#else
		gpuCudaErrorCheck((cudaError_t) cublasSnrm2(cublas_handle, m, rk_explicit_d, 1, &err_explicit));
	#endif
	if (err_explicit < err_best)
	{
		gpu_copy(x_best_d, xk_d, n);
		gpuCudaErrorCheck(cudaDeviceSynchronize());
		err_best = err_explicit;
	}

	// Transfer solution D→H (the ONLY vector transfer back).
	gpuCudaErrorCheck(cudaMemcpy(x_res_out, x_best_d, n * sizeof(ValueType), cudaMemcpyDeviceToHost));

	// Cleanup.
	gpuCudaErrorCheck((cudaError_t) cublasDestroy(cublas_handle));
	gpuCudaErrorCheck(cudaFree(b_d));
	gpuCudaErrorCheck(cudaFree(K_d));
	gpuCudaErrorCheck(cudaFree(r0__d));
	gpuCudaErrorCheck(cudaFree(rk_d));
	gpuCudaErrorCheck(cudaFree(rk_explicit_d));
	gpuCudaErrorCheck(cudaFree(pk_d));
	gpuCudaErrorCheck(cudaFree(xk_d));
	gpuCudaErrorCheck(cudaFree(x_best_d));
	gpuCudaErrorCheck(cudaFree(z_d));
	gpuCudaErrorCheck(cudaFree(h_d));
	gpuCudaErrorCheck(cudaFree(s_d));
	gpuCudaErrorCheck(cudaFree(v_d));
	gpuCudaErrorCheck(cudaFree(buf_d));
	gpuCudaErrorCheck(cudaFree(partial_num_d));
	gpuCudaErrorCheck(cudaFree(partial_den_d));

	num_loops_out = k;
}


//==========================================================================================================================================
//= Compute
//==========================================================================================================================================


void
compute(struct CSR_reference_s * csr, struct Matrix_Format * MF,
		ValueType * b, ValueType * x,
		long max_num_loops,
		long print_labels_and_exit)
{
	int num_threads = omp_get_max_threads();
	__attribute__((unused)) double time;
	long buf_n = 10000;
	char buf[buf_n + 1];
	long i, j;
	double J_estimated, W_avg;
	double err;
	ValueType * vec;
	double gflops;

	num_loops_out = 1;

	if (!print_labels_and_exit)
	{
		vec = (typeof(vec)) malloc(csr->n * sizeof(*vec));

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

		// Warm up GPU with a single spmv call.
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
			preconditioned_bicgstab_gpu(MF, csr->ia, csr->ja, csr->a_ref, csr->m, csr->n, csr->nnz, b, x, max_num_loops);
		);

		rapl_read_end(regs, regs_n);

		/*****************************************************************************************/
		J_estimated = 0;
		for (i=0;i<regs_n;i++){
			J_estimated += ((double) regs[i].uj_accum) / 1e6;
		}
		rapl_close(regs, regs_n);
		free(regs);
		W_avg = J_estimated / time;
		/*****************************************************************************************/

		//=============================================================================
		//= Output section.
		//=============================================================================
		// Report SpMV performance (only the 2 core SpMV calls per iteration).
		{
			long num_spmv_calls = 2 * num_loops_out;
			double total_flops = 2.0 * csr->nnz * num_spmv_calls;
			double spmv_gflops = total_flops / (time_spmv_out * 1e9);

			// Working set: matrix (row_ptr + col_idx + vals) + vectors.
			double matrix_mem = ((csr->m + 1) * sizeof(int) + csr->nnz * sizeof(int) + csr->nnz * sizeof(ValueType)) / (1024.0 * 1024.0);
			double vec_mem = csr->m * sizeof(ValueType) / (1024.0 * 1024.0);  // per vector
			double ws_standalone = matrix_mem + 2 * vec_mem;   // matrix + x + y
			double ws_bicg = matrix_mem + 13 * vec_mem;        // matrix + all BiCG vectors on GPU

			printf("SpMV: calls = %ld, time = %lf s, GFLOPs = %lf, WS_standalone = %.2lf MB, WS_bicg = %.2lf MB\n",
				num_spmv_calls, time_spmv_out, spmv_gflops, ws_standalone, ws_bicg);
		}

		// Compute final error on CPU for reporting.
		MF->spmv(x, vec);
		#pragma omp parallel
		{
			long i;
			ValueType total = 0;
			ValueType partial = 0;
			#pragma omp for
			for (i=0;i<csr->n;i++)
			{
				vec[i] = b[i] - vec[i];
			}
			partial = 0;
			#pragma omp for
			for (i=0;i<csr->n;i++)
				partial += vec[i] * vec[i];
			// Simple reduction (not using omp_thread_reduce_global to avoid header issues in .cu).
			#pragma omp atomic
			total += partial;
			#pragma omp barrier
			#pragma omp single
			{
				err = sqrt(total);
			}
		}
		printf("error = %-12.4g\n", err);

	}

	if (print_labels_and_exit)
	{
		i = 0;
		i += snprintf(buf + i, buf_n - i, "%s", "matrix_name");
		i += snprintf(buf + i, buf_n - i, ",%s", "num_threads");
		i += snprintf(buf + i, buf_n - i, ",%s", "csr_m");
		i += snprintf(buf + i, buf_n - i, ",%s", "csr_n");
		i += snprintf(buf + i, buf_n - i, ",%s", "csr_nnz");
		i += snprintf(buf + i, buf_n - i, ",%s", "time");
		i += snprintf(buf + i, buf_n - i, ",%s", "time_spmv");
		i += snprintf(buf + i, buf_n - i, ",%s", "gflops_spmv");
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

	gflops = csr->nnz_matrix / (time_spmv_out / num_loops_out / 2) * 2 * 1e-9;
	printf("GFLOPS = %lf (%s)\n", gflops, getenv("PROGG"));

	i = 0;
	i += snprintf(buf + i, buf_n - i, "%s", csr->matrix_name);
	i += snprintf(buf + i, buf_n - i, ",%d", omp_get_max_threads());
	i += snprintf(buf + i, buf_n - i, ",%lu", csr->m);
	i += snprintf(buf + i, buf_n - i, ",%lu", csr->n);
	i += snprintf(buf + i, buf_n - i, ",%lu", csr->nnz);
	i += snprintf(buf + i, buf_n - i, ",%lf", time);
	i += snprintf(buf + i, buf_n - i, ",%lf", time_spmv_out);
	i += snprintf(buf + i, buf_n - i, ",%lf", gflops);
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


void
bench(struct CSR_reference_s * csr, struct Matrix_Format * MF, long print_labels_and_exit)
{
	ValueType * b;
	ValueType * x;
	double time;
	long i;

	if (print_labels_and_exit == 1)
	{
		compute(NULL, NULL, NULL, NULL, 0, 1);
		return;
	}

	if (csr->m != csr->n)
		error("the matrix must be square");

	b = (typeof(b)) aligned_alloc(64, csr->n * sizeof(*b));
	for (i=0;i<csr->n;i++)
		b[i] = 1;

	x = (typeof(x)) aligned_alloc(64, csr->n * sizeof(*x));

	char * max_num_loops_list = getenv("CG_MAX_NUM_ITERS");
	if (max_num_loops_list == NULL)
		error("max_num_loops_list is empty");
	long max_num_loops;

	char * max_num_loops_str = max_num_loops_list;
	while (*max_num_loops_str != 0)
	{
		max_num_loops = atol(max_num_loops_str);
		#pragma omp parallel for
		for(int i=0;i<csr->n;++i)
			x[i] = 0;
		compute(csr, MF, b, x, max_num_loops, 0);

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
	}

	free(b);
	free(x);
}
