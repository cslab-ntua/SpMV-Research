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

INPUTDIR=${MASTEDIR}/matrix_generation_parameters/${system}
for dtype in D #S
do
	for f in `cat ${SCRIPTDIR}/validation_matrices.in`;
	do
		fname=${f##*/}
		# also available:
		for exec in $BUILD_DIR_11/cuSPARSE${dtype}csrmv_11-0_mtx $BUILD_DIR_11/cuSPARSE${dtype}coomv_11-0_mtx
		do
			for rep in 1 2 3 4 5
			do
				$exec $STORE_TIMER_DIR/${system}_dtype-${dtype}_run_validation_matrices.csv $f  &>>$STORE_TIMER_DIR/${system}_dtype-${dtype}_run_validation_matrices.err
			done
		done
	done
done

