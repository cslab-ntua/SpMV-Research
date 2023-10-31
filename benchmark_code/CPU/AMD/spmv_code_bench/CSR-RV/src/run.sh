#!/bin/bash

shopt -s -o pipefail
shopt -s extglob globstar dotglob nullglob 2>/dev/null
export GLOBIGNORE=.:..


max_cores=32

# cores=1
# cores=2
# cores=4
# cores=8
cores=16

export OMP_DYNAMIC='false'
export OMP_NUM_THREADS="${cores}"

export GOMP_CPU_AFFINITY="0-$((${max_cores}-1))"


# export LD_LIBRARY_PATH="${AOCL_PATH}/lib:${MKL_PATH}/lib/intel64:${LD_LIBRARY_PATH}"
# export LD_LIBRARY_PATH="${LD_LIBRARY_PATH}:${BOOST_LIB_PATH}:${LLVM_LIB_PATH}:${SPARSEX_LIB_PATH}"

export LD_LIBRARY_PATH="${LD_LIBRARY_PATH}:/various/dgal/gcc/gcc-12.2.0/gcc_bin/lib64"


path_tamu="${HOME}/Data/graphs/tamu"


matrices_paper_csr_rv=(

    # ts-palko
    # neos
    # stat96v3
    # stormG2_1000
    # xenon2
    # s3dkq4m2
    # apache2
    # Si34H36
    # ecology2
    # LargeRegFile
    # largebasis
    # Goodwin_127
    # Hamrle3
    # boneS01
    # sls
    # cont1_l
    # CO
    G3_circuit
    # degme
    # atmosmodl
    # SiO2
    # tp-6
    # af_shell3
    # circuit5M_dc
    # rajat31
    # CurlCurl_4
    # cage14
    # nlpkkt80
    # ss
    # boneS10

)
for ((i=0;i<${#matrices_paper_csr_rv[@]};i++)); do
    m="${matrices_paper_csr_rv[i]}"
    matrices_paper_csr_rv[i]="$path_tamu"/matrices/"$m"/"$m".mtx
done


matrices=(
    "${matrices_paper_csr_rv[@]}"
)


for m in "${matrices[@]}"; do
    ./csv_avx512.exe "$m"
done


