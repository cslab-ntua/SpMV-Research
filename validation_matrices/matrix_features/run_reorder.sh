#!/bin/bash


# path_validation='../'
# path_validation2='../new_matrices'
# path_validation="$HOME/Data/graphs/validation_matrices"
path_validation='/various/pmpakos/SpMV-Research/validation_matrices'
# path_validation=/various/pmpakos/SpMV-Research/validation_matrices/small_cross_row_similarity_below_0.4
# path_validation=/various/pmpakos/SpMV-Research/validation_matrices/synthetic_granularities

cores=48
max_cores=${cores}
export OMP_NUM_THREADS="$cores"
export GOMP_CPU_AFFINITY="0-$((max_cores-1))"
# export GOMP_CPU_AFFINITY="0-23,48-71"


# Encourages idle threads to spin rather than sleep.
export OMP_WAIT_POLICY='active'
# Don't let the runtime deliver fewer threads than those we asked for.
export OMP_DYNAMIC='false'

matrices_validation=(
    #########################
    #### "good" matrices ####
    #########################
    12month1.mtx
    as-Skitter.mtx
    wiki-topcats.mtx
    rgg_n_2_21_s0.mtx
    coPapersDBLP.mtx
    soc-Pokec.mtx
    coPapersCiteseer.mtx
    mycielskian16.mtx
    rgg_n_2_22_s0.mtx
    ljournal-2008.mtx
    mycielskian17.mtx
    hollywood-2009.mtx
    rgg_n_2_23_s0.mtx
    #########################
)

plot=0
store=1
# store=0
shuffle=0
nnz_threshold=0
split_matrix=0
sort_rows=0
separate=0
max_distance=10

threshold=0.01

numClusters=8
# reorder_method=0 # patoh
reorder_method=1 # metis

for matrix in "${matrices_validation[@]}"; do
    IFS=" " read -ra matrix_parts <<< "$matrix"
    matrix_name="${matrix_parts[0]}"

    echo "matrix_name    " $matrix_name
    echo "plot           " $plot
    echo "store          " $store
    echo "nnz_threshold  " $nnz_threshold
    echo "split_matrix   " $split_matrix
    echo "sort_rows      " $sort_rows
    echo "separate       " $separate
    echo "max_distance   " $max_distance
    echo "shuffle        " $shuffle
    echo "window_width   " $window_width
    echo "numClusters    " $numClusters
    echo "threshold      " $threshold
    echo "reorder_method " $reorder_method
    ./mat_reorder.exe $path_validation/$matrix_name $plot $store $nnz_threshold $split_matrix $sort_rows $separate $max_distance $shuffle $numClusters $reorder_method
done
