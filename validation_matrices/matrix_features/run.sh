#!/bin/bash


# path_validation='../'
# path_validation2='../new_matrices'
# path_validation="$HOME/Data/graphs/validation_matrices"
# path_validation='/various/pmpakos/SpMV-Research/validation_matrices'
path_validation='/various/pmpakos/SpMV-Research/validation_matrices/reordered_matrices/RCM/'
# path_validation='/various/pmpakos/SpMV-Research/validation_matrices/matrix_features/matrices'
# path_validation=''
# path_validation='/various/pmpakos/SpMV-Research/validation_matrices/reordered_matrices/'
# path_validation='/various/pmpakos/SpMV-Research/validation_matrices/synthetic_granularities'
# path_validation=/various/pmpakos/SpMV-Research/validation_matrices/small_cross_row_similarity_below_0.4

cores='24'
max_cores=96
export OMP_NUM_THREADS="$cores"
export GOMP_CPU_AFFINITY="23-$((max_cores-1))"


# Encourages idle threads to spin rather than sleep.
export OMP_WAIT_POLICY='active'
# Don't let the runtime deliver fewer threads than those we asked for.
export OMP_DYNAMIC='false'

matrices_validation=(
# nv2010.mtx
# scircuit.mtx
# mac_econ_fwd500.mtx
# raefsky3.mtx
# rgg_n_2_17_s0.mtx
# bbmat.mtx
# appu.mtx
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
# human_gene2.mtx
# GL7d21.mtx
# Ga41As41H72.mtx
# eu-2005.mtx
# wikipedia-20051105.mtx
# kron_g500-logn18.mtx
# StocF-1465.mtx
# rajat31.mtx
# gsm_106857.mtx
# 12month1.mtx
# as-Skitter.mtx
# human_gene1.mtx
# GL7d17.mtx
# delaunay_n22.mtx
# CurlCurl_4.mtx
# cage14.mtx
# mouse_gene.mtx
# wiki-topcats.mtx
# rgg_n_2_21_s0.mtx
# GL7d20.mtx
# coPapersDBLP.mtx
# soc-Pokec.mtx
# coPapersCiteseer.mtx
# mycielskian16.mtx
# vas_stokes_1M.mtx
# GL7d18.mtx
# sx-stackoverflow.mtx
# PFlow_742.mtx
# GL7d19.mtx
# wikipedia-20060925.mtx
# dgreen.mtx
# wikipedia-20061104.mtx
# kron_g500-logn19.mtx
# mawi_201512012345.mtx
# spal_004.mtx
# wikipedia-20070206.mtx
# ldoor.mtx
# dielFilterV2real.mtx
# circuit5M.mtx
# rgg_n_2_22_s0.mtx
# vas_stokes_2M.mtx
# soc-LiveJournal1.mtx
# com-LiveJournal.mtx
# bone010.mtx
# audikw_1.mtx
# ljournal-2008.mtx
# kron_g500-logn20.mtx
# dielFilterV3real.mtx
# mycielskian17.mtx
# cage15.mtx
# hollywood-2009.mtx
# rgg_n_2_23_s0.mtx
# vas_stokes_4M.mtx
# kmer_V2a.mtx
# kron_g500-logn21.mtx

# nv2010_rcm.mtx
# scircuit_rcm.mtx
# mac_econ_fwd500_rcm.mtx
# raefsky3_rcm.mtx
# rgg_n_2_17_s0_rcm.mtx
# bbmat_rcm.mtx
# appu_rcm.mtx
# mc2depi_rcm.mtx
# rma10_rcm.mtx
# cop20k_A_rcm.mtx
# thermomech_dK_rcm.mtx
# webbase-1M_rcm.mtx
# cant_rcm.mtx
# ASIC_680k_rcm.mtx
# roadNet-TX_rcm.mtx
# pdb1HYS_rcm.mtx
# TSOPF_RS_b300_c3_rcm.mtx
# Chebyshev4_rcm.mtx
# consph_rcm.mtx
# com-Youtube_rcm.mtx
# rajat30_rcm.mtx
# radiation_rcm.mtx
# Stanford_Berkeley_rcm.mtx
# shipsec1_rcm.mtx
# PR02R_rcm.mtx
# CurlCurl_2_rcm.mtx
# gupta3_rcm.mtx
# mip1_rcm.mtx
# pwtk_rcm.mtx
# crankseg_2_rcm.mtx
# Si41Ge41H72_rcm.mtx
# TSOPF_RS_b2383_rcm.mtx
# in-2004_rcm.mtx
# human_gene2_rcm.mtx
# Ga41As41H72_rcm.mtx
# eu-2005_rcm.mtx
# wikipedia-20051105_rcm.mtx
# kron_g500-logn18_rcm.mtx
# StocF-1465_rcm.mtx
# rajat31_rcm.mtx
# gsm_106857_rcm.mtx
# as-Skitter_rcm.mtx
# human_gene1_rcm.mtx
# delaunay_n22_rcm.mtx
# CurlCurl_4_rcm.mtx
# cage14_rcm.mtx
# mouse_gene_rcm.mtx
# wiki-topcats_rcm.mtx
# rgg_n_2_21_s0_rcm.mtx
# coPapersDBLP_rcm.mtx
# soc-Pokec_rcm.mtx
# coPapersCiteseer_rcm.mtx
# mycielskian16_rcm.mtx
# vas_stokes_1M_rcm.mtx
# sx-stackoverflow_rcm.mtx
# PFlow_742_rcm.mtx
# wikipedia-20060925_rcm.mtx
# dgreen_rcm.mtx
# wikipedia-20061104_rcm.mtx
# kron_g500-logn19_rcm.mtx
# mawi_201512012345_rcm.mtx
# wikipedia-20070206_rcm.mtx
# ldoor_rcm.mtx
# dielFilterV2real_rcm.mtx
# circuit5M_rcm.mtx
# rgg_n_2_22_s0_rcm.mtx
# vas_stokes_2M_rcm.mtx
# soc-LiveJournal1_rcm.mtx
# com-LiveJournal_rcm.mtx
# bone010_rcm.mtx
# audikw_1_rcm.mtx
# ljournal-2008_rcm.mtx
# kron_g500-logn20_rcm.mtx
# dielFilterV3real_rcm.mtx
# mycielskian17_rcm.mtx
# cage15_rcm.mtx
# hollywood-2009_rcm.mtx
# rgg_n_2_23_s0_rcm.mtx
# vas_stokes_4M_rcm.mtx
# kmer_V2a_rcm.mtx
# kron_g500-logn21_rcm.mtx
)

matrices=(
    # "$path_validation"/scircuit.mtx
    "${matrices_validation[@]}"
)


for a in "${matrices[@]}"
do
    echo '--------'
    echo ${path_validation}/$a
    ./mat_feat.exe   ${path_validation}/$a
done

