#!/bin/bash

script_dir="$(dirname "$(readlink -e "${BASH_SOURCE[0]}")")"

find_valid_dir()
{
    local args=("$@")
    for p in "${args[@]}"; do
        if [[ -d "$p" ]]; then
            printf "$p"
            break
        fi
    done
}

calc_cpu_pinning()
{
    local cores="$1" max_cores="$2" cpu_pinning_step="$3" cpu_pinning_group_size="$4"
    local affinity thread_groups cycle_len i j
    affinity=''
    thread_groups="$(( cores / cpu_pinning_group_size ))"
    for ((i=0;i<thread_groups;i++)); do
        cycle_len="$(( max_cores / cpu_pinning_step ))"
        pos="$(( (i % cycle_len) * cpu_pinning_step + (i / cycle_len) % cpu_pinning_step ))"
        for ((j=0;j<cpu_pinning_group_size;j++)); do
            affinity="$affinity,$((pos + j))"
        done
    done
    affinity="${affinity#,}"
    printf "$affinity"
}


calc_numa_nodes()
{
    local cores="$1" max_cores="$2" affinity="$3"
    local -A dict
    local -a aff
    local -A nodes
    local -A nodes
    local i j c
    numactl_output="$(numactl -H)"
    avail="$(printf "$numactl_output" | awk '/available/ {printf("%d", $2)}')"
    dict=()
    for ((i=0;i<avail;i++)); do
        node_cores=( $(printf "$numactl_output" | awk '/node '"$i"' cpus:/ { for (i=4;i<=NF;i++) {printf("%s ", $i)} }') )   # $i in awk expands the 'i' awk variable and if a number prints the ith line token, else $0.
        for n in "${node_cores[@]}"; do
            dict["$n"]="$i"
        done
    done
    aff=(${affinity//,/' '})
    nodes=()
    for c in "${aff[@]}"; do
        if [[ -v dict["$c"] ]]; then
            nodes["${dict["$c"]}"]=1
        fi
    done
    nodes="$(printf "%s\n" "${!nodes[@]}" | sort -n | tr '\n' ',')"
    nodes="${nodes%,}"
    printf "${nodes}"
}


export SPARSEX_ROOT_DIR="${HOME}/lib"
# export SPARSEX_ROOT_DIR=/various/dgal/epyc1
# export SPARSEX_ROOT_DIR=/home/pmpakos/sparsex
# export SPARSEX_ROOT_DIR=/various/pmpakos/SPMV_BENCHMARKS/sparsex


declare -A conf_vars
conf_vars=(
    # ['PRINT_STATISTICS']=0
    ['PRINT_STATISTICS']=1

    ['USE_PROCESSES']=0
    # ['USE_PROCESSES']=1

    ['force_retry_on_error']=0
    # ['force_retry_on_error']=1

    ['output_to_files']=0
    # ['output_to_files']=1

    ['COOLDOWN']=0
    # ['COOLDOWN']=1

    ['VC_TOLERANCE']=0
    # ['VC_TOLERANCE']='1e-12'
    # ['VC_TOLERANCE']='1e-9'
    # ['VC_TOLERANCE']='1e-7'
    # ['VC_TOLERANCE']='1e-6'
    # ['VC_TOLERANCE']='1e-3'

    # Benchmark with the artificially generated matrices (1) or real matrices (0).
    ['USE_ARTIFICIAL_MATRICES']=0
    # ['USE_ARTIFICIAL_MATRICES']=1

    # Maximum number of the machine's cores.
    # ['max_cores']=160
    # ['max_cores']=256
    # ['max_cores']=128
    # ['max_cores']=64
    ['max_cores']=96
    # ['max_cores']=48
    # ['max_cores']=16
    # ['max_cores']=8

    # Cores / Threads to utilize. Use spaces to define a set of different thread numbers to benchmark.
    # ['cores']=1
    # ['cores']='1 2 4 8 16 32 64 128'
    # ['cores']='64 128'
    # ['cores']=128
    # ['cores']=64
    # ['cores']=48
    # ['cores']=32
    ['cores']=24
    # ['cores']=16
    # ['cores']=12
    # ['cores']=8
    # ['cores']=6
    # ['cores']=4
    # ['cores']=2
    # ['cores']=1
    # ['cores']='1 2 4 8 16 24 48'
    # ['cores']='24 48'
    # ['cores']='1 2 4 8'

    # Cpu pinning distance for contiguous thread ids, 1 means adjacent core numbers.
    ['cpu_pinning_step']=1
    # ['cpu_pinning_step']=4

    # Group size of threads pinned adjacently (e.g. for when hyperthreaded cores have contiguous ids).
    ['cpu_pinning_group_size']=1
    # ['cpu_pinning_group_size']=2
    # ['cpu_pinning_group_size']=3
    # ['cpu_pinning_group_size']=4

    # Thread pinning policy (auto-generated from the above).
    ['cpu_affinity']=''

    # Numa nodes of the selected cores.
    ['numa_nodes']=''

    # Rapl registers.
    ['RAPL_REGISTERS']='0'         # 1 socket : Epyc1, Gold
    # ['RAPL_REGISTERS']='0,1'       # 2 sockets: Epyc1, Gold

    # Path for the mkl library.
    # ['MKL_PATH']='/opt/intel/oneapi/mkl/latest'
    ['MKL_PATH']="$( options=(
                        # '/opt/intel/oneapi/mkl/latest'
                        # '/various/common_tools/intel_parallel_studio/compilers_and_libraries/linux/mkl'
                        '/various/pmpakos/intel/oneapi/mkl/2024.1'
                        # "${HOME}/spack/23.03/0.20.0/intel-oneapi-mkl-2023.1.0-cafkcjc/mkl/latest"
                    )
                    find_valid_dir "${options[@]}"
                )"

    ['AOCL_PATH']="$( options=(
                        '/opt/aoclsparse'
                        '/various/pmpakos/spmv_paper/aocl-sparse/build/release'
                        "${HOME}/aocl-sparse/library"
                    )
                    find_valid_dir "${options[@]}"
                )"

    ['CUDA_PATH']="$( options=(
                        '/usr/local/cuda-12.5'
                    )
                    find_valid_dir "${options[@]}"
                )"
    # SparseX ecosystem environment variables that have to be set.
    # These are environment variables that have to be set for SparseX to work
    # Need to install specific library versions
    ['BOOST_INC_PATH']="$( options=(
                        "${SPARSEX_ROOT_DIR}/boost_1_55_0/bin/include"
                        "${SPARSEX_ROOT_DIR}/boost_1_55_0/local/include"
                    )
                    find_valid_dir "${options[@]}"
                )"
    ['BOOST_LIB_PATH']="$( options=(
                        "${SPARSEX_ROOT_DIR}/boost_1_55_0/bin/lib"
                        "${SPARSEX_ROOT_DIR}/boost_1_55_0/local/lib"
                    )
                    find_valid_dir "${options[@]}"
                )"
    ['LLVM_INC_PATH']="$( options=(
                        "${SPARSEX_ROOT_DIR}/llvm-6.0.0/build/include"
                    )
                    find_valid_dir "${options[@]}"
                )"
    ['LLVM_LIB_PATH']="$( options=(
                        "${SPARSEX_ROOT_DIR}/llvm-6.0.0/build/lib"
                    )
                    find_valid_dir "${options[@]}"
                )"
    ['SPARSEX_CONF_PATH']="$( options=(
                        "${SPARSEX_ROOT_DIR}/sparsex/build/bin"
                        "${SPARSEX_ROOT_DIR}/build/bin"
                    )
                    find_valid_dir "${options[@]}"
                )"
    ['SPARSEX_INC_PATH']="$( options=(
                        "${SPARSEX_ROOT_DIR}/sparsex/build/include"
                        "${SPARSEX_ROOT_DIR}/build/include"
                    )
                    find_valid_dir "${options[@]}"
                )"
    ['SPARSEX_LIB_PATH']="$( options=(
                        "${SPARSEX_ROOT_DIR}/sparsex/build/lib"
                        "${SPARSEX_ROOT_DIR}/build/lib"
                    )
                    find_valid_dir "${options[@]}"
                )"

    # SELL-C-s ecosystem environment variables that have to be set
    # These are environment variables that have to be set for SELL-C-s to work
    ['GHOST_ROOT_DIR']='/various/pmpakos/epyc5_libs/'
    ['GHOST_APPS_ROOT_DIR']=${script_dir}'/spmv_code_bench/sell-C-s'

    # Path for the validation matrices.
    ['path_validation']="$( options=(
                        # "$HOME/Data/graphs/validation_matrices"
                        # "${script_dir}/../../../validation_matrices"
                        '/various/pmpakos/SpMV-Research/validation_matrices'
                        # '/various/pmpakos/SpMV-Research/validation_matrices/matrix_features/matrices'
                        # '/various/pmpakos/SpMV-Research/validation_matrices/download_matrices'
                    )
                    find_valid_dir "${options[@]}"
                )"

    # Path for the openFoam matrices.
    ['path_openFoam']="$( options=(
                        '/m100_work/ExaF_prod22/wp2-matrices'
                        "${HOME}/Data/graphs/matrices_openFoam"
                        '/zhome/academic/HLRS/xex/xexdgala/Data/graphs/openFoam'
                        '/various/dgal/graphs/matrices_openFoam'
                    )
                    find_valid_dir "${options[@]}"
                )"

    ['path_tamu']="${HOME}/Data/graphs/tamu"
    ['path_M3E']="${HOME}/Data/graphs/M3E-Matrix-Collection"

    ['ZFP_ROOT_DIR']="${HOME}/lib/zfp"
    ['FPZIP_ROOT_DIR']="${HOME}/lib/fpzip"
)

conf_vars['cpu_affinity']="$(calc_cpu_pinning "${conf_vars["cores"]}" "${conf_vars["max_cores"]}" "${conf_vars["cpu_pinning_step"]}" "${conf_vars["cpu_pinning_group_size"]}")"

if hash numactl 2>/dev/null; then
    conf_vars['numa_nodes']="$(calc_numa_nodes "${conf_vars["cores"]}" "${conf_vars["max_cores"]}" "${conf_vars["cpu_affinity"]}")"
else
    conf_vars['numa_nodes']="0"
fi


path_artificial="${script_dir}/../../../matrix_generation_parameters"


# Artificial matrices to benchmark.
artificial_matrices_files=(

    # Validation matrices artificial twins.
    # "$path_artificial"/validation_friends/twins_random.txt

    # Validation matrices artificial twins in a +-30% value space of each feature.
    "$path_artificial"/validation_matrices_10_samples_30_range_twins.txt

    # The synthetic dataset studied in the paper.
    # "$path_artificial"/synthetic_matrices_small_dataset.txt
    # "$path_artificial"/synthetic_matrices_small_dataset5.txt
)


# Artificial twins.
#     nr_rows
#     nr_cols
#     avg_nnz_per_row
#     std_nnz_per_row
#     distribution
#     placement
#     bw
#     skew
#     avg_num_neighbours
#     cross_row_similarity
#     seed
declare -A matrices_validation_artificial_twins
matrices_validation_artificial_twins=(

    ['scircuit']='170998 170998 5.6078784547 4.3921621102 normal random 0.2972525308 61.9471560146 0.8033653966 0.6330185674 14 scircuit'
    ['mac_econ_fwd500']='206500 206500 6.1665326877 4.4358653323 normal random 0.0019079565 6.1352901588 0.1766859930 0.3305090836 14 mac_econ_fwd500'
    ['raefsky3']='21200 21200 70.2249056604 6.3269998323 normal random 0.0662003204 0.1391969736 1.9160003439 0.9630195886 14 raefsky3'
    ['rgg_n_2_17_s0']='131072 131072 11.1198883057 3.3442200805 normal random 0.0074269939 1.5180109036 0.0339525189 0.0517407850 14 rgg_n_2_17_s0'
    ['bbmat']='38744 38744 45.7289386744 38.3953137311 normal random 0.0298941574 1.7553668126 1.2626258521 0.8537295305 14 bbmat'
    ['appu']='14000 14000 132.3645714286 36.4944587060 normal random 0.9835433061 1.2211381552 0.0202104145 0.0368171152 14 appu'
    ['conf5_4-8x8-15']='49152 49152 39.0000000000 0.0000000000 normal random 0.2446905772 0.0000000000 1.4415064103 0.8109475160 14 conf5_4-8x8-15'
    ['mc2depi']='525825 525825 3.9941520468 0.0763227705 normal random 0.0013421694 0.0014641288 0.4982970872 0.9989060048 14 mc2depi'
    ['rma10']='46835 46835 50.6886089463 27.7805960612 normal random 0.1877712645 1.8606032601 1.7197246336 0.8664076004 14 rma10'
    ['cop20k_A']='121192 121192 21.6543253680 13.7926624538 normal random 0.6230549795 2.7405921738 1.0958267078 0.6333998460 14 cop20k_A'
    ['thermomech_dK']='204316 204316 13.9305193915 1.4305130732 normal random 0.4446098972 0.4356966483 1.0096155333 0.5436708033 14 thermomech_dK'
    ['webbase-1M']='1000005 1000005 3.1055204724 25.3452097343 normal random 0.1524629001 1512.4339128576 0.3156807714 0.9360952482 14 webbase-1M'
    ['cant']='62451 62451 64.1684360539 14.0562609915 normal random 0.0086040976 0.2155508969 1.6157502290 0.9147287701 14 cant'
    ['ASIC_680k']='682862 682862 5.6699201303 659.8073579974 normal random 0.3746622132 69710.5639935502 0.6690077130 0.8254737741 14 ASIC_680k'
    ['roadNet-TX']='1393383 1393383 2.7582653154 1.0370252134 normal random 0.0049447584 3.3505604529 0.1617289219 0.3009518706 14 roadNet-TX'
    ['pdb1HYS']='36417 36417 119.3059560096 31.8603842202 normal random 0.1299377034 0.7098894877 1.8377583137 0.9317263989 14 pdb1HYS'
    ['TSOPF_RS_b300_c3']='42138 42138 104.7379799706 102.4431671584 normal random 0.6066963064 0.9954557082 1.9234585015 0.9921541433 14 TSOPF_RS_b300_c3'
    ['Chebyshev4']='68121 68121 78.9442462677 1061.4399700423 normal random 0.0453574243 861.9001253496 0.8959855226 0.9937056942 14 Chebyshev4'
    ['consph']='83334 83334 72.1251829985 19.0801941463 normal random 0.0698113380 0.1230474105 1.7133496826 0.8826334255 14 consph'
    ['com-Youtube']='1134890 1134890 5.2650459516 50.7543429107 normal random 0.1948996348 5460.3008631608 0.2820713048 0.4391655358 14 com-Youtube'
    ['rajat30']='643994 643994 9.5891840607 784.5798569082 normal random 0.3760753347 47421.8043929950 0.7909042638 0.6583270050 14 rajat30'
    ['radiation']='223104 223104 34.2337564544 15.1567107844 normal random 0.5965611386 101.1798470951 0.0265313273 0.1358445154 14 radiation'
    ['Stanford_Berkeley']='683446 683446 11.0957939618 284.8320877234 normal random 0.0071185557 7519.6875945489 1.3266041404 0.6526639388 14 Stanford_Berkeley'
    ['shipsec1']='140874 140874 55.4637761404 11.0748106399 normal random 0.0458755451 0.8390381452 1.7123850245 0.8734443677 14 shipsec1'
    ['PR02R']='161070 161070 50.8172595766 19.6982847015 normal random 0.0395857084 0.8104085259 1.2795415983 0.8688602245 14 PR02R'
    ['CurlCurl_2']='806529 806529 11.0619568546 0.9216130393 normal random 0.0279777294 0.1751989427 0.7155217412 0.5241313809 14 CurlCurl_2'
    ['gupta3']='16783 16783 555.5280343204 1233.5202594143 normal random 0.5718415058 25.4109083495 1.9016586927 0.9767488489 14 gupta3'
    ['mip1']='66463 66463 155.7681567188 350.7443175355 normal random 0.5902849043 425.2424451736 1.9249020001 0.9614697347 14 mip1'
    ['rail4284']='4284 1096894 2633.9943977591 4209.2593151006 normal random 0.9555418052 20.3295821919 1.5762672421 0.2691658245 14 rail4284'
    ['pwtk']='217918 217918 53.3889995319 4.7438951025 normal random 0.0593207019 2.3714810462 1.8783518634 0.9498085123 14 pwtk'
    ['crankseg_2']='63838 63838 221.6369247157 95.8757016735 normal random 0.8657685030 14.4441774735 1.7315012986 0.8668702680 14 crankseg_2'
    ['Si41Ge41H72']='185639 185639 80.8626689435 126.9718575982 normal random 0.1935377211 7.1867196402 1.2697823934 0.9631738032 14 Si41Ge41H72'
    ['TSOPF_RS_b2383']='38120 38120 424.2174449108 484.2374989813 normal random 0.4832654419 1.3172078654 1.9799037410 0.9274105439 14 TSOPF_RS_b2383'
    ['in-2004']='1382908 1382908 12.2329562053 37.2300100274 normal random 0.0215004963 632.7797560840 1.5071831956 0.7926559577 14 in-2004'
    ['Ga41As41H72']='268096 268096 68.9621478873 105.3875532241 normal random 0.1722725043 9.1794973258 1.1840159243 0.9695745410 14 Ga41As41H72'
    ['eu-2005']='862664 862664 22.2973718620 29.3334112311 normal random 0.2489010280 312.2656190701 1.3308280574 0.7601715090 14 eu-2005'
    ['wikipedia-20051105']='1634989 1634989 12.0814745543 31.0749782129 normal random 0.3400604749 410.3736264293 0.0670979986 0.0709289111 14 wikipedia-20051105'
    ['kron_g500-logn18']='262144 262144 80.7415313721 453.7323042986 normal random 0.5518512058 607.8935788628 0.0198657199 0.0201733571 14 kron_g500-logn18'
    ['rajat31']='4690002 4690002 4.3318218201 1.1061578469 normal random 0.0009716598 288.0238915611 0.5537439409 0.6485839633 14 rajat31'
    ['human_gene1']='22283 22283 1107.1060000898 1409.1216061190 normal random 0.9314766643 6.1709484000 0.4822844822 0.2820630004 14 human_gene1'
    ['delaunay_n22']='4194304 4194304 5.9999794960 1.3363830536 normal random 0.4196807112 2.8333464332 0.4937315965 0.4809064126 14 delaunay_n22'
    ['GL7d20']='1437547 1911130 20.7945089795 4.1402666304 normal random 0.5379719074 17.9953992368 0.0001211651 0.0001406065 14 GL7d20'
    ['sx-stackoverflow']='2601977 2601977 13.9253536830 137.8498583102 normal random 0.2401553809 2738.4636336314 0.0030920048 0.1165800170 14 sx-stackoverflow'
    ['dgreen']='1200611 1200611 31.8670052165 11.4484515363 normal random 0.5499536455 4.8681384940 0.0501712015 0.1673491272 14 dgreen'
    ['mawi_201512012345']='18571154 18571154 2.0483552072 3805.8118835271 normal random 0.0090474297 8006372.0851891888 0.8839521855 0.8874492450 14 mawi_201512012345'
    ['ldoor']='952203 952203 48.8577278164 11.9465715269 normal random 0.2042067138 0.5760045225 1.7967399628 0.9060465609 14 ldoor'
    ['dielFilterV2real']='1157456 1157456 41.9358938914 16.1485898792 normal random 0.5105043815 1.6230512764 0.8487051801 0.5491215541 14 dielFilterV2real'
    ['circuit5M']='5558326 5558326 10.7090319999 1356.6162743600 normal random 0.5025163836 120504.8496425602 1.0653246756 0.9427678905 14 circuit5M'
    ['soc-LiveJournal1']='4847571 4847571 14.2326482686 36.0802804379 normal random 0.3469818665 1424.8063304206 0.2842716835 0.2817802019 14 soc-LiveJournal1'
    ['bone010']='986703 986703 72.6321142228 15.8104295495 normal random 0.0181730180 0.1152091725 1.7698449586 0.9151356127 14 bone010'
    ['audikw_1']='943695 943695 82.2848981927 42.4452546211 normal random 0.6231501275 3.1927499162 1.5811026362 0.8197043756 14 audikw_1'
    ['cage15']='5154859 5154859 19.2438922190 5.7367193687 normal random 0.2119567644 1.4423333630 0.1975483538 0.7941057325 14 cage15'
    ['kmer_V2a']='55042369 55042369 2.1295885720 0.6731069663 normal random 0.3370633097 17.3133965463 0.0573248727 0.1163647359 14 kmer_V2a'

)


declare -A progs

# SpMV kernels to benchmark (uncomment the ones you want).
progs=(
    # CG
    # ['cg_csr_d']="${script_dir}/spmv_code_bench/cg_csr_d.exe"
    # ['cg_mkl_ie_d']="${script_dir}/spmv_code_bench/cg_mkl_ie_d.exe"
    # ['cg_mkl_ie_f']="${script_dir}/spmv_code_bench/cg_mkl_ie_f.exe"
    # ['cg_csr_cv_stream_d']="${script_dir}/spmv_code_bench/cg_csr_cv_stream_d.exe"

    # Custom csr
    # ['csr_naive_d']="${script_dir}/spmv_code_bench/spmv_csr_naive_d.exe"
    # ['csr_d']="${script_dir}/spmv_code_bench/spmv_csr_d.exe"
    # ['csr_kahan_d']="${script_dir}/spmv_code_bench/spmv_csr_kahan_d.exe"
    # ['csr_prefetch_d']="${script_dir}/spmv_code_bench/spmv_csr_prefetch_d.exe"
    # ['csr_simd_d']="${script_dir}/spmv_code_bench/spmv_csr_simd_d.exe"
    # ['csr_vector_d']="${script_dir}/spmv_code_bench/spmv_csr_vector_d.exe"
    # ['csr_vector_d']="${script_dir}/spmv_code_bench/spmv_csr_balanced_distribute_early_d.exe"
    # ['csr_vector_perfect_nnz_balance_d']="${script_dir}/spmv_code_bench/spmv_csr_vector_perfect_nnz_balance_d.exe"
    # ['csr_f']="${script_dir}/spmv_code_bench/spmv_csr_f.exe"

    # Custom csr x86
    # ['csr_vector_x86_d']="${script_dir}/spmv_code_bench/spmv_csr_vector_x86_d.exe"
    # ['csr_vector_oracle_balance_x86_d']="${script_dir}/spmv_code_bench/spmv_csr_vector_oracle_balance_x86_d.exe"
    # ['csr_vector_queues_x86_d']="${script_dir}/spmv_code_bench/spmv_csr_vector_queues_x86_d.exe"
    # ['csr_vector_perfect_nnz_balance_x86_d']="${script_dir}/spmv_code_bench/spmv_csr__vector_perfect_nnz_balance_x86_d.exe"

    # Custom lut
    # ['csr_vector_lut_x86_d']="${script_dir}/spmv_code_bench/spmv_csr_vector_lut_x86_d.exe"

    # Custom cuda
    # ['csr_cuda_nv_d']="${script_dir}/spmv_code_bench/spmv_csr_cuda_nv_d.exe"
    # ['csr_cuda_buffer_nv_d']="${script_dir}/spmv_code_bench/spmv_csr_cuda_buffer_nv_d.exe"
    # ['csr_cuda_reduce_nv_d']="${script_dir}/spmv_code_bench/spmv_csr_cuda_reduce_nv_d.exe"
    # ['csr_cuda_const_nnz_per_thread_nv_d']="${script_dir}/spmv_code_bench/spmv_csr_cuda_const_nnz_per_thread_nv_d.exe"

    # ['csr_cuda_const_nnz_per_thread_b1024_nnz2_nv_d']="${script_dir}/spmv_code_bench/spmv_csr_cuda_const_nnz_per_thread_b1024_nnz2_nv_d.exe"
    # ['csr_cuda_const_nnz_per_thread_b1024_nnz4_nv_d']="${script_dir}/spmv_code_bench/spmv_csr_cuda_const_nnz_per_thread_b1024_nnz4_nv_d.exe"
    # ['csr_cuda_const_nnz_per_thread_b1024_nnz6_nv_d']="${script_dir}/spmv_code_bench/spmv_csr_cuda_const_nnz_per_thread_b1024_nnz6_nv_d.exe"
    # ['csr_cuda_const_nnz_per_thread_b1024_nnz8_nv_d']="${script_dir}/spmv_code_bench/spmv_csr_cuda_const_nnz_per_thread_b1024_nnz8_nv_d.exe"

    # ['csr_cuda_const_nnz_per_thread_s2_b1024_nnz2_nv_d']="${script_dir}/spmv_code_bench/spmv_csr_cuda_const_nnz_per_thread_s2_b1024_nnz2_nv_d.exe"
    # ['csr_cuda_const_nnz_per_thread_s4_b1024_nnz2_nv_d']="${script_dir}/spmv_code_bench/spmv_csr_cuda_const_nnz_per_thread_s4_b1024_nnz2_nv_d.exe"
    # ['csr_cuda_const_nnz_per_thread_s8_b1024_nnz2_nv_d']="${script_dir}/spmv_code_bench/spmv_csr_cuda_const_nnz_per_thread_s8_b1024_nnz2_nv_d.exe"

    # ['csr_cuda_const_nnz_per_thread_s2_b1024_nnz4_nv_d']="${script_dir}/spmv_code_bench/spmv_csr_cuda_const_nnz_per_thread_s2_b1024_nnz4_nv_d.exe"
    # ['csr_cuda_const_nnz_per_thread_s4_b1024_nnz4_nv_d']="${script_dir}/spmv_code_bench/spmv_csr_cuda_const_nnz_per_thread_s4_b1024_nnz4_nv_d.exe"
    # ['csr_cuda_const_nnz_per_thread_s8_b1024_nnz4_nv_d']="${script_dir}/spmv_code_bench/spmv_csr_cuda_const_nnz_per_thread_s8_b1024_nnz4_nv_d.exe"
    # ['csr_cuda_const_nnz_per_thread_s32_b1024_nnz4_nv_d']="${script_dir}/spmv_code_bench/spmv_csr_cuda_const_nnz_per_thread_s32_b1024_nnz4_nv_d.exe"

    # ['csr_cuda_const_nnz_per_thread_s2_b1024_nnz8_nv_d']="${script_dir}/spmv_code_bench/spmv_csr_cuda_const_nnz_per_thread_s2_b1024_nnz8_nv_d.exe"
    # ['csr_cuda_const_nnz_per_thread_s4_b1024_nnz8_nv_d']="${script_dir}/spmv_code_bench/spmv_csr_cuda_const_nnz_per_thread_s4_b1024_nnz8_nv_d.exe"
    # ['csr_cuda_const_nnz_per_thread_s8_b1024_nnz8_nv_d']="${script_dir}/spmv_code_bench/spmv_csr_cuda_const_nnz_per_thread_s8_b1024_nnz8_nv_d.exe"

    # ['csr_cuda_t1769472_nv_d']="${script_dir}/spmv_code_bench/spmv_csr_cuda_t1769472_nv_d.exe"
    # ['csr_cuda_s4_t1769472_nv_d']="${script_dir}/spmv_code_bench/spmv_csr_cuda_s4_t1769472_nv_d.exe"
    # ['csr_cuda_s4_t221184_nv_d']="${script_dir}/spmv_code_bench/spmv_csr_cuda_s4_t221184_nv_d.exe"

    # ['csr_cuda_buffer_t4194304_rc4_nv_d']="${script_dir}/spmv_code_bench/spmv_csr_cuda_buffer_t4194304_rc4_nv_d.exe"
    # ['csr_cuda_buffer_t221184_rc4_nv_d']="${script_dir}/spmv_code_bench/spmv_csr_cuda_buffer_t221184_rc4_nv_d.exe"
    # ['csr_cuda_buffer_s4_t221184_rc4_nv_d']="${script_dir}/spmv_code_bench/spmv_csr_cuda_buffer_s4_t221184_rc4_nv_d.exe"

    # ['csr_cuda_vector_b256_nv_d']="${script_dir}/spmv_code_bench/spmv_csr_cuda_vector_b256_nv_d.exe"
    # ['csr_cuda_vector_s4_b256_nv_d']="${script_dir}/spmv_code_bench/spmv_csr_cuda_vector_s4_b256_nv_d.exe"

    # Cuda Adaptive
    # ['csr_cuda_adaptive_b1024_mb2_nv_d']="${script_dir}/spmv_code_bench/spmv_csr_cuda_adaptive_b1024_mb2_nv_d.exe"
    # ['csr_cuda_adaptive_b1024_mb4_nv_d']="${script_dir}/spmv_code_bench/spmv_csr_cuda_adaptive_b1024_mb4_nv_d.exe"
    # ['csr_cuda_adaptive_b128_mb2_nv_d']="${script_dir}/spmv_code_bench/spmv_csr_cuda_adaptive_b128_mb2_nv_d.exe"
    # ['csr_cuda_adaptive_b128_mb4_nv_d']="${script_dir}/spmv_code_bench/spmv_csr_cuda_adaptive_b128_mb4_nv_d.exe"
    # ['csr_cuda_adaptive_b128_mb8_nv_d']="${script_dir}/spmv_code_bench/spmv_csr_cuda_adaptive_b128_mb8_nv_d.exe"
    # ['csr_cuda_adaptive_b128_mb16_nv_d']="${script_dir}/spmv_code_bench/spmv_csr_cuda_adaptive_b128_mb16_nv_d.exe"
    # ['csr_cuda_adaptive_b128_mb24_nv_d']="${script_dir}/spmv_code_bench/spmv_csr_cuda_adaptive_b128_mb24_nv_d.exe"
    # ['csr_cuda_adaptive_b128_mb48_nv_d']="${script_dir}/spmv_code_bench/spmv_csr_cuda_adaptive_b128_mb48_nv_d.exe"

    # ['csr_cuda_adaptive_b256_mb1_nv_d']="${script_dir}/spmv_code_bench/spmv_csr_cuda_adaptive_b256_mb1_nv_d.exe"
    # ['csr_cuda_adaptive_b256_mb2_nv_d']="${script_dir}/spmv_code_bench/spmv_csr_cuda_adaptive_b256_mb2_nv_d.exe"
    # ['csr_cuda_adaptive_b256_mb4_nv_d']="${script_dir}/spmv_code_bench/spmv_csr_cuda_adaptive_b256_mb4_nv_d.exe"
    # ['csr_cuda_adaptive_b256_mb8_nv_d']="${script_dir}/spmv_code_bench/spmv_csr_cuda_adaptive_b256_mb8_nv_d.exe"
    # ['csr_cuda_adaptive_b256_mb16_nv_d']="${script_dir}/spmv_code_bench/spmv_csr_cuda_adaptive_b256_mb16_nv_d.exe"
    # ['csr_cuda_adaptive_b256_mb24_nv_d']="${script_dir}/spmv_code_bench/spmv_csr_cuda_adaptive_b256_mb24_nv_d.exe"

    # ['csr_cuda_adaptive_v3_s4_c16_b256_mb4_nv_d']="${script_dir}/spmv_code_bench/spmv_csr_cuda_adaptive_v3_s4_c16_b256_mb4_nv_d.exe"
    # ['csr_cuda_adaptive_v3_s4_c16_b256_mb2_nv_d']="${script_dir}/spmv_code_bench/spmv_csr_cuda_adaptive_v3_s4_c16_b256_mb2_nv_d.exe"
    # ['csr_cuda_adaptive_v3_s4_c16_b256_mb1_nv_d']="${script_dir}/spmv_code_bench/spmv_csr_cuda_adaptive_v3_s4_c16_b256_mb1_nv_d.exe"
    # ['csr_cuda_adaptive_v3_s4_c128_b256_mb1_nv_d']="${script_dir}/spmv_code_bench/spmv_csr_cuda_adaptive_v3_s4_c128_b256_mb1_nv_d.exe"
    # ['csr_cuda_adaptive_v3_s4_c64_b256_mb1_nv_d']="${script_dir}/spmv_code_bench/spmv_csr_cuda_adaptive_v3_s4_c64_b256_mb1_nv_d.exe"
    # ['csr_cuda_adaptive_v3_s4_c32_b256_mb1_nv_d']="${script_dir}/spmv_code_bench/spmv_csr_cuda_adaptive_v3_s4_c32_b256_mb1_nv_d.exe"
    # ['csr_cuda_adaptive_v3_s4_c16_b256_mb1_nv_d']="${script_dir}/spmv_code_bench/spmv_csr_cuda_adaptive_v3_s4_c16_b256_mb1_nv_d.exe"
    # ['csr_cuda_adaptive_v3_s4_c8_b256_mb1_nv_d']="${script_dir}/spmv_code_bench/spmv_csr_cuda_adaptive_v3_s4_c8_b256_mb1_nv_d.exe"
    # ['csr_cuda_adaptive_v3_s4_c4_b256_mb1_nv_d']="${script_dir}/spmv_code_bench/spmv_csr_cuda_adaptive_v3_s4_c4_b256_mb1_nv_d.exe"

    # ['csr_cuda_adaptive_s16_b256_mb2_nv_d']="${script_dir}/spmv_code_bench/spmv_csr_cuda_adaptive_s16_b256_mb2_nv_d.exe"
    # ['csr_cuda_adaptive_s8_b256_mb4_nv_d']="${script_dir}/spmv_code_bench/spmv_csr_cuda_adaptive_s8_b256_mb4_nv_d.exe"
    # ['csr_cuda_adaptive_s8_b256_mb4_nv_d']="${script_dir}/spmv_code_bench/spmv_csr_cuda_adaptive_s8_b256_mb4_nv_d.exe"
    # ['csr_cuda_adaptive_s4_b256_mb4_nv_d']="${script_dir}/spmv_code_bench/spmv_csr_cuda_adaptive_s4_b256_mb4_nv_d.exe"
    # ['csr_cuda_adaptive_s1_b256_mb4_nv_d']="${script_dir}/spmv_code_bench/spmv_csr_cuda_adaptive_s1_b256_mb4_nv_d.exe"
    # ['csr_cuda_adaptive_s4_b256_mb4_nv_d']="${script_dir}/spmv_code_bench/spmv_csr_cuda_adaptive_s4_b256_mb4_nv_d.exe"
    # ['csr_cuda_adaptive_s16_b256_mb4_nv_d']="${script_dir}/spmv_code_bench/spmv_csr_cuda_adaptive_s16_b256_mb4_nv_d.exe"
    # ['csr_cuda_adaptive_s16_b256_mb24_nv_d']="${script_dir}/spmv_code_bench/spmv_csr_cuda_adaptive_s16_b256_mb24_nv_d.exe"

    # UNDER DEVELOPMENT - not working yet
    # ['merge_cuda_nv_d']="${script_dir}/spmv_code_bench/spmv_merge_cuda_nv_d.exe"
    # ['csr_cuda_light_b1024_nv_d']="${script_dir}/spmv_code_bench/spmv_csr_cuda_light_b1024_nv_d.exe"

    # cusparse
    # ['cusparse_csr_nv_d']="${script_dir}/spmv_code_bench/spmv_cusparse_csr_nv_d.exe"
    # ['cusparse_csr_s4_nv_d']="${script_dir}/spmv_code_bench/spmv_cusparse_csr_s4_nv_d.exe"
    # ['cusparse_coo_nv_d']="${script_dir}/spmv_code_bench/spmv_cusparse_coo_nv_d.exe"

    # Custom compressed values block
    # ['csr_cv_block_id_d']="${script_dir}/spmv_code_bench/spmv_csr_cv_block_id_d.exe"
    # ['csr_cv_block_d2f_d']="${script_dir}/spmv_code_bench/spmv_csr_cv_block_d2f_d.exe"
    # ['csr_cv_block_fpc_d']="${script_dir}/spmv_code_bench/spmv_csr_cv_block_fpc_d.exe"
    # ['csr_cv_block_zfp_d']="${script_dir}/spmv_code_bench/spmv_csr_cv_block_zfp_d.exe"
    # ['csr_cv_block_fpzip_d']="${script_dir}/spmv_code_bench/spmv_csr_cv_block_fpzip_d.exe"

    # Custom compressed values stream
    # ['csr_cv_stream_d']="${script_dir}/spmv_code_bench/spmv_csr_cv_stream_d.exe"
    # ['csr_cv_stream_rf_d']="${script_dir}/spmv_code_bench/spmv_csr_cv_stream_rf_d.exe"
    # ['csr_cv_stream_opt_compress_d']="${script_dir}/spmv_code_bench/spmv_csr_cv_stream_opt_compress_d.exe"

    # MKL IE
    # ['mkl_ie_d']="${script_dir}/spmv_code_bench/spmv_mkl_ie_d.exe"
    # ['mkl_ie_f']="${script_dir}/spmv_code_bench/spmv_mkl_ie_f.exe"

    # MKL CSR
    # ['mkl_csr_d']="${script_dir}/spmv_code_bench/spmv_mkl_csr_d.exe"

    # AOCL
    # ['aocl_optmv_d']="${script_dir}/spmv_code_bench/spmv_aocl_optmv_d.exe"

    # CSR-RV
    # ['csrrv_d']="${script_dir}/spmv_code_bench/spmv_csrrv_d.exe"

    # CSR5
    # ['csr5_d']="${script_dir}/spmv_code_bench/spmv_csr5_d.exe"

    # merge spmv
    # ['merge_d']="${script_dir}/spmv_code_bench/spmv_merge_d.exe"

    # sell C sigma
    # ['sell_C_s_d']="${script_dir}/spmv_code_bench/sell-C-s/build/spmvbench/spmv_sell-C-s_d.exe"

    # sparsex
    # ['sparsex_d']="${script_dir}/spmv_code_bench/spmv_sparsex_d.exe"
    # ['sparsex_d']="${script_dir}/spmv_code_sparsex/spmv_sparsex_d.exe"
    # ['sparsex_d']="/various/pmpakos/SpMV-Research/benchmark_code/CPU/AMD/spmv_code_sparsex/spmv_sparsex_d.exe"

    # SpV8
    # ['spv8_d']="${script_dir}/spmv_code_bench/spmv_spv8_d.exe"

    # LCM - partially strided codelet
    # ['lcm_d']="${script_dir}/spmv_code_bench/LCM-partially-strided-codelet/spmv_lcm_d.exe"

    # ['ell_d']="${script_dir}/spmv_code_bench/spmv_ell_d.exe"
    # ['ldu_d']="${script_dir}/spmv_code_bench/spmv_ldu_d.exe"
    # ['mkl_csr_d']="${script_dir}/spmv_code_bench/spmv_mkl_csr_d.exe"
    # ['mkl_dia_d']="${script_dir}/spmv_code_bench/spmv_mkl_dia_d.exe"
    # ['dia_d']="${script_dir}/spmv_code_bench/spmv_dia_d.exe"
    # ['mkl_bsr_2_d']="${script_dir}/spmv_code_bench/spmv_mkl_bsr_2_d.exe"
    # ['mkl_bsr_4_d']="${script_dir}/spmv_code_bench/spmv_mkl_bsr_4_d.exe"
    # ['mkl_bsr_8_d']="${script_dir}/spmv_code_bench/spmv_mkl_bsr_8_d.exe"
    # ['mkl_bsr_16_d']="${script_dir}/spmv_code_bench/spmv_mkl_bsr_16_d.exe"
    # ['mkl_bsr_32_d']="${script_dir}/spmv_code_bench/spmv_mkl_bsr_32_d.exe"
    # ['mkl_bsr_64_d']="${script_dir}/spmv_code_bench/spmv_mkl_bsr_64_d.exe"
    # ['mkl_coo_d']="${script_dir}/spmv_code_bench/spmv_mkl_coo_d.exe"
    # ['mkl_csc_d']="${script_dir}/spmv_code_bench/spmv_mkl_csc_d.exe"

)


# Export variables for make.
config_str=''
for index in "${!conf_vars[@]}"; do
    eval "export $index='${conf_vars["$index"]}'"
    config_str="${config_str}${index}=${conf_vars["$index"]};"
    # printf "%s=%s;" "$index"  "${conf_vars["$index"]}"
done
printf "%s\n" "$config_str"

