from scipy.io import mmread
import numpy as np
import matplotlib.pyplot as plt
import glob
from os import listdir
from os.path import isfile, join

starter = "matrix"+"\t"+"nr_rows"+"\t"+"nr_cols"+"\t"+"nr_nnzs"+"\t"+"density"+"\t"+"nnz-r-min"+"\t"+"nnz-r-max"+"\t"+"nnz-r-avg"+"\t"+"nnz-r-std"+"\t"+"nnz-c-min"+"\t"+"nnz-c-max"+"\t"+"nnz-c-avg"+"\t"+"nnz-c-std"+"\t"+"bw-min"+"\t"+"bw-max"+"\t"+"bw-avg"+"\t"+"bw-std"+"\t"+"sc-min"+"\t"+"sc-max"+"\t"+"sc-avg"+"\t"+"sc-std"+"\t"+"ng-min"+"\t"+"ng-max"+"\t"+"ng-avg"+"\t"+"ng-std"+"\t"+"dis-min"+"\t"+"dis-max"+"\t"+"dis-avg"+"\t"+"dis-std"+"\t"+"cl-min"+"\t"+"cl-max"+"\t"+"cl-avg"+"\t"+"cl-std"+"\n"
def retrieve_files(mypath):
    mtx_files = [mypath+f for f in listdir(mypath) if (".mtx" in join(mypath, f) and isfile(join(mypath, f)))]
    return mtx_files

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
    
#     nnz_r_hist, bin_edges_r = np.histogram(nnz_per_row,bins=range(min(nnz_per_row), max(nnz_per_row)+2,1))
    
#     print("nnz_per_row : min =",min(nnz_per_row), "max =",max(nnz_per_row), "range =",max(nnz_per_row)+1-min(nnz_per_row))
#     print("bin_edges_r", bin_edges_r[:-1])
#     print("nnz_r_hist", nnz_r_hist)
        
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
#         elif("0K.mtx" in filename):
#             img_path = "/various/pmpakos/exafoam_matrices"
#             img_filename = glob.glob(img_path + "/**/"+filename.replace(".mtx",".png"), recursive = True)
#             ext = "exafoam_matrices/"
        else:
            # img_path = "/various/pmpakos/SuiteSparse-5.8.1/ssget/files/"
#             img_path = "/home/pmpakos/without_and_with_rcm"
            img_filename = ["/various/pmpakos/SpMV-Research/validation_matrices/images/" + filename.split(".")[0]+".png"]
#             ext = "athena_matrices/"
        ext = "./"
        if(len(img_filename)>0): # only if png file is found, else show nothing in image plot
            im = plt.imread(img_filename[0])

            axs[0,0].imshow(im)
            axs[0,0].set_title(filename)
            axs[0,0].set_axis_off()

#         axs[0,1].hist(nnz_per_row, bins=range(min(nnz_per_row), max(nnz_per_row)+2,1))
#         axs[0,1].set_title("#nnz/row histogram")

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
#         plt.savefig("/home/pmpakos/sparse_matrix_features/"+ext+filename.replace(".mtx","_features.jpg"),transparent=False)
        plt.savefig("/various/pmpakos/SpMV-Research/validation_matrices/features/"+filename.replace(".mtx","_features.jpg"),transparent=False, dpi=150)
        plt.pause(0.05)
    
#     ################################################################################################
#     # plot histogram of nonzeros per row separately
#     fig,ax = plt.subplots(1,1)
#     fig.set_size_inches(20,12)
#     ax.hist(nnz_per_row, bins=range(min(nnz_per_row), max(nnz_per_row)+2,1))
#     ax.set_title(filename +" - #nnz/row histogram")
#     ax.set_xlabel("nonzeros per row")
#     ax.set_ylabel("occurences")
#     # Make some labels.
#     rects = ax.patches
#     labels = [nnz_r_hist[i] for i in range(len(rects))]
#     for rect, label in zip(rects, labels):
#         height = rect.get_height()
#         ax.text(rect.get_x() + rect.get_width() / 2, height+0.01, label,ha='center', va='bottom')        
#     plt.tight_layout()
#     plt.savefig("/home/pmpakos/sparse_matrix_features/histograms/"+filename.replace(".mtx","_histogram.jpg"),transparent=False, dpi=150)
#     plt.pause(0.05)
#     ################################################################################################
    
    return spm, nnz_per_row, nnz_per_col, bandwidth, scatter, ngroups, dis, clustering#, nnz_r_hist, bin_edges_r

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

filenames = [
    "/various/pmpakos/SpMV-Research/validation_matrices/cop20k_A.sorted.mtx",
    "/various/pmpakos/SpMV-Research/validation_matrices/scircuit.sorted.mtx",
    "/various/pmpakos/SpMV-Research/validation_matrices/mac_econ_fwd500.sorted.mtx",
    "/various/pmpakos/SpMV-Research/validation_matrices/mc2depi.real.mtx",
    "/various/pmpakos/SpMV-Research/validation_matrices/raefsky3.sorted.mtx",
    "/various/pmpakos/SpMV-Research/validation_matrices/bbmat.sorted.mtx",
    "/various/pmpakos/SpMV-Research/validation_matrices/pdb1HYS.sorted.mtx",
    "/various/pmpakos/SpMV-Research/validation_matrices/conf5_4-8x8-15.real.mtx",
    "/various/pmpakos/SpMV-Research/validation_matrices/cant.sorted.mtx",
    "/various/pmpakos/SpMV-Research/validation_matrices/rma10.sorted.mtx",
    "/various/pmpakos/SpMV-Research/validation_matrices/mip1.sorted.mtx",
    "/various/pmpakos/SpMV-Research/validation_matrices/webbase-1M.sorted.mtx",
    "/various/pmpakos/SpMV-Research/validation_matrices/consph.sorted.mtx",
    "/various/pmpakos/SpMV-Research/validation_matrices/shipsec1.sorted.mtx",
    "/various/pmpakos/SpMV-Research/validation_matrices/TSOPF_RS_b300_c3.sorted.mtx",
    "/various/pmpakos/SpMV-Research/validation_matrices/crankseg_2.sorted.mtx",
    "/various/pmpakos/SpMV-Research/validation_matrices/pwtk.sorted.mtx",
    "/various/pmpakos/SpMV-Research/validation_matrices/rail4284.real.mtx",
    "/various/pmpakos/SpMV-Research/validation_matrices/Chebyshev4.sorted.mtx",
    "/various/pmpakos/SpMV-Research/validation_matrices/in-2004.sorted.mtx",
    "/various/pmpakos/SpMV-Research/validation_matrices/Si41Ge41H72.sorted.mtx",
    "/various/pmpakos/SpMV-Research/validation_matrices/PR02R.sorted.mtx",
    "/various/pmpakos/SpMV-Research/validation_matrices/eu-2005.sorted.mtx",
    "/various/pmpakos/SpMV-Research/validation_matrices/wikipedia-20051105.sorted.mtx",
    "/various/pmpakos/SpMV-Research/validation_matrices/Ga41As41H72.sorted.mtx",
    "/various/pmpakos/SpMV-Research/validation_matrices/TSOPF_RS_b2383.sorted.mtx",
    "/various/pmpakos/SpMV-Research/validation_matrices/rajat31.sorted.mtx",
    "/various/pmpakos/SpMV-Research/validation_matrices/ldoor.sorted.mtx",
    "/various/pmpakos/SpMV-Research/validation_matrices/bone010.sorted.mtx",
    "/various/pmpakos/SpMV-Research/validation_matrices/circuit5M.sorted.mtx",
    "/various/pmpakos/SpMV-Research/validation_matrices/cage15.sorted.mtx",
]

plot_it = True

stats_list = []
plot_it = True
for filename in filenames:
    line = stats_extraction(filename, plot_it)
    stats_list.append(line)

file = open("/various/pmpakos/SpMV-Research/validation_matrices/features/0_validation_matrices_features.txt","w")
for line in stats_list:
    file.write(line+"\n")
file.close()