export ID=0; export RANGE="512-1024"; time python3 python_exp/gen_signature_synthetic.py --partition --mtx_param_list $REST_PREFIX/normal_$RANGE_$ID.txt --sig_path ./sig_dat --vec_path ./vec_dat 2>&1 | tee $REST_PREFIX/progress_$RANGE_$ID
cd $OLD_PREFIX/sig_dat ; du -a | cut -d/ -f2 | sort | uniq -c | sort -nr
cd $NEW_PREFIX/sig_dat ; du -a | cut -d/ -f2 | sort | uniq -c | sort -nr

export MATRIX="
";(echo "--->  "$MATRIX"  <---"; time timeout 420 ./build_dir.hw.xilinx_u280_xdma_201920_3/host.exe ./build_dir.hw.xilinx_u280_xdma_201920_3/spmv.xclbin ./sig_dat ./vec_dat $MATRIX 128 0 ) 2>&1 | tee -a tmp_log.txt; yes | xbutil reset -d 0; echo; rm -rf ./sig_dat/$MATRIX; rm -rf ./vec_dat/$MATRIX;


 export MATRIX="
"; mv $OLD_PREFIX/sig_dat/$MATRIX $NEW_PREFIX/sig_dat; mv $OLD_PREFIX/vec_dat/$MATRIX $NEW_PREFIX/vec_dat

