#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

#include <iostream>
#include <fstream>
#include <string.h>
#include <string>
#include <algorithm>
#include <unordered_map>
#include <immintrin.h>

#include "macros/cpp_defines.h"

#include "spmv_bench_common.h"
#include "spmv_kernel.h"

#ifdef __cplusplus
extern "C"{
#endif
	#include "macros/macrolib.h"
	#include "time_it.h"
	#include "parallel_util.h"
#ifdef __cplusplus
}
#endif


#define floatType  ValueType


__m512d _m512zero = _mm512_setzero_pd();
__m128i _m128zero = _mm_setzero_si128();
__m256i _m256zero = _mm256_setzero_si256();
__m128i _m128cmp = _mm_set_epi16(7, 6, 5, 4, 3, 2, 1, 0);
__mmask16 _mmask_load = 0x00ff;


// custom multi-thread parallel reduction function for unordered map
#pragma omp declare reduction(merge : std::unordered_map <floatType, int> : omp_out.insert(omp_in.begin(), omp_in.end()))

void
csr2csrv(const floatType *h_vals, const int &nItems, void *csv_vals_ptr, floatType *csv_vals, int &numVals, int &val_type)
{
	std::unordered_map<floatType, int> value_hashmap;

	int i;
	#pragma omp parallel reduction(merge : value_hashmap)
	{
		long i;
		#pragma omp for schedule(static)
		for (i = 0; i < nItems; i++)
		{
			value_hashmap[h_vals[i]] = 0;
		}
	}

	numVals = value_hashmap.size();

	i = 0;
	for (auto it = value_hashmap.begin(); it != value_hashmap.end(); it++, i++)
	{
		csv_vals[i] = it->first;
		it->second = i;
	}

	// int8
	if (numVals < 256)
	{
		val_type = 0;
		uint8_t *vals_ptr = (uint8_t *)csv_vals_ptr;

		#pragma omp parallel
		{
			long i;
			#pragma omp for schedule(static) nowait
			for (i = 0; i < nItems; i++)
			{
				vals_ptr[i] = value_hashmap[h_vals[i]];
			}
		}
	}
	// int16
	else if (numVals < 65536)
	{
		val_type = 1;
		uint16_t *vals_ptr = (uint16_t *)csv_vals_ptr;

		#pragma omp parallel
		{
			long i;
			#pragma omp for schedule(static) nowait
			for (i = 0; i < nItems; i++)
			{
				vals_ptr[i] = value_hashmap[h_vals[i]];
			}
		}
	}
	// int32
	else
	{
		val_type = 2;
		int *vals_ptr = (int *)csv_vals_ptr;

		#pragma omp parallel
		{
			long i;
			#pragma omp for schedule(static) nowait
			for (i = 0; i < nItems; i++)
			{
				vals_ptr[i] = value_hashmap[h_vals[i]];
			}
		}
	}
}


struct CSRRVArrays : Matrix_Format
{
	INT_T * ia;
	INT_T * ja;
	ValueType * a;

	// csr
	floatType * h_val;
	int * h_cols;
	int * h_rowDelimiters;

	// Number of non-zero elements in the matrix
	int nItems;
	int numRows;
	int numCols;

	// csv
	int numVals;
	int val_type;
	floatType * csv_vals;
	void * csv_vals_ptr;

	CSRRVArrays(INT_T * ia, INT_T * ja, ValueType * a, long m, long n, long nnz) : Matrix_Format(m, n, nnz), ia(ia), ja(ja), a(a)
	{
		h_val = a;
		h_cols = ja;
		h_rowDelimiters = ia;
		nItems = nnz;
		numRows = m;
		numCols = n;

		csv_vals_ptr = aligned_alloc(64, sizeof(int) * nItems);
		csv_vals = (floatType *)aligned_alloc(64, sizeof(floatType) * nItems);
		csr2csrv(h_val, nItems, csv_vals_ptr, csv_vals, numVals, val_type);
	}

	~CSRRVArrays()
	{
		free(a);
		free(ia);
		free(ja);
		free(csv_vals);
		free(csv_vals_ptr);
	}

	void spmv(ValueType * x, ValueType * y);
	void statistics_start();
	int statistics_print_data(char * buf, long buf_n);
};


void
csrrv_spmv(const double * csv_vals, const void * csv_vals_ptr, const int * h_cols, const int * h_rowDelimiters, const int numRows, const int numVals, const floatType * x, floatType * y)
{
	if (numVals <= 256)
	{
		uint8_t * val_ptr = (uint8_t *)csv_vals_ptr;

		#pragma omp parallel
		{
			__m256i _col_ptr, _val_ptr;
			__m128i _val_ptr_uint8;
			__m512d _x, _y, _val;
			__mmask8 _mask;
			int start, padding_end, end, row, i;
			#pragma omp for schedule(static) nowait
			for (row = 0; row < numRows; row++)
			{
				_mm_prefetch((const char *)&y[row], _MM_HINT_T1);

				_y = _mm512_setzero_pd();

				start = h_rowDelimiters[row];
				padding_end = h_rowDelimiters[row + 1];
				end = start + (padding_end - start) / 8 * 8;

				for (i = start; i < end; i += 8)
				{
					_val_ptr_uint8 = _mm_loadu_epi8(&val_ptr[i]);

					// _val_ptr_uint8 = _mm_maskz_loadu_epi8(_mmask_load, &val_ptr[i]);

					_val_ptr = _mm256_cvtepu8_epi32(_val_ptr_uint8);

					_val = _mm512_i32gather_pd(_val_ptr, csv_vals, 8);

					_col_ptr = _mm256_loadu_epi32(&h_cols[i]);

					_x = _mm512_i32gather_pd(_col_ptr, x, 8);

					_y = _mm512_fmadd_pd(_x, _val, _y);
				}

				_mask = _mm_cmp_epi16_mask(_m128cmp, _mm_set1_epi16(padding_end - end), 1);

				_val_ptr_uint8 = _mm_maskz_loadu_epi8(_mask, &val_ptr[end]);
				_val_ptr = _mm256_cvtepu8_epi32(_val_ptr_uint8);
				_val = _mm512_mask_i32gather_pd(_m512zero, _mask, _val_ptr, csv_vals, 8);

				_col_ptr = _mm256_maskz_loadu_epi32(_mask, &h_cols[end]);
				_x = _mm512_mask_i32gather_pd(_m512zero, _mask, _col_ptr, x, 8);

				y[row] = _mm512_reduce_add_pd(_y);
			}
		}
	}
	else if (numVals <= 65536)
	{
		uint16_t * val_ptr = (uint16_t *)csv_vals_ptr;
		#pragma omp parallel
		{
			__m256i _col_ptr, _val_ptr;
			__m128i _val_ptr_uint16;
			__m512d _x, _y, _val;
			__mmask8 _mask;
			int start, padding_end, end, row, i;

			#pragma omp for schedule(static) nowait
			for (row = 0; row < numRows; row++)
			{
				_mm_prefetch((const char *)&y[row], _MM_HINT_T1);

				_y = _mm512_setzero_pd();

				start = h_rowDelimiters[row];
				padding_end = h_rowDelimiters[row + 1];
				end = start + (padding_end - start) / 8 * 8;

				for (i = start; i < end; i += 8)
				{
					_val_ptr_uint16 = _mm_loadu_epi16(&val_ptr[i]); // 4*32

					_val_ptr = _mm256_cvtepu16_epi32(_val_ptr_uint16);

					_val = _mm512_i32gather_pd(_val_ptr, csv_vals, 8);

					_col_ptr = _mm256_loadu_epi32(&h_cols[i]);

					_x = _mm512_i32gather_pd(_col_ptr, x, 8);

					_y = _mm512_fmadd_pd(_x, _val, _y);
				}

				_mask = _mm_cmp_epi16_mask(_m128cmp, _mm_set1_epi16(padding_end - end), 1);

				_val_ptr_uint16 = _mm_maskz_loadu_epi16(_mask, &val_ptr[end]);
				_val_ptr = _mm256_cvtepu16_epi32(_val_ptr_uint16);
				_val = _mm512_mask_i32gather_pd(_m512zero, _mask, _val_ptr, csv_vals, 8);

				_col_ptr = _mm256_maskz_loadu_epi32(_mask, &h_cols[end]);
				_x = _mm512_mask_i32gather_pd(_m512zero, _mask, _col_ptr, x, 8);

				_y = _mm512_fmadd_pd(_x, _val, _y);

				y[row] = _mm512_reduce_add_pd(_y);
			}
		}
	}
	else
	{
		int * val_ptr = (int *)csv_vals_ptr;

		#pragma omp parallel
		{
			__m256i _col_ptr, _val_ptr;
			__m512d _x, _y, _val;
			__mmask8 _mask;
			int start, padding_end, end, num, row, i;
			#pragma omp for schedule(static) nowait
			for (row = 0; row < numRows; row++)
			{
				_mm_prefetch((const char *)&y[row], _MM_HINT_T1);

				_y = _mm512_setzero_pd();

				start = h_rowDelimiters[row];
				padding_end = h_rowDelimiters[row + 1];
				end = start + (padding_end - start) / 8 * 8;

				for (i = start; i < end; i += 8)
				{
					_val_ptr = _mm256_loadu_epi32(&val_ptr[i]);

					_val = _mm512_i32gather_pd(_val_ptr, csv_vals, 8);

					_col_ptr = _mm256_loadu_epi32(&h_cols[i]);

					_x = _mm512_i32gather_pd(_col_ptr, x, 8);

					_y = _mm512_fmadd_pd(_x, _val, _y);
				}

				_mask = _mm_cmp_epi16_mask(_m128cmp, _mm_set1_epi16(padding_end - end), 1);

				_val_ptr = _mm256_maskz_loadu_epi32(_mask, &val_ptr[end]);
				_val = _mm512_mask_i32gather_pd(_m512zero, _mask, _val_ptr, csv_vals, 8);

				_col_ptr = _mm256_maskz_loadu_epi32(_mask, &h_cols[end]);
				_x = _mm512_mask_i32gather_pd(_m512zero, _mask, _col_ptr, x, 8);

				_y = _mm512_fmadd_pd(_x, _val, _y);

				y[row] = _mm512_reduce_add_pd(_y);
			}
		}
	}
}


void
CSRRVArrays::spmv(ValueType * x, ValueType * y)
{
	csrrv_spmv(csv_vals, csv_vals_ptr, h_cols, h_rowDelimiters, numRows, numVals, x, y);
}


struct Matrix_Format *
csr_to_format(INT_T * row_ptr, INT_T * col_ind, ValueType * values, long m, long n, long nnz)
{
	struct CSRRVArrays * csrrv = new CSRRVArrays(row_ptr, col_ind, values, m, n, nnz);
	csrrv->mem_footprint = csrrv->numVals*(sizeof(ValueType) + sizeof(INT_T)) + nnz*sizeof(INT_T) + (m+1)*sizeof(INT_T);
	csrrv->format_name = (char *) "Custom_CSR_BV_LUT_x86";
	return csrrv;
}


//==========================================================================================================================================
//= Print Statistics
//==========================================================================================================================================


void
CSRRVArrays::statistics_start()
{
}


int
statistics_print_labels(__attribute__((unused)) char * buf, __attribute__((unused)) long buf_n)
{
	return 0;
}


int
CSRRVArrays::statistics_print_data(__attribute__((unused)) char * buf, __attribute__((unused)) long buf_n)
{
	return 0;
}

