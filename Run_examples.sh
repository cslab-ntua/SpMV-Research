#!/bin/bash

# TODO: Define thse variables for your system
MASTEDIR=/home/users/panastas/PhD_stuff/SpMV-Research
BUILD_DIR_9=$MASTEDIR/hybrid-spmv-lib/silver1_build
BUILD_DIR_11=$MASTEDIR/Wrapper_cuSPARSE-11/silver1_build
BUILD_DIR_CSR5=$MASTEDIR/CSR5_cuda/PETROS_wrap_operator/silver1_build
STORE_POWER_DIR=$MASTEDIR/hybrid-spmv-lib/Power_measure
STORE_TIMER_DIR=$MASTEDIR/hybrid-spmv-lib/Benchmarks

cd build_runtrash

for dtype in D S
do
	for f in `cat /home/users/panastas/PhD_stuff/SpMV-Research/hybrid-spmv-lib/split_matrices.txt`;
	do
		fname=${f##*/}
		# also available: cuSPARSEXbsrmv_9-2_mtx  #$BUILD_DIR_9/cuSPARSE${dtype}csrmv_9-2_mtx $BUILD_DIR_9/cuSPARSE${dtype}hybmv_9-2_mtx $BUILD_DIR_11/cuSPARSE${dtype}csrmv_11-0_mtx $BUILD_DIR_11/cuSPARSE${dtype}coomv_11-0_mtx
		for exec in $BUILD_DIR_CSR5/CSR5_CUDA_${dtype}SPMV_9-2_mtx
		do
			for rep in 1 2 3 4 5
			do
				$exec $STORE_TIMER_DIR/silver1-TeslaV100_dtype-${dtype}_run_split_matrices.csv $f  &>>$STORE_TIMER_DIR/silver1-TeslaV100_dtype-${dtype}_run_split_matrices.err
			done
		done
	done
done

