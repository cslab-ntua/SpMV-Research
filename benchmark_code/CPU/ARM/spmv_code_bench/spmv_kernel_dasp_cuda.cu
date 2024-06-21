#include <stdlib.h>
#include <stdio.h>
#include <omp.h>
#include <iostream>
#include <cmath>

#include "dasp/dasp_f64.h"

#include "macros/cpp_defines.h"

#include "spmv_bench_common.h"
#include "spmv_kernel.h"

#ifdef __cplusplus
extern "C"{
#endif
	// #include "macros/macrolib.h"
	#include "time_it.h"
	// #include "parallel_util.h"
	// #include "array_metrics.h"

	#include "cuda/cuda_util.h"
#ifdef __cplusplus
}
#endif

#ifndef TIME_IT
#define TIME_IT 0
#endif

struct DASPArrays : Matrix_Format
{
	INT_T * ia;      // the usual rowptr (of size m+1)
	INT_T * ja;      // the colidx of each NNZ (of size nnz)
	ValueType * a;   // the values (of size NNZ)

	ValueType * x = NULL;
	ValueType * y = NULL;
	ValueType * x_d = NULL;
	ValueType * y_d = NULL;

	// DASP specific
	INT_T * order_rid;
	double threshold = 0.75;
	int block_longest = 256;

	int *short_rid_1, *short_rid_2, *short_rid_3, *short_rid_4, *long_rid, *zero_rid;
	int *ridA;

	// INT_T is int
	INT_T *rptA, *long_rpt;
	INT_T *long_rpt_new;
	ValueType *val_by_warp;
	int *rid_by_warp;

	ValueType *short_val, *long_val, *reg_val, *irreg_val;
	int *short_cid, *long_cid, *reg_cid, *irreg_cid;
	INT_T *blockPtr, *irreg_rpt;

	int BlockNum_all;
	int ThreadNum_all;
	int sumBlockNum;

	int rowloop;
	int row_long = 0, row_block = 0, row_zero = 0;
	int blocknum;
	int common_13, short_row_1 = 0, short_row_3 = 0, short_row_2 = 0, short_row_4 = 0, short_row_34;
	int offset_reg, offset_short1, offset_short13, offset_short34, offset_short22;
	INT_T fill0_nnz_short13, fill0_nnz_short34;

	// DASP specific - device buffers
	// init cuda data of long part
	ValueType *long_val_d, *val_by_warp_d;
	int *long_ptr_warp_d;
	int *long_cid_d; 

	// init cuda data of short part
	ValueType *short_val_d;
	int *short_cid_d;

	// init cuda data of reg & irreg part
	ValueType *reg_val_d, *irreg_val_d;
	int *block_ptr_d, *irreg_rpt_d;
	int *reg_cid_d, *irreg_cid_d;

	// cudaEvent_t is useful for timing, but for performance use " cudaEventCreateWithFlags ( &event, cudaEventDisableTiming) "
	cudaEvent_t startEvent_execution;
	cudaEvent_t endEvent_execution;

	cudaEvent_t startEvent_memcpy_long_val;
	cudaEvent_t endEvent_memcpy_long_val;
	cudaEvent_t startEvent_memcpy_val_by_warp;
	cudaEvent_t endEvent_memcpy_val_by_warp;
	cudaEvent_t startEvent_memcpy_long_ptr_warp;
	cudaEvent_t endEvent_memcpy_long_ptr_warp;
	cudaEvent_t startEvent_memcpy_long_cid;
	cudaEvent_t endEvent_memcpy_long_cid;
	cudaEvent_t startEvent_memcpy_short_val;
	cudaEvent_t endEvent_memcpy_short_val;
	cudaEvent_t startEvent_memcpy_short_cid;
	cudaEvent_t endEvent_memcpy_short_cid;
	cudaEvent_t startEvent_memcpy_reg_val;
	cudaEvent_t endEvent_memcpy_reg_val;
	cudaEvent_t startEvent_memcpy_irreg_val;
	cudaEvent_t endEvent_memcpy_irreg_val;
	cudaEvent_t startEvent_memcpy_block_ptr;
	cudaEvent_t endEvent_memcpy_block_ptr;
	cudaEvent_t startEvent_memcpy_irreg_rpt;
	cudaEvent_t endEvent_memcpy_irreg_rpt;
	cudaEvent_t startEvent_memcpy_reg_cid;
	cudaEvent_t endEvent_memcpy_reg_cid;
	cudaEvent_t startEvent_memcpy_irreg_cid;
	cudaEvent_t endEvent_memcpy_irreg_cid;

	cudaEvent_t startEvent_memcpy_x;
	cudaEvent_t endEvent_memcpy_x;
	cudaEvent_t startEvent_memcpy_y;
	cudaEvent_t endEvent_memcpy_y;

	DASPArrays(INT_T * ia, INT_T * ja, ValueType * a, long m, long n, long nnz) : Matrix_Format(m, n, nnz), ia(ia), ja(ja), a(a)
	{
		/************************************************************************************************************************************************************************/
		/*************************************************************************** DASP preprocessing **************************************************************************/
		/************************************************************************************************************************************************************************/
		order_rid = (typeof(order_rid))malloc(m * sizeof(*order_rid));

		// three parts: short row (1 & 3 & 2 & 4), long row, row-block (regular（origin & fill0） & irregular)
		INT_T nnz_short, nnz_long, origin_nnz_reg, fill0_nnz_reg, nnz_irreg;

		// block_longest = -1;
		// for(int i=0; i<m; i++){
		// 	int row_len = ia[i + 1] - ia[i];
		// 	if(row_len>block_longest)
		// 		block_longest=row_len;
		// }

		// get the short part data
		// (short_val, short_cid)
		for (int i = 0; i < m; i ++)
		{
			int row_len = ia[i + 1] - ia[i];
			if (row_len == 1)
			{
				short_row_1 ++;
			}
			else if (row_len == 3)
			{
				short_row_3 ++;
			}
			else if (row_len == 2)
			{
				short_row_2 ++;
			}
			else if (row_len == 0)
			{
				row_zero ++;
			}
			else if (row_len == 4)
			{
				short_row_4 ++;
			}
			// else if (row_len >= warpNum_long * loopNum_long * MMA_M * MMA_K)
			else if (row_len >= block_longest)
			{
				row_long ++;
			}
			else
			{
				row_block ++;
			}
		}

		if (row_block < 59990) rowloop = 1;
		else if (row_block >= 59990 && row_block < 400000) rowloop = 2;
		else rowloop = 4;

		short_rid_1 = (typeof(short_rid_1))malloc(short_row_1 * sizeof(*short_rid_1));
		short_rid_2 = (typeof(short_rid_2))malloc(short_row_2 * sizeof(*short_rid_2));
		short_rid_3 = (typeof(short_rid_3))malloc(short_row_3 * sizeof(*short_rid_3));
		short_rid_4 = (typeof(short_rid_4))malloc(short_row_4 * sizeof(*short_rid_4));
		long_rid = (typeof(long_rid))malloc(row_long * sizeof(*long_rid));
		zero_rid = (typeof(zero_rid))malloc(row_zero * sizeof(*zero_rid));
		ridA = (typeof(ridA))malloc(row_block * sizeof(*ridA));

		INT_T *rptA = (typeof(rptA))malloc((row_block + 1) * sizeof(*rptA));
		memset(rptA, 0, sizeof(INT_T) * (row_block + 1));
		INT_T *long_rpt = (typeof(long_rpt))malloc((row_long + 1) * sizeof(*long_rpt));
		memset(long_rpt, 0, sizeof(INT_T) * (row_long + 1));

		int short_row_flag1 = 0, short_row_flag3 = 0, short_row_flag2 = 0, short_row_flag4 = 0;
		int row_long_flag = 0, flag0 = 0, row_block_flag = 0;
		for (int i = 0; i < m; i ++)
		{
			int row_len = ia[i + 1] - ia[i];
			if (row_len == 1)
			{
				short_rid_1[short_row_flag1] = i;
				short_row_flag1 ++;
			}
			else if (row_len == 3)
			{
				short_rid_3[short_row_flag3] = i;
				short_row_flag3 ++;
			}
			else if (row_len == 2)
			{
				short_rid_2[short_row_flag2] = i;
				short_row_flag2 ++;
			}
			else if (row_len == 0)
			{
				zero_rid[flag0] = i;
				flag0 ++;
			}
			else if (row_len == 4)
			{
				short_rid_4[short_row_flag4] = i;
				short_row_flag4 ++;
			}
			// else if (row_len >= warpNum_long * loopNum_long * MMA_M * MMA_K)
			else if (row_len >= block_longest)
			{
				long_rpt[row_long_flag] = row_len;
				long_rid[row_long_flag] = i;
				row_long_flag ++;
			}
			else
			{
				rptA[row_block_flag] = row_len;
				ridA[row_block_flag] = i;
				row_block_flag ++;
			}
		} 
		nnz_short = short_row_1 + short_row_3 * 3 + short_row_2 * 2 + short_row_4 * 4;
	 
		common_13 = short_row_1 < short_row_3 ? short_row_1 : short_row_3;
		if (common_13 / BlockSize >= 16)
		{
			common_13 = BlockSize * (common_13 / BlockSize);
			short_row_1 = short_row_1 - common_13;
			short_row_3 = short_row_3 - common_13;
		}
		else
		{
			common_13 = 0;
		}

		int short_block13 = (common_13 + BlockSize - 1) / BlockSize;  
		int half_short_row_2 = (short_row_2 + 1) / 2;
		int short_block22 = (half_short_row_2 + BlockSize - 1) / BlockSize;
		short_row_34 = short_row_3 + short_row_4;
		int short_block34 = (short_row_34 + BlockSize - 1) / BlockSize;

		int block13_per_threadblock = warpNum_short * groupNum * 2;
		int block22_per_threadblock = warpNum_short * groupNum * 2;
		int block34_per_threadblock = warpNum_short * groupNum * loopNum_short;

		int threadblock13 = (short_block13 + block13_per_threadblock - 1) / block13_per_threadblock;
		int threadblock22 = (short_block22 + block22_per_threadblock - 1) / block22_per_threadblock;
		int threadblock34 = (short_block34 + block34_per_threadblock - 1) / block34_per_threadblock;

		fill0_nnz_short13 = threadblock13 * block13_per_threadblock * MMA_M * MMA_K;
		fill0_nnz_short34 = threadblock34 * block34_per_threadblock * MMA_M * MMA_K;
		INT_T fill0_nnz_short22 = threadblock22 * block22_per_threadblock * MMA_M * MMA_K;
		INT_T fill0_nnz_short = short_row_1 + fill0_nnz_short13 + fill0_nnz_short34 + fill0_nnz_short22;
		short_val = (typeof(short_val))malloc(fill0_nnz_short * sizeof(*short_val));
		short_cid = (typeof(short_cid))malloc(fill0_nnz_short * sizeof(*short_cid));
		memset(short_val, 0.0, sizeof(ValueType) * fill0_nnz_short);
		memset(short_cid, 0, sizeof(int) * fill0_nnz_short);

		int super_group = 1 + threadblock13 + threadblock34 + threadblock22;
		INT_T *superX_ptr = (typeof(superX_ptr))malloc((super_group + 1) * sizeof(*superX_ptr));
		
		for (int i = 0; i < short_row_1; i ++)
		{
			int cur_row = short_rid_1[i];
			short_val[i] = a[ia[cur_row]];
			short_cid[i] = ja[ia[cur_row]];
		}

		for (int i = 0; i < short_block13; i ++)
		{
			ValueType *cur_short_val = short_val + short_row_1 + i * MMA_M * MMA_K;
			int *cur_short_cid = short_cid + short_row_1 + i * MMA_M * MMA_K;

			for (int j = 0; j < BlockSize && i * BlockSize + j < common_13; j ++)
			{
				int cur_row_1 = short_rid_1[short_row_1 + i * BlockSize + j];
				int cur_row_3 = short_rid_3[i * BlockSize + j];
				cur_short_val[j * MMA_K] = a[ia[cur_row_1]];
				cur_short_cid[j * MMA_K] = ja[ia[cur_row_1]];
				cur_short_val[j * MMA_K + 1] = a[ia[cur_row_3]];
				cur_short_val[j * MMA_K + 2] = a[ia[cur_row_3] + 1];
				cur_short_val[j * MMA_K + 3] = a[ia[cur_row_3] + 2];
				cur_short_cid[j * MMA_K + 1] = ja[ia[cur_row_3]];
				cur_short_cid[j * MMA_K + 2] = ja[ia[cur_row_3] + 1];
				cur_short_cid[j * MMA_K + 3] = ja[ia[cur_row_3] + 2];
			}
		}

		for (int i = 0; i < short_row_3; i ++)
		{
			// ValueType *cur_short_val = short_val + short_row_1 + short_block13 * MMA_M * MMA_K + i * MMA_K;
			// int *cur_short_cid = short_cid + short_row_1 + short_block13 * MMA_M * MMA_K + i * MMA_K;
			ValueType *cur_short_val = short_val + short_row_1 + fill0_nnz_short13 + i * MMA_K;
			int *cur_short_cid = short_cid + short_row_1 + fill0_nnz_short13 + i * MMA_K;
			
			int cur_row = short_rid_3[common_13 + i];

			cur_short_val[0] = a[ia[cur_row]];
			cur_short_val[1] = a[ia[cur_row] + 1]; 
			cur_short_val[2] = a[ia[cur_row] + 2]; 
			cur_short_cid[0] = ja[ia[cur_row]];
			cur_short_cid[1] = ja[ia[cur_row] + 1]; 
			cur_short_cid[2] = ja[ia[cur_row] + 2]; 
		}

		for (int i = 0; i < short_row_4; i ++)
		{
			ValueType *cur_short_val = short_val + short_row_1 + fill0_nnz_short13 + (short_row_3 + i) * MMA_K;
			int *cur_short_cid = short_cid + short_row_1 + fill0_nnz_short13 + (short_row_3 + i) * MMA_K;
			
			int cur_row = short_rid_4[i];

			cur_short_val[0] = a[ia[cur_row]];
			cur_short_val[1] = a[ia[cur_row] + 1]; 
			cur_short_val[2] = a[ia[cur_row] + 2]; 
			cur_short_val[3] = a[ia[cur_row] + 3]; 
			cur_short_cid[0] = ja[ia[cur_row]];
			cur_short_cid[1] = ja[ia[cur_row] + 1]; 
			cur_short_cid[2] = ja[ia[cur_row] + 2]; 
			cur_short_cid[3] = ja[ia[cur_row] + 3]; 
		}

		for (int i = 0; i < short_block22; i ++)
		{
			ValueType *cur_short_val = short_val + short_row_1 + fill0_nnz_short13 + fill0_nnz_short34 + i * MMA_M * MMA_K;
			int *cur_short_cid = short_cid + short_row_1 + fill0_nnz_short13 + fill0_nnz_short34 + i * MMA_M * MMA_K;

			for (int j = 0; j < BlockSize * 2 && (i * BlockSize * 2 + j) < short_row_2; j ++)
			{
				int cur_row = short_rid_2[i * BlockSize * 2 + j];
				cur_short_val[j % BlockSize * MMA_K + (j / BlockSize) * 2] = a[ia[cur_row]];
				cur_short_val[j % BlockSize * MMA_K + (j / BlockSize) * 2 + 1] = a[ia[cur_row] + 1];
				cur_short_cid[j % BlockSize * MMA_K + (j / BlockSize) * 2] = ja[ia[cur_row]];
				cur_short_cid[j % BlockSize * MMA_K + (j / BlockSize) * 2 + 1] = ja[ia[cur_row] + 1];
			}
		}

		int *short_cid_temp = (typeof(short_cid_temp))malloc(fill0_nnz_short * sizeof(*short_cid_temp));
		memcpy(short_cid_temp, short_cid, sizeof(int) * fill0_nnz_short);

		quick_sort_key(short_cid_temp, short_row_1);
		int nnzr = short_row_1 > 0 ? 1 : 0;
		for (int i = 1; i < short_row_1; i ++)
		{
			nnzr += short_cid_temp[i] != short_cid_temp[i - 1] ? 1 : 0;
		}
		superX_ptr[0] = nnzr;

		INT_T *cur_superX_ptr = superX_ptr + 1;
		for (int i = 0; i < threadblock13; i++)
		{
			int *cur_short_cid_temp = short_cid_temp + short_row_1 + i * block13_per_threadblock * MMA_M * MMA_K;
			int len = block13_per_threadblock * MMA_M * MMA_K;
			quick_sort_key(cur_short_cid_temp, len);
			int nnzcid = len > 0 ? 1 : 0;
			for (int j = 1; j < len; j ++)
			{
				nnzcid += cur_short_cid_temp[j] != cur_short_cid_temp[j - 1] ? 1 : 0;
			}
			cur_superX_ptr[i] = nnzcid;
		}

		cur_superX_ptr = superX_ptr + 1 + threadblock13;
		for (int i = 0; i < threadblock34; i++)
		{
			int *cur_short_cid_temp = short_cid_temp + short_row_1 + fill0_nnz_short13 + i * block34_per_threadblock * MMA_M * MMA_K;
			int len = block34_per_threadblock * MMA_M * MMA_K;
			quick_sort_key(cur_short_cid_temp, len);
			int nnzcid = len > 0 ? 1 : 0;
			for (int j = 1; j < len; j ++)
			{
				nnzcid += cur_short_cid_temp[j] != cur_short_cid_temp[j - 1] ? 1 : 0;
			}
			cur_superX_ptr[i] = nnzcid;
		}

		cur_superX_ptr = superX_ptr + 1 + threadblock13 + threadblock34;
		for (int i = 0; i < threadblock22; i++)
		{
			int *cur_short_cid_temp = short_cid_temp + short_row_1 + fill0_nnz_short13 + fill0_nnz_short34 + i * block22_per_threadblock * MMA_M * MMA_K;
			int len = block22_per_threadblock * MMA_M * MMA_K;
			quick_sort_key(cur_short_cid_temp, len);
			int nnzcid = len > 0 ? 1 : 0;
			for (int j = 1; j < len; j ++)
			{
				nnzcid += cur_short_cid_temp[j] != cur_short_cid_temp[j - 1] ? 1 : 0;
			}
			cur_superX_ptr[i] = nnzcid;
		}
		exclusive_scan(superX_ptr, super_group + 1);
		INT_T nnz_superX = superX_ptr[super_group];

		int new_cid_len = short_row_1 + threadblock13 * block13_per_threadblock * MMA_M * MMA_K / 4 + \
										threadblock34 * block34_per_threadblock * MMA_M * MMA_K / 4 + \
										threadblock22 * block22_per_threadblock * MMA_M * MMA_K / 4;

		int *short_cid_new = (typeof(short_cid_new))malloc(new_cid_len * sizeof(*short_cid_new));

		int *superX_cid = (typeof(superX_cid))malloc(nnz_superX * sizeof(*superX_cid));
		int flag = 0;
		if (short_row_1)
		{
			superX_cid[0] = short_cid_temp[0];
			flag ++;
		}
		for (int j = 1; j < short_row_1; j ++)
		{
			if (short_cid_temp[j] != short_cid_temp[j - 1])
			{
				superX_cid[flag] = short_cid_temp[j];
				flag ++;
			}
		}
		if (flag != superX_ptr[1]) printf("flag1 = %d, len = %d\n", flag, superX_ptr[1]);
		for (int i = 0; i < short_row_1; i ++)
		{
			short_cid_new[i] = BinarySearch(superX_cid, superX_ptr[1], short_cid[i]);
		}

		cur_superX_ptr = superX_ptr + 1;
		for (int i = 0; i < threadblock13; i ++)
		{
			int *cur_short_cid_temp = short_cid_temp + short_row_1 + i * block13_per_threadblock * MMA_M * MMA_K;
			int len = block13_per_threadblock * MMA_M * MMA_K;
			int *cur_superX_cid = superX_cid + cur_superX_ptr[i];
			int xlen = cur_superX_ptr[i + 1] - cur_superX_ptr[i];
			int flag_cid = 0;
			if (len)
			{
				cur_superX_cid[0] = cur_short_cid_temp[0];
				flag_cid ++;
			}
			else
			{
				continue;
			}
			for (int j = 1; j < len; j ++)
			{
				if (cur_short_cid_temp[j] != cur_short_cid_temp[j - 1])
				{
					cur_superX_cid[flag_cid] = cur_short_cid_temp[j];
					flag_cid ++;
				}
			}
			if (flag_cid != xlen) printf("flag13 = %d, len = %d\n", flag_cid, xlen);

			int *cur_short_cid_new = short_cid_new + short_row_1 + i * (block13_per_threadblock * MMA_M * MMA_K / 4);
			int *cur_short_cid = short_cid + short_row_1 + i * block13_per_threadblock * MMA_M * MMA_K;
			for (int j = 0; j < len; j ++)
			{
				// cur_short_cid_new[j] = BinarySearch(cur_superX_cid, xlen, cur_short_cid[j]);
				SET_8_BIT(cur_short_cid_new[j / 4], BinarySearch(cur_superX_cid, xlen, cur_short_cid[j]), j % 4);
			}
		}

		cur_superX_ptr = superX_ptr + 1 + threadblock13;
		for (int i = 0; i < threadblock34; i ++)
		{
			int *cur_short_cid_temp = short_cid_temp + short_row_1 + fill0_nnz_short13 + i * block34_per_threadblock * MMA_M * MMA_K;
			int len = block34_per_threadblock * MMA_M * MMA_K;
			int *cur_superX_cid = superX_cid + cur_superX_ptr[i];
			int xlen = cur_superX_ptr[i + 1] - cur_superX_ptr[i];
			int flag_cid = 0;
			if (len)
			{
				cur_superX_cid[0] = cur_short_cid_temp[0];
				flag_cid ++;
			}
			else
			{
				continue;
			}
			for (int j = 1; j < len; j ++)
			{
				if (cur_short_cid_temp[j] != cur_short_cid_temp[j - 1])
				{
					cur_superX_cid[flag_cid] = cur_short_cid_temp[j];
					flag_cid ++;
				}
			}
			if (flag_cid != xlen) printf("flag34 = %d, len = %d\n", flag_cid, xlen);

			int *cur_short_cid_new = short_cid_new + short_row_1 + fill0_nnz_short13 / 4 + i * (block34_per_threadblock * MMA_M * MMA_K / 4);
			int *cur_short_cid = short_cid + short_row_1 + fill0_nnz_short13 + i * block34_per_threadblock * MMA_M * MMA_K;
			for (int j = 0; j < len; j ++)
			{
				// cur_short_cid_new[j] = BinarySearch(cur_superX_cid, xlen, cur_short_cid[j]);
				SET_8_BIT(cur_short_cid_new[j / 4], BinarySearch(cur_superX_cid, xlen, cur_short_cid[j]), j % 4);
			}
		}

		cur_superX_ptr = superX_ptr + 1 + threadblock13 + threadblock34;
		for (int i = 0; i < threadblock22; i ++)
		{
			int *cur_short_cid_temp = short_cid_temp + short_row_1 + fill0_nnz_short13 + fill0_nnz_short34 + i * block22_per_threadblock * MMA_M * MMA_K;
			int len = block22_per_threadblock * MMA_M * MMA_K;
			int *cur_superX_cid = superX_cid + cur_superX_ptr[i];
			int xlen = cur_superX_ptr[i + 1] - cur_superX_ptr[i];
			int flag_cid = 0;
			if (len)
			{
				cur_superX_cid[0] = cur_short_cid_temp[0];
				flag_cid ++;
			}
			else
			{
				continue;
			}
			for (int j = 1; j < len; j ++)
			{
				if (cur_short_cid_temp[j] != cur_short_cid_temp[j - 1])
				{
					cur_superX_cid[flag_cid] = cur_short_cid_temp[j];
					flag_cid ++;
				}
			}
			if (flag_cid != xlen) printf("flag22 = %d, len = %d\n", flag_cid, xlen);

			int *cur_short_cid_new = short_cid_new + short_row_1 + (fill0_nnz_short13 + fill0_nnz_short34) / 4 + i * (block22_per_threadblock * MMA_M * MMA_K / 4);
			int *cur_short_cid = short_cid + short_row_1 + fill0_nnz_short13 + fill0_nnz_short34 + i * block22_per_threadblock * MMA_M * MMA_K;
			for (int j = 0; j < len; j ++)
			{
				// cur_short_cid_new[j] = BinarySearch(cur_superX_cid, xlen, cur_short_cid[j]);
				SET_8_BIT(cur_short_cid_new[j / 4], BinarySearch(cur_superX_cid, xlen, cur_short_cid[j]), j % 4);
			}
		}
		free(superX_ptr);
		free(superX_cid);
		free(short_cid_temp);
		free(short_cid_new);

		// resort except rows
		radix_sort(rptA, ridA, row_block);

		// get the data except short row part
		// (rptA, cidA, valA)
		exclusive_scan(rptA, row_block + 1);
		exclusive_scan(long_rpt, row_long + 1);
		// nnz_row_block = rptA[row_block];
		nnz_long = long_rpt[row_long];

		//record the sort order
		memcpy(order_rid, long_rid, sizeof(int) * row_long);
		memcpy(order_rid + row_long, ridA, sizeof(int) * row_block);
		memcpy(order_rid + row_long + row_block, short_rid_1, sizeof(int) * short_row_1);
		for (int i = 0; i < short_block13; i ++)
		{
			int *cur_order_rid = order_rid + row_long + row_block + short_row_1 + i * BlockSize * 2;

			for (int j = 0; j < BlockSize; j ++)
			{
				cur_order_rid[j] = short_rid_1[short_row_1 + i * BlockSize + j];
				cur_order_rid[BlockSize + j] = short_rid_3[i * BlockSize + j];
			}
		}
		memcpy(order_rid + row_long + row_block + short_row_1 + common_13 * 2, short_rid_3 + common_13, sizeof(int) * short_row_3);
		memcpy(order_rid + row_long + row_block + short_row_1 + common_13 * 2 + short_row_3, short_rid_4, sizeof(int) * short_row_4);
		memcpy(order_rid + row_long + row_block + short_row_1 + common_13 * 2 + short_row_3 + short_row_4, short_rid_2, sizeof(int) * short_row_2);
		memcpy(order_rid + row_long + row_block + short_row_1 + common_13 * 2 + short_row_3 + short_row_4 + short_row_2, zero_rid, sizeof(int) * row_zero);

		int short_row = short_row_1 + common_13 * 2 + short_row_34 + short_row_2;
		int offset_short_row = row_long + row_block;

		ValueType *short3_val = (typeof(short3_val))malloc(nnz_short * sizeof(*short3_val));
		int *short3_cid = (typeof(short3_cid))malloc(nnz_short * sizeof(*short3_cid));
		INT_T *short3_rpt = (typeof(short3_rpt))malloc((short_row + 1) * sizeof(*short3_rpt));

		for (int i = 0; i < short_row; i ++)
		{
			int idx = order_rid[offset_short_row + i];
			short3_rpt[i] = ia[idx + 1] - ia[idx];
		}
		exclusive_scan(short3_rpt, short_row + 1);

		for (int i = 0; i < short_row; i ++)
		{
			int idx = order_rid[offset_short_row + i];
			memcpy(short3_val + short3_rpt[i], a + ia[idx], sizeof(ValueType) * (ia[idx + 1] - ia[idx]));
			memcpy(short3_cid + short3_rpt[i], ja + ia[idx], sizeof(int) * (ia[idx + 1] - ia[idx]));
		}
		free(short3_val);
		free(short3_cid);
		free(short3_rpt);

		// get the long part data
		INT_T *long_rpt_new = (typeof(long_rpt_new))malloc((row_long + 1) * sizeof(*long_rpt_new));
		memset(long_rpt_new, 0, sizeof(INT_T) * (row_long + 1));
		int warp_number = 0;
		for (int i = 0; i < row_long; i ++)
		{
			int nnz_num = long_rpt[i + 1] - long_rpt[i];
			int cur_warp_num = (nnz_num + MMA_M * MMA_K * loopNum_long - 1) / (MMA_M * MMA_K * loopNum_long);
			// warp_number += cur_warp_num;
			long_rpt_new[i] = cur_warp_num;
		}
		exclusive_scan(long_rpt_new, row_long + 1);
		warp_number = long_rpt_new[row_long];

		int BlockNum_long = (warp_number + warpNum_long - 1) / warpNum_long;
		int fill0_nnz_long = BlockNum_long * warpNum_long * loopNum_long * MMA_M * MMA_K;
		warp_number = BlockNum_long * warpNum_long;
		val_by_warp = (typeof(val_by_warp))malloc(warp_number * sizeof(*val_by_warp));
		rid_by_warp = (typeof(rid_by_warp))malloc(warp_number * sizeof(*rid_by_warp));
		long_val = (typeof(long_val))malloc(fill0_nnz_long * sizeof(*long_val));
		memset(long_val, 0.0, sizeof(ValueType) * fill0_nnz_long);
		long_cid = (typeof(long_cid))malloc(fill0_nnz_long * sizeof(*long_cid));
		memset(long_cid, 0, sizeof(int) * fill0_nnz_long);

		// int count_warp = 0;
		for (int i = 0; i < row_long; i ++)
		{
			ValueType *cur_val = long_val + long_rpt_new[i] * loopNum_long * MMA_M * MMA_K;
			int *cur_cid = long_cid + long_rpt_new[i] * loopNum_long * MMA_M * MMA_K;
			int real_rid = long_rid[i];
			for (int j = 0; j < long_rpt[i + 1] - long_rpt[i]; j ++)
			{
				cur_val[j] = a[ia[real_rid] + j];
				cur_cid[j] = ja[ia[real_rid] + j];
			}

			for (int j = long_rpt_new[i]; j < long_rpt_new[i + 1]; j ++)
			{
				rid_by_warp[j] = i;
			}
		}

		// preprocessing the row-block part : divide that into regular part and irregular part  
		blocknum = (row_block + BlockSize - 1) / BlockSize;
		blocknum = ((blocknum + rowloop * 4 - 1) / (rowloop * 4)) * rowloop * 4;
		blockPtr = (typeof(blockPtr))malloc((blocknum + 1) * sizeof(*blockPtr));
		memset(blockPtr, 0, sizeof(INT_T) * (blocknum + 1));

		irreg_rpt = (typeof(irreg_rpt))malloc((row_block + 1) * sizeof(*irreg_rpt));
		memset(irreg_rpt, 0, sizeof(INT_T) * (row_block + 1));

		#pragma omp parallel for
		for (int i = 0; i < blocknum; i++)
		{
			int row_start = i * BlockSize;
			int row_end = (i + 1) * BlockSize >= row_block ? row_block : (i + 1) * BlockSize;
			int k = 1;
			while(1)
			{
				int block_nnz = 0;
				for (int cur_row = row_start; cur_row < row_end; cur_row++)
				{
					int row_len = rptA[cur_row + 1] - rptA[cur_row];
					if (row_len / MMA_K >= k) block_nnz += MMA_K;
					else if(row_len / MMA_K == k - 1) block_nnz += row_len % MMA_K;
				}
				
				if (block_nnz >= threshold * MMA_K * MMA_M)
				{
					blockPtr[i] += MMA_K * MMA_M;
				}
				else
				{
					for (int cur_row = row_start; cur_row < row_end; cur_row++ )
					{
						int row_len = rptA[cur_row + 1] - rptA[cur_row];
						irreg_rpt[cur_row] = row_len - (k - 1) * MMA_K > 0 ? row_len - (k - 1) * MMA_K : 0;
					}
					break;
				}
				k++;
			}
		}
		
		exclusive_scan(blockPtr, blocknum + 1);
		exclusive_scan(irreg_rpt, row_block + 1);
		
		// int offset_row_block = row_long;
		fill0_nnz_reg = blockPtr[blocknum];
		nnz_irreg = irreg_rpt[row_block];
		origin_nnz_reg = nnz - nnz_irreg - nnz_long - nnz_short;

		// get the row-block part data---irregular part
		irreg_val = (typeof(irreg_val))malloc(nnz_irreg * sizeof(*irreg_val));
		irreg_cid = (typeof(irreg_cid))malloc(nnz_irreg * sizeof(*irreg_cid));
		for (int i = 0; i < row_block; i ++)
		{
			int cur_rid = ridA[i];
			int irreg_offset = irreg_rpt[i];
			int irreg_len = irreg_rpt[i + 1] - irreg_offset;
			for (int j = 0; j < irreg_len; j ++)
			{
				irreg_val[irreg_offset + j] = a[ia[cur_rid + 1] - irreg_len + j];
				irreg_cid[irreg_offset + j] = ja[ia[cur_rid + 1] - irreg_len + j];
			}
		}

		// get the row_block part data---regular part
		reg_val = (typeof(reg_val))malloc(fill0_nnz_reg * sizeof(*reg_val));
		reg_cid = (typeof(reg_cid))malloc(fill0_nnz_reg * sizeof(*reg_cid));

		for (int bid = 0; bid < blocknum; bid ++)
		{
			int nnz_block = (blockPtr[bid + 1] - blockPtr[bid]);
			int blocklen = nnz_block / BlockSize;

			for (int rowid = bid * BlockSize; rowid < (bid + 1) * BlockSize; rowid ++)
			{
				int regA_start = blockPtr[bid] + blocklen * (rowid - bid * BlockSize);
				if (rowid < row_block)
				{
					int real_id = ridA[rowid];
					int A_start = ia[real_id];
					int row_len = ia[real_id + 1] - A_start;
					for (int i = 0; i < blocklen; i ++)
					{
						reg_val[regA_start + i] = i < row_len ? a[A_start + i] : 0.0;
						reg_cid[regA_start + i] = i < row_len ? ja[A_start + i] : 0;
					}
				}
				else
				{
					for (int i = 0; i < blocklen; i ++)
					{
						reg_val[regA_start + i] = 0.0;
						reg_cid[regA_start + i] = 0;
					}
				}

			}

			ValueType *temp_val = (typeof(temp_val))malloc(nnz_block * sizeof(*temp_val));
			int *temp_cid = (typeof(temp_cid))malloc(nnz_block * sizeof(*temp_cid));
			ValueType *cur_val = reg_val + blockPtr[bid];
			int *cur_cid = reg_cid + blockPtr[bid];

			for (int i = 0; i < nnz_block; i ++)
			{
				int new_id = ((i % blocklen) / MMA_K) * BlockSize * MMA_K + (i / blocklen) * MMA_K + i % MMA_K;
				temp_val[new_id] = cur_val[i];
				temp_cid[new_id] = cur_cid[i];
			}
			memcpy(cur_val, temp_val, sizeof(ValueType) * nnz_block);
			memcpy(cur_cid, temp_cid, sizeof(int) * nnz_block);
			free(temp_val);
			free(temp_cid);
		}

		long fill0_nnz = fill0_nnz_short + fill0_nnz_long + nnz_irreg + fill0_nnz_reg;
		double rate_fill0 = (double)(fill0_nnz - nnz) / nnz;

		int BlockNum = (blocknum + rowloop * 4 - 1) / (rowloop * 4);
		int ThreadNum_short = warpNum_short * WARP_SIZE;
		int BlockNum_short_1 = (short_row_1 + ThreadNum_short - 1) / ThreadNum_short;
		int BlockNum_short = BlockNum_short_1 + threadblock13 + threadblock34 + threadblock22;

		offset_reg = BlockNum_long;
		offset_short1 = offset_reg + BlockNum;
		offset_short13 = offset_short1 + BlockNum_short_1;
		offset_short34 = offset_short13 + threadblock13;
		offset_short22 = offset_short34 + threadblock34;

		BlockNum_all = BlockNum_long + BlockNum + BlockNum_short;
		ThreadNum_all = 4 * WARP_SIZE;

		sumBlockNum = (row_long + 3) / 4;

		/************************************************************************************************************************************************************************/
		gpuCudaErrorCheck(cudaMalloc(&x_d, n * sizeof(*x_d)));
		gpuCudaErrorCheck(cudaMalloc(&y_d, m * sizeof(*y_d)));

		gpuCudaErrorCheck(cudaMalloc(&long_ptr_warp_d, (row_long + 1) * sizeof(*long_ptr_warp_d)));
		gpuCudaErrorCheck(cudaMalloc(&long_cid_d, fill0_nnz_long * sizeof(*long_cid_d)));
		gpuCudaErrorCheck(cudaMalloc(&long_val_d, fill0_nnz_long * sizeof(*long_val_d)));
		gpuCudaErrorCheck(cudaMalloc(&val_by_warp_d, warp_number * sizeof(*val_by_warp_d)));
		gpuCudaErrorCheck(cudaMemset(val_by_warp_d, 0, warp_number * sizeof(*val_by_warp_d)));

		gpuCudaErrorCheck(cudaMalloc(&short_cid_d, fill0_nnz_short * sizeof(*short_cid_d)));
		gpuCudaErrorCheck(cudaMalloc(&short_val_d, fill0_nnz_short * sizeof(*short_val_d)));
		
		gpuCudaErrorCheck(cudaMalloc(&block_ptr_d, (blocknum + 1) * sizeof(*block_ptr_d)));
		gpuCudaErrorCheck(cudaMalloc(&reg_cid_d, fill0_nnz_reg * sizeof(*reg_cid_d)));
		gpuCudaErrorCheck(cudaMalloc(&reg_val_d, fill0_nnz_reg * sizeof(*reg_val_d)));
		
		gpuCudaErrorCheck(cudaMalloc(&irreg_rpt_d, (row_block + 1) * sizeof(*irreg_rpt_d)));
		gpuCudaErrorCheck(cudaMalloc(&irreg_cid_d, nnz_irreg * sizeof(*irreg_cid_d)));
		gpuCudaErrorCheck(cudaMalloc(&irreg_val_d, nnz_irreg * sizeof(*irreg_val_d)));

		// cuda events for timing measurements
		gpuCudaErrorCheck(cudaEventCreate(&startEvent_execution));
		gpuCudaErrorCheck(cudaEventCreate(&endEvent_execution));
		if(TIME_IT){
			gpuCudaErrorCheck(cudaEventCreate(&startEvent_memcpy_long_val));
			gpuCudaErrorCheck(cudaEventCreate(&endEvent_memcpy_long_val));
			gpuCudaErrorCheck(cudaEventCreate(&startEvent_memcpy_val_by_warp));
			gpuCudaErrorCheck(cudaEventCreate(&endEvent_memcpy_val_by_warp));
			gpuCudaErrorCheck(cudaEventCreate(&startEvent_memcpy_long_ptr_warp));
			gpuCudaErrorCheck(cudaEventCreate(&endEvent_memcpy_long_ptr_warp));
			gpuCudaErrorCheck(cudaEventCreate(&startEvent_memcpy_long_cid));
			gpuCudaErrorCheck(cudaEventCreate(&endEvent_memcpy_long_cid));
			gpuCudaErrorCheck(cudaEventCreate(&startEvent_memcpy_short_val));
			gpuCudaErrorCheck(cudaEventCreate(&endEvent_memcpy_short_val));
			gpuCudaErrorCheck(cudaEventCreate(&startEvent_memcpy_short_cid));
			gpuCudaErrorCheck(cudaEventCreate(&endEvent_memcpy_short_cid));
			gpuCudaErrorCheck(cudaEventCreate(&startEvent_memcpy_reg_val));
			gpuCudaErrorCheck(cudaEventCreate(&endEvent_memcpy_reg_val));
			gpuCudaErrorCheck(cudaEventCreate(&startEvent_memcpy_irreg_val));
			gpuCudaErrorCheck(cudaEventCreate(&endEvent_memcpy_irreg_val));
			gpuCudaErrorCheck(cudaEventCreate(&startEvent_memcpy_block_ptr));
			gpuCudaErrorCheck(cudaEventCreate(&endEvent_memcpy_block_ptr));
			gpuCudaErrorCheck(cudaEventCreate(&startEvent_memcpy_irreg_rpt));
			gpuCudaErrorCheck(cudaEventCreate(&endEvent_memcpy_irreg_rpt));
			gpuCudaErrorCheck(cudaEventCreate(&startEvent_memcpy_reg_cid));
			gpuCudaErrorCheck(cudaEventCreate(&endEvent_memcpy_reg_cid));
			gpuCudaErrorCheck(cudaEventCreate(&startEvent_memcpy_irreg_cid));
			gpuCudaErrorCheck(cudaEventCreate(&endEvent_memcpy_irreg_cid));

			gpuCudaErrorCheck(cudaEventCreate(&startEvent_memcpy_x));
			gpuCudaErrorCheck(cudaEventCreate(&endEvent_memcpy_x));
			gpuCudaErrorCheck(cudaEventCreate(&startEvent_memcpy_y));
			gpuCudaErrorCheck(cudaEventCreate(&endEvent_memcpy_y));
		}
		
		if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(startEvent_memcpy_long_ptr_warp));
		gpuCudaErrorCheck(cudaMemcpy(long_ptr_warp_d, long_rpt_new, (row_long + 1) * sizeof(*long_ptr_warp_d), cudaMemcpyHostToDevice));
		if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(endEvent_memcpy_long_ptr_warp));

		if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(startEvent_memcpy_long_cid));
		gpuCudaErrorCheck(cudaMemcpy(long_cid_d, long_cid, fill0_nnz_long * sizeof(*long_cid_d), cudaMemcpyHostToDevice));
		if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(endEvent_memcpy_long_cid));

		if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(startEvent_memcpy_long_val));
		gpuCudaErrorCheck(cudaMemcpy(long_val_d, long_val, fill0_nnz_long * sizeof(*long_val_d), cudaMemcpyHostToDevice));
		if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(endEvent_memcpy_long_val));
		// for(int i=0;i<fill0_nnz_long;i++) printf("%d = %.2lf\n", long_cid[i], long_val[i]);
		// for(int i=0;i<(row_long+1);i++) printf("%d\n", long_rpt_new[i]);

		if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(startEvent_memcpy_short_cid));
		gpuCudaErrorCheck(cudaMemcpy(short_cid_d, short_cid, fill0_nnz_short * sizeof(*short_cid_d), cudaMemcpyHostToDevice));
		if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(endEvent_memcpy_short_cid));

		if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(startEvent_memcpy_short_val));
		gpuCudaErrorCheck(cudaMemcpy(short_val_d, short_val, fill0_nnz_short * sizeof(*short_val_d), cudaMemcpyHostToDevice));
		if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(endEvent_memcpy_short_val));
		// for(int i=0;i<fill0_nnz_short;i++) printf("%d = %.2lf\n", short_cid[i], short_val[i]);

		if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(startEvent_memcpy_block_ptr));
		gpuCudaErrorCheck(cudaMemcpy(block_ptr_d, blockPtr, (blocknum + 1) * sizeof(*block_ptr_d), cudaMemcpyHostToDevice));
		if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(endEvent_memcpy_block_ptr));

		if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(startEvent_memcpy_reg_cid));
		gpuCudaErrorCheck(cudaMemcpy(reg_cid_d, reg_cid, fill0_nnz_reg * sizeof(*reg_cid_d), cudaMemcpyHostToDevice));
		if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(endEvent_memcpy_reg_cid));

		if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(startEvent_memcpy_reg_val));
		gpuCudaErrorCheck(cudaMemcpy(reg_val_d, reg_val, fill0_nnz_reg * sizeof(*reg_val_d), cudaMemcpyHostToDevice));
		if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(endEvent_memcpy_reg_val));
		// for(int i=0;i<(blocknum + 1);i++) printf("%d\n", blockPtr[i]);
		// for(int i=0;i<fill0_nnz_reg;i++) printf("%d = %.2lf\n", reg_cid[i], reg_val[i]);

		if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(startEvent_memcpy_irreg_rpt));
		gpuCudaErrorCheck(cudaMemcpy(irreg_rpt_d, irreg_rpt, (row_block + 1) * sizeof(*irreg_rpt_d), cudaMemcpyHostToDevice));
		if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(endEvent_memcpy_irreg_rpt));

		if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(startEvent_memcpy_irreg_cid));
		gpuCudaErrorCheck(cudaMemcpy(irreg_cid_d, irreg_cid, nnz_irreg * sizeof(*irreg_cid_d), cudaMemcpyHostToDevice));
		if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(endEvent_memcpy_irreg_cid));

		if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(startEvent_memcpy_irreg_val));
		gpuCudaErrorCheck(cudaMemcpy(irreg_val_d, irreg_val, nnz_irreg * sizeof(*irreg_val_d), cudaMemcpyHostToDevice));
		if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(endEvent_memcpy_irreg_val));
		// for(int i=0;i<(row_block + 1);i++) printf("%d\n", irreg_rpt[i]);
		// for(int i=0;i<nnz_irreg;i++) printf("%d = %.2lf\n", irreg_cid[i], irreg_val[i]);

		if(TIME_IT){
			gpuCudaErrorCheck(cudaEventSynchronize(endEvent_memcpy_long_ptr_warp));
			gpuCudaErrorCheck(cudaEventSynchronize(endEvent_memcpy_long_cid));
			gpuCudaErrorCheck(cudaEventSynchronize(endEvent_memcpy_long_val));
			gpuCudaErrorCheck(cudaEventSynchronize(endEvent_memcpy_short_cid));
			gpuCudaErrorCheck(cudaEventSynchronize(endEvent_memcpy_short_val));
			gpuCudaErrorCheck(cudaEventSynchronize(endEvent_memcpy_block_ptr));
			gpuCudaErrorCheck(cudaEventSynchronize(endEvent_memcpy_reg_cid));
			gpuCudaErrorCheck(cudaEventSynchronize(endEvent_memcpy_reg_val));
			gpuCudaErrorCheck(cudaEventSynchronize(endEvent_memcpy_irreg_rpt));
			gpuCudaErrorCheck(cudaEventSynchronize(endEvent_memcpy_irreg_cid));
			gpuCudaErrorCheck(cudaEventSynchronize(endEvent_memcpy_irreg_val));
			float memcpyTime_cuda_long_ptr_warp, memcpyTime_cuda_long_cid, memcpyTime_cuda_long_val, memcpyTime_cuda_short_cid, memcpyTime_cuda_short_val, memcpyTime_cuda_block_ptr, memcpyTime_cuda_reg_cid, memcpyTime_cuda_reg_val, memcpyTime_cuda_irreg_rpt, memcpyTime_cuda_irreg_cid, memcpyTime_cuda_irreg_val;
			gpuCudaErrorCheck(cudaEventElapsedTime(&memcpyTime_cuda_long_ptr_warp, startEvent_memcpy_long_ptr_warp, endEvent_memcpy_long_ptr_warp));
			gpuCudaErrorCheck(cudaEventElapsedTime(&memcpyTime_cuda_long_cid, startEvent_memcpy_long_cid, endEvent_memcpy_long_cid));
			gpuCudaErrorCheck(cudaEventElapsedTime(&memcpyTime_cuda_long_val, startEvent_memcpy_long_val, endEvent_memcpy_long_val));
			gpuCudaErrorCheck(cudaEventElapsedTime(&memcpyTime_cuda_short_cid, startEvent_memcpy_short_cid, endEvent_memcpy_short_cid));
			gpuCudaErrorCheck(cudaEventElapsedTime(&memcpyTime_cuda_short_val, startEvent_memcpy_short_val, endEvent_memcpy_short_val));
			gpuCudaErrorCheck(cudaEventElapsedTime(&memcpyTime_cuda_block_ptr, startEvent_memcpy_block_ptr, endEvent_memcpy_block_ptr));
			gpuCudaErrorCheck(cudaEventElapsedTime(&memcpyTime_cuda_reg_cid, startEvent_memcpy_reg_cid, endEvent_memcpy_reg_cid));
			gpuCudaErrorCheck(cudaEventElapsedTime(&memcpyTime_cuda_reg_val, startEvent_memcpy_reg_val, endEvent_memcpy_reg_val));
			gpuCudaErrorCheck(cudaEventElapsedTime(&memcpyTime_cuda_irreg_rpt, startEvent_memcpy_irreg_rpt, endEvent_memcpy_irreg_rpt));
			gpuCudaErrorCheck(cudaEventElapsedTime(&memcpyTime_cuda_irreg_cid, startEvent_memcpy_irreg_cid, endEvent_memcpy_irreg_cid));
			gpuCudaErrorCheck(cudaEventElapsedTime(&memcpyTime_cuda_irreg_val, startEvent_memcpy_irreg_val, endEvent_memcpy_irreg_val));
			printf("(CUDA) Memcpy long_ptr_warp time = %.4lf ms, long_cid time = %.4lf ms, long_val time = %.4lf ms, short_cid time = %.4lf ms, short_val time = %.4lf ms, block_ptr time = %.4lf ms, reg_cid time = %.4lf ms, reg_val time = %.4lf ms, irreg_rpt time = %.4lf ms, irreg_cid time = %.4lf ms, irreg_val time = %.4lf ms\n", memcpyTime_cuda_long_ptr_warp, memcpyTime_cuda_long_cid, memcpyTime_cuda_long_val, memcpyTime_cuda_short_cid, memcpyTime_cuda_short_val, memcpyTime_cuda_block_ptr, memcpyTime_cuda_reg_cid, memcpyTime_cuda_reg_val, memcpyTime_cuda_irreg_rpt, memcpyTime_cuda_irreg_cid, memcpyTime_cuda_irreg_val);
		}
		printf("row_long = %d, row_block = %d, blocknum = %d, short_row_1 = %d, common_13 = %d, short_row_34 = %d, short_row_2 = %d, offset_reg = %d, offset_short1 = %d, offset_short13 = %d, offset_short34 = %d, offset_short22 = %d, fill0_nnz_short13 = %d, fill0_nnz_short34 = %d\n", row_long, row_block, blocknum, short_row_1, common_13, short_row_34, short_row_2, offset_reg, offset_short1, offset_short13, offset_short34, offset_short22, fill0_nnz_short13, fill0_nnz_short34);
	}

	~DASPArrays()
	{
		free(ia);
		free(ja);
		free(a);
		gpuCudaErrorCheck(cudaFree(x_d));
		gpuCudaErrorCheck(cudaFree(y_d));

		gpuCudaErrorCheck(cudaFree(long_ptr_warp_d));
		gpuCudaErrorCheck(cudaFree(long_cid_d));
		gpuCudaErrorCheck(cudaFree(long_val_d));
		gpuCudaErrorCheck(cudaFree(val_by_warp_d));

		gpuCudaErrorCheck(cudaFree(short_val_d));
		gpuCudaErrorCheck(cudaFree(short_cid_d));
		
		gpuCudaErrorCheck(cudaFree(block_ptr_d));
		gpuCudaErrorCheck(cudaFree(reg_cid_d));
		gpuCudaErrorCheck(cudaFree(reg_val_d));
		
		gpuCudaErrorCheck(cudaFree(irreg_rpt_d));
		gpuCudaErrorCheck(cudaFree(irreg_cid_d));
		gpuCudaErrorCheck(cudaFree(irreg_val_d));

		// DASP specific
		free(order_rid);
		free(short_rid_1);
		free(short_rid_2);
		free(short_rid_3);
		free(short_rid_4);
		free(long_rid);
		free(zero_rid);
		free(ridA);

		free(rptA);
		free(long_rpt);

		free(short_val);
		free(short_cid);

		free(long_cid);
		free(long_val);
		free(long_rpt_new);
		free(val_by_warp);
		free(rid_by_warp);

		free(reg_val);
		free(reg_cid);
		free(blockPtr);

		free(irreg_rpt);
		free(irreg_cid);
		free(irreg_val);
	}

	void spmv(ValueType * x, ValueType * y);
	void statistics_start();
	int statistics_print_data(__attribute__((unused)) char * buf, __attribute__((unused)) long buf_n);
};


void compute_dasp(DASPArrays * dasp, ValueType * x , ValueType * y);


void
DASPArrays::spmv(ValueType * x, ValueType * y)
{
	compute_dasp(this, x, y);
}


struct Matrix_Format *
csr_to_format(INT_T * row_ptr, INT_T * col_ind, ValueType * values, long m, long n, long nnz)
{

	struct DASPArrays * dasp = new DASPArrays(row_ptr, col_ind, values, m, n, nnz);
	//dasp->mem_footprint = ; TODO!
	dasp->format_name = (char *) "DASP_CUDA";
	return dasp;
}


__host__ void
compute_dasp(DASPArrays * dasp, ValueType * x , ValueType * y)
{
	if (dasp->x == NULL)
	{
		dasp->x = x;
		if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(dasp->startEvent_memcpy_x));
		gpuCudaErrorCheck(cudaMemcpy(dasp->x_d, x, dasp->n * sizeof(ValueType), cudaMemcpyHostToDevice));
		if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(dasp->endEvent_memcpy_x));
		if(TIME_IT){
			gpuCudaErrorCheck(cudaEventSynchronize(dasp->endEvent_memcpy_x));
			float memcpyTime_cuda;
			gpuCudaErrorCheck(cudaEventElapsedTime(&memcpyTime_cuda, dasp->startEvent_memcpy_x, dasp->endEvent_memcpy_x));
			printf("(CUDA) Memcpy x time = %.4lf ms\n", memcpyTime_cuda);
		}
	}

	cudaMemset(dasp->y_d, 0, dasp->m * sizeof(dasp->y_d));

	int carveout = 0;
	gpuCudaErrorCheck(cudaFuncSetAttribute(dasp_spmv2<1>, cudaFuncAttributePreferredSharedMemoryCarveout, carveout));
	gpuCudaErrorCheck(cudaFuncSetAttribute(dasp_spmv2<2>, cudaFuncAttributePreferredSharedMemoryCarveout, carveout));
	gpuCudaErrorCheck(cudaFuncSetAttribute(dasp_spmv2<4>, cudaFuncAttributePreferredSharedMemoryCarveout, carveout));

	if (dasp->rowloop == 1) {
		dasp_spmv2<1><<<dasp->BlockNum_all, dasp->ThreadNum_all>>>(
			dasp->x_d, dasp->y_d,
			dasp->long_val_d, dasp->long_cid_d, dasp->val_by_warp_d, dasp->long_ptr_warp_d, dasp->row_long,
			dasp->reg_val_d, dasp->reg_cid_d, dasp->block_ptr_d, dasp->row_block, dasp->blocknum,
			dasp->irreg_val_d, dasp->irreg_cid_d, dasp->irreg_rpt_d,
			dasp->short_val_d, dasp->short_cid_d, dasp->short_row_1, dasp->common_13, dasp->short_row_34, dasp->short_row_2,
			dasp->offset_reg, dasp->offset_short1, dasp->offset_short13, dasp->offset_short34, dasp->offset_short22,
			dasp->fill0_nnz_short13, dasp->fill0_nnz_short34);
	}
	else if (dasp->rowloop == 2) {
		dasp_spmv2<2><<<dasp->BlockNum_all, dasp->ThreadNum_all>>>(
			dasp->x_d, dasp->y_d,
			dasp->long_val_d, dasp->long_cid_d, dasp->val_by_warp_d, dasp->long_ptr_warp_d, dasp->row_long,
			dasp->reg_val_d, dasp->reg_cid_d, dasp->block_ptr_d, dasp->row_block, dasp->blocknum,
			dasp->irreg_val_d, dasp->irreg_cid_d, dasp->irreg_rpt_d,
			dasp->short_val_d, dasp->short_cid_d, dasp->short_row_1, dasp->common_13, dasp->short_row_34, dasp->short_row_2,
			dasp->offset_reg, dasp->offset_short1, dasp->offset_short13, dasp->offset_short34, dasp->offset_short22,
			dasp->fill0_nnz_short13, dasp->fill0_nnz_short34);
	}
	else {
		dasp_spmv2<4><<<dasp->BlockNum_all, dasp->ThreadNum_all>>>(
			dasp->x_d, dasp->y_d,
			dasp->long_val_d, dasp->long_cid_d, dasp->val_by_warp_d, dasp->long_ptr_warp_d, dasp->row_long,
			dasp->reg_val_d, dasp->reg_cid_d, dasp->block_ptr_d, dasp->row_block, dasp->blocknum,
			dasp->irreg_val_d, dasp->irreg_cid_d, dasp->irreg_rpt_d,
			dasp->short_val_d, dasp->short_cid_d, dasp->short_row_1, dasp->common_13, dasp->short_row_34, dasp->short_row_2,
			dasp->offset_reg, dasp->offset_short1, dasp->offset_short13, dasp->offset_short34, dasp->offset_short22,
			dasp->fill0_nnz_short13, dasp->fill0_nnz_short34);
	}
	gpuCudaErrorCheck(cudaPeekAtLastError());
	gpuCudaErrorCheck(cudaDeviceSynchronize());
	if(dasp->row_long)
		longPart_sum<<<dasp->sumBlockNum, dasp->ThreadNum_all>>>(dasp->long_ptr_warp_d, dasp->val_by_warp_d, dasp->y_d, dasp->row_long);
	gpuCudaErrorCheck(cudaPeekAtLastError());
	gpuCudaErrorCheck(cudaDeviceSynchronize());

	if (dasp->y == NULL)
	{
		dasp->y = y;
		if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(dasp->startEvent_memcpy_y));
		ValueType *y_reordered = (typeof(y_reordered))malloc(dasp->m * sizeof(*y_reordered));
		gpuCudaErrorCheck(cudaMemcpy(y_reordered, dasp->y_d, dasp->m * sizeof(ValueType), cudaMemcpyDeviceToHost));		
		// Need to perform reordering to result, apart from Copying it back to CPU
		for(INT_T i=0; i<dasp->m; i++)
			dasp->y[dasp->order_rid[i]] = y_reordered[i];
		free(y_reordered);
		if(TIME_IT) gpuCudaErrorCheck(cudaEventRecord(dasp->endEvent_memcpy_y));
		
		if(TIME_IT){
			gpuCudaErrorCheck(cudaEventSynchronize(dasp->endEvent_memcpy_y));
			float memcpyTime_cuda;
			gpuCudaErrorCheck(cudaEventElapsedTime(&memcpyTime_cuda, dasp->startEvent_memcpy_y, dasp->endEvent_memcpy_y));
			printf("(CUDA) Memcpy y time = %.4lf ms\n", memcpyTime_cuda);
		}

	}

}


//==========================================================================================================================================
//= Print Statistics
//==========================================================================================================================================


void
DASPArrays::statistics_start()
{
}


int
statistics_print_labels(__attribute__((unused)) char * buf, __attribute__((unused)) long buf_n)
{
	return 0;
}


int
DASPArrays::statistics_print_data(__attribute__((unused)) char * buf, __attribute__((unused)) long buf_n)
{
	return 0;
}

