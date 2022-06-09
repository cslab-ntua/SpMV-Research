#!/bin/bash

# CHECKME: Cmake command (with path if needed for local installations etc) 
cmake_command=~/Lib_install/cmake-3.20/bin/cmake

# CHECKME: A desired name for the GPU testbed to be used for your build-dirs and logfiles.
system='silver1V100'

# CHECKME: Define cuda architecture 80") # (Tesla K40 = 35, GTX 1060/70 = 61,) P100 = 60, V100 = 70, A100 = 80
export cnf_CUDA_arch=70

# CHECKME: Flag ( 0 = no, 1 = yes) declaring if cuda-9 benchmarks should be included (compilation not supported in latest systems like A100)
run_cuda_9=0

# CHECKME: Path for the cuda toolkit root
if ((run_cuda_9)); then
	export cnf_CUDA_TOOLKIT_9_DIR='/usr/local/cuda-9.2'
fi
export cnf_CUDA_TOOLKIT_11_DIR='/usr/local/cuda-11.6'

# CHECKME: CUDA Library paths, in case the benchmark system (or modules) do not load them correctly, or (either) CUDA is installed locally and requires linking by hand.
#if ((run_cuda_9)); then
#    export LD_LIBRARY_PATH="path_to_cuda_9/lib64:path_to_cuda_9/lib:$LD_LIBRARY_PATH"
#fi
#export LD_LIBRARY_PATH="path_to_cuda_11/lib64:path_to_cuda_11/lib:$LD_LIBRARY_PATH"

# CHECKME: Define datatype used for benchmarks 0 = float, 1 = double. Double used in paper. 
export cnf_dtype_id=1

# CHECKME: Benchmark with the artificially generated matrices (1) or the real validation matrices (0).
# ['use_artificial_matrices']=0
use_artificial_matrices=1

# Path for the matrix generation parameters.
path_artificial='../../../matrix_generation_parameters'

# CHECKME: Artificial matrices to benchmark. 2 choises for full set or validation 'friends'
if ((use_artificial_matrices)); then
	# The synthetic dataset studied in the paper.
	filename_artificial=synthetic_matrices_small_dataset

	# Validation matrices artificial twins in a +-30% value space of each feature.
	# filename_artificial=validation_matrices_10_samples_30_range_twins

		
	artificial_matrices_files=( "$path_artificial"/"$filename_artificial.txt" )
fi

# CHECKME: Ideally you don't want to edit bellow here...
# Unless you want to change which implementations/libraries to run (lines 91-121) or mess with paths (don't). 
# =========================================================================================================================================
# =========================================================================================================================================

# Path for the validation matrices.
path_validation='../../../../validation_matrices'

if ((cnf_dtype_id)); then
	dtype=D
else
	dtype=S
fi

if ((use_artificial_matrices)); then
	progtype_string=generate
else
	progtype_string=mtx
fi

if ((run_cuda_9)); then
	progs=(
		# cuSPARSE 9 hyb 
		"../spmv_code_cusparse-9.x/${system}-build/cuSPARSE${dtype}hybmv_9_${progtype_string}"

		# CSR5 cuda 9
		"../spmv_code_csr5_cuda/integrated_csr5_wrap_operator/${system}-build/CSR5_CUDA_${dtype}SPMV_9_${progtype_string}"
		
		# cuSPARSE 11 coo
		"../spmv_code_cusparse-11.x/${system}-build/cuSPARSE${dtype}coomv_11_${progtype_string}"
		
		# cuSPARSE 11 csr
		"../spmv_code_cusparse-11.x/${system}-build/cuSPARSE${dtype}csrmv_11_${progtype_string}"

		## Other options not used in paper
		# cuSPARSE 9 csr ( <= perf to cuSPARSE 11 csr) 
		#"./spmv_code_cusparse-9.x/${system}-build/cuSPARSE${dtype}csrmv_9_${progtype_string}"
		# cuSPARSE 9 bsr 
		#"./spmv_code_cusparse-9.x/${system}-build/cuSPARSE${dtype}bsrmv_9_${progtype_string}"    

	)
else
	progs=(
		# cuSPARSE 11 coo
		"../spmv_code_cusparse-11.x/${system}-build/cuSPARSE${dtype}coomv_11_${progtype_string}"
		
		# cuSPARSE 11 csr
		"../spmv_code_cusparse-11.x/${system}-build/cuSPARSE${dtype}csrmv_11_${progtype_string}"
  
	)
fi
