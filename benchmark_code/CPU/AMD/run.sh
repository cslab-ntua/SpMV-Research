#!/bin/bash

source config.sh
echo


if [[ $hyperthreading == 1 ]]; then
    max_cores=$((2*max_cores))
    cores="$cores $max_cores"
fi


# GOMP_CPU_AFFINITY pins the threads to specific cpus, even when assigning more cores than threads.
# e.g. with 'GOMP_CPU_AFFINITY=0,1,2,3' and 2 threads, the threads are pinned: t0->core0 and t1->core1.
# export GOMP_CPU_AFFINITY="0-$((max_cores-1))"
# export KMP_AFFINITY="granularity=fine,proclist=[0,1,2,3,4,5,6,7], explicit, verbose"
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


export MKL_DEBUG_CPU_TYPE=5


# export LD_LIBRARY_PATH="/opt/hlrs/spack/current/papi/c048e224f-gcc-9.2.0-hxfnx7kt/lib:/opt/intel/oneapi/mkl/latest/lib/intel64:${LD_LIBRARY_PATH}"
export LD_LIBRARY_PATH="${MKL_PATH}/lib/intel64:${LD_LIBRARY_PATH}"


# Encourages idle threads to spin rather than sleep.
# export OMP_WAIT_POLICY='active'
# Don't let the runtime deliver fewer threads than those we asked for.
# export OMP_DYNAMIC='false'

interleaved=()
# interleaved=('numactl' '-i' 'all')


# amduprof_out='/zhome/academic/HLRS/xex/xexdgala/uprof_out.txt'
# amduprof_out='$HOME/uprof_out.txt'

# amduprof_exe='AMDuProfCLI'
amduprof_exe='/home/jim/Documents/Synced_Documents/apps/AMDuProf_Linux_x64_3.4.475/bin/AMDuProfCLI'

# amduprof=()
# amduprof=($amduprof_exe timechart --event power --interval 100)

# amduprof output hawk:
#    /opt/hlrs/non-spack/performance/uprof/3.2.228/bin/AMDuProfCLI
#    ERROR: Power profiler not supported in the current configuration.


matrices_validation=(
    "$path_validation"/scircuit.sorted.mtx
    "$path_validation"/mac_econ_fwd500.sorted.mtx
    "$path_validation"/raefsky3.sorted.mtx
    "$path_validation"/bbmat.sorted.mtx
    "$path_validation"/conf5_4-8x8-15.real.mtx
    "$path_validation"/mc2depi.real.mtx
    "$path_validation"/rma10.sorted.mtx
    "$path_validation"/cop20k_A.sorted.mtx
    "$path_validation"/webbase-1M.sorted.mtx
    "$path_validation"/cant.sorted.mtx
    "$path_validation"/pdb1HYS.sorted.mtx
    "$path_validation"/TSOPF_RS_b300_c3.sorted.mtx
    "$path_validation"/Chebyshev4.sorted.mtx
    "$path_validation"/consph.sorted.mtx
    "$path_validation"/shipsec1.sorted.mtx
    "$path_validation"/PR02R.sorted.mtx
    "$path_validation"/mip1.sorted.mtx
    "$path_validation"/rail4284.real.mtx
    "$path_validation"/pwtk.sorted.mtx
    "$path_validation"/crankseg_2.sorted.mtx
    "$path_validation"/Si41Ge41H72.sorted.mtx
    "$path_validation"/TSOPF_RS_b2383.sorted.mtx
    "$path_validation"/in-2004.sorted.mtx
    "$path_validation"/Ga41As41H72.sorted.mtx
    "$path_validation"/eu-2005.sorted.mtx
    "$path_validation"/wikipedia-20051105.sorted.mtx
    "$path_validation"/rajat31.sorted.mtx
    "$path_validation"/ldoor.sorted.mtx
    "$path_validation"/circuit5M.sorted.mtx
    "$path_validation"/bone010.sorted.mtx
    "$path_validation"/cage15.sorted.mtx
)


bench()
{
    declare args=("$@")
    declare prog="${args[0]}"
    declare prog_args=("${args[@]:1}")
    declare m
    declare t

    # group_sizes="$cores"
    group_sizes="1"

    for group_size in $group_sizes; do                      
        # echo "group size: $group_size"                
        export OMP_MAX_THREAD_GROUP_SIZE="$group_size"

        # for t in 1
        # for t in 8
        # for t in $max_cores
        for t in $cores
        do
            export OMP_NUM_THREADS="$t"
            # export MKL_NUM_THREADS="$t"

            # amduprof_out="$HOME/uprof_out_${prog/*\//}_${f/.mtx/}.txt"

            # "${interleaved[@]}" "$prog" 4690000 4 1.6 normal random 1 14
            "${interleaved[@]}" "$prog" "${prog_args[@]}"

            # "${interleaved[@]}" "$prog" "$m" 4000 105 0.02 gamma random 1 14
            # "${amduprof[@]}" -o "$amduprof_out" "$prog" "$m"

            # numactl --physcpubind=0-"$((t-1))" -- "$prog" "$m"
            # numactl --physcpubind=0-"$((max_cores-1))" -- "$prog" "$m"
            # numactl -i all --physcpubind=0-"$((t-1))" -- "$prog" "$m"
            # numactl -i all --physcpubind=0-"$((max_cores-1))" -- "$prog" "$m"

            # taskset -c 0-"$((max_cores-1))" "$prog" "$m"
        done

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

# prog_args=(
    # This bugs at 128 threads with mkl and mkl_sparse_set_mv_hint() for some reason.
    # '28508159 28508159 5 1.6667 normal random 0.05 0 0.05 0.05 14'
# )


for p in "${progs[@]}"; do

    # declare base file_out
    # base="${p/*\//}"
    # base="${base%%.*}"
    # file_out="out_${base}.out"
    # > "$file_out"
    # exec 1>>"$file_out"

    echo "program: $p"

    for a in "${prog_args[@]}"
    do
        if ((use_artificial_matrices)); then
            echo "File: $a"
            bench $p $a
        else
            echo "File: $a"
            bench $p "$a"
        fi
    done
done

