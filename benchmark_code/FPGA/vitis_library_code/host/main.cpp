/*
 * Copyright 2019 Xilinx, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <cstring>
#include <iostream>
#include <iomanip>
#include <string>
#include <thread>
#include <unistd.h>
#include <vector>
#include <chrono>
#include <cassert>

// This file is required for OpenCL C++ wrapper APIs
#include "blas/L1/tests/sw/include/binFiles.hpp"
#include "blas/L1/tests/sw/include/utils.hpp"
#include "hpc/L2/include/sw/fpga.hpp"
#include "spmvKernel.hpp"

#include "xbutil.h"

using namespace std;

std::string tokenize(string s, string del = "/")
{
    int start = 0;
    int end = s.find(del);
    while (end != -1) {
        start = end + del.size();
        end = s.find(del, start);
    }
    return s.substr(start, end - start);
}

/*
 * Erase First Occurrence of given  substring from main string.
 */
void eraseSubStr(std::string & mainStr, const std::string & toErase)
{
    // Search for the substring in string
    size_t pos = mainStr.find(toErase);
    if (pos != std::string::npos)
    {
        // If found then erase it from string
        mainStr.erase(pos, toErase.length());
    }
}

void split_mtxName(std::string & l_mtxName, char **matrix_name, unsigned int *nr_rows, unsigned int *nr_cols, unsigned int *nr_nzeros, double *avg_nnz_per_row, double *std_nnz_per_row, char **placement, double *avg_bw, double *skew, double *avg_num_neighbours, double *cross_row_similarity, char **distribution, unsigned int *seed)
{
    std::string s = l_mtxName;
    std::string delimiter = "_";

    int cnt = 0;
    size_t pos = 0;
    std::string token;    

    while ((pos = s.find(delimiter)) != std::string::npos) {
        token = s.substr(0, pos);
        // std::cout << cnt << ")\t" << token << std::endl;
        s.erase(0, pos + delimiter.length());
        if(cnt == 0){
            int n = token.length();
            *matrix_name = (char*)malloc((n+1)*sizeof(char));
            strcpy(*matrix_name, token.c_str());
        }
        if(cnt == 1)
            *nr_rows = std::stoi(token);
        if(cnt == 2)
            *nr_cols = std::stoi(token);
        if(cnt == 3)
            *nr_nzeros = std::stoi(token);
        if(cnt == 4){
            eraseSubStr(token, "avg");
            *avg_nnz_per_row = std::stod(token);
        }
        if(cnt == 5){
            eraseSubStr(token, "std");
            *std_nnz_per_row = std::stod(token);
        }
        if(cnt == 6){
            int n = token.length();
            *placement = (char*)malloc((n+1)*sizeof(char));
            strcpy(*placement, token.c_str());
        }
        if(cnt == 7){
            eraseSubStr(token, "bw");
            *avg_bw = std::stod(token);
        }
        if(cnt == 8){
            eraseSubStr(token, "skew");
            *skew = std::stod(token);
        }
        if(cnt == 9){
            eraseSubStr(token, "neigh");
            *avg_num_neighbours = std::stod(token);
        }
        if(cnt == 10){
            eraseSubStr(token, "cross");
            *cross_row_similarity = std::stod(token);
        }
        cnt++;
    }

    if(cnt == 11){
        eraseSubStr(s, ".mtx");
        std::string distr(1, s[0]);
        if(distr.compare("n")){ // normal distribution
            std::string s_n("normal");
            int n = s_n.length();
            *distribution = (char*)malloc((n+1)*sizeof(char));
            strcpy(*distribution, s_n.c_str());
        } 
        else if(distr.compare("g")){ // gamma distribution
            std::string s_g("gamma");
            int n = s_g.length();
            *distribution = (char*)malloc((n+1)*sizeof(char));
            strcpy(*distribution, s_g.c_str());
        } 
        eraseSubStr(s, distr);
        *seed = std::stoi(s);
    }
}

int main(int argc, char** argv) {
    if (argc < 5 || argc > 14) {
        cout << "Usage: " << argv[0] << " <XCLBIN File> <sigature_path> <vector_path> <mtx_name> [numRuns] [device id]" << endl;
        return EXIT_FAILURE;
    }

    int l_idx = 1;

    string l_xclbinFile = argv[l_idx++];
    string l_sigPath = argv[l_idx++];
    string l_vecPath = argv[l_idx++];
    string l_mtxName = argv[l_idx++];
    unsigned int l_numRuns = 1;
    if (argc > l_idx) l_numRuns = atoi(argv[l_idx++]);

    char *matrix_name, *placement, *distribution;
    unsigned int nr_rows, nr_cols, nr_nzeros, seed;
    double avg_nnz_per_row, std_nnz_per_row, avg_bw, skew, avg_num_neighbours, cross_row_similarity;

    split_mtxName(l_mtxName, &matrix_name, &nr_rows, &nr_cols, &nr_nzeros, &avg_nnz_per_row, &std_nnz_per_row, &placement, &avg_bw, &skew, &avg_num_neighbours, &cross_row_similarity, &distribution, &seed);

    string l_sigFilePath = l_sigPath + "/" + l_mtxName;
    string l_vecFilePath = l_vecPath + "/" + l_mtxName;

    string l_sigFileNames[SPARSE_hbmChannels + 2];
    string l_vecFileNames[3];

    for (unsigned int i = 0; i < SPARSE_hbmChannels; ++i) {
        l_sigFileNames[i] = l_sigFilePath + "/nnzVal_" + to_string(i) + ".dat";
    }
    l_sigFileNames[SPARSE_hbmChannels] = l_sigFilePath + "/parParam.dat";
    l_sigFileNames[SPARSE_hbmChannels + 1] = l_sigFilePath + "/rbParam.dat";

    l_vecFileNames[0] = l_vecFilePath + "/inVec.dat";
    l_vecFileNames[1] = l_vecFilePath + "/refVec.dat";
    l_vecFileNames[2] = l_vecFilePath + "/outVec.dat";

    // I/O Data Vectors
    host_buffer_t<uint8_t> l_nnzBuf[SPARSE_hbmChannels];
    host_buffer_t<uint8_t> l_paramBuf[2];
    host_buffer_t<uint8_t> l_vecBuf[2];
    host_buffer_t<uint8_t> l_outVecBuf;

    vector<uint32_t> l_info(6);
    readBin<uint32_t>(l_sigFilePath + "/info.dat", 6 * sizeof(uint32_t), l_info.data());

    if(l_info[0] == 0){
        // non-existent matrix! perhaps, because it was larger than 256 MB! Abort immediately
        std::cout << "\tWARNING! SOMETHING IS LARGER THAN 256 MB! It will break.\n";
        cout << "DATA_CSV:," << l_mtxName << "," << 0 << "," << 0 << "," << 0;
        cout << "," << 0 << "," << 0 << "," << 0 << "," << 0;
        cout << "," << l_numRuns << "," << 0 << "," << 0 << "," << 0 << "," << 0 << "," << 0;
        cout << endl;
        std::cout << "Some nonzero partition(s) is(are) larger than 256 MB. Exiting early!\n";
        return EXIT_FAILURE;
    }
    
    double l_padOverhead = ((double)(l_info[5])-(double)(l_info[2]))*100/(double)(l_info[2]);

    double mem_footprint = 0;
    bool flag256 = false;
    for (unsigned int i = 0; i < SPARSE_hbmChannels + 2; ++i) {
        size_t l_bytes = getBinBytes(l_sigFileNames[i]);
        mem_footprint += (double)l_bytes;
        if(l_bytes/(1024.0*1024) > 256.0 && flag256==false){
            std::cout << "\tWARNING! SOMETHING IS LARGER THAN 256 MB! It will break.\n";
            flag256 = true;
        }
    }
    mem_footprint = mem_footprint/(1024*1024); // convert it to MBs
    
    // Some nonzero partition is larger than 256 MB. Therefore it can not be transferred to HBM channel. Report 0 in performance and exit early!
    if(flag256 == true){
        cout << "DATA_CSV:," << l_mtxName << "," << l_info[0] << "," << l_info[1] << "," << l_info[2];
        cout << "," << l_info[3] << "," << l_info[4] << "," << l_info[5] << "," << l_padOverhead;
        cout << "," << l_numRuns << "," << 0 << "," << 0 << "," << 0 << "," << 0 << "," << 0;
        cout << endl;
        std::cout << "Some nonzero partition(s) is(are) larger than 256 MB. Exiting early!\n";
        return EXIT_FAILURE;
    }

    for (unsigned int i = 0; i < SPARSE_hbmChannels + 2; ++i) {
        size_t l_bytes = getBinBytes(l_sigFileNames[i]);
        if (i < SPARSE_hbmChannels) 
            readBin<uint8_t, aligned_allocator<uint8_t> >(l_sigFileNames[i], l_bytes, l_nnzBuf[i]);
        else 
            readBin<uint8_t, aligned_allocator<uint8_t> >(l_sigFileNames[i], l_bytes, l_paramBuf[i - SPARSE_hbmChannels]);
        std::cout << tokenize(l_sigFileNames[i], "/") << "\t" << std::setprecision(3) << l_bytes/(1024.0*1024) << " MB\n";
    }

    for (unsigned int i = 0; i < 2; ++i) {
        size_t l_bytes = getBinBytes(l_vecFileNames[i]);
        readBin<uint8_t, aligned_allocator<uint8_t> >(l_vecFileNames[i], l_bytes, l_vecBuf[i]);
        std::cout << tokenize(l_vecFileNames[i], "/") << "\t" << std::setprecision(3) << l_bytes/(1024.0*1024) << " MB\n";
        if(l_bytes/(1024.0*1024) > 256.0){
            std::cout << "\tWARNING! LARGER THAN 256 MB! It will break.\n";
            flag256 = true;
            // return EXIT_FAILURE;
        }
    }
    
    // some nonzero partition is larger than 256 MB. therefore it can not be transferred to HBM channel. Report 0 in performance and exit early!
    if(flag256 == true){
            cout << "DATA_CSV:," << l_mtxName << "," << l_info[0] << "," << l_info[1] << "," << l_info[2];
            cout << "," << l_info[3] << "," << l_info[4] << "," << l_info[5] << "," << l_padOverhead;
            cout << "," << l_numRuns << "," << 0 << "," << 0 << "," << 0 << "," << 0 << "," << mem_footprint;
            cout << endl;
            std::cout << "Some nonzero partition(s) is(are) larger than 256 MB. Exiting early!\n";
            return EXIT_FAILURE;
    }

    unsigned int l_yRows = l_vecBuf[1].size() / sizeof(SPARSE_dataType);
    size_t l_outVecBytes = l_yRows * sizeof(SPARSE_dataType);
    l_outVecBuf.resize(l_outVecBytes);

    int l_deviceId = 0;
    if (argc > l_idx) 
        l_deviceId = atoi(argv[l_idx++]);

    FPGA l_fpga(l_deviceId);
    l_fpga.xclbin(l_xclbinFile);

    // /mnt/various/vitis-workspace/2/Vitis_Libraries/blas/L2/src/xcl2/xcl2.cpp
    KernelLoadNnz<SPARSE_hbmChannels> l_kernelLoadNnz(&l_fpga);
    l_kernelLoadNnz.getCU("loadNnzKernel:{krnl_loadNnz}");
    // std::cout << "getCU(\"loadNnzKernel:{krnl_loadNnz}\")\t\t";
    l_kernelLoadNnz.setMem(l_numRuns, l_nnzBuf); // edw kollaei
    // std::cout << "->\tPREPARED l_kernelLoadNnz" << "\n";

    KernelLoadCol l_kernelLoadParX(&l_fpga);
    l_kernelLoadParX.getCU("loadParXkernel:{krnl_loadParX}");
    // std::cout << "getCU(\"loadParXkernel:{krnl_loadParX}\")\t\t";
    l_kernelLoadParX.setMem(l_numRuns, l_paramBuf[0], l_vecBuf[0]);
    // std::cout << "->\tPREPARED l_kernelLoadParX" << "\n";

    KernelLoadRbParam l_kernelLoadRbParam(&l_fpga);
    l_kernelLoadRbParam.getCU("loadRbParamKernel:{krnl_loadRbParam}");
    // std::cout << "getCU(\"loadRbParamKernel:{krnl_loadRbParam}\")\t";
    l_kernelLoadRbParam.setMem(l_numRuns, l_paramBuf[1]);
    // std::cout << "->\tPREPARED l_kernelLoadRbParam" << "\n";

    KernelStoreY l_kernelStoreY(&l_fpga);
    l_kernelStoreY.getCU("storeYkernel:{krnl_storeY}");
    // std::cout << "getCU(\"storeYkernel:{krnl_storeY}\")\t\t";
    l_kernelStoreY.setArgs(l_numRuns, l_yRows, l_outVecBuf);
    // std::cout << "->\tPREPARED l_kernelStoreY" << "\n";

    vector<Kernel> l_kernels;
    l_kernels.push_back(l_kernelLoadRbParam);
    l_kernels.push_back(l_kernelLoadParX);
    l_kernels.push_back(l_kernelLoadNnz);
    l_kernels.push_back(l_kernelStoreY);

    // char **argv_new = makeArgv(2, "-d", "0");
    // char **argv_new = (char**)malloc((2+1) * sizeof(char*));
    // argv_new[0] = (char*)malloc(3*sizeof(char));
    // strcpy(argv_new[0],"-d\0");
    // argv_new[1] = (char*)malloc(2*sizeof(char));
    // strcpy(argv_new[1],"0\0");
    // argv_new[2] = NULL;
    // std::cout << "ARGV_NEW = " << argv_new[0] << " " << argv_new[1] << "\n";
    // xcldev::xclTop_wrapper(3, argv_new);

    double l_runTime = Kernel::run(l_kernels);
    double l_runTime_iter = (double)l_runTime * 1000 / l_numRuns;

    l_kernelStoreY.getMem();

    writeBin<uint8_t>(l_vecFileNames[2], l_yRows * sizeof(SPARSE_dataType),
                      reinterpret_cast<uint8_t*>(&(l_outVecBuf[0])));

    double gflops = 2*double(l_info[2])/((l_runTime_iter)*1e-3)*1e-9;
    double gflops_padded = 2*double(l_info[5])/((l_runTime_iter)*1e-3)*1e-9;
    
    cout << "DATA_CSV:," << l_mtxName << "," << l_info[0] << "," << l_info[1] << "," << l_info[2];
    cout << "," << l_info[3] << "," << l_info[4] << "," << l_info[5] << "," << l_padOverhead;
    cout << "," << l_numRuns << "," << l_runTime << "," << l_runTime_iter << "," << gflops << "," << gflops_padded << "," << mem_footprint;
    cout << endl;

    // fprintf(stderr, "synthetic,");
    // fprintf(stderr, "%s,", distribution);
    // fprintf(stderr, "%s,", placement);
    // fprintf(stderr, "%d,", seed);
    // fprintf(stderr, "%ld,", nr_rows);
    // fprintf(stderr, "%ld,", nr_cols);
    // fprintf(stderr, "%ld,", nr_nzeros);
    //     fprintf(stderr, "%lf,", csr->density);
    //     fprintf(stderr, "%lf,", csr->mem_footprint);
    //     fprintf(stderr, "%s,", csr->mem_range);
    // fprintf(stderr, "%lf,", avg_nnz_per_row);
    // fprintf(stderr, "%lf,", std_nnz_per_row);
    //     fprintf(stderr, "%lf,", csr->avg_bw);
    //     fprintf(stderr, "%lf,", csr->std_bw);
    //     fprintf(stderr, "%lf,", csr->avg_bw_scaled);
    //     fprintf(stderr, "%lf,", csr->std_bw_scaled);
    //     fprintf(stderr, "%lf,", csr->avg_sc);
    //     fprintf(stderr, "%lf,", csr->std_sc);
    //     fprintf(stderr, "%lf,", csr->avg_sc_scaled);
    //     fprintf(stderr, "%lf,", csr->std_sc_scaled);
    // fprintf(stderr, "%lf,", skew);
    // fprintf(stderr, "%lf,", avg_num_neighbours);
    // fprintf(stderr, "%lf,", cross_row_similarity);
    // fprintf(stderr, "Xilinx_SpMV,");
    // fprintf(stderr, "%lf,", l_runTime_iter);
    // fprintf(stderr, "%lf,", gflops_padded); 
    // long W_avg = 33;
    // fprintf(stderr, "%ld,", W_avg);
    // double J_estimated = W_avg*l_runTime_iter;
    // fprintf(stderr, "%lf\n", J_estimated);

    int l_errs = 0;
    compare<SPARSE_dataType>(l_yRows, reinterpret_cast<SPARSE_dataType*>(l_outVecBuf.data()), reinterpret_cast<SPARSE_dataType*>(l_vecBuf[1].data()), l_errs, true);
    if (l_errs == 0) {
        cout << "INFO: Test pass!" << endl;
        return EXIT_SUCCESS;
    } else {
        cout << "ERROR: Test failed! Out of total " << l_yRows << " entries, there are " << l_errs << " mismatches."
             << endl;
        return EXIT_FAILURE;
    }
}
