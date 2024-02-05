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
    # mycielskian16.mtx
    # rgg_n_2_22_s0.mtx
    # ljournal-2008.mtx
    # mycielskian17.mtx
    # hollywood-2009.mtx
    # rgg_n_2_23_s0.mtx
    #########################

    kmer_V2a.mtx

)

plot=0
store=1
shuffle=0
nnz_threshold=0
split_matrix=0
sort_rows=0
separate=0
max_distance=10

numClusters=8
threshold=0.01

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
    # ./mat_experiment.exe
    # ./mat_experiment_inverse.exe
    ./mat_experiment_char.exe
    # ./mat_experiment_char_inverse.exe
    # ./mat_general.exe
)

for program in "${programs[@]}"; do
    for numClusters in 32; do 
        for batch in 32; do # 32 64; do
            for window_width in 32; do # 64 128

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
