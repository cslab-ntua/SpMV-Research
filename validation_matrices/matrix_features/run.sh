#!/bin/bash

# path_validation='../'
# path_validation2='../new_matrices'
# path_validation="$HOME/Data/graphs/validation_matrices"
path_validation='/various/pmpakos/SpMV-Research/validation_matrices'
# path_validation='/various/pmpakos/SpMV-Research/validation_matrices/reordered_matrices/RCM/'
# path_validation='/various/pmpakos/SpMV-Research/validation_matrices/matrix_features/matrices'
# path_validation=''
# path_validation='/various/pmpakos/SpMV-Research/validation_matrices/reordered_matrices/'
# path_validation='/various/pmpakos/SpMV-Research/validation_matrices/synthetic_granularities'
# path_validation=/various/pmpakos/SpMV-Research/validation_matrices/small_cross_row_similarity_below_0.4

cores='48'
max_cores=96
export OMP_NUM_THREADS="$cores"
export GOMP_CPU_AFFINITY="23-$((max_cores-1))"

# Encourages idle threads to spin rather than sleep.
export OMP_WAIT_POLICY='active'
# Don't let the runtime deliver fewer threads than those we asked for.
export OMP_DYNAMIC='false'

matrices_validation=(
# BAD
kmer_V2a.mtx
wikipedia-20070206.mtx
sx-stackoverflow.mtx
wikipedia-20061104.mtx
wikipedia-20060925.mtx
GL7d20.mtx
GL7d19.mtx
GL7d17.mtx
soc-LiveJournal1.mtx
soc-Pokec.mtx
GL7d21.mtx
GL7d18.mtx
dgreen.mtx
kron_g500-logn18.mtx
wikipedia-20051105.mtx
kron_g500-logn21.mtx
kron_g500-logn20.mtx
com-LiveJournal.mtx
kron_g500-logn19.mtx
ljournal-2008.mtx
wiki-topcats.mtx

# # GOOD
# StocF-1465.mtx
# human_gene1.mtx
# Ga41As41H72.mtx
# vas_stokes_2M.mtx
# 12month1.mtx
# CurlCurl_4.mtx
# hollywood-2009.mtx
# vas_stokes_4M.mtx
# vas_stokes_1M.mtx
# dielFilterV2real.mtx
# PFlow_742.mtx
# ldoor.mtx
# eu-2005.mtx
# coPapersDBLP.mtx
# dielFilterV3real.mtx
# mycielskian16.mtx
# audikw_1.mtx
# mycielskian17.mtx
# bone010.mtx
# coPapersCiteseer.mtx
# spal_004.mtx
)

matrices=(
    # "$path_validation"/scircuit.mtx
    "${matrices_validation[@]}"
)


for a in "${matrices[@]}"
do
    echo '--------'
    echo ${path_validation}/$a
    # ./mat_feat.exe ${path_validation}/$a
    ./mat_experiment_col_sorting.exe ${path_validation}/$a
done

