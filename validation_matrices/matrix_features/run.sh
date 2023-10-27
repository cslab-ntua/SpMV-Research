#!/bin/bash


path_validation='../'
path_validation2='../new_matrices'
path_validation="$HOME/Data/graphs/validation_matrices"
path_validation='/various/pmpakos/SpMV-Research/validation_matrices'

cores='24'
max_cores=24
export OMP_NUM_THREADS="$cores"
export GOMP_CPU_AFFINITY="0-$((max_cores-1))"


# Encourages idle threads to spin rather than sleep.
export OMP_WAIT_POLICY='active'
# Don't let the runtime deliver fewer threads than those we asked for.
export OMP_DYNAMIC='false'

matrices_validation=(

    "$path_validation"/scircuit.mtx
    # "$path_validation"/mac_econ_fwd500.mtx
    # "$path_validation"/raefsky3.mtx
    # "$path_validation"/bbmat.mtx
    # "$path_validation"/conf5_4-8x8-15.mtx
    # "$path_validation"/mc2depi.mtx
    # "$path_validation"/rma10.mtx
    # "$path_validation"/cop20k_A.mtx
    # "$path_validation"/webbase-1M.mtx
    # "$path_validation"/cant.mtx
    # "$path_validation"/pdb1HYS.mtx
    # "$path_validation"/TSOPF_RS_b300_c3.mtx
    # "$path_validation"/Chebyshev4.mtx
    # "$path_validation"/consph.mtx
    # "$path_validation"/shipsec1.mtx
    # "$path_validation"/PR02R.mtx
    # "$path_validation"/mip1.mtx
    # "$path_validation"/rail4284.mtx
    # "$path_validation"/pwtk.mtx
    # "$path_validation"/crankseg_2.mtx
    # "$path_validation"/Si41Ge41H72.mtx
    # "$path_validation"/TSOPF_RS_b2383.mtx
    # "$path_validation"/in-2004.mtx
    # "$path_validation"/Ga41As41H72.mtx
    # "$path_validation"/eu-2005.mtx
    # "$path_validation"/wikipedia-20051105.mtx
    # "$path_validation"/ldoor.mtx
    # "$path_validation"/circuit5M.mtx
    # "$path_validation"/bone010.mtx
    # "$path_validation"/cage15.mtx

)

matrices_validation2=(
    "$path_validation2"/rgg_n_2_17_s0.mtx
    "$path_validation2"/appu.mtx
    "$path_validation2"/thermomech_dK.mtx
    "$path_validation2"/ASIC_680k.mtx
    "$path_validation2"/roadNet-TX.mtx
    "$path_validation2"/com-Youtube.mtx
    "$path_validation2"/rajat30.mtx
    "$path_validation2"/radiation.mtx
    "$path_validation2"/Stanford_Berkeley.mtx
    "$path_validation2"/CurlCurl_2.mtx
    "$path_validation2"/gupta3.mtx
    "$path_validation2"/kron_g500-logn18.mtx
    "$path_validation2"/human_gene1.mtx
    "$path_validation2"/delaunay_n22.mtx
    "$path_validation2"/GL7d20.mtx
    "$path_validation2"/sx-stackoverflow.mtx
    "$path_validation2"/dgreen.mtx
    "$path_validation2"/mawi_201512012345.mtx
    "$path_validation2"/dielFilterV2real.mtx
    "$path_validation2"/soc-LiveJournal1.mtx
    "$path_validation2"/audikw_1.mtx
    "$path_validation2"/kmer_V2a.mtx
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

    ./mat_experiment.exe $MATRIX_PATH/$matrix_name $plot $store $nnz_threshold $split_matrix $sort_rows $separate $max_distance $shuffle

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

