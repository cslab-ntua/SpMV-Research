import os
import numpy as np
import random
import time
import multiprocessing as mp
from multiprocessing import shared_memory
from multiprocessing import Process, Array

from contextlib import closing
import logging

import scipy
# info = mp.get_logger().info

prefix = "./tmp/"

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
    a=time.time()
    index_range, nnz_range, partial_snd, nr_cols, placement, d_f, seed = workload_augmented

    np.random.seed(seed)
    random.seed(seed)   

    i0, i1 = index_range
    l0, l1 = nnz_range

    offset = i0
    partial_col_ind_list  = []

    partial_bw = np.zeros((i1-i0,))
    partial_sc = np.zeros((i1-i0,))
    if(placement=="random"):
        for row in range(i0, i1):
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

    b = time.time()
    partial_col_ind = np.asarray([item-1 for sublist in partial_col_ind_list for item in sublist])
    c = time.time()

    col_ind = shared_to_numpy(*shared_col_ind)
    bandwidth = shared_to_numpy(*shared_bw)
    scatter = shared_to_numpy(*shared_sc)

    col_ind[l0:l1] = partial_col_ind
    bandwidth[i0:i1] = partial_bw
    scatter[i0:i1] = partial_sc
    e = time.time()
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
        f = open(filename , "w")
        # f.write("%%MatrixMarket matrix coordinate real general\n") # if this is used, need to pass a value too!
        f.write("%%MatrixMarket matrix coordinate pattern general\n")
        f.write(str(nr_rows)+" "+str(nr_cols)+" "+str(nr_nnz)+"\n")

    row_ptr = np.zeros((nr_rows+1), dtype=np.uint32)

    # no need to speed up this one, very small percentage of execution time
    for row in range(len(snd)):
        row_nonzeros = snd[row]
        if(keep_it == True):
            row_ptr[row+1] = row_nonzeros+row_ptr[row]

    # start parallel (multiprocess) creation of col_ind
    # thank you https://stackoverflow.com/questions/7894791/use-numpy-array-in-shared-memory-for-multiprocessing
    mp.freeze_support() # https://stackoverflow.com/questions/24374288/where-to-put-freeze-support-in-a-python-script
    # n_processes_list = [40, 20, 10,  8, 4, 2, 1]
    n_processes = 40
    # n_processes = os.cpu_count()
    testing_time=time.time()
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
    save_it = True
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

if __name__ == '__main__':
    sparse_matrix_generator_wrapper(64000, 6.0, 0.72, "gamma", "diagonal", 0.005, 14, 64, 1)
    sparse_matrix_generator_wrapper(64000, 6, 0.72, "gamma", "diagonal", 0.005, 14, 64, 1)
    sparse_matrix_generator_wrapper(64000, 6, 0.72, "normal", "random", 1, 14, 64, 1)
    sparse_matrix_generator_wrapper(83000, 10, 0.98, "normal", "random", 1, 14, 64, 1)
    sparse_matrix_generator_wrapper(83000, 10.0, 0.98, "gamma", "diagonal", 0.005, 14, 64, 1)
    sparse_matrix_generator_wrapper(83000, 10, 0.98, "gamma", "diagonal", 0.005, 14, 64, 1)
    sparse_matrix_generator_wrapper(83000, 22, 2.5, "normal", "random", 1, 14, 64, 1)
    sparse_matrix_generator_wrapper(83000, 22, 2.5, "gamma", "diagonal", 0.005, 14, 64, 1)
    sparse_matrix_generator_wrapper(83000, 55, 3, "gamma", "diagonal", 0.005, 14, 64, 1)
    sparse_matrix_generator_wrapper(83000, 55, 3, "normal", "random", 1, 14, 64, 1)
    sparse_matrix_generator_wrapper(121000, 70, 0.25, "normal", "random", 1, 14, 64, 1)
    sparse_matrix_generator_wrapper(121000, 70, 0.25, "gamma", "diagonal", 0.005, 14, 64, 1)
    sparse_matrix_generator_wrapper(186000, 105, 0.98, "gamma", "diagonal", 0.005, 14, 64, 1)
    sparse_matrix_generator_wrapper(186000, 105, 0.98, "normal", "random", 1, 14, 64, 1)
    sparse_matrix_generator_wrapper(186000, 155, 8, "normal", "random", 1, 14, 64, 1)
    sparse_matrix_generator_wrapper(186000, 155, 8, "gamma", "diagonal", 0.005, 14, 64, 1)
    sparse_matrix_generator_wrapper(952000, 80, 0.98, "normal", "random", 1, 14, 64, 1)
    sparse_matrix_generator_wrapper(952000, 80, 0.98, "gamma", "diagonal", 0.005, 14, 64, 1)
    sparse_matrix_generator_wrapper(952000, 155, 2.25, "normal", "random", 1, 14, 64, 1)
    sparse_matrix_generator_wrapper(952000, 155, 2.25, "gamma", "diagonal", 0.005, 14, 64, 1)
    sparse_matrix_generator_wrapper(4690000, 39, 0.02, "normal", "random", 1, 14, 64, 1)
    sparse_matrix_generator_wrapper(4690000, 55, 125, "gamma", "diagonal", 0.005, 14, 64, 1)

