#!/bin/bash


benchmark_results_dir='./'
val_mat_dir='./'

data=(

    "./epyc1/t24/aocl_optmv_d.csv      ./amd-epyc1_validation_matrices      AOCL_OPTMV         Epyc1     24"
    "./epyc1/t24/csr5_d.csv            ./amd-epyc1_validation_matrices      CSR5               Epyc1     24"
    "./epyc1/t24/csr_naive_d.csv       ./amd-epyc1_validation_matrices      Naive_CSR_CPU      Epyc1     24"
    "./epyc1/t24/csr_vector_x86_d.csv  ./amd-epyc1_validation_matrices      Custom_CSR_BV_x86  Epyc1     24"
    "./epyc1/t24/merge_d.csv           ./amd-epyc1_validation_matrices      MERGE              Epyc1     24"
    "./epyc1/t24/mkl_ie_d.csv          ./amd-epyc1_validation_matrices      MKL_IE             Epyc1     24"
    "./epyc1/t24/sell_C_s_d.csv        ./amd-epyc1_validation_matrices      SELL-32-1          Epyc1     24"
    "./epyc1/t24/sparsex_d.csv         ./amd-epyc1_validation_matrices      SparseX            Epyc1     24"

    "./xeon-gold/csr5_d.csv            ./xeon-gold_validation_matrices      CSR5               XeonGold     14"
    "./xeon-gold/csr_naive_d.csv       ./xeon-gold_validation_matrices      Naive_CSR_CPU      XeonGold     14"
    "./xeon-gold/csr_vector_x86_d.csv  ./xeon-gold_validation_matrices      Custom_CSR_BV_x86  XeonGold     14"
    "./xeon-gold/merge_d.csv           ./xeon-gold_validation_matrices      MERGE              XeonGold     14"
    "./xeon-gold/mkl_ie_d.csv          ./xeon-gold_validation_matrices      MKL_IE             XeonGold     14"
    "./xeon-gold/sell_C_s_d.csv        ./xeon-gold_validation_matrices      SELL-32-1          XeonGold     14"
    "./xeon-gold/sparsex_d.csv         ./xeon-gold_validation_matrices      SparseX            XeonGold     14"

    "./arm/armpl_d.csv           ./arm_validation_matrices      ARMPL          ARM     80"
    "./arm/csr_naive_d.csv       ./arm_validation_matrices      Naive_CSR_CPU  ARM     80"
    "./arm/merge_d.csv           ./arm_validation_matrices      MERGE          ARM     80"
    "./arm/sell_C_s_d.csv        ./arm_validation_matrices      SELL-32-1      ARM     80"
    "./arm/sparsex_d.csv         ./arm_validation_matrices      SparseX        ARM     80"

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

