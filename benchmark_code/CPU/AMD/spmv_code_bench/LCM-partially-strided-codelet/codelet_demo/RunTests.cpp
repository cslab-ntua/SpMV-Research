//
// Created by cetinicz on 2021-08-14.
//

#include "RunTests.h"
#include "TestFSC.h"
#include "TestPSC.h"

#include <iostream>
#include <tuple>
#include <vector>

#define LOG_CODELET_TESTS

int main() {
    // Run FSC_BASELINE test configurations
    std::vector<double> TEST_FSC_Baseline_SPMV_RESULTS;
    TEST_FSC_Baseline_SPMV_RESULTS.reserve(std::size(TEST_FSC_BASELINE_SPMV_CONFIG));
    for (auto& config : TEST_FSC_BASELINE_SPMV_CONFIG) {
        TEST_FSC_Baseline_SPMV_RESULTS.push_back(TEST_FSC_Baseline_SPMV(config[0], config[1], config[2]));
    }

    // Run PSC_T1_BASELINE test configurations
    std::vector<double> TEST_PSC_T1_BASELINE_SPMV_RESULTS;
    TEST_PSC_T1_BASELINE_SPMV_RESULTS.reserve(std::size(TEST_PSC_T1_BASELINE_SPMV_CONFIG));
    for (auto& config : TEST_PSC_T1_BASELINE_SPMV_CONFIG) {
        TEST_PSC_T1_BASELINE_SPMV_RESULTS.push_back(TEST_PSC_T1_Baseline_SPMV(config[0], config[1], config[2]));
    }

    // Run PSC_T2_BASELINE test configurations
    std::vector<double> TEST_PSC_T2_BASELINE_SPMV_RESULTS;
    TEST_PSC_T2_BASELINE_SPMV_RESULTS.reserve(std::size(TEST_PSC_T2_BASELINE_SPMV_CONFIG));
    for (auto& config : TEST_PSC_T2_BASELINE_SPMV_CONFIG) {
        TEST_PSC_T2_BASELINE_SPMV_RESULTS.push_back(TEST_PSC_T2_Baseline_SPMV(config[0], config[1], config[2], config[3], config[4]));
    }

    // Run PSC_T3_BASELINE test configurations
    std::vector<double> TEST_PSC_T3_BASELINE_SPMV_RESULTS;
    TEST_PSC_T3_BASELINE_SPMV_RESULTS.reserve(std::size(TEST_PSC_T3_BASELINE_SPMV_CONFIG));
    for (auto& config : TEST_PSC_T3_BASELINE_SPMV_CONFIG) {
        TEST_PSC_T3_BASELINE_SPMV_RESULTS.push_back(TEST_PSC_T3_Baseline_SPMV(config[0], config[1], config[2]));
    }

    // Run PSC_T3_BASELINE test configurations
    std::vector<std::tuple<double,double,double,double,double>> TEST_FSC_PSC_SPMV_RESULTS;
    TEST_FSC_PSC_SPMV_RESULTS.reserve(std::size(TEST_FSC_PSC_SPMV_CONFIG));
    for (auto& config : TEST_FSC_PSC_SPMV_CONFIG) {
        TEST_FSC_PSC_SPMV_RESULTS.push_back(TEST_FSC_PSC_SPMV(config[0], config[1], config[2], config[3], config[4]));
    }

#ifdef LOG_CODELET_TESTS
    std::cout << TEST_FSC_BASELINE_SPMV_HEADER << std::endl;
    for (int i = 0; i < std::size(TEST_FSC_BASELINE_SPMV_CONFIG); ++i) {
        for (int j = 0; j < std::size(TEST_FSC_BASELINE_SPMV_CONFIG[i]); ++j) {
            std::cout << TEST_FSC_BASELINE_SPMV_CONFIG[i][j] << ",";
        }
        std::cout << TEST_FSC_Baseline_SPMV_RESULTS[i] << "\n";
    }
    std::cout << "\n";

    std::cout << TEST_PSC_T1_BASELINE_SPMV_HEADER << std::endl;
    for (int i = 0; i < std::size(TEST_PSC_T1_BASELINE_SPMV_CONFIG); ++i) {
        for (int j = 0; j < std::size(TEST_PSC_T1_BASELINE_SPMV_CONFIG[i]); ++j) {
            std::cout << TEST_PSC_T1_BASELINE_SPMV_CONFIG[i][j] << ",";
        }
        std::cout << TEST_PSC_T1_BASELINE_SPMV_RESULTS[i] << "\n";
    }

    std::cout << TEST_PSC_T2_BASELINE_SPMV_HEADER << std::endl;
    for (int i = 0; i < std::size(TEST_PSC_T2_BASELINE_SPMV_CONFIG); ++i) {
        for (int j = 0; j < std::size(TEST_PSC_T2_BASELINE_SPMV_CONFIG[i]); ++j) {
            std::cout << TEST_PSC_T2_BASELINE_SPMV_CONFIG[i][j] << ",";
        }
        std::cout << TEST_PSC_T2_BASELINE_SPMV_RESULTS[i] << "\n";
    }

    std::cout << TEST_PSC_T3_BASELINE_SPMV_HEADER << std::endl;
    for (int i = 0; i < std::size(TEST_PSC_T3_BASELINE_SPMV_CONFIG); ++i) {
        for (int j = 0; j < std::size(TEST_PSC_T3_BASELINE_SPMV_CONFIG[i]); ++j) {
            std::cout << TEST_PSC_T3_BASELINE_SPMV_CONFIG[i][j] << ",";
        }
        std::cout << TEST_PSC_T3_BASELINE_SPMV_RESULTS[i] << "\n";
    }

    std::cout << TEST_FSC_PSC_SPMV_HEADER << std::endl;
    for (int i = 0; i < std::size(TEST_FSC_PSC_SPMV_CONFIG); ++i) {
        for (int j = 0; j < std::size(TEST_FSC_PSC_SPMV_CONFIG[i]); ++j) {
            std::cout << TEST_FSC_PSC_SPMV_CONFIG[i][j] << ",";
        }
        std::cout
                << std::get<0>(TEST_FSC_PSC_SPMV_RESULTS[i]) << ","
                << std::get<1>(TEST_FSC_PSC_SPMV_RESULTS[i]) << ","
                << std::get<2>(TEST_FSC_PSC_SPMV_RESULTS[i]) << ","
                << std::get<3>(TEST_FSC_PSC_SPMV_RESULTS[i]) << ","
                << std::get<4>(TEST_FSC_PSC_SPMV_RESULTS[i]) << ","
                << "\n";
    }
#endif
}