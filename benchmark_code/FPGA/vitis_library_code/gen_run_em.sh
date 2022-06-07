usage(){ echo "Usage : $0 -l <Xilinx Tools License Path> -v <Vitis Tools Path> -x <XRT Path>"; exit 1; }

if [ "$#" -ne 6 ]; then
    usage
fi

while getopts l:v:x: flag; do
    case "${flag}" in
        l) XILINX_LIC=${OPTARG};;
        v) VITIS_PATH=${OPTARG};;
        x) XRT_PATH=${OPTARG};;
 		:) # If expected argument omitted:
      		echo "Error: -${OPTARG} requires an argument."
      		usage;;
    	*) # If unknown (any other) option:
    		usage;;
    esac
done

export RUN_FILE="run_em.sh"
rm -rf ${RUN_FILE}
echo 'export LC_ALL=en_US.utf8' >> ${RUN_FILE}
echo 'export XILINXD_LICENSE_FILE='$XILINX_LIC >> ${RUN_FILE}
echo 'source '$VITIS_PATH'/settings64.sh' >> ${RUN_FILE}
echo 'source '$XRT_PATH'/setup.sh' >> ${RUN_FILE}

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
