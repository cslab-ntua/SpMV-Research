# %config Completer.use_jedi = False
import os
import numpy as np
import pandas as pd
import random
import time
import multiprocessing as mp

import matplotlib.pyplot as plt
import itertools 

# from multiprocessing import shared_memory
# from multiprocessing import Process, Array

from contextlib import closing
import logging

import scipy
# info = mp.get_logger().info

prefix = "/various/pmpakos/SpMV-Research/artificial_matrix_generation/pmpakos_impl/generated_matrices/"
prefix = "/mnt/various/SpMV-Research/artificial_matrix_generation/pmpakos_impl/generated_matrices/"
prefix = "/home/pmpakos/Downloads/test_em/"

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
    index_range, nnz_range, partial_snd, nr_cols, placement, d_f, seed, avg_bw_target = workload_augmented
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
                if(row_nonzeros < avg_bw_target): 
                    if(int(row-avg_bw_target/2)<1 and int(row+avg_bw_target/2)>nr_cols):
                        iid=1
                        bound_start, bound_finish = 1, nr_cols
                    elif(int(row-avg_bw_target/2)<1):
                        iid=2
                        bound_start, bound_finish = 1, int(row+avg_bw_target/2)
                    elif(int(row+avg_bw_target/2)>nr_cols):
                        iid=3
                        bound_start, bound_finish = int(row-avg_bw_target/2), nr_cols
                    else:
                        iid=4
                        bound_start, bound_finish = int(row-avg_bw_target/2), int(row+avg_bw_target/2)
                            
                else: 
                    if(int(row-row_nonzeros/d_f)<1 and int(row+row_nonzeros/d_f)>nr_cols):
                        iid=5
                        bound_start, bound_finish = 1, nr_cols
                    elif(int(row-row_nonzeros/d_f)<1):
                        iid=6
                        bound_start, bound_finish = 1, int(row+row_nonzeros/d_f)
                    elif(int(row+row_nonzeros/d_f)>nr_cols):
                        iid=7
                        bound_start, bound_finish = int(row-row_nonzeros/d_f), nr_cols
                    else:
                        iid=8
                        bound_start, bound_finish = int(row-row_nonzeros/d_f), int(row+row_nonzeros/d_f)
                if(iid==5 or iid==6 or iid==7 or iid==8):
                    print("ROW", row,"(iid=",iid,") nonzeros = ", row_nonzeros, "(",avg_bw_target,")bound_start", bound_start, "bound_finish", bound_finish)

                integerization = np.vectorize(lambda x : int(np.floor(x+0.5)) if x>0 else 0)
                local_col_ind = integerization(np.linspace(bound_start, bound_finish, row_nonzeros))
                if(local_col_ind[-1]>nr_cols):
                    print("shiiiiiiiiiit", local_col_ind[-1], "at row", row, "bounds were", bound_start, " - ", bound_finish, "(iid =",iid,")")
                # local_col_ind = np.sort(random.sample(range( bound_start, bound_finish+1), row_nonzeros))

                partial_col_ind_list.append(local_col_ind)
                partial_bw[row-offset] = (local_col_ind[-1]-local_col_ind[0]+1)/nr_cols
                
                test = "AAAAAAAAAA" if(np.floor(partial_bw[row-offset]*nr_cols)>avg_bw_target) else ""
                if(row<0):
                    print("row",row,":\tnz",row_nonzeros,
                        "\tS", local_col_ind[0], "E", local_col_ind[-1], 
                        "\tbw", np.floor(partial_bw[row-offset]*nr_cols), "\tavg_bw_target ", avg_bw_target,
                        test)
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
    print(os.getpid(),"\tmain loop", round(b-a,5), "\tcol_ind merging", round(c-b,5), "\tshared_arr", round(e-c,5), "\t-> total:", round(e-a,5))

# Random matrix consists of two parts
# 1) Normal distribution with (avg_n, std_n)
# 2) Exponential function with MAX*exp(-C/nr_rows) distribution function
# INPUT  : nr_rows, targets (avg_nnz_per_row, std_nnz_per_row), skew coefficient
# OUTPUT : MAX, C of exponential, avg_n, std_n of normal    
def calculate_distribution_parameters(avg_target, std_target, skew_coeff, nr_rows):
    # a) calculate max_exp, which derives simply from "skew_coeff" formula
    MAX_exp = int(round(avg_target * (1+skew_coeff),2))
    
    # b) calculate C_exp
    # avg_exp (from dgal calculations) can be approximated as avg_exp = MAX_exp/C_exp
    # var_exp (from dgal calculations) can be approximated as var_exp = std_exp^2 = avg_exp^2 *(C_exp/2-1)
    
    # avg_target   = avg_n + avg_exp                                                = avg_n   + MAX_exp/C_exp
    # std_target^2 = std_n^2 + std_exp^2 = std_n^2 + MAX_exp^2/C_exp^2*(C_exp/2-1)  = std_n^2 + MAX_exp^2/(2*C_exp)
    # (-1 in C_exp/2-1 vanishes, as C_exp>>1)
    
    # need to determine C -> avg_n > 0 and std_n > 0 => 
    # avg_n > 0  =>   avg_target - MAX_exp/C_exp > 0             =>            (1) C_exp > MAX_exp / avg_target
    # std_n > 0  =>   std_target^2 - avg_exp^2 *(C_exp/2-1) > 0  =>            (2) C_exp > MAX_exp^2/(2*std_target^2)
    # between (1) and (2), pick max C_exp
    C_exp1 = MAX_exp / avg_target
    C_exp2 = MAX_exp**2 / (2 * std_target**2)
    C_exp = max(C_exp1, C_exp2)
    # print("C_exp1 =", C_exp1, "\tC_exp2 =", C_exp2)

    avg_exp = MAX_exp / C_exp
    std_exp = np.sqrt(avg_exp**2 * (C_exp/2-1))
    
    # c) calculate avg_n
    avg_n = avg_target - MAX_exp/C_exp
    
    # d) calculate std_n
    # std_n will either be calculated through above equation at b) or selected to be avg_n / 3
    # dgal stated that below "-3*std_n" only 0.1% of nonzero values exist. Therefore, std_n equal to avg_n/3 is enough
    # keep min between these 2
    std_n1 = (std_target**2 - std_exp**2)**0.5
    std_n2 = avg_n / 3
    std_n = min(std_n1, std_n2)
    # print("std_n1 =", std_n1, "\tstd_n2 =", std_n2)
    
    return MAX_exp, C_exp, avg_n, std_n

def generate_random_matrix(nr_rows, nr_cols, avg_nnz_per_row_target, std_nnz_per_row_target, \
                           distribution, seed, placement, d_f, avg_bw_target, skew_coeff, \
                           save_it, keep_it, low_mb, high_mb, precision, matrix_name):
    ###############################################################################################    
    if(std_nnz_per_row_target == 0):
        # if std_target==0, no need to add exponential, proceed with normal distribution only!
        MAX_exp, C_exp, avg_n, std_n = 0, 1, avg_nnz_per_row_target, std_nnz_per_row_target
    else:
        MAX_exp, C_exp, avg_n, std_n = calculate_distribution_parameters(avg_nnz_per_row_target, std_nnz_per_row_target, skew_coeff, nr_rows)
    ###############################################################################################    
    
    start = time.time()
    np.random.seed(seed)
    random.seed(seed)

    # 1) NORMAL DISTRIBUTION
    
    if(distribution == "normal"):
        snd_n = np.random.normal(loc=avg_n,
                                 scale=std_n,
                                 size=nr_rows)
        # https://stackoverflow.com/questions/21738383/python-difference-between-randn-and-normal
        # snd_n = np.random.randn(nr_rows)*std_n + avg_n
        distrib = "n"
    # (without much consideration, gamma distribution can be utilized too - need to verify it though!)
    elif(distribution == "gamma"):
        snd_n = np.random.gamma(shape=(avg_n**2/std_n**2), 
                                scale=(std_n**2/avg_n),
                                size=nr_rows)
        distrib = "g"
    # range of nonzeros per row
    min_snd_n = np.min(snd_n)
    max_snd_n = np.max(snd_n)
    
    # 2) EXPONENTIAL DISTRIBUTION
    # x = np.linspace(1, nr_rows, nr_rows)
    x = np.arange(0,nr_rows)
    snd_exp = MAX_exp*np.exp( -(C_exp/nr_rows) * x)
    halflife = int(np.floor(np.log(2)*(nr_rows/C_exp)))

    # combine normal + exponential distribution for final snd array
    snd = snd_n + snd_exp
    integerization = np.vectorize(lambda x : int(np.floor(x+0.5)) if x>0 else 0)
    snd = integerization(snd)

    nr_nnz = int(np.sum(snd))
    avg_nnz_per_row_final = np.mean(snd)
    std_nnz_per_row_final = np.std(snd)
    density = np.round(nr_nnz/(nr_rows*nr_cols)*100,4)
    
    mem_footprint = round(((precision+32)*nr_nnz + 32*(nr_rows+1))/(8*1024*1024),3) # MB
    
    print("--------------------------\nDISTRIBUTION PARAMETERS")
    print("NORMAL      -> ", "avg =", np.mean(snd_n), "\tstd =", np.std(snd_n))
    print("EXPONENTIAL -> ", "avg =", np.mean(snd_exp), "\tstd =", np.std(snd_exp))
    print("                (C_exp =", C_exp, "/ MAX_exp =", MAX_exp, "/ halflife =", halflife, ")")
    print("---\nFINAL       -> ", "avg =", avg_nnz_per_row_final, "\tstd =", std_nnz_per_row_final)
    print("target      -> ", "avg =", avg_nnz_per_row_target, "\t\tstd =", std_nnz_per_row_target)
    print("MATRIX",matrix_name,"difference with target : ",
          abs(round((avg_nnz_per_row_final-avg_nnz_per_row_target)/avg_nnz_per_row_target*100,3)),"% and ",
          abs(round((std_nnz_per_row_final-std_nnz_per_row_target)/std_nnz_per_row_target*100,3)),"%\n---")

    # time1 : time needed to determine how many nonzeros per row exist
    time1 = round(time.time()-start,4)
    
    # fig,ax = plt.subplots(1,1)
    # fig.set_size_inches(20,12)    
    # plt.plot(x, snd, label="snd")
    # plt.plot(x, snd_exp, color="r", label="snd_exp")
    # plt.scatter(x, snd_n, color="g", label="snd_n")
    # plt.xlabel('$x$')
    # plt.ylabel('$snd$')
    # plt.title("%s     %d  /  %f  /  %f  /  %f"%(matrix_name, nr_rows, avg_nnz_per_row_target, std_nnz_per_row_target, skew_coeff))
    # plt.legend()
    # plt.show()   
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
    
    # print_matrix_distribution_info(filename, nr_row, nr_nnz, snd, max_snd, min_snd, avg_nnz_per_row, std_nnz_per_row)
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
    # n_processes = os.cpu_count() #os.cpu_count()#//4
    n_processes = 4
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
            workload_augmented.append([index_range[i], nnz_range, partial_snd, nr_cols, placement, d_f, seed, avg_bw_target])
        p.map(parallel_function, workload_augmented)
    # Close the processes.
    p.join()
    ################################################################################################################################################################

    placement_full = placement
    if(placement=="diagonal"): # need to add diagonal_factor too
        if(d_f>1):
            print("Diagonal Factor is greater than 1. I have to stop now!")
            return
        placement_full += "_"+str(d_f)

    filename =  prefix+\
                "synthetic_"+\
                str(nr_rows)+"_"+str(nr_cols)+"_"+str(nr_nnz)+\
                "_avg"+str(avg_nnz_per_row_final)+"_std"+str(std_nnz_per_row_final)+\
                "_"+placement_full+\
                "_"+distrib+str(seed)+\
                ".mtx"

    if(save_it == True):
        f = open(filename, "w")
        # f.write("%%MatrixMarket matrix coordinate real general\n") # if this is used, need to pass a value too!
        f.write("%%MatrixMarket matrix coordinate pattern general\n")
        f.write(str(nr_rows)+" "+str(nr_cols)+" "+str(nr_nnz)+"\n")
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
            avg_nnz_per_row_final, std_nnz_per_row_final,
            avg_bw, std_bw,
            avg_sc, std_sc,
            time1, time2)

    return 1



def sparse_matrix_generator_wrapper(nr_rows, avg_nnz_per_row, std_nnz_per_row, distribution, placement, d_f, avg_bw_target, seed, precision, verbose):
    low_mb_list = [4,8,16,32,64,128,256,512,1024,2048]
    high_mb_list =  [8,16,32,64,128,256,512,1024,2048,4096]
    low_mb, high_mb = low_mb_list[0], high_mb_list[-1]
    save_it = True
    keep_it = True

    filename, row_ptr, col_ind, nr_nnz, density, mem_footprint, new_avg_nnz_per_row, new_std_nnz_per_row, avg_bw, std_bw, avg_sc, std_sc, time1, time2 = \
    generate_random_matrix(nr_rows, nr_cols, avg_nnz_per_row, std_nnz_per_row, distribution, seed, placement, d_f, avg_bw_target, skew_coeff, save_it, keep_it, low_mb, high_mb, precision,
                              matrix_name)

    pos = find_class(mem_footprint, low_mb_list, high_mb_list)
    if(pos!=-1):
        mem_range = '['+str(low_mb_list[pos])+'-'+str(high_mb_list[pos])+']'
        if(verbose):
            print("\n"+filename, "\t", nr_rows, nr_cols, nr_nnz, '('+str(density)+'%)\t', mem_footprint, 'MB ('+str(precision)+'-bit precision)', 'in', round(time1+time2,3), 'seconds', '\t', mem_range)
            # print("row_ptr[0:10] :", row_ptr[0:10])
            # print("col_ind[0:10] :", col_ind[0:10])
        print("\n\n>>>>", filename, mem_range, new_avg_nnz_per_row, new_std_nnz_per_row, avg_bw, std_bw, avg_sc, std_sc)
        return row_ptr, col_ind, nr_nnz, density, mem_range, new_avg_nnz_per_row, new_std_nnz_per_row, avg_bw, std_bw, avg_sc, std_sc, time1, time2
    else:
        return      [],      [],      0,       0,        '',                   0,                   0,      0,      0,      0,      0,     0,     0

if __name__ == '__main__':
    distribution = "normal"
    placement = "diagonal"
    d_f = 0.5

    seed = 14
    precision = 64
    verbose = 1

    nr_rows_list = [5558326,1000005,68121,1382908,66463,1634989,862664,4690002,170998,4284,63838,268096,185639,206500,121192,217918,46835,38744,5154859,38120,42138,140874,161070,36417,952203,62451,21200,83334,986703,525825,49152]
    avg_nnz_per_row_list = [10.709032,3.105520472,78.94424627,12.23295621,155.7681567,12.08147455,22.29737186,4.33182182,5.607878455,2633.994398,221.6369247,68.96214789,80.86266894,6.166532688,21.65432537,53.38899953,50.68860895,45.72893867,19.24389222,424.2174449,104.73798,55.46377614,50.81725958,119.305956,48.85772782,64.16843605,70.22490566,72.125183,72.63211422,3.994152047,39]
    std_nnz_per_row_list = [1356.616274,25.34520973,1061.43997,37.23001003,350.7443175,31.07497821,29.33341123,1.106157847,4.39216211,4209.259315,95.87570167,105.3875532,126.9718576,4.435865332,13.79266245,4.743895102,27.78059606,38.39531373,5.736719369,484.237499,102.4431672,11.07481064,19.6982847,31.86038422,11.94657153,14.05626099,6.326999832,19.08019415,15.81042955,0.07632277052,0]
    avg_bw_list = [0.5025163836,0.1524629001,0.04535742434,0.02150049625,0.5902849043,0.3400604749,0.248901028,0.0009716598294,0.2972525308,0.9555418052,0.865768503,0.1722725043,0.1935377211,0.001907956522,0.6230549795,0.05932070192,0.1877712645,0.02989415739,0.2119567644,0.4832654419,0.6066963064,0.04587554507,0.03958570835,0.1299377034,0.2042067138,0.008604097649,0.0662003204,0.06981133797,0.01817301802,0.001342169398,0.2446905772]
    skew_coeff_list = [120504.8496,1512.433913,861.9001253,632.7797558,425.2424452,410.3736266,312.2656191,288.0238916,61.94715601,20.32958219,14.44417747,9.179497325,7.186719641,6.135290158,2.740592173,2.371481046,1.86060326,1.755366813,1.442333363,1.317207865,0.9954557077,0.8390381452,0.8104085258,0.7098894878,0.5760045224,0.215550897,0.1391969736,0.1230474105,0.1152091726,0.001464128789, 0]
    matrix_names = ["circuit5M", "webbase-1M", "Chebyshev4", "in-2004", "mip1", "wikipedia-20051105", "eu-2005", "rajat31", "scircuit", "rail4284", "crankseg_2", "Ga41As41H72", "Si41Ge41H72", "mac_econ_fwd500", "cop20k_A", "pwtk", "rma10", "bbmat", "cage15", "TSOPF_RS_b2383", "TSOPF_RS_b300_c3", "shipsec1", "PR02R", "pdb1HYS", "ldoor", "cant", "raefsky3", "consph", "bone010", "mc2depi", "conf5_4-8x8-15"]

    # nr_rows_list = [1000005]
    # avg_nnz_per_row_list = [3.105520472]
    # std_nnz_per_row_list = [25.34520973]
    # avg_bw_list = [0.1524629001]
    # skew_coeff_list = [1512.433913]
    # matrix_names = ["webbase-1M"]

    for i in range(len(skew_coeff_list)):
        nr_rows = nr_rows_list[i]
        nr_cols = nr_rows
        avg_nnz_per_row = avg_nnz_per_row_list[i]
        std_nnz_per_row = std_nnz_per_row_list[i]
        avg_bw = int(np.floor(avg_bw_list[i]*nr_cols))
        # print(avg_bw)
        skew_coeff = skew_coeff_list[i]
        matrix_name = matrix_names[i]
        a = sparse_matrix_generator_wrapper(nr_rows, avg_nnz_per_row, std_nnz_per_row, distribution, placement, d_f, avg_bw, seed, precision, verbose)
        break
