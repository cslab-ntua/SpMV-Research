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

export MKL_DEBUG_CPU_TYPE=5

export LD_LIBRARY_PATH="${AOCL_PATH}/lib:${MKL_PATH}/lib/intel64:${LD_LIBRARY_PATH}"
export LD_LIBRARY_PATH="${LD_LIBRARY_PATH}:${BOOST_LIB_PATH}:${LLVM_LIB_PATH}:${SPARSEX_LIB_PATH}"
# new addition for gold1
export LD_LIBRARY_PATH=:${LD_LIBRARY_PATH}:${GCC_COMPILER_PATH}/lib64

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
    # scircuit.mtx
    # mac_econ_fwd500.mtx
    # raefsky3.mtx
    # rgg_n_2_17_s0.mtx
    # bbmat.mtx
    # appu.mtx
    # conf5_4-8x8-15.mtx
    # mc2depi.mtx
    rma10.mtx
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
            if [[ "$prog" == *"spmv_sell-C-s.exe"* ]]; then
                if ((!USE_ARTIFICIAL_MATRICES)); then
                    "$prog" -c $OMP_NUM_THREADS -m "${prog_args[@]}" -f SELL-32-1  2>'tmp.err'
                    ret="$?"
                else
                    prog_args2="${prog_args[@]}"
                    "$prog" -c $OMP_NUM_THREADS --artif_args="${prog_args2[@]}" -f SELL-32-1  2>'tmp.err'
                    ret="$?"
                fi
            else
                # "$prog" 4690000 4 1.6 normal random 1 14  2>'tmp.err'

                # numactl -i all "$prog" "${prog_args[@]}"  2>'tmp.err'
                timeout 420s "$prog" "${prog_args[@]}"  2>'tmp.err'
                ret="$?"
                if [[ $ret -eq 124 ]]; then
                    # Timeout occurred (exit status 124)
                    echo "Timeout occurred. Resetting the device..."
                    (echo -ne 'y' ; echo ) | xbutil reset -d 0 
                fi
            fi
            # cat 'tmp.err'
            if ((!ret || !force_retry_on_error )); then      # If not retrying then print the error text to be able to notice it.
                cat 'tmp.err' >&2           
                break
            fi
            echo "ERROR: Program exited with error [${ret}], retrying."
        done
    done

    # rm 'tmp.err'
}


matrices=(
    "${matrices_validation[@]}"
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
    p="${progs["$format_name"]}"

    if ((output_to_files)); then
        > "${format_name}.out"
        exec 1>>"${format_name}.out"
        > "${format_name}.csv"
        exec 2>>"${format_name}.csv"
    fi

    echo "$config_str"

    echo "program: $p"
    echo "number of matrices: ${#prog_args[@]}"

    rep=1
    # rep=4
    # rep=5
    # rep=16
    # rep=1024


    LEVEL3_CACHE_SIZE="$(getconf LEVEL3_CACHE_SIZE)"
    csrcv_num_packet_vals=(
        # 128 
        $((2**7))
        # $((2**16))
        # $((2**12))
        # $((LEVEL3_CACHE_SIZE / 8 / 8 / 16))
    )
    # if [[ "$p" == *'spmv_csr_cv'* ]]; then
        # csrcv_num_packet_vals=( $( declare -i i; for ((i=64;i<LEVEL3_CACHE_SIZE / 8 / cores / 4;i*=2)); do echo "$i"; done ) )
    # fi

    for ((i=0;i<rep;i++)); do
        for a in "${prog_args[@]}"
        do

            rep_in=1
            # rep_in=10

            for packet_vals in "${csrcv_num_packet_vals[@]}"; do
                export CSRCV_NUM_PACKET_VALS="$packet_vals"

                # printf "Temps: " >&1
                # for ((k=0;k<${#temp_labels[@]};k++)); do
                #     printf "%s %s " $(cat ${temp_labels[k]}) $(cat ${temp_inputs[k]}) >&1
                # done
                # echo >&1

                echo "File: $a"
                bench "$p" $a

            done
        done
    done
done

