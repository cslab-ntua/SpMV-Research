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


<<<<<<< Updated upstream
# GOMP_CPU_AFFINITY pins the threads to specific cpus, even when assigning more cores than threads.
# e.g. with 'GOMP_CPU_AFFINITY=0,1,2,3' and 2 threads, the threads are pinned: t0->core0 and t1->core1.
export GOMP_CPU_AFFINITY="$cpu_affinity"

# when running on LUMI-G (dev-g, small-g, standard-g), need to manually set GOMP_CPU_AFFINITY!!! Cores 0, 8, ..., 56 are disabled
if [[ $(hostname) == nid* ]]; then
    # Extract the numerical part and convert it to an integer
    nid=$((10#$(hostname | grep -oP '\d{6}')))
    # Function to check if a number is within a given range
    in_range() {
        (( $1 >= $2 && $1 <= $3 ))
    }

    # dev-g: [5002-5025], [7954-7977]
    # small-g: [5026-5123], [7852-7951]
    # standard-g: [5124-7851]
    if in_range $nid 5002 5025 || in_range $nid 7954 7977 || in_range $nid 5026 5123 || in_range $nid 7852 7951 || in_range $nid 5124 7851; then # dev-g
        # Define the numbers to be removed from GOMP_CPU_AFFINITY (the first core from each of the 8 L3 regions of the LUMI-G CPU)
        numbers_to_remove=(0 8 16 24 32 40 48 56)
        # Remove specified numbers from GOMP_CPU_AFFINITY
        for num in "${numbers_to_remove[@]}"; do
            export GOMP_CPU_AFFINITY=$(echo $GOMP_CPU_AFFINITY | sed -E "s/(^|,)$num(,|$)/\1\2/g")
        done
        # Remove any leading or trailing commas
        export GOMP_CPU_AFFINITY=$(echo $GOMP_CPU_AFFINITY | sed 's/^,//' | sed 's/,$//')
        # Remove any duplicate commas
        export GOMP_CPU_AFFINITY=$(echo $GOMP_CPU_AFFINITY | tr -s ',')
    fi
fi
export XLSMPOPTS="PROCS=$cpu_affinity"

=======
>>>>>>> Stashed changes
lscpu | grep -q -i amd
if (($? == 0)); then
    export MKL_DEBUG_CPU_TYPE=5
fi
# export MKL_ENABLE_INSTRUCTIONS=AVX512
export MKL_VERBOSE=1

export LD_LIBRARY_PATH="${AOCL_PATH}/lib:${MKL_PATH}/lib/intel64:${LD_LIBRARY_PATH}"
export LD_LIBRARY_PATH="${LD_LIBRARY_PATH}:${BOOST_LIB_PATH}:${LLVM_LIB_PATH}:${SPARSEX_LIB_PATH}"
# export LD_LIBRARY_PATH="${LD_LIBRARY_PATH}:${SPARSEX_FLOAT_LIB_PATH}"
export LD_LIBRARY_PATH="${LD_LIBRARY_PATH}:/home/jim/lib/gcc/gcc_12/lib64"
export LD_LIBRARY_PATH="${LD_LIBRARY_PATH}:/various/dgal/gcc/gcc-12.2.0/gcc_bin/lib64"
<<<<<<< Updated upstream
export LD_LIBRARY_PATH="${LD_LIBRARY_PATH}:/opt/cray/pe/gcc/12.2.0/snos/lib64"
export LD_LIBRARY_PATH="${LD_LIBRARY_PATH}:${CUDA_PATH}/lib64"
export LD_LIBRARY_PATH="${LD_LIBRARY_PATH}:${ROCM_PATH}/lib64"
=======
export LD_LIBRARY_PATH="${LD_LIBRARY_PATH}:/local/pmpakos/arm-compiler/gcc-13.2.0_Ubuntu-22.04/lib64"
export LD_LIBRARY_PATH="${LD_LIBRARY_PATH}:${CUDA_PATH}/lib64"
export LD_LIBRARY_PATH="${LD_LIBRARY_PATH}:${HOME}/spack/23.03/0.20.0/intel-tbb-2021.9.0-xxzbl3f/lib64"
export LD_LIBRARY_PATH="${LD_LIBRARY_PATH}:/opt/intel/oneapi/tbb/latest/lib"
>>>>>>> Stashed changes

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


mapfile -t -d $'\n' matrices_tamu_real < <(
    awk -F ',' '
    BEGIN { LINT = "fatal" }
    function cond(flag) { if (!flag) next }
    NR==1 { for (i=1;i<=NF;i++) df[$i] = i ; next}
    // {
        # filename,type,symmetry,nrows,ncols,nnz_file,nnz_diag,nnz,csr_size_sym,csr_size
        cond($df["type"] == "real")
        cond($df["csr_size"] < 6000)
        # cond($df["csr_size"] >= 1)
        printf("%s\n", $df["filename"])
    }
    ' "${script_dir}"/datasets/tamu_sorted_by_size.csv
)
for ((i=0;i<${#matrices_tamu_real[@]};i++)); do
    m="${matrices_tamu_real[i]}"
    matrices_tamu_real[i]="${path_tamu}/matrices/${m}/${m}.mtx"
done


mapfile -t -d $'\n' matrices_tamu_real_symmetric < <(
    awk -F ',' '
    BEGIN { LINT = "fatal" }
    function cond(flag) { if (!flag) next }
    NR==1 { for (i=1;i<=NF;i++) df[$i] = i ; next}
    // {
        # filename,type,symmetry,nrows,ncols,nnz_file,nnz_diag,nnz,csr_size_sym,csr_size
        cond($df["type"] == "real")
        cond($df["csr_size"] < 6000)
        # cond($df["csr_size"] >= 1)
        cond($df["symmetry"] == "symmetric")
        printf("%s\n", $df["filename"])
    }
    ' "${script_dir}"/datasets/tamu_sorted_by_size.csv
)
for ((i=0;i<${#matrices_tamu_real_symmetric[@]};i++)); do
    m="${matrices_tamu_real_symmetric[i]}"
    matrices_tamu_real_symmetric[i]="${path_tamu}/matrices/${m}/${m}.mtx"
done

# printf "%s\n" "${matrices_tamu_real_symmetric[@]}"
# exit


matrices_validation=(

<<<<<<< Updated upstream
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
    mawi_201512012345.mtx
    ldoor.mtx
    dielFilterV2real.mtx
    circuit5M.mtx
    soc-LiveJournal1.mtx
    bone010.mtx
    audikw_1.mtx
    cage15.mtx
    kmer_V2a.mtx
=======
    # scircuit
    # mac_econ_fwd500
    # raefsky3
    # rgg_n_2_17_s0
    # bbmat
    # appu
    # conf5_4-8x8-15
    # mc2depi
    # rma10
    # cop20k_A
    # thermomech_dK
    # webbase-1M
    # cant
    # ASIC_680k
    # roadNet-TX
    # pdb1HYS
    # TSOPF_RS_b300_c3
    # Chebyshev4
    # consph
    # com-Youtube
    # rajat30
    # radiation
    # Stanford_Berkeley
    # shipsec1
    # PR02R
    # CurlCurl_2
    # gupta3
    # mip1
    # rail4284
    # pwtk
    # crankseg_2
    # Si41Ge41H72
    # TSOPF_RS_b2383
    # in-2004
    # Ga41As41H72
    # eu-2005
    # wikipedia-20051105
    # kron_g500-logn18
    # rajat31
    # human_gene1
    # delaunay_n22
    # GL7d20
    # sx-stackoverflow
    # dgreen
    # mawi_201512012345
    # ldoor
    # dielFilterV2real
    # circuit5M
    # soc-LiveJournal1
    # bone010
    # audikw_1
    # cage15
    # kmer_V2a
>>>>>>> Stashed changes

)
matrices_validation_tamu=( ${matrices_validation[@]} )
for ((i=0;i<${#matrices_validation_tamu[@]};i++)); do
    m="${matrices_validation_tamu[i]}"
    matrices_validation_tamu[i]="${path_tamu}/matrices/${m}/${m}.mtx"
done
validation_dirs=(
    "${path_validation}"
    "${path_validation}/new_matrices" 
)
matrices_validation=( $(
    for ((i=0;i<${#matrices_validation[@]};i++)); do
        m="${matrices_validation[i]}.mtx"
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
    mac_econ_fwd500
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

)
for ((i=0;i<${#matrices_compression_small[@]};i++)); do
    m="${matrices_compression_small[i]}"
    matrices_compression_small[i]="${path_tamu}/matrices/${m}/${m}.mtx"
done


matrices_compression_medium=(

    CoupCons3D
    Transport
    Freescale2
    human_gene1
    F1
    CurlCurl_4
    FullChip
    cage14
    ML_Laplace
    nd24k
    Fault_639
    mouse_gene
    nlpkkt80
    vas_stokes_1M
    ss
    inline_1
    PFlow_742
    RM07R
    dgreen
    Emilia_923
    Hardesty3

)
for ((i=0;i<${#matrices_compression_medium[@]};i++)); do
    m="${matrices_compression_medium[i]}"
    matrices_compression_medium[i]="${path_tamu}/matrices/${m}/${m}.mtx"
done


matrices_compression_medium_symmetric=(

    human_gene1
    F1
    CurlCurl_4
    nd24k
    Fault_639
    mouse_gene
    nlpkkt80
    inline_1
    Emilia_923

)
for ((i=0;i<${#matrices_compression_medium_symmetric[@]};i++)); do
    m="${matrices_compression_medium_symmetric[i]}"
    matrices_compression_medium_symmetric[i]="${path_tamu}/matrices/${m}/${m}.mtx"
done


matrices_compression=(

    # spal_004
    # ldoor
    # dielFilterV2real
    af_shell10
    # nv2
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

    # nlpkkt240  # SparseX OOM.
    # MOLIERE_2016   # This crashes with OOM error.

)
for ((i=0;i<${#matrices_compression[@]};i++)); do
    m="${matrices_compression[i]}"
    matrices_compression[i]="${path_tamu}/matrices/${m}/${m}.mtx"
done


matrices_compression_symmetric=(

    ldoor
    dielFilterV2real
    af_shell10
    boneS10
    Hook_1498
    Geo_1438
    Serena
    bone010
    audikw_1
    Long_Coup_dt0
    Long_Coup_dt6
    dielFilterV3real
    nlpkkt120
    Flan_1565
    Cube_Coup_dt0
    Cube_Coup_dt6
    Bump_2911
    nlpkkt160
    Queen_4147
    nlpkkt200

    # nlpkkt240     # SparseX OOM

)
for ((i=0;i<${#matrices_compression_symmetric[@]};i++)); do
    m="${matrices_compression_symmetric[i]}"
    matrices_compression_symmetric[i]="${path_tamu}/matrices/${m}/${m}.mtx"
done


matrices_M3E=(

    Heel_1138
    Utemp20m
    guenda11m
    agg14m
    rtanis44m

    # tripod24m     # SparseX OOM
    # M20           # SparseX OOM
    # Pflow73m      # OOM

)
for ((i=0;i<${#matrices_M3E[@]};i++)); do
    m="${matrices_M3E[i]}"
    matrices_M3E[i]="$path_M3E"/"$m"/"$m".mtx
done


matrices_M3E_symmetric=(

    guenda11m
    agg14m
    rtanis44m

    # tripod24m

)
for ((i=0;i<${#matrices_M3E_symmetric[@]};i++)); do
    m="${matrices_M3E_symmetric[i]}"
    matrices_M3E_symmetric[i]="$path_M3E"/"$m"/"$m".mtx
done


matrices_compression_all=(

    "${matrices_compression_medium[@]}"
    "${matrices_compression[@]}"

    # "${matrices_M3E[@]}"

)

matrices_compression_all_symmetric=(

    "${matrices_compression_medium_symmetric[@]}"
    "${matrices_compression_symmetric[@]}"

    # "${matrices_M3E_symmetric[@]}"

)


matrices_cg=(

    # ldoor
    dielFilterV2real
    # boneS10
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


matrices_bicg=(

    # spal_004

    ldoor
    dielFilterV2real
    af_shell10
    nv2
    boneS10
    circuit5M
    Hook_1498
    Geo_1438
    Serena
    vas_stokes_2M
    bone010
    audikw_1
    Long_Coup_dt0
    Long_Coup_dt6
    dielFilterV3real
    nlpkkt120
    cage15
    ML_Geer
    Flan_1565
    Cube_Coup_dt0
    Cube_Coup_dt6
    Bump_2911
    vas_stokes_4M
    nlpkkt160
    HV15R
    Queen_4147
    stokes
    nlpkkt200

)
for ((i=0;i<${#matrices_bicg[@]};i++)); do
    m="${matrices_bicg[i]}"
    matrices_bicg[i]="${path_tamu}/matrices/${m}/${m}.mtx"
done


matrices_underperform_gpu=(

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
    kron_g500-logn18.mtx
    # wikipedia-20051105.mtx
    # kron_g500-logn21.mtx
    # kron_g500-logn20.mtx
    # com-LiveJournal.mtx
    # kron_g500-logn19.mtx
    # ljournal-2008.mtx
    # wiki-topcats.mtx

    # kmer_V2a_sorted_cols.mtx
    # wikipedia-20070206_sorted_cols.mtx
    # sx-stackoverflow_sorted_cols.mtx
    # wikipedia-20061104_sorted_cols.mtx
    # wikipedia-20060925_sorted_cols.mtx
    # GL7d20_sorted_cols.mtx
    # GL7d19_sorted_cols.mtx
    # GL7d17_sorted_cols.mtx
    # soc-LiveJournal1_sorted_cols.mtx
    # soc-Pokec_sorted_cols.mtx
    # GL7d21_sorted_cols.mtx
    # GL7d18_sorted_cols.mtx
    # dgreen_sorted_cols.mtx
    # kron_g500-logn18_sorted_cols.mtx
    # wikipedia-20051105_sorted_cols.mtx
    # kron_g500-logn21_sorted_cols.mtx
    # kron_g500-logn20_sorted_cols.mtx
    # com-LiveJournal_sorted_cols.mtx
    # kron_g500-logn19_sorted_cols.mtx
    # ljournal-2008_sorted_cols.mtx
    # wiki-topcats_sorted_cols.mtx
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
    declare array_cores=( $cores )
    declare array_affinities=()
    declare c
    declare i

    export LEVEL1_DCACHE_LINESIZE="$(read v < /sys/devices/system/cpu/cpu0/cache/index0/coherency_line_size; echo ${v%K})"
    export LEVEL1_DCACHE_SIZE="$(read v < /sys/devices/system/cpu/cpu0/cache/index0/size; echo $((${v%K} * 1024)) )"
    export LEVEL2_CACHE_SIZE="$(read v < /sys/devices/system/cpu/cpu0/cache/index2/size; echo $((${v%K} * 1024)) )"
    export LEVEL3_CACHE_SIZE="$(read v < /sys/devices/system/cpu/cpu0/cache/index3/size; echo $((${v%K} * 1024)) )"
    export NUM_CPUS="$(ls /sys/devices/system/cpu/ | grep -c 'cpu[[:digit:]]\+')"
    export LEVEL3_CACHE_CPUS_PER_NODE="$(for range in $(cat /sys/devices/system/cpu/cpu0/cache/index3/shared_cpu_list | tr ',' ' '); do mapfile -t -d '-' a < <(printf "$range"); seq "${a[@]}"; done | wc -l)"
    export LEVEL3_CACHE_NUM_NODES="$(( NUM_CPUS / LEVEL3_CACHE_CPUS_PER_NODE ))"
    export LEVEL3_CACHE_SIZE_TOTAL="$(( LEVEL3_CACHE_SIZE * LEVEL3_CACHE_NUM_NODES ))"

    mapfile -t -d ':' array_affinities < <(printf "$cpu_affinity")
    for ((i=0;i<${#array_cores[@]};i++)); do
        c="${array_cores[i]}"
        aff="${array_affinities[i]}"

        export OMP_NUM_THREADS="$c"

        # GOMP_CPU_AFFINITY pins the threads to specific cpus, even when assigning more cores than threads.
        # e.g. with 'GOMP_CPU_AFFINITY=0,1,2,3' and 2 threads, the threads are pinned: t0->core0 and t1->core1.
        export GOMP_CPU_AFFINITY="$aff"
        export XLSMPOPTS="PROCS=$aff"


        while :; do
            # if [[ "$prog" == *"spmv_sparsex.exe"* ]]; then
                # # since affinity is set with the runtime variable, just reset it to "0" so no warnings are displayed, and reset it after execution of benchmark (for other benchmarks to run)
                # # mt_conf="${GOMP_CPU_AFFINITY}"
                # export GOMP_CPU_AFFINITY_backup="${GOMP_CPU_AFFINITY}"
                # export GOMP_CPU_AFFINITY="0"
                # mt_conf=$(seq -s ',' 0 1 "$(($c-1))")
                # if ((!USE_ARTIFICIAL_MATRICES)); then
                    # "$prog" "${prog_args[@]}" -t -o spx.rt.nr_threads=$c -o spx.rt.cpu_affinity=${mt_conf} -o spx.preproc.xform=all #-v  2>'tmp.err'
                # else
                    # prog_args2="${prog_args[@]}"
                    # "$prog" -p "${prog_args2[@]}" -t -o spx.rt.nr_threads=$c -o spx.rt.cpu_affinity=${mt_conf} -o spx.preproc.xform=all #-v  2>'tmp.err'
                # fi
                # export GOMP_CPU_AFFINITY="${GOMP_CPU_AFFINITY_backup}"
            # elif [[ "$prog" == *"spmv_sell-C-s.exe"* ]]; then
            if [[ "$prog" == *"spmv_sell-C-s_d.exe"* ]]; then
                if ((!USE_ARTIFICIAL_MATRICES)); then
                    "$prog" -c "$OMP_NUM_THREADS" -m "${prog_args[@]}" -f SELL-32-1  2>'tmp.err'
                    ret="$?"
                else
                    prog_args2="${prog_args[@]}"
                    "$prog" -c "$OMP_NUM_THREADS" --artif_args="${prog_args2[@]}" -f SELL-32-1  2>'tmp.err'
                    ret="$?"
                fi
            elif [[ "$prog" == *"spmv_lcm_d.exe" ]]; then
                echo numactl -i "$numa_nodes"
                numactl -i "$numa_nodes" "$prog" "${prog_args[@]}"  2>'tmp.err'
            elif [[ "$prog" == *"spmv_cfs"* ]]; then
                export CFS_NUM_THREADS="$OMP_NUM_THREADS"
                # "$prog" "${prog_args[@]}" 1 128  2>'tmp.err'
                "$prog" "${prog_args[@]}" 1 2>'tmp.err'
            else
                # If a GPU kernel is tested, then set env variable accordingly. This is used in spmv_bench, so that the warm up is performed 1000 times, instead of just once (CPU warm up)
                if [[ "$prog" == *"cuda"* ]] || [[ "$prog" == *"cusparse"* ]] || [[ "$prog" == *"rocm"* ]] || [[ "$prog" == *"rocsparse"* ]]; then
                    export GPU_KERNEL=1
                else
                    export GPU_KERNEL=0
                fi

                # "$prog" 4690000 4 1.6 normal random 1 14  2>'tmp.err'

                # numactl -i all "$prog" "${prog_args[@]}"  2>'tmp.err'

                echo '------------------------'
                export PATH="${CUDA_PATH}/bin:$PATH"
                echo "${prog_args[0]}"
                mtx_name="$(basename "${prog_args[0]}")"
                # mtx_name="artificial_${mtx_name%.mtx}"
                mtx_name="${mtx_name%.mtx}"
                
                prog_name="$(basename "$prog")"
                prog_name="${prog_name#"spmv_"}"
                # prog_name="${prog_name#"spmv_csr_cuda_"}"
                prog_name="${prog_name%"_nv_d.exe"}"
                export PROGG="${prog_name}"
                
                echo "${mtx_name}_${prog_name}"
                # compute-sanitizer --tool initcheck --track-unused-memory --print-limit 10000000 "$prog" "${prog_args[@]}"  2>'tmp.err'
                # compute-sanitizer --tool memcheck --print-limit 10000000 "$prog" "${prog_args[@]}"  2>'tmp.err'
                # ncu -o ./out_logs/reports/ncu_reports/ncu_report_${mtx_name}_${prog_name} -f --print-summary=per-kernel --section={ComputeWorkloadAnalysis,InstructionStats,LaunchStats,MemoryWorkloadAnalysis,MemoryWorkloadAnalysis_Chart,MemoryWorkloadAnalysis_Tables,Occupancy,SchedulerStats,SourceCounters,SpeedOfLight,SpeedOfLight_RooflineChart,WarpStateStats} "$prog" "${prog_args[@]}"  2>'tmp.err'
                # nsys profile -o ./out_logs/reports/nsys_reports/nsys_report_${mtx_name}_${prog_name} -f true -t cuda,cublas --cuda-memory-usage=true --stats=true -w true "$prog" "${prog_args[@]}"  2>'tmp.err'
                "$prog" "${prog_args[@]}"  2>'tmp.err'
                # "$prog" "${prog_args[@]}"
                ret="$?"
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
<<<<<<< Updated upstream
    "${matrices_validation[@]}"
=======
    # "${matrices_validation[@]}"
    # "${matrices_validation_tamu[@]}"
>>>>>>> Stashed changes
    # "${matrices_paper_csr_rv[@]}"

    "${matrices_compression_small[@]}"
    # "${matrices_compression_medium[@]}"
    # "${matrices_compression_medium_symmetric[@]}"
    # "${matrices_compression[@]}"
    # "${matrices_compression_symmetric[@]}"
    # "${matrices_M3E[@]}"
    # "${matrices_compression_all[@]}"
    # "${matrices_compression_all_symmetric[@]}"

    # ${matrices_tamu_real[@]}
    # ${matrices_tamu_real_symmetric[@]}

    # "${matrices_cg[@]}"
<<<<<<< Updated upstream
    # "${matrices_underperform_gpu[@]}"
=======
    # "${matrices_bicg[@]}"

    # "${matrices_underperform_gpu[@]}"

    # "${path_tamu}/matrices/cop20k_A/cop20k_A.mtx"
    # "${path_tamu}/matrices/circuit204/circuit204.mtx"
    # "${path_tamu}/matrices/b1_ss/b1_ss.mtx"
    # "${path_tamu}/matrices/c-18/c-18.mtx"
    # "${path_tamu}/matrices/cant/cant.mtx"
    # '/home/jim/Synced_Folder/University/PHD/SPMV/data_analysis/test_matrices/test_symmetric.mtx'
>>>>>>> Stashed changes

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

    # "$path_openFoam"/100K-600K/100K.mtx
    # "$path_openFoam"/100K-600K/600K.mtx
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
    # rep=10
    # rep=16
    # rep=1024


    LEVEL3_CACHE_SIZE="$(getconf LEVEL3_CACHE_SIZE)"
    csrcv_num_packet_vals=(
        # $((2**6))
        # $((2**7))
        # $((2**10))
        $((2**14))
        # $((2**17))
        # $((2**24))
        # $((2**12))
        # $((LEVEL3_CACHE_SIZE / 8 / 8 / 16))
    )

    # if [[ "$prog" == *'spmv_div_'* ]]; then
        # csrcv_num_packet_vals=( $( declare -i i; for (( i=64;i<=2**20;i*=2 )); do echo "$i"; done ) )
    # fi

    ts1="$(date '+%s')"

    for ((i=0;i<rep;i++)); do
        for a in "${prog_args[@]}"; do

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

