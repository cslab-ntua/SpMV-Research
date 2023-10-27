#!/bin/bash


# path_validation='../'
# path_validation2='../new_matrices'
# path_validation="$HOME/Data/graphs/validation_matrices"
# path_validation='/various/pmpakos/SpMV-Research/validation_matrices'

path_validation=/various/pmpakos/SpMV-Research/validation_matrices/small_cross_row_similarity_below_0.4

cores='24'
max_cores=24
export OMP_NUM_THREADS="$cores"
export GOMP_CPU_AFFINITY="0-$((max_cores-1))"


# Encourages idle threads to spin rather than sleep.
export OMP_WAIT_POLICY='active'
# Don't let the runtime deliver fewer threads than those we asked for.
export OMP_DYNAMIC='false'

matrices_validation=(
    # scircuit.mtx
    # mac_econ_fwd500.mtx
    # raefsky3.mtx
    # rgg_n_2_17_s0.mtx
    # bbmat.mtx
    # appu.mtx
    # conf5_4-8x8-15.mtx
    # mc2depi.mtx
    # rma10.mtx
    # cop20k_A.mtx
    # thermomech_dK.mtx
    # webbase-1M.mtx
    # cant.mtx
    # ASIC_680k.mtx
    # roadNet-TX.mtx
    # pdb1HYS.mtx
    # TSOPF_RS_b300_c3.mtx
    # Chebyshev4.mtx
    # consph.mtx
    # com-Youtube.mtx
    # rajat30.mtx
    # radiation.mtx
    # Stanford_Berkeley.mtx
    # shipsec1.mtx
    # PR02R.mtx
    # CurlCurl_2.mtx
    # gupta3.mtx
    # mip1.mtx
    # rail4284.mtx
    # pwtk.mtx
    # crankseg_2.mtx
    # Si41Ge41H72.mtx
    # TSOPF_RS_b2383.mtx
    # in-2004.mtx
    # Ga41As41H72.mtx
    # eu-2005.mtx
    # wikipedia-20051105.mtx
    # kron_g500-logn18.mtx
    # rajat31.mtx
    # human_gene1.mtx
    # delaunay_n22.mtx
    # GL7d20.mtx
    # sx-stackoverflow.mtx
    # dgreen.mtx
    # mawi_201512012345.mtx
    # ldoor.mtx
    # dielFilterV2real.mtx
    # circuit5M.mtx
    # soc-LiveJournal1.mtx
    # bone010.mtx
    # audikw_1.mtx
    # cage15.mtx
    # kmer_V2a.mtx

    # rgg_n_2_17_s0.mtx
    # cop20k_A.mtx
    # roadNet-TX.mtx
    # com-Youtube.mtx
    # shipsec1.mtx
    # pdb1HYS.mtx
    # cant.mtx
    # gupta3.mtx
    # mip1.mtx
    # consph.mtx
    # CurlCurl_2.mtx
    # crankseg_2.mtx
    # pwtk.mtx
    # kron_g500-logn18.mtx
    # delaunay_n22.mtx
    # Si41Ge41H72.mtx
    # Ga41As41H72.mtx
    # human_gene1.mtx
    # bone010.mtx
    # ldoor.mtx
    # dielFilterV2real.mtx
    # audikw_1.mtx
    # circuit5M.mtx


    # rgg_n_2_17_s0_rcm.mtx
    # cop20k_A_rcm.mtx
    # roadNet-TX_rcm.mtx
    # com-Youtube_rcm.mtx
    # shipsec1_rcm.mtx
    # pdb1HYS_rcm.mtx
    # cant_rcm.mtx
    # gupta3_rcm.mtx
    # mip1_rcm.mtx
    # consph_rcm.mtx
    # CurlCurl_2_rcm.mtx
    # crankseg_2_rcm.mtx
    # pwtk_rcm.mtx
    # kron_g500-logn18_rcm.mtx
    # delaunay_n22_rcm.mtx
    # Si41Ge41H72_rcm.mtx
    # Ga41As41H72_rcm.mtx
    # human_gene1_rcm.mtx
    # bone010_rcm.mtx
    # ldoor_rcm.mtx
    # dielFilterV2real_rcm.mtx
    # audikw_1_rcm.mtx
    # circuit5M_rcm.mtx

    GL7d19.mtx
    # GL7d20.mtx
    # GL7d18.mtx
    # GL7d21.mtx
    # GL7d17.mtx
    # GL7d16.mtx
    # rgg_n_2_24_s0.mtx
    # kron_g500-logn21.mtx
    # rgg_n_2_23_s0.mtx
    # patents.mtx
    # cit-Patents.mtx
    # kron_g500-logn20.mtx
    # rgg_n_2_22_s0.mtx
    # kron_g500-logn19.mtx
    # rgg_n_2_21_s0.mtx
    # kron_g500-logn18.mtx
    # rgg_n_2_20_s0.mtx
    # kron_g500-logn17.mtx
    # wikipedia-20070206.mtx
    # wikipedia-20060925.mtx
    # wikipedia-20061104.mtx
    # soc-Pokec.mtx
    # wikipedia-20051105.mtx
    # com-Orkut.mtx
    # kmer_U1a.mtx
    # higgs-twitter.mtx
    # nv2.mtx
    # kmer_P1a.mtx
    # kmer_V2a.mtx
    # sx-stackoverflow.mtx
    # road_central.mtx
    # test1.mtx
    # 12month1.mtx
    # dgreen.mtx
    # mouse_gene.mtx
    # spal_004.mtx
    # GAP-road.mtx
    # road_usa.mtx
    # JP.mtx
    # tp-6.mtx
    # AS365.mtx
    # rail4284.mtx
    # wiki-topcats.mtx
    # M6.mtx
    # com-LiveJournal.mtx
    # NLR.mtx
    # soc-LiveJournal1.mtx
    # human_gene1.mtx
    # relat9.mtx
    # rel9.mtx
    # hugebubbles-00010.mtx
    # human_gene2.mtx
    # flickr.mtx
    # hugebubbles-00020.mtx
    # hugetric-00010.mtx
    # hugetric-00020.mtx
    # ljournal-2008.mtx
    # gsm_106857.mtx
)

matrices=(
    # "$path_validation"/scircuit.mtx
    "${matrices_validation[@]}"
    # "${matrices_validation2[@]}"
)


# for a in "${matrices[@]}"
# do
#     ./mat_feat.exe   $a
# done

# split_matrix=1
split_matrix=0

for matrix in "${matrices[@]}"
do
    plot=0
    store=0
    
    IFS=" " read -ra matrix_parts <<< "$matrix"
    matrix_name="${matrix_parts[0]}"
    # nnz_threshold="${matrix_parts[1]}"
    nnz_threshold=0
    split_matrix=0
    sort_rows=0
    separate=0
    max_distance=10
    shuffle=0

    echo "matrix_name  " $matrix_name
    echo "plot         " $plot
    echo "store        " $store
    echo "nnz_threshold" $nnz_threshold
    echo "split_matrix " $split_matrix
    echo "sort_rows    " $sort_rows
    echo "separate     " $separate
    echo "max_distance " $max_distance
    echo "shuffle      " $shuffle

    ./mat_experiment.exe $path_validation/$matrix_name $plot $store $nnz_threshold $split_matrix $sort_rows $separate $max_distance $shuffle

done


############################################################################################################################################################################################


# export MATRIX_PATH=/various/pmpakos/SpMV-Research2/validation_matrices/generated_matrices
# parameters=(
# #rows   cols     high_nnz row_ratio col_window
# # "2560000 5        16384     256"
# # "25600000 25600000 5        16384     256"
# # "2560000 2560000 5        1024     256"
# # "2560000 2560000 5        8     256"
# )

# # split_matrix=1
# split_matrix=0

# for parameter in "${parameters[@]}"
# do
#     artificial=1

#     IFS=" " read -ra parameter_parts <<< "$parameter"
#     rows="${parameter_parts[0]}"
#     cols="${parameter_parts[1]}"
#     high_nnz="${parameter_parts[2]}"
#     row_ratio="${parameter_parts[3]}"
#     col_window="${parameter_parts[4]}"
    
#     plot=0
#     # plot=1
#     store=0
#     # store=1
    
#     nnz_threshold=0
#     split_matrix=0
#     sort_rows=0
#     separate=0
#     max_distance=10
#     # shuffle=0
#     shuffle=1
    
#     echo "matrix_name  " $matrix_name
#     echo "plot         " $plot
#     echo "store        " $store
#     echo "nnz_threshold" $nnz_threshold
#     echo "split_matrix " $split_matrix
#     echo "sort_rows    " $sort_rows
#     echo "separate     " $separate
#     echo "max_distance " $max_distance
#     echo "shuffle      " $shuffle
#     echo ""
#     echo "artificial   " $artificial
#     echo "rows         " $rows
#     echo "cols         " $cols
#     echo "high_nnz     " $high_nnz
#     echo "row_ratio    " $row_ratio
#     echo "col_window   " $col_window
#     echo ""

#     # for t in 1 2 4 8; do
#     #     export OMP_NUM_THREADS=$t
#     #     export GOMP_CPU_AFFINITY=$(seq -s, 0 $((t-1)))
#     #     echo $GOMP_CPU_AFFINITY
#         ./mat_experiment.exe $MATRIX_PATH/$matrix_name $plot $store $nnz_threshold $split_matrix $sort_rows $separate $max_distance $shuffle $artificial $rows $cols $high_nnz $row_ratio $col_window
#     # done

# done

