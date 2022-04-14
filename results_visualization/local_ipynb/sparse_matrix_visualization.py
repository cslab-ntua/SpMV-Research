# %config Completer.use_jedi = False
# %matplotlib notebook
# %matplotlib inline

import numpy as np
import math
import time
import matplotlib.pyplot as plt
from matplotlib.pyplot import gcf

from scipy.io import mmread

# from scipy.sparse import csr_matrix
# from scipy.sparse import coo_matrix
# from scipy.sparse.csgraph import reverse_cuthill_mckee

from PIL import Image

pic_dim = pic_dim_x = pic_dim_y = 512

def density_RGB(den, dtx, dty):
    temp1 = 1.0 * den / ( dtx * dty) * (256**3)
    if temp1 == float("inf"):
        print('Infinity again')
        return (-1,-1,-1)
    else:
        temp = int(temp1)
        return ( temp / pic_dim**2 , (temp % pic_dim**2) / 256, (temp % pic_dim**2) % 256)

def datum_mapping(A):
    dim_x = A.shape[0]
    dim_y = A.shape[1]
    if dim_x != dim_y:
        print( 'Uneven array' )
        return []
    datum_x = dim_x // (pic_dim - 1)
    datum_y = dim_y // (pic_dim - 1)
    last_x = dim_x - datum_x * (pic_dim - 1)
    last_y = dim_y - datum_y * (pic_dim - 1)
    # print ( 'last_x = ' + str(last_x) + ' last_y = ' + str(last_y))
    # print ( 'Array of shape ('  + str(dim_x) + ',' + str(dim_y) + ') with nnz= ' + str(len(A.row)) +  ' has datum (' + str(datum_x) + ',' + str(datum_y) + ')')
    if datum_x==0 or datum_y==0:
        # print( '0 datum, terminating...')
        return []
    datum_density = np.zeros((pic_dim,pic_dim),dtype=np.int8)
    timer = time.clock()
    for col,row in zip (A.col, A.row) :
        dx  = min(row // datum_x, pic_dim - 1)    
        dy  = min(col // datum_x, pic_dim - 1)                    
        datum_density[dx][dy] = datum_density[dx][dy] + 1

    timer = time.clock() - timer
    # print ( "COO split time: " + str(timer))
    pic = np.zeros((pic_dim,pic_dim, 3), dtype=np.uint8)
    for i in range(0,(pic_dim - 1)):
        for j in range(0,(pic_dim - 1)):
            (pic[i][j][0], pic[i][j][1], pic[i][j][2]) = density_RGB(datum_density[i][j], datum_x, datum_y)
    if last_x != 0 and last_y !=0 :
        (pic[(pic_dim - 1)][(pic_dim - 1)][0], pic[(pic_dim - 1)][(pic_dim - 1)][1], pic[(pic_dim - 1)][(pic_dim - 1)][2]) = density_RGB(datum_density[(pic_dim - 1)][(pic_dim - 1)], last_x, last_y)
    return pic

def visualize_matrix_panastas(spm_coo, img_filename):
    pic = datum_mapping(spm_coo)
    img2 = Image.fromarray(pic, 'RGB')
    img2.save("/various/pmpakos/validation_matrices/panastas_images/"+img_filename)


############################################################################################################################################################################################################################
def mmread_fun(filename):
    start = time.time()
    ###################################################################
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
    ###################################################################
    end = time.time()
    print(">>> Took me", round(end-start,3), "seconds to read matrix")
    return spm_coo, spm

max_dim = 1024
max_ratio = 16
# old - my way
def visualize_matrix(spm_coo, img_filename):
    start = time.time()
    ###################################################################
    rows = spm_coo.get_shape()[0]
    cols = spm_coo.get_shape()[1]
    if(rows==0 or cols==0):
        ratio = 1
    else:
        ratio = rows/cols
        if(ratio>max_ratio):
            ratio = max_ratio # to be able to plot something that is barely visible, unfortunately can't do something for this!
        if(ratio<(1/max_ratio)):
            ratio = 1/max_ratio # to be able to plot something that is barely visible, unfortunately can't do something for this!
    if(ratio>1):
        v_dim_x = max_dim
        v_dim_y = math.ceil(v_dim_x/ratio)        
    else:
        v_dim_y = max_dim
        v_dim_x = math.ceil(v_dim_y*ratio)
    
    visual = np.zeros((v_dim_x, v_dim_y))     # perhaps add 3 dimensions in the future, for density of each pixel   
    factor_r = math.ceil(rows/v_dim_x)
    factor_c = math.ceil(cols/v_dim_y)
    for i in range(len(spm_coo.data)):
        row = spm_coo.row[i]
        col = spm_coo.col[i]
        val = spm_coo.data[i]
        v_row = math.floor(row/factor_r)
        v_col = math.floor(col/factor_c)  
        visual[v_row][v_col] += 1
        # print(row, '->', v_row, '\t\t', col, '->' , v_col)
        # if(i%100000 == 0):
        #     print(i)
        #     img = Image.fromarray(visual*1.0/visual.max(),'RGB')
        #     img.show()
    ###################################################################
    end = time.time()
    print(">>> Took me", round(end-start,3), "seconds to create image of matrix")
    start = time.time()
    ###################################################################
    fig = gcf()
    fig.set_size_inches(64, 64)
    plt.spy(visual[:,:], markersize=1)
    
    locs, labels = plt.xticks()
    labels = [int(item*factor_c) for item in locs[1:-1]]
    plt.xticks(locs[1:-1], labels)
    locs, labels = plt.yticks()
    labels = [int(item*factor_r) for item in locs[1:-1]]
    plt.yticks(locs[1:-1], labels)

    plt.title(img_filename)
    plt.show()
    # fig.savefig("generated_matrices/images/"+img_filename, dpi=100, bbox_inches='tight')
    fig.savefig("/various/pmpakos/SpMV-Research/validation_matrices/v2_dgal/images/"+img_filename, dpi=100, bbox_inches='tight')
    
    ###################################################################
    end = time.time()
    print(">>> Took me", round(end-start,3), "seconds to plot and save image of matrix\n\n")
    
# # do it with Octave, to save new matrix too - DEPRECATED
# def reverse_cuthill_mckee_conversion(spm_coo, spm):
#     rcm_perm = reverse_cuthill_mckee(csr_matrix(spm))
#     rev_perm_dict = {k : rcm_perm.tolist().index(k) for k in rcm_perm}
#     perm_i = [rev_perm_dict[ii] for ii in spm_coo.row]
#     perm_j = [rev_perm_dict[jj] for jj in spm_coo.col]
#     spm_coo_rcm = coo_matrix((spm_coo.data, (perm_i, perm_j)))
#     return spm_coo_rcm








############################################################################################################################################################################################################################

if __name__ == '__main__':
    ############################################################################################################################################################################################################################
    # '''
    # # digital twins now (first, real matrices)
    # '''

    # filenames = [
    # "/various/pmpakos/validation_matrices/scircuit.mtx",
    # "/various/pmpakos/validation_matrices/mac_econ_fwd500.mtx",
    # "/various/pmpakos/validation_matrices/raefsky3.mtx",
    # "/various/pmpakos/validation_matrices/bbmat.mtx",
    # "/various/pmpakos/validation_matrices/conf5_4-8x8-15.mtx",
    # "/various/pmpakos/validation_matrices/mc2depi.mtx",
    # "/various/pmpakos/validation_matrices/rma10.mtx",
    # "/various/pmpakos/validation_matrices/cop20k_A.mtx",
    # "/various/pmpakos/validation_matrices/webbase-1M.mtx",
    # "/various/pmpakos/validation_matrices/cant.mtx",
    # "/various/pmpakos/validation_matrices/pdb1HYS.mtx",
    # "/various/pmpakos/validation_matrices/TSOPF_RS_b300_c3.mtx",
    # "/various/pmpakos/validation_matrices/Chebyshev4.mtx",
    # "/various/pmpakos/validation_matrices/consph.mtx",
    # "/various/pmpakos/validation_matrices/shipsec1.mtx",
    # "/various/pmpakos/validation_matrices/PR02R.mtx",
    # "/various/pmpakos/validation_matrices/mip1.mtx",
    # "/various/pmpakos/validation_matrices/rail4284.mtx",
    # "/various/pmpakos/validation_matrices/pwtk.mtx",
    # "/various/pmpakos/validation_matrices/crankseg_2.mtx",
    # "/various/pmpakos/validation_matrices/Si41Ge41H72.mtx",
    # "/various/pmpakos/validation_matrices/TSOPF_RS_b2383.mtx",
    # "/various/pmpakos/validation_matrices/in-2004.mtx",
    # "/various/pmpakos/validation_matrices/Ga41As41H72.mtx",
    # "/various/pmpakos/validation_matrices/eu-2005.mtx",
    # "/various/pmpakos/validation_matrices/wikipedia-20051105.mtx",
    # "/various/pmpakos/validation_matrices/rajat31.mtx",
    # "/various/pmpakos/validation_matrices/ldoor.mtx",
    # "/various/pmpakos/validation_matrices/circuit5M.mtx",
    # "/various/pmpakos/validation_matrices/bone010.mtx",
    # "/various/pmpakos/validation_matrices/cage15.mtx",
    # ]

    # for filename in filenames:
    #     spm_coo, spm = mmread_fun(filename)
    #     img_filename = filename.split("/")[-1].replace(".mtx",".png")
    # visualize_matrix(spm_coo, img_filename)


    ############################################################################################################################################################################################################################
    # filenames = [
    # "/various/pmpakos/validation_matrices/digital_twins/artificial_normal_distribution_scircuit_170998_170998_950086_diagonal_df0.005_seed_13.mtx",
    # "/various/pmpakos/validation_matrices/digital_twins/artificial_normal_distribution_raefsky3_21200_21200_1477938_diagonal_df0.005_seed_0.mtx",
    # "/various/pmpakos/validation_matrices/digital_twins/artificial_gamma_distribution_bbmat_38744_38744_1758250_diagonal_df0.005_seed_1.mtx",
    # "/various/pmpakos/validation_matrices/digital_twins/artificial_normal_distribution_conf5_4-8x8-15_49152_49152_1916928_diagonal_df0.005_seed_0.mtx",
    # "/various/pmpakos/validation_matrices/digital_twins/artificial_normal_distribution_rma10_46835_46835_2383899_diagonal_df0.005_seed_0.mtx",
    # "/various/pmpakos/validation_matrices/digital_twins/artificial_normal_distribution_cop20k_A_121192_121192_2643139_diagonal_df0.005_seed_2.mtx",
    # "/various/pmpakos/validation_matrices/digital_twins/artificial_gamma_distribution_webbase-1M_1000005_1000005_3079841_diagonal_df0.005_seed_2.mtx",
    # "/various/pmpakos/validation_matrices/digital_twins/artificial_normal_distribution_cant_62451_62451_3973019_diagonal_df0.005_seed_0.mtx",
    # "/various/pmpakos/validation_matrices/digital_twins/artificial_normal_distribution_pdb1HYS_36417_36417_4321056_diagonal_df0.005_seed_0.mtx",
    # "/various/pmpakos/validation_matrices/digital_twins/artificial_gamma_distribution_TSOPF_RS_b300_c3_42138_42138_4393966_diagonal_df0.005_seed_1.mtx",
    # "/various/pmpakos/validation_matrices/digital_twins/artificial_gamma_distribution_Chebyshev4_68121_68121_5391103_diagonal_df0.005_seed_3.mtx",
    # "/various/pmpakos/validation_matrices/digital_twins/artificial_normal_distribution_consph_83334_83334_5965043_diagonal_df0.005_seed_0.mtx",
    # "/various/pmpakos/validation_matrices/digital_twins/artificial_normal_distribution_shipsec1_140874_140874_7749299_diagonal_df0.005_seed_0.mtx",
    # "/various/pmpakos/validation_matrices/digital_twins/artificial_normal_distribution_PR02R_161070_161070_8130431_diagonal_df0.005_seed_0.mtx",
    # "/various/pmpakos/validation_matrices/digital_twins/artificial_gamma_distribution_mip1_66463_66463_10386831_diagonal_df0.005_seed_0.mtx",
    # "/various/pmpakos/validation_matrices/digital_twins/artificial_gamma_distribution_rail4284_4284_1096894_11294973_diagonal_df0.005_seed_4.mtx",
    # "/various/pmpakos/validation_matrices/digital_twins/artificial_normal_distribution_pwtk_217918_217918_11528534_diagonal_df0.005_seed_0.mtx",
    # "/various/pmpakos/validation_matrices/digital_twins/artificial_normal_distribution_crankseg_2_63838_63838_14139678_diagonal_df0.005_seed_0.mtx",
    # "/various/pmpakos/validation_matrices/digital_twins/artificial_gamma_distribution_Si41Ge41H72_185639_185639_14945031_diagonal_df0.005_seed_0.mtx",
    # "/various/pmpakos/validation_matrices/digital_twins/artificial_gamma_distribution_TSOPF_RS_b2383_38120_38120_16109148_diagonal_df0.005_seed_0.mtx",
    # "/various/pmpakos/validation_matrices/digital_twins/artificial_gamma_distribution_Ga41As41H72_268096_268096_18358107_diagonal_df0.005_seed_0.mtx",
    # "/various/pmpakos/validation_matrices/digital_twins/artificial_gamma_distribution_ldoor_952203_952203_46080696_diagonal_df0.005_seed_53.mtx",
    # "/various/pmpakos/validation_matrices/digital_twins/artificial_normal_distribution_bone010_986703_986703_71196490_diagonal_df0.005_seed_0.mtx",
    # ]

    # for filename in filenames:
    #     spm_coo, spm = mmread_fun(filename)
    #     img_filename = filename.split("/")[-1].replace(".mtx",".png")
    #     visualize_matrix(spm_coo, img_filename)

    ############################################################################################################################################################################################################################
    # filenames = [
    # "/various/pmpakos/validation_matrices/digital_twins/artificial_normal_distribution_scircuit_170998_170998_950086_diagonal_df0.5_seed_13.mtx",
    # "/various/pmpakos/validation_matrices/digital_twins/artificial_normal_distribution_raefsky3_21200_21200_1477938_diagonal_df0.5_seed_0.mtx",
    # "/various/pmpakos/validation_matrices/digital_twins/artificial_gamma_distribution_bbmat_38744_38744_1758250_diagonal_df0.5_seed_1.mtx",
    # "/various/pmpakos/validation_matrices/digital_twins/artificial_normal_distribution_conf5_4-8x8-15_49152_49152_1916928_diagonal_df0.5_seed_0.mtx",
    # "/various/pmpakos/validation_matrices/digital_twins/artificial_normal_distribution_rma10_46835_46835_2383899_diagonal_df0.5_seed_0.mtx",
    # "/various/pmpakos/validation_matrices/digital_twins/artificial_normal_distribution_cop20k_A_121192_121192_2643139_diagonal_df0.5_seed_2.mtx",
    # "/various/pmpakos/validation_matrices/digital_twins/artificial_gamma_distribution_webbase-1M_1000005_1000005_3079841_diagonal_df0.5_seed_2.mtx",
    # "/various/pmpakos/validation_matrices/digital_twins/artificial_normal_distribution_cant_62451_62451_3973019_diagonal_df0.5_seed_0.mtx",
    # "/various/pmpakos/validation_matrices/digital_twins/artificial_normal_distribution_pdb1HYS_36417_36417_4321056_diagonal_df0.5_seed_0.mtx",
    # "/various/pmpakos/validation_matrices/digital_twins/artificial_gamma_distribution_TSOPF_RS_b300_c3_42138_42138_4393966_diagonal_df0.5_seed_1.mtx",
    # "/various/pmpakos/validation_matrices/digital_twins/artificial_gamma_distribution_Chebyshev4_68121_68121_5391103_diagonal_df0.5_seed_3.mtx",
    # "/various/pmpakos/validation_matrices/digital_twins/artificial_normal_distribution_consph_83334_83334_5965043_diagonal_df0.5_seed_0.mtx",
    # "/various/pmpakos/validation_matrices/digital_twins/artificial_normal_distribution_shipsec1_140874_140874_7749299_diagonal_df0.5_seed_0.mtx",
    # "/various/pmpakos/validation_matrices/digital_twins/artificial_normal_distribution_PR02R_161070_161070_8130431_diagonal_df0.5_seed_0.mtx",
    # "/various/pmpakos/validation_matrices/digital_twins/artificial_gamma_distribution_mip1_66463_66463_10386831_diagonal_df0.5_seed_0.mtx",
    # "/various/pmpakos/validation_matrices/digital_twins/artificial_gamma_distribution_rail4284_4284_1096894_11294973_diagonal_df0.5_seed_4.mtx",
    # "/various/pmpakos/validation_matrices/digital_twins/artificial_normal_distribution_pwtk_217918_217918_11528534_diagonal_df0.5_seed_0.mtx",
    # "/various/pmpakos/validation_matrices/digital_twins/artificial_normal_distribution_crankseg_2_63838_63838_14139678_diagonal_df0.5_seed_0.mtx",
    # "/various/pmpakos/validation_matrices/digital_twins/artificial_gamma_distribution_Si41Ge41H72_185639_185639_14945031_diagonal_df0.5_seed_0.mtx",
    # "/various/pmpakos/validation_matrices/digital_twins/artificial_gamma_distribution_TSOPF_RS_b2383_38120_38120_16109148_diagonal_df0.5_seed_0.mtx",
    # "/various/pmpakos/validation_matrices/digital_twins/artificial_gamma_distribution_Ga41As41H72_268096_268096_18358107_diagonal_df0.5_seed_0.mtx",
    # "/various/pmpakos/validation_matrices/digital_twins/artificial_gamma_distribution_ldoor_952203_952203_46080696_diagonal_df0.5_seed_53.mtx",
    # "/various/pmpakos/validation_matrices/digital_twins/artificial_normal_distribution_bone010_986703_986703_71196490_diagonal_df0.5_seed_0.mtx"
    # ]

    # for filename in filenames:
    #     spm_coo, spm = mmread_fun(filename)
    #     img_filename = filename.split("/")[-1].replace(".mtx",".png")
    #     visualize_matrix(spm_coo, img_filename)

    ############################################################################################################################################################################################################################
    # filenames = [
    # "/various/pmpakos/validation_matrices/digital_twins/artificial_normal_distribution_scircuit_170998_170998_950086_random_seed_13.mtx",
    # "/various/pmpakos/validation_matrices/digital_twins/artificial_normal_distribution_raefsky3_21200_21200_1477938_random_seed_0.mtx",
    # "/various/pmpakos/validation_matrices/digital_twins/artificial_gamma_distribution_bbmat_38744_38744_1758250_random_seed_1.mtx",
    # "/various/pmpakos/validation_matrices/digital_twins/artificial_normal_distribution_conf5_4-8x8-15_49152_49152_1916928_random_seed_0.mtx",
    # "/various/pmpakos/validation_matrices/digital_twins/artificial_normal_distribution_rma10_46835_46835_2383899_random_seed_0.mtx",
    # "/various/pmpakos/validation_matrices/digital_twins/artificial_normal_distribution_cop20k_A_121192_121192_2643139_random_seed_2.mtx",
    # "/various/pmpakos/validation_matrices/digital_twins/artificial_gamma_distribution_webbase-1M_1000005_1000005_3079841_random_seed_2.mtx",
    # "/various/pmpakos/validation_matrices/digital_twins/artificial_normal_distribution_cant_62451_62451_3973019_random_seed_0.mtx",
    # "/various/pmpakos/validation_matrices/digital_twins/artificial_normal_distribution_pdb1HYS_36417_36417_4321056_random_seed_0.mtx",
    # "/various/pmpakos/validation_matrices/digital_twins/artificial_gamma_distribution_TSOPF_RS_b300_c3_42138_42138_4393966_random_seed_1.mtx",
    # "/various/pmpakos/validation_matrices/digital_twins/artificial_gamma_distribution_Chebyshev4_68121_68121_5391103_random_seed_3.mtx",
    # "/various/pmpakos/validation_matrices/digital_twins/artificial_normal_distribution_consph_83334_83334_5965043_random_seed_0.mtx",
    # "/various/pmpakos/validation_matrices/digital_twins/artificial_normal_distribution_shipsec1_140874_140874_7749299_random_seed_0.mtx",
    # "/various/pmpakos/validation_matrices/digital_twins/artificial_normal_distribution_PR02R_161070_161070_8130431_random_seed_0.mtx",
    # "/various/pmpakos/validation_matrices/digital_twins/artificial_gamma_distribution_mip1_66463_66463_10386831_random_seed_0.mtx",
    # "/various/pmpakos/validation_matrices/digital_twins/artificial_gamma_distribution_rail4284_4284_1096894_11294973_random_seed_4.mtx",
    # "/various/pmpakos/validation_matrices/digital_twins/artificial_normal_distribution_pwtk_217918_217918_11528534_random_seed_0.mtx",
    # "/various/pmpakos/validation_matrices/digital_twins/artificial_normal_distribution_crankseg_2_63838_63838_14139678_random_seed_0.mtx",
    # "/various/pmpakos/validation_matrices/digital_twins/artificial_gamma_distribution_Si41Ge41H72_185639_185639_14945031_random_seed_0.mtx",
    # "/various/pmpakos/validation_matrices/digital_twins/artificial_gamma_distribution_TSOPF_RS_b2383_38120_38120_16109148_random_seed_0.mtx",
    # "/various/pmpakos/validation_matrices/digital_twins/artificial_gamma_distribution_Ga41As41H72_268096_268096_18358107_random_seed_0.mtx",
    # "/various/pmpakos/validation_matrices/digital_twins/artificial_gamma_distribution_ldoor_952203_952203_46080696_random_seed_53.mtx",
    # "/various/pmpakos/validation_matrices/digital_twins/artificial_normal_distribution_bone010_986703_986703_71196490_random_seed_0.mtx",
    # ]

    # for filename in filenames:
    #     spm_coo, spm = mmread_fun(filename)
    #     img_filename = filename.split("/")[-1].replace(".mtx",".png")
    #     visualize_matrix(spm_coo, img_filename)

    ############################################################################################################################################################################################################################
    # filenames = [
    # "/various/pmpakos/athena_ppopp_matrices/filtered/Ronis/xenon2.mtx",
    # "/various/pmpakos/athena_ppopp_matrices/filtered/Ronis/xenon2_rcm.mtx",
        
    # "/various/pmpakos/athena_ppopp_matrices/filtered/PARSEC/Si41Ge41H72.mtx",
    # "/various/pmpakos/athena_ppopp_matrices/filtered/PARSEC/Si41Ge41H72_rcm.mtx",
        
    # "/various/pmpakos/athena_ppopp_matrices/filtered/Dziekonski/dielFilterV3real.mtx",
    # "/various/pmpakos/athena_ppopp_matrices/filtered/Dziekonski/dielFilterV3real_rcm.mtx",
        
    # "/various/pmpakos/athena_ppopp_matrices/filtered/Botonakis/thermomech_dK.mtx",
    # "/various/pmpakos/athena_ppopp_matrices/filtered/Botonakis/thermomech_dK_rcm.mtx",

    # "/various/pmpakos/athena_ppopp_matrices/filtered/Gleich/wikipedia-20051105.mtx",
    # "/various/pmpakos/athena_ppopp_matrices/filtered/Gleich/wikipedia-20051105_rcm.mtx",
        
    # "/various/pmpakos/athena_ppopp_matrices/filtered/SNAP/soc-LiveJournal1.mtx",
    # "/various/pmpakos/athena_ppopp_matrices/filtered/SNAP/soc-LiveJournal1_rcm.mtx",
        
    # "/various/pmpakos/athena_ppopp_matrices/filtered/Sandia/ASIC_680k.mtx",
    # "/various/pmpakos/athena_ppopp_matrices/filtered/Sandia/ASIC_680k_rcm.mtx",
        
    # "/various/pmpakos/athena_ppopp_matrices/filtered/LAW/in-2004.mtx",
    # "/various/pmpakos/athena_ppopp_matrices/filtered/LAW/in-2004_rcm.mtx",
        
    # "/various/pmpakos/athena_ppopp_matrices/filtered/Freescale/circuit5M.mtx",
    # "/various/pmpakos/athena_ppopp_matrices/filtered/Freescale/circuit5M_rcm.mtx",
        
    # "/various/pmpakos/exafoam_matrices/100K.mtx",
    # "/various/pmpakos/exafoam_matrices/100K_rcm.mtx",
        
    # "/various/pmpakos/exafoam_matrices/600K.mtx",
    # "/various/pmpakos/exafoam_matrices/600K_rcm.mtx"    
    # ]

    # for filename in filenames:
    #     break
    #     spm_coo, spm = mmread_fun(filename)
    #     img_filename = filename.split("/")[-1].replace(".mtx",".png")
    #     visualize_matrix(spm_coo, img_filename)

    # #     no longer needed ... 
    # #     spm_coo_rcm=reverse_cuthill_mckee_conversion(spm_coo, spm)
    # #     img_filename = filename.split("/")[-1].replace(".mtx","_rcm.png")
    # #     visualize_matrix(spm_coo_rcm, img_filename)

    ############################################################################################################################################################################################################################
    # filenames = [
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

    # for filename in filenames:
    #     break
    #     spm_coo, spm = mmread_fun(filename)
    #     img_filename = filename.split("/")[-1].replace(".mtx",".png")
    #     visualize_matrix(spm_coo, img_filename)

    ############################################################################################################################################################################################################################
    # filenames = [
    # "/home/pmpakos/generated_matrices/synthetic_65536_65536_2407832_mu_32_std_32_random_seed_14.mtx",
    # "/home/pmpakos/generated_matrices/synthetic_65536_65536_2081984_mu_32_std_16_random_seed_14.mtx",
    # "/home/pmpakos/generated_matrices/synthetic_65536_65536_2064131_mu_32_std_8_random_seed_14.mtx",
    # "/home/pmpakos/generated_matrices/synthetic_65536_65536_1839685_mu_16_std_32_random_seed_14.mtx",
    # "/home/pmpakos/generated_matrices/synthetic_65536_65536_1187706_mu_16_std_16_random_seed_14.mtx",
    # "/home/pmpakos/generated_matrices/synthetic_65536_65536_1024643_mu_16_std_8_random_seed_14.mtx",
    # "/home/pmpakos/generated_matrices/synthetic_65536_65536_1688547_mu_8_std_32_random_seed_14.mtx",
    # "/home/pmpakos/generated_matrices/synthetic_65536_65536_903701_mu_8_std_16_random_seed_14.mtx",
    # "/home/pmpakos/generated_matrices/synthetic_65536_65536_577663_mu_8_std_8_random_seed_14.mtx",
    # ]

    # for filename in filenames:
    #     break
    #     spm_coo, spm = mmread_fun(filename)
    #     img_filename = filename.split("/")[-1].replace(".mtx",".png")
    #     visualize_matrix(spm_coo, img_filename)

    ############################################################################################################################################################################################################################
    # filenames = [
    # # "/home/pmpakos/generated_matrices/synthetic_65536_65536_2407832_mu_32_std_32_diagonal_df0.0005_seed_14.mtx",
    # # "/home/pmpakos/generated_matrices/synthetic_65536_65536_2407832_mu_32_std_32_diagonal_df0.005_seed_14.mtx",
    # # "/home/pmpakos/generated_matrices/synthetic_65536_65536_2407832_mu_32_std_32_diagonal_df0.05_seed_14.mtx",
    # # "/home/pmpakos/generated_matrices/synthetic_65536_65536_2407832_mu_32_std_32_diagonal_df0.5_seed_14.mtx",
    # # "/home/pmpakos/generated_matrices/synthetic_65536_65536_2081984_mu_32_std_16_diagonal_df0.0005_seed_14.mtx",
    # # "/home/pmpakos/generated_matrices/synthetic_65536_65536_2081984_mu_32_std_16_diagonal_df0.005_seed_14.mtx",
    # # "/home/pmpakos/generated_matrices/synthetic_65536_65536_2081984_mu_32_std_16_diagonal_df0.05_seed_14.mtx",
    # # "/home/pmpakos/generated_matrices/synthetic_65536_65536_2081984_mu_32_std_16_diagonal_df0.5_seed_14.mtx",
    # # "/home/pmpakos/generated_matrices/synthetic_65536_65536_2064131_mu_32_std_8_diagonal_df0.0005_seed_14.mtx",
    # # "/home/pmpakos/generated_matrices/synthetic_65536_65536_2064131_mu_32_std_8_diagonal_df0.005_seed_14.mtx",
    # # "/home/pmpakos/generated_matrices/synthetic_65536_65536_2064131_mu_32_std_8_diagonal_df0.05_seed_14.mtx",
    # # "/home/pmpakos/generated_matrices/synthetic_65536_65536_2064131_mu_32_std_8_diagonal_df0.5_seed_14.mtx",
    # # "/home/pmpakos/generated_matrices/synthetic_65536_65536_1839685_mu_16_std_32_diagonal_df0.0005_seed_14.mtx",
    # # "/home/pmpakos/generated_matrices/synthetic_65536_65536_1839685_mu_16_std_32_diagonal_df0.005_seed_14.mtx",
    # # "/home/pmpakos/generated_matrices/synthetic_65536_65536_1839685_mu_16_std_32_diagonal_df0.05_seed_14.mtx",
    # # "/home/pmpakos/generated_matrices/synthetic_65536_65536_1839685_mu_16_std_32_diagonal_df0.5_seed_14.mtx",
    # # "/home/pmpakos/generated_matrices/synthetic_65536_65536_1187706_mu_16_std_16_diagonal_df0.0005_seed_14.mtx",
    # # "/home/pmpakos/generated_matrices/synthetic_65536_65536_1187706_mu_16_std_16_diagonal_df0.005_seed_14.mtx",
    # # "/home/pmpakos/generated_matrices/synthetic_65536_65536_1187706_mu_16_std_16_diagonal_df0.05_seed_14.mtx",
    # # "/home/pmpakos/generated_matrices/synthetic_65536_65536_1187706_mu_16_std_16_diagonal_df0.5_seed_14.mtx",
    # # "/home/pmpakos/generated_matrices/synthetic_65536_65536_1024643_mu_16_std_8_diagonal_df0.0005_seed_14.mtx",
    # # "/home/pmpakos/generated_matrices/synthetic_65536_65536_1024643_mu_16_std_8_diagonal_df0.005_seed_14.mtx",
    # # "/home/pmpakos/generated_matrices/synthetic_65536_65536_1024643_mu_16_std_8_diagonal_df0.05_seed_14.mtx",
    # # "/home/pmpakos/generated_matrices/synthetic_65536_65536_1024643_mu_16_std_8_diagonal_df0.5_seed_14.mtx",
    # # "/home/pmpakos/generated_matrices/synthetic_65536_65536_1688547_mu_8_std_32_diagonal_df0.0005_seed_14.mtx",
    # # "/home/pmpakos/generated_matrices/synthetic_65536_65536_1688547_mu_8_std_32_diagonal_df0.005_seed_14.mtx",
    # # "/home/pmpakos/generated_matrices/synthetic_65536_65536_1688547_mu_8_std_32_diagonal_df0.05_seed_14.mtx",
    # # "/home/pmpakos/generated_matrices/synthetic_65536_65536_1688547_mu_8_std_32_diagonal_df0.5_seed_14.mtx",
    # # "/home/pmpakos/generated_matrices/synthetic_65536_65536_903701_mu_8_std_16_diagonal_df0.0005_seed_14.mtx",
    # # "/home/pmpakos/generated_matrices/synthetic_65536_65536_903701_mu_8_std_16_diagonal_df0.005_seed_14.mtx",
    # "/home/pmpakos/generated_matrices/synthetic_65536_65536_903701_mu_8_std_16_diagonal_df0.05_seed_14.mtx",
    # "/home/pmpakos/generated_matrices/synthetic_65536_65536_903701_mu_8_std_16_diagonal_df0.5_seed_14.mtx",
    # "/home/pmpakos/generated_matrices/synthetic_65536_65536_577663_mu_8_std_8_diagonal_df0.0005_seed_14.mtx",
    # "/home/pmpakos/generated_matrices/synthetic_65536_65536_577663_mu_8_std_8_diagonal_df0.005_seed_14.mtx",
    # "/home/pmpakos/generated_matrices/synthetic_65536_65536_577663_mu_8_std_8_diagonal_df0.05_seed_14.mtx",
    # "/home/pmpakos/generated_matrices/synthetic_65536_65536_577663_mu_8_std_8_diagonal_df0.5_seed_14.mtx"
    # ]

    # for filename in filenames:
    #     spm_coo, spm = mmread_fun(filename)
    #     img_filename = filename.split("/")[-1].replace(".mtx",".png")
    #     visualize_matrix(spm_coo, img_filename)

    ############################################################################################################################################################################################################################
    # filenames = [
    # "/various/pmpakos/validation_matrices/scircuit.mtx",
    # "/various/pmpakos/validation_matrices/mac_econ_fwd500.mtx",
    # "/various/pmpakos/validation_matrices/raefsky3.mtx",
    # "/various/pmpakos/validation_matrices/bbmat.mtx",
    # "/various/pmpakos/validation_matrices/conf5_4-8x8-15.mtx",
    # "/various/pmpakos/validation_matrices/mc2depi.mtx",
    # "/various/pmpakos/validation_matrices/rma10.mtx",
    # "/various/pmpakos/validation_matrices/cop20k_A.mtx",
    # "/various/pmpakos/validation_matrices/webbase-1M.mtx",
    # "/various/pmpakos/validation_matrices/cant.mtx",
    # "/various/pmpakos/validation_matrices/pdb1HYS.mtx",
    # "/various/pmpakos/validation_matrices/TSOPF_RS_b300_c3.mtx",
    # "/various/pmpakos/validation_matrices/Chebyshev4.mtx",
    # "/various/pmpakos/validation_matrices/consph.mtx",
    # "/various/pmpakos/validation_matrices/shipsec1.mtx",
    # "/various/pmpakos/validation_matrices/PR02R.mtx",
    # "/various/pmpakos/validation_matrices/mip1.mtx",
    # # "/various/pmpakos/validation_matrices/rail4284.mtx",
    # "/various/pmpakos/validation_matrices/pwtk.mtx",
    # "/various/pmpakos/validation_matrices/crankseg_2.mtx",
    # "/various/pmpakos/validation_matrices/Si41Ge41H72.mtx",
    # "/various/pmpakos/validation_matrices/TSOPF_RS_b2383.mtx",
    # "/various/pmpakos/validation_matrices/in-2004.mtx",
    # "/various/pmpakos/validation_matrices/Ga41As41H72.mtx",
    # "/various/pmpakos/validation_matrices/eu-2005.mtx",
    # "/various/pmpakos/validation_matrices/wikipedia-20051105.mtx",
    # "/various/pmpakos/validation_matrices/rajat31.mtx",
    # "/various/pmpakos/validation_matrices/ldoor.mtx",
    # "/various/pmpakos/validation_matrices/circuit5M.mtx",
    # "/various/pmpakos/validation_matrices/bone010.mtx",
    # "/various/pmpakos/validation_matrices/cage15.mtx",
    # ]

    # for filename in filenames:
    #     spm_coo, spm = mmread_fun(filename)
    #     img_filename = filename.split("/")[-1].replace(".mtx",".png")
    #     visualize_matrix_panastas(spm_coo, img_filename)


    # filenames = [
    # "/various/pmpakos/validation_matrices/pwtk.mtx",
    # "/various/pmpakos/validation_matrices/crankseg_2.mtx",
    # "/various/pmpakos/validation_matrices/Si41Ge41H72.mtx",
    # "/various/pmpakos/validation_matrices/TSOPF_RS_b2383.mtx",
    # "/various/pmpakos/validation_matrices/in-2004.mtx",
    # "/various/pmpakos/validation_matrices/Ga41As41H72.mtx",
    # "/various/pmpakos/validation_matrices/eu-2005.mtx",
    # "/various/pmpakos/validation_matrices/wikipedia-20051105.mtx",
    # "/various/pmpakos/validation_matrices/rajat31.mtx",
    # "/various/pmpakos/validation_matrices/ldoor.mtx",
    # "/various/pmpakos/validation_matrices/circuit5M.mtx",
    # "/various/pmpakos/validation_matrices/bone010.mtx",
    # "/various/pmpakos/validation_matrices/cage15.mtx",
    # ]

    # for filename in filenames:
    #     spm_coo, spm = mmread_fun(filename)
    #     img_filename = filename.split("/")[-1].replace(".mtx",".png")
    #     visualize_matrix_panastas(spm_coo, img_filename)


    ############################################################################################################################################################################################################################
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

    for filename in filenames:
        spm_coo, spm = mmread_fun(filename)
        img_filename = filename.split("/")[-1].replace(".mtx",".png")
        visualize_matrix(spm_coo, img_filename)
