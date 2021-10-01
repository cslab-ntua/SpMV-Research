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
		$BUILD_DIR_CSR5/CSR5_CUDA_DSPMV $f $STORE_TIMER_DIR/silver1-TeslaV100_double_run_split_matrices.csv &>>$STORE_TIMER_DIR/silver1-TeslaV100_double_run_split_matrices.err
		mv Power_data.txt $STORE_POWER_DIR/double_imp-CSR5_rep-${rep}_file-${fname}
		$BUILD_DIR_CSR5/CSR5_CUDA_SSPMV $f $STORE_TIMER_DIR/silver1-TeslaV100_float_run_split_matrices.csv &>>$STORE_TIMER_DIR/silver1-TeslaV100_double_run_split_matrices.err
		mv Power_data.txt $STORE_POWER_DIR/float_imp-CSR5_rep-${rep}_file-${fname}
		# also available: cuSPARSE_bsr
		for implementation in cuSPARSE_csr cuSPARSE_hyb
		do
			if [[ "$implementation" == *"cuSPARSE_bsr"* ]];then
				for blockdim in 2 4 16 64 #2 4 16 64
				do
					$BUILD_DIR/MakeDSpMVGreatAgain $f $STORE_TIMER_DIR/silver1-TeslaV100_double_run_split_matrices.csv $implementation $blockdim &>>$STORE_TIMER_DIR/silver1-TeslaV100_double_run_split_matrices.err
					mv Power_data.txt $STORE_POWER_DIR/double_imp-${implementation}_blockdim-${blockdim}_rep-${rep}_file-${fname}
					$BUILD_DIR/MakeSSpMVGreatAgain $f $STORE_TIMER_DIR/silver1-TeslaV100_float_run_split_matrices.csv $implementation $blockdim &>>$STORE_TIMER_DIR/silver1-TeslaV100_float_run_split_matrices.err
					mv Power_data.txt $STORE_POWER_DIR/float_imp-${implementation}_blockdim-${blockdim}_rep-${rep}_file-${fname}
				done
			else
				$BUILD_DIR/MakeDSpMVGreatAgain $f $STORE_TIMER_DIR/silver1-TeslaV100_double_run_split_matrices.csv $implementation &>>$STORE_TIMER_DIR/silver1-TeslaV100_double_run_split_matrices.err
				mv Power_data.txt $STORE_POWER_DIR/double_imp-${implementation}_rep-${rep}_file-${fname}
				$BUILD_DIR/MakeSSpMVGreatAgain $f $STORE_TIMER_DIR/silver1-TeslaV100_float_run_split_matrices.csv $implementation &>>$STORE_TIMER_DIR/silver1-TeslaV100_float_run_split_matrices.err
				mv Power_data.txt $STORE_POWER_DIR/float_imp-${implementation}_rep-${rep}_file-${fname}
			fi
		done
	done
done

