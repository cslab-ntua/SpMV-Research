//
// Created by cetinicz on 2021-08-07.
//

#include "Analyzer.h"
#include "DDT.h"
#include "DDTDef.h"
#include "DDTUtils.h"
#include "SparseMatrixIO.h"
#include "GenericCodelets.h"

#include <unordered_map>
#include <numeric>
#include <valarray>
#include <strstream>

#define CSV_INIT() \

typedef struct {
    std::strstream header;
    std::strstream values;
} csv_row_t;


#define CSV_ENTRY(row, var)      \
    row.header << #var << ",";   \
    row.values << var << ",";




namespace DDT {
    void analyzeData(const DDT::GlobalObject& d, std::vector<DDT::Codelet*>** cll, const DDT::Config& config) {
        auto cl = new std::vector<DDT::Codelet*>[config.nThread]();

        for (int i = 0; i < d.sm->_final_level_no; i++) {
            for (int j = 0; j < d.sm->_wp_bounds[i]; ++j) {
                cl[0].insert(cl[0].end(), cll[i][j].begin(), cll[i][j].end());
            }
        }
        analyzeData(d, cl, config);
    }
    void analyzeData(const DDT::GlobalObject& d, const std::vector<DDT::Codelet*>* cll, const DDT::Config& config) {
        // Get matrix statistics
        auto m = readSparseMatrix<CSR>(config.matrixPath);

        std::vector<DDT::Codelet*> cl;
        for (int i = 0; i < config.nThread; ++i) {
            cl.insert(cl.end(), cll[i].begin(), cll[i].end());
        }

        // Get data from matrix
        int rows = m.r,
            cols = m.c,
            nnz = m.nz;
        double average_row_length = 0,
              average_row_length_std_deviation = 0,
              average_row_overlap = 0,
              average_row_overlap_std_deviation = 0,
              average_col_distance = 0,
              average_col_distance_std_deviation = 0,
              average_row_sequential_component = 0,
              average_length_row_sequential_component = 0,
              average_row_skew = 0,
              average_row_skew_std_deviation = 0.,
              unique_row_patterns = 0,
              average_num_unique_row_patterns = 0,
              average_num_unique_row_patterns_std_deviation = 0.,
              average_row_sequential_component_std_deviation = 0.,
              average_length_row_sequential_component_std_deviation = 0.,
              numSeqPoints = 0.;

        std::unordered_map<std::string, int> unique_row_patterns_hash;

        for (int ii = 0; ii < 2; ++ii) {
            for (int i = 0; i < m.r; ++i) {
                if (ii == 0) {
                    average_row_length += m.Lp[i + 1] - m.Lp[i];
                } else {
                    average_row_length_std_deviation += std::pow((m.Lp[i + 1] - m.Lp[i]) - average_row_length, 2);
                }

                std::stringstream ss;
                int length_row_sequential_component_cnt = 0,
                    row_sequential_component_cnt = 0;
                int currentSeqSize = 1;
                const int VW_SQS  = 4;
                for (int j = m.Lp[i]; j < m.Lp[i + 1]; ++j) {
                    if (j + 1 < m.Lp[i + 1]) {
                        if (ii == 0) {
                            average_col_distance += m.Li[j + 1] - m.Li[j];
                        } else {
                            average_col_distance_std_deviation += std::pow((m.Li[j + 1] - m.Li[j]) - average_col_distance, 2);
                        }
                        if (m.Li[j + 1] - m.Li[j] != 1) {
                            if (ii == 0) {
                                if (currentSeqSize >= VW_SQS) {
                                    numSeqPoints += currentSeqSize;
                                }
                                currentSeqSize = 1;
                                average_row_sequential_component++;
                            } else {
                                row_sequential_component_cnt++;
                            }
                        } else {
                            if (ii == 0) {
                                currentSeqSize++;
                                average_length_row_sequential_component++;
                            } else {
                                length_row_sequential_component_cnt++;
                            }
                        }
                        ss << (m.Li[j + 1] - m.Li[j]);
                    }
                }
                if (ii == 1) {
                    average_row_sequential_component_std_deviation += std::pow(row_sequential_component_cnt - average_row_sequential_component, 2);
                    average_length_row_sequential_component_std_deviation += std::pow(length_row_sequential_component_cnt - average_length_row_sequential_component, 2);
                }
                if (ii == 0) {
                    unique_row_patterns_hash[ss.str()] += 1;
                    if (currentSeqSize >= VW_SQS) {
                        numSeqPoints += currentSeqSize;
                    }
                }
                currentSeqSize = 1;
                if (i + 1 < m.r) {
                    if (ii == 0) {
                        average_row_skew += m.Li[m.Lp[i + 1]] - m.Li[m.Lp[i]];
                        average_row_overlap +=
                                std::min(m.Li[m.Lp[i + 1] - 1],
                                         m.Li[m.Lp[i + 2] - 1]) -
                                         std::max(m.Li[m.Lp[i]], m.Li[m.Lp[i + 1]]);
                    } else {
                        average_row_skew_std_deviation += std::pow(m.Li[m.Lp[i + 1]] - m.Li[m.Lp[i]] - average_row_skew,2);
                        average_row_overlap_std_deviation += std::pow((std::min(m.Li[m.Lp[i + 1] - 1],
                                                                               m.Li[m.Lp[i + 2] - 1]) -
                                                                                       std::max(m.Li[m.Lp[i]], m.Li[m.Lp[i + 1]])) - average_row_skew,2);
                    }
                }
            }
            if (ii == 0) {
                average_row_length /= m.r;
                average_row_overlap /= m.r;
                average_col_distance /= m.nz-m.r;
                average_row_skew /= m.r;
                average_row_sequential_component /= m.r;
                average_length_row_sequential_component /= m.r;
            } else {
                average_row_length_std_deviation = std::sqrt(average_row_length_std_deviation/m.r);
                average_row_overlap_std_deviation = std::sqrt(average_row_overlap_std_deviation/m.r);
                average_col_distance_std_deviation = std::sqrt(average_col_distance_std_deviation/(m.nz-m.r));
                average_row_skew_std_deviation = std::sqrt(average_row_skew_std_deviation/m.r);
                average_row_sequential_component_std_deviation = std::sqrt(average_row_sequential_component_std_deviation/m.r);
                average_length_row_sequential_component_std_deviation = std::sqrt(average_length_row_sequential_component_std_deviation/m.r);
            }
        }

        unique_row_patterns = unique_row_patterns_hash.size();
        average_num_unique_row_patterns =
                std::accumulate(
                      unique_row_patterns_hash.begin(),
                      unique_row_patterns_hash.end(),
                        0.,
                      [](float acc, const std::pair<std::string, int>& entry) {
                            return acc + entry.second;
                      }) / unique_row_patterns_hash.size();
        average_num_unique_row_patterns_std_deviation =
                std::sqrt(std::accumulate(
                        unique_row_patterns_hash.begin(),
                        unique_row_patterns_hash.end(),
                        0.,
                        [&](float acc, const std::pair<std::string, int>& entry) {
                            return acc + std::pow(entry.second-average_num_unique_row_patterns,2);
                        }) / unique_row_patterns_hash.size());

        float average_fsc_width = 0,
              average_fsc_width_std_deviation = 0.,
              average_fsc_height = 0.,
              average_fsc_height_std_deviation = 0.,
              average_fsc_points = 0.,
              average_fsc_points_std_deviation = 0.,
              total_loads_fsc = 0.,
              average_psc1_width = 0.,
              average_psc1_width_std_deviation = 0.,
              average_psc1_height = 0.,
              average_psc1_height_std_deviation = 0.,
              average_psc1_points = 0.,
              average_psc1_points_std_deviation = 0.,
              total_loads_psc1 = 0.,
              average_psc2_width = 0.,
              average_psc2_width_std_deviation = 0.,
              average_psc2_height = 0.,
              average_psc2_height_std_deviation = 0.,
              average_psc2_points = 0.,
              average_psc2_points_std_deviation = 0.,
              total_loads_psc2 = 0.,
              average_psc3_width = 0.,
              average_psc3_width_std_deviation = 0.,
              average_psc3_v1_width = 0.,
              average_psc3_v1_width_std_deviation = 0.,
              average_psc3_v2_width = 0.,
              average_psc3_v2_width_std_deviation = 0.,
              total_loads_psc3 = 0.;
      int num_fsc  = 0,
              num_fsc_points  = 0,
              num_psc1 = 0,
              num_psc1_points = 0,
              num_psc2 = 0,
              num_psc2_points = 0,
              num_psc3 = 0,
              num_psc3_points = 0,
              num_psc3_v1 = 0,
              num_psc3_v1_points = 0,
              num_psc3_v2 = 0,
              num_psc3_v2_points = 0;

      // Calculate Codelet Means/Stdev
        for (int i = 0; i < 2; i++) {
            for (auto const &c : cl) {
                switch (c->get_type()) {
                    case DDT::TYPE_FSC:
                        if (i == 0) {
                            num_fsc++;
                            average_fsc_width += c->col_width;
                            average_fsc_height += c->row_width;
                            average_fsc_points += c->row_width*c->col_width;
                            num_fsc_points += c->row_width*c->col_width;
                            total_loads_fsc += 6;
                        } else {
                            average_fsc_width_std_deviation += std::pow(c->col_width - average_fsc_width,2);
                            average_fsc_height_std_deviation += std::pow(c->row_width - average_fsc_height,2);
                            average_fsc_points_std_deviation += std::pow(c->row_width*c->col_width - average_fsc_points, 2);
                        }
                        break;
                    case DDT::TYPE_PSC1:
                        if (i == 0) {
                            num_psc1++;
                            average_psc1_width += c->col_width;
                            average_psc1_height += c->row_width;
                            average_psc1_points += c->col_width * c->row_width;
                            num_psc1_points += c->col_width * c->row_width;
                            total_loads_psc1 += 4 + c->row_width;
                        } else {
                            average_psc1_width_std_deviation += std::pow(c->col_width - average_psc1_width,2);
                            average_psc1_height_std_deviation += std::pow(c->row_width - average_psc1_height,2);
                            average_psc1_points_std_deviation += std::pow(c->col_width * c->row_width - average_psc1_points, 2);
                        }
                        break;
                    case DDT::TYPE_PSC2:
                        if (i == 0) {
                            num_psc2++;
                            average_psc2_width  += c->col_width;
                            average_psc2_height += c->row_width;
                            average_psc2_points += c->col_width*c->row_width;
                            num_psc2_points += c->col_width*c->row_width;
                            total_loads_psc2    += 5 + c->col_width;
                        } else {
                            average_psc2_width_std_deviation += std::pow(c->col_width - average_psc2_width,2);
                            average_psc2_height_std_deviation += std::pow(c->row_width - average_psc2_height,2);
                            average_psc2_points_std_deviation += std::pow(c->col_width*c->row_width - average_psc2_points, 2);
                        }
                        break;
                    case DDT::TYPE_PSC3:
                        if (i == 0) {
                            num_psc3++;
                            average_psc3_width += c->col_width;
                            total_loads_psc3   += 5 + c->col_width;
                            num_psc3_points += c->col_width * c->row_width;
                        } else {
                            average_psc3_width_std_deviation += std::pow(c->col_width - average_psc3_width,2);
                        }
                        break;
                    case DDT::TYPE_PSC3_V1:
                        if (i == 0) {
                            num_psc3_v1++;
                            average_psc3_v1_width += c->col_width;
                            num_psc3_v1_points += c->col_width * c->row_width;
                        } else {
                            average_psc3_v1_width_std_deviation += std::pow(c->col_width - average_psc3_v1_width,2);
                        }
                        break;
                    case DDT::TYPE_PSC3_V2:
                        if (i == 0) {
                            num_psc3_v2++;
                            average_psc3_v2_width += c->col_width;
                            num_psc3_v2_points += c->col_width * c->row_width;
                        } else {
                            average_psc3_v2_width_std_deviation += std::pow(c->col_width - average_psc3_v2_width,2);
                        }
                        break;
                    default:
                        throw std::runtime_error(
                                "Error: codelet type not recognized by "
                                "analyzer... Exiting... ");
                }
            }
            if (i == 0) {
                average_fsc_width /= std::max(num_fsc, 1);
                average_psc1_width /= std::max(num_psc1, 1);
                average_psc2_width /= std::max(num_psc2, 1);
                average_psc3_width /= std::max(num_psc3, 1);
                average_psc3_v1_width /= std::max(num_psc3_v1, 1);
                average_psc3_v2_width /= std::max(num_psc3_v2, 1);
                average_fsc_height /= std::max(num_fsc, 1);
                average_psc1_height /= std::max(num_psc1, 1);
                average_psc2_height /= std::max(num_psc2, 1);
                average_fsc_points /= std::max(num_fsc, 1);
                average_psc1_points /= std::max(num_psc1, 1);
                average_psc2_points /= std::max(num_psc2, 1);
            } else {
                average_fsc_width_std_deviation = std::sqrt(average_fsc_width_std_deviation/std::max(num_fsc,1));
                average_psc1_width_std_deviation = std::sqrt(average_psc1_width_std_deviation/std::max(num_psc1,1));
                average_psc2_width_std_deviation = std::sqrt(average_psc2_width_std_deviation/std::max(num_psc2,1));
                average_psc3_width_std_deviation = std::sqrt(average_psc3_width_std_deviation/std::max(num_psc3,1));
                average_psc3_v1_width_std_deviation = std::sqrt(average_psc3_v1_width_std_deviation/std::max(num_psc3_v1,1));
                average_psc3_v2_width_std_deviation = std::sqrt(average_psc3_v2_width_std_deviation/std::max(num_psc3_v2,1));
                average_fsc_height_std_deviation = std::sqrt(average_fsc_height_std_deviation/std::max(num_fsc,1));
                average_psc1_height_std_deviation = std::sqrt(average_psc1_height_std_deviation/std::max(num_psc1,1));
                average_psc2_height_std_deviation = std::sqrt(average_psc2_height_std_deviation/std::max(num_psc2,1));
                average_fsc_points_std_deviation = std::sqrt(average_fsc_points_std_deviation/std::max(num_fsc,1));
                average_psc1_points_std_deviation = std::sqrt(average_psc1_points_std_deviation/std::max(num_psc1,1));
                average_psc2_points_std_deviation = std::sqrt(average_psc2_points_std_deviation/std::max(num_psc2,1));
            }
        }

        csv_row_t row;
        CSV_ENTRY(row, config.matrixPath);
        CSV_ENTRY(row, config.nThread);
        CSV_ENTRY(row, DDT::clt_width);
        CSV_ENTRY(row, DDT::col_th);
        CSV_ENTRY(row, DDT::prefer_fsc);
        CSV_ENTRY(row, rows);
        CSV_ENTRY(row, cols);

        CSV_ENTRY(row, nnz);
        CSV_ENTRY(row, num_fsc);
        CSV_ENTRY(row, num_fsc_points);
        CSV_ENTRY(row, num_psc1);
        CSV_ENTRY(row, num_psc1_points);
        CSV_ENTRY(row, num_psc2);
        CSV_ENTRY(row, num_psc2_points);
        CSV_ENTRY(row, num_psc3);
        CSV_ENTRY(row, num_psc3_points);
        CSV_ENTRY(row, num_psc3_v1);
        CSV_ENTRY(row, num_psc3_v1_points);
        CSV_ENTRY(row, num_psc3_v2);
        CSV_ENTRY(row, num_psc3_v2_points);

        CSV_ENTRY(row, average_row_length);
        CSV_ENTRY(row, average_row_length_std_deviation);
        CSV_ENTRY(row, average_row_sequential_component);
        CSV_ENTRY(row, average_row_sequential_component_std_deviation);
        CSV_ENTRY(row, average_length_row_sequential_component);
        CSV_ENTRY(row, average_length_row_sequential_component_std_deviation);
        CSV_ENTRY(row, average_row_overlap);
        CSV_ENTRY(row, average_row_overlap_std_deviation);
        CSV_ENTRY(row, average_row_skew);
        CSV_ENTRY(row, average_row_skew_std_deviation);
        CSV_ENTRY(row, average_col_distance);
        CSV_ENTRY(row, average_col_distance_std_deviation);
        CSV_ENTRY(row, unique_row_patterns);
        CSV_ENTRY(row, average_num_unique_row_patterns);
        CSV_ENTRY(row, average_num_unique_row_patterns_std_deviation);
        CSV_ENTRY(row, (numSeqPoints / nnz));

        CSV_ENTRY(row, average_fsc_width);
        CSV_ENTRY(row, average_fsc_width_std_deviation);
        CSV_ENTRY(row, average_fsc_height);
        CSV_ENTRY(row, average_fsc_height_std_deviation);
        CSV_ENTRY(row, average_fsc_points);
        CSV_ENTRY(row, average_fsc_points_std_deviation);
        CSV_ENTRY(row, total_loads_fsc);

        CSV_ENTRY(row, average_psc1_width);
        CSV_ENTRY(row, average_psc1_width_std_deviation);
        CSV_ENTRY(row, average_psc1_height);
        CSV_ENTRY(row, average_psc1_height_std_deviation);
        CSV_ENTRY(row, average_psc1_points);
        CSV_ENTRY(row, average_psc1_points_std_deviation);
        CSV_ENTRY(row, total_loads_psc1);

        CSV_ENTRY(row, average_psc2_width);
        CSV_ENTRY(row, average_psc2_width_std_deviation);
        CSV_ENTRY(row, average_psc2_height);
        CSV_ENTRY(row, average_psc2_height_std_deviation);
        CSV_ENTRY(row, average_psc2_points);
        CSV_ENTRY(row, average_psc2_points_std_deviation);
        CSV_ENTRY(row, total_loads_psc2);

        CSV_ENTRY(row, average_psc3_width);
        CSV_ENTRY(row, average_psc3_width_std_deviation);

        CSV_ENTRY(row, average_psc3_v1_width);
        CSV_ENTRY(row, average_psc3_v1_width_std_deviation);

        CSV_ENTRY(row, average_psc3_v2_width);
        CSV_ENTRY(row, average_psc3_v2_width_std_deviation);
        CSV_ENTRY(row, total_loads_psc3);

        if (config.header) {
          std::cout << row.header.rdbuf() << std::endl;
        }

        std::cout << row.values.rdbuf() << std::endl;
        exit(0);
    }

    /**
     * @brief Calculates average run-time of given codelet
     *
     * @description Given a codelet c, this function runs the codelet
     * 100000 times to find the average run-time of the codelet given the
     * input vector x, matrix Ax and output vector y.
     *
     * @param y  Output vector for SpMV
     * @param x  Input vector for SpMV
     * @param m  Input sparse matrix for SpMV
     * @param c  Codelet to time on numerical method
     *
     * @return   Average run-time of codelet
     */
    double timeSingleCodelet(double*y, double* x, Matrix& m, Codelet *c) {
        const int NUM_RUNS = 1000000;

        const auto Ax = m.Lx;
        const auto Ai = m.Li;

        // Reset memory
        for (int i = 0; i < m.r; ++i) {
            y[i] = 0.;
        }
        for (int i = 0; i < m.c; ++i) {
            x[i] = 1.;
        }

        std::chrono::time_point<std::chrono::steady_clock> t1, t2;
        switch (c->get_type()) {
            case TYPE_FSC:
                t1 = std::chrono::steady_clock::now();
                for (int i = 0; i < NUM_RUNS; ++i) {
                    fsc_t2_2DC(y, Ax, x, c->row_offset, c->first_nnz_loc,
                               c->lbr, c->lbr + c->row_width, c->lbc,
                               c->col_width + c->lbc, c->col_offset);
                }
                t2 = std::chrono::steady_clock::now();
                break;
            case TYPE_PSC1:
                t1 = std::chrono::steady_clock::now();
                for (int i = 0; i < NUM_RUNS; ++i) {
                    psc_t1_2D4R(y, Ax, x, c->offsets, c->lbr,
                                c->lbr + c->row_width, c->lbc,
                                c->lbc + c->col_width);
                }
                t2 = std::chrono::steady_clock::now();
                break;
            case TYPE_PSC2:
                t1 = std::chrono::steady_clock::now();
                for (int i = 0; i < NUM_RUNS; ++i) {
                    psc_t2_2DC(y, Ax, x, c->offsets, c->row_offset,
                               c->first_nnz_loc, c->lbr,
                               c->lbr + c->row_width, c->col_width,
                               c->col_offset);
                }
                t2 = std::chrono::steady_clock::now();
                break;
            case TYPE_PSC3:
                t1 = std::chrono::steady_clock::now();
                for (int i = 0; i < NUM_RUNS; ++i) {
                    psc_t3_1D1R(y, Ax, Ai, x, c->offsets, c->lbr,
                                c->first_nnz_loc, c->col_width);
                }
                t2 = std::chrono::steady_clock::now();
                break;
            default:
                throw std::runtime_error("Error: codelet type not recognized...");
        }

        // Reset memory
        for (int i = 0; i < m.r; ++i) {
            y[i] = 0.;
        }
        for (int i = 0; i < m.c; ++i) {
            x[i] = 1.;
        }

        switch (c->get_type()) {
            case TYPE_FSC:
                    fsc_t2_2DC(y, Ax, x, c->row_offset, c->first_nnz_loc,
                               c->lbr, c->lbr + c->row_width, c->lbc,
                               c->col_width + c->lbc, c->col_offset);
                break;
                case TYPE_PSC1:
                        psc_t1_2D4R(y, Ax, x, c->offsets, c->lbr,
                                    c->lbr + c->row_width, c->lbc,
                                    c->lbc + c->col_width);
                    break;
                    case TYPE_PSC2:
                            psc_t2_2DC(y, Ax, x, c->offsets, c->row_offset,
                                       c->first_nnz_loc, c->lbr,
                                       c->lbr + c->row_width, c->col_width,
                                       c->col_offset);
                        break;
                        case TYPE_PSC3:
                                psc_t3_1D1R(y, Ax, Ai, x, c->offsets, c->lbr,
                                            c->first_nnz_loc, c->col_width);
                            break;
                            default:
                                throw std::runtime_error("Error: codelet type not recognized...");
        }

        return getTimeDifference(t1,t2) / NUM_RUNS;
    }

    /**
     * @brief Calculates the execution time for one or more codelets
     *
     * @invariant All codelet types in vector must be the same
     * @param y
     * @param x
     * @param m
     * @param cl
     *
     * @return Execution time in seconds for codelets in vector
     */
    double timeMultipleCodelet(double* y, double* x, Matrix& m, std::vector<Codelet*>& cl) {
        const int NUM_RUNS = 1000000;

        const auto Ax = m.Lx;
        const auto Ai = m.Li;

        // Reset memory
        for (int i = 0; i < m.r; ++i) {
            y[i] = 0.;
        }
        for (int i = 0; i < m.c; ++i) {
            x[i] = 1.;
        }

        std::chrono::time_point<std::chrono::steady_clock> t1, t2;
        switch (cl[0]->get_type()) {
            case TYPE_FSC:
                t1 = std::chrono::steady_clock::now();
                for (int i = 0; i < NUM_RUNS; ++i) {
                    for (auto const& c : cl) {
                        fsc_t2_2DC(y, Ax, x, c->row_offset, c->first_nnz_loc,
                                   c->lbr, c->lbr + c->row_width, c->lbc,
                                   c->col_width + c->lbc, c->col_offset);
                    }
                }
                t2 = std::chrono::steady_clock::now();
                break;
                case TYPE_PSC1:
                    t1 = std::chrono::steady_clock::now();
                    for (int i = 0; i < NUM_RUNS; ++i) {
                        for (auto const& c : cl) {
                            psc_t1_2D4R(y, Ax, x, c->offsets, c->lbr,
                                        c->lbr + c->row_width, c->lbc,
                                        c->lbc + c->col_width);
                        }
                    }
                    t2 = std::chrono::steady_clock::now();
                    break;
                    case TYPE_PSC2:
                        t1 = std::chrono::steady_clock::now();
                        for (int i = 0; i < NUM_RUNS; ++i) {
                            for (auto const& c : cl) {
                                psc_t2_2DC(y, Ax, x, c->offsets, c->row_offset,
                                           c->first_nnz_loc, c->lbr,
                                           c->lbr + c->row_width, c->col_width,
                                           c->col_offset);
                            }
                        }
                        t2 = std::chrono::steady_clock::now();
                        break;
                        case TYPE_PSC3:
                            t1 = std::chrono::steady_clock::now();
                            for (int i = 0; i < NUM_RUNS; ++i) {
                                for (auto const& c : cl) {
                                    psc_t3_1D1R(y, Ax, Ai, x, c->offsets,
                                                c->lbr, c->first_nnz_loc,
                                                c->col_width);
                                }
                            }
                            t2 = std::chrono::steady_clock::now();
                            break;
                            default:
                                throw std::runtime_error("Error: codelet type not recognized...");
        }

        // Reset memory
        for (int i = 0; i < m.r; ++i) {
            y[i] = 0.;
        }
        for (int i = 0; i < m.c; ++i) {
            x[i] = 1.;
        }

        switch (cl[0]->get_type()) {
            case TYPE_FSC:
                for (auto const& c : cl) {
                    fsc_t2_2DC(y, Ax, x, c->row_offset, c->first_nnz_loc,
                               c->lbr, c->lbr + c->row_width, c->lbc,
                               c->col_width + c->lbc, c->col_offset);
                }
                break;
                case TYPE_PSC1:
                    for (auto const& c : cl) {
                        psc_t1_2D4R(y, Ax, x, c->offsets, c->lbr,
                                    c->lbr + c->row_width, c->lbc,
                                    c->lbc + c->col_width);
                    }
                    break;
                    case TYPE_PSC2:
                        for (auto const& c : cl) {
                            psc_t2_2DC(y, Ax, x, c->offsets, c->row_offset,
                                       c->first_nnz_loc, c->lbr,
                                       c->lbr + c->row_width, c->col_width,
                                       c->col_offset);
                        }
                        break;
                        case TYPE_PSC3:
                            for (auto const& c : cl) {
                                psc_t3_1D1R(y, Ax, Ai, x, c->offsets,
                                            c->lbr, c->first_nnz_loc,
                                            c->col_width);
                            }
                            break;
                            default:
                                throw std::runtime_error("Error: codelet type not recognized...");
        }

        return getTimeDifference(t1,t2) / NUM_RUNS;
    }

    void printCodeletComparisonStatsHeader() {
        std::cout << "CODELET_TYPE,HEIGHT,WIDTH,SEQUENTIAL_COMPONENTS,FSC_EXECUTION_TIME,PSC1v1_EXECUTION_TIME,PSC1v2_EXECUTION_TIME,PSC2_EXECUTION_TIME,\n";
    }

    void printCodeletStatsHeader() {
        std::cout << "CODELET_TYPE,EXECUTION_TIME,WIDTH,HEIGHT,SEQUENTIAL_COMPONENTS\n";
    }

    /**
     * @brief Returns string associated with codelet type enum
     * @param cl Codelet object
     *
     * @return String associated with DDT::CodeletTypes enum
     */
    std::string getCodeletTypeString(DDT::Codelet* cl) {
        switch (cl->get_type()) {
            case DDT::TYPE_FSC:
                return "FSC";
            case DDT::TYPE_PSC1:
                return "PSC1";
            case DDT::TYPE_PSC2:
                return "PSC2";
            case DDT::TYPE_PSC3:
                return "PSC3";
            default:
                throw std::runtime_error("Error: Codelet Type not supported...");
        }
    }

    /**
     * @brief Converts a PSC type 1v1 into one or more FSC codelets
     *
     * @param c PSC type 1v1 to be converted
     *
     * @return Vector of new codelets
     */
    std::vector<DDT::Codelet*> convert_PSC1v1_FSC(DDT::Codelet* c) {
        std::vector<DDT::Codelet*> fscCodelets;
        for (int i = 0; i < c->row_width-1; i+=2) {
            fscCodelets.push_back(new FSCCodelet(c->lbr+i, c->lbc, 2,c->col_width,c->offsets[i],c->offsets[i+1]-c->offsets[i],c->col_offset));
        }
        for (int i = 0; i < c->row_width; i++) {
            fscCodelets.push_back(new FSCCodelet(c->lbr+i, c->lbc, 2,c->col_width,c->offsets[i],c->offsets[i+1]-c->offsets[i],c->col_offset));
        }
        return fscCodelets;
    }

    std::vector<DDT::Codelet*> convert_PSC1v1_PSC1v2(DDT::Codelet* c) {
        std::vector<DDT::Codelet*> psc1v2Codelets;
        for (int i = 0; i < c->row_width-1; i+=2) {
            auto offsets = new int[c->col_width]();
            for (int j = 0; j < c->col_width; ++j) {
                offsets[j] = c->lbc+j;
            }
            psc1v2Codelets.push_back(new PSCT2V1(c->lbr+i, c->row_width,c->col_width,c->first_nnz_loc,c->offsets[i+1]-c->offsets[i],c->col_offset, offsets));
        }
        for (int i = 0; i < c->row_width; i++) {
            auto offsets = new int[c->col_width]();
            for (int j = 0; j < c->col_width; ++j) {
                offsets[j] = c->lbc+j;
            }
            psc1v2Codelets.push_back(new PSCT2V1(c->lbr+i, c->row_width,c->col_width,c->first_nnz_loc,c->offsets[i+1]-c->offsets[i],c->col_offset, offsets));
        }
        return psc1v2Codelets;
    }

    std::vector<DDT::Codelet*> convert_PSC1v1_PSC2(DDT::Codelet* c) {
        std::vector<DDT::Codelet*> psc2Codelets;
        for (int i = 0; i < c->row_width-1; i++) {
            auto offsets = new int[c->col_width]();
            for (int j = 0; j < c->col_width; ++j) {
                offsets[j] = c->lbc+j;
            }
            psc2Codelets.push_back(new PSCT3V1(c->lbr+i, c->col_width, c->offsets[i], offsets));
        }
        return psc2Codelets;
    }

    /**
     * @brief
     * @param c
     * @return
     */
    std::vector<DDT::Codelet*> convert_FSC_PS1v2(DDT::Codelet* c) {
        auto offsets = new int[c->col_width]();
        for (int i = 0; i < c->col_width; ++i) {
            offsets[i] = c->lbc + i;
        }
        return std::vector<DDT::Codelet*>{ new PSCT2V1(c->lbr, c->row_width, c->col_width, c->first_nnz_loc, c->row_offset, c->col_offset, offsets) };
    }

    /**
     * @brief
     * @param c
     * @return
     */
    std::vector<DDT::Codelet*> convert_FSC_PS1v1(DDT::Codelet* c) {
        auto offsets = new int[c->row_width]();
        for (int i = 0; i < c->row_width; ++i) {
            offsets[i] = c->first_nnz_loc + c->row_offset*i;
        }
        return std::vector<DDT::Codelet*>{ new PSCT1V1(c->lbr, c->lbc, c->row_width, c->col_width, c->col_offset, offsets) };
    }

    /**
     * @brief
     * @param c
     * @return
     */
    std::vector<DDT::Codelet*> convert_FSC_PS2(DDT::Codelet* c) {
        std::vector<DDT::Codelet*> psc2Codelets;
        for (int i = 0; i < c->row_width; ++i) {
            auto offsets = new int[c->col_width]();
            for (int j = 0; j < c->col_width; ++j) {
                offsets[j] = c->lbc + j;
            }
            psc2Codelets.push_back(new PSCT3V1(c->lbr+i, c->col_width, c->first_nnz_loc+i*c->row_offset, offsets));
        }
        return psc2Codelets;
    }

    /**
     * @brief
     * @param c
     * @return
     */
    std::vector<DDT::Codelet*> convert_PSC1v2_FSC(DDT::Codelet* c) {
        std::vector<DDT::Codelet*> fscCodelets;
        int totalWidth = 0;
        for (int i = 0, cnt = 0; i < c->col_width; ++i) {
            if (i == (c->col_width-1) || (c->offsets[i+1] - c->offsets[i]) != 1) {
                assert(cnt < c->col_width);
                totalWidth += i+1-cnt;
                fscCodelets.push_back(new FSCCodelet(c->lbr, c->offsets[cnt],c->row_width, i+1-cnt,c->first_nnz_loc+cnt,c->row_offset, c->col_offset));
                cnt = i+1;
            }
        }
        assert(totalWidth == c->col_width);
        return fscCodelets;
    }

    /**
     * @brief
     * @param c
     * @return
     */
    std::vector<DDT::Codelet*> convert_PSC1v2_PSC1v1(DDT::Codelet* c) {
        std::vector<DDT::Codelet*> psc1v1Codelets;
        for (int i = 0, cnt = 0; i < c->col_width; ++i) {
            if ((i == (c->col_width-1)) || (c->offsets[i+1] - c->offsets[i] != 1)) {
                auto offsets = new int[c->row_width]();
                for (int j = 0; j < c->row_width; ++j) {
                    offsets[j] = c->first_nnz_loc+cnt + c->row_offset*j;
                }
                psc1v1Codelets.push_back(new PSCT1V1(c->lbr, c->offsets[cnt],c->row_width, i+1-cnt, c->col_offset, offsets));
                cnt = i+1;
            }
        }
        return psc1v1Codelets;
    }

    /**
     * @brief EZ
     * @param c
     * @return
     */
    std::vector<DDT::Codelet*> convert_PSC1v2_PSC2(DDT::Codelet* c) {
        std::vector<DDT::Codelet*> psc2Codelets;
        for (int i = 0; i < c->row_width; ++i) {
            auto offsets = new int[c->col_width]();
            for (int j = 0; j < c->col_width; ++j) {
                offsets[j] = c->offsets[j] + c->col_offset*i;
            }
            psc2Codelets.push_back(new PSCT3V1(c->lbr, c->col_width, c->first_nnz_loc, offsets));
        }
        return psc2Codelets;
    }

    /**
     * @brief
     * @param c
     * @return
     */
    std::vector<DDT::Codelet*> convert_PSC2_FSC(DDT::Codelet* c) {
        std::vector<DDT::Codelet*> fscCodelets;
        for (int i = 0; i < c->col_width; ++i) {
            fscCodelets.push_back(new FSCCodelet(c->lbr, c->lbc, 1,1,c->first_nnz_loc+i,c->row_width, c->col_offset));
        }
        return fscCodelets;
    }

    /**
     * @brief
     * @param c
     * @return
     */
    std::vector<DDT::Codelet*> convert_PSC2_PSC1v1(DDT::Codelet* c) {
        std::vector<DDT::Codelet*> psc1v1Codelets;
        for (int i = 0; i < c->col_width; ++i) {
            auto offsets = new int[1]();
            *offsets = c->first_nnz_loc;
            psc1v1Codelets.push_back(new PSCT1V1(c->lbr, c->lbc+i, 1,1,0,offsets));
        }
        return psc1v1Codelets;
    }

    /**
     * @brief Converts a PSC type 2 into a PSC type 1v2
     * @param c Codelet to be converted
     * @return Vector of PSC type 1v2s that process the iteration space of c.
     */
    std::vector<DDT::Codelet*> convert_PSC2_PSC1v2(DDT::Codelet* c) {
        std::vector<DDT::Codelet*> psc1v1Codelets;

        return std::vector<DDT::Codelet*> { new PSCT2V1(c->lbr, c->lbc, c->col_width, c->first_nnz_loc, c->row_offset, c->col_offset, c->offsets) };
    }

    /**
     * @brief Returns number of regions in codelet with unit strides
     *
     * @description A strided region is one where the offset between two memory
     * tuples from each dimension is 0 or 1. This code finds all regions where
     * adjacent elements have 0/1 strides.
     *
     * @param cl Codelet to check
     * @return Number of strided regions in codelet with 0 or 1 as stride
     */
    int getSequentialComponents(DDT::Codelet* cl) {
        int sc = 1;
        if (cl->get_type() == DDT::TYPE_FSC || cl->get_type() == DDT::TYPE_PSC1) {
            return sc;
        }
        for (int i = 0; i < cl->col_width-1; ++i) {
            if (cl->offsets[i+1]-cl->offsets[i] != 1) {
                sc++;
            }
        }
        return sc;
    }

    void printCodeletStats(double* y, double* x, Matrix& m, Codelet *cl) {
        auto codeletRuntime = timeSingleCodelet(y,x,m,cl);
        auto sc = getSequentialComponents(cl);
        std::cout
                << getCodeletTypeString(cl) << ","
                << codeletRuntime << ","
                << cl->row_width  << ","
                << cl->col_width  << ","
                << sc << ",\n";
    }

    bool isCorrect(const double*y, const double* yTrue, int rows) {
        for (int i = 0; i < rows; ++i) {
            if (std::abs(y[i]-yTrue[i]) > 0.001) {
                std::cout << i << ":" << y[i] << "," << yTrue[i] << std::endl;
                assert(std::abs(y[i]-yTrue[i]) > 0.001);
                return false;
            }
        }
        return true;
    }


    /**
     * @brief Gets accurate run-time of iteration space contained in c
     * @description Converts given codelet into all different types of codelets
     * and returns a vector of all different execution times of the iteration
     * space in 'c' when evaluating it using different codelet types.
     *
     * @param c
     * @return vector<double> of execution times in the order: [FSC, PSC1v1, PSC1v2, PSC2]
     */
    std::vector<double> timeCodeletComparison(double*y, double* x, Matrix& m, DDT::Codelet* c) {
        std::vector<double> codeletTimes;

        auto yCmp = new double[m.r]();

        double fscTime, psc1v1Time, psc1v2Time, psc2Time;
        switch (c->get_type()) {
            case TYPE_FSC: {
                fscTime = timeSingleCodelet(y, x, m, c);
                auto psc1v1Codelets = convert_FSC_PS1v1(c);
                psc1v1Time = timeMultipleCodelet(y, x, m, psc1v1Codelets);
                auto psc1v2Codelets = convert_FSC_PS1v2(c);
                psc1v2Time = timeMultipleCodelet(y, x, m, psc1v2Codelets);
                auto psc2Codelets = convert_FSC_PS2(c);
                psc2Time = timeMultipleCodelet(y, x, m, psc2Codelets);

                cleanPointerVectors(psc1v1Codelets, psc1v2Codelets, psc2Codelets);

                return std::vector<double>{fscTime, psc1v1Time, psc1v2Time,
                                           psc2Time};
            }
            case TYPE_PSC1: {
                auto fscCodelets = convert_PSC1v1_FSC(c);
                auto fscTime = timeMultipleCodelet(y, x, m, fscCodelets);
                psc1v1Time = timeSingleCodelet(y, x, m, c);
                auto psc1v2Codelets = convert_PSC1v1_PSC1v2(c);
                auto psc1v2Time =
                        timeMultipleCodelet(y, x, m, psc1v2Codelets);
                auto psc2Codelets = convert_PSC1v1_PSC2(c);
                auto psc2Time =
                        timeMultipleCodelet(y, x, m, psc2Codelets);

                // Clean up memory
                cleanPointerVectors(fscCodelets, psc1v2Codelets, psc2Codelets);

                return std::vector<double>{fscTime, psc1v1Time, psc1v2Time,
                                           psc2Time};
            }
            case TYPE_PSC2: {
                auto psc1v2Time = timeSingleCodelet(y, x, m, c);
                std::copy(y, y+m.r, yCmp);
                auto fscCodelets = convert_PSC1v2_FSC(c);
                auto fscTime = timeMultipleCodelet(yCmp, x, m, fscCodelets);
                auto psc1v1Codelets = convert_PSC1v2_PSC1v1(c);
                auto psc1v1Time = timeMultipleCodelet(y, x, m, psc1v1Codelets);
                auto psc2Codelets = convert_PSC1v2_PSC2(c);
                auto psc2Time =
                        timeMultipleCodelet(y, x, m, psc2Codelets);

                // Clean up memory
                cleanPointerVectors(fscCodelets, psc1v1Codelets, psc2Codelets);

                return std::vector<double>{fscTime, psc1v1Time, psc1v2Time,
                                           psc2Time};
            }
            case TYPE_PSC3: {
                auto fscCodelets = convert_PSC2_FSC(c);
                auto fscTime = timeMultipleCodelet(y, x, m, fscCodelets);
                auto psc1v1Codelets = convert_PSC2_PSC1v1(c);
                auto psc1v1Time = timeMultipleCodelet(y, x, m, psc1v1Codelets);
                auto psc1v2Codelets = convert_PSC2_PSC1v2(c);
                auto psc1v2Time = timeMultipleCodelet(y, x, m, psc1v2Codelets);
                auto psc2Time = timeSingleCodelet(y, x, m, c);

                // Clean up memory
                cleanPointerVectors(fscCodelets, psc1v1Codelets, psc1v2Codelets);

                return std::vector<double>{fscTime, psc1v1Time, psc1v2Time,
                                           psc2Time};
            }
            default:
                throw std::runtime_error("Error: codelet type not recognized...");
        }
        delete[] yCmp;
    }

    /**
     * @brief Prints run-time evalulation of various codelet types for iteration space
     *
     * @param y
     * @param x
     * @param m
     * @param cl
     */
    void printCodeletComparisonStats(double* y, double* x, Matrix& m, Codelet *cl) {
        auto codeletRunTimes = timeCodeletComparison(y,x,m,cl);
        auto sc = getSequentialComponents(cl);
        std::cout
        << getCodeletTypeString(cl) << ","
        << cl->row_width  << ","
        << cl->col_width  << ","
        << sc << ",";
        for (auto const& c : codeletRunTimes) {
            std::cout << c << ",";
        }
        std::cout << "\n";
    }

    /**
     * @brief Prints out information about codelets found in a sparsity pattern
     *
     * @description Goes over every codelet inside a matrix sparsity pattern
     * and numerical method and prints out detailed statistics per codelet
     * on efficiencies, size and run-times.
     *
     * @param d The global object containing runtime information
     * @param cll The list of codelets associated with each thread
     * @param config The global configuration object
     *
     * @note This function with exit the program
     */
    void analyzeCodeletExection(const DDT::GlobalObject& d, const std::vector<DDT::Codelet*>* cll, const DDT::Config& config) {
        // Allocate memory and read matrix for timings
        auto m = readSparseMatrix<CSR>(config.matrixPath);
        auto y = new double[m.r]();
        auto x = new double[m.c]();

//        printCodeletStatsHeader();
        printCodeletComparisonStatsHeader();
        for (int i = 0; i < config.nThread; ++i) {
            for (auto const&  cl : cll[i]) {
//                printCodeletStats(y,x,m,cl);
                printCodeletComparisonStats(y,x,m,cl);
            }
        }

        delete[] y;
        delete[] x;

        exit(0);
    }
}
