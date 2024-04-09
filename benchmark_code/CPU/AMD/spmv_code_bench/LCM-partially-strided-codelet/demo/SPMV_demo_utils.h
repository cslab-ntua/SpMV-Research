//
// Created by kazem on 6/13/21.
//

/* a=target variable, b=bit number to act upon 0-n */
#define BIT_SET(a, b) ((a) |= (1ULL << (b)))
#define BIT_CLEAR(a, b) ((a) &= ~(1ULL << (b)))
#define BIT_FLIP(a, b) ((a) ^= (1ULL << (b)))
#define BIT_CHECK(a, b)                                                        \
	(!!((a) & (1ULL << (b))))// '!!' to make sure this returns 0 or 1

/* x=target variable, y=mask */
#define BITMASK_SET(x, y) ((x) |= (y))
#define BITMASK_CLEAR(x, y) ((x) &= (~(y)))
#define BITMASK_FLIP(x, y) ((x) ^= (y))
#define BITMASK_CHECK_ALL(x, y) (!(~(x) & (y)))
#define BITMASK_CHECK_ANY(x, y) ((x) & (y))

#ifndef SPARSE_AVX512_DEMO_SPMVVEC_H
#define SPARSE_AVX512_DEMO_SPMVVEC_H

#include "FusionDemo.h"
#include "SerialSpMVExecutor.h"

#include "DDT.h"
#include "Executor.h"
#include "Input.h"
#include "Inspector.h"
#include "PatternMatching.h"

#include "anonymouslib_avx2.h"

#include <iostream>

#include <climits>

namespace sparse_avx {

	///// SPMV
	void spmv_csr(int n, const int *Ap, const int *Ai, const double *Ax,
			const double *x, double *y) {
		int i, j;
		for (i = 0; i < n; i++) {
			for (j = Ap[i]; j < Ap[i + 1]; j++) { y[i] += Ax[j] * x[Ai[j]]; }
		}
	}

	void spmv_csr_prefetch(int n, const int *Ap, const int *Ai,
			const double *Ax, const double *x, double *y,
			const _mm_hint HINT, int DIST) {
		int i, j;
		for (i = 0; i < n; i++) {
			double yTmp = 0.;
			for (j = Ap[i]; j < Ap[i + 1]; j++) {
				_mm_prefetch((char*)Ai+j+128, _MM_HINT_NTA);
				yTmp += Ax[j] * x[Ai[j]];
			}
			y[i] = yTmp;
		}
	}

	void spmv_csr_parallel(int n, const int *Ap, const int *Ai,
			const double *Ax, const double *x, double *y,
			int nThreads) {
#pragma omp parallel for num_threads(nThreads)
		for (int i = 0; i < n; i++) {
			for (int j = Ap[i]; j < Ap[i + 1]; j++) {
				y[i] += Ax[j] * x[Ai[j]];
			}
		}
	}

	inline void hsum_double_avx_avx(__m256d v0, __m256d v1, __m128d v2,
			double *y, int i) {
		auto h0 = _mm256_hadd_pd(v0, v1);
		__m128d vlow = _mm256_castpd256_pd128(h0);
		__m128d vhigh = _mm256_extractf128_pd(h0, 1);// high 128
		vlow = _mm_add_pd(vlow, vhigh);
		vlow = _mm_add_pd(vlow, v2);
		_mm_storeu_pd(y + i, vlow);
	}

	inline double hsum_double_avx(__m256d v) {
		__m128d vlow = _mm256_castpd256_pd128(v);
		__m128d vhigh = _mm256_extractf128_pd(v, 1);// high 128
		vlow = _mm_add_pd(vlow, vhigh);             // reduce down to 128

		__m128d high64 = _mm_unpackhi_pd(vlow, vlow);
		return _mm_cvtsd_f64(_mm_add_sd(vlow, high64));// reduce to scalar
	}

	inline double hsum_add_double_avx(__m256d v, __m128d a) {
		__m128d vlow = _mm256_castpd256_pd128(v);
		__m128d vhigh = _mm256_extractf128_pd(v, 1);// high 128
		vlow = _mm_add_pd(vlow, vhigh);             // reduce down to 128
		vlow = _mm_add_pd(vlow, a);

		__m128d high64 = _mm_unpackhi_pd(vlow, vlow);
		return _mm_cvtsd_f64(_mm_add_sd(vlow, high64));// reduce to scalar
	}

	__m256i MASK_3 = _mm256_set_epi64x(0, -1, -1, -1);
	void spmv_vec1(int n, const int *Ap, const int *Ai, const double *Ax,
			const double *x, double *y, int nThreads) {
		int i = 0;
		for (; i < n-7; i+=8) {
			_mm_prefetch(&Ai[Ap[i]]+512, _MM_HINT_NTA);
			double d0 = 0., d1= 0., d2= 0., d3= 0., d4 = 0., d5 = 0., d6 = 0., d7 = 0.;
#pragma omp simd
			for (int k0 = Ap[i]; k0 < Ap[i+1]; ++k0) {
				d0 += Ax[k0] * x[Ai[k0]];
			}
#pragma omp simd
			for (int k1 = Ap[i+1]; k1 < Ap[i+2]; ++k1) {
				d1 += Ax[k1] * x[Ai[k1]];
			}
#pragma omp simd
			for (int k2 = Ap[i+2]; k2 < Ap[i+3]; ++k2) {
				d2 += Ax[k2] * x[Ai[k2]];
			}
#pragma omp simd
			for (int k3 = Ap[i+3]; k3 < Ap[i+4]; ++k3) {
				d3 += Ax[k3] * x[Ai[k3]];
			}
#pragma omp simd
			for (int k4 = Ap[i+4]; k4 < Ap[i+5]; ++k4) {
				d4 += Ax[k4] * x[Ai[k4]];
			}
#pragma omp simd
			for (int k5 = Ap[i+5]; k5 < Ap[i+6]; ++k5) {
				d5 += Ax[k5] * x[Ai[k5]];
			}
#pragma omp simd
			for (int k6 = Ap[i+6]; k6 < Ap[i+7]; ++k6) {
				d6 += Ax[k6] * x[Ai[k6]];
			}
#pragma omp simd
			for (int k7 = Ap[i+7]; k7 < Ap[i+8]; ++k7) {
				d7 += Ax[k7] * x[Ai[k7]];
			}

			y[i] = d0;
			y[i+1] = d1;
			y[i+2] = d2;
			y[i+3] = d3;
			y[i+4] = d4;
			y[i+5] = d5;
			y[i+6] = d6;
			y[i+7] = d7;
		}
		for (; i < n; ++i) {
			for (int j = Ap[i]; j < Ap[i+1]; ++j) {
				y[i] += Ax[j] * x[Ai[j]];
			}
		}
	}
	void spmv_vec1_parallel(int n, const int *Ap, const int *Ai,
			const double *Ax, const double *x, double *y,
			int nThreads) {
#pragma omp parallel for num_threads(nThreads)
		for (int i = 0; i < n; i++) {
			auto r0 = _mm256_setzero_pd();
			int j = Ap[i];
			for (; j < Ap[i + 1] - 3; j += 4) {
				auto xv = _mm256_set_pd(x[Ai[j + 3]], x[Ai[j + 2]],
						x[Ai[j + 1]], x[Ai[j]]);
				auto axv0 = _mm256_loadu_pd(Ax + j);
				r0 = _mm256_fmadd_pd(axv0, xv, r0);
			}

			// Compute tail
			double tail = 0.;
			for (; j < Ap[i + 1]; j++) { tail += Ax[j] * x[Ai[j]]; }

			// H-Sum
			y[i] += tail + hsum_double_avx(r0);
		}
	}
	void spmv_vec1_16(int n, const int *Ap, const int *Ai, const double *Ax,
			const double *x, double *y, int nThreads) {
		for (int i = 0; i < n; i++) {
			auto r0 = _mm256_setzero_pd();
			int j = Ap[i];
			for (; j < Ap[i + 1] - 15; j += 16) {
				auto xv0 = _mm256_set_pd(x[Ai[j + 3]], x[Ai[j + 2]],
						x[Ai[j + 1]], x[Ai[j]]);
				auto xv1 = _mm256_set_pd(x[Ai[j + 7]], x[Ai[j + 6]],
						x[Ai[j + 5]], x[Ai[j + 4]]);
				auto xv2 = _mm256_set_pd(x[Ai[j + 11]], x[Ai[j + 10]],
						x[Ai[j + 9]], x[Ai[j + 8]]);
				auto xv3 = _mm256_set_pd(x[Ai[j + 15]], x[Ai[j + 14]],
						x[Ai[j + 13]], x[Ai[j + 12]]);

				auto axv0 = _mm256_loadu_pd(Ax + j);
				auto axv1 = _mm256_loadu_pd(Ax + j + 4);
				auto axv2 = _mm256_loadu_pd(Ax + j + 8);
				auto axv3 = _mm256_loadu_pd(Ax + j + 12);

				r0 = _mm256_fmadd_pd(axv0, xv0, r0);
				r0 = _mm256_fmadd_pd(axv1, xv1, r0);
				r0 = _mm256_fmadd_pd(axv2, xv2, r0);
				r0 = _mm256_fmadd_pd(axv3, xv3, r0);
			}

			// Compute tail
			double tail = 0.;
			for (; j < Ap[i + 1]; j++) { tail += Ax[j] * x[Ai[j]]; }

			// H-Sum
			y[i] += tail + hsum_double_avx(r0);
		}
	}
	void spmv_vec1_8(int n, const int *Ap, const int *Ai, const double *Ax,
			const double *x, double *y, int nThreads) {
		for (int i = 0; i < n; i++) {
			auto r0 = _mm256_setzero_pd();
			int j = Ap[i];
			for (; j < Ap[i + 1] - 7; j += 8) {
				auto xv0 = _mm256_set_pd(x[Ai[j + 3]], x[Ai[j + 2]],
						x[Ai[j + 1]], x[Ai[j]]);
				auto xv1 = _mm256_set_pd(x[Ai[j + 7]], x[Ai[j + 6]],
						x[Ai[j + 5]], x[Ai[j + 4]]);

				auto axv0 = _mm256_loadu_pd(Ax + j);
				auto axv1 = _mm256_loadu_pd(Ax + j + 4);

				r0 = _mm256_fmadd_pd(axv0, xv0, r0);
				r0 = _mm256_fmadd_pd(axv1, xv1, r0);
			}

			// Compute tail
			double tail = 0.;
			for (; j < Ap[i + 1]; j++) { tail += Ax[j] * x[Ai[j]]; }

			// H-Sum
			y[i] += tail + hsum_double_avx(r0);
		}
	}

	inline double wathen3_2_3vec_1d(const int *Ap, const int *Ai,
			const double *Ax, const double *x, int i) {
		int j = Ap[i];
		int o0 = Ai[j], o1 = Ai[j + 3], o2 = Ai[j + 5];

		auto r0 = _mm256_setzero_pd();
		r0 = _mm256_fmadd_pd(_mm256_loadu_pd(Ax + j),
				_mm256_maskload_pd(x + o0, MASK_3), r0);
		r0 = _mm256_fmadd_pd(_mm256_loadu_pd(Ax + j + 5),
				_mm256_maskload_pd(x + o2, MASK_3), r0);

		return hsum_add_double_avx(
				r0, _mm_mul_pd(_mm_loadu_pd(Ax + j + 3), _mm_loadu_pd(x + o1)));
	}

	inline double wathen3_2_3_2_3vec1d(const int *Ap, const int *Ai,
			const double *Ax, const double *x,
			int i) {
		int j = Ap[i];
		int o0 = Ai[j], o1 = Ai[j + 3], o2 = Ai[j + 5], o3 = Ai[j + 8],
		o4 = Ai[j + 10];

		auto r0 = _mm256_setzero_pd();
		auto r1 = _mm_setzero_pd();
		r0 = _mm256_fmadd_pd(_mm256_loadu_pd(Ax + j),
				_mm256_maskload_pd(x + o0, MASK_3), r0);
		r1 = _mm_fmadd_pd(_mm_loadu_pd(Ax + j + 3), _mm_loadu_pd(x + o1), r1);
		r0 = _mm256_fmadd_pd(_mm256_loadu_pd(Ax + j + 5),
				_mm256_maskload_pd(x + o2, MASK_3), r0);
		r1 = _mm_fmadd_pd(_mm_loadu_pd(Ax + j + 8), _mm_loadu_pd(x + o3), r1);
		r0 = _mm256_fmadd_pd(_mm256_loadu_pd(Ax + j + 10),
				_mm256_maskload_pd(x + o4, MASK_3), r0);

		return hsum_add_double_avx(r0, r1);
	}

	inline double wathen_5_3_5_3_5vec_1d(const int *Ap, const int *Ai,
			const double *Ax, const double *x,
			int i) {
		int j = Ap[i];
		int o0 = Ai[j], o1 = Ai[j + 5], o2 = Ai[j + 8], o3 = Ai[j + 13],
		o4 = Ai[j + 16];

		auto r0 = _mm256_setzero_pd();
		r0 = _mm256_fmadd_pd(_mm256_loadu_pd(Ax + j), _mm256_loadu_pd(x + o0),
				r0);
		double tail = Ax[j + 4] * x[o0 + 4];
		r0 = _mm256_fmadd_pd(_mm256_loadu_pd(Ax + j + 5),
				_mm256_maskload_pd(x + o1, MASK_3), r0);
		r0 = _mm256_fmadd_pd(_mm256_loadu_pd(Ax + j + 8),
				_mm256_loadu_pd(x + o2), r0);
		tail += Ax[j + 12] * x[o2 + 4];
		r0 = _mm256_fmadd_pd(_mm256_loadu_pd(Ax + j + 13),
				_mm256_maskload_pd(x + o3, MASK_3), r0);
		r0 = _mm256_fmadd_pd(_mm256_loadu_pd(Ax + j + 16),
				_mm256_loadu_pd(x + o4), r0);
		tail += Ax[j + 20] * x[o4 + 4];
		return hsum_double_avx(r0) + tail;
	}

	inline double wathen3_5_vec_1d(const int *Ap, const int *Ai,
			const double *Ax, const double *x, int i) {
		int j = Ap[i];
		int o0 = Ai[j], o1 = Ai[j + 3];

		auto r0 = _mm256_setzero_pd();
		r0 = _mm256_fmadd_pd(_mm256_loadu_pd(Ax + j),
				_mm256_maskload_pd(x + o0, MASK_3), r0);
		r0 = _mm256_fmadd_pd(_mm256_loadu_pd(Ax + j + 3),
				_mm256_loadu_pd(x + o1), r0);
		double tail = Ax[j + 7] * x[o1 + 4];

		return hsum_double_avx(r0) + tail;
	}

	inline void wathen5_3_5vec_2d(const int *Ap, const int *Ai,
			const double *Ax, const double *x, double *y,
			int i) {
		int j0 = Ap[i], j1 = Ap[i + 1];
		int o0 = Ai[j0], o1 = Ai[j0 + 5], o2 = Ai[j0 + 8];
		int o3 = Ai[j1], o4 = Ai[j1 + 5], o5 = Ai[j1 + 8];

		auto r0 = _mm256_setzero_pd();
		auto r1 = _mm256_setzero_pd();
		auto tail = _mm_loadu_pd(y + i);

		// Row 1
		r0 = _mm256_fmadd_pd(_mm256_loadu_pd(Ax + j0), _mm256_loadu_pd(x + o0),
				r0);
		tail[0] += Ax[j0 + 4] * x[o0 + 4];
		r0 = _mm256_fmadd_pd(_mm256_loadu_pd(Ax + j0 + 5),
				_mm256_maskload_pd(x + o1, MASK_3), r0);
		r0 = _mm256_fmadd_pd(_mm256_loadu_pd(Ax + j0 + 8),
				_mm256_loadu_pd(x + o2), r0);
		tail[0] += Ax[j0 + 12] * x[o2 + 4];

		// Row 2
		r1 = _mm256_fmadd_pd(_mm256_loadu_pd(Ax + j1), _mm256_loadu_pd(x + o3),
				r1);
		tail[1] += Ax[j1 + 4] * x[o3 + 4];
		r1 = _mm256_fmadd_pd(_mm256_loadu_pd(Ax + j1 + 5),
				_mm256_maskload_pd(x + o4, MASK_3), r1);
		r1 = _mm256_fmadd_pd(_mm256_loadu_pd(Ax + j1 + 8),
				_mm256_loadu_pd(x + o5), r1);
		tail[1] += Ax[j1 + 12] * x[o5 + 4];

		// H-Sum
		auto h0 = _mm256_hadd_pd(r0, r1);
		__m128d vlow = _mm256_castpd256_pd128(h0);
		__m128d vhigh = _mm256_extractf128_pd(h0, 1);// high 128
		vlow = _mm_add_pd(vlow, vhigh);
		vlow = _mm_add_pd(vlow, tail);
		_mm_storeu_pd(y + i, vlow);
	}

	inline double wathen5_3_5vec_1d(const int *Ap, const int *Ai,
			const double *Ax, const double *x, int i) {
		int j = Ap[i];
		int o0 = Ai[j], o1 = Ai[j + 5], o2 = Ai[j + 8];

		auto r0 = _mm256_setzero_pd();
		r0 = _mm256_fmadd_pd(_mm256_loadu_pd(Ax + j), _mm256_loadu_pd(x + o0),
				r0);
		double tail = Ax[j + 4] * x[o0 + 4];
		r0 = _mm256_fmadd_pd(_mm256_loadu_pd(Ax + j + 5),
				_mm256_maskload_pd(x + o1, MASK_3), r0);
		r0 = _mm256_fmadd_pd(_mm256_loadu_pd(Ax + j + 8),
				_mm256_loadu_pd(x + o2), r0);
		tail += Ax[j + 12] * x[o2 + 4];
		return hsum_double_avx(r0) + tail;
	}

	void spmv_wathen_clean(int n, const int *Ap, const int *Ai,
			const double *Ax, const double *x, double *y,
			int nThreads) {
		int i = 1, j;
		y[0] += wathen3_5_vec_1d(Ap, Ai, Ax, x, 0);
		for (int k = 0; k < 99; k++, i += 2) {
			y[i] += wathen3_2_3vec_1d(Ap, Ai, Ax, x, i);
			y[i + 1] += wathen5_3_5vec_1d(Ap, Ai, Ax, x, i + 1);
		}
		for (int ii = 0; ii < 119; ++ii) {
			for (int k = 0; k < 4; k++, i++) {
				if (Ap[i + 1] - Ap[i] == 8)
					y[i] += wathen3_2_3vec_1d(Ap, Ai, Ax, x, i);
				else
					y[i] += wathen5_3_5vec_1d(Ap, Ai, Ax, x, i);
			}
			for (int k = 0; k < 49; k++, i += 2) {
				y[i] += wathen5_3_5vec_1d(Ap, Ai, Ax, x, i);
				y[i + 1] += wathen5_3_5vec_1d(Ap, Ai, Ax, x, i + 1);
			}
			for (int k = 0; k < 4; k++, i++) {
				for (j = Ap[i]; j < Ap[i + 1]; j++) {
					y[i] += Ax[j] * x[Ai[j]];
				}
			}
			for (int k = 0; k < 98; k++, i += 2) {
				y[i] += wathen5_3_5vec_1d(Ap, Ai, Ax, x, i);
				y[i + 1] += wathen_5_3_5_3_5vec_1d(Ap, Ai, Ax, x, i + 1);
			}
		}
		for (int k = 0; k < 4; k++, i++) {
			if (Ap[i + 1] - Ap[i] == 8)
				y[i] += wathen3_2_3vec_1d(Ap, Ai, Ax, x, i);
			else
				y[i] += wathen5_3_5vec_1d(Ap, Ai, Ax, x, i);
		}
		for (int k = 0; k < 49; k++, i += 2) {
			y[i] += wathen5_3_5vec_1d(Ap, Ai, Ax, x, i);
			y[i + 1] += wathen5_3_5vec_1d(Ap, Ai, Ax, x, i + 1);
		}
		for (int k = 0; k < 4; k++, i++) {
			for (j = Ap[i]; j < Ap[i + 1]; j++) { y[i] += Ax[j] * x[Ai[j]]; }
		}
		for (int k = 0; k < 99; k++, i += 2) {
			y[i] += wathen3_5_vec_1d(Ap, Ai, Ax, x, i);
			y[i + 1] += wathen5_3_5vec_1d(Ap, Ai, Ax, x, i + 1);
		}
	}

	void spmv_vec2(int n, const int *Ap, const int *Ai, const double *Ax,
			const double *x, double *y, int nThreads) {
		int i = 0;
		for (; i < n - 1; i += 2) {
			auto r0 = _mm256_setzero_pd();
			auto r1 = _mm256_setzero_pd();

			int j0 = Ap[i];
			int j1 = Ap[i + 1];
			for (; j0 < Ap[i + 1] - 3 && j1 < Ap[i + 2] - 3; j0 += 4, j1 += 4) {
				auto xv0 = _mm256_set_pd(x[Ai[j0 + 3]], x[Ai[j0 + 2]],
						x[Ai[j0 + 1]], x[Ai[j0]]);
				auto xv1 = _mm256_set_pd(x[Ai[j1 + 3]], x[Ai[j1 + 2]],
						x[Ai[j1 + 1]], x[Ai[j1]]);

				auto axv0 = _mm256_loadu_pd(Ax + j0);
				auto axv1 = _mm256_loadu_pd(Ax + j1);

				r0 = _mm256_fmadd_pd(axv0, xv0, r0);
				r1 = _mm256_fmadd_pd(axv1, xv1, r1);
			}

			// Compute tail
			__m128d tail = _mm_loadu_pd(y + i);
			for (; j0 < Ap[i + 1]; j0++) { tail[0] += Ax[j0] * x[Ai[j0]]; }
			for (; j1 < Ap[i + 2]; j1++) { tail[1] += Ax[j1] * x[Ai[j1]]; }

			// H-Sum
			auto h0 = _mm256_hadd_pd(r0, r1);
			__m128d vlow = _mm256_castpd256_pd128(h0);
			__m128d vhigh = _mm256_extractf128_pd(h0, 1);// high 128
			vlow = _mm_add_pd(vlow, vhigh);              // reduce down to 128
			vlow = _mm_add_pd(vlow, tail);
			// Store
			_mm_storeu_pd(y + i, vlow);
		}
		for (; i < n; i++) {
			auto r0 = _mm256_setzero_pd();
			int j = Ap[i];
			for (; j < Ap[i + 1] - 3; j += 4) {
				auto xv = _mm256_set_pd(x[Ai[j + 3]], x[Ai[j + 2]],
						x[Ai[j + 1]], x[Ai[j]]);
				auto axv0 = _mm256_loadu_pd(Ax + j);
				r0 = _mm256_fmadd_pd(axv0, xv, r0);
			}

			// Compute tail
			double tail = 0.;
			for (; j < Ap[i + 1]; j++) { tail += Ax[j] * x[Ai[j]]; }

			// H-Sum
			y[i] += tail + hsum_double_avx(r0);
		}
	}

	class SpMVSerial : public sym_lib::FusionDemo {
		protected:
			sym_lib::timing_measurement fused_code() override {
				sym_lib::timing_measurement t1;
				t1.start_timer();
				spmv_csr(L1_csr_->m, L1_csr_->p, L1_csr_->i, L1_csr_->x, x_in_, x_);
				t1.measure_elapsed_time();
				//copy_vector(0,n_,x_in_,x_);
				return t1;
			}

		public:
			SpMVSerial(sym_lib::CSR *L, sym_lib::CSC *L_csc, double *correct_x,
					std::string name)
				: FusionDemo(L_csc->m,L_csc->n, name) {
					L1_csr_ = L;
					L1_csc_ = L_csc;
					correct_x_ = correct_x;
#ifdef PROFILE
					this->pw_ = nullptr;
#endif
				};

			~SpMVSerial() override = default;
	};

	class SpMVSerialPrefetch : public SpMVSerial {
		_mm_hint HINT;
		int DIST;

		protected:
		sym_lib::timing_measurement fused_code() override {
			sym_lib::timing_measurement t1;
			t1.start_timer();
			spmv_csr_prefetch(n_, L1_csr_->p, L1_csr_->i, L1_csr_->x, x_in_, x_,
					HINT, DIST);
			t1.measure_elapsed_time();
			//copy_vector(0,n_,x_in_,x_);
			return t1;
		}

		public:
		SpMVSerialPrefetch(sym_lib::CSR *L, sym_lib::CSC *L_csc,
				double *correct_x, std::string name, _mm_hint h,
				int d)
			: SpMVSerial(L, L_csc, correct_x, name), HINT(h), DIST(d) {
				L1_csr_ = L;
				L1_csc_ = L_csc;
				correct_x_ = correct_x;
			};
	};

	class SpMVParallel : public SpMVSerial {
		protected:
			sym_lib::timing_measurement fused_code() override {
				sym_lib::timing_measurement t1;
				t1.start_timer();
				spmv_csr_parallel(m_, L1_csr_->p, L1_csr_->i, L1_csr_->x, x_in_, x_,
						num_threads_);
				t1.measure_elapsed_time();
				//copy_vector(0,n_,x_in_,x_);
				return t1;
			}

		public:
			SpMVParallel(sym_lib::CSR *L, sym_lib::CSC *L_csc, double *correct_x,
					std::string name)
				: SpMVSerial(L, L_csc, correct_x, name) {
					L1_csr_ = L;
					L1_csc_ = L_csc;
					correct_x_ = correct_x;
				};
	};

	class SpMVVec1Parallel : public SpMVSerial {
		protected:
			sym_lib::timing_measurement fused_code() override {
				sym_lib::timing_measurement t1;
				t1.start_timer();
				spmv_vec1_parallel(m_, L1_csr_->p, L1_csr_->i, L1_csr_->x, x_in_,
						x_, num_threads_);
				t1.measure_elapsed_time();
				//copy_vector(0,n_,x_in_,x_);
				return t1;
			}

		public:
			SpMVVec1Parallel(sym_lib::CSR *L, sym_lib::CSC *L_csc,
					double *correct_x, std::string name)
				: SpMVSerial(L, L_csc, correct_x, name) {
					L1_csr_ = L;
					L1_csc_ = L_csc;
					correct_x_ = correct_x;
				};
	};


	class SpMVVec1 : public SpMVSerial {
		protected:
			sym_lib::timing_measurement fused_code() override {
				sym_lib::timing_measurement t1;
				t1.start_timer();
				spmv_vec1(m_, L1_csr_->p, L1_csr_->i, L1_csr_->x, x_in_, x_,
						num_threads_);
				t1.measure_elapsed_time();
				//copy_vector(0,n_,x_in_,x_);
				return t1;
			}

		public:
			SpMVVec1(sym_lib::CSR *L, sym_lib::CSC *L_csc, double *correct_x,
					std::string name)
				: SpMVSerial(L, L_csc, correct_x, name) {
					L1_csr_ = L;
					L1_csc_ = L_csc;
					correct_x_ = correct_x;
				};
	};

	class SpMVVec1_8 : public SpMVSerial {
		protected:
			sym_lib::timing_measurement fused_code() override {
				sym_lib::timing_measurement t1;
				t1.start_timer();
				spmv_vec1_8(m_, L1_csr_->p, L1_csr_->i, L1_csr_->x, x_in_, x_,
						num_threads_);
				t1.measure_elapsed_time();
				//copy_vector(0,n_,x_in_,x_);
				return t1;
			}

		public:
			SpMVVec1_8(sym_lib::CSR *L, sym_lib::CSC *L_csc, double *correct_x,
					std::string name)
				: SpMVSerial(L, L_csc, correct_x, name) {
					L1_csr_ = L;
					L1_csc_ = L_csc;
					correct_x_ = correct_x;
				};
	};

	class SpMVVec1_16 : public SpMVSerial {
		protected:
			sym_lib::timing_measurement fused_code() override {
				sym_lib::timing_measurement t1;
				t1.start_timer();
				spmv_vec1_16(m_, L1_csr_->p, L1_csr_->i, L1_csr_->x, x_in_, x_,
						num_threads_);
				t1.measure_elapsed_time();
				//copy_vector(0,n_,x_in_,x_);
				return t1;
			}

		public:
			SpMVVec1_16(sym_lib::CSR *L, sym_lib::CSC *L_csc, double *correct_x,
					std::string name)
				: SpMVSerial(L, L_csc, correct_x, name) {
					L1_csr_ = L;
					L1_csc_ = L_csc;
					correct_x_ = correct_x;
				};
	};

	class SpMVVec2 : public SpMVSerial {
		protected:
			sym_lib::timing_measurement fused_code() override {
				sym_lib::timing_measurement t1;
				t1.start_timer();
				spmv_vec2(m_, L1_csr_->p, L1_csr_->i, L1_csr_->x, x_in_, x_,
						num_threads_);
				t1.measure_elapsed_time();
				//copy_vector(0,n_,x_in_,x_);
				return t1;
			}

		public:
			SpMVVec2(sym_lib::CSR *L, sym_lib::CSC *L_csc, double *correct_x,
					std::string name)
				: SpMVSerial(L, L_csc, correct_x, name) {
					L1_csr_ = L;
					L1_csc_ = L_csc;
					correct_x_ = correct_x;
				};
	};

	class SpMVWathen : public SpMVSerial {
		protected:
			sym_lib::timing_measurement fused_code() override {
				sym_lib::timing_measurement t1;
				t1.start_timer();
				spmv_wathen_clean(m_, L1_csr_->p, L1_csr_->i, L1_csr_->x, x_in_, x_,
						num_threads_);
				t1.measure_elapsed_time();
				//copy_vector(0,n_,x_in_,x_);
				return t1;
			}

		public:
			SpMVWathen(sym_lib::CSR *L, sym_lib::CSC *L_csc, double *correct_x,
					std::string name)
				: SpMVSerial(L, L_csc, correct_x, name) {
					L1_csr_ = L;
					L1_csc_ = L_csc;
					correct_x_ = correct_x;
				};
	};


	class TightLoopSpMV : public SpMVSerial {
		typedef void (*FunctionPtr)();
		uint8_t *wlist;
		void reset_pointers() {
			// Assign pointers
			cword = this->wlist;
			axi = this->L1_csr_->i;
			xp = this->x_in_;
			axp = this->L1_csr_->x;
			app = this->L1_csr_->p;
			yp = this->x_;
			m = this->L1_csr_->m;
		}

		/**
		 * Generates a hash for any permutation of numbers with [0,1,2,3,4,5] in
		 * the ones, tens or hundreds position
		 *
		 * @param ba  Contains the x digit number
		 * @param bac Number of digits in the number
		 */
		uint8_t generateBitHash(short *ba, short bac) {
			return (ba[2] * 36 + ba[1] * 6 + ba[0]) % 256;
		}

		/** 
		 * @brief Writes out command word and arguments to memory
		 *
		 * @param cWord Memory Location of current word
		 * @param ba    Memory containing command codes
		 * @param bac   Number of command codes
		 * @param args  Memory containing arguments for command codes
		 */
		void writeWord(uint32_t *&cWord, short *ba, short &bac, uint8_t *args) {
			// Early return if we can't write the word
			if (bac < 3) { return; }
			auto *cTop = (uint8_t *) cWord;
			*cTop++ = generateBitHash(ba, bac);
			for (int i = 0; i < bac; ++i) {
				*cTop++ = args[i];
				args[i] = 0x0;
			}
			bac = 0;
			cWord++;
		}

		/** 
		 * @brief Rounds up number to nearest multiple
		 *
		 * @param numToRound Number to be rounded
		 * @param multiple   Multiple to be rounded to 
		 */
		int roundUp(int numToRound, int multiple) {
			assert(multiple && ((multiple & (multiple - 1)) == 0));
			return (numToRound + multiple - 1) & -multiple;
		}

		/** 
		 * @brief Gets current argument based on current position in matrix
		 *
		 * @param cntr
		 * @param ip
		 * @param j
		 * @param acc
		 */
		inline uint32_t get_arg(int cntr, int *ip, int j, int acc) {
			return ip[j - (cntr - 1)] - acc;
		}

		/** 
		 * @brief Fills argument array and command code array based on matrix position
		 *
		 * @param ba
		 * @param bac
		 * @param args
		 * @param cWord
		 * @param cntr
		 * @param arg
		 */
		inline void build_args(short *ba, short &bac, uint8_t *args,
				uint32_t *&cWord, int cntr, int arg) {
			auto nBits = 32 - __builtin_clz(arg | 1);
			int bitSize = roundUp(nBits, 8);

			/* 
			 * The code below stores rg in the smallest number of 
			 * 16 bit memory segments as possible
			 */
			while (bitSize > 0) {// Determine # of bytes we need
				ba[bac] = bitSize == 8 ? cntr : 0;
				auto mask = ((1 << (bitSize)) - 1) << (bitSize - 8);
				args[bac++] = (mask & arg) >> (bitSize - 8);// Get next 8 bits
				writeWord(cWord, ba, bac, args);
				bitSize -= 8;
			}
		}

		void bitHashToCmd(uint8_t arg, short *cmds) {
			cmds[0] = (arg % 6);
			cmds[1] = (arg % 36) / 6;
			cmds[2] = (arg / 36);
		}

		void printCodeResult(short cmd, uint8_t arg, int &acc, int &row,
				int &col) {
			switch (cmd) {
				case 0:
					acc = acc * 256 + arg;
					break;
				case 1:
					col = col + acc * 256 + arg;
					acc = 0;
					std::cout << "(" << row << "," << col << ")" << std::endl;
					break;
				case 2:
					col = col + acc * 256 + arg;
					acc = 0;
					std::cout << "(" << row << "," << col << ")" << std::endl;
					std::cout << "(" << row << "," << col + 1 << ")"
						<< std::endl;
					col = col + 1;
					break;
				case 3:
					col = col + acc * 256 + arg;
					acc = 0;
					std::cout << "(" << row << "," << col << ")" << std::endl;
					std::cout << "(" << row << "," << col + 1 << ")"
						<< std::endl;
					std::cout << "(" << row << "," << col + 2 << ")"
						<< std::endl;
					col = col + 2;
					break;
				case 4:
					col = col + acc * 256 + arg;
					acc = 0;
					std::cout << "(" << row << "," << col << ")" << std::endl;
					std::cout << "(" << row << "," << col + 1 << ")"
						<< std::endl;
					std::cout << "(" << row << "," << col + 2 << ")"
						<< std::endl;
					std::cout << "(" << row << "," << col + 3 << ")"
						<< std::endl;
					col = col + 3;
					break;
				case 5:
					row = row + 1;
					col = 0;
					break;
				default:
					throw std::runtime_error("Argument invalid... Exiting...");
			}
		}

		void printDCSRMatrix(uint32_t *words) {
			// Globals
			int acc = 0, row = 0, col = 0;

			// Convert into 8-bit Representation
			auto args = (uint8_t *) words;

			// Temporary array to handle commands one at a time
			short cmds[3] = {0};
			while (*args != 255) {
				bitHashToCmd(*args, cmds);
				for (short cmd : cmds) {
					printCodeResult(cmd, *(++args), acc, row, col);
				}
				args++;
				if (args - (uint8_t *) words > 100) { exit(1); }
			}
		}

		inline void f0(int &col, int &acc, uint8_t *args)
			__attribute__((always_inline)) {
				acc = (acc << 8) + *args;
			}

		inline void f1(int &col, int &acc, uint8_t *args)
			__attribute__((always_inline)) {
				col = col + acc * 256 + *args;
				acc = 0;
				*yp += *axp++ * xp[col];
			}

		inline void f2(int &col, int &acc, uint8_t *args)
			__attribute__((always_inline)) {
				col = col + acc * 256 + *args;
				acc = 0;
				*yp += *axp++ * xp[col];
				*yp += *axp++ * xp[col + 1];
				col = col + 1;
			}

		inline void f3(int &col, int &acc, uint8_t *args)
			__attribute__((always_inline)) {
				col = col + acc * 256 + *args;
				acc = 0;
				*yp += *axp++ * xp[col];
				*yp += *axp++ * xp[col + 1];
				*yp += *axp++ * xp[col + 2];
				col = col + 2;
			}

		inline void f4(int &col, int &acc, uint8_t *args)
			__attribute__((always_inline)) {
				col = col + acc * 256 + *args;
				acc = 0;
				*yp += *axp++ * xp[col];
				*yp += *axp++ * xp[col + 1];
				*yp += *axp++ * xp[col + 2];
				*yp += *axp++ * xp[col + 3];
				col = col + 3;
			}

		void f5(int &col, int &acc, uint8_t *args)
			__attribute__((always_inline)) {
				yp++;
				col = 0;
			}

		void dcsr_naive_switch(uint8_t *&args) {
			int col = 0, acc = 0;
			while (args[0] != 255) {
				switch (*args) {
					case 1:
						f1(col, acc, ++args);
						f0(col, acc, ++args);
						f0(col, acc, ++args);
						break;
					case 2:
						f2(col, acc, ++args);
						f0(col, acc, ++args);
						f0(col, acc, ++args);
						break;
					case 3:
						f3(col, acc, ++args);
						f0(col, acc, ++args);
						f0(col, acc, ++args);
						break;
					case 4:
						f4(col, acc, ++args);
						f0(col, acc, ++args);
						f0(col, acc, ++args);
						break;
					case 5:
						f5(col, acc, ++args);
						f0(col, acc, ++args);
						f0(col, acc, ++args);
						break;
					case 6:
						f0(col, acc, ++args);
						f1(col, acc, ++args);
						f0(col, acc, ++args);
						break;
					case 7:
						f1(col, acc, ++args);
						f1(col, acc, ++args);
						f0(col, acc, ++args);
						break;
					case 8:
						f2(col, acc, ++args);
						f1(col, acc, ++args);
						f0(col, acc, ++args);
						break;
					case 9:
						f3(col, acc, ++args);
						f1(col, acc, ++args);
						f0(col, acc, ++args);
						break;
					case 10:
						f4(col, acc, ++args);
						f1(col, acc, ++args);
						f0(col, acc, ++args);
						break;
					case 11:
						f5(col, acc, ++args);
						f1(col, acc, ++args);
						f0(col, acc, ++args);
						break;
					case 12:
						f0(col, acc, ++args);
						f2(col, acc, ++args);
						f0(col, acc, ++args);
						break;
					case 13:
						f1(col, acc, ++args);
						f2(col, acc, ++args);
						f0(col, acc, ++args);
						break;
					case 14:
						f2(col, acc, ++args);
						f2(col, acc, ++args);
						f0(col, acc, ++args);
						break;
					case 15:
						f3(col, acc, ++args);
						f2(col, acc, ++args);
						f0(col, acc, ++args);
						break;
					case 16:
						f4(col, acc, ++args);
						f2(col, acc, ++args);
						f0(col, acc, ++args);
						break;
					case 18:
						f0(col, acc, ++args);
						f3(col, acc, ++args);
						f0(col, acc, ++args);
						break;
					case 19:
						f1(col, acc, ++args);
						f3(col, acc, ++args);
						f0(col, acc, ++args);
						break;
					case 20:
						f2(col, acc, ++args);
						f3(col, acc, ++args);
						f0(col, acc, ++args);
						break;
					case 21:
						f3(col, acc, ++args);
						f3(col, acc, ++args);
						f0(col, acc, ++args);
						break;
					case 22:
						f4(col, acc, ++args);
						f3(col, acc, ++args);
						f0(col, acc, ++args);
						break;
					case 24:
						f0(col, acc, ++args);
						f4(col, acc, ++args);
						f0(col, acc, ++args);
						break;
					case 25:
						f1(col, acc, ++args);
						f4(col, acc, ++args);
						f0(col, acc, ++args);
						break;
					case 26:
						f2(col, acc, ++args);
						f4(col, acc, ++args);
						f0(col, acc, ++args);
						break;
					case 31:
						f1(col, acc, ++args);
						f5(col, acc, ++args);
						f0(col, acc, ++args);
						break;
					case 32:
						f2(col, acc, ++args);
						f5(col, acc, ++args);
						f0(col, acc, ++args);
						break;
					case 33:
						f3(col, acc, ++args);
						f5(col, acc, ++args);
						f0(col, acc, ++args);
						break;
					case 34:
						f4(col, acc, ++args);
						f5(col, acc, ++args);
						f0(col, acc, ++args);
						break;
					case 36:
						f0(col, acc, ++args);
						f0(col, acc, ++args);
						f1(col, acc, ++args);
						break;
					case 37:
						f1(col, acc, ++args);
						f0(col, acc, ++args);
						f1(col, acc, ++args);
						break;
					case 38:
						f2(col, acc, ++args);
						f0(col, acc, ++args);
						f1(col, acc, ++args);
						break;
					case 39:
						f3(col, acc, ++args);
						f0(col, acc, ++args);
						f1(col, acc, ++args);
						break;
					case 40:
						f4(col, acc, ++args);
						f0(col, acc, ++args);
						f1(col, acc, ++args);
						break;
					case 41:
						f5(col, acc, ++args);
						f0(col, acc, ++args);
						f1(col, acc, ++args);
						break;
					case 42:
						f0(col, acc, ++args);
						f1(col, acc, ++args);
						f1(col, acc, ++args);
						break;
					case 43:
						f1(col, acc, ++args);
						f1(col, acc, ++args);
						f1(col, acc, ++args);
						break;
					case 44:
						f2(col, acc, ++args);
						f1(col, acc, ++args);
						f1(col, acc, ++args);
						break;
					case 45:
						f3(col, acc, ++args);
						f1(col, acc, ++args);
						f1(col, acc, ++args);
						break;
					case 46:
						f4(col, acc, ++args);
						f1(col, acc, ++args);
						f1(col, acc, ++args);
						break;
					case 47:
						f5(col, acc, ++args);
						f1(col, acc, ++args);
						f1(col, acc, ++args);
						break;
					case 48:
						f0(col, acc, ++args);
						f2(col, acc, ++args);
						f1(col, acc, ++args);
						break;
					case 49:
						f1(col, acc, ++args);
						f2(col, acc, ++args);
						f1(col, acc, ++args);
						break;
					case 50:
						f2(col, acc, ++args);
						f2(col, acc, ++args);
						f1(col, acc, ++args);
						break;
					case 51:
						f3(col, acc, ++args);
						f2(col, acc, ++args);
						f1(col, acc, ++args);
						break;
					case 52:
						f4(col, acc, ++args);
						f2(col, acc, ++args);
						f1(col, acc, ++args);
						break;
					case 53:
						f5(col, acc, ++args);
						f2(col, acc, ++args);
						f1(col, acc, ++args);
						break;
					case 54:
						f0(col, acc, ++args);
						f3(col, acc, ++args);
						f1(col, acc, ++args);
						break;
					case 55:
						f1(col, acc, ++args);
						f3(col, acc, ++args);
						f1(col, acc, ++args);
						break;
					case 56:
						f2(col, acc, ++args);
						f3(col, acc, ++args);
						f1(col, acc, ++args);
						break;
					case 57:
						f3(col, acc, ++args);
						f3(col, acc, ++args);
						f1(col, acc, ++args);
						break;
					case 58:
						f4(col, acc, ++args);
						f3(col, acc, ++args);
						f1(col, acc, ++args);
						break;
					case 59:
						f5(col, acc, ++args);
						f3(col, acc, ++args);
						f1(col, acc, ++args);
						break;
					case 60:
						f0(col, acc, ++args);
						f4(col, acc, ++args);
						f1(col, acc, ++args);
						break;
					case 61:
						f1(col, acc, ++args);
						f4(col, acc, ++args);
						f1(col, acc, ++args);
						break;
					case 62:
						f2(col, acc, ++args);
						f4(col, acc, ++args);
						f1(col, acc, ++args);
						break;
					case 65:
						f5(col, acc, ++args);
						f4(col, acc, ++args);
						f1(col, acc, ++args);
						break;
					case 66:
						f0(col, acc, ++args);
						f5(col, acc, ++args);
						f1(col, acc, ++args);
						break;
					case 67:
						f1(col, acc, ++args);
						f5(col, acc, ++args);
						f1(col, acc, ++args);
						break;
					case 68:
						f2(col, acc, ++args);
						f5(col, acc, ++args);
						f1(col, acc, ++args);
						break;
					case 69:
						f3(col, acc, ++args);
						f5(col, acc, ++args);
						f1(col, acc, ++args);
						break;
					case 70:
						f4(col, acc, ++args);
						f5(col, acc, ++args);
						f1(col, acc, ++args);
						break;
					case 72:
						f0(col, acc, ++args);
						f0(col, acc, ++args);
						f2(col, acc, ++args);
						break;
					case 73:
						f1(col, acc, ++args);
						f0(col, acc, ++args);
						f2(col, acc, ++args);
						break;
					case 74:
						f2(col, acc, ++args);
						f0(col, acc, ++args);
						f2(col, acc, ++args);
						break;
					case 75:
						f3(col, acc, ++args);
						f0(col, acc, ++args);
						f2(col, acc, ++args);
						break;
					case 76:
						f4(col, acc, ++args);
						f0(col, acc, ++args);
						f2(col, acc, ++args);
						break;
					case 77:
						f5(col, acc, ++args);
						f0(col, acc, ++args);
						f2(col, acc, ++args);
						break;
					case 78:
						f0(col, acc, ++args);
						f1(col, acc, ++args);
						f2(col, acc, ++args);
						break;
					case 79:
						f1(col, acc, ++args);
						f1(col, acc, ++args);
						f2(col, acc, ++args);
						break;
					case 80:
						f2(col, acc, ++args);
						f1(col, acc, ++args);
						f2(col, acc, ++args);
						break;
					case 81:
						f3(col, acc, ++args);
						f1(col, acc, ++args);
						f2(col, acc, ++args);
						break;
					case 82:
						f4(col, acc, ++args);
						f1(col, acc, ++args);
						f2(col, acc, ++args);
						break;
					case 83:
						f5(col, acc, ++args);
						f1(col, acc, ++args);
						f2(col, acc, ++args);
						break;
					case 84:
						f0(col, acc, ++args);
						f2(col, acc, ++args);
						f2(col, acc, ++args);
						break;
					case 85:
						f1(col, acc, ++args);
						f2(col, acc, ++args);
						f2(col, acc, ++args);
						break;
					case 86:
						f2(col, acc, ++args);
						f2(col, acc, ++args);
						f2(col, acc, ++args);
						break;
					case 87:
						f3(col, acc, ++args);
						f2(col, acc, ++args);
						f2(col, acc, ++args);
						break;
					case 88:
						f4(col, acc, ++args);
						f2(col, acc, ++args);
						f2(col, acc, ++args);
						break;
					case 89:
						f5(col, acc, ++args);
						f2(col, acc, ++args);
						f2(col, acc, ++args);
						break;
					case 90:
						f0(col, acc, ++args);
						f3(col, acc, ++args);
						f2(col, acc, ++args);
						break;
					case 91:
						f1(col, acc, ++args);
						f3(col, acc, ++args);
						f2(col, acc, ++args);
						break;
					case 92:
						f2(col, acc, ++args);
						f3(col, acc, ++args);
						f2(col, acc, ++args);
						break;
					case 94:
						f4(col, acc, ++args);
						f3(col, acc, ++args);
						f2(col, acc, ++args);
						break;
					case 95:
						f5(col, acc, ++args);
						f3(col, acc, ++args);
						f2(col, acc, ++args);
						break;
					case 96:
						f0(col, acc, ++args);
						f4(col, acc, ++args);
						f2(col, acc, ++args);
						break;
					case 97:
						f1(col, acc, ++args);
						f4(col, acc, ++args);
						f2(col, acc, ++args);
						break;
					case 98:
						f2(col, acc, ++args);
						f4(col, acc, ++args);
						f2(col, acc, ++args);
						break;
					case 103:
						f1(col, acc, ++args);
						f5(col, acc, ++args);
						f2(col, acc, ++args);
						break;
					case 108:
						f0(col, acc, ++args);
						f0(col, acc, ++args);
						f3(col, acc, ++args);
						break;
					case 109:
						f1(col, acc, ++args);
						f0(col, acc, ++args);
						f3(col, acc, ++args);
						break;
					case 110:
						f2(col, acc, ++args);
						f0(col, acc, ++args);
						f3(col, acc, ++args);
						break;
					case 111:
						f3(col, acc, ++args);
						f0(col, acc, ++args);
						f3(col, acc, ++args);
						break;
					case 112:
						f4(col, acc, ++args);
						f0(col, acc, ++args);
						f3(col, acc, ++args);
						break;
					case 113:
						f5(col, acc, ++args);
						f0(col, acc, ++args);
						f3(col, acc, ++args);
						break;
					case 114:
						f0(col, acc, ++args);
						f1(col, acc, ++args);
						f3(col, acc, ++args);
						break;
					case 115:
						f1(col, acc, ++args);
						f1(col, acc, ++args);
						f3(col, acc, ++args);
						break;
					case 116:
						f2(col, acc, ++args);
						f1(col, acc, ++args);
						f3(col, acc, ++args);
						break;
					case 117:
						f3(col, acc, ++args);
						f1(col, acc, ++args);
						f3(col, acc, ++args);
						break;
					case 118:
						f4(col, acc, ++args);
						f1(col, acc, ++args);
						f3(col, acc, ++args);
						break;
					case 119:
						f5(col, acc, ++args);
						f1(col, acc, ++args);
						f3(col, acc, ++args);
						break;
					case 120:
						f0(col, acc, ++args);
						f2(col, acc, ++args);
						f3(col, acc, ++args);
						break;
					case 121:
						f1(col, acc, ++args);
						f2(col, acc, ++args);
						f3(col, acc, ++args);
						break;
					case 122:
						f2(col, acc, ++args);
						f2(col, acc, ++args);
						f3(col, acc, ++args);
						break;
					case 123:
						f3(col, acc, ++args);
						f2(col, acc, ++args);
						f3(col, acc, ++args);
						break;
					case 126:
						f0(col, acc, ++args);
						f3(col, acc, ++args);
						f3(col, acc, ++args);
						break;
					case 127:
						f1(col, acc, ++args);
						f3(col, acc, ++args);
						f3(col, acc, ++args);
						break;
					case 131:
						f5(col, acc, ++args);
						f3(col, acc, ++args);
						f3(col, acc, ++args);
						break;
					case 132:
						f0(col, acc, ++args);
						f4(col, acc, ++args);
						f3(col, acc, ++args);
						break;
					case 133:
						f1(col, acc, ++args);
						f4(col, acc, ++args);
						f3(col, acc, ++args);
						break;
					case 139:
						f1(col, acc, ++args);
						f5(col, acc, ++args);
						f3(col, acc, ++args);
						break;
					case 140:
						f2(col, acc, ++args);
						f5(col, acc, ++args);
						f3(col, acc, ++args);
						break;
					case 144:
						f0(col, acc, ++args);
						f0(col, acc, ++args);
						f4(col, acc, ++args);
						break;
					case 145:
						f1(col, acc, ++args);
						f0(col, acc, ++args);
						f4(col, acc, ++args);
						break;
					case 146:
						f2(col, acc, ++args);
						f0(col, acc, ++args);
						f4(col, acc, ++args);
						break;
					case 147:
						f3(col, acc, ++args);
						f0(col, acc, ++args);
						f4(col, acc, ++args);
						break;
					case 148:
						f4(col, acc, ++args);
						f0(col, acc, ++args);
						f4(col, acc, ++args);
						break;
					case 149:
						f5(col, acc, ++args);
						f0(col, acc, ++args);
						f4(col, acc, ++args);
						break;
					case 150:
						f0(col, acc, ++args);
						f1(col, acc, ++args);
						f4(col, acc, ++args);
						break;
					case 151:
						f1(col, acc, ++args);
						f1(col, acc, ++args);
						f4(col, acc, ++args);
						break;
					case 152:
						f2(col, acc, ++args);
						f1(col, acc, ++args);
						f4(col, acc, ++args);
						break;
					case 155:
						f5(col, acc, ++args);
						f1(col, acc, ++args);
						f4(col, acc, ++args);
						break;
					case 156:
						f0(col, acc, ++args);
						f2(col, acc, ++args);
						f4(col, acc, ++args);
						break;
					case 157:
						f1(col, acc, ++args);
						f2(col, acc, ++args);
						f4(col, acc, ++args);
						break;
					case 161:
						f5(col, acc, ++args);
						f2(col, acc, ++args);
						f4(col, acc, ++args);
						break;
					case 162:
						f0(col, acc, ++args);
						f3(col, acc, ++args);
						f4(col, acc, ++args);
						break;
					case 168:
						f0(col, acc, ++args);
						f4(col, acc, ++args);
						f4(col, acc, ++args);
						break;
					case 175:
						f1(col, acc, ++args);
						f5(col, acc, ++args);
						f4(col, acc, ++args);
						break;
					case 180:
						f0(col, acc, ++args);
						f0(col, acc, ++args);
						f5(col, acc, ++args);
						break;
					case 181:
						f1(col, acc, ++args);
						f0(col, acc, ++args);
						f5(col, acc, ++args);
						break;
					case 182:
						f2(col, acc, ++args);
						f0(col, acc, ++args);
						f5(col, acc, ++args);
						break;
					case 186:
						f0(col, acc, ++args);
						f1(col, acc, ++args);
						f5(col, acc, ++args);
						break;
					case 187:
						f1(col, acc, ++args);
						f1(col, acc, ++args);
						f5(col, acc, ++args);
						break;
					case 188:
						f2(col, acc, ++args);
						f1(col, acc, ++args);
						f5(col, acc, ++args);
						break;
					case 189:
						f3(col, acc, ++args);
						f1(col, acc, ++args);
						f5(col, acc, ++args);
						break;
					case 190:
						f4(col, acc, ++args);
						f1(col, acc, ++args);
						f5(col, acc, ++args);
						break;
					case 191:
						f5(col, acc, ++args);
						f1(col, acc, ++args);
						f5(col, acc, ++args);
						break;
					case 192:
						f0(col, acc, ++args);
						f2(col, acc, ++args);
						f5(col, acc, ++args);
						break;
					case 193:
						f1(col, acc, ++args);
						f2(col, acc, ++args);
						f5(col, acc, ++args);
						break;
					case 194:
						f2(col, acc, ++args);
						f2(col, acc, ++args);
						f5(col, acc, ++args);
						break;
					case 195:
						f3(col, acc, ++args);
						f2(col, acc, ++args);
						f5(col, acc, ++args);
						break;
					case 196:
						f4(col, acc, ++args);
						f2(col, acc, ++args);
						f5(col, acc, ++args);
						break;
					case 198:
						f0(col, acc, ++args);
						f3(col, acc, ++args);
						f5(col, acc, ++args);
						break;
					case 199:
						f1(col, acc, ++args);
						f3(col, acc, ++args);
						f5(col, acc, ++args);
						break;
					case 200:
						f2(col, acc, ++args);
						f3(col, acc, ++args);
						f5(col, acc, ++args);
						break;
					case 201:
						f3(col, acc, ++args);
						f3(col, acc, ++args);
						f5(col, acc, ++args);
						break;
					case 202:
						f4(col, acc, ++args);
						f3(col, acc, ++args);
						f5(col, acc, ++args);
						break;
					case 204:
						f0(col, acc, ++args);
						f4(col, acc, ++args);
						f5(col, acc, ++args);
						break;
					case 205:
						f1(col, acc, ++args);
						f4(col, acc, ++args);
						f5(col, acc, ++args);
						break;
					case 206:
						f2(col, acc, ++args);
						f4(col, acc, ++args);
						f5(col, acc, ++args);
						break;
					case 207:
						f3(col, acc, ++args);
						f4(col, acc, ++args);
						f5(col, acc, ++args);
						break;
				}
				args++;
			}
		}

		static inline uint32_t rotl32(uint32_t n, unsigned int c) {
			const unsigned int mask =
				(CHAR_BIT * sizeof(n) - 1);// assumes width is a power of 2.

			// assert ( (c<=mask) &&"rotate by type width or more");
			c &= mask;
			return (n << c) | (n >> ((-c) & mask));
		}

		static inline uint32_t rotr32(uint32_t n, unsigned int c) {
			const unsigned int mask = (CHAR_BIT * sizeof(n) - 1);

			// assert ( (c<=mask) &&"rotate by type width or more");
			c &= mask;
			return (n >> c) | (n << ((-c) & mask));
		}


#define ONE_MUL(col_name)                                                      \
		r0 = _mm_fmadd_pd(_mm_load_pd(Ax), _mm_load_sd(x + col_name), r0);      \
		axp += 1;
#define TWO_MUL(col_name)                                                      \
		r0 = _mm_fmadd_pd(_mm_load_pd(Ax), _mm_load_pd(x + col_name), r0);     \
		axp += 2;
#define THREE_MUL(col_name)                                                    \
		r0 = _mm_fmadd_pd(                                                         \
				_mm_load_pd(Ax + 2), _mm_load_sd(x + col_name + 2),             \
				_mm_fmadd_pd(_mm_loadu_pd(Ax), _mm_load_pd(x + col_name), r0)); \
		axp += 3;
#define STORE                                                                  \
		*y = _mm_cvtsd_f64(_mm_add_sd(r0, _mm_unpackhi_pd(r0, r0)));              \
		r0 = _mm_setzero_pd();                                                     \
		++y;

		void dcsr_naive_g3_circuit_switch(uint8_t *&args, double* y, double* x, double* Ax) {
			int col = 0, acc = 0;

			__m128d r0 = _mm_setzero_pd();

			while (*args != 255) {
				switch (*args) {
					case 1:
						col = col + (acc << 8) + args[1];
						ONE_MUL(col);
						acc = (args[2] << 8) + args[3];
						break;
					case 2:
						col = col + (acc << 8) + args[1];
						TWO_MUL(col);
						col = col + 1;
						acc = (args[2] << 8) + args[3];
						break;
					case 3:
						col = col + (acc << 8) + args[1];
						THREE_MUL(col);
						col = col + 2;
						acc = (args[2] << 8) + args[3];
						break;
					case 5:
						STORE;
						col = 0;
						acc = (args[2] << 8) + args[3];
						break;
					case 6:
						col = col + (acc << 16) + (args[1] << 8) + args[2];
						ONE_MUL(col);
						acc = args[3];
						break;
					case 11:
						STORE;
						col = args[2];
						ONE_MUL(col);
						acc = args[3];
						break;
					case 12:
						col = col + (acc << 16) + (args[1] << 8) + args[2];
						TWO_MUL(col++);
						acc = args[3];
						break;
					case 18:
						col = col + (acc << 16) + (args[1] << 8) + args[2];
						THREE_MUL(col);
						col = col + 2;
						acc = args[3];
						break;
					case 23:
						STORE;
						col = args[2];
						THREE_MUL(col);
						col = col + 2;
						acc = args[3];
						break;
					case 31:
						col = col + (acc << 8) + args[1];
						ONE_MUL(col);
						STORE;
						col = 0;
						acc = args[3];
						break;
					case 32:
						col = col + (acc << 8) + args[1];
						TWO_MUL(col);
						STORE;
						col = 0;
						acc = args[3];
						break;
					case 33:
						col = col + (acc << 8) + args[1];
						THREE_MUL(col);
						STORE;
						col = 0;
						acc = args[3];
						break;
					case 36:
						col = col + (acc << 24) + (args[1] << 16) +
							(args[2] << 8) + args[3];
						ONE_MUL(col);
						acc = 0;
						break;
					case 38:
						col = col + (acc << 8) + args[1];
						TWO_MUL(col);
						col = col + 1 + (args[2] << 8) + args[3];
						ONE_MUL(col);
						acc = 0;
						break;
					case 39:
						col = col + (acc << 8) + args[1];
						THREE_MUL(col);
						col = col + 2 + (args[2] << 8) + args[3];
						acc = 0;
						ONE_MUL(col);
						break;
					case 41:
						STORE;
						col = (args[2] << 8) + args[3];
						ONE_MUL(col);
						break;
					case 67:
						col = col + (acc << 8) + args[1];
						ONE_MUL(col);
						STORE;
						ONE_MUL(args[3]);
						col = args[3];
						acc = 0;
						break;
					case 72:
						col = col + (acc << 24) + (args[1] << 16) +
							(args[2] << 8) + args[3];
						TWO_MUL(col++);
						acc = 0;
						break;
					case 73:
						col = col + (acc << 8) + args[1];
						ONE_MUL(col);
						col = col + (args[2] << 8) + args[3];
						TWO_MUL(col);
						acc = 0;
						col = col + 1;
						break;
					case 77:
						STORE;
						col = (args[2] << 8) + args[3];
						TWO_MUL(col);
						acc = 0;
						col = col + 1;
						break;
					case 108:
						col = col + (acc << 24) + (args[1] << 16) +
							(args[2] << 8) + args[3];
						THREE_MUL(col);
						acc = 0;
						col = col + 2;
						break;
					case 109:
						col = col + (acc << 8) + args[1];
						ONE_MUL(col);
						col = col + (args[2] << 8) + args[3];
						THREE_MUL(col);
						col = col + 2;
						acc = 0;
						break;
					case 113:
						STORE;
						col = (args[2] << 8) + args[3];
						THREE_MUL(col);
						col = col + 2;
						acc = 0;
						break;
					case 139:
						col = col + (acc << 8) + args[1];
						ONE_MUL(col);
						STORE;
						THREE_MUL(col);
						col = args[3] + 2;
						acc = 0;
						break;
					case 141:
						col = col + (acc << 8) + args[1];
						THREE_MUL(col);
						STORE;
						THREE_MUL(col);
						acc = 0;
						col = args[3] + 2;
						break;
					case 186:
						col = col + (acc << 16) + (args[1] << 8) + args[2];
						ONE_MUL(col);
						STORE;
						col = 0;
						acc = 0;
						break;
					case 192:
						col = col + (acc << 16) + (args[1] << 8) + args[2];
						TWO_MUL(col);
						STORE;
						col = 0;
						acc = 0;
						break;
					case 198:
						col = col + (acc << 16) + (args[1] << 8) + args[2];
						THREE_MUL(col);
						STORE;
						col = 0;
						acc = 0;
						break;
					case 203:
						col = (acc << 8) + args[2];
						STORE;
						THREE_MUL(col);
						STORE;
						col = 0;
						acc = 0;
						break;
				}
				args += 4;
			}
		}

		static void dcsr_naive_torsion_switch(const uint8_t* carg, double* y, const double* x, const double* Ax) {
			int col = 0, acc = 0;

			auto args = carg;
			__m128d r0 = _mm_setzero_pd();

			while (*args != 255) {
				switch (args[0]) {
					case 31:
						col = col + (acc << 8) + args[1];
						ONE_MUL(col);
						STORE;
						col = 0;
						acc = args[3];
						break;
					case 32:
						col = col + (acc << 8) + args[1];
						TWO_MUL(col);
						STORE;
						col = 0;
						acc = args[3];
						break;
					case 41:
						STORE;
						col = (args[2] << 8) + args[3];
						ONE_MUL(col);
						break;
					case 42:
						col = col + (acc << 16) + (args[1] << 8) + args[2];
						ONE_MUL(col);
						col = col + args[3];
						ONE_MUL(col);
						acc = 0;
						break;
					case 55:
						col = col + (acc << 8) + args[1];
						ONE_MUL(col);
						col = col + args[2];
						THREE_MUL(col);
						col = col + 2 + args[2];
						ONE_MUL(col);
						acc = 0;
						break;
					case 67:
						col = col + (acc << 8) + args[1];
						ONE_MUL(col);
						STORE;
						ONE_MUL(args[3]);
						col = args[3];
						acc = 0;
						break;
					case 114:
						col = col + (acc << 16) + (args[1] << 8) + args[2];
						ONE_MUL(col);
						col = col + args[2];
						THREE_MUL(col);
						col = col + 2;
						acc = 0;
						break;
					case 119:
						STORE;
						col = args[1];
						ONE_MUL(col);
						col = col + args[2];
						THREE_MUL(col);
						col = col + 2;
						acc = 0;
						break;
					case 187:
						col = col + (acc << 8) + args[1];
						ONE_MUL(col);
						ONE_MUL(col + args[2]);
						STORE;
						col = 0;
						acc = 0;
						break;
					case 189:
						col = col + (acc << 8) + args[1];
						THREE_MUL(col);
						ONE_MUL(col + 2 + args[2]);
						STORE;
						col = 0;
						acc = 0;
						break;
					case 192:
						col = col + (acc << 16) + (args[1] << 8) + args[2];
						TWO_MUL(col);
						STORE;
						col = 0;
						acc = 0;
						break;
					case 197:
						STORE;
						TWO_MUL(args[1]);
						STORE;
						acc = 0;
						col = 0;
						break;
					case 211:
						ONE_MUL(col + (acc << 8) + args[1]);
						STORE;
						acc = 0;
						col = 0;
						break;
				}
				args += 4;
			}
		}

		void spmv_dcsr_naive() {
			// Globals
			int acc = 0, col = 0;

			// Convert into 8-bit Representation
			auto args = cword;

			// Temporary array to handle commands one at a time
			short cmds[3] = {0};
			while (*args != 255) {
				bitHashToCmd(*args, cmds);
				for (short cmd : cmds) {
					args++;
					switch (cmd) {
						case 0:
							acc = acc * 256 + *args;
							break;
						case 1:
							col = col + acc * 256 + *args;
							acc = 0;
							*yp += *axp++ * xp[col];
							break;
						case 2:
							col = col + acc * 256 + *args;
							acc = 0;
							*yp += *axp++ * xp[col];
							*yp += *axp++ * xp[col + 1];
							col = col + 1;
							break;
						case 3:
							col = col + acc * 256 + *args;
							acc = 0;
							*yp += *axp++ * xp[col];
							*yp += *axp++ * xp[col + 1];
							*yp += *axp++ * xp[col + 2];
							col = col + 2;
							break;
						case 4:
							col = col + acc * 256 + *args;
							acc = 0;
							*yp += *axp++ * xp[col];
							*yp += *axp++ * xp[col + 1];
							*yp += *axp++ * xp[col + 2];
							*yp += *axp++ * xp[col + 3];
							col = col + 3;
							break;
						case 5:
							yp++;
							col = 0;
							break;
						default:
							break;
					}
				}
				args++;
			}
		}

		/**
		 * @brief Generates specialized source code for a matrices deltas
		 *
		 * @param args Command codes and arguments for a given matrix in DCSR
		 */
		void print_custom_switch(uint8_t *args) {
			bool enabledSwitches[216] = {0};
			short cmds[3];
			auto carg = args;

			// Gather all required switches
			while (*carg != 255) {
				enabledSwitches[*carg] = true;
				carg += 4;
			}

			// Generate switch statement
			std::cout << "switch (*args) {\n";
			for (uint8_t i = 0; i < 216; i++) {
				if (!enabledSwitches[i]) continue;
				bitHashToCmd(i, cmds);
				std::cout << "case " << (int) i << ":\n";
				for (short cmd : cmds) {
					std::cout << "f" << (int) cmd << "(col, acc, ++args);\n";
				}
				std::cout << "break;\n";
			}
			std::cout << "}\n";
		}

		void build_set() override {
			auto p = this->L1_csr_->p;
			auto ip = this->L1_csr_->i;
			auto m = this->L1_csr_->m;

			// Init templates
			build_templates();
			auto words = new uint32_t[this->L1_csr_->nnz];
			short ba[3];
			uint8_t args[3] = {0};
			short bac = 0;

			auto cWord = words;

			// Compress matrix
			for (int i = 0; i < m; ++i) {
				int j = p[i], acc = 0;
				for (int cntr = 1; j < p[i + 1]; ++j, ++cntr) {
					if ((j + 1 == p[i + 1]) || (ip[j + 1] - ip[j]) != 1 ||
							cntr == 4) {
						if (cntr > 1) {
							if (j + 2 == p[i + 1] && (ip[j + 1] - ip[j]) == 1) {
								build_args(ba, bac, args, cWord, cntr + 1,
										get_arg(cntr, ip, j, acc));
							} else {
								build_args(ba, bac, args, cWord, cntr,
										get_arg(cntr, ip, j, acc));
							}
							acc = ip[j];
							cntr = 0;
						} else {
							build_args(ba, bac, args, cWord, 1,
									get_arg(1, ip, j, acc));
							acc = ip[j];
							cntr = 0;
						}
					}
				}


				// Set command bit to 5 to indicate row is done
				ba[bac] = 0x5;
				args[bac++] = 0x0;
				writeWord(cWord, ba, bac, args);
			}
			while (bac < 3 && bac != 0) {
				ba[bac] = 0x5;
				args[bac++] = 0x0;
			}
			writeWord(cWord, ba, bac, args);
			auto lastWord = (uint8_t *) cWord;
			*lastWord = 255;

			//                                    print_custom_switch((uint8_t*)words);
			//                                    exit(0);

			this->wlist = (uint8_t *) words;

			// Assign pointers
			reset_pointers();
		}

		sym_lib::timing_measurement fused_code() override {
			reset_pointers();
			sym_lib::timing_measurement t1;
			t1.start_timer();
			dcsr_naive_torsion_switch(cword, this->x_, this->x_in_,this->L1_csr_->x);
			t1.measure_elapsed_time();
			return t1;
		}

		public:
		TightLoopSpMV(int nThreads, sym_lib::CSR *L, sym_lib::CSC *L_csc,
				double *correct_x, std::string name)
			: SpMVSerial(L, L_csc, correct_x, name) {}

		~TightLoopSpMV() override {}
	};

	class SpMVELL : public SpMVSerial {
		int m;
		int JMAX;
		double* Lxe;
		int* ie;
		sym_lib::timing_measurement analysis_breakdown;


		void build_set() {
			if (this->Lxe != nullptr) {
				return;
			}
			analysis_breakdown.start_timer();
			// Set m
			this->m = this->L1_csr_->m;

			// Get JMAX
			JMAX = 0;
			for (int i = 0; i < this->L1_csr_->m; ++i) {
				JMAX = std::max(JMAX, this->L1_csr_->p[i+1]-this->L1_csr_->p[i]);
			}

			// Allocate memory
			Lxe = new double[JMAX*this->L1_csr_->m]();
			ie = new int[JMAX*this->L1_csr_->m]();

			// Build ELL-Pack format
			for (int i = 0; i < this->L1_csr_->m; ++i) {
				int counter = 0;
				for (int j = this->L1_csr_->p[i], cnt = 1; j < this->L1_csr_->p[i+1]; ++j, ++counter) {
					if (i == this->L1_csr_->i[j]) {
						Lxe[i*JMAX] = this->L1_csr_->x[j];
						ie[i*JMAX]  = i;
					} else {
						Lxe[i*JMAX + cnt] = this->L1_csr_->x[j];
						ie[i*JMAX + cnt]  = this->L1_csr_->i[j];
						cnt++;
					}
				}
				if (counter < JMAX) {
					Lxe[i*JMAX + counter]  = 0;
					ie[i*JMAX + counter] = 1;
				}
			}
			analysis_breakdown.measure_elapsed_time();
		}
		sym_lib::timing_measurement fused_code() override{
			sym_lib::timing_measurement t1;
			t1.start_timer();
#pragma omp parallel for num_threads(this->num_threads_)
			for (int i = 0; i < m; ++i) {
				for (int j = 0; j < JMAX; ++j) {
					x_[i] += this->Lxe[i*JMAX+j] * x_in_[this->ie[i*JMAX+j]];
				}
			}
			t1.measure_elapsed_time();
			return t1;
		}
		public:
		SpMVELL(sym_lib::CSR *L, sym_lib::CSC *L_csc, double *correct_x, std::string name)
			: SpMVSerial(L, L_csc, correct_x, name), Lxe(nullptr), ie(nullptr) {
				L1_csr_ = L;
				L1_csc_ = L_csc;
				correct_x_ = correct_x;
			};
		sym_lib::timing_measurement get_analysis_bw() {
			return analysis_breakdown;
		}
		~SpMVELL() override {
			delete[] Lxe;
			delete[] ie;
		}
	};

	class SpMVDIA : public SpMVSerial {
		double* diagval;
		int* idiag;
		int ndiag = 0;
		int lval = 0;

		sym_lib::timing_measurement analysis_breakdown;

		void build_set() override {
			if (idiag != nullptr)
				return;
			analysis_breakdown.start_timer();
			ndiag = 0;
			lval = this->L1_csr_->m;

			// We need to count the number of diagonals with NNZ
			unsigned* usedDiag = new unsigned[this->L1_csr_->m*2-1];
			memset(usedDiag, 0, sizeof(unsigned)*(this->L1_csr_->m*2-1));

			for (int idxRow = 0 ; idxRow < this->L1_csr_->m ; ++idxRow){
				for (int idxVal = this->L1_csr_->p[idxRow] ; idxVal < this->L1_csr_->p[idxRow+1] ; ++idxVal){
					const int idxCol = this->L1_csr_->i[idxVal];
					const int diag = (this->L1_csr_->m-1)-idxRow+idxCol;
					assert(0 <= diag && diag < this->L1_csr_->m*2-1);
					if(usedDiag[diag] == 0){
						usedDiag[diag] = 1;
						ndiag += 1;
					}
				}
			}

			diagval = new double[ndiag*lval]();
			idiag = new int[ndiag]();

			for (int i = 0, cnt = 0; i < this->L1_csr_->m*2-1; ++i) {
				if (usedDiag[i] == 1) {
					idiag[cnt++] = i - (this->L1_csr_->m - 1);
				}
			}

			// Fill diagonals
			for (int i = 0, cnt = 0; i < this->lval; ++i) {
				for (int j = this->L1_csr_->p[i]; j < this->L1_csr_->p[i+1]; ++j) {
					const int idxCol = this->L1_csr_->i[j];
					const int diag = this->L1_csr_->m-i+idxCol-1;
					const int offset = diag - (this->L1_csr_->m-1);
					int k = 0;
					for (; k < ndiag; ++k) {
						if (idiag[k] == offset) {
							break;
						}
					}
					diagval[k*lval+this->L1_csr_->i[j]] = this->L1_csr_->x[j];
				}
			}
			delete[] usedDiag;
			analysis_breakdown.measure_elapsed_time();
		}

		sym_lib::timing_measurement fused_code() override {
			sym_lib::timing_measurement t1;
			t1.start_timer();
			//#pragma omp parallel for num_threads(this->num_threads_)
			for (int i = 0; i < ndiag; ++i) {
				int offset = idiag[i];
				if (offset < 0) {
					offset *= -1;
					for (int j = 0; j < lval-offset; ++j) {
						x_[j+offset] += diagval[lval*i+j] * x_in_[j];
					}
				} else {
					for (int j = offset; j < lval; ++j) {
						x_[j-offset] += diagval[lval*i+j] * x_in_[j];
					}
				}
			}
			t1.measure_elapsed_time();

			return t1;
		}
		public:
		SpMVDIA(sym_lib::CSR *L, sym_lib::CSC *L_csc, double *correct_x, std::string name)
			: SpMVSerial(L, L_csc, correct_x, name), idiag(nullptr), diagval(nullptr) {
				L1_csr_ = L;
				L1_csc_ = L_csc;
				correct_x_ = correct_x;
			};
		sym_lib::timing_measurement get_analysis_bw() {
			return analysis_breakdown;
		}
		~SpMVDIA() override {
			delete[] diagval;
			delete[] idiag;
		}
	};

	class SpMVBCSR : SpMVSerial {
		int blockSize;
		int nbBlocks;

		void build_set() override {
			nbBlocks = 0;

			// We need to count the number of blocks (aligned!)
			const int maxBlockPerRow = (this->L1_csr_->m+blockSize-1)/blockSize;
			auto* usedBlocks = new unsigned[maxBlockPerRow];

			for(int idxRow = 0 ; idxRow < this->L1_csr_->m ; ++idxRow){
				if(idxRow%blockSize == 0){
					memset(usedBlocks, 0, sizeof(unsigned)*maxBlockPerRow);
				}
				for(int idxVal = this->L1_csr_->p[idxRow] ; idxVal < this->L1_csr_->p[idxRow+1] ; ++idxVal){
					const int idxCol = this->L1_csr_->i[idxVal];
					if(usedBlocks[idxCol/blockSize] == 0){
						usedBlocks[idxCol/blockSize] = 1;
						nbBlocks += 1;
					}
				}
			}

			delete[] usedBlocks;

			auto nbBlockRows = (this->L1_csr_->m+blockSize-1)/blockSize;
			auto lb = blockSize;
			auto ldabsr = lb*lb;
			auto a = new double[nbBlocks*lb*lb];
			auto ia = new int[nbBlockRows+1];
			auto ja = new int[nbBlocks];
		}
	};

	class SpMVCSR5 : public SpMVSerial {
		anonymouslibHandle<int, unsigned int, double> A;
		int*Ap;
		int*Ai;
		double*Ax;
		sym_lib::timing_measurement analysis_breakdown;

		void build_set() override{
			analysis_breakdown.start_timer();
			if (Ap != nullptr)
				return;
			Ap = new int[this->L1_csr_->m+1]();
			Ai = new int[this->L1_csr_->nnz]();
			Ax = new double[this->L1_csr_->nnz]();

			for (int i = 0; i < this->L1_csr_->nnz; ++i) {
				Ax[i] = this->L1_csr_->x[i];
				Ai[i] = this->L1_csr_->i[i];
			}
			for (int i = 0; i < this->L1_csr_->m + 1; ++i) {
				Ap[i] = this->L1_csr_->p[i];
			}
			auto err = A.inputCSR(this->L1_csr_->nnz, Ap, Ai, Ax);
			A.setX(x_in_); // you only need to do it once!
			A.setSigma(16);
			A.asCSR5();
			analysis_breakdown.measure_elapsed_time();
		}
		sym_lib::timing_measurement fused_code() override{
			sym_lib::timing_measurement t1;
			t1.start_timer();
			auto err = A.spmv(1.0, x_);
			t1.measure_elapsed_time();
			//copy_vector(0,n_,x_in_,x_);
			return t1;
		}
		public:
		SpMVCSR5(sym_lib::CSR *L, sym_lib::CSC *L_csc, double *correct_x, std::string name)
			: SpMVSerial(L, L_csc, correct_x, name), A(this->L1_csr_->m, this->L1_csr_->n), Ap(nullptr), Ax(nullptr), Ai(nullptr) {
				L1_csr_ = L;
				L1_csc_ = L_csc;
				correct_x_ = correct_x;
			};
		sym_lib::timing_measurement get_analysis_bw() {
			return analysis_breakdown;
		}
		~SpMVCSR5() override {
			delete[] Ap;
			delete[] Ai;
			delete[] Ax;
		}
	};
	class SpMVDDT : public SpMVSerial {
		protected:
			DDT::Config config;
			std::vector<DDT::Codelet *> *cl;
			DDT::GlobalObject d;
			sym_lib::timing_measurement analysis_breakdown;

			void build_set() override {
				// Allocate memory and generate global object
				// fprintf(stderr, "test build_set 1\n");
				if (this->cl == nullptr) {
					this->cl = new std::vector<DDT::Codelet *>[config.nThread];
					// fprintf(stderr, "test build_set 2\n");
					analysis_breakdown.start_timer();
					// fprintf(stderr, "test build_set 3\n");
					d = DDT::init(this->L1_csr_, config);
					// fprintf(stderr, "test build_set 4\n");
					analysis_breakdown.start_timer();
					// fprintf(stderr, "test build_set 5\n");
					DDT::inspectSerialTrace(d, cl, config);
					// fprintf(stderr, "test build_set 6\n");
					analysis_breakdown.measure_elapsed_time();
					// fprintf(stderr, "test build_set 7\n");
				}
			}

			sym_lib::timing_measurement fused_code() override {
				sym_lib::timing_measurement t1;
				DDT::Args args;
				args.x = x_in_;
				args.y = x_;
				args.r = L1_csr_->m;
				args.Lp = L1_csr_->p;
				args.Li = L1_csr_->i;
				args.Lx = L1_csr_->x;
				t1.start_timer();
				// Execute codes
				DDT::executeCodelets(cl, config, args);
				t1.measure_elapsed_time();
				//copy_vector(0,n_,x_in_,x_);
				return t1;
			}

		public:
			SpMVDDT(sym_lib::CSR *L, sym_lib::CSC *L_csc, double *correct_x,
					DDT::Config &conf, std::string name)
				: SpMVSerial(L, L_csc, correct_x, name), config(conf), cl(nullptr) {
					L1_csr_ = L;
					L1_csc_ = L_csc;
					correct_x_ = correct_x;
				};

			sym_lib::timing_measurement get_analysis_bw() {
				return analysis_breakdown;
			}

			~SpMVDDT() {
				DDT::free(d);
				delete[] cl;
			}
	};


}// namespace sparse_avx


#endif//SPARSE_AVX512_DEMO_SPMVVEC_H
