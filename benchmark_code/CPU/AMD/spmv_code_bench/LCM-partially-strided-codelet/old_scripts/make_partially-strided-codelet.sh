#!/bin/bash


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


mkl_path="$( options=(
                '/opt/intel/oneapi/mkl/latest'
                '/various/common_tools/intel_parallel_studio/compilers_and_libraries/linux/mkl'
                "${HOME}/spack/23.03/0.20.0/intel-oneapi-mkl-2023.1.0-cafkcjc/mkl/latest"
            )
            find_valid_dir "${options[@]}"
        )"

# notes:
#  - include <cstdint> in file 'partially-strided-codelet/demo/SerialSpMVExecutor.h'
#
#  - comment out in partially-strided-codelet/include/Inspector.h
#    line 68 that prints thousands of lines.

hash module 2>/dev/null && module load gcc/12.2.0

# git pull --recurse-submodules
# rm -rf build
# mkdir -p build
# cd build
# cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH="${mkl_path}/lib/intel64/;${mkl_path}/include/" ..
# make -j32

