#!/bin/bash


path_validation='/home/jim/Data/graphs/validation_matrices'


# cores='8'
# max_cores=8
# export OMP_NUM_THREADS="$cores"
# export GOMP_CPU_AFFINITY="0-$((max_cores-1))"


# Encourages idle threads to spin rather than sleep.
export OMP_WAIT_POLICY='active'
# Don't let the runtime deliver fewer threads than those we asked for.
export OMP_DYNAMIC='false'


matrices_validation=(
    "$path_validation"/scircuit.sorted.mtx
    "$path_validation"/mac_econ_fwd500.sorted.mtx
    "$path_validation"/raefsky3.sorted.mtx
    "$path_validation"/bbmat.sorted.mtx
    "$path_validation"/conf5_4-8x8-15.real.mtx
    "$path_validation"/mc2depi.real.mtx
    "$path_validation"/rma10.sorted.mtx
    "$path_validation"/cop20k_A.sorted.mtx
    "$path_validation"/webbase-1M.sorted.mtx
    "$path_validation"/cant.sorted.mtx
    "$path_validation"/pdb1HYS.sorted.mtx
    "$path_validation"/TSOPF_RS_b300_c3.sorted.mtx
    "$path_validation"/Chebyshev4.sorted.mtx
    "$path_validation"/consph.sorted.mtx
    "$path_validation"/shipsec1.sorted.mtx
    "$path_validation"/PR02R.sorted.mtx
    "$path_validation"/mip1.sorted.mtx
    "$path_validation"/rail4284.real.mtx
    "$path_validation"/pwtk.sorted.mtx
    "$path_validation"/crankseg_2.sorted.mtx
    "$path_validation"/Si41Ge41H72.sorted.mtx
    "$path_validation"/TSOPF_RS_b2383.sorted.mtx
    "$path_validation"/in-2004.sorted.mtx
    "$path_validation"/Ga41As41H72.sorted.mtx
    "$path_validation"/eu-2005.sorted.mtx
    "$path_validation"/wikipedia-20051105.sorted.mtx
    "$path_validation"/rajat31.sorted.mtx
    "$path_validation"/ldoor.sorted.mtx
    "$path_validation"/circuit5M.sorted.mtx
    "$path_validation"/bone010.sorted.mtx
    "$path_validation"/cage15.sorted.mtx
)


matrices=(
    "$path_validation"/scircuit.sorted.mtx
    # "${matrices_validation[@]}"
)


for a in "${matrices[@]}"
do
    ./mat_feat.exe   $a
done

