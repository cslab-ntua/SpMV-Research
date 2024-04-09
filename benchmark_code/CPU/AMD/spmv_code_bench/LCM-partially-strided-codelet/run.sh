#!/bin/bash

shopt -s -o pipefail
shopt -s extglob globstar dotglob nullglob 2>/dev/null
export GLOBIGNORE=.:..


# max_cores=12
max_cores=128


# cores=1
cores=6
# cores=64
# cores=40

export OMP_DYNAMIC='false'
export OMP_NUM_THREADS="${cores}"
export MKL_NUM_THREADS="${cores}"

export GOMP_CPU_AFFINITY="0-$((${max_cores}-1))"


path_tamu="${HOME}/Data/graphs/tamu"


matrices_paper_csr_rv=(

    ts-palko
    neos
    stat96v3
    stormG2_1000
    xenon2
    s3dkq4m2
    apache2
    Si34H36
    ecology2
    LargeRegFile
    largebasis
    Goodwin_127
    Hamrle3
    boneS01
    sls
    cont1_l
    CO
    G3_circuit
    degme
    atmosmodl
    SiO2
    tp-6
    af_shell3
    circuit5M_dc
    rajat31
    CurlCurl_4
    cage14
    nlpkkt80
    ss
    boneS10

)
for ((i=0;i<${#matrices_paper_csr_rv[@]};i++)); do
    m="${matrices_paper_csr_rv[i]}"
    matrices_paper_csr_rv[i]="${path_tamu}/matrices/${m}/${m}.mtx"
done


matrices_compression_small=(

    cop20k_A
    # ASIC_680k
    # radiation
    # PR02R
    # crankseg_2
    # rajat31
    # human_gene1
    # dgreen

)
for ((i=0;i<${#matrices_compression_small[@]};i++)); do
    m="${matrices_compression_small[i]}"
    matrices_compression_small[i]="${path_tamu}/matrices/${m}/${m}.mtx"
done


matrices_compression=(
    
    spal_004
    # ldoor
    # dielFilterV2real
    # nv2
    # af_shell10
    # boneS10
    # circuit5M
    # Hook_1498
    # Geo_1438
    # Serena
    # vas_stokes_2M
    # bone010
    # audikw_1
    # Long_Coup_dt0
    # Long_Coup_dt6
    # dielFilterV3real
    # nlpkkt120
    # cage15
    # ML_Geer
    # Flan_1565
    # Cube_Coup_dt0
    # Cube_Coup_dt6
    # Bump_2911
    # vas_stokes_4M
    # nlpkkt160
    # HV15R
    # Queen_4147
    # stokes
    # nlpkkt200

)
for ((i=0;i<${#matrices_compression[@]};i++)); do
    m="${matrices_compression[i]}"
    matrices_compression[i]="${path_tamu}/matrices/${m}/${m}.mtx"
done



matrices=(
    # "${matrices_paper_csr_rv[@]}"
    "${matrices_compression_small[@]}"
    # "${matrices_compression[@]}"
)


./spmv_lcm_d.exe

for m in ${matrices[@]}; do
    numactl -i '0' ./spmv_lcm_d.exe "$m"
    # numactl -i '0-3' ./spmv_lcm_d.exe -m "$m" -n SPMV -s CSR
    # ./spmv_lcm_d.exe -m "$m" -n SPMV -s CSR
done

