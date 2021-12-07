import os
import numpy as np
import random
import time
import multiprocessing as mp

import matplotlib.pyplot as plt
import itertools 


from contextlib import closing
import logging

import scipy
# info = mp.get_logger().info

prefix = "/various/pmpakos/SpMV-Research/artificial_matrix_generation/pmpakos_impl/generated_matrices/"
prefix = "/mnt/various/SpMV-Research/artificial_matrix_generation/pmpakos_impl/generated_matrices/"

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
    index_range, nnz_range, partial_snd, nr_cols, placement, seed, avg_bw = workload_augmented
    np.random.seed(seed)

    i0, i1 = index_range
    random.seed(seed+i0)
    l0, l1 = nnz_range

    offset = i0
    partial_col_ind_list  = []
    partial_bw = np.zeros((i1-i0,))
    partial_sc = np.zeros((i1-i0,))

    reseed_period=1000
    for row in range(i0, i1):
        # random.seed(seed+row)
        row_nonzeros = partial_snd[row-offset]
        if(row_nonzeros>0):

            # random.seed(seed+row)
            # try to do what dgal does with reseeding period for exact replication of results (difficult to achieve, i think)
            if(row%reseed_period==0):
                random.seed(seed+row)

            ###################################################################################
            full_range = np.floor(avg_bw + 0.5)
            if(full_range<row_nonzeros):
                full_range = row_nonzeros
            half_range = full_range//2

            if(placement=="random"):
                bound_relaxed_l = int(nr_cols//2 - half_range)
                bound_relaxed_r = int(nr_cols//2 + half_range + full_range%2)
            elif(placement=="diagonal"):
                bound_relaxed_l = int(row - half_range)
                bound_relaxed_r = int(row + half_range + full_range%2)
            bound_l = bound_relaxed_l
            bound_r = bound_relaxed_r
            if(bound_l < 0):
                bound_l = 0
                if(bound_r < row_nonzeros):
                    bound_r = row_nonzeros
            if(bound_r > nr_cols):
                bound_r = nr_cols
                if(bound_l > nr_cols - row_nonzeros):
                    bound_l = nr_cols - row_nonzeros
            # if(row>=100 and row<=150):
            # # if(row==0):
            # #     print("type(bound_relaxed_l)", type(bound_relaxed_l), "\ttype(bound_relaxed_r)", type(bound_relaxed_r), "\ttype(bound_l)", type(bound_l), "\ttype(bound_r)", type(bound_r))
            # print("row:",row,"\tbound_l", bound_l, "\tbound_r", bound_r,"\tbound_relaxed_l", bound_relaxed_l, "\tbound_relaxed_r", bound_relaxed_r)
            ###################################################################################
            if(1):
                local_col_ind = np.sort(random.sample(range( bound_l, bound_r+1), row_nonzeros))
                # print("aaa")
            else:            
                d1, d2 = 0, 0
                local_col_ind=[]
                for nz in range(row_nonzeros):
                    k = random.randrange(bound_relaxed_l, bound_relaxed_r)
                    if(k >= bound_r):
                        k = bound_r-1
                    elif(k < bound_l):
                        k = bound_l
                    # while(k in local_col_ind):
                    #     k+=1
                    #     if(k >= bound_r):
                    #         k = bound_l
                    retries = 0
                    # Performance bug when bw ~= nnz_per_row     ->    O(nnz_per_row^2 * log(nnz_per_row)).
                    while(k in local_col_ind):
                        retries+=1
                        # if we fail 20 times, we can assume it is nearly filled (e.g. 1/2 ^ 20 = 1/1048576 for half filled row)
                        if(retries<20):
                            k = random.randrange(bound_l, bound_r)
                        else:
                            if(d1<d2):
                                k = bound_l + d1
                                d1+=1
                            else:
                                k = bound_r - d2
                                d2+=1
                    local_col_ind.append(k)
                local_col_ind = np.sort(local_col_ind)

            # if(row%1000==0):
                # print("r",row,"\tnz", row_nonzeros, "\tbound_l", bound_l, "\tbound_r", bound_r,"\tbound_relaxed_l", bound_relaxed_l, "\tbound_relaxed_r", bound_relaxed_r)#, "\t\t", local_col_ind)
            # print("r",row,"\tnz", row_nonzeros, "\tbound_l", bound_l, "\tbound_r", bound_r,"\tbound_relaxed_l", bound_relaxed_l, "\tbound_relaxed_r", bound_relaxed_r, "\t\t", local_col_ind)


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

    if(C_exp > 2):
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
    else:
        MAX_exp = 0
        C_exp = 0
        avg_n = avg_nnz_per_row
        std_n = std_nnz_per_row
    
    return MAX_exp, C_exp, avg_n, std_n

def pdf(x, B, n): # https://gist.github.com/zed/875795
    a = (B - x + 1) * n/x * (n-1)/(x-1)
    # vector = np.zeros((round(n)))
    # vector = np.asarray([((x-i) / (B-i)) for i in range(len(vector))])
    # b = vector.prod()
    b=1.0
    for i in range(0, round(n)):
        b *= (x-i) / (B-i)
    return a * b

def expected_bw(B,n):
    E=0.0
    print(len(range(round(n),round(B+1))))
    for i in range(round(n),round(B+1)):
        E+=pdf(i,B,n)*i
    return E

# how bandwidth changes when -> n uniformly-selected random elements are asked in B bandwidth
# instead of n, feed this estimator with the average number of nonzeros, as calculated after generation of each distribution
def calculate_new_bw(B, n):
    # print(B, "\tavg_bw_target")
    E = expected_bw(B, n)
    # print(E, "\tExpectation for given avg_bw_target")
    if(E==0):
        B_new = n
    else:
        ratio = E / B
        # print(ratio, "\tratio (expectation / target_bw)")
        B_new = np.floor(B / ratio)
    # print(B_new, "\tavg_bw_new (divided by ratio)")
    E_new = expected_bw(B_new, n)
    # print(E_new, "\tExpectation for avg_bw_new")
    # print("n = %g, bw = %g, predicted bw = %g, corrected bw = %g, new prediction = %g\n"%( n, B, E, B_new, E_new))
    return B_new

def generate_random_matrix(nr_rows, nr_cols, avg_nnz_per_row_target, std_nnz_per_row_target, \
                           distribution, seed, placement, avg_bw_target, skew_coeff, \
                           save_it, keep_it, low_mb, high_mb, precision):
    # MAX_exp, C_exp, avg_n, std_n = 0, 1, avg_nnz_per_row_target, std_nnz_per_row_target
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

    # nr_nnz_n = round(np.sum(snd_n))
    # nr_nnz_exp = round(np.sum(snd_exp))
    # print("nr_nnz_n",nr_nnz_n)
    # print("nr_nnz_exp",nr_nnz_exp)

    # print("--------------------------\nDISTRIBUTION PARAMETERS")
    # print("NORMAL      -> ", "avg =", np.mean(snd_n), "\tstd =", np.std(snd_n))
    # print("EXPONENTIAL -> ", "avg =", np.mean(snd_exp), "\tstd =", np.std(snd_exp))
    # print("                (C_exp =", C_exp, "/ MAX_exp =", MAX_exp, "/ halflife =", halflife, ")")
    # print("---\nFINAL       -> ", "avg =", avg_nnz_per_row_final, "\tstd =", std_nnz_per_row_final)
    # print("target      -> ", "avg =", avg_nnz_per_row_target, "\t\tstd =", std_nnz_per_row_target)
    # matrix_name = ""
    # print("MATRIX",matrix_name,"difference with target : ",
    #       abs(round((avg_nnz_per_row_final-avg_nnz_per_row_target)/avg_nnz_per_row_target*100,3)),"% and ",
    #       abs(round((std_nnz_per_row_final-std_nnz_per_row_target)/std_nnz_per_row_target*100,3)),"%\n---")

    ################################################################################################################################################################
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


    # Recalculate banwidth with the actual avg nnz per row.
    # fffff = time.time()
    # avg_bw_new = calculate_new_bw(avg_bw_target * nr_cols, avg_nnz_per_row_final)
    avg_bw_new = avg_bw_target * nr_cols
    # print("TOOK ", round(time.time()-fffff,4), " SECONDS")
    # return
    ################################################################################################################################################################
    # start parallel (multiprocess) creation of col_ind
    # thank you https://stackoverflow.com/questions/7894791/use-numpy-array-in-shared-memory-for-multiprocessing
    mp.freeze_support() # https://stackoverflow.com/questions/24374288/where-to-put-freeze-support-in-a-python-script
    # n_processes_list = [40, 20, 10,  8, 4, 2, 1]
    n_processes = os.cpu_count() #os.cpu_count()#//4
    # n_processes = 2
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
            workload_augmented.append([index_range[i], nnz_range, partial_snd, nr_cols, placement, seed, avg_bw_new])
        p.map(parallel_function, workload_augmented)
    # Close the processes.
    p.join()
    ################################################################################################################################################################

    filename =  prefix+\
                "synthetic_"+\
                str(nr_rows)+"_"+str(nr_cols)+"_"+str(nr_nnz)+\
                "_avg"+str(round(avg_nnz_per_row_final,3))+"_std"+str(round(std_nnz_per_row_final,3))+\
                "_"+placement+"_bw"+str(round(avg_bw_target,3))+\
                "_skew"+str(round(skew_coeff,3))+\
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

def sparse_matrix_generator_wrapper_v2(nr_rows, avg_nnz_per_row, std_nnz_per_row, distribution, placement, avg_bw_target, skew_coeff, seed, precision, verbose):
    low_mb_list = [4,8,16,32,64,128,256,512,1024,2048]
    high_mb_list =  [8,16,32,64,128,256,512,1024,2048,4096]
    low_mb, high_mb = low_mb_list[0], high_mb_list[-1]
    save_it = False
    keep_it = True
    nr_cols = nr_rows

    filename, row_ptr, col_ind, nr_nnz, density, mem_footprint, new_avg_nnz_per_row, new_std_nnz_per_row, avg_bw, std_bw, avg_sc, std_sc, time1, time2 = generate_random_matrix(nr_rows, nr_cols, avg_nnz_per_row, std_nnz_per_row, distribution, seed, placement, avg_bw_target, skew_coeff, save_it, keep_it, low_mb, high_mb, precision)
    pos = find_class(mem_footprint, low_mb_list, high_mb_list)

    if(pos!=-1):
        mem_range = '['+str(low_mb_list[pos])+'-'+str(high_mb_list[pos])+']'
        if(verbose):
            print(filename, "\t", nr_rows, nr_cols, nr_nnz, '('+str(density)+'%)\t', mem_footprint, 'MB ('+str(precision)+'-bit precision)', 'in', round(time1+time2,3), 'seconds', '\t', mem_range)
            print("row_ptr[0:10] :", row_ptr[0:10])
            print("col_ind[0:10] :", col_ind[0:10])
        print(">>>>", filename, mem_range, new_avg_nnz_per_row, new_std_nnz_per_row, avg_bw, std_bw, avg_sc, std_sc)
        return row_ptr, col_ind, nr_nnz, density, mem_range, new_avg_nnz_per_row, new_std_nnz_per_row, avg_bw, std_bw, avg_sc, std_sc, time1, time2
    else:
        return      [],      [],      0,       0,        '',                   0,                   0,      0,      0,      0,      0,     0,     0

if __name__ == '__main__':
    print("main")
    # sparse_matrix_generator_wrapper_v2(207000,8.5,0.84,"normal","random",0.1, 0, 14, 64, 1)
