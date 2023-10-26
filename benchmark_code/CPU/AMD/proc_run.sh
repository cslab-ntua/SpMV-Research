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

# This variable must be set BEFORE the program starts.
# If we set it then all the processes will be pinned to the first processor given (i.e. 0).
# As here we set the affinities by hand, there is no need to set it at all.
# export XLSMPOPTS="PROCS=$cpu_affinity"

export MKL_DEBUG_CPU_TYPE=5

export LD_LIBRARY_PATH="${AOCL_PATH}/lib:${MKL_PATH}/lib/intel64:${LD_LIBRARY_PATH}"
export LD_LIBRARY_PATH="${LD_LIBRARY_PATH}:${BOOST_LIB_PATH}:${LLVM_LIB_PATH}:${SPARSEX_LIB_PATH}"


# Encourages idle threads to spin rather than sleep.
# export OMP_WAIT_POLICY='active'
# Don't let the runtime deliver fewer threads than those we asked for.
# export OMP_DYNAMIC='false'


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
    "$path_validation"/TSOPF_RS_b2383.mtx
    # "$path_validation"/in-2004.mtx
    # "$path_validation"/Ga41As41H72.mtx
    # "$path_validation"/eu-2005.mtx
    # "$path_validation"/wikipedia-20051105.mtx
    # "$path_validation"/ldoor.mtx
    # "$path_validation"/circuit5M.mtx
    # "$path_validation"/bone010.mtx
    # "$path_validation"/cage15.mtx
)


declare -i count_procs
count_procs=0

trap 'count_procs+=1' USR1


awk_cmd='
BEGIN {
    num_procs = 0
    time_max = 0.0
    Watt_max = 0.0
    Joule_max = 0.0
}

// {
    split($0, tok, ",")
    i=1
    file = tok[i++]
    num_procs = tok[i++]
    m = tok[i++]
    n = tok[i++]
    nnz = tok[i++]
    time = tok[i++]
    gflops = tok[i++]
    mem = tok[i++]
    Watt = tok[i++]
    Joule = tok[i++]

    if (time > time_max)
        time_max = time
    if (Watt > Watt_max)
    {
        Watt_max = Watt
        Joule_max = Joule
    }
}

END {
    gflops = (time_max > 0) ? (nnz * num_procs) / time_max * 128 * 2 * 1e-9 : 0
    Watt = Watt / num_procs

    printf("%s", file)
    printf(",%d", num_procs)
    printf(",%d", m)
    printf(",%d", n)
    printf(",%d", nnz)
    printf(",%g", time_max)
    printf(",%g", gflops)
    printf(",%g", mem)
    printf(",%g", Watt_max)
    printf(",%g", Joule_max)
    printf("\n")
}
'

bench()
{
    declare args=("$@")
    declare prog="${args[0]}"
    declare prog_args=("${args[@]:1}")
    declare t

    export OMP_NUM_THREADS="1"
    for t in $cores
    do
        pids=()
        count_procs=0

        export NUM_PROCESSES="$t"

        while :; do
            "$prog" "${prog_args[@]}"  2>'tmp.err'
            ret="$?"
            cat 'tmp.err'
            if ((!ret || !force_retry_on_error)); then      # If not retrying then print the error text to be able to notice it.
                awk "${awk_cmd}" 'tmp.err' >&2
                break
            fi
            echo "ERROR: Program exited with error [${ret}], retrying."
        done
    done

    rm 'tmp.err'
}


matrices=(
    # "${matrices_all[@]}"
    # "${matrices_real[@]}"
    # "${matrices_banded[@]}"
    # "${matrices_block_diagonal[@]}"
    # "${matrices_openFoam[@]}"
    # "${matrices_TI[@]}"
    # "${matrices_selected_sorted[@]}"
    # "${matrices_validation[@]}"

    # /home/jim/Documents/Synced_Documents/other/ASIC_680k.mtx

    # "$path_openFoam"/100K.mtx
    # "$path_openFoam"/600K.mtx
    # "$path_openFoam"/TestMatrices/HEXmats/5krows/processor0
    "${matrices_openFoam_own_neigh[@]}"

    # AOCL crashes with these

    # /various/dgal/graphs/matrices_openFoam/TestMatrices/SHMmats/5krows/processor7
    # /various/dgal/graphs/matrices_openFoam/TestMatrices/SHMmats-RCM/5krows/processor14
    # /various/dgal/graphs/matrices_openFoam/TestMatrices/SHMmats-RCM/5krows/processor42
    # /various/dgal/graphs/matrices_openFoam/TestMatrices/SHMmats/5krows/processor7
    # /various/dgal/graphs/matrices_openFoam/TestMatrices/SHMmats-RCM/5krows/processor14
    # /various/dgal/graphs/matrices_openFoam/TestMatrices/SHMmats-RCM/5krows/processor42


    # "$path_selected"/thermomech_dK.mtx
    # "$path_selected"/ASIC_680k.mtx
    # "$path_selected"/xenon2.mtx
    # "$path_selected"/Si41Ge41H72.mtx
    # "$path_selected"/dense_1024.mtx 
    # "$path_selected"/dense_4096.mtx
    # "$path_selected"/in-2004.mtx
    # "$path_selected"/wikipedia-20051105.mtx
    # "$path_selected"/circuit5M.mtx
    # "$path_selected"/soc-LiveJournal1.mtx
    # "$path_selected"/dielFilterV3real.mtx

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

    for a in "${prog_args[@]}"
    do
        echo "File: $a"
        bench "$p" $a
    done
done

