export ID=1; export RANGE="4-32"; time python3 python_exp/gen_signature_synthetic.py --partition --mtx_param_list ${PARAM_PREFIX}/synthetic_matrices_small_dataset_${RANGE}_part${ID}.txt --sig_path /mnt/local_matrices/sig_dat --vec_path /mnt/local_matrices/vec_dat 2>&1 | tee $PARAM_PREFIX/progress_$RANGE_$ID

cd $OLD_PREFIX/sig_dat ; du -a | cut -d/ -f2 | sort | uniq -c | sort -nr
cd $NEW_PREFIX/sig_dat ; du -a | cut -d/ -f2 | sort | uniq -c | sort -nr

export ID=1; cd /various/pmpakos/vitis-workspace/2/Vitis_Libraries/sparse/L2/tests/fp64/spmv/; screen -dmS gen_em${ID} bash gen_em${ID}.sh; screen -r gen_em${ID}

 export MATRIX="
"; mv $OLD_PREFIX/sig_dat/$MATRIX $NEW_PREFIX/sig_dat; mv $OLD_PREFIX/vec_dat/$MATRIX $NEW_PREFIX/vec_dat

cd /various/pmpakos/vitis-workspace/2/Vitis_Libraries/sparse/L2/benchmarks/spmv_double/; screen -dmS run_em bash run_em.sh; screen -r run_em

export MATRIX="
";(echo "--->  "$MATRIX"  <---"; time timeout 420 ${BUILD_PATH}/host.exe ${BUILD_PATH}/spmv.xclbin ${NEW_PREFIX}/sig_dat ${NEW_PREFIX}/vec_dat $MATRIX 128 0 ) 2>&1 | tee -a tmp_log.txt; yes | xbutil reset -d 0; echo; rm -rf ${NEW_PREFIX}/sig_dat/$MATRIX; rm -rf ${NEW_PREFIX}/vec_dat/$MATRIX;
