#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

#include <cuda.h>
#include <cusparse.h>

#include "macros/cpp_defines.h"

#include "../spmv_kernel.h"

#ifdef __cplusplus
extern "C"{
#endif
	#include "macros/macrolib.h"
	#include "time_it.h"
	#include "parallel_util.h"
	#include "array_metrics.h"

	#include "cuda/cuda_util.h"
	#include "cuda/cusparse_util.h"
#ifdef __cplusplus
}
#endif


#if DOUBLE == 0
	#define ValueTypeCuda  CUDA_R_32F
#elif DOUBLE == 1
	#define ValueTypeCuda  CUDA_R_64F
#endif

#ifndef DETAILED_TIMING
	// #define DETAILED_TIMING 1
	#define DETAILED_TIMING 0
#endif

// Set this to 1 to enable the standalone GPU optimization (D2D full transfer of y to x)
// Set this to 0 to use the standard method (full H2D transfer of x every iteration)
#define STANDALONE_ITERATIVE_OPTIMIZATION 1
// #define STANDALONE_ITERATIVE_OPTIMIZATION 0

// Set this to 1 to enable the iterative optimization (partial H2D + D2D transfer)
// Set this to 0 to use the standard method (full H2D transfer every iteration)
#define HYBRID_ITERATIVE_OPTIMIZATION 1
// #define HYBRID_ITERATIVE_OPTIMIZATION 0


struct CuSPARSE_CSR_Arrays : Matrix_Format
{
	// --- Hybrid awareness ---
	long m_cpu = -1;    // number of CPU rows (-1 in standalone mode)
	long offset;        // row offset into x/y for the GPU partition

	bool is_first_iteration = true;
	bool is_last_iteration  = false;

	// --- Sparse matrix data (host originals) ---
	INT_T * ia;      // the usual rowptr (of size m+1)
	INT_T * ja;      // the colidx of each NNZ (of size nnz)
	ValueType * a;   // the values (of size NNZ)

	// --- Device copies of the sparse matrix ---
	INT_T * ia_d;
	INT_T * ja_d;
	ValueType * a_d;

	// --- Pinned host staging buffers ---
	INT_T * ia_h;
	INT_T * ja_h;
	ValueType * a_h;

	// --- cuSPARSE handles and descriptors ---
	cusparseHandle_t     handle = NULL;
	cusparseSpMatDescr_t matA;
	void*                dBuffer    = NULL;
	size_t               bufferSize = 0;

	// --- x/y host pointers (set on first call) ---
	ValueType * x = NULL;
	ValueType * y = NULL;

	// --- Dense vector descriptors ---
	cusparseDnVecDescr_t vecX;
	cusparseDnVecDescr_t vecY;

	// --- Device x/y buffers (EXPLICIT memory mode only) ---
	#ifdef VECTOR_ALLOC_EXPLICIT
		ValueType * x_d = NULL;
		ValueType * y_d = NULL;
	#endif

	long max_mn = -1;

	// --- CUDA streams ---
	cudaStream_t stream;
	#ifdef VECTOR_ALLOC_EXPLICIT
		cudaStream_t h2d_stream;
	#endif

	// --- CUDA timing events ---
	cudaEvent_t start_event, stop_event;
	cudaEvent_t memset_event;
	#ifdef VECTOR_ALLOC_EXPLICIT
		cudaEvent_t h2d_event, kernel_event, d2h_event, h2d_done_event;
	#endif
	float last_duration_ms = 0;

	// --- Timing accumulators (used when DETAILED_TIMING=1) ---
	double time_h2d_ms = 0;
	double time_memset_ms = 0;
	double time_kernel_ms = 0;
	double time_d2h_ms = 0;
	double time_pure_memset_ms = 0;
	long call_count = 0;


	CuSPARSE_CSR_Arrays(INT_T * ia, INT_T * ja, ValueType * a, long m, long n, long nnz, long m_cpu = -1): Matrix_Format(m, n, nnz), m_cpu(m_cpu), ia(ia), ja(ja), a(a)
	{
		int max_smem_per_block, multiproc_count, max_threads_per_block, warp_size, max_threads_per_multiproc;
		gpuCudaErrorCheck(cudaDeviceGetAttribute(&max_smem_per_block, cudaDevAttrMaxSharedMemoryPerBlock, 0));
		gpuCudaErrorCheck(cudaDeviceGetAttribute(&multiproc_count, cudaDevAttrMultiProcessorCount, 0));
		gpuCudaErrorCheck(cudaDeviceGetAttribute(&max_threads_per_block, cudaDevAttrMaxThreadsPerBlock, 0));
		gpuCudaErrorCheck(cudaDeviceGetAttribute(&warp_size, cudaDevAttrWarpSize, 0));
		gpuCudaErrorCheck(cudaDeviceGetAttribute(&max_threads_per_multiproc, cudaDevAttrMaxThreadsPerMultiProcessor, 0));
		printf("max_smem_per_block=%d\n", max_smem_per_block);
		printf("multiproc_count=%d\n", multiproc_count);
		printf("max_threads_per_block=%d\n", max_threads_per_block);
		printf("warp_size=%d\n", warp_size);
		printf("max_threads_per_multiproc=%d\n", max_threads_per_multiproc);

		// --- Sparse matrix device allocations ---
		gpuCudaErrorCheck(cudaMalloc(&ia_d, (m+1) * sizeof(*ia_d)));
		gpuCudaErrorCheck(cudaMalloc(&ja_d, nnz   * sizeof(*ja_d)));
		gpuCudaErrorCheck(cudaMalloc(&a_d,  nnz   * sizeof(*a_d)));

		// --- Pinned staging buffers (for fast H2D) ---
		gpuCudaErrorCheck(cudaMallocHost(&ia_h, (m+1) * sizeof(*ia_h)));
		gpuCudaErrorCheck(cudaMallocHost(&ja_h, nnz   * sizeof(*ja_h)));
		gpuCudaErrorCheck(cudaMallocHost(&a_h,  nnz   * sizeof(*a_h)));

		memcpy(ia_h, ia, (m+1) * sizeof(*ia_h));
		memcpy(ja_h, ja, nnz   * sizeof(*ja_h));
		memcpy(a_h,  a,  nnz   * sizeof(*a_h));

		gpuCudaErrorCheck(cudaMemcpy(ia_d, ia_h, (m+1) * sizeof(*ia_d), cudaMemcpyHostToDevice));
		gpuCudaErrorCheck(cudaMemcpy(ja_d, ja_h, nnz   * sizeof(*ja_d), cudaMemcpyHostToDevice));
		gpuCudaErrorCheck(cudaMemcpy(a_d,  a_h,  nnz   * sizeof(*a_d),  cudaMemcpyHostToDevice));

		gpuCusparseErrorCheck(cusparseCreateCsr(
			&matA, m, n, nnz,
			ia_d, ja_d, a_d,
			CUSPARSE_INDEX_32I, CUSPARSE_INDEX_32I,
			CUSPARSE_INDEX_BASE_ZERO, ValueTypeCuda));

		// Hybrid mode: GPU handles rows [m_cpu, m_cpu+m); offset into the shared y vector.
		// Standalone mode: GPU handles all rows; offset = 0.
		offset = (m_cpu != -1) ? m_cpu : 0;
		long total_m = (m_cpu != -1) ? (m + m_cpu) : m;
		max_mn = (total_m > n) ? total_m : n;

		// --- CUDA streams ---
		gpuCudaErrorCheck(cudaStreamCreate(&stream));
		#ifdef VECTOR_ALLOC_EXPLICIT
			gpuCudaErrorCheck(cudaStreamCreate(&h2d_stream));
		#endif

		// --- CUDA events ---
			gpuCudaErrorCheck(cudaEventCreate(&start_event));
			gpuCudaErrorCheck(cudaEventCreate(&stop_event));
			gpuCudaErrorCheck(cudaEventCreate(&memset_event));
		#ifdef VECTOR_ALLOC_EXPLICIT
			gpuCudaErrorCheck(cudaEventCreate(&h2d_event));
			gpuCudaErrorCheck(cudaEventCreate(&kernel_event));
			gpuCudaErrorCheck(cudaEventCreate(&d2h_event));
			gpuCudaErrorCheck(cudaEventCreate(&h2d_done_event));
			gpuCudaErrorCheck(cudaEventRecord(h2d_done_event, h2d_stream));
		#endif

		// --- Sparse matrix device allocations ---
		gpuCudaErrorCheck(cudaMalloc(&ia_d, (m+1) * sizeof(*ia_d)));
		gpuCudaErrorCheck(cudaMalloc(&ja_d, nnz   * sizeof(*ja_d)));
		gpuCudaErrorCheck(cudaMalloc(&a_d,  nnz   * sizeof(*a_d)));

		// --- Pinned staging buffers ---
		gpuCudaErrorCheck(cudaMallocHost(&ia_h, (m+1) * sizeof(*ia_h)));
		gpuCudaErrorCheck(cudaMallocHost(&ja_h, nnz   * sizeof(*ja_h)));
		gpuCudaErrorCheck(cudaMallocHost(&a_h,  nnz   * sizeof(*a_h)));

		memcpy(ia_h, ia, (m+1) * sizeof(*ia_h));
		memcpy(ja_h, ja, nnz   * sizeof(*ja_h));
		memcpy(a_h,  a,  nnz   * sizeof(*a_h));

		gpuCudaErrorCheck(cudaMemcpy(ia_d, ia_h, (m+1) * sizeof(*ia_d), cudaMemcpyHostToDevice));
		gpuCudaErrorCheck(cudaMemcpy(ja_d, ja_h, nnz   * sizeof(*ja_d), cudaMemcpyHostToDevice));
		gpuCudaErrorCheck(cudaMemcpy(a_d,  a_h,  nnz   * sizeof(*a_d),  cudaMemcpyHostToDevice));

		// --- cuSPARSE handle (bound to main stream) ---
		gpuCusparseErrorCheck(cusparseCreate(&handle));
		gpuCusparseErrorCheck(cusparseSetStream(handle, stream));

		// --- cuSPARSE sparse matrix descriptor ---
		gpuCusparseErrorCheck(cusparseCreateCsr(
			&matA, m, n, nnz,
			ia_d, ja_d, a_d,
			CUSPARSE_INDEX_32I, CUSPARSE_INDEX_32I,
			CUSPARSE_INDEX_BASE_ZERO, ValueTypeCuda));

		// --- EXPLICIT mode: allocate device x/y buffers and create descriptors ---
		#ifdef VECTOR_ALLOC_EXPLICIT
			gpuCudaErrorCheck(cudaMalloc(&x_d, max_mn * sizeof(*x_d)));
			gpuCudaErrorCheck(cudaMalloc(&y_d, max_mn * sizeof(*y_d)));
			gpuCusparseErrorCheck(cusparseCreateDnVec(&vecX, n,       x_d,          ValueTypeCuda));
			gpuCusparseErrorCheck(cusparseCreateDnVec(&vecY, m, y_d + offset,       ValueTypeCuda));
		#endif
		// Non-EXPLICIT: vecX/vecY created on first compute_csr() call with real host pointers.
	}

	~CuSPARSE_CSR_Arrays()
	{
		free(a);
		free(ia);
		free(ja);

		gpuCusparseErrorCheck(cusparseDestroySpMat(matA));
		if (x != NULL) gpuCusparseErrorCheck(cusparseDestroyDnVec(vecX));
		if (y != NULL) gpuCusparseErrorCheck(cusparseDestroyDnVec(vecY));
		gpuCusparseErrorCheck(cusparseDestroy(handle));

		gpuCudaErrorCheck(cudaFree(ia_d));
		gpuCudaErrorCheck(cudaFree(ja_d));
		gpuCudaErrorCheck(cudaFree(a_d));
		#ifdef VECTOR_ALLOC_EXPLICIT
			gpuCudaErrorCheck(cudaFree(x_d));
			gpuCudaErrorCheck(cudaFree(y_d));
		#endif
		gpuCudaErrorCheck(cudaFree(dBuffer));

		gpuCudaErrorCheck(cudaFreeHost(ia_h));
		gpuCudaErrorCheck(cudaFreeHost(ja_h));
		gpuCudaErrorCheck(cudaFreeHost(a_h));

		gpuCudaErrorCheck(cudaStreamDestroy(stream));
		#ifdef VECTOR_ALLOC_EXPLICIT
			gpuCudaErrorCheck(cudaStreamDestroy(h2d_stream));
		#endif
		gpuCudaErrorCheck(cudaEventDestroy(start_event));
		gpuCudaErrorCheck(cudaEventDestroy(stop_event));
		gpuCudaErrorCheck(cudaEventDestroy(memset_event));
		#ifdef VECTOR_ALLOC_EXPLICIT
			gpuCudaErrorCheck(cudaEventDestroy(h2d_event));
			gpuCudaErrorCheck(cudaEventDestroy(kernel_event));
			gpuCudaErrorCheck(cudaEventDestroy(d2h_event));
			gpuCudaErrorCheck(cudaEventDestroy(h2d_done_event));
		#endif
	}

	void spmv(ValueType * x, ValueType * y) override;

	void synchronize() override {
		gpuCudaErrorCheck(cudaStreamSynchronize(stream));
		#ifdef VECTOR_ALLOC_EXPLICIT
			gpuCudaErrorCheck(cudaStreamSynchronize(h2d_stream));
		#endif
		gpuCudaErrorCheck(cudaEventElapsedTime(&last_duration_ms, start_event, stop_event));

		#if DETAILED_TIMING
			#ifdef VECTOR_ALLOC_EXPLICIT
				float h2d_ms = 0, memset_ms = 0, kernel_ms = 0, d2h_ms = 0, pure_memset = 0;
				if (last_duration_ms > 0) {
					gpuCudaErrorCheck(cudaEventElapsedTime(&h2d_ms, start_event, h2d_event));
					gpuCudaErrorCheck(cudaEventElapsedTime(&memset_ms, h2d_event, memset_event));
					gpuCudaErrorCheck(cudaEventElapsedTime(&kernel_ms, memset_event, kernel_event));
					gpuCudaErrorCheck(cudaEventElapsedTime(&d2h_ms, kernel_event, d2h_event));
				}
				time_h2d_ms += h2d_ms;
				time_memset_ms += memset_ms;
				time_kernel_ms += kernel_ms;
				time_d2h_ms += d2h_ms;
				time_pure_memset_ms += pure_memset;
			#else
				float memset_ms = 0, kernel_ms = 0;
				if (last_duration_ms > 0) {
					gpuCudaErrorCheck(cudaEventElapsedTime(&memset_ms, start_event, memset_event));
					gpuCudaErrorCheck(cudaEventElapsedTime(&kernel_ms, memset_event, stop_event));
				}
				time_memset_ms += memset_ms;
				time_kernel_ms += kernel_ms;
			#endif
			call_count++;
		#endif
	}

	double get_last_duration() override { return (double)last_duration_ms; }
	void set_last_iteration(bool is_last) override { is_last_iteration = is_last; }
	void statistics_start() override;
	int  statistics_print_data(__attribute__((unused)) char * buf, __attribute__((unused)) long buf_n) override;

	void issue_h2d_for_next_iteration(ValueType * y) override {
		#ifdef VECTOR_ALLOC_EXPLICIT
			if (m_cpu == -1) return;

			#if HYBRID_ITERATIVE_OPTIMIZATION
				long h2d_copy_elements = (m_cpu < n) ? m_cpu : n;
				if (h2d_copy_elements > 0) {
					// Wait for kernel to finish reading x_d before we overwrite it
					gpuCudaErrorCheck(cudaStreamWaitEvent(h2d_stream, kernel_event, 0));
					// Proactively pipeline CPU result y[0..m_cpu-1] → x_d for next iteration
					gpuCudaErrorCheck(cudaMemcpyAsync(x_d, y, h2d_copy_elements * sizeof(*x_d), cudaMemcpyHostToDevice, h2d_stream));
					// Signal that next iteration can use x_d
					gpuCudaErrorCheck(cudaEventRecord(h2d_done_event, h2d_stream));
				}
			#endif
		#endif
	}
};


void compute_csr(CuSPARSE_CSR_Arrays * restrict csr, ValueType * restrict x, ValueType * restrict y);

void
CuSPARSE_CSR_Arrays::spmv(ValueType * x, ValueType * y)
{
	compute_csr(this, x, y);
}


struct Matrix_Format *
cusparse_csr_to_format(INT_T * row_ptr, INT_T * col_ind, ValueTypeReference * values,
                       long m, long n, long nnz,
                       long symmetric, long symmetry_expanded, long m_cpu)
{
	if (symmetric && !symmetry_expanded)
		error("symmetric matrices not supported by this format, expand symmetry");
	struct CuSPARSE_CSR_Arrays * csr = new CuSPARSE_CSR_Arrays(row_ptr, col_ind, values, m, n, nnz, m_cpu);
	csr->mem_footprint = nnz * (sizeof(ValueType) + sizeof(INT_T)) + (m+1) * sizeof(INT_T);
	csr->format_name = (char *) "CUSPARSE_CSR";
	return csr;
}


//==========================================================================================================================================
//= cuSPARSE SpMV compute
//==========================================================================================================================================


void
compute_csr(CuSPARSE_CSR_Arrays * restrict csr, ValueType * restrict x, ValueType * restrict y)
{
	gpuCudaErrorCheck(cudaEventRecord(csr->start_event, csr->stream));

	const double alpha = 1.0;
	const double beta  = 0.0;

	#ifdef VECTOR_ALLOC_EXPLICIT
		// ===================================================================
		// EXPLICIT MODE: device buffers x_d / y_d; explicit H2D + D2H.
		// ===================================================================

		if (csr->m_cpu == -1) // --- STANDALONE MODE ---
		{
			#if STANDALONE_ITERATIVE_OPTIMIZATION
				if (csr->is_first_iteration) {
					gpuCudaErrorCheck(cudaMemcpyAsync(csr->x_d, x, csr->n * sizeof(*csr->x_d), cudaMemcpyHostToDevice, csr->stream));
				}
				// else {
				// // Subsequent iterations: Do nothing for H2D. x_d is populated via D2D from the previous iteration.
				// }
			#else
				// Old method: Always transfer full x from Host
				gpuCudaErrorCheck(cudaMemcpyAsync(csr->x_d, x, csr->n * sizeof(*csr->x_d), cudaMemcpyHostToDevice, csr->stream));
			#endif
		}
		else // --- HYBRID MODE ---
		{
			#if HYBRID_ITERATIVE_OPTIMIZATION
				if (csr->is_first_iteration) {
					// First iteration must bootstrap logically
					gpuCudaErrorCheck(cudaMemcpyAsync(csr->x_d, x, csr->n * sizeof(*csr->x_d), cudaMemcpyHostToDevice, csr->stream));
				} else {
					// Successive iterations evaluate off proactive dispatch linked inside h2d_done_event 
					gpuCudaErrorCheck(cudaStreamWaitEvent(csr->stream, csr->h2d_done_event, 0));
				}
			#else
				// Standard method: transfer full x every time
				gpuCudaErrorCheck(cudaMemcpyAsync(csr->x_d, x, csr->n * sizeof(*csr->x_d), cudaMemcpyHostToDevice, csr->stream));
			#endif
		}

		#if DETAILED_TIMING
			gpuCudaErrorCheck(cudaEventRecord(csr->h2d_event, csr->stream));
		#endif

		if (csr->is_first_iteration)
		{
			printf("cuSPARSE CSR SpMV — EXPLICIT mode, offset=%ld, m=%ld, n=%ld\n", csr->offset, csr->m, csr->n);
			if (csr->m_cpu == -1)
				printf("Standalone configuration (Optimization: %s)\n", STANDALONE_ITERATIVE_OPTIMIZATION ? "ON" : "OFF");
			else
				printf("Hybrid configuration: m_cpu is %ld (Optimization: %s)\n", csr->m_cpu, HYBRID_ITERATIVE_OPTIMIZATION ? "ON" : "OFF");
			csr->is_first_iteration = false;
			csr->x = x;

			// Allocate workspace (needs matA, vecX, vecY to be set up already)
			gpuCusparseErrorCheck(cusparseSpMV_bufferSize(csr->handle, CUSPARSE_OPERATION_NON_TRANSPOSE, &alpha, csr->matA, csr->vecX, &beta, csr->vecY, ValueTypeCuda, CUSPARSE_SPMV_ALG_DEFAULT, &csr->bufferSize));
			gpuCudaErrorCheck(cudaMalloc(&csr->dBuffer, csr->bufferSize));
			printf("SpMV_bufferSize = %lu bytes\n", csr->bufferSize);

			gpuCusparseErrorCheck(cusparseSpMV_preprocess(csr->handle, CUSPARSE_OPERATION_NON_TRANSPOSE,&alpha, csr->matA, csr->vecX, &beta, csr->vecY, ValueTypeCuda, CUSPARSE_SPMV_ALG_DEFAULT, csr->dBuffer));
		}
		else {
			// Point descriptors at current device buffers.
			gpuCusparseErrorCheck(cusparseDnVecSetValues(csr->vecX, csr->x_d));
			gpuCusparseErrorCheck(cusparseDnVecSetValues(csr->vecY, csr->y_d + csr->offset));
		}

		// Different than custom csr implementation, there is no need for memsetasync here (it is done by cusparseDnVecSetValues internally)

		#if DETAILED_TIMING
			// For accurate pure-kernel profiling, we isolate "pre-kernel synchronization" (Memset Wait) here
			gpuCudaErrorCheck(cudaEventRecord(csr->memset_event, csr->stream));
		#endif

		// Launch cuSPARSE SpMV.
		gpuCusparseErrorCheck(cusparseSpMV(csr->handle, CUSPARSE_OPERATION_NON_TRANSPOSE, &alpha, csr->matA, csr->vecX, &beta, csr->vecY, ValueTypeCuda, CUSPARSE_SPMV_ALG_DEFAULT, csr->dBuffer));

		gpuCudaErrorCheck(cudaEventRecord(csr->kernel_event, csr->stream));

		gpuCudaErrorCheck(cudaPeekAtLastError());
		// gpuCudaErrorCheck(cudaDeviceSynchronize()); // Removed for async overlap

		if (csr->y == NULL)
			csr->y = y;

		bool skip_d2h = false;
		if (csr->m_cpu == -1) {
			#if STANDALONE_ITERATIVE_OPTIMIZATION
				skip_d2h = !csr->is_last_iteration;
			#endif
		}

		if (!skip_d2h) {
			// D2H: copy GPU result back to the host y vector.
			gpuCudaErrorCheck(cudaMemcpyAsync(y + csr->offset, csr->y_d + csr->offset, csr->m * sizeof(*csr->y_d), cudaMemcpyDeviceToHost, csr->stream));
		}

		#if DETAILED_TIMING
			gpuCudaErrorCheck(cudaEventRecord(csr->d2h_event, csr->stream));
		#endif

		if (!csr->is_last_iteration) {
			// Zero-cost Ping-Pong Pointer Swap handles DtD perfectly without executing a transfer!
			ValueType * tmp = csr->x_d;
			csr->x_d = csr->y_d;
			csr->y_d = tmp;
		}

	#else
		// ===================================================================
		// NON-EXPLICIT MODE (MALLOC / MANAGED / MALLOCHOST):
		// GPU accesses host pointers directly via ATS / HMM / unified memory.
		// ===================================================================

		if (csr->is_first_iteration)
		{
			printf("cuSPARSE CSR SpMV — non-explicit memory, offset=%ld, m=%ld, n=%ld\n", csr->offset, csr->m, csr->n);
			if (csr->m_cpu == -1)
				printf("Standalone configuration (non-explicit memory)\n");
			else
				printf("Hybrid configuration: m_cpu is %ld (%.2lf MB) (non-explicit memory)\n", csr->m_cpu, csr->m_cpu * 1.0 * sizeof(ValueType) / (1024*1024));
			csr->is_first_iteration = false;
			csr->x = x;

			// Create dense vector descriptors pointing at host pointers.
			// These will be updated via SetValues on each subsequent call.
			gpuCusparseErrorCheck(cusparseCreateDnVec(&csr->vecX, csr->n, x,              ValueTypeCuda));
			gpuCusparseErrorCheck(cusparseCreateDnVec(&csr->vecY, csr->m, y + csr->offset, ValueTypeCuda));

			// Allocate workspace.
			gpuCusparseErrorCheck(cusparseSpMV_bufferSize(csr->handle, CUSPARSE_OPERATION_NON_TRANSPOSE, &alpha, csr->matA, csr->vecX, &beta, csr->vecY, ValueTypeCuda, CUSPARSE_SPMV_ALG_DEFAULT, &csr->bufferSize));
			gpuCudaErrorCheck(cudaMalloc(&csr->dBuffer, csr->bufferSize));
			printf("SpMV_bufferSize = %lu bytes\n", csr->bufferSize);

			gpuCusparseErrorCheck(cusparseSpMV_preprocess(csr->handle, CUSPARSE_OPERATION_NON_TRANSPOSE, &alpha, csr->matA, csr->vecX, &beta, csr->vecY, ValueTypeCuda, CUSPARSE_SPMV_ALG_DEFAULT, csr->dBuffer));
		}
		else
		{
			// Update vector descriptors in case x/y pointers changed (pointer swap).
			gpuCusparseErrorCheck(cusparseDnVecSetValues(csr->vecX, x));
			gpuCusparseErrorCheck(cusparseDnVecSetValues(csr->vecY, y + csr->offset));
		}

		// Update: This is not needed anymore, it is already done by cusparseDnVecSetValues above!
		// // Zero the GPU portion of y directly on the host/unified buffer.
		// gpuCudaErrorCheck(cudaMemsetAsync(y + csr->offset, 0, csr->m * sizeof(*y), csr->stream));

		#if DETAILED_TIMING
			gpuCudaErrorCheck(cudaEventRecord(csr->memset_event, csr->stream));
		#endif

		// Launch cuSPARSE SpMV with host/unified pointers (zero-copy / ATS).
		gpuCusparseErrorCheck(cusparseSpMV(csr->handle, CUSPARSE_OPERATION_NON_TRANSPOSE, &alpha, csr->matA, csr->vecX, &beta, csr->vecY, ValueTypeCuda, CUSPARSE_SPMV_ALG_DEFAULT, csr->dBuffer));

		gpuCudaErrorCheck(cudaPeekAtLastError());
		// gpuCudaErrorCheck(cudaDeviceSynchronize()); // Removed for async overlap

		if (csr->y == NULL)
			csr->y = y;

	#endif

	gpuCudaErrorCheck(cudaEventRecord(csr->stop_event, csr->stream));
}


//==========================================================================================================================================
//= Print Statistics
//==========================================================================================================================================


void
CuSPARSE_CSR_Arrays::statistics_start()
{
	time_h2d_ms = 0;
	time_memset_ms = 0;
	time_kernel_ms = 0;
	time_d2h_ms = 0;
	time_pure_memset_ms = 0;
	call_count = 0;
	is_first_iteration = true;
	is_last_iteration  = false;
}


int
cusparse_csr_statistics_print_labels(__attribute__((unused)) char * buf, __attribute__((unused)) long buf_n)
{
	return 0;
}


int
CuSPARSE_CSR_Arrays::statistics_print_data(__attribute__((unused)) char * buf, __attribute__((unused)) long buf_n)
{
	#if DETAILED_TIMING
		double total_h2d_mb = 0, total_d2h_mb = 0;
		long actual_h2d_calls = 0, actual_d2h_calls = 0;

		if (m_cpu == -1) // --- STANDALONE MODE ---
		{ 
			#if STANDALONE_ITERATIVE_OPTIMIZATION
				total_h2d_mb = (n * sizeof(ValueType) / 1e6);
				actual_h2d_calls = 1;
				total_d2h_mb = (m * sizeof(ValueType) / 1e6);
				actual_d2h_calls = 1;
			#else
				total_h2d_mb = call_count * (n * sizeof(ValueType) / 1e6);
				actual_h2d_calls = call_count;
				total_d2h_mb = call_count * (m * sizeof(ValueType) / 1e6);
				actual_d2h_calls = call_count;
			#endif
		} 
		else // --- HYBRID MODE ---
		{
			#if HYBRID_ITERATIVE_OPTIMIZATION
				total_h2d_mb = (n + (call_count > 1 ? (call_count - 1) : 0) * m_cpu) * sizeof(ValueType) / 1e6;
				actual_h2d_calls = call_count;
			#else
				total_h2d_mb = call_count * (n * sizeof(ValueType) / 1e6);
				actual_h2d_calls = call_count;
			#endif
			total_d2h_mb = call_count * (m * sizeof(ValueType) / 1e6);
			actual_d2h_calls = call_count;
		}

		double avg_h2d = (actual_h2d_calls > 0) ? (time_h2d_ms / actual_h2d_calls) : 0;
		double avg_d2h = (actual_d2h_calls > 0) ? (time_d2h_ms / actual_d2h_calls) : 0;

		double avg_memset = (call_count > 0) ? (time_memset_ms / call_count) : 0;
		double avg_pure_memset = (call_count > 0) ? (time_pure_memset_ms / call_count) : 0;
		double avg_kernel = (call_count > 0) ? (time_kernel_ms / call_count) : 0;

		double h2d_bw = (time_h2d_ms > 0) ? (total_h2d_mb / time_h2d_ms) : 0;
		double d2h_bw = (time_d2h_ms > 0) ? (total_d2h_mb / time_d2h_ms) : 0;

		double moved_h2d_mb_avg = (actual_h2d_calls > 0) ? (total_h2d_mb / actual_h2d_calls) : 0;
		double moved_d2h_mb_avg = (actual_d2h_calls > 0) ? (total_d2h_mb / actual_d2h_calls) : 0;
		
		printf("h2d: %.4lf ms (%.2lf MB - %.2lf GB/s), sync/memset: %.4lf ms (pure memset: %.4lf), kernel: %.4lf ms, d2h: %.4lf ms (%.2lf MB - %.2lf GB/s)\n", 
		       avg_h2d, moved_h2d_mb_avg, h2d_bw, avg_memset, avg_pure_memset, avg_kernel, avg_d2h, moved_d2h_mb_avg, d2h_bw);
		double total_time = time_h2d_ms + time_memset_ms + time_kernel_ms + time_d2h_ms;
		printf("Timing summary percentages: h2d: %.2lf%%, memset: %.2lf%%, kernel: %.2lf%%, d2h: %.2lf%%\n", 
		       (time_h2d_ms / total_time) * 100, (time_memset_ms / total_time) * 100, (time_kernel_ms / total_time) * 100, (time_d2h_ms / total_time) * 100);
	#endif
	return 0;
}


// ==========================================================================
// Standalone entry points (only compiled when NOT in hybrid mode).
// The hybrid dispatcher calls cusparse_csr_to_format() directly.
// ==========================================================================
#ifndef HYBRID
struct Matrix_Format *
csr_to_format(INT_T * row_ptr, INT_T * col_ind, ValueTypeReference * values,
              long m, long n, long nnz, long symmetric, long symmetry_expanded)
{
	return cusparse_csr_to_format(row_ptr, col_ind, values, m, n, nnz,
	                              symmetric, symmetry_expanded, -1);
}

int
statistics_print_labels(char * buf, long buf_n)
{
	return cusparse_csr_statistics_print_labels(buf, buf_n);
}
#endif
