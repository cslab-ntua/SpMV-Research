#!/bin/bash

# path_validation='../'
# path_validation2='../new_matrices'
# path_validation="$HOME/Data/graphs/validation_matrices"
# path_validation='/various/pmpakos/SpMV-Research/validation_matrices'
# path_validation='/local/pmpakos/SpMV-Research/validation_matrices'
path_validation='/local/pmpakos/SpMV-Research/validation_matrices/download_matrices/new_folder'
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
# kmer_V2a.mtx
# wikipedia-20070206.mtx
# sx-stackoverflow.mtx
# wikipedia-20061104.mtx
# wikipedia-20060925.mtx
# GL7d20.mtx
# GL7d19.mtx
# GL7d17.mtx
# soc-LiveJournal1.mtx
# soc-Pokec.mtx
# GL7d21.mtx
# GL7d18.mtx
# dgreen.mtx
# kron_g500-logn18.mtx
# wikipedia-20051105.mtx
# kron_g500-logn21.mtx
# kron_g500-logn20.mtx
# com-LiveJournal.mtx
# kron_g500-logn19.mtx
# ljournal-2008.mtx
# wiki-topcats.mtx

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

# cit-Patents.mtx
# human_gene2.mtx
# GL7d21.mtx
# Ga41As41H72.mtx
# great-britain_osm.mtx
# hugetric-00000.mtx
# eu-2005.mtx
# Freescale1.mtx
# wikipedia-20051105.mtx
# circuit5M_dc.mtx
# bundle_adj.mtx
# msdoor.mtx
# fem_hifreq_circuit.mtx
# kron_g500-logn18.mtx
# StocF-1465.mtx
# rajat31.mtx
# gsm_106857.mtx
# hugetric-00010.mtx
# M6.mtx
# CoupCons3D.mtx
# 12month1.mtx
# as-Skitter.mtx
# 333SP.mtx
# hugetric-00020.mtx
# AS365.mtx
# Transport.mtx
# Freescale2.mtx
# human_gene1.mtx
# NLR.mtx
# GL7d17.mtx
# delaunay_n22.mtx
# F1.mtx
# rel9.mtx
# CurlCurl_4.mtx
# FullChip.mtx
# cage14.mtx
# ML_Laplace.mtx
# germany_osm.mtx
# nd24k.mtx
# Fault_639.mtx
# mouse_gene.mtx
# nlpkkt80.mtx
# wiki-topcats.mtx
# asia_osm.mtx
# adaptive.mtx
# rgg_n_2_21_s0.mtx
# GL7d20.mtx
# coPapersDBLP.mtx
# soc-Pokec.mtx
# coPapersCiteseer.mtx
# dielFilterV3clx.mtx
# mycielskian16.mtx
# vas_stokes_1M.mtx
# packing-500x100x100-b050.mtx
# GL7d18.mtx
# inline_1.mtx
# sx-stackoverflow.mtx
# PFlow_742.mtx
# RM07R.mtx
# GL7d19.mtx
# wikipedia-20060925.mtx
# road_central.mtx
# dgreen.mtx
# hugetrace-00010.mtx
# wikipedia-20061104.mtx
# Emilia_923.mtx
# relat9.mtx
# Hardesty3.mtx
# kron_g500-logn19.mtx
# mawi_201512012345.mtx
# spal_004.mtx
# wikipedia-20070206.mtx
# ldoor.mtx
# dielFilterV2real.mtx
# delaunay_n23.mtx
# af_shell10.mtx
# hugetrace-00020.mtx
# boneS10.mtx
# wb-edu.mtx
# hugebubbles-00000.mtx
# circuit5M.mtx
# Hook_1498.mtx
# rgg_n_2_22_s0.mtx
# Geo_1438.mtx
# hugebubbles-00010.mtx
# Serena.mtx
# GAP-road.mtx
# road_usa.mtx
# vas_stokes_2M.mtx
# soc-LiveJournal1.mtx
# hugebubbles-00020.mtx
# com-LiveJournal.mtx
# bone010.mtx
# audikw_1.mtx
# ljournal-2008.mtx
# mawi_201512020000.mtx
# channel-500x100x100-b050.mtx
# Long_Coup_dt0.mtx
# Long_Coup_dt6.mtx
# kron_g500-logn20.mtx
# dielFilterV3real.mtx
# nlpkkt120.mtx
# mycielskian17.mtx
# cage15.mtx
# delaunay_n24.mtx
# ML_Geer.mtx
# hollywood-2009.mtx
# Flan_1565.mtx
# europe_osm.mtx
# Cube_Coup_dt0.mtx
# Cube_Coup_dt6.mtx
# Bump_2911.mtx
# rgg_n_2_23_s0.mtx
# vas_stokes_4M.mtx
# kmer_V2a.mtx
# kmer_U1a.mtx
# mawi_201512020030.mtx
# kron_g500-logn21.mtx
# indochina-2004.mtx
# nlpkkt160.mtx
# com-Orkut.mtx
# rgg_n_2_24_s0.mtx
# HV15R.mtx
# mycielskian18.mtx
# uk-2002.mtx
# mawi_201512020130.mtx
# Queen_4147.mtx
com-LiveJournal.mtx
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
    ./mat_experiment_row_slice.exe ${path_validation}/$a
    # ./mat_experiment_col_sorting.exe ${path_validation}/$a
done

