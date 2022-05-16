#!/bin/bash

source ${HOME}/.bashrc

system=epyc5A100

# TODO: Define these variables for your system
MASTEDIR=/home/panastas/PhD_stuff/SpMV-Research
export LD_LIBRARY_PATH="${MASTEDIR}/artificial-matrix-generator:$LD_LIBRARY_PATH"
GPU_MASTER_DIR=$MASTEDIR/benchmark_code/GPU
BUILD_DIR_9=$GPU_MASTER_DIR/Wrapper_cuSPARSE-9/${system}-build
BUILD_DIR_11=$GPU_MASTER_DIR/Wrapper_cuSPARSE-11/${system}-build
BUILD_DIR_CSR5=$GPU_MASTER_DIR/CSR5_cuda/PETROS_wrap_operator/${system}-build

STORE_TIMER_DIR=$MASTEDIR/benchmark_results/${system}

SCRIPTDIR=${GPU_MASTER_DIR}/${system}-scripts
mkdir -p ${SCRIPTDIR}/build_runtrash
cd ${SCRIPTDIR}/build_runtrash

INPUTDIR=${MASTEDIR}/matrix_generation_parameters
for rep in 1 # 2 3 4 5
do
	for dtype in D #S
	do
		for filename in `cat ${SCRIPTDIR}/dataset_friends_run_order.in`;
		do
			cat $INPUTDIR/$filename | while read line
			do
				#printf "$line "
				# also available:
				for exec in $BUILD_DIR_11/cuSPARSE${dtype}csrmv_11-0_generate $BUILD_DIR_11/cuSPARSE${dtype}coomv_11-0_generate $BUILD_DIR_CSR5/CSR5_CUDA_${dtype}SPMV_9-2_generate 
				do
					$exec $STORE_TIMER_DIR/${system}_dtype-${dtype}_run_friend_dataset.csv $line &>>$STORE_TIMER_DIR/${system}_dtype-${dtype}_run_friend_dataset.err
				done
			done
		done
	done
done

