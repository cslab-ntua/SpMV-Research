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
export GOMP_CPU_AFFINITY="24-47"
# export GOMP_CPU_AFFINITY="0-23,48-71"


# Encourages idle threads to spin rather than sleep.
export OMP_WAIT_POLICY='active'
# Don't let the runtime deliver fewer threads than those we asked for.
export OMP_DYNAMIC='false'

matrices_validation=(
    # synthetic_m1024_n1024_avg8_gg2_rg1_pg4.mtx
    # synthetic_m1024_n1024_avg32_gg2_rg1_pg4.mtx
    # synthetic_m8192_n8192_avg8_gg2_rg1_pg4.mtx
    # synthetic_m8192_n8192_avg16_gg2_rg1_pg4.mtx
    # synthetic_m8192_n8192_avg32_gg2_rg1_pg4.mtx
    # synthetic_m8192_n8192_avg32_gg2_rg2_pg4.mtx
    # synthetic_m1048576_n1048576_avg8_gg2_rg1_pg4.mtx
    # synthetic_m1048576_n1048576_avg16_gg2_rg1_pg4.mtx
    # synthetic_m1048576_n1048576_avg32_gg4_rg4_pg4.mtx
    # synthetic_m1048576_n1048576_avg32_gg4_rg8_pg128.mtx
    # synthetic_m1048576_n1048576_avg32_gg8_rg4_pg4.mtx
    # synthetic_m1048576_n1048576_avg32_gg2_rg2_pg4.mtx

    # synthetic_m1048576_n1048576_avg32_gg4_rg2_pg4.mtx
    # synthetic_m1048576_n1048576_avg32_gg8_rg2_pg4.mtx
    # synthetic_m1048576_n1048576_avg32_gg16_rg2_pg4.mtx
    # GL7d19.mtx

    # scircuit.mtx
    # mac_econ_fwd500.mtx
    # raefsky3.mtx
    # rgg_n_2_17_s0.mtx
    # bbmat.mtx
    # appu.mtx
    # conf5_4-8x8-15.mtx
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
    # Ga41As41H72.mtx
    # eu-2005.mtx
    # wikipedia-20051105.mtx
    # kron_g500-logn18.mtx
    # rajat31.mtx
    # human_gene1.mtx
    # delaunay_n22.mtx
    # GL7d20.mtx
    # sx-stackoverflow.mtx
    # dgreen.mtx
    # mawi_201512012345.mtx
    # ldoor.mtx
    # dielFilterV2real.mtx
    # circuit5M.mtx
    # soc-LiveJournal1.mtx
    # bone010.mtx
    # audikw_1.mtx
    # cage15.mtx
    # kmer_V2a.mtx

    # GL7d21.mtx
    # eu-2005.mtx
    # wikipedia-20051105.mtx
    # kron_g500-logn18.mtx
    # StocF-1465.mtx
    # gsm_106857.mtx
    # 12month1.mtx
    # as-Skitter.mtx
    # human_gene1.mtx
    # GL7d17.mtx
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
    # ss.mtx
    # GL7d18.mtx
    # sx-stackoverflow.mtx
    # PFlow_742.mtx
    # GL7d19.mtx
    # wikipedia-20060925.mtx
    # dgreen.mtx
    # wikipedia-20061104.mtx
    # kron_g500-logn19.mtx
    # spal_004.mtx
    # wikipedia-20070206.mtx
    # dielFilterV2real.mtx
    # nv2.mtx
    # rgg_n_2_22_s0.mtx
    # vas_stokes_2M.mtx
    # soc-LiveJournal1.mtx
    # com-LiveJournal.mtx
    # ljournal-2008.mtx
    # kron_g500-logn20.mtx
    # dielFilterV3real.mtx
    # mycielskian17.mtx
    # cage15.mtx
    # hollywood-2009.mtx
    # rgg_n_2_23_s0.mtx
    # vas_stokes_4M.mtx
    # kron_g500-logn21.mtx

    # coPapersDBLP.mtx
    #########################
    #### "good" matrices ####
    #########################
    # 12month1.mtx
    # as-Skitter.mtx
    # wiki-topcats.mtx
    # rgg_n_2_21_s0.mtx
    # coPapersDBLP.mtx
    # soc-Pokec.mtx
    # coPapersCiteseer.mtx
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
shuffle=0
nnz_threshold=0
split_matrix=0
sort_rows=0
separate=0
max_distance=10

batch=32
numClusters=8
threshold=0.01

echo "plot          " $plot
echo "store         " $store
echo "nnz_threshold " $nnz_threshold
echo "split_matrix  " $split_matrix
echo "sort_rows     " $sort_rows
echo "separate      " $separate
echo "max_distance  " $max_distance
echo "shuffle       " $shuffle
echo "window_width  " $window_width
echo "numClusters   " $numClusters
echo "threshold     " $threshold

for matrix in "${matrices_validation[@]}"; do
    IFS=" " read -ra matrix_parts <<< "$matrix"
    matrix_name="${matrix_parts[0]}"

    for window_width in 64 128; do
        echo "matrix_name   " $matrix_name
        ./mat_experiment_char.exe $path_validation/$matrix_name $plot $store $nnz_threshold $split_matrix $sort_rows $separate $max_distance $shuffle $window_width $batch $numClusters $threshold
        echo '---'
    done
done
# rgg_n_2_17_s0.mtx cop20k_A.mtx roadNet-TX.mtx com-Youtube.mtx shipsec1.mtx pdb1HYS.mtx cant.mtx gupta3.mtx mip1.mtx consph.mtx CurlCurl_2.mtx crankseg_2.mtx pwtk.mtx kron_g500-logn18.mtx delaunay_n22.mtx Si41Ge41H72.mtx Ga41As41H72.mtx human_gene1.mtx bone010.mtx ldoor.mtx dielFilterV2real.mtx audikw_1.mtx circuit5M.mtx 
# rgg_n_2_17_s0_rcm.mtx cop20k_A_rcm.mtx roadNet-TX_rcm.mtx com-Youtube_rcm.mtx shipsec1_rcm.mtx pdb1HYS_rcm.mtx cant_rcm.mtx gupta3_rcm.mtx mip1_rcm.mtx consph_rcm.mtx CurlCurl_2_rcm.mtx crankseg_2_rcm.mtx pwtk_rcm.mtx kron_g500-logn18_rcm.mtx delaunay_n22_rcm.mtx Si41Ge41H72_rcm.mtx Ga41As41H72_rcm.mtx human_gene1_rcm.mtx bone010_rcm.mtx ldoor_rcm.mtx dielFilterV2real_rcm.mtx audikw_1_rcm.mtx circuit5M_rcm.mtx
# GL7d19.mtx GL7d20.mtx GL7d18.mtx GL7d21.mtx GL7d17.mtx GL7d16.mtx rgg_n_2_24_s0.mtx kron_g500-logn21.mtx rgg_n_2_23_s0.mtx patents.mtx cit-Patents.mtx kron_g500-logn20.mtx rgg_n_2_22_s0.mtx kron_g500-logn19.mtx rgg_n_2_21_s0.mtx kron_g500-logn18.mtx rgg_n_2_20_s0.mtx kron_g500-logn17.mtx wikipedia-20070206.mtx wikipedia-20060925.mtx wikipedia-20061104.mtx soc-Pokec.mtx wikipedia-20051105.mtx com-Orkut.mtx kmer_U1a.mtx higgs-twitter.mtx nv2.mtx kmer_P1a.mtx kmer_V2a.mtx sx-stackoverflow.mtx road_central.mtx test1.mtx 12month1.mtx dgreen.mtx mouse_gene.mtx spal_004.mtx GAP-road.mtx road_usa.mtx JP.mtx tp-6.mtx AS365.mtx rail4284.mtx wiki-topcats.mtx M6.mtx com-LiveJournal.mtx NLR.mtx soc-LiveJournal1.mtx human_gene1.mtx relat9.mtx rel9.mtx hugebubbles-00010.mtx human_gene2.mtx flickr.mtx hugebubbles-00020.mtx hugetric-00010.mtx hugetric-00020.mtx ljournal-2008.mtx gsm_106857.mtx
