#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

#include <immintrin.h>
#include <algorithm>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <cassert>
#include <iostream>
#include <map>
#include <queue>
#include <vector>
#include <unordered_map>
#include "unistd.h"

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


using namespace std;
using namespace chrono;


struct spv8_csr_matrix {
	int m, rows, cols;
	double *nnz, *x, *y;
	int *col, *rowb, *rowe;
	int *tstart;
	int *tend;
};


struct tr_matrix {
	spv8_csr_matrix mat;
	vector<int> spvv8_len;
	vector<vector<int>> tasks;
};


spv8_csr_matrix
// apply_order(spv8_csr_matrix &mat, vector<vector<int>> &tasks, int copy_oob = true)
apply_order(spv8_csr_matrix &mat, vector<vector<int>> &tasks)
{
	spv8_csr_matrix ret;

	ret.m = mat.m;
	ret.rows = mat.rows;
	ret.cols = mat.cols;
	ret.nnz = (double *) malloc(mat.m * sizeof(double));
	ret.col = (int *) malloc(mat.m * sizeof(int));
	ret.rowb = (int *) malloc(mat.rows * sizeof(int));
	ret.rowe = (int *) malloc(mat.rows * sizeof(int));
	// if (copy_oob) {
		// ret.x = (double *) malloc(mat.cols * sizeof(double));
		// ret.y = (double *) malloc(mat.rows * sizeof(double));
	// }
	ret.tstart = (int *) malloc(tasks.size() * sizeof(int));
	ret.tend = (int *) malloc(tasks.size() * sizeof(int));

	// if (copy_oob) {
		// for (int i = 0; i < mat.cols; i++)
			// ret.x[i] = mat.x[i];
		// for (int i = 0; i < mat.rows; i++)
			// ret.y[i] = 0;
	// }

	int npos = 0, pos = 0;
	int start = 0, t = 0;
	for (vector<int> &task : tasks) {
		ret.tstart[t] = start;
		ret.tend[t++] = start + task.size();
		start += task.size();
		for (int row : task) {
			int b = mat.rowb[row];
			int e = mat.rowe[row];
			ret.rowb[pos] = npos;
			ret.rowe[pos++] = npos + e - b;
			for (int i = b; i < e; i++) {
				ret.nnz[npos] = mat.nnz[i];
				ret.col[npos++] = mat.col[i];
			}
		}
	}

	return ret;
}


tr_matrix
tr_reorder(spv8_csr_matrix &mat, vector<vector<int>> &tasks)
{
	tr_matrix tr;

	for (vector<int> &task : tasks) {
		unordered_map<int, vector<int>> buckets;

		for (int r : task) {
			int rowlen = mat.rowe[r] - mat.rowb[r];
			buckets[rowlen].push_back(r);
		}

		vector<int> keys;
		for (auto kv : buckets) {
			keys.push_back(kv.first);
		}
		sort(keys.begin(), keys.end());

		vector<int> order;
		vector<int> remain;
		for (int k : keys) {
			vector<int> &samelen_task = buckets[k];
			int left = samelen_task.size() % 8;
			if (k > 32)
				left = samelen_task.size();
			int bulk = samelen_task.size() - left;
			order.insert(order.end(), samelen_task.begin(), samelen_task.begin() + bulk);
			remain.insert(remain.end(), samelen_task.begin() + bulk, samelen_task.end());
		}

		tr.spvv8_len.push_back(order.size());

		task.clear();
		task.insert(task.end(), order.begin(), order.end());
		task.insert(task.end(), remain.begin(), remain.end());
	}

	tr.mat = apply_order(mat, tasks);

	int size = tasks.size();
	for (int t = 0; t < size; t++) {
		int start = tr.mat.tstart[t];
		int tr_len = tr.spvv8_len[t];
		int p = 0, c = 0;
		for (p = start; c < tr_len; c += 8, p += 8) {
			int rowlen = tr.mat.rowe[p] - tr.mat.rowb[p];
			int base = tr.mat.rowb[p];
			vector<double> nnz;
			vector<int> col;
			nnz.insert(nnz.end(), tr.mat.nnz + base, tr.mat.nnz + base + rowlen * 8);
			col.insert(col.end(), tr.mat.col + base, tr.mat.col + base + rowlen * 8);
			for (int l = 0; l < rowlen; l++) {
				for (int r = 0; r < 8; r++) {
					tr.mat.nnz[base + l * 8 + r] = nnz[r * rowlen + l];
					tr.mat.col[base + l * 8 + r] = col[r * rowlen + l];
				}
			}
		}
	}

	tr.tasks = tasks;

	return tr;
}


bool
is_banded(spv8_csr_matrix &mat, int band_size = -1)
{
	if (band_size == -1)
		band_size = mat.cols / 64;
	int band_count = 0;
	bool banded = false;

	for (int r = 0; r < mat.rows; r++) {
		int rb = mat.rowb[r];
		int re = mat.rowe[r];
		for (int i = rb; i < re; i++) {
			int col = mat.col[i];
			if (abs(col - r) <= band_size)
				band_count++;
		}
	}

	if (double(band_count) / mat.m >= 0.3) {
		banded = true;
	}

	return banded;
}


tr_matrix
process(spv8_csr_matrix &mat, int panel_num)
{
	vector<vector<int>> tasks(panel_num);

	int pos = 0;
	int len = mat.m / panel_num;
	int limit = mat.rows - 7;
	int i;
	int count = 0;
	for (i = 0; i < limit; i += 8) {
		for (int j = 0; j < 8; j++) {
			int rowlen = mat.rowe[i + j] - mat.rowb[i + j];
			if (rowlen > 0) {
				tasks[pos].push_back(i + j);
				count += rowlen;
			}
		}

		if (count >= len) {
			if (pos + 1 < panel_num) {
				pos += 1;
				count = 0;
			}
		}
	}

	if (i < mat.rows) {
		for (; i < mat.rows; i++) {
			tasks[pos].push_back(i);
		}
	}

	tr_matrix ret = tr_reorder(mat, tasks);
	return ret;
}


inline __attribute__((always_inline))
double
avx512_fma_spvv_kernel(int *col, double *nnz, int rowlen, double *x)
{
	int limit = rowlen - 7;
	int *col_p;
	double *nnz_p;
	double sum = 0;
	__m256i c1;
	__m512d v1, v2, s;
	s = _mm512_setzero_pd();
	int i;

	for (i = 0; i < limit; i += 8) {
		col_p = col + i;
		nnz_p = nnz + i;
		c1 = _mm256_loadu_si256((const __m256i *) col_p);
		v2 = _mm512_i32gather_pd(c1, x, 8);
		v1 = _mm512_loadu_pd(nnz_p);
		s = _mm512_fmadd_pd(v1, v2, s);
	}

	sum += _mm512_reduce_add_pd(s);
	for (; i < rowlen; i++) {
		sum += nnz[i] * x[col[i]];
	}

	return sum;
}


inline __attribute__((always_inline))
void
avx512_spvv8_kernel_tr(const int *rows, int *rowb, int *rowe, int *col, double *nnz, double *x, double *y)
{
	__m256i rs = _mm256_loadu_si256((const __m256i *) rows);
	__m512d acc = _mm512_setzero_pd();

	int rowlen = *rowe - *rowb;
	int base = *rowb;

	{
		int idx0 = rows[0];
		int idx1 = rows[1];
		int idx2 = rows[2];
		int idx3 = rows[3];
		int idx4 = rows[4];
		int idx5 = rows[5];
		int idx6 = rows[6];
		int idx7 = rows[7];

		_m_prefetchw(y + idx0);
		_m_prefetchw(y + idx1);
		_m_prefetchw(y + idx2);
		_m_prefetchw(y + idx3);
		_m_prefetchw(y + idx4);
		_m_prefetchw(y + idx5);
		_m_prefetchw(y + idx6);
		_m_prefetchw(y + idx7);
	}

	for (int c = 0; c < rowlen; c++) {
		int offset = base + c * 8;
		__m256i cc = _mm256_loadu_si256((const __m256i *) (col + offset));
		__m512d nz = _mm512_loadu_pd(nnz + offset);
		__m512d xx = _mm512_i32gather_pd(cc, x, 8);
		acc = _mm512_fmadd_pd(nz, xx, acc);
	}

	_mm512_i32scatter_pd(y, rs, acc, 8);
}

void
spmv_tr_spvv8_kernel(tr_matrix &tr, int threads)
{
	int size = tr.tasks.size();
	#pragma omp parallel for num_threads(threads) schedule(dynamic)
	for (int tid = 0; tid < size; tid++) {
		vector<int> &task = tr.tasks[tid];
		spv8_csr_matrix &mat = tr.mat;
		int *rows = task.data();
		int start = mat.tstart[tid];
		int end = mat.tend[tid];
		int limit = tr.spvv8_len[tid];
		int p, c;
		for (p = start, c = 0; c < limit; p += 8, c += 8) {
			avx512_spvv8_kernel_tr(rows + c, mat.rowb + p, mat.rowe + p, mat.col, mat.nnz, mat.x, mat.y);
		}
		for (; p < end; p++) {
			int r = rows[p - start];
			int begin = mat.rowb[p];
			int end = mat.rowe[p];
			int rowlen = end - begin;
			_mm_prefetch(mat.y + r, _MM_HINT_ET1);
			mat.y[r] = avx512_fma_spvv_kernel(mat.col + begin, mat.nnz + begin, rowlen, mat.x);
		}
	}
}



struct SpV8_Array : Matrix_Format
{
	INT_T * ia;      // the usual rowptr (of size m+1)
	INT_T * ja;      // the colidx of each NNZ (of size nnz)
	ValueType * a;   // the values (of size NNZ)

	spv8_csr_matrix mat;
	tr_matrix tr;

	SpV8_Array(INT_T * ia, INT_T * ja, ValueType * a, long m, long n, long nnz) : Matrix_Format(m, n, nnz), ia(ia), ja(ja), a(a)
	{
		int num_threads = omp_get_max_threads();
		long i;

		mat.nnz = (typeof(mat.nnz))  malloc(mat.m * sizeof(double));
		mat.col = (typeof(mat.col))  malloc(mat.m * sizeof(int));

		mat.m = nnz;
		mat.rows = m;
		mat.cols = n;

		mat.nnz = a;
		mat.col = ja;
		mat.rowb = ia;
		mat.rowe = (typeof(mat.rowe)) malloc(mat.rows * sizeof(mat.rowe));
		for (i=0;i<m;i++)
		{
			mat.rowe[i] = ia[i+1];
		}

		mat.tstart = NULL;
		mat.tend = NULL;

		bool banded = is_banded(mat);
		int panel_count = max(num_threads * 4, mat.rows / 2000);
		if (banded)
			panel_count = max(num_threads * 4, mat.rows / 10000);
		tr = process(mat, panel_count);

	}

	~SpV8_Array()
	{

	}

	void spmv(ValueType * x, ValueType * y);
	void statistics_start();
	int statistics_print_data(char * buf, long buf_n);
};

void
SpV8_Array::spmv(ValueType * x, ValueType * y)
{
	int num_threads = omp_get_max_threads();
	mat.x = x;
	mat.y = y;
	spmv_tr_spvv8_kernel(tr, num_threads);
}


struct Matrix_Format *
csr_to_format(INT_T * row_ptr, INT_T * col_ind, ValueType * values, long m, long n, long nnz)
{
	struct SpV8_Array * spv8 = new SpV8_Array(row_ptr, col_ind, values, m, n, nnz);
	spv8->mem_footprint = nnz * (sizeof(ValueType) + sizeof(INT_T)) + (m+1) * sizeof(INT_T);
	spv8->format_name = (char *) "SPV8";
	return spv8;
}


//==========================================================================================================================================
//= Print Statistics
//==========================================================================================================================================


void
SpV8_Array::statistics_start()
{
}


int
statistics_print_labels(__attribute__((unused)) char * buf, __attribute__((unused)) long buf_n)
{
	return 0;
}


int
SpV8_Array::statistics_print_data(__attribute__((unused)) char * buf, __attribute__((unused)) long buf_n)
{
	return 0;
}

