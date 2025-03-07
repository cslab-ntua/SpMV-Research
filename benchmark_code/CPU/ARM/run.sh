#!/bin/bash

script_dir="$(dirname "$(readlink -e "${BASH_SOURCE[0]}")")"
source "$script_dir"/config.sh
echo

if [[ "$(whoami)" == 'xexdgala' ]]; then
    path_other='/zhome/academic/HLRS/xex/xexdgala/Data/graphs/other'
    path_athena='/zhome/academic/HLRS/xex/xexdgala/Data/graphs/matrices_athena'
    path_selected='/zhome/academic/HLRS/xex/xexdgala/Data/graphs/selected_matrices'
    path_selected_sorted='/zhome/academic/HLRS/xex/xexdgala/Data/graphs/selected_matrices_sorted'
else
    path_other='/home/jim/Data/graphs/other'
    path_athena='/home/jim/Data/graphs/matrices_athena'
    path_selected='/home/jim/Data/graphs/selected_matrices'
    path_selected_sorted='/home/jim/Data/graphs/selected_matrices_sorted'
fi


# GOMP_CPU_AFFINITY pins the threads to specific cpus, even when assigning more cores than threads.
# e.g. with 'GOMP_CPU_AFFINITY=0,1,2,3' and 2 threads, the threads are pinned: t0->core0 and t1->core1.
export GOMP_CPU_AFFINITY="$cpu_affinity"
export XLSMPOPTS="PROCS=$cpu_affinity"

lscpu | grep -q -i amd
if (($? == 0)); then
    export MKL_DEBUG_CPU_TYPE=5
fi
# export MKL_ENABLE_INSTRUCTIONS=AVX512
export MKL_VERBOSE=1

export LD_LIBRARY_PATH="${AOCL_PATH}/lib:${MKL_PATH}/lib/intel64:${LD_LIBRARY_PATH}"
export LD_LIBRARY_PATH="${LD_LIBRARY_PATH}:${BOOST_LIB_PATH}:${LLVM_LIB_PATH}:${SPARSEX_LIB_PATH}"
export LD_LIBRARY_PATH="${LD_LIBRARY_PATH}:/home/jim/lib/gcc/gcc_12/lib64"
export LD_LIBRARY_PATH="${LD_LIBRARY_PATH}:/various/dgal/gcc/gcc-12.2.0/gcc_bin/lib64"
export LD_LIBRARY_PATH="${LD_LIBRARY_PATH}:${CUDA_PATH}/lib64"

# Encourages idle threads to spin rather than sleep.
# export OMP_WAIT_POLICY='active'
# Don't let the runtime deliver fewer threads than those we asked for.
export OMP_DYNAMIC='false'


matrices_openFoam=("$path_openFoam"/*.mtx)

# matrices_openFoam_own_neigh=( "$path_openFoam"/TestMatrices/*/*/* )

cd "$path_openFoam"
text="$(printf "%s\n" TestMatrices/*/*/*)"
cd - &>/dev/null
sorted_text="$(sort -t '/' -k2,2 -k3,3n -k4.10,4n <<<"${text}")"

IFS_buf="$IFS"
IFS=$'\n'
matrices_openFoam_own_neigh=( $(printf "${path_openFoam}/%s\n" ${sorted_text}) )
IFS="$IFS_buf"


matrices_validation=(

    scircuit.mtx
    mac_econ_fwd500.mtx
    raefsky3.mtx
    rgg_n_2_17_s0.mtx
    bbmat.mtx
    appu.mtx
    mc2depi.mtx
    rma10.mtx
    cop20k_A.mtx
    thermomech_dK.mtx
    webbase-1M.mtx
    cant.mtx
    ASIC_680k.mtx
    roadNet-TX.mtx
    pdb1HYS.mtx
    TSOPF_RS_b300_c3.mtx
    Chebyshev4.mtx
    consph.mtx
    com-Youtube.mtx
    rajat30.mtx
    radiation.mtx
    Stanford_Berkeley.mtx
    shipsec1.mtx
    PR02R.mtx
    CurlCurl_2.mtx
    gupta3.mtx
    mip1.mtx
    rail4284.mtx
    pwtk.mtx
    crankseg_2.mtx
    Si41Ge41H72.mtx
    TSOPF_RS_b2383.mtx
    in-2004.mtx
    Ga41As41H72.mtx
    eu-2005.mtx
    wikipedia-20051105.mtx
    kron_g500-logn18.mtx
    rajat31.mtx
    human_gene1.mtx
    delaunay_n22.mtx
    GL7d20.mtx
    sx-stackoverflow.mtx
    dgreen.mtx
    # mawi_201512012345.mtx
    ldoor.mtx
    dielFilterV2real.mtx
    circuit5M.mtx
    soc-LiveJournal1.mtx
    bone010.mtx
    audikw_1.mtx
    cage15.mtx
    # kmer_V2a.mtx

)
validation_dirs=(
    "${path_validation}"
    "${path_validation}/new_matrices" 
)
matrices_validation=( $(
    for ((i=0;i<${#matrices_validation[@]};i++)); do
        m="${matrices_validation[i]}"
        for d in "${validation_dirs[@]}"; do
            if [[ -f "${d}/${m}" ]]; then
                echo "${d}/${m}"
                break
            fi
        done
    done
) )

matrices_short_rows=(
    scircuit.mtx
    mac_econ_fwd500.mtx
    mc2depi.mtx
    webbase-1M.mtx
    ASIC_680k.mtx
    roadNet-TX.mtx
    com-Youtube.mtx
    rajat30.mtx
)
# LARGE
# matrices_short_rows=(
#     scircuit.mtx
#     )
matrices_long_rows=(
    cant.mtx
    pdb1HYS.mtx
    TSOPF_RS_b300_c3.mtx
    Chebyshev4.mtx
    consph.mtx
)
matrices_short_rows=(
    # webbase-1M.mtx
    thermomech_dK.mtx
    # com-Youtube.mtx
)
matrices_long_rows=()

matrices_short_rows=( $(
    for ((i=0;i<${#matrices_short_rows[@]};i++)); do
        m="${matrices_short_rows[i]}"
        for d in "${validation_dirs[@]}"; do
            if [[ -f "${d}/${m}" ]]; then
                echo "${d}/${m}"
                break
            fi
        done
    done
) )

matrices_long_rows=( $(
    for ((i=0;i<${#matrices_long_rows[@]};i++)); do
        m="${matrices_long_rows[i]}"
        for d in "${validation_dirs[@]}"; do
            if [[ -f "${d}/${m}" ]]; then
                echo "${d}/${m}"
                break
            fi
        done
    done
) )

matrices_paper_csr_rv=(

    ts-palko
    neos
    stat96v3
    stormG2_1000
    xenon2
    s3dkq4m2
    apache2
    Si34H36
    ecology2
    LargeRegFile
    largebasis
    Goodwin_127
    Hamrle3
    boneS01
    sls
    cont1_l
    CO
    G3_circuit
    degme
    atmosmodl
    SiO2
    tp-6
    af_shell3
    circuit5M_dc
    rajat31
    CurlCurl_4
    cage14
    nlpkkt80
    ss
    boneS10

)
for ((i=0;i<${#matrices_paper_csr_rv[@]};i++)); do
    m="${matrices_paper_csr_rv[i]}"
    matrices_paper_csr_rv[i]="${path_tamu}/matrices/${m}/${m}.mtx"
done


matrices_compression_small=(

    # scircuit
    # mac_econ_fwd500
    # raefsky3
    # bbmat
    # appu
    # rma10
    # cop20k_A
    # thermomech_dK
    # webbase-1M
    # cant
    # ASIC_680k
    # pdb1HYS
    # TSOPF_RS_b300_c3
    # Chebyshev4
    # consph
    # rajat30
    # radiation
    # shipsec1
    # PR02R
    # CurlCurl_2
    # pwtk
    # crankseg_2
    # Si41Ge41H72
    # TSOPF_RS_b2383
    # Ga41As41H72
    # rajat31
    # human_gene1
    # dgreen

    cop20k_A
    # ASIC_680k
    # radiation
    # PR02R
    # crankseg_2
    # rajat31
    # human_gene1
    # dgreen

)
for ((i=0;i<${#matrices_compression_small[@]};i++)); do
    m="${matrices_compression_small[i]}"
    matrices_compression_small[i]="${path_tamu}/matrices/${m}/${m}.mtx"
done


matrices_compression=(

    # spal_004
    ldoor
    # dielFilterV2real
    # nv2
    # af_shell10
    # boneS10
    # circuit5M
    # Hook_1498
    # Geo_1438
    # Serena
    # vas_stokes_2M
    # bone010
    # audikw_1
    # Long_Coup_dt0
    # Long_Coup_dt6
    # dielFilterV3real
    # nlpkkt120
    # cage15
    # ML_Geer
    # Flan_1565
    # Cube_Coup_dt0
    # Cube_Coup_dt6
    # Bump_2911
    # vas_stokes_4M
    # nlpkkt160
    # HV15R
    # Queen_4147
    # stokes
    # nlpkkt200

    # Transport
    # Freescale2
    # FullChip
    # cage14
    # ML_Laplace
    # vas_stokes_1M
    # ss
    # RM07R
    # dgreen
    # Hardesty3
    # nlpkkt240

    # MOLIERE_2016

    # Transport
    # Freescale2
    # ldoor
    # dielFilterV2real
    # af_shell10
    # FullChip
    # cage14
    # ML_Laplace
    # boneS10
    # Hook_1498
    # Geo_1438
    # Serena
    # vas_stokes_1M
    # ss
    # bone010
    # RM07R
    # dgreen
    # audikw_1
    # Hardesty3
    # Long_Coup_dt0
    # Long_Coup_dt6
    # dielFilterV3real
    # spal_004
    # nlpkkt120
    # nv2
    # Flan_1565
    # circuit5M
    # Cube_Coup_dt0
    # Cube_Coup_dt6
    # vas_stokes_2M
    # Bump_2911
    # cage15
    # ML_Geer
    # nlpkkt160
    # vas_stokes_4M
    # Queen_4147
    # nlpkkt200
    # HV15R
    # stokes
    # nlpkkt240
    # MOLIERE_2016

)
for ((i=0;i<${#matrices_compression[@]};i++)); do
    m="${matrices_compression[i]}"
    matrices_compression[i]="${path_tamu}/matrices/${m}/${m}.mtx"
done


matrices_M3E=(

    # StocF_1465

    Heel_1138
    Hook_1498
    Utemp20m
    guenda11m
    agg14m

)
for ((i=0;i<${#matrices_M3E[@]};i++)); do
    m="${matrices_M3E[i]}"
    matrices_M3E[i]="$path_M3E"/"$m"/"$m".mtx
done


matrices_cg=(

    # ldoor
    # dielFilterV2real
    boneS10
    # Hook_1498
    # Geo_1438
    # Serena
    # bone010
    # audikw_1
    # dielFilterV3real
    # Flan_1565
    # Cube_Coup_dt0
    # Cube_Coup_dt6
    # Bump_2911
    # Queen_4147

    # af_shell10
    # Long_Coup_dt0
    # Long_Coup_dt6
)
for ((i=0;i<${#matrices_cg[@]};i++)); do
    m="${matrices_cg[i]}"
    matrices_cg[i]="${path_tamu}/matrices/${m}/${m}.mtx"
done


matrices_underperform_gpu=(

    # kmer_V2a.mtx
    wikipedia-20070206.mtx
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

    # kmer_V2a_sorted_cols.mtx
    wikipedia-20070206_sorted_cols.mtx
    sx-stackoverflow_sorted_cols.mtx
    wikipedia-20061104_sorted_cols.mtx
    wikipedia-20060925_sorted_cols.mtx
    GL7d20_sorted_cols.mtx
    GL7d19_sorted_cols.mtx
    GL7d17_sorted_cols.mtx
    soc-LiveJournal1_sorted_cols.mtx
    soc-Pokec_sorted_cols.mtx
    GL7d21_sorted_cols.mtx
    GL7d18_sorted_cols.mtx
    dgreen_sorted_cols.mtx
    kron_g500-logn18_sorted_cols.mtx
    wikipedia-20051105_sorted_cols.mtx
    kron_g500-logn21_sorted_cols.mtx
    kron_g500-logn20_sorted_cols.mtx
    com-LiveJournal_sorted_cols.mtx
    kron_g500-logn19_sorted_cols.mtx
    ljournal-2008_sorted_cols.mtx
    wiki-topcats_sorted_cols.mtx
)
matrices_goodperform_gpu=(
    StocF-1465.mtx
    human_gene1.mtx
    Ga41As41H72.mtx
    vas_stokes_2M.mtx
    12month1.mtx
    CurlCurl_4.mtx
    hollywood-2009.mtx
    vas_stokes_4M.mtx
    vas_stokes_1M.mtx
    dielFilterV2real.mtx
    PFlow_742.mtx
    ldoor.mtx
    eu-2005.mtx
    coPapersDBLP.mtx
    dielFilterV3real.mtx
    mycielskian16.mtx
    audikw_1.mtx
    mycielskian17.mtx
    bone010.mtx
    coPapersCiteseer.mtx
    spal_004.mtx

    StocF-1465_sorted_cols.mtx
    human_gene1_sorted_cols.mtx
    Ga41As41H72_sorted_cols.mtx
    vas_stokes_2M_sorted_cols.mtx
    12month1_sorted_cols.mtx
    CurlCurl_4_sorted_cols.mtx
    hollywood-2009_sorted_cols.mtx
    vas_stokes_4M_sorted_cols.mtx
    vas_stokes_1M_sorted_cols.mtx
    dielFilterV2real_sorted_cols.mtx
    PFlow_742_sorted_cols.mtx
    ldoor_sorted_cols.mtx
    eu-2005_sorted_cols.mtx
    coPapersDBLP_sorted_cols.mtx
    dielFilterV3real_sorted_cols.mtx
    mycielskian16_sorted_cols.mtx
    audikw_1_sorted_cols.mtx
    mycielskian17_sorted_cols.mtx
    bone010_sorted_cols.mtx
    coPapersCiteseer_sorted_cols.mtx
    spal_004_sorted_cols.mtx
)
matrices_underperform_gpu_GPU_PART=(
    # kmer_V2a_split_gpu_optimal_COL_WAY.mtx
    wikipedia-20070206_split_gpu_optimal_COL_WAY.mtx
    sx-stackoverflow_split_gpu_optimal_COL_WAY.mtx
    wikipedia-20061104_split_gpu_optimal_COL_WAY.mtx
    wikipedia-20060925_split_gpu_optimal_COL_WAY.mtx
    GL7d20_split_gpu_optimal_COL_WAY.mtx
    GL7d19_split_gpu_optimal_COL_WAY.mtx
    GL7d17_split_gpu_optimal_COL_WAY.mtx
    soc-LiveJournal1_split_gpu_optimal_COL_WAY.mtx
    soc-Pokec_split_gpu_optimal_COL_WAY.mtx
    GL7d21_split_gpu_optimal_COL_WAY.mtx
    GL7d18_split_gpu_optimal_COL_WAY.mtx
    dgreen_split_gpu_optimal_COL_WAY.mtx
    kron_g500-logn18_split_gpu_optimal_COL_WAY.mtx
    wikipedia-20051105_split_gpu_optimal_COL_WAY.mtx
    kron_g500-logn21_split_gpu_optimal_COL_WAY.mtx
    kron_g500-logn20_split_gpu_optimal_COL_WAY.mtx
    com-LiveJournal_split_gpu_optimal_COL_WAY.mtx
    kron_g500-logn19_split_gpu_optimal_COL_WAY.mtx
    ljournal-2008_split_gpu_optimal_COL_WAY.mtx
    wiki-topcats_split_gpu_optimal_COL_WAY.mtx
)
matrices_goodperform_gpu_GPU_PART=(
    StocF-1465_split_gpu_optimal_COL_WAY.mtx
    human_gene1_split_gpu_optimal_COL_WAY.mtx
    Ga41As41H72_split_gpu_optimal_COL_WAY.mtx
    vas_stokes_2M_split_gpu_optimal_COL_WAY.mtx
    12month1_split_gpu_optimal_COL_WAY.mtx
    CurlCurl_4_split_gpu_optimal_COL_WAY.mtx
    hollywood-2009_split_gpu_optimal_COL_WAY.mtx
    vas_stokes_4M_split_gpu_optimal_COL_WAY.mtx
    vas_stokes_1M_split_gpu_optimal_COL_WAY.mtx
    dielFilterV2real_split_gpu_optimal_COL_WAY.mtx
    PFlow_742_split_gpu_optimal_COL_WAY.mtx
    ldoor_split_gpu_optimal_COL_WAY.mtx
    eu-2005_split_gpu_optimal_COL_WAY.mtx
    coPapersDBLP_split_gpu_optimal_COL_WAY.mtx
    dielFilterV3real_split_gpu_optimal_COL_WAY.mtx
    mycielskian16_split_gpu_optimal_COL_WAY.mtx
    audikw_1_split_gpu_optimal_COL_WAY.mtx
    mycielskian17_split_gpu_optimal_COL_WAY.mtx
    bone010_split_gpu_optimal_COL_WAY.mtx
    coPapersCiteseer_split_gpu_optimal_COL_WAY.mtx
    spal_004_split_gpu_optimal_COL_WAY.mtx
)
matrices_underperform_gpu_CPU_PART=(
    # kmer_V2a_split_cpu_optimal_COL_WAY.mtx
    wikipedia-20070206_split_cpu_optimal_COL_WAY.mtx
    sx-stackoverflow_split_cpu_optimal_COL_WAY.mtx
    wikipedia-20061104_split_cpu_optimal_COL_WAY.mtx
    wikipedia-20060925_split_cpu_optimal_COL_WAY.mtx
    GL7d20_split_cpu_optimal_COL_WAY.mtx
    GL7d19_split_cpu_optimal_COL_WAY.mtx
    GL7d17_split_cpu_optimal_COL_WAY.mtx
    soc-LiveJournal1_split_cpu_optimal_COL_WAY.mtx
    soc-Pokec_split_cpu_optimal_COL_WAY.mtx
    GL7d21_split_cpu_optimal_COL_WAY.mtx
    GL7d18_split_cpu_optimal_COL_WAY.mtx
    dgreen_split_cpu_optimal_COL_WAY.mtx
    kron_g500-logn18_split_cpu_optimal_COL_WAY.mtx
    wikipedia-20051105_split_cpu_optimal_COL_WAY.mtx
    kron_g500-logn21_split_cpu_optimal_COL_WAY.mtx
    kron_g500-logn20_split_cpu_optimal_COL_WAY.mtx
    com-LiveJournal_split_cpu_optimal_COL_WAY.mtx
    kron_g500-logn19_split_cpu_optimal_COL_WAY.mtx
    ljournal-2008_split_cpu_optimal_COL_WAY.mtx
    wiki-topcats_split_cpu_optimal_COL_WAY.mtx
)
matrices_goodperform_gpu_CPU_PART=(
    StocF-1465_split_cpu_optimal_COL_WAY.mtx
    human_gene1_split_cpu_optimal_COL_WAY.mtx
    Ga41As41H72_split_cpu_optimal_COL_WAY.mtx
    vas_stokes_2M_split_cpu_optimal_COL_WAY.mtx
    12month1_split_cpu_optimal_COL_WAY.mtx
    CurlCurl_4_split_cpu_optimal_COL_WAY.mtx
    hollywood-2009_split_cpu_optimal_COL_WAY.mtx
    vas_stokes_4M_split_cpu_optimal_COL_WAY.mtx
    vas_stokes_1M_split_cpu_optimal_COL_WAY.mtx
    dielFilterV2real_split_cpu_optimal_COL_WAY.mtx
    PFlow_742_split_cpu_optimal_COL_WAY.mtx
    ldoor_split_cpu_optimal_COL_WAY.mtx
    eu-2005_split_cpu_optimal_COL_WAY.mtx
    coPapersDBLP_split_cpu_optimal_COL_WAY.mtx
    dielFilterV3real_split_cpu_optimal_COL_WAY.mtx
    mycielskian16_split_cpu_optimal_COL_WAY.mtx
    audikw_1_split_cpu_optimal_COL_WAY.mtx
    mycielskian17_split_cpu_optimal_COL_WAY.mtx
    bone010_split_cpu_optimal_COL_WAY.mtx
    coPapersCiteseer_split_cpu_optimal_COL_WAY.mtx
    spal_004_split_cpu_optimal_COL_WAY.mtx
)

matrices_underperform_gpu=( $(
    for ((i=0;i<${#matrices_underperform_gpu[@]};i++)); do
        m="${matrices_underperform_gpu[i]}"
        for d in "${validation_dirs[@]}"; do
            if [[ -f "${d}/${m}" ]]; then
                echo "${d}/${m}"
                break
            fi
        done
    done
) )
matrices_goodperform_gpu=( $(
    for ((i=0;i<${#matrices_goodperform_gpu[@]};i++)); do
        m="${matrices_goodperform_gpu[i]}"
        for d in "${validation_dirs[@]}"; do
            if [[ -f "${d}/${m}" ]]; then
                echo "${d}/${m}"
                break
            fi
        done
    done
) )
matrices_underperform_gpu_GPU_PART=( $(
    for ((i=0;i<${#matrices_underperform_gpu_GPU_PART[@]};i++)); do
        m="${matrices_underperform_gpu_GPU_PART[i]}"
        for d in "${validation_dirs[@]}"; do
            if [[ -f "${d}/${m}" ]]; then
                echo "${d}/${m}"
                break
            fi
        done
    done
) )
matrices_goodperform_gpu_GPU_PART=( $(
    for ((i=0;i<${#matrices_goodperform_gpu_GPU_PART[@]};i++)); do
        m="${matrices_goodperform_gpu_GPU_PART[i]}"
        for d in "${validation_dirs[@]}"; do
            if [[ -f "${d}/${m}" ]]; then
                echo "${d}/${m}"
                break
            fi
        done
    done
) )
matrices_underperform_gpu_CPU_PART=( $(
    for ((i=0;i<${#matrices_underperform_gpu_CPU_PART[@]};i++)); do
        m="${matrices_underperform_gpu_CPU_PART[i]}"
        for d in "${validation_dirs[@]}"; do
            if [[ -f "${d}/${m}" ]]; then
                echo "${d}/${m}"
                break
            fi
        done
    done
) )
matrices_goodperform_gpu_CPU_PART=( $(
    for ((i=0;i<${#matrices_goodperform_gpu_CPU_PART[@]};i++)); do
        m="${matrices_goodperform_gpu_CPU_PART[i]}"
        for d in "${validation_dirs[@]}"; do
            if [[ -f "${d}/${m}" ]]; then
                echo "${d}/${m}"
                break
            fi
        done
    done
) )

# export SUFFIX="_remove_n11_GPU_ws1024"
matrices_before_RCM=(
    SiO2${SUFFIX}.mtx
    fcondp2${SUFFIX}.mtx
    bmw3_2${SUFFIX}.mtx
    pwtk${SUFFIX}.mtx
    fullb${SUFFIX}.mtx
    troll${SUFFIX}.mtx
    Hardesty1${SUFFIX}.mtx
    halfb${SUFFIX}.mtx
    test1${SUFFIX}.mtx
    TSOPF_FS_b300_c3${SUFFIX}.mtx
    BenElechi1${SUFFIX}.mtx
    delaunay_n21${SUFFIX}.mtx
    CurlCurl_3${SUFFIX}.mtx
    rgg_n_2_20_s0${SUFFIX}.mtx
    crankseg_2${SUFFIX}.mtx
    nd12k${SUFFIX}.mtx
    pkustk14${SUFFIX}.mtx
    Si41Ge41H72${SUFFIX}.mtx
    hugetrace-00000${SUFFIX}.mtx
    kkt_power${SUFFIX}.mtx
    italy_osm${SUFFIX}.mtx
    venturiLevel3${SUFFIX}.mtx
    af_4_k101${SUFFIX}.mtx
    af_3_k101${SUFFIX}.mtx
    af_5_k101${SUFFIX}.mtx
    af_2_k101${SUFFIX}.mtx
    af_0_k101${SUFFIX}.mtx
    af_1_k101${SUFFIX}.mtx
    af_shell1${SUFFIX}.mtx
    af_shell3${SUFFIX}.mtx
    af_shell9${SUFFIX}.mtx
    af_shell2${SUFFIX}.mtx
    af_shell4${SUFFIX}.mtx
    af_shell5${SUFFIX}.mtx
    af_shell8${SUFFIX}.mtx
    af_shell7${SUFFIX}.mtx
    af_shell6${SUFFIX}.mtx
    human_gene2${SUFFIX}.mtx
    Ga41As41H72${SUFFIX}.mtx
    great-britain_osm${SUFFIX}.mtx
    hugetric-00000${SUFFIX}.mtx
    bundle_adj${SUFFIX}.mtx
    msdoor${SUFFIX}.mtx
    kron_g500-logn18${SUFFIX}.mtx
    StocF-1465${SUFFIX}.mtx
    rajat31${SUFFIX}.mtx
    gsm_106857${SUFFIX}.mtx
    hugetric-00010${SUFFIX}.mtx
    M6${SUFFIX}.mtx
    CoupCons3D${SUFFIX}.mtx
    as-Skitter${SUFFIX}.mtx
    333SP${SUFFIX}.mtx
    hugetric-00020${SUFFIX}.mtx
    AS365${SUFFIX}.mtx
    Transport${SUFFIX}.mtx
    human_gene1${SUFFIX}.mtx
    NLR${SUFFIX}.mtx
    delaunay_n22${SUFFIX}.mtx
    F1${SUFFIX}.mtx
    CurlCurl_4${SUFFIX}.mtx
    cage14${SUFFIX}.mtx
    ML_Laplace${SUFFIX}.mtx
    germany_osm${SUFFIX}.mtx
    nd24k${SUFFIX}.mtx
    Fault_639${SUFFIX}.mtx
    mouse_gene${SUFFIX}.mtx
    nlpkkt80${SUFFIX}.mtx
    asia_osm${SUFFIX}.mtx
    adaptive${SUFFIX}.mtx
    rgg_n_2_21_s0${SUFFIX}.mtx
    coPapersDBLP${SUFFIX}.mtx
    coPapersCiteseer${SUFFIX}.mtx
    mycielskian16${SUFFIX}.mtx
    packing-500x100x100-b050${SUFFIX}.mtx
    inline_1${SUFFIX}.mtx
    PFlow_742${SUFFIX}.mtx
    road_central${SUFFIX}.mtx
    dgreen${SUFFIX}.mtx
    hugetrace-00010${SUFFIX}.mtx
    Emilia_923${SUFFIX}.mtx
    kron_g500-logn19${SUFFIX}.mtx
    ldoor${SUFFIX}.mtx
    dielFilterV2real${SUFFIX}.mtx
    delaunay_n23${SUFFIX}.mtx
    af_shell10${SUFFIX}.mtx
    nv2${SUFFIX}.mtx
    hugetrace-00020${SUFFIX}.mtx
    boneS10${SUFFIX}.mtx
    hugebubbles-00000${SUFFIX}.mtx
    Hook_1498${SUFFIX}.mtx
    rgg_n_2_22_s0${SUFFIX}.mtx
    Geo_1438${SUFFIX}.mtx
    hugebubbles-00010${SUFFIX}.mtx
    Serena${SUFFIX}.mtx
    road_usa${SUFFIX}.mtx
    GAP-road${SUFFIX}.mtx
    hugebubbles-00020${SUFFIX}.mtx
    com-LiveJournal${SUFFIX}.mtx
    bone010${SUFFIX}.mtx
    audikw_1${SUFFIX}.mtx
    channel-500x100x100-b050${SUFFIX}.mtx
    Long_Coup_dt6${SUFFIX}.mtx
    Long_Coup_dt0${SUFFIX}.mtx
    kron_g500-logn20${SUFFIX}.mtx
    dielFilterV3real${SUFFIX}.mtx
    nlpkkt120${SUFFIX}.mtx
    mycielskian17${SUFFIX}.mtx
    cage15${SUFFIX}.mtx
    delaunay_n24${SUFFIX}.mtx
    ML_Geer${SUFFIX}.mtx
    hollywood-2009${SUFFIX}.mtx
    Flan_1565${SUFFIX}.mtx
    europe_osm${SUFFIX}.mtx
    Cube_Coup_dt6${SUFFIX}.mtx
    Cube_Coup_dt0${SUFFIX}.mtx
    Bump_2911${SUFFIX}.mtx
    rgg_n_2_23_s0${SUFFIX}.mtx
    kmer_V2a${SUFFIX}.mtx
    kmer_U1a${SUFFIX}.mtx
    kron_g500-logn21${SUFFIX}.mtx
    nlpkkt160${SUFFIX}.mtx
    com-Orkut${SUFFIX}.mtx
    rgg_n_2_24_s0${SUFFIX}.mtx
    mycielskian18${SUFFIX}.mtx
    Queen_4147${SUFFIX}.mtx
    kmer_P1a${SUFFIX}.mtx
)
matrices_RCM=(
    SiO2_RCM.mtx
    fcondp2_RCM.mtx
    bmw3_2_RCM.mtx
    pwtk_RCM.mtx
    fullb_RCM.mtx
    troll_RCM.mtx
    Hardesty1_RCM.mtx
    halfb_RCM.mtx
    test1_RCM.mtx
    TSOPF_FS_b300_c3_RCM.mtx
    BenElechi1_RCM.mtx
    delaunay_n21_RCM.mtx
    CurlCurl_3_RCM.mtx
    rgg_n_2_20_s0_RCM.mtx
    crankseg_2_RCM.mtx
    nd12k_RCM.mtx
    pkustk14_RCM.mtx
    Si41Ge41H72_RCM.mtx
    hugetrace-00000_RCM.mtx
    kkt_power_RCM.mtx
    italy_osm_RCM.mtx
    venturiLevel3_RCM.mtx
    af_4_k101_RCM.mtx
    af_3_k101_RCM.mtx
    af_5_k101_RCM.mtx
    af_2_k101_RCM.mtx
    af_0_k101_RCM.mtx
    af_1_k101_RCM.mtx
    af_shell1_RCM.mtx
    af_shell3_RCM.mtx
    af_shell9_RCM.mtx
    af_shell2_RCM.mtx
    af_shell4_RCM.mtx
    af_shell5_RCM.mtx
    af_shell8_RCM.mtx
    af_shell7_RCM.mtx
    af_shell6_RCM.mtx
    human_gene2_RCM.mtx
    Ga41As41H72_RCM.mtx
    great-britain_osm_RCM.mtx
    hugetric-00000_RCM.mtx
    bundle_adj_RCM.mtx
    msdoor_RCM.mtx
    kron_g500-logn18_RCM.mtx
    StocF-1465_RCM.mtx
    rajat31_RCM.mtx
    gsm_106857_RCM.mtx
    hugetric-00010_RCM.mtx
    M6_RCM.mtx
    CoupCons3D_RCM.mtx
    as-Skitter_RCM.mtx
    333SP_RCM.mtx
    hugetric-00020_RCM.mtx
    AS365_RCM.mtx
    Transport_RCM.mtx
    human_gene1_RCM.mtx
    NLR_RCM.mtx
    delaunay_n22_RCM.mtx
    F1_RCM.mtx
    CurlCurl_4_RCM.mtx
    cage14_RCM.mtx
    ML_Laplace_RCM.mtx
    germany_osm_RCM.mtx
    nd24k_RCM.mtx
    Fault_639_RCM.mtx
    mouse_gene_RCM.mtx
    nlpkkt80_RCM.mtx
    asia_osm_RCM.mtx
    adaptive_RCM.mtx
    rgg_n_2_21_s0_RCM.mtx
    coPapersDBLP_RCM.mtx
    coPapersCiteseer_RCM.mtx
    mycielskian16_RCM.mtx
    packing-500x100x100-b050_RCM.mtx
    inline_1_RCM.mtx
    PFlow_742_RCM.mtx
    road_central_RCM.mtx
    dgreen_RCM.mtx
    hugetrace-00010_RCM.mtx
    Emilia_923_RCM.mtx
    kron_g500-logn19_RCM.mtx
    ldoor_RCM.mtx
    dielFilterV2real_RCM.mtx
    delaunay_n23_RCM.mtx
    af_shell10_RCM.mtx
    nv2_RCM.mtx
    hugetrace-00020_RCM.mtx
    boneS10_RCM.mtx
    hugebubbles-00000_RCM.mtx
    Hook_1498_RCM.mtx
    rgg_n_2_22_s0_RCM.mtx
    Geo_1438_RCM.mtx
    hugebubbles-00010_RCM.mtx
    Serena_RCM.mtx
    road_usa_RCM.mtx
    GAP-road_RCM.mtx
    hugebubbles-00020_RCM.mtx
    com-LiveJournal_RCM.mtx
    bone010_RCM.mtx
    audikw_1_RCM.mtx
    channel-500x100x100-b050_RCM.mtx
    Long_Coup_dt6_RCM.mtx
    Long_Coup_dt0_RCM.mtx
    kron_g500-logn20_RCM.mtx
    dielFilterV3real_RCM.mtx
    nlpkkt120_RCM.mtx
    mycielskian17_RCM.mtx
    cage15_RCM.mtx
    delaunay_n24_RCM.mtx
    ML_Geer_RCM.mtx
    hollywood-2009_RCM.mtx
    Flan_1565_RCM.mtx
    europe_osm_RCM.mtx
    Cube_Coup_dt6_RCM.mtx
    Cube_Coup_dt0_RCM.mtx
    Bump_2911_RCM.mtx
    rgg_n_2_23_s0_RCM.mtx
    kmer_V2a_RCM.mtx
    kmer_U1a_RCM.mtx
    kron_g500-logn21_RCM.mtx
    nlpkkt160_RCM.mtx
    com-Orkut_RCM.mtx
    rgg_n_2_24_s0_RCM.mtx
    mycielskian18_RCM.mtx
    Queen_4147_RCM.mtx
    kmer_P1a_RCM.mtx
)

# export SUFFIX="_remove_n00_GPU_ws1024"
export SUFFIX=""
# export SUFFIX="_RCM"
matrices_before_RCM=(
    # coPapersCiteseer.mtx
    # Queen_4147.mtx
    # inline_1.mtx
    # hugetric-00010.mtx
    # audikw_1.mtx
    # Cube_Coup_dt0.mtx
    # Long_Coup_dt0.mtx
    # Bump_2911.mtx
    # Cube_Coup_dt6.mtx
    # bundle_adj.mtx
    
    # coPapersCiteseer${SUFFIX}.mtx
    # kron_g500-logn21${SUFFIX}.mtx
    # kron_g500-logn20${SUFFIX}.mtx
    # com-Orkut${SUFFIX}.mtx
    # kron_g500-logn19${SUFFIX}.mtx
    # kron_g500-logn18${SUFFIX}.mtx
    # com-LiveJournal${SUFFIX}.mtx
    # hollywood-2009${SUFFIX}.mtx
    # road_usa${SUFFIX}.mtx
    # GAP-road${SUFFIX}.mtx
    # road_central${SUFFIX}.mtx

)
matrices_before_RCM=( $(
    for ((i=0;i<${#matrices_before_RCM[@]};i++)); do
        m="${matrices_before_RCM[i]}"
        for d in "${validation_dirs[@]}"; do
            if [[ -f "${d}/${m}" ]]; then
                echo "${d}/${m}"
                break
            fi
        done
    done
) )
matrices_RCM=( $(
    for ((i=0;i<${#matrices_RCM[@]};i++)); do
        m="${matrices_RCM[i]}"
        for d in "${validation_dirs[@]}"; do
            if [[ -f "${d}/${m}" ]]; then
                echo "${d}/${m}"
                break
            fi
        done
    done
) )

matrices_footprint=(
    TSOPF_RS_b300_c3.mtx
    Chebyshev4.mtx
    consph.mtx
    com-Youtube.mtx
    radiation.mtx
    Stanford_Berkeley.mtx
    shipsec1.mtx
    PR02R.mtx
    CurlCurl_2.mtx
    gupta3.mtx

    TSOPF_RS_b2383.mtx
    in-2004.mtx
    Ga41As41H72.mtx
    rajat31.mtx
    ldoor.mtx
    circuit5M.mtx
    soc-LiveJournal1.mtx
    bone010.mtx
    audikw_1.mtx
    cage15.mtx
)
matrices_footprint=( $(
    for ((i=0;i<${#matrices_footprint[@]};i++)); do
        m="${matrices_footprint[i]}"
        for d in "${validation_dirs[@]}"; do
            if [[ -f "${d}/${m}" ]]; then
                echo "${d}/${m}"
                break
            fi
        done
    done
) )

matrices_reordered=(
    pwtk.mtx

    # pwtk.mtx
    # TSOPF_RS_b2383.mtx
    # ldoor.mtx
    # bone010.mtx
    # audikw_1.mtx
    # wikipedia-20051105.mtx
    # kron_g500-logn18.mtx
    # GL7d20.mtx
    # sx-stackoverflow.mtx
    # dgreen.mtx
)
matrices_reordered=( $(
    for ((i=0;i<${#matrices_reordered[@]};i++)); do
        m="${matrices_reordered[i]}"
        for d in "${validation_dirs[@]}"; do
            if [[ -f "${d}/${m}" ]]; then
                echo "${d}/${m}"
                break
            fi
        done
    done
) )

export SUFFIX=""
# export SUFFIX="_RCM"
# export SUFFIX="_sorted_cols"
matrices_good_vs_bad=(
    # DGAL
    # spal_004.mtx
    # ldoor.mtx
    # dielFilterV2real.mtx
    # af_shell10.mtx
    # nv2.mtx
    # boneS10.mtx
    # circuit5M.mtx
    # Hook_1498.mtx
    # Geo_1438.mtx
    # Serena.mtx
    # vas_stokes_2M.mtx
    # bone010.mtx
    # audikw_1.mtx
    # Long_Coup_dt0.mtx
    # Long_Coup_dt6.mtx
    # dielFilterV3real.mtx
    # nlpkkt120.mtx
    # cage15.mtx
    # ML_Geer.mtx
    # Flan_1565.mtx
    # Cube_Coup_dt0.mtx
    # Cube_Coup_dt6.mtx
    # Bump_2911.mtx
    # vas_stokes_4M.mtx
    # nlpkkt160.mtx
    # HV15R.mtx
    # Queen_4147.mtx
    # stokes.mtx
    # nlpkkt200.mtx


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
    # ldoor.mtx
    # dielFilterV2real.mtx
    # circuit5M.mtx
    # soc-LiveJournal1.mtx
    # bone010.mtx
    # audikw_1.mtx
    # cage15.mtx

    # pwtk${SUFFIX}.mtx
    # crankseg_2${SUFFIX}.mtx

    # pwtk${SUFFIX}.mtx
    # # crankseg_2${SUFFIX}.mtx
    # # Si41Ge41H72${SUFFIX}.mtx
    # TSOPF_RS_b2383${SUFFIX}.mtx
    # # in-2004${SUFFIX}.mtx
    # # Ga41As41H72${SUFFIX}.mtx
    # ldoor${SUFFIX}.mtx
    # bone010${SUFFIX}.mtx
    # audikw_1${SUFFIX}.mtx

    # # eu-2005${SUFFIX}.mtx
    # wikipedia-20051105${SUFFIX}.mtx
    # kron_g500-logn18${SUFFIX}.mtx
    # # human_gene1${SUFFIX}.mtx
    # # delaunay_n22${SUFFIX}.mtx
    # GL7d20${SUFFIX}.mtx
    # sx-stackoverflow${SUFFIX}.mtx
    # dgreen${SUFFIX}.mtx
    # # soc-LiveJournal1${SUFFIX}.mtx

)
matrices_good_vs_bad=( $(
    for ((i=0;i<${#matrices_good_vs_bad[@]};i++)); do
        m="${matrices_good_vs_bad[i]}"
        for d in "${validation_dirs[@]}"; do
            if [[ -f "${d}/${m}" ]]; then
                echo "${d}/${m}"
                break
            fi
        done
    done
) )


matrices_validation_loop=()
for ((i=0;i<${#matrices_validation[@]};i++)); do
    path="${matrices_validation[i]}"
    dir="$(dirname "${path}")"
    filename="$(basename "${path}")"
    base="${filename%.*}"
    ext="${filename#${filename%.*}}"
    n=128
    for ((j=0;j<n;j++)); do
        matrices_validation_loop+=( "${matrices_validation_artificial_twins["$base"]}" )
    done
    matrices_validation_loop+=( "${matrices_validation[i]}" )
done



bench()
{
    declare args=("$@")
    declare prog="${args[0]}"
    declare prog_args=("${args[@]:1}")
    declare t

    for t in $cores
    do
        export OMP_NUM_THREADS="$t"

        while :; do
            # if [[ "$prog" == *"spmv_sparsex.exe"* ]]; then
                # # since affinity is set with the runtime variable, just reset it to "0" so no warnings are displayed, and reset it after execution of benchmark (for other benchmarks to run)
                # # mt_conf="${GOMP_CPU_AFFINITY}"
                # export GOMP_CPU_AFFINITY_backup="${GOMP_CPU_AFFINITY}"
                # export GOMP_CPU_AFFINITY="0"
                # mt_conf=$(seq -s ',' 0 1 "$(($t-1))")
                # if ((!USE_ARTIFICIAL_MATRICES)); then
                    # "$prog" "${prog_args[@]}" -t -o spx.rt.nr_threads=$t -o spx.rt.cpu_affinity=${mt_conf} -o spx.preproc.xform=all #-v  2>'tmp.err'
                # else
                    # prog_args2="${prog_args[@]}"
                    # "$prog" -p "${prog_args2[@]}" -t -o spx.rt.nr_threads=$t -o spx.rt.cpu_affinity=${mt_conf} -o spx.preproc.xform=all #-v  2>'tmp.err'
                # fi
                # export GOMP_CPU_AFFINITY="${GOMP_CPU_AFFINITY_backup}"
            # elif [[ "$prog" == *"spmv_sell-C-s.exe"* ]]; then
            if [[ "$prog" == *"spmv_sell-C-s_d.exe"* ]]; then
                if ((!USE_ARTIFICIAL_MATRICES)); then
                    "$prog" -c $OMP_NUM_THREADS -m "${prog_args[@]}" -f SELL-32-1  2>'tmp.err'
                    ret="$?"
                else
                    prog_args2="${prog_args[@]}"
                    "$prog" -c $OMP_NUM_THREADS --artif_args="${prog_args2[@]}" -f SELL-32-1  2>'tmp.err'
                    ret="$?"
                fi
            elif [[ "$prog" == *"spmv_lcm_d.exe" ]]; then
                echo numactl -i "$numa_nodes"
                numactl -i "$numa_nodes" "$prog" "${prog_args[@]}"  2>'tmp.err'
            else
                # If a GPU kernel is tested, then set env variable accordingly. This is used in spmv_bench, so that the warm up is performed 1000 times, instead of just once (CPU warm up)
                if [[ "$prog" == *"cuda"* ]] || [[ "$prog" == *"cusparse"* ]]; then
                    export GPU_KERNEL=1
                else
                    export GPU_KERNEL=0
                fi

                # "$prog" 4690000 4 1.6 normal random 1 14  2>'tmp.err'

                # numactl -i all "$prog" "${prog_args[@]}"  2>'tmp.err'

                echo '------------------------'
                export PATH=${CUDA_PATH}/bin:$PATH
                echo "${prog_args[0]}"
                mtx_name=$(basename "${prog_args[0]}")
                # mtx_name=artificial_${mtx_name%.mtx}
                mtx_name=${mtx_name%.mtx}
                
                prog_name=$(basename "$prog")
                prog_name=${prog_name#"spmv_"}
                # prog_name=${prog_name#"spmv_csr_cuda_"}
                prog_name=${prog_name%"_nv_d.exe"}
                export PROGG=${prog_name}
                
                echo "${mtx_name}_${prog_name}"
                # compute-sanitizer --tool initcheck --track-unused-memory "$prog" "${prog_args[@]}"  2>'tmp.err'
                # compute-sanitizer --tool memcheck "$prog" "${prog_args[@]}"  2>'tmp.err'
                # ncu -o ./out_logs/reports/ncu_reports/WindowSize_RCM_ncu_report_${mtx_name}_${prog_name} -f --print-summary=per-kernel --section={ComputeWorkloadAnalysis,InstructionStats,LaunchStats,MemoryWorkloadAnalysis,MemoryWorkloadAnalysis_Chart,MemoryWorkloadAnalysis_Tables,Occupancy,SchedulerStats,SourceCounters,SpeedOfLight,SpeedOfLight_RooflineChart,WarpStateStats} "$prog" "${prog_args[@]}"  2>'tmp.err'
                # nsys profile -o ./out_logs/reports/nsys_reports/nsys_report_${mtx_name}_${prog_name} -f true -t cuda,cublas --cuda-memory-usage=true --stats=true -w true "$prog" "${prog_args[@]}"  2>'tmp.err'
                "$prog" "${prog_args[@]}"  2>'tmp.err'
                
                # perf stat -e fp_dp_spec,fp_fixed_ops_spec,fp_hp_spec,fp_scale_ops_spec,fp_sp_spec "$prog" "${prog_args[@]}"  2>'tmp.err'

                # perf record -e instructions:u "$prog" "${prog_args[@]}"  2>'tmp.err'
                # perf report | grep -E "libarmpl" 2>'tmp.err'
            fi
            if ((output_to_files)); then   # If outputing to files, also print stderr to stdout.
                cat 'tmp.err'
            fi
            if ((!ret || !force_retry_on_error)); then      # If not retrying then print the error text to be able to notice it.
                cat 'tmp.err' >&2
                break
            fi
            echo "ERROR: Program exited with error [${ret}], retrying."
        done
    done

    rm 'tmp.err'
}


matrices=(

    # "${matrices_openFoam[@]}"
    # "${matrices_validation[@]}"
    
    # "${matrices_validation_GPU_PART[@]}"
    # "${matrices_validation_CPU_PART[@]}"
    # "${matrices_paper_csr_rv[@]}"
    # "${matrices_compression_small[@]}"
    # "${matrices_compression[@]}"
    # "${matrices_M3E[@]}"
    # "${matrices_cg[@]}"

    # "${matrices_underperform_gpu[@]}"
    # "${matrices_goodperform_gpu[@]}"
    
    # "${matrices_underperform_gpu_GPU_PART[@]}"
    # "${matrices_underperform_gpu_CPU_PART[@]}"
    
    # "${matrices_goodperform_gpu_GPU_PART[@]}"
    # "${matrices_goodperform_gpu_CPU_PART[@]}"

    # "${matrices_footprint[@]}"
    # "${matrices_reordered[@]}"
    # "${matrices_before_RCM[@]}"
    # "${matrices_RCM[@]}"
    
    "${matrices_short_rows[@]}"
    # "${matrices_long_rows[@]}"

    # "${matrices_good_vs_bad[@]}"

    # "$path_tamu"/matrices/kron_g500-logn18/kron_g500-logn18.mtx
    # '/home/jim/Synced_Folder/Data/kmeans/matrices/kron_g500-logn18_c1024_wl26214_reordered_rows.mtx'
    # '/home/jim/Synced_Folder/Data/kmeans/matrices/kron_g500-logn18_c1024_wl26214_reordered_both.mtx'
    # '/home/jim/Synced_Folder/Data/kmeans/matrices/kron_g500-logn18_reordered.mtx'
    # '/home/jim/Synced_Folder/Data/kmeans/matrices/kron_g500-logn18_reordered_0.mtx'
    # '/home/jim/Synced_Folder/lib/C/tests/kmeans/kron_g500-logn18_reordered.mtx'

    # "$path_tamu"/matrices/cop20k_A/cop20k_A.mtx
    # '/home/jim/Synced_Folder/Data/kmeans/matrices/cop20k_A_reordered_tom_down_inner_clusters.mtx'
    # '/home/jim/Synced_Folder/lib/C/tests/kmeans/cop20k_A_reordered.mtx'
    # '/home/jim/Synced_Folder/Data/kmeans/matrices/cop20k_A_reordered.mtx'
    # '/home/jim/Synced_Folder/Data/kmeans/matrices/cop20k_A_reordered_top_down.mtx'
    # '/home/jim/Synced_Folder/Data/kmeans/matrices/cop20k_A_reordered_top_down_incr_clusters.mtx'

    # "$path_tamu"/matrices/ASIC_680k/ASIC_680k.mtx
    # '682862 682862 5.6699201303 659.8073579974 normal random 0.3746622132 69710.5639935502 0.6690077130 0.8254737741 14 ASIC_680k'

    # nr_rows nr_cols avg_nnz_per_row std_nnz_per_row distribution placement bw           skew          avg_num_neighbours cross_row_similarity seed
    # ' 16783   16783   555.5280343204  1233.5202594143 normal       random    1            0             0                  0                    14 gupta3-1'
    # ' 16783   16783   555.5280343204  1233.5202594143 normal       random    1            25.4109083495 0                  0                    14 gupta3-2'
    # ' 16783   16783   555.5280343204  1233.5202594143 normal       random    0.5718415058 25.4109083495 0                  0                    14 gupta3-3'
    # ' 16783   16783   555.5280343204  1233.5202594143 normal       random    0.5718415058 25.4109083495 1.9016586927       0                    14 gupta3-4'
    # ' 16783   16783   555.5280343204  1233.5202594143 normal       random    0.5718415058 25.4109083495 1.9016586927       0.9767488489         14 gupta3-5'
    # "${path_validation}"/gupta3.mtx

    # "${matrices_validation_artificial_twins[@]}"
    # "${matrices_validation_loop[@]}"

    # "$path_other"/simple.mtx
    # "$path_other"/simple_symmetric.mtx

    # /home/jim/Documents/Synced_Documents/other/ASIC_680k.mtx

    # "$path_openFoam"/100K.mtx
    # "$path_openFoam"/600K.mtx
    # "$path_openFoam"/TestMatrices/HEXmats/5krows/processor0
    # "${matrices_openFoam_own_neigh[@]}"

)


if ((!USE_ARTIFICIAL_MATRICES)); then
    prog_args=("${matrices[@]}")
else
    prog_args=()
    tmp=()
    for f in "${artificial_matrices_files[@]}"; do
        IFS=$'\n' read -d '' -a tmp < "$f"
        prog_args+=("${tmp[@]}")
    done
fi

# prog_args=(

    # '28508159 28508159 5 1.6667 normal random 0.05 0 0.05 0.05 14'              # This bugs at 128 threads with mkl and mkl_sparse_set_mv_hint() for some reason.

    # '5154859 5154859 19.24389 5.73672 normal random 0.21196 1.44233 0.19755 1.03234 14'
    # '952203 952203 48.85772782 11.94657153 normal random 0.2042067138 0.5760045224 1.79674 0.906047 14 ldoor'
# )

temp_labels=( $(printf "%s\n" /sys/class/hwmon/hwmon*/temp*_label | sort) )
temp_inputs=( ${temp_labels[@]/label/input} )

for format_name in "${!progs[@]}"; do
    prog="${progs["$format_name"]}"

    if ((output_to_files)); then
    	mkdir -p out_logs
        > out_logs/"${format_name}.out"
        exec 1>>out_logs/"${format_name}.out"
        > out_logs/"${format_name}.csv"
        exec 2>>out_logs/"${format_name}.csv"
    fi

    echo "$config_str"

    echo "program: $prog"
    echo "number of matrices: ${#prog_args[@]}"

    # Just print the output labels first.
    "$prog"

    rep=1
    # rep=4
    # rep=5
    # rep=16
    # rep=1024


    LEVEL3_CACHE_SIZE="$(getconf LEVEL3_CACHE_SIZE)"
    csrcv_num_packet_vals=(
        # $((2**6))
        # $((2**7))
        # $((2**10))
        $((2**14))
        # $((2**17))
        # $((2**20))
        # $((2**12))
        # $((LEVEL3_CACHE_SIZE / 8 / 8 / 16))
    )

    # if [[ "$prog" == *'spmv_csr_cv'* ]]; then
        # csrcv_num_packet_vals=( $( declare -i i; for (( i=64;i<=2**24;i*=2 )); do echo "$i"; done ) )
    # fi

    ts1="$(date '+%s')"

    for ((i=0;i<rep;i++)); do
        for a in "${prog_args[@]}"
        do

            rep_in=1
            # rep_in=10

            for packet_vals in "${csrcv_num_packet_vals[@]}"; do
                export CSRCV_NUM_PACKET_VALS="$packet_vals"

                printf "Temps: " >&1
                for ((k=0;k<${#temp_labels[@]};k++)); do
                    printf "%s %s " $(cat ${temp_labels[k]}) $(cat ${temp_inputs[k]}) >&1
                done
                echo >&1

                echo "File: $a"
                bench "$prog" $a

            done
        done
    done

    ts2="$(date '+%s')"
    printf "\n\nTotal time = $(( (ts2 - ts1) / 60 )) minutes $(( (ts2 - ts1) % 60 )) seconds\n"

done

