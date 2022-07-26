#!/bin/bash

script_dir="$(dirname "$(readlink -e "${BASH_SOURCE[0]}")")"
source "$script_dir"/config.sh
echo

if [[ $hyperthreading == 1 ]]; then
    max_cores=$((2*max_cores))
    cores="$cores $max_cores"
fi

# GOMP_CPU_AFFINITY pins the threads to specific cpus, even when assigning more cores than threads.
# e.g. with 'GOMP_CPU_AFFINITY=0,1,2,3' and 2 threads, the threads are pinned: t0->core0 and t1->core1.
if [[ $hyperthreading == 1 ]]; then
    affinity=''
    for ((i=0;i<max_cores/2;i++)); do
        affinity="$affinity,$i,$((i,max_cores/2+i))"
    done
    affinity="${affinity:1}"
    export GOMP_CPU_AFFINITY="$affinity"
    printf "cpu affinities: %s\n" "$affinity"
else
    export GOMP_CPU_AFFINITY="0-$((max_cores-1))"
fi

# export MKL_DEBUG_CPU_TYPE=5
# export LD_LIBRARY_PATH="${MKL_PATH}/lib/intel64:${LD_LIBRARY_PATH}"

# Encourages idle threads to spin rather than sleep.
# export OMP_WAIT_POLICY='active'
# Don't let the runtime deliver fewer threads than those we asked for.
# export OMP_DYNAMIC='false'

matrices_validation=(
    "$path_validation"/scircuit.mtx
    "$path_validation"/mac_econ_fwd500.mtx
    "$path_validation"/raefsky3.mtx
    "$path_validation"/bbmat.mtx
    "$path_validation"/conf5_4-8x8-15.mtx
    "$path_validation"/mc2depi.mtx
    "$path_validation"/rma10.mtx
    "$path_validation"/cop20k_A.mtx
    "$path_validation"/webbase-1M.mtx
    "$path_validation"/cant.mtx
    "$path_validation"/pdb1HYS.mtx
    "$path_validation"/TSOPF_RS_b300_c3.mtx
    "$path_validation"/Chebyshev4.mtx
    "$path_validation"/consph.mtx
    "$path_validation"/shipsec1.mtx
    "$path_validation"/PR02R.mtx
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
    "$path_validation"/ldoor.mtx
    "$path_validation"/circuit5M.mtx
    "$path_validation"/bone010.mtx
    "$path_validation"/cage15.mtx
)

bench()
{
    declare args=("$@")
    declare iter="${args[0]}"
    declare prog="${args[1]}"
    declare prog_args=("${args[@]:2}")
    declare t

    for t in $cores
    do
        export OMP_NUM_THREADS="$t"
        # export MKL_NUM_THREADS="$t"

        if [ $prog = "./spmv_code_merge/spmv_merge.exe" ]; then
            if ((!use_artificial_matrices)); then
                "$prog" --mtx="${prog_args[@]}"
            else
                prog_args2="${prog_args[@]}"  # need to replace the original prog_args  spaces with \(space), in order to be read as a string between " " for --artif_args argument to work! (shit...)
                # "$prog" --param="${prog_args2[@]}" 2>>arm_validation_30_friends_10_sample_merge_d.csv
                "$prog" --param="${prog_args2[@]}" 2>>arm_synthetic_merge_iter${iter}_t${t}_d.csv
            fi
        else
            "$prog" "${prog_args[@]}"
        fi

    done
}

matrices=(
    "${matrices_validation[@]}"
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
    echo "number of matrices: ${#prog_args[@]}"
fi

# iter=5
# for i in 1 2 3 4 5; do
for i in 2 3 4 5; do
    for p in "${progs[@]}"; do
        # declare base file_out file_err
        # base="${p/*\//}"
        # base="${base%%.*}"
        # file_out="out_${base}.out"
        # file_err="out_${base}.err"
        # > "$file_out"
        # > "$file_err"
        # exec 1>>"$file_out"
        # exec 2>>"$file_err"
        echo "program: $p"
        for a in "${prog_args[@]}"
        do
            if ((use_artificial_matrices)); then
                echo "Matrix: $a"
                bench $i $p $a
            else
                echo "File: $a"
                bench $i $p "$a"
            fi
        done
    done
done
