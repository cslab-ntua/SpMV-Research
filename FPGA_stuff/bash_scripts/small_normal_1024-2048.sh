time timeout 420 ./build_dir.hw.xilinx_u280_xdma_201920_3/host.exe ./build_dir.hw.xilinx_u280_xdma_201920_3/spmv.xclbin ./sig_dat ./vec_dat 
 128 0; yes | xbutil reset -d 0; echo

rm -rf ./sig_dat/*; rm -rf ./vec_dat/*
