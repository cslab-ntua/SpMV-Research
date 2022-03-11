#!/bin/bash

module load compiler/gnu/7.3.0
module load system/cuda/10.1.243
source ${HOME}/.bashrc

system=vulcanP100

# CUDA Library paths
export LD_LIBRARY_PATH="/opt/system/cuda/10.1.243/lib64:/opt/system/cuda/10.1.243/lib:$LD_LIBRARY_PATH"
export LD_LIBRARY_PATH="/opt/system/cuda/9.2.88/lib64:/opt/system/cuda/9.2.88/lib:$LD_LIBRARY_PATH"

# TODO: Define these variables for your system
MASTEDIR=/zhome/academic/HLRS/xex/xexpanas/SpMV-Research
export LD_LIBRARY_PATH="${MASTEDIR}/artificial-matrix-generator:$LD_LIBRARY_PATH"
BUILD_DIR_9=$MASTEDIR/Wrapper_cuSPARSE-9/${system}-build
BUILD_DIR_11=$MASTEDIR/Wrapper_cuSPARSE-11/${system}-build
BUILD_DIR_CSR5=$MASTEDIR/CSR5_cuda/PETROS_wrap_operator/${system}-build

STORE_TIMER_DIR=$MASTEDIR/Benchmarks/${system}

SCRIPTDIR=${MASTEDIR}/${system}-scripts
mkdir -p ${SCRIPTDIR}/build_runtrash
cd ${SCRIPTDIR}/build_runtrash

#INPUTDIR=/various/pmpakos/artificial_matrix_generation/matrix_generation_parameters/double/small
INPUTDIR=${MASTEDIR}/matrix_generation_parameters/${system}
for rep in 1 # 2 3 4 5
do
	for dtype in D #S
	do
		for filename in `cat ${SCRIPTDIR}/dataset_part1.in`;
		do
			cat $INPUTDIR/$filename | while read line
			do
				#printf "$line "
				# also available: $BUILD_DIR_9/cuSPARSEXbsrmv_9-2_mtx $BUILD_DIR_9/cuSPARSE${dtype}csrmv_9-2_generate
				for exec in $BUILD_DIR_9/cuSPARSE${dtype}hybmv_9-2_generate $BUILD_DIR_11/cuSPARSE${dtype}csrmv_11-0_generate $BUILD_DIR_11/cuSPARSE${dtype}coomv_11-0_generate $BUILD_DIR_CSR5/CSR5_CUDA_${dtype}SPMV_9-2_generate 
				do
					$exec $STORE_TIMER_DIR/${system}_dtype-${dtype}_run_dataset_part1.csv $line &>>$STORE_TIMER_DIR/${system}_dtype-${dtype}_run_dataset_part1.err
				done
			done
		done
	done
done

