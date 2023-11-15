#!/bin/bash


# path_validation='../'
# path_validation2='../new_matrices'
# path_validation="$HOME/Data/graphs/validation_matrices"
# path_validation='/various/pmpakos/SpMV-Research/validation_matrices'
path_validation='/various/pmpakos/SpMV-Research/validation_matrices/synthetic_granularities'
# path_validation=/various/pmpakos/SpMV-Research/validation_matrices/small_cross_row_similarity_below_0.4

cores='24'
max_cores=24
export OMP_NUM_THREADS="$cores"
export GOMP_CPU_AFFINITY="0-$((max_cores-1))"


# Encourages idle threads to spin rather than sleep.
export OMP_WAIT_POLICY='active'
# Don't let the runtime deliver fewer threads than those we asked for.
export OMP_DYNAMIC='false'

matrices_validation=(
    # ${path_validation}/synthetic_m1024_n1024_avg8_gg2_rg1_pg4.mtx
    # ${path_validation}/synthetic_m1024_n1024_avg32_gg2_rg1_pg4.mtx
    # ${path_validation}/synthetic_m8192_n8192_avg8_gg2_rg1_pg4.mtx
    # ${path_validation}/synthetic_m8192_n8192_avg16_gg2_rg1_pg4.mtx
    # ${path_validation}/synthetic_m8192_n8192_avg32_gg2_rg1_pg4.mtx
    # ${path_validation}/synthetic_m1048576_n1048576_avg8_gg2_rg1_pg4.mtx
    # ${path_validation}/synthetic_m1048576_n1048576_avg16_gg2_rg1_pg4.mtx
    # ${path_validation}/synthetic_m1048576_n1048576_avg32_gg4_rg4_pg4.mtx
    # ${path_validation}/synthetic_m1048576_n1048576_avg32_gg4_rg8_pg128.mtx
    # ${path_validation}/synthetic_m1048576_n1048576_avg32_gg8_rg4_pg4.mtx
    # ${path_validation}/synthetic_m8192_n8192_avg32_gg2_rg2_pg4.mtx
    ${path_validation}/synthetic_m1048576_n1048576_avg32_gg2_rg2_pg4.mtx
)

matrices=(
    # "$path_validation"/scircuit.mtx
    "${matrices_validation[@]}"
)


for a in "${matrices[@]}"
do
    echo '--------'
    echo $a
    ./mat_feat.exe   $a
done

