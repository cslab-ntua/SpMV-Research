//
// Created by Kazem on 11/10/19.
//

#ifndef PROJECT_FUSIONDEMO_H
#define PROJECT_FUSIONDEMO_H
#include "def.h"
#include <math.h>
#ifdef PAPI
// #include "PAPIWrapper.h"
#endif

#undef PROFILE

namespace sym_lib{

	template<class type>
		bool is_float_equal(const type x, const type y, double absTol, double relTol) {
			return std::abs(x - y) <= std::max(absTol, relTol * std::max(std::abs(x), std::abs(y)));
		}

	template<class type>
		bool is_generic_equal(const type x, const type y, double eps) {
			return std::abs(x - y) > eps;
		}

	template<class type>
		bool is_equal(int beg_idx, int end_idx, const type* vec1, const type* vec2,
				double eps=1e-8){
			for (int i = beg_idx; i < end_idx; ++i) {
				if (std::isnan(vec1[i]) || std::isnan(vec2[i]))
					return false;
				if constexpr (std::is_same_v<type, double> || std::is_same_v<type, float>) {
					if (!is_float_equal(vec1[i],vec2[i], eps, eps)) {
						std::cout << i << ":" << vec1[i] << "," << vec2[i] << std::endl;
						return false;
					}
				} else {
					if (!is_generic_equal(vec1[i],vec2[i], eps))
						return false;
				}
			}
			return true;
		}
	class FusionDemo {

		protected:
			int m_{};
			int n_{};
			double *correct_x_{};
			double *x_{}, *x_in_{};
			std::string name_{};

			int num_test_{};
			int num_threads_{};
			CSR *L1_csr_, *L2_csr_, *A_csr_;
			CSC *L1_csc_, *L1t_csc_, *L2_csc_, *A_csc_;

			/// profiling info
			int redundant_nodes_{};
			double redundant_nnz_{};
			double cost_nnz_{}; double cost_unit_{};
			double avg_parallelism{}; double avg_iter_parallelism{};
			double critical_path_{}; double max_diff_{};
			timing_measurement analysis_time_{};


			virtual void build_set(){};
			virtual void setting_up();
			virtual timing_measurement fused_code() = 0;
			virtual void testing();
		public:
			FusionDemo();
			explicit  FusionDemo(int, std::string);
			FusionDemo(int m, int n, std::string name);
			virtual ~FusionDemo();

#ifdef PAPI
			PAPIWrapper *pw_ = nullptr;
			explicit  FusionDemo(int, std::string, PAPIWrapper *pw);
			FusionDemo(CSR *L, CSC* L_csc, CSR *A, CSC *A_csc,
					double *correct_x, std::string name, PAPIWrapper *pw);
			void set_pw(PAPIWrapper *pw){pw_=pw;}
#endif

			int redundantNodes(){ return redundant_nodes_;}
			double redundantNNZ(){ return redundant_nnz_;}
			double *solution(){ return x_;}
			timing_measurement evaluate();

			void set_num_test(int nt){num_test_=nt;};
			void set_num_threads(int nt){num_threads_=nt;};
			std::string Name(){ return name_;}
			timing_measurement analysisTime();
			double CostUnit(){ return cost_unit_;}
			double CostNNZ(){ return cost_nnz_;}
			double avgParallleism(){ return avg_parallelism;}
			double avgIterParallelism(){ return avg_iter_parallelism;}
			double criticalPath(){ return critical_path_; }
			double maxDiff(){return max_diff_; }
	};

	void generate_matrices_from_mtx(CSC *L1_csc,
			CSR *&L2_csr, CSC *&B, CSR *&B_csr);

	void print_common_header();
	void print_common(std::string matrix_name, std::string variant, std::string strategy,
			CSC *B, CSC *L, int num_thread);


}



#endif //PROJECT_FUSIONDEMO_H
