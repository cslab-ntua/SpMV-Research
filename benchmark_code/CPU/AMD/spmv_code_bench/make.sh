#!/bin/bash

shopt -s -o pipefail
shopt -s extglob globstar dotglob nullglob 2>/dev/null
export GLOBIGNORE=.:..


export CPATH=''
export library=../../../../lib


script_dir="$(dirname "$(readlink -e "${BASH_SOURCE[0]}")")"
source "$script_dir"/../config.sh


export AMG_PATH=../../../../artificial-matrix-generator


if [[ -d "/home/jim/lib/gcc/gcc_12/bin" ]]; then
    gcc_bin=/home/jim/lib/gcc/gcc_12/bin/gcc
    gpp_bin=/home/jim/lib/gcc/gcc_12/bin/g++
elif [[ -d "/home/jim/Documents/gcc_versions/gcc_12/bin" ]]; then
    gcc_bin=/home/jim/Documents/gcc_versions/gcc_12/bin/gcc
    gpp_bin=/home/jim/Documents/gcc_versions/gcc_12/bin/g++
elif [[ -d "/various/dgal/gcc/gcc-12.2.0/gcc_bin/bin" ]]; then
    gcc_bin=/various/dgal/gcc/gcc-12.2.0/gcc_bin/bin/gcc
    gpp_bin=/various/dgal/gcc/gcc-12.2.0/gcc_bin/bin/g++
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

if [[ -d "${CUDA_PATH}/bin" ]]; then
    # NVCC="${CUDA_PATH}/bin/nvcc -ccbin=${CC}"
    NVCC="${CUDA_PATH}/bin/nvcc -ccbin=gcc"
else
    NVCC="nvcc -ccbin=${CC}"
fi
export NVCC

export ARCH="$(uname -m)"


CFLAGS=''
CFLAGS+=" -Wall -Wextra"
CFLAGS+=" -pipe"  # Tells the compiler to use pipes instead of temporary files (faster compilation, but uses more memory).
# CFLAGS+=" -Wno-unused-variable"
CFLAGS+=" -Wno-alloc-size-larger-than"
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

if [[ ${ARCH} == x86_64 ]]; then
    CFLAGS+=" -mbmi"
    CFLAGS+=" -mbmi2"
    CFLAGS+=" -march=native"
    # CFLAGS+=" -mfma"
    # CFLAGS+=" -mavx"
    # CFLAGS+=" -mavx2"
    # CFLAGS+=" -mavx512f"
elif [[ ${ARCH} == ppc64le ]]; then
    CFLAGS+=" -mcpu=power9"
else
    CFLAGS+=" -mcpu=native"
fi

CFLAGS+=" -I'${library}'"
CFLAGS+=" -I'${AMG_PATH}'"

CFLAGS+=" -D'INT_T=int32_t'"

if ((${PRINT_STATISTICS} == 1)); then
    CFLAGS+=" -D'PRINT_STATISTICS'"
fi

CFLAGS+=" -D'LEVEL1_DCACHE_LINESIZE=$(getconf LEVEL1_DCACHE_LINESIZE)'"
CFLAGS+=" -D'LEVEL1_DCACHE_SIZE=$(getconf LEVEL1_DCACHE_SIZE)'"
CFLAGS+=" -D'LEVEL2_CACHE_SIZE=$(getconf LEVEL2_CACHE_SIZE)'"
CFLAGS+=" -D'LEVEL3_CACHE_SIZE=$(getconf LEVEL3_CACHE_SIZE)'"

# Always read matrix in double-precision for checking accuracy against doubles.
CFLAGS+=" -D'MATRIX_MARKET_FLOAT_T=double'"


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


CFLAGS_D="${CFLAGS} -D'DOUBLE=1' -D'ValueType=double'"
CPPFLAGS_D="${CPPFLAGS} -D'DOUBLE=1' -D'ValueType=double'"

CFLAGS_F="${CFLAGS} -D'DOUBLE=0' -D'ValueType=float'"
CPPFLAGS_F="${CPPFLAGS} -D'DOUBLE=0' -D'ValueType=float'"

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
NVCCFLAGS+=' -arch=sm_80'
NVCCFLAGS+=' -DPERSISTENT_L2_PREFETCH'
# NVCCFLAGS+=' -lineinfo'

export NVCCFLAGS


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
# printf "%s\n" "${targets_d[@]}"
# echo
# printf "%s\n" "${targets_f[@]}"
# echo
# printf "%s\n" "${targets_nv_d[@]}"
# echo
# printf "%s\n" "${targets_nv_f[@]}"


unset MAKELEVEL

if ((${#targets_d[@]} > 0)); then
    export CFLAGS="${CFLAGS_D}"
    export CPPFLAGS="${CPPFLAGS_D}"
    export SUFFIX='_d'
    export TARGETS="${targets_d[*]}"
    make -f Makefile_in "$@"
fi

if ((${#targets_f[@]} > 0)); then
    export CFLAGS="${CFLAGS_F}"
    export CPPFLAGS="${CPPFLAGS_F}"
    export SUFFIX='_f'
    export TARGETS="${targets_f[*]}"
    make -f Makefile_in "$@"
fi

if ((${#targets_nv_d[@]} > 0)); then
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
		export NUM_STREAMS=;export NUM_THREADS=;export ROW_CLUSTER_SIZE=;export BLOCK_SIZE=;export NNZ_PER_THREAD=;export MULTIBLOCK_SIZE=;export COL_SIZE=;
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
            echo ${BLOCK_SIZE}
            echo ${MULTIBLOCK_SIZE}
        elif [[ $target =~ (s([0-9]+)_)?c([0-9]+)_b([0-9]+)_mb([0-9]+)${SUFFIX}.exe ]]; then # csr_cuda_adaptive_v2
            export NUM_STREAMS="${BASH_REMATCH[2]}"
            export COL_SIZE="${BASH_REMATCH[3]}"
            export BLOCK_SIZE="${BASH_REMATCH[4]}"
            export MULTIBLOCK_SIZE="${BASH_REMATCH[5]}"
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
    export CC=gcc
    export CPP=g++
    export CFLAGS="${CFLAGS_NV_F}"
    export CPPFLAGS="${CPPFLAGS_NV_F}"
    export SUFFIX='_nv_f'
    export TARGETS="${targets_nv_f[*]}"
    make -f Makefile_in "$@"
fi


