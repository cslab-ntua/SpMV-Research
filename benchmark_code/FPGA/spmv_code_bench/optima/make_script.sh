# export TARGET=Emulation-SW
export TARGET=Hardware
export CU=10
export PROJECT_NAME=optima_spmv

export PREFIX=/various/common_tools/xilinx_stx
if [[ `id -u` == "0" ]]; then
	export PREFIX=/opt
fi

export XILINX_XRT=/opt/xilinx/xrt
export XILINX_VIVADO=${PREFIX}/Vivado/2020.2/
export XILINX_VITIS=${PREFIX}/Vitis/2020.2/
export XILINX_VITIS_HLS=${PREFIX}/Vitis_HLS/2020.2/
export VITIS_PLATFORM=xilinx_u280_xdma_201920_3

echo -e "Welcome to build of project : ${PROJECT_NAME}, for platform ${VITIS_PLATFORM} with ${CU} Compute Units and with active build configuration : ${TARGET}"

rm -rf *summary *.csv
cd ${PROJECT_NAME}/$TARGET
echo -e "\n-------------------------------------"
echo -e "-------------------------------------"
echo "Building host side"
make clean PROJECT_NAME=${PROJECT_NAME}
make all -j20 CU=${CU} PROJECT_NAME=${PROJECT_NAME} XILINX_VITIS=${XILINX_VITIS} XILINX_XRT=${XILINX_XRT} XILINX_VIVADO=${XILINX_VIVADO} XILINX_VITIS_HLS=${XILINX_VITIS_HLS} VITIS_PLATFORM=${VITIS_PLATFORM}

cd ../../${PROJECT_NAME}_kernels/$TARGET
echo -e "\n-------------------------------------"
echo -e "-------------------------------------"
echo "Building fpga side (hw function)"
make clean PROJECT_NAME=${PROJECT_NAME}
make all PROJECT_NAME=${PROJECT_NAME} XILINX_VITIS=${XILINX_VITIS} XILINX_XRT=${XILINX_XRT} XILINX_VIVADO=${XILINX_VIVADO} XILINX_VITIS_HLS=${XILINX_VITIS_HLS} VITIS_PLATFORM=${VITIS_PLATFORM}

cd ../../${PROJECT_NAME}_system_hw_link/$TARGET
echo -e "\n-------------------------------------"
echo -e "-------------------------------------"
echo "Building hw link"
make clean PROJECT_NAME=${PROJECT_NAME}
make all CU=${CU} PROJECT_NAME=${PROJECT_NAME} XILINX_VITIS=${XILINX_VITIS} XILINX_XRT=${XILINX_XRT} XILINX_VIVADO=${XILINX_VIVADO} XILINX_VITIS_HLS=${XILINX_VITIS_HLS} VITIS_PLATFORM=${VITIS_PLATFORM}

cd ../../${PROJECT_NAME}_system/$TARGET
echo -e "\n-------------------------------------"
echo -e "-------------------------------------"
echo "Building final wrap-up of system"
make clean PROJECT_NAME=${PROJECT_NAME}
make all CU=${CU} PROJECT_NAME=${PROJECT_NAME} XILINX_VITIS=${XILINX_VITIS} XILINX_XRT=${XILINX_XRT} XILINX_VIVADO=${XILINX_VIVADO} XILINX_VITIS_HLS=${XILINX_VITIS_HLS} VITIS_PLATFORM=${VITIS_PLATFORM}

cd ../../
if [[ `id -u` == "0" ]]; then
	echo -e "\n-------------------------------------"
	echo -e "-------------------------------------"
	echo "Changing ownership"
	chown -R 10294:10000 ./${PROJECT_NAME}
	chown -R 10294:10000 ./${PROJECT_NAME}_kernels
	time chown -R 10294:10000 ./${PROJECT_NAME}_system_hw_link
	chown -R 10294:10000 ./${PROJECT_NAME}_system
fi
echo -e "\n-------------------------------------"
echo -e "-------------------------------------"
echo -e "FINISHED"
