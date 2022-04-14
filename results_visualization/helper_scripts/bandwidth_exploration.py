# %config Completer.use_jedi = False
# %matplotlib notebook
# %matplotlib inline
from scipy.io import mmread
import numpy as np
import matplotlib.pyplot as plt
import glob
from os import listdir
from os.path import isfile, join
import pylab
import pandas as pd
from pprint import pprint

from fitter import Fitter, get_common_distributions, get_distributions


import os, psutil
def print_mem_usage():
    print("MEMORY USED :", psutil.Process(os.getpid()).memory_info().rss / 1024 ** 2, "MB")


starter = "matrix"+"\t"+"nr_rows"+"\t"+"nr_cols"+"\t"+"nr_nnzs"+"\t"+"density"+"\t"+"nnz-r-min"+"\t"+"nnz-r-max"+"\t"+"nnz-r-avg"+"\t"+"nnz-r-std"+"\t"+"nnz-c-min"+"\t"+"nnz-c-max"+"\t"+"nnz-c-avg"+"\t"+"nnz-c-std"+"\t"+"bw-min"+"\t"+"bw-max"+"\t"+"bw-avg"+"\t"+"bw-std"+"\t"+"sc-min"+"\t"+"sc-max"+"\t"+"sc-avg"+"\t"+"sc-std"+"\t"+"ng-min"+"\t"+"ng-max"+"\t"+"ng-avg"+"\t"+"ng-std"+"\t"+"dis-min"+"\t"+"dis-max"+"\t"+"dis-avg"+"\t"+"dis-std"+"\t"+"cl-min"+"\t"+"cl-max"+"\t"+"cl-avg"+"\t"+"cl-std"+"\n"
def retrieve_files(mypath):
    mtx_files = [mypath+f for f in listdir(mypath) if (".mtx" in join(mypath, f) and isfile(join(mypath, f)))]
    return mtx_files

'''
---
# MATRIX FEATURE EXTRACTION
### features
- **nnz/row** : simple
- **nnz/col** : simple
- **bandwidth** : column distance between the first and last nonzero element of each row (normalized by number of columns)
- **scattering** : how nonzeros are scattered within each row (how irregular the accesses to the right hand-side vector will be)
- **ngroups** : number of groups formed by consecutive elements of each row
- **dis** : average distance between each ngroup of each row
- **clustering** : how clustered nonzero groups are within each row
'''

def ngroups_and_dis_calc(row_ptr, col_ind, nnz_per_row):
    ngroups = []
    dis = []
    for i in range(len(row_ptr)-1):
        if(nnz_per_row[i]>0):
            curr_ng = 1
            prev = col_ind[row_ptr[i]]
            tmp_dis = []
            for j in range(row_ptr[i], row_ptr[i+1]):
                if(col_ind[j] > prev+1): # means we need to increase ngroups. in addition, keep in tmp_dis the new distance between the groups
                    curr_ng+=1
                    tmp_dis.append(col_ind[j]-prev)
                prev = col_ind[j]
            ngroups.append(curr_ng)
            dis.append(sum(tmp_dis)/len(tmp_dis) if(len(tmp_dis)>0) else 0)
        else:
            ngroups.append(0)
            dis.append(0)
    ngroups = np.asarray(ngroups)
    dis = np.asarray(dis)
    return ngroups,dis

########################################################################################

def partial_bandwidth_calculation(row_ptr, col_ind, bandwidth, parts):
    bandwidth_parts = np.zeros((len(row_ptr)-1,parts))
    scattering_parts = np.zeros((len(row_ptr)-1,parts))
    cnt = 0
    for i in range(len(row_ptr)-1):
        row_nz = (row_ptr[i+1]-row_ptr[i])
        # if(row_nz>2*parts):
        if(row_nz>1):
            alt_parts = parts
            step = 0
            while(step<2):
                step = row_nz // alt_parts
                # print("row", i, "\trow_nonzeros = ", row_nz, "\talt_parts", alt_parts, "\tstep =", step)
                if(step<2):
                    alt_parts = alt_parts//2

            bounds = [(row_ptr[i] + j*step, row_ptr[i] + (j+1)*step - 1) for j in range(alt_parts)]
            bounds[-1] = tuple([list(bounds[-1])[0], max(list(bounds[-1])[1], row_ptr[i+1]-1)])
            
            partial_col_ind = col_ind[row_ptr[i]:row_ptr[i+1]]
            
            bandwidth_parts[i][:alt_parts] = [(col_ind[b_j] - col_ind[b_i]) for (b_i,b_j) in bounds]
            scattering_parts[i][:alt_parts] = [(col_ind[b_j] - col_ind[b_i])/(b_j-b_i+1) for (b_i,b_j) in bounds]

            # if(row_nz>16):
            #     cnt+=1
            #     print("\nrow :", i, "\t\tbw :", bandwidth[i], "\trow_nz :", row_nz, "(row_ptr =",row_ptr[i]," - ",row_ptr[i+1]-1,")\tstep :", step, "alt_parts = ", alt_parts)
            #     print(bounds)
            #     print([ for (b_i,b_j) in bounds])
            #     print([(col_ind[b_i], col_ind[b_j]) for (b_i,b_j) in bounds])
            #     print('\t', partial_col_ind)
            #     print([int(x) for x in list(bandwidth_parts[i])])
            # if(cnt==1000):
            #     break

    return bandwidth_parts, scattering_parts

def nonzero_distribution(bounds, alt_parts, row_nz, partial_col_ind):
    bw_distrib = np.zeros((alt_parts))
    b_u = list(bounds[0])[1]
    cnt = 0
    for c_i in partial_col_ind:
        if(c_i > b_u):
            cnt+=1
            b_u = list(bounds[cnt])[1]
        bw_distrib[cnt] += 1
    bw_distrib = bw_distrib / row_nz * 100
    return bw_distrib

def partial_bandwidth_calculation2(row_ptr, col_ind, bandwidth, parts):
    bandwidth_parts2 = np.zeros((len(row_ptr)-1,parts))
    for i in range(len(row_ptr)-1):
        row_nz = (row_ptr[i+1]-row_ptr[i])
        if(row_nz>1):
            alt_parts = parts
            step = 0
            if(bandwidth[i] == 1):
                alt_parts = 1
                step = bandwidth[i]
            else:
                while(step<2):
                    step = bandwidth[i] // alt_parts
                    # print("row", i, "\tbandwidth = ", bandwidth[i], "\trow_nz :", row_nz,  "\talt_parts", alt_parts, "\tstep =", step)
                    if(step<2):
                        alt_parts = alt_parts//2

            bounds = [(col_ind[row_ptr[i]] + j*step, col_ind[row_ptr[i]] + (j+1)*step - 1) for j in range(alt_parts)]
            bounds[-1] = tuple([list(bounds[-1])[0], max(list(bounds[-1])[1], col_ind[row_ptr[i+1]-1])])

            partial_col_ind = col_ind[row_ptr[i]:row_ptr[i+1]]

            bandwidth_parts2[i][:alt_parts] = nonzero_distribution(bounds, alt_parts, row_nz, partial_col_ind)
            # print("\nrow :", i, "\t\tbw :", bandwidth[i], "\trow_nz :", row_nz, "\tstep :", step, "alt_parts = ", alt_parts)
            # print(bounds)
            # print('\t', partial_col_ind)
            # print([round(x,2) for x in list(bandwidth_parts2[i])])
    return bandwidth_parts2, []

########################################################################################


def bandwidth_exploration(filename, parts, strat, plot_it):
    spm_coo = mmread(filename)
    print_mem_usage()
    filename = filename.split("/")[-1]

    nr_rows = spm_coo.get_shape()[0]
    nr_cols = spm_coo.get_shape()[1]
    nr_nnzs = spm_coo.getnnz()
    spm = spm_coo.tocsr()
    row_ptr = spm.indptr
    col_ind = spm.indices
    values = spm.data

    mem_footprint = round((row_ptr.nbytes + col_ind.nbytes + values.nbytes)/(1024*1024),3)
    print(filename, ":\tdimensions", spm_coo.get_shape(), "/ nnz", spm_coo.getnnz(),"/ mem footprint",mem_footprint,'MB (CSR)')

    spm_csc = spm_coo.tocsc()
    col_ptr = spm_csc.indptr

    # simple, just keep #nnzs of each row and each column separately
    nnz_per_row = np.ediff1d(row_ptr)    
    nnz_per_col = np.ediff1d(col_ptr)

    # the column distance between the first and last nonzero element of each row (normalized by number of columns)
    bandwidth   = np.asarray([(col_ind[row_ptr[i+1]-1]-col_ind[row_ptr[i]]) if nnz_per_row[i]>0 else 0 for i in range(len(row_ptr)-1)])
    print_mem_usage()

    if(strat == "bw-distrib"):
        bandwidth_parts, scattering_parts = partial_bandwidth_calculation(row_ptr, col_ind, bandwidth, parts)
    elif(strat == "nz-distrib"):
        bandwidth_parts, scattering_parts = partial_bandwidth_calculation2(row_ptr, col_ind, bandwidth, parts)
    
    print_mem_usage()

    print(nr_cols)
    print("AVG of total bandwidth", np.mean(bandwidth))
    print("AVG VALUES OF EACH PART")
    for i in range(parts):
        print("PART",i,"\t",np.mean(bandwidth_parts[:,i]))

    if(plot_it==True):
        # print(plt.style.available)
        # plt.style.use('seaborn-dark-palette')

        height = 1+1+parts//2 # 1 for total_bw, 1 for bw_partials + parts/2 for bw_parts
        fig,ax = plt.subplots(height,2)
        fig.set_size_inches(20,3*height)

        x = np.arange(nr_rows)

        ax1 = plt.subplot2grid((height, 2), (0,0), colspan=2)
        ax1.plot(x,bandwidth, label='total bandwidth', c='g')
        ax1.set_title("total bandwidth")

        ax2 = plt.subplot2grid((height, 2), (1,0), colspan=2)
        colors = ['indianred',   'goldenrod',      'cyan',           'cornflowerblue',
                  'maroon',      'darkorange',     'mediumseagreen', 'midnightblue',
                  'orangered',   'yellow',         'darkslategrey',  'indigo',
                  'saddlebrown', 'darkolivegreen', 'firebrick' ,     'magenta'
                  ]

        for i in range(parts):
            ax2.plot(x,bandwidth_parts[:,i], label='part '+str(i), c=colors[i])

        if(strat == "bw-distrib"):
            ax2.set_title(filename + " bandwidth_parts (1st implementation - row-nonzeros split in equal parts)")
            ax2.set_ylabel("bandwidth (distance between 1st and last nonzero of part)")
        elif(strat == "nz-distrib"):
            ax2.set_title(filename + " bandwidth_parts (2nd implementation - bandwidth split in equal parts)")
            ax2.set_ylabel("percentage of nonzeros present in each part")
        ax2.set_xlabel("#row")

        ax2.legend()

        for i in range(parts//2):
            label = "part "+str(2*i+1)
            ax3 = plt.subplot2grid((height, 2), (int(2+i),0))
            ax3.plot(x,bandwidth_parts[:,2*i], label=label, c=colors[2*i])
            ax3.set_title(label)

            label = "part "+str(2*i+2)
            ax4 = plt.subplot2grid((height, 2), (int(2+i),1))
            ax4.plot(x,bandwidth_parts[:,2*i+1], label=label, c=colors[2*i+1])
            ax4.set_title(label)

        plt.tight_layout()

        plt.savefig("../../validation_matrices/images/bandwidth_exploration/"+str(parts)+"_parts_"+strat+"/"+filename.split("/")[-1].replace(".mtx","_"+str(parts)+"_parts_bandwidth_study.jpg"),transparent=False, dpi=150)
        plt.pause(0.05)

    stats1 = [filename.replace(".mtx",""), nr_cols, np.mean(bandwidth), "\t".join([str(np.mean(bandwidth_parts[:,pp])) for pp in range(parts)])]
    stats2 = [filename.replace(".mtx",""), nr_cols, np.mean(bandwidth), "\t".join([str(np.mean(scattering_parts[:,pp])) for pp in range(parts)])]
    line1 = "\t".join((str(x) for x in stats1))
    line2 = "\t".join((str(x) for x in stats2))
    print(line1)
    print(line2,'\n')
    return line1,line2


def pprint_results(matrixx, df_dict, parts_list):
    print("\t [  matrix_name ,      bw_total,    avg,      std,    varcoeff]")
    matrix_results = [df[df["matrix_name"]==matrixx][["matrix_name","bw_total","avg-parts","std-parts","varcoeff-parts"]].values.flatten().tolist() for df in df_dict]
    for i in range(len(parts_list)):
        print(parts_list[i], '\t' ,matrix_results[i])
    print()


def wrapper(filenames, parts, strat, plot_it):
    print("Wrapper for", len(filenames), "files ( split in", parts,"parts with strat",strat,"), plot_it =", plot_it)
    stats_list1 = []
    stats_list2 = []
    for filename in filenames:
        print_mem_usage()
        line1, line2 = bandwidth_exploration(filename, parts, strat, plot_it)
        stats_list1.append(line1)
        stats_list2.append(line2)

    file1 = open("/various/pmpakos/SpMV-Research/Pythonia/local_ipynb/bandwidth_exploration_"+str(parts)+"_parts_"+strat+"_stats_bw.txt","w")
    file2 = open("/various/pmpakos/SpMV-Research/Pythonia/local_ipynb/bandwidth_exploration_"+str(parts)+"_parts_"+strat+"_stats_sc.txt","w")
    for line in stats_list1:
        file1.write(line+"\n")
    for line in stats_list2:
        file2.write(line+"\n")
    file1.close()
    file2.close()


if __name__ == '__main__':

    filenames = [
        "/various/pmpakos/SpMV-Research/validation_matrices/cage15.sorted.mtx",
        "/various/pmpakos/SpMV-Research/validation_matrices/circuit5M.sorted.mtx",
        "/various/pmpakos/SpMV-Research/validation_matrices/bone010.sorted.mtx",
        "/various/pmpakos/SpMV-Research/validation_matrices/ldoor.sorted.mtx",
        "/various/pmpakos/SpMV-Research/validation_matrices/rajat31.sorted.mtx",
        "/various/pmpakos/SpMV-Research/validation_matrices/TSOPF_RS_b2383.sorted.mtx",
        "/various/pmpakos/SpMV-Research/validation_matrices/Ga41As41H72.sorted.mtx",
        "/various/pmpakos/SpMV-Research/validation_matrices/wikipedia-20051105.sorted.mtx",
        "/various/pmpakos/SpMV-Research/validation_matrices/eu-2005.sorted.mtx",
        "/various/pmpakos/SpMV-Research/validation_matrices/PR02R.sorted.mtx",
        "/various/pmpakos/SpMV-Research/validation_matrices/Si41Ge41H72.sorted.mtx",
        "/various/pmpakos/SpMV-Research/validation_matrices/in-2004.sorted.mtx",
        "/various/pmpakos/SpMV-Research/validation_matrices/Chebyshev4.sorted.mtx",
        "/various/pmpakos/SpMV-Research/validation_matrices/rail4284.real.mtx",
        "/various/pmpakos/SpMV-Research/validation_matrices/pwtk.sorted.mtx",
        "/various/pmpakos/SpMV-Research/validation_matrices/crankseg_2.sorted.mtx",
        "/various/pmpakos/SpMV-Research/validation_matrices/TSOPF_RS_b300_c3.sorted.mtx",
        "/various/pmpakos/SpMV-Research/validation_matrices/shipsec1.sorted.mtx",
        "/various/pmpakos/SpMV-Research/validation_matrices/consph.sorted.mtx",
        "/various/pmpakos/SpMV-Research/validation_matrices/webbase-1M.sorted.mtx",
        "/various/pmpakos/SpMV-Research/validation_matrices/mip1.sorted.mtx",
        "/various/pmpakos/SpMV-Research/validation_matrices/rma10.sorted.mtx",
        "/various/pmpakos/SpMV-Research/validation_matrices/cant.sorted.mtx",
        "/various/pmpakos/SpMV-Research/validation_matrices/conf5_4-8x8-15.real.mtx",
        "/various/pmpakos/SpMV-Research/validation_matrices/pdb1HYS.sorted.mtx",
        "/various/pmpakos/SpMV-Research/validation_matrices/bbmat.sorted.mtx",
        "/various/pmpakos/SpMV-Research/validation_matrices/raefsky3.sorted.mtx",
        "/various/pmpakos/SpMV-Research/validation_matrices/mc2depi.real.mtx",
        "/various/pmpakos/SpMV-Research/validation_matrices/mac_econ_fwd500.sorted.mtx",
        "/various/pmpakos/SpMV-Research/validation_matrices/scircuit.sorted.mtx",
        "/various/pmpakos/SpMV-Research/validation_matrices/cop20k_A.sorted.mtx",
    ]

    plot_it = True
    strat = "bw-distrib"
    # strat = "nz-distrib"

    parts_list = [2,4,8,16]
    for parts in parts_list:
        wrapper(filenames, parts, strat, plot_it)
    ##############################################################################################################################

    df_dict = []
    for parts in parts_list:
        filename = "/various/pmpakos/SpMV-Research/Pythonia/local_ipynb/bandwidth_exploration_"+str(parts)+"_parts_"+strat+"_stats_bw.txt"
        filename = "/various/pmpakos/SpMV-Research/Pythonia/local_ipynb/bandwidth_exploration_"+str(parts)+"_parts_"+strat+"_stats_sc.txt"

        header = ["matrix_name", "nr_cols", "bw_total"]
        prt = ["part "+str(i) for i in range(parts)]

        df = pd.read_table(filename, delimiter ="\t", names=header + prt) 
        df["avg-parts"] = df[prt].mean(axis=1)
        df["std-parts"] = df[prt].std(axis=1)
        df["varcoeff-parts"] = df["std-parts"] / df["avg-parts"] * 100

        # df.sort_values(by=['std-parts'])
        df = df.round(2)
        df1 = df[header+["avg-parts","std-parts", "varcoeff-parts"]+prt]
        df2 = df[header+["avg-parts","std-parts", "varcoeff-parts"]]
        
        df1.to_csv(filename.replace(".txt","1.csv"), index=False, sep='\t')
        df2.to_csv(filename.replace(".txt","2.csv"), index=False, sep='\t')
        
        df_dict.append(df)

    # matrixx = "Chebyshev4.sorted"
    # pprint_results(matrixx, df_dict, parts_list)

    # matrixx = "circuit5M.sorted"
    # pprint_results(matrixx, df_dict, parts_list)

    # matrixx = "ldoor.sorted"
    # pprint_results(matrixx, df_dict, parts_list)

    # matrixx = "wikipedia-20051105.sorted"
    # pprint_results(matrixx, df_dict, parts_list)