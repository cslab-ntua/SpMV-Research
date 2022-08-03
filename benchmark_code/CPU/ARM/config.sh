#!/bin/bash

# To reproduce results, this environment variable has to change to the root directory of the ARM Compiler (<ARM-Compiler-Path> in README.md)
# export ARM_ROOT_DIR="<ARM-Compiler-Path>"
export ARM_ROOT_DIR=/home/spmv/arm/

export ARMCLANG_ROOT_DIR=${ARM_ROOT_DIR}/arm-linux-compiler-22.0.1_Generic-AArch64_Ubuntu-20.04_aarch64-linux
export GCC_ROOT_DIR=${ARM_ROOT_DIR}/gcc-11.2.0_Generic-AArch64_Ubuntu-20.04_aarch64-linux
export ARMPL_ROOT_DIR=${ARM_ROOT_DIR}/armpl-22.0.1_AArch64_Ubuntu-20.04_arm-linux-compiler_aarch64-linux/

# These are environment variables that have to be set for SparseX to work
# Need to install specific library versions
export SPARSEX_ROOT_DIR=/home/spmv/sparsex/
export BOOST_LIB_PATH=${SPARSEX_ROOT_DIR}/boost_1_55_0/local/lib/
export LLVM_LIB_PATH=${SPARSEX_ROOT_DIR}/llvm-6.0.0/build/lib/

export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${BOOST_LIB_PATH}:${LLVM_LIB_PATH}

declare -A conf_vars
conf_vars=(
	# Cores / Threads to utilize. Use spaces to define a set of different thread numbers to benchmark.
	['cores']=80
	
	# Maximum number of the machine's cores.
	['max_cores']=160
	
	# Use hyperthreading.
	['hyperthreading']=0

	# Path for the validation matrices.
	# ['path_validation']='/various/pmpakos/SpMV-Research/validation_matrices/'
	['path_validation']='../../../validation_matrices/'

	# Benchmark with the artificially generated matrices (1) or the real validation matrices (0).
	['use_artificial_matrices']=0
	# ['use_artificial_matrices']=1

    # Rapl registers.
    # ['RAPL_REGISTERS']='0'         # 1 socket : Epyc1, Gold
    # ['RAPL_REGISTERS']='0,1'       # 2 sockets: Epyc1, Gold

	########################################################################################################
	# SparseX ecosystem environment variables that have to be set
	['BOOST_LIB_PATH']=${BOOST_LIB_PATH}
	['LLVM_LIB_PATH']=${LLVM_LIB_PATH}
	['SPARSEX_ROOT_DIR']=${SPARSEX_ROOT_DIR}

	########################################################################################################
	# ARM ecosystem environment variables that have to be set
	['ARM_ROOT_DIR']=${ARM_ROOT_DIR}

	['ARMCLANG_ROOT_DIR']=${ARMCLANG_ROOT_DIR}
	['ARM_LINUX_COMPILER_LIBRARIES']=${ARMCLANG_ROOT_DIR}/lib
	['ARM_HPC_COMPILER_LICENSE_SEARCH_PATH']=$ARM_ROOT_DIR/licenses
	['ARM_LICENSE_DIR']=$ARM_ROOT_DIR/licences
	['ARMPL_ROOT_DIR']=${ARMPL_ROOT_DIR}

	# Arm-specific environment variables
	['ARM_LINUX_COMPILER_DIR']=${ARMCLANG_ROOT_DIR}
	['ARM_LINUX_COMPILER_BUILD']=1630
	['ARM_LINUX_COMPILER_INCLUDES']=${ARMCLANG_ROOT_DIR}/includes

	# GNU-specific environment variables
	['GCC_ROOT_DIR']=${GCC_ROOT_DIR}
	['GCC_BUILD']=213
	['GCC_DIR']=${GCC_ROOT_DIR}
	['GCC_INCLUDES']=${GCC_ROOT_DIR}/include
	['COMPILER_PATH']=${GCC_ROOT_DIR}

	# Standard environment variables
	['PATH']=${PATH}:${GCC_ROOT_DIR}/binutils_bin:${ARMCLANG_ROOT_DIR}/bin
	['CPATH']=${CPATH}:${GCC_ROOT_DIR}/include:${ARMCLANG_ROOT_DIR}/include:${ARMPL_ROOT_DIR}/include_common
	['LD_LIBRARY_PATH']=${LD_LIBRARY_PATH}:${GCC_ROOT_DIR}/lib:${GCC_ROOT_DIR}/lib64:${ARMCLANG_ROOT_DIR}/lib:${ARMCLANG_ROOT_DIR}/lib/clang/13.0.0/armpl_links/lib:${ARMPL_ROOT_DIR}/lib
	['LIBRARY_PATH']=${LIBRARY_PATH}:${GCC_ROOT_DIR}/lib:${GCC_ROOT_DIR}/lib64:${ARMCLANG_ROOT_DIR}/lib:${ARMPL_ROOT_DIR}/lib
	['MANPATH']=${MANPATH}:${GCC_ROOT_DIR}/share/man:${ARMCLANG_ROOT_DIR}/share/man

	# Arm-PL-specific environment variables
	['ARMPL_LIBRARIES']=${ARMPL_ROOT_DIR}/lib
	['ARMPL_BUILD']=870
	['ARMPL_DIR']=${ARMPL_ROOT_DIR}
	['ARMPL_INCLUDES']=${ARMPL_ROOT_DIR}/include
	['ARMPL_INCLUDES_ILP64']=${ARMPL_ROOT_DIR}/include_ilp64
	['ARMPL_INCLUDES_ILP64_MP']=${ARMPL_ROOT_DIR}/include_ilp64_mp
	['ARMPL_INCLUDES_INT64']=${ARMPL_ROOT_DIR}/include_ilp64
	['ARMPL_INCLUDES_INT64_MP']=${ARMPL_ROOT_DIR}/include_ilp64_mp
	['ARMPL_INCLUDES_LP64_MP']=${ARMPL_ROOT_DIR}/include_lp64_mp
	['ARMPL_INCLUDES_MP']=${ARMPL_ROOT_DIR}/include_lp64_mp

	# Standard environment variables for BLAS and LAPACK
	['BLAS']=:${ARMPL_ROOT_DIR}/lib/libarmpl_lp64.a
	['BLAS_SHARED']=${ARMPL_ROOT_DIR}/lib/libarmpl_lp64.so
	['BLAS_STATIC']=${ARMPL_ROOT_DIR}/lib/libarmpl_lp64.a
	['LAPACK']=${ARMPL_ROOT_DIR}/lib/libarmpl_lp64.a
	['LAPACK_SHARED']=${ARMPL_ROOT_DIR}/lib/libarmpl_lp64.so
	['LAPACK_STATIC']=${ARMPL_ROOT_DIR}/lib/libarmpl_lp64.a
	########################################################################################################
)

# path_artificial='/various/pmpakos/SpMV-Research/matrix_generation_parameters/'
path_artificial='../../../matrix_generation_parameters/'

# Artificial matrices to benchmark.
artificial_matrices_files=(
	# Validation matrices artificial twins.
	# "$path_artificial"/validation_friends/twins_random.txt

	# Validation matrices artificial twins in a +-30% value space of each feature.
	# "$path_artificial"/validation_matrices_10_samples_30_range_twins.txt

	# The synthetic dataset studied in the paper.
	"$path_artificial"/synthetic_matrices_small_dataset.txt
	# "$path_artificial"/synthetic_matrices_small_dataset2.txt
	# "$path_artificial"/synthetic_matrices_small_dataset3.txt
)

# SpMV kernels to benchmark.
progs=(
	# ./spmv_code/spmv_csr_naive.exe
	# ./spmv_code/spmv_armpl.exe
	# ./spmv_code_merge/spmv_merge.exe
	./spmv_code_sparsex/spmv_sparsex.exe
)

# Export variables for make.
for index in "${!conf_vars[@]}"; do
	eval "export $index='${conf_vars["$index"]}'"
	config_str="${config_str}${index}=${conf_vars["$index"]};"
	# printf "%s=%s;" "$index"  "${conf_vars["$index"]}"
done
printf "%s" "$config_str"
