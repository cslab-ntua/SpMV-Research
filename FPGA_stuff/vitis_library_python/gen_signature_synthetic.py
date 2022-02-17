# Copyright 2019 Xilinx, Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

 # Copyright 2019 Xilinx, Inc.
 #
 # Licensed under the Apache License, Version 2.0 (the "License");
 # you may not use this file except in compliance with the License.
 # You may obtain a copy of the License at
 #
 #     http://www.apache.org/licenses/LICENSE-2.0
 #
 # Unless required by applicable law or agreed to in writing, software
 # distributed under the License is distributed on an "AS IS" BASIS,
 # WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 # See the License for the specific language governing permissions and
 # limitations under the License.
 
import numpy as np
import scipy.io as sio
import scipy.sparse as sp
import os
import subprocess
import argparse
import math
from matrix_params_synthetic import *
from signature_synthetic import *

import sys
# sys.path.append("/various/pmpakos/vitis-workspace/2/Vitis_Libraries/sparse/L2/tests/fp64/spmv/python/")
# from artificial_matrix_generation import *
# from artificial_matrix_generation_v2 import *

import gc

def partition_matrix(nr_rows, nr_cols, avg_nnz_per_row, std_nnz_per_row, distribution, placement, avg_bw, skew_coeff, avg_num_neighbors, cross_row_similarity, seed, precision, verbose, maxRows, maxCols, channels, parEntries, accLatency, memBits, sigPath, vecPath):
    start_total = time.time()
    l_sig = signature(parEntries, accLatency, channels, maxRows, maxCols, memBits)
    
    # start = time.time()
    flag_abort, mtxName = l_sig.process_synthetic(nr_rows, nr_cols, avg_nnz_per_row, std_nnz_per_row, distribution, placement, avg_bw, skew_coeff, avg_num_neighbors, cross_row_similarity, seed, precision, verbose, vecPath, parEntries)
    # end = time.time()
    # print("\t\tPARTITION - process\t\t",round(end - start,3))
    
    if(flag_abort == True):
        end_total = time.time()
        print("INFO: matrix {} partition FAILED!!!!".format(mtxName))
        print("MATRIX FINISHED. \t\tTOTAL TIME :",round(end_total-start_total,3))
        mtxVecPath = vecPath+'/'+mtxName
        if os.path.exists(mtxVecPath):
            subprocess.run(["rm", "-rf", mtxVecPath])
        del l_sig
        print_mem_usage()
        return flag_abort

    # start = time.time()
    mtxSigPath = sigPath+'/'+mtxName
    if not os.path.exists(mtxSigPath):
        subprocess.run(["mkdir", "-p", mtxSigPath])
    l_rbParamFileName = mtxSigPath+'/rbParam.dat'
    l_sig.store_rbParam(l_rbParamFileName)
    # end = time.time()
    # print("\t\tPARTITION - store_rbParam\t",round(end - start,3))
    
    # start = time.time()
    l_parParamFileName = mtxSigPath+'/parParam.dat'
    l_sig.store_parParam(l_parParamFileName)
    # end = time.time()
    # print("\t\tPARTITION - store_parParam\t",round(end - start,3))
    
    start = time.time()
    l_nnzFileNames = []
    for i in range(channels):
        l_nnzFileNames.append(mtxSigPath+'/nnzVal_' + str(i) + '.dat')
    l_sig.store_nnz(l_nnzFileNames)
    end = time.time()
    print("\tstore_nnz\t\t",round(end - start,3))
    
    l_infoFileName = mtxSigPath+'/info.dat'
    l_sig.store_info(l_infoFileName)

    end_total = time.time()
    print("INFO: matrix {} partition done.".format(mtxName))
    # print("      Original      m, n, nnzs = {}, {}, {}".format(l_sig.m, l_sig.n, l_sig.nnz))
    # print("      After padding m, n, nnzs = {}, {}, {}".format(l_sig.mPad, l_sig.nPad, l_sig.nnzPad))
    print("      Original (Padded)  m, n, nnzs = {}({}), {}({}), {}({})".format(l_sig.m, l_sig.mPad, l_sig.n, l_sig.nPad, l_sig.nnz, l_sig.nnzPad))
    print("      Padding overhead is {} %".format(round((l_sig.nnzPad-l_sig.nnz)*100/l_sig.nnz,3)))
    print("MATRIX FINISHED. \t\tTOTAL TIME :",round(end_total-start_total,3))

    gc.collect()
    del l_sig.nnzStore
    del l_sig.parParam
    del l_sig.rbParam
    del l_sig

def check_signature(mtxName, mtxFullName, maxRows, maxCols, channels, parEntries, accLatency, memBits, mtxSigPath):
    l_nnzFileNames = []
    for i in range(channels):
        l_nnzFileNames.append(mtxSigPath+'/nnzVal_' + str(i) + '.dat')

    l_parParamFileName = mtxSigPath+'/parParam.dat'
    l_rbParamFileName = mtxSigPath+'/rbParam.dat'
    l_infoFileName = mtxSigPath+'/info.dat'
    
    l_sig = signature(parEntries, accLatency, channels, maxRows, maxCols, memBits)
    l_sig.load_rbParam(l_rbParamFileName)
    l_sig.load_parParam(l_parParamFileName)
    l_sig.load_nnz(l_nnzFileNames)
    l_sig.load_info(l_infoFileName)
    if l_sig.check(mtxFullName, mtxName):
        print("INFO: {} signature verification pass!".format(mtxName))
        return True 
    else:
        print("ERROR: {} signature verification failed!".format(mtxName))
        return False
    
def process_matrices(isPartition, isClean, isCheck, mtxList, mtx_param_list, maxRows, maxCols, channels, parEntries, accLatency, memBits, sigPath, vecPath):
    downloadList = open(mtx_param_list, 'r')
    downloadNames = downloadList.readlines()
    l_equal = True
    for line in downloadNames:
        line = line.strip("\n")
        if(len(line)==0): #empty line
            continue
        print('-------')
        print(line)
        line = line.split(" ")
        nr_rows, nr_cols, avg_nnz_per_row, std_nnz_per_row, distribution, placement, avg_bw, skew_coeff, avg_num_neighbors, cross_row_similarity, seed = int(line[0]), int(line[1]), float(line[2]), float(line[3]), str(line[4]), str(line[5]), float(line[6]), float(line[7]), float(line[8]), float(line[9]), int(line[10])
        precision = 64
        verbose = 0

        # placement_full = placement
        # if(placement=="diagonal"): # need to add diagonal_factor too
        #     if(d_f>1):
        #         print("Diagonal Factor is greater than 1. I have to stop now!")
        #         return
        #     placement_full += "_"+str(d_f)
        # can't use +"_"+str(nr_nnz) in mtxName.... anyway
        # mtxName =  "synthetic_"+\
        #             str(nr_rows)+"_"+str(nr_cols)+\
        #             "_avg"+str(avg_nnz_per_row)+"_std"+str(std_nnz_per_row)+\
        #             "_"+placement_full+\
        #             "_"+distribution[0]+str(seed)
        # print(mtxName)
        # mtxSigPath = sigPath+'/'+mtxName
        # mtxVecPath = vecPath+'/'+mtxName
        # if not path.exists(mtxSigPath):
        #     subprocess.run(["mkdir", "-p", mtxSigPath])
        # if not path.exists(mtxVecPath):
        #     subprocess.run(["mkdir", "-p", mtxVecPath])
        if isPartition:
            flag_abort = partition_matrix(nr_rows, nr_cols, avg_nnz_per_row, std_nnz_per_row, distribution, placement, avg_bw, skew_coeff, avg_num_neighbors, cross_row_similarity, seed, precision, verbose, maxRows, maxCols, channels, parEntries, accLatency, memBits, sigPath, vecPath)
            # print_mem_usage()

        if(flag_abort==True):
            print("flag_abort = TRUE!!!!")
        # if isCheck:
        #     l_equal = l_equal and check_signature(mtxName, mtxFullName, maxRows, maxCols, channels, parEntries, accLatency, memBits, mtxSigPath)
        # if isClean:
        #     subprocess.run(["rm", "-rf", './mtx_files/'+mtxName+'/'])
    # if isClean:
    #     subprocess.run(["rm", "-rf", "./mtx_files"])
    if isCheck and l_equal:
        print("All check pass!")

def decode_mtx_sig(mtxSigPath, maxRows, maxCols, channels, parEntries, accLatency, memBits, txtPath):
    if not path.exists(mtxSigPath):
        print("ERROR: {} directory doesn't exist.".format(mtxPath))
        return
    subprocess.run(["mkdir", "-p", txtPath])
    l_nnzFileNames, l_nnzTxtFileNames = [],[]
    for i in range(channels):
        l_nnzFileNames.append(mtxSigPath+'/nnzVal_' + str(i) + '.dat')
        l_nnzTxtFileNames.append(txtPath+'/nnzVal_' + str(i) + '.txt')

    l_parParamFileName = mtxSigPath+'/parParam.dat'
    l_parParamTxtFileName = txtPath+'/parParam.txt'
    l_rbParamFileName = mtxSigPath+'/rbParam.dat'
    l_rbParamTxtFileName = txtPath+'/rbParam.txt'
    l_infoFileName = mtxSigPath+'/info.dat'
    l_infoTxtFileName = txtPath+'/info.txt'
    l_sig = signature(parEntries, accLatency, channels, maxRows, maxCols, memBits)

    start = time.time()
    l_sig.load_rbParam(l_rbParamFileName)
    end = time.time()
    print("\t\tDECODE - load_rbParam\t\t",round(end - start,3))
    

    start = time.time()
    l_sig.print_rbParam(l_rbParamTxtFileName)
    end = time.time()
    print("\t\tDECODE - print_rbParam\t\t",round(end - start,3))

    start = time.time()
    l_sig.load_parParam(l_parParamFileName)
    end = time.time()
    print("\t\tDECODE - load_parParam\t\t",round(end - start,3))

    start = time.time()
    l_sig.print_parParam(l_parParamTxtFileName)
    end = time.time()
    print("\t\tDECODE - print_parParam\t\t",round(end - start,3))

    start = time.time()
    l_sig.load_nnz(l_nnzFileNames)
    end = time.time()
    print("\t\tDECODE - load_nnz\t\t",round(end - start,3))

    start = time.time()
    l_sig.print_nnz(l_nnzTxtFileNames)
    end = time.time()
    print("\t\tDECODE - print_nnz\t\t",round(end - start,3))

    start = time.time()
    l_sig.load_info(l_infoFileName)
    end = time.time()
    print("\t\tDECODE - load_info\t\t",round(end - start,3))

    start = time.time()
    l_sig.print_info(l_infoTxtFileName)
    end = time.time()
    print("\t\tDECODE - print_info\t\t",round(end - start,3))


def main(args):
    if (args.usage):
        print('Usage example:')
        print('python3 gen_signature.py --partition [--clean] --mtx_list ./test_matrices.txt --sig_path ./sig_dat')
        print('python3 gen_signature.py --check  --mtx_list ./test_matrices.txt --sig_path ./sig_dat')
        print('python3 gen_signature.py --decode --mtx_path ./sig_dat/mtx_name  --txt_path ./txt_out/mtx_name')
    elif (args.decode):
        decode_mtx_sig(args.mtx_path, args.max_rows, args.max_cols, args.channels, args.par_entries,args.acc_latency, args.mem_bits, args.txt_path)
    else:
        process_matrices(args.partition, args.clean, args.check, args.mtx_list, args.mtx_param_list, args.max_rows, args.max_cols, args.channels, args.par_entries, args.acc_latency, args.mem_bits, args.sig_path, args.vec_path)

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='partition sparse matrix, verify partitions and decode partiton info')
    parser.add_argument('--usage',action='store_true',help='print usage example')
    parser.add_argument('--partition',action='store_true',help='partition sparse matrix across HBM channels')
    parser.add_argument('--clean',action='store_true',help='clean up downloaded .mtx file after the run')
    parser.add_argument('--check',action='store_true',help='check partitions against orginial matrices')
    parser.add_argument('--mtx_list',type=str,help='a file containing URLs for downloading sprase matrices')
    parser.add_argument('--mtx_param_list',type=str,help='a file containing synthetic matrix parameters')
    parser.add_argument('--max_rows',type=int,default=4096,help='maximum number of rows in each channel block, default value 4096')
    parser.add_argument('--max_cols',type=int,default=4096,help='maximum number of cols in each channel block, default value 4096')
    parser.add_argument('--channels',type=int,default=16,help='number of HBM channels, default value 16')
    parser.add_argument('--par_entries',type=int,default=4,help='number of NNZ entries retrieved from one HBM channel')
    parser.add_argument('--acc_latency',type=int,default=8,help='number of cycles used for double precision accumulation')
    parser.add_argument('--mem_bits',type=int,default=256,help='number of bits in each HBM channel access')
    parser.add_argument('--sig_path',type=str,default='./sig_dat',help='directory for storing partition results, default value ./sig_dat')

    parser.add_argument('--vec_path',type=str,default='./vec_dat',help='directory for storing vector results, default value ./vec_dat')

    parser.add_argument('--decode',action='store_true',help='print signature files into text files')
    parser.add_argument('--mtx_path',type=str,help='directory for matrix signature data')
    parser.add_argument('--txt_path',type=str,default='./txt_out',help='directory for storing text files, default value ./txt_out')
    args = parser.parse_args()
    main(args)
