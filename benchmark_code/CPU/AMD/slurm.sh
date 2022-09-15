#!/bin/bash
#SBATCH -A ExaF_prod22
#SBATCH -p m100_usr_prod
#SBATCH --time 24:00:00                 # format: HH:MM:SS
#SBATCH -N 1                            # 1 node
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=128
#SBATCH --mem=246000                    # memory per node out of 246000MB
#SBATCH --job-name=job
#SBATCH --output=job.out
#SBATCH --error=job.err



cd /m100/home/userexternal/dgalanop/Shared/benchmarks/SpMV/SpMV-Research/benchmark_code/CPU/AMD
> job.out
> job.err

module load xl
module load essl
module load gnu
module load openblas

cd spmv_code_bench
make clean; make -j
cd ../

./run.sh
# ./proc_run.sh


