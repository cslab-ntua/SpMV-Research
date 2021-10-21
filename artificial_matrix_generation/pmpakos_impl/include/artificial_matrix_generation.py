import os
import numpy as np
import pandas as pd
import random
import time
import multiprocessing as mp
from multiprocessing import shared_memory
from multiprocessing import Process, Array

from contextlib import closing
import logging

import scipy
# info = mp.get_logger().info

prefix = "/various/pmpakos/SpMV-Research/artificial_matrix_generation/pmpakos_impl/generated_matrices/"

############################################################################################################################################################
def find_class(mem_footprint, low_mb_list, high_mb_list):
    for i in range(len(low_mb_list)):
        if(mem_footprint>=low_mb_list[i] and mem_footprint<=high_mb_list[i]):
            return i
    return -1

# The shared array pointer is a global variable so that it can be accessed by the child processes. It consists of 3 tuples of (pointer, dtype, shape).
def _init(shared_col_ind_, shared_bw_, shared_sc_):
    global shared_col_ind
    shared_col_ind = shared_col_ind_
    global shared_bw
    shared_bw = shared_bw_
    global shared_sc
    shared_sc = shared_sc_

# Get a NumPy array from a shared memory buffer, with a given dtype and shape. No copy is involved, the array reflects the underlying shared buffer.
def shared_to_numpy(shared_arr, dtype, shape):
    return np.frombuffer(shared_arr, dtype=dtype).reshape(shape)

def create_shared_array(dtype, shape):
    # Create a new shared array. Return the shared array pointer, and a NumPy array view to it. 
    # Note that the buffer values are not initialized.
    dtype = np.dtype(dtype)
    cdtype = np.ctypeslib.as_ctypes_type(dtype)       # Get a ctype type from the NumPy dtype.
    shared_arr = mp.RawArray(cdtype, sum(shape))      # Create the RawArray instance.
    arr = shared_to_numpy(shared_arr, dtype, shape)   # Get a NumPy array view.
    return shared_arr, arr

def parallel_function(workload_augmented):
    # a=time.time()
    index_range, nnz_range, partial_snd, nr_cols, placement, d_f, seed = workload_augmented

    np.random.seed(seed)

    i0, i1 = index_range
    random.seed(seed+i0)
    l0, l1 = nnz_range

    offset = i0
    partial_col_ind_list  = []
    partial_bw = np.zeros((i1-i0,))
    partial_sc = np.zeros((i1-i0,))
    if(placement=="random"):
        for row in range(i0, i1):
            random.seed(seed+row)
            row_nonzeros = partial_snd[row-offset]
            if(row_nonzeros>0):
                local_col_ind = np.sort(random.sample(range(1,nr_cols+1), row_nonzeros))
                partial_col_ind_list.append(local_col_ind)
                partial_bw[row-offset] = (local_col_ind[-1]-local_col_ind[0]+1)/nr_cols
                partial_sc[row-offset] = row_nonzeros/partial_bw[row-offset]
    elif(placement=="diagonal"):
        # d_f : diagonal factor must be smaller than 1, so that sampling can give unique values within range
        # (range examined greater than population asked by random.sample()))
        # place them around the main diagonal
        for row in range(i0, i1):
            random.seed(seed+row)
            row_nonzeros = partial_snd[row-offset]
            if(row_nonzeros>0):
                if(int(row-row_nonzeros/d_f)<1 and int(row+row_nonzeros/d_f)>nr_cols):
                    local_col_ind = np.sort(random.sample(range( 1, nr_cols+1), row_nonzeros))
                elif(int(row-row_nonzeros/d_f)<1):
                    local_col_ind = np.sort(random.sample(range( 1, int(row+row_nonzeros/d_f)+1), row_nonzeros))
                elif(int(row+row_nonzeros/d_f)>nr_cols):
                    local_col_ind = np.sort(random.sample(range( int(row-row_nonzeros/d_f), nr_cols+1), row_nonzeros))
                else:
                    local_col_ind = np.sort(random.sample(range( int(row-row_nonzeros/d_f), int(row+row_nonzeros/d_f)+1), row_nonzeros))
                partial_col_ind_list.append(local_col_ind)
                partial_bw[row-offset] = (local_col_ind[-1]-local_col_ind[0]+1)/nr_cols
                partial_sc[row-offset] = row_nonzeros/partial_bw[row-offset]

    # b = time.time()
    partial_col_ind = np.asarray([item-1 for sublist in partial_col_ind_list for item in sublist])
    # c = time.time()

    col_ind = shared_to_numpy(*shared_col_ind)
    bandwidth = shared_to_numpy(*shared_bw)
    scatter = shared_to_numpy(*shared_sc)

    col_ind[l0:l1] = partial_col_ind
    bandwidth[i0:i1] = partial_bw
    scatter[i0:i1] = partial_sc
    # e = time.time()
    # print(os.getpid(),"\tmain loop", round(b-a,5), "\tcol_ind merging", round(c-b,5), "\tshared_arr", round(e-c,5), "\t-> total:", round(e-a,5))


def generate_random_matrix(nr_rows, nr_cols, avg_nnz_per_row, std_nnz_per_row, \
                           distribution, seed, placement, d_f, \
                           save_it, keep_it, low_mb, high_mb, precision):
    ###############################################################################################    
    start = time.time()
    np.random.seed(seed)
    random.seed(seed)

    if(distribution == "normal"):
        snd = np.random.normal(loc=avg_nnz_per_row,
                               scale=std_nnz_per_row,
                               size=nr_rows)
        distrib = "n"
    elif(distribution == "gamma"):
        snd = np.random.gamma(shape=(avg_nnz_per_row**2/std_nnz_per_row**2), 
                              scale=(std_nnz_per_row**2/avg_nnz_per_row),
                              size=nr_rows)
        distrib = "g"

    integerization = np.vectorize(lambda x : int(x) if x>0 else int(-x))
    snd = integerization(snd)

    # range of nonzeros per row
    min_snd = np.min(snd)
    max_snd = np.max(snd)
    nr_nnz = np.sum(snd)
    new_avg_nnz_per_row = np.mean(snd)
    new_std_nnz_per_row = np.std(snd)
    density = np.round(nr_nnz/(nr_rows*nr_cols)*100,4)
    
    mem_footprint = round(((precision+32)*nr_nnz + 32*(nr_rows+1))/(8*1024*1024),3) # MB
    
    # time1 : time needed to determine how many nonzeros per row exist
    time1 = round(time.time()-start,4)
    
    if(mem_footprint<low_mb or mem_footprint>high_mb):
        return (" ",
            [], [],
            nr_nnz, density, mem_footprint,
            0, 0,
            0, 0,
            0, 0,
            time1, 0)
    ###############################################################################################
    start = time.time()
    
    placement_full = placement
    if(placement=="diagonal"): # need to add diagonal_factor too
        if(d_f>1):
            print("Diagonal Factor is greater than 1. I have to stop now!")
            return
        placement_full += "_"+str(d_f)

    filename =  prefix+\
                "synthetic_"+\
                str(nr_rows)+"_"+str(nr_cols)+"_"+str(nr_nnz)+\
                "_avg"+str(avg_nnz_per_row)+"_std"+str(std_nnz_per_row)+\
                "_"+placement_full+\
                "_"+distrib+str(seed)+\
                ".mtx"
    # print_matrix_distribution_info(filename, nr_row, nr_nnz, snd, max_snd, min_snd, avg_nnz_per_row, std_nnz_per_row)
    if(save_it == True):
        f = open(filename, "w")
        # f.write("%%MatrixMarket matrix coordinate real general\n") # if this is used, need to pass a value too!
        f.write("%%MatrixMarket matrix coordinate pattern general\n")
        f.write(str(nr_rows)+" "+str(nr_cols)+" "+str(nr_nnz)+"\n")

    row_ptr = np.zeros((nr_rows+1), dtype=np.uint32)

    # no need to speed up this one, very small percentage of execution time
    for row in range(len(snd)):
        row_nonzeros = snd[row]
        if(keep_it == True):
            row_ptr[row+1] = row_nonzeros+row_ptr[row]

    ################################################################################################################################################################
    # start parallel (multiprocess) creation of col_ind
    # thank you https://stackoverflow.com/questions/7894791/use-numpy-array-in-shared-memory-for-multiprocessing
    mp.freeze_support() # https://stackoverflow.com/questions/24374288/where-to-put-freeze-support-in-a-python-script
    # n_processes_list = [40, 20, 10,  8, 4, 2, 1]
    n_processes = os.cpu_count()
    # Initialize 3 shared arrays. col_ind (the giant) and bandwidth and scattering (the goliaths) that will be shared among processes.
    # Bandwidth and Scattering were initially calculated in serial manner, but it was pretty time consuming for large matrices (20 sec for 5M rows)
    # Therefore, sharing is caring.
    dtype = np.int32
    shape = (int(nr_nnz),)
    shared_col_ind, col_ind = create_shared_array(dtype, shape)
    col_ind.flat[:] = np.zeros(shape)

    dtype2 = np.float64
    shape2 = (nr_rows,)
    shared_bw, bandwidth = create_shared_array(dtype2, shape2)
    bandwidth.flat[:] = np.zeros(shape2)

    shared_sc, scatter = create_shared_array(dtype2, shape2)
    scatter.flat[:] = np.zeros(shape2)

    # Create a Pool of processes and expose the shared array to the processes, in a global variable (_init() function).
    with closing(mp.Pool(n_processes, initializer=_init, initargs=((shared_col_ind, dtype, shape),(shared_bw, dtype2, shape2),(shared_sc, dtype2, shape2)))) as p:
        n = nr_rows // n_processes
        index_range = [(k*n, (k+1)*n) for k in range(n_processes)]
        # verify that last process will process all final rows (if n_processes does not divide nr_rows fully)
        s,_ = index_range[-1]
        index_range[-1] = (s,nr_rows)
        workload_augmented = []
        for i in range(len(index_range)):
            i0, i1 = index_range[i]
            partial_snd = snd[i0:i1]
            nnz_range = row_ptr[i0], row_ptr[i1]
            workload_augmented.append([index_range[i], nnz_range, partial_snd, nr_cols, placement, d_f, seed])
        p.map(parallel_function, workload_augmented)
    # Close the processes.
    p.join()
    ################################################################################################################################################################

    if(save_it == True):
        for row in range(len(snd)):
            local_col_ind = col_ind[row_ptr[row]:row_ptr[row+1]]
            for i in local_col_ind:
                f.write(str(row+1)+" "+str(i+1)+"\n") # no need to write a value, only keep indices
        f.close()

    # fuck numpy arrays, no need to mess with their API
    row_ptr = row_ptr.tolist()
    col_ind = col_ind.tolist()
    
    avg_bw = np.mean(bandwidth)
    std_bw = np.std(bandwidth)
    avg_sc = np.mean(scatter)
    std_sc = np.std(scatter)
    
    # time2 : time needed to determine where nonzeros are placed within each row
    time2 = round(time.time()-start,4)
    ###############################################################################################
    return (filename.split("/")[-1],
            row_ptr, col_ind,
            nr_nnz, density, mem_footprint,
            new_avg_nnz_per_row, new_std_nnz_per_row,
            avg_bw, std_bw,
            avg_sc, std_sc,
            time1, time2)

def sparse_matrix_generator_wrapper(nr_rows, avg_nnz_per_row, std_nnz_per_row, distribution, placement, d_f, seed, precision, verbose):
    low_mb_list = [4,8,16,32,64,128,256,512,1024,2048]
    high_mb_list =  [8,16,32,64,128,256,512,1024,2048,4096]
    low_mb, high_mb = low_mb_list[0], high_mb_list[-1]
    save_it = False
    keep_it = True
    nr_cols = nr_rows
    
    filename, row_ptr, col_ind, nr_nnz, density, mem_footprint, new_avg_nnz_per_row, new_std_nnz_per_row, avg_bw, std_bw, avg_sc, std_sc, time1, time2 = generate_random_matrix(nr_rows, nr_cols, avg_nnz_per_row, std_nnz_per_row, distribution, seed, placement, d_f, save_it, keep_it, low_mb, high_mb, precision)
    pos = find_class(mem_footprint, low_mb_list, high_mb_list)
    if(pos!=-1):
        mem_range = '['+str(low_mb_list[pos])+'-'+str(high_mb_list[pos])+']'
        if(verbose):
            print(filename, "\t", nr_rows, nr_cols, nr_nnz, '('+str(density)+'%)\t', mem_footprint, 'MB ('+str(precision)+'-bit precision)', 'in', round(time1+time2,3), 'seconds', '\t', mem_range)
            print("row_ptr[0:10] :", row_ptr[0:10])
            print("col_ind[0:10] :", col_ind[0:10])
        return row_ptr, col_ind, nr_nnz, density, mem_range, new_avg_nnz_per_row, new_std_nnz_per_row, avg_bw, std_bw, avg_sc, std_sc, time1, time2
    else:
        return [],           [],      0,       0,        '',                   0,                   0,       0,     0,      0,       0,    0,     0

##########################################################################################################################################################################################################################################################
##########################################################################################################################################################################################################################################################
##########################################################################################################################################################################################################################################################
##########################################################################################################################################################################################################################################################
def matrix_size_estimator(nr_rows, nr_cols, avg_nnz_per_row, std_nnz_per_row, \
                                    distribution, seed, placement, d_f, \
                                    low_mb, high_mb, precision):
    ###############################################################################################    
    np.random.seed(seed)
    random.seed(seed)

    if(distribution == "normal"):
        snd = np.random.normal(loc=avg_nnz_per_row,
                               scale=std_nnz_per_row,
                               size=nr_rows)
        distrib = "n"
    elif(distribution == "gamma"):
        snd = np.random.gamma(shape=(avg_nnz_per_row**2/std_nnz_per_row**2), 
                              scale=(std_nnz_per_row**2/avg_nnz_per_row),
                              size=nr_rows)
        distrib = "g"

    integerization = np.vectorize(lambda x : int(x) if x>0 else int(-x))
    snd = integerization(snd)
    # range of nonzeros per row
    nr_nnz = np.sum(snd)
    density = np.round(nr_nnz/(nr_rows*nr_cols)*100,4)
    mem_footprint = round(((precision+32)*nr_nnz + 32*(nr_rows+1))/(8*1024*1024),3) # MB
    
    filename =  "synthetic_"+\
                str(nr_rows)+"_"+str(nr_cols)+"_"+str(nr_nnz)+\
                "_avg"+str(avg_nnz_per_row)+"_std"+str(std_nnz_per_row)+\
                "_"+placement+\
                "_"+distrib+str(seed)+\
                ".mtx"

    ###############################################################################################
    return (filename.split("/")[-1], nr_nnz, density, mem_footprint)

def matrix_size_estimator_wrapper(precision_type, distribution):
    # nr_rows_list = [4000, 21000, 38000, 47000, 64000, 68000, 83000, 121000, 141000, 161000, 171000, 186000, 207000, 218000, 268000, 526000, 863000, 952000, 987000, 1000000, 1383000, 1635000, 4690000, 5155000, 5558000]
    # avg_nnz_per_row_list = [3.1,4,4.3,5.6,6,10,12,20,22,39,45,50,55,65,70,80,105,120,155,220,420,2600]
    # std_nnz_per_row_list = [0.02,0.09,0.2,0.25,0.3,0.39,0.43,0.55,0.64,0.72,0.78,0.84,0.98,1.14,1.32,1.5,1.6,2.25,2.5,3,8,13,125]

    nr_rows_list = [4000, 21000, 38000, 47000, 64000, 68000, 83000, 121000, 141000, 161000, 171000, 186000, 207000, 218000, 268000, 526000, 863000, 952000, 987000, 1000000, 1383000, 1635000, 4690000, 5155000, 5558000]
    avg_nnz_per_row_list = [3.1,4,6,10,20,45,70,105,220,420,2600]
    std_nnz_per_row_list = [0.02,0.2,0.64,0.84,1.6,2.25,2.5,3,8,13,125]

    # low_mb_list = [4,8,16,32,64,128,256,512,1024,2048]
    # high_mb_list =  [8,16,32,64,128,256,512,1024,2048,4096]
    low_mb_list = [4,8,16,32,64,128,256,512,1024] # removed largest mem range (2048-4096)
    high_mb_list =  [8,16,32,64,128,256,512,1024,2048] 

    low_mb, high_mb = low_mb_list[0], high_mb_list[-1]
    cnt = 0

    if(precision_type=="double"):
        precision = 64
    else:
        precision = 32

    placement = "random"
    d_f = 1

    files_list, logs = [], []
    for i in range(len(low_mb_list)):
        log_filename = "../matrix_generation_parameters/"+precision_type+"/logs/matrix_generator_"+distribution+"_"+str(low_mb_list[i])+"-"+str(high_mb_list[i])+"_log.txt"
        logs.append(log_filename)
        f = open(log_filename,"w")
        files_list.append(f)
        files_list[i].write("\t".join(["filename","nr_rows","nr_cols","nr_nnz","density","mem_footprint"])+"\n")

    for nr_rows in nr_rows_list:
        print("\n\n--------------------------\n\nCURRENTLY TESTING nr_rows [",nr_rows,"]\n")
        start = time.time()
        nr_cols = nr_rows
        for avg_nnz_per_row in avg_nnz_per_row_list:
            for std_nnz_per_row in std_nnz_per_row_list:
                for seed in [14]: #[14,80,96]
                    cnt+=4
                    filename, nr_nnz, density, mem_footprint = matrix_size_estimator(nr_rows, nr_cols, avg_nnz_per_row, std_nnz_per_row, distribution, seed, placement, d_f, low_mb, high_mb, precision)
                    pos = find_class(mem_footprint, low_mb_list, high_mb_list)
                    if(pos!=-1):
                        print("matrix"+str(cnt),"\t", nr_rows, nr_cols, nr_nnz, "(",density,"%)\t\t",mem_footprint,"MB", "\t [",low_mb_list[pos],"-",high_mb_list[pos],"]")
                        str_list = [filename.split("/")[-1], nr_rows, nr_cols, nr_nnz, density, mem_footprint]
                        str_list = "\t".join([str(x) for x in str_list])+"\n"
                        files_list[pos].write(str_list)
        print("\nTOOK ME", round(time.time()-start,4), " SECONDS TO FINISH nr_rows [",nr_rows,"]")

    for i in range(len(low_mb_list)):
        files_list[i].close()

    root_path = "../matrix_generation_parameters/"+precision_type+"/"
    cnt = 0
    for log in logs:
        log_filename = log.split("/")[-1].split("_")
        distribution = log_filename[2]
        mem_footprint_range = log_filename[3]

        mgp_filename = root_path + distribution + "_" + mem_footprint_range + ".txt"
        mgp_small_filename = root_path + "/small/" + distribution + "_" + mem_footprint_range + ".txt"
        print(mgp_filename)
        f = open(mgp_filename,"w")
        f2 = open(mgp_small_filename,"w")
        
        dataframe = pd.read_csv(log, delimiter ="\t")
        dataframe = dataframe.sort_values("mem_footprint").reset_index() # sort by mem size, so that sampling the dataset is useful. reset_index() done to perform sampling too
        dataframe_size = dataframe.shape[0]
        small_pcg = 10
        for index, row in dataframe.iterrows():
            filename = row["filename"].split("_")
            nr_rows = filename[1]
            avg_nnz_per_row = filename[4].strip("avg")
            std_nnz_per_row = filename[5].strip("std")
            seed = filename[7].strip(".mtx").strip("n").strip("g")

            placement_list = ["random","diagonal" ]# and diagonal

            for placement in placement_list:
                df_list = [1]
                if(placement=="diagonal"):
                    df_list = [0.5, 0.05, 0.005]
                for d_f in df_list:
                    # matrix parameters (that are fed to C function) follow the given format :
                    # nr_rows   avg_nnz_per_row   std_nnz_per_row   distribution   placement    diagonal_factor   seed
                    line = str(nr_rows)+" "+str(avg_nnz_per_row)+" "+str(std_nnz_per_row)+" "+distribution+" "+placement+" "+str(d_f)+" "+str(seed)
                    f.write(line+"\n")
                    if(index % small_pcg ==0):
                        f2.write(line+"\n")
                    cnt+=1
        f.close()
        f2.close()
    print("After all,",cnt,"matrices will be tested!")

##########################################################################################################################################################################################################################################################
##########################################################################################################################################################################################################################################################
##########################################################################################################################################################################################################################################################
##########################################################################################################################################################################################################################################################

if __name__ == '__main__':
    print("main")

    # matrix_size_estimator_wrapper("double", "normal")
    # matrix_size_estimator_wrapper("double", "gamma")

    # matrix_size_estimator_wrapper("float", "normal")
    # matrix_size_estimator_wrapper("float", "gamma")

    # sparse_matrix_generator_wrapper(161000,4,0.64,"normal","diagonal",0.005,14,64,1)
    # sparse_matrix_generator_wrapper(161000,10,0.02,"normal","random",1,14,64,1)
    # sparse_matrix_generator_wrapper(207000,4,0.84,"normal","diagonal",0.05,14,64,1)
    # sparse_matrix_generator_wrapper(141000,20,0.64,"normal","random",1,14,64,1)
    # sparse_matrix_generator_wrapper(38000,220,0.84,"normal","diagonal",0.005,14,64,1)
    # sparse_matrix_generator_wrapper(526000,70,2.5,"normal","diagonal",0.5,14,64,1)
    # sparse_matrix_generator_wrapper(4690000,4,1.6,"normal","random",1,14,64,1)
    # sparse_matrix_generator_wrapper(1635000,45,0.2,"normal","diagonal",0.05,14,64,1)
    # sparse_matrix_generator_wrapper(5558000,20,0.64,"normal","diagonal",0.005,14,64,1)
