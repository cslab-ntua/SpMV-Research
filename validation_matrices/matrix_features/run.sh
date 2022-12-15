#!/bin/bash


path_validation='/home/jim/Data/graphs/validation_matrices'

cores='4'
max_cores=4
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


matrices=(
    # "$path_validation"/scircuit.mtx
    "${matrices_validation[@]}"
)


for a in "${matrices[@]}"
do
    ./mat_feat.exe   $a
done

