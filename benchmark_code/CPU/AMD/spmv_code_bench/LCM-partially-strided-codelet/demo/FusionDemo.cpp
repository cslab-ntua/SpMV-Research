//
// Created by Kazem on 11/10/19.
//
#define DBG_LOG
#define CSV_LOG
#include <cstring>
#include <cmath>
#include <utils.h>
#include <omp.h>
#include "FusionDemo.h"
#ifdef METIS
#include <metis_interface.h>
#endif

#undef PROFILE
// #define PROFILE


namespace sym_lib {

	FusionDemo::FusionDemo() : L1_csr_(NULLPNTR), L1_csc_(NULLPNTR), L2_csr_(NULLPNTR), L2_csc_(NULLPNTR), A_csr_(NULLPNTR),A_csc_(NULLPNTR), x_(NULLPNTR), x_in_(NULLPNTR), correct_x_(NULLPNTR)
	{
		num_test_=256;
		redundant_nodes_=0;
#ifdef PROFILE
		pw_ = NULLPNTR;
#endif
	}

	FusionDemo::FusionDemo(int n, std::string name):FusionDemo() {
		m_ = n;
		n_ = n;
		name_ = name;

		x_in_ = static_cast<double *>(std::aligned_alloc(32, sizeof(double) * n));
		x_ = static_cast<double *>(std::aligned_alloc(32, sizeof(double) * n));
#ifdef PROFILE
		pw_ = nullptr;
#endif
	}

	FusionDemo::FusionDemo(int m, int n, std::string name):FusionDemo() {
		m_ = m;
		n_ = n;
		name_ = name;

		x_in_ = static_cast<double *>(std::aligned_alloc(32, sizeof(double) * n));
		x_ = static_cast<double *>(std::aligned_alloc(32, sizeof(double) * m));
#ifdef PROFILE
		pw_ = nullptr;
#endif
	}

#ifdef PROFILE
	FusionDemo::FusionDemo(int n, std::string name, PAPIWrapper *pw):FusionDemo(n, name){
		pw_ = pw;
	}

	FusionDemo::FusionDemo(CSR *L, CSC* L_csc, CSR *A, CSC *A_csc,
			double *correct_x, std::string name, PAPIWrapper *pw):
		FusionDemo(L->n,name, pw){
			L1_csr_ = L;
			L1_csc_ = L_csc;
			A_csr_ = A;
			A_csc_ = A_csc;
			correct_x_ = correct_x;
		}
#endif

	FusionDemo::~FusionDemo() {
		delete []x_in_;
		delete []x_;
	}

	void FusionDemo::setting_up() {
		for (int i = 0; i < this->n_; ++i) {
			x_in_[i] = 1;
		}
		std::fill_n(x_,m_,0.0);
	}

	void FusionDemo::testing() {
		if(correct_x_)
			if (!is_equal(0, m_, correct_x_, x_,1e-6))
				PRINT_LOG(name_ + " code != reference solution.\n");
	}


	timing_measurement FusionDemo::evaluate() {

		volatile unsigned long * L3_cache_block;
		long L3_cache_block_n = atol(getenv("LEVEL3_CACHE_SIZE_TOTAL")) / sizeof(*L3_cache_block);
		L3_cache_block = (typeof(L3_cache_block)) malloc(L3_cache_block_n * sizeof(*L3_cache_block));
		int clear_caches = atoi(getenv("CLEAR_CACHES"));

		timing_measurement mean_t;
		std::vector<timing_measurement> time_array;
		analysis_time_.start_timer();
		build_set();
		analysis_time_.measure_elapsed_time();
		for (int i = 0; i < num_test_; ++i)
		{
			if (__builtin_expect(clear_caches, 0))
			{
				_Pragma("omp parallel")
				{
					long i;
					_Pragma("omp for")
					for (i=0;i<L3_cache_block_n;i++)
						L3_cache_block[i] = 0;
				}
			}

			// printf("test %d\n", i);
			setting_up();
#ifdef PROFILE
			if (pw_ != nullptr) { pw_->begin_profiling(); }
#endif
			timing_measurement t1 = fused_code();
#ifdef PROFILE
			if (pw_ != nullptr)
				pw_->finish_profiling();
#endif
			time_array.emplace_back(t1);
		}
		testing();

		double sum = 0;
		long i;
		for (i=0;i<num_test_;i++)
			sum += time_array[i].elapsed_time;
		sum /= num_test_;

		// mean_t = sym_lib::time_median(time_array);

		mean_t.elapsed_time = sum;
		return mean_t;
	}


	timing_measurement FusionDemo::analysisTime() {
		return analysis_time_;
	}



	void print_common_header(){
		PRINT_CSV("Matrix Name,A Dimension,A Nonzero,L Nonzero,Code Type,Data Type,"
				"Metis Enabled,Number of Threads");
	}

	void print_common(std::string matrix_name, std::string variant, std::string strategy,
			CSC *B, CSC *L, int num_threads){
		PRINT_CSV(matrix_name);
		PRINT_CSV(B->m);
		PRINT_CSV(B->nnz);
		if(L)
			PRINT_CSV(L->nnz);
		PRINT_CSV(variant);
		PRINT_CSV(strategy);
#ifdef METIS
		PRINT_CSV("Metis");
#else
		PRINT_CSV("No Metis");
#endif
		PRINT_CSV(num_threads);
	}



}

