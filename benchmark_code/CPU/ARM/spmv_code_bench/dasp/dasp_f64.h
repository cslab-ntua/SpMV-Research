/*
	第一版整合版本，统一一个线程块4个warp
	long part： 一个warp进行两次mma，多个block得到一个y
	row_block part：
		根据row-block行数进行分情况讨论
		row-block < 59990                        一个warp算1个行块
		row-block >= 59990 && row-block < 400000 一个warp算2个行块
		row-block >= 400000                      一个warp算4个行块
		
	short part：一个warp进行四次mma，一个warp得到32个y

	增加 bypass L1 cache
*/

#include "common.h"
#include "utils.h"

#define groupNum 1
#define warpNum_short 4
#define loopNum_short 4
#define warpNum_long 4
#define loopNum_long 2


__device__ __forceinline__ MAT_VAL_TYPE warpReduceSum(MAT_VAL_TYPE sum){
	sum += __shfl_down_sync(0xffffffff,sum,16);
	sum += __shfl_down_sync(0xffffffff,sum,8);
	sum += __shfl_down_sync(0xffffffff,sum,4);
	sum += __shfl_down_sync(0xffffffff,sum,2);
	sum += __shfl_down_sync(0xffffffff,sum,1);
	return sum;
}

__device__ __forceinline__ MAT_VAL_TYPE load_double_from_global(const MAT_VAL_TYPE* a)
{
	MAT_VAL_TYPE r;
	asm volatile("ld.global.cs.f64 %0, [%1];" : "=d"(r) : "l"(a));
	return r;
}

__device__ __forceinline__ void store_double_to_global(const MAT_VAL_TYPE* a, MAT_VAL_TYPE v)
{
	asm volatile("st.global.cs.f64 [%0], %1;" :: "l"(a), "d"(v));
}

__device__ __forceinline__ int load_int_from_global(const int* a)
{
	int r;
	asm volatile("ld.global.cs.s32 %0, [%1];" : "=r"(r) : "l"(a));
	return r;
}

__global__ void longPart_sum(int *dlongA_rpt, MAT_VAL_TYPE *dwarp_val, MAT_VAL_TYPE *dY_val, int row_long)
{
	int bid = blockIdx.x;
	int tid = threadIdx.x;
	int laneid = 31 & tid;
	int global_warpid = bid * warpNum_long + (tid >> 5);

	if (global_warpid >= row_long) return;

	int offset_longA = load_int_from_global(dlongA_rpt + global_warpid);
	MAT_VAL_TYPE *cur_temp_val = dwarp_val + offset_longA;
	int len = load_int_from_global(dlongA_rpt + global_warpid + 1) - offset_longA;

	MAT_VAL_TYPE thread_val = 0;
	for (int i = laneid; i < len; i += WARP_SIZE)
	{
		thread_val += load_double_from_global(cur_temp_val + i);
	}
	thread_val = warpReduceSum(thread_val);

	if (laneid == 0)
		store_double_to_global(dY_val + global_warpid, thread_val);
}

template <int rowloop>  // this parameter must be 1 or 2 or 4
__global__ void dasp_spmv2(MAT_VAL_TYPE *dX_val, MAT_VAL_TYPE *dY_val,
						  MAT_VAL_TYPE *dlongA_val, int *dlongA_cid, MAT_VAL_TYPE *dwarp_val, MAT_PTR_TYPE *dlongA_rpt, int row_long,
						  MAT_VAL_TYPE *dregA_val, int *dregA_cid, MAT_PTR_TYPE *dblockA_ptr, int row_block, int blocknum, 
						  MAT_VAL_TYPE *dirregA_val, int *dirregA_cid, MAT_PTR_TYPE *dirregA_rpt,
						  MAT_VAL_TYPE *dshort_val, int *dshort_cid, int short_row_1, int common_13, int short_row_34, int short_row_2,
						  int offset_reg, int offset_short1, int offset_short13, int offset_short34, int offset_short22,
						  MAT_PTR_TYPE fill0_nnz_short13, MAT_PTR_TYPE fill0_nnz_short34)
{
	int bid = blockIdx.x;
	int tid = threadIdx.x;
	int laneid = 31 & tid;
 
	if (bid < offset_reg)
	{
		// long part
		int global_warpid = bid * warpNum_long + (tid >> 5);
		int offset = global_warpid * loopNum_long * MMA_M * MMA_K;
		MAT_VAL_TYPE *curA_val = dlongA_val + offset;
		int *curA_cid = dlongA_cid + offset;

		int groupID = laneid >> 2;
		int tID_in_group = 3 & laneid;

		MAT_VAL_TYPE fragA, fragB;
		MAT_VAL_TYPE fragC[2];
		fragC[0] = 0.0, fragC[1] = 0.0;

		int idx = tID_in_group + groupID * MMA_K;
		
		#pragma unroll
		for (int i = 0; i < loopNum_long; i++)
		{
			fragA = load_double_from_global(curA_val + idx);
			int x_idx = load_int_from_global(curA_cid + idx);
			fragB = dX_val[x_idx];

			mma_m8n8k4(fragC, fragA, fragB);
			idx += 32; // MMA_M * MMA_K
		}

		fragC[0] += __shfl_down_sync(0xffffffff, fragC[0], 9, 32);
		fragC[0] += __shfl_down_sync(0xffffffff, fragC[0], 18, 32);
		fragC[1] += __shfl_down_sync(0xffffffff, fragC[1], 9, 32);
		fragC[1] += __shfl_down_sync(0xffffffff, fragC[1], 18, 32);
		fragC[0] += __shfl_sync(0xffffffff, fragC[1], 4);

		if (laneid == 0) 
			store_double_to_global(dwarp_val + global_warpid, fragC[0]);

		// if (global_warpid >= row_long) return;

		// // MAT_VAL_TYPE *cur_temp_val = dwarp_val + dlongA_rpt[global_warpid];
		// int offset_longA = load_int_from_global(dlongA_rpt + global_warpid);
		// MAT_VAL_TYPE *cur_temp_val = dwarp_val + offset_longA;
		// int len = load_int_from_global(dlongA_rpt + global_warpid + 1) - offset_longA;

		// MAT_VAL_TYPE thread_val = 0;
		// for (int i = laneid; i < len; i += WARP_SIZE)
		// {
		// 	thread_val += load_double_from_global(cur_temp_val + i); // PROBLEM WITH SANITIZER
		// }
		// thread_val = warpReduceSum(thread_val);

		// if (laneid == 0)
		// 	store_double_to_global(dY_val + global_warpid, thread_val);


	}
	else if (bid >= offset_reg && bid < offset_short1)
	{
		// row-block part
		int bid_reg = bid - offset_reg;
		int warp_local = tid >> 5;

		int groupID = laneid >> 2;
		int tID_in_group = 3 & laneid;
		MAT_VAL_TYPE fragA, fragB, fragC[2];

		if (rowloop == 1)
		{
			int block_idx = bid_reg * 4 + warp_local;
			// int offset_A = dblockA_ptr[block_idx];
			int offset_A = load_int_from_global(dblockA_ptr + block_idx);
			int blocklen = (load_int_from_global(dblockA_ptr + block_idx + 1) - offset_A) >> 3;

			if (block_idx >= blocknum) return;

			MAT_VAL_TYPE *curA_val = dregA_val + offset_A;
			int *curA_cid = dregA_cid + offset_A;

			fragC[0] = 0.0, fragC[1] = 0.0;
			int idx = tID_in_group + groupID * MMA_K;
			for (int i = 0; i < blocklen; i += MMA_K)
			{
				fragA = load_double_from_global(curA_val + idx);
				int x_idx = load_int_from_global(curA_cid + idx);
				fragB = dX_val[x_idx];
				mma_m8n8k4(fragC, fragA, fragB);
				idx += 32;
			}

			int offset_y = block_idx * BlockSize + groupID;
			if (tID_in_group == (groupID >> 1) && offset_y < row_block)
			{
				store_double_to_global(dY_val + row_long + offset_y, fragC[1 & groupID]);
			}

			int cur_row = block_idx * BlockSize + laneid;
			if (laneid < BlockSize && cur_row < row_block)
			{
				MAT_VAL_TYPE cur_y = 0.0;
				// for (int i = dirregA_rpt[cur_row]; i < dirregA_rpt[cur_row + 1]; i ++)
				for (int i = dirregA_rpt[cur_row]; i < dirregA_rpt[cur_row + 1]; i ++)
				{
					cur_y += load_double_from_global(dirregA_val + i) * dX_val[dirregA_cid[i]];
				}
				cur_y += load_double_from_global(dY_val + row_long + cur_row);
				store_double_to_global(dY_val + row_long + cur_row, cur_y);
			}
		}

		if (rowloop == 2)
		{
			MAT_VAL_TYPE res;
			#pragma unroll
			for (int i = 0; i < 2; i ++)
			{
				int block_idx = bid_reg * 8 + warp_local * 2 + i;
				int offset_A = load_int_from_global(dblockA_ptr + block_idx);
				int blocklen = (load_int_from_global(dblockA_ptr + block_idx + 1) - offset_A) >> 3;
				// int offset_A = dblockA_ptr[block_idx];
				// int blocklen = (dblockA_ptr[block_idx + 1] - offset_A) >> 3;

				MAT_VAL_TYPE *curA_val = dregA_val + offset_A;
				int *curA_cid = dregA_cid + offset_A;

				fragC[0] = 0.0, fragC[1] = 0.0;
				int idx = tID_in_group + groupID * MMA_K;
				for (int j = 0; j < blocklen; j += MMA_K)
				{
					fragA = load_double_from_global(curA_val + idx);
					int x_idx = load_int_from_global(curA_cid + idx);
					fragB = dX_val[x_idx];
					mma_m8n8k4(fragC, fragA, fragB);
					idx += 32;
				}
				int target_id = ((laneid - i * 8) >> 1) * 9;
				fragC[0] = __shfl_sync(0xffffffff, fragC[0], target_id);
				fragC[1] = __shfl_sync(0xffffffff, fragC[1], target_id + 4);
				if ((laneid >> 3) == i) res = (1 & laneid) == 0 ? fragC[0] : fragC[1];
			}

			int cur_row = bid_reg * 8 * BlockSize + warp_local * 2 * BlockSize + laneid;
			if (laneid < 16 && cur_row < row_block)
			{
				for (int i = dirregA_rpt[cur_row]; i < dirregA_rpt[cur_row + 1]; i ++)
				{
					res += load_double_from_global(dirregA_val + i) * dX_val[dirregA_cid[i]];
				}
				store_double_to_global(dY_val + row_long + cur_row, res);
			}
		}

		if (rowloop == 4)
		{
			MAT_VAL_TYPE res;
			#pragma unroll
			for (int i = 0; i < 4; i ++)
			{
				int block_idx = bid_reg * 16 + warp_local * 4 + i;
				// int offset_A = dblockA_ptr[block_idx];
				// int blocklen = (dblockA_ptr[block_idx + 1] - offset_A) >> 3;
				int offset_A = load_int_from_global(dblockA_ptr + block_idx);
				int blocklen = (load_int_from_global(dblockA_ptr + block_idx + 1) - offset_A) >> 3;

				MAT_VAL_TYPE *curA_val = dregA_val + offset_A;
				int *curA_cid = dregA_cid + offset_A;

				fragC[0] = 0.0, fragC[1] = 0.0;
				int idx = tID_in_group + groupID * MMA_K;
				for (int j = 0; j < blocklen; j += MMA_K)
				{
					fragA = load_double_from_global(curA_val + idx);
					int x_idx = load_int_from_global(curA_cid + idx);
					fragB = dX_val[x_idx];
					mma_m8n8k4(fragC, fragA, fragB);
					idx += 32;
				}
				int target_id = ((laneid - i * 8) >> 1) * 9;
				fragC[0] = __shfl_sync(0xffffffff, fragC[0], target_id);
				fragC[1] = __shfl_sync(0xffffffff, fragC[1], target_id + 4);
				if ((laneid >> 3) == i) res = (1 & laneid) == 0 ? fragC[0] : fragC[1];
			}
			int cur_row = bid_reg * 16 * BlockSize + warp_local * 4 * BlockSize + laneid;
			if (cur_row < row_block)
			{
				for (int i = dirregA_rpt[cur_row]; i < dirregA_rpt[cur_row + 1]; i ++)
				{
					res += load_double_from_global(dirregA_val + i) * dX_val[dirregA_cid[i]];
				}
				store_double_to_global(dY_val + row_long + cur_row, res);
			}
		}
	}
	else if (bid >= offset_short1 && bid < offset_short13)
	{
		// short part - 1 nnz/row
		int bid1 = bid - offset_short1;
		int tid1 = bid1 * blockDim.x + tid;
		if (tid1 >= short_row_1)
		{
			return;
		}
		
		int x_idx = load_int_from_global(dshort_cid + tid1);
		MAT_VAL_TYPE temp_y = load_double_from_global(dshort_val + tid1) * dX_val[x_idx];
		store_double_to_global(dY_val + row_long + row_block + tid1, temp_y);

	}
	else if (bid >= offset_short13 && bid < offset_short34)
	{
		// short part - block 1&3
		int warpid_local = tid >> 5;
		int bid13 = bid - offset_short13;

		MAT_VAL_TYPE fragA = 0.0, fragB = 0.0, fragC[2], res;
		int groupID = laneid >> 2;
		int tID_in_group = 3 & laneid;

		#pragma unroll
		for (int i = 0; i < groupNum; i ++)
		{
			int offset = short_row_1 + ((bid13 * groupNum + i) * warpNum_short + warpid_local) * MMA_M * MMA_K * 2;
			MAT_VAL_TYPE *cur_val = dshort_val + offset;
			int *cur_cid = dshort_cid + offset;
			int idx = tID_in_group + groupID * MMA_K;
			
			fragC[0] = 0.0, fragC[1] = 0.0;
			fragA = load_double_from_global(cur_val + idx);
			int cid = load_int_from_global(cur_cid + idx);
			fragB = tID_in_group == 0 ? dX_val[cid] : 0;
			mma_m8n8k4(fragC, fragA, fragB);
			int target_id = (laneid >> 1) * 9;
			fragC[0] = __shfl_sync(0xffffffff, fragC[0], target_id);
			fragC[1] = __shfl_sync(0xffffffff, fragC[1], target_id + 4);
			if (laneid < 8) res = (1 & laneid) == 0 ? fragC[0] : fragC[1];

			fragC[0] = 0.0, fragC[1] = 0.0;
			fragB = tID_in_group == 0 ? 0 : dX_val[cid];
			mma_m8n8k4(fragC, fragA, fragB);
			target_id = ((laneid - 8) >> 1) * 9;
			fragC[0] = __shfl_sync(0xffffffff, fragC[0], target_id);
			fragC[1] = __shfl_sync(0xffffffff, fragC[1], target_id + 4);
			if (laneid >> 3 == 1) res = (1 & laneid) == 0 ? fragC[0] : fragC[1];
			idx += 32;

			fragC[0] = 0.0, fragC[1] = 0.0;
			fragA = load_double_from_global(cur_val + idx);
			cid = load_int_from_global(cur_cid + idx);
			fragB = tID_in_group == 0 ? dX_val[cid] : 0;
			mma_m8n8k4(fragC, fragA, fragB);
			target_id = ((laneid - 16) >> 1) * 9;
			fragC[0] = __shfl_sync(0xffffffff, fragC[0], target_id);
			fragC[1] = __shfl_sync(0xffffffff, fragC[1], target_id + 4);
			if (laneid >> 3 == 2) res = (1 & laneid) == 0 ? fragC[0] : fragC[1];

			fragC[0] = 0.0, fragC[1] = 0.0;
			fragB = tID_in_group == 0 ? 0 : dX_val[cid];
			mma_m8n8k4(fragC, fragA, fragB);
			target_id = ((laneid - 24) >> 1) * 9;
			fragC[0] = __shfl_sync(0xffffffff, fragC[0], target_id);
			fragC[1] = __shfl_sync(0xffffffff, fragC[1], target_id + 4);
			if (laneid >= 24) res = (1 & laneid) == 0 ? fragC[0] : fragC[1];

			int offset_y = ((bid13 * groupNum + i) * warpNum_short  + warpid_local) * WARP_SIZE + laneid;
			if (offset_y < common_13 * 2) 
				store_double_to_global(dY_val + row_long + row_block + short_row_1 + offset_y, res);

		}
	}
	else if (bid >= offset_short34 && bid < offset_short22)
	{
		// short part - block3 & block4
		int warpid_local = tid >> 5;
		int bid34 = bid - offset_short34;

		MAT_VAL_TYPE fragA = 0.0, fragB = 0.0, fragC[2], res;
		int groupID = laneid >> 2;
		int tID_in_group = 3 & laneid;

		#pragma unroll
		for (int j = 0; j < groupNum; j ++)
		{
			int offset = short_row_1 + fill0_nnz_short13 + ((bid34 * groupNum + j) * warpNum_short + warpid_local) * MMA_M * MMA_K * loopNum_short;
			MAT_VAL_TYPE *cur_val = dshort_val + offset;
			int *cur_cid = dshort_cid + offset;
			int idx = tID_in_group + groupID * MMA_K;

			fragC[0] = 0.0, fragC[1] = 0.0;
			fragA = load_double_from_global(cur_val + idx);
			int cid = load_int_from_global(cur_cid + idx);
			fragB = dX_val[cid];
			mma_m8n8k4(fragC, fragA, fragB);
			int target_id = (laneid >> 1) * 9;
			fragC[0] = __shfl_sync(0xffffffff, fragC[0], target_id);
			fragC[1] = __shfl_sync(0xffffffff, fragC[1], target_id + 4);
			if (laneid < 8) res = (1 & laneid) == 0 ? fragC[0] : fragC[1];
			idx += 32;

			fragC[0] = 0.0, fragC[1] = 0.0;
			fragA = load_double_from_global(cur_val + idx);
			cid = load_int_from_global(cur_cid + idx);
			fragB = dX_val[cid];
			mma_m8n8k4(fragC, fragA, fragB);
			target_id = ((laneid - 8) >> 1) * 9;
			fragC[0] = __shfl_sync(0xffffffff, fragC[0], target_id);
			fragC[1] = __shfl_sync(0xffffffff, fragC[1], target_id + 4);
			if (laneid >> 3 == 1) res = (1 & laneid) == 0 ? fragC[0] : fragC[1];
			idx += 32;

			fragC[0] = 0.0, fragC[1] = 0.0;
			fragA = load_double_from_global(cur_val + idx);
			cid = load_int_from_global(cur_cid + idx);
			fragB = dX_val[cid];
			mma_m8n8k4(fragC, fragA, fragB);
			target_id = ((laneid - 16) >> 1) * 9;
			fragC[0] = __shfl_sync(0xffffffff, fragC[0], target_id);
			fragC[1] = __shfl_sync(0xffffffff, fragC[1], target_id + 4);
			if (laneid >> 3 == 2) res = (1 & laneid) == 0 ? fragC[0] : fragC[1];
			idx += 32;

			fragC[0] = 0.0, fragC[1] = 0.0;
			fragA = load_double_from_global(cur_val + idx);
			cid = load_int_from_global(cur_cid + idx);
			fragB = dX_val[cid];
			mma_m8n8k4(fragC, fragA, fragB);
			target_id = ((laneid - 24) >> 1) * 9;
			fragC[0] = __shfl_sync(0xffffffff, fragC[0], target_id);
			fragC[1] = __shfl_sync(0xffffffff, fragC[1], target_id + 4);
			if (laneid >= 24) res = (1 & laneid) == 0 ? fragC[0] : fragC[1];
			
			int offset_y = ((bid34 * groupNum + j) * warpNum_short + warpid_local) * WARP_SIZE + laneid;
			if (offset_y < short_row_34) 
				store_double_to_global(dY_val + row_long + row_block + short_row_1 + common_13 * 2 + offset_y, res);

		}
	}
	else
	{
		// short part - blocl 2&2
		int warpid_local = tid >> 5;
		int bid22 = bid - offset_short22;

		MAT_VAL_TYPE fragA = 0.0, fragB = 0.0, fragC[2], res;
		int groupID = laneid >> 2;
		int tID_in_group = 3 & laneid;
		
		#pragma unroll
		for (int i = 0; i < groupNum; i ++)
		{
			int offset = short_row_1 + fill0_nnz_short13 + fill0_nnz_short34 + ((bid22 * groupNum + i) * warpNum_short + warpid_local) * MMA_M * MMA_K * 2;
			MAT_VAL_TYPE *cur_val = dshort_val + offset;
			int *cur_cid = dshort_cid + offset;
			int idx = tID_in_group + groupID * MMA_K;

			fragC[0] = 0.0, fragC[1] = 0.0;
			fragA = load_double_from_global(cur_val + idx);
			int cid = load_int_from_global(cur_cid + idx);
			fragB = tID_in_group < 2 ? dX_val[cid] : 0;
			mma_m8n8k4(fragC, fragA, fragB);
			int target_id = (laneid >> 1) * 9;
			fragC[0] = __shfl_sync(0xffffffff, fragC[0], target_id);
			fragC[1] = __shfl_sync(0xffffffff, fragC[1], target_id + 4);
			if (laneid < 8) res = (1 & laneid) == 0 ? fragC[0] : fragC[1];

			fragC[0] = 0.0, fragC[1] = 0.0;
			fragB = tID_in_group < 2 ? 0 : dX_val[cid];
			mma_m8n8k4(fragC, fragA, fragB);
			target_id = ((laneid - 8) >> 1) * 9;
			fragC[0] = __shfl_sync(0xffffffff, fragC[0], target_id);
			fragC[1] = __shfl_sync(0xffffffff, fragC[1], target_id + 4);
			if (laneid >> 3 == 1) res = (1 & laneid) == 0 ? fragC[0] : fragC[1];
			idx += 32;

			fragC[0] = 0.0, fragC[1] = 0.0;
			fragA = load_double_from_global(cur_val + idx);
			cid = load_int_from_global(cur_cid + idx);
			fragB = tID_in_group < 2 ? dX_val[cid] : 0;
			mma_m8n8k4(fragC, fragA, fragB);
			target_id = ((laneid - 16) >> 1) * 9;
			fragC[0] = __shfl_sync(0xffffffff, fragC[0], target_id);
			fragC[1] = __shfl_sync(0xffffffff, fragC[1], target_id + 4);
			if (laneid >> 3 == 2) res = (1 & laneid) == 0 ? fragC[0] : fragC[1];

			fragC[0] = 0.0, fragC[1] = 0.0;
			fragB = tID_in_group < 2 ? 0 : dX_val[cid];
			mma_m8n8k4(fragC, fragA, fragB);
			target_id = ((laneid - 24) >> 1) * 9;
			fragC[0] = __shfl_sync(0xffffffff, fragC[0], target_id);
			fragC[1] = __shfl_sync(0xffffffff, fragC[1], target_id + 4);
			if (laneid >= 24) res = (1 & laneid) == 0 ? fragC[0] : fragC[1];

			int offset_y = ((bid22 * groupNum + i) * warpNum_short + warpid_local) * WARP_SIZE + laneid;
			if (offset_y < short_row_2) 
				store_double_to_global(dY_val + row_long + row_block + short_row_1 + common_13 * 2 + short_row_34 + offset_y, res);
		}
	}
}

