/*
 * =====================================================================================
 *
 *       Filename:  PatternMatching.cpp
 *
 *    Description:  Pattern matching code for trace analysis
 *
 *        Version:  1.0
 *        Created:  2021-07-05 05:50:43 PM
 *       Revision:  1.0
 *       Compiler:  gcc
 *
 *         Author:  Zachary Cetinic,
 *   Organization:
 *
 * =====================================================================================
 */
#include "PatternMatching.h"
#include "DDT.h"
#include "DDTDef.h"
#include "DDTUtils.h"

#include <immintrin.h>

#include <chrono>


int UNIT_THRESHOLDS[4] = {2, 2, 2, 2};

const int TPD = 3;
int ZERO_MASK = 0x0FFF;
int FSC_MASK = 0x0FFF;
int PSC1_MASK = 0x0F0F;
int PSC2_MASK = 0x0FFF;

__m128i unitThresholds = _mm_set_epi32(UNIT_THRESHOLDS[3], UNIT_THRESHOLDS[2],
		UNIT_THRESHOLDS[1], UNIT_THRESHOLDS[0]);

namespace DDT {
	/**
	 * @brief Generate parallel first order differences
	 *
	 * @param ip Array of pointers to the start tuple of each iteration
	 * @param ips Size of the ip array
	 * @param tuples Array of padded tuples
	 * @param differences Array of blank memory to store diffences
	 *
	 */
	void computeParallelizedFOD(int **ip, int ips, int *differences,
			int nThreads) {
#pragma omp parallel for num_threads(nThreads)
		for (int i = 0; i < ips; i++) {
			computeFirstOrder(differences + (ip[i] - ip[0]), ip[i],
					ip[i + 1] - ip[i]);
		}
	}

	/**
	 * @brief Compute FODs using iteration thread bounds stored in tb
	 * @param tb Array of iteration bounds for each thread
	 * @param ip
	 * @param ips
	 * @param differences
	 * @param nThreads
	 */
	void computeThreadBoundParallelizedFOD(int* tb, int **ip, int ips, int *differences,
			int nThreads) {
#pragma omp parallel for num_threads(nThreads)
		for (int t = 0; t < nThreads; t++) {
			for (int i = tb[t]; i < tb[t+1]-1; i++) {
				computeFirstOrder(differences + (ip[i] - ip[0]), ip[i],
						ip[i + 1] - ip[i]);
			}
		}
	}

	/**
	 * Generate first order differences for tuples.
	 *
	 * @param differences Memory address to store differences
	 * @param tuples Pointer to tuples which must be padded
	 * to lengths of four
	 * @param numTuples Number of tuples to process
	 */
	void computeFirstOrder(int *differences, int *tuples, int numTuples) {
		int i = 0;
		int to = 3;// Tuple offset

		for (; i < numTuples - to * 4; i += 8) {
			__m256i lhs = _mm256_loadu_si256(
					reinterpret_cast<const __m256i *>(tuples + i));
			__m256i rhs = _mm256_loadu_si256(
					reinterpret_cast<const __m256i *>(tuples + i + to));
			__m256i fod = _mm256_sub_epi32(rhs, lhs);
			_mm256_storeu_si256(reinterpret_cast<__m256i *>(differences + i),
					fod);
		}
		for (; i < numTuples - to * 2; i += 4) {
			__m128i lhs = _mm_loadu_si128(
					reinterpret_cast<const __m128i *>(tuples + i));
			__m128i rhs = _mm_loadu_si128(
					reinterpret_cast<const __m128i *>(tuples + i + to));
			__m128i fod = _mm_sub_epi32(rhs, lhs);
			_mm_storeu_si128(reinterpret_cast<__m128i *>(differences + i), fod);
		}
		for (; i < numTuples - to; i++) {
			differences[i] = *(tuples + i + to) - *(tuples + i);
		}
	}

	/**
	 * @brief Generates codelets from differences and tuple information within
	 * bounds found in pruning.
	 *
	 * @param d Global memory object containing all pointers and information
	 * required for mining
	 */
	void minePrunedDifferences(DDT::GlobalObject& d) {
		int rd = 2;

		for (int ib = 0; ib < d.ipbs; ++ib) {
			auto type = d.ipbt[ib];

			if (d.ipb[ib+1] - d.ipb[ib] == 1)
				continue;

			// Normal Search
			if (type == 0) {
				for (int i = d.ipb[ib]; i < d.ipb[ib+1]-1; ++i) {
					auto ip = d.mt.ip;
					auto c  = d.c;
					int* df = d.d;

					auto lhscp = (ip[i] - ip[0]) / TPD;
					auto rhscp = (ip[i + 1] - ip[0]) / TPD;
					auto lhstps = (ip[i + 1] - ip[i]) / TPD;
					auto rhstps = (ip[i + 2] - ip[i + 1]) / TPD;
					findCLCS(TPD, ip[i], ip[i + 1], lhstps, rhstps, c + lhscp,
							c + rhscp, df + lhscp * TPD, df + rhscp * TPD, rd);
				}
			}
			// Skewed search
			if (type > 0) {
				for (int i = d.ipb[ib]; i < d.ipb[ib+1]-type-1; ++i) {
					auto ip = d.mt.ip;
					auto c  = d.c;
					int* df = d.d;

					auto lhscp = (ip[i] - ip[0]) / TPD;
					auto rhscp = (ip[i + 1 + type] - ip[0]) / TPD;
					auto lhstps = (ip[i + 1] - ip[i]) / TPD;
					auto rhstps = (ip[i + 2 + type] - ip[i + 1 + type]) / TPD;
					findCLCS(TPD, ip[i], ip[i + 1 + type], lhstps, rhstps, c + lhscp,
							c + rhscp, df + lhscp * TPD, df + rhscp * TPD, rd);
				}
			}
		}
	}

	/**
	 *
	 * Generates codelets from differences and tuple information.
	 * @param cl Pointer to several vectors that store codelets for each trace
	 * @param ip Pointer to tuples at start of iterations
	 * @param ips Number of pointers in ip
	 * @param c Storage for codelet groupings
	 *
	 */
	void mineDifferences(int **ip, DDT::PatternDAG *c, int *d, int nThreads,
			const int *tBound) {
		//#pragma omp parallel for num_threads(nThreads)
		for (int ii = 0; ii < nThreads; ++ii) {
			int nc = 0;
			for (int i = tBound[ii]; i < tBound[ii + 1] - 1; i++) {
				auto lhscp = (ip[i] - ip[0]) / TPD;
				auto rhscp = (ip[i + 1] - ip[0]) / TPD;
				auto lhstps = (ip[i + 1] - ip[i]) / TPD;
				auto rhstps = (ip[i + 2] - ip[i + 1]) / TPD;
				nc += findCLCS(TPD, ip[i], ip[i + 1], lhstps, rhstps, c + lhscp,
						c + rhscp, d + lhscp * TPD, d + rhscp * TPD, 2);
			}
		}
	}

	/**
	 * @brief Returns true if all 32bit elements in vector are 0/1
	 *
	 * @param dv Vector to check unit-ness
	 * @return true if all elements in vector are unit (0 or 1)
	 */
	inline bool isUnit(__m128i dv) {
		uint16_t unitMask =
			_mm_movemask_epi8(_mm_cmplt_epi32(dv, unitThresholds));
		return (unitMask | 0xF000) == 0xFFFF;
	}

	/**
	 *
	 * @brief Generate the longest common subsequence between two codelet regions.
	 *
	 * Updates the pointers and values in lhscp and rhscp
	 * to reflect the new codelet groupings.
	 *
	 * @param tpd    Dimensionality of tuples
	 * @param lhstp  Left pointer to start of tuple grouping
	 * @param rhstp  Right pointer to start of tuple grouping
	 * @param lhstps Size of tuples in left pointer to iterate
	 * @param rhstps Size of tuples in right pointer to iterate
	 * @param lhscp  Left pointer to codelet
	 * @param rhscp  Right pointer to codelet
	 * @param lhstpd Left pointer to first order differences
	 * @param rhstpd Right pointer to first order differences
	 * @param rd     Dimension that has reuse between iterations
	 *
	 */
	int findCLCS(int tpd, int *lhstp, int *rhstp, int lhstps, int rhstps,
			DDT::PatternDAG *lhscp, DDT::PatternDAG *rhscp, int *lhstpd,
			int *rhstpd, int rd) {
		int THRESHOLDS[4] = {5, 100000, DDT::col_th, 0};
		const int REUSE_DIMENSION = rd;
		const int PSC1_RD_DIM = 2;
		__m128i thresholds = _mm_set_epi32(THRESHOLDS[3], THRESHOLDS[2],
				THRESHOLDS[1], THRESHOLDS[0]);
		__m128i unitThresholds =
			_mm_set_epi32(UNIT_THRESHOLDS[3], UNIT_THRESHOLDS[2],
					UNIT_THRESHOLDS[1], UNIT_THRESHOLDS[0]);
		auto lhs = _mm_loadu_si128(reinterpret_cast<const __m128i *>(lhstp));
		auto rhs = _mm_loadu_si128(reinterpret_cast<const __m128i *>(rhstp));

		int nc = 0;
		for (int i = 0, j = 0; i < lhstps - 1 && j < rhstps - 1;) {
			// Difference and comparison
			__m128i sub = _mm_sub_epi32(rhs, lhs);
			__m128i cmp = _mm_cmplt_epi32(_mm_abs_epi32(sub), thresholds);

			// Mask upper 8 bits that aren't used
			uint16_t mm = _mm_movemask_epi8(cmp);
			mm = mm & ZERO_MASK;

			if (ZERO_MASK == mm) {
				__m128i lhsdv = _mm_loadu_si128(
						reinterpret_cast<const __m128i *>(lhstpd + i * tpd));
				__m128i rhsdv = _mm_loadu_si128(
						reinterpret_cast<const __m128i *>(rhstpd + j * tpd));
				__m128i xordv = _mm_cmpeq_epi32(rhsdv, lhsdv);


				__m128i odv = lhsdv;// originalDifferenceVector
				bool hsad = true, tmpHsad = true;   // hasSameAdjacentDifferences
				bool iu = isUnit(lhsdv);

				int iStart = i;
				int jStart = j;
				uint16_t imm = _mm_movemask_epi8(xordv);

				while ((i < lhstps - 1 && j < rhstps - 1) &&
						ZERO_MASK == (imm & ZERO_MASK) &&
						!isInCodelet(lhscp+i+1) &&
						(DDT::prefer_fsc ? hsad && iu : true)) {
					hsad = tmpHsad;
					lhsdv = _mm_loadu_si128(reinterpret_cast<const __m128i *>(
								lhstpd + ++i * tpd));
					rhsdv = _mm_loadu_si128(reinterpret_cast<const __m128i *>(
								rhstpd + ++j * tpd));
					xordv = _mm_cmpeq_epi32(rhsdv, lhsdv);
					imm = _mm_movemask_epi8(xordv);

					// Compare FOD across iteration
					uint16_t MASK =
						_mm_movemask_epi8(_mm_cmpeq_epi32(odv, lhsdv));
					tmpHsad = tmpHsad && (ZERO_MASK == (MASK & ZERO_MASK));
					iu = isUnit(lhsdv);
				}

				// Adjust pointers to form codelet
				int sz = i - iStart;
				if (sz > DDT::clt_width) {
					DDT::CodeletType t;
					uint16_t unitMask = _mm_movemask_epi8(
							_mm_cmplt_epi32(odv, unitThresholds));
					if (!isInCodelet(lhscp + iStart)) {
						lhscp[iStart].sz = sz;
						lhscp[iStart].pt = lhscp[iStart].ct;
						t = hsad && (unitMask | 0xF000) == 0xFFFF
							? DDT::TYPE_FSC
							: DDT::TYPE_PSC2;
						lhscp[iStart].t = t;
						nc++;
					} else {
						uint16_t MASK = generateDifferenceMask(
								lhscp[iStart].pt, lhstp + iStart * tpd,
								rhstp + jStart * tpd, ZERO_MASK);

						// Checks for PSC Type 1 and PSC Type 2
						// @TODO: FIX HACK sub[0] == 0
						if (!((MASK == PSC1_MASK && hsad &&
										(unitMask | 0xF000) == 0xFFFF && sub[PSC1_RD_DIM] == 0) ||
									MASK == FSC_MASK)) {
							i = lhscp[iStart].sz + iStart + 1;
							lhs = _mm_loadu_si128(
									reinterpret_cast<const __m128i *>(lhstp +
										i * tpd));
							continue;
						}
						// Update codelet type
						t = hsad && MASK == PSC1_MASK ? DDT::TYPE_PSC1
							: lhscp[iStart].t;
					}
					if (sz == lhscp[iStart].sz) {
						rhscp[jStart].sz = sz;
						rhscp[jStart].pt = lhscp[iStart].ct;
						rhscp[jStart].t = t;
					} else {
						i = lhscp[iStart].sz + iStart;
					}
					lhs = _mm_loadu_si128(reinterpret_cast<const __m128i *>(
								lhstp + ++i * tpd));
					rhs = _mm_loadu_si128(reinterpret_cast<const __m128i *>(
								rhstp + ++j * tpd));
					continue;
				} else {
					if (isInCodelet(lhscp+iStart)) {
						i = iStart + lhscp[iStart].sz + 1;
					}
				}
			}
			if (lhstp[i * tpd + REUSE_DIMENSION] <= rhstp[j * tpd + REUSE_DIMENSION]) {
				// Since lhscp->sz is number of tuples in codelet not including itself
				i += lhscp[i].sz + 1;
				lhs = _mm_loadu_si128(
						reinterpret_cast<const __m128i *>(lhstp + i * tpd));
			} else if (lhstp[i * tpd + REUSE_DIMENSION] > rhstp[j * tpd + REUSE_DIMENSION]) {
				j += rhscp[j].sz + 1;
				rhs = _mm_loadu_si128(
						reinterpret_cast<const __m128i *>(rhstp + j * tpd));
			}
		}
		return nc;
	}

	/**
	 *
	 * @brief Generates a 16-bit bitmask describing (rhs-mid == mid-lhs)
	 *
	 * @param lhs Memory location of start of lhs tuple
	 * @param mid Memory location of start of mid tuple
	 * @param rhs Memory location of start of rhs tuple
	 * @return Returns 16-bit bitmask
	 */
	inline uint16_t generateDifferenceMask(int *lhs, int *mid, int *rhs,
			int MASK) {
		// Load tuples into memory
		__m128i lhsv = _mm_loadu_si128(reinterpret_cast<const __m128i *>(lhs));
		__m128i midv = _mm_loadu_si128(reinterpret_cast<const __m128i *>(mid));
		__m128i rhsv = _mm_loadu_si128(reinterpret_cast<const __m128i *>(rhs));

		// Generate differences
		__m128i cmp = _mm_cmpeq_epi32(_mm_sub_epi32(midv, lhsv),
				_mm_sub_epi32(rhsv, midv));

		uint16_t mm = _mm_movemask_epi8(cmp);
		return (mm & ZERO_MASK);
	}

	uint32_t hsum_epi32_avx(__m128i x) {
		__m128i hi64 = _mm_unpackhi_epi64(x, x);
		__m128i sum64 = _mm_add_epi32(hi64, x);
		__m128i hi32 = _mm_shuffle_epi32(sum64, _MM_SHUFFLE(2, 3, 0, 1));
		__m128i sum32 = _mm_add_epi32(sum64, hi32);
		return _mm_cvtsi128_si32(sum32);// movd
	}

	// only needs AVX2
	uint32_t hsum_8x32(__m256i v) {
		__m128i sum128 = _mm_add_epi32(_mm256_castsi256_si128(v),
				_mm256_extracti128_si256(v, 1));
		return hsum_epi32_avx(sum128);
	}


}// namespace DDT
