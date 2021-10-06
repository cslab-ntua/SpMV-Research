#!/bin/bash

# TODO: Define thse variables for your system
MASTEDIR=/home/users/panastas/PhD_stuff/SpMV-Research
BUILD_DIR=$MASTEDIR/hybrid-spmv-lib/silver1_build
BUILD_DIR_CSR5=$MASTEDIR/CSR5_cuda/PETROS_wrap/silver1_build
STORE_POWER_DIR=$MASTEDIR/hybrid-spmv-lib/Power_measure
STORE_TIMER_DIR=$MASTEDIR/hybrid-spmv-lib/silver1_build

for rep in 1 2 3 4 5
do
	for f in `cat /home/users/panastas/PhD_stuff/SpMV-Research/hybrid-spmv-lib/split_matrices.txt`;
	do
		fname=${f##*/}
		for dtype in D S
		do
			$BUILD_DIR_CSR5/CSR5_CUDA_${dtype}SPMV $f $STORE_TIMER_DIR/silver1-TeslaV100_dtype-${dtype}_run_split_matrices.csv &>>$STORE_TIMER_DIR/silver1-TeslaV100_dtype-${dtype}_run_split_matrices.err
			mv Power_data.txt $STORE_POWER_DIR/dtype-${dtype}_imp-CSR5_rep-${rep}_file-${fname}
			# also available (?): cuSPARSE_bsr
			for cuda in "9-2"
			do
				for implementation in csr hyb
				do
					if [[ "$implementation" == *"bsr"* ]];then
						for blockdim in 2 4 16 64 #2 4 16 64
						do
							exec=$BUILD_DIR/cuSPARSE${dtype}${implementation}mv_${cuda}_mtx
							$exec $STORE_TIMER_DIR/silver1-TeslaV100_dtype-${dtype}_run_split_matrices.csv $blockdim $f  &>>$STORE_TIMER_DIR/silver1-TeslaV100_dtype-${dtype}_run_split_matrices.err
							mv Power_data.txt $STORE_POWER_DIR/dtype-${dtype}_imp-${implementation}_blockdim-${blockdim}_rep-${rep}_file-${fname}
						done
					else
							exec=$BUILD_DIR/cuSPARSE${dtype}${implementation}mv_${cuda}_mtx
							$exec $STORE_TIMER_DIR/silver1-TeslaV100_dtype-${dtype}_run_split_matrices.csv $f  &>>$STORE_TIMER_DIR/silver1-TeslaV100_dtype-${dtype}_run_split_matrices.err
							mv Power_data.txt $STORE_POWER_DIR/dtype-${dtype}_imp-${implementation}_rep-${rep}_file-${fname}
					fi
				done
			done
		done
	done
done

