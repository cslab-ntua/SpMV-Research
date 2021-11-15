#!/bin/bash


bench_dir='../'
val_mat_dir="./"

data=(
    'csr5_d.out             CSR5_CPU'
    'csr_custom_BV_d.txt    Custom_CSR_BV_CPU'
    'csr_custom_B_d.txt     Custom_CSR_B_CPU'
    'csr_naive_d.out        Naive_CSR_CPU'
    'mkl_ie_d.out           MKL_IE'
    'mkl_ie_opt_d.out       MKL_IE_Optimize'
)


THREADS='64 128'

for t in $THREADS; do
    file_out="${bench_dir}/hawk_validation_matrices_t${t}_d.csv"
    > "${file_out}"
    for tuple in "${data[@]}"; do
        tuple=($tuple)
        file="${tuple[0]}"
        IMPLEMENTATION="${tuple[1]}"
        ./parse_validation.awk -v "THREADS=$t" -v "IMPLEMENTATION=$IMPLEMENTATION" "${val_mat_dir}/${file}" >> "${file_out}"
    done
done


