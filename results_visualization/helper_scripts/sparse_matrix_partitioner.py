# %config Completer.use_jedi = False
# %matplotlib notebook
# %matplotlib inline
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

############################################################################################################################################################################################################################

if __name__ == '__main__':
    filename = "/various/pmpakos/athena_ppopp_matrices/filtered/Sandia/ASIC_680k.mtx"

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

    # simple, just keep #nnzs of each row and each column separately
    nnz_per_row = np.ediff1d(row_ptr)    

    if(max(nnz_per_row)+1-min(nnz_per_row) < 200):
        nbins_range = range(min(nnz_per_row), max(nnz_per_row)+2,1)
    else:
        nbins_range = 200    
    nnz_r_hist, bin_edges_r = np.histogram(nnz_per_row,bins=nbins_range)

    print("nnz_per_row : min =",min(nnz_per_row), "max =",max(nnz_per_row), "range =",max(nnz_per_row)+1-min(nnz_per_row))
    print("bin_edges_r", bin_edges_r[:-1])
    print("nnz_r_hist", nnz_r_hist)
