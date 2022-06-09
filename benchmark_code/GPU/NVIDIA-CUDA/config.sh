#!/bin/bash

declare -A conf_vars
conf_vars=(

    # A desired name for the GPU testbed to be used for your build-dirs and logfiles.
    ['system']='silver1V100'
    
     # Flag ( 0 = no, 1 = yes) declaring if cuda-9 benchmarks should be included (compilation not supported in latest systems like A100)
    ['run_cuda_9']=1
    
    # Define cuda architecture 80") # (Tesla K40 = 35, GTX 1060/70 = 61,) P100 = 60, V100 = 70, A100 = 80
    ['CUDA_arch']=70
  
    # Define datatype used for benchmarks 0 = float, 1 = double
    ['dtype_id']=1  
    
    # Path for the cuda toolkit root
    ['CUDA_TOOLKIT_DIR']='/usr/local/cuda-11.6'

    # Path for the validation matrices.
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

# Export variables for make.
for index in "${!conf_vars[@]}"; do
    eval "$index='${conf_vars["$index"]}'"
    printf "%s=%s;" "$index"  "${conf_vars["$index"]}"
done

path_generator='../../../artificial-matrix-generator'
cd $path_generator
make
cd -

mkdir -p ./spmv_code_cusparse-11.x/${system}-build
cp ./spmv_code_cusparse-11.x/CMakeLists.txt ./spmv_code_cusparse-11.x/${system}-build/CMakeLists.txt
cd ./spmv_code_cusparse-11.x/${system}-build
cmake ./
cd -

if ((run_cuda_9)); then
	mkdir -p ./spmv_code_cusparse-9.x/${system}-build
	cp ./spmv_code_cusparse-9.x/CMakeLists.txt ./spmv_code_cusparse-9.x/${system}-build/CMakeLists.txt
	cd ./spmv_code_cusparse-9.x/${system}-build
	cmake ./
	cd -
	
	mkdir -p ./spmv_code_csr5_cuda/integrated_csr5_wrap_operator/${system}-build
	cp ./spmv_code_csr5_cuda/integrated_csr5_wrap_operator/CMakeLists.txt ./spmv_code_csr5_cuda/integrated_csr5_wrap_operator/${system}-build/CMakeLists.txt
	cd ./spmv_code_csr5_cuda/integrated_csr5_wrap_operator/${system}-build
	cmake ./
	cd -
fi

if ((dtype_id)); then
	dtype=D
else
	dtype=S
fi

if ((use_artificial_matrices)); then
	progtype_string=generate
else
	progtype_string=mtx
fi

# SpMV kernels to benchmark (uncomment the ones you want).
progs=(
    # cuSPARSE 9 hyb 
    "./spmv_code_cusparse-9.x/${system}-build/cuSPARSE${dtype}hybmv_9_${progtype_string}"

    # CSR5 cuda 9
    "./spmv_code_csr5_cuda/integrated_csr5_wrap_operator/${system}-build/CSR5_CUDA_${dtype}SPMV_9_${progtype_string}"
    
    # cuSPARSE 11 coo
    "./spmv_code_cusparse-11.x/${system}-build/cuSPARSE${dtype}coomv_11_${progtype_string}"
    
    # cuSPARSE 11 csr
    "./spmv_code_cusparse-11.x/${system}-build/cuSPARSE${dtype}csrmv_11_${progtype_string}"

    ## Other options not used in paper
    # cuSPARSE 9 csr ( <= perf to cuSPARSE 11 csr) 
    #"./spmv_code_cusparse-9.x/${system}-build/cuSPARSE${dtype}csrmv_9_${progtype_string}"
    # cuSPARSE 9 bsr 
    #"./spmv_code_cusparse-9.x/${system}-build/cuSPARSE${dtype}bsrmv_9_${progtype_string}"    

)

