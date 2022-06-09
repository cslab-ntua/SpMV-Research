#!/bin/bash

source config.sh

# Path for the matrix generator
path_generator='../../../artificial-matrix-generator'
cd $path_generator
make -j
cd -

mkdir -p ./spmv_code_cusparse-11.x/${system}-build
cp ./spmv_code_cusparse-11.x/CMakeLists.txt ./spmv_code_cusparse-11.x/${system}-build/CMakeLists.txt
cd ./spmv_code_cusparse-11.x/${system}-build
$cmake_command ./
make -j
cd -

if ((run_cuda_9)); then
	mkdir -p ./spmv_code_cusparse-9.x/${system}-build
	cp ./spmv_code_cusparse-9.x/CMakeLists.txt ./spmv_code_cusparse-9.x/${system}-build/CMakeLists.txt
	cd ./spmv_code_cusparse-9.x/${system}-build
	$cmake_command ./
	make -j
	cd -
	
	mkdir -p ./spmv_code_csr5_cuda/integrated_csr5_wrap_operator/${system}-build
	cp ./spmv_code_csr5_cuda/integrated_csr5_wrap_operator/CMakeLists.txt ./spmv_code_csr5_cuda/integrated_csr5_wrap_operator/${system}-build/CMakeLists.txt
	cd ./spmv_code_csr5_cuda/integrated_csr5_wrap_operator/${system}-build
	$cmake_command ./
	make -j
	cd -
fi


# Artificial matrix generator .so file used in Cmake requires linking. 
export LD_LIBRARY_PATH="../../../../artificial-matrix-generator:$LD_LIBRARY_PATH"

matrices_validation=(
    "$path_validation"/scircuit.mtx
    "$path_validation"/mac_econ_fwd500.mtx
    "$path_validation"/raefsky3.mtx
    "$path_validation"/bbmat.mtx
    "$path_validation"/conf5_4-8x8-15.mtx
    "$path_validation"/mc2depi.mtx
    "$path_validation"/rma10.mtx
    "$path_validation"/cop20k_A.mtx
    "$path_validation"/webbase-1M.mtx
    "$path_validation"/cant.mtx
    "$path_validation"/pdb1HYS.mtx
    "$path_validation"/TSOPF_RS_b300_c3.mtx
    "$path_validation"/Chebyshev4.mtx
    "$path_validation"/consph.mtx
    "$path_validation"/shipsec1.mtx
    "$path_validation"/PR02R.mtx
    "$path_validation"/mip1.mtx
    "$path_validation"/rail4284.mtx
    "$path_validation"/pwtk.mtx
    "$path_validation"/crankseg_2.mtx
    "$path_validation"/Si41Ge41H72.mtx
    "$path_validation"/TSOPF_RS_b2383.mtx
    "$path_validation"/in-2004.mtx
    "$path_validation"/Ga41As41H72.mtx
    "$path_validation"/eu-2005.mtx
    "$path_validation"/wikipedia-20051105.mtx
    "$path_validation"/rajat31.mtx
    "$path_validation"/ldoor.mtx
    "$path_validation"/circuit5M.mtx
    "$path_validation"/bone010.mtx
    "$path_validation"/cage15.mtx
)


#The directory where the benchmark logs will be stored
store_log_dir=../../../../benchmark_results/${system}
mkdir -p $store_log_dir
if ((!use_artificial_matrices)); then
	store_log_file=$store_log_dir/${system}_dtype-${dtype}_run_validation_matrices.csv
else 
	store_log_file=$store_log_dir/${system}_dtype-${dtype}_run_${filename_artificial}.csv
fi

bench()
{
    declare args=("$@")
    declare prog="${args[0]}"
    declare prog_args=("${args[@]:1}")
	echo "$prog ${store_log_file} ${prog_args[@]}"
    "$prog" "${store_log_file}" "${prog_args[@]}"
}

matrices=(
    "${matrices_validation[@]}"
)


if ((!use_artificial_matrices)); then
    prog_args=("${matrices[@]}")
else
    prog_args=()
    tmp=()
    for f in "${artificial_matrices_files[@]}"; do
        IFS=$'\n' read -d '' -a tmp < "$f"
        prog_args+=("${tmp[@]}")
    done
    echo "number of matrices: ${#prog_args[@]}"
fi

#Make a temporary dir for file outputs for the nvem power measurment logs (not used, can be deleted afterwards unless needed for debugging/checking)
mkdir -p ./build_runtrash
cd ./build_runtrash

for p in "${progs[@]}"; do
    # declare base file_out file_err
    # base="${p/*\//}"
    # base="${base%%.*}"
    # file_out="out_${base}.out"
    # file_err="out_${base}.err"
    # > "$file_out"
    # > "$file_err"
    # exec 1>>"$file_out"
    # exec 2>>"$file_err"
    echo "program: $p"
    for a in "${prog_args[@]}"
    do
        if ((use_artificial_matrices)); then
            echo "Matrix: $a"
            bench $p $a
        else
            echo "File: $a"
            bench $p "$a"
        fi
    done
done
