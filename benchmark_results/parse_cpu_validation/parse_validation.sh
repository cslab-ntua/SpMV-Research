#!/bin/bash


benchmark_results_dir='./'
val_mat_dir='./'

data=(

    # "./epyc1/aocl_optmv_d.csv      ./amd-epyc1_validation_matrices      AOCL_OPTMV         Epyc1     24"
    # "./epyc1/csr5_d.csv            ./amd-epyc1_validation_matrices      CSR5               Epyc1     24"
    # "./epyc1/csr_naive_d.csv       ./amd-epyc1_validation_matrices      Naive_CSR_CPU      Epyc1     24"
    # "./epyc1/csr_x86_vector_d.csv  ./amd-epyc1_validation_matrices      Custom_CSR_BV_x86  Epyc1     24"
    # "./epyc1/merge_d.csv           ./amd-epyc1_validation_matrices      MERGE              Epyc1     24"
    # "./epyc1/mkl_ie_d.csv          ./amd-epyc1_validation_matrices      MKL_IE             Epyc1     24"
    # "./epyc1/sell_C_s_d.csv        ./amd-epyc1_validation_matrices      SELL-32-1          Epyc1     24"
    # "./epyc1/sparsex_d.csv         ./amd-epyc1_validation_matrices      SparseX            Epyc1     24"

    # "./intel-gold2/csr5_d.csv            ./intel-gold2_validation_matrices      CSR5               XeonGold     14"
    # "./intel-gold2/csr_naive_d.csv       ./intel-gold2_validation_matrices      Naive_CSR_CPU      XeonGold     14"
    # "./intel-gold2/csr_vector_x86_d.csv  ./intel-gold2_validation_matrices      Custom_CSR_BV_x86  XeonGold     14"
    # "./intel-gold2/merge_d.csv           ./intel-gold2_validation_matrices      MERGE              XeonGold     14"
    # "./intel-gold2/mkl_ie_d.csv          ./intel-gold2_validation_matrices      MKL_IE             XeonGold     14"
    # "./intel-gold2/sell_C_s_d.csv        ./intel-gold2_validation_matrices      SELL-32-1          XeonGold     14"
    # "./intel-gold2/sparsex_d.csv         ./intel-gold2_validation_matrices      SparseX            XeonGold     14"

    # "./arm/armpl_d.csv           ./arm_validation_matrices      ARMPL          ARM     80"
    # "./arm/csr_naive_d.csv       ./arm_validation_matrices      Naive_CSR_CPU  ARM     80"
    # "./arm/merge_d.csv           ./arm_validation_matrices      MERGE          ARM     80"
    # "./arm/sell_C_s_d.csv        ./arm_validation_matrices      SELL-32-1      ARM     80"
    # "./arm/sparsex_d.csv         ./arm_validation_matrices      SparseX        ARM     80"

    # "./power9/csr_d.csv             ./power9-m100_validation_matrices      Custom_CSR_B       Power9     32"
    # "./power9/csr_naive_d.csv       ./power9-m100_validation_matrices      Naive_CSR_CPU      Power9     32"
    # "./power9/merge_d.csv           ./power9-m100_validation_matrices      MERGE              Power9     32"
    # "./power9/sparsex_d.csv         ./power9-m100_validation_matrices      SparseX            Power9     32"
    # "./power9/xlc_csr_d.csv           ./power9-m100_validation_matrices      Custom_CSR_B       Power9     32"
    # "./power9/xlc_csr_naive_d.csv     ./power9-m100_validation_matrices      Naive_CSR_CPU      Power9     32"
    # "./power9/csr_vector_d.csv      ./power9-m100_validation_matrices      Custom_CSR_BV      Power9     32"
    
    # "./epyc5-A100/csr5_cuda_nv_d.csv                                ./epyc5-A100_validation_matrices CSR5_CUDA      A100 24" 
    # "./epyc5-A100/csr_cuda_const_nnz_per_thread_b1024_nnz4_nv_d.csv ./epyc5-A100_validation_matrices Custom_CSR_CUDA_constant_nnz_per_thread_b1024_nnz4 A100 24" 
    # "./epyc5-A100/cusparse_csr_nv_d.csv                             ./epyc5-A100_validation_matrices CUSPARSE_CSR    A100 24" 
    # "./epyc5-A100/cusparse_coo_nv_d.csv                             ./epyc5-A100_validation_matrices CUSPARSE_COO    A100 24" 
    # "./epyc5-A100/dasp_cuda_nv_d.csv                                ./epyc5-A100_validation_matrices DASP_CUDA      A100 24" 

    # "./intel-icy3/intermediate/validation/csr5_d.csv                             ./intel-icy3_validation_matrices CSR5           Icy 16"
    # "./intel-icy3/intermediate/validation/csr_naive_d.csv                        ./intel-icy3_validation_matrices Naive_CSR_CPU  Icy 16"
    # "./intel-icy3/intermediate/validation/csr_vector_d.csv                       ./intel-icy3_validation_matrices Custom_CSR_BV  Icy 16"
    # "./intel-icy3/intermediate/validation/csr_vector_perfect_nnz_balance_d.csv   ./intel-icy3_validation_matrices Custom_CSR_PBV Icy 16"
    # "./intel-icy3/intermediate/validation/merge_d.csv                            ./intel-icy3_validation_matrices MERGE          Icy 16"
    # "./intel-icy3/intermediate/validation/mkl_ie_d.csv                           ./intel-icy3_validation_matrices MKL_IE         Icy 16"
    # "./intel-icy3/intermediate/validation/sell_C_s_d.csv                         ./intel-icy3_validation_matrices SELL-32-1      Icy 16"
    # "./intel-icy3/intermediate/validation/sparsex_d.csv                          ./intel-icy3_validation_matrices SparseX        Icy 16"

    # "./amd-epyc7763/aocl_optmv_d.csv                                ./amd-epyc7763_validation_matrices AOCL_OPTMV Epyc64 64"
    # "./amd-epyc7763/csr5_d.csv                                      ./amd-epyc7763_validation_matrices CSR5 Epyc64 64"
    # "./amd-epyc7763/csr_d.csv                                       ./amd-epyc7763_validation_matrices Custom_CSR_B Epyc64 64"
    # "./amd-epyc7763/csr_naive_d.csv                                 ./amd-epyc7763_validation_matrices Naive_CSR_CPU Epyc64 64"
    # "./amd-epyc7763/csr_vector_x86_d.csv                            ./amd-epyc7763_validation_matrices Custom_CSR_BV_x86 Epyc64 64"
    # "./amd-epyc7763/merge_d.csv                                     ./amd-epyc7763_validation_matrices MERGE Epyc64 64"
    # "./amd-epyc7763/mkl_ie_d.csv                                    ./amd-epyc7763_validation_matrices MKL_IE Epyc64 64"
    # "./amd-epyc7763/sell_C_s_d.csv                                  ./amd-epyc7763_validation_matrices SELL-32-1 Epyc64 64"
    # "./amd-epyc7763/sparsex_d.csv                                   ./amd-epyc7763_validation_matrices SparseX Epyc64 64"

    # "./amd-mi250/csr_rocm_acc_flat_b512_nv_d.csv                    ./amd-mi250_validation_matrices ACC_FLAT_b512 MI250 64"
    # "./amd-mi250/csr_rocm_acc_line_enhance_b512_nv_d.csv            ./amd-mi250_validation_matrices ACC_LINE_ENHANCE_b512 MI250 64"
    # "./amd-mi250/csr_rocm_adaptive_b512_mb1_nv_d.csv                ./amd-mi250_validation_matrices Custom_CSR_CUDA_ADAPTIVE_b512_1 MI250 64"
    # "./amd-mi250/csr_rocm_const_nnz_per_thread_b512_nnz4_nv_d.csv   ./amd-mi250_validation_matrices Custom_CSR_ROCM_constant_nnz_per_thread_b512_nnz4 MI250 64"
    # "./amd-mi250/csr_rocm_vector_b512_nv_d.csv                      ./amd-mi250_validation_matrices Custom_CSR_ROCM_VECTOR_b512 MI250 64"
    # "./amd-mi250/rocsparse_coo_nv_d.csv                             ./amd-mi250_validation_matrices ROCSPARSE_COO MI250 64"
    # "./amd-mi250/rocsparse_csr_nv_d.csv                             ./amd-mi250_validation_matrices ROCSPARSE_CSR MI250 64"
    # "./amd-mi250/rocsparse_hyb_nv_d.csv                             ./amd-mi250_validation_matrices ROCSPARSE_HYB MI250 64"

    # "./grace1-arm/armpl_d.csv                                       ./grace1-arm_validation_matrices ARMPL ARMGrace 72"
    # "./grace1-arm/csr_d.csv                                         ./grace1-arm_validation_matrices Custom_CSR_B ARMGrace 72"
    # "./grace1-arm/csr_naive_d.csv                                   ./grace1-arm_validation_matrices Naive_CSR_CPU ARMGrace 72"
    # "./grace1-arm/csr_vector_sve_d.csv                              ./grace1-arm_validation_matrices Custom_CSR_BV_SVE ARMGrace 72"
    # "./grace1-arm/merge_d.csv                                       ./grace1-arm_validation_matrices MERGE ARMGrace 72"
    # "./grace1-arm/sparsex_d.csv                                     ./grace1-arm_validation_matrices SparseX ARMGrace 72"

    # "./grace1-H100/csr5_cuda_nv_d.csv                                ./grace1-H100_validation_matrices CSR5_CUDA H100 72"
    # "./grace1-H100/csr_cuda_adaptive_b256_mb1_nv_d.csv               ./grace1-H100_validation_matrices Custom_CSR_CUDA_ADAPTIVE_b256_1 H100 72"
    # "./grace1-H100/csr_cuda_const_nnz_per_thread_b1024_nnz4_nv_d.csv ./grace1-H100_validation_matrices Custom_CSR_CUDA_constant_nnz_per_thread_b1024_nnz4 H100 72"
    # "./grace1-H100/csr_cuda_vector_b256_nv_d.csv                     ./grace1-H100_validation_matrices Custom_CSR_CUDA_VECTOR_b256 H100 72"
    # "./grace1-H100/cusparse_coo_nv_d.csv                             ./grace1-H100_validation_matrices CUSPARSE_COO H100 72"
    # "./grace1-H100/cusparse_csr_nv_d.csv                             ./grace1-H100_validation_matrices CUSPARSE_CSR H100 72"
    # "./grace1-H100/dasp_cuda_nv_d.csv                                ./grace1-H100_validation_matrices DASP_CUDA H100 72"
    # "./grace1-H100/merge_cuda_nv_d.csv                               ./grace1-H100_validation_matrices MERGE_CUDA H100 72"

    "./intel-sapphire/aocl_optmv_d.csv                               ./intel-sapphire_validation_matrices AOCL_OPTMV IntelSapphire 56"
    "./intel-sapphire/csr5_d.csv                                     ./intel-sapphire_validation_matrices CSR5 IntelSapphire 56"
    "./intel-sapphire/csr_d.csv                                      ./intel-sapphire_validation_matrices Custom_CSR_B IntelSapphire 56"
    "./intel-sapphire/csr_naive_d.csv                                ./intel-sapphire_validation_matrices Naive_CSR_CPU IntelSapphire 56"
    "./intel-sapphire/csr_vector_x86_d.csv                           ./intel-sapphire_validation_matrices Custom_CSR_BV_x86 IntelSapphire 56"
    "./intel-sapphire/merge_d.csv                                    ./intel-sapphire_validation_matrices MERGE IntelSapphire 56"
    "./intel-sapphire/mkl_ie_d.csv                                   ./intel-sapphire_validation_matrices MKL_IE IntelSapphire 56"
    "./intel-sapphire/sell_C_s_d.csv                                 ./intel-sapphire_validation_matrices SELL-32-1 IntelSapphire 56"
    "./intel-sapphire/sparsex_d.csv                                  ./intel-sapphire_validation_matrices SparseX IntelSapphire 56"
    "./intel-sapphire/lcm_d.csv                                      ./intel-sapphire_validation_matrices LCM IntelSapphire 56"



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

