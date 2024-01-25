#!/bin/bash


# path_validation='../'
# path_validation2='../new_matrices'
# path_validation="$HOME/Data/graphs/validation_matrices"
# path_validation='/various/pmpakos/SpMV-Research/validation_matrices/matrix_features/matrices'
# path_validation='/various/pmpakos/SpMV-Research/validation_matrices/reordered_matrices/RCM/'
path_validation='/various/pmpakos/SpMV-Research/validation_matrices/reordered_matrices/'
# path_validation='/various/pmpakos/SpMV-Research/validation_matrices/synthetic_granularities'
# path_validation=/various/pmpakos/SpMV-Research/validation_matrices/small_cross_row_similarity_below_0.4

cores='48'
max_cores=96
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
    # ${path_validation}/synthetic_m1048576_n1048576_avg32_gg2_rg2_pg4.mtx

    # human_gene2.mtx
    # GL7d21.mtx
    # eu-2005.mtx
    # wikipedia-20051105.mtx
    # fem_hifreq_circuit.mtx
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
    # dielFilterV3clx.mtx
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

