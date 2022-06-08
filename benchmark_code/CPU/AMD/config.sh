#!/bin/bash

declare -A conf_vars
conf_vars=(
    # Maximum number of the machine's cores.
    # ['max_cores']=128
    ['max_cores']=8

    # Cores / Threads to utilize. Use spaces to define a set of different thread numbers to benchmark.
    # ['cores']=1
    # ['cores']=64
    # ['cores']=128
    # ['cores']='1 2 4 8 16 32 64 128'
    ['cores']=8
    # ['cores']='1 2 4 8'
    # ['cores']=6

    # Use hyperthreading.
    ['hyperthreading']=0
    # ['hyperthreading']=1

    # Path for the mkl library.
    ['MKL_PATH']='/opt/intel/oneapi/mkl/latest'

    # Path for the validation matrices.
    # ['path_validation']='/zhome/academic/HLRS/xex/xexdgala/Data/graphs/validation_matrices'
    # ['path_validation']='/home/jim/Data/graphs/validation_matrices'
    ['path_validation']='../../../validation_matrices/'

    # Benchmark with the artificially generated matrices (1) or the real validation matrices (0).
    # ['use_artificial_matrices']=0
    ['use_artificial_matrices']=1
)

path_artificial='../../../matrix_generation_parameters'

# Artificial matrices to benchmark.
artificial_matrices_files=(

    # Validation matrices artificial twins.
    # "$path_artificial"/validation_friends/twins_random.txt

    # Validation matrices artificial twins in a +-30% value space of each feature.
    # "$path_artificial"/SpMV-Research/validation_matrices_10_samples_30_range_twins.txt

    # The synthetic dataset studied in the paper.
    "$path_artificial"/synthetic_matrices_small_dataset.txt
)

# SpMV kernels to benchmark (uncomment the one you want).
progs=(
    # MKL IE
    './spmv_code_mkl-naive/spmv_sparse_mv.exe'

    # Custom naive
    './spmv_code_mkl-naive/spmv_csr_naive.exe'

    # CSR5
    './spmv_code_csr5/spmv_csr5.exe'

    # './spmv_code_mkl-naive/spmv_ell.exe'
    # './spmv_code_mkl-naive/spmv_ldu.exe'
    # './spmv_code_mkl-naive/spmv_csr_custom.exe'
    # './spmv_code_mkl-naive/spmv_csr_custom_vector.exe'
    # './spmv_code_mkl-naive/spmv_csr.exe'
    # './spmv_code_mkl-naive/spmv_dia.exe'
    # './spmv_code_mkl-naive/spmv_dia_custom.exe'
    # './spmv_code_mkl-naive/spmv_bsr_2.exe'
    # './spmv_code_mkl-naive/spmv_bsr_4.exe'
    # './spmv_code_mkl-naive/spmv_bsr_8.exe'
    # './spmv_code_mkl-naive/spmv_bsr_16.exe'
    # './spmv_code_mkl-naive/spmv_bsr_32.exe'
    # './spmv_code_mkl-naive/spmv_bsr_64.exe'
    # './spmv_code_mkl-naive/spmv_coo.exe'
)

# Export variables for make.
for index in "${!conf_vars[@]}"; do
    eval "$index='${conf_vars["$index"]}'"
    printf "%s=%s;" "$index"  "${conf_vars["$index"]}"
done

