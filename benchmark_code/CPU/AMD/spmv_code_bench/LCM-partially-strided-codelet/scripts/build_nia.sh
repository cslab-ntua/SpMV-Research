#!/bin/sh

#SBATCH --cpus-per-task=40
#SBATCH --export=ALL
#SBATCH --job-name="sym"
#SBATCH --mail-type=begin  # email me when the job starts
#SBATCH --mail-type=end    # email me when the job finishes
#SBATCH --mail-user=kazem.cheshmi@gmail.com
#SBATCH --nodes=1
#SBATCH --output="fusy.%j.%N.out"
#SBATCH -t 23:00:00


module load NiaEnv/2019b
module load cmake/3.17.3
module load intel
module load intel/2019u3
module load intel
module load gcc
module load mkl
module load metis/5.1.0
module load gcc/9.4.0 

export MKLROOT=/scinet/intel/2019u3/compilers_and_libraries_2019/linux/mkl

THRDS=20
export OMP_NUM_THREADS=$THRDS
export MKL_NUM_THREADS=$THRDS


#make clean
#cmake .. -DCMAKE_CXX_FLAGS="-O2 -march=native"
cmake -DCMAKE_PREFIX_PATH="/scinet/intel/2019u3/compilers_and_libraries_2019/linux/mkl/lib/intel64/;/scinet/intel/2019u3/compilers_and_libraries_2019/linux/mkl/include/;"  -DCMAKE_BUILD_TYPE=Release ..
#make sptrsv_demo
#make spmv_spmv_ind_demo 
make -j 20


for f in $(find /scratch/m/mmehride/kazem/dlmc -name '*.smtx'); do 
  echo $f;
  /scratch/m/mmehride/kazem/development/DDT/build/demo/spmm_demo --matrix $f --numerical_operation SPMM --storage_format CSR --n_tile_size=32 --b_matrix_columns=32 -t $THRDS >> spmm_32.csv
#--matrix $f --numerical_operation SPMM --storage_format CSR --n_tile_size=64 --b_matrix_columns=64 
done


for f in $(find /scratch/m/mmehride/kazem/dlmc -name '*.smtx'); do 
  echo $f;
  /scratch/m/mmehride/kazem/development/DDT/build/demo/spmm_demo --matrix $f --numerical_operation SPMM --storage_format CSR --n_tile_size=64 --b_matrix_columns=64 -t $THRDS >> spmm_64.csv
#--matrix $f --numerical_operation SPMM --storage_format CSR --n_tile_size=64 --b_matrix_columns=64 
done



for f in $(find /scratch/m/mmehride/kazem/dlmc -name '*.smtx'); do 
  echo $f;
  /scratch/m/mmehride/kazem/development/DDT/build/demo/spmm_demo --matrix $f --numerical_operation SPMM --storage_format CSR --n_tile_size=128 --b_matrix_columns=128 -t $THRDS >> spmm_128.csv
#--matrix $f --numerical_operation SPMM --storage_format CSR --n_tile_size=64 --b_matrix_columns=64 
done
