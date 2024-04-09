#!/bin/sh

#SBATCH --cpus-per-task=40
#SBATCH --export=ALL
#SBATCH --job-name="TRSV"
#SBATCH --mail-type=begin  # email me when the job starts
#SBATCH --mail-type=end    # email me when the job finishes
#SBATCH --mail-user=zacharycetinic@gmail.com
#SBATCH --nodes=1
#SBATCH --output="DDT.%j.%N.out"
#SBATCH -t 12:00:00

module load NiaEnv/2019b
module load cmake/3.17.3
#module load intel
#module load intel/2019u4
#module load intel
module load gcc
module load metis/5.1.0

#export MKLROOT=/scinet/intel/2019u4/compilers_and_libraries_2019.4.243/linux/mkl
#export SUITEROOT=/home/m/mmehride/kazem/programs/SuiteSparse/
#export METISROOT=/scinet/niagara/software/2019b/opt/intel-2019u4/metis/5.1.0/

make clean
#cmake .. -DCMAKE_CXX_FLAGS="-O2 -march=native"
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j 20


## Paths for comet
BINPATH=/scratch/m/mmehride/kazem/development/DDT/build/demo/
UFDB=/scratch/m/mmehride/kazem/UFDB/
LOGS=/scratch/m/mmehride/kazem/development/DDT/build/logs/
SCRIPTPATH=/scratch/m/mmehride/kazem/development/DDT/scripts/


# bash $SCRIPTPATH/run_exp.sh $BINPATH/sptrsv_demo $UFDB 2 20 > $LOGS/sptrsv.csv
bash $SCRIPTPATH/run_exp.sh $BINPATH/spmv_demo $UFDB 3 20 > $LOGS/spmv.csv
