#!/bin/bash


benchmark_results_dir='../'
val_mat_dir='./'

data=(

    "hawk_csr_custom_BV_d.out           amd-epyc/amd-epyc_validation_matrices                           Custom_CSR_BV_CPU       HawkAmdRome     64,128"
    "hawk_csr_custom_B_d.out            amd-epyc/amd-epyc_validation_matrices                           Custom_CSR_B_CPU        HawkAmdRome     64,128"
    "hawk_mkl_ie_d.out                  amd-epyc/amd-epyc_validation_matrices                           MKL_IE_no_optimize      HawkAmdRome     64,128"
    "hawk_mkl_ie_opt_no_hint_d.out      amd-epyc/amd-epyc_validation_matrices                           MKL_IE_no_hint          HawkAmdRome     64,128"

    "hawk_csr5_d.out                    amd-epyc/amd-epyc_validation_matrices                           CSR5                    HawkAmdRome     64,128"
    "hawk_csr_naive_d.out               amd-epyc/amd-epyc_validation_matrices                           Naive_CSR_CPU           HawkAmdRome     64,128"
    "hawk_mkl_ie_opt_d.out              amd-epyc/amd-epyc_validation_matrices                           MKL_IE                  HawkAmdRome     64,128"

    "arm_csr_naive_d.out                arm/arm_validation_matrices                                     Naive_CSR_CPU           Arm             80,160"
    "arm_library_d.out                  arm/arm_validation_matrices                                     ARM_library             Arm             80,160"
    "arm_sell-c-sigma_d.out             experimental/arm/arm_validation_matrices_sell_c_s               SELL-C-s                Arm             80"
    "arm_merge-160_d.out                experimental/arm/arm_validation_matrices_merge                  merge-spmv              Arm             160"
    "arm_sparsex_d.out                  experimental/arm/arm_validation_matrices_sparsex                SparseX                 Arm             80"
    
    "xeon-gold_validation_merge.csv     experimental/xeon-gold/xeon-gold_validation_matrices_merge      merge-spmv              XeonGold1       14"

    "epyc1_csr5_d.csv                   experimental/amd-epyc24/amd-epyc24_validation_matrices          CSR5                    Epyc1AmdRome    24"
    "epyc1_csr_naive_d.csv              experimental/amd-epyc24/amd-epyc24_validation_matrices          Naive_CSR_CPU           Epyc1AmdRome    24"
    "epyc1_mkl_ie_opt_d.csv             experimental/amd-epyc24/amd-epyc24_validation_matrices          MKL_IE                  Epyc1AmdRome    24"
    "epyc1_validation_merge.csv         experimental/amd-epyc24/amd-epyc24_validation_matrices          merge-spmv              Epyc1AmdRome    24"

)


declare -A dict_files_out
dict_files_out=()


for tuple in "${data[@]}"; do
    tuple=($tuple)
    file="${tuple[0]}"
    file_out_base="${tuple[1]}"
    IMPLEMENTATION="${tuple[2]}"
    DEVICE="${tuple[3]}"
    IFS_buf="$IFS"
    IFS=','
    THREADS=(${tuple[4]})
    IFS="$IFS_buf"
    for t in "${THREADS[@]}"; do
        out="$(./parse_validation.awk -v "THREADS=$t" -v "IMPLEMENTATION=$IMPLEMENTATION" -v "DEVICE=$DEVICE" "${val_mat_dir}/${file}")"
        file_out="${benchmark_results_dir}/${file_out_base}_t${t}_d.csv"
        dict_files_out[${file_out}]="${out}\n${dict_files_out[${file_out}]}"
        # echo ./parse_validation.awk -v "THREADS=$t" -v "IMPLEMENTATION=$IMPLEMENTATION" -v "DEVICE=$DEVICE" "${val_mat_dir}/${file}"
        # echo "$out"
    done
done


for file_out in "${!dict_files_out[@]}"; do
        printf "${dict_files_out[$file_out]}" > "$file_out"
        # echo "$file_out"
        # echo "${dict_files_out[$file_out]}"
        # echo
done

