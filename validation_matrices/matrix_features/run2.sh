#!/bin/bash


# path_validation='../'
# path_validation2='../new_matrices'
# path_validation="$HOME/Data/graphs/validation_matrices"
path_validation='/various/pmpakos/SpMV-Research/validation_matrices'
# path_validation=/various/pmpakos/SpMV-Research/validation_matrices/small_cross_row_similarity_below_0.4
# path_validation=/various/pmpakos/SpMV-Research/validation_matrices/synthetic_granularities

cores=24
max_cores=${cores}
export OMP_NUM_THREADS="$cores"
export GOMP_CPU_AFFINITY="0-$((max_cores-1))"
# export GOMP_CPU_AFFINITY="0-23,48-71"


# Encourages idle threads to spin rather than sleep.
export OMP_WAIT_POLICY='active'
# Don't let the runtime deliver fewer threads than those we asked for.
export OMP_DYNAMIC='false'

matrices_validation=(
    # GL7d21.mtx
    # CurlCurl_2.mtx
    kron_g500-logn18.mtx
    # delaunay_n22.mtx
)

plot=0
store=0
shuffle=0
nnz_threshold=0
split_matrix=0
sort_rows=0
separate=0
max_distance=10

numClusters=8
threshold=0.005

seed=14

echo "plot          " $plot
echo "store         " $store
echo "nnz_threshold " $nnz_threshold
echo "split_matrix  " $split_matrix
echo "sort_rows     " $sort_rows
echo "separate      " $separate
echo "max_distance  " $max_distance
echo "shuffle       " $shuffle
echo "threshold     " $threshold

programs=(
    ./mat_experiment.exe
    # ./mat_experiment_inverse.exe
    # ./mat_experiment_char.exe
    # ./mat_experiment_char_inverse.exe
    # ./mat_general.exe
)

for program in "${programs[@]}"; do
    for numClusters in 8; do 
        for batch in 1; do # 32 64; do
            for window_width in 32768; do # 64 128

                for matrix in "${matrices_validation[@]}"; do
                    IFS=" " read -ra matrix_parts <<< "$matrix"
                    matrix_name="${matrix_parts[0]}"

                    echo "program       " $program
                    echo "matrix_name   " $matrix_name
                    echo "window_width  " $window_width
                    echo "batch         " $batch
                    echo "numClusters   " $numClusters
                    echo "seed          " $seed

                    $program $path_validation/$matrix_name $plot $store $nnz_threshold $split_matrix $sort_rows $separate $max_distance $shuffle $seed $window_width $batch $numClusters $threshold
                    echo '---'
                done
            done
        done
    done
done
