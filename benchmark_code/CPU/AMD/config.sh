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


declare -A conf_vars
conf_vars=(
    ['output_to_files']=0
    # ['output_to_files']=1

    ['COOLDOWN']=0
    # ['COOLDOWN']=1

    # Benchmark with the artificially generated matrices (1) or the real validation matrices (0).
    ['use_artificial_matrices']=0
    # ['use_artificial_matrices']=1

    # Maximum number of the machine's cores.
    # ['max_cores']=160
    # ['max_cores']=256
    # ['max_cores']=128
    # ['max_cores']=64
    ['max_cores']=48
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
    # ['cores']=16
    ['cores']=8
    # ['cores']=4
    # ['cores']='1 2 4 8 16 24 48'
    # ['cores']='24 48'
    # ['cores']=48
    # ['cores']=24
    # ['cores']='1 2 4 8'
    # ['cores']=14
    # ['cores']=6

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

    # Rapl registers.
    ['RAPL_REGISTERS']='0'         # 1 socket : Epyc1, Gold
    # ['RAPL_REGISTERS']='0,1'       # 2 sockets: Epyc1, Gold

    # Path for the mkl library.
    # ['MKL_PATH']='/opt/intel/oneapi/mkl/latest'
    ['MKL_PATH']="$( options=(
                        '/opt/intel/oneapi/mkl/latest'
                        '/various/common_tools/intel_parallel_studio/compilers_and_libraries/linux/mkl'
                    )
                    find_valid_dir "${options[@]}"
                )"

    ['AOCL_PATH']="$( options=(
                        '/opt/aoclsparse'
                        '/various/pmpakos/spmv_paper/aocl-sparse/build/release'
                    )
                    find_valid_dir "${options[@]}"
                )"

    # SparseX ecosystem environment variables that have to be set.
    # These are environment variables that have to be set for SparseX to work
    # Need to install specific library versions
    ['SPARSEX_ROOT_DIR']='/home/pmpakos/sparsex'
    ['BOOST_LIB_PATH']='/home/pmpakos/sparsex/boost_1_55_0/local/lib/'
    ['LLVM_LIB_PATH']='/home/pmpakos/sparsex/llvm-6.0.0/build/lib'

    # SELL-C-s ecosystem environment variables that have to be set
    # These are environment variables that have to be set for SELL-C-s to work
    ['GHOST_ROOT_DIR']='/home/pmpakos/ESSEX'
    ['GHOST_APPS_ROOT_DIR']='/various/pmpakos/SpMV-Research/benchmark_code/CPU/AMD/spmv_code_sell-C-s'

    # Path for the validation matrices.
    ['path_validation']="$( options=(
                        "$HOME/Data/graphs/validation_matrices"
                        # "${script_dir}/../../../validation_matrices"
                        '/various/pmpakos/SpMV-Research/validation_matrices'
                    )
                    find_valid_dir "${options[@]}"
                )"

    # Path for the openFoam matrices.
    ['path_openFoam']="$( options=(
                        '/zhome/academic/HLRS/xex/xexdgala/Data/graphs/openFoam'
                        '/various/dgal/graphs/matrices_openFoam'
                        '/home/jim/Data/graphs/matrices_openFoam'
                    )
                    find_valid_dir "${options[@]}"
                )"
)

conf_vars['cpu_affinity']="$(calc_cpu_pinning "${conf_vars["cores"]}" "${conf_vars["max_cores"]}" "${conf_vars["cpu_pinning_step"]}" "${conf_vars["cpu_pinning_group_size"]}")"


path_artificial="${script_dir}/../../../matrix_generation_parameters"


# Artificial matrices to benchmark.
artificial_matrices_files=(

    # Validation matrices artificial twins.
    # "$path_artificial"/validation_friends/twins_random.txt

    # Validation matrices artificial twins in a +-30% value space of each feature.
    "$path_artificial"/validation_matrices_10_samples_30_range_twins.txt

    # The synthetic dataset studied in the paper.
    #"$path_artificial"/synthetic_matrices_small_dataset.txt
    # "$path_artificial"/synthetic_matrices_small_dataset5.txt
)


# Artificial twins.
declare -A matrices_validation_artificial_twins
matrices_validation_artificial_twins=(
    ['scircuit']='170998 170998 5.607878455 4.39216211 normal random 0.2972525308 61.94715601 0.803365 0.633019 14 scircuit'
    ['mac_econ_fwd500']='206500 206500 6.166532688 4.435865332 normal random 0.001907956522 6.135290158 0.176686 0.330509 14 mac_econ_fwd500'
    ['raefsky3']='21200 21200 70.22490566 6.326999832 normal random 0.0662003204 0.1391969736 1.916 0.963020 14 raefsky3'
    ['bbmat']='38744 38744 45.72893867 38.39531373 normal random 0.02989415739 1.755366813 1.262626 0.853730 14 bbmat'
    ['conf5_4-8x-15']='49152 49152 39 0 normal random 0.2446905772 0 1.4415068 0.810948 14 conf5_4-8x-15'
    ['mc2depi']='525825 525825 3.994152047 0.07632277052 normal random 0.001342169398 0.001464128789 0.498297 0.998906 14 mc2depi'
    ['rma10']='46835 46835 50.68860895 27.78059606 normal random 0.1877712645 1.86060326 1.719725 0.866408 14 rma10'
    ['cop20k_A']='121192 121192 21.65432537 13.79266245 normal random 0.6230549795 2.740592173 1.095827 0.633400 14 cop20k_A'
    ['webbase-1M']='1000005 1000005 3.105520472 25.34520973 normal random 0.1524629001 1512.433913 0.315681 0.936095 14 webbase-1M'
    ['cant']='62451 62451 64.16843605 14.05626099 normal random 0.008604097649 0.215550897 1.61575 0.914729 14 cant'
    ['pdb1HYS']='36417 36417 119.305956 31.86038422 normal random 0.1299377034 0.7098894878 1.837758 0.931726 14 pdb1HYS'
    ['TSOPF_RS_b300_c3']='42138 42138 104.73798 102.4431672 normal random 0.6066963064 0.9954557077 1.923459 0.992154 14 TSOPF_RS_b300_c3'
    ['Chebyshev4']='68121 68121 78.94424627 1061.43997 normal random 0.04535742434 861.9001253 0.895986 0.993706 14 Chebyshev4'
    ['consph']='83334 83334 72.125183 19.08019415 normal random 0.06981133797 0.1230474105 1.71335 0.882633 14 consph'
    ['shipsec1']='140874 140874 55.46377614 11.07481064 normal random 0.04587554507 0.8390381452 1.712385 0.873444 14 shipsec1'
    ['PR02R']='161070 161070 50.81725958 19.6982847 normal random 0.03958570835 0.8104085258 1.279542 0.868860 14 PR02R'
    ['mip1']='66463 66463 155.7681567 350.7443175 normal random 0.5902849043 425.2424452 1.924902 0.961470 14 mip1'
    ['rail4284']='4284 1096894 2633.994398 4209.259315 normal random 0.9555418052 20.32958219 1.576267 0.269166 14 rail4284'
    ['pwtk']='217918 217918 53.38899953 4.743895102 normal random 0.05932070192 2.371481046 1.878352 0.949809 14 pwtk'
    ['crankseg_2']='63838 63838 221.6369247 95.87570167 normal random 0.865768503 14.44417747 1.731501 0.866870 14 crankseg_2'
    ['Si41Ge41H72']='185639 185639 80.86266894 126.9718576 normal random 0.1935377211 7.186719641 1.269782 0.963174 14 Si41Ge41H72'
    ['TSOPF_RS_b2383']='38120 38120 424.2174449 484.237499 normal random 0.4832654419 1.317207865 1.979904 0.927411 14 TSOPF_RS_b2383'
    ['in-2004']='1382908 1382908 12.23295621 37.23001003 normal random 0.02150049625 632.7797558 1.507183 0.792656 14 in-2004'
    ['Ga41As41H72']='268096 268096 68.96214789 105.3875532 normal random 0.1722725043 9.179497325 1.184016 0.969575 14 Ga41As41H72'
    ['eu-2005']='862664 862664 22.29737186 29.33341123 normal random 0.248901028 312.2656191 1.330828 0.760172 14 eu-2005'
    ['wikipedia-20051105']='1634989 1634989 12.08147455 31.07497821 normal random 0.3400604749 410.3736266 0.067098 0.070929 14 wikipedia-20051105'
    ['rajat31']='4690002 4690002 4.33182182 1.106157847 normal random 0.0009716598294 288.0238916 0.553744 0.648584 14 rajat31'
    ['ldoor']='952203 952203 48.85772782 11.94657153 normal random 0.2042067138 0.5760045224 1.79674 0.906047 14 ldoor'
    ['circuit5M']='5558326 5558326 10.709032 1356.616274 normal random 0.5025163836 120504.8496 1.065325 0.942768 14 circuit5M'
    ['bone010']='986703 986703 72.63211422 15.81042955 normal random 0.01817301802 0.1152091726 1.769845 0.915136 14 bone010'
    ['cage15']='5154859 5154859 19.24389222 5.736719369 normal random 0.2119567644 1.442333363 0.197548 0.794106 14 cage15'
)


declare -A progs

# SpMV kernels to benchmark (uncomment the ones you want).
progs=(
    # Custom csr
    # ['csr_naive_d']="${script_dir}/spmv_code_bench/spmv_csr_naive.exe"
    # ['csr_d']="${script_dir}/spmv_code_bench/spmv_csr.exe"
    # ['csr_prefetch_d']="${script_dir}/spmv_code_bench/spmv_csr_prefetch.exe"
    # ['csr_simd_d']="${script_dir}/spmv_code_bench/spmv_csr_simd.exe"
    # ['csr_vector_d']="${script_dir}/spmv_code_bench/spmv_csr_vector.exe"

    # Custom csr x86
    # ['csr_x86_vector_d']="${script_dir}/spmv_code_bench/spmv_csr_x86_vector.exe"
    # ['csr_x86_vector_queues_d']="${script_dir}/spmv_code_bench/spmv_csr_x86_vector_queues.exe"
    ['csr_x86_vector_perfect_nnz_balance_d']="${script_dir}/spmv_code_bench/spmv_csr_x86_vector_perfect_nnz_balance.exe"

    # MKL IE
    # ['mkl_ie_d']="${script_dir}/spmv_code_bench/spmv_mkl_ie.exe"

    # AOCL
    # ['aocl_optmv_d']="${script_dir}/spmv_code_bench/spmv_aocl_optmv.exe"

    # CSR5
    # ['csr5_d']="${script_dir}/spmv_code_bench/spmv_csr5.exe"

    # merge spmv
    # ['merge_d']="${script_dir}/spmv_code_bench/spmv_merge.exe"

    # sell C sigma
    # ['sell_C_s_d']="${script_dir}/spmv_code_sell-C-s/build/spmvbench/spmv_sell-C-s.exe"
    # ['sell_C_s_d']="/various/pmpakos/SpMV-Research/benchmark_code/CPU/AMD/spmv_code_sell-C-s/build/spmvbench/spmv_sell-C-s.exe"

    # sparsex
    # ['sparsex_d']="${script_dir}/spmv_code_sparsex/spmv_sparsex.exe"

    # ['ell_d']="${script_dir}/spmv_code_bench/spmv_ell.exe"
    # ['ldu_d']="${script_dir}/spmv_code_bench/spmv_ldu.exe"
    # ['mkl_csr_d']="${script_dir}/spmv_code_bench/spmv_mkl_csr.exe"
    # ['mkl_dia_d']="${script_dir}/spmv_code_bench/spmv_mkl_dia.exe"
    # ['dia_d']="${script_dir}/spmv_code_bench/spmv_dia.exe"
    # ['mkl_bsr_2_d']="${script_dir}/spmv_code_bench/spmv_mkl_bsr_2.exe"
    # ['mkl_bsr_4_d']="${script_dir}/spmv_code_bench/spmv_mkl_bsr_4.exe"
    # ['mkl_bsr_8_d']="${script_dir}/spmv_code_bench/spmv_mkl_bsr_8.exe"
    # ['mkl_bsr_16_d']="${script_dir}/spmv_code_bench/spmv_mkl_bsr_16.exe"
    # ['mkl_bsr_32_d']="${script_dir}/spmv_code_bench/spmv_mkl_bsr_32.exe"
    # ['mkl_bsr_64_d']="${script_dir}/spmv_code_bench/spmv_mkl_bsr_64.exe"
    # ['mkl_coo_d']="${script_dir}/spmv_code_bench/spmv_mkl_coo.exe"
    # ['mkl_csc_d']="${script_dir}/spmv_code_bench/spmv_mkl_csc.exe"

)


# Export variables for make.
config_str=''
for index in "${!conf_vars[@]}"; do
    eval "export $index='${conf_vars["$index"]}'"
    config_str="${config_str}${index}=${conf_vars["$index"]};"
    # printf "%s=%s;" "$index"  "${conf_vars["$index"]}"
done
printf "%s" "$config_str"

