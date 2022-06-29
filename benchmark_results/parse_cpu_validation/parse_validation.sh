#!/bin/bash


bench_dir='../'
val_mat_dir='./'

data=(

    # 'csr_custom_BV_d.out            Custom_CSR_BV_CPU'
    # 'csr_custom_B_d.out             Custom_CSR_B_CPU'
    # 'mkl_ie_d.out                   MKL_IE_no_optimize'
    # 'mkl_ie_opt_no_hint_d.out       MKL_IE_no_hint'

    # 'csr5_d.out                     CSR5'
    # 'csr_naive_d.out                Naive_CSR_CPU'
    # 'mkl_ie_opt_d.out               MKL_IE'

    # 'arm_csr_naive_d.out            Naive_CSR_CPU'
    # 'arm_library_d.out              ARM_library'
    # 'arm_sell-c-sigma_d.out              SELL-C-s'
    'arm_merge-160_d.out              merge-spmv          Arm'
    # 'arm_sparsex_d.out              SparseX             Arm'
    
    # 'amd-epyc24_validation_merge.csv merge-spmv Epyc1AmdRome'
    # 'xeon-gold_validation_merge.csv merge-spmv XeonGold1'

    # 'epyc1_csr5_d.csv                     CSR5'
    # 'epyc1_csr_naive_d.csv                Naive_CSR_CPU'
    # 'epyc1_mkl_ie_opt_d.csv               MKL_IE'

)


# THREADS='64 128'
# THREADS='80 160'
THREADS='160'

for t in $THREADS; do
    # file_out="${bench_dir}/hawk_validation_matrices_t${t}_d.csv"
    # file_out="${bench_dir}/arm_validation_matrices_t${t}_d.csv"
    # file_out="${bench_dir}/arm_validation_matrices_sell_c_s_t${t}_d.csv"
    file_out="${bench_dir}/arm_validation_matrices_merge-spmv_t${t}_d.csv"

    > "${file_out}"
    for tuple in "${data[@]}"; do
        tuple=($tuple)
        file="${tuple[0]}"
        IMPLEMENTATION="${tuple[1]}"
        DEVICE="${tuple[2]}"
        ./parse_validation.awk -v "THREADS=$t" -v "IMPLEMENTATION=$IMPLEMENTATION" -v "DEVICE=$DEVICE" "${val_mat_dir}/${file}" >> "${file_out}"
    done
done
