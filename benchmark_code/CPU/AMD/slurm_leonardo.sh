#!/bin/bash
#SBATCH --account=EUHPC_D12_058
#SBATCH --partition=dcgp_usr_prod
#SBATCH --exclusive
#SBATCH --time 10:00:00                 # format: HH:MM:SS
#SBATCH --nodes=1                       # 1 node
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=112
#SBATCH --mem=200000                    # memory per node out of 246000MB

#SBATCH --job-name=job_leonardo
#SBATCH --output=job_leonardo.out
#SBATCH --error=job_leonardo.err



cd /leonardo_scratch/fast/EUHPC_D12_058/pmpakos/SpMV-Research/benchmark_code/CPU/AMD
> job_leonardo.out
> job_leonardo.err

module load intel-oneapi-mkl/2023.2.0 gcc/12.2.0

cd spmv_code_bench
make -j
cd ../
time bash run.sh
