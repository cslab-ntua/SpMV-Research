#!/bin/bash


path_validation='../'
path_validation2='../new_matrices'

cores='8'
max_cores=8
export OMP_NUM_THREADS="$cores"
export GOMP_CPU_AFFINITY="0-$((max_cores-1))"


# Encourages idle threads to spin rather than sleep.
export OMP_WAIT_POLICY='active'
# Don't let the runtime deliver fewer threads than those we asked for.
export OMP_DYNAMIC='false'

matrices_validation=(

    "$path_validation"/scircuit.mtx
    "$path_validation"/mac_econ_fwd500.mtx
    "$path_validation"/raefsky3.mtx
    "$path_validation"/bbmat.mtx
    "$path_validation"/conf5_4-8x8-15.mtx
    "$path_validation"/mc2depi.mtx
    "$path_validation"/rma10.mtx
    "$path_validation"/cop20k_A.mtx
    "$path_validation"/webbase-1M.mtx
    "$path_validation"/cant.mtx
    "$path_validation"/pdb1HYS.mtx
    "$path_validation"/TSOPF_RS_b300_c3.mtx
    "$path_validation"/Chebyshev4.mtx
    "$path_validation"/consph.mtx
    "$path_validation"/shipsec1.mtx
    "$path_validation"/PR02R.mtx
    "$path_validation"/mip1.mtx
    "$path_validation"/rail4284.mtx
    "$path_validation"/pwtk.mtx
    "$path_validation"/crankseg_2.mtx
    "$path_validation"/Si41Ge41H72.mtx
    "$path_validation"/TSOPF_RS_b2383.mtx
    "$path_validation"/in-2004.mtx
    "$path_validation"/Ga41As41H72.mtx
    "$path_validation"/eu-2005.mtx
    "$path_validation"/wikipedia-20051105.mtx
    "$path_validation"/ldoor.mtx
    "$path_validation"/circuit5M.mtx
    "$path_validation"/bone010.mtx
    "$path_validation"/cage15.mtx

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
    "${matrices_validation2[@]}"
)


for a in "${matrices[@]}"
do
    ./mat_feat.exe   $a
done

