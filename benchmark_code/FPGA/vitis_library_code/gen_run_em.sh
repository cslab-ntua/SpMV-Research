export RUN_FILE="run_em.sh"
i=0
for iter in 1 2 3 4 5;
do
	for MATR in $(ls ./sig_dat/);
	do
		echo $MATR
		echo 'export MATRIX="'$MATR'";(echo "--->  "$MATRIX"  <---"; time timeout 420 ./build_dir.hw.xilinx_u280_xdma_201920_3/host.exe ./build_dir.hw.xilinx_u280_xdma_201920_3/spmv.xclbin ./sig_dat ./vec_dat $MATRIX 128 0 ) 2>&1 | tee -a results_'${iter}'.txt; ' >> ${RUN_FILE}
		if [ $iter -eq 5 ]; 
		then
			echo 'export MATRIX="'$MATR'"; rm -rf ./sig_dat/$MATRIX; rm -rf ./vec_dat/$MATRIX;' >> ${RUN_FILE}
		fi
		((i+=1))
		n=$(($i%200))
		if [ $n -eq 0 ]; 
		then
			echo 'yes | xbutil reset -d 0; echo; ' >> ${RUN_FILE}
		fi
	done
done
