#!/bin/bash

shopt -s -o pipefail
shopt -s extglob globstar dotglob nullglob 2>/dev/null
export GLOBIGNORE=.:..


export CPATH=''
export library=../../../../lib


script_dir="$(dirname "$(readlink -e "${BASH_SOURCE[0]}")")"
source "$script_dir"/../config.sh


export AMG_PATH=../../../../artificial-matrix-generator


# if [[ -d "/home/jim/lib/gcc/gcc_12/bin" ]]; then
    # gcc_bin=/home/jim/lib/gcc/gcc_12/bin/gcc
    # gpp_bin=/home/jim/lib/gcc/gcc_12/bin/g++
# elif [[ -d "/home/jim/Documents/gcc_versions/gcc_12/bin" ]]; then
if [[ -d "/home/jim/Documents/gcc_versions/gcc_12/bin" ]]; then
    gcc_bin=/home/jim/Documents/gcc_versions/gcc_12/bin/gcc
    gpp_bin=/home/jim/Documents/gcc_versions/gcc_12/bin/g++
elif [[ -d "/various/dgal/gcc/gcc-12.2.0/gcc_bin/bin" ]]; then
    gcc_bin=/various/dgal/gcc/gcc-12.2.0/gcc_bin/bin/gcc
    gpp_bin=/various/dgal/gcc/gcc-12.2.0/gcc_bin/bin/g++
elif [[ -d "/opt/cray/pe/gcc/12.2.0/snos/bin/" ]]; then
    gcc_bin=/opt/cray/pe/gcc/12.2.0/snos/bin/gcc
    gpp_bin=/opt/cray/pe/gcc/12.2.0/snos/bin/g++
elif [[ -d "/local/pmpakos/arm-compiler/gcc-13.2.0_Ubuntu-22.04/bin" ]]; then
    gcc_bin=/local/pmpakos/arm-compiler/gcc-13.2.0_Ubuntu-22.04/bin/gcc
    gpp_bin=/local/pmpakos/arm-compiler/gcc-13.2.0_Ubuntu-22.04/bin/g++
else
    gcc_bin=gcc
    gpp_bin=g++
fi


CC="$gcc_bin"
# CC=clang
# CC=xlc
export CC

CPP="$gpp_bin"
# CPP=clang++
# CPP=xlc++
export CPP
CXX="$CPP"
export CXX

if [[ -d "${CUDA_PATH}/bin" ]]; then
    NVCC="${CUDA_PATH}/bin/nvcc -ccbin=${CC}"
else
    NVCC="nvcc -ccbin=${CC}"
fi
export NVCC

if [[ -d "${ROCM_PATH}/bin" ]]; then
    HIPCC="${ROCM_PATH}/bin/hipcc"
else
    HIPCC="hipcc"
fi
export HIPCC

export ARCH="$(uname -m)"


CFLAGS=''
CFLAGS+=" -Wall -Wextra"
CFLAGS+=" -pipe"  # Tells the compiler to use pipes instead of temporary files (faster compilation, but uses more memory).
# CFLAGS+=" -Wno-unused-variable"
CFLAGS+=" -Wno-alloc-size-larger-than"
CFLAGS+=" -fno-unsigned-char"
CFLAGS+=" -fopenmp"

CFLAGS+=" -D'_GNU_SOURCE'"

# CFLAGS+=" -g"
# CFLAGS+=" -g3 -fno-omit-frame-pointer"
# CFLAGS+=" -Og"
# CFLAGS+=" -O0"
# CFLAGS+=" -O2"
CFLAGS+=" -O3"

# CFLAGS+=" -ffast-math"

CFLAGS+=" -flto=auto"

CFLAGS+=" -fsigned-char"

if [[ ${ARCH} == x86_64 ]]; then
    CFLAGS+=" -mbmi"
    CFLAGS+=" -mbmi2"
    CFLAGS+=" -march=native"
    CFLAGS+=" -mno-avx512fp16"
    # CFLAGS+=" -mfma"
    # CFLAGS+=" -mavx"
    # CFLAGS+=" -mavx2"
    # CFLAGS+=" -mavx512f"
elif [[ ${ARCH} == ppc64le ]]; then
    CFLAGS+=" -mcpu=power9"
elif [[ ${ARCH} == aarch64 ]]; then
    CFLAGS+=" -march=native"
    CFLAGS+=" -flax-vector-conversions"
    # CFLAGS+=" -msve-vector-bits=512"
    # CFLAGS+=" -msve-vector-bits=256"
    CFLAGS+=" -msve-vector-bits=128"
else
    CFLAGS+=" -mcpu=native"
fi

CFLAGS+=" -I'${library}'"
CFLAGS+=" -I'${AMG_PATH}'"

CFLAGS+=" -D'INT_T=int32_t'"

if ((${PRINT_STATISTICS} == 1)); then
    CFLAGS+=" -D'PRINT_STATISTICS'"
fi

export LEVEL1_DCACHE_LINESIZE="$(read v < /sys/devices/system/cpu/cpu0/cache/index0/coherency_line_size; echo ${v%K})"
export LEVEL1_DCACHE_SIZE="$(read v < /sys/devices/system/cpu/cpu0/cache/index0/size; echo $((${v%K} * 1024)) )"
export LEVEL2_CACHE_SIZE="$(read v < /sys/devices/system/cpu/cpu0/cache/index2/size; echo $((${v%K} * 1024)) )"
export LEVEL3_CACHE_SIZE="$(read v < /sys/devices/system/cpu/cpu0/cache/index3/size; echo $((${v%K} * 1024)) )"
export NUM_CPUS="$(ls /sys/devices/system/cpu/ | grep -c 'cpu[[:digit:]]\+')"
export LEVEL3_CACHE_CPUS_PER_NODE="$(for range in $(cat /sys/devices/system/cpu/cpu0/cache/index3/shared_cpu_list | tr ',' ' '); do mapfile -t -d '-' a < <(printf "$range"); seq "${a[@]}"; done | wc -l)"
export LEVEL3_CACHE_NUM_NODES="$(( NUM_CPUS / LEVEL3_CACHE_CPUS_PER_NODE ))"
export LEVEL3_CACHE_SIZE_TOTAL="$(( LEVEL3_CACHE_SIZE * LEVEL3_CACHE_NUM_NODES ))"
CFLAGS+=" -D'LEVEL1_DCACHE_LINESIZE=${LEVEL1_DCACHE_LINESIZE}ULL'"
CFLAGS+=" -D'LEVEL1_DCACHE_SIZE=${LEVEL1_DCACHE_SIZE}ULL'"
CFLAGS+=" -D'LEVEL2_CACHE_SIZE=${LEVEL2_CACHE_SIZE}ULL'"
CFLAGS+=" -D'LEVEL3_CACHE_SIZE=${LEVEL3_CACHE_SIZE}ULL'"
CFLAGS+=" -D'NUM_CPUS=${NUM_CPUS}ULL'"
CFLAGS+=" -D'LEVEL3_CACHE_CPUS_PER_NODE=${LEVEL3_CACHE_CPUS_PER_NODE}ULL'"
CFLAGS+=" -D'LEVEL3_CACHE_NUM_NODES=${LEVEL3_CACHE_NUM_NODES}ULL'"
CFLAGS+=" -D'LEVEL3_CACHE_SIZE_TOTAL=${LEVEL3_CACHE_SIZE_TOTAL}ULL'"


# Read the matrix in double-precision for checking accuracy against doubles.
CFLAGS+=" -D'ValueTypeReference=double'"
CFLAGS+=" -D'MATRIX_MARKET_FLOAT_T=ValueTypeReference'"


CPPFLAGS=''
CPPFLAGS+=" ${CFLAGS}"

if [[ ${CC} == xlc ]]; then
    CFLAGS+=" -Wno-typedef-redefinition"
    CFLAGS+=" -D'__XLC__'"
fi
if [[ ${CPP} == xlc++ ]]; then
    CPPFLAGS+=" -std=c++11"
    CPPFLAGS+=" -D'__XLC__'"
fi


CFLAGS_D="${CFLAGS} -D'DOUBLE=1' -D'ValueType=double' -D'ValueTypeI=uint64_t'"
CPPFLAGS_D="${CPPFLAGS} -D'DOUBLE=1' -D'ValueType=double' -D'ValueTypeI=uint64_t'"

CFLAGS_F="${CFLAGS} -D'DOUBLE=0' -D'ValueType=float' -D'ValueTypeI=uint32_t'"
CPPFLAGS_F="${CPPFLAGS} -D'DOUBLE=0' -D'ValueType=float' -D'ValueTypeI=uint32_t'"

CFLAGS_NV_D="${CFLAGS_D} -fno-lto"
CPPFLAGS_NV_D="${CPPFLAGS_D} -fno-lto"

CFLAGS_NV_F="${CFLAGS_F} -fno-lto"
CPPFLAGS_NV_F="${CPPFLAGS_F} -fno-lto"


LDFLAGS=''
LDFLAGS+=" -lm"

export LDFLAGS


NVCCFLAGS=
NVCCFLAGS+=" -allow-unsupported-compiler"
# NVCCFLAGS+=" --dlink-time-opt"
NVCCFLAGS+=' -gencode arch=compute_80,code=sm_80'
NVCCFLAGS+=' -DPERSISTENT_L2_PREFETCH'
# NVCCFLAGS+=' -lineinfo'
# NVCCFLAGS+=' -G -g'
# NVCCFLAGS+=' --ptxas-options=-v'
export NVCCFLAGS

HIPCCFLAGS=
# HIPCCFLAGS+=' --offload-arch=gfx90a' # MI250
HIPCCFLAGS+=' --offload-arch=native' # MI250
export HIPCCFLAGS

export NUM_STREAMS=;export NUM_THREADS=;export ROW_CLUSTER_SIZE=;export BLOCK_SIZE=;export NNZ_PER_THREAD=;export MULTIBLOCK_SIZE=;



targets_d=()
targets_f=()
targets_nv_d=()
targets_nv_f=()

for p in "${progs[@]}"; do
    base="$(basename "$p")"
    if   [[ "$base" =~ _nv_d.exe ]]; then
        targets_nv_d+=("$base")
    elif [[ "$base" =~ _nv_f.exe ]]; then
        targets_nv_f+=("$base")
    elif [[ "$base" =~ _d.exe ]]; then
        targets_d+=("$base")
    elif [[ "$base" =~ _f.exe ]]; then
        targets_f+=("$base")
    fi
done

export NUM_THREADS=
export NUM_STREAMS=
export ROW_CLUSTER_SIZE=
export BLOCK_SIZE=
export NNZ_PER_THREAD=
export MULTIBLOCK_SIZE=


unset MAKELEVEL

if ((${#targets_d[@]} > 0)); then
    export DOUBLE=1
    export CFLAGS="${CFLAGS_D}"
    export CPPFLAGS="${CPPFLAGS_D}"
    export SUFFIX='_d'
    export TARGETS="${targets_d[*]}"
    make -f Makefile_in "$@"
fi

if ((${#targets_f[@]} > 0)); then
    export DOUBLE=0
    export CFLAGS="${CFLAGS_F}"
    export CPPFLAGS="${CPPFLAGS_F}"
    export SUFFIX='_f'
    export TARGETS="${targets_f[*]}"
    make -f Makefile_in "$@"
fi

if ((${#targets_nv_d[@]} > 0)); then
    export DOUBLE=1
    export CC=gcc
    export CPP=g++
    export CFLAGS="${CFLAGS_NV_D}"
    export CPPFLAGS="${CPPFLAGS_NV_D}"
    export SUFFIX='_nv_d'
    export TARGETS="${targets_nv_d[*]}"
    export TIME_IT=0
    # make -f Makefile_in "$@"
    for target in $TARGETS; do
        echo $target
        # need to handle each target separately to extract compilation parameters as env variables
        if [[ $target =~ (s([0-9]+)_)?t([0-9]+)_rc([0-9]+)${SUFFIX}.exe ]]; then # csr_cuda_buffer
            export NUM_STREAMS="${BASH_REMATCH[2]}"
            export NUM_THREADS="${BASH_REMATCH[3]}"
            export ROW_CLUSTER_SIZE="${BASH_REMATCH[4]}"
        elif [[ $target =~ (s([0-9]+)_)?b([0-9]+)_nnz([0-9]+)${SUFFIX}.exe ]]; then # csr_cuda_const_nnz_per_thread
            export NUM_STREAMS="${BASH_REMATCH[2]}"
            export BLOCK_SIZE="${BASH_REMATCH[3]}"
            export NNZ_PER_THREAD="${BASH_REMATCH[4]}"
        elif [[ $target =~ (s([0-9]+)_)?b([0-9]+)_mb([0-9]+)${SUFFIX}.exe ]]; then # csr_cuda_adaptive
            export NUM_STREAMS="${BASH_REMATCH[2]}"
            export BLOCK_SIZE="${BASH_REMATCH[3]}"
            export MULTIBLOCK_SIZE="${BASH_REMATCH[4]}"
        elif [[ $target =~ _s([0-9]+)${SUFFIX}.exe ]]; then # cusparse_csr (stream variant only)
            export NUM_STREAMS="${BASH_REMATCH[1]}"
        elif [[ $target =~ (s([0-9]+)_)?t([0-9]+)${SUFFIX}.exe ]]; then # csr_cuda
            export NUM_STREAMS="${BASH_REMATCH[2]}"
            export NUM_THREADS="${BASH_REMATCH[3]}"
        elif [[ $target =~ (s([0-9]+)_)?b([0-9]+)${SUFFIX}.exe ]]; then # csr_cuda_vector
            export NUM_STREAMS="${BASH_REMATCH[2]}"
            export BLOCK_SIZE="${BASH_REMATCH[3]}"
        fi

        make -f Makefile_in "$target"
    done
fi

if ((${#targets_nv_f[@]} > 0)); then
    export DOUBLE=0
    export CC=gcc
    export CPP=g++
    export CFLAGS="${CFLAGS_NV_F}"
    export CPPFLAGS="${CPPFLAGS_NV_F}"
    export SUFFIX='_nv_f'
    export TARGETS="${targets_nv_f[*]}"
    make -f Makefile_in "$@"
fi


