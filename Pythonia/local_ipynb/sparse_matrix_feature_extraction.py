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


def mmread_fun(filename, plot_it=False):
    spm_coo = mmread(filename)
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
    bandwidth   = np.asarray([(col_ind[row_ptr[i+1]-1]-col_ind[row_ptr[i]])/nr_cols if nnz_per_row[i]>0 else 0 for i in range(len(row_ptr)-1)])
    
    # how nonzeros are scattered within each row (how irregular the accesses to the right hand-side vector will be)
    scatter     = np.asarray([(nnz_per_row[i]/(col_ind[row_ptr[i+1]-1]-col_ind[row_ptr[i]])) if (nnz_per_row[i]>0 and (col_ind[row_ptr[i+1]-1]-col_ind[row_ptr[i]])>0) else 0 for i in range(len(row_ptr)-1)])

    # ngroups : number of groups formed by consecutive elements of each row
    # dis : average distance between each ngroup of each row
    ngroups,dis = ngroups_and_dis_calc(row_ptr, col_ind, nnz_per_row)

    # how clustered nonzero groups are within each row
    clustering  = np.asarray([(ngroups[i]/nnz_per_row[i]) if (nnz_per_row[i]>0) else 0 for i in range(len(ngroups))])

    if(plot_it == True):
        fig, axs = plt.subplots(5,2)
        fig.set_size_inches(15,20)
        img_filename = []

        if("sparse_" in filename):
            img_path = "/home/pmpakos/sparse_matrices/"
            img_filename = glob.glob(img_path + "/**/"+filename.replace(".mtx",".png").replace("sparse","fig"), recursive = True)
            if ("_x" in filename):
                ext = "random_matrices_x/"
            elif ("_y" in filename):
                ext = "random_matrices_y/"
            else:
                ext = "./"
        elif("00K.mtx" in filename):
            img_path = "/various/pmpakos/exafoam_matrices"
            img_filename = glob.glob(img_path + "/**/"+filename.replace(".mtx",".png"), recursive = True)
            ext = "exafoam_matrices/"
        else:
            # img_path = "/home/pmpakos/without_and_with_rcm"
            # img_filename = glob.glob(img_path + "/**/"+filename.replace(".mtx",".png"), recursive = True)
            # img_filename = ["/various/pmpakos/SpMV-Research/validation_matrices/images/" + filename.split(".")[0]+".png"]
            img_filename = ["/various/pmpakos/SpMV-Research/validation_matrices/v2_dgal/images/" + filename.split(".mtx")[0]+".png"]
        ext = "./"
        if(len(img_filename)>0): # only if png file is found, else show nothing in image plot
            im = plt.imread(img_filename[0])

            axs[0,0].imshow(im)
            axs[0,0].set_title(filename)
            axs[0,0].set_axis_off()

        axs[1,0].plot(nnz_per_row)
        axs[1,0].set_title("nnz_per_row")

        axs[1,1].plot(nnz_per_col)
        axs[1,1].set_title("nnz_per_col")

        axs[2,0].plot(bandwidth)
        axs[2,0].set_title("bandwidth")

        axs[2,1].plot(scatter)
        axs[2,1].set_title("scatter")

        axs[3,0].plot(ngroups)
        axs[3,0].set_title("ngroups")

        axs[3,1].plot(dis)
        axs[3,1].set_title("dis")

        axs[4,0].plot(clustering)
        axs[4,0].set_title("clustering")
        
        plt.tight_layout()
        # plt.savefig("/various/pmpakos/SpMV-Research/validation_matrices/features/"+filename.replace(".mtx","_features.jpg"),transparent=False, dpi=150)
        plt.savefig("/various/pmpakos/SpMV-Research/validation_matrices/v2_dgal/features/"+filename.replace(".mtx","_features.jpg"),transparent=False, dpi=150)
        plt.pause(0.05)
    
    return spm, nnz_per_row, nnz_per_col, bandwidth, scatter, ngroups, dis, clustering


def return_stats(filename, spm,nnz_per_row, nnz_per_col, bandwidth, scatter, ngroups, dis, clustering):
    filename = filename.split("/")[-1]
    nr_rows = spm.get_shape()[0]
    nr_cols = spm.get_shape()[1]
    nr_nnzs = spm.getnnz()
    density = nr_nnzs / (nr_rows*nr_cols) * 100
    
    min_nnz_per_row = np.min(nnz_per_row)
    max_nnz_per_row = np.max(nnz_per_row)
    avg_nnz_per_row = np.mean(nnz_per_row)
    std_nnz_per_row = np.std(nnz_per_row)

    min_nnz_per_col = np.min(nnz_per_col)
    max_nnz_per_col = np.max(nnz_per_col)
    avg_nnz_per_col = np.mean(nnz_per_col)
    std_nnz_per_col = np.std(nnz_per_col)
    
    min_bandwidth = np.min(bandwidth)
    max_bandwidth = np.max(bandwidth)
    avg_bandwidth = np.mean(bandwidth)
    std_bandwidth = np.std(bandwidth)
    
    min_scatter = np.min(scatter)
    max_scatter = np.max(scatter)
    avg_scatter = np.mean(scatter)
    std_scatter = np.std(scatter)
    
    min_ngroups = np.min(ngroups)
    max_ngroups = np.max(ngroups)
    avg_ngroups = np.mean(ngroups)
    std_ngroups = np.std(ngroups)
    
    min_dis = np.min(dis)
    max_dis = np.max(dis)
    avg_dis = np.mean(dis)
    std_dis = np.std(dis)
    
    min_clustering = np.min(clustering)
    max_clustering = np.max(clustering)
    avg_clustering = np.mean(clustering)
    std_clustering = np.std(clustering)

    stats = [nr_rows,nr_cols,nr_nnzs,density,min_nnz_per_row,max_nnz_per_row,avg_nnz_per_row,std_nnz_per_row,min_nnz_per_col,max_nnz_per_col,avg_nnz_per_col,std_nnz_per_col,min_bandwidth,max_bandwidth,avg_bandwidth,std_bandwidth,min_scatter,max_scatter,avg_scatter,std_scatter,min_ngroups,max_ngroups,avg_ngroups,std_ngroups,min_dis,max_dis,avg_dis,std_dis,min_clustering,max_clustering,avg_clustering,std_clustering]
    stats = "\t".join((str(x) for x in stats))
    line = filename.replace(".mtx","") + "\t" + stats
    return line
    
def stats_extraction(filename, plot_it):
#     spm, nnz_per_row, nnz_per_col, bandwidth, scatter, ngroups, dis, clustering, nnz_r_hist, bin_edges_r = mmread_fun(filename, plot_it)
    spm, nnz_per_row, nnz_per_col, bandwidth, scatter, ngroups, dis, clustering = mmread_fun(filename, plot_it)
    line = return_stats(filename, spm, nnz_per_row, nnz_per_col, bandwidth, scatter, ngroups, dis, clustering)
    
    return line #, nnz_r_hist, bin_edges_r, nnz_per_row

############################################################################################################################################################################################################################
def summary2(f_obj, Nbest=5, lw=2, plot=True, method="sumsquare_error", clf=True):
    """Plots the distribution of the data and Nbest distribution"""
    if plot:
        if clf:
            pylab.clf()
        f_obj.hist()
        f_obj.plot_pdf(Nbest=Nbest, lw=lw, method=method)
        pylab.grid(True)
    Nbest = min(Nbest, len(f_obj.distributions))
    try:
        names = f_obj.df_errors.sort_values(by=method).index[0:Nbest]
    except:  # pragma: no cover
        names = f_obj.df_errors.sort(method).index[0:Nbest]
    return f_obj.df_errors.loc[names]

def plot_histogram(vector, name):
    threshold = 250
    if((max(vector)-min(vector)) > threshold):
        step = (max(vector)+2 - min(vector))//threshold
        bins_range = range(min(vector), max(vector)+2,step)
        print("GOOD", len(list(bins_range)))
    else:
        bins_range = range(min(vector), max(vector)+2,1)
        print("BADD", len(list(bins_range)))

    vector_hist, bin_edges_r = np.histogram(vector, bins=bins_range)
    print(name,": min =",min(vector), "max =",max(vector), "nr_bins =",len(list(bins_range)))
    
    # plot histogram of nonzeros per row separately
    fig,ax = plt.subplots(1,1)
    fig.set_size_inches(20,12)
    ax.hist(vector, bins=bins_range)
    ax.set_title(filename + "       " + name + " histogram")
    ax.set_xlabel(name)
    ax.set_ylabel("occurences")
    # Make some labels.
    rects = ax.patches
    labels = [str(vector_hist[i]) if vector_hist[i]>0 else "" for i in range(len(rects))]
    for rect, label in zip(rects, labels):
        height = rect.get_height()
        ax.text(rect.get_x() + rect.get_width() / 2, height+0.01, label,ha='center', va='bottom')        
    plt.tight_layout()
    plt.savefig("../../validation_matrices/images/distribution_estimation/"+filename.split("/")[-1].replace(".mtx","_histogram_"+name+".jpg"),transparent=False, dpi=150)
    plt.pause(0.05)

    fig,ax = plt.subplots(1,1)
    fig.set_size_inches(20,12)
    f = Fitter(vector, timeout=45,
               distributions=["gamma", "beta", "cauchy", "chi2", "exponpow", "lognorm", "powerlaw", "rayleigh",
                              "burr","norm", "exponweib", "weibull_max", "weibull_min", "pareto", "genextreme"])
    f.fit()
    # f.summary(Nbest=10)
    summary2(f, Nbest=10)

    plt.savefig("../../validation_matrices/images/distribution_estimation/"+filename.split("/")[-1].replace(".mtx","_histogram_distr_"+name+".jpg"),transparent=False, dpi=150)
    plt.pause(0.05)
    print(f.get_best(method = 'sumsquare_error'))
    return vector_hist, bin_edges_r

def mmread_fun2(filename, plot_it=False):
    spm_coo = mmread(filename)
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

    nnz_per_row = np.ediff1d(row_ptr)
    print("nnz_per_row ready")
    bandwidth2  = np.asarray([(col_ind[row_ptr[i+1]-1]-col_ind[row_ptr[i]]) if nnz_per_row[i]>0 else 0 for i in range(len(row_ptr)-1)])
    print("bandwidth2  ready")

    nnzr_hist, nnzr_bin_edges_r = plot_histogram(nnz_per_row, "nnz-row")
    bw_hist,   bw_bin_edges_r   = plot_histogram(bandwidth2,  "bandwidth")
    
    return nnzr_hist, nnzr_bin_edges_r, bw_hist, bw_bin_edges_r

############################################################################################################################################################################################################################

if __name__ == '__main__':
    ############################################################################################################################################################################################################################
    '''
    # DISTRIBUTION ESTIMATION (of nnz-row and bandwidth)
    '''
    # print(get_common_distributions())
    # print('---')
    # print(get_distributions())
    # filenames = [
    #     "/various/pmpakos/SpMV-Research/validation_matrices/cop20k_A.sorted.mtx",
    #     "/various/pmpakos/SpMV-Research/validation_matrices/scircuit.sorted.mtx",
    #     "/various/pmpakos/SpMV-Research/validation_matrices/mac_econ_fwd500.sorted.mtx",
    #     "/various/pmpakos/SpMV-Research/validation_matrices/mc2depi.real.mtx",
    #     "/various/pmpakos/SpMV-Research/validation_matrices/raefsky3.sorted.mtx",
    #     "/various/pmpakos/SpMV-Research/validation_matrices/bbmat.sorted.mtx",
    #     "/various/pmpakos/SpMV-Research/validation_matrices/pdb1HYS.sorted.mtx",
    #     "/various/pmpakos/SpMV-Research/validation_matrices/conf5_4-8x8-15.real.mtx",
    #     "/various/pmpakos/SpMV-Research/validation_matrices/cant.sorted.mtx",
    #     "/various/pmpakos/SpMV-Research/validation_matrices/rma10.sorted.mtx",
    #     "/various/pmpakos/SpMV-Research/validation_matrices/mip1.sorted.mtx",
    #     "/various/pmpakos/SpMV-Research/validation_matrices/webbase-1M.sorted.mtx",
    #     "/various/pmpakos/SpMV-Research/validation_matrices/consph.sorted.mtx",
    #     "/various/pmpakos/SpMV-Research/validation_matrices/shipsec1.sorted.mtx",
    #     "/various/pmpakos/SpMV-Research/validation_matrices/TSOPF_RS_b300_c3.sorted.mtx",
    #     "/various/pmpakos/SpMV-Research/validation_matrices/crankseg_2.sorted.mtx",
    #     "/various/pmpakos/SpMV-Research/validation_matrices/pwtk.sorted.mtx",
    #     "/various/pmpakos/SpMV-Research/validation_matrices/rail4284.real.mtx",
    #     "/various/pmpakos/SpMV-Research/validation_matrices/Chebyshev4.sorted.mtx",
    #     "/various/pmpakos/SpMV-Research/validation_matrices/in-2004.sorted.mtx",
    #     "/various/pmpakos/SpMV-Research/validation_matrices/Si41Ge41H72.sorted.mtx",
    #     "/various/pmpakos/SpMV-Research/validation_matrices/PR02R.sorted.mtx",
    #     "/various/pmpakos/SpMV-Research/validation_matrices/eu-2005.sorted.mtx",
    #     "/various/pmpakos/SpMV-Research/validation_matrices/wikipedia-20051105.sorted.mtx",
    #     "/various/pmpakos/SpMV-Research/validation_matrices/Ga41As41H72.sorted.mtx",
    #     "/various/pmpakos/SpMV-Research/validation_matrices/TSOPF_RS_b2383.sorted.mtx",
    #     "/various/pmpakos/SpMV-Research/validation_matrices/rajat31.sorted.mtx",
    #     "/various/pmpakos/SpMV-Research/validation_matrices/ldoor.sorted.mtx",
    #     "/various/pmpakos/SpMV-Research/validation_matrices/bone010.sorted.mtx",
    #     "/various/pmpakos/SpMV-Research/validation_matrices/circuit5M.sorted.mtx",
    #     "/various/pmpakos/SpMV-Research/validation_matrices/cage15.sorted.mtx",
    # ]
    # for filename in filenames:
    #     print("------")
    #     nnzr_hist, nnzr_bin_edges_r, bw_hist, bw_bin_edges_r = mmread_fun2(filename, plot_it = True)

    ############################################################################################################################################################################################################################
    '''
    # RCM (Reverse Cuthill-McKee) reordering
    '''
    # filenames = [
    #     "/various/pmpakos/athena_ppopp_matrices/filtered/Ronis/xenon2.mtx",
    #     "/various/pmpakos/athena_ppopp_matrices/filtered/PARSEC/Si41Ge41H72.mtx",
    #     "/various/pmpakos/athena_ppopp_matrices/filtered/Dziekonski/dielFilterV3real.mtx",
    #     "/various/pmpakos/athena_ppopp_matrices/filtered/Botonakis/thermomech_dK.mtx",
    #     "/various/pmpakos/athena_ppopp_matrices/filtered/Gleich/wikipedia-20051105.mtx",
    #     "/various/pmpakos/athena_ppopp_matrices/filtered/SNAP/soc-LiveJournal1.mtx",
    #     "/various/pmpakos/athena_ppopp_matrices/filtered/Sandia/ASIC_680k.mtx",
    #     "/various/pmpakos/athena_ppopp_matrices/filtered/Freescale/circuit5M.mtx",
    #     "/various/pmpakos/athena_ppopp_matrices/filtered/LAW/in-2004.mtx",
    #     "/various/pmpakos/exafoam_matrices/100K.mtx",
    #     "/various/pmpakos/exafoam_matrices/600K.mtx"]

    # plot_it = True
    # stats_list = []
    # plot_it = True
    # for filename in filenames:
    #     line = stats_extraction(filename, plot_it)
    #     stats_list.append(line)

    # file = open("targeted_matrices_features.txt","w")
    # for line in stats_list:
    #     file.write(line+"\n")
    # file.close()

    # filenames=[
    #     "/various/pmpakos/athena_ppopp_matrices/filtered/Ronis/xenon2_rcm.mtx",
    #     "/various/pmpakos/athena_ppopp_matrices/filtered/PARSEC/Si41Ge41H72_rcm.mtx",
    #     "/various/pmpakos/athena_ppopp_matrices/filtered/Dziekonski/dielFilterV3real_rcm.mtx",
    #     "/various/pmpakos/athena_ppopp_matrices/filtered/Botonakis/thermomech_dK_rcm.mtx",
    #     "/various/pmpakos/athena_ppopp_matrices/filtered/Gleich/wikipedia-20051105_rcm.mtx",
    #     "/various/pmpakos/athena_ppopp_matrices/filtered/SNAP/soc-LiveJournal1_rcm.mtx",
    #     "/various/pmpakos/athena_ppopp_matrices/filtered/Sandia/ASIC_680k_rcm.mtx",
    #     "/various/pmpakos/athena_ppopp_matrices/filtered/LAW/in-2004_rcm.mtx",
    #     "/various/pmpakos/athena_ppopp_matrices/filtered/Freescale/circuit5M_rcm.mtx",
    #     "/various/pmpakos/exafoam_matrices/100K_rcm.mtx",
    #     "/various/pmpakos/exafoam_matrices/600K_rcm.mtx"]

    # plot_it = True
    # stats_list = []
    # plot_it = True
    # for filename in filenames:
    #     line = stats_extraction(filename, plot_it)
    #     stats_list.append(line)

    # file = open("targeted_matrices_rcm_features.txt","w")
    # for line in stats_list:
    #     file.write(line+"\n")
    # file.close()

    ############################################################################################################################################################################################################################
    '''
    # dgal partitioning
    '''
    # dgal partitioning
    # filenames=[
    #     "/various/pmpakos/athena_ppopp_matrices/filtered/Freescale/circuit5M_sorted_1.mtx",
    #     "/various/pmpakos/athena_ppopp_matrices/filtered/Freescale/circuit5M_sorted_2.mtx",
    #     "/various/pmpakos/athena_ppopp_matrices/filtered/Freescale/circuit5M_sorted_3.mtx",
    #     "/various/pmpakos/athena_ppopp_matrices/filtered/Freescale/circuit5M_sorted_4.mtx",
    #     "/various/pmpakos/athena_ppopp_matrices/filtered/Dziekonski/dielFilterV3real_sorted_1.mtx",
    #     "/various/pmpakos/athena_ppopp_matrices/filtered/Dziekonski/dielFilterV3real_sorted_2.mtx",
    #     "/various/pmpakos/athena_ppopp_matrices/filtered/Dziekonski/dielFilterV3real_sorted_3.mtx",
    #     "/various/pmpakos/athena_ppopp_matrices/filtered/Dziekonski/dielFilterV3real_sorted_4.mtx",
    #     "/various/pmpakos/athena_ppopp_matrices/filtered/SNAP/soc-LiveJournal1_sorted_1.mtx",
    #     "/various/pmpakos/athena_ppopp_matrices/filtered/SNAP/soc-LiveJournal1_sorted_2.mtx",
    #     "/various/pmpakos/athena_ppopp_matrices/filtered/SNAP/soc-LiveJournal1_sorted_3.mtx",
    #     "/various/pmpakos/athena_ppopp_matrices/filtered/SNAP/soc-LiveJournal1_sorted_4.mtx",
    #     "/various/pmpakos/athena_ppopp_matrices/filtered/Gleich/wikipedia-20051105_sorted_1.mtx",
    #     "/various/pmpakos/athena_ppopp_matrices/filtered/Gleich/wikipedia-20051105_sorted_2.mtx",
    #     "/various/pmpakos/athena_ppopp_matrices/filtered/Gleich/wikipedia-20051105_sorted_3.mtx",
    #     "/various/pmpakos/athena_ppopp_matrices/filtered/Gleich/wikipedia-20051105_sorted_4.mtx"
    # ]

    # plot_it = True
    # stats_list = []
    # plot_it = True
    # for filename in filenames:
    #     line = stats_extraction(filename, plot_it)
    #     stats_list.append(line)

    # file = open("targeted_large_matrices_dgal_partitioning.txt","w")
    # for line in stats_list:
    #     file.write(line+"\n")
    # file.close()

    # # dgal partitioning
    # filenames=[
    #     "/various/pmpakos/athena_ppopp_matrices/filtered/dgal_partitioning/artificial_gamma_distribution_soc-LiveJournal1_sorted_4_3953914_3953914_16582299_random_seed_10.mtx",
    #     "/various/pmpakos/athena_ppopp_matrices/filtered/dgal_partitioning/artificial_gamma_distribution_soc-LiveJournal1_sorted_4_3953914_3953914_16582299_diagonal_df0.5_seed_10.mtx",
    #     "/various/pmpakos/athena_ppopp_matrices/filtered/dgal_partitioning/artificial_gamma_distribution_soc-LiveJournal1_sorted_4_3953914_3953914_16582299_diagonal_df0.05_seed_10.mtx",
    #     "/various/pmpakos/athena_ppopp_matrices/filtered/dgal_partitioning/artificial_gamma_distribution_soc-LiveJournal1_sorted_4_3953914_3953914_16582299_diagonal_df0.005_seed_10.mtx",    
    #     "/various/pmpakos/athena_ppopp_matrices/filtered/dgal_partitioning/artificial_normal_distribution_dielFilterV3real_sorted_4_636608_636608_19688400_random_seed_29.mtx",
    #     "/various/pmpakos/athena_ppopp_matrices/filtered/dgal_partitioning/artificial_normal_distribution_dielFilterV3real_sorted_4_636608_636608_19688400_diagonal_df0.5_seed_29.mtx",
    #     "/various/pmpakos/athena_ppopp_matrices/filtered/dgal_partitioning/artificial_normal_distribution_dielFilterV3real_sorted_4_636608_636608_19688400_diagonal_df0.05_seed_29.mtx",
    #     "/various/pmpakos/athena_ppopp_matrices/filtered/dgal_partitioning/artificial_normal_distribution_dielFilterV3real_sorted_4_636608_636608_19688400_diagonal_df0.005_seed_29.mtx",
    # ]

    # stats_list = []
    # plot_it = False
    # for filename in filenames:
    #     line = stats_extraction(filename, plot_it)
    #     stats_list.append(line)

    # file = open("targeted_large_matrices_dgal_partitioning_artificial_digital_twins.txt","w")
    # for line in stats_list:
    #     file.write(line+"\n")
    # file.close()

    ############################################################################################################################################################################################################################
    '''
    v2_dgal generator first validation matrices "friends"
    '''
    filenames = [
        "/various/pmpakos/SpMV-Research/validation_matrices/v2_dgal/matrices/synthetic_4284_4284_9017780_avg2104.991_std764.374_random_bw1.0_skew20.33_n14.mtx",
        "/various/pmpakos/SpMV-Research/validation_matrices/v2_dgal/matrices/synthetic_21200_21200_1488681_avg70.221_std6.341_random_bw0.066_skew0.139_n14.mtx",
        "/various/pmpakos/SpMV-Research/validation_matrices/v2_dgal/matrices/synthetic_36417_36417_4343880_avg119.282_std31.869_random_bw0.13_skew0.71_n14.mtx",
        "/various/pmpakos/SpMV-Research/validation_matrices/v2_dgal/matrices/synthetic_38120_38120_14577949_avg382.423_std245.445_random_bw0.482_skew1.317_n14.mtx",
        "/various/pmpakos/SpMV-Research/validation_matrices/v2_dgal/matrices/synthetic_38744_38744_1768999_avg45.659_std31.436_random_bw0.029_skew1.755_n14.mtx",
        "/various/pmpakos/SpMV-Research/validation_matrices/v2_dgal/matrices/synthetic_42138_42138_3884671_avg92.189_std51.06_random_bw0.602_skew0.995_n14.mtx",
        "/various/pmpakos/SpMV-Research/validation_matrices/v2_dgal/matrices/synthetic_46835_46835_2372893_avg50.665_std27.766_random_bw0.186_skew1.861_n14.mtx",
        "/various/pmpakos/SpMV-Research/validation_matrices/v2_dgal/matrices/synthetic_62451_62451_4007810_avg64.175_std14.044_random_bw0.009_skew0.216_n14.mtx",
        "/various/pmpakos/SpMV-Research/validation_matrices/v2_dgal/matrices/synthetic_63838_63838_14150047_avg221.656_std96.326_random_bw0.866_skew14.444_n14.mtx",
        "/various/pmpakos/SpMV-Research/validation_matrices/v2_dgal/matrices/synthetic_66463_66463_10387201_avg156.285_std398.714_random_bw0.59_skew425.242_n14.mtx",
        "/various/pmpakos/SpMV-Research/validation_matrices/v2_dgal/matrices/synthetic_68121_68121_5407070_avg79.374_std1077.003_random_bw0.045_skew861.9_n14.mtx",
        "/various/pmpakos/SpMV-Research/validation_matrices/v2_dgal/matrices/synthetic_83334_83334_6011057_avg72.132_std19.105_random_bw0.07_skew0.123_n14.mtx",
        "/various/pmpakos/SpMV-Research/validation_matrices/v2_dgal/matrices/synthetic_121192_121192_2626755_avg21.674_std13.799_random_bw0.608_skew2.741_n14.mtx",
        "/various/pmpakos/SpMV-Research/validation_matrices/v2_dgal/matrices/synthetic_140874_140874_7813336_avg55.463_std11.085_random_bw0.046_skew0.839_n14.mtx",
        "/various/pmpakos/SpMV-Research/validation_matrices/v2_dgal/matrices/synthetic_161070_161070_8186221_avg50.824_std19.687_random_bw0.039_skew0.81_n14.mtx",
        "/various/pmpakos/SpMV-Research/validation_matrices/v2_dgal/matrices/synthetic_170998_170998_958305_avg5.604_std4.459_random_bw0.287_skew61.947_n14.mtx",
        "/various/pmpakos/SpMV-Research/validation_matrices/v2_dgal/matrices/synthetic_185639_185639_15011481_avg80.864_std117.762_random_bw0.188_skew7.187_n14.mtx",
        "/various/pmpakos/SpMV-Research/validation_matrices/v2_dgal/matrices/synthetic_206500_206500_1273294_avg6.166_std4.444_random_bw0.002_skew6.135_n14.mtx",
        "/various/pmpakos/SpMV-Research/validation_matrices/v2_dgal/matrices/synthetic_217918_217918_11619584_avg53.321_std4.756_random_bw0.059_skew2.371_n14.mtx",
        "/various/pmpakos/SpMV-Research/validation_matrices/v2_dgal/matrices/synthetic_268096_268096_18498642_avg69.0_std101.276_random_bw0.168_skew9.179_n14.mtx",
        "/various/pmpakos/SpMV-Research/validation_matrices/v2_dgal/matrices/synthetic_525825_525825_2104694_avg4.003_std0.078_random_bw0.001_skew0.001_n14.mtx",
        "/various/pmpakos/SpMV-Research/validation_matrices/v2_dgal/matrices/synthetic_862664_862664_19213423_avg22.272_std29.816_random_bw0.249_skew312.266_n14.mtx",
        "/various/pmpakos/SpMV-Research/validation_matrices/v2_dgal/matrices/synthetic_952203_952203_46522684_avg48.858_std11.947_random_bw0.204_skew0.576_n14.mtx",
        "/various/pmpakos/SpMV-Research/validation_matrices/v2_dgal/matrices/synthetic_986703_986703_71659094_avg72.625_std15.808_random_bw0.018_skew0.115_n14.mtx",
        "/various/pmpakos/SpMV-Research/validation_matrices/v2_dgal/matrices/synthetic_1000005_1000005_3170796_avg3.171_std25.563_random_bw0.147_skew1512.434_n14.mtx",
        "/various/pmpakos/SpMV-Research/validation_matrices/v2_dgal/matrices/synthetic_1382908_1382908_16945359_avg12.253_std37.522_random_bw0.021_skew632.78_n14.mtx",
        "/various/pmpakos/SpMV-Research/validation_matrices/v2_dgal/matrices/synthetic_1634989_1634989_19780430_avg12.098_std31.198_random_bw0.339_skew410.374_n14.mtx",
        "/various/pmpakos/SpMV-Research/validation_matrices/v2_dgal/matrices/synthetic_4690002_4690002_18769835_avg4.002_std1.183_random_bw0.001_skew288.024_n14.mtx",
        "/various/pmpakos/SpMV-Research/validation_matrices/v2_dgal/matrices/synthetic_5154859_5154859_99196687_avg19.243_std5.743_random_bw0.211_skew1.442_n14.mtx",
        "/various/pmpakos/SpMV-Research/validation_matrices/v2_dgal/matrices/synthetic_5558326_5558326_60190921_avg10.829_std1412.188_random_bw0.45_skew120504.85_n14.mtx",
    ]

    stats_list = []
    plot_it = True
    for filename in filenames:
        print_mem_usage()
        line = stats_extraction(filename, plot_it)
        stats_list.append(line)

    file = open("/various/pmpakos/SpMV-Research/Benchmarks/validation_matrices_synthetic_features.txt","w")
    for line in stats_list:
        file.write(line+"\n")
    file.close()
