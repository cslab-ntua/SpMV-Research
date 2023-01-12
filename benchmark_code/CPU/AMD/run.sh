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

    "$path_validation"/scircuit.mtx
    "$path_validation"/mac_econ_fwd500.mtx
    "$path_validation"/raefsky3.mtx
    "$path_validation"/rgg_n_2_17_s0.mtx
    "$path_validation"/bbmat.mtx
    "$path_validation"/appu.mtx
    "$path_validation"/conf5_4-8x8-15.mtx
    "$path_validation"/mc2depi.mtx
    "$path_validation"/rma10.mtx
    "$path_validation"/cop20k_A.mtx
    "$path_validation"/thermomech_dK.mtx
    "$path_validation"/webbase-1M.mtx
    "$path_validation"/cant.mtx
    "$path_validation"/ASIC_680k.mtx
    "$path_validation"/pdb1HYS.mtx
    "$path_validation"/roadNet-TX.mtx
    "$path_validation"/TSOPF_RS_b300_c3.mtx
    "$path_validation"/Chebyshev4.mtx
    "$path_validation"/consph.mtx
    "$path_validation"/com-Youtube.mtx
    "$path_validation"/rajat30.mtx
    "$path_validation"/radiation.mtx
    "$path_validation"/Stanford_Berkeley.mtx
    "$path_validation"/shipsec1.mtx
    "$path_validation"/PR02R.mtx
    "$path_validation"/CurlCurl_2.mtx
    "$path_validation"/gupta3.mtx
    "$path_validation"/mip1.mtx
    "$path_validation"/rail4284.mtx
    "$path_validation"/pwtk.mtx
    "$path_validation"/crankseg_2.mtx
    "$path_validation"/Si41Ge41H72.mtx
    "$path_validation"/TSOPF_RS_b2383.mtx
    "$path_validation"/in-2004.mtx
    "$path_validation"/Ga41As41H72.mtx
    "$path_validation"/eu-2005.mtx
    "$path_validation"/wikipedia-20051105.mtx
    "$path_validation"/kron_g500-logn18.mtx
    "$path_validation"/rajat31.mtx
    "$path_validation"/human_gene1.mtx
    "$path_validation"/delaunay_n22.mtx
    "$path_validation"/GL7d20.mtx
    "$path_validation"/sx-stackoverflow.mtx
    "$path_validation"/dgreen.mtx
    "$path_validation"/mawi_201512012345.mtx
    "$path_validation"/ldoor.mtx
    "$path_validation"/dielFilterV2real.mtx
    "$path_validation"/circuit5M.mtx
    "$path_validation"/soc-LiveJournal1.mtx
    "$path_validation"/bone010.mtx
    "$path_validation"/audikw_1.mtx
    "$path_validation"/cage15.mtx
    "$path_validation"/kmer_V2a.mtx

)


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
# printf "%s\n" "${matrices_validation_loop[@]}"
# exit

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
                # if ((!use_artificial_matrices)); then
                    # "$prog" "${prog_args[@]}" -t -o spx.rt.nr_threads=$t -o spx.rt.cpu_affinity=${mt_conf} -o spx.preproc.xform=all #-v  2>'tmp.err'
                # else
                    # prog_args2="${prog_args[@]}"
                    # "$prog" -p "${prog_args2[@]}" -t -o spx.rt.nr_threads=$t -o spx.rt.cpu_affinity=${mt_conf} -o spx.preproc.xform=all #-v  2>'tmp.err'
                # fi
                # export GOMP_CPU_AFFINITY="${GOMP_CPU_AFFINITY_backup}"
            # elif [[ "$prog" == *"spmv_sell-C-s.exe"* ]]; then
            if [[ "$prog" == *"spmv_sell-C-s.exe"* ]]; then
                if ((!use_artificial_matrices)); then
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
                "$prog" "${prog_args[@]}"  2>'tmp.err'
                ret="$?"
            fi
            cat 'tmp.err'
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
    "${matrices_validation[@]}"

    # "${matrices_validation_artificial_twins[@]}"
    # "${matrices_validation_loop[@]}"

    # "$path_other"/simple.mtx
    # "$path_other"/simple_symmetric.mtx

    # /home/jim/Documents/Synced_Documents/other/ASIC_680k.mtx

    # "$path_openFoam"/100K.mtx
    # "$path_openFoam"/600K.mtx
    # "$path_openFoam"/TestMatrices/HEXmats/5krows/processor0
    # "${matrices_openFoam_own_neigh[@]}"

    # '/home/jim/Data/graphs/tamu/ML/thermomech_dK.mtx'

    # "$path_validation"/scircuit.mtx
    # "$path_validation"/mac_econ_fwd500.mtx
    # "$path_validation"/raefsky3.mtx
    # "$path_validation"/bbmat.mtx
    # "$path_validation"/conf5_4-8x8-15.mtx
    # "$path_validation"/mc2depi.mtx
    # "$path_validation"/rma10.mtx
    # "$path_validation"/cop20k_A.mtx
    # "$path_validation"/webbase-1M.mtx
    # "$path_validation"/cant.mtx
    # "$path_validation"/pdb1HYS.mtx
    # "$path_validation"/TSOPF_RS_b300_c3.mtx
    # "$path_validation"/Chebyshev4.mtx
    # "$path_validation"/consph.mtx
    # "$path_validation"/shipsec1.mtx
    # "$path_validation"/PR02R.mtx
    # "$path_validation"/mip1.mtx
    # "$path_validation"/rail4284.mtx
    # "$path_validation"/pwtk.mtx
    # "$path_validation"/crankseg_2.mtx
    # "$path_validation"/Si41Ge41H72.mtx
    # "$path_validation"/TSOPF_RS_b2383.mtx
    # "$path_validation"/in-2004.mtx
    # "$path_validation"/Ga41As41H72.mtx
    # "$path_validation"/eu-2005.mtx
    # "$path_validation"/wikipedia-20051105.mtx
    # "$path_validation"/ldoor.mtx
    # "$path_validation"/circuit5M.mtx
    # "$path_validation"/bone010.mtx
    # "$path_validation"/cage15.mtx

    # "$path_selected"/soc-LiveJournal1.mtx
    # "$path_selected"/soc-LiveJournal1_sorted_1.mtx
    # "$path_selected"/soc-LiveJournal1_sorted_2.mtx
    # "$path_selected"/soc-LiveJournal1_sorted_3.mtx
    # "$path_selected"/soc-LiveJournal1_sorted_4.mtx

    # "$path_selected"/dielFilterV3real.mtx
    # "$path_selected"/dielFilterV3real_sorted_1.mtx
    # "$path_selected"/dielFilterV3real_sorted_2.mtx
    # "$path_selected"/dielFilterV3real_sorted_3.mtx
    # "$path_selected"/dielFilterV3real_sorted_4.mtx

    # "$path_selected"/circuit5M.mtx
    # "$path_selected"/circuit5M_sorted_1.mtx
    # "$path_selected"/circuit5M_sorted_2.mtx
    # "$path_selected"/circuit5M_sorted_3.mtx
    # "$path_selected"/circuit5M_sorted_4.mtx

    # "$path_selected"/wikipedia-20051105.mtx
    # "$path_selected"/wikipedia-20051105_sorted_1.mtx
    # "$path_selected"/wikipedia-20051105_sorted_2.mtx
    # "$path_selected"/wikipedia-20051105_sorted_3.mtx
    # "$path_selected"/wikipedia-20051105_sorted_4.mtx

    # "$path_selected_sorted"/circuit5M.mtx
)


if ((!use_artificial_matrices)); then
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

    for ((i=0;i<rep;i++)); do
        for a in "${prog_args[@]}"
        do

            rep_in=1
            # rep_in=10

            for ((j=0;j<rep_in;j++)); do

                printf "Temps: " >&1
                for ((k=0;k<${#temp_labels[@]};k++)); do
                    printf "%s %s " $(cat ${temp_labels[k]}) $(cat ${temp_inputs[k]}) >&1
                done
                echo >&1

                echo "File: $a"
                bench "$p" $a

            done
        done
    done
done

