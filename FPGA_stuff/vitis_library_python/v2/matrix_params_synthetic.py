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
import ctypes
from cffi import FFI

import numpy as np
import scipy.io as sio
import scipy.sparse as sp
import math

import sys
sys.path.append("/various/pmpakos/vitis-workspace/2/Vitis_Libraries/sparse/L2/tests/fp64/spmv/python_exp/")
from artificial_matrix_generation import *
from artificial_matrix_generation_v2 import *

import multiprocessing as mp
from multiprocessing import shared_memory
from multiprocessing import Process, Array

from tqdm import tqdm

import os, psutil

def print_mem_usage():
    print("MEMORY USED :", psutil.Process(os.getpid()).memory_info().rss / 1024 ** 2, "MB")
#######################################################################################################

class row_block_param:
    def __init__(self, memBits, channels):
        self.channels = channels
        self.memBytes = memBits // 8
        self.totalRows = 0
        self.totalRbs = 0
        self.buf = bytearray()

    def add_rbIdxInfo(self, p_minRowId, p_minColId, p_numCols, p_numPars):
        int32Arr = np.zeros(self.memBytes // 4, dtype=np.uint32)
        int32Arr[0:4] = [p_minRowId, p_minColId, p_numCols, p_numPars]
        self.buf.extend(int32Arr.tobytes())

    def add_rbSizeInfo(self, p_numRows, p_numNnzs):
        int32Arr = np.zeros(self.memBytes // 4, dtype=np.uint32)
        int32Arr[0:2] = [p_numRows, p_numNnzs]
        self.buf.extend(int32Arr.tobytes())

    def add_dummyInfo(self):
        int32Arr = np.zeros(self.memBytes // 4, dtype=np.uint32)
        self.buf.extend(int32Arr.tobytes())

    def add_chInfo16(self, p_info):
        chInfo16Arr = np.zeros(self.channels, dtype=np.uint16)
        for i in range(self.channels):
            chInfo16Arr[i] = p_info[i]
        self.buf.extend(chInfo16Arr.tobytes())

    def add_chInfo32(self, p_info):
        chInfo32Arr = np.zeros(self.channels, dtype=np.uint32)
        for i in range(channels):
            chInfo32Arr[i] = p_info[i]
        self.buf.extend(chInfo32Arr.tobytes())
        
    def get_rb_offset(self, p_rbId):
        l_offset = self.memBytes
        l_offset += p_rbId * (self.memBytes*8)
        return l_offset
        
    def get_rbInfo(self, p_rbId, p_rbInfoId):
        l_size = self.memBytes // 4
        l_offset = self.get_rb_offset(p_rbId)
        l_offset += p_rbInfoId*self.memBytes
        l_infoArr = np.frombuffer(self.buf, dtype=np.uint32, count=l_size, offset=l_offset)
        return l_infoArr
    
    def set_rbColInfo(self, p_rbId, p_minColId, p_numCols):
        l_offset = self.get_rb_offset(p_rbId)
        int32Arr = np.frombuffer(self.buf, dtype=np.uint32, count=self.memBytes // 4, offset=l_offset)
        int32Arr[1:3] = [p_minColId, p_numCols]
        self.buf[l_offset : l_offset+self.memBytes] = int32Arr.tobytes() 

    def set_numPars(self, p_rbId, p_numPars):
        l_offset = self.get_rb_offset(p_rbId)
        int32Arr = np.frombuffer(self.buf, dtype=np.uint32, count=self.memBytes // 4, offset=l_offset)
        int32Arr[3] = p_numPars
        self.buf[l_offset : l_offset+self.memBytes] = int32Arr.tobytes()

    def set_numNnzs(self, p_rbId, p_numNnzs):
        l_offset = self.get_rb_offset(p_rbId)
        l_offset += self.memBytes
        int32Arr = np.frombuffer(self.buf, dtype=np.uint32, count=self.memBytes // 4, offset=l_offset)
        int32Arr[1] = p_numNnzs
        self.buf[l_offset : l_offset+self.memBytes] = int32Arr.tobytes() 

    def get_chInfo16(self, p_rbId, p_chInfo16Id):
        l_offset = self.get_rb_offset(p_rbId)
        l_offset += self.memBytes * (2 + p_chInfo16Id)
        l_chInfo16 = np.frombuffer(self.buf, dtype=np.uint16, count=self.channels, offset=l_offset)
        return l_chInfo16

    def set_chInfo16(self, p_rbId, p_chInfo16Id, p_info):
        chInfo16Arr = np.zeros(self.channels, dtype=np.uint16)
        for i in range(self.channels):
            chInfo16Arr[i] = p_info[i]
        l_offset = self.get_rb_offset(p_rbId)
        l_offset += self.memBytes * (2 + p_chInfo16Id)
        self.buf[l_offset:l_offset+self.memBytes] = chInfo16Arr.tobytes()

    def get_chInfo32(self, p_rbId):
        l_offset = self.get_rb_offset(p_rbId)
        l_offset += self.memBytes * 4
        l_chInfo32 = np.frombuffer(self.buf, dtype=np.uint32, count=self.channels, offset=l_offset)
        return l_chInfo32

    def set_chInfo32(self, p_rbId, p_info):
        chInfo32Arr = np.zeros(self.channels, dtype=np.uint32)
        for i in range(self.channels):
            chInfo32Arr[i] = p_info[i]
        l_offset = self.get_rb_offset(p_rbId)
        l_offset += self.memBytes * 4
        self.buf[l_offset:l_offset+self.channels*4] = chInfo32Arr.tobytes()

    def write_file(self, fileName):
        fo = open(fileName, "wb")
        int32Arr = np.zeros(self.memBytes//4, dtype=np.uint32)
        int32Arr[0] = self.totalRows
        int32Arr[1] = self.totalRbs
        self.buf[:self.memBytes] = int32Arr.tobytes()
        fo.write(self.buf)
        fo.close()

    def read_file(self, fileName):
        fi = open(fileName, "rb")
        self.buf = fi.read()
        int32Arr = np.frombuffer(self.buf, dtype=np.uint32, count=self.memBytes // 4, offset=0)
        self.totalRows = int32Arr[0]
        self.totalRbs = int32Arr[1]
        fi.close()

    def print_file(self, fileName):
        fo = open(fileName, "w")
        fo.write("Total rows and rbs are: {}, {}\n".format(self.totalRows, self.totalRbs))
        for i in range(self.totalRbs):
            fo.write("\nRb {} info:\n".format(i))
            l_offset = self.memBytes + i*8*self.memBytes
            int32Arr = np.frombuffer(self.buf, dtype=np.uint32, count=self.memBytes//4, offset=l_offset)
            fo.write("  startRowId={}\n".format(int32Arr[0]))
            int32Arr = np.frombuffer(self.buf, dtype=np.uint32, count=self.memBytes//4, offset=l_offset+self.memBytes)
            # fo.write("  rows={}\n".format(int32Arr[0]))
            fo.write("  rows={}\n".format(int32Arr[0]))
            fo.write("  nonzeros={}\n".format(int32Arr[1]))
            chInt16Arr=np.frombuffer(self.buf, dtype=np.uint16, count=self.channels, offset=l_offset+2*self.memBytes)
            fo.write("  startRowId for channel 0-15: {}\n".format(chInt16Arr))
            chInt16Arr=np.frombuffer(self.buf, dtype=np.uint16, count=self.channels, offset=l_offset+3*self.memBytes)
            fo.write("  rows in channel 0-15: {}\n".format(chInt16Arr))
            chInt32Arr=np.frombuffer(self.buf, dtype=np.uint32, count=self.channels, offset=l_offset+4*self.memBytes)
            fo.write("  nnzs in channel 0-15: {}\n".format(chInt32Arr))
        fo.close()

class par_param:
    def __init__(self, memBits, channels):
        self.memBytes = memBits//8
        self.channels = channels
        self.totalPars = 0
        self.buf = bytearray()
    
    def add_chInfo16(self, p_info):
        chInfo16Arr = np.zeros(self.channels, dtype=np.uint16)
        for i in range(self.channels):
            chInfo16Arr[i] = p_info[i]
        self.buf.extend(chInfo16Arr.tobytes())

    def add_chInfo32(self, p_info):
        chInfo32Arr = np.zeros(self.channels, np.uint32)
        for i in range(self.channels):
            chInfo32Arr[i] = p_info[i]
        self.buf.extend(chInfo32Arr.tobytes())

    def add_parInfo(self, p_baseColAddr, p_colBks, p_rows, p_nnzs):
        int32Arr = np.zeros(self.memBytes//4, dtype=np.uint32)
        int32Arr[0:4] = [p_baseColAddr, p_colBks, p_rows, p_nnzs]
        self.buf.extend(int32Arr.tobytes())

    def add_dummyInfo(self):
        int32Arr = np.zeros(self.memBytes//4, dtype=np.uint32)
        self.buf.extend(int32Arr.tobytes())

    def get_par_offset(self, p_parId):
        l_offset = self.memBytes + p_parId * 8 * self.memBytes
        return l_offset

    def get_chInfo16(self, p_parId, p_chInfo16Id):
        l_offset = self.get_par_offset(p_parId)
        l_offset += (5+p_chInfo16Id)*self.memBytes
        l_chInfo16 = np.frombuffer(self.buf, dtype=np.uint16, count=self.channels, offset=l_offset)
        return l_chInfo16

    def set_chInfo16(self, p_parId, p_chInfo16Id, p_info):
        chInfo16Arr = np.zeros(self.channels, dtype=np.uint16)
        for i in range(self.channels):
            chInfo16Arr[i] = p_info[i]
        l_offset = self.get_par_offset(p_parId)
        l_offset += (5+p_chInfo16Id)*self.memBytes
        self.buf[l_offset:l_offset+2*self.channels] = chInfo16Arr.tobytes()

    def get_chInfo32(self, p_parId, p_chInfo32Id):
        l_offset = self.get_par_offset(p_parId)
        l_offset += p_chInfo32Id * self.memBytes*2 
        l_chInfo32 = np.frombuffer(self.buf, dtype=np.uint32, count=self.channels, offset=l_offset)
        return l_chInfo32

    def set_chInfo32(self, p_parId, p_chInfo32Id, p_info):
        chInfo32Arr = np.zeros(self.channels, dtype=np.uint32)
        for i in range(self.channels):
            chInfo32Arr[i] = p_info[i]
        l_offset = self.get_par_offset(p_parId)
        l_offset += p_chInfo32Id * chInfo32Arr.shape[0]*4
        self.buf[l_offset:l_offset+4*self.channels] = chInfo32Arr.tobytes()

    def get_parInfo(self, p_parId):
        l_offset = self.get_par_offset(p_parId)
        l_offset += 4*self.memBytes
        l_int32Arr = np.frombuffer(self.buf, dtype=np.uint32, count=self.memBytes // 4, offset=l_offset)
        return l_int32Arr

    def set_parInfo(self, p_parId, p_info):
        int32Arr = np.zeros(self.memBytes//4, dtype=np.uint32)
        for i in range(self.memBytes // 4):
            int32Arr[i] = p_info[i]
        l_offset = self.get_par_offset(p_parId)
        l_offset += 4*self.memBytes
        self.buf[l_offset: l_offset+self.memBytes] = sef.int32Arr.tobytes()


    def write_file(self, filename):
        int32Arr = np.zeros(self.memBytes//4, dtype=np.uint32)
        int32Arr[0] = self.totalPars
        self.buf[:self.memBytes] = int32Arr.tobytes()
        fo = open(filename, "wb")
        fo.write(self.buf)
        fo.close() 

    def read_file(self, filename):
        fi = open(filename, "rb")
        self.buf = fi.read()
        int32Arr = np.frombuffer(self.buf, dtype=np.uint32, count=self.memBytes//4, offset=0)
        self.totalPars = int32Arr[0]
        fi.close()

    def print_file(self, filename):
        fo = open(filename, "w")
        fo.write("Total num of partitions: {}\n".format(self.totalPars))
        for i in range(self.totalPars):
            fo.write("\nPartition {}:\n".format(i))
            l_offset = self.memBytes + i*8*self.memBytes
            chInt32Arr = np.frombuffer(self.buf, dtype=np.uint32, count=self.channels, offset=l_offset)
            fo.write("  num of cols in {} channels: {}\n".format(self.channels, chInt32Arr))
            chInt32Arr = np.frombuffer(self.buf, dtype=np.uint32, count=self.channels, offset=l_offset+2*self.memBytes)
            fo.write("  num of nnzs in {} channels: {}\n".format(self.channels, chInt32Arr))
            int32Arr = np.frombuffer(self.buf, dtype=np.uint32, count=self.memBytes//4, offset=l_offset+4*self.memBytes)
            # fo.write("  min colBkId: {}\n".format(int32Arr[0]))
            fo.write("  min colBkId: {}\n".format(int32Arr[0]))
            fo.write("  total_nonzeros: {}\n".format(int32Arr[3]))
            chInt16Arr = np.frombuffer(self.buf, dtype=np.uint16, count=self.channels, offset=l_offset+5*self.memBytes)
            fo.write("  min colBkId offset for {} channels: {}\n".format(self.channels, chInt16Arr))
            chInt16Arr = np.frombuffer(self.buf, dtype=np.uint16, count=self.channels, offset=l_offset+6*self.memBytes)
            fo.write("  number of colBks for {} channels: {}\n".format(self.channels, chInt16Arr))
        fo.close()
 
class nnz_store:
    def __init__(self, memBits, parEntries, accLatency, channels):
        self.memBytes = memBits // 8
        self.parEntries = parEntries
        self.accLatency = accLatency
        self.channels = channels
        self.totalBks = [0]*channels
        self.totalRowIdxBks = [0]*channels
        self.totalColIdxBks = [0]*channels
        self.totalNnzBks = [0]*channels
        self.buf = [] 
        for i in range(channels):
            self.buf.append(bytearray())
  
    def add_dummyInfo(self, p_chId):
        int32Arr = np.zeros(self.memBytes//4, dtype=np.uint32)
        self.buf[p_chId].extend(int32Arr.tobytes())
 
    def add_idxArr(self, p_chId, p_idxArr):
        int16Arr = np.zeros(self.memBytes//2, dtype=np.uint16)
        for i in range(self.memBytes // 2):
            int16Arr[i] = p_idxArr[i]
        self.buf[p_chId].extend(int16Arr.tobytes())

    def add_nnzArr(self, p_chId,  p_nnzArr):
        float64Arr = np.zeros(self.memBytes//8, dtype=np.float64)
        for i in range(self.memBytes // 8):
            float64Arr[i] = p_nnzArr[i]
        self.buf[p_chId].extend(float64Arr.tobytes())

    def get_chPar(self, p_chId, p_offset, p_nnzIdx, p_nnzs, p_sRowId, p_sColId):
        l_memIdxWidth = self.memBytes//2
        l_rowIdxGap = self.parEntries * self.accLatency
        l_rowIdxMod = l_memIdxWidth * l_rowIdxGap
        l_colIdxMod = l_memIdxWidth * self.parEntries
        l_row,l_col,l_data=[],[],[]
        l_offset,l_nnzIdx,l_nnzs = p_offset,p_nnzIdx,p_nnzs
        while l_nnzs > 0 :
            if l_nnzIdx % l_rowIdxMod == 0:
                l_rowIdx = np.frombuffer(self.buf[p_chId], dtype=np.uint16, count=l_memIdxWidth, offset=l_offset)
                for i in range(l_memIdxWidth):
                    l_row.extend([l_rowIdx[i]+p_sRowId]*l_rowIdxGap)
                l_offset += self.memBytes
            if l_nnzIdx % l_colIdxMod == 0:
                l_colIdx = np.frombuffer(self.buf[p_chId], dtype=np.uint16, count=l_memIdxWidth, offset=l_offset)
                for i in range(l_memIdxWidth):
                    for j in range(self.parEntries):
                        l_col.append((l_colIdx[i]+p_sColId)*self.parEntries+j)
                l_offset += self.memBytes
            l_data.extend(np.frombuffer(self.buf[p_chId], dtype=np.float64, count=self.parEntries, offset=l_offset))
            l_offset += self.memBytes
            l_nnzIdx += self.parEntries
            l_nnzs -= self.parEntries
        return [l_row,l_col,l_data, l_offset]

    def write_file(self, filenames):
        for i in tqdm(range(self.channels)):
        # for i in range(self.channels):
            assert self.totalBks[i] == (self.totalRowIdxBks[i]+self.totalColIdxBks[i]+self.totalNnzBks[i])
            int32Arr = np.zeros(self.memBytes//4, dtype=np.uint32)
            int32Arr[0:4] = [self.totalBks[i], self.totalRowIdxBks[i], self.totalColIdxBks[i], self.totalNnzBks[i]]
            self.buf[i][:self.memBytes] = int32Arr.tobytes()

            # print("\tself.buf["+str(i)+"] size =", round(len(self.buf[i])*1.0/(1024*1024.0),3),"MB")
            fo = open(filenames[i], "wb")
            fo.write(self.buf[i])
            fo.close()
        
        del self.buf

        ################################################################################################################################################################
        # mp.freeze_support() # https://stackoverflow.com/questions/24374288/where-to-put-freeze-support-in-a-python-script
        # n_processes = self.channels
        # with closing(mp.Pool(n_processes, initializer=_init_parallel_fun, initargs=())) as p:
        #     workload_augmented = []
        #     for i in range(self.channels):
        #         filename = filenames[i]
        #         totalBks = self.totalBks[i]
        #         totalRowIdxBks = self.totalRowIdxBks[i]
        #         totalColIdxBks = self.totalColIdxBks[i]
        #         totalNnzBks = self.totalNnzBks[i]
        #         memBytes = self.memBytes
        #         buff = self.buf[i]                
        #         workload_augmented.append([filename, totalBks, totalRowIdxBks, totalColIdxBks, totalNnzBks, memBytes, buff])
        #     p.map(parallel_function, workload_augmented)
        #     # Close the processes.
        # p.join()
        ################################################################################################################################################################


    def read_file(self, filenames):
        for i in range(self.channels):
            int32Arr = np.zeros(self.memBytes//4, dtype=np.uint32)
            fi = open(filenames[i], "rb")
            self.buf[i] = fi.read()
            int32Arr = np.frombuffer(self.buf[i], dtype=np.uint32, count=self.memBytes//4, offset=0)
            [self.totalBks[i], self.totalRowIdxBks[i], self.totalColIdxBks[i], self.totalNnzBks[i]] = int32Arr[0:4]
            assert self.totalBks[i] == (self.totalRowIdxBks[i]+self.totalColIdxBks[i]+self.totalNnzBks[i])
            fi.close()

    def print_file(self, filenames):
        l_rowIdxMod = self.accLatency * (self.memBytes//2)
        l_colIdxMod = self.memBytes//2
        for i in range(self.channels):
            fo = open(filenames[i], "w")
            fo.write("Total Bks, RowIdxBks, ColIdxBks, NNzBks: {}, {}, {}, {}\n".format(self.totalBks[i], self.totalRowIdxBks[i], self.totalColIdxBks[i], self.totalNnzBks[i]))
            bk = 0
            l_offset = self.memBytes
            while bk < self.totalNnzBks[i]:
                if bk % l_rowIdxMod == 0:
                    chInt16Arr = np.frombuffer(self.buf[i], dtype=np.uint16, count=self.memBytes//2, offset=l_offset)
                    fo.write("\nRow Idx for {} channels: {}\n".format(self.channels, chInt16Arr))
                    l_offset += self.memBytes
                if bk % l_rowIdxMod == 0:
                    chInt16Arr = np.frombuffer(self.buf[i], dtype=np.uint16, count=self.memBytes//2, offset=l_offset)
                    fo.write("Col Idx for {} channels: {}\n".format(self.channels, chInt16Arr))
                    l_offset += self.memBytes
                float64Arr = np.frombuffer(self.buf[i], dtype=np.float64, count=self.parEntries, offset=l_offset)
                fo.write("NNZ val for BK {}: {}\n".format(bk, float64Arr))
                l_offset += self.memBytes
                bk += 1
            fo.close()

class sparse_matrix:
    def __init__(self):
        self.m,self.n,self.nnz = 0,0,0
        self.minColId, self.minRowId = 0,0
        self.mtxName=""
    
    def __del__(self):
        # print("DELETING SPARSE_MATRIX")
        del self.row
        del self.col
        del self.data

    def read_matrix(self, mtxFullName, mtxName):
        mat = sio.mmread(mtxFullName)
        if sp.issparse(mat):
            mat.eliminate_zeros()
            self.row = mat.row.astype(np.uint32)
            self.col = mat.col.astype(np.uint32)
            self.data = mat.data.astype(np.float64)
            self.mtxName = mtxName
            self.m,self.n = mat.shape
            self.nnz = mat.nnz
            self.minRowId = np.amin(self.row)
            self.minColId = np.amin(self.col)
            return True
        else:
            return False

    # def create_synthetic_matrix(self, mtxName, nr_rows, avg_nnz_per_row, std_nnz_per_row, distribution, placement, d_f, seed, precision, verbose):
    def create_synthetic_matrix(self, nr_rows, avg_nnz_per_row, std_nnz_per_row, distribution, placement, avg_bw_target, skew_coeff, seed, precision, verbose):
        #####################################################################################################
        ffi = FFI()
        ffi.cdef(
            """
            struct csr_matrix {
                int * row_ptr;
                int * col_ind;
                double * values;

                unsigned int nr_rows;
                unsigned int nr_cols;
                unsigned int nr_nzeros;

                double density;
                double mem_footprint;
                int precision;
                char mem_range[128];

                double avg_nnz_per_row;
                double std_nnz_per_row;
                double min_nnz_per_row;
                double max_nnz_per_row;

                double skew;

                int seed;
                char * distribution;
                char * placement;
                double bandwidth_scaled;

                double avg_bw;
                double std_bw;
                double avg_sc;
                double std_sc;
            };
            struct csr_matrix * artificial_matrix_generation(long nr_rows, long nr_cols, double avg_nnz_per_row, double std_nnz_per_row, char * distribution, unsigned int seed, char * placement, double bw_scaled, double skew);
            int free_csr_matrix(struct csr_matrix *csr);
            """
        )

        so_file = "/various/pmpakos/SpMV-Research/artificial-matrix-generator/artificial_matrix_generation_double.so"
        # so_file = "/various/pmpakos/SpMV-Research/artificial-matrix-generator/artificial_matrix_generation_float.so"
        lib = ffi.dlopen(so_file)
        csr_matrix_struct = lib.artificial_matrix_generation(nr_rows, nr_rows, avg_nnz_per_row, std_nnz_per_row, str.encode(distribution), seed, str.encode(placement), avg_bw_target, skew_coeff)

        nr_rows = csr_matrix_struct.nr_rows
        nr_cols = csr_matrix_struct.nr_cols
        nr_nnz = csr_matrix_struct.nr_nzeros
        if(nr_nnz==0):
            ret_val = lib.free_csr_matrix(csr_matrix_struct)
            if(ret_val != 0):
                print("PROBLEM WHEN DE-ALLOCATING C MEMORY. CAREFUL")
            # need to create a dummy sparse array, in order to be able to free it without any warning later! ....
            mat = sp.csr_matrix(([1.0], [0], [0,1]), shape=(1, 1)).tocoo()
            self.row = mat.row.astype(np.uint32)
            self.col = mat.col.astype(np.uint32)
            self.data = mat.data.astype(np.float64)
            del mat
            return "<EMPTY>"

        density = csr_matrix_struct.density
        mem_footprint = csr_matrix_struct.mem_footprint
        mem_range=[]
        for i in csr_matrix_struct.mem_range:
            mem_range.append(i.decode())
            if(i==b"\x00"):
                break
        mem_range = "".join(i for i in mem_range)

        avg_nnz_per_row = csr_matrix_struct.avg_nnz_per_row
        std_nnz_per_row = csr_matrix_struct.std_nnz_per_row
        
        avg_bw = csr_matrix_struct.avg_bw
        std_bw = csr_matrix_struct.std_bw

        avg_sc = csr_matrix_struct.avg_sc
        std_sc = csr_matrix_struct.std_sc

        bandwidth_scaled = csr_matrix_struct.bandwidth_scaled

        # https://python.hotexamples.com/site/file?hash=0x63c231f2401ca488a0fb143b43c83155fddc0a7bd1fe70552c09ef274b6968a3&fullName=eg-master/ffi/cffi/main.py&project=ben-albrecht/eg
        # row_ptr = np.asarray([csr_matrix_struct.row_ptr[i] for i in range(nr_rows+1)])
        # col_ind = np.asarray([csr_matrix_struct.col_ind[i] for i in range(nr_nnz)])
        # values = np.asarray([csr_matrix_struct.values[i] for i in range(nr_nnz)])
        row_ptr = np.frombuffer(ffi.buffer(csr_matrix_struct.row_ptr, (nr_rows+1)*4), dtype = np.int32)
        col_ind = np.frombuffer(ffi.buffer(csr_matrix_struct.col_ind, nr_nnz*4), dtype = np.int32)
        values = np.frombuffer(ffi.buffer(csr_matrix_struct.values, nr_nnz*8), dtype = np.float64)

        # print("\tnr_rows", nr_rows)
        # print("\tnr_cols", nr_cols)
        # print("\tnr_nnz", nr_nnz)
        # print("\tdensity", density)
        # print("\tmem_footprint", mem_footprint)
        # print("\tmem_range", mem_range)
        # print("\tavg_nnz_per_row", avg_nnz_per_row)
        # print("\tstd_nnz_per_row", std_nnz_per_row)
        # print("\tavg_bw", avg_bw)
        # print("\tstd_bw", std_bw)
        # print("\tavg_sc", avg_sc)
        # print("\tstd_sc", std_sc)
        # print("\tseed", seed)
        # print("\tdistribution", distribution)
        # print("\tplacement", placement)
        # print("\tbandwidth_scaled", bandwidth_scaled)
        # this replaces the previous >>>> that was used in previous version of generator
        mtxName =  "synthetic_"+\
                   str(nr_rows)+"_"+str(nr_cols)+"_"+str(nr_nnz)+\
                   "_avg"+str(round(avg_nnz_per_row,3))+"_std"+str(round(std_nnz_per_row,3))+\
                   "_"+placement + "_bw" + str(round(avg_bw,3))+\
                   "_skew"+str(round(skew_coeff,3))+\
                   "_"+distribution[0]+str(seed)+\
                   ".mtx"
        print(">>>>", mtxName, mem_range, avg_nnz_per_row, std_nnz_per_row, avg_bw, std_bw, avg_sc, std_sc)

        #####################################################################################################
        # OLD OLD OLD OLD OLD OLD 
        # # row_ptr, col_ind, nr_nnz, density, mem_range, new_avg_nnz_per_row, new_std_nnz_per_row, avg_bw, std_bw, avg_sc, std_sc, time1, time2 = sparse_matrix_generator_wrapper(nr_rows, avg_nnz_per_row, std_nnz_per_row, distribution, placement, d_f, seed, precision, verbose)
        # row_ptr, col_ind, nr_nnz, density, mem_range, new_avg_nnz_per_row, new_std_nnz_per_row, avg_bw, std_bw, avg_sc, std_sc, time1, time2 = sparse_matrix_generator_wrapper_v2(nr_rows, avg_nnz_per_row, std_nnz_per_row, distribution, placement, avg_bw_target, skew_coeff, seed, precision, verbose)
        # nr_cols = nr_rows
        # sample_values = np.asarray([4.1792, 2.1257, 7.2853, 4.3276, 4.6612, 1.184, 2.9755, 7.0970, 8.7629, 4.4306, 9.5388, 3.70473, 7.6795, 7.3303])
        # # values = np.asarray([sample_values[i%len(sample_values)] for i in range(nr_nnz)])
        # # iterable = (sample_values[i%len(sample_values)] for i in range(nr_nnz))
        # # values = np.fromiter(iterable, np.float64)
        # values = np.tile(sample_values, int(np.ceil(nr_nnz / len(sample_values))) )[0:nr_nnz]
        #####################################################################################################
        mat = sp.csr_matrix((values, col_ind, row_ptr), shape=(nr_rows, nr_cols))
        mat = mat.tocoo()
        if sp.issparse(mat):
            mat.eliminate_zeros()
            self.row = mat.row.astype(np.uint32)
            self.col = mat.col.astype(np.uint32)
            self.data = mat.data.astype(np.float64)
            self.mtxName = mtxName
            self.m,self.n = mat.shape
            self.nnz = mat.nnz
            self.minRowId = np.amin(self.row)
            self.minColId = np.amin(self.col)

            ret_val = lib.free_csr_matrix(csr_matrix_struct)
            if(ret_val != 0):
                print("PROBLEM WHEN DE-ALLOCATING C MEMORY. CAREFUL")

            del mat
            del row_ptr
            del col_ind
            del values
            print_mem_usage()
            return mtxName
            # return True
        else:
            return mtxName
            # return False

    def create_matrix(self, p_row, p_col, p_data):
        self.row = np.asarray(p_row).astype(np.uint32)
        self.col = np.asarray(p_col).astype(np.uint32)
        self.data = np.asarray(p_data).astype(np.float64)
        self.nnz = self.row.shape[0]
        if self.nnz != 0:
            self.minRowId = np.amin(self.row)
            self.minColId = np.amin(self.col)
            self.m = np.amax(self.row)+1-self.minRowId
            self.n = np.amax(self.col)+1-self.minColId
        else:
            self.minRowId = 0 
            self.minColId = 0
            self.m,self.n=0,0

    def sort_coo(self, p_order):
        if p_order =='r':
            order = np.lexsort((self.col, self.row))
        elif p_order == 'c':
            order = np.lexsort((self.row, self.col))
        else:
            print("ERROR: order input must be \'r\' or \'c\'")
            return False
        self.row = self.row[order]
        self.col = self.col[order]
        self.data = self.data[order]
        return True

    def sort(self, p_order):
        l_res = self.sort_coo(p_order)
        return l_res

    def is_equal(self, p_spm):
        p_spm.sort('r')
        self.sort('r')
        l_equalRow = self.row == p_spm.row
        l_equalCol = self.col == p_spm.col
        l_equalData = self.data == p_spm.data
        l_equalParam = (self.m == p_spm.m) and (self.n==p_spm.n) and (self.nnz == p_spm.nnz)
        l_res = l_equalRow.all() and l_equalCol.all() and l_equalData.all() and l_equalParam
        return l_res

    def to_list(self):
        self.row = list(self.row)
        self.col = list(self.col)
        self.data = list(self.data)
