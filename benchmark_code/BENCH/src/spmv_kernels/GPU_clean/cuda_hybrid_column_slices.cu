#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

#include <cuda.h>
#include <cooperative_groups.h>
#include <cuda_pipeline_primitives.h>

#include "macros/cpp_defines.h"

#include "../spmv_kernel.h"


#define ValueTypeStored  ValueType
// #define ValueTypeStored  float


#ifdef __cplusplus
extern "C"{
#endif
	#include "macros/macrolib.h"
	#include "time_it.h"
	#include "parallel_util.h"
	#include "array_metrics.h"
	#include "io.h"

	#include "aux/csr_converter_reference.h"

	#include "aux/csr_util.h"

	#include "cuda/cuda_util.h"

#ifdef __cplusplus
}
#endif

// After all needed includes, else they will be in the namespace only, since they are included only once.
namespace ns_nested_format
{
	// #include "cuda_csr_transpose.cu"
	#include "cuda_csr_transpose_expand_rows.cu"
	// #include "cusparse_csr.cu"

	// #include "../csr.cpp"
}

namespace cg = cooperative_groups;

using namespace cooperative_groups;

#ifndef NNZ_PER_THREAD
	#define NNZ_PER_THREAD  5
#endif

#ifndef BLOCK_SIZE
	// #define BLOCK_SIZE  32
	// #define BLOCK_SIZE  64
	#define BLOCK_SIZE  128
	// #define BLOCK_SIZE  256
	// #define BLOCK_SIZE  512
	// #define BLOCK_SIZE  1024
#endif


void
cuda_push_duplicate_base(void ** dst_ptr, void * src, long bytes)
{
	cudaMalloc(dst_ptr, bytes);
	cudaMemcpy(*((char **) dst_ptr), src, bytes, cudaMemcpyHostToDevice);
}
#define cuda_push_duplicate(dst_ptr, src, bytes) cuda_push_duplicate_base((void **) dst_ptr, src, bytes)


template<typename T>
void
transpose(T * A, INT_T m, INT_T n)
{
	T * buf = (typeof(buf)) aligned_alloc(64, m*n * sizeof(*buf));
	long i, j;
	for (j=0;j<n;j++)
	{
		for (i=0;i<m;i++)
			buf[j*m + i] = A[i*n + j];
	}
	for (i=0;i<m*n;i++)
		A[i] = buf[i];
	free(buf);
}


struct HybridArrays : Matrix_Format
{
	long num_matrices;
	struct Matrix_Format ** MFs;

	long n_slice;

	long persistent_l2_cache;

	INT_T * offsets;
	INT_T * ia_buf;
	INT_T * ja_buf;
	ValueTypeReference * a_buf;

	ValueType * x = NULL;
	ValueType * y = NULL;

	HybridArrays(INT_T * row_ptr, INT_T * ja, ValueTypeReference * a, long m, long n, long nnz) : Matrix_Format(m, n, nnz)
	{
		// double time_balance;
		long i;

		cuda_device_print_attributes();

		long l2_cache_size = cuda_device_get_attribute(cudaDevAttrL2CacheSize, 0);
		long max_persistent_l2_cache = cuda_device_get_attribute(cudaDevAttrMaxPersistingL2CacheSize, 0);

		long x_size = n * sizeof(ValueType);

		/* set-aside 3/4 of L2 cache for persisting accesses or the max allowed */
		persistent_l2_cache = l2_cache_size * 0.75;
		if (persistent_l2_cache > max_persistent_l2_cache)
			persistent_l2_cache = max_persistent_l2_cache;
		if (persistent_l2_cache > x_size)
			persistent_l2_cache = x_size;
		persistent_l2_cache = persistent_l2_cache / sizeof(ValueType) * sizeof(ValueType);

		// cudaDeviceSetLimit(cudaLimitPersistingL2CacheSize, persistent_l2_cache);

		n_slice = persistent_l2_cache / sizeof(ValueType);
		num_matrices = (n + n_slice - 1) / n_slice;
		MFs = (typeof(MFs)) malloc(num_matrices * sizeof(*MFs));
		offsets = (typeof(offsets)) malloc((num_matrices+1) * sizeof(*offsets));
		for (i=0;i<num_matrices+1;i++)
			offsets[i] = 0;

		printf("l2_cache_size=%ld, persistent_l2_cache=%ld, n=%ld, n_slice=%ld, num_matrices=%ld\n", l2_cache_size, persistent_l2_cache, n, n_slice, num_matrices);

		ia_buf = (typeof(ia_buf)) malloc(nnz * sizeof(*ia_buf));
		ja_buf = (typeof(ja_buf)) malloc(nnz * sizeof(*ja_buf));
		a_buf = (typeof(a_buf)) malloc(nnz * sizeof(*a_buf));

		_Pragma("omp parallel")
		{
			long i, j, k, col, sum, pos;
			_Pragma("omp for")
			for (j=0;j<nnz;j++)
			{
				col = ja[j];
				k = col / n_slice;
				__atomic_fetch_add(&offsets[k], 1, __ATOMIC_RELAXED);
			}
			_Pragma("omp single")
			{
				sum = 0;
				for (k=0;k<num_matrices+1;k++)
				{
					sum += offsets[k];
					offsets[k] = sum;
				}
			}
			_Pragma("omp for")
			for (i=0;i<m;i++)
			{
				for (j=row_ptr[i];j<row_ptr[i+1];j++)
				{
					col = ja[j];
					k = col / n_slice;
					pos = __atomic_sub_fetch(&offsets[k], 1, __ATOMIC_RELAXED);
					ia_buf[pos] = i;
					ja_buf[pos] = ja[j];
					a_buf[pos] = a[j];
				}
			}
		}

		INT_T * row_ptr_tmp = (typeof(row_ptr_tmp)) aligned_alloc(64, (m+1) * sizeof(*row_ptr_tmp));
		INT_T * ja_tmp = (typeof(ja_tmp)) aligned_alloc(64, nnz * sizeof(*ja_tmp));
		ValueTypeReference * a_tmp = (typeof(a_tmp)) aligned_alloc(64, nnz * sizeof(*a_tmp));
		// i = 1;
		for (i=0;i<num_matrices;i++)
		{
			long base = offsets[i];
			long nnz_slice = offsets[i+1] - base;
			coo_to_csr(&ia_buf[base], &ja_buf[base], &a_buf[base], m, n, nnz_slice, row_ptr_tmp, ja_tmp, a_tmp, 1, 0);
			MFs[i] = ns_nested_format::csr_to_format(row_ptr_tmp, ja_tmp, a_tmp, m, n, nnz_slice, 0, 0);
			// coo_to_csr(&ia_buf[0], &ja_buf[0], &a_buf[0], m, n, nnz, row_ptr_tmp, ja_tmp, a_tmp, 1, 0);
			// MFs[i] = ns_nested_format::csr_to_format(row_ptr_tmp, ja_tmp, a_tmp, m, n, nnz, 0, 0);
			printf("matrix slice %ld: %ld %ld %ld\n", i, m, n, nnz_slice);
			printf("matrix slice format %ld: %ld %ld %ld size=%g\n", i, MFs[i]->m, MFs[i]->n, MFs[i]->nnz, MFs[i]->mem_footprint);

			__attribute__((unused)) long text_n = 1000;
			__attribute__((unused)) char text[text_n];

			// __attribute__((unused)) long enable_legend = 1;
			// __attribute__((unused)) long num_pixels_x = 1080;
			// __attribute__((unused)) long num_pixels_y = 1080;
			// snprintf(text, text_n, "matrix_%ld", i);
			// csr_plot(text, row_ptr_tmp, ja_tmp, a_tmp, m, n, nnz_slice, enable_legend, num_pixels_x, num_pixels_y);

			// long num_chars;
			// int fd = safe_open("slice.mtx", O_CREAT|O_WRONLY|O_TRUNC);
			// num_chars = snprintf(text, text_n, "%%%%MatrixMarket matrix coordinate real general\n");
			// safe_write(fd, text, num_chars);
			// num_chars = snprintf(text, text_n, "%ld %ld %ld\n", m, n, nnz_slice);
			// safe_write(fd, text, num_chars);
			// for (long j=0;j<nnz_slice;j++)
			// {
				// num_chars = snprintf(text, text_n, "%d %d %g\n", ia_buf[base+j], ja_buf[base+j], a_buf[base+j]);
				// safe_write(fd, text, num_chars);
			// }
			// safe_close(fd);

		}
		free(row_ptr_tmp);
		free(ja_tmp);
		free(a_tmp);

	}

	~HybridArrays()
	{
	}

	void spmv(ValueType * x, ValueType * y);
	void statistics_start();
	int statistics_print_data(__attribute__((unused)) char * buf, __attribute__((unused)) long buf_n);
};


void
HybridArrays::spmv(ValueType * x, ValueType * y)
{
	long i;
	cudaStream_t stream = 0;   // Default stream, kernels use this stream implicitly.
	cudaStreamAttrValue attr = {};
	attr.accessPolicyWindow.hitRatio  = 1.0;
	attr.accessPolicyWindow.hitProp   = cudaAccessPropertyPersisting;
	attr.accessPolicyWindow.missProp  = cudaAccessPropertyStreaming;

	// i = 1;
	for (i=0;i<num_matrices;i++)
	{

		attr.accessPolicyWindow.base_ptr  = (void *) (&x[i*n_slice]);
		attr.accessPolicyWindow.num_bytes = n_slice * sizeof(ValueType);
		cudaStreamSetAttribute(stream, cudaStreamAttributeAccessPolicyWindow, &attr);

		MFs[i]->spmv(x, y);
		// printf("i=%ld\n", i);
	}
}


struct Matrix_Format *
csr_to_format(INT_T * row_ptr, INT_T * col_ind, ValueTypeReference * values, long m, long n, long nnz, long symmetric, long symmetry_expanded)
{
	if (symmetric && !symmetry_expanded)
		error("symmetric matrices not supported by this format, expand symmetry");
	struct HybridArrays * csr = new HybridArrays(row_ptr, col_ind, values, m, n, nnz);
	// for (long i=0;i<10;i++)
		// printf("%d\n", row_ptr[i]);
	csr->mem_footprint = csr->nnz * (sizeof(ValueTypeStored) + sizeof(INT_T)) + (m+1) * sizeof(INT_T);
	char *format_name;
	format_name = (char *)malloc(100*sizeof(char));
	snprintf(format_name, 100, "Custom_CSR_CUDA_transpose_expand_rows_b%d_nnz%d", BLOCK_SIZE, NNZ_PER_THREAD);
	csr->format_name = format_name;
	return csr;
}


//==========================================================================================================================================
//= Print Statistics
//==========================================================================================================================================


void
HybridArrays::statistics_start()
{
}


int
statistics_print_labels(__attribute__((unused)) char * buf, __attribute__((unused)) long buf_n)
{
	return 0;
}


int
HybridArrays::statistics_print_data(__attribute__((unused)) char * buf, __attribute__((unused)) long buf_n)
{
	return 0;
}

